#ifndef CQChartsObjRef_H
#define CQChartsObjRef_H

#include <CQChartsGeom.h>
#include <CQChartsUtil.h>
#include <QString>
#include <iostream>

/*!
 * \brief ObjRef class
 * ingroup Charts
 */
class CQChartsObjRef {
 public:
  enum class Location {
    CENTER,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    INTERSECT
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

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

  //---

  friend bool operator==(const CQChartsObjRef &lhs, const CQChartsObjRef &rhs) {
    if (lhs.name_ != rhs.name_) return false;
    return true;
  }

  friend bool operator!=(const CQChartsObjRef &lhs, const CQChartsObjRef &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsObjRef &l) {
    l.print(os);

    return os;
  }

  //---

  static QString  locationToName(const Location &location);
  static Location nameToLocation(const QString &name);


 private:
  QString  name_;
  Location location_ { Location::CENTER };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsObjRef)

#endif
