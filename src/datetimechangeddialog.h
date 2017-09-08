/* =================================================================================================

(c - GPLv3) T.W.J. de Geus | tom@geus.me | www.geus.me | github.com/tdegeus/chroto

================================================================================================= */

#ifndef DATETIMECHANGEDDIALOG_H
#define DATETIMECHANGEDDIALOG_H

#include <QDialog>

// =================================================================================================

struct CalResponse { enum Value  { Cancel = 0 , Photo = 1 , Camera = 2 , Folder = 3 , All = 4 }; };

// =================================================================================================

namespace Ui {
class DateTimeChangedDialog;
}

// =================================================================================================

class DateTimeChangedDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DateTimeChangedDialog(QWidget *parent = 0);
  ~DateTimeChangedDialog();

  size_t  m_response=CalResponse::Cancel;

  size_t getResponse();
  void setTimeNew(const QString &name);
  void setTimeOld(const QString &name);

private:
  Ui::DateTimeChangedDialog *ui;
};

// =================================================================================================

#endif // DATETIMECHANGEDDIALOG_H
