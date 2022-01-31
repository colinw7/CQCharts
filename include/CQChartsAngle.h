#ifndef CQChartsAngle_H
#define CQChartsAngle_H

#include <CQChartsTmpl.h>
#include <CQChartsGeom.h>
#include <CQUtilMeta.h>
#include <CMathUtil.h>
#include <QString>

/*!
 * \brief class to contain charts angle (degrees)
 * \ingroup Charts
 *
 * TODO: support postfix for type of degrees/radians
 */
class CQChartsAngle :
  public CQChartsComparatorBase<CQChartsAngle>,
  public CQChartsToStringBase<CQChartsAngle> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsAngle, metaTypeId)

 public:
  enum class Type {
    DEGREES,
    RADIANS
  };

  using Angle = CQChartsAngle;
  using Point = CQChartsGeom::Point;

 public:
  static Angle degrees(double value) { return Angle(Type::DEGREES, value); }
  static Angle radians(double value) { return Angle(Type::RADIANS, value); }

  //! default constructor
  CQChartsAngle() = default;

  explicit CQChartsAngle(const QString &str);

  explicit CQChartsAngle(double a) :
   a_(a) {
  }

  CQChartsAngle(Type type, double value) {
    a_ = (type == Type::RADIANS ? CMathUtil::Rad2Deg(value) : value);
  }

  double value() const { return a_; }
  void setValue(double a) { a_ = a; }

  //---

  bool isValid() const { return true; }

  // is zero angle
  bool isZero() const {
    //double a = CMathUtil::normalizeAngle(a_, /*isEnd*/false);
    return CMathUtil::isZero(a_);
  }

  //---

  // get angle mid point
  static Angle avg(const Angle &angle1, const Angle &angle2) {
    double a1 = angle1.value();
    double a2 = angle2.value();

    return Angle(CMathUtil::avg(a1, a2));
  }

  // do angles produce a circle
  static bool isCircle(const Angle &angle1, const Angle &angle2) {
    double a1 = angle1.value();
    double a2 = angle2.value();

    double a21 = a2 - a1;

    return (std::abs(a21) > 360.0 || CMathUtil::realEq(std::abs(a21), 360.0));
  }

  // point at angle on circle
  static Point circlePoint(const Point &c, double r, const Angle &angle) {
    return CQChartsGeom::circlePoint(c, r, angle.radians());
  }

  //---

  double degrees() const { return a_; }
  double radians() const { return CMathUtil::Deg2Rad(a_); }

  //---

  double cos() const { return std::cos(radians()); }
  double sin() const { return std::sin(radians()); }

  //---

  void flipX() { a_ = 180.0 + a_; }

  //---

  // operator +, +=
  friend Angle operator+(const Angle &lhs, const Angle &rhs) {
    return Angle(lhs.a_ + rhs.a_);
  }

  Angle &operator+=(const Angle &rhs) { a_ += rhs.a_; return *this; }

  // operator -, -=
  friend Angle operator-(const Angle &lhs, const Angle &rhs) {
    return Angle(lhs.a_ - rhs.a_);
  }

  Angle &operator-=(const Angle &rhs) { a_ -= rhs.a_; return *this; }

  //---

  // operator -
  Angle operator-() const { return Angle(-a_); }

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const Angle &lhs, const Angle &rhs) {
    if (lhs.a_ >  rhs.a_) return  1;
    if (lhs.a_ <  rhs.a_) return -1;
    return 0;
  }

  //---

  QString toString() const;

  bool fromString(const QString &s);

 private:
  double a_ { 0.0 }; //!< angle (degrees)
};

//---

CQUTIL_DCL_META_TYPE(CQChartsAngle)

#endif
