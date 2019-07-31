#include <CQChartsPlotObj.h>
#include <CQChartsPlot.h>
#include <CQPropertyViewModel.h>

CQChartsPlotObj::
CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect, const ColorInd &is,
                const ColorInd &ig, const ColorInd &iv) :
 CQChartsObj(plot->charts(), rect), plot_(plot), is_(is), ig_(ig), iv_(iv)
{
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

  str = plot_->idColumnString(ind.row(), ind.parent(), ok);

  return ok;
}

//---

double
CQChartsPlotObj::
xColorValue(bool relative) const
{
  double x = rect_.getXMid();

  if (relative) {
    const CQChartsGeom::Range &dataRange = plot_->dataRange();

    return CMathUtil::map(x, dataRange.xmin(), dataRange.xmax(), 0.0, 1.0);
  }
  else
    return x;
}

double
CQChartsPlotObj::
yColorValue(bool relative) const
{
  double y = rect_.getYMid();

  if (relative) {
    const CQChartsGeom::Range &dataRange = plot_->dataRange();

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
  return plot_->calcColorInd(this, nullptr, is_, ig_, iv_);
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
    QModelIndex ind2 = plot_->normalizeIndex(ind1);

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
    plot_->addSelectIndex(ind);
}

void
CQChartsPlotObj::
getHierSelectIndices(Indices &inds) const
{
  getSelectIndices(inds);

  if (plot_->idColumn().isValid())
    addColumnSelectIndex(inds, plot_->idColumn());

  if (plot_->visibleColumn().isValid())
    addColumnSelectIndex(inds, plot_->visibleColumn());

  if (plot_->colorColumn().isValid())
    addColumnSelectIndex(inds, plot_->colorColumn());
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

  QModelIndex ind = plot_->selectIndex(row, column.column(), parent);

  addSelectIndex(inds, ind);
}

void
CQChartsPlotObj::
addSelectIndex(Indices &inds, const QModelIndex &ind) const
{
  inds.insert(ind);
}

//------

CQChartsGroupObj::
CQChartsGroupObj(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox, const ColorInd &ig) :
 CQChartsPlotObj(plot, bbox, ColorInd(), ig, ColorInd())
{
}
