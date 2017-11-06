#include <CQChartsTable.h>
#include <CQChartsHeader.h>

#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QMenu>
#include <QActionGroup>

class CQChartsTableSelectionModel : public QItemSelectionModel {
 public:
  CQChartsTableSelectionModel(CQChartsTable *table) :
   QItemSelectionModel(table->model()), table_(table) {
  }

  void select(const QModelIndex &index, SelectionFlags flags) {
    QItemSelectionModel::select(index, adjustFlags(flags));
  }

  void select(const QItemSelection &selection, SelectionFlags flags) {
    QItemSelectionModel::select(selection, adjustFlags(flags));
  }

 private:
  SelectionFlags adjustFlags(SelectionFlags flags) const {
    if     (table_->selectionBehavior() == QAbstractItemView::SelectRows)
      flags |= Rows;
    else if (table_->selectionBehavior() == QAbstractItemView::SelectColumns)
      flags |= Columns;

    return flags;
  }

 private:
  CQChartsTable *table_ { nullptr };
};

//------

CQChartsTable::
CQChartsTable(QWidget *parent) :
 CQTableView(parent)
{
  setSortingEnabled(true);

  horizontalHeader()->setSectionsClickable(true);
  //horizontalHeader()->setHighlightSections(true);

  verticalHeader()->setVisible(false);

  setSelectionBehavior(SelectRows);

  connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClickSlot(int)));
}

void
CQChartsTable::
addMenuActions(QMenu *menu)
{
  CQTableView::addMenuActions(menu);

  QActionGroup *actionGroup = new QActionGroup(menu);

  QAction *selectItems   = new QAction("Select Items"  , menu);
  QAction *selectRows    = new QAction("Select Rows"   , menu);
  QAction *selectColumns = new QAction("Select Columns", menu);

  selectItems  ->setCheckable(true);
  selectRows   ->setCheckable(true);
  selectColumns->setCheckable(true);

  selectItems  ->setChecked(selectionBehavior() == SelectItems);
  selectRows   ->setChecked(selectionBehavior() == SelectRows);
  selectColumns->setChecked(selectionBehavior() == SelectColumns);

  actionGroup->addAction(selectItems);
  actionGroup->addAction(selectRows);
  actionGroup->addAction(selectColumns);

  connect(actionGroup, SIGNAL(triggered(QAction *)),
          this, SLOT(selectionBehaviorSlot(QAction *)));

  menu->addActions(actionGroup->actions());
}

void
CQChartsTable::
setModel(const ModelP &model)
{
  if (sm_)
    disconnect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionSlot()));

  //---

  model_ = model;

  QTableView::setModel(model_.data());

  sm_ = new CQChartsTableSelectionModel(this);

  setSelectionModel(sm_);

  //---

  connect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));
}

void
CQChartsTable::
setFilter(const QString &filter)
{
  if (! model_)
    return;

  QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_.data());
  assert(proxyModel);

  QString filter1 = filter;

  if (filter.length()) {
    QStringList strs = filter.split(':', QString::KeepEmptyParts);

    if (strs.size() == 2) {
      int column = -1;

      QString name = strs[0];

      for (int i = 0; i < model_->columnCount(); ++i) {
        QString name1 = model_->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();

        if (name == name1) {
          column = i;
          break;
        }
      }

      if (column > 0)
        proxyModel->setFilterKeyColumn(column);

      filter1 = strs[1];
    }
  }

  proxyModel->setFilterWildcard(filter1);
}

void
CQChartsTable::
headerClickSlot(int section)
{
  emit columnClicked(section);
}

void
CQChartsTable::
selectionSlot()
{
  QModelIndexList indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);
}

void
CQChartsTable::
selectionBehaviorSlot(QAction *action)
{
  if      (action->text() == "Select Items")
    setSelectionBehavior(SelectItems);
  else if (action->text() == "Select Rows")
    setSelectionBehavior(SelectRows);
  else if (action->text() == "Select Columns")
    setSelectionBehavior(SelectColumns);
}

QSize
CQChartsTable::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
