#include <CQChartsLengthEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsWidgetUtil.h>

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

  CQChartsUnits units = (length_.isValid() ? length_.units() : CQChartsUnits::NONE);

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

  CQChartsUnits units = unitsEdit_->units();

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
    double               value = length_.value();
    const CQChartsUnits &units = length_.units();

    edit_     ->setValue(value);
    unitsEdit_->setUnits(units);
  }
  else {
    edit_     ->setValue(0.0);
    unitsEdit_->setUnits(CQChartsUnits::NONE);
  }

  connectSlots(true);
}

void
CQChartsLengthEdit::
widgetsToLength()
{
  double        value = edit_->value();
  CQChartsUnits units = unitsEdit_->units();

  CQChartsLength length(value, units);

  if (! length.isValid())
    return;

  length_ = length;
}

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
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsLength length = value.value<CQChartsLength>();

  QString str = length.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsLengthPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsLength>().toString();

  return str;
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
  CQChartsLengthEdit *edit = new CQChartsLengthEdit(parent);

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

  return QVariant::fromValue(edit->length());
}

void
CQChartsLengthPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsLengthEdit *>(w);
  assert(edit);

  CQChartsLength length = var.value<CQChartsLength>();

  edit->setLength(length);
}
