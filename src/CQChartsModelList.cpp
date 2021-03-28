#include <CQChartsModelList.h>
#include <CQChartsModelDataWidget.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQUtil.h>
#include <CQTabWidget.h>

#include <QVBoxLayout>

CQChartsModelList::
CQChartsModelList(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("modelList");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  // per model tab
  viewTab_ = CQUtil::makeWidget<CQTabWidget>("viewTab");

  viewTab_->setTabsClosable(true);

  layout->addWidget(viewTab_);

  connect(viewTab_, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
  connect(viewTab_, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabSlot(int)));

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
  auto *modelData = charts_->getModelDataByInd(ind);
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

  connect(modelWidget, SIGNAL(filterTextChanged(const QString &)),
          this, SIGNAL(filterTextChanged(const QString &)));

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

  auto *modelData = charts_->currentModelData();

  for (int i = 0; i < viewTab_->count(); ++i) {
    auto *modelWidget = qobject_cast<CQChartsModelDataWidget *>(viewTab_->widget(i));

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
  auto *modelWidget = modelDataWidget(ind);

  if (modelWidget)
    modelWidget->setDetails();
}

void
CQChartsModelList::
currentTabChanged(int)
{
  CQChartsWidgetUtil::AutoDisconnect modelChangedAutoDisconnect(
    charts_, SIGNAL(currentModelChanged(int)), this, SLOT(updateCurrentModel()));

  auto *modelDataWidget = currentModelDataWidget();

  if (modelDataWidget)
    charts_->setCurrentModelData(modelDataWidget->modelData());
}

void
CQChartsModelList::
closeTabSlot(int i)
{
  auto *modelDataWidget = qobject_cast<CQChartsModelDataWidget *>(viewTab_->widget(i));
  if (! modelDataWidget) return;

  auto *modelData = modelDataWidget->modelData();
  if (! modelData) return;

  delete modelDataWidget;

  charts_->removeModelData(modelData);
}

CQChartsModelData *
CQChartsModelList::
currentModelData() const
{
  auto *modelDataWidget = currentModelDataWidget();

  if (modelDataWidget)
    return modelDataWidget->modelData();

  return nullptr;
}

CQChartsModelDataWidget *
CQChartsModelList::
currentModelDataWidget() const
{
  auto *modelDataWidget = qobject_cast<CQChartsModelDataWidget *>(viewTab_->currentWidget());

  return modelDataWidget;
}

CQChartsModelDataWidget *
CQChartsModelList::
modelDataWidget(int ind) const
{
  auto *modelData = charts_->getModelDataByInd(ind);

  for (int i = 0; i < viewTab_->count(); ++i) {
    auto *modelWidget = qobject_cast<CQChartsModelDataWidget *>(viewTab_->widget(i));

    if (modelWidget->modelData() == modelData)
      return modelWidget;
  }

  return nullptr;
}
