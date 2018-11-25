#include <CQChartsPolygonEdit.h>

#include <CQPropertyView.h>
#include <CQPixmapCache.h>
#include <CQWidgetMenu.h>
#include <CQPoint2DEdit.h>

#include <QLineEdit>
#include <QComboBox>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QStylePainter>

#include <svg/add_svg.h>
#include <svg/remove_svg.h>

CQChartsPolygonEdit::
CQChartsPolygonEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("polygonEdit");

  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Sunken);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  //---

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  edit_ = new QLineEdit;
  edit_->setObjectName("edit");

  edit_->setFrame(false);

  connect(edit_, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));

  layout->addWidget(edit_);

  //---

  button_ = new CQChartsPolygonEditMenuButton;
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

  menuFrame_ = new QFrame;

  QVBoxLayout *menuFrameLayout = new QVBoxLayout(menuFrame_);
  menuFrameLayout->setMargin(0); menuFrameLayout->setSpacing(2);

  menu_->setWidget(menuFrame_);

  //---

  QFrame *unitsFrame = new QFrame;

  QHBoxLayout *unitsFrameLayout = new QHBoxLayout(unitsFrame);
  unitsFrameLayout->setMargin(0); unitsFrameLayout->setSpacing(2);

  unitsCombo_ = new QComboBox;

  unitsCombo_->addItems(CQChartsUtil::unitNames());

  unitsCombo_->setObjectName("units");

  unitsFrameLayout->addWidget(new QLabel("Units"));
  unitsFrameLayout->addWidget(unitsCombo_);
  unitsFrameLayout->addStretch(1);

  connect(unitsCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(unitsChanged()));

  menuFrameLayout->addWidget(unitsFrame);

  //---

  controlFrame_ = new QFrame;

  QHBoxLayout *controlFrameLayout = new QHBoxLayout(controlFrame_);
  controlFrameLayout->setMargin(0); controlFrameLayout->setSpacing(2);

  QToolButton *addButton    = new QToolButton;
  QToolButton *removeButton = new QToolButton;

  addButton   ->setIcon(CQPixmapCacheInst->getIcon("ADD"));
  removeButton->setIcon(CQPixmapCacheInst->getIcon("REMOVE"));

  controlFrameLayout->addStretch(1);
  controlFrameLayout->addWidget(addButton);
  controlFrameLayout->addWidget(removeButton);

  connect(addButton, SIGNAL(clicked()), this, SLOT(addSlot()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(removeSlot()));

  menuFrameLayout->addWidget(controlFrame_);

  //---

  scrollArea_ = new QScrollArea;

  pointsFrame_ = new QFrame;

  QVBoxLayout *pointsFrameLayout = new QVBoxLayout(pointsFrame_);
  pointsFrameLayout->setMargin(0); pointsFrameLayout->setSpacing(0);

  scrollArea_->setWidget(pointsFrame_);

  menuFrameLayout->addWidget(scrollArea_);

  //---

  updateState();
}

const CQChartsPolygon &
CQChartsPolygonEdit::
polygon() const
{
  return polygon_;
}

void
CQChartsPolygonEdit::
setPolygon(const CQChartsPolygon &polygon)
{
  polygon_ = polygon;

  polygonToWidgets();

  updateState();
}

void
CQChartsPolygonEdit::
widgetsToPolygon()
{
  QString text = edit_->text();

  QPolygonF polygon;

  CQChartsUtil::stringToPolygon(text, polygon);

  QString str = unitsCombo_->currentText();

  CQChartsUnits units;

  CQChartsUtil::decodeUnits(str, units);

  polygon_ = CQChartsPolygon(polygon, units);
}

void
CQChartsPolygonEdit::
polygonToWidgets()
{
  connectSlots(false);

  const QPolygonF     &polygon = polygon_.polygon();
  const CQChartsUnits &units   = polygon_.units();

  QString ustr = CQChartsUtil::unitsString(units);

  QString pstr = CQChartsUtil::polygonToString(polygon);

  edit_->setText(pstr);

  unitsCombo_->setCurrentIndex(unitsCombo_->findText(ustr, Qt::MatchExactly));

  connectSlots(true);
}

void
CQChartsPolygonEdit::
connectSlots(bool b)
{
  if (b) {
    connect(edit_, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged()));
  }
  else {
    disconnect(edit_, SIGNAL(textChanged(const QString &)),
               this, SLOT(textChanged()));
  }
}

QString
CQChartsPolygonEdit::
placeholderText() const
{
  return edit_->placeholderText();
}

