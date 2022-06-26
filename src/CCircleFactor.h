#ifndef CCircleFactor_H
#define CCircleFactor_H

#include <vector>
#include <cmath>

namespace CCircleFactor {

//------

class CircleMgr;
class Circle;

using Circles = std::vector<Circle *>;

//----

struct Point {
  double x { 0.0 };
  double y { 0.0 };

  Point() { }

  Point(double x, double y) :
   x(x), y(y) {
  }

  friend Point &operator+=(Point &lhs, const Point &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;

    return lhs;
  }
};

using Points = std::vector<Point>;

//---

struct CirclePoint {
  const Circle *circle { nullptr };
  Point         point;

  CirclePoint(const Circle *c, const Point &p) :
   circle(c), point(p) {
  }
};

using CirclePoints = std::vector<CirclePoint>;

//---

class CircleMgr {
 public:
  using Factors = std::vector<int>;

 public:
  CircleMgr();

  virtual ~CircleMgr() { }

  //---

  std::size_t lastId() { return lastId_; }

  void resetLastId() { lastId_ = 0; }

  void incLastId(std::size_t d) { lastId_ += d; }

  //---

  int factor() const { return factor_; }
  void setFactor(int i) { factor_ = i; }

  const Factors &factors() const { return factors_; }

  //---

  double s() const { return s_; }
  void setS(double s, double maxS) { s_ = s; maxS_ = maxS; }

  double maxS() const { return maxS_; }

  //---

  const Point &center() const { return center_; }
  void setCenter(const Point &c);

  //---

  bool isDebug() const { return debug_; }
  void setDebug(bool debug) { debug_ = debug; }

  //---

  void reset();

  void calc();

  void generate(double w, double h);

  Circle *makeCircle();

  Circle *makeCircle(Circle *parent, std::size_t n);

  //---

  virtual void addDrawCircle(double xc, double yc, double size, double f) = 0;

  virtual void addDebugCircle(double /*xc*/, double /*yc*/, double /*size*/,
                              double /*strokeAlpha*/, double /*fillAlpha*/) { }

 private:
  void calcFactors(Circle *circle, const Factors &f);
  void calcPrime  (Circle *circle, int n);

 private:
  int         factor_ { 1 };
  Circle*     circle_ { nullptr };
  Factors     factors_;
  double      s_      { 1.0 };
  double      maxS_   { 1.0 };
  std::size_t lastId_ { 0 };
  Point       center_ { 0.5, 0.5 };
  bool        debug_  { false };

  Point  pos_;
  double size_   { 1.0 };
};

//---

// class for single circle or collection of child circles
class Circle {
 public:
  friend class CircleMgr;

 public:
  Circle(CircleMgr *mgr);
  Circle(Circle *parent, std::size_t n);

 ~Circle();

  CircleMgr *mgr() const { return mgr_; }

  Circle *parent() const { return parent_; }

  std::size_t id() const { return id_; }
  void setId(std::size_t n);

  std::size_t n() const { return n_; }

  double x() const { return c_.x; }
  double y() const { return c_.y; }
  double r() const { return r_; }

  double a() const { return a_; }
  void setA(double a) { a_ = a; }

  double xc() const { return xc_; }
  double yc() const { return yc_; }

  void addCircle(Circle *circle);

  void addPoint();

  void place();

  //double calcR() const;

  void fit();

  void move  (double x, double y);
  void moveBy(double dx, double dy);

  double closestCircleCircleDistance() const;

  double closestPointDistance() const;

  double closestSize() const;

  std::size_t size() const;

  Point center() const;

  void getPoints(Points &points) const;

  void getCirclePoints(CirclePoints &points) const;

  std::size_t numPoints() const { return points_.size(); }

  Point getPoint(int i) const;
  void setPoint(int i, const Point &p) { points_[size_t(i)] = p; }

  void generate(const Point &pos, double size);

 private:
  CircleMgr*  mgr_    { nullptr };   // manager
  Circle*     parent_ { nullptr };   // parent circle (null if none)
  std::size_t id_     { 0 };         // index (for color)
  std::size_t n_      { 0 };         // index in parent
  Point       c_;                    // center (0->1 (screen size))
  double      r_      { 0.5 };       // radius
  double      a_      { -M_PI/2.0 }; // angle
  Points      points_;               // offset from center (0-1)
  Circles     circles_;              // sub circles
  double      xc_     { 0.0 };
  double      yc_     { 0.0 };
};

//---

}

#endif
