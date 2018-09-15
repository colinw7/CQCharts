#ifndef CQChartsModelDetails_H
#define CQChartsModelDetails_H

#include <CQChartsColumn.h>
#include <CQChartsUtil.h>

class CQChartsModelDetails;
class CQChartsModelData;
class CQCharts;
class CQChartsIValues;
class CQChartsRValues;
class CQChartsSValues;
class QAbstractItemModel;

class CQChartsModelColumnDetails {
 public:
  using VariantList = QList<QVariant>;

 public:
  CQChartsModelColumnDetails(CQChartsModelDetails *details, const CQChartsColumn &column);

  virtual ~CQChartsModelColumnDetails();

  CQChartsModelDetails *details() const { return details_; }

  const CQChartsColumn &column() const { return column_; }

  QString headerName() const;

  QString typeName() const;

  CQBaseModel::Type type() const;
  void setType(CQBaseModel::Type type);

  const CQChartsNameValues &nameValues() const;

  static bool isNamedValue(const QString &name);

  QVariant getNamedValue(const QString &name) const;

  QVariant minValue() const;
  QVariant maxValue() const;

  QVariant meanValue() const;

  QVariant stdDevValue() const;

  QVariant dataName(const QVariant &v) const;

  int numRows() const;

  bool isNumeric() const;

  bool isMonotonic () const;
  bool isIncreasing() const;

  int numUnique() const;

  VariantList uniqueValues() const;
  VariantList uniqueCounts() const;

  int uniqueId(const QVariant &v) const;

  QVariant uniqueValue(int i) const;

  int numNull() const;

  QVariant medianValue() const;
  QVariant lowerMedianValue() const;
  QVariant upperMedianValue() const;

  QVariantList outlierValues() const;

  double map(const QVariant &var) const;

  virtual bool checkRow(const QVariant &) { return true; }

 private:
  bool initData();

  void addInt   (int i);
  void addReal  (double r);
  void addString(const QString &s);

 private:
  CQChartsModelColumnDetails(const CQChartsModelColumnDetails &) = delete;
  CQChartsModelColumnDetails &operator=(const CQChartsModelColumnDetails &) = delete;

 private:
  CQChartsModelDetails* details_     { nullptr };
  CQChartsColumn        column_;
  QString               typeName_;
  CQBaseModel::Type     type_        { CQBaseModel::Type::NONE };
  CQChartsNameValues    nameValues_;
  QVariant              minValue_;
  QVariant              maxValue_;
  int                   numRows_     { 0 };
  bool                  monotonic_   { true };
  bool                  increasing_  { true };
  bool                  initialized_ { false };
  CQChartsIValues*      ivals_       { nullptr };
  CQChartsRValues*      rvals_       { nullptr };
  CQChartsSValues*      svals_       { nullptr };
};

//---

class CQChartsModelDetails {
 public:
  CQChartsModelDetails(CQChartsModelData *data);

 ~CQChartsModelDetails();

  CQChartsModelData *data() const { return data_; }

  int numColumns() const { initData(); return numColumns_; }

  int numRows() const { initData(); return numRows_; }

  bool isHierarchical() const { initData(); return hierarchical_; }

  CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column);
  const CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) const;

  CQChartsColumns numericColumns() const;

  CQChartsColumns monotonicColumns() const;

  void reset();

  void update();

 private:
  void initData() const;

 private:
  CQChartsModelDetails(const CQChartsModelDetails &) = delete;
  CQChartsModelDetails &operator=(const CQChartsModelDetails &) = delete;

 private:
  using ColumnDetails = std::map<CQChartsColumn,CQChartsModelColumnDetails *>;

  CQChartsModelData* data_         { nullptr };
  bool               initialized_  { false };
  int                numColumns_   { 0 };
  int                numRows_      { 0 };
  bool               hierarchical_ { false };
  ColumnDetails      columnDetails_;
};

#endif
