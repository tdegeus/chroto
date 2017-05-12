#include "imageselection.h"
#include "ui_imageselection.h"

// =================================================================================================

std::vector<size_t> linspace(size_t start_in, size_t end_in, size_t num_in)
{
  std::vector<double> linspaced;
  std::vector<size_t> output;

  double start = static_cast<double>(start_in);
  double end   = static_cast<double>(end_in);
  double num   = static_cast<double>(num_in);

  if (num == 0)
    return output;

  if (num == 1) {
    output.push_back(start_in);
    return output;
  }

  double delta = (end-start)/(num-1.);

  for (size_t i=0; i<num_in; ++i )
    linspaced.push_back(start+delta*static_cast<double>(i));

  for (size_t i=0; i<num_in; ++i )
    output.push_back(static_cast<size_t>(linspaced[i]));

  output[0       ] = start_in;
  output[num_in-1] = end_in;

  return output;
}

// =================================================================================================

ImageSelection::ImageSelection(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ImageSelection)
{
  ui->setupUi(this);

  // upon closing (using OK) -> determine index in "files" from shown files
  connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(update_index()));
}

// =================================================================================================

ImageSelection::~ImageSelection()
{
  delete ui;
}

// =================================================================================================

void ImageSelection::promptWarning(QString msg)
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

void ImageSelection::addThumbnails()
{
  // initialize conversion thumbnail-index -> index in "files"
  for ( auto &i : row2idx )
    i = -1;
  while ( row2idx.size()<files.size() )
    row2idx.push_back(-1);

  // thumbnail settings
  ui->listWidget->setViewMode  (QListWidget::IconMode);
  ui->listWidget->setIconSize  (QSize(200,200)       );
  ui->listWidget->setResizeMode(QListWidget::Adjust  );

  // select index from "files" to show as thumbnails
  nshow = 9;
  std::vector<size_t> i = linspace(0,files.size()-1,nshow);

  // add thumbnails
  for ( size_t j=0 ; j<i.size() ; j++ ) {
    row2idx[j] = i[j];
    ui->listWidget->addItem(new QListWidgetItem(QIcon(files[i[j]]),disp[i[j]]));
  }
}

// =================================================================================================

void ImageSelection::on_pushButton_clicked()
{
  // read selected thumbnail
  int irow = ui->listWidget->currentRow();

  // no thumbnail selected -> guide user
  if ( irow<0 ) {
    this->promptWarning(tr("Please select an image, right of which more images should be shown"));
    return;
  }

  // last thumbnail selected -> select prior to last thumbnail, to always proceed to the right
  if ( irow==static_cast<int>(nshow)-1 )
    --irow;

  // number of thumbnails to insert (max 3)
  size_t n = std::min(3,row2idx[irow+1]-row2idx[irow]-1);
  if ( n==0 )
    return;

  // index of "files" to add as thumbnails
  std::vector<size_t> rows = linspace(row2idx[irow],row2idx[irow+1],n+2);

  // make room in conversion from thumbnail-index -> rows in "files"
  for ( size_t i=row2idx.size()-1 ; i>static_cast<size_t>(irow)+n ; --i )
    row2idx[i] = row2idx[i-n];

  // add thumbnails, add to conversion
  for ( size_t i=1 ; i<rows.size()-1 ; ++i ) {
    row2idx[irow+i] = rows[i];
    ui->listWidget->insertItem(irow+i,new QListWidgetItem(QIcon(files[rows[i]]),disp[rows[i]]));
  }
}

// =================================================================================================

void ImageSelection::update_index()
{
  // sync class-fields (read by main window)
  this->idx          = row2idx[ui->listWidget->currentRow()];
  this->apply_to_all = ui->checkBox->isChecked();
}

// =================================================================================================
