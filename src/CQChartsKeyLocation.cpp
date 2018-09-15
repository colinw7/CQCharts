#include <CQChartsKeyLocation.h>

CQUTIL_DEF_META_TYPE(CQChartsKeyLocation, toString, fromString)

void
CQChartsKeyLocation::
registerMetaType()
{
  CQUTIL_REGISTER_META(CQChartsKeyLocation);
}

QString
CQChartsKeyLocation::
toString() const
{
  switch (type_) {
    case Type::TOP_LEFT:      return "tl";
    case Type::TOP_CENTER:    return "tc";
    case Type::TOP_RIGHT:     return "tr";
    case Type::CENTER_LEFT:   return "cl";
    case Type::CENTER_CENTER: return "cc";
    case Type::CENTER_RIGHT:  return "cr";
    case Type::BOTTOM_LEFT:   return "bl";
    case Type::BOTTOM_CENTER: return "bc";
    case Type::BOTTOM_RIGHT:  return "br";
    case Type::ABS_POS:       return "abs";
    default:                  return "none";
  }
}

bool
CQChartsKeyLocation::
decodeString(const QString &str, Type &type)
{
  QString lstr = str.toLower();

  if      (lstr == "tl" ) type = Type::TOP_LEFT;
  else if (lstr == "tc" ) type = Type::TOP_CENTER;
  else if (lstr == "tr" ) type = Type::TOP_RIGHT;
  else if (lstr == "cl" ) type = Type::CENTER_LEFT;
  else if (lstr == "cc" ) type = Type::CENTER_CENTER;
  else if (lstr == "cr" ) type = Type::CENTER_RIGHT;
  else if (lstr == "bl" ) type = Type::BOTTOM_LEFT;
  else if (lstr == "bc" ) type = Type::BOTTOM_CENTER;
  else if (lstr == "br" ) type = Type::BOTTOM_RIGHT;
  else if (lstr == "abs") type = Type::ABS_POS;

  return true;
}
