#include <CQChartsAnnotation.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsArrow.h>
#include <CQChartsEditHandles.h>
#include <CQChartsSmooth.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsArcData.h>
#include <CQChartsDensity.h>
#include <CQChartsBivariateDensity.h>
#include <CQChartsFitData.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQUtil.h>

#include <QStylePainter>

const QStringList &
CQChartsAnnotation::
typeNames()
{
  static QStringList names = QStringList() <<
    "rectangle" << "ellipse" << "polygon" << "polyline" << "text" << "image" << "arrow" <<
    "point" << "pie_slice" << "axis" << "key" << "point_set" << "value_set" << "button";

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

    if (fillAlpha() != CQChartsAlpha())
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

    if (strokeAlpha() != CQChartsAlpha())
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
writePoints(std::ostream &os, const CQChartsGeom::Polygon &polygon) const
{
  if (polygon.size()) {
    os << " -points {";

    for (int i = 0; i < polygon.size(); ++i) {
      if (i > 0) os << " ";

      auto p1 = polygon.point(i);

      os << "{" << p1.x << " " << p1.y << "}";
    }

    os << "}";
  }
}

void
CQChartsAnnotation::
writeProperties(std::ostream &os, const QString &varName) const
{
  CQPropertyViewModel::NameValues nameValues;

  auto *propertyModel = this->propertyModel();

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
setEnabled(bool b)
{
  CQChartsUtil::testAndSet(enabled_, b, [&]() { invalidate(); } );
}

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
    plot()->invalidateLayer(CQChartsBuffer::Type::MIDDLE);
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
  addProp(path, "enabled"  , "enabled"  , "Is enabled"   );
  addProp(path, "checkable", "checkable", "Is checkable" );
  addProp(path, "checked"  , "checked"  , "Is checked"   );
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
    auto *item = addProp(path, name, alias, desc);
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
    auto *item = model->addProperty(path, this, name, alias);
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
  auto *propertyModel = this->propertyModel();
  if (! propertyModel) return false;

  return propertyModel->getProperty(this, name, value);
}

bool
CQChartsAnnotation::
getTclProperty(const QString &name, QVariant &value) const
{
  auto *propertyModel = this->propertyModel();
  if (! propertyModel) return false;

  return propertyModel->getTclProperty(this, name, value);
}

bool
CQChartsAnnotation::
getPropertyDesc(const QString &name, QString &desc, bool hidden) const
{
  const auto *item = propertyItem(name, hidden);
  if (! item) return false;

  desc = item->desc();

  return true;
}

bool
CQChartsAnnotation::
getPropertyType(const QString &name, QString &type, bool hidden) const
{
  const auto *item = propertyItem(name, hidden);
  if (! item) return false;

  type = item->typeName();

  return true;
}

bool
CQChartsAnnotation::
getPropertyUserType(const QString &name, QString &type, bool hidden) const
{
  const auto *item = propertyItem(name, hidden);
  if (! item) return false;

  type = item->userTypeName();

  return true;
}

bool
CQChartsAnnotation::
getPropertyObject(const QString &name, QObject* &object, bool hidden) const
{
  object = nullptr;

  const auto *item = propertyItem(name, hidden);
  if (! item) return false;

  object = item->object();

  return true;
}

bool
CQChartsAnnotation::
getPropertyIsHidden(const QString &name, bool &is_hidden) const
{
  is_hidden = false;

  const auto *item = propertyItem(name, /*hidden*/true);
  if (! item) return false;

  is_hidden = CQCharts::getItemIsHidden(item);

  return true;
}

bool
CQChartsAnnotation::
getPropertyIsStyle(const QString &name, bool &is_style) const
{
  is_style = false;

  const auto *item = propertyItem(name, /*hidden*/true);
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
  auto *propertyModel = this->propertyModel();
  if (! propertyModel) return false;

  return propertyModel->setProperty(this, name, value);
}

void
CQChartsAnnotation::
getPropertyNames(QStringList &names, bool hidden) const
{
  auto *propertyModel = this->propertyModel();
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
  auto *propertyModel = this->propertyModel();
  if (! propertyModel) return nullptr;

  const auto *item = propertyModel->propertyItem(this, name, hidden);
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
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  if (! isEnabled())
    return false;

  emit pressed(id());

  return id().length();
}

//------

bool
CQChartsAnnotation::
editPress(const CQChartsGeom::Point &p)
{
  if (! isEnabled())
    return false;

  editHandles_->setDragPos(p);

  return true;
}

bool
CQChartsAnnotation::
editMove(const CQChartsGeom::Point &p)
{
  const auto &dragPos  = editHandles_->dragPos();
  const auto &dragSide = editHandles_->dragSide();

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
editMoveBy(const CQChartsGeom::Point &f)
{
  editHandles_->setDragSide(CQChartsResizeSide::MOVE);

  editHandles_->updateBBox(f.x, f.y);

  setBBox(editHandles_->bbox(), CQChartsResizeSide::MOVE);

  invalidate();
}

//------

void
CQChartsAnnotation::
draw(CQChartsPaintDevice *device)
{
  drawInit(device);
  drawTerm(device);
}

void
CQChartsAnnotation::
drawInit(CQChartsPaintDevice *device)
{
  if (device->type() == CQChartsPaintDevice::Type::SVG) {
    auto *painter = dynamic_cast<CQChartsSVGPaintDevice *>(device);

    CQChartsSVGPaintDevice::GroupData groupData;

    groupData.onclick   = true;
    groupData.clickProc = "annotationClick";

    painter->startGroup(id(), groupData);
  }
}

void
CQChartsAnnotation::
drawTerm(CQChartsPaintDevice *device)
{
  if (device->type() == CQChartsPaintDevice::Type::SVG) {
    auto *painter = dynamic_cast<CQChartsSVGPaintDevice *>(device);

    painter->endGroup();
  }
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
  init();
}

CQChartsRectangleAnnotation::
CQChartsRectangleAnnotation(CQChartsPlot *plot, const CQChartsRect &rectangle) :
 CQChartsAnnotation(plot, Type::RECT), rectangle_(rectangle)
{
  init();
}

CQChartsRectangleAnnotation::
~CQChartsRectangleAnnotation()
{
}

void
CQChartsRectangleAnnotation::
init()
{
  assert(rectangle_.isValid());

  setObjectName(QString("rectangle.%1").arg(ind()));

  setMargin(CQChartsMargin());

  setStroked(true);
  setFilled (true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

void
CQChartsRectangleAnnotation::
setRectangle(const CQChartsRect &rectangle)
{
  assert(rectangle.isValid());

  rectangle_ = rectangle;

  emit dataChanged();
}

void
CQChartsRectangleAnnotation::
setRectangle(const CQChartsPosition &start, const CQChartsPosition &end)
{
  CQChartsRect rectangle = CQChartsViewPlotObj::makeRect(view(), plot(), start, end);

  setRectangle(rectangle);
}

CQChartsPosition
CQChartsRectangleAnnotation::
start() const
{
  if (! rectangle().bbox().isValid())
    return CQChartsPosition();

  CQChartsGeom::Point p(rectangle().bbox().getXMin(), rectangle().bbox().getYMin());

  return CQChartsPosition(p, rectangle().units());
}

void
CQChartsRectangleAnnotation::
setStart(const CQChartsPosition &p)
{
  auto start = positionToParent(p);
  auto end   = positionToParent(this->end());

  if      (plot())
    rectangle_ = CQChartsRect(CQChartsGeom::BBox(start, end), CQChartsUnits::PLOT);
  else if (view())
    rectangle_ = CQChartsRect(CQChartsGeom::BBox(start, end), CQChartsUnits::VIEW);

  assert(rectangle_.isValid());

  emit dataChanged();
}

CQChartsPosition
CQChartsRectangleAnnotation::
end() const
{
  if (! rectangle().bbox().isValid())
    return CQChartsPosition();

  CQChartsGeom::Point p(rectangle().bbox().getXMax(), rectangle().bbox().getYMax());

  return CQChartsPosition(p, rectangle().units());
}

void
CQChartsRectangleAnnotation::
setEnd(const CQChartsPosition &p)
{
  auto start = positionToParent(this->start());
  auto end   = positionToParent(p);

  if      (plot())
    rectangle_ = CQChartsRect(CQChartsGeom::BBox(start, end), CQChartsUnits::PLOT);
  else if (view())
    rectangle_ = CQChartsRect(CQChartsGeom::BBox(start, end), CQChartsUnits::VIEW);

  assert(rectangle_.isValid());

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
  auto start = positionToParent(this->start());
  auto end   = positionToParent(this->end  ());

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMax(), y2 = bbox.getYMax();

  // external margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  x1 -= xlm; y1 -= ybm;
  x2 += xrm; y2 += ytm;

  start = CQChartsGeom::Point(std::min(x1, x2), std::min(y1, y2));
  end   = CQChartsGeom::Point(std::max(x1, x2), std::max(y1, y2));

  if      (plot())
    rectangle_ = CQChartsRect(CQChartsGeom::BBox(start, end), CQChartsUnits::PLOT);
  else if (view())
    rectangle_ = CQChartsRect(CQChartsGeom::BBox(start, end), CQChartsUnits::VIEW);

  assert(rectangle_.isValid());

  bbox_ = bbox;
}

void
CQChartsRectangleAnnotation::
draw(CQChartsPaintDevice *device)
{
  drawInit(device);

  //---

  // calc box
  auto start = positionToParent(this->start());
  auto end   = positionToParent(this->end  ());

  // external margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  double x1 = std::min(start.x, end.x);
  double y1 = std::min(start.y, end.y);
  double x2 = std::max(start.x, end.x);
  double y2 = std::max(start.y, end.y);

  double x = x1 + xlm; // left
  double y = y1 + ybm; // bottom
  double w = (x2 - x1) - xlm - xrm;
  double h = (y2 - y1) - ytm - ybm;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    CQChartsBrushData(isFilled (), bgColor, fillAlpha(), fillPattern()));

  updatePenBrushState(penBrush);

  //---

  // draw box
  //CQChartsBoxObj::draw(device, bbox_, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawRoundedPolygon(device, bbox_, cornerSize(), borderSides());

  //---

  drawTerm(device);
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
  if (margin().isValid())
    os << " -margin " << margin().toString();
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
  init();
}

CQChartsEllipseAnnotation::
CQChartsEllipseAnnotation(CQChartsPlot *plot, const CQChartsPosition &center,
                          const CQChartsLength &xRadius, const CQChartsLength &yRadius) :
 CQChartsAnnotation(plot, Type::ELLIPSE), center_(center), xRadius_(xRadius), yRadius_(yRadius)
{
  init();
}

CQChartsEllipseAnnotation::
~CQChartsEllipseAnnotation()
{
}

void
CQChartsEllipseAnnotation::
init()
{
  assert(xRadius_.value() > 0.0 && yRadius_.value() > 0.0);

  setObjectName(QString("ellipse.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
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
  center_ = CQChartsPosition(bbox.getCenter());

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  assert(w > 0.0 && h > 0.0);

  xRadius_ = w/2;
  yRadius_ = h/2;

  bbox_ = bbox;
}

bool
CQChartsEllipseAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  auto center = positionToParent(center_);

  double dx = p.getX() - center.x;
  double dy = p.getY() - center.y;

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
  drawInit(device);

  //---

  auto center = positionToParent(center_);

  double xr = lengthParentWidth (xRadius_);
  double yr = lengthParentHeight(yRadius_);

  double x1 = center.x - xr;
  double y1 = center.y - yr;
  double x2 = center.x + xr;
  double y2 = center.y + yr;

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    CQChartsBrushData(isFilled (), bgColor, fillAlpha(), fillPattern()));

  updatePenBrushState(penBrush);

  //---

  // create path
  QPainterPath path;

  path.addEllipse(bbox_.qrect());

  //---

  // draw path
  device->fillPath  (path, penBrush.brush);
  device->strokePath(path, penBrush.pen  );

  //---

  drawTerm(device);
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
  init();
}

CQChartsPolygonAnnotation::
CQChartsPolygonAnnotation(CQChartsPlot *plot, const CQChartsPolygon &polygon) :
 CQChartsAnnotation(plot, Type::POLYGON), polygon_(polygon)
{
  init();
}

CQChartsPolygonAnnotation::
~CQChartsPolygonAnnotation()
{
  delete smooth_;
}

void
CQChartsPolygonAnnotation::
init()
{
  assert(polygon_.isValid(/*closed*/true));

  setObjectName(QString("polygon.%1").arg(ind()));

  setStroked(true);
  setFilled (true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

void
CQChartsPolygonAnnotation::
setRoundedLines(bool b)
{
  CQChartsUtil::testAndSet(roundedLines_, b, [&]() { invalidate(); } );
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

  addProp(path1, "polygon"     , ""       , "Polygon points");
  addProp(path1, "roundedLines", "rounded", "Smooth lines");

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

  auto poly = polygon_.polygon();

  for (int i = 0; i < poly.size(); ++i) {
    double x = sx*(poly.point(i).x - x1) + x1 + dx;
    double y = sy*(poly.point(i).y - y1) + y1 + dy;

    poly.setPoint(i, CQChartsGeom::Point(x, y));
  }

  CQChartsPolygon polygon(poly);

  if (polygon.isValid(/*closed*/true)) {
    polygon_ = polygon;
    bbox_    = bbox;
  }
}

//---

bool
CQChartsPolygonAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  const auto &polygon = polygon_.polygon();

  return (polygon.containsPoint(p, Qt::OddEvenFill));
}

void
CQChartsPolygonAnnotation::
draw(CQChartsPaintDevice *device)
{
  const auto &polygon = polygon_.polygon();
  if (! polygon.size()) return;

  //---

  drawInit(device);

  //---

  // calc bbox
  double x1 = polygon.point(0).x;
  double y1 = polygon.point(0).y;
  double x2 = x1;
  double y2 = y1;

  for (int i = 1; i < polygon.size(); ++i) {
    x1 = std::min(x1, polygon.point(i).x);
    y1 = std::min(y1, polygon.point(i).y);
    x2 = std::max(x2, polygon.point(i).x);
    y2 = std::max(y2, polygon.point(i).y);
  }

  bbox_ = CQChartsGeom::BBox(x1, y1, x2, y2);

  //---

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    CQChartsBrushData(isFilled (), bgColor, fillAlpha(), fillPattern()));

  updatePenBrushState(penBrush);

  //---

  // create path
  QPainterPath path;

  if (isRoundedLines()) {
    initSmooth();

    // draw path
    path = smooth_->createPath(/*closed*/true);
  }
  else {
    path = CQChartsDrawUtil::polygonToPath(polygon, /*closed*/true);
  }

  //---

  // draw filled path
  device->fillPath  (path, penBrush.brush);
  device->strokePath(path, penBrush.pen  );

  //---

  drawTerm(device);
}

void
CQChartsPolygonAnnotation::
initSmooth() const
{
  // init smooth if needed
  if (! smooth_) {
    auto *th = const_cast<CQChartsPolygonAnnotation *>(this);

    const auto &polygon = polygon_.polygon();

    th->smooth_ = new CQChartsSmooth(polygon, /*sorted*/false);
  }
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
  init();
}

CQChartsPolylineAnnotation::
CQChartsPolylineAnnotation(CQChartsPlot *plot, const CQChartsPolygon &polygon) :
 CQChartsAnnotation(plot, Type::POLYLINE), polygon_(polygon)
{
  init();
}

CQChartsPolylineAnnotation::
~CQChartsPolylineAnnotation()
{
  delete smooth_;
}

void
CQChartsPolylineAnnotation::
init()
{
  assert(polygon_.isValid(/*closed*/false));

  setObjectName(QString("polyline.%1").arg(ind()));

  setStroked(true);

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

void
CQChartsPolylineAnnotation::
setRoundedLines(bool b)
{
  CQChartsUtil::testAndSet(roundedLines_, b, [&]() { invalidate(); } );
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

  addProp(path1, "polygon"     , ""       , "Polyline points");
  addProp(path1, "roundedLines", "rounded", "Smooth lines");

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

  auto qpoly = polygon_.polygon();

  for (int i = 0; i < qpoly.size(); ++i) {
    double x = sx*(qpoly.point(i).x - x1) + x1 + dx;
    double y = sy*(qpoly.point(i).y - y1) + y1 + dy;

    qpoly.setPoint(i, CQChartsGeom::Point(x, y));
  }

  CQChartsPolygon polygon(qpoly);

  if (polygon.isValid(/*closed*/false)) {
    polygon_ = polygon;
    bbox_    = bbox;
  }
}

//---

bool
CQChartsPolylineAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  const auto &polygon = polygon_.polygon();

  if (! polygon.size())
    return false;

  auto pp = windowToPixel(p);

  auto pl1 = windowToPixel(polygon.point(0));

  for (int i = 1; i < polygon.size(); ++i) {
    auto pl2 = windowToPixel(polygon.point(i));

    double d;

    if (CQChartsUtil::PointLineDistance(pp, pl1, pl2, &d) && d < 3)
      return true;

    pl1 = pl2;
  }

  return false;
}

void
CQChartsPolylineAnnotation::
draw(CQChartsPaintDevice *device)
{
  const auto &polygon = polygon_.polygon();
  if (! polygon.size()) return;

  //---

  drawInit(device);

  //---

  // calc bbox
  double x1 = polygon.point(0).x;
  double y1 = polygon.point(0).y;
  double x2 = x1;
  double y2 = y1;

  for (int i = 1; i < polygon.size(); ++i) {
    x1 = std::min(x1, polygon.point(i).x);
    y1 = std::min(y1, polygon.point(i).y);
    x2 = std::max(x2, polygon.point(i).x);
    y2 = std::max(y2, polygon.point(i).y);
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

  if (isRoundedLines()) {
    initSmooth();

    // draw path
    path = smooth_->createPath(/*closed*/false);
  }
  else {
    path = CQChartsDrawUtil::polygonToPath(polygon, /*closed*/false);
  }

  //---

  // draw path
  device->strokePath(path, pen);

  //---

  drawTerm(device);
}

void
CQChartsPolylineAnnotation::
initSmooth() const
{
  // init smooth if needed
  if (! smooth_) {
    auto *th = const_cast<CQChartsPolylineAnnotation *>(this);

    const auto &polygon = polygon_.polygon();

    th->smooth_ = new CQChartsSmooth(polygon, /*sorted*/false);
  }
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
  if (r.isSet())
    assert(r.rect().isValid());

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
      bbox_ = plot()->rectToPlot(rect);
    else
      bbox_ = view()->rectToView(rect);
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
    auto *item = addProp(path, name, alias, desc);
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

  addProp(path1, "padding", "", "Text rectangle inner padding");
  addProp(path1, "margin" , "", "Text rectangle outer margin");

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
calcTextSize(CQChartsGeom::Size &psize, CQChartsGeom::Size &wsize) const
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
  auto p = positionToParent(positionValue());

  x = 0.0;
  y = 0.0;

  if      (textAlign() & Qt::AlignLeft)
    x = p.x;
  else if (textAlign() & Qt::AlignRight)
    x = p.x - w;
  else
    x = p.x - w/2;

  if      (textAlign() & Qt::AlignBottom)
    y = p.y;
  else if (textAlign() & Qt::AlignTop)
    y = p.y - h;
  else
    y = p.y - h/2;
}

void
CQChartsTextAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  if (rectangle().isSet()) {
    CQChartsRect rect;

    if      (plot())
      rect = CQChartsRect(bbox, CQChartsUnits::PLOT);
    else if (view())
      rect = CQChartsRect(bbox, CQChartsUnits::VIEW);

    setRectangle(rect);
  }
  else {
    // get inner padding
    double xlp = lengthParentWidth (padding().left  ());
    double xrp = lengthParentWidth (padding().right ());
    double ytp = lengthParentHeight(padding().top   ());
    double ybp = lengthParentHeight(padding().bottom());

    // get outer margin
    double xlm = lengthParentWidth (margin().left  ());
    double xrm = lengthParentWidth (margin().right ());
    double ytm = lengthParentHeight(margin().top   ());
    double ybm = lengthParentHeight(margin().bottom());

    //---

    // get position from align
    double x1, y1;

    if      (textAlign() & Qt::AlignLeft)
      x1 = bbox.getXMin() + xlp + xlm;
    else if (textAlign() & Qt::AlignRight)
      x1 = bbox.getXMax() - xrp - xrm;
    else
      x1 = bbox.getXMid();

    if      (textAlign() & Qt::AlignBottom)
      y1 = bbox.getYMin() + ybp + ybm;
    else if (textAlign() & Qt::AlignTop)
      y1 = bbox.getYMax() - ytp - ytm;
    else
      y1 = bbox.getYMid();

    CQChartsGeom::Point ll(x1, y1);

    //double x2 = x1 + bbox.getWidth () - xlp - xrp - xlm - xrm;
    //double y2 = y1 + bbox.getHeight() - ybp - ytp - ybm - xtm;

    //CQChartsGeom::Point ur(x2, y2);

    //---

    CQChartsPosition position;

    if      (plot())
      position = CQChartsPosition(ll, CQChartsUnits::PLOT);
    else if (view())
      position = CQChartsPosition(ll, CQChartsUnits::VIEW);

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
  drawInit(device);

  //---

  // recalculate position to bbox on draw as can change depending on pixel mapping
  if (! rectangle().isSet())
    positionToBBox();

  if (! bbox_.isValid())
    return;

  //---

  // set rect pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isEnabled()) {
    if (isCheckable() && ! isChecked()) {
      bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
      strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
    }
  }
  else {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.8);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.8);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    CQChartsBrushData(isFilled (), bgColor, fillAlpha(), fillPattern()));

  if (isEnabled())
    updatePenBrushState(penBrush);

  //---

  // draw box
  //CQChartsBoxObj::draw(device, bbox_, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawRoundedPolygon(device, bbox_, cornerSize(), borderSides());

  //---

  // set text pen and brush
  QColor c = interpColor(textColor(), ColorInd());

  if (isEnabled()) {
    if (isCheckable() && ! isChecked())
      c = CQChartsUtil::blendColors(backgroundColor(), c, 0.5);
  }
  else {
    c = CQChartsUtil::blendColors(backgroundColor(), c, 0.8);
  }

  setPen(penBrush.pen, true, c, textAlpha());

  penBrush.brush.setStyle(Qt::NoBrush);

  if (isEnabled())
    updatePenBrushState(penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // set text options
  CQChartsTextOptions textOptions;

  textOptions.angle         = textAngle();
  textOptions.align         = textAlign();
  textOptions.contrast      = isTextContrast();
  textOptions.contrastAlpha = textContrastAlpha();
  textOptions.formatted     = isTextFormatted();
  textOptions.scaled        = isTextScaled();
  textOptions.html          = isTextHtml();
  textOptions.clipped       = false;

  adjustTextOptions(textOptions);

  //---

  // set font
  setPainterFont(device, textFont());

  //---

  // set box
//auto pbbox = windowToPixel(bbox_);

  // get inner padding
  double xlp = lengthParentWidth (padding().left  ());
  double xrp = lengthParentWidth (padding().right ());
  double ytp = lengthParentHeight(padding().top   ());
  double ybp = lengthParentHeight(padding().bottom());

  // get outer margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  double tx =          bbox_.getXMin  () +       xlm + xlp;
  double ty =          bbox_.getYMin  () +       ybm + ybp;
  double tw = std::max(bbox_.getWidth () - xlm - xrm - xlp - xrp, 0.0);
  double th = std::max(bbox_.getHeight() - ybm - ytm - ybp - ytp, 0.0);

  CQChartsGeom::BBox tbbox(tx, ty, tx + tw, ty + th);

  //---

  // draw text
  if (tbbox.isValid()) {
    device->setRenderHints(QPainter::Antialiasing);

    CQChartsDrawUtil::drawTextInBox(device, tbbox, textStr(), textOptions);
  }

  //---

  drawTerm(device);
}

void
CQChartsTextAnnotation::
initRectangle()
{
  // convert position to rectangle if needed
  if (! rectangle().isSet()) {
    positionToBBox();

    //---

    CQChartsRect rect;

    if      (plot())
      rect = CQChartsRect(bbox_, CQChartsUnits::PLOT);
    else if (view())
      rect = CQChartsRect(bbox_, CQChartsUnits::VIEW);

    setRectangle(rect);
  }
}

void
CQChartsTextAnnotation::
positionToBBox()
{
  assert(! rectangle().isSet());

  // get inner padding
  double xlp = lengthParentWidth (padding().left  ());
  double xrp = lengthParentWidth (padding().right ());
  double ytp = lengthParentHeight(padding().top   ());
  double ybp = lengthParentHeight(padding().bottom());

  // get outer margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  CQChartsGeom::Size psize, wsize;

  calcTextSize(psize, wsize);

  double x, y;

  positionToLL(wsize.width(), wsize.height(), x, y);

  CQChartsGeom::Point ll(x                 - xlp - xlm, y                  - ybp - ybm);
  CQChartsGeom::Point ur(x + wsize.width() + xrp + xrm, y + wsize.height() + ytp + ytm);

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

  if (textAlpha() != CQChartsAlpha())
    os << " -alpha " << textAlpha().value();

  if (textAngle() != CQChartsAngle())
    os << " -angle " << textAngle().value();

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
                       const CQChartsImage &image) :
 CQChartsAnnotation(view, Type::IMAGE)
{
  setPosition(position);

  init(image);
}

CQChartsImageAnnotation::
CQChartsImageAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                       const CQChartsImage &image) :
 CQChartsAnnotation(plot, Type::IMAGE)
{
  setPosition(position);

  init(image);
}

CQChartsImageAnnotation::
CQChartsImageAnnotation(CQChartsView *view, const CQChartsRect &rect, const CQChartsImage &image) :
 CQChartsAnnotation(view, Type::IMAGE)
{
  setRectangle(rect);

  init(image);
}

CQChartsImageAnnotation::
CQChartsImageAnnotation(CQChartsPlot *plot, const CQChartsRect &rect, const CQChartsImage &image) :
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
init(const CQChartsImage &image)
{
  setObjectName(QString("image.%1").arg(ind()));

  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  image_         = image;
  disabledImage_ = CQChartsImage();

  image_        .setId(objectName());
  disabledImage_.setId(objectName() + "_dis");

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
  if (r.isSet())
    assert(r.rect().isValid());

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
      bbox_ = plot()->rectToPlot(rect);
    else
      bbox_ = view()->rectToView(rect);
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

  addProp(path1, "padding", "", "Image rectangle inner padding");
  addProp(path1, "margin" , "", "Image rectangle outer margin");

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
calcImageSize(CQChartsGeom::Size &psize, CQChartsGeom::Size &wsize) const
{
  // convert to window size
  psize = CQChartsGeom::Size(image_.size());

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
  auto p = positionToParent(positionValue());

  x = p.x - w/2;
  y = p.y - h/2;
}

void
CQChartsImageAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  if (rectangle().isSet()) {
    CQChartsRect rect;

    if      (plot())
      rect = CQChartsRect(bbox, CQChartsUnits::PLOT);
    else if (view())
      rect = CQChartsRect(bbox, CQChartsUnits::VIEW);

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
      position = CQChartsPosition(ll, CQChartsUnits::PLOT);
    else if (view())
      position = CQChartsPosition(ll, CQChartsUnits::VIEW);

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
  drawInit(device);

  //---

  // recalculate position to bbox on draw as can change depending on pixel mapping
  if (! rectangle().isSet())
    positionToBBox();

  if (! bbox_.isValid())
    return;

  //---

  // set rect pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    CQChartsBrushData(isFilled (), bgColor, fillAlpha(), fillPattern()));

  updatePenBrushState(penBrush);

  //---

  // draw box
  //CQChartsBoxObj::draw(device, bbox_, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawRoundedPolygon(device, bbox_, cornerSize(), borderSides());

  //---

  // set box
//auto pbbox = windowToPixel(bbox_);

  // get inner padding
  double xlp = lengthParentWidth (padding().left  ());
  double xrp = lengthParentWidth (padding().right ());
  double ytp = lengthParentHeight(padding().top   ());
  double ybp = lengthParentHeight(padding().bottom());

  // get outer margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  double tx =          bbox_.getXMin  () +       xlm + xlp;
  double ty =          bbox_.getYMin  () +       ybm + ybp;
  double tw = std::max(bbox_.getWidth () - xlm - xrm - xlp - xrp, 0.0);
  double th = std::max(bbox_.getHeight() - ybm - ytm - ybp - ytp, 0.0);

  CQChartsGeom::BBox tbbox(tx, ty, tx + tw, ty + th);

  //---

  // draw image
  if (isCheckable() && ! isChecked()) {
    QColor bg = backgroundColor();

    if (! disabledImage_.isValid()) {
      const QImage &image = image_.image();

      int iw = image.width ();
      int ih = image.height();

      QImage disabledImage = CQChartsUtil::initImage(QSize(iw, ih));

      for (int y = 0; y < ih; ++y) {
        for (int x = 0; x < iw; ++x) {
          QRgb rgba = image.pixel(x, y);

          int r = qRed  (rgba);
          int g = qGreen(rgba);
          int b = qBlue (rgba);
          int a = qAlpha(rgba);

          QColor c(r, g, b ,a);

          QColor c1 = CQChartsUtil::blendColors(bg, c, 0.5);

          c1.setAlpha(a);

          disabledImage.setPixel(x, y, c1.rgba());
        }
      }

      disabledImage_ = CQChartsImage(disabledImage);
    }

    device->drawImageInRect(tbbox, disabledImage_);
  }
  else {
    device->drawImageInRect(tbbox, image_);
  }

  //---

  drawTerm(device);
}

void
CQChartsImageAnnotation::
initRectangle()
{
  // convert position to rectangle if needed
  if (! rectangle().isSet()) {
    positionToBBox();

    //---

    CQChartsRect rect;

    if      (plot())
      rect = CQChartsRect(bbox_, CQChartsUnits::PLOT);
    else if (view())
      rect = CQChartsRect(bbox_, CQChartsUnits::VIEW);

    setRectangle(rect);
  }
}

void
CQChartsImageAnnotation::
positionToBBox()
{
  assert(! rectangle().isSet());

  // get inner padding
  double xlp = lengthParentWidth (padding().left  ());
  double xrp = lengthParentWidth (padding().right ());
  double ytp = lengthParentHeight(padding().top   ());
  double ybp = lengthParentHeight(padding().bottom());

  // get outer margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  CQChartsGeom::Size psize, wsize;

  calcImageSize(psize, wsize);

  double x, y;

  positionToLL(wsize.width(), wsize.height(), x, y);

  CQChartsGeom::Point ll(x                 - xlp - xlm, y                  - ybp - ybm);
  CQChartsGeom::Point ur(x + wsize.width() + xrp + xrm, y + wsize.height() + ytp + ytm);

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

  if (image_.isValid())
    os << " -image {" << image_.fileName().toStdString() << "}";

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
  init();
}

CQChartsArrowAnnotation::
CQChartsArrowAnnotation(CQChartsPlot *plot, const CQChartsPosition &start,
                        const CQChartsPosition &end) :
 CQChartsAnnotation(plot, Type::ARROW), start_(start), end_(end)
{
  init();
}

CQChartsArrowAnnotation::
~CQChartsArrowAnnotation()
{
}

void
CQChartsArrowAnnotation::
init()
{
  setObjectName(QString("arrow.%1").arg(ind()));

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);

  if (plot())
    arrow_ = std::make_unique<CQChartsArrow>(plot());
  else
    arrow_ = std::make_unique<CQChartsArrow>(view());

  connect(arrow(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
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
                          const QString &desc, bool hidden=false) {
    auto *item = &(model->addProperty(path, arrow(), name, alias)->setDesc(desc));
    item->setHidden(hidden);
    return item;
  };

  auto addArrowStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                               const QString &desc, bool hidden=false) {
    auto *item = addArrowProp(path, name, alias, desc, hidden);
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

  addArrowProp     (frontHeadPath, "frontVisible"  , "visible",
                    "Arrow front head visible", /*hidden*/true);
  addArrowProp     (frontHeadPath, "frontType"     , "type",
                    "Arrow front head type");
  addArrowStyleProp(frontHeadPath, "frontLength"   , "length",
                    "Arrow front head length");
  addArrowStyleProp(frontHeadPath, "frontAngle"    , "angle",
                    "Arrow front head angle");
  addArrowStyleProp(frontHeadPath, "frontBackAngle", "backAngle",
                    "Arrow front head back angle", /*hidden*/true);
  addArrowStyleProp(frontHeadPath, "frontLineEnds" , "line",
                    "Arrow front head is drawn using lines", /*hidden*/true);

  QString tailHeadPath = path1 + "/tailHead";

  addArrowProp     (tailHeadPath, "tailVisible"  , "visible",
                    "Arrow tail head visible", /*hidden*/true);
  addArrowProp     (tailHeadPath, "tailType"    , "type",
                    "Arrow tail head type");
  addArrowStyleProp(tailHeadPath, "tailLength"   , "length",
                    "Arrow tail head length");
  addArrowStyleProp(tailHeadPath, "tailAngle"    , "angle",
                    "Arrow tail head angle");
  addArrowStyleProp(tailHeadPath, "tailBackAngle", "backAngle",
                    "Arrow tail head back angle", /*hidden*/true);
  addArrowStyleProp(tailHeadPath, "tailLineEnds" , "line",
                    "Arrow tail head is drawn using lines", /*hidden*/true);

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

  names << "line.width";

  names << "frontHead.type" << "frontHead.length" << "frontHead.angle";
  names << "tailHead.type"  << "tailHead.length"  << "tailHead.angle";

  names << "fill.visible" << "fill.color" << "fill.alpha";

  names << "stroke.visible" << "stroke.color" << "stroke.alpha" << "stroke.width";

#if 0
  // can't use objectNames as root is wrong
  auto *propertyModel = this->propertyModel();
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
  auto start = positionToParent(start_);
  auto end   = positionToParent(end_  );

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMax(), y2 = bbox.getYMax();

  // get inner padding
  double xlp = lengthParentWidth (padding().left  ());
  double xrp = lengthParentWidth (padding().right ());
  double ytp = lengthParentHeight(padding().top   ());
  double ybp = lengthParentHeight(padding().bottom());

  // get outer margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  x1 += xlp + xlm; y1 += ybp + ybm;
  x2 -= xrp + xrm; y2 -= ytp + ytm;

  if (start.x > end.x) std::swap(x1, x2);
  if (start.y > end.y) std::swap(y1, y2);

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
  auto p1 = windowToPixel(p);

  return arrow()->contains(p1);
}

void
CQChartsArrowAnnotation::
draw(CQChartsPaintDevice *device)
{
  drawInit(device);

  //---

  // calc box
  auto start = positionToParent(start_);
  auto end   = positionToParent(end_  );

  // get inner padding
  double xlp = lengthParentWidth (padding().left  ());
  double xrp = lengthParentWidth (padding().right ());
  double ytp = lengthParentHeight(padding().top   ());
  double ybp = lengthParentHeight(padding().bottom());

  // get outer margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  double x1 = std::min(start.x, end.x);
  double y1 = std::min(start.y, end.y);
  double x2 = std::max(start.x, end.x);
  double y2 = std::max(start.y, end.y);

  double x = x1 - xlp - xlm; // left
  double y = y1 - ybp - ybm; // bottom
  double w = (x2 - x1) + xlp + xrp + xlm + xrm;
  double h = (y2 - y1) + ybp + ytp + ybm + ytm;

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = arrow()->interpFillColor  (ColorInd());
  QColor strokeColor = arrow()->interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor, 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (arrow()->isStroked(), strokeColor, arrow()->strokeAlpha()),
    CQChartsBrushData(arrow()->isFilled (), bgColor, arrow()->fillAlpha(), arrow()->fillPattern()));

  updatePenBrushState(penBrush);

  //---

  // draw arrow
  {
  CQChartsWidgetUtil::AutoDisconnect
    autoDisconnect(arrow(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));

  arrow()->setFrom(start);
  arrow()->setTo  (end  );

  arrow()->draw(device, penBrush);
  }

  //---

  drawTerm(device);
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
    if      (arrow()->frontAngle().value() != 0.0 && arrow()->tailAngle().value() != 0.0)
      os << " -angle {" << arrow()->frontAngle().value() << " " <<
                           arrow()->tailAngle ().value() << "}";
    else if (arrow()->frontAngle().value() != 0.0)
      os << " -angle {" << arrow()->frontAngle().value() << " 0.0 }";
    else if (arrow()->tailAngle().value() != 0.0)
      os << " -angle {0.0 " << arrow()->tailAngle().value() << "}";

    if      (arrow()->frontBackAngle().value() >= 0.0 && arrow()->tailBackAngle().value() >= 0.0)
      os << " -angle {" << arrow()->frontBackAngle().value() << " " <<
                           arrow()->tailBackAngle ().value() << "}";
    else if (arrow()->frontBackAngle().value() >= 0.0)
      os << " -angle {" << arrow()->frontBackAngle().value() << " -1}";
    else if (arrow()->tailBackAngle ().value() >= 0.0)
      os << " -angle {-1 " << arrow()->tailBackAngle().value() << "}";
  }
  else if (fcustom) {
    if (arrow()->frontAngle().value() != 0.0)
      os << " -angle {" << arrow()->frontAngle().value() << " 0.0 }";

    if (arrow()->frontBackAngle().value() >= 0.0)
      os << " -angle {" << arrow()->frontBackAngle().value() << " -1}";
  }
  else if (tcustom) {
    if (arrow()->tailAngle().value() != 0.0)
      os << " -angle {0.0 " << arrow()->tailAngle().value() << "}";

    if (arrow()->tailBackAngle().value() >= 0.0)
      os << " -angle {-1 " << arrow()->tailBackAngle().value() << "}";
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
  auto *propertyModel = this->propertyModel();

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
 CQChartsAnnotation(view, Type::POINT), CQChartsObjPointData<CQChartsPointAnnotation>(this),
 position_(position), type_(type)
{
  init();
}

CQChartsPointAnnotation::
CQChartsPointAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                        const CQChartsSymbol &type) :
 CQChartsAnnotation(plot, Type::POINT), CQChartsObjPointData<CQChartsPointAnnotation>(this),
 position_(position), type_(type)
{
  init();
}

CQChartsPointAnnotation::
~CQChartsPointAnnotation()
{
}

void
CQChartsPointAnnotation::
init()
{
  setObjectName(QString("point.%1").arg(ind()));

  setSymbolType(type_);
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
    auto *item = addProp(path, name, alias, desc);
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
  auto position = positionToParent(position_);

  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  position += CQChartsGeom::Point(dx, dy);

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
  drawInit(device);

  //---

  const CQChartsSymbolData &symbolData = this->symbolData();

  auto position = positionToParent(position_);

  // get inner padding
  double xlp = lengthParentWidth (padding().left  ());
  double xrp = lengthParentWidth (padding().right ());
  double ytp = lengthParentHeight(padding().top   ());
  double ybp = lengthParentHeight(padding().bottom());

  // get outer margin
  double xlm = lengthParentWidth (margin().left  ());
  double xrm = lengthParentWidth (margin().right ());
  double ytm = lengthParentHeight(margin().top   ());
  double ybm = lengthParentHeight(margin().bottom());

  double sw = lengthParentWidth (symbolData.size());
  double sh = lengthParentHeight(symbolData.size());

  double w = sw + xlp + xrp + xlm + xrm;
  double h = sh + ybp + ytp + ybm + ytm;
  double x = position.x - w/2.0; // left
  double y = position.y - h/2.0; // bottom

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  //---

  const CQChartsStrokeData &strokeData = symbolData.stroke();
  const CQChartsFillData   &fillData   = symbolData.fill();

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor lineColor = interpColor(strokeData.color(), ColorInd());
  QColor fillColor = interpColor(fillData  .color(), ColorInd());

  if (isCheckable() && ! isChecked()) {
    lineColor = CQChartsUtil::blendColors(backgroundColor(), lineColor, 0.5);
    fillColor = CQChartsUtil::blendColors(backgroundColor(), fillColor, 0.5);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (strokeData.isVisible(), lineColor, strokeData.alpha(),
                      strokeData.width(), strokeData.dash()),
    CQChartsBrushData(fillData  .isVisible(), fillColor, fillData.alpha(), fillData.pattern()));

  updatePenBrushState(penBrush, CQChartsPlot::DrawType::SYMBOL);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw symbol
  CQChartsGeom::Point ps(bbox_.getXMid(), bbox_.getYMid());

  CQChartsDrawUtil::drawSymbol(device, symbolData.type(), ps, symbolData.size());

  //---

  drawTerm(device);
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

  if (fillAlpha() != CQChartsAlpha())
    os << " -fill_alpha " << fillAlpha().value();

  if (isStroked())
    os << " -stroked 1";

  if (strokeColor().isValid())
    os << " -stroke_color {" << strokeColor().toString().toStdString() << "}";

  if (strokeAlpha() != CQChartsAlpha())
    os << " -stroke_alpha " << strokeAlpha().value();

  if (strokeWidth().isSet())
    os << " -stroke_width {" << strokeWidth().toString().toStdString() << "}";
#endif

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsPieSliceAnnotation::
CQChartsPieSliceAnnotation(CQChartsView *view, const CQChartsPosition &position,
                           const CQChartsLength &innerRadius, const CQChartsLength &outerRadius,
                           const CQChartsAngle &startAngle, const CQChartsAngle &spanAngle) :
 CQChartsAnnotation(view, Type::PIE_SLICE), position_(position),
 innerRadius_(innerRadius), outerRadius_(outerRadius),
 startAngle_(startAngle), spanAngle_(spanAngle)
{
  init();
}

CQChartsPieSliceAnnotation::
CQChartsPieSliceAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                           const CQChartsLength &innerRadius, const CQChartsLength &outerRadius,
                           const CQChartsAngle &startAngle, const CQChartsAngle &spanAngle) :
 CQChartsAnnotation(plot, Type::PIE_SLICE), position_(position),
 innerRadius_(innerRadius), outerRadius_(outerRadius),
 startAngle_(startAngle), spanAngle_(spanAngle)
{
  init();
}

CQChartsPieSliceAnnotation::
~CQChartsPieSliceAnnotation()
{
}

void
CQChartsPieSliceAnnotation::
init()
{
  setObjectName(QString("pie_slice.%1").arg(ind()));

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);

  setStroked(true);
  setFilled (true);
}

