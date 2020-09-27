#include <CQChartsMarginEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <cassert>

CQChartsMarginEdit::
CQChartsMarginEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("lengthEdit");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  layout->addWidget(edit_);

  edit_->setToolTip("Margins in Percent (Left Top Right Bottom)");

  //---

  setFocusProxy(edit_);

  connectSlots(true);
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
  auto value = edit_->text();

  CQChartsMargin margin(value);

  if (margin.isValid()) {
    margin_ = margin;

    emit marginChanged();
  }
}

void
CQChartsMarginEdit::
marginToWidgets()
{
  connectSlots(false);

  auto value = margin_.toString();

  edit_->setText(value);

  connectSlots(true);
}

void
CQChartsMarginEdit::
widgetsToMargin()
{
  auto value = edit_->text();

  CQChartsMargin margin(value);

  if (margin.isValid())
    margin_ = margin;
}

void
CQChartsMarginEdit::
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
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  auto margin = value.value<CQChartsMargin>();

  auto str = margin.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*CQPropertyViewType::margin());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsMarginPropertyViewType::
tip(const QVariant &value) const
{
  auto str = value.value<CQChartsMargin>().toString();

  return str;
}

//------

QWidget *
CQChartsMarginPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsMarginEdit(parent);

  return edit;
}

void
CQChartsMarginPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsMarginEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(marginChanged()), obj, method);
}

QVariant
CQChartsMarginPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsMarginEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->margin());
}

void
CQChartsMarginPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsMarginEdit *>(w);
  assert(edit);

  auto margin = var.value<CQChartsMargin>();

  edit->setMargin(margin);
}
