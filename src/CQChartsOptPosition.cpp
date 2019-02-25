#include <CQChartsOptPosition.h>

CQUTIL_DEF_META_TYPE(CQChartsOptPosition, toString, fromString)

int CQChartsOptPosition::metaTypeId;

void
CQChartsOptPosition::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptPosition);
}
