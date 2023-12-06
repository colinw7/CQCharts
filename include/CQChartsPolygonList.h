#ifndef CQChartsPolygonList_H
#define CQChartsPolygonList_H

#include <CQChartsGeom.h>
#include <CQUtilMeta.h>

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

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsPolygonList, metaTypeId)

 public:
  CQChartsPolygonList() = default;

  explicit CQChartsPolygonList(const QString &str) {
   setValue(str);
  }

  //---

  const Polygons &polygons() const { return polygons_; }

  //---

  bool setValue(const QString &str);

  //---

  QString toString() const;
  bool fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsPolygonList &lhs, const CQChartsPolygonList &rhs) {
    if (lhs.polygons_.size() != rhs.polygons_.size()) return false;

    auto np = lhs.polygons_.size();

    for (size_t i = 0; i < np; ++i) {
      if (lhs.polygons_[i] != rhs.polygons_[i])
        return false;
    }

    return true;
  }

 private:
  Polygons polygons_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsPolygonList)

#endif
