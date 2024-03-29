#include <CQChartsOptPositionEdit.h>

CQChartsOptPositionEdit::
CQChartsOptPositionEdit(QWidget *parent) :
 CQChartsOptEdit(parent, "Position")
{
}

void
CQChartsOptPositionEdit::
setValue(const CQChartsOptPosition &value)
{
  value_ = value;

  valueToWidgets();
}

void
CQChartsOptPositionEdit::
setTextValue(const QString &str)
{
  CQChartsOptPosition value;

  value.fromString(str);

  if (value.isSet())
    value_ = value;
}

void
CQChartsOptPositionEdit::
resetValue()
{
  value_ = CQChartsOptPosition();
}

bool
CQChartsOptPositionEdit::
isValueSet() const
{
  return value_.isSet();
}

QString
CQChartsOptPositionEdit::
valueToText()
{
  const auto &l = value_.position();

  return l.toString();
}

QVariant
CQChartsOptPositionEdit::
toVariant() const
{
  return CQChartsOptPosition::toVariant(value());
}

void
CQChartsOptPositionEdit::
fromVariant(const QVariant &var)
{
  auto value = CQChartsOptPosition::fromVariant(var);

  setValue(value);
}

//------

CQPropertyViewEditorFactory *
CQChartsOptPositionPropertyViewType::
getEditor() const
{
  return new CQChartsOptPositionPropertyViewEditor;
}

QString
CQChartsOptPositionPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto optPosition = CQChartsOptPosition::fromVariant(value);

  QString str;

  if (optPosition.isSet()) {
    str = optPosition.toString();
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
CQChartsOptPositionPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsOptPositionEdit(parent);
}
