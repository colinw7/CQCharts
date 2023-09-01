#include <CQChartsGeom.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE_ID(CQChartsGeom::BBox   , CQChartsGeomBBox   , toString, fromString)
CQUTIL_DEF_META_TYPE_ID(CQChartsGeom::Point  , CQChartsGeomPoint  , toString, fromString)
CQUTIL_DEF_META_TYPE_ID(CQChartsGeom::Point3D, CQChartsGeomPoint3D, toString, fromString)

namespace CQChartsGeom {

bool lineIntersectCircle(const BBox &rect, const Point &p1, const Point &p2, Point &pi)
{
  auto a = pointAngle(p1, p2);

  auto c = std::cos(a);
  auto s = std::sin(a);

  auto pi1 = Point(rect.getXMid() + rect.getWidth ()*c/2.0,
                   rect.getYMid() + rect.getHeight()*s/2.0);
  auto pi2 = Point(rect.getXMid() - rect.getWidth ()*c/2.0,
                   rect.getYMid() - rect.getHeight()*s/2.0);

  auto d1 = p2.distanceTo(pi1);
  auto d2 = p2.distanceTo(pi2);

  if (d1 < d2)
    pi = pi1;
  else
    pi = pi2;

  return true;
}

bool lineIntersectRect(const BBox &rect, const Point &p1, const Point &p2, Point &pi)
{
  Points points;

  points.resize(4);

  points[0] = rect.getLL();
  points[1] = rect.getLR();
  points[2] = rect.getUR();
  points[3] = rect.getUL();

  return lineIntersectPolygon(points, p1, p2, pi);
}

bool lineIntersectPolygon(const Points &points, const Point &p1, const Point &p2, Point &pi)
{
  auto n = points.size();
  if (n < 3) return false;

  auto d = -1.0;

  for (size_t i1 = n - 1, i2 = 0; i2 < n; i1 = i2++) {
    Point  pi1;
    double mu1, mu2;

    if (! CQChartsUtil::intersectLines(points[i1], points[i2], p1, p2, pi1, mu1, mu2))
      continue;

    if (mu1 < 0.0 || mu1 > 1.0)
      continue;

    auto d1 = p2.distanceTo(pi1);

    if (d < 0.0 || d1 < d) {
      pi = pi1;
      d  = d1;
    }
  }

  return (d >= 0);
}

}

//---

namespace CQChartsGeom {

QString
Size::
toString() const
{
  return CQChartsUtil::sizeToString(*this);
}

bool
Size::
fromString(const QString &s)
{
  Size size;

  if (! CQChartsUtil::stringToSize(s, size))
    return false;

  size_ = size.size_;
  set_  = true;

  return true;
}

}

//---

namespace CQChartsGeom {

BBox::
BBox(const Range &range) :
 pmin_(range.xmin(), range.ymin()), pmax_(range.xmax(), range.ymax()), set_(true) {
  update();
}

void
BBox::
equalScale(double targetAspect, bool grow)
{
  auto c = getCenter();

  double w = getWidth ();
  double h = getHeight();

  auto sourceAspect = (h > 0.0 ? w/h : 1.0);

  auto aspectRatio = targetAspect/sourceAspect;

  if (grow) {
    if (aspectRatio > 1.0)
      w *= aspectRatio;
    else
      h /= aspectRatio;
  }
  else {
    if (aspectRatio < 1.0)
      w *= aspectRatio;
    else
      h /= aspectRatio;
  }

  pmin_ = Point(c.x - w/2, c.y - h/2);
  pmax_ = Point(c.x + w/2, c.y + h/2);
}

QString
BBox::
toString() const
{
  return CQChartsUtil::bboxToString(*this);
}

bool
BBox::
fromString(const QString &s)
{
  BBox bbox;

  if (! CQChartsUtil::stringToBBox(s, bbox))
    return false;

  pmin_ = bbox.pmin_;
  pmax_ = bbox.pmax_;
  set_  = true;

  return true;
}

//---

void
Range::
equalScale(double targetAspect)
{
  auto c = center();

  double w = xsize();
  double h = ysize();

  auto sourceAspect = (h > 0.0 ? w/h : 1.0);

  auto aspectRatio = targetAspect/sourceAspect;

  if (aspectRatio > 1.0)
    w *= aspectRatio;
  else
    h /= aspectRatio;

  x1_ = c.x - w/2; x2_ = c.x + w/2;
  y1_ = c.y - h/2; y2_ = c.y + h/2;
}

//---

QString
Point::
toString() const
{
  return CQChartsUtil::pointToString(*this);
}

bool
Point::
fromString(const QString &s)
{
  Point p;

  if (! CQChartsUtil::stringToPoint(s, p))
    return false;

  x = p.x;
  y = p.y;

  return true;
}

//---

QString
Point3D::
toString() const
{
  return CQChartsUtil::point3DToString(*this);
}

bool
Point3D::
fromString(const QString &s)
{
  Point3D p;

  if (! CQChartsUtil::stringToPoint3D(s, p))
    return false;

  x = p.x;
  y = p.y;
  z = p.z;

  return true;
}

}

