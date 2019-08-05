#ifndef CQChartsSmooth_H
#define CQChartsSmooth_H

#include <CQChartsGeom.h>
#include <vector>

//---

/*!
 * \brief Two Point Bezier
 * \ingroup Charts
 */
class CQChartsBezier2 {
 public:
  CQChartsBezier2() :
   p1_(), p2_(), p3_() {
  }

  CQChartsBezier2(double x1, double y1, double x2, double y2, double x3, double y3) :
   p1_(x1, y1), p2_(x2, y2), p3_(x3, y3) {
  }

  CQChartsBezier2(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2,
                  const CQChartsGeom::Point &p3) :
   p1_(p1), p2_(p2), p3_(p3) {
  }

  CQChartsBezier2(const CQChartsBezier2 &bezier) :
    p1_(bezier.p1_), p2_(bezier.p2_), p3_(bezier.p3_) {
  }

  CQChartsBezier2 &operator=(const CQChartsBezier2 &bezier) {
    p1_ = bezier.p1_;
    p2_ = bezier.p2_;
    p3_ = bezier.p3_;

    return *this;
  }

  const CQChartsGeom::Point &getFirstPoint  () const { return p1_; }
  const CQChartsGeom::Point &getControlPoint() const { return p2_; }
  const CQChartsGeom::Point &getLastPoint   () const { return p3_; }

  void setFirstPoint  (const CQChartsGeom::Point &p1) { p1_ = p1; }
  void setControlPoint(const CQChartsGeom::Point &p2) { p2_ = p2; };
  void setLastPoint   (const CQChartsGeom::Point &p3) { p3_ = p3; };

  void getFirstPoint  (double *x, double *y) const { *x = p1_.x; *y = p1_.y; }
  void getControlPoint(double *x, double *y) const { *x = p2_.x; *y = p2_.y; }
  void getLastPoint   (double *x, double *y) const { *x = p3_.x; *y = p3_.y; }

  void setFirstPoint  (double x, double y) { setFirstPoint  (CQChartsGeom::Point(x, y)); }
  void setControlPoint(double x, double y) { setControlPoint(CQChartsGeom::Point(x, y)); }
  void setLastPoint   (double x, double y) { setLastPoint   (CQChartsGeom::Point(x, y)); }

  void setPoints(double x1, double y1, double x2, double y2, double x3, double y3) {
    setPoints(CQChartsGeom::Point(x1, y1), CQChartsGeom::Point(x2, y2),
              CQChartsGeom::Point(x3, y3));
  }

  void setPoints(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2,
                 const CQChartsGeom::Point &p3) {
    p1_ = p1; p2_ = p2; p3_ = p3;
  }

  void calc(double t, double *x, double *y) const {
    CQChartsGeom::Point p;

    calc(t, p);

    *x = p.x;
    *y = p.y;
  }

  void calc(double t, CQChartsGeom::Point &p) const {
    p = calc(t);
  }

  CQChartsGeom::Point calc(double t) const {
    double u = (1.0 - t);

    double tt = t*t;
    double uu = u*u;

    return p1_*uu + 2.0*p2_*t*u + p3_*tt;
  }

  bool interp(double x, double y, double *t) const {
    return interp(CQChartsGeom::Point(x, y), t);
  }

  bool interp(const CQChartsGeom::Point &p, double *t) const {
    double t1 = (::fabs(p.x   - p1_.x) + ::fabs(p.y   - p1_.y))/
                (::fabs(p3_.x - p1_.x) + ::fabs(p3_.y - p1_.y));

    CQChartsGeom::Point pp;

    calc(t1, pp);

    double dx1 = ::fabs(p.x - pp.x);
    double dy1 = ::fabs(p.y - pp.y);

    while (dx1 > 1E-5 || dy1 > 1E-5) {
      if ((pp.x < p.x && pp.x < p3_.x) || (pp.x > p.x && pp.x > p3_.x)) {
        if (pp.x != p3_.x)
          t1 = (1.0 - t1)*(p.x - pp.x)/(p3_.x - pp.x) + t1;
        else
          t1 = 1.0;
      }
      else {
        if (pp.x != p1_.x)
          t1 = t1*(p.x - p1_.x)/(pp.x - p1_.x);
        else
          t1 = 0.0;
      }

      calc(t1, pp);

      double dx2 = ::fabs(p.x - pp.x);
      double dy2 = ::fabs(p.y - pp.y);

      if (dx2 < dx1 && dy2 < dy1)
        goto next;

      if ((pp.y < p.y && pp.y < p3_.y) || (pp.y > p.y && pp.y > p3_.y)) {
        if (pp.y != p3_.y)
          t1 = (1.0 - t1)*(p.y - pp.y)/(p3_.y - pp.y) + t1;
        else
          t1 = 1.0;
      }
      else {
        if (pp.y != p1_.y)
          t1 = t1*(p.y - p1_.y)/(pp.y - p1_.y);
        else
          t1 = 0.0;
      }

      calc(t1, pp);

      dx2 = ::fabs(p.x - pp.x);
      dy2 = ::fabs(p.y - pp.y);

      if (dx2 >= dx1 || dy2 >= dy1)
        return false;

   next:
      dx1 = dx2;
      dy1 = dy2;
    }

    *t = t1;

    return true;
  }

