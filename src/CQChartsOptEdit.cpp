#include <CQChartsOptEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QHBoxLayout>

CQChartsOptEdit::
CQChartsOptEdit(QWidget *parent, const char *type) :
 QFrame(parent)
{
  setObjectName(QString("opt%1Edit").arg(type));

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

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

  CQChartsWidgetUtil::connectDisconnect(connected_,
    edit_, SIGNAL(editingFinished()), this, SLOT(editChanged()));
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
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  //---

  bool ok;

  QString str = valueString(item, value, ok);

  QFont font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(font);

  int w = fm.width(str);

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, itemState);
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
  auto *edit = qobject_cast<CQChartsOptEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQChartsOptPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsOptEdit *>(w);
  assert(edit);

  return edit->toVariant();
}

void
CQChartsOptPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsOptEdit *>(w);
  assert(edit);

  edit->fromVariant(var);
}
