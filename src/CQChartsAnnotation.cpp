#include <CQChartsAnnotation.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsArrow.h>
#include <CQPropertyViewModel.h>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

CQChartsAnnotation::
CQChartsAnnotation(CQChartsView *view) :
 CQChartsTextBoxObj(view), editHandles_(view, CQChartsEditHandles::Mode::MOVE)
{
  static int s_lastInd;

  ind_ = ++s_lastInd;
}

CQChartsAnnotation::
CQChartsAnnotation(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot), editHandles_(plot, CQChartsEditHandles::Mode::MOVE)
{
  static int s_lastInd;

  ind_ = ++s_lastInd;
}

CQChartsAnnotation::
~CQChartsAnnotation()
{
}

QString
CQChartsAnnotation::
calcId() const
{
  if (id_ && id_->length())
    return *id_;

  return QString("annotation:%1").arg(ind_);
}

QString
CQChartsAnnotation::
calcTipId() const
{
  if (tipId_ && tipId_->length())
    return *tipId_;

  return calcId();
}

QString
CQChartsAnnotation::
pathId() const
{
  QString id = QString("%1").arg(ind_);

  if      (plot())
    return plot()->pathId() + ":" + id;
  else if (view())
    return view()->id() + ":" + id;
  else
    return id;
}

void
CQChartsAnnotation::
addProperties(CQPropertyViewModel *, const QString &)
{
}

void
CQChartsAnnotation::
addStrokeFillProperties(CQPropertyViewModel *model, const QString &path)
{
  addStrokeProperties(model, path);
  addFillProperties  (model, path);
}

void
CQChartsAnnotation::
addFillProperties(CQPropertyViewModel *model, const QString &path)
{
  QString bgPath = path + "/fill";

  model->addProperty(bgPath, this, "background"       , "visible");
  model->addProperty(bgPath, this, "backgroundColor"  , "color"  );
  model->addProperty(bgPath, this, "backgroundAlpha"  , "alpha"  );
  model->addProperty(bgPath, this, "backgroundPattern", "pattern");
}

void
CQChartsAnnotation::
addStrokeProperties(CQPropertyViewModel *model, const QString &path)
{
  QString borderPath = path + "/border";

  model->addProperty(borderPath, this, "border"     , "visible"   );
  model->addProperty(borderPath, this, "borderColor", "color"     );
  model->addProperty(borderPath, this, "borderAlpha", "alpha"     );
  model->addProperty(borderPath, this, "borderWidth", "width"     );
  model->addProperty(borderPath, this, "cornerSize" , "cornerSize");
  model->addProperty(borderPath, this, "borderSides", "sides"     );
}

bool
CQChartsAnnotation::
getProperty(const QString &name, QVariant &value)
{
  if (view())
    return view()->propertyModel()->getProperty(this, name, value);

  return false;
}

bool
CQChartsAnnotation::
setProperty(const QString &name, const QVariant &value)
{
  if (view())
    return view()->propertyModel()->setProperty(this, name, value);

  return false;
}

bool
CQChartsAnnotation::
contains(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  return inside(p);
}

bool
CQChartsAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return bbox().inside(p);
}

//------

bool
CQChartsAnnotation::
selectPress(const CQChartsGeom::Point &)
{
  return id().length();
}

//------

bool
CQChartsAnnotation::
editPress(const CQChartsGeom::Point &p)
{
  editHandles_.setDragPos(p);

  return true;
}

bool
CQChartsAnnotation::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point        &dragPos  = editHandles_.dragPos();
  const CQChartsResizeHandle::Side &dragSide = editHandles_.dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles_.updateBBox(dx, dy);

  if (dragSide != CQChartsResizeHandle::Side::MOVE)
    autoSize_ = false;

  setBBox(editHandles_.bbox(), dragSide);

  editHandles_.setDragPos(p);

  if      (plot())
    plot()->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
  else if (view())
    view()->update();

  return true;
}

bool
CQChartsAnnotation::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_.selectInside(p);
}

bool
CQChartsAnnotation::
editRelease(const CQChartsGeom::Point &)
{
  return true;
}

void
CQChartsAnnotation::
editMoveBy(const QPointF &f)
{
  editHandles_.setDragSide(CQChartsResizeHandle::Side::MOVE);

  editHandles_.updateBBox(f.x(), f.y());

  setBBox(editHandles_.bbox(), CQChartsResizeHandle::Side::MOVE);

  if      (plot())
    plot()->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
  else if (view())
    view()->update();
}

//------

void
CQChartsAnnotation::
draw(QPainter *painter)
{
  // draw edit handles for view (TODO: used ?)
  if (! plot() && view()->mode() == CQChartsView::Mode::EDIT && isSelected())
    drawEditHandles(painter);
}

