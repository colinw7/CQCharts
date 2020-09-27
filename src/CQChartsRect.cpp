#include <CQChartsRect.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsRect, toString, fromString)

int CQChartsRect::metaTypeId;

void
CQChartsRect::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsRect);

  CQPropertyViewMgrInst->setUserName("CQChartsRect", "rectangle");
}

bool
CQChartsRect::
setValue(const QString &str, const CQChartsUnits &defUnits)
{
  CQChartsUnits units;
  BBox          bbox;

  if (! decodeString(str, units, bbox, defUnits))
    return false;

  units_ = units;
  bbox_  = bbox;

  return true;
}

CQChartsPosition
CQChartsRect::
center() const
{
  return Position(bbox_.getCenter(), units_);
}

CQChartsLength
CQChartsRect::
xRadius() const
{
  return Length(bbox_.getWidth()/2.0, units_);
}

CQChartsLength
CQChartsRect::
yRadius() const
{
  return Length(bbox_.getHeight()/2.0, units_);
}

QString
CQChartsRect::
toString() const
{
  if (isValid()) {
    auto ustr = CQChartsUtil::unitsString(units_);

    return QString("%1 %2 %3 %4 %5").
             arg(bbox_.getXMin()).arg(bbox_.getYMin()).
             arg(bbox_.getXMax()).arg(bbox_.getYMax()).arg(ustr);
  }
  else
    return QString();
}

bool
CQChartsRect::
decodeString(const QString &str, CQChartsUnits &units, BBox &bbox, const CQChartsUnits &defUnits)
{
  CQStrParse parse(str);

  if (! CQChartsUtil::parseBBox(parse, bbox, /*terminated*/false))
    return false;

  parse.skipSpace();

  if (! CQChartsUtil::decodeUnits(parse.getAt(), units, defUnits))
    return false;

  return true;
}
