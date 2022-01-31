#ifndef CQChartsObjRef_H
#define CQChartsObjRef_H

#include <CQChartsGeom.h>
#include <CQChartsUtil.h>
#include <CQUtilMeta.h>

#include <QString>
#include <iostream>

/*!
 * \brief ObjRef class
 * ingroup Charts
 */
class CQChartsObjRef :
  public CQChartsEqBase<CQChartsObjRef>,
  public CQChartsToStringBase<CQChartsObjRef> {
 public:
  enum class Location {
    NONE,
    CENTER,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    LL,
    UL,
    LR,
    UR,
    INTERSECT
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsObjRef, metaTypeId)

 public:
  using Point = CQChartsGeom::Point;

 public:
  CQChartsObjRef(const QString &name="", const Location &location=Location::CENTER) :
   name_(name), location_(location) {
  }

  //---

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const Location &location() const { return location_; }
  void setLocation(const Location &v) { location_ = v; }

  //---

  bool isValid() const { return name_ != ""; }

  //---

  QString toString() const;
  bool fromString(const QString &s);

  QStringList toStrings() const;
  bool fromStrings(const QStringList &strs);

  //---

  friend bool operator==(const CQChartsObjRef &lhs, const CQChartsObjRef &rhs) {
    if (lhs.name_ != rhs.name_) return false;
    return true;
  }

  //---

  static QString  locationToName(const Location &location);
  static Location nameToLocation(const QString &name);

 private:
  QString  name_;
  Location location_ { Location::CENTER };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsObjRef)

#endif
