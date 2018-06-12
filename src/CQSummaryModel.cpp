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
  beginResetModel();

  maxRows_ = std::max(maxRows, 1);

  endResetModel();
}

void
CQSummaryModel::
setSorted(bool b)
{
  beginResetModel();

  sorted_ = b;

  endResetModel();
}

void
CQSummaryModel::
setRandom(bool b)
{
  if (b != random_) {
    beginResetModel();

    random_ = b;

    initRandom();

    endResetModel();
  }
}

void
CQSummaryModel::
initRandom()
{
  rowInds_.clear();

  if (! random_)
    return;

  //---

  QAbstractItemModel *model = this->sourceModel();

  int nr = model->rowCount();

  if (maxRows_ >= nr)
    return;

  //---

  using RowSet = std::set<int>;

  RandInRange rand(0, nr - 1);

  RowSet rowSet;

  int numRandom = 0;

  while (numRandom < maxRows_) {
    int r = rand.gen();

    auto p = rowSet.find(r);

    if (p == rowSet.end()) {
      rowSet.insert(r);

      ++numRandom;
    }
  }

  //---

  rowInds_.resize(maxRows_);

  int i = 0;

  for (const auto &r : rowSet)
    rowInds_[i++] = r;
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

  return std::min(nr, maxRows());
}

// get child node for row/column of parent
QModelIndex
CQSummaryModel::
index(int row, int column, const QModelIndex &) const
{
  if (row < 0 || row >= maxRows())
    return QModelIndex();

  return createIndex(row, column);
}

// get parent for child
QModelIndex
CQSummaryModel::
parent(const QModelIndex &) const
{
  return QModelIndex();
}

bool
CQSummaryModel::
hasChildren(const QModelIndex &) const
{
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
  assert(false);

  if (! sourceIndex.isValid())
    return QModelIndex();

  int r = sourceIndex.row   ();
  int c = sourceIndex.column();

  if (r < 0 || r >= maxRows())
    return QModelIndex();

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

  if (r < 0 || r >= maxRows())
    return QModelIndex();

  if (random_ && int(rowInds_.size()) == maxRows()) {
    r = rowInds_[r];
  }

  QAbstractItemModel *model = this->sourceModel();

  return model->index(r, c);
}
