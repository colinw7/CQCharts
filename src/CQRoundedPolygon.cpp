#include <CQRoundedPolygon.h>
#include <QPainterPath>
#include <QPainter>
#include <cmath>

namespace Util {

void interpLine(const QPointF &p1, const QPointF &p2, double size, QPointF &pc1, QPointF &pc2) {
  double dx = p2.x() - p1.x();
  double dy = p2.y() - p1.y();

  double l = std::hypot(dx, dy);

  if (l < 1E-6) {
    pc1 = p1;
    pc2 = p2;

    return;
  }

  double m1 = std::min(l/2, size)/l;
  double m2 = 1.0 - m1;

  double x1 = p1.x() + m1*dx;
  double y1 = p1.y() + m1*dy;

  double x2 = p1.x() + m2*dx;
  double y2 = p1.y() + m2*dy;

  pc1 = QPointF(x1, y1);
  pc2 = QPointF(x2, y2);
}

}

//------

namespace CQRoundedPolygon {

void
draw(QPainter *painter, const QRectF &rect, double size)
{
  if (size > 0)
    painter->drawRoundedRect(rect, size, size);
  else
    painter->drawRect(rect);
}

void
draw(QPainter *painter, const QPolygonF &poly, double size)
{
  QPainterPath path;

  if (poly.count() < 3) {
    painter->drawPolygon(poly);
    return;
  }

  int i1 = poly.count() - 1;
  int i2 = 0;
  int i3 = 1;

  while (i2 < poly.count()) {
    const QPointF &p1 = poly[i1];
    const QPointF &p2 = poly[i2];
    const QPointF &p3 = poly[i3];

    if (size > 0) {
      QPointF p12s, p12e, p23s, p23e;

      Util::interpLine(p1, p2, size, p12s, p12e);
      Util::interpLine(p2, p3, size, p23s, p23e);

      if (i2 == 0)
        path.moveTo(p12s);
      else
        path.lineTo(p12s);

      path.lineTo(p12e);
      path.quadTo(p2, p23s);
    }
    else {
      if (i2 == 0)
        path.moveTo(p2);
      else
        path.lineTo(p2);

      path.lineTo(p3);
    }

    i1 = i2;
    i2 = i3++;

    if (i2 == 0)
      break;

    if (i3 >= poly.count())
      i3 = 0;
  }

  path.closeSubpath();

  painter->drawPath(path);
}

}
