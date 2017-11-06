#include <CQChartsCsv.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQCsvModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsCsv::
CQChartsCsv(CQCharts *charts) :
 QSortFilterProxyModel(), charts_(charts)
{
  csvModel_ = new CQCsvModel;

  exprModel_ = new CQExprModel(csvModel_);

  setSourceModel(exprModel_);
}

CQChartsCsv::
~CQChartsCsv()
{
  delete exprModel_;
  delete csvModel_;
}

void
CQChartsCsv::
setCommentHeader(bool b)
{
  csvModel_->setCommentHeader(b);
}

void
CQChartsCsv::
setFirstLineHeader(bool b)
{
  csvModel_->setFirstLineHeader(b);
}

bool
CQChartsCsv::
load(const QString &filename)
{
  return csvModel_->load(filename);
}

int
CQChartsCsv::
columnCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::columnCount(parent);
}

int
CQChartsCsv::
rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

QVariant
CQChartsCsv::
headerData(int section, Qt::Orientation orientation, int role) const
{
  return QSortFilterProxyModel::headerData(section, orientation, role);
}

bool
CQChartsCsv::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsCsv::
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
CQChartsCsv::
parent(const QModelIndex &index) const
{
  return QSortFilterProxyModel::parent(index);
}

Qt::ItemFlags
CQChartsCsv::
flags(const QModelIndex &index) const
{
  return QSortFilterProxyModel::flags(index);
}

bool
CQChartsCsv::
filterAcceptsRow(int row, const QModelIndex &parent) const
{
  return QSortFilterProxyModel::filterAcceptsRow(row, parent);
}
