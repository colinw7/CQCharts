#ifndef CPOINT_2D_H
#define CPOINT_2D_H

#include <cassert>
#include <cmath>
#include <iostream>

class CPoint2D {
 public:
  CPoint2D() { }

  CPoint2D(double x1, double y1) :
   x(x1), y(y1) {
  }

  CPoint2D(const CPoint2D &point) :
   x(point.x), y(point.y) {
  }

  CPoint2D &operator=(const CPoint2D &point) {
    x = point.x;
    y = point.y;

    return *this;
  }

  double getX() const { return x; }
  double getY() const { return y; }

  void getXY(double *x1, double *y1) const {
    *x1 = x; *y1 = y;
  }

  void getXY(double xy[2]) const {
    xy[0] = x; xy[1] = y;
  }

  double operator[](int i) const {
    switch (i) {
      case 0 : return x;
      case 1 : return y;
      default: assert(false);
    }
  }

  double &operator[](int i) {
    switch (i) {
      case 0 : return x;
      case 1 : return y;
      default: assert(false); }
  }

  void setX(double x1) { x = x1; }
  void setY(double y1) { y = y1; }

  void setXY(double x1, double y1) {
    x = x1; y = y1;
  }

  void setXY(double xy[2]) {
    x = xy[0]; y = xy[1];
  }

  //------

  CPoint2D &zero() {
    x = 0.0; y = 0.0;

    return *this;
  }

  //-----

  CPoint2D operator+() const {
    return CPoint2D(x, y);
  }

  CPoint2D operator-() const {
    return CPoint2D(-x, -y);
  }

  //-----

  bool equal(const CPoint2D &rhs, double tol=1E-6) const {
    double dx = fabs(x - rhs.x);
    double dy = fabs(y - rhs.y);

    return (dx < tol && dy < tol);
  }

  //-----

  friend bool operator==(const CPoint2D &lhs, const CPoint2D &rhs) {
    return lhs.equal(rhs, 1E-6);
  }

  friend bool operator!=(const CPoint2D &lhs, const CPoint2D &rhs) {
    return ! (lhs == rhs);
  }

  //------

  // Addition of points makes no mathematical sense but
  // is useful for weighted sum

  CPoint2D &operator+=(const CPoint2D &rhs) {
    x += rhs.x; y += rhs.y;

    return *this;
  }

  CPoint2D &operator+=(double rhs) {
    x += rhs; y += rhs;

    return *this;
  }

  CPoint2D operator+(const CPoint2D &rhs) const {
    return CPoint2D(x + rhs.x, y + rhs.y);
  }

  friend CPoint2D operator+(const CPoint2D &lhs, double rhs) {
    return CPoint2D(lhs.x + rhs, lhs.y + rhs);
  }

  friend CPoint2D operator+(double lhs, const CPoint2D &rhs) {
    return CPoint2D(rhs.x + lhs, rhs.y + lhs);
  }

  //------

  // Subtraction of points makes no mathematical sense but is useful for weighted sum
  CPoint2D &operator-=(const CPoint2D &rhs) {
    x -= rhs.x; y -= rhs.y;

    return *this;
  }

  CPoint2D &operator-=(double rhs) {
    x -= rhs; y -= rhs;

    return *this;
  }

  CPoint2D operator-(const CPoint2D &rhs) const {
    return CPoint2D(x - rhs.x, y - rhs.y);
  }

  //------

  // Multiplication of points makes no mathematical sense but is useful for weighted sum
  CPoint2D &operator*=(double rhs) {
    x *= rhs; y *= rhs;

    return *this;
  }

  CPoint2D &operator*=(const CPoint2D &rhs) {
    x *= rhs.x; y *= rhs.y;

    return *this;
  }

  CPoint2D operator*(const CPoint2D &rhs) const {
    return CPoint2D(x*rhs.x, y*rhs.y);
  }

  friend CPoint2D operator*(const CPoint2D &lhs, double rhs) {
    return CPoint2D(lhs.x*rhs, lhs.y*rhs);
  }

  friend CPoint2D operator*(double lhs, const CPoint2D &rhs) {
    return CPoint2D(rhs.x*lhs, rhs.y*lhs);
  }

  //------

  // Division of points makes no mathematical sense but is useful for weighted sum
  CPoint2D &operator/=(double rhs) {
    double irhs = 1.0/rhs;

    x *= irhs; y *= irhs;

    return *this;
  }

  CPoint2D &operator/=(const CPoint2D &rhs) {
    x /= rhs.x; y /= rhs.y;

    return *this;
  }

  CPoint2D operator/(const CPoint2D &rhs) const {
    return CPoint2D(x/rhs.x, y/rhs.y);
  }

  friend CPoint2D operator/(const CPoint2D &lhs, double rhs) {
    double irhs = 1.0/rhs;

    return CPoint2D(lhs.x*irhs, lhs.y*irhs);
  }

  friend CPoint2D operator/(double lhs, const CPoint2D &rhs) {
    return CPoint2D(lhs/rhs.x, lhs/rhs.y);
  }

  //------

  double minComponent() const {
    return std::min(x, y);
  }

  double maxComponent() const {
    return std::max(x, y);
  }

  //-----

  double distanceSqrTo(const CPoint2D &rhs) const {
    double dx = x - rhs.x;
    double dy = y - rhs.y;

    return (dx*dx + dy*dy);
  }

  double distanceTo(const CPoint2D &rhs) const {
    return sqrt(distanceSqrTo(rhs));
  }

  //-----

  CPoint2D rotate(const CPoint2D &center, double da) const {
    double s = sin(da);
    double c = cos(da);

    double x1 = x - center.x;
    double y1 = y - center.y;

    double x2 = x1*c - y1*s;
    double y2 = x1*s + y1*c;

    return CPoint2D(x2 + center.x, y2 + center.y);
  }

  //-----

  CPoint2D flip(const CPoint2D &c, bool x_axis=true) const {
    CPoint2D p = *this;

    if (x_axis)
      p.x = 2*c.x - p.x;
    else
      p.y = 2*c.y - p.y;

    return p;
  }

  //-----

  static CPoint2D min(const CPoint2D &lhs, const CPoint2D &rhs) {
    return CPoint2D(std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y));
  }

  static CPoint2D max(const CPoint2D &lhs, const CPoint2D &rhs) {
    return CPoint2D(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y));
  }

  //-----

  void print(std::ostream &os) const {
    os << "(" << x << "," << y << ")";
  }

  friend std::ostream &operator<<(std::ostream &os, const CPoint2D &point) {
    point.print(os);

    return os;
  }

 public:
  double x { 0 };
  double y { 0 };
};

#endif
