#include <CQChartsPlotObj.h>
#include <CQChartsPlot.h>
#include <CQChartsEditHandles.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsVariant.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QMetaProperty>

CQChartsPlotObj::
CQChartsPlotObj(CQChartsPlot *plot, const BBox &rect, const ColorInd &is,
                const ColorInd &ig, const ColorInd &iv) :
 CQChartsObj(plot->charts(), rect), plot_(plot), is_(is), ig_(ig), iv_(iv)
{
  assert(plot_);

  assert(! CMathUtil::isNaN(rect.getXMin()) && ! CMathUtil::isNaN(rect.getYMin()) &&
         ! CMathUtil::isNaN(rect.getXMax()) && ! CMathUtil::isNaN(rect.getYMax()));

  assert(is_.isValid());
  assert(ig_.isValid());
  assert(iv_.isValid());
}

CQChartsPlotObj::
~CQChartsPlotObj()
{
}

//---

void
CQChartsPlotObj::
setDrawLayer(const DrawLayer &l)
{
  drawLayer_ = l;

  Q_EMIT layerChanged();
}

CQChartsEditHandles *
CQChartsPlotObj::
editHandles() const
{
  if (! editHandles_) {
    auto *th = const_cast<CQChartsPlotObj *>(this);

    th->editHandles_ = std::make_unique<EditHandles>(plot(), EditHandles::Mode::MOVE);
  }

  return editHandles_.get();
}

//---

bool
CQChartsPlotObj::
calcColumnId(const QModelIndex &ind, QString &str) const
{
  bool ok;

  str = plot()->idColumnString(ind.row(), ind.parent(), ok);

  return ok;
}

//---

double
CQChartsPlotObj::
xColorValue(bool relative) const
{
  double x = rect().getXMid();

  if (relative) {
    const auto &dataRange = plot()->dataRange();

    return CMathUtil::map(x, dataRange.xmin(), dataRange.xmax(), 0.0, 1.0);
  }
  else
    return x;
}

double
CQChartsPlotObj::
yColorValue(bool relative) const
{
  double y = rect().getYMid();

  if (relative) {
    const auto &dataRange = plot()->dataRange();

    return CMathUtil::map(y, dataRange.ymin(), dataRange.ymax(), 0.0, 1.0);
  }
  else
    return y;
}

//---

QModelIndex
CQChartsPlotObj::
modelInd() const
{
  return (! modelInds_.empty() ? modelInds_[0] : QModelIndex());
}

void
CQChartsPlotObj::
setModelInd(const QModelIndex &ind)
{
  assert(ind.isValid());

  modelInds_.clear();

  addModelInd(ind);
}

void
CQChartsPlotObj::
setModelInds(const QModelIndices &inds)
{
  modelInds_ = inds;
}

void
CQChartsPlotObj::
addModelInd(const QModelIndex &ind)
{
  assert(ind.isValid());

  assert(ind.model() == plot()->sourceModel());

  modelInds_.push_back(ind);
}

//---

CQChartsPlotObj::ColorInd
CQChartsPlotObj::
calcColorInd() const
{
  return plot()->calcColorInd(this, nullptr, is(), ig(), iv());
}

//---

QString
CQChartsPlotObj::
propertyId() const
{
  return QString("object:%1").arg(id());
}

void
CQChartsPlotObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "rect"      )->setDesc("Bounding box");
  model->addProperty(path, this, "visible"   )->setDesc("Is Visible");
  model->addProperty(path, this, "selected"  )->setDesc("Is Selected");
  model->addProperty(path, this, "selectable")->setDesc("Is Selectable");
  model->addProperty(path, this, "editable"  )->setDesc("Is Editable");
}

//---

bool
CQChartsPlotObj::
selectPress(const Point &, SelData &)
{
  return true;
}

// are any of the normalized indices in object's select indices
bool
CQChartsPlotObj::
isSelectIndices(const Indices &indices) const
{
  Indices inds;

  getSelectIndices(inds);

  for (auto &ind1 : inds) {
    //auto ind2 = plot()->normalizeIndex(ind1);

    //if (indices.find(ind2) != indices.end())
    //  return true;

    if (indices.find(ind1) != indices.end())
      return true;
  }

  return false;
}

#if 0
// is normalized index in object's select indices
bool
CQChartsPlotObj::
isSelectIndex(const QModelIndex &ind) const
{
  Indices inds;

  getSelectIndices(inds);

  for (auto &ind1 : inds) {
    //auto ind2 = plot()->normalizeIndex(ind1);

    //if (ind == ind2)
    //  return true;

    if (ind == ind1)
      return true;
  }

  return false;
}
#endif

// get object select indices (normalized)
void
CQChartsPlotObj::
getNormalizedSelectIndices(Indices &indices) const
{
  Indices inds;

  getSelectIndices(inds);

  for (auto &ind1 : inds) {
    //auto ind2 = plot()->normalizeIndex(ind1);

    //indices.insert(ind2);

    indices.insert(ind1);
  }
}

