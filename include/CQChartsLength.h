#ifndef CQChartsLength_H
#define CQChartsLength_H

#include <CQChartsTypes.h>
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
  CQChartsLength(const CQChartsUnits &units, double value) :
   units_(units), value_(value) {
  }

  CQChartsLength(double value=0.0, const CQChartsUnits &units=CQChartsUnits::PLOT) :
   units_(units), value_(value) {
  }

  explicit CQChartsLength(const QString &s, const CQChartsUnits &units=CQChartsUnits::PLOT) {
    setValue(s, units);
  }

  CQChartsLength(const CQChartsLength &rhs) :
    units_(rhs.units_), value_(rhs.value_) {
  }

  CQChartsLength &operator=(const CQChartsLength &rhs) {
    units_ = rhs.units_;
    value_ = rhs.value_;

    return *this;
  }

  bool isValid() const { return units_ != CQChartsUnits::NONE; }

  const CQChartsUnits &units() const { return units_; }

  double value() const { assert(isValid()); return value_; }

  void setValue(const CQChartsUnits &units, double value) {
    units_ = units;
    value_ = value;
  }

  bool setValue(const QString &str, const CQChartsUnits &defUnits=CQChartsUnits::PLOT);

  bool isSet() const {
    return (units_ != CQChartsUnits::PIXEL || value_ != 0.0);
  }

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
  static bool decodeString(const QString &str, CQChartsUnits &units, double &value,
                           const CQChartsUnits &defUnits=CQChartsUnits::PIXEL);

 private:
  CQChartsUnits units_ { CQChartsUnits::PIXEL };
  double        value_ { 0.0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLength)

#endif
