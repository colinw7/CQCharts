#include <CQFloatTip.h>

#include <CQPixmapButton.h>
#include <CQPixmapCache.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QHelpEvent>
#include <QTimerEvent>
#include <QStylePainter>
#include <QStyleOptionFrame>

#include <iostream>

#include <svg/lock_svg.h>

CQFloatTip::
CQFloatTip(QWidget *widget) :
 QFrame(nullptr, Qt::Window | Qt::FramelessWindowHint), widget_(widget)
{
  setObjectName("floatTip");

  setFocusPolicy(Qt::NoFocus);

  setAttribute(Qt::WA_TranslucentBackground);

//setPalette(QToolTip::palette());
//setWindowOpacity(opacity_);

  setContentsMargins(border(), border(), border(), border());

  //---

  auto *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  //---

  QFontMetrics fm(font());

  int is = fm.height();

  //--

  lockButton_ = new CQPixmapButton(CQPixmapCacheInst->getIcon("LOCK"));

  lockButton_->setMargin(2);
  lockButton_->setIconSize(QSize(is, is));
  lockButton_->setFocusPolicy(Qt::NoFocus);

  lockButton_->setCheckable(true);
  lockButton_->setChecked  (false);

  connect(lockButton_, SIGNAL(clicked(bool)), this, SLOT(lockSlot(bool)));

  layout->addWidget(lockButton_);

  //---

  label_ = new QLabel;

  layout->addWidget(label_);

  //---

  //if (widget_)
  //  widget_->setFocusProxy(this);

  qApp->installEventFilter(this);

  connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
          this, SLOT(focusChangedSlot(QWidget *, QWidget *)));
}

CQFloatTip::
~CQFloatTip()
{
}

void
CQFloatTip::
setWidget(QWidget *w)
{
  widget_ = w;

  //if (widget_)
  //  widget_->setFocusProxy(this);
}

void
CQFloatTip::
setText(const QString &text)
{
  text_ = text;

  label_->setText(text_);
}

void
CQFloatTip::
setLocked(bool b)
{
  locked_ = b;

  if (locked_ != lockButton_->isChecked()) {
    disconnect(lockButton_, SIGNAL(clicked(bool)), this, SLOT(lockSlot(bool)));

    lockButton_->setChecked(locked_);

    connect(lockButton_, SIGNAL(clicked(bool)), this, SLOT(lockSlot(bool)));
  }
}

void
CQFloatTip::
setBorder(int i)
{
  border_ = i;

  setContentsMargins(border(), border(), border(), border());

  if (isVisible())
    place();
}

void
CQFloatTip::
setMargin(int i)
{
  margin_ = i;

  if (isVisible())
    place();
}

void
CQFloatTip::
showTip(const QPoint &)
{
  if (! widget_)
    return;

  setParent(widget_);

  resize(sizeHint());

  place();

  this->setMouseTracking(true);

  this->setVisible(true);

  startHideTimer();
}

void
CQFloatTip::
place()
{
  int pw = widget_->width ();
  int ph = widget_->height();

  int w = this->width ();
  int h = this->height();

  int x = x_;
  int y = y_;

  if (x < 0) {
    if      (align_ & Qt::AlignLeft)
      x = margin();
    else if (align_ & Qt::AlignRight)
      x = pw - w - margin() - 1;
    else
      x = (pw - w)/2.0;
  }

  if (y < 0) {
    if      (align_ & Qt::AlignTop)
      y = margin();
    else if (align_ & Qt::AlignBottom)
      y = ph - h - margin() - 1;
    else
      y = (ph - h)/2.0;
  }

  this->move(x, y);
}

void
CQFloatTip::
hideTip()
{
  this->setVisible(false);
}

bool
CQFloatTip::
isIgnoreKey(Qt::Key, Qt::KeyboardModifiers) const
{
  return false;
}

