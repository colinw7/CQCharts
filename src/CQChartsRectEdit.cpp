#include <CQChartsRectEdit.h>
#include <CQChartsUnitsEdit.h>

#include <CQPropertyView.h>
#include <CQRectEdit.h>
#include <CQUtil.h>

#include <QHBoxLayout>

CQChartsRectEdit::
CQChartsRectEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("rectEdit");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQRectEdit>("rect");

  layout->addWidget(edit_);

  //---

  unitsEdit_ = new CQChartsUnitsEdit;

  layout->addWidget(unitsEdit_);

  //---

  connectSlots(true);
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
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, edit_, SIGNAL(valueChanged()), SLOT(editChanged()));
  connectDisconnect(b, unitsEdit_, SIGNAL(unitsChanged()), SLOT(unitsChanged()));
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
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
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
