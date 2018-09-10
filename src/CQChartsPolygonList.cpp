#include <CQChartsPolygonList.h>

CQUTIL_DEF_META_TYPE(CQChartsPolygonList, toString, fromString)

int CQChartsPolygonList::metaTypeId;

void
CQChartsPolygonList::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPolygonList);
}
