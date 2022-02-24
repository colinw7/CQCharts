#include <CQChartsViewPlotObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAnnotation.h>
#include <CQChartsObjRef.h>
#include <CQChartsEditHandles.h>

CQChartsViewPlotObj::
CQChartsViewPlotObj(View *view) :
 CQChartsObj(view->charts()), view_(view)
{
}

CQChartsViewPlotObj::
CQChartsViewPlotObj(Plot *plot) :
 CQChartsObj(plot->charts()), plot_(plot)
{
}

CQChartsViewPlotObj::
~CQChartsViewPlotObj()
{
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
      th->editHandles_ = std::make_unique<EditHandles>(plot(), EditHandles::Mode::MOVE);
    else if (view())
      th->editHandles_ = std::make_unique<EditHandles>(view(), EditHandles::Mode::MOVE);
  }

  return editHandles_.get();
}

void
CQChartsViewPlotObj::
drawEditHandles(PaintDevice *device) const
{
  assert(view()->mode() == View::Mode::EDIT && isSelected());

  if (! isVisible())
    return;

  setEditHandlesBBox();

  editHandles()->draw(device);
}

void
CQChartsViewPlotObj::
setEditHandlesBBox() const
{
  auto rect = this->rect();
  if (! rect.isValid()) return;

  auto *th = const_cast<CQChartsViewPlotObj *>(this);

  th->editHandles()->setBBox(rect);
}

//---

void
CQChartsViewPlotObj::
setPenBrush(PenBrush &penBrush, const PenData &penData, const BrushData &brushData) const
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
setPenBrush(PenBrush &penBrush,
            bool stroked, const QColor &strokeColor, const Alpha &strokeAlpha,
            const Length &strokeWidth, const LineDash &strokeDash,
            bool filled, const QColor &fillColor, const Alpha &fillAlpha,
            const FillPattern &pattern) const
{
  if      (plot())
    plot()->setPenBrush(penBrush,
      PenData  (stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash),
      BrushData(filled, fillColor, fillAlpha, pattern));
  else if (view())
    view()->setPenBrush(penBrush,
      PenData  (stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash),
      BrushData(filled, fillColor, fillAlpha, pattern));
}

void
CQChartsViewPlotObj::
setPen(QPen &pen, bool stroked, const QColor &strokeColor, const Alpha &strokeAlpha,
       const Length &strokeWidth, const LineDash &strokeDash) const
{
  if      (plot())
    plot()->setPen(pen, stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash);
  else if (view())
    view()->setPen(pen, stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash);
}

void
CQChartsViewPlotObj::
setBrush(QBrush &brush, bool filled, const QColor &fillColor, const Alpha &fillAlpha,
         const FillPattern &pattern) const
{
  if      (plot())
    plot()->setBrush(brush, filled, fillColor, fillAlpha, pattern);
  else if (view())
    view()->setBrush(brush, filled, fillColor, fillAlpha, pattern);
}
#endif

void
CQChartsViewPlotObj::
updatePenBrushState(PenBrush &penBrush, DrawType drawType) const
{
  if      (plot())
    plot()->updateObjPenBrushState(this, penBrush, drawType);
  else if (view())
    view()->updateObjPenBrushState(this, penBrush, drawType);
}

//---

QFont
CQChartsViewPlotObj::
calcFont(const Font &font) const
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
setPainterFont(PaintDevice *device, const Font &font) const
{
  if      (plot())
    plot()->setPainterFont(device, font);
  else if (view())
    view()->setPainterFont(device, font);
}

void
CQChartsViewPlotObj::
adjustTextOptions(TextOptions &textOptions) const
{
  if (plot())
    textOptions = plot()->adjustTextOptions(textOptions);
}

//---

CQChartsGeom::Point
CQChartsViewPlotObj::
positionToParent(const ObjRef &objRef, const Position &pos) const
{
  assert(pos.isValid());

  BBox         bbox;
  CQChartsObj *obj = nullptr;

  if (! objectRect(objRef, obj, bbox))
    return positionToParent(pos);

  Length xlen(pos.p().x, pos.units());
  Length ylen(pos.p().y, pos.units());

  auto x = lengthParentSignedWidth (xlen);
  auto y = lengthParentSignedHeight(ylen);

  Point p(x, y);

  if      (objRef.location() == ObjRef::Location::LEFT     ) p += bbox.getMidL();
  else if (objRef.location() == ObjRef::Location::RIGHT    ) p += bbox.getMidR();
  else if (objRef.location() == ObjRef::Location::BOTTOM   ) p += bbox.getMidB();
  else if (objRef.location() == ObjRef::Location::TOP      ) p += bbox.getMidT();
  else if (objRef.location() == ObjRef::Location::LL       ) p += bbox.getLL();
  else if (objRef.location() == ObjRef::Location::LR       ) p += bbox.getLR();
  else if (objRef.location() == ObjRef::Location::UL       ) p += bbox.getUL();
  else if (objRef.location() == ObjRef::Location::UR       ) p += bbox.getUR();
  else if (objRef.location() == ObjRef::Location::CENTER   ) p += bbox.getCenter();
  else if (objRef.location() == ObjRef::Location::INTERSECT) p += bbox.getCenter();

  return p;
}

