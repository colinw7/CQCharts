#include <CQChartsLineCapEdit.h>

CQChartsLineCapEdit::
CQChartsLineCapEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("lineCap");

  setToolTip("Line Cap");

  //---

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(lineCapChanged()));
}

void
CQChartsLineCapEdit::
setLineCap(const CQChartsLineCap &lineCap)
{
  lineCap_ = lineCap;

  setEnumString(lineCap_.toString());
}

void
CQChartsLineCapEdit::
setEnumFromString(const QString &str)
{
  lineCap_ = CQChartsLineCap(str);
}

QVariant
CQChartsLineCapEdit::
getVariantFromEnum() const
{
  return CQChartsLineCap::toVariant(lineCap());
}

void
CQChartsLineCapEdit::
setEnumFromVariant(const QVariant &var)
{
  auto lineCap = CQChartsLineCap::fromVariant(var);

  setLineCap(lineCap);
}

QString
CQChartsLineCapEdit::
variantToString(const QVariant &var) const
{
  auto lineCap = CQChartsLineCap::fromVariant(var);

  return lineCap.toString();
}

void
CQChartsLineCapEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(lineCapChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsLineCapPropertyViewType::
getEditor() const
{
  return new CQChartsLineCapPropertyViewEditor;
}

QString
CQChartsLineCapPropertyViewType::
variantToString(const QVariant &var) const
{
  auto lineCap = CQChartsLineCap::fromVariant(var);

  return lineCap.toString();
}

//------

QWidget *
CQChartsLineCapPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsLineCapEdit(parent);
}
