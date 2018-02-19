#include <CQChartsAnnotation.h>
#include <CQChartsPlot.h>
#include <CQChartsArrow.h>
#include <CQPropertyViewModel.h>
#include <QPainter>

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

void
CQChartsAnnotation::
addProperties(CQPropertyViewModel *, const QString &)
{
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
editPress(const CQChartsGeom::Point &p)
{
  editHandles_.setDragPos(p);

  return true;
}

bool
CQChartsAnnotation::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point &dragPos = editHandles_.dragPos();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles_.updateBBox(dx, dy);

  setBBox(editHandles_.bbox());

  editHandles_.setDragPos(p);

  plot_->update();

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

//------

void
CQChartsAnnotation::
draw(QPainter *painter)
{
  if (isSelected()) {
    editHandles_.setBBox(this->bbox());

    editHandles_.draw(painter);
  }
}

//---

CQChartsRectAnnotation::
CQChartsRectAnnotation(CQChartsPlot *plot, const QPointF &start, const QPointF &end) :
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

  QString bgPath = path1 + "/background";

  model->addProperty(bgPath, this, "background"       , "visible");
  model->addProperty(bgPath, this, "backgroundColor"  , "color"  );
  model->addProperty(bgPath, this, "backgroundAlpha"  , "alpha"  );
  model->addProperty(bgPath, this, "backgroundPattern", "pattern");

  QString borderPath = path1 + "/border";

  model->addProperty(borderPath, this, "border"     , "visible"   );
  model->addProperty(borderPath, this, "borderColor", "color"     );
  model->addProperty(borderPath, this, "borderAlpha", "alpha"     );
  model->addProperty(borderPath, this, "borderWidth", "width"     );
  model->addProperty(borderPath, this, "cornerSize" , "cornerSize");
  model->addProperty(borderPath, this, "borderSides", "sides"     );
}

void
CQChartsRectAnnotation::
setBBox(const CQChartsGeom::BBox &bbox)
{
  double dx1 = bbox.getXMin() - bbox_.getXMin();
  double dy1 = bbox.getYMin() - bbox_.getYMin();
  double dx2 = bbox.getXMax() - bbox_.getXMax();
  double dy2 = bbox.getYMax() - bbox_.getYMax();

  if (start_.x() < end_.x()) {
    start_.setX(start_.x() + dx1);
    end_  .setX(end_  .x() + dx2);
  }
  else {
    start_.setX(start_.x() + dx2);
    end_  .setX(end_  .x() + dx1);
  }

  if (start_.y() < end_.y()) {
    start_.setY(start_.y() + dy1);
    end_  .setY(end_  .y() + dy2);
  }
  else {
    start_.setY(start_.y() + dy2);
    end_  .setY(end_  .y() + dy1);
  }

  bbox_ = bbox;
}

