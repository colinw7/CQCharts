#ifndef CQChartsColumn_H
#define CQChartsColumn_H

#include <CQChartsUtil.h>
#include <CQBaseModel.h>
#include <QString>

using CQChartsNameValues = std::map<QString,QString>;

class CQChartsColumnType {
 public:
  CQChartsColumnType(CQBaseModel::Type type) :
   type_(type) {
  }

  CQBaseModel::Type type() const { return type_; }

  virtual ~CQChartsColumnType() { }

  // input variant to data variant
  virtual QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues) const = 0;

  // data variant to output variant
  virtual QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const = 0;

 private:
  CQBaseModel::Type type_;
};

//---

class CQChartsColumnStringType : public CQChartsColumnType {
 public:
  CQChartsColumnStringType() :
   CQChartsColumnType(CQBaseModel::Type::STRING) {
  }

  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Double)
      return CQChartsUtil::toString(var.toDouble());

    if (var.type() == QVariant::Int)
      return CQChartsUtil::toString((long) var.toInt());

    return var;
  }

  QVariant dataName(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Double)
      return CQChartsUtil::toString(var.toDouble());

    if (var.type() == QVariant::Int)
      return CQChartsUtil::toString((long) var.toInt());

    return var;
  }
};

//---

class CQChartsColumnRealType : public CQChartsColumnType {
 public:
  CQChartsColumnRealType() :
   CQChartsColumnType(CQBaseModel::Type::REAL) {
  }

  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Double)
      return var;

    bool ok;

    double r = CQChartsUtil::toReal(var, ok);

    if (! ok)
      return var;

    return QVariant::fromValue<double>(r);
  }

  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    auto p1 = nameValues.find("format");

    if (p1 != nameValues.end())
      return CQChartsUtil::toString(userData(var, nameValues).toDouble(), (*p1).second);
    else
      return CQChartsUtil::toString(userData(var, nameValues).toDouble());
  }
};

//---

class CQChartsColumnIntegerType : public CQChartsColumnType {
 public:
  CQChartsColumnIntegerType() :
   CQChartsColumnType(CQBaseModel::Type::INTEGER) {
  }

  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Int)
      return var;

    bool ok;

    long l = CQChartsUtil::toInt(var, ok);

    if (! ok)
      return var;

    return QVariant::fromValue<long>(l);
  }

  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    return CQChartsUtil::toString((long) userData(var, nameValues).toInt());
  }
};

//---

class CQChartsColumnTimeType : public CQChartsColumnType {
 public:
  CQChartsColumnTimeType() :
   CQChartsColumnType(CQBaseModel::Type::TIME) {
  }

  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    auto p = nameValues.find("format");

    if (p != nameValues.end()) {
      double t;

      if (! stringToTime((*p).second, var.toString(), t))
        return var;

      return QVariant::fromValue<double>(t);
    }

    return var;
  }

  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    bool ok;

    double t = var.toDouble(&ok);

    if (! ok)
      return var;

    auto p = nameValues.find("oformat");

    if (p != nameValues.end())
      return timeToString((*p).second, t);

    auto p1 = nameValues.find("format");

    if (p1 != nameValues.end())
      return timeToString((*p1).second, t);

    return QVariant::fromValue<double>(t);
  }

 private:
  static QString timeToString(const QString &fmt, double r) {
    static char buffer[512];

    time_t t(r);

    struct tm *tm1 = localtime(&t);

    (void) strftime(buffer, 512, fmt.toLatin1().constData(), tm1);

    return buffer;
  }

  static bool stringToTime(const QString &fmt, const QString &str, double &t) {
    struct tm tm1; memset(&tm1, 0, sizeof(tm));

    char *p = strptime(str.toLatin1().constData(), fmt.toLatin1().constData(), &tm1);

    if (! p)
      return false;

    t = mktime(&tm1);

    return true;
  }

#if 0
  QString calcTimeFmt() const {
    if (timeFmt_ == "")
      return "%d/%m/%y,%H:%M";

    return timeFmt_;
  }
#endif
};

//---

class CQChartsColumnTypeMgr {
 public:
  CQChartsColumnTypeMgr();
 ~CQChartsColumnTypeMgr();

  void addType(CQBaseModel::Type type, CQChartsColumnType *data);

  CQChartsColumnType *decodeTypeData(const QString &type, CQChartsNameValues &nameValues) const;

  QString encodeTypeData(CQBaseModel::Type type, const CQChartsNameValues &nameValues) const;

  CQChartsColumnType *getType(CQBaseModel::Type type) const;

  QVariant getUserData(const QAbstractItemModel *model, int column, const QVariant &var) const;

  QVariant getDisplayData(const QAbstractItemModel *model, int column, const QVariant &var) const;

  bool getModelColumnType(const QAbstractItemModel *model, int column, CQBaseModel::Type &type,
                          CQChartsNameValues &nameValues) const;

  bool setModelColumnType(QAbstractItemModel *model, int column, CQBaseModel::Type type,
                          const CQChartsNameValues &nameValues);

 private:
  using TypeData = std::map<CQBaseModel::Type,CQChartsColumnType*>;

  TypeData typeData_;
};

//---

class CQChartsColumn {
 public:
  struct NameValue {
    NameValue(const QString &name, const QString &value) :
     name(name), value(value) {
    }

    QString name;
    QString value;
  };

 public:
  CQChartsColumn(const QString &name=QString()) :
   name_(name) {
  }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const QString &type() const { return type_; }
  void setType(const QString &s) { type_ = s; }

  bool decodeType(QString &baseType, CQChartsNameValues &nameValues) const;

  static bool decodeType(const QString &type, QString &baseType, CQChartsNameValues &nameValues);

  static bool decodeNameValues(const QString &str, CQChartsNameValues &nameValues);

  static QString encodeNameValues(const CQChartsNameValues &nameValues);

 private:
  QString name_;
  QString type_ { "string" };
};

#endif
