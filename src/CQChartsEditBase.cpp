#include <CQChartsEditBase.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsColor.h>
#include <CQCharts.h>

#include <QLabel>
#include <QGridLayout>
#include <QPainter>

void
CQChartsEditBase::
drawCenteredText(QPainter *painter, const QString &text)
{
  QColor c = palette().color(QPalette::Window);

  QColor tc = CQChartsUtil::invColor(c);

  painter->setPen(tc);

  QFontMetricsF fm(font());

  double fa = fm.ascent();
  double fd = fm.descent();

  painter->drawText(rect().left() + 2, rect().center().y() + (fa - fd)/2, text);
}

//------

CQChartsEditPreview::
CQChartsEditPreview(CQChartsEditBase *edit) :
 edit_(edit)
{
}

void
CQChartsEditPreview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  draw(&painter);
}

QColor
CQChartsEditPreview::
interpColor(CQChartsPlot *plot, CQChartsView *view, const CQChartsColor &color)
{
  QColor c;

  if      (plot)
    return plot->charts()->interpColor(color, 0, 1);
  else if (view)
    return view->charts()->interpColor(color, 0, 1);
  else
    return color.color();
}

QSize
CQChartsEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
