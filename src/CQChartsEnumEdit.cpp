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

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

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
  CQUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

void
CQChartsEnumEdit::
setEnumString(const QString &str)
{
  auto lstr = str.toLower();

  connectSlots(false);

  for (int i = 0; i < combo_->count(); ++i) {
    if (combo_->itemText(i).toLower() == lstr) {
      combo_->setCurrentIndex(i);
      break;
    }
  }

  connectSlots(true);
}

void
CQChartsEnumEdit::
comboChanged()
{
  connectSlots(false);

  setEnumFromString(combo_->currentText());

  connectSlots(true);

  Q_EMIT enumChanged();
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
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  auto str = variantToString(value);

  QFontMetrics fm(option.font);

  int w = fm.horizontalAdvance(str);

  //---

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsEnumPropertyViewType::
tip(const QVariant &value) const
{
  auto str = variantToString(value);

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
  auto *edit = qobject_cast<CQChartsEnumEdit *>(w);
  assert(edit);

  edit->connect(obj, method);
}

QVariant
CQChartsEnumPropertyViewEditorFactory::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsEnumEdit *>(w);
  assert(edit);

  return edit->getVariantFromEnum();
}

void
CQChartsEnumPropertyViewEditorFactory::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsEnumEdit *>(w);
  assert(edit);

  edit->setEnumFromVariant(var);
}
