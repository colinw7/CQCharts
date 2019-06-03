#include <CQChartsDrawUtil.h>
#include <CQChartsRotatedText.h>
#include <CQChartsUtil.h>

#include <CMathUtil.h>

#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

namespace CQChartsDrawUtil {

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
      painter->setClipRect(rect);

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

      if (options.contrast)
        drawContrastText(painter, x, y, strs[i]);
      else
        drawSimpleText(painter, x, y, strs[i]);

      y += fm.height();
    }
  }
  else {
    drawRotatedTextInBox(painter, rect, text, pen, options);
  }

  painter->restore();
}

void
drawRotatedTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
                     const QPen &pen, const CQChartsTextOptions &options)
{
  painter->setPen(pen);

  // TODO: support align and contrast
  CQChartsRotatedText::draw(painter, rect.center().x(), rect.center().y(),
                            text, options.angle, Qt::AlignHCenter | Qt::AlignVCenter,
                            /*alignBox*/false, options.contrast);
}

//------

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

    if (options.contrast)
      drawContrastText(painter, point.x() + dx, point.y() + dy, text);
    else
      drawSimpleText(painter, point.x() + dx, point.y() + dy, text);
  }
  else {
    assert(false);
  }
}

//------

void
drawAlignedText(QPainter *painter, double x, double y, const QString &text,
                Qt::Alignment align, double dx, double dy)
{
  QFontMetricsF fm(painter->font());

  double x1 = x;
  double y1 = y;

  double tw = fm.width(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  if      (align & Qt::AlignLeft)
    x1 = x + dx;
  else if (align & Qt::AlignRight)
    x1 = x - tw - dx;
  else if (align & Qt::AlignHCenter)
    x1 = x - tw/2;

  if      (align & Qt::AlignTop)
    y1 = y + ta + dy;
  else if (align & Qt::AlignBottom)
    y1 = y - td - dy;
  else if (align & Qt::AlignVCenter)
    y1 = y + (ta - td)/2;

  painter->drawText(QPointF(x1, y1), text);
}

//------

QRectF
calcAlignedTextRect(const QFont &font, double x, double y, const QString &text,
                    Qt::Alignment align, double dx, double dy)
{
  QFontMetricsF fm(font);

  double x1 = x;
  double y1 = y;

  double tw = fm.width(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  if      (align & Qt::AlignLeft)
    x1 = x + dx;
  else if (align & Qt::AlignRight)
    x1 = x - tw - dx;
  else if (align & Qt::AlignHCenter)
    x1 = x - tw/2;

  if      (align & Qt::AlignTop)
    y1 = y + ta + dy;
  else if (align & Qt::AlignBottom)
    y1 = y - td - dy;
  else if (align & Qt::AlignVCenter)
    y1 = y + (ta - td)/2;

  return QRectF(x1, y1 - ta, tw, ta + td);
}

//------

void drawContrastText(QPainter *painter, double x, double y, const QString &text) {
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
        painter->drawText(QPointF(x + dx, y + dy), text);
    }
  }

  //---

  // draw text
  painter->setPen(pen);

  painter->drawText(QPointF(x, y), text);
}

//------

QSizeF calcTextSize(const QString &text, const QFont &font, const CQChartsTextOptions &options)
{
  if (options.html)
    return CQChartsDrawPrivate::calcHtmlTextSize(text, font, options.margin);

  //---

  QFontMetricsF fm(font);

  return QSizeF(fm.width(text), fm.height());
}

//------

void drawCenteredText(QPainter *painter, const QPointF &pos, const QString &text)
{
  QFontMetricsF fm(painter->font());

  QPointF pos1(pos.x() - fm.width(text)/2, pos.y() + (fm.ascent() - fm.descent())/2);

  drawSimpleText(painter, pos1, text);
}

//------

void drawSimpleText(QPainter *painter, double x, double y, const QString &text)
{
  drawSimpleText(painter, QPointF(x, y), text);
}

void drawSimpleText(QPainter *painter, const QPointF &pos, const QString &text)
{
  painter->drawText(pos, text);
}

}

//------

namespace CQChartsDrawPrivate {

QSizeF calcHtmlTextSize(const QString &text, const QFont &font, int margin)
{
  QTextDocument td;

  td.setDocumentMargin(margin);
  td.setHtml(text);
  td.setDefaultFont(font);

  QAbstractTextDocumentLayout *layout = td.documentLayout();

  return layout->documentSize();
}

//------

void drawScaledHtmlText(QPainter *painter, const QRectF &trect, const QString &text,
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

void drawHtmlText(QPainter *painter, const QRectF &trect, const QString &text,
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

  painter->setClipRect(trect1);

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

}