void
CQFloatTip::
lockSlot(bool b)
{
  locked_ = b;

  if (b) {
    if (! this->isVisible())
      this->setVisible(true);
  }
  else {
    if (this->isVisible())
      this->setVisible(false);
  }
}

void
CQFloatTip::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  //---

  // draw background
  QStyleOptionFrame opt;

  opt.initFrom(this);

  painter.drawPrimitive(QStyle::PE_PanelTipLabel, opt);

  //---

  // draw title lines
  bool draggable = (inside_ && isLocked());

  int x1 = width() - border() - 2;
  int x2 = border() + lockButton_->width() + 4;

  int y1 = border();
  int y2 = y1 + lockButton_->height();

  int nl = 3;

  double dy = (y2 - y1)/(nl + 1);

  double y = y1 + dy;

  for (int i = 0; i < nl; ++i) {
    int iy = int(y);

    if (draggable) {
      painter.setPen(QColor(100, 100, 220));

      painter.drawLine(x1, iy, x2, iy);

      painter.setPen(QColor(255, 255, 255));

      ++iy;

      painter.drawLine(x1, iy, x2, iy);
    }
    else {
      painter.setPen(QColor(128, 128, 128));

      painter.drawLine(x1, iy, x2, iy);
    }

    y += dy;
  }

  //---

  // if inside draw and locked inside borders
  if (draggable) {
    int bw = 3;

    painter.setPen(QColor(100, 100, 200));

    auto drawHBorder = [&](int y) {
      for (int i = 0; i < bw; ++i)
        painter.drawLine(0, y + i, width() - i, y + i);
    };

    auto drawVBorder = [&](int x) {
      for (int i = 0; i < bw; ++i)
        painter.drawLine(x + i, 0, x + i, height() - 1);
    };

    //---

    if      (mousePos_.x() < border()) {
      drawVBorder(0);
    }
    else if (mousePos_.x() >= width() - border()) {
      drawVBorder(width() - 1 - bw);
    }
    else if (mousePos_.y() < border()) {
      drawHBorder(0);
    }
    else if (mousePos_.y() >= height() - border()) {
      drawHBorder(height() - 1 - bw);
    }
  }
}

QSize
CQFloatTip::
sizeHint() const
{
  auto s1 = lockButton_->sizeHint();
  auto s2 = label_->sizeHint();

  return QSize(std::max(s1.width(), s2.width()) + 2*border(),
                        s1.height() + s2.height() + 2*border());
}

