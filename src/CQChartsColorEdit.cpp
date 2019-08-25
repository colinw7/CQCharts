#include <CQChartsColorEdit.h>
#include <CQColors.h>
#include <CQChartsObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <CQColorsEditModel.h>
#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
#include <CQColorEdit.h>
#include <CQCheckBox.h>

#include <QComboBox>
#include <QSpinBox>
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
setColor(const CQChartsColor &color)
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
updateColor(const CQChartsColor &color, bool updateText)
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
  CQChartsColor color(edit_->text());

  if (! color.isValid())
    return;

  updateColor(color, /*updateText*/ false);
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

  setToolTip(color().colorStr());

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
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(colorChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(colorChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsColorLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  QColor c = (color().isValid() ? interpColor(color()) : palette().color(QPalette::Window));

  painter->fillRect(rect, QBrush(c));

  //---

  QString str = (color().isValid() ? color().colorStr() : "<none>");

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
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsColor color = value.value<CQChartsColor>();

  int x = option.rect.left();

  //---

  // draw color if can be directly determined
  if (color.isDirect()) {
    CQChartsObj *obj = qobject_cast<CQChartsObj *>(item->object());

    if (obj) {
      QRect rect = option.rect;

      rect.setWidth(option.rect.height());

      rect.adjust(0, 1, -3, -2);

      QColor c = obj->charts()->interpColor(color, CQChartsUtil::ColorInd());

      painter->fillRect(rect, QBrush(c));

      painter->setPen(CQChartsUtil::bwColor(c));

      painter->drawRect(rect);

      x = rect.right() + 2;
    }
  }

  //---

  QString str = color.colorStr();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  QStyleOptionViewItem option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 8, option1.rect.height());

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsColorPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsColor>().colorStr();

  return str;
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
  CQChartsColorLineEdit *edit = new CQChartsColorLineEdit(parent);

  return edit;
}

void
CQChartsColorPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsColorLineEdit *edit = qobject_cast<CQChartsColorLineEdit *>(w);
  assert(edit);

  // TODO: why do we need direct connection for plot object to work ?
  QObject::connect(edit, SIGNAL(colorChanged()), obj, method, Qt::DirectConnection);
}

QVariant
CQChartsColorPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsColorLineEdit *edit = qobject_cast<CQChartsColorLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->color());
}

void
CQChartsColorPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsColorLineEdit *edit = qobject_cast<CQChartsColorLineEdit *>(w);
  assert(edit);

  CQChartsColor color = var.value<CQChartsColor>();

  edit->setColor(color);
}

//------

