/* =================================================================================================

(c - GPLv3) T.W.J. de Geus | tom@geus.me | www.geus.me | github.com/tdegeus/chroto

================================================================================================= */

#include "datetimechangeddialog.h"
#include "ui_datetimechangeddialog.h"

// =================================================================================================

DateTimeChangedDialog::DateTimeChangedDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DateTimeChangedDialog)
{
  ui->setupUi(this);

  connect( ui->pushButton_cancel, &QPushButton::clicked, [=](){ m_response=CalResponse::Cancel; } );
  connect( ui->pushButton_photo , &QPushButton::clicked, [=](){ m_response=CalResponse::Photo;  } );
  connect( ui->pushButton_cam   , &QPushButton::clicked, [=](){ m_response=CalResponse::Camera; } );
  connect( ui->pushButton_fol   , &QPushButton::clicked, [=](){ m_response=CalResponse::Folder; } );
  connect( ui->pushButton_all   , &QPushButton::clicked, [=](){ m_response=CalResponse::All;    } );

  connect( ui->pushButton_cancel, SIGNAL(clicked(bool)), this, SLOT(close()) );
  connect( ui->pushButton_photo , SIGNAL(clicked(bool)), this, SLOT(close()) );
  connect( ui->pushButton_cam   , SIGNAL(clicked(bool)), this, SLOT(close()) );
  connect( ui->pushButton_fol   , SIGNAL(clicked(bool)), this, SLOT(close()) );
  connect( ui->pushButton_all   , SIGNAL(clicked(bool)), this, SLOT(close()) );
}

// =================================================================================================

DateTimeChangedDialog::~DateTimeChangedDialog()
{
  delete ui;
}

// =================================================================================================

size_t DateTimeChangedDialog::getResponse()
{
  return m_response;
}

// =================================================================================================

void DateTimeChangedDialog::setTimeNew(const QString &name)
{
  ui->label_t->setText(name);
}

// =================================================================================================

void DateTimeChangedDialog::setTimeOld(const QString &name)
{
  ui->label_t0->setText(name);
}

// =================================================================================================
