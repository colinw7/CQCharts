#ifndef CQChartsModelDetails_H
#define CQChartsModelDetails_H

#include <CQChartsColumn.h>
#include <CQChartsColumnType.h>
#include <CQChartsModelTypes.h>
#include <CQChartsUtil.h>
#include <CQBucketer.h>
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
  using ModelData     = CQChartsModelData;
  using Column        = CQChartsColumn;
  using Columns       = CQChartsColumns;
  using ColumnDetails = CQChartsModelColumnDetails;

 public:
  CQChartsModelDetails(ModelData *data);

 ~CQChartsModelDetails();

  ModelData *data() const { return data_; }

  int numColumns() const;
  int numRows   () const;

  bool isHierarchical() const;

  ColumnDetails *columnDetails(const Column &column);
  const ColumnDetails *columnDetails(const Column &column) const;

  Columns numericColumns() const;

  Columns monotonicColumns() const;

  void reset();

  std::vector<int> duplicates() const;
  std::vector<int> duplicates(const Column &column) const;

  double correlation(const Column &column1, const Column &column2) const;

  CQCharts *charts() const;

  QAbstractItemModel *model() const;

 Q_SIGNALS:
  void detailsReset();

 private Q_SLOTS:
  void modelTypeChangedSlot(int modelInd);

 private:
  void resetValues();

  std::vector<int> columnDuplicates(const Column &column, bool all) const;

  void updateSimple();
  //void updateFull();

  void initSimpleData() const;
  //void initFullData() const;

 private:
  enum class Initialized {
    NONE,
    SIMPLE,
    FULL
  };

  CQChartsModelDetails(const CQChartsModelDetails &) = delete;
  CQChartsModelDetails &operator=(const CQChartsModelDetails &) = delete;

 private:
  using ColumnDetailsMap = std::map<Column, ColumnDetails *>;

  ModelData* data_ { nullptr }; //!< model data

  // cached data
  Initialized      initialized_  { Initialized::NONE }; //!< is initialized
  int              numColumns_   { 0 };                 //!< model number of columns
  int              numRows_      { 0 };                 //!< model number of rows
  bool             hierarchical_ { false };             //!< model is hierarchical
  ColumnDetailsMap columnDetails_;                      //!< model column details

  // mutex
  mutable std::mutex initMutex_;   //!< mutex for init
  mutable std::mutex columnMutex_; //!< mutex for column details
};

//---

/*!
 * \brief Model Column Details
 * \ingroup Charts
 */
class CQChartsModelColumnDetails {
 public:
  using Details       = CQChartsModelDetails;
  using ModelTypeData = CQChartsModelTypeData;
  using Column        = CQChartsColumn;
  using ColumnType    = CQBaseModelType;
  using TableDrawType = CQChartsColumnType::DrawType;
  using ValueCount    = std::pair<QVariant, int>;
  using ValueCounts   = std::vector<ValueCount>;
  using NameValues    = CQChartsNameValues;
  using ValueSet      = CQChartsValueSet;
  using ColorStops    = CQChartsColorStops;
  using Color         = CQChartsColor;
  using Image         = CQChartsImage;

 public:
  CQChartsModelColumnDetails(Details *details, const Column &column);

  virtual ~CQChartsModelColumnDetails();

  Details *details() const { return details_; }

  const Column &column() const { return column_; }

  QString headerName() const;

  bool isKey() const;

  QString typeName() const;

  const ModelTypeData &typeData() const;

  ColumnType type() const;
  void setType(ColumnType type);

  ColumnType baseType() const;
  void setBaseType(ColumnType type);

  const NameValues &nameValues() const;

  static const QStringList &getLongNamedValues();
  static const QStringList &getShortNamedValues();

  static bool isNamedValue(const QString &name);

  QVariant getNamedValue(const QString &name) const;

  QVariant minValue() const;
  QVariant maxValue() const;

  QVariant meanValue  (bool useNaN=true) const;
  QVariant sumValue   (bool useNaN=true) const;
  QVariant stdDevValue(bool useNaN=true) const;

  QVariant badValue() const;

  QVariant dataName(const QVariant &v) const;

  int numRows() const;

  bool isNumeric() const;

  bool isMonotonic () const;
  bool isIncreasing() const;

  //---

  int numUnique() const;

  QVariantList uniqueValues() const;
  QVariantList uniqueCounts() const;

  ValueCounts uniqueValueCounts() const;

  int uniqueId(const QVariant &v) const;

  QVariant uniqueValue(int i) const;

  //---

  int numBuckets() const;

  int bucket(const QVariant &v) const;

  void bucketRange(int i, QVariant &vmin, QVariant &vmax) const;

  //---

  int numNull() const;

  int numValues() const { return int(valueInds_.size()); }

  int valueInd(const QVariant &value) const;

