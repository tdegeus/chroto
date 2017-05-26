#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #include "imageselection.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDirIterator>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QScrollBar>
#include <QListWidget>

#include <QThread>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <algorithm>

#include "cppcolormap/cppcolormap.h"
#include "easyexif/exif.h"
#include "json/json.hpp"

using json = nlohmann::json;

// =================================================================================================

class Thumbnails: public QObject
{
  Q_OBJECT

private:
  std::vector<QString> path;
  std::vector<QIcon>   data;
  std::vector<int>     isread;

public:
  QIcon& at ( size_t i )
  {
    return data[i];
  };

  size_t size()
  {
    return data.size();
  }

  void push_back(QString name)
  {
    QPixmap pix(50,50);
    pix.fill(QColor("white"));

    path.push_back(name);
    data.push_back(QIcon(pix));
    isread.push_back(0);
  };

  void pop_back(std::vector<int> index)
  {
    std::sort(index.begin(),index.end(),[](size_t i,size_t j){return i>j;});

    for ( auto &i: index ) {
      data.erase(data.begin()+i);
      path.erase(path.begin()+i);
      isread.erase(isread.begin()+i);
    }
  }

public slots:
  void read()
  {
    for ( size_t i=0; i<path.size(); ++i ) {
      if ( QThread::currentThread()->isInterruptionRequested() )
        return;
      if ( !isread[i] ) {
        QPixmap pix(path[i]);
        pix.scaled(50,50,Qt::KeepAspectRatio, Qt::FastTransformation);
        data[i] = QIcon(pix);
        isread[i] = 1;
      }
    }
  };
};

// =================================================================================================

class File
{
public:
  QString     dir       = ""   ; // directory in which the file is stored
  QString     path      = ""   ; // absolute file-path to the file
  QString     disp      = ""   ; // display name
  size_t      folder    = 0    ; // folder-index (corresponds to "fileView")
  size_t      camera    = 0    ; // camera-index (allows several cameras in one folder)
  int         rotation  = 0    ; // rotation in degrees
  std::time_t time      = 0    ; // time at which the photo was taken
  size_t      index     = 0    ; // for sorting: position in list -> locate where "idx" went
  bool        sort      = true ; // for sorting: selectively sort subset
  size_t      ithumb           ; // index in list with thumbnails

  File            (const File &) = default;
  File& operator= (const File &) = default;
  File(){};
};

// =================================================================================================

// read absolute time + rotation from JPEG with specified file-name "fname"
inline std::tuple<std::time_t,int> jpg2info (std::string fname)
{
  // read JPEG-file into a buffer
  FILE *fp = std::fopen(fname.c_str(),"rb");
  if (!fp) {
    throw std::runtime_error("File cannot be opened");
  }
  fseek(fp, 0, SEEK_END);
  unsigned long fsize = ftell(fp);
  rewind(fp);
  unsigned char *buf = new unsigned char[fsize];
  if (fread(buf, 1, fsize, fp) != fsize) {
    delete[] buf;
    throw std::runtime_error("File cannot be read");
  }
  fclose(fp);

  // parse EXIF
  easyexif::EXIFInfo result;
  int code = result.parseFrom(buf, fsize);
  delete[] buf;
  if ( code ) {
    throw std::runtime_error("Error parsing file");
  }

  // interpret time string
  struct std::tm tm;
  std::istringstream iss;
  if      ( result.DateTime         .size()==19 ) { iss.str(result.DateTime         ); }
  else if ( result.DateTimeOriginal .size()==19 ) { iss.str(result.DateTimeOriginal ); }
  else if ( result.DateTimeDigitized.size()==19 ) { iss.str(result.DateTimeDigitized); }
  else    { throw std::runtime_error("No time string found"); }
  iss >> std::get_time(&tm,"%Y:%m:%d %H:%M:%S");
  std::time_t t = mktime(&tm);

  // convert orientation to rotation
  int rot = 0;
  if      ( result.Orientation==8 ) rot = -90;
  else if ( result.Orientation==6 ) rot =  90;
  else if ( result.Orientation==3 ) rot = 180;

  return std::make_tuple(t,rot);
};

// =================================================================================================

inline std::vector<size_t> selectedItems(QListWidget* list, bool ascending=true)
{
  // allocate
  std::vector<size_t> out;

  // fill
  foreach ( QListWidgetItem *item, list->selectedItems() ) {
    int i = list->row(item);
    if ( i>=0 )
      out.push_back(static_cast<size_t>(i));
  }

  // sort from low to high / high to low
  if ( ascending )
    std::sort(out.begin(),out.end());
  else
    std::sort(out.begin(),out.end(),[](size_t i,size_t j){return i>j;});

  // return list
  return out;
};

// =================================================================================================

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  QThread workerThread;

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_nfolder_comboBox_currentIndexChanged(int index);// change the displayed number of folders
  void selectFolder(size_t folder);    // select+read folder
  void selectExcl(QListWidget*);       // remove items selected in one of the listWidgets
  void select2idx(QListWidget*);       //
  void dataNameSort(size_t folder);    // sort photos in a specific folder by name (time modified)
  void dataTimeSort();                 // sort all photos by time
  void viewFileList();                 // update listWidgets with current files/order
  void displayImage();                 // display cur/prev/next images (selectively enables buttons)
  void showDate();                     // show date in output tab
  void viewStream();                   // view images as thumbnails in stream
  void selectDel(std::vector<size_t> index, bool del=false);
  void on_prevImg_pushButton_clicked();// show previous photo
  void on_nextImg_pushButton_clicked();// show next photo
  void on_headImg_pushButton_clicked();// jump to first photo
  void on_lastImg_pushButton_clicked();// jump to last photo
  void on_exclImg_pushButton_clicked();// exclude image from proceedings (never removed from disk)
  void on_deltImg_pushButton_clicked();// remove photo (can be removed from disk using "clean")
  void on_outPath_pushButton_clicked();// select output path
  void on_outPath_lineEdit_editingFinished();// manually edit output path
  void on_write_pushButton_clicked();  // write sorted batch to output folder
  void on_clean_pushButton_clicked();  // remove all photos marked deleted, remove empty directories
  void on_pushButtonT2i_del_clicked();


  void on_pushButtonT2i_up_clicked();

private:
  Ui::MainWindow            *ui;
  size_t                    idx  = 0;    // current photo (index in "data")
  Thumbnails                *thumbnail;   // class containing all thumbnails
  std::vector<File>         data;        // array with photos + information
  std::vector<File>         delData;     // deleted images
  std::vector<QListWidget*> fileView;    // list with widgets to show selected files
  std::vector<QLineEdit*>   pathView;    // list with widgets to show selected paths
  std::vector<QPushButton*> dirSelec;    // list with widgets to select a folder
  std::vector<QPushButton*> delSelec;    // list with widgets to remover selects files in list
  std::vector<QPushButton*> nameSort;    // list with widgets to sort by name for that camera
  std::list  <QString>      cleanPaths;  // list with input paths (checked to clean later on)
  QString                   workDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  void promptWarning(QString);           // function to show pop-up warning
  void resizeEvent(QResizeEvent*);       // function used to actively resize the window
};

#endif // MAINWINDOW_H
