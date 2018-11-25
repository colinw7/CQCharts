#include <CQChartsFillUnderEdit.h>

#include <CQWidgetMenu.h>
#include <CQPropertyView.h>
#include <CQRealSpin.h>

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStylePainter>

CQChartsFillUnderSideEdit::
CQChartsFillUnderSideEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("fillUnderSide");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  combo_ = new QComboBox;

  combo_->addItems(QStringList() << CQChartsFillUnderSide::sideNames());

  combo_->setObjectName("combo");

  layout->addWidget(combo_);

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

const CQChartsFillUnderSide &
CQChartsFillUnderSideEdit::
fillUnderSide() const
{
  return fillUnderSide_;
}

void
CQChartsFillUnderSideEdit::
setFillUnderSide(const CQChartsFillUnderSide &fillUnderSide)
{
  disconnect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  fillUnderSide_ = fillUnderSide;

  combo_->setCurrentIndex(combo_->findText(fillUnderSide_.toString()));

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

void
CQChartsFillUnderSideEdit::
comboChanged()
{
  disconnect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  fillUnderSide_ = CQChartsFillUnderSide(combo_->currentText());

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));

  emit fillUnderSideChanged();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsFillUnderSidePropertyViewType::
CQChartsFillUnderSidePropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsFillUnderSidePropertyViewType::
getEditor() const
{
  return new CQChartsFillUnderSidePropertyViewEditor;
}

bool
CQChartsFillUnderSidePropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsFillUnderSidePropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsFillUnderSide fillUnderSide = value.value<CQChartsFillUnderSide>();

  QString str = fillUnderSide.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsFillUnderSidePropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsFillUnderSide>().toString();

  return str;
}

//------

CQChartsFillUnderSidePropertyViewEditor::
CQChartsFillUnderSidePropertyViewEditor()
{
}

QWidget *
CQChartsFillUnderSidePropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsFillUnderSideEdit *edit = new CQChartsFillUnderSideEdit(parent);

  return edit;
}

void
CQChartsFillUnderSidePropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsFillUnderSideEdit *edit = qobject_cast<CQChartsFillUnderSideEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillUnderSideChanged()), obj, method);
}

QVariant
CQChartsFillUnderSidePropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsFillUnderSideEdit *edit = qobject_cast<CQChartsFillUnderSideEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillUnderSide());
}

void
CQChartsFillUnderSidePropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsFillUnderSideEdit *edit = qobject_cast<CQChartsFillUnderSideEdit *>(w);
  assert(edit);

  CQChartsFillUnderSide fillUnderSide = var.value<CQChartsFillUnderSide>();

  edit->setFillUnderSide(fillUnderSide);
}

//------

CQChartsFillUnderPosEdit::
CQChartsFillUnderPosEdit(QWidget *parent) :
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

  xtypeCombo_ = new QComboBox;

  xtypeCombo_->addItems(QStringList() << "None" << "Min" << "Max" << "Pos");

  connect(xtypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));

  QLabel *xtypeLabel = new QLabel("X Type");

  menuFrameLayout->addWidget(xtypeLabel , 0, 0);
  menuFrameLayout->addWidget(xtypeCombo_, 0, 1);

  //---

  QLabel *xposLabel = new QLabel("X Pos");

  xposEdit_ = new CQRealSpin;

  connect(xposEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));

  menuFrameLayout->addWidget(xposLabel, 1, 0);
  menuFrameLayout->addWidget(xposEdit_, 1, 1);

  //---

  ytypeCombo_ = new QComboBox;

  ytypeCombo_->addItems(QStringList() << "None" << "Min" << "Max" << "Pos");

  connect(ytypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));

  QLabel *ytypeLabel = new QLabel("Y Type");

  menuFrameLayout->addWidget(ytypeLabel , 2, 0);
  menuFrameLayout->addWidget(ytypeCombo_, 2, 1);

  //---

  QLabel *yposLabel = new QLabel("Y Pos");

  yposEdit_ = new CQRealSpin;

  connect(yposEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));

  menuFrameLayout->addWidget(yposLabel, 3, 0);
  menuFrameLayout->addWidget(yposEdit_, 3, 1);

  //---

  updateState();
}

const CQChartsFillUnderPos &
CQChartsFillUnderPosEdit::
fillUnderPos() const
{
  return fillUnderPos_;
}

void
CQChartsFillUnderPosEdit::
setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos)
{
  fillUnderPos_ = fillUnderPos;

  fillUnderPosToWidgets(true);

  updateState();

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPosEdit::
connectSlots(bool b)
{
  if (b) {
    connect(edit_, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));

    connect(xtypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));
    connect(xposEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));
    connect(ytypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));
    connect(yposEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));
  }
  else {
    disconnect(edit_, SIGNAL(textChanged(const QString &)),
               this, SLOT(textChanged(const QString &)));

    disconnect(xtypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));
    disconnect(xposEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));
    disconnect(ytypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));
    disconnect(yposEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));
  }
}

