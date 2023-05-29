#include <CQTabBar.h>

#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionTab>
#include <QMouseEvent>
#include <QToolTip>
#include <QDrag>
#include <QMimeData>

#include <cassert>

namespace {
static const char *dragId     = "CQTabBarDragId";
static const char *mimeId     = "CQTabBarMimeId";
static const char *mimeNameId = "CQTabBarMimeNameId";
static const char *mimeTabId  = "CQTabBarMimeTabId";
}

// create tab bar
CQTabBar::
CQTabBar(QWidget *parent) :
 QWidget(parent)
{
  setObjectName("tabBar");

  setAcceptDrops(true);

  // add scroll buttons if tab bar is clipped
  lscroll_ = new CQTabBarScrollButton(this, "lscroll");
  rscroll_ = new CQTabBarScrollButton(this, "rscroll");

  connect(lscroll_, SIGNAL(clicked()), this, SLOT(lscrollSlot()));
  connect(rscroll_, SIGNAL(clicked()), this, SLOT(rscrollSlot()));

  lscroll_->hide();
  rscroll_->hide();

  //---

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

// delete tab bar
CQTabBar::
~CQTabBar()
{
  clear();
}

// clear tab bar
void
CQTabBar::
clear()
{
  for (auto *button : buttons_)
    delete button;

  buttons_.clear();

  currentIndex_ = -1;
}

// add tab for widget with specified text
int
CQTabBar::
addTab(const QString &text, QWidget *w)
{
  return addTab(QIcon(), text, w);
}

// add tab for widget with specified text
int
CQTabBar::
addTab(const QIcon &icon, const QString &text, QWidget *w)
{
  // create button
  auto *button = new CQTabBarButton(this);

  button->setText  (text);
  button->setIcon  (icon);
  button->setWidget(w);

  return addTab(button);
}

// add tab for button
int
CQTabBar::
addTab(CQTabBarButton *button)
{
  int ind = count();

  return insertTab(ind, button);
}

// insert tab for widget with specified text
int
CQTabBar::
insertTab(int ind, const QString &text, QWidget *w)
{
  return insertTab(ind, QIcon(), text, w);
}

// insert tab for widget with specified text
int
CQTabBar::
insertTab(int ind, const QIcon &icon, const QString &text, QWidget *w)
{
  // create button
  auto *button = new CQTabBarButton(this);

  button->setText  (text);
  button->setIcon  (icon);
  button->setWidget(w);

  return insertTab(ind, button);
}

int
CQTabBar::
insertTab(int ind, CQTabBarButton *button)
{
  buttons_.push_back(nullptr);

  for (int i = int(buttons_.size() - 1); i > ind; --i)
    buttons_[size_t(i)] = buttons_[size_t(i - 1)];

  buttons_[size_t(ind)] = button;

  int index = int(buttons_.size()) - 1;

  button->setIndex(index);

  // update current
  if (! allowNoTab() && currentIndex() < 0)
    setCurrentIndex(index);

  // update display
  updateSizes();

  update();

  return index;
}

// remove tab for widget
void
CQTabBar::
removeTab(QWidget *widget)
{
  // get tab index
  int ind = getTabIndex(widget);
  assert(ind >= 0);

  removeTab(ind);
}

// remove tab for index
void
CQTabBar::
removeTab(int ind)
{
  int pos = tabButtonPos(ind);

  auto *button = buttons_[size_t(pos)];

  buttons_[size_t(pos)] = nullptr;

  delete button;

  // reset current if deleted is current
  if (currentIndex() == ind)
    setCurrentIndex(-1);

  // update display
  updateSizes();

  update();
}

// get number of tabs
int
CQTabBar::
count() const
{
  int n = 0;

  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;
    if (! button) continue;

    ++n;
  }

  return n;
}

// get tab at count
int
CQTabBar::
tabInd(int i) const
{
  int n = 0;

  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;
    if (! button) continue;

    if (n == i)
      return button->index();

    ++n;
  }

  return -1;
}

// set current tab
void
CQTabBar::
setCurrentIndex(int ind)
{
  // button array can contain empty slots so ensure the requested one is valid
  auto *button = tabButton(ind);
  if (! button) return;

  // process if changed
  if (ind != currentIndex_) {
    currentIndex_ = ind;

    // if one tab must be active and nothing active then use first non-null button
    if (! allowNoTab() && currentIndex_ < 0 && count() > 0)
      currentIndex_ = 0;

    update();

    Q_EMIT currentChanged(currentIndex_);
  }
}

