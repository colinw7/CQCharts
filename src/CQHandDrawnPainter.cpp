#include <CQHandDrawnPainter.h>

#include <QPainter>
#include <random>
#include <cmath>
#include <cassert>

class RealInRange {
 public:
  RealInRange(double min, double max) :
   eng_(rd_()), rdis_(min, max) {
  }

  double gen() {
    return rdis_(eng_);
  }

 private:
  std::random_device                     rd_;   //!< random device
  std::default_random_engine             eng_;  //!< random engine
//std::mt19937                           mt_;   //!< mersine twister
  std::uniform_real_distribution<double> rdis_; //!< uniform distribution
};

namespace Math {
  inline double map(double r1, double r2, double f) {
    return r1 + (r2 - r1)*f;
  }

  inline double degToRad(double a) {
    return M_PI*a/180.0;
  }
}

//---

CQHandDrawnPainter::
CQHandDrawnPainter(QPainter *painter) :
 painter_(painter)
{
}

void
CQHandDrawnPainter::
setSize(int width, int height)
{
  width_  = width;
  height_ = height;

  maxSize_ = std::max(width_, height_);
}

//---

void
CQHandDrawnPainter::
fillPath(const QPainterPath &path, const QBrush &brush)
{
  QBrush saveBrush = painter_->brush();

  QPainterPath path1 = randomizePath(path);

  if (brush.style() != Qt::NoBrush && brush.style() != Qt::SolidPattern) {
    painter_->setBrush(brush);

    fillPatternPath(path1);
  }
  else {
    painter_->fillPath(path1, brush);
  }

  painter_->setBrush(saveBrush);
}

void
CQHandDrawnPainter::
strokePath(const QPainterPath &path, const QPen &pen)
{
  QPen savePen = painter_->pen();

  QPainterPath path1 = randomizePath(path);
  QPainterPath path2 = randomizePath(path);

  QPen pen1 = pen;
  QColor c1 = pen.color(); c1.setAlphaF(0.5);
  pen1.setColor(c1);

  painter_->strokePath(path1, pen1);
  painter_->strokePath(path2, pen1);

  painter_->setPen(savePen);
}

void
CQHandDrawnPainter::
drawPath(const QPainterPath &path)
{
  fillPath  (path, painter_->brush());
  strokePath(path, painter_->pen  ());
}

void
CQHandDrawnPainter::
fillPatternPath(const QPainterPath &path)
{
  if (altColor().isValid()) {
    QColor saveAltColor = altColor();

    QBrush brush(altColor());

    altColor_ = QColor();

    fillPath(path, brush);

    altColor_ = saveAltColor;
  }

  FillData fillData;

  fillData.delta = fillDelta();

  if      (painter_->brush().style() >= Qt::Dense1Pattern &&
           painter_->brush().style() <= Qt::Dense7Pattern) {
    fillData.fillType = FillType::DOTS;
  }
  else if (painter_->brush().style() == Qt::HorPattern) {
    fillData.fillType = FillType::HATCH;
  }
  else if (painter_->brush().style() == Qt::VerPattern) {
    fillData.fillType = FillType::HATCH;
    fillData.angle    = M_PI/2.0;
  }
  else if (painter_->brush().style() == Qt::CrossPattern) {
    fillData.fillType = FillType::CROSS_HATCH;
  }
  else if (painter_->brush().style() == Qt::BDiagPattern) {
    fillData.fillType = FillType::HATCH;
    fillData.angle    = Math::degToRad(fillAngle());
  }
  else if (painter_->brush().style() == Qt::FDiagPattern) {
    fillData.fillType = FillType::HATCH;
    fillData.angle    = -Math::degToRad(fillAngle());
  }
  else if (painter_->brush().style() == Qt::DiagCrossPattern) {
    fillData.fillType = FillType::CROSS_HATCH;
    fillData.angle    = -Math::degToRad(fillAngle());
  }
  else {
    assert(false);
  }

  fillDataPolygon(path.toFillPolygon(), fillData);
}

