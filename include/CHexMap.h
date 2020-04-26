#ifndef CHexMap_H
#define CHexMap_H

#include <vector>
#include <map>
#include <cmath>
#include <iostream>
#include <cassert>

template<class DATA>
class CHexMap {
 public:
  struct Point {
    Point(double x, double y) :
     x(x), y(y) {
    }

    double x { 0.0 };
    double y { 0.0 };
  };

  using Polygon = std::vector<Point>;

  //---

  using DataArray = std::vector<DATA *>;
  using JData     = std::map<int, DataArray>;
  using IJData    = std::map<int, JData>;

 public:
  CHexMap() {
    setNum(50);
  }

  CHexMap(double s) {
    setSize(s);
  }

  double size() const { return s_; }

  void setSize(double s) {
    s_ = s;
    r_ = 2.0*s_/sqrt(3.0);

    t1_ = 1.0/sqrt(3);
  }

  double dsize() const { return r_; }

  void setRange(double xmin, double ymin, double xmax, double ymax) {
    xmin_ = xmin;
    ymin_ = ymin;
    xmax_ = xmax;
    ymax_ = ymax;
  }

  void getRange(double &xmin, double &ymin, double &xmax, double &ymax) {
    xmin = xmin_;
    ymin = ymin_;
    xmax = xmax_;
    ymax = ymax_;
  }

  void setNum(int n) {
    assert(n > 0);

    setSize((xmax_ - xmin_)/(2.0*n));
  }

  double getXMin() const { return xmin_; }
  double getYMin() const { return ymin_; }
  double getXMax() const { return xmax_; }
  double getYMax() const { return ymax_; }

  double getXMid() const { return (xmin_ + xmax_)/2.0; }
  double getYMid() const { return (ymin_ + ymax_)/2.0; }

  double getXSize() const { return xmax_ - xmin_; }
  double getYSize() const { return ymax_ - ymin_; }

  void addPoint(const Point &p) {
    int i, j;

    pointToPos(p, i, j);

    data_[i][j].push_back(nullptr);

    dataArray_.push_back(nullptr);
  }

  void addData(DATA *data) {
    double x = data->x();
    double y = data->y();

    int i, j;

    pointToPos(Point(x, y), i, j);

    data_[i][j].push_back(data);

    dataArray_.push_back(data);
  }

  int numData() const { return dataArray_.size(); }

  int numData(int i, int j) const {
    auto pi = data_.find(i);
    if (pi == data_.end()) return 0;

    auto pj = (*pi).second.find(j);
    if (pj == (*pi).second.end()) return 0;

    return (*pj).second.size();
  }

  void clear() {
    data_.clear();

    dataArray_.clear();
  }

  void print() const {
    for (const auto &pj : data_) {
      int j = pj.first;

      for (const auto &pi : pj.second) {
        int i = pi.first;

        std::cerr << i << ":" << j << " " << pi.second.size() << "\n";
      }
    }
  }

  const IJData &data() const { return data_; }

  void pointToPos(const Point &p, int &i, int &j) {
    double dx = p.x;

    i = std::round(dx/s_);

    //--

    if (i & 1) {
      double dy = p.y - 1.5*r_;

      j = std::round(dy/(3.0*r_));
    }
    else {
      double dy = p.y;

      j = std::round(dy/(3.0*r_));
    }
  }

  void indexPolygon(int ix, int iy, Polygon &polygon) const {
    double dy = (std::abs(ix) & 1 ? 1.5*r_ : 0.0);

    double x2 = ix*s_;
    double x1 = x2 - s_;
    double x3 = x2 + s_;

    double y2 = 3.0*r_*iy;
    double y1 = y2 - r_;
    double y3 = y2 + r_;

    Point p1(x1, y2 - r_/2.0 + dy);
    Point p2(x1, y2 + r_/2.0 + dy);
    Point p3(x2, y3          + dy);
    Point p4(x3, y2 + r_/2.0 + dy);
    Point p5(x3, y2 - r_/2.0 + dy);
    Point p6(x2, y1          + dy);

    polygon.push_back(p1);
    polygon.push_back(p2);
    polygon.push_back(p3);
    polygon.push_back(p4);
    polygon.push_back(p5);
    polygon.push_back(p6);
  }

