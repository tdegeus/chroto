#include "mainwindow.h"
#include "ui_mainwindow.h"

// ============================================================================

std::tuple<std::time_t,int> jpg2info ( std::string fname )
{
  // read ".jpg" file into a buffer
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
  iss.str(result.DateTime);
  iss >> std::get_time(&tm,"%Y:%m:%d %H:%M:%S");
  std::time_t t = mktime(&tm);

  // convert orientation to rotation
  int rot = 0;
  if      ( result.Orientation==8 ) rot = -90;
  else if ( result.Orientation==6 ) rot =  90;

  return std::make_tuple(t,rot);
}

// ============================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // enforce opening on the first tab
  ui->tabWidget->setCurrentIndex(0);

  // fill arrays collecting the file-lists and related buttons
  fileView.push_back(ui->cam0_listWidget   );
  fileView.push_back(ui->cam1_listWidget   );
  fileView.push_back(ui->cam2_listWidget   );
  fileView.push_back(ui->cam3_listWidget   );
  fileView.push_back(ui->cam4_listWidget   );
  pathView.push_back(ui->cam0_lineEdit     );
  pathView.push_back(ui->cam1_lineEdit     );
  pathView.push_back(ui->cam2_lineEdit     );
  pathView.push_back(ui->cam3_lineEdit     );
  pathView.push_back(ui->cam4_lineEdit     );
  dirSelec.push_back(ui->cam0_pushButton   );
  dirSelec.push_back(ui->cam1_pushButton   );
  dirSelec.push_back(ui->cam2_pushButton   );
  dirSelec.push_back(ui->cam3_pushButton   );
  dirSelec.push_back(ui->cam4_pushButton   );
  delSelec.push_back(ui->cam0_rm_pushButton);
  delSelec.push_back(ui->cam1_rm_pushButton);
  delSelec.push_back(ui->cam2_rm_pushButton);
  delSelec.push_back(ui->cam3_rm_pushButton);
  delSelec.push_back(ui->cam4_rm_pushButton);
  nameSort.push_back(ui->cam0_nameSort_pushButton);
  nameSort.push_back(ui->cam1_nameSort_pushButton);
  nameSort.push_back(ui->cam2_nameSort_pushButton);
  nameSort.push_back(ui->cam3_nameSort_pushButton);
  nameSort.push_back(ui->cam4_nameSort_pushButton);

  // select folder / remove selected files / sort files / view files
  for ( size_t i=0 ; i<dirSelec.size() ; ++i ) {
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
  // link scroll position listWidgets
  for ( size_t i=0 ; i<fileView.size() ; ++i )
    for ( size_t j=0 ; j<fileView.size() ; ++j )
      if ( i!=j )
        connect(fileView[i]->verticalScrollBar(),SIGNAL(valueChanged(int)),fileView[j]->verticalScrollBar(),SLOT(setValue(int)));

  // re-sort data / update file-list / display image
  connect(ui->mvDwImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->mvDwSet_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->mvUpImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->mvUpSet_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->exclImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataTimeSort()));
  connect(ui->mvDwImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->mvDwSet_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->mvUpImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->mvUpSet_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->exclImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->prevImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->nextImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->headImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->lastImg_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->nextBnd_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->prevBnd_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  // re-sort data / update file-list / display image
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->dataTimeSort();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->viewFileList();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->displayImage();});

  // select output folder
  connect(ui->outPath_pushButton,SIGNAL(clicked(bool)),this,SLOT(on_outPath_lineEdit_editingFinished()));
}

// ============================================================================

MainWindow::~MainWindow()
{
  delete ui;
}

// ============================================================================

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   this->displayImage();
}

// ============================================================================

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

// ============================================================================

void MainWindow::selectFolder(size_t camera)
{
  // if list is not empty: remove previous selection
  // -----------------------------------------------

  std::vector<int> rm(data.size(),0);

  for ( size_t i = 0; i < rm.size(); ++i )
    if ( data[i].camera==camera )
      rm[i] = 1;

  size_t j = 0;
  for ( size_t i = 0; i < rm.size(); ++i ) {
    if ( rm[i] ) {
      data.erase(data.begin()+i-j);
      ++j;
    }
  }

  // select folder -> read files
  // ---------------------------

  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();

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
    QFileInfo finfo = lst_json.at(0);
    std::ifstream inp(finfo.absoluteFilePath().toStdString());
    inp >> jdata;
  }

  std::time_t t;
  int rot;

  // read all "*.jpg" files, optionally overwrite time with time from "chroto.json"
  for ( int i = 0; i < lst_jpeg.size(); ++i ) {
    QFileInfo finfo = lst_jpeg.at(i);
    try         { std::tie(t,rot) = jpg2info(finfo.absoluteFilePath().toStdString()); }
    catch (...) { continue; }
    File file;
    file.camera    = camera;
    file.disp      = finfo.fileName();
    file.path      = finfo.absoluteFilePath();
    file.dir       = finfo.absolutePath();
    file.time      = t;
    file.rotation  = rot;
    if ( lst_json.size()==1 )
      file.time = static_cast<std::time_t>(jdata[finfo.fileName().toStdString()]["time"]);
    data.push_back(file);
  }
}

