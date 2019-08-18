#include <CQChartsPaletteName.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsPaletteName, toString, fromString)

int CQChartsPaletteName::metaTypeId;

void
CQChartsPaletteName::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPaletteName);

  CQPropertyViewMgrInst->setUserName("CQChartsPaletteName", "paletteName");
}
