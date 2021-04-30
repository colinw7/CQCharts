#ifndef CQChartsArcData_H
#define CQChartsArcData_H

#include <CMathUtil.h>

/*!
 * \brief Arc Data class
 * \ingroup Charts
 */
class CQChartsArcData {
 public:
  using Point = CQChartsGeom::Point;

 public:
  CQChartsArcData() { }

  const Point &center() const { return c_; }
  void setCenter(const Point &c) { c_ = c; }

  double innerRadius() const { return ri_; }
  void setInnerRadius(double r) { ri_ = r; }

  double outerRadius() const { return ro_; }
  void setOuterRadius(double r) { ro_ = r; }

  const CQChartsAngle &angle1() const { return a1_; }
  void setAngle1(const CQChartsAngle &a) { a1_ = a; }

  const CQChartsAngle &angle2() const { return a2_; }
  void setAngle2(const CQChartsAngle &a) { a2_ = a; }

  bool inside(const Point &p) const {
    // calc distance from center (radius)
    double r = p.distanceTo(center());

    //---

    // check in radius extent
    double ri = innerRadius();
    double ro = outerRadius();

    if (r < ri || r > ro)
      return false;

    //---

    // if circle we are done
    if (CQChartsAngle::isCircle(angle1(), angle2()))
      return true;

    //---

    // check angle
    double a = CMathUtil::Rad2Deg(atan2(p.y - center().y, p.x - center().x));
    a = CMathUtil::normalizeAngle(a);

    double a1 = CMathUtil::normalizeAngle(angle1().value());
    double a2 = CMathUtil::normalizeAngle(angle2().value());

    if (a1 > a2) {
      // crosses zero
      if (a >= 0.0 && a <= a2) // check above (0 -> a2)
        return true;

      if (a <= 360.0 && a >= a1) // check below (a1 -> 360)
        return true;
    }
    else {
      if (a >= a1 && a <= a2)
        return true;
    }

    return false;
  }

 private:
  Point         c_  { 0.0, 0.0 };
  double        ri_ { 0.0 };
  double        ro_ { 1.0 };
  CQChartsAngle a1_ { 0.0 };
  CQChartsAngle a2_ { 360.0 };
};

#endif
