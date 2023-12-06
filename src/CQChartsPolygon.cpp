#include <CQChartsPolygon.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsPolygon, toString, fromString)

int CQChartsPolygon::metaTypeId;

void
CQChartsPolygon::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPolygon);

  CQPropertyViewMgrInst->setUserName("CQChartsPolygon", "polygon");
}

QString
CQChartsPolygon::
toString() const
{
  if (! isValid())
    return "";

  //---

  auto ustr = CQChartsUnits::unitsString(units_);

  QString str;

  for (int i = 0; i < polygon_.size(); ++i) {
    if (i > 0) str += " ";

    str += QString("%1 %2").arg(polygon_.point(i).x).arg(polygon_.point(i).y);
  }

  str += " " + ustr;

  return str;
}

bool
CQChartsPolygon::
fromString(const QString &s)
{
  if (s.trimmed() == "") {
    *this = CQChartsPolygon();
    return true;
  }

  return setValue(s);
}

bool
CQChartsPolygon::
decodeString(const QString &str, Units &units, Polygon &polygon, const Units &defUnits)
{
  CQStrParse parse(str);

  if (! CQChartsUtil::parsePolygon(parse, polygon))
    return false;

  parse.skipSpace();

  if (! CQChartsUnits::decodeUnits(parse.getAt(), units, defUnits))
    return false;

  return true;
}
