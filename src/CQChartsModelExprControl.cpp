#include <CQChartsModelExprControl.h>

#include <CQChartsExprModel.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsColumnTypeCombo.h>
#include <CQChartsColumnExprEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsModelProcess.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>

#include <CQUtil.h>
#include <CQTclUtil.h>

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

  auto *columnType = typeCombo_->columnType();

  if (columnType)
    setType(CQChartsColumnTypeId(columnType->type()));

  if (modelData_)
    connect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(modelColumnSlot(int)));
}

void
CQChartsModelExprControl::
init()
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

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

  //--

  // add type (add, remove, modify)
  addRadio_     = createRadio("add"    , "Add"    , "Add Column Expression", true);
  removeRadio_  = createRadio("remove" , "Remove" , "Remove Expression"       );
  modifyRadio_  = createRadio("modify" , "Modify" , "Modify Column Expression");
  replaceRadio_ = createRadio("replace", "Replace", "Replace Value with New Value");

  exprModeLayout->addWidget(addRadio_   );
  exprModeLayout->addWidget(removeRadio_);
  exprModeLayout->addWidget(modifyRadio_);
  exprModeLayout->addWidget(replaceRadio_);
  exprModeLayout->addStretch(1);

  //---

  auto *exprGridLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  layout->addLayout(exprGridLayout);

  int row = 0;

  //---

  // column edit (remove, modify)
  columnLabel_ = CQUtil::makeLabelWidget<QLabel>("Column", "columnLabel");

//columnEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("columnEdit");
//columnEdit_->setToolTip("Column to Modify");

  columnNumEdit_ = CQUtil::makeWidget<CQChartsColumnCombo>("columnCombo");

  columnNumEdit_->setAllowNone(false);
  columnNumEdit_->setToolTip("Column to Remove/Modify");
//columnNumEdit_->setExtraOnly(true); // TODO

  exprGridLayout->addWidget(columnLabel_  , row, 0);
//exprGridLayout->addWidget(columnEdit_   , row, 3);
  exprGridLayout->addWidget(columnNumEdit_, row, 1, 1, 3);

  connect(columnNumEdit_, SIGNAL(columnChanged()), this, SLOT(columnSlot()));

  ++row;

  //---

  // header edit (add, modify)
  headerLabel_ = CQUtil::makeLabelWidget<QLabel>("Header", "headerLabel");
  headerEdit_  = CQUtil::makeWidget<CQChartsLineEdit>("headerEdit");
  headerEdit_->setToolTip("Column Header");

  exprGridLayout->addWidget(headerLabel_, row, 0);
  exprGridLayout->addWidget(headerEdit_ , row, 1, 1, 3);

  ++row;

  //---

  // expression edit (add, modify)
  exprLabel_ = CQUtil::makeLabelWidget<QLabel>("Expression", "exprLabel");
  exprEdit_  = CQUtil::makeWidget<CQChartsColumnExprEdit>("exprEdit");

  exprGridLayout->addWidget(exprLabel_, row, 0);
  exprGridLayout->addWidget(exprEdit_ , row, 1, 1, 3);

  ++row;

  //----

  // variables edit (add)
  varLabel_ = CQUtil::makeLabelWidget<QLabel>("Variables", "exprLabel");
  varEdit_  = CQUtil::makeWidget<CQChartsLineEdit>("exprEdit");

  exprGridLayout->addWidget(varLabel_, row, 0);
  exprGridLayout->addWidget(varEdit_ , row, 1, 1, 3);

  ++row;

  //----

  // column type edit (add, modify)
  typeLabel_ = CQUtil::makeLabelWidget<QLabel>("Type", "typeLabel");

//typeEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("typeEdit");
//typeEdit_->setToolTip("Column Type");

  typeCombo_ = CQUtil::makeWidget<CQChartsColumnTypeCombo>("typeComno");
  typeCombo_->setToolTip("Column Type");

  exprGridLayout->addWidget(typeLabel_, row, 0);
//exprGridLayout->addWidget(typeEdit_ , row, 1, 1, 3);
  exprGridLayout->addWidget(typeCombo_, row, 1, 1, 3);

  ++row;

  //----

  // column type edit (add, modify)
  oldValueLabel_ = CQUtil::makeLabelWidget<QLabel>("Old Value", "oldValueLabel");
  oldValueEdit_  = CQUtil::makeWidget<CQChartsLineEdit>("oldValue");
  newValueLabel_ = CQUtil::makeLabelWidget<QLabel>("New Value", "newValueLabel");
  newValueEdit_  = CQUtil::makeWidget<CQChartsLineEdit>("newValue");

  exprGridLayout->addWidget(oldValueLabel_, row, 0);
  exprGridLayout->addWidget(oldValueEdit_ , row, 1);
  exprGridLayout->addWidget(newValueLabel_, row, 2);
  exprGridLayout->addWidget(newValueEdit_ , row, 3);

  ++row;

  //---

  layout->addStretch(1);

  //--

  // Button Bar (Apply)
  auto *exprButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  layout->addLayout(exprButtonLayout);

  auto *exprApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "exprApply");

  connect(exprApplyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  exprButtonLayout->addStretch(1);
  exprButtonLayout->addWidget(exprApplyButton);

  //---

  modeSlot();
}

