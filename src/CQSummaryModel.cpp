#include <CQSummaryModel.h>
#include <random>
#include <set>
#include <cassert>

namespace {

class RandInRange {
 public:
  RandInRange(int min, int max) :
   eng_(rd_()), idis_(min, max) {
  }

  int gen() {
    return idis_(eng_);
  }

 private:
  std::random_device                 rd_;
  std::default_random_engine         eng_;
  std::uniform_int_distribution<int> idis_;
};

inline bool variantReal(const QVariant &var, double &r) {
  if (var.type() == QVariant::Int   ) { r = var.toInt   (); return true; }
  if (var.type() == QVariant::Double) { r = var.toDouble(); return true; }
  return false;
}

int variantCmp(const QVariant &lhs, const QVariant &rhs) {
  double r1, r2;

  bool lnumeric = variantReal(lhs, r1);
  bool rnumeric = variantReal(rhs, r2);

  if (lnumeric && rnumeric) {
    if (r1 < r2) return -1;
    if (r1 > r2) return  1;
    return 0;
  }
  else {
    return lhs.toString().compare(rhs.toString());
  };
}

}

//------

CQSummaryModel::
CQSummaryModel(QAbstractItemModel *model, int maxRows) :
 maxRows_(maxRows)
{
  setSourceModel(model);
}

CQSummaryModel::
~CQSummaryModel()
{
}

QAbstractItemModel *
CQSummaryModel::
sourceModel() const
{
  return QAbstractProxyModel::sourceModel();
}

void
CQSummaryModel::
setSourceModel(QAbstractItemModel *sourceModel)
{
  QAbstractProxyModel::setSourceModel(sourceModel);
}

void
CQSummaryModel::
setMaxRows(int maxRows)
{
  if (maxRows != maxRows_) {
    maxRows_ = std::max(maxRows, 1);

    if (mode() != Mode::NORMAL && mode() != Mode::PAGED)
      resetMapping();
  }
}

void
CQSummaryModel::
setMode(const Mode &m)
{
  if (m != mode()) {
    mode_ = m;

    resetMapping();
  }
}

void
CQSummaryModel::
setSortColumn(int c)
{
  if (c != sortColumn_) {
    sortColumn_ = c;

    if (mode() == Mode::SORTED)
      resetMapping();
  }
}

void
CQSummaryModel::
setSortRole(int r)
{
  if (r != sortRole_) {
    sortRole_ = r;

    if (mode() == Mode::SORTED)
      resetMapping();
  }
}

void
CQSummaryModel::
setPageSize(int i)
{
  if (i != pageSize_) {
    pageSize_ = i;

    if (mode() == Mode::PAGED)
      resetMapping();
  }
}

void
CQSummaryModel::
setCurrentPage(int i)
{
  if (i != currentPage_) {
    currentPage_ = i;

    if (mode() == Mode::PAGED)
      resetMapping();
  }
}

void
CQSummaryModel::
resetMapping()
{
  beginResetModel();

  initMapping();

  endResetModel();
}

void
CQSummaryModel::
initMapping()
{
  mapValid_ = false;
  mapNone_  = false;

  rowInds_.clear();
  indRows_.clear();

  if (mode() == Mode::NORMAL || mode() == Mode::PAGED)
    return;

  //---

  QAbstractItemModel *model = this->sourceModel();

  int nr = model->rowCount();

  //---

  if      (mode() == Mode::RANDOM) {
    // if summary count greater or equal to actual count then nothing to do
    if (maxRows() >= nr) {
      mapValid_ = true;
      mapNone_  = true;
      return;
    }

    // create set of random rows (sorted)
    int nr1 = nr - maxRows();

    bool invert = (nr1 < nr/2);

    RowSet rowSet;

    if (! invert)
      randRows(rowSet, maxRows(), nr);
    else
      randRows(rowSet, nr1, nr);

    //---

    // create mapping
    rowInds_.resize(maxRows());

    if (! invert) {
      int i = 0;

      for (const auto &r : rowSet) {
        rowInds_[i] = r;
        indRows_[r] = i;

        ++i;
      }
    }
    else {
      int i = 0;

      for (int r = 0; r < nr; ++r) {
        auto p = rowSet.find(r);

        if (p != rowSet.end())
          continue;

        rowInds_[i] = r;
        indRows_[r] = i;

        ++i;
      }
    }

    assert(int(indRows_.size()) == maxRows());

    mapValid_ = true;
  }
  else if (mode() == Mode::SORTED) {
    int nc = model->columnCount();

    if (sortColumn_ < 0 || sortColumn_ >= nc) {
      mapValid_ = true;
      mapNone_  = true;
      return;
    }

    // get array of values and row numbers
    using ValueRow  = std::pair<QVariant,int>;
    using RowValues = std::vector<ValueRow>;

    RowValues rowValues;

    rowValues.resize(nr);

    for (int r = 0; r < nr; ++r) {
      QModelIndex ind = model->index(r, sortColumn_, QModelIndex());

      QVariant value = model->data(ind, sortRole_);

      rowValues[r] = ValueRow(value, r);
    }

    // sort summary size
    std::partial_sort(rowValues.begin(), rowValues.begin() + maxRows(), rowValues.end(),
                      [](const ValueRow &lhs, const ValueRow &rhs) {
                        return variantCmp(lhs.first, rhs.first) < 0;
                      });

    //---

    // create mapping
    rowInds_.resize(maxRows());

    int i = 0;

    for (const auto &rv : rowValues) {
      int r = rv.second;

      rowInds_[i] = r;
      indRows_[r] = i;

      ++i;

      if (i >= maxRows())
        break;
    }

    mapValid_ = true;
  }
  else {
    assert(false);
  }
}

