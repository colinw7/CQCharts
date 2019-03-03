#include <CQChartsEnumEdit.h>

#include <CQPropertyView.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsEnumEdit::
CQChartsEnumEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("enumEdit");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  combo_ = new QComboBox;

  combo_->setObjectName("combo");

  layout->addWidget(combo_);

  QObject::connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

void
CQChartsEnumEdit::
init()
{
  combo_->addItems(enumNames());
}

void
CQChartsEnumEdit::
setEnumString(const QString &str)
{
  QObject::disconnect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  combo_->setCurrentIndex(combo_->findText(str));

  QObject::connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

void
CQChartsEnumEdit::
comboChanged()
{
  QObject::disconnect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  setEnumFromString(combo_->currentText());

  QObject::connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

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
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
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
