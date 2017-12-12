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

  if      (align & Qt::AlignLeft)
    x1 = x + dx;
  else if (align & Qt::AlignRight)
    x1 = x - fm.width(text) - dx;
  else if (align & Qt::AlignHCenter)
    x1 = x - fm.width(text)/2;

  if      (align & Qt::AlignTop)
    y1 = y + fm.ascent() + dy;
  else if (align & Qt::AlignBottom)
    y1 = y - fm.descent() - dy;
  else if (align & Qt::AlignVCenter)
    y1 = y + (fm.ascent() - fm.descent())/2;

  painter->drawText(QPointF(x1, y1), text);
}
