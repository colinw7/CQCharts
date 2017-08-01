#include <CQHeaderView.h>
#include <QTableWidget>
#include <cassert>

CQHeaderView::
CQHeaderView(QWidget *parent) :
 QHeaderView(Qt::Horizontal, parent)
{
  connect(this, SIGNAL(sectionResized(int, int, int)), this,
          SLOT(handleSectionResized(int)));
  connect(this, SIGNAL(sectionMoved(int, int, int)), this,
          SLOT(handleSectionMoved(int, int, int)));

#if QT_VERSION < 0x050000
  setMovable(true);
#else
  setSectionsMovable(true);
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
