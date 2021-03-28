#include <CQChartsPath.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsSVGUtil.h>

#include <CQPropertyView.h>

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

  return CQChartsSVGUtil::pathToString(*path_);
}

CQChartsGeom::BBox
CQChartsPath::
bbox() const
{
  return BBox(path_->boundingRect());
}

//---

void
CQChartsPath::
move(double dx, double dy)
{
  *path_ = movePath(*path_, dx, dy);
}

void
CQChartsPath::
scale(double sx, double sy)
{
  *path_ = scalePath(*path_, sx, sy);
}

void
CQChartsPath::
flip(bool flipX, bool flipY)
{
  *path_ = flipPath(*path_, flipX, flipY);
}

void
CQChartsPath::
moveScale(double dx, double dy, double sx, double sy)
{
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
pathPoints(const QPainterPath &path)
{
  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    void moveTo(const Point &p) override {
      assert(i == 0);

      points_.push_back(p);
    }

    void lineTo(const Point &p) override {
      assert(i > 0);

      auto pp1 = points_.back();
      auto pp2 = p;

      points_.push_back((pp1 + pp2)/2.0);
      points_.push_back(pp2);
    }

    void quadTo(const Point &, const Point &) override { assert(false); }

    void curveTo(const Point &, const Point &, const Point &) override { assert(false); }

    const Points &points() const { return points_; }

   private:
    Points points_;
  };

  PathVisitor visitor;

  CQChartsDrawUtil::visitPath(path, visitor);

  return visitor.points();
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
fromSVGFile(const QString &fileName)
{
  CQChartsSVGUtil::Paths  paths;
  CQChartsSVGUtil::Styles styles;
  BBox                    bbox;

  if (! CQChartsSVGUtil::svgFileToPaths(fileName, paths, styles, bbox))
    return false;

  if (paths.empty())
    return false;

  *this = paths[0];

  return true;
}
