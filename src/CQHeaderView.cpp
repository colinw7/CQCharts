#include <CQHeaderView.h>
#include <QTreeWidget>
#include <QTableWidget>
#include <QContextMenuEvent>
#include <QMenu>
#include <cassert>

CQHeaderView::
CQHeaderView(QWidget *parent) :
 QHeaderView(Qt::Horizontal, parent)
{
  setObjectName("headerView");

  connect(this, SIGNAL(sectionResized(int, int, int)), this,
          SLOT(handleSectionResized(int)));
  connect(this, SIGNAL(sectionMoved(int, int, int)), this,
          SLOT(handleSectionMoved(int, int, int)));

  setSectionsMovable(true);

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQHeaderView::
setSectionsMovable(bool movable)
{
#if QT_VERSION < 0x050000
  QHeaderView::setMovable(movable);
#else
  QHeaderView::setSectionsMovable(movable);
#endif
}

void
CQHeaderView::
setSectionsClickable(bool clickable)
{
#if QT_VERSION < 0x050000
  QHeaderView::setClickable(clickable);
#else
  QHeaderView::setSectionsClickable(clickable);
#endif
}

void
CQHeaderView::
setWidgetFactory(CQHeaderViewWidgetFactory *factory)
{
  factory_ = factory;
}

QWidget *
CQHeaderView::
widget(int i) const
{
  return widgets_[i];
}

void
CQHeaderView::
showEvent(QShowEvent *e)
{
  initWidgets();

  QHeaderView::showEvent(e);
}

void
CQHeaderView::
contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);

  menu.addAction("Fit Column", this, SLOT(fitColumnSlot()));
  menu.addAction("Fit All"   , this, SLOT(fitAllSlot()));

  menuSection_ = logicalIndexAt(event->pos());

  menu.exec(event->globalPos());
}

void
CQHeaderView::
fitColumnSlot()
{
  if (menuSection_ < 0)
    return;

  QTreeView  *tree  = qobject_cast<QTreeView  *>(parentWidget());
  QTableView *table = qobject_cast<QTableView *>(parentWidget());

  if      (tree) {
    int nr = tree->model()->rowCount();

    int w = 0;

    for (int r = 0; r < nr; ++r) {
      QModelIndex ind = tree->model()->index(r, menuSection_);

      QSize s = tree->model()->data(ind, Qt::SizeHintRole).toSize();

      w = std::max(w, s.width());
    }

    if (w > 0)
      resizeSection(menuSection_, w);
  }
  else if (table) {
    int nr = table->model()->rowCount();

    int w = 0;

    for (int r = 0; r < nr; ++r) {
      QModelIndex ind = table->model()->index(r, menuSection_);

      QSize s = table->model()->data(ind, Qt::SizeHintRole).toSize();

      w = std::max(w, s.width());
    }

    if (w > 0)
      resizeSection(menuSection_, w);
  }
}

void
CQHeaderView::
fitAllSlot()
{
  QFontMetrics fm(font());

  QTreeView  *tree  = qobject_cast<QTreeView  *>(parentWidget());
  QTableView *table = qobject_cast<QTableView *>(parentWidget());

  if      (tree) {
    int nr = tree->model()->rowCount();
    int nc = tree->model()->columnCount();

    std::map<int,int> w;

    for (int c = 0; c < nc; ++c) {
      QSize s = tree->model()->headerData(c, Qt::Horizontal, Qt::SizeHintRole).toSize();

      if (! s.isValid())
        s = QSize(tree->header()->sectionSizeHint(c), 0);

      if (s.isValid())
        w[c] = std::max(w[c], s.width());
    }

    for (int r = 0; r < nr; ++r) {
      for (int c = 0; c < nc; ++c) {
        QModelIndex ind = tree->model()->index(r, c);

        QSize s = tree->model()->data(ind, Qt::SizeHintRole).toSize();

        if (! s.isValid())
          s = tree->sizeHintForIndex(ind);

        if (! s.isValid()) {
          QString str = tree->model()->data(ind, Qt::DisplayRole).toString();

          s = QSize(fm.width(str) + 8, fm.height() + 4);
        }

        w[c] = std::max(w[c], s.width());
      }
    }

    for (int c = 0; c < nc; ++c) {
      if (w[c] > 0) {
        resizeSection(c, w[c]);
      }
    }
  }
  else if (table) {
    int nr = table->model()->rowCount();
    int nc = table->model()->columnCount();

    std::map<int,int> w;

    for (int c = 0; c < nc; ++c) {
      QSize s = table->model()->headerData(c, Qt::Horizontal, Qt::SizeHintRole).toSize();

      if (! s.isValid())
        s = QSize(table->horizontalHeader()->sectionSizeHint(c), 0);

      if (s.isValid())
        w[c] = std::max(w[c], s.width());
    }

    for (int r = 0; r < nr; ++r) {
      for (int c = 0; c < nc; ++c) {
        QModelIndex ind = table->model()->index(r, c);

        QSize s = table->model()->data(ind, Qt::SizeHintRole).toSize();

        if (! s.isValid())
          s = table->sizeHintForIndex(ind);

        if (! s.isValid()) {
          QString str = table->model()->data(ind, Qt::DisplayRole).toString();

          s = QSize(fm.width(str) + 8, fm.height() + 4);
        }

        w[c] = std::max(w[c], s.width());
      }
    }

    for (int c = 0; c < nc; ++c) {
      if (w[c] > 0) {
        resizeSection(c, w[c] + 8);
      }
    }
  }
}

void
CQHeaderView::
initWidgets()
{
  if (! factory_)
    return;

  int n = count();

  while (widgets_.size() < n)
    widgets_.push_back(0);

  while (widgets_.size() > n) {
    QWidget *w = widgets_.back();

    delete w;

    widgets_.pop_back();
  }

  for (int i = 0; i < n; ++i) {
    QWidget *w = widgets_[i];

    if (! w) {
      w = factory_->createWidget(i);

      w->setParent(this);

      widgets_[i] = w;
    }

    w->setGeometry(sectionViewportPosition(i), 0, sectionSize(i) - 5, height());

    w->show();
  }
}

void
CQHeaderView::
handleSectionResized(int i)
{
  for (int j = visualIndex(i); j < count(); ++j) {
    int logical = logicalIndex(j);

    if (logical < 0 || logical >= widgets_.size())
      continue;

    QWidget *w = widgets_[logical];

    w->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 5, height());
  }
}

void
CQHeaderView::
handleSectionMoved(int /*logical*/, int oldVisualIndex, int newVisualIndex)
{
  for (int i = qMin(oldVisualIndex, newVisualIndex); i < count(); ++i) {
    int logical = logicalIndex(i);

    if (logical < 0 || logical >= widgets_.size())
      continue;

    QWidget *w = widgets_[logical];

    w->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 5, height());
  }
}

#if 0
void
CQHeaderView::
scrollContentsBy(QTableWidget *table, int dx, int dy)
{
  table->scrollContentsBy(dx, dy);

  if (dx != 0)
    fixWidgetPositions();
}
#endif

void
CQHeaderView::
fixWidgetPositions()
{
  for (int i = 0; i < count(); ++i) {
    QWidget *w = widgets_[i];

    w->setGeometry(sectionViewportPosition(i), 0, sectionSize(i) - 5, height());
  }
}
