#include <CQChartsPolygonEdit.h>
#include <CQChartsUnitsEdit.h>

#include <CQPropertyView.h>
#include <CQPixmapCache.h>
#include <CQPoint2DEdit.h>
#include <CQWidgetMenu.h>

#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPainter>

#include <svg/add_svg.h>
#include <svg/remove_svg.h>

CQChartsPolygonLineEdit::
CQChartsPolygonLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("polygonEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsPolygonEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);
}

const CQChartsPolygon &
CQChartsPolygonLineEdit::
polygon() const
{
  return dataEdit_->polygon();
}

void
CQChartsPolygonLineEdit::
setPolygon(const CQChartsPolygon &polygon)
{
  updatePolygon(polygon, /*updateText*/ true);
}

void
CQChartsPolygonLineEdit::
updatePolygon(const CQChartsPolygon &polygon, bool updateText)
{
  connectSlots(false);

  dataEdit_->setPolygon(polygon);

  connectSlots(true);

  if (updateText)
    polygonToWidgets();

  emit polygonChanged();
}

void
CQChartsPolygonLineEdit::
textChanged()
{
  CQChartsPolygon polygon(edit_->text());

  if (! polygon.isValid())
    return;

  updatePolygon(polygon, /*updateText*/ false);
}

void
CQChartsPolygonLineEdit::
polygonToWidgets()
{
  connectSlots(false);

  if (polygon().isValid())
    edit_->setText(polygon().toString());
  else
    edit_->setText("");

  setToolTip(polygon().toString());

  connectSlots(true);
}

void
CQChartsPolygonLineEdit::
menuEditChanged()
{
  updateMenu();

  polygonToWidgets();

  emit polygonChanged();
}

void
CQChartsPolygonLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(polygonChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(polygonChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsPolygonLineEdit::
updateMenu()
{
  CQChartsLineEditBase::updateMenu();

  //---

  QSize s = dataEdit_->sizeHint();

  int w = std::max(this->width(), s.width());
  int h = s.height();

  //---

  dataEdit_->setFixedSize(w, h);

  int bw = 2;

  menu_->setFixedSize(QSize(w + 2*bw, h + 2*bw));

  menu_->updateAreaSize();
}

void
CQChartsPolygonLineEdit::
drawPreview(QPainter *painter, const QRect &)
{
  QString str = (polygon().isValid() ? polygon().toString() : "<none>");

  drawCenteredText(painter, str);
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
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
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
  CQChartsPolygonLineEdit *edit = new CQChartsPolygonLineEdit(parent);

  return edit;
}

void
CQChartsPolygonPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsPolygonLineEdit *edit = qobject_cast<CQChartsPolygonLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(polygonChanged()), obj, method);
}

QVariant
CQChartsPolygonPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsPolygonLineEdit *edit = qobject_cast<CQChartsPolygonLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->polygon());
}

void
CQChartsPolygonPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsPolygonLineEdit *edit = qobject_cast<CQChartsPolygonLineEdit *>(w);
  assert(edit);

  CQChartsPolygon polygon = var.value<CQChartsPolygon>();

  edit->setPolygon(polygon);
}

//------

CQChartsPolygonEdit::
CQChartsPolygonEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("polygonEdit");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  controlFrame_ = CQUtil::makeWidget<QFrame>("controlFrame");

  QHBoxLayout *controlFrameLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame_, 0, 2);

  layout->addWidget(controlFrame_);

  //---

  unitsEdit_ = new CQChartsUnitsEdit;

  //--

  auto createButton = [&](const QString &name, const QString &iconName, const QString &tip,
                          const char *receiver) {
    QToolButton *button = CQUtil::makeWidget<QToolButton>(name);

    button->setIcon(CQPixmapCacheInst->getIcon(iconName));

    connect(button, SIGNAL(clicked()), this, receiver);

    button->setToolTip(tip);

    return button;
  };

  //--

  QToolButton *addButton    = createButton("add"   , "ADD"   , "Add point"   , SLOT(addSlot()));
  QToolButton *removeButton = createButton("remove", "REMOVE", "Remove point", SLOT(removeSlot()));

  controlFrameLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Units", "unitsLabel"));
  controlFrameLayout->addWidget(unitsEdit_);
  controlFrameLayout->addStretch(1);
  controlFrameLayout->addWidget(addButton);
  controlFrameLayout->addWidget(removeButton);

  //---

  scrollArea_ = CQUtil::makeWidget<QScrollArea>("scrollArea");

  pointsFrame_ = CQUtil::makeWidget<QFrame>("pointsFrame");

  (void) CQUtil::makeLayout<QVBoxLayout>(pointsFrame_, 0, 0);

  scrollArea_->setWidget(pointsFrame_);

  layout->addWidget(scrollArea_);

  //---

  connectSlots(true);

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

  updatePointEdits();

  polygonToWidgets();

  updateState();
}

