#include <CSVGUtil.h>
#include <CStrParse.h>
#include <CMathGen.h>
#include <CArcToBezier.h>

bool
CSVGUtil::
visitPath(const std::string &data, PathVisitor &visitor)
{
  CStrParse parse(data);

  //---

  auto parseFlag = [&](int *i) {
    if (! parse.isChar('0') && ! parse.isChar('1'))
      return false;

    *i = (parse.getCharAt() - '0');

    parse.skipChar();

    return true;
  };

  auto skipCommaSpace = [&]() {
    parse.skipSpace();

    while (parse.isChar(',')) {
      parse.skipChar();

      parse.skipSpace();
    }
  };

  //---

  visitor.init();

  //---

  char c = '\0', lastC = '\0';

  bool rereadCmd = false;

  parse.skipSpace();

  bool flag = true;

  while (! parse.eof()) {
    if (! rereadCmd)
      parse.readChar(&c);
    else
      rereadCmd = false;

    // relative move to
    if      (c == 'm') {
      lastC = c;

      double x, y;

      parse.skipSpace();

      if (! parse.readReal(&x)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y)) { flag = false; break; }

      skipCommaSpace();

      visitor.rmoveTo(x, y);

      visitor.incLastPoint(x, y);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parse.readReal(&x)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y)) { flag = false; break; }

        skipCommaSpace();

        visitor.rlineTo(x, y);

        visitor.incLastPoint(x, y);
      }
    }
    // absolute move to
    else if (c == 'M') {
      lastC = c;

      double x, y;

      parse.skipSpace();

      if (! parse.readReal(&x)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y)) { flag = false; break; }

      skipCommaSpace();

      visitor.moveTo(x, y);

      visitor.setLastPoint(x, y);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parse.readReal(&x)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y)) { flag = false; break; }

        skipCommaSpace();

        visitor.lineTo(x, y);

        visitor.setLastPoint(x, y);
      }
    }
    // relative line to
    else if (c == 'l') {
      lastC = c;

      double x, y;

      parse.skipSpace();

      if (! parse.readReal(&x)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y)) { flag = false; break; }

      skipCommaSpace();

      visitor.rlineTo(x, y);

      visitor.incLastPoint(x, y);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parse.readReal(&x)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y)) { flag = false; break; }

        skipCommaSpace();

        visitor.rlineTo(x, y);

        visitor.incLastPoint(x, y);
      }
    }
    // absolute line to
    else if (c == 'L') {
      lastC = c;

      double x, y;

      parse.skipSpace();

      if (! parse.readReal(&x)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y)) { flag = false; break; }

      skipCommaSpace();

      visitor.lineTo(x, y);

      visitor.setLastPoint(x, y);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parse.readReal(&x)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y)) { flag = false; break; }

        skipCommaSpace();

        visitor.lineTo(x, y);

        visitor.setLastPoint(x, y);
      }
    }
    // relative hline to
    else if (c == 'h') {
      lastC = c;

      double d;

      parse.skipSpace();

      if (! parse.readReal(&d)) { flag = false; break; }

      visitor.rhlineTo(d);

      visitor.incLastPoint(d, 0);
    }
    // absolute hline to
    else if (c == 'H') {
      lastC = c;

      double d;

      parse.skipSpace();

      if (! parse.readReal(&d)) { flag = false; break; }

      visitor.hlineTo(d);

      visitor.setLastPoint(d, visitor.lastY());
    }
    // relative vline to
    else if (c == 'v') {
      lastC = c;

      double d;

      parse.skipSpace();

      if (! parse.readReal(&d)) { flag = false; break; }

      visitor.rvlineTo(d);

      visitor.incLastPoint(0, d);
    }
    // absolute vline to
    else if (c == 'V') {
      lastC = c;

      double d;

      parse.skipSpace();

      if (! parse.readReal(&d)) { flag = false; break; }

      visitor.vlineTo(d);

      visitor.setLastPoint(visitor.lastX(), d);
    }
    // relative arc to
    else if (c == 'a') {
      lastC = c;

      parse.skipSpace();

      // read (rx, ry)
      double rx, ry;

      if (! parse.readReal(&rx)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&ry)) { flag = false; break; }

      skipCommaSpace();

      // read xa
      double xa;

      if (! parse.readReal(&xa)) { flag = false; break; }

      skipCommaSpace();

      // read fa, fs (single digit integers)
      int fa, fs;
      if (! parseFlag(&fa)) { flag = false; break; }

      skipCommaSpace();

      if (! parseFlag(&fs)) { flag = false; break; }

      skipCommaSpace();

      // read (x2, y2)
      double x2, y2;

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      // add arc
      visitor.rarcTo(rx, ry, xa, fa, fs, x2, y2);

      visitor.incLastPoint(x2, y2);

      skipCommaSpace();

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        // read (rx, ry)
        if (! parse.readReal(&rx)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&ry)) { flag = false; break; }

        skipCommaSpace();

        // read xa
        if (! parse.readReal(&xa)) { flag = false; break; }

        skipCommaSpace();

        // read fa, fs (single digit integers)
        int fa1, fs1;

        if (! parseFlag(&fa1)) { flag = false; break; }

        skipCommaSpace();

        if (! parseFlag(&fs1)) { flag = false; break; }

        skipCommaSpace();

        // read (x2, y2)
        if (! parse.readReal(&x2)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y2)) { flag = false; break; }

        // add arc
        visitor.rarcTo(rx, ry, xa, fa1, fs1, x2, y2);

        visitor.incLastPoint(x2, y2);

        parse.skipSpace();
      }
    }
    // absolute arc to
    else if (c == 'A') {
      lastC = c;

      int    fa, fs;
      double rx, ry, xa, x2, y2;

      parse.skipSpace();

      if (! parse.readReal(&rx)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&ry)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&xa)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readInteger(&fa)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readInteger(&fs)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      visitor.arcTo(rx, ry, xa, fa, fs, x2, y2);

      visitor.setLastPoint(x2, y2);
    }
    // relative quadratic bezier to
    else if (c == 'q') {
      lastC = c;

      double x1, y1, x2, y2;

      parse.skipSpace();

      if (! parse.readReal(&x1)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y1)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      skipCommaSpace();

      visitor.rbezier2To(x1, y1, x2, y2);

      //visitor.incLastControlPoint(x1, y1);

      visitor.incLastPoint(x2, y2);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parse.readReal(&x1)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y1)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&x2)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y2)) { flag = false; break; }

        skipCommaSpace();

        visitor.rbezier2To(x1, y1, x2, y2);

        //visitor.incLastControlPoint(x1, y1);

        visitor.incLastPoint(x2, y2);
      }
    }
    // absolute quadratic bezier to
    else if (c == 'Q') {
      lastC = c;

      double x1, y1, x2, y2;

      parse.skipSpace();

      if (! parse.readReal(&x1)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y1)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      skipCommaSpace();

      visitor.bezier2To(x1, y1, x2, y2);

      visitor.setLastControlPoint(x1, y1);

      visitor.setLastPoint(x2, y2);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parse.readReal(&x1)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y1)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&x2)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y2)) { flag = false; break; }

        skipCommaSpace();

        visitor.bezier2To(x1, y1, x2, y2);

        visitor.setLastControlPoint(x1, y1);

        visitor.setLastPoint(x2, y2);
      }
    }
    // relative quadratic bezier to (with mirrored control point)
    else if (c == 't') {
      lastC = c;

      double x2, y2;

      parse.skipSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      visitor.mrbezier2To(x2, y2);

      visitor.incLastPoint(x2, y2);
    }
    // absolute quadratic bezier to (with mirrored control point)
    else if (c == 'T') {
      lastC = c;

      double x2, y2;

      parse.skipSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      visitor.mbezier2To(x2, y2);

    //visitor.setLastControlPoint(x1, y1);

      visitor.setLastPoint(x2, y2);
    }
    // relative cubic bezier to
    else if (c == 'c') {
      lastC = c;

      double x1, y1, x2, y2, x3, y3;

      parse.skipSpace();

      if (! parse.readReal(&x1)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y1)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x3)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y3)) { flag = false; break; }

      skipCommaSpace();

      visitor.rbezier3To(x1, y1, x2, y2, x3, y3);

      //visitor.incLastControlPoint(x2, y2);

      visitor.incLastPoint(x3, y3);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parse.readReal(&x1)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y1)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&x2)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y2)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&x3)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y3)) { flag = false; break; }

        skipCommaSpace();

        visitor.rbezier3To(x1, y1, x2, y2, x3, y3);

        //visitor.incLastControlPoint(x2, y2);

        visitor.incLastPoint(x3, y3);
      }
    }
    // absolute cubic bezier to
    else if (c == 'C') {
      lastC = c;

      double x1, y1, x2, y2, x3, y3;

      parse.skipSpace();

      if (! parse.readReal(&x1)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y1)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x3)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y3)) { flag = false; break; }

      skipCommaSpace();

      visitor.bezier3To(x1, y1, x2, y2, x3, y3);

      visitor.setLastControlPoint(x2, y2);

      visitor.setLastPoint(x3, y3);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parse.readReal(&x1)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y1)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&x2)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y2)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&x3)) { flag = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y3)) { flag = false; break; }

        skipCommaSpace();

        visitor.bezier3To(x1, y1, x2, y2, x3, y3);

        visitor.setLastControlPoint(x2, y2);

        visitor.setLastPoint(x3, y3);
      }
    }
    // relative cubic bezier to (with mirrored control point)
    else if (c == 's') {
      lastC = c;

      double x2, y2, x3, y3;

      parse.skipSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x3)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y3)) { flag = false; break; }

      visitor.mrbezier3To(x2, y2, x3, y3);

    //visitor.incLastControlPoint(x2, y2);

      visitor.incLastPoint(x3, y3);
    }
    // absolute cubic bezier to (with mirrored control point)
    else if (c == 'S') {
      lastC = c;

      double x2, y2, x3, y3;

      parse.skipSpace();

      if (! parse.readReal(&x2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x3)) { flag = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y3)) { flag = false; break; }

      visitor.mbezier3To(x2, y2, x3, y3);

    //visitor.setLastControlPoint(x2, y2);

      visitor.setLastPoint(x3, y3);
    }
    // relative close path
    else if (c == 'z') {
      lastC = c;

      visitor.closePath(/*relative*/true);
    }
    // absolute close path
    else if (c == 'Z') {
      lastC = c;

      visitor.closePath(/*relative*/false);
    }
    else {
      // repeat last command if any
      bool isNumber = false;

      if (c == '-' || c == '+' || c == '.') {
        if (isdigit(parse.getCharAt()))
          isNumber = true;
      }
      else if (isdigit(c)) {
        isNumber = true;
      }

      if (isNumber && lastC != '\0') {
        parse.unreadChar();

        c = lastC;

        rereadCmd = true;

        continue;
      }

      flag = false;

      break;
    }

    skipCommaSpace();
  }

  //---

  if (! flag)
    visitor.handleError(parse.getBefore(), parse.getAt(), parse.getAfter());

  //---

  visitor.term();

  //---

  return flag;
}

