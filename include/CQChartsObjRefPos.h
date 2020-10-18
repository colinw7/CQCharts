#ifndef CQChartsObjRefPos_H
#define CQChartsObjRefPos_H

#include <CQChartsObjRef.h>
#include <CQChartsPosition.h>

/*!
 * \brief ObjRef Position class
 * ingroup Charts
 */
class CQChartsObjRefPos {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using ObjRef   = CQChartsObjRef;
  using Position = CQChartsPosition;

 public:
  CQChartsObjRefPos() = default;

  CQChartsObjRefPos(const ObjRef &objRef, const Position &position) :
   objRef_(objRef), position_(position) {
  }

  //---

  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &objRef) { objRef_ = objRef; }

  const Position &position() const { return position_; }
  void setPosition(const Position &p) { position_ = p; }

  //---

  QString toString() const;
  bool fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsObjRefPos &lhs, const CQChartsObjRefPos &rhs) {
    if (lhs.objRef_   != rhs.objRef_  ) return false;
    if (lhs.position_ != rhs.position_) return false;
    return true;
  }

  friend bool operator!=(const CQChartsObjRefPos &lhs, const CQChartsObjRefPos &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsObjRefPos &l) {
    l.print(os);

    return os;
  }

 private:
  ObjRef   objRef_;
  Position position_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsObjRefPos)

#endif
