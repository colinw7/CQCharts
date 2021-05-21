#include <CQChartsOptRealEdit.h>

CQChartsOptRealEdit::
CQChartsOptRealEdit(QWidget *parent) :
 CQChartsOptEdit(parent, "Real")
{
}

void
CQChartsOptRealEdit::
setValue(const CQChartsOptReal &value)
{
  value_ = value;

  valueToWidgets();
}

void
CQChartsOptRealEdit::
setTextValue(const QString &str)
{
  CQChartsOptReal value;

  value.fromString(str);

  if (value.isSet())
    value_ = value;
}

void
CQChartsOptRealEdit::
resetValue()
{
  value_ = CQChartsOptReal();
}

bool
CQChartsOptRealEdit::
isValueSet() const
{
  return value_.isSet();
}

QString
CQChartsOptRealEdit::
valueToText()
{
  double r = value_.real();

  return QString::number(r);
}

QVariant
CQChartsOptRealEdit::
toVariant() const
{
  return QVariant::fromValue(value());
}

void
CQChartsOptRealEdit::
fromVariant(const QVariant &var)
{
  auto value = var.value<CQChartsOptReal>();

  setValue(value);
}

//------

CQPropertyViewEditorFactory *
CQChartsOptRealPropertyViewType::
getEditor() const
{
  return new CQChartsOptRealPropertyViewEditor;
}

QString
CQChartsOptRealPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto optReal = value.value<CQChartsOptReal>();

  QString str;

  if (optReal.isSet()) {
    str = optReal.toString();
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
CQChartsOptRealPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsOptRealEdit(parent);
}