void
CQChartsPieSliceAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "position"   , "", "Pie slice position");
  addProp(path1, "innerRadius", "", "Pie slice inner radius");
  addProp(path1, "outerRadius", "", "Pie slice outer radius");
  addProp(path1, "startAngle" , "", "Pie slice start angle");
  addProp(path1, "spanAngle"  , "", "Pie slice span angle");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsPieSliceAnnotation::
propertyId() const
{
  return QString("pieSliceAnnotation%1").arg(ind());
}

void
CQChartsPieSliceAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  auto position = positionToParent(position_);

  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  position += CQChartsGeom::Point(dx, dy);

  position_ = CQChartsPosition(position);

  bbox_ = bbox;
}

bool
CQChartsPieSliceAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsArcData arcData;

  auto c = positionToParent(position_);

  arcData.setCenter(CQChartsGeom::Point(c));

  arcData.setInnerRadius(lengthParentWidth(innerRadius()));
  arcData.setOuterRadius(lengthParentWidth(outerRadius()));

  arcData.setAngle1(startAngle());
  arcData.setAngle2(startAngle() + spanAngle());

  return arcData.inside(p);
}

void
CQChartsPieSliceAnnotation::
draw(CQChartsPaintDevice *device)
{
  drawInit(device);

  //---

  // calc box
  auto position = positionToParent(position_);

  double xri = lengthParentWidth (innerRadius());
  double yri = lengthParentHeight(innerRadius());

  double xro = lengthParentWidth (outerRadius());
  double yro = lengthParentHeight(outerRadius());

  CQChartsGeom::Point c(position);

  bbox_ = CQChartsGeom::BBox(c.x - xro, c.y - yro, c.x + xro, c.y + yro);

  //---

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    CQChartsBrushData(isFilled (), bgColor, fillAlpha(), fillPattern()));

  updatePenBrushState(penBrush);

  //---

  // draw pie slice
  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawPieSlice(device, c, CMathUtil::avg(xri, yri), CMathUtil::avg(xro, yro),
                                 startAngle(), startAngle() + spanAngle(), false, false);

  //---

  drawTerm(device);
}

