#include <CQDataModel.h>

CQDataModel::
CQDataModel()
{
}

CQDataModel::
CQDataModel(int numCols, int numRows)
{
  init(numCols, numRows);
}

void
CQDataModel::
init(int numCols, int numRows)
{
  header_.resize(numCols);

  data_.resize(numRows);

  for (int i = 0; i < numRows; ++i)
    data_[i].resize(numCols);
}

int
CQDataModel::
columnCount(const QModelIndex &) const
{
  return header_.size();
}

int
CQDataModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return data_.size();
}

QVariant
CQDataModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QVariant();

  int numCols = columnCount();

  if (section < 0 || section >= numCols)
    return QVariant();

  if      (role == Qt::DisplayRole) {
    return header_[section];
  }
  else if (role == Qt::ToolTipRole) {
    QVariant var = header_[section];

    Type type = columnType(section);

    QString str = var.toString() + ":" + typeName(type);

    return QVariant(str);
  }
  else if (role == Qt::EditRole) {
    return QVariant();
  }
  else {
    return CQBaseModel::headerData(section, orientation, role);
  }
}

bool
CQDataModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (orientation != Qt::Horizontal)
    return false;

  int numCols = columnCount();

  if (section < 0 || section >= numCols)
    return false;

  if (role == Qt::DisplayRole) {
    header_[section] = value;

    return true;
  }
  else {
    return CQBaseModel::setHeaderData(section, orientation, value, role);
  }
}

QVariant
CQDataModel::
data(const QModelIndex &index, int role) const
{
  if (! index.isValid())
    return QVariant();

  if      (role == Qt::DisplayRole) {
    if (index.row() < 0 || index.row() >= int(data_.size()))
      return QVariant();

    const Cells &cells = data_[index.row()];

    if (index.column() < 0 || index.column() >= int(cells.size()))
      return QVariant();

    QVariant var = cells[index.column()];

    Type type = columnType(index.column());

    if (type == Type::NONE)
      return var;

    if (! isSameType(var, type))
      return typeStringToVariant(var.toString(), type);

    return var;
  }
  else if (role == Qt::UserRole) {
    if (index.row() < 0 || index.row() >= int(data_.size()))
      return QVariant();

    const Cells &cells = data_[index.row()];

    if (index.column() < 0 || index.column() >= int(cells.size()))
      return QVariant();

    QVariant var = cells[index.column()];

    return var;
  }

  return QVariant();
}

bool
CQDataModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (isReadOnly())
    return false;

  if (! index.isValid())
    return false;

  if (role == Qt::DisplayRole) {
    if (index.row() >= int(data_.size()))
      return false;

    Cells &cells = data_[index.row()];

    if (index.column() >= int(cells.size()))
      return false;

    //Type type = columnType(index.column());

    cells[index.column()] = value;

    return true;
  }

  return false;
}

QModelIndex
CQDataModel::
index(int row, int column, const QModelIndex &) const
{
  return createIndex(row, column, nullptr);
}

QModelIndex
CQDataModel::
parent(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  return QModelIndex();
}

Qt::ItemFlags
CQDataModel::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
