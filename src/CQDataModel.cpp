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

CQDataModel::
~CQDataModel()
{
}

void
CQDataModel::
init(int numCols, int numRows)
{
  hheader_.resize(numCols);
  vheader_.resize(numRows);

  data_.resize(numRows);

  for (int i = 0; i < numRows; ++i)
    data_[i].resize(numCols);
}

//------

void
CQDataModel::
initFilter()
{
  int numHeaders = hheader_.size();

  filterDatas_.clear();

  QStringList patterns = filter_.split(",");

  for (int i = 0; i < patterns.size(); ++i) {
    FilterData filterData;

    QStringList fields = patterns[i].split(":");

    if (fields.length() == 2) {
      QString name  = fields[0];
      QString value = fields[1];

      filterData.column = -1;

      for (int j = 0; j < numHeaders; ++j) {
        if (hheader_[j] == name) {
          filterData.column = j;
          break;
        }
      }

      if (filterData.column == -1) {
        bool ok;

        filterData.column = name.toInt(&ok);

        if (! ok)
          filterData.column = -1;
      }

      filterData.regexp = QRegExp(value, Qt::CaseSensitive, QRegExp::Wildcard);
    }
    else {
      filterData.column = 0;
      filterData.regexp = QRegExp(patterns[i], Qt::CaseSensitive, QRegExp::Wildcard);
    }

    filterData.valid = (filterData.column >= 0 && filterData.column < numHeaders);

    filterDatas_.push_back(filterData);
  }

  setFilterInited(true);
}

bool
CQDataModel::
acceptsRow(const Cells &cells) const
{
  if (! hasFilter())
    return true;

  //---

  if (! isFilterInited()) {
    CQDataModel *th = const_cast<CQDataModel *>(this);

    th->initFilter();
  }

  //---

  for (std::size_t i = 0; i < filterDatas_.size(); ++i) {
    const FilterData &filterData = filterDatas_[i];

    if (! filterData.valid)
      continue;

    QString field = cells[filterData.column].toString();

    if (! filterData.regexp.exactMatch(field))
      return false;
  }

  return true;
}

//------

int
CQDataModel::
columnCount(const QModelIndex &) const
{
  return hheader_.size();
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
  if (orientation == Qt::Horizontal) {
    if (hheader_.empty())
      return CQBaseModel::headerData(section, orientation, role);

    int numCols = columnCount();

    if (section < 0 || section >= numCols)
      return QVariant();

    if      (role == Qt::DisplayRole) {
      if (hheader_[section].toString().length())
        return hheader_[section];

      QVariant var = CQBaseModel::headerData(section, orientation, role);

      if (var.isValid())
        return var;

      return "";
    }
    else if (role == Qt::ToolTipRole) {
      QVariant var = hheader_[section];

      Type type = columnType(section);

      QString str = var.toString() + ":" + typeName(type);

      return QVariant(str);
    }
    else if (role == Qt::EditRole) {
      if (hheader_[section].toString().length())
        return hheader_[section];

      QVariant var = CQBaseModel::headerData(section, orientation, role);

      if (var.isValid())
        return var;

      return "";
    }
    else {
      return CQBaseModel::headerData(section, orientation, role);
    }
  }
  else {
    if (vheader_.empty())
      return CQBaseModel::headerData(section, orientation, role);

    int numRows = rowCount();

    if (section < 0 || section >= numRows)
      return QVariant();

    if      (role == Qt::DisplayRole) {
      if (vheader_[section].toString().length())
        return vheader_[section];
      else
        return CQBaseModel::headerData(section, orientation, role);
    }
    else if (role == Qt::ToolTipRole) {
      return vheader_[section];
    }
    else if (role == Qt::EditRole) {
      if (vheader_[section].toString().length())
        return vheader_[section];
      else
        return CQBaseModel::headerData(section, orientation, role);
    }
    else {
      return CQBaseModel::headerData(section, orientation, role);
    }
  }
}

bool
CQDataModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (orientation == Qt::Horizontal) {
    if (hheader_.empty())
      return CQBaseModel::setHeaderData(section, orientation, value, role);

    int numCols = columnCount();

    if (section < 0 || section >= numCols)
      return false;

    if (role == Qt::DisplayRole) {
      hheader_[section] = value;

      return true;
    }
    else {
      return CQBaseModel::setHeaderData(section, orientation, value, role);
    }
  }
  else {
    if (vheader_.empty())
      return CQBaseModel::setHeaderData(section, orientation, value, role);

    int numRows = rowCount();

    if (section < 0 || section >= numRows)
      return false;

    if (role == Qt::DisplayRole) {
      vheader_[section] = value;

      return true;
    }
    else {
      return CQBaseModel::setHeaderData(section, orientation, value, role);
    }
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
  else if (role == Qt::EditRole) {
    if (index.row() < 0 || index.row() >= int(data_.size()))
      return QVariant();

    const Cells &cells = data_[index.row()];

    if (index.column() < 0 || index.column() >= int(cells.size()))
      return QVariant();

    QVariant var = cells[index.column()];

    return var;
  }
  else if (! isReadOnly()) {
    auto p1 = extraData_.find(index.row());
    if (p1 == extraData_.end()) return QVariant();

    const ColumnRoleVariant &columnRoleVariant = (*p1).second;

    auto p2 = columnRoleVariant.find(index.column());
    if (p2 == columnRoleVariant.end()) return QVariant();

    const RoleVariant &roleVariant = (*p2).second;

    auto p3 = roleVariant.find(index.column());
    if (p3 == roleVariant.end()) return QVariant();

    return (*p3).second;
  }

  return CQBaseModel::data(index, role);
}

bool
CQDataModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (isReadOnly())
    return false;

  if (! index.isValid())
    return false;

  if (index.row() >= int(data_.size()))
    return false;

  Cells &cells = data_[index.row()];

  if (index.column() >= int(cells.size()))
    return false;

  if      (role == Qt::DisplayRole) {
    //Type type = columnType(index.column());

    cells[index.column()] = value;
  }
  else if (role == Qt::EditRole) {
    //Type type = columnType(index.column());

    cells[index.column()] = value;
  }
  else {
    extraData_[index.row()][index.column()][role] = value;
  }

  return true;
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