void
CQChartsPolygonEdit::
setPlaceholderText(const QString &s)
{
  edit_->setPlaceholderText(s);
}

void
CQChartsPolygonEdit::
showMenu()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsPolygonEdit::
updateMenu()
{
  connectSlots(false);

  //---

  int n  = polygon_.numPoints();
  int ne = pointEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    pointEdits_[i]->setValue(polygon_.point(i));

  //---

  menu_->updateAreaSize();

  //---

  connectSlots(true);
}

void
CQChartsPolygonEdit::
textChanged()
{
  connectSlots(false);

  polygon_ = CQChartsPolygon(edit_->text());

  polygonToWidgets();

  connectSlots(true);

  emit polygonChanged();
}

void
CQChartsPolygonEdit::
unitsChanged()
{
  updateState();

  emit polygonChanged();
}

void
CQChartsPolygonEdit::
addSlot()
{
  polygon_.addPoint(QPointF());

  polygonToWidgets();

  updateState();
}

void
CQChartsPolygonEdit::
removeSlot()
{
  polygon_.removePoint();

  polygonToWidgets();

  updateState();
}

void
CQChartsPolygonEdit::
updateState()
{
  int ind = unitsCombo_->currentIndex();

  unitsCombo_->setToolTip(CQChartsUtil::unitTipNames()[ind]);

  //---

  QFontMetrics fm(font());

  int ew = 32*fm.width("8");
  int eh = fm.height() + 4;

  int w = ew;
  int h = 2*controlFrame_->sizeHint().height();

  int n  = polygon_.numPoints();
  int ch = n*eh;

  int n1  = std::min(n, 10);
  int ch1 = n1*eh;

  int sw = 0;

  if (n1 < n) {
    QStyleOptionSlider opt;

    sw = style()->pixelMetric(QStyle::PM_ScrollBarExtent, &opt, this);
  }

  h += ch1;

  int w1 = w + sw;

  //---

  int ne = pointEdits_.size();

  while (ne < n) {
    CQPoint2DEdit *edit = new CQPoint2DEdit;

    connect(edit, SIGNAL(valueChanged()), this, SLOT(pointSlot()));

    qobject_cast<QVBoxLayout *>(pointsFrame_->layout())->addWidget(edit);

    pointEdits_.push_back(edit);

    ++ne;
  }

  while (ne > n) {
    delete pointEdits_.back();

    pointEdits_.pop_back();

    --ne;
  }

  scrollArea_->setFixedSize(QSize(w1, ch1));

  pointsFrame_->setFixedSize(QSize(w, ch));

  menuFrame_->setFixedSize(QSize(w1, h));

  int bw = 2;

  menu_->setFixedSize(QSize(w1 + 2*bw, h + 2*bw));
}

void
CQChartsPolygonEdit::
pointSlot()
{
  int n  = polygon_.numPoints();
  int ne = pointEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    polygon_.setPoint(i, pointEdits_[i]->getQValue());

  polygonToWidgets();

  updateState();
}

void
CQChartsPolygonEdit::
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
CQChartsPolygonEdit::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQChartsPolygonEdit::
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

CQChartsPolygonEditMenuButton::
CQChartsPolygonEditMenuButton(QWidget *parent) :
 QPushButton(parent)
{
}

void
CQChartsPolygonEditMenuButton::
paintEvent(QPaintEvent*)
{
  // drawn by CQChartsPolygonEdit
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsPolygonPropertyViewType::
CQChartsPolygonPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsPolygonPropertyViewType::
getEditor() const
{
  return new CQChartsPolygonPropertyViewEditor;
}

bool
CQChartsPolygonPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsPolygonPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsPolygon polygon = value.value<CQChartsPolygon>();

  QString str = polygon.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsPolygonPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsPolygon>().toString();

  return str;
}

//------

CQChartsPolygonPropertyViewEditor::
CQChartsPolygonPropertyViewEditor()
{
}

QWidget *
CQChartsPolygonPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsPolygonEdit *edit = new CQChartsPolygonEdit(parent);

  return edit;
}

void
CQChartsPolygonPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsPolygonEdit *edit = qobject_cast<CQChartsPolygonEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(polygonChanged()), obj, method);
}

QVariant
CQChartsPolygonPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsPolygonEdit *edit = qobject_cast<CQChartsPolygonEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->polygon());
}

void
CQChartsPolygonPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsPolygonEdit *edit = qobject_cast<CQChartsPolygonEdit *>(w);
  assert(edit);

  CQChartsPolygon polygon = var.value<CQChartsPolygon>();

  edit->setPolygon(polygon);
}
