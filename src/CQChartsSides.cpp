#include <CQChartsSides.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsSides, toString, fromString)

int CQChartsSides::metaTypeId;

void
CQChartsSides::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsSides);

  CQPropertyViewMgrInst->setUserName("CQChartsSides", "sides");
}

bool
CQChartsSides::
decodeString(const QString &str, Sides &sides)
{
  sides = 0;

  if (str.indexOf('t') >= 0) sides |= int(Side::TOP   );
  if (str.indexOf('l') >= 0) sides |= int(Side::LEFT  );
  if (str.indexOf('b') >= 0) sides |= int(Side::BOTTOM);
  if (str.indexOf('r') >= 0) sides |= int(Side::RIGHT );

  return true;
}