// get tab index for specified widget
int
CQTabBar::
getTabIndex(QWidget *w) const
{
  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;

    if (button && button->widget() == w)
      return button->index();
  }

  return -1;
}

// set tab location (relative to contents)
void
CQTabBar::
setPosition(const Position &position)
{
  // assert position is valid (just in case)
  assert(position == Position::North || position == Position::South ||
         position == Position::West || position == Position::East);

  position_ = position;

  update();
}

// set allow no current tab
void
CQTabBar::
setAllowNoTab(bool allow)
{
  allowNoTab_ = allow;

  if (! allowNoTab() && currentIndex() < 0 && count() > 0)
    setCurrentIndex(0);
}

// set tab button style
void
CQTabBar::
setButtonStyle(const Qt::ToolButtonStyle &buttonStyle)
{
  buttonStyle_ = buttonStyle;

  updateSizes();

  update();
}

// get tab text
QString
CQTabBar::
tabText(int ind) const
{
  auto *button = tabButton(ind);

  return (button ? button->text() : "");
}

// set tab text
void
CQTabBar::
setTabText(int ind, const QString &text)
{
  auto *button = tabButton(ind);

  if (button)
    button->setText(text);

  updateSizes();

  update();
}

QIcon
CQTabBar::
tabIcon(int ind) const
{
  auto *button = tabButton(ind);

  return (button ? button->icon() : QIcon());
}

// set tab icon
void
CQTabBar::
setTabIcon(int ind, const QIcon &icon)
{
  auto *button = tabButton(ind);

  if (button)
    button->setIcon(icon);

  updateSizes();

  update();
}

// get tab tooltip
QString
CQTabBar::
tabToolTip(int index) const
{
  auto *button = tabButton(index);

  return (button ? button->toolTip() : "");
}

// set tab tooltip
void
CQTabBar::
setTabToolTip(int ind, const QString &tip)
{
  auto *button = tabButton(ind);

  if (button)
    button->setToolTip(tip);
}

// get tab visible
bool
CQTabBar::
isTabVisible(int index) const
{
  auto *button = tabButton(index);

  return (button ? button->visible() : false);
}

// set tab visible
void
CQTabBar::
setTabVisible(int ind, bool visible)
{
  auto *button = tabButton(ind);

  if (button)
    button->setVisible(visible);

  updateSizes();

  update();
}

// get tab pending
bool
CQTabBar::
isTabPending(int index) const
{
  auto *button = tabButton(index);

  return (button ? button->pending() : false);
}

// set tab pending state
void
CQTabBar::
setTabPending(int ind, bool pending)
{
  auto *button = tabButton(ind);

  if (button)
    button->setPending(pending);

  update();
}

// get tab data
QVariant
CQTabBar::
tabData(int ind) const
{
  auto *button = tabButton(ind);

  if (button)
    return button->data();
  else
    return QVariant();
}

// set tab data
void
CQTabBar::
setTabData(int ind, const QVariant &data)
{
  auto *button = tabButton(ind);

  if (button)
    button->setData(data);
}

// get button for tab
CQTabBarButton *
CQTabBar::
tabButton(int ind) const
{
  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;

    if (button && button->index() == ind)
      return button;
  }

  return nullptr;
}

// get array pos for tab
int
CQTabBar::
tabButtonPos(int ind) const
{
  for (int i = 0; i < int(buttons_.size()); ++i) {
    auto *button = buttons_[size_t(i)];

    if (button && button->index() == ind)
      return i;
  }

  assert(false);

  return -1;
}

// get widget for tab
QWidget *
CQTabBar::
tabWidget(int ind) const
{
  auto *button = tabButton(ind);

  if (button)
    return button->widget();

  return nullptr;
}

// draw tab buttons
void
CQTabBar::
paintEvent(QPaintEvent *)
{
  QStylePainter stylePainter(this);

  //------

  // calculate width and height of region
  int xo = 0;

  if (! isFlowTabs() && offset_ > 0) {
    int offset = offset_;

    for (auto p = buttons_.begin(); offset > 0 && p != buttons_.end(); ++p) {
      auto *button = *p;

      if (! button || ! button->visible()) continue;

      xo += button->width();

      --offset;
    }
  }

  int w = width ();
  int h = height();

  // set tab style
#if QT_VERSION < 0x050000
  QStyleOptionTabV2 tabStyle;
#else
  QStyleOptionTab tabStyle;
#endif

  tabStyle.initFrom(this);

  tabStyle.shape = getTabShape();

  int overlap = style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &tabStyle, this);

  // set tab base style
