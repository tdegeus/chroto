/*
See "mainwindow.h" to get an overview of the program, and a sense where what is. Another way to get
an overview is to use Qt Creator on "mainwindow.ui". The latter can also be used to investigate some
of the signals and slots.

(c) T.W.J. de Geus | tom@geus.me | www.geus.me | github.com/tdegeus/chroto
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

// =================================================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  // initialize widget using the UI-file
  ui->setupUi(this);

  // class to read/store list of thumbnails; assign to worker-thread
  thumbnail = new Thumbnails;
  thumbnail->moveToThread(&workerThread);
  // list of thumbnails is deleted only when the worker-thread is deleted (in destructor below)
  connect(&workerThread,&QThread::finished,thumbnail,&QObject::deleteLater);

  // tabFiles: fill arrays collecting the file-lists and related buttons
  tF_listWidgets         .push_back( ui->tF_listWidget_0          );
  tF_listWidgets         .push_back( ui->tF_listWidget_1          );
  tF_listWidgets         .push_back( ui->tF_listWidget_2          );
  tF_listWidgets         .push_back( ui->tF_listWidget_3          );
  tF_listWidgets         .push_back( ui->tF_listWidget_4          );
  tF_listWidgets         .push_back( ui->tF_listWidget_5          );
  tF_listWidgets         .push_back( ui->tF_listWidget_6          );
  tF_listWidgets         .push_back( ui->tF_listWidget_7          );
  tF_listWidgets         .push_back( ui->tF_listWidget_8          );
  tF_listWidgets         .push_back( ui->tF_listWidget_9          );
  tF_listWidgets         .push_back( ui->tF_listWidget_10         );
  tF_listWidgets         .push_back( ui->tF_listWidget_11         );
  // --
  tF_lineEdits           .push_back( ui->tF_lineEdit_0            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_1            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_2            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_3            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_4            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_5            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_6            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_7            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_8            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_9            );
  tF_lineEdits           .push_back( ui->tF_lineEdit_10           );
  tF_lineEdits           .push_back( ui->tF_lineEdit_11           );
  // --
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_0   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_1   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_2   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_3   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_4   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_5   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_6   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_7   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_8   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_9   );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_10  );
  tF_pushButtons_select  .push_back( ui->tF_pushButton_select_11  );
  // --
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_0     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_1     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_2     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_3     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_4     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_5     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_6     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_7     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_8     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_9     );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_10    );
  tF_pushButtons_excl    .push_back( ui->tF_pushButton_excl_11    );
  // --
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_0 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_1 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_2 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_3 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_4 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_5 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_6 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_7 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_8 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_9 );
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_10);
  tF_pushButtons_nameSort.push_back( ui->tF_pushButton_nameSort_11);

  // tabFiles: link scroll position listWidgets
  for ( auto &i: tF_listWidgets )
    for ( auto &j: tF_listWidgets )
      if ( i!=j )
        connect(i->verticalScrollBar(),SIGNAL(valueChanged(int)),
                j->verticalScrollBar(),SLOT  (setValue    (int)));

  // tabView: center image
  ui->tV_label->setAlignment(Qt::AlignCenter);

  // tabFiles: select folder / remove selected files / sort by name
  for ( size_t i = 0 ; i < tF_pushButtons_select.size() ; ++i )
  {
    connect(tF_pushButtons_select  [i],&QPushButton::clicked,[=](){tF_addFiles  (          i   );});
    connect(tF_pushButtons_nameSort[i],&QPushButton::clicked,[=](){data_sortName(          i   );});
    connect(tF_pushButtons_excl    [i],&QPushButton::clicked,[=](){listExcl(tF_listWidgets[i],i);});
  }

  // "data" changed -> sort by time and refresh views
  connect( this     , SIGNAL( dataChanged  () ), this      , SLOT( data_update() ) );

  // "read" thumbnails, update "data" / refresh views when reading is completed
  connect( this     , SIGNAL( thumbnailRead() ), thumbnail , SLOT( read       () ) );
  connect( thumbnail, SIGNAL( completed    () ), this      , SLOT( data_update() ) );

  // tab changed: update "data" and views to latest values, clear selected items
  connect( ui->tabWidget, &QTabWidget::currentChanged, [=](){ clearSelAll (); } );
  connect( ui->tabWidget, &QTabWidget::currentChanged, [=](){ data_update (); } );

  // tabView: "idx" changed -> update view
  connect( this, SIGNAL( indexChanged() ), this, SLOT( tV_view() ) );

  // tabFiles: convert selected item to current index
  for ( auto &i: tF_listWidgets ) connect(i,&QListWidget::itemSelectionChanged,[=](){list2idx(i);});

  // tabSort: convert selected item to current index
  connect( ui->tS_listWidget,&QListWidget::itemSelectionChanged,[=](){list2idx(ui->tS_listWidget);});

  // tabSort: manually refresh view to show the latest thumbnails
  connect( ui->tS_pushButton_refresh, SIGNAL( clicked(bool) ), SLOT( tS_view() ) );

  // tabWrite: couple "outPath" pushButton to "outPath" lineEdit
  connect( ui->tW_pushButton_path,SIGNAL(clicked(bool)),SLOT(on_tW_lineEdit_path_editingFinished()));

  // start fresh: clear all data and widgets
  connect( ui->actionNew, SIGNAL( triggered(bool) ), this, SLOT( clearAll() ) );

  // connect full screen button
  connect( ui->tV_pushButton_fullScreen, SIGNAL( clicked(bool) ), SLOT( tV_stopFullScreen () ) );
  connect( ui->tV_pushButton_fullScreen, SIGNAL( clicked(bool) ), SLOT( tV_startFullScreen() ) );

  // set shortcuts
  // - define
  QShortcut *short_esc   = new QShortcut(QKeySequence(Qt::Key_Escape                  ), this);
  QShortcut *short_next1 = new QShortcut(QKeySequence(Qt::Key_Right                   ), this);
  QShortcut *short_next2 = new QShortcut(QKeySequence(Qt::Key_Space                   ), this);
  QShortcut *short_left  = new QShortcut(QKeySequence(Qt::Key_Left                    ), this);
  QShortcut *short_excl  = new QShortcut(QKeySequence(Qt::Key_E                       ), this);
  QShortcut *short_del1  = new QShortcut(QKeySequence(Qt::Key_Delete                  ), this);
  QShortcut *short_del2  = new QShortcut(QKeySequence(Qt::Key_Backspace               ), this);
  QShortcut *short_del3  = new QShortcut(QKeySequence(Qt::Key_D                       ), this);
  QShortcut *short_undo  = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z            ), this);
  QShortcut *short_full  = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F), this);
  // - enable in full screen
  short_esc  ->setContext(Qt::ApplicationShortcut);
  short_next1->setContext(Qt::ApplicationShortcut);
  short_next2->setContext(Qt::ApplicationShortcut);
  short_left ->setContext(Qt::ApplicationShortcut);
  short_excl ->setContext(Qt::ApplicationShortcut);
  short_del1 ->setContext(Qt::ApplicationShortcut);
  short_del2 ->setContext(Qt::ApplicationShortcut);
  short_del3 ->setContext(Qt::ApplicationShortcut);
  short_undo ->setContext(Qt::ApplicationShortcut);
  // - connect shortcuts
  connect(short_esc  , SIGNAL( activated() ), this, SLOT( tV_stopFullScreen                () ));
  connect(short_full , SIGNAL( activated() ), this, SLOT( tV_startFullScreen               () ));
  connect(short_next1, SIGNAL( activated() ), this, SLOT( on_tV_pushButton_next_clicked    () ));
  connect(short_next2, SIGNAL( activated() ), this, SLOT( on_tV_pushButton_next_clicked    () ));
  connect(short_left , SIGNAL( activated() ), this, SLOT( on_tV_pushButton_prev_clicked    () ));
  connect(short_excl , SIGNAL( activated() ), this, SLOT( on_tV_pushButton_excl_clicked    () ));
  connect(short_excl , SIGNAL( activated() ), this, SLOT( on_tS_pushButton_Iexcl_clicked   () ));
  connect(short_del1 , SIGNAL( activated() ), this, SLOT( on_tS_pushButton_Idel_clicked    () ));
  connect(short_del2 , SIGNAL( activated() ), this, SLOT( on_tS_pushButton_Idel_clicked    () ));
  connect(short_del3 , SIGNAL( activated() ), this, SLOT( on_tS_pushButton_Idel_clicked    () ));
  connect(short_del1 , SIGNAL( activated() ), this, SLOT( on_tV_pushButton_del_clicked     () ));
  connect(short_del2 , SIGNAL( activated() ), this, SLOT( on_tV_pushButton_del_clicked     () ));
  connect(short_del3 , SIGNAL( activated() ), this, SLOT( on_tV_pushButton_del_clicked     () ));
  connect(short_undo , SIGNAL( activated() ), this, SLOT( on_tV_pushButton_undoDel_clicked () ));

  // enforce opening on correct tab
  ui->tabWidget->setCurrentIndex( tabFiles );

  // tabFiles: set number of folders (==2)
  ui->tF_comboBox->setCurrentIndex(1);

  // start worker-thread (to which "thumbnail" is assigned)
  workerThread.start();

  // set style, due to : "https://github.com/ColinDuquesnoy/QDarkStyleSheet"
  QFile f(":qdarkstyle/style.qss");
  f.open(QFile::ReadOnly | QFile::Text);
  QTextStream ts(&f);
  qApp->setStyleSheet(ts.readAll());
}

// =================================================================================================

MainWindow::~MainWindow()
{
  // make sure that the Thumbnails thread also stops
  workerThread.requestInterruption();
  workerThread.quit();
  workerThread.wait();

  // remove the application
  delete ui;
}

// =================================================================================================

void MainWindow::resizeEvent(QResizeEvent* event)
{
  QMainWindow::resizeEvent(event);
  this->tV_view();
}

// =================================================================================================

void MainWindow::promptWarning(QString msg)
{
  QMessageBox::warning(this,tr("chroto"),msg,QMessageBox::Ok,QMessageBox::Ok);
}

// =================================================================================================

bool MainWindow::promptQuestion(QString msg)
{
  QMessageBox::StandardButton reply;

  reply = QMessageBox::question(this,tr("chroto"),msg,QMessageBox::Yes|QMessageBox::No);

  if (reply == QMessageBox::Yes)
    return true;

  return false;
}

// =================================================================================================

void MainWindow::listExcl(QListWidget *list, int ilist)
{
  // get a list with selected items
  std::vector<size_t> index = selectedItems(list,false);

  // proceed only for non-empty lists
  if  ( index.size() <= 0 ) return;

  // remove indices which are not part of the current folder
  if ( ilist >= 0 )
  {
    std::vector<int> rm;

    for ( int i = index.size()-1 ; i >= 0 ; --i )
      if ( data[index[i]].folder != static_cast<size_t>( ilist ) )
        rm.push_back(i);

    for ( auto &i : rm )
      index.erase(index.begin()+i);
  }

  // exclude images: remove from data
  for ( size_t &i : index ) data.erase(data.begin()+i);

  // empty selecting
  clearSel(list);

  // set number current item (TODO: might need revision)
  if ( index[index.size()-1] > 0 ) idx = index[index.size()-1]-1;
  else                             idx = 0;

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::listDel(QListWidget *list)
{
  // get a list with selected items
  std::vector<size_t> index = selectedItems(list,false);

  // proceed only for non-empty lists
  if  ( index.size() <= 0 ) return;

  // delete images: add to "delData" (removal when 'clean up' is pressed)
  for ( size_t &i : index ) delData.push_back(data[i]);
  // delete images: remove from "data"
  for ( size_t &i : index ) data.erase(data.begin()+i);

  // empty selecting
  clearSel(list);

  // set number current item (TODO: might need revision)
  if ( index[index.size()-1] > 0 ) idx = index[index.size()-1]-1;
  else                             idx = 0;

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::list2idx(QListWidget *list)
{
  // get a list with selected items
  std::vector<size_t> sel = selectedItems(list);

  // new current item: default equal to "idx", try to change below
  size_t jdx = idx;

  // new current item: take from selection
  if ( sel.size() > 0     ) jdx = sel[0];
  // new current item: fix possible out-of-bound
  if ( jdx >= data.size() ) jdx = data.size()-1;

  // if needed, change current item
  if ( jdx != idx )
  {
    idx = jdx;
    emit indexChanged();
  }
}

// =================================================================================================

void MainWindow::clearSel(QListWidget *list)
{
  for ( int j = 0 ; j < list->count() ; ++j ) list->item(j)->setSelected(false);
}

// =================================================================================================

void MainWindow::clearSelAll()
{
  // all lists on tabFiles
  for ( auto &list: tF_listWidgets ) clearSel(list);

  // list of tabSort
  clearSel(ui->tS_listWidget);
}

// =================================================================================================

void MainWindow::clearAll()
{
  // prompt user for confirmation
  if ( !promptQuestion("This clears everything, to start fresh. Continue?") )
    return;

  // remove all data
  while ( data   .size()>0 ) { data   .pop_back(); }
  while ( delData.size()>0 ) { delData.pop_back(); }

  // reset index and working directory
  idx     = 0;
  workDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

  // tabWrite: reset all widgets to default
  ui->tW_lineEdit_date     -> clear();
  ui->tW_lineEdit_path     -> clear();
  ui->tW_lineEdit_name     -> setText("sorted");
  ui->tW_checkBox_keepOrig -> setChecked(false);
  ui->tW_pushButton_write  -> setEnabled(false);

  // tabFiles: set number of folders (==2)
  ui->tF_comboBox->setCurrentIndex(1);

  // enforce opening on the first tab
  ui->tabWidget->setCurrentIndex(0);

  // emit signal to clear also all thumbnails, and empty all views
  emit dataChanged();
}

// =================================================================================================

size_t MainWindow::data_renumCamera()
{
  // only proceed for non-empty data
  if ( data.size() == 0 ) return 0;

  // update camera index to smallest possible
  // - logical list: 1 == camera is currently in use; 0 == camera is not in use (default)
  std::vector<int> cam( data.size() , 0 );
  // - fill
  for ( auto &i : data ) cam[i.camera] = 1;
  // - convert to new indices, step 1/2: first index starts at 0
  cam[0] -= 1;
  // - convert to new indices, step 2/2: cumulative sum
  for ( size_t i=1; i<cam.size(); ++i ) cam[i] += cam[i-1];
  // - renumber data
  for ( auto &i : data ) i.camera = cam[i.camera];

  // return number of cameras
  return ( *std::max_element(cam.begin(), cam.end() ) ) + 1;
}

// =================================================================================================

size_t MainWindow::data_nCamera()
{
  size_t n = 0;

  for ( auto &i : data ) n = std::max( n , i.camera );

  return n + 1;
}

// =================================================================================================

size_t MainWindow::data_nFolder()
{
  std::vector<int> v( data.size() , 0 );

  for ( auto &i : data ) v[i.folder] = 1;

  return std::accumulate(v.begin(), v.end(), 0);
}

// =================================================================================================

void MainWindow::data_sortName(size_t folder)
{
  // check to continue
  if ( data.size() == 0 ) return;

  // clear all selected items
  clearSelAll();

  // store current order, to retrieve the new position of "idx"
  for ( size_t i = 0 ; i < data.size() ; ++i ) data[i].index = i;

  // only sort for this folder (items with "data[i].sort == false" are left untouched)
  for ( auto &file : data ) {
    if ( file.folder==folder ) { file.sort = true ; file.modified = 1; }
    else                       { file.sort = false;                    }
  }

  // apply selective sort, based on file-name
  std::sort(data.begin(),data.end(),
    [](File i,File j){
      if ( i.sort && j.sort ) { return i.path.toStdString()<j.path.toStdString(); }
      else                    { return i.index             <j.index             ; }
  });

  // update time such that the sorted list is also in chronological order
  for ( size_t i=data.size()-1; i>0; --i )
    if ( data[i-1].time > data[i].time )
      data[i-1].time = data[i].time-1;

  // locate new position of "idx"
  for ( size_t i=0; i<data.size(); ++i ) {
    if ( data[i].index==idx ) {
      idx = i;
      emit dataChanged();
      return;
    }
  }

  // force to view from the first index
  idx = 0;

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::data_sortTime()
{
  // check to continue
  if ( data.size() == 0 ) return;

  // store current order, to retrieve the new position of "idx"
  for ( size_t i = 0 ; i < data.size() ; ++i ) data[i].index = i;

  // sort chronologically (if the times are identical retain existing order)
  std::sort(data.begin(),data.end(),
    [](File i,File j){
      if ( i.time==j.time ) return i.index<j.index;
      else                  return i.time <j.time ;
  });

  // locate new position of "idx"
  for ( size_t i=0; i<data.size(); ++i ) {
    if ( data[i].index==idx ) {
      idx = i;
      break;
    }
  }

  // make sure that all photos are at least one second apart
  // this is needed to allow image insertion between image
  for ( size_t i=0; i<data.size()-1; ++i )
    if ( data[i+1].time <= data[i].time )
      data[i+1].time = data[i].time+1;
}

// =================================================================================================

void MainWindow::data_update()
{
  // request to stop reading thumbnails (to lose fewer time below)
  thumbnail->requestStop();

  // update camera index to the smallest possible index
  data_renumCamera();

  // list with thumbnails to include
  // - initialize
  std::vector<int>    incl( thumbnail->size() , 0 ); // 1 if the thumbnail is still part of "data"
  std::vector<size_t> rm;                            // list with thumbnails to remove
  // - fill
  for ( auto &i : data ) incl[i.ithumb] = 1;
  // - list items to remove
  for ( size_t i=0; i<incl.size(); ++i ) if ( incl[i]==0 ) rm.push_back(i);
  // - convert to new indices, step 1/2: cumulative sum
  for ( size_t i=1; i<incl.size(); ++i ) incl[i] += incl[i-1];
  // - convert to new indices, step 2/2: correct one (list started at 1, not 0)
  for ( auto &i : incl ) --i;
  // - renumber data
  for ( auto &i : data ) i.ithumb = incl[i.ithumb];

  // correct current index if needed (out-of-bounds -> last)
  if ( idx >= data.size() ) idx = data.size()-1;

  // sort by time
  data_sortTime();

  // remove thumbnails that are no longer part of "data"
  thumbnail->erase(rm);
  // if needed, launch reading thumbnails (that have not been read)
  if ( thumbnail->unread() ) { emit thumbnailRead(); }

  // update widget
  if      ( ui->tabWidget->currentIndex() == tabFiles ) tF_view();
  else if ( ui->tabWidget->currentIndex() == tabView  ) tV_view();
  else if ( ui->tabWidget->currentIndex() == tabSort  ) tS_view();
  else if ( ui->tabWidget->currentIndex() == tabWrite ) tW_view();
}

// =================================================================================================

void MainWindow::tF_view()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabFiles ) return;

  // empty storage paths
  for ( auto line : tF_lineEdits ) line->clear();
  // empty listWidgets
  for ( auto list : tF_listWidgets ) while ( list->count()>0 ) list->takeItem(0);

  // no data -> quit this function
  if ( data.size() == 0 ) return;

  // fill listWidgets
  for ( auto &file : data )
  {
    for ( size_t l=0; l<tF_listWidgets.size(); ++l )
    {
      if ( file.folder==l )
      {
        tF_listWidgets[l]->addItem(file.disp);
        tF_lineEdits[l]->setText(file.dir );
      }
      else
      {
        tF_listWidgets[l]->addItem("");
      }
    }
  }

  // update selected row to current index
  for ( size_t l = 0 ; l < tF_listWidgets.size() ; ++l ) tF_listWidgets[l]->setCurrentRow(idx);
}

// =================================================================================================

void MainWindow::tV_view()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabView ) return;

  // selectively disable/enable buttons
  ui->tV_pushButton_prev       -> setEnabled( data   .size() > 0 && idx > 0             );
  ui->tV_pushButton_first      -> setEnabled( data   .size() > 0 && idx > 0             );
  ui->tV_pushButton_next       -> setEnabled( data   .size() > 0 && idx < data.size()-1 );
  ui->tV_pushButton_last       -> setEnabled( data   .size() > 0 && idx < data.size()-1 );
  ui->tV_pushButton_undoDel    -> setEnabled( delData.size() > 0                        );
  ui->tV_pushButton_fullScreen -> setEnabled( data   .size() > 0                        );
  ui->tV_pushButton_del        -> setEnabled( data   .size() > 0                        );
  ui->tV_pushButton_excl       -> setEnabled( data   .size() > 0                        );

  // clear currently viewed photos
  ui->tV_label->clear();

  // check to continue
  if ( data.size() == 0 ) return;

  // view current photo "idx"
  QPixmap    p(data[idx].path);
  int        w = ui->tV_label->width();
  int        h = ui->tV_label->height();
  QTransform transform;
  QTransform trans = transform.rotate(data[idx].rotation);
  ui->tV_label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
}

// =================================================================================================

void MainWindow::tS_view(void)
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // selective enable buttons
  ui->tS_pushButton_refresh -> setEnabled( data.size()    > 0 );
  ui->tS_pushButton_Idel    -> setEnabled( data.size()    > 0 );
  ui->tS_pushButton_Iexcl   -> setEnabled( data.size()    > 0 );
  ui->tS_pushButton_Iup     -> setEnabled( data.size()    > 1 );
  ui->tS_pushButton_Idown   -> setEnabled( data.size()    > 1 );
  ui->tS_pushButton_Isync   -> setEnabled( data.size()    > 1 );
  ui->tS_pushButton_split   -> setEnabled( data.size()    > 1 );
  ui->tS_pushButton_Cup     -> setEnabled( data_nCamera() > 1 );
  ui->tS_pushButton_Cdown   -> setEnabled( data_nCamera() > 1 );
  ui->tS_pushButton_Csync   -> setEnabled( data_nCamera() > 1 );
  ui->tS_pushButton_Fup     -> setEnabled( data_nFolder() > 1 );
  ui->tS_pushButton_Fdown   -> setEnabled( data_nFolder() > 1 );
  ui->tS_pushButton_Fsync   -> setEnabled( data_nFolder() > 1 );

  // list with selected rows
  std::vector<size_t> old = selectedItems(ui->tS_listWidget);
  // convert rows to new index
  // - allocate
  std::vector<size_t> renum(data.size());
  // - fill
  for ( size_t i = 0 ; i < data.size() ; ++i ) renum[data[i].index] = i;
  // - new rows
  std::vector<size_t> rows;
  // - renumber
  for ( auto &i : old ) rows.push_back(renum[i]);

  // empty the list
  while ( ui->tS_listWidget->count() > 0 ) ui->tS_listWidget->takeItem(0);

  // check to continue
  if ( data.size() == 0 ) return;

  // read number of cameras
  size_t n = data_nCamera();

  // create a colormap
  std::vector<QColor> col;
  // - manually set first five colors
  col.push_back( QColor(  72,   8 ,   7 ) );
  col.push_back( QColor(  24,  63 ,  63 ) );
  col.push_back( QColor(  22,  64 , 124 ) );
  col.push_back( QColor(  68,  47 ,  35 ) );
  col.push_back( QColor( 100, 100 , 100 ) );
  col.push_back( QColor(  92,  92 , 134 ) );
  // - automatically add more colors
  if ( n > 6 )
  {
    std::vector<int> cmap = cppcolormap::Set1(n-6);
    for ( size_t i = 0 ; i < n-6 ; ++i )
      col.push_back( QColor( cmap[i*3+0], cmap[i*3+1], cmap[i*3+2] ) );
  }

  // define style of the widget
  ui->tS_listWidget->setIconSize(QSize(npix,npix));

  // fill the lists with names / icons
  for ( auto &i : data )
    ui->tS_listWidget->addItem(new QListWidgetItem(thumbnail->at(i.ithumb),i.disp));

  // set background color, corresponding to the camera index
  if ( n > 1 )
    for ( size_t i = 0 ; i < data.size() ; ++i )
      ui->tS_listWidget->item(i)->setBackground(QBrush(col[data[i].camera]));

  // restore selection
  // - clear entire selection
  for ( int i=0; i<ui->tS_listWidget->count(); ++i ) ui->tS_listWidget->item(i)->setSelected(false);
  // - no rows selected -> set "idx"
  if  ( rows.size()==0 && idx<data.size()-1 ) rows.push_back(idx);
  // - apply previous selection, moved one up
  for ( auto &row : rows ) ui->tS_listWidget->item(row)->setSelected(true);

  // change focus
  QListWidgetItem *item = ui->tS_listWidget->item(idx);
  ui->tS_listWidget->scrollToItem(item, QAbstractItemView::EnsureVisible);
}

// =================================================================================================

void MainWindow::tW_view()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabWrite ) return;

  // update buttons
  ui->tW_comboBox     -> setEnabled( data   .size() > 0 );
  ui->tW_pushButton_write -> setEnabled( data   .size() > 0 );
  ui->tW_pushButton_clean -> setEnabled( delData.size() > 0 );

  // check to continue
  if ( data.size() == 0 ) return;

  // get the earliest data as string
  std::string str(std::ctime(&data[0].time));

  // show the earliest date
  ui->tW_lineEdit_date->setText( QString::fromStdString(str) );

  // update the list with folders to use as reference
  // - store current index
  int cur = ui->tW_comboBox->currentIndex();
  // - clear the widget
  ui->tW_comboBox->clear();
  // - add folders
  for ( auto &i : tF_lineEdits )
    if ( i->text().size() > 0 )
      ui->tW_comboBox->addItem( i->text() );
  // - set index to previous value (if possible)
  if ( cur < ui->tW_comboBox->count() )
    ui->tW_comboBox->setCurrentIndex(cur);
}

// =================================================================================================

void MainWindow::on_tF_comboBox_currentIndexChanged(int index)
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabFiles ) return;

  // set all invisible
  for ( size_t i=0; i<tF_pushButtons_nameSort.size(); ++i ) {
    tF_listWidgets         [i]->setVisible(false);
    tF_lineEdits           [i]->setVisible(false);
    tF_pushButtons_select  [i]->setVisible(false);
    tF_pushButtons_excl    [i]->setVisible(false);
    tF_pushButtons_nameSort[i]->setVisible(false);
  }

  // find the number of folders containing selected photos
  size_t folder = 0; for ( auto &file : data ) folder = std::max(folder,file.folder);

  // if request number of folders is too low -> correct
  if ( index < static_cast<int>(folder) )
  {
    index = static_cast<int>(folder);
    ui->tF_comboBox->setCurrentIndex(index);
  }

  // enable requested number of columns
  for ( int i = 0; i < std::min( index+1, static_cast<int>(tF_pushButtons_nameSort.size()) ) ; ++i )
  {
    tF_listWidgets         [i]->setVisible(true);
    tF_lineEdits           [i]->setVisible(true);
    tF_pushButtons_select  [i]->setVisible(true);
    tF_pushButtons_excl    [i]->setVisible(true);
    tF_pushButtons_nameSort[i]->setVisible(true);
  }
}

// =================================================================================================

void MainWindow::tF_addFiles(size_t folder)
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabFiles ) return;

  // if list is not empty: remove previous selection
  // -----------------------------------------------

  std::vector<int> rm;

  for ( int i = data.size()-1 ; i >= 0 ; --i )
    if ( data[i].folder == folder )
      rm.push_back(i);

  for ( auto &i : rm )
    data.erase(data.begin()+i);

  // analyze data
  // ------------

  // update camera index to the smallest possible index, store the number of cameras
  size_t camera = data_renumCamera();

  // select folder -> read files
  // ---------------------------

  // select a folder using a dialog
  // - define dialog
  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(workDir);
  dialog.setViewMode (QFileDialog::List);
  // - launch dialog and read output
  QDir dir;
  if ( dialog.exec() ) dir = dialog.directory();
  else                 return;

  // store new suggested directory
  try         { workDir = dir.filePath("../"); }
  catch (...) { workDir = workDir;             }

  // set filters
  QStringList filter_jpeg;
  QStringList filter_json;
  filter_jpeg << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG";
  filter_json << "chroto.json";
  // find all files matching the filters
  QFileInfoList lst_json = dir.entryInfoList(filter_json,QDir::Files);
  QFileInfoList lst_jpeg = dir.entryInfoList(filter_jpeg,QDir::Files);

  // if "chroto.json" was found -> read data
  json jdata;
  if ( lst_json.size()==1 ) {
    std::ifstream inp(QFileInfo(lst_json.at(0)).absoluteFilePath().toStdString());
    inp >> jdata;
  }

  // analyze files -> add to data
  // ----------------------------

  std::time_t t;
  int rot,modified;

  // read all JPG-files, if "chroto.json" exists: overwrite time from JPEG with stored values,
  // segment in the different cameras that were stored in "chroto.json"
  for ( int i = 0; i < lst_jpeg.size() ; ++i )
  {
    // - extract file from list
    QFileInfo finfo = lst_jpeg.at(i);
    // - try the read the EXIF information; if needed, fall back to basic file information
    try
    {
      modified        = 0;
      std::tie(t,rot) = jpg2info(finfo.absoluteFilePath().toStdString());
    }
    catch (...)
    {
      modified        = 1;
      rot             = 0;
      t               = finfo.created().toTime_t();
    }
    // - store information to "File" instance
    File file;
    file.camera    = camera;
    file.folder    = folder;
    file.disp      = finfo.fileName();
    file.path      = finfo.absoluteFilePath();
    file.dir       = finfo.absolutePath();
    file.time      = t;
    file.time_orig = t;
    file.rotation  = rot;
    file.modified  = modified;
    file.ithumb    = thumbnail->push_back(file.path,rot); // create new entry in thumbnail list
    // -  if JSON file was found: overwrite information
    if ( lst_json.size()==1 )
    {
      file.time     = static_cast<std::time_t>(jdata[finfo.fileName().toStdString()]["time"    ]);
      file.camera  += static_cast<size_t>     (jdata[finfo.fileName().toStdString()]["camera"  ]);
      file.modified = static_cast<int>        (jdata[finfo.fileName().toStdString()]["modified"]);
    }
    // - store in list
    data.push_back(file);
  }

  // set the thumbnail size based on the size of "data"
  if      ( data.size() <   100 ) { npix = 256; thumbnail->setSize(64); }
  if      ( data.size() <   200 ) { npix = 128; thumbnail->setSize(32); }
  else if ( data.size() <   500 ) { npix =  64; thumbnail->setSize(32); }
  else if ( data.size() <  2000 ) { npix =  32; thumbnail->setSize(16); }
  else                            { npix =  16; thumbnail->setSize(16); }

  // enforce view the first photo
  idx = 0;

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::tV_startFullScreen()
{
  // only act on correct tab, and not full screen mode
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( fullScreen ) return;

  fullScreen = true;

  ui->tV_pushButton_fullScreen->setText("Minimize");
  ui->tV_label->setWindowFlags(ui->tV_label->windowFlags() | Qt::Window);
  ui->tV_label->showFullScreen();

  QApplication::setOverrideCursor(Qt::BlankCursor);

  emit indexChanged();
}

// =================================================================================================

void MainWindow::tV_stopFullScreen()
{
  // only act on correct tab, and on full screen mode
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( ! fullScreen ) return;

  fullScreen = false;

  ui->tV_pushButton_fullScreen->setText("Full screen");
  ui->tV_label->setWindowFlags(ui->tV_label->windowFlags() & ~Qt::Window);
  ui->tV_label->show();

  QApplication::setOverrideCursor(Qt::ArrowCursor);

  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_prev_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( idx == 0 ) return;

  idx -= 1;
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_next_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( idx >= data.size()-1 ) return;

  idx += 1;
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_first_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( idx == 0 ) return;

  idx = 0;
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_last_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( idx >= data.size()-1 ) return;

  idx = data.size()-1;
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_excl_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( idx >= data.size() ) return;

  data.erase(data.begin()+idx);
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_del_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( idx >= data.size() ) return;

  delData.push_back(data[idx]);
  data.erase(data.begin()+idx);
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_undoDel_clicked()
{
  // only act on correct tab, check there is something to insert
  if ( ui->tabWidget->currentIndex() != tabView ) return;
  if ( delData.size() == 0 ) return;

  // insert last item from "delData" in "data"; and remove from "delData"
  data   .push_back(delData[delData.size()-1]       );
  delData.erase    (delData.begin()+delData.size()-1);

  // switch to inserted image
  idx = data.size()-1;

  // create new thumbnail, as it was removed
  auto file = &data[idx];
  file->ithumb = thumbnail->push_back(file->path,file->rotation);

  // signal change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_navTop_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  ui->tS_listWidget->scrollToTop();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_navBottom_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  ui->tS_listWidget->scrollToBottom();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_navPgUp_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  int i = ui->tS_listWidget->verticalScrollBar()->value();
  int N = ui->tS_listWidget->verticalScrollBar()->maximum();

  i -= N/10;

  if ( i < 0 ) i = 0;

  ui->tS_listWidget->verticalScrollBar()->setValue(i);
}

// =================================================================================================

void MainWindow::on_tS_pushButton_navPgDwn_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  int i = ui->tS_listWidget->verticalScrollBar()->value();
  int N = ui->tS_listWidget->verticalScrollBar()->maximum();

  i += N/10;

  if ( i >= N ) i = N-1;

  ui->tS_listWidget->verticalScrollBar()->setValue(i);
}

// =================================================================================================

void MainWindow::on_tS_listWidget_itemSelectionChanged()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  selLast = -1;

  // current selection
  std::vector<size_t> sel = selectedItems(ui->tS_listWidget);

  // catch all the obvious cases in which there is not one last selected item
  if ( sel.size()==0 || selPrev.size()==0 || sel.size()!=selPrev.size()+1 ) {
    selPrev = sel;
    return;
  }

  std::vector<size_t> diff(sel.size()+selPrev.size());
  std::vector<size_t>::iterator it;

  it = std::set_difference(sel.begin(),sel.end(),selPrev.begin(),selPrev.end(),diff.begin());

  diff.resize(it-diff.begin());

  selPrev = sel;

  if ( diff.size()==1 ) selLast = diff[0];
}

// =================================================================================================

void MainWindow::on_tS_pushButton_split_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // find which camera index to use (one more than the current maximum)
  size_t camera = data_nCamera();

  // change camera index
  for ( auto &i : rows ) data[i].camera = camera;

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Iexcl_clicked()
{
  // only act on correct tab, and non-empty data
  if ( ui->tabWidget->currentIndex() != tabSort ) return;
  if ( data.size() == 0 ) return;

  listExcl(ui->tS_listWidget,-1);
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Idel_clicked()
{
  // only act on correct tab, and non-empty data
  if ( ui->tabWidget->currentIndex() != tabSort ) return;
  if ( data.size() == 0 ) return;

  listDel(ui->tS_listWidget);
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Iup_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // top of the list: don't know what to do
  if ( rows[0] == 0 ) return promptWarning("Selection includes first photo, cannot proceed");

  // move up (earlier)
  for ( auto &i: rows ) {
    data[i].time    -= data[i].time-data[i-1].time+1;
    data[i].modified = 1;
  }

  // set index
  idx = rows[rows.size()-1];

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Idown_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget,false);

  // check to continue
  if ( rows.size()==0 ) return;

  // top of the list: don't know what to do
  if ( rows[0] == data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");

  // move up (earlier)
  for ( auto &i: rows ) {
    data[i].time    += data[i+1].time-data[i].time+1;
    data[i].modified = 1;
  }

  // set index
  idx = rows[0];

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Isync_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // check if there is a destination: the last selected image
  if ( selLast==-1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // move up (earlier)
  for ( auto &i: rows ) {
    data[i].time     = data[selLast].time;
    data[i].modified = 1;
  }

  // set index
  idx = rows[rows.size()-1];

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Cup_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0] == 0 )
    return promptWarning("Selection includes first photo, cannot proceed");
  if ( rows.size() > 1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( data[row-1].camera==data[row].camera )
    return promptWarning("Previous photo is not of another camera, cannot proceed");

  // get time difference
  std::time_t dt = data[row].time-data[row-1].time+1;

  // apply to all
  for ( auto &i : data )
    if ( i.camera == data[row].camera )
      i.time -= dt;

  // set index
  idx = rows[rows.size()-1];

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Cdown_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget,false);

  // check to continue
  if ( rows.size() == 0 ) return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0] >= data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");
  if ( rows.size() > 1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( data[row+1].camera == data[row].camera )
    return promptWarning("Next photo is not of another camera, cannot proceed");

  // get time difference
  std::time_t dt = data[row+1].time-data[row].time+1;

  // apply to all
  for ( auto &i : data )
    if ( i.camera == data[row].camera )
      i.time += dt;

  // set index
  idx = rows[0];

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Csync_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // check if there is a destination: the last selected image
  if ( selLast==-1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // read number of cameras
  size_t ncam = 0;
  for ( auto &i : data )
    ncam = std::max(ncam,i.camera);

  // list cameras
  std::vector<int> check(ncam+1,0);
  // check if camera occurs
  for ( auto &row : rows ) {
    if ( check[data[row].camera] )
      return promptWarning("Selection includes several photos from the same camera, cannot proceed");
    check[data[row].camera] = 1;
  }

  // sync
  size_t ref = selLast;
  for ( auto &row : rows ) {
    if ( row != ref ) {
      std::time_t dt = data[row].time-data[ref].time;
      for ( auto &i : data )
        if ( i.camera == data[row].camera )
          i.time -= dt;
    }
  }

  // set index
  idx = rows[rows.size()-1];

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Fup_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0]==0 )
    return promptWarning("Selection includes first photo, cannot proceed");
  if ( rows.size() > 1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( data[row-1].folder == data[row].folder )
    return promptWarning("Previous photo is not of another folder, cannot proceed");

  // get time difference
  std::time_t dt = data[row].time-data[row-1].time+1;

  // apply to all
  for ( auto &i : data )
    if ( i.folder == data[row].folder )
      i.time -= dt;

  // set index
  idx = rows[rows.size()-1];

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Fdown_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget,false);

  // check to continue
  if ( rows.size() == 0 ) return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0] >= data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");
  if ( rows.size() > 1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( data[row+1].folder == data[row].folder )
    return promptWarning("Next photo is not of another folder, cannot proceed");

  // get time difference
  std::time_t dt = data[row+1].time-data[row].time+1;

  // apply to all
  for ( auto &i : data )
    if ( i.folder == data[row].folder )
      i.time += dt;

  // set index
  idx = rows[0];

  // signal to process change
  emit dataChanged();
}


// =================================================================================================

void MainWindow::on_tS_pushButton_Fsync_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabSort ) return;

  // check if there is a destination: the last selected image
  if ( selLast == -1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // read number of folders
  size_t nfol = 0;
  for ( auto &i : data )
    nfol = std::max(nfol,i.folder);

  // list folders
  std::vector<int> check(nfol+1,0);
  // check if folder occurs
  for ( auto &row : rows ) {
    if ( check[data[row].folder] )
      return promptWarning("Selection includes several photos from the same folder, cannot proceed");
    check[data[row].folder] = 1;
  }

  // sync
  size_t ref = selLast;
  for ( auto &row : rows ) {
    if ( row != ref ) {
      std::time_t dt = data[row].time-data[ref].time;
      for ( auto &i : data )
        if ( i.folder == data[row].folder )
          i.time -= dt;
    }
  }

  // set index
  idx = rows[rows.size()-1];

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tW_comboBox_activated(int index)
{
  // only act on correct tab, non-empty data, selected folder
  if ( ui->tabWidget->currentIndex() != tabWrite ) return;
  if ( index < 0 ) return;
  if ( data.size() == 0 ) return;

  // local variables
  std::time_t dt   = 0; // time difference
  int         sign = 0; // sign of the time difference (std::time_t can only be positive)

  // try to find reference image in selected folder, and extract time difference between the
  // time as a result of sorting, and the original time of the image
  for ( auto &i : data ) {
    if ( static_cast<int>(i.folder) == index && !(i.modified) ) {
      if ( i.time > i.time_orig ) {
        dt   = i.time - i.time_orig;
        sign = -1;
        break;
      }
      else if ( i.time <= i.time_orig ) {
        dt   = i.time_orig - i.time;
        sign = +1;
        break;
      }
    }
  }

  if ( sign == 0 ) {
    promptWarning("All images have been manually modified, consider using different folder");
    for ( auto &i : data ) {
      if ( static_cast<int>(i.folder) == index ) {
        if ( i.time > i.time_orig ) {
          dt   = i.time - i.time_orig;
          sign = -1;
          break;
        }
        else if ( i.time <= i.time_orig ) {
          dt   = i.time_orig - i.time;
          sign = +1;
          break;
        }
      }
    }
  }

  // change the time of the sorted set
  if ( sign == 0 ) {
    promptWarning("Folder contains no image that can serve as reference, please select another");
    return;
  }

  if ( sign == +1 ) {
    for ( auto &i : data )
      i.time += dt;
  }
  else if ( sign == -1 ) {
    for ( auto &i : data )
      i.time -= dt;
  }
}

// =================================================================================================

void MainWindow::on_tW_pushButton_path_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabWrite ) return;

  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(workDir);
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();

  ui->tW_lineEdit_path->setText(dir.absolutePath());

  // store new suggested directory
  workDir = dir.absolutePath();
}

// =================================================================================================

void MainWindow::on_tW_lineEdit_path_editingFinished()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != tabWrite ) return;

  if ( ui->tW_lineEdit_path->text().length()>0 )
    ui->tW_pushButton_write->setEnabled(true);
  else
    ui->tW_pushButton_write->setEnabled(false);
}

// =================================================================================================

void MainWindow::on_tW_pushButton_write_clicked()
{
  // only act on correct tab, non-empty data
  if ( ui->tabWidget->currentIndex() != tabWrite ) return;
  if ( data.size() == 0 ) return;

  // number of characters needed the fit the photos
  // (ignore that the removed image might reduce N)
  int N = QString::number(data.size()).length();

  // create output directory, if it does not exist
  QDir outdir(ui->tW_lineEdit_path->text());
  if ( !outdir.exists() )
    QDir().mkdir(ui->tW_lineEdit_path->text());

  // check if output files exist
  // - "PATH/chroto.json"
  if ( QFileInfo(outdir.filePath("chroto.json")).isFile() ) {
    this->promptWarning(\
      tr("The file '%1' already exists, please select an empty directory").arg("chroto.json"));
    return;
  }
  // - "PATH/NAME-%0Xd.jpg"
  for ( int i=0; i<static_cast<int>(data.size()); ++i ) {
    QString fname = ui->tW_lineEdit_name->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
    if ( QFileInfo(outdir.filePath(fname)).isFile() ) {
      this->promptWarning(\
        tr("The file '%1' already exists, please select an empty directory").arg(fname));
      return;
    }
  }

  // update list with paths
  for ( auto &file : data )
    cleanPaths.push_back(file.dir);
  // convert to unique list
  cleanPaths.unique();

  // write output
  // - allocate JSON-struct
  json j;
  // - loop over photos
  for ( int i=0; i<static_cast<int>(data.size()); ++i ) {
    // - format -> filename
    QString fname = ui->tW_lineEdit_name->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
    QString fpath = outdir.filePath(fname);
    // - store information to JSON-struct
    j[fname.toStdString()]["modified"] = data[i].modified;
    j[fname.toStdString()]["camera"  ] = data[i].camera;
    j[fname.toStdString()]["time"    ] = static_cast<long>(data[i].time);
    j["camera"][std::to_string(data[i].camera)] = data[i].dir.toStdString();
    // - copy or move
    if ( ui->tW_checkBox_keepOrig->isChecked() ) { QFile::copy(  data[i].path,fpath); }
    else                                        { QFile::rename(data[i].path,fpath); }
  }

  // store "PATH/chroto.json"
  QString fpath = outdir.filePath("chroto.json");
  std::ofstream o(fpath.toStdString());
  o << std::setw(4) << j << std::endl;

  // clear data-structure
  while ( data.size()>0 )
    data.erase(data.begin());

  // clear thumbnails
  thumbnail->empty();

  // disable button (enabled when data is added)
  ui->tW_pushButton_write->setEnabled(false);
}

// =================================================================================================

void MainWindow::on_tW_pushButton_clean_clicked()
{
  // only act on correct tab, non-empty data
  if ( ui->tabWidget->currentIndex() != tabWrite ) return;
  if ( delData.size() == 0 ) return;

  // update list with paths
  for ( auto &file : delData )
    cleanPaths.push_back(file.dir);
  // convert to unique list
  cleanPaths.unique();

  // remove deleted photos
  for ( auto &file : delData )
    QFile::remove(file.path);

  // list with useless system files
  std::vector<QString> sfiles;
  sfiles.push_back(".DS_Store");
  sfiles.push_back("Thumbs.db");

  // visit all folders that had been selected
  for ( auto dir : cleanPaths ) {
    // - remove useless system files
    for ( auto file : sfiles )
      QFile::remove(QDir(dir).filePath(file));
    // - remove empty folders
    if ( QDir(dir).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).isEmpty() )
      QDir(dir).removeRecursively();
  }

  // clear lists
  while ( cleanPaths.size()>0 ) cleanPaths.erase(cleanPaths.begin());
  while ( delData   .size()>0 ) delData   .erase(delData   .begin());

  // disable button (enabled when data is added)
  ui->tW_pushButton_clean->setEnabled(false);
}

