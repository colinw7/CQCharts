#ifndef CQChartsPosition_H
#define CQChartsPosition_H

#include <CQChartsGeom.h>
#include <CQChartsTypes.h>
#include <QString>
#include <iostream>

/*!
 * \brief Position class
 * ingroup Charts
 */
class CQChartsPosition {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using Point = CQChartsGeom::Point;
  using Units = CQChartsUnits;

 public:
  CQChartsPosition(const Units &units, const Point &p) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const Point &p=Point(0, 0), const Units &units=Units::PLOT) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const QString &s, const Units &defUnits=Units::PIXEL) {
    setPoint(s, defUnits);
  }

  CQChartsPosition(const CQChartsPosition &rhs) :
    units_(rhs.units_), p_(rhs.p_) {
  }

  CQChartsPosition &operator=(const CQChartsPosition &rhs) {
    units_ = rhs.units_;
    p_     = rhs.p_;

    return *this;
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

  friend bool operator!=(const CQChartsPosition &lhs, const CQChartsPosition &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsPosition &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  static bool decodeString(const QString &str, Units &units, Point &point,
                           const Units &defUnits=Units::PIXEL);

 private:
  Units units_ { Units::PIXEL };
  Point p_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPosition)

#endif