QString
CQChartsFillUnderPosEdit::
placeholderText() const
{
  return edit_->placeholderText();
}

void
CQChartsFillUnderPosEdit::
setPlaceholderText(const QString &s)
{
  edit_->setPlaceholderText(s);
}

void
CQChartsFillUnderPosEdit::
showMenu()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsFillUnderPosEdit::
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
CQChartsFillUnderPosEdit::
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
CQChartsFillUnderPosEdit::
fillUnderPosToWidgets(bool updateText)
{
  connectSlots(false);

  if (fillUnderPos_.isValid()) {
    if      (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::NONE) {
      xtypeCombo_->setCurrentIndex(0);
    }
    else if (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::MIN) {
      xtypeCombo_->setCurrentIndex(1);
    }
    else if (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::MAX) {
      xtypeCombo_->setCurrentIndex(2);
    }
    else if (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::POS) {
      xtypeCombo_->setCurrentIndex(3);
    }

    xposEdit_->setValue(fillUnderPos_.xpos());

    if      (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::NONE) {
      ytypeCombo_->setCurrentIndex(0);
    }
    else if (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::MIN) {
      ytypeCombo_->setCurrentIndex(1);
    }
    else if (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::MAX) {
      ytypeCombo_->setCurrentIndex(2);
    }
    else if (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::POS) {
      ytypeCombo_->setCurrentIndex(3);
    }

    yposEdit_->setValue(fillUnderPos_.ypos());

    if (updateText)
      edit_->setText(fillUnderPos_.toString());
  }
  else {
    xtypeCombo_->setCurrentIndex(0);
    xposEdit_->setValue(0.0);

    ytypeCombo_->setCurrentIndex(0);
    yposEdit_->setValue(0.0);

    if (updateText)
      edit_->setText("");
  }

  connectSlots(true);
}

void
CQChartsFillUnderPosEdit::
widgetsToFillUnderPos()
{
  connectSlots(false);

  //---

  CQChartsFillUnderPos::Type xtype = CQChartsFillUnderPos::Type::NONE;

  int xtypeInd = xtypeCombo_->currentIndex();

  if      (xtypeInd == 0) xtype = CQChartsFillUnderPos::Type::NONE;
  else if (xtypeInd == 1) xtype = CQChartsFillUnderPos::Type::MIN;
  else if (xtypeInd == 2) xtype = CQChartsFillUnderPos::Type::MAX;
  else if (xtypeInd == 3) xtype = CQChartsFillUnderPos::Type::POS;

  double xpos = xposEdit_->value();

  CQChartsFillUnderPos::Type ytype = CQChartsFillUnderPos::Type::NONE;

  int ytypeInd = ytypeCombo_->currentIndex();

  if      (ytypeInd == 0) ytype = CQChartsFillUnderPos::Type::NONE;
  else if (ytypeInd == 1) ytype = CQChartsFillUnderPos::Type::MIN;
  else if (ytypeInd == 2) ytype = CQChartsFillUnderPos::Type::MAX;
  else if (ytypeInd == 3) ytype = CQChartsFillUnderPos::Type::POS;

  double ypos = yposEdit_->value();

  fillUnderPos_ = CQChartsFillUnderPos(xtype, xpos, ytype, ypos);

  edit_->setText(fillUnderPos_.toString());

  updateState();

  //---

  connectSlots(true);

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPosEdit::
updateState()
{
}

void
CQChartsFillUnderPosEdit::
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
CQChartsFillUnderPosEdit::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQChartsFillUnderPosEdit::
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
  // drawn by CQChartsFillUnderPosEdit
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
  CQChartsFillUnderPosEdit *edit = new CQChartsFillUnderPosEdit(parent);

  return edit;
}

void
CQChartsFillUnderPosPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsFillUnderPosEdit *edit = qobject_cast<CQChartsFillUnderPosEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillUnderPosChanged()), obj, method);
}

QVariant
CQChartsFillUnderPosPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsFillUnderPosEdit *edit = qobject_cast<CQChartsFillUnderPosEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillUnderPos());
}

void
CQChartsFillUnderPosPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsFillUnderPosEdit *edit = qobject_cast<CQChartsFillUnderPosEdit *>(w);
  assert(edit);

  CQChartsFillUnderPos fillUnderPos = var.value<CQChartsFillUnderPos>();

  edit->setFillUnderPos(fillUnderPos);
}
