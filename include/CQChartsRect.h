#ifndef CQChartsRect_H
#define CQChartsRect_H

#include <CQChartsPosition.h>
#include <CQChartsUtil.h>
#include <QString>
#include <QRectF>
#include <iostream>

//! Rectangle class
class CQChartsRect {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsRect(const CQChartsUnits &units, const QRectF &rect) :
   units_(units), rect_(rect) {
  }

  CQChartsRect(const QRectF &rect=QRectF(), const CQChartsUnits &units=CQChartsUnits::PLOT) :
   units_(units), rect_(rect) {
  }

  CQChartsRect(const QString &s, const CQChartsUnits &units=CQChartsUnits::PLOT) {
    setValue(s, units);
  }

  CQChartsRect(const CQChartsRect &rhs) :
    units_(rhs.units_), rect_(rhs.rect_) {
  }

  CQChartsRect &operator=(const CQChartsRect &rhs) {
    units_ = rhs.units_;
    rect_  = rhs.rect_;

    return *this;
  }

  //---

  bool isValid() const { return units_ != CQChartsUnits::NONE; }

  //---

  const CQChartsUnits &units() const { return units_; }
  void setUnits(const CQChartsUnits &units) { units_ = units; }

  //---

  const QRectF &rect() const { return rect_; }

  void setValue(const CQChartsUnits &units, const QRectF &rect) {
    units_ = units;
    rect_  = rect;
  }

  bool setValue(const QString &str, const CQChartsUnits &defUnits=CQChartsUnits::PLOT) {
    CQChartsUnits units;
    QRectF        rect;

    if (! decodeString(str, units, rect, defUnits))
      return false;

    units_ = units;
    rect_  = rect;

    return true;
  }

  bool isSet() const {
    return (units_ != CQChartsUnits::NONE);
  }

  //---

  QString toString() const {
    QString ustr = CQChartsUtil::unitsString(units_);

    return QString("%1 %2 %3 %4 %5").
             arg(rect_.left()).arg(rect_.top()).arg(rect_.right()).arg(rect_.bottom()).arg(ustr);
  }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  friend bool operator==(const CQChartsRect &lhs, const CQChartsRect &rhs) {
    if (lhs.units_ != rhs.units_) return false;
    if (lhs.rect_  != rhs.rect_ ) return false;

    return true;
  }

  friend bool operator!=(const CQChartsRect &lhs, const CQChartsRect &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsRect &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  static bool decodeString(const QString &str, CQChartsUnits &units, QRectF &rect,
                           const CQChartsUnits &defUnits=CQChartsUnits::PLOT);

 private:
  CQChartsUnits units_ { CQChartsUnits::PLOT };
  QRectF        rect_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsRect)

#endif