void
CQChartsPieSliceAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_pie_slice_annotation", parentVarName, varName);

  if (position().isSet())
    os << " -position {" << position().toString().toStdString() << "}";

  if (innerRadius().isValid())
    os << " -inner_radius {" << innerRadius().toString().toStdString() << "}";

  if (outerRadius().isValid())
    os << " -outer_radius {" << outerRadius().toString().toStdString() << "}";

  os << " -start_angle " << startAngle().value();

  os << " -span_angle " << spanAngle().value();

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsAxisAnnotation::
CQChartsAxisAnnotation(CQChartsPlot *plot, Qt::Orientation direction, double start, double end) :
 CQChartsAnnotation(plot, Type::AXIS)
{
  init();

  axis_ = new CQChartsAxis(plot, direction, start, end);
}

CQChartsAxisAnnotation::
~CQChartsAxisAnnotation()
{
  delete axis_;
}

void
CQChartsAxisAnnotation::
init()
{
  setObjectName(QString("axis.%1").arg(ind()));

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

double
CQChartsAxisAnnotation::
position() const
{
  return axis_->position().realOr(0.0);
}

void
CQChartsAxisAnnotation::
setPosition(double r)
{
  axis_->setPosition(CQChartsOptReal(r));
}

void
CQChartsAxisAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addStrokeProperties(model, path1);

  axis_->addProperties(model, path1);
}

