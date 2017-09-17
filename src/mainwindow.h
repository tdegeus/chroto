/* =================================================================================================

(c - GPLv3) T.W.J. de Geus | tom@geus.me | www.geus.me | github.com/tdegeus/chroto

================================================================================================= */

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
#include <QShortcut>
#include <QTextStream>
#include <QDateTime>
#include <QApplication>

#include "datetimechangeddialog.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cassert>

#include "cppcolormap/cppcolormap.h"
#include "easyexif/exif.h"
#include "date/date.h"
#include "json/json.hpp"

#ifdef WITHEXIV2
  #include "exiv2/exiv2.hpp"
#endif

using json = nlohmann::json;
using Time = date::sys_seconds;

class File;
class Files;
class Thumbnails;

// =================================================================================================
// File : class containing all information of one image
// =================================================================================================

class File
{

public:

  QString path      = ""   ; // absolute (complete) file-path of the file
  QString dir       = ""   ; // directory name
  QString fname     = ""   ; // file name
  QString disp      = ""   ; // display name, constructed from (parts of) the path and the filename
  size_t  folder    = 0    ; // folder-index (corresponds to "m_tF_listWidgets")
  size_t  camera    = 0    ; // camera-index (allows several cameras in one folder)
  int     rotation  = 0    ; // rotation in degrees
  size_t  index     = 0    ; // for sorting: position in list -> locate where "m_idx" went
  bool    sort      = true ; // for sorting: selectively sort subset
  bool    rot_mod   = false; // signal if the photo has been manually rotated
  QIcon   thumb            ; // thumbnail
  bool    thumb_r   = false; // signal if thumbnail has been read
  size_t  thumb_pix = 32   ; // size of the thumbnail (used only when reading)
  Time    t0        = Time(std::chrono::duration<int>(0)); // time, as read from file
  Time    t         = Time(std::chrono::duration<int>(0)); // time, constantly updated on sort

  File            (const File &) = default;
  File& operator= (const File &) = default;
  File(){}

  bool  readEXIF();
  bool  writeEXIF();
  QIcon thumbnail();
  void  setRotation(int rot);
};

// =================================================================================================
// Files : class with a list of "File"
// =================================================================================================

class Files
{

private:

  std::vector<File> m_data;
  size_t            m_ncam=0;
  size_t            m_nfol=0;

public:

  File&       operator[](size_t i)       { return m_data[i]; }
  const File& operator[](size_t i) const { return m_data[i]; }

  std::vector<File>* ptr  ()       { return &m_data;         }
  const File*        data () const { return  m_data.data (); }
  auto               begin()       { return  m_data.begin(); }
  auto               end  ()       { return  m_data.end  (); }
  size_t             size ()       { return  m_data.size (); }

  void   push_back(File file);
  void   pop_back();
  void   empty();
  void   erase(std::vector<File>::iterator i);
  void   erase(std::vector<size_t> i);
  File   take(size_t i);
  size_t sort(size_t idx=0);
  size_t sortName(size_t ifol, size_t idx=0);
  bool   allThumbnailsRead();
  void   setThumbnailResolution(size_t N);
  void   setDispName();

};

// =================================================================================================
// Thumbnails : class to read thumbnails in the background (m_data points to m_data in MainWindow)
// =================================================================================================

class Thumbnails: public QObject
{
  Q_OBJECT

private:

  std::vector<File> *m_data;
  bool               m_busy=false;
  bool               m_stop=false;

public:

  Thumbnails(){};
  Thumbnails(std::vector<File> *input){ m_data = input; }

  void requestStop(){ m_stop = true; } // signal to stop reading
  bool isBusy()     { return m_busy; } // return if the class is (still) reading

public slots:

  void run(); // read all thumbnails

signals:

  void completed(); // signal that all thumbnails have been read

};

// =================================================================================================
// support functions
// =================================================================================================

// return selected item in a "QListWidget" as a list of indices (rows)
std::vector<size_t> selectedItems(QListWidget* list, bool ascending=true);

// find the common path between to names (and rewind until a specific deliminator, e.g. "/")
std::string commonPath(const std::string &path, const std::string &name);
std::string commonPath(const std::string &path, const std::string &name, const std::string &delim);

// remove "path" from the beginning of "name" (no checks, purely based on length)
std::string removePath(const std::string &path, const std::string &name);

