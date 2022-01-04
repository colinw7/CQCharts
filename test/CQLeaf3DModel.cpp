#include <CQLeaf3DModel.h>
#include <CLeaf3DCalc.h>

CQLeaf3DModel::
CQLeaf3DModel(int start, int end)
{
  setObjectName("leaf3DModel");

  calc_ = new CLeaf3DCalc(start, end);
}

CQLeaf3DModel::
~CQLeaf3DModel()
{
}

void
CQLeaf3DModel::
init()
{
  if (! initialized_) {
    for (int i = 0; i < calc_->getIterationEnd(); i++) {
      double x, y, z;

      calc_->nextValue(x, y, z);

      if (i >= calc_->getIterationStart())
        points_.emplace_back(x, y, z);
    }

    initialized_ = true;
  }
}

// get number of columns
int
CQLeaf3DModel::
columnCount(const QModelIndex &) const
{
  return 3;
}

// get number of child rows for parent
int
CQLeaf3DModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return calc_->getIterationEnd() - calc_->getIterationStart() + 1;
}

// get child node for row/column of parent
QModelIndex
CQLeaf3DModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (row < 0 || row >= rowCount(parent))
    return QModelIndex();

  return createIndex(row, column);
}

// get parent for child
QModelIndex
CQLeaf3DModel::
parent(const QModelIndex &) const
{
  // flat - no parent
  return QModelIndex();
}

bool
CQLeaf3DModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  // flat - no children
  return false;
}

QVariant
CQLeaf3DModel::
data(const QModelIndex &index, int role) const
{
  const_cast<CQLeaf3DModel *>(this)->init();

  if (role == Qt::DisplayRole) {
    if (index.row() < 0 || index.row() >= int(points_.size()))
      return QVariant();

    const auto &p = points_[index.row()];

    if      (index.column() == 0)
      return p.x;
    else if (index.column() == 1)
      return p.y;
    else if (index.column() == 2)
      return p.z;
  }

  return QVariant();
}

QVariant
CQLeaf3DModel::
headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  static const char *names[] = { "X", "Y", "Z" };

  if (section < 0 || section >= 3)
    return QVariant();

  if (role == Qt::DisplayRole)
    return names[section];

  return QVariant();
}

Qt::ItemFlags
CQLeaf3DModel::
flags(const QModelIndex & /*index*/) const
{
  return Qt::ItemFlags();
}
