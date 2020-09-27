#include <CQChartsLengthEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>
#include <CQRealSpin.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <cassert>

CQChartsLengthEdit::
CQChartsLengthEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("lengthEdit");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQRealSpin>("real");

  edit_->setToolTip("Length value");

  layout->addWidget(edit_);

  //---

  unitsEdit_ = new CQChartsUnitsEdit;

  layout->addWidget(unitsEdit_);

  //---

  connectSlots(true);
}

const CQChartsLength &
CQChartsLengthEdit::
length() const
{
  return length_;
}

void
CQChartsLengthEdit::
setLength(const CQChartsLength &length)
{
  length_ = length;

  lengthToWidgets();
}

void
CQChartsLengthEdit::
editChanged()
{
  double value = edit_->value();

  auto units = (length_.isValid() ? length_.units() : CQChartsUnits::NONE);

  CQChartsLength length(value, units);

  if (! length.isValid())
    return;

  length_ = length;

  emit lengthChanged();
}

void
CQChartsLengthEdit::
unitsChanged()
{
  double value = (length_.isValid() ? length_.value() : 0.0);

  auto units = unitsEdit_->units();

  CQChartsLength length(value, units);

  if (! length.isValid())
    return;

  length_ = length;

  emit lengthChanged();
}

void
CQChartsLengthEdit::
lengthToWidgets()
{
  connectSlots(false);

  if (length_.isValid()) {
    double      value = length_.value();
    const auto &units = length_.units();

    edit_     ->setValue(value);
    unitsEdit_->setUnits(units);
  }
  else {
    edit_     ->setValue(0.0);
    unitsEdit_->setUnits(CQChartsUnits::NONE);
  }

  connectSlots(true);
}

#if 0
void
CQChartsLengthEdit::
widgetsToLength()
{
  double value = edit_->value();
  auto   units = unitsEdit_->units();

  CQChartsLength length(value, units);

  if (! length.isValid())
    return;

  length_ = length;
}
#endif

void
CQChartsLengthEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(edit_, SIGNAL(valueChanged(double)), SLOT(editChanged()));
  connectDisconnect(unitsEdit_, SIGNAL(unitsChanged()), SLOT(unitsChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsLengthPropertyViewType::
CQChartsLengthPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsLengthPropertyViewType::
getEditor() const
{
  return new CQChartsLengthPropertyViewEditor;
}

bool
CQChartsLengthPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsLengthPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  bool ok;
  auto length = CQChartsVariant::toLength(value, ok);
  if (! ok) return;

  auto str = length.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsLengthPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  auto length = CQChartsVariant::toLength(value, ok);
  if (! ok) return "";

  return length.toString();
}

//------

CQChartsLengthPropertyViewEditor::
CQChartsLengthPropertyViewEditor()
{
}

QWidget *
CQChartsLengthPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsLengthEdit(parent);

  return edit;
}

void
CQChartsLengthPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsLengthEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(lengthChanged()), obj, method);
}

QVariant
CQChartsLengthPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsLengthEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromLength(edit->length());
}

void
CQChartsLengthPropertyViewEditor::
setValue(QWidget *w, const QVariant &value)
{
  auto *edit = qobject_cast<CQChartsLengthEdit *>(w);
  assert(edit);

  bool ok;
  auto length = CQChartsVariant::toLength(value, ok);
  if (! ok) return;

  edit->setLength(length);
}
