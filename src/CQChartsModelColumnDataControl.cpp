#include <CQChartsModelColumnDataControl.h>

#include <CQChartsParamEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsColumnTypeCombo.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsLineEdit.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>
#include <CQChartsVariant.h>

#include <CQUtil.h>
#include <CQPropertyViewModel.h>
#include <CQPropertyViewTree.h>
#include <CQDataModel.h>
#include <CQTabSplit.h>

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

CQChartsModelColumnDataControl::
CQChartsModelColumnDataControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("columnDataControl");
}

void
CQChartsModelColumnDataControl::
setCharts(CQCharts *charts)
{
  charts_ = charts;
}

int
CQChartsModelColumnDataControl::
modelInd() const
{
  return (modelData() ? modelData()->ind() : -1);
}

void
CQChartsModelColumnDataControl::
setModelInd(int ind)
{
  auto *modelData = (charts_ ? charts_->getModelDataByInd(ind) : nullptr);
  if (! modelData) return;

  setModelData(modelData);
}

void
CQChartsModelColumnDataControl::
setModelData(CQChartsModelData *modelData)
{
  if (modelData != modelData_) {
    if (modelData_)
      disconnect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));

    modelData_ = modelData;
    charts_    = (modelData_ ? modelData_->charts() : nullptr);

    if (modelData_ && ! generalFrame_)
      init();

    if (modelData_) {
      int icolumn = modelData_->currentColumn();

      if (icolumn < 0 && modelData_->details()->numColumns() > 0)
        icolumn = 0;

      setColumnData(0);
    }

    if (modelData_)
      connect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));
  }
}

