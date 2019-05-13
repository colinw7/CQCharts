#include <CQChartsTitleLocation.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsTitleLocation, toString, fromString)

int CQChartsTitleLocation::metaTypeId;

void
CQChartsTitleLocation::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsTitleLocation);

  CQPropertyViewMgrInst->setUserName("CQChartsTitleLocation", "title_location");
}

QString
CQChartsTitleLocation::
toString() const
{
  switch (type_) {
    case Type::TOP:      return "TOP";
    case Type::CENTER:   return "CENTER";
    case Type::BOTTOM:   return "BOTTOM";
    case Type::ABS_POS:  return "ABS_POS";
    case Type::ABS_RECT: return "ABS_RECT";
    default:             return "NONE";
  }
}

bool
CQChartsTitleLocation::
fromString(const QString &str)
{
  Type type { Type::TOP };

  QString lstr = str.toLower();

  if      (lstr == "top"     ) type = Type::TOP;
  else if (lstr == "center"  ) type = Type::CENTER;
  else if (lstr == "bottom"  ) type = Type::BOTTOM;
  else if (lstr == "abs_pos" ) type = Type::ABS_POS;
  else if (lstr == "abs_rect") type = Type::ABS_RECT;
  else                         return false;

  type_ = type;

  return true;
}

QStringList
CQChartsTitleLocation::
enumNames() const
{
  static QStringList names = QStringList() <<
    "TOP" << "CENTER" << "BOTTOM" << "ABS_POS" << "ABS_RECT";

  return names;
}
