#ifndef CQChartsPath_H
#define CQChartsPath_H

#include <CQChartsGeom.h>
#include <QPainterPath>
#include <iostream>

class CQChartsPlot;

/*!
 * \brief Geometric Path data
 * \ingroup Charts
 */
class CQChartsPath :
  public CQChartsEqBase<CQChartsPath>,
  public CQChartsToStringBase<CQChartsPath> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using Point  = CQChartsGeom::Point;
  using Points = std::vector<Point>;

 public:
  CQChartsPath() = default;

  explicit CQChartsPath(const QString &str) {
    setValue(str);
  }

  CQChartsPath(const CQChartsPath &rhs) {
    if (! rhs.path_) return;

    path_ = new QPainterPath(*rhs.path_);
  }

  CQChartsPath &operator=(const CQChartsPath &rhs) {
    delete path_;

    if (rhs.path_)
      path_ = new QPainterPath(*rhs.path_);
    else
      path_ = nullptr;

    return *this;
  }

 ~CQChartsPath() {
    delete path_;
  }

  //---

  QPainterPath &path() {
    return *pathPtr();
  }

  const QPainterPath &path() const {
    return *(this->pathPtr());
  }

  //---

  bool setValue(const QString &str);

  //---

  QString toString() const;

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  bool isValid() const { return ! path().isEmpty(); }

  //---

  QPolygonF qpoly() const { return path().toFillPolygon(); }

  //---

  int numPoints() const { return path().elementCount(); }

  Point pointAt(int i) const { auto e = path().elementAt(i); return Point(e.x, e.y); }

  void setPointAt(int i, const Point &p) { path().setElementPositionAt(i, p.x, p.y); }

  //---

  void move(double dx, double dy);
  void flip(bool flipX, bool flipY);

  //---

  friend bool operator==(const CQChartsPath &lhs, const CQChartsPath &rhs) {
    if (  lhs.path_ ==   rhs.path_) return true;
    if (! lhs.path_ || ! rhs.path_) return false;

    if (*lhs.path_ == *rhs.path_) return true;

    return false;
  }

  //---

  static QString pathToString(const QPainterPath &path);

  static QPainterPath movePath(const QPainterPath &path, double dx, double dy);

  static QPainterPath flipPath(const QPainterPath &path, bool flipX, bool flipY);

  static QPainterPath reversePath(const QPainterPath &path);

  static QPainterPath combinePaths(const QPainterPath &path1, const QPainterPath &path2);

  //---

  static Points pathPoints(const QPainterPath &path);

 private:
  const QPainterPath *pathPtr() const { return const_cast<CQChartsPath *>(this)->pathPtr(); }

  QPainterPath *pathPtr() {
    static QPainterPath s_path;

    if (! path_)
      return &s_path;

    return path_;
  }

 private:
  QPainterPath* path_ { nullptr };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPath)

#endif
