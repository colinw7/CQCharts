#ifndef CQChartsModelDetails_H
#define CQChartsModelDetails_H

#include <CQChartsColumn.h>
#include <CQChartsColumnType.h>
#include <CQBaseModelTypes.h>
#include <CQChartsUtil.h>
#include <future>

class CQChartsModelColumnDetails;
class CQChartsModelData;
class CQCharts;
class CQChartsValueSet;

class QAbstractItemModel;

/*!
 * \brief Model Details
 * \ingroup Charts
 */
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

  CQCharts *charts() const;

  QAbstractItemModel *model() const;

 signals:
  void detailsReset();

 private slots:
  void modelTypeChangedSlot(int modelInd);

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

  CQChartsModelData* data_ { nullptr }; //!< model data

  // cached data
  Initialized   initialized_  { Initialized::NONE }; //!< is initialized
  int           numColumns_   { 0 };                 //!< model number of columns
  int           numRows_      { 0 };                 //!< model number of rows
  bool          hierarchical_ { false };             //!< model is hierarchical
  ColumnDetails columnDetails_;                      //!< model column details

  // mutex
  mutable std::mutex mutex_; //!< mutex
};

//---

/*!
 * \brief Model Column Details
 * \ingroup Charts
 */
class CQChartsModelColumnDetails {
 public:
  using VariantList   = QList<QVariant>;
  using TableDrawType = CQChartsColumnType::DrawType;

 public:
  CQChartsModelColumnDetails(CQChartsModelDetails *details, const CQChartsColumn &column);

  virtual ~CQChartsModelColumnDetails();

  CQChartsModelDetails *details() const { return details_; }

  const CQChartsColumn &column() const { return column_; }

  QString headerName() const;

  bool isKey() const;

  QString typeName() const;

  CQBaseModelType type() const;
  void setType(CQBaseModelType type);

  CQBaseModelType baseType() const;
  void setBaseType(CQBaseModelType type);

  const CQChartsNameValues &nameValues() const;

  static const QStringList &getLongNamedValues();
  static const QStringList &getShortNamedValues();

  static bool isNamedValue(const QString &name);

  QVariant getNamedValue(const QString &name) const;

  QVariant minValue() const;
  QVariant maxValue() const;

  QVariant meanValue(bool useNaN=true) const;

  QVariant stdDevValue(bool useNaN=true) const;

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

  QVariant medianValue     (bool useNaN=true) const;
  QVariant lowerMedianValue(bool useNaN=true) const;
  QVariant upperMedianValue(bool useNaN=true) const;

  QVariantList outlierValues() const;

  bool isOutlier(const QVariant &value) const;

  double map(const QVariant &var) const;

  const CQChartsColor &tableDrawColor() const { return tableDrawColor_; }
  void setTableDrawColor(const CQChartsColor &c) { tableDrawColor_ = c; }

  const TableDrawType &tableDrawType() const { return tableDrawType_; }
  void setTableDrawType(const TableDrawType &t) { tableDrawType_ = t; }

  const CQChartsColorStops &tableDrawStops() const { return tableDrawStops_; }
  void setTableDrawType(const CQChartsColorStops &s) { tableDrawStops_ = s; }

  bool columnNameValue(const QString &name, QString &value) const;

  virtual bool checkRow(const QVariant &) { return true; }

  void initCache() const;

  void resetTypeInitialized() { typeInitialized_ = false; }

  const CQChartsColumnType *columnType() const;

 private:
  bool initData();

  void initType() const;
  bool calcType();

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

  // cached type data
  bool                  typeInitialized_ { false };                 //!< is type data set
  CQBaseModelType       type_            { CQBaseModelType::NONE }; //!< column data type
  CQBaseModelType       baseType_        { CQBaseModelType::NONE }; //!< column data base type
  CQChartsNameValues    nameValues_;                                //!< name values
  QString               typeName_;                                  //!< type name

  // cached data
  bool                  initialized_     { false };   //!< is data set
  QVariant              minValue_;                    //!< min value (as variant)
  QVariant              maxValue_;                    //!< max value (as variant)
  int                   numRows_         { 0 };       //!< number of rows
  bool                  monotonic_       { true };    //!< values are monotonic
  bool                  increasing_      { true };    //!< values are increasing
  CQChartsValueSet*     valueSet_        { nullptr }; //!< values
  VariantInds           valueInds_;                   //!< unique values

  // table render data
  CQChartsColor         tableDrawColor_;
  TableDrawType         tableDrawType_   { TableDrawType::HEATMAP };
  CQChartsColorStops    tableDrawStops_;

  // mutex
  mutable std::mutex    mutex_; //!< mutex
};

#endif
