#include <CQChartsLineEdit.h>

CQChartsLineEdit::
CQChartsLineEdit(QWidget *parent) :
 QLineEdit(parent)
{
#if QT_VERSION >= 0x050000
  setClearButtonEnabled(true);
#endif
}
