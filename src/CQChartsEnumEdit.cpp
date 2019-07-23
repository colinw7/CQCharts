#include <CQChartsEnumEdit.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsEnumEdit::
CQChartsEnumEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("enumEdit");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsEnumEdit::
init()
{
  combo_->addItems(enumNames());
}

void
CQChartsEnumEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      QObject::connect(w, from, this, to);
    else
      QObject::disconnect(w, from, this, to);
  };

  connectDisconnect(b, combo_, SIGNAL(currentIndexChanged(int)), SLOT(comboChanged()));
}

void
CQChartsEnumEdit::
setEnumString(const QString &str)
{
  connectSlots(false);

  combo_->setCurrentIndex(combo_->findText(str));

  connectSlots(true);
}

void
CQChartsEnumEdit::
comboChanged()
{
  connectSlots(false);

  setEnumFromString(combo_->currentText());

  connectSlots(true);

  emit enumChanged();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsEnumPropertyViewType::
CQChartsEnumPropertyViewType()
{
}

bool
CQChartsEnumPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsEnumPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  QString str = variantToString(value);

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsEnumPropertyViewType::
tip(const QVariant &value) const
{
  QString str = variantToString(value);

  return str;
}

//------

CQChartsEnumPropertyViewEditorFactory::
CQChartsEnumPropertyViewEditorFactory()
{
}

void
CQChartsEnumPropertyViewEditorFactory::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsEnumEdit *edit = qobject_cast<CQChartsEnumEdit *>(w);
  assert(edit);

  edit->connect(obj, method);
}

QVariant
CQChartsEnumPropertyViewEditorFactory::
getValue(QWidget *w)
{
  CQChartsEnumEdit *edit = qobject_cast<CQChartsEnumEdit *>(w);
  assert(edit);

  return edit->getVariantFromEnum();
}

void
CQChartsEnumPropertyViewEditorFactory::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsEnumEdit *edit = qobject_cast<CQChartsEnumEdit *>(w);
  assert(edit);

  edit->setEnumFromVariant(var);
}
