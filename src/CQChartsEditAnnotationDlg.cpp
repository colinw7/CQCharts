#include <CQChartsEditAnnotationDlg.h>

#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsAnnotation.h>
#include <CQChartsArrow.h>

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
#include <CQChartsImageEdit.h>
#include <CQChartsAngleEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsDialogButtons.h>

#include <CQRealSpin.h>
#include <CQCheckBox.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QFrame>
#include <QComboBox>
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
  setObjectName("editAnnotationDlg");

  if (annotation()->plot())
    setWindowTitle(QString("Edit Plot %1 : %2 Annotation (%3)").
      arg(annotation()->plot()->id()).arg(annotation()->typeName()).arg(annotation()->id()));
  else
    setWindowTitle(QString("Edit View : %1 Annotation (%2)").
      arg(annotation()->typeName()).arg(annotation()->id()));

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //----

  auto gridData = createGrid(layout);

  //--

  // id, tip edits
  idEdit_  = addLineEdit(gridData, "id" , "Id" , annotation()->id   (), "Annotation Id");
  tipEdit_ = addLineEdit(gridData, "tip", "Tip", annotation()->tipId(), "Annotation Tooltip");

  //----

  // create widgets for annotation type
  frameLayout_ = CQUtil::makeLayout<QVBoxLayout>(2, 2);

  if      (annotation()->type() == CQChartsAnnotation::Type::RECT)
    createRectFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::ELLIPSE)
    createEllipseFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::POLYGON)
    createPolygonFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::POLYLINE)
    createPolyLineFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::TEXT)
    createTextFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::IMAGE)
    createImageFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::PATH)
    createPathFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::ARROW)
    createArrowFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::POINT)
    createPointFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::PIE_SLICE)
    createPieSliceFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::AXIS)
    createAxisFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::KEY)
    createKeyFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::POINT_SET)
    createPointSetFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::VALUE_SET)
    createValueSetFrame();
  else if (annotation()->type() == CQChartsAnnotation::Type::BUTTON)
    createButtonFrame();

  layout->addLayout(frameLayout_);

  //---

  // message label
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

  auto gridData = createGrid(frameLayout);

  //--

  // rect
  rectWidgets_.rectEdit =
    addRectEdit(gridData, "rectEdit", "Rect", annotation->rectangle(), "Rectangle");

  //--

  // outer margin
  rectWidgets_.marginEdit =
    addMarginEdit(gridData, "marginEdit", "Margin", annotation->margin(), "Rectangle Margin");

  // inner padding
  rectWidgets_.paddingEdit =
    addMarginEdit(gridData, "paddingEdit", "Padding", annotation->padding(), "Rectangle Padding");

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

  ellipseWidgets_.centerRadio->setChecked(true);

  frameLayout->addWidget(centerRectFrame);

  connect(ellipseWidgets_.centerRadio, SIGNAL(toggled(bool)), this, SLOT(ellipseCenterSlot(bool)));

  //---

  auto gridData = createGrid(frameLayout);

  //---

  ellipseWidgets_.centerEdit =
    addPositionEdit(gridData, "centerEdit", "Center", annotation->center(),
                    "Ellipse Center Position");

  ellipseWidgets_.rxEdit =
    addLengthEdit(gridData, "rxEdit", "Radius X", annotation->xRadius(),
                  "Ellipse X Radius Length");
  ellipseWidgets_.ryEdit =
    addLengthEdit(gridData, "ryEdit", "Radius Y", annotation->yRadius(),
                  "Ellipse Y Radius Length");

  ellipseWidgets_.rectEdit =
    addRectEdit(gridData, "rectEdit", "Rect", CQChartsRect(), "Ellipse Rectangle");

  ellipseCenterSlot(true);

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

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polygonWidgets_.frame, 2, 2);

  polygonWidgets_.frame = CQUtil::makeWidget<QFrame>("polygonFrame");

  frameLayout_->addWidget(polygonWidgets_.frame);

  //---

  auto gridData = createGrid(frameLayout);

  //---

  polygonWidgets_.pointsEdit =
    addPolygonEdit(gridData, "pointsEdit", "Points", annotation->polygon(), "Polygon Points");

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

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polylineWidgets_.frame, 2, 2);

  polylineWidgets_.frame = CQUtil::makeWidget<QFrame>("polylineFrame");

  frameLayout_->addWidget(polylineWidgets_.frame);

  //---

  auto gridData = createGrid(frameLayout);

  //---

  polylineWidgets_.pointsEdit =
    addPolygonEdit(gridData, "pointsEdit", "Points", annotation->polygon(), "Polyline Points");

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

  if (annotation->position().isSet()) {
    textWidgets_.positionRadio->setChecked(true);
    textWidgets_.positionEdit ->setPosition(annotation->positionValue());
  }

  if (annotation->rectangle().isSet()) {
    textWidgets_.rectRadio->setChecked(true);
    textWidgets_.rectEdit ->setRect(annotation->rectangleValue());
  }

  frameLayout->addWidget(positionRectFrame);

  connect(textWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(textPositionSlot(bool)));

  //---

  auto gridData1 = createGrid(frameLayout);

  //--

  textWidgets_.positionEdit =
    addPositionEdit(gridData1, "positionEdit", "Position", CQChartsPosition(), "Text Position");
  textWidgets_.rectEdit =
    addRectEdit(gridData1, "rectEdit", "Rect", CQChartsRect(), "Text Rectangle");

  //---

  auto gridData = createGrid(frameLayout);

  //---

  textWidgets_.textEdit =
    addLineEdit(gridData, "edit", "Text", annotation->textStr(), "Text String");

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

  textPositionSlot(true);
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

  if (annotation->position().isSet()) {
    imageWidgets_.positionRadio->setChecked(true);
    imageWidgets_.positionEdit ->setPosition(annotation->positionValue());
  }

  if (annotation->rectangle().isSet()) {
    imageWidgets_.rectRadio->setChecked(true);
    imageWidgets_.rectEdit ->setRect(annotation->rectangleValue());
  }

  frameLayout->addWidget(positionRectFrame);

  connect(imageWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(imagePositionSlot(bool)));

  //---

  auto gridData1 = createGrid(frameLayout);

  //--

  imageWidgets_.positionEdit =
    addPositionEdit(gridData1, "positionEdit", "Position", CQChartsPosition(), "Image Position");
  imageWidgets_.rectEdit =
    addRectEdit(gridData1, "rectEdit", "Rect", CQChartsRect(), "Image Rectangle");

  //---

  auto gridData = createGrid(frameLayout);

  //--

  imageWidgets_.imageEdit =
    addImageEdit(gridData, "imageEdit", "Image", annotation->image(), "Image");
  imageWidgets_.disabledImageEdit =
    addImageEdit(gridData, "disabledImageEdit", "Disabled", annotation->disabledImage(),
                 "Disabled Image");

  //---

  frameLayout->addStretch(1);

  imagePositionSlot(true);
}

