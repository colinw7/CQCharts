#ifndef CQChartsColumnType_H
#define CQChartsColumnType_H

// TODO: Add support for
//  Fill Data
//    CQChartsFillPattern
//  Stroke Data
//    CQChartsLineDash
//  Text Data
//    CQChartsFont
//  Geometry
//    CQChartsPoints ?
//    CQChartsPolygon
//    CQChartsPolygonList
//    CQChartsPosition
//    CQChartsRect
//    CQChartsStyle

#include <CQChartsColor.h>
#include <CQChartsColorStops.h>
#include <CQChartsUtil.h>
#include <CQChartsModelTypes.h>
#include <CQChartsPaletteName.h>
#include <CQChartsColorColumnData.h>
#include <CQChartsSymbolTypeData.h>
#include <CQChartsSymbolSizeData.h>
#include <CQChartsFontSizeData.h>

#include <QObject>
#include <QString>
#include <future>

class CQChartsModelColumnDetails;
class CQCharts;

/*!
 * \brief column type parameter
 * \ingroup Charts
 */
class CQChartsColumnTypeParam {
 public:
  using Type = CQBaseModelType;

 public:
  enum class Attribute {
    NONE       = 0,
    GENERIC    = (1<<0),
    NULL_VALUE = (1<<1)
  };

 public:
  CQChartsColumnTypeParam(const QString &name, Type type, int role, const QString &tip,
                          const QVariant &def=QVariant()) :
   name_(name), type_(type), role_(role), tip_(tip), def_(def) {
  }

  CQChartsColumnTypeParam(const QString &name, Type type, const QString &tip,
                          const QVariant &def=QVariant()) :
   name_(name), type_(type), role_(int(CQBaseModelRole::TypeValues)), tip_(tip), def_(def) {
  }

  const QString &name() const { return name_; }

  Type type() const { return type_; }

  int role() const { return role_; }

  const QString &tip() const { return tip_; }

  const QString &desc() const { return desc_; }
  CQChartsColumnTypeParam &setDesc(const QString &desc) { desc_ = desc; return *this; }

  const QVariant &def() const { return def_; }

  CQChartsColumnTypeParam &addValue(const QString &value) {
    values_ << value; return *this; }
  CQChartsColumnTypeParam &addValues(const QStringList &values) {
    values_ << values; return *this; }
  QStringList values() const { return values_; }

  bool isHidden() const { return hidden_; }
  CQChartsColumnTypeParam &setHidden(bool b) { hidden_ = b; return *this; }

  bool isGeneric() const { return hasAttribute(Attribute::GENERIC); }
  CQChartsColumnTypeParam &setGeneric(bool b) { return setAttribute(b, Attribute::GENERIC); }

  bool isNullValue() const { return hasAttribute(Attribute::NULL_VALUE); }
  CQChartsColumnTypeParam &setNullValue(bool b) { return setAttribute(b, Attribute::NULL_VALUE); }

 private:
  bool hasAttribute(Attribute attribute) const {
    return (attributes_ & uint(attribute));
  }

  CQChartsColumnTypeParam &setAttribute(bool b, Attribute attribute) {
    if (b) attributes_ |=  uint(attribute);
    else   attributes_ &= ~uint(attribute);
    return *this;
  }

 private:
  QString     name_;                        //!< name
  Type        type_       { Type::STRING }; //!< type
  int         role_       { -1 };           //!< model role
  QString     tip_;                         //!< tip string
  QString     desc_;                        //!< description string
  QVariant    def_;                         //!< default value
  QStringList values_;                      //!< enum values
  bool        hidden_     { false };        //!< is hidden param
  uint        attributes_ { 0 };            //!< parameter attributes
};

//---

/*!
 * \brief column type base class
 * \ingroup Charts
 *
 * Class to represent type of data present in the model column data.
 *
 * When defined can reduce the conversion of column data to type required by
 * the plot for the column.
 *
 * supports one base parameter
 *  . key - is column a key (for grouping)
 */
