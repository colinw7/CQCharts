#include <CQChartsTsv.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQTsvModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsTsv::
CQChartsTsv(CQCharts *charts) :
 QSortFilterProxyModel(), charts_(charts)
{
  tsvModel_ = new CQTsvModel;

  exprModel_ = new CQExprModel(tsvModel_);

  setSourceModel(exprModel_);
}

CQChartsTsv::
~CQChartsTsv()
{
  delete exprModel_;
  delete tsvModel_;
}

void
CQChartsTsv::
setCommentHeader(bool b)
{
  tsvModel_->setCommentHeader(b);
}

void
CQChartsTsv::
setFirstLineHeader(bool b)
{
  tsvModel_->setFirstLineHeader(b);
}

bool
CQChartsTsv::
load(const QString &filename)
{
  return tsvModel_->load(filename);
}

int
CQChartsTsv::
columnCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::columnCount(parent);
}

int
CQChartsTsv::
rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

QVariant
CQChartsTsv::
headerData(int section, Qt::Orientation orientation, int role) const
{
  return QSortFilterProxyModel::headerData(section, orientation, role);
}

bool
CQChartsTsv::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsTsv::
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
CQChartsTsv::
parent(const QModelIndex &index) const
{
  return QSortFilterProxyModel::parent(index);
}

Qt::ItemFlags
CQChartsTsv::
flags(const QModelIndex &index) const
{
  return QSortFilterProxyModel::flags(index);
}
