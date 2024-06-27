#ifndef CQChartsModelDetails_H
#define CQChartsModelDetails_H

#include <CQChartsColumn.h>
#include <CQChartsColumnType.h>
#include <CQChartsModelTypes.h>
#include <CQChartsUtil.h>
#include <CQBucketer.h>

#include <future>
#include <optional>

class CQChartsModelColumnDetails;
class CQChartsModelData;
class CQCharts;
class CQChartsValueSet;

class QAbstractItemModel;

/*!
 * \brief Model Details
 * \ingroup Charts
 *
 * Cached details of model dimensions, hierarchical and per column data.
 */
class CQChartsModelDetails : public QObject {
  Q_OBJECT

  Q_PROPERTY(int numColumns   READ numColumns    )
  Q_PROPERTY(int numRows      READ numRows       )
  Q_PROPERTY(int hierarchical READ isHierarchical)

  Q_PROPERTY(int numBuckets READ numBuckets WRITE setNumBuckets)

 public:
  using ModelData     = CQChartsModelData;
  using Column        = CQChartsColumn;
  using Columns       = CQChartsColumns;
  using ColumnDetails = CQChartsModelColumnDetails;

 public:
  CQChartsModelDetails(ModelData *data);

 ~CQChartsModelDetails();

  ModelData *data() const { return data_; }

  //! get num columns/rows
  int numColumns() const;
  int numRows   () const;

  //! get is hierarchical
  bool isHierarchical() const;

  //! get/set number of preferred buckets
  int numBuckets() const { return numBuckets_; }
  void setNumBuckets(int n);

  //! get details for column
  ColumnDetails *columnDetails(const Column &column);
  const ColumnDetails *columnDetails(const Column &column) const;

  //! numeric columns (from column types)
  Columns numericColumns() const;

  //! monotonic columns (from column types)
  Columns monotonicColumns() const;

  //! reset cache
  void reset();

  //! get rows with duplicate values (all columns or per column)
  std::vector<int> duplicates() const;
  std::vector<int> duplicates(const Column &column) const;

  //! get correlation between two columns
  double correlation(const Column &column1, const Column &column2) const;

  //! get associated charts
  CQCharts *charts() const;

  //! get associated model
  QAbstractItemModel *model() const;

 Q_SIGNALS:
  //! emitted when data reset
  void detailsReset();

 private Q_SLOTS:
  //! handle model type change (if matches this model)
  void modelTypeChangedSlot(int modelInd);

 private:
  //! reset cached data
  void resetValues();

  //! get rows with duplicate values (all columns or per column)
  std::vector<int> columnDuplicates(const Column &column, bool all) const;

  //! update simple data (dimensions, hierarchical)
  void updateSimple();
//void updateFull();

  //! ensure simple data is initialized (locked update)
  void initSimpleData() const;
//void initFullData() const;

 private:
  enum class Initialized {
    NONE,
    SIMPLE,
    FULL
  };

  // disable copy
  CQChartsModelDetails(const CQChartsModelDetails &) = delete;
  CQChartsModelDetails &operator=(const CQChartsModelDetails &) = delete;

 private:
  using ColumnDetailsMap = std::map<Column, ColumnDetails *>;

  ModelData* data_ { nullptr }; //!< model data

  int numBuckets_ { 20 };

  // cached data (simple and columns)
  Initialized      initialized_  { Initialized::NONE }; //!< is initialized
  int              numColumns_   { 0 };                 //!< model number of columns
  int              numRows_      { 0 };                 //!< model number of rows
  bool             hierarchical_ { false };             //!< model is hierarchical
  ColumnDetailsMap columnDetails_;                      //!< model column details

  // mutex for update (simple and columns)
  mutable std::mutex initMutex_;   //!< mutex for init
  mutable std::mutex columnMutex_; //!< mutex for column details
};

//---

