#include <CQChartsRotatedText.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsUtil.h>

#include <cmath>

namespace CQChartsRotatedText {

void
draw(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
     double angle, Qt::Alignment align, bool alignBBox, bool contrast)
{
  if (device->type() != CQChartsPaintDevice::Type::SCRIPT)
    device->save();

  QFontMetricsF fm(device->font());

  double th = fm.height();
  double tw = fm.width(text);

  double a1 = CMathUtil::Deg2Rad(angle);

  double c = cos(-a1);
  double s = sin(-a1);

  double tx = 0.0, ty = 0.0;
  double ax = 0.0, ay = 0.0;

  if (! alignBBox) {
    double dx = 0.0, dy = 0.0;

    if      (align & Qt::AlignLeft)
      dx = 0.0;
    else if (align & Qt::AlignRight)
      dx = -tw;
    else if (align & Qt::AlignHCenter)
      dx = -tw/2.0;

    if      (align & Qt::AlignBottom)
      dy = 0.0;
    else if (align & Qt::AlignTop)
      dy = th;
    else if (align & Qt::AlignVCenter)
      dy = th/2.0;

    tx = c*dx - s*dy;
    ty = s*dx + c*dy;

    ax = -s*fm.descent();
    ay =  c*fm.descent();
  }
  else {
    if      (align & Qt::AlignLeft)
      tx = -th*s;
    else if (align & Qt::AlignRight)
      tx = -tw*c;
    else if (align & Qt::AlignHCenter)
      tx = -(th*s + tw*c)/2.0;

    if      (align & Qt::AlignBottom)
      ty = 0.0;
    else if (align & Qt::AlignTop)
      ty = -(tw*s - th*c);
    else if (align & Qt::AlignVCenter)
      ty = -(tw*s - th*c)/2;

    ax = -s*fm.descent();
    ay =  c*fm.descent();
  }

  //------

  QPointF pp = device->windowToPixel(p);

  QPointF pt(pp.x() + tx - ax, pp.y() + ty - ay);

  QPointF pt1 = device->pixelToWindow(pt);

  device->setTransformRotate(pt1, angle);

  if (contrast) {
    QColor tc = device->pen().color();

  //QColor icolor = CQChartsUtil::invColor(tc);
    QColor icolor = CQChartsUtil::bwColor(tc);

    icolor.setAlphaF(0.5);

    // draw contrast outline
    device->setPen(icolor);

    for (int dy = -2; dy <= 2; ++dy) {
      for (int dx = -2; dx <= 2; ++dx) {
        if (dx != 0 || dy != 0)
          device->drawTransformedText(QPointF(dx, dy), text);
      }
    }

    // draw text
    device->setPen(tc);

    device->drawTransformedText(QPointF(0, 0), text);
  }
  else {
    device->drawTransformedText(QPointF(0, 0), text);
  }

  if (device->type() != CQChartsPaintDevice::Type::SCRIPT)
    device->restore();
}

QRectF
bbox(double x, double y, const QString &text, const QFont &font, double angle, double border,
     Qt::Alignment align, bool alignBBox)
{
  QRectF bbox;
  Points points;

  bboxData(x, y, text, font, angle, border, bbox, points, align, alignBBox);

  return bbox;
}

Points
bboxPoints(double x, double y, const QString &text, const QFont &font, double angle, double border,
           Qt::Alignment align, bool alignBBox)
{
  QRectF bbox;
  Points points;

  bboxData(x, y, text, font, angle, border, bbox, points, align, alignBBox);

  return points;
}

void
bboxData(double x, double y, const QString &text, const QFont &font, double angle,
         double border, QRectF &bbox, Points &points, Qt::Alignment align, bool alignBBox)
{
  QFontMetricsF fm(font);

  //------

  double th = fm.height()    + 2*border;
  double tw = fm.width(text) + 2*border;

  double a1 = CMathUtil::Deg2Rad(angle);

  double c = cos(-a1);
  double s = sin(-a1);

  //---

  double tx = 0.0, ty = 0.0;

  if (! alignBBox) {
    double dx = 0.0, dy = 0.0;

    if      (align & Qt::AlignLeft)
      dx = -border;
    else if (align & Qt::AlignRight)
      dx = -tw + border;
    else if (align & Qt::AlignHCenter)
      dx = -tw/2.0;

    if      (align & Qt::AlignBottom)
      dy = border;
    else if (align & Qt::AlignTop)
      dy = th - border;
    else if (align & Qt::AlignVCenter)
      dy = th/2.0;

    //---

    tx = c*dx - s*dy;
    ty = s*dx + c*dy;
  }
  else {
    if      (align & Qt::AlignLeft)
      tx = -th*s - border;
    else if (align & Qt::AlignRight)
      tx = -tw*c + border;
    else if (align & Qt::AlignHCenter)
      tx = -(th*s + tw*c)/2.0;

    if      (align & Qt::AlignBottom)
      ty = border;
    else if (align & Qt::AlignTop)
      ty = -(tw*s - th*c) - border;
    else if (align & Qt::AlignVCenter)
      ty = -(tw*s - th*c)/2.0;
  }

  //------

  //x -= c*border - s*border;
  //y -= s*border + c*border;

  double x1 = x + tx, x2 = x + tw*c + tx, x3 = x + tw*c + th*s + tx, x4 = x + th*s + tx;
  double y1 = y + ty, y2 = y + tw*s + ty, y3 = y + tw*s - th*c + ty, y4 = y - th*c + ty;

  points.clear();

  points.push_back(QPointF(x1, y1));
  points.push_back(QPointF(x2, y2));
  points.push_back(QPointF(x3, y3));
  points.push_back(QPointF(x4, y4));

  //---

  double xmin = points[0].x(); double xmax = xmin;
  double ymin = points[0].y(); double ymax = ymin;

  for (int i = 1; i < 4; ++i) {
    xmin = std::min(xmin, points[i].x());
    ymin = std::min(ymin, points[i].y());
    xmax = std::max(xmax, points[i].x());
    ymax = std::max(ymax, points[i].y());
  }

  bbox = QRectF(xmin, ymin, xmax - xmin, ymax - ymin);
}

}
