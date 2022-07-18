#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsRotatedText.h>

CQChartsRotatedTextBoxObj::
CQChartsRotatedTextBoxObj(Plot *plot) :
 CQChartsTextBoxObj(plot)
{
}

void
CQChartsRotatedTextBoxObj::
draw(PaintDevice *device, const Point &center, const QString &text,
     double angle, Qt::Alignment align, bool isRadial) const
{
  draw(device, center, text, "", angle, align, isRadial);
}

void
CQChartsRotatedTextBoxObj::
draw(PaintDevice *device, const Point &center, const QString &text1, const QString &text2,
     double angle, Qt::Alignment align, bool isRadial) const
{
  auto textOptions = this->textOptions();

  textOptions.color = interpTextColor(ColorInd());
  textOptions.font  = textFont();
  textOptions.alpha = textAlpha();

  textOptions.angle = CQChartsAngle(angle);
  textOptions.align = align;

  PenBrush penBrush;

  calcPenBrush(penBrush);

  draw(device, center, text1, text2, isRadial, penBrush, textOptions,
       margin(), cornerSize(), borderSides(), bbox_);
}

void
CQChartsRotatedTextBoxObj::
draw(PaintDevice *device, const Point &center, const QString &text1, const QString &text2,
     bool isRadial, const PenBrush &penBrush, const TextOptions &textOptions,
     const Margin &margin, const Length &cornerSize, const Sides &borderSides, BBox &drawBBox)
{
  device->save();

  device->setPainterFont(textOptions.font);

  QFontMetricsF fm(device->font());

  // get text width
  double tw1 = fm.horizontalAdvance(text1);
  double tw2 = (text2 != "" ? fm.horizontalAdvance(text2) : 0.0);
  double th1 = fm.height();

  // add external margin
  double xlm = device->lengthPixelWidth (margin.left  ());
  double xrm = device->lengthPixelWidth (margin.right ());
  double ytm = device->lengthPixelHeight(margin.top   ());
  double ybm = device->lengthPixelHeight(margin.bottom());

  double twb = std::max(tw1, tw2) + xlm + xrm;
  double thb = th1 + ybm + ytm;

  if (text2 != "")
    thb += fm.height();

  //---

  auto pcenter = device->windowToPixel(center);

  double cx = pcenter.x;
  double cy = pcenter.y - thb/2;
  double cd = 0.0;

  if      (textOptions.align & Qt::AlignHCenter) {
    cx -= twb/2;
  }
  else if (textOptions.align & Qt::AlignRight) {
    cx -= twb;
    cd  = -xrm;
  }
  else {
    cd  = xlm;
  }

  BBox pbbox(cx, cy, cx + twb, cy + thb);

  drawBBox = device->pixelToWindow(pbbox);

  //---

  // draw box
  double cdx, cdy;

  if (textOptions.angle.isZero()) {
    CQChartsBoxObj::drawBox(device, drawBBox, penBrush, cornerSize, borderSides);

    cdx = cd;
    cdy = 0.0;

    if (text2 != "")
      cdy -= th1/2.0;
  }
  else {
    BBox                        pbbox;
    CQChartsRotatedText::Points ppoints;

    CQChartsGeom::Margin border(xlm, ytm, xrm, ybm);

    TextOptions options;

    options.angle = textOptions.angle;
    options.align = textOptions.align;

    CQChartsRotatedText::calcBBoxData(pcenter.x, pcenter.y, text1, device->font(), options, border,
                                      pbbox, ppoints, /*alignBBox*/true, isRadial);

    Polygon poly;

    for (std::size_t i = 0; i < ppoints.size(); ++i)
      poly.addPoint(device->pixelToWindow(ppoints[i]));

    CQChartsBoxObj::drawPolygon(device, poly, penBrush, cornerSize);

    //---

    double c = textOptions.angle.cos();
    double s = textOptions.angle.sin();

    cdx =  c*cd;
    cdy = -s*cd;
  }

  //---

  // draw text
  auto c = textOptions.color;

  CQChartsPenBrush tpenBrush;

  CQChartsUtil::setPen(tpenBrush.pen, true, c, textOptions.alpha);
  //setPen(tpenBrush, CQChartsPenData(true, c, textOptions.alpha));

  device->setPen(tpenBrush.pen);

  Point p1(pcenter.x + cdx, pcenter.y + cdy);

  auto textOptions1 = textOptions;

  CQChartsRotatedText::draw(device, device->pixelToWindow(p1), text1,
                            textOptions1, /*alignBBox*/true, isRadial);

  if (text2 != "") {
    p1 += Point(0.0, th1);

    CQChartsRotatedText::draw(device, device->pixelToWindow(p1), text2,
                              textOptions1, /*alignBBox*/true, isRadial);
  }

  device->restore();
}

