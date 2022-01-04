#include <CQDragon3DModel.h>
#include <CDragon3DCalc.h>

CQDragon3DModel::
CQDragon3DModel(int start, int end)
{
  setObjectName("lorenzModel");

  calc_ = new CDragon3DCalc(start, end);
}

CQDragon3DModel::
~CQDragon3DModel()
{
}

void
CQDragon3DModel::
init()
{
  if (! initialized_) {
    double x, y, z, g;

    while (calc_->nextValue(x, y, z, g)) {
      auto p = Point(x, y, z);

      int g1 = int(g*255);

      p.c = QColor(g1, g1, g1);

      points_.push_back(p);
    }

    initialized_ = true;
  }
}

// get number of columns
int
CQDragon3DModel::
columnCount(const QModelIndex &) const
{
  return 4;
}

// get number of child rows for parent
int
CQDragon3DModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return calc_->numValues();
}

// get child node for row/column of parent
QModelIndex
CQDragon3DModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (row < 0 || row >= rowCount(parent))
    return QModelIndex();

  return createIndex(row, column);
}

// get parent for child
QModelIndex
CQDragon3DModel::
parent(const QModelIndex &) const
{
  // flat - no parent
  return QModelIndex();
}

bool
CQDragon3DModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  // flat - no children
  return false;
}

QVariant
CQDragon3DModel::
data(const QModelIndex &index, int role) const
{
  const_cast<CQDragon3DModel *>(this)->init();

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
    else if (index.column() == 3)
      return p.c;
  }

  return QVariant();
}

QVariant
CQDragon3DModel::
headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  static const char *names[] = { "X", "Y", "Z", "G" };

  if (section < 0 || section >= 4)
    return QVariant();

  if (role == Qt::DisplayRole)
    return names[section];

  return QVariant();
}

Qt::ItemFlags
CQDragon3DModel::
flags(const QModelIndex & /*index*/) const
{
  return Qt::ItemFlags();
}
