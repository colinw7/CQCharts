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

/*
void
setPenBrush(QPainter *painter, const CQChartsPenBrush &penBrush)
{
  painter->setPen  (penBrush.pen);
  painter->setBrush(penBrush.brush);
}
*/

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

#if 0
void
drawRoundedPolygon(QPainter *painter, const QRectF &qrect, double xsize, double ysize,
                   const CQChartsSides &sides)
{
  static double minSize1 = 2.5; // pixels
  static double minSize2 = 1.5; // pixels

  double minSize = std::min(qrect.width(), qrect.height());

  if      (minSize >= minSize1) {
    CQChartsRoundedPolygon::draw(painter, qrect, xsize, ysize, sides);
  }
  else if (minSize >= minSize2) {
    QPen pen = painter->pen();

    QColor pc = pen.color();
    double f  = (minSize - minSize2)/(minSize1 - minSize2);

    pc.setAlphaF(f*pc.alphaF());

    pen.setColor(pc);

    painter->setPen(pen);

    CQChartsRoundedPolygon::draw(painter, qrect, xsize, ysize, sides);
  }
  else {
    QColor bc = painter->brush().color();

    painter->setPen(bc);
    painter->setBrush(Qt::NoBrush);

    if (qrect.width() > qrect.height())
      painter->drawLine(QPointF(qrect.left (), qrect.center().y()),
                        QPointF(qrect.right(), qrect.center().y()));
    else
      painter->drawLine(QPointF(qrect.center().y(), qrect.bottom()),
                        QPointF(qrect.center().y(), qrect.top   ()));
  }
}
#endif

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

#if 0
void
drawRoundedPolygon(QPainter *painter, const QPolygonF &poly, double xsize, double ysize)
{
  static double minSize = 2.5; // pixels

  QRectF qrect = poly.boundingRect();

  if (qrect.width() > minSize && qrect.height() > minSize) {
    CQChartsRoundedPolygon::draw(painter, poly, xsize, ysize);
  }
  else {
    QColor bc = painter->brush().color();

    painter->setPen(bc);
    painter->setBrush(Qt::NoBrush);

    painter->drawLine(QPointF(qrect.left (), qrect.bottom()),
                      QPointF(qrect.right(), qrect.top   ()));
  }
}
#endif

void
drawTextInBox(CQChartsPaintDevice *device, const QRectF &rect, const QString &text,
              const CQChartsTextOptions &options)
{
  if (options.html) {
    CQChartsDrawPrivate::drawScaledHtmlText(device, rect, text, options);
    return;
  }

  //---

  QPen pen = device->pen();

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

      double fs = device->font().pointSizeF()*s;

      fs = CMathUtil::clamp(fs, options.minScaleFontSize, options.maxScaleFontSize);

      QFont font1 = device->font();

      if (fs > 0.0)
        font1.setPointSizeF(fs);

      device->setFont(font1);

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
        drawContrastText(device, pt, strs[i]);
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

  device->restore();
}

