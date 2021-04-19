#include <CQFloatTip.h>

#include <CQIconButton.h>
//#include <CEnv.h>

#include <QApplication>
#include <QScrollArea>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QHelpEvent>
#include <QTimerEvent>
#include <QStylePainter>
#include <QStyleOptionFrame>

#include <iostream>

#include <svg/lock_svg.h>
#include <svg/query_svg.h>

CQFloatTip::
CQFloatTip(QWidget *widget) :
 QFrame(nullptr, Qt::Window | Qt::FramelessWindowHint), widget_(widget)
{
  init();
}

void
CQFloatTip::
init()
{
  setObjectName("floatTip");

  setFocusPolicy(Qt::NoFocus);

  setAttribute(Qt::WA_TranslucentBackground);

//setPalette(QToolTip::palette());
//setWindowOpacity(opacity_);

  setContentsMargins(border(), border(), border(), border());

  //---

#if 0
  if      (CEnvInst->getBool("GUI_TITLE_BAR_NONE"))
    barStyle_ = BAR_NONE;
  else if (CEnvInst->getBool("GUI_TITLE_BAR_GRADIENT"))
    barStyle_ = BAR_GRADIENT;
  else if (CEnvInst->getBool("GUI_TITLE_BAR_LINES"))
    barStyle_ = BAR_LINES;
#endif

  //---

  auto *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  //---

  //QFontMetrics fm(font());
  //int is = fm.height() - 2;

  //---

  auto *headerLayout = new QHBoxLayout();
  headerLayout->setMargin(0); headerLayout->setSpacing(0);

  layout->addLayout(headerLayout);

  //---

  lockButton_ = new CQIconButton; lockButton_->setIcon("LOCK");

  lockButton_->setObjectName("lock");
  //lockButton_->setMargin(2);
  //lockButton_->setIconSize(QSize(is, is));
  lockButton_->setSize(CQIconButton::Size::SMALL);
  lockButton_->setFocusPolicy(Qt::NoFocus);

  lockButton_->setCheckable(true);
  lockButton_->setChecked  (false);

  connect(lockButton_, SIGNAL(clicked(bool)), this, SLOT(lockSlot(bool)));

  headerLayout->addWidget(lockButton_);

  //---

  queryButton_ = new CQIconButton; queryButton_->setIcon("QUERY");

  queryButton_->setObjectName("query");
  //queryButton_->setMargin(2);
  //queryButton_->setIconSize(QSize(is, is));
  queryButton_->setSize(CQIconButton::Size::SMALL);
  queryButton_->setFocusPolicy(Qt::NoFocus);

  connect(queryButton_, SIGNAL(clicked(bool)), this, SLOT(querySlot()));

  headerLayout->addWidget(queryButton_);

  headerLayout->addStretch(1);

  //---

  scroll_ = new QScrollArea;

  scroll_->setObjectName("scroll");
  scroll_->setBackgroundRole(QPalette::ToolTipBase);

  layout->addWidget(scroll_);

  //---

  label_ = new QLabel;

  label_->setObjectName("label");

  scroll_->setWidget(label_);

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

  //---

  updateWidgetPalette();

  if (isVisible())
    resizeFit();
}