class CQChartsColumnType {
 public:
  using Type            = CQBaseModelType;
  using ColumnTypeParam = CQChartsColumnTypeParam;
  using Params          = std::vector<ColumnTypeParam *>;

  enum class DrawType {
    NORMAL,
    BARCHART,
    HEATMAP
  };

  using Column        = CQChartsColumn;
  using NameValues    = CQChartsNameValues;
  using ModelTypeData = CQChartsModelTypeData;
  using Color         = CQChartsColor;
  using ColorStops    = CQChartsColorStops;
  using ColumnDetails = CQChartsModelColumnDetails;

 public:
  explicit CQChartsColumnType(Type type);

  virtual ~CQChartsColumnType();

  // get type
  Type type() const { return type_; }

  // get/set index
  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  // get name
  virtual QString name() const;

  // help methods for compatibility with basic data types
  virtual bool isNumeric () const { return false; }
  virtual bool isIntegral() const { return false; }
  virtual bool isBoolean () const { return false; }
  virtual bool isTime    () const { return false; }

  // get/set hidden
  bool isHidden() const { return hidden_; }
  void setHidden(bool hidden) { hidden_ = hidden; }

  virtual QString formatName() const { return "format"; }
  virtual QString oformatName() const { return "oformat"; }

  ColumnTypeParam *addGenericParam(const QString &name, Type type, int role, const QString &tip,
                                   const QVariant &def=QVariant());
  ColumnTypeParam *addGenericParam(const QString &name, Type type, const QString &tip,
                                   const QVariant &def=QVariant());

  ColumnTypeParam *addParam(const QString &name, Type type, int role, const QString &tip,
                            const QVariant &def=QVariant());
  ColumnTypeParam *addParam(const QString &name, Type type, const QString &tip,
                            const QVariant &def=QVariant());

  const Params &params() const { return params_; }

  QStringList paramNames() const;

  bool hasParam(const QString &name) const;

  const ColumnTypeParam *getParam(const QString &name) const;

  virtual QString desc() const = 0;

  // input variant to data variant for edit
  virtual QVariant userData(CQCharts *charts, const QAbstractItemModel *model,
                            const Column &column, const QVariant &var,
                            const ModelTypeData &typeData, bool &converted) const = 0;

  // data variant to output variant (string) for display
  virtual QVariant dataName(CQCharts *charts, const QAbstractItemModel *model,
                            const Column &column, const QVariant &var,
                            const ModelTypeData &typeData, bool &converted) const = 0;

  // data min/max value
  virtual QVariant minValue(const NameValues &) const { return QVariant(); }
  virtual QVariant maxValue(const NameValues &) const { return QVariant(); }

  // get value sum
  virtual QVariant sumValue(const NameValues &) const { return QVariant(); }

  // get target value
  virtual QVariant targetValue(const NameValues &) const { return QVariant(); }

  // get decreasing
  virtual QVariant decreasing(const NameValues &) const { return QVariant(); }

  // get output format
  virtual QString getOFormat(const NameValues &) const { return QString(); }

  // get bad value
  virtual QVariant badValue(const NameValues &) const { return QVariant(); }

  // index value (TODO: assert if index invalid or not supported ?)
  virtual QVariant indexVar(const QVariant &var, const QString &) const { return var; }

  // index type (TODO: assert if index invalid or not supported ?)
  virtual Type indexType(const QString &) const { return type(); }

  ColumnDetails *columnDetails(CQCharts *charts, const QAbstractItemModel *model,
                               const Column &column) const;

  int preferredWidth(const NameValues &nameValues) const;

  QString      nullValue   (const NameValues &nameValues) const;
  QString      currentValue(const NameValues &nameValues) const;
  QVariantList extraValues (const NameValues &nameValues) const;

