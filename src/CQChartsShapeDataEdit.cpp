#include <CQChartsShapeDataEdit.h>

#include <CQChartsStrokeDataEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsShapeDataLineEdit::
CQChartsShapeDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("shapeDataLineEdit");

  //---

  menuEdit_ = new CQChartsShapeDataEdit;

  menu_->setWidget(menuEdit_);

  connect(menuEdit_, SIGNAL(shapeDataChanged()), this, SLOT(menuEditChanged()));
}

CQChartsPlot *
CQChartsShapeDataLineEdit::
plot() const
{
  return menuEdit_->plot();
}

void
CQChartsShapeDataLineEdit::
setPlot(CQChartsPlot *plot)
{
  menuEdit_->setPlot(plot);
}

CQChartsView *
CQChartsShapeDataLineEdit::
view() const
{
  return menuEdit_->view();
}

void
CQChartsShapeDataLineEdit::
setView(CQChartsView *view)
{
  menuEdit_->setView(view);
}

const CQChartsShapeData &
CQChartsShapeDataLineEdit::
shapeData() const
{
  return menuEdit_->data();
}

void
CQChartsShapeDataLineEdit::
setShapeData(const CQChartsShapeData &shapeData)
{
  updateShapeData(shapeData, /*updateText*/ true);
}

void
CQChartsShapeDataLineEdit::
updateShapeData(const CQChartsShapeData &shapeData, bool updateText)
{
  connectSlots(false);

  menuEdit_->setData(shapeData);

  if (updateText)
    shapeDataToWidgets();

  connectSlots(true);

  emit shapeDataChanged();
}

void
CQChartsShapeDataLineEdit::
textChanged()
{
  CQChartsShapeData shapeData(edit_->text());

  if (! shapeData.isValid())
    return;

  updateShapeData(shapeData, /*updateText*/ false);
}

void
CQChartsShapeDataLineEdit::
shapeDataToWidgets()
{
  connectSlots(false);

  if (shapeData().isValid())
    edit_->setText(shapeData().toString());
  else
    edit_->setText("");

  connectSlots(true);
}

void
CQChartsShapeDataLineEdit::
menuEditChanged()
{
  shapeDataToWidgets();

  emit shapeDataChanged();
}

void
CQChartsShapeDataLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(menuEdit_, SIGNAL(shapeDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(menuEdit_, SIGNAL(shapeDataChanged()), this, SLOT(menuEditChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsShapeDataPropertyViewType::
CQChartsShapeDataPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsShapeDataPropertyViewType::
getEditor() const
{
  return new CQChartsShapeDataPropertyViewEditor;
}

bool
CQChartsShapeDataPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsShapeDataPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->drawItem();

  QObject *obj = (item ? item->object() : nullptr);

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);
  CQChartsView *view = qobject_cast<CQChartsView *>(obj);

  //---

  delegate->drawBackground(painter, option, ind, inside);

  CQChartsShapeData data = value.value<CQChartsShapeData>();

  QColor pc, fc;

  if      (plot) {
    pc = plot->charts()->interpColor(data.border().color(), 0, 1);
    fc = plot->charts()->interpColor(data.background().color(), 0, 1);
  }
  else if (view) {
    pc = view->charts()->interpColor(data.border().color(), 0, 1);
    fc = view->charts()->interpColor(data.background().color(), 0, 1);
  }
  else {
    pc = data.border().color().color();
    fc = data.background().color().color();
  }

  double width = CQChartsUtil::limitLineWidth(data.border().width().value());

  QPen pen;

  CQChartsUtil::setPen(pen, data.border().isVisible(), pc, data.border().alpha(),
                       width, data.border().dash());

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.background().isVisible(), fc, data.background().alpha(),
                         data.background().pattern());

  painter->setPen  (pen);
  painter->setBrush(brush);

  double cxs = data.border().cornerSize().value();
  double cys = data.border().cornerSize().value();

  CQChartsRoundedPolygon::draw(painter, option.rect, cxs, cys);
}

QString
CQChartsShapeDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsShapeData>().toString();

  return str;
}

//------

CQChartsShapeDataPropertyViewEditor::
CQChartsShapeDataPropertyViewEditor()
{
}

QWidget *
CQChartsShapeDataPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  QObject *obj = (item ? item->object() : nullptr);

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);
  CQChartsView *view = qobject_cast<CQChartsView *>(obj);

  CQChartsShapeDataLineEdit *edit = new CQChartsShapeDataLineEdit(parent);

  if      (plot) edit->setPlot(plot);
  else if (view) edit->setView(view);

  return edit;
}

