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

  bucketPos_ = numColumns;
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

  if (c < 0)
    return QVariant();

  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return QVariant();

  if (c < bucketPos()) {
    QModelIndex ind = model->index(r, c);

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

  if (c < 0)
    return false;

  if (c < bucketPos()) {
    QAbstractItemModel *model = this->sourceModel();

    if (! model)
      return false;

    QModelIndex ind = model->index(r, c);

    return model->setData(ind, value, role);
  }

  return false;
}

QVariant
CQBucketModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical) {
    QAbstractItemModel *model = this->sourceModel();

    if (! model)
      return QVariant();

    return model->headerData(section, orientation, role);
  }

  if (section < 0 || section >= columnCount())
    return QVariant();

  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return QVariant();

  if (section < bucketPos())
    return model->headerData(section, orientation, role);

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
  if (orientation == Qt::Vertical) {
    QAbstractItemModel *model = this->sourceModel();

    if (! model)
      return false;

    return model->setHeaderData(section, orientation, role);
  }

  if (section < 0 || section >= columnCount())
    return false;

  if (section < bucketPos()) {
    QAbstractItemModel *model = this->sourceModel();

    if (! model)
      return false;

    return model->setHeaderData(section, orientation, value, role);
  }

  return false;
}

Qt::ItemFlags
CQBucketModel::
flags(const QModelIndex &index) const
{
  int r = index.row   ();
  int c = index.column();

  if (c < 0)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (c < bucketPos()) {
    QAbstractItemModel *model = this->sourceModel();

    if (! model)
      return 0;

    QModelIndex ind = model->index(r, c);

    return model->flags(ind);
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// map index in source model to folded model
QModelIndex
CQBucketModel::
mapFromSource(const QModelIndex &sourceIndex) const
{
  if (! sourceIndex.isValid())
    return QModelIndex();

  int r = sourceIndex.row   ();
  int c = sourceIndex.column();

  if (c >= 0 && c < bucketPos())
    return this->index(r, c);

  return QModelIndex();
}

// map index in folded model to source model
QModelIndex
CQBucketModel::
mapToSource(const QModelIndex &proxyIndex) const
{
  if (! proxyIndex.isValid())
    return QModelIndex();

  int r = proxyIndex.row   ();
  int c = proxyIndex.column();

  if (c >= 0 && c < bucketPos()) {
    QAbstractItemModel *model = this->sourceModel();

    if (! model)
      return QModelIndex();

    return model->index(r, c);
  }

  return QModelIndex();
}
