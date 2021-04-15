#include <CQChartsSymbolEdit.h>
#include <CQChartsSymbolTypeEdit.h>
#include <CQChartsObj.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQCheckBox.h>

#include <QStackedWidget>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>

CQChartsSymbolLineEdit::
CQChartsSymbolLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("symbolLineEdit");

  setToolTip("Symbol");

  //---

  menuEdit_ = dataEdit_ = new CQChartsSymbolEdit;

  dataEdit_->setNoFocus();

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  symbolToWidgets();
}

const CQChartsSymbol &
CQChartsSymbolLineEdit::
symbol() const
{
  return dataEdit_->symbol();
}

void
CQChartsSymbolLineEdit::
setSymbol(const CQChartsSymbol &symbol)
{
  updateSymbol(symbol, /*updateText*/ true);
}

void
CQChartsSymbolLineEdit::
setNoFocus()
{
  dataEdit_->setNoFocus();
}

void
CQChartsSymbolLineEdit::
updateSymbol(const CQChartsSymbol &symbol, bool updateText)
{
  connectSlots(false);

  dataEdit_->setSymbol(symbol);

  connectSlots(true);

  if (updateText)
    symbolToWidgets();

  emit symbolChanged();
}

void
CQChartsSymbolLineEdit::
textChanged()
{
  CQChartsSymbol symbol;

  if (edit_->text().trimmed() != "") {
    symbol = CQChartsSymbol(edit_->text());

    if (! symbol.isValid()) {
      symbolToWidgets();
      return;
    }
  }

  updateSymbol(symbol, /*updateText*/false);
}

void
CQChartsSymbolLineEdit::
symbolToWidgets()
{
  connectSlots(false);

  if (symbol().isValid())
    edit_->setText(symbol().toString());
  else
    edit_->setText("");

  auto tip = QString("%1 (%2)").arg(symbol().toString());

  edit_->setToolTip(tip);

  connectSlots(true);
}

void
CQChartsSymbolLineEdit::
menuEditChanged()
{
  symbolToWidgets();

  emit symbolChanged();
}

void
CQChartsSymbolLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(symbolChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsSymbolLineEdit::
drawPreview(QPainter *painter, const QRect & /*rect*/)
{
  auto str = (symbol().isValid() ? symbol().toString() : "<none>");

  drawCenteredText(painter, str);
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
draw(CQPropertyViewItem * /*item*/, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  bool ok;
  auto symbol = CQChartsVariant::toSymbol(value, ok);
  if (! ok) return;

  int x = option.rect.left();

  //---

  auto str = symbol.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  auto option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 2*margin(), option1.rect.height());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsSymbolPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  auto symbol = CQChartsVariant::toSymbol(value, ok);
  if (! ok) return "";

  return symbol.toString();
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
  auto *edit = new CQChartsSymbolLineEdit(parent);

  return edit;
}

void
CQChartsSymbolPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsSymbolLineEdit *>(w);
  assert(edit);

  // TODO: why do we need direct connection for plot object to work ?
  QObject::connect(edit, SIGNAL(symbolChanged()), obj, method, Qt::DirectConnection);
}

QVariant
CQChartsSymbolPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsSymbolLineEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromSymbol(edit->symbol());
}

void
CQChartsSymbolPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsSymbolLineEdit *>(w);
  assert(edit);

  bool ok;
  auto symbol = CQChartsVariant::toSymbol(var, ok);
  if (! ok) return;

  edit->setSymbol(symbol);
}

//------

