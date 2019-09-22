#include <CQChartsBoxDataEdit.h>

#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsPaintDevice.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQRealSpin.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QLabel>
#include <QVBoxLayout>

CQChartsBoxDataLineEdit::
CQChartsBoxDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("boxDataLineEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsBoxDataEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

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

  connectSlots(true);

  if (updateText)
    boxDataToWidgets();

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

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  // visible
  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  int row = 0;

  //--

  // margin
  marginEdit_ = CQUtil::makeWidget<CQRealSpin>("marginEdit");

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Margin", marginEdit_, row);

  //--

  // padding
  paddingEdit_ = CQUtil::makeWidget<CQRealSpin>("paddingEdit");

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Padding", paddingEdit_, row);

  //--

  // shape (stroke, fill)
  shapeEdit_ = new CQChartsShapeDataEdit(nullptr, /*tabbed*/true);

  shapeEdit_->setTitle("Shape");
  shapeEdit_->setPreview(false);

  groupLayout->addWidget(shapeEdit_, row, 0, 1, 2); ++row;

  //---

  // sides
  sidesEdit_ = new CQChartsSidesEdit;

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Sides", sidesEdit_, row);

  //---

  preview_ = new CQChartsBoxDataEditPreview(this);

  groupLayout->addWidget(preview_, row, 1); ++row;

  //---

  groupLayout->setRowStretch(row, 1);

  //---

  connectSlots(true);

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

  connectDisconnect(b, groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, marginEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToData()));
  connectDisconnect(b, paddingEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToData()));
  connectDisconnect(b, shapeEdit_, SIGNAL(shapeDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, sidesEdit_, SIGNAL(sidesChanged()), SLOT(widgetsToData()));
}

void
CQChartsBoxDataEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_   ->setChecked(data_.isVisible());
  marginEdit_ ->setValue  (data_.margin());
  paddingEdit_->setValue  (data_.padding());
  shapeEdit_  ->setData   (data_.shape());
  sidesEdit_  ->setSides  (data_.borderSides());

  preview_->update();

  connectSlots(true);
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
  QColor pc = interpColor(plot, view, data.shape().stroke().color());

  QPen pen;

  double width = CQChartsUtil::limitLineWidth(data.shape().stroke().width().value());

  CQChartsUtil::setPen(pen, data.shape().stroke().isVisible(), pc,
                       data.shape().stroke().alpha(), width, data.shape().stroke().dash());

  painter->setPen(pen);

  //---

  // set brush
  QColor fc = interpColor(plot, view, data.shape().fill().color());

  QBrush brush;

  CQChartsUtil::setBrush(brush, data.shape().fill().isVisible(), fc,
                         data.shape().fill().alpha(), data.shape().fill().pattern());

  painter->setBrush(brush);

  //---

  // draw box
  CQChartsPixelPainter device(painter);

  CQChartsDrawUtil::drawRoundedPolygon(&device, rect, data.shape().stroke().cornerSize(),
                                       data.shape().stroke().cornerSize(), data.borderSides());
}
