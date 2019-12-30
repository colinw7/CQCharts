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
  CQChartsGrahamHull();

  int numPoints() const { return points_.size(); }

  void clear();

  void addPoint(const CQChartsGeom::Point &point);

  void getHull(CQChartsGeom::Polygon &points) const;

  void draw(const CQChartsPlot *plot, CQChartsPaintDevice *device) const;

  CQChartsGeom::BBox bbox() const;

 private:
  bool constCalc() const;
  bool calc();

  void sortLowestClockwise();
  void squash();
  void findLowest();
  bool doScan();

  static bool pointLineLeft(const CQChartsGeom::Point &a, const CQChartsGeom::Point &b,
                            const CQChartsGeom::Point &c);

  static int areaSign(const CQChartsGeom::Point &a, const CQChartsGeom::Point &b,
                      const CQChartsGeom::Point &c);

 private:
  using Points    = std::vector<CQChartsGeom::Point>;
  using IPoints   = std::vector<int>;
  using DelPoints = std::set<int>;

  Points    points_;
  bool      needsCalc_ { true };
  IPoints   ipoints_;
  DelPoints delPoints_;
  bool      rc_        { false };
};

#endif
