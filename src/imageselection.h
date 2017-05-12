#ifndef IMAGESELECTION_H
#define IMAGESELECTION_H

#include <QDialog>
#include <QMessageBox>

#include <iostream>

// =================================================================================================

// linearly spaced list of positive integers between start and end points
std::vector<size_t> linspace(size_t start, size_t end, size_t num);

// =================================================================================================
// - shows thumbnails of a selection of photos, specified in "files"/"disp"
// - selection can be refined by the pushButton
// - when OK is pressed the selected image is converted to the corresponding index in "files"
// =================================================================================================

namespace Ui {
class ImageSelection;
}

class ImageSelection : public QDialog
{
  Q_OBJECT

public:
  explicit ImageSelection(QWidget *parent = 0);
  ~ImageSelection();
  std::vector<QString> files;   // absolute filenames of photos
  std::vector<QString> disp;    // display name used as identification, index corresponds to "files"
  std::vector<int>     row2idx; // conversion between thumbnail index and index in "files"
  size_t nshow        = 0;      // number of images shown
  size_t idx          = 0;      // index in "files" that is selected (sync in "update_index")
  bool   apply_to_all = false;  // user selection in checkBox        (sync in "update_index")

public slots:
  void addThumbnails();         // after "files"/"disp" have been added show initial selection

private slots:
  void on_pushButton_clicked(); // increase the number of thumbnails at selected position
  void update_index();          // convert thumbnail-index -> index in "files"

private:
  Ui::ImageSelection *ui;
  void promptWarning(QString msg);

};

// =================================================================================================

#endif // IMAGESELECTION_H
