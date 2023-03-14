#ifndef CQChartsGeom_H
#define CQChartsGeom_H

#include <CQChartsTmpl.h>

#include <CQUtil.h>
#include <CMathUtil.h>
#include <QRectF>
#include <QSizeF>
#include <QPointF>
#include <QPolygonF>

#include <cassert>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <cstring>
//#include <sstream>

namespace CQChartsGeom {

#if 0
/*!
 * \brief Real class
 * \ingroup Charts
 */
class Real : public CQChartsComparatorBase<Real> {
 public:
  Real() = default;

  explicit Real(double r) :
   r_(r) {
  }

  double value() const { return r_; }

  bool equal(const Real &rhs, double tol=1E-6) const {
    double d = std::abs(value() - rhs.value());

    return (d < tol);
  }

  //-----

  friend int cmp(const Real &lhs, const Real &rhs) {
    if (lhs.equal(rhs, 1E-6)) return 0;
    return (lhs.value() > rhs.value() ? 1 : -1);
  }

 private:
  double r_ { 0.0 };
};
#endif

//---

/*!
 * \brief Point class
 * \ingroup Charts
 */
class Point :
  public CQChartsEqBase<Point>,
  public CQChartsToStringBase<Point> {
 public:
  Point() = default;

  Point(double x1, double y1) :
   x(x1), y(y1) {
  }

  explicit Point(const QPointF &point) :
   x(point.x()), y(point.y()) {
  }

  explicit Point(const QPoint &point) :
   x(point.x()), y(point.y()) {
  }

  //---

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
      default: { assert(false); return 0.0; }
    }
  }

  double &operator[](int i) {
    switch (i) {
      case 0 : return x;
      case 1 : return y;
      default: { assert(false); return x; }
    }
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

  Point &zero() {
    x = 0.0; y = 0.0;

    return *this;
  }

  //-----

  QPointF qpoint () const { return QPointF(x, y); }
  QPoint  qpointi() const { return QPoint (int(x), int(y)); }

  //-----

  Point operator+() const { return Point( x,  y); }
  Point operator-() const { return Point(-x, -y); }

  //-----

  bool equal(const Point &rhs, double tol=1E-6) const {
    double dx = std::abs(x - rhs.x);
    double dy = std::abs(y - rhs.y);

    return (dx < tol && dy < tol);
  }

  //-----

  friend bool operator==(const Point &lhs, const Point &rhs) {
    return lhs.equal(rhs, 1E-6);
  }

  friend bool operator<(const Point &lhs, const Point &rhs) {
    return (lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y));
  }

  friend bool operator>=(const Point &lhs, const Point &rhs) {
    return ! operator<(lhs, rhs);
  }

  //------

  // Addition of points makes no mathematical sense but
  // is useful for weighted sum

  Point &operator+=(const Point &rhs) { x += rhs.x; y += rhs.y; return *this; }

//Point &operator+=(double rhs) { x += rhs; y += rhs; return *this; }

  Point operator+(const Point &rhs) const { return Point(x + rhs.x, y + rhs.y); }

//friend Point operator+(const Point &lhs, double rhs) { return Point(lhs.x + rhs, lhs.y + rhs); }
//friend Point operator+(double lhs, const Point &rhs) { return Point(rhs.x + lhs, rhs.y + lhs); }

  //------

  // Subtraction of points makes no mathematical sense but is useful for weighted sum
  Point &operator-=(const Point &rhs) { x -= rhs.x; y -= rhs.y; return *this; }

//Point &operator-=(double rhs) { x -= rhs; y -= rhs; return *this; }

  Point operator-(const Point &rhs) const { return Point(x - rhs.x, y - rhs.y); }

  //------

  // Multiplication of points makes no mathematical sense but is useful for weighted sum
  Point &operator*=(double rhs) { x *= rhs; y *= rhs; return *this; }

//Point &operator*=(const Point &rhs) { x *= rhs.x; y *= rhs.y; return *this; }
//Point operator*(const Point &rhs) const { return Point(x*rhs.x, y*rhs.y); }

  friend Point operator*(const Point &lhs, double rhs) { return Point(lhs.x*rhs, lhs.y*rhs); }
  friend Point operator*(double lhs, const Point &rhs) { return Point(rhs.x*lhs, rhs.y*lhs); }

  //------

  // Division of points makes no mathematical sense but is useful for weighted sum
  Point &operator/=(double rhs) { double irhs = 1.0/rhs; x *= irhs; y *= irhs; return *this; }

//Point &operator/=(const Point &rhs) { x /= rhs.x; y /= rhs.y; return *this; }
//Point operator/(const Point &rhs) const { return Point(x/rhs.x, y/rhs.y); }

  friend Point operator/(const Point &lhs, double rhs) {
    double irhs = 1.0/rhs;

    return Point(lhs.x*irhs, lhs.y*irhs);
  }

//friend Point operator/(double lhs, const Point &rhs) { return Point(lhs/rhs.x, lhs/rhs.y); }

  //------

  static Point interp(const Point &from, const Point &to, double f=0.5) {
    return Point(CMathUtil::lerp(f, from.x, to.x), CMathUtil::lerp(f, from.y, to.y));
  }

  //---

  double minComponent() const { return std::min(x, y); }
  double maxComponent() const { return std::max(x, y); }

  //-----

  double distanceSqrTo(const Point &rhs) const {
    double dx = x - rhs.x;
    double dy = y - rhs.y;

    return (dx*dx + dy*dy);
  }

  double distanceTo(const Point &rhs) const {
    return sqrt(distanceSqrTo(rhs));
  }

  //-----

  Point rotate(const Point &center, double da) const {
    double s = std::sin(da);
    double c = std::cos(da);

    double x1 = x - center.x;
    double y1 = y - center.y;

    double x2 = x1*c - y1*s;
    double y2 = x1*s + y1*c;

    return Point(x2 + center.x, y2 + center.y);
  }

  //-----

  Point flip(const Point &c, bool x_axis=true) const {
    Point p = *this;

    if (x_axis)
      p.x = 2*c.x - p.x;
    else
      p.y = 2*c.y - p.y;

    return p;
  }

  //-----

  static Point min(const Point &lhs, const Point &rhs) {
    return Point(std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y));
  }

  static Point max(const Point &lhs, const Point &rhs) {
    return Point(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y));
  }

  static Point avg(const Point &lhs, const Point &rhs) {
    return Point(CMathUtil::avg(lhs.x, rhs.x), CMathUtil::avg(lhs.y, rhs.y));
  }

  //-----

  QString toString() const;
  bool fromString(const QString &s);

 public:
  double x { 0 };
  double y { 0 };
};

inline Point makeDirPoint(bool flipped, double x, double y) {
  if (! flipped)
    return Point(x, y);
  else
    return Point(y, x);
}

}

//------

namespace CQChartsGeom {

class Line {
 public:
  Line() = default;

  Line(const Point &start, const Point &end) :
   start_(start), end_(end) {
  }

  const Point &start() const { return start_; }
  void setStart(const Point &v) { start_ = v; }

  const Point &end() const { return end_; }
  void setEnd(const Point &v) { end_ = v; }

 private:
  Point start_;
  Point end_;
};

}

//------

namespace CQChartsGeom {

/*!
 * \brief Size class
 * \ingroup Charts
 */
class Size :
  public CQChartsEqBase<Size> {
 public:
  Size() = default;

  Size(double w, double h) :
   size_(w, h), set_(true) {
  }

  explicit Size(const QSizeF &s) :
   size_(s), set_(true) {
  }

  bool isSet() const { return set_; }

  const QSizeF &qsize() const { assert(set_); return size_; }

  double width() const { assert(set_); return size_.width(); }
  void setWidth(double w) { size_.setWidth(w); set_ = true; }

  double height() const { assert(set_); return size_.height(); }
  void setHeight(double h) { size_.setHeight(h); set_ = true; }

  double optWidth(double defWidth=0.0) const {
    if (! set_) return defWidth;
    return size_.width();
  }

  double optHeight(double defHeight=0.0) const {
    if (! set_) return defHeight;
    return size_.height();
  }

  //---

  static Size interp(const Size &from, const Size &to, double f=0.5) {
    return Size(CMathUtil::lerp(f, from.width (), to.width ()),
                CMathUtil::lerp(f, from.height(), to.height()));
  }

  //---

  friend bool operator==(const Size &lhs, const Size &rhs) {
    if (! lhs.set_ && ! rhs.set_) return true;
    if (! lhs.set_ || ! rhs.set_) return false;

    return (lhs.size_.width () == rhs.size_.width () &&
            lhs.size_.height() == rhs.size_.height());
  }

  //---

  QString toString() const;
  bool fromString(const QString &s);

 private:
  QSizeF size_;
  bool   set_ { false };
};

}

