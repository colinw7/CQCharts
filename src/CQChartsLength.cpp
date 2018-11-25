#include <CQChartsLength.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsLength, toString, fromString)

int CQChartsLength::metaTypeId;

void
CQChartsLength::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsLength);
}

bool
CQChartsLength::
decodeString(const QString &str, CQChartsUnits &units, double &value,
             const CQChartsUnits &defUnits)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (! parse.readReal(&value))
    return false;

  //---

  parse.skipSpace();

  if (! CQChartsUtil::decodeUnits(parse.getAt(), units, defUnits))
    return false;

  return true;
}
