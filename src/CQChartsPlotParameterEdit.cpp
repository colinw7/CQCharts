#include <CQChartsPlotParameterEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsVariant.h>

#include <CQRealSpin.h>
#include <CQIntegerSpin.h>
#include <CQCheckBox.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QCheckBox>

CQChartsColumnParameterEdit::
CQChartsColumnParameterEdit(const CQChartsPlotParameter *parameter, bool isBasic) :
 parameter_(parameter)
{
  auto objName = parameter->name() + "ColumnEdit";

  setObjectName(objName);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  auto column = CQChartsColumn::fromVariant(parameter->defValue());

  auto tip = parameter->tip();

  if (isBasic) {
    combo_ = CQUtil::makeWidget<CQChartsColumnCombo>("combo");

    if (column.isValid())
      combo_->setColumn(column);

    if (tip.length())
      combo_->setToolTip(tip);

    layout->addWidget(combo_);

    connect(combo_, SIGNAL(columnChanged()), this, SIGNAL(columnChanged()));
  }
  else {
    edit_ = CQUtil::makeWidget<CQChartsColumnLineEdit>("edit");

    edit_->setPlaceholderText("Column Name or Number");

    if (column.isValid())
      edit_->setColumn(column);

    if (tip.length())
      edit_->setToolTip(tip);

    layout->addWidget(edit_);

    connect(edit_, SIGNAL(columnChanged()), this, SIGNAL(columnChanged()));
  }
}

void
CQChartsColumnParameterEdit::
setModelData(const CQChartsModelData *modelData)
{
  if      (combo_)
    combo_->setModelData(modelData);
  else if (edit_)
    edit_->setModelData(modelData);
}

CQChartsColumn
CQChartsColumnParameterEdit::
getColumn() const
{
  CQChartsColumn column;

  if      (combo_)
    column = combo_->getColumn();
  else if (edit_)
    column = edit_->column();

  return column;
}

void
CQChartsColumnParameterEdit::
setColumn(const CQChartsColumn &column)
{
  if      (combo_)
    combo_->setColumn(column);
  else if (edit_)
    edit_->setColumn(column);
}

bool
CQChartsColumnParameterEdit::
isNumericOnly() const
{
  if      (combo_)
    return combo_->isNumericOnly();
  else if (edit_)
    return edit_->isNumericOnly();
  else
    return false;
}

void
CQChartsColumnParameterEdit::
setNumericOnly(bool b)
{
  if      (combo_)
    combo_->setNumericOnly(b);
  else if (edit_)
    edit_->setNumericOnly(b);
}

//---

CQChartsColumnsParameterEdit::
CQChartsColumnsParameterEdit(const CQChartsPlotParameter *parameter, bool isBasic) :
 CQChartsColumnsLineEdit(nullptr, isBasic), parameter_(parameter)
{
  setObjectName(parameter->name() + "ColumnsEdit");

  setPlaceholderText("Column Names or Numbers");

  auto columns = CQChartsColumns::fromVariant(parameter->defValue());

  if (columns.isValid())
    setColumns(columns);

  auto tip = parameter->tip();

  if (tip.length())
    setToolTip(tip);
}

//---

CQChartsStringParameterEdit::
CQChartsStringParameterEdit(const CQChartsPlotParameter *parameter) :
 parameter_(parameter)
{
  setObjectName(parameter->name() + "String");

  bool ok;

  auto str = CQChartsVariant::toString(parameter->defValue(), ok);

  setText(str);

  auto tip = parameter->tip();

  if (tip.length())
    setToolTip(tip);
}

//---

CQChartsRealParameterEdit::
CQChartsRealParameterEdit(const CQChartsPlotParameter *parameter) :
 parameter_(parameter)
{
  auto objName = parameter->name() + "Real";

  setObjectName(objName);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  bool ok;

  double r = CQChartsVariant::toReal(parameter->defValue(), ok);

  auto tip = parameter->tip();

  if (parameter->isRequired()) {
    spinEdit_ = CQUtil::makeWidget<CQRealSpin>("spinEdit");

    spinEdit_->setValue(r);

    connect(spinEdit_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged()));

    if (tip.length())
      spinEdit_->setToolTip(tip);

    layout->addWidget(spinEdit_);
  }
  else {
    lineEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

    lineEdit_->setText(QString::number(r));

    connect(lineEdit_, SIGNAL(textChanged(const QString &)), this, SIGNAL(valueChanged()));

    if (tip.length())
      lineEdit_->setToolTip(tip);

    layout->addWidget(lineEdit_);
  }
}

