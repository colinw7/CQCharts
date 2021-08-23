#include <CQChartsUnits.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsUnits, toString, fromString)

int CQChartsUnits::metaTypeId;

void
CQChartsUnits::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsUnits);

  CQPropertyViewMgrInst->setUserName("CQChartsUnits", "units");
}

QString
CQChartsUnits::
unitsString(const Type &type)
{
  if      (type == Type::PIXEL  ) return "px";
  else if (type == Type::PERCENT) return "%" ;
  else if (type == Type::PLOT   ) return "P" ;
  else if (type == Type::VIEW   ) return "V" ;
  else if (type == Type::EM     ) return "em";
  else if (type == Type::EX     ) return "ex";
  else                            return ""  ;
}

bool
CQChartsUnits::
decodeUnits(const QString &str, Type &type, const Type &defType)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if      (parse.isString("px") || parse.isString("pixel"))
    type = Type::PIXEL;
  else if (parse.isString("%" ) || parse.isString("percent"))
    type = Type::PERCENT;
  else if (parse.isString("V" ) || parse.isString("view"))
    type = Type::VIEW;
  else if (parse.isString("P" ) || parse.isString("plot"))
    type = Type::PLOT;
  else if (parse.isString("em"))
    type = Type::EM;
  else if (parse.isString("ex"))
    type = Type::EX;
  else if (parse.eof())
    type = defType;
  else
    return false;

  return true;
}

QStringList
CQChartsUnits::
unitNames(bool includeNone)
{
  static auto names      = QStringList() << "px" << "%" << "P" << "V" << "em" << "ex";
  static auto none_names = QStringList() << "none" << names;

  return (includeNone ? none_names : names);
}

QStringList
CQChartsUnits::
unitTipNames(bool includeNone)
{
  static auto names = QStringList() <<
    "px (Pixel)" << "% (Percent)" << "P (Plot)" << "V (View)" <<
    "em (Font Height)" << "ex (Font 'x' Character Width)";

  static auto none_names = QStringList() << "None" << names;

  return (includeNone ? none_names : names);
}
