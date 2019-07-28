#include <CQTransposeModel.h>
#include <cassert>
#include <iostream>

//------

CQTransposeModel::
CQTransposeModel(QAbstractItemModel *model)
{
  setObjectName("transposeModel");

  setSourceModel(model);
}

CQTransposeModel::
~CQTransposeModel()
{
}

QAbstractItemModel *
CQTransposeModel::
sourceModel() const
{
  return QAbstractProxyModel::sourceModel();
}

void
CQTransposeModel::
setSourceModel(QAbstractItemModel *sourceModel)
{
  QAbstractProxyModel::setSourceModel(sourceModel);

  connectSlots();
}

void
CQTransposeModel::
connectSlots()
{
  connectDisconnectSlots(true);
}

void
CQTransposeModel::
disconnectSlots()
{
  connectDisconnectSlots(false);
}

void
CQTransposeModel::
connectDisconnectSlots(bool b)
{
  QAbstractItemModel *model = this->sourceModel();
  if (! model) return;

  auto connectDisconnect = [&](bool b, const char *from, const char *to) {
    if (b)
      connect(model, from, this, to);
    else
      disconnect(model, from, this, to);
  };

  connectDisconnect(b,
    SIGNAL(columnsAboutToBeInserted(const QModelIndex&, int, int)),
    SLOT(columnsAboutToBeInsertedSlot(const QModelIndex&, int, int)));
  connectDisconnect(b,
    SIGNAL(columnsInserted(const QModelIndex&, int, int)),
    SLOT(columnsInsertedSlot(const QModelIndex&, int, int)));
  connectDisconnect(b,
    SIGNAL(columnsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
    SLOT(columnsAboutToBeMovedSlot(const QModelIndex&, int, int, const QModelIndex&, int)));
  connectDisconnect(b,
    SIGNAL(columnsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
    SLOT(columnsMovedSlot(const QModelIndex&, int, int, const QModelIndex&, int)));
  connectDisconnect(b,
    SIGNAL(columnsAboutToBeRemoved(const QModelIndex&, int, int)),
    SLOT(columnsAboutToBeRemovedSlot(const QModelIndex&, int, int)));
  connectDisconnect(b,
    SIGNAL(columnsRemoved(const QModelIndex&, int, int)),
    SLOT(columnsRemovedSlot(const QModelIndex&, int, int)));

  connectDisconnect(b,
    SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
    SLOT(rowsAboutToBeInsertedSlot(const QModelIndex&, int, int)));
  connectDisconnect(b,
    SIGNAL(rowsInserted(const QModelIndex&, int, int)),
    SLOT(rowsInsertedSlot(const QModelIndex&, int, int)));
  connectDisconnect(b,
    SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
    SLOT(rowsAboutToBeMovedSlot(const QModelIndex&, int, int, const QModelIndex&, int)));
  connectDisconnect(b,
    SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
    SLOT(rowsMovedSlot(const QModelIndex&, int, int, const QModelIndex&, int)));
  connectDisconnect(b,
    SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
    SLOT(rowsAboutToBeRemovedSlot(const QModelIndex&, int, int)));
  connectDisconnect(b,
    SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
    SLOT(rowsRemovedSlot(const QModelIndex&, int, int)));

  connectDisconnect(b,
    SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
    SLOT(dataChangedSlot(const QModelIndex&, const QModelIndex&)));
  connectDisconnect(b,
    SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
    SLOT(headerDataChangedSlot(Qt::Orientation, int, int)));

  connectDisconnect(b,
    SIGNAL(modelAboutToBeReset()), SLOT(modelAboutToBeResetSlot()));
  connectDisconnect(b,
    SIGNAL(modelReset()), SLOT(modelResetSlot()));
}

// get number of columns
int
CQTransposeModel::
columnCount(const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->sourceModel();

  return model->rowCount(mapToSource(parent));
}

// get number of child rows for parent
int
CQTransposeModel::
rowCount(const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->sourceModel();

  return model->columnCount(mapToSource(parent));
}

// get child node for row/column of parent
QModelIndex
CQTransposeModel::
index(int row, int column, const QModelIndex &) const
{
  return createIndex(row, column);
}

// get parent for child
QModelIndex
CQTransposeModel::
parent(const QModelIndex &) const
{
  return QModelIndex();
}

bool
CQTransposeModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  return false;
}

QVariant
CQTransposeModel::
data(const QModelIndex &index, int role) const
{
  QAbstractItemModel *model = this->sourceModel();

  return model->data(mapToSource(index), role);
}

bool
CQTransposeModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  QAbstractItemModel *model = this->sourceModel();

  return model->setData(mapToSource(index), value, role);
}