QPainterPath
CQHandDrawnPainter::
randomizePath(const QPainterPath &path) const
{
  QRectF rect = path.boundingRect();

  QPointF p1 = rect.topLeft    ();
  QPointF p2 = rect.bottomRight();

  double l = std::hypot(p2.x() - p1.x(), p2.y() - p1.y());

  dampen_ = 0.4 + 0.6*l/maxSize_;

  //---

  QPainterPath path1;

  int n = path.elementCount();

  for (int i = 0; i < n; ++i) {
    const QPainterPath::Element &e = path.elementAt(i);

    if      (e.isMoveTo()) {
      path1.moveTo(randomize(QPointF(e.x, e.y)));
    }
    else if (e.isLineTo()) {
      path1.lineTo(randomize(QPointF(e.x, e.y)));
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
        if (e2t == QPainterPath::CurveToDataElement) {
          path1.cubicTo(randomize(QPointF(e .x, e .y)),
                        randomize(QPointF(e1.x, e1.y)),
                        randomize(QPointF(e2.x, e2.y)));

          i += 2;
        }
        else {
          path1.quadTo(randomize(QPointF(e .x, e .y)),
                       randomize(QPointF(e1.x, e1.y)));

          ++i;
        }
      }
    }
    else {
      assert(false);
    }
  }

  return path1;
}

//---

void
CQHandDrawnPainter::
fillRect(const QRectF &rect, const QBrush &brush)
{
  QPainterPath path = rectPath(rect);

  fillPath(path, brush);
}

void
CQHandDrawnPainter::
drawRect(const QRectF &rect)
{
  QPainterPath path = rectPath(rect);

  drawPath(path);
}

void
CQHandDrawnPainter::
strokeRect(const QRectF &rect)
{
  QPainterPath path = rectPath(rect);

  strokePath(path, painter_->pen());
}

QPainterPath
CQHandDrawnPainter::
rectPath(const QRectF &rect) const
{
  double x1 = rect.left  ();
  double y1 = rect.top   ();
  double x2 = rect.right ();
  double y2 = rect.bottom();

  QPainterPath path;

  path.moveTo(x1, y1);
  path.lineTo(x2, y1);
  path.lineTo(x2, y2);
  path.lineTo(x1, y2);

  path.closeSubpath();

  return path;
}

//---

void
CQHandDrawnPainter::
drawCircle(const QPointF &center, double r)
{
  drawCircle1(center, r);
  drawCircle1(center, r);
}

void
CQHandDrawnPainter::
fillCircle(const QPointF &center, double r)
{
  fillCircle1(center, r);
  fillCircle1(center, r);
}

void
CQHandDrawnPainter::
drawCircle1(const QPointF &center, double r)
{
  QPainterPath path = circlePath(center, r);

  painter_->drawPath(path);
}

void
CQHandDrawnPainter::
fillCircle1(const QPointF &center, double r)
{
  QPainterPath path = circlePath(center, r);

  painter_->fillPath(path, painter_->brush());
}

QPainterPath
CQHandDrawnPainter::
circlePath(const QPointF &center, double r) const
{
  dampen_ = 1.0;

  int n = 8;

  if (r < 16) {
    n       = 4;
    dampen_ = 0.7;
  }

  std::vector<QPointF> points;

  points.resize(n);

  double da = 2*M_PI/n;

  double a = 0.0;

  for (int i = 0; i < n; ++i) {
    double c = std::cos(a);
    double s = std::sin(a);

    points[i] = randomize(center + QPointF(r*c, r*s));

    a += da;
  }

  double l = std::hypot(points[1].x() - points[0].x(), points[1].y() - points[0].y())/3.0;

  std::vector<QPointF> points1, points2;

  points1.resize(n);
  points2.resize(n);

  a = 0.0;

  for (int i = 0; i < n; ++i) {
    double c = std::cos(a);
    double s = std::sin(a);

    double lc = l*c;
    double ls = l*s;

    points1[i] = randomize(QPointF(points[i].x() + ls, points[i].y() - lc));
    points2[i] = randomize(QPointF(points[i].x() - ls, points[i].y() + lc));

    a += da;
  }

  QPainterPath path;

  path.moveTo(points[0]);

  for (int i = 0; i < n; ++i) {
    int i1 = i + 1; if (i1 >= n) i1 = 0;

    path.cubicTo(points2[i], points1[i1], points[i1]);
  }

  return path;
}

