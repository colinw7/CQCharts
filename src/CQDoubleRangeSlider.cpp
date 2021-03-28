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

  update();
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

  QFontMetricsF fm(font());

  QFontMetricsF tfm(painter.font());

  double ym = height()/2.0;

  //---

  auto interpValue = [](double v1, double v2, double f) {
    return v1*(1 - f) + v2*f;
  };

  auto interpColor = [&](const QColor &c1, const QColor &c2, double f) {
    qreal r1, g1, b1, a1;
    qreal r2, g2, b2, a2;

    c1.getRgbF(&r1, &g1, &b1, &a1);
    c2.getRgbF(&r2, &g2, &b2, &a2);

    return QColor::fromRgbF(interpValue(r1, r2, f),
                            interpValue(g1, g2, f),
                            interpValue(b1, b2, f));
  };

  auto colorAt = [&](double x) {
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
  };

  auto bwColor = [&](const QColor &c) {
    int g = qGray(c.red(), c.green(), c.blue());

    return (g > 128 ? QColor(0, 0, 0) : QColor(255, 255, 255));
  };

  auto drawText = [&](int x, const QString &text) {
    if (lgSet_) {
      int w = tfm.width(text);

      int xm = x + w/2;

      painter.setPen(bwColor(colorAt(1.0*xm/width())));
    }

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
    auto minStr = realToString(rangeMin());
    auto maxStr = realToString(rangeMax());

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

  double bs = tfm.height()/2.0;

  painter.drawEllipse(QRectF(xs3 - bs/2, ym - bs/2, bs, bs));
  painter.drawEllipse(QRectF(xs4 - bs/2, ym - bs/2, bs, bs));

  //---

  if (showSliderLabels()) {
    auto sminStr = realToString(sliderMin());
    auto smaxStr = realToString(sliderMax());

    int twsMin = tfm.width(sminStr);
    int twsMax = tfm.width(smaxStr);

    painter.setPen  (palette().color(QPalette::HighlightedText));
    painter.setBrush(Qt::NoBrush);

  //int xm = (xs1_ + xs2_)/2;

    int tl1 = xs3 - twsMin - bs/2.0 - 2;

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
CQDoubleRangeSlider::
realToString(double r) const
{
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