//---

void
CQChartsModelExprControl::
modeSlot()
{
  setMode(this->mode());
}

CQChartsModelExprControl::Mode
CQChartsModelExprControl::
mode() const
{
  auto mode = Mode::ADD;

  if      (addRadio_    ->isChecked()) mode = Mode::ADD;
  else if (removeRadio_ ->isChecked()) mode = Mode::REMOVE;
  else if (modifyRadio_ ->isChecked()) mode = Mode::MODIFY;
  else if (replaceRadio_->isChecked()) mode = Mode::REPLACE;

  return mode;
}

void
CQChartsModelExprControl::
setMode(const Mode &mode)
{
  // column edit (modify/remove)
  bool hasColumn = (mode == Mode::REMOVE || mode == Mode::MODIFY);
  columnLabel_  ->setEnabled(hasColumn);
//columnEdit_   ->setEnabled(hasColumn);
  columnNumEdit_->setEnabled(hasColumn);

  // header edit (add, modify)
  bool hasHeader = (mode == Mode::ADD || mode == Mode::MODIFY);
  headerLabel_->setEnabled(hasHeader);
  headerEdit_ ->setEnabled(hasHeader);

  // expr edit (add, modify)
  bool hasExpr = (mode == Mode::ADD || mode == Mode::MODIFY);
  exprLabel_->setEnabled(hasExpr);
  exprEdit_ ->setEnabled(hasExpr);

  // var edit (add)
  bool hasVar = (mode == Mode::ADD);
  varLabel_->setEnabled(hasVar);
  varEdit_ ->setEnabled(hasVar);

  // type edit (add, modify)
  bool hasType = (mode == Mode::ADD || mode == Mode::MODIFY);
  typeLabel_->setEnabled(hasType);
//typeEdit_ ->setEnabled(hasType);
  typeCombo_->setEnabled(hasType);

  // old/new edit (replace)
  bool hasReplace = (mode == Mode::REPLACE);
  oldValueLabel_->setEnabled(hasReplace);
  oldValueEdit_ ->setEnabled(hasReplace);
  newValueLabel_->setEnabled(hasReplace);
  newValueEdit_ ->setEnabled(hasReplace);
}

//---

void
CQChartsModelExprControl::
columnSlot()
{
  setColumn(columnNumEdit_->getColumn());
}

void
CQChartsModelExprControl::
modelColumnSlot(int icolumn)
{
  setColumn(CQChartsColumn(icolumn));
}

CQChartsModelExprControl::Column
CQChartsModelExprControl::
column() const
{
  return columnNumEdit_->getColumn();
}

void
CQChartsModelExprControl::
setColumn(const CQChartsColumn &column)
{
  // update column edit
  if (column != this->column())
    columnNumEdit_->setColumn(column);

  //---

  // update header and expr from model
  if (modelData_) {
  //auto *baseModel = CQChartsModelUtil::getBaseModel(modelData_->currentModel().data());
    auto *exprModel = CQChartsModelUtil::getExprModel(modelData_->currentModel().data());

    QString header, expr, var;
    bool    columnSet = false;

    if (exprModel) {
      int icolumn = column.column();

      if (icolumn >= 0 && exprModel->getExtraColumnDetails(icolumn, header, expr))
        columnSet = true;

      QStringList strs;

      for (const auto & [name, var] : exprModel->nameValues()) {
        QStringList strs1;

        strs1 << name;
        strs1 << var.toString();

        strs << CQTclUtil::mergeList(strs1);
      }

      var = CQTclUtil::mergeList(strs);
    }

    if (! columnSet) {
      bool ok;
      header = CQChartsModelUtil::modelHHeaderString(modelData_->currentModel().data(), column, ok);
      expr = "";
    }

    headerEdit_->setText(header);
    exprEdit_  ->setText(expr);
    varEdit_   ->setText(var);
  }

  // TODO: column type
}

//---

QString
CQChartsModelExprControl::
header() const
{
  return headerEdit_->text();
}

void
CQChartsModelExprControl::
setHeader(const QString &s)
{
  headerEdit_->setText(s);
}

//---

QString
CQChartsModelExprControl::
expr() const
{
  return exprEdit_->expr();
}

