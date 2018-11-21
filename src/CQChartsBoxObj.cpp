#include <CQChartsBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQPropertyViewModel.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>

CQChartsBoxObj::
CQChartsBoxObj(CQChartsView *view) :
 CQChartsObj(view),
 CQChartsObjShapeData<CQChartsBoxObj>(this),
 view_(view)
{
}

CQChartsBoxObj::
CQChartsBoxObj(CQChartsPlot *plot) :
 CQChartsObj(plot),
 CQChartsObjShapeData<CQChartsBoxObj>(this),
 plot_(plot)
{
}

CQCharts *
CQChartsBoxObj::
charts() const
{
  return view()->charts();
}

CQChartsView *
CQChartsBoxObj::
view() const
{
  if (plot())
    return plot()->view();
  else
    return view_;
}

void
CQChartsBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "margin" );
  model->addProperty(path, this, "padding");

  QString bgPath = path + "/fill";

  model->addProperty(bgPath, this, "filled"     , "visible");
  model->addProperty(bgPath, this, "fillColor"  , "color"  );
  model->addProperty(bgPath, this, "fillAlpha"  , "alpha"  );
  model->addProperty(bgPath, this, "fillPattern", "pattern");

  QString borderPath = path + "/border";

  model->addProperty(borderPath, this, "border"     , "visible"   );
  model->addProperty(borderPath, this, "borderColor", "color"     );
  model->addProperty(borderPath, this, "borderAlpha", "alpha"     );
  model->addProperty(borderPath, this, "borderWidth", "width"     );
  model->addProperty(borderPath, this, "cornerSize" , "cornerSize");
  model->addProperty(borderPath, this, "borderSides", "sides"     );
}

void
CQChartsBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (isFilled()) {
    QBrush brush;
    QPen   pen(Qt::NoPen);

    QColor bgColor = interpFillColor(0, 1);

    if      (plot())
      plot()->setBrush(brush, true, bgColor, fillAlpha(), fillPattern());
    else if (view())
      view()->setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

    if (plot())
      plot()->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
  }

  if (isBorder()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor borderColor = interpBorderColor(0, 1);

    if      (plot())
      plot()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), borderDash());
    else if (view())
      view()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), borderDash());

    if (plot())
      plot()->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
  }
}

void
CQChartsBoxObj::
redrawBoxObj()
{
  if      (plot())
    plot()->invalidateLayers();
  else if (view())
    view()->update();
}

void
CQChartsBoxObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  if (isFilled()) {
    QBrush brush;
    QPen   pen(Qt::NoPen);

    QColor bgColor = interpFillColor(0, 1);

    if      (plot())
      plot()->setBrush(brush, true, bgColor, fillAlpha(), fillPattern());
    else if (view())
      view()->setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, poly, cxs, cys);
  }

  if (isBorder()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor borderColor = interpBorderColor(0, 1);

    if      (plot())
      plot()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), borderDash());
    else if (view())
      view()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), borderDash());

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, poly, cxs, cys);
  }
}
