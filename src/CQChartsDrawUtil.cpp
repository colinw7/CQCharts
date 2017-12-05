#include <CQChartsDrawUtil.h>
#include <CQChartsRenderer.h>

void
CQChartsDrawUtil::
drawAlignedText(CQChartsRenderer *renderer, double x, double y, const QString &text,
                Qt::Alignment align, double dx, double dy)
{
  QFontMetricsF fm(renderer->font());

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

  renderer->drawText(QPointF(x1, y1), text);
}
