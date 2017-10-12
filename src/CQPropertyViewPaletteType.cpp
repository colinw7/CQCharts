#include <CQPropertyViewPaletteType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPaletteChooser.h>

CQPropertyViewPaletteType::
CQPropertyViewPaletteType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewPaletteType::
getEditor() const
{
  return new CQPropertyViewPaletteEditor;
}

bool
CQPropertyViewPaletteType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewPaletteType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  QString str = CQUtil::paletteToString(value.value<QPalette>());

  delegate->drawString(painter, option, str, index, inside);
}

QString
CQPropertyViewPaletteType::
tip(const QVariant &value) const
{
  return CQUtil::paletteToString(value.value<QPalette>());
}

//------

CQPropertyViewPaletteEditor::
CQPropertyViewPaletteEditor()
{
}

QWidget *
CQPropertyViewPaletteEditor::
createEdit(QWidget *parent)
{
  CQPaletteChooser *chooser = new CQPaletteChooser(parent);

  return chooser;
}

void
CQPropertyViewPaletteEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQPaletteChooser *chooser = qobject_cast<CQPaletteChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(paletteChanged(const QString&)), obj, method);
}

QVariant
CQPropertyViewPaletteEditor::
getValue(QWidget *w)
{
  CQPaletteChooser *chooser = qobject_cast<CQPaletteChooser *>(w);
  assert(chooser);

  return chooser->palette();
}

void
CQPropertyViewPaletteEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQPaletteChooser *chooser = qobject_cast<CQPaletteChooser *>(w);
  assert(chooser);

  QString str = var.toString();

  chooser->setPaletteDef(str);
}
