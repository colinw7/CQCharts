#include <CQTabBarWidget.h>
#include <CQTabBar.h>
#include <CQUtil.h>

#include <QStackedWidget>
#include <QMenu>
#include <QContextMenuEvent>

CQTabBarWidget::
CQTabBarWidget(QWidget *parent) :
 QWidget(parent)
{
  setObjectName("tabBarWidget");

  tabBar_ = new CQTabBar(this);

  stack_ = CQUtil::makeWidget<QStackedWidget>(this, "stack");

  if (orientation() == Qt::Horizontal)
    tabBar_->setPosition(CQTabBar::Position::North);
  else
    tabBar_->setPosition(CQTabBar::Position::West);

  connect(tabBar_, SIGNAL(currentChanged(int)), this, SLOT(tabSlot(int)));
  connect(tabBar_, SIGNAL(tabMoved(int, int)), this, SLOT(moveTabSlot(int, int)));

  connect(tabBar_, SIGNAL(showContextMenuSignal(const QPoint &)),
          this, SLOT(showTabMenu(const QPoint &)));

  tabBar_->setVisible(true);
  stack_ ->setVisible(true);

  //---

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQTabBarWidget::
setOrientation(Qt::Orientation orient)
{
  orientation_ = orient;

  if (orientation() == Qt::Horizontal)
    tabBar_->setPosition(CQTabBar::Position::North);
  else
    tabBar_->setPosition(CQTabBar::Position::West);

  resizeEvent(nullptr);
}

Qt::ToolButtonStyle
CQTabBarWidget::
buttonStyle() const
{
  return tabBar_->buttonStyle();
}

void
CQTabBarWidget::
setButtonStyle(const Qt::ToolButtonStyle &buttonStyle)
{
  tabBar_->setButtonStyle(buttonStyle);
}

bool
CQTabBarWidget::
isFlowTabs() const
{
  return tabBar_->isFlowTabs();
}

void
CQTabBarWidget::
setFlowTabs(bool b)
{
  tabBar_->setFlowTabs(b);
}

void
CQTabBarWidget::
clear()
{
  tabBar_->clear();

  while (stack_->count())
    stack_->removeWidget(stack_->widget(0));
}

void
CQTabBarWidget::
addTab(QWidget *page, const QIcon &icon, const QString &text)
{
  stack_->addWidget(page);

  tabBar_->addTab(icon, text, page);
}

void
CQTabBarWidget::
addTab(QWidget *page, const QString &text)
{
  stack_->addWidget(page);

  tabBar_->addTab(text, page);
}

int
CQTabBarWidget::
count() const
{
  return tabBar_->count();
}

//---

QString
CQTabBarWidget::
tabText(int ind) const
{
  return tabBar_->tabText(ind);
}

QIcon
CQTabBarWidget::
tabIcon(int ind) const
{
  return tabBar_->tabIcon(ind);
}

//---

int
CQTabBarWidget::
currentIndex() const
{
  return tabBar_->currentIndex();
}

void
CQTabBarWidget::
setCurrentIndex(int tab)
{
  tabBar_->setCurrentIndex(tab);
}

void
CQTabBarWidget::
tabSlot(int tab)
{
  if (tab >= 0)
    stack_->setCurrentIndex(tab);
}

void
CQTabBarWidget::
moveTabSlot(int fromIndex, int toIndex)
{
  std::vector<QWidget *> widgets;

  for (int i = 0; i < count(); ++i)
    widgets.push_back(stack_->widget(i));

  std::swap(widgets[fromIndex], widgets[toIndex]);

  while (stack_->count())
    stack_->removeWidget(stack_->widget(0));

  for (auto *w : widgets)
    stack_->addWidget(w);

  stack_->setCurrentIndex(tabBar_->currentIndex());
}

void
CQTabBarWidget::
contextMenuEvent(QContextMenuEvent *e)
{
  showTabMenu(e->globalPos());
}

void
CQTabBarWidget::
showTabMenu(const QPoint &gpos)
{
  auto *menu = createTabMenu();

  (void) menu->exec(gpos);

  delete menu;
}

QMenu *
CQTabBarWidget::
createTabMenu() const
{
  auto *menu = new QMenu;

  menu->setObjectName("menu");

  //---

  for (int i = 0; i < count(); ++i) {
    auto icon = tabIcon(i);
    auto text = tabText(i);

    auto *action = menu->addAction(icon, text);

    action->setCheckable(true);
    action->setChecked(i == currentIndex());

    action->setData(i);

    connect(action, SIGNAL(triggered()), this, SLOT(tabActionSlot()));
  }

  return menu;
}

void
CQTabBarWidget::
tabActionSlot()
{
  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  int i = action->data().toInt();

  setCurrentIndex(i);
}

void
CQTabBarWidget::
resizeEvent(QResizeEvent *)
{
  int th = tabBar_->buttonsHeight();

  int w = width();
  int h = height();

  if (orientation() == Qt::Horizontal) {
    tabBar_->move  (0, 0);
    tabBar_->resize(w, th);

    tabBar_->updateSizes();

    int th = tabBar_->buttonsHeight();

    tabBar_->resize(w, th);

    stack_->move  (0, th);
    stack_->resize(w, h - th);
  }
  else {
    tabBar_->move  (0, 0);
    tabBar_->resize(th, h);

    tabBar_->updateSizes();

    int th = tabBar_->buttonsHeight();

    tabBar_->resize(th, h);

    stack_->move  (0, th);
    stack_->resize(w - th, h);
  }
}

QSize
CQTabBarWidget::
sizeHint() const
{
  QFontMetrics fm(font());

  int buttonHeight = fm.height() + 6;

  QSize s;

  for (int i = 0; i < stack_->count(); ++i) {
    if (auto *w = stack_->widget(i)) {
      if (tabBar_->isTabVisible(i))
        s = s.expandedTo(w->sizeHint());
    }
  }

  return QSize(s.width(), s.height() + buttonHeight);
}
