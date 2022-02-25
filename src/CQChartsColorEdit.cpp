#include <CQChartsColorEdit.h>
#include <CQColors.h>
#include <CQChartsObj.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <CQColorsEditModel.h>
#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
#include <CQIntegerSpin.h>
#include <CQColorEdit.h>
#include <CQCheckBox.h>

#include <QComboBox>
#include <QLabel>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>

CQChartsColorLineEdit::
CQChartsColorLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("colorLineEdit");

  setToolTip("Color");

  //---

  menuEdit_ = dataEdit_ = new CQChartsColorEdit;

  dataEdit_->setNoFocus();

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  colorToWidgets();
}

const CQChartsColor &
CQChartsColorLineEdit::
color() const
{
  return dataEdit_->color();
}

void
CQChartsColorLineEdit::
setColor(const Color &color)
{
  updateColor(color, /*updateText*/ true);
}

void
CQChartsColorLineEdit::
setNoFocus()
{
  dataEdit_->setNoFocus();
}

void
CQChartsColorLineEdit::
updateColor(const Color &color, bool updateText)
{
  connectSlots(false);

  dataEdit_->setColor(color);

  connectSlots(true);

  if (updateText)
    colorToWidgets();

  emit colorChanged();
}

void
CQChartsColorLineEdit::
textChanged()
{
  Color color;

  if (edit_->text().trimmed() != "") {
    color = Color(edit_->text());

    if (! color.isValid()) {
      colorToWidgets();
      return;
    }
  }

  updateColor(color, /*updateText*/false);
}

void
CQChartsColorLineEdit::
colorToWidgets()
{
  connectSlots(false);

  if (color().isValid())
    edit_->setText(color().colorStr());
  else
    edit_->setText("");

  auto tip = QString("%1 (%2)").arg(toolTip()).arg(color().colorStr());

  edit_->setToolTip(tip);

  connectSlots(true);
}

void
CQChartsColorLineEdit::
menuEditChanged()
{
  colorToWidgets();

  emit colorChanged();
}

void
CQChartsColorLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(colorChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsColorLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  auto c = (color().isValid() ? interpColor(color()) : palette().color(QPalette::Window));

  painter->fillRect(rect, QBrush(c));

  //---

  auto str = (color().isValid() ? color().colorStr() : "<none>");

  drawCenteredText(painter, str);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsColorPropertyViewType::
CQChartsColorPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsColorPropertyViewType::
getEditor() const
{
  return new CQChartsColorPropertyViewEditor;
}

bool
CQChartsColorPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsColorPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  bool ok;
  auto color = CQChartsVariant::toColor(value, ok);
  if (! ok) return;

  int x = option.rect.left();

  //---

  // draw color if can be directly determined
  if (color.isDirect()) {
    auto *obj = qobject_cast<CQChartsObj *>(item->object());

    if (obj) {
      auto rect = option.rect;

      rect.setWidth(option.rect.height());

      rect.adjust(0, 1, -3, -2);

      auto c = obj->charts()->interpColor(color, CQChartsUtil::ColorInd());

      painter->fillRect(rect, QBrush(c));

      painter->setPen(CQChartsUtil::bwColor(c));

      painter->drawRect(rect);

      x = rect.right() + 2;
    }
  }

  //---

  auto str = color.colorStr();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  auto option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 2*margin(), option1.rect.height());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsColorPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  auto color = CQChartsVariant::toColor(value, ok);
  if (! ok) return "";

  return color.colorStr();
}

//------

CQChartsColorPropertyViewEditor::
CQChartsColorPropertyViewEditor()
{
}

QWidget *
CQChartsColorPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsColorLineEdit(parent);

  return edit;
}

void
CQChartsColorPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsColorLineEdit *>(w);
  assert(edit);

  // TODO: why do we need direct connection for plot object to work ?
  QObject::connect(edit, SIGNAL(colorChanged()), obj, method, Qt::DirectConnection);
}

QVariant
CQChartsColorPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsColorLineEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromColor(edit->color());
}

void
CQChartsColorPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsColorLineEdit *>(w);
  assert(edit);

  bool ok;
  auto color = CQChartsVariant::toColor(var, ok);
  if (! ok) return;

  edit->setColor(color);
}

//------