  double gradientStart() const {
    return atan2(p2_.y - p1_.y, p2_.x - p1_.x);
  }

  double gradientEnd() const {
    return atan2(p3_.y - p2_.y, p3_.x - p2_.x);
  }

  double gradient(double t) const {
    double u = 1.0 - t;

    CQChartsGeom::Point p = (p2_ - p1_)*u + (p3_ - p2_)*t;

    double g = atan2(p.y, p.x);

    return g;
  }

#if 0
  void getHullPolygon(std::vector<CQChartsGeom::Point> &points) const {
    points.push_back(p1_);
    points.push_back(p2_);
    points.push_back(p3_);
  }

  void getHullBBox(CQChartsGeom::BBox &bbox) const {
    bbox.reset();

    bbox.add(p1_);
    bbox.add(p2_);
    bbox.add(p3_);
  }
#endif

  void split(CQChartsBezier2 &bezier1, CQChartsBezier2 &bezier2) const {
    // split at control point
    CQChartsGeom::Point p12 = (p1_ + p2_)/2.0;
    CQChartsGeom::Point p23 = (p2_ + p3_)/2.0;

    CQChartsGeom::Point pm = (p12 + p23)/2.0;

    bezier1 = CQChartsBezier2(p1_, p12, pm );
    bezier2 = CQChartsBezier2(pm , p23, p3_);
  }

#if 0
  double arcLength(double tol=1E-3) const {
    double l1 = p1_.distanceTo(p3_);
    double l2 = p1_.distanceTo(p2_) + p2_.distanceTo(p3_);

    if (fabs(l2 - l1) < tol)
      return l1;

    CQChartsBezier2 bezier1, bezier2;

    split(bezier1, bezier2);

    return bezier1.arcLength(tol) + bezier2.arcLength(tol);
  }
#endif

#if 0
  void print(std::ostream &os) const {
    os << "[[" << p1_.x << ", " << p1_.y << "] [" <<
                  p2_.x << ", " << p2_.y << "] [" <<
                  p3_.x << ", " << p3_.y << "]]";
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsBezier2 &bezier) {
    bezier.print(os);

    return os;
  }
#endif

 private:
  CQChartsGeom::Point p1_, p2_, p3_;
};

//------

/*!
 * \brief Three Point Bezier
 * \ingroup Charts
 */
class CQChartsBezier3 {
 public:
  CQChartsBezier3() :
   p1_(), p2_(), p3_(), p4_() {
  }

  CQChartsBezier3(double x1, double y1, double x2, double y2,
             double x3, double y3, double x4, double y4) :
   p1_(x1, y1), p2_(x2, y2), p3_(x3, y3), p4_(x4, y4) {
  }

  CQChartsBezier3(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2,
                  const CQChartsGeom::Point &p3, const CQChartsGeom::Point &p4) :
   p1_(p1), p2_(p2), p3_(p3), p4_(p4) {
  }

  CQChartsBezier3(const CQChartsBezier3 &bezier) :
    p1_(bezier.p1_), p2_(bezier.p2_), p3_(bezier.p3_), p4_(bezier.p4_) {
  }

  // create order 3 bezier from order 2 using 'degree elevation'
  CQChartsBezier3(const CQChartsBezier2 &bezier2) {
    p1_ = bezier2.getFirstPoint();
    p4_ = bezier2.getLastPoint ();

    const CQChartsGeom::Point &p = bezier2.getControlPoint();

    p2_ = (p1_ + 2*p)/3;
    p3_ = (2*p + p4_)/3;
  }

