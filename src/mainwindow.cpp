/* =================================================================================================

(c - GPLv3) T.W.J. de Geus | tom@geus.me | www.geus.me | github.com/tdegeus/chroto

================================================================================================= */

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
  m_thumnails = new Thumbnails;
  m_thumnails->moveToThread(&workerThread);
  // list of thumbnails is deleted only when the worker-thread is deleted (in destructor below)
  connect(&workerThread,&QThread::finished,m_thumnails,&QObject::deleteLater);

  // set basic colormap (dynamically extended below, if needed)
  m_col.push_back( QColor(  72,   8,   7 ) );
  m_col.push_back( QColor( 109,   1,  78 ) );
  m_col.push_back( QColor( 136,  14,   0 ) );
  m_col.push_back( QColor( 124,  96,   0 ) );
  m_col.push_back( QColor(   8,  89,   8 ) );
  m_col.push_back( QColor(  24,  63,  63 ) );
  m_col.push_back( QColor(  29,  29,  29 ) );
  m_col.push_back( QColor(  22,  64 ,124 ) );
  m_col.push_back( QColor(   8,   0,  79 ) );
  m_col.push_back( QColor(  92,  92, 134 ) );
  m_col.push_back( QColor(  68,  47,  35 ) );
  m_col.push_back( QColor( 100, 100, 100 ) );

  // Tab::Files : fill arrays collecting the file-lists and related buttons
  m_tF_listWidgets         .push_back(ui->tF_listWidget_0          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_1          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_2          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_3          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_4          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_5          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_6          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_7          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_8          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_9          );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_10         );
  m_tF_listWidgets         .push_back(ui->tF_listWidget_11         );
  // --
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_0            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_1            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_2            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_3            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_4            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_5            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_6            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_7            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_8            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_9            );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_10           );
  m_tF_lineEdits           .push_back(ui->tF_lineEdit_11           );
  // --
  m_tF_labels_N            .push_back(ui->tF_label_N_0             );
  m_tF_labels_N            .push_back(ui->tF_label_N_1             );
  m_tF_labels_N            .push_back(ui->tF_label_N_2             );
  m_tF_labels_N            .push_back(ui->tF_label_N_3             );
  m_tF_labels_N            .push_back(ui->tF_label_N_4             );
  m_tF_labels_N            .push_back(ui->tF_label_N_5             );
  m_tF_labels_N            .push_back(ui->tF_label_N_6             );
  m_tF_labels_N            .push_back(ui->tF_label_N_7             );
  m_tF_labels_N            .push_back(ui->tF_label_N_8             );
  m_tF_labels_N            .push_back(ui->tF_label_N_9             );
  m_tF_labels_N            .push_back(ui->tF_label_N_10            );
  m_tF_labels_N            .push_back(ui->tF_label_N_11            );
  // --
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_0   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_1   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_2   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_3   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_4   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_5   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_6   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_7   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_8   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_9   );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_10  );
  m_tF_pushButtons_select  .push_back(ui->tF_pushButton_select_11  );
  // --
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_0     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_1     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_2     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_3     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_4     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_5     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_6     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_7     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_8     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_9     );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_10    );
  m_tF_pushButtons_excl    .push_back(ui->tF_pushButton_excl_11    );
  // --
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_0 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_1 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_2 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_3 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_4 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_5 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_6 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_7 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_8 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_9 );
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_10);
  m_tF_pushButtons_nameSort.push_back(ui->tF_pushButton_nameSort_11);

  // Tab::Files : link scroll position listWidgets
  for ( auto &i: m_tF_listWidgets )
    for ( auto &j: m_tF_listWidgets )
      if ( i!=j )
        connect(i->verticalScrollBar(),SIGNAL(valueChanged(int)),
                j->verticalScrollBar(),SLOT  (setValue    (int)));

  // Tab::View : center image
  ui->tV_label->setAlignment(Qt::AlignCenter);

  // Tab::Files : select folder / remove selected files / sort by name / sync selection
  for ( size_t i = 0 ; i < m_tF_pushButtons_select.size() ; ++i )
  {
    connect( m_tF_pushButtons_select  [i], &QPushButton::clicked, [=](){ tF_addFiles  (i); } );
    connect( m_tF_pushButtons_nameSort[i], &QPushButton::clicked, [=](){ tF_nameSort  (i); } );
    connect( m_tF_pushButtons_excl    [i], &QPushButton::clicked, [=](){ tF_excludeSel(i); } );
    connect( m_tF_listWidgets[i], &QListWidget::itemSelectionChanged, [=](){tF_unifySelection(i);});
  }

  // "m_data" changed -> sort by time and refresh view in relevant tab
  connect( this, SIGNAL( dataChanged() ), this, SLOT( dataUpdate() ) );

  // Tab::Sort : "read" thumbnails, update "m_data" / refresh thumbnails
  connect( this       , SIGNAL( thumbnailRead() ), m_thumnails , SLOT( read      () ) );
  connect( m_thumnails, SIGNAL( completed    () ), this        , SLOT( dataUpdate() ) );

  // Tab::Sort : manually refresh view to show the latest thumbnails
  connect( ui->tS_pushButton_refresh, SIGNAL( clicked(bool) ), SLOT( tS_view() ) );

  // tab changed: show introduction, clear existing selection, process "m_data" (see above)
  connect( ui->tabWidget, &QTabWidget::currentChanged, [=](){ instruction       (); } );
  connect( ui->tabWidget, &QTabWidget::currentChanged, [=](){ selectionClearAll (); } );
  connect( ui->tabWidget, &QTabWidget::currentChanged, [=](){ dataUpdate        (); } );

  // Tab::View : "m_idx" or full-screen changed -> update view
  connect( this, SIGNAL( indexChanged() ), this, SLOT( tV_view() ) );

  // Tab::Files : convert selected item to current index
  for ( auto &i: m_tF_listWidgets )
    connect(i,&QListWidget::itemSelectionChanged,[=](){selection2idx(i);});

  // Tab::Sort : convert selected item to current index
  connect( ui->tS_listWidget, &QListWidget::itemSelectionChanged,
    [=](){selection2idx(ui->tS_listWidget);});

  // Tab::Write : couple "outPath" pushButton to "outPath" lineEdit
  connect( ui->tW_pushButton_path,SIGNAL(clicked(bool)),SLOT(on_tW_lineEdit_path_editingFinished()));

  // start fresh: clear "m_data", "m_dataDel", and widgets; set default values for state-variables
  connect( ui->actionNew, &QAction::triggered, [=](){ resetApp(true); } );

  // Tab::View : connect full screen button
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

  // initialize clear application
  resetApp();

  // start worker-thread (to which "m_thumnails" is assigned)
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

void MainWindow::selection2idx(QListWidget *list)
{
  // get a list with selected items
  std::vector<size_t> sel = selectedItems(list);

  // new "idx": default equal to "m_idx", try to change below
  size_t idx = m_idx;

  // try to take from selection
  if ( sel.size() > 0       ) idx = sel[0];
  // fix possible out-of-bounds
  if ( idx >= m_data.size() ) idx = m_data.size()-1;

  // no change: do nothing
  if ( idx == m_idx ) return;

  // change current item
  m_idx = idx;

  // emit signal to process the change
  emit indexChanged();
}

// =================================================================================================

void MainWindow::selectionClear(QListWidget *list)
{
  for ( int j = 0 ; j < list->count() ; ++j )
    list->item(j)->setSelected(false);
}

// =================================================================================================

void MainWindow::selectionClearAll()
{
  // all lists on Tab::Files
  for ( auto &list: m_tF_listWidgets ) selectionClear(list);

  // list of Tab::Sort
  selectionClear(ui->tS_listWidget);
}

// =================================================================================================

