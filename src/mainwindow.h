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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <algorithm>

#include "easyexif/exif.h"
#include "json/json.hpp"

using json = nlohmann::json;

// =================================================================================================

class File
{
public:
  QString     dir      = ""   ; // directory in which the file is stored
  QString     path     = ""   ; // absolute file-path to the file
  QString     disp     = ""   ; // display name
  size_t      folder   = 0    ; // folder-index (corresponds to "fileView")
  size_t      camera   = 0    ; // camera-index (allows several cameras in one folder)
  int         rotation = 0    ; // rotation in degrees
  std::time_t time     = 0    ; // time at which the photo was taken
  size_t      index    = 0    ; // for sorting: position in list -> locale where "idx" went
  bool        sort     = true ; // for sorting: selectively sort subset
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

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void selectFolder(size_t folder);    // select+read folder
  void dataRmvSelec(QListWidget*);     // remove items selected in one of the listWidgets
  void dataNameSort(size_t folder);    // sort photos in a specific folder by name (time modified)
  void dataTimeSort();                 // sort all photos by time
  void viewFileList();                 // update listWidgets with current files/order
  void idxViewLabel(QLabel*,size_t i); // view photo "i" in a supplied label
  void displayImage();                 // display cur/prev/next images (selectively enables buttons)
  void showDate();                     // show date in output tab
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
  void on_delImg_pushButton_clicked();// remove photo (can be removed from disk using "clean")
  void on_outPath_pushButton_clicked();// select output path
  void on_outPath_lineEdit_editingFinished();// manually edit output path
  void on_write_pushButton_clicked();  //
  void on_clean_pushButton_clicked();
  void on_nfolder_comboBox_currentIndexChanged(int index);

  void on_jumpSelect_pushButton_clicked();

  void on_exclImg_pushButton_clicked();

private:
  Ui::MainWindow            *ui;
  size_t                    idx  = 0;    // current photo (index in "data")
  bool                      init = true; // re-initialize "idx" to zero, upon viewing
  std::vector<File>         data;        // array with photos + information
  std::vector<File>         delData;     // deleted images
  std::vector<QListWidget*> fileView;    // list with widgets to show selected files
  std::vector<QLineEdit*>   pathView;    // list with widgets to show selected paths
  std::vector<QPushButton*> dirSelec;    // list with widgets to select a folder
  std::vector<QPushButton*> delSelec;    // list with widgets to remover selects files in list
  std::vector<QPushButton*> nameSort;    // list with widgets to sort by name for that camera
  QString                   workDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  void promptWarning(QString);           // function to show pop-up warning
  void resizeEvent(QResizeEvent*);       // function used to actively resize the window
};

#endif // MAINWINDOW_H
