#include <CQChartsPath.h>
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

  QString str;

  int n = path_->elementCount();
  if (n == 0) return "";

  for (int i = 0; i < n; ++i) {
    const auto &e = path_->elementAt(i);

    if (str.length())
      str += " ";

    if      (e.isMoveTo()) {
      str += QString("M %1 %2").arg(e.x).arg(e.y);
    }
    else if (e.isLineTo()) {
      str += QString("L %1 %2").arg(e.x).arg(e.y);
    }
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (i < n - 1) {
        e1  = path_->elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path_->elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        ++i;

        if (e2t == QPainterPath::CurveToDataElement) {
          ++i;

          str += QString("C %1 %2 %3 %4 %5 %6").
            arg(e.x).arg(e.y).arg(e1.x).arg(e1.y).arg(e2.x).arg(e2.y);
        }
        else {
          str += QString("Q %1 %2 %3 %4").
            arg(e.x).arg(e.y).arg(e1.x).arg(e1.y);
        }
      }
    }
  }

  return str;
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
movePath(const QPainterPath &path, double dx, double dy) const
{
  QPainterPath ppath;

  int n = path.elementCount();

  auto moveElement = [&](const QPainterPath::Element &e) {
    return QPointF(e.x + dx, e.y + dy);
  };

  for (int i = 0; i < n; ++i) {
    const auto &e = path.elementAt(i);

    if      (e.isMoveTo()) {
      ppath.moveTo(moveElement(e));
    }
    else if (e.isLineTo()) {
      ppath.lineTo(moveElement(e));
    }
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (i < n - 1) {
        e1  = path.elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path.elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        ++i;

        if (e2t == QPainterPath::CurveToDataElement) {
          ++i;

          ppath.cubicTo(moveElement(e), moveElement(e1), moveElement(e2));
        }
        else {
          ppath.quadTo(moveElement(e), moveElement(e1));
        }
      }
    }
    else {
      assert(false);
    }
  }

  return ppath;
}

QPainterPath
CQChartsPath::
flipPath(const QPainterPath &path, bool flipX, bool flipY) const
{
  QPainterPath ppath;

  auto bbox = path.boundingRect();

  auto center = bbox.center();

  int n = path.elementCount();

  auto flipElement = [&](const QPainterPath::Element &e) {
    return QPointF((flipX ? 2*center.x() - e.x : e.x), (flipY ? 2*center.y() - e.y : e.y));
  };

  for (int i = 0; i < n; ++i) {
    const auto &e = path.elementAt(i);

    if      (e.isMoveTo()) {
      ppath.moveTo(flipElement(e));
    }
    else if (e.isLineTo()) {
      ppath.lineTo(flipElement(e));
    }
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (i < n - 1) {
        e1  = path.elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path.elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        ++i;

        if (e2t == QPainterPath::CurveToDataElement) {
          ++i;

          ppath.cubicTo(flipElement(e), flipElement(e1), flipElement(e2));
        }
        else {
          ppath.quadTo(flipElement(e), flipElement(e1));
        }
      }
    }
    else {
      assert(false);
    }
  }

  return ppath;
}

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
