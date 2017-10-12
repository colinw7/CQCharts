#include <CQPropertyViewAlignType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQAlignEdit.h>

CQPropertyViewAlignType::
CQPropertyViewAlignType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewAlignType::
getEditor() const
{
  return new CQPropertyViewAlignEditor;
}

bool
CQPropertyViewAlignType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewAlignType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  QString str = CQAlignEdit::toString((Qt::Alignment) value.toInt());

  delegate->drawString(painter, option, str, index, inside);
}

QString
CQPropertyViewAlignType::
tip(const QVariant &value) const
{
  return CQAlignEdit::toString((Qt::Alignment) value.toInt());
}

//------

CQPropertyViewAlignEditor::
CQPropertyViewAlignEditor()
{
}

QWidget *
CQPropertyViewAlignEditor::
createEdit(QWidget *parent)
{
  CQAlignEdit *edit = new CQAlignEdit(parent);

  return edit;
}

void
CQPropertyViewAlignEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQAlignEdit *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged(Qt::Alignment)), obj, method);
}

QVariant
CQPropertyViewAlignEditor::
getValue(QWidget *w)
{
  CQAlignEdit *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  return QVariant(edit->align());
}

void
CQPropertyViewAlignEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQAlignEdit *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  int i = var.toInt();

  edit->setAlign((Qt::Alignment) i);
}
