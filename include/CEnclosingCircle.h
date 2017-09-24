#ifndef CEnclosingCircle_H
#define CEnclosingCircle_H

#include <cmath>
#include <algorithm>
#include <CPointsCircle.h>

class CEnclosingCircle {
 public:
  class Circle {
   public:
    Circle(double x=0.0, double y=0.0, double r=1.0) :
     x_(x), y_(y), r_(r) {
    }

    double x() const { return x_; }
    double y() const { return y_; }
    double r() const { return r_; }

   private:
    double x_ { 0.0 };
    double y_ { 0.0 };
    double r_ { 1.0 };
  };

  typedef std::vector<Circle> Circles;

 public:
  CEnclosingCircle() { }

  // add circle
  void addCircle(double x, double y, double r) {
    addCircle(Circle(x, y, r));
  }

  void addCircle(const Circle &circle) {
    circles_.push_back(circle);
  }

  bool calc(double &xc, double &yc, double &r) {
    Circle circle;

    if (! calc(circle))
      return false;

    xc = circle.x();
    yc = circle.y();
    r  = circle.r();

    return true;
  }

  // Given N circles calculate the center and radius of the enclosing circle
  bool calc(Circle &ecircle) {
    uint nc = circles_.size();

    if (nc == 0)
      return false;

    if (nc == 1) {
      ecircle = circles_[0];
      return true;
    }

    if (nc == 2) {
      ecircle = enclosingCircle(circles_[0], circles_[1]);
      return true;
    }

    getPointCircle(ecircle);

    sortClosest(ecircle.x(), ecircle.y());

    for (uint i = 0; i < nc; ++i)
      ecircle = enclosingCircle(ecircle, circles_[i]);

    return true;
  }

  // get added circles
  const Circles getCircles() const { return circles_; }

 private:
  void getPointCircle(Circle &pcircle) {
    CPointsCircle::PointArray points;

    uint nc = circles_.size();

    for (uint i = 1; i < nc; ++i)
      points.push_back(CPointsCircle::Point(circles_[i].x(), circles_[i].y()));

    CPointsCircle::Circle circle;

    (void) CPointsCircle::makeCircle(points, circle);

    pcircle = Circle(circle.c.x, circle.c.y, circle.r);
  }

  struct SortClosestCmp {
    SortClosestCmp(double x1, double y1) : x(x1), y(y1) { }

    bool operator()(const Circle &a, const Circle &b) {
      double dx1 = a.x() - x;
      double dy1 = a.y() - y;
      double dx2 = b.x() - x;
      double dy2 = b.y() - y;

      double d1 = sqrt(dx1*dx1 + dy1*dy1) + a.r();
      double d2 = sqrt(dx2*dx2 + dy2*dy2) + b.r();

      return (d1 < d2);
    }

    double x, y;
  };

  void sortClosest(double xc, double yc) {
    std::sort(circles_.begin(), circles_.end(), SortClosestCmp(xc, yc));
  }

  Circle enclosingCircle(const Circle &circle1, const Circle &circle2) {
    // Ensure that first circle radius is no larger than second circle radius
    if (circle1.r() > circle2.r())
      return enclosingCircle(circle2, circle1);

    // Compute the distance between centers of the circles
    double dx = circle1.x() - circle2.x();
    double dy = circle1.y() - circle2.y();

    double d = sqrt(dx*dx + dy*dy);

    // Check if first circle lies entirely inside the second circle. If so we are
    // done so just return second circle.
    if (d + circle1.r() <= circle2.r())
      return circle2;

    // If not inside then the enclosing circle has a radius of (d + r1 + r2)/2
    double r = (d + circle1.r() + circle2.r())/2.0;

    // The center of the enclosing circle must lie along the line connecting the two centers.
    //   center = (1-theta)*[x1,y1] + theta*[x2,y2]
    //
    // where theta is given by
    //
    //  theta = 1/2 + (r2 - r1)/(2*d)
    //
    // (Note: by construction theta >= 0.0 && theta <= 1.0)
    double theta = 0.5 + (circle2.r() - circle1.r())/(2.0*d);

    double x = (1.0 - theta)*circle1.x() + theta*circle2.x();
    double y = (1.0 - theta)*circle1.y() + theta*circle2.y();

    return Circle(x, y, r);
  }

 private:
  Circles circles_;
};

#endif
