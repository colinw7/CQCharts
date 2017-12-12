#include <CQChartsViewExpander.h>

#include <QPainter>
#include <QMouseEvent>
#include <QMenu>

#include <iostream>

CQChartsViewExpander::
CQChartsViewExpander(QWidget *parent, QWidget *w, const Side &side) :
 QFrame(parent), parent_(parent), w_(w), side_(side)
{
  setObjectName("expander");

  setMouseTracking(true);

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

  if (expanded_)
    raise();
}

void
CQChartsViewExpander::
setDetached(bool b)
{
  detached_ = b;

  if (detached_) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);

    setParent(nullptr);

    w_->setParent(this);
  }
  else {
    setWindowFlags(Qt::Window);

    setParent(parent_);

    w_->setParent(parent_);
  }

  w_->show();

  updateGeometry();

  show();
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
  if (! detached_) {
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
        w_->move  (parent_->width() - w_->width() - r_, 0);
        w_->resize(w_->width(), parent_->height());

        move(parent_->width() - w_->width() - width() - r_, 0);
      }
      else if (side_ == Side::LEFT) {
        w_->move  (l_, 0);
        w_->resize(w_->width(), parent_->height());

        move(w_->width() + l_, 0);
      }
      else if (side_ == Side::BOTTOM) {
        w_->move  (0, parent_->height() - w_->height() - b_);
        w_->resize(parent_->width(), w_->height());

        move(0, parent_->height() - w_->height() - height() - b_);
      }
      else if (side_ == Side::TOP) {
        w_->move  (0, t_);
        w_->resize(parent_->width(), w_->height());

        move(0, w_->height() + t_);
      }
    }
    else {
      if      (side_ == Side::RIGHT) {
        move(parent_->width() - width() - r_, 0);
      }
      else if (side_ == Side::LEFT) {
        move(l_, 0);
      }
      else if (side_ == Side::BOTTOM) {
        move(0, parent_->height() - height() - b_);
      }
      else if (side_ == Side::TOP) {
        move(0, t_);
      }
    }

    if (isVertical())
      resize(width(), parent_->height());
    else
      resize(parent_->width(), height());
  }
  else {
    QFontMetrics fm(font());

    titleHeight_ = fm.height() + 2*border_;

    setMinimumWidth (2*border_               ); setMaximumWidth (QWIDGETSIZE_MAX);
    setMinimumHeight(2*border_ + titleHeight_); setMaximumHeight(QWIDGETSIZE_MAX);

    w_->move(border_, titleHeight_ + border_);

    resize(w_->width() + 2*border_, w_->height() + 2*border_ + titleHeight_);
  }

  update();
}

