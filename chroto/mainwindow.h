#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDirIterator>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

#include <iostream>
#include <vector>
#include <string>

// ============================================================================

class Files
{
private:
  std::vector<std::string> _src;
  std::vector<std::string> _dest;
  std::vector<std::string> _path;
  std::vector<int>         _set;

public:

  Files (const Files &) = default;

  Files& operator=(const Files &) = default;

  Files(){};

  size_t size ( void ) { return _src.size(); };

  void push_back ( QString fileName, int set )
  {
    QFile f(fileName);
    QFileInfo fileInfo(f.fileName());
    _src .push_back(fileInfo.fileName().toStdString());
    _path.push_back(fileInfo.absoluteDir().dirName().toStdString());
//    _dest.push_back(""   );
    _set .push_back(set  );
  };

  QString src_short ( size_t idx )
  {
    QString out;

//    if ( std::abs(_set[idx])==1 )
//      out = QString::fromStdString(content)

  };

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
  void updateFiles(void);

private slots:
  void on_folder_pushButton_clicked();

  void on_add_pushButton_clicked();

private:
  Ui::MainWindow *ui;
  Files           data;
};

#endif // MAINWINDOW_H
