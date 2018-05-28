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

  QVariant dataName(const QVariant &v) const;

  int numRows() const;

  bool isMonotonic () const;
  bool isIncreasing() const;

  int numUnique() const;

  double map(const QVariant &var) const;

  virtual bool checkRow(const QVariant &) { return true; }

 private:
  bool init();

  void addInt   (int i);
  void addReal  (double r);
  void addString(const QString &s);

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

  int numColumns() const { init(); return numColumns_; }

  int numRows() const { init(); return numRows_; }

  bool isHierarchical() const { init(); return hierarchical_; }

  const CQChartsModelColumnDetails &columnDetails(int i) const { init(); return columnDetails_[i]; }
  CQChartsModelColumnDetails &columnDetails(int i) { init(); return columnDetails_[i]; }

  void init() const;

  void reset();

  void update();

 private:
  using ColumnDetails = std::vector<CQChartsModelColumnDetails>;

  CQCharts*           charts_       { nullptr };
  QAbstractItemModel* model_        { nullptr };
  bool                initialized_  { false };
  int                 numColumns_   { 0 };
  int                 numRows_      { 0 };
  bool                hierarchical_ { false };
  ColumnDetails       columnDetails_;
};

#endif
