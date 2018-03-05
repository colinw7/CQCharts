#ifndef CQChartsColumnType_H
#define CQChartsColumnType_H

#include <CQChartsUtil.h>
#include <CQBaseModel.h>
#include <QString>

// column type base class
class CQChartsColumnType {
 public:
  CQChartsColumnType(CQBaseModel::Type type) :
   type_(type) {
  }

  virtual ~CQChartsColumnType() { }

  CQBaseModel::Type type() const { return type_; }

  virtual QString name() const { return CQBaseModel::typeName(type_); }

  // input variant to data variant for edit
  virtual QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues) const = 0;

  // data variant to output variant (string) for display
  virtual QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const = 0;

  // data min/max value
  virtual QVariant minValue(const CQChartsNameValues &) const { return QVariant(); }
  virtual QVariant maxValue(const CQChartsNameValues &) const { return QVariant(); }

 private:
  CQBaseModel::Type type_; // base type
};

//---

// string column type class
class CQChartsColumnStringType : public CQChartsColumnType {
 public:
  CQChartsColumnStringType() :
   CQChartsColumnType(CQBaseModel::Type::STRING) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::String) return var;

    QString str;

    CQChartsUtil::variantToString(var, str);

    return QVariant::fromValue<QString>(str);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    return userData(var, nameValues);
  }
};

//---

// real column type class
class CQChartsColumnRealType : public CQChartsColumnType {
 public:
  CQChartsColumnRealType() :
   CQChartsColumnType(CQBaseModel::Type::REAL) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Double) return var;

    bool ok;

    double r = CQChartsUtil::toReal(var, ok);
    if (! ok) return var;

    return QVariant::fromValue<double>(r);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    bool ok;

    double r = CQChartsUtil::toReal(var, ok);
    if (! ok) return QVariant();

    // optional format for real
    auto p1 = nameValues.find("format");

    if (p1 == nameValues.end())
      return CQChartsUtil::toString(r);

    // support units suffix with scale factor and format
    auto p2 = nameValues.find("format_scale");

    if (p2 != nameValues.end()) {
      bool ok1;

      double scale = CQChartsUtil::toReal((*p2).second, ok1);

      if (ok1)
        r *= scale;
    }

    // convert value using format
    return CQChartsUtil::toString(r, (*p1).second);
  }

  // get min value
  QVariant minValue(const CQChartsNameValues &nameValues) const override {
    auto p = nameValues.find("min");
    if (p == nameValues.end()) return QVariant();

    bool ok;

    double r = CQChartsUtil::toReal((*p).second, ok);
    if (! ok) return QVariant();

    return QVariant(r);
  }

  // get min value
  QVariant maxValue(const CQChartsNameValues &nameValues) const override {
    auto p = nameValues.find("max");
    if (p == nameValues.end()) return QVariant();

    bool ok;

    double r = CQChartsUtil::toReal((*p).second, ok);
    if (! ok) return QVariant();

    return QVariant(r);
  }
};

//---

// integer column type class
class CQChartsColumnIntegerType : public CQChartsColumnType {
 public:
  CQChartsColumnIntegerType() :
   CQChartsColumnType(CQBaseModel::Type::INTEGER) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Int) return var;

    bool ok;

    long l = CQChartsUtil::toInt(var, ok);
    if (! ok) return var;

    return QVariant::fromValue<int>(l);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &) const override {
    bool ok;

    long l = CQChartsUtil::toInt(var, ok);
    if (! ok) return QVariant();

    return CQChartsUtil::toString(l);
  }
};

//---

// time column type class
class CQChartsColumnTimeType : public CQChartsColumnType {
 public:
  CQChartsColumnTimeType() :
   CQChartsColumnType(CQBaseModel::Type::TIME) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    // use format string to convert model (input) string to time (double)
    // TODO: assert if no format ?
    auto p = nameValues.find("format");

    if (p == nameValues.end())
      return var;

    double t;

    if (! CQChartsUtil::stringToTime((*p).second, var.toString(), t))
      return var;

    return QVariant::fromValue<double>(t);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    // get time value (double)
    bool ok;

    double r = CQChartsUtil::toReal(var, ok);
    if (! ok) return var;

    //---

    // use oformat (optional) or format (required ?) to convert time format
    // to user (display) string
    auto p = nameValues.find("oformat");

    if (p != nameValues.end())
      return CQChartsUtil::timeToString((*p).second, r);

    auto p1 = nameValues.find("format");

    if (p1 != nameValues.end())
      return CQChartsUtil::timeToString((*p1).second, r);

    //---

    return CQChartsUtil::toString(r);
  }
};

//---

