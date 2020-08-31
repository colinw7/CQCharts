#include <CQChartsModelColumnDataControl.h>

#include <CQChartsParamEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsExprModel.h>
#include <CQChartsModelData.h>
#include <CQChartsLineEdit.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>
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
setModelData(CQChartsModelData *modelData)
{
  if (modelData != modelData_) {
    if (modelData_)
      disconnect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));

    modelData_ = modelData;

    if (modelData_ && ! editFrame_)
      init();

    if (modelData_)
      setColumnData(modelData_->currentColumn());

    if (modelData_)
      connect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));
  }
}

void
CQChartsModelColumnDataControl::
init()
{
  auto *charts = modelData_->charts();

  //---

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

  editFrame_  = CQUtil::makeWidget<QFrame>("editFrame");
  editLayout_ = CQUtil::makeLayout<QGridLayout>(editFrame_, 0, 2);

  editRow_ = 0;

  split->addWidget(editFrame_, "General");

  //---

  paramFrame_  = CQUtil::makeWidget<QFrame>("paramFrame");
  paramLayout_ = CQUtil::makeLayout<QGridLayout>(paramFrame_, 0, 2);

  split->addWidget(paramFrame_, "Parameters");

  //---

  auto addLineEdit = [&](const QString &name, const QString &objName, const QString &tipText) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
    auto *edit  = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit" );

    label->setText(name);

    editLayout_->addWidget(label, editRow_, 0);
    editLayout_->addWidget(edit , editRow_, 1); ++editRow_;

    edit->setToolTip(tipText);

    return edit;
  };

  auto addComboBox = [&](const QString &name, const QString &objName, const QString &tipText) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
    auto *combo = CQUtil::makeWidget<QComboBox>(objName + "Combo");

    label->setText(name);

    editLayout_->addWidget(label, editRow_, 0);
    editLayout_->addWidget(combo, editRow_, 1); ++editRow_;

    combo->setToolTip(tipText);

    return combo;
  };

  //---

  // column name
  nameEdit_ = addLineEdit("Name", "name", "Column Name");

  //---

  // data and header type
  auto *columnTypeMgr = charts->columnTypeMgr();

  QStringList typeNames;

  columnTypeMgr->typeNames(typeNames);

  //---

  typeCombo_ = addComboBox("Type", "type", "Column Type");

  typeCombo_->addItems(typeNames);

  connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChangedSlot()));

  //---

  headerTypeCombo_ = addComboBox("Header Type", "headerType", "Header Column Type");

  headerTypeCombo_->addItems(typeNames);

  connect(headerTypeCombo_, SIGNAL(currentIndexChanged(int)),
          this, SLOT(headerTypeChangedSlot()));

  //---

  editLayout_->setRowStretch(editRow_, 1);

  //---

  // control bar (Apply)
  auto *columnButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  layout->addLayout(columnButtonLayout);

  auto *typeApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "typeApply");

  connect(typeApplyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  columnButtonLayout->addStretch(1);
  columnButtonLayout->addWidget(typeApplyButton);
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

void
CQChartsModelColumnDataControl::
applySlot()
{
  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  auto *charts = modelData_->charts();

  //---

  // get column to change
  CQChartsColumn column = columnNumEdit_->getColumn();

  if (! column.isValid()) {
    charts->errorMsg("Invalid column");
    return;
  }

  int icolumn = column.column();

  //--

  // set name
  QString nameStr = nameEdit_->text();

  if (nameStr.length()) {
    if (! model->setHeaderData(icolumn, Qt::Horizontal, nameStr, Qt::DisplayRole)) {
      charts->errorMsg("Failed to set name");
      return;
    }
  }

  //---

  auto *columnTypeMgr = charts->columnTypeMgr();

  //--

  // set data type
  QString typeStr = typeCombo_->currentText();

  CQBaseModelType columnType = CQBaseModel::nameType(typeStr);

  const auto *typeData = columnTypeMgr->getType(columnType);

  if (! typeData) {
    charts->errorMsg("Invalid column type '" + typeStr + "'");
    return;
  }

  //---

  // set header type
  QString headerTypeStr = headerTypeCombo_->currentText();

  CQBaseModelType headerColumnType = CQBaseModel::nameType(headerTypeStr);

  const auto *headerTypeData = columnTypeMgr->getType(headerColumnType);

  if (headerTypeData) {
    if (! columnTypeMgr->setModelHeaderType(model.data(), column, headerColumnType)) {
      charts->errorMsg("Failed to set header type");
      return;
    }
  }

  //---

  // update type parameters
  CQChartsNameValues nameValues;

  for (const auto &paramEdit : paramEdits_) {
    QString name = paramEdit.label->text();

    const auto *param = typeData->getParam(name);

    if (! param) {
      charts->errorMsg("Invalid parameter '" + name + "'");
      continue;
    }

    QString value;

    if      (param->type() == CQBaseModelType::BOOLEAN) {
      if (paramEdit.edit->type() == CQBaseModelType::BOOLEAN)
        value = (paramEdit.edit->getBool() ? "1" : "0");
    }
    else if (param->type() == CQBaseModelType::INTEGER) {
      if (paramEdit.edit->type() == CQBaseModelType::INTEGER)
        value = QString("%1").arg(paramEdit.edit->getInteger());
    }
    else if (param->type() == CQBaseModelType::ENUM) {
      if (paramEdit.edit->type() == CQBaseModelType::ENUM)
        value = paramEdit.edit->getEnum();
    }
    else if (param->type() == CQBaseModelType::COLOR) {
      if (paramEdit.edit->type() == CQBaseModelType::COLOR)
        value = paramEdit.edit->getColor();
     }
    else {
      if (paramEdit.edit->type() == CQBaseModelType::STRING)
        value = paramEdit.edit->getString();
    }

    if (value != "")
      nameValues.setNameValue(name, value);
  }

  //---

  // update column type
  if (! columnTypeMgr->setModelColumnType(model.data(), column, columnType, nameValues)) {
    charts->errorMsg("Failed to set column data");
    return;
  }

  //--

  // update controls
  setColumnData(icolumn);
}

void
CQChartsModelColumnDataControl::
setColumnData(int column)
{
  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  auto *charts = modelData_->charts();

  //---

  // update column number
  columnNumEdit_->setModel(model.data());
  columnNumEdit_->setColumn(CQChartsColumn(column));

  //---

  // update column name
  QString headerStr = model->headerData(column, Qt::Horizontal).toString();

  nameEdit_->setText(headerStr);

  //---

  CQChartsModelTypeData columnTypeData;

  if (CQChartsModelUtil::columnValueType(charts, model.data(), CQChartsColumn(column),
                                         columnTypeData)) {
    auto *columnTypeMgr = charts->columnTypeMgr();

    //--

    // update data type
    const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

    int typeInd = typeCombo_->findText(typeData->name());

    if (typeInd >= 0)
      typeCombo_->setCurrentIndex(typeInd);

    //--

    // update header type
    const auto *headerTypeData = columnTypeMgr->getType(columnTypeData.headerType);

    int headerTypeInd = typeCombo_->findText(headerTypeData->name());

    if (headerTypeInd >= 0)
      headerTypeCombo_->setCurrentIndex(headerTypeInd);

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

        paramLayout_->addWidget(paramEdit.label, paramEdit.row, 0);
        paramLayout_->addWidget(paramEdit.edit , paramEdit.row, 1);

        paramEdits_.push_back(paramEdit);
      }

      paramLayout_->setRowStretch(paramInd, 0);

      //---

      // update widget to parameter name and type
      auto &paramEdit = paramEdits_[paramInd];

      paramEdit.label->setText(param->name());

      QVariant var;

      columnTypeData.nameValues.nameValue(param->name(), var);

      if      (param->type() == CQBaseModelType::BOOLEAN)
        paramEdit.edit->setBool(var.toBool());
      else if (param->type() == CQBaseModelType::INTEGER)
        paramEdit.edit->setInteger(var.toInt());
      else if (param->type() == CQBaseModelType::ENUM)
        paramEdit.edit->setEnum(var.toString(), param->values());
      else if (param->type() == CQBaseModelType::COLOR)
        paramEdit.edit->setColor(var.toString());
      else
        paramEdit.edit->setString(var.toString());

      paramEdit.label->setObjectName(param->name() + "_label");
      paramEdit.edit ->setObjectName(param->name() + "_edit" );
      paramEdit.edit ->setToolTip(param->tip());

      //---

      ++paramInd;
    }

    // remove unused edits
    while (paramInd < int(paramEdits_.size())) {
      auto &paramEdit1 = paramEdits_.back();

      CQUtil::removeGridRow(paramLayout_, paramEdit1.row, /*delete*/false);

      delete paramEdit1.label;
      delete paramEdit1.edit;

      paramEdits_.pop_back();
    }

    //---

    paramLayout_->setRowStretch(paramInd, 1);

    paramLayout_->invalidate();
  }
  else {
    typeCombo_      ->setCurrentIndex(-1);
    headerTypeCombo_->setCurrentIndex(-1);
  }
}