CQChartsSymbolEdit::
CQChartsSymbolEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("symbolEdit");

  //---

  auto addGridLabelWidget = [&](QGridLayout *grid, const QString &label, QWidget *edit, int &r) {
    auto *labelW = CQUtil::makeLabelWidget<QLabel>(label, "label");

    grid->addWidget(labelW, r, 0);
    grid->addWidget(edit  , r, 1);

    ++r;
  };

  auto addLayoutStretch = [&](QGridLayout *grid, int r) {
    grid->setRowStretch(r, 1);
    grid->setColumnStretch(2, 1);
  };

  //---

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 2, 2);
  int   row    = 0;

  auto addLabelWidget = [&](const QString &label, QWidget *edit) {
    addGridLabelWidget(layout, label, edit, row);
  };

  //---

  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(QStringList() << "None" << "Symbol" << "Character" << "Path" << "SVG");

  typeCombo_->setToolTip("Symbol value type");

  addLabelWidget("Type", typeCombo_);

  //---

  stack_ = CQUtil::makeWidget<QStackedWidget>("stack");

  layout->addWidget(stack_, row, 0, 1, 2); ++row;

  //---

  auto *noneFrame = CQUtil::makeWidget<QFrame>("noneFrame");

  stack_->addWidget(noneFrame);

  //---

  auto *typeFrame  = CQUtil::makeWidget<QFrame>("typeFrame");
  auto *typeLayout = CQUtil::makeLayout<QGridLayout>(typeFrame);

  stack_->addWidget(typeFrame);

  int typeRow = 0;

  typeEdit_ = CQUtil::makeWidget<CQChartsSymbolTypeEdit>(this, "type");

  addGridLabelWidget(typeLayout, "Symbol", typeEdit_, typeRow);

  addLayoutStretch(typeLayout, typeRow);

  //---

  auto *charFrame  = CQUtil::makeWidget<QFrame>("charFrame");
  auto *charLayout = CQUtil::makeLayout<QGridLayout>(charFrame);

  stack_->addWidget(charFrame);

  int charRow = 0;

  charEdit_     = CQUtil::makeWidget<QLineEdit>(this, "char");
  charNameEdit_ = CQUtil::makeWidget<QLineEdit>(this, "name");

  addGridLabelWidget(charLayout, "Char", charEdit_    , charRow);
  addGridLabelWidget(charLayout, "Name", charNameEdit_, typeRow);

  addLayoutStretch(charLayout, charRow);

  //---

  auto *pathFrame  = CQUtil::makeWidget<QFrame>("pathFrame");
  auto *pathLayout = CQUtil::makeLayout<QGridLayout>(pathFrame);

  stack_->addWidget(pathFrame);

  int pathRow = 0;

  pathCombo_   = CQUtil::makeWidget<QComboBox>(this, "path");
  pathSrcEdit_ = CQUtil::makeWidget<QLineEdit>(this, "pathSrc");

  pathCombo_->addItems(CQChartsSymbol::pathNames());

  addGridLabelWidget(pathLayout, "Path"  , pathCombo_  , pathRow);
  addGridLabelWidget(pathLayout, "Source", pathSrcEdit_, pathRow);

  addLayoutStretch(pathLayout, pathRow);

  //---

  auto *svgFrame  = CQUtil::makeWidget<QFrame>("svgFrame");
  auto *svgLayout = CQUtil::makeLayout<QGridLayout>(svgFrame);

  stack_->addWidget(svgFrame);

  int svgRow = 0;

  svgCombo_   = CQUtil::makeWidget<QComboBox>(this, "svg");
  svgSrcEdit_ = CQUtil::makeWidget<QLineEdit>(this, "svgSrc");

  svgCombo_->addItems(CQChartsSymbol::svgNames());

  addGridLabelWidget(svgLayout, "SVG"   , svgCombo_  , svgRow);
  addGridLabelWidget(svgLayout, "Source", svgSrcEdit_, svgRow);

  addLayoutStretch(svgLayout, svgRow);

  //---

  filledCheck_  = CQUtil::makeWidget<CQCheckBox>("filledCheck");
  strokedCheck_ = CQUtil::makeWidget<CQCheckBox>("strokedCheck");

  addLabelWidget("Filled" , filledCheck_ );
  addLabelWidget("Stroked", strokedCheck_);

  //---

  addLayoutStretch(layout, row);

  //---

  connectSlots(true);

  setFixedHeight(CQChartsSymbolEdit::minimumSizeHint().height());

  updateState();
}

void
CQChartsSymbolEdit::
setSymbol(const CQChartsSymbol &symbol)
{
  symbol_ = symbol;

  symbolToWidgets();

  updateState();

  emit symbolChanged();
}

void
CQChartsSymbolEdit::
setNoFocus()
{
//symbolEdit_->setNoFocus();

  typeEdit_    ->setFocusPolicy(Qt::NoFocus);
//charEdit_    ->setFocusPolicy(Qt::NoFocus);
//charNameEdit_->setFocusPolicy(Qt::NoFocus);
  pathCombo_   ->setFocusPolicy(Qt::NoFocus);
//pathSrcEdit_ ->setFocusPolicy(Qt::NoFocus);
  svgCombo_    ->setFocusPolicy(Qt::NoFocus);
//svgSrcEdit_  ->setFocusPolicy(Qt::NoFocus);
  filledCheck_ ->setFocusPolicy(Qt::NoFocus);
  strokedCheck_->setFocusPolicy(Qt::NoFocus);
}

