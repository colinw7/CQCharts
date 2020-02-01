#include <CQChartsAngleEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

CQChartsAngleEdit::
CQChartsAngleEdit(QWidget *parent) :
 CQAngleSpinBox(parent)
{
  setObjectName("angle");

  setToolTip("Angle (-360.0 -> 360.0)");

  //---

  connectSlots(true);
}

void
CQChartsAngleEdit::
setAngle(const CQChartsAngle &angle)
{
  angle_ = angle;

  angleToWidgets();
}

void
CQChartsAngleEdit::
editChanged()
{
  CQChartsAngle angle(this->value());

  if (angle.isValid()) {
    angle_ = angle;

    emit angleChanged();
  }
}

void
CQChartsAngleEdit::
angleToWidgets()
{
  connectSlots(false);

  this->setValue(angle_.value());

  connectSlots(true);
}

void
CQChartsAngleEdit::
widgetsToAngle()
{
  CQChartsAngle angle(this->value());

  if (angle.isValid())
    angle_ = angle;
}

void
CQChartsAngleEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  CQChartsWidgetUtil::connectDisconnect(connected_,
    this, SIGNAL(angleChanged(const CAngle &)), this, SLOT(editChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQPropertyViewEditorFactory *
CQChartsAnglePropertyViewType::
getEditor() const
{
  return new CQChartsAnglePropertyViewEditor;
}

bool
CQChartsAnglePropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsAnglePropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  QString str = value.value<CQChartsAngle>().toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsAnglePropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsAngle>().toString();

  return str;
}

//------

QWidget *
CQChartsAnglePropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsAngleEdit *edit = new CQChartsAngleEdit(parent);

  return edit;
}

void
CQChartsAnglePropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsAngleEdit *edit = qobject_cast<CQChartsAngleEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(angleChanged()), obj, method);
}

QVariant
CQChartsAnglePropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsAngleEdit *edit = qobject_cast<CQChartsAngleEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->angle());
}

void
CQChartsAnglePropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsAngleEdit *edit = qobject_cast<CQChartsAngleEdit *>(w);
  assert(edit);

  CQChartsAngle angle = var.value<CQChartsAngle>();

  edit->setAngle(angle);
}