  CQChartsBezier3 &operator=(const CQChartsBezier3 &bezier) {
    p1_ = bezier.p1_;
    p2_ = bezier.p2_;
    p3_ = bezier.p3_;
    p4_ = bezier.p4_;

    return *this;
  }

  const CQChartsGeom::Point &getFirstPoint   () const { return p1_; }
  const CQChartsGeom::Point &getControlPoint1() const { return p2_; }
  const CQChartsGeom::Point &getControlPoint2() const { return p3_; }
  const CQChartsGeom::Point &getLastPoint    () const { return p4_; }

  void setFirstPoint   (const CQChartsGeom::Point &p1) { p1_ = p1; }
  void setControlPoint1(const CQChartsGeom::Point &p2) { p2_ = p2; };
  void setControlPoint2(const CQChartsGeom::Point &p3) { p3_ = p3; };
  void setLastPoint    (const CQChartsGeom::Point &p4) { p4_ = p4; };

  void getFirstPoint   (double *x, double *y) const { *x = p1_.x; *y = p1_.y; }
  void getControlPoint1(double *x, double *y) const { *x = p2_.x; *y = p2_.y; }
  void getControlPoint2(double *x, double *y) const { *x = p3_.x; *y = p3_.y; }
  void getLastPoint    (double *x, double *y) const { *x = p4_.x; *y = p4_.y; }

  void setFirstPoint   (double x, double y) { setFirstPoint   (CQChartsGeom::Point(x, y)); }
  void setControlPoint1(double x, double y) { setControlPoint1(CQChartsGeom::Point(x, y)); }
  void setControlPoint2(double x, double y) { setControlPoint2(CQChartsGeom::Point(x, y)); }
  void setLastPoint    (double x, double y) { setLastPoint    (CQChartsGeom::Point(x, y)); }

  void setPoints(double x1, double y1, double x2, double y2,
                 double x3, double y3, double x4, double y4) {
    setPoints(CQChartsGeom::Point(x1, y1), CQChartsGeom::Point(x2, y2),
              CQChartsGeom::Point(x3, y3), CQChartsGeom::Point(x4, y4));
  }

  void setPoints(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2,
                 const CQChartsGeom::Point &p3, const CQChartsGeom::Point &p4) {
    p1_ = p1; p2_ = p2; p3_ = p3; p4_ = p4;
  }

  void getPoints(CQChartsGeom::Point &p1, CQChartsGeom::Point &p2,
                 CQChartsGeom::Point &p3, CQChartsGeom::Point &p4) const {
    p1 = p1_; p2 = p2_; p3 = p3_; p4 = p4_;
  }

  void calc(double t, double *x, double *y) const {
    CQChartsGeom::Point p;

    calc(t, p);

    *x = p.x;
    *y = p.y;
  }

  void calc(double t, CQChartsGeom::Point &p) const {
    p = calc(t);
  }

  CQChartsGeom::Point calc(double t) const {
    double u = (1.0 - t);

    double tt  = t*t;
    double ttt = tt*t;

    double uu  = u*u;
    double uuu = uu*u;

    return p1_*uuu + 3.0*p2_*t*uu + 3.0*p3_*tt*u + p4_*ttt;
  }

  bool interp(double x, double y, double *t) const {
    return interp(CQChartsGeom::Point(x, y), t);
  }

  bool interp(const CQChartsGeom::Point &p, double *t) const {
    double t1 = (::fabs(p.x   - p1_.x) + ::fabs(p.y   - p1_.y))/
                (::fabs(p4_.x - p1_.x) + ::fabs(p4_.y - p1_.y));

    CQChartsGeom::Point pp;

    calc(t1, pp);

    double dx1 = ::fabs(p.x - pp.x);
    double dy1 = ::fabs(p.y - pp.y);

    while (dx1 > 1E-5 || dy1 > 1E-5) {
      if ((pp.x < p.x && pp.x < p4_.x) || (pp.x > p.x && pp.x > p4_.x)) {
        if (pp.x != p4_.x)
          t1 = (1.0 - t1)*(p.x - pp.x)/(p4_.x - pp.x ) + t1;
        else
          t1 = 1.0;
      }
      else {
        if (pp.x != p1_.x)
          t1 = t1*(p.x - p1_.x)/(pp.x - p1_.x);
        else
          t1 = 0.0;
      }

      calc(t1, pp);

      double dx2 = ::fabs(p.x - pp.x);
      double dy2 = ::fabs(p.y - pp.y);

      if (dx2 < dx1 && dy2 < dy1)
        goto next;

      if ((pp.y < p.y && pp.y < p4_.y) || (pp.y > p.y && pp.y > p4_.y)) {
        if (pp.y != p4_.y)
          t1 = (1.0 - t1)*(p.y - pp.y )/(p4_.y - pp.y) + t1;
        else
          t1 = 1.0;
      }
      else {
        if (pp.y != p1_.y)
          t1 = t1*(p.y - p1_.y)/(pp.y - p1_.y);
        else
          t1 = 0.0;
      }

      calc(t1, pp);

      dx2 = ::fabs(p.x - pp.x);
      dy2 = ::fabs(p.y - pp.y);

      if (dx2 >= dx1 || dy2 >= dy1)
        return false;

   next:
      dx1 = dx2;
      dy1 = dy2;
    }

    *t = t1;

    return true;
  }