void
CQChartsSymbolEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(typeCombo_   , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToSymbol()));
  connectDisconnect(typeEdit_    , SIGNAL(symbolChanged()), SLOT(widgetsToSymbol()));
  connectDisconnect(charEdit_    , SIGNAL(editingFinished()), SLOT(widgetsToSymbol()));
  connectDisconnect(charNameEdit_, SIGNAL(editingFinished()), SLOT(widgetsToSymbol()));
  connectDisconnect(pathCombo_   , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToSymbol()));
  connectDisconnect(pathSrcEdit_ , SIGNAL(editingFinished()), SLOT(widgetsToSymbol()));
  connectDisconnect(svgCombo_    , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToSymbol()));
  connectDisconnect(svgSrcEdit_  , SIGNAL(editingFinished()), SLOT(widgetsToSymbol()));
  connectDisconnect(filledCheck_ , SIGNAL(stateChanged(int)), SLOT(widgetsToSymbol()));
  connectDisconnect(strokedCheck_, SIGNAL(stateChanged(int)), SLOT(widgetsToSymbol()));
}

void
CQChartsSymbolEdit::
symbolToWidgets()
{
  connectSlots(false);

  if (symbol_.isValid()) {
    if      (symbol_.type() == CQChartsSymbol::Type::CHAR) {
      typeCombo_->setCurrentIndex(2);

      charEdit_    ->setText(symbol_.charStr ());
      charNameEdit_->setText(symbol_.charName());
    }
    else if (symbol_.type() == CQChartsSymbol::Type::PATH) {
      typeCombo_->setCurrentIndex(3);

      pathCombo_  ->setCurrentIndex(pathCombo_->findText(symbol_.pathName()));
      pathSrcEdit_->setText(symbol_.srcStr());
    }
    else if (symbol_.type() == CQChartsSymbol::Type::SVG) {
      typeCombo_->setCurrentIndex(4);

      svgCombo_  ->setCurrentIndex(svgCombo_->findText(symbol_.svgName()));
      svgSrcEdit_->setText(symbol_.srcStr());
    }
    else {
      typeCombo_->setCurrentIndex(1);

      typeEdit_->setSymbolType(symbol_.symbolType());
    }
  }
  else {
    typeCombo_->setCurrentIndex(0);
  }

  filledCheck_ ->setChecked(symbol_.isFilled ());
  strokedCheck_->setChecked(symbol_.isStroked());

  stack_->setCurrentIndex(typeCombo_->currentIndex());

  connectSlots(true);
}

void
CQChartsSymbolEdit::
widgetsToSymbol()
{
  connectSlots(false);

  int typeInd = typeCombo_->currentIndex();

  stack_->setCurrentIndex(typeInd);

  CQChartsSymbol symbol;

  if      (typeInd == 1) {
    symbol = CQChartsSymbol(typeEdit_->symbolType());
  }
  else if (typeInd == 2) {
    CQChartsSymbol::CharData charData;

    charData.c    = charEdit_    ->text();
    charData.name = charNameEdit_->text();

    symbol = CQChartsSymbol(charData);
  }
  else if (typeInd == 3) {
    CQChartsSymbol::PathData pathData;

    pathData.name = pathCombo_  ->currentText();
    pathData.src  = pathSrcEdit_->text(); // read only - ignored

    symbol = CQChartsSymbol(pathData);

    pathSrcEdit_->setText(symbol.srcStr());
  }
  else if (typeInd == 4) {
    CQChartsSymbol::SVGData svgData;

    svgData.name = svgCombo_  ->currentText();
    svgData.src  = svgSrcEdit_->text(); // read only - ignored

    symbol = CQChartsSymbol(svgData);

    svgSrcEdit_->setText(symbol.srcStr());
  }

  // can't uncheck both
  if (! filledCheck_->isChecked() && ! strokedCheck_->isChecked())
    filledCheck_->setChecked(true);

  symbol.setFilled (filledCheck_ ->isChecked());
  symbol.setStroked(strokedCheck_->isChecked());

  symbol_ = symbol;

  //---

  updateState();

  //--

  connectSlots(true);

  emit symbolChanged();
}

void
CQChartsSymbolEdit::
updateState()
{
}

QSize
CQChartsSymbolEdit::
sizeHint() const
{
  auto s1 = CQChartsEditBase::sizeHint();
  auto s2 = minimumSizeHint();

  return QSize(s1.width(), s2.height());
}

QSize
CQChartsSymbolEdit::
minimumSizeHint() const
{
  QFontMetrics fm(font());

  int eh = fm.height() + 4;

  return QSize(0, eh*8);
}
