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
foldModel(const QString &str)
{
  foldClear();

  //---

  using FoldDatas = std::vector<CQFoldData>;

  FoldDatas foldDatas;

  QStringList strs = str.split(",", QString::SkipEmptyParts);

  for (int i = 0; i < strs.length(); ++i) {
    QStringList strs1 = strs[i].split(":", QString::SkipEmptyParts);

    if (strs1.length() == 0)
      continue;

    bool ok;

    int column = strs1[0].toInt(&ok);

    if (! ok)
      continue;

    CQFoldData foldData(column);

    if (strs1.length() > 1) {
      CQFoldData::Type type = CQFoldData::Type::REAL_RANGE;

      int i = 1;

      if (strs1.length() > 2) {
        if (strs1[1] == "i")
          type = CQFoldData::Type::INTEGER_RANGE;

        ++i;
      }

      double delta = strs1[i].toDouble(&ok);

      if (! ok)
        continue;

      foldData.setType (type);
      foldData.setDelta(delta);
    }

    foldDatas.push_back(foldData);
  }

  //---

  ModelP modelp = this->model();

  for (const auto &foldData : foldDatas) {
    QAbstractItemModel *model = modelp.data();

    CQFoldedModel *foldedModel = new CQFoldedModel(model, foldData);

    modelp = ModelP(foldedModel);

    this->addFoldedModel(modelp);
  }

  if (! this->foldedModels().empty()) {
    QAbstractItemModel *model = modelp.data();

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model);

    if (! proxyModel) {
      QSortFilterProxyModel *foldProxyModel = new QSortFilterProxyModel;

      foldProxyModel->setSourceModel(model);

      modelp = ModelP(foldProxyModel);
    }

    this->setFoldProxyModel(modelp);
  }
}

void
CQChartsModelData::
foldClear()
{
  clearFoldedModels();

  resetFoldProxyModel();
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
