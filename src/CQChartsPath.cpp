#include <CQChartsPath.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsSVGUtil.h>

#include <CQPropertyView.h>
#include <CMathGeom2D.h>

//---

CQUTIL_DEF_META_TYPE(CQChartsPath, toString, fromString)

int CQChartsPath::metaTypeId;

void
CQChartsPath::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPath);

  CQPropertyViewMgrInst->setUserName("CQChartsPath", "path");
}

QString
CQChartsPath::
toString() const
{
  if (! path_)
    return "";

  return pathToString(*path_);
}

bool
CQChartsPath::
fromString(const QString &s)
{
  return setValue(s);
}

QString
CQChartsPath::
pathToString(const QPainterPath &path)
{
  return CQChartsSVGUtil::pathToString(path);
}

CQChartsGeom::BBox
CQChartsPath::
bbox() const
{
  if (! path_) return BBox();

  return BBox(path_->boundingRect());
}

//---

bool
CQChartsPath::
isControlPoint(int i) const
{
  assert(path_);

  int n = path_->elementCount();
  if (i < 0 || i >= n) return false;

  // contol points are curveTo or first of two curveToDatas
  auto et = path_->elementAt(i);

  if (et.type == QPainterPath::CurveToElement)
    return true;

  if (et.type == QPainterPath::CurveToDataElement) {
    if (i < n - 1) {
      auto et1 = path_->elementAt(i + 1);

      if (et1.type == QPainterPath::CurveToDataElement)
        return true;
    }
  }

  return false;
}

//---

void
CQChartsPath::
move(double dx, double dy)
{
  assert(path_);

  *path_ = movePath(*path_, dx, dy);
}

void
CQChartsPath::
scale(double sx, double sy)
{
  assert(path_);

  *path_ = scalePath(*path_, sx, sy);
}

void
CQChartsPath::
flip(bool flipX, bool flipY)
{
  assert(path_);

  *path_ = flipPath(*path_, flipX, flipY);
}

void
CQChartsPath::
moveScale(double dx, double dy, double sx, double sy)
{
  assert(path_);

  *path_ = moveScalePath(*path_, dx, dy, sx, sy);
}

//---

QPainterPath
CQChartsPath::
movePath(const QPainterPath &path, double dx, double dy)
{
  return moveScalePath(path, dx, dy, 1.0, 1.0);
}

QPainterPath
CQChartsPath::
scalePath(const QPainterPath &path, double sx, double sy)
{
  return moveScalePath(path, 0.0, 0.0, sx, sy);
}

QPainterPath
CQChartsPath::
flipPath(const QPainterPath &path, bool flipX, bool flipY)
{
  return moveScalePath(path, 0.0, 0.0, (flipX ? -1.0 : 1.0), (flipY ? -1.0 : 1.0));
}

QPainterPath
CQChartsPath::
moveScalePath(const QPainterPath &path, double dx, double dy, double sx, double sy)
{
  BBox bbox(path.boundingRect());

  if (! bbox.isValid())
    return path;

  return moveScalePath(path, bbox, dx, dy, sx, sy);
}

QPainterPath
CQChartsPath::
moveScalePath(const QPainterPath &path, const BBox &bbox,
              double dx, double dy, double sx, double sy)
{
  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(const BBox &bbox, double dx, double dy, double sx, double sy) :
     bbox_(bbox), dx_(dx), dy_(dy), sx_(sx), sy_(sy) {
      center_ = bbox_.getCenter();
    }

    void moveTo(const Point &p) override {
      path_.moveTo(scalePoint(p));
    }

    void lineTo(const Point &p) override {
      path_.lineTo(scalePoint(p));
    }

    void quadTo(const Point &p1, const Point &p2) override {
      path_.quadTo(scalePoint(p1), scalePoint(p2));
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      path_.cubicTo(scalePoint(p1), scalePoint(p2), scalePoint(p3));
    }

    const QPainterPath &path() const { return path_; }

   private:
    QPointF scalePoint(const Point &p) const {
      return QPointF(sx_*(p.x - center_.x) + center_.x + dx_,
                     sy_*(p.y - center_.y) + center_.y + dy_);
    };

   private:
    BBox         bbox_;
    Point        center_;
    double       dx_ { 0.0 };
    double       dy_ { 0.0 };
    double       sx_ { 1.0 };
    double       sy_ { 1.0 };
    QPainterPath path_;
  };

  PathVisitor visitor(bbox, dx, dy, sx, sy);

  CQChartsDrawUtil::visitPath(path, visitor);

  return visitor.path();
}