void MainWindow::resetApp(bool prompt)
{
  // hide button if compiled without exiv2
  #ifndef WITHEXIV2
  ui->tW_checkBox_exif->setVisible(false);
  #endif

  // prompt user for confirmation
  if ( prompt )
    if ( !promptQuestion("This clears everything, to start fresh. Continue?") )
      return;

  // remove all data
  while ( m_data   .size()>0 ) { m_data   .pop_back(); }
  while ( m_dataDel.size()>0 ) { m_dataDel.pop_back(); }

  // Tab::Write : (re)set all widgets to default
  ui->tW_lineEdit_date     -> setText("");
  ui->tW_lineEdit_path     -> setText("");
  ui->tW_lineEdit_name     -> setText("sorted");
  ui->tW_checkBox_keepOrig -> setChecked(false);
  ui->tW_pushButton_write  -> setEnabled(false);

  // (re)set state variables
  m_fullScreen = false ;
  m_npix       =  64   ;
  m_max_fol    =  12   ;
  m_nfol       =   0   ;
  m_ncam       =   0   ;
  m_idx        =   0   ;
  m_selLast    =  -1   ;
  m_workDir    = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

  // enforce opening on the correct tab
  // - avoid emitting "QTabWidget::currentChanged" signal
  QSignalBlocker blocker( ui->tabWidget );
  // - change tab
  ui->tabWidget->setCurrentIndex( Tab::Files );

  // write instruction to status-bar
  instruction();

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::instruction()
{
  QString text;

  switch( ui->tabWidget->currentIndex() )
  {
    case Tab::Files:
      text  = "Store the photos of each camera in a separate folder. Select each folder here.";
      break;
    case Tab::View:
      text  = "Check out your great photos! Also time to delete less successful experiments.";
      break;
    case Tab::Sort:
      text  = "Correct the order of photos if needed. Strategy: move entire folders/cameras first.";
      text += " Try to move towards a correct reference.";
      break;
    case Tab::Write:
      text  = "Write the sorted photos. Use clean-up to actually remove all 'deleted' photos.";
      break;
    default:
      text = "";
      break;
  }

  ui->statusBar->showMessage(text);
}

// =================================================================================================

void MainWindow::dataUpdate()
{
  // request to stop reading thumbnails (to lose fewer time below)
  m_thumnails->requestStop();

  // initialize counters
  m_ncam = 0;
  m_nfol = 0;

  // update camera index to the smallest possible index
  if ( m_data.size() > 0 )
  {
    // - logical list: 1 == camera is currently in use; 0 == camera is not in use (default)
    std::vector<int> cam( m_data.size() , 0 );
    // - fill
    for ( auto &i : m_data ) cam[i.camera] = 1;
    // - convert to indices, step 1/2: first index starts at 0
    cam[0] -= 1;
    // - convert to indices, step 2/2: cumulative sum
    for ( size_t i=1; i<cam.size(); ++i ) cam[i] += cam[i-1];
    // - renumber m_data
    for ( auto &i : m_data ) i.camera = cam[i.camera];
    // - store the number of cameras
    m_ncam = ( *std::max_element(cam.begin(), cam.end() ) ) + 1;
  }

  // update folder index to the smallest possible index
  if ( m_data.size() > 0 )
  {
    // - logical list: 1 == folder is currently in use; 0 == folder is not in use (default)
    std::vector<int> fol( m_data.size() , 0 );
    // - fill
    for ( auto &i : m_data ) fol[i.folder] = 1;
    // - convert to indices, step 1/2: first index starts at 0
    fol[0] -= 1;
    // - convert to indices, step 2/2: cumulative sum
    for ( size_t i=1; i<fol.size(); ++i ) fol[i] += fol[i-1];
    // - renumber m_data
    for ( auto &i : m_data ) i.folder = fol[i.folder];
    // - store the number of cameras
    m_nfol = ( *std::max_element(fol.begin(), fol.end() ) ) + 1;
  }

  // add colors if needed
  if ( m_ncam > m_col.size() )
  {
    // - remove everything that is not part of the basic colormap
    while ( m_col.size() > m_max_fol ) m_col.erase(m_col.begin()+m_col.size()-1);
    // - extend colormap
    std::vector<int> cmap = cppcolormap::Set1(m_ncam-m_max_fol);
    for ( size_t i = 0 ; i < m_ncam-m_max_fol ; ++i )
      m_col.push_back( QColor( cmap[i*3+0], cmap[i*3+1], cmap[i*3+2] ) );
  }

  // correct current index if needed (out-of-bounds -> last)
  if ( m_idx >= m_data.size() ) m_idx = m_data.size()-1;

  // sort by time
  if ( m_data.size() > 0 )
  {
    // - store current order, to retrieve the new position of "m_idx"
    for ( size_t i = 0 ; i < m_data.size() ; ++i ) m_data[i].index = i;
    // - sort chronologically (if the times are identical retain existing order)
    std::sort(m_data.begin(),m_data.end(),
      [](File i,File j){
        if ( i.t == j.t ) return i.index < j.index;
        else              return i.t     < j.t;
    });
    // - locate new position of "m_idx"
    for ( size_t i=0; i<m_data.size(); ++i ) {
      if ( m_data[i].index==m_idx ) {
        m_idx = i;
        break;
      }
    }
    // - make sure that all photos are at least one second apart (to allow image insertion)
    for ( size_t i = 0 ; i < m_data.size()-1 ; ++i )
      if ( m_data[i+1].t <= m_data[i].t )
        m_data[i+1].t = m_data[i].t+std::chrono::duration<int>(1);
  }

  // remove thumbnails that are not in "m_data", update index in "m_data" accordingly
  if ( m_thumnails->size() > 0 )
  {
    // - initialize
    std::vector<int>    incl( m_thumnails->size() , 0 ); // 1 if the m_thumnails is part of "m_data"
    std::vector<size_t> rm;                            // list with thumbnails to remove
    // - fill
    for ( auto &i : m_data ) incl[i.ithumb] = 1;
    // - list items to remove
    for ( size_t i=0; i<incl.size(); ++i ) if ( incl[i]==0 ) rm.push_back(i);
    // - convert to indices, step 1/2: first index starts at 0
    incl[0] -= 1;
    // - convert to indices, step 2/2: cumulative sum
    for ( size_t i=1; i<incl.size(); ++i ) incl[i] += incl[i-1];
    // - renumber m_data
    for ( auto &i : m_data ) i.ithumb = incl[i.ithumb];
    // - remove thumbnails that are no longer part of "m_data"
    m_thumnails->erase(rm);
    // - if needed, launch reading thumbnails (that have not been read)
    if ( m_thumnails->unread() ) { emit thumbnailRead(); }
  }

  // update widget
  if      ( ui->tabWidget->currentIndex() == Tab::Files ) tF_view();
  else if ( ui->tabWidget->currentIndex() == Tab::View  ) tV_view();
  else if ( ui->tabWidget->currentIndex() == Tab::Sort  ) tS_view();
  else if ( ui->tabWidget->currentIndex() == Tab::Write ) tW_view();
}

// =================================================================================================

void MainWindow::tF_view()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Files ) return;

  // handle visibility: only the first "m_nfol + 1" folders will be visible
  for ( size_t ifol = 0 ; ifol < m_max_fol ; ++ifol ) {
    m_tF_listWidgets         [ifol]->setVisible(ifol<=m_nfol);
    m_tF_labels_N            [ifol]->setVisible(ifol<=m_nfol);
    m_tF_lineEdits           [ifol]->setVisible(ifol<=m_nfol);
    m_tF_pushButtons_select  [ifol]->setVisible(ifol<=m_nfol);
    m_tF_pushButtons_excl    [ifol]->setVisible(ifol<=m_nfol);
    m_tF_pushButtons_nameSort[ifol]->setVisible(ifol<=m_nfol);
  }

  // enable all buttons of folders containing photos, rename selection button
  for ( size_t ifol = 0 ; ifol < m_nfol ; ++ifol ) {
    m_tF_listWidgets         [ifol]->setEnabled(true);
    m_tF_labels_N            [ifol]->setEnabled(true);
    m_tF_lineEdits           [ifol]->setEnabled(true);
    m_tF_pushButtons_select  [ifol]->setEnabled(true);
    m_tF_pushButtons_excl    [ifol]->setEnabled(true);
    m_tF_pushButtons_nameSort[ifol]->setEnabled(true);
    m_tF_pushButtons_select  [ifol]->setText("Exclude folder");
  }

  // enable selection button of the next folder and rename it, disable all other widgets
  for ( size_t ifol = m_nfol ; ifol < m_max_fol ; ++ifol ) {
    m_tF_listWidgets         [ifol]->setEnabled(false);
    m_tF_labels_N            [ifol]->setEnabled(false);
    m_tF_lineEdits           [ifol]->setEnabled(false);
    m_tF_pushButtons_select  [ifol]->setEnabled(true );
    m_tF_pushButtons_excl    [ifol]->setEnabled(false);
    m_tF_pushButtons_nameSort[ifol]->setEnabled(false);
    m_tF_labels_N            [ifol]->setText("");
    m_tF_pushButtons_select  [ifol]->setText("Select folder");
  }

  // empty storage paths
  for ( auto line : m_tF_lineEdits ) line->clear();
  // empty listWidgets
  for ( auto list : m_tF_listWidgets ) while ( list->count()>0 ) list->takeItem(0);

  // no data -> quit this function
  if ( m_data.size() == 0 ) return;

  // fill listWidgets & set basic information
  for ( size_t ifol = 0 ; ifol < m_nfol ; ++ifol )
  {
    // - zero initialize number of cameras and number pictures in this folder
    std::vector<size_t> cam ( m_ncam , 0 );
    size_t npic = 0;

    // - loop over the data
    for ( size_t i = 0 ; i < m_data.size() ; ++i )
    {
      // -- file in folder
      if ( m_data[i].folder == ifol )
      {
        // --- write file and folder name
        m_tF_listWidgets[ifol]->addItem(m_data[i].fname);
        m_tF_lineEdits  [ifol]->setText(m_data[i].dir  );
        // -- set color
        if ( m_ncam > 1 )
          m_tF_listWidgets[ifol]->item(i)->setBackground(QBrush(m_col[m_data[i].camera]));
        // --- signal usage of camera
        cam[m_data[i].camera] = 1;
        // --- add one picture
        npic++;
      }
      // -- file not in folder
      else
      {
        // --- write empty line
        m_tF_listWidgets[ifol]->addItem("");
      }
    }

    // - block the widget from sending signals
    QSignalBlocker blocker( m_tF_listWidgets[ifol] );

    // - change the current item to "m_idx"
    m_tF_listWidgets[ifol]->setCurrentRow(m_idx);

    // - compute the number of cameras in this folder
    size_t ncam = std::accumulate(cam.begin(), cam.end(), 0);

    // - write basic information
    // -- initialize text
    QString text = "";
    // -- add number of photos
    if ( npic == 1 ) text += QString("%1 photo - " ).arg(npic);
    else             text += QString("%1 photos - ").arg(npic);
    // -- add number of cameras
    if ( ncam == 1 ) text += QString("%1 camera" ).arg(ncam);
    else             text += QString("%1 cameras").arg(ncam);
    // -- write to widget
    m_tF_labels_N[ifol]->setText(text);
  }
}