bool
CQFloatTip::
eventFilter(QObject *o, QEvent *e)
{
  auto *widget = static_cast<QWidget *>(o);

  switch (e->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      auto *ke = static_cast<QKeyEvent *>(e);

      if (widget == widget_) {
        if (! isIgnoreKey((Qt::Key) ke->key(), ke->modifiers()))
          hideLater();
      }

      break;
    }
    case QEvent::Enter: {
      if (widget == this || widget == lockButton_ || widget == label_) {
        startHideTimer();
      }

      break;
    }
    case QEvent::Leave: {
      if (widget == widget_)
        hideLater();

      if      (widget == this) {
        inside_ = false;

        update();
      }
      else if (widget == lockButton_ || widget == label_) {
        startHideTimer();
      }

      break;
    }
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate: {
      break;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel: {
      if (widget == widget_) {
        hideLater();
        break;
      }

      if      (widget == this) {
        startHideTimer();

        if      (e->type() == QEvent::MouseButtonPress) {
          auto *me = static_cast<QMouseEvent *>(e);

          if (isLocked()) {
            Qt::Alignment halign = (align_ & Qt::AlignHorizontal_Mask);
            Qt::Alignment valign = (align_ & Qt::AlignVertical_Mask);

            bool inBorder = false;

            if      (me->x() < border()) {
              halign   = Qt::AlignLeft;
              x_       = -1;
              inBorder = true;
            }
            else if (me->x() >= width() - border()) {
              halign   = Qt::AlignRight;
              x_       = -1;
              inBorder = true;
            }
            else if (me->y() < border()) {
              valign   = Qt::AlignTop;
              y_       = -1;
              inBorder = true;
            }
            else if (me->y() >= height() - border()) {
              valign  = Qt::AlignBottom;
              y_       = -1;
              inBorder = true;
            }
            else if (me->y() >= border() + 2 && me->y() < border() + lockButton_->height() + 2) {
              dragging_   = true;
              dragOffset_ = me->pos();
              dragPos_    = me->globalPos();
            }

            if (inBorder) {
              align_ = halign | valign;

              place();
            }
          }
        }
        else if (e->type() == QEvent::MouseButtonRelease) {
          dragging_ = false;
        }

        return true; // don't propagate
      }
      else if (widget == lockButton_ || widget == label_) {
        startHideTimer();
      }

      break;
    }
    case QEvent::Resize: {
      if (widget == widget_)
        place();

      break;
    }
#if 0
    case QEvent::FocusIn: {
      break;
    }
    case QEvent::FocusOut: {
      if (widget == widget_)
        hideLater();

      if (widget == this || widget == lockButton_ || widget == label_) {
        startHideTimer();
      }

      break;
    }
#endif
    case QEvent::MouseMove: {
      if (widget == widget_)
        break;

      if      (widget == this) {
        startHideTimer();

        auto *me = static_cast<QMouseEvent *>(e);

        if (dragging_) {
          if (! isVisible())
            break;

          auto gpos = me->globalPos();

          auto pos = widget_->mapFromGlobal(gpos);

          pos -= dragOffset_;

          x_ = std::min(std::max(pos.x(), 0), widget_->width () - width ());
          y_ = std::min(std::max(pos.y(), 0), widget_->height() - height());

          place();
        }
        else {
          inside_   = true;
          mousePos_ = me->pos();

          update();
        }

        return true; // don't propagate
      }
      else if (widget == lockButton_ || widget == label_) {
        startHideTimer();
      }

      break;
    }
    case QEvent::ToolTip: {
      if (widget == widget_) {
        auto *he = static_cast<QHelpEvent *>(e);

        showTip(he->globalPos());
      }

      if (widget == this || widget == lockButton_ || widget == label_) {
        startHideTimer();
      }

      break;
    }
    case QEvent::FontChange: {
      if ((widget_ == this || widget == label_) && isVisible()) {
        QTimer::singleShot(50, this, SLOT(fontSlot()));
      }

      break;
    }
    default:
      break;
  }

  return false;
}

void
CQFloatTip::
fontSlot()
{
  QFontMetrics fm(font());

  int is = fm.height();

  lockButton_->setIconSize(QSize(is, is));

  layout()->invalidate();

  resize(sizeHint());

  place();
}

void
CQFloatTip::
focusChangedSlot(QWidget * /*oldW*/, QWidget *newW)
{
//std::cerr << "focusChangedSlot:";
//if (oldW) std::cerr << " Old:'" << oldW->objectName().toStdString() << "'";
//if (newW) std::cerr << " New '" << newW->objectName().toStdString() << "'";
//std::cerr << "\n";

  if (newW && newW != widget_)
    hideLater();
}

void
CQFloatTip::
startHideTimer()
{
  stopTimer();

  double hideSecs = 3.0;

  hideTimer_ = startTimer(hideSecs*1000);
}

void
CQFloatTip::
stopTimer()
{
  if (hideTimer_) {
    killTimer(hideTimer_);

    hideTimer_ = 0;
  }
}

void
CQFloatTip::
timerEvent(QTimerEvent *event)
{
  if (event->timerId() == hideTimer_)
    hideLater();
}

void
CQFloatTip::
hideLater()
{
  if (! isVisible())
    return;

  QTimer::singleShot(50, this, SLOT(hideSlot()));
}

void
CQFloatTip::
hideSlot()
{
  if (isLocked())
    return;

  this->hideTip();

  this->setMouseTracking(false);

  stopTimer();
}