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

  // convert orientation to ratation
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

  listWidgets.push_back(ui->cam0_listWidget);
  listWidgets.push_back(ui->cam1_listWidget);
  listWidgets.push_back(ui->cam2_listWidget);
  listWidgets.push_back(ui->cam3_listWidget);
  listWidgets.push_back(ui->cam4_listWidget);
  lineEdits.push_back(ui->cam0_lineEdit);
  lineEdits.push_back(ui->cam1_lineEdit);
  lineEdits.push_back(ui->cam2_lineEdit);
  lineEdits.push_back(ui->cam3_lineEdit);
  lineEdits.push_back(ui->cam4_lineEdit);
  add_pushButtons.push_back(ui->cam0_pushButton);
  add_pushButtons.push_back(ui->cam1_pushButton);
  add_pushButtons.push_back(ui->cam2_pushButton);
  add_pushButtons.push_back(ui->cam3_pushButton);
  add_pushButtons.push_back(ui->cam4_pushButton);
  rmv_pushButtons.push_back(ui->cam0_rm_pushButton);
  rmv_pushButtons.push_back(ui->cam1_rm_pushButton);
  rmv_pushButtons.push_back(ui->cam2_rm_pushButton);
  rmv_pushButtons.push_back(ui->cam3_rm_pushButton);
  rmv_pushButtons.push_back(ui->cam4_rm_pushButton);


  // select folder / remove selected files / sort files / view files
  for ( size_t i=0 ; i<add_pushButtons.size() ; ++i ) {
    connect(add_pushButtons[i],&QPushButton::clicked,[=](){this->selectFolder(i);});
    connect(rmv_pushButtons[i],&QPushButton::clicked,[=](){this->filesRm(listWidgets[i]);});
    connect(add_pushButtons[i],SIGNAL(clicked(bool)),this,SLOT(dataSort()));
    connect(rmv_pushButtons[i],SIGNAL(clicked(bool)),this,SLOT(dataSort()));
    connect(add_pushButtons[i],SIGNAL(clicked(bool)),this,SLOT(updateFiles()));
    connect(rmv_pushButtons[i],SIGNAL(clicked(bool)),this,SLOT(updateFiles()));
  }
  // link scroll position listWidgets
  for ( size_t i=0 ; i<listWidgets.size() ; ++i )
    for ( size_t j=0 ; j<listWidgets.size() ; ++j )
      if ( i!=j )
        connect(listWidgets[i]->verticalScrollBar(),SIGNAL(valueChanged(int)),listWidgets[j]->verticalScrollBar(),SLOT(setValue(int)));

  // update image counter
  connect(ui->first_pushButton   ,&QPushButton::clicked,[=](){idx=0; });
  connect(ui->last_pushButton    ,&QPushButton::clicked,[=](){idx=data.size()-1; while ( data[idx-1].rm ) { --idx; if ( idx==0 ) break; } });
  connect(ui->next_pushButton    ,&QPushButton::clicked,[=](){if (idx+1<data.size()) ++idx; });
  connect(ui->prev_pushButton    ,&QPushButton::clicked,[=](){if (idx>0            ) --idx; });
  connect(ui->next_bnd_pushButton,&QPushButton::clicked,[=](){while ( data[idx+1].camera==data[idx].camera && !data[idx+1].rm ){++idx; if ( idx==data.size()-1) break; } });
  connect(ui->prev_bnd_pushButton,&QPushButton::clicked,[=](){while ( data[idx-1].camera==data[idx].camera && !data[idx-1].rm ){--idx; if ( idx==0            ) break; } });

  // re-sort data / update file-list / display image
  connect(ui->earlier_pushButton    ,SIGNAL(clicked(bool)),this,SLOT(dataSort()    ));
  connect(ui->earlier_all_pushButton,SIGNAL(clicked(bool)),this,SLOT(dataSort()    ));
  connect(ui->later_pushButton      ,SIGNAL(clicked(bool)),this,SLOT(dataSort()    ));
  connect(ui->later_all_pushButton  ,SIGNAL(clicked(bool)),this,SLOT(dataSort()    ));
  connect(ui->del_pushButton        ,SIGNAL(clicked(bool)),this,SLOT(dataSort()    ));
  connect(ui->earlier_pushButton    ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->earlier_all_pushButton,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->later_pushButton      ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->later_all_pushButton  ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->del_pushButton        ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->prev_pushButton       ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->next_pushButton       ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->first_pushButton      ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->last_pushButton       ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->next_bnd_pushButton   ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  connect(ui->prev_bnd_pushButton   ,SIGNAL(clicked(bool)),this,SLOT(displayImage()));
  // re-sort data / update file-list / display image
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->dataSort();    });
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->updateFiles(); });
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){this->displayImage();});

  connect(ui->output_pushButton,SIGNAL(clicked(bool)),this,SLOT(on_output_lineEdit_editingFinished()));
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

