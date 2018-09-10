#include <CQChartsDrawUtil.h>
#include <QPainter>

void
CQChartsDrawUtil::
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

QRectF
CQChartsDrawUtil::
alignedTextRect(const QFont &font, double x, double y, const QString &text,
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
