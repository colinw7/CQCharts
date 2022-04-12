#include <CQChartsModelExprControl.h>

#include <CQChartsExprModel.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsColumnTypeCombo.h>
#include <CQChartsColumnExprEdit.h>
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
  if (modelData_)
    disconnect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(modelColumnSlot(int)));

  modelData_ = modelData;

  exprEdit_     ->setModelData(modelData_);
  columnNumEdit_->setModelData(modelData_);

  if (modelData_)
    typeCombo_->setCharts(modelData_->charts());

  column_ = columnNumEdit_->getColumn();

  auto *columnType = typeCombo_->columnType();

  if (columnType)
    type_ = CQChartsColumnTypeId(columnType->type());

  if (modelData_)
    connect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(modelColumnSlot(int)));
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

  auto *exprLabel = CQUtil::makeLabelWidget<QLabel>("Expression", "exprLabel");

  exprEdit_ = CQUtil::makeWidget<CQChartsColumnExprEdit>("exprEdit");

  exprGridLayout->addWidget(exprLabel, row, 0);
  exprGridLayout->addWidget(exprEdit_, row, 1);

  connect(exprEdit_, SIGNAL(exprChanged()), this, SLOT(exprSlot()));

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

  connect(columnNumEdit_, SIGNAL(columnChanged()), this, SLOT(columnSlot()));

  ++row;

  //----

  // name edit
  auto *exprNameLabel = CQUtil::makeLabelWidget<QLabel>("Name", "nameLabel");

  nameEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("nameEdit");
  nameEdit_->setToolTip("Column Name");

  exprGridLayout->addWidget(exprNameLabel, row, 0);
  exprGridLayout->addWidget(nameEdit_    , row, 1);

  connect(nameEdit_, SIGNAL(editingFinished()), this, SLOT(nameSlot()));

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

  connect(typeCombo_, SIGNAL(typeChanged()), this, SLOT(typeSlot()));

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
  auto mode = Mode::ADD;

  if      (addRadio_   ->isChecked()) mode = Mode::ADD;
  else if (removeRadio_->isChecked()) mode = Mode::REMOVE;
  else if (modifyRadio_->isChecked()) mode = Mode::MODIFY;

  setMode(mode);
}

void
CQChartsModelExprControl::
setMode(const Mode &mode)
{
  exprMode_ = mode;

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
exprSlot()
{
  expr_ = exprEdit_->expr();
}

void
CQChartsModelExprControl::
setExpr(const QString &s)
{
  expr_ = s;

  exprEdit_->setExpr(s);
}

void
CQChartsModelExprControl::
columnSlot()
{
  column_ = columnNumEdit_->getColumn();
}

void
CQChartsModelExprControl::
modelColumnSlot(int icolumn)
{
  setColumn(CQChartsColumn(icolumn));
}

void
CQChartsModelExprControl::
setColumn(const CQChartsColumn &column)
{
  column_ = column;

  columnNumEdit_->setColumn(column_);

  if (modelData_) {
    auto *absModel  = CQChartsModelUtil::getBaseModel(modelData_->currentModel().data());
    auto *exprModel = CQChartsModelUtil::getExprModel(absModel);

    QString header, expr;
    bool    columnSet = false;

    if (exprModel) {
      int icolumn = column.column();

      if (icolumn >= 0 && exprModel->getExtraColumnDetails(icolumn, header, expr))
        columnSet = true;
    }

    if (! columnSet) {
      bool ok;
      header = CQChartsModelUtil::modelHHeaderString(modelData_->currentModel().data(), column, ok);
      expr = "";
    }

    nameEdit_->setText(header);
    exprEdit_->setText(expr);
  }
}

void
CQChartsModelExprControl::
nameSlot()
{
  name_ = nameEdit_->text();
}

void
CQChartsModelExprControl::
setName(const QString &s)
{
  name_ = s;

  nameEdit_->setText(s);
}

void
CQChartsModelExprControl::
typeSlot()
{
  const auto *typeData = typeCombo_->columnType();

  type_ = CQChartsColumnTypeId(typeData ? typeData->type() : CQBaseModelType::STRING);
}

void
CQChartsModelExprControl::
setType(const CQChartsColumnTypeId &t)
{
  type_ = t;

  if (modelData_) {
    auto *charts = modelData_->charts();

    auto *columnTypeMgr = charts->columnTypeMgr();

    auto *typeP = columnTypeMgr->getType(type_.type());

    typeCombo_->setColumnType(typeP);
  }
}

void
CQChartsModelExprControl::
applySlot()
{
  if (! modelData_) {
    std::cerr << "No model data\n";
    return;
  }

  auto *charts = modelData_->charts();

  auto model = modelData_->currentModel();

  //---

  auto function = CQChartsExprModel::Function::EVAL;

  switch (exprMode_) {
    case Mode::ADD   : function = CQChartsExprModel::Function::ADD   ; break;
    case Mode::REMOVE: function = CQChartsExprModel::Function::DELETE; break;
    case Mode::MODIFY: function = CQChartsExprModel::Function::ASSIGN; break;
    default:                                                           break;
  }

  //---

  // get column
#if 0
  auto columnStr = columnEdit_->text();

  if (! CQChartsModelUtil::stringToColumn(model.data(), columnStr, column)) {
    bool ok;

    long icolumn = CQChartsUtil::toInt(columnStr, ok);

    if (ok)
      column_ = CQChartsColumn(icolumn);
#else
  column_ = columnNumEdit_->getColumn();
#endif

  // set current column (for change notify)
  if (function == CQChartsExprModel::Function::ASSIGN)
    modelData_->setCurrentColumn(column_.column());

  //---

  // apply function
  int icolumn1 = column_.column();

  expr_ = exprEdit_->expr();

  if (expr_.length()) {
    // process expression and return new column
    icolumn1 = CQChartsModelUtil::processExpression(model.data(), function, column_, expr_);

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
    name_ = nameEdit_->text();

    if (name_.length())
      model->setHeaderData(icolumn1, Qt::Horizontal, name_, Qt::DisplayRole);

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

    type_ = CQChartsColumnTypeId(typeData ? typeData->type() : CQBaseModelType::STRING);

    auto *columnTypeMgr = charts->columnTypeMgr();

    CQChartsNameValues nameValues;

    if (! columnTypeMgr->setModelColumnType(model.data(), column1, type_.type(), nameValues)) {
      charts->errorMsg("Failed to set column type");
      return;
    }
  }
}
