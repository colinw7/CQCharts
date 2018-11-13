#include <CQBucketModel.h>
#include <cassert>
#include <iostream>

//------

CQBucketModel::
CQBucketModel(QAbstractItemModel *model, const CQBucketer &bucketer) :
 bucketer_(bucketer)
{
  setSourceModel(model);
}

CQBucketModel::
~CQBucketModel()
{
}

void
CQBucketModel::
setBucketer(const CQBucketer &bucketer)
{
  bucketer_ = bucketer;

  doResetModel();
}

QAbstractItemModel *
CQBucketModel::
sourceModel() const
{
  QAbstractItemModel *sourceModel = QAbstractProxyModel::sourceModel();

  return sourceModel;
}

void
CQBucketModel::
setSourceModel(QAbstractItemModel *sourceModel)
{
  QAbstractProxyModel::setSourceModel(sourceModel);

  connectSlots();

  bucket();
}

void
CQBucketModel::
setBucketColumn(int i)
{
  if (i != bucketColumn_) {
    bucketColumn_ = i;

    doResetModel();
  }
}

void
CQBucketModel::
doResetModel()
{
  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return;

  beginResetModel();

  bucket();

  endResetModel();
}

void
CQBucketModel::
connectSlots()
{
  QAbstractItemModel *model = this->sourceModel();

  if (model) {
    connect(model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            this, SLOT(bucket()));
    connect(model, SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SLOT(bucket()));
    connect(model, SIGNAL(columnsRemoved(const QModelIndex &, int, int)),
            this, SLOT(bucket()));

    connect(model, SIGNAL(modelReset()), this, SLOT(bucket()));

    connect(model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this, SLOT(bucket()));
    connect(model, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SLOT(bucket()));
    connect(model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            this, SLOT(bucket()));
  }
}

void
CQBucketModel::
disconnectSlots()
{
  QAbstractItemModel *model = this->sourceModel();

  if (model) {
    disconnect(model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
               this, SLOT(bucket()));
    disconnect(model, SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
               this, SLOT(bucket()));
    disconnect(model, SIGNAL(columnsRemoved(const QModelIndex &, int, int)),
               this, SLOT(bucket()));

    disconnect(model, SIGNAL(modelReset()), this, SLOT(bucket()));

    disconnect(model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
               this, SLOT(bucket()));
    disconnect(model, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
               this, SLOT(bucket()));
    disconnect(model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
               this, SLOT(bucket()));
  }
}

void
CQBucketModel::
bucket()
{
  clear();

  //---

  // check column valid
  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return;

  int numColumns = model->columnCount();

  if (bucketColumn() < 0 || bucketColumn() >= numColumns)
    return;

  if (bucketer_.type() != CQBucketer::Type::STRING) {
    bool   rset = false;
    double rmin = 0.0, rmax = 0.0;

    for (int r = 0; r < model->rowCount(); ++r) {
      QModelIndex ind = model->index(r, bucketColumn());

      QVariant var = model->data(ind, bucketRole());

      bool ok;

      double rval = CQBucketer::varReal(var, ok);
      if (! ok) continue;

      if (! rset) {
        rmin = rval;
        rmax = rval;

        rset = true;
      }
      else {
        rmin = std::min(rmin, rval);
        rmax = std::max(rmax, rval);
      }
    }

    bucketer_.setRMin(rmin);
    bucketer_.setRMax(rmax);
  }

  //---

  bucketPos_ = 0;
//bucketPos_ = numColumns;
  bucketed_  = true;
}

void
CQBucketModel::
clear()
{
  bucketed_ = false;
}

// get number of columns
int
CQBucketModel::
columnCount(const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return 0;

  return model->columnCount(parent) + 1;
}

// get number of child rows for parent
int
CQBucketModel::
rowCount(const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return 0;

  return model->rowCount(parent);
}

// get child node for row/column of parent
QModelIndex
CQBucketModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid())
    return QModelIndex();

  return createIndex(row, column, nullptr);
}

