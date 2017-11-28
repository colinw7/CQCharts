#include <CQChartsExprData.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQDataModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsExprData::
CQChartsExprData(CQCharts *charts, int nc, int nr) :
 QSortFilterProxyModel(), charts_(charts)
{
  dataModel_ = new CQDataModel(nc, nr);

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = dataModel_->index(r, c);

      dataModel_->setData(ind, QVariant(r*nc + c));
    }
  }

  exprModel_ = new CQExprModel(dataModel_);

  setSourceModel(exprModel_);
}

CQChartsExprData::
~CQChartsExprData()
{
  delete exprModel_;
  delete dataModel_;
}

int
CQChartsExprData::
columnCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::columnCount(parent);
}

int
CQChartsExprData::
rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

QVariant
CQChartsExprData::
headerData(int section, Qt::Orientation orientation, int role) const
{
  return QSortFilterProxyModel::headerData(section, orientation, role);
}

bool
CQChartsExprData::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsExprData::
data(const QModelIndex &index, int role) const
{
  QVariant var = QSortFilterProxyModel::data(index, role);

  if (role == Qt::UserRole && ! var.isValid())
    var = QSortFilterProxyModel::data(index, Qt::DisplayRole);

  if (role == Qt::DisplayRole || role == Qt::UserRole) {
    if (! index.isValid())
      return QVariant();

    assert(index.model() == this);

    QModelIndex index1 = QSortFilterProxyModel::mapToSource(index);

    assert(index.column() == index1.column());

    CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

    if (role == Qt::DisplayRole)
      return columnTypeMgr->getDisplayData(this, index1.column(), var);
    else
      return columnTypeMgr->getUserData(this, index1.column(), var);
  }

  return var;
}

QModelIndex
CQChartsExprData::
parent(const QModelIndex &index) const
{
  return QSortFilterProxyModel::parent(index);
}

Qt::ItemFlags
CQChartsExprData::
flags(const QModelIndex &index) const
{
  return QSortFilterProxyModel::flags(index);
}