QString
CQChartsAxisAnnotation::
propertyId() const
{
  return QString("axisAnnotation%1").arg(ind());
}

void
CQChartsAxisAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  bbox_ = bbox;
}

bool
CQChartsAxisAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return bbox().inside(p);
}

void
CQChartsAxisAnnotation::
draw(CQChartsPaintDevice *device)
{
  drawInit(device);

  //---

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  CQChartsPenData penData(isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

  CQChartsBrushData brushData(isFilled(), bgColor, fillAlpha(), fillPattern());

  setPenBrush(penBrush, penData, brushData);

  updatePenBrushState(penBrush);

  //---

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  axis_->draw(plot(), device);

  //---

  drawTerm(device);

  //--

  bbox_ = axis_->bbox();
}

void
CQChartsAxisAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_axis_annotation", parentVarName, varName);

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

class CQChartsKeyAnnotationPlotKey : public CQChartsPlotKey {
 public:
  CQChartsKeyAnnotationPlotKey(CQChartsPlot *plot, CQChartsKeyAnnotation *annotation) :
   CQChartsPlotKey(plot), annotation_(annotation) {
  }

  void redraw(bool) override {
    annotation_->invalidate();
  }

 private:
  CQChartsKeyAnnotation *annotation_ { nullptr };
};

CQChartsKeyAnnotation::
CQChartsKeyAnnotation(CQChartsView *view) :
 CQChartsAnnotation(view, Type::KEY)
{
  init();

  key_ = new CQChartsViewKey(view);
}

CQChartsKeyAnnotation::
CQChartsKeyAnnotation(CQChartsPlot *plot) :
 CQChartsAnnotation(plot, Type::KEY)
{
  init();

  key_ = new CQChartsKeyAnnotationPlotKey(plot, this);
}

CQChartsKeyAnnotation::
~CQChartsKeyAnnotation()
{
  delete key_;
}

void
CQChartsKeyAnnotation::
init()
{
  setObjectName(QString("key.%1").arg(ind()));

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
}

void
CQChartsKeyAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addStrokeProperties(model, path1);

  QString keyPath = path + "/" + propertyId() + "/key";

  key_->addProperties(model, keyPath, "");
}

