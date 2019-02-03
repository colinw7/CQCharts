#include <CQChartsAlphaEdit.h>

CQChartsAlphaEdit::
CQChartsAlphaEdit(QWidget *parent) :
 CQRealSpin(parent)
{
  setObjectName("alpha");

  setRange(0.0, 1.0);
  setValue(1.0);
}
