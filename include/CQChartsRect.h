#ifndef CQChartsRect_H
#define CQChartsRect_H

#include <CQChartsPosition.h>
#include <CQChartsGeom.h>
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

  CQChartsRect(const CQChartsRect &rhs) :
    units_(rhs.units_), bbox_(rhs.bbox_) {
  }

  CQChartsRect &operator=(const CQChartsRect &rhs) {
    units_ = rhs.units_;
    bbox_  = rhs.bbox_;

    return *this;
  }

  //---

  bool isValid() const { return isSet() && bbox_.isValid(); }

  //---

  const CQChartsUnits &units() const { return units_; }
  void setUnits(const CQChartsUnits &units) { units_ = units; }

  bool hasUnits() const { return units_ != CQChartsUnits::NONE; }

  //---

  const CQChartsGeom::BBox &bbox() const { return bbox_; }

  void setValue(const CQChartsUnits &units, const CQChartsGeom::BBox &bbox) {
    units_ = units;
    bbox_  = bbox;
  }

  bool setValue(const QString &str, const CQChartsUnits &defUnits=CQChartsUnits::PLOT) {
    CQChartsUnits units;
    CQChartsGeom::BBox        bbox;

    if (! decodeString(str, units, bbox, defUnits))
      return false;

    units_ = units;
    bbox_  = bbox;

    return true;
  }

  bool isSet() const { return bbox_.isSet(); }

  //---

  QString toString() const {
    QString ustr = CQChartsUtil::unitsString(units_);

    return QString("%1 %2 %3 %4 %5").
             arg(bbox_.getXMin()).arg(bbox_.getYMin()).
             arg(bbox_.getXMax()).arg(bbox_.getYMax()).arg(ustr);
  }

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
