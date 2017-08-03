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

  // T1: fill arrays collecting the file-lists and related buttons
  fileList.push_back(ui->listWidgetT1_0          ); pathView.push_back(ui->lineEditT1_0        );
  fileList.push_back(ui->listWidgetT1_1          ); pathView.push_back(ui->lineEditT1_1        );
  fileList.push_back(ui->listWidgetT1_2          ); pathView.push_back(ui->lineEditT1_2        );
  fileList.push_back(ui->listWidgetT1_3          ); pathView.push_back(ui->lineEditT1_3        );
  fileList.push_back(ui->listWidgetT1_4          ); pathView.push_back(ui->lineEditT1_4        );
  fileList.push_back(ui->listWidgetT1_5          ); pathView.push_back(ui->lineEditT1_5        );
  fileList.push_back(ui->listWidgetT1_6          ); pathView.push_back(ui->lineEditT1_6        );
  fileList.push_back(ui->listWidgetT1_7          ); pathView.push_back(ui->lineEditT1_7        );
  fileList.push_back(ui->listWidgetT1_8          ); pathView.push_back(ui->lineEditT1_8        );
  fileList.push_back(ui->listWidgetT1_9          ); pathView.push_back(ui->lineEditT1_9        );
  fileList.push_back(ui->listWidgetT1_10         ); pathView.push_back(ui->lineEditT1_10       );
  fileList.push_back(ui->listWidgetT1_11         ); pathView.push_back(ui->lineEditT1_11       );
  // -
  dirSelec.push_back(ui->pushButtonT1_select_0   ); delSelec.push_back(ui->pushButtonT1_excl_0 );
  dirSelec.push_back(ui->pushButtonT1_select_1   ); delSelec.push_back(ui->pushButtonT1_excl_1 );
  dirSelec.push_back(ui->pushButtonT1_select_2   ); delSelec.push_back(ui->pushButtonT1_excl_2 );
  dirSelec.push_back(ui->pushButtonT1_select_3   ); delSelec.push_back(ui->pushButtonT1_excl_3 );
  dirSelec.push_back(ui->pushButtonT1_select_4   ); delSelec.push_back(ui->pushButtonT1_excl_4 );
  dirSelec.push_back(ui->pushButtonT1_select_5   ); delSelec.push_back(ui->pushButtonT1_excl_5 );
  dirSelec.push_back(ui->pushButtonT1_select_6   ); delSelec.push_back(ui->pushButtonT1_excl_6 );
  dirSelec.push_back(ui->pushButtonT1_select_7   ); delSelec.push_back(ui->pushButtonT1_excl_7 );
  dirSelec.push_back(ui->pushButtonT1_select_8   ); delSelec.push_back(ui->pushButtonT1_excl_8 );
  dirSelec.push_back(ui->pushButtonT1_select_9   ); delSelec.push_back(ui->pushButtonT1_excl_9 );
  dirSelec.push_back(ui->pushButtonT1_select_10  ); delSelec.push_back(ui->pushButtonT1_excl_10);
  dirSelec.push_back(ui->pushButtonT1_select_11  ); delSelec.push_back(ui->pushButtonT1_excl_11);
  // -
  nameSort.push_back(ui->pushButtonT1_nameSort_0 );
  nameSort.push_back(ui->pushButtonT1_nameSort_1 );
  nameSort.push_back(ui->pushButtonT1_nameSort_2 );
  nameSort.push_back(ui->pushButtonT1_nameSort_3 );
  nameSort.push_back(ui->pushButtonT1_nameSort_4 );
  nameSort.push_back(ui->pushButtonT1_nameSort_5 );
  nameSort.push_back(ui->pushButtonT1_nameSort_6 );
  nameSort.push_back(ui->pushButtonT1_nameSort_7 );
  nameSort.push_back(ui->pushButtonT1_nameSort_8 );
  nameSort.push_back(ui->pushButtonT1_nameSort_9 );
  nameSort.push_back(ui->pushButtonT1_nameSort_10);
  nameSort.push_back(ui->pushButtonT1_nameSort_11);

  // T1: link scroll position listWidgets
  for ( auto &i: fileList )
    for ( auto &j: fileList )
      if ( i!=j )
        connect(i->verticalScrollBar(),SIGNAL(valueChanged(int)),
                j->verticalScrollBar(),SLOT  (setValue    (int)));
  // T3: center image
  ui->view_idx_label->setAlignment(Qt::AlignCenter);

  // T1: select folder / remove selected files / sort by name -> apply to specific "QListWidget"
  for ( size_t i=0; i<dirSelec.size(); ++i ) {
    connect(dirSelec[i],&QPushButton::clicked,[=](){addFiles    (     i );});
    connect(nameSort[i],&QPushButton::clicked,[=](){dataNameSort(     i );});
    connect(delSelec[i],&QPushButton::clicked,[=](){listExcl(fileList[i]);});
  }
  // T2: exclude/delete photos
  connect(ui->pushButtonT2i_excl,&QPushButton::clicked,[=](){listExcl(ui->listWidgetT2);});
  connect(ui->pushButtonT2i_del ,&QPushButton::clicked,[=](){listDel (ui->listWidgetT2);});

  // Tx: "data" changed -> sort by time and refresh views
  connect(this,SIGNAL(dataChanged()),this,SLOT(dataUpdate()));
  // Tx: "read" thumbnails, update "data" / refresh views when reading is completed
  connect(this     ,SIGNAL(thumbnailRead()),thumbnail,SLOT(read())      );
  connect(thumbnail,SIGNAL(completed    ()),this     ,SLOT(dataUpdate()));
  // Tx: tab changed: update "data" and views to latest values
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){clearSelAll();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){dataUpdate ();});
  // T3: "idx" changes -> update view
  connect(this,SIGNAL(indexChanged()),this,SLOT(viewImage()));

  // T1: convert selected item to current index
  for ( auto &i: fileList ) connect(i,&QListWidget::itemSelectionChanged,[=](){list2idx(i);});
  // T2: convert selected item to current index
  connect(ui->listWidgetT2,&QListWidget::itemSelectionChanged,[=](){list2idx(ui->listWidgetT2);});

  // T2: manually refresh view to show the latest thumbnails
  connect(ui->pushButtonT2_refresh,SIGNAL(clicked(bool)),SLOT(viewStream()));

  // T4: couple "outPath" pushButton to "outPath" lineEdit
  connect(ui->pushButtonT4_path,SIGNAL(clicked(bool)),SLOT(on_lineEditT4_path_editingFinished()));

  // start fresh: clear all data and widgets
  connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(clearAll()));

  // enforce opening on the first tab
  ui->tabWidget->setCurrentIndex(0);

  // T1: set number of folders (==2)
  ui->comboBoxT1_nfol->setCurrentIndex(1);

  // start worker-thread (to which "thumbnail" is assigned)
  workerThread.start();
}

