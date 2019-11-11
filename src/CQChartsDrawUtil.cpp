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
drawRoundedPolygon(CQChartsPaintDevice *device, const QRectF &qrect,
                   const CQChartsLength &xlen, const CQChartsLength &ylen,
                   const CQChartsSides &sides)
{
  static double minSize1 = 2.5; // pixels
  static double minSize2 = 1.5; // pixels

  QRectF prect = device->windowToPixel(qrect);

  double minSize = std::min(prect.width(), prect.height());

  double xsize = device->lengthPixelWidth (xlen);
  double ysize = device->lengthPixelHeight(ylen);

  if      (minSize >= minSize1) {
    CQChartsRoundedPolygon::draw(device, qrect, xsize, ysize, sides);
  }
  else if (minSize >= minSize2) {
    QPen pen = device->pen();

    QColor pc = pen.color();
    double f  = (minSize - minSize2)/(minSize1 - minSize2);

    pc.setAlphaF(f*pc.alphaF());

    pen.setColor(pc);

    device->setPen(pen);

    CQChartsRoundedPolygon::draw(device, qrect, xsize, ysize, sides);
  }
  else {
    QColor bc = device->brush().color();

    device->setPen(bc);
    device->setBrush(Qt::NoBrush);

    if (prect.width() > prect.height())
      device->drawLine(QPointF(qrect.left (), qrect.center().y()),
                       QPointF(qrect.right(), qrect.center().y()));
    else
      device->drawLine(QPointF(qrect.center().y(), qrect.bottom()),
                       QPointF(qrect.center().y(), qrect.top   ()));
  }
}

void
drawRoundedPolygon(CQChartsPaintDevice *device, const QPolygonF &poly,
                   const CQChartsLength &xlen, const CQChartsLength &ylen)
{
  static double minSize = 2.5; // pixels

  QRectF qrect = poly.boundingRect();

  double xsize = device->lengthPixelWidth (xlen);
  double ysize = device->lengthPixelHeight(ylen);

  if (qrect.width() > minSize && qrect.height() > minSize) {
    CQChartsRoundedPolygon::draw(device, poly, xsize, ysize);
  }
  else {
    QColor bc = device->brush().color();

    device->setPen(bc);
    device->setBrush(Qt::NoBrush);

    device->drawLine(QPointF(qrect.left (), qrect.bottom()),
                     QPointF(qrect.right(), qrect.top   ()));
  }
}

