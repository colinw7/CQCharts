#include <CQChartsModel.h>

CQChartsModel::
CQChartsModel() :
 columnHeaders_(false)
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
  if (col < 0 || col >= int(columns_.size()))
    return QString();

  return columns_[col].type();
}

void
CQChartsModel::
setColumnType(int col, const QString &type)
{
  int n = columns_.size();

  if (col < 0 || col >= n)
    return;

  columns_[col].setType(type);
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
  if (section < 0 || section >= int(columns_.size()))
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
  }

  return QVariant();
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

    QString type = columnType(index.column());

    QString            baseType;
    CQChartsNameValues nameValues;

    CQChartsColumn::decodeType(type, baseType, nameValues);

    CQChartsColumnType *typeData = CQChartsColumnTypeMgrInst->getType(baseType);

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
