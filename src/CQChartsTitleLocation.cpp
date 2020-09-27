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
    case Type::TOP:                return "TOP";
    case Type::CENTER:             return "CENTER";
    case Type::BOTTOM:             return "BOTTOM";
    case Type::ABSOLUTE_POSITION:  return "ABSOLUTE_POSITION";
    case Type::ABSOLUTE_RECTANGLE: return "ABSOLUTE_RECTANGLE";
    default:                       return "NONE";
  }
}

bool
CQChartsTitleLocation::
fromString(const QString &str)
{
  return setValue(str);
}

bool
CQChartsTitleLocation::
setValue(const QString &str)
{
  Type type = Type::NONE;

  if (! decodeString(str, type))
    return false;

  type_ = type;

  return true;
}

bool
CQChartsTitleLocation::
decodeString(const QString &str, Type &type)
{
  auto ustr = str.toUpper().replace(' ', '_');

  if      (ustr == "TOP"               ) type = Type::TOP;
  else if (ustr == "CENTER"            ) type = Type::CENTER;
  else if (ustr == "BOTTOM"            ) type = Type::BOTTOM;
  else if (ustr == "ABSOLUTE_POSITION" ) type = Type::ABSOLUTE_POSITION;
  else if (ustr == "ABSOLUTE_RECTANGLE") type = Type::ABSOLUTE_RECTANGLE;
  else                                   return false;

  return true;
}

QStringList
CQChartsTitleLocation::
enumNames() const
{
  static QStringList names = QStringList() <<
    "TOP" << "CENTER" << "BOTTOM" << "ABSOLUTE_POSITION" << "ABSOLUTE_RECTANGLE";

  return names;
}
