#include <CQChartsEditAnnotationDlg.h>
#include <CQChartsAnnotation.h>
#include <CQChartsArrow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsRectEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsMarginEdit.h>
#include <CQChartsSymbolDataEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsPolygonEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsArrowDataEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAngleEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQRealSpin.h>
#include <CQCheckBox.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QFrame>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>

CQChartsEditAnnotationDlg::
CQChartsEditAnnotationDlg(QWidget *parent, CQChartsAnnotation *annotation) :
 QDialog(parent), annotation_(annotation)
{
  initWidgets();
}

void
CQChartsEditAnnotationDlg::
initWidgets()
{
  setObjectName("createAnnotationDlg");

//setWindowTitle("Edit Annotation");
  setWindowTitle(QString("Edit %1 Annotation (%2)").
    arg(annotation_->typeName()).arg(annotation_->id()));

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //----

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  layout->addLayout(gridLayout);

  int row = 0;

  //--

  // id, tip edits
  idEdit_  = createLineEdit("id" , annotation_->id   (), "Annotation Id");
  tipEdit_ = createLineEdit("tip", annotation_->tipId(), "Annotation Tooltip");

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
  else if (annotation_->type() == CQChartsAnnotation::Type::IMAGE)
    createImageFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::ARROW)
    createArrowFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::POINT)
    createPointFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::PIE_SLICE)
    createPieSliceFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::AXIS)
    createAxisFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::KEY)
    createKeyFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::POINT_SET)
    createPointSetFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::VALUE_SET)
    createValueSetFrame();
  else if (annotation_->type() == CQChartsAnnotation::Type::BUTTON)
    createButtonFrame();

  layout->addLayout(frameLayout_);

  //---

  msgLabel_ = CQUtil::makeWidget<QLabel>("msgLabel");

  CQChartsWidgetUtil::setTextColor(msgLabel_, Qt::red);

  layout->addWidget(msgLabel_);

  //---

  // OK, Apply, Cancel Buttons
  auto *buttons = CQUtil::makeWidget<CQChartsDialogButtons>("buttons");

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  layout->addWidget(buttons);
}

//------