void
CQChartsModelColumnDataControl::
init()
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  // column chooser
  auto *columnFrame  = CQUtil::makeWidget<QFrame>("columnFrame");
  auto *columnLayout = CQUtil::makeLayout<QHBoxLayout>(columnFrame, 2, 2);

  auto *columnNumLabel = CQUtil::makeLabelWidget<QLabel>("Column", "columnNumLabel");

  columnNumEdit_ = new CQChartsColumnCombo;

  columnNumEdit_->setAllowNone(false);
  columnNumEdit_->setToolTip("Column Number");

  connect(columnNumEdit_, SIGNAL(columnChanged()), this, SLOT(columnNumChanged()));

  columnLayout->addWidget(columnNumLabel);
  columnLayout->addWidget(columnNumEdit_);

  columnLayout->addStretch(1);

  layout->addWidget(columnFrame);

  //---

  auto *split = CQUtil::makeWidget<CQTabSplit>("area");

  split->setOrientation(Qt::Vertical);
  split->setGrouped(true);
  split->setState(CQTabSplit::State::TAB);

  layout->addWidget(split);

  //---

  generalFrame_  = CQUtil::makeWidget<QFrame>("editFrame");
  generalLayout_ = CQUtil::makeLayout<QGridLayout>(generalFrame_, 0, 2);

  generalFrame_->setAutoFillBackground(true);

  split->addWidget(generalFrame_, "General");

  //---

  paramFrame_  = CQUtil::makeWidget<QFrame>("paramFrame");
  paramLayout_ = CQUtil::makeLayout<QVBoxLayout>(paramFrame_);

  paramSubFrame_  = CQUtil::makeWidget<QFrame>("paramSubFrame");
  paramSubLayout_ = CQUtil::makeLayout<QGridLayout>(paramSubFrame_, 0, 2);

  paramFrame_->setAutoFillBackground(true);

  split->addWidget(paramFrame_, "Parameters");

  //---

  nullValueFrame_  = CQUtil::makeWidget<QFrame>("nullValueFrame");
  nullValueLayout_ = CQUtil::makeLayout<QGridLayout>(nullValueFrame_, 0, 2);

  nullValueFrame_->setAutoFillBackground(true);

  split->addWidget(nullValueFrame_, "Null Value");

  //---

  int generalRow = 0;

  auto addGeneralLineEdit = [&](const QString &name, const QString &objName,
                                const QString &tipText) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
    auto *edit  = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit" );

    label->setText(name);

    generalLayout_->addWidget(label, generalRow, 0);
    generalLayout_->addWidget(edit , generalRow, 1); ++generalRow;

    edit->setToolTip(tipText);

    return edit;
  };

  auto addGeneralColumnTypeCombo = [&](const QString &name, const QString &objName,
                                       const QString &tipText) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
    auto *combo = CQUtil::makeWidget<CQChartsColumnTypeCombo>(objName + "Combo");

    label->setText(name);

    combo->setCharts(charts_);

    generalLayout_->addWidget(label, generalRow, 0);
    generalLayout_->addWidget(combo, generalRow, 1); ++generalRow;

    combo->setToolTip(tipText);

    return combo;
  };

  //------

  // column name
  nameEdit_ = addGeneralLineEdit("Name", "name", "Column Name");

  // data type
  typeCombo_ = addGeneralColumnTypeCombo("Type", "type", "Column Type");

  connect(typeCombo_, SIGNAL(typeChanged()), this, SLOT(typeChangedSlot()));

  // header type
  headerTypeCombo_ = addGeneralColumnTypeCombo("Header Type", "headerType", "Header Column Type");

  connect(headerTypeCombo_, SIGNAL(typeChanged()), this, SLOT(headerTypeChangedSlot()));

  //--

  generalLayout_->setRowStretch(generalRow++, 1);

  //--

  // Apply button for General Tab
  auto *generalButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  generalLayout_->addLayout(generalButtonLayout, generalRow++, 0, 1, 2);

  auto *generalApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "generalApply");

  connect(generalApplyButton, SIGNAL(clicked()), this, SLOT(generalApplySlot()));

  generalButtonLayout->addStretch(1);
  generalButtonLayout->addWidget(generalApplyButton);

  //------

  paramLayout_->addWidget(paramSubFrame_);
  paramLayout_->addStretch(1);

  // Apply button for Parameters Tab
  auto *paramButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  paramLayout_->addLayout(paramButtonLayout);

  auto *paramApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "typeApply");

  connect(paramApplyButton, SIGNAL(clicked()), this, SLOT(paramApplySlot()));

  paramButtonLayout->addStretch(1);
  paramButtonLayout->addWidget(paramApplyButton);

  //------

  int nullValueRow = 0;

  auto addNullValueLineEdit = [&](const QString &name, const QString &objName,
                                  const QString &tipText) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
    auto *edit  = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit" );

    label->setText(name);

    nullValueLayout_->addWidget(label, nullValueRow, 0);
    nullValueLayout_->addWidget(edit , nullValueRow, 1); ++nullValueRow;

    edit->setToolTip(tipText);

    return edit;
  };

  //---

  nullValueEdit_ =
    addNullValueLineEdit("Null Value", "nullValue", "Value to apply to Null Values in Column");

  nullValueLayout_->setRowStretch(nullValueRow++, 1);

  //--

  // Apply button for Null Value Tab
  auto *nullValueButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  nullValueLayout_->addLayout(nullValueButtonLayout, nullValueRow++, 0, 1, 2);

  auto *nullValueApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "nullValueApply");

  connect(nullValueApplyButton, SIGNAL(clicked()), this, SLOT(nullValueApplySlot()));

  nullValueButtonLayout->addStretch(1);
  nullValueButtonLayout->addWidget(nullValueApplyButton);
}

void
CQChartsModelColumnDataControl::
columnNumChanged()
{
  if (! modelData_)
    return;

  auto c = columnNumEdit_->getColumn();

  modelData_->setCurrentColumn(c.column());
}

void
CQChartsModelColumnDataControl::
typeChangedSlot()
{
}

void
CQChartsModelColumnDataControl::
headerTypeChangedSlot()
{
}

//------

