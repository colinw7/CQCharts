#include <CQChartsTree.h>
#include <QSortFilterProxyModel>
#include <cassert>

CQChartsTree::
CQChartsTree(QWidget *parent) :
 CQTreeView(parent)
{
  setObjectName("table");

  //header()->setSectionsClickable(true);
  //header()->setHighlightSections(true);
}

void
CQChartsTree::
setModel(const ModelP &model)
{
  model_ = model;

  CQTreeView::setModel(model_.data());
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
