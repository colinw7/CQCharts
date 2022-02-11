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
setBBox(const BBox &b)
{
  bbox_ = b;
  rect_ = bbox_;
}

CQChartsGeom::Margin
CQChartsBoxObj::
parentMargin(const Margin &m) const
{
  return CQChartsGeom::Margin(lengthParentWidth (m.left  ()),
                              lengthParentHeight(m.top   ()),
                              lengthParentWidth (m.right ()),
                              lengthParentHeight(m.bottom()));
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

  if (! hasCustomMargin())
    addProp(path, "margin", "", desc.length() ? desc + " outer margin" : "Outer margin");

  if (! hasCustomPadding())
    addProp(path, "padding", "", desc.length() ? desc + " inner padding" : "Inner padding");

  //---

  auto bgPath = path + "/fill";

  auto fillDesc = (desc.length() ? desc + " fill" : "Fill");

  addStyleProp(bgPath, "filled"     , "visible", fillDesc + " visible");
  addStyleProp(bgPath, "fillColor"  , "color"  , fillDesc + " color");
  addStyleProp(bgPath, "fillAlpha"  , "alpha"  , fillDesc + " alpha");
  addStyleProp(bgPath, "fillPattern", "pattern", fillDesc + " pattern");

  //---

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

  calcPenBrush(penBrush);

  draw(device, bbox, penBrush);
}

void
CQChartsBoxObj::
draw(PaintDevice *device, const BBox &bbox, const PenBrush &penBrush) const
{
  drawBox(device, bbox, penBrush, cornerSize(), borderSides());

  const_cast<CQChartsBoxObj *>(this)->setBBox(bbox);
}

void
CQChartsBoxObj::
draw(PaintDevice *device, const Polygon &poly) const
{
  // set pen and brush
  PenBrush penBrush;

  calcPenBrush(penBrush);

  draw(device, poly, penBrush);
}

void
CQChartsBoxObj::
draw(PaintDevice *device, const Polygon &poly, const PenBrush &penBrush) const
{
  drawPolygon(device, poly, penBrush, cornerSize());

  const_cast<CQChartsBoxObj *>(this)->setBBox(poly.boundingBox());
}

void
CQChartsBoxObj::
drawBox(PaintDevice *device, const BBox &bbox, const PenBrush &penBrush,
        const Length &cornerSize, const Sides &borderSides)
{
  if (penBrush.brush.style() != Qt::NoBrush) {
    // set pen and brush
    auto penBrush1 = penBrush;

    penBrush1.pen = QPen(Qt::NoPen);

    // fill box
    CQChartsDrawUtil::drawRoundedRect(device, penBrush1, bbox, cornerSize, borderSides);
  }

  if (penBrush.pen.style() != Qt::NoPen) {
    // set pen and brush
    auto penBrush1 = penBrush;

    penBrush1.brush = QBrush(Qt::NoBrush);

    // stroke box
    CQChartsDrawUtil::drawRoundedRect(device, penBrush1, bbox, cornerSize, borderSides);
  }
}

void
CQChartsBoxObj::
drawPolygon(PaintDevice *device, const Polygon &poly, const PenBrush &penBrush,
            const Length &cornerSize)
{
  if (penBrush.brush.style() != Qt::NoBrush) {
    // set pen and brush
    auto penBrush1 = penBrush;

    penBrush1.pen = QPen(Qt::NoPen);

    // fill polygon
    CQChartsDrawUtil::drawRoundedPolygon(device, penBrush1, poly, cornerSize);
  }

  if (penBrush.pen.style() != Qt::NoPen) {
    // set pen and brush
    auto penBrush1 = penBrush;

    penBrush1.brush = QBrush(Qt::NoBrush);

    // stroke polygon
    CQChartsDrawUtil::drawRoundedPolygon(device, penBrush1, poly, cornerSize);
  }
}

void
CQChartsBoxObj::
calcPenBrush(PenBrush &penBrush) const
{
  auto bgColor     = interpFillColor  (ColorInd());
  auto strokeColor = interpStrokeColor(ColorInd());

  setPenBrush(penBrush,
    PenData(isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    BrushData(isFilled(), bgColor, fillAlpha(), fillPattern()));

  if (isStateColoring())
    updatePenBrushState(penBrush);
}

void
CQChartsBoxObj::
boxObjInvalidate()
{
  if      (plot())
    plot()->drawObjs();
  else if (view())
    view()->doUpdate();
}

//---

bool
CQChartsBoxObj::
contains(const Point &p) const
{
  return bbox().inside(p);
}
