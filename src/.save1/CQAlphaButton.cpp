#include <CQAlphaButton.h>

#include <QPainter>
#include <QMouseEvent>

CQAlphaButton::
CQAlphaButton(QWidget *parent) :
 QWidget(parent), alpha_(1.0), fg_(0,0,0), bg_(196,196,196)
{
  setFixedWidth(8);

  setToolTip(QString("%1").arg(alpha_));
}

void
CQAlphaButton::
setColor(QColor fg)
{
  fg_ = fg;

  update();
}

double
CQAlphaButton::
getAlpha() const
{
  return alpha_;
}

void
CQAlphaButton::
setAlpha(double alpha)
{
  alpha_ = alpha;

  setToolTip(QString("%1").arg(alpha_));

  update();
}

void
CQAlphaButton::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  int w = width();
  int h = height();

  int pos = alphaToPos();

  double dg = (h > 1 ? 1.0/(h - 1) : 0.0);

  int fr = fg_.red  ();
  int fg = fg_.green();
  int fb = fg_.blue ();

  int br = bg_.red  ();
  int bg = bg_.green();
  int bb = bg_.blue ();

  for (int i = 0; i < h; ++i) {
    double rg  = (h - 1 - i)*dg;
    double rg1 = 1 - rg;

    int ir1 = std::min(int(br*rg1 + fr*rg), 255);
    int ig1 = std::min(int(bg*rg1 + fg*rg), 255);
    int ib1 = std::min(int(bb*rg1 + fb*rg), 255);

    QColor c(ir1, ig1, ib1);

    p.setPen(c);

    p.drawLine(0, i, w - 1, i);
  }

  QColor c(255, 0, 0);

  p.setPen(c);

  p.drawLine(0, pos, w - 1, pos);
}

void
CQAlphaButton::
mousePressEvent(QMouseEvent *e)
{
  posToAlpha(e->pos().y());

  update();
}

void
CQAlphaButton::
mouseMoveEvent(QMouseEvent *e)
{
  posToAlpha(e->pos().y());

  update();
}

void
CQAlphaButton::
mouseReleaseEvent(QMouseEvent *e)
{
  posToAlpha(e->pos().y());

  update();
}

void
CQAlphaButton::
posToAlpha(int pos)
{
  double alpha = std::min(std::max(0.0, (1.0*pos)/(height() - 1)), 1.0);

  setAlpha(1.0 - alpha);

  emit valueChanged();
}

int
CQAlphaButton::
alphaToPos()
{
  int pos = std::min(std::max(0, int(alpha_*(height() - 1))), height() - 1);

  return (height() - 1 - pos);
}
