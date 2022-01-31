#ifndef CQChartsPoints_H
#define CQChartsPoints_H

#include <CQChartsPosition.h>
#include <CQChartsUtil.h>
#include <CQUtilMeta.h>

#include <QString>
#include <iostream>

/*!
 * \brief Points class
 * ingroup Charts
 */
class CQChartsPoints :
  public CQChartsEqBase<CQChartsPoints>,
  public CQChartsToStringBase<CQChartsPoints> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsPoints, metaTypeId)

 public:
  using Position = CQChartsPosition;
  using Points   = std::vector<Position>;
  using Units    = CQChartsUnits::Type;

 public:
  CQChartsPoints(const Points &points=Points()) :
   points_(points) {
  }

  explicit CQChartsPoints(const QString &s, const Units &defUnits=Units::PIXEL) {
    fromString(s, defUnits);
  }

  //---

  bool isValid() const { return true; }

  const Points &points() const { return points_; }

  void setPoints(const Points &points) { points_ = points; }

  void addPoint(const Position &pos) { points_.push_back(pos); }

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

  //---

 private:
  Points points_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsPoints)

#endif
