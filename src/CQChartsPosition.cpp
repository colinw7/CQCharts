#include <CQChartsPosition.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsPosition, toString, fromString)

int CQChartsPosition::metaTypeId;

void
CQChartsPosition::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPosition);
}

bool
CQChartsPosition::
decodeString(const QString &str, Units &units, QPointF &point, const Units &defUnits)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    return decodeString(str1, units, point, defUnits);
  }

  double x = 0.0;

  if (! parse.readReal(&x))
    return false;

  parse.skipSpace();

  if (parse.isChar(',')) {
    parse.skipChar();

    parse.skipSpace();
  }

  double y = 0.0;

  if (! parse.readReal(&y))
    return false;

  point = QPointF(x, y);

  //---

  if (! decodeUnits(parse.getAt(), units, defUnits))
    return false;

  return true;
}

bool
CQChartsPosition::
decodeUnits(const QString &str, Units &units, const Units &defUnits)
{
  CQStrParse parse(str);

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
