#include <CQChartsCreateAnnotationDlg.h>
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
#include <QStackedWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>

CQChartsCreateAnnotationDlg::
CQChartsCreateAnnotationDlg(QWidget *parent, CQChartsView *view) :
 QDialog(parent), view_(view)
{
  initWidgets();
}

CQChartsCreateAnnotationDlg::
CQChartsCreateAnnotationDlg(QWidget *parent, CQChartsPlot *plot) :
 QDialog(parent), plot_(plot)
{
  initWidgets();
}

void
CQChartsCreateAnnotationDlg::
initWidgets()
{
  setObjectName("createAnnotationDlg");

  setWindowTitle("Create Annotation");

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //----

  // type combo
  typeCombo_ = CQUtil::makeWidget<QComboBox>("type");

  typeCombo_->addItems(typeNames());

  connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSlot(int)));

  addLabelWidget(layout, "Type", typeCombo_);

  //----

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  layout->addLayout(gridLayout);

  int row = 0;

  //--

  // id, tip edits
  idEdit_  = createLineEdit("id" , "Annotation Id");
  tipEdit_ = createLineEdit("tip", "Annotation Tooltip");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Id" , idEdit_ , row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Tip", tipEdit_, row);

  //----

  // per type widgets stack
  typeStack_ = CQUtil::makeWidget<QStackedWidget>("typeStack");

  layout->addWidget(typeStack_);

  // create widget frames
  createRectFrame    ();
  createEllipseFrame ();
  createPolygonFrame ();
  createPolyLineFrame();
  createTextFrame    ();
  createImageFrame   ();
  createPathFrame    ();
  createArrowFrame   ();
  createPointFrame   ();
  createPieSliceFrame();

  if (plot_)
    createAxisFrame();

  createKeyFrame     ();
  createPointSetFrame();
  createValueSetFrame();
  createButtonFrame  ();

  //---

  // add to stack
  typeStack_->addWidget(rectWidgets_    .frame);
  typeStack_->addWidget(ellipseWidgets_ .frame);
  typeStack_->addWidget(polygonWidgets_ .frame);
  typeStack_->addWidget(polylineWidgets_.frame);
  typeStack_->addWidget(textWidgets_    .frame);
  typeStack_->addWidget(imageWidgets_   .frame);
  typeStack_->addWidget(pathWidgets_    .frame);
  typeStack_->addWidget(arrowWidgets_   .frame);
  typeStack_->addWidget(pointWidgets_   .frame);
  typeStack_->addWidget(pieSliceWidgets_.frame);

  if (plot_)
    typeStack_->addWidget(axisWidgets_.frame);

  typeStack_->addWidget(keyWidgets_     .frame);
  typeStack_->addWidget(pointSetWidgets_.frame);
  typeStack_->addWidget(valueSetWidgets_.frame);
  typeStack_->addWidget(buttonWidgets_  .frame);

  //---

  msgLabel_ = CQUtil::makeWidget<QLabel>("msgLabel");

  CQChartsWidgetUtil::setTextColor(msgLabel_, Qt::red);

  layout->addWidget(msgLabel_);

  //---

  // OK, Apply, Cancel Buttons
  auto *buttons = CQUtil::makeWidget<CQChartsDialogButtons>("buttons");

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  layout->addWidget(buttons);

  //---

  typeSlot(0);
}

QStringList
CQChartsCreateAnnotationDlg::
typeNames() const
{
  static QStringList strs;

  if (strs.length() == 0) {
    strs << "Rectangle" << "Ellipse" << "Polygon" << "Polyline" << "Text" << "Image" <<
            "Path" << "Arrow" << "Point" << "Pie Slice";

    if (plot_)
      strs << "Axis";

    strs << "Key" << "Point Set" << "Value Set" << "Button";
  }

  return strs;
}

//------

