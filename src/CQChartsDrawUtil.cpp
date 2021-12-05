#include <CQChartsDrawUtil.h>
#include <CQChartsRotatedText.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsUtil.h>

#include <CMathUtil.h>

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

namespace CQChartsDrawUtil {

void
setPenBrush(CQChartsPaintDevice *device, const PenBrush &penBrush)
{
  device->setPen      (penBrush.pen);
  device->setBrush    (penBrush.brush);
  device->setAltColor (penBrush.altColor);
  device->setAltAlpha (penBrush.altAlpha);
  device->setFillAngle(penBrush.fillAngle);
}

void
setPenBrush(QPainter *painter, const PenBrush &penBrush)
{
  painter->setPen  (penBrush.pen);
  painter->setBrush(penBrush.brush);
}

void
setBrush(QBrush &brush, const BrushData &data)
{
  CQChartsUtil::setBrush(brush, data.isVisible(), data.color(), data.alpha(), data.pattern());
}

//---

void
drawDotLine(PaintDevice *device, const PenBrush &penBrush, const BBox &bbox,
            const Length &lineWidth, bool horizontal, const Symbol &symbolType,
            const Length &symbolSize, const Angle &angle)
{
  // draw solid line
  double lw = (! horizontal ? device->lengthPixelWidth (lineWidth) :
                              device->lengthPixelHeight(lineWidth));

  auto c = bbox.getCenter();

  if (! horizontal) {
    if (lw < 3 && angle.isZero()) {
      setPenBrush(device, penBrush);

      auto p1 = Point(c.x, bbox.getYMin());
      auto p2 = Point(c.x, bbox.getYMax());

      device->drawLine(p1, p2);
    }
    else {
      auto pbbox = device->windowToPixel(bbox);

      double xc = pbbox.getXMid();

      BBox pbbox1(xc - lw/2.0, pbbox.getYMin(), xc + lw/2.0, pbbox.getYMax());

      drawRoundedRect(device, penBrush, device->pixelToWindow(pbbox1),
                      Length(), Sides(Sides::Side::ALL), angle);
    }
  }
  else {
    if (lw < 3 && angle.isZero()) {
      setPenBrush(device, penBrush);

      auto p1 = Point(bbox.getXMin(), c.y);
      auto p2 = Point(bbox.getXMax(), c.y);

      device->drawLine(p1, p2);
    }
    else {
      auto pbbox = device->windowToPixel(bbox);

      double yc = pbbox.getYMid();

      BBox pbbox1(pbbox.getXMid(), yc - lw/2.0, pbbox.getXMax(), yc + lw/2.0);

      drawRoundedRect(device, penBrush, device->pixelToWindow(pbbox1),
                      Length(), Sides(Sides::Side::ALL), angle);
    }
  }

  //---

  // draw dot
  Point p;

  if (! horizontal)
    p = Point(bbox.getXMid(), bbox.getYMax());
  else
    p = Point(bbox.getXMax(), bbox.getYMid());

  if (! angle.isZero())
    p = p.rotate(c, angle.radians());

  drawSymbol(device, penBrush, symbolType, p, symbolSize);
}

//---

void
drawRoundedRect(PaintDevice *device, const PenBrush &penBrush, const BBox &bbox,
                const Length &cornerSize, const Sides &sides, const Angle &angle)
{
  setPenBrush(device, penBrush);

  drawRoundedRect(device, bbox, cornerSize, cornerSize, sides, angle);
}

void
drawRoundedRect(PaintDevice *device, const BBox &bbox,
                const Length &cornerSize, const Sides &sides, const Angle &angle)
{
  drawRoundedRect(device, bbox, cornerSize, cornerSize, sides, angle);
}

void
drawRoundedRect(PaintDevice *device, const BBox &bbox, const Length &xCornerSize,
                const Length &yCornerSize, const Sides &sides, const Angle &angle)
{
  static double minSize1 = 2.5; // pixels
  static double minSize2 = 1.5; // pixels

  auto pbbox = device->windowToPixel(bbox);

  double minSize = pbbox.getMinSize();

  double xsize = device->lengthWindowWidth (xCornerSize);
  double ysize = device->lengthWindowHeight(yCornerSize);

  if      (minSize >= minSize1) {
    drawAdjustedRoundedRect(device, bbox, xsize, ysize, sides, angle);
  }
  else if (minSize >= minSize2) {
    auto pen = device->pen();

    auto pc = pen.color();
    auto f  = (minSize - minSize2)/(minSize1 - minSize2);

    pc.setAlphaF(f*pc.alphaF());

    pen.setColor(pc);

    device->setPen(pen);

    drawAdjustedRoundedRect(device, bbox, xsize, ysize, sides, angle);
  }
  else {
    auto bc = device->brush().color();

    device->setPen(bc);
    device->setBrush(Qt::NoBrush);

    if (pbbox.getWidth() > pbbox.getHeight())
      device->drawLine(Point(bbox.getXMin(), bbox.getYMid()),
                       Point(bbox.getXMax(), bbox.getYMid()));
    else
      device->drawLine(Point(bbox.getXMid(), bbox.getYMin()),
                       Point(bbox.getXMid(), bbox.getYMax()));
  }
}

void
drawAdjustedRoundedRect(PaintDevice *device, const BBox &bbox, double xsize, double ysize,
                        const CQChartsSides &sides, const Angle &angle)
{
  // draw rounded rect if corners
  if (xsize > 0 || ysize > 0) {
    // TODO: support sides with corners
    QPainterPath path;

    path.addRoundedRect(bbox.qrect(), xsize, ysize);

    if (! angle.isZero())
      device->drawPath(rotatePath(path, angle.degrees()));
    else
      device->drawPath(path);
  }
  // draw rect if no corners
  else {
    QPainterPath path;

    if (sides.isAll()) {
      if (! angle.isZero()) {
        QPainterPath path;

#if 0
        auto s = std::sqrt(2);

        auto w = bbox.getWidth ()*s;
        auto h = bbox.getHeight()*s;

        BBox bbox1(bbox.getXMid() - w/2.0, bbox.getYMid() - h/2.0,
                   bbox.getXMid() + w/2.0, bbox.getYMid() + h/2.0);

        polygonSidesPath(path, bbox1, 4);

        device->drawPath(rotatePath(path, angle.degrees() + 45));
#else
        auto x1 = bbox.getXMin(), y1 = bbox.getYMin();
        auto x2 = bbox.getXMax(), y2 = bbox.getYMax();

        path.moveTo(x1, y1);
        path.lineTo(x2, y1);
        path.lineTo(x2, y2);
        path.lineTo(x1, y2);

        path.closeSubpath();

        device->drawPath(rotatePath(path, angle.degrees()));
#endif
      }
      else
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
drawRoundedPolygon(PaintDevice *device, const PenBrush &penBrush, const Polygon &poly,
                   const Length &cornerSize, const Angle &angle)
{
  setPenBrush(device, penBrush);

  drawRoundedPolygon(device, poly, cornerSize, cornerSize, angle);
}

void
drawRoundedPolygon(PaintDevice *device, const Polygon &poly, const Length &cornerSize,
                   const Angle &angle)
{
  drawRoundedPolygon(device, poly, cornerSize, cornerSize, angle);
}

void
drawRoundedPolygon(PaintDevice *device, const Polygon &poly, const Length &xCornerSize,
                   const Length &yCornerSize, const Angle &)
{
  static double minSize = 2.5; // pixels

  auto bbox = poly.boundingBox();

  double pw = device->lengthPixelWidth (Length(bbox.getWidth (), device->parentUnits()));
  double ph = device->lengthPixelHeight(Length(bbox.getHeight(), device->parentUnits()));

  if (pw > minSize && ph > minSize) {
    double xsize = device->lengthWindowWidth (xCornerSize);
    double ysize = device->lengthWindowHeight(yCornerSize);

    QPainterPath path;

    if (! roundedPolygonPath(path, poly, xsize, ysize)) {
      device->drawPolygon(poly);
      return;
    }

    device->drawPath(path);
  }
  else {
    auto bc = device->brush().color();

    device->setPen(bc);
    device->setBrush(Qt::NoBrush);

    if (bbox.getWidth() > minSize) // horizontal line
      device->drawLine(Point(bbox.getXMin(), bbox.getYMid()),
                       Point(bbox.getXMax(), bbox.getYMin()));
    else                           // vertical line
      device->drawLine(Point(bbox.getXMid(), bbox.getYMin()),
                       Point(bbox.getXMid(), bbox.getYMax()));
  }
}

//------

void
drawTextInBox(PaintDevice *device, const BBox &rect,
              const QString &text, const TextOptions &options)
{
  assert(rect.isValid());

  if (! rect.isValid())
    return;

  //---

  // handle html separately
  if (options.html) {
    if (options.scaled)
      CQChartsDrawPrivate::drawScaledHtmlText(device, rect, text, options);
    else
      CQChartsDrawPrivate::drawHtmlText(device, rect.getCenter(), rect, text, options);

    return;
  }

  //---

  auto pen = device->pen();

  if (options.clipped)
    device->save();

  if (options.angle.isZero()) {
    auto text1 = clipTextToLength(text, device->font(), options.clipLength, options.clipElide);

    if (options.clipped)
      device->setClipRect(rect, Qt::IntersectClip);

    device->setPen(pen);

    //---

    QStringList strs;

    if (options.formatted) {
      auto prect = device->windowToPixel(rect);

      CQChartsUtil::formatStringInRect(text1, device->font(), prect, strs,
                                       CQChartsUtil::FormatData(options.formatSeps));
    }
    else
      strs << text1;

    //---

    drawStringsInBox(device, rect, strs, options);
  }
  else {
    if (options.clipped)
      device->setClipRect(rect, Qt::IntersectClip);

    drawRotatedTextInBox(device, rect, text, pen, options);
  }

  if (options.clipped)
    device->restore();
}

void
drawStringsInBox(PaintDevice *device, const BBox &rect,
                 const QStringList &strs, const TextOptions &options)
{
  auto prect = device->windowToPixel(rect);

  QFontMetricsF fm(device->font());

  double th = strs.size()*fm.height() + 2*options.margin;

  if (options.scaled) {
    // calc text scale
    double s = options.scale;

    if (s <= 0.0) {
      double tw = 0;

      for (int i = 0; i < strs.size(); ++i)
        tw = std::max(tw, fm.width(strs[i]));

      tw += 2*options.margin;

      double sx = (tw > 0 ? prect.getWidth ()/tw : 1);
      double sy = (th > 0 ? prect.getHeight()/th : 1);

      s = std::min(sx, sy);
    }

    // scale font
    device->setFont(CQChartsUtil::scaleFontSize(
      device->font(), s, options.minScaleFontSize, options.maxScaleFontSize));

    fm = QFontMetricsF(device->font());

    th = strs.size()*fm.height();
  }

  //---

  double dy = 0.0;

  if      (options.align & Qt::AlignVCenter)
    dy = (prect.getHeight() - th)/2.0;
  else if (options.align & Qt::AlignBottom)
    dy = prect.getHeight() - th;

  double y = prect.getYMin() + dy + fm.ascent();

  for (int i = 0; i < strs.size(); ++i) {
    double dx = 0.0;

    double tw = fm.width(strs[i]);

    if      (options.align & Qt::AlignHCenter)
      dx = (prect.getWidth() - tw)/2;
    else if (options.align & Qt::AlignRight)
      dx = prect.getWidth() - tw;

    double x = prect.getXMin() + dx;

    auto pt = device->pixelToWindow(Point(x, y));

    if (options.contrast)
      drawContrastText(device, pt, strs[i], options.contrastAlpha);
    else
      drawSimpleText(device, pt, strs[i]);

    y += fm.height();
  }
}

void
drawRotatedTextInBox(PaintDevice *device, const BBox &rect,
                     const QString &text, const QPen &pen, const TextOptions &options)
{
  assert(rect.isValid());

  device->setPen(pen);

  CQChartsRotatedText::drawInBox(device, rect, text, options, /*alignBox*/false);
}

//------

CQChartsGeom::BBox
calcTextAtPointRect(PaintDevice *device, const Point &point, const QString &text,
                    const TextOptions &options, bool centered, double pdx, double pdy)
{
  auto text1 = clipTextToLength(text, device->font(), options.clipLength, options.clipElide);

  //---

  // handle html separately
  if (options.html) {
    Size psize = CQChartsDrawPrivate::calcHtmlTextSize(text1, device->font(), options.margin);

    auto sw = device->pixelToWindowWidth (psize.width () + 4);
    auto sh = device->pixelToWindowHeight(psize.height() + 4);

    BBox rect(point.x - sw/2.0, point.y - sh/2.0,
              point.x + sw/2.0, point.y + sh/2.0);

    return rect;
  }

  //---

  QFontMetricsF fm(device->font());

  double ta = fm.ascent();
  double td = fm.descent();

  double tw = fm.width(text1);

  //---

  if (options.angle.isZero()) {
    // calc dx : point is left or hcenter of text (
    // drawContrastText and drawSimpleText wants left aligned
    double dx1 = 0.0, dy1 = 0.0; // pixel

    if (! centered) { // point is left
      if      (options.align & Qt::AlignHCenter) dx1 = -tw/2.0;
      else if (options.align & Qt::AlignRight  ) dx1 = -tw - pdx;
    }
    else {            // point is center
      if      (options.align & Qt::AlignLeft ) dx1 =  tw/2.0 + pdx;
      else if (options.align & Qt::AlignRight) dx1 = -tw/2.0 - pdx;
    }

    if      (options.align & Qt::AlignTop    ) dy1 =  ta + pdy;
    else if (options.align & Qt::AlignBottom ) dy1 = -td - pdy;
    else if (options.align & Qt::AlignVCenter) dy1 = (ta - td)/2.0;

    auto tp = point;

    if (dx1 != 0.0 || dy1 != 0.0) {
      // apply delta (pixels)
      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(Point(pp.x + dx1, pp.y + dy1));
    }

    auto pp = device->windowToPixel(tp);

    BBox prect(pp.x, pp.y - ta, pp.x + tw, pp.y + td);

    return device->pixelToWindow(prect);
  }
  else {
    // calc dx : point is left or hcenter of text
    // CQChartsRotatedText::draw wants center aligned
    auto tp = point;

    if (! centered) {
      double dx1 = -tw/2.0;

      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(Point(pp.x + dx1, pp.y));
    }

    auto pp = device->windowToPixel(tp);

    BBox prect(pp.x, pp.y - ta, pp.x + tw, point.y + td);

    return CQChartsRotatedText::calcBBox(tp.x, tp.y, text1, device->font(),
                                         options, 0, /*alignBox*/true);
  }
}

void
drawTextsAtPoint(PaintDevice *device, const Point &point, const QStringList &texts,
                const TextOptions &options)
{
  if      (texts.size() == 1) {
    drawTextAtPoint(device, point, texts[0], options);
  }
  else if (texts.size() == 2) {
    QFontMetricsF fm(device->font());

    double th = fm.height();

    auto ppoint = device->windowToPixel(point);

    auto point1 = device->pixelToWindow(Point(ppoint.x, ppoint.y - th/2));
    auto point2 = device->pixelToWindow(Point(ppoint.x, ppoint.y + th/2));

    drawTextAtPoint(device, point1, texts[0], options);
    drawTextAtPoint(device, point2, texts[1], options);
  }
  else {
    assert(false);
  }
}

void
drawTextAtPoint(PaintDevice *device, const Point &point, const QString &text,
                const TextOptions &options, bool centered, double pdx, double pdy)
{
  auto text1 = clipTextToLength(text, device->font(), options.clipLength, options.clipElide);

  //---

  QFontMetricsF fm(device->font());

  double ta = fm.ascent();
  double td = fm.descent();

  double tw = fm.width(text1);

  //---

  // handle html separately
  if (options.html) {
    auto psize = CQChartsDrawPrivate::calcHtmlTextSize(text1, device->font(), options.margin);

    auto sw = device->pixelToWindowWidth (psize.width () + 4);
    auto sh = device->pixelToWindowHeight(psize.height() + 4);

    auto dx = device->pixelToSignedWindowWidth (pdx);
    auto dy = device->pixelToSignedWindowHeight(pdy);

    auto c = Point(point.x + dx, point.y + dy);

    BBox rect;

    if (centered)
      rect = BBox(c.x - sw/2.0, c.y - sh/2.0, c.x + sw/2.0, c.y + sh/2.0);
    else
      rect = BBox(c.x, c.y, c.x + sw, c.y + sh);

    if (options.scaled)
      CQChartsDrawPrivate::drawScaledHtmlText(device, rect, text1, options);
    else
      CQChartsDrawPrivate::drawHtmlText(device, c, rect, text1, options, pdx, pdy);

    return;
  }

  //---

  if (options.angle.isZero()) {
    // calc dx : point is left or hcenter of text (
    // drawContrastText and drawSimpleText wants left aligned
    double dx1 = 0.0, dy1 = 0.0; // pixel

    if (! centered) { // point is left
      if      (options.align & Qt::AlignHCenter) dx1 = -tw/2.0;
      else if (options.align & Qt::AlignRight  ) dx1 = -tw - pdx;
    }
    else {            // point is center
      if      (options.align & Qt::AlignLeft ) dx1 =  tw/2.0 + pdx;
      else if (options.align & Qt::AlignRight) dx1 = -tw/2.0 - pdx;
    }

    if      (options.align & Qt::AlignTop    ) dy1 =  ta + pdy;
    else if (options.align & Qt::AlignBottom ) dy1 = -td - pdy;
    else if (options.align & Qt::AlignVCenter) dy1 = (ta - td)/2.0;

    auto tp = point;

    if (dx1 != 0.0 || dy1 != 0.0) {
      // apply delta (pixels)
      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(Point(pp.x + dx1, pp.y + dy1));
    }

    if (options.contrast)
      drawContrastText(device, tp, text1, options.contrastAlpha);
    else
      drawSimpleText(device, tp, text1);
  }
  else {
    // calc dx : point is left or hcenter of text
    // CQChartsRotatedText::draw wants center aligned
    auto tp = point;

    if (! centered) {
      double dx1 = -tw/2.0;

      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(Point(pp.x + dx1, pp.y));
    }

    CQChartsRotatedText::draw(device, tp, text1, options, /*alignBox*/true);
  }
}

//------

void
drawAlignedText(PaintDevice *device, const Point &p, const QString &text,
                Qt::Alignment align, double pdx, double pdy)
{
  QFontMetricsF fm(device->font());

  double tw = fm.width(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  double dx1 = 0.0, dy1 = 0.0;

  if      (align & Qt::AlignLeft   ) dx1 = pdx;
  else if (align & Qt::AlignRight  ) dx1 = -tw - pdx;
  else if (align & Qt::AlignHCenter) dx1 = -tw/2;

  if      (align & Qt::AlignTop    ) dy1 =  ta + pdy;
  else if (align & Qt::AlignBottom ) dy1 = -td - pdy;
  else if (align & Qt::AlignVCenter) dy1 = (ta - td)/2;

  auto pp = device->windowToPixel(p);
  auto pt = device->pixelToWindow(Point(pp.x + dx1, pp.y + dy1));

  drawSimpleText(device, pt, text);
}

//------

CQChartsGeom::BBox
calcAlignedTextRect(PaintDevice *device, const QFont &font, const Point &p,
                    const QString &text, Qt::Alignment align, double pdx, double pdy)
{
  QFontMetricsF fm(font);

  double tw = fm.width(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  double dx1 = 0.0, dy1 = 0.0;

  if      (align & Qt::AlignLeft   ) dx1 = pdx;
  else if (align & Qt::AlignRight  ) dx1 = -tw - pdx;
  else if (align & Qt::AlignHCenter) dx1 = -tw/2;

  if      (align & Qt::AlignTop    ) dy1 =  ta + pdy;
  else if (align & Qt::AlignBottom ) dy1 = -td - pdy;
  else if (align & Qt::AlignVCenter) dy1 = (ta - td)/2;

  auto pp = device->windowToPixel(p);
  auto pt = device->pixelToWindow(Point(pp.x + dx1, pp.y + dy1));

  BBox pbbox(pt.x, pt.y - ta, pt.x + tw, pt.y + td);

  return device->pixelToWindow(pbbox);
}

//------

void
drawContrastText(PaintDevice *device, const Point &p, const QString &text, const Alpha &alpha)
{
  auto pen = device->pen();

  //---

  // set contrast color
  // TODO: allow set type (invert, bw) and alpha
//auto icolor = CQChartsUtil::invColor(pen.color());
  auto icolor = CQChartsUtil::bwColor(pen.color());

  setColorAlpha(icolor, alpha);

  //---

  auto pp = device->windowToPixel(p);

  // draw contrast outline
  device->setPen(icolor);

  for (int dy = -2; dy <= 2; ++dy) {
    for (int dx = -2; dx <= 2; ++dx) {
      if (dx != 0 || dy != 0) {
        auto p1 = device->pixelToWindow(Point(pp.x + dx, pp.y + dy));

        drawSimpleText(device, p1, text);
      }
    }
  }

  //---

  // draw text
  device->setPen(pen);

  drawSimpleText(device, p, text);
}

//------

CQChartsGeom::Size
calcTextSize(const QString &text, const QFont &font, const TextOptions &options)
{
  if (options.html)
    return CQChartsDrawPrivate::calcHtmlTextSize(text, font, options.margin);

  //---

  QFontMetricsF fm(font);

  return Size(fm.width(text), fm.height());
}

//------

void
drawCenteredText(PaintDevice *device, const Point &pos, const QString &text)
{
  QFontMetricsF fm(device->font());

  auto ppos = device->windowToPixel(pos);

  Point ppos1(ppos.x - fm.width(text)/2, ppos.y + (fm.ascent() - fm.descent())/2);

  drawSimpleText(device, device->pixelToWindow(ppos1), text);
}

//------

void
drawSimpleText(PaintDevice *device, const Point &pos, const QString &text)
{
  device->drawText(pos, text);
}

//---

void
drawSymbol(PaintDevice *device, const PenBrush &penBrush, const Symbol &symbol,
           const Point &c, const Length &size)
{
  drawSymbol(device, penBrush, symbol, c, size, size);
}

void
drawSymbol(PaintDevice *device, const PenBrush &penBrush, const Symbol &symbol,
           const Point &c, const Length &xsize, const Length &ysize)
{
  setPenBrush(device, penBrush);

  drawSymbol(device, symbol, c, xsize, ysize);
}

void
drawSymbol(PaintDevice *device, const Symbol &symbol, const BBox &bbox)
{
  assert(bbox.isValid());

  auto pbbox = device->windowToPixel(bbox);

  double cx = bbox.getXMid();
  double cy = bbox.getYMid();
  double sx = pbbox.getWidth();
  double sy = pbbox.getHeight();

  auto xsize = Length::pixel(sx/2.0);
  auto ysize = Length::pixel(sy/2.0);

  drawSymbol(device, symbol, Point(cx, cy), xsize, ysize);
}

void
drawSymbol(PaintDevice *device, const Symbol &symbol, const Point &c, const Length &size)
{
  drawSymbol(device, symbol, c, size, size);
}

void
drawSymbol(PaintDevice *device, const Symbol &symbol, const Point &c,
           const Length &xsize, const Length &ysize)
{
  if (! xsize.isValid() || ! ysize.isValid())
    return;

  CQChartsPlotSymbolRenderer srenderer(device, Point(c), xsize, ysize);

  if      (symbol.isFilled() && ! symbol.isStroked())
    CQChartsPlotSymbolMgr::fillSymbol(symbol, &srenderer); // filled
  else if (symbol.isStroked() && ! symbol.isFilled())
    CQChartsPlotSymbolMgr::drawSymbol(symbol, &srenderer); // wireframe
  else {
    CQChartsPlotSymbolMgr::fillSymbol  (symbol, &srenderer); // filled
    CQChartsPlotSymbolMgr::strokeSymbol(symbol, &srenderer); // stroked
  }
}

//---

bool
polygonSidesPath(QPainterPath &path, const BBox &bbox, int n, const Angle &angle)
{
  path = QPainterPath();

  if (n < 3) return false;

  double xc = bbox.getXMid();
  double yc = bbox.getYMid();

  double xr = bbox.getWidth ()/2.0;
  double yr = bbox.getHeight()/2.0;

  double a  = M_PI/2.0 - angle.radians();
  double da = 2.0*M_PI/n;

  for (int i = 0; i < n; ++i) {
    double c = std::cos(a);
    double s = std::sin(a);

    double x = xc + c*xr;
    double y = yc + s*yr;

    if (i == 0)
      path.moveTo(QPointF(x, y));
    else
      path.lineTo(QPointF(x, y));

    a += da;
  }

  path.closeSubpath();

  return true;
}

void
diamondPath(QPainterPath &path, const BBox &bbox)
{
  path = QPainterPath();

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMid(), y2 = bbox.getYMid();
  double x3 = bbox.getXMax(), y3 = bbox.getYMax();

  path.moveTo(QPointF(x1, y2));
  path.lineTo(QPointF(x2, y1));
  path.lineTo(QPointF(x3, y2));
  path.lineTo(QPointF(x2, y3));

  path.closeSubpath();
}

void
trianglePath(QPainterPath &path, const Point &p1, const Point &p2, const Point &p3)
{
  path = QPainterPath();

  path.moveTo(p1.x, p1.y);
  path.lineTo(p2.x, p2.y);
  path.lineTo(p3.x, p3.y);

  path.closeSubpath();
}

//---

void
editHandlePath(PaintDevice *, QPainterPath &path, const BBox &bbox)
{
  path = QPainterPath();

  //---

  auto ll = bbox.getLL();
  auto ur = bbox.getUR();

  path.moveTo(ll.x, ll.y);
  path.lineTo(ur.x, ll.y);
  path.lineTo(ur.x, ur.y);
  path.lineTo(ll.x, ur.y);

  path.closeSubpath();
}

//---

void
roundedLinePath(QPainterPath &path, const Point &p1, const Point &p2, double w)
{
  path = QPainterPath();

  double w2 = w/2.0;

  double a = std::atan2(p2.y - p1.y, p2.x - p1.x);

  double c = std::cos(a);
  double s = std::sin(a);

  QPointF pl1(p1.x + w2*s, p1.y - w2*c);
  QPointF pl2(p2.x + w2*s, p2.y - w2*c);
  QPointF pl6(p2.x - w2*s, p2.y + w2*c);
  QPointF pl7(p1.x - w2*s, p1.y + w2*c);

  QPointF pl4(p2.x + w2*c, p2.y + w2*s);
  QPointF pl9(p1.x - w2*c, p1.y - w2*s);

  QPointF pl3(pl2.x() + w2*c, pl2.y() + w2*s);
  QPointF pl5(pl6.x() + w2*c, pl6.y() + w2*s);
  QPointF pl8(pl7.x() - w2*c, pl7.y() - w2*s);
  QPointF pl0(pl1.x() - w2*c, pl1.y() - w2*s);

  QPointF pl23((pl2.x() + pl3.x())/2.0, (pl2.y() + pl3.y())/2.0);
  QPointF pl34((pl3.x() + pl4.x())/2.0, (pl3.y() + pl4.y())/2.0);
  QPointF pl45((pl4.x() + pl5.x())/2.0, (pl4.y() + pl5.y())/2.0);
  QPointF pl56((pl5.x() + pl6.x())/2.0, (pl5.y() + pl6.y())/2.0);

  QPointF pl78((pl7.x() + pl8.x())/2.0, (pl7.y() + pl8.y())/2.0);
  QPointF pl89((pl8.x() + pl9.x())/2.0, (pl8.y() + pl9.y())/2.0);
  QPointF pl90((pl9.x() + pl0.x())/2.0, (pl9.y() + pl0.y())/2.0);
  QPointF pl01((pl0.x() + pl1.x())/2.0, (pl0.y() + pl1.y())/2.0);

  path.moveTo (pl1);
  path.lineTo (pl2);
  path.cubicTo(pl23, pl34, pl4);
  path.cubicTo(pl45, pl56, pl6);
  path.lineTo (pl7);
  path.cubicTo(pl78, pl89, pl9);
  path.cubicTo(pl90, pl01, pl1);

  path.closeSubpath();
}

bool
roundedPolygonPath(QPainterPath &path, const Polygon &poly, double xsize, double ysize)
{
  auto interpLine = [&](const QPointF &p1, const QPointF &p2, QPointF &pc1, QPointF &pc2) {
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
  };

  //---

  path = QPainterPath();

  //---

  if (poly.size() < 3)
    return false;

  int i1 = poly.size() - 1;
  int i2 = 0;
  int i3 = 1;

  while (i2 < poly.size()) {
    const auto &p1 = poly.qpoint(i1);
    const auto &p2 = poly.qpoint(i2);
    const auto &p3 = poly.qpoint(i3);

    if (xsize > 0 || ysize > 0) {
      QPointF p12s, p12e, p23s, p23e;

      interpLine(p1, p2, p12s, p12e);
      interpLine(p2, p3, p23s, p23e);

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

  return true;
}

//---

void
drawPieSlice(PaintDevice *device, const Point &c, double ri, double ro,
             const Angle &angle1, const Angle &angle2, bool isInvertX, bool isInvertY)
{
  QPainterPath path;

  pieSlicePath(path, c, ri, ro, angle1, angle2, isInvertX, isInvertY);

  if (CQChartsAngle::isCircle(angle1, angle2)) {
    device->fillPath(path, device->brush());

    QPainterPath ipath, opath;

    ellipsePath(ipath, BBox(c.x - ri, c.y - ri, c.x + ri, c.y + ri));
    ellipsePath(opath, BBox(c.x - ro, c.y - ro, c.x + ro, c.y + ro));

    device->strokePath(ipath, device->pen());
    device->strokePath(opath, device->pen());
  }
  else {
    device->drawPath(path);
  }
}

void
pieSlicePath(QPainterPath &path, const Point &c, double ri, double ro, const Angle &a1,
             const Angle &a2, bool isInvertX, bool isInvertY)
{
  path = QPainterPath();

  //---

  BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  //---

  if (! CMathUtil::isZero(ri)) {
    BBox bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

    //---

    double da = (isInvertX != isInvertY ? -1 : 1);

    double ra1 = da*a1.radians();
    double ra2 = da*a2.radians();

    auto p1 = CQChartsGeom::circlePoint(c, ri, ra1);
    auto p2 = CQChartsGeom::circlePoint(c, ro, ra1);
    auto p3 = CQChartsGeom::circlePoint(c, ri, ra2);
    auto p4 = CQChartsGeom::circlePoint(c, ro, ra2);

    path.moveTo(p1.x, p1.y);
    path.lineTo(p2.x, p2.y);

    path.arcTo(bbox.qrect(), -a1.value(), a1.value() - a2.value());

    path.lineTo(p4.x, p4.y);
    path.lineTo(p3.x, p3.y);

    path.arcTo(bbox1.qrect(), -a2.value(), a2.value() - a1.value());
  }
  else {
    if (CQChartsAngle::isCircle(a1, a2)) {
      path.addEllipse(bbox.qrect());
    }
    else {
      path.moveTo(QPointF(c.x, c.y));

      path.arcTo(bbox.qrect(), -a1.value(), a1.value() - a2.value());
    }
  }

  path.closeSubpath();
}

//---

void
drawEllipse(PaintDevice *device, const BBox &bbox)
{
  QPainterPath path;

  ellipsePath(path, bbox);

  device->drawPath(path);
}

void
ellipsePath(QPainterPath &path, const BBox &bbox)
{
  path = QPainterPath();

  //---

  double xc = bbox.getXMid();
  double yc = bbox.getYMid();

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  double xr = w/2.0;
  double yr = h/2.0;

  double f = 4.0*(std::sqrt(2.0) - 1.0)/3.0;

  double dx = xr*f;
  double dy = yr*f;

  path.moveTo (                                    xc + xr, yc     );
  path.cubicTo(xc + xr, yc + dy, xc + dx, yc + yr, xc     , yc + yr);
  path.cubicTo(xc - dx, yc + yr, xc - xr, yc + dy, xc - xr, yc     );
  path.cubicTo(xc - xr, yc - dy, xc - dx, yc - yr, xc     , yc - yr);
  path.cubicTo(xc + dx, yc - yr, xc + xr, yc - dy, xc + xr, yc     );

  path.closeSubpath();
}

//---

void
drawArc(PaintDevice *device, const BBox &bbox, const Angle &angle, const Angle &dangle)
{
  QPainterPath path;

  arcPath(path, bbox, angle, dangle);

  device->drawPath(path);
}

void
arcPath(QPainterPath &path, const BBox &bbox, const Angle &angle, const Angle &dangle)
{
  path = QPainterPath();

  //---

  auto c = bbox.getCenter();

  auto rect = bbox.qrect();

  path.arcMoveTo(rect, -angle.value());
  path.arcTo    (rect, -angle.value(), -dangle.value());
  path.lineTo   (c.x, c.y);

  path.closeSubpath();
}

//---

void
drawArcSegment(PaintDevice *device, const BBox &ibbox, const BBox &obbox,
               const Angle &angle, const Angle &dangle)
{
  QPainterPath path;

  arcSegmentPath(path, ibbox, obbox, angle, dangle);

  device->drawPath(path);
}

void
arcSegmentPath(QPainterPath &path, const BBox &ibbox, const BBox &obbox,
               const Angle &angle, const Angle &dangle)
{
  path = QPainterPath();

  // arc segment for start angle and delta angle for circles in inner and outer boxes
  auto angle2 = angle + dangle;

  auto irect = ibbox.qrect();
  auto orect = obbox.qrect();

  //---

  path.arcMoveTo(orect, -angle.value());
  path.arcTo    (orect, -angle.value() , -dangle.value());
  path.arcTo    (irect, -angle2.value(),  dangle.value());

  path.closeSubpath();
}

//---

void
drawArcsConnector(PaintDevice *device, const BBox &ibbox, const Angle &a1,
                  const Angle &da1, const Angle &a2, const Angle &da2, bool isSelf)
{
  QPainterPath path;

  arcsConnectorPath(path, ibbox, a1, da1, a2, da2, isSelf);

  // draw path
  device->drawPath(path);
}

void
arcsConnectorPath(QPainterPath &path, const BBox &ibbox, const Angle &a1,
                  const Angle &da1, const Angle &a2, const Angle &da2,
                  bool isSelf)
{
  path = QPainterPath();

  // draw connecting arc between inside of two arc segments
  // . arc segments have start angle and delta angle for circles in inner and outer boxes
  // isSelf is true if connecting arcs are the same
  auto a11 = a1 + da1;
  auto a21 = a2 + da2;

  auto irect = ibbox.qrect();
  auto c     = irect.center();

  //---

  path.arcMoveTo(irect, -a1 .value());   auto p1 = path.currentPosition();
  path.arcMoveTo(irect, -a11.value());   auto p2 = path.currentPosition();
  path.arcMoveTo(irect, -a2 .value()); //auto p3 = path.currentPosition();
  path.arcMoveTo(irect, -a21.value());   auto p4 = path.currentPosition();

  //--

  if (! isSelf) {
    path.moveTo(p1);
    path.quadTo(c, p4);
    path.arcTo (irect, -a21.value(), da2.value());
    path.quadTo(c, p2);
    path.arcTo (irect, -a11.value(), da1.value());

    path.closeSubpath();
  }
  else {
    path.moveTo(p1);
    path.quadTo(c, p2);
    path.arcTo (irect, -a11.value(), da1.value());

    path.closeSubpath();
  }
}

//---

// TODO: support path angle, offset control points by line width
void
edgePath(QPainterPath &path, const BBox &ibbox, const BBox &obbox, bool isLine,
         Qt::Orientation orientation)
{
  path = QPainterPath();

  // x from right of source rect to left of dest rect
  if (orientation == Qt::Horizontal) {
    bool swapped = false;

    // input on left, output on right
    double x1 = ibbox.getXMax(), x2 = obbox.getXMin();

    if (x1 > x2) {
      x1 = obbox.getXMax(), x2 = ibbox.getXMin();
      swapped = true;
    }

    // start y range from source node, and end y range from dest node
    double y11 = ibbox.getYMax(), y12 = ibbox.getYMin();
    double y21 = obbox.getYMax(), y22 = obbox.getYMin();

    if (swapped) {
      std::swap(y11, y21);
      std::swap(y12, y22);
    }

    if (isLine) {
      double y1m = CMathUtil::avg(y11, y12);
      double y2m = CMathUtil::avg(y21, y22);

      curvePath(path, Point(x1, y1m), Point(x2, y2m));
    }
    else {
      // curve control point x at 1/3 and 2/3
      double x3 = CMathUtil::lerp(1.0/3.0, x1, x2);
      double x4 = CMathUtil::lerp(2.0/3.0, x1, x2);

      path.moveTo (QPointF(x1, y11));
      path.cubicTo(QPointF(x3, y11), QPointF(x4, y21), QPointF(x2, y21));
      path.lineTo (QPointF(x2, y22));
      path.cubicTo(QPointF(x4, y22), QPointF(x3, y12), QPointF(x1, y12));

      path.closeSubpath();
    }
  }
  else {
    bool swapped = false;

    // input on bottom, output on top
    double y1 = ibbox.getYMax(), y2 = obbox.getYMin();

    if (y1 > y2) {
      y1 = obbox.getYMax(), y2 = ibbox.getYMin();
      swapped = true;
    }

    // start x range from source node, and end x range from dest node
    double x11 = ibbox.getXMax(), x12 = ibbox.getXMin();
    double x21 = obbox.getXMax(), x22 = obbox.getXMin();

    if (swapped) {
      std::swap(x11, x21);
      std::swap(x12, x22);
    }

    if (isLine) {
      double x1m = CMathUtil::avg(x11, x12);
      double x2m = CMathUtil::avg(x21, x22);

      curvePath(path, Point(x1m, y1), Point(x2m, y2));
    }
    else {
      // curve control point y at 1/3 and 2/3
      double y3 = CMathUtil::lerp(1.0/3.0, y1, y2);
      double y4 = CMathUtil::lerp(2.0/3.0, y1, y2);

      path.moveTo (QPointF(x11, y1));
      path.cubicTo(QPointF(x11, y3), QPointF(x21, y4), QPointF(x21, y2));
      path.lineTo (QPointF(x22, y2));
      path.cubicTo(QPointF(x22, y4), QPointF(x12, y3), QPointF(x12, y1));

      path.closeSubpath();
    }
  }
}

void
edgePath(QPainterPath &path, const Point &p1, const Point &p2, double lw,
         Qt::Orientation orientation)
{
  if (orientation == Qt::Horizontal) {
    double y11 = p1.y + lw/2.0, y12 = p1.y - lw/2.0;
    double y21 = p2.y + lw/2.0, y22 = p2.y - lw/2.0;

    // curve control point x at 1/3 and 2/3
    double x3 = CMathUtil::lerp(1.0/3.0, p1.x, p2.x);
    double x4 = CMathUtil::lerp(2.0/3.0, p1.x, p2.x);

    path.moveTo (QPointF(p1.x, y11));
    path.cubicTo(QPointF(x3  , y11), QPointF(x4, y21), QPointF(p2.x, y21));
    path.lineTo (QPointF(p2.x, y22));
    path.cubicTo(QPointF(x4  , y22), QPointF(x3, y12), QPointF(p1.x, y12));

    path.closeSubpath();
  }
  else {
    double x11 = p1.x + lw/2.0, x12 = p1.x - lw/2.0;
    double x21 = p2.x + lw/2.0, x22 = p2.x - lw/2.0;

    // curve control point y at 1/3 and 2/3
    double y3 = CMathUtil::lerp(1.0/3.0, p1.y, p2.y);
    double y4 = CMathUtil::lerp(2.0/3.0, p1.y, p2.y);

    path.moveTo (QPointF(x11, p1.y));
    path.cubicTo(QPointF(x11, y3  ), QPointF(x21, y4), QPointF(x21, p2.y));
    path.lineTo (QPointF(x22, p2.y));
    path.cubicTo(QPointF(x22, y4  ), QPointF(x12, y3), QPointF(x12, p1.y));

    path.closeSubpath();
  }
}

void
selfEdgePath(QPainterPath &path, const BBox &bbox, double lw, Qt::Orientation orientation)
{
  double xr = bbox.getWidth ()/2.0;
  double yr = bbox.getHeight()/2.0;

  if (orientation == Qt::Horizontal) {
    // draw arc from 45 degrees left of vertical to 45 degrees right of vertical
    double a = M_PI/4.0;

    double c = std::cos(a);
    double s = std::sin(a);

    double xm = bbox.getXMid();
    double ym = bbox.getYMid();

    double yt  = bbox.getYMax() + yr/2.0;
    double yt1 = yt - lw/2.0;
    double yt2 = yt + lw/2.0;

    double x1 = xm - xr*c, y1 = ym + xr*s;
    double x2 = xm + xr*c, y2 = y1;

    double lw1 = std::sqrt(2)*lw/2.0;

    path.moveTo (QPointF(x1 - lw1, y1 - lw1));
    path.cubicTo(QPointF(x1 - lw1, yt2), QPointF(x2 + lw1, yt2), QPointF(x2 + lw1, y2 - lw1));
    path.lineTo (QPointF(x2 - lw1, y2 + lw1));
    path.cubicTo(QPointF(x2 - lw1, yt1), QPointF(x1 + lw1, yt1), QPointF(x1 + lw1, y1 + lw1));

    path.closeSubpath();
  }
  else {
    // draw arc from 45 degrees above horizontal to 45 degrees below horizontal
    double a = M_PI/4.0;

    double c = std::cos(a);
    double s = std::sin(a);

    double xm = bbox.getXMid();
    double ym = bbox.getYMid();

    double xt  = bbox.getXMax() + xr/2.0;
    double xt1 = xt - lw/2.0;
    double xt2 = xt + lw/2.0;

    double x1 = xm - xr*c, y1 = ym + yr*s;
    double x2 = xm + xr*c, y2 = y1;

    double lw1 = std::sqrt(2)*lw/2.0;

    path.moveTo (QPointF(x1 - lw1, y1 - lw1));
    path.cubicTo(QPointF(xt2     , y1 - lw1), QPointF(xt2, y2 + lw1), QPointF(x2 - lw1, y2 + lw1));
    path.lineTo (QPointF(x2 + lw1, y2 - lw1));
    path.cubicTo(QPointF(xt1     , y2 - lw1), QPointF(xt1, y1 + lw1), QPointF(x1 + lw1, y1 + lw1));

    path.closeSubpath();
  }
}

//---

void
curvePath(QPainterPath &path, const BBox &ibbox, const BBox &obbox, bool rectilinear)
{
  // x from right of source rect to left of dest rect
  bool swapped = false;

  double x1 = ibbox.getXMax(), x2 = obbox.getXMin();

  if (x1 > x2) {
    x1 = obbox.getXMax(), x2 = ibbox.getXMin();
    swapped = true;
  }

  double y1 = ibbox.getYMid();
  double y2 = obbox.getYMid();

  if (swapped)
    std::swap(y1, y2);

  curvePath(path, Point(x1, y1), Point(x2, y2), rectilinear);
}

void
curvePath(QPainterPath &path, const Point &p1, const Point &p4, bool rectilinear)
{
  path = QPainterPath();

  //---

  if (! rectilinear) {
    double x2 = CMathUtil::lerp(1.0/3.0, p1.x, p4.x);
    double x3 = CMathUtil::lerp(2.0/3.0, p1.x, p4.x);

    auto p2 = Point(x2, p1.y);
    auto p3 = Point(x3, p4.y);

    path.moveTo (p1.qpoint());
    path.cubicTo(p2.qpoint(), p3.qpoint(), p4.qpoint());
  }
  else {
    double x2 = CMathUtil::lerp(0.5, p1.x, p4.x);

    auto p2 = Point(x2, p1.y);
    auto p3 = Point(x2, p4.y);

    path.moveTo(p1.qpoint());
    path.lineTo(p2.qpoint());
    path.lineTo(p3.qpoint());
    path.lineTo(p4.qpoint());
  }
}

void
selfCurvePath(QPainterPath &path, const BBox &bbox, bool rectilinear)
{
  path = QPainterPath();

  //---

  double xr = bbox.getWidth ()/2.0;
  double yr = bbox.getHeight()/2.0;

  // draw arc from 45 degrees left of vertical to 45 degrees right of vertical
  double a = M_PI/4.0;

  double c = std::cos(a);
  double s = std::sin(a);

  double xm = bbox.getXMid();
  double ym = bbox.getYMid();

  double yt = bbox.getYMax() + yr/2.0;

  double x1 = xm - xr*c, y1 = ym + xr*s;
  double x2 = xm + xr*c, y2 = y1;

  if (! rectilinear) {
    path.moveTo (QPointF(x1, y1));
    path.cubicTo(QPointF(x1, yt), QPointF(x2, yt), QPointF(x2, y2));
  }
  else {
    path.moveTo(QPointF(x1, y1));
    path.lineTo(QPointF(x1, yt));
    path.lineTo(QPointF(x2, yt));
    path.lineTo(QPointF(x2, y2));
  }
}

//---

void
cornerHandlePath(PaintDevice *device, QPainterPath &path, const Point &p)
{
  double sx = device->pixelToWindowWidth (16);
  double sy = device->pixelToWindowHeight(16);

  path.addEllipse(p.x - sx/2, p.y - sy/2, sx, sy);
}

void
resizeHandlePath(PaintDevice *device, QPainterPath &path, const Point &p)
{
  double msx1 = device->pixelToWindowWidth (12);
  double msy1 = device->pixelToWindowHeight(12);
  double msx2 = device->pixelToWindowWidth (4);
  double msy2 = device->pixelToWindowHeight(4);

  path.moveTo(p.x - msx1, p.y       );
  path.lineTo(p.x - msx2, p.y + msy2);
  path.lineTo(p.x       , p.y + msy1);
  path.lineTo(p.x + msx2, p.y + msy2);
  path.lineTo(p.x + msx1, p.y       );
  path.lineTo(p.x + msx2, p.y - msy2);
  path.lineTo(p.x       , p.y - msy1);
  path.lineTo(p.x - msx2, p.y - msy2);

  path.closeSubpath();
}

void
extraHandlePath(PaintDevice *device, QPainterPath &path, const Point &p)
{
  double sx = device->pixelToWindowWidth (16);
  double sy = device->pixelToWindowHeight(16);

  path.moveTo(QPointF(p.x - sx/2, p.y - sy/2));
  path.lineTo(QPointF(p.x + sx/2, p.y - sy/2));
  path.lineTo(QPointF(p.x + sx/2, p.y + sy/2));
  path.lineTo(QPointF(p.x - sx/2, p.y + sy/2));

  path.closeSubpath();
}

//---

QString
clipTextToLength(PaintDevice *device, const QString &text,
                 const Length &clipLength, const Qt::TextElideMode &clipElide)
{
  if (! clipLength.isValid())
    return text;

  double clipLengthPixels = device->lengthPixelWidth(clipLength);

  return clipTextToLength(text, device->font(), clipLengthPixels, clipElide);
}

QString
clipTextToLength(const QString &text, const QFont &font, double clipLength,
                 const Qt::TextElideMode &clipElide)
{
  if (clipLength <= 0.0)
    return text;

  if (clipElide != Qt::ElideLeft && clipElide != Qt::ElideRight)
    return text;

  //---

  QFontMetricsF fm(font);

  double ellipsisWidth = fm.width("...");

  if (ellipsisWidth > clipLength)
    return "";

  double clipLength1 = clipLength - ellipsisWidth;

  //---

  auto isClipped = [&](const QString &str) {
    double w = fm.width(str);

    return (w > clipLength1);
  };

  auto isLenClipped = [&](const QString &str, int len) {
    if      (clipElide == Qt::ElideLeft)
      return isClipped(str.right(len));
    else if (clipElide == Qt::ElideRight)
      return isClipped(str.left(len));
    else
      assert(false);
  };

  //---

  if (! isClipped(text))
    return text;

  //---

  using LenClipped = std::map<int, bool>;

  LenClipped lenClipped;

  int len = text.length();

  int len1 = 0;
  int len2 = len;

  int midLen = (len1 + len2)/2;

  while (midLen > 0) {
    auto pl = lenClipped.find(midLen);

    bool clipped;

    if (pl == lenClipped.end()) {
      clipped = isLenClipped(text, midLen);

      lenClipped[midLen] = clipped;
    }
    else {
      clipped = (*pl).second;

      if (! clipped && midLen == len1)
        break;
    }

    if (clipped)
      len2 = midLen;
    else
      len1 = midLen;

    midLen = (len1 + len2)/2;
  }

  QString text1;

  if      (clipElide == Qt::ElideLeft)
    text1 = "..." + text.right(midLen);
  else if (clipElide == Qt::ElideRight)
    text1 = text.left(midLen) + "...";
  else
    assert(false);

  return text1;
}

}

//------

namespace CQChartsDrawPrivate {

CQChartsGeom::Size
calcHtmlTextSize(const QString &text, const QFont &font, int margin)
{
  QTextDocument td;

  td.setDocumentMargin(margin);
  td.setHtml(text);
  td.setDefaultFont(font);

  auto *layout = td.documentLayout();

  return Size(layout->documentSize());
}

//------

void
drawScaledHtmlText(PaintDevice *device, const BBox &tbbox, const QString &text,
                   const TextOptions &options)
{
  assert(tbbox.isValid());

  // calc scale
  double s = options.scale;

  if (s <= 0.0) {
    auto psize = calcHtmlTextSize(text, device->font(), options.margin);

    double pw = psize.width ();
    double ph = psize.height();

    auto ptbbox = device->windowToPixel(tbbox);

    double xs = ptbbox.getWidth ()/pw;
    double ys = ptbbox.getHeight()/ph;

    s = std::min(xs, ys);
  }

  //---

  // scale font
  device->setFont(CQChartsUtil::scaleFontSize(device->font(), s));

  //---

  drawHtmlText(device, tbbox.getCenter(), tbbox, text, options);
}

void
drawHtmlText(PaintDevice *device, const Point &center, const BBox &tbbox,
             const QString &text, const TextOptions &options, double pdx, double pdy)
{
  assert(tbbox.isValid());

  auto ptbbox = device->windowToPixel(tbbox);

  //---

  auto psize = calcHtmlTextSize(text, device->font(), options.margin);

  double c = options.angle.cos();
  double s = options.angle.sin();

  double pdx1 = 0.0, pdy1 = 0.0;

  if (! options.formatted) {
    if      (options.align & Qt::AlignHCenter)
      pdx1 = (ptbbox.getWidth() - psize.width())/2.0;
    else if (options.align & Qt::AlignRight)
      pdx1 = ptbbox.getWidth() - psize.width();

    if      (options.align & Qt::AlignVCenter)
      pdy1 = (ptbbox.getHeight() - psize.height())/2.0;
    else if (options.align & Qt::AlignBottom)
      pdy1 = ptbbox.getHeight() - psize.height();
  }

  if (! options.angle.isZero()) {
    double pdx2 = c*pdx1 - s*pdy1;
    double pdy2 = s*pdx1 + c*pdy1;

    pdx1 = pdx2;
    pdy1 = pdy2;
  }

  pdx += pdx1;
  pdy += pdy1;

//auto ptbbox1 = ptbbox.translated(pdx, pdy); // inner text rect
  BBox ptbbox1(ptbbox.getXMin() + pdx                , ptbbox.getYMin() + pdy,
               ptbbox.getXMin() + pdx + psize.width(), ptbbox.getYMin() + pdy + psize.height());

  //---

  QImage    image;
  QPainter *painter  = nullptr;
  QPainter *ipainter = nullptr;

  if (device->isInteractive()) {
    painter = dynamic_cast<CQChartsViewPlotPaintDevice *>(device)->painter();
  }
  else {
    image = CQChartsUtil::initImage(QSize(int(ptbbox.getWidth()), int(ptbbox.getHeight())));

    ipainter = new QPainter(&image);

    painter = ipainter;
  }

  //---

  painter->save();

  //painter->drawRect(ptbbox .qrect()); // DEBUG
  //painter->drawRect(ptbbox1.qrect()); // DEBUG

  if (! options.angle.isZero()) {
  //auto tc = ptbbox1.getCenter().qpoint();
    auto tc = device->windowToPixel(center).qpoint();

    painter->translate(tc);
    painter->rotate(-options.angle.value());
    painter->translate(-tc);
  }

  QTextDocument td;

  td.setDocumentMargin(options.margin);
  td.setHtml(text);
  td.setDefaultFont(device->font());

  //auto ptbbox2 = ptbbox1.translated(-ptbbox.getXMin(), -ptbbox.getYMin()); // move to origin

  double tx = ptbbox1.getXMin();
  double ty = ptbbox1.getYMin();

  if (device->isInteractive())
    painter->translate(tx, ty);

  //if (options.angle.isZero())
  //  painter->setClipRect(ptbbox2.qrect(), Qt::IntersectClip);

  int pw = ptbbox.getWidth();  // psize.width() ?
  int ph = ptbbox.getHeight(); // psize.width() ?
  int pm = 8;

  td.setPageSize(QSizeF(pw + pm, ph + pm));

  //---

  // TODO: setting 'global' align screws up existing align in html text
  if (options.alignHtml) {
    QTextCursor cursor(&td);

    cursor.select(QTextCursor::Document);

    QTextBlockFormat f;

    f.setAlignment(options.align);

    cursor.setBlockFormat(f);
  }

  //---

  QAbstractTextDocumentLayout::PaintContext ctx;

  auto pc = device->pen().color();

  ctx.palette.setColor(QPalette::Text, pc);

  auto *layout = td.documentLayout();

  layout->setPaintDevice(painter->device());

  if (options.contrast) {
  //auto ipc = CQChartsUtil::invColor(pc);
    auto ipc = CQChartsUtil::bwColor(pc);

    ctx.palette.setColor(QPalette::Text, ipc);

    for (int dy = -2; dy <= 2; ++dy) {
      for (int dx = -2; dx <= 2; ++dx) {
        if (dx != 0 || dy != 0) {
          painter->translate(dx, dy);

          layout->draw(painter, ctx);

          painter->translate(-dx, -dy);
        }
      }
    }

    ctx.palette.setColor(QPalette::Text, pc);

    layout->draw(painter, ctx);
  }
  else {
    layout->draw(painter, ctx);
  }

  if (device->isInteractive())
    painter->translate(-tx, -ty);

  //---

  painter->restore();

  delete ipainter;
}

}

//---

namespace CQChartsDrawUtil {

double
lengthPixelWidth(PaintDevice *device, const Length &len)
{
  return device->lengthPixelWidth(len);
}

}
