#include <CQPropertyViewColorType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQColorChooser.h>
#include <cassert>

CQPropertyViewColorType::
CQPropertyViewColorType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewColorType::
getEditor() const
{
  return new CQPropertyViewColorEditor;
}

bool
CQPropertyViewColorType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewColorType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  delegate->drawColor(painter, option, value.value<QColor>(), index, inside);
}

//------

CQPropertyViewColorEditor::
CQPropertyViewColorEditor()
{
}

QWidget *
CQPropertyViewColorEditor::
createEdit(QWidget *parent)
{
  CQColorChooser *chooser = new CQColorChooser(parent);

  chooser->setAutoFillBackground(true);

  chooser->setStyles(CQColorChooser::Text | CQColorChooser::ColorButton);

  return chooser;
}

void
CQPropertyViewColorEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(colorChanged(const QColor&)), obj, method);
}

QVariant
CQPropertyViewColorEditor::
getValue(QWidget *w)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  return chooser->colorName();
}

void
CQPropertyViewColorEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  QString str = var.toString();

  chooser->setColorName(str);
}
