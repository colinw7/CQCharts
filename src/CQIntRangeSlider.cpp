#include <CQIntRangeSlider.h>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFontMetrics>
#include <cmath>

CQIntRangeSlider::
CQIntRangeSlider(QWidget *parent) :
 CQRangeSlider(parent)
{
  setObjectName("intRangeSlider");

  setFocusPolicy(Qt::ClickFocus);

  updateTip();
}

//---

void
CQIntRangeSlider::
setRangeMin(int i)
{
  if (i != range_.min) {
    if (i > range_.max)
      return;

    range_.min = i;

    updateTip();

    update();

    emit rangeChanged(rangeMin(), rangeMax());
  }
}

void
CQIntRangeSlider::
setRangeMax(int i)
{
  if (i != range_.max) {
    if (i < range_.min)
      return;

    range_.max = i;

    updateTip();

    update();

    emit rangeChanged(rangeMin(), rangeMax());
  }
}

void
CQIntRangeSlider::
setRangeMinMax(int min, int max)
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
CQIntRangeSlider::
setSliderMin(int i, bool force)
{
  if (force || i != slider_.min) {
    if (i > slider_.max)
      return;

    slider_.min = i;

    updateTip();

    update();

    if (pressed_)
      emit sliderRangeChanging(sliderMin(), sliderMax());
    else
      emit sliderRangeChanged(sliderMin(), sliderMax());
  }
}

void
CQIntRangeSlider::
setSliderMax(int i, bool force)
{
  if (force || i != slider_.max) {
    if (i < slider_.min)
      return;

    slider_.max = i;

    updateTip();

    update();

    if (pressed_)
      emit sliderRangeChanging(sliderMin(), sliderMax());
    else
      emit sliderRangeChanged(sliderMin(), sliderMax());
  }
}

void
CQIntRangeSlider::
setSliderMinMax(int min, int max, bool force)
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
CQIntRangeSlider::
updateTip()
{
  setToolTip(QString("%1 - %2\n(Range %3 - %4)").
    arg(sliderMin()).arg(sliderMax()).arg(rangeMin()).arg(rangeMax()));
}

//---

void
CQIntRangeSlider::
mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton) {
    pressed_ = true;
    save_    = slider_;

    pixelToSliderValue(e->x(), pressInd_);
  }
}

void
CQIntRangeSlider::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_)
    pixelSetSliderValue(e->x(), pressInd_);
}

void
CQIntRangeSlider::
mouseReleaseEvent(QMouseEvent *e)
{
  if (pressed_) {
    pressed_ = false;

    pixelSetSliderValue(e->x(), pressInd_, /*force*/true);
  }
}

void
CQIntRangeSlider::
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

void
CQIntRangeSlider::
fixSliderValues()
{
  auto i1 = sliderMin();
  auto i2 = sliderMax();

  if (i1 > i2)
    std::swap(i1, i2);

  setSliderMin(clampValue(i1), /*force*/true);
  setSliderMax(clampValue(i2), /*force*/true);
}

int
CQIntRangeSlider::
clampValue(int i) const
{
  return std::min(std::max(i, rangeMin()), rangeMax());
}

int
CQIntRangeSlider::
deltaValue(int i, int inc) const
{
  return i + inc*sliderDelta();
}

void
CQIntRangeSlider::
pixelToSliderValue(int px, int &ind, bool force)
{
  auto x = pixelToValue(px);

  double dx1 = std::abs(sliderMin() - x);
  double dx2 = std::abs(sliderMax() - x);

  ind = (dx1 < dx2 ? 0 : 1);

  if (ind == 0)
    setSliderMin(clampValue(x), force);
  else
    setSliderMax(clampValue(x), force);
}

void
CQIntRangeSlider::
pixelSetSliderValue(int px, int ind, bool force)
{
  auto x = pixelToValue(px);

  if (ind == 0)
    setSliderMin(clampValue(x), force);
  else
    setSliderMax(clampValue(x), force);
}

