#include <CQChartsRectEdit.h>
#include <CQChartsUnitsEdit.h>

#include <CQPropertyView.h>
#include <CQRectEdit.h>

#include <QHBoxLayout>

CQChartsRectEdit::
CQChartsRectEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("rectEdit");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  edit_ = new CQRectEdit;

  edit_->setObjectName("rect");

  layout->addWidget(edit_);

  connect(edit_, SIGNAL(valueChanged()), this, SLOT(editChanged()));

  //---

  unitsEdit_ = new CQChartsUnitsEdit;

  layout->addWidget(unitsEdit_);

  connect(unitsEdit_, SIGNAL(unitsChanged()), this, SLOT(unitsChanged()));
}

const CQChartsRect &
CQChartsRectEdit::
rect() const
{
  return rect_;
}

void
CQChartsRectEdit::
setRect(const CQChartsRect &rect)
{
  rect_ = rect;

  rectToWidgets();
}

void
CQChartsRectEdit::
editChanged()
{
  QRectF               qrect = edit_->getValue();
  const CQChartsUnits &units = rect_.units();

  CQChartsRect rect(qrect, units);

  if (! rect_.isValid())
    return;

  rect_ = rect;

  emit rectChanged();
}

void
CQChartsRectEdit::
unitsChanged()
{
  const QRectF& qrect = rect_.rect();
  CQChartsUnits units = unitsEdit_->units();

  CQChartsRect rect(qrect, units);

  if (! rect_.isValid())
    return;

  rect_ = rect;

  emit rectChanged();
}

void
CQChartsRectEdit::
rectToWidgets()
{
  connectSlots(false);

  const QRectF        &rect  = rect_.rect();
  const CQChartsUnits &units = rect_.units();

  edit_->setValue(rect);

  unitsEdit_->setUnits(units);

  connectSlots(true);
}

void
CQChartsRectEdit::
widgetsToRect()
{
  QRectF        qrect = edit_->getValue();
  CQChartsUnits units = unitsEdit_->units();

  CQChartsRect rect(qrect, units);

  if (! rect.isValid())
    return;

  rect_ = rect;
}

void
CQChartsRectEdit::
connectSlots(bool b)
{
  if (b) {
    connect(edit_, SIGNAL(valueChanged()), this, SLOT(editChanged()));
    connect(unitsEdit_, SIGNAL(unitsChanged()), this, SLOT(unitsChanged()));
  }
  else {
    disconnect(edit_, SIGNAL(valueChanged()), this, SLOT(editChanged()));
    disconnect(unitsEdit_, SIGNAL(unitsChanged()), this, SLOT(unitsChanged()));
  }
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsRectPropertyViewType::
CQChartsRectPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsRectPropertyViewType::
getEditor() const
{
  return new CQChartsRectPropertyViewEditor;
}

bool
CQChartsRectPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsRectPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsRect rect = value.value<CQChartsRect>();

  QString str = rect.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsRectPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsRect>().toString();

  return str;
}

//------

CQChartsRectPropertyViewEditor::
CQChartsRectPropertyViewEditor()
{
}

QWidget *
CQChartsRectPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsRectEdit *edit = new CQChartsRectEdit(parent);

  return edit;
}

void
CQChartsRectPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsRectEdit *edit = qobject_cast<CQChartsRectEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(rectChanged()), obj, method);
}

QVariant
CQChartsRectPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsRectEdit *edit = qobject_cast<CQChartsRectEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->rect());
}

void
CQChartsRectPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsRectEdit *edit = qobject_cast<CQChartsRectEdit *>(w);
  assert(edit);

  CQChartsRect rect = var.value<CQChartsRect>();

  edit->setRect(rect);
}
