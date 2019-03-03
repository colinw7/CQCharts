#include <CQChartsKeyLocationEdit.h>

CQChartsKeyLocationEdit::
CQChartsKeyLocationEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("keyLocation");

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(keyLocationChanged()));
}

void
CQChartsKeyLocationEdit::
setKeyLocation(const CQChartsKeyLocation &keyLocation)
{
  keyLocation_ = keyLocation;

  setEnumString(keyLocation_.toString());
}

void
CQChartsKeyLocationEdit::
setEnumFromString(const QString &str)
{
  keyLocation_ = CQChartsKeyLocation(str);
}

QVariant
CQChartsKeyLocationEdit::
getVariantFromEnum() const
{
  return QVariant::fromValue(keyLocation());
}

void
CQChartsKeyLocationEdit::
setEnumFromVariant(const QVariant &var)
{
  CQChartsKeyLocation keyLocation = var.value<CQChartsKeyLocation>();

  setKeyLocation(keyLocation);
}

QString
CQChartsKeyLocationEdit::
variantToString(const QVariant &var) const
{
  CQChartsKeyLocation keyLocation = var.value<CQChartsKeyLocation>();

  return keyLocation.toString();
}

void
CQChartsKeyLocationEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(keyLocationChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsKeyLocationPropertyViewType::
getEditor() const
{
  return new CQChartsKeyLocationPropertyViewEditor;
}

QString
CQChartsKeyLocationPropertyViewType::
variantToString(const QVariant &var) const
{
  CQChartsKeyLocation keyLocation = var.value<CQChartsKeyLocation>();

  return keyLocation.toString();
}

//------

QWidget *
CQChartsKeyLocationPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsKeyLocationEdit(parent);
}
