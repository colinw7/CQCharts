#include <CQChartsSymbolEdit.h>

#include <CQPropertyView.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsSymbolEdit::
CQChartsSymbolEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("symbol");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  combo_ = new QComboBox;

  combo_->addItems(CQChartsSymbol::typeNames());

  combo_->setObjectName("combo");

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsSymbolEdit::
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

const CQChartsSymbol &
CQChartsSymbolEdit::
symbol() const
{
  return symbol_;
}

void
CQChartsSymbolEdit::
setSymbol(const CQChartsSymbol &symbol)
{
  connectSlots(false);

  symbol_ = symbol;

  combo_->setCurrentIndex(combo_->findText(symbol_.toString()));

  connectSlots(true);
}

void
CQChartsSymbolEdit::
comboChanged()
{
  connectSlots(false);

  symbol_ = CQChartsSymbol(combo_->currentText());

  connectSlots(true);

  emit symbolChanged();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsSymbolPropertyViewType::
CQChartsSymbolPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsSymbolPropertyViewType::
getEditor() const
{
  return new CQChartsSymbolPropertyViewEditor;
}

bool
CQChartsSymbolPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsSymbolPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsSymbol symbol = value.value<CQChartsSymbol>();

  QString str = symbol.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsSymbolPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsSymbol>().toString();

  return str;
}

//------

CQChartsSymbolPropertyViewEditor::
CQChartsSymbolPropertyViewEditor()
{
}

QWidget *
CQChartsSymbolPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsSymbolEdit *edit = new CQChartsSymbolEdit(parent);

  return edit;
}

void
CQChartsSymbolPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsSymbolEdit *edit = qobject_cast<CQChartsSymbolEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(symbolChanged()), obj, method);
}

QVariant
CQChartsSymbolPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsSymbolEdit *edit = qobject_cast<CQChartsSymbolEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->symbol());
}

void
CQChartsSymbolPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsSymbolEdit *edit = qobject_cast<CQChartsSymbolEdit *>(w);
  assert(edit);

  CQChartsSymbol symbol = var.value<CQChartsSymbol>();

  edit->setSymbol(symbol);
}
