#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

// ============================================================================

class File
{
public:
  QString     dir      = ""   ; // directory in which the file is stored
  QString     path     = ""   ; // absolute file-path to the file
  QString     disp     = ""   ; // display name
  size_t      camera   = 0    ; // camera-index (corresponds to "fileView")
  int         rotation = 0    ; // rotation in degrees
  std::time_t time     = 0    ; // time at which the photo was taken
  size_t      index    = 0    ; // position in list with all photos
  bool        sort     = true ; // tempory variable to allow subset sort
};

// ============================================================================

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
  void selectFolder(size_t);
  void dataRm(QListWidget *);
  void dataNameSort(size_t);
  void dataSort();
  void fillListWidgets();
  void view(QLabel *, size_t);
  void displayImage();
  void on_prevImg_pushButton_clicked();
  void on_prevBnd_pushButton_clicked();
  void on_nextImg_pushButton_clicked();
  void on_nextBnd_pushButton_clicked();
  void on_headImg_pushButton_clicked();
  void on_lastImg_pushButton_clicked();
  void on_mvDwImg_pushButton_clicked();
  void on_mvDwSet_pushButton_clicked();
  void on_mvUpImg_pushButton_clicked();
  void on_mvUpSet_pushButton_clicked();
  void on_exclImg_pushButton_clicked();
  void on_outPath_pushButton_clicked();
  void on_outPath_lineEdit_editingFinished();
  void on_write_pushButton_clicked();

private:
  Ui::MainWindow            *ui;
  size_t                    idx  = 0;
  bool                      init = true;
  std::vector<File>         data;
  std::vector<QListWidget*> fileView;
  std::vector<QLineEdit*>   pathView;
  std::vector<QPushButton*> dirSelec;
  std::vector<QPushButton*> delSelec;
  std::vector<QPushButton*> nameSort;
  void promptWarning(QString);
  void dataReadTime();
  void resizeEvent(QResizeEvent*);
};

#endif // MAINWINDOW_H
