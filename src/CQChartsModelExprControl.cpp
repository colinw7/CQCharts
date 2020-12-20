#include <CQChartsModelExprControl.h>

#include <CQChartsExprModel.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsColumnTypeCombo.h>
#include <CQChartsLineEdit.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

CQChartsModelExprControl::
CQChartsModelExprControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("exprControl");

  init();
}

void
CQChartsModelExprControl::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;

  if (modelData_) {
    columnNumEdit_->setModelData(modelData_);
    typeCombo_    ->setCharts   (modelData_->charts());
  }
}

void
CQChartsModelExprControl::
init()
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //--

  // model radio buttons
  auto *exprModeFrame  = CQUtil::makeWidget<QFrame>("exprMode");
  auto *exprModeLayout = CQUtil::makeLayout<QHBoxLayout>(exprModeFrame, 0, 2);

  layout->addWidget(exprModeFrame);

  auto createRadio = [&](const QString &name, const QString &text,
                         const QString &tipText, bool checked=false) {
    auto *radio = CQUtil::makeWidget<QRadioButton>(name);

    radio->setText(text);
    radio->setToolTip(tipText);

    if (checked)
      radio->setChecked(true);

    connect(radio, SIGNAL(toggled(bool)), this, SLOT(modeSlot()));

    return radio;
  };

  addRadio_    = createRadio("add"   , "Add"   , "Add Column Expression"   , true);
  removeRadio_ = createRadio("remove", "Remove", "Remove Expression"       );
  modifyRadio_ = createRadio("modify", "Modify", "Modify Column Expression");

  exprModeLayout->addWidget(addRadio_   );
  exprModeLayout->addWidget(removeRadio_);
  exprModeLayout->addWidget(modifyRadio_);
  exprModeLayout->addStretch(1);

  //--

  auto *exprGridLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  layout->addLayout(exprGridLayout);

  int row = 0;

  //---

  auto *exprValueLabel = CQUtil::makeLabelWidget<QLabel>("Expression", "valueLabel");

  valueEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("valueEdit");

  valueEdit_->setToolTip("+<expr> OR -<column> OR =<column>:<expr>\n"
    "Use: @<number> as shorthand for column(<number>)\n"
    "Functions: column, row, cell, setColumn, setRow, setCell\n"
    " header, setHeader, type, setType, map, bucket, norm, key, rand");

  exprGridLayout->addWidget(exprValueLabel, row, 0);
  exprGridLayout->addWidget(valueEdit_    , row, 1);

  ++row;

  //----

  // column edit
  columnLabel_ = CQUtil::makeLabelWidget<QLabel>("Column", "columnLabel");

//columnEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("columnEdit");
//columnEdit_->setToolTip("Column to Modify");

  columnNumEdit_ = CQUtil::makeWidget<CQChartsColumnCombo>("columnCombo");

  columnNumEdit_->setAllowNone(false);
  columnNumEdit_->setToolTip("Column to Remove/Modify");
//columnNumEdit_->setExtraOnly(true); // TODO

  exprGridLayout->addWidget(columnLabel_  , row, 0);
//exprGridLayout->addWidget(columnEdit_   , row, 1);
  exprGridLayout->addWidget(columnNumEdit_, row, 1);

  ++row;

  //----

  // name edit
  auto *exprNameLabel = CQUtil::makeLabelWidget<QLabel>("Name", "nameLabel");

  nameEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("nameEdit");
  nameEdit_->setToolTip("Column Name");

  exprGridLayout->addWidget(exprNameLabel, row, 0);
  exprGridLayout->addWidget(nameEdit_    , row, 1);

  ++row;

  //--

  // type edit
  typeLabel_ = CQUtil::makeLabelWidget<QLabel>("Type", "typeLabel");

//typeEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("typeEdit");
//typeEdit_->setToolTip("Column Type");

  typeCombo_ = CQUtil::makeWidget<CQChartsColumnTypeCombo>("typeComno");
  typeCombo_->setToolTip("Column Type");

  exprGridLayout->addWidget(typeLabel_, row, 0);
