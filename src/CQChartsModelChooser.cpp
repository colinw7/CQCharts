#include <CQChartsModelChooser.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsVariant.h>

#include <QComboBox>
#include <QVBoxLayout>

CQChartsModelChooser::
CQChartsModelChooser(CQCharts *charts)
{
  setObjectName("modelChooser");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  combo_->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  layout->addWidget(combo_);

  //---

  setCharts(charts);
}

void
CQChartsModelChooser::
setCharts(CQCharts *charts)
{
  connectSlots(false);

  charts_ = charts;

  connectSlots(true);

  updateModels();
}

void
CQChartsModelChooser::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentModel(int)));

  if (charts_) {
    CQChartsWidgetUtil::connectDisconnect(b,
      charts_, SIGNAL(modelDataChanged()), this, SLOT(updateModels()));
    CQChartsWidgetUtil::connectDisconnect(b,
      charts_, SIGNAL(modelDataDataChanged()), this, SLOT(updateModels()));
  }
}

int
CQChartsModelChooser::
currentModelInd() const
{
  auto *modelData = (charts_ ? charts_->currentModelData() : nullptr);

  return (modelData ? modelData->ind() : -1);
}

void
CQChartsModelChooser::
setCurrentModel(int ind)
{
  bool ok;
  long modelInd = CQChartsVariant::toInt(combo_->itemData(ind), ok);
  if (! ok) return;

  charts_->setCurrentModelInd(int(modelInd));

  emit currentModelChanged();
}

void
CQChartsModelChooser::
updateModels()
{
  connectSlots(false);

  int currentInd = currentModelInd();

  CQCharts::ModelDatas modelDatas;

  if (charts_)
    charts_->getModelDatas(modelDatas);

  combo_->clear();

  int i = 0, ind = -1;

  for (const auto &modelData : modelDatas) {
    if (modelData->isInd(currentInd))
      ind = i;

    combo_->addItem(modelData->desc(), QVariant(modelData->ind()));

    ++i;
  }

  if (ind >= 0)
    combo_->setCurrentIndex(ind);

  connectSlots(true);
}