void
drawTextInBox(CQChartsPaintDevice *device, const QRectF &rect, const QString &text,
              const CQChartsTextOptions &options)
{
  if (! rect.isValid())
    return;

  //---

  if (options.html) {
    CQChartsDrawPrivate::drawScaledHtmlText(device, rect, text, options);
    return;
  }

  //---

  QPen pen = device->pen();

  if (options.clipped)
    device->save();

  QRectF prect = device->windowToPixel(rect);

  if (CMathUtil::isZero(options.angle)) {
    QFontMetricsF fm(device->font());

    if (options.clipped)
      device->setClipRect(rect, Qt::IntersectClip);

    device->setPen(pen);

    //---

    QStringList strs;

    if (options.formatted)
      CQChartsUtil::formatStringInRect(text, device->font(), prect, strs);
    else
      strs << text;

    //---

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i)
      tw = std::max(tw, fm.width(strs[i]));

    tw += 2*options.margin;

    double th = strs.size()*fm.height() + 2*options.margin;

    if (options.scaled) {
      double sx = (tw > 0 ? prect.width ()/tw : 1);
      double sy = (th > 0 ? prect.height()/th : 1);

      double s = std::min(sx, sy);

      device->setFont(CQChartsUtil::scaleFontSize(
        device->font(), s, options.minScaleFontSize, options.maxScaleFontSize));

      fm = QFontMetricsF(device->font());

      th = strs.size()*fm.height();
    }

    //---

    double dy = 0.0;

    if      (options.align & Qt::AlignVCenter)
      dy = (prect.height() - th)/2.0;
    else if (options.align & Qt::AlignBottom)
      dy = prect.height() - th;

    double y = prect.top() + dy + fm.ascent();

    for (int i = 0; i < strs.size(); ++i) {
      double dx = 0.0;

      double tw = fm.width(strs[i]);

      if      (options.align & Qt::AlignHCenter)
         dx = (prect.width() - tw)/2;
      else if (options.align & Qt::AlignRight)
         dx = prect.width() - tw;

      double x = prect.left() + dx;

      QPointF pt = device->pixelToWindow(QPointF(x, y));

      if (options.contrast)
        drawContrastText(device, pt, strs[i], options.contrastAlpha);
      else
        drawSimpleText(device, pt, strs[i]);

      y += fm.height();
    }
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
drawRotatedTextInBox(CQChartsPaintDevice *device, const QRectF &rect, const QString &text,
                     const QPen &pen, const CQChartsTextOptions &options)
{
  device->setPen(pen);

  // TODO: support align and contrast
  CQChartsRotatedText::draw(device, rect.center(), text, options.angle,
                            Qt::AlignHCenter | Qt::AlignVCenter, /*alignBox*/false,
                            options.contrast, options.contrastAlpha);
}

//------

void
drawTextAtPoint(CQChartsPaintDevice *device, const QPointF &point, const QString &text,
                const CQChartsTextOptions &options)
{
  QPointF ppoint = device->windowToPixel(point);

  QPen pen = device->pen();

  if (CMathUtil::isZero(options.angle)) {
    QFontMetricsF fm(device->font());

    double tw = fm.width(text);
    double ta = fm.ascent();
    double td = fm.descent();

    double dx = 0.0;

    if      (options.align & Qt::AlignHCenter)
      dx = -tw/2.0;
    else if (options.align & Qt::AlignRight)
      dx = -tw;

    double dy = 0.0;

    if      (options.align & Qt::AlignTop)
      dy = -ta;
    else if (options.align & Qt::AlignVCenter)
      dy = (ta - td)/2.0;
    else if (options.align & Qt::AlignBottom)
      dy = td;

    device->setPen(pen);

    QPointF tp = device->pixelToWindow(QPointF(ppoint.x() + dx, ppoint.y() + dy));

    if (options.contrast)
      drawContrastText(device, tp, text, options.contrastAlpha);
    else
      drawSimpleText(device, tp, text);
  }
  else {
    assert(false);
  }
}

//------

void
drawAlignedText(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
                Qt::Alignment align, double dx, double dy)
{
  QFontMetricsF fm(device->font());

  QPointF pp = device->windowToPixel(p);

  double x1 = pp.x();
  double y1 = pp.y();

  double tw = fm.width(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  if      (align & Qt::AlignLeft)
    x1 = pp.x() + dx;
  else if (align & Qt::AlignRight)
    x1 = pp.x() - tw - dx;
  else if (align & Qt::AlignHCenter)
    x1 = pp.x() - tw/2;

  if      (align & Qt::AlignTop)
    y1 = pp.y() + ta + dy;
  else if (align & Qt::AlignBottom)
    y1 = pp.y() - td - dy;
  else if (align & Qt::AlignVCenter)
    y1 = pp.y() + (ta - td)/2;

  QPointF pt = device->pixelToWindow(QPointF(x1, y1));

  device->drawText(pt, text);
}

//------

QRectF
calcAlignedTextRect(const QFont &font, const QPointF &p, const QString &text,
                    Qt::Alignment align, double dx, double dy)
{
  QFontMetricsF fm(font);

  double x1 = p.x();
  double y1 = p.y();

  double tw = fm.width(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  if      (align & Qt::AlignLeft)
    x1 = p.x() + dx;
  else if (align & Qt::AlignRight)
    x1 = p.x() - tw - dx;
  else if (align & Qt::AlignHCenter)
    x1 = p.x() - tw/2;

  if      (align & Qt::AlignTop)
    y1 = p.y() + ta + dy;
  else if (align & Qt::AlignBottom)
    y1 = p.y() - td - dy;
  else if (align & Qt::AlignVCenter)
    y1 = p.y() + (ta - td)/2;

  return QRectF(x1, y1 - ta, tw, ta + td);
}

//------

void
drawContrastText(CQChartsPaintDevice *device, const QPointF &p, const QString &text, double alpha)
{
  QPen pen = device->pen();

  //---

  // set contrast color
  // TODO: allow set type (invert, bw) and alpha
//QColor icolor = CQChartsUtil::invColor(pen.color());
  QColor icolor = CQChartsUtil::bwColor(pen.color());

  icolor.setAlphaF(alpha);

  //---

  QPointF pp = device->windowToPixel(p);

  // draw contrast outline
  device->setPen(icolor);

  for (int dy = -2; dy <= 2; ++dy) {
    for (int dx = -2; dx <= 2; ++dx) {
      if (dx != 0 || dy != 0) {
        QPointF p1 = device->pixelToWindow(QPointF(pp.x() + dx, pp.y() + dy));

        device->drawText(p1, text);
      }
    }
  }

  //---

  // draw text
  device->setPen(pen);

  device->drawText(p, text);
}

//------

QSizeF
calcTextSize(const QString &text, const QFont &font, const CQChartsTextOptions &options)
{
  if (options.html)
    return CQChartsDrawPrivate::calcHtmlTextSize(text, font, options.margin);

  //---

  QFontMetricsF fm(font);

  return QSizeF(fm.width(text), fm.height());
}

//------

void
drawCenteredText(CQChartsPaintDevice *device, const QPointF &pos, const QString &text)
{
  QFontMetricsF fm(device->font());

  QPointF ppos = device->windowToPixel(pos);

  QPointF ppos1(ppos.x() - fm.width(text)/2, ppos.y() + (fm.ascent() - fm.descent())/2);

  drawSimpleText(device, device->pixelToWindow(ppos1), text);
}

//------

void
drawSimpleText(CQChartsPaintDevice *device, const QPointF &pos, const QString &text)
{
  device->drawText(pos, text);
}

//---

void
drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
           const QPointF &c, const CQChartsLength &size)
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
drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol, const QRectF &rect)
{
  QRectF prect = device->windowToPixel(rect);

  double cx = rect.center().x();
  double cy = rect.center().y();
  double ss = std::min(prect.width(), prect.height());

  CQChartsLength symbolSize(ss/2.0, CQChartsUnits::PIXEL);

  drawSymbol(device, symbol, QPointF(cx, cy), symbolSize);
}

void
drawPieSlice(CQChartsPaintDevice *device, const CQChartsGeom::Point &c,
             double ri, double ro, double a1, double a2,
             bool isInvertX, bool isInvertY)
{
  CQChartsGeom::BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  //---

  QPainterPath path;

  if (! CMathUtil::isZero(ri)) {
    CQChartsGeom::BBox bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

    //---

    double da = (isInvertX != isInvertY ? -1 : 1);

    double ra1 = da*CMathUtil::Deg2Rad(a1);
    double ra2 = da*CMathUtil::Deg2Rad(a2);

    CQChartsGeom::Point p1 = CQChartsGeom::circlePoint(c, ri, ra1);
    CQChartsGeom::Point p2 = CQChartsGeom::circlePoint(c, ro, ra1);
    CQChartsGeom::Point p3 = CQChartsGeom::circlePoint(c, ri, ra2);
    CQChartsGeom::Point p4 = CQChartsGeom::circlePoint(c, ro, ra2);

    path.moveTo(p1.x, p1.y);
    path.lineTo(p2.x, p2.y);

    path.arcTo(bbox.qrect(), -a1, a1 - a2);

    path.lineTo(p4.x, p4.y);
    path.lineTo(p3.x, p3.y);

    path.arcTo(bbox1.qrect(), -a2, a2 - a1);
  }
  else {
    double a21 = a2 - a1;

    if (std::abs(a21) < 360.0) {
      path.moveTo(QPointF(c.x, c.y));

      path.arcTo(bbox.qrect(), -a1, a1 - a2);
    }
    else {
      path.addEllipse(bbox.qrect());
    }
  }

  path.closeSubpath();

  device->drawPath(path);
}

}

