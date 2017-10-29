#include <CQChartsViewExpander.h>
#include <CQChartsView.h>
#include <QPainter>
#include <QMouseEvent>

CQChartsViewExpander::
CQChartsViewExpander(CQChartsView *view) :
 QFrame(view), view_(view)
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

      view_->updateGeometry();

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

    view_->moveExpander(dx);

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

  p.setBrush(QColor(0,0,0));

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
