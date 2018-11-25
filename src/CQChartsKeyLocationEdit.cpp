#include <CQChartsKeyLocationEdit.h>

#include <CQPropertyView.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsKeyLocationEdit::
CQChartsKeyLocationEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("keyLocation");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  combo_ = new QComboBox;

  combo_->addItems(CQChartsKeyLocation::locationNames());

  combo_->setObjectName("combo");

  layout->addWidget(combo_);

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

const CQChartsKeyLocation &
CQChartsKeyLocationEdit::
keyLocation() const
{
  return keyLocation_;
}

void
CQChartsKeyLocationEdit::
setKeyLocation(const CQChartsKeyLocation &keyLocation)
{
  disconnect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  keyLocation_ = keyLocation;

  combo_->setCurrentIndex(combo_->findText(keyLocation_.toString()));

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

void
CQChartsKeyLocationEdit::
comboChanged()
{
  disconnect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  keyLocation_ = CQChartsKeyLocation(combo_->currentText());

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  emit keyLocationChanged();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsKeyLocationPropertyViewType::
CQChartsKeyLocationPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsKeyLocationPropertyViewType::
getEditor() const
{
  return new CQChartsKeyLocationPropertyViewEditor;
}

bool
CQChartsKeyLocationPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsKeyLocationPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsKeyLocation keyLocation = value.value<CQChartsKeyLocation>();

  QString str = keyLocation.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsKeyLocationPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsKeyLocation>().toString();

  return str;
}

//------

CQChartsKeyLocationPropertyViewEditor::
CQChartsKeyLocationPropertyViewEditor()
{
}

QWidget *
CQChartsKeyLocationPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsKeyLocationEdit *edit = new CQChartsKeyLocationEdit(parent);

  return edit;
}

void
CQChartsKeyLocationPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsKeyLocationEdit *edit = qobject_cast<CQChartsKeyLocationEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(keyLocationChanged()), obj, method);
}

QVariant
CQChartsKeyLocationPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsKeyLocationEdit *edit = qobject_cast<CQChartsKeyLocationEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->keyLocation());
}

void
CQChartsKeyLocationPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsKeyLocationEdit *edit = qobject_cast<CQChartsKeyLocationEdit *>(w);
  assert(edit);

  CQChartsKeyLocation keyLocation = var.value<CQChartsKeyLocation>();

  edit->setKeyLocation(keyLocation);
}
