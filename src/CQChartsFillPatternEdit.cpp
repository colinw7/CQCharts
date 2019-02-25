#include <CQChartsFillPatternEdit.h>

CQChartsFillPatternEdit::
CQChartsFillPatternEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("fillPattern");

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(fillPatternChanged()));
}

void
CQChartsFillPatternEdit::
setFillPattern(const CQChartsFillPattern &fillPattern)
{
  fillPattern_ = fillPattern;

  setEnumString(fillPattern_.toString());
}

void
CQChartsFillPatternEdit::
setEnumFromString(const QString &str)
{
  fillPattern_ = CQChartsFillPattern(str);
}

QVariant
CQChartsFillPatternEdit::
getVariantFromEnum() const
{
  return QVariant::fromValue(fillPattern());
}

void
CQChartsFillPatternEdit::
setEnumFromVariant(const QVariant &var)
{
  CQChartsFillPattern fillPatten = var.value<CQChartsFillPattern>();

  setFillPattern(fillPatten);
}

QString
CQChartsFillPatternEdit::
variantToString(const QVariant &var) const
{
  CQChartsFillPattern fillPatten = var.value<CQChartsFillPattern>();

  return fillPatten.toString();
}

void
CQChartsFillPatternEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(fillPattenChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsFillPatternPropertyViewType::
getEditor() const
{
  return new CQChartsFillPatternPropertyViewEditorFactory;
}

QString
CQChartsFillPatternPropertyViewType::
variantToString(const QVariant &var) const
{
  CQChartsFillPattern fillPattern = var.value<CQChartsFillPattern>();

  return fillPattern.toString();
}

//------

QWidget *
CQChartsFillPatternPropertyViewEditorFactory::
createEdit(QWidget *parent)
{
  return new CQChartsFillPatternEdit(parent);
}