QString
CQChartsKeyAnnotation::
propertyId() const
{
  return QString("keyAnnotation%1").arg(ind());
}

void
CQChartsKeyAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  bbox_ = bbox;
}

bool
CQChartsKeyAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return bbox().inside(p);
}

bool
CQChartsKeyAnnotation::
selectPress(const CQChartsGeom::Point &w, CQChartsSelMod selMod)
{
  if (key_->selectPress(w, selMod)) {
    emit pressed(QString("%1:%2").arg(id()).arg(key_->id()));
    return true;
  }

  auto *plotKey = dynamic_cast<CQChartsPlotKey *>(key_);

  if (plotKey->contains(w)) {
    auto *item = plotKey->getItemAt(w);

    if (item) {
      bool handled = item->selectPress(w, selMod);

      if (handled) {
        emit pressed(QString("%1:%2:%3").arg(id()).arg(key_->id()).arg(item->id()));
        return true;
      }
    }
  }

  return CQChartsAnnotation::selectPress(w, selMod);
}

void
CQChartsKeyAnnotation::
draw(CQChartsPaintDevice *device)
{
  drawInit(device);

  //---

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  CQChartsPenData penData(isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

  CQChartsBrushData brushData(isFilled(), bgColor, fillAlpha(), fillPattern());

  setPenBrush(penBrush, penData, brushData);

  updatePenBrushState(penBrush);

  //---

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  key_->draw(device);

  //---

  drawTerm(device);

  //---

  bbox_ = key_->bbox();
}

void
CQChartsKeyAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_key_annotation", parentVarName, varName);

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsPointSetAnnotation::
CQChartsPointSetAnnotation(CQChartsView *view, const CQChartsPoints &values) :
 CQChartsAnnotation(view, Type::POINT_SET), values_(values)
{
  init();
}

CQChartsPointSetAnnotation::
CQChartsPointSetAnnotation(CQChartsPlot *plot, const CQChartsPoints &values) :
 CQChartsAnnotation(plot, Type::POINT_SET), values_(values)
{
  init();
}

CQChartsPointSetAnnotation::
~CQChartsPointSetAnnotation()
{
}

void
CQChartsPointSetAnnotation::
init()
{
  setObjectName(QString("pointset.%1").arg(ind()));

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);

  setStroked(true);
  setFilled (true);

  updateValues();
}