void
CQChartsModelColumnDataControl::
generalApplySlot()
{
  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  //---

  // get column to change
  auto column = columnNumEdit_->getColumn();

  if (! column.isValid()) {
    charts_->errorMsg("Invalid column");
    return;
  }

  //---

  // set name
  auto nameStr = nameEdit_->text();

  if (nameStr.length()) {
    if (! model->setHeaderData(column.column(), Qt::Horizontal, nameStr, Qt::DisplayRole)) {
      charts_->errorMsg("Failed to set name");
      return;
    }
  }

  //---

  // get current type data
  auto *columnTypeMgr = charts_->columnTypeMgr();

  CQChartsModelTypeData typeData;

  if (! columnTypeMgr->getModelColumnType(model.data(), column, typeData))
    typeData.type = CQBaseModelType::STRING;

  //---

  // set data type
  const auto *columnType = typeCombo_->columnType();

  if (! columnType) {
    charts_->errorMsg("Invalid column type");
    return;
  }

  if (! columnTypeMgr->setModelColumnType(model.data(), column, columnType->type(),
                                          typeData.nameValues)) {
    charts_->errorMsg("Failed to set column type");
    return;
  }

  //---

  // set header type
  const auto *headerColumnType = headerTypeCombo_->columnType();

  if (headerColumnType) {
    if (! columnTypeMgr->setModelHeaderType(model.data(), column, headerColumnType->type())) {
      charts_->errorMsg("Failed to set header type");
      return;
    }
  }

  //---

  // update tabs for new data
  int icolumn = column.column();

  setColumnData(icolumn);
}

void
CQChartsModelColumnDataControl::
paramApplySlot()
{
  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  //---

  // get column to change
  auto column = columnNumEdit_->getColumn();

  if (! column.isValid()) {
    charts_->errorMsg("Invalid column");
    return;
  }

  //---

  // get current type data
  auto *columnTypeMgr = charts_->columnTypeMgr();

  CQChartsModelTypeData typeData;

  if (! columnTypeMgr->getModelColumnType(model.data(), column, typeData))
    typeData.type = CQBaseModelType::STRING;

  auto *columnType = columnTypeMgr->getType(typeData.type);

  //---

  // update type parameters
  auto nameValues = typeData.nameValues;

  for (const auto &paramEdit : paramEdits_) {
    //auto name = paramEdit.label->text();
    auto name = paramEdit.label->property("paramName").toString();

    const auto *param = columnType->getParam(name);

    if (! param) {
      charts_->errorMsg("Invalid parameter '" + name + "'");
      continue;
    }

    QString value;

    if      (param->type() == CQBaseModelType::BOOLEAN) {
      if (paramEdit.edit->type() == CQBaseModelType::BOOLEAN)
        value = (paramEdit.edit->getBool() ? "1" : "0");
    }
    else if (param->type() == CQBaseModelType::INTEGER) {
      if (paramEdit.edit->type() == CQBaseModelType::INTEGER)
        value = QString::number(paramEdit.edit->getInteger());
      else
        value = paramEdit.edit->getString();
    }
    else if (param->type() == CQBaseModelType::ENUM) {
      if (paramEdit.edit->type() == CQBaseModelType::ENUM)
        value = paramEdit.edit->getEnum();
      else
        assert(false);
    }
    else if (param->type() == CQBaseModelType::COLOR) {
      if (paramEdit.edit->type() == CQBaseModelType::COLOR)
        value = paramEdit.edit->getColor();
      else
        assert(false);
     }
    else {
      if (paramEdit.edit->type() == CQBaseModelType::STRING)
        value = paramEdit.edit->getString();
      else
        assert(false);
    }

    if (value != "" || nameValues.hasNameValue(name))
      nameValues.setNameValue(name, value);
  }

  if (! columnTypeMgr->setModelColumnType(model.data(), column, columnType->type(),
                                          nameValues)) {
    charts_->errorMsg("Failed to set column type");
    return;
  }

  //---

  // update tabs for new data
  int icolumn = column.column();

  setColumnData(icolumn);
}

void
CQChartsModelColumnDataControl::
nullValueApplySlot()
{
  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  //---

  // get column to change
  auto column = columnNumEdit_->getColumn();

  if (! column.isValid()) {
    charts_->errorMsg("Invalid column");
    return;
  }

  //---

  disconnect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));

  auto value = nullValueEdit_->text();

  modelData_->replaceNullValues(column, value);

  connect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));

  //---

  // update tabs for new data
  int icolumn = column.column();

  setColumnData(icolumn);
}