QPainterPath
CQChartsPath::
reversePath(const QPainterPath &path)
{
  return path.toReversed();
}

QPainterPath
CQChartsPath::
combinePaths(const QPainterPath &path1, const QPainterPath &path2)
{
  // return path1.addPath(path2);

  using Point = CQChartsGeom::Point;

  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(const QPainterPath &path) :
     path_(path) {
    }

    void moveTo(const Point &p) override {
      path_.lineTo(p.x, p.y);
    }

    void lineTo(const Point &p) override {
      path_.lineTo(p.x, p.y);
    }

    void quadTo(const Point &p1, const Point &p2) override {
      path_.quadTo(p1.x, p1.y, p2.x, p2.y);
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      path_.cubicTo(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
    }

    const QPainterPath &path() const { return path_; }

   private:
    QPainterPath path_;
  };

  PathVisitor visitor(path1);

  CQChartsDrawUtil::visitPath(path2, visitor);

  return visitor.path();
}

CQChartsPath::Points
CQChartsPath::
pathPoints(const QPainterPath &path, double tol)
{
  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(double tol) : tol_(tol) { }

    void moveTo(const Point &p) override {
      assert(i == 0);

      points_.push_back(p);
    }

    void lineTo(const Point &p) override {
      assert(i > 0);

      points_.push_back(p);
    }

    void quadTo(const Point &cp, const Point &p) override {
      auto ps = points_.back();

      C2Bezier2D bezier(ps.x, ps.y, cp.x, cp.y, p.x, p.y);

      std::vector<CPoint2D> points;

      CMathGeom2D::BezierToLines(bezier, points, tol_);

      for (size_t i = 1; i < points.size(); ++i) {
        const auto &bp = points[i];

        points_.push_back(Point(bp.x, bp.y));
      }
    }

    void curveTo(const Point &cp1, const Point &cp2, const Point &p) override {
      auto ps = points_.back();

      C3Bezier2D bezier(ps.x, ps.y, cp1.x, cp1.y, cp2.x, cp2.y, p.x, p.y);

      std::vector<CPoint2D> points;

      CMathGeom2D::BezierToLines(bezier, points, tol_);

      for (size_t i = 1; i < points.size(); ++i) {
        const auto &bp = points[i];

        points_.push_back(Point(bp.x, bp.y));
      }
    }

    const Points &points() const { return points_; }

   private:
    double tol_ { 1E-3 };
    Points points_;
  };

  PathVisitor visitor(tol);

  CQChartsDrawUtil::visitPath(path, visitor);

  return visitor.points();
}

//---

CQChartsPath &
CQChartsPath::
moveTo(const Point &p)
{
  if (! path_)
    path_ = new QPainterPath;

  path_->moveTo(p.x, p.y);

  return *this;
}

CQChartsPath &
CQChartsPath::
lineTo(const Point &p)
{
  assert(path_);

  path_->lineTo(p.x, p.y);

  return *this;
}

CQChartsGeom::Point
CQChartsPath::
pointAtPercent(double d) const
{
  if (! path_)
    return Point();

  return Point(path_->pointAtPercent(d));
}

//---

bool
CQChartsPath::
setValue(const QString &str)
{
  QPainterPath path;

  if (! CQChartsSVGUtil::stringToPath(str, path))
    return false;

  //---

  delete path_;

  path_ = new QPainterPath(path);

  return true;
}

//---

bool
CQChartsPath::
fromSVGFile(const QString &filename)
{
  CQChartsSVGUtil::Paths  paths;
  CQChartsSVGUtil::Styles styles;
  BBox                    bbox;

  if (! CQChartsSVGUtil::svgFileToPaths(filename, paths, styles, bbox))
    return false;

  if (paths.empty())
    return false;

  *this = paths[0];

  return true;
}
