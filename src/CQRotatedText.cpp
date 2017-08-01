#include <CQRotatedText.h>

#include <QPainter>
#include <cmath>

namespace CQRotatedText {

void
drawRotatedText(QPainter *painter, double x, double y, const QString &text,
                double angle, Qt::Alignment align)
{
  painter->save();

  QFontMetrics fm(painter->font());

  int th = fm.height();
  int tw = fm.width(text);

  double dx = 0.0;

  if      (align & Qt::AlignLeft)
    dx = 0;
  else if (align & Qt::AlignRight)
    dx = -tw;
  else if (align & Qt::AlignHCenter)
    dx = -tw/2.0;

  double dy = 0.0;

  if      (align & Qt::AlignBottom)
    dy = 0;
  else if (align & Qt::AlignTop)
    dy = th;
  else if (align & Qt::AlignVCenter)
    dy = th/2.0;

  double a1 = M_PI*angle/180.0;

  double c = cos(a1);
  double s = sin(a1);

  double tx = c*dx - s*dy;
  double ty = s*dx + c*dy;

  QTransform t;

  t.translate(x + tx, y + ty);
  t.rotate(angle);
//t.translate(0, -fm.descent());

  painter->setTransform(t);

  painter->drawText(0, 0, text);

  painter->restore();
}

}
