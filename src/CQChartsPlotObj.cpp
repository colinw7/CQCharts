#include <CQChartsPlotObj.h>

CQChartsPlotObj::
CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect) :
 CQChartsObj(nullptr, rect), plot_(plot)
{
  assert(! CMathUtil::isNaN(rect.getXMin()) && ! CMathUtil::isNaN(rect.getYMin()) &&
         ! CMathUtil::isNaN(rect.getXMax()) && ! CMathUtil::isNaN(rect.getYMax()));
}

bool
CQChartsPlotObj::
calcColumnId(const QModelIndex &ind, QString &str) const
{
  bool ok;

  str = plot_->idColumnString(ind.row(), ind.parent(), ok);

  return ok;
}

bool
CQChartsPlotObj::
isSelectIndex(const QModelIndex &ind) const
{
  Indices inds;

  getSelectIndices(inds);

  if (plot_->idColumn().isValid())
    addColumnSelectIndex(inds, plot_->idColumn());

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

  getSelectIndices(inds);

  for (const auto &ind : inds)
    plot_->addSelectIndex(ind);
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
CQChartsGroupObj(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox) :
 CQChartsPlotObj(plot, bbox)
{
}
