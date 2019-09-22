#include <CQChartsGrahamHull.h>
#include <CQChartsPlot.h>
#include <CQChartsPaintDevice.h>

#include <cstdlib>
#include <cmath>
#include <cassert>

CQChartsGrahamHull::
CQChartsGrahamHull()
{
}

void
CQChartsGrahamHull::
addPoint(const QPointF &point)
{
  points_.push_back(point);
}

bool
CQChartsGrahamHull::
calc()
{
  int num_points = points_.size();

  if (num_points < 3)
    return false;

  // set point indices
  ipoints_.resize(num_points);

  for (int i = 0; i < num_points; ++i)
    ipoints_[i] = i;

  // find lowest point
  findLowest();

  // sort against lowest clockwise
  sortLowestClockwise();

  // remove colinear
  squash();

  int num_ipoints = ipoints_.size();

  if (num_ipoints < 3)
    return false;

  return doScan();
}

bool
CQChartsGrahamHull::
doScan()
{
  // do graham scan
  IPoints ipoint_stack;

  ipoint_stack.push_back(ipoints_[0]);
  ipoint_stack.push_back(ipoints_[1]);

  int i = 2;

  int num_ipoints = ipoints_.size();

  while (i < num_ipoints) {
    uint ns = ipoint_stack.size();

    if (ns < 2) {
      ipoints_.clear();
      return false;
    }

    int i1 = ipoint_stack[ns - 2];
    int i2 = ipoint_stack[ns - 1];
    int i3 = ipoints_[i];

    if (pointLineLeft(points_[i1], points_[i2], points_[i3])) {
      ipoint_stack.push_back(i3);

      ++i;
    }
    else
      ipoint_stack.pop_back();
  }

  // set return points
  ipoints_ = ipoint_stack;

  return true;
}

void
CQChartsGrahamHull::
getHull(Points &points) const
{
  int num_ipoints = ipoints_.size();

  points.resize(num_ipoints);

  for (int i = 0; i < num_ipoints; ++i)
    points[i] = points_[ipoints_[i]];
}

void
CQChartsGrahamHull::
findLowest()
{
  int num_points = ipoints_.size();

  int     min_i = 0;
  QPointF min_p = points_[ipoints_[min_i]];

  for (int i = 1; i < num_points; ++i) {
    const QPointF &p = points_[ipoints_[i]];

    if (p.y() < min_p.y() || (p.y() == min_p.y() && p.x() > min_p.x())) {
      min_i = i;
      min_p = p;
    }
  }

  if (min_i > 0)
    std::swap(ipoints_[0], ipoints_[min_i]);
}

void
CQChartsGrahamHull::
sortLowestClockwise()
{
  if (ipoints_.size() < 3)
    return;

  // sort points by angle they make with lowest point and x-axis
  del_points_.clear();

  auto p1 = ipoints_.begin(); ++p1; // first unsorted point;
  auto p2 = ipoints_.end();

  int i0 = ipoints_[0];

  std::sort(p1, p2, [&](const int &i1, const int &i2) {
    const QPointF &p0 = points_[i0];
    const QPointF &p1 = points_[i1];
    const QPointF &p2 = points_[i2];

    int as = areaSign(p0, p1, p2);

    if (as > 0) return true;
    if (as < 0) return false;

    // zero area (p1 and/or p2 are colinear)
    double x = fabs(p1.x() - p0.x()) - fabs(p2.x() - p0.x());
    double y = fabs(p1.y() - p0.y()) - fabs(p2.y() - p0.y());

    // remove colinear points

    if (x < 0 || y < 0) {
      del_points_.insert(i1);
      return true;
    }

    if (x > 0 || y > 0) {
      del_points_.insert(i2);
      return false;
    }

    // p1 and p2 are coincident
    if (i1 > i2)
      del_points_.insert(i2);
    else
      del_points_.insert(i1);

    return false;
  });
}

void
CQChartsGrahamHull::
squash()
{
  if (del_points_.empty())
    return;

  // remove colinear points
  int num_ipoints = ipoints_.size();

  int j = 0;

  for (int i = 0; i < num_ipoints; ++i) {
    int pi = ipoints_[i];

    if (del_points_.find(pi) == del_points_.end())
      ipoints_[j++] = pi;
  }

  assert(j == (num_ipoints - int(del_points_.size())));

  ipoints_.resize(j);
}

bool
CQChartsGrahamHull::
pointLineLeft(const QPointF &a, const QPointF &b, const QPointF &c)
{
  return areaSign(a, b, c) > 0;
}

int
CQChartsGrahamHull::
areaSign(const QPointF &a, const QPointF &b, const QPointF &c)
{
  double area2 = (b.x()- a.x())*(c.y() - a.y()) - (c.x()- a.x())*(b.y() - a.y());

  if      (area2 > 0.0) return  1;
  else if (area2 < 0.0) return -1;
  else                  return  0;
}

void
CQChartsGrahamHull::
draw(const CQChartsPlot *, CQChartsPaintDevice *device) const
{
  std::vector<QPointF> hpoints;

  getHull(hpoints);

  int n = hpoints.size();

  QPainterPath path;

  if (n > 0) {
    const QPointF &p = hpoints[0];

    path.moveTo(p);

    for (int i = 1; i < n; ++i) {
      const QPointF &p = hpoints[i];

      path.lineTo(p);
    }

    path.closeSubpath();

    device->fillPath  (path, device->brush());
    device->strokePath(path, device->pen());
  }
}
