#ifndef CQChartsRect_H
#define CQChartsRect_H

#include <CQChartsGeom.h>
#include <CQChartsTypes.h>
#include <QString>
#include <iostream>

//! Rectangle class
class CQChartsRect {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsRect(const CQChartsUnits &units, const CQChartsGeom::BBox &bbox) :
   units_(units), bbox_(bbox) {
  }

  CQChartsRect(const CQChartsGeom::BBox &bbox=CQChartsGeom::BBox(),
               const CQChartsUnits &units=CQChartsUnits::PLOT) :
   units_(units), bbox_(bbox) {
  }

  CQChartsRect(const QString &s, const CQChartsUnits &units=CQChartsUnits::PLOT) {
    setValue(s, units);
  }

  //---

  bool isValid() const { return isSet() && bbox_.isValid(); }

  //---

  const CQChartsUnits &units() const { return units_; }
  void setUnits(const CQChartsUnits &units) { units_ = units; }

  bool hasUnits() const { return units_ != CQChartsUnits::NONE; }

  //---

  const CQChartsGeom::BBox &bbox() const { return bbox_; }

  //---

  void setValue(const CQChartsUnits &units, const CQChartsGeom::BBox &bbox) {
    units_ = units;
    bbox_  = bbox;
  }

  bool setValue(const QString &str, const CQChartsUnits &defUnits=CQChartsUnits::PLOT);

  bool isSet() const { return bbox_.isSet(); }

  //---

  QString toString() const;

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  friend bool operator==(const CQChartsRect &lhs, const CQChartsRect &rhs) {
    if (lhs.units_ != rhs.units_) return false;
    if (lhs.bbox_  != rhs.bbox_ ) return false;

    return true;
  }

  friend bool operator!=(const CQChartsRect &lhs, const CQChartsRect &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsRect &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  static bool decodeString(const QString &str, CQChartsUnits &units, CQChartsGeom::BBox &bbox,
                           const CQChartsUnits &defUnits=CQChartsUnits::PLOT);

 private:
  CQChartsUnits      units_ { CQChartsUnits::PLOT };
  CQChartsGeom::BBox bbox_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsRect)

#endif
