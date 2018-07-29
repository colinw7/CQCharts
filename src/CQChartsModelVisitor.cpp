#include <CQChartsModelVisitor.h>
#include <CQChartsUtil.h>

void
CQChartsModelVisitor::
init(QAbstractItemModel *model)
{
  model_        = model;
  numCols_      = model_->columnCount(QModelIndex());
  hierarchical_ = false;
  hierSet_      = false;
  row_          = 0;
  numRows_      = 0;
}

bool
CQChartsModelVisitor::
isHierarchical() const
{
  if (! hierSet_) {
    CQChartsModelVisitor *th = const_cast<CQChartsModelVisitor *>(this);

    th->hierarchical_ = CQChartsUtil::isHierarchical(model_);
    th->hierSet_      = true;
  }

  return hierarchical_;
}
