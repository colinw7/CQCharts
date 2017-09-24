#ifndef CBBOX_2D_H
#define CBBOX_2D_H

//#include <CMathGen.h>
#include <CPoint2D.h>
#include <CVector2D.h>
#include <CSize2D.h>

class CBBox2D {
 public:
  CBBox2D() :
   pmin_(), pmax_(), set_(false) {
  }

  CBBox2D(const CBBox2D &bbox) :
   pmin_(bbox.pmin_), pmax_(bbox.pmax_), set_(bbox.set_) {
  }

  explicit CBBox2D(const CPoint2D &point) :
   pmin_(point), pmax_(point), set_(true) {
  }

  CBBox2D(const CPoint2D &pmin, const CPoint2D &pmax) :
   pmin_(pmin), pmax_(pmax), set_(true) {
    update();
  }

  CBBox2D(double x1, double y1, double x2, double y2) :
   pmin_(x1, y1), pmax_(x2, y2), set_(true) {
    update();
  }

  CBBox2D(const CPoint2D &o, const CSize2D &s) :
   pmin_(o), pmax_(o + s), set_(true) {
    update();
  }

  void reset() { set_ = false; }

  bool isSet() const { return set_; }

  CBBox2D operator+(const CPoint2D &rhs) const {
    CBBox2D t(*this);

    t += rhs;

    return t;
  }

  CBBox2D &operator+=(const CPoint2D &rhs) {
    add(rhs.x, rhs.y);

    return *this;
  }

  CBBox2D operator+(const CBBox2D &rhs) const {
    CBBox2D t(*this);

    t += rhs;

    return t;
  }

  CBBox2D &operator+=(const CBBox2D &rhs) {
    add(rhs);

    return *this;
  }

  friend CBBox2D operator*(const CBBox2D &lhs, double rhs) {
    return CBBox2D(lhs.pmin_*rhs, lhs.pmax_*rhs);
  }

  friend CBBox2D operator*(double rhs, const CBBox2D &lhs) {
    return CBBox2D(lhs.pmin_*rhs, lhs.pmax_*rhs);
  }

  void add(const CPoint2D &point) {
    add(point.x, point.y);
  }

  void add(double x, double y) {
    if (! set_) {
      pmin_ = CPoint2D(x, y);
      pmax_ = pmin_;

      set_ = true;
    }
    else {
      pmin_.x = std::min(pmin_.x, x);
      pmin_.y = std::min(pmin_.y, y);
      pmax_.x = std::max(pmax_.x, x);
      pmax_.y = std::max(pmax_.y, y);
    }
  }

  void add(const CBBox2D &bbox) {
    if (! bbox.set_) return;

    if (! set_) {
      pmin_ = bbox.pmin_;
      pmax_ = bbox.pmax_;

      set_ = true;
    }
    else {
      pmin_.x = std::min(pmin_.x, bbox.pmin_.x);
      pmin_.y = std::min(pmin_.y, bbox.pmin_.y);
      pmax_.x = std::max(pmax_.x, bbox.pmax_.x);
      pmax_.y = std::max(pmax_.y, bbox.pmax_.y);
    }
  }

  bool overlaps(const CBBox2D &bbox) const {
    if (! set_ || ! bbox.set_) return false;

    return ((pmax_.x >= bbox.pmin_.x && pmin_.x <= bbox.pmax_.x) &&
            (pmax_.y >= bbox.pmin_.y && pmin_.y <= bbox.pmax_.y));
  }

  bool overlapsX(const CBBox2D &bbox) const {
    if (! set_ || ! bbox.set_) return false;

    return (pmax_.x >= bbox.pmin_.x && pmin_.x <= bbox.pmax_.x);
  }

  bool overlapsY(const CBBox2D &bbox) const {
    if (! set_ || ! bbox.set_) return false;

    return (pmax_.y >= bbox.pmin_.y && pmin_.y <= bbox.pmax_.y);
  }

  bool intersect(const CBBox2D &bbox) const {
    if (! set_ || ! bbox.set_) return false;

    if ((pmax_.x < bbox.pmin_.x || pmin_.x > bbox.pmax_.x) ||
        (pmax_.y < bbox.pmin_.y || pmin_.y > bbox.pmax_.y))
      return false;

    return true;
  }

  bool intersect(const CBBox2D &bbox, CBBox2D &ibbox) const {
    if (! set_ || ! bbox.set_) return false;

    if ((pmax_.x < bbox.pmin_.x || pmin_.x > bbox.pmax_.x) ||
        (pmax_.y < bbox.pmin_.y || pmin_.y > bbox.pmax_.y))
      return false;

    ibbox.set_    = true;
    ibbox.pmin_.x = std::max(pmin_.x, bbox.pmin_.x);
    ibbox.pmin_.y = std::max(pmin_.y, bbox.pmin_.y);
    ibbox.pmax_.x = std::min(pmax_.x, bbox.pmax_.x);
    ibbox.pmax_.y = std::min(pmax_.y, bbox.pmax_.y);

    return true;
  }

