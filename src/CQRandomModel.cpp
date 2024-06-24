#include <CQRandomModel.h>
#include <CMathRand.h>

#include <QTimer>

CQRandomModel::
CQRandomModel()
{
  setObjectName("randomModel");

  timer_ = new QTimer(this);

  connect(timer_, SIGNAL(timeout()), this, SLOT(timerSlot()));

  resize(1, 1);

  init();
}

CQRandomModel::
~CQRandomModel()
{
}

void
CQRandomModel::
init()
{
  if (! initialized_) {
    timer_->start(250);

    initialized_ = true;
  }
}

void
CQRandomModel::
timerSlot()
{
  beginResetModel();

  for (auto &column : columns_) {
    column.doStep();
  }

  endResetModel();
}

//---

void
CQRandomModel::
resize(int numRows, int numColumns)
{
  columns_.resize(numColumns);

  for (auto &column : columns_)
    column.cells.resize(numRows);

  for (auto &column : columns_) {
    for (auto &cell : column.cells)
      cell.initNewValue(column.min, column.randValue());
  }
}

//---

void
CQRandomModel::
setColumnName(int c, const QString &name)
{
  assert(c >= 0 && c < int(columns_.size()));

  columns_[c].name = name;
}

void
CQRandomModel::
setColumnRange(int c, double min, double max)
{
  assert(c >= 0 && c < int(columns_.size()));

  auto &column = columns_[c];

  column.min = min;
  column.max = max;

  for (auto &cell : column.cells) {
    cell.initNewValue(column.min, column.randValue());
  }
}

void
CQRandomModel::
setColumnSteps(int c, int n)
{
  assert(c >= 0 && c < int(columns_.size()));

  columns_[c].steps = n;
}

//---

// get number of columns
int
CQRandomModel::
columnCount(const QModelIndex &) const
{
  return columns_.size();
}

// get number of child rows for parent
int
CQRandomModel::
rowCount(const QModelIndex &) const
{
  if (columns_.empty())
    return 0;

  return columns_[0].cells.size();
}

// get child node for row/column of parent
QModelIndex
CQRandomModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (row < 0 || row >= rowCount(parent))
    return QModelIndex();

  return createIndex(row, column);
}

// get parent for child
QModelIndex
CQRandomModel::
parent(const QModelIndex &) const
{
  // flat - no parent
  return QModelIndex();
}

bool
CQRandomModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  // flat - no children
  return false;
}

QVariant
CQRandomModel::
data(const QModelIndex &index, int role) const
{
  int c = index.column();

  if (c < 0 || c >= int(columns_.size()))
    return QVariant();

  auto &column = columns_[c];

  int r = index.row();

  if (r < 0 || r >= int(column.cells.size()))
    return QVariant();

  const auto &cell = column.cells[r];

  if (role == Qt::DisplayRole)
    return QVariant(cell.value);

  return QVariant();
}

QVariant
CQRandomModel::
headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  if (section < 0 || section >= int(columns_.size()))
    return QVariant();

  auto &column = columns_[section];

  if (role == Qt::DisplayRole)
    return column.name;

  return QVariant();
}

Qt::ItemFlags
CQRandomModel::
flags(const QModelIndex & /*index*/) const
{
  return Qt::ItemFlags();
}

//---

void
CQRandomModel::ColumnData::
doStep()
{
  ++step;

  if (step >= steps) {
    for (auto &cell : cells)
      cell.genNewValue(randValue());

    step = 0;
  }
  else {
    double d = double(step)/double(steps - 1);

    for (auto &cell : cells)
      cell.updateValue(d);
  }

  if (sum > 0.0) {
    auto sum1 = 0.0;

    for (auto &cell : cells)
      sum1 += std::abs(cell.newValue);

    if (sum1 > 0.0) {
      auto f = sum/sum1;

      for (auto &cell : cells)
        cell.newValue *= f;
    }
  }
}

double
CQRandomModel::ColumnData::
randValue() const
{
  return CMathRand::randInRange(min, max);
}

//---

void
CQRandomModel::CellData::
initNewValue(double min, double r)
{
  oldValue = min;
  newValue = r;
  value    = min;
}

void
CQRandomModel::CellData::
genNewValue(double r)
{
  oldValue = value;
  newValue = r;
}

void
CQRandomModel::CellData::
updateValue(double r)
{
  value = oldValue + r*(newValue - oldValue);
}