// =================================================================================================

MainWindow::~MainWindow()
{
  workerThread.requestInterruption();
  workerThread.quit();
  workerThread.wait();

  delete ui;
}

// =================================================================================================

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   this->viewImage();
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

void MainWindow::dataUpdate()
{
  // request to stop reading thumbnails (to lose fewer time below)
  thumbnail->requestStop();

  // update camera index to smallest possible
  // - initialize
  std::vector<int> cam(data.size(),0);
  // - fill
  for ( auto &i: data ) cam[i.camera] = 1;
  // - convert to new indices, step 1/2: cumulative sum
  for ( size_t i=1; i<cam.size(); ++i ) cam[i] += cam[i-1];
  // - convert to new indices, step 2/2: correct one
  for ( auto &i: cam ) --i;
  // - renumber data
  for ( auto &i: data ) i.camera = cam[i.camera];

  // list with thumbnails to include
  // - initialize
  std::vector<int>    incl(thumbnail->size(),0); // 1 if the thumbnail is still part of "data"
  std::vector<size_t> rm;                        // list with thumbnails to remove
  // - fill
  for ( auto &i: data ) incl[i.ithumb] = 1;
  // - list items to remove
  for ( size_t i=0; i<incl.size(); ++i ) if ( incl[i]==0 ) rm.push_back(i);
  // - convert to new indices, step 1/2: cumulative sum
  for ( size_t i=1; i<incl.size(); ++i ) incl[i] += incl[i-1];
  // - convert to new indices, step 2/2: correct one
  for ( auto &i: incl ) --i;
  // - renumber data
  for ( auto &i: data ) i.ithumb = incl[i.ithumb];

  // correct current index if needed
  if ( idx>=data.size() ) idx = data.size()-1;

  // sort by time
  dataTimeSort();

  // remove thumbnails that are no longer part of "data"
  thumbnail->erase(rm);
  // if needed, launch reading thumbnails (that have not been read)
  if ( thumbnail->unread() ) { emit thumbnailRead(); }

  // update widget
  if      ( ui->tabWidget->currentIndex()==0 ) viewFileList();
  else if ( ui->tabWidget->currentIndex()==1 ) viewStream();
  else if ( ui->tabWidget->currentIndex()==2 ) viewImage();
  else if ( ui->tabWidget->currentIndex()==3 ) showDate();
}

