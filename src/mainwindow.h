/*
class MainWindow

  This class controls everything. When the program is opened first "MainWindow::MainWindow" is run,
  then the different functions are run based on the buttons that the user clicks. The streamline
  can be understood (i) from the function directly linked to user actions
  (e.g. "MainWindow::on_tS_pushButton_Iup_clicked"); (ii) from the connections between the different
  signals and slots (NB there are signals emitted by the Qt-widgets, but also some custom signals).

  This class also stores all "data". The thumbnails of the entire "data" is stored in a different
  class "thumbnail", which runs on a different processor in the background. The processes of
  obtaining the thumbnails is restarted each time the "data" is added. "tS_view" reads from this
  separate class, and restarts reading if the reading is incomplete.

class File

  The "data" in "MainWindow" is a vector of the "File"-class. This class holds all information on
  the photo (time, location, etc.), except the thumbnail.

class Thumbnails

  Class that holds a vector of thumbnails. The process of getting the thumbnails can be (re)started
  using "read". Note that except when items are removed using "Thumbnails::erase", the order never
  changes. For each item, "MainWindow::data[i]", the index in this list is stored as "File::ithumb".

(c) T.W.J. de Geus | tom@geus.me | www.geus.me | github.com/tdegeus/chroto
*/

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
#include <QThread>
#include <QDateTime>
#include <QListWidget>
#include <QLabel>
#include <QShortcut>
#include <QTextStream>

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
  std::vector<int>     rotation;
  std::vector<int>     isread;
  bool                 busy = false;
  bool                 stop = false;
  size_t               npix = 50;

public:

  // read QIcon "i"
  QIcon& at          ( size_t i ) { return data[i];     }

  // get the size
  size_t size        (          ) { return data.size(); }

  // stop reading (restart reading by calling "Thumbnails::read")
  void   requestStop (          ) { stop = true;        }

  // check if the class is (still) in the middle of the loop to read all images
  bool   isBusy      (          ) { return busy;        }

  // get the amount of unread images
  size_t unread()
  {
    size_t n = 0;

    for ( auto &i: isread )
      if ( !i )
        ++n;

    return n;
  }

  // add an image to the list; return the index at which it is inserted
  size_t push_back(QString name, int rot=0)
  {
    QPixmap pix(npix,npix);
    pix.fill(QColor("white"));

    path    .push_back(name);
    data    .push_back(QIcon(pix));
    isread  .push_back(0);
    rotation.push_back(rot);

    return data.size()-1;
  }

  // remove items from the lists; warning: only time that the index of the images changes
  void erase(std::vector<size_t> index)
  {
    stop = true;

    std::sort(index.begin(),index.end(),[](size_t i,size_t j){return i>j;});

    for ( auto &i: index ) {
      data    .erase(data    .begin()+i);
      path    .erase(path    .begin()+i);
      isread  .erase(isread  .begin()+i);
      rotation.erase(rotation.begin()+i);
    }
  }

  // remove all items from the lists
  void empty()
  {
    stop = true;

    while ( data    .size()>0 ) data    .erase(data    .begin());
    while ( path    .size()>0 ) path    .erase(path    .begin());
    while ( isread  .size()>0 ) isread  .erase(isread  .begin());
    while ( rotation.size()>0 ) rotation.erase(rotation.begin());

    busy = false;
    npix = 50;
  }

  // overwrite the size of the thumbnails
  void setSize( size_t N )
  {
    // - stop the current read
    stop = true;
    // - overwrite number of pixels in both directions
    npix = N;
    // - mark all for reading
    for ( auto &i : isread ) i = 0;
  }

public slots:

  // loop to read all thumbnails
  void read()
  {
    busy = true ;
    stop = false;

    // - loop over all photos
    for ( size_t i=0; i<data.size(); ++i )
    {
      // -- break the loop if requested externally
      if ( stop || QThread::currentThread()->isInterruptionRequested() ) {
        busy = false;
        stop = false;
        return;
      }

      // -- read if not read before: account for pre-specified rotation
      //    (obtained from the EXIF-data in "MainWindow" below)
      if ( !isread[i] )
      {
        QMatrix rot;
        rot.rotate(rotation[i]);

        QPixmap pix(path[i]);
        pix.scaled(npix,npix,Qt::KeepAspectRatio, Qt::FastTransformation);

        if ( stop ) {
          busy = false;
          stop = false;
          return;
        }

        data  [i] = QIcon(QPixmap(pix.transformed(rot)));
        isread[i] = 1;
      }
    }

    // - transmit that all images have been read
    busy = false;
    stop = false;
    emit completed();
  }

signals:

  void completed();

};

// =================================================================================================

class File
{
public:
  QString     dir       = ""   ; // directory in which the file is stored
  QString     path      = ""   ; // absolute file-path to the file
  QString     disp      = ""   ; // display name
  size_t      folder    = 0    ; // folder-index (corresponds to "tF_listWidgets")
  size_t      camera    = 0    ; // camera-index (allows several cameras in one folder)
  int         rotation  = 0    ; // rotation in degrees
  std::time_t time_orig = 0    ; // time at which the photo was taken: no change on sort
  std::time_t time      = 0    ; // time at which the photo was taken: changed on sort
  size_t      index     = 0    ; // for sorting: position in list -> locate where "idx" went
  bool        sort      = true ; // for sorting: selectively sort subset
  int         modified  = 0    ; // signal that time is still in line with the rest of the photos
  size_t      ithumb           ; // index in list with thumbnails

  File            (const File &) = default;
  File& operator= (const File &) = default;
  File(){}
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
    std::string txt = "Error parsing file, error code: ";
    std::string err = txt + std::to_string(code);
    throw std::runtime_error(err);
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
}

// =================================================================================================