//---

namespace CQChartsGeom {

Points
calcParetoFront(const Points &points, const Point &origin)
{
  using Points    = std::vector<Point>;
  using PosPoints = std::map<double, Points>;

  PosPoints xvals, yvals;

  for (auto &p : points) {
    xvals[p.x].push_back(p);
    yvals[p.y].push_back(p);
  }

  using IndPoints = std::vector<Points>;
  using IndPos    = std::map<double, uint>;

  IndPoints xinds, yinds;
  IndPos    xpos, ypos;

  for (const auto &px : xvals) {
    xpos[px.first] = xinds.size();

    xinds.push_back(px.second);
  }

  for (const auto &py : yvals) {
    ypos[py.first] = yinds.size();

    yinds.push_back(py.second);
  }

  auto xmin = xpos.begin ()->first;
  auto xmax = xpos.rbegin()->first;
  auto ymin = ypos.begin ()->first;
  auto ymax = ypos.rbegin()->first;

  bool invX = (origin.x > (xmax + xmin)/2.0);
  bool invY = (origin.y > (ymax + ymin)/2.0);

  auto nx = xinds.size();
  auto ny = yinds.size();

  //---

  using PointSet = std::set<Point>;

  PointSet pointSet;

  for (auto &p : points) {
    auto px = xpos.find(p.x); assert(px != xpos.end());
    auto py = ypos.find(p.y); assert(py != ypos.end());

    bool add = false;

    auto xind = (*px).second;
    auto yind = (*py).second;

    if ((! invX && xind == 0) || (invX && xind == nx - 1) ||
        (! invY && yind == 0) || (invY && yind == ny - 1)) {
      add = true;
    }
    else {
      auto d = std::hypot(p.x - origin.x, p.y - origin.y);

      bool xadd = true;

      int xind1 = int(invX ? xind + 1 : xind - 1);

      while (xadd && xind1 >= 0 && xind1 <= int(nx - 1)) {
        for (const auto &p1 : xinds[xind1]) {
          auto d1 = std::hypot(p1.x - origin.x, p1.y - origin.y);
          if (d1 < d) {
            xadd = false;
            break;
          }
        }

        xind1 = (invX ? xind1 + 1 : xind1 - 1);
      }

      bool yadd = true;

      int yind1 = (invY ? yind + 1 : yind - 1);

      while (yadd && yind1 >= 0 && yind1 <= int(ny - 1)) {
        for (const auto &p1 : yinds[yind1]) {
          auto d1 = std::hypot(p1.x - origin.x, p1.y - origin.y);
          if (d1 < d) {
            yadd = false;
            break;
          }
        }

        yind1 = (invY ? yind1 + 1 : yind1 - 1);
      }

      add = (xadd || yadd);
    }

    if (add)
      pointSet.insert(p);
  }

  //---

  Points front;

  for (const auto &p : pointSet)
    front.push_back(p);

  return front;
}

}

//---

namespace CQChartsGeom {

int bboxMetaTypeId;
int pointMetaTypeId;
int point3DMetaTypeId;

void registerMetaTypes()
{
  bboxMetaTypeId    = CQUTIL_REGISTER_META_ID(CQChartsGeomBBox);
  pointMetaTypeId   = CQUTIL_REGISTER_META_ID(CQChartsGeomPoint);
  point3DMetaTypeId = CQUTIL_REGISTER_META_ID(CQChartsGeomPoint3D);

  CQPropertyViewMgrInst->setUserName("CQChartsGeom::BBox"   , "geom_bbox");
  CQPropertyViewMgrInst->setUserName("CQChartsGeom::Point"  , "geom_point");
  CQPropertyViewMgrInst->setUserName("CQChartsGeom::Point3D", "geom_point3d");
}

}
