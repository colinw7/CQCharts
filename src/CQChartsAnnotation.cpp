#include <CQChartsAnnotation.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsArrow.h>
#include <CQChartsEditHandles.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQUtil.h>

const QStringList &
CQChartsAnnotation::
typeNames()
{
  static QStringList names = QStringList() <<
    "rectangle" << "ellipse" << "polygon" << "polyline" << "text" << "arrow" << "point" << "image";

  return names;
}

CQChartsAnnotation::
CQChartsAnnotation(CQChartsView *view, Type type) :
 CQChartsTextBoxObj(view), type_(type)
{
  static int s_lastInd;

  editHandles_ = new CQChartsEditHandles(view_, CQChartsEditHandles::Mode::MOVE);

  ind_ = ++s_lastInd;
}

CQChartsAnnotation::
CQChartsAnnotation(CQChartsPlot *plot, Type type) :
 CQChartsTextBoxObj(plot), type_(type)
{
  static int s_lastInd;

  editHandles_ = new CQChartsEditHandles(plot_, CQChartsEditHandles::Mode::MOVE);

  ind_ = ++s_lastInd;
}

CQChartsAnnotation::
~CQChartsAnnotation()
{
  delete editHandles_;
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
  QString id = propertyId();

  if      (plot())
    return plot()->pathId() + ":" + id;
  else if (view())
    return view()->id() + ":" + id;
  else
    return id;
}

//---

void
CQChartsAnnotation::
writeKeys(std::ostream &os, const QString &cmd, const QString &parentVarName,
          const QString &varName) const
{
  auto parentName = [&]() {
    if (parentVarName == "") {
      if      (view())
        return view()->id();
      else if (plot())
        return plot()->id();
      else
        return QString("$parent");
    }
    else
      return QString("$") + parentVarName;
  };

  auto annotationName = [&]() {
    return (varName != "" ? varName : "plot");
  };

  os << "set " << annotationName().toStdString();
  os << " [" << cmd.toStdString();

  if      (plot())
    os << " -plot " << parentName().toStdString();
  else if (view())
    os << " -view " << parentName().toStdString();

  if (id() != "")
    os << " -id " << id().toStdString();

  if (tipId() != "")
    os << " -tip \"" << tipId().toStdString() << "\"";
}

#if 0
void
CQChartsAnnotation::
writeFill(std::ostream &os) const
{
  if (isFilled()) {
    os << " -filled 1";

    if (fillColor().isValid())
      os << " -fill_color " << fillColor().toString().toStdString();

    if (fillAlpha() != 1.0)
      os << " -fill_alpha " << fillAlpha();
  }
}
#endif

#if 0
void
CQChartsAnnotation::
writeStroke(std::ostream &os) const
{
  if (isStroked()) {
    os << " -stroked 1";

    if (strokeColor().isValid())
      os << " -stroke_color " << strokeColor().toString().toStdString();

    if (strokeAlpha() != 1.0)
      os << " -stroke_alpha " << strokeAlpha();

    if (strokeWidth().isSet())
      os << " -stroke_width " << strokeWidth().toString().toStdString();

    if (! strokeDash().isSolid())
      os << " -stroke_dash " << strokeDash().toString().toStdString();
  }
}
#endif

void
CQChartsAnnotation::
writePoints(std::ostream &os, const QPolygonF &polygon) const
{
  if (polygon.size()) {
    os << " -points {";

    for (int i = 0; i < polygon.size(); ++i) {
      if (i > 0) os << " ";

      const QPointF &p1 = polygon[i];

      os << "{" << p1.x() << " " << p1.y() << "}";
    }

    os << "}";
  }
}

void
CQChartsAnnotation::
writeProperties(std::ostream &os, const QString &varName) const
{
  CQPropertyViewModel::NameValues nameValues;

  CQPropertyViewModel *propertyModel = this->propertyModel();

  if (propertyModel)
    propertyModel->getChangedNameValues(this, nameValues, /*tcl*/true);

  if (nameValues.empty())
    return;

  os << "\n";

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str = "";

    os << "set_charts_property -annotation $" << varName.toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}

//---

void
CQChartsAnnotation::
initRectangle()
{
}

//---

void
CQChartsAnnotation::
setCheckable(bool b)
{
  CQChartsUtil::testAndSet(checkable_, b, [&]() { invalidate(); } );
}

void
CQChartsAnnotation::
setChecked(bool b)
{
  CQChartsUtil::testAndSet(checked_, b, [&]() { invalidate(); } );
}

//---

void
CQChartsAnnotation::
invalidate()
{
  if      (plot()) {
    plot()->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
    plot()->invalidateLayer(CQChartsBuffer::Type::OVERLAY);
  }
  else if (view()) {
    view()->invalidateObjects();
    view()->invalidateOverlay();

    view()->update();
  }
}

void
CQChartsAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  model->setObjectRoot(path, this);

  addProp(path, "id"       , "id"       , "Annotation id");
  addProp(path, "checkable", "checkable", "Is checkable");
  addProp(path, "checked"  , "checked"  , "Is checked"  );
}

void
CQChartsAnnotation::
addStrokeFillProperties(CQPropertyViewModel *model, const QString &path)
{
  addStrokeProperties(model, path + "/stroke");
  addFillProperties  (model, path + "/fill"  );
}

void
CQChartsAnnotation::
addFillProperties(CQPropertyViewModel *model, const QString &path)
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

  addStyleProp(path, "filled"     , "visible", "Fill visible");
  addStyleProp(path, "fillColor"  , "color"  , "Fill color"  );
  addStyleProp(path, "fillAlpha"  , "alpha"  , "Fill alpha"  );
  addStyleProp(path, "fillPattern", "pattern", "Fill pattern", true);
}

void
CQChartsAnnotation::
addStrokeProperties(CQPropertyViewModel *model, const QString &path)
{
  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    CQPropertyViewItem *item = model->addProperty(path, this, name, alias);
    item->setDesc(desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  addStyleProp(path, "stroked"    , "visible"   , "Stroke visible"   );
  addStyleProp(path, "strokeColor", "color"     , "Stroke color"     );
  addStyleProp(path, "strokeAlpha", "alpha"     , "Stroke alpha"     );
  addStyleProp(path, "strokeWidth", "width"     , "Stroke width"     );
  addStyleProp(path, "cornerSize" , "cornerSize", "Box corner size"  );
  addStyleProp(path, "borderSides", "sides"     , "Box visible sides");
}

bool
CQChartsAnnotation::
getProperty(const QString &name, QVariant &value) const
{
  CQPropertyViewModel *propertyModel = this->propertyModel();
  if (! propertyModel) return false;

  return propertyModel->getProperty(this, name, value);
}

bool
CQChartsAnnotation::
getTclProperty(const QString &name, QVariant &value) const
{
  CQPropertyViewModel *propertyModel = this->propertyModel();
  if (! propertyModel) return false;

  return propertyModel->getTclProperty(this, name, value);
}

bool
CQChartsAnnotation::
getPropertyDesc(const QString &name, QString &desc, bool hidden) const
{
  const CQPropertyViewItem *item = propertyItem(name, hidden);
  if (! item) return false;

  desc = item->desc();

  return true;
}

bool
CQChartsAnnotation::
getPropertyType(const QString &name, QString &type, bool hidden) const
{
  const CQPropertyViewItem *item = propertyItem(name, hidden);
  if (! item) return false;

  type = item->typeName();

  return true;
}

bool
CQChartsAnnotation::
getPropertyUserType(const QString &name, QString &type, bool hidden) const
{
  const CQPropertyViewItem *item = propertyItem(name, hidden);
  if (! item) return false;

  type = item->userTypeName();

  return true;
}

bool
CQChartsAnnotation::
getPropertyObject(const QString &name, QObject* &object, bool hidden) const
{
  object = nullptr;

  const CQPropertyViewItem *item = propertyItem(name, hidden);
  if (! item) return false;

  object = item->object();

  return true;
}

bool
CQChartsAnnotation::
getPropertyIsHidden(const QString &name, bool &is_hidden) const
{
  is_hidden = false;

  const CQPropertyViewItem *item = propertyItem(name, /*hidden*/true);
  if (! item) return false;

  is_hidden = CQCharts::getItemIsHidden(item);

  return true;
}

bool
CQChartsAnnotation::
getPropertyIsStyle(const QString &name, bool &is_style) const
{
  is_style = false;

  const CQPropertyViewItem *item = propertyItem(name, /*hidden*/true);
  if (! item) return false;

  is_style = CQCharts::getItemIsStyle(item);

  return true;
}

bool
CQChartsAnnotation::
setProperties(const QString &properties)
{
  bool rc = true;

  CQChartsNameValues nameValues(properties);

  for (const auto &nv : nameValues.nameValues()) {
    const QString  &name  = nv.first;
    const QVariant &value = nv.second;

    if (! setProperty(name, value))
      rc = false;
  }

  return rc;
}

bool
CQChartsAnnotation::
setProperty(const QString &name, const QVariant &value)
{
  CQPropertyViewModel *propertyModel = this->propertyModel();
  if (! propertyModel) return false;

  return propertyModel->setProperty(this, name, value);
}

void
CQChartsAnnotation::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQPropertyViewModel *propertyModel = this->propertyModel();
  if (! propertyModel) return;

  propertyModel->objectNames(this, names, hidden);
}