// =================================================================================================

void MainWindow::tV_view()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;

  // selectively disable/enable buttons
  ui->tV_pushButton_prev       -> setEnabled( m_data   .size() > 0 && m_idx > 0               );
  ui->tV_pushButton_first      -> setEnabled( m_data   .size() > 0 && m_idx > 0               );
  ui->tV_pushButton_next       -> setEnabled( m_data   .size() > 0 && m_idx < m_data.size()-1 );
  ui->tV_pushButton_last       -> setEnabled( m_data   .size() > 0 && m_idx < m_data.size()-1 );
  ui->tV_pushButton_undoDel    -> setEnabled( m_dataDel.size() > 0                            );
  ui->tV_pushButton_fullScreen -> setEnabled( m_data   .size() > 0                            );
  ui->tV_pushButton_del        -> setEnabled( m_data   .size() > 0                            );
  ui->tV_pushButton_excl       -> setEnabled( m_data   .size() > 0                            );
  ui->tV_dateTimeEdit          -> setEnabled( m_data   .size() > 0                            );

  // clear currently viewed photos
  ui->tV_label->clear();

  // check to continue
  if ( m_data.size() == 0 ) return;

  // write basic information
  ui->tV_label_index ->setText(QString("%1 / ").arg(m_idx+1)+QString("%1").arg(m_data.size()));
  ui->tV_label_folder->setText(m_data[m_idx].disp);

  // set the current time of the photo
  // - prevent signal emission
  QSignalBlocker blocker( ui->tV_dateTimeEdit );
  // - convert the Qt format
  QDateTime t;
  t.setTimeSpec(Qt::UTC);
  t.setSecsSinceEpoch(m_data[m_idx].t.time_since_epoch().count());
  // - store to widget
  ui->tV_dateTimeEdit->setDateTime(t);

  // view current photo "m_idx"
  QPixmap    p(m_data[m_idx].path);
  int        w = ui->tV_label->width();
  int        h = ui->tV_label->height();
  QTransform transform;
  QTransform trans = transform.rotate(m_data[m_idx].rotation);
  ui->tV_label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
}

// =================================================================================================

void MainWindow::tS_view(void)
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // selective enable buttons
  ui->tS_pushButton_refresh -> setEnabled( m_data.size() > 0 );
  ui->tS_pushButton_Idel    -> setEnabled( m_data.size() > 0 );
  ui->tS_pushButton_Iexcl   -> setEnabled( m_data.size() > 0 );
  ui->tS_pushButton_Iup     -> setEnabled( m_data.size() > 1 );
  ui->tS_pushButton_Idown   -> setEnabled( m_data.size() > 1 );
  ui->tS_pushButton_Isync   -> setEnabled( m_data.size() > 1 );
  ui->tS_pushButton_split   -> setEnabled( m_data.size() > 1 );
  ui->tS_pushButton_Cup     -> setEnabled( m_ncam        > 1 );
  ui->tS_pushButton_Cdown   -> setEnabled( m_ncam        > 1 );
  ui->tS_pushButton_Csync   -> setEnabled( m_ncam        > 1 );
  ui->tS_pushButton_Fup     -> setEnabled( m_nfol        > 1 );
  ui->tS_pushButton_Fdown   -> setEnabled( m_nfol        > 1 );
  ui->tS_pushButton_Fsync   -> setEnabled( m_nfol        > 1 );

  // list with selected rows
  std::vector<size_t> old = selectedItems(ui->tS_listWidget);
  // convert rows to new index
  // - allocate
  std::vector<size_t> renum(m_data.size());
  // - fill
  for ( size_t i = 0 ; i < m_data.size() ; ++i ) renum[m_data[i].index] = i;
  // - new rows
  std::vector<size_t> rows;
  // - renumber
  for ( auto &i : old ) rows.push_back(renum[i]);

  // empty the list
  while ( ui->tS_listWidget->count() > 0 ) ui->tS_listWidget->takeItem(0);

  // check to continue
  if ( m_data.size() == 0 ) return;

  // define style of the widget
  ui->tS_listWidget->setIconSize(QSize(m_npix,m_npix));

  // fill the lists with names / icons
  for ( auto &i : m_data )
    ui->tS_listWidget->addItem(new QListWidgetItem(m_thumnails->at(i.ithumb),i.disp));

  // set background color, corresponding to the camera index
  if ( m_ncam > 1 )
    for ( size_t i = 0 ; i < m_data.size() ; ++i )
      ui->tS_listWidget->item(i)->setBackground(QBrush(m_col[m_data[i].camera]));

  // restore selection
  // - clear entire selection
  selectionClear(ui->tS_listWidget);
  // - no rows selected -> set "m_idx"
  if  ( rows.size()==0 && m_idx<m_data.size()-1 ) rows.push_back(m_idx);
  // - apply previous selection, moved one up
  for ( auto &row : rows ) ui->tS_listWidget->item(row)->setSelected(true);

  // change focus
  QListWidgetItem *item = ui->tS_listWidget->item(m_idx);
  ui->tS_listWidget->scrollToItem(item, QAbstractItemView::EnsureVisible);
}

