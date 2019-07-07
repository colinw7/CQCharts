#include <CQChartsCreateAnnotationDlg.h>
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
#include <QStackedWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
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
  setWindowTitle("Create rect Annotation");

  //---

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //----

  // type combo
  typeCombo_ = CQUtil::makeWidget<QComboBox>("type");

  typeCombo_->addItems(CQChartsAnnotation::typeNames());

  connect(typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSlot(int)));

  addLabelWidget(layout, "Type", typeCombo_);

  //----

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  layout->addLayout(gridLayout);

  int row = 0;

  //--

  // id, tip edits
  idEdit_  = CQUtil::makeWidget<CQLineEdit>("id");
  tipEdit_ = CQUtil::makeWidget<CQLineEdit>("tip");

  //--

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Id" , idEdit_ , row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Tip", tipEdit_, row);

  //----

  // per type widgets stack
  typeStack_ = CQUtil::makeWidget<QStackedWidget>("typeStack");

  layout->addWidget(typeStack_);

  createRectFrame    ();
  createEllipseFrame ();
  createPolygonFrame ();
  createPolyLineFrame();
  createTextFrame    ();
  createArrowFrame   ();
  createPointFrame   ();

  typeStack_->addWidget(rectWidgets_    .frame);
  typeStack_->addWidget(ellipseWidgets_ .frame);
  typeStack_->addWidget(polygonWidgets_ .frame);
  typeStack_->addWidget(polylineWidgets_.frame);
  typeStack_->addWidget(textWidgets_    .frame);
  typeStack_->addWidget(arrowWidgets_   .frame);
  typeStack_->addWidget(pointWidgets_   .frame);

  //---

  // OK, Apply, Cancel Buttons
  CQChartsDialogButtons *buttons = CQUtil::makeWidget<CQChartsDialogButtons>("buttons");

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  layout->addWidget(buttons);
}

void
CQChartsCreateAnnotationDlg::
createRectFrame()
{
  rectWidgets_.frame = CQUtil::makeWidget<QFrame>("rectFrame");

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(rectWidgets_.frame, 2, 2);

  //----

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // rect
  rectWidgets_.rectEdit = CQUtil::makeWidget<CQChartsRectEdit>("rectEdit");

  rectWidgets_.rectEdit->setRect(CQChartsRect());

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Rect", rectWidgets_.rectEdit, row);

  //--

  // margin
  rectWidgets_.marginEdit  = CQUtil::makeWidget<CQRealSpin>("marginEdit");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Margin", rectWidgets_.marginEdit, row);

  //--

  // padding
  rectWidgets_.paddingEdit = CQUtil::makeWidget<CQRealSpin>("paddingEdit");

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

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(ellipseWidgets_.frame, 2, 2);

  //---

  ellipseWidgets_.centerEdit = new CQChartsPositionEdit;
  ellipseWidgets_.rxEdit     = new CQChartsLengthEdit;
  ellipseWidgets_.ryEdit     = new CQChartsLengthEdit;

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

  addSidesWidget(ellipseWidgets_, frameLayout);

  //---

  frameLayout->addStretch(1);
}

void
CQChartsCreateAnnotationDlg::
createPolygonFrame()
{
  polygonWidgets_.frame = CQUtil::makeWidget<QFrame>("polygonFrame");

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polygonWidgets_.frame, 2, 2);

  //---

  polygonWidgets_.pointsEdit = new CQChartsPolygonEdit;

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

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(polylineWidgets_.frame, 2, 2);

  //---

  polylineWidgets_.pointsEdit = new CQChartsPolygonEdit;

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

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(textWidgets_.frame, 2, 2);

  //---

  QFrame *positionRectFrame = CQUtil::makeWidget<QFrame>("positionRectFrame");

  QHBoxLayout *positionRectLayout = CQUtil::makeLayout<QHBoxLayout>(positionRectFrame, 2, 2);

  textWidgets_.positionRadio = CQUtil::makeLabelWidget<QRadioButton>("Position", "position");
  textWidgets_.rectRadio     = CQUtil::makeLabelWidget<QRadioButton>("Rect", "rect");

  positionRectLayout->addWidget(textWidgets_.positionRadio);
  positionRectLayout->addWidget(textWidgets_.rectRadio);
  positionRectLayout->addStretch(1);

  connect(textWidgets_.positionRadio, SIGNAL(toggled(bool)), this, SLOT(textPositionSlot(bool)));

  textWidgets_.positionEdit = new CQChartsPositionEdit;
  textWidgets_.rectEdit     = new CQChartsRectEdit;

  textWidgets_.positionRadio->setChecked(true);

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

  //---

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

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
createArrowFrame()
{
  arrowWidgets_.frame = CQUtil::makeWidget<QFrame>("arrowFrame");

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(arrowWidgets_.frame, 2, 2);

  //---

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //--

  // start, end point
  arrowWidgets_.startEdit = new CQChartsPositionEdit;
  arrowWidgets_.endEdit   = new CQChartsPositionEdit;

  arrowWidgets_.startEdit->setPosition(CQChartsPosition(QPointF(0, 0)));
  arrowWidgets_.endEdit  ->setPosition(CQChartsPosition(QPointF(1, 1)));

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Start", arrowWidgets_.startEdit, row);
  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "End"  , arrowWidgets_.endEdit  , row);

  //---

  // arrow data edit
  arrowWidgets_.dataEdit = new CQChartsArrowDataEdit;

  arrowWidgets_.dataEdit->setPlot(plot());
  arrowWidgets_.dataEdit->setView(view());

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

  QVBoxLayout *frameLayout = CQUtil::makeLayout<QVBoxLayout>(pointWidgets_.frame, 2, 2);

  //---

  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  frameLayout->addLayout(gridLayout);

  int row = 0;

  //---

  pointWidgets_.positionEdit = new CQChartsPositionEdit;

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
addFillWidgets(Widgets &widgets, QBoxLayout *playout)
{
  widgets.backgroundDataEdit = new CQChartsFillDataEdit;

  widgets.backgroundDataEdit->setTitle("Fill");

  playout->addWidget(widgets.backgroundDataEdit);
}

