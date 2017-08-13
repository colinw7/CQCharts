#include <CQChartsTsv.h>
#include <CQTsvModel.h>

CQChartsTsv::
CQChartsTsv() :
 CQChartsModel()
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

QString
CQChartsTsv::
columnType(int col) const
{
  auto p = columnTypes_.find(col);

  if (p == columnTypes_.end())
    return QString();

  return (*p).second;
}

void
CQChartsTsv::
setColumnType(int col, const QString &type)
{
  columnTypes_[col] = type;
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
    QString type = columnType(index.column());

    QString            baseType;
    CQChartsNameValues nameValues;

    CQChartsColumn::decodeType(type, baseType, nameValues);

    CQChartsColumnType *typeData = CQChartsColumnTypeMgrInst->getType(baseType);

    if (typeData)
      return typeData->userData(var.toString(), nameValues);
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
