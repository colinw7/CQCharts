#include <CQChartsModelColumnDataControl.h>

#include <CQChartsParamEdit.h>
#include <CQChartsExprModel.h>
#include <CQChartsModelData.h>
#include <CQChartsLineEdit.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewTree.h>
#include <CQDataModel.h>

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
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  editFrame_  = CQUtil::makeWidget<QFrame>("columnEditFrame");
  editLayout_ = CQUtil::makeLayout<QGridLayout>(editFrame_, 0, 2);

  layout->addWidget(editFrame_);

  row_ = 0;

  //---

  auto addLineEdit = [&](const QString &name, const QString &objName) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
    auto *edit  = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit" );

    label->setText(name);

    editLayout_->addWidget(label, row_, 0);
    editLayout_->addWidget(edit , row_, 1);

    ++row_;

    return edit;
  };

  auto addComboBox = [&](const QString &name, const QString &objName) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
    auto *combo = CQUtil::makeWidget<QComboBox>(objName + "Combo");

    label->setText(name);

    editLayout_->addWidget(label, row_, 0);
    editLayout_->addWidget(combo, row_, 1);

    ++row_;

    return combo;
  };

  //---

  numEdit_         = addLineEdit("Number"     , "number"    );
  nameEdit_        = addLineEdit("Name"       , "name"      );
  typeCombo_       = addComboBox("Type"       , "type"      );
  headerTypeCombo_ = addComboBox("Header Type", "headerType");

  QStringList typeNames;

  auto *charts = modelData_->charts();

  auto *columnTypeMgr = charts->columnTypeMgr();

  columnTypeMgr->typeNames(typeNames);

  typeCombo_      ->addItems(typeNames);
  headerTypeCombo_->addItems(typeNames);

  numEdit_        ->setToolTip("Column Number");
  nameEdit_       ->setToolTip("Column Name");
  typeCombo_      ->setToolTip("Column Type");
  headerTypeCombo_->setToolTip("Header Column Type");

  editLayout_->setRowStretch(row_, 1);

  connect(typeCombo_, SIGNAL(currentIndexChanged(int)),
          this, SLOT(typeChangedSlot()));
  connect(headerTypeCombo_, SIGNAL(currentIndexChanged(int)),
          this, SLOT(headerTypeChangedSlot()));

  //---

  layout->addStretch(1);

  //---

  auto *columnButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  layout->addLayout(columnButtonLayout);

  auto *typeApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "typeApply");

  connect(typeApplyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  columnButtonLayout->addStretch(1);
  columnButtonLayout->addWidget(typeApplyButton);
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

  QString numStr = numEdit_->text();

  bool ok;

  int icolumn = (int) CQChartsUtil::toInt(numStr, ok);

  if (! ok) {
    charts->errorMsg("Invalid column number '" + numStr + "'");
    return;
  }

  CQChartsColumn column(icolumn);

  if (! column.isValid()) {
    charts->errorMsg("Invalid column");
    return;
  }

  //--

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

  QString typeStr = typeCombo_->currentText();

  #if 0
  if (! CQChartsUtil::setColumnTypeStr(charts, model.data(), icolumn, typeStr)) {
    charts->errorMsg("Invalid type '" + typeStr + "'");
    return;
  }
#endif

  CQBaseModelType columnType = CQBaseModel::nameType(typeStr);

  const auto *typeData = columnTypeMgr->getType(columnType);

  if (! typeData) {
    charts->errorMsg("Invalid column type '" + typeStr + "'");
    return;
  }

  CQChartsNameValues nameValues;

  for (const auto &paramEdit : paramEdits_) {
    QString name = paramEdit.label->text();

    const auto *param = typeData->getParam(name);

    if (! param) {
      charts->errorMsg("Invalid parameter '" + name + "'");
      return;
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

  if (! columnTypeMgr->setModelColumnType(model.data(), column, columnType, nameValues)) {
    charts->errorMsg("Failed to set column data");
    return;
  }

  //--

  QString headerTypeStr = headerTypeCombo_->currentText();

  CQBaseModelType headerColumnType = CQBaseModel::nameType(headerTypeStr);

  const auto *headerTypeData = columnTypeMgr->getType(headerColumnType);

  if (headerTypeData) {
    if (! columnTypeMgr->setModelHeaderType(model.data(), column, headerColumnType)) {
      charts->errorMsg("Failed to set header type");
      return;
    }
  }

  setColumnData(icolumn);
}

void
CQChartsModelColumnDataControl::
setColumnData(int column)
{
  numEdit_->setText(QString("%1").arg(column));

  //---

  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  auto *charts = modelData_->charts();

  //---

  QString headerStr = model->headerData(column, Qt::Horizontal).toString();

  nameEdit_->setText(headerStr);

  //---

  CQChartsModelTypeData columnTypeData;

  if (CQChartsModelUtil::columnValueType(charts, model.data(), CQChartsColumn(column),
                                         columnTypeData)) {
    auto *columnTypeMgr = charts->columnTypeMgr();

    //--

    const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

  //QString typeStr =
  //  columnTypeMgr->encodeTypeData(columnTypeData.type, columnTypeData.nameValues);

    int typeInd = typeCombo_->findText(typeData->name());

    if (typeInd >= 0)
      typeCombo_->setCurrentIndex(typeInd);

    //--

    const auto *headerTypeData = columnTypeMgr->getType(columnTypeData.headerType);

    int headerTypeInd = typeCombo_->findText(headerTypeData->name());

    if (headerTypeInd >= 0)
      headerTypeCombo_->setCurrentIndex(headerTypeInd);

    //---

    int paramInd = 0;

    editLayout_->setRowStretch(row_, 0);

    for (const auto &param : typeData->params()) {
      while (paramInd >= int(paramEdits_.size())) {
        ParamEdit paramEdit;

        paramEdit.row   = row_ + paramInd;
        paramEdit.label = CQUtil::makeLabelWidget<QLabel>("", "label");
        paramEdit.edit  = new CQChartsParamEdit;

        editLayout_->addWidget(paramEdit.label, paramEdit.row, 0);
        editLayout_->addWidget(paramEdit.edit , paramEdit.row, 1);

        paramEdits_.push_back(paramEdit);
      }

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

      ++paramInd;
    }

    while (paramInd < int(paramEdits_.size())) {
      auto &paramEdit1 = paramEdits_.back();

      CQUtil::removeGridRow(editLayout_, paramEdit1.row, /*delete*/false);

      delete paramEdit1.label;
      delete paramEdit1.edit;

      paramEdits_.pop_back();
    }

    editLayout_->setRowStretch(row_ + paramInd, 1);

    editLayout_->invalidate();
  }
  else {
    typeCombo_      ->setCurrentIndex(-1);
    headerTypeCombo_->setCurrentIndex(-1);
  }
}
