#include <CQChartsModelControl.h>
#include <CQChartsModelExprControl.h>
#ifdef CQCHARTS_FOLDED_MODEL
#include <CQChartsModelFoldControl.h>
#endif
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

  exprFrame_ = addExprFrame();

  controlTab->addTab(exprFrame_, "Expression");

  //---

#ifdef CQCHARTS_FOLDED_MODEL
  foldFrame_ = addFoldFrame();

  controlTab->addTab(foldFrame_, "Fold");
#endif

  //---

  columnDataFrame_ = addColumnDataFrame();

  controlTab->addTab(columnDataFrame_, "Column Data");

  //---

  auto *propertiesFrame = addPropertiesFrame();

  controlTab->addTab(propertiesFrame, "Properties");

  //---

  setModelData(modelData);
}

CQChartsModelControl::
~CQChartsModelControl()
{
  propertyTree_->setPropertyModel(propertyModel_);

  delete propertyModel_;
}

CQChartsModelExprControl *
CQChartsModelControl::
addExprFrame()
{
  auto *exprFrame = new CQChartsModelExprControl(this);

  exprFrame->setModelData(modelData_);

  return exprFrame;
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

void
CQChartsModelControl::
updateCurrentModel()
{
  auto *modelData = charts_->currentModelData();

  setModelData(modelData);
}

void
CQChartsModelControl::
setModelData(CQChartsModelData *modelData)
{
  if (modelData != modelData_) {
    modelData_ = modelData;

    exprFrame_->setModelData(modelData_);

#ifdef CQCHARTS_FOLDED_MODEL
    foldFrame_->setModelData(modelData_);
#endif

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
