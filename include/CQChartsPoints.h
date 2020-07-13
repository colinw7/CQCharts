#ifndef CQChartsPoints_H
#define CQChartsPoints_H

#include <CQChartsPosition.h>
#include <CQChartsUtil.h>
#include <QString>
#include <iostream>

/*!
 * \brief Points class
 * ingroup Charts
 */
class CQChartsPoints {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using Points = std::vector<CQChartsPosition>;
  using Units  = CQChartsUnits;

 public:
  CQChartsPoints(const Points &points=Points()) :
   points_(points) {
  }

  explicit CQChartsPoints(const QString &s, const Units &defUnits=Units::PIXEL) {
    fromString(s, defUnits);
  }

  const Points &points() const { return points_; }

  bool isValid() const { return true; }

  void setPoints(const Points &points) { points_ = points; }

  //---

  QString toString() const;

  bool fromString(const QString &s, const Units &defUnits=Units::PIXEL);

  //---

  friend bool operator==(const CQChartsPoints &lhs, const CQChartsPoints &rhs) {
    int nl = lhs.points_.size();
    int nr = rhs.points_.size();

    if (nl != nr) return false;

    for (int i = 0; i < nl; ++i) {
      if (lhs.points_[i] != rhs.points_[i])
        return false;
    }

    return true;
  }

  friend bool operator!=(const CQChartsPoints &lhs, const CQChartsPoints &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsPoints &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  Points points_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsPoints)

#endif
