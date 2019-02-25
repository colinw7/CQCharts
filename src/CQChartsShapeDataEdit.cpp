#include <CQChartsShapeDataEdit.h>

#include <CQChartsStrokeDataEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQUtil.h>

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

  menuEdit_ = dataEdit_ = new CQChartsShapeDataEdit;

  menu_->setWidget(dataEdit_);

  connect(dataEdit_, SIGNAL(shapeDataChanged()), this, SLOT(menuEditChanged()));

  //---

  shapeDataToWidgets();
}

const CQChartsShapeData &
CQChartsShapeDataLineEdit::
shapeData() const
{
  return dataEdit_->data();
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

  dataEdit_->setData(shapeData);

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

  setToolTip(shapeData().toString());

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
    connect(dataEdit_, SIGNAL(shapeDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(shapeDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsShapeDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsShapeDataEditPreview::draw(painter, shapeData(), rect, plot(), view());
}

//------

CQPropertyViewEditorFactory *
CQChartsShapeDataPropertyViewType::
getEditor() const
{
  return new CQChartsShapeDataPropertyViewEditor;
}

void
CQChartsShapeDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  CQChartsShapeData data = value.value<CQChartsShapeData>();

  CQChartsShapeDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsShapeDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsShapeData>().toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsShapeDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  return new CQChartsShapeDataLineEdit(parent);
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
 CQChartsEditBase(parent)
{
  setObjectName("shapeDataEdit");

  QGridLayout *layout = new QGridLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

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
setData(const CQChartsShapeData &d)
{
  data_ = d;

  dataToWidgets();
}

void
CQChartsShapeDataEdit::
setPlot(CQChartsPlot *plot)
{
  CQChartsEditBase::setPlot(plot);

  fillEdit_  ->setPlot(plot);
  strokeEdit_->setPlot(plot);
}

void
CQChartsShapeDataEdit::
setView(CQChartsView *view)
{
  CQChartsEditBase::setView(view);

  fillEdit_  ->setView(view);
  strokeEdit_->setView(view);
}

void
CQChartsShapeDataEdit::
setTitle(const QString &)
{
  fillEdit_  ->setTitle("Fill");
  strokeEdit_->setTitle("Stroke");
}

void
CQChartsShapeDataEdit::
setPreview(bool b)
{
  fillEdit_  ->setPreview(b);
  strokeEdit_->setPreview(b);

  preview_->setVisible(b);
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
 CQChartsEditPreview(edit), edit_(edit)
{
}

void
CQChartsShapeDataEditPreview::
draw(QPainter *painter)
{
  const CQChartsShapeData &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsShapeDataEditPreview::
draw(QPainter *painter, const CQChartsShapeData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen and brush
  QColor pc = interpColor(plot, view, data.border    ().color());
  QColor fc = interpColor(plot, view, data.background().color());

  double width = CQChartsUtil::limitLineWidth(data.border().width().value());

  QPen pen;

  CQChartsUtil::setPen(pen, data.border().isVisible(), pc, data.border().alpha(),
                       width, data.border().dash());

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.background().isVisible(), fc, data.background().alpha(),
                         data.background().pattern());

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw shape
  double cxs = data.border().cornerSize().value();
  double cys = data.border().cornerSize().value();

  CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
}
