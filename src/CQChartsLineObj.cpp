#include <CQChartsLineObj.h>
#include <CQChartsUtil.h>
#include <CQChartsRenderer.h>
#include <CQPropertyViewModel.h>

CQChartsLineObj::
CQChartsLineObj(CQChartsPlot *plot) :
 plot_(plot)
{
  color_.setValue(CQChartsPaletteColor::Type::PALETTE);
}

QColor
CQChartsLineObj::
interpColor(int i, int n) const
{
  return color_.interpColor(plot_, i, n);
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
draw(CQChartsRenderer *renderer, const QPointF &p1, const QPointF &p2) const
{
  QColor c = interpColor(0, 1);

  draw(renderer, p1, p2, c, width(), dash());
}

void
CQChartsLineObj::
draw(CQChartsRenderer *renderer, const QPointF &p1, const QPointF &p2, const QColor &color,
     double width, const CQChartsLineDash &dash)
{
  QPen pen(color);

  if (width > 0.0)
    pen.setWidthF(width);

  CQChartsUtil::penSetLineDash(pen, dash);

  draw(renderer, p1, p2, pen);
}

void
CQChartsLineObj::
draw(CQChartsRenderer *renderer, const QPointF &p1, const QPointF &p2, const QPen &pen)
{
  renderer->setPen(pen);

  renderer->drawLine(p1, p2);
}
