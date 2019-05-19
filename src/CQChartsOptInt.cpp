#include <CQChartsOptInt.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsOptInt, toString, fromString)

int CQChartsOptInt::metaTypeId;

void
CQChartsOptInt::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptInt);

  CQPropertyViewMgrInst->setUserName("CQChartsOptInt", "optional_integer");
}