// =================================================================================================

void MainWindow::on_comboBoxT1_nfol_currentIndexChanged(int index)
{
  // set all invisible
  for ( size_t i=0; i<nameSort.size(); ++i ) {
    fileList[i]->setVisible(false);
    pathView[i]->setVisible(false);
    dirSelec[i]->setVisible(false);
    delSelec[i]->setVisible(false);
    nameSort[i]->setVisible(false);
  }

  // find the number of folders containing selected photos
  size_t folder = 0;
  for ( auto &file : data )
    folder = std::max(folder,file.folder);
  // if request number of folders is too low -> correct
  if ( index<static_cast<int>(folder) ) {
    index = static_cast<int>(folder);
    ui->comboBoxT1_nfol->setCurrentIndex(index);
  }

  // enable requested number of columns
  for ( int i=0; i<std::min(index+1,static_cast<int>(nameSort.size())); ++i ) {
    fileList[i]->setVisible(true);
    pathView[i]->setVisible(true);
    dirSelec[i]->setVisible(true);
    delSelec[i]->setVisible(true);
    nameSort[i]->setVisible(true);
  }
}

// =================================================================================================

void MainWindow::addFiles(size_t folder)
{
  // if list is not empty: remove previous selection
  // -----------------------------------------------

  std::vector<int> rm;

  for ( int i=data.size()-1; i>=0; --i )
    if ( data[i].folder==folder )
      rm.push_back(i);

  for ( auto &i : rm )
    data.erase(data.begin()+i);


  // set camera index to the lowest possible
  // ---------------------------------------

  if ( data.size()>0 ) {
    std::vector<int> camera(data.size(),0);
    for ( auto &file : data )
      camera[file.camera] = 1;
    camera[0] -= 1;
    for ( size_t i=1; i<data.size(); ++i )
      camera[i] += camera[i-1];
    for ( auto &file : data )
      file.camera = static_cast<size_t>(camera[file.camera]);
  }

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

  // allocate local variables
  std::time_t t;
  int         rot;
  size_t      camera   = 0;
  int         modified = 0;

  // find which camera index to use (one more than the current maximum)
  if ( data.size()>0 ) {
    for ( auto &file : data ) camera = std::max(camera,file.camera);
    ++camera;
  }

  // read all JPG-files, if "chroto.json" exists: overwrite time from JPEG with stored values,
  // segment in the different cameras that were stored in "chroto.json"
  for ( int i=0; i<lst_jpeg.size(); ++i ) {
    // - extract file from list
    QFileInfo finfo = lst_jpeg.at(i);
    // - try the read the EXIF information. skip image if unsuccessful
    try {
      std::tie(t,rot) = jpg2info(finfo.absoluteFilePath().toStdString());
    }
    catch (...) {
      modified = 1;
      rot      = 0;
      t        = finfo.created().toTime_t();
    }

    // - compute information to file
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
    if ( lst_json.size()==1 ) {
      file.time     = static_cast<std::time_t>(jdata[finfo.fileName().toStdString()]["time"    ]);
      file.camera  += static_cast<size_t>     (jdata[finfo.fileName().toStdString()]["camera"  ]);
      file.modified = static_cast<int>        (jdata[finfo.fileName().toStdString()]["modified"]);
    }
    // - store in list
    data.push_back(file);
  }

  // set the thumbail size
  if ( data.size()<200 ) {
    npix = 200;
    thumbnail->setSize(50);
  }
  else if ( data.size()<1000 ) {
    npix = 100;
    thumbnail->setSize(25);
  }
  else if ( data.size()<10000 ) {
    npix = 50;
    thumbnail->setSize(25);
  }
  else {
    npix = 15;
    thumbnail->setSize(15);
  }

  emit dataChanged();
}

// =================================================================================================

void MainWindow::listExcl(QListWidget *list)
{
  std::vector<size_t> index = selectedItems(list,false);

  for ( size_t &i: index )
    data.erase(data.begin()+i);

  clearSel(list);

  emit dataChanged();
}