void
CQChartsEditAnnotationDlg::
createRectFrame()
{
  auto *annotation = dynamic_cast<CQChartsRectangleAnnotation *>(annotation_);
  assert(annotation);

  //---

  rectWidgets_.frame = CQUtil::makeWidget<QFrame>("rectFrame");

  frameLayout_->addWidget(rectWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(rectWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // rect
  rectWidgets_.rectEdit = createRectEdit("rectEdit", annotation->rectangle(), "Rectangle");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Rect", rectWidgets_.rectEdit, row);

  //--

  // outer margin
  rectWidgets_.marginEdit =
    createMarginEdit("marginEdit", annotation->margin(), "Rectangle Margin");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Margin", rectWidgets_.marginEdit, row);

  //--

  // inner padding
  rectWidgets_.paddingEdit =
    createMarginEdit("paddingEdit", annotation->padding(), "Rectangle Padding");

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
  auto *annotation = dynamic_cast<CQChartsEllipseAnnotation *>(annotation_);
  assert(annotation);

  //---

  ellipseWidgets_.frame = CQUtil::makeWidget<QFrame>("ellipseFrame");

  frameLayout_->addWidget(ellipseWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(ellipseWidgets_.frame, 2, 2);

  //---

  auto *centerRectFrame  = CQUtil::makeWidget<QFrame>("centerRectFrame");
  auto *centerRectLayout = CQUtil::makeLayout<QHBoxLayout>(centerRectFrame, 2, 2);

  ellipseWidgets_.centerRadio = CQUtil::makeLabelWidget<QRadioButton>("Center", "centerRadio");
  ellipseWidgets_.centerRadio->setToolTip("Create ellipse from center and radii");

  ellipseWidgets_.rectRadio = CQUtil::makeLabelWidget<QRadioButton>("Rect", "rectRadio");
  ellipseWidgets_.rectRadio->setToolTip("Create ellipse in rectangle");

  centerRectLayout->addWidget(ellipseWidgets_.centerRadio);
  centerRectLayout->addWidget(ellipseWidgets_.rectRadio);
  centerRectLayout->addStretch(1);

  connect(ellipseWidgets_.centerRadio, SIGNAL(toggled(bool)), this, SLOT(ellipseCenterSlot(bool)));

  ellipseWidgets_.centerEdit =
    createPositionEdit("centerEdit", annotation->center(), "Ellipse Center Position");

  ellipseWidgets_.rxEdit =
    createLengthEdit("rxEdit", annotation->xRadius(), "Ellipse X Radius Length");
  ellipseWidgets_.ryEdit =
    createLengthEdit("ryEdit", annotation->yRadius(), "Ellipse Y Radius Length");

  ellipseWidgets_.rectEdit = createRectEdit("rectEdit", CQChartsRect(), "Ellipse Rectangle");

  ellipseWidgets_.centerRadio->setChecked(true);

  frameLayout->addWidget(centerRectFrame);

  ellipseCenterSlot(true);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Center"  , ellipseWidgets_.centerEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Radius X", ellipseWidgets_.rxEdit    , row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Radius Y", ellipseWidgets_.ryEdit    , row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Rect"    , ellipseWidgets_.rectEdit  , row);

  //---

  addFillWidgets  (ellipseWidgets_, frameLayout);
  addStrokeWidgets(ellipseWidgets_, frameLayout, /*cornerSize*/true);

  addSidesWidget(ellipseWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createPolygonFrame()
{
  auto *annotation = dynamic_cast<CQChartsPolygonAnnotation *>(annotation_);
  assert(annotation);

  //---

  polygonWidgets_.frame = CQUtil::makeWidget<QFrame>("polygonFrame");

  frameLayout_->addWidget(polygonWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polygonWidgets_.frame, 2, 2);

  //---

  polygonWidgets_.pointsEdit =
    createPolygonEdit("pointsEdit", annotation->polygon(), "Polygon Points");

  addLabelWidget(frameLayout, "Points", polygonWidgets_.pointsEdit);

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
  auto *annotation = dynamic_cast<CQChartsPolylineAnnotation *>(annotation_);
  assert(annotation);

  //---

  polylineWidgets_.frame = CQUtil::makeWidget<QFrame>("polylineFrame");

  frameLayout_->addWidget(polylineWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polylineWidgets_.frame, 2, 2);

  //---

  polylineWidgets_.pointsEdit =
    createPolygonEdit("pointsEdit", annotation->polygon(), "Polyline Points");

  addLabelWidget(frameLayout, "Points", polylineWidgets_.pointsEdit);

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
  auto *annotation = dynamic_cast<CQChartsTextAnnotation *>(annotation_);
  assert(annotation);

  //---

  textWidgets_.frame = CQUtil::makeWidget<QFrame>("textFrame");

  frameLayout_->addWidget(textWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(textWidgets_.frame, 2, 2);

  //---

  auto *positionRectFrame  = CQUtil::makeWidget<QFrame>("positionRectFrame");
  auto *positionRectLayout = CQUtil::makeLayout<QHBoxLayout>(positionRectFrame, 2, 2);

  textWidgets_.positionRadio = CQUtil::makeLabelWidget<QRadioButton>("Position", "positionRadio");
  textWidgets_.positionRadio->setToolTip("Position text at point");

  textWidgets_.rectRadio = CQUtil::makeLabelWidget<QRadioButton>("Rect"    , "rectRadio");
  textWidgets_.rectRadio->setToolTip("Place text in rectangle");

  positionRectLayout->addWidget(textWidgets_.positionRadio);
  positionRectLayout->addWidget(textWidgets_.rectRadio);
  positionRectLayout->addStretch(1);

  connect(textWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(textPositionSlot(bool)));

  textWidgets_.positionEdit =
    createPositionEdit("positionEdit", CQChartsPosition(), "Text Position");
  textWidgets_.rectEdit =
    createRectEdit    ("rectEdit", CQChartsRect(), "Text Rectangle");

  if (annotation->position().isSet()) {
    textWidgets_.positionRadio->setChecked(true);
    textWidgets_.positionEdit ->setPosition(annotation->positionValue());
  }

  if (annotation->rectangle().isSet()) {
    textWidgets_.rectRadio->setChecked(true);
    textWidgets_.rectEdit ->setRect(annotation->rectangleValue());
  }

  frameLayout->addWidget(positionRectFrame);

  textPositionSlot(true);

  //---

  auto *gridLayout1 = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout1);

  int row1 = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Position", textWidgets_.positionEdit, row1);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Rect"    , textWidgets_.rectEdit    , row1);

  //---

  textWidgets_.textEdit = createLineEdit("edit", annotation->textStr(), "Text String");

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

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

  addSidesWidget(textWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createImageFrame()
{
  auto *annotation = dynamic_cast<CQChartsImageAnnotation *>(annotation_);
  assert(annotation);

  //---

  imageWidgets_.frame = CQUtil::makeWidget<QFrame>("imageFrame");

  frameLayout_->addWidget(imageWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(imageWidgets_.frame, 2, 2);

  //----

  auto *positionRectFrame  = CQUtil::makeWidget<QFrame>("positionRectFrame");
  auto *positionRectLayout = CQUtil::makeLayout<QHBoxLayout>(positionRectFrame, 2, 2);

  imageWidgets_.positionRadio = CQUtil::makeLabelWidget<QRadioButton>("Position", "positionRadio");
  imageWidgets_.positionRadio->setToolTip("Position image at point");

  imageWidgets_.rectRadio = CQUtil::makeLabelWidget<QRadioButton>("Rect"    , "rectRadio");
  imageWidgets_.rectRadio->setToolTip("Place image in rectangle");

  positionRectLayout->addWidget(imageWidgets_.positionRadio);
  positionRectLayout->addWidget(imageWidgets_.rectRadio);
  positionRectLayout->addStretch(1);

  connect(imageWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(imagePositionSlot(bool)));

  imageWidgets_.positionEdit =
    createPositionEdit("positionEdit", CQChartsPosition(), "Image Position");
  imageWidgets_.rectEdit =
    createRectEdit("rectEdit", CQChartsRect(), "Image Rectangle");

  if (annotation->position().isSet()) {
    imageWidgets_.positionRadio->setChecked(true);
    imageWidgets_.positionEdit ->setPosition(annotation->positionValue());
  }

  if (annotation->rectangle().isSet()) {
    imageWidgets_.rectRadio->setChecked(true);
    imageWidgets_.rectEdit ->setRect(annotation->rectangleValue());
  }

  frameLayout->addWidget(positionRectFrame);

  imagePositionSlot(true);

  //---

  auto *gridLayout1 = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout1);

  int row1 = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Position", imageWidgets_.positionEdit, row1);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Rect"    , imageWidgets_.rectEdit    , row1);

  //--

  imageWidgets_.imageEdit = createLineEdit("imageEdit", annotation->image().toString(), "Image");

  //---

  imageWidgets_.disabledImageEdit =
    createLineEdit("disabledImageEdit", annotation->disabledImage().toString(), "Disabled Image");

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Image",
    imageWidgets_.imageEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Disabled",
    imageWidgets_.disabledImageEdit, row);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createArrowFrame()
{
  auto *annotation = dynamic_cast<CQChartsArrowAnnotation *>(annotation_);
  assert(annotation);

  auto *arrow = annotation->arrow();

  const auto &stroke = arrow->shapeData().stroke();
  const auto &fill   = arrow->shapeData().fill();

  //---

  arrowWidgets_.frame = CQUtil::makeWidget<QFrame>("arrowFrame");

  frameLayout_->addWidget(arrowWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(arrowWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // start, end point
  arrowWidgets_.startEdit =
    createPositionEdit("startEdit", annotation->start(), "Arrow Start Point");
  arrowWidgets_.endEdit =
    createPositionEdit("endEdit"  , annotation->end  (), "Arrow End Point"  );

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Start", arrowWidgets_.startEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "End"  , arrowWidgets_.endEdit  , row);

  //---

  // arrow data edit
  arrowWidgets_.dataEdit = CQUtil::makeWidget<CQChartsArrowDataEdit>("dataEdit");

  arrowWidgets_.dataEdit->setPlot(annotation->plot());
  arrowWidgets_.dataEdit->setView(annotation->view());
  arrowWidgets_.dataEdit->setData(annotation->arrowData());

  frameLayout->addWidget(arrowWidgets_.dataEdit);

  //---

  auto *gridLayout1 = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout1);

  int row1 = 0;

  //--

  // stroke width, stroke color, filled and fill color
  arrowWidgets_.strokeWidthEdit =
    createLengthEdit("strokeWidthEdit", stroke.width(), "Arrow Stroke Width");
  arrowWidgets_.strokeColorEdit = CQUtil::makeWidget<CQChartsColorLineEdit>("strokeColorEdit");
  arrowWidgets_.strokeColorEdit->setColor  (stroke.color());

  arrowWidgets_.filledCheck = CQUtil::makeWidget<CQCheckBox>("filledCheck");
  arrowWidgets_.filledCheck->setToolTip("Is Filled");
  arrowWidgets_.filledCheck->setChecked(fill.isVisible());

  arrowWidgets_.fillColorEdit = CQUtil::makeWidget<CQChartsColorLineEdit>("fillColorEdit");
  arrowWidgets_.fillColorEdit->setColor  (fill.color());

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
  auto *annotation = dynamic_cast<CQChartsPointAnnotation *>(annotation_);
  assert(annotation);

  //---

  pointWidgets_.frame = CQUtil::makeWidget<QFrame>("pointFrame");

  frameLayout_->addWidget(pointWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(pointWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //---

  // position
  pointWidgets_.positionEdit =
    createPositionEdit("positionEdit", annotation->position(), "Point Position");

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
createPieSliceFrame()
{
}

void
CQChartsEditAnnotationDlg::
createAxisFrame()
{
}

void
CQChartsEditAnnotationDlg::
createKeyFrame()
{
}

void
CQChartsEditAnnotationDlg::
createPointSetFrame()
{
}

void
CQChartsEditAnnotationDlg::
createValueSetFrame()
{
}

void
CQChartsEditAnnotationDlg::
createButtonFrame()
{
}

//------

void
CQChartsEditAnnotationDlg::
addFillWidgets(Widgets &widgets, QBoxLayout *playout)
{
  auto boxData = annotation_->boxData();

  auto &fillData = boxData.shape().fill();

  //---

  widgets.backgroundDataEdit = CQUtil::makeWidget<CQChartsFillDataEdit>("backgroundDataEdit");

  widgets.backgroundDataEdit->setTitle("Fill");
  widgets.backgroundDataEdit->setData(fillData);
  widgets.backgroundDataEdit->setToolTip("Enable Fill");

  playout->addWidget(widgets.backgroundDataEdit);
}

void
CQChartsEditAnnotationDlg::
addStrokeWidgets(Widgets &widgets, QBoxLayout *playout, bool cornerSize)
{
  auto boxData = annotation_->boxData();

  auto &strokeData = boxData.shape().stroke();

  //---

  widgets.strokeDataEdit = new CQChartsStrokeDataEdit(nullptr,
                             CQChartsStrokeDataEditConfig().setCornerSize(cornerSize));

  widgets.strokeDataEdit->setTitle("Stroke");
  widgets.strokeDataEdit->setData(strokeData);
  widgets.strokeDataEdit->setToolTip("Enable Stroke");

  playout->addWidget(widgets.strokeDataEdit);
}

void
CQChartsEditAnnotationDlg::
addSidesWidget(Widgets &widgets, QBoxLayout *playout)
{
  auto boxData = annotation_->boxData();

  //---

  widgets.borderSidesEdit = CQUtil::makeWidget<CQChartsSidesEdit>("borderSidesEdit");

  widgets.borderSidesEdit->setSides(boxData.borderSides());

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  playout->addLayout(gridLayout);

  int row = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Border Sides", widgets.borderSidesEdit, row);
}

QHBoxLayout *
CQChartsEditAnnotationDlg::
addLabelWidget(QBoxLayout *playout, const QString &label, QWidget *widget)
{
  auto *layout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  auto *qlabel = CQUtil::makeLabelWidget<QLabel>(label, "label" + label);

  layout->addWidget (qlabel);
  layout->addWidget (widget);
  layout->addStretch(1);

  playout->addLayout(layout);

  return layout;
}

//---

CQChartsLineEdit *
CQChartsEditAnnotationDlg::
createLineEdit(const QString &name, const QString &text, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(name);

  edit->setText(text);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsPositionEdit *
CQChartsEditAnnotationDlg::
createPositionEdit(const QString &name, const CQChartsPosition &pos, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsPositionEdit>(name);

  edit->setPlot(annotation_->plot());

  edit->setPosition(pos);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsLengthEdit *
CQChartsEditAnnotationDlg::
createLengthEdit(const QString &name, const CQChartsLength &len, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsLengthEdit>(name);

  edit->setLength(len);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsRectEdit *
CQChartsEditAnnotationDlg::
createRectEdit(const QString &name, const CQChartsRect &rect, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsRectEdit>(name);

  edit->setPlot(annotation_->plot());

  edit->setRect(rect);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsPolygonEdit *
CQChartsEditAnnotationDlg::
createPolygonEdit(const QString &name, const CQChartsPolygon &poly, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsPolygonEdit>(name);

  edit->setPlot(annotation_->plot());

  edit->setPolygon(poly);

  if      (annotation_->view()) edit->setUnits(CQChartsUnits::VIEW);
  else if (annotation_->plot()) edit->setUnits(CQChartsUnits::PLOT);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsMarginEdit *
CQChartsEditAnnotationDlg::
createMarginEdit(const QString &name, const CQChartsMargin &margin, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsMarginEdit>(name);

  edit->setMargin(margin);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

//---

void
CQChartsEditAnnotationDlg::
ellipseCenterSlot(bool)
{
  auto *annotation = dynamic_cast<CQChartsEllipseAnnotation *>(annotation_);
  assert(annotation);

  auto center = annotation->center();
  auto xr     = annotation->xRadius();
  auto yr     = annotation->yRadius();

  if (ellipseWidgets_.centerRadio->isChecked()) {
    ellipseWidgets_.centerEdit->setEnabled(true);
    ellipseWidgets_.rxEdit    ->setEnabled(true);
    ellipseWidgets_.ryEdit    ->setEnabled(true);
    ellipseWidgets_.rectEdit  ->setEnabled(false);

    ellipseWidgets_.centerEdit->setPosition(center);
    ellipseWidgets_.rxEdit    ->setLength(xr);
    ellipseWidgets_.ryEdit    ->setLength(yr);
  }
  else {
    ellipseWidgets_.centerEdit->setEnabled(false);
    ellipseWidgets_.rxEdit    ->setEnabled(false);
    ellipseWidgets_.ryEdit    ->setEnabled(false);
    ellipseWidgets_.rectEdit  ->setEnabled(true);

    CQChartsGeom::Point center1;
    double              xr1 = 0.0;
    double              yr1 = 0.0;
    CQChartsUnits       units1 { CQChartsUnits::PIXEL };

    if      (annotation_->view()) {
      center1 = annotation_->view()->positionToView(center);
      xr1     = annotation_->view()->lengthViewWidth(xr);
      yr1     = annotation_->view()->lengthViewHeight(yr);
      units1  = CQChartsUnits::VIEW;
    }
    else if (annotation_->plot()) {
      center1 = annotation_->plot()->positionToPlot(center);
      xr1     = annotation_->plot()->lengthPlotWidth(xr);
      yr1     = annotation_->plot()->lengthPlotHeight(yr);
      units1  = CQChartsUnits::PLOT;
    }

    auto rp = CQChartsGeom::Point(xr1, yr1);

    CQChartsRect rect(CQChartsGeom::BBox(center1 - rp, center1 + rp), units1);

    ellipseWidgets_.rectEdit->setRect(rect);
  }
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
imagePositionSlot(bool)
{
  if (imageWidgets_.positionRadio->isChecked()) {
    imageWidgets_.positionEdit->setEnabled(true);
    imageWidgets_.rectEdit    ->setEnabled(false);
  }
  else {
    imageWidgets_.positionEdit->setEnabled(false);
    imageWidgets_.rectEdit    ->setEnabled(true);
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
  else if (annotation_->type() == CQChartsAnnotation::Type::IMAGE)
    rc = updateImageAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::ARROW)
    rc = updateArrowAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::POINT)
    rc = updatePointAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::PIE_SLICE)
    rc = updatePieSliceAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::AXIS)
    rc = updateAxisAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::KEY)
    rc = updateKeyAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::POINT_SET)
    rc = updatePointSetAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::VALUE_SET)
    rc = updateValueSetAnnotation();
  else if (annotation_->type() == CQChartsAnnotation::Type::BUTTON)
    rc = updateButtonAnnotation();

  return rc;
}

//------

bool
CQChartsEditAnnotationDlg::
updateRectangleAnnotation()
{
  CQChartsBoxData boxData;

  auto &shapeData = boxData.shape();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto rect = rectWidgets_.rectEdit->rect();

  if (! rect.isValid())
    return setErrorMsg("Invalid rectangle geometry");

  boxData.setMargin (rectWidgets_.marginEdit ->margin());
  boxData.setPadding(rectWidgets_.paddingEdit->margin());

  auto fill   = rectWidgets_.backgroundDataEdit->data();
  auto stroke = rectWidgets_.strokeDataEdit    ->data();

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

  auto &shapeData = boxData.shape();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  CQChartsPosition center;
  CQChartsLength   rx, ry;

  if (ellipseWidgets_.centerRadio->isChecked()) {
    center = ellipseWidgets_.centerEdit->position();
    rx     = ellipseWidgets_.rxEdit->length();
    ry     = ellipseWidgets_.ryEdit->length();
  }
  else {
    auto rect = ellipseWidgets_.rectEdit->rect();

    center = rect.center();
    rx     = rect.xRadius();
    ry     = rect.yRadius();
  }

  if (rx.value() <= 0.0 || ry.value() <= 0.0)
    return setErrorMsg("Invalid ellipse radius");

  auto fill   = ellipseWidgets_.backgroundDataEdit->data();
  auto stroke = ellipseWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  boxData.setBorderSides(ellipseWidgets_.borderSidesEdit->sides());

  //---

  auto *annotation = dynamic_cast<CQChartsEllipseAnnotation *>(annotation_);
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

  auto &shapeData = boxData.shape();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto polygon = polygonWidgets_.pointsEdit->polygon();

  if (! polygon.isValid(/*closed*/true))
    return setErrorMsg("Not enough polygon points");

  auto fill   = polygonWidgets_.backgroundDataEdit->data();
  auto stroke = polygonWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  //---

  auto *annotation = dynamic_cast<CQChartsPolygonAnnotation *>(annotation_);
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

  auto &shapeData = boxData.shape();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto polygon = polylineWidgets_.pointsEdit->polygon();

  if (! polygon.isValid(/*closed*/false))
    return setErrorMsg("Not enough polygon points");

  auto fill   = polylineWidgets_.backgroundDataEdit->data();
  auto stroke = polylineWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  //---

  auto *annotation = dynamic_cast<CQChartsPolylineAnnotation *>(annotation_);
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
  CQChartsBoxData boxData;

  auto &shapeData = boxData.shape();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto pos  = textWidgets_.positionEdit->position();
  auto rect = textWidgets_.rectEdit->rect();
  auto text = textWidgets_.textEdit->text();

  if (! textWidgets_.positionRadio->isChecked()) {
    if (! rect.isValid())
      return setErrorMsg("Invalid text rectangle");
  }

  if (text.simplified().length() == 0)
    return setErrorMsg("Text string is empty");

  auto textData = textWidgets_.dataEdit->data();

  auto fill   = textWidgets_.backgroundDataEdit->data();
  auto stroke = textWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  boxData.setBorderSides(textWidgets_.borderSidesEdit->sides());

  //---

  auto *annotation = dynamic_cast<CQChartsTextAnnotation *>(annotation_);
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
updateImageAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto pos  = imageWidgets_.positionEdit->position();
  auto rect = imageWidgets_.rectEdit->rect();

  auto imageStr         = imageWidgets_.imageEdit->text();
  auto disabledImageStr = imageWidgets_.disabledImageEdit->text();

  if (! imageWidgets_.positionRadio->isChecked()) {
    if (! rect.isValid())
      return setErrorMsg("Invalid image rectangle");
  }

  //---

  auto *annotation = dynamic_cast<CQChartsImageAnnotation *>(annotation_);
  assert(annotation);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setImage        (CQChartsImage(imageStr        , CQChartsImage::Type::NONE));
  annotation->setDisabledImage(CQChartsImage(disabledImageStr, CQChartsImage::Type::NONE));

  if (imageWidgets_.positionRadio->isChecked())
    annotation->setPosition(pos);
  else
    annotation->setRectangle(rect);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updateArrowAnnotation()
{
  CQChartsShapeData shapeData;

  auto &stroke = shapeData.stroke();
  auto &fill   = shapeData.fill();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto start = arrowWidgets_.startEdit->position();
  auto end   = arrowWidgets_.endEdit  ->position();

  if (start == end)
    return setErrorMsg("Arrow is zero length");

  auto arrowData = arrowWidgets_.dataEdit->data();

  stroke.setWidth(arrowWidgets_.strokeWidthEdit->length());
  stroke.setColor(arrowWidgets_.strokeColorEdit->color());

  fill.setVisible(arrowWidgets_.filledCheck->isChecked());
  fill.setColor  (arrowWidgets_.fillColorEdit->color());

  //---

  auto *annotation = dynamic_cast<CQChartsArrowAnnotation *>(annotation_);
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
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto pos        = pointWidgets_.positionEdit->position();
  auto symbolData = pointWidgets_.dataEdit->data();

  //---

  auto *annotation = dynamic_cast<CQChartsPointAnnotation *>(annotation_);
  assert(annotation);

  //---

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setPosition(pos);

  annotation->setSymbolData(symbolData);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updatePieSliceAnnotation()
{
  return true;
}

bool
CQChartsEditAnnotationDlg::
updateAxisAnnotation()
{
  return true;
}

bool
CQChartsEditAnnotationDlg::
updateKeyAnnotation()
{
  return true;
}

bool
CQChartsEditAnnotationDlg::
updatePointSetAnnotation()
{
  return true;
}

bool
CQChartsEditAnnotationDlg::
updateValueSetAnnotation()
{
  return true;
}

bool
CQChartsEditAnnotationDlg::
updateButtonAnnotation()
{
  return true;
}

//------

void
CQChartsEditAnnotationDlg::
cancelSlot()
{
  close();
}

bool
CQChartsEditAnnotationDlg::
setErrorMsg(const QString &msg)
{
  msgLabel_->setText   (msg);
  msgLabel_->setToolTip(msg);

  return false;
}

void
CQChartsEditAnnotationDlg::
clearErrorMsg()
{
  msgLabel_->setText   ("");
  msgLabel_->setToolTip("");
}
