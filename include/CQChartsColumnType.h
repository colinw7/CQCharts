#ifndef CQChartsColumnType_H
#define CQChartsColumnType_H

#include <CQChartsUtil.h>
#include <CQBaseModelTypes.h>
#include <QObject>
#include <QString>

class CQChartsModelColumnDetails;
class CQCharts;

// column type parameter
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

  const QVariant &def() const { return def_; }

 private:
  QString  name_;
  Type     type_ { CQBaseModelType::STRING };
  int      role_ { -1 };
  QString  tip_;
  QVariant def_;
};

//---

// column type base class
//
// supports one base parameter
//  . key - is column a key (for grouping)
class CQChartsColumnType {
 public:
  using Type   = CQBaseModelType;
  using Params = std::vector<CQChartsColumnTypeParam>;

 public:
  CQChartsColumnType(Type type);

  virtual ~CQChartsColumnType() { }

  Type type() const { return type_; }

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  virtual QString name() const;

  virtual bool isNumeric() const { return false; }

  const Params &params() const { return params_; }

  bool hasParam(const QString &name) const;

  const CQChartsColumnTypeParam *getParam(const QString &name) const;

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

 protected:
  Type   type_;       // base type
  int    ind_ { -1 }; // insertion index
  Params params_;     // parameters
};

//---

// string column type class
class CQChartsColumnStringType : public CQChartsColumnType {
 public:
  CQChartsColumnStringType();

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

// boolean column type class
class CQChartsColumnBooleanType : public CQChartsColumnType {
 public:
  CQChartsColumnBooleanType();

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

// real column type class
//
// supports the following parameter
//  . format       - output format
//  . format_scale - scale factor for output format e.g. 0.001 for multiples of a thousand
//  . min          - override calculated min value
//  . max          - override calculated max value
class CQChartsColumnRealType : public CQChartsColumnType {
 public:
  CQChartsColumnRealType();

  bool isNumeric() const override { return true; }

  // input variant to data variant for edit
  QVariant userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                    const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // get min value
  QVariant minValue(const CQChartsNameValues &nameValues) const override;

  // get min value
  QVariant maxValue(const CQChartsNameValues &nameValues) const override;

  bool rmin(const CQChartsNameValues &nameValues, double &r) const;
  bool rmax(const CQChartsNameValues &nameValues, double &r) const;
};

//---

// integer column type class
//
// supports the following parameter
//  . format - output format
//  . min    - override calculated min value
//  . max    - override calculated max value
class CQChartsColumnIntegerType : public CQChartsColumnType {
 public:
  CQChartsColumnIntegerType();

  bool isNumeric() const override { return true; }

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

// time column type class
//
// supports the following parameter
//  . iformat - input format to convert input data to model data (time)
//  . oformat - output (display) format to convert time to string
//  - format  - convenience parameter when iformat and oformat are the same
class CQChartsColumnTimeType : public CQChartsColumnType {
 public:
  CQChartsColumnTimeType();

  bool isNumeric() const override { return true; }

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

// rect column type class
class CQChartsColumnRectType : public CQChartsColumnType {
 public:
  CQChartsColumnRectType();

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

// polygon column type class
class CQChartsColumnPolygonType : public CQChartsColumnType {
 public:
  CQChartsColumnPolygonType();

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

// polygon list column type class
class CQChartsColumnPolygonListType : public CQChartsColumnType {
 public:
  CQChartsColumnPolygonListType();

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

// connection list column type class
class CQChartsColumnConnectionListType : public CQChartsColumnType {
 public:
  CQChartsColumnConnectionListType();

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

// name pair column type class
class CQChartsColumnNamePairType : public CQChartsColumnType {
 public:
  CQChartsColumnNamePairType();

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

// path column type class
class CQChartsColumnPathType : public CQChartsColumnType {
 public:
  CQChartsColumnPathType();

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

// style column type class
class CQChartsColumnStyleType : public CQChartsColumnType {
 public:
  CQChartsColumnStyleType();

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

// color column type class
//
// supports the following parameter
//  . mapped  - is input data mapped to color (if not input data is taken as color name)
//  . min     - override min value for numeric value map
//  . max     - override max value for numeric value map
//  . palette - specific palette to lookup color in
class CQChartsColumnColorType : public CQChartsColumnType {
 public:
  CQChartsColumnColorType();

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

// image column type class
class CQChartsColumnImageType : public CQChartsColumnType {
 public:
  CQChartsColumnImageType();

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

// symbol type column type class
//
// supports the following parameter
//  . mapped - is input data mapped to symbol type (if not input data is taken as symbol name)
//  . min    - override min value for numeric value map
//  . max    - override max value for numeric value map
class CQChartsColumnSymbolTypeType : public CQChartsColumnType {
 public:
  CQChartsColumnSymbolTypeType();

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

// symbol size column type class
//
// supports the following parameter
//  . mapped   - is input data mapped to symbol size (if not input data is taken as symbol size)
//  . min      - override input min value for numeric value map
//  . max      - override input max value for numeric value map
//  . size_min - override output min value for numeric value map
//  . size_max - override output max value for numeric value map
class CQChartsColumnSymbolSizeType : public CQChartsColumnType {
 public:
  CQChartsColumnSymbolSizeType();

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

// font size column type class
//
// supports the following parameter
//  . mapped   - is input data mapped to font size (if not input data is taken as font size)
//  . min      - override input min value for numeric value map
//  . max      - override input max value for numeric value map
//  . size_min - override output min value for numeric value map
//  . size_max - override output max value for numeric value map
class CQChartsColumnFontSizeType : public CQChartsColumnType {
 public:
  CQChartsColumnFontSizeType();

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

// column type manager
class CQChartsColumnTypeMgr : public QObject {
  Q_OBJECT

 public:
  using Type = CQBaseModelType;

 public:
  CQChartsColumnTypeMgr(CQCharts *charts);
 ~CQChartsColumnTypeMgr();

  void typeNames(QStringList &names) const;

  void addType(Type type, CQChartsColumnType *data);

  CQChartsColumnType *decodeTypeData(const QString &type, CQChartsNameValues &nameValues) const;

  QString encodeTypeData(Type type, const CQChartsNameValues &nameValues) const;

  CQChartsColumnType *getType(Type type) const;

  QVariant getUserData(const QAbstractItemModel *model, const CQChartsColumn &column,
                       const QVariant &var, bool &converted) const;

  QVariant getDisplayData(const QAbstractItemModel *model, const CQChartsColumn &column,
                          const QVariant &var, bool &converted) const;

  bool getModelColumnType(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Type &type, Type &baseType, CQChartsNameValues &nameValues) const;

  bool setModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column, Type type,
                          const CQChartsNameValues &nameValues=CQChartsNameValues());

  void startCache();
  void endCache();

 private:
  using TypeData = std::map<Type,CQChartsColumnType*>;

  struct TypeCacheData {
    Type                type     { Type::NONE };
    Type                baseType { Type::NONE };
    CQChartsNameValues  nameValues;
    CQChartsColumnType* typeData { nullptr };
    bool                valid    { false };
  };

  using ColumnTypeCache = std::map<CQChartsColumn,TypeCacheData>;

 private:
  bool getModelColumnTypeData(const QAbstractItemModel *model, const CQChartsColumn &column,
                              TypeCacheData &typeCacheData) const;

 private:
  CQCharts*               charts_     { nullptr };
  TypeData                typeData_;
  int                     cacheDepth_ { 0 };
  mutable ColumnTypeCache columnTypeDataCache_;
};

#endif