double
CQChartsRealParameterEdit::
value() const
{
  double r { 0.0 };

  if      (spinEdit_)
    r = spinEdit_->value();
  else if (lineEdit_) {
    bool ok;

    r = CQChartsUtil::toReal(lineEdit_->text(), ok);
  }

  return r;
}

//---

CQChartsIntParameterEdit::
CQChartsIntParameterEdit(const CQChartsPlotParameter *parameter) :
 parameter_(parameter)
{
  auto objName = parameter->name() + "Real";

  setObjectName(objName);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  bool ok;

  long i = CQChartsVariant::toInt(parameter->defValue(), ok);

  auto tip = parameter->tip();

  if (parameter->isRequired()) {
    spinEdit_ = CQUtil::makeWidget<CQIntegerSpin>("spinEdit");

    spinEdit_->setValue(i);

    connect(spinEdit_, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged()));

    if (tip.length())
      spinEdit_->setToolTip(tip);

    layout->addWidget(spinEdit_);
  }
  else {
    lineEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

    lineEdit_->setText(QString::number(i));

    connect(lineEdit_, SIGNAL(textChanged(const QString &)), this, SIGNAL(valueChanged()));

    if (tip.length())
      lineEdit_->setToolTip(tip);

    layout->addWidget(lineEdit_);
  }
}

int
CQChartsIntParameterEdit::
value() const
{
  long i { 0 };

  if      (spinEdit_)
    i = spinEdit_->value();
  else if (lineEdit_) {
    bool ok;

    i = CQChartsUtil::toInt(lineEdit_->text(), ok);
  }

  return static_cast<int>(i);
}

//---

CQChartsEnumParameterEdit::
CQChartsEnumParameterEdit(const CQChartsEnumParameter *parameter) :
 parameter_(parameter)
{
  setObjectName(parameter_->name() + "Enum");

  addItems(parameter_->names());

  bool ok;

  long i = CQChartsVariant::toInt(parameter_->defValue(), ok);

  setCurrentValue(int(i));

  auto tip = parameter_->tip();

  if (tip.length())
    setToolTip(tip);
}

void
CQChartsEnumParameterEdit::
setCurrentValue(int i)
{
  setCurrentIndex(findText(parameter_->valueName(i)));
}

int
CQChartsEnumParameterEdit::
currentValue() const
{
  return parameter_->nameValue(currentText());
}

//---

CQChartsBoolParameterEdit::
CQChartsBoolParameterEdit(const CQChartsPlotParameter *parameter, bool choice) :
 parameter_(parameter)
{
  setObjectName(parameter_->name() + "Bool");

  bool ok;
  bool b = CQChartsVariant::toBool(parameter->defValue(), ok);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  if (choice) {
    choice_ = CQUtil::makeWidget<CQCheckBox>("choice");

    connect(choice_, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

    layout->addWidget(choice_);
  }
  else {
    check_ = CQUtil::makeWidget<QCheckBox>("check");

    connect(check_, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

    check_->setText(parameter->desc());

    layout->addWidget(check_);
  }

  setChecked(b);

  auto tip = parameter->tip();

  if (tip.length()) {
    if (choice_)
      choice_->setToolTip(tip);
    else
      check_->setToolTip(tip);
  }
}

bool
CQChartsBoolParameterEdit::
isChecked() const
{
  if (choice_)
    return choice_->isChecked();
  else
    return check_->isChecked();
}

void
CQChartsBoolParameterEdit::
setChecked(bool b)
{
  if (choice_)
    choice_->setChecked(b);
  else
    check_->setChecked(b);
}
