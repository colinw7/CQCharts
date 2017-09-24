#include <CQChartsTsv.h>
#include <CQCharts.h>
#include <CQTsvModel.h>

CQChartsTsv::
CQChartsTsv(CQCharts *charts) :
 CQChartsModel(charts)
{
  setColumnHeaders(true);

  model_ = new CQTsvModel;
}

void
CQChartsTsv::
setCommentHeader(bool b)
{
  model_->setCommentHeader(b);
}

void
CQChartsTsv::
setFirstLineHeader(bool b)
{
  model_->setFirstLineHeader(b);
}

bool
CQChartsTsv::
load(const QString &filename)
{
  return model_->load(filename);
}

int
CQChartsTsv::
columnCount(const QModelIndex &parent) const
{
  return model_->columnCount(parent);
}

int
CQChartsTsv::
rowCount(const QModelIndex &parent) const
{
  return model_->rowCount(parent);
}

QVariant
CQChartsTsv::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == CQCharts::Role::ColumnType)
    return CQChartsModel::headerData(section, orientation, role);

  return model_->headerData(section, orientation, role);
}

QVariant
CQChartsTsv::
data(const QModelIndex &index, int role) const
{
  if (! index.isValid())
    return QVariant();

  QVariant var = model_->data(index, role);

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
  return model_->index(row, column, parent);
}

QModelIndex
CQChartsTsv::
parent(const QModelIndex &index) const
{
  return model_->parent(index);
}

Qt::ItemFlags
CQChartsTsv::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