// =================================================================================================

void MainWindow::listDel(QListWidget *list)
{
  std::vector<size_t> index = selectedItems(list,false);

  for ( size_t &i: index )
    delData.push_back(data[i]);

  for ( size_t &i: index )
    data.erase(data.begin()+i);

  clearSel(list);

  emit dataChanged();
}

// =================================================================================================

void MainWindow::list2idx(QListWidget *list)
{
  std::vector<size_t> sel = selectedItems(list);
  size_t jdx = idx;

  if ( sel.size()>0 )
    jdx = sel[0];

  if ( jdx>=data.size() )
    jdx = data.size()-1;

  if ( jdx!=idx ) {
    idx = jdx;
    emit indexChanged();
  }
}

// =================================================================================================

void MainWindow::clearSel(QListWidget *list)
{
  for ( int j=0; j<list->count(); ++j )
    list->item(j)->setSelected(false);
}

// =================================================================================================

void MainWindow::clearSelAll()
{
  for ( auto &list: fileList )
    clearSel(list);

  clearSel(ui->listWidgetT2);
}

// =================================================================================================

void MainWindow::viewFileList()
{
  // empty storage paths
  for ( auto line : pathView )
    line->clear();
  // empty listWidgets
  for ( auto list : fileList )
    while ( list->count()>0 )
      list->takeItem(0);

  // no photos selected -> quit this function
  if ( data.size()==0 )
    return;

  // fill listWidgets
  for ( auto &file : data ) {
    for ( size_t l=0; l<fileList.size(); ++l ) {
      if ( file.folder==l ) {
        fileList[l]->addItem(file.disp);
        pathView[l]->setText(file.dir );
      }
      else {
        fileList[l]->addItem("");
      }
    }
  }

  // update selected row to current index
  for ( size_t l=0; l<fileList.size(); ++l )
    fileList[l]->setCurrentRow(idx);

  // update the folders on T4
  // - store current index
  int cur = ui->comboBoxt4_ref->currentIndex();
  // - clear the widget
  ui->comboBoxt4_ref->clear();
  // - add folders
  for ( auto &i : pathView )
    if ( i->isVisible() )
      ui->comboBoxt4_ref->addItem(i->text() );
  // - set index to previous value
  if ( cur < ui->comboBoxt4_ref->count() )
    ui->comboBoxt4_ref->setCurrentIndex(cur);
}

// =================================================================================================

void MainWindow::viewStream(void)
{
  // list with selected rows
  std::vector<size_t> old = selectedItems(ui->listWidgetT2);
  // convert rows to new index
  // - allocate
  std::vector<size_t> renum(data.size());
  // - fill
  for ( size_t i=0; i<data.size(); ++i )
    renum[data[i].index] = i;
  // - new rows
  std::vector<size_t> rows;
  // - renumber
  for ( auto &i: old )
    rows.push_back(renum[i]);

  // empty the list
  while ( ui->listWidgetT2->count()>0 )
    ui->listWidgetT2->takeItem(0);

  // no data -> quit
  if ( data.size()<=0 )
    return;

  // read number of cameras
  size_t n = 0;
  for ( auto &i : data )
    n = std::max(n,i.camera);
  ++n;

  // create a colormap
  std::vector<int> cmap = cppcolormap::Set1(n);
  std::vector<QColor> col;
  for ( size_t i=0; i<n; ++i )
    col.push_back(QColor(cmap[i*3+0],cmap[i*3+1],cmap[i*3+2]));

  // define style of the widget
  ui->listWidgetT2->setIconSize(QSize(npix,npix));

  // fill the lists with names / icons
  for ( auto &i : data )
    ui->listWidgetT2->addItem(new QListWidgetItem(thumbnail->at(i.ithumb),i.disp));

  // set background color, corresponding to the camera index
  for ( size_t i=0; i<data.size(); ++i )
    ui->listWidgetT2->item(i)->setBackground(QBrush(col[data[i].camera]));

  // change focus
  QListWidgetItem *item = ui->listWidgetT2->item(idx);
  ui->listWidgetT2->scrollToItem(item, QAbstractItemView::EnsureVisible);

  // restore selection
  // - clear entire selection
  for ( int j=0; j<ui->listWidgetT2->count(); ++j )
    ui->listWidgetT2->item(j)->setSelected(false);
  // - no rows selected -> set "idx"
  if ( rows.size()==0 && idx<data.size()-1 )
    rows.push_back(idx);
  // - apply previous selection, moved one up
  for ( auto &row : rows )
    ui->listWidgetT2->item(row)->setSelected(true);
}