CQPropertyViewModel *
CQChartsAnnotation::
propertyModel() const
{
  if      (plot())
    return plot()->propertyModel();
  else if (view())
    return view()->propertyModel();
  else
    return nullptr;
}

const CQPropertyViewItem *
CQChartsAnnotation::
propertyItem(const QString &name, bool hidden) const
{
  CQPropertyViewModel *propertyModel = this->propertyModel();
  if (! propertyModel) return nullptr;

  const CQPropertyViewItem *item = propertyModel->propertyItem(this, name, hidden);
  if (! item) return nullptr;

  return item;
}

//------

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

QColor
CQChartsAnnotation::
interpColor(const CQChartsColor &c, const ColorInd &ind) const
{
  if      (plot())
    return plot()->interpColor(c, ind);
  else if (view())
    return view()->interpColor(c, ind);
  else
    return charts()->interpColor(c, ind);
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
  editHandles_->setDragPos(p);

  return true;
}

bool
CQChartsAnnotation::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point &dragPos = editHandles_->dragPos();
  const CQChartsResizeSide& dragSide = editHandles_->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles_->updateBBox(dx, dy);

  if (dragSide != CQChartsResizeSide::MOVE)
    initRectangle();

  setBBox(editHandles_->bbox(), dragSide);

  editHandles_->setDragPos(p);

  invalidate();

  return true;
}

bool
CQChartsAnnotation::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_->selectInside(p);
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
  editHandles_->setDragSide(CQChartsResizeSide::MOVE);

  editHandles_->updateBBox(f.x(), f.y());

  setBBox(editHandles_->bbox(), CQChartsResizeSide::MOVE);

  invalidate();
}

//------

void
CQChartsAnnotation::
draw(CQChartsPaintDevice *)
{
}

