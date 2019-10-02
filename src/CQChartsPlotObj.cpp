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
writeScriptData(std::ostream &os) const
{
  auto encodeString = [&](const QString &str) {
    return CQChartsScriptPainter::encodeString(str).toStdString();
  };

  os << "  this.id    = \"" << this->id().toStdString() << "\";\n";
  os << "  this.tipId = \"" << encodeString(this->tipId()) << "\";\n";

  const CQChartsGeom::BBox &rect = this->rect();

  os << "  this.xmin = " << rect.getXMin() << ";\n";
  os << "  this.ymin = " << rect.getYMin() << ";\n";
  os << "  this.xmax = " << rect.getXMax() << ";\n";
  os << "  this.ymax = " << rect.getYMax() << ";\n";

  if (this->isPolygon()) {
    os << "  this.poly = [";

    QPolygonF poly = this->polygon();

    int np = poly.length();

    for (int i = 0; i < np; ++i) {
      if (i > 0) os << ", ";

      os << poly[i].x() << ", " << poly[i].y();
    }

    os << "];\n";
  }
}

//------

CQChartsGroupObj::
CQChartsGroupObj(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox, const ColorInd &ig) :
 CQChartsPlotObj(plot, bbox, ColorInd(), ig, ColorInd())
{
}
