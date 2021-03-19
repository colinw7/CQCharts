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
mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton) {
    pressed_ = true;
    save_    = slider_;

    pixelToSliderValue(e->x(), pressInd_);
  }
}

void
CQTimeRangeSlider::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_)
    pixelSetSliderValue(e->x(), pressInd_);
}

void
CQTimeRangeSlider::
mouseReleaseEvent(QMouseEvent *e)
{
  if (pressed_) {
    pressed_ = false;

    pixelSetSliderValue(e->x(), pressInd_, /*force*/true);
  }
}

void
CQTimeRangeSlider::
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
CQTimeRangeSlider::
pixelSetSliderValue(int px, int ind, bool force)
{
  double x = pixelToValue(px);

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

  QFontMetricsF fm(font());

  double ym = height()/2.0;

  int xl = 2;
  int xr = width() - 2;
  int yt = ym - fm.height()/2.0 - 1;
  int yb = ym + fm.height()/2.0 + 1;

  //---

  if (showRangeLabels()) {
    QString minStr = timeToString(rangeMin());
    QString maxStr = timeToString(rangeMax());

    int twMin = fm.width(minStr);
    int twMax = fm.width(maxStr);

    painter.setPen  (palette().color(QPalette::WindowText));
    painter.setBrush(Qt::NoBrush);

    painter.drawText(xl        , yt + fm.ascent(), minStr);
    painter.drawText(xr - twMax, yt + fm.ascent(), maxStr);

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

  double bs = fm.height()/2.0;

  painter.drawEllipse(QRectF(xs3 - bs/2, ym - bs/2, bs, bs));
  painter.drawEllipse(QRectF(xs4 - bs/2, ym - bs/2, bs, bs));

  //---

  if (showSliderLabels()) {
    QString sminStr = timeToString(sliderMin());
    QString smaxStr = timeToString(sliderMax());

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
      painter.drawText(xs5, yt + fm.ascent(), sminStr);

    int xs6 = xs4 + bs;

    if (xs6 + twsMax > xs2_) {
      xs6 = xs4 - twsMax - bs;

      if (xs6 < xm)
        xs6 = -1;
    }

    if (xs6 >= 0)
      painter.drawText(xs6, yt + fm.ascent(), smaxStr);
  }
}

QString
CQTimeRangeSlider::
timeToString(double r) const
{
  static char buffer[512];

  time_t t(r);

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

  t = mktime(&tm1);

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
