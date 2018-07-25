#ifndef CQChartsColumnType_H
#define CQChartsColumnType_H

#include <CQChartsUtil.h>
#include <CQBaseModel.h>
#include <QObject>
#include <QString>

// column type base class
class CQChartsColumnType {
 public:
  using Type = CQBaseModel::Type;

 public:
  CQChartsColumnType(Type type) :
   type_(type) {
  }

  virtual ~CQChartsColumnType() { }

  Type type() const { return type_; }

  virtual QString name() const { return CQBaseModel::typeName(type_); }

  virtual bool isNumeric() const { return false; }

  // input variant to data variant for edit
  virtual QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                            bool &converted) const = 0;

  // data variant to output variant (string) for display
  virtual QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                            bool &converted) const = 0;

  // data min/max value
  virtual QVariant minValue(const CQChartsNameValues &) const { return QVariant(); }
  virtual QVariant maxValue(const CQChartsNameValues &) const { return QVariant(); }

  // index value (TODO: assert if index invalid or not supported ?)
  virtual QVariant indexVar(const QVariant &var, const QString &) const { return var; }

  // index type (TODO: assert if index invalid or not supported ?)
  virtual Type indexType(const QString &) const { return type(); }

 private:
  Type type_; // base type
};

//---

// string column type class
class CQChartsColumnStringType : public CQChartsColumnType {
 public:
  CQChartsColumnStringType() :
   CQChartsColumnType(Type::STRING) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override {
    return userData(var, nameValues, converted);
  }
};

//---

// real column type class
class CQChartsColumnRealType : public CQChartsColumnType {
 public:
  CQChartsColumnRealType() :
   CQChartsColumnType(Type::REAL) {
  }

  bool isNumeric() const override { return true; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
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
class CQChartsColumnIntegerType : public CQChartsColumnType {
 public:
  CQChartsColumnIntegerType() :
   CQChartsColumnType(Type::INTEGER) {
  }

  bool isNumeric() const override { return true; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;
};

//---

// time column type class
class CQChartsColumnTimeType : public CQChartsColumnType {
 public:
  CQChartsColumnTimeType() :
   CQChartsColumnType(Type::TIME) {
  }

  bool isNumeric() const override { return true; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
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
  CQChartsColumnRectType() :
   CQChartsColumnType(Type::RECT) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;
};

//---

// polygon column type class
class CQChartsColumnPolygonType : public CQChartsColumnType {
 public:
  CQChartsColumnPolygonType() :
   CQChartsColumnType(Type::POLYGON) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;
};

//---

// path column type class
class CQChartsColumnPathType : public CQChartsColumnType {
 public:
  CQChartsColumnPathType() :
   CQChartsColumnType(Type::PATH) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;
};

//---

// style column type class
class CQChartsColumnStyleType : public CQChartsColumnType {
 public:
  CQChartsColumnStyleType() :
   CQChartsColumnType(Type::STYLE) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;
};

//---

// color column type class
class CQChartsColumnColorType : public CQChartsColumnType {
 public:
  CQChartsColumnColorType() :
   CQChartsColumnType(Type::COLOR) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;
};

//---

// column type manager
class CQChartsColumnTypeMgr : public QObject {
  Q_OBJECT

 public:
  using Type = CQBaseModel::Type;

 public:
  CQChartsColumnTypeMgr(CQCharts *charts);
 ~CQChartsColumnTypeMgr();

  void addType(Type type, CQChartsColumnType *data);

  CQChartsColumnType *decodeTypeData(const QString &type, CQChartsNameValues &nameValues) const;

  QString encodeTypeData(Type type, const CQChartsNameValues &nameValues) const;

  CQChartsColumnType *getType(Type type) const;

  QVariant getUserData(QAbstractItemModel *model, const CQChartsColumn &column,
                       const QVariant &var, bool &converted) const;

  QVariant getDisplayData(QAbstractItemModel *model, const CQChartsColumn &column,
                          const QVariant &var, bool &converted) const;

  bool getModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column,
                          Type &type, CQChartsNameValues &nameValues) const;

  bool setModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column, Type type,
                          const CQChartsNameValues &nameValues=CQChartsNameValues());

 private:
  using TypeData = std::map<Type,CQChartsColumnType*>;

  CQCharts* charts_ { nullptr };
  TypeData  typeData_;
};

//---

namespace CQChartsColumnUtil {
  bool decodeType(const QString &type, QString &baseType, CQChartsNameValues &nameValues);

  bool decodeNameValues(const QString &str, CQChartsNameValues &nameValues);

  QString encodeNameValues(const CQChartsNameValues &nameValues);
}

#endif