//------

namespace CQChartsGeom {

class Range;

}

//------

namespace CQChartsGeom {

/*!
 * \brief Bounding Box class
 * \ingroup Charts
 */
class BBox :
  public CQChartsEqBase<BBox>,
  public CQChartsToStringBase<BBox> {
 public:
  BBox() = default;

  explicit BBox(const Point &point) :
   pmin_(point), pmax_(point), set_(true) {
  }

  BBox(const Point &pmin, const Point &pmax) :
   pmin_(pmin), pmax_(pmax), set_(true) {
    (void) update();
  }

  BBox(double x1, double y1, double x2, double y2) :
   pmin_(x1, y1), pmax_(x2, y2), set_(true) {
    (void) update();
  }

  explicit BBox(const QRectF &rect) :
   pmin_(rect.bottomLeft()), pmax_(rect.topRight()), set_(! rect.isNull()) {
    if (isSet())
      (void) update();
  }

  BBox(const QPointF &p1, const QPointF &p2) :
   pmin_(p1.x(), p1.y()), pmax_(p2.x(), p2.y()), set_(true) {
    if (isSet())
      (void) update();
  }

  explicit BBox(const Range &range);

  BBox(const Point &o, const Size &s) :
   pmin_(o), pmax_(o + Point(s.width(), s.height())), set_(true) {
    (void) update();
  }

  //---

  void reset() { set_ = false; }

  bool isSet() const { return set_; }

  //---

  QRectF qrect() const {
    if (isSet())
      return QRectF(getLL().qpoint(), getUR().qpoint()).normalized();
    else
      return QRectF();
  }

  QRect qrecti() const {
    if (isSet())
      return QRect(getLL().qpointi(), getUR().qpointi()).normalized();
    else
      return QRect();
  }

  //---

  Size size() const { return Size(getWidth(), getHeight()); }

  //QSizeF qsize() const { return QSizeF(getWidth(), getHeight()); }

  double getMinSize() const { return std::min(getWidth(), getHeight()); }
  double getMaxSize() const { return std::max(getWidth(), getHeight()); }

  //---

  BBox operator+(const Point &rhs) const {
    BBox t(*this);

    t += rhs;

    return t;
  }

  BBox &operator+=(const Point &rhs) {
    add(rhs.x, rhs.y);

    return *this;
  }

  BBox operator+(const BBox &rhs) const {
    BBox t(*this);

    t += rhs;

    return t;
  }

  BBox &operator+=(const BBox &rhs) {
    add(rhs);

    return *this;
  }

  friend BBox operator*(const BBox &lhs, double rhs) {
    return BBox(lhs.pmin_*rhs, lhs.pmax_*rhs);
  }

  friend BBox operator*(double rhs, const BBox &lhs) {
    return BBox(lhs.pmin_*rhs, lhs.pmax_*rhs);
  }

  void add(const Point &point) {
    add(point.x, point.y);
  }

  void add(double x, double y) {
    if (! set_) {
      pmin_ = Point(x, y);
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

  void add(const BBox &bbox) {
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

  void addX(const BBox &bbox) {
    assert(set_);

    if (! bbox.set_) return;

    pmin_.x = std::min(pmin_.x, bbox.pmin_.x);
    pmax_.x = std::max(pmax_.x, bbox.pmax_.x);
  }

  void addY(const BBox &bbox) {
    assert(set_);

    if (! bbox.set_) return;

    pmin_.y = std::min(pmin_.y, bbox.pmin_.y);
    pmax_.y = std::max(pmax_.y, bbox.pmax_.y);
  }

  bool overlaps(const BBox &bbox) const {
    if (! set_ || ! bbox.set_) return false;

    return ((pmax_.x >= bbox.pmin_.x && pmin_.x <= bbox.pmax_.x) &&
            (pmax_.y >= bbox.pmin_.y && pmin_.y <= bbox.pmax_.y));
  }

  bool overlapsX(const BBox &bbox) const {
    if (! set_ || ! bbox.set_) return false;

    return (pmax_.x >= bbox.pmin_.x && pmin_.x <= bbox.pmax_.x);
  }

  bool overlapsY(const BBox &bbox) const {
    if (! set_ || ! bbox.set_) return false;

    return (pmax_.y >= bbox.pmin_.y && pmin_.y <= bbox.pmax_.y);
  }

  bool intersect(const BBox &bbox) const {
    if (! set_ || ! bbox.set_) return false;

    if ((pmax_.x < bbox.pmin_.x || pmin_.x > bbox.pmax_.x) ||
        (pmax_.y < bbox.pmin_.y || pmin_.y > bbox.pmax_.y))
      return false;

    return true;
  }

  bool intersect(const BBox &bbox, BBox &ibbox) const {
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

  // supplied point inside this rect
  bool inside(const Point &point) const {
    if (! set_) return false;

    return ((point.x >= pmin_.x && point.x <= pmax_.x) &&
            (point.y >= pmin_.y && point.y <= pmax_.y));
  }

  // supplied bbox inside this bbox
  bool inside(const BBox &bbox) const {
    if (! set_) return false;

    return ((bbox.pmin_.x >= pmin_.x && bbox.pmax_.x <= pmax_.x) &&
            (bbox.pmin_.y >= pmin_.y && bbox.pmax_.y <= pmax_.y));
  }

  bool insideX(double x) const {
    if (! set_) return false;

    return (x >= pmin_.x && x <= pmax_.x);
  }

  bool insideY(double y) const {
    if (! set_) return false;

    return (y >= pmin_.y && y <= pmax_.y);
  }

  bool insideX(const BBox &bbox) const {
    if (! set_) return false;

    return (bbox.pmin_.x >= pmin_.x && bbox.pmax_.x <= pmax_.x);
  }

  bool insideY(const BBox &bbox) const {
    if (! set_) return false;

    return (bbox.pmin_.y >= pmin_.y && bbox.pmax_.y <= pmax_.y);
  }

  double distanceTo(const Point &p) const {
    if      (p.x < pmin_.x) {
      if      (p.y < pmin_.y) return getLL().distanceTo(p);
      else if (p.y > pmax_.y) return getUL().distanceTo(p);
      else                    return pmin_.x - p.x;
    }
    else if (p.x > pmax_.x) {
      if      (p.y < pmin_.y) return getLR().distanceTo(p);
      else if (p.y > pmax_.y) return getUR().distanceTo(p);
      else                    return p.x - pmax_.x;
    }
    else {
      if      (p.y < pmin_.y) return pmin_.y - p.y;
      else if (p.y > pmax_.y) return p.y - pmax_.y;
      else                    return 0.0;
    }
  }

  double distanceTo(const BBox &bbox) const {
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

  bool expand(double delta) {
    assert(set_);

    pmin_ -= Point(delta, delta);
    pmax_ += Point(delta, delta);

    return update();
  }

  bool expand(double dx1, double dy1, double dx2, double dy2) {
    assert(set_);

    pmin_.x += dx1;
    pmin_.y += dy1;
    pmax_.x += dx2;
    pmax_.y += dy2;

    return update();
  }

  BBox expanded(double dx1, double dy1, double dx2, double dy2) const {
    assert(set_);

    BBox bbox(*this);

    (void) bbox.expand(dx1, dy1, dx2, dy2);

    return bbox;
  }

  BBox adjusted(double dx1, double dy1, double dx2, double dy2) const {
    bool swapped;

    return adjusted(dx1, dy1, dx2, dy2, swapped);
  }

  BBox adjusted(double dx1, double dy1, double dx2, double dy2, bool &swapped) const {
    assert(set_);

    BBox bbox(*this);

    swapped = ! bbox.expand(dx1, dy1, dx2, dy2);

    return bbox;
  }

  BBox translated(double dx, double dy) const {
    return movedBy(Point(dx, dy));
  }

  bool isValid() const { return set_ && (getWidth()*getHeight() > 0.0); }

  double area() const {
    if (! set_) return 0.0;

    return getWidth()*getHeight();
  }

  double perimeter() const {
    if (! set_) return 0.0;

    return 2*getWidth() + 2*getHeight();
  }

  void equalScale(double targetAspect, bool grow=true);

  Point getMin() const { assert(set_); return pmin_; }
  Point getMax() const { assert(set_); return pmax_; }

  double getLeft  () const { return getXMin(); }
  double getBottom() const { return getYMin(); }
  double getRight () const { return getXMax(); }
  double getTop   () const { return getYMax(); }

  double getXMin() const { return getMin().x; }
  double getYMin() const { return getMin().y; }
  double getXMax() const { return getMax().x; }
  double getYMax() const { return getMax().y; }

  double getXMid() const { return CMathUtil::avg(getXMin(), getXMax()); }
  double getYMid() const { return CMathUtil::avg(getYMin(), getYMax()); }

  double getXYMid(bool horizontal) const { return (horizontal ? getXMid() : getYMid()); }

  void getXYRange(bool horizontal, double &min, double &max) const {
    min = (horizontal ? getXMin() : getYMin());
    max = (horizontal ? getXMax() : getYMax());
  }

  Point getCenter() const { return 0.5*(getMin() + getMax()); }

  void setCenter(const Point &point) {
    double dx = point.x - getCenter().x;
    double dy = point.y - getCenter().y;

    moveBy(Point(dx, dy));
  }

  void setLL(const Point &point) {
    pmin_ = point;

    if (! set_) {
      pmax_ = point;
      set_  = true;
    }
  }

  void setLR(const Point &point) {
    pmax_.x = point.x;
    pmin_.y = point.y;

    if (! set_) {
      pmin_ = point;
      pmax_ = point;
      set_  = true;
    }
  }

  void setUL(const Point &point) {
    pmin_.x = point.x;
    pmax_.y = point.y;

    if (! set_) {
      pmin_ = point;
      set_  = true;
    }
  }

  void setUR(const Point &point) {
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

  double getMinExtent(bool horizontal) const { return (horizontal ? getXMin() : getYMin()); }
  double getMaxExtent(bool horizontal) const { return (horizontal ? getXMax() : getYMax()); }

  void setMinExtent(bool horizontal, double e) { horizontal ? setXMin(e) : setYMin(e); }
  void setMaxExtent(bool horizontal, double e) { horizontal ? setXMax(e) : setYMax(e); }

  void setExtent(double emin, double emax, bool horizontal) {
    (horizontal ? setXRange(emin, emax) : setYRange(emin, emax));
  }

  bool expandExtent(double emin, double emax, bool horizontal) {
    assert(set_);

    if (horizontal)
      setXRange(getXMin() - emin, getXMax() + emax);
    else
      setYRange(getYMin() - emin, getYMax() + emax);

    return update();
  }

  void setXRange(double xl, double xr) { setXMin(xl); setXMax(xr); }
  void setYRange(double yb, double yt) { setYMin(yb); setYMax(yt); }

  void setLeft  (double x) { setXMin(x); }
  void setBottom(double y) { setYMin(y); }
  void setRight (double x) { setXMax(x); }
  void setTop   (double y) { setYMax(y); }

  void setXMin(double x) { pmin_.x = x; set_ = true; }
  void setYMin(double y) { pmin_.y = y; set_ = true; }

  void setXMax(double x) { pmax_.x = x; set_ = true; }
  void setYMax(double y) { pmax_.y = y; set_ = true; }

#if 0
  void setSize(const Size &size) {
    if (! set_) {
      pmin_.x = 0;
      pmin_.y = 0;
    }

    pmax_.x = pmin_.x + size.width;
    pmax_.y = pmin_.y + size.height;
    set_    = true;
  }
#endif

  Point getLL() const { return getMin(); }
  Point getLR() const { assert(set_); return Point(pmax_.x, pmin_.y); }
  Point getUL() const { assert(set_); return Point(pmin_.x, pmax_.y); }
  Point getUR() const { return getMax(); }

  Point getMidL() const { assert(set_); return Point(pmin_.x, getYMid()); }
  Point getMidR() const { assert(set_); return Point(pmax_.x, getYMid()); }
  Point getMidB() const { assert(set_); return Point(getXMid(), pmin_.y); }
  Point getMidT() const { assert(set_); return Point(getXMid(), pmax_.y); }

#if 0
  Size getSize() const {
    return Size(getWidth(), getHeight());
  }
#endif

#if 0
  double getRadius() const {
    if (! set_) return 0.0;

    double dx = std::abs(pmax_.x - pmin_.x);
    double dy = std::abs(pmax_.y - pmin_.y);

    return 0.5*std::hypot(dx, dy);
  }
#endif

  double getSize(bool horizontal) const { return (horizontal ? getWidth() : getHeight()); }

  double getWidth () const { return std::abs(getXMax() - getXMin()); }
  double getHeight() const { return std::abs(getYMax() - getYMin()); }

  double getOptWidth (double def=0.0) const { return (set_ ? getWidth () : def); }
  double getOptHeight(double def=0.0) const { return (set_ ? getHeight() : def); }

  BBox &moveXTo(double x) {
    assert(set_);

    double dx = x - pmin_.x;

    pmin_.x += dx;
    pmax_.x += dx;

    (void) update();

    return *this;
  }

  BBox &moveYTo(double y) {
    assert(set_);

    double dy = y - pmin_.y;

    pmin_.y += dy;
    pmax_.y += dy;

    (void) update();

    return *this;
  }

  BBox &moveTo(const Point &p) {
    assert(set_);

    Point delta = p - pmin_;

    pmin_ += delta;
    pmax_ += delta;

    (void) update();

    return *this;
  }

  BBox &moveBy(const Point &delta) {
    assert(set_);

    pmin_ += delta;
    pmax_ += delta;

    (void) update();

    return *this;
  }

  BBox &moveBy(const Point &dmin, const Point &dmax) {
    assert(set_);

    pmin_ += dmin;
    pmax_ += dmax;

    (void) update();

    return *this;
  }

  BBox movedBy(const Point &delta) const {
    BBox bbox(*this);

    bbox.moveBy(delta);

    return bbox;
  }

  //---

  bool isZero() const { return isXZero() || isYZero(); }

  bool isXZero() const { return CMathUtil::isZero(getWidth()); }
  bool isYZero() const { return CMathUtil::isZero(getHeight()); }

  void makeNonZero(double d=1.0) {
    if (isSet()) {
      if (isXZero()) {
        double x = getXMid(), y = getYMid();

        add(x - d, y);
        add(x + d, y);
      }

      if (isYZero()) {
        double x = getXMid(), y = getYMid();

        add(x, y - d);
        add(x, y + d);
      }
    }
    else {
      add(-d, -d);
      add( d,  d);
    }
  }

  //---

  void scale(double xf, double yf) {
    double w = getWidth ()*xf;
    double h = getHeight()*yf;

    BBox rect(getXMin(), getYMin(), getXMin() + w, getYMin() + h);

    pmin_ = rect.getLL();
    pmax_ = rect.getUR();
  }

  BBox centerScaled(double xf, double yf) const {
    BBox bbox(*this);

    bbox.centerScale(xf, yf);

    return bbox;
  }

  void centerScale(double xf, double yf) {
    auto xc = getXMid();
    auto yc = getYMid();

    auto xr = xf*getWidth ()/2.0;
    auto yr = yf*getHeight()/2.0;

    BBox rect(xc - xr, yc - yr, xc + xr, yc + yr);

    pmin_ = rect.getLL();
    pmax_ = rect.getUR();
  }

  //---

  // true if ok, false if changed (swapped)
  bool update() {
    assert(set_);

    bool changed = false;

    if (pmin_.x > pmax_.x) { std::swap(pmin_.x, pmax_.x); changed = true; }
    if (pmin_.y > pmax_.y) { std::swap(pmin_.y, pmax_.y); changed = true; }

    return ! changed;
  }

  //---

  static BBox interp(const BBox &from, const BBox &to, double f=0.5) {
    auto c = Point::interp(from.getCenter(), to.getCenter(), f);

    auto s = Size::interp(from.size(), to.size(), f);

    return BBox(c.x - s.width()/2.0, c.y - s.height()/2.0,
                c.x + s.width()/2.0, c.y + s.height()/2.0);
  }

  //---

  friend bool operator==(const BBox &lhs, const BBox &rhs) {
    return (lhs.pmin_ == rhs.pmin_ && lhs.pmax_ == rhs.pmax_);
  }

  //---

  QString toString() const;
  bool fromString(const QString &s);

 private:
  Point pmin_;
  Point pmax_;
  bool  set_ { false };
};

inline BBox makeDirBBox(bool flipped, double x1, double y1, double x2, double y2) {
  if (! flipped)
    return BBox(x1, y1, x2, y2);
  else
    return BBox(y1, x1, y2, x2);
}

}

//------

namespace CQChartsGeom {

/*!
 * \brief Range class
 * \ingroup Charts
 *
 * TODO: enforce min/max order always ? Same as BBox
 */
class Range :
  public CQChartsEqBase<Range>,
  public CQChartsToStringBase<Range> {
 public:
  Range() = default;

  Range(double x1, double y1, double x2, double y2) :
   set_(true), x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
  }

  explicit Range(const BBox &bbox) :
   set_(bbox.isSet()), x1_(bbox.getXMin()), y1_(bbox.getYMin()),
                       x2_(bbox.getXMax()), y2_(bbox.getYMax()) {
  }

  //---

  bool isSet() const { return set_; }

  bool isValid() const {
    if (! isSet()) return false;

    return (! CMathUtil::isNaN(x1_) && ! CMathUtil::isNaN(y1_) &&
            ! CMathUtil::isNaN(x2_) && ! CMathUtil::isNaN(y2_));
  }

  //---

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

  double xmid() const { assert(set_); return CMathUtil::avg(x1_, x2_); }
  double ymid() const { assert(set_); return CMathUtil::avg(y1_, y2_); }

  double xmin() const { assert(set_); return std::min(x1_, x2_); }
  double ymin() const { assert(set_); return std::min(y1_, y2_); }
  double xmax() const { assert(set_); return std::max(x1_, x2_); }
  double ymax() const { assert(set_); return std::max(y1_, y2_); }

  double min(bool horizontal) const { return (horizontal ? xmin() : ymin()); }
  double max(bool horizontal) const { return (horizontal ? xmax() : ymax()); }

  double left  () const { assert(set_); return x1_; }
  double bottom() const { assert(set_); return y1_; }
  double right () const { assert(set_); return x2_; }
  double top   () const { assert(set_); return y2_; }

  void setLeft  (double t) { set_ = true; x1_ = t; }
  void setBottom(double t) { set_ = true; y1_ = t; }
  void setRight (double t) { set_ = true; x2_ = t; }
  void setTop   (double t) { set_ = true; y2_ = t; }

  double xsize() const { assert(set_); return std::abs(x2_ - x1_); }
  double ysize() const { assert(set_); return std::abs(y2_ - y1_); }

  double size(bool horizontal) const { return (horizontal ? xsize() : ysize()); }

  double aspect() const { return (ysize() > 0.0 ? xsize()/ysize() : 1.0); }

  //---

  BBox bbox() const {
     if (isSet())
      return BBox(xmin(), ymin(), xmax(), ymax());
    else
      return BBox();
  }

#if 0
  QRectF qrect() const {
    if (isSet())
      return QRectF(xmin(), ymin(), xsize(), ysize()).normalized();
    else
      return QRectF();
  }
#endif

  //---

  bool isZero() const { return isXZero() || isYZero(); }

  bool isXZero() const { return CMathUtil::isZero(xsize()); }
  bool isYZero() const { return CMathUtil::isZero(ysize()); }

  void makeNonZero(double d=1.0) {
    if (isSet()) {
      if (isXZero()) {
        double x = xmid(), y = ymid();

        updateRange(x - d, y);
        updateRange(x + d, y);
      }

      if (isYZero()) {
        double x = xmid(), y = ymid();

        updateRange(x, y - d);
        updateRange(x, y + d);
      }
    }
    else {
      updateRange(-d, -d);
      updateRange( d,  d);
    }
  }

  Point center() const {
    assert(set_); return Point(CMathUtil::avg(x1_, x2_), CMathUtil::avg(y1_, y2_));
  }

  void inc(double dx, double dy) {
    assert(set_);

    x1_ += dx; y1_ += dy;
    x2_ += dx; y2_ += dy;
  }

  void incX(double dx) { assert(set_); x1_ += dx; x2_ += dx; }
  void incY(double dy) { assert(set_); y1_ += dy; y2_ += dy; }

  void updateRange(const Point &p) {
    updateRange(p.x, p.y);
  }

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

  //---

  friend bool operator==(const Range &lhs, const Range &rhs) {
    if (! lhs.set_ && ! rhs.set_) return true;
    if (! lhs.set_ || ! rhs.set_) return false;

    return (lhs.x1_ == rhs.x1_ && lhs.y1_ == rhs.y1_ &&
            lhs.x2_ == rhs.x2_ && lhs.y2_ == rhs.y2_);
  }

  //---

  void equalScale(double targetAspect);

  // supplied point inside this rect
  bool inside(const Point &point) const {
    if (! set_) return false;

    return ((point.x >= x1_ && point.x <= x2_) &&
            (point.y >= y1_ && point.y <= y2_));
  }

  QString toString() const {
    return QString("%1 %2 %3 %4").arg(x1_).arg(y1_).arg(x2_).arg(y2_);
  }

  Range &operator+=(const Range &r) {
    if (! r.set_) return *this;

    if (set_) {
      x1_ = std::min(x1_, r.x1_);
      y1_ = std::min(y1_, r.y1_);
      x2_ = std::max(x2_, r.x2_);
      y2_ = std::max(y2_, r.y2_);
    }
    else {
      x1_ = r.x1_;
      y1_ = r.y1_;
      x2_ = r.x2_;
      y2_ = r.y2_;
    }

    set_ = true;

    return *this;
  }

 private:
  bool   set_ { false };
  double x1_ { 0 }, y1_ { 0 }, x2_ { 0 }, y2_ { 0 };
};

}

//------

namespace CQChartsGeom {

/*!
 * \brief Polygon class
 * \ingroup Charts
 */
class Polygon :
  public CQChartsEqBase<Polygon> {
 public:
  Polygon() = default;

  explicit Polygon(const QPolygonF &qpoly) :
   qpoly_(qpoly) {
  }

  explicit Polygon(const BBox &bbox) {
   qpoly_ << bbox.getLL().qpoint();
   qpoly_ << bbox.getLR().qpoint();
   qpoly_ << bbox.getUR().qpoint();
   qpoly_ << bbox.getUL().qpoint();
  }

  const QPolygonF &qpoly() const { return qpoly_; }

  bool empty() const { return qpoly_.empty(); }

  int size() const { return qpoly_.count(); }

  const QPointF &qpoint(int i) const { return qpoly_.at(i); }
  Point point(int i) const { return Point(qpoly_.at(i)); }

  void addPoint(const Point &p) { qpoly_ << p.qpoint(); }
  void addPoint(const QPointF &p) { qpoly_ << p; }

  void setPoint(int i, const Point &p) { qpoly_[i] = p.qpoint(); }
  void setPoint(int i, const QPointF &p) { qpoly_[i] = p; }

  void removePoint() { qpoly_.pop_back(); }

  void resize(int n) { qpoly_.resize(n); }

  bool containsPoint(const Point &p, Qt::FillRule fillRule) const {
    return qpoly_.containsPoint(p.qpoint(), fillRule);
  }

  bool containsPoint(const QPointF &p, Qt::FillRule fillRule) const {
    return qpoly_.containsPoint(p, fillRule);
  }

  BBox boundingBox() const { return BBox(qpoly_.boundingRect()); }

  Point getCenter() const { return boundingBox().getCenter(); }

  //---

  Polygon rotated(double a) const {
    auto c = boundingBox().getCenter();

    return rotated(c, a);
  }

  Polygon rotated(const Point &c, double a) const {
    QPolygonF qpoly;

    for (int i = 0; i < qpoly_.count(); ++i) {
      auto p = Point(qpoly_[i]).rotate(c, a);

      qpoly << p.qpoint();
    }

    return Polygon(qpoly);
  }

  //---

  friend bool operator==(const Polygon &lhs, const Polygon &rhs) {
    return (lhs.qpoly_ == rhs.qpoly_ );
  }

  //---

  double area() const { return CQUtil::polygonArea(qpoly_); }

 private:
  QPolygonF qpoly_;
};

using Polygons = std::vector<Polygon>;

}

//------

namespace CQChartsGeom {

/*!
 * \brief Margin class
 * \ingroup Charts
 */
class Margin {
 public:
  Margin() = default;

  Margin(double l) : l_(l), t_(l), r_(l), b_(l) { }

  Margin(double l, double t, double r, double b) : l_(l), t_(t), r_(r), b_(b) { }

  //---

  double left() const { return l_; }
  void setLeft(double l) { l_ = l; }

  double top() const { return t_; }
  void setTop(double t) { t_ = t; }

  double right() const { return r_; }
  void setRight(double r) { r_ = r; }

  double bottom() const { return b_; }
  void setBottom(double b) { b_ = b; }

  double width () { return left() + right(); }
  double height() { return top() + bottom(); }

  //---

  static BBox insetBBox(const BBox &bbox, const Margin &m) {
    return BBox(bbox.getXMin() + m.left (), bbox.getYMin() + m.bottom(),
                bbox.getXMax() - m.right(), bbox.getYMax() - m.top   ());
  }

  static BBox outsetBBox(const BBox &bbox, const Margin &m) {
    return BBox(bbox.getXMin() - m.left (), bbox.getYMin() - m.bottom(),
                bbox.getXMax() + m.right(), bbox.getYMax() + m.top   ());
  }

 private:
  double l_ { 0.0 };
  double t_ { 0.0 };
  double r_ { 0.0 };
  double b_ { 0.0 };
};

}

//------

namespace CQChartsGeom {

/*!
 * \brief Minimum/Maximum class
 * \ingroup Charts
 */
template<typename T>
class MinMax {
 public:
  MinMax() = default;

  explicit MinMax(const T &t) {
    add(t);
  }

  MinMax(const T &t1, const T &t2) {
    add(t1);
    add(t2);
  }

  void add(const T &t) {
    if (! set_) {
      min_ = t;
      max_ = t;
      set_ = true;
    }
    else {
      min_ = std::min(t, min_);
      max_ = std::max(t, max_);
    }
  }

  bool isSet() const { return set_; }

  void reset() { set_ = false; }

  const T &min() const { assert(set_); return min_; }
  const T &max() const { assert(set_); return max_; }

  T min(const T &t) const { return (set_ ? min_ : t); }
  T max(const T &t) const { return (set_ ? max_ : t); }

 private:
  T    min_ { };
  T    max_ { };
  bool set_ { false };
};

using RMinMax = MinMax<double>;
using IMinMax = MinMax<int>;

}

//------

namespace CQChartsGeom {

/*!
 * \brief Matrix class
 * \ingroup Charts
 *
 * / a b tx \
 * | c d ty |
 * \ 0 0 1  /
 *
 * / m00 m01 m02 \
 * | m10 m11 m12 |
 * \ m20 m21 m22 /
 */
class Matrix :
  public CQChartsComparatorBase<Matrix>,
  public CQChartsToStringBase<Matrix> {
 public:
  enum class Type {
    IDENTITY
  };

 public:
  // constructor/destructor
  Matrix() = default;

 ~Matrix() { }

  explicit Matrix(Type type) {
    if (type == Type::IDENTITY)
      setIdentity();
    else
      assert(false && "Bad Matrix Type");
  }

  Matrix(double a, double b, double c, double d) :
   m00_(a), m01_(b), m10_(c), m11_(d) {
    setBottomIdentity();
  }

  Matrix(double a, double b, double c, double d, double tx, double ty) :
   m00_(a), m01_(b), m02_(tx), m10_(c), m11_(d), m12_(ty) {
    setBottomIdentity();
  }

  Matrix(double m00, double m01, double m02, double m10, double m11, double m12,
            double m20, double m21, double m22) :
   m00_(m00), m01_(m01), m02_(m02), m10_(m10), m11_(m11), m12_(m12),
   m20_(m20), m21_(m21), m22_(m22) {
  }

  Matrix(const double *m, int n) {
    if      (n == 4)
      setValues(m[0], m[1], m[2], m[3]);
    else if (n == 6)
      setValues(m[0], m[1], m[2], m[3], m[4], m[5]);
    else
     assert(false && "Invalid size");
  }

  Matrix *dup() const {
    return new Matrix(*this);
  }

  //------

  // copy operations
  Matrix(const Matrix &a) {
    memcpy(&m00_, &a.m00_, 9*sizeof(double));
  }

  Matrix(Matrix &&a) {
    memcpy(&m00_, &a.m00_, 9*sizeof(double));
  }

  Matrix &operator=(const Matrix &a) {
    if (this != &a)
      memcpy(&m00_, &a.m00_, 9*sizeof(double));

    return *this;
  }

  Matrix &operator=(Matrix &&a) {
    if (this != &a)
      memcpy(&m00_, &a.m00_, 9*sizeof(double));

    return *this;
  }

  //------

  QString toString() const {
    return QString("{{ %1 %2 %3 } { %4_ %5 %6 } { %7 %8 %9}}").
             arg(m00_).arg(m01_).arg(m02_).
             arg(m10_).arg(m11_).arg(m12_).
             arg(m20_).arg(m21_).arg(m22_);
  }

  //------

  static Matrix identity() {
    return Matrix(Type::IDENTITY);
  }

  static Matrix translation(const Point &point) {
    return translation(point.x, point.y);
  }

  static Matrix translation(double tx, double ty) {
    Matrix matrix;

    matrix.setTranslation(tx, ty);

    return matrix;
  }

  static Matrix scale(const Point &point) {
    return scale(point.x, point.y);
  }

  static Matrix scale(double s) {
    Matrix matrix;

    matrix.setScale(s, s);

    return matrix;
  }

  static Matrix scale(double sx, double sy) {
    Matrix matrix;

    matrix.setScale(sx, sy);

    return matrix;
  }

  static Matrix rotation(double a) {
    Matrix matrix;

    matrix.setRotation(a);

    return matrix;
  }

#if 0
  static Matrix skew(double sx, double sy) {
    Matrix matrix;

    matrix.setSkew(sx, sy);

    return matrix;
  }

  static Matrix skewX(double a) {
    Matrix matrix;

    matrix.setSkewX(a);

    return matrix;
  }

  static Matrix skewY(double a) {
    Matrix matrix;

    matrix.setSkewY(a);

    return matrix;
  }
#endif

  static Matrix reflection(double a) {
    Matrix matrix;

    matrix.setReflection(a);

    return matrix;
  }

  static Matrix reflection(double dx, double dy) {
    Matrix matrix;

    matrix.setReflection(dx, dy);

    return matrix;
  }

  void setIdentity() {
    setInnerIdentity ();
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setTranslation(double tx, double ty) {
    setInnerIdentity ();
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  void setScale(double s) {
    setInnerScale    (s, s);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setScale(double sx, double sy) {
    setInnerScale    (sx, sy);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setScaleTranslation(double sx, double sy, double tx, double ty) {
    setInnerScale    (sx, sy);
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  void setScaleTranslation(double s, double tx, double ty) {
    setInnerScale    (s, s);
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  void setRotation(double a) {
    setInnerRotation (a);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setRotationTranslation(double a, double tx, double ty) {
    setInnerRotation (a);
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  void setReflection(double a) {
    setUnitInnerReflection(std::cos(a), std::sin(a));
    setOuterIdentity      ();
    setBottomIdentity     ();
  }

  void setReflection(double dx, double dy) {
    setInnerReflection(dx, dy);
    setOuterIdentity  ();
    setBottomIdentity ();
  }

#if 0
  void setSkew(double x, double y) {
    setInnerSkew     (x, y);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setSkewX(double a) {
    setInnerSkewX    (a);
    setOuterIdentity ();
    setBottomIdentity();
  }

  void setSkewY(double a) {
    setInnerSkewY    (a);
    setOuterIdentity ();
    setBottomIdentity();
  }
#endif

  void setValues(double a, double b, double c, double d) {
    m00_ = a, m01_ = b;
    m10_ = c, m11_ = d;

    setOuterIdentity ();
    setBottomIdentity();
  }

  void setValues(double a, double b, double c, double d, double tx, double ty) {
    m00_ = a, m01_ = b, m02_ = tx;
    m10_ = c, m11_ = d, m12_ = ty;

    setBottomIdentity();
  }

  void setValues(double a, double b, double c, double d, double e, double f,
                 double g, double h, double i) {
    m00_ = a, m01_ = b, m02_ = c;
    m10_ = d, m11_ = e, m12_ = f;
    m20_ = g, m21_ = h, m22_ = i;
  }

  void setValues(const double *v, int n) {
    if      (n == 4)
      setValues(v[0], v[1], v[2], v[3]);
    else if (n == 6)
      setValues(v[0], v[1], v[2], v[3], v[4], v[5]);
    else if (n == 9)
      setValues(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);
    else
      assert(false && "Invalid Size");
  }

  void getValues(double *a, double *b, double *c, double *d) const {
    if (a) *a = m00_;
    if (b) *b = m01_;
    if (c) *c = m10_;
    if (d) *d = m11_;
  }

  void getValues(double *a, double *b, double *c, double *d, double *tx, double *ty) const {
    if (a ) *a  = m00_;
    if (b ) *b  = m01_;
    if (c ) *c  = m10_;
    if (d ) *d  = m11_;
    if (tx) *tx = m02_;
    if (ty) *ty = m12_;
  }

  void getValues(double *v, int n) const {
    if      (n == 4) {
      v[0] = m00_; v[1] = m01_;
      v[2] = m10_; v[3] = m11_;
    }
    else if (n == 6) {
      v[0] = m00_; v[1] = m01_;
      v[2] = m10_; v[3] = m11_;
      v[4] = m02_; v[5] = m12_;
    }
    else if (n == 9) {
      v[0] = m00_; v[1] = m01_; v[2] = m02_;
      v[3] = m10_; v[4] = m11_; v[5] = m12_;
      v[6] = m20_; v[7] = m21_; v[8] = m22_;
    }
    else
      assert(false && "Invalid Size");
  }

  //---------

  void setColumn(int c, double x, double y) {
    switch (c) {
      case 0: m00_ = x; m10_ = y; break;
      case 1: m01_ = x; m11_ = y; break;
    }
  }

  void setColumn(int c, const Point &point) {
    switch (c) {
      case 0: m00_ = point.x; m10_ = point.y; break;
      case 1: m01_ = point.x; m11_ = point.y; break;
    }
  }

  void getColumn(int c, double *x, double *y) const {
    switch (c) {
      case 0: if (x) *x = m00_; if (y) *y = m10_; break;
      case 1: if (x) *x = m01_; if (y) *y = m11_; break;
    }
  }

  //------

  void setRow(int r, double x, double y) {
    switch (r) {
      case 0: m00_ = x; m01_ = y; break;
      case 1: m10_ = x; m11_ = y; break;
    }
  }

  void setRow(int r, const Point &point) {
    switch (r) {
      case 0: m00_ = point.x; m01_ = point.y; break;
      case 1: m10_ = point.x; m11_ = point.y; break;
    }
  }

  void getRow(int r, double *x, double *y) const {
    switch (r) {
      case 0: if (x) *x = m00_; if (y) *y = m01_; break;
      case 1: if (x) *x = m10_; if (y) *y = m11_; break;
    }
  }

  //------

  void multiplyPoint(double xi, double yi, double *xo, double *yo) const {
    *xo = m00_*xi + m01_*yi + m02_;
    *yo = m10_*xi + m11_*yi + m12_;
  }

  void multiplyPoint(const Point &point1, Point &point2) const {
    point2.x = m00_*point1.x + m01_*point1.y + m02_;
    point2.y = m10_*point1.x + m11_*point1.y + m12_;
  }

  void preMultiplyPoint(double xi, double yi, double *xo, double *yo) const {
    *xo = m00_*xi + m10_*yi;
    *yo = m01_*xi + m11_*yi;
  }

  void preMultiplyPoint(const Point &ipoint, Point &opoint) const {
    opoint.x = m00_*ipoint.x + m10_*ipoint.y;
    opoint.y = m01_*ipoint.x + m11_*ipoint.y;
  }

  const Matrix &translate(double x, double y) {
    m02_ += x;
    m12_ += y;

    return *this;
  }

  const Matrix &transpose() {
    std::swap(m10_, m01_);
    std::swap(m20_, m02_);
    std::swap(m21_, m12_);

    return *this;
  }

  Matrix transposed() const {
    return Matrix(m00_, m10_, m20_, m01_, m11_, m21_, m02_, m12_, m22_);
  }

  bool invert(Matrix &imatrix) const {
    double d = determinant();

    if (std::abs(d) == 0.0)
      return false;

    double id = 1.0/d;

    imatrix.m00_ =  id*calcDeterminant(m11_, m12_, m21_, m22_);
    imatrix.m10_ = -id*calcDeterminant(m10_, m12_, m20_, m22_);
    imatrix.m20_ =  id*calcDeterminant(m10_, m11_, m20_, m21_);

    imatrix.m01_ = -id*calcDeterminant(m01_, m02_, m21_, m22_);
    imatrix.m11_ =  id*calcDeterminant(m00_, m02_, m20_, m22_);
    imatrix.m21_ = -id*calcDeterminant(m00_, m01_, m20_, m21_);

    imatrix.m02_ =  id*calcDeterminant(m01_, m02_, m11_, m12_);
    imatrix.m12_ = -id*calcDeterminant(m00_, m02_, m10_, m12_);
    imatrix.m22_ =  id*calcDeterminant(m00_, m01_, m10_, m11_);

    return true;
  }

  Matrix inverse() const {
    Matrix imatrix;

    if (! invert(imatrix))
      assert(false && "Divide by zero");

    return imatrix;
  }

  double determinant() const {
    return (m00_*calcDeterminant(m11_, m12_, m21_, m22_) -
            m01_*calcDeterminant(m10_, m12_, m20_, m22_) +
            m02_*calcDeterminant(m10_, m11_, m20_, m21_));
  }

  const Matrix &normalize() {
    double d = determinant();

    double id = 1.0/d;

    m00_ *= id;
    m01_ *= id;
    m10_ *= id;
    m11_ *= id;

    return *this;
  }

  void setTransform(double xmin1, double ymin1, double xmax1, double ymax1,
                    double xmin2, double ymin2, double xmax2, double ymax2) {
    double sx = (xmax2 - xmin2)/(xmax1 - xmin1);
    double sy = (ymax2 - ymin2)/(ymax1 - ymin1);

    double tx = -xmin1*sx + xmin2;
    double ty = -ymin1*sy + ymin2;

    setInnerScale    (sx, sy);
    setOuterTranslate(tx, ty);
    setBottomIdentity();
  }

  static Matrix *newIdentityMatrix() {
    Matrix *m = new Matrix();

    m->setIdentity();

    return m;
  }

  static bool solveAXeqB(const Matrix &a, Point &x, const Point &b) {
    double det_a = a.determinant();

    if (std::abs(det_a) <= 0.0)
      return false;

    double idet_a = 1.0/det_a;

    Matrix t(a);

    t.setColumn(0, b.x, b.y);

    double det_t = t.determinant();

    x.x = det_t*idet_a;

    t = a;

    t.setColumn(1, b.x, b.y);

    det_t = t.determinant();

    x.y = det_t*idet_a;

    return true;
  }

  //------

  bool isIdentity() const {
    return isInnerIdentity() && isTranslateIdentity() &&
           realEq(m20_, 0.0) && realEq(m21_, 0.0) && realEq(m22_, 1.0);
  }

  bool isInnerIdentity() const {
    return realEq(m00_, 1.0) && realEq(m01_, 0.0) &&
           realEq(m10_, 0.0) && realEq(m11_, 1.0);
  }

  bool isTranslateIdentity() const {
    return realEq(m02_, 0.0) && realEq(m12_, 0.0);
  }

  bool isInnerRotation() const {
    return ! realEq(m01_, 0.0) && ! realEq(m10_, 0.0);
  }

  double getAngle() const {
    return std::asin(m10_);
  }

  void getSize(double *sx, double *sy) const {
    *sx = std::abs(m00_ + m01_);
    *sy = std::abs(m10_ + m11_);
  }

  void getTranslate(double *tx, double *ty) const {
    *tx = m02_;
    *ty = m12_;
  }

  //------

  void zero() { memset(&m00_, 0, 9*sizeof(double)); }

  const Matrix &operator+=(const Matrix &b) {
    m00_ += b.m00_; m01_ += b.m01_; m02_ += b.m02_;
    m10_ += b.m10_; m11_ += b.m11_; m12_ += b.m12_;
    m20_ += b.m20_; m21_ += b.m21_; m22_ += b.m22_;

    return *this;
  }

  Matrix operator+(const Matrix &b) const {
    Matrix c = *this;

    c += b;

    return c;
  }

  const Matrix &operator-=(const Matrix &b) {
    m00_ -= b.m00_; m01_ -= b.m01_, m02_ -= b.m02_;
    m10_ -= b.m10_; m11_ -= b.m11_; m12_ -= b.m12_;
    m20_ -= b.m20_; m21_ -= b.m21_; m22_ -= b.m22_;

    return *this;
  }

  Matrix operator-(const Matrix &b) const {
    Matrix c = *this;

    c -= b;

    return c;
  }

  const Matrix &operator*=(const Matrix &b) {
    Matrix a = *this;

    m00_ = a.m00_*b.m00_ + a.m01_*b.m10_ + a.m02_*b.m20_;
    m01_ = a.m00_*b.m01_ + a.m01_*b.m11_ + a.m02_*b.m21_;
    m02_ = a.m00_*b.m02_ + a.m01_*b.m12_ + a.m02_*b.m22_;

    m10_ = a.m10_*b.m00_ + a.m11_*b.m10_ + a.m12_*b.m20_;
    m11_ = a.m10_*b.m01_ + a.m11_*b.m11_ + a.m12_*b.m21_;
    m12_ = a.m10_*b.m02_ + a.m11_*b.m12_ + a.m12_*b.m22_;

    m20_ = a.m20_*b.m00_ + a.m21_*b.m10_ + a.m22_*b.m20_;
    m21_ = a.m20_*b.m01_ + a.m21_*b.m11_ + a.m22_*b.m21_;
    m22_ = a.m20_*b.m02_ + a.m21_*b.m12_ + a.m22_*b.m22_;

    return *this;
  }

  Matrix operator*(const Matrix &b) const {
    Matrix c = *this;

    c *= b;

    return c;
  }

  const Matrix &operator*=(double s) {
    Matrix a = *this;

    m00_ = a.m00_*s; m01_ = a.m01_*s; m02_ = a.m02_*s;
    m10_ = a.m10_*s; m11_ = a.m11_*s; m12_ = a.m12_*s;
    m20_ = a.m20_*s; m21_ = a.m21_*s; m22_ = a.m22_*s;

    return *this;
  }

  Matrix operator*(double s) const {
    Matrix c = *this;

    c *= s;

    return c;
  }

  friend Point operator*(const Matrix &m, const Point &p) {
    Point p1;

    m.multiplyPoint(p, p1);

    return p1;
  }

  friend Point operator*(const Point &p, const Matrix &m) {
    Point p1;

    m.preMultiplyPoint(p, p1);

    return p1;
  }

  const Matrix &operator/=(const Matrix &b) {
    Matrix bi;

    if (! b.invert(bi)) {
      assert(false && "Divide by zero");
      return *this;
    }

    return (*this) *= bi;
  }

  Matrix operator/(const Matrix &b) const {
    Matrix c = *this;

    c /= b;

    return c;
  }

  //------

  void setValue(uint i, double value) {
    (&m00_)[i] = value;
  }

  void setValue(uint i, uint j, double value) {
    assert(i < 3 && j < 3);

    double &m = (&m00_)[3*j + i];

    m = value;
  }

  const double &getValue(uint i) const {
    assert(i < 9);

    return (&m00_)[i];
  }

  const double &getValue(uint i, uint j) const {
    assert(i < 3 && j < 3);

    const double &m = (&m00_)[3*j + i];

    return m;
  }

  const double &operator[](uint i) const {
    assert(i < 9);

    return (&m00_)[i];
  }

  double &operator[](uint i) {
    assert(i < 9);

    return (&m00_)[i];
  }

  //------

  void setInnerIdentity() {
    m00_ = 1.0, m01_ = 0.0;
    m10_ = 0.0, m11_ = 1.0;
  }

  void setInnerScale(double sx, double sy) {
    m00_ = sx , m01_ = 0.0;
    m10_ = 0.0, m11_ = sy ;
  }

  void setInnerRotation(double a) {
    double c = std::cos(a);
    double s = std::sin(a);

    m00_ =  c, m01_ = -s;
    m10_ =  s, m11_ =  c;
  }

#if 0
  void setInnerSkew(double x, double y) {
    double tx = std::tan(x);
    double ty = std::tan(y);

    m00_ = 1 , m01_ = tx;
    m10_ = ty, m11_ = 1 ;
  }

  void setInnerSkewX(double x) {
    double tx = std::tan(x);

    m00_ = 1, m01_ = tx;
    m10_ = 0, m11_ = 1 ;
  }

  void setInnerSkewY(double y) {
    double ty = std::tan(y);

    m00_ = 1 , m01_ = 0;
    m10_ = ty, m11_ = 1;
  }
#endif

  void setInnerReflection(double dx, double dy) {
    double l = std::hypot(dx, dy);

    setUnitInnerReflection(dx/l, dy/l);
  }

  void setUnitInnerReflection(double dx, double dy) {
    //m00_ = (dx*dx - dy*dy)/l; m01_ = 2*dx*dy/l;
    //m10_ = m01_             ; m11_ = -m00_;

    m00_ = 2.0*dx*dx - 1.0; m01_ = 2.0*dx*dy;
    m10_ = m01_           ; m11_ = 2.0*dy*dy - 1.0;
  }

  void setOuterIdentity() {
    m02_ = 0.0; m12_ = 0.0;
  }

  void setOuterTranslate(double tx, double ty) {
    m02_ = tx; m12_ = ty;
  }

  void setBottomIdentity() {
    m20_ = 0.0, m21_ = 0.0, m22_ = 1.0;
  }

  //---

  int cmp(const Matrix &m) const {
    { if (m00_ < m.m00_) return -1; } { if (m00_ > m.m00_) return 1; }
    { if (m10_ < m.m10_) return -1; } { if (m10_ > m.m10_) return 1; }
    { if (m01_ < m.m01_) return -1; } { if (m01_ > m.m01_) return 1; }
    { if (m11_ < m.m11_) return -1; } { if (m11_ > m.m11_) return 1; }
    { if (m02_ < m.m02_) return -1; } { if (m02_ > m.m02_) return 1; }
    { if (m12_ < m.m12_) return -1; } { if (m12_ > m.m12_) return 1; }
    { if (m20_ < m.m20_) return -1; } { if (m20_ > m.m20_) return 1; }
    { if (m21_ < m.m21_) return -1; } { if (m21_ > m.m21_) return 1; }
    { if (m22_ < m.m22_) return -1; } { if (m22_ > m.m22_) return 1; }

    return 0;
  }

  friend int cmp(const Matrix &m1, const Matrix &m2) {
    return m1.cmp(m2);
  }

 private:
  static bool realEq(double r1, double r2) {
    return (std::abs((r1) - (r2)) < 1E-5);
  }

  static double calcDeterminant(double m00, double m01, double m10, double m11) {
    return m00*m11 - m01*m10;
  }

 private:
  double m00_ { 0.0 }, m01_ { 0.0 }, m02_ { 0.0 };
  double m10_ { 0.0 }, m11_ { 0.0 }, m12_ { 0.0 };
  double m20_ { 0.0 }, m21_ { 0.0 }, m22_ { 0.0 };
};

}

//------

namespace CQChartsGeom {

//! value in range
struct RangeValue {
  RangeValue(double v=0.0, double min=0.0, double max=1.0) :
   v(v), min(min), max(max) {
  }

  double map() const { return map(v); }

  double map  (double v1) const { return CMathUtil::map(v1, min, max, 0.0, 1.0); }
  double unmap(double v1) const { return CMathUtil::map(v1, 0.0, 1.0, min, max); }

  double v   { 0.0 };
  double min { 0.0 };
  double max { 1.0 };
};

}

//------

namespace CQChartsGeom {

using Point = CQChartsGeom::Point;

// point on circle perimeter (center (c), radius(r), radian angle (a))
inline Point circlePoint(const Point &c, double r, double a) {
  double ca = std::cos(a); double sa = std::sin(a);

  return Point(c.x + r*ca, c.y + r*sa);
}

inline Point ellipsePoint(const Point &c, double xr, double yr, double a) {
  double ca = std::cos(a); double sa = std::sin(a);

  return Point(c.x + xr*ca, c.y + yr*sa);
}

#if 0
inline Point ellipsePoint(const Point &c, double xr, double yr, double a) {
  double ca = std::cos(a); double sa = std::sin(a);

  double n = xr*yr;
  double d = std::hypot(xr*sa, yr*ca);

  double rt = (d > 0.0 ? n/d : 0.0);

  return Point(c.x + rt*ca, c.y + rt*sa);
}
#endif

inline double pointAngle(const Point &p1, const Point &p2) {
  return std::atan2(p2.y - p1.y, p2.x - p1.x);
}

inline Point movePointOnLine(const Point &p, double a, double d) {
  return Point(p.x + d*std::cos(a), p.y + d*std::sin(a));
}

inline Point movePointPerpLine(const Point &p, double a, double d) {
  return Point(p.x + d*std::sin(a), p.y - d*std::cos(a));
}

}

//------

namespace CQChartsGeom {

using Points = std::vector<Point>;

bool lineIntersectCircle(const BBox &rect, const Point &p1, const Point &p2, Point &pi);
bool lineIntersectRect(const BBox &rect, const Point &p1, const Point &p2, Point &pi);
bool lineIntersectPolygon(const Points &points, const Point &p1, const Point &p2, Point &pi);

}

//------

namespace CQChartsGeom {

/*!
 * \brief Point class
 * \ingroup Charts
 */
class Point3D :
  public CQChartsComparatorBase<Point3D> {
 public:
  Point3D() = default;

  Point3D(double x1, double y1, double z1) :
   x(x1), y(y1), z(z1) {
  }

  //---

  // dot product
  double dotProduct(const Point3D &p) const {
    return (x*p.x + y*p.y + z*p.z);
  }

  //---

  Point3D &operator+=(const Point3D &rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

  Point3D operator+(const Point3D &rhs) const { return Point3D(x + rhs.x, y + rhs.y, z + rhs.z); }

  Point3D &operator/=(double rhs) {
    double irhs = 1.0/rhs; x *= irhs; y *= irhs; z *= irhs; return *this; }

  friend Point3D operator/(const Point3D &lhs, double rhs) {
    double irhs = 1.0/rhs;

    return Point3D(lhs.x*irhs, lhs.y*irhs, lhs.z*irhs);
  }

  //---

  int cmp(const Point3D &p) const {
    { if (x < p.x) return -1; } { if (x > p.x) return 1; }
    { if (y < p.y) return -1; } { if (y > p.y) return 1; }
    { if (z < p.z) return -1; } { if (z > p.z) return 1; }

    return 0;
  }

  friend int cmp(const Point3D &p1, const Point3D &p2) {
    return p1.cmp(p2);
  }

  //---

  Point point2D() const { return Point(x, y); }

  //-----

  QString toString() const;
  bool fromString(const QString &s);

 public:
  double x { 0 };
  double y { 0 };
  double z { 0 };
};

/*!
 * \brief Range 3D class
 * \ingroup Charts
 */
class Range3D {
 public:
  Range3D() = default;

  Range3D(double x1, double y1, double z1, double x2, double y2, double z2) :
   set_(true), x1_(x1), y1_(y1), z1_(z1), x2_(x2), y2_(y2), z2_(z2) {
  }

  bool isSet() const { return set_; }

  double xmin() const { assert(set_); return std::min(x1_, x2_); }
  double ymin() const { assert(set_); return std::min(y1_, y2_); }
  double zmin() const { assert(set_); return std::min(z1_, z2_); }
  double xmax() const { assert(set_); return std::max(x1_, x2_); }
  double ymax() const { assert(set_); return std::max(y1_, y2_); }
  double zmax() const { assert(set_); return std::max(z1_, z2_); }

  double xmid() const { assert(set_); return CMathUtil::avg(x1_, x2_); }
  double ymid() const { assert(set_); return CMathUtil::avg(y1_, y2_); }
  double zmid() const { assert(set_); return CMathUtil::avg(z1_, z2_); }

  double xsize() const { return xmax() - xmin(); }
  double ysize() const { return ymax() - ymin(); }
  double zsize() const { return zmax() - zmin(); }

  void setXMin(double x) { x1_ = x; set_ = true; }
  void setYMin(double y) { y1_ = y; set_ = true; }
  void setZMin(double z) { z1_ = z; set_ = true; }
  void setXMax(double x) { x2_ = x; set_ = true; }
  void setYMax(double y) { y2_ = y; set_ = true; }
  void setZMax(double z) { z2_ = z; set_ = true; }

  void updateRange(double x, double y, double z) {
    if (! set_) {
      x1_ = x; y1_ = y; z1_ = z;
      x2_ = x; y2_ = y; z2_ = z;

      set_ = true;
    }
    else {
      x1_ = std::min(x1_, x); y1_ = std::min(y1_, y); z1_ = std::min(z1_, z);
      x2_ = std::max(x2_, x); y2_ = std::max(y2_, y); z2_ = std::max(z2_, z);
    }
  }

 private:
  bool   set_ { false };
  double x1_ { 0 }, y1_ { 0 }, z1_ { 0 }, x2_ { 0 }, y2_ { 0 }, z2_ { 0 };
};

/*!
 * \brief Polygon 3D class
 * \ingroup Charts
 */
class Polygon3D {
 public:
  using Points = std::vector<Point3D>;

 public:
  Polygon3D() = default;

  const Points &points() const { return points_; }

  void addPoint(const Point3D &p) {
    points_.push_back(p);
  }

 private:
  Points points_;
};

}

//------

namespace CQChartsGeom {

void registerMetaTypes();

}

CQUTIL_DCL_META_TYPE(CQChartsGeom::BBox)
CQUTIL_DCL_META_TYPE(CQChartsGeom::Point)
CQUTIL_DCL_META_TYPE(CQChartsGeom::Point3D)

#endif
