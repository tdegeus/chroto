#include "mainwindow.h"
#include "ui_mainwindow.h"

// =================================================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // T1: fill arrays collecting the file-lists and related buttons
  fileView.push_back(ui->cam_listWidget_0 ); pathView.push_back(ui->cam_lineEdit_0      );
  fileView.push_back(ui->cam_listWidget_1 ); pathView.push_back(ui->cam_lineEdit_1      );
  fileView.push_back(ui->cam_listWidget_2 ); pathView.push_back(ui->cam_lineEdit_2      );
  fileView.push_back(ui->cam_listWidget_3 ); pathView.push_back(ui->cam_lineEdit_3      );
  fileView.push_back(ui->cam_listWidget_4 ); pathView.push_back(ui->cam_lineEdit_4      );
  fileView.push_back(ui->cam_listWidget_5 ); pathView.push_back(ui->cam_lineEdit_5      );
  fileView.push_back(ui->cam_listWidget_6 ); pathView.push_back(ui->cam_lineEdit_6      );
  fileView.push_back(ui->cam_listWidget_7 ); pathView.push_back(ui->cam_lineEdit_7      );
  fileView.push_back(ui->cam_listWidget_8 ); pathView.push_back(ui->cam_lineEdit_8      );
  fileView.push_back(ui->cam_listWidget_9 ); pathView.push_back(ui->cam_lineEdit_9      );
  fileView.push_back(ui->cam_listWidget_10); pathView.push_back(ui->cam_lineEdit_10     );
  fileView.push_back(ui->cam_listWidget_11); pathView.push_back(ui->cam_lineEdit_11     );
  // -
  dirSelec.push_back(ui->cam_pushButton_0 ); delSelec.push_back(ui->cam_rm_pushButton_0 );
  dirSelec.push_back(ui->cam_pushButton_1 ); delSelec.push_back(ui->cam_rm_pushButton_1 );
  dirSelec.push_back(ui->cam_pushButton_2 ); delSelec.push_back(ui->cam_rm_pushButton_2 );
  dirSelec.push_back(ui->cam_pushButton_3 ); delSelec.push_back(ui->cam_rm_pushButton_3 );
  dirSelec.push_back(ui->cam_pushButton_4 ); delSelec.push_back(ui->cam_rm_pushButton_4 );
  dirSelec.push_back(ui->cam_pushButton_5 ); delSelec.push_back(ui->cam_rm_pushButton_5 );
  dirSelec.push_back(ui->cam_pushButton_6 ); delSelec.push_back(ui->cam_rm_pushButton_6 );
  dirSelec.push_back(ui->cam_pushButton_7 ); delSelec.push_back(ui->cam_rm_pushButton_7 );
  dirSelec.push_back(ui->cam_pushButton_8 ); delSelec.push_back(ui->cam_rm_pushButton_8 );
  dirSelec.push_back(ui->cam_pushButton_9 ); delSelec.push_back(ui->cam_rm_pushButton_9 );
  dirSelec.push_back(ui->cam_pushButton_10); delSelec.push_back(ui->cam_rm_pushButton_10);
  dirSelec.push_back(ui->cam_pushButton_11); delSelec.push_back(ui->cam_rm_pushButton_11);
  // -
  nameSort.push_back(ui->cam_nameSort_pushButton_0 );
  nameSort.push_back(ui->cam_nameSort_pushButton_1 );
  nameSort.push_back(ui->cam_nameSort_pushButton_2 );
  nameSort.push_back(ui->cam_nameSort_pushButton_3 );
  nameSort.push_back(ui->cam_nameSort_pushButton_4 );
  nameSort.push_back(ui->cam_nameSort_pushButton_5 );
  nameSort.push_back(ui->cam_nameSort_pushButton_6 );
  nameSort.push_back(ui->cam_nameSort_pushButton_7 );
  nameSort.push_back(ui->cam_nameSort_pushButton_8 );
  nameSort.push_back(ui->cam_nameSort_pushButton_9 );
  nameSort.push_back(ui->cam_nameSort_pushButton_10);
  nameSort.push_back(ui->cam_nameSort_pushButton_11);

  // T1: link scroll position listWidgets
  for ( auto &i: fileView )
    for ( auto &j: fileView )
      if ( i!=j )
        connect(i->verticalScrollBar(),SIGNAL(valueChanged(int)),
                j->verticalScrollBar(),SLOT  (setValue    (int)));

  // T1: select folder / remove selected files / sort by name -> apply to specific "folder"
  for ( size_t i=0; i<dirSelec.size(); ++i ) {
    connect(dirSelec[i],&QPushButton::clicked,[=](){selectFolder(       i );});
    connect(nameSort[i],&QPushButton::clicked,[=](){dataNameSort(       i );});
    connect(delSelec[i],&QPushButton::clicked,[=](){selectExcl(fileView[i]);});
  }
  // T1: sort by time
  for ( auto &i : dirSelec ) connect(i,SIGNAL(clicked(bool)),SLOT(dataTimeSort()));
  for ( auto &i : delSelec ) connect(i,SIGNAL(clicked(bool)),SLOT(dataTimeSort()));
  for ( auto &i : nameSort ) connect(i,SIGNAL(clicked(bool)),SLOT(dataTimeSort()));
  // T1: update lists with files
  for ( auto &i : dirSelec ) connect(i,SIGNAL(clicked(bool)),SLOT(viewFileList()));
  for ( auto &i : delSelec ) connect(i,SIGNAL(clicked(bool)),SLOT(viewFileList()));
  for ( auto &i : nameSort ) connect(i,SIGNAL(clicked(bool)),SLOT(viewFileList()));

  // T1: create list of thumbnails, and assign to worker thread
  thumbnail = new Thumbnails;
  thumbnail->moveToThread(&workerThread);
  // T1: mark that list of thumbnails is deleted only when the worked thread is deleted
  connect(&workerThread,&QThread::finished,thumbnail,&QObject::deleteLater);
  // T1: files read -> launch reading thumbnails
  for ( auto &i : dirSelec ) connect(i,&QPushButton::clicked,thumbnail,&Thumbnails::read);
  // T1: start worker thread
  workerThread.start();

  // T1/T2: transfer selected item to current index
  for ( auto &i: fileView ) connect(i,&QListWidget::itemSelectionChanged,[=](){select2idx(i);});
  connect(ui->listWidgetT2,&QListWidget::itemSelectionChanged,[=](){select2idx(ui->listWidgetT2);});

  // T2: exclude images
  connect(ui->pushButtonT2i_excl,&QPushButton::clicked,[=](){selectExcl(ui->listWidgetT2);});

  // T2: all buttons
  {
    std::vector<QPushButton*> btn;
    btn.push_back(ui->pushButtonT2i_del ); btn.push_back(ui->pushButtonT2i_excl);
    btn.push_back(ui->pushButtonT2i_dwn ); btn.push_back(ui->pushButtonT2i_up  );
    btn.push_back(ui->pushButtonT2c_dwn ); btn.push_back(ui->pushButtonT2c_up  );
    btn.push_back(ui->pushButtonT2c_sync); btn.push_back(ui->pushButtonT2c_sync);
    for ( auto &i : btn ) connect(i,SIGNAL(clicked(bool)),SLOT(dataTimeSort()));
    for ( auto &i : btn ) connect(i,SIGNAL(clicked(bool)),SLOT(viewStream()));
  }
  // T3: collect all buttons
  {
    std::vector<QPushButton*> btn;
    btn.push_back(ui->exclImg_pushButton); btn.push_back(ui->deltImg_pushButton);
    btn.push_back(ui->prevImg_pushButton); btn.push_back(ui->nextImg_pushButton);
    btn.push_back(ui->headImg_pushButton); btn.push_back(ui->lastImg_pushButton);
    // T3: update view
    for ( auto &i : btn ) connect(i,SIGNAL(clicked(bool)),SLOT(displayImage()));
  }

  // switch tabs: sort by time -> view list / display images / show data
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){dataTimeSort();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){viewFileList();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){displayImage();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){showDate    ();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){viewStream  ();});

  // T4: couple outPath button to outPath line-edit
  connect(ui->outPath_pushButton,SIGNAL(clicked(bool)),this,
    SLOT(on_outPath_lineEdit_editingFinished()));

  // enforce opening on the first tab
  ui->tabWidget->setCurrentIndex(0);

  // T1: set number of folders (==2)
  ui->nfolder_comboBox->setCurrentIndex(1);
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
   this->displayImage();
}