// See: https://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes
void
CSVGUtil::
convertArcCoords(double x1, double y1, double x2, double y2, double phi,
                 double rx, double ry, int fa, int fs, bool unit_circle,
                 double *cx, double *cy, double *rx1, double *ry1,
                 double *theta, double *delta)
{
  // start and end are the same so it's a complete ellipse
  if (fabs(x2 - x1) < 1E-6 && fabs(y2 - y1) < 1E-6) {
    if (fs == 1) {
      *cx = x1 + rx;
      *cy = y1;
    }
    else {
      *cx = x1 - rx;
      *cy = y1;
    }

    *rx1 = rx;
    *ry1 = ry;

    *theta = 0;
    *delta = 360;

    return;
  }

  // TODO: if rx == 0 || ry == 0 then straight line

  rx = fabs(rx);
  ry = fabs(ry);

  auto phi1 = CMathGen::DegToRad(phi);

  double sin_phi = std::sin(phi1);
  double cos_phi = std::cos(phi1);

  // step 1 : compute x1', y1'
  double dx = (x1 - x2)/2.0;
  double dy = (y1 - y2)/2.0;

  double dx1 =  cos_phi*dx + sin_phi*dy;
  double dy1 = -sin_phi*dx + cos_phi*dy;

  // step 2 compute cx', cy'
  double rxx = rx*rx;
  double ryy = ry*ry;

  double dxx1 = dx1*dx1;
  double dyy1 = dy1*dy1;

  // Fix radii
  double rcheck = dxx1/rxx + dyy1/ryy;

  if (rcheck > 1) {
    double s = sqrt(rcheck);

    rx *= s;
    ry *= s;

    rxx = rx*rx;
    ryy = ry*ry;
  }

  //---

  if (unit_circle) {
    double a00 =  cos_phi/rx;
    double a01 =  sin_phi/rx;
    double a10 = -sin_phi/ry;
    double a11 =  cos_phi/ry;

    /* (xt1, yt1) is current point in transformed coordinate space.
       (xt2, yt2) is new point in transformed coordinate space.

       The arc fits a unit-radius circle in this space.
    */
    double xt1 = a00*x1 + a01*y1;
    double yt1 = a10*x1 + a11*y1;
    double xt2 = a00*x2 + a01*y2;
    double yt2 = a10*x2 + a11*y2;

    double d = (xt2 - xt1)*(xt2 - xt1) + (yt2 - yt1)*(yt2 - yt1);

    double sfactor_sq = 1.0/d - 0.25;

    if (sfactor_sq < 0) sfactor_sq = 0;

    double sfactor = sqrt(sfactor_sq);

    if (fa == fs) sfactor = -sfactor;

    // (cx, cy) is center of the circle.
    *cx = 0.5*(xt1 + xt2) - sfactor*(yt2 - yt1);
    *cy = 0.5*(yt1 + yt2) + sfactor*(xt2 - xt1);

    // Calculate angles
    *theta = atan2(yt1 - *cy, xt1 - *cx);

    double theta1 = atan2(yt2 - *cy, xt2 - *cx);

    *delta = theta1 - *theta;

    if      (fs == 0 && *delta > 0)
      *delta -= 2*M_PI;
    else if (fs == 1 && *delta < 0)
      *delta += 2*M_PI;

    *theta = CMathGen::RadToDeg(*theta);
    *delta = CMathGen::RadToDeg(*delta);
  }
  else {
    // Calculate center of arc
    double sfactor_sq = (rxx*ryy - rxx*dyy1 - ryy*dxx1)/(rxx*dyy1 + ryy*dxx1);

    if (sfactor_sq < 0) sfactor_sq = 0;

    double sfactor = sqrt(sfactor_sq);

    if (fa == fs) sfactor = -sfactor;

    double cx1 =  sfactor*((rx*dy1)/ry);
    double cy1 = -sfactor*((ry*dx1)/rx);

    // step 3 - compute cx, cy from cx', cy'
    double sx2 = (x1 + x2)/2.0;
    double sy2 = (y1 + y2)/2.0;

    *cx = sx2 + cos_phi*cx1 - sin_phi*cy1;
    *cy = sy2 + sin_phi*cx1 + cos_phi*cy1;

    // step 4 - Calculate arc angles (a and da)
    double ux = ( dx1 - cx1)/rx;
    double uy = ( dy1 - cy1)/ry;

    double vx = (-dx1 - cx1)/rx;
    double vy = (-dy1 - cy1)/ry;

    double mod_u = sqrt(ux*ux + uy*uy);
    double mod_v = ux;

    int sign = (uy < 0) ? -1 : 1;

    *theta = sign*std::acos(mod_v/mod_u);
    *theta = CMathGen::RadToDeg(*theta);

    while (*theta >=  360) *theta -= 360;
    while (*theta <= -360) *theta += 360;

    mod_u = sqrt((ux*ux + uy*uy) * (vx*vx + vy*vy));
    mod_v = ux*vx + uy*vy;

    sign = ((ux*vy - uy*vx) < 0) ? -1 : 1;

    *delta = sign*std::acos(mod_v/mod_u);
    *delta = CMathGen::RadToDeg(*delta);

    if      (fs == 0 && *delta > 0)
      *delta -= 360;
    else if (fs == 1 && *delta < 0)
      *delta += 360;

    while (*delta >=  360) *delta -= 360;
    while (*delta <= -360) *delta += 360;
  }

  *rx1 = rx;
  *ry1 = ry;
}