// =================================================================================================

void MainWindow::tW_view()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Write ) return;

  // update buttons
  ui->tW_pushButton_write -> setEnabled( m_data   .size() > 0 );
  ui->tW_pushButton_clean -> setEnabled( m_dataDel.size() > 0 );

  // check to continue
  if ( m_data.size() == 0 ) return;

  // get the time earliest of the earliest file as string
  // - allocate string stream
  std::ostringstream oss;
  // - convert time
  oss << date::format("%Y:%m:%d %H:%M:%S", m_data[0].t);
  // - convert to string
  auto str = oss.str();

  // show the earliest date
  ui->tW_lineEdit_date->setText( QString::fromStdString(str) );
}

// =================================================================================================

void MainWindow::tF_unifySelection(size_t ifol)
{
  // get selected items
  std::vector<size_t> rows = selectedItems(m_tF_listWidgets[ifol]);

  // set selected items of all other folders
  for ( size_t jfol = 0 ; jfol < m_nfol ; ++jfol )
  {
    if ( jfol != ifol )
    {
      // - keep the widget from emitting signals
      QSignalBlocker blocker( m_tF_listWidgets[jfol] );
      // - clear entire selection
      selectionClear(m_tF_listWidgets[jfol]);
      // - apply selection
      for ( auto &row : rows )
        m_tF_listWidgets[jfol]->item(row)->setSelected(true);
    }
  }
}

// =================================================================================================

void MainWindow::tF_excludeSel(size_t ifol)
{
  // get a list with selected items
  std::vector<size_t> index = selectedItems(m_tF_listWidgets[ifol],false);

  // proceed only for non-empty lists
  if ( index.size() <= 0 ) return;

  // remove indices which are not part of the current folder
  {
    // - allocate items to take from "index"
    std::vector<size_t> rm;
    // - determine which items to take from "index"
    for ( size_t i = index.size() ; i-- > 0 ; )
      if ( m_data[index[i]].folder != ifol )
        rm.push_back(i);
    // - take from "index"
    for ( auto &i : rm ) index.erase(index.begin()+i);
  }

  // exclude images: remove from m_data
  for ( auto &i : index ) m_data.erase(m_data.begin()+i);

  // empty selection
  selectionClear(m_tF_listWidgets[ifol]);

  // set number current item to close to the selection
  if ( index[index.size()-1] > 0 ) m_idx = index[index.size()-1]-1;
  else                             m_idx = 0;

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::tF_addFiles(size_t ifol)
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Files ) return;

  // if list is not empty: remove previous selection & quit
  // ------------------------------------------------------

  if ( m_data.size() > 0 )
  {
    // - allocate list with items to remove from "m_data"
    std::vector<size_t> rm;
    // - check "m_data" for occurrences of "ifol"
    for ( size_t i = m_data.size() ; i-- > 0 ; )
      if ( m_data[i].folder == ifol )
        rm.push_back(i);
    // - "ifol" not empty
    if ( rm.size() > 0 )
    {
      // -- remove all occurrences of "ifol" from "m_data"
      for ( auto &i : rm ) m_data.erase(m_data.begin()+i);
      // -- signal change of "m_data"
      emit dataChanged();
      // -- quit function
      return;
    }
  }

  // select folder -> read files
  // ---------------------------

  // select a folder using a dialog
  // - allocate
  QFileDialog dialog(this);
  QDir        dir;
  // - define dialog
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(m_workDir);
  dialog.setViewMode (QFileDialog::List);
  // - launch dialog and read output
  if ( dialog.exec() ) dir = dialog.directory();
  else                 return;
  // - store new suggested directory
  try         { m_workDir = dir.filePath("../"); }
  catch (...) { m_workDir = m_workDir;             }

  // read files: all "*.jpg" files and if it exists "chroto.json"
  // - set filters
  QStringList filter_jpeg;
  QStringList filter_json;
  filter_jpeg << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG";
  filter_json << "chroto.json";
  // - find all files matching the filters
  QFileInfoList lst_json = dir.entryInfoList(filter_json,QDir::Files);
  QFileInfoList lst_jpeg = dir.entryInfoList(filter_jpeg,QDir::Files);
  // - read "chroto.json" if it was found
  json jdata;
  if ( lst_json.size()==1 ) {
    std::ifstream inp(QFileInfo(lst_json.at(0)).absoluteFilePath().toStdString());
    inp >> jdata;
  }

  // analyze files -> add to "m_data"
  // --------------------------------

  // read number of images
  size_t N = static_cast<size_t>( lst_jpeg.size() );

  // read all JPG-files, if "chroto.json" exists: overwrite time from JPEG with stored values,
  // segment in the different cameras that were stored in "chroto.json"
  for ( size_t i = 0; i < N ; ++i )
  {
    // - extract file from list
    QFileInfo finfo = lst_jpeg.at(i);
    // - store information to "File" instance
    File file;
    file.camera = m_ncam;
    file.folder = ifol;
    file.fname  = finfo.fileName();
    file.path   = finfo.absoluteFilePath();
    file.dir    = finfo.absolutePath();
    // - try the read the EXIF information (stored directly); fall back to basic file information
    if ( !file.readinfo() )
    {
      file.rotation = 0;
      file.t = date::sys_seconds(std::chrono::duration<std::time_t>(finfo.created().toTime_t()));
    }

    // -  if JSON file was found: overwrite information
    if ( lst_json.size() == 1 )
    {
      // -- modified time
      if ( jdata[finfo.fileName().toStdString()].count("time") )
      {
        std::string str = jdata[finfo.fileName().toStdString()]["time"];
        std::istringstream iss{str};
        iss >> date::parse("%Y:%m:%d %H:%M:%S", file.t);
      }
      // --- camera index
      if ( jdata[finfo.fileName().toStdString()].count("camera") )
      {
        file.camera  += static_cast<size_t>(jdata[finfo.fileName().toStdString()]["camera"]);
      }
      // -- rotation
      if ( jdata[finfo.fileName().toStdString()].count("rotation") )
      {
        file.rotation = static_cast<int>(jdata[finfo.fileName().toStdString()]["rotation"]);
      }
    }
    // create new entry in m_thumnails list
    file.ithumb = m_thumnails->push_back(file.path,file.rotation);
    // - store in list
    m_data.push_back(file);
  }

  // update app settings
  // -------------------

  // find the display name
  if ( m_data.size() > 0 )
  {
    // - initialize common path
    std::string path = m_data[0].path.toStdString();
    // - compute common path
    for ( auto &i : m_data )
      path = commonPath(path,i.path.toStdString(),"/");
    // - remove path from names
    for ( auto &i : m_data )
      i.disp = QString::fromStdString( removePath(path,i.path.toStdString()) );
  }

  // set the m_thumnails size based on the size of "m_data"
  if      ( m_data.size() <   100 ) { m_npix = 256; m_thumnails->setResolution(64); }
  if      ( m_data.size() <   200 ) { m_npix = 128; m_thumnails->setResolution(32); }
  else if ( m_data.size() <   500 ) { m_npix =  64; m_thumnails->setResolution(32); }
  else if ( m_data.size() <  2000 ) { m_npix =  32; m_thumnails->setResolution(16); }
  else                              { m_npix =  16; m_thumnails->setResolution(16); }

  // enforce view the first photo
  m_idx = 0;

  // write to status-bar
  // - initialize message
  QString text;
  text = QString("Read %1 photos").arg(N) + QString(" (total %1 photos).").arg(m_data.size());
  // - add message to warn the user
  if ( m_nfol == m_max_fol-1 )
    text += " The next folder is the last folder. Just sort and write, then read and add more!";
  // - add to status-bar
  ui->statusBar->showMessage(text);

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::tF_nameSort(size_t ifol)
{
  // check to continue
  if ( m_data.size() == 0 ) return;

  // clear all selected items
  selectionClearAll();

  // store current order, to retrieve the new position of "m_idx"
  for ( size_t i = 0 ; i < m_data.size() ; ++i ) m_data[i].index = i;

  // only sort for this folder (items with "m_data[i].sort == false" are left untouched)
  for ( auto &file : m_data ) {
    if ( file.folder==ifol ) file.sort = true ;
    else                     file.sort = false;
  }

  // apply selective sort, based on file-name
  std::sort(m_data.begin(),m_data.end(),
    [](File i,File j){
      if ( i.sort && j.sort ) { return i.path.toStdString()<j.path.toStdString(); }
      else                    { return i.index             <j.index             ; }
  });

  // update time such that the sorted list is also in chronological order
  // N.B. since the real time is unknown an assumption is made
  for ( size_t i = m_data.size()-1 ; i > 0 ; --i )
    if ( m_data[i-1].t > m_data[i].t )
      m_data[i-1].t = m_data[i].t-std::chrono::duration<int>(1);

  // locate new position of "m_idx"
  for ( size_t i = 0 ; i < m_data.size() ; ++i ) {
    if ( m_data[i].index == m_idx ) {
      m_idx = i;
      emit dataChanged();
      return;
    }
  }

  // nothing found: force to view from the first index
  m_idx = 0;

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::tV_startFullScreen()
{
  // only act on correct tab, and not full screen mode
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_fullScreen ) return;

  m_fullScreen = true;

  ui->tV_pushButton_fullScreen->setText("Minimize");
  ui->tV_label->setWindowFlags(ui->tV_label->windowFlags() | Qt::Window);
  ui->tV_label->showFullScreen();

  QApplication::setOverrideCursor(Qt::BlankCursor);

  // emit signal to process the change
  emit indexChanged();
}

