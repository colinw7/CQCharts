#ifndef CSVGUtil_H
#define CSVGUtil_H

#include <C3Bezier2D.h>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>

namespace CSVGUtil {

// convert path arc
void convertArcCoords(double x1, double y1, double x2, double y2, double phi,
                      double rx, double ry, int fa, int fs, bool unit_circle,
                      double *cx, double *cy, double *xr, double *yr,
                      double *theta, double *delta);

//---

using BezierList = std::vector<C3Bezier2D>;

void arcToBeziers(double x1, double y1, double x2, double y2, double phi,
                  double rx, double ry, int fa, int fs, bool unit_circle,
                  BezierList &beziers);

//---

class PathVisitor {
 public:
  PathVisitor() { }

  virtual ~PathVisitor() { }

  virtual void init() { }
  virtual void term() { }

  virtual void moveTo(double x, double y) = 0;

  virtual void rmoveTo(double dx, double dy) {
    moveTo(lastX() + dx, lastY() + dy);
  }

  virtual void lineTo(double x, double y) = 0;

  virtual void rlineTo(double dx, double dy) {
    lineTo(lastX() + dx, lastY() + dy);
  }

  virtual void hlineTo(double x) { lineTo(x, lastY()); }
  virtual void vlineTo(double y) { lineTo(lastX(), y); }

  virtual void rhlineTo(double dx) { rlineTo(dx, 0.0); }
  virtual void rvlineTo(double dy) { rlineTo(0.0, dy); }

  virtual void arcTo(double rx, double ry, double xa, int fa, int fs, double x2, double y2) = 0;

  virtual void rarcTo(double rx, double ry, double xa, int fa, int fs, double dx2, double dy2) {
    arcTo(rx, ry, xa, fa, fs, lastX() + dx2, lastY() + dy2);
  }

  virtual void bezier2To(double x1, double y1, double x2, double y2) = 0;

  virtual void rbezier2To(double dx1, double dy1, double dx2, double dy2) {
    double x1 = lastX() + dx1;
    double y1 = lastY() + dy1;

    double x2 = lastX() + dx2;
    double y2 = lastY() + dy2;

    bezier2To(x1, y1, x2, y2);

    setLastControlPoint(x2, y2);
  }

  virtual void bezier3To(double x1, double y1, double x2, double y2, double x3, double y3) = 0;

  virtual void rbezier3To(double dx1, double dy1, double dx2, double dy2, double dx3, double dy3) {
    double x1 = lastX() + dx1;
    double y1 = lastY() + dy1;

    double x2 = lastX() + dx2;
    double y2 = lastY() + dy2;

    double x3 = lastX() + dx3;
    double y3 = lastY() + dy3;

    bezier3To(x1, y1, x2, y2, x3, y3);

    setLastControlPoint(x2, y2);
  }

  virtual void mbezier2To(double x2, double y2) {
    // control point is mirror of last point
    double dx = lastX() - lastControlX();
    double dy = lastY() - lastControlY();

    double x1 = lastX() + dx;
    double y1 = lastY() + dy;

    bezier2To(x1, y1, x2, y2);

    setLastControlPoint(x1, y1);
  }

  virtual void mrbezier2To(double dx2, double dy2) {
    // control point is mirror of last point
    double dx1 = lastX() - lastControlX();
    double dy1 = lastY() - lastControlY();

    double x1 = lastX() + dx1;
    double y1 = lastY() + dy1;

    double x2 = lastX() + dx2;
    double y2 = lastY() + dy2;

    bezier2To(x1, y1, x2, y2);

    setLastControlPoint(x1, y1);
  }

  virtual void mbezier3To(double x2, double y2, double x3, double y3) {
    // control point is mirror of last point
    double dx = lastX() - lastControlX();
    double dy = lastY() - lastControlY();

    double x1 = lastX() + dx;
    double y1 = lastY() + dy;

    bezier3To(x1, y1, x2, y2, x3, y3);

    setLastControlPoint(x2, y2);
  }

  virtual void mrbezier3To(double dx2, double dy2, double dx3, double dy3) {
    // first control point is mirror of last control point in last point
    double dx1 = lastX() - lastControlX();
    double dy1 = lastY() - lastControlY();

    double x1 = lastX() + dx1;
    double y1 = lastY() + dy1;

    double x2 = lastX() + dx2;
    double y2 = lastY() + dy2;

    double x3 = lastX() + dx3;
    double y3 = lastY() + dy3;

    bezier3To(x1, y1, x2, y2, x3, y3);

    setLastControlPoint(x2, y2);
  }

  virtual void closePath(bool relative) = 0;

  //---

  virtual void handleError(const std::string & /*preStr*/, const std::string & /*atStr*/,
                           const std::string & /*postStr*/) const { }

  //---

  // last point

  double lastX() const { return lastX_; }
  double lastY() const { return lastY_; }

  void incLastPoint(double dx, double dy) {
    lastX_ += dx;
    lastY_ += dy;
  }

  void setLastPoint(double x, double y) {
    lastX_ = x;
    lastY_ = y;
  }

  //---

  // last control point

  double lastControlX() const { return lastControlX_; }
  double lastControlY() const { return lastControlY_; }

  void incLastControlPoint(double dx, double dy) {
    lastControlX_ += dx;
    lastControlY_ += dy;
  }

  void setLastControlPoint(double x, double y) {
    lastControlX_ = x;
    lastControlY_ = y;
  }

 protected:
  double lastX_ { 0 };
  double lastY_ { 0 };

  double lastControlX_ { 0 };
  double lastControlY_ { 0 };
};

//---

bool visitPath(const std::string &data, PathVisitor &visitor);

//---

}

#endif
