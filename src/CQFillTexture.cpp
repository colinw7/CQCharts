#include <CQFillTexture.h>

#include <QPainter>
#include <QPen>
#include <QBrush>

#include <cmath>

CQFillTexture::
CQFillTexture()
{
}

void
CQFillTexture::
fillRect(QPainter *painter, const QRectF &rect)
{
  QPainterPath path;

  int x = rect.left();
  int y = rect.top();
  int w = rect.width();
  int h = rect.height();

  path.moveTo(x    , y    );
  path.lineTo(x + w, y    );
  path.lineTo(x + w, y + h);
  path.lineTo(x    , y + h);

  path.closeSubpath();

  fillPath(painter, path);
}

void
CQFillTexture::
fillPath(QPainter *painter, const QPainterPath &path)
{
  fillPolygon(painter, path.toFillPolygon());
}

void
CQFillTexture::
fillPolygon(QPainter *painter, const QPolygonF &poly)
{
  painter->setClipRegion(poly.toPolygon());

  if (isFilled())
    painter->setBrush(bgColor());
  else
    painter->setBrush(Qt::NoBrush);

  if (isStroked())
    painter->setPen(fgColor());
  else
    painter->setPen(Qt::NoPen);

  //---

  if (isBgFilled()) {
    QBrush saveBrush = painter->brush();

    QBrush brush(altColor());

    painter->setBrush(brush);

    painter->drawPolygon(poly);

    painter->setBrush(saveBrush);
  }

  //---

  if      (fillData_.fillType == FillType::HATCH) {
    hatchFillPolygon(painter, poly, fillData_);
  }
  else if (fillData_.fillType == FillType::CROSS_HATCH) {
    FillData fillData1 = fillData_;
    FillData fillData2 = fillData_;

    fillData1.fillType = FillType::HATCH;
    fillData2.fillType = FillType::HATCH;
    fillData2.angle    = fillData1.angle + M_PI/2.0;

    hatchFillPolygon(painter, poly, fillData1);
    hatchFillPolygon(painter, poly, fillData2);
  }
  else if (fillData_.fillType == FillType::ZIG_ZAG) {
    FillData fillData1 = fillData_;

    fillData1.fillType = FillType::HATCH;
    fillData1.connect  = true;

    hatchFillPolygon(painter, poly, fillData1);
  }
  else if (fillData_.fillType == FillType::DOTS ||
           fillData_.fillType == FillType::CROSSES ||
           fillData_.fillType == FillType::CAPS ||
           fillData_.fillType == FillType::WOVEN ||
           fillData_.fillType == FillType::WAVES ||
           fillData_.fillType == FillType::NYLON ||
           fillData_.fillType == FillType::SQUARES) {
    FillData fillData1 = fillData_;

    fillData1.fillType  = FillType::HATCH;
    fillData1.shapeType = static_cast<ShapeType>(fillData_.fillType);

    hatchFillPolygon(painter, poly, fillData1);
  }
  else if (fillData_.fillType == FillType::HEXAGONS) {
    fillHexagons(painter, poly, fillData_);
  }
}