  double gradientStart() const {
    return atan2(p2_.y - p1_.y, p2_.x - p1_.x);
  }

  double gradientEnd() const {
    return atan2(p4_.y - p3_.y, p4_.x - p3_.x);
  }

  double gradient(double t) const {
    double u = 1.0 - t;

    double tt = t*t;
    double uu = u*u;
    double tu = t*u;

    CQChartsGeom::Point p = (p2_ - p1_)*uu + 2.0*(p3_ - p2_)*tu + (p4_ - p3_)*tt;

    double g = atan2(p.y, p.x);

    return g;
  }

#if 0
  void getHullPolygon(std::vector<CQChartsGeom::Point> &points) const {
    points.push_back(p1_);
    points.push_back(p2_);
    points.push_back(p3_);
    points.push_back(p4_);
  }

  void getHullBBox(CQChartsGeom::BBox &bbox) const {
    bbox.reset();

    bbox.add(p1_);
    bbox.add(p2_);
    bbox.add(p3_);
    bbox.add(p4_);
  }
#endif

  void split(CQChartsBezier3 &bezier1, CQChartsBezier3 &bezier2) const {
    split(0.5, bezier1, bezier2);
  }

  void split(double t, CQChartsBezier3 &bezier1, CQChartsBezier3 &bezier2) const {
    // split at t (0 - 1) of curve
    double u = 1.0 - t;

    CQChartsGeom::Point p11 = u*p1_ + t*p2_;
    CQChartsGeom::Point p12 = u*p2_ + t*p3_;
    CQChartsGeom::Point p13 = u*p3_ + t*p4_;

    CQChartsGeom::Point p21 = u*p11 + t*p12;
    CQChartsGeom::Point p22 = u*p12 + t*p13;

    CQChartsGeom::Point p31 = u*p21 + t*p22;

    bezier1 = CQChartsBezier3(p1_, p11, p21, p31);
    bezier2 = CQChartsBezier3(p31, p22, p13, p4_);
  }

