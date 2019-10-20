#ifndef CQChartsGrahamHull_H
#define CQChartsGrahamHull_H

#include <QPolygonF>
#include <QPointF>
#include <QRectF>

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

  void addPoint(const QPointF &point);

  void getHull(QPolygonF &points) const;

  void draw(const CQChartsPlot *plot, CQChartsPaintDevice *device) const;

  QRectF bbox() const;

 private:
  bool constCalc() const;
  bool calc();

  void sortLowestClockwise();
  void squash();
  void findLowest();
  bool doScan();

  static bool pointLineLeft(const QPointF &a, const QPointF &b, const QPointF &c);

  static int areaSign(const QPointF &a, const QPointF &b, const QPointF &c);

 private:
  typedef std::vector<int> IPoints;
  typedef std::set<int>    DelPoints;

  QPolygonF points_;
  bool      needsCalc_ { true };
  IPoints   ipoints_;
  DelPoints delPoints_;
  bool      rc_        { false };
};

#endif
