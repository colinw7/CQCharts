#include <CQChartsPlotObj.h>

CQChartsPlotObj::
CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect) :
 plot_(plot), rect_(rect)
{
}

const QString &
CQChartsPlotObj::
id() const
{
  if (! id_) {
    const_cast<CQChartsPlotObj*>(this)->id_ = calcId();

    assert((*id_).length());
  }

  return *id_;
}

const QString &
CQChartsPlotObj::
tipId() const
{
  if (! tipId_) {
    const_cast<CQChartsPlotObj*>(this)->tipId_ = calcTipId();

    assert((*tipId_).length());
  }

  return *tipId_;
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
addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
               const QModelIndex &parent) const
{
  if (column.type() != CQChartsColumn::Type::DATA)
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
CQChartsGroupObj(CQChartsPlot *plot) :
 CQChartsPlotObj(plot, CQChartsGeom::BBox(0, 0, 1, 1))
{
}
