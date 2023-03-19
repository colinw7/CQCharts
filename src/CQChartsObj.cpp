#include <CQChartsObj.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CMathGeom2D.h>

CQChartsObj::
CQChartsObj(CQCharts *charts, const BBox &rect) :
 QObject(nullptr), charts_(charts), rect_(rect)
{
  assert(charts_);
}

const QString &
CQChartsObj::
id() const
{
  if (! id_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! id_) {
      const_cast<CQChartsObj*>(this)->id_ = calcId();

      CQCHARTS_ASSERT((*id_).length(), "Object has empty id");
    }
  }

  return *id_;
}

void
CQChartsObj::
setId(const QString &s)
{
  id_ = s;

  dataInvalidate(DataType::ID);

  Q_EMIT idChanged();
}

void
CQChartsObj::
resetId()
{
  id_ = OptString();
}

const QString &
CQChartsObj::
tipId() const
{
  if (! tipId_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! tipId_) {
      const_cast<CQChartsObj*>(this)->tipId_ = calcTipId();

      if (! (*tipId_).length())
        const_cast<CQChartsObj*>(this)->tipId_ = id();

      assert((*tipId_).length());
    }
  }

  return *tipId_;
}

void
CQChartsObj::
setTipId(const QString &s)
{
  tipId_ = s;

  dataInvalidate(DataType::TIP);
}

bool
CQChartsObj::
intersectShape(const Point &p1, const Point &p2, Point &pi) const
{
  std::vector<Point> ipoints;

  if      (objShapeType() == ObjShapeType::RECT) {
    double x1 = rect().getXMin(); double y1 = rect().getYMin();
    double x2 = rect().getXMax(); double y2 = rect().getYMax();

    auto intersectLines = [&](double x11, double y11, double x21, double y21,
                              double &xi, double &yi) {
      double mu1, mu2;

      if (! CQChartsUtil::intersectLines(p1.x, p1.y, p2.x, p2.y, x11, y11, x21, y21,
                                         xi, yi, mu1, mu2))
        return false;

      if (mu1 < 0.0 || mu1 > 1.0)
        return false;

      return true;
    };

    double xi, yi;

    if (intersectLines(x1, y1, x2, y1, xi, yi)) ipoints.push_back(Point(xi, yi));
    if (intersectLines(x2, y1, x2, y2, xi, yi)) ipoints.push_back(Point(xi, yi));
    if (intersectLines(x2, y2, x1, y2, xi, yi)) ipoints.push_back(Point(xi, yi));
    if (intersectLines(x1, y2, x1, y1, xi, yi)) ipoints.push_back(Point(xi, yi));
  }
  else if (objShapeType() == ObjShapeType::CIRCLE) {
    Point pi;

    if (! CQChartsGeom::lineIntersectCircle(rect(), p1, p2, pi))
      return false;

    ipoints.push_back(pi);
  }
  else
    return false;

  if (ipoints.empty())
    return false;

  Point  minP;
  double minD = -1;

  for (const auto &p : ipoints) {
    auto d = CQChartsUtil::PointPointDistance(p, p1);

    if (minD < 0 || d < minD) {
      minP = p;
      minD = d;
    }
  }

  pi = minP;

  return true;
}
