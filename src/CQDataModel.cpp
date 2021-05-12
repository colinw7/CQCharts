#include <CQDataModel.h>
#include <CQModelDetails.h>
#include <iostream>

CQDataModel::
CQDataModel()
{
  setObjectName("dataModel");

  connect(this, SIGNAL(columnTypeChanged(int)), this, SLOT(resetColumnCache(int)));
}

CQDataModel::
CQDataModel(int numCols, int numRows)
{
  setObjectName("dataModel");

  init(numCols, numRows);

  connect(this, SIGNAL(columnTypeChanged(int)), this, SLOT(resetColumnCache(int)));
}

CQDataModel::
~CQDataModel()
{
  delete details_;
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

void
CQDataModel::
resizeModel(int numCols, int numRows)
{
  beginResetModel();

  init(numCols, numRows);

  endResetModel();
}

//------

void
CQDataModel::
initFilter()
{
  setFilterInited(true);

  //---

  filterDatas_.clear();

  if (! hasFilter())
    return;

  //---

  int numHeaders = hheader_.size();

  auto patterns = filter_.split(",");

  for (int i = 0; i < patterns.size(); ++i) {
    FilterData filterData;

    auto fields = patterns[i].split(":");

    if (fields.length() == 2) {
      auto name  = fields[0];
      auto value = fields[1];

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
}

bool
CQDataModel::
acceptsRow(const Cells &cells) const
{
  if (! hasFilter())
    return true;

  //---

  if (! isFilterInited()) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! isFilterInited()) {
      auto *th = const_cast<CQDataModel *>(this);

      th->initFilter();
    }
  }

  //---

  for (std::size_t i = 0; i < filterDatas_.size(); ++i) {
    const FilterData &filterData = filterDatas_[i];

    if (! filterData.valid)
      continue;

    auto field = cells[filterData.column].toString();

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

      return CQBaseModel::headerData(section, orientation, role);
    }
    else if (role == Qt::EditRole) {
      if (hheader_[section].toString().length())
        return hheader_[section];

      return CQBaseModel::headerData(section, orientation, role);
    }
    else if (role == Qt::ToolTipRole) {
      auto var = hheader_[section];

      auto type = columnType(section);

      auto str = var.toString() + ":" + typeName(type);

      return QVariant(str);
    }
    else if (role == static_cast<int>(CQBaseModelRole::DataMin)) {
      auto *details = getDetails();

      return details->columnDetails(section)->minValue();
    }
    else if (role == static_cast<int>(CQBaseModelRole::DataMax)) {
      auto *details = getDetails();

      return details->columnDetails(section)->maxValue();
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
    else if (role == Qt::EditRole) {
      if (vheader_[section].toString().length())
        return vheader_[section];
      else
        return CQBaseModel::headerData(section, orientation, role);
    }
    else if (role == Qt::ToolTipRole) {
      return vheader_[section];
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

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
      hheader_[section] = value;

      emit headerDataChanged(orientation, section, section);

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

      emit headerDataChanged(orientation, section, section);

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

  int r = index.row();
  int c = index.column();

  int nr = data_.size();

  if (r < 0 || r >= nr)
    return QVariant();

  const Cells &cells = data_[r];

  int nc = int(cells.size());

  if (c < 0 || c >= nc)
    return QVariant();

  //---

  const auto &columnData = getColumnData(c);

  //---

  auto getRowRoleValue = [&](int row, int role, QVariant &value) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto pr = columnData.roleRowValues.find(role);
    if (pr == columnData.roleRowValues.end()) return false;

    const RowValues &rowValues = (*pr).second;

    auto pr1 = rowValues.find(row);
    if (pr1 == rowValues.end()) return false;

    value = (*pr1).second;

    return true;
  };

  auto setRowRoleValue = [&](int row, int role, const QVariant &value) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto &columnData1 = const_cast<ColumnData &>(columnData);

    columnData1.roleRowValues[role][row] = value;
  };

  //---

  if      (role == Qt::DisplayRole) {
    return cells[c];
  }
  else if (role == Qt::EditRole) {
    auto type = columnType(c);

    // check in cached values
    QVariant var;

    if (getRowRoleValue(r, int(CQBaseModelRole::CachedValue), var)) {
      if (type == CQBaseModelType::NONE || isSameType(var, type))
        return var;
    }

    // not cached so get raw value
    var = cells[c];

    // column has no type or already correct type then just return
    if (type == CQBaseModelType::NONE || isSameType(var, type))
      return var;

    // cache converted value
    if (var.type() == QVariant::String) {
      auto var1 = typeStringToVariant(var.toString(), type);

      if (var1.isValid())
        setRowRoleValue(r, int(CQBaseModelRole::CachedValue), var1);

      return var1;
    }

    return var;
  }
  else if (role == Qt::ToolTipRole) {
    return cells[c];
  }
  else if (role == int(CQBaseModelRole::RawValue) ||
           role == int(CQBaseModelRole::IntermediateValue) ||
           role == int(CQBaseModelRole::CachedValue) ||
           role == int(CQBaseModelRole::OutputValue)) {
    QVariant var;

    if (getRowRoleValue(r, role, var))
      return var;

    if (role == int(CQBaseModelRole::RawValue)) {
      return cells[c];
    }

    return QVariant();
  }
  else {
    QVariant var;

    if (getRowRoleValue(r, role, var))
      return var;

    return QVariant();
  }

  return CQBaseModel::data(index, role);
}

bool
CQDataModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (isReadOnly()) {
    std::cerr << "CQDataModel::setData for read only model\n";
    return false;
  }

  if (! index.isValid())
    return false;

  int r = index.row();
  int c = index.column();

  int nr = data_.size();

  if (r >= nr)
    return false;

  Cells &cells = data_[r];

//int nc = int(cells.size());
  int nc = columnCount();

  if (c < 0 || c >= nc)
    return false;

  //---

  auto &columnData = getColumnData(c);

  //---

  auto clearRowRoleValue = [&](int row, int role) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto pr = columnData.roleRowValues.find(role);
    if (pr == columnData.roleRowValues.end()) return false;

    RowValues &rowValues = (*pr).second;

    auto pr1 = rowValues.find(row);
    if (pr1 == rowValues.end()) return false;

    rowValues.erase(pr1);

    return true;
  };

  auto setRowRoleValue = [&](int row, int role, const QVariant &value) {
    std::unique_lock<std::mutex> lock(mutex_);

    columnData.roleRowValues[role][row] = value;
  };

  //---

  if      (role == Qt::DisplayRole) {
    //auto type = columnType(c);

    cells[c] = value;

    emit dataChanged(index, index, QVector<int>(1, role));
  }
  else if (role == Qt::EditRole) {
    //auto type = columnType(c);

    while (c >= int(cells.size()))
      cells.push_back(QVariant());

    cells[c] = value;

    clearRowRoleValue(r, int(CQBaseModelRole::RawValue));
    clearRowRoleValue(r, int(CQBaseModelRole::IntermediateValue));
    clearRowRoleValue(r, int(CQBaseModelRole::CachedValue));
    clearRowRoleValue(r, int(CQBaseModelRole::OutputValue));

    emit dataChanged(index, index, QVector<int>(1, role));
  }
  else if (role == int(CQBaseModelRole::RawValue) ||
           role == int(CQBaseModelRole::IntermediateValue) ||
           role == int(CQBaseModelRole::CachedValue) ||
           role == int(CQBaseModelRole::OutputValue)) {
    setRowRoleValue(r, role, value);
  }
  else {
    setRowRoleValue(r, role, value);

    emit dataChanged(index, index, QVector<int>(1, role));
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

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (! isReadOnly())
    flags |= Qt::ItemIsEditable;

  return flags;
}

