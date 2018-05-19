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

  CQBaseModel::Type type() const { return type_; }
  void setType(CQBaseModel::Type type) { type_ = type; }

  const CQChartsNameValues &nameValues() const { return nameValues_; }

  QVariant minValue() const;
  QVariant maxValue() const;

  QVariant dataName(const QVariant &v) const;

  int numRows() const;

  bool isMonotonic () const { return monotonic_; }
  bool isIncreasing() const { return increasing_; }

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
  void setNumColumns(int i) { numColumns_ = i; }

  int numRows() const { init(); return numRows_; }
  void setNumRows(int i) { numRows_ = i; }

  CQChartsModelColumnDetails &columnDetails(int i) { return columnDetails_[i]; }

  void init() const;

  void update();

 private:
  using ColumnDetails = std::vector<CQChartsModelColumnDetails>;

  CQCharts*           charts_      { nullptr };
  QAbstractItemModel* model_       { nullptr };
  bool                initialized_ { false };
  int                 numColumns_  { 0 };
  int                 numRows_     { 0 };
  ColumnDetails       columnDetails_;
};

#endif
