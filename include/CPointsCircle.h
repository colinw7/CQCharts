#ifndef CPointsCircle_H
#define CPointsCircle_H

/*
 * Smallest enclosing circle
 *
 * Copyright (c) 2014 Nayuki Minase
 * All rights reserved. Contact Nayuki for licensing.
 * http://nayuki.eigenstate.org/page/smallest-enclosing-circle
 */

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <sys/types.h>

namespace CPointsCircle {

namespace {

//------

class Point {
 public:
  double x, y;

 public:
  Point(double x1=0.0, double y1=0.0) :
   x(x1), y(y1) {
  }

  Point subtract(const Point &p) const {
    return Point(x - p.x, y - p.y);
  }

  double distance(const Point &p) const {
    return hypot(x - p.x, y - p.y);
  }

  // Signed area / determinant thing
  double cross(const Point &p) const {
    return x*p.y - y*p.x;
  }

  // Magnitude squared
  double norm() const {
    return x*x + y*y;
  }
};

typedef std::vector<Point> PointArray;

//------

class Circle {
 public:
  Point  c { 0.0, 0.0 }; // Center
  double r { 1.0 };      // Radius

  bool valid;

 public:
  Circle() : valid(false) { }

  Circle(const Point &c1, double r1) :
   c(c1), r(r1) {
    valid = true;
  }

  bool contains(const Point &p) const {
    assert(valid);

    return c.distance(p) <= r + 1E-12;
  }

  bool contains(const PointArray &points) {
    for (uint i = 0; i < points.size(); i++) {
      const Point &p = points[i];

      if (! contains(p))
        return false;
    }

    return true;
  }
};

//------

// TODO: smart slice (iterators or struct with range)
PointArray slice(const PointArray &points, int i1, int i2) {
  PointArray points1;

  for (int i = i1; i <= i2; ++i)
    points1.push_back(points[i]);

  return points1;
}

// TODO: better rand
int randIn(int low, int high) {
  int number = (abs(rand()) % (high - low + 1)) + low;

  return std::min(std::max(number, low), high);
}

// seed rand ?
void shufflePoints(PointArray &points) {
  int n = points.size();

  for (int i = 0; i < n; ++i) {
    int i1 = randIn(0, n - 1);
    int i2 = randIn(0, n - 1);

    if (i1 != i2)
      std::swap(points[i1], points[i2]);
  }
}

//------

Circle makeCircumcircle(const Point &a, const Point &b, const Point &c) {
  // Mathematical algorithm from Wikipedia: Circumscribed circle
  double d = (a.x*(b.y - c.y) + b.x*(c.y - a.y) + c.x*(a.y - b.y))*2;

  if (d == 0)
    return Circle();

  double x = (a.norm()*(b.y - c.y) + b.norm()*(c.y - a.y) + c.norm()*(a.y - b.y))/d;
  double y = (a.norm()*(c.x - b.x) + b.norm()*(a.x - c.x) + c.norm()*(b.x - a.x))/d;

  Point p(x, y);

  return Circle(p, p.distance(a));
}


Circle makeDiameter(const Point &a, const Point &b) {
  return Circle(Point((a.x + b.x)/2, (a.y + b.y)/2), a.distance(b)/2);
}

// Two boundary points known
Circle makeCircleTwoPoints(const PointArray &points, const Point &p, const Point &q) {
  Circle temp = makeDiameter(p, q);

  if (temp.contains(points))
    return temp;

  Circle left;
  Circle right;

  for (uint i = 0; i < points.size(); i++) {
    const Point &r = points[i];

    Point pq = q.subtract(p);

    double cross = pq.cross(r.subtract(p));

    Circle c = makeCircumcircle(p, q, r);

    if (c.valid) {
      if      (cross > 0 &&
               (! left .valid || pq.cross(c.c.subtract(p)) > pq.cross(left .c.subtract(p))))
        left = c;
      else if (cross < 0 &&
               (! right.valid || pq.cross(c.c.subtract(p)) < pq.cross(right.c.subtract(p))))
        right = c;
    }
  }

  return (! right.valid || (left.valid && left.r <= right.r)) ? left : right;
}

// One boundary point known
Circle makeCircleOnePoint(const PointArray &points, const Point &p) {
  Circle c(p, 0);

  for (uint i = 0; i < points.size(); i++) {
    const Point &q = points[i];

    if (! c.contains(q)) {
      if (c.r == 0)
        c = makeDiameter(p, q);
      else
        c = makeCircleTwoPoints(slice(points, 0, i), p, q);
    }
  }

  return c;
}

// Returns the smallest circle that encloses all the given points.
// Runs in expected O(n) time, randomized.
bool makeCircle(const PointArray &points, Circle &circle) {
  // Make copy to preserve the caller's data, shuffle order
  PointArray points1 = points;

  shufflePoints(points1);

  // Initially: No boundary points known
  // Progressively add points to circle or recompute circle
  Circle c;

  for (uint i = 0; i < points1.size(); i++) {
    const Point &p = points1[i];

    if (! c.valid || ! c.contains(p))
      c = makeCircleOnePoint(slice(points1, 0, i), p);
  }

  if (! c.valid)
    return false;

  circle = c;

  return true;
}

}

}

#endif
