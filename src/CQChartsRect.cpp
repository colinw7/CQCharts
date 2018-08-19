#include <CQChartsRect.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsRect, toString, fromString)

void
CQChartsRect::
registerMetaType()
{
  CQUTIL_REGISTER_META(CQChartsRect);
}

bool
CQChartsRect::
decodeString(const QString &str, Units &units, QRectF &rect, const Units &defUnits)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    return decodeString(str1, units, rect, defUnits);
  }

  QPointF p1, p2;

  if (! parsePoint(parse, p1))
    return false;

  if (! parsePoint(parse, p2))
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

bool
CQChartsRect::
parsePoint(CQStrParse &parse, QPointF &p)
{
  parse.skipSpace();

  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    CQStrParse parse1(str1);

    return parsePoint(parse1, p);
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

  p = QPointF(x, y);

  return true;
}
