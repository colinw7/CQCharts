#ifndef CQChartsLength_H
#define CQChartsLength_H

#include <CQChartsTypes.h>
#include <CQChartsUnits.h>
#include <CQChartsTmpl.h>
#include <CMathUtil.h>
#include <QString>
#include <iostream>
#include <cassert>

/*!
 * \brief Length class
 * \ingroup Charts
 */
class CQChartsLength :
  public CQChartsComparatorBase<CQChartsLength>,
  public CQChartsToStringBase<CQChartsLength> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using Length = CQChartsLength;
  using Units  = CQChartsUnits::Type;

 public:
  static Length view   (double value) { return Length(value, Units::VIEW); }
  static Length plot   (double value) { return Length(value, Units::PLOT); }
  static Length pixel  (double value) { return Length(value, Units::PIXEL); }
  static Length percent(double value) { return Length(value, Units::PERCENT); }

  static Length view(const QString &str) { return Length(str, Units::VIEW); }
  static Length plot(const QString &str) { return Length(str, Units::PLOT); }

 public:
  CQChartsLength() = default;

  CQChartsLength(const Units &units, double value) :
   units_(units), value_(value) {
  }

  CQChartsLength(double value, const Units &units) :
   units_(units), value_(value) {
  }

  explicit CQChartsLength(const QString &s, const Units &units=Units::PLOT) {
    setValue(s, units);
  }

  //---

  bool isValid() const { return (units_ != Units::NONE && ! CMathUtil::isNaN(value_)); }

  bool isSet() const { return (units_ != Units::NONE && value_ != 0.0); }

  //---

  const Units &units() const { return units_; }

#if 0
  double unitsValue(const Units &units) const {
    assert(isValid() && units_ == units); return value_;
  }
#endif

  double value() const { assert(isValid()); return value_; }

  void setValue(const Units &units, double value) {
    units_ = units;
    value_ = value;
  }

  bool setValue(const QString &str, const Units &defUnits=Units::PLOT);

  //---

  QString toString() const;

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const Length &lhs, const Length &rhs) {
    if (lhs.units_ > rhs.units_) return  1;
    if (lhs.units_ < rhs.units_) return -1;
    if (lhs.value_ > rhs.value_) return  1;
    if (lhs.value_ < rhs.value_) return -1;

    return 0;
  }

  //---

 private:
  static bool decodeString(const QString &str, Units &units, double &value,
                           const Units &defUnits=Units::PIXEL);

 private:
  Units  units_ { Units::NONE };
  double value_ { 0.0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLength)

#endif
