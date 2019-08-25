#include <CQChartsEditAnnotationDlg.h>
#include <CQChartsAnnotation.h>
#include <CQChartsArrow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsRectEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsSymbolDataEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsPolygonEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsArrowDataEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQRealSpin.h>
#include <CQCheckBox.h>
#include <CQGroupBox.h>
#include <CQLineEdit.h>
#include <CQUtil.h>

#include <QFrame>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CQChartsEditAnnotationDlg::
CQChartsEditAnnotationDlg(QWidget *parent, CQChartsAnnotation *annotation) :
 QDialog(parent), annotation_(annotation)
{
  setWindowTitle(QString("Edit %1 Annotation (%2)").
    arg(annotation->typeName()).arg(annotation_->id()));

  initWidgets();
}

void
CQChartsEditAnnotationDlg::
initWidgets()
{
  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //----

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  layout->addLayout(gridLayout);

  int row = 0;

  //--

  // id, tip edits
  idEdit_  = CQUtil::makeWidget<CQLineEdit>("id");
  tipEdit_ = CQUtil::makeWidget<CQLineEdit>("tip");

  //--

  idEdit_ ->setText(annotation_->id   ());
  tipEdit_->setText(annotation_->tipId());

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Id" , idEdit_ , row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Tip", tipEdit_, row);

  //----

  // create widgets for annotation type
  frameLayout_ = CQUtil::makeLayout<QVBoxLayout>(2, 2);

  if      (annotation_->type() == CQChartsAnnotation::Type::RECT)
    createRectFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::ELLIPSE)
    createEllipseFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::POLYGON)
    createPolygonFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::POLYLINE)
    createPolyLineFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::TEXT)
    createTextFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::ARROW)
    createArrowFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::POINT)
    createPointFrame();

  layout->addLayout(frameLayout_);

  //---

  // OK, Apply, Cancel Buttons
  CQChartsDialogButtons *buttons = new CQChartsDialogButtons;

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  layout->addWidget(buttons);
}