CQChartsColorEdit::
CQChartsColorEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("colorEdit");

  //---

  QGridLayout *layout = CQUtil::makeLayout<QGridLayout>(this, 2, 2);

  //---

  int row = 0;

  auto addLabelWidget = [&](const QString &label, QWidget *edit) {
    QLabel *labelW = CQUtil::makeLabelWidget<QLabel>(label, "label");

    layout->addWidget(labelW, row, 0);
    layout->addWidget(edit  , row, 1);

    widgetLabels_[edit] = labelW;

    ++row;
  };

  //---

  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(QStringList() <<
    "None" << "Palette" << "Palette Value" << "Indexed Palette" << "Indexed Palette Value" <<
    "Interface" << "Interface Value" << "Model" << "Model Value" << "Color");

  typeCombo_->setToolTip("Color value type");

  addLabelWidget("Type", typeCombo_);

  //---

  indPalCombo_ = CQUtil::makeWidget<QComboBox>(this, "indPaletteCombo");

  indPalCombo_->addItems(QStringList() << "Index" << "Palette");

  connect(indPalCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(indPalSlot(int)));

  indPalStack_ = CQUtil::makeWidget<QStackedWidget>(this, "indPaletteStack");

  //-

  indEdit_ = CQUtil::makeWidget<QSpinBox>("indEdit");

  indEdit_->setRange(-1, 99);
  indEdit_->setToolTip("Palette index in theme");

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
  QHBoxLayout *rLayout = new QHBoxLayout(rFrame_);
  rLayout->setMargin(0); rLayout->setSpacing(2);

  rEdit_ = CQUtil::makeWidget<CQColorsEditModel>("rEdit");
  rNeg_  = CQUtil::makeLabelWidget<QCheckBox>("Negate", "negate");

  rEdit_->setToolTip("Red model number");
  rNeg_ ->setToolTip("Invert red model value");

  rLayout->addWidget(rEdit_);
  rLayout->addWidget(rNeg_);

  addLabelWidget("R", rFrame_);

  //---

  gFrame_ = CQUtil::makeWidget<QFrame>("gFrame");
  QHBoxLayout *gLayout = new QHBoxLayout(gFrame_);
  gLayout->setMargin(0); gLayout->setSpacing(2);

  gEdit_ = CQUtil::makeWidget<CQColorsEditModel>("gEdit");
  gNeg_  = CQUtil::makeLabelWidget<QCheckBox>("Negate", "negate");

  gEdit_->setToolTip("Green model number");
  gNeg_ ->setToolTip("Invert green model value");

  gLayout->addWidget(gEdit_);
  gLayout->addWidget(gNeg_);

  addLabelWidget("G", gFrame_);

  //---

  bFrame_ = CQUtil::makeWidget<QFrame>("bFrame");
  QHBoxLayout *bLayout = new QHBoxLayout(bFrame_);
  bLayout->setMargin(0); bLayout->setSpacing(2);

  bEdit_ = CQUtil::makeWidget<CQColorsEditModel>("bEdit");
  bNeg_  = CQUtil::makeLabelWidget<QCheckBox>("Negate", "negate");

  bEdit_->setToolTip("Blue model number");
  bNeg_ ->setToolTip("Invert blue model value");

  bLayout->addWidget(bEdit_);
  bLayout->addWidget(bNeg_);

  addLabelWidget("B", bFrame_);

  //---

  valueEdit_ = CQUtil::makeWidget<CQRealSpin>("valueEdit");

  valueEdit_->setToolTip("Palette or interface value (0-1 if not scaled)");

  addLabelWidget("Value", valueEdit_);

  //---

  colorEdit_ = CQUtil::makeWidget<CQColorEdit>("colorEdit");

  colorEdit_->setToolTip("Explicit color name");

  addLabelWidget("Color", colorEdit_);

  //---

  scaleCheck_ = CQUtil::makeWidget<CQCheckBox>("scaleCheck");

  scaleCheck_->setToolTip("Rescale value from palette x range");

  addLabelWidget("Scale", scaleCheck_);

  //---

  layout->setRowStretch(row, 1);

  layout->setColumnStretch(2, 1);

  //---

  connectSlots(true);

  setFixedHeight(minimumSizeHint().height());

  updateState();
}

void
CQChartsColorEdit::
setColor(const CQChartsColor &color)
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

  typeCombo_ ->setFocusPolicy(Qt::NoFocus);
//indEdit_   ->setFocusPolicy(Qt::NoFocus);
//valueEdit_ ->setFocusPolicy(Qt::NoFocus);
  scaleCheck_->setFocusPolicy(Qt::NoFocus);
}

void
CQChartsColorEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, typeCombo_  , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, indEdit_    , SIGNAL(valueChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, paletteEdit_, SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, rEdit_      , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, rNeg_       , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, gEdit_      , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, gNeg_       , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, bEdit_      , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, bNeg_       , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, valueEdit_  , SIGNAL(valueChanged(double)), SLOT(widgetsToColor()));
  connectDisconnect(b, colorEdit_  , SIGNAL(colorChanged(const QColor &)), SLOT(widgetsToColor()));
  connectDisconnect(b, scaleCheck_ , SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
}