void MainWindow::updateFiles ( void )
{
  // empty listWidgets
  for ( size_t l=0 ; l<listWidgets.size() ; ++l )
    while(listWidgets[l]->count()>0)
      listWidgets[l]->takeItem(0);

  // empty list -> exit
  if ( data.size()==0 )
    return;

  // fill listWidgets
  for ( size_t i=0 ; i<data.size() ; ++i ) {
    for ( size_t l=0 ; l<listWidgets.size() ; ++l ) {
      if ( data[i].camera==l ) {
        listWidgets[l]->addItem(data[i].disp);
        lineEdits  [l]->setText(data[i].dir);
      }
      else {
        listWidgets[l]->addItem("");
      }
    }
  }
}

// ============================================================================

void MainWindow::dataSort(void)
{
  for ( size_t i = 0; i < data.size(); ++i ) {
    data[i].index = i;
  }

  std::sort(data.begin(),data.end(),
    [](Files i,Files j){
      if ( i.rm && !j.rm  ) return false;
      if ( j.rm && !i.rm  ) return true;
      if ( i.time==j.time ) return i.index<j.index;
      return i.time<j.time;
  });

  for ( size_t i = 0; i < data.size(); ++i ) {
    if ( data[i].index==idx ) {
      idx = i;
      return;
    }
  }

}

// ============================================================================

void MainWindow::filesRm(QListWidget *list)
{
  QList<QListWidgetItem*> items = list->selectedItems();

  std::vector<int> rm(data.size());

  for ( size_t i = 0; i < rm.size(); ++i )
    rm[i] = 0;

  foreach (QListWidgetItem *item, items) {
    rm[list->row(item)] = 1;
  }

  size_t j = 0;
  for ( size_t i = 0; i < rm.size(); ++i ) {
    if ( rm[i] ) {
      data.erase(data.begin()+i-j);
      ++j;
    }
  }
}

// ============================================================================

void MainWindow::selectFolder(size_t camera)
{
  QStringList filter_jpeg;
  filter_jpeg << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG";

  QStringList filter_json;
  filter_json << "chroto.json";

  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();
  QString dirName = dir.absolutePath();

  QFileInfoList lst_json = dir.entryInfoList(filter_json,QDir::Files);
  QFileInfoList lst_jpeg = dir.entryInfoList(filter_jpeg,QDir::Files);

  json j;
  if ( lst_json.size()==1 ) {
    QFileInfo finfo = lst_json.at(0);
    std::ifstream inp(finfo.absoluteFilePath().toStdString());
    inp >> j;
  }

  std::time_t t;
  int rot;

  for ( int i = 0; i < lst_jpeg.size(); ++i ) {
    QFileInfo finfo = lst_jpeg.at(i);
    try         { std::tie(t,rot) = jpg2info(finfo.absoluteFilePath().toStdString()); }
    catch (...) { continue; }
    Files file;
    file.camera    = camera;
    file.disp      = finfo.fileName();
    file.path      = finfo.absoluteFilePath();
    file.dir       = finfo.absolutePath();
    file.time      = t;
    file.rotation  = rot;
    if ( lst_json.size()==1 )
      file.time = static_cast<std::time_t>(j[finfo.fileName().toStdString()]);
    data.push_back(file);
  }
}

// ============================================================================

void MainWindow::view ( QLabel *lab, size_t i )
{
  QPixmap p(data[i].path);
  int w = lab->width();
  int h = lab->height();
  QTransform transform;
  QTransform trans = transform.rotate(data[i].rotation);
  lab->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
}

// ============================================================================

