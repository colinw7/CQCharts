#include <CQPropertyViewAngleType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQAngleSpinBox.h>

CQPropertyViewAngleType::
CQPropertyViewAngleType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewAngleType::
getEditor() const
{
  return new CQPropertyViewAngleEditor;
}

bool
CQPropertyViewAngleType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewAngleType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  QString str = value.value<CAngle>().toString().c_str();

  str += "\u00b0";

  delegate->drawString(painter, option, str, index, inside);
}

QString
CQPropertyViewAngleType::
tip(const QVariant &value) const
{
  QString str = value.value<CAngle>().toString().c_str();

  str += "\u00b0";

  return str;
}

//------

CQPropertyViewAngleEditor::
CQPropertyViewAngleEditor()
{
}

QWidget *
CQPropertyViewAngleEditor::
createEdit(QWidget *parent)
{
  CQAngleSpinBox *edit = new CQAngleSpinBox(parent);

  return edit;
}

void
CQPropertyViewAngleEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(angleChanged(const CAngle &)), obj, method);
}

QVariant
CQPropertyViewAngleEditor::
getValue(QWidget *w)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  return QVariant::fromValue(edit->getAngle());
}

void
CQPropertyViewAngleEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  CAngle angle = var.value<CAngle>();

  edit->setAngle(angle);
}
