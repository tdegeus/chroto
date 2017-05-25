#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "imageselection.h"

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

  void push_back(QString name)
  {
    QPixmap pix(50,50);
    pix.fill(QColor("white"));

    path.push_back(name);
    data.push_back(QIcon(pix));
    isread.push_back(0);
  };

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
};

// =================================================================================================

// read absolute time + rotation from JPEG with specified file-name "fname"
std::tuple<std::time_t,int> jpg2info(std::string fname);

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
  void dataRmvSelec(QListWidget*);     // remove items selected in one of the listWidgets
  void dataNameSort(size_t folder);    // sort photos in a specific folder by name (time modified)
  void dataTimeSort();                 // sort all photos by time
  void viewFileList();                 // update listWidgets with current files/order
  void idxViewLabel(QLabel*,size_t i); // view photo "i" in a supplied label
  void displayImage();                 // display cur/prev/next images (selectively enables buttons)
  void showDate();                     // show date in output tab
  void viewStream();                   // view images as thumbnails in stream
  void on_prevImg_pushButton_clicked();// show previous photo
  void on_prevBnd_pushButton_clicked();// jump to previous photo of a different camera
  void on_nextImg_pushButton_clicked();// show next photo
  void on_nextBnd_pushButton_clicked();// jump to next photo of a different camera
  void on_headImg_pushButton_clicked();// jump to first photo
  void on_lastImg_pushButton_clicked();// jump to last photo
  void on_mvDwImg_pushButton_clicked();// move photo one photo earlier
  void on_mvDwSet_pushButton_clicked();// move photo earlier than latest photo of another camera
  void on_mvUpImg_pushButton_clicked();// move photo one photo later
  void on_mvUpSet_pushButton_clicked();// move photo later than earliest photo of another camera
  void on_jumpSel_pushButton_clicked();// open dialog to select time-jump visually
  void on_exclImg_pushButton_clicked();// exclude image from proceedings (never removed from disk)
  void on_deltImg_pushButton_clicked();// remove photo (can be removed from disk using "clean")
  void on_outPath_pushButton_clicked();// select output path
  void on_outPath_lineEdit_editingFinished();// manually edit output path
  void on_write_pushButton_clicked();  // write sorted batch to output folder
  void on_clean_pushButton_clicked();  // remove all photos marked deleted, remove empty directories

private:
  Ui::MainWindow            *ui;
  size_t                    idx  = 0;    // current photo (index in "data")
  bool                      init = true; // re-initialize "idx" to zero, upon viewing
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