void
CQIntRangeSlider::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setFont(textFont());

  QFontMetricsF fm(font());

  QFontMetricsF tfm(painter.font());

  double ym = height()/2.0;

  //---

  auto drawText = [&](int x, const QString &text) {
    int dy = (tfm.ascent() - tfm.descent())/2;

    painter.drawText(x, ym + dy, text);
  };

  //---

  int xl = 2;
  int xr = width() - 2;
  int yt = ym - fm.height()/2.0 - 1;
  int yb = ym + fm.height()/2.0 + 1;

  //---

  if (showRangeLabels()) {
    auto minStr = intToString(rangeMin());
    auto maxStr = intToString(rangeMax());

    int twMin = tfm.width(minStr);
    int twMax = tfm.width(maxStr);

    painter.setPen  (palette().color(QPalette::WindowText));
    painter.setBrush(Qt::NoBrush);

    drawText(xl        , minStr);
    drawText(xr - twMax, maxStr);

    xs1_ = xl + twMin + 2;
    xs2_ = xr - twMax - 2;
  }
  else {
    xs1_ = xl;
    xs2_ = xr;
  }

  //---

  auto bg0 = palette().color(QPalette::Background);
  auto bg1 = palette().color(QPalette::Highlight);
  auto fg0 = palette().color(QPalette::Text);
  auto fg1 = blendColors(bg0, fg0, 0.3);

  auto bg2 = blendColors(bg1, bg0, 0.5);
  auto bg3 = blendColors(bg1, fg0, 0.8);

  painter.setPen  (Qt::NoPen);
  painter.setBrush(bg2);

  painter.drawRoundedRect(QRect(xs1_, yt, xs2_ - xs1_ + 1, yb - yt + 1), 3, 3);

  int xs3 = valueToPixel(sliderMin());
  int xs4 = valueToPixel(sliderMax());

  painter.setPen  (fg1);
  painter.setBrush(bg3);

  painter.drawRoundedRect(QRect(xs3, yt, xs4 - xs3 + 1, yb - yt + 1), 1, 1);

  painter.setPen  (fg1);
  painter.setBrush(palette().color(QPalette::Button));

  //---

  double bs = tfm.height()/2.0;

  painter.drawEllipse(QRectF(xs3 - bs/2, ym - bs/2, bs, bs));
  painter.drawEllipse(QRectF(xs4 - bs/2, ym - bs/2, bs, bs));

  //---

  if (showSliderLabels()) {
    auto sminStr = intToString(sliderMin());
    auto smaxStr = intToString(sliderMax());

    int twsMin = tfm.width(sminStr);
    int twsMax = tfm.width(smaxStr);

    painter.setPen  (palette().color(QPalette::HighlightedText));
    painter.setBrush(Qt::NoBrush);

  //int xm = (xs1_ + xs2_)/2;

    int tl1 = xs3 - twsMin - bs;

    if (tl1 < xs1_)
      tl1 = xs3 + bs/2.0 + 2;

    int tl2 = tl1 + twsMin;

    int tle = std::max(tl2, int(xs3 + bs/2.0 + 1));

    int tr1 = xs4 + bs/2.0 + 2;

    if (tr1 + twsMax > xs2_)
      tr1 = xs4 - twsMax - bs/2.0 - 2;

  //int tr2 = tr1 + twsMax;

    int trs = std::min(tr1, int(xs4 - bs/2.0 - 1));

    if (tr1 >= tle)
      drawText(tr1, smaxStr);
    else
      trs = 9999;

    if (tl2 <= trs && tl2 < int(xs4 - bs/2.0 - 1))
      drawText(tl1, sminStr);
  }
}

QString
CQIntRangeSlider::
intToString(int i) const
{
  return QString("%1").arg(i);
}

double
CQIntRangeSlider::
valueToPixel(int x) const
{
  if (rangeMax() > rangeMin())
    return (xs2_ - xs1_)*(1.0*x - rangeMin())/(rangeMax() - rangeMin()) + xs1_;
  else
    return xs1_;
}

int
CQIntRangeSlider::
pixelToValue(double px) const
{
  if (xs2_ > xs1_)
    return (px - xs1_)*(rangeMax() - rangeMin())/(xs2_ - xs1_) + rangeMin();
  else
    return rangeMin();
}

QSize
CQIntRangeSlider::
sizeHint() const
{
  QFontMetrics fm(font());

  int b = 4;

  int w = 2*b;

  int labelWidth = fm.width("XXX");

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
