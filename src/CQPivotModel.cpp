#include <CQPivotModel.h>
#include <CMathUtil.h>
#include <assert.h>

//------

CQPivotModel::
CQPivotModel(QAbstractItemModel *model)
{
  setObjectName("pivotModel");

  setSourceModel(model);

  setDataType(CQBaseModelDataType::PIVOT);
}

CQPivotModel::
~CQPivotModel()
{
}

QAbstractItemModel *
CQPivotModel::
sourceModel() const
{
  return sourceModel_;
}

void
CQPivotModel::
setSourceModel(QAbstractItemModel *sourceModel)
{
  sourceModel_ = sourceModel;

  invalidateModel();
}

//------

CQPivotModel::ValueType
CQPivotModel::
stringToValueType(const QString &str)
{
  auto str1 = str.toLower();

  if      (str1 == "count")
    return ValueType::COUNT;
  else if (str1 == "count_unique")
    return ValueType::COUNT_UNIQUE;
  else if (str1 == "sum")
    return ValueType::SUM;
  else if (str1 == "min")
    return ValueType::MIN;
  else if (str1 == "max")
    return ValueType::MAX;
  else if (str1 == "mean")
    return ValueType::MEAN;
  else
    return ValueType::NONE;
}

QString
CQPivotModel::
valueTypeToString(ValueType valueType)
{
  switch (valueType) {
    case ValueType::COUNT       : return "count";
    case ValueType::COUNT_UNIQUE: return "count_unique";
    case ValueType::SUM         : return "sum";
    case ValueType::MIN         : return "min";
    case ValueType::MAX         : return "max";
    case ValueType::MEAN        : return "mean";
    default: return "";
  }
}

//------

CQPivotModel::ColumnType
CQPivotModel::
columnType(Column column) const
{
  auto p = columnTypes_.find(column);
  if (p != columnTypes_.end()) return (*p).second;

  auto *sm = this->sourceModel();

  auto *baseModel = (sm ? dynamic_cast<CQBaseModel *>(sm) : nullptr);
  if (! baseModel) return CQBaseModelType::STRING;

  return baseModel->columnType(column);
}

void
CQPivotModel::
setColumnType(Column column, const ColumnType &t)
{
  columnTypes_[column] = t;
}

//------

// get number of columns
int
CQPivotModel::
columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  auto *sm = this->sourceModel();
  if (! sm) return 0;

  updateModel();

  // keys + vertical header + totals
  if (isIncludeTotals())
    return int(hColKeys_.size() + 2);
  else
    return int(hColKeys_.size() + 1);
}

// get number of child rows for parent
int
CQPivotModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  auto *sm = this->sourceModel();
  if (! sm) return 0;

  updateModel();

  // keys + totals
  if (isIncludeTotals())
    return int(vRowKeys_.size() + 1);
  else
    return int(vRowKeys_.size());
}

// get child node for row/column of parent
QModelIndex
CQPivotModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid())
    return QModelIndex();

  return createIndex(row, column, nullptr);
}

// get parent for child
QModelIndex
CQPivotModel::
parent(const QModelIndex &) const
{
  return QModelIndex();
}

bool
CQPivotModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  return false;
}

QVariant
CQPivotModel::
data(const QModelIndex &index, int role) const
{
  int r = index.row   ();
  int c = index.column();

  int nr = rowCount   ();
  int nc = columnCount();

  if (r < 0 || r >= nr)
    return QVariant();

  if (c < 0 || c >= nc)
    return QVariant();

  auto *sm = this->sourceModel();
  if (! sm) return QVariant();

  //---

  // vertical header
  if (c == 0) {
    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
      if (isIncludeTotals()) {
        if (r == nr - 1)
          return "Totals";
      }

      auto pv = vRowKeys_.find(r);
      assert(pv != vRowKeys_.end());

      return (*pv).second.key();
    }
    else
      return CQBaseModel::data(index, role);
  }

  //---

  // vertical totals
  if (isIncludeTotals()) {
    if (c == nc - 1) {
      if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        if (r == nr - 1)
          return data_.sum;

        return vdata_[size_t(r)].sum;
      }
      else
        return CQBaseModel::data(index, role);
    }
  }

  //---

  // horizontal totals
  if (isIncludeTotals()) {
    if (r == nr - 1) {
      if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        return hdata_[size_t(c - 1)].sum;
      }
      else
        return CQBaseModel::data(index, role);
    }
  }

  //---

  // grid data
  int c1 = c - 1;

  auto pv = vRowKeys_.find(r);
  assert(pv != vRowKeys_.end());

  auto ph = hColKeys_.find(c1);
  assert(ph != hColKeys_.end());

  const Keys &vkeys = (*pv).second;
  const Keys &hkeys = (*ph).second;

  if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
    auto p = values_.find(hkeys);
    if (p == values_.end()) return QVariant();

    auto p1 = (*p).second.find(vkeys);
    if (p1 == (*p).second.end()) return QVariant();

    const Values &values = (*p1).second;

    if      (values.count() != 0)
      return typeValue(values);
    else if (values.rcount())
      return values.rcount();
    else
      return calcFillValue();
  }
  else {
    return CQBaseModel::data(index, role);
  }
}

