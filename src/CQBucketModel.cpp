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

  bucketValid_ = false;

  bucket();
}

void
CQBucketModel::
setBucketColumn(int i)
{
  if (i != bucketColumn_) {
    bucketColumn_ = i;

    bucketValid_ = false;
  }
}

const CQBucketer::Type &
CQBucketModel::
bucketType() const
{
  return bucketer_.type();
}

void
CQBucketModel::
setBucketType(const CQBucketer::Type &type)
{
  if (type != bucketType()) {
    bucketer_.setType(type);

    bucketValid_ = false;
  }
}

bool
CQBucketModel::
isBucketIntegral() const
{
  return bucketer_.isIntegral();
}

void
CQBucketModel::
setBucketIntegral(bool b)
{
  if (b != isBucketIntegral()) {
    bucketer_.setIntegral(b);

    bucketValid_ = false;
  }
}

double
CQBucketModel::
bucketStart() const
{
  return bucketer_.rstart();
}

void
CQBucketModel::
setBucketStart(double r)
{
  bucketer_.setRStart(r);

  bucketValid_ = false;
}

double
CQBucketModel::
bucketDelta() const
{
  return bucketer_.rdelta();
}

void
CQBucketModel::
setBucketDelta(double r)
{
  bucketer_.setRDelta(r);

  bucketValid_ = false;
}

double
CQBucketModel::
bucketMin() const
{
  return bucketer_.rmin();
}

void
CQBucketModel::
setBucketMin(double r)
{
  bucketer_.setRMin(r);

  bucketValid_ = false;
}

double
CQBucketModel::
bucketMax() const
{
  return bucketer_.rmax();
}

void
CQBucketModel::
setBucketMax(double r)
{
  bucketer_.setRMax(r);

  bucketValid_ = false;
}

int
CQBucketModel::
bucketCount() const
{
  return bucketer_.numAuto();
}

void
CQBucketModel::
setBucketCount(int i)
{
  bucketer_.setNumAuto(i);

  bucketValid_ = false;
}

void
CQBucketModel::
doResetModel()
{
  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return;

  beginResetModel();

  bucketValid_ = false;

  bucket();

  endResetModel();
}

void
CQBucketModel::
connectSlots()
{
  connectDisconnectSlots(true);
}

void
CQBucketModel::
disconnectSlots()
{
  connectDisconnectSlots(false);
}

void
CQBucketModel::
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
    SIGNAL(columnsInserted(const QModelIndex &, int, int)), SLOT(bucketSlot()));
  connectDisconnect(b,
    SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
    SLOT(bucketSlot()));
  connectDisconnect(b,
    SIGNAL(columnsRemoved(const QModelIndex &, int, int)), SLOT(bucketSlot()));

  connectDisconnect(b,
    SIGNAL(modelReset()), SLOT(bucketSlot()));

  connectDisconnect(b,
    SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(bucketSlot()));
  connectDisconnect(b,
    SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)), SLOT(bucketSlot()));
  connectDisconnect(b,
    SIGNAL(rowsRemoved(const QModelIndex &, int, int)), SLOT(bucketSlot()));
}

void
CQBucketModel::
bucketSlot()
{
  bucketValid_ = true;

  bucket();
}

