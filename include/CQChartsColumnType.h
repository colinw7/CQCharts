#ifndef CQChartsColumnType_H
#define CQChartsColumnType_H

#include <CQChartsColor.h>
#include <CQChartsColorStops.h>
#include <CQChartsUtil.h>
#include <CQBaseModelTypes.h>
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

 private:
  QString     name_;                             //!< name
  Type        type_ { CQBaseModelType::STRING }; //!< type
  int         role_ { -1 };                      //!< model role
  QString     tip_;                              //!< tip string
  QString     desc_;                             //!< description string
  QVariant    def_;                              //!< default value
  QStringList values_;                           //!< enum values
};

//---

/*!
 * \brief column type base class
 * \ingroup Charts
 *
 * supports one base parameter
 *  . key - is column a key (for grouping)
 */
class CQChartsColumnType {
 public:
  using Type   = CQBaseModelType;
  using Params = std::vector<CQChartsColumnTypeParam *>;

  enum class DrawType {
    NORMAL,
    BARCHART,
    HEATMAP
  };

 public:
  CQChartsColumnType(Type type);

  virtual ~CQChartsColumnType();

  Type type() const { return type_; }

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  virtual QString name() const;

  virtual bool isNumeric () const { return false; }
  virtual bool isIntegral() const { return false; }
  virtual bool isBoolean () const { return false; }
  virtual bool isTime    () const { return false; }

  virtual QString formatName() const { return "format"; }

  CQChartsColumnTypeParam *addParam(const QString &name, Type type, int role, const QString &tip,
                                    const QVariant &def=QVariant());
  CQChartsColumnTypeParam *addParam(const QString &name, Type type, const QString &tip,
                                    const QVariant &def=QVariant());

  const Params &params() const { return params_; }

  bool hasParam(const QString &name) const;

  const CQChartsColumnTypeParam *getParam(const QString &name) const;

  virtual QString desc() const = 0;

  // input variant to data variant for edit
  virtual QVariant userData(CQCharts *charts, const QAbstractItemModel *model,
                            const CQChartsColumn &column, const QVariant &var,
                            const CQChartsNameValues &nameValues, bool &converted) const = 0;

  // data variant to output variant (string) for display
  virtual QVariant dataName(CQCharts *chart, const QAbstractItemModel *model,
                            const CQChartsColumn &column, const QVariant &var,
                            const CQChartsNameValues &nameValues, bool &converted) const = 0;

  // data min/max value
  virtual QVariant minValue(const CQChartsNameValues &) const { return QVariant(); }
  virtual QVariant maxValue(const CQChartsNameValues &) const { return QVariant(); }

  // index value (TODO: assert if index invalid or not supported ?)
  virtual QVariant indexVar(const QVariant &var, const QString &) const { return var; }

  // index type (TODO: assert if index invalid or not supported ?)
  virtual Type indexType(const QString &) const { return type(); }

  CQChartsModelColumnDetails *columnDetails(CQCharts *charts, const QAbstractItemModel *model,
                                            const CQChartsColumn &column) const;

  CQChartsColor      drawColor(const CQChartsNameValues &nameValues) const;
  DrawType           drawType (const CQChartsNameValues &nameValues) const;
  CQChartsColorStops drawStops(const CQChartsNameValues &nameValues) const;

  bool nameValueString(const CQChartsNameValues &nameValues,
                       const QString &name, QString &value) const;

 protected:
  Type   type_;       //!< base type
  int    ind_ { -1 }; //!< insertion index
  Params params_;     //!< parameters
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
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
 */
class CQChartsColumnRealType : public CQChartsColumnType {
 public:
  CQChartsColumnRealType();

  bool isNumeric() const override { return true; }

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // get min/max value
  QVariant minValue(const CQChartsNameValues &nameValues) const override;
  QVariant maxValue(const CQChartsNameValues &nameValues) const override;

  bool rmin(const CQChartsNameValues &nameValues, double &r) const;
  bool rmax(const CQChartsNameValues &nameValues, double &r) const;
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
 */
class CQChartsColumnIntegerType : public CQChartsColumnType {
 public:
  CQChartsColumnIntegerType();

  bool isNumeric () const override { return true; }
  bool isIntegral() const override { return true; }

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // get min/max value
  QVariant minValue(const CQChartsNameValues &nameValues) const override;
  QVariant maxValue(const CQChartsNameValues &nameValues) const override;