void
CSVGUtil::
arcToBeziers(double x1, double y1, double x2, double y2, double phi,
             double rx, double ry, int fa, int fs, bool unit_circle,
             BezierList &beziers)
{
  double cx, cy, rx1, ry1, theta, delta;

  convertArcCoords(x1, y1, x2, y2, phi, rx, ry, fa, fs, unit_circle,
                   &cx, &cy, &rx1, &ry1, &theta, &delta);

  auto rotatePoint = [](const CPoint2D &p, const CPoint2D &center, double a) {
    double s = std::sin(a);
    double c = std::cos(a);

    double xx1 = p.x - center.x;
    double yy1 = p.y - center.y;

    double xx2 = xx1*c - yy1*s;
    double yy2 = xx1*s + yy1*c;

    return CPoint2D(xx2 + center.x, yy2 + center.y);
  };

  auto rotateBezier = [&](const C3Bezier2D &bezier, const CPoint2D &c, double a) {
    return C3Bezier2D(rotatePoint(bezier.getFirstPoint   (), c, a),
                      rotatePoint(bezier.getControlPoint1(), c, a),
                      rotatePoint(bezier.getControlPoint2(), c, a),
                      rotatePoint(bezier.getLastPoint    (), c, a));
  };

  auto phi1 = CMathGen::DegToRad(phi);

  auto p1 = rotatePoint(CPoint2D(x1, y1), CPoint2D(cx, cy), -phi1);
  auto p2 = rotatePoint(CPoint2D(x2, y2), CPoint2D(cx, cy), -phi1);

  convertArcCoords(p1.x, p1.y, p2.x, p2.y, 0.0, rx, ry, fa, fs, unit_circle,
                   &cx, &cy, &rx1, &ry1, &theta, &delta);

  double a1 = CMathGen::DegToRad(theta);
  double a2 = CMathGen::DegToRad(theta + delta);

  BezierList beziers1;

  CArcToBezier::ArcToBeziers(cx, cy, rx1, ry1, a1, a2, beziers1);

  for (const auto &bezier1 : beziers1)
    beziers.push_back(rotateBezier(bezier1, CPoint2D(cx, cy), phi1));
}