  bool split(const CQChartsGeom::Point &p, CQChartsBezier3 &bezier1,
             CQChartsBezier3 &bezier2) const {
    double t;

    if (! interp(p, &t)) return false;

    split(t, bezier1, bezier2);

    return true;
  }

#if 0
  CQChartsGeom::Point deCasteljauInterp(double t) const {
    double u = 1.0 - t;

    CQChartsGeom::Point p11 = u*p1_ + t*p2_;
    CQChartsGeom::Point p12 = u*p2_ + t*p3_;
    CQChartsGeom::Point p13 = u*p3_ + t*p4_;

    CQChartsGeom::Point p21 = u*p11 + t*p12;
    CQChartsGeom::Point p22 = u*p12 + t*p13;

    CQChartsGeom::Point p31 = u*p21 + t*p22;

    return p31;
  }
#endif

#if 0
  template<typename FUNC>
  static CQChartsBezier3 bestFit(FUNC f, double x1, double y1, double g1,
                                 double x2, double y2, double g2, int steps=50) {
    double c1 = y1 - g1*x1;
    double c2 = y2 - g2*x2;

    double y1e = g1*x2 + c1;
    double y2e = g2*x1 + c2;

    double s = 1.0/steps;

    double x11 = x1;
    double y11 = y1;

    double x21 = x2;
    double y21 = y2;

    int minI1 = -1;
    int minI2 = -1;

    bool changed1 = true;
    bool changed2 = true;

    int max_iter = 100;

    while (max_iter && (changed1 || changed2)) {
      int    oldI1 = minI1;
      double minD1 = 1E50;

      for (int i = 0; i <= 2*steps; ++i) {
        double x11 = (x2  - x1)*i*s + x1;
        double y11 = (y1e - y1)*i*s + y1;

        CQChartsBezier3 b(x1, y1, x11, y11, x21, y21, x2, y2);

        double bx, by;

        b.calc(0.33, &bx, &by);

        double d = fabs(by - f(bx));

        if (d < minD1) {
          minD1 = d;
          minI1 = i;
        }
        else
          break;
      }

      x11 = (x2  - x1)*minI1*s + x1;
      y11 = (y1e - y1)*minI1*s + y1;

      changed1 = (minI1 != oldI1);

      //----

      int    oldI2 = minI2;
      double minD2 = 1E50;

      for (int i = 0; i <= 2*steps; ++i) {
        double x21 = (x1  - x2)*i*s + x2;
        double y21 = (y2e - y2)*i*s + y2;

        CQChartsBezier3 b(x1, y1, x11, y11, x21, y21, x2, y2);

        double bx, by;

        b.calc(0.66, &bx, &by);

        double d = fabs(by - f(bx));

        if (d < minD2) {
          minD2 = d;
          minI2 = i;
        }
        else
          break;
      }

      x21 = (x1  - x2)*minI2*s + x2;
      y21 = (y2e - y2)*minI2*s + y2;

      changed2 = (minI2 != oldI2);

      --max_iter;
    }

    return CQChartsBezier3(x1, y1, x11, y11, x21, y21, x2, y2);
  }
#endif

#if 0
  template<typename FUNC>
  static CQChartsBezier3 bestParamFit(FUNC f, double t1=0, double t2=1, int steps=50) {
    assert(t2 > t1);

    CQChartsGeom::Point p1 = f(t1);
    CQChartsGeom::Point p2 = f(t1 + 0.01);
    CQChartsGeom::Point p3 = f(t2 - 0.01);
    CQChartsGeom::Point p4 = f(t2);

    double x1 = p1.x, y1 = p1.y;
    double x2 = p4.x, y2 = p4.y;

    double g1 = (p2.y - p1.y)/(p2.x - p1.x);
    double g2 = (p4.y - p3.y)/(p4.x - p3.x);

    double x11 = x1, y11 = y1;
    double x21 = x2, y21 = y2;

    double dx = (x2 - x1)/(steps - 1);

    int minI1 = -1;
    int minI2 = -1;

    bool changed1 = true;
    bool changed2 = true;

    int max_iter = 100;

    while (max_iter && (changed1 || changed2)) {
      int    oldI1 = minI1;
      double minD1 = 1E50;

      for (int i = 1; i < 2*steps; ++i) {
        double x11 = i*dx + x1;
        double y11 = g1*(x11 - x1) + y1;

        CQChartsBezier3 b(x1, y1, x11, y11, x21, y21, x2, y2);

        double bx, by;

        double it = 0.33*(t2 - t1) + t1;

        b.calc(it, &bx, &by);

        CQChartsGeom::Point pi = f(it);

        double d = std::max(fabs(bx - pi.x), fabs(by - pi.y));

        if (d < minD1) {
          minD1 = d;
          minI1 = i;
        }
      }

      x11 = minI1*dx + x1;
      y11 = g1*(x11 - x1) + y1;

      changed1 = (minI1 != oldI1);

      //----

      int    oldI2 = minI2;
      double minD2 = 1E50;

      for (int i = steps - 1; i > -steps; --i) {
        double x21 = i*dx + x1;
        double y21 = g2*(x21 - x2) + y2;

        CQChartsBezier3 b(x1, y1, x11, y11, x21, y21, x2, y2);

        double bx, by;

        double it = 0.66*(t2 - t1) + t1;

        b.calc(it, &bx, &by);

        CQChartsGeom::Point pi = f(it);

        double d = std::max(fabs(bx - pi.x), fabs(by - pi.y));

        if (d < minD2) {
          minD2 = d;
          minI2 = i;
        }
      }

      x21 = minI2*dx + x1;
      y21 = g2*(x21 - x2) + y2;

      changed2 = (minI2 != oldI2);

      --max_iter;
    }

    return CQChartsBezier3(x1, y1, x11, y11, x21, y21, x2, y2);
  }
#endif

#if 0
  template<typename FUNC>
  static CQChartsBezier3 bestPolarFit(FUNC f, double a1, double r1, double g1,
                                      double a2, double r2, double g2, int steps=50) {
    double x1 = r1*cos(a1);
    double y1 = r1*sin(a1);

    double x2 = r2*cos(a2);
    double y2 = r2*sin(a2);

    double c1 = y1 - g1*x1;
    double c2 = y2 - g2*x2;

    double dx = (x2 - x1)/steps;
    double dy = (y2 - y1)/steps;

    double x11, y11, x21, y21;

    bool use_dx = (fabs(g1) < 4 && fabs(g2) < 4);
    //bool use_dx = false;
    //bool use_dx = true;

    if (use_dx) {
      x11 = x1 + dx; y11 = g1*x11 + c1;
      x21 = x2 - dx; y21 = g2*x21 + c2;
    }
    else {
      y11 = y1 + dy; x11 = (y11 - c1)/g1;
      y21 = y2 - dy; x21 = (y21 - c2)/g2;
    }

    int minI1 = -1;
    int minI2 = -1;

    bool changed1 = true;
    bool changed2 = true;

    while (changed1 || changed2) {
      int    oldI1 = minI1;
      double minD1 = 1E50;

      for (int i = 1; i < steps; ++i) {
        double x11, y11;

        if (use_dx) {
          x11 = x1 + i*dx;
          y11 = g1*x11 + c1;
        }
        else {
          y11 = y1 + i*dy;
          x11 = (y11 - c1)/g1;
        }

        CQChartsBezier3 b(x1, y1, x11, y11, x21, y21, x2, y2);

        double bx, by;

        b.calc(0.33, &bx, &by);

        double ba = a1 + 0.33*(a2 - a1);
        double br = f(ba);

        double d;

        if  (use_dx)
          d = fabs(by - br*sin(ba));
        else
          d = fabs(bx - br*cos(ba));

        if (d < minD1) {
          minD1 = d;
          minI1 = i;
        }
      }

      if  (use_dx) {
        x11 = x1 + minI1*dx;
        y11 = g1*x11 + c1;
      }
      else {
        y11 = y1 + minI1*dy;
        x11 = (y11 - c1)/g1;
      }

      changed1 = (minI1 != oldI1);

      //----

      int    oldI2 = minI2;
      double minD2 = 1E50;

      for (int i = 1; i < steps; ++i) {
        double x21, y21;

        if (use_dx) {
          x21 = x2 - i*dx;
          y21 = g2*x21 + c2;
        }
        else {
          y21 = y2 - i*dy;
          x21 = (y21 - c2)/g2;
        }

        CQChartsBezier3 b(x1, y1, x11, y11, x21, y21, x2, y2);

        double bx, by;

        b.calc(0.66, &bx, &by);

        double ba = a1 + 0.66*(a2 - a1);
        double br = f(ba);

        double d;

        if  (use_dx)
          d = fabs(by - br*sin(ba));
        else
          d = fabs(bx - br*cos(ba));

        if (d < minD2) {
          minD2 = d;
          minI2 = i;
        }
      }

      if  (use_dx) {
        x21 = x2 - minI2*dx;
        y21 = g2*x21 + c2;
      }
      else {
        y21 = y2 - minI2*dy;
        x21 = (y21 - c2)/g2;
      }

      changed2 = (minI2 != oldI2);
    }

    return CQChartsBezier3(x1, y1, x11, y11, x21, y21, x2, y2);
  }
#endif

#if 0
  double arcLength(double tol=1E-3) const {
    double l1 = p1_.distanceTo(p4_);
    double l2 = p1_.distanceTo(p2_) + p2_.distanceTo(p3_) + p3_.distanceTo(p4_);

    if (fabs(l2 - l1) < tol)
      return l1;

    CQChartsBezier3 bezier1, bezier2;

    split(bezier1, bezier2);

    return bezier1.arcLength(tol) + bezier2.arcLength(tol);
  }
#endif

#if 0
  void print(std::ostream &os) const {
    os << "[[" << p1_.x << ", " << p1_.y << "] [" <<
                  p2_.x << ", " << p2_.y << "] [" <<
                  p3_.x << ", " << p3_.y << "] [" <<
                  p4_.x << ", " << p4_.y << "]]";
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsBezier3 &bezier) {
    bezier.print(os);

    return os;
  }
#endif