void
CQChartsColorEdit::
colorToWidgets()
{
  connectSlots(false);

  if (color_.isValid()) {
    if      (color_.type() == CQChartsColor::Type::PALETTE) {
      typeCombo_->setCurrentIndex(1);

      if (indPalStack_->currentIndex() == 0) {
        if (color_.hasPaletteIndex())
          indEdit_->setValue(color_.ind());
        else
          indEdit_->setValue(-1);
      }
      else {
        QString name;

        if (color_.hasPaletteName() && color_.getPaletteName(name))
          paletteEdit_->setCurrentIndex(paletteEdit_->findText(name));
        else
          paletteEdit_->setCurrentIndex(0);
      }
    }
    else if (color_.type() == CQChartsColor::Type::PALETTE_VALUE) {
      typeCombo_->setCurrentIndex(2);

      if (indPalStack_->currentIndex() == 0) {
        if (color_.hasPaletteIndex())
          indEdit_->setValue(color_.ind());
        else
          indEdit_->setValue(-1);
      }
      else {
        QString name;

        if (color_.hasPaletteName() && color_.getPaletteName(name))
          paletteEdit_->setCurrentIndex(paletteEdit_->findText(name));
        else
          paletteEdit_->setCurrentIndex(0);
      }

      valueEdit_->setValue(color_.value());

      scaleCheck_->setChecked(color_.isScale());
    }
    else if (color_.type() == CQChartsColor::Type::INDEXED) {
      typeCombo_->setCurrentIndex(3);

      if (indPalStack_->currentIndex() == 0) {
        if (color_.hasPaletteIndex())
          indEdit_->setValue(color_.ind());
        else
          indEdit_->setValue(-1);
      }
      else {
        QString name;

        if (color_.hasPaletteName() && color_.getPaletteName(name))
          paletteEdit_->setCurrentIndex(paletteEdit_->findText(name));
        else
          paletteEdit_->setCurrentIndex(0);
      }
    }
    else if (color_.type() == CQChartsColor::Type::INDEXED_VALUE) {
      typeCombo_->setCurrentIndex(4);

      if (indPalStack_->currentIndex() == 0) {
        if (color_.hasPaletteIndex())
          indEdit_->setValue(color_.ind());
        else
          indEdit_->setValue(-1);
      }
      else {
        QString name;

        if (color_.hasPaletteName() && color_.getPaletteName(name))
          paletteEdit_->setCurrentIndex(paletteEdit_->findText(name));
        else
          paletteEdit_->setCurrentIndex(0);
      }

      valueEdit_->setValue(color_.value());

      scaleCheck_->setChecked(color_.isScale());
    }
    else if (color_.type() == CQChartsColor::Type::INTERFACE) {
      typeCombo_->setCurrentIndex(5);
    }
    else if (color_.type() == CQChartsColor::Type::INTERFACE_VALUE) {
      typeCombo_->setCurrentIndex(6);

      valueEdit_->setValue(color_.value());
    }
    else if (color_.type() == CQChartsColor::Type::MODEL) {
      typeCombo_->setCurrentIndex(7);

      int r, g, b;

      color_.getModelRGB(r, g, b);

      rEdit_->setCurrentIndex(std::abs(r));
      gEdit_->setCurrentIndex(std::abs(g));
      bEdit_->setCurrentIndex(std::abs(b));

      rNeg_->setChecked(r < 0);
      gNeg_->setChecked(g < 0);
      bNeg_->setChecked(b < 0);
    }
    else if (color_.type() == CQChartsColor::Type::MODEL_VALUE) {
      typeCombo_->setCurrentIndex(8);

      int r, g, b;

      color_.getModelRGB(r, g, b);

      rEdit_->setCurrentIndex(std::abs(r));
      gEdit_->setCurrentIndex(std::abs(g));
      bEdit_->setCurrentIndex(std::abs(b));

      rNeg_->setChecked(r < 0);
      gNeg_->setChecked(g < 0);
      bNeg_->setChecked(b < 0);

      valueEdit_->setValue(color_.value());
    }
    else if (color_.type() == CQChartsColor::Type::COLOR) {
      typeCombo_->setCurrentIndex(9);

      colorEdit_->setColor(color_.color());
    }
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

  CQChartsColor color;

  if      (typeInd == 1) {
    color = CQChartsColor(CQChartsColor::Type::PALETTE);

    if (indPalStack_->currentIndex() == 0)
      color.setInd(indEdit_->value());
    else
      color.setPaletteName(paletteEdit_->currentText());
  }
  else if (typeInd == 2) {
    color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE);

    if (indPalStack_->currentIndex() == 0)
      color.setInd(indEdit_->value());
    else
      color.setPaletteName(paletteEdit_->currentText());

    if (scaleCheck_->isChecked())
      color.setScaleValue(CQChartsColor::Type::PALETTE_VALUE, valueEdit_->value(), true);
    else
      color.setValue(CQChartsColor::Type::PALETTE_VALUE, valueEdit_->value());
  }
  else if (typeInd == 3) {
    color = CQChartsColor(CQChartsColor::Type::INDEXED);

    if (indPalStack_->currentIndex() == 0)
      color.setInd(indEdit_->value());
    else
      color.setPaletteName(paletteEdit_->currentText());
  }
  else if (typeInd == 4) {
    color = CQChartsColor(CQChartsColor::Type::INDEXED_VALUE);

    if (indPalStack_->currentIndex() == 0)
      color.setInd(indEdit_->value());
    else
      color.setPaletteName(paletteEdit_->currentText());

    color.setValue(CQChartsColor::Type::INDEXED_VALUE, valueEdit_->value());
  }
  else if (typeInd == 5) {
    color = CQChartsColor(CQChartsColor::Type::INTERFACE);
  }
  else if (typeInd == 6) {
    color = CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE);

    color.setValue(CQChartsColor::Type::INTERFACE_VALUE, valueEdit_->value());
  }
  else if (typeInd == 7) {
    color = CQChartsColor(CQChartsColor::Type::MODEL);

    int r = rEdit_->currentIndex(); if (rNeg_->isChecked()) r = -r;
    int g = gEdit_->currentIndex(); if (gNeg_->isChecked()) g = -g;
    int b = bEdit_->currentIndex(); if (bNeg_->isChecked()) b = -b;

    color.setModelRGB(r, g, b);
  }
  else if (typeInd == 8) {
    color = CQChartsColor(CQChartsColor::Type::MODEL_VALUE);

    int r = rEdit_->currentIndex(); if (rNeg_->isChecked()) r = -r;
    int g = gEdit_->currentIndex(); if (gNeg_->isChecked()) g = -g;
    int b = bEdit_->currentIndex(); if (bNeg_->isChecked()) b = -b;

    color.setModelRGB(r, g, b);

    color.setValue(CQChartsColor::Type::MODEL_VALUE, valueEdit_->value());
  }
  else if (typeInd == 9) {
    color = CQChartsColor(CQChartsColor::Type::COLOR);

    QColor c = colorEdit_->color();

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

    widgetLabels_[w]->setVisible(visible);
  };

  setEditVisible(indPalStack_, false);
  setEditVisible(rFrame_     , false);
  setEditVisible(gFrame_     , false);
  setEditVisible(bFrame_     , false);
  setEditVisible(valueEdit_  , false);
  setEditVisible(colorEdit_  , false);
  setEditVisible(scaleCheck_ , false);

  if (color_.isValid()) {
    if      (color_.type() == CQChartsColor::Type::PALETTE) {
      setEditVisible(indPalStack_, true);
    }
    else if (color_.type() == CQChartsColor::Type::PALETTE_VALUE) {
      setEditVisible(indPalStack_, true);
      setEditVisible(valueEdit_  , true);
      setEditVisible(scaleCheck_ , true);
    }
    else if (color_.type() == CQChartsColor::Type::INDEXED) {
      setEditVisible(indPalStack_, true);
    }
    else if (color_.type() == CQChartsColor::Type::INDEXED_VALUE) {
      setEditVisible(indPalStack_, true);
      setEditVisible(valueEdit_  , true);
    }
    else if (color_.type() == CQChartsColor::Type::INTERFACE) {
    }
    else if (color_.type() == CQChartsColor::Type::INTERFACE_VALUE) {
      setEditVisible(valueEdit_, true);
    }
    else if (color_.type() == CQChartsColor::Type::MODEL) {
      setEditVisible(rFrame_, true);
      setEditVisible(gFrame_, true);
      setEditVisible(bFrame_, true);
    }
    else if (color_.type() == CQChartsColor::Type::MODEL_VALUE) {
      setEditVisible(rFrame_, true);
      setEditVisible(gFrame_, true);
      setEditVisible(bFrame_, true);

      setEditVisible(valueEdit_, true);
    }
    else if (color_.type() == CQChartsColor::Type::COLOR) {
      setEditVisible(colorEdit_, true);
    }
  }
}

QSize
CQChartsColorEdit::
sizeHint() const
{
  QSize s1 = CQChartsEditBase::sizeHint();
  QSize s2 = minimumSizeHint();

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
