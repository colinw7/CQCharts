#include <CQChartsPosition.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

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
decodeString(const QString &str, CQChartsUnits &units, CQChartsGeom::Point &point,
             const CQChartsUnits &defUnits)
{
  // format is <x> <y> [<units>]

  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  if (strs.length() < 2)
    return false;

  double x, y;

  if (! CQChartsUtil::toReal(strs[0], x))
    return false;

  if (! CQChartsUtil::toReal(strs[1], y))
    return false;

  point = CQChartsGeom::Point(x, y);

  //---

  if (strs.length() > 2) {
    if (! CQChartsUtil::decodeUnits(strs[2], units, defUnits))
      return false;
  }

  return true;
}