 private:
  CQChartsGeom::Point p1_, p2_, p3_, p4_;
};

//------

//! smooth curve through set of points
class CQChartsSmooth {
 public:
  using Points = std::vector<CQChartsGeom::Point>;

  enum class SegmentType {
    NONE,
    LINE,
    CURVE2,
    CURVE3
  };

 private:
  class Segment {
   public:
    Segment() { }

    virtual ~Segment() { }

    virtual SegmentType type() const = 0;

    virtual bool inside(const CQChartsSmooth *smooth, double x) const = 0;

    virtual double interp(const CQChartsSmooth *smooth, double x) const = 0;

    virtual CQChartsGeom::Point controlPoint1(const CQChartsSmooth *smooth) const = 0;
    virtual CQChartsGeom::Point controlPoint2(const CQChartsSmooth *smooth) const = 0;
  };

  //---

  class Line : public Segment {
   public:
    Line(int i) : i_(i) { }

    SegmentType type() const override { return SegmentType::LINE; }

    bool inside(const CQChartsSmooth *smooth, double x) const override {
      const CQChartsGeom::Point &p1 = smooth->point(i_    );
      const CQChartsGeom::Point &p2 = smooth->point(i_ + 1);

      return (x >= p1.x && x <= p2.x);
    }

    double interp(const CQChartsSmooth *smooth, double x) const override {
      const CQChartsGeom::Point &p1 = smooth->point(i_    );
      const CQChartsGeom::Point &p2 = smooth->point(i_ + 1);

      if (p1.x == p2.x) return p1.y;

      double m = (x - p1.x)/(p2.x - p1.x);
      double y = (1 - m)*p1.y + m*p2.y;

      return y;
    }