void
CQChartsAnnotation::
drawEditHandles(QPainter *painter) const
{
  assert(view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  const_cast<CQChartsAnnotation *>(this)->editHandles_->setBBox(this->bbox());

  editHandles_->draw(painter);
}

//---

CQChartsRectangleAnnotation::
CQChartsRectangleAnnotation(CQChartsView *view, const CQChartsRect &rectangle) :
 CQChartsAnnotation(view, Type::RECT), rectangle_(rectangle)
{
  setObjectName(QString("rectangle.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsRectangleAnnotation::
CQChartsRectangleAnnotation(CQChartsPlot *plot, const CQChartsRect &rectangle) :
 CQChartsAnnotation(plot, Type::RECT), rectangle_(rectangle)
{
  setObjectName(QString("rectangle.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsRectangleAnnotation::
~CQChartsRectangleAnnotation()
{
}

void
CQChartsRectangleAnnotation::
setRectangle(const CQChartsRect &rectangle)
{
  rectangle_ = rectangle;

  emit dataChanged();
}

void
CQChartsRectangleAnnotation::
setRectangle(const CQChartsPosition &start, const CQChartsPosition &end)
{
  if (start.units() != end.units()) {
    QPointF pstart = positionToParent(start);
    QPointF pend   = positionToParent(end);

    if      (plot())
      rectangle_ = CQChartsRect(QRectF(pstart, pend), CQChartsUnits::PLOT);
    else if (view())
      rectangle_ = CQChartsRect(QRectF(pstart, pend), CQChartsUnits::VIEW);
  }
  else {
    rectangle_ = CQChartsRect(QRectF(start.p(), end.p()), start.units());
  }

  emit dataChanged();
}

CQChartsPosition
CQChartsRectangleAnnotation::
start() const
{
  QPointF p(rectangle().rect().left(), rectangle().rect().top());

  return CQChartsPosition(p, rectangle().units());
}

void
CQChartsRectangleAnnotation::
setStart(const CQChartsPosition &p)
{
  QPointF start = positionToParent(p);
  QPointF end   = positionToParent(this->end());

  if      (plot())
    rectangle_ = CQChartsRect(QRectF(start, end), CQChartsUnits::PLOT);
  else if (view())
    rectangle_ = CQChartsRect(QRectF(start, end), CQChartsUnits::VIEW);

  emit dataChanged();
}

CQChartsPosition
CQChartsRectangleAnnotation::
end() const
{
  QPointF p(rectangle().rect().right(), rectangle().rect().bottom());

  return CQChartsPosition(p, rectangle().units());
}

void
CQChartsRectangleAnnotation::
setEnd(const CQChartsPosition &p)
{
  QPointF start = positionToParent(this->start());
  QPointF end   = positionToParent(p);

  if      (plot())
    rectangle_ = CQChartsRect(QRectF(start, end), CQChartsUnits::PLOT);
  else if (view())
    rectangle_ = CQChartsRect(QRectF(start, end), CQChartsUnits::VIEW);

  emit dataChanged();
}

void
CQChartsRectangleAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "rectangle", "", "Rectangle bounding box");
  addProp(path1, "start"    , "", "Rectangle bottom left")->setHidden(true);
  addProp(path1, "end"      , "", "Rectangle top right")->setHidden(true);
  addProp(path1, "margin"   , "", "Rectangle inner margin")->setHidden(true);
  addProp(path1, "padding"  , "", "Rectangle outer padding")->setHidden(true);

  addStrokeFillProperties(model, path1);
}

QString
CQChartsRectangleAnnotation::
propertyId() const
{
  return QString("rectangleAnnotation%1").arg(ind());
}

void
CQChartsRectangleAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  QPointF start = positionToParent(this->start());
  QPointF end   = positionToParent(this->end  ());

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMax(), y2 = bbox.getYMax();

  double xp = pixelToWindowWidth (padding());
  double yp = pixelToWindowHeight(padding());
  double xm = pixelToWindowWidth (margin ());
  double ym = pixelToWindowHeight(margin ());

  x1 += xp + xm; y1 += yp + ym;
  x2 -= xp + xm; y2 -= yp + ym;

  start = QPointF(std::min(x1, x2), std::min(y1, y2));
  end   = QPointF(std::max(x1, x2), std::max(y1, y2));

  if      (plot())
    rectangle_ = CQChartsRect(QRectF(start, end), CQChartsUnits::PLOT);
  else if (view())
    rectangle_ = CQChartsRect(QRectF(start, end), CQChartsUnits::VIEW);

  bbox_ = bbox;
}

void
CQChartsRectangleAnnotation::
draw(CQChartsPaintDevice *device)
{
  // calc box
  QPointF start = positionToParent(this->start());
  QPointF end   = positionToParent(this->end  ());

  double xp = pixelToWindowWidth (padding());
  double yp = pixelToWindowHeight(padding());
  double xm = pixelToWindowWidth (margin ());
  double ym = pixelToWindowHeight(margin ());

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

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPen  (pen  , true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());
  setBrush(brush, true, bgColor, fillAlpha(), fillPattern());

  updatePenBrushState(pen, brush);

  //---

  // draw box
  CQChartsBoxObj::draw(device, bbox_.qrect(), pen, brush);

  //---

  // draw base class
  CQChartsAnnotation::draw(device);
}

void
CQChartsRectangleAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_rectangle_annotation", parentVarName, varName);

#if 0
  if (start().isSet())
    os << " -start {" << start().toString().toStdString() << "}";

  if (end().isSet())
    os << " -end {" << end().toString().toStdString() << "}";
#endif

  if (rectangle().isSet())
    os << " -rectangle {" << rectangle().toString().toStdString() << "}";

#if 0
  if (margin() != 0.0)
    os << " -margin "  << margin ();

  if (padding() != 0.0)
    os << " -padding " << padding();
#endif

#if 0
  writeFill(os);

  writeStroke(os);
#endif

#if 0
  if (cornerSize().isSet())
    os << " -corner_size " << cornerSize();

  if (! borderSides().isAll())
    os << " -border_sides " << borderSides().toString().toStdString();
#endif

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsEllipseAnnotation::
CQChartsEllipseAnnotation(CQChartsView *view, const CQChartsPosition &center,
                          const CQChartsLength &xRadius, const CQChartsLength &yRadius) :
 CQChartsAnnotation(view, Type::ELLIPSE), center_(center), xRadius_(xRadius), yRadius_(yRadius)
{
  setObjectName(QString("ellipse.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsEllipseAnnotation::
CQChartsEllipseAnnotation(CQChartsPlot *plot, const CQChartsPosition &center,
                          const CQChartsLength &xRadius, const CQChartsLength &yRadius) :
 CQChartsAnnotation(plot, Type::ELLIPSE), center_(center), xRadius_(xRadius), yRadius_(yRadius)
{
  setObjectName(QString("ellipse.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsEllipseAnnotation::
~CQChartsEllipseAnnotation()
{
}

void
CQChartsEllipseAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "center" , "", "Ellipse center point");
  addProp(path1, "xRadius", "", "Ellipse x radius");
  addProp(path1, "yRadius", "", "Ellipse y radius");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsEllipseAnnotation::
propertyId() const
{
  return QString("ellipseAnnotation%1").arg(ind());
}

void
CQChartsEllipseAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  center_ = CQChartsPosition(bbox.getCenter().qpoint());

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
  QPointF center = positionToParent(center_);

  double dx = p.getX() - center.x();
  double dy = p.getY() - center.y();

  double xr = lengthParentWidth (xRadius_);
  double yr = lengthParentHeight(yRadius_);

  double xr2 = xr*xr;
  double yr2 = yr*yr;

  return (((dx*dx)/xr2 + (dy*dy)/yr2) < 1);
}

void
CQChartsEllipseAnnotation::
draw(CQChartsPaintDevice *device)
{
  QPointF center = positionToParent(center_);

  double xr = lengthParentWidth (xRadius_);
  double yr = lengthParentHeight(yRadius_);

  double x1 = center.x() - xr;
  double y1 = center.y() - yr;
  double x2 = center.x() + xr;
  double y2 = center.y() + yr;

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  //CQChartsBoxObj::draw(device, bbox_.qrect());

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPen  (pen  , isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash());
  setBrush(brush, isFilled (), bgColor, fillAlpha(), fillPattern());

  updatePenBrushState(pen, brush);

  //---

  // create path
  QPainterPath path;

  path.addEllipse(bbox_.qrect());

  //---

  // draw path
  device->fillPath  (path, brush);
  device->strokePath(path, pen  );

  //---

  // draw base class
  CQChartsAnnotation::draw(device);
}

void
CQChartsEllipseAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_ellipse_annotation", parentVarName, varName);

  if (center().isSet())
    os << " -center {" << center().toString().toStdString() << "}";

  if (xRadius().isSet())
    os << " -rx {" << xRadius().toString().toStdString() << "}";

  if (yRadius().isSet())
    os << " -ry {" << yRadius().toString().toStdString() << "}";

#if 0
  writeFill(os);

  writeStroke(os);
#endif

#if 0
  if (cornerSize().isSet())
    os << " -corner_size " << cornerSize();

  if (! borderSides().isAll())
    os << " -border_sides " << borderSides().toString().toStdString();
#endif

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsPolygonAnnotation::
CQChartsPolygonAnnotation(CQChartsView *view, const CQChartsPolygon &polygon) :
 CQChartsAnnotation(view, Type::POLYGON), polygon_(polygon)
{
  setObjectName(QString("polygon.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolygonAnnotation::
CQChartsPolygonAnnotation(CQChartsPlot *plot, const CQChartsPolygon &polygon) :
 CQChartsAnnotation(plot, Type::POLYGON), polygon_(polygon)
{
  setObjectName(QString("polygon.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolygonAnnotation::
~CQChartsPolygonAnnotation()
{
}

void
CQChartsPolygonAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "polygon", "", "Polygon points");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsPolygonAnnotation::
propertyId() const
{
  return QString("polygonAnnotation%1").arg(ind());
}

void
CQChartsPolygonAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();
  double sx = (bbox_.getWidth () > 0 ? bbox.getWidth ()/bbox_.getWidth () : 1.0);
  double sy = (bbox_.getHeight() > 0 ? bbox.getHeight()/bbox_.getHeight() : 1.0);

  double x1 = bbox_.getXMin();
  double y1 = bbox_.getYMin();

  QPolygonF polygon = polygon_.polygon();

  for (int i = 0; i < polygon.size(); ++i) {
    polygon[i].setX(sx*(polygon[i].x() - x1) + x1 + dx);
    polygon[i].setY(sy*(polygon[i].y() - y1) + y1 + dy);
  }

  polygon_ = polygon;
  bbox_    = bbox;
}

bool
CQChartsPolygonAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  const QPolygonF &polygon = polygon_.polygon();

  return (polygon.containsPoint(p.qpoint(), Qt::OddEvenFill));
}

void
CQChartsPolygonAnnotation::
draw(CQChartsPaintDevice *device)
{
  const QPolygonF &polygon = polygon_.polygon();

  if (! polygon.size())
    return;

  double x1 = polygon[0].x();
  double y1 = polygon[0].y();
  double x2 = x1;
  double y2 = y1;

  for (int i = 1; i < polygon.size(); ++i) {
    x1 = std::min(x1, polygon[i].x());
    y1 = std::min(y1, polygon[i].y());
    x2 = std::max(x2, polygon[i].x());
    y2 = std::max(y2, polygon[i].y());
  }

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPen  (pen  , isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash());
  setBrush(brush, isFilled (), bgColor, fillAlpha(), fillPattern());

  updatePenBrushState(pen, brush);

  //---

  // create path
  QPainterPath path;

  path.moveTo(polygon[0].x(), polygon[0].y());

  for (int i = 1; i < polygon.size(); ++i)
    path.lineTo(polygon[i].x(), polygon[i].y());

  path.closeSubpath();

  //---

  // draw path
  device->fillPath  (path, brush);
  device->strokePath(path, pen  );

  //---

  // draw base class
  CQChartsAnnotation::draw(device);
}

void
CQChartsPolygonAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_polygon_annotation", parentVarName, varName);

  writePoints(os, polygon_.polygon());

#if 0
  writeFill(os);

  writeStroke(os);
#endif

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsPolylineAnnotation::
CQChartsPolylineAnnotation(CQChartsView *view, const CQChartsPolygon &polygon) :
 CQChartsAnnotation(view, Type::POLYLINE), polygon_(polygon)
{
  setObjectName(QString("polyline.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolylineAnnotation::
CQChartsPolylineAnnotation(CQChartsPlot *plot, const CQChartsPolygon &polygon) :
 CQChartsAnnotation(plot, Type::POLYLINE), polygon_(polygon)
{
  setObjectName(QString("polyline.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPolylineAnnotation::
~CQChartsPolylineAnnotation()
{
}

void
CQChartsPolylineAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "polygon", "", "Polyline points");

  addStrokeProperties(model, path1 + "/stroke");
}

QString
CQChartsPolylineAnnotation::
propertyId() const
{
  return QString("polylineAnnotation%1").arg(ind());
}

void
CQChartsPolylineAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();
  double sx = (bbox_.getWidth () > 0 ? bbox.getWidth ()/bbox_.getWidth () : 1.0);
  double sy = (bbox_.getHeight() > 0 ? bbox.getHeight()/bbox_.getHeight() : 1.0);

  double x1 = bbox_.getXMin();
  double y1 = bbox_.getYMin();

  QPolygonF polygon = polygon_.polygon();

  for (int i = 0; i < polygon.size(); ++i) {
    polygon[i].setX(sx*(polygon[i].x() - x1) + x1 + dx);
    polygon[i].setY(sy*(polygon[i].y() - y1) + y1 + dy);
  }

  polygon_ = polygon;
  bbox_    = bbox;
}

bool
CQChartsPolylineAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::Point pp = windowToPixel(p);

  const QPolygonF &polygon = polygon_.polygon();

  for (int i = 1; i < polygon.size(); ++i) {
    double x1 = polygon[i - 1].x();
    double y1 = polygon[i - 1].y();
    double x2 = polygon[i    ].x();
    double y2 = polygon[i    ].y();

    CQChartsGeom::Point pl1 = windowToPixel(CQChartsGeom::Point(x1, y1));
    CQChartsGeom::Point pl2 = windowToPixel(CQChartsGeom::Point(x2, y2));

    double d;

    if (CQChartsUtil::PointLineDistance(pp, pl1, pl2, &d) && d < 3)
      return true;
  }

  return false;
}

void
CQChartsPolylineAnnotation::
draw(CQChartsPaintDevice *device)
{
  const QPolygonF &polygon = polygon_.polygon();

  if (! polygon.size())
    return;

  double x1 = polygon[0].x();
  double y1 = polygon[0].y();
  double x2 = x1;
  double y2 = y1;

  for (int i = 1; i < polygon.size(); ++i) {
    x1 = std::min(x1, polygon[i].x());
    y1 = std::min(y1, polygon[i].y());
    x2 = std::max(x2, polygon[i].x());
    y2 = std::max(y2, polygon[i].y());
  }

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  // set pen
  QPen pen;

  QColor strokeColor = interpStrokeColor(ColorInd());

  setPen(pen, true, strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

  //---

  // create path
  QPainterPath path;

  path.moveTo(polygon[0].x(), polygon[0].y());

  for (int i = 1; i < polygon.size(); ++i)
    path.lineTo(polygon[i].x(), polygon[i].y());

  //---

  // draw path
  device->strokePath(path, pen);

  //---

  // draw base class
  CQChartsAnnotation::draw(device);
}

void
CQChartsPolylineAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_polyline_annotation", parentVarName, varName);

  writePoints(os, polygon_.polygon());

#if 0
  writeFill(os);

  writeStroke(os);
#endif

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsTextAnnotation::
CQChartsTextAnnotation(CQChartsView *view, const CQChartsPosition &position,
                       const QString &textStr) :
 CQChartsAnnotation(view, Type::TEXT)
{
  setPosition(position);

  init(textStr);
}

CQChartsTextAnnotation::
CQChartsTextAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                       const QString &textStr) :
 CQChartsAnnotation(plot, Type::TEXT)
{
  setPosition(position);

  init(textStr);
}

CQChartsTextAnnotation::
CQChartsTextAnnotation(CQChartsView *view, const CQChartsRect &rect, const QString &textStr) :
 CQChartsAnnotation(view, Type::TEXT)
{
  setRectangle(rect);

  init(textStr);
}

CQChartsTextAnnotation::
CQChartsTextAnnotation(CQChartsPlot *plot, const CQChartsRect &rect, const QString &textStr) :
 CQChartsAnnotation(plot, Type::TEXT)
{
  setRectangle(rect);

  init(textStr);
}

CQChartsTextAnnotation::
~CQChartsTextAnnotation()
{
}

void
CQChartsTextAnnotation::
init(const QString &textStr)
{
  setObjectName(QString("text.%1").arg(ind()));

  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextStr  (textStr);
  setTextColor(themeFg);

  setStroked(false);
  setFilled (true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPosition
CQChartsTextAnnotation::
positionValue() const
{
  return position_.positionOr(CQChartsPosition());
}

void
CQChartsTextAnnotation::
setPosition(const CQChartsPosition &p)
{
  setPosition(CQChartsOptPosition(p));
}

void
CQChartsTextAnnotation::
setPosition(const CQChartsOptPosition &p)
{
  position_ = p;

  positionToBBox();

  emit dataChanged();
}

CQChartsRect
CQChartsTextAnnotation::
rectangleValue() const
{
  return rectangle().rectOr(CQChartsRect());
}

void
CQChartsTextAnnotation::
setRectangle(const CQChartsRect &r)
{
  setRectangle(CQChartsOptRect(r));
}

void
CQChartsTextAnnotation::
setRectangle(const CQChartsOptRect &r)
{
  rectangle_ = r;

  rectToBBox();

  emit dataChanged();
}

void
CQChartsTextAnnotation::
rectToBBox()
{
  if (rectangle().isSet()) {
    CQChartsRect rect = this->rectangleValue();

    if (plot())
      bbox_ = CQChartsGeom::BBox(plot()->rectToPlot(rect));
    else
      bbox_ = CQChartsGeom::BBox(view()->rectToView(rect));
  }
  else
    bbox_ = CQChartsGeom::BBox();
}

void
CQChartsTextAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
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

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "position" , "position" , "Text origin");
  addProp(path1, "rectangle", "rectangle", "Text bounding box");

  QString textPath = path1 + "/text";

  addProp(textPath, "textStr", "string", "Text string");

  addStyleProp(textPath, "textData"     , "style"    , "Text style", true);
  addStyleProp(textPath, "textColor"    , "color"    , "Text color");
  addStyleProp(textPath, "textAlpha"    , "alpha"    , "Text alpha");
  addStyleProp(textPath, "textFont"     , "font"     , "Text font");
  addStyleProp(textPath, "textAngle"    , "angle"    , "Text angle");
  addStyleProp(textPath, "textContrast" , "contrast" , "Text has contrast");
  addStyleProp(textPath, "textAlign"    , "align"    , "Text align");
  addStyleProp(textPath, "textFormatted", "formatted", "Text formatted to fit in box");
  addStyleProp(textPath, "textScaled"   , "scaled"   , "Text scaled to fit box");
  addStyleProp(textPath, "textHtml"     , "html"     , "Text is HTML");

  addProp(path1, "margin" , "", "Text rectangle inner margin");
  addProp(path1, "padding", "", "Text rectangle outer padding");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsTextAnnotation::
propertyId() const
{
  return QString("textAnnotation%1").arg(ind());
}

void
CQChartsTextAnnotation::
calcTextSize(QSizeF &psize, QSizeF &wsize) const
{
  // get font
  QFont font = calcFont(textFont());

  // get text size (pixel)
  CQChartsTextOptions textOptions;

  textOptions.html = isTextHtml();

  psize = CQChartsDrawUtil::calcTextSize(textStr(), font, textOptions);

  // convert to window size
  if      (plot())
    wsize = plot()->pixelToWindowSize(psize);
  else if (view())
    wsize = view()->pixelToWindowSize(psize);
  else
    wsize = psize;
}

void
CQChartsTextAnnotation::
positionToLL(double w, double h, double &x, double &y) const
{
  QPointF p = positionToParent(positionValue());

  x = 0.0;
  y = 0.0;

  if      (textAlign() & Qt::AlignLeft)
    x = p.x();
  else if (textAlign() & Qt::AlignRight)
    x = p.x() - w;
  else
    x = p.x() - w/2;

  if      (textAlign() & Qt::AlignBottom)
    y = p.y();
  else if (textAlign() & Qt::AlignTop)
    y = p.y() - h;
  else
    y = p.y() - h/2;
}

void
CQChartsTextAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  if (rectangle().isSet()) {
    QRectF qrect = bbox.qrect();

    CQChartsRect rect;

    if      (plot())
      rect = CQChartsRect(qrect, CQChartsUnits::PLOT);
    else if (view())
      rect = CQChartsRect(qrect, CQChartsUnits::VIEW);

    setRectangle(rect);
  }
  else {
    // get padding and margin
    double xp = pixelToWindowWidth (padding());
    double yp = pixelToWindowHeight(padding());
    double xm = pixelToWindowWidth (margin ());
    double ym = pixelToWindowHeight(margin ());

    //---

    // get position from align
    double x1, y1;

    if      (textAlign() & Qt::AlignLeft)
      x1 = bbox.getXMin() + xp + xm;
    else if (textAlign() & Qt::AlignRight)
      x1 = bbox.getXMax() - xp - xm;
    else
      x1 = bbox.getXMid();

    if      (textAlign() & Qt::AlignBottom)
      y1 = bbox.getYMin() + yp + ym;
    else if (textAlign() & Qt::AlignTop)
      y1 = bbox.getYMax() - yp - ym;
    else
      y1 = bbox.getYMid();

    CQChartsGeom::Point ll(x1, y1);

    //double x2 = x1 + bbox.getWidth () - 2*xp - 2*xm;
    //double y2 = y1 + bbox.getHeight() - 2*yp - 2*ym;

    //CQChartsGeom::Point ur(x2, y2);

    //---

    CQChartsPosition position;

    if      (plot())
      position = CQChartsPosition(ll.qpoint(), CQChartsUnits::PLOT);
    else if (view())
      position = CQChartsPosition(ll.qpoint(), CQChartsUnits::VIEW);

    setPosition(position);
  }

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
draw(CQChartsPaintDevice *device)
{
  // recalculate position to bbox on draw as can change depending on pixel mapping
  if (! rectangle().isSet())
    positionToBBox();

  //---

  // draw box
  CQChartsBoxObj::draw(device, bbox_.qrect());

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor c = interpColor(textColor(), ColorInd());

  if (isCheckable() && ! isChecked())
    c = CQChartsUtil::blendColors(backgroundColor(), c, 0.5);

  setPen(pen, true, c, textAlpha());

  brush.setStyle(Qt::NoBrush);

  updatePenBrushState(pen, brush);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // set text options
  CQChartsTextOptions textOptions;

  textOptions.angle     = textAngle();
  textOptions.contrast  = isTextContrast();
  textOptions.formatted = true;
  textOptions.html      = isTextHtml();
  textOptions.clipped   = false;
  textOptions.align     = textAlign();

  adjustTextOptions(textOptions);

  //---

  // set font
  setPainterFont(device, textFont());

  //---

  // set box
  CQChartsGeom::BBox prect = windowToPixel(bbox_);

  double tx =          prect.getXMin  () +   margin() +   padding();
  double ty =          prect.getYMin  () +   margin() +   padding();
  double tw = std::max(prect.getWidth () - 2*margin() - 2*padding(), 0.0);
  double th = std::max(prect.getHeight() - 2*margin() - 2*padding(), 0.0);

  QRectF trect(tx, ty, tw, th);

  //---

  // draw text
  device->setRenderHints(QPainter::Antialiasing);

  CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(trect), textStr(), textOptions);

  //---

  // draw base class
  CQChartsAnnotation::draw(device);
}

void
CQChartsTextAnnotation::
initRectangle()
{
  // convert position to rectangle if needed
  if (! rectangle().isSet()) {
    positionToBBox();

    //---

    QRectF qrect = bbox_.qrect();

    CQChartsRect rect;

    if      (plot())
      rect = CQChartsRect(qrect, CQChartsUnits::PLOT);
    else if (view())
      rect = CQChartsRect(qrect, CQChartsUnits::VIEW);

    setRectangle(rect);
  }
}

void
CQChartsTextAnnotation::
positionToBBox()
{
  assert(! rectangle().isSet());

  double xp = pixelToWindowWidth (padding());
  double yp = pixelToWindowHeight(padding());
  double xm = pixelToWindowWidth (margin ());
  double ym = pixelToWindowHeight(margin ());

  QSizeF psize, wsize;

  calcTextSize(psize, wsize);

  double x, y;

  positionToLL(wsize.width(), wsize.height(), x, y);

  CQChartsGeom::Point ll(x                 - xp - xm, y                  - yp - ym);
  CQChartsGeom::Point ur(x + wsize.width() + xp + xm, y + wsize.height() + yp + ym);

  bbox_ = CQChartsGeom::BBox(ll, ur);
}

void
CQChartsTextAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_text_annotation", parentVarName, varName);

  if (rectangle().isSet()) {
    if (rectangleValue().isSet())
      os << " -rectangle {" << rectangleValue().toString().toStdString() << "}";
  }
  else {
    if (positionValue().isSet())
      os << " -position {" << positionValue().toString().toStdString() << "}";
  }

  if (textStr().length())
    os << " -text {" << textStr().toStdString() << "}";

  if (textFont() != view()->font())
    os << " -font {" << textFont().toString().toStdString() << "}";

  if (textColor().isValid())
    os << " -color {" << textColor().toString().toStdString() << "}";

  if (textAlpha() != 1.0)
    os << " -alpha " << textAlpha();

  if (textAngle() != 1.0)
    os << " -angle " << textAngle();

  if (isTextContrast())
    os << " -contrast 1";

  if (textAlign() != (Qt::AlignLeft | Qt::AlignVCenter))
    os << " -align {" << CQUtil::alignToString(textAlign()).toStdString() << "}";

  if (isTextHtml())
    os << " -html";

#if 0
  writeFill(os);

  writeStroke(os);
#endif

#if 0
  if (cornerSize().isSet())
    os << " -corner_size " << cornerSize();

  if (! borderSides().isAll())
    os << " -border_sides " << borderSides().toString().toStdString();
#endif

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsImageAnnotation::
CQChartsImageAnnotation(CQChartsView *view, const CQChartsPosition &position,
                       const QImage &image) :
 CQChartsAnnotation(view, Type::IMAGE)
{
  setPosition(position);

  init(image);
}

CQChartsImageAnnotation::
CQChartsImageAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                       const QImage &image) :
 CQChartsAnnotation(plot, Type::IMAGE)
{
  setPosition(position);

  init(image);
}

CQChartsImageAnnotation::
CQChartsImageAnnotation(CQChartsView *view, const CQChartsRect &rect, const QImage &image) :
 CQChartsAnnotation(view, Type::IMAGE)
{
  setRectangle(rect);

  init(image);
}

CQChartsImageAnnotation::
CQChartsImageAnnotation(CQChartsPlot *plot, const CQChartsRect &rect, const QImage &image) :
 CQChartsAnnotation(plot, Type::IMAGE)
{
  setRectangle(rect);

  init(image);
}

CQChartsImageAnnotation::
~CQChartsImageAnnotation()
{
}

void
CQChartsImageAnnotation::
init(const QImage &image)
{
  setObjectName(QString("image.%1").arg(ind()));

  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  image_         = image;
  disabledImage_ = QImage();

  image_        .setText("id", objectName());
  disabledImage_.setText("id", objectName() + "_dis");

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

CQChartsPosition
CQChartsImageAnnotation::
positionValue() const
{
  return position_.positionOr(CQChartsPosition());
}

void
CQChartsImageAnnotation::
setPosition(const CQChartsPosition &p)
{
  setPosition(CQChartsOptPosition(p));
}

void
CQChartsImageAnnotation::
setPosition(const CQChartsOptPosition &p)
{
  position_ = p;

  positionToBBox();

  emit dataChanged();
}

CQChartsRect
CQChartsImageAnnotation::
rectangleValue() const
{
  return rectangle().rectOr(CQChartsRect());
}

void
CQChartsImageAnnotation::
setRectangle(const CQChartsRect &r)
{
  setRectangle(CQChartsOptRect(r));
}

void
CQChartsImageAnnotation::
setRectangle(const CQChartsOptRect &r)
{
  rectangle_ = r;

  rectToBBox();

  emit dataChanged();
}

void
CQChartsImageAnnotation::
rectToBBox()
{
  if (rectangle().isSet()) {
    CQChartsRect rect = this->rectangleValue();

    if (plot())
      bbox_ = CQChartsGeom::BBox(plot()->rectToPlot(rect));
    else
      bbox_ = CQChartsGeom::BBox(view()->rectToView(rect));
  }
  else
    bbox_ = CQChartsGeom::BBox();
}

void
CQChartsImageAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "position" , "position" , "Image origin");
  addProp(path1, "rectangle", "rectangle", "Image bounding box");

  QString imagePath = path1 + "/image";

  addProp(imagePath, "name", "image", "Image name");

  addProp(path1, "margin" , "", "Image rectangle inner margin");
  addProp(path1, "padding", "", "Image rectangle outer padding");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsImageAnnotation::
propertyId() const
{
  return QString("imageAnnotation%1").arg(ind());
}

void
CQChartsImageAnnotation::
calcImageSize(QSizeF &psize, QSizeF &wsize) const
{
  // convert to window size
  psize = image_.size();

  if      (plot())
    wsize = plot()->pixelToWindowSize(psize);
  else if (view())
    wsize = view()->pixelToWindowSize(psize);
  else
    wsize = psize;
}

void
CQChartsImageAnnotation::
positionToLL(double w, double h, double &x, double &y) const
{
  QPointF p = positionToParent(positionValue());

  x = p.x() - w/2;
  y = p.y() - h/2;
}

void
CQChartsImageAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  if (rectangle().isSet()) {
    QRectF qrect = bbox.qrect();

    CQChartsRect rect;

    if      (plot())
      rect = CQChartsRect(qrect, CQChartsUnits::PLOT);
    else if (view())
      rect = CQChartsRect(qrect, CQChartsUnits::VIEW);

    setRectangle(rect);
  }
  else {
    // get position
    double x1 = bbox.getXMid();
    double y1 = bbox.getYMid();

    CQChartsGeom::Point ll(x1, y1);

    //---

    CQChartsPosition position;

    if      (plot())
      position = CQChartsPosition(ll.qpoint(), CQChartsUnits::PLOT);
    else if (view())
      position = CQChartsPosition(ll.qpoint(), CQChartsUnits::VIEW);

    setPosition(position);
  }

  bbox_ = bbox;
}

bool
CQChartsImageAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return CQChartsAnnotation::inside(p);
}

void
CQChartsImageAnnotation::
draw(CQChartsPaintDevice *device)
{
  // recalculate position to bbox on draw as can change depending on pixel mapping
  if (! rectangle().isSet())
    positionToBBox();

  //---

  // draw box
  CQChartsBoxObj::draw(device, bbox_.qrect());

  //---

  // set box
  CQChartsGeom::BBox prect = windowToPixel(bbox_);

  double tx =          prect.getXMin  () +   margin() +   padding();
  double ty =          prect.getYMin  () +   margin() +   padding();
  double tw = std::max(prect.getWidth () - 2*margin() - 2*padding(), 0.0);
  double th = std::max(prect.getHeight() - 2*margin() - 2*padding(), 0.0);

  QRectF trect(tx, ty, tw, th);

  //---

  // draw image
  if (isCheckable() && ! isChecked()) {
    QColor bg = backgroundColor();

    if (disabledImage_.isNull()) {
      int iw = image_.width ();
      int ih = image_.height();

      disabledImage_ = QImage(iw, ih, QImage::Format_ARGB32_Premultiplied);

      for (int y = 0; y < ih; ++y) {
        for (int x = 0; x < iw; ++x) {
          QRgb rgba = image_.pixel(x, y);

          int r = qRed  (rgba);
          int g = qGreen(rgba);
          int b = qBlue (rgba);
          int a = qAlpha(rgba);

          QColor c(r, g, b ,a);

          QColor c1 = CQChartsUtil::blendColors(bg, c, 0.5);

          c1.setAlpha(a);

          disabledImage_.setPixel(x, y, c1.rgba());
        }
      }
    }

    device->drawImageInRect(device->pixelToWindow(trect), disabledImage_);
  }
  else {
    device->drawImageInRect(device->pixelToWindow(trect), image_);
  }

  //---

  // draw base class
  CQChartsAnnotation::draw(device);
}

void
CQChartsImageAnnotation::
initRectangle()
{
  // convert position to rectangle if needed
  if (! rectangle().isSet()) {
    positionToBBox();

    //---

    QRectF qrect = bbox_.qrect();

    CQChartsRect rect;

    if      (plot())
      rect = CQChartsRect(qrect, CQChartsUnits::PLOT);
    else if (view())
      rect = CQChartsRect(qrect, CQChartsUnits::VIEW);

    setRectangle(rect);
  }
}

void
CQChartsImageAnnotation::
positionToBBox()
{
  assert(! rectangle().isSet());

  double xp = pixelToWindowWidth (padding());
  double yp = pixelToWindowHeight(padding());
  double xm = pixelToWindowWidth (margin ());
  double ym = pixelToWindowHeight(margin ());

  QSizeF psize, wsize;

  calcImageSize(psize, wsize);

  double x, y;

  positionToLL(wsize.width(), wsize.height(), x, y);

  CQChartsGeom::Point ll(x                 - xp - xm, y                  - yp - ym);
  CQChartsGeom::Point ur(x + wsize.width() + xp + xm, y + wsize.height() + yp + ym);

  bbox_ = CQChartsGeom::BBox(ll, ur);
}

void
CQChartsImageAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_image_annotation", parentVarName, varName);

  if (rectangle().isSet()) {
    if (rectangleValue().isSet())
      os << " -rectangle {" << rectangleValue().toString().toStdString() << "}";
  }
  else {
    if (positionValue().isSet())
      os << " -position {" << positionValue().toString().toStdString() << "}";
  }

  if (! image_.isNull())
    os << " -image {" << image_.text().toStdString() << "}";

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsArrowAnnotation::
CQChartsArrowAnnotation(CQChartsView *view, const CQChartsPosition &start,
                        const CQChartsPosition &end) :
 CQChartsAnnotation(view, Type::ARROW), start_(start), end_(end)
{
  setObjectName(QString("arrow.%1").arg(ind()));

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);

  arrow_ = std::make_unique<CQChartsArrow>(view);

  connect(arrow(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
}

CQChartsArrowAnnotation::
CQChartsArrowAnnotation(CQChartsPlot *plot, const CQChartsPosition &start,
                        const CQChartsPosition &end) :
 CQChartsAnnotation(plot, Type::ARROW), start_(start), end_(end)
{
  setObjectName(QString("arrow.%1").arg(ind()));

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);

  arrow_ = std::make_unique<CQChartsArrow>(plot);

  connect(arrow(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
}

CQChartsArrowAnnotation::
~CQChartsArrowAnnotation()
{
}

const CQChartsArrowData &
CQChartsArrowAnnotation::
arrowData() const
{
  return arrow()->data();
}

void
CQChartsArrowAnnotation::
setArrowData(const CQChartsArrowData &data)
{
  arrow()->setData(data);

  emit dataChanged();
}

void
CQChartsArrowAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addArrowProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    return &(model->addProperty(path, arrow(), name, alias)->setDesc(desc));
  };

  auto addArrowStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                               const QString &desc) {
    CQPropertyViewItem *item = addArrowProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "start", "", "Arrow start point");
  addProp(path1, "end"  , "", "Arrow end point");

  QString linePath = path1 + "/line";

  addArrowStyleProp(linePath, "lineWidth", "width", "Arrow connecting line width");

  QString frontHeadPath = path1 + "/frontHead";

//addArrowProp     (frontHeadPath, "frontVisible"  , "visible",
//                  "Arrow front head visible");
  addArrowProp     (frontHeadPath, "frontType"     , "type",
                    "Arrow front head type");
  addArrowStyleProp(frontHeadPath, "frontLength"   , "length",
                    "Arrow front head length");
  addArrowStyleProp(frontHeadPath, "frontAngle"    , "angle",
                    "Arrow front head angle");
//addArrowStyleProp(frontHeadPath, "frontBackAngle", "backAngle",
//                  "Arrow front head back angle");
//addArrowStyleProp(frontHeadPath, "frontLineEnds" , "line",
//                  "Arrow front head is drawn using lines");

  QString tailHeadPath = path1 + "/tailHead";

//addArrowProp     (tailHeadPath, "tailVisible"  , "visible",
//                  "Arrow tail head visible");
  addArrowProp     (tailHeadPath, "tailType"    , "type",
                    "Arrow tail head type");
  addArrowStyleProp(tailHeadPath, "tailLength"   , "length",
                    "Arrow tail head length");
  addArrowStyleProp(tailHeadPath, "tailAngle"    , "angle",
                    "Arrow tail head angle");
//addArrowStyleProp(tailHeadPath, "tailBackAngle", "backAngle",
//                  "Arrow tail head back angle");
//addArrowStyleProp(tailHeadPath, "tailLineEnds" , "line",
//                  "Arrow tail head is drawn using lines");

  QString fillPath = path1 + "/fill";

  addArrowStyleProp(fillPath, "filled"   , "visible", "Arrow fill visible");
  addArrowStyleProp(fillPath, "fillColor", "color"  , "Arrow fill color");
  addArrowStyleProp(fillPath, "fillAlpha", "alpha"  , "Arrow fill alpha");

  QString strokePath = path1 + "/stroke";

  addArrowStyleProp(strokePath, "stroked"    , "visible", "Arrow stroke visible");
  addArrowStyleProp(strokePath, "strokeColor", "color"  , "Arrow stroke color");
  addArrowStyleProp(strokePath, "strokeAlpha", "alpha"  , "Arrow stroke alpha");
  addArrowStyleProp(strokePath, "strokeWidth", "width"  , "Arrow stroke width");

#if DEBUG_LABELS
  addArrowProp(path1, "debugLabels", "debugLabels", "Show debug labels");
#endif
}

void
CQChartsArrowAnnotation::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQChartsAnnotation::getPropertyNames(names, hidden);

  names << "frontHead.visible" << "frontHead.angle" << "frontHead.backAngle" <<
           "frontHead.length" << "frontHead.line";

  names << "tailHead.visible" << "tailHead.angle" << "tailHead.backAngle" <<
           "tailHead.length" << "tailHead.line";

  names << "line.width";

  names << "fill.visible" << "fill.color" << "fill.alpha";

  names << "stroke.visible" << "stroke.color" << "stroke.alpha" << "stroke.width";

#if 0
  // can't use objectNames as root is wrong
  CQPropertyModel *propertyModel = this->propertyModel();
  if (! propertyModel) return;

  propertyModel->objectNames(arrow(), names);
#endif
}

QString
CQChartsArrowAnnotation::
propertyId() const
{
  return QString("arrowAnnotation%1").arg(ind());
}

void
CQChartsArrowAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  QPointF start = positionToParent(start_);
  QPointF end   = positionToParent(end_  );

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMax(), y2 = bbox.getYMax();

  double xp = pixelToWindowWidth (padding());
  double yp = pixelToWindowHeight(padding());
  double xm = pixelToWindowWidth (margin ());
  double ym = pixelToWindowHeight(margin ());

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
  CQChartsGeom::Point p1 = windowToPixel(p);

  return arrow()->contains(p1.qpoint());
}

void
CQChartsArrowAnnotation::
draw(CQChartsPaintDevice *device)
{
  // calc box
  QPointF start = positionToParent(start_);
  QPointF end   = positionToParent(end_  );

  double xp = pixelToWindowWidth (padding());
  double yp = pixelToWindowHeight(padding());
  double xm = pixelToWindowWidth (margin ());
  double ym = pixelToWindowHeight(margin ());

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

  //CQChartsBoxObj::draw(device, bbox_.qrect());

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor bgColor     = arrow()->interpFillColor  (ColorInd());
  QColor strokeColor = arrow()->interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor, 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPen  (pen  , arrow()->isStroked(),
           strokeColor, arrow()->strokeAlpha(), 1.0, CQChartsLineDash());
  setBrush(brush, arrow()->isFilled (),
           bgColor, arrow()->fillAlpha(), arrow()->fillPattern());

  updatePenBrushState(pen, brush);

  //---

  // draw arrow
  disconnect(arrow(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));

  arrow()->setFrom(start);
  arrow()->setTo  (end  );

  arrow()->draw(device, pen, brush);

  connect(arrow(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));

  //---

  // draw base class
  CQChartsAnnotation::draw(device);
}

void
CQChartsArrowAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_arrow_annotation", parentVarName, varName);

  // start/end points
  if (start().isSet())
    os << " -start {" << start().toString().toStdString() << "}";

  if (end().isSet())
    os << " -end {" << end().toString().toStdString() << "}";

  // line width
  if (arrow()->lineWidth().isSet() && arrow()->lineWidth().value() > 0)
    os << " -line_width {" << arrow()->lineWidth().toString().toStdString() << "}";

  // front head data
  QString fName;
  bool    fcustom = false;

  if (CQChartsArrowData::dataToName((CQChartsArrowData::HeadType) arrow()->frontType(),
                                    arrow()->isFrontLineEnds(), arrow()->isFrontVisible(),
                                    arrow()->frontAngle(), arrow()->frontBackAngle(), fName)) {
    os << " -fhead " << fName.toStdString();
  }
  else {
    fcustom = true;

    os << " -fhead 1";
  }

  // tail head data
  QString tName;
  bool    tcustom = false;

  if (CQChartsArrowData::dataToName((CQChartsArrowData::HeadType) arrow()->tailType(),
                                    arrow()->isTailLineEnds(), arrow()->isTailVisible(),
                                    arrow()->tailAngle(), arrow()->tailBackAngle(), tName)) {
    os << " -thead " << tName.toStdString();
  }
  else {
    tcustom = true;

    os << " -thead 1";
  }

  // add angles if custom
  if      (fcustom && tcustom) {
    if      (arrow()->frontAngle() != 0.0 && arrow()->tailAngle() != 0.0)
      os << " -angle {" << arrow()->frontAngle() << " " << arrow()->tailAngle() << "}";
    else if (arrow()->frontAngle() != 0.0)
      os << " -angle {" << arrow()->frontAngle() << " 0.0 }";
    else if (arrow()->tailAngle() != 0.0)
      os << " -angle {0.0 " << arrow()->tailAngle() << "}";

    if      (arrow()->frontBackAngle() >= 0.0 && arrow()->tailBackAngle() >= 0.0)
      os << " -angle {" << arrow()->frontBackAngle() << " " << arrow()->tailBackAngle() << "}";
    else if (arrow()->frontBackAngle() >= 0.0)
      os << " -angle {" << arrow()->frontBackAngle() << " -1}";
    else if (arrow()->tailBackAngle () >= 0.0)
      os << " -angle {-1 " << arrow()->tailBackAngle() << "}";
  }
  else if (fcustom) {
    if (arrow()->frontAngle() != 0.0)
      os << " -angle {" << arrow()->frontAngle() << " 0.0 }";

    if (arrow()->frontBackAngle() >= 0.0)
      os << " -angle {" << arrow()->frontBackAngle() << " -1}";
  }
  else if (tcustom) {
    if (arrow()->tailAngle() != 0.0)
      os << " -angle {0.0 " << arrow()->tailAngle() << "}";

    if (arrow()->tailBackAngle () >= 0.0)
      os << " -angle {-1 " << arrow()->tailBackAngle() << "}";
  }

  if      (arrow()->frontLength().isSet() && arrow()->tailLength().isSet())
    os << " -length {" << arrow()->frontLength().toString().toStdString() <<
                   " " << arrow()->tailLength ().toString().toStdString() << "}";
  else if (arrow()->frontLength().isSet())
    os << " -length {" << arrow()->frontLength().toString().toStdString() << " -1}";
  else if (arrow()->tailLength().isSet())
    os << " -length {-1 " << arrow()->tailLength ().toString().toStdString() << "}";

#if 0
  if (arrow()->isFilled())
    os << " -filled 1";

  if (arrow()->fillColor().isValid())
    os << " -fill_color {" << arrow()->fillColor().toString().toStdString() << "}";

  if (arrow()->isStroked())
    os << " -stroked 1";

  if (arrow()->strokeColor().isValid())
    os << " -stroke_color {" << arrow()->strokeColor().toString().toStdString() << "}";

  if (arrow()->strokeWidth().isValid())
    os << " -stroke_width {" << arrow()->strokeWidth().toString().toStdString() << "}";
#endif

  os << "]\n";

  //---

  writeProperties(os, varName);

  //---

  // write arrow properties
  CQPropertyViewModel *propertyModel = this->propertyModel();

  CQPropertyViewModel::NameValues nameValues;

  if (propertyModel)
    propertyModel->getChangedNameValues(this, arrow(), nameValues, /*tcl*/true);

  if (nameValues.empty())
    return;

  os << "\n";

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str = "";

    os << "set_charts_property -annotation $" << varName.toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}

//---

CQChartsPointAnnotation::
CQChartsPointAnnotation(CQChartsView *view, const CQChartsPosition &position,
                        const CQChartsSymbol &type) :
 CQChartsAnnotation(view, Type::POINT),
 CQChartsObjPointData<CQChartsPointAnnotation>(this),
 position_(position)
{
  setObjectName(QString("point.%1").arg(ind()));

  setSymbolType(type);
}

CQChartsPointAnnotation::
CQChartsPointAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                        const CQChartsSymbol &type) :
 CQChartsAnnotation(plot, Type::POINT),
 CQChartsObjPointData<CQChartsPointAnnotation>(this),
 position_(position)
{
  setObjectName(QString("point.%1").arg(ind()));

  setSymbolType(type);
}

CQChartsPointAnnotation::
~CQChartsPointAnnotation()
{
}

void
CQChartsPointAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
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

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "position", "", "Point position");

  QString symbolPath = path1 + "/symbol";

  addProp(symbolPath, "symbolType", "type", "Point symbol type");
  addProp(symbolPath, "symbolSize", "size", "Point symbol size");

  QString fillPath = path1 + "/fill";

  addStyleProp(fillPath, "symbolFilled"     , "visible", "Point symbol fill visible");
  addStyleProp(fillPath, "symbolFillColor"  , "color"  , "Point symbol fill color");
  addStyleProp(fillPath, "symbolFillAlpha"  , "alpha"  , "Point symbol fill alpha");
  addStyleProp(fillPath, "symbolFillPattern", "pattern", "Point symbol fill pattern", true);

  QString strokePath = path1 + "/stroke";

  addStyleProp(strokePath, "symbolStroked"    , "visible", "Point symbol stroke visible");
  addStyleProp(strokePath, "symbolStrokeColor", "color"  , "Point symbol stroke color");
  addStyleProp(strokePath, "symbolStrokeAlpha", "alpha"  , "Point symbol stroke alpha");
  addStyleProp(strokePath, "symbolStrokeWidth", "width"  , "Point symbol stroke width");
  addStyleProp(strokePath, "symbolStrokeDash" , "dash"   , "Point symbol stroke dash");
}

QString
CQChartsPointAnnotation::
propertyId() const
{
  return QString("pointAnnotation%1").arg(ind());
}

void
CQChartsPointAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  QPointF position = positionToParent(position_);

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
draw(CQChartsPaintDevice *device)
{
  const CQChartsSymbolData &symbolData = this->symbolData();

  QPointF position = positionToParent(position_);

  double xp = pixelToWindowWidth (padding());
  double yp = pixelToWindowHeight(padding());
  double xm = pixelToWindowWidth (margin ());
  double ym = pixelToWindowHeight(margin ());

  double sw = lengthParentWidth (symbolData.size());
  double sh = lengthParentHeight(symbolData.size());

  double x = position.x() - xp - xm; // bottom
  double y = position.y() - yp - ym; // top
  double w = sw + 2*xp + 2*xm;
  double h = sh + 2*yp + 2*ym;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  //CQChartsBoxObj::draw(device, bbox_.qrect());

  //---

  double px = bbox_.getXMid();
  double py = bbox_.getYMid();

  //---

  const CQChartsStrokeData &strokeData = symbolData.stroke();
  const CQChartsFillData   &fillData   = symbolData.fill();

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor lineColor = interpColor(strokeData.color(), ColorInd());
  QColor fillColor = interpColor(fillData  .color(), ColorInd());

  if (isCheckable() && ! isChecked()) {
    lineColor = CQChartsUtil::blendColors(backgroundColor(), lineColor, 0.5);
    fillColor = CQChartsUtil::blendColors(backgroundColor(), fillColor, 0.5);
  }

  setPen  (pen  , strokeData.isVisible(), lineColor, strokeData.alpha(),
           strokeData.width(), strokeData.dash());
  setBrush(brush, fillData.isVisible(), fillColor, fillData.alpha(), fillData.pattern());

  updatePenBrushState(pen, brush, CQChartsPlot::DrawType::SYMBOL);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // draw symbol
  QPointF ps(px, py);

  CQChartsDrawUtil::drawSymbol(device, symbolData.type(), ps, symbolData.size());

  //---

  // draw base class
  CQChartsAnnotation::draw(device);
}

void
CQChartsPointAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_point_annotation", parentVarName, varName);

  const CQChartsSymbolData &symbolData = this->symbolData();

  if (position().isSet())
    os << " -position {" << position().toString().toStdString() << "}";

  if (symbolType() != CQChartsSymbol::Type::NONE)
    os << " -type {" << symbolType().toString().toStdString() << "}";

  if (symbolData.size().isSet())
    os << " -size {" << symbolData.size().toString().toStdString() << "}";

#if 0
  if (isFilled())
    os << " -filled 1";

  if (fillColor().isValid())
    os << " -fill_color {" << fillColor().toString().toStdString() << "}";

  if (fillAlpha() != 1.0)
    os << " -fill_alpha " << fillAlpha();

  if (isStroked())
    os << " -stroked 1";

  if (strokeColor().isValid())
    os << " -stroke_color {" << strokeColor().toString().toStdString() << "}";

  if (strokeAlpha() != 1.0)
    os << " -stroke_alpha " << strokeAlpha();

  if (strokeWidth().isSet())
    os << " -stroke_width {" << strokeWidth().toString().toStdString() << "}";
#endif

  os << "]\n";

  //---

  writeProperties(os, varName);
}
