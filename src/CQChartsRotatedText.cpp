#include <CQChartsRotatedText.h>
#include <CQChartsTextOptions.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsUtil.h>

#include <cmath>

namespace CQChartsRotatedText {

void
drawInBox(CQChartsPaintDevice *device, const BBox &rect, const QString &text,
          const CQChartsTextOptions &options, bool /*alignBBox*/, bool /*isRadial*/)
{
  auto a1 = -options.angle;

  double c = a1.cos();
  double s = a1.sin();

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

  double px = 0.0;
  double py = 0.0;

  auto prect  = device->windowToPixel(rect);
  auto prect1 = calcBBox(px, py, text, device->font(), options, 0, /*alignBBox*/true);

  //---

  if (options.scaled) {
    // calc font to fit in passed rect
    double stw  = prect .getWidth ();
    double sth  = prect .getHeight();
    double stw1 = prect1.getWidth ();
    double sth1 = prect1.getHeight();

    double sx = (stw1 > 0 ? stw/stw1 : 1);
    double sy = (sth1 > 0 ? sth/sth1 : 1);

    double scale = std::min(sx, sy);

    device->setFont(CQChartsUtil::scaleFontSize(
      device->font(), scale, options.minScaleFontSize, options.maxScaleFontSize));

    double px1 = 0.0;
    double py1 = 0.0;

    prect1 = calcBBox(px1, py1, text, device->font(), options, 0, /*alignBBox*/true);

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

  double pdx = prect.getWidth () - prect1.getWidth ();
  double pdy = prect.getHeight() - prect1.getHeight();

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

  drawDelta(device, rect.getCenter(), text, options, tx, ty, ax, ay);
}

void
draw(CQChartsPaintDevice *device, const Point &p, const QString &text,
     const CQChartsTextOptions &options, bool alignBBox, bool isRadial)
{
  QFontMetricsF fm(device->font());

  double th = fm.height();
  double tw = fm.width(text);

  auto a1 = -options.angle;

  double c = a1.cos();
  double s = a1.sin();

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

  if (isRadial) {
    if (options.align & Qt::AlignLeft) {
      tx = 0.0;
      ty = 0.0;
    }
    else {
      tx = -tw*c;
      ty = -tw*s;
    }

    tx += -th*s/2.0;
    ty +=  th*c/2.0;
  }

  //---

  double ax = -s*fm.descent();
  double ay =  c*fm.descent();

  //---

  drawDelta(device, p, text, options, tx, ty, ax, ay);
}

// p is in window coords, tx, ty, ax, ay are pixel coords
void
drawDelta(CQChartsPaintDevice *device, const Point &p, const QString &text,
          const CQChartsTextOptions &options, double tx, double ty, double ax, double ay)
{
  if (device->isInteractive())
    device->save();

  //---

  auto pp = device->windowToPixel(p);

  Point pt(pp.x + tx - ax, pp.y + ty - ay);

  auto pt1 = device->pixelToWindow(pt);

  device->setTransformRotate(pt1, options.angle.value());

  if (options.contrast) {
    auto tc = device->pen().color();

  //auto icolor = CQChartsUtil::invColor(tc);
    auto icolor = CQChartsUtil::bwColor(tc);

    // draw contrast outline
    device->setPen(CQChartsDrawUtil::setColorAlpha(icolor, options.contrastAlpha));

    for (int dy = -2; dy <= 2; ++dy) {
      for (int dx = -2; dx <= 2; ++dx) {
        if (dx != 0 || dy != 0)
          device->drawTransformedText(Point(dx, dy), text);
      }
    }

    // draw text
    device->setPen(tc);

    device->drawTransformedText(Point(0.0, 0.0), text);
  }
  else {
    device->drawTransformedText(Point(0.0, 0.0), text);
  }

  //---

  if (device->isInteractive())
    device->restore();
}

CQChartsGeom::BBox
calcBBox(double px, double py, const QString &text, const QFont &font,
         const CQChartsTextOptions &options, double border, bool alignBBox, bool isRadial)
{
  BBox   pbbox;
  Points ppoints;

  Margin border1(border);

  calcBBoxData(px, py, text, font, options, border1, pbbox, ppoints, alignBBox, isRadial);

  return pbbox;
}

Points
bboxPoints(double px, double py, const QString &text, const QFont &font,
           const CQChartsTextOptions &options, double border, bool alignBBox, bool isRadial)
{
  BBox   pbbox;
  Points ppoints;

  Margin border1(border);

  calcBBoxData(px, py, text, font, options, border1, pbbox, ppoints, alignBBox, isRadial);

  return ppoints;
}

void
calcBBoxData(double px, double py, const QString &text, const QFont &font,
             const CQChartsTextOptions &options, double border, BBox &pbbox, Points &ppoints,
             bool alignBBox, bool isRadial)
{
  Margin border1(border);

  calcBBoxData(px, py, text, font, options, border1, pbbox, ppoints, alignBBox, isRadial);
}

void
calcBBoxData(double px, double py, const QString &text, const QFont &font,
             const CQChartsTextOptions &options, const Margin &border, BBox &pbbox,
             Points &ppoints, bool alignBBox, bool isRadial)
{
  QFontMetricsF fm(font);

  //------

  double xlm = border.left  ();
  double xrm = border.right ();
  double ytm = border.top   ();
  double ybm = border.bottom();

  double th = fm.height()    + xlm + xrm;
  double tw = fm.width(text) + ybm + ytm;

  auto a1 = -options.angle;

  double c = a1.cos();
  double s = a1.sin();

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

  //---

  if (isRadial) {
    if (options.align & Qt::AlignLeft) {
      tx = 0.0;
      ty = 0.0;
    }
    else {
      tx = -tw*c;
      ty = -tw*s;
    }

    tx += -th*s/2.0;
    ty +=  th*c/2.0;
  }

  //------

  double x1 = px + tx, x2 = px + tw*c + tx, x3 = px + tw*c + th*s + tx, x4 = px + th*s + tx;
  double y1 = py + ty, y2 = py + tw*s + ty, y3 = py + tw*s - th*c + ty, y4 = py - th*c + ty;

  ppoints.clear();

  ppoints.emplace_back(x1, y1);
  ppoints.emplace_back(x2, y2);
  ppoints.emplace_back(x3, y3);
  ppoints.emplace_back(x4, y4);

  //---

  double xmin = ppoints[0].x; double xmax = xmin;
  double ymin = ppoints[0].y; double ymax = ymin;

  for (int i = 1; i < 4; ++i) {
    xmin = std::min(xmin, ppoints[i].x);
    ymin = std::min(ymin, ppoints[i].y);
    xmax = std::max(xmax, ppoints[i].x);
    ymax = std::max(ymax, ppoints[i].y);
  }

  pbbox = BBox(xmin, ymin, xmax, ymax);
}

}
