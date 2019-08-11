#include <CQDataModel.h>
#include <CQModelDetails.h>

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

      return CQBaseModel::headerData(section, orientation, role);
    }
    else if (role == Qt::EditRole) {
      if (hheader_[section].toString().length())
        return hheader_[section];

      return CQBaseModel::headerData(section, orientation, role);
    }
    else if (role == Qt::ToolTipRole) {
      QVariant var = hheader_[section];

      CQBaseModelType type = columnType(section);

      QString str = var.toString() + ":" + typeName(type);

      return QVariant(str);
    }
    else if (role == static_cast<int>(CQBaseModelRole::DataMin)) {
      CQModelDetails *details = getDetails();

      return details->columnDetails(section)->minValue();
    }
    else if (role == static_cast<int>(CQBaseModelRole::DataMax)) {
      CQModelDetails *details = getDetails();

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

  int nr = data_.size();

  if (index.row() < 0 || index.row() >= nr)
    return QVariant();

  const Cells &cells = data_[index.row()];

  int nc = int(cells.size());

  if (index.column() < 0 || index.column() >= nc)
    return QVariant();

  const ColumnData &columnData = getColumnData(index.column());

  if      (role == Qt::DisplayRole) {
    return cells[index.column()];
  }
  else if (role == Qt::EditRole) {
    CQBaseModelType type = columnType(index.column());

    // check in cached values
    auto pr = columnData.roleRowValues.find(int(CQBaseModelRole::CachedValue));

    if (pr != columnData.roleRowValues.end()) {
      const RowValues &rowValues = (*pr).second;

      auto pr1 = rowValues.find(index.row());

      if (pr1 != rowValues.end()) {
        const QVariant &var = (*pr1).second;

        if (type == CQBaseModelType::NONE || isSameType(var, type))
          return var;
      }
    }

    // not cached so get raw value
    QVariant var = cells[index.column()];

    // column has no type or already correct type then just return
    if (type == CQBaseModelType::NONE || isSameType(var, type))
      return var;

    // cache converted value
    if (var.type() == QVariant::String) {
      std::unique_lock<std::mutex> lock(mutex_);

      QVariant var1 = typeStringToVariant(var.toString(), type);

      ColumnData &columnData1 = const_cast<ColumnData &>(columnData);

      columnData1.roleRowValues[int(CQBaseModelRole::CachedValue)][index.row()] = var1;

      return var1;
    }

    return var;
  }
  else if (role == Qt::ToolTipRole) {
    return cells[index.column()];
  }
  else if (role == int(CQBaseModelRole::RawValue) ||
           role == int(CQBaseModelRole::IntermediateValue) ||
           role == int(CQBaseModelRole::CachedValue) ||
           role == int(CQBaseModelRole::OutputValue)) {
    auto pr = columnData.roleRowValues.find(role);

    if (pr != columnData.roleRowValues.end()) {
      const RowValues &rowValues = (*pr).second;

      auto pr1 = rowValues.find(index.row());

      if (pr1 != rowValues.end())
        return (*pr1).second;
    }

    if (role == int(CQBaseModelRole::RawValue)) {
      return cells[index.column()];
    }

    return QVariant();
  }
  else if (! isReadOnly()) {
    auto pr = columnData.roleRowValues.find(role);

    if (pr == columnData.roleRowValues.end())
      return QVariant();

    const RowValues &rowValues = (*pr).second;

    auto pr1 = rowValues.find(index.row());

    if (pr1 == rowValues.end())
      return QVariant();

    return (*pr1).second;
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

  ColumnData &columnData = getColumnData(index.column());

  if      (role == Qt::DisplayRole) {
    //CQBaseModelType type = columnType(index.column());

    cells[index.column()] = value;
  }
  else if (role == Qt::EditRole) {
    //CQBaseModelType type = columnType(index.column());

    cells[index.column()] = value;
  }
  else if (role == int(CQBaseModelRole::RawValue) ||
           role == int(CQBaseModelRole::IntermediateValue) ||
           role == int(CQBaseModelRole::CachedValue) ||
           role == int(CQBaseModelRole::OutputValue)) {
    std::unique_lock<std::mutex> lock(mutex_);

    columnData.roleRowValues[role][index.row()] = value;
  }
  else {
    std::unique_lock<std::mutex> lock(mutex_);

    columnData.roleRowValues[role][index.row()] = value;
  }

  emit dataChanged(index, index, QVector<int>(1, role));

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

void
CQDataModel::
resetColumnCache(int column)
{
  ColumnData &columnData = getColumnData(column);

  columnData.roleRowValues.clear();
}

CQModelDetails *
CQDataModel::
getDetails() const
{
  if (! details_) {
    CQDataModel *th = const_cast<CQDataModel *>(this);

    th->details_ = new CQModelDetails(th);
  }

  return details_;
}
