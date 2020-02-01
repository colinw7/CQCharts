#include <CQChartsKeyLocation.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsKeyLocation, toString, fromString)

int CQChartsKeyLocation::metaTypeId;

void
CQChartsKeyLocation::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsKeyLocation);

  CQPropertyViewMgrInst->setUserName("CQChartsKeyLocation", "key_location");
}

QString
CQChartsKeyLocation::
toString() const
{
  switch (type_) {
    case Type::AUTO:               return "AUTO";
    case Type::TOP_LEFT:           return "TOP_LEFT";
    case Type::TOP_CENTER:         return "TOP_CENTER";
    case Type::TOP_RIGHT:          return "TOP_RIGHT";
    case Type::CENTER_LEFT:        return "CENTER_LEFT";
    case Type::CENTER_CENTER:      return "CENTER_CENTER";
    case Type::CENTER_RIGHT:       return "CENTER_RIGHT";
    case Type::BOTTOM_LEFT:        return "BOTTOM_LEFT";
    case Type::BOTTOM_CENTER:      return "BOTTOM_CENTER";
    case Type::BOTTOM_RIGHT:       return "BOTTOM_RIGHT";
    case Type::ABSOLUTE_POSITION:  return "ABSOLUTE_POSITION";
    case Type::ABSOLUTE_RECTANGLE: return "ABSOLUTE_RECTANGLE";
    default:                       return "NONE";
  }
}

bool
CQChartsKeyLocation::
decodeString(const QString &str, Type &type)
{
  QString ustr = str.toUpper().replace(' ', '_');

  if      (ustr == "AUTO"              ) type = Type::AUTO;
  else if (ustr == "TOP_LEFT"          ) type = Type::TOP_LEFT;
  else if (ustr == "TOP_CENTER"        ) type = Type::TOP_CENTER;
  else if (ustr == "TOP_RIGHT"         ) type = Type::TOP_RIGHT;
  else if (ustr == "CENTER_LEFT"       ) type = Type::CENTER_LEFT;
  else if (ustr == "CENTER_CENTER"     ) type = Type::CENTER_CENTER;
  else if (ustr == "CENTER_RIGHT"      ) type = Type::CENTER_RIGHT;
  else if (ustr == "BOTTOM_LEFT"       ) type = Type::BOTTOM_LEFT;
  else if (ustr == "BOTTOM_CENTER"     ) type = Type::BOTTOM_CENTER;
  else if (ustr == "BOTTOM_RIGHT"      ) type = Type::BOTTOM_RIGHT;
  else if (ustr == "ABSOLUTE_POSITION" ) type = Type::ABSOLUTE_POSITION;
  else if (ustr == "ABSOLUTE_RECTANGLE") type = Type::ABSOLUTE_RECTANGLE;
  else                                   return false;

  return true;
}

QStringList
CQChartsKeyLocation::
enumNames() const
{
  static QStringList names = QStringList() <<
    "AUTO" << "TOP_LEFT" << "TOP_CENTER" << "TOP_RIGHT" <<
    "CENTER_LEFT" << "CENTER_CENTER" << "CENTER_RIGHT" <<
    "BOTTOM_LEFT" << "BOTTOM_CENTER" << "BOTTOM_RIGHT" <<
    "ABSOLUTE_POSITION" << "ABSOLUTE_RECTANGLE";

  return names;
}