void
CQChartsPointSetAnnotation::
updateValues()
{
  hull_.clear();

  for (auto &p : values_.points()) {
    auto p1 = positionToParent(p);

    hull_.addPoint(p1);

    xrange_.add(p1.x);
    yrange_.add(p1.y);

    gridCell_.addPoint(CQChartsGeom::Point(p1));
  }
}

void
CQChartsPointSetAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "values"  , "", "Values");
  addProp(path1, "drawType", "", "Draw type");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsPointSetAnnotation::
propertyId() const
{
  return QString("pointSetAnnotation%1").arg(ind());
}

void
CQChartsPointSetAnnotation::
setBBox(const CQChartsGeom::BBox &, const CQChartsResizeSide &)
{
#if 0
  auto hbbox = hull_.bbox();

  auto ll = positionToParent(hbbox.getUL());

  double dx = hbbox.getXMin() - rect_.getXMin();
  double dy = hbbox.getYMin() - rect_.getYMin();

  ll += CQChartsGeom::Point(dx, dy);

  rect_ = CQChartsGeom::BBox(ll.x, ll.y, bbox.getWidth(), bbox.getHeight());

  // TODO: move all points ?
#endif
}

bool
CQChartsPointSetAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return bbox().inside(p);
}

void
CQChartsPointSetAnnotation::
draw(CQChartsPaintDevice *device)
{
  drawInit(device);

  //---

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  CQChartsPenData penData(isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash());

  CQChartsBrushData brushData(isFilled(), bgColor, fillAlpha(), fillPattern());

  setPenBrush(penBrush, penData, brushData);

  updatePenBrushState(penBrush);

  //---

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  if      (drawType() == DrawType::SYMBOLS) {
    CQChartsSymbolData symbolData;

    for (const auto &p : values_.points()) {
      auto p1 = positionToParent(p);

      CQChartsDrawUtil::drawSymbol(device, symbolData.type(),
                                   CQChartsGeom::Point(p1), symbolData.size());
    }
  }
  else if (drawType() == DrawType::HULL) {
    hull_.draw(plot_, device);
  }
  else if (drawType() == DrawType::DENSITY) {
    CQChartsBivariateDensity density;

    CQChartsBivariateDensity::Data data;

    data.gridSize = 16;
    data.delta    = 0.0;

    for (const auto &p : values_.points()) {
      auto p1 = positionToParent(p);

      data.values.push_back(p1);
    }

    data.xrange = xrange_;
    data.yrange = yrange_;

    if (plot_)
      density.draw(plot_, device, data);
  }
  else if (drawType() == DrawType::BEST_FIT) {
    CQChartsFitData fitData;

    CQChartsGeom::Polygon poly;

    for (const auto &p : values_.points()) {
      auto p1 = positionToParent(p);

      poly.addPoint(p1);
    }

    fitData.calc(poly, 3);

    CQChartsGeom::Point p1 =
      device->windowToPixel(CQChartsGeom::Point(xrange_.min(), yrange_.min()));
    CQChartsGeom::Point p2 =
      device->windowToPixel(CQChartsGeom::Point(xrange_.max(), yrange_.max()));

    double dx = (p2.x - p1.x)/100.0;

    CQChartsGeom::Polygon fitPoly;

    for (int i = 0; i <= 100; ++i) {
      double px = p1.x + i*dx;

      auto p = device->pixelToWindow(CQChartsGeom::Point(px, 0.0));

      double y = fitData.interp(p.x);

      fitPoly.addPoint(CQChartsGeom::Point(p.x, y));
    }

    QPainterPath path = CQChartsDrawUtil::polygonToPath(fitPoly, /*closed*/false);

    device->strokePath(path, penBrush.pen);
  }
  else if (drawType() == DrawType::GRID) {
    gridCell_.setNX(40);
    gridCell_.setNY(40);

    gridCell_.setXInterval(xrange_.min(), xrange_.max());
    gridCell_.setYInterval(yrange_.min(), yrange_.max());

    gridCell_.resetPoints();

    for (const auto &p : values_.points()) {
      auto p1 = positionToParent(p);

      gridCell_.addPoint(CQChartsGeom::Point(p1));
    }

    int maxN = gridCell_.maxN();

    for (const auto &px : gridCell_.xyPoints()) {
      int                              ix      = px.first;
      const CQChartsGridCell::YPoints &yPoints = px.second;

      double xmin, xmax;

      gridCell_.xIValues(ix, xmin, xmax);

      for (const auto &py : yPoints) {
        int                             iy     = py.first;
        const CQChartsGridCell::Points &points = py.second;

        int n = points.size();
        if (n <= 0) continue;

        double ymin, ymax;

        gridCell_.yIValues(iy, ymin, ymax);

        //---

        CQChartsGeom::BBox bbox(xmin, ymin, xmax, ymax);

        ColorInd colorInd(CMathUtil::map(double(n), 1.0, 1.0*maxN, 0.0, 1.0));

        QColor bgColor = view()->interpColor(CQChartsColor(CQChartsColor::Type::PALETTE), colorInd);

        // set pen and brush
        CQChartsPenBrush penBrush;

        setPenBrush(penBrush, penData, CQChartsBrushData(true, bgColor));

        CQChartsDrawUtil::setPenBrush(device, penBrush);

        // draw rect
        device->drawRect(bbox);
      }
    }
  }

  //---

  drawTerm(device);
}

