#include <CQChartsOptBool.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsOptBool, toString, fromString)

int CQChartsOptBool::metaTypeId;

void
CQChartsOptBool::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptBool);

  CQPropertyViewMgrInst->setUserName("CQChartsOptBool", "optional_boolean");
}
