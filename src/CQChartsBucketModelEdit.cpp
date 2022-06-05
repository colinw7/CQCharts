#include <CQChartsBucketModelEdit.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>

#include <CQBucketModel.h>
#include <CQRealSpin.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

CQChartsBucketModelEdit::
CQChartsBucketModelEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("bucketModelEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  enabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Enabled", "enabledCheck");

  enabledCheck_->setToolTip("Enable summary of model data");

  layout->addWidget(enabledCheck_);

  //---

  auto *editFrame  = CQUtil::makeWidget<QFrame>("editFrame");
  auto *editLayout = CQUtil::makeLayout<QGridLayout>(editFrame, 0, 2);

  layout->addWidget(editFrame);

  //---

  int row = 0;

  auto addEditWidget = [&](const QString &label, QWidget *edit, const QString &name) {
    editLayout->addWidget(CQUtil::makeLabelWidget<QLabel>(label, name + "Label"), row, 0);
    editLayout->addWidget(edit                                                  , row, 1);

    ++row;
  };

  //---

  columnEdit_ = CQUtil::makeWidget<CQChartsColumnCombo>("columnEdit");
  typeCombo_  = CQUtil::makeWidget<QComboBox          >("typeCombo");
  startEdit_  = CQUtil::makeWidget<CQRealSpin         >("startEdit");
  deltaEdit_  = CQUtil::makeWidget<CQRealSpin         >("deltaEdit");
  minEdit_    = CQUtil::makeWidget<CQRealSpin         >("minEdit");
  maxEdit_    = CQUtil::makeWidget<CQRealSpin         >("maxEdit");
  countEdit_  = CQUtil::makeWidget<CQIntegerSpin      >("countEdit");

  addEditWidget("Column", columnEdit_, "column");
  addEditWidget("Type"  , typeCombo_ , "type");
  addEditWidget("Start" , startEdit_ , "start");
  addEditWidget("Delta" , deltaEdit_ , "delta");
  addEditWidget("Min"   , minEdit_   , "min");
  addEditWidget("Max"   , maxEdit_   , "max");
  addEditWidget("Count" , countEdit_ , "count");

  //---

  multiCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Multi Column", "multiCheck");

  multiCheck_->setToolTip("Enable multiple bucket columns");

  layout->addWidget(multiCheck_);

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQChartsBucketModelEdit::
setModelData(ModelData *modelData)
{
  modelData_ = modelData;

  updateWidgetsFromModel();
}

//---

void
CQChartsBucketModelEdit::
connectSlots(bool b)
{
  CQChartsWidgetUtil::optConnectDisconnect(b,
    modelData_, SIGNAL(dataChanged()), this, SLOT(updateWidgetsFromModel()));

  CQChartsWidgetUtil::connectDisconnect(b,
    enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    columnEdit_, SIGNAL(columnChanged()), this, SLOT(columnSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSlot(int)));
  CQChartsWidgetUtil::connectDisconnect(b,
    startEdit_, SIGNAL(valueChanged(double)), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    deltaEdit_, SIGNAL(valueChanged(double)), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    minEdit_, SIGNAL(valueChanged(double)), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    maxEdit_, SIGNAL(valueChanged(double)), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    countEdit_, SIGNAL(valueChanged(int)), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    multiCheck_, SIGNAL(stateChanged(int)), this, SLOT(updateModelFromWidgets()));
}

//---

void
CQChartsBucketModelEdit::
enabledSlot()
{
  connectSlots(false);

  //---

  if (modelData_) {
    bool enabled = enabledCheck_->isChecked();

    if (enabledCheck_->isChecked())
      modelData_->addBucketModel();

    modelData_->setBucketEnabled(enabled);

    auto *bucketModel = (modelData_ ? modelData_->bucketModel() : nullptr);

    if (bucketModel)
      bucketModel->setBucketPos(-1);
  }

  connectSlots(true);

  //---

  updateWidgetsFromModel();
}

void
CQChartsBucketModelEdit::
columnSlot()
{
  connectSlots(false);

  auto *bucketModel = (modelData_ ? modelData_->bucketModel() : nullptr);

  //---

  int column = columnEdit_->getColumn().column();

  if (bucketModel && column != bucketModel->bucketColumn()) {
    bucketModel->setBucketColumn(column);

    //---

    auto *charts = modelData_->charts();

    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    bool numeric  = false;
    bool integral = false;

    CQChartsModelTypeData columnTypeData;

    if (CQChartsModelUtil::columnValueType(charts, model.data(),
                                           CQChartsColumn(column), columnTypeData)) {
      auto *columnTypeMgr = charts->columnTypeMgr();

      const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

      if (typeData) {
        if (typeData->isNumeric())
          bucketModel->setBucketType(CQBucketModel::BucketType::REAL_AUTO);

        if (typeData->isIntegral())
          bucketModel->setBucketIntegral(true);
      }
    }

    if (numeric != numeric_ || integral != integral_) {
      if (integral)
        bucketModel->setBucketType(CQBucketModel::BucketType::INTEGER_RANGE);
      else
        bucketModel->setBucketType(CQBucketModel::BucketType::REAL_AUTO);

      numeric_  = numeric;
      integral_ = integral;
    }
  }

  //---

  connectSlots(true);

  //---

  updateWidgetsFromModel();
}

