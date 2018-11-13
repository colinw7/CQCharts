#ifndef CQChartsLength_H
#define CQChartsLength_H

#include <QString>
#include <iostream>

class CQChartsLength {
 public:
  enum class Units {
    NONE,
    VIEW,
    PLOT,
    PIXEL,
    PERCENT
  };

 public:
  static void registerMetaType();

 public:
  CQChartsLength(const Units &units, double value) :
   units_(units), value_(value) {
  }

  CQChartsLength(double value=0.0, const Units &units=Units::PLOT) :
   units_(units), value_(value) {
  }

  CQChartsLength(const QString &s, const Units &units=Units::PLOT) {
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

  const Units &units() const { return units_; }
  double       value() const { return value_; }

  void setValue(const Units &units, double value) {
    units_ = units;
    value_ = value;
  }

  bool setValue(const QString &str, const Units &defUnits=Units::PLOT) {
    Units  units;
    double value;

    if (! decodeString(str, units, value, defUnits))
      return false;

    units_ = units;
    value_ = value;

    return true;
  }

  bool isSet() const {
    return (units_ != Units::PIXEL || value_ != 0.0);
  }

  //---

  QString toString() const {
    QString ustr;

    if      (units_ == Units::PIXEL  ) ustr = "px";
    else if (units_ == Units::PERCENT) ustr = "%" ;
    else if (units_ == Units::PLOT   ) ustr = "P" ;
    else if (units_ == Units::VIEW   ) ustr = "V" ;
    else                               ustr = ""  ;

    return QString("%1%2").arg(value_).arg(ustr);
  }

  void fromString(const QString &s) {
    setValue(s);
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
  bool decodeString(const QString &str, Units &units, double &value, const Units &defUnits);

 private:
  Units  units_ { Units::PIXEL };
  double value_ { 0.0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLength)

#endif
