#include <CQChartsColumnBucket.h>

CQChartsColumnBucket::
CQChartsColumnBucket(ColumnType columnType) :
 columnType_(columnType)
{
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
setDataType(const DataType &v)
{
  dataType_ = v;
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
setBucketer(const CQBucketer &v)
{
  bucketer_ = v;
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
  rvals_.clear();
  ivals_.clear();
  svals_.clear();

  indName_.clear();
}

int
CQChartsColumnBucket::
addValue(const QVariant &value)
{
  assert(columnType() != ColumnType::NONE);

  bool ok;

  if      (columnType() == ColumnType::REAL) {
    double r = CQChartsUtil::toReal(value, ok);
    if (! ok) return -1;

    return addReal(r);
  }
  else if (columnType() == ColumnType::INTEGER) {
    long i = CQChartsUtil::toInt(value, ok);
    if (! ok) return -1;

    return addInteger(i);
  }
  else if (columnType() == ColumnType::STRING) {
    QString s;

    CQChartsUtil::variantToString(value, s);

    return addString(s);
  }

  return -1;
}

int
CQChartsColumnBucket::
addReal(double r)
{
  return rvals_.addValue(r);
}

int
CQChartsColumnBucket::
addInteger(int i)
{
  return ivals_.addValue(i);
}

int
CQChartsColumnBucket::
addString(const QString &s)
{
  return svals_.addValue(s);
}

int
CQChartsColumnBucket::
ind(const QVariant &value) const
{
  bool ok;

  if      (columnType() == ColumnType::REAL) {
    double r = CQChartsUtil::toReal(value, ok);
    if (! ok) return 0;

    return rvals_.id(r);
  }
  else if (columnType() == ColumnType::INTEGER) {
    long i = CQChartsUtil::toInt(value, ok);
    if (! ok) return 0;

    return ivals_.id(i);
  }
  else if (columnType() == ColumnType::STRING) {
    QString s;

    CQChartsUtil::variantToString(value, s);

    return svals_.id(s);
  }
  else
    return 0;
}

int
CQChartsColumnBucket::
ind(const QString &s) const
{
  return svals_.id(s);
}

int
CQChartsColumnBucket::
bucket(double value) const
{
  return bucketer_.bucket(value);
}

QString
CQChartsColumnBucket::
bucketName(int ind) const
{
  return bucketer_.bucketName(ind);
}

QString
CQChartsColumnBucket::
iname(int ind) const
{
  if      (columnType() == ColumnType::REAL   ) return QString("%1").arg(rvals_.ivalue(ind));
  else if (columnType() == ColumnType::INTEGER) return QString("%1").arg(ivals_.ivalue(ind));
  else if (columnType() == ColumnType::STRING ) return svals_.ivalue(ind);
  else                                          return "";
}

int
CQChartsColumnBucket::
numUnique() const
{
  if      (columnType() == ColumnType::REAL   ) return rvals_.numUnique();
  else if (columnType() == ColumnType::INTEGER) return ivals_.numUnique();
  else if (columnType() == ColumnType::STRING ) return svals_.numUnique();
  else                                          return 0;
}

int
CQChartsColumnBucket::
imin() const
{
  if      (columnType() == ColumnType::REAL   ) return rvals_.imin();
  else if (columnType() == ColumnType::INTEGER) return ivals_.imin();
  else if (columnType() == ColumnType::STRING ) return svals_.imin();
  else                                          assert(false);
}

int
CQChartsColumnBucket::
imax() const
{
  if      (columnType() == ColumnType::REAL   ) return rvals_.imax();
  else if (columnType() == ColumnType::INTEGER) return ivals_.imax();
  else if (columnType() == ColumnType::STRING ) return svals_.imax();
  else                                          assert(false);
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
  else                                          assert(false);
}