void
CQChartsCreateAnnotationDlg::
addStrokeWidgets(Widgets &widgets, QBoxLayout *playout, bool cornerSize)
{
  widgets.strokeDataEdit =
    new CQChartsStrokeDataEdit(nullptr,
          CQChartsStrokeDataEditConfig().setCornerSize(cornerSize));

  widgets.strokeDataEdit->setTitle("Stroke");

  playout->addWidget(widgets.strokeDataEdit);
}

void
CQChartsCreateAnnotationDlg::
addSidesWidget(Widgets &widgets, QBoxLayout *playout)
{
  QGridLayout *gridLayout = CQUtil::makeLayout<QGridLayout>(2, 2);

  int row = 0;

  //--

  widgets.borderSidesEdit = CQUtil::makeWidget<CQChartsSidesEdit>("borderSidesEdit");

  CQChartsWidgetUtil::addGridLabelWidget(gridLayout, "Border Sides", widgets.borderSidesEdit, row);

  //--

  playout->addLayout(gridLayout);
}

QHBoxLayout *
CQChartsCreateAnnotationDlg::
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
CQChartsCreateAnnotationDlg::
typeSlot(int ind)
{
  if      (view_)
    setWindowTitle(QString("Create View %1 Annotation").
      arg(typeCombo_->currentText()));
  else if (plot_)
    setWindowTitle(QString("Create Plot %1 Annotation (%2)").
      arg(typeCombo_->currentText()).arg(plot_->id()));

  typeStack_->setCurrentIndex(ind);
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

  if      (ind == 0) rc = createRectangleAnnotation();
  else if (ind == 1) rc = createEllipseAnnotation();
  else if (ind == 2) rc = createPolygonAnnotation();
  else if (ind == 3) rc = createPolylineAnnotation();
  else if (ind == 4) rc = createTextAnnotation();
  else if (ind == 5) rc = createArrowAnnotation();
  else if (ind == 6) rc = createPointAnnotation();

  return rc;
}

bool
CQChartsCreateAnnotationDlg::
createRectangleAnnotation()
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

  CQChartsRectangleAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addRectangleAnnotation(rect);
  else if (plot_)
    annotation = plot_->addRectangleAnnotation(rect);
  else
    return false;

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

  CQChartsEllipseAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addEllipseAnnotation(center, rx, ry);
  else if (plot_)
    annotation = plot_->addEllipseAnnotation(center, rx, ry);
  else
    return false;

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

  CQChartsPolygonAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addPolygonAnnotation(polygon);
  else if (plot_)
    annotation = plot_->addPolygonAnnotation(polygon);
  else
    return false;

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

  CQChartsPolylineAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addPolylineAnnotation(polygon);
  else if (plot_)
    annotation = plot_->addPolylineAnnotation(polygon);
  else
    return false;

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

  CQChartsTextAnnotation *annotation = nullptr;

  if      (view_) {
    if (textWidgets_.positionRadio->isChecked())
      annotation = view_->addTextAnnotation(pos, text);
    else
      annotation = view_->addTextAnnotation(rect, text);
  }
  else if (plot_) {
    if (textWidgets_.positionRadio->isChecked())
      annotation = view_->addTextAnnotation(pos, text);
    else
      annotation = view_->addTextAnnotation(rect, text);
  }

  if (! annotation)
    return false;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setTextData(textData);
  annotation->setBoxData (boxData );

  return true;
}

bool
CQChartsCreateAnnotationDlg::
createArrowAnnotation()
{
  CQChartsShapeData shapeData;

  CQChartsStrokeData &stroke = shapeData.stroke();
  CQChartsFillData   &fill   = shapeData.fill();

  //---

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition start = arrowWidgets_.startEdit->position();
  CQChartsPosition end   = arrowWidgets_.endEdit  ->position();

  const CQChartsArrowData &arrowData = arrowWidgets_.dataEdit->data();

  stroke.setWidth(arrowWidgets_.strokeWidthEdit->length());
  stroke.setColor(arrowWidgets_.strokeColorEdit->color());

  fill.setVisible(arrowWidgets_.filledCheck->isChecked());
  fill.setColor  (arrowWidgets_.fillColorEdit->color());

  //---

  CQChartsArrowAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addArrowAnnotation(start, end);
  else if (plot_)
    annotation = plot_->addArrowAnnotation(start, end);
  else
    return false;

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
  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition          pos        = pointWidgets_.positionEdit->position();
  const CQChartsSymbolData &symbolData = pointWidgets_.dataEdit->data();

  //---

  CQChartsPointAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addPointAnnotation(pos, symbolData.type());
  else if (plot_)
    annotation = plot_->addPointAnnotation(pos, symbolData.type());
  else
    return false;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setSymbolData(symbolData);

  return true;
}

void
CQChartsCreateAnnotationDlg::
cancelSlot()
{
  close();
}
