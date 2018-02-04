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

  CQChartsLength(const QString &s) {
    setValue(s);
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

  bool setValue(const QString &str) {
    Units  units;
    double value;

    if (! decodeString(str, units, value))
      return false;

    units_ = units;
    value_ = value;

    return true;
  }

  //---

  QString toString() const {
    if      (units_ == Units::PIXEL  ) return QString("%1px").arg(value_);
    else if (units_ == Units::PERCENT) return QString("%1%" ).arg(value_);
    else if (units_ == Units::PLOT   ) return QString("%1"  ).arg(value_);
    else                               return QString("%1"  ).arg(value_);
  }

  void fromString(const QString &s) {
    setValue(s);
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
  bool decodeString(const QString &str, Units &units, double &value) {
    std::string sstr = str.toStdString();

    const char *c_str = sstr.c_str();

    int i = 0;

    while (c_str[i] != 0 && ::isspace(c_str[i]))
      ++i;

    if (c_str[i] == '\0')
      return false;

    const char *p;

    errno = 0;

    value = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE)
      return false;

    while (*p != 0 && ::isspace(*p))
      ++p;

    if      (*p == '\0')
      units = Units::PLOT;
    else if (strcmp(p, "px") == 0)
      units = Units::PIXEL;
    else if (strcmp(p, "%") == 0)
      units = Units::PERCENT;
    else
      return false;

    return true;
  }

 private:
  Units units_  { Units::PIXEL };
  double value_ { 0.0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLength)

#endif
