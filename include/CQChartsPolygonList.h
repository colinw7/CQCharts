#ifndef CQChartsPolygonList_H
#define CQChartsPolygonList_H

#include <CQChartsUtil.h>
#include <CQChartsGeom.h>

/*!
 * \brief polygon list
 * \ingroup Charts
 */
class CQChartsPolygonList {
 public:
  using Polygons = CQChartsGeom::Polygons;

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsPolygonList() = default;

  explicit CQChartsPolygonList(const QString &str) {
   setValue(str);
  }

  //---

  const Polygons &polygons() const { return polygons_; }

  //---

  bool setValue(const QString &str) {
    return CQChartsUtil::stringToPolygons(str, polygons_);
  }

  //---

  QString toString() const {
    return CQChartsUtil::polygonListToString(polygons_);
  }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  friend bool operator==(const CQChartsPolygonList &lhs, const CQChartsPolygonList &rhs) {
    if (lhs.polygons_.size() != rhs.polygons_.size()) return false;

    int np = lhs.polygons_.size();

    for (int i = 0; i < np; ++i) {
      if (lhs.polygons_[i] != rhs.polygons_[i])
        return false;
    }

    return true;
  }

 private:
  Polygons polygons_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPolygonList)

#endif
