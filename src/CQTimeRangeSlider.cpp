#include <CQTimeRangeSlider.h>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFontMetrics>
#include <cmath>

CQTimeRangeSlider::
CQTimeRangeSlider(QWidget *parent) :
 CQRangeSlider(parent)
{
  setObjectName("timeRangeSlider");

  setFocusPolicy(Qt::ClickFocus);

  range_.min = 0.0;
  range_.max = double(std::time(nullptr));

  slider_.min = range_.min;
  slider_.max = range_.max;

  updateTip();
}

//---

void
CQTimeRangeSlider::
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
CQTimeRangeSlider::
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
CQTimeRangeSlider::
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
CQTimeRangeSlider::
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
CQTimeRangeSlider::
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
CQTimeRangeSlider::
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
CQTimeRangeSlider::
updateTip()
{
  setToolTip(QString("%1 - %2\n(Range %3 - %4)").
    arg(timeToString(sliderMin())).arg(timeToString(sliderMax())).
    arg(timeToString(rangeMin ())).arg(timeToString(rangeMax())));
}

//---

void
CQTimeRangeSlider::
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
CQTimeRangeSlider::
clampValue(double r) const
{
  return std::min(std::max(r, rangeMin()), rangeMax());
}

double
CQTimeRangeSlider::
deltaValue(double r, int inc) const
{
  return r + inc*sliderDelta();
}

void
CQTimeRangeSlider::
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
CQTimeRangeSlider::
pixelSetSliderValue(int px, int ind, bool force)
{
  auto x = pixelToValue(px);

  if (ind == 0)
    setSliderMin(clampValue(x), force);
  else
    setSliderMax(clampValue(x), force);
}

void
CQTimeRangeSlider::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setFont(textFont());

  QFontMetricsF fm(painter.font());

  QFontMetricsF tfm(painter.font());

  double ym = height()/2.0;

  //---

  auto drawText = [&](int x, const QString &text) {
    int dy = int((tfm.ascent() - tfm.descent())/2.0);

    painter.drawText(x, int(ym + dy), text);
  };

  //---

  xs1_ = xBorder();
  xs2_ = width() - xBorder();

  //---

  if (showRangeLabels()) {
    auto minStr = timeToString(rangeMin());
    auto maxStr = timeToString(rangeMax());

    int twMin = int(tfm.width(minStr));
    int twMax = int(tfm.width(maxStr));

    painter.setPen  (palette().color(QPalette::WindowText));
    painter.setBrush(Qt::NoBrush);

    drawText(xs1_        , minStr);
    drawText(xs2_ - twMax, maxStr);

    xs1_ += twMin + 2;
    xs2_ -= twMax + 2;
  }

  //---

  drawSlider(&painter);

  //---

  auto xs3 = valueToPixel(sliderMin());
  auto xs4 = valueToPixel(sliderMax());

  double bs = tfm.height()/2.0;

  painter.drawEllipse(QRectF(xs3 - bs/2, ym - bs/2, bs, bs));
  painter.drawEllipse(QRectF(xs4 - bs/2, ym - bs/2, bs, bs));

  //---

  if (showSliderLabels()) {
    auto sminStr = timeToString(sliderMin());
    auto smaxStr = timeToString(sliderMax());

    int twsMin = int(tfm.width(sminStr));
    int twsMax = int(tfm.width(smaxStr));

    painter.setPen  (palette().color(QPalette::HighlightedText));
    painter.setBrush(Qt::NoBrush);

  //int xm = (xs1_ + xs2_)/2;

    int tl1 = int(xs3 - twsMin - bs);

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
}

QString
CQTimeRangeSlider::
timeToString(double r) const
{
  static char buffer[512];

  auto t = static_cast<time_t>(r);

  struct tm *tm1 = localtime(&t);
  if (! tm1) return "<no_time>";

  (void) strftime(buffer, 512, formatStr_.toLatin1().constData(), tm1);

  return buffer;
}

bool
CQTimeRangeSlider::
stringToTime(const QString &str, double &t) const
{
  struct tm tm1; memset(&tm1, 0, sizeof(tm));

  char *p = strptime(str.toLatin1().constData(), formatStr_.toLatin1().constData(), &tm1);
  if (! p) return false;

  t = static_cast<double>(mktime(&tm1));

  return true;
}

double
CQTimeRangeSlider::
valueToPixel(double x) const
{
  if (rangeMax() > rangeMin())
    return (xs2_ - xs1_)*(x - rangeMin())/(rangeMax() - rangeMin()) + xs1_;
  else
    return xs1_;
}

double
CQTimeRangeSlider::
pixelToValue(double px) const
{
  if (xs2_ > xs1_)
    return (px - xs1_)*(rangeMax() - rangeMin())/(xs2_ - xs1_) + rangeMin();
  else
    return rangeMin();
}

QSize
CQTimeRangeSlider::
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
