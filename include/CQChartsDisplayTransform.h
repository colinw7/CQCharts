#ifndef CQChartsDisplayTransform_H
#define CQChartsDisplayTransform_H

#include <CQChartsGeom.h>
#include <CQChartsDisplayRange.h>

class CQChartsDisplayTransform {
 public:
  CQChartsDisplayTransform(const CQChartsDisplayRange *range) :
   range_(range) {
    update();
  }

  double getScale() const { return scale_; }

  const CQChartsGeom::Matrix &getMatrix() const { return matrix_; }

  const CQChartsGeom::Matrix &getIMatrix() const { return imatrix_; }

  void zoom(double factor) {
    scale_ *= factor;

    update();
  }

  void reset() {
    resetScale();
    resetAngle();
    resetPan();
  }

  void resetScale() {
    scale_ = 1.0;

    update();
  }

  void resetAngle() {
    angle_ = 0.0;

    update();
  }

  void resetPan() {
    tx_ = 0.0;
    ty_ = 0.0;

    update();
  }

  void zoomTo(const CQChartsGeom::BBox &bbox) {
    double w = bbox.getWidth ();
    double h = bbox.getHeight();

    CQChartsGeom::Point c = bbox.getCenter();

    double w1 = range_->getWindowWidth ();
    double h1 = range_->getWindowHeight();

    double xscale = w1/w;
    double yscale = h1/h;

    scale_ = std::min(xscale, yscale);

    CQChartsGeom::Point c1 = range_->getWindowCenter();

    tx_ = c1.x - c.x;
    ty_ = c1.y - c.y;

    update();
  }

  void zoomIn(double f=1.5) {
    scale_ *= f;

    update();
  }

  void zoomOut(double f=1.5) {
    scale_ /= f;

    update();
  }

  void zoomIn(const CQChartsGeom::Point &c, double f=1.5) {
    scale_ *= f;

    CQChartsGeom::Point c1 = range_->getWindowCenter();

    tx_ = c1.x - c.x;
    ty_ = c1.y - c.y;

    update();
  }

  void zoomOut(const CQChartsGeom::Point &c, double f=1.5) {
    scale_ /= f;

    CQChartsGeom::Point c1 = range_->getWindowCenter();

    tx_ = c1.x - c.x;
    ty_ = c1.y - c.y;

    update();
  }

  void panBy(double dx, double dy) {
    tx_ += dx/scale_;
    ty_ += dy/scale_;

    update();
  }

  void panTo(const CQChartsGeom::Point &c) {
    CQChartsGeom::Point c1 = range_->getWindowCenter();

    tx_ = c1.x - c.x;
    ty_ = c1.y - c.y;

    update();
  }

  void panLeft() {
    double w = range_->getWindowWidth()/scale_;

    panBy(-w/8, 0);
  }

  void panRight() {
    double w = range_->getWindowWidth()/scale_;

    panBy(w/8, 0);
  }

  void panDown() {
    double h = range_->getWindowHeight()/scale_;

    panBy(0, -h/8);
  }

  void panUp() {
    double h = range_->getWindowHeight()/scale_;

    panBy(0, h/8);
  }

  void spin(double da) {
    angle_ += da;

    update();
  }

 private:
  void update() {
    CQChartsGeom::Point c = range_->getWindowCenter();

    CQChartsGeom::Matrix m1, m2, m3, m4;

    m1.setTranslation(tx_*scale_, ty_*scale_);
    m2.setTranslation( c.x,  c.y);
    m3.setScale      (scale_);
//  m3.setRotation   (angle_);
    m4.setTranslation(-c.x, -c.y);

    matrix_ = m1*m2*m3*m4;

    if (! matrix_.invert(imatrix_))
      imatrix_.setIdentity();
  }

 private:
  const CQChartsDisplayRange* range_ { nullptr };
  double                      scale_ { 1.0 };
  double                      angle_ { 0.0 };
  double                      tx_    { 0.0 };
  double                      ty_    { 0.0 };
  CQChartsGeom::Matrix        matrix_;
  CQChartsGeom::Matrix        imatrix_;
};

#endif