  Color      drawColor(const NameValues &nameValues) const;
  DrawType   drawType (const NameValues &nameValues) const;
  ColorStops drawStops(const NameValues &nameValues) const;

  QVariant remapNamedValue(CQCharts *charts, const QAbstractItemModel *model,
                           const Column &column, const QVariant &var) const;

  NameValues namedValues(const NameValues &nameValues) const;
  NameValues namedColors(const NameValues &nameValues) const;
  NameValues namedImages(CQCharts *charts, const NameValues &nameValues) const;

  bool getNameValueVariant(const NameValues &nameValues, const QString &name,
                           Type type, QVariant &value) const;
  bool setNameValueVariant(NameValues &nameValues, const QString &name,
                           Type type, const QVariant &value) const;

  bool nameValueString(const NameValues &nameValues, const QString &name, QString &value) const;

 protected:
  Type   type_;             //!< base type
  int    ind_    { -1 };    //!< insertion index
  Params params_;           //!< parameters
  bool   hidden_ { false }; //!< is type hidden

  using NameValueMap = std::map<QString, QVariant>;

  mutable NameValueMap nameValueMap_;
  mutable bool         nameValueMapSet_ { false };

  mutable std::mutex mutex_; //!< mutex
};

//---

/*!
 * \brief string column type class
 * \ingroup Charts
 */
class CQChartsColumnStringType : public CQChartsColumnType {
 public:
  CQChartsColumnStringType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;
};

//---

/*!
 * \brief boolean column type class
 * \ingroup Charts
 */
class CQChartsColumnBooleanType : public CQChartsColumnType {
 public:
  CQChartsColumnBooleanType();

  bool isBoolean() const override { return true; }

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;
};

//---

/*!
 * \brief real column type class
 * \ingroup Charts
 *
 * supports the following parameter
 *  . format       - output format
 *  . format_scale - scale factor for output format e.g. 0.001 for multiples of a thousand
 *  . min          - override calculated min value
 *  . max          - override calculated max value
 *
 * format is applied to data in model for variant to string (Qt::DisplayRole)
 */
class CQChartsColumnRealType : public CQChartsColumnType {
 public:
  CQChartsColumnRealType();

  bool isNumeric() const override { return true; }

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // get min/max value
  QVariant minValue(const NameValues &nameValues) const override;
  QVariant maxValue(const NameValues &nameValues) const override;

  // get value sum
  QVariant sumValue(const NameValues &nameValues) const override;

  // get target value
  QVariant targetValue(const NameValues &nameValues) const override;

  // get decreasing
  QVariant decreasing(const NameValues &nameValues) const override;

  // get bad value
  QVariant badValue(const NameValues &nameValues) const override;

  bool rmin(const NameValues &nameValues, double &r) const;
  bool rmax(const NameValues &nameValues, double &r) const;
  bool rsum(const NameValues &nameValues, double &r) const;

  bool rtarget(const NameValues &nameValues, double &r) const;

  QString getIFormat(const NameValues &nameValues) const;
  QString getOFormat(const NameValues &nameValues) const override;
};

//---

/*!
 * \brief integer column type class
 * \ingroup Charts
 *
 * supports the following parameter
 *  . format - output format
 *  . min    - override calculated min value
 *  . max    - override calculated max value
 *
 * format is applied to data in model for variant to string (Qt::DisplayRole)
 */
class CQChartsColumnIntegerType : public CQChartsColumnType {
 public:
  CQChartsColumnIntegerType();

  bool isNumeric () const override { return true; }
  bool isIntegral() const override { return true; }

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // get min/max value
  QVariant minValue(const NameValues &nameValues) const override;
  QVariant maxValue(const NameValues &nameValues) const override;

  // get value sum
  QVariant sumValue(const NameValues &nameValues) const override;

  bool imin(const NameValues &nameValues, long &i) const;
  bool imax(const NameValues &nameValues, long &i) const;
  bool isum(const NameValues &nameValues, long &i) const;

