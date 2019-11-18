#include <CQChartsRotatedText.h>
#include <CQChartsTextOptions.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsUtil.h>

#include <cmath>

namespace CQChartsRotatedText {

void
drawInBox(CQChartsPaintDevice *device, const QRectF &rect, const QString &text,
          const CQChartsTextOptions &options, bool /*alignBBox*/)
{
  double a1 = CMathUtil::Deg2Rad(options.angle);

  double c = cos(-a1);
  double s = sin(-a1);

  //---

  QFontMetricsF fm(device->font());

  double th = fm.height();
  double tw = fm.width(text);

  //---

  double dx = -tw/2.0;
  double dy =  th/2.0;

  double tx = c*dx - s*dy;
  double ty = s*dx + c*dy;

  //---

  double ax = -s*fm.descent();
  double ay =  c*fm.descent();

  //---

  QRectF prect  = device->windowToPixel(rect);
  QRectF prect1 = calcBBox(0.0, 0.0, text, device->font(), options, 0, /*alignBBox*/ true);

  //---

  if (options.scaled) {
    // calc font to fit in passed rect
    double stw  = prect .width ();
    double sth  = prect .height();
    double stw1 = prect1.width ();
    double sth1 = prect1.height();

    double sx = (stw1 > 0 ? stw/stw1 : 1);
    double sy = (sth1 > 0 ? sth/sth1 : 1);

    double scale = std::min(sx, sy);

    device->setFont(CQChartsUtil::scaleFontSize(
      device->font(), scale, options.minScaleFontSize, options.maxScaleFontSize));

    prect1 = calcBBox(0.0, 0.0, text, device->font(), options, 0, /*alignBBox*/ true);

    //--

    fm = QFontMetricsF(device->font());

    th = fm.height();
    tw = fm.width(text);

    dx = -tw/2.0;
    dy =  th/2.0;

    tx = c*dx - s*dy;
    ty = s*dx + c*dy;

    ax = -s*fm.descent();
    ay =  c*fm.descent();
  }

  //---

  double ptx = 0.0, pty = 0.0;

  double pdx = prect.width () - prect1.width ();
  double pdy = prect.height() - prect1.height();

  if      (options.align & Qt::AlignLeft)
    ptx = -pdx/2.0;
  else if (options.align & Qt::AlignRight)
    ptx = pdx/2.0;
  else
    ptx = 0.0;

  if      (options.align & Qt::AlignBottom)
    pty = pdy/2.0;
  else if (options.align & Qt::AlignTop)
    pty = -pdy/2.0;
  else
    pty = 0.0;

  tx += ptx;
  ty += pty;

  //---

  drawDelta(device, rect.center(), text, options, tx, ty, ax, ay);
}

void
draw(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
     const CQChartsTextOptions &options, bool alignBBox)
{
  QFontMetricsF fm(device->font());

  double th = fm.height();
  double tw = fm.width(text);

  double a1 = CMathUtil::Deg2Rad(options.angle);

  double c = cos(-a1);
  double s = sin(-a1);

  //---

  double tx = 0.0, ty = 0.0;

  if (! alignBBox) {
    double dx = 0.0, dy = 0.0;

    if      (options.align & Qt::AlignLeft)
      dx = 0.0;
    else if (options.align & Qt::AlignRight)
      dx = -tw;
    else if (options.align & Qt::AlignHCenter)
      dx = -tw/2.0;

    if      (options.align & Qt::AlignBottom)
      dy = 0.0;
    else if (options.align & Qt::AlignTop)
      dy = th;
    else if (options.align & Qt::AlignVCenter)
      dy = th/2.0;

    tx = c*dx - s*dy;
    ty = s*dx + c*dy;
  }
  else {
    if      (options.align & Qt::AlignLeft)
      tx = -th*s;
    else if (options.align & Qt::AlignRight)
      tx = -tw*c;
    else if (options.align & Qt::AlignHCenter)
      tx = -(th*s + tw*c)/2.0;

    if      (options.align & Qt::AlignBottom)
      ty = 0.0;
    else if (options.align & Qt::AlignTop)
      ty = -(tw*s - th*c);
    else if (options.align & Qt::AlignVCenter)
      ty = -(tw*s - th*c)/2;
  }

  //---

  double ax = -s*fm.descent();
  double ay =  c*fm.descent();

  //---

  drawDelta(device, p, text, options, tx, ty, ax, ay);
}

void
drawDelta(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
          const CQChartsTextOptions &options, double tx, double ty, double ax, double ay)
{
  if (device->isInteractive())
    device->save();

  //---

  QPointF pp = device->windowToPixel(p);

  QPointF pt(pp.x() + tx - ax, pp.y() + ty - ay);

  QPointF pt1 = device->pixelToWindow(pt);

  device->setTransformRotate(pt1, options.angle);

  if (options.contrast) {
    QColor tc = device->pen().color();

  //QColor icolor = CQChartsUtil::invColor(tc);
    QColor icolor = CQChartsUtil::bwColor(tc);

    icolor.setAlphaF(options.contrastAlpha);

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

    device->drawTransformedText(QPointF(0.0, 0.0), text);
  }
  else {
    device->drawTransformedText(QPointF(0.0, 0.0), text);
  }

  //---

  if (device->isInteractive())
    device->restore();
}

QRectF
calcBBox(double x, double y, const QString &text, const QFont &font,
         const CQChartsTextOptions &options, double border, bool alignBBox)
{
  QRectF bbox;
  Points points;

  CQChartsGeom::Margin border1(border);

  calcBBoxData(x, y, text, font, options, border1, bbox, points, alignBBox);

  return bbox;
}

Points
bboxPoints(double x, double y, const QString &text, const QFont &font,
           const CQChartsTextOptions &options, double border, bool alignBBox)
{
  QRectF bbox;
  Points points;

  CQChartsGeom::Margin border1(border);

  calcBBoxData(x, y, text, font, options, border1, bbox, points, alignBBox);

  return points;
}

void
calcBBoxData(double x, double y, const QString &text, const QFont &font,
             const CQChartsTextOptions &options, double border, QRectF &bbox, Points &points,
             bool alignBBox)
{
  CQChartsGeom::Margin border1(border);

  calcBBoxData(x, y, text, font, options, border1, bbox, points, alignBBox);
}

void
calcBBoxData(double x, double y, const QString &text, const QFont &font,
             const CQChartsTextOptions &options, const CQChartsGeom::Margin &border,
             QRectF &bbox, Points &points, bool alignBBox)
{
  QFontMetricsF fm(font);

  //------

  double xlm = border.left  ();
  double xrm = border.right ();
  double ytm = border.top   ();
  double ybm = border.bottom();

  double th = fm.height()    + xlm + xrm;
  double tw = fm.width(text) + ybm + ytm;

  double a1 = CMathUtil::Deg2Rad(options.angle);

  double c = cos(-a1);
  double s = sin(-a1);

  //---

  double tx = 0.0, ty = 0.0;

  if (! alignBBox) {
    double dx = 0.0, dy = 0.0;

    if      (options.align & Qt::AlignLeft)
      dx = -xlm;
    else if (options.align & Qt::AlignRight)
      dx = -tw + xrm;
    else if (options.align & Qt::AlignHCenter)
      dx = -tw/2.0;

    if      (options.align & Qt::AlignBottom)
      dy = ybm;
    else if (options.align & Qt::AlignTop)
      dy = th - ytm;
    else if (options.align & Qt::AlignVCenter)
      dy = th/2.0;

    //---

    tx = c*dx - s*dy;
    ty = s*dx + c*dy;
  }
  else {
    if      (options.align & Qt::AlignLeft)
      tx = -th*s - xlm;
    else if (options.align & Qt::AlignRight)
      tx = -tw*c + xrm;
    else if (options.align & Qt::AlignHCenter)
      tx = -(th*s + tw*c)/2.0;

    if      (options.align & Qt::AlignBottom)
      ty = ybm;
    else if (options.align & Qt::AlignTop)
      ty = -(tw*s - th*c) - ytm;
    else if (options.align & Qt::AlignVCenter)
      ty = -(tw*s - th*c)/2.0;
  }

  //------

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
