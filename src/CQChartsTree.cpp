#include <CQChartsTree.h>

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QMenu>
#include <QActionGroup>
#include <cassert>

class CQChartsTreeSelectionModel : public QItemSelectionModel {
 public:
  CQChartsTreeSelectionModel(CQChartsTree *tree) :
   QItemSelectionModel(tree->CQTreeView::model()), tree_(tree) {
  }

  void select(const QModelIndex &index, SelectionFlags flags) {
    QItemSelectionModel::select(index, adjustFlags(flags));
  }

  void select(const QItemSelection &selection, SelectionFlags flags) {
    QItemSelectionModel::select(selection, adjustFlags(flags));
  }

 private:
  SelectionFlags adjustFlags(SelectionFlags flags) const {
    if     (tree_->selectionBehavior() == QAbstractItemView::SelectRows)
      flags |= Rows;
    else if (tree_->selectionBehavior() == QAbstractItemView::SelectColumns)
      flags |= Columns;

    return flags;
  }

 private:
  CQChartsTree *tree_ { nullptr };
};

//------

CQChartsTree::
CQChartsTree(QWidget *parent) :
 CQTreeView(parent)
{
  setObjectName("tree");

  setSortingEnabled(true);

  header()->setSectionsClickable(true);
  //header()->setHighlightSections(true);

  setSelectionBehavior(SelectRows);
}

void
CQChartsTree::
addMenuActions(QMenu *menu)
{
  CQTreeView::addMenuActions(menu);

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
CQChartsTree::
setModel(const ModelP &model)
{
  if (sm_)
    disconnect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionSlot()));

  //---

  model_ = model;

  CQTreeView::setModel(model_.data());

  sm_ = new CQChartsTreeSelectionModel(this);

  setSelectionModel(sm_);

  //---

  connect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));
}

void
CQChartsTree::
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
CQChartsTree::
selectionSlot()
{
  QModelIndexList indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);
}

void
CQChartsTree::
selectionBehaviorSlot(QAction *action)
{
  if      (action->text() == "Select Items")
    setSelectionBehavior(SelectItems);
  else if (action->text() == "Select Rows")
    setSelectionBehavior(SelectRows);
  else if (action->text() == "Select Columns")
    setSelectionBehavior(SelectColumns);
}

void
CQChartsTree::
calcDetails(Details &details)
{
  details.numColumns = model_->columnCount();
  details.numRows    = model_->rowCount   ();
}

QSize
CQChartsTree::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
