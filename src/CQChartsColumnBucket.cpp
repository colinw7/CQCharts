#include <CQChartsColumnBucket.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>

CQChartsColumnBucket::
CQChartsColumnBucket(ColumnType columnType) :
 columnType_(columnType)
{
  valueSet_ = new CQChartsValueSet;
}

CQChartsColumnBucket::
~CQChartsColumnBucket()
{
  delete valueSet_;
}

void
CQChartsColumnBucket::
setColumn(const CQChartsColumn &c)
{
  column_ = c;
}

void
CQChartsColumnBucket::
setColumnType(ColumnType columnType)
{
  if (columnType != columnType_) {
    columnType_ = columnType;

    clear();
  }
}

void
CQChartsColumnBucket::
setDataType(const DataType &t)
{
  dataType_ = t;
}

void
CQChartsColumnBucket::
setRowGrouping(bool b)
{
  rowGrouping_ = b;
}

void
CQChartsColumnBucket::
setUseRow(bool b)
{
  useRow_ = b;
}

void
CQChartsColumnBucket::
setExactValue(bool b)
{
  exactValue_ = b;
}

void
CQChartsColumnBucket::
setBucketer(const CQBucketer &b)
{
  bucketer_ = b;
}

void
CQChartsColumnBucket::
reset()
{
  column_     = CQChartsColumn();
  columnType_ = ColumnType::NONE;

  dataType_    = DataType::NONE;
  rowGrouping_ = false;
  useRow_      = false;
  exactValue_  = false;
  bucketer_    = CQBucketer();

  clear();
}

void
CQChartsColumnBucket::
clear()
{
  valueSet_->clearVals();

  indName_.clear();
}

int
CQChartsColumnBucket::
addValue(const QVariant &value)
{
  assert(columnType() != ColumnType::NONE);

  bool ok;

  if      (columnType() == ColumnType::REAL) {
    double r = CQChartsVariant::toReal(value, ok);
    if (! ok) return -1;

    return addReal(r);
  }
  else if (columnType() == ColumnType::INTEGER) {
    long i = CQChartsVariant::toInt(value, ok);
    if (! ok) return -1;

    return addInteger(int(i));
  }
  else if (columnType() == ColumnType::STRING) {
    QString s;

    CQChartsVariant::toString(value, s);

    return addString(s);
  }

  return -1;
}

int
CQChartsColumnBucket::
addReal(double r)
{
  return valueSet_->rvals().addValue(r);
}

int
CQChartsColumnBucket::
addInteger(int i)
{
  return valueSet_->ivals().addValue(i);
}

int
CQChartsColumnBucket::
addString(const QString &s)
{
  return valueSet_->svals().addValue(s);
}

int
CQChartsColumnBucket::
ind(const QVariant &value) const
{
  bool ok;

  if      (columnType() == ColumnType::REAL) {
    double r = CQChartsVariant::toReal(value, ok);
    if (! ok) return 0;

    return valueSet_->rvals().id(r);
  }
  else if (columnType() == ColumnType::INTEGER) {
    long i = CQChartsVariant::toInt(value, ok);
    if (! ok) return 0;

    return valueSet_->ivals().id(int(i));
  }
  else if (columnType() == ColumnType::STRING) {
    QString s;

    CQChartsVariant::toString(value, s);

    return valueSet_->svals().id(s);
  }
  else
    return 0;
}

int
CQChartsColumnBucket::
ind(const QString &s) const
{
  return valueSet_->svals().id(s);
}

int
CQChartsColumnBucket::
bucket(const QVariant &value) const
{
  return bucketer_.bucket(value);
}

int
CQChartsColumnBucket::
sbucket(const QVariant &value) const
{
  if      (columnType() == ColumnType::REAL) {
    return -1;
  }
  else if (columnType() == ColumnType::INTEGER) {
    return -1;
  }
  else if (columnType() == ColumnType::STRING) {
    QString s;

    CQChartsVariant::toString(value, s);

    return valueSet_->svals().sbucket(s);
  }
  else
    return -1;
}

QString
CQChartsColumnBucket::
buckets(int ind) const
{
  if      (columnType() == ColumnType::REAL) {
    return "";
  }
  else if (columnType() == ColumnType::INTEGER) {
    return "";
  }
  else if (columnType() == ColumnType::STRING) {
    return valueSet_->svals().buckets(ind);
  }
  else
    return "";
}

QString
CQChartsColumnBucket::
bucketName(int ind) const
{
  return bucketer_.bucketName(ind);
}

QVariant
CQChartsColumnBucket::
bucketValue(int ind) const
{
  if      (columnType() == ColumnType::REAL) {
    return CQChartsVariant::fromReal(valueSet_->rvals().ivalue(ind));
  }
  else if (columnType() == ColumnType::INTEGER) {
    return CQChartsVariant::fromInt(valueSet_->ivals().ivalue(ind));
  }
  else if (columnType() == ColumnType::STRING) {
    return QVariant(valueSet_->svals().ivalue(ind));
  }
  else
    return "";
}

QString
CQChartsColumnBucket::
iname(int ind) const
{
  if      (columnType() == ColumnType::REAL)
    return QString::number(valueSet_->rvals().ivalue(ind));
  else if (columnType() == ColumnType::INTEGER)
    return QString::number(valueSet_->ivals().ivalue(ind));
  else if (columnType() == ColumnType::STRING)
    return valueSet_->svals().ivalue(ind);
  else
    return "";
}