#if 0
void
drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
              const CQChartsTextOptions &options)
{
  if (options.html) {
    CQChartsDrawPrivate::drawScaledHtmlText(painter, rect, text, options);
    return;
  }

  //---

  QPen pen = painter->pen();

  painter->save();

  if (CMathUtil::isZero(options.angle)) {
    QFontMetricsF fm(painter->font());

    if (options.clipped)
      painter->setClipRect(rect, Qt::IntersectClip);

    painter->setPen(pen);

    //---

    QStringList strs;

    if (options.formatted)
      CQChartsUtil::formatStringInRect(text, painter->font(), rect, strs);
    else
      strs << text;

    //---

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i)
      tw = std::max(tw, fm.width(strs[i]));

    tw += 2*options.margin;

    double th = strs.size()*fm.height() + 2*options.margin;

    if (options.scaled) {
      double sx = (tw > 0 ? rect.width ()/tw : 1);
      double sy = (th > 0 ? rect.height()/th : 1);

      double s = std::min(sx, sy);

      double fs = painter->font().pointSizeF()*s;

      fs = CMathUtil::clamp(fs, options.minScaleFontSize, options.maxScaleFontSize);

      QFont font1 = painter->font();

      if (fs > 0.0)
        font1.setPointSizeF(fs);

      painter->setFont(font1);

      fm = QFontMetricsF(painter->font());

      th = strs.size()*fm.height();
    }

    //---

    double dy = 0.0;

    if      (options.align & Qt::AlignVCenter)
      dy = (rect.height() - th)/2.0;
    else if (options.align & Qt::AlignBottom)
      dy = rect.height() - th;

    double y = rect.top() + dy + fm.ascent();

    for (int i = 0; i < strs.size(); ++i) {
      double dx = 0.0;

      double tw = fm.width(strs[i]);

      if      (options.align & Qt::AlignHCenter)
         dx = (rect.width() - tw)/2;
      else if (options.align & Qt::AlignRight)
         dx = rect.width() - tw;

      double x = rect.left() + dx;

      CQChartsPixelPainter device(painter);

      if (options.contrast)
        drawContrastText(&device, QPointF(x, y), strs[i]);
      else
        drawSimpleText(&device, QPointF(x, y), strs[i]);

      y += fm.height();
    }
  }
  else {
    if (options.clipped)
      painter->setClipRect(rect, Qt::IntersectClip);

    drawRotatedTextInBox(painter, rect, text, pen, options);
  }

  painter->restore();
}
#endif

void
drawRotatedTextInBox(CQChartsPaintDevice *device, const QRectF &rect, const QString &text,
                     const QPen &pen, const CQChartsTextOptions &options)
{
  device->setPen(pen);

  // TODO: support align and contrast
  CQChartsRotatedText::draw(device, rect.center(), text, options.angle,
                            Qt::AlignHCenter | Qt::AlignVCenter,
                            /*alignBox*/false, options.contrast);
}

#if 0
void
drawRotatedTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
                     const QPen &pen, const CQChartsTextOptions &options)
{
  painter->setPen(pen);

  // TODO: support align and contrast
  CQChartsRotatedText::draw(painter, rect.center(), text, options.angle,
                            Qt::AlignHCenter | Qt::AlignVCenter,
                            /*alignBox*/false, options.contrast);
}
#endif

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
      drawContrastText(device, tp, text);
    else
      drawSimpleText(device, tp, text);
  }
  else {
    assert(false);
  }
}

#if 0
void
drawTextAtPoint(QPainter *painter, const QPointF &point, const QString &text,
                const CQChartsTextOptions &options)
{
  QPen pen = painter->pen();

  if (CMathUtil::isZero(options.angle)) {
    QFontMetricsF fm(painter->font());

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

    painter->setPen(pen);

    CQChartsPixelPainter device(painter);

    if (options.contrast)
      drawContrastText(&device, QPointF(point.x() + dx, point.y() + dy), text);
    else
      drawSimpleText(&device, QPointF(point.x() + dx, point.y() + dy), text);
  }
  else {
    assert(false);
  }
}
#endif

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

