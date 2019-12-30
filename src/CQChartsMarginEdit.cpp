#include <CQChartsMarginEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsLineEdit.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <cassert>

CQChartsMarginEdit::
CQChartsMarginEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("lengthEdit");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  layout->addWidget(edit_);

  edit_->setToolTip("Margins in Percent (Left Top Right Bottom)");

  //---

  setFocusProxy(edit_);

  connectSlots(true);
}

const CQChartsMargin &
CQChartsMarginEdit::
margin() const
{
  return margin_;
}

void
CQChartsMarginEdit::
setMargin(const CQChartsMargin &margin)
{
  margin_ = margin;

  marginToWidgets();
}

void
CQChartsMarginEdit::
editChanged()
{
  QString value = edit_->text();

  CQChartsMargin margin(value);

  if (! margin.isValid())
    return;

  margin_ = margin;

  emit marginChanged();
}

void
CQChartsMarginEdit::
marginToWidgets()
{
  connectSlots(false);

  QString value = margin_.toString();

  edit_->setText(value);

  connectSlots(true);
}

void
CQChartsMarginEdit::
widgetsToMargin()
{
  QString value = edit_->text();

  CQChartsMargin margin(value);

  if (! margin.isValid())
    return;

  margin_ = margin;
}

void
CQChartsMarginEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, edit_, SIGNAL(editingFinished()), SLOT(editChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsMarginPropertyViewType::
CQChartsMarginPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsMarginPropertyViewType::
getEditor() const
{
  return new CQChartsMarginPropertyViewEditor;
}

bool
CQChartsMarginPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsMarginPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsMargin margin = value.value<CQChartsMargin>();

  QString str = margin.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsMarginPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsMargin>().toString();

  return str;
}

//------

CQChartsMarginPropertyViewEditor::
CQChartsMarginPropertyViewEditor()
{
}

QWidget *
CQChartsMarginPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsMarginEdit *edit = new CQChartsMarginEdit(parent);

  return edit;
}

void
CQChartsMarginPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsMarginEdit *edit = qobject_cast<CQChartsMarginEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(marginChanged()), obj, method);
}

QVariant
CQChartsMarginPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsMarginEdit *edit = qobject_cast<CQChartsMarginEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->margin());
}

void
CQChartsMarginPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsMarginEdit *edit = qobject_cast<CQChartsMarginEdit *>(w);
  assert(edit);

  CQChartsMargin margin = var.value<CQChartsMargin>();

  edit->setMargin(margin);
}
