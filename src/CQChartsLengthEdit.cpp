#include <CQChartsLengthEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>
#include <CQPropertyViewTree.h>
#include <CQRealSpin.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <cassert>

bool CQChartsSwitchLengthEdit::s_isAlt { true };

CQChartsSwitchLengthEdit::
CQChartsSwitchLengthEdit(QWidget *parent) :
 CQSwitchLineEdit(parent)
{
  setObjectName("switchLengthEdit");

  setToolTip("Set Length");

  //---

  edit_ = new CQChartsLengthEdit(parent);

  setAltEdit(edit_);
  setShowAltEdit(s_isAlt);

  connectSlots(true);
}

void
CQChartsSwitchLengthEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQUtil::connectDisconnect(b, w, from, this, to);
  };

  connectDisconnect(this, SIGNAL(editSwitched(bool)), SLOT(editSwitched(bool)));
  connectDisconnect(this, SIGNAL(editingFinished()), SLOT(textChangedSlot()));
  connectDisconnect(edit_, SIGNAL(lengthChanged()), SLOT(lengthChangedSlot()));

  connectDisconnect(this, SIGNAL(editingFinished()), SIGNAL(altEditingFinished()));
  connectDisconnect(edit_, SIGNAL(editingFinished()), SIGNAL(altEditingFinished()));
}

CQChartsLength
CQChartsSwitchLengthEdit::
length() const
{
  return edit_->length();
}

void
CQChartsSwitchLengthEdit::
setLength(const CQChartsLength &l)
{
  edit_->setLength(l);

  setText(length().toString());
}

void
CQChartsSwitchLengthEdit::
editSwitched(bool b)
{
  s_isAlt = b;
}

void
CQChartsSwitchLengthEdit::
setPropertyView(CQPropertyViewTree *pv)
{
  pv_ = pv;

  connect(this, SIGNAL(altEditingFinished()), pv_, SLOT(closeEditorSlot()));
}

void
CQChartsSwitchLengthEdit::
lengthChangedSlot()
{
  connectSlots(false);

  setText(edit_->length().toString());

  Q_EMIT lengthChanged();

  connectSlots(true);
}

void
CQChartsSwitchLengthEdit::
textChangedSlot()
{
  connectSlots(false);

  CQChartsLength l;

  if (! l.fromString(text()))
    setText(length().toString());

  edit_->setLength(l);

  Q_EMIT lengthChanged();

  connectSlots(true);
}

void
CQChartsSwitchLengthEdit::
updatePlacement()
{
  if (pv_) {
    disconnect(this, SIGNAL(altEditingFinished()), pv_, SLOT(closeEditorSlot()));

    auto triggers = pv_->editTriggers();

    pv_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    CQSwitchLineEdit::updatePlacement();

    pv_->setEditTriggers(triggers);

    connect(this, SIGNAL(altEditingFinished()), pv_, SLOT(closeEditorSlot()));
  }
  else
    CQSwitchLineEdit::updatePlacement();
}

//---

CQChartsLengthEdit::
CQChartsLengthEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("lengthEdit");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQRealSpin>("real");

  edit_->setDecimals(3);
  edit_->setToolTip("Length value");

  layout->addWidget(edit_);

  //---

  unitsEdit_ = new CQChartsUnitsEdit;

  layout->addWidget(unitsEdit_);

  //---

  setFocusProxy(edit_);

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

  auto units = (length_.isValid() ? length_.units() : Units::NONE);

  CQChartsLength length(value, units);

  if (! length.isValid())
    return;

  length_ = length;

  Q_EMIT lengthChanged();
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

  Q_EMIT lengthChanged();
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
    unitsEdit_->setUnits(Units::NONE);
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
    CQUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(edit_, SIGNAL(valueChanged(double)), SLOT(editChanged()));
  connectDisconnect(edit_, SIGNAL(keyPress(int, int)), SLOT(spinKeyPress(int, int)));

  connectDisconnect(edit_, SIGNAL(editingFinished()), SIGNAL(editingFinished()));

  connectDisconnect(unitsEdit_, SIGNAL(unitsChanged()), SLOT(unitsChanged()));
}

void
CQChartsLengthEdit::
spinKeyPress(int key, int /*modifiers*/)
{
  static int lastKey = 0;

  if      (key == 'M' && lastKey == 'E') unitsEdit_->setUnits(Units::EM);
  else if (key == 'X' && lastKey == 'E') unitsEdit_->setUnits(Units::EX);
  else if (key == '%'                  ) unitsEdit_->setUnits(Units::PERCENT);
  else if (key == 'P'                  ) unitsEdit_->setUnits(Units::PLOT);
  else if (key == 'P' && lastKey == 'S') unitsEdit_->setUnits(Units::SUB_PLOT);
  else if (key == 'V'                  ) unitsEdit_->setUnits(Units::VIEW);
  else if (key == 'X'                  ) unitsEdit_->setUnits(Units::PIXEL);

  lastKey = key;
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

  int w = fm.horizontalAdvance(str);

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
  auto *edit = new CQChartsSwitchLengthEdit(parent);

  auto *pv = dynamic_cast<CQPropertyViewTree *>(parent ? parent->parentWidget() : nullptr);

  if (pv)
    edit->setPropertyView(pv);

  return edit;
}

void
CQChartsLengthPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsSwitchLengthEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(lengthChanged()), obj, method);
}

QVariant
CQChartsLengthPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsSwitchLengthEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromLength(edit->length());
}

void
CQChartsLengthPropertyViewEditor::
setValue(QWidget *w, const QVariant &value)
{
  auto *edit = qobject_cast<CQChartsSwitchLengthEdit *>(w);
  assert(edit);

  bool ok;
  auto length = CQChartsVariant::toLength(value, ok);
  if (! ok) return;

  edit->setLength(length);
}
