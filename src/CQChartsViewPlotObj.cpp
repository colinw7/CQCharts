#include <CQChartsViewPlotObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAnnotation.h>
#include <CQChartsObjRef.h>
#include <CQChartsEditHandles.h>

CQChartsViewPlotObj::
CQChartsViewPlotObj(CQChartsView *view) :
 CQChartsObj(view->charts()), view_(view)
{
}

CQChartsViewPlotObj::
CQChartsViewPlotObj(CQChartsPlot *plot) :
 CQChartsObj(plot->charts()), plot_(plot)
{
}

CQChartsViewPlotObj::
~CQChartsViewPlotObj()
{
  delete editHandles_;
}

//---

CQCharts *
CQChartsViewPlotObj::
charts() const
{
  return view()->charts();
}

CQChartsView *
CQChartsViewPlotObj::
view() const
{
  return (plot_ ? plot_->view() : view_);
}

//---

CQChartsEditHandles *
CQChartsViewPlotObj::
editHandles() const
{
  if (! editHandles_) {
    auto *th = const_cast<CQChartsViewPlotObj *>(this);

    if      (plot())
      th->editHandles_ = new CQChartsEditHandles(plot(), CQChartsEditHandles::Mode::MOVE);
    else if (view())
      th->editHandles_ = new CQChartsEditHandles(view(), CQChartsEditHandles::Mode::MOVE);
  }

  return editHandles_;
}

