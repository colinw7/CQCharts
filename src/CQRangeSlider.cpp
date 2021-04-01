#include <CQRangeSlider.h>

#include <QPainter>
#include <QMouseEvent>

CQRangeSlider::
CQRangeSlider(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("rangeSlider");

  textFont_ = font();
}

//---

void
CQRangeSlider::
setLinearGradient(const QLinearGradient &lg)
{
  lg_    = lg;
  lgSet_ = true;

  update();
}

void
CQRangeSlider::
clearLinearGradient()
{
  lgSet_ = false;
}

//---

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
  painter->setBrush(lgSet_ ? QBrush(lg_) : QBrush(bg2));

  painter->drawRoundedRect(QRect(xs1_, yt, xs2_ - xs1_ + 1, yb - yt + 1), 3, 3);

  auto xs3 = valueToPixel(getSliderMin());
  auto xs4 = valueToPixel(getSliderMax());

  if (lgSet_) {
    bg3.setAlphaF(0.3);

    painter->setPen  (Qt::NoPen);
    painter->setBrush(bg3);

    painter->drawRoundedRect(QRect(xs1_, yt, xs3  - xs1_ + 1, yb - yt + 1), 1, 1);
    painter->drawRoundedRect(QRect(xs4 , yt, xs2_ - xs4  + 1, yb - yt + 1), 1, 1);

    painter->setPen  (fg1);
    painter->setBrush(Qt::NoBrush);

    painter->drawRoundedRect(QRect(xs3, yt, xs4 - xs3 + 1, yb - yt + 1), 1, 1);
  }
  else {
    painter->setPen  (fg1);
    painter->setBrush(bg3);

    painter->drawRoundedRect(QRect(xs3, yt, xs4 - xs3 + 1, yb - yt + 1), 1, 1);
  }

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

//------

void
CQRangeSlider::
mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton) {
    pressed_ = true;

    saveRange();

    pixelToSliderValue(e->x(), pressInd_);
  }
}

void
CQRangeSlider::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_)
    pixelSetSliderValue(e->x(), pressInd_);
}

void
CQRangeSlider::
mouseReleaseEvent(QMouseEvent *e)
{
  if (pressed_) {
    pressed_ = false;

    pixelSetSliderValue(e->x(), pressInd_, /*force*/true);
  }
}

void
CQRangeSlider::
keyPressEvent(QKeyEvent *e)
{
  if      (e->key() == Qt::Key_Left ) deltaSliderMin(-1);
  else if (e->key() == Qt::Key_Right) deltaSliderMin( 1);
  else if (e->key() == Qt::Key_Down ) deltaSliderMax(-1);
  else if (e->key() == Qt::Key_Up   ) deltaSliderMax( 1);
}

//------

QColor
CQRangeSlider::
colorAt(double x) const
{
  assert(lgSet_);

  double sx1 = 0.0, sx2 = 0.0;
  QColor sc1, sc2;

  for (const auto &s : lg_.stops()) {
    sx2 = s.first;
    sc2 = s.second;

    if (x >= sx1 && x <= sx2)
      return interpColor(sc1, sc2, (x - sx1)/(sx2 - sx1));

    sx1 = sx2;
    sc1 = sc2;
  }

  return QColor();
}

QColor
CQRangeSlider::
interpColor(const QColor &c1, const QColor &c2, double f)
{
  auto interpValue = [](double v1, double v2, double f) {
    return v1*(1 - f) + v2*f;
  };

  qreal r1, g1, b1, a1;
  qreal r2, g2, b2, a2;

  c1.getRgbF(&r1, &g1, &b1, &a1);
  c2.getRgbF(&r2, &g2, &b2, &a2);

  return QColor::fromRgbF(interpValue(r1, r2, f),
                          interpValue(g1, g2, f),
                          interpValue(b1, b2, f));
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

QColor
CQRangeSlider::
bwColor(const QColor &c)
{
  int g = qGray(c.red(), c.green(), c.blue());

  return (g > 128 ? QColor(0, 0, 0) : QColor(255, 255, 255));
}