  QString getIFormat(const NameValues &nameValues) const;
  QString getOFormat(const NameValues &nameValues) const override;
};

//---

/*!
 * \brief time column type class
 * \ingroup Charts
 *
 * supports the following parameter
 *  . iformat - input format to convert input data to model data (time)
 *  . oformat - output (display) format to convert time to string
 *  . format  - convenience parameter when iformat and oformat are the same
 *
 * iformat/format is applied to data in model to convert from string to time
 * oformat/format is applied to time value for string conversion (Qt::DisplayRole)
 */
class CQChartsColumnTimeType : public CQChartsColumnType {
 public:
  CQChartsColumnTimeType();

  bool isNumeric() const override { return true; }
  bool isTime   () const override { return true; }

  QString formatName() const override { return "oformat"; }

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  QString getIFormat(const NameValues &nameValues) const;
  QString getOFormat(const NameValues &nameValues) const override;

  QVariant indexVar(const QVariant &var, const QString &ind) const override;

  Type indexType(const QString &) const override;
};

//---

/*!
 * \brief point column type class
 * \ingroup Charts
 */
class CQChartsColumnPointType : public CQChartsColumnType {
 public:
  CQChartsColumnPointType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  QVariant indexVar(const QVariant &var, const QString &ind) const override;

  Type indexType(const QString &) const override;
};

//---

/*!
 * \brief rect column type class
 * \ingroup Charts
 */
class CQChartsColumnRectType : public CQChartsColumnType {
 public:
  CQChartsColumnRectType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  QVariant indexVar(const QVariant &var, const QString &ind) const override;

  Type indexType(const QString &) const override;
};

//---

/*!
 * \brief length column type class
 * \ingroup Charts
 */
class CQChartsColumnLengthType : public CQChartsColumnType {
 public:
  CQChartsColumnLengthType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;
};

//---

/*!
 * \brief polygon column type class
 * \ingroup Charts
 */
class CQChartsColumnPolygonType : public CQChartsColumnType {
 public:
  CQChartsColumnPolygonType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  QVariant indexVar(const QVariant &var, const QString &ind) const override;

  Type indexType(const QString &) const override;
};

//---

/*!
 * \brief polygon list column type class
 * \ingroup Charts
 */
class CQChartsColumnPolygonListType : public CQChartsColumnType {
 public:
  CQChartsColumnPolygonListType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

 private:
  bool isPolygonListVariant(const QVariant &var) const;
};

//---

/*!
 * \brief connection list column type class
 * \ingroup Charts
 */
class CQChartsColumnConnectionListType : public CQChartsColumnType {
 public:
  CQChartsColumnConnectionListType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

 private:
  bool isVariantType(const QVariant &var) const;
};

//---

/*!
 * \brief name pair column type class
 * \ingroup Charts
 */
class CQChartsColumnNamePairType : public CQChartsColumnType {
 public:
  CQChartsColumnNamePairType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

 private:
  bool isNamePairVariant(const QVariant &var) const;
};

//---

/*!
 * \brief name pair column type class
 * \ingroup Charts
 */
class CQChartsColumnNameValuesType : public CQChartsColumnType {
 public:
  CQChartsColumnNameValuesType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  QVariant indexVar(const QVariant &var, const QString &ind) const override;

  Type indexType(const QString &) const override;

 private:
  bool isNameValuesVariant(const QVariant &var) const;
};

//---

/*!
 * \brief path column type class
 * \ingroup Charts
 */
class CQChartsColumnPathType : public CQChartsColumnType {
 public:
  CQChartsColumnPathType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;
};

//---

/*!
 * \brief style column type class
 * \ingroup Charts
 */
class CQChartsColumnStyleType : public CQChartsColumnType {
 public:
  CQChartsColumnStyleType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;
};

//---

