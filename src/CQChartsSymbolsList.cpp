#include <CQChartsSymbolsList.h>
#include <CQChartsSymbolSet.h>
#include <CQChartsPixelPaintDevice.h>
#include <CQChartsVariant.h>
#include <CQChartsMargin.h>
#include <CQCharts.h>

#include <CQIconButton.h>

#include <QItemDelegate>
#include <QToolButton>
#include <QPainter>

class CQChartsSymbolsItemDelegate : public QItemDelegate {
 public:
  CQChartsSymbolsItemDelegate(CQChartsSymbolsList *list) :
   QItemDelegate(list), list_(list) {
  }

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &ind) const override {
    painter->setRenderHints(QPainter::Antialiasing);

    if (ind.column() == 0) {
      auto *item = list_->item(ind.row());
      assert(item);

      auto symbol = list_->symbolAt(ind.row());

      QItemDelegate::drawBackground(painter, option, ind);

      int s = option.rect.height() - 2;

      CQChartsGeom::BBox bbox(option.rect.left()     + 2, option.rect.center().y() - s/2,
                              option.rect.left() + s + 2, option.rect.center().y() + s/2);

      CQChartsPixelPaintDevice device(painter);

      auto bg = list_->palette().window().color();
      auto fg = CQChartsUtil::bwColor(bg);

      painter->setPen(fg);

      if (symbol.isFilled()) {
        if (CQChartsUtil::grayValue(fg) < 128) // black
          painter->setBrush(QColor(100, 200, 100)); // Qt::green;
        else
          painter->setBrush(QColor(100, 100, 160));
      }
      else
        painter->setBrush(Qt::NoBrush);

      if (symbol.isValid())
        CQChartsDrawUtil::drawSymbol(&device, symbol, bbox);

      QRect trect(option.rect.left () + s + 4, option.rect.top() + 2,
                  option.rect.width() - s - 6, option.rect.height() - 4);

      QString name;

      if      (symbol.type() == CQChartsSymbol::Type::CHAR) {
        name = symbol.charName();
        if (name == "") name = "char";
      }
      else if (symbol.type() == CQChartsSymbol::Type::PATH) {
        name = symbol.pathName();
        if (name == "") name = "path";
      }
      else if (symbol.type() == CQChartsSymbol::Type::SVG) {
        name = symbol.svgName();
        if (name == "") name = "svg";
      }
      else
        name = symbol.toString();

      QItemDelegate::drawDisplay(painter, option, trect, name);
    }
    else
      QItemDelegate::paint(painter, option, ind);
  }

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind) const override {
    return QItemDelegate::sizeHint(option, ind);
  }

 private:
  CQChartsSymbolsList *list_ { nullptr };
};

//------

