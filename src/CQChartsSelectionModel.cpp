#include <CQChartsSelectionModel.h>
#include <CQChartsModelData.h>
#include <QAbstractItemView>

#ifdef CQCHARTS_MODEL_VIEW
namespace {

CQModelView *modelView(QAbstractItemView *view) {
  return qobject_cast<CQModelView *>(view);
}

}

//---

CQChartsSelectionModel::
CQChartsSelectionModel(QAbstractItemView *view, CQChartsModelData *modelData) :
 CQChartsSelectionModelBase(modelView(view)), view_(view)
{
  setObjectName("chartsSelectionModel");

  setModel(modelData->model().data());
}

CQChartsSelectionModel::
CQChartsSelectionModel(QAbstractItemView *view, QAbstractItemModel *model) :
 CQChartsSelectionModelBase(modelView(view)), view_(view)
{
  setObjectName("chartsSelectionModel");

  setModel(model);
}
#else
CQChartsSelectionModel::
CQChartsSelectionModel(QAbstractItemView *view, CQChartsModelData *modelData) :
 CQChartsSelectionModelBase(modelData->model().data()), view_(view)
{
  setObjectName("chartsSelectionModel");
}

CQChartsSelectionModel::
CQChartsSelectionModel(QAbstractItemView *view, QAbstractItemModel *model) :
 CQChartsSelectionModelBase(model), view_(view)
{
  setObjectName("chartsSelectionModel");
}
#endif

void
CQChartsSelectionModel::
select(const QModelIndex &ind, SelectionFlags flags)
{
  CQChartsSelectionModelBase::select(ind, adjustFlags(flags));
}

void
CQChartsSelectionModel::
select(const QItemSelection &selection, SelectionFlags flags)
{
  CQChartsSelectionModelBase::select(selection, adjustFlags(flags));
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