/*!
 * \brief color column type class
 * \ingroup Charts
 *
 * supports the following parameter
 *  . mapped  - is input data mapped to color (if not input data is taken as color name)
 *  . min     - override min value for numeric value map
 *  . max     - override max value for numeric value map
 *  . palette - specific palette to lookup color in
 */
class CQChartsColumnColorType : public CQChartsColumnType {
 public:
  using ColorColumnData = CQChartsColorColumnData;
  using PaletteName     = CQChartsPaletteName;

 public:
  CQChartsColumnColorType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                  const NameValues &nameValues, ColorColumnData &colorColumnData) const;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                  const NameValues &nameValues, bool &mapped,
                  double &mapMin, double &mapMax, PaletteName &palette) const;
};

//---

/*!
 * \brief font column type class
 * \ingroup Charts
 */
class CQChartsColumnFontType : public CQChartsColumnType {
 public:
  CQChartsColumnFontType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;
};

//---

/*!
 * \brief image column type class
 * \ingroup Charts
 */
class CQChartsColumnImageType : public CQChartsColumnType {
 public:
  CQChartsColumnImageType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;
};

//---

/*!
 * \brief symbol type column type class
 * \ingroup Charts
 *
 * supports the following parameter
 *  . mapped - is input data mapped to symbol type (if not input data is taken as symbol name)
 *  . min    - override min value for numeric value map
 *  . max    - override max value for numeric value map
 */
class CQChartsColumnSymbolTypeType : public CQChartsColumnType {
 public:
  using SymbolTypeData = CQChartsSymbolTypeData;

 public:
  CQChartsColumnSymbolTypeType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                  const NameValues &nameValues, SymbolTypeData &symbolTypeData) const;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                  const NameValues &nameValues, bool &mapped,
                  long &mapMin, long &mapMax, long &dataMin, long &dataMax) const;
};

//---

/*!
 * \brief symbol size column type class
 * \ingroup Charts
 *
 * supports the following parameter
 *  . mapped   - is input data mapped to symbol size (if not input data is taken as symbol size)
 *  . min      - override input min value for numeric value map
 *  . max      - override input max value for numeric value map
 *  . size_min - override output min value for numeric value map
 *  . size_max - override output max value for numeric value map
 */
class CQChartsColumnSymbolSizeType : public CQChartsColumnType {
 public:
  using SymbolSizeData = CQChartsSymbolSizeData;

 public:
  CQChartsColumnSymbolSizeType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                  const NameValues &nameValues, SymbolSizeData &symbolSizeData) const;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                  const NameValues &nameValues, bool &mapped,
                  double &mapMin, double &mapMax, double &dataMin, double &dataMax) const;
};

//---

/*!
 * \brief font size column type class
 * \ingroup Charts
 *
 * supports the following parameter
 *  . mapped   - is input data mapped to font size (if not input data is taken as font size)
 *  . min      - override input min value for numeric value map
 *  . max      - override input max value for numeric value map
 *  . size_min - override output min value for numeric value map
 *  . size_max - override output max value for numeric value map
 */
class CQChartsColumnFontSizeType : public CQChartsColumnType {
 public:
  using FontSizeData = CQChartsFontSizeData;

 public:
  CQChartsColumnFontSizeType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                    const QVariant &var, const ModelTypeData &typeData,
                    bool &converted) const override;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                  const NameValues &nameValues, FontSizeData &fontSizeData) const;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                  const NameValues &nameValues, bool &mapped,
                  double &mapMin, double &mapMax, double &dataMin, double &dataMax) const;
};

//---

/*!
 * \brief column type manager
 * \ingroup Charts
 */
class CQChartsColumnTypeMgr : public QObject {
  Q_OBJECT

 public:
  using Column        = CQChartsColumn;
  using ColumnType    = CQChartsColumnType;
  using NameValues    = CQChartsNameValues;
  using ModelTypeData = CQChartsModelTypeData;
  using Type          = CQBaseModelType;

