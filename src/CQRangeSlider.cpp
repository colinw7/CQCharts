#include <CQRangeSlider.h>

CQRangeSlider::
CQRangeSlider(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("rangeSlider");
}

QColor
CQRangeSlider::
blendColors(const QColor &c1, const QColor &c2, double f)
{
  auto iclamp = [](int val, int low, int high) {
    if (val < low ) return low;
    if (val > high) return high;
    return val;
  };

  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}
