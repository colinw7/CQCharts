#include <CQChartsValueListEdit.h>

#include <CQPropertyView.h>
#include <CQPropertyViewTree.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsValueListEdit::
CQChartsValueListEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("valueListEdit");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsValueListEdit::
setValueList(const CQChartsValueList &v)
{
  valueList_ = v;

  QStringList strs;

  for (const auto &value : valueList_.values()) {
    QString str;

    if (! CQChartsVariant::toString(value, str))
      str = value.toString();

    strs += str;
  }

  combo_->clear();

  combo_->addItems(strs);
}

void
CQChartsValueListEdit::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));
}

void
CQChartsValueListEdit::
comboChanged(int ind)
{
  if (ind != valueList_.currentInd()) {
    connectSlots(false);

    valueList_.setCurrentInd(ind);

    connectSlots(true);

    Q_EMIT currentIndChanged();
  }
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsValueListPropertyViewType::
CQChartsValueListPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsValueListPropertyViewType::
getEditor() const
{
  return new CQChartsValueListPropertyViewEditor;
}

bool
CQChartsValueListPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsValueListPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  auto valueList = CQChartsValueList::fromVariant(value);

  auto currentInd = valueList.currentInd();

  auto currentValue = (currentInd >= 0 ? valueList.value(currentInd) : QString());

  QString str;

  if (! CQChartsVariant::toString(currentValue, str))
    str = currentValue.toString();

  QFontMetrics fm(option.font);

  int w = fm.horizontalAdvance(str);

  //---

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsValueListPropertyViewType::
tip(const QVariant &value) const
{
  QString str;

  if (! CQChartsVariant::toString(value, str))
    str = value.toString();

  return str;
}

//------

CQChartsValueListPropertyViewEditor::
CQChartsValueListPropertyViewEditor()
{
}

QWidget *
CQChartsValueListPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsValueListEdit(parent);

  //auto *pv = dynamic_cast<CQPropertyViewTree *>(parent ? parent->parentWidget() : nullptr);
  //if (pv) edit->setPropertyView(pv);

  return edit;
}

void
CQChartsValueListPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsValueListEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(currentIndChanged()), obj, method);
}

QVariant
CQChartsValueListPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsValueListEdit *>(w);
  assert(edit);

  return CQChartsValueList::toVariant(edit->valueList());
}

void
CQChartsValueListPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsValueListEdit *>(w);
  assert(edit);

  edit->setValueList(CQChartsValueList::fromVariant((var)));
}
