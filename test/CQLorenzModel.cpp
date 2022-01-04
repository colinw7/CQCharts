#include <CQLorenzModel.h>
#include <CLorenzCalc.h>

CQLorenzModel::
CQLorenzModel(int start, int end)
{
  setObjectName("lorenzModel");

  calc_ = new CLorenzCalc(start, end);
}

CQLorenzModel::
~CQLorenzModel()
{
}

double
CQLorenzModel::
getA() const
{
  return CLorenzCalc::A;
}

void
CQLorenzModel::
setA(double r)
{
  CLorenzCalc::A = r;

  initialized_ = false;

  init();
}

double
CQLorenzModel::
getB() const
{
  return CLorenzCalc::B;
}

void
CQLorenzModel::
setB(double r)
{
  CLorenzCalc::B = r;

  initialized_ = false;

  init();
}

double
CQLorenzModel::
getC() const
{
  return CLorenzCalc::C;
}

void
CQLorenzModel::
setC(double r)
{
  CLorenzCalc::C = r;

  initialized_ = false;

  init();
}

void
CQLorenzModel::
init()
{
  if (! initialized_) {
    beginResetModel();

    points_.clear();

    for (int i = 0; i < calc_->getIterationEnd(); i++) {
      double x, y, z;

      calc_->nextValue(x, y, z);

      if (i >= calc_->getIterationStart())
        points_.emplace_back(x, y, z);
    }

    endResetModel();

    initialized_ = true;
  }
}

// get number of columns
int
CQLorenzModel::
columnCount(const QModelIndex &) const
{
  return 3;
}

// get number of child rows for parent
int
CQLorenzModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return calc_->getIterationEnd() - calc_->getIterationStart() + 1;
}

// get child node for row/column of parent
QModelIndex
CQLorenzModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (row < 0 || row >= rowCount(parent))
    return QModelIndex();

  return createIndex(row, column);
}

// get parent for child
QModelIndex
CQLorenzModel::
parent(const QModelIndex &) const
{
  // flat - no parent
  return QModelIndex();
}

bool
CQLorenzModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  // flat - no children
  return false;
}

QVariant
CQLorenzModel::
data(const QModelIndex &index, int role) const
{
  const_cast<CQLorenzModel *>(this)->init();

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
CQLorenzModel::
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
CQLorenzModel::
flags(const QModelIndex & /*index*/) const
{
  return Qt::ItemFlags();
}
