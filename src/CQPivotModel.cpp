#include <CQPivotModel.h>
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

// get number of columns
int
CQPivotModel::
columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  QAbstractItemModel *model = this->sourceModel();
  if (! model) return 0;

  updateModel();

  // keys + vertical header + totals
  if (isIncludeTotals())
    return hColKeys_.size() + 2;
  else
    return hColKeys_.size() + 1;
}

// get number of child rows for parent
int
CQPivotModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  QAbstractItemModel *model = this->sourceModel();
  if (! model) return 0;

  updateModel();

  // keys + totals
  if (isIncludeTotals())
    return vRowKeys_.size() + 1;
  else
    return vRowKeys_.size();
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

  QAbstractItemModel *model = this->sourceModel();
  if (! model) return QVariant();

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

        return vdata_[r].sum;
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
        return hdata_[c - 1].sum;
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
    auto p = values_.find(hkeys.key());
    if (p == values_.end()) return QVariant();

    auto p1 = (*p).second.find(vkeys.key());
    if (p1 == (*p).second.end()) return QVariant();

    const Values &values = (*p1).second;

    if (values.count() == 0)
      return QVariant();

    return typeValue(values);
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

  QAbstractItemModel *model = this->sourceModel();
  if (! model) return false;

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
  QAbstractItemModel *model = this->sourceModel();
  if (! model) return QVariant();

  int nc = columnCount();

  if (section < 0 || section >= nc)
    return QVariant();

  if (orientation == Qt::Horizontal) {
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
  QAbstractItemModel *model = this->sourceModel();
  if (! model) return false;

  int nc = columnCount();

  if (section < 0 || section >= nc)
    return false;

  if (orientation == Qt::Horizontal) {
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
  QAbstractItemModel *model = this->sourceModel();
  if (! model) return 0;

  int r = index.row   ();
  int c = index.column();

  int nr = rowCount   ();
  int nc = columnCount();

  if (c < 0 || c >= nc)
    return 0;

  if (r < 0 || r >= nr)
    return 0;

  //---

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//------

bool
CQPivotModel::
modelInds(const QString &hkey, const QString &vkey, Inds &inds) const
{
  auto p = values_.find(hkey);
  if (p == values_.end()) return false;

  auto p1 = (*p).second.find(vkey);
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

  CQPivotModel *th = const_cast<CQPivotModel *>(this);

  th->updateModel();
}

void
CQPivotModel::
updateModel()
{
  modelValid_ = true;

  hKeysCol_.clear();
  hColKeys_.clear();
  vKeysRow_.clear();
  vRowKeys_.clear();
  values_  .clear();

  QAbstractItemModel *sm = sourceModel();

  int nr = sm->rowCount();

  for (int row = 0; row < nr; ++row) {
    Keys hkeys;

    for (auto &column : hColumns_) {
      QModelIndex ind = sm->index(row, column);

      QVariant data = sm->data(ind);

      if (data.isValid())
        hkeys.add(data.toString());
    }

    //---

    Keys vkeys;

    for (auto &column : vColumns_) {
      QModelIndex ind = sm->index(row, column);

      QVariant data = sm->data(ind).toString();

      if (data.isValid())
        vkeys.add(data.toString());
    }

    //---

    auto ph = hKeysCol_.find(hkeys.key());

    if (ph == hKeysCol_.end()) {
      int col = hKeysCol_.size();

      hKeysCol_[hkeys.key()] = col;
      hColKeys_[col        ] = hkeys;
    }

    auto pv = vKeysRow_.find(vkeys.key());

    if (pv == vKeysRow_.end()) {
      int row = vKeysRow_.size();

      vKeysRow_[vkeys.key()] = row;
      vRowKeys_[row        ] = vkeys;
    }

    QModelIndex ind = sm->index(row, valueColumn_);

    QVariant data = sm->data(ind);

    if (data.isValid()) {
      Values &values = values_[hkeys.key()][vkeys.key()];

      bool ok;

      double r = data.toReal(&ok);

      if (ok)
        values.add(r);

      values.add(ind, data.toString());
    }
  }

  //---

  // calc summary data
  calcData();

  //---

  // set horizontal header (keys)
  Keys hkeys;

  for (auto &column : hColumns_) {
    QString value = sm->headerData(column, Qt::Horizontal).toString();

    hkeys.add(value);
  }

  hheader_ = hkeys.key();

  //---

  // set vertical header (keys)
  Keys vkeys;

  for (auto &column : vColumns_) {
    QString value = sm->headerData(column, Qt::Horizontal).toString();

    vkeys.add(value);
  }

  vheader_ = vkeys.key();
}

void
CQPivotModel::
calcData()
{
  int nr = vKeysRow_.size();

  vdata_.resize(nr);

  for (const auto &pv : vKeysRow_) {
    const QString &vkey = pv.first;
    int            r    = pv.second;

    ValueData data;

    for (const auto &ph : hKeysCol_) {
      const QString &hkey = ph.first;

      const Values &values = values_[hkey][vkey];

      if (values.count() == 0)
        continue;

      double value = typeValue(values);

      data.min  = (data.set ? std::min(data.min, value) : value);
      data.max  = (data.set ? std::max(data.max, value) : value);
      data.sum += value;
      data.set  = true;
    }

    assert(r >= 0 && r < nr);

    vdata_[r] = data;
  }

  //---

  int nc = hKeysCol_.size();

  hdata_.resize(nc);

  for (const auto &ph : hKeysCol_) {
    const QString &hkey = ph.first;
    int            c    = ph.second;

    ValueData data;

    for (const auto &pv : vKeysRow_) {
      const QString &vkey = pv.first;

      const Values &values = values_[hkey][vkey];

      if (values.count() == 0)
        continue;

      double value = typeValue(values);

      data.min = (data.set ? std::min(data.min, value) : value);
      data.max = (data.set ? std::max(data.max, value) : value);
      data.sum = value;
      data.set = true;
    }

    assert(c >= 0 && c < nc);

    hdata_[c] = data;
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

double
CQPivotModel::
typeValue(const Values &values) const
{
  if      (valueType() == ValueType::SUM)
    return values.sum();
  else if (valueType() == ValueType::MIN)
    return values.min();
  else if (valueType() == ValueType::MAX)
    return values.max();
  else if (valueType() == ValueType::MEAN)
    return values.mean();
  else if (valueType() == ValueType::COUNT)
    return values.count();
  else if (valueType() == ValueType::COUNT_UNIQUE)
    return values.countUnique();
  else {
    assert(false);
    return 0.0;
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
      strs << ph.first;
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
      strs << ph.first;
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
  updateModel();

  auto p = hKeysCol_.find(key);
  if (p == hKeysCol_.end()) return -1;

  return (*p).second;
}

int
CQPivotModel::
vkeyRow(const QString &key) const
{
  updateModel();

  auto p = vKeysRow_.find(key);
  if (p == vKeysRow_.end()) return -1;

  return (*p).second;
}

double
CQPivotModel::
hmin(int c) const
{
  assert(c >= 0 && c <= int(hdata_.size()));

  return hdata_[c].min;
}

double
CQPivotModel::
hmax(int c) const
{
  assert(c >= 0 && c <= int(hdata_.size()));

  return hdata_[c].max;
}

double
CQPivotModel::
vmin(int r) const
{
  assert(r >= 0 && r <= int(vdata_.size()));

  return vdata_[r].min;
}

double
CQPivotModel::
vmax(int r) const
{
  assert(r >= 0 && r <= int(vdata_.size()));

  return vdata_[r].max;
}