CQChartsColorEdit::
CQChartsColorEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("colorEdit");

  //---

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 2, 2);

  //---

  int row = 0;

  auto addLabelWidget = [&](const QString &label, QWidget *edit) {
    auto *labelW = CQUtil::makeLabelWidget<QLabel>(label, "label");

    layout->addWidget(labelW, row, 0);
    layout->addWidget(edit  , row, 1);

    widgetLabels_[edit] = labelW;

    ++row;
  };

  auto addCheckWidget = [&](const QString &label, QWidget *edit) {
    auto *check = CQUtil::makeLabelWidget<QCheckBox>(label, "label");

    layout->addWidget(check, row, 0);
    layout->addWidget(edit , row, 1);

    widgetLabels_[edit] = check;

    ++row;
  };

  //---

  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(QStringList() << "None" <<
    "Palette" << "Indexed Palette" << "Interface" << "Contrast" <<
    "Model" << "Lighter" << "Darker" << "Color");

  typeCombo_->setToolTip("Color value type");

  addLabelWidget("Type", typeCombo_);

  //---

  indPalCombo_ = CQUtil::makeWidget<QComboBox>(this, "indPaletteCombo");

  indPalCombo_->addItems(QStringList() << "Index" << "Palette");

  indPalStack_ = CQUtil::makeWidget<QStackedWidget>(this, "indPaletteStack");

  //-

  indEdit_ = CQUtil::makeWidget<CQIntegerSpin>("indEdit");

  indEdit_->setRange(-1, 99);
  indEdit_->setToolTip("Palette index in theme (-1 is unset)");

  //-

  paletteEdit_ = CQUtil::makeWidget<QComboBox>("paletteCombo");

  QStringList paletteNames;

  CQColorsMgrInst->getPaletteNames(paletteNames);

  paletteEdit_->addItems(paletteNames);

  //-

  indPalStack_->addWidget(indEdit_);
  indPalStack_->addWidget(paletteEdit_);

  layout->addWidget(indPalCombo_, row, 0);
  layout->addWidget(indPalStack_, row, 1);

  widgetLabels_[indPalStack_] = indPalCombo_;

  ++row;

  //---

  rFrame_ = CQUtil::makeWidget<QFrame>("rFrame");

  auto *rLayout = CQUtil::makeLayout<QHBoxLayout>(rFrame_, 0, 2);

  rEdit_ = CQUtil::makeWidget<CQColorsEditModel>("rEdit");
  rNeg_  = CQUtil::makeLabelWidget<QCheckBox>("Negate", "negate");

  rEdit_->setToolTip("Red model number");
  rNeg_ ->setToolTip("Invert red model value");

  rLayout->addWidget(rEdit_);
  rLayout->addWidget(rNeg_);

  addLabelWidget("R", rFrame_);

  //---

  gFrame_ = CQUtil::makeWidget<QFrame>("gFrame");

  auto *gLayout = CQUtil::makeLayout<QHBoxLayout>(gFrame_, 0, 2);

  gEdit_ = CQUtil::makeWidget<CQColorsEditModel>("gEdit");
  gNeg_  = CQUtil::makeLabelWidget<QCheckBox>("Negate", "negate");

  gEdit_->setToolTip("Green model number");
  gNeg_ ->setToolTip("Invert green model value");

  gLayout->addWidget(gEdit_);
  gLayout->addWidget(gNeg_);

  addLabelWidget("G", gFrame_);

  //---

  bFrame_ = CQUtil::makeWidget<QFrame>("bFrame");

  auto *bLayout = CQUtil::makeLayout<QHBoxLayout>(bFrame_, 0, 2);

  bEdit_ = CQUtil::makeWidget<CQColorsEditModel>("bEdit");
  bNeg_  = CQUtil::makeLabelWidget<QCheckBox>("Negate", "negate");

  bEdit_->setToolTip("Blue model number");
  bNeg_ ->setToolTip("Invert blue model value");

  bLayout->addWidget(bEdit_);
  bLayout->addWidget(bNeg_);

  addLabelWidget("B", bFrame_);

  //---

  valueEdit_ = CQUtil::makeWidget<CQRealSpin>("valueEdit");

  valueEdit_->setToolTip("Palette, interface or contrast value (0-1 if not scaled)");

  addCheckWidget("Value", valueEdit_);

  valueCheck_ = qobject_cast<QCheckBox *>(widgetLabels_[valueEdit_]);

  //---

  colorEdit_ = CQUtil::makeWidget<CQColorEdit>("colorEdit");

  colorEdit_->setToolTip("Explicit color name");

  addLabelWidget("Color", colorEdit_);

  //---

  scaleCheck_ = CQUtil::makeWidget<CQCheckBox>("scaleCheck");

  scaleCheck_->setToolTip("Rescale value from palette x range");

  addLabelWidget("Scale", scaleCheck_);

  //---

  invertCheck_ = CQUtil::makeWidget<CQCheckBox>("invertCheck");

  invertCheck_->setToolTip("Invert palette color value");

  addLabelWidget("Invert", invertCheck_);

  //---

  layout->setRowStretch(row, 1);

  layout->setColumnStretch(2, 1);

  //---

  connectSlots(true);

  setFixedHeight(CQChartsColorEdit::minimumSizeHint().height());

  updateState();
}

