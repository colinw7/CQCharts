#ifndef CQChartsColumnBucket_H
#define CQChartsColumnBucket_H

#include <CQChartsValueSet.h>
#include <CQBaseModel.h>

// bucket of values keyed off header, column or path
class CQChartsColumnBucket {
 public:
  using ColumnType = CQBaseModel::Type;

  enum class DataType {
    NONE,
    HEADER,
    COLUMN,
    PATH
  };

 public:
  CQChartsColumnBucket(ColumnType columnType=ColumnType::NONE) :
   columnType_(columnType) {
  }

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c) { column_ = c; }

  ColumnType columnType() const { return columnType_; }

  void setColumnType(ColumnType columnType) {
    if (columnType != columnType_) {
      columnType_ = columnType;

      clear();
    }
  }

  const DataType &dataType() const { return dataType_; }
  void setDataType(const DataType &v) { dataType_ = v; }

  bool isRowGrouping() const { return rowGrouping_; }
  void setRowGrouping(bool b) { rowGrouping_ = b; }

  bool isDefaultRow() const { return defaultRow_; }
  void setDefaultRow(bool b) { defaultRow_ = b; }

  void clear() {
    rvals_.clear();
    ivals_.clear();
    svals_.clear();

    indName_.clear();
  }

  int addValue(const QVariant &value) {
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

  int addReal(double r) {
    return rvals_.addValue(r);
  }

  int addInteger(int i) {
    return ivals_.addValue(i);
  }

  int addString(const QString &s) {
    return svals_.addValue(s);
  }

  // get index for value
  int ind(const QVariant &value) const {
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

  // get index for string
  int ind(const QString &s) const {
    return svals_.id(s);
  }

  // get name for index
  QString iname(int ind) const {
    if      (columnType() == ColumnType::REAL   ) return QString("%1").arg(rvals_.ivalue(ind));
    else if (columnType() == ColumnType::INTEGER) return QString("%1").arg(ivals_.ivalue(ind));
    else if (columnType() == ColumnType::STRING ) return svals_.ivalue(ind);
    else                                          return "";
  }

  int numUnique() const {
    if      (columnType() == ColumnType::REAL   ) return rvals_.numUnique();
    else if (columnType() == ColumnType::INTEGER) return ivals_.numUnique();
    else if (columnType() == ColumnType::STRING ) return svals_.numUnique();
    else                                          return 0;
  }

  int imin() const {
    if      (columnType() == ColumnType::REAL   ) return rvals_.imin();
    else if (columnType() == ColumnType::INTEGER) return ivals_.imin();
    else if (columnType() == ColumnType::STRING ) return svals_.imin();
    else                                          assert(false);
  }

  int imax() const {
    if      (columnType() == ColumnType::REAL   ) return rvals_.imax();
    else if (columnType() == ColumnType::INTEGER) return ivals_.imax();
    else if (columnType() == ColumnType::STRING ) return svals_.imax();
    else                                          assert(false);
  }

  void setIndName(int i, const QString &name) {
    indName_[i] = name;
  }

  QString indName(int i) const {
    auto p = indName_.find(i);

    if (p != indName_.end())
      return (*p).second;

    return iname(i);
  }

  void print(std::ostream &os) const {
    os << "Bucket:\n";

    if      (dataType() == DataType::HEADER) {
      os << " Header:\n";
    }
    else if (dataType() == DataType::COLUMN) {
      os << " Column:\n";
      os << "  Type:   " << typeName().toStdString() << "\n";
      os << "  Column: " << column_.column() << "\n";
    }
    else if (dataType() == DataType::COLUMN) {
      os << " Path:\n";
    }
  }

  QString typeName() const {
    if      (columnType() == ColumnType::REAL   ) return "real";
    else if (columnType() == ColumnType::INTEGER) return "integer";
    else if (columnType() == ColumnType::STRING ) return "string";
    else                                          assert(false);
  }

 private:
  using IndName = std::map<int,QString>;

  CQChartsColumn  column_;
  ColumnType      columnType_  { ColumnType::NONE };
  DataType        dataType_    { DataType::NONE };
  bool            rowGrouping_ { false };
  bool            defaultRow_  { false };
  CQChartsRValues rvals_;
  CQChartsIValues ivals_;
  CQChartsSValues svals_;
  IndName         indName_;
};

#endif
