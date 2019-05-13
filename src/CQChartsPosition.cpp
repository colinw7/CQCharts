#include <CQChartsPosition.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsPosition, toString, fromString)

int CQChartsPosition::metaTypeId;

void
CQChartsPosition::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPosition);

  CQPropertyViewMgrInst->setUserName("CQChartsPosition", "position");
}

bool
CQChartsPosition::
decodeString(const QString &str, CQChartsUnits &units, QPointF &point,
             const CQChartsUnits &defUnits)
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

  if (! CQChartsUtil::decodeUnits(parse.getAt(), units, defUnits))
    return false;

  return true;
}