void
CQFillTexture::
hatchFillPolygon(QPainter *painter, const QPolygonF &poly, const FillData &fillData)
{
  int np = poly.length();
  if (np == 0) return;

  //---

  // get x, y range
  double xmin = poly[0].x();
  double ymin = poly[0].y();
  double xmax = xmin;
  double ymax = ymin;

  for (int i1 = 1; i1 < np; ++i1) {
    const QPointF &p = poly[i1];

    xmin = std::min(xmin, p.x());
    ymin = std::min(ymin, p.y());
    xmax = std::max(xmax, p.x());
    ymax = std::max(ymax, p.y());
  }

  //---

  QPointF center((xmin + xmax)/2.0, (ymin + ymax)/2.0);

  double s = std::sin(fillData.angle);
  double c = std::cos(fillData.angle);

  //---

  auto rotatePoint = [&](const QPointF &p) {
    double x1 = p.x() - center.x();
    double y1 = p.y() - center.y();

    double x2 = x1*c - y1*s;
    double y2 = x1*s + y1*c;

    return QPointF(x2 + center.x(), y2 + center.y());
  };

  //---

  // rotate polygon
  QPolygonF poly1;

  for (int i1 = 0; i1 < np; ++i1) {
    const QPointF &p = poly[i1];

    poly1.push_back(rotatePoint(p));
  }

   //---

  // set rotated range
  double xrmin = poly1[0].x();
  double yrmin = poly1[0].y();
  double xrmax = xrmin;
  double yrmax = yrmin;

  for (int i1 = 1; i1 < np; ++i1) {
    const QPointF &p = poly1[i1];

    xrmin = std::min(xrmin, p.x());
    yrmin = std::min(yrmin, p.y());
    xrmax = std::max(xrmax, p.x());
    yrmax = std::max(yrmax, p.y());
  }

  //---

  struct Line {
    QPointF p1;
    QPointF p2;

    Line() = default;

    Line(const QPointF &p1, const QPointF &p2) :
     p1(p1), p2(p2) {
    }
  };

  using Lines = std::vector<Line>;

  Lines lines;

  auto delta = std::max(2*fillData.radius + fillData.delta, 1.0);

  for (double yy = yrmin; yy <= yrmax; yy += delta) {
    bool   xset  = false;
    double lxmin = 0.0;
    double lxmax = 0.0;

    double xx;

    // clip each line to rotated polygon
    for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
      const QPointF &p1 = poly1[i1];
      const QPointF &p2 = poly1[i2];

      if ((p1.y() < yy && p2.y() < yy) || (p1.y() > yy && p2.y() > yy) || p1.y() == p2.y())
        continue;

      if      (yy == p1.y())
        xx = p1.x();
      else if (yy == p2.y())
        xx = p2.x();
      else {
        double factor = (p2.x() - p1.x())/(p2.y() - p1.y());

        xx = (yy - p1.y())*factor + p1.x();
      }

      if (xset) {
        lxmin = std::min(lxmin, xx);
        lxmax = std::max(lxmax, xx);
      }
      else {
        lxmin = xx;
        lxmax = xx;
        xset  = true;
      }
    }

    if (! xset)
      continue;

    //-----

    lines.push_back(Line(QPointF(lxmin, yy), QPointF(lxmax, yy)));
  }

  //---

  s = std::sin(-fillData.angle);
  c = std::cos(-fillData.angle);

  QPointF lastPoint;
  bool    first = true;

  auto fillData1 = fillData;

  fillData1.lineNum = 0;

  QPainterPath path;

  if      (fillData1.shapeType == ShapeType::DOTS)
    circlePath(fillData1.radius, path);
  else if (fillData1.shapeType == ShapeType::CROSSES)
    crossPath(fillData1.radius, fillData1.angle, path);
  else if (fillData1.shapeType == ShapeType::CAPS)
    capPath(fillData1.radius, fillData1.angle, path);
  else if (fillData1.shapeType == ShapeType::WAVES)
    wavePath(fillData1.radius, fillData1.angle, path);
  else if (fillData1.shapeType == ShapeType::WOVEN)
    wovenPath(fillData1.radius, fillData1.angle, path);
  else if (fillData1.shapeType == ShapeType::NYLON)
    nylonPath(fillData1.radius, fillData1.angle, path);
  else if (fillData1.shapeType == ShapeType::SQUARES)
    squarePath(fillData1.radius, fillData1.angle, path);

  for (auto &line : lines) {
    if      (fillData1.shapeType == ShapeType::DOTS ||
             fillData1.shapeType == ShapeType::HEXAGONS ||
             fillData1.shapeType == ShapeType::CROSSES ||
             fillData1.shapeType == ShapeType::CAPS ||
             fillData1.shapeType == ShapeType::WOVEN ||
             fillData1.shapeType == ShapeType::WAVES ||
             fillData1.shapeType == ShapeType::NYLON ||
             fillData1.shapeType == ShapeType::SQUARES) {
      double dd = delta;

      double r = fillData1.radius;
      double x = xrmin - 2*r;

      fillData1.cellNum = 0;

      while (true) {
        if (x - 2*r > line.p2.x()) // offscreen
          break;

        bool onLeft = (x + r < line.p1.x());

        if (! onLeft) {
          QPointF p1 = rotatePoint(QPointF(x, line.p1.y()));

          if      (fillData1.shapeType == ShapeType::DOTS ||
                   fillData1.shapeType == ShapeType::SQUARES)
            moveFillShapePath(painter, path, p1, fillData1);
          else if (fillData1.shapeType == ShapeType::CROSSES ||
                   fillData1.shapeType == ShapeType::CAPS ||
                   fillData1.shapeType == ShapeType::WAVES ||
                   fillData1.shapeType == ShapeType::WOVEN ||
                   fillData1.shapeType == ShapeType::NYLON)
            moveDrawShapePath(painter, path, p1, fillData1);
        }

        x += dd;

        ++fillData1.cellNum;
      }
    }
    else {
      QPointF p1 = rotatePoint(line.p1);
      QPointF p2 = rotatePoint(line.p2);

      if (fillData1.connect && ! first)
        drawLine(painter, lastPoint, p1, fillData1.width);

      drawLine(painter, p1, p2, fillData1.width);

      lastPoint = p2;
      first     = false;
    }

    ++fillData1.lineNum;
  }
}