#if 0
void
drawAlignedText(QPainter *painter, const QPointF &p, const QString &text,
                Qt::Alignment align, double dx, double dy)
{
  QFontMetricsF fm(painter->font());

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

  painter->drawText(QPointF(x1, y1), text);
}
#endif

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
drawContrastText(CQChartsPaintDevice *device, const QPointF &p, const QString &text)
{
  QPen pen = device->pen();

  //---

  // set contrast color
  // TODO: allow set type (invert, bw) and alpha
//QColor icolor = CQChartsUtil::invColor(pen.color());
  QColor icolor = CQChartsUtil::bwColor(pen.color());

  icolor.setAlphaF(0.5);

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

#if 0
void
drawContrastText(QPainter *painter, const QPointF &p, const QString &text)
{
  QPen pen = painter->pen();

  //---

  // set contrast color
  // TODO: allow set type (invert, bw) and alpha
//QColor icolor = CQChartsUtil::invColor(pen.color());
  QColor icolor = CQChartsUtil::bwColor(pen.color());

  icolor.setAlphaF(0.5);

  //---

  // draw contrast outline
  painter->setPen(icolor);

  for (int dy = -2; dy <= 2; ++dy) {
    for (int dx = -2; dx <= 2; ++dx) {
      if (dx != 0 || dy != 0)
        painter->drawText(QPointF(p.x() + dx, p.y() + dy), text);
    }
  }

  //---

  // draw text
  painter->setPen(pen);

  painter->drawText(p, text);
}
#endif

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

#if 0
void
drawCenteredText(QPainter *painter, const QPointF &pos, const QString &text)
{
  QFontMetricsF fm(painter->font());

  QPointF pos1(pos.x() - fm.width(text)/2, pos.y() + (fm.ascent() - fm.descent())/2);

  drawSimpleText(painter, pos1, text);
}
#endif

//------

void
drawSimpleText(CQChartsPaintDevice *device, const QPointF &pos, const QString &text)
{
  device->drawText(pos, text);
}

#if 0
void
drawSimpleText(QPainter *painter, const QPointF &pos, const QString &text)
{
  painter->drawText(pos, text);
}
#endif

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

#if 0
void
drawSymbol(QPainter *painter, const CQChartsSymbol &symbol,
           const QPointF &c, const QSizeF &size)
{
  CQChartsPlotSymbolRenderer srenderer(painter, CQChartsGeom::Point(c),
                                       CMathUtil::avg(size.width(), size.height()));

  if (painter->brush().style() != Qt::NoBrush) {
    CQChartsPlotSymbolMgr::fillSymbol(symbol, &srenderer);

    if (painter->pen().style() != Qt::NoPen)
      CQChartsPlotSymbolMgr::strokeSymbol(symbol, &srenderer);
  }
  else {
    if (painter->pen().style() != Qt::NoPen)
      CQChartsPlotSymbolMgr::drawSymbol(symbol, &srenderer);
  }
}
#endif

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

#if 0
void
drawSymbol(QPainter *painter, const CQChartsSymbol &symbol, const QRectF &rect)
{
  double cx = rect.center().x();
  double cy = rect.center().y();
  double ss = std::min(rect.width(), rect.height());

  drawSymbol(painter, symbol, QPointF(cx, cy), QSizeF(ss/2.0, ss/2.0));
}
#endif

#if 0
void
drawSymbol(QPainter *painter, const CQChartsSymbol &symbol,
           double cx, double cy, double sw, double sh)
{
  drawSymbol(painter, symbol, QPointF(cx, cy), QSizeF(sw, sh));
}
#endif

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

    path.arcTo(bbox.qrect(), a1, a2 - a1);

    path.lineTo(p4.x, p4.y);
    path.lineTo(p3.x, p3.y);

    path.arcTo(bbox1.qrect(), a2, a1 - a2);
  }
  else {
    double a21 = a2 - a1;

    if (std::abs(a21) < 360.0) {
      path.moveTo(QPointF(c.x, c.y));

      path.arcTo(bbox.qrect(), a1, a2 - a1);
    }
    else {
      path.addEllipse(bbox.qrect());
    }
  }

  path.closeSubpath();

  device->drawPath(path);
}

