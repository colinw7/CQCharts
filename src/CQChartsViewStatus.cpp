#include <CQChartsViewStatus.h>
#include <CQChartsView.h>
#include <QPainter>

CQChartsViewStatus::
CQChartsViewStatus(CQChartsView *view) :
 QFrame(view), view_(view)
{
  setObjectName("status");
}

void
CQChartsViewStatus::
setText(const QString &s)
{
  text_ = s;

  update();
}

void
CQChartsViewStatus::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), palette().color(QPalette::Window));

  QFontMetrics fm(font());

  p.drawText(2, 2 + fm.ascent(), text());
}

QSize
CQChartsViewStatus::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XX"), fm.height() + 4);
}
