#include <CQFireworksModel.h>
#include <CFireworks.h>

#include <QTimer>
#include <QColor>

CQFireworksModel::
CQFireworksModel()
{
  setObjectName("fireworksModel");

  fireworks_ = new CFireworks;

  timer_ = new QTimer(this);

  connect(timer_, SIGNAL(timeout()), this, SLOT(timerSlot()));

  init();
}

CQFireworksModel::
~CQFireworksModel()
{
}

void
CQFireworksModel::
init()
{
  if (! initialized_) {
    fireworks_->updateCurrentParticles();

    timer_->start(100);

    initialized_ = true;
  }
}

void
CQFireworksModel::
timerSlot()
{
  beginResetModel();

  fireworks_->step();

  fireworks_->updateParticles();

  fireworks_->updateCurrentParticles();

  endResetModel();
}

// get number of columns
int
CQFireworksModel::
columnCount(const QModelIndex &) const
{
  return 3;
}

// get number of child rows for parent
int
CQFireworksModel::
rowCount(const QModelIndex &) const
{
  return fireworks_->currentParticles().size();
}

// get child node for row/column of parent
QModelIndex
CQFireworksModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (row < 0 || row >= rowCount(parent))
    return QModelIndex();

  return createIndex(row, column);
}

// get parent for child
QModelIndex
CQFireworksModel::
parent(const QModelIndex &) const
{
  // flat - no parent
  return QModelIndex();
}

bool
CQFireworksModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  // flat - no children
  return false;
}

QVariant
CQFireworksModel::
data(const QModelIndex &index, int role) const
{
  if (role == Qt::DisplayRole) {
    const auto &particles = fireworks_->currentParticles();

    auto *particle = particles[index.row()];

    auto p = particle->getPosition();

    if      (index.column() == 0)
      return QVariant(p.x());
    else if (index.column() == 1)
      return QVariant(p.y());
    else if (index.column() == 2) {
      auto c = particle->getColor();

      return QColor(c.getRed()*255, c.getGreen()*255, c.getBlue()*255);
    }
  }

  return QVariant();
}

QVariant
CQFireworksModel::
headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  static const char *names[] = { "X", "Y", "Color" };

  if (section < 0 || section >= 3)
    return QVariant();

  if (role == Qt::DisplayRole)
    return names[section];

  return QVariant();
}

Qt::ItemFlags
CQFireworksModel::
flags(const QModelIndex & /*index*/) const
{
  return Qt::ItemFlags();
}
