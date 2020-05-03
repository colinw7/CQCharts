#include <CQChartsDrawUtil.h>
#include <CQChartsRotatedText.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsUtil.h>

#include <CMathUtil.h>

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

namespace CQChartsDrawUtil {

void
setPenBrush(CQChartsPaintDevice *device, const CQChartsPenBrush &penBrush)
{
  device->setPen  (penBrush.pen);
  device->setBrush(penBrush.brush);
}

void
drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
                   const CQChartsLength &len, const CQChartsSides &sides)
{
  drawRoundedPolygon(device, bbox, len, len, sides);
}

void
drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
                   const CQChartsLength &xlen, const CQChartsLength &ylen,
                   const CQChartsSides &sides)
{
  static double minSize1 = 2.5; // pixels
  static double minSize2 = 1.5; // pixels

  auto pbbox = device->windowToPixel(bbox);

  double minSize = pbbox.getMinSize();

  double xsize = device->lengthPixelWidth (xlen);
  double ysize = device->lengthPixelHeight(ylen);

  if      (minSize >= minSize1) {
    CQChartsRoundedPolygon::draw(device, bbox, xsize, ysize, sides);
  }
  else if (minSize >= minSize2) {
    QPen pen = device->pen();

    QColor pc = pen.color();
    double f  = (minSize - minSize2)/(minSize1 - minSize2);

    pc.setAlphaF(f*pc.alphaF());

    pen.setColor(pc);

    device->setPen(pen);

    CQChartsRoundedPolygon::draw(device, bbox, xsize, ysize, sides);
  }
  else {
    QColor bc = device->brush().color();

    device->setPen(bc);
    device->setBrush(Qt::NoBrush);

    if (pbbox.getWidth() > pbbox.getHeight())
      device->drawLine(CQChartsGeom::Point(bbox.getXMin(), bbox.getYMid()),
                       CQChartsGeom::Point(bbox.getXMax(), bbox.getYMid()));
    else
      device->drawLine(CQChartsGeom::Point(bbox.getXMid(), bbox.getYMin()),
                       CQChartsGeom::Point(bbox.getXMid(), bbox.getYMax()));
  }
}

void
drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::Polygon &poly,
                   const CQChartsLength &len)
{
  drawRoundedPolygon(device, poly, len, len);
}

void
drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::Polygon &poly,
                   const CQChartsLength &xlen, const CQChartsLength &ylen)
{
  static double minSize = 2.5; // pixels

  auto bbox = poly.boundingBox();

  double xsize = device->lengthPixelWidth (xlen);
  double ysize = device->lengthPixelHeight(ylen);

  if (bbox.getWidth() > minSize && bbox.getHeight() > minSize) {
    CQChartsRoundedPolygon::draw(device, poly, xsize, ysize);
  }
  else {
    QColor bc = device->brush().color();

    device->setPen(bc);
    device->setBrush(Qt::NoBrush);

    if (bbox.getWidth() > minSize) // horizontal line
      device->drawLine(CQChartsGeom::Point(bbox.getXMin(), bbox.getYMid()),
                       CQChartsGeom::Point(bbox.getXMax(), bbox.getYMin()));
    else                           // vertical line
      device->drawLine(CQChartsGeom::Point(bbox.getXMid(), bbox.getYMin()),
                       CQChartsGeom::Point(bbox.getXMid(), bbox.getYMax()));
  }
}

void
drawTextInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect,
              const QString &text, const CQChartsTextOptions &options)
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
      CQChartsDrawPrivate::drawHtmlText(device, rect, text, options);

    return;
  }

  //---

  QPen pen = device->pen();

  if (options.clipped)
    device->save();

  if (CMathUtil::isZero(options.angle.value())) {
    if (options.clipped)
      device->setClipRect(rect, Qt::IntersectClip);

    device->setPen(pen);

    //---

    QStringList strs;

    if (options.formatted) {
      auto prect = device->windowToPixel(rect);

      CQChartsUtil::formatStringInRect(text, device->font(), prect, strs,
                                       CQChartsUtil::FormatData(options.formatSeps));
    }
    else
      strs << text;

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
drawStringsInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect,
                 const QStringList &strs, const CQChartsTextOptions &options)
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

    auto pt = device->pixelToWindow(CQChartsGeom::Point(x, y));

    if (options.contrast)
      drawContrastText(device, pt, strs[i], options.contrastAlpha);
    else
      drawSimpleText(device, pt, strs[i]);

    y += fm.height();
  }
}

