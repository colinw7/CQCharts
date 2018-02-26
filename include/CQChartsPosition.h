#ifndef CQChartsPosition_H
#define CQChartsPosition_H

#include <QString>
#include <QPointF>
#include <iostream>

class CQChartsPosition {
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
  CQChartsPosition(const Units &units, const QPointF &p) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const QPointF &p=QPointF(0,0), const Units &units=Units::PLOT) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const QString &s) {
    setPoint(s);
  }

  CQChartsPosition(const CQChartsPosition &rhs) :
    units_(rhs.units_), p_(rhs.p_) {
  }

  CQChartsPosition &operator=(const CQChartsPosition &rhs) {
    units_ = rhs.units_;
    p_ = rhs.p_;

    return *this;
  }

  const Units   &units() const { return units_; }
  const QPointF &p    () const { return p_; }

  void setPoint(const Units &units, const QPointF &p) {
    units_ = units;
    p_     = p;
  }

  bool setPoint(const QString &str) {
    Units   units;
    QPointF p;

    if (! decodeString(str, units, p))
      return false;

    units_ = units;
    p_     = p;

    return true;
  }

  //---

  QString toString() const {
    QString pstr = QString("%1,%2").arg(p_.x()).arg(p_.y());

    if      (units_ == Units::PIXEL  ) return QString("%1px").arg(pstr);
    else if (units_ == Units::PERCENT) return QString("%1%" ).arg(pstr);
    else if (units_ == Units::PLOT   ) return QString("%1"  ).arg(pstr);
    else if (units_ == Units::VIEW   ) return QString("%1V" ).arg(pstr);
    else                               return QString("%1"  ).arg(pstr);
  }

  void fromString(const QString &s) {
    setPoint(s);
  }

  //---

  friend bool operator==(const CQChartsPosition &lhs, const CQChartsPosition &rhs) {
    if (lhs.units_ != rhs.units_) return false;
    if (lhs.p_ != rhs.p_) return false;

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
  bool decodeString(const QString &str, Units &units, QPointF &point) {
    std::string sstr = str.toStdString();

    const char *c_str = sstr.c_str();

    int i = 0;

    while (c_str[i] != 0 && ::isspace(c_str[i]))
      ++i;

    if (c_str[i] == '\0')
      return false;

    double x = 0.0, y = 0.0;

    const char *p;

    //---

    errno = 0;

    x = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE)
      return false;

    while (*p != 0 && ::isspace(*p))
      ++p;

    //---

    if (*p == ',') {
      ++p;

      while (*p != 0 && ::isspace(*p))
        ++p;
    }

    //---

    errno = 0;

    y = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE)
      return false;

    while (*p != 0 && ::isspace(*p))
      ++p;

    //---

    point = QPointF(x, y);

    if      (*p == '\0')
      units = Units::PLOT;
    else if (strcmp(p, "px") == 0)
      units = Units::PIXEL;
    else if (strcmp(p, "%") == 0)
      units = Units::PERCENT;
    else if (strcmp(p, "V") == 0)
      units = Units::VIEW;
    else
      return false;

    return true;
  }

 private:
  Units   units_ { Units::PIXEL };
  QPointF p_     { 0, 0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPosition)

#endif
