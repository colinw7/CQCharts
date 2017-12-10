#include <CQChartsViewExpander.h>
#include <CQChartsWindow.h>

#include <QPainter>
#include <QMouseEvent>
#include <QMenu>

CQChartsViewExpander::
CQChartsViewExpander(CQChartsWindow *window, QWidget *w, const Side &side) :
 QFrame(window), window_(window), w_(w), side_(side)
{
  setObjectName("expander");

  setAutoFillBackground(true);

  updateGeometry();
}

void
CQChartsViewExpander::
setSide(const Side &side)
{
  side_ = side;

  updateGeometry();
}

void
CQChartsViewExpander::
setExpanded(bool b)
{
  expanded_ = b;

  updateGeometry();
}

void
CQChartsViewExpander::
setMargins(int l, int b, int r, int t)
{
  l_ = l; b_ = b;
  r_ = r; t_ = t;
}

bool
CQChartsViewExpander::
isVertical() const
{
  return (side_ == Side::LEFT || side_ == Side::RIGHT);
}

void
CQChartsViewExpander::
updateGeometry()
{
  if (isVertical()) {
    setMinimumHeight(0); setMaximumHeight(QWIDGETSIZE_MAX);
    setFixedWidth(8);
  }
  else {
    setMinimumWidth(0); setMaximumWidth(QWIDGETSIZE_MAX);
    setFixedHeight(8);
  }

  w_->setVisible(isExpanded());

  if (isExpanded()) {
    if      (side_ == Side::RIGHT) {
      w_->move  (window_->width() - w_->width() - r_, 0);
      w_->resize(w_->width(), window_->height());

      move(window_->width() - w_->width() - width() - r_, 0);
    }
    else if (side_ == Side::LEFT) {
      w_->move  (l_, 0);
      w_->resize(w_->width(), window_->height());

      move(w_->width() + l_, 0);
    }
    else if (side_ == Side::BOTTOM) {
      w_->move  (0, window_->height() - w_->height() - b_);
      w_->resize(window_->width(), w_->height());

      move(0, window_->height() - w_->height() - height() - b_);
    }
    else if (side_ == Side::TOP) {
      w_->move  (0, t_);
      w_->resize(window_->width(), w_->height());

      move(0, w_->height() + t_);
    }
  }
  else {
    if      (side_ == Side::RIGHT) {
      move(window_->width() - width() - r_, 0);
    }
    else if (side_ == Side::LEFT) {
      move(l_, 0);
    }
    else if (side_ == Side::BOTTOM) {
      move(0, window_->height() - height() - b_);
    }
    else if (side_ == Side::TOP) {
      move(0, t_);
    }
  }

  if (isVertical())
    resize(width(), window_->height());
  else
    resize(window_->width(), height());

  update();
}

void
CQChartsViewExpander::
mousePressEvent(QMouseEvent *me)
{
  if      (me->button() == Qt::LeftButton) {
    QRect handleRect;

    if (isVertical()) {
      int s  = width();
      int ym = height()/2;

      handleRect = QRect(0, ym - s, width(), 2*s);
    }
    else {
      int s  = height();
      int xm = width()/2;

      handleRect = QRect(xm - s, 0, 2*s, height());
    }

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
  else if (me->button() == Qt::RightButton) {
    QMenu *menu = new QMenu;

    QActionGroup *sideGroup = new QActionGroup(menu);

    if (isVertical()) {
      QAction *leftAction  = new QAction("Left" , menu);
      QAction *rightAction = new QAction("Right", menu);

      leftAction ->setCheckable(true);
      rightAction->setCheckable(true);

      leftAction ->setChecked(side_ == Side::LEFT);
      rightAction->setChecked(side_ == Side::RIGHT);

      sideGroup->addAction(leftAction);
      sideGroup->addAction(rightAction);

      connect(leftAction , SIGNAL(triggered()), this, SLOT(leftSlot()));
      connect(rightAction, SIGNAL(triggered()), this, SLOT(rightSlot()));
    }
    else {
      QAction *topAction    = new QAction("Top"   , menu);
      QAction *bottomAction = new QAction("Bottom", menu);

      topAction   ->setCheckable(true);
      bottomAction->setCheckable(true);

      topAction   ->setChecked(side_ == Side::LEFT );
      bottomAction->setChecked(side_ == Side::RIGHT);

      sideGroup->addAction(topAction   );
      sideGroup->addAction(bottomAction);

      connect(topAction   , SIGNAL(triggered()), this, SLOT(topSlot()));
      connect(bottomAction, SIGNAL(triggered()), this, SLOT(bottomSlot()));
    }

    menu->addActions(sideGroup->actions());

    //menu->popup(me->globalPos());
    menu->exec(me->globalPos());

    delete menu;
  }
}

void
CQChartsViewExpander::
mouseMoveEvent(QMouseEvent *me)
{
  if (pressed_) {
    movePos_ = me->pos();

    if (isVertical()) {
      int dx = movePos_.x() - pressPos_.x();

      move(x() + dx, y());

      if (side_ == Side::RIGHT) {
        w_->resize(w_->width() - dx, w_->height());

        w_->move(w_->x() + dx, w_->y());
      }
      else {
        w_->resize(w_->width() + dx, w_->height());
      }

      w_->raise();
    }
    else {
      int dy = movePos_.y() - pressPos_.y();

      move(x(), y() + dy);

      if (side_ == Side::BOTTOM) {
        w_->resize(w_->width(), w_->height() - dy);

        w_->move(w_->x(), w_->y() + dy);
      }
      else {
        w_->resize(w_->width(), w_->height() + dy);
      }

      w_->raise();
    }

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

  p.setBrush(palette().text().color());

  QPolygonF poly;

  if (isVertical()) {
    int s  = width();
    int ym = height()/2;

    if (side_ == Side::RIGHT) {
      if (! expanded_)
        poly << QPoint(0, ym    ) << QPoint(s, ym - s) << QPoint(s, ym + s);
      else
        poly << QPoint(s, ym    ) << QPoint(0, ym - s) << QPoint(0, ym + s);
    }
    else {
      if (! expanded_)
        poly << QPoint(s, ym    ) << QPoint(0, ym - s) << QPoint(0, ym + s);
      else
        poly << QPoint(0, ym    ) << QPoint(s, ym - s) << QPoint(s, ym + s);
    }
  }
  else {
    int s  = height();
    int xm = width()/2;

    if (side_ == Side::BOTTOM) {
      if (! expanded_)
        poly << QPoint(xm    , 0) << QPoint(xm - s, s) << QPoint(xm + s, s);
      else
        poly << QPoint(xm    , s) << QPoint(xm - s, 0) << QPoint(xm + s, 0);
    }
    else {
      if (! expanded_)
        poly << QPoint(xm    , s) << QPoint(xm - s, 0) << QPoint(xm + s, 0);
      else
        poly << QPoint(xm    , 0) << QPoint(xm - s, s) << QPoint(xm + s, s);
    }
  }

  p.drawPolygon(poly);
}

void
CQChartsViewExpander::
leftSlot()
{
  setSide(Side::LEFT);
}

void
CQChartsViewExpander::
rightSlot()
{
  setSide(Side::RIGHT);
}

void
CQChartsViewExpander::
topSlot()
{
  setSide(Side::TOP);
}

void
CQChartsViewExpander::
bottomSlot()
{
  setSide(Side::BOTTOM);
}
