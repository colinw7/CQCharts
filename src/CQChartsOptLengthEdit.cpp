#include <CQChartsOptLengthEdit.h>

CQChartsOptLengthEdit::
CQChartsOptLengthEdit(QWidget *parent) :
 CQChartsOptEdit(parent, "Length")
{
}

void
CQChartsOptLengthEdit::
setValue(const CQChartsOptLength &value)
{
  value_ = value;

  valueToWidgets();
}

void
CQChartsOptLengthEdit::
setTextValue(const QString &str)
{
  CQChartsOptLength value;

  value.fromString(str);

  if (value.isSet())
    value_ = value;
}

void
CQChartsOptLengthEdit::
resetValue()
{
  value_ = CQChartsOptLength();
}

bool
CQChartsOptLengthEdit::
isValueSet() const
{
  return value_.isSet();
}

QString
CQChartsOptLengthEdit::
valueToText()
{
  const auto &l = value_.length();

  return l.toString();
}

QVariant
CQChartsOptLengthEdit::
toVariant() const
{
  return CQChartsOptLength::toVariant(value());
}

void
CQChartsOptLengthEdit::
fromVariant(const QVariant &var)
{
  auto value = CQChartsOptLength::fromVariant(var);

  setValue(value);
}

//------

CQPropertyViewEditorFactory *
CQChartsOptLengthPropertyViewType::
getEditor() const
{
  return new CQChartsOptLengthPropertyViewEditor;
}

QString
CQChartsOptLengthPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto optLength = CQChartsOptLength::fromVariant(value);

  QString str;

  if (optLength.isSet()) {
    str = optLength.toString();
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
CQChartsOptLengthPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsOptLengthEdit(parent);
}