void
CQChartsShapeDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsShapeDataLineEdit *edit = qobject_cast<CQChartsShapeDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(shapeDataChanged()), obj, method);
}

QVariant
CQChartsShapeDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsShapeDataLineEdit *edit = qobject_cast<CQChartsShapeDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->shapeData());
}

void
CQChartsShapeDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsShapeDataLineEdit *edit = qobject_cast<CQChartsShapeDataLineEdit *>(w);
  assert(edit);

  CQChartsShapeData data = var.value<CQChartsShapeData>();

  edit->setShapeData(data);
}

//------

CQChartsShapeDataEdit::
CQChartsShapeDataEdit(QWidget *parent) :
 QFrame(parent)
{
  QGridLayout *layout = new QGridLayout(this);

  //---

  // background
  fillEdit_ = new CQChartsFillDataEdit;

  fillEdit_->setTitle  ("Fill");
  fillEdit_->setPreview(false);

  connect(fillEdit_, SIGNAL(fillDataChanged()), this, SLOT(widgetsToData()));

  layout->addWidget(fillEdit_, 0, 0, 1, 2);

  // border
  strokeEdit_ = new CQChartsStrokeDataEdit;

  strokeEdit_->setTitle("Stroke");
  strokeEdit_->setPreview(false);

  connect(strokeEdit_, SIGNAL(strokeDataChanged()), this, SLOT(widgetsToData()));

  layout->addWidget(strokeEdit_, 1, 0, 1, 2);

  //---

  preview_ = new CQChartsShapeDataEditPreview(this);

  layout->addWidget(preview_, 2, 1);

  //---

  layout->setRowStretch(3, 1);

  //---

  dataToWidgets();
}

void
CQChartsShapeDataEdit::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  fillEdit_  ->setPlot(plot_);
  strokeEdit_->setPlot(plot_);
}

void
CQChartsShapeDataEdit::
setView(CQChartsView *view)
{
  view_ = view;

  fillEdit_  ->setView(view_);
  strokeEdit_->setView(view_);
}

void
CQChartsShapeDataEdit::
dataToWidgets()
{
  disconnect(fillEdit_, SIGNAL(fillDataChanged()), this, SLOT(widgetsToData()));
  disconnect(strokeEdit_, SIGNAL(strokeDataChanged()), this, SLOT(widgetsToData()));

  fillEdit_  ->setData(data_.background());
  strokeEdit_->setData(data_.border());

  preview_->update();

  connect(fillEdit_, SIGNAL(fillDataChanged()), this, SLOT(widgetsToData()));
  connect(strokeEdit_, SIGNAL(strokeDataChanged()), this, SLOT(widgetsToData()));

}

void
CQChartsShapeDataEdit::
widgetsToData()
{
  data_.setBackground(fillEdit_  ->data());
  data_.setBorder    (strokeEdit_->data());

  preview_->update();

  emit shapeDataChanged();
}

//------

CQChartsShapeDataEditPreview::
CQChartsShapeDataEditPreview(CQChartsShapeDataEdit *edit) :
 edit_(edit)
{
}

void
CQChartsShapeDataEditPreview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  const CQChartsShapeData &data = edit_->data();

  QColor pc, fc;

  if      (edit_->plot()) {
    pc = edit_->plot()->charts()->interpColor(data.border().color(), 0, 1);
    fc = edit_->plot()->charts()->interpColor(data.background().color(), 0, 1);
  }
  else if (edit_->view()) {
    pc = edit_->view()->charts()->interpColor(data.border().color(), 0, 1);
    fc = edit_->view()->charts()->interpColor(data.background().color(), 0, 1);
  }
  else {
    pc = data.border().color().color();
    fc = data.background().color().color();
  }

  double width = CQChartsUtil::limitLineWidth(data.border().width().value());

  QPen pen;

  CQChartsUtil::setPen(pen, data.border().isVisible(), pc, data.border().alpha(),
                       width, data.border().dash());

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.background().isVisible(), fc, data.background().alpha(),
                         data.background().pattern());

  painter.setPen  (pen);
  painter.setBrush(brush);

  double cxs = data.border().cornerSize().value();
  double cys = data.border().cornerSize().value();

  CQChartsRoundedPolygon::draw(&painter, rect(), cxs, cys);
}

QSize
CQChartsShapeDataEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XXXX"), fm.height() + 4);
}
