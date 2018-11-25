#include <CQChartsColorEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
#include <CQColorChooser.h>
#include <CQCheckBox.h>

#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QStylePainter>

CQChartsColorEdit::
CQChartsColorEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("colorEdit");

  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Sunken);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  //---

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  edit_ = new QLineEdit;
  edit_->setObjectName("edit");

  edit_->setFrame(false);

  connect(edit_, SIGNAL(textChanged(const QString &)),
          this, SLOT(textChanged(const QString &)));

  layout->addWidget(edit_);

  //---

  button_ = new CQChartsColorEditMenuButton;
  button_->setObjectName("button");

  QStyleOptionComboBox opt;

  initStyle(opt);

  QRect r = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  button_->setFixedWidth(r.size().width());

  button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  button_->setFocusPolicy(Qt::NoFocus);

  connect(button_, SIGNAL(clicked()), this, SLOT(showMenu()));

  layout->addWidget(button_);

  //---

  menu_ = new CQWidgetMenu(this);

  connect(menu_, SIGNAL(menuShown()), this, SLOT(updateMenu()));

  //---

  QFrame *menuFrame = new QFrame;

  QVBoxLayout *menuFrameLayout = new QVBoxLayout(menuFrame);
  menuFrameLayout->setMargin(2); menuFrameLayout->setSpacing(2);

  menu_->setWidget(menuFrame);

  //---

  typeCombo_ = new QComboBox;

  typeCombo_->addItems(QStringList() <<
    "Palette" << "Palette Value" << "Interface" << "Interface Value" << "Color");

  connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToColor()));

  menuFrameLayout->addWidget(typeCombo_);

  //---

  QHBoxLayout *indLayout = new QHBoxLayout;
  indLayout->setMargin(0); indLayout->setSpacing(2);

  QLabel *indLabel = new QLabel("Index");

  indEdit_ = new QSpinBox;

  connect(indEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToColor()));

  indLayout->addWidget(indLabel);
  indLayout->addWidget(indEdit_);

  menuFrameLayout->addLayout(indLayout);

  //---

  QHBoxLayout *valueLayout = new QHBoxLayout;
  valueLayout->setMargin(0); valueLayout->setSpacing(2);

  QLabel *valueLabel = new QLabel("Value");

  valueEdit_ = new CQRealSpin;

  connect(valueEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToColor()));

  valueLayout->addWidget(valueLabel);
  valueLayout->addWidget(valueEdit_);

  menuFrameLayout->addLayout(valueLayout);

  //---

  QHBoxLayout *colorLayout = new QHBoxLayout;
  colorLayout->setMargin(0); colorLayout->setSpacing(2);

  QLabel *colorLabel = new QLabel("Color");

  colorEdit_ = new CQColorChooser;

  connect(colorEdit_, SIGNAL(colorChanged(const QColor &)), this, SLOT(widgetsToColor()));

  colorLayout->addWidget(colorLabel);
  colorLayout->addWidget(colorEdit_);

  menuFrameLayout->addLayout(colorLayout);

  //---

  QHBoxLayout *scaleLayout = new QHBoxLayout;
  scaleLayout->setMargin(0); scaleLayout->setSpacing(2);

  QLabel *scaleLabel = new QLabel("Scale");

  scaleCheck_ = new CQCheckBox;

  connect(scaleCheck_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToColor()));

  scaleLayout->addWidget(scaleLabel);
  scaleLayout->addWidget(scaleCheck_);

  menuFrameLayout->addLayout(scaleLayout);

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

  colorToWidgets(true);

  updateState();

  emit colorChanged();
}

void
CQChartsColorEdit::
connectSlots(bool b)
{
  if (b) {
    connect(edit_, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));

    connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToColor()));
    connect(indEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToColor()));
    connect(valueEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToColor()));
    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)), this, SLOT(widgetsToColor()));
    connect(scaleCheck_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToColor()));
  }
  else {
    disconnect(edit_, SIGNAL(textChanged(const QString &)),
               this, SLOT(textChanged(const QString &)));

    disconnect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToColor()));
    disconnect(indEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToColor()));
    disconnect(valueEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToColor()));
    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)), this, SLOT(widgetsToColor()));
    disconnect(scaleCheck_, SIGNAL(stateChanged(int)), this, SLOT(widgetsToColor()));
  }
}

QString
CQChartsColorEdit::
placeholderText() const
{
  return edit_->placeholderText();
}

void
CQChartsColorEdit::
setPlaceholderText(const QString &s)
{
  edit_->setPlaceholderText(s);
}

void
CQChartsColorEdit::
showMenu()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsColorEdit::
updateMenu()
{
  connectSlots(false);

  //---

  int w = std::max(menu_->sizeHint().width(), this->width());

  menu_->setFixedWidth(w);

  menu_->updateAreaSize();

  //---

  connectSlots(true);
}

void
CQChartsColorEdit::
textChanged(const QString &)
{
  CQChartsColor color(edit_->text());

  if (! color.isValid())
    return;

  color_ = color;

  colorToWidgets(false);

  updateState();

  emit colorChanged();
}

void
CQChartsColorEdit::
colorToWidgets(bool updateText)
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

    if (updateText)
      edit_->setText(color_.toString());
  }
  else {
    typeCombo_->setCurrentIndex(0);

    if (updateText)
      edit_->setText("");
  }

  connectSlots(true);
}

void
CQChartsColorEdit::
widgetsToColor()
{
  connectSlots(false);

  //---

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

  edit_->setText(color_.toString());

  updateState();

  //---

  connectSlots(true);

  emit colorChanged();
}

void
CQChartsColorEdit::
updateState()
{
  connectSlots(false);

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

  connectSlots(true);
}

void
CQChartsColorEdit::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focusrect and selected etc.
  QStyleOptionComboBox opt;

  initStyle(opt);

  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void
CQChartsColorEdit::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQChartsColorEdit::
initStyle(QStyleOptionComboBox &opt)
{
  opt.initFrom(this);

  opt.editable = true;
  opt.frame    = true;

  if (hasFocus()) opt.state |= QStyle::State_Selected;

  opt.subControls = QStyle::SC_All;

  if      (button_ && button_->isDown()) {
    opt.activeSubControls = QStyle::SC_ComboBoxArrow;
    opt.state |= QStyle::State_Sunken;
  }
  else if (button_ && button_->underMouse()) {
    opt.activeSubControls = QStyle::SC_ComboBoxArrow;
  }
  else if (edit_ && edit_->underMouse()) {
    opt.activeSubControls = QStyle::SC_ComboBoxEditField;
  }
}

//------

CQChartsColorEditMenuButton::
CQChartsColorEditMenuButton(QWidget *parent) :
 QPushButton(parent)
{
}

void
CQChartsColorEditMenuButton::
paintEvent(QPaintEvent*)
{
  // drawn by CQChartsColorEdit
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
  CQChartsColorEdit *edit = new CQChartsColorEdit(parent);

  return edit;
}

void
CQChartsColorPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsColorEdit *edit = qobject_cast<CQChartsColorEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(colorChanged()), obj, method);
}

QVariant
CQChartsColorPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsColorEdit *edit = qobject_cast<CQChartsColorEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->color());
}

void
CQChartsColorPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsColorEdit *edit = qobject_cast<CQChartsColorEdit *>(w);
  assert(edit);

  CQChartsColor color = var.value<CQChartsColor>();

  edit->setColor(color);
}
