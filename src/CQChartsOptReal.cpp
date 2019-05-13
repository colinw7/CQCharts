#include <CQChartsOptReal.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsOptReal, toString, fromString)

int CQChartsOptReal::metaTypeId;

void
CQChartsOptReal::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptReal);

  CQPropertyViewMgrInst->setUserName("CQChartsOptReal", "optional_real");
}
