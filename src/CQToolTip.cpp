#include <CQToolTip.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QCursor>
#include <QKeyEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QTimer>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QBitmap>

#include <cassert>

static CQToolTip *s_instance;

CQToolTip *
CQToolTip::
instance()
{
  if (! s_instance)
    s_instance = new CQToolTip;

  return s_instance;
}

void
CQToolTip::
release()
{
  delete s_instance;

  s_instance = nullptr;
}

// the tooltip widget
CQToolTip::
CQToolTip() :
 QWidget(0, Qt::Window | Qt::FramelessWindowHint),
 margin_ (style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth)),
 opacity_(style()->styleHint  (QStyle::SH_ToolTipLabel_Opacity)/255.0)
{
  setAttribute(Qt::WA_TranslucentBackground);

  //setPalette(QToolTip::palette());
  setWindowOpacity(opacity_);

  setContentsMargins(margin_, margin_, margin_, margin_);

  qApp->installEventFilter(this);
}

CQToolTip::
~CQToolTip()
{
  qApp->removeEventFilter(this);
}

void
CQToolTip::
show(const QPoint &pos, CQToolTipIFace *tooltip, QWidget *parent)
{
  if (isVisible()) return;

  if (! tooltip->canTip(pos))
    return;

  QDesktopWidget *desktop = QApplication::desktop();

  int snum;

  if (desktop->isVirtualDesktop())
    snum = desktop->screenNumber(pos);
  else
    snum = desktop->screenNumber(this);

  setParent(desktop->screen(snum));

  setWindowFlags(Qt::ToolTip);

  QWidget *tipWidget = tooltip->showWidget(pos);

  if (! tipWidget)
    return;

  tooltip_ = tipWidget;
  parent_  = parent;

  int margin = calcMargin();

  setContentsMargins(margin, margin, margin, margin);

  showAtPos(pos);

  updateOpacity(tooltip);

  tooltip_->update();

  startHideTimer();
}

void
CQToolTip::
updateSize()
{
  QSize s = sizeHint();

  resize(s);

  int margin = calcMargin();

  tooltip_->setParent(this);

  tooltip_->move(margin, margin);

  tooltip_->resize(s.width() - 2*margin, s.height() - 2*margin);

  tooltip_->show();
}

QSize
CQToolTip::
sizeHint() const
{
  QSize s      = calcSize();
  int   margin = calcMargin();

  return QSize(s.width() + 2*margin, s.height() + 2*margin);
}

void
CQToolTip::
updateOpacity(CQToolTipIFace *tooltip)
{
  double o = opacity_;

  if (tooltip->opacity() >= 0)
    o = tooltip->opacity();

  setWindowOpacity(o);

  //tooltip_->setWindowOpacity(o);
}

void
CQToolTip::
showAtPos(const QPoint &pos)
{
  QRect drect = desktopRect(pos);

  //---

  QCursor c = parent_->cursor();

  CQToolTipIFace *tooltip = getToolTip(parent_);

  //---

  // cursor size and hotspot
  QSize size(16, 16);

  const QBitmap *bm = c.bitmap();

  if (bm)
    size = bm->size();

  int cw = size.width ();
  int ch = size.height();

  QPoint hs = c.hotSpot();

  //---

  // tip rect size

  QSize ts = sizeHint();

  int tw = ts.width ();
  int th = ts.height();

  //---

  Qt::Alignment align = tooltip->alignment();

  QPoint pos1;

  if (! tooltip->outside()) {
    int dx = -hs.x();
    int dy = -hs.y();

    if      (align & Qt::AlignLeft)
      dx += cw;
    else if (align & Qt::AlignRight)
      dx -= tw;
    else
      dx -= tw/2;

    if      (align & Qt::AlignTop)
      dy += ch;
    else if (align & Qt::AlignBottom)
      dy -= th;
    else
      dy -= th/2;

    int x = pos.x() + dx;
    int y = pos.y() + dy;

    pos1 = QPoint(x, y);
  }
  else {
    int x, y;

    if      (align & Qt::AlignLeft)
      x = 0;
    else if (align & Qt::AlignLeft)
      x = parent_->width() - 1 - tw;
    else
      x = parent_->width()/2 - tw/2;

    if      (align & Qt::AlignTop)
      y = 0;
    else if (align & Qt::AlignBottom)
      y = parent_->height() - 1 - th;
    else
      y = parent_->height()/2 - th/2;

    pos1 = parent_->mapToGlobal(QPoint(x, y));
  }

  //---

  QSize s = this->sizeHint();

  int w = s.width ();
  int h = s.height();

  if (pos1.x() + w > drect.right()) {
    if (w < drect.width())
      pos1.setX(drect.right() - w);
    else
      pos1.setX(drect.left());
  }

  if (pos1.y() + h > drect.bottom()) {
    if (h < drect.height())
      pos1.setY(drect.bottom() - h);
    else
      pos1.setY(drect.top());
  }

  move(pos1);

  updateSize();

  QWidget::show();
}

