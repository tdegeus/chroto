#ifndef IMAGESELECTION_H
#define IMAGESELECTION_H

#include <QDialog>
#include <QMessageBox>

#include <iostream>

namespace Ui {
class ImageSelection;
}

class ImageSelection : public QDialog
{
  Q_OBJECT

public:
  explicit ImageSelection(QWidget *parent = 0);
  ~ImageSelection();
  std::vector<QString> files;
  std::vector<QString> disp;
  std::vector<int>     row2idx;
  size_t nshow = 0;
  size_t idx = 0;
  bool   apply_to_all = false;

public slots:
  void display();
  void update_index();

private slots:
  void on_pushButton_clicked();

private:
  Ui::ImageSelection *ui;
  void promptWarning(QString msg);

};

#endif // IMAGESELECTION_H