    CQChartsGeom::Point controlPoint1(const CQChartsSmooth *smooth) const override {
      return smooth->point(i_    ); }
    CQChartsGeom::Point controlPoint2(const CQChartsSmooth *smooth) const override {
      return smooth->point(i_ + 1); }

   private:
    int i_ { -1 };
  };

  //---

  class Curve2 : public Segment {
   public:
    Curve2(int i, const CQChartsGeom::Point &mp) : i_(i), mp_(mp) { }

    SegmentType type() const override { return SegmentType::CURVE2; }

    bool inside(const CQChartsSmooth *smooth, double x) const override {
      const CQChartsGeom::Point &p1 = smooth->point(i_    );
      const CQChartsGeom::Point &p2 = smooth->point(i_ + 1);

      return (x >= p1.x && x <= p2.x);
    }

    double interp(const CQChartsSmooth *smooth, double x) const override {
      const CQChartsGeom::Point &p1 = smooth->point(i_    );
      const CQChartsGeom::Point &p3 = smooth->point(i_ + 1);

      CQChartsBezier2 bezier(p1, mp_, p3);

      double t = (x - p1.x)/(p3.x - p1.x);

      CQChartsGeom::Point pi;

      bezier.calc(t, pi);

      return pi.y;
    }

    CQChartsGeom::Point controlPoint1(const CQChartsSmooth *) const override { return mp_; }
    CQChartsGeom::Point controlPoint2(const CQChartsSmooth *) const override { return mp_; }

   private:
    int                 i_ { -1 };
    CQChartsGeom::Point mp_;
  };

  //---

  class Curve3 : public Segment {
   public:
    Curve3(int i, const CQChartsGeom::Point &mp1, const CQChartsGeom::Point &mp2) :
     i_(i), mp1_(mp1), mp2_(mp2) { }

    SegmentType type() const override { return SegmentType::CURVE3; }

    bool inside(const CQChartsSmooth *smooth, double x) const override {
      const CQChartsGeom::Point &p1 = smooth->point(i_    );
      const CQChartsGeom::Point &p2 = smooth->point(i_ + 1);

      return (x >= p1.x && x <= p2.x);
    }

    double interp(const CQChartsSmooth *smooth, double x) const override {
      const CQChartsGeom::Point &p1 = smooth->point(i_    );
      const CQChartsGeom::Point &p3 = smooth->point(i_ + 1);

      CQChartsBezier3 bezier(p1, mp1_, mp2_, p3);

      double t = (x - p1.x)/(p3.x - p1.x);

      CQChartsGeom::Point pi;

      bezier.calc(t, pi);

      return pi.y;
    }

    CQChartsGeom::Point controlPoint1(const CQChartsSmooth *) const override { return mp1_; }
    CQChartsGeom::Point controlPoint2(const CQChartsSmooth *) const override { return mp2_; }

