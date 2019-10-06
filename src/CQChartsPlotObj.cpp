#include <CQChartsPlotObj.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQPropertyViewModel.h>

CQChartsPlotObj::
CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect, const ColorInd &is,
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
    const CQChartsGeom::Range &dataRange = plot()->dataRange();

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
    const CQChartsGeom::Range &dataRange = plot()->dataRange();

    return CMathUtil::map(y, dataRange.ymin(), dataRange.ymax(), 0.0, 1.0);
  }
  else
    return y;
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
addProperties(CQPropertyViewModel *, const QString &)
{
}

//---

bool
CQChartsPlotObj::
isSelectIndex(const QModelIndex &ind) const
{
  Indices inds;

  getHierSelectIndices(inds);

  for (auto &ind1 : inds) {
    QModelIndex ind2 = plot()->normalizeIndex(ind1);

    if (ind == ind2)
      return true;
  }

  return false;
}

void
CQChartsPlotObj::
addSelectIndices()
{
  Indices inds;

  getHierSelectIndices(inds);

  for (const auto &ind : inds)
    plot()->addSelectIndex(ind);
}

void
CQChartsPlotObj::
getHierSelectIndices(Indices &inds) const
{
  getSelectIndices(inds);

  if (plot()->idColumn().isValid())
    addColumnSelectIndex(inds, plot()->idColumn());

  if (plot()->visibleColumn().isValid())
    addColumnSelectIndex(inds, plot()->visibleColumn());

  if (plot()->colorColumn().isValid())
    addColumnSelectIndex(inds, plot()->colorColumn());
}

void
CQChartsPlotObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    for (const auto &ind : modelInds())
      addSelectIndex(inds, ind.row(), column, ind.parent());
  }
}

void
CQChartsPlotObj::
addSelectIndex(Indices &inds, const CQChartsModelIndex &ind) const
{
  addSelectIndex(inds, ind.row, ind.column, ind.parent);
}

void
CQChartsPlotObj::
addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
               const QModelIndex &parent) const
{
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return;

  QModelIndex ind = plot()->selectIndex(row, column.column(), parent);

  addSelectIndex(inds, ind);
}

void
CQChartsPlotObj::
addSelectIndex(Indices &inds, const QModelIndex &ind) const
{
  inds.insert(ind);
}

void
CQChartsPlotObj::
drawBg(CQChartsPaintDevice *) const
{
}

void
CQChartsPlotObj::
drawFg(CQChartsPaintDevice *) const
{
}

void
CQChartsPlotObj::
draw(CQChartsPaintDevice *)
{
}

void
CQChartsPlotObj::
drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsPenBrush &penBrush,
                   const CQChartsGeom::BBox &rect, const CQChartsLength &cornerSize) const
{
  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawRoundedPolygon(device, rect.qrect(), cornerSize, cornerSize);
}

void
CQChartsPlotObj::
drawDebugRect(CQChartsPaintDevice *device)
{
  plot()->drawWindowColorBox(device, rect());
}

void
CQChartsPlotObj::
writeScriptData(CQChartsScriptPainter *device) const
{
  auto encodeString = [&](const QString &str) {
    return CQChartsScriptPainter::encodeString(str).toStdString();
  };

  std::ostream &os = device->os();

  os << "  this.id    = \"" << this->id().toStdString() << "\";\n";
  os << "  this.tipId = \"" << encodeString(this->tipId()) << "\";\n";

  const CQChartsGeom::BBox &rect = this->rect();

  os << "\n";
  os << "  this.xmin = " << rect.getXMin() << ";\n";
  os << "  this.ymin = " << rect.getYMin() << ";\n";
  os << "  this.xmax = " << rect.getXMax() << ";\n";
  os << "  this.ymax = " << rect.getYMax() << ";\n";

  os << "\n";
  os << "  this.isInside = 0;\n";

  writeScriptGC(device, penBrush_);

  if      (this->isPolygon()) {
    QPolygonF poly = this->polygon();

    int np = poly.length();

    os << "\n";
    os << "  this.poly = [";

    for (int i = 0; i < np; ++i) {
      if (i > 0) os << ", ";

      os << poly[i].x() << ", " << poly[i].y();
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
    CQChartsArcData arc = this->arcData();

    os << "\n";
    os << "  this.arc = {};\n";
    os << "  this.arc.cx = " << arc.center().x << ";\n";
    os << "  this.arc.cy = " << arc.center().y << ";\n";
    os << "  this.arc.ri = " << arc.innerRadius() << ";\n";
    os << "  this.arc.ro = " << arc.outerRadius() << ";\n";
    os << "  this.arc.a1 = " << arc.angle1() << ";\n";
    os << "  this.arc.a2 = " << arc.angle2() << ";\n";
  }
}

void
CQChartsPlotObj::
writeScriptGC(CQChartsScriptPainter *device, const CQChartsPenBrush &penBrush) const
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
writeScriptInsideColor(CQChartsScriptPainter *device, bool isSave) const
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
CQChartsGroupObj(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox, const ColorInd &ig) :
 CQChartsPlotObj(plot, bbox, ColorInd(), ig, ColorInd())
{
}
