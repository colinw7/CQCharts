#include <CQChartsBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QPainter>

CQChartsBoxObj::
CQChartsBoxObj(CQChartsView *view) :
 CQChartsObj(view),
 CQChartsObjBoxData<CQChartsBoxObj>(this),
 view_(view)
{
}

CQChartsBoxObj::
CQChartsBoxObj(CQChartsPlot *plot) :
 CQChartsObj(plot),
 CQChartsObjBoxData<CQChartsBoxObj>(this),
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
  model->addProperty(path, this, "margin" )->setDesc("Outer margin");
  model->addProperty(path, this, "padding")->setDesc("Inner padding");

  QString bgPath = path + "/fill";

  model->addProperty(bgPath, this, "filled"     , "visible")->setDesc("Fill visible");
  model->addProperty(bgPath, this, "fillColor"  , "color"  )->setDesc("Fill color");
  model->addProperty(bgPath, this, "fillAlpha"  , "alpha"  )->setDesc("Fill alpha");
  model->addProperty(bgPath, this, "fillPattern", "pattern")->setDesc("Fill pattern");

  QString borderPath = path + "/stroke";

  model->addProperty(borderPath, this, "border"     , "visible"   )->setDesc("Stroke visible");
  model->addProperty(borderPath, this, "borderColor", "color"     )->setDesc("Stroke color");
  model->addProperty(borderPath, this, "borderAlpha", "alpha"     )->setDesc("Stroke alpha");
  model->addProperty(borderPath, this, "borderWidth", "width"     )->setDesc("Stroke width");
  model->addProperty(borderPath, this, "cornerSize" , "cornerSize")->setDesc("Box corner size");
  model->addProperty(borderPath, this, "borderSides", "sides"     )->setDesc("Box visible sides");
}

void
CQChartsBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (isFilled()) {
    // set pen and brush
    QBrush brush;
    QPen   pen(Qt::NoPen);

    QColor bgColor = interpFillColor(0, 1);

    if      (plot())
      plot()->setBrush(brush, true, bgColor, fillAlpha(), fillPattern());
    else if (view())
      view()->setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

    if (plot() && isStateColoring())
      plot()->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    // fill border
    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
  }

  if (isBorder()) {
    // set pen and brush
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor borderColor = interpBorderColor(0, 1);

    if      (plot())
      plot()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), borderDash());
    else if (view())
      view()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), borderDash());

    if (plot() && isStateColoring())
      plot()->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    // draw border
    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
  }
}

void
CQChartsBoxObj::
boxDataInvalidate()
{
  if      (plot())
    plot()->drawObjs();
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
