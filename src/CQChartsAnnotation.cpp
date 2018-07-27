#include <CQChartsAnnotation.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsArrow.h>
#include <CQPropertyViewModel.h>
#include <QPainter>

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
pathId() const
{
  QString id = QString("%1").arg(ind_);

  if      (plot_)
    return plot_->pathId() + ":" + id;
  else if (view_)
    return view_->id() + ":" + id;
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
addStrokeProperties(CQPropertyViewModel *model, const QString &path)
{
  QString bgPath = path + "/background";

  model->addProperty(bgPath, this, "background"       , "visible");
  model->addProperty(bgPath, this, "backgroundColor"  , "color"  );
  model->addProperty(bgPath, this, "backgroundAlpha"  , "alpha"  );
  model->addProperty(bgPath, this, "backgroundPattern", "pattern");
}

void
CQChartsAnnotation::
addFillProperties(CQPropertyViewModel *model, const QString &path)
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

  if      (plot_)
    plot_->invalidateLayer(CQChartsLayer::Type::ANNOTATION);
  else if (view_)
    view_->update();

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

  if      (plot_)
    plot_->invalidateLayer(CQChartsLayer::Type::ANNOTATION);
  else if (view_)
    view_->update();
}

//------

void
CQChartsAnnotation::
draw(QPainter *painter)
{
  if (! plot_ && view_->mode() == CQChartsView::Mode::EDIT && isSelected())
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
  return (plot_ ? plot_->view() : view_);
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
  QString path1 = path + QString("/rectAnnotation.%1").arg(ind());

  CQChartsAnnotation::addProperties(model, path1);

  model->addProperty(path1, this, "start");
  model->addProperty(path1, this, "end"  );

  model->addProperty(path1, this, "margin");
  model->addProperty(path1, this, "padding");

  addStrokeFillProperties(model, path1);
}