// get parent for child
QModelIndex
CQBucketModel::
parent(const QModelIndex &) const
{
  return QModelIndex();
}

bool
CQBucketModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  return false;
}

QVariant
CQBucketModel::
data(const QModelIndex &index, int role) const
{
  int r = index.row   ();
  int c = index.column();

  if (c < 0 || c >= columnCount())
    return QVariant();

  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return QVariant();

  int c1 = mapColumn(c);

  if (c1 >= 0) {
    QModelIndex ind = model->index(r, c1);

    return model->data(ind, role);
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
    QModelIndex ind = model->index(r, bucketColumn());

    QVariant var = model->data(ind, bucketRole());

    int bucket = bucketer_.bucket(var);

    if      (role == Qt::DisplayRole) {
      return bucketer_.bucketName(bucket);
    }
    if      (role == Qt::DisplayRole) {
      return bucket;
    }
    else if (role == Qt::ToolTipRole) {
      return QString("%1 (%2)").arg(bucketer_.bucketName(bucket)).arg(bucket);
    }
  }

  return QVariant();
}

bool
CQBucketModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  int r = index.row   ();
  int c = index.column();

  if (c < 0 || c >= columnCount())
    return false;

  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return false;

  int c1 = mapColumn(c);

  if (c1 >= 0) {
    QModelIndex ind = model->index(r, c);

    return model->setData(ind, value, role);
  }

  return false;
}

QVariant
CQBucketModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return QVariant();

  if (orientation == Qt::Vertical) {
    return model->headerData(section, orientation, role);
  }

  if (section < 0 || section >= columnCount())
    return QVariant();

  int section1 = mapColumn(section);

  if (section1 >= 0) {
    return model->headerData(section1, orientation, role);
  }

  if (role == Qt::DisplayRole) {
    QString name = model->headerData(bucketColumn(), orientation, role).toString();

    return QString("bucket(%1)").arg(name);
  }

  return model->headerData(bucketColumn(), orientation, role);
}

bool
CQBucketModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return false;

  if (orientation == Qt::Vertical) {
    return model->setHeaderData(section, orientation, role);
  }

  if (section < 0 || section >= columnCount())
    return false;

  int section1 = mapColumn(section);

  if (section1 >= 0) {
    return model->setHeaderData(section1, orientation, value, role);
  }

  return false;
}

Qt::ItemFlags
CQBucketModel::
flags(const QModelIndex &index) const
{
  int r = index.row   ();
  int c = index.column();

  if (c < 0 || c >= columnCount())
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return 0;

  int c1 = mapColumn(c);

  if (c1 >= 0) {
    QModelIndex ind = model->index(r, c1);

    return model->flags(ind);
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// map index in source model to bucket model
QModelIndex
CQBucketModel::
mapFromSource(const QModelIndex &sourceIndex) const
{
  if (! sourceIndex.isValid())
    return QModelIndex();

  int r = sourceIndex.row   ();
  int c = sourceIndex.column();

  if (c < 0 || c >= columnCount())
    return QModelIndex();

  int c1 = unmapColumn(c);

  return this->index(r, c1);
}

// map index in bucket model to source model
QModelIndex
CQBucketModel::
mapToSource(const QModelIndex &proxyIndex) const
{
  if (! proxyIndex.isValid())
    return QModelIndex();

  int r = proxyIndex.row   ();
  int c = proxyIndex.column();

  if (c < 0 || c >= columnCount())
    return QModelIndex();

  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return QModelIndex();

  int c1 = mapColumn(c);

  if (c1 >= 0)
    return model->index(r, c1);

  return QModelIndex();
}

// map bucket column to source column
int
CQBucketModel::
mapColumn(int c) const
{
  if (c < bucketPos())
    return c;

  if (c > bucketPos())
    return c - 1;

  return -1;
}

// map source column to bucket column
int
CQBucketModel::
unmapColumn(int c) const
{
  if (c < bucketPos())
    return c;

  if (c >= bucketPos())
    return c + 1;

  return -1;
}
