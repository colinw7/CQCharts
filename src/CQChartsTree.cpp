#include <CQChartsTree.h>
#include <CQChartsUtil.h>

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

  void select(const QModelIndex &ind, SelectionFlags flags) {
    QItemSelectionModel::select(ind, adjustFlags(flags));
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
CQChartsTree(CQCharts *charts, QWidget *parent) :
 CQTreeView(parent), charts_(charts)
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

  if (model_.data()) {
    sm_ = new CQChartsTreeSelectionModel(this);

    setSelectionModel(sm_);
  }

  //---

  if (sm_)
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

  QString filter1;
  int     column { -1 };

  CQChartsUtil::decodeModelFilterStr(model_.data(), filter, filter1, column);

  if (column >= 0)
    proxyModel->setFilterKeyColumn(column);

  proxyModel->setFilterWildcard(filter1);

  emit filterChanged();
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

  for (int c = 0; c < details.numColumns; ++c) {
    CQChartsUtil::ModelColumnDetails columnDetails(charts_, model_.data(), c);

    QString  typeName = columnDetails.typeName();
    QVariant minValue = columnDetails.minValue();
    QVariant maxValue = columnDetails.maxValue();

    details.columns.emplace_back(typeName, minValue, maxValue);

    details.numRows = std::max(details.numRows, columnDetails.numRows());
  }
}

QSize
CQChartsTree::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