// =================================================================================================

void MainWindow::tV_stopFullScreen()
{
  // only act on correct tab, and on full screen mode
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( ! m_fullScreen ) return;

  m_fullScreen = false;

  ui->tV_pushButton_fullScreen->setText("Full screen");
  ui->tV_label->setWindowFlags(ui->tV_label->windowFlags() & ~Qt::Window);
  ui->tV_label->show();

  QApplication::setOverrideCursor(Qt::ArrowCursor);

  // emit signal to process the change
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_dateTimeEdit_editingFinished()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx >= m_data.size()-1 ) return;

  // read the time shift from the new date
  // - read new date
  QDateTime tm = ui->tV_dateTimeEdit->dateTime();
  // - do not account for any time zone
  tm.setTimeSpec(Qt::UTC);
  // - convert to correct format
  date::sys_seconds t = date::sys_seconds(
    std::chrono::duration<qint64>(tm.toSecsSinceEpoch())
  );
  // - convert to time shift
  std::chrono::duration<int> dt = t - m_data[m_idx].t;

  // check if time is different
  if ( t == m_data[m_idx].t ) return;

  // open a dialog to prompt how to apply the change in time (not,photo,camera,folder,all)
  DateTimeChangedDialog dialog;
  // set time to guide the user
  // - original time
  {
    // -- allocate string stream
    std::ostringstream oss;
    // -- convert time
    oss << date::format("%a %Y:%m:%d %H:%M:%S", m_data[m_idx].t);
    // -- convert to string
    auto str = oss.str();
    // -- apply to widget
    dialog.setTimeOld(QString::fromStdString( str ));
  }
  // - new time
  {
    // -- allocate string stream
    std::ostringstream oss;
    // -- convert time
    oss << date::format("%a %Y:%m:%d %H:%M:%S", t);
    // -- convert to string
    auto str = oss.str();
    // -- apply to widget
    dialog.setTimeNew(QString::fromStdString( str ));
  }

  // launch dialog
  dialog.exec();

  // cancel -> do nothing, and signal to refresh the view to view the original date
  if ( dialog.getResponse() == CalResponse::Cancel )
  {
    emit indexChanged();
    return;
  }

  // apply time shift to all (relevant) photos
  if ( dialog.getResponse() == CalResponse::Camera )
  {
    for ( size_t i = 0 ; i < m_data.size() ; ++i )
      if ( m_data[i].camera == m_data[m_idx].camera )
        m_data[i].t += dt;
  }
  else if ( dialog.getResponse() == CalResponse::Folder )
  {
    for ( size_t i = 0 ; i < m_data.size() ; ++i )
      if ( m_data[i].folder == m_data[m_idx].folder )
        m_data[i].t += dt;
  }
  else if ( dialog.getResponse() == CalResponse::All )
  {
    for ( size_t i = 0 ; i < m_data.size() ; ++i )
      m_data[i].t += dt;
  }
}

// =================================================================================================

void MainWindow::on_tV_pushButton_fromJpeg_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx >= m_data.size()-1 ) return;

  // check if photo contains valid time information
  if ( m_data[m_idx].t0.time_since_epoch().count() == 0 )
  {
    promptWarning("JPEG does not contain a valid time string, set time manually");
    return;
  }

  // check if time is different
  if ( m_data[m_idx].t == m_data[m_idx].t0 ) return;

  // read the time shift
  std::chrono::duration<int> dt = m_data[m_idx].t0 - m_data[m_idx].t;

  // open a dialog to prompt how to apply the change in time (not,photo,camera,folder,all)
  DateTimeChangedDialog dialog;
  // set time to guide the user
  // - original time
  {
    // -- allocate string stream
    std::ostringstream oss;
    // -- convert time
    oss << date::format("%a %Y:%m:%d %H:%M:%S", m_data[m_idx].t);
    // -- convert to string
    auto str = oss.str();
    // -- apply to widget
    dialog.setTimeOld(QString::fromStdString( str ));
  }
  // - new time
  {
    // -- allocate string stream
    std::ostringstream oss;
    // -- convert time
    oss << date::format("%a %Y:%m:%d %H:%M:%S", m_data[m_idx].t0);
    // -- convert to string
    auto str = oss.str();
    // -- apply to widget
    dialog.setTimeNew(QString::fromStdString( str ));
  }

  // launch dialog
  dialog.exec();

  // cancel -> do nothing
  if ( dialog.getResponse() == CalResponse::Cancel ) return;

  // apply time shift to all (relevant) photos
  if ( dialog.getResponse() == CalResponse::Camera )
  {
    for ( size_t i = 0 ; i < m_data.size() ; ++i )
      if ( m_data[i].camera == m_data[m_idx].camera )
        m_data[i].t += dt;
  }
  else if ( dialog.getResponse() == CalResponse::Folder )
  {
    for ( size_t i = 0 ; i < m_data.size() ; ++i )
      if ( m_data[i].folder == m_data[m_idx].folder )
        m_data[i].t += dt;
  }
  else if ( dialog.getResponse() == CalResponse::All )
  {
    for ( size_t i = 0 ; i < m_data.size() ; ++i )
      m_data[i].t += dt;
  }

  // refresh view with new time
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_prev_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx == 0 ) return;

  m_idx -= 1;

  // emit signal to process the change
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_next_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx >= m_data.size()-1 ) return;

  m_idx += 1;

  // emit signal to process the change
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_first_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx == 0 ) return;

  m_idx = 0;

  // emit signal to process the change
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_last_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx >= m_data.size()-1 ) return;

  m_idx = m_data.size()-1;

  // emit signal to process the change
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_excl_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx >= m_data.size() ) return;

  m_data.erase(m_data.begin()+m_idx);

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_del_clicked()
{
  // only act on correct tab, check index to continue
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx >= m_data.size() ) return;

  m_dataDel.push_back(m_data[m_idx]);
  m_data.erase(m_data.begin()+m_idx);

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_undoDel_clicked()
{
  // only act on correct tab, check there is something to insert
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_dataDel.size() == 0 ) return;

  // insert last item from "m_dataDel" in "m_data"; and remove from "m_dataDel"
  m_data   .push_back(m_dataDel[m_dataDel.size()-1]       );
  m_dataDel.erase    (m_dataDel.begin()+m_dataDel.size()-1);

  // switch to inserted image
  m_idx = m_data.size()-1;

  // create new m_thumnails, as it was removed
  auto file = &m_data[m_idx];
  file->ithumb = m_thumnails->push_back(file->path,file->rotation);

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_rotL_clicked()
{
  // only act on correct tab, and for correct parameters
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx >= m_data.size() ) return;
  if ( m_data.size() == 0 ) return;

  // rotate, and signal manual rotation
  m_data[m_idx].rotation -= 90;
  m_data[m_idx].rot_mod   = true;

  // signal to redraw image
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tV_pushButton_rotR_clicked()
{
  // only act on correct tab, and for correct parameters
  if ( ui->tabWidget->currentIndex() != Tab::View ) return;
  if ( m_idx >= m_data.size() ) return;
  if ( m_data.size() == 0 ) return;

  // rotate, and signal manual rotation
  m_data[m_idx].rotation += 90;
  m_data[m_idx].rot_mod   = true;

  // signal to redraw image
  emit indexChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_navTop_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  ui->tS_listWidget->scrollToTop();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_navBottom_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  ui->tS_listWidget->scrollToBottom();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_navPgUp_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

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
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

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
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  m_selLast = -1;

  // current selection
  std::vector<size_t> sel = selectedItems(ui->tS_listWidget);

  // catch all the obvious cases in which there is not one last selected item
  if ( sel.size()==0 || m_selPrev.size()==0 || sel.size()!=m_selPrev.size()+1 ) {
    m_selPrev = sel;
    return;
  }

  std::vector<size_t> diff(sel.size()+m_selPrev.size());
  std::vector<size_t>::iterator it;

  it = std::set_difference(sel.begin(),sel.end(),m_selPrev.begin(),m_selPrev.end(),diff.begin());

  diff.resize(it-diff.begin());

  m_selPrev = sel;

  if ( diff.size()==1 ) m_selLast = diff[0];
}