CQChartsSymbolsListControl::
CQChartsSymbolsListControl(CQCharts *charts) :
 QFrame()
{
  setObjectName("symbolsListControl");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  //---

  // Add symbols list
  symbolsList_ = new CQChartsSymbolsList(charts);

  connect(symbolsList_, SIGNAL(setNameChanged(const QString &)),
          this, SIGNAL(setNameChanged(const QString &)));
  connect(symbolsList_, SIGNAL(symbolChanged()), this, SIGNAL(symbolChanged()));

  layout->addWidget(symbolsList_);

  //---

  // control frame (move up/down buttons on right)
  auto *buttonsFrame  = CQUtil::makeWidget<QFrame>("buttonsFrame");
  auto *buttonsLayout = CQUtil::makeLayout<QVBoxLayout>(buttonsFrame, 2, 2);

  layout->addWidget(buttonsFrame);

  auto addToolButton = [&](const QString &name, const QString &iconName, const char *slotName) {
    auto *button = CQUtil::makeWidget<CQIconButton>(name);

    button->setIcon(iconName);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto *upButton   = addToolButton("up"  , "UP"  , SLOT(symbolUpSlot  ()));
  auto *downButton = addToolButton("down", "DOWN", SLOT(symbolDownSlot()));

  upButton   ->setToolTip("Move symbol up in list");
  downButton ->setToolTip("Move symbol down in list");

  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(upButton);
  buttonsLayout->addWidget(downButton);
  buttonsLayout->addStretch(1);
}

CQCharts *
CQChartsSymbolsListControl::
charts() const
{
  return symbolsList_->charts();
}

void
CQChartsSymbolsListControl::
setCharts(CQCharts *charts)
{
  symbolsList_->setCharts(charts);
}

const QString &
CQChartsSymbolsListControl::
setName() const
{
  return symbolsList_->setName();
}

void
CQChartsSymbolsListControl::
setSetName(const QString &s)
{
  symbolsList_->setSetName(s);
}

void
CQChartsSymbolsListControl::
setSetNameSlot(const QString &name)
{
  symbolsList_->setSetNameSlot(name);
}

const CQChartsSymbol &
CQChartsSymbolsListControl::
symbol() const
{
  return symbolsList_->symbol();
}

void
CQChartsSymbolsListControl::
setSymbol(const Symbol &s)
{
  symbolsList_->setSymbol(s);
}

void
CQChartsSymbolsListControl::
symbolUpSlot()
{
  symbolsList_->moveCurrentUp();
}

void
CQChartsSymbolsListControl::
symbolDownSlot()
{
  symbolsList_->moveCurrentDown();
}

//------

CQChartsSymbolsList::
CQChartsSymbolsList(CQCharts *charts)
{
  setObjectName("symbolsList");

  setToolTip("Symbols");

  delegate_ = new CQChartsSymbolsItemDelegate(this);

  setItemDelegate(delegate_);

  //---

  setCharts(charts);

  setSetName("all");

  //---

  connect(this, SIGNAL(itemSelectionChanged()),
          this, SLOT(selectionChangeSlot()));
}

void
CQChartsSymbolsList::
setCharts(CQCharts *charts)
{
  if (charts != charts_) {
    charts_ = charts;

    updateItems();
  }
}

void
CQChartsSymbolsList::
setSymbol(const Symbol &symbol)
{
  if (symbol != symbol_) {
    symbol_ = symbol;

    setCurrentSymbol();

    Q_EMIT symbolChanged();
  }
}

void
CQChartsSymbolsList::
setSetName(const QString &name)
{
  if (name != setName_) {
    setName_ = name;

    updateItems();

    Q_EMIT setNameChanged(setName_);
  }
}

void
CQChartsSymbolsList::
setSetNameSlot(const QString &name)
{
  setSetName(name);
}

void
CQChartsSymbolsList::
updateItems()
{
  auto *symbolSet = this->symbolSet();
  if (! symbolSet) return;

  //---

  auto createItem = [&](const Symbol &symbol, int i) {
    auto name = symbol.toString();

    auto *item = new QListWidgetItem(name);

    item->setToolTip(name);

    item->setData(Qt::UserRole, i);

    addItem(item);

    return item;
  };

  //---

  clear();

  int n = symbolSet->numSymbols();

  for (int i = 0; i < n; ++i) {
    auto symbol = symbolSet->symbol(i);

    createItem(symbol, i);
  }

  //---

  setCurrentSymbol();
}

void
CQChartsSymbolsList::
setCurrentSymbol()
{
  auto *symbolSet = this->symbolSet();
  if (! symbolSet) return;

  int n = symbolSet->numSymbols();

  for (int i = 0; i < n; ++i) {
    auto symbol = symbolSet->symbol(i);

    if (symbol == symbol_) {
      setCurrentRow(i);
      break;
    }
  }
}

CQChartsSymbol
CQChartsSymbolsList::
symbolAt(int ind) const
{
  auto *symbolSet = this->symbolSet();
  if (! symbolSet) return Symbol();

  return symbolSet->symbol(ind);
}

CQChartsSymbolSet *
CQChartsSymbolsList::
symbolSet() const
{
  if (! charts_)
    return nullptr;

  auto *symbolSetMgr = charts_->symbolSetMgr();

  return symbolSetMgr->symbolSet(setName_);
}

void
CQChartsSymbolsList::
moveCurrentUp()
{
  auto *item = currentItem();
  if (! item) return;

  auto *symbolSet = this->symbolSet();
  if (! symbolSet) return;

  bool ok;
  long i = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

  if (! ok || ! symbolSet->moveUp(int(i)))
    return;

  updateItems();

  setCurrentItem(this->item(int(i - 1)), QItemSelectionModel::Select);
}

void
CQChartsSymbolsList::
moveCurrentDown()
{
  auto *item = currentItem();
  if (! item) return;

  auto *symbolSet = this->symbolSet();
  if (! symbolSet) return;

  bool ok;
  long i = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

  if (! ok || ! symbolSet->moveDown(int(i)))
    return;

  updateItems();

  setCurrentItem(this->item(int(i + 1)), QItemSelectionModel::Select);
}

QListWidgetItem *
CQChartsSymbolsList::
currentItem() const
{
  auto selected = this->selectedItems();
  if (! selected.length()) return nullptr;

  return selected[0];
}

void
CQChartsSymbolsList::
selectionChangeSlot()
{
  Symbol symbol;

  (void) selectedSymbol(symbol);

  setSymbol(symbol);
}

bool
CQChartsSymbolsList::
selectedSymbol(Symbol &symbol) const
{
  auto selected = this->selectedItems();
  if (! selected.length()) return false;

  auto *item = selected[0];

  auto *symbolSet = this->symbolSet();
  if (! symbolSet) return false;

  bool ok;
  long i = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
  if (! ok) return false;

  symbol = symbolSet->symbol(int(i));

  return true;
}
