#include <CQChartsOptLength.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsOptLength, toString, fromString)

int CQChartsOptLength::metaTypeId;

void
CQChartsOptLength::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptLength);

  CQPropertyViewMgrInst->setUserName("CQChartsOptLength", "optional_length");
}
