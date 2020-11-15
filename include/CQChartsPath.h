#ifndef CQChartsPath_H
#define CQChartsPath_H

#include <QPainterPath>
#include <iostream>

class CQChartsPlot;

/*!
 * \brief Geometric Path data
 * \ingroup Charts
 */
class CQChartsPath {
 public:
  static void registerMetaType();

  static int metaTypeId;

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

  void move(double dx, double dy);
  void flip(bool flipX, bool flipY);

  //---

  friend bool operator==(const CQChartsPath &lhs, const CQChartsPath &rhs) {
    if (  lhs.path_ ==   rhs.path_) return true;
    if (! lhs.path_ || ! rhs.path_) return false;

    if (*lhs.path_ == *rhs.path_) return true;

    return false;
  }

  friend bool operator!=(const CQChartsPath &lhs, const CQChartsPath &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsPath &c) {
    c.print(os);

    return os;
  }

  //---

 private:
  const QPainterPath *pathPtr() const { return const_cast<CQChartsPath *>(this)->pathPtr(); }

  QPainterPath *pathPtr() {
    static QPainterPath s_path;

    if (! path_)
      return &s_path;

    return path_;
  }

  QPainterPath movePath(const QPainterPath &path, double dx, double dy) const;
  QPainterPath flipPath(const QPainterPath &path, bool flipX, bool flipY) const;

 private:
  QPainterPath* path_ { nullptr };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPath)

#endif
