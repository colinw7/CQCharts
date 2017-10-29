#include <CQChartsGnuData.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQGnuDataModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsGnuData::
CQChartsGnuData(CQCharts *charts) :
 QSortFilterProxyModel(), charts_(charts)
{
  dataModel_ = new CQGnuDataModel;

  exprModel_ = new CQExprModel(dataModel_);

  setSourceModel(exprModel_);
}

CQChartsGnuData::
~CQChartsGnuData()
{
  delete exprModel_;
  delete dataModel_;
}

void
CQChartsGnuData::
setCommentHeader(bool b)
{
  dataModel_->setCommentHeader(b);
}

void
CQChartsGnuData::
setFirstLineHeader(bool b)
{
  dataModel_->setFirstLineHeader(b);
}

bool
CQChartsGnuData::
load(const QString &filename)
{
  return dataModel_->load(filename);
}

int
CQChartsGnuData::
columnCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::columnCount(parent);
}

int
CQChartsGnuData::
rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

QVariant
CQChartsGnuData::
headerData(int section, Qt::Orientation orientation, int role) const
{
  return QSortFilterProxyModel::headerData(section, orientation, role);
}

bool
CQChartsGnuData::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsGnuData::
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
CQChartsGnuData::
parent(const QModelIndex &index) const
{
  return QSortFilterProxyModel::parent(index);
}

Qt::ItemFlags
CQChartsGnuData::
flags(const QModelIndex &index) const
{
  return QSortFilterProxyModel::flags(index);
}
