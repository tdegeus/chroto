#include "mainwindow.h"
#include "ui_mainwindow.h"

// ============================================================================

int orientation2angle ( int orientation )
{
  if ( orientation==8 ) return -90;
  if ( orientation==6 ) return  90;
  return 0;
}

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

  return std::make_tuple(t,orientation2angle(result.Orientation));
}

// ============================================================================

std::string longestPath( const std::vector<std::string> &dirs, char separator )
{
  std::vector<std::string>::const_iterator vsi = dirs.begin();
  int maxCharactersCommon = vsi->length();

  std::string compareString = *vsi;

  for ( vsi = dirs.begin()+1 ; vsi != dirs.end() ; vsi++ ) {
    std::pair<std::string::const_iterator,std::string::const_iterator> p = std::mismatch(compareString.begin(),compareString.end(),vsi->begin());
    if ( ( p.first-compareString.begin() ) < maxCharactersCommon )
      maxCharactersCommon = p.first-compareString.begin();
  }

  std::string::size_type found = compareString.rfind(separator,maxCharactersCommon);

  return compareString.substr(0,found);
}

// ============================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // remove selected files
  connect(ui->folder_rm_pushButton,&QPushButton::clicked,[=](){this->filesRm(ui->folder_listWidget);});
  connect(ui->add_rm_pushButton   ,&QPushButton::clicked,[=](){this->filesRm(ui->add_listWidget   );});

  // listWidgets: link scroll position
  QScrollBar *I1v = ui->folder_listWidget->verticalScrollBar();
  QScrollBar *I2v = ui->add_listWidget   ->verticalScrollBar();
  connect(I1v,SIGNAL(valueChanged(int)),I2v,SLOT(setValue(int)));
  connect(I2v,SIGNAL(valueChanged(int)),I1v,SLOT(setValue(int)));

  // update image counter
  connect(ui->next_pushButton,&QPushButton::clicked,[=](){if (idx+1<data.size()) ++idx; });
  connect(ui->prev_pushButton,&QPushButton::clicked,[=](){if (idx>0            ) --idx; });

  // update file display
  connect(ui->folder_pushButton   ,SIGNAL(clicked(bool)),this,SLOT(updateFiles()));
  connect(ui->add_pushButton      ,SIGNAL(clicked(bool)),this,SLOT(updateFiles()));
  connect(ui->folder_rm_pushButton,SIGNAL(clicked(bool)),this,SLOT(updateFiles()));
  connect(ui->add_rm_pushButton   ,SIGNAL(clicked(bool)),this,SLOT(updateFiles()));
  // resort data / update file-list / display image
  connect(ui->folder_pushButton     ,&QPushButton::clicked      ,[=](){this->dataSort();    });
  connect(ui->add_pushButton        ,&QPushButton::clicked      ,[=](){this->dataSort();    });
  connect(ui->earlier_pushButton    ,&QPushButton::clicked      ,[=](){this->dataSort();    });
  connect(ui->earlier_all_pushButton,&QPushButton::clicked      ,[=](){this->dataSort();    });
  connect(ui->later_pushButton      ,&QPushButton::clicked      ,[=](){this->dataSort();    });
  connect(ui->later_all_pushButton  ,&QPushButton::clicked      ,[=](){this->dataSort();    });
  connect(ui->del_pushButton        ,&QPushButton::clicked      ,[=](){this->dataSort();    });
  connect(ui->tabWidget             ,&QTabWidget::currentChanged,[=](){this->dataSort();    });
  connect(ui->tabWidget             ,&QTabWidget::currentChanged,[=](){this->updateFiles(); });
  connect(ui->tabWidget             ,&QTabWidget::currentChanged,[=](){this->displayImage();});
  connect(ui->earlier_pushButton    ,&QPushButton::clicked      ,[=](){this->displayImage();});
  connect(ui->earlier_all_pushButton,&QPushButton::clicked      ,[=](){this->displayImage();});
  connect(ui->later_pushButton      ,&QPushButton::clicked      ,[=](){this->displayImage();});
  connect(ui->later_all_pushButton  ,&QPushButton::clicked      ,[=](){this->displayImage();});
  connect(ui->del_pushButton        ,&QPushButton::clicked      ,[=](){this->displayImage();});
  connect(ui->prev_pushButton       ,&QPushButton::clicked      ,[=](){this->displayImage();});
  connect(ui->next_pushButton       ,&QPushButton::clicked      ,[=](){this->displayImage();});
}

// ============================================================================

