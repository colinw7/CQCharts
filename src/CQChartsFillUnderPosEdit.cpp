#include <CQChartsFillUnderPos.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
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

CQChartsFillUnderPos::
CQChartsFillUnderPos(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("fillUnderPos");

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

  button_ = new CQChartsFillUnderPosMenuButton;
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

  QGridLayout *menuFrameLayout = new QGridLayout(menuFrame);
  menuFrameLayout->setMargin(2); menuFrameLayout->setSpacing(2);

  menu_->setWidget(menuFrame);

  //---

  xTypeCombo_ = new QComboBox;

  xTypeCombo_->addItems(QStringList() << "Min" << "Max" << "Pos");

  connect(xTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));

  QLabel xTypeLabel = new QLabel("X Label");

  menuFrameLayout->addWidget(xTypeLabel , 0, 0);
  menuFrameLayout->addWidget(xTypeCombo_, 0, 1);

  //---

  QLabel *xPosLabel = new QLabel("X Pos");

  xPosEdit_ = new CQRealSpin;

  connect(xPosEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));

  indLayout->addWidget(xPosLabel, 1, 0);
  indLayout->addWidget(xPosEdit_, 1, 1);

  //---

  yTypeCombo_ = new QComboBox;

  yTypeCombo_->addItems(QStringList() << "Min" << "Max" << "Pos");

  connect(yTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));

  QLabel yTypeLabel = new QLabel("Y Label");

  menuFrameLayout->addWidget(yTypeLabel , 0, 0);
  menuFrameLayout->addWidget(yTypeCombo_, 0, 1); 

  //---

  QLabel *yPosLabel = new QLabel("Y Pos");

  yPosEdit_ = new CQRealSpin;

  connect(yPosEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));

  indLayout->addWidget(yPosLabel, 1, 0);
  indLayout->addWidget(yPosEdit_, 1, 1);

  //---

  updateState();
}

const CQChartsFillUnderPos &
CQChartsFillUnderPos::
fillUnderPos() const
{
  return fillUnderPos_;
}

void
CQChartsFillUnderPos::
setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos)
{
  fillUnderPos_ = fillUnderPos;

  fillUnderPosToWidgets(true);

  updateState();

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPos::
connectSlots(bool b)
{
  if (b) {
    connect(edit_, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));

    connect(xTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));
    connect(xPosEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));
    connect(yTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));
    connect(yPosEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));
  }
  else {
    disconnect(edit_, SIGNAL(textChanged(const QString &)),
               this, SLOT(textChanged(const QString &)));

    disconnect(xTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));
    disconnect(xPosEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));
    disconnect(yTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));
    disconnect(yPosEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));
  }
}

QString
CQChartsFillUnderPos::
placeholderText() const
{
  return edit_->placeholderText();
}

void
CQChartsFillUnderPos::
setPlaceholderText(const QString &s)
{
  edit_->setPlaceholderText(s);
}

