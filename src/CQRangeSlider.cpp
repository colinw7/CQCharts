#include <CQRangeSlider.h>

#include <QPainter>

CQRangeSlider::
CQRangeSlider(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("rangeSlider");

  textFont_ = font();
}

void
CQRangeSlider::
drawSlider(QPainter *painter)
{
  QFontMetricsF fm(font());

  double ym = height()/2.0;

  int yb = ym + fm.height()/2.0 + 1;
  int yt = ym - fm.height()/2.0 - 1;

  auto bg0 = palette().color(QPalette::Background);
  auto bg1 = palette().color(QPalette::Highlight);
  auto fg0 = palette().color(QPalette::Text);
  auto fg1 = blendColors(bg0, fg0, 0.3);

  auto bg2 = blendColors(bg1, bg0, 0.5);
  auto bg3 = blendColors(bg1, fg0, 0.8);

  painter->setPen  (Qt::NoPen);
  painter->setBrush(bg2);

  painter->drawRoundedRect(QRect(xs1_, yt, xs2_ - xs1_ + 1, yb - yt + 1), 3, 3);

  auto xs3 = valueToPixel(getSliderMin());
  auto xs4 = valueToPixel(getSliderMax());

  painter->setPen  (fg1);
  painter->setBrush(bg3);

  painter->drawRoundedRect(QRect(xs3, yt, xs4 - xs3 + 1, yb - yt + 1), 1, 1);

  painter->setPen  (fg1);
  painter->setBrush(palette().color(QPalette::Button));

  //---

  double bs = fm.height()/3.0;

  if (sliderPos() == SliderPos::CENTER) {
    painter->drawEllipse(QRectF(xs3 - bs/2, ym - bs/2, bs, bs));
    painter->drawEllipse(QRectF(xs4 - bs/2, ym - bs/2, bs, bs));
  }
  else {
    if (sliderPos() == SliderPos::TOP || sliderPos() == SliderPos::BOTH) {
      painter->drawEllipse(QRectF(xs3 - bs/2, yt - bs/2, bs, bs));
      painter->drawEllipse(QRectF(xs4 - bs/2, yt - bs/2, bs, bs));
    }

    if (sliderPos() == SliderPos::BOTTOM || sliderPos() == SliderPos::BOTH) {
      painter->drawEllipse(QRectF(xs3 - bs/2, yb - bs/2, bs, bs));
      painter->drawEllipse(QRectF(xs4 - bs/2, yb - bs/2, bs, bs));
    }
  }
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