#if 0
void
drawPieSlice(QPainter *painter, const CQChartsGeom::Point &c,
             double ri, double ro, double a1, double a2, bool isInvertX, bool isInvertY)
{
  CQChartsGeom::BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  CQChartsGeom::BBox pbbox = windowToPixel(bbox);

  //---

  QPainterPath path;

  if (! CMathUtil::isZero(ri)) {
    CQChartsGeom::BBox bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

    CQChartsGeom::BBox pbbox1 = windowToPixel(bbox1);

    //---

    double da = (isInvertX != isInvertY ? -1 : 1);

    double ra1 = da*CMathUtil::Deg2Rad(a1);
    double ra2 = da*CMathUtil::Deg2Rad(a2);

    CQChartsGeom::Point p1 = windowToPixel(CQChartsGeom::circlePoint(c, ri, ra1));
    CQChartsGeom::Point p2 = windowToPixel(CQChartsGeom::circlePoint(c, ro, ra1));
    CQChartsGeom::Point p3 = windowToPixel(CQChartsGeom::circlePoint(c, ri, ra2));
    CQChartsGeom::Point p4 = windowToPixel(CQChartsGeom::circlePoint(c, ro, ra2));

    path.moveTo(p1.x, p1.y);
    path.lineTo(p2.x, p2.y);

    path.arcTo(pbbox.qrect(), a1, a2 - a1);

    path.lineTo(p4.x, p4.y);
    path.lineTo(p3.x, p3.y);

    path.arcTo(pbbox1.qrect(), a2, a1 - a2);
  }
  else {
    CQChartsGeom::Point pc = windowToPixel(c);

    //---

    double a21 = a2 - a1;

    if (std::abs(a21) < 360.0) {
      path.moveTo(QPointF(pc.x, pc.y));

      path.arcTo(pbbox.qrect(), a1, a2 - a1);
    }
    else {
      path.addEllipse(pbbox.qrect());
    }
  }

  path.closeSubpath();

  painter->drawPath(path);
}
#endif

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
  QFont font1 = device->font();

  double fs = font1.pointSizeF()*s;

  if (fs > 0.0)
    font1.setPointSizeF(fs);

  device->setFont(font1);

  //---

  drawHtmlText(device, trect, text, options);
}

#if 0
void
drawScaledHtmlText(QPainter *painter, const QRectF &trect, const QString &text,
                   const CQChartsTextOptions &options)
{
  // calc scale
  QSizeF psize = calcHtmlTextSize(text, painter->font(), options.margin);

  double pw = psize.width ();
  double ph = psize.height();

  double xs = trect.width ()/pw;
  double ys = trect.height()/ph;

  double s = std::min(xs, ys);

  //---

  // scale font
  QFont font1 = painter->font();

  double fs = font1.pointSizeF()*s;

  if (fs > 0.0)
    font1.setPointSizeF(fs);

  painter->setFont(font1);

  //---

  drawHtmlText(painter, trect, text, options);
}
#endif

void
drawHtmlText(CQChartsPaintDevice *device, const QRectF &trect, const QString &text,
             const CQChartsTextOptions &options)
{
  QRectF ptrect = device->windowToPixel(trect);

  //---

  QImage    image;
  QPainter *painter  = nullptr;
  QPainter *ipainter = nullptr;

  if (device->type() != CQChartsPaintDevice::Type::SCRIPT) {
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

  if (device->type() != CQChartsPaintDevice::Type::SCRIPT)
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

  if (device->type() != CQChartsPaintDevice::Type::SCRIPT)
    painter->translate(-ptrect.x(), -ptrect.y());

  //---

  painter->restore();

  delete ipainter;
}

#if 0
void
drawHtmlText(QPainter *painter, const QRectF &trect, const QString &text,
             const CQChartsTextOptions &options)
{
  painter->save();

  //---

  QTextDocument td;

  td.setDocumentMargin(options.margin);
  td.setHtml(text);
  td.setDefaultFont(painter->font());

  QRectF trect1 = trect.translated(-trect.x(), -trect.y());

  painter->translate(trect.x(), trect.y());

  painter->setClipRect(trect1, Qt::IntersectClip);

  QAbstractTextDocumentLayout::PaintContext ctx;

  QColor pc = painter->pen().color();

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

  painter->translate(-trect.x(), -trect.y());

  //---

  painter->restore();
}
#endif

}
