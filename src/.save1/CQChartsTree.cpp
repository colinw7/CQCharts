#include <CQChartsTree.h>
#include <CQChartsHeader.h>
#include <QSortFilterProxyModel>

CQChartsTree::
CQChartsTree(QWidget *parent) :
 QTreeView(parent)
{
  setObjectName("table");

  header_ = new CQChartsHeader(this);

  header_->setSectionsClickable(true);
  header_->setHighlightSections(true);

  setHeader(header_);
}

void
CQChartsTree::
setModel(const ModelP &model)
{
  model_ = model;

  QTreeView::setModel(model_.data());
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
