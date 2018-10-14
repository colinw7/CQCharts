#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQSummaryModel.h>

#ifdef CQCHARTS_FOLDED_MODEL
#include <CQFoldedModel.h>
#endif

#include <QSortFilterProxyModel>

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
  delete summaryModelData_;
}

QString
CQChartsModelData::
id() const
{
  if (name().length())
    return name();

  return QString("%1").arg(ind());
}

CQChartsModelDetails *
CQChartsModelData::
details()
{
  if (! details_)
    details_ = new CQChartsModelDetails(this);

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
  connect(model_.data(), SIGNAL(modelReset()),
          this, SLOT(modelResetSlot()));

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
  disconnect(model_.data(), SIGNAL(modelReset()),
             this, SLOT(modelResetSlot()));

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
modelResetSlot()
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
#ifdef CQCHARTS_FOLDED_MODEL
  if (! foldedModels_.empty())
    return foldProxyModel_;
#endif

  return model_;
}

#ifdef CQCHARTS_FOLDED_MODEL
void
CQChartsModelData::
foldModel(const FoldData &foldData)
{
  foldClear(/*notify*/false);

  //---

  CQChartsModelDetails *details = this->details();

  using FoldDatas = std::vector<CQFoldData>;

  FoldDatas foldDatas;

  QStringList columnStrs = foldData.columnsStr.split(",", QString::SkipEmptyParts);

  for (int i = 0; i < columnStrs.length(); ++i) {
    QStringList columnSubStrs = columnStrs[i].split(":", QString::SkipEmptyParts);

    if (columnSubStrs.length() == 0)
      continue;

    //---

    // get column
    QString columnStr = columnSubStrs[0];

    CQChartsColumn column;

    if (! CQChartsUtil::stringToColumn(model_.data(), columnStr, column)) {
      bool ok;

      int icolumn = columnStr.toInt(&ok);

      if (! ok)
        continue;

      column = CQChartsColumn(icolumn);
    }

    //---

    // get type, count and delta
    CQFoldData::Type type = CQFoldData::Type::STRING;

    int    count    = foldData.count;
    double delta    = foldData.delta;
    bool   integral = false;

    CQFoldData columnFoldData(column.column());

    if (columnSubStrs.length() > 1) {
      int i = 1;

      // <column>:<type>:<delta>
      if (columnSubStrs.length() > 2) {
        // get type
        QString typeStr = columnSubStrs[1];

        if      (typeStr == "i")
          type = CQFoldData::Type::INTEGER_RANGE;
        else if (typeStr == "r")
          type = CQFoldData::Type::REAL_RANGE;
        else if (typeStr == "a")
          type = CQFoldData::Type::REAL_AUTO;
        else if (typeStr == "s")
          type = CQFoldData::Type::STRING;
        else
          continue;

        //---

        // get delta
        bool ok;

        QString deltaStr = columnSubStrs[2];

        delta = columnSubStrs[i].toDouble(&ok);

        if (! ok)
          continue;
      }
      // <column>:<delta>
      else {
        // set type
        type = CQFoldData::Type::REAL_RANGE;

        //---

        // get delta
        bool ok;

        QString deltaStr = columnSubStrs[1];

        delta = columnSubStrs[i].toDouble(&ok);

        if (! ok)
          continue;
      }
    }
    else {
      // get type from column
      const CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

      if      (columnDetails->type() == CQBaseModel::Type::REAL) {
        if (foldData.isAuto)
          type = CQFoldData::Type::REAL_AUTO;
        else
          type = CQFoldData::Type::REAL_RANGE;
      }
      else if (columnDetails->type() == CQBaseModel::Type::INTEGER) {
        integral = true;

        if (foldData.isAuto)
          type = CQFoldData::Type::REAL_AUTO;
        else
          type = CQFoldData::Type::REAL_RANGE;
      }
      else {
        type = CQFoldData::Type::STRING;
      }
    }

    //---

    // add fold data
    columnFoldData.setType    (type);
    columnFoldData.setNumAuto (count);
    columnFoldData.setDelta   (delta);
    columnFoldData.setIntegral(integral);

    foldDatas.push_back(columnFoldData);
  }

  //---

  ModelP modelp = this->model();

  // create folded models
  for (const auto &foldData : foldDatas) {
    QAbstractItemModel *model = modelp.data();

    CQFoldedModel *foldedModel = new CQFoldedModel(model, foldData);

    modelp = ModelP(foldedModel);

    foldedModels_.push_back(modelp);
  }

  //---

  if (! foldedModels_.empty()) {
    // add sort/filter proxy if needed
    QAbstractItemModel *model = modelp.data();

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model);

    if (! proxyModel) {
      QSortFilterProxyModel *foldProxyModel = new QSortFilterProxyModel;

      foldProxyModel->setObjectName("foldProxyModel");

      foldProxyModel->setSortRole(static_cast<int>(CQBaseModel::Role::CustomSort));

      foldProxyModel->setSourceModel(model);

      modelp = ModelP(foldProxyModel);
    }

    foldProxyModel_ = modelp;
  }

  //---

  delete details_;

  details_ = nullptr;

  //---

  emit currentModelChanged();
}

void
CQChartsModelData::
foldClear(bool notify)
{
  foldedModels_.clear();

  foldProxyModel_ = ModelP();

  if (notify) {
    delete details_;

    details_ = nullptr;

    //---

    emit currentModelChanged();
  }
}

CQChartsModelData::FoldedModels
CQChartsModelData::
foldedModels() const
{
  if (! foldProxyModel_)
    return FoldedModels();

  QSortFilterProxyModel *proxyModel =
    qobject_cast<QSortFilterProxyModel *>(foldProxyModel_.data());
  if (! proxyModel) return FoldedModels();

  CQFoldedModel *foldedModel = qobject_cast<CQFoldedModel *>(proxyModel);

  if (! foldedModel) {
    QAbstractItemModel *sourceModel = proxyModel->sourceModel();

    foldedModel = qobject_cast<CQFoldedModel *>(sourceModel);
  }

  FoldedModels foldedModels;

  while (foldedModel) {
    foldedModels.push_back(foldedModel);

    foldedModel = qobject_cast<CQFoldedModel *>(foldedModel->sourceModel());
  }

  //return foldedModels_;
  return foldedModels;
}
#endif

CQSummaryModel *
CQChartsModelData::
addSummaryModel()
{
  if (! summaryModel_) {
    summaryModel_ = new CQSummaryModel(model_.data());

    summaryModelP_ = ModelP(summaryModel_);

    summaryModelData_ = new CQChartsModelData(charts_, summaryModelP_);
  }

  return summaryModel_;
}
