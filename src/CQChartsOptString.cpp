#include <CQChartsOptString.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsOptString, toString, fromString)

int CQChartsOptString::metaTypeId;

void
CQChartsOptString::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptString);

  CQPropertyViewMgrInst->setUserName("CQChartsOptString", "optional_string");
}