void
CQChartsViewExpander::
mousePressEvent(QMouseEvent *me)
{
  if      (me->button() == Qt::LeftButton) {
    if (! detached_) {
      QRect handleRect = this->handleRect();

      if (handleRect.contains(me->pos())) {
        setExpanded(! isExpanded());

        //emit updateParentGeometry();

        return;
      }

      if (! isExpanded())
        return;
    }
    else {
      pressSide_ = posToPressSide(me->pos());
    }

    pressed_  = true;
    pressPos_ = me->globalPos();
    movePos_  = pressPos_;
  }
  else if (me->button() == Qt::RightButton) {
    QMenu *menu = new QMenu;

    QActionGroup *sideGroup = new QActionGroup(menu);

    if (isVertical()) {
      QAction *leftAction  = new QAction("Left"  , menu);
      QAction *rightAction = new QAction("Right" , menu);

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

    menu->addSeparator();

    QAction *detachAction = new QAction((! isDetached() ? "Detach" : "Attach"), menu);

    connect(detachAction, SIGNAL(triggered()), this, SLOT(detachSlot()));

    menu->addAction(detachAction);

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
    QPoint lastMovePos = movePos_;

    movePos_ = me->globalPos();

    if (! detached_) {
      if (isVertical()) {
        int dx = movePos_.x() - lastMovePos.x();

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
        int dy = movePos_.y() - lastMovePos.y();

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
    }
    else {
      if      (pressSide_ == PressSide::NONE) {
        int dx = movePos_.x() - lastMovePos.x();
        int dy = movePos_.y() - lastMovePos.y();

        move(x() + dx, y() + dy);
      }
      else {
        int dx = movePos_.x() - lastMovePos.x();
        int dy = movePos_.y() - lastMovePos.y();

        if (pressSide_ == PressSide::TOP_LEFT) {
          move(x() + dx, y() + dy);

          w_->resize(w_->width() - dx, w_->height() - dy);
        }
        else if (pressSide_ == PressSide::TOP_RIGHT) {
          move(x(), y() + dy);

          w_->resize(w_->width() + dx, w_->height() - dy);
        }
        else if (pressSide_ == PressSide::BOTTOM_LEFT) {
          move(x() + dx, y());

          w_->resize(w_->width() - dx, w_->height() + dy);
        }
        else if (pressSide_ == PressSide::BOTTOM_RIGHT) {
          w_->resize(w_->width() + dx, w_->height() + dy);
        }
        else if (pressSide_ == PressSide::LEFT) {
          move(x() + dx, y());

          w_->resize(w_->width() - dx, w_->height());
        }
        else if (pressSide_ == PressSide::RIGHT) {
          w_->resize(w_->width() + dx, w_->height());
        }
        else if (pressSide_ == PressSide::BOTTOM) {
          w_->resize(w_->width(), w_->height() + dy);
        }
        else if (pressSide_ == PressSide::TOP) {
          move(x(), y() + dy);

          w_->resize(w_->width(), w_->height() - dy);
        }

        updateGeometry();
      }
    }
  }
  else {
    if (! detached_) {
      QRect handleRect = this->handleRect();

      if (handleRect.contains(me->pos())) {
        if (isVertical())
          setCursor(Qt::SizeHorCursor);
        else
          setCursor(Qt::SizeVerCursor);
      }
      else
        setCursor(Qt::ArrowCursor);
    }
    else {
      PressSide pressSide = posToPressSide(me->pos());

      switch (pressSide) {
        case PressSide::TOP_LEFT:
        case PressSide::BOTTOM_RIGHT:
          setCursor(Qt::SizeFDiagCursor);
          break;
        case PressSide::BOTTOM_LEFT:
        case PressSide::TOP_RIGHT:
          setCursor(Qt::SizeBDiagCursor);
          break;
        case PressSide::TOP:
        case PressSide::BOTTOM:
          setCursor(Qt::SizeVerCursor);
          break;
        case PressSide::LEFT:
        case PressSide::RIGHT:
          setCursor(Qt::SizeHorCursor);
          break;
        case PressSide::NONE:
          setCursor(Qt::SizeAllCursor);
          break;
        default:
          setCursor(Qt::ArrowCursor);
          break;
      }
    }
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
resizeEvent(QResizeEvent *)
{
  //std::cerr << width() << " " << height() << "\n";
}

void
CQChartsViewExpander::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setBrush(palette().text().color());

  if (! detached_) {
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
  else {
    for (int y = border_; y <= titleHeight_ - border_; y += 3) {
      p.drawLine(border_, y, width() - border_, y);
    }
  }
}

QRect
CQChartsViewExpander::
handleRect() const
{
  if (isVertical()) {
    int s  = width();
    int ym = height()/2;

    return QRect(0, ym - s, width(), 2*s);
  }
  else {
    int s  = height();
    int xm = width()/2;

    return QRect(xm - s, 0, 2*s, height());
  }
}

CQChartsViewExpander::PressSide
CQChartsViewExpander::
posToPressSide(const QPoint &pos)
{
  if      (pos.x() < border_ && pos.y() < border_)
    return PressSide::TOP_LEFT;
  else if (pos.x() < border_ && pos.y() > height() - border_)
    return PressSide::BOTTOM_LEFT;
  else if (pos.x() > width() - border_ && pos.y() < border_)
    return PressSide::TOP_RIGHT;
  else if (pos.x() > width() - border_ && pos.y() > height() - border_)
    return PressSide::BOTTOM_RIGHT;
  else if (pos.x() < border_)
    return PressSide::LEFT;
  else if (pos.x() > width() - border_)
    return PressSide::RIGHT;
  else if (pos.y() < border_)
    return PressSide::TOP;
  else if (pos.y() > height() - border_)
    return PressSide::BOTTOM;
  else
    return PressSide::NONE;
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

void
CQChartsViewExpander::
detachSlot()
{
  setDetached(! isDetached());
}