CQChartsPosition
CQChartsViewPlotObj::
positionFromParent(const ObjRef &objRef, const Position &pos) const
{
  assert(pos.isValid());

  BBox         bbox;
  CQChartsObj *obj = nullptr;

  if (! objectRect(objRef, obj, bbox))
    return pos;

  Length xlen(pos.p().x, pos.units());
  Length ylen(pos.p().y, pos.units());

  auto x = lengthParentSignedWidth (xlen);
  auto y = lengthParentSignedHeight(ylen);

  Point p(x, y);

  if      (objRef.location() == ObjRef::Location::LEFT     ) p -= bbox.getMidL();
  else if (objRef.location() == ObjRef::Location::RIGHT    ) p -= bbox.getMidR();
  else if (objRef.location() == ObjRef::Location::BOTTOM   ) p -= bbox.getMidB();
  else if (objRef.location() == ObjRef::Location::TOP      ) p -= bbox.getMidT();
  if      (objRef.location() == ObjRef::Location::LL       ) p -= bbox.getLL();
  else if (objRef.location() == ObjRef::Location::LR       ) p -= bbox.getLR();
  else if (objRef.location() == ObjRef::Location::UL       ) p -= bbox.getUL();
  else if (objRef.location() == ObjRef::Location::UR       ) p -= bbox.getUR();
  else if (objRef.location() == ObjRef::Location::CENTER   ) p -= bbox.getCenter();
  else if (objRef.location() == ObjRef::Location::INTERSECT) p -= bbox.getCenter();

  if      (plot())
    return CQChartsPosition::plot(p);
  else if (view())
    return CQChartsPosition::view(p);
  else
    return CQChartsPosition::plot(p);
}

CQChartsGeom::Point
CQChartsViewPlotObj::
positionToPixel(const ObjRef &objRef, const Position &pos) const
{
  auto p = positionToParent(objRef, pos);

  return windowToPixel(p);
}

//---

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
lengthParentWidth(const Length &len) const
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
lengthParentHeight(const Length &len) const
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
lengthParentSignedWidth(const Length &len) const
{
  double w = 1.0;

  if      (plot())
    w = plot()->lengthPlotSignedWidth(len);
  else if (view())
    w = view()->lengthViewSignedWidth(len);

  return w;
}

double
CQChartsViewPlotObj::
lengthParentSignedHeight(const Length &len) const
{
  double h = 1.0;

  if      (plot())
    h = plot()->lengthPlotSignedHeight(len);
  else if (view())
    h = view()->lengthViewSignedHeight(len);

  return h;
}

double
CQChartsViewPlotObj::
lengthWindowWidth(const Length &len) const
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
lengthPixelWidth(const Length &len) const
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
lengthPixelHeight(const Length &len) const
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

double
CQChartsViewPlotObj::
windowToPixelWidth(double w) const
{
  double pw = 0.0;

  if      (plot())
    pw = plot()->windowToPixelWidth(w);
  else if (view())
    pw = view()->windowToPixelWidth(w);

  return pw;
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
makeLength(View *view, Plot *plot, double len)
{
  assert(view || plot);

  if      (view)
    return Length::view(len);
  else if (plot)
    return Length::plot(len);
  else
    return Length();
}

CQChartsPosition
CQChartsViewPlotObj::
makePosition(View *view, Plot *plot, double x, double y)
{
  assert(view || plot);

  if      (view)
    return Position::view(Point(x, y));
  else if (plot)
    return Position::plot(Point(x, y));
  else
    return Position();
}

CQChartsRect
CQChartsViewPlotObj::
makeRect(View *view, Plot *plot, double x1, double y1, double x2, double y2)
{
  assert(view || plot);

  if      (view)
    return Rect::view(BBox(x1, y1, x2, y2));
  else if (plot)
    return Rect::plot(BBox(x1, y1, x2, y2));
  else
    return Rect();
}

CQChartsRect
CQChartsViewPlotObj::
makeRect(View *view, Plot *plot, const Position &start, const Position &end)
{
  assert(view || plot);

  Rect rectangle;

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
      rectangle = Rect::plot(bbox);
    else if (view)
      rectangle = Rect::view(bbox);
  }
  else {
    rectangle = Rect(BBox(start.p(), end.p()), start.units());
  }

  return rectangle;
}
