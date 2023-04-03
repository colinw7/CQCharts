#ifndef CQChartsPosition_H
#define CQChartsPosition_H

#include <CQChartsGeom.h>
#include <CQChartsUnits.h>
#include <CQChartsTypes.h>
#include <CQUtilMeta.h>

#include <QString>
#include <iostream>

/*!
 * \brief Position class
 * ingroup Charts
 */
class CQChartsPosition :
  public CQChartsEqBase<CQChartsPosition>,
  public CQChartsToStringBase<CQChartsPosition> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsPosition, metaTypeId)

 public:
  using Point = CQChartsGeom::Point;
  using Units = CQChartsUnits::Type;

 public:
  static CQChartsPosition view (const Point &p) { return CQChartsPosition(p, Units::VIEW); }
  static CQChartsPosition plot (const Point &p) { return CQChartsPosition(p, Units::PLOT); }
  static CQChartsPosition pixel(const Point &p) { return CQChartsPosition(p, Units::PIXEL); }

  static CQChartsPosition view(const QString &str) { return CQChartsPosition(str, Units::VIEW); }
  static CQChartsPosition plot(const QString &str) { return CQChartsPosition(str, Units::PLOT); }

 public:
  CQChartsPosition() = default;

  CQChartsPosition(const Units &units, const Point &p) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const Point &p, const Units &units) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const QString &s, const Units &defUnits=Units::PIXEL) {
    setPoint(s, defUnits);
  }

  const Units &units() const { return units_; }
  const Point &p    () const { return p_; }

  bool isValid() const { return units_ != Units::NONE; }

  void setPoint(const Units &units, const Point &p) {
    units_ = units;
    p_     = p;
  }

  bool setPoint(const QString &str, const Units &defUnits=Units::PIXEL) {
    Units units;
    Point p;

    if (! decodeString(str, units, p, defUnits))
      return false;

    units_ = units;
    p_     = p;

    return true;
  }

  bool isSet() const {
    return (units_ != Units::NONE);
  }

  //---

  QString toString() const;

  bool fromString(const QString &s, const Units &defUnits=Units::PIXEL) {
    return setPoint(s, defUnits);
  }

  //---

  friend bool operator==(const CQChartsPosition &lhs, const CQChartsPosition &rhs) {
    if (lhs.units_ != rhs.units_) return false;
    if (lhs.p_     != rhs.p_    ) return false;

    return true;
  }

  //---

 private:
  static bool decodeString(const QString &str, Units &units, Point &point,
                           const Units &defUnits=Units::PIXEL);

 private:
  Units units_ { Units::NONE };
  Point p_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsPosition)

#endif
