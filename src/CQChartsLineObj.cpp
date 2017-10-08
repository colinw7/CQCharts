#include <CQChartsLineObj.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewTree.h>
#include <QPainter>

CQChartsLineObj::
CQChartsLineObj()
{
}

void
CQChartsLineObj::
addProperties(CQPropertyViewTree *tree, const QString &path)
{
  tree->addProperty(path, this, "displayed", "visible");
  tree->addProperty(path, this, "color"    , "color"  );
  tree->addProperty(path, this, "width"    , "width"  );
  tree->addProperty(path, this, "dash"     , "dash"   );
}

void
CQChartsLineObj::
draw(QPainter *p, const QPointF &p1, const QPointF &p2) const
{
  draw(p, p1, p2, color(), width(), dash());
}

void
CQChartsLineObj::
draw(QPainter *p, const QPointF &p1, const QPointF &p2, const QColor &color,
     double width, const CLineDash &dash)
{
  QPen pen(color);

  if (width > 0.0)
    pen.setWidth(width);

  CQChartsUtil::penSetLineDash(pen, dash);

  p->setPen(pen);

  p->drawLine(p1, p2);
}
