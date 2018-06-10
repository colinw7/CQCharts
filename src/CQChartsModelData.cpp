#include <CQChartsModelData.h>

CQChartsModelData::
CQChartsModelData(CQCharts *charts, ModelP &model) :
 charts_(charts), model_(model)
{
  connectModel();
}

CQChartsModelData::
~CQChartsModelData()
{
  delete details_;
}

CQChartsModelDetails *
CQChartsModelData::
details()
{
  if (! details_)
    details_ = new CQChartsModelDetails(charts_, model_.data());

  return details_;
}

const CQChartsModelDetails *
CQChartsModelData::
details() const
{
  return const_cast<CQChartsModelData *>(this)->details();
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

  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelLayoutChangedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelRowsInsertedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelRowsRemovedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelColumnsInsertedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelColumnsRemovedSlot()
{
  if (details_)
    details_->reset();

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
