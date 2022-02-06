#include <CQChartsOptBoolEdit.h>

CQChartsOptBoolEdit::
CQChartsOptBoolEdit(QWidget *parent) :
 CQChartsOptEdit(parent, "Boolean")
{
}

void
CQChartsOptBoolEdit::
setValue(const CQChartsOptBool &value)
{
  value_ = value;

  valueToWidgets();
}

void
CQChartsOptBoolEdit::
setTextValue(const QString &str)
{
  CQChartsOptBool value;

  value.fromString(str);

  if (value.isSet())
    value_ = value;
}

void
CQChartsOptBoolEdit::
resetValue()
{
  value_ = CQChartsOptBool();
}

bool
CQChartsOptBoolEdit::
isValueSet() const
{
  return value_.isSet();
}

QString
CQChartsOptBoolEdit::
valueToText()
{
  bool b = value_.isBool();

  return QString::number(b);
}

QVariant
CQChartsOptBoolEdit::
toVariant() const
{
  return CQChartsOptBool::toVariant(value());
}

void
CQChartsOptBoolEdit::
fromVariant(const QVariant &var)
{
  auto value = CQChartsOptBool::fromVariant(var);

  setValue(value);
}

//------

CQPropertyViewEditorFactory *
CQChartsOptBoolPropertyViewType::
getEditor() const
{
  return new CQChartsOptBoolPropertyViewEditor;
}

QString
CQChartsOptBoolPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto optBool = CQChartsOptBool::fromVariant(value);

  QString str;

  if (optBool.isSet()) {
    str = optBool.toString();
    ok  = true;
  }
  else {
    str = "Undefined";
    ok  = false;
  }

  return str;
}

//------

QWidget *
CQChartsOptBoolPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsOptBoolEdit(parent);
}