void MainWindow::displayImage(void)
{
  ui->prev_pushButton       ->setEnabled(false);
  ui->next_pushButton       ->setEnabled(false);
  ui->first_pushButton      ->setEnabled(false);
  ui->last_pushButton       ->setEnabled(false);
  ui->earlier_pushButton    ->setEnabled(false);
  ui->earlier_all_pushButton->setEnabled(false);
  ui->later_pushButton      ->setEnabled(false);
  ui->later_all_pushButton  ->setEnabled(false);
  ui->del_pushButton        ->setEnabled(false);
  ui->next_bnd_pushButton   ->setEnabled(false);
  ui->prev_bnd_pushButton   ->setEnabled(false);

  ui->view_Label     ->clear();
  ui->view_prev_label->clear();
  ui->view_next_label->clear();

  if ( data.size()==0 )
    return;

  if ( init ) {
    idx  = 0;
    init = false;
  }

  ui->del_pushButton->setEnabled(true);

  for ( size_t i=1 ; i<data.size() ; ++i ) {
    if ( data[i-1].camera!=data[i].camera ) {
      ui->next_bnd_pushButton->setEnabled(true);
      ui->prev_bnd_pushButton->setEnabled(true);
      break;
    }
  }

  this->view(ui->view_Label,idx);

  if ( idx+1 < data.size() )
  {
    this->view(ui->view_next_label,idx+1);
    ui->next_pushButton ->setEnabled(true);
    ui->last_pushButton ->setEnabled(true);
    ui->later_pushButton->setEnabled(true);
    if ( data[idx].camera!=data[idx+1].camera )
      ui->later_all_pushButton  ->setEnabled(true);
  }

  if ( idx > 0 )
  {
    this->view(ui->view_prev_label,idx-1);
    ui->prev_pushButton   ->setEnabled(true);
    ui->first_pushButton  ->setEnabled(true);
    ui->earlier_pushButton->setEnabled(true);
    if ( data[idx].camera!=data[idx-1].camera )
      ui->earlier_all_pushButton->setEnabled(true);
  }

}

// ============================================================================

void MainWindow::on_earlier_pushButton_clicked()
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

void MainWindow::on_earlier_all_pushButton_clicked()
{
  size_t t0 = data[idx].time;
  this->on_earlier_pushButton_clicked();

  for ( size_t i = 0; i < data.size(); ++i )
    if ( i!=idx && data[i].camera==data[idx].camera )
      data[i].time -= t0-data[idx].time;
}

// ============================================================================

void MainWindow::on_later_pushButton_clicked()
{
  if ( idx==data.size()-1 )
    return;
  if ( data[idx+1].rm )
    return;

  data[idx].time += data[idx+1].time-data[idx].time+1;

  size_t i = idx+2;
  while ( i<data.size() && !data[i].rm && data[i].time==data[idx].time ) {
    data[i].time += 1;
    ++i;
  }
}

// ============================================================================

void MainWindow::on_later_all_pushButton_clicked()
{
  size_t t0 = data[idx].time;
  this->on_later_pushButton_clicked();

  for ( size_t i = 0; i < data.size(); ++i )
    if ( i!=idx && data[i].camera==data[idx].camera )
      data[i].time += data[idx].time-t0;
}

// ============================================================================

void MainWindow::on_del_pushButton_clicked()
{
  data[idx].rm = true;
  --idx;
}

// ============================================================================

void MainWindow::on_output_pushButton_clicked()
{
  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();

  QString dirName = dir.absolutePath();

  ui->output_lineEdit->setText(dirName);
}

// ============================================================================

void MainWindow::on_output_lineEdit_editingFinished()
{
  if ( ui->output_lineEdit->text().length()>0 )
    ui->write_pushButton->setEnabled(true);
  else
    ui->write_pushButton->setEnabled(false);
}

// ============================================================================

void MainWindow::on_write_pushButton_clicked()
{
  int N = 0;
  QString tmp;
  for ( size_t i = 0; i < data.size(); ++i ) {
    tmp = QString::number(i);
    N   = std::max(N,tmp.length());
  }

  json j;

  for ( int i=0 ; i<static_cast<int>(data.size()) ; ++i ) {
    QString name = ui->name_lineEdit->text() + QString("-") + QString("%1.jpg").arg(i,N,10,QChar('0'));

    j[name.toStdString()] = static_cast<long>(data[i].time);

    QString out = QDir(ui->output_lineEdit->text()).filePath(name);

    if ( ui->cp_checkBox->isChecked() )
      QFile::copy(data[i].path,out);
    else
      QFile::rename(data[i].path,out);

  }

  QString out_json = QDir(ui->output_lineEdit->text()).filePath("chroto.json");
  std::ofstream o(out_json.toStdString());
  o << std::setw(4) << j << std::endl;

}

