#include <CQChartsTree.h>
#include <QSortFilterProxyModel>
#include <cassert>

class CQChartsTreeSelectionModel : public QItemSelectionModel {
 public:
  CQChartsTreeSelectionModel(CQChartsTree *tree) :
   QItemSelectionModel(tree->model()), tree_(tree) {
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

  //header()->setSectionsClickable(true);
  //header()->setHighlightSections(true);

  setSelectionBehavior(SelectRows);
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

  proxyModel->setFilterWildcard(filter);
}

void
CQChartsTree::
selectionSlot()
{
  QModelIndexList indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);
}