//------

int
CQChartsModelColumnDataControl::
columnData() const
{
  return icolumn_;
}

void
CQChartsModelColumnDataControl::
setColumnData(int icolumn)
{
  icolumn_ = icolumn;

  //----

  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  //---

  CQChartsColumn column(icolumn_);

  // update column number
  columnNumEdit_->setModelData(modelData_);
  columnNumEdit_->setColumn(column);

  //---

  // update column name
  auto headerStr = model->headerData(icolumn_, Qt::Horizontal).toString();

  nameEdit_->setText(headerStr);

  //---

  CQChartsModelTypeData columnTypeData;

  if (CQChartsModelUtil::columnValueType(charts_, model.data(), column, columnTypeData)) {
    auto *columnTypeMgr = charts_->columnTypeMgr();

    //--

    // update data type
    const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

    typeCombo_->setColumnType(typeData);

    //--

    // update header type
    const auto *headerTypeData = columnTypeMgr->getType(columnTypeData.headerType);

    headerTypeCombo_->setColumnType(headerTypeData);

    //---

    // update column parameters (add if needed)
    int paramInd = 0;

    for (const auto &param : typeData->params()) {
      // add missing widgets for parameter index
      while (paramInd >= int(paramEdits_.size())) {
        ParamEdit paramEdit;

        paramEdit.row   = paramInd;
        paramEdit.label = CQUtil::makeLabelWidget<QLabel>("", "label");
        paramEdit.edit  = new CQChartsParamEdit;

        paramSubLayout_->addWidget(paramEdit.label, paramEdit.row, 0);
        paramSubLayout_->addWidget(paramEdit.edit , paramEdit.row, 1);

        paramEdits_.push_back(std::move(paramEdit));
      }

      paramSubLayout_->setRowStretch(paramInd, 0);

      //---

      // update widget to parameter name and type
      auto &paramEdit = paramEdits_[size_t(paramInd)];

    //paramEdit.label->setText(param->name());
      paramEdit.label->setText(param->tip());
      paramEdit.label->setProperty("paramName", param->name());

      QVariant var;

      columnTypeData.nameValues.nameValue(param->name(), var);

      if      (param->type() == CQBaseModelType::BOOLEAN) {
        bool ok;
        bool b = CQChartsVariant::toBool(var, ok);

        paramEdit.edit->setBool(b);
      }
      else if (param->type() == CQBaseModelType::INTEGER) {
        bool ok;
        long i = CQChartsVariant::toInt(var, ok);

        if (ok)
          paramEdit.edit->setInteger(int(i));
        else
          paramEdit.edit->setString(var.toString());
      }
      else if (param->type() == CQBaseModelType::ENUM) {
        paramEdit.edit->setEnum(var.toString(), param->values());
      }
      else if (param->type() == CQBaseModelType::COLOR) {
        paramEdit.edit->setColor(var.toString());
      }
      else {
        paramEdit.edit->setString(var.toString());
      }

      paramEdit.label->setObjectName(param->name() + "_label");
      paramEdit.edit ->setObjectName(param->name() + "_edit" );
      paramEdit.edit ->setToolTip(QString("%1 (%2)").arg(param->tip()).arg(param->name()));

      //---

      ++paramInd;
    }

    // remove unused edits
    while (paramInd < int(paramEdits_.size())) {
      auto &paramEdit1 = paramEdits_.back();

      CQUtil::removeGridRow(paramSubLayout_, paramEdit1.row, /*delete*/false);

      delete paramEdit1.label;
      delete paramEdit1.edit;

      paramEdits_.pop_back();
    }

    //---

    paramSubLayout_->setRowStretch(paramInd, 1);

    paramSubLayout_->invalidate();
  }
  else {
    typeCombo_      ->setColumnType(nullptr);
    headerTypeCombo_->setColumnType(nullptr);
  }
}
