#include <CQPropertyViewLineDashType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQLineDash.h>
#include <QPainter>

CQPropertyViewLineDashType::
CQPropertyViewLineDashType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewLineDashType::
getEditor() const
{
  return new CQPropertyViewLineDashEditor;
}

bool
CQPropertyViewLineDashType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewLineDashType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, index, inside);

  CLineDash dash = value.value<CLineDash>();

  QIcon icon = CQLineDash::dashIcon(dash);

  QString str = dash.toString().c_str();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QRect irect = option.rect;

  irect.setLeft(irect.left() + w + 8);

  painter->drawPixmap(irect, icon.pixmap(option.rect.size()));

  //--

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, index, inside);
}

QString
CQPropertyViewLineDashType::
tip(const QVariant &value) const
{
  QString str = value.value<CLineDash>().toString().c_str();

  return str;
}

//------

CQPropertyViewLineDashEditor::
CQPropertyViewLineDashEditor()
{
}

QWidget *
CQPropertyViewLineDashEditor::
createEdit(QWidget *parent)
{
  CQLineDash *edit = new CQLineDash(parent);

  return edit;
}

void
CQPropertyViewLineDashEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQLineDash *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged(const CLineDash &)), obj, method);
}

QVariant
CQPropertyViewLineDashEditor::
getValue(QWidget *w)
{
  CQLineDash *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  return QVariant::fromValue(edit->getLineDash());
}

void
CQPropertyViewLineDashEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQLineDash *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  CLineDash dash = var.value<CLineDash>();

  edit->setLineDash(dash);
}