#if QT_VERSION < 0x050000
  QStyleOptionTabBarBaseV2 baseStyle;
#else
  QStyleOptionTabBarBase baseStyle;
#endif

  baseStyle.initFrom(this);

  // calculate button geometry and first/last tab buttons
  CQTabBarButton *firstButton = nullptr;
  CQTabBarButton *lastButton  = nullptr;

  int x = -xo;
  int y = 0;

  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;

    if (! button || ! button->visible()) continue;

    // calc button width
    int w1 = button->width();

    // flow to next row if needed
    if (isFlowTabs() && firstButton) {
      if (isVertical()) {
        if (x + w1 > h) {
          x  = 0;
          y += button->height();
        }
      }
      else {
        if (x + w1 > w) {
          x  = 0;
          y += button->height();
        }
      }
    }

    if (firstButton == nullptr)
      firstButton = button;
    else
      lastButton = button;

    // calculate and store button rectangle
    QRect r;

    if (isVertical())
      r = QRect(y, x, h_, w1);
    else
      r = QRect(x, y, w1, h_);

    button->setRect(r);

    // update base line rectangle
    if (button->index() == currentIndex())
      baseStyle.selectedTabRect = r;

    //-----

    x += w1;
  }

  if (firstButton)
    y += firstButton->height();

  buttonsHeight_ = y;

  //---

  // draw tab base
  if      (position_ == Position::North)
    baseStyle.rect = QRect(0, buttonsHeight_ - overlap, w, overlap);
  else if (position_ == Position::South)
    baseStyle.rect = QRect(0, 0, w, overlap);
  else if (position_ == Position::West)
    baseStyle.rect = QRect(buttonsHeight_ - overlap, 0, overlap, h);
  else if (position_ == Position::East)
    baseStyle.rect = QRect(0, 0, overlap, h);

  baseStyle.shape = getTabShape();

  stylePainter.drawPrimitive(QStyle::PE_FrameTabBarBase, baseStyle);

  //------

  // draw buttons
  x = -xo;

  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;

    if (! button || ! button->visible()) continue;

    //----

    // set button style
    tabStyle.initFrom(this);

    tabStyle.state &= ~(QStyle::State_HasFocus | QStyle::State_MouseOver);

    tabStyle.rect = button->rect();

    tabStyle.row = 0;

    if (button->index() == pressIndex_)
      tabStyle.state |= QStyle::State_Sunken;
    else
      tabStyle.state &= ~QStyle::State_Sunken;

    if (button->index() == currentIndex())
      tabStyle.state |=  QStyle::State_Selected;
    else
      tabStyle.state &= ~QStyle::State_Selected;

    if (button->index() == moveIndex_)
      tabStyle.state |=  QStyle::State_MouseOver;
    else
      tabStyle.state &= ~QStyle::State_MouseOver;

    tabStyle.shape = getTabShape();

    if (buttonStyle_ == Qt::ToolButtonTextOnly || buttonStyle_ == Qt::ToolButtonTextBesideIcon)
      tabStyle.text = button->text();

    if (buttonStyle_ == Qt::ToolButtonIconOnly || buttonStyle_ == Qt::ToolButtonTextBesideIcon)
      tabStyle.icon = button->positionIcon(position_);

    tabStyle.iconSize = iconSize();

    if      (button == firstButton)
      tabStyle.position = QStyleOptionTab::Beginning;
    else if (button == lastButton)
      tabStyle.position = QStyleOptionTab::End;
    else
      tabStyle.position = QStyleOptionTab::Middle;

    if (button->index() == currentIndex())
      tabStyle.palette.setColor(QPalette::WindowText, palette().highlight().color());

    if (button->pending())
      tabStyle.palette.setColor(QPalette::Button, pendingColor());

    // draw button
    stylePainter.drawControl(QStyle::CE_TabBarTab, tabStyle);

    //-----

    x += button->width();
  }

  // update scroll buttons
  if (! isFlowTabs()) {
    lscroll_->setEnabled(offset_ > 0);
    rscroll_->setEnabled(offset_ < clipNum_);
  }
}

// handle resize
void
CQTabBar::
resizeEvent(QResizeEvent *)
{
  updateSizes();
}