// =================================================================================================

void MainWindow::on_pushButtonT2_navTop_clicked()
{
  ui->listWidgetT2->scrollToTop();
}

// =================================================================================================

void MainWindow::on_pushButtonT2_navBottom_clicked()
{
  ui->listWidgetT2->scrollToBottom();
}

// =================================================================================================

void MainWindow::on_pushButtonT2_navPgUp_clicked()
{
  int i = ui->listWidgetT2->verticalScrollBar()->value();
  int N = ui->listWidgetT2->verticalScrollBar()->maximum();

  i -= N/10;

  if ( i < 0 ) i = 0;

  ui->listWidgetT2->verticalScrollBar()->setValue(i);
}

// =================================================================================================

void MainWindow::on_pushButtonT2_navPgDwn_clicked()
{
  int i = ui->listWidgetT2->verticalScrollBar()->value();
  int N = ui->listWidgetT2->verticalScrollBar()->maximum();

  i += N/10;

  if ( i >= N ) i = N-1;

  ui->listWidgetT2->verticalScrollBar()->setValue(i);
}

// =================================================================================================

void MainWindow::on_listWidgetT2_itemSelectionChanged()
{
  selLast = -1;

  // current selection
  std::vector<size_t> sel = selectedItems(ui->listWidgetT2);

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

  if ( diff.size()==1 )
    selLast = diff[0];
}

// =================================================================================================

void MainWindow::dataNameSort(size_t folder)
{
  // quit function if no files are present
  if ( data.size()<=0 )
    return;

  // clear all selected items
  clearSelAll();

  // store current order, to retrieve the new position of "idx"
  for ( size_t i=0; i<data.size(); ++i )
    data[i].index = i;

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

  idx = 0;
  emit dataChanged();
}

// =================================================================================================

