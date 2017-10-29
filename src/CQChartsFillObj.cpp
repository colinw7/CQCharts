#include <CQChartsFillObj.h>
#include <CQPropertyViewTree.h>
#include <CQRoundedPolygon.h>
#include <QPainter>

CQChartsFillObj::
CQChartsFillObj()
{
}

void
CQChartsFillObj::
addProperties(CQPropertyViewTree *tree, const QString &path)
{
  tree->addProperty(path, this, "visible");
  tree->addProperty(path, this, "color"  );
  tree->addProperty(path, this, "alpha"  );
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
