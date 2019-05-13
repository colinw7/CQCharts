#include <CQChartsAxisSide.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsAxisSide, toString, fromString)

int CQChartsAxisSide::metaTypeId;

void
CQChartsAxisSide::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsAxisSide);

  CQPropertyViewMgrInst->setUserName("CQChartsAxisSide", "axis_side");
}

QString
CQChartsAxisSide::
toString() const
{
  switch (type_) {
    case Type::BOTTOM_LEFT: return "BOTTOM_LEFT";
    case Type::TOP_RIGHT  : return "TOP_RIGHT";
    default               : return "NONE";
  }
}

bool
CQChartsAxisSide::
fromString(const QString &str)
{
  Type type = Type::NONE;

  if      (str == "BOTTOM_LEFT") type = Type::BOTTOM_LEFT;
  else if (str == "TOP_RIGHT"  ) type = Type::TOP_RIGHT;
  else                           return false;

  type_ = type;

  return true;
}

QStringList
CQChartsAxisSide::
enumNames() const
{
  static QStringList names = QStringList() << "BOTTOM_LEFT" << "TOP_RIGHT";

  return names;
}
