#include <CQChartsLengthEdit.h>
#include <CQChartsUnitsEdit.h>

#include <CQPropertyView.h>
#include <CQRealSpin.h>

#include <QHBoxLayout>

CQChartsLengthEdit::
CQChartsLengthEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("lengthEdit");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  edit_ = new CQRealSpin;

  edit_->setObjectName("real");

  layout->addWidget(edit_);

  connect(edit_, SIGNAL(valueChanged(double)), this, SLOT(editChanged()));

  //---

  unitsEdit_ = new CQChartsUnitsEdit;

  layout->addWidget(unitsEdit_);

  connect(unitsEdit_, SIGNAL(unitsChanged()), this, SLOT(unitsChanged()));
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
  double               value = edit_->value();
  const CQChartsUnits &units = length_.units();

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
  double        value = length_.value();
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

  double               value = length_.value();
  const CQChartsUnits &units = length_.units();

  edit_->setValue(value);

  unitsEdit_->setUnits(units);

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
  if (b) {
    connect(edit_, SIGNAL(valueChanged(double)), this, SLOT(editChanged()));
    connect(unitsEdit_, SIGNAL(unitsChanged()), this, SLOT(unitsChanged()));
  }
  else {
    disconnect(edit_, SIGNAL(valueChanged(double)), this, SLOT(editChanged()));
    disconnect(unitsEdit_, SIGNAL(unitsChanged()), this, SLOT(unitsChanged()));
  }
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
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsLength length = value.value<CQChartsLength>();

  QString str = length.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

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
  CQChartsLengthEdit *edit = qobject_cast<CQChartsLengthEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(lengthChanged()), obj, method);
}

QVariant
CQChartsLengthPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsLengthEdit *edit = qobject_cast<CQChartsLengthEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->length());
}

void
CQChartsLengthPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsLengthEdit *edit = qobject_cast<CQChartsLengthEdit *>(w);
  assert(edit);

  CQChartsLength length = var.value<CQChartsLength>();

  edit->setLength(length);
}