// add object select indices (normalized) to plot selection buffer to use
// in cross select from objects to selection model
void
CQChartsPlotObj::
addSelectIndices(Plot *plot)
{
  Indices inds;

  getNormalizedSelectIndices(inds);

  for (const auto &ind : inds)
    plot->addSelectIndex(ind);
}

// get object select indices (normalized) from object's columns
void
CQChartsPlotObj::
getSelectIndices(Indices &inds) const
{
  getObjSelectIndices(inds);

  if (plot()->idColumn().isValid())
    addColumnSelectIndex(inds, plot()->idColumn());

  if (plot()->visibleColumn().isValid())
    addColumnSelectIndex(inds, plot()->visibleColumn());

  if (plot()->colorColumn().isValid())
    addColumnSelectIndex(inds, plot()->colorColumn());
}

// add plot object's indices to array for specified (new) column
void
CQChartsPlotObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    for (const auto &ind : modelInds())
      addSelectIndex(inds, ind.row(), column, ind.parent());
  }
}

// add model index to array
void
CQChartsPlotObj::
addSelectIndex(Indices &inds, const ModelIndex &ind) const
{
  addSelectIndex(inds, ind.row(), ind.column(), ind.parent());
}

// add model index data to array (ignore if column not right type)
void
CQChartsPlotObj::
addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
               const QModelIndex &parent) const
{
  if (column.type() != Column::Type::DATA &&
      column.type() != Column::Type::DATA_INDEX)
    return;

  auto ind = plot()->selectIndex(row, Column(column.column()), parent);

  //addSelectIndex(inds, ind);
  inds.insert(ind);
}

#if 0
void
CQChartsPlotObj::
addSelectIndex(Indices &inds, const QModelIndex &ind) const
{
  inds.insert(ind);
}
#endif

//---

void
CQChartsPlotObj::
drawBg(PaintDevice *) const
{
}

void
CQChartsPlotObj::
drawFg(PaintDevice *) const
{
}

void
CQChartsPlotObj::
draw(PaintDevice *) const
{
}

void
CQChartsPlotObj::
drawEditHandles(PaintDevice *device) const
{
  assert(plot()->view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  setEditHandlesBBox();

  editHandles()->draw(device);
}

void
CQChartsPlotObj::
setEditHandlesBBox() const
{
  if (isEditResize())
    editHandles()->setMode(EditHandles::Mode::RESIZE);
  else
    editHandles()->setMode(EditHandles::Mode::MOVE);

  editHandles()->setBBox(this->rect());
}

void
CQChartsPlotObj::
drawDebugRect(PaintDevice *device) const
{
  drawDebugRect(device, rect());
}

void
CQChartsPlotObj::
drawDebugRect(PaintDevice *device, const BBox &rect) const
{
  plot()->drawWindowColorBox(device, rect);
}

//---

bool
CQChartsPlotObj::
contains(const Point &p) const
{
  return inside(p);
}

//---

void
CQChartsPlotObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/false);

  writeObjScriptData(device);

  writeScriptProperties(device);
}

void
CQChartsPlotObj::
writeObjScriptData(ScriptPaintDevice *device) const
{
  auto encodeString = [&](const QString &str) {
    return ScriptPaintDevice::encodeString(str).toStdString();
  };

  std::ostream &os = device->os();

  os << "  this.id    = \"" << this->id().toStdString() << "\";\n";
  os << "  this.tipId = \"" << encodeString(this->tipId()) << "\";\n";

  const auto &rect = this->rect();

  os << "\n";
  os << "  this.xmin = " << rect.getXMin() << ";\n";
  os << "  this.ymin = " << rect.getYMin() << ";\n";
  os << "  this.xmax = " << rect.getXMax() << ";\n";
  os << "  this.ymax = " << rect.getYMax() << ";\n";

  os << "\n";
  os << "  this.isInside = 0;\n";

  writeScriptGC(device, penBrush_);

  if      (this->isPolygon()) {
    auto poly = this->polygon();

    int np = poly.size();

    os << "\n";
    os << "  this.poly = [";

    for (int i = 0; i < np; ++i) {
      if (i > 0) os << ", ";

      os << poly.point(i).x << ", " << poly.point(i).y;
    }

    os << "];\n";
  }
  else if (this->isCircle()) {
    os << "\n";
    os << "  this.xc = " << rect.getXMid() << ";\n";
    os << "  this.yc = " << rect.getYMid() << ";\n";
    os << "  this.radius = " << this->radius() << ";\n";
  }
  else if (this->isArc()) {
    auto arc = this->arcData();

    os << "\n";
    os << "  this.arc = {};\n";
    os << "  this.arc.cx = " << arc.center().x << ";\n";
    os << "  this.arc.cy = " << arc.center().y << ";\n";
    os << "  this.arc.ri = " << arc.innerRadius() << ";\n";
    os << "  this.arc.ro = " << arc.outerRadius() << ";\n";
    os << "  this.arc.a1 = " << arc.angle1().value() << ";\n";
    os << "  this.arc.a2 = " << arc.angle2().value() << ";\n";
  }
}

