#include <CQChartsRoundedPolygon.h>
#include <CQChartsPaintDevice.h>
#include <QPainterPath>
#include <cmath>

namespace Util {

void interpLine(const QPointF &p1, const QPointF &p2, double xsize, double ysize,
                QPointF &pc1, QPointF &pc2) {
  double dx = p2.x() - p1.x();
  double dy = p2.y() - p1.y();

  double l = std::hypot(dx, dy);

  if (l < 1E-6) {
    pc1 = p1;
    pc2 = p2;

    return;
  }

  double mx1 = (xsize < l/2 ? xsize/l : 0.5);
  double mx2 = 1.0 - mx1;

  double my1 = (ysize < l/2 ? ysize/l : 0.5);
  double my2 = 1.0 - my1;

  double x1 = p1.x() + mx1*dx;
  double y1 = p1.y() + my1*dy;

  double x2 = p1.x() + mx2*dx;
  double y2 = p1.y() + my2*dy;

  pc1 = QPointF(x1, y1);
  pc2 = QPointF(x2, y2);
}

}

//------

namespace CQChartsRoundedPolygon {

void
draw(CQChartsPaintDevice *device, const BBox &bbox, double xsize, double ysize,
     const CQChartsSides &sides)
{
  if (xsize > 0 || ysize > 0) {
    QPainterPath path;

    path.addRoundedRect(bbox.qrect(), xsize, ysize);

    device->drawPath(path);
  }
  else {
    QPainterPath path;

    if (sides.isAll()) {
      device->drawRect(bbox);
    }
    else {
      device->fillRect(bbox);

      if (sides.isLeft())
        device->drawLine(bbox.getLL(), bbox.getUL());

      if (sides.isRight())
        device->drawLine(bbox.getLR(), bbox.getUR());

      if (sides.isTop())
        device->drawLine(bbox.getUL(), bbox.getUR());

      if (sides.isBottom())
        device->drawLine(bbox.getLL(), bbox.getLR());
    }
  }
}

void
draw(CQChartsPaintDevice *device, const Polygon &poly, double xsize, double ysize)
{
  QPainterPath path;

  if (poly.size() < 3) {
    device->drawPolygon(poly);
    return;
  }

  int i1 = poly.size() - 1;
  int i2 = 0;
  int i3 = 1;

  while (i2 < poly.size()) {
    const QPointF &p1 = poly.qpoint(i1);
    const QPointF &p2 = poly.qpoint(i2);
    const QPointF &p3 = poly.qpoint(i3);

    if (xsize > 0 || ysize > 0) {
      QPointF p12s, p12e, p23s, p23e;

      Util::interpLine(p1, p2, xsize, ysize, p12s, p12e);
      Util::interpLine(p2, p3, xsize, ysize, p23s, p23e);

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

    if (i3 >= poly.size())
      i3 = 0;
  }

  path.closeSubpath();

  device->drawPath(path);
}

}
