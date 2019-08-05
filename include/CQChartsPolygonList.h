#ifndef CQChartsPolygonList_H
#define CQChartsPolygonList_H

#include <CQChartsUtil.h>
#include <QPolygonF>

/*!
 * \brief polygon list
 * \ingroup Charts
 */
class CQChartsPolygonList {
 public:
  using Polygons = std::vector<QPolygonF>;

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsPolygonList(const QString &s=QString()) {
   setValue(s);
  }

  CQChartsPolygonList(const CQChartsPolygonList &rhs) :
    polygons_(rhs.polygons_) {
  }

  CQChartsPolygonList &operator=(const CQChartsPolygonList &rhs) {
    polygons_ = rhs.polygons_;

    return *this;
  }

  const Polygons &polygons() const { return polygons_; }

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

 private:
  Polygons polygons_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPolygonList)

#endif
