#include <CQChartsFillObj.h>
#include <CQPropertyViewModel.h>
#include <QPainter>

CQChartsFillObj::
CQChartsFillObj(CQChartsPlot *plot) :
 plot_(plot)
{
  color_.setValue(CQChartsPaletteColor::Type::PALETTE);
}

void
CQChartsFillObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible");
  model->addProperty(path, this, "color"  );
  model->addProperty(path, this, "alpha"  );
}

QColor
CQChartsFillObj::
interpColor(int i, int n) const
{
  return color_.interpColor(plot_, i, n);
}

void
CQChartsFillObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (! isVisible())
    return;

  QBrush brush(interpColor(0, 1));

  painter->setBrush(brush);
  painter->setPen  (Qt::NoPen);

  painter->fillRect(rect, painter->brush());
}

void
CQChartsFillObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  if (! isVisible())
    return;

  QBrush brush(interpColor(0, 1));

  painter->setBrush(brush);
  painter->setPen  (Qt::NoPen);

  painter->drawPolygon(poly);
}