void
CQHandDrawnPainter::
drawLine(const QPointF &p1, const QPointF &p2)
{
  setSize(painter_->device()->width(), painter_->device()->height());

  double l = std::hypot(p2.x() - p1.x(), p2.y() - p1.y());

  dampen_ = 0.4 + 0.6*l/maxSize_;

  drawLine1(p1, p2);
  drawLine1(p1, p2);
}

void
CQHandDrawnPainter::
drawLine1(const QPointF &p1, const QPointF &p2)
{
  QPointF pr1 = randomize(p1);
  QPointF pr2 = randomize(p2);

  RealInRange rl(0.40, 0.60);
  RealInRange rr(0.55, 0.85);

  double g1 = rl.gen();
  double g2 = rr.gen();

  double x3 = p1.x() + g1*(p2.x() - p1.x());
  double y3 = p1.y() + g1*(p2.y() - p1.y());

  double x4 = p1.x() + g2*(p2.x() - p1.x());
  double y4 = p1.y() + g2*(p2.y() - p1.y());

  QPointF pr3 = randomize(QPointF(x3, y3));
  QPointF pr4 = randomize(QPointF(x4, y4));

  QPainterPath path;

  path.moveTo (pr1);
  path.cubicTo(pr3, pr4, pr2);

  painter_->drawPath(path);
}

void
CQHandDrawnPainter::
drawPoint(const QPointF &p)
{
  painter_->drawPoint(p);
}

void
CQHandDrawnPainter::
drawDebugPoint(const QPointF &p)
{
  QPen savePen = painter_->pen();

  painter_->setPen(Qt::red);

  painter_->drawLine(QPointF(p.x() - 4, p.y()), QPointF(p.x() + 4, p.y()));
  painter_->drawLine(QPointF(p.x(), p.y() - 4), QPointF(p.x(), p.y() + 4));

  painter_->setPen(savePen);
}

void
CQHandDrawnPainter::
fillDataPolygon(const QPolygonF &poly, const FillData &fillData)
{
  QPen savePen = painter_->pen();

  QPen pen1 = savePen;

  pen1.setColor(painter_->brush().color());

  painter_->setPen(pen1);

  if      (fillData.fillType == FillType::HATCH) {
    hatchFillPolygon(poly, fillData);
  }
  else if (fillData.fillType == FillType::CROSS_HATCH) {
    FillData fillData1 = fillData;
    FillData fillData2 = fillData;

    fillData1.fillType = FillType::HATCH;
    fillData2.fillType = FillType::HATCH;
    fillData2.angle    = fillData1.angle + M_PI/2.0;

    hatchFillPolygon(poly, fillData1);
    hatchFillPolygon(poly, fillData2);
  }
  else if (fillData.fillType == FillType::ZIG_ZAG) {
    FillData fillData1 = fillData;

    fillData1.fillType = FillType::HATCH;
    fillData1.connect  = true;

    hatchFillPolygon(poly, fillData1);
  }
  else if (fillData.fillType == FillType::DOTS) {
    FillData fillData1 = fillData;

    fillData1.fillType = FillType::HATCH;
    fillData1.dots     = true;

    hatchFillPolygon(poly, fillData1);
  }

  painter_->setPen(savePen);
}

