#ifndef CQChartsArea_H
#define CQChartsArea_H

#include <CQChartsUnits.h>
#include <CQChartsTypes.h>
#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>
#include <CMathUtil.h>
#include <QString>
#include <iostream>
#include <cassert>

/*!
 * \brief Area class
 * \ingroup Charts
 */
class CQChartsArea :
  public CQChartsEqBase<CQChartsArea>,
  public CQChartsToStringBase<CQChartsArea> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsArea, metaTypeId)

 public:
  using Units = CQChartsUnits::Type;

 public:
  static CQChartsArea view   (double value) { return CQChartsArea(value, Units::VIEW); }
  static CQChartsArea plot   (double value) { return CQChartsArea(value, Units::PLOT); }
  static CQChartsArea pixel  (double value) { return CQChartsArea(value, Units::PIXEL); }
  static CQChartsArea percent(double value) { return CQChartsArea(value, Units::PERCENT); }

  static CQChartsArea view(const QString &str) { return CQChartsArea(str, Units::VIEW); }
  static CQChartsArea plot(const QString &str) { return CQChartsArea(str, Units::PLOT); }

 public:
  CQChartsArea() = default;

  CQChartsArea(const Units &units, double value) :
   units_(units), value_(value) {
  }

  CQChartsArea(double value, const Units &units) :
   units_(units), value_(value) {
  }

  explicit CQChartsArea(const QString &s, const Units &units=Units::PLOT) {
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

  friend bool operator==(const CQChartsArea &lhs, const CQChartsArea &rhs) {
    if (lhs.units_ != rhs.units_) return false;
    if (lhs.value_ != rhs.value_) return false;

    return true;
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

CQUTIL_DCL_META_TYPE(CQChartsArea)

#endif
