#include <CQDoubleRangeSlider.h>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFontMetrics>
#include <cmath>

CQDoubleRangeSlider::
CQDoubleRangeSlider(QWidget *parent) :
 CQRangeSlider(parent)
{
  setObjectName("doubleRangeSlider");

  setFocusPolicy(Qt::ClickFocus);

  updateTip();
}

//---

void
CQDoubleRangeSlider::
setRangeMin(double r)
{
  if (r != range_.min) {
    if (r > range_.max)
      return;

    range_.min = r;

    updateTip();

    update();

    emit rangeChanged(rangeMin(), rangeMax());
  }
}

void
CQDoubleRangeSlider::
setRangeMax(double r)
{
  if (r != range_.max) {
    if (r < range_.min)
      return;

    range_.max = r;

    updateTip();

    update();

    emit rangeChanged(rangeMin(), rangeMax());
  }
}

void
CQDoubleRangeSlider::
setRangeMinMax(double min, double max)
{
  if (min != range_.min || max != range_.max) {
    range_.min = min;
    range_.max = max;

    if (range_.min > range_.max)
      std::swap(range_.min, range_.max);

    updateTip();

    update();

    emit rangeChanged(rangeMin(), rangeMax());
  }
}

//---

void
CQDoubleRangeSlider::
setSliderMin(double r, bool force)
{
  if (force || r != slider_.min) {
    if (r > slider_.max)
      return;

    slider_.min = r;

    updateTip();

    update();

    if (pressed_)
      emit sliderRangeChanging(sliderMin(), sliderMax());
    else
      emit sliderRangeChanged(sliderMin(), sliderMax());
  }
}

void
CQDoubleRangeSlider::
setSliderMax(double r, bool force)
{
  if (force || r != slider_.max) {
    if (r < slider_.min)
      return;

    slider_.max = r;

    updateTip();

    update();

    if (pressed_)
      emit sliderRangeChanging(sliderMin(), sliderMax());
    else
      emit sliderRangeChanged(sliderMin(), sliderMax());
  }
}

void
CQDoubleRangeSlider::
setSliderMinMax(double min, double max, bool force)
{
  if (force || min != slider_.min || max != slider_.max) {
    slider_.min = min;
    slider_.max = max;

    if (slider_.min > slider_.max)
      std::swap(slider_.min, slider_.max);

    updateTip();

    update();

    if (pressed_)
      emit sliderRangeChanging(sliderMin(), sliderMax());
    else
      emit sliderRangeChanged(sliderMin(), sliderMax());
  }
}

//---

void
CQDoubleRangeSlider::
setLinearGradient(const QLinearGradient &lg)
{
  lg_    = lg;
  lgSet_ = true;
}

void
CQDoubleRangeSlider::
clearLinearGradient()
{
  lgSet_ = false;
}

//---

void
CQDoubleRangeSlider::
updateTip()
{
  setToolTip(QString("%1 - %2\n(Range %3 - %4)").
    arg(sliderMin()).arg(sliderMax()).arg(rangeMin()).arg(rangeMax()));
}

//---

void
CQDoubleRangeSlider::
mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton) {
    pressed_ = true;
    save_    = slider_;

    pixelToSliderValue(e->x(), pressInd_);
  }
}

void
CQDoubleRangeSlider::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_)
    pixelSetSliderValue(e->x(), pressInd_);
}

void
CQDoubleRangeSlider::
mouseReleaseEvent(QMouseEvent *e)
{
  if (pressed_) {
    pressed_ = false;

    pixelSetSliderValue(e->x(), pressInd_, /*force*/true);
  }
}

void
CQDoubleRangeSlider::
keyPressEvent(QKeyEvent *e)
{
  if      (e->key() == Qt::Key_Left)
    setSliderMin(clampValue(deltaValue(sliderMin(), -1)), /*force*/true);
  else if (e->key() == Qt::Key_Right)
    setSliderMin(clampValue(deltaValue(sliderMin(),  1)), /*force*/true);
  else if (e->key() == Qt::Key_Down)
    setSliderMax(clampValue(deltaValue(sliderMax(), -1)), /*force*/true);
  else if (e->key() == Qt::Key_Up)
    setSliderMax(clampValue(deltaValue(sliderMax(),  1)), /*force*/true);
}

double
CQDoubleRangeSlider::
clampValue(double r) const
{
  return std::min(std::max(r, rangeMin()), rangeMax());
}

double
CQDoubleRangeSlider::
deltaValue(double r, int inc) const
{
  if (sliderDelta() < 1.0) {
    int n = -std::round(std::log10(sliderDelta()));

    double scale = std::pow(10, n);

    int v = std::round(r*scale) + inc;

    return v/scale;
  }
  else {
    return r + inc*sliderDelta();
  }
}

void
CQDoubleRangeSlider::
pixelToSliderValue(int px, int &ind, bool force)
{
  double x = pixelToValue(px);

  double dx1 = std::abs(sliderMin() - x);
  double dx2 = std::abs(sliderMax() - x);

  ind = (dx1 < dx2 ? 0 : 1);

  if (ind == 0)
    setSliderMin(clampValue(x), force);
  else
    setSliderMax(clampValue(x), force);
}

