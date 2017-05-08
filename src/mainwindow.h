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

class Files
{
public:
  QString     dir      = "";
  QString     path     = "";
  QString     disp     = "";
  bool        rm       = false;
  size_t      index    = 0;
  size_t      camera   = 0;
  int         rotation = 0;
  std::time_t time     = 0;
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
  void selectFolder(size_t camera);
  void dataRm(QListWidget *list);
  void dataSort(void);
  void fillListWidgets(void);
  void view( QLabel *, size_t );
  void displayImage(void);
  void on_prev_pushButton_clicked();
  void on_prev_bnd_pushButton_clicked();
  void on_next_pushButton_clicked();
  void on_next_bnd_pushButton_clicked();
  void on_first_pushButton_clicked();
  void on_last_pushButton_clicked();
  void on_earlier_pushButton_clicked();
  void on_earlier_all_pushButton_clicked();
  void on_later_pushButton_clicked();
  void on_later_all_pushButton_clicked();
  void on_del_pushButton_clicked();
  void on_output_pushButton_clicked();
  void on_output_lineEdit_editingFinished();
  void on_write_pushButton_clicked();

private:
  Ui::MainWindow *ui;
  size_t         idx  = 0;
  bool           init = true;
  std::vector<Files>        data;
  std::vector<QListWidget*> listWidgets;
  std::vector<QLineEdit*>   lineEdits;
  std::vector<QPushButton*> add_pushButtons;
  std::vector<QPushButton*> rmv_pushButtons;
  void promptWarning(QString msg);
  void dataReadTime(void);
  void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H
