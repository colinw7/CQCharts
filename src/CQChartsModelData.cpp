#include <CQChartsModelData.h>

CQChartsModelData::
CQChartsModelData(CQCharts *charts, ModelP &model) :
 charts_(charts), model_(model), details_(charts, model_.data())
{
  connectModel();
}

void
CQChartsModelData::
connectModel()
{
  if (! model_.data())
    return;

  connect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  connect(model_.data(), SIGNAL(layoutChanged()),
          this, SLOT(modelLayoutChangedSlot()));

  connect(model_.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(modelRowsInsertedSlot()));
  connect(model_.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
          this, SLOT(modelRowsRemovedSlot()));
  connect(model_.data(), SIGNAL(columnsInserted(QModelIndex,int,int)),
          this, SLOT(modelColumnsInsertedSlot()));
  connect(model_.data(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
          this, SLOT(modelColumnsRemovedSlot()));
}

void
CQChartsModelData::
disconnectModel()
{
  if (! model_.data())
    return;

  disconnect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
             this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  disconnect(model_.data(), SIGNAL(layoutChanged()),
             this, SLOT(modelLayoutChangedSlot()));

  disconnect(model_.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(modelRowsInsertedSlot()));
  disconnect(model_.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SLOT(modelRowsRemovedSlot()));
  disconnect(model_.data(), SIGNAL(columnsInserted(QModelIndex,int,int)),
             this, SLOT(modelColumnsInsertedSlot()));
  disconnect(model_.data(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
             this, SLOT(modelColumnsRemovedSlot()));
}

void
CQChartsModelData::
modelDataChangedSlot(const QModelIndex & /*tl*/, const QModelIndex & /*br*/)
{
  // TODO: check if model uses changed columns
  //int column1 = tl.column();
  //int column2 = br.column();

  details_.reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelLayoutChangedSlot()
{
  details_.reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelRowsInsertedSlot()
{
  details_.reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelRowsRemovedSlot()
{
  details_.reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelColumnsInsertedSlot()
{
  details_.reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelColumnsRemovedSlot()
{
  details_.reset();

  emit modelChanged();
}

CQChartsModelData::ModelP
CQChartsModelData::
currentModel() const
{
  if (! foldedModels_.empty())
    return foldProxyModel_;

  return model_;
}