void
CQChartsColorEdit::
setColor(const Color &color)
{
  color_ = color;

  colorToWidgets();

  updateState();

  emit colorChanged();
}

void
CQChartsColorEdit::
setNoFocus()
{
//colorEdit_->setNoFocus();

  typeCombo_  ->setFocusPolicy(Qt::NoFocus);
//indEdit_    ->setFocusPolicy(Qt::NoFocus);
//valueEdit_  ->setFocusPolicy(Qt::NoFocus);
  valueCheck_ ->setFocusPolicy(Qt::NoFocus);
  scaleCheck_ ->setFocusPolicy(Qt::NoFocus);
  invertCheck_->setFocusPolicy(Qt::NoFocus);
}

void
CQChartsColorEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(typeCombo_  , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(indPalCombo_, SIGNAL(currentIndexChanged(int)), SLOT(indPalSlot(int)));
  connectDisconnect(indEdit_    , SIGNAL(valueChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(paletteEdit_, SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(rEdit_      , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(rNeg_       , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(gEdit_      , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(gNeg_       , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(bEdit_      , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(bNeg_       , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(valueEdit_  , SIGNAL(valueChanged(double)), SLOT(widgetsToColor()));
  connectDisconnect(valueCheck_ , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(colorEdit_  , SIGNAL(colorChanged(const QColor &)), SLOT(widgetsToColor()));
  connectDisconnect(scaleCheck_ , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(invertCheck_, SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
}

void
CQChartsColorEdit::
colorToWidgets()
{
  connectSlots(false);

  if (color_.isValid()) {
    bool hasValue = false;

    if      (color_.type() == Color::Type::PALETTE ||
             color_.type() == Color::Type::PALETTE_VALUE) {
      typeCombo_->setCurrentIndex(1);

      if      (color_.hasPaletteIndex()) {
        indPalStack_->setCurrentIndex(0);
        indPalCombo_->setCurrentIndex(0);

        indEdit_ ->setValue(color_.ind());
      }
      else if (color_.hasPaletteName()) {
        indPalStack_->setCurrentIndex(1);
        indPalCombo_->setCurrentIndex(1);

        QString name;

        if (color_.getPaletteName(name))
          paletteEdit_->setCurrentIndex(paletteEdit_->findText(name));
        else
          paletteEdit_->setCurrentIndex(0);
      }
      else {
        indPalStack_->setCurrentIndex(0);
        indPalCombo_->setCurrentIndex(0);

        indEdit_ ->setValue(-1);
      }

      hasValue = (color_.type() == Color::Type::PALETTE_VALUE);

      if (hasValue)
        scaleCheck_->setChecked(color_.isScale());
      else
        scaleCheck_->setChecked(false);

      invertCheck_->setChecked(color_.isInvert());
    }
    else if (color_.type() == Color::Type::INDEXED ||
             color_.type() == Color::Type::INDEXED_VALUE) {
      typeCombo_->setCurrentIndex(2);

      if      (color_.hasPaletteIndex()) {
        indPalStack_->setCurrentIndex(0);
        indPalCombo_->setCurrentIndex(0);

        indEdit_ ->setValue(color_.ind());
      }
      else if (color_.hasPaletteName()) {
        indPalStack_->setCurrentIndex(1);
        indPalCombo_->setCurrentIndex(1);

        QString name;

        if (color_.getPaletteName(name))
          paletteEdit_->setCurrentIndex(paletteEdit_->findText(name));
        else
          paletteEdit_->setCurrentIndex(0);
      }
      else {
        indPalStack_->setCurrentIndex(0);
        indPalCombo_->setCurrentIndex(0);

        indEdit_ ->setValue(-1);
      }

      hasValue = (color_.type() == Color::Type::INDEXED_VALUE);

      invertCheck_->setChecked(color_.isInvert());
    }
    else if (color_.type() == Color::Type::INTERFACE ||
             color_.type() == Color::Type::INTERFACE_VALUE) {
      typeCombo_->setCurrentIndex(3);

      hasValue = (color_.type() == Color::Type::INTERFACE_VALUE);
    }
    else if (color_.type() == Color::Type::CONTRAST ||
             color_.type() == Color::Type::CONTRAST_VALUE) {
      typeCombo_->setCurrentIndex(4);

      hasValue = (color_.type() == Color::Type::INTERFACE_VALUE);
    }
    else if (color_.type() == Color::Type::MODEL ||
             color_.type() == Color::Type::MODEL_VALUE) {
      typeCombo_->setCurrentIndex(5);

      int r, g, b;

      color_.getModelRGB(r, g, b);

      rEdit_->setCurrentIndex(std::abs(r));
      gEdit_->setCurrentIndex(std::abs(g));
      bEdit_->setCurrentIndex(std::abs(b));

      rNeg_->setChecked(r < 0);
      gNeg_->setChecked(g < 0);
      bNeg_->setChecked(b < 0);

      hasValue = (color_.type() == Color::Type::MODEL_VALUE);
    }
    else if (color_.type() == Color::Type::LIGHTER ||
             color_.type() == Color::Type::LIGHTER_VALUE) {
      typeCombo_->setCurrentIndex(6);

      hasValue = (color_.type() == Color::Type::LIGHTER_VALUE);
    }
    else if (color_.type() == Color::Type::DARKER ||
             color_.type() == Color::Type::DARKER_VALUE) {
      typeCombo_->setCurrentIndex(7);

      hasValue = (color_.type() == Color::Type::DARKER_VALUE);
    }
    else if (color_.type() == Color::Type::COLOR) {
      typeCombo_->setCurrentIndex(8);

      colorEdit_->setColor(color_.color());
    }

    //---

    valueCheck_->setChecked(hasValue);

    if (hasValue)
      valueEdit_->setValue(color_.value());
    else
      valueEdit_->setValue(0.0);

    valueEdit_->setEnabled(valueCheck_->isChecked());
  }
  else {
    typeCombo_->setCurrentIndex(0);
  }

  connectSlots(true);
}

void
CQChartsColorEdit::
widgetsToColor()
{
  int typeInd = typeCombo_->currentIndex();

  Color color;

  if      (typeInd == 1) {
    if (valueCheck_->isChecked())
      color = Color::makePaletteValue();
    else
      color = Color::makePalette();

    if (indPalStack_->currentIndex() == 0)
      color.setInd(indEdit_->value());
    else
      color.setPaletteName(paletteEdit_->currentText());

    if (valueCheck_->isChecked()) {
      if (scaleCheck_->isChecked())
        color.setScaleValue(Color::Type::PALETTE_VALUE, valueEdit_->value(), true);
      else
        color = Color::makePaletteValue(valueEdit_->value());
    }

    if (invertCheck_->isChecked())
      color.setInvert(true);
  }
  else if (typeInd == 2) {
    if (valueCheck_->isChecked())
      color = Color(Color::Type::INDEXED_VALUE);
    else
      color = Color(Color::Type::INDEXED);

    if (indPalStack_->currentIndex() == 0)
      color.setInd(indEdit_->value());
    else
      color.setPaletteName(paletteEdit_->currentText());

    if (valueCheck_->isChecked())
      color.setValue(Color::Type::INDEXED_VALUE, valueEdit_->value());

    if (invertCheck_->isChecked())
      color.setInvert(true);
  }
  else if (typeInd == 3) {
    if (valueCheck_->isChecked())
      color = Color::makeInterfaceValue();
    else
      color = Color::makeInterface();

    if (valueCheck_->isChecked())
      color = Color::makeInterfaceValue(valueEdit_->value());
  }
  else if (typeInd == 4) {
    if (valueCheck_->isChecked())
      color = Color::makeContrastValue();
    else
      color = Color::makeContrast();

    if (valueCheck_->isChecked())
      color = Color::makeContrastValue(valueEdit_->value());
  }
  else if (typeInd == 5) {
    if (valueCheck_->isChecked())
      color = Color(Color::Type::MODEL_VALUE);
    else
      color = Color(Color::Type::MODEL);

    int r = rEdit_->currentIndex(); if (rNeg_->isChecked()) r = -r;
    int g = gEdit_->currentIndex(); if (gNeg_->isChecked()) g = -g;
    int b = bEdit_->currentIndex(); if (bNeg_->isChecked()) b = -b;

    color.setModelRGB(r, g, b);

    if (valueCheck_->isChecked())
      color.setValue(Color::Type::MODEL_VALUE, valueEdit_->value());
  }
  else if (typeInd == 6) {
    if (valueCheck_->isChecked())
      color = Color(Color::Type::LIGHTER_VALUE);
    else
      color = Color(Color::Type::LIGHTER);

    if (valueCheck_->isChecked())
      color.setValue(Color::Type::LIGHTER_VALUE, valueEdit_->value());
  }
  else if (typeInd == 7) {
    if (valueCheck_->isChecked())
      color = Color(Color::Type::DARKER_VALUE);
    else
      color = Color(Color::Type::DARKER);

    if (valueCheck_->isChecked())
      color.setValue(Color::Type::DARKER_VALUE, valueEdit_->value());
  }
  else if (typeInd == 8) {
    color = Color(Color::Type::COLOR);

    auto c = colorEdit_->color();

    if (c.isValid())
      color.setColor(c);
  }

  color_ = color;

  //---

  updateState();

  emit colorChanged();
}

void
CQChartsColorEdit::
indPalSlot(int ind)
{
  indPalStack_->setCurrentIndex(ind);
}

void
CQChartsColorEdit::
updateState()
{
  auto setEditVisible = [&](QWidget *w, bool visible) {
    w->setVisible(visible);
    w->setEnabled(visible);

    widgetLabels_[w]->setVisible(visible);
  };

  setEditVisible(indPalStack_, false);
  setEditVisible(rFrame_     , false);
  setEditVisible(gFrame_     , false);
  setEditVisible(bFrame_     , false);
  setEditVisible(valueEdit_  , false);
  setEditVisible(colorEdit_  , false);
  setEditVisible(scaleCheck_ , false);
  setEditVisible(invertCheck_, false);

  if (color_.isValid()) {
    if      (color_.type() == Color::Type::PALETTE ||
             color_.type() == Color::Type::PALETTE_VALUE) {
      setEditVisible(indPalStack_, true);
      setEditVisible(valueEdit_  , true);
      setEditVisible(invertCheck_, true);

      if (color_.type() == Color::Type::PALETTE_VALUE)
        setEditVisible(scaleCheck_, true);
    }
    else if (color_.type() == Color::Type::INDEXED ||
             color_.type() == Color::Type::INDEXED_VALUE) {
      setEditVisible(indPalStack_, true);
      setEditVisible(valueEdit_  , true);
      setEditVisible(invertCheck_, true);
    }
    else if (color_.type() == Color::Type::INTERFACE ||
             color_.type() == Color::Type::INTERFACE_VALUE) {
      setEditVisible(valueEdit_, true);
    }
    else if (color_.type() == Color::Type::CONTRAST ||
             color_.type() == Color::Type::CONTRAST_VALUE) {
      setEditVisible(valueEdit_, true);
    }
    else if (color_.type() == Color::Type::MODEL ||
             color_.type() == Color::Type::MODEL_VALUE) {
      setEditVisible(rFrame_   , true);
      setEditVisible(gFrame_   , true);
      setEditVisible(bFrame_   , true);
      setEditVisible(valueEdit_, true);
    }
    else if (color_.type() == Color::Type::LIGHTER ||
             color_.type() == Color::Type::LIGHTER_VALUE) {
      setEditVisible(valueEdit_, true);
    }
    else if (color_.type() == Color::Type::DARKER ||
             color_.type() == Color::Type::DARKER_VALUE) {
      setEditVisible(valueEdit_, true);
    }
    else if (color_.type() == Color::Type::COLOR) {
      setEditVisible(colorEdit_, true);
    }
  }

  valueEdit_->setEnabled(valueCheck_->isChecked());
}

QSize
CQChartsColorEdit::
sizeHint() const
{
  auto s1 = CQChartsEditBase::sizeHint();
  auto s2 = minimumSizeHint();

  return QSize(s1.width(), s2.height());
}

QSize
CQChartsColorEdit::
minimumSizeHint() const
{
  QFontMetrics fm(font());

  int eh = fm.height() + 4;

  return QSize(0, eh*8);
}