void
CQBucketModel::
bucket()
{
  if (bucketValid_)
    return;

  bucketValid_ = true;

  //---

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

    calcRMinMax(model, QModelIndex(), rset, rmin, rmax);

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
calcRMinMax(QAbstractItemModel *model, const QModelIndex &parent,
            bool &rset, double &rmin, double &rmax) const
{
  int nr = model->rowCount(parent);

  for (int r = 0; r < nr; ++r) {
    QModelIndex parent1 = model->index(r, 0, parent);

    int nr1 = model->rowCount(parent1);

    if (nr1 > 0) {
      calcRMinMax(model, parent1, rset, rmin, rmax);
    }
    else {
      QModelIndex ind = model->index(r, bucketColumn(), parent);

      QVariant var = model->data(ind, Qt::DisplayRole);

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
  }
}

void
CQBucketModel::
clear()
{
  bucketed_ = false;
}

//------

// get number of columns
int
CQBucketModel::
columnCount(const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->sourceModel();

  if (! model)
    return 0;

  if (isMultiColumn())
    return model->columnCount(parent) + 3;
  else
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

  //---

  int c1 = mapColumn(c);

  if (c1 >= 0) {
    QModelIndex ind = model->index(r, c1, index.parent());

    return model->data(ind, role);
  }

  //---

  const_cast<CQBucketModel *>(this)->bucket();

  //---

  if (! isMultiColumn()) {
    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
      QModelIndex ind = model->index(r, bucketColumn(), index.parent());

      QVariant var = model->data(ind, bucketRole());

      int bucket = bucketer_.bucket(var);

      if      (role == Qt::DisplayRole) {
        return bucketer_.bucketName(bucket);
      }
      else if (role == Qt::EditRole) {
        return bucket;
      }
      else if (role == Qt::ToolTipRole) {
        return QString("%1 (%2)").arg(bucketer_.bucketName(bucket)).arg(bucket);
      }
    }
  }
  else {
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
      QModelIndex ind = model->index(r, bucketColumn(), index.parent());

      QVariant var = model->data(ind, bucketRole());

      int bucket = bucketer_.bucket(var);

      if      (c1 == -3) {
        return bucket;
      }
      else if (c1 == -2) {
        QVariant min, max;

        bucketer_.bucketMinMax(bucket, min, max);

        return min;
      }
      else {
        QVariant min, max;

        bucketer_.bucketMinMax(bucket, min, max);

        return max;
      }
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

  //---

  int c1 = mapColumn(c);

  if (c1 >= 0) {
    QModelIndex ind = model->index(r, c, index.parent());

    return model->setData(ind, value, role);
  }

  //---

  // can't set bucket data

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

  //---

  int section1 = mapColumn(section);

  if (section1 >= 0) {
    return model->headerData(section1, orientation, role);
  }

  //---

  const_cast<CQBucketModel *>(this)->bucket();

  //---

  if (! isMultiColumn()) {
    if (role == Qt::DisplayRole) {
      QString name = model->headerData(bucketColumn(), orientation, role).toString();

      return QString("bucket(%1)").arg(name);
    }
  }
  else {
    if      (section1 == -3) {
      return "bucket";
    }
    else if (section1 == -2) {
      return "bucket_min";
    }
    else {
      return "bucket_max";
    }
  }

  //---

  // pass to base class ?
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

  //---

  int section1 = mapColumn(section);

  if (section1 >= 0) {
    return model->setHeaderData(section1, orientation, value, role);
  }

  //---

  // can't set bucket data

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

  //---

  int c1 = mapColumn(c);

  if (c1 >= 0) {
    QModelIndex ind = model->index(r, c1, index.parent());

    return model->flags(ind);
  }

  //---

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

  //---

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

  //---

  int c1 = mapColumn(c);

  if (c1 >= 0)
    return model->index(r, c1, proxyIndex.parent());

  return QModelIndex();
}

// map bucket column to source column
int
CQBucketModel::
mapColumn(int c) const
{
  int b1 = bucketPos();
  int nb = (! isMultiColumn() ? 1 : 3);
  int b2 = b1 + nb - 1;

  if (c < b1)
    return c;

  if (c > b2) {
    if (isMultiColumn())
      return c - 3;
    else
      return c - 1;
  }

  return c - b2 - 1;
}

// map source column to bucket column
int
CQBucketModel::
unmapColumn(int c) const
{
  if (c < bucketPos())
    return c;

  if (c >= bucketPos()) {
    if (isMultiColumn())
      return c + 3;
    else
      return c + 1;
  }

  return -1;
}