// center in pixels
CQChartsGeom::BBox
CQChartsRotatedTextBoxObj::
bbox(const Point &pcenter, const QString &text, double angle, Qt::Alignment align,
     bool isRadial) const
{
  TextOptions textOptions;

  textOptions.font  = textFont();
  textOptions.alpha = textAlpha();

  textOptions.angle = CQChartsAngle(angle);
  textOptions.align = align;

  if (plot())
    return bbox(plot(), pcenter, text, isRadial, textOptions, margin());
  else
    return bbox(view(), pcenter, text, isRadial, textOptions, margin());
}

CQChartsGeom::BBox
CQChartsRotatedTextBoxObj::
bbox(Plot *plot, const Point &pcenter, const QString &text,
     bool isRadial, const TextOptions &textOptions, const Margin &margin)
{
  return bbox1(plot->view(), plot, pcenter, text, isRadial, textOptions, margin);
}

CQChartsGeom::BBox
CQChartsRotatedTextBoxObj::
bbox(View *view, const Point &pcenter, const QString &text,
     bool isRadial, const TextOptions &textOptions, const Margin &margin)
{
  return bbox1(view, nullptr, pcenter, text, isRadial, textOptions, margin);
}

CQChartsGeom::BBox
CQChartsRotatedTextBoxObj::
bbox1(View *view, Plot *plot, const Point &pcenter, const QString &text,
      bool isRadial, const TextOptions &textOptions, const Margin &margin)
{
  QFont font;

  if      (plot)
    font = plot->view()->plotFont(plot, textOptions.font);
  else if (view)
    font = view->viewFont(textOptions.font);

  QFontMetricsF fm(font);

  double tw = fm.horizontalAdvance(text);

  // external margin
  double xlm { 0.0 }, xrm { 0.0 }, ytm { 0.0 }, ybm { 0.0 };

  if     (plot) {
    xlm = plot->lengthPixelWidth (margin.left  ());
    xrm = plot->lengthPixelWidth (margin.right ());
    ytm = plot->lengthPixelHeight(margin.top   ());
    ybm = plot->lengthPixelHeight(margin.bottom());
  }
  else if (view) {
    xlm = view->lengthPixelWidth (margin.left  ());
    xrm = view->lengthPixelWidth (margin.right ());
    ytm = view->lengthPixelHeight(margin.top   ());
    ybm = view->lengthPixelHeight(margin.bottom());
  }

  double tw1 = tw + xlm + xrm;
  double th1 = fm.height() + ybm + ytm;

  double cx = pcenter.x;
  double cy = pcenter.y - th1/2;

  if      (textOptions.align & Qt::AlignHCenter) {
    cx -= tw1/2;
  }
  else if (textOptions.align & Qt::AlignRight) {
    cx -= tw1;
  }

  BBox pbbox;

  if (textOptions.angle.isZero()) {
    pbbox = BBox(cx, cy, cx + tw1, cy + th1);
  }
  else {
    CQChartsRotatedText::Points ppoints;

    CQChartsGeom::Margin border(xlm, ytm, xrm, ybm);

    TextOptions textOptions1;

    textOptions1.angle = textOptions.angle;
    textOptions1.align = textOptions.align;

    CQChartsRotatedText::calcBBoxData(pcenter.x, pcenter.y, text, font, textOptions1, border,
                                      pbbox, ppoints, /*alignBBox*/true, isRadial);
  }

  if      (plot)
    return plot->pixelToWindow(pbbox);
  else if (view)
    return view->pixelToWindow(pbbox);
  else
    return pbbox;
}

void
CQChartsRotatedTextBoxObj::
drawConnectedRadialText(PaintDevice *device, const Point &center, double ro, double lr,
                        const Angle &ta, const QString &text, const QPen &lpen, bool isRotated)
{
  Angle la = ta;

  auto textOptions = this->textOptions();

  textOptions.color = interpTextColor(ColorInd());
  textOptions.font  = textFont();
  textOptions.alpha = textAlpha();

  textOptions.angle = (isRotated ? ta : Angle());
  textOptions.align = Qt::AlignHCenter | Qt::AlignVCenter;

  PenBrush penBrush;

  calcPenBrush(penBrush);

  drawConnectedRadialText(device, center, ro, lr, la, text, lpen,
                          penBrush, textOptions, margin(), cornerSize(), borderSides());
}

void
CQChartsRotatedTextBoxObj::
drawConnectedRadialText(PaintDevice *device, const Point &center, double ro, double lr,
                        const Angle &la, const QString &text, const QPen &lpen,
                        const PenBrush &penBrush, const TextOptions &textOptions,
                        const Margin &margin, const Length &cornerSize, const Sides &borderSides)
{
  BBox tbbox;

  drawCalcConnectedRadialText(nullptr, nullptr, device, center, ro, lr, la, text, lpen,
                              penBrush, textOptions, margin, cornerSize, borderSides, tbbox);
}