void
CQChartsFillUnderPos::
showMenu()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsFillUnderPos::
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
CQChartsFillUnderPos::
textChanged(const QString &)
{
  CQChartsFillUnderPos fillUnderPos(edit_->text());

  if (! fillUnderPos.isValid())
    return;

  fillUnderPos_ = fillUnderPos;

  fillUnderPosToWidgets(false);

  updateState();

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPos::
fillUnderPosToWidgets(bool updateText)
{
  connectSlots(false);

  if (fillUnderPos_.isValid()) {
    if      (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::MIN) {
      xTypeCombo_->setCurrentIndex(0);
    }
    else if (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::MAX) {
      xTypeCombo_->setCurrentIndex(1);
    }
    else if (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::POS) {
      xTypeCombo_->setCurrentIndex(2);
    }

    xPosEdit_->setValue(fillUnderPos_.xpos());

    if      (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::MIN) {
      yTypeCombo_->setCurrentIndex(0);
    }
    else if (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::MAX) {
      yTypeCombo_->setCurrentIndex(1);
    }
    else if (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::POS) {
      yTypeCombo_->setCurrentIndex(2);
    }

    yPosEdit_->setValue(fillUnderPos_.ypos());

    if (updateText)
      edit_->setText(fillUnderPos_.toString());
  }
  else {
    xTypeCombo_->setCurrentIndex(0);
    xPosEdit_->setValue(0.0);

    yTypeCombo_->setCurrentIndex(0);
    yPosEdit_->setValue(0.0);

    if (updateText)
      edit_->setText("");
  }

  connectSlots(true);
}

void
CQChartsFillUnderPos::
widgetsToFillUnderPos()
{
  connectSlots(false);

  //---

  CQChartsFillUnderPos::Type xtype, ytype;
  double                     xpos, ypos;

  int xtypeInd = xtypeCombo_->currentIndex();

  if      (xtypeInd == 0)
    xtype = CQChartsFillUnderPos::Type::MIN;
  }
  else if (xtypeInd == 1) {
    xtype = CQChartsFillUnderPos::Type::MAX;
  }
  else if (typeInd == 2) {
    xtype = CQChartsFillUnderPos::Type::POS;
  }

  xpos = xposEdit_->value();

  int ytypeInd = ytypeCombo_->currentIndex();

  if      (ytypeInd == 0)
    ytype = CQChartsFillUnderPos::Type::MIN;
  }
  else if (ytypeInd == 1) {
    ytype = CQChartsFillUnderPos::Type::MAX;
  }
  else if (typeInd == 2) {
    ytype = CQChartsFillUnderPos::Type::POS;
  }

  ypos = yposEdit_->value();

  fillUnderPos_ = CQChartsFillUnderPos(xtype, xpos, ytype, ypos);

  edit_->setText(fillUnderPos_.toString());

  updateState();

  //---

  connectSlots(true);

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPos::
updateState()
{
}

void
CQChartsFillUnderPos::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  painter.setPen(palette().fillUnderPos(QPalette::Text));

  // draw the combobox frame, focusrect and selected etc.
  QStyleOptionComboBox opt;

  initStyle(opt);

  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void
CQChartsFillUnderPos::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQChartsFillUnderPos::
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

CQChartsFillUnderPosMenuButton::
CQChartsFillUnderPosMenuButton(QWidget *parent) :
 QPushButton(parent)
{
}

void
CQChartsFillUnderPosMenuButton::
paintEvent(QPaintEvent*)
{
  // drawn by CQChartsFillUnderPos
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsFillUnderPosPropertyViewType::
CQChartsFillUnderPosPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsFillUnderPosPropertyViewType::
getEditor() const
{
  return new CQChartsFillUnderPosPropertyViewEditor;
}

bool
CQChartsFillUnderPosPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsFillUnderPosPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsFillUnderPos fillUnderPos = value.value<CQChartsFillUnderPos>();

  QString str = fillUnderPos.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsFillUnderPosPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsFillUnderPos>().toString();

  return str;
}

//------

CQChartsFillUnderPosPropertyViewEditor::
CQChartsFillUnderPosPropertyViewEditor()
{
}

QWidget *
CQChartsFillUnderPosPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsFillUnderPos *edit = new CQChartsFillUnderPos(parent);

  return edit;
}

void
CQChartsFillUnderPosPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsFillUnderPos *edit = qobject_cast<CQChartsFillUnderPos *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillUnderPosChanged()), obj, method);
}

QVariant
CQChartsFillUnderPosPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsFillUnderPos *edit = qobject_cast<CQChartsFillUnderPos *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillUnderPos());
}

void
CQChartsFillUnderPosPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsFillUnderPos *edit = qobject_cast<CQChartsFillUnderPos *>(w);
  assert(edit);

  CQChartsFillUnderPos fillUnderPos = var.value<CQChartsFillUnderPos>();

  edit->setFillUnderPos(fillUnderPos);
}
