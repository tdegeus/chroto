#include "mainwindow.h"
#include "ui_mainwindow.h"

// =================================================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // class to read/store list of thumbnails; assign to worker thread
  thumbnail = new Thumbnails;
  thumbnail->moveToThread(&workerThread);
  // list of thumbnails is deleted only when the worker thread is deleted (in destructor below)
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

  // T1: select folder / remove selected files / sort by name -> apply to specific "folder"
  for ( size_t i=0; i<dirSelec.size(); ++i ) {
    connect(dirSelec[i],&QPushButton::clicked,[=](){addFiles    (     i );});
    connect(nameSort[i],&QPushButton::clicked,[=](){dataNameSort(     i );});
    connect(delSelec[i],&QPushButton::clicked,[=](){listExcl(fileList[i]);});
  }
  // T2: exclude images
  connect(ui->pushButtonT2i_excl,&QPushButton::clicked,[=](){listExcl(ui->listWidgetT2);});
  connect(ui->pushButtonT2i_del ,&QPushButton::clicked,[=](){listDel (ui->listWidgetT2);});

  // "data" changed / thumbnails complete -> read thumbnails, sort by name, refresh views
  connect(this     ,SIGNAL(dataChanged  ()),this     ,SLOT(dataUpdate()));
  connect(this     ,SIGNAL(thumbnailRead()),thumbnail,SLOT(read())      );
  connect(thumbnail,SIGNAL(completed    ()),this     ,SLOT(dataUpdate()));
  // tab changed: update "data" and views to latest values
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){dataUpdate();});
  // index changes: update view
  connect(this,SIGNAL(indexChanged()),this,SLOT(viewImage()));

  // T1/T2: convert selected item to current index
  for ( auto &i: fileList ) connect(i,&QListWidget::itemSelectionChanged,[=](){list2idx(i);});
  connect(ui->listWidgetT2,&QListWidget::itemSelectionChanged,[=](){list2idx(ui->listWidgetT2);});

  // T4: couple outPath button to outPath line-edit
  connect(ui->pushButtonT4_path,SIGNAL(clicked(bool)),this,
    SLOT(on_lineEditT4_path_editingFinished()));

  // clear all
  connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(clearAll()));

  // enforce opening on the first tab
  ui->tabWidget->setCurrentIndex(0);

  // T1: set number of folders (==2)
  ui->comboBoxT1_nfol->setCurrentIndex(1);

  // T3: center image
  ui->view_idx_label->setAlignment(Qt::AlignCenter);

  // start worker thread (to which "thumbnail" is assigned)
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
  // request to stop reading thumbnails (to loose as little as time below)
  thumbnail->requestStop();

  // list with thumbnails to include
  // - initialize
  std::vector<int>    incl(thumbnail->size(),0);
  std::vector<size_t> rm;
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
  if ( idx>=data.size() )
    idx = data.size()-1;

  // sort by time
  dataTimeSort();

  // remove thumbnails that are no longer part of "data"
  thumbnail->erase(rm);
  // launch reading thumbnails that have not been read
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
  if ( dialog.exec() )
    dir = dialog.directory();
  else
    return;

  // store new suggested directory
  try { workDir = dir.filePath("../"); }
  catch (...) { workDir = workDir; }

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
  size_t      camera = 0;

  // find which camera index to use (one more than the current maximum)
  if ( data.size()>0 ) {
    for ( auto &file : data )
      camera = std::max(camera,file.camera);
    ++camera;
  }

  // read all JPG-files, if "chroto.json" exists: overwrite time from JPEG with stored values,
  // segment in the different cameras that were stored in "chroto.json"
  for ( int i=0; i<lst_jpeg.size(); ++i ) {
    // - extract file from list
    QFileInfo finfo = lst_jpeg.at(i);
    // - try the read the EXIF information. skip image if unsuccessful
    try         { std::tie(t,rot) = jpg2info(finfo.absoluteFilePath().toStdString()); }
    catch (...) { continue; }
    // - compute information to file
    File file;
    file.camera    = camera;
    file.folder    = folder;
    file.disp      = finfo.fileName();
    file.path      = finfo.absoluteFilePath();
    file.dir       = finfo.absolutePath();
    file.time      = t;
    file.rotation  = rot;
    file.ithumb    = thumbnail->push_back(file.path); // create new entry in thumbnail list
    if ( lst_json.size()==1 ) {
      file.time    = static_cast<std::time_t>(jdata[finfo.fileName().toStdString()]["time"  ]);
      file.camera += static_cast<size_t>     (jdata[finfo.fileName().toStdString()]["camera"]);
    }
    // - store in list
    data.push_back(file);
  }

  emit dataChanged();
}

// =================================================================================================

void MainWindow::listExcl(QListWidget *list)
{
  for ( size_t &i : selectedItems(list,false) )
    data.erase(data.begin()+i);

  emit dataChanged();
}

// =================================================================================================

void MainWindow::listDel(QListWidget *list)
{
  for ( size_t &i : selectedItems(list,false) ) {
    delData.push_back(data[i]);
    data.erase(data.begin()+i);
  }

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
}

// =================================================================================================

void MainWindow::viewStream(void)
{
  // empty the list
  while ( ui->listWidgetT2->count()>0 )
    ui->listWidgetT2->takeItem(0);

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
  ui->listWidgetT2->setIconSize(QSize(200,200));

  // fill the lists with names / icons
  for ( auto &i : data )
    ui->listWidgetT2->addItem(new QListWidgetItem(thumbnail->at(i.ithumb),i.disp));

  // set background color, corresponding to the camera index
  for ( size_t i=0; i<data.size(); ++i )
    ui->listWidgetT2->item(i)->setBackground(QBrush(col[data[i].camera]));

}

// =================================================================================================

void MainWindow::dataNameSort(size_t folder)
{
  // quit function if no files are present
  if ( data.size()<=0 )
    return;

  // store current order, to retrieve the new position of "idx"
  for ( size_t i=0; i<data.size(); ++i )
    data[i].index = i;

  // only sort for this folder (items with "data[i].sort == false" are left untouched)
  for ( auto &file : data ) {
    if ( file.folder==folder ) { file.sort = true;  }
    else                       { file.sort = false; }
  }

  // apply selective sort, based on file-name
  std::sort(data.begin(),data.end(),
    [](File i,File j){
      if ( i.sort && j.sort ) return i.path.toStdString()<j.path.toStdString();
      else                    return i.index<j.index;
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
      return;
    }
  }

  // make sure that all images are at least one second apart
  // this is needed to allow image insertion between image
  for ( size_t i=0; i<data.size()-1; ++i )
    if ( data[i+1].time<=data[i].time )
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
  // number of characters needed the fit the images
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
  // - loop over images
  for ( int i=0; i<static_cast<int>(data.size()); ++i ) {
    // - format -> filename
    QString fname = ui->lineEditT4_name->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
    QString fpath = outdir.filePath(fname);
    // - store information to JSON-struct
    j[fname.toStdString()]["camera"] = data[i].camera;
    j[fname.toStdString()]["time"  ] = static_cast<long>(data[i].time);
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