//------

namespace CQChartsDrawPrivate {

QSizeF
calcHtmlTextSize(const QString &text, const QFont &font, int margin)
{
  QTextDocument td;

  td.setDocumentMargin(margin);
  td.setHtml(text);
  td.setDefaultFont(font);

  QAbstractTextDocumentLayout *layout = td.documentLayout();

  return layout->documentSize();
}

//------

void
drawScaledHtmlText(CQChartsPaintDevice *device, const QRectF &trect, const QString &text,
                   const CQChartsTextOptions &options)
{
  // calc scale
  QSizeF psize = calcHtmlTextSize(text, device->font(), options.margin);

  double pw = psize.width ();
  double ph = psize.height();

  QRectF ptrect = device->windowToPixel(trect);

  double xs = ptrect.width ()/pw;
  double ys = ptrect.height()/ph;

  double s = std::min(xs, ys);

  //---

  // scale font
  device->setFont(CQChartsUtil::scaleFontSize(device->font(), s));

  //---

  drawHtmlText(device, trect, text, options);
}

void
drawHtmlText(CQChartsPaintDevice *device, const QRectF &trect, const QString &text,
             const CQChartsTextOptions &options)
{
  QRectF ptrect = device->windowToPixel(trect);

  //---

  QImage    image;
  QPainter *painter  = nullptr;
  QPainter *ipainter = nullptr;

  if (device->isInteractive()) {
    painter = dynamic_cast<CQChartsViewPlotPainter *>(device)->painter();
  }
  else {
    image = CQChartsUtil::initImage(QSize(ptrect.width(), ptrect.height()));

    ipainter = new QPainter(&image);

    painter = ipainter;
  }

  //---

  painter->save();

  QTextDocument td;

  td.setDocumentMargin(options.margin);
  td.setHtml(text);
  td.setDefaultFont(device->font());

  QRectF ptrect1 = ptrect.translated(-ptrect.x(), -ptrect.y());

  if (device->isInteractive())
    painter->translate(ptrect.x(), ptrect.y());

  painter->setClipRect(ptrect1, Qt::IntersectClip);

  QAbstractTextDocumentLayout::PaintContext ctx;

  QColor pc = device->pen().color();

  ctx.palette.setColor(QPalette::Text, pc);

  QAbstractTextDocumentLayout *layout = td.documentLayout();

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
    painter->translate(-ptrect.x(), -ptrect.y());

  //---

  painter->restore();

  delete ipainter;
}

}