  QVariant medianValue     (bool useNaN=true) const;
  QVariant lowerMedianValue(bool useNaN=true) const;
  QVariant upperMedianValue(bool useNaN=true) const;

  QVariantList outlierValues() const;

  bool isOutlier(const QVariant &value) const;

  double map(const QVariant &var) const;

  //---

  uint valueCount() const;

  QVariant value(uint i) const;

  //---

  ValueSet *valueSet() const { return valueSet_; }

  //---

  // cached column type data

  // get cahced "preferred_width" column type value
  int preferredWidth() const { return preferredWidth_; }
  //void setPreferredWidth(int w) { preferredWidth_ = w; }

  // get cached "null_value" column type value
  const QString &nullValue() const { return nullValue_; }
  //void setNullValue(const QString &v) { nullValue_ = v; }

  // get cached "draw_color" column type value
  const Color &tableDrawColor() const { return tableDrawColor_; }
  //void setTableDrawColor(const Color &c) { tableDrawColor_ = c; }

  // get cached "draw_type" column type value
  const TableDrawType &tableDrawType() const { return tableDrawType_; }
  //void setTableDrawType(const TableDrawType &t) { tableDrawType_ = t; }

  // get cached "draw_stops" column type value
  const ColorStops &tableDrawStops() const { return tableDrawStops_; }
  //void setTableDrawType(const ColorStops &s) { tableDrawStops_ = s; }

//const NameValues &namedValues() const { return namedValues_; }
//void setNamedValues(const NameValues &s) { namedValues_ = s; }

//const NameValues &namedColors() const { return namedColors_; }
//void setNamedColors(const NameValues &s) { namedColors_ = s; }

//const NameValues &namedImages() const { return namedImages_; }
//void setNamedImages(const NameValues &s) { namedImages_ = s; }

  bool columnNameValue(const QString &name, QString &value) const;

  //---

  virtual bool checkRow(const QVariant &) { return true; }

  void resetTypeInitialized();

  void initCache() const;

  void initBucketer() const;

  const CQChartsColumnType *columnType() const;

  template<typename T>
  const T *columnTypeT() const { return dynamic_cast<const T *>(columnType()); }

  //---

  QColor heatmapColor(double r, double min, double max, const QColor &bgColor) const;

  QColor barchartColor() const;

#if 0
  bool namedValue(const QString &name, QVariant &value) const;
#endif
  bool namedColor(const QString &name, Color &color) const;
  bool namedImage(const QString &name, Image &image) const;

 private:
  ValueSet *calcValueSet() const;

  void initCache1() const;
  bool calcCache ();

  void calcBucketer();

  bool initType () const;
  bool initType1() const;
  bool calcType ();

  void addInt   (long i, bool ok);
  void addReal  (double r, bool ok);
  void addString(const QString &s);
  void addTime  (double t, bool ok);
  void addColor (const Color &c, bool ok);

  void addValue(const QVariant &value);

  bool columnColor(const QVariant &var, Color &color) const;

 private:
  CQChartsModelColumnDetails(const CQChartsModelColumnDetails &) = delete;
  CQChartsModelColumnDetails &operator=(const CQChartsModelColumnDetails &) = delete;

 private:
  using VariantInds = std::map<QVariant, int>;

  Details* details_ { nullptr };
  Column   column_;

  // cached type data
  bool          typeInitialized_ { false }; //!< is type data set
  ModelTypeData typeData_;                  //!< column data type
  QString       typeName_;                  //!< type name

  // cached data
  bool        initialized_ { false };   //!< is data set
  QVariant    minValue_;                //!< min value (as variant)
  QVariant    maxValue_;                //!< max value (as variant)
  QVariant    sumValue_;                //!< value sum (as variant)
  QVariant    badValue_;                //!< bad value (as variant)
  int         numRows_     { 0 };       //!< number of rows
  bool        monotonic_   { true };    //!< values are monotonic
  bool        increasing_  { true };    //!< values are increasing
  ValueSet*   valueSet_    { nullptr }; //!< values
  VariantInds valueInds_;               //!< unique values
  CQBucketer* bucketer_    { nullptr }; //!< bucketed values

  // cached parameter values
  int           preferredWidth_  { -1 };
  QString       nullValue_;
  Color         tableDrawColor_;
  TableDrawType tableDrawType_   { TableDrawType::HEATMAP };
  ColorStops    tableDrawStops_;
//NameValues    namedValues_;
//NameValues    namedColors_;
//NameValues    namedImages_;

  // mutex
  mutable std::mutex        initMutex_;        //!< init mutex
  mutable std::atomic<bool> initializing_ { }; //!< initializing

  mutable std::mutex        bucketMutex_;      //!< bucket mutex
  mutable std::atomic<bool> bucketing_    { }; //!< bucketing
};

#endif