void
CQChartsViewPlotObj::
drawEditHandles(QPainter *painter) const
{
  assert(view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  auto *th = const_cast<CQChartsViewPlotObj *>(this);

  BBox rect = this->rect();

  if (! rect.isValid())
    return;

  th->editHandles()->setBBox(rect);

  editHandles()->draw(painter);
}

//---

void
CQChartsViewPlotObj::
setPenBrush(CQChartsPenBrush &penBrush, const CQChartsPenData &penData,
            const CQChartsBrushData &brushData) const
{
  if      (plot())
    plot()->setPenBrush(penBrush, penData, brushData);
  else if (view())
    view()->setPenBrush(penBrush, penData, brushData);
}

void
CQChartsViewPlotObj::
setPen(PenBrush &penBrush, const PenData &penData) const
{
  if      (plot())
    plot()->setPen(penBrush, penData);
  else if (view())
    view()->setPen(penBrush, penData);
}

#if 0
void
CQChartsViewPlotObj::
setPenBrush(CQChartsPenBrush &penBrush,
            bool stroked, const QColor &strokeColor, const CQChartsAlpha &strokeAlpha,
            const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash,
            bool filled, const QColor &fillColor, const CQChartsAlpha &fillAlpha,
            const CQChartsFillPattern &pattern) const
{
  if      (plot())
    plot()->setPenBrush(penBrush,
      CQChartsPenData  (stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash),
      CQChartsBrushData(filled, fillColor, fillAlpha, pattern));
  else if (view())
    view()->setPenBrush(penBrush,
      CQChartsPenData  (stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash),
      CQChartsBrushData(filled, fillColor, fillAlpha, pattern));
}

void
CQChartsViewPlotObj::
setPen(QPen &pen, bool stroked, const QColor &strokeColor, const CQChartsAlpha &strokeAlpha,
       const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash) const
{
  if      (plot())
    plot()->setPen(pen, stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash);
  else if (view())
    view()->setPen(pen, stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash);
}

void
CQChartsViewPlotObj::
setBrush(QBrush &brush, bool filled, const QColor &fillColor, const CQChartsAlpha &fillAlpha,
         const CQChartsFillPattern &pattern) const
{
  if      (plot())
    plot()->setBrush(brush, filled, fillColor, fillAlpha, pattern);
  else if (view())
    view()->setBrush(brush, filled, fillColor, fillAlpha, pattern);
}
#endif

void
CQChartsViewPlotObj::
updatePenBrushState(CQChartsPenBrush &penBrush, DrawType drawType) const
{
  if      (plot())
    plot()->updateObjPenBrushState(this, penBrush, drawType);
  else if (view())
    view()->updateObjPenBrushState(this, penBrush, drawType);
}

//---

QFont
CQChartsViewPlotObj::
calcFont(const CQChartsFont &font) const
{
  QFont font1;

  if      (plot())
    font1 = view()->plotFont(plot(), font);
  else if (view())
    font1 = view()->viewFont(font);

  return font1;
}

void
CQChartsViewPlotObj::
setPainterFont(CQChartsPaintDevice *device, const CQChartsFont &font) const
{
  if      (plot())
    view()->setPlotPainterFont(plot(), device, font);
  else if (view())
    view()->setPainterFont(device, font);
}

#if 0
void
CQChartsViewPlotObj::
setPainterFont(QPainter *painter, const CQChartsFont &font) const
{
  if      (plot())
    view()->setPlotPainterFont(plot(), painter, font);
  else if (view())
    view()->setPainterFont(painter, font);
}
#endif

void
CQChartsViewPlotObj::
adjustTextOptions(CQChartsTextOptions &textOptions) const
{
  if (plot())
    textOptions = plot()->adjustTextOptions(textOptions);
}

//---

CQChartsGeom::Point
CQChartsViewPlotObj::
positionToParent(const ObjRef &objRef, const Position &pos) const
{
  BBox         bbox;
  CQChartsObj *obj = nullptr;

  if (! objectRect(objRef, obj, bbox))
    return positionToParent(pos);

  CQChartsLength xlen(pos.p().x, pos.units());
  CQChartsLength ylen(pos.p().y, pos.units());

  auto x = lengthParentWidth (xlen);
  auto y = lengthParentHeight(ylen);

  Point p(x, y);

  if      (objRef.location() == ObjRef::Location::LEFT)
    p += bbox.getMidL();
  else if (objRef.location() == ObjRef::Location::RIGHT)
    p += bbox.getMidR();
  else if (objRef.location() == ObjRef::Location::BOTTOM)
    p += bbox.getMidB();
  else if (objRef.location() == ObjRef::Location::TOP)
    p += bbox.getMidT();
  else if (objRef.location() == ObjRef::Location::INTERSECT)
    p = bbox.getCenter();
  else
    p += bbox.getCenter();

  return p;
}

CQChartsGeom::Point
CQChartsViewPlotObj::
intersectObjRef(const ObjRef &objRef, const Point &p1, const Point &p2) const
{
  BBox         bbox;
  CQChartsObj *obj = nullptr;

  if (! objectRect(objRef, obj, bbox))
    return p1;

  Point pi;

  if (! obj->intersectShape(p1, p2, pi))
    return p1;

  return pi;
}

CQChartsGeom::Point
CQChartsViewPlotObj::
positionToPixel(const ObjRef &objRef, const Position &pos) const
{
  auto p = positionToParent(objRef, pos);

  return windowToPixel(p);
}

bool
CQChartsViewPlotObj::
objectRect(const ObjRef &objRef, CQChartsObj* &obj, BBox &bbox) const
{
  obj = nullptr;

  if (! objRef.isValid())
    return false;

  if (! plot())
    return false;

  auto *plotObj = plot()->getPlotObject(objRef.name());

  if (plotObj) {
    obj  = plotObj;
    bbox = plotObj->rect();
  }
  else {
    auto *annotation = plot()->getAnnotationByPathId(objRef.name());

    if (annotation) {
      obj  = annotation;
      bbox = annotation->rect();
    }
  }

  return (obj != nullptr);
}

CQChartsGeom::Point
CQChartsViewPlotObj::
positionToParent(const Position &pos) const
{
  Point p;

  if      (plot())
    p = plot()->positionToPlot(pos);
  else if (view())
    p = view()->positionToView(pos);

  return p;
}

CQChartsGeom::Point
CQChartsViewPlotObj::
positionToPixel(const Position &pos) const
{
  Point p;

  if      (plot())
    p = plot()->positionToPixel(pos);
  else if (view())
    p = view()->positionToPixel(pos);

  return p;
}

double
CQChartsViewPlotObj::
lengthParentWidth(const CQChartsLength &len) const
{
  double w = 1.0;

  if      (plot())
    w = plot()->lengthPlotWidth(len);
  else if (view())
    w = view()->lengthViewWidth(len);

  return w;
}

double
CQChartsViewPlotObj::
lengthParentHeight(const CQChartsLength &len) const
{
  double h = 1.0;

  if      (plot())
    h = plot()->lengthPlotHeight(len);
  else if (view())
    h = view()->lengthViewHeight(len);

  return h;
}

double
CQChartsViewPlotObj::
lengthPixelWidth(const CQChartsLength &len) const
{
  double w = 1.0;

  if      (plot())
    w = plot()->lengthPixelWidth(len);
  else if (view())
    w = view()->lengthPixelWidth(len);

  return w;
}

double
CQChartsViewPlotObj::
lengthPixelHeight(const CQChartsLength &len) const
{
  double h = 1.0;

  if      (plot())
    h = plot()->lengthPixelHeight(len);
  else if (view())
    h = view()->lengthPixelHeight(len);

  return h;
}

CQChartsGeom::Point
CQChartsViewPlotObj::
windowToPixel(const Point &w) const
{
  Point p;

  if      (plot())
    p = plot()->windowToPixel(w);
  else if (view())
    p = view()->windowToPixel(w);

  return p;
}

CQChartsGeom::BBox
CQChartsViewPlotObj::
windowToPixel(const BBox &w) const
{
  BBox p;

  if      (plot())
    p = plot()->windowToPixel(w);
  else if (view())
    p = view()->windowToPixel(w);

  return p;
}

CQChartsGeom::Point
CQChartsViewPlotObj::
pixelToWindow(const Point &w) const
{
  Point p;

  if      (plot())
    p = plot()->pixelToWindow(w);
  else if (view())
    p = view()->pixelToWindow(w);

  return p;
}

CQChartsGeom::BBox
CQChartsViewPlotObj::
pixelToWindow(const BBox &w) const
{
  BBox p;

  if      (plot())
    p = plot()->pixelToWindow(w);
  else if (view())
    p = view()->pixelToWindow(w);

  return p;
}

double
CQChartsViewPlotObj::
pixelToWindowWidth(double pw) const
{
  double w = 0.0;

  if      (plot())
    w = plot()->pixelToWindowWidth(pw);
  else if (view())
    w = view()->pixelToWindowWidth(pw);

  return w;
}

double
CQChartsViewPlotObj::
pixelToWindowHeight(double ph) const
{
  double h = 0.0;

  if      (plot())
    h = plot()->pixelToWindowHeight(ph);
  else if (view())
    h = view()->pixelToWindowHeight(ph);

  return h;
}

QColor
CQChartsViewPlotObj::
backgroundColor() const
{
  CQChartsUtil::ColorInd ci;

  QColor bg;

  if      (plot())
    bg = plot()->view()->interpBackgroundFillColor(ci);
  else if (view())
    bg = view()->interpBackgroundFillColor(ci);

  return bg;
}

//---

CQChartsLength
CQChartsViewPlotObj::
makeLength(CQChartsView *view, CQChartsPlot *plot, double len)
{
  assert(view || plot);

  if      (view)
    return CQChartsLength(len, CQChartsUnits::VIEW);
  else if (plot)
    return CQChartsLength(len, CQChartsUnits::PLOT);
  else
    return CQChartsLength();
}

CQChartsPosition
CQChartsViewPlotObj::
makePosition(CQChartsView *view, CQChartsPlot *plot, double x, double y)
{
  assert(view || plot);

  if      (view)
    return Position(Point(x, y), CQChartsUnits::VIEW);
  else if (plot)
    return Position(Point(x, y), CQChartsUnits::PLOT);
  else
    return Position();
}

CQChartsRect
CQChartsViewPlotObj::
makeRect(CQChartsView *view, CQChartsPlot *plot, double x1, double y1, double x2, double y2)
{
  assert(view || plot);

  if      (view)
    return CQChartsRect(BBox(x1, y1, x2, y2), CQChartsUnits::VIEW);
  else if (plot)
    return CQChartsRect(BBox(x1, y1, x2, y2), CQChartsUnits::PLOT);
  else
    return CQChartsRect();
}

CQChartsRect
CQChartsViewPlotObj::
makeRect(CQChartsView *view, CQChartsPlot *plot, const Position &start, const Position &end)
{
  assert(view || plot);

  CQChartsRect rectangle;

  // if different units then convert rectangle
  if (start.units() != end.units()) {
    Point pstart, pend;

    if      (plot) {
      pstart = plot->positionToPlot(start);
      pend   = plot->positionToPlot(end);
    }
    else if (view) {
      pstart = view->positionToView(start);
      pend   = view->positionToView(end);
    }

    BBox bbox(pstart, pend);

    if      (plot)
      rectangle = CQChartsRect(bbox, CQChartsUnits::PLOT);
    else if (view)
      rectangle = CQChartsRect(bbox, CQChartsUnits::VIEW);
  }
  else {
    rectangle = CQChartsRect(BBox(start.p(), end.p()), start.units());
  }

  return rectangle;
}
