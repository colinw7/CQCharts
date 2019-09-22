#include <CQChartsBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsBoxObj::
CQChartsBoxObj(CQChartsView *view) :
 CQChartsViewPlotObj(view), CQChartsObjBoxData<CQChartsBoxObj>(this)
{
}

CQChartsBoxObj::
CQChartsBoxObj(CQChartsPlot *plot) :
 CQChartsViewPlotObj(plot), CQChartsObjBoxData<CQChartsBoxObj>(this)
{
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
draw(CQChartsPaintDevice *device, const QRectF &rect) const
{
  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor bgColor = interpFillColor(ColorInd());

  setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

  QColor strokeColor = interpStrokeColor(ColorInd());

  setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

  if (isStateColoring())
    updatePenBrushState(pen, brush);

  draw(device, rect, pen, brush);
}

#if 0
void
CQChartsBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor bgColor = interpFillColor(ColorInd());

  setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

  QColor strokeColor = interpStrokeColor(ColorInd());

  setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

  if (isStateColoring())
    updatePenBrushState(pen, brush);

  draw(painter, rect, pen, brush);
}
#endif

void
CQChartsBoxObj::
draw(CQChartsPaintDevice *device, const QRectF &rect, const QPen &pen, const QBrush &brush) const
{
  if (isFilled()) {
    // set pen and brush
    QPen pen1(Qt::NoPen);

    device->setPen  (pen1);
    device->setBrush(brush);

    //---

    // fill box
    CQChartsDrawUtil::drawRoundedPolygon(device, rect, cornerSize(), cornerSize(), borderSides());
  }

  if (isStroked()) {
    // set pen and brush
    QBrush brush1(Qt::NoBrush);

    device->setPen  (pen);
    device->setBrush(brush1);

    //---

    // stroke box
    CQChartsDrawUtil::drawRoundedPolygon(device, rect, cornerSize(), cornerSize(), borderSides());
  }
}

#if 0
void
CQChartsBoxObj::
draw(QPainter *painter, const QRectF &rect, const QPen &pen, const QBrush &brush) const
{
  double cxs = lengthPixelWidth (cornerSize());
  double cys = lengthPixelHeight(cornerSize());

  if (isFilled()) {
    // set pen and brush
    QPen pen1(Qt::NoPen);

    painter->setPen  (pen1);
    painter->setBrush(brush);

    //---

    // fill box
    CQChartsDrawUtil::drawRoundedPolygon(painter, rect, cxs, cys, borderSides());
  }

  if (isStroked()) {
    // set pen and brush
    QBrush brush1(Qt::NoBrush);

    painter->setPen  (pen);
    painter->setBrush(brush1);

    //---

    // stroke box
    CQChartsDrawUtil::drawRoundedPolygon(painter, rect, cxs, cys, borderSides());
  }
}
#endif

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
draw(CQChartsPaintDevice *device, const QPolygonF &poly) const
{
  if (isFilled()) {
    QBrush brush;
    QPen   pen(Qt::NoPen);

    QColor bgColor = interpFillColor(ColorInd());

    setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

    device->setPen  (pen);
    device->setBrush(brush);

    CQChartsDrawUtil::drawRoundedPolygon(device, poly, cornerSize(), cornerSize());
  }

  if (isStroked()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor strokeColor = interpStrokeColor(ColorInd());

    setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

    device->setPen  (pen);
    device->setBrush(brush);

    CQChartsDrawUtil::drawRoundedPolygon(device, poly, cornerSize(), cornerSize());
  }
}

#if 0
void
CQChartsBoxObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  double cxs = lengthPixelWidth (cornerSize());
  double cys = lengthPixelHeight(cornerSize());

  if (isFilled()) {
    QBrush brush;
    QPen   pen(Qt::NoPen);

    QColor bgColor = interpFillColor(ColorInd());

    setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

    painter->setPen  (pen);
    painter->setBrush(brush);

    CQChartsDrawUtil::drawRoundedPolygon(painter, poly, cxs, cys);
  }

  if (isStroked()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor strokeColor = interpStrokeColor(ColorInd());

    setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

    painter->setPen  (pen);
    painter->setBrush(brush);

    CQChartsDrawUtil::drawRoundedPolygon(painter, poly, cxs, cys);
  }
}
#endif