void
CQFloatTip::
setEnabled(bool b)
{
  enabled_ = b;

  if (! b)
    setLocked(false);
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
setBarStyle(const BarStyle &s)
{
  barStyle_ = s;

  update();
}

void
CQFloatTip::
showTip(const QPoint &gpos)
{
  if (! widget_ || ! isEnabled())
    return;

  gpos_ = gpos;

  setParent(widget_);

  resizeFit();

  place();

  this->setMouseTracking(true);

  this->setVisible(true);

  startHideTimer();
}

void
CQFloatTip::
showQuery(const QPoint &)
{
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
      x = (pw - w)/2;
  }

  if (y < 0) {
    if      (align_ & Qt::AlignTop)
      y = margin();
    else if (align_ & Qt::AlignBottom)
      y = ph - h - margin() - 1;
    else
      y = (ph - h)/2;
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
    if (! isEnabled())
      return;

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
querySlot()
{
  showQuery(gpos_);
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

  // draw title bar
  drawTitleBar(&painter);

  //---

  // if inside draw and locked inside borders
  bool draggable = (inside_ && isLocked());

  if (draggable) {
    int bw = 3;

    painter.setPen(QColor(100, 100, 220));

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

void
CQFloatTip::
drawTitleBar(QPainter *painter)
{
  bool draggable = (inside_ && isLocked());

  int x1 = border() + lockButton_->width() + queryButton_->width() + 6;
  int x2 = width() - border() - 2;

  int y1 = border();
  int y2 = y1 + lockButton_->height();

#if 0
  int nl = std::max(titleLines(), 2);

  double dy = 1.0*(y2 - y1)/(nl + 1);

  double y = y1 + dy;

  for (int i = 0; i < nl; ++i) {
    int iy = int(y);

    if (draggable) {
      painter->setPen(QColor(100, 100, 220));

      painter->drawLine(x1, iy, x2, iy);

      painter->setPen(QColor(255, 255, 255));

      ++iy;

      painter->drawLine(x1, iy, x2, iy);
    }
    else {
      painter->setPen(QColor(128, 128, 128));

      painter->drawLine(x1, iy, x2, iy);
    }

    y += dy;
  }
#else
  auto mergedColors = [&](const QColor &colorA, const QColor &colorB, double factor = 50.0) {
    const double maxFactor = 100.0;

    QColor tmp = colorA;

    tmp.setRed  ((tmp.red  ()*factor)/maxFactor + (colorB.red  ()*(maxFactor - factor))/maxFactor);
    tmp.setGreen((tmp.green()*factor)/maxFactor + (colorB.green()*(maxFactor - factor))/maxFactor);
    tmp.setBlue ((tmp.blue ()*factor)/maxFactor + (colorB.blue ()*(maxFactor - factor))/maxFactor);

    return tmp;
  };

  QRect rect(x1, y1, x2 - x1, y2 - y1);

  QColor barColor = (draggable ? QColor(100, 100, 220) : QColor(180, 180, 180));
  QColor bgColor  = palette().color(QPalette::ToolTipBase);

  QColor gradientStartColor = mergedColors(barColor, bgColor, 40);
  QColor gradientStopColor  = mergedColors(barColor, bgColor, 10);

  QLinearGradient gradient(rect.left(), rect.top(), rect.right(), rect.bottom());

  gradient.setColorAt(0, gradientStartColor);
  gradient.setColorAt(1, gradientStopColor);

  painter->fillRect(rect, gradient);
#endif
}

QSize
CQFloatTip::
sizeHint() const
{
  auto s1 = lockButton_->sizeHint();
  auto s2 = label_->sizeHint();

  return QSize(std::max(2*s1.width(), s2.width()) + 2*border(),
                        s1.height() + s2.height() + 2*border());
}

bool
CQFloatTip::
eventFilter(QObject *o, QEvent *e)
{
  auto *widget = static_cast<QWidget *>(o);

  auto isChildWidget = [&]() {
    return (widget == lockButton_ || widget == queryButton_ || widget == label_);
  };

  //---

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
      if (widget == this || isChildWidget()) {
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
      else if (isChildWidget()) {
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
      else if (isChildWidget()) {
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

      if (widget == this || isChildWidget()) {
        startHideTimer();
      }

      break;
    }
#endif
    case QEvent::MouseMove: {
      auto *me = static_cast<QMouseEvent *>(e);

      auto gpos = me->globalPos();

      if (widget == widget_) {
        if (isVisible() && isLocked() && isFollowMouse()) {
          showTip(gpos);
        }

        break;
      }

      if      (widget == this) {
        startHideTimer();

        if (dragging_) {
          if (! isVisible())
            break;

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
      else if (isChildWidget()) {
        startHideTimer();
      }

      break;
    }
    case QEvent::ToolTip: {
      if (widget == widget_) {
        auto *he = static_cast<QHelpEvent *>(e);

        showTip(he->globalPos());
      }

      if (widget == this || isChildWidget()) {
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
  //QFontMetrics fm(font());
  //int is = fm.height() - 2;

  //lockButton_ ->setIconSize(QSize(is, is));
  //queryButton_->setIconSize(QSize(is, is));

  layout()->invalidate();

  resizeFit();

  place();
}

void
CQFloatTip::
resizeFit()
{
  auto ls = label_->sizeHint();

  auto s1 = sizeHint();
  auto s2 = widget_->size();

  int dx = 4, dy = 4;

  // if label bigger than screen need space for scroll bars
  if (s1.width() > s2.width() || s1.height() > s2.height()) {
    dx = 20;
    dy = 20;
  }

  label_->resize(ls);

  int w = std::min(s1.width () + dx, s2.width ());
  int h = std::min(s1.height() + dy, s2.height());

  resize(QSize(w, h));
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

  hideTimer_ = startTimer(int(hideSecs*1000));
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

void
CQFloatTip::
updateWidgetPalette()
{
  setWidgetPalette(label_      );
  setWidgetPalette(lockButton_ );
  setWidgetPalette(queryButton_);
}

void
CQFloatTip::
setWidgetPalette(QWidget *w)
{
  auto palette = this->palette();

  palette.setColor(QPalette::Window    , palette.color(QPalette::ToolTipBase));
  palette.setColor(QPalette::WindowText, palette.color(QPalette::ToolTipText));
  palette.setColor(QPalette::Base      , palette.color(QPalette::ToolTipBase));
  palette.setColor(QPalette::Text      , palette.color(QPalette::ToolTipText));
  palette.setColor(QPalette::Button    , palette.color(QPalette::ToolTipBase));
  palette.setColor(QPalette::ButtonText, palette.color(QPalette::ToolTipText));

  w->setPalette(palette);
}