MainWindow::~MainWindow()
{
  delete ui;
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
  // Empty list widgets
  // ------------------

  while(ui->folder_listWidget->count()>0)
    ui->folder_listWidget->takeItem(0);
  while(ui->add_listWidget->count()>0)
    ui->add_listWidget->takeItem(0);

  if ( data.size()==0 )
    return;

  // Read the time, if it was not read before
  // ----------------------------------------

  // Sort based on time
  // ------------------

  this->dataSort();

  // Display in listWidgets
  // ----------------------

  // get common file-path from files to add
  std::vector<std::string> lst;
  std::string path;

  for ( size_t i = 0; i < data.size(); ++i ) {
    if ( data[i].add ) {
      lst.push_back(data[i].path.toStdString());
    }
  }

  if ( lst.size()>0 ){
    path = longestPath(lst,QDir::separator().toLatin1());
    QDir dir(QString::fromStdString(path));

    for ( size_t i = 0; i < data.size(); ++i ) {
      if ( data[i].add ) {
        data[i].disp = dir.relativeFilePath(data[i].path);
      }
    }
  }

  // add to listWidgets
  for ( size_t i = 0; i < data.size(); ++i ) {
    if ( !data[i].add ) {
      ui->folder_listWidget->addItem(data[i].disp);
      ui->add_listWidget   ->addItem("");
    }
    else {
      ui->folder_listWidget->addItem("");
      ui->add_listWidget   ->addItem(data[i].disp);
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
      if ( !i.include ) return false;
      if ( !j.include ) return true;
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

void MainWindow::on_folder_pushButton_clicked()
{
  QStringList filters;
  filters << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG";

  QFileDialog dialog(this);
  dialog.setFileMode (QFileDialog::Directory);
  dialog.setOption   (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  dialog.setViewMode (QFileDialog::List);

  QDir dir;
  if (dialog.exec())
    dir = dialog.directory();
  QString dirName = dir.absolutePath();

  ui->folder_lineEdit->setText(dirName);

  QFileInfoList list = dir.entryInfoList(filters,QDir::Files);
  std::time_t t;
  int rot;

  for ( int i = 0; i < list.size(); ++i ) {
    QFileInfo finfo = list.at(i);
    try         { std::tie(t,rot) = jpg2info(finfo.absoluteFilePath().toStdString()); }
    catch (...) { continue; }
    Files file;
    file.add      = false;
    file.disp     = finfo.fileName();
    file.path     = finfo.absoluteFilePath();
    file.time     = t;
    file.rotation = rot;
    data.push_back(file);
  }
}

// ============================================================================

void MainWindow::on_add_pushButton_clicked()
{
  QStringList filters;
    filters << "Image files (*.jpg *.jpeg *.JPG *.JPEG)"
            << "Any files (*)";

  QFileDialog dialog(this);
  dialog.setFileMode   (QFileDialog::ExistingFiles);
  dialog.setOption     (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory  (QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  dialog.setNameFilters(filters);
  dialog.setViewMode   (QFileDialog::List);

  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();

  std::time_t t;
  int rot;

  for ( int i = 0; i < fileNames.size(); ++i ) {
    try         { std::tie(t,rot) = jpg2info(fileNames[i].toStdString()); }
    catch (...) { continue; }
    Files file;
    file.add      = true;
    file.disp     = fileNames[i];
    file.path     = fileNames[i];
    file.time     = t;
    file.rotation = rot;
    data.push_back(file);
  }
}

// ============================================================================

void MainWindow::displayImage(void)
{
  if ( data.size()==0 )
    return;

  if ( init ) {
    idx = 0;
    init = false;
  }

  ui->view_Label->clear();
  ui->view_prev_label->clear();
  ui->view_next_label->clear();

  QPixmap p(data[idx].path);
  int w = ui->view_Label->width();
  int h = ui->view_Label->height();
  QTransform transform;
  QTransform trans = transform.rotate(orientation2angle(data[idx].orientation));
  ui->view_Label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));

  if ( idx+1 < data.size() )
  {
    QPixmap pn(data[idx+1].path);
    int w = ui->view_next_label->width();
    int h = ui->view_next_label->height();
    QTransform transform;
    QTransform trans = transform.rotate(orientation2angle(data[idx+1].orientation));
    ui->view_next_label->setPixmap(pn.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
  }

  if ( idx > 0 )
  {
    QPixmap pp(data[idx-1].path);
    int w = ui->view_prev_label->width();
    int h = ui->view_prev_label->height();
    QTransform transform;
    QTransform trans = transform.rotate(orientation2angle(data[idx-1].orientation));
    ui->view_prev_label->setPixmap(pp.scaled(w,h,Qt::KeepAspectRatio).transformed(trans));
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
    if ( i!=idx && data[i].add==data[idx].add )
      data[i].time -= t0-data[idx].time;
}

// ============================================================================

void MainWindow::on_later_pushButton_clicked()
{
  if ( idx==data.size()-1 )
    return;
  if ( data[idx+1].include==false )
    return;

  data[idx].time += data[idx+1].time-data[idx].time+1;

  size_t i = idx+2;
  while ( i<data.size() && data[i].include==true && data[i].time==data[idx].time ) {
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
    if ( i!=idx && data[i].add==data[idx].add )
      data[i].time += data[idx].time-t0;
}

// ============================================================================

void MainWindow::on_del_pushButton_clicked()
{
  data[idx].include = false;
  --idx;
}

// ============================================================================
