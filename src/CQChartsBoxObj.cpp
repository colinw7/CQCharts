#include <CQChartsBoxObj.h>
#include <CQPropertyViewModel.h>
#include <CQRoundedPolygon.h>
#include <QPainter>

CQChartsBoxObj::
CQChartsBoxObj()
{
}

void
CQChartsBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "margin" );
  model->addProperty(path, this, "padding");

  QString bgPath = path + "/background";

  model->addProperty(bgPath, this, "background"     , "visible");
  model->addProperty(bgPath, this, "backgroundColor", "color"  );

  QString borderPath = path + "/border";

  model->addProperty(borderPath, this, "border"          , "visible"   );
  model->addProperty(borderPath, this, "borderColor"     , "color"     );
  model->addProperty(borderPath, this, "borderWidth"     , "width"     );
  model->addProperty(borderPath, this, "borderCornerSize", "cornerSize");
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