void
CQChartsPointSetAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_point_set_annotation", parentVarName, varName);

  os << " -values {" << values().toString().toStdString() << "}";

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsValueSetAnnotation::
CQChartsValueSetAnnotation(CQChartsView *view, const CQChartsRect &rectangle,
                          const CQChartsReals &values) :
 CQChartsAnnotation(view, Type::VALUE_SET), rectangle_(rectangle), values_(values)
{
  init();
}

CQChartsValueSetAnnotation::
CQChartsValueSetAnnotation(CQChartsPlot *plot, const CQChartsRect &rectangle,
                           const CQChartsReals &values) :
 CQChartsAnnotation(plot, Type::VALUE_SET), rectangle_(rectangle), values_(values)
{
  init();
}

CQChartsValueSetAnnotation::
~CQChartsValueSetAnnotation()
{
}

void
CQChartsValueSetAnnotation::
init()
{
  assert(rectangle_.isValid());

  setObjectName(QString("valueset.%1").arg(ind()));

  editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);

  setStroked(true);
  setFilled (true);

  updateValues();
}

void
CQChartsValueSetAnnotation::
updateValues()
{
  delete density_;

  density_ = new CQChartsDensity;

  density_->setDrawType(CQChartsDensity::DrawType::WHISKER);

  density_->setXVals(values_.reals());

  connect(density_, SIGNAL(dataChanged()), this, SLOT(invalidateSlot()));
}