void
CQChartsModelExprControl::
setExpr(const QString &s)
{
  exprEdit_->setExpr(s);
}

//---

QString
CQChartsModelExprControl::
var() const
{
  return varEdit_->text();
}

void
CQChartsModelExprControl::
setVar(const QString &s)
{
  varEdit_->setText(s);
}

//---

CQChartsColumnTypeId
CQChartsModelExprControl::
type() const
{
  const auto *typeData = typeCombo_->columnType();

  return CQChartsColumnTypeId(typeData ? typeData->type() : CQBaseModelType::STRING);
}

void
CQChartsModelExprControl::
setType(const CQChartsColumnTypeId &t)
{
  if (! modelData_) return;

  auto *charts = modelData_->charts();

  auto *columnTypeMgr = charts->columnTypeMgr();

  auto *typeP = columnTypeMgr->getType(t.type());

  typeCombo_->setColumnType(typeP);
}

//---

void
CQChartsModelExprControl::
applySlot()
{
  if (! modelData_) {
    std::cerr << "No model data\n";
    return;
  }

  //---

  auto *charts = modelData_->charts();
  auto  model  = modelData_->currentModel();

  //---

  using ProcessType = CQChartsModelProcessData::Type;

  auto processType = ProcessType::NONE;

  auto mode = this->mode();

  switch (mode) {
    case Mode::ADD    : processType = ProcessType::ADD    ; break;
    case Mode::REMOVE : processType = ProcessType::DELETE ; break;
    case Mode::MODIFY : processType = ProcessType::MODIFY ; break;
    case Mode::REPLACE: processType = ProcessType::REPLACE; break;
    default:                                                break;
  }

  //---

  // get column
  Column column;

#if 0
  auto columnStr = columnEdit_->text();

  if (! CQChartsModelUtil::stringToColumn(model.data(), columnStr, column)) {
    bool ok;
    auto icolumn = CQChartsUtil::toInt(columnStr, ok);

    if (ok)
      column = CQChartsColumn(icolumn);
  }
#else
  column = this->column();
#endif

  //---

  auto header = this->header();
  auto expr   = this->expr();
  auto var    = this->var();

  //---

  // set current column (for change notify)
  if (processType == ProcessType::MODIFY || processType == ProcessType::REPLACE) {
    disconnect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(modelColumnSlot(int)));

    modelData_->setCurrentColumn(column.column());

    connect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(modelColumnSlot(int)));
  }

  //---

  // process model
  CQChartsModelProcessData data;

  auto ecolumn = this->column();
  auto type    = this->type();

  CQUtil::AutoDisconnect currentColumnDisconnect(
    modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(modelColumnSlot(int)));

  if      (processType == ProcessType::ADD) {
    if (! expr.length()) {
      charts->errorMsg("Missing expression");
      return;
    }

    auto *addData = data.addData();

    addData->type          = CQChartsModelProcessData::AddData::Type::EXPR;
    addData->header        = header;
    addData->expr          = expr;
    addData->columnType    = type.toString();

    QStringList strs;
    (void) CQTclUtil::splitList(var, strs);

    for (const auto &str : strs) {
      QStringList strs1;
      (void) CQTclUtil::splitList(str, strs1);
      if (strs1.length() < 2) continue;

      addData->varNameValues[strs1[0]] = strs1[1];
    }

    auto errorType = CQChartsModelProcess::addColumn(model, *addData);
    if (errorType != CQChartsModelProcess::ErrorType::NONE) {
      charts->errorMsg("Add column failed");
      return;
    }

    ecolumn = addData->column;
  }
  else if (processType == ProcessType::DELETE) {
    auto *deleteData = data.deleteData();

    deleteData->column = column;

    if (! CQChartsModelProcess::deleteColumn(model, *deleteData)) {
      charts->errorMsg("Remove column failed");
      return;
    }
  }
  else if (processType == ProcessType::MODIFY) {
    auto *modifyData = data.modifyData();

    modifyData->column     = column;
    modifyData->header     = header;
    modifyData->expr       = expr;
    modifyData->columnType = type.toString();

    auto errorType = CQChartsModelProcess::modifyColumn(model, *modifyData);
    if (errorType != CQChartsModelProcess::ErrorType::NONE) {
      charts->errorMsg("Modify column failed");
      return;
    }
  }
  else if (processType == ProcessType::REPLACE) {
    auto *replaceData = data.replaceData();

    replaceData->column   = column;
    replaceData->oldValue = oldValueEdit_->text();
    replaceData->newValue = newValueEdit_->text();

    (void) CQChartsModelProcess::replaceColumn(charts, model, *replaceData);
  }
  else {
    charts->errorMsg("Invalid function");
    return;
  }

  if (ecolumn.column() != -1)
    modelData_->setCurrentColumn(ecolumn.column());
}
