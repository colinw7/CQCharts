#ifndef CQChartsFillPattern_H
#define CQChartsFillPattern_H

#include <CQChartsEnum.h>
#include <CQChartsColor.h>

/*!
 * \brief fill pattern
 * \ingroup Charts
 */
class CQChartsFillPattern {
 public:
  enum class Type {
    NONE,
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG,
    LGRADIENT,
    RGRADIENT,
    PALETTE
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsFillPattern(Type type=Type::SOLID) :
   type_(type) {
  }

  explicit CQChartsFillPattern(const QString &str) {
    (void) setValue(str);
  }

  //---

  Type type() const { return type_; }
  void setType(Type type) { type_  = type; }

  bool isValid() const { return type() != Type::NONE; }

  //---

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  //---

  const QString &palette() const { return palette_; }
  void setPalette(const QString &s) { palette_ = s; }

  //---

  double angle() const { return angle_; }
  void setAngle(double r) { angle_ = r; }

  //---

  const CQChartsColor &altColor() const { return altColor_; }
  void setAltColor(const CQChartsColor &c) { altColor_ = c; }

  //---

  QString toString() const;
  bool fromString(const QString &s);

  //---

  QStringList enumNames() const;

  //---

  static Type stringToType(const QString &str);
  static QString typeToString(const Type &type);

  static Type styleToType(const Qt::BrushStyle &style);
  static Qt::BrushStyle typeToStyle(const Type &type);

  //---

  Qt::BrushStyle style() const { return typeToStyle(type_); }

  //---

  friend bool operator==(const CQChartsFillPattern &lhs, const CQChartsFillPattern &rhs) {
    return (lhs.type    () == rhs.type    () &&
            lhs.scale   () == rhs.scale   () &&
            lhs.palette () == rhs.palette () &&
            lhs.angle   () == rhs.angle   () &&
            lhs.altColor() == rhs.altColor());
  }

  friend bool operator!=(const CQChartsFillPattern &lhs, const CQChartsFillPattern &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  friend std::ostream &operator<<(std::ostream &os, const CQChartsFillPattern &t) {
    t.print(os);
    return os;
  }

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

 private:
  bool setValue(const QString &str);

 private:
  Type          type_  { Type::NONE };
  double        scale_ { 1.0 };
  QString       palette_;
  double        angle_ { 0.0 };
  CQChartsColor altColor_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsFillPattern)

#endif