  void xLinePos(int ix, double &x) const {
    x = ix*s_;
  }

  void xLineRange(int ix, double &xmin, double &xmax) const {
    double xm = ix*s_;

    xmin = xm - s_;
    xmax = xm + s_;
  }

  void yLinePos(int iy, double &y1, double &y2) const {
    double xmid = getXMid();
    double ymid = getYMid();

    double x = xmid + 4.0*s_*iy;

    y1 = ymid - t1_*(x - getXMin());
    y2 = ymid + t1_*(getXMax() - x);
  }

  void yLineRange(int iy, double &y11, double &y12, double &y21, double &y22) const {
    double xmid = getXMid();
    double ymid = getYMid();

    double x = xmid + 4.0*s_*iy;

    double x1 = x - 2.0*s_;
    double x2 = x + 2.0*s_;

    y11 = ymid - t1_*(x1 - getXMin());
    y12 = ymid + t1_*(getXMax() - x1);

    y21 = ymid - t1_*(x2 - getXMin());
    y22 = ymid + t1_*(getXMax() - x2);
  }

  void zLinePos(int iz, double &z1, double &z2) const {
    double xmid = getXMid();
    double ymid = getYMid();

    double x = xmid - 2.0*s_*iz;

    z1 = ymid + t1_*(x - getXMin());
    z2 = ymid - t1_*(getXMax() - x);
  }

  void zLineRange(int iz, double &z11, double &z12, double &z21, double &z22) const {
    double xmid = getXMid();
    double ymid = getYMid();

    double x = xmid + 2.0*s_*iz;

    double x1 = x - 2.0*s_;
    double x2 = x + 2.0*s_;

    z11 = ymid + t1_*(x1 - getXMin());
    z12 = ymid - t1_*(getXMax() - x1);

    z21 = ymid + t1_*(x2 - getXMin());
    z22 = ymid - t1_*(getXMax() - x2);
  }

  static double pointLineDistance(const Point &point, const Point &line1, const Point &line2) {
    double dx1 = line2.x - line1.x;
    double dy1 = line2.y - line1.y;

    double u2 = dx1*dx1 + dy1*dy1;
    assert(u2 > 0.0);

    double dx2 = point.x - line1.x;
    double dy2 = point.y - line1.y;

    double u1 = dx2*dx1 + dy2*dy1;

    double u = u1/u2;

    auto intersection = Point(line1.x + u*dx1, line1.y + u*dy1);

    return pointPointDistance(point, intersection);
  }

  static double pointPointDistance(const Point &point1, const Point &point2) {
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;

    return std::hypot(dx, dy);
  }

  static bool intersectLines(const Point &l1s, const Point &l1e,
                             const Point &l2s, const Point &l2e, Point &pi) {
    double dx1 = l1e.x - l1s.x;
    double dy1 = l1e.y - l1s.y;
    double dx2 = l2e.x - l2s.x;
    double dy2 = l2e.y - l2s.y;

    double delta = dx1*dy2 - dy1*dx2;

    if (fabs(delta) < 1E-6) // parallel
      return false;

    double idelta = 1.0/delta;

    double dx = l2s.x - l1s.x;
    double dy = l2s.y - l1s.y;

    double m1 = (dx*dy2 - dy*dx2)*idelta;
  //double m2 = (dx*dy1 - dy*dx1)*idelta;

    double xi = l1s.x + m1*dx1;
    double yi = l1s.y + m1*dy1;

    pi = Point(xi, yi);

    return true;
  }

  static bool pointLineLeft(const Point &a, const Point &b, const Point &c) {
    return areaSign(a, b, c) > 0;
  }

  static int areaSign(const Point &a, const Point &b, const Point &c) {
    double area2 = (b.x- a.x)*(c.y - a.y) - (c.x- a.x)*(b.y - a.y);

    if      (area2 > 0.0) return  1;
    else if (area2 < 0.0) return -1;
    else                  return  0;
  }

 private:
  double    s_    { 1.0 };
  double    r_    { 0.0 };
  double    t1_   { 1.0 };
  double    xmin_ { -1.0 };
  double    ymin_ { -1.0 };
  double    xmax_ {  1.0 };
  double    ymax_ {  1.0 };
  IJData    data_;
  DataArray dataArray_;
};

#endif
