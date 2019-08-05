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
    case Type::AUTO:          return "AUTO";
    case Type::TOP_LEFT:      return "TOP_LEFT";
    case Type::TOP_CENTER:    return "TOP_CENTER";
    case Type::TOP_RIGHT:     return "TOP_RIGHT";
    case Type::CENTER_LEFT:   return "CENTER_LEFT";
    case Type::CENTER_CENTER: return "CENTER_CENTER";
    case Type::CENTER_RIGHT:  return "CENTER_RIGHT";
    case Type::BOTTOM_LEFT:   return "BOTTOM_LEFT";
    case Type::BOTTOM_CENTER: return "BOTTOM_CENTER";
    case Type::BOTTOM_RIGHT:  return "BOTTOM_RIGHT";
    case Type::ABS_POSITION:  return "ABS_POSITION";
    case Type::ABS_RECT:      return "ABS_RECT";
    default:                  return "NONE";
  }
}

bool
CQChartsKeyLocation::
decodeString(const QString &str, Type &type)
{
  QString lstr = str.toLower();

  if      (lstr == "auto"         ) type = Type::AUTO;
  else if (lstr == "top_left"     ) type = Type::TOP_LEFT;
  else if (lstr == "top_center"   ) type = Type::TOP_CENTER;
  else if (lstr == "top_right"    ) type = Type::TOP_RIGHT;
  else if (lstr == "center_left"  ) type = Type::CENTER_LEFT;
  else if (lstr == "center_center") type = Type::CENTER_CENTER;
  else if (lstr == "center_right" ) type = Type::CENTER_RIGHT;
  else if (lstr == "bottom_left"  ) type = Type::BOTTOM_LEFT;
  else if (lstr == "bottom_center") type = Type::BOTTOM_CENTER;
  else if (lstr == "bottom_right" ) type = Type::BOTTOM_RIGHT;
  else if (lstr == "abs_position" ) type = Type::ABS_POSITION;
  else if (lstr == "abs_rect"     ) type = Type::ABS_RECT;
  else                              return false;

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
    "ABS_POSITION" << "ABS_RECT";

  return names;
}