// update size of tab area
void
CQTabBar::
updateSizes()
{
  // calculate width and height of region
  QFontMetrics fm(font());

  int iw = iconSize().width();
  int bh = qMax(iw, fm.height()) + TAB_BORDER;

  // remove resize width
  if (isVertical())
    bh = qMin(bh, width () - RESIZE_WIDTH);
  else
    bh = qMin(bh, height() - RESIZE_WIDTH);

  int x  = 0;
  int y  = 0;
  int bw = 0;

  int w = width ();
  int h = height();

  clipNum_ = 0;

  CQTabBarButton *firstButton = nullptr;

  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;

    if (! button || ! button->visible()) continue;

    if (! firstButton)
      firstButton = button;

    bw = x + button->width();

    if (isVertical()) {
      if (bw > h) {
        if (isFlowTabs()) {
          if (firstButton) {
            x  = 0;
            y += button->height();
          }
        }
        else {
          ++clipNum_;
        }
      }
    }
    else {
      if (bw > w) {
        if (isFlowTabs()) {
          if (firstButton) {
            x  = 0;
            y += button->height();
          }
        }
        else {
          ++clipNum_;
        }
      }
    }

    x += button->width();
  }

  if (firstButton)
    y += firstButton->height();

  iw_ = iw;
  w_  = bw;
  h_  = bh;

  buttonsHeight_ = y;

  //-----

  // update scroll if clipped
  if (! isFlowTabs())
    showScrollButtons(clipNum_ > 0);

  if (offset_ > clipNum_)
    offset_ = clipNum_;
}

// update scroll buttons
void
CQTabBar::
showScrollButtons(bool show)
{
  lscroll_->setVisible(show);
  rscroll_->setVisible(show);

  if (show) {
    // position scroll buttons depending in tab position
    if (isVertical()) {
      int xs = iconWidth() + 6;
      int ys = iconWidth();

      int d = h_ - xs;

      lscroll_->setFixedSize(xs, ys);
      rscroll_->setFixedSize(xs, ys);

      lscroll_->move(d, height() - 2*ys);
      rscroll_->move(d, height() -   ys);

      lscroll_->setArrowType(Qt::UpArrow);
      rscroll_->setArrowType(Qt::DownArrow);
    }
    else {
      int xs = iconWidth();
      int ys = iconWidth() + 6;

      int d = h_ - ys;

      lscroll_->setFixedSize(xs, ys);
      rscroll_->setFixedSize(xs, ys);

      lscroll_->move(width() - 2*xs, d);
      rscroll_->move(width() -   xs, d);

      lscroll_->setArrowType(Qt::LeftArrow);
      rscroll_->setArrowType(Qt::RightArrow);
    }
  }
  else
    offset_ = 0;
}

// called when left/bottom scroll is pressed
void
CQTabBar::
lscrollSlot()
{
  // scroll to previous tab (if any)
  --offset_;

  if (offset_ < 0)
    offset_ = 0;

  update();
}

// called when right/top scroll is pressed
void
CQTabBar::
rscrollSlot()
{
  // scroll to next tab (if any)
  ++offset_;

  if (offset_ > clipNum_)
    offset_ = clipNum_;

  update();
}

// handle tool tip event
bool
CQTabBar::
event(QEvent *e)
{
  if (e->type() == QEvent::ToolTip) {
    auto *helpEvent = static_cast<QHelpEvent *>(e);

    int ind = tabAt(helpEvent->pos());

    if (ind != -1) {
      auto *button = tabButton(ind);

      if (button)
        QToolTip::showText(helpEvent->globalPos(), button->toolTip());
    }
    else {
      QToolTip::hideText();

      e->ignore();
    }

    return true;
  }

  return QWidget::event(e);
}

// get preferred size
QSize
CQTabBar::
sizeHint() const
{
  QFontMetrics fm(font());

  int iw = iconSize().width();
  int h  = qMax(iw, fm.height()) + TAB_BORDER + RESIZE_WIDTH;

  int w = 0;

  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;

    if (! button || ! button->visible()) continue;

    w += button->width();
  }

  if (isVertical())
    return QSize(h, w);
  else
    return QSize(w, h);
}

// get minimum size
QSize
CQTabBar::
minimumSizeHint() const
{
  auto s = sizeHint();

  if (isVertical())
    return QSize(s.width(), 0);
  else
    return QSize(0, s.height());
}

// set press point
void
CQTabBar::
setPressPoint(const QPoint &p)
{
  pressed_    = true;
  pressPos_   = p;
  pressIndex_ = tabAt(pressPos_);
}

