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
 CQChartsObj(view->charts()),
 CQChartsObjBoxData<CQChartsBoxObj>(this),
 view_(view)
{
}

CQChartsBoxObj::
CQChartsBoxObj(CQChartsPlot *plot) :
 CQChartsObj(plot->charts()),
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
addProperties(CQPropertyViewModel *model, const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden=false) {
    CQPropertyViewItem *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addProp(path, "margin" , "", desc.length() ? desc + " inner margin" : "Inner margin");
  addProp(path, "padding", "", desc.length() ? desc + " outer padding" : "Outer padding");

  QString bgPath = path + "/fill";

  QString fillDesc = (desc.length() ? desc + " fill" : "Fill");

  addStyleProp(bgPath, "filled"     , "visible", fillDesc + " visible");
  addStyleProp(bgPath, "fillColor"  , "color"  , fillDesc + " color");
  addStyleProp(bgPath, "fillAlpha"  , "alpha"  , fillDesc + " alpha");
  addStyleProp(bgPath, "fillPattern", "pattern", fillDesc + " pattern", true);

  QString strokePath = path + "/stroke";

  QString strokeDesc = (desc.length() ? desc + " stroke" : "Stroke");

  addStyleProp(strokePath, "stroked"    , "visible", strokeDesc + " visible");
  addStyleProp(strokePath, "strokeColor", "color"  , strokeDesc + " color");
  addStyleProp(strokePath, "strokeAlpha", "alpha"  , strokeDesc + " alpha");
  addStyleProp(strokePath, "strokeWidth", "width"  , strokeDesc + " width");

  addStyleProp(strokePath, "cornerSize" , "cornerSize",
               desc.length() ? desc + "box corner size" : "Box corner size");
  addStyleProp(strokePath, "borderSides", "sides"     ,
               desc.length() ? desc + "box visible sides" : "Box visible sides");
}

void
CQChartsBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (isFilled()) {
    // set pen and brush
    QBrush brush;
    QPen   pen(Qt::NoPen);

    QColor bgColor = interpFillColor(ColorInd());

    if      (plot())
      plot()->setBrush(brush, true, bgColor, fillAlpha(), fillPattern());
    else if (view())
      view()->setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

    if (isStateColoring()) {
      if      (plot())
        plot()->updateObjPenBrushState(this, pen, brush);
      else if (view())
        view()->updateObjPenBrushState(this, pen, brush);
    }

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    // fill box
    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys, borderSides());
  }

  if (isStroked()) {
    // set pen and brush
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor strokeColor = interpStrokeColor(ColorInd());

    if      (plot())
      plot()->setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());
    else if (view())
      view()->setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

    if (isStateColoring()) {
      if      (plot())
        plot()->updateObjPenBrushState(this, pen, brush);
      else if (view())
        view()->updateObjPenBrushState(this, pen, brush);
    }

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    // stroke box
    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys, borderSides());
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

    QColor bgColor = interpFillColor(ColorInd());

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

  if (isStroked()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor strokeColor = interpStrokeColor(ColorInd());

    if      (plot())
      plot()->setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());
    else if (view())
      view()->setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, poly, cxs, cys);
  }
}
