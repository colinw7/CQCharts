#ifndef CQChartsColumnBucket_H
#define CQChartsColumnBucket_H

#include <CQChartsModelTypes.h>
#include <CQChartsColumn.h>
#include <CQBucketer.h>

class CQChartsValueSet;

/*!
 * \brief bucket of values keyed off header, column or path
 * \ingroup Charts
 */
class CQChartsColumnBucket {
 public:
  using ColumnType = CQBaseModelType;

  enum class DataType {
    NONE,
    HEADER,      // bucket by header name
    COLUMN,      // bucket by column value
    COLUMN_ROOT, // bucket by column root value
    PATH         // bucket by hierarchical path
  };

  using Column = CQChartsColumn;

 public:
  explicit CQChartsColumnBucket(ColumnType columnType=ColumnType::NONE);

 ~CQChartsColumnBucket();

  const Column &column() const { return column_; }
  void setColumn(const Column &c);

  ColumnType columnType() const { return columnType_; }
  void setColumnType(ColumnType columnType);

  const DataType &dataType() const { return dataType_; }
  void setDataType(const DataType &v);

  bool isRowGrouping() const { return rowGrouping_; }
  void setRowGrouping(bool b);

  // default to row number
  bool isUseRow() const { return useRow_; }
  void setUseRow(bool b);

  bool isExactValue() const { return exactValue_; }
  void setExactValue(bool b);

  const CQBucketer &bucketer() const { return bucketer_; }
  void setBucketer(const CQBucketer &v);

  //---

  bool isValid() const {
    if (dataType() == DataType::NONE)
      return false;

    if (dataType() == DataType::COLUMN && columnType() == ColumnType::NONE)
      return false;

    return true;
  }

  void reset();

  void clear();

  int addValue(const QVariant &value);

  int addReal(double r);

  int addInteger(int i);

  int addString(const QString &s);

  // get index for value
  int ind(const QVariant &value) const;

  // get index for string
  int ind(const QString &s) const;

  // get name for index
  QString iname(int ind) const;

  int bucket(const QVariant &value) const;

  int     sbucket(const QVariant &value) const;
  QString buckets(int i) const;

  QString bucketName(int ind) const;

  QVariant bucketValue(int ind) const;

  uint numUnique() const;

  //---

  int numBuckets() const;

  void bucketInds(std::vector<int> &inds);

  QString bucketIndName(int ind, bool hier=false) const;

  //---

  int imin() const;
  int imax() const;

  double rmin() const;
  double rmax() const;

  void setIndName(int i, const QString &name);

  QString indName(int i) const;

  void print(std::ostream &os) const;

  QString typeName() const;

 private:
  using IndName = std::map<int, QString>;

  Column            column_;
  ColumnType        columnType_  { ColumnType::NONE };
  DataType          dataType_    { DataType::NONE };
  bool              rowGrouping_ { false };
  bool              useRow_      { false };
  bool              exactValue_  { false };
  CQBucketer        bucketer_;
  CQChartsValueSet* valueSet_    { nullptr };
  IndName           indName_;
};

#endif