// ============================================================================

void MainWindow::dataNameSort(size_t camera)
{
  for ( size_t i = 0; i < data.size(); ++i )
    data[i].index = i;

  for ( size_t i = 0; i < data.size(); ++i ) {
    if ( data[i].camera==camera )
      data[i].sort = true;
    else
      data[i].sort = false;
  }

  std::sort(data.begin(),data.end(),
    [](File i,File j){
      if ( i.sort && j.sort ) return i.path.toStdString()<j.path.toStdString();
      else                    return i.index<j.index;
  });

  for ( size_t i=data.size()-1 ; i>0 ; --i )
    if ( data[i-1].time > data[i].time )
      data[i-1].time = data[i].time-1;

  for ( size_t i = 0; i < data.size(); ++i ) {
    if ( data[i].index==idx ) {
      idx = i;
      return;
    }
  }
}

// ============================================================================

void MainWindow::dataTimeSort()
{
  for ( size_t i = 0; i < data.size(); ++i ) {
    data[i].index = i;
  }

  std::sort(data.begin(),data.end(),
    [](File i,File j){
      if ( i.time==j.time ) return i.index<j.index;
      else                  return i.time <j.time ;
  });

  for ( size_t i = 0; i < data.size(); ++i ) {
    if ( data[i].index==idx ) {
      idx = i;
      return;
    }
  }
}

// ============================================================================

void MainWindow::dataRmvSelec(QListWidget *list)
{
  QList<QListWidgetItem*> items = list->selectedItems();

  std::vector<int> rm(data.size(),0);

  foreach (QListWidgetItem *item, items)
    rm[list->row(item)] = 1;

  size_t j = 0;
  for ( size_t i = 0; i < rm.size(); ++i ) {
    if ( rm[i] ) {
      data.erase(data.begin()+i-j);
      --idx;
      ++j;
    }
  }

  if ( idx<1 )
    idx = 0;
}

// ============================================================================

void MainWindow::viewFileList()
{
  // empty listWidgets
  for ( size_t l=0 ; l<fileView.size() ; ++l )
    while(fileView[l]->count()>0)
      fileView[l]->takeItem(0);

  // empty list -> exit
  if ( data.size()==0 )
    return;

  // fill listWidgets
  for ( size_t i=0 ; i<data.size() ; ++i ) {
    for ( size_t l=0 ; l<fileView.size() ; ++l ) {
      if ( data[i].camera==l ) {
        fileView[l]->addItem(data[i].disp);
        pathView[l]->setText(data[i].dir);
      }
      else {
        fileView[l]->addItem("");
      }
    }
  }
}

// ============================================================================

void MainWindow::idxViewLabel(QLabel *lab, size_t i)
{
  QPixmap    p(data[i].path);
  int        w = lab->width();
  int        h = lab->height();
  QTransform transform;
  QTransform trans = transform.rotate(data[i].rotation);
  lab->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
}

// ============================================================================

void MainWindow::displayImage()
{
  ui->prevImg_pushButton->setEnabled(false);
  ui->nextImg_pushButton->setEnabled(false);
  ui->headImg_pushButton->setEnabled(false);
  ui->lastImg_pushButton->setEnabled(false);
  ui->mvDwImg_pushButton->setEnabled(false);
  ui->mvDwSet_pushButton->setEnabled(false);
  ui->mvUpImg_pushButton->setEnabled(false);
  ui->mvUpSet_pushButton->setEnabled(false);
  ui->exclImg_pushButton->setEnabled(false);
  ui->nextBnd_pushButton->setEnabled(false);
  ui->prevBnd_pushButton->setEnabled(false);

  ui->view_idx_label->clear();
  ui->view_prv_label->clear();
  ui->view_nxt_label->clear();

  if ( data.size()==0 )
    return;

  if ( init ) {
    idx  = 0;
    init = false;
  }

  ui->exclImg_pushButton->setEnabled(true);

  if ( idx>0 ) {
    for ( size_t i=0 ; i<idx ; ++i ) {
      if ( data[i].camera!=data[i+1].camera ) {
        ui->prevBnd_pushButton->setEnabled(true);
        break;
      }
    }
  }

  for ( size_t i=idx+1 ; i<data.size()-1 ; ++i ) {
    if ( data[i].camera!=data[i+1].camera ) {
      ui->nextBnd_pushButton->setEnabled(true);
      break;
    }
  }

  this->idxViewLabel(ui->view_idx_label,idx);

  if ( idx+1 < data.size() )
  {
    this->idxViewLabel(ui->view_nxt_label,idx+1);
    ui->nextImg_pushButton->setEnabled(true);
    ui->lastImg_pushButton->setEnabled(true);
    ui->mvUpImg_pushButton->setEnabled(true);
    if ( data[idx].camera!=data[idx+1].camera )
      ui->mvUpSet_pushButton->setEnabled(true);
  }

  if ( idx > 0 )
  {
    this->idxViewLabel(ui->view_prv_label,idx-1);
    ui->prevImg_pushButton->setEnabled(true);
    ui->headImg_pushButton->setEnabled(true);
    ui->mvDwImg_pushButton->setEnabled(true);
    if ( data[idx].camera!=data[idx-1].camera )
      ui->mvDwSet_pushButton->setEnabled(true);
  }

}