void
CQChartsValueSetAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addDensityProp = [&](const QString &path, const QString &name, const QString &alias,
                            const QString &desc) {
    return &(model->addProperty(path, density_, name, alias)->setDesc(desc));
  };

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "rectangle", "", "Rectangle");
  addProp(path1, "values"   , "", "Values");

  addDensityProp(path1, "numSamples"     , "", "Number of samples");
  addDensityProp(path1, "smoothParameter", "", "Smooth parameter");
  addDensityProp(path1, "drawType"       , "", "Density draw types");
  addDensityProp(path1, "orientation"    , "", "Density orientation");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsValueSetAnnotation::
propertyId() const
{
  return QString("valueSetAnnotation%1").arg(ind());
}

void
CQChartsValueSetAnnotation::
setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &)
{
  auto bbox1 = rectangle_.bbox();

  auto ll = positionToParent(bbox1.getMin());

  double dx = bbox.getXMin() - bbox_.getXMin();
  double dy = bbox.getYMin() - bbox_.getYMin();

  ll += CQChartsGeom::Point(dx, dy);

  bbox1 = CQChartsGeom::BBox(ll.x, ll.y, ll.x + bbox1.getWidth(), ll.y + bbox1.getHeight());

  if (bbox1.isSet())
    rectangle_.setValue(rectangle_.units(), bbox1);
}

bool
CQChartsValueSetAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return bbox().inside(p);
}

void
CQChartsValueSetAnnotation::
draw(CQChartsPaintDevice *device)
{
  drawInit(device);

  //---

  auto bbox = rectangle_.bbox();

  bbox_ = density_->bbox(bbox);

  // set pen and brush
  CQChartsPenBrush penBrush;

  QColor bgColor     = interpFillColor  (ColorInd());
  QColor strokeColor = interpStrokeColor(ColorInd());

  if (isCheckable() && ! isChecked()) {
    bgColor     = CQChartsUtil::blendColors(backgroundColor(), bgColor    , 0.5);
    strokeColor = CQChartsUtil::blendColors(backgroundColor(), strokeColor, 0.5);
  }

  setPenBrush(penBrush,
    CQChartsPenData  (isStroked(), strokeColor, strokeAlpha(), strokeWidth(), strokeDash()),
    CQChartsBrushData(isFilled (), bgColor, fillAlpha(), fillPattern()));

  updatePenBrushState(penBrush);

  //---

  // draw density
  CQChartsDrawUtil::setPenBrush(device, penBrush);

  if (plot())
    density_->draw(plot(), device, bbox);
  else
    assert(false);

  //---

  drawTerm(device);
}

void
CQChartsValueSetAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_value_set_annotation", parentVarName, varName);

  if (rectangle().isSet())
    os << " -rectangle {" << rectangle().toString().toStdString() << "}";

  os << " -values {" << values().toString().toStdString() << "}";

  os << "]\n";

  //---

  writeProperties(os, varName);
}

//---

CQChartsButtonAnnotation::
CQChartsButtonAnnotation(CQChartsView *view, const CQChartsPosition &position,
                         const QString &textStr) :
 CQChartsAnnotation(view, Type::BUTTON)
{
  setPosition(position);

  init(textStr);
}

CQChartsButtonAnnotation::
CQChartsButtonAnnotation(CQChartsPlot *plot, const CQChartsPosition &position,
                         const QString &textStr) :
 CQChartsAnnotation(plot, Type::BUTTON)
{
  setPosition(position);

  init(textStr);
}

CQChartsButtonAnnotation::
~CQChartsButtonAnnotation()
{
}

void
CQChartsButtonAnnotation::
init(const QString &textStr)
{
  setObjectName(QString("button.%1").arg(ind()));

  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextStr  (textStr);
  setTextColor(themeFg);
}

void
CQChartsButtonAnnotation::
setPosition(const CQChartsPosition &p)
{
  position_ = p;

  emit dataChanged();
}

void
CQChartsButtonAnnotation::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
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

  QString path1 = path + "/" + propertyId();

  CQChartsAnnotation::addProperties(model, path1);

  addProp(path1, "position", "position", "Text origin");

  QString textPath = path1 + "/text";

  addProp(textPath, "textStr", "string", "Text string");

  addStyleProp(textPath, "textColor", "color", "Text color");
  addStyleProp(textPath, "textAlpha", "alpha", "Text alpha");
  addStyleProp(textPath, "textFont" , "font" , "Text font");
  addStyleProp(textPath, "textAlign", "align", "Text align");

  addStrokeFillProperties(model, path1);
}

QString
CQChartsButtonAnnotation::
propertyId() const
{
  return QString("buttonAnnotation%1").arg(ind());
}

void
CQChartsButtonAnnotation::
mousePress(const CQChartsGeom::Point &, SelMod)
{
  pressed_ = true;

  invalidate();
}

void
CQChartsButtonAnnotation::
mouseMove(const CQChartsGeom::Point &)
{
}

void
CQChartsButtonAnnotation::
mouseRelease(const CQChartsGeom::Point &)
{
  pressed_ = false;

  invalidate();
}

bool
CQChartsButtonAnnotation::
inside(const CQChartsGeom::Point &p) const
{
  return CQChartsAnnotation::inside(p);
}

void
CQChartsButtonAnnotation::
draw(CQChartsPaintDevice *device)
{
  if (device->type() == CQChartsPaintDevice::Type::SVG)
    return;

  //---

  drawInit(device);

  //---

  prect_ = calcPixelRect();
  bbox_  = pixelToWindow(CQChartsGeom::BBox(prect_));

  //---

  if (device->isInteractive()) {
    auto *painter = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

    QImage img = CQChartsUtil::initImage(QSize(prect_.width(), prect_.height()));

    QStylePainter spainter(&img, view());

    QStyleOptionButton opt;

    opt.rect = QRect(0, 0, prect_.width(), prect_.height());
    opt.text = textStr();

    if (pressed_)
      opt.state |= QStyle::State_Sunken;
    else
      opt.state |= QStyle::State_Raised;

    opt.state |= QStyle::State_Active;

    if (isEnabled())
      opt.state |= QStyle::State_Enabled;

    if (isCheckable()) {
      if (isChecked())
        opt.state |= QStyle::State_On;
      else
        opt.state |= QStyle::State_Off;
    }

    opt.palette = view()->palette();

    spainter.setFont(calcFont(textFont()));

    QColor bg = opt.palette.color(QPalette::Button);
    QColor fg = opt.palette.color(QPalette::ButtonText);

    QColor c = fg;

    if      (! isEnabled())
      c = CQChartsUtil::blendColors(bg, fg, 0.6);
    else if (isInside())
      c = Qt::blue;

    opt.palette.setColor(QPalette::ButtonText, c);

    spainter.drawControl(QStyle::CE_PushButton, opt);

    QPointF p = prect_.topLeft();

    painter->painter()->drawImage(p.x(), p.y(), img);
  }

  //---

  drawTerm(device);
}

void
CQChartsButtonAnnotation::
writeHtml(CQChartsHtmlPaintDevice *device)
{
  prect_ = calcPixelRect();
  bbox_  = pixelToWindow(CQChartsGeom::BBox(prect_));

  device->createButton(bbox_, textStr(), id(), "annotationClick");
}

QRect
CQChartsButtonAnnotation::
calcPixelRect() const
{
  QFont font = calcFont(textFont());

  QFontMetrics fm(font);

  int w = fm.width(textStr()) + 4;
  int h = fm.height() + 4;

  auto p = positionToPixel(position());

  return QRect(int(p.x), int(p.y), w, h);
}

void
CQChartsButtonAnnotation::
write(std::ostream &os, const QString &parentVarName, const QString &varName) const
{
  // -view/-plot -id -tip
  writeKeys(os, "create_charts_button_annotation", parentVarName, varName);

  os << " -position {" << position().toString().toStdString() << "}";

  if (textStr().length())
    os << " -text {" << textStr().toStdString() << "}";

  os << "]\n";

  //---

  writeProperties(os, varName);
}
