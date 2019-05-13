#include <CQChartsPolygonList.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsPolygonList, toString, fromString)

int CQChartsPolygonList::metaTypeId;

void
CQChartsPolygonList::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPolygonList);

  CQPropertyViewMgrInst->setUserName("CQChartsPolygonList", "polygon_list");
}
