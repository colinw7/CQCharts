#include <CQChartsRect.h>
#include <CQChartsUtil.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsRect, toString, fromString)

int CQChartsRect::metaTypeId;

void
CQChartsRect::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsRect);
}

bool
CQChartsRect::
decodeString(const QString &str, Units &units, QRectF &rect, const Units &defUnits)
{
  CQStrParse parse(str);

  if (! CQChartsUtil::parseRect(parse, rect))
    return false;

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
