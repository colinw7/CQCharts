#include <CQChartsModelList.h>
#include <CQChartsModelDataWidget.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQUtil.h>

#include <QVBoxLayout>
#include <QTabWidget>

CQChartsModelList::
CQChartsModelList(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("modelList");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  // per model tab
  viewTab_ = CQUtil::makeWidget<QTabWidget>("viewTab");

  layout->addWidget(viewTab_);

  connect(viewTab_, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

  //---

  connect(charts, SIGNAL(modelDataAdded(int)), this, SLOT(addModelData(int)));

  connect(charts, SIGNAL(modelTypeChanged(int)), this, SLOT(updateModelType(int)));

  connect(charts, SIGNAL(currentModelChanged(int)), this, SLOT(updateCurrentModel()));
}

CQChartsModelList::
~CQChartsModelList()
{
}

void
CQChartsModelList::
addModelData(int ind)
{
  CQChartsModelData *modelData = charts_->getModelData(ind);
  if (! modelData) return;

  addModelData(modelData);
}

void
CQChartsModelList::
addModelData(CQChartsModelData *modelData)
{
  CQChartsWidgetUtil::AutoDisconnect viewTabAutoDisconnect(
    viewTab_, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

  //---

  int ind = modelData->ind();

  auto *modelWidget = new CQChartsModelDataWidget(charts_, modelData);

  viewTab_->addTab(modelWidget, QString("Model %1").arg(ind));

  viewTab_->setCurrentIndex(viewTab_->count() - 1);

  //---

  {
  CQChartsWidgetUtil::AutoDisconnect modelChangedAutoDisconnect(
    charts_, SIGNAL(currentModelChanged(int)), this, SLOT(updateCurrentModel()));

  charts_->setCurrentModelInd(ind);
  }
}

void
CQChartsModelList::
updateCurrentModel()
{
  CQChartsWidgetUtil::AutoDisconnect currentChangedAutoDisconnect(
    viewTab_, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

  CQChartsModelData *modelData = charts_->currentModelData();

  for (int i = 0; i < viewTab_->count(); ++i) {
    CQChartsModelDataWidget *modelWidget =
      qobject_cast<CQChartsModelDataWidget *>(viewTab_->widget(i));

    if (modelWidget->modelData() == modelData) {
      viewTab_->setCurrentIndex(i);
      break;
    }
  }
}

void
CQChartsModelList::
updateModelType(int ind)
{
  CQChartsModelDataWidget *modelWidget = modelDataWidget(ind);

  if (modelWidget)
    modelWidget->setDetails();
}

void
CQChartsModelList::
currentTabChanged(int)
{
  CQChartsWidgetUtil::AutoDisconnect modelChangedAutoDisconnect(
    charts_, SIGNAL(currentModelChanged(int)), this, SLOT(updateCurrentModel()));

  CQChartsModelDataWidget *modelDataWidget = currentModelDataWidget();

  if (modelDataWidget)
    charts_->setCurrentModelInd(modelDataWidget->modelData()->ind());
}

CQChartsModelData *
CQChartsModelList::
currentModelData() const
{
  CQChartsModelDataWidget *modelDataWidget = currentModelDataWidget();

  if (modelDataWidget)
    return charts_->getModelData(modelDataWidget->modelData()->ind());

  return nullptr;
}

CQChartsModelDataWidget *
CQChartsModelList::
currentModelDataWidget() const
{
  CQChartsModelDataWidget *modelDataWidget =
    qobject_cast<CQChartsModelDataWidget *>(viewTab_->currentWidget());

  return modelDataWidget;
}

CQChartsModelDataWidget *
CQChartsModelList::
modelDataWidget(int ind) const
{
  CQChartsModelData *modelData = charts_->getModelData(ind);

  for (int i = 0; i < viewTab_->count(); ++i) {
    CQChartsModelDataWidget *modelWidget =
      qobject_cast<CQChartsModelDataWidget *>(viewTab_->widget(i));

    if (modelWidget->modelData() == modelData)
      return modelWidget;
  }

  return nullptr;
}