/*!
 * \brief Model Column Details
 * \ingroup Charts
 *
 * Cached details on column type and value data
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
  using OptAlign      = std::optional<Qt::Alignment>;

 public:
  CQChartsModelColumnDetails(Details *details, const Column &column);

  virtual ~CQChartsModelColumnDetails();

  //! get details data
  Details *details() const { return details_; }

  //! get column
  const Column &column() const { return column_; }

  //! get header name
  QString headerName() const;

  //! get is key (should be unique)
  bool isKey() const;

  //! get type name
  QString typeName() const;

  //! get type data
  const ModelTypeData &typeData() const;

  //! get type enum
  ColumnType type() const;
  void setType(ColumnType type);

  //! get base type enum
  ColumnType baseType() const;
  void setBaseType(ColumnType type);

  //---

  //! get associated named values (extra data)
  const NameValues &nameValues() const;

  //! get long names of supported name values
  static const QStringList &getLongNamedValues();
  //! get short names of supported name values
  static const QStringList &getShortNamedValues();

  //! get if name is named value (long or short)
  static bool isNamedValue(const QString &name);

  //! get named value (from name value data)
  QVariant getNamedValue(const QString &name) const;

  //! get min/max value
  QVariant minValue() const;
  QVariant maxValue() const;

  //! get mean, sum, std deviation
  QVariant meanValue  (bool useNaN=true) const;
  QVariant sumValue   (bool useNaN=true) const;
  QVariant targetValue(bool useNaN=true) const;
  QVariant stdDevValue(bool useNaN=true) const;

  //! get bad value
  QVariant badValue() const;

  //! get decreasing
  QVariant decreasing() const;

  //! get output format
  QVariant oformat() const;

  //---

  //! get column data value from variant
  QVariant dataName(const QVariant &v) const;

  //---

  //! get number of column rows (always same as model rows ?)
  int numRows() const;

  //! get is numeric values
  bool isNumeric() const;

  //! get is monotonic values
  bool isMonotonic () const;
  //! get is sorted into increasing values
  bool isIncreasing() const;

  //---

  //! number of unique values
  int numUnique() const;
  //! max number of unique values
  int maxUnique() const;

  //! unique values
  QVariantList uniqueValues() const;
  //! count per unique value
  QVariantList uniqueCounts() const;

  //! unique values and counts
  ValueCounts uniqueValueCounts() const;

  //! unique index for value
  int uniqueId(const QVariant &v) const;

  //! unique value for index
  QVariant uniqueValue(int i) const;

  //---

  //! get/set number of preferred buckets
  int numBuckets() const;
  void setNumBuckets(int n);

  //! bucket for value
  int bucket(const QVariant &v) const;

  //! range for bucket index
  void bucketRange(int i, QVariant &vmin, QVariant &vmax) const;

  //---

  //! number of null values
  int numNull() const;

  //! number of unique values
  int numValues() const { return int(valueInds_.size()); }

  //! unique value index
  int valueInd(const QVariant &value) const;

  //---

  //! median value
  QVariant medianValue     (bool useNaN=true) const;
  //! lower median value
  QVariant lowerMedianValue(bool useNaN=true) const;
  //! upper median value
  QVariant upperMedianValue(bool useNaN=true) const;

  //! outlier values
  QVariantList outlierValues() const;

  //! is outlier value
  bool isOutlier(const QVariant &value) const;

  //---

  //! map value to range (0.0, 1.0)
  double map(const QVariant &var) const;

  //---

  //! number of values
  uint valueCount() const;

  //! index value
  QVariant value(uint i) const;

  //! value set
  ValueSet *valueSet() const { return valueSet_; }

  //---

  // cached column type data

  // get cached "preferred_width" column type value
  int preferredWidth() const { return preferredWidth_; }
  //void setPreferredWidth(int w) { preferredWidth_ = w; }

  // get cached "alignment" column type value
  OptAlign alignment() const { return alignment_; }
  //void setAlignment(const OptAlign &a) { alignment_ = a; }

  // get cached "null_value" column type value
  const QString &nullValue() const;
  //void setNullValue(const QString &v) { nullValue_ = v; }

  // get cached "current_value" column type value
  const QString &currentValue() const;
  //void setcurrentValue(const QString &v) { currentValue_ = v; }

  // get cached "extra_values" column type value
  const QVariantList &extraValues() const;
  //void setExtraValues(const QString &v) { extraValues_ = v; }

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

  //! get column named value (from column type data)
  bool columnNameValue(const QString &name, QString &value) const;

  //---

  //! check if value should be included in details
  virtual bool checkRow(const QVariant &) { return true; }

  //---

  //! reset type data
  void resetTypeInitialized();

  //! init cache data
  void initCache() const;

  //! init bucketer data
  void initBucketer(bool force=false) const;

  //! get column type
  const CQChartsColumnType *columnType() const;

  //! get column type (cast to T)
  template<typename T>
  const T *columnTypeT() const { return dynamic_cast<const T *>(columnType()); }

  //---

  //! get heatmap color for value (from draw color and draw stops) (blended with background)
  //! Defaults to red draw color (no stops)
  QColor heatmapColor(double r, double min, double max, const QColor &bgColor) const;

  //! get bar color
  QColor barchartColor() const;

#if 0
  //! get mapped name value
  bool namedValue(const QString &name, QVariant &value) const;
#endif
  //! get mapped named color
  bool namedColor(const QString &name, Color &color) const;
  //! get mapped named image
  bool namedImage(const QString &name, Image &image) const;

 private:
  //! calc value set (init if needed)
  ValueSet *calcValueSet() const;

  //! init cache (after lock acquired)
  void initCache1() const;
  //! calc cache (after lock acquired)
  bool calcCache ();

  //! calc bucketer (after lock acquired)
  void calcBucketer();

  //! init type data
  bool initType () const;
  //! init type data (after lock acquired)
  bool initType1() const;
  //! calc type data (after lock acquired)
  bool calcType ();

  //! add integer to values
  void addInt   (long i, bool ok);
  //! add real to values
  void addReal  (double r, bool ok);
  //! add string to values
  void addString(const QString &s);
  //! add time to values
  void addTime  (double t, bool ok);
  //! add color to values
  void addColor (const Color &c, bool ok);

  //! add variant to values
  void addValue(const QVariant &value);

  //! get color for column value
  bool columnColor(const QVariant &var, Color &color) const;

  //! get column type data
  const CQChartsColumnType *columnTypeI() const;

 private:
  //! disable copy
  CQChartsModelColumnDetails(const CQChartsModelColumnDetails &) = delete;
  CQChartsModelColumnDetails &operator=(const CQChartsModelColumnDetails &) = delete;

 private:
  using VariantInds = std::map<QVariant, int>;

  Details* details_ { nullptr }; //!< details
  Column   column_;              //!< column

  // cached type data
  bool          typeInitialized_ { false }; //!< is type data set
  ModelTypeData typeData_;                  //!< column data type
  QString       typeName_;                  //!< type name

  // cached data
  bool     initialized_ { false }; //!< is data set
  QVariant minValue_;              //!< min value (as variant)
  QVariant maxValue_;              //!< max value (as variant)
  QVariant sumValue_;              //!< value sum (as variant)
  QVariant targetValue_;           //!< target value (as variant)
  QVariant badValue_;              //!< bad value (as variant)
  QVariant decreasing_;            //!< is decreasing sense (as variant)
  QVariant oformat_;               //!< output format (as variant)
  int      numRows_     { 0 };     //!< number of rows
  bool     monotonic_   { true };  //!< values are monotonic
  bool     increasing_  { true };  //!< values are increasing

  // values
  ValueSet*   valueSet_ { nullptr }; //!< values
  VariantInds valueInds_;            //!< unique values

  // buckets
  using BucketerP = std::unique_ptr<CQBucketer>;

  BucketerP bucketer_;          //!< bucketed values
  int       numBuckets_ { -1 }; //!< number of buckets

  // cached parameter values
  int preferredWidth_ { -1 }; //!< preferred column width

  OptAlign alignment_;

  QString      nullValue_;    //!< null value
  QString      currentValue_; //!< null value
  QVariantList extraValues_;  //!< extra values

  Color         tableDrawColor_;                             //!< table draw color
  TableDrawType tableDrawType_   { TableDrawType::HEATMAP }; //!< table draw type
  ColorStops    tableDrawStops_;                             //!< table draw stops

#if 0
  NameValues namedValues_; //! column type named values
  NameValues namedColors_; //! column type named colors
  NameValues namedImages_; //! column type named images
#endif

  // mutex
  mutable std::mutex        initMutex_;        //!< init mutex
  mutable std::atomic<bool> initializing_ { }; //!< initializing

  mutable std::mutex        bucketMutex_;      //!< bucket mutex
  mutable std::atomic<bool> bucketing_    { }; //!< bucketing
};

#endif
