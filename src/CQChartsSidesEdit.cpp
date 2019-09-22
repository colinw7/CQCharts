#include <CQChartsSidesEdit.h>
#include <CQChartsUtil.h>

#include <CQWidgetMenu.h>
#include <CQStrParse.h>

#include <QStylePainter>
#include <QStyleOptionComboBox>
#include <QMouseEvent>

//------

CQChartsSidesEdit::
CQChartsSidesEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("sides");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

CQChartsSidesEdit::
~CQChartsSidesEdit()
{
}

const CQChartsSides &
CQChartsSidesEdit::
sides() const
{
  return sides_;
}

void
CQChartsSidesEdit::
setSides(const CQChartsSides &sides)
{
  sides_ = sides;

  update();

  emit sidesChanged();
}

void
CQChartsSidesEdit::
mousePressEvent(QMouseEvent *e)
{
  QPoint p = e->pos();

  QStyleOptionComboBox opt;

  initStyleOption(opt);

  if (! opt.rect.contains(p))
    return;

  CQWidgetMenu *menu = new CQWidgetMenu;

  CQChartsSidesEditMenuWidget *widget = new CQChartsSidesEditMenuWidget(this);

  menu->setWidget(widget);

  QPoint gpos = mapToGlobal(rect().bottomLeft());

  menu->exec(gpos);

  delete menu;
}

void
CQChartsSidesEdit::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  //---

  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focus rect and selected etc.
  QStyleOptionComboBox opt;

  initStyleOption(opt);

  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw the control (no text)
  opt.currentText = "";

  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);

  //---

  QRect r = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);

  painter.setClipRect(r);

  //---

  QString str = sides_.toString();

  QFontMetrics fm(font());

  painter.drawText(r.left(), r.center().y() + (fm.ascent() - fm.descent())/2, str);
}

QSize
CQChartsSidesEdit::
sizeHint() const
{
  QString str = sides_.toString();

  QFontMetrics fm(font());

  QSize fs(fm.width(str) + 4, fm.height() + 4);

  QStyleOptionComboBox opt;

  initStyleOption(opt);

  QRect popupRect =
    style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  return QSize(fs.width() + popupRect.width(), fs.height());
}

void
CQChartsSidesEdit::
initStyleOption(QStyleOptionComboBox &opt) const
{
  opt.rect  = rect();
  opt.state = QStyle::State_Enabled;
}

//-----

CQChartsSidesEditMenuWidget::
CQChartsSidesEditMenuWidget(CQChartsSidesEdit *edit) :
 edit_(edit)
{
  setMouseTracking(true);
}

void
CQChartsSidesEditMenuWidget::
resizeEvent(QResizeEvent *)
{
  QRect rect = this->rect();

  int m = 4;
  int s = 8;

  int x1 = rect.left  () + m;
  int y1 = rect.top   () + m;
  int x2 = rect.right () - m;
  int y2 = rect.bottom() - m;

  sideRect_[CQChartsSides::Side::LEFT   ] = Rect(QRect(x1      , y1, s, y2 - y1));
  sideRect_[CQChartsSides::Side::RIGHT  ] = Rect(QRect(x2 - s  , y1, s, y2 - y1));
  sideRect_[CQChartsSides::Side::TOP    ] = Rect(QRect(x1, y1      , x2 - x1, s));
  sideRect_[CQChartsSides::Side::BOTTOM ] = Rect(QRect(x1, y2 - s  , x2 - x1, s));
}

void
CQChartsSidesEditMenuWidget::
paintEvent(QPaintEvent *)
{
  QColor bg = palette().color(QPalette::Window);

  QRect rect = this->rect();

  QPainter painter(this);

  painter.setBrush(bg);

  painter.fillRect(rect, painter.brush());

  for (int i = 0; i < 2; ++i) {
    drawSideRect(&painter, CQChartsSides::Side::LEFT  , i == 1);
    drawSideRect(&painter, CQChartsSides::Side::RIGHT , i == 1);
    drawSideRect(&painter, CQChartsSides::Side::TOP   , i == 1);
    drawSideRect(&painter, CQChartsSides::Side::BOTTOM, i == 1);
  }
}

void
CQChartsSidesEditMenuWidget::
mousePressEvent(QMouseEvent *me)
{
  CQChartsSides::Sides sides = edit_->sides().sides();

  for (auto &rect : sideRect_) {
    if (! rect.second.r.contains(me->pos()))
      continue;

    if (sides & int(rect.first))
      sides &= ~int(rect.first);
    else
      sides |= int(rect.first);
  }

  edit_->setSides(CQChartsSides(sides));

  update();
}

void
CQChartsSidesEditMenuWidget::
mouseMoveEvent(QMouseEvent *me)
{
  for (auto &rect : sideRect_)
    rect.second.inside = rect.second.r.contains(me->pos());

  update();
}

void
CQChartsSidesEditMenuWidget::
drawSideRect(QPainter *p, CQChartsSides::Side rectSide, bool on)
{
  CQChartsSides::Sides sides = edit_->sides().sides();

  QColor bg       = palette().color(QPalette::Window);
  QColor onColor  = QColor("#586e75");
  QColor offColor = CQChartsUtil::blendColors(onColor, bg, 0.5);

  QColor highlight = palette().color(QPalette::Highlight);

  if (! sideRect_[rectSide].inside) {
    if (sides & int(rectSide)) {
      if (! on)
        return;

      p->setBrush(onColor);
    }
    else {
      if (on)
        return;

      p->setBrush(offColor);
    }
  }
  else {
    p->setBrush(highlight);
  }

  p->fillRect(sideRect_[rectSide].r, p->brush());
}

QSize
CQChartsSidesEditMenuWidget::
sizeHint() const
{
  return QSize(128, 128);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsSidesPropertyViewType::
CQChartsSidesPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsSidesPropertyViewType::
getEditor() const
{
  return new CQChartsSidesPropertyViewEditor;
}

bool
CQChartsSidesPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsSidesPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsSides sides = value.value<CQChartsSides>();

  QString str = sides.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsSidesPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsSides>().toString();

  return str;
}

//------

CQChartsSidesPropertyViewEditor::
CQChartsSidesPropertyViewEditor()
{
}

QWidget *
CQChartsSidesPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsSidesEdit *edit = new CQChartsSidesEdit(parent);

  return edit;
}

void
CQChartsSidesPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsSidesEdit *edit = qobject_cast<CQChartsSidesEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(sidesChanged()), obj, method);
}

QVariant
CQChartsSidesPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsSidesEdit *edit = qobject_cast<CQChartsSidesEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->sides());
}

void
CQChartsSidesPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsSidesEdit *edit = qobject_cast<CQChartsSidesEdit *>(w);
  assert(edit);

  CQChartsSides sides = var.value<CQChartsSides>();

  edit->setSides(sides);
}
