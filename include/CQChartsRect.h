#ifndef CQChartsRect_H
#define CQChartsRect_H

#include <QString>
#include <QRectF>
#include <iostream>

class CQChartsRect {
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
  CQChartsRect(const Units &units, const QRectF &rect) :
   units_(units), rect_(rect) {
  }

  CQChartsRect(const QRectF &rect=QRectF(), const Units &units=Units::PLOT) :
   units_(units), rect_(rect) {
  }

  CQChartsRect(const QString &s, const Units &units=Units::PLOT) {
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

  const Units &units() const { return units_; }

  const QRectF &rect() const { return rect_; }

  void setValue(const Units &units, const QRectF &rect) {
    units_ = units;
    rect_  = rect;
  }

  bool setValue(const QString &str, const Units &defUnits=Units::PLOT) {
    Units  units;
    QRectF rect;

    if (! decodeString(str, units, rect, defUnits))
      return false;

    units_ = units;
    rect_  = rect;

    return true;
  }

  //---

  QString toString() const {
    QString ustr;

    if      (units_ == Units::PIXEL  ) ustr = "px";
    else if (units_ == Units::PERCENT) ustr = "%" ;
    else if (units_ == Units::PLOT   ) ustr = "P" ;
    else if (units_ == Units::VIEW   ) ustr = "V" ;
    else                               ustr = ""  ;

    return QString("%1 %2 %3 %4 %5").
             arg(rect_.left()).arg(rect_.bottom()).arg(rect_.right()).arg(rect_.top()).arg(ustr);
  }

  void fromString(const QString &s) {
    setValue(s);
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
  bool decodeString(const QString &str, Units &units, QRectF &rect, const Units &defUnits);

 private:
  Units  units_ { Units::PIXEL };
  QRectF rect_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsRect)

#endif
