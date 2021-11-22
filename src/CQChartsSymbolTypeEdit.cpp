#include <CQChartsSymbolTypeEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsVariant.h>

//#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsSymbolTypeEdit::
CQChartsSymbolTypeEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("symbol");

  setToolTip("Symbol Type");

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  combo_->addItem("<none>");

  combo_->addItems(CQChartsSymbolType::typeNames());

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsSymbolTypeEdit::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

const CQChartsSymbolType &
CQChartsSymbolTypeEdit::
symbolType() const
{
  return symbolType_;
}

void
CQChartsSymbolTypeEdit::
setSymbolType(const CQChartsSymbolType &symbolType)
{
  connectSlots(false);

  symbolType_ = symbolType;

  if (symbolType_.isValid())
    combo_->setCurrentIndex(combo_->findText(symbolType_.toString()));
  else
    combo_->setCurrentIndex(0);

  connectSlots(true);
}

void
CQChartsSymbolTypeEdit::
comboChanged()
{
  connectSlots(false);

  if (combo_->currentIndex() > 0)
    symbolType_ = CQChartsSymbolType(combo_->currentText());
  else
    symbolType_ = CQChartsSymbolType();

  connectSlots(true);

  emit symbolChanged();
}

//------

#if 0
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsSymbolTypePropertyViewType::
CQChartsSymbolTypePropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsSymbolTypePropertyViewType::
getEditor() const
{
  return new CQChartsSymbolTypePropertyViewEditor;
}

bool
CQChartsSymbolTypePropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsSymbolTypePropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  bool ok;
  auto symbol = CQChartsVariant::toSymbol(value, ok);
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

  if (symbol.isValid())
    CQChartsDrawUtil::drawSymbol(&device, symbol, bbox1);

  painter->restore();

  int x = int(bbox1.getXMax() + 2);

  //--

  // draw symbol name
  auto str = symbol.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  auto option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 2*margin(), option1.rect.height());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsSymbolTypePropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  auto symbol = CQChartsVariant::toSymbol(value, ok);
  if (! ok) return "";

  return symbol.toString();
}

//------

CQChartsSymbolTypePropertyViewEditor::
CQChartsSymbolTypePropertyViewEditor()
{
}

QWidget *
CQChartsSymbolTypePropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsSymbolTypeEdit(parent);

  return edit;
}

void
CQChartsSymbolTypePropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsSymbolTypeEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(symbolChanged()), obj, method);
}

QVariant
CQChartsSymbolTypePropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsSymbolTypeEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromSymbol(CQChartsSymbol(edit->symbolType()));
}

void
CQChartsSymbolTypePropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsSymbolTypeEdit *>(w);
  assert(edit);

  bool ok;
  auto symbol = CQChartsVariant::toSymbol(var, ok);
  if (! ok) return;

  edit->setSymbolType(symbol.symbolType());
}
#endif