  bool imin(const CQChartsNameValues &nameValues, int &i) const;
  bool imax(const CQChartsNameValues &nameValues, int &i) const;
};

//---

/*!
 * \brief time column type class
 * \ingroup Charts
 *
 * supports the following parameter
 *  . iformat - input format to convert input data to model data (time)
 *  . oformat - output (display) format to convert time to string
 *  - format  - convenience parameter when iformat and oformat are the same
 */
class CQChartsColumnTimeType : public CQChartsColumnType {
 public:
  CQChartsColumnTimeType();

  bool isNumeric() const override { return true; }
  bool isTime   () const override { return true; }

  QString formatName() const override { return "oformat"; }

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  QString getIFormat(const CQChartsNameValues &nameValues) const;
  QString getOFormat(const CQChartsNameValues &nameValues) const;

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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

 private:
  bool isNamePairVariant(const QVariant &var) const;
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
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
  CQChartsColumnColorType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                  const CQChartsNameValues &nameValues, bool &mapped,
                  double &map_min, double &map_max, QString &palette) const;
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
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
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
  CQChartsColumnSymbolTypeType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                  const CQChartsNameValues &nameValues, bool &mapped,
                  int &map_min, int &map_max, int &data_min, int &data_max) const;
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
  CQChartsColumnSymbolSizeType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                  const CQChartsNameValues &nameValues, bool &mapped,
                  double &map_min, double &map_max, double &data_min, double &data_max) const;
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
  CQChartsColumnFontSizeType();

  QString desc() const override;

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  bool getMapData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                  const CQChartsNameValues &nameValues, bool &mapped,
                  double &map_min, double &map_max, double &data_min, double &data_max) const;
};

//---

/*!
 * \brief column type manager
 * \ingroup Charts
 */
class CQChartsColumnTypeMgr : public QObject {
  Q_OBJECT

 public:
  using Type = CQBaseModelType;

 public:
  static QString description();

 public:
  CQChartsColumnTypeMgr(CQCharts *charts);
 ~CQChartsColumnTypeMgr();

  void typeNames(QStringList &names) const;

  void addType(Type type, CQChartsColumnType *data);

#if 0
  const CQChartsColumnType *decodeTypeData(const QString &type,
                                           CQChartsNameValues &nameValues) const;

  QString encodeTypeData(Type type, const CQChartsNameValues &nameValues) const;
#endif

  const CQChartsColumnType *getType(Type type) const;

  const CQChartsColumnType *getNamedType(const QString &name) const;

  QVariant getUserData(const QAbstractItemModel *model, const CQChartsColumn &column,
                       const QVariant &var, bool &converted) const;

  QVariant getDisplayData(const QAbstractItemModel *model, const CQChartsColumn &column,
                          const QVariant &var, bool &converted) const;

  bool getModelColumnType(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Type &type, Type &baseType, CQChartsNameValues &nameValues) const;

  bool setModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column, Type type,
                          const CQChartsNameValues &nameValues=CQChartsNameValues());

  void startCache(const QAbstractItemModel *model);
  void endCache  (const QAbstractItemModel *model);

 private:
  using TypeData = std::map<Type,CQChartsColumnType*>;

  struct TypeCacheData {
    Type                      type     { Type::NONE };
    Type                      baseType { Type::NONE };
    CQChartsNameValues        nameValues;
    const CQChartsColumnType* typeData { nullptr };
    bool                      valid    { false };
  };

  using ColumnTypeCache = std::map<CQChartsColumn,TypeCacheData>;

  struct CacheData {
    ColumnTypeCache columnTypeCache;
    int             depth { 0 };
    TypeCacheData   typeCacheData;
  };

  using ModelCacheData = std::map<int,CacheData>;

 private:
  bool getModelColumnTypeData(const QAbstractItemModel *model, const CQChartsColumn &column,
                              const TypeCacheData* &typeCacheData) const;

  const CacheData &getModelCacheData(const QAbstractItemModel *model, bool &ok) const;

 private:
  CQCharts*          charts_     { nullptr }; //!< charts
  TypeData           typeData_;               //!< type data
  ModelCacheData     modelCacheData_;         //!< column type cache (per model)
  mutable std::mutex mutex_;
};

#endif
