#include <CQChartsColumnEdit.h>
#include <CQWidgetMenu.h>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QStylePainter>

CQChartsColumnEdit::
CQChartsColumnEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("columnEdit");

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
          this, SIGNAL(textChanged(const QString &)));

  layout->addWidget(edit_);

  button_ = new CQChartsColumnEditMenuButton;
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

  menuColumnGroup_ = new QGroupBox("Column");

  menuColumnGroup_->setCheckable(true);
  menuColumnGroup_->setChecked(true);

  connect(menuColumnGroup_, SIGNAL(clicked(bool)), this, SLOT(menuColumnGroupClicked(bool)));

  menuFrameLayout->addWidget(menuColumnGroup_);

  //--

  QVBoxLayout *menuColumnGroupLayout = new QVBoxLayout(menuColumnGroup_);
  menuColumnGroupLayout->setMargin(2); menuColumnGroupLayout->setSpacing(2);

  columnCombo_ = new QComboBox;

  connect(columnCombo_, SIGNAL(currentIndexChanged(int)),
          this, SLOT(menuColumnChanged(int)));

  menuColumnGroupLayout->addWidget(columnCombo_);

  //---

  menuExprGroup_ = new QGroupBox("Expression");

  menuExprGroup_->setCheckable(true);
  menuExprGroup_->setChecked(false);

  connect(menuExprGroup_, SIGNAL(clicked(bool)), this, SLOT(menuExprGroupClicked(bool)));

  menuFrameLayout->addWidget(menuExprGroup_);

  //--

  QVBoxLayout *menuExprGroupLayout = new QVBoxLayout(menuExprGroup_);
  menuExprGroupLayout->setMargin(2); menuExprGroupLayout->setSpacing(2);

  expressionEdit_ = new QLineEdit;

  connect(expressionEdit_, SIGNAL(textChanged(const QString &)),
          this, SLOT(expressionTextChanged(const QString &)));

  menuExprGroupLayout->addWidget(expressionEdit_);

  //---

  vheaderCheck_ = new QCheckBox("Vertical Header");

  connect(vheaderCheck_, SIGNAL(clicked(bool)), this, SLOT(vheaderCheckClicked(bool)));

  menuFrameLayout->addWidget(vheaderCheck_);

  //---

  updateState();
}

void
CQChartsColumnEdit::
setModel(QAbstractItemModel *model)
{
  model_ = model;
}

QString
CQChartsColumnEdit::
text() const
{
  return edit_->text();
}

void
CQChartsColumnEdit::
setText(const QString &s)
{
  edit_->setText(s);
}

QString
CQChartsColumnEdit::
placeholderText() const
{
  return edit_->placeholderText();
}

void
CQChartsColumnEdit::
setPlaceholderText(const QString &s)
{
  edit_->setPlaceholderText(s);
}

void
CQChartsColumnEdit::
showMenu()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsColumnEdit::
updateMenu()
{
  int w = std::max(menu_->sizeHint().width(), this->width());

  menu_->setFixedWidth(w);

  menu_->updateAreaSize();

  //---

  columnCombo_->clear();

  if (! model_)
    return;

  int nc = model_->columnCount();

  for (int c = 0; c < nc; ++c) {
    QString name = model_->headerData(c, Qt::Horizontal).toString();

    QString label;

    if (! name.simplified().length())
      label = QString("%1 : <no name>").arg(c);
    else
      label = QString("%1 : %2").arg(c).arg(name);

    columnCombo_->addItem(label);
  }

  bool ok;

  int column = edit_->text().toInt(&ok);

  if (ok)
    columnCombo_->setCurrentIndex(column);
}

void
CQChartsColumnEdit::
menuColumnGroupClicked(bool b)
{
  if (! b)
    menuColumnGroup_->setChecked(true);
  else {
    menuExprGroup_->setChecked(false);
    vheaderCheck_ ->setChecked(false);

    edit_->setText(QString("%1").arg(columnCombo_->currentIndex()));

    updateState();
  }
}

void
CQChartsColumnEdit::
menuExprGroupClicked(bool b)
{
  if (! b)
    menuExprGroup_->setChecked(true);
  else {
    menuColumnGroup_->setChecked(false);
    vheaderCheck_   ->setChecked(false);

    if (expressionEdit_->text().simplified().length())
      edit_->setText(QString("(%1)").arg(expressionEdit_->text()));
    else
      edit_->setText("");

    updateState();
  }
}

void
CQChartsColumnEdit::
vheaderCheckClicked(bool b)
{
  if (! b)
    vheaderCheck_->setChecked(true);
  else {
    edit_->setText("@VH");

    menuColumnGroup_->setChecked(false);
    menuExprGroup_ ->setChecked(false);

    updateState();
  }
}

void
CQChartsColumnEdit::
menuColumnChanged(int i)
{
  edit_->setText(QString("%1").arg(i));
}

void
CQChartsColumnEdit::
expressionTextChanged(const QString &str)
{
  if (str.simplified().length())
    edit_->setText(QString("(%1)").arg(str));
  else
    edit_->setText("");
}

void
CQChartsColumnEdit::
updateState()
{
  columnCombo_   ->setEnabled(menuColumnGroup_->isChecked());
  expressionEdit_->setEnabled(menuExprGroup_  ->isChecked());
}

void
CQChartsColumnEdit::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focusrect and selected etc.
  QStyleOptionComboBox opt;

  initStyle(opt);

  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw the icon and text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void
CQChartsColumnEdit::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQChartsColumnEdit::
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

CQChartsColumnEditMenuButton::
CQChartsColumnEditMenuButton(QWidget *parent) :
 QPushButton(parent)
{
}

void
CQChartsColumnEditMenuButton::
paintEvent(QPaintEvent*)
{
  // drawn by CQChartsColumnEdit
}