void
drawRotatedTextInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect,
                     const QString &text, const QPen &pen, const CQChartsTextOptions &options)
{
  assert(rect.isValid());

  device->setPen(pen);

  CQChartsRotatedText::drawInBox(device, rect, text, options, /*alignBox*/false);
}

//------

void
drawTextAtPoint(CQChartsPaintDevice *device, const CQChartsGeom::Point &point, const QString &text,
                const CQChartsTextOptions &options, bool centered, double dx, double dy)
{
  QFontMetricsF fm(device->font());

  double ta = fm.ascent();
  double td = fm.descent();

  auto tw = [&]() { return fm.width(text); };

  if (CMathUtil::isZero(options.angle.value())) {
    // calc dx : point is left or hcenter of text (
    // drawContrastText and drawSimpleText wants left aligned
    double dx1 = 0.0, dy1 = 0.0;

    if (! centered) { // point is left
      if      (options.align & Qt::AlignHCenter) dx1 = -tw()/2.0;
      else if (options.align & Qt::AlignRight  ) dx1 = -tw() - dx;
    }
    else {            // point is center
      if      (options.align & Qt::AlignLeft ) dx1 =  tw()/2.0 + dx;
      else if (options.align & Qt::AlignRight) dx1 = -tw()/2.0 - dx;
    }

    if      (options.align & Qt::AlignTop    ) dy1 =  ta + dy;
    else if (options.align & Qt::AlignBottom ) dy1 = -td - dy;
    else if (options.align & Qt::AlignVCenter) dy1 = (ta - td)/2.0;

    auto tp = point;

    if (dx1 != 0.0 || dy1 != 0.0) {
      // apply delta (pixels)
      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(CQChartsGeom::Point(pp.x + dx1, pp.y + dy1));
    }

    if (options.contrast)
      drawContrastText(device, tp, text, options.contrastAlpha);
    else
      drawSimpleText(device, tp, text);
  }
  else {
    // calc dx : point is left or hcenter of text
    // CQChartsRotatedText::draw wants center aligned
    auto tp = point;

    if (! centered) {
      double dx1 = -tw()/2.0;

      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(CQChartsGeom::Point(pp.x + dx1, pp.y));
    }

    CQChartsRotatedText::draw(device, tp, text, options, /*alignBox*/true);
  }
}

//------

