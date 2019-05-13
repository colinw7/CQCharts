#include <CQChartsAxisTickLabelPlacement.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsAxisTickLabelPlacement, toString, fromString)

int CQChartsAxisTickLabelPlacement::metaTypeId;

void
CQChartsAxisTickLabelPlacement::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsAxisTickLabelPlacement);

  CQPropertyViewMgrInst->setUserName("CQChartsAxisTickLabelPlacement", "axis_tick_label_placement");
}

QString
CQChartsAxisTickLabelPlacement::
toString() const
{
  switch (type_) {
    case Type::MIDDLE     : return "MIDDLE";
    case Type::BOTTOM_LEFT: return "BOTTOM_LEFT";
    case Type::TOP_RIGHT  : return "TOP_RIGHT";
    case Type::BETWEEN    : return "BETWEEN";
    default               : return "NONE";
  }
}

bool
CQChartsAxisTickLabelPlacement::
fromString(const QString &str)
{
  Type type = Type::NONE;

  if      (str == "MIDDLE"     ) type = Type::MIDDLE;
  else if (str == "BOTTOM_LEFT") type = Type::BOTTOM_LEFT;
  else if (str == "TOP_RIGHT"  ) type = Type::TOP_RIGHT;
  else if (str == "BETWEEN"    ) type = Type::BETWEEN;
  else                           return false;

  type_ = type;

  return true;
}

QStringList
CQChartsAxisTickLabelPlacement::
enumNames() const
{
  static QStringList names =
    QStringList() << "MIDDLE" << "BOTTOM_LEFT" << "TOP_RIGHT" << "BETWEEN";

  return names;
}