void
CQDataModel::
resetColumnCache(int column)
{
  std::unique_lock<std::mutex> lock(mutex_);

  auto &columnData = getColumnData(column);

  columnData.roleRowValues.clear();
}

CQModelDetails *
CQDataModel::
getDetails() const
{
  if (! details_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! details_) {
      auto *th = const_cast<CQDataModel *>(this);

      th->details_ = new CQModelDetails(th);
    }
  }

  return details_;
}

//------

void
CQDataModel::
applyFilterColumns(const QStringList &columns)
{
  if (! columns.length())
    return;

  Data  data    = data_;
  Cells hheader = hheader_;

  std::map<int,int> columnMap;

  int nc1 = hheader_.size();
  int nc2 = columns.length();

  for (int c = 0; c < nc1; ++c)
    columnMap[c] = -1;

  for (int c = 0; c < nc2; ++c) {
    const auto &name = columns[c];

    // get index for matching column name
    int ind = -1;

    for (int c1 = 0; c1 < nc1; ++c1) {
      if (hheader[c1] == name) {
        ind = c1;
        break;
      }
    }

    // if name not found, try and convert column name to number
    if (ind == -1) {
      bool ok;

      int ind1 = name.toInt(&ok);

      if (ok && ind1 >= 0 && ind1 < nc1)
        ind = ind1;
    }

    if (ind == -1) {
      std::cerr << "Invalid column name '" << name.toStdString() << "'\n";
      continue;
    }

    columnMap[ind] = c;
  }

  // get new number of columns
  nc2 = 0;

  for (int c = 0; c < nc1; ++c) {
    int c1 = columnMap[c];

    if (c1 >= 0 && c1 < nc1)
      ++nc2;
  }

  // remap horizontal header and row data
  hheader_.clear(); hheader_.resize(nc2);

  int nr = data.size();

  for (int r = 0; r < nr; ++r) {
    Cells &cells1 = data [r]; // old data
    Cells &cells2 = data_[r]; // new data

    cells2.clear(); cells2.resize(nc2);

    for (int c = 0; c < nc1; ++c) {
      int c1 = columnMap[c];

      if (c1 < 0 || c1 >= nc1)
        continue;

      hheader_[c1] = hheader[c];
      cells2  [c1] = cells1 [c];
    }
  }
}
