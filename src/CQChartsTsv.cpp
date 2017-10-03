#include <CQChartsTsv.h>
#include <CQCharts.h>
#include <CQTsvModel.h>
#include <QSortFilterProxyModel>

CQChartsTsv::
CQChartsTsv(CQCharts *charts) :
 CQChartsModel(charts)
{
  setColumnHeaders(true);

  tsvModel_ = new CQTsvModel;

  proxyModel_ = new QSortFilterProxyModel;

  proxyModel_->setSourceModel(tsvModel_);
}

CQChartsTsv::
~CQChartsTsv()
{
  delete tsvModel_;
  delete proxyModel_;
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
  return proxyModel_->columnCount(parent);
}

int
CQChartsTsv::
rowCount(const QModelIndex &parent) const
{
  return proxyModel_->rowCount(parent);
}

QVariant
CQChartsTsv::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == CQCharts::Role::ColumnType)
    return CQChartsModel::headerData(section, orientation, role);

  return proxyModel_->headerData(section, orientation, role);
}

QVariant
CQChartsTsv::
data(const QModelIndex &index, int role) const
{
  if (! index.isValid())
    return QVariant();

  QVariant var = proxyModel_->data(index, role);

  if      (role == Qt::DisplayRole) {
    CQChartsNameValues nameValues;

    CQChartsColumnType *typeData = columnTypeData(index.column(), nameValues);

    if (typeData)
      return typeData->userData(var, nameValues);
  }

  return var;
}

QModelIndex
CQChartsTsv::
index(int row, int column, const QModelIndex &parent) const
{
  return proxyModel_->index(row, column, parent);
}

QModelIndex
CQChartsTsv::
parent(const QModelIndex &index) const
{
  return proxyModel_->parent(index);
}

Qt::ItemFlags
CQChartsTsv::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