void
CQChartsEditAnnotationDlg::
createRectFrame()
{
  CQChartsRectangleAnnotation *annotation =
    dynamic_cast<CQChartsRectangleAnnotation *>(annotation_);
  assert(annotation);

  //---

  rectWidgets_.frame = CQUtil::makeWidget<QFrame>("rectFrame");

  frameLayout_->addWidget(rectWidgets_.frame);

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(rectWidgets_.frame, 2, 2);

  //---

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // rect
  rectWidgets_.rectEdit = CQUtil::makeWidget<CQChartsRectEdit>("rectEdit");

  rectWidgets_.rectEdit->setRect(CQChartsRect());

  rectWidgets_.rectEdit->setRect(annotation->rectangle());

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Rect", rectWidgets_.rectEdit, row);

  //--

  // margin
  rectWidgets_.marginEdit = CQUtil::makeWidget<CQRealSpin>("marginEdit");

  rectWidgets_.marginEdit ->setValue(annotation->margin ());

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Margin", rectWidgets_.marginEdit, row);

  //--

  // padding
  rectWidgets_.paddingEdit = CQUtil::makeWidget<CQRealSpin>("paddingEdit");

  rectWidgets_.paddingEdit->setValue(annotation->padding());

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Padding", rectWidgets_.paddingEdit, row);

  //----

  addFillWidgets  (rectWidgets_, frameLayout);
  addStrokeWidgets(rectWidgets_, frameLayout, /*cornerSize*/true);

  addSidesWidget(rectWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createEllipseFrame()
{
  CQChartsEllipseAnnotation *annotation = dynamic_cast<CQChartsEllipseAnnotation *>(annotation_);
  assert(annotation);

  //---

  ellipseWidgets_.frame = CQUtil::makeWidget<QFrame>("ellipseFrame");

  frameLayout_->addWidget(ellipseWidgets_.frame);

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(ellipseWidgets_.frame, 2, 2);

  //---

  ellipseWidgets_.centerEdit = new CQChartsPositionEdit;
  ellipseWidgets_.rxEdit     = new CQChartsLengthEdit;
  ellipseWidgets_.ryEdit     = new CQChartsLengthEdit;

  //---

  ellipseWidgets_.centerEdit->setPosition(annotation->center());
  ellipseWidgets_.rxEdit    ->setLength  (annotation->xRadius());
  ellipseWidgets_.ryEdit    ->setLength  (annotation->yRadius());

  //---

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Center"  , ellipseWidgets_.centerEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Radius X", ellipseWidgets_.rxEdit    , row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Radius Y", ellipseWidgets_.ryEdit    , row);

  //---

  addFillWidgets  (ellipseWidgets_, frameLayout);
  addStrokeWidgets(ellipseWidgets_, frameLayout, /*cornerSize*/true);

  addSidesWidget(rectWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createPolygonFrame()
{
  CQChartsPolygonAnnotation *annotation = dynamic_cast<CQChartsPolygonAnnotation *>(annotation_);
  assert(annotation);

  //---

  polygonWidgets_.frame = CQUtil::makeWidget<QFrame>("polygonFrame");

  frameLayout_->addWidget(polygonWidgets_.frame);

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polygonWidgets_.frame, 2, 2);

  //---

  polygonWidgets_.pointsEdit = new CQChartsPolygonEdit;

  addLabelWidget(frameLayout, "Points", polygonWidgets_.pointsEdit);

  //---

  polygonWidgets_.pointsEdit->setPolygon(annotation->polygon());

  //---

  addFillWidgets  (polygonWidgets_, frameLayout);
  addStrokeWidgets(polygonWidgets_, frameLayout, /*cornerSize*/false);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createPolyLineFrame()
{
  CQChartsPolylineAnnotation *annotation = dynamic_cast<CQChartsPolylineAnnotation *>(annotation_);
  assert(annotation);

  //---

  polylineWidgets_.frame = CQUtil::makeWidget<QFrame>("polylineFrame");

  frameLayout_->addWidget(polylineWidgets_.frame);

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polylineWidgets_.frame, 2, 2);

  //---

  polylineWidgets_.pointsEdit = new CQChartsPolygonEdit;

  addLabelWidget(frameLayout, "Points", polylineWidgets_.pointsEdit);

  //---

  polylineWidgets_.pointsEdit->setPolygon(annotation->polygon());

  //---

  addFillWidgets  (polylineWidgets_, frameLayout);
  addStrokeWidgets(polylineWidgets_, frameLayout, /*cornerSize*/false);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createTextFrame()
{
  CQChartsTextAnnotation *annotation = dynamic_cast<CQChartsTextAnnotation *>(annotation_);
  assert(annotation);

  //---

  textWidgets_.frame = CQUtil::makeWidget<QFrame>("textFrame");

  frameLayout_->addWidget(textWidgets_.frame);

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(textWidgets_.frame, 2, 2);

  //---

  QFrame *positionRectFrame = CQUtil::makeWidget<QFrame>("positionRectFrame");

  QHBoxLayout *positionRectLayout = CQUtil::makeLayout<QHBoxLayout>(positionRectFrame, 2, 2);

  textWidgets_.positionRadio = CQUtil::makeLabelWidget<QRadioButton>("Position", "positionRadio");
  textWidgets_.rectRadio     = CQUtil::makeLabelWidget<QRadioButton>("Rect", "rectRadio");

  positionRectLayout->addWidget(textWidgets_.positionRadio);
  positionRectLayout->addWidget(textWidgets_.rectRadio);
  positionRectLayout->addStretch(1);

  connect(textWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(textPositionSlot(bool)));

  textWidgets_.positionEdit = new CQChartsPositionEdit;
  textWidgets_.rectEdit     = new CQChartsRectEdit;

  if (annotation->position().isSet()) {
    textWidgets_.positionRadio->setChecked(true);

    textWidgets_.positionEdit->setPosition(annotation->positionValue());
  }

  if (annotation->rectangle().isSet()) {
    textWidgets_.rectRadio->setChecked(true);

    textWidgets_.rectEdit->setRect(annotation->rectangleValue());
  }

  frameLayout->addWidget(positionRectFrame);

  textPositionSlot(true);

  //---

  QGridLayout *gridLayout1 = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout1);

  int row1 = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Position", textWidgets_.positionEdit, row1);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Rect"    , textWidgets_.rectEdit    , row1);

  //---

  textWidgets_.textEdit = CQUtil::makeWidget<CQLineEdit>("edit");

  textWidgets_.textEdit->setText(annotation->textStr());

  //---

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Text", textWidgets_.textEdit, row);

  //---

  textWidgets_.dataEdit = new CQChartsTextDataEdit(nullptr, /*optional*/false);

  textWidgets_.dataEdit->setPlot(annotation->plot());
  textWidgets_.dataEdit->setView(annotation->view());
  textWidgets_.dataEdit->setData(annotation->textData());

  //--

  frameLayout->addWidget(textWidgets_.dataEdit);

  //---

  addFillWidgets  (textWidgets_, frameLayout);
  addStrokeWidgets(textWidgets_, frameLayout, /*cornerSize*/true);

  addSidesWidget(rectWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createArrowFrame()
{
  CQChartsArrowAnnotation *annotation = dynamic_cast<CQChartsArrowAnnotation *>(annotation_);
  assert(annotation);

  CQChartsArrow *arrow = annotation->arrow();

  const CQChartsStrokeData &stroke = arrow->shapeData().stroke();
  const CQChartsFillData   &fill   = arrow->shapeData().fill();

  //---

  arrowWidgets_.frame = CQUtil::makeWidget<QFrame>("arrowFrame");

  frameLayout_->addWidget(arrowWidgets_.frame);

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(arrowWidgets_.frame, 2, 2);

  //----

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // start, end point
  arrowWidgets_.startEdit = new CQChartsPositionEdit;
  arrowWidgets_.endEdit   = new CQChartsPositionEdit;

  arrowWidgets_.startEdit->setPosition(annotation->start());
  arrowWidgets_.endEdit  ->setPosition(annotation->end());

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Start", arrowWidgets_.startEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "End"  , arrowWidgets_.endEdit  , row);

  //---

  // arrow data edit
  arrowWidgets_.dataEdit = new CQChartsArrowDataEdit;

  arrowWidgets_.dataEdit->setPlot(annotation->plot());
  arrowWidgets_.dataEdit->setView(annotation->view());
  arrowWidgets_.dataEdit->setData(annotation->arrowData());

  frameLayout->addWidget(arrowWidgets_.dataEdit);

  //---

  QGridLayout *gridLayout1 = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout1);

  int row1 = 0;

  //--

  // stroke width, stroke color, filled and fill color
  arrowWidgets_.strokeWidthEdit = new CQChartsLengthEdit;
  arrowWidgets_.strokeColorEdit = new CQChartsColorLineEdit;
  arrowWidgets_.filledCheck     = CQUtil::makeWidget<CQCheckBox>("filledCheck");
  arrowWidgets_.fillColorEdit   = new CQChartsColorLineEdit;

  arrowWidgets_.strokeWidthEdit->setLength  (stroke.width());
  arrowWidgets_.strokeColorEdit->setColor   (stroke.color());
  arrowWidgets_.filledCheck    ->setChecked (fill.isVisible());
  arrowWidgets_.fillColorEdit  ->setColor   (fill.color());

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Stroke Width",
    arrowWidgets_.strokeWidthEdit, row1);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Stroke Color",
    arrowWidgets_.strokeColorEdit, row1);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Filled"      ,
    arrowWidgets_.filledCheck    , row1);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Fill Color"  ,
    arrowWidgets_.fillColorEdit  , row1);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createPointFrame()
{
  CQChartsPointAnnotation *annotation = dynamic_cast<CQChartsPointAnnotation *>(annotation_);
  assert(annotation);

  //---

  pointWidgets_.frame = CQUtil::makeWidget<QFrame>("pointFrame");

  frameLayout_->addWidget(pointWidgets_.frame);

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(pointWidgets_.frame, 2, 2);

  //---

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // position
  pointWidgets_.positionEdit = new CQChartsPositionEdit;

  pointWidgets_.positionEdit->setPosition(annotation->position());

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Position", pointWidgets_.positionEdit, row);

  //---

  pointWidgets_.dataEdit = new CQChartsSymbolDataEdit(nullptr, /*optional*/false);

  pointWidgets_.dataEdit->setPlot(annotation->plot());
  pointWidgets_.dataEdit->setView(annotation->view());
  pointWidgets_.dataEdit->setData(annotation->symbolData());

  frameLayout->addWidget(pointWidgets_.dataEdit);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
addFillWidgets(Widgets &widgets, QBoxLayout *playout)
{
  CQChartsBoxData boxData = annotation_->boxData();

  CQChartsFillData &fillData = boxData.shape().fill();

  //---

  widgets.backgroundDataEdit = new CQChartsFillDataEdit;

  widgets.backgroundDataEdit->setTitle("Fill");
  widgets.backgroundDataEdit->setData(fillData);

  playout->addWidget(widgets.backgroundDataEdit);
}

void
CQChartsEditAnnotationDlg::
addStrokeWidgets(Widgets &widgets, QBoxLayout *playout, bool cornerSize)
{
  CQChartsBoxData boxData = annotation_->boxData();

  CQChartsStrokeData &strokeData = boxData.shape().stroke();

  //---

  widgets.strokeDataEdit =
    new CQChartsStrokeDataEdit(nullptr,
          CQChartsStrokeDataEditConfig().setCornerSize(cornerSize));

  widgets.strokeDataEdit->setTitle("Stroke");
  widgets.strokeDataEdit->setData(strokeData);

  playout->addWidget(widgets.strokeDataEdit);
}

void
CQChartsEditAnnotationDlg::
addSidesWidget(Widgets &widgets, QBoxLayout *playout)
{
  CQChartsBoxData boxData = annotation_->boxData();

  //---

  widgets.borderSidesEdit = CQUtil::makeWidget<CQChartsSidesEdit>("borderSidesEdit");

  widgets.borderSidesEdit->setSides(boxData.borderSides());

  //---

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  playout->addLayout(gridLayout);

  int row = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Border Sides", widgets.borderSidesEdit, row);
}

QHBoxLayout *
CQChartsEditAnnotationDlg::
addLabelWidget(QBoxLayout *playout, const QString &label, QWidget *widget)
{
  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  QLabel *qlabel = CQUtil::makeLabelWidget<QLabel>(label, "label" + label);

  layout->addWidget (qlabel);
  layout->addWidget (widget);
  layout->addStretch(1);

  playout->addLayout(layout);

  return layout;
}

void
CQChartsEditAnnotationDlg::
textPositionSlot(bool)
{
  if (textWidgets_.positionRadio->isChecked()) {
    textWidgets_.positionEdit->setEnabled(true);
    textWidgets_.rectEdit    ->setEnabled(false);
  }
  else {
    textWidgets_.positionEdit->setEnabled(false);
    textWidgets_.rectEdit    ->setEnabled(true);
  }
}

void
CQChartsEditAnnotationDlg::
okSlot()
{
  if (applySlot())
    cancelSlot();
}

bool
CQChartsEditAnnotationDlg::
applySlot()
{
  bool rc = false;

  if      (annotation_->type() == CQChartsAnnotation::Type::RECT)
    rc = updateRectangleAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::ELLIPSE)
    rc = updateEllipseAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::POLYGON)
    rc = updatePolygonAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::POLYLINE)
    rc = updatePolylineAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::TEXT)
    rc = updateTextAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::ARROW)
    rc = updateArrowAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::POINT)
    rc = updatePointAnnotation();

  return rc;
}

