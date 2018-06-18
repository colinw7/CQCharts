#include <CQChartsModelVisitor.h>
#include <CQChartsUtil.h>

void
CQChartsModelVisitor::
init(QAbstractItemModel *model)
{
  numCols_      = model->columnCount(QModelIndex());
  hierarchical_ = CQChartsUtil::isHierarchical(model);
  row_          = 0;
  numRows_      = 0;
}
