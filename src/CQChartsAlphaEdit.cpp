#include <CQChartsAlphaEdit.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

CQChartsAlphaEdit::
CQChartsAlphaEdit(QWidget *parent) :
 CQComboSlider(parent)
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

    Q_EMIT alphaChanged();
  }
}

void
CQChartsAlphaEdit::
alphaToWidgets()
{
  connectSlots(false);

  this->setValue(alpha_.valueOr(1.0));

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

  CQUtil::connectDisconnect(connected_,
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
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  bool ok;
  auto alpha = CQChartsVariant::toAlpha(value, ok);
  if (! ok) return;

  auto str = alpha.toString();

  QFontMetrics fm(option.font);

  int w = fm.horizontalAdvance(str);

  //---

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsAlphaPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  auto alpha = CQChartsVariant::toAlpha(value, ok);
  if (! ok) return "";

  return alpha.toString();
}

//------

QWidget *
CQChartsAlphaPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsAlphaEdit(parent);

  edit->setAutoFillBackground(true);

  return edit;
}

void
CQChartsAlphaPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsAlphaEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(alphaChanged()), obj, method);
}

QVariant
CQChartsAlphaPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsAlphaEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromAlpha(edit->alpha());
}

void
CQChartsAlphaPropertyViewEditor::
setValue(QWidget *w, const QVariant &value)
{
  auto *edit = qobject_cast<CQChartsAlphaEdit *>(w);
  assert(edit);

  bool ok;
  auto alpha = CQChartsVariant::toAlpha(value, ok);
  if (! ok) return;

  edit->setAlpha(alpha);
}
