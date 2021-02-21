#include <CQChartsPlotParameter.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsVariant.h>
#include <CQBaseModel.h>
#include <CQUtil.h>

#include <CQRealSpin.h>
#include <CQIntegerSpin.h>

#include <QHBoxLayout>

CQChartsPlotParameter::
CQChartsPlotParameter(const QString &name, const QString &desc, const Type &type,
                      const QString &propName, const Attributes &attributes,
                      const QVariant &defValue) :
 name_(name), desc_(desc), type_(type), propName_(propName), attributes_(attributes),
 defValue_(defValue)
{
  addProperty("name"         , "name"           , "");
  addProperty("desc"         , "desc"           , "");
  addProperty("type"         , "type"           , "");
  addProperty("prop_name"    , "propName"       , "");
  addProperty("group_id"     , "groupId"        , "");
  addProperty("def_value"    , "defValue"       , "");
  addProperty("tip"          , "tip"            , "");
  addProperty("column"       , "isColumn"       , "");
  addProperty("hidden"       , "isHidden"       , "");
  addProperty("multiple"     , "isMultiple"     , "");
  addProperty("optional"     , "isOptional"     , "");
  addProperty("required"     , "isRequired"     , "");
  addProperty("discriminator", "isDiscriminator", "");
  addProperty("monotonic"    , "isMonotonic"    , "");
  addProperty("numeric"      , "isNumeric"      , "");
  addProperty("string"       , "isString"       , "");
  addProperty("bool"         , "isBool"         , "");
  addProperty("color"        , "isColor"        , "");
  addProperty("type_detail"  , "typeDetail"     , "");
  addProperty("groupable"    , "isGroupable"    , "");
  addProperty("mapped"       , "isMapped"       , "");
  addProperty("mapMin"       , "mapMin"         , "");
  addProperty("mapMax"       , "mapMax"         , "");
}

QString
CQChartsPlotParameter::
typeName() const
{
  return CQBaseModel::typeName(type_);
}

CQChartsPlotParameter &
CQChartsPlotParameter::
setTypeName(const QString &name)
{
  type_ = CQBaseModel::nameType(name);

  return *this;
}

void
CQChartsPlotParameter::
addProperty(const QString &name, const QString &propName, const QString &desc)
{
  properties_[name] = PropertyData(name, propName, desc);
}

void
CQChartsPlotParameter::
propertyNames(QStringList &names) const
{
  for (const auto &p : properties_)
    names.push_back(p.first);
}

bool
CQChartsPlotParameter::
hasProperty(const QString &name) const
{
  auto p = properties_.find(name);

  return (p != properties_.end());
}

QVariant
CQChartsPlotParameter::
getPropertyValue(const QString &name) const
{
  auto p = properties_.find(name);

  if (p == properties_.end())
    return QVariant();

  QVariant var;

  if (! CQUtil::getProperty(this, (*p).second.propName, var))
    return QVariant();

  return var;
}

//---

CQChartsColumnParameterEdit::
CQChartsColumnParameterEdit(const CQChartsPlotParameter *parameter, bool isBasic) :
 parameter_(parameter)
{
  auto objName = parameter->name() + "Column";

  setObjectName(objName);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  auto column = parameter->defValue().value<CQChartsColumn>();

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

    lineEdit_->setText(QString("%1").arg(r));

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

    lineEdit_->setText(QString("%1").arg(i));

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
  int i { 0 };

  if      (spinEdit_)
    i = spinEdit_->value();
  else if (lineEdit_) {
    bool ok;

    i = (int) CQChartsUtil::toInt(lineEdit_->text(), ok);
  }

  return i;
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
CQChartsBoolParameterEdit(const CQChartsPlotParameter *parameter) :
 parameter_(parameter)
{
  setObjectName(parameter_->name() + "Bool");

  bool b = parameter->defValue().toBool();

  setText(parameter->desc());

  setChecked(b);

  auto tip = parameter->tip();

  if (tip.length())
    setToolTip(tip);
}
