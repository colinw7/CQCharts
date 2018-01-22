#ifndef CRANGE_2D_H
#define CRANGE_2D_H

#include <cmath>
#include <ostream>
#include <cassert>

class CRange2D {
 public:
  CRange2D() { }

  CRange2D(double x1, double y1, double x2, double y2) :
   set_(true), x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
  }

  bool isSet() const { return set_; }

  void set(double x1, double y1, double x2, double y2) {
    set_ = true;

    x1_ = x1; y1_ = y1;
    x2_ = x2; y2_ = y2;
  }

  bool get(double *x1, double *y1, double *x2, double *y2) const {
    *x1 = x1_; *y1 = y1_;
    *x2 = x2_; *y2 = y2_;

    return set_;
  }

  void reset() {
    set_ = false; x1_ = 0; y1_ = 0; x2_ = 0; y2_ = 0;
  }

  double dx() const { assert(set_); return x2_ - x1_; }
  double dy() const { assert(set_); return y2_ - y1_; }

  double xmid() const { assert(set_); return (x2_ + x1_)/2; }
  double ymid() const { assert(set_); return (y2_ + y1_)/2; }

  double xmin() const { assert(set_); return std::min(x1_, x2_); }
  double ymin() const { assert(set_); return std::min(y1_, y2_); }
  double xmax() const { assert(set_); return std::max(x1_, x2_); }
  double ymax() const { assert(set_); return std::max(y1_, y2_); }

  double left  () const { assert(set_); return x1_; }
  double bottom() const { assert(set_); return y1_; }
  double right () const { assert(set_); return x2_; }
  double top   () const { assert(set_); return y2_; }

  void setLeft  (const double &t) { set_ = true; x1_ = t; }
  void setBottom(const double &t) { set_ = true; y1_ = t; }
  void setRight (const double &t) { set_ = true; x2_ = t; }
  void setTop   (const double &t) { set_ = true; y2_ = t; }

  double xsize() const { assert(set_); return fabs(x2_ - x1_); }
  double ysize() const { assert(set_); return fabs(y2_ - y1_); }

  void inc(double dx, double dy) {
    assert(set_);

    x1_ += dx; y1_ += dy;
    x2_ += dx; y2_ += dy;
  }

  void incX(double dx) { assert(set_); x1_ += dx; x2_ += dx; }
  void incY(double dy) { assert(set_); y1_ += dy; y2_ += dy; }

  void updateRange(double x, double y) {
    if (! set_) {
      x1_ = x; y1_ = y;
      x2_ = x; y2_ = y;

      set_ = true;
    }
    else {
      x1_ = std::min(x1_, x); y1_ = std::min(y1_, y);
      x2_ = std::max(x2_, x); y2_ = std::max(y2_, y);
    }
  }

  CRange2D &operator=(const CRange2D &range) {
    set_ = range.set_;
    x1_  = range.x1_; y1_ = range.y1_;
    x2_  = range.x2_; y2_ = range.y2_;

    return *this;
  }

  friend bool operator==(const CRange2D &lhs, const CRange2D &rhs) {
    if (! lhs.set_ && ! rhs.set_) return true;
    if (! lhs.set_ || ! rhs.set_) return false;

    return (lhs.x1_ == rhs.x1_ && lhs.y1_ == rhs.y1_ &&
            lhs.x2_ == rhs.x2_ && lhs.y2_ == rhs.y2_);
  }

  friend bool operator!=(const CRange2D &lhs, const CRange2D &rhs) {
    return ! (lhs == rhs);
  }

  void print(std::ostream &os) const {
    os << x1_ << "," << y1_ << "," << x2_ << "," << y2_;
  }

  friend std::ostream &operator<<(std::ostream &os, const CRange2D &range) {
    range.print(os);

    return os;
  }

 private:
  bool   set_ { false };
  double x1_ { 0 }, y1_ { 0 }, x2_ { 0 }, y2_ { 0 };
};

#endif
