#include <CQChartsOptReal.h>

CQUTIL_DEF_META_TYPE(CQChartsOptReal, toString, fromString)

int CQChartsOptReal::metaTypeId;

void
CQChartsOptReal::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptReal);
}