void
CQFillTexture::
fillHexagons(QPainter *painter, const QPolygonF &poly, const FillData &fillData)
{
  QPen savePen = painter->pen();
  QPen pen1    = savePen;

  pen1.setColor (painter->pen().color());
  pen1.setWidthF(fillData.width);

  painter->setPen(pen1);

  //---

  auto rect = poly.boundingRect();

  double w = rect.width();
  double h = rect.height();

  double xc = rect.center().x();
  double yc = rect.center().y();

  double sx = 2.0/std::sqrt(3.0);

  double r1 = fillData.radius/std::sqrt(2.0);

  int nx = int(sx*w/(2*fillData.radius) + 0.5) + 2;
  int ny = int(   h/(2*fillData.radius) + 0.5) + 4;

  for (int ix = -nx/2; ix <= nx/2; ++ix) {
    double x = 1.5*ix*fillData.radius + ix*fillData.delta + xc;

    double y1 = -ix*r1 - ix*fillData.delta;

    int ix1 = ix/2;

    for (int iy = -ny/2 + ix1; iy <= ny/2 + ix1; ++iy) {
      double y = iy*2*r1 + iy*fillData.delta + yc;

      double yb = y + y1 - r1;
      double yt = y + y1 + r1;
      double ym = (yb + yt)/2.0;

      double xl = x - fillData.radius;
      double xr = x + fillData.radius;
      double xm = (xl + xr)/2.0;

      double xl1 = xm - fillData.radius/2;
      double xr1 = xm + fillData.radius/2;

      QPainterPath path;

      path.moveTo(xl1, yb);
      path.lineTo(xr1, yb);
      path.lineTo(xr , ym);
      path.lineTo(xr1, yt);
      path.lineTo(xl1, yt);
      path.lineTo(xl , ym);

      path.closeSubpath();

      path = rotatePath(path, QPointF(xc, yc), fillData.angle);

      if (fillData.filled)
        painter->fillPath(path, painter->brush());

      if (fillData.stroked)
        painter->strokePath(path, painter->pen());
    }
  }

  //---

  painter->setPen(savePen);
}

void
CQFillTexture::
drawLine(QPainter *painter, const QPointF &p1, const QPointF &p2, double width)
{
  QPen savePen = painter->pen();
  QPen pen1    = savePen;

  pen1.setColor (painter->pen().color());
  pen1.setWidthF(width);

  painter->setPen(pen1);

  painter->drawLine(p1, p2);

  painter->setPen(savePen);
}

void
CQFillTexture::
moveFillShapePath(QPainter *painter, const QPainterPath &path, const QPointF &center,
                  const FillData &fillData)
{
  auto path1 = movePath(path, center);

  if (fillData.filled)
    painter->fillPath(path1, painter->brush());

  if (fillData.stroked)
    drawShapePath(painter, path1, fillData.width);
}

void
CQFillTexture::
moveDrawShapePath(QPainter *painter, const QPainterPath &path, const QPointF &center,
                  const FillData &fillData)
{
  auto path1 = movePath(path, center);

  drawShapePath(painter, path1, fillData.width);
}

void
CQFillTexture::
drawShapePath(QPainter *painter, const QPainterPath &path, double width)
{
  QPen savePen = painter->pen();
  QPen pen1    = savePen;

  pen1.setColor (painter->pen().color());
  pen1.setWidthF(width);

  painter->setPen(pen1);

  painter->strokePath(path, painter->pen());

  painter->setPen(savePen);
}

void
CQFillTexture::
circlePath(double r, QPainterPath &path) const
{
  path.moveTo(r, 0.0);

  path.arcTo(QRectF(-r, -r, 2*r, 2*r), 0.0, 360.0);

  path.closeSubpath();
}

void
CQFillTexture::
crossPath(double r, double angle, QPainterPath &path) const
{
  path.moveTo(-r/2, -r/2);
  path.lineTo( r/2,  r/2);

  path.moveTo(-r/2,  r/2);
  path.lineTo( r/2, -r/2);

  path = rotatePath(path, QPointF(0, 0), angle);
}

