#ifndef CQChartsPosition_H
#define CQChartsPosition_H

#include <CQChartsUtil.h>
#include <QString>
#include <QPointF>
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
  CQChartsPosition(const CQChartsUnits &units, const QPointF &p) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const QPointF &p=QPointF(0,0), const CQChartsUnits &units=CQChartsUnits::PLOT) :
   units_(units), p_(p) {
  }

  CQChartsPosition(const QString &s, const CQChartsUnits &defUnits=CQChartsUnits::PIXEL) {
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

  const CQChartsUnits &units() const { return units_; }
  const QPointF       &p    () const { return p_; }

  bool isValid() const { return units_ != CQChartsUnits::NONE; }

  void setPoint(const CQChartsUnits &units, const QPointF &p) {
    units_ = units;
    p_     = p;
  }

  bool setPoint(const QString &str, const CQChartsUnits &defUnits=CQChartsUnits::PIXEL) {
    CQChartsUnits units;
    QPointF       p;

    if (! decodeString(str, units, p, defUnits))
      return false;

    units_ = units;
    p_     = p;

    return true;
  }

  bool isSet() const {
    return (units_ != CQChartsUnits::NONE);
  }

  //---

  QString toString() const {
    QString ustr = CQChartsUtil::unitsString(units_);

    return QString("%1 %2 %3").arg(p_.x()).arg(p_.y()).arg(ustr);
  }

  bool fromString(const QString &s) {
    return setPoint(s);
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
  static bool decodeString(const QString &str, CQChartsUnits &units, QPointF &point,
                           const CQChartsUnits &defUnits=CQChartsUnits::PIXEL);

 private:
  CQChartsUnits units_ { CQChartsUnits::PIXEL };
  QPointF       p_     { 0, 0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPosition)

#endif
