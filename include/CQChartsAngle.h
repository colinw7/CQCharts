#ifndef CQChartsAngle_H
#define CQChartsAngle_H

#include <CQChartsTmpl.h>
#include <CMathUtil.h>
#include <QString>

/*!
 * \brief class to contain charts angle (degrees)
 * \ingroup Charts
 *
 * TODO: support postfix for type of degrees/radians
 */
class CQChartsAngle :
  public CQChartsEqBase<CQChartsAngle>,
  public CQChartsToStringBase<CQChartsAngle> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  enum class Type {
    DEGREES,
    RADIANS
  };

 public:
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

  bool isZero() const {
    //double a = CMathUtil::normalizeAngle(a_, /*isEnd*/false);
    return CMathUtil::isZero(a_);
  }

  static bool isCircle(const CQChartsAngle &angle1, const CQChartsAngle &angle2) {
    double a1 = angle1.value();
    double a2 = angle2.value();

    double a21 = a2 - a1;

    return CMathUtil::realEq(std::abs(a21), 360.0);
  }

  //---

  double degrees() const { return a_; }
  double radians() const { return CMathUtil::Deg2Rad(a_); }

  //---

  double cos() const { return std::cos(radians()); }
  double sin() const { return std::sin(radians()); }

  //---

  // operator +, +=
  friend CQChartsAngle operator+(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return CQChartsAngle(lhs.a_ + rhs.a_);
  }

  CQChartsAngle &operator+=(const CQChartsAngle &rhs) { a_ += rhs.a_; return *this; }

  // operator -, -=
  friend CQChartsAngle operator-(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return CQChartsAngle(lhs.a_ - rhs.a_);
  }

  CQChartsAngle &operator-=(const CQChartsAngle &rhs) { a_ -= rhs.a_; return *this; }

  //---

  // operator -
  CQChartsAngle operator-() const { return CQChartsAngle(-a_); }

  //---

  //! operator ==
  friend bool operator==(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return (lhs.a_ == rhs.a_);
  }

  //---

  QString toString() const;

  bool fromString(const QString &s);

 private:
  double a_ { 0.0 }; //!< angle (degrees)
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsAngle)

#endif