void
CQChartsRotatedTextBoxObj::
calcConnectedRadialTextBBox(const Point &center, double ro, double lr, const Angle &ta,
                            const QString &text, bool isRotated, BBox &tbbox)
{
  Angle la = ta;

  TextOptions textOptions;

  textOptions.font = textFont();

  textOptions.angle      = (isRotated ? ta : Angle());
  textOptions.align      = Qt::AlignHCenter | Qt::AlignVCenter;
  textOptions.clipLength = lengthPixelWidth(textClipLength());

  if (plot())
    calcConnectedRadialTextBBox(plot(), center, ro, lr, la, text,
                                textOptions, margin(), tbbox);
  else
    calcConnectedRadialTextBBox(view(), center, ro, lr, la, text,
                                textOptions, margin(), tbbox);
}

void
CQChartsRotatedTextBoxObj::
calcConnectedRadialTextBBox(Plot *plot, const Point &center, double ro, double lr, const Angle &la,
                            const QString &text, const TextOptions &textOptions,
                            const Margin &margin, BBox &tbbox)
{
  QPen     lpen;
  PenBrush penBrush;
  Length   cornerSize;
  Sides    borderSides;

  drawCalcConnectedRadialText(plot->view(), plot, nullptr, center, ro, lr, la, text, lpen,
                              penBrush, textOptions, margin, cornerSize, borderSides, tbbox);
}

void
CQChartsRotatedTextBoxObj::
calcConnectedRadialTextBBox(View *view, const Point &center, double ro, double lr, const Angle &la,
                            const QString &text, const TextOptions &textOptions,
                            const Margin &margin, BBox &tbbox)
{
  QPen     lpen;
  PenBrush penBrush;
  Length   cornerSize;
  Sides    borderSides;

  drawCalcConnectedRadialText(view, nullptr, nullptr, center, ro, lr, la, text, lpen,
                              penBrush, textOptions, margin, cornerSize, borderSides, tbbox);
}

void
CQChartsRotatedTextBoxObj::
drawCalcConnectedRadialText(View *view, Plot *plot, PaintDevice *device, const Point &center,
                            double ro, double lr, const Angle &la, const QString &text,
                            const QPen &lpen, const PenBrush &penBrush,
                            const TextOptions &textOptions, const Margin &margin,
                            const Length &cornerSize, const Sides &borderSides, BBox &tbbox)
{
  if (device)
    device->save();

  //---

  auto windowToPixel = [&](const Point &p) {
    if (device) return device->windowToPixel(p);
    if (plot  ) return plot  ->windowToPixel(p);
    if (view  ) return view  ->windowToPixel(p);
    return p;
  };

  auto pixelToWindow = [&](const Point &p) {
    if (device) return device->pixelToWindow(p);
    if (plot  ) return plot  ->pixelToWindow(p);
    if (view  ) return view  ->pixelToWindow(p);
    return p;
  };

  auto isInvertX = [&]() {
    if (device && device->plot()) return device->plot()->isInvertX();
    if (plot) return plot->isInvertX();
    return false;
  };

  bool labelRight = (la.cos() >= 0.0);

  auto p = CQChartsAngle::circlePoint(center, lr, la);

  auto pt = windowToPixel(p);

  //---

  double dx    = 0.0;
  auto   align = textOptions.align;

  // connect text to outer edge
  if (lr > ro) {
    auto pl1 = CQChartsAngle::circlePoint(center, ro, la);
    auto pl2 = CQChartsAngle::circlePoint(center, lr, la);

    auto lp1 = windowToPixel(pl1);
    auto lp2 = windowToPixel(pl2);

    int tickSize = 16; // TODO: allow customize

    if (isInvertX())
      labelRight = ! labelRight;

    dx    = tickSize;
    align = Qt::AlignLeft | Qt::AlignVCenter;

    if (! labelRight) {
      dx    = -tickSize;
      align = Qt::AlignRight | Qt::AlignVCenter;
    }

    if (device) {
      device->setPen(lpen);

      Point lp3(lp2.x + dx, lp2.y);

      auto l1 = pixelToWindow(lp1);
      auto l2 = pixelToWindow(lp2);
      auto l3 = pixelToWindow(lp3);

      device->drawLine(l1, l2);
      device->drawLine(l2, l3);
    }
  }

  //---

  // draw text
  Point pt1(pt.x + dx, pt.y);

  auto textOptions1 = textOptions;

  textOptions1.align = align;

  bool isRadial = (la == textOptions1.angle);

  if (isRadial && ! labelRight)
    textOptions1.angle.flipX();

  if (device) {
    auto t1 = pixelToWindow(pt1);

    draw(device, t1, text, "", isRadial, penBrush, textOptions1,
         margin, cornerSize, borderSides, tbbox);
  }
  else {
    if      (plot)
      tbbox = bbox(plot, pt1, text, isRadial, textOptions1, margin);
    else if (view)
      tbbox = bbox(view, pt1, text, isRadial, textOptions1, margin);
  }

  //---

  if (device)
    device->restore();
}
