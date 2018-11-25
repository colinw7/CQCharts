#include <CQChartsPositionEdit.h>

#include <CQPropertyView.h>
#include <CQPoint2DEdit.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsPositionEdit::
CQChartsPositionEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("positionEdit");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  edit_ = new CQPoint2DEdit;

  edit_->setObjectName("point");

  layout->addWidget(edit_);

  connect(edit_, SIGNAL(valueChanged()), this, SLOT(editChanged()));

  //---

  unitsCombo_ = new QComboBox;

  unitsCombo_->setObjectName("units");
  unitsCombo_->addItems(CQChartsUtil::unitNames());

  layout->addWidget(unitsCombo_);

  connect(unitsCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(unitsChanged()));

  //---

  updateUnits();
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

  updateUnits();
}

void
CQChartsPositionEdit::
editChanged()
{
  QPointF              value = edit_->getQValue();
  const CQChartsUnits &units = position_.units();

  CQChartsPosition position(value, units);

  if (! position.isValid())
    return;

  position_ = position;

  emit positionChanged();
}

void
CQChartsPositionEdit::
unitsChanged()
{
  QPointF       value = position_.p();
  CQChartsUnits units = (CQChartsUnits) unitsCombo_->currentIndex();

  CQChartsPosition position(value, units);

  if (! position.isValid())
    return;

  position_ = position;

  updateUnits();

  emit positionChanged();
}

void
CQChartsPositionEdit::
positionToWidgets()
{
  connectSlots(false);

  QPointF              value = position_.p();
  const CQChartsUnits &units = position_.units();

  QString ustr = CQChartsUtil::unitsString(units);

  edit_->setValue(value);

  unitsCombo_->setCurrentIndex(unitsCombo_->findText(ustr, Qt::MatchExactly));

  connectSlots(true);
}

void
CQChartsPositionEdit::
widgetsToPosition()
{
  QPointF       value = edit_->getQValue();
  CQChartsUnits units = (CQChartsUnits) unitsCombo_->currentIndex();

  CQChartsPosition position(value, units);

  if (! position.isValid())
    return;

  position_ = position;
}

void
CQChartsPositionEdit::
connectSlots(bool b)
{
  if (b) {
    connect(edit_, SIGNAL(valueChanged()), this, SLOT(editChanged()));
    connect(unitsCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(unitsChanged()));
  }
  else {
    disconnect(edit_, SIGNAL(valueChanged()), this, SLOT(editChanged()));
    disconnect(unitsCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(unitsChanged()));
  }
}

void
CQChartsPositionEdit::
updateUnits()
{
  int ind = unitsCombo_->currentIndex();

  unitsCombo_->setToolTip(CQChartsUtil::unitTipNames()[ind]);
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
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsPosition position = value.value<CQChartsPosition>();

  QString str = position.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

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
  CQChartsPositionEdit *edit = new CQChartsPositionEdit(parent);

  return edit;
}

void
CQChartsPositionPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsPositionEdit *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(positionChanged()), obj, method);
}

QVariant
CQChartsPositionPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsPositionEdit *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->position());
}

void
CQChartsPositionPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsPositionEdit *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  CQChartsPosition position = var.value<CQChartsPosition>();

  edit->setPosition(position);
}
