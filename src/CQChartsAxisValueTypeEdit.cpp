#include <CQChartsAxisValueTypeEdit.h>

CQChartsAxisValueTypeEdit::
CQChartsAxisValueTypeEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("axisValueType");

  setToolTip("Axis Value Type");

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(valueChanged()));
}

void
CQChartsAxisValueTypeEdit::
setAxisValueType(const CQChartsAxisValueType &value)
{
  value_ = value;

  setEnumString(value_.toString());
}

void
CQChartsAxisValueTypeEdit::
setEnumFromString(const QString &str)
{
  value_ = CQChartsAxisValueType(str);
}

QVariant
CQChartsAxisValueTypeEdit::
getVariantFromEnum() const
{
  return CQChartsAxisValueType::toVariant(axisValueType());
}

void
CQChartsAxisValueTypeEdit::
setEnumFromVariant(const QVariant &var)
{
  auto value = CQChartsAxisValueType::fromVariant(var);

  setAxisValueType(value);
}

QString
CQChartsAxisValueTypeEdit::
variantToString(const QVariant &var) const
{
  auto value = CQChartsAxisValueType::fromVariant(var);

  return value.toString();
}

void
CQChartsAxisValueTypeEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(valueChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsAxisValueTypePropertyViewType::
getEditor() const
{
  return new CQChartsAxisValueTypePropertyViewEditor;
}

QString
CQChartsAxisValueTypePropertyViewType::
variantToString(const QVariant &var) const
{
  auto value = CQChartsAxisValueType::fromVariant(var);

  return value.toString();
}

//------

QWidget *
CQChartsAxisValueTypePropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsAxisValueTypeEdit(parent);
}
