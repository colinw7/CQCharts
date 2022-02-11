#ifndef CQChartsGridCell_H
#define CQChartsGridCell_H

#include <CInterval.h>
#include <map>

/*!
 * \brief Grid for Values in Cells by Buckets/Intervals
 * \ingroup Charts
 */
class CQChartsGridCell {
 public:
  using Point    = CQChartsGeom::Point;
  using Points   = std::vector<Point>;
  using YPoints  = std::map<int, Points>;
  using XYPoints = std::map<int, YPoints>;

 public:
  CQChartsGridCell() { }

  int nx() const { return nx_; }
  void setNX(int i) { nx_ = i; }

  int ny() const { return ny_; }
  void setNY(int i) { ny_ = i; }

  int maxN() const { return maxN_; }
  void setMaxN(int i) { maxN_ = i; }

  void setXInterval(double xmin, double xmax) {
    xinterval_.setStart   (xmin);
    xinterval_.setEnd     (xmax);
    xinterval_.setNumMajor(nx_);
  }

  void resetXInterval() {
    xinterval_.setStart   (0);
    xinterval_.setEnd     (1);
    xinterval_.setNumMajor(1);
  }

  void setYInterval(double ymin, double ymax) {
    yinterval_.setStart   (ymin);
    yinterval_.setEnd     (ymax);
    yinterval_.setNumMajor(ny_);
  }

  void resetYInterval() {
    yinterval_.setStart   (0);
    yinterval_.setEnd     (1);
    yinterval_.setNumMajor(1);
  }

  double xStart() const { return xinterval_.calcStart(); }
  double xEnd  () const { return xinterval_.calcEnd  (); }

  double yStart() const { return yinterval_.calcStart(); }
  double yEnd  () const { return yinterval_.calcEnd  (); }

  void xIValues(int i, double &xmin, double &xmax) const {
    xinterval_.intervalValues(i, xmin, xmax);
  }

  void yIValues(int i, double &ymin, double &ymax) const {
    yinterval_.intervalValues(i, ymin, ymax);
  }

  int xValueInterval(double x) const { return xinterval_.valueInterval(x); }
  int yValueInterval(double y) const { return yinterval_.valueInterval(y); }

  void resetPoints() {
    xyPoints_.clear();

    maxN_ = 0;
  }

  void addPoint(const Point &p) {
    int ix = xValueInterval(p.x);
    int iy = yValueInterval(p.y);

    Points &points = xyPoints_[ix][iy];

    points.push_back(p);

    maxN_ = std::max(maxN_, int(points.size()));
  }

  const XYPoints &xyPoints() const { return xyPoints_; }

  const Points &points(int ix, int iy) const {
    auto px = xyPoints_.find(ix); assert(px != xyPoints_.end());

    const YPoints &yPoints = (*px).second;
    auto py = yPoints.find(iy); assert(py != yPoints.end());

    return (*py).second;
  }

  int numPoints(int ix, int iy) const {
    return int(points(ix, iy).size());
  }

 private:
  int       nx_        { 100 }; //!< number of x grid intervals
  int       ny_        { 100 }; //!< number of y grid intervals
  CInterval xinterval_;         //!< x point range
  CInterval yinterval_;         //!< y point range
  XYPoints  xyPoints_;          //!< grid cell points
  int       maxN_      { 0 };   //!< maximum number of points in grid cell
};

#endif