// handle mouse press
void
CQTabBar::
mousePressEvent(QMouseEvent *e)
{
  // init press state and redraw
  setPressPoint(e->pos());

  update();
}

// handle mouse move (while pressed)
void
CQTabBar::
mouseMoveEvent(QMouseEvent *e)
{
  // update press state and redraw
  if (! pressed_)
    setPressPoint(e->pos());

  // If left button pressed check for drag
  if (e->buttons() & Qt::LeftButton) {
    // check drag distance
    if (pressIndex_ >= 0 &&
        (e->pos() - pressPos_).manhattanLength() >= QApplication::startDragDistance()) {
      auto *button = tabButton(pressIndex_);

      auto icon = (button ? button->icon() : QIcon());

      // initiate drag
      auto *drag = new QDrag(this);

      drag->setPixmap(icon.pixmap(iconSize()));

      auto *mimeData = new QMimeData;

      // use unique id for our mime data and store source palette type
      auto numStr = QString("%1").arg(pressIndex_);

      mimeData->setData(mimeId    , dragId);
      mimeData->setData(mimeNameId, objectName().toLatin1());
      mimeData->setData(mimeTabId , numStr.toLatin1());

      drag->setMimeData(mimeData);

      drag->exec();
    }
  }

  update();
}

// handle mouse release
void
CQTabBar::
mouseReleaseEvent(QMouseEvent *e)
{
  // reset pressed state
  pressed_ = false;

  // check if new tab button is pressed
  pressIndex_ = tabAt(e->pos());

  bool isCurrent = (pressIndex_ != -1 && pressIndex_ == currentIndex());

  if (pressIndex_ != -1) {
    if (! isCurrent)
      setCurrentIndex(pressIndex_); // will send currentChanged signal
    else
      Q_EMIT currentPressed(pressIndex_);
  }

  // signal tab button pressed
  Q_EMIT tabPressedSignal(pressIndex_, ! isCurrent);

  // redraw
  update();
}

// handle drag enter event
void
CQTabBar::
dragEnterEvent(QDragEnterEvent *event)
{
  QString name;
  int     fromIndex;

  if (! dragValid(event->mimeData(), name, fromIndex)) {
    event->ignore();
    return;
  }

  event->acceptProposedAction();
}

// handle drag move
void
CQTabBar::
dragMoveEvent(QDragMoveEvent *event)
{
  QString name;
  int     fromIndex;

  if (! dragValid(event->mimeData(), name, fromIndex)) {
    event->ignore();
    return;
  }

  if (dragPosValid(name, event->pos()))
    event->acceptProposedAction();
  else
    event->ignore();
}

// handle drop event
void
CQTabBar::
dropEvent(QDropEvent *event)
{
  QString name;
  int     fromIndex;

  if (! dragValid(event->mimeData(), name, fromIndex)) {
    event->ignore();
    return;
  }

  if (! dragPosValid(name, event->pos()))
    return;

  // get index at release position
  int toIndex = tabAt(event->pos());

  if (name == objectName()) {
    // skip invalid and do nothing drops
    if (fromIndex < 0 || toIndex < 0 || fromIndex == toIndex)
      return;

    int pos1 = tabButtonPos(fromIndex);
    int pos2 = tabButtonPos(toIndex  );

    auto *button1 = buttons_[size_t(pos1)];
    auto *button2 = buttons_[size_t(pos2)];

    buttons_[size_t(pos2)] = button1;
    buttons_[size_t(pos1)] = button2;

    button1->setIndex(toIndex);
    button2->setIndex(fromIndex);

    if      (fromIndex == currentIndex()) setCurrentIndex(toIndex);
    else if (toIndex   == currentIndex()) setCurrentIndex(fromIndex);

    Q_EMIT tabMoved(fromIndex, toIndex);

    event->acceptProposedAction();
  }
  else {
    Q_EMIT tabMovePageSignal(name, fromIndex, this->objectName(), toIndex);

    event->acceptProposedAction();
  }
}

// is drag valid
bool
CQTabBar::
dragValid(const QMimeData *m, QString &name, int &num) const
{
  // Only accept if it's our request
  auto formats = m->formats();

  if (! formats.contains(mimeId) || m->data(mimeId) != dragId)
    return false;

  assert(formats.contains(mimeTabId));
  assert(formats.contains(mimeNameId));

  name = m->data(mimeNameId).data();

  auto *numStr = m->data(mimeTabId).data();

  bool ok;
  num = QString(numStr).toInt(&ok);
  if (! ok) return false;

  return true;
}