// =================================================================================================

void MainWindow::promptWarning ( QString msg )
{
  QMessageBox::warning(\
    this,\
    tr("chroto"),\
    msg,\
    QMessageBox::Ok,\
    QMessageBox::Ok\
  );
}

// =================================================================================================

void MainWindow::on_nfolder_comboBox_currentIndexChanged(int index)
{
  // set all invisible
  for ( size_t i=0; i<nameSort.size(); ++i ) {
    fileView[i]->setVisible(false);
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
    ui->nfolder_comboBox->setCurrentIndex(index);
  }

  // enable requested number of columns
  for ( int i=0; i<std::min(index+1,static_cast<int>(nameSort.size())); ++i ) {
    fileView[i]->setVisible(true);
    pathView[i]->setVisible(true);
    dirSelec[i]->setVisible(true);
    delSelec[i]->setVisible(true);
    nameSort[i]->setVisible(true);
  }
}

// =================================================================================================

void MainWindow::selectFolder(size_t folder)
{
  // if list is not empty: remove previous selection
  // -----------------------------------------------

  std::vector<size_t> rm;

  for ( size_t i=0; i<data.size(); ++i )
    if ( data[i].folder==folder )
      rm.push_back(i);

  selectDel(rm,false);


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
    QFileInfo finfo = lst_jpeg.at(i);
    try         { std::tie(t,rot) = jpg2info(finfo.absoluteFilePath().toStdString()); }
    catch (...) { continue; }

    size_t ithumb = data.size();

    File file;
    file.camera    = camera;
    file.folder    = folder;
    file.disp      = finfo.fileName();
    file.path      = finfo.absoluteFilePath();
    file.dir       = finfo.absolutePath();
    file.time      = t;
    file.rotation  = rot;
    file.ithumb    = ithumb;
    if ( lst_json.size()==1 ) {
      file.time    = static_cast<std::time_t>(jdata[finfo.fileName().toStdString()]["time"  ]);
      file.camera += static_cast<size_t>     (jdata[finfo.fileName().toStdString()]["camera"]);
    }
    data.push_back(file);
    thumbnail->push_back(file.path);
  }
}

