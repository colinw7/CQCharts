#include <CQChartsFontEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsBoxObj.h>
#include <CQChartsUtil.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
#include <CQFontEdit.h>
#include <CQCheckBox.h>

#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

CQChartsFontLineEdit::
CQChartsFontLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("fontLineEdit");

  setToolTip("Font");

  //---

  menuEdit_ = dataEdit_ = new CQChartsFontEdit;

  dataEdit_->setNoFocus();

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  fontToWidgets();
}

const CQChartsFont &
CQChartsFontLineEdit::
font() const
{
  return dataEdit_->font();
}

void
CQChartsFontLineEdit::
setFont(const CQChartsFont &font)
{
  updateFont(font, /*updateText*/ true);
}

void
CQChartsFontLineEdit::
setNoFocus()
{
  dataEdit_->setNoFocus();
}

void
CQChartsFontLineEdit::
updateFont(const CQChartsFont &font, bool updateText)
{
  connectSlots(false);

  dataEdit_->setFont(font);

  connectSlots(true);

  if (updateText)
    fontToWidgets();

  emit fontChanged();
}

void
CQChartsFontLineEdit::
textChanged()
{
  CQChartsFont font(edit_->text());

  if (! font.isValid())
    return;

  updateFont(font, /*updateText*/ false);
}

void
CQChartsFontLineEdit::
fontToWidgets()
{
  connectSlots(false);

  if (font().isValid())
    edit_->setText(font().toString());
  else
    edit_->setText("");

  setToolTip(font().toString());

  connectSlots(true);
}

void
CQChartsFontLineEdit::
menuEditChanged()
{
  fontToWidgets();

  emit fontChanged();
}

void
CQChartsFontLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(fontChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsFontLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  QColor c = palette().color(QPalette::Window);

  painter->fillRect(rect, QBrush(c));

  //---

  QString str = (font().isValid() ? font().toString() : "<none>");

  painter->setFont(font().calcFont());

  drawCenteredText(painter, str);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsFontPropertyViewType::
CQChartsFontPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsFontPropertyViewType::
getEditor() const
{
  return new CQChartsFontPropertyViewEditor;
}

bool
CQChartsFontPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsFontPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  bool ok;
  CQChartsFont font = CQChartsVariant::toFont(value, ok);
  if (! ok) return;

  //---

  QString str = font.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);

  //---

  auto *view = qobject_cast<CQChartsView   *>(item->object());
  auto *plot = qobject_cast<CQChartsPlot   *>(item->object());
  auto *box  = qobject_cast<CQChartsBoxObj *>(item->object());

  if (box) {
    plot = box->plot();
    view = box->view();
  }

  //---

  int x = option1.rect.right();

  int w1 = option.rect.width() - w - 8;

  if (w1 > 0) {
    painter->save();

    option1.rect = QRect(x, option.rect.top(), w1, option.rect.height());

    CQChartsTextOptions textOptions;

    textOptions.scaled  = true;
    textOptions.align   = Qt::AlignLeft | Qt::AlignVCenter;
    textOptions.clipped = true;

    if      (plot)
      painter->setFont(plot->view()->plotFont(plot, font));
    else if (view)
      painter->setFont(view->viewFont(font));
    else
      painter->setFont(font.font());

    CQChartsPixelPaintDevice device(painter);

    CQChartsDrawUtil::drawTextInBox(&device, CQChartsGeom::BBox(option1.rect),
                                    "Abc", textOptions);

    painter->restore();
  }
}

QString
CQChartsFontPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  CQChartsFont font = CQChartsVariant::toFont(value, ok);
  if (! ok) return "";

  return font.toString();
}

//------

CQChartsFontPropertyViewEditor::
CQChartsFontPropertyViewEditor()
{
}

QWidget *
CQChartsFontPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsFontLineEdit(parent);

  return edit;
}

void
CQChartsFontPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsFontLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fontChanged()), obj, method);
}

QVariant
CQChartsFontPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsFontLineEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromFont(edit->font());
}

void
CQChartsFontPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsFontLineEdit *>(w);
  assert(edit);

  bool ok;
  CQChartsFont font = CQChartsVariant::toFont(var, ok);
  if (! ok) return;

  edit->setFont(font);
}

//------

CQChartsFontEdit::
CQChartsFontEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("fontEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(QStringList() << "Font" << "Inherited");

  typeCombo_->setToolTip("Font value type");

  layout->addWidget(typeCombo_);

  //---

  auto *fontLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  auto *fontLabel = CQUtil::makeLabelWidget<QLabel>("Font", "fontLabel");

  fontEdit_ = CQUtil::makeWidget<CQFontEdit>("fontEdit");

  fontEdit_->setToolTip("Font name");

  fontLayout->addWidget(fontLabel);
  fontLayout->addWidget(fontEdit_);

  layout->addLayout(fontLayout);

  //----

  int inheritedRow = 0;

  auto *inheritedLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  auto addInheritedWidget = [&](const QString &label, QWidget *w) {
    CQChartsWidgetUtil::addGridLabelWidget(inheritedLayout, label, w, inheritedRow);
  };

  //--

  normalCheck_ = CQUtil::makeWidget<CQCheckBox>("normalCheck");

  normalCheck_->setToolTip("Reset style to normal");

  addInheritedWidget("Normal", normalCheck_);

  //--

  boldCheck_ = CQUtil::makeWidget<CQCheckBox>("boldCheck");

  boldCheck_->setToolTip("Add bold style");

  addInheritedWidget("Bold", boldCheck_);

  //--

  italicCheck_ = CQUtil::makeWidget<CQCheckBox>("italicCheck");

  italicCheck_->setToolTip("Add italic style");

  addInheritedWidget("Italic", italicCheck_);

  //--

  sizeTypeCombo_ = CQUtil::makeWidget<QComboBox>("sizeTypeCombo");

  sizeTypeCombo_->addItems(QStringList() << "None" << "Explicit" << "Increment" << "Decrement");

  sizeTypeCombo_->setToolTip("Size modifier type");

  addInheritedWidget("Size Type", sizeTypeCombo_);

  //--

  sizeEdit_ = CQUtil::makeWidget<CQRealSpin>("sizeEdit");

  sizeEdit_->setToolTip("Size value or delta");

  addInheritedWidget("Size", sizeEdit_);

  //--

  layout->addLayout(inheritedLayout);

  //---

  connectSlots(true);

  updateState();
}