// is drag position valid
bool
CQTabBar::
dragPosValid(const QString &name, const QPoint &pos) const
{
  // drag in same tab bar must be to another tab
  if (name == this->objectName()) {
    int ind = tabAt(pos);

    if (ind < 0)
      return false;
  }

  return true;
}

// get tab at specified point
int
CQTabBar::
tabAt(const QPoint &point) const
{
  for (auto p = buttons_.begin(); p != buttons_.end(); ++p) {
    auto *button = *p;

    if (! button || ! button->visible()) continue;

    if (button->rect().contains(point))
      return button->index();
  }

  return -1;
}

// get icon size
QSize
CQTabBar::
iconSize() const
{
  return iconSize_;
}

// set icon size
void
CQTabBar::
setIconSize(const QSize &size)
{
  iconSize_ = size;

  update();
}

// handle context menu request
void
CQTabBar::
contextMenuEvent(QContextMenuEvent *e)
{
  Q_EMIT showContextMenuSignal(e->globalPos());
}

//! get tab shape for position
QTabBar::Shape
CQTabBar::
getTabShape() const
{
  switch (position_) {
    case Position::North: return QTabBar::RoundedNorth;
    case Position::South: return QTabBar::RoundedSouth;
    case Position::West : return QTabBar::RoundedWest;
    case Position::East : return QTabBar::RoundedEast;
    default             : assert(false); return QTabBar::RoundedNorth;
  }
}

bool
CQTabBar::
isVertical() const
{
  return (position_ == Position::West || position_ == Position::East);
}

//-------

// create tab button
CQTabBarButton::
CQTabBarButton(CQTabBar *bar) :
 bar_(bar)
{
}

// set button text
void
CQTabBarButton::
setText(const QString &text)
{
  text_ = text;
}

// set button icon
void
CQTabBarButton::
setIcon(const QIcon &icon)
{
  icon_ = icon;

  // ensure new icon causes recalc
  if (iconPosition_ != Position::North && iconPosition_ != Position::South)
    iconPosition_ = Position::North;
}

// set button data
void
CQTabBarButton::
setData(const QVariant &data)
{
  data_ = data;
}

// get icon for tab position
const QIcon &
CQTabBarButton::
positionIcon(CQTabBar::Position pos) const
{
  if (pos == Position::North || pos == Position::South)
    return icon_;

  if (pos == iconPosition_)
    return positionIcon_;

  iconPosition_ = pos;

  QTransform t;

  auto p = pixmap();

  t.rotate(iconPosition_ == Position::West ? 90 : -90);

  positionIcon_ = QIcon(p.transformed(t));

  return positionIcon_;
}

// get tab tooltip
const QString &
CQTabBarButton::
toolTip() const
{
  if (toolTip_ != "")
    return toolTip_;
  else
    return text_;
}

// set tab tooltip
void
CQTabBarButton::
setToolTip(const QString &tip)
{
  toolTip_ = tip;
}

// set tab widget
void
CQTabBarButton::
setWidget(QWidget *w)
{
  w_ = w;
}

// set tab visible
void
CQTabBarButton::
setVisible(bool visible)
{
  visible_ = visible;
}

// set tab pending
void
CQTabBarButton::
setPending(bool pending)
{
  pending_ = pending;
}

// set tab rectangle
void
CQTabBarButton::
setRect(const QRect &r)
{
  r_ = r;
}

// get tab icon pixmap
QPixmap
CQTabBarButton::
pixmap() const
{
  return icon_.pixmap(bar_->iconSize());
}

// get button width depending on button style
int
CQTabBarButton::
width() const
{
  QFontMetrics fm(bar_->font());

  //------

  auto buttonStyle = bar_->buttonStyle();

  int w = 0;

  if      (buttonStyle == Qt::ToolButtonTextOnly)
    w = fm.horizontalAdvance(text()) + 24;
  else if (buttonStyle == Qt::ToolButtonIconOnly)
    w = bar_->iconWidth() + 24;
  else
    w = bar_->iconWidth() + fm.horizontalAdvance(text()) + 32;

  return w;
}

int
CQTabBarButton::
height() const
{
  QFontMetrics fm(bar_->font());

  return fm.height() + TAB_BORDER;
}

//---------

CQTabBarScrollButton::
CQTabBarScrollButton(CQTabBar *bar, const char *name) :
 QToolButton(bar)
{
  setObjectName(name);

  setAutoRepeat(true);

  setFocusPolicy(Qt::NoFocus);
}
