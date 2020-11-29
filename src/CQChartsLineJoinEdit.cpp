#include <CQChartsLineJoinEdit.h>

CQChartsLineJoinEdit::
CQChartsLineJoinEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("lineJoin");

  setToolTip("Line Join");

  //---

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(lineJoinChanged()));
}

void
CQChartsLineJoinEdit::
setLineJoin(const CQChartsLineJoin &lineJoin)
{
  lineJoin_ = lineJoin;

  setEnumString(lineJoin_.toString());
}

void
CQChartsLineJoinEdit::
setEnumFromString(const QString &str)
{
  lineJoin_ = CQChartsLineJoin(str);
}

QVariant
CQChartsLineJoinEdit::
getVariantFromEnum() const
{
  return QVariant::fromValue(lineJoin());
}

void
CQChartsLineJoinEdit::
setEnumFromVariant(const QVariant &var)
{
  auto lineJoin = var.value<CQChartsLineJoin>();

  setLineJoin(lineJoin);
}

QString
CQChartsLineJoinEdit::
variantToString(const QVariant &var) const
{
  auto lineJoin = var.value<CQChartsLineJoin>();

  return lineJoin.toString();
}

void
CQChartsLineJoinEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(lineJoinChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsLineJoinPropertyViewType::
getEditor() const
{
  return new CQChartsLineJoinPropertyViewEditor;
}

QString
CQChartsLineJoinPropertyViewType::
variantToString(const QVariant &var) const
{
  auto lineJoin = var.value<CQChartsLineJoin>();

  return lineJoin.toString();
}

//------

QWidget *
CQChartsLineJoinPropertyViewEditor::
createEdit(QWidget *parent)
{
  return new CQChartsLineJoinEdit(parent);
}
