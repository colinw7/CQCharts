#include <CQChartsPolygon.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsPolygon, toString, fromString)

int CQChartsPolygon::metaTypeId;

void
CQChartsPolygon::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPolygon);

  CQPropertyViewMgrInst->setUserName("CQChartsPolygon", "polygon");
}

bool
CQChartsPolygon::
decodeString(const QString &str, CQChartsUnits &units, QPolygonF &polygon,
             const CQChartsUnits &defUnits)
{
  CQStrParse parse(str);

  if (! CQChartsUtil::parsePolygon(parse, polygon))
    return false;

  parse.skipSpace();

  if (! CQChartsUtil::decodeUnits(parse.getAt(), units, defUnits))
    return false;

  return true;
}