//exprGridLayout->addWidget(typeEdit_ , row, 1);
  exprGridLayout->addWidget(typeCombo_, row, 1);

  ++row;

  //--

  layout->addStretch(1);

  //--

  auto *exprButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  layout->addLayout(exprButtonLayout);

  auto *exprApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "exprApply");

  connect(exprApplyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  exprButtonLayout->addStretch(1);
  exprButtonLayout->addWidget(exprApplyButton);

  //---

  modeSlot();
}

void
CQChartsModelExprControl::
modeSlot()
{
  exprMode_ = Mode::ADD;

  if      (addRadio_   ->isChecked()) exprMode_ = Mode::ADD;
  else if (removeRadio_->isChecked()) exprMode_ = Mode::REMOVE;
  else if (modifyRadio_->isChecked()) exprMode_ = Mode::MODIFY;

  columnLabel_  ->setEnabled(exprMode_ == Mode::MODIFY);
//columnEdit_   ->setEnabled(exprMode_ != Mode::ADD);
  columnNumEdit_->setEnabled(exprMode_ != Mode::ADD);

  nameEdit_->setEnabled(exprMode_ != Mode::REMOVE);

  typeLabel_->setEnabled(exprMode_ != Mode::REMOVE);
//typeEdit_ ->setEnabled(exprMode_ != Mode::REMOVE);
  typeCombo_->setEnabled(exprMode_ != Mode::REMOVE);
}

void
CQChartsModelExprControl::
applySlot()
{
  auto *charts = modelData_->charts();

  if (! modelData_) {
    charts->errorMsg("No model data");
    return;
  }

  CQChartsExprModel::Function function { CQChartsExprModel::Function::EVAL };

  switch (exprMode_) {
    case Mode::ADD   : function = CQChartsExprModel::Function::ADD   ; break;
    case Mode::REMOVE: function = CQChartsExprModel::Function::DELETE; break;
    case Mode::MODIFY: function = CQChartsExprModel::Function::ASSIGN; break;
    default:                                                           break;
  }

  ModelP model = modelData_->currentModel();

  //---

  // get column
  CQChartsColumn column;

#if 0
  auto columnStr = columnEdit_->text();

  if (! CQChartsModelUtil::stringToColumn(model.data(), columnStr, column)) {
    bool ok;

    int icolumn = (int) CQChartsUtil::toInt(columnStr, ok);

    if (ok)
      column = CQChartsColumn(icolumn);
#else
  column = columnNumEdit_->getColumn();
#endif

  //---

  // set current column (for change notify)
  if (function == CQChartsExprModel::Function::ASSIGN)
    modelData_->setCurrentColumn(column.column());

  //---

  // apply function
  int icolumn1 = column.column();

  auto expr = valueEdit_->text().trimmed();

  if (expr.length()) {
    icolumn1 = CQChartsModelUtil::processExpression(model.data(), function, column, expr);

    if (function == CQChartsExprModel::Function::ADD && icolumn1 >= 0)
      modelData_->setCurrentColumn(icolumn1);
  }

  //---

  if (function == CQChartsExprModel::Function::ADD ||
      function == CQChartsExprModel::Function::ASSIGN) {
    if (icolumn1 < 0) {
      charts->errorMsg("Invalid column");
      return;
    }

    // set name
    auto nameStr = nameEdit_->text();

    if (nameStr.length())
      model->setHeaderData(icolumn1, Qt::Horizontal, nameStr, Qt::DisplayRole);

    //---

    // set type
    CQChartsColumn column1(icolumn1);

#if 0
    auto typeStr = typeEdit_->text();

    if (typeStr.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts, model.data(), column1, typeStr)) {
        charts->errorMsg("Invalid type '" + typeStr + "'");
        return;
      }
    }
#endif

    const auto *typeData = typeCombo_->columnType();

    if (! typeData) {
      charts->errorMsg("Invalid column type");
      return;
    }

    auto *columnTypeMgr = charts->columnTypeMgr();

    CQChartsNameValues nameValues;

    if (! columnTypeMgr->setModelColumnType(model.data(), column1, typeData->type(), nameValues)) {
      charts->errorMsg("Failed to set column type");
      return;
    }
  }
}
