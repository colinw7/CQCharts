#include <CQChartsCreateAnnotationDlg.h>

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
#include <CQChartsImageEdit.h>
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
CQChartsCreateAnnotationDlg(QWidget *parent, View *view) :
 QDialog(parent), view_(view)
{
  initWidgets();
}

CQChartsCreateAnnotationDlg::
CQChartsCreateAnnotationDlg(QWidget *parent, Plot *plot) :
 QDialog(parent), plot_(plot)
{
  initWidgets();
}

CQChartsView *
CQChartsCreateAnnotationDlg::
view() const
{
  return view_.data();
}

CQChartsPlot *
CQChartsCreateAnnotationDlg::
plot() const
{
  return plot_.data();
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

  auto gridData = createGrid(layout);

  //--

  // id, tip edits
  idEdit_  = addLineEdit(gridData, "id" , "Id" , "Annotation Id");
  tipEdit_ = addLineEdit(gridData, "tip", "Tip", "Annotation Tooltip");

  //----

  // per type widgets stack
  typeStack_ = CQUtil::makeWidget<QStackedWidget>("typeStack");

  layout->addWidget(typeStack_);

  for (const auto &typeName : typeNames()) {
    auto *frame = createNamedFrame(typeName);

    typeStack_->addWidget(frame);
  }

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
            "Path" << "Arrow" << "Arc" << "Point" << "Pie Slice";

    if (plot_)
      strs << "Axis";

    strs << "Key" << "Point Set" << "Value Set" << "Button";
  }

  return strs;
}

//------

QFrame *
CQChartsCreateAnnotationDlg::
createNamedFrame(const QString &typeName)
{
  if      (typeName == "Rectangle"     ) return createRectFrame        ();
  else if (typeName == "Ellipse"       ) return createEllipseFrame     ();
  else if (typeName == "Polygon"       ) return createPolygonFrame     ();
  else if (typeName == "Polyline"      ) return createPolyLineFrame    ();
  else if (typeName == "Text"          ) return createTextFrame        ();
  else if (typeName == "Image"         ) return createImageFrame       ();
  else if (typeName == "Path"          ) return createPathFrame        ();
  else if (typeName == "Arrow"         ) return createArrowFrame       ();
  else if (typeName == "Arc"           ) return createArcFrame         ();
  else if (typeName == "Point"         ) return createPointFrame       ();
  else if (typeName == "Pie Slice"     ) return createPieSliceFrame    ();
  else if (typeName == "Axis"          ) return createAxisFrame        ();
  else if (typeName == "Key"           ) return createKeyFrame         ();
  else if (typeName == "Point Set"     ) return createPointSetFrame    ();
  else if (typeName == "Value Set"     ) return createValueSetFrame    ();
  else if (typeName == "Button"        ) return createButtonFrame      ();
//else if (typeName == "Widget"        ) return createWidgetFrame      ();
//else if (typeName == "Symbol Map Key") return createSymbolMapKeyFrame();
  else { assert(false); return nullptr; }
}

