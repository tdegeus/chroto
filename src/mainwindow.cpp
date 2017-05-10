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

  // assume two sets
  ui->comboBox->setCurrentIndex(1);

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

  for ( size_t i=ui->comboBox->currentIndex()+1 ; i<nameSort.size() ; ++i ) {
    fileView[i]->setVisible(false);
    pathView[i]->setVisible(false);
    dirSelec[i]->setVisible(false);
    delSelec[i]->setVisible(false);
    nameSort[i]->setVisible(false);
  }

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
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->showDate    ();});

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

void MainWindow::selectFolder(size_t folder)
{
  // if list is not empty: remove previous selection
  // -----------------------------------------------

  std::vector<int> rm(data.size(),0);

  for ( size_t i = 0; i < rm.size(); ++i )
    if ( data[i].folder==folder )
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
  dialog.setDirectory(workDir);
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();

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
    QFileInfo finfo = lst_json.at(0);
    std::ifstream inp(finfo.absoluteFilePath().toStdString());
    inp >> jdata;
  }

  std::time_t t;
  int rot;
  size_t camera = 0;
  for ( size_t i=0 ; i<data.size() ; ++i )
    camera = std::max(camera,data[i].camera);
  ++camera;

  // read all "*.jpg" files, optionally overwrite time with time from "chroto.json"
  for ( int i = 0; i < lst_jpeg.size(); ++i ) {
    QFileInfo finfo = lst_jpeg.at(i);
    try         { std::tie(t,rot) = jpg2info(finfo.absoluteFilePath().toStdString()); }
    catch (...) { continue; }
    File file;
    file.camera    = camera;
    file.folder    = folder;
    file.disp      = finfo.fileName();
    file.path      = finfo.absoluteFilePath();
    file.dir       = finfo.absolutePath();
    file.time      = t;
    file.rotation  = rot;
    if ( lst_json.size()==1 ) {
      file.time    = static_cast<std::time_t>(jdata[finfo.fileName().toStdString()]["time"  ]);
      file.camera += static_cast<size_t>     (jdata[finfo.fileName().toStdString()]["camera"]);
    }
    data.push_back(file);
  }
}

// ============================================================================