 public:
  static QString description();

 public:
  CQChartsColumnTypeMgr(CQCharts *charts);
 ~CQChartsColumnTypeMgr();

  void typeNames(QStringList &names, bool hidden=false) const;

  template<typename T>
  void addTypeT(Type type) {
    addType(type, new T);
  }

  void addType(Type type, ColumnType *data);

#if 0
  const ColumnType *decodeTypeData(const QString &type, NameValues &nameValues) const;

  QString encodeTypeData(Type type, const NameValues &nameValues) const;
#endif

  const ColumnType *getType(Type type) const;

  template<typename T>
  const T *getTypeT(Type type) const {
    auto *t = dynamic_cast<const T *>(getType(type));
    assert(t);
    return t;
  }

  const ColumnType *getNamedType(const QString &name) const;

  QVariant getUserData(const QAbstractItemModel *model, const Column &column,
                       const QVariant &var, bool &converted) const;

  QVariant getDisplayData(const QAbstractItemModel *model, const Column &column,
                          const QVariant &var, bool &converted) const;

  QVariant getHeaderUserData(const QAbstractItemModel *model, int section,
                             const QVariant &var, bool &converted) const;

  bool getModelColumnType(const QAbstractItemModel *model, const Column &column,
                          ModelTypeData &columnTypeData) const;

  bool setModelColumnType(QAbstractItemModel *model, const Column &column, Type type,
                          const NameValues &nameValues=NameValues());

  bool setModelHeaderType(QAbstractItemModel *model, const Column &column, Type type,
                          const NameValues &nameValues=NameValues());

  void startCache(const QAbstractItemModel *model);
  void endCache  (const QAbstractItemModel *model);

 private:
  using TypeData = std::map<Type, ColumnType*>;

  struct TypeCacheData {
    ModelTypeData     typeData;
    const ColumnType* columnType       { nullptr };
    bool              valid            { false };
    const ColumnType* headerColumnType { nullptr };
    bool              headerValid      { false };
  };

  using ColumnTypeCache = std::map<Column, TypeCacheData>;

  struct CacheData {
    ColumnTypeCache columnTypeCache;
    int             depth { 0 };
    TypeCacheData   typeCacheData;
  };

  using ModelCacheData = std::map<int, CacheData>;
  using CacheDataStack = std::vector<CacheData>;

 private:
  bool getModelColumnTypeData(const QAbstractItemModel *model, const Column &column,
                              const TypeCacheData* &typeCacheData) const;

  const CacheData &getModelCacheData(const QAbstractItemModel *model, bool &ok) const;

 private:
  CQCharts*          charts_         { nullptr }; //!< charts
  TypeData           typeData_;                   //!< type data
  ModelCacheData     modelCacheData_;              //!< column type cache (per model)
  CacheDataStack     cacheDataStack_;             //!< cache data stack
  mutable std::mutex mutex_;                      //!< mutex
};

//----

class CQChartsColumnTypeId :
  public CQChartsComparatorBase<CQChartsColumnTypeId> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsColumnTypeId, metaTypeId)

 public:
  CQChartsColumnTypeId() = default;

  explicit CQChartsColumnTypeId(CQBaseModelType type); // data

  explicit CQChartsColumnTypeId(const QString &s); // parsed

  //--

  CQBaseModelType type() const { return type_; }

  //--

  QString toString() const;

  bool fromString(const QString &s);

  //---

  int cmp(const CQChartsColumnTypeId &c) const {
    if (type_ < c.type_) return -1;
    if (type_ > c.type_) return  1;
    return 0;
  }

  friend int cmp(const CQChartsColumnTypeId &c1, const CQChartsColumnTypeId &c2) {
    return c1.cmp(c2);
  }

  //---

 private:
  CQBaseModelType type_ { CQBaseModelType::STRING };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsColumnTypeId)

#endif
