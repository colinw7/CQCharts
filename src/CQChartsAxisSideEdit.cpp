#include <CQChartsAxisSideEdit.h>

CQChartsAxisSideEdit::
CQChartsAxisSideEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("axisSide");

  setToolTip("Axis Side");

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
  return CQChartsAxisSide::toVariant(axisSide());
}

void
CQChartsAxisSideEdit::
setEnumFromVariant(const QVariant &var)
{
  auto axisSide = CQChartsAxisSide::fromVariant(var);

  setAxisSide(axisSide);
}

QString
CQChartsAxisSideEdit::
variantToString(const QVariant &var) const
{
  auto axisSide = CQChartsAxisSide::fromVariant(var);

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
  auto axisSide = CQChartsAxisSide::fromVariant(var);

  return axisSide.toString();
}

//------

QWidget *
CQChartsAxisSidePropertyViewEditorFactory::
createEdit(QWidget *parent)
{
  return new CQChartsAxisSideEdit(parent);
}