void
CQChartsPlotObj::
writeScriptProperties(ScriptPaintDevice *device) const
{
  std::ostream &os = device->os();

  const auto *metaObject = this->metaObject();
  if (! metaObject) return;

  int firstProp = metaObject->propertyOffset();
  int numProp   = metaObject->propertyCount();

  if (firstProp < numProp)
    os << "\n";

  for (int p = firstProp; p < numProp; ++p) {
    auto metaProperty = metaObject->property(p);

    auto name = metaProperty.name();

    auto var = this->property(name);

    QString valueStr;

    if (! CQChartsVariant::toString(var, valueStr))
      continue;

    if (! CQChartsVariant::isNumeric(var))
      valueStr = "\"" + valueStr + "\"";

    auto assignStr = QString("  this.%1 = %2;").arg(name).arg(valueStr);

    os << assignStr.toStdString() << "\n";
  }
}

void
CQChartsPlotObj::
writeScriptGC(ScriptPaintDevice *device, const CQChartsPenBrush &penBrush) const
{
  std::ostream &os = device->os();

  os << "\n";

  if (device->strokeStyleName().length())
    os << "  this." << device->strokeStyleName().toStdString() << " = \"";
  else
    os << "  this.strokeColor = \"";

  if (penBrush.pen.style() == Qt::NoPen)
    os <<  "#00000000";
  else
    os << CQChartsUtil::encodeScriptColor(penBrush.pen.color()).toStdString();

  os << "\";\n";

  if (device->fillStyleName().length())
    os << "  this." << device->fillStyleName().toStdString() << " = \"";
  else
    os << "  this.fillColor = \"";

  if (penBrush.brush.style() == Qt::NoBrush)
    os <<  "#00000000";
  else
    os << CQChartsUtil::encodeScriptColor(penBrush.brush.color()).toStdString();

  os << "\";\n";
}

void
CQChartsPlotObj::
writeScriptInsideColor(ScriptPaintDevice *device, bool isSave) const
{
  std::ostream &os = device->os();

  if (isSolid()) {
    if (isSave) {
      os << "      this.saveFillColor = this.fillColor;\n";
      os << "      this.fillColor = \"rgb(255,0,0)\";\n";
    }
    else {
      os << "      this.fillColor = this.saveFillColor;\n";
    }
  }
  else {
    if (isSave) {
      os << "      this.saveStrokeColor = this.strokeColor;\n";
      os << "      this.strokeColor = \"rgb(255,0,0)\";\n";
    }
    else {
      os << "      this.strokeColor = this.saveStrokeColor;\n";
    }
  }
}

//------

CQChartsGroupObj::
CQChartsGroupObj(CQChartsPlot *plot, const BBox &bbox, const ColorInd &ig) :
 CQChartsPlotObj(plot, bbox, ColorInd(), ig, ColorInd())
{
}

//------

CQChartsPlotPointObj::
CQChartsPlotPointObj(Plot *plot, const BBox &rect, const Point &p,
                     const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(plot, rect, is, ig, iv), p_(p)
{
}

bool
CQChartsPlotPointObj::
inside(const Point &p) const
{
  if (! isVisible() || isFiltered())
    return false;

  //---

  double sx, sy;

  calcSymbolPixelSize(sx, sy);

  //---

  auto p1 = plot_->windowToPixel(point());

  BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

bool
CQChartsPlotPointObj::
rectIntersect(const BBox &r, bool inside) const
{
  if (! isVisible()) return false;

  //---

  double sx, sy;

  calcSymbolPixelSize(sx, sy);

  //---

  auto p1 = plot_->windowToPixel(point());

  BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pr = plot_->windowToPixel(r);

  if (inside)
    return pr.inside(pbbox);
  else
    return pr.overlaps(pbbox);
}

void
CQChartsPlotPointObj::
calcSymbolPixelSize(double &sx, double &sy, bool square, bool enforceMinSize) const
{
  if (plot_->isScaleSymbolSize()) {
    auto prect = plot_->windowToPixel(rect());

    sx = prect.getWidth ()/2.0;
    sy = prect.getHeight()/2.0;

    if (enforceMinSize) {
      sx = std::max(sx, 4.0);
      sy = std::max(sy, 4.0);
    }

    if (square) { sx = std::min(sx, sy); sy = sx; }
  }
  else {
    // always square !?
    plot_->pixelSymbolSize(calcSymbolSize(), sx, sy, calcSymbolDir());
  }
}
