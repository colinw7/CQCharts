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

  virtual const char *name() const = 0;

  // input variant to data variant for edit
  virtual QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues) const = 0;

  // data variant to output variant (string) for display
  virtual QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const = 0;

 private:
  CQBaseModel::Type type_;
};

//---

// string column type class
class CQChartsColumnStringType : public CQChartsColumnType {
 public:
  CQChartsColumnStringType() :
   CQChartsColumnType(CQBaseModel::Type::STRING) {
  }

  const char *name() const { return "string"; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Double)
      return CQChartsUtil::toString(var.toDouble());

    if (var.type() == QVariant::Int)
      return CQChartsUtil::toString((long) var.toInt());

    return var; // var.toString() ?
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Double)
      return CQChartsUtil::toString(var.toDouble());

    if (var.type() == QVariant::Int)
      return CQChartsUtil::toString((long) var.toInt());

    return var; // var.toString() ?
  }
};

//---

// real column type class
class CQChartsColumnRealType : public CQChartsColumnType {
 public:
  CQChartsColumnRealType() :
   CQChartsColumnType(CQBaseModel::Type::REAL) {
  }

  const char *name() const { return "real"; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Double)
      return var;

    bool ok;

    double r = CQChartsUtil::toReal(var, ok);

    if (! ok)
      return var; // QVariant() ?

    return QVariant::fromValue<double>(r);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    double value = userData(var, nameValues).toDouble();

    auto p1 = nameValues.find("format");

    if (p1 != nameValues.end()) {
      // support units suffix with scale factor and format
      auto p2 = nameValues.find("format_scale");

      if (p2 != nameValues.end()) {
        bool ok;

        double scale = CQChartsUtil::toReal((*p2).second, ok);

        if (ok)
          value *= scale;
      }

      // convert value using format
      return CQChartsUtil::toString(value, (*p1).second);
    }
    else
      return CQChartsUtil::toString(value);
  }
};

//---

// integer column type class
class CQChartsColumnIntegerType : public CQChartsColumnType {
 public:
  CQChartsColumnIntegerType() :
   CQChartsColumnType(CQBaseModel::Type::INTEGER) {
  }

  const char *name() const { return "integer"; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Int)
      return var;

    bool ok;

    long l = CQChartsUtil::toInt(var, ok);

    if (! ok)
      return var; // QVariant() ?

    return QVariant::fromValue<int>(l);
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    return CQChartsUtil::toString((long) userData(var, nameValues).toInt());
  }
};

//---

// time column type class
class CQChartsColumnTimeType : public CQChartsColumnType {
 public:
  CQChartsColumnTimeType() :
   CQChartsColumnType(CQBaseModel::Type::TIME) {
  }

  const char *name() const { return "time"; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    // use format string to convert model (input) string to time (double)
    // TODO: assert if no format ?
    auto p = nameValues.find("format");

    if (p != nameValues.end()) {
      double t;

      if (! CQChartsUtil::stringToTime((*p).second, var.toString(), t))
        return var;

      return QVariant::fromValue<double>(t);
    }

    return var;
  }

  // data variant to output variant (string) for display
  QVariant dataName(const QVariant &var, const CQChartsNameValues &nameValues) const override {
    // get time value (double)
    bool ok;

    double t = var.toDouble(&ok);

    if (! ok)
      return var;

    //---

    // use oformat (optional) or format (required ?) to convert time format
    // to user (display) string
    auto p = nameValues.find("oformat");

    if (p != nameValues.end())
      return CQChartsUtil::timeToString((*p).second, t);

    auto p1 = nameValues.find("format");

    if (p1 != nameValues.end())
      return CQChartsUtil::timeToString((*p1).second, t);

    //---

    return QVariant::fromValue<double>(t);
  }
};

//---

// rect column type class
class CQChartsColumnRectType : public CQChartsColumnType {
 public:
  CQChartsColumnRectType() :
   CQChartsColumnType(CQBaseModel::Type::RECT) {
  }

  const char *name() const { return "rect"; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::RectF)
      return var;

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

      return QString("%1 %2 %3 %4").arg(r.left()).arg(r.top()).arg(r.right()).arg(r.bottom());
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

  const char *name() const { return "polygon"; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::PolygonF)
      return var;

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

// color column type class
class CQChartsColumnColorType : public CQChartsColumnType {
 public:
  CQChartsColumnColorType() :
   CQChartsColumnType(CQBaseModel::Type::COLOR) {
  }

  const char *name() const { return "color"; }

  // input variant to data variant for edit
  QVariant userData(const QVariant &var, const CQChartsNameValues &) const override {
    if (var.type() == QVariant::Color)
      return var;

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
