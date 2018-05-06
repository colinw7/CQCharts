#ifndef CQChartsModelDetails_H
#define CQChartsModelDetails_H

#include <CQChartsColumn.h>
#include <CQChartsUtil.h>

class CQCharts;
class QAbstractItemModel;

class CQChartsModelColumnDetails {
 public:
  CQChartsModelColumnDetails(CQCharts *charts, QAbstractItemModel *model,
                             const CQChartsColumn &column);

  virtual ~CQChartsModelColumnDetails() { }

  CQCharts *charts() const { return charts_; }

  QAbstractItemModel *model() const { return model_; }

  const CQChartsColumn &column() const { return column_; }

  QString typeName() const;

  QVariant minValue() const;
  QVariant maxValue() const;

  QVariant dataName(const QVariant &v) const;

  int numRows() const;

  bool isMonotonic () const { return monotonic_; }
  bool isIncreasing() const { return increasing_; }

  virtual bool checkRow(const QVariant &) { return true; }

 private:
  bool init();

 private:
  CQCharts*           charts_      { nullptr };
  QAbstractItemModel* model_       { nullptr };
  CQChartsColumn      column_;
  QString             typeName_;
  CQBaseModel::Type   type_        { CQBaseModel::Type::NONE };
  CQChartsNameValues  nameValues_;
  QVariant            minValue_;
  QVariant            maxValue_;
  int                 numRows_;
  bool                monotonic_   { true };
  bool                increasing_  { true };
  bool                initialized_ { false };
};

//---

class CQChartsModelDetails {
 public:
  CQChartsModelDetails();

  int numColumns() const { return numColumns_; }
  void setNumColumns(int i) { numColumns_ = i; }

  int numRows() const { return numRows_; }
  void setNumRows(int i) { numRows_ = i; }

  CQChartsModelColumnDetails &columnDetails(int i) { return columnDetails_[i]; }

  void update(CQCharts *charts, QAbstractItemModel *model);

 private:
  using ColumnDetails = std::vector<CQChartsModelColumnDetails>;

  CQCharts*           charts_      { nullptr };
  QAbstractItemModel* model_       { nullptr };
  int                 numColumns_  { 0 };
  int                 numRows_     { 0 };
  ColumnDetails       columnDetails_;
};

#endif