void
CQToolTip::
enterEvent(QEvent *)
{
  hideLater();
}

void
CQToolTip::
paintEvent(QPaintEvent *)
{
  CQToolTipIFace *tooltip = getToolTip(parent_);

  if (tooltip && tooltip->isTransparent())
    return;

  QStylePainter painter(this);

  QStyleOptionFrame opt;

  opt.initFrom(this);

  painter.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
}

bool
CQToolTip::
eventFilter(QObject *o, QEvent *e)
{
  switch (e->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      int                   key = static_cast<QKeyEvent *>(e)->key();
      Qt::KeyboardModifiers mod = static_cast<QKeyEvent *>(e)->modifiers();

      if ((mod & Qt::KeyboardModifierMask) ||
          (key == Qt::Key_Shift || key == Qt::Key_Control ||
           key == Qt::Key_Alt || key == Qt::Key_Meta))
        break;

      hideLater();

      break;
    }
    case QEvent::Leave:
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    case QEvent::Wheel:
      hideLater();
      break;
    case QEvent::MouseMove: {
      if (isVisible()) {
        QWidget *parent = static_cast<QWidget *>(o);

        CQToolTipIFace *tooltip = getToolTip(parent);

        if (tooltip) {
          QPoint pos = ((QMouseEvent *) e)->globalPos();

          if (! tooltip->updateWidget(pos)) {
            hideLater();

            return false;
          }

          showAtPos(pos);

          updateOpacity(tooltip);

          tooltip_->update();
        }
      }

      startHideTimer();

      break;
    }
    case QEvent::ToolTip: {
      QWidget *parent = static_cast<QWidget *>(o);

      CQToolTipIFace *tooltip = getToolTip(parent);

      if (tooltip && ! isVisible()) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);

        show(helpEvent->globalPos(), tooltip, parent);

        if (tooltip->trackMouse())
          setMouseTracking(true);

        return true;
      }

      break;
    }
    default:
      break;
  }

  return false;
}

void
CQToolTip::
timerEvent(QTimerEvent *event)
{
  if (event->timerId() == hideTimer_)
    hideLater();
}

void
CQToolTip::
hideLater()
{
  if (! isVisible()) return;

  if (hideTimer_) {
    killTimer(hideTimer_);

    hideTimer_ = 0;
  }

  QTimer::singleShot(0, this, SLOT(hideSlot()));
}

void
CQToolTip::
startHideTimer()
{
  if (hideTimer_)
    killTimer(hideTimer_);

  double hideSecs =  hideSecs_;

  CQToolTipIFace *tooltip = getToolTip(parent_);

  if (tooltip && tooltip->hideSecs() > 0)
    hideSecs = tooltip->hideSecs();

  hideTimer_ = startTimer(hideSecs*1000);
}

void
CQToolTip::
hideSlot()
{
  this->hide();

  CQToolTipIFace *tooltip = getToolTip(parent_);

  //if (tooltip && tooltip->trackMouse())
  setMouseTracking(false);

  if (tooltip)
    tooltip->hideWidget();
}

CQToolTipIFace *
CQToolTip::
getToolTip(QWidget *parent)
{
  CQToolTip *inst = CQToolTipInst;

  if (! inst->tooltips_.contains(parent))
    return 0;

  return inst->tooltips_.value(parent);
}

void
CQToolTip::
setToolTip(QWidget *parent, QWidget *tooltip)
{
  setToolTip(parent, new CQToolTipWidgetIFace(tooltip));
}

void
CQToolTip::
setToolTip(QWidget *parent, CQToolTipIFace *tooltip)
{
  assert(parent);

  CQToolTip *inst = CQToolTipInst;

  if (inst->tooltips_.contains(parent)) {
    delete inst->tooltips_.value(parent);

    inst->tooltips_.remove(parent);
  }

  if (tooltip) {
    inst->hide();

    inst->tooltips_[parent] = tooltip;
  }
}

void
CQToolTip::
unsetToolTip(QWidget *parent)
{
  setToolTip(parent, (CQToolTipIFace *) 0);
}

int
CQToolTip::
calcMargin() const
{
  CQToolTipIFace *tooltip = getToolTip(parent_);

  if (tooltip->margin() >= 0)
    return tooltip->margin();
  else
    return margin_;
}

QSize
CQToolTip::
calcSize() const
{
  CQToolTipIFace *tooltip = getToolTip(parent_);

  QSize s = tooltip->sizeHint();

  if (! s.isValid())
    s = tooltip_->sizeHint();

  return s;
}

QRect
CQToolTip::
desktopRect(const QPoint &pos) const
{
  QDesktopWidget *desktop = QApplication::desktop();

  int snum;

  if (desktop->isVirtualDesktop())
    snum = desktop->screenNumber(pos);
  else
    snum = desktop->screenNumber(this);

  return desktop->availableGeometry(snum);
}
