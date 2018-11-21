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

  static int metaTypeId;

 public:
  CQChartsPosition(const Units &units, const QPointF &p) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const QPointF &p=QPointF(0,0), const Units &units=Units::PLOT) :
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
    p_ = rhs.p_;

    return *this;
  }

  const Units   &units() const { return units_; }
  const QPointF &p    () const { return p_; }

  void setPoint(const Units &units, const QPointF &p) {
    units_ = units;
    p_     = p;
  }

  bool setPoint(const QString &str, const Units &defUnits=Units::PIXEL) {
    Units   units;
    QPointF p;

    if (! decodeString(str, units, p, defUnits))
      return false;

    units_ = units;
    p_     = p;

    return true;
  }

  bool isSet() const {
    return (units_ != Units::PIXEL || p_.x() != 0.0 || p_.y());
  }

  //---

  QString toString() const {
    QString ustr = unitsString(units_);

    return QString("%1 %2 %3").arg(p_.x()).arg(p_.y()).arg(ustr);
  }

  void fromString(const QString &s) {
    setPoint(s);
  }

  static QString unitsString(const Units &units) {
    if      (units == Units::PIXEL  ) return "px";
    else if (units == Units::PERCENT) return "%" ;
    else if (units == Units::PLOT   ) return "P" ;
    else if (units == Units::VIEW   ) return "V" ;
    else                              return ""  ;
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

  static bool decodeUnits(const QString &str, Units &units, const Units &defUnits=Units::PIXEL);

 private:
  static bool decodeString(const QString &str, Units &units, QPointF &point,
                           const Units &defUnits=Units::PIXEL);

 private:
  Units   units_ { Units::PIXEL };
  QPointF p_     { 0, 0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPosition)

#endif