// return selected item in a "QListWidget" as a list of indices (rows)
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
}

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

  // support functions
  void listExcl(QListWidget*); // exclude selected images (take from "data")
  void listDel (QListWidget*); // delete  selected images (take from "data", add to "delData")
  void list2idx(QListWidget*); // convert selected image -> "idx"
  void clearSel(QListWidget*); // clear selection of a list
  void clearSelAll();          // clear selection of all lists
  void clearAll();             // reset the widget

  // "data" manipulation
  size_t data_renumCamera();        // update cameras to the smallest possible index (returns size)
  size_t data_nCamera();            // the number of cameras in "data" (a.k.a. size)
  void   data_update();             // update "data", "thumbnail", and relevant view
  void   data_sortName(size_t fol); // sort photos in the folder by name (time modified)
  void   data_sortTime();            // sort all photos by time

  // control the view and buttons in each of the tabs
  void tF_view(); // tabFiles: update listWidgets with "data"
  void tV_view(); // tabView : display image "idx", selectively enable buttons
  void tS_view(); // tabSort : view images as thumbnails in stream
  void tW_view(); // tabWrite: control write and clean buttons

  // tabFiles: change the displayed number of folders
  void on_tF_comboBox_currentIndexChanged(int index);

  // tabFiles: select folder, add all images to "data"
  void tF_addFiles(size_t folder);

  // tabView: full screen control
  void tV_startFullScreen();
  void tV_stopFullScreen();

  // tabView: navigation / (un)delete / exclude
  void on_tV_pushButton_prev_clicked   (); // set "idx -= 1"
  void on_tV_pushButton_next_clicked   (); // set "idx += 1"
  void on_tV_pushButton_first_clicked  (); // set "idx  = 0"
  void on_tV_pushButton_last_clicked   (); // set "idx  = data.size()-1"
  void on_tV_pushButton_excl_clicked   (); // exclude image (take from "data")
  void on_tV_pushButton_del_clicked    (); // delete  image (take from "data", add to "delData")
  void on_tV_pushButton_undoDel_clicked(); // re-insert latex image from "delData" into "data"

  // tabSort: quick scroll navigation
  void on_tS_pushButton_navTop_clicked   ();
  void on_tS_pushButton_navBottom_clicked();
  void on_tS_pushButton_navPgUp_clicked  ();
  void on_tS_pushButton_navPgDwn_clicked ();

  // tabSort: register last selected
  void on_tS_listWidget_itemSelectionChanged();

  // tabSort: sort control buttons
  void on_tS_pushButton_Iexcl_clicked(); // exclude image(s) (take from "data")
  void on_tS_pushButton_Idel_clicked (); // delete  image(s) (take from "data", add to "delData")
  void on_tS_pushButton_Iup_clicked  (); // move image(s) up
  void on_tS_pushButton_Idown_clicked(); // move image(s) down
  void on_tS_pushButton_Isync_clicked(); // sync image(s)
  void on_tS_pushButton_Cup_clicked  (); // move image(s) up   (apply for all images in camera)
  void on_tS_pushButton_Cdown_clicked(); // move image(s) down (apply for all images in camera)
  void on_tS_pushButton_Csync_clicked(); // sync image(s)      (apply for all images in camera)
  void on_tS_pushButton_Fup_clicked  (); // move image(s) up   (apply for all images in folder)
  void on_tS_pushButton_Fdown_clicked(); // move image(s) down (apply for all images in folder)
  void on_tS_pushButton_Fsync_clicked(); // sync image(s)      (apply for all images in folder)
  void on_tS_pushButton_split_clicked(); // split selected images to new 'camera'

  // tabWrite
  void on_tW_pushButton_path_clicked      (); // select output path
  void on_tW_lineEdit_path_editingFinished(); // manually edit output path
  void on_tW_pushButton_write_clicked     (); // write sorted batch to output folder
  void on_tW_pushButton_clean_clicked     (); // remove "delData" from disk, remove empty folders
  void on_tW_comboBox_activated(int index);   // set time reference based on folder

signals:
  void thumbnailRead(); // start reading the thumbnails
  void dataChanged();   // something has been changed in "data"
  void indexChanged();  // "idx" has been changed (but not "data")

private:
  // Qt variables
  Ui::MainWindow *ui;

  // tab identifiers
  enum tab { tabFiles = 0 , tabView = 1 , tabSort = 2 , tabWrite = 3 };

  // last selected directory
  QString workDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

  // state variables
  bool   fullScreen = false ;   // true is the program is full screen
  size_t npix       = 200   ;   // size of the thumbnails (tabView)
  size_t idx        =   0   ;   // current photo (index in "data")
  int    selLast    =  -1   ;   // last selected index (in tabView); -1 if no last selected item

  // selected items
  std::vector<size_t> selPrev;  // previous selection  (in tabView)

  // data
  Thumbnails        *thumbnail;  // class containing all thumbnails
  std::vector<File>  data;       // array with photos + information
  std::vector<File>  delData;    // deleted images
  std::list<QString> cleanPaths; // list with input paths (checked to clean later on)

  // list with widgets
  std::vector<QListWidget*> tF_listWidgets;          // to show selected files
  std::vector<QLineEdit*>   tF_lineEdits;            // to show selected paths
  std::vector<QPushButton*> tF_pushButtons_select;   // to select a folder
  std::vector<QPushButton*> tF_pushButtons_excl;     // to remover selects files in list
  std::vector<QPushButton*> tF_pushButtons_nameSort; // to sort by name for that camera

  // support functions
  void promptWarning (QString msg); // pop-up warning
  bool promptQuestion(QString msg); // pop-up question that the user has to confirm
  void resizeEvent(QResizeEvent*);  // actively resize the viewed image with the window
};

#endif // MAINWINDOW_H
