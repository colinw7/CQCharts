#include <CQChartsAxisTickLabelPlacementEdit.h>

CQChartsAxisTickLabelPlacementEdit::
CQChartsAxisTickLabelPlacementEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("axisTickLabelPlacement");

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(axisTickLabelPlacementChanged()));
}

void
CQChartsAxisTickLabelPlacementEdit::
setAxisTickLabelPlacement(const CQChartsAxisTickLabelPlacement &axisTickLabelPlacement)
{
  axisTickLabelPlacement_ = axisTickLabelPlacement;

  setEnumString(axisTickLabelPlacement_.toString());
}

void
CQChartsAxisTickLabelPlacementEdit::
setEnumFromString(const QString &str)
{
  axisTickLabelPlacement_ = CQChartsAxisTickLabelPlacement(str);
}

QVariant
CQChartsAxisTickLabelPlacementEdit::
getVariantFromEnum() const
{
  return QVariant::fromValue(axisTickLabelPlacement());
}

void
CQChartsAxisTickLabelPlacementEdit::
setEnumFromVariant(const QVariant &var)
{
  CQChartsAxisTickLabelPlacement axisTickLabelPlacement =
    var.value<CQChartsAxisTickLabelPlacement>();

  setAxisTickLabelPlacement(axisTickLabelPlacement);
}

QString
CQChartsAxisTickLabelPlacementEdit::
variantToString(const QVariant &var) const
{
  CQChartsAxisTickLabelPlacement axisTickLabelPlacement =
    var.value<CQChartsAxisTickLabelPlacement>();

  return axisTickLabelPlacement.toString();
}

void
CQChartsAxisTickLabelPlacementEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(axisTickLabelPlacementChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsAxisTickLabelPlacementPropertyViewType::
getEditor() const
{
  return new CQChartsAxisTickLabelPlacementPropertyViewEditorFactory;
}

QString
CQChartsAxisTickLabelPlacementPropertyViewType::
variantToString(const QVariant &var) const
{
  CQChartsAxisTickLabelPlacement axisTickLabelPlacement =
    var.value<CQChartsAxisTickLabelPlacement>();

  return axisTickLabelPlacement.toString();
}

//------

QWidget *
CQChartsAxisTickLabelPlacementPropertyViewEditorFactory::
createEdit(QWidget *parent)
{
  return new CQChartsAxisTickLabelPlacementEdit(parent);
}
