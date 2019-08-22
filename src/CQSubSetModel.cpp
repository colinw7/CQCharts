#include <CQSubSetModel.h>
#include <assert.h>

//------

CQSubSetModel::
CQSubSetModel(QAbstractItemModel *model, const QModelIndex &tl, const QModelIndex &br)
{
  setObjectName("subsetModel");

  setSortRole(Qt::EditRole);

  setSourceModel(model);

  setBounds(tl, br);
}

CQSubSetModel::
~CQSubSetModel()
{
}

QAbstractItemModel *
CQSubSetModel::
sourceModel() const
{
  return QAbstractProxyModel::sourceModel();
}

QSortFilterProxyModel *
CQSubSetModel::
sourceFilterModel() const
{
  return qobject_cast<QSortFilterProxyModel *>(this->sourceModel());
}

void
CQSubSetModel::
setSourceModel(QAbstractItemModel *sourceModel)
{
  QSortFilterProxyModel::setSourceModel(sourceModel);
}

void
CQSubSetModel::
setBounds(const QModelIndex &tl, const QModelIndex &br)
{
  beginResetModel();

  if (tl.isValid()) {
    assert(tl.model() == sourceModel());

    tl_ = tl;
  }
  else
    tl_ = QModelIndex();

  if (br.isValid()) {
    assert(br.model() == sourceModel());

    br_ = br;
  }
  else
    br_ = QModelIndex();

  endResetModel();
}

void
CQSubSetModel::
setTopLeft(const QModelIndex &tl)
{
  setBounds(tl, br_);
}

void
CQSubSetModel::
setBottomRight(const QModelIndex &br)
{
  setBounds(tl_, br);
}

bool
CQSubSetModel::
filterAcceptsColumn(int column, const QModelIndex &parent) const
{
  if (! tl_.isValid() || ! br_.isValid())
    return true;

  if (parent.isValid())
    return true;

  int c1 = tl_.column();
  int c2 = br_.column();

  if (c1 > c2) std::swap(c1, c2);

  if (column < c1 || column > c2)
    return false;

  return true;
}

bool
CQSubSetModel::
filterAcceptsRow(int row, const QModelIndex &parent) const
{
  if (! tl_.isValid() || ! br_.isValid())
    return true;

  if (parent.isValid())
    return true;

  int r1 = tl_.row();
  int r2 = br_.row();

  if (r1 > r2) std::swap(r1, r2);

  if (row < r1 || row > r2)
    return false;

  return true;
}
