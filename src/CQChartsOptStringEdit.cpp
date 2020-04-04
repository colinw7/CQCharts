#include <CQChartsOptStringEdit.h>

CQChartsOptStringEdit::
CQChartsOptStringEdit(QWidget *parent) :
 CQChartsOptEdit(parent, "String")
{
}

void
CQChartsOptStringEdit::
setValue(const CQChartsOptString &value)
{
  value_ = value;

  valueToWidgets();
}

void
CQChartsOptStringEdit::
setTextValue(const QString &str)
{
  CQChartsOptString value;

  value.fromString(str);

  if (value.isSet())
    value_ = value;
}

void
CQChartsOptStringEdit::
resetValue()
{
  value_ = CQChartsOptString();
}

bool
CQChartsOptStringEdit::
isValueSet() const
{
  return value_.isSet();
}

QString
CQChartsOptStringEdit::
valueToText()
{
  QString s = value_.string();

  return s;
}

QVariant
CQChartsOptStringEdit::
toVariant() const
{
  return QVariant::fromValue(value());
}

void
CQChartsOptStringEdit::
fromVariant(const QVariant &var)
{
  CQChartsOptString value = var.value<CQChartsOptString>();

  setValue(value);
}

//------

CQPropertyViewEditorFactory *
CQChartsOptStringPropertyViewType::
getEditor() const
{
  return new CQChartsOptStringPropertyViewEditor;
}

QString
CQChartsOptStringPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  CQChartsOptString optString = value.value<CQChartsOptString>();

  QString str;

  if (optString.isSet()) {
    str = optString.toString();
    ok  = true;
  }
  else {
    str = "";
    ok  = false;
  }

  return str;
}

//------

QWidget *
CQChartsOptStringPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsOptStringEdit(parent);
}