// rect column type class
class CQChartsColumnRectType : public CQChartsColumnType {
 public:
  CQChartsColumnRectType() :
   CQChartsColumnType(CQBaseModel::Type::RECT) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::RectF) return var;

    QRectF r;

    if (var.type() == QVariant::Rect) {
      r = var.value<QRect>();
      return QVariant::fromValue<QRectF>(r);
    }

    QString str = var.toString();

    (void) CQChartsUtil::stringToRect(str, r);

    return QVariant::fromValue<QRectF>(r);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::RectF) {
      QRectF r = var.value<QRectF>();

      return CQChartsUtil::rectToString(r);
    }

    return var; // TODO: other var formats
  }
};

//---

// polygon column type class
class CQChartsColumnPolygonType : public CQChartsColumnType {
 public:
  CQChartsColumnPolygonType() :
   CQChartsColumnType(CQBaseModel::Type::POLYGON) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::PolygonF) return var;

    QPolygonF poly;

    if (var.type() == QVariant::Polygon) {
      poly = var.value<QPolygon>();
      return QVariant::fromValue<QPolygonF>(poly);
    }

    QString str = var.toString();

    (void) CQChartsUtil::stringToPolygon(str, poly);

    return QVariant::fromValue<QPolygonF>(poly);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::PolygonF) {
      QPolygonF poly = var.value<QPolygonF>();

      return CQChartsUtil::polygonToString(poly);
    }

    return var; // TODO: other var formats
  }
};

//---

// path column type class
class CQChartsColumnPathType : public CQChartsColumnType {
 public:
  CQChartsColumnPathType() :
   CQChartsColumnType(CQBaseModel::Type::PATH) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.userType() == CQChartsPath::metaType()) return var;

    QString str = var.toString();

    CQChartsPath path;

    (void) CQChartsUtil::stringToPath(str, path);

    return QVariant::fromValue<CQChartsPath>(path);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.userType() == CQChartsPath::metaType()) {
      CQChartsPath path = var.value<CQChartsPath>();

      return CQChartsUtil::pathToString(path);
    }

    return var; // TODO: other var formats
  }
};

//---

// style column type class
class CQChartsColumnStyleType : public CQChartsColumnType {
 public:
  CQChartsColumnStyleType() :
   CQChartsColumnType(CQBaseModel::Type::STYLE) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.userType() == CQChartsStyle::metaType()) return var;

    QString str = var.toString();

    CQChartsStyle style;

    (void) CQChartsUtil::stringToStyle(str, style);

    return QVariant::fromValue<CQChartsStyle>(style);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.userType() == CQChartsStyle::metaType()) {
      CQChartsStyle style = var.value<CQChartsStyle>();

      return CQChartsUtil::styleToString(style);
    }

    return var; // TODO: other var formats
  }
};

//---

// color column type class
class CQChartsColumnColorType : public CQChartsColumnType {
 public:
  CQChartsColumnColorType() :
   CQChartsColumnType(CQBaseModel::Type::COLOR) {
  }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Color) return var;

    QString str = var.toString();

    QColor c(str);

    return c;
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Color) {
      QColor c = var.value<QColor>();

      return c.name();
    }

    return var; // TODO: other var formats
  }
};

//---

// column type manager
class CQChartsColumnTypeMgr {
 public:
  CQChartsColumnTypeMgr();
 ~CQChartsColumnTypeMgr();

  void addType(CQBaseModel::Type type, CQChartsColumnType *data);

  CQChartsColumnType *decodeTypeData(const QString &type, CQChartsNameValues &nameValues) const;

  QString encodeTypeData(CQBaseModel::Type type, const CQChartsNameValues &nameValues) const;

  CQChartsColumnType *getType(CQBaseModel::Type type) const;

  QVariant getUserData(QAbstractItemModel *model, const CQChartsColumn &column,
                       const QVariant &var) const;

  QVariant getDisplayData(QAbstractItemModel *model, const CQChartsColumn &column,
                          const QVariant &var) const;

  bool getModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column,
                          CQBaseModel::Type &type, CQChartsNameValues &nameValues) const;

  bool setModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column,
                          CQBaseModel::Type type, const CQChartsNameValues &nameValues);

 private:
  using TypeData = std::map<CQBaseModel::Type,CQChartsColumnType*>;

  TypeData typeData_;
};

//---

namespace CQChartsColumnUtil {
  bool decodeType(const QString &type, QString &baseType, CQChartsNameValues &nameValues);

  bool decodeNameValues(const QString &str, CQChartsNameValues &nameValues);

  QString encodeNameValues(const CQChartsNameValues &nameValues);
}

#endif
