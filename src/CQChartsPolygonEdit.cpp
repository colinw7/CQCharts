#include <CQChartsPolygonEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsGeomPointEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQPixmapCache.h>
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

  setToolTip("Polygon Points");

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

const CQChartsPlot *
CQChartsPolygonLineEdit::
plot() const
{
  return dataEdit_->plot();
}

void
CQChartsPolygonLineEdit::
setPlot(CQChartsPlot *plot)
{
  dataEdit_->setPlot(plot);

  CQChartsLineEditBase::setPlot(plot);
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
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(polygonChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsPolygonLineEdit::
updateMenu()
{
  CQChartsLineEditBase::updateMenu();

  //---

  auto s = dataEdit_->sizeHint();

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
  auto str = (polygon().isValid() ? polygon().toString() : "<none>");

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
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  auto polygon = value.value<CQChartsPolygon>();

  auto str = polygon.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsPolygonPropertyViewType::
tip(const QVariant &value) const
{
  auto str = value.value<CQChartsPolygon>().toString();

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
  auto *edit = new CQChartsPolygonLineEdit(parent);

  return edit;
}

void
CQChartsPolygonPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsPolygonLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(polygonChanged()), obj, method);
}

QVariant
CQChartsPolygonPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsPolygonLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->polygon());
}

void
CQChartsPolygonPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsPolygonLineEdit *>(w);
  assert(edit);

  auto polygon = var.value<CQChartsPolygon>();

  edit->setPolygon(polygon);
}

//------

CQChartsPolygonEdit::
CQChartsPolygonEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("polygonEdit");

  setToolTip("Polygon Points");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  controlFrame_ = CQUtil::makeWidget<QFrame>("controlFrame");

  auto *controlFrameLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame_, 0, 2);

  layout->addWidget(controlFrame_);

  //---

  unitsEdit_ = new CQChartsUnitsEdit;

  //--

  auto createButton = [&](const QString &name, const QString &iconName, const QString &tip,
                          const char *receiver) {
    auto *button = CQUtil::makeWidget<QToolButton>(name);

    button->setIcon(CQPixmapCacheInst->getIcon(iconName));

    connect(button, SIGNAL(clicked()), this, receiver);

    button->setToolTip(tip);

    return button;
  };

  //--

  auto *addButton    = createButton("add"   , "ADD"   , "Add point"   , SLOT(addSlot()));
  auto *removeButton = createButton("remove", "REMOVE", "Remove point", SLOT(removeSlot()));

  controlFrameLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Units", "unitsLabel"));
  controlFrameLayout->addWidget(unitsEdit_);
  controlFrameLayout->addStretch(1);
  controlFrameLayout->addWidget(addButton);
  controlFrameLayout->addWidget(removeButton);

  //---

  scrollArea_ = CQUtil::makeWidget<QScrollArea>("scrollArea");

  pointsFrame_ = CQUtil::makeWidget<CQChartsPolygonEditPointsFrame>("pointsFrame");

  pointsFrame_->setEdit(this);

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

const CQChartsUnits &
CQChartsPolygonEdit::
units() const
{
  return polygon().units();
}

void
CQChartsPolygonEdit::
setUnits(const CQChartsUnits &units)
{
  auto polygon = polygon_;

  polygon.setUnits(units);

  setPolygon(polygon);
}

void
CQChartsPolygonEdit::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  for (auto &pointEdit : pointEdits_) {
    if (pointEdit)
      pointEdit->setPlot(plot);
  }

  CQChartsEditBase::setPlot(plot);
}

void
CQChartsPolygonEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  CQChartsWidgetUtil::connectDisconnect(connected_,
    unitsEdit_, SIGNAL(unitsChanged()), this, SLOT(unitsChanged()));
}

void
CQChartsPolygonEdit::
polygonToWidgets()
{
  connectSlots(false);

  //---

//const auto &polygon = polygon_.polygon();
  const auto &units   = polygon_.units();

  unitsEdit_->setUnits(units);

  int n  = numPoints();
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
  int n  = numPoints();
  int ne = pointEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    polygon_.setPoint(i, pointEdits_[i]->getValue());

  auto units = unitsEdit_->units();

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
  polygon_.addPoint(CQChartsGeom::Point());

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
  int n  = numPoints();
  int ne = pointEdits_.size();

  while (ne < n) {
    auto *edit = CQUtil::makeWidget<CQChartsGeomPointEdit>("edit");

    edit->setPlot(plot_);

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

int
CQChartsPolygonEdit::
numPoints() const
{
  return polygon_.numPoints();
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

  QStyleOptionSlider opt;

  int ss = style()->pixelMetric(QStyle::PM_ScrollBarExtent, &opt, this);

  int w = ew;
  int h = controlFrame_->sizeHint().height() + 4;

  int ch  = 0;
  int ch1 = 0;
  int sw  = 0;
//int sh  = 0;

  int n = numPoints();

  if (n > 0) {
    int n1 = std::min(n, 10);

    ch  = n *eh;
    ch1 = n1*eh;

    if (n1 < n) {
      sw = ss;
    //sh = ss;
    }
  }
  else {
    ch  = eh;
    ch1 = eh;
  }

  h += ch1;

  int w1 = w + sw;

  scrollSize = QSize(w1 + 4, ch1 + 4);
  pointsSize = QSize(w, ch);
  fullSize   = QSize(w1 + 8, h + 8);
}

//------

CQChartsPolygonEditPointsFrame::
CQChartsPolygonEditPointsFrame(CQChartsPolygonEdit *edit) :
 edit_(edit)
{
}

void
CQChartsPolygonEditPointsFrame::
paintEvent(QPaintEvent *e)
{
  QFrame::paintEvent(e);

  if (edit_->numPoints() == 0) {
    QFontMetrics fm(font());

    int x = 4;
    int y = height()/2 + (fm.ascent() - fm.descent())/2;

    QPainter painter(this);

    painter.drawText(x, y, "<No Points>");
  }
}
