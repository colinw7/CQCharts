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

  menu.addSeparator();

  QAction *stretchAction = menu.addAction("Stretch Last", this, SLOT(stretchLastSlot(bool)));
  stretchAction->setCheckable(true);
  stretchAction->setChecked(stretchLastSection());

  QAction *sortAction = menu.addAction("Sort Indicator", this, SLOT(sortIndicatorSlot(bool)));
  sortAction->setCheckable(true);
  sortAction->setChecked(isSortIndicatorShown());

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
  QTreeView  *tree  = qobject_cast<QTreeView  *>(parentWidget());
  QTableView *table = qobject_cast<QTableView *>(parentWidget());

  ColumnWidths columnWidths;

  QHeaderView        *header = nullptr;
  QAbstractItemModel *model  = nullptr;

  if      (tree) {
    header = tree->header();
    model  = tree->model();
  }
  else if (table) {
    header = table->horizontalHeader();
    model  = table->model();
  }

  if (! model)
    return;

  // calc header max column widths
  int nc = model->columnCount();

  for (int c = 0; c < nc; ++c) {
    QSize s = model->headerData(c, Qt::Horizontal, Qt::SizeHintRole).toSize();

    if (! s.isValid() && header)
      s = QSize(header->sectionSizeHint(c), 0);

    if (s.isValid())
      columnWidths[c] = std::max(columnWidths[c], s.width());
  }

  if      (tree) {
    calcTreeWidths(tree, QModelIndex(), 0, columnWidths);
  }
  else if (table) {
    calcTableWidths(table, columnWidths);
  }

  for (int c = 0; c < nc; ++c) {
    if (columnWidths[c] > 0) {
      resizeSection(c, columnWidths[c] + 24);
    }
  }
}

void
CQHeaderView::
calcTreeWidths(QTreeView *tree, const QModelIndex &ind, int depth, ColumnWidths &columnWidths)
{
  int indent = depth*tree->indentation();

  QFontMetrics fm(font());

  QAbstractItemModel *model = tree->model();

  int nc = model->columnCount();

  // calc row max column widths
  int nr = model->rowCount(ind);

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind1 = model->index(r, c, ind);

      QSize s = model->data(ind1, Qt::SizeHintRole).toSize();

      if (! s.isValid())
        s = tree->sizeHintForIndex(ind1);

      if (! s.isValid()) {
        QString str = model->data(ind1, Qt::DisplayRole).toString();

        s = QSize(fm.width(str), fm.height());
      }

      columnWidths[c] = std::max(columnWidths[c], s.width() + indent);
    }
  }

  // process children (column 0)
  for (int r = 0; r < nr; ++r) {
    QModelIndex ind1 = model->index(r, 0, ind);

    calcTreeWidths(tree, ind1, depth + 1, columnWidths);
  }
}

void
CQHeaderView::
calcTableWidths(QTableView *table, ColumnWidths &columnWidths)
{
  QFontMetrics fm(font());

  QAbstractItemModel *model = table->model();

  int nc = model->columnCount();

  // calc row max widths
  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = model->index(r, c);

      QSize s = model->data(ind, Qt::SizeHintRole).toSize();

      if (! s.isValid())
        s = table->sizeHintForIndex(ind);

      if (! s.isValid()) {
        QString str = model->data(ind, Qt::DisplayRole).toString();

        s = QSize(fm.width(str), fm.height());
      }

      columnWidths[c] = std::max(columnWidths[c], s.width());
    }
  }
}

void
CQHeaderView::
stretchLastSlot(bool b)
{
  setStretchLastSection(b);
}

void
CQHeaderView::
sortIndicatorSlot(bool b)
{
  setSortIndicatorShown(b);
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
