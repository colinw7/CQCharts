#include <CQChartsModelControl.h>
#include <CQChartsModelExprControl.h>
#include <CQChartsModelFilterControl.h>
#ifdef CQCHARTS_FOLDED_MODEL
#include <CQChartsModelFoldControl.h>
#endif
#include <CQChartsModelFlattenControl.h>
#include <CQChartsModelColumnDataControl.h>

#include <CQChartsModelData.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewTree.h>
#include <CQUtil.h>

#include <QTabWidget>
#include <QStackedWidget>
#include <QVBoxLayout>

CQChartsModelControl::
CQChartsModelControl(CQCharts *charts, CQChartsModelData *modelData) :
 charts_(charts)
{
  setObjectName("control");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  auto *controlTab = CQUtil::makeWidget<QTabWidget>("tab");

  controlTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(controlTab);

  //---

  columnDataFrame_ = addColumnDataFrame();

  controlTab->addTab(columnDataFrame_, "Column Data");

  //---

  auto *propertiesFrame = addPropertiesFrame();

  controlTab->addTab(propertiesFrame, "Properties");

  //---

  exprFrame_ = addExprFrame();

  controlTab->addTab(exprFrame_, "Extra Colummns");

  //---

  filterFrame_ = addFilterFrame();

  controlTab->addTab(filterFrame_, "Filter");

  //---

#ifdef CQCHARTS_FOLDED_MODEL
  foldFrame_ = addFoldFrame();

  controlTab->addTab(foldFrame_, "Fold");
#endif

  //---

  flattenFrame_ = addFlattenFrame();

  controlTab->addTab(flattenFrame_, "Flatten");

  //---

  setModelData(modelData);
}

CQChartsModelControl::
~CQChartsModelControl()
{
  propertyTree_->setPropertyModel(propertyModel_);

  delete propertyModel_;
}

CQChartsModelColumnDataControl *
CQChartsModelControl::
addColumnDataFrame()
{
  auto *columnDataFrame = new CQChartsModelColumnDataControl(this);

  columnDataFrame->setModelData(modelData_);

  return columnDataFrame;
}

QFrame *
CQChartsModelControl::
addPropertiesFrame()
{
  auto *propertiesFrame       = CQUtil::makeWidget<QFrame>("propertiesFrame");
  auto *propertiesFrameLayout = CQUtil::makeLayout<QVBoxLayout>(propertiesFrame, 2, 2);

  //---

  propertyModel_ = new CQPropertyViewModel;

  propertyTree_ = new CQPropertyViewTree(this, propertyModel_);

  propertiesFrameLayout->addWidget(propertyTree_);

  //------

  return propertiesFrame;
}

CQChartsModelExprControl *
CQChartsModelControl::
addExprFrame()
{
  auto *exprFrame = new CQChartsModelExprControl(this);

  exprFrame->setModelData(modelData_);

  return exprFrame;
}

CQChartsModelFilterControl *
CQChartsModelControl::
addFilterFrame()
{
  auto *filterFrame = new CQChartsModelFilterControl(this);

  filterFrame->setModelData(modelData_);

  return filterFrame;
}

#ifdef CQCHARTS_FOLDED_MODEL
CQChartsModelFoldControl *
CQChartsModelControl::
addFoldFrame()
{
  auto *foldFrame = new CQChartsModelFoldControl(this);

  foldFrame->setModelData(modelData_);

  return foldFrame;
}
#endif

CQChartsModelFlattenControl *
CQChartsModelControl::
addFlattenFrame()
{
  auto *flattenFrame = new CQChartsModelFlattenControl(this);

  flattenFrame->setModelData(modelData_);

  return flattenFrame;
}

void
CQChartsModelControl::
updateCurrentModel()
{
  auto *modelData = charts_->currentModelData();

  setModelData(modelData);
}

void
CQChartsModelControl::
filterTextSlot(const QString &text)
{
  filterFrame_->setFilterText(text);
}

void
CQChartsModelControl::
setModelData(CQChartsModelData *modelData)
{
  if (modelData != modelData_) {
    modelData_ = modelData;

    exprFrame_     ->setModelData(modelData_);
    filterFrame_   ->setModelData(modelData_);
#ifdef CQCHARTS_FOLDED_MODEL
    foldFrame_     ->setModelData(modelData_);
#endif
    flattenFrame_  ->setModelData(modelData_);
    columnDataFrame_->setModelData(modelData_);

    //---

    if (modelData_) {
      auto *propertyModel = modelData_->propertyViewModel();

      propertyTree_->setPropertyModel(propertyModel);
    }
    else {
      propertyTree_->setPropertyModel(propertyModel_);
    }
  }
}
