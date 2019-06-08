#include <CQChartsLineEditBase.h>
#include <CQChartsEditBase.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQCharts.h>
#include <CQChartsPropertyViewTree.h>

#include <CQWidgetMenu.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QStylePainter>
#include <QStyleOptionComboBox>
#include <cassert>

CQChartsLineEditBase::
CQChartsLineEditBase(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("editBase");

  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Sunken);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  //---

  layout_ = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = new CQChartsLineEditEdit(this);

//connect(edit_, SIGNAL(textChanged(const QString &)), this, SLOT(textChangedSlot()));
  connect(edit_, SIGNAL(editingFinished()), this, SLOT(textChangedSlot()));

  layout_->addWidget(edit_);

  //---

  button_ = new CQChartsLineEditMenuButton(this);

  connect(button_, SIGNAL(clicked()), this, SLOT(menuButtonSlot()));

  layout_->addWidget(button_);

  //---

  menu_ = new CQWidgetMenu(this);

  connect(menu_, SIGNAL(menuShown ()), this, SLOT(showMenuSlot()));
  connect(menu_, SIGNAL(menuHidden()), this, SLOT(hideMenuSlot()));

  //---

  setFocusProxy(edit_);
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
setEditable(bool b)
{
  editable_ = b;

  edit_->setReadOnly(! b);

  update();
}

CQChartsPlot *
CQChartsLineEditBase::
plot() const
{
  return (menuEdit_ ? menuEdit_->plot() : nullptr);
}

void
CQChartsLineEditBase::
setPlot(CQChartsPlot *plot)
{
  assert(menuEdit_);

  menuEdit_->setPlot(plot);
}

CQChartsView *
CQChartsLineEditBase::
view() const
{
  return (menuEdit_ ? menuEdit_->view() : nullptr);
}

void
CQChartsLineEditBase::
setView(CQChartsView *view)
{
  assert(menuEdit_);

  menuEdit_->setView(view);
}

void
CQChartsLineEditBase::
textChangedSlot()
{
  textChanged();
}

void
CQChartsLineEditBase::
menuButtonSlot()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsLineEditBase::
showMenuSlot()
{
  updateMenu();
}

void
CQChartsLineEditBase::
hideMenuSlot()
{
  if (propertyViewTree_)
    propertyViewTree_->closeCurrentEditor();
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
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

//connectDisconnect(b, edit_, SIGNAL(textChanged(const QString &)), SLOT(textChangedSlot()));
  connectDisconnect(b, edit_, SIGNAL(editingFinished()), SLOT(textChangedSlot()));
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

void
CQChartsLineEditBase::
drawPreview(QPainter *painter, const QRect &)
{
  drawBackground(painter);

  //---

  drawCenteredText(painter, "Preview");
}

void
CQChartsLineEditBase::
drawBackground(QPainter *painter) const
{
  QColor c = palette().color(QPalette::Window);

  painter->fillRect(rect(), QBrush(c));

  tc_ = CQChartsUtil::bwColor(c);
}

void
CQChartsLineEditBase::
drawCenteredText(QPainter *painter, const QString &text) const
{
  painter->setPen(tc_);

  QFontMetricsF fm(font());

  double fa = fm.ascent();
  double fd = fm.descent();

  painter->drawText(rect().left() + 2, rect().center().y() + (fa - fd)/2, text);
}

QColor
CQChartsLineEditBase::
interpColor(const CQChartsColor &color) const
{
  QColor c;

  if      (plot())
    return plot()->interpColor(color, CQChartsUtil::ColorInd());
  else if (view())
    return view()->interpColor(color, CQChartsUtil::ColorInd());
  else
    return color.color();
}

//------

CQChartsLineEditEdit::
CQChartsLineEditEdit(CQChartsLineEditBase *edit) :
 CQLineEdit(edit), edit_(edit)
{
  setObjectName("edit");

  setFrame(false);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void
CQChartsLineEditEdit::
paintEvent(QPaintEvent *e)
{
  if (edit_->isEditable())
    CQLineEdit::paintEvent(e);
  else {
    QPainter painter(this);

    edit_->drawBackground(&painter);

    edit_->drawPreview(&painter, rect());
  }
}

//------

CQChartsLineEditMenuButton::
CQChartsLineEditMenuButton(CQChartsLineEditBase *edit) :
 QPushButton(edit), edit_(edit)
{
  setObjectName("button");

  //---

  QStyleOptionComboBox opt;

  edit_->initStyle(opt);

  QRect r = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  setFixedWidth(r.size().width());

  //---

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFocusPolicy(Qt::NoFocus);
}

void
CQChartsLineEditMenuButton::
paintEvent(QPaintEvent *)
{
  // drawn by CQChartsLineEditBase
}
