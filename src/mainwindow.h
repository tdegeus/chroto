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
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "easyexif/exif.h"

// ============================================================================

class Files
{
public:
  QString     path = "";
  QString     disp = "";
  bool        add;
  int         rotation;
  int         orientation;
  bool        include = true;
  size_t      index = 0;
  std::time_t time;
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
  void updateFiles(void);
  void dataSort(void);
  void displayImage(void);
  void filesRm(QListWidget *list);
  void on_folder_pushButton_clicked();
  void on_add_pushButton_clicked();
  void on_earlier_pushButton_clicked();
  void on_earlier_all_pushButton_clicked();
  void on_later_pushButton_clicked();
  void on_later_all_pushButton_clicked();
  void on_del_pushButton_clicked();

private:
  Ui::MainWindow     *ui;
  std::vector<Files> data;
  size_t             idx  = 0;
  bool               init = true;
  QString            path = "";
  void promptWarning(QString msg);
  void dataReadTime(void);
};

#endif // MAINWINDOW_H
