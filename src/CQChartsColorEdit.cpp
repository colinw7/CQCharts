#include <CQChartsColorEdit.h>
#include <CQChartsObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
#include <CQColorEdit.h>
#include <CQCheckBox.h>

#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
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

      painter->setPen(CQChartsUtil::bwColor(c)); // TODO: contrast border

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

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(QStringList() <<
    "None" << "Palette" << "Palette Value" << "Indexed Palette" << "Indexed Palette Value" <<
    "Interface" << "Interface Value" << "Color");

  typeCombo_->setToolTip("Color value type");

  layout->addWidget(typeCombo_);

  //---

  QHBoxLayout *indLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  QLabel *indLabel = CQUtil::makeLabelWidget<QLabel>("Index", "indLabel");

  indEdit_ = CQUtil::makeWidget<QSpinBox>("indEdit");

  indEdit_->setRange(-1, 99);

  indEdit_->setToolTip("Theme palette index");

  indLayout->addWidget(indLabel);
  indLayout->addWidget(indEdit_);

  layout->addLayout(indLayout);

  //---

  QHBoxLayout *valueLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  QLabel *valueLabel = CQUtil::makeLabelWidget<QLabel>("Value", "valueLabel");

  valueEdit_ = CQUtil::makeWidget<CQRealSpin>("valueEdit");

  valueEdit_->setToolTip("Palette or interface value");

  valueLayout->addWidget(valueLabel);
  valueLayout->addWidget(valueEdit_);

  layout->addLayout(valueLayout);

  //---

  QHBoxLayout *colorLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  QLabel *colorLabel = CQUtil::makeLabelWidget<QLabel>("Color", "colorLabel");

  colorEdit_ = CQUtil::makeWidget<CQColorEdit>("colorEdit");

  colorEdit_->setToolTip("Color name");

  colorLayout->addWidget(colorLabel);
  colorLayout->addWidget(colorEdit_);

  layout->addLayout(colorLayout);

  //---

  QHBoxLayout *scaleLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  QLabel *scaleLabel = CQUtil::makeLabelWidget<QLabel>("Scale", "scaleLabel");

  scaleCheck_ = CQUtil::makeWidget<CQCheckBox>("scaleCheck");

  scaleCheck_->setToolTip("Is palette value scaled from palette x range");

  scaleLayout->addWidget(scaleLabel);
  scaleLayout->addWidget(scaleCheck_);

  layout->addLayout(scaleLayout);

  //---

  connectSlots(true);

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

  connectDisconnect(b, typeCombo_ , SIGNAL(currentIndexChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, indEdit_   , SIGNAL(valueChanged(int)), SLOT(widgetsToColor()));
  connectDisconnect(b, valueEdit_ , SIGNAL(valueChanged(double)), SLOT(widgetsToColor()));
  connectDisconnect(b, colorEdit_ , SIGNAL(colorChanged(const QColor &)), SLOT(widgetsToColor()));
  connectDisconnect(b, scaleCheck_, SIGNAL(stateChanged(int)), SLOT(widgetsToColor()));
}

void
CQChartsColorEdit::
colorToWidgets()
{
  connectSlots(false);

  if (color_.isValid()) {
    if      (color_.type() == CQChartsColor::Type::PALETTE) {
      typeCombo_->setCurrentIndex(1);

      indEdit_->setValue(color_.ind());
    }
    else if (color_.type() == CQChartsColor::Type::PALETTE_VALUE) {
      typeCombo_->setCurrentIndex(2);

      indEdit_->setValue(color_.ind());

      valueEdit_->setValue(color_.value());

      scaleCheck_->setChecked(color_.isScale());
    }
    else if (color_.type() == CQChartsColor::Type::INDEXED) {
      typeCombo_->setCurrentIndex(3);

      indEdit_->setValue(color_.ind());
    }
    else if (color_.type() == CQChartsColor::Type::INDEXED_VALUE) {
      typeCombo_->setCurrentIndex(4);

      indEdit_->setValue(color_.ind());

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
    else if (color_.type() == CQChartsColor::Type::COLOR) {
      typeCombo_->setCurrentIndex(7);

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

    color.setInd(indEdit_->value());
  }
  else if (typeInd == 2) {
    color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE);

    color.setInd(indEdit_->value());

    if (scaleCheck_->isChecked())
      color.setScaleValue(CQChartsColor::Type::PALETTE_VALUE, valueEdit_->value(), true);
    else
      color.setValue(CQChartsColor::Type::PALETTE_VALUE, valueEdit_->value());
  }
  else if (typeInd == 3) {
    color = CQChartsColor(CQChartsColor::Type::INDEXED);

    color.setInd(indEdit_->value());
  }
  else if (typeInd == 4) {
    color = CQChartsColor(CQChartsColor::Type::INDEXED_VALUE);

    color.setInd  (indEdit_->value());
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
updateState()
{
  indEdit_   ->setEnabled(false);
  valueEdit_ ->setEnabled(false);
  colorEdit_ ->setEnabled(false);
  scaleCheck_->setEnabled(false);

  if (color_.isValid()) {
    if      (color_.type() == CQChartsColor::Type::PALETTE) {
      indEdit_->setEnabled(true);
    }
    else if (color_.type() == CQChartsColor::Type::PALETTE_VALUE) {
      indEdit_   ->setEnabled(true);
      valueEdit_ ->setEnabled(true);
      scaleCheck_->setEnabled(true);
    }
    else if (color_.type() == CQChartsColor::Type::INDEXED) {
      indEdit_->setEnabled(true);
    }
    else if (color_.type() == CQChartsColor::Type::INDEXED_VALUE) {
      indEdit_  ->setEnabled(true);
      valueEdit_->setEnabled(true);
    }
    else if (color_.type() == CQChartsColor::Type::INTERFACE) {
    }
    else if (color_.type() == CQChartsColor::Type::INTERFACE_VALUE) {
      valueEdit_->setEnabled(true);
    }
    else if (color_.type() == CQChartsColor::Type::COLOR) {
      colorEdit_->setEnabled(true);
    }
  }
}
