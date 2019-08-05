#ifndef CQChartsPolygon_H
#define CQChartsPolygon_H

#include <CQChartsUtil.h>
#include <QString>
#include <QPolygonF>
#include <iostream>

/*!
 * \brief Polygon class
 * \ingroup Charts
 */
class CQChartsPolygon {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsPolygon(const CQChartsUnits &units, const QPolygonF &polygon) :
   units_(units), polygon_(polygon) {
  }

  CQChartsPolygon(const QPolygonF &polygon=QPolygonF(),
                  const CQChartsUnits &units=CQChartsUnits::PLOT) :
   units_(units), polygon_(polygon) {
  }

  CQChartsPolygon(const QString &s, const CQChartsUnits &units=CQChartsUnits::PLOT) {
    setValue(s, units);
  }

  CQChartsPolygon(const CQChartsPolygon &rhs) :
    units_(rhs.units_), polygon_(rhs.polygon_) {
  }

  CQChartsPolygon &operator=(const CQChartsPolygon &rhs) {
    units_   = rhs.units_;
    polygon_ = rhs.polygon_;

    return *this;
  }

  //---

  bool isValid() const { return units_ != CQChartsUnits::NONE && polygon_.size() > 0; }

  //---

  const CQChartsUnits &units() const { return units_; }
  void setUnits(const CQChartsUnits &units) { units_ = units; }

  //---

  const QPolygonF &polygon() const { return polygon_; }

  int numPoints() const { return polygon_.size(); }

  const QPointF &point(int i) const { return polygon_[i]; }
  void setPoint(int i, const QPointF &p) { polygon_[i] = p; }

  void addPoint(const QPointF &p) { polygon_ << p; }

  void removePoint() { polygon_.pop_back(); }

  //---

  void setValue(const CQChartsUnits &units, const QPolygonF &polygon) {
    units_   = units;
    polygon_ = polygon;
  }

  bool setValue(const QString &str, const CQChartsUnits &defUnits=CQChartsUnits::PLOT) {
    CQChartsUnits units;
    QPolygonF     polygon;

    if (! decodeString(str, units, polygon, defUnits))
      return false;

    units_   = units;
    polygon_ = polygon;

    return true;
  }

  //---

  QString toString() const {
    if (! isValid())
      return "";

    //---

    QString ustr = CQChartsUtil::unitsString(units_);

    QString str;

    for (int i = 0; i < polygon_.size(); ++i) {
      if (i > 0) str += " ";

      str += QString("%1 %2").arg(polygon_[i].x()).arg(polygon_[i].y());
    }

    str += " " + ustr;

    return str;
  }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  friend bool operator==(const CQChartsPolygon &lhs, const CQChartsPolygon &rhs) {
    if (lhs.units_   != rhs.units_  ) return false;
    if (lhs.polygon_ != rhs.polygon_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsPolygon &lhs, const CQChartsPolygon &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsPolygon &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  static bool decodeString(const QString &str, CQChartsUnits &units, QPolygonF &polygon,
                           const CQChartsUnits &defUnits=CQChartsUnits::PLOT);

 private:
  CQChartsUnits units_ { CQChartsUnits::PIXEL };
  QPolygonF     polygon_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPolygon)

#endif
