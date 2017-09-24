#include <CQChartsModel.h>
#include <CQCharts.h>

CQChartsModel::
CQChartsModel(CQCharts *charts) :
 charts_(charts), columnHeaders_(false)
{
}

void
CQChartsModel::
addColumn(const QString &name)
{
  columns_.emplace_back(name);
}

QString
CQChartsModel::
columnType(int col) const
{
  int n = columnCount();

  if (col < 0 || col >= n)
    return QString();

  if (n > int(columns_.size())) {
    CQChartsModel *th = const_cast<CQChartsModel *>(this);

    while (n > int(th->columns_.size()))
      th->columns_.emplace_back("");
  }

  return columns_[col].type();
}

bool
CQChartsModel::
setColumnType(int col, const QString &type)
{
  int n = columnCount();

  if (col < 0 || col >= n)
    return false;

  if (! isValidColumnType(type))
    return false;

  while (n > int(columns_.size()))
    columns_.emplace_back("");

  columns_[col].setType(type);

  return true;
}

int
CQChartsModel::
columnCount(const QModelIndex &) const
{
  return columns_.size();
}

QVariant
CQChartsModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  int n = columnCount();

  if (section < 0 || section >= n)
    return QVariant();

  const CQChartsColumn &column = columns_[section];

  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      return QVariant(column.name());
    }
    else if (role == Qt::EditRole) {
      return QVariant();
    }
    else if (role == Qt::ToolTipRole) {
      return QVariant(QString("%1\n%2").arg(column.name()).arg(column.type()));
    }
    else if (role == CQCharts::Role::ColumnType) {
      return QVariant(columnType(section));
    }
  }

  return QVariant();
}

bool
CQChartsModel::
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
CQChartsModel::
data(const QModelIndex &index, int role) const
{
  if (! index.isValid())
    return QVariant();

  if      (role == Qt::UserRole) {
    const Cells &cells = data_[index.row()];

    if (index.column() < 0 || index.column() >= int(cells.size()))
      return QVariant();

    const QString &cell = cells[index.column()];

    //---

    CQChartsNameValues nameValues;

    CQChartsColumnType *typeData = columnTypeData(index.column(), nameValues);

    if (typeData) {
      return typeData->userData(cell, nameValues);
    }
    else {
      return cell;
    }
  }
  else if (role == Qt::DisplayRole) {
    const Cells &cells = data_[index.row()];

    if (index.column() < 0 || index.column() >= int(cells.size()))
      return QVariant();

    const QString &cell = cells[index.column()];

    return cell;
  }

  return QVariant();
}

CQChartsColumnType *
CQChartsModel::
columnTypeData(int column, CQChartsNameValues &nameValues) const
{
  QString type = columnType(column);

  return charts_->columnTypeMgr()->decodeTypeData(type, nameValues);
}

bool
CQChartsModel::
isValidColumnType(const QString &type) const
{
  QString            baseType;
  CQChartsNameValues nameValues;

  CQChartsColumn::decodeType(type, baseType, nameValues);

  CQChartsColumnType *typeData = charts_->columnTypeMgr()->getType(baseType);

  return typeData;
}

QModelIndex
CQChartsModel::
index(int row, int column, const QModelIndex &) const
{
  return createIndex(row, column, nullptr);
}

QModelIndex
CQChartsModel::
parent(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  return QModelIndex();
}

int
CQChartsModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return data_.size();
}
