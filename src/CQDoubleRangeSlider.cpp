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
setRangeMinMax(double min, double max, bool resetSlider)
{
  if (min != range_.min || max != range_.max) {
    range_.min = min;
    range_.max = max;

    if (range_.min > range_.max)
      std::swap(range_.min, range_.max);

    if (resetSlider) {
      slider_.min = range_.min;
      slider_.max = range_.max;
    }

    updateTip();

    update();

    emit rangeChanged(rangeMin(), rangeMax());

    if (resetSlider)
      emit sliderRangeChanged(sliderMin(), sliderMax());
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
updateTip()
{
  setToolTip(QString("%1 - %2\n(Range %3 - %4)").
    arg(sliderMin()).arg(sliderMax()).arg(rangeMin()).arg(rangeMax()));
}

//---

void
CQDoubleRangeSlider::
fixSliderValues()
{
  auto i1 = sliderMin();
  auto i2 = sliderMax();

  if (i1 > i2)
    std::swap(i1, i2);

  setSliderMin(clampValue(i1), /*force*/true);
  setSliderMax(clampValue(i2), /*force*/true);
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
    int n = -int(std::round(std::log10(sliderDelta())));

    double scale = std::pow(10, n);

    int v = int(std::round(r*scale) + inc);

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
CQDoubleRangeSlider::
pixelSetSliderValue(int px, int ind, bool force)
{
  auto x = pixelToValue(px);

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

  painter.setFont(textFont());

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
CQDoubleRangeSlider::
drawRangeLabels(QPainter *painter)
{
  QFontMetricsF fm(font());

  QFontMetricsF tfm(painter->font());

  double ym = height()/2.0;

  //---

  auto minStr = realToString(rangeMin());
  auto maxStr = realToString(rangeMax());

  int twMin = int(tfm.horizontalAdvance(minStr));
  int twMax = int(tfm.horizontalAdvance(maxStr));

  painter->setPen  (palette().color(QPalette::WindowText));
  painter->setBrush(Qt::NoBrush);

  drawText(painter, xs1_        , int(ym), minStr);
  drawText(painter, xs2_ - twMax, int(ym), maxStr);

  //---

  xs1_ += twMin + 2;
  xs2_ -= twMax + 2;
}

void
CQDoubleRangeSlider::
drawSliderLabels(QPainter *painter)
{
  QFontMetricsF fm(font());

  QFontMetricsF tfm(textFont());

  double ym = height()/2.0;

  //---

  auto xs3 = valueToPixel(sliderMin());
  auto xs4 = valueToPixel(sliderMax());

  double bs = fm.height()/3.0;

  auto sminStr = realToString(sliderMin());
  auto smaxStr = realToString(sliderMax());

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
    drawText(painter, tr1, int(ym), smaxStr);
  else
    trs = 9999;

  if (tl2 <= trs && tl2 < int(xs4 - bs/2.0 - 1))
    drawText(painter, tl1, int(ym), sminStr);
}

void
CQDoubleRangeSlider::
drawText(QPainter *painter, int x, int y, const QString &text)
{
  QFontMetricsF tfm(textFont());

  if (lgSet_) {
    int w = int(tfm.horizontalAdvance(text));

    int xm = x + w/2;

    auto c = bwColor(colorAt(1.0*xm/width()));

    if (! isEnabled())
      c.setAlphaF(0.8);

    painter->setPen(c);
  }

  int dy = int((tfm.ascent() - tfm.descent())/2.0);

  painter->drawText(x, y + dy, text);
};

QString
CQDoubleRangeSlider::
realToString(double r) const
{
  if (decimalPlaces() <= 0)
    return QString("%1").arg(int(r));
  else
    return QString("%1").arg(r, 0, 'g', decimalPlaces());
}

double
CQDoubleRangeSlider::
valueToPixel(double x) const
{
  if (rangeMax() > rangeMin())
    return (xs2_ - xs1_)*(x - rangeMin())/(rangeMax() - rangeMin()) + xs1_;
  else
    return xs1_;
}

double
CQDoubleRangeSlider::
pixelToValue(double px) const
{
  if (xs2_ > xs1_)
    return (px - xs1_)*(rangeMax() - rangeMin())/(xs2_ - xs1_) + rangeMin();
  else
    return rangeMin();
}

QSize
CQDoubleRangeSlider::
sizeHint() const
{
  QFontMetrics fm(font());

  int b = 4;

  int w = 2*b;

  int labelWidth = fm.horizontalAdvance("XXX.XXX");

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
