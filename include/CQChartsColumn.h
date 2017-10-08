#ifndef CQChartsColumn_H
#define CQChartsColumn_H

#include <CQChartsUtil.h>
#include <QString>

typedef std::map<QString,QString> CQChartsNameValues;

class CQChartsColumnType {
 public:
  CQChartsColumnType(const QString &name) :
   name_(name) {
  }

  const QString &name() const { return name_; }

  virtual ~CQChartsColumnType() { }

  // input variant to data variant
  virtual QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues) const = 0;

  // data string to output data
  virtual QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const = 0;

 private:
  QString name_;
};

//---

class CQChartsColumnStringType : public CQChartsColumnType {
 public:
  CQChartsColumnStringType() :
   CQChartsColumnType("string") {
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
   CQChartsColumnType("real") {
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
    return userData(var, nameValues);
  }
};

//---

class CQChartsColumnIntegerType : public CQChartsColumnType {
 public:
  CQChartsColumnIntegerType() :
   CQChartsColumnType("integer") {
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
    return userData(var, nameValues);
  }
};

//---

class CQChartsColumnTimeType : public CQChartsColumnType {
 public:
  CQChartsColumnTimeType() :
   CQChartsColumnType("time") {
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

  void addType(const QString &name, CQChartsColumnType *type);

  CQChartsColumnType *decodeTypeData(const QString &type, CQChartsNameValues &nameValues) const;

  CQChartsColumnType *getType(const QString &name) const;

 private:
  typedef std::map<QString,CQChartsColumnType *> NameType;

  NameType nameType_;
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

  bool decodeType(QString &baseType, CQChartsNameValues &nameValues) const {
    return decodeType(type_, baseType, nameValues);
  }

  static bool decodeType(const QString &type, QString &baseType, CQChartsNameValues &nameValues) {
    int pos = type.indexOf(":");

    if (pos < 0) {
      baseType = type;

      return true;
    }

    baseType = type.mid(0, pos);

    QString rhs = type.mid(pos + 1);

    QStringList strs = rhs.split(",", QString::SkipEmptyParts);

    for (int i = 0; i < strs.length(); ++i) {
      int pos1 = strs[i].indexOf("=");

      if (pos1 < 1) {
        nameValues[strs[i]] = "1";
      }
      else {
        QString name  = strs[i].mid(0, pos1 ).simplified();
        QString value = strs[i].mid(pos1 + 1).simplified();

        nameValues[name] = value;
      }
    }

    return true;
  }

 private:
  QString name_;
  QString type_ { "string" };
};

#endif