void
CQChartsAnnotation::
drawEditHandles(QPainter *painter)
{
  assert(view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  editHandles_.setBBox(this->bbox());

  editHandles_.draw(painter);
}

//---

CQChartsView *
CQChartsAnnotation::
view() const
{
  return (plot() ? plot()->view() : CQChartsTextBoxObj::view());
}

//---

CQChartsRectAnnotation::
CQChartsRectAnnotation(CQChartsView *view, const CQChartsPosition &start,
                       const CQChartsPosition &end) :
 CQChartsAnnotation(view), start_(start), end_(end)
{
  setObjectName(QString("rect.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsRectAnnotation::
CQChartsRectAnnotation(CQChartsPlot *plot, const CQChartsPosition &start,
                       const CQChartsPosition &end) :
 CQChartsAnnotation(plot), start_(start), end_(end)
{
  setObjectName(QString("rect.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsRectAnnotation::
~CQChartsRectAnnotation()
{
}

void
CQChartsRectAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  model->addProperty(path1, this, "start");
  model->addProperty(path1, this, "end"  );

  model->addProperty(path1, this, "margin");
  model->addProperty(path1, this, "padding");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsRectAnnotation::
propertyId() const
{
  return QString("rectAnnotation.%1").arg(ind());
}

void
CQChartsRectAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  QPointF start, end;

  if      (plot()) {
    start = plot()->positionToPlot(start_);
    end   = plot()->positionToPlot(end_  );
  }
  else if (view()) {
    start = view()->positionToView(start_);
    end   = view()->positionToView(end_  );
  }

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMax(), y2 = bbox.getYMax();

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot()) {
    xp = plot()->pixelToWindowWidth (padding());
    yp = plot()->pixelToWindowHeight(padding());
    xm = plot()->pixelToWindowWidth (margin ());
    ym = plot()->pixelToWindowHeight(margin ());
  }
  else if (view()) {
    xp = view()->pixelToWindowWidth (padding());
    yp = view()->pixelToWindowHeight(padding());
    xm = view()->pixelToWindowWidth (margin ());
    ym = view()->pixelToWindowHeight(margin ());
  }

  x1 += xp + xm; y1 += yp + ym;
  x2 -= xp + xm; y2 -= yp + ym;

  start = QPointF(std::min(x1, x2), std::min(y1, y2));
  end   = QPointF(std::max(x1, x2), std::max(y1, y2));

  start_ = CQChartsPosition(start);
  end_   = CQChartsPosition(end  );

  bbox_ = bbox;
}

void
CQChartsRectAnnotation::
draw(QPainter *painter)
{
  QPointF start, end;

  if      (plot()) {
    start = plot()->positionToPlot(start_);
    end   = plot()->positionToPlot(end_  );
  }
  else if (view()) {
    start = view()->positionToView(start_);
    end   = view()->positionToView(end_  );
  }

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot()) {
    xp = plot()->pixelToWindowWidth (padding());
    yp = plot()->pixelToWindowHeight(padding());
    xm = plot()->pixelToWindowWidth (margin ());
    ym = plot()->pixelToWindowHeight(margin ());
  }
  else if (view()) {
    xp = view()->pixelToWindowWidth (padding());
    yp = view()->pixelToWindowHeight(padding());
    xm = view()->pixelToWindowWidth (margin ());
    ym = view()->pixelToWindowHeight(margin ());
  }

  double x1 = std::min(start.x(), end.x());
  double y1 = std::min(start.y(), end.y());
  double x2 = std::max(start.x(), end.x());
  double y2 = std::max(start.y(), end.y());

  double x = x1 - xp - xm; // bottom
  double y = y1 - yp - ym; // top
  double w = (x2 - x1) + 2*xp + 2*xm;
  double h = (y2 - y1) + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  CQChartsGeom::BBox prect;

  if      (plot())
    plot()->windowToPixel(bbox_, prect);
  else if (view())
    prect = view()->windowToPixel(bbox_);

  CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsEllipseAnnotation::
CQChartsEllipseAnnotation(CQChartsView *view, const CQChartsPosition &center,
                          const CQChartsLength &xRadius, const CQChartsLength &yRadius) :
 CQChartsAnnotation(view), center_(center), xRadius_(xRadius), yRadius_(yRadius)
{
  setObjectName(QString("ellipse.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsEllipseAnnotation::
CQChartsEllipseAnnotation(CQChartsPlot *plot, const CQChartsPosition &center,
                          const CQChartsLength &xRadius, const CQChartsLength &yRadius) :
 CQChartsAnnotation(plot), center_(center), xRadius_(xRadius), yRadius_(yRadius)
{
  setObjectName(QString("ellipse.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsEllipseAnnotation::
~CQChartsEllipseAnnotation()
{
}

void
CQChartsEllipseAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  model->addProperty(path1, this, "center" );
  model->addProperty(path1, this, "xRadius");
  model->addProperty(path1, this, "yRadius");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsEllipseAnnotation::
propertyId() const
{
  return QString("ellipseAnnotation.%1").arg(ind());
}

void
CQChartsEllipseAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  center_ = CQChartsPosition(CQChartsUtil::toQPoint(bbox.getCenter()));

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  xRadius_ = w/2;
  yRadius_ = h/2;

  bbox_ = bbox;
}

bool
CQChartsEllipseAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  QPointF center;

  if      (plot())
    center = plot()->positionToPlot(center_);
  else if (view())
    center = view()->positionToView(center_);

  double dx = p.getX() - center.x();
  double dy = p.getY() - center.y();

  double xr = 0.0, yr = 0.0;

  if      (plot()) {
    xr = plot()->lengthPlotWidth (xRadius_);
    yr = plot()->lengthPlotHeight(yRadius_);
  }
  else if (view()) {
    xr = view()->lengthViewWidth (xRadius_);
    yr = view()->lengthViewHeight(yRadius_);
  }

  double xr2 = xr*xr;
  double yr2 = yr*yr;

  return (((dx*dx)/xr2 + (dy*dy)/yr2) < 1);
}

void
CQChartsEllipseAnnotation::
draw(QPainter *painter)
{
  QPointF center;

  if      (plot())
    center = plot()->positionToPlot(center_);
  else if (view())
    center = view()->positionToView(center_);

  double xr = 0.0, yr = 0.0;

  if      (plot()) {
    xr = plot()->lengthPlotWidth (xRadius_);
    yr = plot()->lengthPlotHeight(yRadius_);
  }
  else if (view()) {
    xr = view()->lengthViewWidth (xRadius_);
    yr = view()->lengthViewHeight(yRadius_);
  }

  double x1 = center.x() - xr;
  double y1 = center.y() - yr;
  double x2 = center.x() + xr;
  double y2 = center.y() + yr;

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  CQChartsGeom::BBox prect;

  if      (plot())
    plot()->windowToPixel(bbox_, prect);
  else if (view())
    prect = view()->windowToPixel(bbox_);

  //CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  // create path
  QPainterPath path;

  path.addEllipse(CQChartsUtil::toQRect(prect));

  //---

  QPen   pen;
  QBrush brush;

  QColor bgColor = interpBackgroundColor(0, 1);

  if      (plot())
    plot()->setBrush(brush, isBackground(), bgColor, backgroundAlpha(), backgroundPattern());
  else if (view())
    view()->setBrush(brush, isBackground(), bgColor, backgroundAlpha(), backgroundPattern());

  QColor borderColor = interpBorderColor(0, 1);

  if      (plot())
    plot()->setPen(pen, isBorder(), borderColor, borderAlpha(), borderWidth(), borderDash());
  else if (view())
    view()->setPen(pen, isBorder(), borderColor, borderAlpha(), borderWidth(), borderDash());

  if (plot())
    plot()->updateObjPenBrushState(this, pen, brush);

  //---

  painter->fillPath  (path, brush);
  painter->strokePath(path, pen  );

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsPolygonAnnotation::
CQChartsPolygonAnnotation(CQChartsView *view, const QPolygonF &points) :
 CQChartsAnnotation(view), points_(points)
{
  setObjectName(QString("poly.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolygonAnnotation::
CQChartsPolygonAnnotation(CQChartsPlot *plot, const QPolygonF &points) :
 CQChartsAnnotation(plot), points_(points)
{
  setObjectName(QString("poly.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolygonAnnotation::
~CQChartsPolygonAnnotation()
{
}

void
CQChartsPolygonAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addStrokeFillProperties(model, path1);
}

QString
CQChartsPolygonAnnotation::
propertyId() const
{
  return QString("polygonAnnotation.%1").arg(ind());
}

void
CQChartsPolygonAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();
  double sx = (bbox_.getWidth () > 0 ? bbox.getWidth ()/bbox_.getWidth () : 1.0);
  double sy = (bbox_.getHeight() > 0 ? bbox.getHeight()/bbox_.getHeight() : 1.0);

  double x1 = bbox_.getXMin();
  double y1 = bbox_.getYMin();

  for (int i = 0; i < points_.size(); ++i) {
    points_[i].setX(sx*(points_[i].x() - x1) + x1 + dx);
    points_[i].setY(sy*(points_[i].y() - y1) + y1 + dy);
  }

  bbox_ = bbox;
}

bool
CQChartsPolygonAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return (points_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill));
}

void
CQChartsPolygonAnnotation::
draw(QPainter *painter)
{
  if (! points_.size())
    return;

  double x1 = points_[0].x();
  double y1 = points_[0].y();
  double x2 = x1;
  double y2 = y1;

  for (int i = 1; i < points_.size(); ++i) {
    x1 = std::min(x1, points_[i].x());
    y1 = std::min(y1, points_[i].y());
    x2 = std::max(x2, points_[i].x());
    y2 = std::max(y2, points_[i].y());
  }

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  // create path
  QPainterPath path;

  double px = 0.0, py = 0.0;

  if      (plot())
    plot()->windowToPixel(points_[0].x(), points_[0].y(), px, py);
  else if (view())
    view()->windowToPixel(points_[0].x(), points_[0].y(), px, py);

  path.moveTo(px, py);

  for (int i = 1; i < points_.size(); ++i) {
    double px = 0.0, py = 0.0;

    if      (plot())
      plot()->windowToPixel(points_[i].x(), points_[i].y(), px, py);
    else if (view())
      view()->windowToPixel(points_[i].x(), points_[i].y(), px, py);

    path.lineTo(px, py);
  }

  path.closeSubpath();

  //---

  QPen   pen;
  QBrush brush;

  QColor bgColor = interpBackgroundColor(0, 1);

  if      (plot())
    plot()->setBrush(brush, isBackground(), bgColor, backgroundAlpha(), backgroundPattern());
  else if (view())
    view()->setBrush(brush, isBackground(), bgColor, backgroundAlpha(), backgroundPattern());

  QColor borderColor = interpBorderColor(0, 1);

  if      (plot())
    plot()->setPen(pen, isBorder(), borderColor, borderAlpha(), borderWidth(), borderDash());
  else if (view())
    view()->setPen(pen, isBorder(), borderColor, borderAlpha(), borderWidth(), borderDash());

  if (plot())
    plot()->updateObjPenBrushState(this, pen, brush);

  //---

  painter->fillPath  (path, brush);
  painter->strokePath(path, pen  );

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsPolylineAnnotation::
CQChartsPolylineAnnotation(CQChartsView *view, const QPolygonF &points) :
 CQChartsAnnotation(view), points_(points)
{
  setObjectName(QString("poly.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolylineAnnotation::
CQChartsPolylineAnnotation(CQChartsPlot *plot, const QPolygonF &points) :
 CQChartsAnnotation(plot), points_(points)
{
  setObjectName(QString("poly.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolylineAnnotation::
~CQChartsPolylineAnnotation()
{
}

void
CQChartsPolylineAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addStrokeProperties(model, path1);
}

QString
CQChartsPolylineAnnotation::
propertyId() const
{
  return QString("polylineAnnotation.%1").arg(ind());
}

void
CQChartsPolylineAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();
  double sx = (bbox_.getWidth () > 0 ? bbox.getWidth ()/bbox_.getWidth () : 1.0);
  double sy = (bbox_.getHeight() > 0 ? bbox.getHeight()/bbox_.getHeight() : 1.0);

  double x1 = bbox_.getXMin();
  double y1 = bbox_.getYMin();

  for (int i = 0; i < points_.size(); ++i) {
    points_[i].setX(sx*(points_[i].x() - x1) + x1 + dx);
    points_[i].setY(sy*(points_[i].y() - y1) + y1 + dy);
  }

  bbox_ = bbox;
}

bool
CQChartsPolylineAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  double px, py;

  plot()->windowToPixel(p.x, p.y, px, py);

  CQChartsGeom::Point pp(px, py);

  for (int i = 1; i < points_.size(); ++i) {
    double x1 = points_[i - 1].x();
    double y1 = points_[i - 1].y();
    double x2 = points_[i    ].x();
    double y2 = points_[i    ].y();

    double px1, py1, px2, py2;

    plot()->windowToPixel(x1, y1, px1, py1);
    plot()->windowToPixel(x2, y2, px2, py2);

    CQChartsGeom::Point pl1(px1, py1);
    CQChartsGeom::Point pl2(px2, py2);

    double d;

    if (CQChartsUtil::PointLineDistance(pp, pl1, pl2, &d) && d < 3)
      return true;
  }

  return false;
}

void
CQChartsPolylineAnnotation::
draw(QPainter *painter)
{
  if (! points_.size())
    return;

  double x1 = points_[0].x();
  double y1 = points_[0].y();
  double x2 = x1;
  double y2 = y1;

  for (int i = 1; i < points_.size(); ++i) {
    x1 = std::min(x1, points_[i].x());
    y1 = std::min(y1, points_[i].y());
    x2 = std::max(x2, points_[i].x());
    y2 = std::max(y2, points_[i].y());
  }

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  // create path
  QPainterPath path;

  double px = 0.0, py = 0.0;

  if      (plot())
    plot()->windowToPixel(points_[0].x(), points_[0].y(), px, py);
  else if (view())
    view()->windowToPixel(points_[0].x(), points_[0].y(), px, py);

  path.moveTo(px, py);

  for (int i = 1; i < points_.size(); ++i) {
    double px = 0.0, py = 0.0;

    if      (plot())
      plot()->windowToPixel(points_[i].x(), points_[i].y(), px, py);
    else if (view())
      view()->windowToPixel(points_[i].x(), points_[i].y(), px, py);

    path.lineTo(px, py);
  }

  //---

  QPen pen;

  QColor borderColor = interpBorderColor(0, 1);

  if      (plot())
    plot()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), borderDash());
  else if (view())
    view()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), borderDash());

  painter->strokePath(path, pen);

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsTextAnnotation::
CQChartsTextAnnotation(CQChartsView *view, const CQChartsPosition &position,
                       const QString &textStr) :
 CQChartsAnnotation(view), position_(position)
{
  setObjectName(QString("text.%1").arg(ind()));

  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextStr  (textStr);
  setTextColor(themeFg);

  boxData_.shape.background.visible = false;
  boxData_.shape.border    .visible = false;

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsTextAnnotation::
CQChartsTextAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                       const QString &textStr) :
 CQChartsAnnotation(plot), position_(position)
{
  setObjectName(QString("text.%1").arg(ind()));

  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextStr  (textStr);
  setTextColor(themeFg);

  boxData_.shape.background.visible = false;
  boxData_.shape.border    .visible = false;

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsTextAnnotation::
~CQChartsTextAnnotation()
{
}

void
CQChartsTextAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  model->addProperty(path1, this, "position"    , "position");
  model->addProperty(path1, this, "textStr"     , "text"    );
  model->addProperty(path1, this, "textFont"    , "font"    );
  model->addProperty(path1, this, "textColor"   , "color"   );
  model->addProperty(path1, this, "textAlpha"   , "alpha"   );
  model->addProperty(path1, this, "textAngle"   , "angle"   );
  model->addProperty(path1, this, "textContrast", "contrast");
  model->addProperty(path1, this, "textAlign"   , "align"   );

  addStrokeFillProperties(model, path1);
}

QString
CQChartsTextAnnotation::
propertyId() const
{
  return QString("textAnnotation.%1").arg(ind());
}

void
CQChartsTextAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot()) {
    xp = plot()->pixelToWindowWidth (padding());
    yp = plot()->pixelToWindowHeight(padding());
    xm = plot()->pixelToWindowWidth (margin ());
    ym = plot()->pixelToWindowHeight(margin ());
  }
  else if (view()) {
    xp = view()->pixelToWindowWidth (padding());
    yp = view()->pixelToWindowHeight(padding());
    xm = view()->pixelToWindowWidth (margin ());
    ym = view()->pixelToWindowHeight(margin ());
  }

  double x = bbox.getXMin() + xp + xm;
  double y = bbox.getYMin() + yp + ym;

  CQChartsGeom::Point vp;

  if (plot())
    vp = plot()->windowToView(CQChartsGeom::Point(x, y));
  else
    vp = CQChartsGeom::Point(x, y);

  position_ = CQChartsPosition(CQChartsUtil::toQPoint(vp), CQChartsPosition::Units::VIEW);

  bbox_ = bbox;
}

bool
CQChartsTextAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return CQChartsAnnotation::inside(p);
}

void
CQChartsTextAnnotation::
draw(QPainter *painter)
{
  if (autoSize_) {
    double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

    if      (plot()) {
      xp = plot()->pixelToWindowWidth (padding());
      yp = plot()->pixelToWindowHeight(padding());
      xm = plot()->pixelToWindowWidth (margin ());
      ym = plot()->pixelToWindowHeight(margin ());
    }
    else if (view()) {
      xp = view()->pixelToWindowWidth (padding());
      yp = view()->pixelToWindowHeight(padding());
      xm = view()->pixelToWindowWidth (margin ());
      ym = view()->pixelToWindowHeight(margin ());
    }

    if (! isHtml()) {
      QFont font;

      if      (plot())
        font = view()->plotFont(plot(), textFont());
      else if (view())
        font = view()->viewFont(textFont());

      QFontMetricsF fm(font);

      double w = 0.0, h = 0.0;

      if      (plot()) {
        w = plot()->pixelToWindowWidth (fm.width(textStr())) + 2*xp + 2*xm;
        h = plot()->pixelToWindowHeight(fm.height())         + 2*yp + 2*ym;
      }
      else if (view()) {
        w = view()->pixelToWindowWidth (fm.width(textStr())) + 2*xp + 2*xm;
        h = view()->pixelToWindowHeight(fm.height())         + 2*yp + 2*ym;
      }

      QPointF p;

      if      (plot())
        p = plot()->positionToPlot(position_);
      else if (view())
        p = view()->positionToView(position_);

      double x = p.x();
      double y = p.y();

      if      (textAlign() & Qt::AlignLeft) {
      }
      else if (textAlign() & Qt::AlignHCenter) {
        x -= w/2.0;
      }
      else if (textAlign() & Qt::AlignRight) {
        x -= w - 2*xp - 2*xm;
      }

      if      (textAlign() & Qt::AlignTop) {
        y -= h - 2*yp - 2*ym;
      }
      else if (textAlign() & Qt::AlignVCenter) {
        y -= h/2;
      }
      else if (textAlign() & Qt::AlignBottom) {
      }

      bbox_ = CQChartsGeom::BBox(x - xp - xm, y - yp - ym, x + w, y + h);
    }
    else {
      QTextDocument td;

      td.setHtml(textStr());

      QAbstractTextDocumentLayout *layout = td.documentLayout();

      QSizeF size = layout->documentSize();

      double w = 0.0, h = 0.0;

      if      (plot()) {
        w = plot()->pixelToWindowWidth (size.width ());
        h = plot()->pixelToWindowHeight(size.height());
      }
      else if (view()) {
        w = view()->pixelToWindowWidth (size.width ());
        h = view()->pixelToWindowHeight(size.height());
      }

      QPointF p;

      if      (plot())
        p = plot()->positionToPlot(position_);
      else if (view())
        p = view()->positionToView(position_);

      double x = p.x();
      double y = p.y();

      bbox_ = CQChartsGeom::BBox(x - xp - xm, y - yp - ym, x + w, y + h);
    }
  }

  //---

  // draw box
  CQChartsGeom::BBox prect;

  if      (plot())
    plot()->windowToPixel(bbox_, prect);
  else if (view())
    prect = view()->windowToPixel(bbox_);

  CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor c;

  if      (plot())
    c = textColor().interpColor(plot(), 0, 1);
  else if (view())
    c = textColor().interpColor(view(), 0, 1);

  if      (plot())
    plot()->setPen(pen, true, c, textAlpha(), CQChartsLength("0px"));
  else if (view())
    view()->setPen(pen, true, c, textAlpha(), CQChartsLength("0px"));

  brush.setStyle(Qt::NoBrush);

  if (plot())
    plot()->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw text
  QFont font;

  if     (plot())
    view()->setPlotPainterFont(plot(), painter, textFont());
  else if (view())
    view()->setPainterFont(painter, textFont());

  double tx = prect.getXMin  () +   margin() +   padding();
  double ty = prect.getYMin  () +   margin() +   padding();
  double tw = prect.getWidth () - 2*margin() - 2*padding();
  double th = prect.getHeight() - 2*margin() - 2*padding();

  if (! isHtml()) {
    QRectF trect(tx, ty, tw, th);

    CQChartsTextOptions textOptions;

    textOptions.angle     = textAngle();
    textOptions.contrast  = isTextContrast();
    textOptions.formatted = true;
    textOptions.clipped   = false;
    textOptions.align     = textAlign();

    if      (plot())
      plot()->drawTextInBox(painter, trect, textStr(), pen, textOptions);
    else if (view())
      view()->drawTextInBox(painter, trect, textStr(), pen, textOptions);
  }
  else {
    QRect trect(tx, ty, tw, th);

    painter->setRenderHints(QPainter::Antialiasing);

    QTextDocument td;

    td.setHtml(textStr());

    QRect trect1 = trect.translated(-trect.x(), -trect.y());

    painter->translate(trect.x(), trect.y());

    painter->setClipRect(trect1);

    QAbstractTextDocumentLayout::PaintContext ctx;

    ctx.palette.setColor(QPalette::Text, pen.color());

    QAbstractTextDocumentLayout *layout = td.documentLayout();

    layout->setPaintDevice(painter->device());

    layout->draw(painter, ctx);

    painter->translate(-trect.x(), -trect.y());
  }

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsArrowAnnotation::
CQChartsArrowAnnotation(CQChartsView *view, const CQChartsPosition &start,
                        const CQChartsPosition &end) :
 CQChartsAnnotation(view), start_(start), end_(end)
{
  setObjectName(QString("arrow.%1").arg(ind()));

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);

  arrow_ = new CQChartsArrow(view);

  connect(arrow_, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
}

CQChartsArrowAnnotation::
CQChartsArrowAnnotation(CQChartsPlot *plot, const CQChartsPosition &start,
                        const CQChartsPosition &end) :
 CQChartsAnnotation(plot), start_(start), end_(end)
{
  setObjectName(QString("arrow.%1").arg(ind()));

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);

  arrow_ = new CQChartsArrow(plot);

  connect(arrow_, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
}

CQChartsArrowAnnotation::
~CQChartsArrowAnnotation()
{
  delete arrow_;
}

void
CQChartsArrowAnnotation::
setData(const CQChartsArrowData &data)
{
  arrow_->setData(data);

  emit dataChanged();
}

void
CQChartsArrowAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  model->addProperty(path1, this  , "start"    );
  model->addProperty(path1, this  , "end"      );
  model->addProperty(path1, arrow_, "length"   );
  model->addProperty(path1, arrow_, "angle"    );
  model->addProperty(path1, arrow_, "backAngle");
  model->addProperty(path1, arrow_, "fhead"    );
  model->addProperty(path1, arrow_, "thead"    );
  model->addProperty(path1, arrow_, "filled"   );
  model->addProperty(path1, arrow_, "empty"    );
  model->addProperty(path1, arrow_, "lineWidth");
  model->addProperty(path1, arrow_, "labels"   );
}

QString
CQChartsArrowAnnotation::
propertyId() const
{
  return QString("arrowAnnotation.%1").arg(ind());
}

void
CQChartsArrowAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  QPointF start, end;

  if      (plot()) {
    start = plot()->positionToPlot(start_);
    end   = plot()->positionToPlot(end_  );
  }
  else if (view()) {
    start = view()->positionToView(start_);
    end   = view()->positionToView(end_  );
  }

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMax(), y2 = bbox.getYMax();

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot()) {
    xp = plot()->pixelToWindowWidth (padding());
    yp = plot()->pixelToWindowHeight(padding());
    xm = plot()->pixelToWindowWidth (margin ());
    ym = plot()->pixelToWindowHeight(margin ());
  }
  else if (view()) {
    xp = view()->pixelToWindowWidth (padding());
    yp = view()->pixelToWindowHeight(padding());
    xm = view()->pixelToWindowWidth (margin ());
    ym = view()->pixelToWindowHeight(margin ());
  }

  x1 += xp + xm; y1 += yp + ym;
  x2 -= xp + xm; y2 -= yp + ym;

  if (start.x() > end.x()) std::swap(x1, x2);
  if (start.y() > end.y()) std::swap(y1, y2);

  start.setX(x1); end.setX(x2);
  start.setY(y1); end.setY(y2);

  start_ = CQChartsPosition(start);
  end_   = CQChartsPosition(end  );

  bbox_ = bbox;
}

bool
CQChartsArrowAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  QPointF start, end;

  if      (plot()) {
    start = plot()->positionToPlot(start_);
    end   = plot()->positionToPlot(end_  );
  }
  else if (view()) {
    start = view()->positionToView(start_);
    end   = view()->positionToView(end_  );
  }

  CQChartsGeom::Point p1;

  if      (plot())
    p1 = plot()->windowToPixel(p);
  else if (view())
    p1 = view()->windowToPixel(p);

  CQChartsGeom::Point ps = CQChartsUtil::fromQPoint(start);
  CQChartsGeom::Point pe = CQChartsUtil::fromQPoint(end  );

  double d;

  return (CQChartsUtil::PointLineDistance(p1, ps, pe, &d) && d < 3);
}

void
CQChartsArrowAnnotation::
draw(QPainter *painter)
{
  QPointF start, end;

  if      (plot()) {
    start = plot()->positionToPlot(start_);
    end   = plot()->positionToPlot(end_  );
  }
  else if (view()) {
    start = view()->positionToView(start_);
    end   = view()->positionToView(end_  );
  }

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot()) {
    xp = plot()->pixelToWindowWidth (padding());
    yp = plot()->pixelToWindowHeight(padding());
    xm = plot()->pixelToWindowWidth (margin ());
    ym = plot()->pixelToWindowHeight(margin ());
  }
  else if (view()) {
    xp = view()->pixelToWindowWidth (padding());
    yp = view()->pixelToWindowHeight(padding());
    xm = view()->pixelToWindowWidth (margin ());
    ym = view()->pixelToWindowHeight(margin ());
  }

  double x1 = std::min(start.x(), end.x());
  double y1 = std::min(start.y(), end.y());
  double x2 = std::max(start.x(), end.x());
  double y2 = std::max(start.y(), end.y());

  double x = x1 - xp - xm; // bottom
  double y = y1 - yp - ym; // top
  double w = (x2 - x1) + 2*xp + 2*xm;
  double h = (y2 - y1) + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  CQChartsGeom::BBox prect;

  if      (plot())
    plot()->windowToPixel(bbox_, prect);
  else if (view())
    prect = view()->windowToPixel(bbox_);

  //CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  arrow_->setFrom(start);
  arrow_->setTo  (end  );

  arrow_->draw(painter);

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsPointAnnotation::
CQChartsPointAnnotation(CQChartsView *view, const CQChartsPosition &position,
                        const CQChartsSymbol &type) :
 CQChartsAnnotation(view), position_(position)
{
  setObjectName(QString("point.%1").arg(ind()));

  pointData_.type = type;
}

CQChartsPointAnnotation::
CQChartsPointAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                        const CQChartsSymbol &type) :
 CQChartsAnnotation(plot), position_(position)
{
  setObjectName(QString("point.%1").arg(ind()));

  pointData_.type = type;
}

CQChartsPointAnnotation::
~CQChartsPointAnnotation()
{
}

void
CQChartsPointAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  model->addProperty(path1, this, "position");
}

QString
CQChartsPointAnnotation::
propertyId() const
{
  return QString("pointAnnotation.%1").arg(ind());
}

void
CQChartsPointAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  QPointF position;

  if      (plot())
    position = plot()->positionToPlot(position_);
  else if (view())
    position = view()->positionToView(position_);

  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  position += QPointF(dx, dy);

  position_ = CQChartsPosition(position);

  bbox_ = bbox;
}

bool
CQChartsPointAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return CQChartsAnnotation::inside(p);
}

void
CQChartsPointAnnotation::
draw(QPainter *painter)
{
  QPointF position;

  if      (plot())
    position = plot()->positionToPlot(position_);
  else if (view())
    position = view()->positionToView(position_);

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot()) {
    xp = plot()->pixelToWindowWidth (padding());
    yp = plot()->pixelToWindowHeight(padding());
    xm = plot()->pixelToWindowWidth (margin ());
    ym = plot()->pixelToWindowHeight(margin ());
  }
  else if (view()) {
    xp = view()->pixelToWindowWidth (padding());
    yp = view()->pixelToWindowHeight(padding());
    xm = view()->pixelToWindowWidth (margin ());
    ym = view()->pixelToWindowHeight(margin ());
  }

  double sw = 0.0, sh = 0.0;

  if      (plot()) {
    sw = plot()->lengthPlotWidth (pointData_.size);
    sh = plot()->lengthPlotHeight(pointData_.size);
  }
  else if (view()) {
    sw = view()->lengthViewWidth (pointData_.size);
    sh = view()->lengthViewHeight(pointData_.size);
  }

  double x = position.x() - xp - xm; // bottom
  double y = position.y() - yp - ym; // top
  double w = sw + 2*xp + 2*xm;
  double h = sh + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  CQChartsGeom::BBox prect;

  if      (plot())
    plot()->windowToPixel(bbox_, prect);
  else if (view())
    prect = view()->windowToPixel(bbox_);

  //CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  double px = prect.getXMid();
  double py = prect.getYMid();

  //---

  QPen   pen;
  QBrush brush;

  QColor lineColor, fillColor;

  if      (plot()) {
    lineColor = pointData_.stroke.color.interpColor(plot(), 0, 1);
    fillColor = pointData_.fill  .color.interpColor(plot(), 0, 1);
  }
  else if (view()) {
    lineColor = pointData_.stroke.color.interpColor(view(), 0, 1);
    fillColor = pointData_.fill  .color.interpColor(view(), 0, 1);
  }

  if      (plot())
    plot()->setPen(pen, pointData_.stroke.visible, lineColor, pointData_.stroke.alpha,
                   pointData_.stroke.width, pointData_.stroke.dash);
  else if (view())
    view()->setPen(pen, pointData_.stroke.visible, lineColor, pointData_.stroke.alpha,
                   pointData_.stroke.width, pointData_.stroke.dash);

  if      (plot())
    plot()->setBrush(brush, pointData_.fill.visible, fillColor, pointData_.fill.alpha,
                     pointData_.fill.pattern);
  else if (view())
    view()->setBrush(brush, pointData_.fill.visible, fillColor, pointData_.fill.alpha,
                     pointData_.fill.pattern);

  if (plot())
    plot()->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  CQChartsSymbol2DRenderer srenderer(painter, CQChartsGeom::Point(px, py),
                                     CMathUtil::avg(sw, sh));

  if (painter->brush().style() != Qt::NoBrush)
    CQChartsPlotSymbolMgr::fillSymbol(pointData_.type, &srenderer);

  if (painter->pen().style() != Qt::NoPen)
    CQChartsPlotSymbolMgr::drawSymbol(pointData_.type, &srenderer);

  //---

  CQChartsAnnotation::draw(painter);
}
