#include <CQChartsViewExpander.h>
#include <CQChartsWindow.h>
#include <QPainter>
#include <QMouseEvent>

CQChartsViewExpander::
CQChartsViewExpander(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("expander");

  setAutoFillBackground(true);

  setFixedWidth(8);
}

void
CQChartsViewExpander::
mousePressEvent(QMouseEvent *me)
{
  if (me->button() == Qt::LeftButton) {
    int s  = width();
    int ym = height()/2;

    QRect handleRect(0, ym - s, width(), 2*s);

    if (handleRect.contains(me->pos())) {
      setExpanded(! isExpanded());

      window_->updateGeometry();

      return;
    }

    if (! isExpanded())
      return;

    pressed_  = true;
    pressPos_ = me->pos();
  }
}

void
CQChartsViewExpander::
mouseMoveEvent(QMouseEvent *me)
{
  if (pressed_) {
    movePos_ = me->pos();

    int dx = pressPos_.x() - movePos_.x();

    window_->moveExpander(dx);

    movePos_ = pressPos_;
  }
}

void
CQChartsViewExpander::
mouseReleaseEvent(QMouseEvent *me)
{
  mouseMoveEvent(me);

  pressed_ = false;
}

void
CQChartsViewExpander::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  int s  = width();
  int ym = height()/2;

  p.setBrush(palette().text().color());

  QPolygonF poly;

  if (! expanded_) {
    poly << QPoint(0, ym    );
    poly << QPoint(s, ym - s);
    poly << QPoint(s, ym + s);
  }
  else {
    poly << QPoint(s, ym    );
    poly << QPoint(0, ym - s);
    poly << QPoint(0, ym + s);
  }

  p.drawPolygon(poly);
}
