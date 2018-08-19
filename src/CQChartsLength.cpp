#include <CQChartsLength.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsLength, toString, fromString)

void
CQChartsLength::
registerMetaType()
{
  CQUTIL_REGISTER_META(CQChartsLength);
}

bool
CQChartsLength::
decodeString(const QString &str, Units &units, double &value, const Units &defUnits)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (! parse.readReal(&value))
    return false;

  //---

  parse.skipSpace();

  if      (parse.isString("px") || parse.isString("pixel"))
    units = Units::PIXEL;
  else if (parse.isString("%" ) || parse.isString("percent"))
    units = Units::PERCENT;
  else if (parse.isString("V" ) || parse.isString("view"))
    units = Units::VIEW;
  else if (parse.isString("P" ) || parse.isString("plot"))
    units = Units::PLOT;
  else if (parse.eof())
    units = defUnits;
  else
    return false;

  return true;
}
