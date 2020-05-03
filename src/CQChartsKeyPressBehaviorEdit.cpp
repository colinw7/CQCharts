#include <CQChartsKeyPressBehaviorEdit.h>

CQChartsKeyPressBehaviorEdit::
CQChartsKeyPressBehaviorEdit(QWidget *parent) :
 CQChartsEnumEdit(parent)
{
  setObjectName("keyPressBehavior");

  setToolTip("Key Press Behavior");

  //---

  init();

  QObject::connect(this, SIGNAL(enumChanged()), this, SIGNAL(keyPressBehaviorChanged()));
}

void
CQChartsKeyPressBehaviorEdit::
setKeyPressBehavior(const CQChartsKeyPressBehavior &keyPressBehavior)
{
  keyPressBehavior_ = keyPressBehavior;

  setEnumString(keyPressBehavior_.toString());
}

void
CQChartsKeyPressBehaviorEdit::
setEnumFromString(const QString &str)
{
  keyPressBehavior_ = CQChartsKeyPressBehavior(str);
}

QVariant
CQChartsKeyPressBehaviorEdit::
getVariantFromEnum() const
{
  return QVariant::fromValue(keyPressBehavior());
}

void
CQChartsKeyPressBehaviorEdit::
setEnumFromVariant(const QVariant &var)
{
  auto keyPressBehavior = var.value<CQChartsKeyPressBehavior>();

  setKeyPressBehavior(keyPressBehavior);
}

QString
CQChartsKeyPressBehaviorEdit::
variantToString(const QVariant &var) const
{
  auto keyPressBehavior = var.value<CQChartsKeyPressBehavior>();

  return keyPressBehavior.toString();
}

void
CQChartsKeyPressBehaviorEdit::
connect(QObject *obj, const char *method)
{
  QObject::connect(this, SIGNAL(keyPressBehaviorChanged()), obj, method);
}

//------

CQPropertyViewEditorFactory *
CQChartsKeyPressBehaviorPropertyViewType::
getEditor() const
{
  return new CQChartsKeyPressBehaviorPropertyViewEditorFactory;
}

QString
CQChartsKeyPressBehaviorPropertyViewType::
variantToString(const QVariant &var) const
{
  auto keyPressBehavior = var.value<CQChartsKeyPressBehavior>();

  return keyPressBehavior.toString();
}

//------

QWidget *
CQChartsKeyPressBehaviorPropertyViewEditorFactory::
createEdit(QWidget *parent)
{
  return new CQChartsKeyPressBehaviorEdit(parent);
}