void
CQSummaryModel::
randRows(RowSet &rowSet, int n, int nr) const
{
  // create set of random rows (sorted)
  RandInRange rand(0, nr - 1);

  int numRandom = 0;

  int iters = 0;

  while (numRandom < n) {
    int r = rand.gen();

    auto p = rowSet.find(r);

    if (p == rowSet.end()) {
      rowSet.insert(r);

      ++numRandom;
    }

    ++iters;
  }
}

//------

// get number of columns
int
CQSummaryModel::
columnCount(const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->sourceModel();

  return model->columnCount(parent);
}

// get number of child rows for parent
int
CQSummaryModel::
rowCount(const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->sourceModel();

  int nr = model->rowCount(parent);

  if (mode() == Mode::PAGED)
    return std::min(nr, pageSize());
  else
    return std::min(nr, maxRows());
}

// get child node for row/column of parent
QModelIndex
CQSummaryModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (row < 0 || row >= rowCount(parent))
    return QModelIndex();

  return createIndex(row, column);
}

// get parent for child
QModelIndex
CQSummaryModel::
parent(const QModelIndex &) const
{
  // flat - no parent
  return QModelIndex();
}

bool
CQSummaryModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  // flat - no children
  return false;
}

QVariant
CQSummaryModel::
data(const QModelIndex &index, int role) const
{
  QAbstractItemModel *model = this->sourceModel();

  return model->data(mapToSource(index), role);
}

bool
CQSummaryModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  QAbstractItemModel *model = this->sourceModel();

  return model->setData(mapToSource(index), value, role);
}

QVariant
CQSummaryModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  QAbstractItemModel *model = this->sourceModel();

  return model->headerData(section, orientation, role);
}

bool
CQSummaryModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  QAbstractItemModel *model = this->sourceModel();

  return model->setHeaderData(section, orientation, value, role);
}

Qt::ItemFlags
CQSummaryModel::
flags(const QModelIndex &index) const
{
  QAbstractItemModel *model = this->sourceModel();

  return model->flags(mapToSource(index));
}

// map index in source model to summary model
QModelIndex
CQSummaryModel::
mapFromSource(const QModelIndex &sourceIndex) const
{
  if (! sourceIndex.isValid())
    return QModelIndex();

  int r = sourceIndex.row   ();
  int c = sourceIndex.column();

  QAbstractItemModel *model = this->sourceModel();

  if      (mode() == Mode::NORMAL) {
    if (r < 0 || r >= model->rowCount())
      return QModelIndex();
  }
  else if (mode() == Mode::PAGED) {
    int r1 = currentPage()*pageSize();
    int r2 = r1 + pageSize();

    if (r < r1 || r >= r2)
      return QModelIndex();

    r = r - r1;
  }
  else {
    assert(mapValid_);

    if (! mapNone_) {
      auto p = indRows_.find(r);

      if (p == indRows_.end())
        return QModelIndex();

      r = (*p).second;
    }
  }

  return this->index(r, c);
}

// map index in summary model to source model
QModelIndex
CQSummaryModel::
mapToSource(const QModelIndex &proxyIndex) const
{
  if (! proxyIndex.isValid())
    return QModelIndex();

  int r = proxyIndex.row   ();
  int c = proxyIndex.column();

  if (r < 0 || r >= rowCount())
    return QModelIndex();

  if      (mode() == Mode::NORMAL) {
  }
  else if (mode() == Mode::PAGED) {
    int r1 = currentPage()*pageSize();

    r = r1 + r;
  }
  else {
    assert(mapValid_);

    if (! mapNone_)
      r = rowInds_[r];
  }

  QAbstractItemModel *model = this->sourceModel();

  return model->index(r, c);
}
