#include <CQChartsFillPatternEdit.h>

#include <CQPropertyView.h>
#include <CQRealSpin.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsFillPatternEdit::
CQChartsFillPatternEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("fillPattern");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  combo_ = new QComboBox;

  combo_->addItems(CQChartsFillPattern::typeNames());

  combo_->setObjectName("combo");

  layout->addWidget(combo_);

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

const CQChartsFillPattern &
CQChartsFillPatternEdit::
fillPattern() const
{
  return fillPattern_;
}

void
CQChartsFillPatternEdit::
setFillPattern(const CQChartsFillPattern &fillPattern)
{
  disconnect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  fillPattern_ = fillPattern;

  combo_->setCurrentIndex(combo_->findText(fillPattern_.toString()));

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

void
CQChartsFillPatternEdit::
comboChanged()
{
  disconnect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  fillPattern_ = CQChartsFillPattern(combo_->currentText());

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  emit fillPatternChanged();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsFillPatternPropertyViewType::
CQChartsFillPatternPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsFillPatternPropertyViewType::
getEditor() const
{
  return new CQChartsFillPatternPropertyViewEditor;
}

bool
CQChartsFillPatternPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsFillPatternPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsFillPattern fillPattern = value.value<CQChartsFillPattern>();

  QString str = fillPattern.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsFillPatternPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsFillPattern>().toString();

  return str;
}

//------

CQChartsFillPatternPropertyViewEditor::
CQChartsFillPatternPropertyViewEditor()
{
}

QWidget *
CQChartsFillPatternPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsFillPatternEdit *edit = new CQChartsFillPatternEdit(parent);

  return edit;
}

void
CQChartsFillPatternPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsFillPatternEdit *edit = qobject_cast<CQChartsFillPatternEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillPatternChanged()), obj, method);
}

QVariant
CQChartsFillPatternPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsFillPatternEdit *edit = qobject_cast<CQChartsFillPatternEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillPattern());
}

void
CQChartsFillPatternPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsFillPatternEdit *edit = qobject_cast<CQChartsFillPatternEdit *>(w);
  assert(edit);

  CQChartsFillPattern fillPattern = var.value<CQChartsFillPattern>();

  edit->setFillPattern(fillPattern);
}
