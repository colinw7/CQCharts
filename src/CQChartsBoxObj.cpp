#include <CQChartsBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsBoxObj::
CQChartsBoxObj(View *view) :
 CQChartsViewPlotObj(view), CQChartsObjBoxData<CQChartsBoxObj>(this)
{
}

CQChartsBoxObj::
CQChartsBoxObj(Plot *plot) :
 CQChartsViewPlotObj(plot), CQChartsObjBoxData<CQChartsBoxObj>(this)
{
}

void
CQChartsBoxObj::
addProperties(PropertyModel *model, const QString &path, const QString &desc)
{
  addBoxProperties(model, path, desc);
}

void
CQChartsBoxObj::
addBoxProperties(PropertyModel *model, const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden=false) {
    auto *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addProp(path, "margin" , "", desc.length() ? desc + " outer margin" : "Outer margin");
  addProp(path, "padding", "", desc.length() ? desc + " inner padding" : "Inner padding");

  auto bgPath = path + "/fill";

  auto fillDesc = (desc.length() ? desc + " fill" : "Fill");

  addStyleProp(bgPath, "filled"     , "visible", fillDesc + " visible");
  addStyleProp(bgPath, "fillColor"  , "color"  , fillDesc + " color");
  addStyleProp(bgPath, "fillAlpha"  , "alpha"  , fillDesc + " alpha");
  addStyleProp(bgPath, "fillPattern", "pattern", fillDesc + " pattern");

  auto strokePath = path + "/stroke";

  auto strokeDesc = (desc.length() ? desc + " stroke" : "Stroke");

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
draw(PaintDevice *device, const BBox &bbox) const
{
  // set pen and brush
  PenBrush penBrush;

  auto bgColor     = interpFillColor  (ColorInd());
  auto strokeColor = interpStrokeColor(ColorInd());

  setPenBrush(penBrush,
    PenData(true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    BrushData(true, bgColor, fillAlpha(), fillPattern()));

  if (isStateColoring())
    updatePenBrushState(penBrush);

  draw(device, bbox, penBrush);
}

void
CQChartsBoxObj::
draw(PaintDevice *device, const BBox &bbox, const PenBrush &penBrush) const
{
  bbox_ = bbox;

  //---

  if (isFilled()) {
    // set pen and brush
    auto penBrush1 = penBrush;

    penBrush1.pen = QPen(Qt::NoPen);

    CQChartsDrawUtil::setPenBrush(device, penBrush1);

    //---

    // fill box
    CQChartsDrawUtil::drawRoundedPolygon(device, bbox, cornerSize(), borderSides());
  }

  if (isStroked()) {
    // set pen and brush
    auto penBrush1 = penBrush;

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
draw(PaintDevice *device, const Polygon &poly) const
{
  bbox_ = poly.boundingBox();

  if (isFilled()) {
    PenBrush penBrush;

    auto bgColor = interpFillColor(ColorInd());

    setPenBrush(penBrush,
      PenData(false, QColor(), CQChartsAlpha(), CQChartsLength(), CQChartsLineDash()),
      BrushData(true , bgColor, fillAlpha(), fillPattern()));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsDrawUtil::drawRoundedPolygon(device, poly, cornerSize());
  }

  if (isStroked()) {
    PenBrush penBrush;

    auto strokeColor = interpStrokeColor(ColorInd());

    setPenBrush(penBrush,
      PenData(true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
      BrushData(false, QColor(), CQChartsAlpha(), CQChartsFillPattern()));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsDrawUtil::drawRoundedPolygon(device, poly, cornerSize());
  }
}

//---

bool
CQChartsBoxObj::
contains(const Point &p) const
{
  return bbox_.inside(p);
}
