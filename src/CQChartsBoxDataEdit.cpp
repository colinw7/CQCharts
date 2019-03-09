#include <CQChartsBoxDataEdit.h>

#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CQChartsBoxDataLineEdit::
CQChartsBoxDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("boxDataLineEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsBoxDataEdit;

  menu_->setWidget(dataEdit_);

  connect(dataEdit_, SIGNAL(boxDataChanged()), this, SLOT(menuEditChanged()));

  //---

  boxDataToWidgets();
}

const CQChartsBoxData &
CQChartsBoxDataLineEdit::
boxData() const
{
  return dataEdit_->data();
}

void
CQChartsBoxDataLineEdit::
setBoxData(const CQChartsBoxData &boxData)
{
  updateBoxData(boxData, /*updateText*/ true);
}

void
CQChartsBoxDataLineEdit::
updateBoxData(const CQChartsBoxData &boxData, bool updateText)
{
  connectSlots(false);

  dataEdit_->setData(boxData);

  if (updateText)
    boxDataToWidgets();

  connectSlots(true);

  emit boxDataChanged();
}

void
CQChartsBoxDataLineEdit::
textChanged()
{
  CQChartsBoxData boxData(edit_->text());

  if (! boxData.isValid())
    return;

  updateBoxData(boxData, /*updateText*/ false);
}

void
CQChartsBoxDataLineEdit::
boxDataToWidgets()
{
  connectSlots(false);

  if (boxData().isValid())
    edit_->setText(boxData().toString());
  else
    edit_->setText("");

  setToolTip(boxData().toString());

  connectSlots(true);
}

void
CQChartsBoxDataLineEdit::
menuEditChanged()
{
  boxDataToWidgets();

  emit boxDataChanged();
}

void
CQChartsBoxDataLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(boxDataChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(boxDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsBoxDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsBoxData data = this->boxData();

  CQChartsBoxDataEditPreview::draw(painter, data, rect, plot(), view());
}

//------

CQPropertyViewEditorFactory *
CQChartsBoxDataPropertyViewType::
getEditor() const
{
  return new CQChartsBoxDataPropertyViewEditor;
}

void
CQChartsBoxDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  CQChartsBoxData data = value.value<CQChartsBoxData>();

  CQChartsBoxDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsBoxDataPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsBoxData>().toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsBoxDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  return new CQChartsBoxDataLineEdit(parent);
}

void
CQChartsBoxDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsBoxDataLineEdit *edit = qobject_cast<CQChartsBoxDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(boxDataChanged()), obj, method);
}

QVariant
CQChartsBoxDataPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsBoxDataLineEdit *edit = qobject_cast<CQChartsBoxDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->boxData());
}

void
CQChartsBoxDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsBoxDataLineEdit *edit = qobject_cast<CQChartsBoxDataLineEdit *>(w);
  assert(edit);

  CQChartsBoxData data = var.value<CQChartsBoxData>();

  edit->setBoxData(data);
}

//------

CQChartsBoxDataEdit::
CQChartsBoxDataEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("boxDataEdit");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  // visible
  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = new QGridLayout(groupBox_);

  int row = 0;

  //--

  // margin
  marginEdit_ = CQUtil::makeWidget<CQRealSpin>("marginEdit");

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Margin", marginEdit_, row);

  connect(marginEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  //--

  // padding
  paddingEdit_ = CQUtil::makeWidget<CQRealSpin>("paddingEdit");

  connect(paddingEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Padding", paddingEdit_, row);

  //--

  // shape (stroke, fill)
  shapeEdit_ = new CQChartsShapeDataEdit(nullptr, /*tabbed*/true);

  shapeEdit_->setTitle("Shape");
  shapeEdit_->setPreview(false);

  connect(shapeEdit_, SIGNAL(shapeDataChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(shapeEdit_, row, 0, 1, 2); ++row;

  //---

  // sides
  sidesEdit_ = new CQChartsSidesEdit;

  connect(sidesEdit_, SIGNAL(sidesChanged()), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Sides", sidesEdit_, row);

  //---

  preview_ = new CQChartsBoxDataEditPreview(this);

  groupLayout->addWidget(preview_, row, 1); ++row;

  //---

  groupLayout->setRowStretch(row, 1);

  //---

  dataToWidgets();
}

void
CQChartsBoxDataEdit::
setData(const CQChartsBoxData &d)
{
  data_ = d;

  dataToWidgets();
}

void
CQChartsBoxDataEdit::
setPlot(CQChartsPlot *plot)
{
  CQChartsEditBase::setPlot(plot);

  shapeEdit_->setPlot(plot);
}

void
CQChartsBoxDataEdit::
setView(CQChartsView *view)
{
  CQChartsEditBase::setView(view);

  shapeEdit_->setView(view);
}

void
CQChartsBoxDataEdit::
setTitle(const QString &title)
{
  groupBox_->setTitle(title);
}

void
CQChartsBoxDataEdit::
setPreview(bool b)
{
  shapeEdit_->setPreview(b);

  preview_->setVisible(b);
}

void
CQChartsBoxDataEdit::
dataToWidgets()
{
  disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  disconnect(marginEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(paddingEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(shapeEdit_, SIGNAL(shapeDataChanged()), this, SLOT(widgetsToData()));
  disconnect(sidesEdit_, SIGNAL(sidesChanged()), this, SLOT(widgetsToData()));

  groupBox_   ->setChecked(data_.isVisible());
  marginEdit_ ->setValue  (data_.margin());
  paddingEdit_->setValue  (data_.padding());
  shapeEdit_  ->setData   (data_.shape());
  sidesEdit_  ->setSides  (data_.borderSides());

  preview_->update();

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  connect(marginEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(paddingEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(shapeEdit_, SIGNAL(shapeDataChanged()), this, SLOT(widgetsToData()));
  connect(sidesEdit_, SIGNAL(sidesChanged()), this, SLOT(widgetsToData()));
}

void
CQChartsBoxDataEdit::
widgetsToData()
{
  data_.setVisible    (groupBox_   ->isChecked());
  data_.setMargin     (marginEdit_ ->value());
  data_.setPadding    (paddingEdit_->value());
  data_.setShape      (shapeEdit_  ->data());
  data_.setBorderSides(sidesEdit_  ->sides());

  preview_->update();

  emit boxDataChanged();
}

//------

CQChartsBoxDataEditPreview::
CQChartsBoxDataEditPreview(CQChartsBoxDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
}

void
CQChartsBoxDataEditPreview::
draw(QPainter *painter)
{
  const CQChartsBoxData &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsBoxDataEditPreview::
draw(QPainter *painter, const CQChartsBoxData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  // set pen
  QColor pc = interpColor(plot, view, data.shape().border().color());

  QPen pen;

  double width = CQChartsUtil::limitLineWidth(data.shape().border().width().value());

  CQChartsUtil::setPen(pen, data.shape().border().isVisible(), pc,
                       data.shape().border().alpha(), width, data.shape().border().dash());

  painter->setPen(pen);

  //---

  // set brush
  QColor fc = interpColor(plot, view, data.shape().background().color());

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.shape().background().isVisible(), fc,
                         data.shape().background().alpha(),
                         data.shape().background().pattern());

  painter->setBrush(brush);

  //---

  // draw box
  double cxs = data.shape().border().cornerSize().value();
  double cys = data.shape().border().cornerSize().value();

  CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
}
