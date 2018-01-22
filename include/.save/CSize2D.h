#ifndef CSIZE_2D_H
#define CSIZE_2D_H

//#include <CISize2D.h>
#include <CPoint2D.h>

class CSize2D {
 public:
  CSize2D() :
   width(0), height(0) {
  }

  CSize2D(double w, double h) :
   width(w), height(h) {
  }

#if 0
  explicit CSize2D(const CISize2D &size) :
   width(size.getWidth()), height(size.getHeight()) {
  }
#endif

  CSize2D(const CSize2D &size) :
   width(size.width), height(size.height) {
  }

  void set(double w, double h) {
    width  = w;
    height = h;
  }

  void get(double *w, double *h) const {
    *w = width;
    *h = height;
  }

  double getWidth () const { return width ; }
  double getHeight() const { return height; }

  void setWidth (double w) { width  = w; }
  void setHeight(double h) { height = h; }

  double area() const { return width*height; }

  friend CSize2D operator*(double m, const CSize2D &size) {
    return CSize2D(m*size.width, m*size.height);
  }

  friend CSize2D operator*(const CSize2D &size, double m) {
    return CSize2D(m*size.width, m*size.height);
  }

  friend CSize2D operator/(const CSize2D &size, double m) {
    return CSize2D(size.width/m, size.height/m);
  }

  friend std::ostream &operator<<(std::ostream &os, const CSize2D &size) {
    return os << "(" << size.width << "," << size.height << ")";
  }

  friend CPoint2D operator+(const CSize2D &s, const CPoint2D &p) {
    return CPoint2D(p.x + s.width, p.y + s.height);
  }

  friend CPoint2D operator+(const CPoint2D &p, const CSize2D &s) {
    return (s + p);
  }

  friend bool operator==(const CSize2D &lhs, const CSize2D &rhs) {
    return (lhs.width == rhs.width && lhs.height == rhs.height);
  }

  friend bool operator!=(const CSize2D &lhs, const CSize2D &rhs) {
    return ! (lhs == rhs);
  }

 public:
  double width { 0 }, height { 0 };
};

#endif