bool
CQPivotModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  int r = index.row   ();
  int c = index.column();

  int nr = rowCount   ();
  int nc = columnCount();

  if (r < 0 || r >= nr)
    return false;

  if (c < 0 || c >= nc)
    return false;

  auto *sm = this->sourceModel();
  if (! sm) return false;

  //---

  if      (role == Qt::DisplayRole) {
    return false;
  }
  else if (role == Qt::EditRole) {
    return false;
  }
  else {
    CQBaseModel::setData(index, value, role);
  }

  return true;
}

QVariant
CQPivotModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  auto *sm = this->sourceModel();
  if (! sm) return QVariant();

  if (orientation == Qt::Horizontal) {
    int nc = columnCount();

    if (section < 0 || section >= nc)
      return QVariant();

    // vertical header
    if (section == 0) {
      if (role == Qt::DisplayRole)
        return vheader_;
      else
        return CQBaseModel::headerData(section, orientation, role);
    }

    // totals
    if (isIncludeTotals()) {
      if (section == nc - 1) {
        if (role == Qt::DisplayRole)
          return "Totals";
        else
          return CQBaseModel::headerData(section, orientation, role);
      }
    }

    // horizontal keys
    int section1 = section - 1;

    auto ph = hColKeys_.find(section1);
    assert(ph != hColKeys_.end());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
      return (*ph).second.key();
    else
      return CQBaseModel::headerData(section, orientation, role);
  }
  else {
    return CQBaseModel::headerData(section, orientation, role);
  }
}

bool
CQPivotModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  auto *sm = this->sourceModel();
  if (! sm) return false;

  if (orientation == Qt::Horizontal) {
    int nc = columnCount();

    if (section < 0 || section >= nc)
      return false;

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
      return false;
    }
    else {
      return CQBaseModel::setHeaderData(section, orientation, value, role);
    }
  }
  else {
    return CQBaseModel::setHeaderData(section, orientation, value, role);
  }
}

