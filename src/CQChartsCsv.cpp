#include <CQChartsCsv.h>
#include <CQCharts.h>
#include <CQCsvModel.h>

CQChartsCsv::
CQChartsCsv(CQCharts *charts) :
 CQChartsModel(charts)
{
  setColumnHeaders(true);

  model_ = new CQCsvModel;
}

CQChartsCsv::
~CQChartsCsv()
{
  delete model_;
}

void
CQChartsCsv::
setCommentHeader(bool b)
{
  model_->setCommentHeader(b);
}

void
CQChartsCsv::
setFirstLineHeader(bool b)
{
  model_->setFirstLineHeader(b);
}

bool
CQChartsCsv::
load(const QString &filename)
{
  return model_->load(filename);
}

int
CQChartsCsv::
columnCount(const QModelIndex &parent) const
{
  return model_->columnCount(parent);
}

int
CQChartsCsv::
rowCount(const QModelIndex &parent) const
{
  return model_->rowCount(parent);
}

QVariant
CQChartsCsv::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == CQCharts::Role::ColumnType)
    return CQChartsModel::headerData(section, orientation, role);

  return model_->headerData(section, orientation, role);
}

QVariant
CQChartsCsv::
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
CQChartsCsv::
index(int row, int column, const QModelIndex &parent) const
{
  return model_->index(row, column, parent);
}

QModelIndex
CQChartsCsv::
parent(const QModelIndex &index) const
{
  return model_->parent(index);
}

Qt::ItemFlags
CQChartsCsv::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