void MainWindow::dataNameSort(size_t folder)
{
  for ( size_t i = 0; i < data.size(); ++i )
    data[i].index = i;

  for ( size_t i = 0; i < data.size(); ++i ) {
    if ( data[i].folder==folder )
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
      if ( data[i].folder==l ) {
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
  ui->jump_Dw_spinBox->setEnabled(false);
  ui->jump_Up_spinBox->setEnabled(false);

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

  this->idxViewLabel(ui->view_idx_label,idx);

  if ( idx+1 < data.size() )
  {
    this->idxViewLabel(ui->view_nxt_label,idx+1);
    ui->nextImg_pushButton->setEnabled(true);
    ui->lastImg_pushButton->setEnabled(true);
    ui->mvUpImg_pushButton->setEnabled(true);
    ui->jump_Up_spinBox->setEnabled(true);
    ui->jump_Up_spinBox->setMaximum(data.size()-1-idx);
  }

  if ( idx > 0 )
  {
    this->idxViewLabel(ui->view_prv_label,idx-1);
    ui->prevImg_pushButton->setEnabled(true);
    ui->headImg_pushButton->setEnabled(true);
    ui->mvDwImg_pushButton->setEnabled(true);
    ui->jump_Dw_spinBox->setEnabled(true);
    ui->jump_Dw_spinBox->setMaximum(idx);
  }

  if ( idx>0 ) {
    for ( size_t i=idx ; i>0 ; --i ) {
      if ( data[i-1].camera!=data[idx].camera ) {
        ui->prevBnd_pushButton->setEnabled(true);
        ui->mvDwSet_pushButton->setEnabled(true);
        break;
      }
    }
  }

  for ( size_t i=idx ; i<data.size()-1 ; ++i ) {
    if ( data[i+1].camera!=data[idx].camera ) {
      ui->nextBnd_pushButton->setEnabled(true);
      ui->mvUpSet_pushButton->setEnabled(true);
      break;
    }
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

  data[idx].time -= data[idx].time-data[idx-ui->jump_Dw_spinBox->value()].time+1;

  int i = static_cast<int>(idx)-ui->jump_Dw_spinBox->value()-1;
  while ( i>=0 &&  data[i].time==data[idx].time ) {
    data[i].time -= 1;
    --i;
  }
}

// ============================================================================

void MainWindow::on_mvDwSet_pushButton_clicked()
{
  size_t i;

  for ( i=idx ; i>0 ; --i )
    if ( data[i-1].camera!=data[idx].camera )
      break;
  --i;

  std::time_t dt = data[idx].time-data[i].time-1;

  for ( size_t j=0; j<data.size(); ++j )
    if ( data[j].camera==data[idx].camera )
      data[j].time -= dt;
}

// ============================================================================

void MainWindow::on_mvUpImg_pushButton_clicked()
{
  if ( idx==data.size()-1 )
    return;

  data[idx].time += data[idx+ui->jump_Up_spinBox->value()].time-data[idx].time+1;

  size_t i = idx+ui->jump_Up_spinBox->value()+1;
  while ( i<data.size() && data[i].time==data[idx].time ) {
    data[i].time += 1;
    ++i;
  }
}

// ============================================================================

void MainWindow::on_mvUpSet_pushButton_clicked()
{
  size_t i;

  for ( i=idx+1 ; i<data.size() ; ++i )
    if ( data[i].camera!=data[idx].camera )
      break;

  std::time_t dt = data[i].time-data[idx].time+1;

  for ( size_t j=0; j<data.size(); ++j )
    if ( data[j].camera==data[idx].camera )
      data[j].time += dt;
}

// ============================================================================

void MainWindow::on_exclImg_pushButton_clicked()
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

// ============================================================================

void MainWindow::showDate()
{
  if ( data.size()<=0 )
    return;

  std::string str(std::ctime(&data[0].time));

  ui->date_lineEdit->setText(QString::fromStdString(str));
}

// ============================================================================

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

  // store new suggested directory
  workDir = dir.absolutePath();

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
    // - store information to JSON-struct
    j[fname.toStdString()]["camera"] = data[i].camera;
    j[fname.toStdString()]["time"  ] = static_cast<long>(data[i].time);
    j["camera"][std::to_string(data[i].camera)] = data[i].dir.toStdString();
    // - copy or move
    if ( ui->cp_checkBox->isChecked() ) { QFile::copy(  data[i].path,fpath); }
    else                                { QFile::rename(data[i].path,fpath); }
  }

  // store "PATH/chroto.json"
  QString fpath = QDir(ui->outPath_lineEdit->text()).filePath("chroto.json");
  std::ofstream o(fpath.toStdString());
  o << std::setw(4) << j << std::endl;
}

// ============================================================================

void MainWindow::on_clean_pushButton_clicked()
{
  for ( size_t i=0 ; i<delData.size() ; ++i ) {
    QFile::remove(delData[i].path);
    QDir dir(delData[i].dir);
    QFileInfoList dirInfo = dir.entryInfoList(QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::Hidden);
    if ( dirInfo.isEmpty() )
      dir.removeRecursively();
  }

  size_t N = delData.size();

  for ( size_t i=0 ; i<N ; ++i )
    delData.erase(delData.begin());
}

// ============================================================================

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
  for ( size_t i=0 ; i<nameSort.size() ; ++i ) {
    fileView[i]->setVisible(false);
    pathView[i]->setVisible(false);
    dirSelec[i]->setVisible(false);
    delSelec[i]->setVisible(false);
    nameSort[i]->setVisible(false);
  }

  size_t folder = 0;
  for ( size_t i=0 ; i<data.size() ; ++i )
    folder = std::max(folder,data[i].folder);
  if ( index<static_cast<int>(folder) ) {
    index = static_cast<int>(folder);
    ui->comboBox->setCurrentIndex(index);
  }

  for ( int i=0 ; i<std::min(index+1,static_cast<int>(nameSort.size())) ; ++i ) {
    fileView[i]->setVisible(true);
    pathView[i]->setVisible(true);
    dirSelec[i]->setVisible(true);
    delSelec[i]->setVisible(true);
    nameSort[i]->setVisible(true);
  }
}