void
CQChartsPolygonEdit::
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

  connectDisconnect(b, unitsEdit_, SIGNAL(unitsChanged()), SLOT(unitsChanged()));
}

void
CQChartsPolygonEdit::
polygonToWidgets()
{
  connectSlots(false);

  //---

//const QPolygonF     &polygon = polygon_.polygon();
  const CQChartsUnits &units   = polygon_.units();

  unitsEdit_->setUnits(units);

  int n  = polygon_.numPoints();
  int ne = pointEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    pointEdits_[i]->setValue(polygon_.point(i));

  //---

  connectSlots(true);
}

void
CQChartsPolygonEdit::
widgetsToPolygon()
{
  int n  = polygon_.numPoints();
  int ne = pointEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    polygon_.setPoint(i, pointEdits_[i]->getQValue());

  CQChartsUnits units = unitsEdit_->units();

  polygon_.setUnits(units);
}

void
CQChartsPolygonEdit::
unitsChanged()
{
  widgetsToPolygon();

  emit polygonChanged();
}

void
CQChartsPolygonEdit::
addSlot()
{
  polygon_.addPoint(QPointF());

  updatePointEdits();

  widgetsToPolygon();

  updateState();

  emit polygonChanged();
}

void
CQChartsPolygonEdit::
removeSlot()
{
  polygon_.removePoint();

  updatePointEdits();

  widgetsToPolygon();

  updateState();

  emit polygonChanged();
}

void
CQChartsPolygonEdit::
pointSlot()
{
  widgetsToPolygon();

  updateState();

  emit polygonChanged();
}

void
CQChartsPolygonEdit::
updateState()
{
  QSize scrollSize, pointsSize, fullSize;

  calcSizes(scrollSize, pointsSize, fullSize);

  scrollArea_->setFixedSize(scrollSize);

  pointsFrame_->setFixedSize(pointsSize);
}

void
CQChartsPolygonEdit::
updatePointEdits()
{
  int n = polygon_.numPoints();

  int ne = pointEdits_.size();

  while (ne < n) {
    CQPoint2DEdit *edit = CQUtil::makeWidget<CQPoint2DEdit>("edit");

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
}

QSize
CQChartsPolygonEdit::
sizeHint() const
{
  QSize scrollSize, pointsSize, fullSize;

  calcSizes(scrollSize, pointsSize, fullSize);

  return fullSize;
}

void
CQChartsPolygonEdit::
calcSizes(QSize &scrollSize, QSize &pointsSize, QSize &fullSize) const
{
  QFontMetrics fm(font());

  int ew = 32*fm.width("8");
  int eh = fm.height() + 4;

  int w = ew;
  int h = controlFrame_->sizeHint().height() + 4;

  int n  = polygon_.numPoints();
  int ch = n*eh;

  int n1  = std::min(n, 10);
  int ch1 = n1*eh;

  QStyleOptionSlider opt;

  int ss = style()->pixelMetric(QStyle::PM_ScrollBarExtent, &opt, this);

  int sw = 0;
//int sh = 0;

  if (n1 < n) {
    sw = ss;
//  sh = ss;
  }

  h += ch1;

  int w1 = w + sw;

  scrollSize = QSize(w1, ch1);

  pointsSize = QSize(w, ch);

  fullSize = QSize(w1 + 8, h + 8);
}
