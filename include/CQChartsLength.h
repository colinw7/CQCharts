#ifndef CQChartsLength_H
#define CQChartsLength_H

#include <CQChartsTypes.h>
#include <CMathUtil.h>
#include <QString>
#include <iostream>
#include <cassert>

/*!
 * \brief Length class
 * \ingroup Charts
 */
class CQChartsLength {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using Units = CQChartsUnits;

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

  bool isSet() const { return (units_ != Units::PIXEL || value_ != 0.0); }

  //---

  const Units &units() const { return units_; }

  double unitsValue(const Units &units) const {
    assert(isValid() && units_ == units); return value_;
  }

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

  friend bool operator==(const CQChartsLength &lhs, const CQChartsLength &rhs) {
    if (lhs.units_ != rhs.units_) return false;
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsLength &lhs, const CQChartsLength &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsLength &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  static bool decodeString(const QString &str, Units &units, double &value,
                           const Units &defUnits=Units::PIXEL);

 private:
  Units  units_ { Units::PIXEL };
  double value_ { 0.0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLength)

#endif
