#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsRotatedText.h>

CQChartsRotatedTextBoxObj::
CQChartsRotatedTextBoxObj(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
{
}

void
CQChartsRotatedTextBoxObj::
draw(CQChartsPaintDevice *device, const CQChartsGeom::Point &center,
     const QString &text, double angle, Qt::Alignment align, bool isRotated) const
{
  device->save();

  setPainterFont(device, textFont());

  QFontMetricsF fm(device->font());

  double tw = fm.width(text);

  // external margin
  double xlm = lengthPixelWidth (margin().left  ());
  double xrm = lengthPixelWidth (margin().right ());
  double ytm = lengthPixelHeight(margin().top   ());
  double ybm = lengthPixelHeight(margin().bottom());

  double tw1 = tw + xlm + xrm;
  double th1 = fm.height() + ybm + ytm;

  auto pcenter = device->windowToPixel(center);

  double cx = pcenter.x;
  double cy = pcenter.y - th1/2;
  double cd = 0.0;

  if      (align & Qt::AlignHCenter) {
    cx -= tw1/2;
  }
  else if (align & Qt::AlignRight) {
    cx -= tw1;
    cd  = -xrm;
  }
  else {
    cd  = xlm;
  }

  CQChartsGeom::BBox pbbox(cx, cy, cx + tw1, cy + th1);

  bbox_ = device->pixelToWindow(pbbox);

  //---

  // draw box
  double cdx, cdy;

  if (CMathUtil::isZero(angle)) {
    CQChartsBoxObj::draw(device, bbox_);

    cdx = cd;
    cdy = 0.0;
  }
  else {
    CQChartsGeom::BBox          pbbox;
    CQChartsRotatedText::Points ppoints;

    CQChartsGeom::Margin border(xlm, ytm, xrm, ybm);

    CQChartsTextOptions options;

    options.angle = CQChartsAngle(angle);
    options.align = align;

    CQChartsRotatedText::calcBBoxData(pcenter.x, pcenter.y, text, device->font(), options, border,
                                      pbbox, ppoints, /*alignBBox*/true, /*radial*/isRotated);

    CQChartsGeom::Polygon poly;

    for (std::size_t i = 0; i < ppoints.size(); ++i)
      poly.addPoint(pixelToWindow(ppoints[i]));

    CQChartsBoxObj::draw(device, poly);

    //---

    double tangle = CMathUtil::Deg2Rad(angle);

    double c = cos(tangle);
    double s = sin(tangle);

    cdx =  c*cd;
    cdy = -s*cd;
  }

  //---

  // draw text
  QColor c = interpTextColor(ColorInd());

  QPen pen;

  setPen(pen, true, c, textAlpha());

  device->setPen(pen);

  CQChartsGeom::Point p1(pcenter.x + cdx, pcenter.y + cdy);

  CQChartsTextOptions options;

  options.angle         = CQChartsAngle(angle);
  options.align         = align;
  options.contrast      = isTextContrast();
  options.contrastAlpha = textContrastAlpha();

  CQChartsRotatedText::draw(device, device->pixelToWindow(p1), text,
                            options, /*alignBBox*/true, /*radial*/isRotated);

  device->restore();
}

// center in pixels
CQChartsGeom::BBox
CQChartsRotatedTextBoxObj::
bbox(const CQChartsGeom::Point &pcenter, const QString &text, double angle,
     Qt::Alignment align, bool isRotated) const
{
  QFont font = calcFont(textFont());

  QFontMetricsF fm(font);

  double tw = fm.width(text);

  // external margin
  double xlm = lengthPixelWidth (margin().left  ());
  double xrm = lengthPixelWidth (margin().right ());
  double ytm = lengthPixelHeight(margin().top   ());
  double ybm = lengthPixelHeight(margin().bottom());

  double tw1 = tw + xlm + xrm;
  double th1 = fm.height() + ybm + ytm;

  double cx = pcenter.x;
  double cy = pcenter.y - th1/2;

  if      (align & Qt::AlignHCenter) {
    cx -= tw1/2;
  }
  else if (align & Qt::AlignRight) {
    cx -= tw1;
  }

  CQChartsGeom::BBox pbbox;

  if (angle != 0.0) {
    pbbox = CQChartsGeom::BBox(cx, cy, cx + tw1, cy + th1);
  }
  else {
    CQChartsRotatedText::Points ppoints;

    CQChartsGeom::Margin border(xlm, ytm, xrm, ybm);

    CQChartsTextOptions options;

    options.angle = CQChartsAngle(angle);
    options.align = align;

    CQChartsRotatedText::calcBBoxData(pcenter.x, pcenter.y, text, font, options, border,
                                      pbbox, ppoints, /*alignBBox*/true, /*radial*/isRotated);
  }

  return pixelToWindow(pbbox);
}

void
CQChartsRotatedTextBoxObj::
drawConnectedRadialText(CQChartsPaintDevice *device, const CQChartsGeom::Point &center,
                        double ro, double lr, double ta, const QString &text,
                        const QPen &lpen, bool isRotated)
{
  CQChartsGeom::BBox tbbox;

  drawCalcConnectedRadialText(device, center, ro, lr, ta, text, lpen, isRotated, tbbox);
}

void
CQChartsRotatedTextBoxObj::
calcConnectedRadialTextBBox(const CQChartsGeom::Point &center, double ro, double lr, double ta,
                            const QString &text, bool isRotated, CQChartsGeom::BBox &tbbox)
{
  QPen lpen;

  drawCalcConnectedRadialText((CQChartsPaintDevice *) 0, center, ro, lr, ta,
                              text, lpen, isRotated, tbbox);
}

void
CQChartsRotatedTextBoxObj::
drawCalcConnectedRadialText(CQChartsPaintDevice *device, const CQChartsGeom::Point &center,
                            double ro, double lr, double ta, const QString &text,
                            const QPen &lpen, bool isRotated, CQChartsGeom::BBox &tbbox)
{
  if (device)
    device->save();

  //---

  double tangle = CMathUtil::Deg2Rad(ta);

  double tc = cos(tangle);
  double ts = sin(tangle);

  double tx = center.x + lr*tc;
  double ty = center.y + lr*ts;

  auto pt = windowToPixel(CQChartsGeom::Point(tx, ty));

  //---

  double        dx    = 0.0;
  Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

  // connect text to outer edge
  if (lr > ro) {
    double lx1 = center.x + ro*tc;
    double ly1 = center.y + ro*ts;
    double lx2 = center.x + lr*tc;
    double ly2 = center.y + lr*ts;

    auto lp1 = windowToPixel(CQChartsGeom::Point(lx1, ly1));
    auto lp2 = windowToPixel(CQChartsGeom::Point(lx2, ly2));

    int tickSize = 16; // TODO: allow customize

    bool labelRight = (tc >= 0);

    if (plot() && plot()->isInvertX())
      labelRight = ! labelRight;

    if (labelRight) {
      dx    = tickSize;
      align = Qt::AlignLeft | Qt::AlignVCenter;
    }
    else {
      dx    = -tickSize;
      align = Qt::AlignRight | Qt::AlignVCenter;
    }

    if (device) {
      device->setPen(lpen);

      CQChartsGeom::Point lp3(lp2.x + dx, lp2.y);

      auto l1 = device->pixelToWindow(lp1);
      auto l2 = device->pixelToWindow(lp2);
      auto l3 = device->pixelToWindow(lp3);

      device->drawLine(l1, l2);
      device->drawLine(l2, l3);
    }
  }

  //---

  // draw text
  CQChartsGeom::Point pt1(pt.x + dx, pt.y);

  double angle = 0.0;

  if (isRotated)
    angle = (tc >= 0 ? ta : 180.0 + ta);

  auto t1 = (device ? device->pixelToWindow(pt1) : pt1);

  if (device)
    draw(device, t1, text, angle, align, isRotated);
  else
    tbbox = this->bbox(t1, text, angle, align, isRotated);

  //---

  if (device)
    device->restore();
}