bool
CQChartsEditAnnotationDlg::
updateRectangleAnnotation()
{
  CQChartsBoxData boxData;

  CQChartsShapeData &shapeData = boxData.shape();

  //---

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsRect rect = rectWidgets_.rectEdit->rect();

  boxData.setMargin (rectWidgets_.marginEdit ->value());
  boxData.setPadding(rectWidgets_.paddingEdit->value());

  CQChartsFillData   fill   = rectWidgets_.backgroundDataEdit->data();
  CQChartsStrokeData stroke = rectWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  boxData.setBorderSides(rectWidgets_.borderSidesEdit->sides());

  //---

  CQChartsRectangleAnnotation *annotation =
    dynamic_cast<CQChartsRectangleAnnotation *>(annotation_);
  assert(annotation);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setRectangle(rect);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updateEllipseAnnotation()
{
  CQChartsBoxData boxData;

  CQChartsShapeData &shapeData = boxData.shape();

  //---

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition center = ellipseWidgets_.centerEdit->position();
  CQChartsLength   rx     = ellipseWidgets_.rxEdit->length();
  CQChartsLength   ry     = ellipseWidgets_.ryEdit->length();

  CQChartsFillData   fill   = ellipseWidgets_.backgroundDataEdit->data();
  CQChartsStrokeData stroke = ellipseWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  boxData.setBorderSides(ellipseWidgets_.borderSidesEdit->sides());

  //---

  CQChartsEllipseAnnotation *annotation = dynamic_cast<CQChartsEllipseAnnotation *>(annotation_);
  assert(annotation);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setCenter (center);
  annotation->setXRadius(rx);
  annotation->setYRadius(ry);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updatePolygonAnnotation()
{
  CQChartsBoxData boxData;

  CQChartsShapeData &shapeData = boxData.shape();

  //---

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPolygon polygon = polygonWidgets_.pointsEdit->polygon();

  CQChartsFillData   fill   = polygonWidgets_.backgroundDataEdit->data();
  CQChartsStrokeData stroke = polygonWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  //---

  CQChartsPolygonAnnotation *annotation = dynamic_cast<CQChartsPolygonAnnotation *>(annotation_);
  assert(annotation);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setPolygon(polygon);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updatePolylineAnnotation()
{
  CQChartsBoxData boxData;

  CQChartsShapeData &shapeData = boxData.shape();

  //---

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPolygon polygon = polylineWidgets_.pointsEdit->polygon();

  CQChartsFillData   fill   = polylineWidgets_.backgroundDataEdit->data();
  CQChartsStrokeData stroke = polylineWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  //---

  CQChartsPolylineAnnotation *annotation = dynamic_cast<CQChartsPolylineAnnotation *>(annotation_);
  assert(annotation);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setPolygon(polygon);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updateTextAnnotation()
{
  CQChartsBoxData  boxData;

  CQChartsShapeData &shapeData = boxData.shape();

  //---

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition pos  = textWidgets_.positionEdit->position();
  CQChartsRect     rect = textWidgets_.rectEdit    ->rect();

  QString text = textWidgets_.textEdit->text();

  const CQChartsTextData &textData = textWidgets_.dataEdit->data();

  CQChartsFillData   fill   = textWidgets_.backgroundDataEdit->data();
  CQChartsStrokeData stroke = textWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  boxData.setBorderSides(textWidgets_.borderSidesEdit->sides());

  //---

  CQChartsTextAnnotation *annotation = dynamic_cast<CQChartsTextAnnotation *>(annotation_);
  assert(annotation);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setTextStr(text);

  if (textWidgets_.positionRadio->isChecked())
    annotation->setPosition(pos);
  else
    annotation->setRectangle(rect);

  annotation->setTextData(textData);
  annotation->setBoxData (boxData );

  return true;
}

bool
CQChartsEditAnnotationDlg::
updateArrowAnnotation()
{
  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition start = arrowWidgets_.startEdit->position();
  CQChartsPosition end   = arrowWidgets_.endEdit  ->position();

  const CQChartsArrowData &arrowData = arrowWidgets_.dataEdit->data();

  CQChartsShapeData shapeData;

  CQChartsStrokeData &stroke = shapeData.stroke();
  CQChartsFillData   &fill   = shapeData.fill();

  stroke.setWidth(arrowWidgets_.strokeWidthEdit->length());
  stroke.setColor(arrowWidgets_.strokeColorEdit->color());

  fill.setVisible(arrowWidgets_.filledCheck->isChecked());
  fill.setColor  (arrowWidgets_.fillColorEdit->color());

  //---

  CQChartsArrowAnnotation *annotation = dynamic_cast<CQChartsArrowAnnotation *>(annotation_);
  assert(annotation);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setStart(start);
  annotation->setEnd  (end);

  annotation->setArrowData(arrowData);

  annotation->arrow()->setShapeData(shapeData);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updatePointAnnotation()
{
  CQChartsPointAnnotation *annotation = dynamic_cast<CQChartsPointAnnotation *>(annotation_);
  assert(annotation);

  //---

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition          pos        = pointWidgets_.positionEdit->position();
  const CQChartsSymbolData &symbolData = pointWidgets_.dataEdit->data();

  //---

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setPosition(pos);

  annotation->setSymbolData(symbolData);

  return true;
}

void
CQChartsEditAnnotationDlg::
cancelSlot()
{
  close();
}