// =================================================================================================

void MainWindow::on_tS_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
  int row = ui->tS_listWidget->row(item);

  if ( row < 0 ) row = 0;

  m_idx = static_cast<size_t>(row);

  if ( m_idx >= m_data.size() ) m_idx = m_data.size()-1;

  ui->tabWidget->setCurrentIndex( Tab::View );
}

// =================================================================================================

void MainWindow::on_tS_pushButton_split_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // change camera index
  for ( auto &i : rows ) m_data[i].camera = m_ncam;

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Iexcl_clicked()
{
  // only act on correct tab, and non-empty m_data
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;
  if ( m_data.size() == 0 ) return;

  // get a list with selected items
  std::vector<size_t> index = selectedItems(ui->tS_listWidget,false);

  // proceed only for non-empty lists
  if  ( index.size() <= 0 ) return;

  // exclude images: remove from m_data
  for ( auto &i : index ) m_data.erase(m_data.begin()+i);

  // empty selection
  selectionClear(ui->tS_listWidget);

  // set number current item to close to the selection
  if ( index[index.size()-1] > 0 ) m_idx = index[index.size()-1]-1;
  else                             m_idx = 0;

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Idel_clicked()
{
  // only act on correct tab, and non-empty m_data
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;
  if ( m_data.size() == 0 ) return;

  // get a list with selected items
  std::vector<size_t> index = selectedItems(ui->tS_listWidget,false);

  // proceed only for non-empty lists
  if  ( index.size() <= 0 ) return;

  // delete images: add to "m_dataDel" (removal when 'clean up' is pressed)
  for ( auto &i : index ) m_dataDel.push_back(m_data[i]);
  // delete images: remove from "m_data"
  for ( auto &i : index ) m_data.erase(m_data.begin()+i);

  // empty selection
  selectionClear(ui->tS_listWidget);

  // set number current item to close to the selection
  if ( index[index.size()-1] > 0 ) m_idx = index[index.size()-1]-1;
  else                             m_idx = 0;

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Iup_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // top of the list: don't know what to do
  if ( rows[0] == 0 ) return promptWarning("Selection includes first photo, cannot proceed");

  // move up (earlier)
  for ( auto &i: rows )
    m_data[i].t -= m_data[i].t-m_data[i-1].t+std::chrono::duration<int>(1);

  // set index
  m_idx = rows[rows.size()-1];

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Idown_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget,false);

  // check to continue
  if ( rows.size()==0 ) return;

  // top of the list: don't know what to do
  if ( rows[0] == m_data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");

  // move up (earlier)
  for ( auto &i: rows )
    m_data[i].t += m_data[i+1].t-m_data[i].t+std::chrono::duration<int>(1);

  // set index
  m_idx = rows[0];

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Isync_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // check if there is a destination: the last selected image
  if ( m_selLast==-1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // move up (earlier)
  for ( auto &i: rows )
    m_data[i].t = m_data[m_selLast].t;

  // set index
  m_idx = rows[rows.size()-1];

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Cup_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

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
  if ( m_data[row-1].camera==m_data[row].camera )
    return promptWarning("Previous photo is not of another camera, cannot proceed");

  // apply time difference to all
  for ( auto &i : m_data )
    if ( i.camera == m_data[row].camera )
      i.t -= m_data[row].t-m_data[row-1].t+std::chrono::duration<int>(1);

  // set index
  m_idx = rows[rows.size()-1];

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Cdown_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget,false);

  // check to continue
  if ( rows.size() == 0 ) return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0] >= m_data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");
  if ( rows.size() > 1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( m_data[row+1].camera == m_data[row].camera )
    return promptWarning("Next photo is not of another camera, cannot proceed");

  // apply time difference to all
  for ( auto &i : m_data )
    if ( i.camera == m_data[row].camera )
      i.t += m_data[row+1].t-m_data[row].t+std::chrono::duration<int>(1);

  // set index
  m_idx = rows[0];

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Csync_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // check if there is a destination: the last selected image
  if ( m_selLast==-1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // read number of cameras
  size_t NCAM = 0;
  for ( auto &i : m_data )
    NCAM = std::max(NCAM,i.camera);

  // list cameras
  std::vector<int> check(NCAM+1,0);
  // check if camera occurs
  for ( auto &row : rows ) {
    if ( check[m_data[row].camera] )
      return promptWarning("Selection includes several photos from the same camera, cannot proceed");
    check[m_data[row].camera] = 1;
  }

  // sync
  size_t ref = m_selLast;
  for ( auto &row : rows ) {
    if ( row != ref ) {
      for ( auto &i : m_data )
        if ( i.camera == m_data[row].camera )
          i.t -= m_data[row].t-m_data[ref].t;
    }
  }

  // set index
  m_idx = rows[rows.size()-1];

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Fup_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

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
  if ( m_data[row-1].folder == m_data[row].folder )
    return promptWarning("Previous photo is not of another folder, cannot proceed");

  // apply time difference to all
  for ( auto &i : m_data )
    if ( i.folder == m_data[row].folder )
      i.t -= m_data[row].t-m_data[row-1].t+std::chrono::duration<int>(1);

  // set index
  m_idx = rows[rows.size()-1];

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tS_pushButton_Fdown_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget,false);

  // check to continue
  if ( rows.size() == 0 ) return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0] >= m_data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");
  if ( rows.size() > 1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( m_data[row+1].folder == m_data[row].folder )
    return promptWarning("Next photo is not of another folder, cannot proceed");

  // apply time difference to all
  for ( auto &i : m_data )
    if ( i.folder == m_data[row].folder )
      i.t += m_data[row+1].t-m_data[row].t+std::chrono::duration<int>(1);

  // set index
  m_idx = rows[0];

  // emit signal to process the change
  emit dataChanged();
}


// =================================================================================================

void MainWindow::on_tS_pushButton_Fsync_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Sort ) return;

  // check if there is a destination: the last selected image
  if ( m_selLast == -1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->tS_listWidget);

  // check to continue
  if ( rows.size() == 0 ) return;

  // read number of folders
  size_t NFOL = 0;
  for ( auto &i : m_data )
    NFOL = std::max(NFOL,i.folder);

  // list folders
  std::vector<int> check(NFOL+1,0);
  // check if folder occurs
  for ( auto &row : rows ) {
    if ( check[m_data[row].folder] )
      return promptWarning("Selection includes several photos from the same folder, cannot proceed");
    check[m_data[row].folder] = 1;
  }

  // sync
  size_t ref = m_selLast;
  for ( auto &row : rows ) {
    if ( row != ref ) {
      for ( auto &i : m_data )
        if ( i.folder == m_data[row].folder )
          i.t -= m_data[row].t-m_data[ref].t;
    }
  }

  // set index
  m_idx = rows[rows.size()-1];

  // emit signal to process the change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_tW_pushButton_path_clicked()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Write ) return;

  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(m_workDir);
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if ( dialog.exec() ) dir = dialog.directory();
  else return;

  ui->tW_lineEdit_path->setText(dir.absolutePath());

  // store new suggested directory
  m_workDir = dir.absolutePath();
}