  bool inside(double x, double y) const {
    return inside(CPoint2D(x, y));
  }

  bool inside(const CPoint2D &point) const {
    if (! set_) return false;

    return ((point.x >= pmin_.x && point.x <= pmax_.x) &&
            (point.y >= pmin_.y && point.y <= pmax_.y));
  }

  bool inside(const CBBox2D &bbox) const {
    if (! set_) return false;

    return ((bbox.pmin_.x >= pmin_.x && bbox.pmax_.x <= pmax_.x) &&
            (bbox.pmin_.y >= pmin_.y && bbox.pmax_.y <= pmax_.y));
  }

  double distanceTo(const CBBox2D &bbox) const {
    if (! set_) return 1E50; // assert

    // intersect
    if ((pmax_.x >= bbox.pmin_.x && pmin_.x <= bbox.pmax_.x) ||
        (pmax_.y >= bbox.pmin_.y && pmin_.y <= bbox.pmax_.y))
      return 0;

    // above or below
    if      ((pmin_.x >= bbox.pmin_.x && pmin_.x <= bbox.pmax_.x) ||
             (pmax_.x >= bbox.pmin_.x && pmax_.x <= bbox.pmax_.x)) {
      if (pmin_.y >= bbox.pmax_.y) // above
        return pmin_.y - bbox.pmax_.y;
      else                         // below
        return pmax_.y - bbox.pmin_.y;
    }
    // left or right
    else if ((pmin_.y >= bbox.pmin_.y && pmin_.y <= bbox.pmax_.y) ||
             (pmax_.y >= bbox.pmin_.y && pmax_.y <= bbox.pmax_.y)) {
      if (pmin_.x >= bbox.pmax_.x) // right
        return pmin_.x - bbox.pmax_.x;
      else                         // left
        return pmax_.x - bbox.pmin_.x;
    }
    // bottom left/top left
    else if (pmax_.x <= bbox.pmin_.x) {
      // bottom left
      if (pmax_.y <= bbox.pmin_.x) {
        return hypot(bbox.pmin_.x - pmax_.x, bbox.pmin_.y - pmax_.y);
      }
      // top left
      else {
        return hypot(bbox.pmin_.x - pmax_.x, bbox.pmin_.y - pmin_.y);
      }
    }
    // bottom right/top right
    else {
      // bottom right
      if (pmax_.y <= bbox.pmin_.x) {
        return hypot(bbox.pmax_.x - pmin_.x, bbox.pmin_.y - pmax_.y);
      }
      // top right
      else {
        return hypot(bbox.pmax_.x - pmin_.x, bbox.pmax_.y - pmin_.y);
      }
    }
  }

  void expand(double delta) {
    if (! set_) return;

    pmin_ -= delta;
    pmax_ += delta;

    update();
  }

  void expand(double x1, double y1, double x2, double y2) {
    if (! set_) return;

    pmin_.x += x1;
    pmin_.y += y1;
    pmax_.x += x2;
    pmax_.y += y2;

    update();
  }

  CBBox2D expanded(double x1, double y1, double x2, double y2) const {
    CBBox2D bbox(*this);

    bbox.expand(x1, y1, x2, y2);

    return bbox;
  }

  double area() const {
    if (! set_) return 0.0;

    CVector2D diag = CVector2D(pmin_, pmax_);

    return fabs(diag.getX()*diag.getY());
  }

  double perimeter() const {
    return 2*getWidth() + 2*getHeight();
  }

#if 0
  CMathGen::AxisType2D maxAxis() const {
    if (! set_) return CMathGen::X_AXIS_2D;

    CVector2D diag(pmax_.x - pmin_.x, pmax_.y - pmin_.y);

    if      (diag.getX() > diag.getY())
      return CMathGen::X_AXIS_2D;
    else
      return CMathGen::Y_AXIS_2D;
  }
#endif

  CPoint2D getMin() const { assert(set_); return pmin_; }
  CPoint2D getMax() const { assert(set_); return pmax_; }

  double getLeft  () const { return getXMin(); }
  double getBottom() const { return getYMin(); }
  double getRight () const { return getXMax(); }
  double getTop   () const { return getYMax(); }

  double getXMin() const { return getMin().x; }
  double getYMin() const { return getMin().y; }
  double getXMax() const { return getMax().x; }
  double getYMax() const { return getMax().y; }

  double getXMid() const { return (getXMin() + getXMax())/2; }
  double getYMid() const { return (getYMin() + getYMax())/2; }

