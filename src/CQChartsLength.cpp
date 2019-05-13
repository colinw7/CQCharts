#include <CQChartsLength.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsLength, toString, fromString)

int CQChartsLength::metaTypeId;

void
CQChartsLength::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsLength);

  CQPropertyViewMgrInst->setUserName("CQChartsLength", "length");
}

bool
CQChartsLength::
setValue(const QString &str, const CQChartsUnits &defUnits)
{
  CQChartsUnits units;
  double        value;

  if (! decodeString(str, units, value, defUnits))
    return false;

  units_ = units;
  value_ = value;

  return true;
}

QString
CQChartsLength::
toString() const
{
  QString ustr = CQChartsUtil::unitsString(units_);

  return QString("%1%2").arg(value_).arg(ustr);
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