// =================================================================================================

void MainWindow::on_tW_lineEdit_path_editingFinished()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex() != Tab::Write ) return;

  if ( ui->tW_lineEdit_path->text().length()>0 )
    ui->tW_pushButton_write->setEnabled(true);
  else
    ui->tW_pushButton_write->setEnabled(false);
}

// =================================================================================================

void MainWindow::on_tW_pushButton_write_clicked()
{
  // only act on correct tab, non-empty m_data
  if ( ui->tabWidget->currentIndex() != Tab::Write ) return;
  if ( m_data.size() == 0 ) return;

  // number of characters needed the fit the photos
  // (ignore that the removed image might reduce N)
  int N = QString::number(m_data.size()).length();

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
  for ( size_t i = 0 ; i < m_data.size() ; ++i )
  {
    QString fname = ui->tW_lineEdit_name->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
    if ( QFileInfo(outdir.filePath(fname)).isFile() ) {
      this->promptWarning(\
        tr("The file '%1' already exists, please select an empty directory").arg(fname));
      return;
    }
  }

  // update list with paths
  for ( auto &file : m_data )
    m_cleanPaths.push_back(file.dir);
  // convert to unique list
  m_cleanPaths.unique();

  // write output
  // - allocate JSON-struct
  json j;
  // - logical to write EXIF-data directly to JPEG
  bool writeExif = ( ui->tW_checkBox_exif->isVisible() and ui->tW_checkBox_exif->isChecked() );
  bool written;
  // - loop over photos
  for ( size_t i = 0 ; i < m_data.size() ; ++i )
  {
    // - format -> filename
    QString fname = ui->tW_lineEdit_name->text()+QString("-")+QString("%1.jpg").arg(i+1,N,10,QChar('0'));
    QString fpath = outdir.filePath(fname);
    // - write EXIF-data directly to JPEG
    if ( writeExif )
    {
      // -- write
      m_data[i].writeinfo();
      // -- temporary copy of file definition
      File f = m_data[i];
      // -- re-read EXIF-data
      written = f.readinfo();
      // -- check if the storage of the time was successful
      if ( written ) written = f.t == m_data[i].t;
    }
    else
    {
      // -- EXIF-data not written -> do not check, always write to JSON
      written = false;
    }
    // - store information to JSON-struct
    // -- the camera index, if more than one camera
    if ( m_ncam > 1 )
      j[fname.toStdString()]["camera"] = m_data[i].camera;
    // -- the new time, if time is unequal to the original time
    if ( m_data[i].t != m_data[i].t0 and !written )
    {
      // - allocate string stream
      std::ostringstream oss;
      // - convert time
      oss << date::format("%Y:%m:%d %H:%M:%S", m_data[i].t);
      // - convert to string
      auto str = oss.str();
      // - add to JSON file
      j[fname.toStdString()]["time"] = str;
    }
    // -- the rotation, if manually modified
    if ( m_data[i].rot_mod and !written )
      j[fname.toStdString()]["rotation"] = m_data[i].rotation;
    // - copy or move
    if ( ui->tW_checkBox_keepOrig->isChecked() ) { QFile::copy(  m_data[i].path,fpath); }
    else                                         { QFile::rename(m_data[i].path,fpath); }
  }

  // store "PATH/chroto.json"
  QString fpath = outdir.filePath("chroto.json");
  std::ofstream o(fpath.toStdString());
  o << std::setw(4) << j << std::endl;

  // clear m_data-structure
  while ( m_data.size()>0 )
    m_data.erase(m_data.begin());

  // clear thumbnails
  m_thumnails->empty();

  // disable button (enabled when m_data is added)
  ui->tW_pushButton_write->setEnabled(false);
}

// =================================================================================================

void MainWindow::on_tW_pushButton_clean_clicked()
{
  // only act on correct tab, non-empty m_data
  if ( ui->tabWidget->currentIndex() != Tab::Write ) return;
  if ( m_dataDel.size() == 0 ) return;

  // update list with paths
  for ( auto &file : m_dataDel )
    m_cleanPaths.push_back(file.dir);
  // convert to unique list
  m_cleanPaths.unique();

  // remove deleted photos
  for ( auto &file : m_dataDel )
    QFile::remove(file.path);

  // list with useless system files
  std::vector<QString> sfiles;
  sfiles.push_back(".DS_Store");
  sfiles.push_back("Thumbs.db");

  // visit all folders that had been selected
  for ( auto dir : m_cleanPaths ) {
    // - remove useless system files
    for ( auto file : sfiles )
      QFile::remove(QDir(dir).filePath(file));
    // - remove empty folders
    if ( QDir(dir).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).isEmpty() )
      QDir(dir).removeRecursively();
  }

  // clear lists
  while ( m_cleanPaths.size()>0 ) m_cleanPaths.erase(m_cleanPaths.begin());
  while ( m_dataDel   .size()>0 ) m_dataDel   .erase(m_dataDel   .begin());

  // disable button (enabled when m_data is added)
  ui->tW_pushButton_clean->setEnabled(false);
}

// =================================================================================================
// Thumbnails class - functions
// =================================================================================================

QIcon& Thumbnails::at ( size_t i )
{
  return m_data[i];
}

// =================================================================================================

size_t Thumbnails::size ()
{
  return m_data.size();
}

// =================================================================================================

void Thumbnails::requestStop ()
{
  m_stop = true;
}

// =================================================================================================

bool Thumbnails::isBusy ()
{
  return m_busy;
}

// =================================================================================================

size_t Thumbnails::unread()
{
  size_t n = 0;

  for ( auto &i : m_isread )
    if ( !i )
      ++n;

  return n;
}

// =================================================================================================

size_t Thumbnails::push_back(QString name, int rot)
{
  QPixmap pix(m_npix,m_npix);
  pix.fill(QColor("white"));

  m_path    .push_back(name);
  m_data    .push_back(QIcon(pix));
  m_isread  .push_back(0);
  m_rotation.push_back(rot);

  return m_data.size()-1;
}

// =================================================================================================

void Thumbnails::erase(std::vector<size_t> index)
{
  m_stop = true;

  std::sort(index.begin(),index.end(),[](size_t i,size_t j){return i>j;});

  for ( auto &i: index ) {
    m_data    .erase(m_data    .begin()+i);
    m_path    .erase(m_path    .begin()+i);
    m_isread  .erase(m_isread  .begin()+i);
    m_rotation.erase(m_rotation.begin()+i);
  }
}

// =================================================================================================

void Thumbnails::empty()
{
  m_stop = true;

  while ( m_data    .size()>0 ) m_data    .erase(m_data    .begin());
  while ( m_path    .size()>0 ) m_path    .erase(m_path    .begin());
  while ( m_isread  .size()>0 ) m_isread  .erase(m_isread  .begin());
  while ( m_rotation.size()>0 ) m_rotation.erase(m_rotation.begin());

  m_busy = false;
  m_npix = 32;
}

