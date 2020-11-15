#include <CQChartsPath.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>
#include <CMathGen.h>

#include <cmath>

namespace {

void convertArcCoords(double x1, double y1, double x2, double y2, double phi,
                      double rx, double ry, int fa, int fs, bool unit_circle,
                      double *cx, double *cy, double *rx1, double *ry1,
                      double *theta, double *delta) {
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

  rx = fabs(rx);
  ry = fabs(ry);

  phi = CMathGen::DegToRad(phi);

  double sin_phi = sin(phi);
  double cos_phi = cos(phi);

  double dx = (x1 - x2)/2.0;
  double dy = (y1 - y2)/2.0;

  double dx1 =  cos_phi*dx + sin_phi*dy;
  double dy1 = -sin_phi*dx + cos_phi*dy;

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

    double sfactor_sq = 1.0 / d - 0.25;

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

    double sx2 = (x1 + x2)/2.0;
    double sy2 = (y1 + y2)/2.0;

    *cx = sx2 + cos_phi*cx1 - sin_phi*cy1;
    *cy = sy2 + sin_phi*cx1 + cos_phi*cy1;

    // Calculate arc angles
    double ux = ( dx1 - cx1)/rx;
    double uy = ( dy1 - cy1)/ry;

    double vx = (-dx1 - cx1)/rx;
    double vy = (-dy1 - cy1)/ry;

    double mod_u = sqrt(ux*ux + uy*uy);
    double mod_v = ux;

    int sign = (uy < 0) ? -1 : 1;

    *theta = sign*acos(mod_v/mod_u);
    *theta = CMathGen::RadToDeg(*theta);

    while (*theta >=  360) *theta -= 360;
    while (*theta <= -360) *theta += 360;

    mod_u = sqrt((ux*ux + uy*uy) * (vx*vx + vy*vy));
    mod_v = ux*vx + uy*vy;

    sign = ((ux*vy - uy*vx) < 0) ? -1 : 1;

    *delta = sign*acos(mod_v/mod_u);
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

}

//---

CQUTIL_DEF_META_TYPE(CQChartsPath, toString, fromString)

int CQChartsPath::metaTypeId;

void
CQChartsPath::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPath);

  CQPropertyViewMgrInst->setUserName("CQChartsPath", "path");
}

QString
CQChartsPath::
toString() const
{
  if (! path_)
    return "";

  QString str;

  int n = path_->elementCount();
  if (n == 0) return "";

  for (int i = 0; i < n; ++i) {
    const auto &e = path_->elementAt(i);

    if (str.length())
      str += " ";

    if      (e.isMoveTo()) {
      str += QString("M %1 %2").arg(e.x).arg(e.y);
    }
    else if (e.isLineTo()) {
      str += QString("L %1 %2").arg(e.x).arg(e.y);
    }
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (i < n - 1) {
        e1  = path_->elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path_->elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        ++i;

        if (e2t == QPainterPath::CurveToDataElement) {
          ++i;

          str += QString("C %1 %2 %3 %4 %5 %6").
            arg(e.x).arg(e.y).arg(e1.x).arg(e1.y).arg(e2.x).arg(e2.y);
        }
        else {
          str += QString("Q %1 %2 %3 %4").
            arg(e.x).arg(e.y).arg(e1.x).arg(e1.y);
        }
      }
    }
  }

  return str;
}

void
CQChartsPath::
move(double dx, double dy)
{
  *path_ = movePath(*path_, dx, dy);
}

void
CQChartsPath::
flip(bool flipX, bool flipY)
{
  *path_ = flipPath(*path_, flipX, flipY);
}

QPainterPath
CQChartsPath::
movePath(const QPainterPath &path, double dx, double dy) const
{
  QPainterPath ppath;

  auto bbox = path.boundingRect();

  int n = path.elementCount();

  auto moveElement = [&](const QPainterPath::Element &e) {
    return QPointF(e.x + dx, e.y + dy);
  };

  for (int i = 0; i < n; ++i) {
    const auto &e = path.elementAt(i);

    if      (e.isMoveTo()) {
      ppath.moveTo(moveElement(e));
    }
    else if (e.isLineTo()) {
      ppath.lineTo(moveElement(e));
    }
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (i < n - 1) {
        e1  = path.elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path.elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        ++i;

        if (e2t == QPainterPath::CurveToDataElement) {
          ++i;

          ppath.cubicTo(moveElement(e), moveElement(e1), moveElement(e2));
        }
        else {
          ppath.quadTo(moveElement(e), moveElement(e1));
        }
      }
    }
    else {
      assert(false);
    }
  }

  return ppath;
}