QFrame *
CQChartsCreateAnnotationDlg::
createRectFrame()
{
  rectWidgets_.frame = CQUtil::makeWidget<QFrame>("rectFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(rectWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //--

  // rect
  rectWidgets_.rectEdit = addRectEdit(gridData, "rectEdit", "Rect", "Rectangle");

  //--

  // outer margin
  rectWidgets_.marginEdit = addMarginEdit(gridData, "marginEdit", "Margin", "Rectangle Margin");

  // inner padding
  rectWidgets_.paddingEdit = addMarginEdit(gridData, "paddingEdit", "Padding", "Rectangle Padding");

  //---

  addFillWidgets  (rectWidgets_, frameLayout);
  addStrokeWidgets(rectWidgets_, frameLayout, /*cornerSize*/true);

  addSidesWidget(rectWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);

  return rectWidgets_.frame;
}

QFrame *
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

  ellipseWidgets_.centerRadio->setChecked(true);

  frameLayout->addWidget(centerRectFrame);

  connect(ellipseWidgets_.centerRadio, SIGNAL(toggled(bool)), this, SLOT(ellipseCenterSlot(bool)));

  //---

  auto gridData = createGrid(frameLayout);

  //---

  ellipseWidgets_.centerEdit =
    addPositionEdit(gridData, "centerEdit", "Center", 0.0, 0.0, "Ellipse Center Position");

  ellipseWidgets_.rxEdit =
    addLengthEdit(gridData, "rxEdit", "Radius X", 1.0, "Ellipse X Radius Length");
  ellipseWidgets_.ryEdit =
    addLengthEdit(gridData, "ryEdit", "Radius Y", 1.0, "Ellipse Y Radius Length");

  ellipseWidgets_.rectEdit = addRectEdit(gridData, "rectEdit", "Rect", "Ellipse Rectangle");

  ellipseCenterSlot(true);

  //---

  addFillWidgets  (ellipseWidgets_, frameLayout);
  addStrokeWidgets(ellipseWidgets_, frameLayout, /*cornerSize*/true);

  addSidesWidget(ellipseWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);

  return ellipseWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createPolygonFrame()
{
  polygonWidgets_.frame = CQUtil::makeWidget<QFrame>("polygonFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polygonWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //---

  polygonWidgets_.pointsEdit = addPolygonEdit(gridData, "pointsEdit", "Points", "Polygon Points");

  //---

  addFillWidgets  (polygonWidgets_, frameLayout);
  addStrokeWidgets(polygonWidgets_, frameLayout, /*cornerSize*/false);

  //---

  frameLayout->addStretch(1);

  return polygonWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createPolyLineFrame()
{
  polylineWidgets_.frame = CQUtil::makeWidget<QFrame>("polylineFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polylineWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //---

  polylineWidgets_.pointsEdit = addPolygonEdit(gridData, "pointsEdit", "Points", "Polyline Points");

  //---

  addFillWidgets  (polylineWidgets_, frameLayout);
  addStrokeWidgets(polylineWidgets_, frameLayout, /*cornerSize*/false);

  //---

  frameLayout->addStretch(1);

  return polylineWidgets_.frame;
}

QFrame *
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

  textWidgets_.positionRadio->setChecked(true);

  frameLayout->addWidget(positionRectFrame);

  connect(textWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(textPositionSlot(bool)));

  //---

  auto gridData1 = createGrid(frameLayout);

  //---

  textWidgets_.positionEdit =
    addPositionEdit(gridData1, "positionEdit", "Position", 0.0, 0.0, "Text Position");
  textWidgets_.rectEdit =
    addRectEdit(gridData1, "rectEdit", "Rect", "Text Rectangle");

  //--

  auto gridData = createGrid(frameLayout);

  //--

  textWidgets_.textEdit = addLineEdit(gridData, "edit", "Text", "Text String");

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

  textPositionSlot(true);

  return textWidgets_.frame;
}

QFrame *
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

  imageWidgets_.positionRadio->setChecked(true);

  frameLayout->addWidget(positionRectFrame);

  connect(imageWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(imagePositionSlot(bool)));

  //---

  auto gridData1 = createGrid(frameLayout);

  //---

  imageWidgets_.positionEdit =
    addPositionEdit(gridData1, "positionEdit", "Position", 0.0, 0.0, "Image Position");
  imageWidgets_.rectEdit =
    addRectEdit(gridData1, "rectEdit", "Rect", "Image Rectangle");

  //---

  auto gridData = createGrid(frameLayout);

  //--

  imageWidgets_.imageEdit =
    addImageEdit(gridData, "imageEdit", "Image", "Image");
  imageWidgets_.disabledImageEdit =
    addImageEdit(gridData, "disabledImageEdit", "Disabled Image", "Disabled Image");

  //--

  frameLayout->addStretch(1);

  imagePositionSlot(true);

  return imageWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createPathFrame()
{
  pathWidgets_.frame = CQUtil::makeWidget<QFrame>("pathFrame");

  return pathWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createArrowFrame()
{
  arrowWidgets_.frame = CQUtil::makeWidget<QFrame>("arrowFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(arrowWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //--

  // start, end point
  arrowWidgets_.startEdit =
    addPositionEdit(gridData, "startEdit", "Start", 0.0, 0.0, "Arrow Start Point");
  arrowWidgets_.endEdit =
    addPositionEdit(gridData, "endEdit", "End", 1.0, 1.0, "Arrow End Point");

  //---

  // arrow data edit
  arrowWidgets_.dataEdit = CQUtil::makeWidget<CQChartsArrowDataEdit>("dataEdit");

  arrowWidgets_.dataEdit->setPlot(plot());
  arrowWidgets_.dataEdit->setView(view());

  auto len = CQChartsViewPlotObj::makeLength(view(), plot(), 1);

  arrowWidgets_.dataEdit->lengthEdit()->setLength(len);

  frameLayout->addWidget(arrowWidgets_.dataEdit);

  //---

  auto gridData1 = createGrid(frameLayout);

  //--

  // stroke width, stroke color, filled and fill color
  arrowWidgets_.strokeWidthEdit =
    addLengthEdit(gridData1, "strokeWidthEdit", "Stroke Width", 0.0, "Arrow Stroke Width");
  arrowWidgets_.strokeColorEdit =
    addColorEdit(gridData1, "strokeColorEdit", "Stroke Color", "Arrow Stroke Color");

  arrowWidgets_.filledCheck =
    addBoolEdit(gridData1, "filledCheck", "Filled", "Arrow Is Filled");

  arrowWidgets_.fillColorEdit =
    addColorEdit(gridData1, "fillColorEdit", "Fill Color", "Arrow Fill Color");

  //---

  frameLayout->addStretch(1);

  return arrowWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createArcFrame()
{
  arcWidgets_.frame = CQUtil::makeWidget<QFrame>("arcFrame");

  return arcWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createPointFrame()
{
  pointWidgets_.frame = CQUtil::makeWidget<QFrame>("pointFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(pointWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //---

  // position
  pointWidgets_.positionEdit =
    addPositionEdit(gridData, "positionEdit", "Position", 0.0, 0.0, "Point Position");

  //---

  pointWidgets_.dataEdit = new CQChartsSymbolDataEdit(nullptr, /*optional*/false);

  pointWidgets_.dataEdit->setPlot(plot());
  pointWidgets_.dataEdit->setView(view());

  frameLayout->addWidget(pointWidgets_.dataEdit);

  //---

  frameLayout->addStretch(1);

  return pointWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createPieSliceFrame()
{
  pieSliceWidgets_.frame = CQUtil::makeWidget<QFrame>("pieSliceFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(pieSliceWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //--

  // center
  pieSliceWidgets_.centerEdit =
    addPositionEdit(gridData, "center", "Center", 0.0, 0.0, "Center Point");

  //--

  // radii
  pieSliceWidgets_.innerRadiusEdit =
    addLengthEdit(gridData, "innerRadius", "Inner Radius", 0.0, "Inner Radius");
  pieSliceWidgets_.outerRadiusEdit =
    addLengthEdit(gridData, "outerRadius", "Outer Radius", 1.0, "Outer Radius");

  //---

  // angles
  pieSliceWidgets_.startAngleEdit =
    addAngleEdit(gridData, "startAngle", "Start Angle", 0.0, "Start Angle");
  pieSliceWidgets_.spanAngleEdit =
    addAngleEdit(gridData, "spanAngle" , "Span Angle", 360.0, "Span Angle");

  //---

  frameLayout->addStretch(1);

  return pieSliceWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createAxisFrame()
{
  assert(plot_);

  axisWidgets_.frame = CQUtil::makeWidget<QFrame>("axisFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(axisWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //--

  auto orientNames = QStringList() << "Horizontal" << "Vertical";

  axisWidgets_.orientationEdit =
    addComboEdit(gridData, "orientation", "Orientation", orientNames, "Orientation");

  axisWidgets_.positionEdit =
    addRealEdit(gridData, "position", "Position", 0.0, "Position");

  axisWidgets_.startEdit =
    addRealEdit(gridData, "start", "Start", 0.0, "Start Value");
  axisWidgets_.endEdit =
    addRealEdit(gridData, "end"  , "End"  , 1.0, "End Value");

  //---

  frameLayout->addStretch(1);

  return axisWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createKeyFrame()
{
  keyWidgets_.frame = CQUtil::makeWidget<QFrame>("keyFrame");

  return keyWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createPointSetFrame()
{
  pointSetWidgets_.frame = CQUtil::makeWidget<QFrame>("pointSetFrame");

  return pointSetWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createValueSetFrame()
{
  valueSetWidgets_.frame = CQUtil::makeWidget<QFrame>("valueSetFrame");

  return valueSetWidgets_.frame;
}

QFrame *
CQChartsCreateAnnotationDlg::
createButtonFrame()
{
  buttonWidgets_.frame = CQUtil::makeWidget<QFrame>("buttonFrame");

  auto *frameLayout = CQUtil::makeLayout<QVBoxLayout>(buttonWidgets_.frame, 2, 2);

  //---

  auto gridData = createGrid(frameLayout);

  //--

  buttonWidgets_.positionEdit =
    addPositionEdit(gridData, "positionEdit", "Position", 0.0, 0.0, "Button Position");

  buttonWidgets_.textEdit =
    addLineEdit(gridData, "edit", "Text", "Text String");

  //---

  frameLayout->addStretch(1);

  return buttonWidgets_.frame;
}

//------

void
CQChartsCreateAnnotationDlg::
addFillWidgets(Widgets &widgets, QBoxLayout *playout)
{
  widgets.backgroundDataEdit = CQUtil::makeWidget<CQChartsFillDataEdit>("backgroundDataEdit");

  widgets.backgroundDataEdit->setTitle("Fill");
  widgets.backgroundDataEdit->setPlot(plot());
  widgets.backgroundDataEdit->setView(view());
  widgets.backgroundDataEdit->setToolTip("Enable Fill");

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
  auto gridData = createGrid(playout);

  //---

  widgets.borderSidesEdit = CQUtil::makeWidget<CQChartsSidesEdit>("borderSidesEdit");

  widgets.borderSidesEdit->setSides(CQChartsSides(CQChartsSides::Side::ALL));

  widgets.borderSidesEdit->setToolTip("Border Sides");

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, "Border Sides",
                                         widgets.borderSidesEdit, gridData.row);
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

CQCheckBox *
CQChartsCreateAnnotationDlg::
addBoolEdit(GridData &gridData, const QString &name, const QString &label,
            const QString &tip) const
{
  auto *check = CQUtil::makeWidget<CQCheckBox>(name);

  if (tip != "")
    check->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, check, gridData.row);

  return check;
}

CQChartsLineEdit *
CQChartsCreateAnnotationDlg::
addLineEdit(GridData &gridData, const QString &name, const QString &label,
            const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(name);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQRealSpin *
CQChartsCreateAnnotationDlg::
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
CQChartsCreateAnnotationDlg::
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
CQChartsCreateAnnotationDlg::
addPositionEdit(GridData &gridData, const QString &name, const QString &label,
                double x, double y, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsPositionEdit>(name);

  edit->setPlot(plot());

  auto pos = CQChartsViewPlotObj::makePosition(view(), plot(), x, y);

  edit->setPosition(pos);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsLengthEdit *
CQChartsCreateAnnotationDlg::
addLengthEdit(GridData &gridData, const QString &name, const QString &label,
              double l, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsLengthEdit>(name);

  auto len = CQChartsViewPlotObj::makeLength(view(), plot(), l);

  edit->setLength(len);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsRectEdit *
CQChartsCreateAnnotationDlg::
addRectEdit(GridData &gridData, const QString &name, const QString &label,
            const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsRectEdit>(name);

  edit->setPlot(plot());

  auto rect = CQChartsViewPlotObj::makeRect(view(), plot(), 0, 0, 1, 1);

  edit->setRect(rect);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsPolygonEdit *
CQChartsCreateAnnotationDlg::
addPolygonEdit(GridData &gridData, const QString &name, const QString &label,
               const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsPolygonEdit>(name);

  edit->setPlot(plot());

  if      (plot()) edit->setUnits(Units::PLOT);
  else if (view()) edit->setUnits(Units::VIEW);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsMarginEdit *
CQChartsCreateAnnotationDlg::
addMarginEdit(GridData &gridData, const QString &name, const QString &label,
              const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsMarginEdit>(name);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsImageEdit *
CQChartsCreateAnnotationDlg::
addImageEdit(GridData &gridData, const QString &name, const QString &label,
             const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsImageEdit>(name);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsColorLineEdit *
CQChartsCreateAnnotationDlg::
addColorEdit(GridData &gridData, const QString &name, const QString &label,
             const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsColorLineEdit>(name);

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

CQChartsAngleEdit *
CQChartsCreateAnnotationDlg::
addAngleEdit(GridData &gridData, const QString &name, const QString &label,
             double a, const QString &tip) const
{
  auto *edit = CQUtil::makeWidget<CQChartsAngleEdit>(name);

  edit->setAngle(CQChartsAngle(a));

  if (tip != "")
    edit->setToolTip(tip);

  CQChartsWidgetUtil::addGridLabelWidget(gridData.layout, label, edit, gridData.row);

  return edit;
}

//---

CQChartsCreateAnnotationDlg::GridData
CQChartsCreateAnnotationDlg::
createGrid(QBoxLayout *layout) const
{
  GridData gridData;

  gridData.layout = CQUtil::makeLayout<QGridLayout>(2, 2);

  layout->addLayout(gridData.layout);

  return gridData;
}

//---

void
CQChartsCreateAnnotationDlg::
typeSlot(int ind)
{
  if      (plot())
    setWindowTitle(QString("Create Plot %1 Annotation (%2)").
      arg(typeCombo_->currentText()).arg(plot()->id()));
  else if (view())
    setWindowTitle(QString("Create View %1 Annotation").
      arg(typeCombo_->currentText()));

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
  if (ind == ind1++) rc = createArcAnnotation      ();
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

  if      (plot())
    annotation = plot()->addRectangleAnnotation(rect);
  else if (view())
    annotation = view()->addRectangleAnnotation(rect);
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

  if      (plot())
    annotation = plot()->addEllipseAnnotation(center, rx, ry);
  else if (view())
    annotation = view()->addEllipseAnnotation(center, rx, ry);
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

  if (polygon.units() == Units::NONE) {
    if      (plot()) polygon.setUnits(Units::PLOT);
    else if (view()) polygon.setUnits(Units::VIEW);
  }

  if (! polygon.isValid(/*closed*/true))
    return setErrorMsg("Not enough polygon points");

  auto fill   = polygonWidgets_.backgroundDataEdit->data();
  auto stroke = polygonWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  //---

  CQChartsPolygonAnnotation *annotation = nullptr;

  if      (plot())
    annotation = plot()->addPolygonAnnotation(polygon);
  else if (view())
    annotation = view()->addPolygonAnnotation(polygon);
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

  if (polygon.units() == Units::NONE) {
    if      (plot()) polygon.setUnits(Units::PLOT);
    else if (view()) polygon.setUnits(Units::VIEW);
  }

  if (! polygon.isValid(/*closed*/false))
    return setErrorMsg("Not enough polyline points");

  auto fill   = polylineWidgets_.backgroundDataEdit->data();
  auto stroke = polylineWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  //---

  CQChartsPolylineAnnotation *annotation = nullptr;

  if      (plot())
    annotation = plot()->addPolylineAnnotation(polygon);
  else if (view())
    annotation = view()->addPolylineAnnotation(polygon);
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

  if (text.trimmed().length() == 0)
    return setErrorMsg("Text string is empty");

  auto textData = textWidgets_.dataEdit->data();

  auto fill   = textWidgets_.backgroundDataEdit->data();
  auto stroke = textWidgets_.strokeDataEdit    ->data();

  shapeData.setFill  (fill);
  shapeData.setStroke(stroke);

  boxData.setBorderSides(textWidgets_.borderSidesEdit->sides());

  //---

  CQChartsTextAnnotation *annotation = nullptr;

  if      (plot()) {
    if (isPos)
      annotation = plot()->addTextAnnotation(pos, text);
    else
      annotation = plot()->addTextAnnotation(rect, text);
  }
  else if (view()) {
    if (isPos)
      annotation = view()->addTextAnnotation(pos, text);
    else
      annotation = view()->addTextAnnotation(rect, text);
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

  auto image    = imageWidgets_.imageEdit->image();
  auto disImage = imageWidgets_.disabledImageEdit->image();

  //---

  CQChartsImageAnnotation *annotation = nullptr;

  if      (plot()) {
    if (isPos)
      annotation = plot()->addImageAnnotation(pos, image);
    else
      annotation = plot()->addImageAnnotation(rect, image);
  }
  else if (view()) {
    if (isPos)
      annotation = view()->addImageAnnotation(pos, image);
    else
      annotation = view()->addImageAnnotation(rect, image);
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

  if      (plot())
    annotation = plot()->addArrowAnnotation(start, end);
  else if (view())
    annotation = view()->addArrowAnnotation(start, end);
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
createArcAnnotation()
{
  return false;
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

  if      (plot())
    annotation = plot()->addPointAnnotation(pos, symbolData.symbol());
  else if (view())
    annotation = view()->addPointAnnotation(pos, symbolData.symbol());
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

  if      (plot())
    annotation = plot()->addPieSliceAnnotation(center, innerRadius, outerRadius,
                                               startAngle, spanAngle);
  else if (view())
    annotation = view()->addPieSliceAnnotation(center, innerRadius, outerRadius,
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

  if      (plot())
    annotation = plot()->addKeyAnnotation();
  else if (view())
    annotation = view()->addKeyAnnotation();
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

  auto rect = CQChartsRect::view(CQChartsGeom::BBox(10, 10, 90, 90));

  CQChartsPoints points;

  if      (plot())
    annotation = plot()->addPointSetAnnotation(rect, points);
  else if (view())
    annotation = view()->addPointSetAnnotation(rect, points);
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

  auto rect = CQChartsRect::view(CQChartsGeom::BBox(10, 10, 90, 90));

  CQChartsReals reals;

  if      (plot())
    annotation = plot()->addValueSetAnnotation(rect, reals);
  else if (view())
    annotation = view()->addValueSetAnnotation(rect, reals);
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

  if (text.trimmed().length() == 0)
    return setErrorMsg("Button text is empty");

  //---

  CQChartsButtonAnnotation *annotation = nullptr;

  if      (plot())
    annotation = plot()->addButtonAnnotation(pos, text);
  else if (view())
    annotation = view()->addButtonAnnotation(pos, text);
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
