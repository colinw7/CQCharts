#include <CQChartsFillObj.h>
#include <CQPropertyViewModel.h>
#include <CQRoundedPolygon.h>
#include <QPainter>

CQChartsFillObj::
CQChartsFillObj()
{
}

void
CQChartsFillObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible");
  model->addProperty(path, this, "color"  );
  model->addProperty(path, this, "alpha"  );
}

void
CQChartsFillObj::
draw(QPainter *p, const QRectF &rect) const
{
  if (! isVisible())
    return;

  QBrush brush(color());

  p->setBrush(brush);
  p->setPen  (Qt::NoPen);

  p->fillRect(rect, p->brush());
}

void
CQChartsFillObj::
draw(QPainter *p, const QPolygonF &poly) const
{
  if (! isVisible())
    return;

  QBrush brush(color());

  p->setBrush(brush);
  p->setPen  (Qt::NoPen);

  p->drawPolygon(poly);
}
