#include <CQPropertyViewIntegerType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <QSpinBox>
#include <cassert>

CQPropertyViewIntegerType::
CQPropertyViewIntegerType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewIntegerType::
getEditor() const
{
  return new CQPropertyViewIntegerEditor;
}

bool
CQPropertyViewIntegerType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewIntegerType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  delegate->drawString(painter, option, value.toString(), index, inside);
}

//------

CQPropertyViewIntegerEditor::
CQPropertyViewIntegerEditor(int min, int max, int step) :
 min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyViewIntegerEditor::
createEdit(QWidget *parent)
{
  QSpinBox *spin = new QSpinBox(parent);

  spin->setRange(min_, max_);
  spin->setSingleStep(step_);

  return spin;
}

void
CQPropertyViewIntegerEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  QObject::connect(spin, SIGNAL(valueChanged(int)), obj, method);
}

QVariant
CQPropertyViewIntegerEditor::
getValue(QWidget *w)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  return QVariant(spin->value());
}

void
CQPropertyViewIntegerEditor::
setValue(QWidget *w, const QVariant &var)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  int i = var.toInt();

  spin->setValue(i);
}
