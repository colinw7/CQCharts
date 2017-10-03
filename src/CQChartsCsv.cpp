#include <CQChartsCsv.h>
#include <CQCharts.h>
#include <CQCsvModel.h>
#include <QSortFilterProxyModel>

CQChartsCsv::
CQChartsCsv(CQCharts *charts) :
 CQChartsModel(charts)
{
  setColumnHeaders(true);

  csvModel_ = new CQCsvModel;

  proxyModel_ = new QSortFilterProxyModel;

  proxyModel_->setSourceModel(csvModel_);
}

CQChartsCsv::
~CQChartsCsv()
{
  delete csvModel_;
  delete proxyModel_;
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
  return proxyModel_->columnCount(parent);
}

int
CQChartsCsv::
rowCount(const QModelIndex &parent) const
{
  return proxyModel_->rowCount(parent);
}

bool
CQChartsCsv::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (orientation != Qt::Horizontal)
    return false;

  if (role == CQCharts::Role::ColumnType) {
    setColumnType(section, value.toString());

    return true;
  }

  return false;
}

QVariant
CQChartsCsv::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == CQCharts::Role::ColumnType)
    return CQChartsModel::headerData(section, orientation, role);

  return proxyModel_->headerData(section, orientation, role);
}

QVariant
CQChartsCsv::
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
CQChartsCsv::
index(int row, int column, const QModelIndex &parent) const
{
  return proxyModel_->index(row, column, parent);
}

QModelIndex
CQChartsCsv::
parent(const QModelIndex &index) const
{
  return proxyModel_->parent(index);
}

Qt::ItemFlags
CQChartsCsv::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
