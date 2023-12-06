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

QString
CQChartsSides::
toString() const
{
  QString str;

  if (isLeft  ()) str += "l";
  if (isRight ()) str += "r";
  if (isTop   ()) str += "t";
  if (isBottom()) str += "b";

  return str;
}

bool
CQChartsSides::
fromString(const QString &s)
{
  return setValue(s);
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
