#include <CQChartsOptPosition.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsOptPosition, toString, fromString)

int CQChartsOptPosition::metaTypeId;

void
CQChartsOptPosition::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptPosition);

  CQPropertyViewMgrInst->setUserName("CQChartsOptPosition", "optional_position");
}
