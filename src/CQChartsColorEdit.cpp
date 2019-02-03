#include <CQChartsColorEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
#include <CQColorChooser.h>
#include <CQCheckBox.h>

#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

CQChartsColorLineEdit::
CQChartsColorLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("colorLineEdit");

  //---

  menuEdit_ = new CQChartsColorEdit;

  menu_->setWidget(menuEdit_);

  connect(menuEdit_, SIGNAL(colorChanged()), this, SLOT(menuEditChanged()));
}

const CQChartsColor &
CQChartsColorLineEdit::
color() const
{
  return menuEdit_->color();
}

void
CQChartsColorLineEdit::
setColor(const CQChartsColor &color)
{
  updateColor(color, /*updateText*/ true);
}

void
CQChartsColorLineEdit::
updateColor(const CQChartsColor &color, bool updateText)
{
  connectSlots(false);

  menuEdit_->setColor(color);

  if (updateText)
    colorToWidgets();

  connectSlots(true);

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
    edit_->setText(color().toString());
  else
    edit_->setText("");

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
    connect(menuEdit_, SIGNAL(colorChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(menuEdit_, SIGNAL(colorChanged()), this, SLOT(menuEditChanged()));
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
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsColor color = value.value<CQChartsColor>();

  QString str = color.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsColorPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsColor>().toString();

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

  QObject::connect(edit, SIGNAL(colorChanged()), obj, method);
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
 QFrame(parent)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  typeCombo_ = new QComboBox;

  typeCombo_->addItems(QStringList() <<
    "Palette" << "Palette Value" << "Interface" << "Interface Value" << "Color");

  connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToColor()));

  layout->addWidget(typeCombo_);

  //---

  QHBoxLayout *indLayout = new QHBoxLayout;
  indLayout->setMargin(0); indLayout->setSpacing(2);

  QLabel *indLabel = new QLabel("Index");

  indEdit_ = new QSpinBox;

  connect(indEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToColor()));

  indLayout->addWidget(indLabel);
  indLayout->addWidget(indEdit_);

  layout->addLayout(indLayout);

  //---

  QHBoxLayout *valueLayout = new QHBoxLayout;
  valueLayout->setMargin(0); valueLayout->setSpacing(2);

  QLabel *valueLabel = new QLabel("Value");

  valueEdit_ = new CQRealSpin;

  connect(valueEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToColor()));

  valueLayout->addWidget(valueLabel);
  valueLayout->addWidget(valueEdit_);

  layout->addLayout(valueLayout);

  //---

  QHBoxLayout *colorLayout = new QHBoxLayout;
  colorLayout->setMargin(0); colorLayout->setSpacing(2);

  QLabel *colorLabel = new QLabel("Color");

  colorEdit_ = new CQColorChooser;

  connect(colorEdit_, SIGNAL(colorChanged(const QColor &)), this, SLOT(widgetsToColor()));

  colorLayout->addWidget(colorLabel);
  colorLayout->addWidget(colorEdit_);

  layout->addLayout(colorLayout);

  //---

  QHBoxLayout *scaleLayout = new QHBoxLayout;
  scaleLayout->setMargin(0); scaleLayout->setSpacing(2);

  QLabel *scaleLabel = new QLabel("Scale");

  scaleCheck_ = new CQCheckBox;

  connect(scaleCheck_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToColor()));

  scaleLayout->addWidget(scaleLabel);
  scaleLayout->addWidget(scaleCheck_);

  layout->addLayout(scaleLayout);

  //---

  updateState();
}

const CQChartsColor &
CQChartsColorEdit::
color() const
{
  return color_;
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
connectSlots(bool b)
{
  if (b) {
    connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToColor()));
    connect(indEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToColor()));
    connect(valueEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToColor()));
    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)), this, SLOT(widgetsToColor()));
    connect(scaleCheck_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToColor()));
  }
  else {
    disconnect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToColor()));
    disconnect(indEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToColor()));
    disconnect(valueEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToColor()));
    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)), this, SLOT(widgetsToColor()));
    disconnect(scaleCheck_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToColor()));
  }
}

void
CQChartsColorEdit::
colorToWidgets()
{
  connectSlots(false);

  if (color_.isValid()) {
    if      (color_.type() == CQChartsColor::Type::PALETTE) {
      typeCombo_->setCurrentIndex(0);

      indEdit_->setValue(color_.ind());
    }
    else if (color_.type() == CQChartsColor::Type::PALETTE_VALUE) {
      typeCombo_->setCurrentIndex(1);

      indEdit_->setValue(color_.ind());

      valueEdit_->setValue(color_.value());

      scaleCheck_->setChecked(color_.isScale());
    }
    else if (color_.type() == CQChartsColor::Type::INTERFACE) {
      typeCombo_->setCurrentIndex(2);
    }
    else if (color_.type() == CQChartsColor::Type::INTERFACE_VALUE) {
      typeCombo_->setCurrentIndex(3);

      valueEdit_->setValue(color_.value());
    }
    else if (color_.type() == CQChartsColor::Type::COLOR) {
      typeCombo_->setCurrentIndex(4);

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

  if      (typeInd == 0) {
    color = CQChartsColor(CQChartsColor::Type::PALETTE);

    color.setInd(indEdit_->value());
  }
  else if (typeInd == 1) {
    color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE);

    color.setInd  (indEdit_   ->value    ());
    color.setValue(CQChartsColor::Type::PALETTE_VALUE, valueEdit_ ->value());
    color.setScale(scaleCheck_->isChecked());
  }
  else if (typeInd == 2) {
    color = CQChartsColor(CQChartsColor::Type::INTERFACE);
  }
  else if (typeInd == 3) {
    color = CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE);

    color.setValue(CQChartsColor::Type::INTERFACE_VALUE, valueEdit_->value());
  }
  else if (typeInd == 4) {
    color = CQChartsColor(CQChartsColor::Type::COLOR);

    color.setColor(colorEdit_->color());
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
