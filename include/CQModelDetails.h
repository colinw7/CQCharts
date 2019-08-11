#ifndef CQModelDetails_H
#define CQModelDetails_H

#include <CQBaseModelTypes.h>
#include <future>

class CQModelDetails;
class CQValueSet;

class QAbstractItemModel;

/*!
 * \brief Model Column Details
 */
class CQModelColumnDetails {
 public:
  using VariantList = QList<QVariant>;

 public:
  CQModelColumnDetails(CQModelDetails *details, int column);

  virtual ~CQModelColumnDetails();

  CQModelDetails *details() const { return details_; }

  QAbstractItemModel *model() const;

  int column() const { return column_; }

  QString headerName() const;

  bool isKey() const;

  CQBaseModelType type() const;
  void setType(CQBaseModelType type);

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

  virtual bool checkRow(const QVariant &) { return true; }

  void initCache() const;

  void resetTypeInitialized() { typeInitialized_ = false; }

 private:
  bool initData();

  void initType() const;
  bool calcType();

  void addInt   (int i);
  void addReal  (double r);
  void addString(const QString &s);

  void addValue(const QVariant &value);

 private:
  CQModelColumnDetails(const CQModelColumnDetails &) = delete;
  CQModelColumnDetails &operator=(const CQModelColumnDetails &) = delete;

 private:
  using VariantInds = std::map<QVariant,int>;

  CQModelDetails* details_ { nullptr };
  int             column_  { -1 };

  // cached type data
  bool            typeInitialized_ { false };                 //!< is type data set
  CQBaseModelType type_            { CQBaseModelType::NONE }; //!< column data type

  // cached data
  bool            initialized_     { false };   //!< is data set
  QVariant        minValue_;                    //!< min value (as variant)
  QVariant        maxValue_;                    //!< max value (as variant)
  int             numRows_         { 0 };       //!< number of rows
  bool            monotonic_       { true };    //!< values are monotonic
  bool            increasing_      { true };    //!< values are increasing
  CQValueSet*     valueSet_        { nullptr }; //!< values
  VariantInds     valueInds_;                   //!< unique values

  // mutex
  mutable std::mutex mutex_; //!< mutex
};

//---

/*!
 * \brief Model Details
 */
class CQModelDetails : public QObject {
  Q_OBJECT

  Q_PROPERTY(int numColumns   READ numColumns    )
  Q_PROPERTY(int numRows      READ numRows       )
  Q_PROPERTY(int hierarchical READ isHierarchical)

 public:
  using Columns = std::vector<int>;

 public:
  CQModelDetails(QAbstractItemModel *model);

 ~CQModelDetails();

  QAbstractItemModel *model() const { return model_; }

  int numColumns() const;

  int numRows() const;

  bool isHierarchical() const;

  CQModelColumnDetails *columnDetails(int column);
  const CQModelColumnDetails *columnDetails(int column) const;

  Columns numericColumns() const;

  Columns monotonicColumns() const;

  void reset();

  std::vector<int> duplicates() const;
  std::vector<int> duplicates(int column) const;

 signals:
  void detailsReset();

 private:
  void resetValues();

  std::vector<int> columnDuplicates(int column, bool all) const;

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

  CQModelDetails(const CQModelDetails &) = delete;
  CQModelDetails &operator=(const CQModelDetails &) = delete;

 private:
  using ColumnDetails = std::map<int,CQModelColumnDetails *>;

  QAbstractItemModel* model_ { nullptr }; //!< model

  // cached data
  Initialized   initialized_  { Initialized::NONE }; //!< is initialized
  int           numColumns_   { 0 };                 //!< model number of columns
  int           numRows_      { 0 };                 //!< model number of rows
  bool          hierarchical_ { false };             //!< model is hierarchical
  ColumnDetails columnDetails_;                      //!< model column details

  // mutex
  mutable std::mutex mutex_; //!< mutex
};

#endif
