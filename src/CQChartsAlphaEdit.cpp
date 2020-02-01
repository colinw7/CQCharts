#include <CQChartsAlphaEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

CQChartsAlphaEdit::
CQChartsAlphaEdit(QWidget *parent) :
 CQRealSpin(parent)
{
  setObjectName("alpha");

  setToolTip("Color Alpha (0.0 -> 1.0)");

  setRange(0.0, 1.0);
  setValue(1.0);

  //---

  connectSlots(true);
}

void
CQChartsAlphaEdit::
setAlpha(const CQChartsAlpha &alpha)
{
  alpha_ = alpha;

  alphaToWidgets();
}

void
CQChartsAlphaEdit::
editChanged()
{
  CQChartsAlpha alpha(this->value());

  if (alpha.isValid()) {
    alpha_ = alpha;

    emit alphaChanged();
  }
}

void
CQChartsAlphaEdit::
alphaToWidgets()
{
  connectSlots(false);

  this->setValue(alpha_.value());

  connectSlots(true);
}

void
CQChartsAlphaEdit::
widgetsToAlpha()
{
  CQChartsAlpha alpha(this->value());

  if (alpha.isValid())
    alpha_ = alpha;
}

void
CQChartsAlphaEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  CQChartsWidgetUtil::connectDisconnect(connected_,
    this, SIGNAL(valueChanged(double)), this, SLOT(editChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQPropertyViewEditorFactory *
CQChartsAlphaPropertyViewType::
getEditor() const
{
  return new CQChartsAlphaPropertyViewEditor;
}

bool
CQChartsAlphaPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsAlphaPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  QString str = value.value<CQChartsAlpha>().toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsAlphaPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsAlpha>().toString();

  return str;
}

//------

QWidget *
CQChartsAlphaPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsAlphaEdit *edit = new CQChartsAlphaEdit(parent);

  return edit;
}

void
CQChartsAlphaPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsAlphaEdit *edit = qobject_cast<CQChartsAlphaEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(alphaChanged()), obj, method);
}

QVariant
CQChartsAlphaPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsAlphaEdit *edit = qobject_cast<CQChartsAlphaEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->alpha());
}

void
CQChartsAlphaPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsAlphaEdit *edit = qobject_cast<CQChartsAlphaEdit *>(w);
  assert(edit);

  CQChartsAlpha alpha = var.value<CQChartsAlpha>();

  edit->setAlpha(alpha);
}
