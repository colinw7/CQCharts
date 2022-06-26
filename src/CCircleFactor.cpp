#include <CCircleFactor.h>
#include <CPrime.h>

#include <cmath>
#include <cassert>

namespace CCircleFactor {

//---

CircleMgr::
CircleMgr()
{
}

void
CircleMgr::
setCenter(const Point &c)
{
  center_ = c;
}

void
CircleMgr::
reset()
{
  delete circle_;

  circle_ = nullptr;
}

void
CircleMgr::
calc()
{
  resetLastId();

  reset();

  factors_ = CPrime::factors(factor_);

  circle_ = makeCircle();

  if (CPrime::isPrime(factor_))
    calcPrime(circle_, factor_);
  else
    calcFactors(circle_, factors_);

  circle_->place();

  circle_->fit();
}

void
CircleMgr::
generate(double w, double h)
{
  // position in unit circle, centered at 0.5, 0.5
  double xc = circle_->xc();
  double yc = circle_->yc();

  pos_  = Point(center_.x + (xc - 0.5)*w, center_.y + (0.5 - yc)*h);
  size_ = std::min(w, h);

  circle_->generate(pos_, size_);
}

void
CircleMgr::
calcFactors(Circle *circle, const Factors &f)
{
  if (f.size() == 1) {
    calcPrime(circle, f[0]);
    return;
  }

  //------

  // split into first factor and list of remaining factors
  auto pf = f.begin();

  auto n1 = *pf++;

  Factors f1;

  std::copy(pf, f.end(), std::back_inserter(f1));

  //------

  // add n circles
  for (int i = 0; i < n1; ++i) {
    auto *circle1 = makeCircle(circle, size_t(i));

    circle->addCircle(circle1);

    calcFactors(circle1, f1);
  }
}

void
CircleMgr::
calcPrime(Circle *circle, int n)
{
  assert(n > 0);

  // reserve n ids
  circle->setId(size_t(n));

  // add n points
  for (int i = 0; i < n; ++i)
    circle->addPoint();
}

Circle *
CircleMgr::
makeCircle()
{
  return new Circle(this);
}

Circle *
CircleMgr::
makeCircle(Circle *parent, std::size_t n)
{
  return new Circle(parent, n);
}

//------

Circle::
Circle(CircleMgr *mgr) :
 mgr_(mgr)
{
}

Circle::
Circle(Circle *parent, std::size_t n) :
 mgr_(parent->mgr()), parent_(parent), n_(n)
{
}

Circle::
~Circle()
{
  for (auto &circle : circles_)
    delete circle;
}

void
Circle::
setId(std::size_t n)
{
  id_ = mgr()->lastId();

  mgr()->incLastId(n);
}

void
Circle::
addCircle(Circle *circle)
{
  circles_.push_back(circle);
}

void
Circle::
addPoint()
{
  points_.emplace_back(0.0, 0.0);
}

void
Circle::
place()
{
  if (! circles_.empty()) {
    auto nc = circles_.size();

    double da = 2.0*M_PI/double(nc);

    // place child circles
    double a = a_;

    for (auto &circle : circles_) {
      if (size() == 2 && circle->size() == 2)
        circle->setA(a + M_PI/2.0);
      else
        circle->setA(a);

      circle->place();

      a += da;
    }

    // find minimum point distance for child circles
    double d = 1E50;

    for (auto &circle : circles_) {
      d = std::min(d, circle->closestPointDistance());
    }

    double rr = d/2.0;

    // place in circle (center (0.5, 0.5), radius 0.5)
    c_ = Point(0.5, 0.5);
    r_ = 0.5;

    for (;;) {
      a = a_;

      for (auto &circle : circles_) {
        double x1 = x() + r_*std::cos(a);
        double y1 = y() + r_*std::sin(a);

        circle->move(x1, y1);

        a += da;
      }

      double r1 = closestCircleCircleDistance()/2;

      double dr = fabs(r1 - rr);

      if (dr < 1E-3)
        break;

      if (r1 < rr)
        r_ += dr/2;
      else
        r_ -= dr/2;
    }
  }
  else {
    auto np = numPoints();

    c_ = Point(0.5, 0.5);
    r_ = 0.5;

    // place points in circle
    if (np > 1) {
      double a  = a_;
      double da = 2.0*M_PI/double(np);

      for (std::size_t i = 0; i < np; ++i) {
        double x1 = std::cos(a);
        double y1 = std::sin(a);

        setPoint(int(i), Point(x1, y1));

        a += da;
      }
    }
    else {
      setPoint(0, Point(0.0, 0.0));
    }
  }
}

#if 0
double
Circle::
calcR() const
{
  if (parent_) {
    if (parent_->size() == 2 && size() == 2)
      return parent_->calcR();
    else
      return 0.5*parent_->calcR();
  }
  else
    return 0.5;
}
#endif

void
Circle::
fit()
{
  // get all points
  Points points;

  getPoints(points);

  // calc closest centers and range
  auto np = points.size();

  double xmin = 0.5;
  double ymin = 0.5;
  double xmax = xmin;
  double ymax = ymin;

  double d = 2;

  for (std::size_t i = 0; i < np; ++i) {
    const Point &p1 = points[i];

    for (std::size_t j = i + 1; j < np; ++j) {
      assert(i != j);

      const Point &p2 = points[j];

      double dx = p1.x - p2.x;
      double dy = p1.y - p2.y;

      double d1 = dx*dx + dy*dy;

      if (d1 < d)
        d = d1;
    }

    xmin = std::min(xmin, p1.x);
    ymin = std::min(ymin, p1.y);
    xmax = std::max(xmax, p1.x);
    ymax = std::max(ymax, p1.y);
  }

  //---

  // use closest center to defined size so points don't touch
  double s = 0.0;

  if (d > 1E-6)
    s = sqrt(d);
  else
    s = 1.0/double(np);

  xmin -= s/2.0;
  ymin -= s/2.0;
  xmax += s/2.0;
  ymax += s/2.0;

  double xs = xmax - xmin;
  double ys = ymax - ymin;

  double maxS = std::max(xs, ys);

  mgr()->setS(s, maxS);

  xc_ = ((xmax + xmin)/2.0 - 0.5)/maxS + 0.5;
  yc_ = ((ymax + ymin)/2.0 - 0.5)/maxS + 0.5;

  //c_ += Point(xc_ - 0.5, yc_ - 0.5);

  //moveBy(0.5 - xc_, 0.5 - yc_);
}

double
Circle::
closestCircleCircleDistance() const
{
  // get all points
  CirclePoints points;

  getCirclePoints(points);

  // calc closest centers and range
  auto np = points.size();

  double d = 1E50;

  for (std::size_t i = 0; i < np; ++i) {
    const CirclePoint &p1 = points[i];

    for (std::size_t j = i + 1; j < np; ++j) {
      const CirclePoint &p2 = points[j];

      if (p1.circle == p2.circle) continue;

      double dx = p1.point.x - p2.point.x;
      double dy = p1.point.y - p2.point.y;

      double d1 = dx*dx + dy*dy;

      if (d1 < d)
        d = d1;
    }
  }

  return sqrt(d);
}

double
Circle::
closestPointDistance() const
{
  // get all points
  Points points;

  getPoints(points);

  // calc closest centers and range
  auto np = points.size();

  double d = 1E50;

  for (std::size_t i = 0; i < np; ++i) {
    const Point &p1 = points[i];

    for (std::size_t j = i + 1; j < np; ++j) {
      assert(i != j);

      const Point &p2 = points[j];

      double dx = p1.x - p2.x;
      double dy = p1.y - p2.y;

      double d1 = dx*dx + dy*dy;

      if (d1 < d)
        d = d1;
    }
  }

  return sqrt(d);
}

double
Circle::
closestSize() const
{
  double d = 1E50;

  if (! circles_.empty()) {
    auto nc = circles_.size();

    for (std::size_t i = 0; i < nc; ++i) {
      Circle *c1 = circles_[i];

      Point p1 = c1->center();

      for (std::size_t j = i + 1; j < nc; ++j) {
        assert(i != j);

        Circle *c2 = circles_[j];

        Point p2 = c2->center();

        double dx = p1.x - p2.x;
        double dy = p1.y - p2.y;

        double d1 = dx*dx + dy*dy;

        if (d1 < d)
          d = d1;
      }
    }
  }
  else {
    auto np = points_.size();

    for (std::size_t i = 0; i < np; ++i) {
      const Point &p1 = points_[i];

      for (std::size_t j = i + 1; j < np; ++j) {
        assert(i != j);

        const Point &p2 = points_[j];

        double dx = p1.x - p2.x;
        double dy = p1.y - p2.y;

        double d1 = dx*dx + dy*dy;

        if (d1 < d)
          d = d1;
      }
    }
  }

  d = sqrt(d);

  return d;
}

std::size_t
Circle::
size() const
{
  return std::max(circles_.size(), points_.size());
}

Point
Circle::
center() const
{
  return c_;
}

void
Circle::
getPoints(Points &points) const
{
  for (auto &circle : circles_)
    circle->getPoints(points);

  auto np = numPoints();

  for (std::size_t i = 0; i < np; ++i)
    points.push_back(getPoint(int(i)));
}

void
Circle::
getCirclePoints(CirclePoints &points) const
{
  for (auto &circle : circles_)
    circle->getCirclePoints(points);

  auto np = numPoints();

  for (std::size_t i = 0; i < np; ++i)
    points.emplace_back(this, getPoint(int(i)));
}

Point
Circle::
getPoint(int i) const
{
  return Point(x() + r_*points_[size_t(i)].x, y() + r_*points_[size_t(i)].y);
}

void
Circle::
move(double x, double y)
{
  double dx = x - this->x();
  double dy = y - this->y();

  moveBy(dx, dy);
}

void
Circle::
moveBy(double dx, double dy)
{
  c_ += Point(dx, dy);

  for (auto &circle : circles_)
    circle->moveBy(dx, dy);
}

void
Circle::
generate(const Point &pos, double size)
{
  static double ps = 8;

  double size1 = size/mgr()->maxS();

  if (! circles_.empty()) {
    for (auto &circle : circles_)
      circle->generate(pos, size);
  }
  else {
    double s = 0.9*mgr()->s()*size1;

    // draw center
    if (mgr_->isDebug()) {
      double xc = (x() - 0.5)*size1 + pos.x;
      double yc = (y() - 0.5)*size1 + pos.y;

      mgr_->addDebugCircle(xc, yc, ps, 0.0, 0.4);
    }

    // draw point circles
    auto np = numPoints();

    for (std::size_t i = 0; i < np; ++i) {
      Point p = getPoint(int(i));

      double x = (p.x - 0.5)*size1 + pos.x;
      double y = (p.y - 0.5)*size1 + pos.y;

      // draw point circle
      auto f = double(id_ + i)/double(mgr_->lastId());

      mgr_->addDrawCircle(x, y, s, f);

      // draw point
      if (mgr_->isDebug()) {
        mgr_->addDebugCircle(x, y, ps, 0.0, 1.0);
      }
    }
  }

  //------

  // draw bounding circle
  if (mgr_->isDebug()) {
    double s = r_*size1;

    double x = (this->x() - 0.5)*size1 + pos.x;
    double y = (this->y() - 0.5)*size1 + pos.y;

    mgr_->addDebugCircle(x, y, 2*s, 0.4, 0.0);
  }
}

//---

}
