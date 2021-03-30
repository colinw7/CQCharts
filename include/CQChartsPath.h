#ifndef CQChartsPath_H
#define CQChartsPath_H

#include <CQChartsGeom.h>
#include <QPainterPath>
#include <iostream>

class CQChartsPlot;

class CXMLTag;

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
  using BBox   = CQChartsGeom::BBox;

 public:
  CQChartsPath() = default;

  explicit CQChartsPath(const QString &str) {
    setValue(str);
  }

  CQChartsPath(const CQChartsPath &rhs) {
    if (! rhs.path_) return;

    path_ = new QPainterPath(*rhs.path_);
  }

  CQChartsPath(CQChartsPath &&rhs) {
    std::swap(path_, rhs.path_);
  }

  CQChartsPath &operator=(const CQChartsPath &rhs) {
    if (this != &rhs) {
      delete path_;

      if (rhs.path_)
        path_ = new QPainterPath(*rhs.path_);
      else
        path_ = nullptr;
    }

    return *this;
  }

  CQChartsPath &operator=(CQChartsPath &&rhs) {
    if (this != &rhs) {
      delete path_;

      path_ = nullptr;

      std::swap(path_, rhs.path_);
    }

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

  void setPath(const QPainterPath &path) {
    delete path_;

    path_ = new QPainterPath(path);
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

  BBox bbox() const;

  QPolygonF qpoly() const { return path().toFillPolygon(); }

  //---

  int numPoints() const { return path().elementCount(); }

  Point pointAt(int i) const { auto e = path().elementAt(i); return Point(e.x, e.y); }

  void setPointAt(int i, const Point &p) { path().setElementPositionAt(i, p.x, p.y); }

  //---

  void move (double dx, double dy);
  void scale(double sx, double sy);
  void flip (bool flipX, bool flipY);

  void moveScale(double dx, double dy, double sx, double sy);

  //---

  friend bool operator==(const CQChartsPath &lhs, const CQChartsPath &rhs) {
    if (  lhs.path_ ==   rhs.path_) return true;
    if (! lhs.path_ || ! rhs.path_) return false;

    if (*lhs.path_ == *rhs.path_) return true;

    return false;
  }

  //---

  static QPainterPath movePath (const QPainterPath &path, double dx, double dy);
  static QPainterPath scalePath(const QPainterPath &path, double sx, double sy);
  static QPainterPath flipPath (const QPainterPath &path, bool flipX, bool flipY);

  static QPainterPath moveScalePath(const QPainterPath &path,
                                    double dx, double dy, double sx, double sy);
  static QPainterPath moveScalePath(const QPainterPath &path, const BBox &bbox,
                                    double dx, double dy, double sx, double sy);

  static QPainterPath reversePath(const QPainterPath &path);

  static QPainterPath combinePaths(const QPainterPath &path1, const QPainterPath &path2);

  //---

  static Points pathPoints(const QPainterPath &path);

  //---

  bool fromSVGFile(const QString &fileName);

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
