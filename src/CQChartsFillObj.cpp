#include <CQChartsFillObj.h>
#include <CQChartsRenderer.h>
#include <CQPropertyViewModel.h>

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
draw(CQChartsRenderer *renderer, const QRectF &rect) const
{
  if (! isVisible())
    return;

  QBrush brush(interpColor(0, 1));

  renderer->setBrush(brush);
  renderer->setPen  (Qt::NoPen);

  renderer->fillRect(rect, renderer->brush());
}

void
CQChartsFillObj::
draw(CQChartsRenderer *renderer, const QPolygonF &poly) const
{
  if (! isVisible())
    return;

  QBrush brush(interpColor(0, 1));

  renderer->setBrush(brush);
  renderer->setPen  (Qt::NoPen);

  renderer->drawPolygon(poly);
}
