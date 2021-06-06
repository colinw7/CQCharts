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

  CQPropertyViewMgrInst->setUserName("CQChartsObjRef", "objRef");
}

QString
CQChartsObjRef::
toString() const
{
  if (location_ == Location::CENTER)
    return name();

  return CQTcl::mergeList(toStrings());
}

QStringList
CQChartsObjRef::
toStrings() const
{
  return QStringList() << name() << locationToName(location_);
}

bool
CQChartsObjRef::
fromString(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  return fromStrings(strs);
}

bool
CQChartsObjRef::
fromStrings(const QStringList &strs)
{
  if (strs.length() >= 2) {
    setName    (strs[0]);
    setLocation(nameToLocation(strs[1]));
  }
  else {
    setName    (strs[0]);
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
    case Location::LL       : return "ll";
    case Location::UL       : return "ul";
    case Location::LR       : return "lr";
    case Location::UR       : return "ur";
    case Location::INTERSECT: return "intersect";
    default                 : return "none";
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
  else if (lname == "ll"       ) return Location::LL;
  else if (lname == "ul"       ) return Location::UL;
  else if (lname == "lr"       ) return Location::LR;
  else if (lname == "ur"       ) return Location::UR;
  else if (lname == "intersect") return Location::INTERSECT;

  return Location::NONE;
}