void
CQHandDrawnPainter::
hatchFillPolygon(const QPolygonF &poly, const FillData &fillData)
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

  QPolygonF poly1;

  for (int i1 = 0; i1 < np; ++i1) {
    const QPointF &p = poly[i1];

    poly1.push_back(rotatePoint(p));
  }

  //---

  // update y range
  ymin = poly1[0].y();
  ymax = ymin;

  for (int i1 = 1; i1 < np; ++i1) {
    const QPointF &p = poly1[i1];

    ymin = std::min(ymin, p.y());
    ymax = std::max(ymax, p.y());
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

  for (double yy = ymin; yy <= ymax; yy += fillData.delta) {
    bool   xset = false;
    double xmin = 0.0;
    double xmax = 0.0;

    double xx;

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
        xmin = std::min(xmin, xx);
        xmax = std::max(xmax, xx);
      }
      else {
        xmin = xx;
        xmax = xx;
        xset = true;
      }
    }

    if (! xset)
      continue;

    //-----

    lines.push_back(Line(QPointF(xmin, yy), QPointF(xmax, yy)));
  }

  //---

  s = std::sin(-fillData.angle);
  c = std::cos(-fillData.angle);

  QPointF lastPoint;
  bool    first = true;

  for (auto &line : lines) {
    if (fillData.dots) {
      QPen   savePen   = painter_->pen();
      QBrush saveBrush = painter_->brush();

      painter_->setBrush(savePen.color());

      int    nd = 8;
      double dd = (xmax - xmin)/(nd + 1);

      RealInRange rr(-dd/3, dd/3);

      double x = line.p1.x() + dd/2;

      while (true) {
        if (x + dd/3 > line.p2.x())
          break;

        x += rr.gen();

        QPointF p1 = randomize(rotatePoint(QPointF(x, line.p1.y())));

        fillCircle(p1, dd/6);

        x += dd;
      }

      painter_->setPen  (savePen);
      painter_->setBrush(saveBrush);
    }
    else {
      QPointF p1 = rotatePoint(line.p1);
      QPointF p2 = rotatePoint(line.p2);

      if (fillData.connect && ! first)
        drawLine(lastPoint, p1);

      //painter_->drawLine(p1, p2);
      drawLine(p1, p2);

      lastPoint = p2;
      first     = false;
    }
  }
}

//---

void
CQHandDrawnPainter::
drawPolygon(const QPolygonF &poly)
{
  int np = poly.length();
  if (np == 0) return;

  QPainterPath path;

  for (int i = 0; i < np; ++i) {
    if (i == 0)
      path.moveTo(poly[i]);
    else
      path.lineTo(poly[i]);
  }

  path.closeSubpath();

  strokePath(path, painter_->pen());
}

void
CQHandDrawnPainter::
fillPolygon(const QPolygonF &poly)
{
  int np = poly.length();
  if (np == 0) return;

  QPainterPath path;

  for (int i = 0; i < np; ++i) {
    if (i == 0)
      path.moveTo(poly[i]);
    else
      path.lineTo(poly[i]);
  }

  path.closeSubpath();

  fillPath(path, painter_->brush());
}

void
CQHandDrawnPainter::
drawPolyline(const QPolygonF &poly)
{
  int np = poly.length();
  if (np == 0) return;

  for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
    drawLine(poly[i1], poly[i2]);
  }
}

//---

void
CQHandDrawnPainter::
drawText(const QPointF &p, const QString &text)
{
  painter_->drawText(p, text);
}

void
CQHandDrawnPainter::
drawImage(const QPointF &p, const QImage &image)
{
  painter_->drawImage(p, image);
}

void
CQHandDrawnPainter::
drawImage(const QRectF &r, const QImage &image)
{
  painter_->drawImage(r, image);
}

QPointF
CQHandDrawnPainter::
randomize(const QPointF &p) const
{
  RealInRange rr(-roughness()*dampen_, roughness()*dampen_);

  double dx = rr.gen();
  double dy = rr.gen();

  return QPointF(p.x() + dx, p.y() + dy);
}
