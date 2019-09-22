#ifndef CQChartsGrahamHull_H
#define CQChartsGrahamHull_H

#include <QPointF>

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
  typedef std::vector<QPointF> Points;

 public:
  CQChartsGrahamHull();

  int numPoints() const { return points_.size(); }

  void addPoint(const QPointF &point);

  bool calc();

  void getHull(Points &points) const;

  void draw(const CQChartsPlot *plot, CQChartsPaintDevice *device) const;

 private:
  void sortLowestClockwise();
  void squash();
  void findLowest();
  bool doScan();

  static bool pointLineLeft(const QPointF &a, const QPointF &b, const QPointF &c);

  static int areaSign(const QPointF &a, const QPointF &b, const QPointF &c);

 private:
  typedef std::vector<int> IPoints;
  typedef std::set<int>    DelPoints;

  Points    points_;
  IPoints   ipoints_;
  DelPoints del_points_;
};

#endif