int
CQChartsColumnBucket::
numUnique() const
{
  if      (columnType() == ColumnType::REAL)
    return valueSet_->rvals().numUnique();
  else if (columnType() == ColumnType::INTEGER)
    return valueSet_->ivals().numUnique();
  else if (columnType() == ColumnType::STRING)
    return valueSet_->svals().numUnique();
  else
    return 0;
}

//---

int
CQChartsColumnBucket::
numBuckets() const
{
  if      (columnType() == ColumnType::REAL) {
    auto rmin = this->rmin();
    auto rmax = this->rmax();

    int bucket1 = bucket(rmin);
    int bucket2 = bucket(rmax);

    return (bucket2 - bucket1 + 1);
  }
  else if (columnType() == ColumnType::INTEGER) {
    if (isExactValue())
      return numUnique();

    auto imin = this->imin();
    auto imax = this->imax();

    int bucket1 = bucket(imin);
    int bucket2 = bucket(imax);

    return (bucket2 - bucket1 + 1);
  }
  else {
    if (isExactValue())
      return numUnique();

    return valueSet_->svals().numBuckets();
  }
}

void
CQChartsColumnBucket::
bucketInds(std::vector<int> &inds)
{
  if      (columnType() == ColumnType::REAL) {
    double rmin = this->rmin();
    double rmax = this->rmax();

    int bucket1 = bucket(rmin);
    int bucket2 = bucket(rmax);

    for (int groupInd = bucket1; groupInd <= bucket2; ++groupInd)
      inds.push_back(groupInd);
  }
  else if (columnType() == ColumnType::INTEGER) {
    if (isExactValue()) {
      for (int groupInd = imin(); groupInd <= imax(); ++groupInd)
        inds.push_back(groupInd);
    }
    else {
      auto imin = this->imin();
      auto imax = this->imax();

      int bucket1 = bucket(imin);
      int bucket2 = bucket(imax);

      for (int groupInd = bucket1; groupInd <= bucket2; ++groupInd)
        inds.push_back(groupInd);
    }
  }
  else {
    for (int groupInd = imin(); groupInd <= imax(); ++groupInd)
      inds.push_back(groupInd);
  }
}

QString
CQChartsColumnBucket::
bucketIndName(int ind, bool hier) const
{
  if      (isExactValue()) {
    return indName(ind);
  }
  else if (columnType() == ColumnType::REAL ||
           columnType() == ColumnType::INTEGER) {
    return bucketName(ind);
  }
  else {
    if (hier)
      return iname(ind);
    else
      return buckets(ind);
  }
}

//---

int
CQChartsColumnBucket::
imin() const
{
  if      (columnType() == ColumnType::REAL)
    return valueSet_->rvals().imin();
  else if (columnType() == ColumnType::INTEGER)
    return valueSet_->ivals().imin();
  else if (columnType() == ColumnType::STRING)
    return valueSet_->svals().imin();
  else {
    assert(false); return 0;
  }
}

int
CQChartsColumnBucket::
imax() const
{
  if      (columnType() == ColumnType::REAL)
    return valueSet_->rvals().imax();
  else if (columnType() == ColumnType::INTEGER)
    return valueSet_->ivals().imax();
  else if (columnType() == ColumnType::STRING)
    return valueSet_->svals().imax();
  else {
    assert(false); return 0;
  }
}

double
CQChartsColumnBucket::
rmin() const
{
  if      (columnType() == ColumnType::REAL)
    return valueSet_->rvals().min();
  else if (columnType() == ColumnType::INTEGER)
    return valueSet_->ivals().imin();
  else if (columnType() == ColumnType::STRING) {
    assert(false); return 0;
  }
  else {
    assert(false); return 0;
  }
}

double
CQChartsColumnBucket::
rmax() const
{
  if      (columnType() == ColumnType::REAL)
    return valueSet_->rvals().max();
  else if (columnType() == ColumnType::INTEGER)
    return valueSet_->ivals().imax();
  else if (columnType() == ColumnType::STRING) {
    assert(false); return 0;
  }
  else {
    assert(false); return 0;
  }
}

void
CQChartsColumnBucket::
setIndName(int i, const QString &name)
{
  indName_[i] = name;
}

QString
CQChartsColumnBucket::
indName(int i) const
{
  auto p = indName_.find(i);

  if (p != indName_.end())
    return (*p).second;

  return iname(i);
}

void
CQChartsColumnBucket::
print(std::ostream &os) const
{
  os << "Bucket:\n";

  if      (dataType() == DataType::HEADER) {
    os << " Header:\n";
  }
  else if (dataType() == DataType::COLUMN) {
    os << " Column:\n";
    os << "  Type:   " << typeName().toStdString() << "\n";
    os << "  Column: " << column_.column() << "\n";
  }
  else if (dataType() == DataType::COLUMN_ROOT) {
    os << " Column Root:\n";
    os << "  Type:   " << typeName().toStdString() << "\n";
    os << "  Column: " << column_.column() << "\n";
  }
  else if (dataType() == DataType::PATH) {
    os << " Path:\n";
  }
}

QString
CQChartsColumnBucket::
typeName() const
{
  if      (columnType() == ColumnType::REAL   ) return "real";
  else if (columnType() == ColumnType::INTEGER) return "integer";
  else if (columnType() == ColumnType::STRING ) return "string";
  else                                          { assert(false); return ""; }
}
