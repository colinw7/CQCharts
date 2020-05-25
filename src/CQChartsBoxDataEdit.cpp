#include <CQChartsBoxDataEdit.h>

#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsMarginEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsViewPlotPaintDevice.h>

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

  setToolTip("Box Data");

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
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(boxDataChanged()), this, SLOT(menuEditChanged()));
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
  auto *edit = qobject_cast<CQChartsBoxDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(boxDataChanged()), obj, method);
}

QVariant
CQChartsBoxDataPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsBoxDataLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->boxData());
}

void
CQChartsBoxDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsBoxDataLineEdit *>(w);
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

  setToolTip("Box Data");

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  // visible
  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(false);
  groupBox_->setTitle("Visible");

  layout->addWidget(groupBox_);

  //---

  auto *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  int row = 0;

  //--

  // outer margin
  marginEdit_ = CQUtil::makeWidget<CQChartsMarginEdit>("marginEdit");

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Margin", marginEdit_, row);

  //--

  // inner padding
  paddingEdit_ = CQUtil::makeWidget<CQChartsMarginEdit>("paddingEdit");

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

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(marginEdit_, SIGNAL(marginChanged()), SLOT(widgetsToData()));
  connectDisconnect(paddingEdit_, SIGNAL(marginChanged()), SLOT(widgetsToData()));
  connectDisconnect(shapeEdit_, SIGNAL(shapeDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(sidesEdit_, SIGNAL(sidesChanged()), SLOT(widgetsToData()));
}

void
CQChartsBoxDataEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_   ->setChecked(data_.isVisible());
  marginEdit_ ->setMargin (data_.margin());
  paddingEdit_->setMargin (data_.padding());
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
  data_.setMargin     (marginEdit_ ->margin());
  data_.setPadding    (paddingEdit_->margin());
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
  setToolTip("Box Preview");
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
  CQChartsPenBrush penBrush;

  // set pen and brush
  QColor pc = interpColor(plot, view, data.shape().stroke().color());
  QColor fc = interpColor(plot, view, data.shape().fill().color());

  double width = CQChartsUtil::limitLineWidth(data.shape().stroke().width().value());

  CQChartsUtil::setPenBrush(penBrush,
    data.shape().stroke().isVisible(), pc, data.shape().stroke().alpha(),
    width, data.shape().stroke().dash(),
    data.shape().fill().isVisible(), fc, data.shape().fill().alpha(),
    data.shape().fill().pattern());

  painter->setPen  (penBrush.pen);
  painter->setBrush(penBrush.brush);

  //---

  // draw box
  CQChartsPixelPaintDevice device(painter);

  CQChartsDrawUtil::drawRoundedPolygon(&device, CQChartsGeom::BBox(rect),
                                       data.shape().stroke().cornerSize(),
                                       data.borderSides());
}
