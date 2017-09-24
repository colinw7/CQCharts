#include <CQChartsBoxObj.h>
#include <CQPropertyViewTree.h>
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

  tree->addProperty(borderPath, this, "border"      , "visible");
  tree->addProperty(borderPath, this, "borderColor" , "color"  );
  tree->addProperty(borderPath, this, "borderWidth" , "width"  );
  tree->addProperty(borderPath, this, "borderRadius", "radius" );
}

void
CQChartsBoxObj::
draw(QPainter *p, const QRectF &rect)
{
  if (isBackground()) {
    p->fillRect(rect, QBrush(backgroundColor()));
  }

  if (isBorder()) {
    QPen pen(borderColor());

    pen.setWidth(borderWidth());

    p->setPen  (pen);
    p->setBrush(Qt::NoBrush);

    if (borderRadius() > 0)
      p->drawRoundedRect(rect, borderRadius(), borderRadius());
    else
      p->drawRect(rect);
  }
}

void
CQChartsBoxObj::
draw(QPainter *p, const QPolygonF &poly)
{
  if (isBackground()) {
    p->setPen  (Qt::NoPen);
    p->setBrush(backgroundColor());

    p->drawPolygon(poly);
  }

  if (isBorder()) {
    QPen pen(borderColor());

    pen.setWidth(borderWidth());

    p->setPen  (pen);
    p->setBrush(Qt::NoBrush);

    p->drawPolygon(poly);
  }
}
