#include <CQChartsGeom.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE_ID(CQChartsGeom::BBox , CQChartsGeomBBox , toString, fromString)
CQUTIL_DEF_META_TYPE_ID(CQChartsGeom::Point, CQChartsGeomPoint, toString, fromString)

namespace CQChartsGeom {

BBox::
BBox(const Range &range) :
 pmin_(range.xmin(), range.ymin()), pmax_(range.xmax(), range.ymax()), set_(true) {
  update();
}

QString
BBox::
toString() const
{
  return CQChartsUtil::bboxToString(*this);
}

bool
BBox::
fromString(const QString &s)
{
  CQChartsGeom::BBox bbox;

  if (! CQChartsUtil::stringToBBox(s, bbox))
    return false;

  pmin_ = bbox.pmin_;
  pmax_ = bbox.pmax_;
  set_  = true;

  return true;
}

//---

QString
Point::
toString() const
{
  return CQChartsUtil::pointToString(*this);
}

bool
Point::
fromString(const QString &s)
{
  CQChartsGeom::Point p;

  if (! CQChartsUtil::stringToPoint(s, p))
    return false;

  x = p.x;
  y = p.y;

  return true;
}

//---

int bboxMetaTypeId;
int pointMetaTypeId;

void registerMetaTypes()
{
  bboxMetaTypeId  = CQUTIL_REGISTER_META_ID(CQChartsGeomBBox);
  pointMetaTypeId = CQUTIL_REGISTER_META_ID(CQChartsGeomPoint);

  CQPropertyViewMgrInst->setUserName("CQChartsGeom::BBox" , "geom_bbox");
  CQPropertyViewMgrInst->setUserName("CQChartsGeom::Point", "geom_point");
}

}