void
drawAlignedText(CQChartsPaintDevice *device, const CQChartsGeom::Point &p, const QString &text,
                Qt::Alignment align, double dx, double dy)
{
  QFontMetricsF fm(device->font());

  double tw = fm.width(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  double dx1 = 0.0, dy1 = 0.0;

  if      (align & Qt::AlignLeft   ) dx1 = dx;
  else if (align & Qt::AlignRight  ) dx1 = -tw - dx;
  else if (align & Qt::AlignHCenter) dx1 = -tw/2;

  if      (align & Qt::AlignTop    ) dy1 =  ta + dy;
  else if (align & Qt::AlignBottom ) dy1 = -td - dy;
  else if (align & Qt::AlignVCenter) dy1 = (ta - td)/2;

  auto pp = device->windowToPixel(p);
  auto pt = device->pixelToWindow(CQChartsGeom::Point(pp.x + dx1, pp.y + dy1));

  drawSimpleText(device, pt, text);
}

//------

CQChartsGeom::BBox
calcAlignedTextRect(CQChartsPaintDevice *device, const QFont &font, const CQChartsGeom::Point &p,
                    const QString &text, Qt::Alignment align, double dx, double dy)
{
  QFontMetricsF fm(font);

  double tw = fm.width(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  double dx1 = 0.0, dy1 = 0.0;

  if      (align & Qt::AlignLeft   ) dx1 = dx;
  else if (align & Qt::AlignRight  ) dx1 = -tw - dx;
  else if (align & Qt::AlignHCenter) dx1 = -tw/2;

  if      (align & Qt::AlignTop    ) dy1 =   ta + dy;
  else if (align & Qt::AlignBottom ) dy1 = - td - dy;
  else if (align & Qt::AlignVCenter) dy1 = (ta - td)/2;

  auto pp = device->windowToPixel(p);
  auto pt = device->pixelToWindow(CQChartsGeom::Point(pp.x + dx1, pp.y + dy1));

  CQChartsGeom::BBox pbbox(pt.x, pt.y - ta, pt.x + tw, pt.y + td);

  return device->pixelToWindow(pbbox);
}

//------

void
drawContrastText(CQChartsPaintDevice *device, const CQChartsGeom::Point &p,
                 const QString &text, const CQChartsAlpha &alpha)
{
  QPen pen = device->pen();

  //---

  // set contrast color
  // TODO: allow set type (invert, bw) and alpha
//QColor icolor = CQChartsUtil::invColor(pen.color());
  QColor icolor = CQChartsUtil::bwColor(pen.color());

  icolor.setAlphaF(alpha.value());

  //---

  auto pp = device->windowToPixel(p);

  // draw contrast outline
  device->setPen(icolor);

  for (int dy = -2; dy <= 2; ++dy) {
    for (int dx = -2; dx <= 2; ++dx) {
      if (dx != 0 || dy != 0) {
        CQChartsGeom::Point p1 =
          device->pixelToWindow(CQChartsGeom::Point(pp.x + dx, pp.y + dy));

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
calcTextSize(const QString &text, const QFont &font, const CQChartsTextOptions &options)
{
  if (options.html)
    return CQChartsDrawPrivate::calcHtmlTextSize(text, font, options.margin);

  //---

  QFontMetricsF fm(font);

  return CQChartsGeom::Size(fm.width(text), fm.height());
}

//------

void
drawCenteredText(CQChartsPaintDevice *device, const CQChartsGeom::Point &pos, const QString &text)
{
  QFontMetricsF fm(device->font());

  auto ppos = device->windowToPixel(pos);

  CQChartsGeom::Point ppos1(ppos.x - fm.width(text)/2, ppos.y + (fm.ascent() - fm.descent())/2);

  drawSimpleText(device, device->pixelToWindow(ppos1), text);
}

//------

void
drawSimpleText(CQChartsPaintDevice *device, const CQChartsGeom::Point &pos, const QString &text)
{
  device->drawText(pos, text);
}

//---

void
drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
           const CQChartsGeom::Point &c, const CQChartsLength &size)
{
  CQChartsPlotSymbolRenderer srenderer(device, CQChartsGeom::Point(c), size);

  if (device->brush().style() != Qt::NoBrush) {
    CQChartsPlotSymbolMgr::fillSymbol(symbol, &srenderer);

    if (device->pen().style() != Qt::NoPen)
      CQChartsPlotSymbolMgr::strokeSymbol(symbol, &srenderer);
  }
  else {
    if (device->pen().style() != Qt::NoPen)
      CQChartsPlotSymbolMgr::drawSymbol(symbol, &srenderer);
  }
}

void
drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
           const CQChartsGeom::BBox &bbox)
{
  assert(bbox.isValid());

  auto pbbox = device->windowToPixel(bbox);

  double cx = bbox.getXMid();
  double cy = bbox.getYMid();
  double ss = pbbox.getMinSize();

  CQChartsLength symbolSize(ss/2.0, CQChartsUnits::PIXEL);

  drawSymbol(device, symbol, CQChartsGeom::Point(cx, cy), symbolSize);
}

//---

void
drawPieSlice(CQChartsPaintDevice *device, const CQChartsGeom::Point &c,
             double ri, double ro, const CQChartsAngle &a1, const CQChartsAngle &a2,
             bool isInvertX, bool isInvertY)
{
  CQChartsGeom::BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  //---

  QPainterPath path;

  if (! CMathUtil::isZero(ri)) {
    CQChartsGeom::BBox bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

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
    double a21 = a2.value() - a1.value();

    if (std::abs(a21) < 360.0) {
      path.moveTo(QPointF(c.x, c.y));

      path.arcTo(bbox.qrect(), -a1.value(), a1.value() - a2.value());
    }
    else {
      path.addEllipse(bbox.qrect());
    }
  }

  path.closeSubpath();

  device->drawPath(path);
}

void
drawArc(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
        const CQChartsAngle &angle, const CQChartsAngle &dangle)
{
  auto c = bbox.getCenter();

  QRectF rect = bbox.qrect();

  QPainterPath path;

  path.arcMoveTo(rect, -angle.value());
  path.arcTo    (rect, -angle.value(), -dangle.value());
  path.lineTo   (c.x, c.y);

  path.closeSubpath();

  device->drawPath(path);
}

void
drawArcSegment(CQChartsPaintDevice *device, const CQChartsGeom::BBox &ibbox,
               const CQChartsGeom::BBox &obbox, const CQChartsAngle &angle,
               const CQChartsAngle &dangle)
{
  // draw arc segment for start angle and delta angle for circles in inner and outer boxes
  CQChartsAngle angle2 = angle + dangle;

  QRectF irect = ibbox.qrect();
  QRectF orect = obbox.qrect();

  //---

  QPainterPath path;

  path.arcMoveTo(orect, -angle.value());
  path.arcTo    (orect, -angle.value() , -dangle.value());
  path.arcTo    (irect, -angle2.value(),  dangle.value());

  path.closeSubpath();

  device->drawPath(path);
}

void
drawArcsConnector(CQChartsPaintDevice *device, const CQChartsGeom::BBox &ibbox,
                  const CQChartsAngle &a1, const CQChartsAngle &da1,
                  const CQChartsAngle &a2, const CQChartsAngle &da2, bool isSelf)
{
  // draw connecting arc between inside of two arc segments
  // . arc segments have start angle and delta angle for circles in inner and outer boxes
  // isSelf is true if connecting arcs are the same
  CQChartsAngle a11 = a1 + da1;
  CQChartsAngle a21 = a2 + da2;

  QRectF  irect = ibbox.qrect();
  QPointF c     = irect.center();

  //---

  QPainterPath path;

  path.arcMoveTo(irect, -a1 .value());   QPointF p1 = path.currentPosition();
  path.arcMoveTo(irect, -a11.value());   QPointF p2 = path.currentPosition();
  path.arcMoveTo(irect, -a2 .value()); //QPointF p3 = path.currentPosition();
  path.arcMoveTo(irect, -a21.value());   QPointF p4 = path.currentPosition();

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

  //---

  // draw path
  device->drawPath(path);
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

  return CQChartsGeom::Size(layout->documentSize());
}

//------

void
drawScaledHtmlText(CQChartsPaintDevice *device, const CQChartsGeom::BBox &tbbox,
                   const QString &text, const CQChartsTextOptions &options)
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

  drawHtmlText(device, tbbox, text, options);
}

void
drawHtmlText(CQChartsPaintDevice *device, const CQChartsGeom::BBox &tbbox,
             const QString &text, const CQChartsTextOptions &options)
{
  assert(tbbox.isValid());

  auto ptbbox = device->windowToPixel(tbbox);

  //---

  auto psize = calcHtmlTextSize(text, device->font(), options.margin);

  double dx = 0.0, dy = 0.0;

  if (! options.formatted) {
    if      (options.align & Qt::AlignHCenter)
      dx = (ptbbox.getWidth() - psize.width())/2.0;
    else if (options.align & Qt::AlignRight)
      dx = ptbbox.getWidth() - psize.width();

    if      (options.align & Qt::AlignVCenter)
      dy = (ptbbox.getHeight() - psize.height())/2.0;
    else if (options.align & Qt::AlignBottom)
      dy = ptbbox.getHeight() - psize.height();
  }

  ptbbox = ptbbox.translated(dx, dy);

  //---

  QImage    image;
  QPainter *painter  = nullptr;
  QPainter *ipainter = nullptr;

  if (device->isInteractive()) {
    painter = dynamic_cast<CQChartsViewPlotPainter *>(device)->painter();
  }
  else {
    image = CQChartsUtil::initImage(QSize(int(ptbbox.getWidth()), int(ptbbox.getHeight())));

    ipainter = new QPainter(&image);

    painter = ipainter;
  }

  //---

  painter->save();

  if (! CMathUtil::isZero(options.angle.value())) {
    QPointF tc = tbbox.getCenter().qpoint();

    painter->translate(tc);
    painter->rotate(options.angle.value());
    painter->translate(-tc);
  }

  QTextDocument td;

  td.setDocumentMargin(options.margin);
  td.setHtml(text);
  td.setDefaultFont(device->font());

  auto ptbbox1 = ptbbox.translated(-ptbbox.getXMin(), -ptbbox.getYMin());

  if (device->isInteractive())
    painter->translate(ptbbox.getXMin(), ptbbox.getYMin());

  painter->setClipRect(ptbbox1.qrect(), Qt::IntersectClip);

  int pm = 8;

  td.setPageSize(QSizeF(ptbbox.getWidth() + pm, ptbbox.getHeight() + pm));

  //---

  QTextCursor cursor(&td);

  cursor.select(QTextCursor::Document);

  QTextBlockFormat f;

  f.setAlignment(options.align);

  cursor.setBlockFormat(f);

  //---

  QAbstractTextDocumentLayout::PaintContext ctx;

  QColor pc = device->pen().color();

  ctx.palette.setColor(QPalette::Text, pc);

  auto *layout = td.documentLayout();

  layout->setPaintDevice(painter->device());

  if (options.contrast) {
  //QColor ipc = CQChartsUtil::invColor(pc);
    QColor ipc = CQChartsUtil::bwColor(pc);

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
    painter->translate(-ptbbox.getXMin(), -ptbbox.getYMin());

  //---

  painter->restore();

  delete ipainter;
}

}
