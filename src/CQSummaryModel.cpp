#include <CQSummaryModel.h>
#include <cassert>

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
  beginResetModel();

  random_ = b;

  endResetModel();
}

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

  QAbstractItemModel *model = this->sourceModel();

  return model->index(r, c);
}