void
CQChartsCreateAnnotationDlg::
createRectFrame()
{
  rectWidgets_.frame = CQUtil::makeWidget<QFrame>("rectFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(rectWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // rect
  rectWidgets_.rectEdit = createRectEdit("rectEdit", "Rectangle");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Rect", rectWidgets_.rectEdit, row);

  //--

  // outer margin
  rectWidgets_.marginEdit = createMarginEdit("marginEdit", "Rectangle Margin");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Margin", rectWidgets_.marginEdit, row);

  //--

  // inner padding
  rectWidgets_.paddingEdit = createMarginEdit("paddingEdit", "Rectangle Padding");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Padding", rectWidgets_.paddingEdit, row);

  //---

  addFillWidgets  (rectWidgets_, frameLayout);
  addStrokeWidgets(rectWidgets_, frameLayout, /*cornerSize*/true);

  addSidesWidget(rectWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsCreateAnnotationDlg::
createEllipseFrame()
{
  ellipseWidgets_.frame = CQUtil::makeWidget<QFrame>("ellipseFrame");

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
    createPositionEdit("centerEdit", 0.0, 0.0, "Ellipse Center Position");

  ellipseWidgets_.rxEdit = createLengthEdit("rxEdit", 1.0, "Ellipse X Radius Length");
  ellipseWidgets_.ryEdit = createLengthEdit("ryEdit", 1.0, "Ellipse Y Radius Length");

  ellipseWidgets_.rectEdit = createRectEdit("rectEdit", "Ellipse Rectangle");

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
CQChartsCreateAnnotationDlg::
createPolygonFrame()
{
  polygonWidgets_.frame = CQUtil::makeWidget<QFrame>("polygonFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polygonWidgets_.frame, 2, 2);

  //---

  polygonWidgets_.pointsEdit = createPolygonEdit("pointsEdit", "Polygon Points");

  addLabelWidget(frameLayout, "Points", polygonWidgets_.pointsEdit);

  //---

  addFillWidgets  (polygonWidgets_, frameLayout);
  addStrokeWidgets(polygonWidgets_, frameLayout, /*cornerSize*/false);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsCreateAnnotationDlg::
createPolyLineFrame()
{
  polylineWidgets_.frame = CQUtil::makeWidget<QFrame>("polylineFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polylineWidgets_.frame, 2, 2);

  //---

  polylineWidgets_.pointsEdit = createPolygonEdit("pointsEdit", "Polyline Points");

  addLabelWidget(frameLayout, "Points", polylineWidgets_.pointsEdit);

  //---

  addFillWidgets  (polylineWidgets_, frameLayout);
  addStrokeWidgets(polylineWidgets_, frameLayout, /*cornerSize*/false);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsCreateAnnotationDlg::
createTextFrame()
{
  textWidgets_.frame = CQUtil::makeWidget<QFrame>("textFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(textWidgets_.frame, 2, 2);

  //---

  auto *positionRectFrame  = CQUtil::makeWidget<QFrame>("positionRectFrame");
  auto *positionRectLayout = CQUtil::makeLayout<QHBoxLayout>(positionRectFrame, 2, 2);

  textWidgets_.positionRadio = CQUtil::makeLabelWidget<QRadioButton>("Position", "positionRadio");
  textWidgets_.positionRadio->setToolTip("Create text at position");

  textWidgets_.rectRadio = CQUtil::makeLabelWidget<QRadioButton>("Rect", "rectRadio");
  textWidgets_.rectRadio->setToolTip("Create text in rectangle");

  positionRectLayout->addWidget(textWidgets_.positionRadio);
  positionRectLayout->addWidget(textWidgets_.rectRadio);
  positionRectLayout->addStretch(1);

  connect(textWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(textPositionSlot(bool)));

  textWidgets_.positionEdit = createPositionEdit("positionEdit", 0.0, 0.0, "Text Position");
  textWidgets_.rectEdit     = createRectEdit    ("rectEdit", "Text Rectangle");

  textWidgets_.positionRadio->setChecked(true);

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

  textWidgets_.textEdit = createLineEdit("edit", "Text String");

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Text", textWidgets_.textEdit, row);

  //---

  textWidgets_.dataEdit = new CQChartsTextDataEdit(nullptr, /*optional*/false);

  textWidgets_.dataEdit->setPlot(plot());
  textWidgets_.dataEdit->setView(view());

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
CQChartsCreateAnnotationDlg::
createImageFrame()
{
  imageWidgets_.frame = CQUtil::makeWidget<QFrame>("imageFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(imageWidgets_.frame, 2, 2);

  //---

  auto *positionRectFrame  = CQUtil::makeWidget<QFrame>("positionRectFrame");
  auto *positionRectLayout = CQUtil::makeLayout<QHBoxLayout>(positionRectFrame, 2, 2);

  imageWidgets_.positionRadio = CQUtil::makeLabelWidget<QRadioButton>("Position", "positionRadio");
  imageWidgets_.positionRadio->setToolTip("Create image at position");

  imageWidgets_.rectRadio = CQUtil::makeLabelWidget<QRadioButton>("Rect"    , "rectRadio");
  imageWidgets_.rectRadio->setToolTip("Create image in rectangle");

  positionRectLayout->addWidget(imageWidgets_.positionRadio);
  positionRectLayout->addWidget(imageWidgets_.rectRadio);
  positionRectLayout->addStretch(1);

  connect(imageWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(imagePositionSlot(bool)));

  imageWidgets_.positionEdit = createPositionEdit("positionEdit", 0.0, 0.0, "Image Position");
  imageWidgets_.rectEdit     = createRectEdit    ("rectEdit", "Image Rectangle");

  imageWidgets_.positionRadio->setChecked(true);

  frameLayout->addWidget(positionRectFrame);

  imagePositionSlot(true);

  //---

  auto *gridLayout1 = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout1);

  int row1 = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Position", imageWidgets_.positionEdit, row1);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout1, "Rect"    , imageWidgets_.rectEdit    , row1);

  //---

  imageWidgets_.imageEdit         = createLineEdit("imageEdit"        , "Image");
  imageWidgets_.disabledImageEdit = createLineEdit("disabledImageEdit", "Disabled Image");

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Image",
    imageWidgets_.imageEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Disabled Image",
    imageWidgets_.disabledImageEdit, row);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsCreateAnnotationDlg::
createPathFrame()
{
}

void
CQChartsCreateAnnotationDlg::
createArrowFrame()
{
  arrowWidgets_.frame = CQUtil::makeWidget<QFrame>("arrowFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(arrowWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // start, end point
  arrowWidgets_.startEdit = createPositionEdit("startEdit", 0.0, 0.0, "Arrow Start Point");
  arrowWidgets_.endEdit   = createPositionEdit("endEdit"  , 1.0, 1.0, "Arrow End Point"  );

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Start", arrowWidgets_.startEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "End"  , arrowWidgets_.endEdit  , row);

  //---

  // arrow data edit
  arrowWidgets_.dataEdit = CQUtil::makeWidget<CQChartsArrowDataEdit>("dataEdit");

  arrowWidgets_.dataEdit->setPlot(plot());
  arrowWidgets_.dataEdit->setView(view());

  auto len = CQChartsViewPlotObj::makeLength(view(), plot(), 1);

  arrowWidgets_.dataEdit->lengthEdit()->setLength(len);

  frameLayout->addWidget(arrowWidgets_.dataEdit);

  //---

  auto *gridLayout1 = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout1);

  int row1 = 0;

  //--

  // stroke width, stroke color, filled and fill color
  arrowWidgets_.strokeWidthEdit = createLengthEdit("strokeWidthEdit", 0.0, "Arrow Stroke Width");
  arrowWidgets_.strokeColorEdit = createColorEdit ("strokeColorEdit", "Arrow Stroke Color");

  arrowWidgets_.filledCheck = CQUtil::makeWidget<CQCheckBox>("filledCheck");
  arrowWidgets_.filledCheck->setToolTip("Is Filled");

  arrowWidgets_.fillColorEdit = createColorEdit("fillColorEdit", "Arrow Fill Color");

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
CQChartsCreateAnnotationDlg::
createPointFrame()
{
  pointWidgets_.frame = CQUtil::makeWidget<QFrame>("pointFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(pointWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //---

  // position
  pointWidgets_.positionEdit = createPositionEdit("positionEdit", 0.0, 0.0, "Point Position");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Position", pointWidgets_.positionEdit, row);

  //---

  pointWidgets_.dataEdit = new CQChartsSymbolDataEdit(nullptr, /*optional*/false);

  pointWidgets_.dataEdit->setPlot(plot());
  pointWidgets_.dataEdit->setView(view());

  frameLayout->addWidget(pointWidgets_.dataEdit);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsCreateAnnotationDlg::
createPieSliceFrame()
{
  pieSliceWidgets_.frame = CQUtil::makeWidget<QFrame>("pieSliceFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(pieSliceWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // center
  pieSliceWidgets_.centerEdit = createPositionEdit("center", 0.0, 0.0, "Center Point");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Center", pieSliceWidgets_.centerEdit, row);

  //--

  // radii
  pieSliceWidgets_.innerRadiusEdit = createLengthEdit("innerRadius", 0.0, "Inner Radius");
  pieSliceWidgets_.outerRadiusEdit = createLengthEdit("outerRadius", 1.0, "Outer Radius");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Inner Radius",
    pieSliceWidgets_.innerRadiusEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Outer Radius",
    pieSliceWidgets_.outerRadiusEdit, row);

  //---

  // angles
  pieSliceWidgets_.startAngleEdit = createAngleEdit("startAngle",   0.0, "Start Angle");
  pieSliceWidgets_.spanAngleEdit  = createAngleEdit("spanAngle" , 360.0, "Span Angle");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Start Angle",
    pieSliceWidgets_.startAngleEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Span Angle",
    pieSliceWidgets_.spanAngleEdit, row);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsCreateAnnotationDlg::
createAxisFrame()
{
  axisWidgets_.frame = CQUtil::makeWidget<QFrame>("axisFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(axisWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  axisWidgets_.orientationEdit = CQUtil::makeWidget<QComboBox>("orientation");

  axisWidgets_.orientationEdit->addItem("Horizontal");
  axisWidgets_.orientationEdit->addItem("Vertical"  );

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Orientation",
                                         axisWidgets_.orientationEdit, row);

  //--

  axisWidgets_.positionEdit = createRealEdit("position", 0.0, "Position");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Position", axisWidgets_.positionEdit, row);

  //--

  axisWidgets_.startEdit = createRealEdit("start", 0.0, "Start Value");
  axisWidgets_.endEdit   = createRealEdit("end"  , 1.0, "End Value");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Start", axisWidgets_.startEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "End"  , axisWidgets_.endEdit  , row);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsCreateAnnotationDlg::
createKeyFrame()
{
  keyWidgets_.frame = CQUtil::makeWidget<QFrame>("keyFrame");
}

void
CQChartsCreateAnnotationDlg::
createPointSetFrame()
{
  pointSetWidgets_.frame = CQUtil::makeWidget<QFrame>("pointSetFrame");
}

void
CQChartsCreateAnnotationDlg::
createValueSetFrame()
{
  valueSetWidgets_.frame = CQUtil::makeWidget<QFrame>("valueSetFrame");
}

void
CQChartsCreateAnnotationDlg::
createButtonFrame()
{
  buttonWidgets_.frame = CQUtil::makeWidget<QFrame>("buttonFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(buttonWidgets_.frame, 2, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  buttonWidgets_.positionEdit = createPositionEdit("positionEdit", 0.0, 0.0, "Button Position");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Position", buttonWidgets_.positionEdit, row);

  buttonWidgets_.textEdit = createLineEdit("edit", "Text String");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Text", buttonWidgets_.textEdit, row);

  //---

  frameLayout->addStretch(1);
}

//------

void
CQChartsCreateAnnotationDlg::
addFillWidgets(Widgets &widgets, QBoxLayout *playout)
{
  widgets.backgroundDataEdit = CQUtil::makeWidget<CQChartsFillDataEdit>("backgroundDataEdit");

  widgets.backgroundDataEdit->setTitle("Fill");
  widgets.backgroundDataEdit->setToolTip("Enable Fill");

  widgets.backgroundDataEdit->setPlot(plot());
  widgets.backgroundDataEdit->setView(view());

  playout->addWidget(widgets.backgroundDataEdit);
}

void
CQChartsCreateAnnotationDlg::
addStrokeWidgets(Widgets &widgets, QBoxLayout *playout, bool cornerSize)
{
  widgets.strokeDataEdit = new CQChartsStrokeDataEdit(nullptr,
                             CQChartsStrokeDataEditConfig().setCornerSize(cornerSize));

  widgets.strokeDataEdit->setTitle("Stroke");
  widgets.strokeDataEdit->setToolTip("Enable Stroke");

  widgets.strokeDataEdit->setPlot(plot());
  widgets.strokeDataEdit->setView(view());

  playout->addWidget(widgets.strokeDataEdit);
}

void
CQChartsCreateAnnotationDlg::
addSidesWidget(Widgets &widgets, QBoxLayout *playout)
{
  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  int row = 0;

  //--

  widgets.borderSidesEdit = CQUtil::makeWidget<CQChartsSidesEdit>("borderSidesEdit");

  widgets.borderSidesEdit->setSides(CQChartsSides::Side::ALL);

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Border Sides", widgets.borderSidesEdit, row);

  //--

  playout->addLayout(gridLayout);
}

QHBoxLayout *
CQChartsCreateAnnotationDlg::
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
CQChartsCreateAnnotationDlg::
createLineEdit(const QString &name, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(name);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQRealSpin *
CQChartsCreateAnnotationDlg::
createRealEdit(const QString &name, double r, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQRealSpin>(name);

  edit->setValue(r);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsPositionEdit *
CQChartsCreateAnnotationDlg::
createPositionEdit(const QString &name, double x, double y, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsPositionEdit>(name);

  edit->setPlot(plot());

  auto pos = CQChartsViewPlotObj::makePosition(view(), plot(), x, y);

  edit->setPosition(pos);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsLengthEdit *
CQChartsCreateAnnotationDlg::
createLengthEdit(const QString &name, double l, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsLengthEdit>(name);

  auto len = CQChartsViewPlotObj::makeLength(view(), plot(), l);

  edit->setLength(len);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsRectEdit *
CQChartsCreateAnnotationDlg::
createRectEdit(const QString &name, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsRectEdit>(name);

  edit->setPlot(plot());

  auto rect = CQChartsViewPlotObj::makeRect(view(), plot(), 0, 0, 1, 1);

  edit->setRect(rect);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsPolygonEdit *
CQChartsCreateAnnotationDlg::
createPolygonEdit(const QString &name, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsPolygonEdit>(name);

  edit->setPlot(plot());

  if      (view()) edit->setUnits(CQChartsUnits::VIEW);
  else if (plot()) edit->setUnits(CQChartsUnits::PLOT);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsMarginEdit *
CQChartsCreateAnnotationDlg::
createMarginEdit(const QString &name, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsMarginEdit>(name);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsColorLineEdit *
CQChartsCreateAnnotationDlg::
createColorEdit(const QString &name, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsColorLineEdit>(name);

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

CQChartsAngleEdit *
CQChartsCreateAnnotationDlg::
createAngleEdit(const QString &name, double a, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsAngleEdit>(name);

  edit->setAngle(CQChartsAngle(a));

  if (tip != "")
    edit->setToolTip(tip);

  return edit;
}

//---

void
CQChartsCreateAnnotationDlg::
typeSlot(int ind)
{
  if      (view())
    setWindowTitle(QString("Create View %1 Annotation").
      arg(typeCombo_->currentText()));
  else if (plot())
    setWindowTitle(QString("Create Plot %1 Annotation (%2)").
      arg(typeCombo_->currentText()).arg(plot()->id()));

  typeStack_->setCurrentIndex(ind);

  clearErrorMsg();
}

void
CQChartsCreateAnnotationDlg::
ellipseCenterSlot(bool)
{
  if (ellipseWidgets_.centerRadio->isChecked()) {
    ellipseWidgets_.centerEdit->setEnabled(true);
    ellipseWidgets_.rxEdit    ->setEnabled(true);
    ellipseWidgets_.ryEdit    ->setEnabled(true);
    ellipseWidgets_.rectEdit  ->setEnabled(false);
  }
  else {
    ellipseWidgets_.centerEdit->setEnabled(false);
    ellipseWidgets_.rxEdit    ->setEnabled(false);
    ellipseWidgets_.ryEdit    ->setEnabled(false);
    ellipseWidgets_.rectEdit  ->setEnabled(true);
  }
}

void
CQChartsCreateAnnotationDlg::
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
CQChartsCreateAnnotationDlg::
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

//---

void
CQChartsCreateAnnotationDlg::
okSlot()
{
  if (applySlot())
    cancelSlot();
}

bool
CQChartsCreateAnnotationDlg::
applySlot()
{
  int ind = typeStack_->currentIndex();

  bool rc = false;

  int ind1 = 0;

  if (ind == ind1++) rc = createRectangleAnnotation();
  if (ind == ind1++) rc = createEllipseAnnotation  ();
  if (ind == ind1++) rc = createPolygonAnnotation  ();
  if (ind == ind1++) rc = createPolylineAnnotation ();
  if (ind == ind1++) rc = createTextAnnotation     ();
  if (ind == ind1++) rc = createImageAnnotation    ();
  if (ind == ind1++) rc = createPathAnnotation     ();
  if (ind == ind1++) rc = createArrowAnnotation    ();
  if (ind == ind1++) rc = createPointAnnotation    ();
  if (ind == ind1++) rc = createPieSliceAnnotation ();

  if (plot_) {
    if (ind == ind1++) rc = createAxisAnnotation();
  }

  if (ind == ind1++) rc = createKeyAnnotation     ();
  if (ind == ind1++) rc = createPointSetAnnotation();
  if (ind == ind1++) rc = createValueSetAnnotation();
  if (ind == ind1++) rc = createButtonAnnotation  ();

  return rc;
}

void
CQChartsCreateAnnotationDlg::
cancelSlot()
{
  close();
}

//---

bool
CQChartsCreateAnnotationDlg::
createRectangleAnnotation()
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

  CQChartsRectangleAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addRectangleAnnotation(rect);
  else if (plot())
    annotation = plot()->addRectangleAnnotation(rect);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createEllipseAnnotation()
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

  CQChartsEllipseAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addEllipseAnnotation(center, rx, ry);
  else if (plot())
    annotation = plot()->addEllipseAnnotation(center, rx, ry);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createPolygonAnnotation()
{
  CQChartsBoxData boxData;

  auto &shapeData = boxData.shape();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto polygon = polygonWidgets_.pointsEdit->polygon();

  if (polygon.units() == CQChartsUnits::NONE) {
    if      (view()) polygon.setUnits(CQChartsUnits::VIEW);
    else if (plot()) polygon.setUnits(CQChartsUnits::PLOT);
  }

  if (! polygon.isValid(/*closed*/true))
    return setErrorMsg("Not enough polygon points");

  auto fill   = polygonWidgets_.backgroundDataEdit->data();
  auto stroke = polygonWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  //---

  CQChartsPolygonAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addPolygonAnnotation(polygon);
  else if (plot())
    annotation = plot()->addPolygonAnnotation(polygon);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createPolylineAnnotation()
{
  CQChartsBoxData boxData;

  auto &shapeData = boxData.shape();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto polygon = polylineWidgets_.pointsEdit->polygon();

  if (polygon.units() == CQChartsUnits::NONE) {
    if      (view()) polygon.setUnits(CQChartsUnits::VIEW);
    else if (plot()) polygon.setUnits(CQChartsUnits::PLOT);
  }

  if (! polygon.isValid(/*closed*/false))
    return setErrorMsg("Not enough polyline points");

  auto fill   = polylineWidgets_.backgroundDataEdit->data();
  auto stroke = polylineWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  //---

  CQChartsPolylineAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addPolylineAnnotation(polygon);
  else if (plot())
    annotation = plot()->addPolylineAnnotation(polygon);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createTextAnnotation()
{
  CQChartsBoxData boxData;

  auto &shapeData = boxData.shape();

  //---

  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  bool isPos = textWidgets_.positionRadio->isChecked();
  auto pos   = textWidgets_.positionEdit->position();
  auto rect  = textWidgets_.rectEdit->rect();
  auto text  = textWidgets_.textEdit->text();

  if (! isPos) {
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

  CQChartsTextAnnotation *annotation = nullptr;

  if      (view()) {
    if (isPos)
      annotation = view()->addTextAnnotation(pos, text);
    else
      annotation = view()->addTextAnnotation(rect, text);
  }
  else if (plot()) {
    if (isPos)
      annotation = plot()->addTextAnnotation(pos, text);
    else
      annotation = plot()->addTextAnnotation(rect, text);
  }
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  annotation->setTextData(textData);
  annotation->setBoxData (boxData );

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createImageAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  bool isPos = imageWidgets_.positionRadio->isChecked();
  auto pos   = imageWidgets_.positionEdit->position();
  auto rect  = imageWidgets_.rectEdit->rect();

  if (! isPos) {
    if (! rect.isValid())
      return setErrorMsg("Invalid image rectangle");
  }

  auto imageText    = imageWidgets_.imageEdit->text();
  auto disImageText = imageWidgets_.disabledImageEdit->text();

  CQChartsImage image   (imageText);
  CQChartsImage disImage(disImageText);

  //---

  CQChartsImageAnnotation *annotation = nullptr;

  if      (view()) {
    if (isPos)
      annotation = view()->addImageAnnotation(pos, image);
    else
      annotation = view()->addImageAnnotation(rect, image);
  }
  else if (plot()) {
    if (isPos)
      annotation = plot()->addImageAnnotation(pos, image);
    else
      annotation = plot()->addImageAnnotation(rect, image);
  }
  else
    return false;

  if (disImage.isValid())
    annotation->setDisabledImage(disImage);

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createPathAnnotation()
{
  return false;
}

bool
CQChartsCreateAnnotationDlg::
createArrowAnnotation()
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

  CQChartsArrowAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addArrowAnnotation(start, end);
  else if (plot())
    annotation = plot()->addArrowAnnotation(start, end);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  annotation->setArrowData(arrowData);

  annotation->arrow()->setShapeData(shapeData);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createPointAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto pos        = pointWidgets_.positionEdit->position();
  auto symbolData = pointWidgets_.dataEdit->data();

  //---

  CQChartsPointAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addPointAnnotation(pos, symbolData.type());
  else if (plot())
    annotation = plot()->addPointAnnotation(pos, symbolData.type());
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  annotation->setSymbolData(symbolData);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createPieSliceAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto center      = pieSliceWidgets_.centerEdit->position();
  auto innerRadius = pieSliceWidgets_.innerRadiusEdit->length();
  auto outerRadius = pieSliceWidgets_.outerRadiusEdit->length();
  auto startAngle  = pieSliceWidgets_.startAngleEdit->angle();
  auto spanAngle   = pieSliceWidgets_.spanAngleEdit->angle();

  //---

  CQChartsPieSliceAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addPieSliceAnnotation(center, innerRadius, outerRadius,
                                               startAngle, spanAngle);
  else if (plot())
    annotation = plot()->addPieSliceAnnotation(center, innerRadius, outerRadius,
                                               startAngle, spanAngle);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createAxisAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  Qt::Orientation orient = Qt::Horizontal;

  if (axisWidgets_.orientationEdit->currentIndex() == 1)
    orient = Qt::Vertical;

  auto position = axisWidgets_.positionEdit->value();
  auto start    = axisWidgets_.startEdit->value();
  auto end      = axisWidgets_.endEdit->value();

  //---

  CQChartsAxisAnnotation *annotation = nullptr;

  if (plot())
    annotation = plot()->addAxisAnnotation(orient, start, end);
  else
    return false;

  annotation->setPosition(position);

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createKeyAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  //---

  CQChartsKeyAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addKeyAnnotation();
  else if (plot())
    annotation = plot()->addKeyAnnotation();
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createPointSetAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  //---

  CQChartsPointSetAnnotation *annotation = nullptr;

  CQChartsPoints points;

  if      (view())
    annotation = view()->addPointSetAnnotation(points);
  else if (plot())
    annotation = plot()->addPointSetAnnotation(points);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  return false;
}

bool
CQChartsCreateAnnotationDlg::
createValueSetAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  //---

  CQChartsValueSetAnnotation *annotation = nullptr;

  CQChartsRect rect(CQChartsUnits::VIEW, CQChartsGeom::BBox(10, 10, 90, 90));

  CQChartsReals reals;

  if      (view())
    annotation = view()->addValueSetAnnotation(rect, reals);
  else if (plot())
    annotation = plot()->addValueSetAnnotation(rect, reals);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  return false;
}

bool
CQChartsCreateAnnotationDlg::
createButtonAnnotation()
{
  auto id    = idEdit_ ->text();
  auto tipId = tipEdit_->text();

  auto pos  = buttonWidgets_.positionEdit->position();
  auto text = buttonWidgets_.textEdit->text();

  if (text.simplified().length() == 0)
    return setErrorMsg("Button text is empty");

  //---

  CQChartsButtonAnnotation *annotation = nullptr;

  if      (view())
    annotation = view()->addButtonAnnotation(pos, text);
  else if (plot())
    annotation = plot()->addButtonAnnotation(pos, text);
  else
    return false;

  //---

  if (id != "")
    annotation->setId(id);

  annotation->setTipId(tipId);

  return false;
}

//------

bool
CQChartsCreateAnnotationDlg::
setErrorMsg(const QString &msg)
{
  msgLabel_->setText   (msg);
  msgLabel_->setToolTip(msg);

  return false;
}

void
CQChartsCreateAnnotationDlg::
clearErrorMsg()
{
  msgLabel_->setText   ("");
  msgLabel_->setToolTip("");
}
