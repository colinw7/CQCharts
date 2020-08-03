#ifndef CQChartsRect_H
#define CQChartsRect_H

#include <CQChartsPosition.h>
#include <CQChartsLength.h>
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
  using BBox     = CQChartsGeom::BBox;
  using Units    = CQChartsUnits;
  using Position = CQChartsPosition;
  using Length   = CQChartsLength;

 public:
  CQChartsRect(const Units &units, const BBox &bbox) :
   units_(units), bbox_(bbox) {
  }

  CQChartsRect(const BBox &bbox=BBox(), const Units &units=Units::PLOT) :
   units_(units), bbox_(bbox) {
  }

  CQChartsRect(const QString &s, const Units &units=Units::PLOT) {
    setValue(s, units);
  }

  //---

  bool isValid() const { return isSet() && bbox_.isValid(); }

  //---

  const Units &units() const { return units_; }
  void setUnits(const Units &units) { units_ = units; }

  bool hasUnits() const { return units_ != Units::NONE; }

  //---

  const BBox &bbox() const { return bbox_; }

  Position center () const;
  Length   xRadius() const;
  Length   yRadius() const;

  //---

  void setValue(const Units &units, const BBox &bbox) {
    units_ = units;
    bbox_  = bbox;
  }

  bool setValue(const QString &str, const Units &defUnits=Units::PLOT);

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
  static bool decodeString(const QString &str, Units &units, BBox &bbox,
                           const Units &defUnits=Units::PLOT);

 private:
  Units units_ { Units::PLOT };
  BBox  bbox_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsRect)

#endif
