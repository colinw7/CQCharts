#include <CQChartsAxisSideEdit.h>

CQChartsAxisSideEdit::
CQChartsAxisSideEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("axisSide");

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(axisSideChanged()));
}

void
CQChartsAxisSideEdit::
setAxisSide(const CQChartsAxisSide &axisSide)
{
  axisSide_ = axisSide;

  setEnumString(axisSide_.toString());
}

void
CQChartsAxisSideEdit::
setEnumFromString(const QString &str)
{
  axisSide_ = CQChartsAxisSide(str);
}

QVariant
CQChartsAxisSideEdit::
getVariantFromEnum() const
{
  return QVariant::fromValue(axisSide());
}

void
CQChartsAxisSideEdit::
setEnumFromVariant(const QVariant &var)
{
  CQChartsAxisSide axisSide = var.value<CQChartsAxisSide>();

  setAxisSide(axisSide);
}

QString
CQChartsAxisSideEdit::
variantToString(const QVariant &var) const
{
  CQChartsAxisSide axisSide = var.value<CQChartsAxisSide>();

  return axisSide.toString();
}

void
CQChartsAxisSideEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(axisSideChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsAxisSidePropertyViewType::
getEditor() const
{
  return new CQChartsAxisSidePropertyViewEditorFactory;
}

QString
CQChartsAxisSidePropertyViewType::
variantToString(const QVariant &var) const
{
  CQChartsAxisSide axisSide = var.value<CQChartsAxisSide>();

  return axisSide.toString();
}

//------

QWidget *
CQChartsAxisSidePropertyViewEditorFactory::
createEdit(QWidget *parent)
{
  return new CQChartsAxisSideEdit(parent);
}
