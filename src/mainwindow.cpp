#include "mainwindow.h"
#include "ui_mainwindow.h"

// =================================================================================================

std::tuple<std::time_t,int> jpg2info ( std::string fname )
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
    throw std::runtime_error("Error parsing file");
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

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // enforce opening on the first tab
  ui->tabWidget->setCurrentIndex(0);

  // assume two sets
  ui->nfolder_comboBox->setCurrentIndex(1);

  // fill arrays collecting the file-lists and related buttons
  fileView.push_back(ui->cam_listWidget_0          );
  fileView.push_back(ui->cam_listWidget_1          );
  fileView.push_back(ui->cam_listWidget_2          );
  fileView.push_back(ui->cam_listWidget_3          );
  fileView.push_back(ui->cam_listWidget_4          );
  fileView.push_back(ui->cam_listWidget_5          );
  fileView.push_back(ui->cam_listWidget_6          );
  fileView.push_back(ui->cam_listWidget_7          );
  fileView.push_back(ui->cam_listWidget_8          );
  fileView.push_back(ui->cam_listWidget_9          );
  fileView.push_back(ui->cam_listWidget_10         );
  fileView.push_back(ui->cam_listWidget_11         );
  pathView.push_back(ui->cam_lineEdit_0            );
  pathView.push_back(ui->cam_lineEdit_1            );
  pathView.push_back(ui->cam_lineEdit_2            );
  pathView.push_back(ui->cam_lineEdit_3            );
  pathView.push_back(ui->cam_lineEdit_4            );
  pathView.push_back(ui->cam_lineEdit_5            );
  pathView.push_back(ui->cam_lineEdit_6            );
  pathView.push_back(ui->cam_lineEdit_7            );
  pathView.push_back(ui->cam_lineEdit_8            );
  pathView.push_back(ui->cam_lineEdit_9            );
  pathView.push_back(ui->cam_lineEdit_10           );
  pathView.push_back(ui->cam_lineEdit_11           );
  dirSelec.push_back(ui->cam_pushButton_0          );
  dirSelec.push_back(ui->cam_pushButton_1          );
  dirSelec.push_back(ui->cam_pushButton_2          );
  dirSelec.push_back(ui->cam_pushButton_3          );
  dirSelec.push_back(ui->cam_pushButton_4          );
  dirSelec.push_back(ui->cam_pushButton_5          );
  dirSelec.push_back(ui->cam_pushButton_6          );
  dirSelec.push_back(ui->cam_pushButton_7          );
  dirSelec.push_back(ui->cam_pushButton_8          );
  dirSelec.push_back(ui->cam_pushButton_9          );
  dirSelec.push_back(ui->cam_pushButton_10         );
  dirSelec.push_back(ui->cam_pushButton_11         );
  delSelec.push_back(ui->cam_rm_pushButton_0       );
  delSelec.push_back(ui->cam_rm_pushButton_1       );
  delSelec.push_back(ui->cam_rm_pushButton_2       );
  delSelec.push_back(ui->cam_rm_pushButton_3       );
  delSelec.push_back(ui->cam_rm_pushButton_4       );
  delSelec.push_back(ui->cam_rm_pushButton_5       );
  delSelec.push_back(ui->cam_rm_pushButton_6       );
  delSelec.push_back(ui->cam_rm_pushButton_7       );
  delSelec.push_back(ui->cam_rm_pushButton_8       );
  delSelec.push_back(ui->cam_rm_pushButton_9       );
  delSelec.push_back(ui->cam_rm_pushButton_10      );
  delSelec.push_back(ui->cam_rm_pushButton_11      );
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
  // link scroll position listWidgets
  for ( size_t i=0; i<fileView.size(); ++i )
    for ( size_t j=0; j<fileView.size(); ++j )
      if ( i!=j )
        connect(fileView[i]->verticalScrollBar(),SIGNAL(valueChanged(int)),
                fileView[j]->verticalScrollBar(),SLOT  (setValue    (int)));

  // set part of the fileWidgets invisible (made visible using "nfolder_comboBox")
  for ( size_t i=ui->nfolder_comboBox->currentIndex()+1; i<nameSort.size(); ++i ) {
    fileView[i]->setVisible(false);
    pathView[i]->setVisible(false);
    dirSelec[i]->setVisible(false);
    delSelec[i]->setVisible(false);
    nameSort[i]->setVisible(false);
  }

  // select folder / remove selected files / sort by name -> sort by time -> view in listWidge
  for ( size_t i=0; i<dirSelec.size(); ++i ) {
    connect(dirSelec[i],&QPushButton::clicked,[=](){this->selectFolder(         i );});
    connect(nameSort[i],&QPushButton::clicked,[=](){this->dataNameSort(         i );});
    connect(delSelec[i],&QPushButton::clicked,[=](){this->dataRmvSelec(fileView[i]);});
    connect(dirSelec[i],SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
    connect(delSelec[i],SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
    connect(nameSort[i],SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
    connect(dirSelec[i],SIGNAL(clicked(bool)),this,SLOT(viewFileList()));
    connect(delSelec[i],SIGNAL(clicked(bool)),this,SLOT(viewFileList()));
    connect(nameSort[i],SIGNAL(clicked(bool)),this,SLOT(viewFileList()));
  }

  // any image manipulation button pressed -> (sort by time) -> display images
  connect(ui->mvDwImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->mvDwSet_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->mvUpImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->mvUpSet_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->deltImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->jumpSel_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->exclImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->mvDwImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->mvDwSet_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->mvUpImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->mvUpSet_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->deltImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->exclImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->prevImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->nextImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->headImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->lastImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->nextBnd_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->prevBnd_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->jumpSel_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  // switch tabs: sort by time -> view list / display images / show data
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->dataTimeSort();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->viewFileList();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->displayImage();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->showDate    ();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->viewStream  ();});

  // couple outPath button to outPath line-edit
  connect(ui->outPath_pushButton,SIGNAL(clicked(bool)),this,
    SLOT(on_outPath_lineEdit_editingFinished()));
}

// =================================================================================================

MainWindow::~MainWindow()
{
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

  std::vector<int> rm(data.size(),0);

  for ( size_t i=0; i<rm.size(); ++i )
    if ( data[i].folder==folder )
      rm[i] = 1;

  size_t j = 0;
  for ( size_t i=0; i<rm.size(); ++i ) {
    if ( rm[i] ) {
      data.erase(data.begin()+i-j);
      ++j;
    }
  }

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
    QPixmap pix(50,50);
    pix.fill(QColor("white"));
    File file;
    file.camera    = camera;
    file.folder    = folder;
    file.disp      = finfo.fileName();
    file.path      = finfo.absoluteFilePath();
    file.dir       = finfo.absolutePath();
    file.time      = t;
    file.rotation  = rot;
    file.thumbnail = pix;
    if ( lst_json.size()==1 ) {
      file.time    = static_cast<std::time_t>(jdata[finfo.fileName().toStdString()]["time"  ]);
      file.camera += static_cast<size_t>     (jdata[finfo.fileName().toStdString()]["camera"]);
    }
    data.push_back(file);
  }
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

void MainWindow::dataRmvSelec(QListWidget *list)
{
  // initialize list that keep track which items have to be removed
  std::vector<int> rm(data.size(),0);

  // mark all select items for removal
  foreach (QListWidgetItem *item, list->selectedItems())
    rm[list->row(item)] = 1;

  // remove select items
  size_t j = 0;
  for ( size_t i=0; i<rm.size(); ++i ) {
    if ( rm[i] ) {
      data.erase(data.begin()+i-j);
      --idx;
      ++j;
    }
  }
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

//  ui->listWidgetT2->setViewMode  (QListWidget::IconMode);
  ui->listWidgetT2->setIconSize  (QSize(200,200)       );

  QPixmap pix(data[0].path);
  pix.scaled(50,50,Qt::KeepAspectRatio, Qt::FastTransformation);
  data[0].thumbnail = pix;

  for ( auto &i : data )
  {
    ui->listWidgetT2->addItem(new QListWidgetItem(QIcon(i.thumbnail),i.disp));
  }
}

// =================================================================================================

void MainWindow::idxViewLabel(QLabel *lab, size_t i)
{
  QPixmap    p(data[i].path);
  int        w = lab->width();
  int        h = lab->height();
  QTransform transform;
  QTransform trans = transform.rotate(data[i].rotation);
  lab->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
}

// =================================================================================================

void MainWindow::displayImage()
{
  // disable all navigation buttons: selectively enable below
  ui->prevImg_pushButton->setEnabled(false);
  ui->nextImg_pushButton->setEnabled(false);
  ui->headImg_pushButton->setEnabled(false);
  ui->lastImg_pushButton->setEnabled(false);
  ui->mvDwImg_pushButton->setEnabled(false);
  ui->mvDwSet_pushButton->setEnabled(false);
  ui->mvUpImg_pushButton->setEnabled(false);
  ui->mvUpSet_pushButton->setEnabled(false);
  ui->deltImg_pushButton->setEnabled(false);
  ui->exclImg_pushButton->setEnabled(false);
  ui->nextBnd_pushButton->setEnabled(false);
  ui->prevBnd_pushButton->setEnabled(false);
  ui->prevImg_spinBox   ->setEnabled(false);
  ui->nextImg_spinBox   ->setEnabled(false);
  ui->jump_Dw_spinBox   ->setEnabled(false);
  ui->jump_Up_spinBox   ->setEnabled(false);
  ui->jumpSel_pushButton->setEnabled(false);

  // clear currently viewed photos
  ui->view_idx_label->clear();
  ui->view_prv_label->clear();
  ui->view_nxt_label->clear();

  // no photos selected -> exit this function
  if ( data.size()==0 )
    return;

  // optionally initialized currently viewed photo "idx" to be the first
  if ( init ) {
    idx  = 0;
    init = false;
  }

  // enable deleting
  ui->deltImg_pushButton->setEnabled(true);
  ui->exclImg_pushButton->setEnabled(true);

  // enable time-jump
  if ( data.size()>1 )
    ui->jumpSel_pushButton->setEnabled(true);

  // view current photo "idx"
  this->idxViewLabel(ui->view_idx_label,idx);

  // view next photo "idx+1", and enable navigation button right
  if ( idx+1 < data.size() )
  {
    this->idxViewLabel(ui->view_nxt_label,idx+1);
    ui->nextImg_pushButton->setEnabled(true);
    ui->lastImg_pushButton->setEnabled(true);
    ui->mvUpImg_pushButton->setEnabled(true);
    ui->jump_Up_spinBox   ->setEnabled(true);
    ui->nextImg_spinBox   ->setEnabled(true);
  }

  // view previous photo "idx-1", and enable navigation button right
  if ( idx > 0 )
  {
    this->idxViewLabel(ui->view_prv_label,idx-1);
    ui->prevImg_pushButton->setEnabled(true);
    ui->headImg_pushButton->setEnabled(true);
    ui->mvDwImg_pushButton->setEnabled(true);
    ui->jump_Dw_spinBox   ->setEnabled(true);
    ui->prevImg_spinBox   ->setEnabled(true);
  }

  // check if there if a different camera somewhere in the history, and enable navigation buttons
  if ( idx>0 ) {
    for ( size_t i=idx; i>0; --i ) {
      if ( data[i-1].camera!=data[idx].camera ) {
        ui->prevBnd_pushButton->setEnabled(true);
        ui->mvDwSet_pushButton->setEnabled(true);
        break;
      }
    }
  }

  // check if there if a different camera somewhere in the future, and enable navigation buttons
  for ( size_t i=idx; i<data.size()-1; ++i ) {
    if ( data[i+1].camera!=data[idx].camera ) {
      ui->nextBnd_pushButton->setEnabled(true);
      ui->mvUpSet_pushButton->setEnabled(true);
      break;
    }
  }


}

// =================================================================================================

void MainWindow::on_prevImg_pushButton_clicked()
{
  idx -= std::min(idx,static_cast<size_t>(ui->prevImg_spinBox->value()));
}

// =================================================================================================

void MainWindow::on_nextImg_pushButton_clicked()
{
  idx += std::min(data.size()-1-idx,static_cast<size_t>(ui->nextImg_spinBox->value()));
}

// =================================================================================================

void MainWindow::on_prevBnd_pushButton_clicked()
{
  if ( idx<=1 )
    return;

  while ( data[idx-1].camera==data[idx].camera ){
    --idx;
    if ( idx==0 )
      break;
  }

  if ( idx>0 )
    --idx;
}

// =================================================================================================

void MainWindow::on_nextBnd_pushButton_clicked()
{
  if ( idx>=data.size()-3 )
    return;

  ++idx;

  while ( data[idx+1].camera==data[idx].camera ){
    ++idx;
    if ( idx==data.size()-1)
      break;
  }
}

// =================================================================================================

void MainWindow::on_headImg_pushButton_clicked()
{
  idx = 0;
}

// =================================================================================================

void MainWindow::on_lastImg_pushButton_clicked()
{
  if ( data.size()<=1 ) {
    idx = 0;
    return;
  }

  idx = data.size()-1;
}

// =================================================================================================

void MainWindow::on_mvDwImg_pushButton_clicked()
{
  size_t i = idx-std::min(idx,static_cast<size_t>(ui->jump_Dw_spinBox->value()));

  if ( i!=idx )
    data[idx].time -= data[idx].time-data[i].time+1;
}

// =================================================================================================

void MainWindow::on_mvUpImg_pushButton_clicked()
{
  size_t i = idx+std::min(data.size()-1-idx,static_cast<size_t>(ui->jump_Up_spinBox->value()));

  if ( i!=idx )
    data[idx].time += data[i].time-data[idx].time+1;
}

// =================================================================================================

void MainWindow::on_mvDwSet_pushButton_clicked()
{
  size_t i;

  for ( i=idx; i>0; --i )
    if ( data[i-1].camera!=data[idx].camera )
      break;
  --i;

  std::time_t dt = data[idx].time-data[i].time+1;

  for ( size_t j=0; j<data.size(); ++j )
    if ( data[j].camera==data[idx].camera )
      data[j].time -= dt;
}

// =================================================================================================

void MainWindow::on_mvUpSet_pushButton_clicked()
{
  size_t i;

  for ( i=idx+1; i<data.size(); ++i )
    if ( data[i].camera!=data[idx].camera )
      break;

  std::time_t dt = data[i].time-data[idx].time+1;

  for ( size_t j=0; j<data.size(); ++j )
    if ( data[j].camera==data[idx].camera )
      data[j].time += dt;
}

// =================================================================================================

void MainWindow::on_jumpSel_pushButton_clicked()
{
  // open pop-up dialog
  ImageSelection *select = new ImageSelection(this);
  // add all files to dialog
  for ( auto &file : data ) {
    select->files.push_back(file.path);
    select->disp .push_back(file.disp);
  }
  // add images to thumbnails (was delayed because of the need to transfer data)
  select->addThumbnails();
  // show dialog (only continue when OK is pressed)
  if ( !select->exec() )
    return;

  // compute time difference w.r.t. selected photo
  long dt = static_cast<long>(data[idx].time)-static_cast<long>(data[select->idx].time);

  // apply time difference
  // - either to all images
  if ( select->apply_to_all ) {
    for ( auto &file : data )
      if ( file.camera==data[idx].camera )
        file.time = static_cast<std::time_t>(static_cast<long>(file.time)-dt);
  }
  // - or to one image
  else {
    data[idx].time -= static_cast<std::time_t>(dt);
  }
}

// =================================================================================================

void MainWindow::on_exclImg_pushButton_clicked()
{
  data.erase(data.begin()+idx);

  if ( idx+1 >= data.size() )
    idx = data.size()-1;
}

// =================================================================================================

void MainWindow::on_deltImg_pushButton_clicked()
{
  File f;
  f.dir      = data[idx].dir     ;
  f.path     = data[idx].path    ;
  f.disp     = data[idx].disp    ;
  f.camera   = data[idx].camera  ;
  f.folder   = data[idx].folder  ;
  f.rotation = data[idx].rotation;
  f.time     = data[idx].time    ;
  delData.push_back(f);

  data.erase(data.begin()+idx);

  if ( idx+1 >= data.size() )
    idx = data.size()-1;
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
  init = true;
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

