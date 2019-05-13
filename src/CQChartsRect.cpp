#include <CQChartsRect.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsRect, toString, fromString)

int CQChartsRect::metaTypeId;

void
CQChartsRect::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsRect);

  CQPropertyViewMgrInst->setUserName("CQChartsRect", "rectangle");
}

bool
CQChartsRect::
decodeString(const QString &str, CQChartsUnits &units, QRectF &rect,
             const CQChartsUnits &defUnits)
{
  CQStrParse parse(str);

  if (! CQChartsUtil::parseRect(parse, rect))
    return false;

  parse.skipSpace();

  if (! CQChartsUtil::decodeUnits(parse.getAt(), units, defUnits))
    return false;

  return true;
}
