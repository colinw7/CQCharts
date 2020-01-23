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
    case Type::TOP:           return "TOP";
    case Type::CENTER:        return "CENTER";
    case Type::BOTTOM:        return "BOTTOM";
    case Type::ABS_POSITION:  return "ABS_POSITION";
    case Type::ABS_RECTANGLE: return "ABS_RECTANGLE";
    default:                  return "NONE";
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
  Type type { Type::TOP };

  QString lstr = str.toLower();

  if      (lstr == "top"          ) type = Type::TOP;
  else if (lstr == "center"       ) type = Type::CENTER;
  else if (lstr == "bottom"       ) type = Type::BOTTOM;
  else if (lstr == "abs_position" ) type = Type::ABS_POSITION;
  else if (lstr == "abs_rectangle") type = Type::ABS_RECTANGLE;
  else                              return false;

  type_ = type;

  return true;
}

QStringList
CQChartsTitleLocation::
enumNames() const
{
  static QStringList names = QStringList() <<
    "TOP" << "CENTER" << "BOTTOM" << "ABS_POSITION" << "ABS_RECTANGLE";

  return names;
}