   private:
    int                 i_ { -1 };
    CQChartsGeom::Point mp1_;
    CQChartsGeom::Point mp2_;
  };

  //---

 public:
  CQChartsSmooth(const Points &points=Points(), bool sorted=true) :
   points_(points), sorted_(sorted) {
    reset();

    smooth();
  }

 ~CQChartsSmooth() {
    reset();
  }

  //---

  void clearPoints() {
    points_.clear();

    reset();
  }

  void setPoints(const Points &points) {
    points_ = points;

    reset();

    smooth();
  }

  void addPoint(const CQChartsGeom::Point &p) {
    points_.push_back(p);

    reset();

    smooth();
  }

  //---

  void sort() {
    std::sort(points_.begin(), points_.end(),
              [](const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2) {
                return p1.x < p2.x; });
  }

  //---

  double interp(double x) {
    if (segments_.empty()) {
      // single point
      if (points_.size() == 1)
        return points_[0].y;
      // assert ?
      else
        return 0;
    }

    //---

    // find segment for x (must be monotonic ?)
    for (auto &s : segments_) {
      if (s->inside(this, x))
        return s->interp(this, x);
    }

    if (x <= points_[0].x)
      return segments_.front()->interp(this, x);
    else
      return segments_.back ()->interp(this, x);
  }

  const Points &points() { return points_; }

  int numPoints() const { return points_.size(); }

  const CQChartsGeom::Point &point(int i) const { return points_[i]; }

  SegmentType segmentType(int i) const { return segments_[i]->type(); }

  CQChartsGeom::Point controlPoint1(int i) const {
    return segments_[i]->controlPoint1(this);
  }

  CQChartsGeom::Point controlPoint2(int i) const {
    return segments_[i]->controlPoint2(this);
  }

 private:
  void reset() {
    for (auto &s : segments_)
      delete s;

    segments_.clear();
  }

  void smooth() {
    if (sorted_)
      sort();

    if (points_.size() < 2)
      return;

    if (points_.size() == 2) {
      segments_.push_back(new Line(0));
      return;
    }

    // generate beziers for each point pair
    double g1 = 0.0, c1 = 0.0, g2, c2;

    for (int i1 = 0, i2 = 1, i3 = 2; i3 < int(points_.size()); i1 = i2, i2 = i3++) {
      if (points_[i1].x == points_[i2].x || points_[i2].x == points_[i3].x) {
        segments_.push_back(new Line(i1));
        continue;
      }

      double gl1 = (points_[i2].y - points_[i1].y)/(points_[i2].x - points_[i1].x);
      double gl2 = (points_[i3].y - points_[i2].y)/(points_[i3].x - points_[i2].x);

      g2 = gl1 + gl2;
      c2 = points_[i2].y - points_[i2].x*g2;

      if (i1 > 0) {
        double dx = (points_[i2].x - points_[i1].x)/3;

        double mx1 = points_[i1].x + dx;
        double mx2 = points_[i2].x - dx;

        CQChartsGeom::Point mp1(mx1, g1*mx1 + c1);
        CQChartsGeom::Point mp2(mx2, g2*mx2 + c2);

        segments_.push_back(new Curve3(i1, mp1, mp2));
      }
      else {
        double dx = (points_[i2].x - points_[i1].x)/3;

        double mx = points_[i2].x - dx;

        CQChartsGeom::Point mp(mx, g2*mx + c2);

        segments_.push_back(new Curve2(i1, mp));
      }

      g1 = g2;
      c1 = c2;
    }

    // add last segment
    int i1 = points_.size() - 3;
    int i2 = i1 + 1;
    int i3 = i2 + 1;

    if (points_[i1].x == points_[i2].x || points_[i2].x == points_[i3].x) {
      segments_.push_back(new Line(i2));
      return;
    }

    double gl1 = (points_[i2].y - points_[i1].y)/(points_[i2].x - points_[i1].x);
    double gl2 = (points_[i3].y - points_[i2].y)/(points_[i3].x - points_[i2].x);

    double g = gl1 + gl2;
    double c = points_[i2].y - points_[i2].x*g;

    double mx = (points_[i2].x + points_[i3].x)/2;

    CQChartsGeom::Point mp(mx, g*mx + c);

    segments_.push_back(new Curve2(i2, mp));
  }

 private:
  using Segments = std::vector<Segment *>;

  Points   points_;
  Segments segments_;
  bool     sorted_ { true };
};

#endif