void
CQChartsBucketModelEdit::
typeSlot(int ind)
{
  connectSlots(false);

  auto *bucketModel = (modelData_ ? modelData_->bucketModel() : nullptr);

  //---

  auto typeName = typeCombo_->itemText(ind);

  CQBucketModel::BucketType type = CQBucketModel::BucketType::STRING;

  if      (typeName == "STRING"       ) type = CQBucketModel::BucketType::STRING;
  else if (typeName == "INTEGER_RANGE") type = CQBucketModel::BucketType::INTEGER_RANGE;
  else if (typeName == "REAL_RANGE"   ) type = CQBucketModel::BucketType::REAL_RANGE;
  else if (typeName == "REAL_AUTO"    ) type = CQBucketModel::BucketType::REAL_AUTO;

  if (bucketModel)
    bucketModel->setBucketType(type);

  //---

  connectSlots(true);

  //---

  updateWidgetsFromModel();
}

//---

// update widgets from state
void
CQChartsBucketModelEdit::
updateWidgetsFromModel()
{
  connectSlots(false);

  auto *bucketModel = (modelData_ ? modelData_->bucketModel() : nullptr);

  //---

  if (modelData_) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //---

    enabledCheck_->setChecked(modelData_->isBucketEnabled());

    //---

    columnEdit_->setProxy(false);
    columnEdit_->setModelData(modelData_);

    if (bucketModel) {
      typeCombo_->clear();

      if (numeric_) {
        if (! integral_)
          typeCombo_->addItems(QStringList() << "REAL_RANGE" << "REAL_AUTO");
        else
          typeCombo_->addItems(QStringList() << "INTEGER_RANGE");
      }
      else
        typeCombo_->addItems(QStringList() << "STRING");

      QString typeName;

      switch (bucketModel->bucketType()) {
        case CQBucketModel::BucketType::STRING       : typeName = "STRING"; break;
        case CQBucketModel::BucketType::INTEGER_RANGE: typeName = "INTEGER_RANGE"; break;
        case CQBucketModel::BucketType::REAL_RANGE   : typeName = "REAL_RANGE"; break;
        case CQBucketModel::BucketType::REAL_AUTO    : typeName = "REAL_AUTO"; break;
        default: break;
      }

      int typeInd = typeCombo_->findText(typeName);

      if (typeInd >= 0)
        typeCombo_->setCurrentIndex(typeInd);

      //---

      columnEdit_->setColumn (CQChartsColumn(bucketModel->bucketColumn()));
      startEdit_ ->setValue  (bucketModel->bucketStart());
      deltaEdit_ ->setValue  (bucketModel->bucketDelta());
      minEdit_   ->setValue  (bucketModel->bucketMin());
      maxEdit_   ->setValue  (bucketModel->bucketMax());
      countEdit_ ->setValue  (bucketModel->bucketCount());
      multiCheck_->setChecked(bucketModel->isMultiColumn());
    }
  }

  //--

  columnEdit_->setEnabled(bucketModel);
  typeCombo_ ->setEnabled(bucketModel);
  multiCheck_->setEnabled(bucketModel);

  if (bucketModel) {
    auto type = bucketModel->bucketType();

    startEdit_ ->setEnabled(type == CQBucketModel::BucketType::REAL_RANGE ||
                            type == CQBucketModel::BucketType::INTEGER_RANGE);
    deltaEdit_ ->setEnabled(type == CQBucketModel::BucketType::REAL_RANGE ||
                            type == CQBucketModel::BucketType::INTEGER_RANGE);
    minEdit_   ->setEnabled(type == CQBucketModel::BucketType::REAL_AUTO);
    maxEdit_   ->setEnabled(type == CQBucketModel::BucketType::REAL_AUTO);
    countEdit_ ->setEnabled(type == CQBucketModel::BucketType::REAL_AUTO);
  }
  else {
    startEdit_ ->setEnabled(false);
    deltaEdit_ ->setEnabled(false);
    minEdit_   ->setEnabled(false);
    maxEdit_   ->setEnabled(false);
    countEdit_ ->setEnabled(false);
  }

  //--

  connectSlots(true);
}

// update state from widgets
void
CQChartsBucketModelEdit::
updateModelFromWidgets()
{
  auto *bucketModel = (modelData_ ? modelData_->bucketModel() : nullptr);

  if (modelData_->isBucketEnabled() && bucketModel) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //--

    bool changed = false;

    //--

    int column = columnEdit_->getColumn().column();

    if (column != bucketModel->bucketColumn()) {
      bucketModel->setBucketColumn(column); changed = true;
    }

    if (startEdit_->value() != bucketModel->bucketStart()) {
      bucketModel->setBucketStart(startEdit_->value()); changed = true;
    }

    if (deltaEdit_->value() != bucketModel->bucketDelta()) {
      bucketModel->setBucketDelta(deltaEdit_->value()); changed = true;
    }

    if (minEdit_->value() != bucketModel->bucketMin()) {
      bucketModel->setBucketMin(minEdit_->value()); changed = true;
    }

    if (maxEdit_->value() != bucketModel->bucketMax()) {
      bucketModel->setBucketMax(maxEdit_->value()); changed = true;
    }

    if (countEdit_->value() != bucketModel->bucketCount()) {
      bucketModel->setBucketCount(countEdit_->value()); changed = true;
    }

    if (multiCheck_->isChecked() != bucketModel->isMultiColumn()) {
      bucketModel->setMultiColumn(multiCheck_->isChecked()); changed = true;
    }

    if (changed)
      modelData_->emitModelChanged();
  }
}
