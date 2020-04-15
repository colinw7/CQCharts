#include <CQChartsViewExpander.h>

#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QTimer>

#include <iostream>

CQChartsViewExpander::
CQChartsViewExpander(QWidget *parent, QWidget *w, const Side &side) :
 QFrame(parent), parent_(parent), w_(w), side_(side)
{
  setObjectName("expander");

  setMouseTracking(true);

  setAutoFillBackground(true);

  windowFlags_ = this->windowFlags();

  detachTimer_ = new QTimer(this); detachTimer_->setSingleShot(true);

  connect(detachTimer_, SIGNAL(timeout()), this, SLOT(detachSlot()));

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
    w_->raise();

  raise();
}

void
CQChartsViewExpander::
setDetached(bool b)
{
  detached_ = b;

  if (detached_) {
    setParent(nullptr);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);

    w_->setParent(this);
  }
  else {
    setParent(parent_);

    setWindowFlags(windowFlags_);

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
      int maxSize = this->maxSize();

      if (side_ == Side::LEFT || side_ == Side::RIGHT) {
        int ww = w_->width();

        if (ww > maxSize)
          ww = maxSize;

        if (side_ == Side::RIGHT) {
          w_->move  (parent_->width() - ww - r_, 0);
          w_->resize(ww, parent_->height());

          move(parent_->width() - ww - width() - r_, 0);
        }
        else {
          w_->move  (l_, 0);
          w_->resize(ww, parent_->height());

          move(ww + l_, 0);
        }
      }
      else {
        int wh = w_->height();

        if (wh >= maxSize)
          wh = maxSize;

        if (side_ == Side::BOTTOM) {
          w_->move  (0, parent_->height() - wh - b_);
          w_->resize(parent_->width(), wh);

          move(0, parent_->height() - wh - height() - b_);
        }
        else {
          w_->move  (0, t_);
          w_->resize(parent_->width(), wh);

          move(0, wh + t_);
        }
      }

      w_->raise();
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

    raise();
  }
  else {
    QFontMetrics fm(font());

    titleHeight_ = fm.height() + 2*border_;

    setMinimumWidth (2*border_               ); setMaximumWidth (QWIDGETSIZE_MAX);
    setMinimumHeight(2*border_ + titleHeight_); setMaximumHeight(QWIDGETSIZE_MAX);

    w_->move(border_, titleHeight_ + border_);

    resize(w_->width() + 2*border_, w_->height() + 2*border_ + titleHeight_);
  }

  QFrame::updateGeometry();

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
    auto *menu = new QMenu;

    auto *sideGroup = new QActionGroup(menu);

    if (isVertical()) {
      auto *leftAction  = new QAction("Left"  , menu);
      auto *rightAction = new QAction("Right" , menu);

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
      auto *topAction    = new QAction("Top"   , menu);
      auto *bottomAction = new QAction("Bottom", menu);

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

    auto *detachAction = new QAction((! isDetached() ? "Detach" : "Attach"), menu);

    connect(detachAction, SIGNAL(triggered()), this, SLOT(detachLaterSlot()));

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
      }

      w_->raise();
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
          setCursor(Qt::SplitHCursor);
        else
          setCursor(Qt::SplitVCursor);
      }
      else {
        if (isVertical())
          setCursor(Qt::SizeHorCursor);
        else
          setCursor(Qt::SizeVerCursor);
      }
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

  updateGeometry();
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
    QRect titleRect(0, 0, width(), titleHeight_);

    drawTitleLines(&p, titleRect);

    QFontMetrics fm(font());

    int tw = (title_.length() ? fm.width(title_) : 0);

    int is = (! icon_.isNull() ? fm.ascent() : 0);

    QRect clearRect(border_, 0, is + tw + 2*border_ + (is > 0 ? border_ : 0), titleHeight_);

    p.fillRect(clearRect, QBrush(palette().window().color()));

    double x = border_;

    if (! icon_.isNull()) {
      int iy = (titleHeight_ - is)/2;

      p.drawPixmap(int(x), iy, icon_.pixmap(QSize(is, is)));

      x += is + border_;
    }

    if (title_.length()) {
      QColor c = this->palette().color(QPalette::WindowText);

      p.setPen(c);

      p.drawText(int(x), border_ + fm.ascent(), title_);
    }
  }
}

// draw title lines
void
CQChartsViewExpander::
drawTitleLines(QPainter *p, const QRect &r)
{
  int num_lines = (r.height() - 4)/5;

//int h = r.height();

//int gap = h/(num_lines + 2);
  int gap = 3;

  int left  = r.left () + border_;
  int right = r.right() - border_;

  int y = r.center().y() - (gap + 1)*num_lines/2;

  QColor c1 = this->palette().color(QPalette::WindowText);
  QColor c2 = c1;

  c1.setAlphaF(0.6);
  c2.setAlphaF(0.4);

  for (int i = 0; i < num_lines; ++i) {
    int y1 = y + gap*i;

    p->setPen(c1);
    p->drawLine(left, y1    , right, y1    );

    p->setPen(c2);
    p->drawLine(left, y1 + 1, right, y1 + 1);
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

int
CQChartsViewExpander::
maxSize() const
{
  if (side_ == Side::LEFT || side_ == Side::RIGHT)
    return std::max(parent_->width() - l_ - r_ - 16, 1);
  else
    return std::max(parent_->height() - b_ - t_ - 16, 1);
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
detachLaterSlot()
{
  detachTimer_->start(10);
}

void
CQChartsViewExpander::
detachSlot()
{
  setDetached(! isDetached());
}