// ============================================================================

void MainWindow::on_prevImg_pushButton_clicked()
{
  if ( idx>0 )
    --idx;
}

// ============================================================================

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

// ============================================================================

void MainWindow::on_nextImg_pushButton_clicked()
{
  if ( idx==data.size()-1 )
    return;

  ++idx;
}

// ============================================================================

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

// ============================================================================

void MainWindow::on_headImg_pushButton_clicked()
{
  idx = 0;
}

// ============================================================================

void MainWindow::on_lastImg_pushButton_clicked()
{
  if ( data.size()<=1 ) {
    idx = 0;
    return;
  }

  idx = data.size()-1;
}

// ============================================================================

void MainWindow::on_mvDwImg_pushButton_clicked()
{
  if ( idx==0 )
    return;

  data[idx].time -= data[idx].time-data[idx-1].time+1;

  int i = static_cast<int>(idx)-2;
  while ( i>=0 &&  data[i].time==data[idx].time ) {
    data[i].time -= 1;
    --i;
  }
}

// ============================================================================

void MainWindow::on_mvDwSet_pushButton_clicked()
{
  size_t t0 = data[idx].time;
  this->on_mvDwImg_pushButton_clicked();

  for ( size_t i = 0; i < data.size(); ++i )
    if ( i!=idx && data[i].camera==data[idx].camera )
      data[i].time -= t0-data[idx].time;
}

// ============================================================================

void MainWindow::on_mvUpImg_pushButton_clicked()
{
  if ( idx==data.size()-1 )
    return;

  data[idx].time += data[idx+1].time-data[idx].time+1;

  size_t i = idx+2;
  while ( i<data.size() && data[i].time==data[idx].time ) {
    data[i].time += 1;
    ++i;
  }
}

// ============================================================================

void MainWindow::on_mvUpSet_pushButton_clicked()
{
  size_t t0 = data[idx].time;
  this->on_mvUpImg_pushButton_clicked();

  for ( size_t i = 0; i < data.size(); ++i )
    if ( i!=idx && data[i].camera==data[idx].camera )
      data[i].time += data[idx].time-t0;
}

// ============================================================================

void MainWindow::on_exclImg_pushButton_clicked()
{
  data.erase(data.begin()+idx);

  if ( idx>0 )
    --idx;
}

// ============================================================================

void MainWindow::on_outPath_pushButton_clicked()
{
  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();

  ui->outPath_lineEdit->setText(dir.absolutePath());
}

// ============================================================================

void MainWindow::on_outPath_lineEdit_editingFinished()
{
  if ( ui->outPath_lineEdit->text().length()>0 )
    ui->write_pushButton->setEnabled(true);
  else
    ui->write_pushButton->setEnabled(false);
}

// ============================================================================

void MainWindow::on_write_pushButton_clicked()
{
  // number of characters needed the fit the images
  // (ignore that the removed image might reduce N)
  int N = QString::number(data.size()).length();

  // check if output files exist
  // - "PATH/chroto.json"
  QFileInfo finfo(QDir(ui->outPath_lineEdit->text()).filePath("chroto.json"));
  if ( finfo.isFile() ) {
    this->promptWarning(\
      tr("The file '%1' already exists, please select an empty directory").arg("chroto.json"));
    return;
  }
  // - "PATH/NAME-%0Xd.jpg"
  for ( int i=0 ; i<static_cast<int>(data.size()) ; ++i ) {
    QString   fname = ui->name_lineEdit->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
    QFileInfo finfo(QDir(ui->outPath_lineEdit->text()).filePath(fname));
    if ( finfo.isFile() ) {
      this->promptWarning(\
        tr("The file '%1' already exists, please select an empty directory").arg(fname));
      return;
    }
  }

  // write output
  // - allocate JSON-struct
  json j;
  // - loop over images
  for ( int i=0 ; i<static_cast<int>(data.size()) ; ++i ) {
    // - format -> filename
    QString fname = ui->name_lineEdit->text()+QString("-")+QString("%1.jpg").arg(i,N,10,QChar('0'));
    QString fpath = QDir(ui->outPath_lineEdit->text()).filePath(fname);
    // - store infomration to JSON-structu
    j[fname.toStdString()]["origin"] = data[i].path.toStdString();
    j[fname.toStdString()]["time"  ] = static_cast<long>(data[i].time);
    // - copy or move
    if ( ui->cp_checkBox->isChecked() ) { QFile::copy(  data[i].path,fpath); }
    else                                { QFile::rename(data[i].path,fpath); }
  }

  // store "PATH/chroto.json"
  QString fpath = QDir(ui->outPath_lineEdit->text()).filePath("chroto.json");
  std::ofstream o(fpath.toStdString());
  o << std::setw(4) << j << std::endl;
}