QPainterPath
CQChartsPath::
flipPath(const QPainterPath &path, bool flipX, bool flipY) const
{
  QPainterPath ppath;

  auto bbox = path.boundingRect();

  auto center = bbox.center();

  int n = path.elementCount();

  auto flipElement = [&](const QPainterPath::Element &e) {
    return QPointF((flipX ? 2*center.x() - e.x : e.x), (flipY ? 2*center.y() - e.y : e.y));
  };

  for (int i = 0; i < n; ++i) {
    const auto &e = path.elementAt(i);

    if      (e.isMoveTo()) {
      ppath.moveTo(flipElement(e));
    }
    else if (e.isLineTo()) {
      ppath.lineTo(flipElement(e));
    }
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (i < n - 1) {
        e1  = path.elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path.elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        ++i;

        if (e2t == QPainterPath::CurveToDataElement) {
          ++i;

          ppath.cubicTo(flipElement(e), flipElement(e1), flipElement(e2));
        }
        else {
          ppath.quadTo(flipElement(e), flipElement(e1));
        }
      }
    }
    else {
      assert(false);
    }
  }

  return ppath;
}

bool
CQChartsPath::
setValue(const QString &str)
{
  delete path_;

  path_ = new QPainterPath;

  //---

  CQStrParse parse(str);

  auto parseChar = [&]() {
    parse.skipSpace();

    auto c = parse.getChar();

    parse.skipSpace();

    return c;
  };

  auto skipCommaSpace = [&]() {
    parse.skipSpace();

    while (parse.isChar(',')) {
      parse.skipChar();

      parse.skipSpace();
    }
  };

  auto parseReal = [&](double &r) -> bool {
    if (! parse.readReal(&r))
      return false;

    parse.skipSpace();

    return true;
  };

  auto parsePoint = [&](QPointF &p) -> bool {
    double x = 0.0, y = 0.0;

    skipCommaSpace();

    if (! parseReal(x)) return false;

    skipCommaSpace();

    if (! parseReal(y)) return false;

    p = QPointF(x, y);

    return true;
  };

  auto parseFlag = [&](int *i) -> bool {
    if (! parse.isChar('0') && ! parse.isChar('1'))
      return false;

    *i = (parse.getCharAt().toLatin1() - '0');

    parse.skipChar();

    return true;
  };

  //---

  QChar   c;
  QChar   lastC;
  QPointF lastP;

  bool rereadCmd = false;

  bool valid = true;

  while (! parse.eof()) {
    if (! rereadCmd)
      c = parseChar();
    else
      rereadCmd = false;

    // relative move to
    if      (c == 'm') {
      lastC = c;

      QPointF p;

      if (! parsePoint(p)) {
        valid = false;
        break;
      }

      p += lastP;

      path_->moveTo(p);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parsePoint(p)) {
          valid = false;
          break;
        }

        p += lastP;

        path_->lineTo(p);
      }

      lastP = p;
    }
    // absolute move to
    else if (c == 'M') {
      lastC = c;

      QPointF p;

      if (! parsePoint(p)) {
        valid = false;
        break;
      }

      path_->moveTo(p);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parsePoint(p)) {
          valid = false;
          break;
        }

        path_->lineTo(p);
      }

      lastP = p;
    }
    // relative line to
    else if (c == 'l') {
      lastC = c;

      QPointF p;

      if (! parsePoint(p)) {
        valid = false;
        break;
      }

      p += lastP;

      path_->lineTo(p);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parsePoint(p)) {
          valid = false;
          break;
        }

        p += lastP;

        path_->lineTo(p);
      }

      lastP = p;
    }
    // absolute line to
    else if (c == 'L') {
      lastC = c;

      QPointF p;

      if (! parsePoint(p)) {
        valid = false;
        break;
      }

      path_->lineTo(p);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parsePoint(p)) {
          valid = false;
          break;
        }

        path_->lineTo(p);
      }

      lastP = p;
    }
    // relative hline to
    else if (c == 'h') {
      lastC = c;

      double d;

      parse.skipSpace();

      if (! parse.readReal(&d)) { valid = false; break; }

      QPointF p(lastP.x() + d, lastP.y());

      path_->lineTo(p);

      lastP = p;
    }
    // absolute hline to
    else if (c == 'H') {
      lastC = c;

      double d;

      parse.skipSpace();

      if (! parse.readReal(&d)) { valid = false; break; }

      QPointF p(d, lastP.y());

      path_->lineTo(p);

      lastP = p;
    }
    // relative vline to
    else if (c == 'v') {
      lastC = c;

      double d;

      parse.skipSpace();

      if (! parse.readReal(&d)) { valid = false; break; }

      QPointF p(lastP.x(), lastP.y() + d);

      path_->lineTo(p);

      lastP = p;
    }
    // absolute vline to
    else if (c == 'V') {
      lastC = c;

      double d;

      parse.skipSpace();

      if (! parse.readReal(&d)) { valid = false; break; }

      QPointF p(lastP.x(), d);

      path_->lineTo(p);

      lastP = p;
    }
    // relative arc to
    else if (c == 'a') {
      lastC = c;

      parse.skipSpace();

      // read (rx, ry)
      double rx, ry;

      if (! parse.readReal(&rx)) { valid = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&ry)) { valid = false; break; }

      skipCommaSpace();

      // read xa
      double xa;

      if (! parse.readReal(&xa)) { valid = false; break; }

      skipCommaSpace();

      // read fa, fs (single digit integers)
      int fa, fs;
      if (! parseFlag(&fa)) { valid = false; break; }

      skipCommaSpace();

      if (! parseFlag(&fs)) { valid = false; break; }

      skipCommaSpace();

      // read (x2, y2)
      double x2, y2;

      if (! parse.readReal(&x2)) { valid = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { valid = false; break; }

      skipCommaSpace();

      // add arc
      bool   unit_circle = false;
      double cx, cy, rx1, ry1, theta, delta;

      convertArcCoords(lastP.x(), lastP.y(), lastP.x() + x2, lastP.y() + y2,
                       xa, rx, ry, fa, fs, unit_circle,
                       &cx, &cy, &rx1, &ry1, &theta, &delta);

      path_->arcTo(QRectF(cx - rx1, cy - ry1, 2*rx1, 2*ry1), theta, delta);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        // read (rx, ry)
        if (! parse.readReal(&rx)) { valid = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&ry)) { valid = false; break; }

        skipCommaSpace();

        // read xa
        if (! parse.readReal(&xa)) { valid = false; break; }

        skipCommaSpace();

        // read fa, fs (single digit integers)
        int fa1, fs1;

        if (! parseFlag(&fa1)) { valid = false; break; }

        skipCommaSpace();

        if (! parseFlag(&fs1)) { valid = false; break; }

        skipCommaSpace();

        // read (x2, y2)
        if (! parse.readReal(&x2)) { valid = false; break; }

        skipCommaSpace();

        if (! parse.readReal(&y2)) { valid = false; break; }

        // add arc
        convertArcCoords(lastP.x(), lastP.y(), lastP.x() + x2, lastP.y() + y2,
                         xa, rx, ry, fa1, fs1, unit_circle,
                         &cx, &cy, &rx1, &ry1, &theta, &delta);

        path_->arcTo(QRectF(cx - rx1, cy - ry1, 2*rx1, 2*ry1), theta, delta);

        parse.skipSpace();
      }

      lastP = QPointF(lastP.x() + x2, lastP.y() + y2);
    }
    // absolute arc to
    else if (c == 'A') {
      lastC = c;

      int    fa, fs;
      double rx, ry, xa, x2, y2;

      parse.skipSpace();

      if (! parse.readReal(&rx)) { valid = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&ry)) { valid = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&xa)) { valid = false; break; }

      skipCommaSpace();

      if (! parse.readInteger(&fa)) { valid = false; break; }

      skipCommaSpace();

      if (! parse.readInteger(&fs)) { valid = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&x2)) { valid = false; break; }

      skipCommaSpace();

      if (! parse.readReal(&y2)) { valid = false; break; }

      bool   unit_circle = false;
      double cx, cy, rx1, ry1, theta, delta;

      convertArcCoords(lastP.x(), lastP.y(), x2, y2, xa, rx, ry, fa, fs, unit_circle,
                       &cx, &cy, &rx1, &ry1, &theta, &delta);

      path_->arcTo(QRectF(cx - rx1, cy - ry1, 2*rx1, 2*ry1), theta, delta);

      lastP = QPointF(x2, y2);
    }
    // relative quad bezier to
    else if (c == 'q') {
      lastC = c;

      QPointF p1, p2;

      if (! parsePoint(p1)) {
        valid = false;
        break;
      }

      if (! parsePoint(p2)) {
        valid = false;
        break;
      }

      p1 += lastP;
      p2 += lastP;

      path_->quadTo(p1, p2);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parsePoint(p1)) {
          valid = false;
          break;
        }

        if (! parsePoint(p2)) {
          valid = false;
          break;
        }

        p1 += lastP;
        p2 += lastP;

        path_->quadTo(p1, p2);
      }

      lastP = p2;
    }
    // absolute quad bezier to
    else if (c == 'Q') {
      lastC = c;

      QPointF p1, p2;

      if (! parsePoint(p1)) {
        valid = false;
        break;
      }

      if (! parsePoint(p2)) {
        valid = false;
        break;
      }

      path_->quadTo(p1, p2);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parsePoint(p1)) {
          valid = false;
          break;
        }

        if (! parsePoint(p2)) {
          valid = false;
          break;
        }

        path_->quadTo(p1, p2);
      }

      lastP = p2;
    }
    // TODO: 't' : 'T'
    // relative cubic bezier to
    else if (c == 'c') {
      lastC = c;

      QPointF p1, p2, p3;

      if (! parsePoint(p1)) {
        valid = false;
        break;
      }

      if (! parsePoint(p2)) {
        valid = false;
        break;
      }

      if (! parsePoint(p3)) {
        valid = false;
        break;
      }

      p1 += lastP;
      p2 += lastP;
      p3 += lastP;

      path_->cubicTo(p1, p2, p3);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parsePoint(p1)) {
          valid = false;
          break;
        }

        if (! parsePoint(p2)) {
          valid = false;
          break;
        }

        if (! parsePoint(p3)) {
          valid = false;
          break;
        }

        p1 += lastP;
        p2 += lastP;
        p3 += lastP;

        path_->cubicTo(p1, p2, p3);
      }

      lastP = p3;
    }
    // TODO: 's' : 'S'
    // absolute cubic bezier to
    else if (c == 'C') {
      lastC = c;

      QPointF p1, p2, p3;

      if (! parsePoint(p1)) {
        valid = false;
        break;
      }

      if (! parsePoint(p2)) {
        valid = false;
        break;
      }

      if (! parsePoint(p3)) {
        valid = false;
        break;
      }

      path_->cubicTo(p1, p2, p3);

      // TODO: leave to rereadCmd logic
      while (parse.isDigit() || parse.isChar('-') || parse.isChar('+') || parse.isChar('.')) {
        if (! parsePoint(p1)) {
          valid = false;
          break;
        }

        if (! parsePoint(p2)) {
          valid = false;
          break;
        }

        if (! parsePoint(p3)) {
          valid = false;
          break;
        }

        path_->cubicTo(p1, p2, p3);
      }

      lastP = p3;
    }
    // close path
    else if (c == 'Z' || c == 'z') {
      path_->closeSubpath();
    }
    else {
      // repeat last command if any
      bool isNumber = false;

      if (c == '-' || c == '+' || c == '.') {
        if (parse.getCharAt().isNumber())
          isNumber = true;
      }
      else if (c.isNumber()) {
        isNumber = true;
      }

      if (isNumber && lastC != '\0') {
        parse.unreadChar();

        c = lastC;

        rereadCmd = true;

        continue;
      }

      valid = false;

      break;
    }
  }

#if 0
  if (! valid)
    std::cerr << "Invalid path: " << str.toStdString() << "\n";
#endif

  return valid;
}