void
CQChartsRectAnnotation::
draw(QPainter *painter)
{
  double xp = plot_->pixelToWindowWidth (padding());
  double yp = plot_->pixelToWindowHeight(padding());
  double xm = plot_->pixelToWindowWidth (margin());
  double ym = plot_->pixelToWindowHeight(margin());

  double x1 = std::min(start_.x(), end_.x());
  double y1 = std::min(start_.y(), end_.y());
  double x2 = std::max(start_.x(), end_.x());
  double y2 = std::max(start_.y(), end_.y());

  double x = x1 - xp - xm; // bottom
  double y = y1 - yp - ym; // top
  double w = (x2 - x1) + 2*xp + 2*xm;
  double h = (y2 - y1) + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(bbox_, prect);

  CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsEllipseAnnotation::
CQChartsEllipseAnnotation(CQChartsPlot *plot, const QPointF &center,
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

  QString bgPath = path1 + "/background";

  model->addProperty(bgPath, this, "background"       , "visible");
  model->addProperty(bgPath, this, "backgroundColor"  , "color"  );
  model->addProperty(bgPath, this, "backgroundAlpha"  , "alpha"  );
  model->addProperty(bgPath, this, "backgroundPattern", "pattern");

  QString borderPath = path1 + "/border";

  model->addProperty(borderPath, this, "border"     , "visible");
  model->addProperty(borderPath, this, "borderColor", "color"  );
  model->addProperty(borderPath, this, "borderAlpha", "alpha"  );
  model->addProperty(borderPath, this, "borderWidth", "width"  );
}

void
CQChartsEllipseAnnotation::
setBBox(const CQChartsGeom::BBox &bbox)
{
  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  center_ += QPointF(dx, dy);

  bbox_ = bbox;
}

void
CQChartsEllipseAnnotation::
draw(QPainter *painter)
{
  double x1 = center_.x() - xRadius_;
  double y1 = center_.y() - yRadius_;
  double x2 = center_.x() + xRadius_;
  double y2 = center_.y() + yRadius_;

  bbox_ = CQChartsGeom::BBox(x1, y1, x2 - x1, y2 - y1);

  //---

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(bbox_, prect);

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

    double bw = plot_->lengthPixelWidth(borderWidth());

    pen.setWidthF(bw);

    painter->strokePath(path, pen);
  }

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsPolyAnnotation::
CQChartsPolyAnnotation(CQChartsPlot *plot, const QPolygonF &points) :
 CQChartsAnnotation(plot), points_(points)
{
  setObjectName(QString("poly.%1").arg(ind()));

  setBorder(true);

  editHandles_.setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolyAnnotation::
~CQChartsPolyAnnotation()
{
}

void
CQChartsPolyAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + QString("/polyAnnotation.%1").arg(ind());

  CQChartsAnnotation::addProperties(model, path1);

  QString bgPath = path1 + "/background";

  model->addProperty(bgPath, this, "background"       , "visible");
  model->addProperty(bgPath, this, "backgroundColor"  , "color"  );
  model->addProperty(bgPath, this, "backgroundAlpha"  , "alpha"  );
  model->addProperty(bgPath, this, "backgroundPattern", "pattern");

  QString borderPath = path1 + "/border";

  model->addProperty(borderPath, this, "border"     , "visible");
  model->addProperty(borderPath, this, "borderColor", "color"  );
  model->addProperty(borderPath, this, "borderAlpha", "alpha"  );
  model->addProperty(borderPath, this, "borderWidth", "width"  );
}

void
CQChartsPolyAnnotation::
setBBox(const CQChartsGeom::BBox &bbox)
{
  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  for (int i = 0; i < points_.length(); ++i) {
    points_[i].setX(points_[i].x() + dx);
    points_[i].setY(points_[i].x() + dy);
  }

  bbox_ = bbox;
}

void
CQChartsPolyAnnotation::
setData(const CQChartsShapeData &shape)
{
  CQChartsBoxData data;

  data.background = shape.background;
  data.border     = shape.border;

  CQChartsAnnotation::setData(data);

  emit dataChanged();
}

void
CQChartsPolyAnnotation::
draw(QPainter *painter)
{
  if (! points_.length())
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

  double px, py;

  plot_->windowToPixel(points_[0].x(), points_[0].y(), px, py);

  path.moveTo(px, py);

  for (int i = 1; i < points_.size(); ++i) {
    double px, py;

    plot_->windowToPixel(points_[i].x(), points_[i].y(), px, py);

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

    double bw = plot_->lengthPixelWidth(borderWidth());

    pen.setWidthF(bw);

    painter->strokePath(path, pen);
  }

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsTextAnnotation::
CQChartsTextAnnotation(CQChartsPlot *plot, const QPointF &position, const QString &textStr) :
 CQChartsAnnotation(plot), position_(position), textStr_(textStr)
{
  setObjectName(QString("text.%1").arg(ind()));

  CQChartsColor themeFg(CQChartsColor::Type::THEME_VALUE, 1);

  setTextColor(themeFg);
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
}

void
CQChartsTextAnnotation::
setBBox(const CQChartsGeom::BBox &bbox)
{
  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  position_ += QPointF(dx, dy);

  bbox_ = bbox;
}

void
CQChartsTextAnnotation::
draw(QPainter *painter)
{
  QFontMetricsF fm(textFont());

  double xp = plot_->pixelToWindowWidth (padding());
  double yp = plot_->pixelToWindowHeight(padding());
  double xm = plot_->pixelToWindowWidth (margin());
  double ym = plot_->pixelToWindowHeight(margin());

  double x = position_.x() - xp - xm; // bottom
  double y = position_.y() - yp - ym; // top
  double w = plot_->pixelToWindowWidth (fm.width(textStr())) + 2*xp + 2*xm;
  double h = plot_->pixelToWindowHeight(fm.height())         + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(bbox_, prect);

  CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  QColor c = textColor().interpColor(plot_, 0, 1);

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

  textOptions.angle    = textAngle();
  textOptions.contrast = isTextContrast();

  plot_->drawTextInBox(painter, trect, textStr(), pen, textOptions);

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsArrowAnnotation::
CQChartsArrowAnnotation(CQChartsPlot *plot, const QPointF &start, const QPointF &end) :
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
setBBox(const CQChartsGeom::BBox &bbox)
{
  double dx1 = bbox.getXMin() - bbox_.getXMin();
  double dy1 = bbox.getYMin() - bbox_.getYMin();
  double dx2 = bbox.getXMax() - bbox_.getXMax();
  double dy2 = bbox.getYMax() - bbox_.getYMax();

  if (start_.x() < end_.x()) {
    start_.setX(start_.x() + dx1);
    end_  .setX(end_  .x() + dx2);
  }
  else {
    start_.setX(start_.x() + dx2);
    end_  .setX(end_  .x() + dx1);
  }

  if (start_.y() < end_.y()) {
    start_.setY(start_.y() + dy1);
    end_  .setY(end_  .y() + dy2);
  }
  else {
    start_.setY(start_.y() + dy2);
    end_  .setY(end_  .y() + dy1);
  }

  bbox_ = bbox;
}

void
CQChartsArrowAnnotation::
draw(QPainter *painter)
{
  double xp = plot_->pixelToWindowWidth (padding());
  double yp = plot_->pixelToWindowHeight(padding());
  double xm = plot_->pixelToWindowWidth (margin());
  double ym = plot_->pixelToWindowHeight(margin());

  double x1 = std::min(start_.x(), end_.x());
  double y1 = std::min(start_.y(), end_.y());
  double x2 = std::max(start_.x(), end_.x());
  double y2 = std::max(start_.y(), end_.y());

  double x = x1 - xp - xm; // bottom
  double y = y1 - yp - ym; // top
  double w = (x2 - x1) + 2*xp + 2*xm;
  double h = (y2 - y1) + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(bbox_, prect);

  //CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  arrow_->setFrom(start_);
  arrow_->setTo  (end_  );

  arrow_->draw(painter);

  //---

  CQChartsAnnotation::draw(painter);
}

//---

CQChartsPointAnnotation::
CQChartsPointAnnotation(CQChartsPlot *plot, const QPointF &position,
                        const CQChartsPlotSymbol::Type &type) :
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
setBBox(const CQChartsGeom::BBox &bbox)
{
  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  position_ += QPointF(dx, dy);

  bbox_ = bbox;
}

void
CQChartsPointAnnotation::
draw(QPainter *painter)
{
  double xp = plot_->pixelToWindowWidth (padding());
  double yp = plot_->pixelToWindowHeight(padding());
  double xm = plot_->pixelToWindowWidth (margin());
  double ym = plot_->pixelToWindowHeight(margin());

  double sw = plot_->pixelToWindowWidth (pointData_.size);
  double sh = plot_->pixelToWindowHeight(pointData_.size);

  double x = position_.x() - xp - xm; // bottom
  double y = position_.y() - yp - ym; // top
  double w = sw + 2*xp + 2*xm;
  double h = sh + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(bbox_, prect);

  //CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(prect));

  //---

  double px = prect.getXMid();
  double py = prect.getYMid();

  QColor lineColor = pointData_.stroke.color.interpColor(plot_, 0, 1);
  QColor fillColor = pointData_.fill .color.interpColor(plot_, 0, 1);

  lineColor.setAlphaF(pointData_.stroke.alpha);
  fillColor.setAlphaF(pointData_.fill  .alpha);

  QPen   pen;
  QBrush brush;

  if (pointData_.stroke.visible)
    pen.setColor(lineColor);
  else
    pen.setStyle(Qt::NoPen);

  double bw = plot_->lengthPixelWidth(pointData_.stroke.width);

  pen.setWidthF(bw);

  if (pointData_.fill.visible) {
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(fillColor);
  }
  else
    brush.setStyle(Qt::NoBrush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  CQChartsSymbol2DRenderer srenderer(painter, CQChartsGeom::Point(px, py), pointData_.size);

  if (painter->brush().style() != Qt::NoBrush)
    CQChartsPlotSymbolMgr::fillSymbol(pointData_.type, &srenderer);

  if (painter->pen().style() != Qt::NoPen)
    CQChartsPlotSymbolMgr::drawSymbol(pointData_.type, &srenderer);

  //---

  CQChartsAnnotation::draw(painter);
}
