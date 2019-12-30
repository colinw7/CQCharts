#include <CQChartsAlphaEdit.h>

CQChartsAlphaEdit::
CQChartsAlphaEdit(QWidget *parent) :
 CQRealSpin(parent)
{
  setObjectName("alpha");

  setToolTip("Color Alpha (0.0 -> 1.0)");

  setRange(0.0, 1.0);
  setValue(1.0);
}
