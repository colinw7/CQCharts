#include <CQWidgetMenu.h>

#include <QApplication>

#include <cassert>

// create menu
CQWidgetMenu::
CQWidgetMenu(QWidget *parent) :
 QMenu(parent)
{
  setObjectName("widget_menu");
}

// get menu action (create if necessary)
CQWidgetMenuAction *
CQWidgetMenu::
getAction() const
{
  if (! action_) {
    CQWidgetMenu *th = const_cast<CQWidgetMenu *>(this);

    // create menu action widget
    th->action_ = th->createAction();

    th->addAction(th->action_);

    // connect to menu action widget contents area signals
    CQWidgetMenuArea *area = th->action_->getArea();

    connect(this, SIGNAL(aboutToShow()), area, SLOT(menuShowSlot()));
    connect(this, SIGNAL(aboutToHide()), area, SLOT(menuHideSlot()));

    connect(area, SIGNAL(menuShown ()), this, SIGNAL(menuShown ()));
    connect(area, SIGNAL(menuHidden()), this, SIGNAL(menuHidden()));
  }

  return action_;
}

// set contents widget
void
CQWidgetMenu::
setWidget(QWidget *widget)
{
  getAction()->getArea()->setWidget(widget);
}

// create menu action widget (virtual factory)
CQWidgetMenuAction *
CQWidgetMenu::
createAction()
{
  return new CQWidgetMenuAction(this);
}

// create menu area (virtual factory)
CQWidgetMenuArea *
CQWidgetMenu::
createArea() const
{
  return new CQWidgetMenuArea;
}

void
CQWidgetMenu::
resetSize()
{
  getAction()->getArea()->resetSize();
}

//---------

CQWidgetMenuAction::
CQWidgetMenuAction(CQWidgetMenu *menu) :
 QWidgetAction(menu), menu_(menu), area_(0)
{
}

CQWidgetMenuAction::
~CQWidgetMenuAction()
{
  delete area_;
}

// get menu widget (required implementation for QWidgetAction)
QWidget *
CQWidgetMenuAction::
getWidget() const
{
  return getArea();
}

// get area widget
CQWidgetMenuArea *
CQWidgetMenuAction::
getArea() const
{
  if (! area_) {
    CQWidgetMenuAction *th = const_cast<CQWidgetMenuAction *>(this);

    th->area_ = menu_->createArea();
  }

  return area_;
}

// create menu widget (required implementation for QWidgetAction)
QWidget *
CQWidgetMenuAction::
createWidget(QWidget *parent)
{
  CQWidgetMenuArea *area = getArea();

  area->setParent(parent);

  area->setAction(this);

  return area;
}

//---------

CQWidgetMenuArea::
CQWidgetMenuArea(QWidget *parent) :
 QWidget(parent), action_(0), w_(-1), h_(-1)
{
  setObjectName("menu_area");

  // create contents area
  area_ = new QWidget(this);

  // init contents widget
  areaWidget_ = 0;
}

// set contents widget
void
CQWidgetMenuArea::
setWidget(QWidget *w)
{
  assert(w); // can't be null

  areaWidget_ = w;

  areaWidget_->setParent(area_);
}

// handle resize
void
CQWidgetMenuArea::
resizeEvent(QResizeEvent *)
{
  // update widget layout
  updateLayout();
}

// update contents widget
void
CQWidgetMenuArea::
updateLayout()
{
  int w = width ();
  int h = height();

  area_->move  (0, 0);
  area_->resize(w, h);
}

// register action
void
CQWidgetMenuArea::
setAction(CQWidgetMenuAction *action)
{
  action_ = action;
}

// set area widget to specified size
void
CQWidgetMenuArea::
setSize(int w, int h)
{
  w_ = std::max(w, 16);
  h_ = std::max(h, 16);

  if (areaWidget_) {
    int aw = areaWidget_->sizeHint().width ();
    int ah = areaWidget_->sizeHint().height();

    w_ = std::min(w_, aw);
    h_ = std::min(h_, ah);
  }

  QRect ar = qobject_cast<QMenu *>(parentWidget())->actionGeometry(action_);

  int dx = ar.x();
  int dy = ar.y();

  QWidget *parent = parentWidget();

  parent->resize(w_ + 2*dx, h_ + 2*dy);
}

// reset size
void
CQWidgetMenuArea::
resetSize()
{
  w_ = -1;
  h_ = -1;
}

// menu shown slot
void
CQWidgetMenuArea::
menuShowSlot()
{
  // handle menu shown
  menuShow();

  // signal menu shown
  emit menuShown();
}

// handle menu show
void
CQWidgetMenuArea::
menuShow()
{
  // NOTE: extra show/hide code is necessary if content widget geometry
  // has changed so we update the menu size properly

  QMenu *menu = action_->getMenu();

#if 0
  menu->blockSignals(true);
#endif

  QRect ar = menu->actionGeometry(action_);

  int dx = ar.x();
  int dy = ar.y();

  QSize s = areaWidget_->sizeHint();

  // ensure contents is in menu
  areaWidget_->setParent(area_);

  // update size and position
  menu->resize(s.width() + 2*dx, s.height() + 2*dy);

  areaWidget_->move(0, 0);
  areaWidget_->resize(s);

#if 0
  menu->show();
  qApp->processEvents();

  menu->hide();
  qApp->processEvents();

  menu->show();
  qApp->processEvents();

  menu->blockSignals(false);
#endif
}

// menu hide slot
void
CQWidgetMenuArea::
menuHideSlot()
{
  // handle menu hide
  menuHide();

  // signal menu hidden
  emit menuHidden();
}

// handle menu hide
void
CQWidgetMenuArea::
menuHide()
{
}

// provide size hint for menu size
QSize
CQWidgetMenuArea::
sizeHint() const
{
  int hw = 200, hh = 200;

  if (areaWidget_) {
    hw = areaWidget_->sizeHint().width ();
    hh = areaWidget_->sizeHint().height();
  }

  CQWidgetMenuArea *th = const_cast<CQWidgetMenuArea *>(this);

  th->w_ = hw;
  th->h_ = hh;

  return QSize(w_, h_);
}
