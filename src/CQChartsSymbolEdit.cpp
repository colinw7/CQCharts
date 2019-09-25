#include <CQChartsSymbolEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsSymbolEdit::
CQChartsSymbolEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("symbol");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  combo_->addItems(CQChartsSymbol::typeNames());

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
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsSymbol symbolType = value.value<CQChartsSymbol>();

  //---

  // draw symbol
  painter->save();

  int ss = std::max(option.rect.height()/2 - 2, 1);

  QRect rect1(option.rect.left(), option.rect.center().y() - ss, 2*ss, 2*ss);

  painter->setClipRect(rect1.adjusted(-1, -1, 1, 1));

  painter->setPen  (Qt::black);
  painter->setBrush(Qt::white);

  CQChartsPixelPainter device(painter);

  CQChartsDrawUtil::drawSymbol(&device, symbolType, rect1);

  painter->restore();

  int x = rect1.right() + 2;

  //--

  // draw symbol name
  QString str = symbolType.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 8, option1.rect.height());

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
