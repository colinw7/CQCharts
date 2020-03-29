#include <CQChartsPositionEdit.h>
#include <CQChartsGeomPointEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QHBoxLayout>

CQChartsPositionEdit::
CQChartsPositionEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("positionEdit");

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQChartsGeomPointEdit>("point");

  edit_->setToolTip("Position");

  layout->addWidget(edit_);

  //---

  unitsEdit_ = new CQChartsUnitsEdit;

  layout->addWidget(unitsEdit_);

  //---

  setFocusProxy(edit_);

  connectSlots(true);
}

const CQChartsPosition &
CQChartsPositionEdit::
position() const
{
  return position_;
}

void
CQChartsPositionEdit::
setPosition(const CQChartsPosition &position)
{
  position_ = position;

  positionToWidgets();
}

void
CQChartsPositionEdit::
editChanged()
{
  auto        point = edit_->getValue();
  const auto &units = position_.units();

  CQChartsPosition position(point, units);

  if (! position.isValid())
    return;

  position_ = position;

  emit positionChanged();
}

void
CQChartsPositionEdit::
unitsChanged()
{
  auto value = position_.p();
  auto units = unitsEdit_->units();

  CQChartsPosition position(value, units);

  if (! position.isValid())
    return;

  position_ = position;

  emit positionChanged();
}

void
CQChartsPositionEdit::
positionToWidgets()
{
  connectSlots(false);

  auto        point = position_.p();
  const auto &units = position_.units();

  edit_->setValue(point);

  unitsEdit_->setUnits(units);

  connectSlots(true);
}

void
CQChartsPositionEdit::
widgetsToPosition()
{
  auto value = edit_->getValue();
  auto units = unitsEdit_->units();

  CQChartsPosition position(value, units);

  if (! position.isValid())
    return;

  position_ = position;
}

void
CQChartsPositionEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(edit_, SIGNAL(valueChanged()), SLOT(editChanged()));
  connectDisconnect(unitsEdit_, SIGNAL(unitsChanged()), SLOT(unitsChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsPositionPropertyViewType::
CQChartsPositionPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsPositionPropertyViewType::
getEditor() const
{
  return new CQChartsPositionPropertyViewEditor;
}

bool
CQChartsPositionPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsPositionPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsPosition position = value.value<CQChartsPosition>();

  QString str = position.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsPositionPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsPosition>().toString();

  return str;
}

//------

CQChartsPositionPropertyViewEditor::
CQChartsPositionPropertyViewEditor()
{
}

QWidget *
CQChartsPositionPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsPositionEdit(parent);

  return edit;
}

void
CQChartsPositionPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(positionChanged()), obj, method);
}

QVariant
CQChartsPositionPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->position());
}

void
CQChartsPositionPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  CQChartsPosition position = var.value<CQChartsPosition>();

  edit->setPosition(position);
}