  CPoint2D getCenter() const {
    return 0.5*(getMin() + getMax());
  }

  void setCenter(const CPoint2D &point) {
    double dx = point.x - getCenter().x;
    double dy = point.y - getCenter().y;

    moveBy(CPoint2D(dx, dy));
  }

  void setLL(const CPoint2D &point) {
    pmin_ = point;

    if (! set_) {
      pmax_ = point;
      set_  = true;
    }
  }

  void setUR(const CPoint2D &point) {
    pmax_ = point;

    if (! set_) {
      pmin_ = point;
      set_  = true;
    }
  }

  void setX(double x) { setXMin(x); }
  void setY(double y) { setYMin(y); }

  void setWidth(double width) {
    if (! set_)
      pmin_.x = 0;

    pmax_.x = pmin_.x + width;
    set_    = true;
  }

  void setHeight(double height) {
    if (! set_)
      pmin_.y = 0;

    pmax_.y = pmin_.y + height;
    set_    = true;
  }

  void setLeft  (double x) { setXMin(x); }
  void setBottom(double y) { setYMin(y); }
  void setRight (double x) { setXMax(x); }
  void setTop   (double y) { setYMax(y); }

  void setXMin(double x) {
    pmin_.x = x;
    set_    = true;
  }

  void setYMin(double y) {
    pmin_.y = y;
    set_    = true;
  }

  void setXMax(double x) {
    pmax_.x = x;
    set_    = true;
  }

  void setYMax(double y) {
    pmax_.y = y;
    set_    = true;
  }

  void setSize(const CSize2D &size) {
    if (! set_) {
      pmin_.x = 0;
      pmin_.y = 0;
    }

    pmax_.x = pmin_.x + size.width;
    pmax_.y = pmin_.y + size.height;
    set_    = true;
  }

  CPoint2D getLL() const { return getMin(); }
  CPoint2D getLR() const { assert(set_); return CPoint2D(pmax_.x, pmin_.y); }
  CPoint2D getUL() const { assert(set_); return CPoint2D(pmin_.x, pmax_.y); }
  CPoint2D getUR() const { return getMax(); }

  CSize2D getSize() const {
    return CSize2D(getWidth(), getHeight());
  }

  double getRadius() const {
    CVector2D radius = 0.5*CVector2D(getMin(), getMax());

    return radius.length();
  }

  double getWidth() const {
    return fabs(getXMax() - getXMin());
  }

  double getHeight() const {
    return fabs(getYMax() - getYMin());
  }

  CBBox2D &moveXTo(double x) {
    assert(set_);

    double dx = x - pmin_.x;

    pmin_.x += dx;
    pmax_.x += dx;

    update();

    return *this;
  }

  CBBox2D &moveYTo(double y) {
    assert(set_);

    double dy = y - pmin_.y;

    pmin_.y += dy;
    pmax_.y += dy;

    update();

    return *this;
  }

  CBBox2D &moveTo(const CPoint2D &p) {
    assert(set_);

    CPoint2D delta = p - pmin_;

    pmin_ += delta;
    pmax_ += delta;

    update();

    return *this;
  }

  CBBox2D &moveBy(const CVector2D &delta) {
    assert(set_);

    pmin_ += delta;
    pmax_ += delta;

    update();

    return *this;
  }

  CBBox2D &moveBy(const CPoint2D &delta) {
    assert(set_);

    pmin_ += delta;
    pmax_ += delta;

    update();

    return *this;
  }

  CBBox2D &moveBy(const CPoint2D &dmin, const CPoint2D &dmax) {
    assert(set_);

    pmin_ += dmin;
    pmax_ += dmax;

    update();

    return *this;
  }

  CBBox2D movedBy(const CPoint2D &delta) const {
    CBBox2D bbox(*this);

    bbox.moveBy(delta);

    return bbox;
  }

  friend bool operator==(const CBBox2D &lhs, const CBBox2D &rhs) {
    return (lhs.pmin_ == rhs.pmin_ && lhs.pmax_ == rhs.pmax_);
  }

  void print(std::ostream &os) const {
    if (! set_)
      os << "( not set )";
    else
      os << "(" << pmin_ << ") (" << pmax_ << ")";
  }

  friend std::ostream &operator<<(std::ostream &os, const CBBox2D &bbox) {
    bbox.print(os);

    return os;
  }

 private:
  void update() {
    assert(set_);

    if (pmin_.x > pmax_.x) std::swap(pmin_.x, pmax_.x);
    if (pmin_.y > pmax_.y) std::swap(pmin_.y, pmax_.y);
  }

 private:
  CPoint2D pmin_;
  CPoint2D pmax_;
  bool     set_ { false };
};

#endif