const CQChartsFont &
CQChartsFontEdit::
font() const
{
  return font_;
}

void
CQChartsFontEdit::
setFont(const CQChartsFont &font)
{
  font_ = font;

  fontToWidgets();

  updateState();

  emit fontChanged();
}

void
CQChartsFontEdit::
setNoFocus()
{
//fontEdit_->setNoFocus();

  typeCombo_  ->setFocusPolicy(Qt::NoFocus);
  boldCheck_  ->setFocusPolicy(Qt::NoFocus);
  italicCheck_->setFocusPolicy(Qt::NoFocus);
}

void
CQChartsFontEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(b, w, from, this, to);
  };

  connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)), SLOT(widgetsToFont()));
  connectDisconnect(b, fontEdit_, SIGNAL(fontChanged(const QFont &)), SLOT(widgetsToFont()));
  connectDisconnect(b, normalCheck_, SIGNAL(stateChanged(int)), SLOT(widgetsToFont()));
  connectDisconnect(b, boldCheck_, SIGNAL(stateChanged(int)), SLOT(widgetsToFont()));
  connectDisconnect(b, italicCheck_, SIGNAL(stateChanged(int)), SLOT(widgetsToFont()));
  connectDisconnect(b, sizeTypeCombo_, SIGNAL(currentIndexChanged(int)), SLOT(widgetsToFont()));
  connectDisconnect(b, sizeEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToFont()));
}

void
CQChartsFontEdit::
fontToWidgets()
{
  connectSlots(false);

  if (font_.isValid()) {
    if      (font_.type() == CQChartsFont::Type::FONT) {
      typeCombo_->setCurrentIndex(0);
    }
    else if (font_.type() == CQChartsFont::Type::INHERITED) {
      typeCombo_->setCurrentIndex(1);

      normalCheck_->setChecked(font_.data().normal);
      boldCheck_  ->setChecked(font_.data().bold  );
      italicCheck_->setChecked(font_.data().italic);
      sizeEdit_   ->setValue  (font_.data().size);

      if      (font_.data().sizeType == CQChartsFont::SizeType::NONE)
        sizeTypeCombo_->setCurrentIndex(0);
      else if (font_.data().sizeType == CQChartsFont::SizeType::EXPLICIT)
        sizeTypeCombo_->setCurrentIndex(1);
      else if (font_.data().sizeType == CQChartsFont::SizeType::INCREMENT)
        sizeTypeCombo_->setCurrentIndex(2);
      else if (font_.data().sizeType == CQChartsFont::SizeType::DECREMENT)
        sizeTypeCombo_->setCurrentIndex(3);
    }

    fontEdit_->setFont(font_.font());
  }
  else {
    QFont font;

    fontEdit_->setFont(font);

    typeCombo_->setCurrentIndex(0);
  }

  connectSlots(true);
}

void
CQChartsFontEdit::
widgetsToFont()
{
  int typeInd = typeCombo_->currentIndex();

  CQChartsFont font;

  if      (typeInd == 0) {
    font = CQChartsFont(fontEdit_->font());
  }
  else if (typeInd == 1) {
    CQChartsFont::InheritData inheritData;

    inheritData.normal = normalCheck_->isChecked();
    inheritData.bold   = boldCheck_  ->isChecked();
    inheritData.italic = italicCheck_->isChecked();
    inheritData.size   = sizeEdit_   ->value();

    if      (sizeTypeCombo_->currentIndex() == 0)
      inheritData.sizeType = CQChartsFont::SizeType::NONE;
    else if (sizeTypeCombo_->currentIndex() == 1)
      inheritData.sizeType = CQChartsFont::SizeType::EXPLICIT;
    else if (sizeTypeCombo_->currentIndex() == 2)
      inheritData.sizeType = CQChartsFont::SizeType::INCREMENT;
    else if (sizeTypeCombo_->currentIndex() == 3)
      inheritData.sizeType = CQChartsFont::SizeType::DECREMENT;

    font = CQChartsFont(inheritData);
  }

  font_ = font;

  //---

  updateState();

  emit fontChanged();
}

void
CQChartsFontEdit::
updateState()
{
  fontEdit_     ->setEnabled(false);
  normalCheck_  ->setEnabled(false);
  boldCheck_    ->setEnabled(false);
  italicCheck_  ->setEnabled(false);
  sizeTypeCombo_->setEnabled(false);
  sizeEdit_     ->setEnabled(false);

  if (font_.isValid()) {
    if      (font_.type() == CQChartsFont::Type::FONT) {
      fontEdit_->setEnabled(true);
    }
    else if (font_.type() == CQChartsFont::Type::INHERITED) {
      normalCheck_  ->setEnabled(true);
      boldCheck_    ->setEnabled(true);
      italicCheck_  ->setEnabled(true);
      sizeTypeCombo_->setEnabled(true);
      sizeEdit_     ->setEnabled(true);
    }
  }
}
