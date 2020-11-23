#ifndef CQChartsGrahamHull_H
#define CQChartsGrahamHull_H

#include <CQChartsGeom.h>

#include <vector>
#include <set>

class CQChartsPlot;
class CQChartsPaintDevice;

/*!
 * \brief Convex hull using Graham's algorithm
 * \ingroup Charts
 */
class CQChartsGrahamHull {
 public:
  using Plot        = CQChartsPlot;
  using PaintDevice = CQChartsPaintDevice;
  using Point       = CQChartsGeom::Point;
  using BBox        = CQChartsGeom::BBox;
  using Polygon     = CQChartsGeom::Polygon;

 public:
  CQChartsGrahamHull();

  int numPoints() const { return points_.size(); }

  void clear();

  void addPoint(const Point &point);

  void getHull(Polygon &points) const;

  void draw(PaintDevice *device) const;

  BBox bbox() const;

  double area() const;

 private:
  bool constCalc() const;
  bool calc();

  void sortLowestClockwise();
  void squash();
  void findLowest();
  bool doScan();

  static bool pointLineLeft(const Point &a, const Point &b, const Point &c);

  static int areaSign(const Point &a, const Point &b, const Point &c);

 private:
  using Points    = std::vector<Point>;
  using IPoints   = std::vector<int>;
  using DelPoints = std::set<int>;

  Points    points_;
  bool      needsCalc_ { true };
  IPoints   ipoints_;
  DelPoints delPoints_;
  bool      rc_        { false };
};

#endif
