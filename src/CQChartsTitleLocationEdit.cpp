#include <CQChartsTitleLocationEdit.h>

CQChartsTitleLocationEdit::
CQChartsTitleLocationEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("titleLocation");

  setToolTip("Title Location");

  //---

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(titleLocationChanged()));
}

void
CQChartsTitleLocationEdit::
setTitleLocation(const CQChartsTitleLocation &titleLocation)
{
  titleLocation_ = titleLocation;

  setEnumString(titleLocation_.toString());
}

void
CQChartsTitleLocationEdit::
setEnumFromString(const QString &str)
{
  titleLocation_ = CQChartsTitleLocation(str);
}

QVariant
CQChartsTitleLocationEdit::
getVariantFromEnum() const
{
  return QVariant::fromValue(titleLocation());
}

void
CQChartsTitleLocationEdit::
setEnumFromVariant(const QVariant &var)
{
  auto titleLocation = var.value<CQChartsTitleLocation>();

  setTitleLocation(titleLocation);
}

QString
CQChartsTitleLocationEdit::
variantToString(const QVariant &var) const
{
  auto titleLocation = var.value<CQChartsTitleLocation>();

  return titleLocation.toString();
}

void
CQChartsTitleLocationEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(titleLocationChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsTitleLocationPropertyViewType::
getEditor() const
{
  return new CQChartsTitleLocationPropertyViewEditorFactory;
}

QString
CQChartsTitleLocationPropertyViewType::
variantToString(const QVariant &var) const
{
  auto titleLocation = var.value<CQChartsTitleLocation>();

  return titleLocation.toString();
}

//------

QWidget *
CQChartsTitleLocationPropertyViewEditorFactory::
createEdit(QWidget *parent)
{
  return new CQChartsTitleLocationEdit(parent);
}
