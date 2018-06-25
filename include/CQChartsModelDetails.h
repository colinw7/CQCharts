#ifndef CQChartsModelDetails_H
#define CQChartsModelDetails_H

#include <CQChartsColumn.h>
#include <CQChartsUtil.h>

class CQCharts;
class CQChartsIValues;
class CQChartsRValues;
class CQChartsSValues;
class QAbstractItemModel;

class CQChartsModelColumnDetails {
 public:
  using VariantList = QList<QVariant>;

 public:
  CQChartsModelColumnDetails(CQCharts *charts, QAbstractItemModel *model,
                             const CQChartsColumn &column);

  virtual ~CQChartsModelColumnDetails();

  CQCharts *charts() const { return charts_; }

  QAbstractItemModel *model() const { return model_; }

  const CQChartsColumn &column() const { return column_; }

  QString typeName() const;

  CQBaseModel::Type type() const;
  void setType(CQBaseModel::Type type);

  const CQChartsNameValues &nameValues() const;

  QVariant minValue() const;
  QVariant maxValue() const;

  QVariant meanValue() const;

  QVariant dataName(const QVariant &v) const;

  int numRows() const;

  bool isMonotonic () const;
  bool isIncreasing() const;

  int numUnique() const;

  VariantList uniqueValues() const;
  VariantList uniqueCounts() const;

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
  CQCharts*           charts_      { nullptr };
  QAbstractItemModel* model_       { nullptr };
  CQChartsColumn      column_;
  QString             typeName_;
  CQBaseModel::Type   type_        { CQBaseModel::Type::NONE };
  CQChartsNameValues  nameValues_;
  QVariant            minValue_;
  QVariant            maxValue_;
  int                 numRows_     { 0 };
  bool                monotonic_   { true };
  bool                increasing_  { true };
  bool                initialized_ { false };
  CQChartsIValues*    ivals_       { nullptr };
  CQChartsRValues*    rvals_       { nullptr };
  CQChartsSValues*    svals_       { nullptr };
};

//---

class CQChartsModelDetails {
 public:
  CQChartsModelDetails(CQCharts *charts=nullptr, QAbstractItemModel *model=nullptr);

 ~CQChartsModelDetails();

  int numColumns() const { initData(); return numColumns_; }

  int numRows() const { initData(); return numRows_; }

  bool isHierarchical() const { initData(); return hierarchical_; }

  CQChartsModelColumnDetails *columnDetails(int i);
  const CQChartsModelColumnDetails *columnDetails(int i) const;

  void reset();

  void update();

 private:
  void initData() const;

 private:
  CQChartsModelDetails(const CQChartsModelDetails &) = delete;
  CQChartsModelDetails &operator=(const CQChartsModelDetails &) = delete;

 private:
  using ColumnDetails = std::vector<CQChartsModelColumnDetails *>;

  CQCharts*           charts_       { nullptr };
  QAbstractItemModel* model_        { nullptr };
  bool                initialized_  { false };
  int                 numColumns_   { 0 };
  int                 numRows_      { 0 };
  bool                hierarchical_ { false };
  ColumnDetails       columnDetails_;
};

#endif
