#include <CQChartsLineObj.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewModel.h>
#include <QPainter>

CQChartsLineObj::
CQChartsLineObj()
{
}

void
CQChartsLineObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "displayed", "visible");
  model->addProperty(path, this, "color"    , "color"  );
  model->addProperty(path, this, "width"    , "width"  );
  model->addProperty(path, this, "dash"     , "dash"   );
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

  draw(p, p1, p2, pen);
}

void
CQChartsLineObj::
draw(QPainter *p, const QPointF &p1, const QPointF &p2, const QPen &pen)
{
  p->setPen(pen);

  p->drawLine(p1, p2);
}