void
CQFillTexture::
capPath(double r, double angle, QPainterPath &path) const
{
  path = QPainterPath();

  path.moveTo(-r,  r);
  path.lineTo( 0, -r);
  path.lineTo( r,  r);

  path = rotatePath(path, QPointF(0, 0), angle);
}

void
CQFillTexture::
wovenPath(double r, double angle, QPainterPath &path) const
{
  path.moveTo(-r,  0);
  path.lineTo( 0, -r);
  path.moveTo( 0,  0);
  path.lineTo( r,  r);

  path = rotatePath(path, QPointF(0, 0), angle);
}

void
CQFillTexture::
wavePath(double r, double angle, QPainterPath &path) const
{
  path = QPainterPath();

  path.moveTo(-r  ,  0);
  path.quadTo(-r/2,  r, 0, 0);
  path.quadTo( r/2, -r, r, 0);

  path = rotatePath(path, QPointF(0, 0), angle);
}

void
CQFillTexture::
nylonPath(double r, double angle, QPainterPath &path) const
{
  path.moveTo(-r  , -r/2);
  path.lineTo(-r/2, -r/2);
  path.lineTo(-r/2, -r  );

  path.moveTo(-r/2,  0  );
  path.lineTo(-r/2,  r/2);
  path.lineTo(   0,  r/2);

  path.moveTo(   0, -r/2);
  path.lineTo( r/2, -r/2);
  path.lineTo( r/2,    0);

  path.moveTo( r/2,  r  );
  path.lineTo( r/2,  r/2);
  path.lineTo( r  ,  r/2);

  path = rotatePath(path, QPointF(0, 0), angle);
}

void
CQFillTexture::
squarePath(double r, double angle, QPainterPath &path) const
{
  static QPainterPath s_path;
  static double       s_r     { -1 };
  static double       s_angle { -1 };

  if (r != s_r || angle != s_angle) {
    s_r     = r;
    s_angle = angle;

    s_path = QPainterPath();

    s_path.moveTo(-r, -r);
    s_path.lineTo( r, -r);
    s_path.lineTo( r,  r);
    s_path.lineTo(-r,  r);

    s_path.closeSubpath();

    s_path = rotatePath(s_path, QPointF(0, 0), s_angle);
  }

  path = s_path;
}

QPainterPath
CQFillTexture::
movePath(const QPainterPath &path, const QPointF &c) const
{
  QTransform t;

  t.translate(c.x(), c.y());

  return t.map(path);
}

QPainterPath
CQFillTexture::
rotatePath(const QPainterPath &path, const QPointF &c, double angle) const
{
  QTransform t;

  t.translate(c.x(), c.y());
  t.rotate(-180.0*angle/M_PI);
  t.translate(-c.x(), -c.y());

  return t.map(path);
}

QString
CQFillTexture::
toString() const
{
  auto fillTypeString = [](const FillType &type) {
    switch (type) {
      case FillType::FILL       : return "fill";
      case FillType::HATCH      : return "hatch";
      case FillType::CROSS_HATCH: return "cross_hatch";
      case FillType::ZIG_ZAG    : return "zog_zag";
      case FillType::DOTS       : return "dots";
      case FillType::HEXAGONS   : return "hexagons";
      case FillType::CROSSES    : return "crosses";
      case FillType::CAPS       : return "caps";
      case FillType::WOVEN      : return "woven";
      case FillType::WAVES      : return "waves";
      case FillType::NYLON      : return "nylon";
      case FillType::SQUARES    : return "squares";
      default:                    return "none";
    }
  };

  auto radToDeg = [](double r) {
    return r*180.0/M_PI;
  };

  auto strs = QStringList() <<
              QString("type=%1").arg(fillTypeString(fillData_.fillType)) <<
              QString("width=%1").arg(fillData_.width) <<
              QString("delta=%1").arg(fillData_.delta) <<
              QString("angle=%1").arg(radToDeg(fillData_.angle)) <<
              QString("radius=%1").arg(fillData_.radius) <<
              QString("bgColor=%1").arg(isFilled() ? fillData_.bgColor.name() : "<none>") <<
              QString("fgColor=%1").arg(isStroked() ? fillData_.bgColor.name() : "<none>") <<
              QString("AltColor=%1").arg(isBgFilled() ? fillData_.altColor.name() : "<none>");

  return strs.join(" ");
}