// =================================================================================================

void Thumbnails::setResolution( size_t N )
{
  // - check if the resolution is different
  if ( N == m_npix ) return;
  // - m_stop the current read
  m_stop = true;
  // - overwrite number of pixels in both directions
  m_npix = N;
  // - mark all for reading
  for ( auto &i : m_isread ) i = 0;
}

// =================================================================================================

void Thumbnails::read()
{
  m_busy = true ;
  m_stop = false;

  // - loop over all photos
  for ( size_t i=0; i<m_data.size(); ++i )
  {
    // -- break the loop if requested externally
    if ( m_stop || QThread::currentThread()->isInterruptionRequested() ) {
      m_busy = false;
      m_stop = false;
      return;
    }

    // -- read if not read before: account for pre-specified m_rotation
    //    (obtained from the EXIF-data in "MainWindow" below)
    if ( !m_isread[i] )
    {
      QMatrix rot;
      rot.rotate(m_rotation[i]);

      QPixmap pix(m_path[i]);
      pix.scaled(m_npix,m_npix,Qt::KeepAspectRatio, Qt::FastTransformation);

      if ( m_stop ) {
        m_busy = false;
        m_stop = false;
        return;
      }

      m_data  [i] = QIcon(QPixmap(pix.transformed(rot)));
      m_isread[i] = 1;
    }
  }

  // - transmit that all images have been read
  m_busy = false;
  m_stop = false;
  emit completed();
}

// =================================================================================================

bool File::readinfo()
{
  // read data/time and rotation:
  // - try to read using "exiv2", fall back on "easyexif"
  // - if "exiv2" is not present at compile time the first step is completely ignored
  #ifdef WITHEXIV2
    try
    {
      // read using "exiv2"
      // ------------------

      Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path.toStdString().c_str());

      if ( image.get() == 0 ) return false;

      image->readMetadata();

      Exiv2::ExifData &exifData = image->exifData();

      if ( exifData.empty() ) return false;

      // read the data/time
      try
      {
        Exiv2::Exifdatum& tag = exifData["Exif.Photo.DateTimeOriginal"];
        std::string str = tag.toString();

        std::istringstream iss{str};
        iss >> date::parse("%Y:%m:%d %H:%M:%S", t);

        t0 = t;
      }
      catch ( ... )
      {
        return false;
      }

      // read the rotation
      try
      {
        Exiv2::Exifdatum &tag = exifData["Exif.Image.Orientation"];
        long rot = tag.toLong();

        rotation = 0;
        if      ( rot==8 ) rotation = -90;
        else if ( rot==6 ) rotation =  90;
        else if ( rot==3 ) rotation = 180;
      }
      catch ( ... )
      {
        return false;
      }

      return true;
    }
    catch ( ... )
    {
  #endif

      // read using "easyexif"
      // ---------------------

      // read JPEG-file into a buffer
      // - open file
      FILE *fid = std::fopen(path.toStdString().c_str(),"rb");
      // - check success
      if (!fid) return false;
      // - read size
      fseek(fid, 0, SEEK_END);
      unsigned long fsize = ftell(fid);
      rewind(fid);
      // - read to buffer
      unsigned char *buf = new unsigned char[fsize];
      if (fread(buf, 1, fsize, fid) != fsize) {
        delete[] buf;
        return false;
      }
      // - close file
      fclose(fid);

      // parse EXIF
      // - allocate
      easyexif::EXIFInfo result;
      // - parse
      int code = result.parseFrom(buf, fsize);
      // - release buffer
      delete[] buf;
      // - check success
      if ( code ) return false;

      // convert orientation to rotation
      rotation = 0;
      if      ( result.Orientation==8 ) rotation = -90;
      else if ( result.Orientation==6 ) rotation =  90;
      else if ( result.Orientation==3 ) rotation = 180;

      // get time string
      std::istringstream iss;
      if      ( result.DateTimeOriginal .size() > 0 ) iss.str(result.DateTimeOriginal );
      else if ( result.DateTime         .size() > 0 ) iss.str(result.DateTime         );
      else if ( result.DateTimeDigitized.size() > 0 ) iss.str(result.DateTimeDigitized);
      else    { return false; }

      // interpret time string
      try { iss >> date::parse("%Y:%m:%d %H:%M:%S", t); } catch ( ... ) { return false; }

      // copy time
      t0 = t;

      return true;

  #ifdef WITHEXIV2
    }
  #endif
}

// =================================================================================================

bool File::writeinfo()
{
  #ifdef WITHEXIV2

    // do nothing if nothing was changed
    if ( t == t0 and !rot_mod ) return true;

    try
    {
      // read EXIF-data
      // - open image
      Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path.toStdString());
      // - check read
      if ( image.get() == 0 ) return false;
      // - read EXIF-data
      image->readMetadata();
      // - create pointer
      Exiv2::ExifData &exifData = image->exifData();
      // - check
      if ( exifData.empty() ) return false;

      // change time (if needed)
      if ( t != t0 )
      {
        // - allocate string stream
        std::ostringstream oss;
        // - convert time
        oss << date::format("%Y:%m:%d %H:%M:%S", t);
        // - convert to string
        auto str = oss.str();
        // - write to EXIF-data
        exifData["Exif.Photo.DateTimeOriginal"] = str;
      }

      // change rotation (if needed)
      if ( rot_mod )
      {
        // - allocate variable
        int r;
        // - convert rotation of EXIF-value
        if      ( rotation == -90 ) r = 8;
        else if ( rotation ==  90 ) r = 6;
        else if ( rotation == 180 ) r = 3;
        else                        r = 1;
        // - write to EXIF-data
        exifData["Exif.Image.Orientation"] = uint16_t(r);
      }

      // write EXIF-data
      image->setExifData(exifData);
      image->writeMetadata();

      return true;
    }
    catch ( ... )
    {
      return false;
    }

  #endif
}

// =================================================================================================

std::vector<size_t> selectedItems(QListWidget* list, bool ascending)
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

std::string commonPath (const std::string &path, const std::string &name)
{
  size_t i,N;

  // copy input to output (which will be modified)
  std::string out_path = path;

  // find index for which there is still overlap: 'path[:i] == name[:i]'
  // - zero-initialize
  i = 0;
  // - loop until there is no more overlap
  while ( true )
  {
    if ( i >= path.size() or i >= name.size() ) break;
    if ( path[i] != name[i] ) break;
    ++i;
  }

  // remove from the end of the path
  // - number of items to remove
  N = out_path.size() - i;
  // - remove items
  for ( size_t j = 0 ; j < N ; ++j )
    out_path.erase(out_path.begin()+out_path.size()-1);

  // returned reduced path
  return out_path;
}

// =================================================================================================

std::string commonPath (const std::string &path, const std::string &name, const std::string &delim)
{
  assert( delim.size() == 1 );

  size_t i,N;

  // copy input to output (which will be modified)
  std::string out_path = path;

  // find index for which there is still overlap: 'path[:i] == name[:i]'
  // - zero-initialize
  i = 0;
  // - loop until there is no more overlap
  while ( true )
  {
    if ( i >= path.size() or i >= name.size() ) break;
    if ( path[i] != name[i] ) break;
    ++i;
  }

  // rewind until deliminator
  for (  ; i > 0 ; --i )
    if ( path[i] == delim[0] )
      break;

  // remove from the end of the path
  // - number of items to remove
  N = out_path.size() - i - 1;
  // - remove items
  for ( size_t j = 0 ; j < N ; ++j )
    out_path.erase(out_path.begin()+out_path.size()-1);

  // returned reduced path
  return out_path;
}

// =================================================================================================

std::string removePath (const std::string &path, const std::string &name)
{
  // copy input to output (which will be modified)
  std::string out = name;

  // check
  if ( path.size() > name.size() ) return out;

  // remove path from name
  for ( size_t i = 0 ; i < path.size() ; ++i )
    out.erase(out.begin());

  // return
  return out;
}

