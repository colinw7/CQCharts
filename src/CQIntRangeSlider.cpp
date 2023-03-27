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

  CQIntRangeSlider::updateTip();
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
  auto x = int(pixelToValue(px));

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
  auto x = int(pixelToValue(px));

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

  //---

  xs1_ = xBorder();
  xs2_ = width() - xBorder();

  // draw optional full range labels at start/end
  if (showRangeLabels())
    drawRangeLabels(&painter);

  //---

  // draw slider
  drawSlider(&painter);

  //---

  // draw current (slider) range labels
  if (showSliderLabels())
    drawSliderLabels(&painter);
}

void
CQIntRangeSlider::
drawRangeLabels(QPainter *painter)
{
  QFontMetricsF fm(font());

  QFontMetricsF tfm(textFont());

  double ym = height()/2.0;

  //---

  auto drawText = [&](int x, const QString &text) {
    int dy = int((tfm.ascent() - tfm.descent())/2.0);

    painter->drawText(x, int(ym + dy), text);
  };

  //---

  auto minStr = intToString(rangeMin());
  auto maxStr = intToString(rangeMax());

  int twMin = int(tfm.horizontalAdvance(minStr));
  int twMax = int(tfm.horizontalAdvance(maxStr));

  painter->setPen  (palette().color(QPalette::WindowText));
  painter->setBrush(Qt::NoBrush);

  drawText(xs1_        , minStr);
  drawText(xs2_ - twMax, maxStr);

  //---

  xs1_ += twMin + 2;
  xs2_ -= twMax + 2;
}

void
CQIntRangeSlider::
drawSliderLabels(QPainter *painter)
{
  QFontMetricsF fm(font());

  QFontMetricsF tfm(textFont());

  double ym = height()/2.0;

  //---

  auto drawText = [&](int x, const QString &text) {
    int dy = int((tfm.ascent() - tfm.descent())/2.0);

    painter->drawText(x, int(ym + dy), text);
  };

  //---

  auto xs3 = valueToPixel(sliderMin());
  auto xs4 = valueToPixel(sliderMax());

  double bs = fm.height()/3.0;

  auto sminStr = intToString(sliderMin());
  auto smaxStr = intToString(sliderMax());

  int twsMin = int(tfm.horizontalAdvance(sminStr));
  int twsMax = int(tfm.horizontalAdvance(smaxStr));

  painter->setPen  (palette().color(QPalette::HighlightedText));
  painter->setBrush(Qt::NoBrush);

//int xm = (xs1_ + xs2_)/2;

  int tl1 = int(xs3 - twsMin - bs/2.0 - 2);

  if (tl1 < xs1_)
    tl1 = int(xs3 + bs/2.0 + 2);

  int tl2 = tl1 + twsMin;

  int tle = std::max(tl2, int(xs3 + bs/2.0 + 1));

  int tr1 = int(xs4 + bs/2.0 + 2);

  if (tr1 + twsMax > xs2_)
    tr1 = int(xs4 - twsMax - bs/2.0 - 2);

//int tr2 = tr1 + twsMax;

  int trs = std::min(tr1, int(xs4 - bs/2.0 - 1));

  if (tr1 >= tle)
    drawText(tr1, smaxStr);
  else
    trs = 9999;

  if (tl2 <= trs && tl2 < int(xs4 - bs/2.0 - 1))
    drawText(tl1, sminStr);
}

QString
CQIntRangeSlider::
intToString(int i) const
{
  return QString::number(i);
}

double
CQIntRangeSlider::
valueToPixel(double x) const
{
  if (rangeMax() > rangeMin())
    return (xs2_ - xs1_)*(x - rangeMin())/(rangeMax() - rangeMin()) + xs1_;
  else
    return xs1_;
}

double
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

  int labelWidth = fm.horizontalAdvance("XXX");

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
