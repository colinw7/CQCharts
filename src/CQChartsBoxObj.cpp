#include <CQChartsBoxObj.h>
#include <CQPropertyViewTree.h>
#include <CQRoundedPolygon.h>
#include <QPainter>

CQChartsBoxObj::
CQChartsBoxObj()
{
}

void
CQChartsBoxObj::
addProperties(CQPropertyViewTree *tree, const QString &path)
{
  tree->addProperty(path, this, "margin" );
  tree->addProperty(path, this, "padding");

  QString bgPath = path + "/background";

  tree->addProperty(bgPath, this, "background"     , "visible");
  tree->addProperty(bgPath, this, "backgroundColor", "color"  );

  QString borderPath = path + "/border";

  tree->addProperty(borderPath, this, "border"          , "visible"   );
  tree->addProperty(borderPath, this, "borderColor"     , "color"     );
  tree->addProperty(borderPath, this, "borderWidth"     , "width"     );
  tree->addProperty(borderPath, this, "borderCornerSize", "cornerSize");
}

void
CQChartsBoxObj::
draw(QPainter *p, const QRectF &rect) const
{
  if (isBackground()) {
    QBrush brush(backgroundColor());

    p->setBrush(brush);
    p->setPen  (Qt::NoPen);

    CQRoundedPolygon::draw(p, rect, borderCornerSize());
  }

  if (isBorder()) {
    QPen pen(borderColor());

    pen.setWidth(borderWidth());

    p->setPen  (pen);
    p->setBrush(Qt::NoBrush);

    CQRoundedPolygon::draw(p, rect, borderCornerSize());
  }
}

void
CQChartsBoxObj::
draw(QPainter *p, const QPolygonF &poly) const
{
  if (isBackground()) {
    QBrush brush(backgroundColor());

    p->setBrush(brush);
    p->setPen  (Qt::NoPen);

    CQRoundedPolygon::draw(p, poly, borderCornerSize());
  }

  if (isBorder()) {
    QPen pen(borderColor());

    pen.setWidth(borderWidth());

    p->setPen  (pen);
    p->setBrush(Qt::NoBrush);

    CQRoundedPolygon::draw(p, poly, borderCornerSize());
  }
}