void
CQChartsEditAnnotationDlg::
createPathFrame()
{
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

  auto gridData = createGrid(frameLayout);

  //--

  // start, end point
  arrowWidgets_.startEdit =
    addPositionEdit(gridData, "startEdit", "Start", annotation->start(), "Arrow Start Point");
  arrowWidgets_.endEdit =
    addPositionEdit(gridData, "endEdit"  , "End"  , annotation->end  (), "Arrow End Point"  );

  //---

  // arrow data edit
  arrowWidgets_.dataEdit = CQUtil::makeWidget<CQChartsArrowDataEdit>("dataEdit");

  arrowWidgets_.dataEdit->setPlot(annotation->plot());
  arrowWidgets_.dataEdit->setView(annotation->view());
  arrowWidgets_.dataEdit->setData(annotation->arrowData());

  frameLayout->addWidget(arrowWidgets_.dataEdit);

  //---

  auto gridData1 = createGrid(frameLayout);

  //--

  // stroke width, stroke color, filled and fill color
  arrowWidgets_.strokeWidthEdit =
    addLengthEdit(gridData1, "strokeWidthEdit", "Stroke Width", stroke.width(),
                  "Arrow Stroke Width");
  arrowWidgets_.strokeColorEdit =
    addColorEdit(gridData1, "strokeColorEdit", "Stroke Color", stroke.color(),
                 "Arrow Stroke Color");

  arrowWidgets_.filledCheck =
    addBoolEdit(gridData1, "filledCheck", "Filled", fill.isVisible(), "Arrow Is Filled");

  arrowWidgets_.fillColorEdit =
    addColorEdit(gridData1, "fillColorEdit", "Fill Color", fill.color(),
                 "Arrow Fll Color");

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

  auto gridData = createGrid(frameLayout);

  //---

  // position
  pointWidgets_.positionEdit =
    addPositionEdit(gridData, "positionEdit", "Position", annotation->position(), "Point Position");

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
  auto *annotation = dynamic_cast<CQChartsPieSliceAnnotation *>(annotation_);
  assert(annotation);

  //---

  pieSliceWidgets_.frame = CQUtil::makeWidget<QFrame>("pieSliceFrame");

  frameLayout_->addWidget(pieSliceWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(pieSliceWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //---

  // center
  pieSliceWidgets_.centerEdit =
    addPositionEdit(gridData, "center", "Center", annotation->position(), "Center");

  //--

  // radii
  pieSliceWidgets_.innerRadiusEdit =
    addLengthEdit(gridData, "innerRadius", "Inner Radius", annotation->innerRadius(),
                  "Inner Radius");
  pieSliceWidgets_.outerRadiusEdit =
    addLengthEdit(gridData, "outerRadius", "Outer Radius", annotation->outerRadius(),
                  "Outer Radius");

  //---

  // angles
  pieSliceWidgets_.startAngleEdit =
    addAngleEdit(gridData, "startAngle", "Start Angle", annotation->startAngle(), "Start Angle");
  pieSliceWidgets_.spanAngleEdit  =
    addAngleEdit(gridData, "spanAngle" , "Span Angle", annotation->spanAngle(), "Span Angle");

  //---

  frameLayout->addStretch(1);
}

void
CQChartsEditAnnotationDlg::
createAxisFrame()
{
  auto *annotation = dynamic_cast<CQChartsAxisAnnotation *>(annotation_);
  assert(annotation);

  //---

  axisWidgets_.frame = CQUtil::makeWidget<QFrame>("axisFrame");

  frameLayout_->addWidget(axisWidgets_.frame);

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(axisWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //---

  auto orientNames = QStringList() << "Horizontal" << "Vertical";

  axisWidgets_.orientationEdit =
    addComboEdit(gridData, "orientation", "Orientation", orientNames, "Orientation");

  if (annotation->direction() == Qt::Vertical)
    axisWidgets_.orientationEdit->setCurrentIndex(1);

  axisWidgets_.positionEdit =
    addRealEdit(gridData, "position", "Position", annotation->position(), "Position");

  axisWidgets_.startEdit =
    addRealEdit(gridData, "start", "Start", annotation->start(), "Start Value");
  axisWidgets_.endEdit =
    addRealEdit(gridData, "end"  , "End"  , annotation->end  (), "End Value");

  //---

  frameLayout->addStretch(1);
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
  auto boxData = annotation()->boxData();

  auto &fillData = boxData.shape().fill();

  //---

  widgets.backgroundDataEdit = CQUtil::makeWidget<CQChartsFillDataEdit>("backgroundDataEdit");

  widgets.backgroundDataEdit->setTitle("Fill");
  widgets.backgroundDataEdit->setPlot(annotation()->plot());
  widgets.backgroundDataEdit->setView(annotation()->view());
  widgets.backgroundDataEdit->setData(fillData);
  widgets.backgroundDataEdit->setToolTip("Enable Fill");

  playout->addWidget(widgets.backgroundDataEdit);
}

void
CQChartsEditAnnotationDlg::
addStrokeWidgets(Widgets &widgets, QBoxLayout *playout, bool cornerSize)
{
  auto boxData = annotation()->boxData();

  auto &strokeData = boxData.shape().stroke();

  //---

  widgets.strokeDataEdit = new CQChartsStrokeDataEdit(nullptr,
                             CQChartsStrokeDataEditConfig().setCornerSize(cornerSize));

  widgets.strokeDataEdit->setTitle("Stroke");
  widgets.strokeDataEdit->setPlot(annotation()->plot());
  widgets.strokeDataEdit->setView(annotation()->view());
  widgets.strokeDataEdit->setData(strokeData);
  widgets.strokeDataEdit->setToolTip("Enable Stroke");

  playout->addWidget(widgets.strokeDataEdit);
}

void
CQChartsEditAnnotationDlg::
addSidesWidget(Widgets &widgets, QBoxLayout *playout)
{
  auto boxData = annotation()->boxData();

  //--

  auto gridData = createGrid(playout);

  //---

  widgets.borderSidesEdit = CQUtil::makeWidget<CQChartsSidesEdit>("borderSidesEdit");

  widgets.borderSidesEdit->setSides(boxData.borderSides());

  widgets.borderSidesEdit->setToolTip("Border Sides");

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, "Border Sides",
                                         widgets.borderSidesEdit, gridData.row);
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

CQCheckBox *
CQChartsEditAnnotationDlg::
addBoolEdit(GridData &gridData, const QString &name, const QString &label, bool checked,
            const QString &tip) const
{
  auto *check = CQUtil::makeWidget<CQCheckBox>(name);

  if (tip != "")
    check->setToolTip(tip);

  check->setChecked(checked);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, check, gridData.row);

  return check;
}

CQChartsLineEdit *
CQChartsEditAnnotationDlg::
addLineEdit(GridData &gridData, const QString &name, const QString &label,
            const QString &text, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(name);

  edit->setText(text);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQRealSpin *
CQChartsEditAnnotationDlg::
addRealEdit(GridData &gridData, const QString &name, const QString &label,
            double r, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQRealSpin>(name);

  edit->setValue(r);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

QComboBox *
CQChartsEditAnnotationDlg::
addComboEdit(GridData &gridData, const QString &name, const QString &label,
             const QStringList &items, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<QComboBox>(name);

  if (tip != "")
    edit->setToolTip(tip);

  edit->addItems(items);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsPositionEdit *
CQChartsEditAnnotationDlg::
addPositionEdit(GridData &gridData, const QString &name, const QString &label,
                const CQChartsPosition &pos, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsPositionEdit>(name);

  edit->setView(annotation()->view());
  edit->setPlot(annotation()->plot());

  edit->setPosition(pos);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsLengthEdit *
CQChartsEditAnnotationDlg::
addLengthEdit(GridData &gridData, const QString &name, const QString &label,
              const CQChartsLength &len, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsLengthEdit>(name);

  edit->setLength(len);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsRectEdit *
CQChartsEditAnnotationDlg::
addRectEdit(GridData &gridData, const QString &name, const QString &label,
            const CQChartsRect &rect, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsRectEdit>(name);

  edit->setView(annotation()->view());
  edit->setPlot(annotation()->plot());

  edit->setRect(rect);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsPolygonEdit *
CQChartsEditAnnotationDlg::
addPolygonEdit(GridData &gridData, const QString &name, const QString &label,
               const CQChartsPolygon &poly, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsPolygonEdit>(name);

  edit->setView(annotation()->view());
  edit->setPlot(annotation()->plot());

  edit->setPolygon(poly);

  if      (annotation()->view()) edit->setUnits(Units::VIEW);
  else if (annotation()->plot()) edit->setUnits(Units::PLOT);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsMarginEdit *
CQChartsEditAnnotationDlg::
addMarginEdit(GridData &gridData, const QString &name, const QString &label,
              const CQChartsMargin &margin, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsMarginEdit>(name);

  edit->setMargin(margin);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsImageEdit *
CQChartsEditAnnotationDlg::
addImageEdit(GridData &gridData, const QString &name, const QString &label,
             const CQChartsImage &image, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsImageEdit>(name);

  edit->setImage(image);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsColorLineEdit *
CQChartsEditAnnotationDlg::
addColorEdit(GridData &gridData, const QString &name, const QString &label,
             const CQChartsColor &color, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsColorLineEdit>(name);

  edit->setColor(color);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsAngleEdit *
CQChartsEditAnnotationDlg::
addAngleEdit(GridData &gridData, const QString &name, const QString &label,
             const CQChartsAngle &a, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsAngleEdit>(name);

  edit->setAngle(a);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

//---

CQChartsEditAnnotationDlg::GridData
CQChartsEditAnnotationDlg::
createGrid(QBoxLayout *layout) const
{
  GridData gridData;

  gridData.layout = CQUtil::makeLayout<QGridLayout>(2, 2);

  layout->addLayout(gridData.layout);

  return gridData;
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
    Units               units1 { Units::PIXEL };

    if      (annotation->view()) {
      center1 = annotation->view()->positionToView(center);
      xr1     = annotation->view()->lengthViewWidth(xr);
      yr1     = annotation->view()->lengthViewHeight(yr);
      units1  = Units::VIEW;
    }
    else if (annotation->plot()) {
      center1 = annotation->plot()->positionToPlot(center);
      xr1     = annotation->plot()->lengthPlotWidth(xr);
      yr1     = annotation->plot()->lengthPlotHeight(yr);
      units1  = Units::PLOT;
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

  if      (annotation()->type() == CQChartsAnnotation::Type::RECT)
    rc = updateRectangleAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::ELLIPSE)
    rc = updateEllipseAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::POLYGON)
    rc = updatePolygonAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::POLYLINE)
    rc = updatePolylineAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::TEXT)
    rc = updateTextAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::IMAGE)
    rc = updateImageAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::PATH)
    rc = updatePathAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::ARROW)
    rc = updateArrowAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::POINT)
    rc = updatePointAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::PIE_SLICE)
    rc = updatePieSliceAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::AXIS)
    rc = updateAxisAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::KEY)
    rc = updateKeyAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::POINT_SET)
    rc = updatePointSetAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::VALUE_SET)
    rc = updateValueSetAnnotation();
  else if (annotation()->type() == CQChartsAnnotation::Type::BUTTON)
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

  auto *annotation = dynamic_cast<CQChartsRectangleAnnotation *>(annotation_);
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

  if (text.trimmed().length() == 0)
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

  auto image         = imageWidgets_.imageEdit->image();
  auto disabledImage = imageWidgets_.disabledImageEdit->image();

  if (! imageWidgets_.positionRadio->isChecked()) {
    if (! rect.isValid())
      return setErrorMsg("Invalid image rectangle");
  }

  //---

  auto *annotation = dynamic_cast<CQChartsImageAnnotation *>(annotation_);
  assert(annotation);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setImage        (image);
  annotation->setDisabledImage(disabledImage);

  if (imageWidgets_.positionRadio->isChecked())
    annotation->setPosition(pos);
  else
    annotation->setRectangle(rect);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updatePathAnnotation()
{
  return false;
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
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto position    = pieSliceWidgets_.centerEdit->position();
  auto innerRadius = pieSliceWidgets_.innerRadiusEdit->length();
  auto outerRadius = pieSliceWidgets_.outerRadiusEdit->length();
  auto startAngle  = pieSliceWidgets_.startAngleEdit->angle();
  auto spanAngle   = pieSliceWidgets_.spanAngleEdit->angle();

  //---

  auto *annotation = dynamic_cast<CQChartsPieSliceAnnotation *>(annotation_);
  assert(annotation);

  //---

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setPosition   (position);
  annotation->setInnerRadius(innerRadius);
  annotation->setOuterRadius(outerRadius);
  annotation->setStartAngle (startAngle);
  annotation->setSpanAngle  (spanAngle);

  return true;
}

bool
CQChartsEditAnnotationDlg::
updateAxisAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto direction = (axisWidgets_.orientationEdit->currentIndex() == 0 ?
                    Qt::Horizontal : Qt::Vertical);
  auto position  = axisWidgets_.positionEdit->value();
  auto start     = axisWidgets_.startEdit->value();
  auto end       = axisWidgets_.endEdit->value();

  //---

  auto *annotation = dynamic_cast<CQChartsAxisAnnotation *>(annotation_);
  assert(annotation);

  //---

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setDirection(direction);
  annotation->setPosition (position);
  annotation->setStart    (start);
  annotation->setEnd      (end);

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
