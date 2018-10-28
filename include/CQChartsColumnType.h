#ifndef CQChartsColumnType_H
#define CQChartsColumnType_H

#include <CQChartsUtil.h>
#include <CQBaseModel.h>
#include <QObject>
#include <QString>

// column type parameter
class CQChartsColumnTypeParam {
 public:
  using Type = CQBaseModel::Type;

 public:
  CQChartsColumnTypeParam(const QString &name, Type type, int role, const QString &tip) :
   name_(name), type_(type), role_(role), tip_(tip) {
  }

  CQChartsColumnTypeParam(const QString &name, Type type, const QString &tip) :
   name_(name), type_(type), role_(int(CQBaseModel::Role::TypeValues)), tip_(tip) {
  }

  const QString &name() const { return name_; }

  Type type() const { return type_; }

  int role() const { return role_; }

  const QString &tip() const { return tip_; }

 private:
  QString name_;
  Type    type_ { CQBaseModel::Type::STRING };
  int     role_ { -1 };
  QString tip_;
};

//---

// column type base class
class CQChartsColumnType {
 public:
  using Type   = CQBaseModel::Type;
  using Params = std::vector<CQChartsColumnTypeParam>;

 public:
  CQChartsColumnType(Type type);

  virtual ~CQChartsColumnType() { }

  Type type() const { return type_; }

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  virtual QString name() const { return CQBaseModel::typeName(type_); }

  virtual bool isNumeric() const { return false; }

  const Params &params() const { return params_; }

  bool hasParam(const QString &name) const;

  const CQChartsColumnTypeParam *getParam(const QString &name) const;

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

 protected:
  Type   type_;       // base type
  int    ind_ { -1 }; // insertion index
  Params params_;     // parameters
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

// string column type class
class CQChartsColumnBooleanType : public CQChartsColumnType {
 public:
  CQChartsColumnBooleanType() :
   CQChartsColumnType(Type::BOOLEAN) {
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
  CQChartsColumnRealType();

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
  CQChartsColumnIntegerType();

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
  CQChartsColumnTimeType();

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

// polygon list column type class
class CQChartsColumnPolygonListType : public CQChartsColumnType {
 public:
  CQChartsColumnPolygonListType() :
   CQChartsColumnType(Type::POLYGON_LIST) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

 private:
  bool isPolygonListVariant(const QVariant &var) const;
};

//---

// connection list column type class
class CQChartsColumnConnectionListType : public CQChartsColumnType {
 public:
  CQChartsColumnConnectionListType() :
   CQChartsColumnType(Type::CONNECTION_LIST) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

 private:
  bool isConnectionListVariant(const QVariant &var) const;
};

//---

// connection list column type class
class CQChartsColumnNamePairType : public CQChartsColumnType {
 public:
  CQChartsColumnNamePairType() :
   CQChartsColumnType(Type::NAME_PAIR) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues,
                    bool &converted) const override;

 private:
  bool isNamePairVariant(const QVariant &var) const;
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

  void typeNames(QStringList &names) const;

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

#endif
