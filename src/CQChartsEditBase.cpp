#include <CQChartsEditBase.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsColor.h>
#include <CQCharts.h>
#include <QPainter>

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