void
CQDoubleRangeSlider::
pixelSetSliderValue(int px, int ind, bool force)
{
  double x = pixelToValue(px);

  if (ind == 0)
    setSliderMin(clampValue(x), force);
  else
    setSliderMax(clampValue(x), force);
}

void
CQDoubleRangeSlider::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QFontMetricsF fm(font());

  double ym = height()/2.0;

  int xl = 2;
  int xr = width() - 2;
  int yt = ym - fm.height()/2.0 - 1;
  int yb = ym + fm.height()/2.0 + 1;

  //---

  if (showRangeLabels()) {
    QString minStr = realToString(rangeMin());
    QString maxStr = realToString(rangeMax());

    int twMin = fm.width(minStr);
    int twMax = fm.width(maxStr);

    painter.setPen  (palette().color(QPalette::WindowText));
    painter.setBrush(Qt::NoBrush);

    painter.drawText(xl        , yt + fm.ascent() + 1, minStr);
    painter.drawText(xr - twMax, yt + fm.ascent() + 1, maxStr);

    xs1_ = xl + twMin + 2;
    xs2_ = xr - twMax - 2;
  }
  else {
    xs1_ = xl;
    xs2_ = xr;
  }

  //---

  QColor bg0 = palette().color(QPalette::Background);
  QColor bg1 = palette().color(QPalette::Highlight);
  QColor fg0 = palette().color(QPalette::Text);
  QColor fg1 = blendColors(bg0, fg0, 0.3);

  QColor bg2 = blendColors(bg1, bg0, 0.5);
  QColor bg3 = blendColors(bg1, fg0, 0.8);

  painter.setPen  (Qt::NoPen);
  painter.setBrush(lgSet_ ? QBrush(lg_) : QBrush(bg2));

  painter.drawRoundedRect(QRect(xs1_, yt, xs2_ - xs1_ + 1, yb - yt + 1), 3, 3);

  int xs3 = valueToPixel(sliderMin());
  int xs4 = valueToPixel(sliderMax());

  if (lgSet_) {
    bg3.setAlphaF(0.3);

    painter.setPen  (Qt::NoPen);
    painter.setBrush(bg3);

    painter.drawRoundedRect(QRect(xs1_, yt, xs3  - xs1_ + 1, yb - yt + 1), 1, 1);
    painter.drawRoundedRect(QRect(xs4 , yt, xs2_ - xs4  + 1, yb - yt + 1), 1, 1);

    painter.setPen  (fg1);
    painter.setBrush(Qt::NoBrush);

    painter.drawRoundedRect(QRect(xs3, yt, xs4 - xs3 + 1, yb - yt + 1), 1, 1);
  }
  else {
    painter.setPen  (fg1);
    painter.setBrush(bg3);

    painter.drawRoundedRect(QRect(xs3, yt, xs4 - xs3 + 1, yb - yt + 1), 1, 1);
  }

  painter.setPen  (fg1);
  painter.setBrush(palette().color(QPalette::Button));

  //---

  double bs = fm.height()/2.0;

  painter.drawEllipse(QRectF(xs3 - bs/2, ym - bs/2, bs, bs));
  painter.drawEllipse(QRectF(xs4 - bs/2, ym - bs/2, bs, bs));

  //---

  if (showSliderLabels()) {
    QString sminStr = realToString(sliderMin());
    QString smaxStr = realToString(sliderMax());

    int twsMin = fm.width(sminStr);
    int twsMax = fm.width(smaxStr);

    painter.setPen  (palette().color(QPalette::HighlightedText));
    painter.setBrush(Qt::NoBrush);

    int xm = (xs1_ + xs2_)/2;

    int xs5 = xs3 - twsMin - bs;

    if (xs5 < xs1_) {
      xs5 = xs3 + bs;

      if (xs5 + twsMin > xm)
        xs5 = -1;
    }

    if (xs5 >= 0)
      painter.drawText(xs5, yt + fm.ascent() + 1, sminStr);

    int xs6 = xs4 + bs;

    if (xs6 + twsMax > xs2_) {
      xs6 = xs4 - twsMax - bs;

      if (xs6 < xm)
        xs6 = -1;
    }

    if (xs6 >= 0)
      painter.drawText(xs6, yt + fm.ascent() + 1, smaxStr);
  }
}

QString
CQDoubleRangeSlider::
realToString(double r) const
{
  return QString("%1").arg(r, 0, 'g', decimalPlaces());
}

double
CQDoubleRangeSlider::
valueToPixel(double x) const
{
  return (xs2_ - xs1_)*(x - rangeMin())/(rangeMax() - rangeMin()) + xs1_;
}

double
CQDoubleRangeSlider::
pixelToValue(double px) const
{
  return (px - xs1_)*(rangeMax() - rangeMin())/(xs2_ - xs1_) + rangeMin();
}

QSize
CQDoubleRangeSlider::
sizeHint() const
{
  QFontMetrics fm(font());

  int b = 4;

  int w = 2*b;

  int labelWidth = fm.width("XXX.XXX");

  if (showRangeLabels() || showSliderLabels()) {
    if (showRangeLabels())
      w += 2*labelWidth;

    if (showSliderLabels())
      w += 2*labelWidth;
  }
  else {
    w += 100;
  }

  return QSize(w, fm.height() + 2*b);
}