// =================================================================================================

void MainWindow::selectExcl(QListWidget *list)
{
  for ( size_t &i : selectedItems(list,false) )
    data.erase(data.begin()+i);
}

// =================================================================================================

void MainWindow::select2idx(QListWidget *list)
{
  std::vector<size_t> sel = selectedItems(list);

  if ( sel.size()>0 ) {
    idx = sel[0];
    return;
  }

  if ( idx>=data.size() )
    idx = data.size()-1;
}

// =================================================================================================

void MainWindow::viewFileList()
{
  // empty storage paths
  for ( auto line : pathView )
    line->clear();
  // empty listWidgets
  for ( auto list : fileView )
    while ( list->count()>0 )
      list->takeItem(0);

  // no photos selected -> quit this function
  if ( data.size()==0 )
    return;

  // fill listWidgets
  for ( auto &file : data ) {
    for ( size_t l=0; l<fileView.size(); ++l ) {
      if ( file.folder==l ) {
        fileView[l]->addItem(file.disp);
        pathView[l]->setText(file.dir );
      }
      else {
        fileView[l]->addItem("");
      }
    }
  }

  for ( size_t l=0; l<fileView.size(); ++l )
    fileView[l]->setCurrentRow(idx);
}

// =================================================================================================

void MainWindow::viewStream(void)
{
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

  // empty the list
  while ( ui->listWidgetT2->count()>0 )
    ui->listWidgetT2->takeItem(0);

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
      return;
    }
  }
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

void MainWindow::displayImage()
{
  // selectively disable/enable navigation buttons
  ui->prevImg_pushButton->setEnabled(idx>0            );
  ui->headImg_pushButton->setEnabled(idx>0            );
  ui->nextImg_pushButton->setEnabled(idx<data.size()-1);
  ui->lastImg_pushButton->setEnabled(idx<data.size()-1);
  ui->deltImg_pushButton->setEnabled(data.size()>0    );
  ui->exclImg_pushButton->setEnabled(data.size()>0    );

  // no photos selected -> exit this function
  if ( data.size()==0 )
    return;

  // clear currently viewed photos
  ui->view_idx_label->clear();
  // view current photo "idx"
  QPixmap    p(data[idx].path);
  int        w = ui->view_idx_label->width();
  int        h = ui->view_idx_label->height();
  QTransform transform;
  QTransform trans = transform.rotate(data[idx].rotation);
  ui->view_idx_label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
}

// =================================================================================================

void MainWindow::on_prevImg_pushButton_clicked()
{
  idx -= 1;
}

// =================================================================================================

void MainWindow::on_nextImg_pushButton_clicked()
{
  idx += 1;
}

// =================================================================================================

void MainWindow::on_headImg_pushButton_clicked()
{
  idx = 0;
}

// =================================================================================================

