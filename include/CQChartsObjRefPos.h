#ifndef CQChartsObjRefPos_H
#define CQChartsObjRefPos_H

#include <CQChartsObjRef.h>
#include <CQChartsPosition.h>
#include <CQUtilMeta.h>

/*!
 * \brief ObjRef Position class
 * ingroup Charts
 */
class CQChartsObjRefPos :
  public CQChartsEqBase<CQChartsObjRefPos>,
  public CQChartsToStringBase<CQChartsObjRefPos> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsObjRefPos, metaTypeId)

 public:
  using ObjRef   = CQChartsObjRef;
  using Position = CQChartsPosition;
  using Units    = CQChartsUnits::Type;
  using Point    = CQChartsGeom::Point;

 public:
  static CQChartsObjRefPos plot(const Point &p) {
    return CQChartsObjRefPos(Position(p, Units::PLOT)); }
  static CQChartsObjRefPos plot(const QString &str) {
    return CQChartsObjRefPos(Position(str, Units::PLOT)); }

 public:
  CQChartsObjRefPos() = default;

  CQChartsObjRefPos(const ObjRef &objRef, const Position &position) :
   objRef_(objRef), position_(position) {
  }

  CQChartsObjRefPos(const Position &position) :
   position_(position) {
  }

  CQChartsObjRefPos(const QString &s, const Units &units=Units::PLOT) {
    setObjRef(s, units);
  }

  //---

  bool isValid() const {
    if (! position_.isValid())
      return false;

    return true;
  }

  //---

  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &objRef) { objRef_ = objRef; }

  const Position &position() const { return position_; }
  void setPosition(const Position &p) { position_ = p; }

  bool setObjRef(const QString &str, const Units &units=Units::PLOT) {
    ObjRef   objRef;
    Position position;

    if (! decodeString(str, objRef, position, units))
      return false;

    objRef_   = objRef;
    position_ = position;

    return true;
  }

  //---

  QString toString() const;

  bool fromString(const QString &s, const Units &units=Units::PLOT) {
    return setObjRef(s, units);
  }

  bool decodeString(const QString &str, ObjRef &objRef, Position &position,
                    const Units &units=Units::PLOT);

  //---

  friend bool operator==(const CQChartsObjRefPos &lhs, const CQChartsObjRefPos &rhs) {
    if (lhs.objRef_   != rhs.objRef_  ) return false;
    if (lhs.position_ != rhs.position_) return false;
    return true;
  }

  //---

 private:
  ObjRef   objRef_;
  Position position_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsObjRefPos)

#endif
