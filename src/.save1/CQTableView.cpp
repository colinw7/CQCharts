#include <CQTableView.h>
#include <CQHeaderView.h>
#include <QMenu>

CQTableView::
CQTableView(QWidget *parent) :
 QTableView(parent)
{
  setObjectName("tableView");

  //--

  header_ = new CQHeaderView(this);

  setHorizontalHeader(header_);

  //---

  setSelectionMode(ExtendedSelection);

  //setUniformRowHeights(true);

  setAlternatingRowColors(true);

  //---

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(customContextMenuSlot(const QPoint&)));
}

CQTableView::
~CQTableView()
{
}

void
CQTableView::
customContextMenuSlot(const QPoint &pos)
{
  // Map point to global from the viewport to account for the header.
  QPoint mpos = viewport()->mapToGlobal(pos);

  QMenu *menu = new QMenu;

  addMenuActions(menu);

  menu->exec(mpos);

  delete menu;
}

void
CQTableView::
addMenuActions(QMenu *menu)
{
  QAction *showVerticalAction = new QAction("Show Vertical", menu);

  showVerticalAction->setCheckable(true);
  showVerticalAction->setChecked(verticalHeader()->isVisible());

  connect(showVerticalAction, SIGNAL(triggered()), this, SLOT(showVertical()));

  menu->addAction(showVerticalAction);
}

void
CQTableView::
showVertical()
{
  verticalHeader()->setVisible(! verticalHeader()->isVisible());
}

void
CQTableView::
update()
{
  header_->update();

  viewport()->update();

  QTableView::update();
}
