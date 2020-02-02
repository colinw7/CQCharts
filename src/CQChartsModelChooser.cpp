#include <CQChartsModelChooser.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <QComboBox>
#include <QVBoxLayout>

CQChartsModelChooser::
CQChartsModelChooser(CQCharts *charts) :
 charts_(charts)
{
  auto layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  combo_->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  layout->addWidget(combo_);

  connectSlots(true);

  updateModels();
}

void
CQChartsModelChooser::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentModel(int)));
}

int
CQChartsModelChooser::
currentModelInd() const
{
  CQChartsModelData *modelData = charts_->currentModelData();

  return (modelData ? modelData->ind() : -1);
}

void
CQChartsModelChooser::
setCurrentModel(int ind)
{
  bool ok;

  int modelInd = combo_->itemData(ind).toInt(&ok);
  if (! ok) return;

  charts_->setCurrentModelInd(modelInd);

  emit currentModelChanged();
}

void
CQChartsModelChooser::
updateModels()
{
  connectSlots(false);

  int currentInd = currentModelInd();

  CQCharts::ModelDatas modelDatas;

  charts_->getModelDatas(modelDatas);

  combo_->clear();

  int i = 0, ind = -1;

  for (const auto &modelData : modelDatas) {
    if (modelData->ind() == currentInd)
      ind = i;

    combo_->addItem(modelData->desc(), QVariant(modelData->ind()));

    ++i;
  }

  if (ind >= 0)
    combo_->setCurrentIndex(ind);

  connectSlots(true);
}
