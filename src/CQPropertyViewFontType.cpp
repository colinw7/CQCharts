#include <CQPropertyViewFontType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQFontChooser.h>
#include <cassert>

CQPropertyViewFontType::
CQPropertyViewFontType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewFontType::
getEditor() const
{
  return new CQPropertyViewFontEditor;
}

bool
CQPropertyViewFontType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewFontType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  delegate->drawFont(painter, option, value.value<QFont>(), index, inside);
}

//------

CQPropertyViewFontEditor::
CQPropertyViewFontEditor()
{
}

QWidget *
CQPropertyViewFontEditor::
createEdit(QWidget *parent)
{
  CQFontChooser *chooser = new CQFontChooser(parent);

  return chooser;
}

void
CQPropertyViewFontEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(fontChanged(const QString&)), obj, method);
}

QVariant
CQPropertyViewFontEditor::
getValue(QWidget *w)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  return chooser->fontName();
}

void
CQPropertyViewFontEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  QString str = var.toString();

  chooser->setFontName(str);
}