QVariant
CQTransposeModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  QAbstractItemModel *model = this->sourceModel();

  Qt::Orientation orientation1 = (orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

  return model->headerData(section, orientation1, role);
}

bool
CQTransposeModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  QAbstractItemModel *model = this->sourceModel();

  Qt::Orientation orientation1 = (orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

  return model->setHeaderData(section, orientation1, value, role);
}

Qt::ItemFlags
CQTransposeModel::
flags(const QModelIndex &index) const
{
  QAbstractItemModel *model = this->sourceModel();

  return model->flags(mapToSource(index));
}

// map index in source model to folded model
QModelIndex
CQTransposeModel::
mapFromSource(const QModelIndex &sourceIndex) const
{
  if (! sourceIndex.isValid())
    return QModelIndex();

  int r = sourceIndex.row   ();
  int c = sourceIndex.column();

  return this->index(c, r);
}

// map index in folded model to source model
QModelIndex
CQTransposeModel::
mapToSource(const QModelIndex &proxyIndex) const
{
  if (! proxyIndex.isValid())
    return QModelIndex();

  int r = proxyIndex.row   ();
  int c = proxyIndex.column();

  QAbstractItemModel *model = this->sourceModel();

  return model->index(c, r);
}

//------

void
CQTransposeModel::
columnsAboutToBeInsertedSlot(const QModelIndex &parent, int start, int end)
{
  beginInsertRows(mapFromSource(parent), start, end);
}

void
CQTransposeModel::
columnsInsertedSlot(const QModelIndex &, int, int)
{
  endInsertRows();
}

void
CQTransposeModel::
columnsAboutToBeMovedSlot(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                          const QModelIndex &destinationParent, int destinationColumn)
{
  beginMoveRows(mapFromSource(sourceParent), sourceStart, sourceEnd,
                mapFromSource(destinationParent), destinationColumn);
}

void
CQTransposeModel::
columnsMovedSlot(const QModelIndex&, int, int, const QModelIndex&, int)
{
  endMoveRows();
}

void
CQTransposeModel::
columnsAboutToBeRemovedSlot(const QModelIndex &parent, int start, int end)
{
  beginRemoveRows(mapFromSource(parent), start, end);
}

void
CQTransposeModel::
columnsRemovedSlot(const QModelIndex&, int, int)
{
  endRemoveRows();
}

void
CQTransposeModel::
rowsAboutToBeInsertedSlot(const QModelIndex &parent, int start, int end)
{
  beginInsertColumns(mapFromSource(parent), start, end);
}

void
CQTransposeModel::
rowsInsertedSlot(const QModelIndex&, int, int)
{
  endInsertColumns();
}

void
CQTransposeModel::
rowsAboutToBeMovedSlot(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                       const QModelIndex &destinationParent, int destinationColumn)
{
  beginMoveColumns(mapFromSource(sourceParent), sourceStart, sourceEnd,
                   mapFromSource(destinationParent), destinationColumn);
}

void
CQTransposeModel::
rowsMovedSlot(const QModelIndex&, int, int, const QModelIndex&, int)
{
  endMoveColumns();
}

void
CQTransposeModel::
rowsAboutToBeRemovedSlot(const QModelIndex &parent, int start, int end)
{
  beginRemoveColumns(mapFromSource(parent), start, end);
}

void
CQTransposeModel::
rowsRemovedSlot(const QModelIndex&, int, int)
{
  endRemoveColumns();
}

void
CQTransposeModel::
dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
  emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

void
CQTransposeModel::
headerDataChangedSlot(Qt::Orientation orientation, int first, int last)
{
  Qt::Orientation orientation1 = (orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

  emit headerDataChanged(orientation1, first, last);
}

void
CQTransposeModel::
modelAboutToBeResetSlot()
{
  beginResetModel();
}

void
CQTransposeModel::
modelResetSlot()
{
  endResetModel();
}
