#ifndef CQChartsCamera_H
#define CQChartsCamera_H

#include <CQChartsGeom.h>
#include <CCoordFrame3D.h>
#include <CMatrix3DH.h>
#include <CPoint3D.h>
#include <memory>

class CQChartsPlot3D;

//---

class CQChartsXYPlane {
 public:
  CQChartsXYPlane(double z=0.5, bool relative=true) :
   z_(z), relative_(relative) {
  }

  void reset() { z_ = 0.5; relative_ = true; }

  double z() const { return z_; }
  void setZ(double z) { z_ = z; }

  bool isRelative() const { return relative_; }
  void setRelative(bool b) { relative_ = b; }

 private:
  double z_        { 0.5  };
  bool   relative_ { true };
};

//---

class CQChartsCamera {
 public:
  using Point3D = CQChartsGeom::Point3D;
  using Range3D = CQChartsGeom::Range3D;

 public:
  CQChartsCamera(CQChartsPlot3D *plot=nullptr);

  virtual ~CQChartsCamera();

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b) { enabled_ = b; }

  void init();

  void init(const CQChartsCamera &camera) {
    CQChartsPlot3D *plot;

    std::swap(plot, plot_);

    *this = camera;

    std::swap(plot, plot_);
  }

  void reset();

  void setMap();

  //---

  double xmin() const { return xmin_; }
  void setXMin(double x) { xmin_ = x; init(); }

  double xmax() const { return xmax_; }
  void setXMax(double x) { xmax_ = x; init(); }

  double ymin() const { return ymin_; }
  void setYMin(double y) { ymin_ = y; init(); }

  double ymax() const { return ymax_; }
  void setYMax(double y) { ymax_ = y; init(); }

  double near() const { return near_; }
  void setNear(double z) { near_ = z; init(); }

  double far() const { return far_ ; }
  void setFar (double z) { far_  = z; init(); }

  //---

  double rotateX() const { return rotateX_; }
  void setRotateX(double a) { rotateX_ = a; init(); }

  double rotateY() const { return rotateY_; }
  void setRotateY(double a) { rotateY_ = a; init(); }

  double rotateZ() const { return rotateZ_; }
  void setRotateZ(double a) { rotateZ_ = a; init(); }

  //---

  double scaleX() const { return scaleX_; }
  void setScaleX(double r) { scaleX_ = r; }

  double scaleY() const { return scaleY_; }
  void setScaleY(double r) { scaleY_ = r; }

  double scaleZ() const { return scaleZ_; }
  void setScaleZ(double r) { scaleZ_ = r; }

  //---

  void resetZoom() { scaleX_ = 0.707; scaleY_ = 0.707; scaleZ_ = 0.707; }

  void zoomIn (double f=1.1) { scaleX_ *= f; scaleY_ *= f; scaleZ_ *= f; }
  void zoomOut(double f=1.1) { scaleX_ /= f; scaleY_ /= f; scaleZ_ /= f; }

  void setPosition(const Point3D &position) {
    coordFrame_.setOrigin(CPoint3D(position.x, position.y, position.z));
  }

  void setDirection(const Point3D &dir);

  void moveDX(double dx) { coordFrame_.moveX(dx); }
  void moveDY(double dy) { coordFrame_.moveY(dy); }
  void moveDZ(double dz) { coordFrame_.moveZ(dz); }

  void rotateDX(double dx) { coordFrame_.rotateAboutX(dx); }
  void rotateDY(double dy) { coordFrame_.rotateAboutY(dy); }
  void rotateDZ(double dz) { coordFrame_.rotateAboutZ(dz); }

//double fieldOfView() const { return fov_; }
//void setFieldOfView(double r) { fov_ = r; }

  const CQChartsXYPlane &xyPlane() const { return xyPlane_; }
  void setXYPlane(const CQChartsXYPlane &v) { xyPlane_ = v; }

  double planeZ() const { return xyPlane_.z(); }
  void setPlaneZ(double z) { xyPlane_.setZ(z); }

  bool isPlaneRelative() const { return xyPlane_.isRelative(); }
  void setPlaneRelative(bool b) { xyPlane_.setRelative(b); }

  void planeZRange(double &zmin, double &zmax) const;

  Point3D transform  (const Point3D &p) const;
  Point3D untransform(const Point3D &p) const;

  void showView(std::ostream &os) const;

  void unsetView();

 private:
  CQChartsPlot3D*    plot_       { nullptr };
  bool               enabled_    { true };
  double             xmin_       { -1 };
  double             xmax_       {  1 };
  double             ymin_       { -1 };
  double             ymax_       {  1 };
  double             near_       { 0.1 };
  double             far_        { 100 };
  double             rotateX_    { 60.0 };
  double             rotateY_    {  0.0 };
  double             rotateZ_    { 30.0 };
  double             scaleX_     { 0.707 };
  double             scaleY_     { 0.707 };
  double             scaleZ_     { 0.707 };
  CCoordFrame3D      coordFrame_;
  Point3D            direction_  { 0, 0, 1 };
//double             fov_        { 90 };
  CQChartsXYPlane    xyPlane_;
  mutable CMatrix3DH projMatrix_;
  mutable CMatrix3DH iprojMatrix_;
};

typedef std::shared_ptr<CQChartsCamera> CQChartsCameraP;

#endif
