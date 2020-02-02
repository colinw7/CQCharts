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
  addBoxProperties(model, path, desc);
}

void
CQChartsBoxObj::
addBoxProperties(CQPropertyViewModel *model, const QString &path, const QString &desc)
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
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox) const
{
  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  setPenBrush(penBrush,
    true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash(),
    true, bgColor, fillAlpha(), fillPattern());

  if (isStateColoring())
    updatePenBrushState(penBrush);

  draw(device, bbox, penBrush);
}

void
CQChartsBoxObj::
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
     const CQChartsPenBrush &penBrush) const
{
  bbox_ = bbox;

  //---

  if (isFilled()) {
    // set pen and brush
    CQChartsPenBrush penBrush1 = penBrush;

    penBrush1.pen = QPen(Qt::NoPen);

    CQChartsDrawUtil::setPenBrush(device, penBrush1);

    //---

    // fill box
    CQChartsDrawUtil::drawRoundedPolygon(device, bbox, cornerSize(), borderSides());
  }

  if (isStroked()) {
    // set pen and brush
    CQChartsPenBrush penBrush1 = penBrush;

    penBrush1.brush = QBrush(Qt::NoBrush);

    CQChartsDrawUtil::setPenBrush(device, penBrush1);

    //---

    // stroke box
    CQChartsDrawUtil::drawRoundedPolygon(device, bbox, cornerSize(), borderSides());
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
draw(CQChartsPaintDevice *device, const CQChartsGeom::Polygon &poly) const
{
  if (isFilled()) {
    CQChartsPenBrush penBrush;

    QColor bgColor = interpFillColor(ColorInd());

    setPenBrush(penBrush,
      false, QColor(), CQChartsAlpha(), CQChartsLength(), CQChartsLineDash(),
      true , bgColor, fillAlpha(), fillPattern());

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsDrawUtil::drawRoundedPolygon(device, poly, cornerSize());
  }

  if (isStroked()) {
    CQChartsPenBrush penBrush;

    QColor strokeColor = interpStrokeColor(ColorInd());

    setPenBrush(penBrush,
      true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash(),
      false, QColor(), CQChartsAlpha(), CQChartsFillPattern());

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsDrawUtil::drawRoundedPolygon(device, poly, cornerSize());
  }
}

//---

bool
CQChartsBoxObj::
contains(const CQChartsGeom::Point &p) const
{
  return bbox_.inside(p);
}
