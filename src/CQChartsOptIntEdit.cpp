#include <CQChartsOptIntEdit.h>

CQChartsOptIntEdit::
CQChartsOptIntEdit(QWidget *parent) :
 CQChartsOptEdit(parent, "Integer")
{
}

void
CQChartsOptIntEdit::
setValue(const CQChartsOptInt &value)
{
  value_ = value;

  valueToWidgets();
}

void
CQChartsOptIntEdit::
setTextValue(const QString &str)
{
  CQChartsOptInt value;

  value.fromString(str);

  if (value.isSet())
    value_ = value;
}

void
CQChartsOptIntEdit::
resetValue()
{
  value_ = CQChartsOptInt();
}

bool
CQChartsOptIntEdit::
isValueSet() const
{
  return value_.isSet();
}

QString
CQChartsOptIntEdit::
valueToText()
{
  int i = value_.integer();

  return QString::number(i);
}

QVariant
CQChartsOptIntEdit::
toVariant() const
{
  return CQChartsOptInt::toVariant(value());
}

void
CQChartsOptIntEdit::
fromVariant(const QVariant &var)
{
  auto value = CQChartsOptInt::fromVariant(var);

  setValue(value);
}

//------

CQPropertyViewEditorFactory *
CQChartsOptIntPropertyViewType::
getEditor() const
{
  return new CQChartsOptIntPropertyViewEditor;
}

QString
CQChartsOptIntPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto optInt = CQChartsOptInt::fromVariant(value);

  QString str;

  if (optInt.isSet()) {
    str = optInt.toString();
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
CQChartsOptIntPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsOptIntEdit(parent);
}