Qt::ItemFlags
CQPivotModel::
flags(const QModelIndex &index) const
{
  auto *sm = this->sourceModel();
  if (! sm) return Qt::ItemFlags();

  int r = index.row   ();
  int c = index.column();

  int nr = rowCount   ();
  int nc = columnCount();

  if (c < 0 || c >= nc)
    return Qt::ItemFlags();

  if (r < 0 || r >= nr)
    return Qt::ItemFlags();

  //---

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//------

bool
CQPivotModel::
modelInds(const QString &hkey, const QString &vkey, Inds &inds) const
{
  Keys hkeys(KeyString(KeyString::STRING, hkey), separator());
  Keys vkeys(KeyString(KeyString::STRING, vkey), separator());

  auto p = values_.find(hkeys);
  if (p == values_.end()) return false;

  auto p1 = (*p).second.find(vkeys);
  if (p1 == (*p).second.end()) return false;

  const Values &values = (*p1).second;

  inds = values.inds();

  return true;
}

//------

void
CQPivotModel::
updateModel() const
{
  if (modelValid_)
    return;

  //---

  auto *th = const_cast<CQPivotModel *>(this);

  th->updateModel();
}

void
CQPivotModel::
updateModel()
{
  modelValid_ = true;

  //---

  bool multipleValueTypes = false;

  for (const auto &pv : columnValueTypes_) {
    if (pv.first >= 0 || pv.second.size() > 1) {
      multipleValueTypes = true;
      break;
    }
  }

  bool multipleValues = (valueColumns_.size() > 1);

  //---

  hKeysCol_.clear();
  vKeysRow_.clear();
  values_  .clear();

  auto *sm = sourceModel();

  int nr = sm->rowCount();

  for (int row = 0; row < nr; ++row) {
    Keys hkeys(separator());

    if (! hColumns_.empty()) {
      for (auto &column : hColumns_) {
        auto ind  = sm->index(row, column);
        auto data = sm->data(ind);

        if (data.isValid())
          hkeys.add(KeyString(KeyString::Type::STRING, data.toString()));
      }
    }

    //---

    Keys vkeys(separator());

    for (auto &column : vColumns_) {
      auto ind  = sm->index(row, column);
      auto data = sm->data(ind);

      if (data.isValid())
        vkeys.add(KeyString(KeyString::Type::STRING, data.toString()));
    }

    //---

    if (! hColumns_.empty() && ! multipleValues) {
      auto ph = hKeysCol_.find(hkeys);

      if (ph == hKeysCol_.end()) {
        auto col = hKeysCol_.size();

        hKeysCol_[hkeys] = int(col);
      }
    }

    auto pv = vKeysRow_.find(vkeys);

    if (pv == vKeysRow_.end()) {
      auto row = vKeysRow_.size();

      vKeysRow_[vkeys] = int(row);
    }

    //---

    if (! valueColumns_.empty()) {
      for (const auto &valueColumn : valueColumns_) {
        auto valueTypes = this->columnValueTypes(valueColumn);

        if (valueTypes.empty())
          valueTypes.push_back(ValueType::SUM);

        for (const auto &valueType : valueTypes) {
          auto hkeys1 = hkeys;

          if (hColumns_.empty() || multipleValues) {
            auto str = sm->headerData(valueColumn, Qt::Horizontal).toString();

            if (! multipleValues)
              hkeys1.add(KeyString(KeyString::Type::STRING, str));
            else
              hkeys1.addFront(KeyString(KeyString::Type::STRING, str));

            if (multipleValueTypes)
              hkeys1.add(KeyString(KeyString::Type::VALUE_TYPE, valueTypeToString(valueType)));

            auto ph = hKeysCol_.find(hkeys1);

            if (ph == hKeysCol_.end()) {
              auto col = hKeysCol_.size();

              hKeysCol_[hkeys1] = int(col);
            }
          }

          Values &values = values_[hkeys1][vkeys];

          values.setValueType(valueType);

          auto ind  = sm->index(row, valueColumn);
          auto data = sm->data(ind);

          if (data.isValid()) {
            bool ok;

            double r = data.toReal(&ok);

            if (ok)
              values.addReal(r);

            values.addValue(ind, data.toString());
          }
        }
      }
    }
    else {
      Values &values = values_[hkeys][vkeys];

      values.setValueType(this->valueType());

      values.addReal(1);
    }
  }

  //---

  hColKeys_.clear();
  vRowKeys_.clear();

  size_t col = 0;

  for (auto &p : hKeysCol_) {
    p.second = int(col++);

    hColKeys_[p.second] = p.first;
  }

  size_t row = 0;

  for (auto &p : vKeysRow_) {
    p.second = int(row++);

    vRowKeys_[p.second] = p.first;
  }

  //---

  // calc summary data
  calcData();

  //---

  // set horizontal summary header (keys)
  Keys hkeys(separator());

  if      (! hColumns_.empty()) {
    for (auto &column : hColumns_) {
      auto value = sm->headerData(column, Qt::Horizontal).toString();

      hkeys.add(KeyString(KeyString::STRING, value));
    }
  }
  else if (! valueColumns_.empty()) {
    for (auto &column : valueColumns_) {
      auto value = sm->headerData(column, Qt::Horizontal).toString();

      hkeys.add(KeyString(KeyString::STRING, value));
    }
  }

  hheader_ = hkeys.key();

  //---

  // set vertical summary header (keys)
  Keys vkeys(separator());

  for (auto &column : vColumns_) {
    auto value = sm->headerData(column, Qt::Horizontal).toString();

    vkeys.add(KeyString(KeyString::STRING, value));
  }

  vheader_ = vkeys.key();
}

void
CQPivotModel::
calcData()
{
  auto nr = vKeysRow_.size();

  vdata_.resize(nr);

  for (const auto &pv : vKeysRow_) {
    const auto &vkey = pv.first;
    int         r    = pv.second;

    ValueData data;

    for (const auto &ph : hKeysCol_) {
      const auto &hkey = ph.first;

      const Values &values = values_[hkey][vkey];

      if      (values.count() != 0) {
        auto value = typeValue(values);

        data.min = (data.set ? std::min(data.min, value) : value);
        data.max = (data.set ? std::max(data.max, value) : value);

        if (value.type() == QVariant::Double) {
          bool ok;
          data.sum = data.sum.toDouble(&ok) + value.toDouble(&ok);
        }
        else
          data.sum = data.sum.toString() + value.toString();

        data.set  = true;
      }
      else if (values.rcount()) {
        data.min = 0;
        data.max = 1;

        bool ok;
        data.sum = data.sum.toDouble(&ok) + values.rcount();

        data.set = true;
      }
    }

    assert(r >= 0 && size_t(r) < nr);

    vdata_[size_t(r)] = data;
  }

  //---

  auto nc = hKeysCol_.size();

  hdata_.resize(nc);

  for (const auto &ph : hKeysCol_) {
    const auto &hkey = ph.first;
    int         c    = ph.second;

    ValueData data;

    for (const auto &pv : vKeysRow_) {
      const auto &vkey = pv.first;

      const Values &values = values_[hkey][vkey];

      if      (values.count() != 0) {
        auto value = typeValue(values);

        data.min = (data.set ? std::min(data.min, value) : value);
        data.max = (data.set ? std::max(data.max, value) : value);
        data.sum = value;
        data.set = true;
      }
      else if (values.rcount()) {
        data.min  = 0;
        data.max  = 1;

        bool ok;
        data.sum = data.sum.toDouble(&ok) + values.rcount();

        data.set  = true;
      }
    }

    assert(c >= 0 && size_t(c) < nc);

    hdata_[size_t(c)] = data;
  }

  //---

  data_ = ValueData();

  for (const auto &d : vdata_) {
    data_.min  = (data_.set ? std::min(data_.min, d.min) : d.min);
    data_.max  = (data_.set ? std::max(data_.max, d.max) : d.max);
    data_.sum  = d.sum;
    data_.set  = true;
  }
}

QVariant
CQPivotModel::
typeValue(const Values &values) const
{
  if      (values.valueType() == ValueType::SUM) {
    if (values.dataType() == ColumnType::REAL)
      return values.rsum();
    else
      return values.ssum();
  }
  else if (values.valueType() == ValueType::MIN) {
    if (values.dataType() == ColumnType::REAL)
      return values.rmin();
    else
      return values.smin();
  }
  else if (values.valueType() == ValueType::MAX) {
    if (values.dataType() == ColumnType::REAL)
      return values.rmax();
    else
      return values.smax();
  }
  else if (values.valueType() == ValueType::MEAN) {
    if (values.dataType() == ColumnType::REAL)
      return values.rmean();
    else
      return calcFillValue();
  }
  else if (values.valueType() == ValueType::COUNT)
    return values.count();
  else if (values.valueType() == ValueType::COUNT_UNIQUE)
    return values.countUnique();
  else {
    assert(false);
    return calcFillValue();
  }
}

QStringList
CQPivotModel::
hkeys(bool sorted) const
{
  updateModel();

  QStringList strs;

  if (sorted) {
    for (const auto &ph : hKeysCol_)
      strs << ph.first.key();
  }
  else {
    for (const auto &ph : hColKeys_)
      strs << ph.second.key();
  }

  return strs;
}

QStringList
CQPivotModel::
vkeys(bool sorted) const
{
  updateModel();

  QStringList strs;

  if (sorted) {
    for (const auto &ph : vKeysRow_)
      strs << ph.first.key();
  }
  else {
    for (const auto &ph : vRowKeys_)
      strs << ph.second.key();
  }

  return strs;
}

int
CQPivotModel::
hkeyCol(const QString &key) const
{
  Keys keys(KeyString(KeyString::STRING, key), separator());

  updateModel();

  auto p = hKeysCol_.find(keys);
  if (p == hKeysCol_.end()) return -1;

  return (*p).second;
}

int
CQPivotModel::
vkeyRow(const QString &key) const
{
  Keys keys(KeyString(KeyString::STRING, key), separator());

  updateModel();

  auto p = vKeysRow_.find(keys);
  if (p == vKeysRow_.end()) return -1;

  return (*p).second;
}

QVariant
CQPivotModel::
hmin(int c) const
{
  assert(c >= 0 && c <= int(hdata_.size()));

  return hdata_[size_t(c)].min;
}

QVariant
CQPivotModel::
hmax(int c) const
{
  assert(c >= 0 && c <= int(hdata_.size()));

  return hdata_[size_t(c)].max;
}

QVariant
CQPivotModel::
vmin(int r) const
{
  assert(r >= 0 && r <= int(vdata_.size()));

  return vdata_[size_t(r)].min;
}

QVariant
CQPivotModel::
vmax(int r) const
{
  assert(r >= 0 && r <= int(vdata_.size()));

  return vdata_[size_t(r)].max;
}

QVariant
CQPivotModel::
calcFillValue() const
{
  if (fillValue().isValid())
    return fillValue();
  else
    return CMathUtil::getNaN();
}