// =================================================================================================
// MainWindow: everything starts and ends here
// =================================================================================================

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  QThread m_thread;

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:

  // functions to interpret or manipulate the selection of a QListWidget
  void selection2idx (QListWidget* list); // convert the lowest selected row in "list" -> "m_idx"
  void selectionClear(QListWidget* list); // clear selection "list"
  void selectionClearAll();               // clear selection "m_tF_listWidgets + ui->tS_listWidget"

  // reset the widget, with the option to prompt for continuation
  void resetApp(bool prompt=false);

  // view instruction for the tab, after the tab has been changed
  void instruction();

  // after change in "m_data", clean "m_data", sync with "m_thumbnails", and update relevant view
  void dataUpdate();

  // control the view and buttons in each of the tabs
  void tF_view(); // Tab::Files : update listWidgets with "m_data"
  void tV_view(); // Tab::View  : display image "m_idx", resize
  void tS_view(); // Tab::Sort  : view images as thumbnails, update on sort
  void tW_view(); // Tab::Write : control write and clean buttons

  // Tab::Files : select folder, add all images to "m_data"
  void tF_unifySelection(size_t ifol); // copy user-selection to all other QListWidgets on this tab
  void tF_addFiles      (size_t ifol); // add files for the selected folder
  void tF_excludeSel    (size_t ifol); // exclude selected files of that folder
  void tF_nameSort      (size_t ifol); // sort photos in the folder by name (time modified)

  // Tab::View : full screen control
  void tV_startFullScreen();
  void tV_stopFullScreen();

  // Tab::View : navigation / (un)delete / exclude
  void on_tV_dateTimeEdit_editingFinished(); // change the data of one or batch of photos
  void on_tV_pushButton_fromJpeg_clicked (); // set time to original
  void on_tV_pushButton_prev_clicked     (); // set "m_idx -= 1"
  void on_tV_pushButton_next_clicked     (); // set "m_idx += 1"
  void on_tV_pushButton_first_clicked    (); // set "m_idx  = 0"
  void on_tV_pushButton_last_clicked     (); // set "m_idx  = m_data.size()-1"
  void on_tV_pushButton_excl_clicked     (); // exclude image (take from "m_data")
  void on_tV_pushButton_del_clicked      (); // delete image (take from "m_data" add to "m_dataDel")
  void on_tV_pushButton_undoDel_clicked  (); // re-insert last image from "m_dataDel" into "m_data"
  void on_tV_pushButton_rotL_clicked     (); // rotate the photo counter-clockwise
  void on_tV_pushButton_rotR_clicked     (); // rotate the photo clockwise

  // Tab::Sort : view image in Tab::View on double-click
  void on_tS_listWidget_itemDoubleClicked(QListWidgetItem *item);

  // Tab::Sort : quick scroll navigation
  void on_tS_pushButton_navTop_clicked   ();
  void on_tS_pushButton_navBottom_clicked();
  void on_tS_pushButton_navPgUp_clicked  ();
  void on_tS_pushButton_navPgDwn_clicked ();

  // Tab::Sort : register last selected
  void on_tS_listWidget_itemSelectionChanged();

  // Tab::Sort : sort control buttons
  void on_tS_pushButton_Iexcl_clicked(); // exclude image(s) (take from "m_data")
  void on_tS_pushButton_Idel_clicked (); // delete  image(s) (take from "m_data" add to "m_dataDel")
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

  // Tab::Write
  void on_tW_pushButton_path_clicked      (); // select output path
  void on_tW_lineEdit_path_editingFinished(); // manually edit output path
  void on_tW_pushButton_write_clicked     (); // write sorted batch to output folder
  void on_tW_pushButton_clean_clicked     (); // remove "m_dataDel" from disk, remove empty folders

signals:
  void thumbnailRead(); // start reading the thumbnails
  void dataChanged();   // something has been changed in "m_data"
  void indexChanged();  // "m_idx" has been changed (but "m_data" is the same)

private:
  // Qt variables
  Ui::MainWindow *ui;

  // tab identifiers: use e.g. "ui->tabWidget->currentIndex() == Tab::Files"
  struct Tab { enum Value { Files = 0 , View = 1 , Sort = 2 , Write = 3 }; };

  // state variables
  QString             m_workDir;    // last selected directory
  bool                m_fullScreen; // "true" is the program is full screen, "false otherwise"
  size_t              m_npix;       // size of the thumbnails (relevant on "Tab::View")
  size_t              m_max_fol;    // maximum number of folders on "Tab::Files"
  size_t              m_nfol;       // number of folders selected
  size_t              m_ncam;       // number of cameras selected
  size_t              m_idx;        // current photo (index in "m_data")
  int                 m_selLast;    // last selected index (in "Tab::Sort"); default "-1"
  std::vector<size_t> m_selPrev;    // previous selection (in "tab::Sort")

  // data
  Thumbnails         *m_thumbnails;  // class containing all thumbnails
  Files               m_data;       // array with photos + information
  Files               m_dataDel;    // deleted photos
  std::list<QString>  m_cleanPaths; // list with input paths (checked to clean later on)

  // list with widgets on "Tab::Files"
  std::vector<QListWidget*> m_tF_listWidgets;          // to show selected files
  std::vector<QLabel*>      m_tF_labels_N;             // label with the number of photos in list
  std::vector<QLineEdit*>   m_tF_lineEdits;            // to show selected path
  std::vector<QPushButton*> m_tF_pushButtons_select;   // to select (or exclude) a folder
  std::vector<QPushButton*> m_tF_pushButtons_excl;     // to remove selected files
  std::vector<QPushButton*> m_tF_pushButtons_nameSort; // to sort folder by name

  // colormap to highlight cameras
  std::vector<QColor> m_col;

  // support functions
  void promptWarning (QString msg); // pop-up warning
  bool promptQuestion(QString msg); // pop-up question that the user has to confirm
  void resizeEvent(QResizeEvent*);  // actively resize the viewed image with the window
};

#endif // MAINWINDOW_H