void
CQChartsRectAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  QPointF start, end;

  if      (plot_) {
    start = plot_->positionToPlot(start_);
    end   = plot_->positionToPlot(end_  );
  }
  else if (view_) {
    start = view_->positionToView(start_);
    end   = view_->positionToView(end_  );
  }

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMax(), y2 = bbox.getYMax();

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot_) {
    xp = plot_->pixelToWindowWidth (padding());
    yp = plot_->pixelToWindowHeight(padding());
    xm = plot_->pixelToWindowWidth (margin ());
    ym = plot_->pixelToWindowHeight(margin ());
  }
  else if (view_) {
    xp = view_->pixelToWindowWidth (padding());
    yp = view_->pixelToWindowHeight(padding());
    xm = view_->pixelToWindowWidth (margin ());
    ym = view_->pixelToWindowHeight(margin ());
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

  if      (plot_) {
    start = plot_->positionToPlot(start_);
    end   = plot_->positionToPlot(end_  );
  }
  else if (view_) {
    start = view_->positionToView(start_);
    end   = view_->positionToView(end_  );
  }

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot_) {
    xp = plot_->pixelToWindowWidth (padding());
    yp = plot_->pixelToWindowHeight(padding());
    xm = plot_->pixelToWindowWidth (margin ());
    ym = plot_->pixelToWindowHeight(margin ());
  }
  else if (view_) {
    xp = view_->pixelToWindowWidth (padding());
    yp = view_->pixelToWindowHeight(padding());
    xm = view_->pixelToWindowWidth (margin ());
    ym = view_->pixelToWindowHeight(margin ());
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

  if      (plot_)
    plot_->windowToPixel(bbox_, prect);
  else if (view_)
    prect = view_->windowToPixel(bbox_);

  CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsEllipseAnnotation::
CQChartsEllipseAnnotation(CQChartsView *view, const CQChartsPosition &center,
                          double xRadius, double yRadius) :
 CQChartsAnnotation(view), center_(center), xRadius_(xRadius), yRadius_(yRadius)
{
  setObjectName(QString("ellipse.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsEllipseAnnotation::
CQChartsEllipseAnnotation(CQChartsPlot *plot, const CQChartsPosition &center,
                          double xRadius, double yRadius) :
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
  QString path1 = path + QString("/ellipseAnnotation.%1").arg(ind());

  CQChartsAnnotation::addProperties(model, path1);

  model->addProperty(path1, this, "center" );
  model->addProperty(path1, this, "xRadius");
  model->addProperty(path1, this, "yRadius");

  addStrokeFillProperties(model, path1);
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

void
CQChartsEllipseAnnotation::
draw(QPainter *painter)
{
  QPointF center;

  if      (plot_)
    center = plot_->positionToPlot(center_);
  else if (view_)
    center = view_->positionToView(center_);

  double x1 = center.x() - xRadius_;
  double y1 = center.y() - yRadius_;
  double x2 = center.x() + xRadius_;
  double y2 = center.y() + yRadius_;

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  CQChartsGeom::BBox prect;

  if      (plot_)
    plot_->windowToPixel(bbox_, prect);
  else if (view_)
    prect = view_->windowToPixel(bbox_);

  //CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  QPainterPath path;

  path.addEllipse(CQChartsUtil::toQRect(prect));

  //---

  if (isBackground()) {
    QColor bgColor = interpBackgroundColor(0, 1);

    bgColor.setAlphaF(backgroundAlpha());

    QBrush brush(bgColor);

    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) backgroundPattern()));

    painter->fillPath(path, brush);
  }

  if (isBorder()) {
    QColor borderColor = interpBorderColor(0, 1);

    borderColor.setAlphaF(borderAlpha());

    QPen pen(borderColor);

    double bw = 0.0;

    if      (plot_)
      bw = plot_->lengthPixelWidth(borderWidth());
    else if (view_)
      bw = view_->lengthPixelWidth(borderWidth());

    pen.setWidthF(bw);

    painter->strokePath(path, pen);
  }

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
  QString path1 = path + QString("/polyAnnotation.%1").arg(ind());

  CQChartsAnnotation::addProperties(model, path1);

  addStrokeFillProperties(model, path1);
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

  QPainterPath path;

  double px = 0.0, py = 0.0;

  if      (plot_)
    plot_->windowToPixel(points_[0].x(), points_[0].y(), px, py);
  else if (view_)
    view_->windowToPixel(points_[0].x(), points_[0].y(), px, py);

  path.moveTo(px, py);

  for (int i = 1; i < points_.size(); ++i) {
    double px = 0.0, py = 0.0;

    if      (plot_)
      plot_->windowToPixel(points_[i].x(), points_[i].y(), px, py);
    else if (view_)
      view_->windowToPixel(points_[i].x(), points_[i].y(), px, py);

    path.lineTo(px, py);
  }

  path.closeSubpath();

  //---

  if (isBackground()) {
    QColor bgColor = interpBackgroundColor(0, 1);

    bgColor.setAlphaF(backgroundAlpha());

    QBrush brush(bgColor);

    painter->fillPath(path, brush);
  }

  if (isBorder()) {
    QColor borderColor = interpBorderColor(0, 1);

    borderColor.setAlphaF(borderAlpha());

    QPen pen(borderColor);

    double bw = 0.0;

    if      (plot_)
      bw = plot_->lengthPixelWidth(borderWidth());
    else if (view_)
      bw = view_->lengthPixelWidth(borderWidth());

    pen.setWidthF(bw);

    painter->strokePath(path, pen);
  }

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
  QString path1 = path + QString("/polyAnnotation.%1").arg(ind());

  CQChartsAnnotation::addProperties(model, path1);

  addStrokeProperties(model, path1);
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

  QPainterPath path;

  double px = 0.0, py = 0.0;

  if      (plot_)
    plot_->windowToPixel(points_[0].x(), points_[0].y(), px, py);
  else if (view_)
    view_->windowToPixel(points_[0].x(), points_[0].y(), px, py);

  path.moveTo(px, py);

  for (int i = 1; i < points_.size(); ++i) {
    double px = 0.0, py = 0.0;

    if      (plot_)
      plot_->windowToPixel(points_[i].x(), points_[i].y(), px, py);
    else if (view_)
      view_->windowToPixel(points_[i].x(), points_[i].y(), px, py);

    path.lineTo(px, py);
  }

  //---

  QColor borderColor = interpBorderColor(0, 1);

  borderColor.setAlphaF(borderAlpha());

  QPen pen(borderColor);

  double bw = 0.0;

  if      (plot_)
    bw = plot_->lengthPixelWidth(borderWidth());
  else if (view_)
    bw = view_->lengthPixelWidth(borderWidth());

  pen.setWidthF(bw);

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
  QString path1 = path + QString("/textAnnotation.%1").arg(ind());

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

void
CQChartsTextAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if (plot_) {
    xp = plot_->pixelToWindowWidth (padding());
    yp = plot_->pixelToWindowHeight(padding());
    xm = plot_->pixelToWindowWidth (margin ());
    ym = plot_->pixelToWindowHeight(margin ());
  }
  else if (view_) {
    xp = view_->pixelToWindowWidth (padding());
    yp = view_->pixelToWindowHeight(padding());
    xm = view_->pixelToWindowWidth (margin ());
    ym = view_->pixelToWindowHeight(margin ());
  }

  double x = bbox.getXMin() + xp + xm;
  double y = bbox.getYMin() + yp + ym;

  CQChartsGeom::Point vp;

  if (plot_)
    vp = plot_->windowToView(CQChartsGeom::Point(x, y));
  else
    vp = CQChartsGeom::Point(x, y);

  position_ = CQChartsPosition(CQChartsUtil::toQPoint(vp), CQChartsPosition::Units::VIEW);

  bbox_ = bbox;
}

void
CQChartsTextAnnotation::
draw(QPainter *painter)
{
  if (autoSize_) {
    QFontMetricsF fm(textFont());

    double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

    if      (plot_) {
      xp = plot_->pixelToWindowWidth (padding());
      yp = plot_->pixelToWindowHeight(padding());
      xm = plot_->pixelToWindowWidth (margin ());
      ym = plot_->pixelToWindowHeight(margin ());
    }
    else if (view_) {
      xp = view_->pixelToWindowWidth (padding());
      yp = view_->pixelToWindowHeight(padding());
      xm = view_->pixelToWindowWidth (margin ());
      ym = view_->pixelToWindowHeight(margin ());
    }

    double w = 0.0, h = 0.0;

    if      (plot_) {
      w = plot_->pixelToWindowWidth (fm.width(textStr())) + 2*xp + 2*xm;
      h = plot_->pixelToWindowHeight(fm.height())         + 2*yp + 2*ym;
    }
    else if (view_) {
      w = view_->pixelToWindowWidth (fm.width(textStr())) + 2*xp + 2*xm;
      h = view_->pixelToWindowHeight(fm.height())         + 2*yp + 2*ym;
    }

    QPointF p;

    if      (plot_)
      p = plot_->positionToPlot(position_);
    else if (view_)
      p = view_->positionToView(position_);

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

    bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);
  }

  //---

  CQChartsGeom::BBox prect;

  if      (plot_)
    plot_->windowToPixel(bbox_, prect);
  else if (view_)
    prect = view_->windowToPixel(bbox_);

  CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  QColor c;

  if (plot_)
    c = textColor().interpColor(plot_, 0, 1);
  else
    c = textColor().interpColor(view_, 0, 1);

  c.setAlphaF(textAlpha());

  QPen pen(c);

  painter->setPen (pen);
  painter->setFont(textFont());

  double tx = prect.getXMin  () +   margin() +   padding();
  double ty = prect.getYMin  () +   margin() +   padding();
  double tw = prect.getWidth () - 2*margin() - 2*padding();
  double th = prect.getHeight() - 2*margin() - 2*padding();

  QRectF trect(tx, ty, tw, th);

  CQChartsTextOptions textOptions;

  textOptions.angle     = textAngle();
  textOptions.contrast  = isTextContrast();
  textOptions.formatted = true;
  textOptions.clipped   = false;
  textOptions.align     = textAlign();

  if      (plot_)
    plot_->drawTextInBox(painter, trect, textStr(), pen, textOptions);
  else if (view_)
    view_->drawTextInBox(painter, trect, textStr(), pen, textOptions);

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

  arrow_ = new CQChartsArrow(view_);

  connect(arrow_, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
}

CQChartsArrowAnnotation::
CQChartsArrowAnnotation(CQChartsPlot *plot, const CQChartsPosition &start,
                        const CQChartsPosition &end) :
 CQChartsAnnotation(plot), start_(start), end_(end)
{
  setObjectName(QString("arrow.%1").arg(ind()));

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);

  arrow_ = new CQChartsArrow(plot_);

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
  QString path1 = path + QString("/arrowAnnotation.%1").arg(ind());

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

void
CQChartsArrowAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  QPointF start, end;

  if      (plot_) {
    start = plot_->positionToPlot(start_);
    end   = plot_->positionToPlot(end_  );
  }
  else if (view_) {
    start = view_->positionToView(start_);
    end   = view_->positionToView(end_  );
  }

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMax(), y2 = bbox.getYMax();

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot_) {
    xp = plot_->pixelToWindowWidth (padding());
    yp = plot_->pixelToWindowHeight(padding());
    xm = plot_->pixelToWindowWidth (margin ());
    ym = plot_->pixelToWindowHeight(margin ());
  }
  else if (view_) {
    xp = view_->pixelToWindowWidth (padding());
    yp = view_->pixelToWindowHeight(padding());
    xm = view_->pixelToWindowWidth (margin ());
    ym = view_->pixelToWindowHeight(margin ());
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

void
CQChartsArrowAnnotation::
draw(QPainter *painter)
{
  QPointF start, end;

  if      (plot_) {
    start = plot_->positionToPlot(start_);
    end   = plot_->positionToPlot(end_  );
  }
  else if (view_) {
    start = view_->positionToView(start_);
    end   = view_->positionToView(end_  );
  }

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot_) {
    xp = plot_->pixelToWindowWidth (padding());
    yp = plot_->pixelToWindowHeight(padding());
    xm = plot_->pixelToWindowWidth (margin ());
    ym = plot_->pixelToWindowHeight(margin ());
  }
  else if (view_) {
    xp = view_->pixelToWindowWidth (padding());
    yp = view_->pixelToWindowHeight(padding());
    xm = view_->pixelToWindowWidth (margin ());
    ym = view_->pixelToWindowHeight(margin ());
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

  if      (plot_)
    plot_->windowToPixel(bbox_, prect);
  else if (view_)
    prect = view_->windowToPixel(bbox_);

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
  QString path1 = path + QString("/pointAnnotation.%1").arg(ind());

  CQChartsAnnotation::addProperties(model, path1);

  model->addProperty(path1, this, "position");
}

void
CQChartsPointAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &)
{
  QPointF position;

  if      (plot_)
    position = plot_->positionToPlot(position_);
  else if (view_)
    position = view_->positionToView(position_);

  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  position += QPointF(dx, dy);

  position_ = CQChartsPosition(position);

  bbox_ = bbox;
}

