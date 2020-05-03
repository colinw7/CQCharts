#include <CQChartsCamera.h>
#include <CQChartsPlot3D.h>

CQChartsCamera::
CQChartsCamera(CQChartsPlot3D *plot) :
 plot_(plot)
{
  init();
}

CQChartsCamera::
~CQChartsCamera()
{
}

void
CQChartsCamera::
init()
{
  coordFrame_.init();

  setDirection(CQChartsGeom::Point3D(0, 0, 1));

//projMatrix_.buildPerspective(fov, 1.0, near, far);
//projMatrix_.buildOrtho(xmin_, xmax_, ymin_, ymax_, near_, far_);
//projMatrix_.buildFrustrum(-2, 2, -2, 2, near, far);

  const CQChartsGeom::Range3D &range3D = plot_->range3D();

  if (range3D.isSet()) {
    double xmin = range3D.xmin();
    double xmax = range3D.xmax();
    double ymin = range3D.ymin();
    double ymax = range3D.ymax();

    projMatrix_.buildOrtho(xmin, xmax, ymin, ymax, near_, far_);
  }
  else
    projMatrix_.buildOrtho(xmin_, xmax_, ymin_, ymax_, near_, far_);

  projMatrix_.invert(iprojMatrix_);

  //---

  rotateDX(CMathUtil::Deg2Rad(rotateX_));
  rotateDY(CMathUtil::Deg2Rad(rotateY_));
  rotateDZ(CMathUtil::Deg2Rad(rotateZ_));

#if 0
  if (plot_)
    plot_->cameraChanged();
#endif
}

void
CQChartsCamera::
reset()
{
  enabled_     = false;
  xmin_        = -1;
  ymin_        = -1;
  xmax_        = 1;
  ymax_        = 1;
  near_        = 0.1;
  far_         = 100;
  rotateX_     = 60.0;
  rotateY_     = 0.0;
  rotateZ_     = 30.0;
  scaleX_      = 1.0;
  scaleY_      = 1.0;
  scaleZ_      = 1.0;
  coordFrame_  = CCoordFrame3D();
  direction_   = CQChartsGeom::Point3D(0, 0, 1);
//fov_         = 90;
  projMatrix_  = CMatrix3DH();
  iprojMatrix_ = CMatrix3DH();
}

void
CQChartsCamera::
setMap()
{
  enabled_ = false;

  rotateX_ = 0.0;
  rotateY_ = 0.0;
  rotateZ_ = 0.0;

  init();
}

void
CQChartsCamera::
setDirection(const CQChartsGeom::Point3D &dir)
{
  CVector3D right, up, dir1;

  coordFrame_.getBasis(right, up, dir1);

  CVector3D vdir(dir.x, dir.y, dir.z);

  dir1 = vdir.unit();

  right = up  .crossProduct(dir1);
  up    = dir1.crossProduct(right);

  if (COrthonormalBasis3D::validate(right, up, dir1)) {
    coordFrame_.setBasis(right, up, dir1);

    direction_ = dir;
  }
}

CQChartsGeom::Point3D
CQChartsCamera::
transform(const CQChartsGeom::Point3D &p) const
{
  if (! enabled_) return p;

  //---

  // map to unit radius cube centered at 0,0
  const CQChartsGeom::Range3D &range3D = plot_->range3D();

  if (! range3D.isSet())
    return p;

  double xmin = range3D.xmin();
  double xmax = range3D.xmax();
  double ymin = range3D.ymin();
  double ymax = range3D.ymax();

  //---

  double zmin = 0.0, zmax = 1.0;

#if 0
  planeZRange(zmin, zmax);
#else
  zmin = range3D.zmin();
  zmax = range3D.zmax();
#endif

  //---

  double x1 = CMathUtil::map(p.x, xmin, xmax, -scaleX_, scaleX_);
  double y1 = CMathUtil::map(p.y, ymin, ymax, -scaleY_, scaleY_);
  double z1 = CMathUtil::map(p.z, zmin, zmax, -scaleZ_, scaleZ_);

  // transform to 2D
  CPoint3D p1(x1, y1, z1);

  CPoint3D p2 = coordFrame_.transformTo(p1);

  CPoint3D p3;

  projMatrix_.multiplyPoint(p2, p3);

  // remap back to x, y, z range
  double x3 = CMathUtil::map(p3.x, -1, 1, xmin, xmax);
  double y3 = CMathUtil::map(p3.y, -1, 1, ymin, ymax);
  double z3 = CMathUtil::map(p3.z, -1, 1, zmin, zmax);

  return CQChartsGeom::Point3D(x3, y3, z3);
}

CQChartsGeom::Point3D
CQChartsCamera::
untransform(const CQChartsGeom::Point3D &p) const
{
  if (! enabled_) return p;

  // map to unit radius cube centered at 0,0
  const CQChartsGeom::Range3D &range3D = plot_->range3D();

  if (! range3D.isSet())
    return p;

  double xmin = range3D.xmin();
  double xmax = range3D.xmax();
  double ymin = range3D.ymin();
  double ymax = range3D.ymax();

  //---

  double zmin = 0.0, zmax = 1.0;

#if 0
  planeZRange(zmin, zmax);
#else
  zmin = range3D.zmin();
  zmax = range3D.zmax();
#endif

  //---

  double x3 = CMathUtil::map(p.x, xmin, xmax, -1, 1);
  double y3 = CMathUtil::map(p.y, ymin, ymax, -1, 1);
  double z3 = CMathUtil::map(p.z, zmin, zmax, -1, 1);

  CPoint3D p3(x3, y3, z3);

  CPoint3D p2;

  iprojMatrix_.multiplyPoint(p3, p2);

  CPoint3D p1 = coordFrame_.transformFrom(p2);

  double x = CMathUtil::map(p1.x, -scaleX_, scaleX_, xmin, xmax);
  double y = CMathUtil::map(p1.y, -scaleY_, scaleY_, ymin, ymax);
  double z = CMathUtil::map(p1.z, -scaleZ_, scaleZ_, zmin, zmax);

  return CQChartsGeom::Point3D(x, y, z);
}

void
CQChartsCamera::
planeZRange(double &zmin, double &zmax) const
{
  const CQChartsGeom::Range3D &range3D = plot_->range3D();

  zmin = range3D.zmin();
  zmax = range3D.zmax();

  if (xyPlane_.isRelative()) {
    zmin -= xyPlane_.z()*(zmax - zmin);
  }
}

void
CQChartsCamera::
showView(std::ostream &os) const
{
  os << "view is " << rotateX_ << ", " << rotateZ_ << " rot_z";
  os << ", " << scaleX_ << ", " << scaleZ_ << " scale_z" << std::endl;
}

void
CQChartsCamera::
unsetView()
{
  enabled_ = true;

  rotateX_ = 60.0;
  rotateZ_ = 30.0;
  scaleX_  = 1.0;
  scaleZ_  = 1.0;

  init();
}
