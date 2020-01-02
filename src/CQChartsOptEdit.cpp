#include <CQChartsOptEdit.h>
#include <CQChartsLineEdit.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QHBoxLayout>

CQChartsOptEdit::
CQChartsOptEdit(QWidget *parent, const char *type) :
 QFrame(parent)
{
  setObjectName(QString("opt%1Edit").arg(type));

  //---

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  edit_->setPlaceholderText("Undefined");
  edit_->setToolTip(QString("Optional %1 (use empty string to unset)").arg(type));

  layout->addWidget(edit_);

  //---

  setFocusProxy(edit_);

  connectSlots(true);
}

void
CQChartsOptEdit::
editChanged()
{
  QString str = edit_->text();

  if (str.simplified() != "")
    setTextValue(str);
  else
    resetValue();

  emit valueChanged();
}

void
CQChartsOptEdit::
valueToWidgets()
{
  connectSlots(false);

  if (isValueSet())
    edit_->setText(valueToText());
  else
    edit_->setText("");

  connectSlots(true);
}

void
CQChartsOptEdit::
widgetsToValue()
{
  QString str = edit_->text();

  setTextValue(str);
}

void
CQChartsOptEdit::
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

bool
CQChartsOptPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsOptPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  //---

  bool ok;

  QString str = valueString(item, value, ok);

  QFont font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(font);

  int w = fm.width(str);

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsOptPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  QString str = valueString(nullptr, value, ok);

  return str;
}

//------

void
CQChartsOptPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsOptEdit *edit = qobject_cast<CQChartsOptEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQChartsOptPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsOptEdit *edit = qobject_cast<CQChartsOptEdit *>(w);
  assert(edit);

  return edit->toVariant();
}

void
CQChartsOptPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsOptEdit *edit = qobject_cast<CQChartsOptEdit *>(w);
  assert(edit);

  edit->fromVariant(var);
}
