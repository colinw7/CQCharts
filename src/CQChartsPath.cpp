#include <CQChartsPath.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyView.h>
#include <CSVGUtil.h>

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

QString
CQChartsPath::
pathToString(const QPainterPath &path)
{
  using Point = CQChartsGeom::Point;

  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    void moveTo(const Point &p) override {
      if (str_.length()) str_ += " ";

      str_ += QString("M %1 %2").arg(p.x).arg(p.y);
    }

    void lineTo(const Point &p) override {
      if (str_.length()) str_ += " ";

      str_ += QString("L %1 %2").arg(p.x).arg(p.y);
    }

    void quadTo(const Point &p1, const Point &p2) override {
      if (str_.length()) str_ += " ";

      str_ += QString("Q %1 %2 %3 %4").arg(p1.x).arg(p1.y).arg(p2.x).arg(p2.y);
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      if (str_.length()) str_ += " ";

      str_ += QString("C %1 %2 %3 %4 %5 %6").
                arg(p1.x).arg(p1.y).arg(p2.x).arg(p2.y).arg(p3.x).arg(p3.y);
    }

    const QString &str() const { return str_; }

   private:
    QString str_;
  };

  PathVisitor visitor;

  CQChartsDrawUtil::visitPath(path, visitor);

  return visitor.str();
}

void
CQChartsPath::
move(double dx, double dy)
{
  *path_ = movePath(*path_, dx, dy);
}

void
CQChartsPath::
flip(bool flipX, bool flipY)
{
  *path_ = flipPath(*path_, flipX, flipY);
}

QPainterPath
CQChartsPath::
movePath(const QPainterPath &path, double dx, double dy)
{
  using Point = CQChartsGeom::Point;

  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(double dx, double dy) :
     dx_(dx), dy_(dy) {
    }

    void moveTo(const Point &p) override {
      path_.moveTo(movePoint(p));
    }

    void lineTo(const Point &p) override {
      path_.lineTo(movePoint(p));
    }

    void quadTo(const Point &p1, const Point &p2) override {
      path_.quadTo(movePoint(p1), movePoint(p2));
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      path_.cubicTo(movePoint(p1), movePoint(p2), movePoint(p3));
    }

    const QPainterPath &path() const { return path_; }

   private:
    QPointF movePoint(const Point &p) const {
      return QPointF(p.x + dx_, p.y + dy_);
    };

   private:
    double       dx_ { 0.0 };
    double       dy_ { 0.0 };
    QPainterPath path_;
  };

  PathVisitor visitor(dx, dy);

  CQChartsDrawUtil::visitPath(path, visitor);

  return visitor.path();
}

QPainterPath
CQChartsPath::
flipPath(const QPainterPath &path, bool flipX, bool flipY)
{
  using BBox  = CQChartsGeom::BBox;
  using Point = CQChartsGeom::Point;

  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(const BBox &bbox, bool flipX, bool flipY) :
     bbox_(bbox), flipX_(flipX), flipY_(flipY) {
      center_ = bbox_.getCenter();
    }

    void moveTo(const Point &p) override {
      path_.moveTo(flipPoint(p));
    }

    void lineTo(const Point &p) override {
      path_.lineTo(flipPoint(p));
    }

    void quadTo(const Point &p1, const Point &p2) override {
      path_.quadTo(flipPoint(p1), flipPoint(p2));
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      path_.cubicTo(flipPoint(p1), flipPoint(p2), flipPoint(p3));
    }

    const QPainterPath &path() const { return path_; }

   private:
    QPointF flipPoint(const Point &p) const {
      return QPointF((flipX_ ? 2*center_.x - p.x : p.x),
                     (flipY_ ? 2*center_.y - p.y : p.y));
    };

   private:
    BBox         bbox_;
    Point        center_;
    bool         flipX_ { false };
    bool         flipY_ { false };
    QPainterPath path_;
  };

  auto rect = path.boundingRect();

  PathVisitor visitor(BBox(rect), flipX, flipY);

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
  delete path_;

  path_ = new QPainterPath;

  //---

  class PathVisitor : public CSVGUtil::PathVisitor {
   public:
    PathVisitor() { }

    const QPainterPath &path() const { return path_; }

    void moveTo(double x, double y) override {
      path_.moveTo(x, y);
    }

    void lineTo(double x, double y) override {
      path_.lineTo(x, y);
    }

    void arcTo(double rx, double ry, double xa, int fa, int fs, double x2, double y2) override {
      bool unit_circle = false;

      //double cx, cy, rx1, ry1, theta, delta;

      //CSVGUtil::convertArcCoords(lastX(), lastY(), x2, y2, xa, rx, ry, fa, fs, unit_circle,
      //                           &cx, &cy, &rx1, &ry1, &theta, &delta);

      //path_.arcTo(QRectF(cx - rx1, cy - ry1, 2*rx1, 2*ry1), -theta, -delta);

      //double a1 = CMathUtil::Deg2Rad(theta);
      //double a2 = CMathUtil::Deg2Rad(theta + delta);

      CSVGUtil::BezierList beziers;

      CSVGUtil::arcToBeziers(lastX(), lastY(), x2, y2, xa, rx, ry, fa, fs, unit_circle, beziers);

      auto qpoint = [](const CPoint2D &p) { return QPointF(p.x, p.y); };

      if (! beziers.empty())
        path_.lineTo(qpoint(beziers[0].getFirstPoint()));

      for (const auto &bezier : beziers)
        path_.cubicTo(qpoint(bezier.getControlPoint1()),
                      qpoint(bezier.getControlPoint2()),
                      qpoint(bezier.getLastPoint    ()));
    }

    void bezier2To(double x1, double y1, double x2, double y2) override {
      path_.quadTo(QPointF(x1, y1), QPointF(x2, y2));
    }

    void bezier3To(double x1, double y1, double x2, double y2, double x3, double y3) override {
      path_.cubicTo(QPointF(x1, y1), QPointF(x2, y2), QPointF(x3, y3));
    }

    void closePath(bool /*relative*/) override {
      path_.closeSubpath();
    }

   private:
    QPainterPath path_;
  };

  PathVisitor visitor;

  if (! CSVGUtil::visitPath(str.toStdString(), visitor)) {
    //std::cerr << "Invalid path: " << str.toStdString() << "\n";
    return false;
  }

  *path_ = visitor.path();

  return true;
}