void
CQChartsPointAnnotation::
draw(QPainter *painter)
{
  QPointF position;

  if      (plot_)
    position = plot_->positionToPlot(position_);
  else if (view_)
    position = view_->positionToView(position_);

  double xp = 0.0, yp = 0.0, xm = 0.0, ym = 0.0;

  if      (plot_) {
    xp = plot_->pixelToWindowWidth (padding());
    yp = plot_->pixelToWindowHeight(padding());
    xm = plot_->pixelToWindowWidth (margin ());
    ym = plot_->pixelToWindowHeight(margin ());
  }
  else if (view_) {
    xp = view_->pixelToWindowWidth (padding());
    yp = view_->pixelToWindowHeight(padding());
    xm = view_->pixelToWindowWidth (margin ());
    ym = view_->pixelToWindowHeight(margin ());
  }

  double sw = 0.0, sh = 0.0;

  if      (plot_) {
    sw = plot_->lengthPlotWidth (pointData_.size);
    sh = plot_->lengthPlotHeight(pointData_.size);
  }
  else if (view_) {
    sw = view_->lengthViewWidth (pointData_.size);
    sh = view_->lengthViewHeight(pointData_.size);
  }

  double x = position.x() - xp - xm; // bottom
  double y = position.y() - yp - ym; // top
  double w = sw + 2*xp + 2*xm;
  double h = sh + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  CQChartsGeom::BBox prect;

  if      (plot_)
    plot_->windowToPixel(bbox_, prect);
  else if (view_)
    prect = view_->windowToPixel(bbox_);

  //CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  double px = prect.getXMid();
  double py = prect.getYMid();

  QColor lineColor, fillColor;

  if (plot_) {
    lineColor = pointData_.stroke.color.interpColor(plot_, 0, 1);
    fillColor = pointData_.fill  .color.interpColor(plot_, 0, 1);
  }
  else {
    lineColor = pointData_.stroke.color.interpColor(view_, 0, 1);
    fillColor = pointData_.fill  .color.interpColor(view_, 0, 1);
  }

  lineColor.setAlphaF(pointData_.stroke.alpha);
  fillColor.setAlphaF(pointData_.fill  .alpha);

  QPen   pen;
  QBrush brush;

  if (pointData_.stroke.visible)
    pen.setColor(lineColor);
  else
    pen.setStyle(Qt::NoPen);

  double bw = 0.0;

  if      (plot_)
    bw = plot_->lengthPixelWidth(pointData_.stroke.width);
  else if (view_)
    bw = view_->lengthPixelWidth(pointData_.stroke.width);

  pen.setWidthF(bw);

  if (pointData_.fill.visible) {
    brush.setColor(fillColor);
    brush.setStyle(Qt::SolidPattern);
  }
  else
    brush.setStyle(Qt::NoBrush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  CQChartsSymbol2DRenderer srenderer(painter, CQChartsGeom::Point(px, py),
                                     CQChartsUtil::avg(sw, sh));

  if (painter->brush().style() != Qt::NoBrush)
    CQChartsPlotSymbolMgr::fillSymbol(pointData_.type, &srenderer);

  if (painter->pen().style() != Qt::NoPen)
    CQChartsPlotSymbolMgr::drawSymbol(pointData_.type, &srenderer);

  //---

  CQChartsAnnotation::draw(painter);
}