void MainWindow::dataTimeSort()
{
  // quit function if no files are present
  if ( data.size()<=0 )
    return;

  // store current order, to retrieve the new position of "idx"
  for ( size_t i=0; i<data.size(); ++i )
    data[i].index = i;

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

void MainWindow::viewImage()
{
  // only act on correct tab
  if ( ui->tabWidget->currentIndex()!=2 )
    return;

  // selectively disable/enable navigation buttons
  ui->pushButtonT3_prev ->setEnabled(idx>0            );
  ui->pushButtonT3_first->setEnabled(idx>0            );
  ui->pushButtonT3_next ->setEnabled(idx<data.size()-1);
  ui->pushButtonT3_last ->setEnabled(idx<data.size()-1);
  ui->pushButtonT3_del  ->setEnabled(data.size()>0    );
  ui->pushButtonT3_excl ->setEnabled(data.size()>0    );

  // clear currently viewed photos
  ui->view_idx_label->clear();

  // no photos selected -> exit this function
  if ( data.size()==0 ) return;

  // view current photo "idx"
  QPixmap    p(data[idx].path);
  int        w = ui->view_idx_label->width();
  int        h = ui->view_idx_label->height();
  QTransform transform;
  QTransform trans = transform.rotate(data[idx].rotation);
  ui->view_idx_label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
}

// =================================================================================================

void MainWindow::on_pushButtonT2i_up_clicked()
{
  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // top of the list: don't know what to do
  if ( rows[0]==0 )
    return promptWarning("Selection includes first photo, cannot proceed");

  // move up (earlier)
  for ( auto &i: rows ) {
    data[i].time    -= data[i].time-data[i-1].time+1;
    data[i].modified = 1;
  }

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_pushButtonT2i_dwn_clicked()
{
  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2,false);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // top of the list: don't know what to do
  if ( rows[0]==data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");

  // move up (earlier)
  for ( auto &i: rows ) {
    data[i].time    += data[i+1].time-data[i].time+1;
    data[i].modified = 1;
  }

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_pushButtonT2i_sync_clicked()
{
  // check if there is a destination: the last selected image
  if ( selLast==-1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // move up (earlier)
  for ( auto &i: rows ) {
    data[i].time     = data[selLast].time;
    data[i].modified = 1;
  }

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_pushButtonT2c_up_clicked()
{
  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0]==0 )
    return promptWarning("Selection includes first photo, cannot proceed");
  if ( rows.size()>1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( data[row-1].camera==data[row].camera )
    return promptWarning("Previous photo is not of another camera, cannot proceed");

  // get time difference
  std::time_t dt = data[row].time-data[row-1].time+1;

  // apply to all
  for ( auto &i: data )
    if ( i.camera==data[row].camera )
      i.time -= dt;

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_pushButtonT2c_dwn_clicked()
{
  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2,false);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0]>=data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");
  if ( rows.size()>1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( data[row+1].camera==data[row].camera )
    return promptWarning("Next photo is not of another camera, cannot proceed");

  // get time difference
  std::time_t dt = data[row+1].time-data[row].time+1;

  // apply to all
  for ( auto &i: data )
    if ( i.camera==data[row].camera )
      i.time += dt;

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_pushButtonT2c_sync_clicked()
{
  // check if there is a destination: the last selected image
  if ( selLast==-1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // read number of cameras
  size_t ncam = 0;
  for ( auto &i: data )
    ncam = std::max(ncam,i.camera);

  // list cameras
  std::vector<int> check(ncam+1,0);
  // check if camera occurs
  for ( auto &row: rows ) {
    if ( check[data[row].camera] )
      return promptWarning("Selection includes several photos from the same camera, cannot proceed");
    check[data[row].camera] = 1;
  }

  // sync
  size_t ref = selLast;
  for ( auto &row: rows ) {
    if ( row!=ref ) {
      std::time_t dt = data[row].time-data[ref].time;
      for ( auto &i: data )
        if ( i.camera==data[row].camera )
          i.time -= dt;
    }
  }

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_pushButtonT2f_up_clicked()
{
  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0]==0 )
    return promptWarning("Selection includes first photo, cannot proceed");
  if ( rows.size()>1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( data[row-1].folder==data[row].folder )
    return promptWarning("Previous photo is not of another folder, cannot proceed");

  // get time difference
  std::time_t dt = data[row].time-data[row-1].time+1;

  // apply to all
  for ( auto &i: data )
    if ( i.folder==data[row].folder )
      i.time -= dt;

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_pushButtonT2f_dwn_clicked()
{
  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2,false);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // top of the list / more then one item: don't know what to do
  if ( rows[0]>=data.size()-1 )
    return promptWarning("Selection includes last photo, cannot proceed");
  if ( rows.size()>1 )
    return promptWarning("Selection contains more than one photo, cannot proceed");

  // get index
  size_t row = rows[0];

  // not a boundary: don't know what to do
  if ( data[row+1].folder==data[row].folder )
    return promptWarning("Next photo is not of another folder, cannot proceed");

  // get time difference
  std::time_t dt = data[row+1].time-data[row].time+1;

  // apply to all
  for ( auto &i: data )
    if ( i.folder==data[row].folder )
      i.time += dt;

  // signal to process change
  emit dataChanged();
}


// =================================================================================================

void MainWindow::on_pushButtonT2f_sync_clicked()
{
  // check if there is a destination: the last selected image
  if ( selLast==-1 ) {
    return promptWarning(
      "Specify the 'destination' explicitly by selecting it last (using Crtl/Cmd + Click)"
    );
  }

  // get sorted list of selected items
  std::vector<size_t> rows = selectedItems(ui->listWidgetT2);

  // no items selected: quit
  if ( rows.size()==0 )
    return;

  // read number of folders
  size_t nfol = 0;
  for ( auto &i: data )
    nfol = std::max(nfol,i.folder);

  // list folders
  std::vector<int> check(nfol+1,0);
  // check if folder occurs
  for ( auto &row: rows ) {
    if ( check[data[row].folder] )
      return promptWarning("Selection includes several photos from the same folder, cannot proceed");
    check[data[row].folder] = 1;
  }

  // sync
  size_t ref = selLast;
  for ( auto &row: rows ) {
    if ( row!=ref ) {
      std::time_t dt = data[row].time-data[ref].time;
      for ( auto &i: data )
        if ( i.folder==data[row].folder )
          i.time -= dt;
    }
  }

  // signal to process change
  emit dataChanged();
}

// =================================================================================================

void MainWindow::on_pushButtonT3_prev_clicked()
{
  if ( idx!=0 ) {
    idx -= 1;
    emit indexChanged();
  }
}

// =================================================================================================

void MainWindow::on_pushButtonT3_next_clicked()
{
  if ( idx<data.size()-1 ) {
    idx += 1;
    emit indexChanged();
  }
}

// =================================================================================================

void MainWindow::on_pushButtonT3_first_clicked()
{
  if ( idx!=0 ) {
    idx = 0;
    emit indexChanged();
  }
}

// =================================================================================================

void MainWindow::on_pushButtonT3_last_clicked()
{
  if ( idx!=data.size()-1 ) {
    idx = data.size()-1;
    emit indexChanged();
  }
}

// =================================================================================================

void MainWindow::on_pushButtonT3_excl_clicked()
{
  if ( idx<data.size() ) {
    data.erase(data.begin()+idx);
    emit dataChanged();
  }
}

// =================================================================================================

void MainWindow::on_pushButtonT3_del_clicked()
{
  if ( idx<data.size() ) {
    delData.push_back(data[idx]);
    data.erase(data.begin()+idx);
    emit dataChanged();
  }
}

// =================================================================================================

void MainWindow::showDate()
{
  if ( data.size()<=0 )
    return;

  std::string str(std::ctime(&data[0].time));

  ui->lineEditT4_date->setText(QString::fromStdString(str));
}

// =================================================================================================

void MainWindow::on_comboBoxt4_ref_activated(int index)
{
  // no folder selected -> do nothing
  if ( index < 0 )
    return;

  // local variables
  std::time_t dt   = 0; // time difference
  int         sign = 0; // sign of the time difference (std::time_t can only be positve)

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

void MainWindow::on_pushButtonT4_path_clicked()
{
  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(workDir);
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();

  ui->lineEditT4_path->setText(dir.absolutePath());

  // store new suggested directory
  workDir = dir.absolutePath();
}

// =================================================================================================

void MainWindow::on_lineEditT4_path_editingFinished()
{
  if ( ui->lineEditT4_path->text().length()>0 )
    ui->pushButtonT4_write->setEnabled(true);
  else
    ui->pushButtonT4_write->setEnabled(false);
}

// =================================================================================================

void MainWindow::on_pushButtonT4_write_clicked()
{
  // number of characters needed the fit the photos
  // (ignore that the removed image might reduce N)
  int N = QString::number(data.size()).length();

  // create output directory, if it does not exist
  QDir outdir(ui->lineEditT4_path->text());
  if ( !outdir.exists() )
    QDir().mkdir(ui->lineEditT4_path->text());

  // check if output files exist
  // - "PATH/chroto.json"
  if ( QFileInfo(outdir.filePath("chroto.json")).isFile() ) {
    this->promptWarning(\
      tr("The file '%1' already exists, please select an empty directory").arg("chroto.json"));
    return;
  }
  // - "PATH/NAME-%0Xd.jpg"
  for ( int i=0; i<static_cast<int>(data.size()); ++i ) {
    QString fname = ui->lineEditT4_name->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
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
    QString fname = ui->lineEditT4_name->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
    QString fpath = outdir.filePath(fname);
    // - store information to JSON-struct
    j[fname.toStdString()]["modified"] = data[i].modified;
    j[fname.toStdString()]["camera"  ] = data[i].camera;
    j[fname.toStdString()]["time"    ] = static_cast<long>(data[i].time);
    j["camera"][std::to_string(data[i].camera)] = data[i].dir.toStdString();
    // - copy or move
    if ( ui->checkBoxT4_keepOrig->isChecked() ) { QFile::copy(  data[i].path,fpath); }
    else                                        { QFile::rename(data[i].path,fpath); }
  }

  // store "PATH/chroto.json"
  QString fpath = outdir.filePath("chroto.json");
  std::ofstream o(fpath.toStdString());
  o << std::setw(4) << j << std::endl;

  // clear data-structure
  while ( data.size()>0 )
    data.erase(data.begin());
}

// =================================================================================================

void MainWindow::on_pushButtonT4_clean_clicked()
{
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

  // T4: reset all widgets to default
  ui->lineEditT4_date    ->clear();
  ui->lineEditT4_path    ->clear();
  ui->lineEditT4_name    ->setText("sorted");
  ui->checkBoxT4_keepOrig->setChecked(false);
  ui->pushButtonT4_write ->setEnabled(false);

  // T1: set number of folders (==2)
  ui->comboBoxT1_nfol->setCurrentIndex(1);

  // enforce opening on the first tab
  ui->tabWidget->setCurrentIndex(0);

  // emit signal to clear also all thumbnails, and empty all views
  emit dataChanged();
}

