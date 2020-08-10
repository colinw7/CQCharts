#include <CQChartsSymbolEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsSymbolEdit::
CQChartsSymbolEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("symbol");

  setToolTip("Symbol Type");

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  combo_->addItems(CQChartsSymbol::typeNames());

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsSymbolEdit::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
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
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  bool ok;
  CQChartsSymbol symbolType = CQChartsVariant::toSymbol(value, ok);
  if (! ok) return;

  //---

  // draw symbol
  painter->save();

  int ss = std::max(option.rect.height()/2 - 2, 1);

  CQChartsGeom::BBox bbox1(option.rect.left()       , option.rect.center().y() - ss,
                           option.rect.left() + 2*ss, option.rect.center().y() + ss);

  painter->setClipRect(bbox1.adjusted(-1, -1, 1, 1).qrect());

  painter->setPen  (Qt::black);
  painter->setBrush(Qt::white);

  CQChartsPixelPaintDevice device(painter);

  CQChartsDrawUtil::drawSymbol(&device, symbolType, bbox1);

  painter->restore();

  int x = int(bbox1.getXMax() + 2);

  //--

  // draw symbol name
  QString str = symbolType.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 2*margin(), option1.rect.height());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsSymbolPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  CQChartsSymbol symbolType = CQChartsVariant::toSymbol(value, ok);
  if (! ok) return "";

  return symbolType.toString();
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
  auto *edit = new CQChartsSymbolEdit(parent);

  return edit;
}

void
CQChartsSymbolPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsSymbolEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(symbolChanged()), obj, method);
}

QVariant
CQChartsSymbolPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsSymbolEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromSymbol(edit->symbol());
}

void
CQChartsSymbolPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsSymbolEdit *>(w);
  assert(edit);

  bool ok;
  CQChartsSymbol symbol = CQChartsVariant::toSymbol(var, ok);
  if (! ok) return;

  edit->setSymbol(symbol);
}
