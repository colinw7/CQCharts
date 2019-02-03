#include <CQChartsLineEditBase.h>
#include <CQWidgetMenu.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QStylePainter>

CQChartsLineEditBase::
CQChartsLineEditBase(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("editBase");

  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Sunken);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  //---

  layout_ = new QHBoxLayout(this);
  layout_->setMargin(0); layout_->setSpacing(2);

  //---

  edit_ = new QLineEdit;
  edit_->setObjectName("edit");

  edit_->setFrame(false);

  connect(edit_, SIGNAL(textChanged(const QString &)), this, SLOT(textChangedSlot()));

  layout_->addWidget(edit_);

  //---

  button_ = new CQChartsLineEditMenuButton;
  button_->setObjectName("button");

  QStyleOptionComboBox opt;

  initStyle(opt);

  QRect r = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  button_->setFixedWidth(r.size().width());

  button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  button_->setFocusPolicy(Qt::NoFocus);

  connect(button_, SIGNAL(clicked()), this, SLOT(showMenuSlot()));

  layout_->addWidget(button_);

  //---

  menu_ = new CQWidgetMenu(this);

  connect(menu_, SIGNAL(menuShown()), this, SLOT(updateMenuSlot()));
}

QString
CQChartsLineEditBase::
text() const
{
  return edit_->text();
}

void
CQChartsLineEditBase::
setText(const QString &s)
{
  edit_->setText(s);
}

QString
CQChartsLineEditBase::
placeholderText() const
{
  return edit_->placeholderText();
}

void
CQChartsLineEditBase::
setPlaceholderText(const QString &s)
{
  edit_->setPlaceholderText(s);
}

void
CQChartsLineEditBase::
textChangedSlot()
{
  textChanged();
}

void
CQChartsLineEditBase::
showMenuSlot()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsLineEditBase::
updateMenuSlot()
{
  updateMenu();
}

void
CQChartsLineEditBase::
updateMenu()
{
  int w = std::max(menu_->sizeHint().width(), this->width());

  menu_->setFixedWidth(w);

  menu_->updateAreaSize();
}

void
CQChartsLineEditBase::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, edit_, SIGNAL(textChanged(const QString &)),
                    SLOT(textChangedSlot()));
}

void
CQChartsLineEditBase::
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
CQChartsLineEditBase::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQChartsLineEditBase::
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

CQChartsLineEditMenuButton::
CQChartsLineEditMenuButton(QWidget *parent) :
 QPushButton(parent)
{
}

void
CQChartsLineEditMenuButton::
paintEvent(QPaintEvent*)
{
  // drawn by CQChartsLineEditBase
}
