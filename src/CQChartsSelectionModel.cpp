#include <CQChartsSelectionModel.h>
#include <CQChartsModelData.h>
#include <QAbstractItemView>

CQChartsSelectionModel::
CQChartsSelectionModel(QAbstractItemView *view, CQChartsModelData *modelData) :
 QItemSelectionModel(modelData->model().data()), view_(view)
{
  setObjectName("chartsSelectionModel");
}

CQChartsSelectionModel::
CQChartsSelectionModel(QAbstractItemView *view, QAbstractItemModel *model) :
 QItemSelectionModel(model), view_(view)
{
  setObjectName("chartsSelectionModel");
}

void
CQChartsSelectionModel::
select(const QModelIndex &ind, SelectionFlags flags)
{
  QItemSelectionModel::select(ind, adjustFlags(flags));
}

void
CQChartsSelectionModel::
select(const QItemSelection &selection, SelectionFlags flags)
{
  QItemSelectionModel::select(selection, adjustFlags(flags));
}

CQChartsSelectionModel::SelectionFlags
CQChartsSelectionModel::
adjustFlags(SelectionFlags flags) const
{
  if      (view_->selectionBehavior() == QAbstractItemView::SelectRows)
    flags |= Rows;
  else if (view_->selectionBehavior() == QAbstractItemView::SelectColumns)
    flags |= Columns;

  return flags;
}