void MainWindow::on_lastImg_pushButton_clicked()
{
  idx = data.size()-1;
}

// =================================================================================================

void MainWindow::on_exclImg_pushButton_clicked()
{
  std::vector<size_t> rm = {idx};
  selectDel(rm,false);
}

// =================================================================================================

void MainWindow::on_deltImg_pushButton_clicked()
{
  std::vector<size_t> rm = {idx};
  selectDel(rm,true);
}

// =================================================================================================

void MainWindow::showDate()
{
  if ( data.size()<=0 )
    return;

  std::string str(std::ctime(&data[0].time));

  ui->date_lineEdit->setText(QString::fromStdString(str));
}

// =================================================================================================

void MainWindow::on_outPath_pushButton_clicked()
{
  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(workDir);
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();

  ui->outPath_lineEdit->setText(dir.absolutePath());

  // store new suggested directory
  workDir = dir.absolutePath();
}

// =================================================================================================

void MainWindow::on_outPath_lineEdit_editingFinished()
{
  if ( ui->outPath_lineEdit->text().length()>0 )
    ui->write_pushButton->setEnabled(true);
  else
    ui->write_pushButton->setEnabled(false);
}

// =================================================================================================

void MainWindow::on_write_pushButton_clicked()
{
  // number of characters needed the fit the images
  // (ignore that the removed image might reduce N)
  int N = QString::number(data.size()).length();

  // create output directory, if it does not exist
  QDir outdir(ui->outPath_lineEdit->text());
  if ( !outdir.exists() )
    QDir().mkdir(ui->outPath_lineEdit->text());

  // check if output files exist
  // - "PATH/chroto.json"
  if ( QFileInfo(outdir.filePath("chroto.json")).isFile() ) {
    this->promptWarning(\
      tr("The file '%1' already exists, please select an empty directory").arg("chroto.json"));
    return;
  }
  // - "PATH/NAME-%0Xd.jpg"
  for ( int i=0; i<static_cast<int>(data.size()); ++i ) {
    QString fname = ui->name_lineEdit->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
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
    QString fname = ui->name_lineEdit->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
    QString fpath = outdir.filePath(fname);
    // - store information to JSON-struct
    j[fname.toStdString()]["camera"] = data[i].camera;
    j[fname.toStdString()]["time"  ] = static_cast<long>(data[i].time);
    j["camera"][std::to_string(data[i].camera)] = data[i].dir.toStdString();
    // - copy or move
    if ( ui->cp_checkBox->isChecked() ) { QFile::copy(  data[i].path,fpath); }
    else                                { QFile::rename(data[i].path,fpath); }
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

void MainWindow::on_clean_pushButton_clicked()
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
  while ( cleanPaths.size()>0 )
    cleanPaths.erase(cleanPaths.begin());

  while ( delData.size()>0 )
    delData.erase(delData.begin());
}



void MainWindow::selectDel(std::vector<size_t> rm, bool del)
{
  if ( rm.size()==0 )
    return;

  if ( del )
    for ( auto &i: rm )
      delData.push_back(data[i]);

  std::vector<int> rm_thumb;

  for ( auto &i : rm )
    rm_thumb.push_back(data[i].ithumb);

  std::vector<int> incl(thumbnail->size(),1);
  for ( auto &i : rm_thumb )
    incl[i] = 0;

  for ( size_t i=1; i<incl.size(); ++i )
    incl[i] += incl[i-1];

  for ( auto &i: incl )
    --i;

  std::sort(rm.begin(),rm.end(),[](size_t i,size_t j){return i>j;});
  for ( auto &i : rm )
    data.erase(data.begin()+i);

  thumbnail->pop_back(rm_thumb);

  for ( auto &i: data )
    i.ithumb = incl[i.ithumb];


  if ( idx+1 >= data.size() )
    idx = data.size()-1;

}


void MainWindow::on_pushButtonT2i_del_clicked()
{
  std::vector<size_t> sel = selectedItems(ui->listWidgetT2);

  for ( auto &i: sel )
    delData.push_back(data[i]);

  for ( auto &i: sel )
    data.erase(data.begin()+i);

  if ( idx+1 >= data.size() )
    idx = data.size()-1;
}

void MainWindow::on_pushButtonT2i_up_clicked()
{
  std::vector<size_t> sel = selectedItems(ui->listWidgetT2);

  for ( auto &i: sel ) {
    if ( i>0 ) {
      data[i].time -= data[i].time-data[i-1].time-1;
    }
  }

}
