#include "mainwindow.h"
#include "ui_mainwindow.h"

// ============================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);


  // Read the JPEG file into a buffer
   FILE *fp = fopen("/Users/tdegeus/Dropbox/bruiloft/IMG_8393.JPG","rb");
   if (!fp) {
     printf("Can't open file.\n");
     return;
   }
   fseek(fp, 0, SEEK_END);
   unsigned long fsize = ftell(fp);
   rewind(fp);
   unsigned char *buf = new unsigned char[fsize];
   if (fread(buf, 1, fsize, fp) != fsize) {
     printf("Can't read file.\n");
     delete[] buf;
     return;
   }
   fclose(fp);

   // Parse EXIF
   easyexif::EXIFInfo result;
   int code = result.parseFrom(buf, fsize);
   delete[] buf;

   printf("Image orientation    : %d\n", result.Orientation);
   printf("Image date/time      : %s\n", result.DateTime.c_str());
//  QImageReader img;
//  img.setFileName();
//  img.transformation()
}

// ============================================================================

MainWindow::~MainWindow()
{
  delete ui;
}

// ============================================================================

void MainWindow::updateFiles ( void )
{

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
  QString dirName = dir.absoluteFilePath(dir.dirName());

  ui->folder_lineEdit->setText(dirName);

  QFileInfoList list = dir.entryInfoList(filters,QDir::Files);

  for ( int i = 0; i < list.size(); ++i ) {
    QFileInfo fileInfo = list.at(i);
    ui->folder_listWidget->addItem(fileInfo.fileName());
//    data.push_back(dir.absoluteFilePath(fileInfo.fileName()),1);
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

  for ( int i = 0; i < fileNames.size(); ++i ) {
    ui->add_listWidget->addItem(fileNames[i]);
  }

}
