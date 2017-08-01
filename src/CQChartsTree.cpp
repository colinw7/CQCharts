#include <CQChartsTree.h>
#include <CQChartsHeader.h>

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
setModel(QAbstractItemModel *model)
{
  QTreeView::setModel(model);
}
