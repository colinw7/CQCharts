#ifndef CQChartsModelDetails_H
#define CQChartsModelDetails_H

#include <CQChartsColumn.h>
#include <CQChartsUtil.h>

class CQChartsModelDetails;
class CQChartsModelData;
class CQCharts;
class CQChartsValueSet;
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

  bool isKey() const;

  QString typeName() const;

  CQBaseModel::Type type() const;
  void setType(CQBaseModel::Type type);

  CQBaseModel::Type baseType() const;
  void setBaseType(CQBaseModel::Type type);

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

  int numValues() const { return valueInds_.size(); }

  int valueInd(const QVariant &value) const;

  QVariant medianValue() const;
  QVariant lowerMedianValue() const;
  QVariant upperMedianValue() const;

  QVariantList outlierValues() const;

  double map(const QVariant &var) const;

  virtual bool checkRow(const QVariant &) { return true; }

  void initCache() const;

 private:
  bool initData();
  bool initType();

  void addInt   (int i);
  void addReal  (double r);
  void addString(const QString &s);
  void addTime  (double t);
  void addColor (const CQChartsColor &c);

  void addValue(const QVariant &value);

  bool columnColor(const QVariant &var, CQChartsColor &color) const;

 private:
  CQChartsModelColumnDetails(const CQChartsModelColumnDetails &) = delete;
  CQChartsModelColumnDetails &operator=(const CQChartsModelColumnDetails &) = delete;

 private:
  using VariantInds = std::map<QVariant,int>;

  CQChartsModelDetails* details_         { nullptr };
  CQChartsColumn        column_;
  QString               typeName_;
  CQBaseModel::Type     type_            { CQBaseModel::Type::NONE };
  CQBaseModel::Type     baseType_        { CQBaseModel::Type::NONE };
  CQChartsNameValues    nameValues_;
  QVariant              minValue_;
  QVariant              maxValue_;
  int                   numRows_         { 0 };
  bool                  monotonic_       { true };
  bool                  increasing_      { true };
  bool                  initialized_     { false };
  bool                  typeInitialized_ { false };
  CQChartsValueSet*     valueSet_        { nullptr };
  VariantInds           valueInds_;
};

//---

class CQChartsModelDetails : public QObject {
  Q_OBJECT

  Q_PROPERTY(int numColumns   READ numColumns    )
  Q_PROPERTY(int numRows      READ numRows       )
  Q_PROPERTY(int hierarchical READ isHierarchical)

 public:
  CQChartsModelDetails(CQChartsModelData *data);

 ~CQChartsModelDetails();

  CQChartsModelData *data() const { return data_; }

  int numColumns() const;

  int numRows() const;

  bool isHierarchical() const;

  CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column);
  const CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) const;

  CQChartsColumns numericColumns() const;

  CQChartsColumns monotonicColumns() const;

  void reset();

  std::vector<int> duplicates() const;
  std::vector<int> duplicates(const CQChartsColumn &column) const;

 signals:
  void detailsReset();

 private:
  void resetValues();

  std::vector<int> columnDuplicates(const CQChartsColumn &column, bool all) const;

  void updateSimple();
  void updateFull();

  void initSimpleData() const;
  void initFullData() const;

 private:
  enum class Initialized {
    NONE,
    SIMPLE,
    FULL
  };

  CQChartsModelDetails(const CQChartsModelDetails &) = delete;
  CQChartsModelDetails &operator=(const CQChartsModelDetails &) = delete;

 private:
  using ColumnDetails = std::map<CQChartsColumn,CQChartsModelColumnDetails *>;

  CQChartsModelData* data_         { nullptr };
  Initialized        initialized_  { Initialized::NONE };
  int                numColumns_   { 0 };
  int                numRows_      { 0 };
  bool               hierarchical_ { false };
  ColumnDetails      columnDetails_;
};

#endif
