#include <CQBusyButton.h>

#include <QStylePainter>
#include <QStyleOption>
#include <QTimer>

#include <cmath>

CQBusyButton::
CQBusyButton(QWidget *parent) :
 QToolButton(parent)
{
  setObjectName("busyButton");

  setLabel("Ready");

  timer_ = new QTimer(this);

  connect(timer_, SIGNAL(timeout()), this, SLOT(timerSlot()));

  connect(this, SIGNAL(clicked()), this, SLOT(clickSlot()));

  setBusy(true);
}

CQBusyButton::
~CQBusyButton()
{
}

void
CQBusyButton::
timerSlot()
{
  ++ticks_;

  update();
}

void
CQBusyButton::
clickSlot()
{
  setBusy(! isBusy());

  emit busyStateChanged(isBusy());
}

void
CQBusyButton::
setBusy(bool busy)
{
  if (busy == busy_)
    return;

  busy_  = busy;
  ticks_ = 0;

  label_ = (busy_ ? "Busy" : "Ready");

  if (busy_)
    timer_->start(100);
  else
    timer_->stop();

  update();
}

void
CQBusyButton::
paintEvent(QPaintEvent *)
{
  QStylePainter p(this);

  QStyleOptionToolButton opt;

  initStyleOption(&opt);

  p.drawComplexControl(QStyle::CC_ToolButton, opt);

  QFontMetrics fm(font());

  auto fg = palette().windowText().color();

  if (busy_) {
    drawBusy(&p);

    int is = fm.height() + 2;

    p.setPen(fg);
    p.drawText(is + 4, fm.ascent(), label());
  }
  else {
    int tw = fm.horizontalAdvance(label());

    p.setPen(fg);
    p.drawText((width() - tw)/2, fm.ascent(), label());
  }
}

void
CQBusyButton::
drawBusy(QPainter *painter) const
{
  QFontMetrics fm(font());

  int is = fm.height() + 2;

  int x = is/2 + 2;
  int y = height()/2;

  int count = 10; // number if border circles to draw

  double a  = 0.0;
  double da = 2.0*M_PI/count;

  double r1 = is/3.0; // circle position radius
  double r2 = r1/8.0; // min circle size
  double r3 = r1/3.0; // max circle size

  auto fg = palette().highlight().color();

  QPen pen(Qt::NoPen);
  painter->setPen(pen);

  auto circlePoint = [](const QPointF &c, double r, double a) {
    double ca = std::cos(a); double sa = std::sin(a);

    return QPointF(c.x() + r*ca, c.y() + r*sa);
  };

  int ind = ticks_ % count;

  for (int i = 0; i < count; ++i) {
    int i1 = i - ind;

    if (i1 < 0) i1 += count;

    double r = i1*(r2 - r3)/(count - 1) + r3;

    QPointF c(x, y);

    auto p1 = circlePoint(c, r1, a);

    QRectF qrect(p1.x() - r, p1.y() - r, 2*r, 2*r);

    fg.setAlpha(int(255*r/r3));
    QBrush brush(fg);
    painter->setBrush(brush);

    painter->drawEllipse(qrect);

    a += da;
  }
}

QSize
CQBusyButton::
sizeHint() const
{
  QFontMetrics fm(font());

  int is = fm.height() + 2;

  int tw = fm.horizontalAdvance(label());

  return QSize(tw + is + 6, is + 4);
}
