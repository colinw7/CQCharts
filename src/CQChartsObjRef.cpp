#include <CQChartsObjRef.h>
#include <CQTclUtil.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsObjRef, toString, fromString)

int CQChartsObjRef::metaTypeId;

void
CQChartsObjRef::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsObjRef);

  CQPropertyViewMgrInst->setUserName("CQChartsObjRef", "objREf");
}

QString
CQChartsObjRef::
toString() const
{
  if (location_ == Location::CENTER)
    return name();

  return CQTcl::mergeList(QStringList() << name() << locationToName(location_));
}

bool
CQChartsObjRef::
fromString(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  if (strs.length() >= 2) {
    setName    (strs[0]);
    setLocation(nameToLocation(strs[1]));
  }
  else {
    setName    (str);
    setLocation(Location::CENTER);
  }

  return true;
}

QString
CQChartsObjRef::
locationToName(const Location &location)
{
  switch (location) {
    case Location::CENTER   : return "center";
    case Location::LEFT     : return "left";
    case Location::RIGHT    : return "right";
    case Location::TOP      : return "top";
    case Location::BOTTOM   : return "bottom";
    case Location::INTERSECT: return "intersect";
    default                 : return "center";
  };
}

CQChartsObjRef::Location
CQChartsObjRef::
nameToLocation(const QString &name)
{
  auto lname = name.toLower();

  if      (lname == "center"   ) return Location::CENTER;
  else if (lname == "left"     ) return Location::LEFT;
  else if (lname == "right"    ) return Location::RIGHT;
  else if (lname == "top"      ) return Location::TOP;
  else if (lname == "bottom"   ) return Location::BOTTOM;
  else if (lname == "intersect") return Location::INTERSECT;

  return Location::CENTER;
}
