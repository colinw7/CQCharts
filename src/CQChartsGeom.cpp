#include <CQChartsGeom.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE_ID(CQChartsGeom::BBox   , CQChartsGeomBBox   , toString, fromString)
CQUTIL_DEF_META_TYPE_ID(CQChartsGeom::Point  , CQChartsGeomPoint  , toString, fromString)
CQUTIL_DEF_META_TYPE_ID(CQChartsGeom::Point3D, CQChartsGeomPoint3D, toString, fromString)

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
  BBox bbox;

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
  Point p;

  if (! CQChartsUtil::stringToPoint(s, p))
    return false;

  x = p.x;
  y = p.y;

  return true;
}

//---

QString
Point3D::
toString() const
{
  return CQChartsUtil::point3DToString(*this);
}

bool
Point3D::
fromString(const QString &s)
{
  Point3D p;

  if (! CQChartsUtil::stringToPoint3D(s, p))
    return false;

  x = p.x;
  y = p.y;
  z = p.z;

  return true;
}

//---

int bboxMetaTypeId;
int pointMetaTypeId;
int point3DMetaTypeId;

void registerMetaTypes()
{
  bboxMetaTypeId    = CQUTIL_REGISTER_META_ID(CQChartsGeomBBox);
  pointMetaTypeId   = CQUTIL_REGISTER_META_ID(CQChartsGeomPoint);
  point3DMetaTypeId = CQUTIL_REGISTER_META_ID(CQChartsGeomPoint3D);

  CQPropertyViewMgrInst->setUserName("CQChartsGeom::BBox"   , "geom_bbox");
  CQPropertyViewMgrInst->setUserName("CQChartsGeom::Point"  , "geom_point");
  CQPropertyViewMgrInst->setUserName("CQChartsGeom::Point3D", "geom_point3d");
}

}
