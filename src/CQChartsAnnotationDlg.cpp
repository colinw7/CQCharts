#include <CQChartsAnnotationDlg.h>
#include <CQChartsAnnotation.h>
#include <CQChartsArrow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsLengthEdit.h>

#include <CQPoint2DEdit.h>
#include <CQAngleSpinBox.h>
#include <CQFontChooser.h>
#include <CQColorChooser.h>
#include <CQAlignEdit.h>
#include <CQLineDash.h>
#include <CQRealSpin.h>
#include <CQCheckBox.h>

#include <QFrame>
#include <QGroupBox>
#include <QStackedWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class CQChartsAlphaEdit : public CQRealSpin {
 public:
  CQChartsAlphaEdit() {
    setRange(0.0, 1.0);
    setValue(1.0);
  }
};

//---

CQChartsAnnotationDlg::
CQChartsAnnotationDlg(CQChartsView *view) :
 QDialog(), view_(view)
{
  setWindowTitle("Create Annotation");

  initWidgets();
}

CQChartsAnnotationDlg::
CQChartsAnnotationDlg(CQChartsPlot *plot) :
 QDialog(), plot_(plot)
{
  setWindowTitle("Create Annotation");

  initWidgets();
}

void
CQChartsAnnotationDlg::
initWidgets()
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  //----

  QComboBox *typeCombo = new QComboBox;

  typeCombo->addItems(QStringList() <<
    "rect" << "ellipse" << "polygon" << "polyline" << "text" << "arrow" << "point");

  connect(typeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSlot(int)));

  addLabelWidget(layout, "Type", typeCombo);

  //----

  idEdit_ = new QLineEdit;

  addLabelWidget(layout, "Id", idEdit_);

  //---

  tipEdit_ = new QLineEdit;

  addLabelWidget(layout, "Tip", tipEdit_);

  //---

  typeStack_ = new QStackedWidget;

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

  QFrame *buttonFrame = new QFrame;

  QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);

  QPushButton *okButton     = new QPushButton("OK"    );
  QPushButton *applyButton  = new QPushButton("Apply" );
  QPushButton *cancelButton = new QPushButton("Cancel");

  buttonLayout->addStretch(1);
  buttonLayout->addWidget (okButton);
  buttonLayout->addWidget (applyButton);
  buttonLayout->addWidget (cancelButton);

  connect(okButton    , SIGNAL(clicked()), this, SLOT(okSlot()));
  connect(applyButton , SIGNAL(clicked()), this, SLOT(applySlot()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));

  layout->addWidget(buttonFrame);
}

void
CQChartsAnnotationDlg::
createRectFrame()
{
  rectWidgets_.frame = new QFrame;

  QVBoxLayout *frameLayout = new QVBoxLayout(rectWidgets_.frame);

  rectWidgets_.startEdit = new CQChartsPositionEdit;
  rectWidgets_.endEdit   = new CQChartsPositionEdit;

  rectWidgets_.startEdit->setPosition(CQChartsPosition(QPointF(0, 0)));
  rectWidgets_.endEdit  ->setPosition(CQChartsPosition(QPointF(1, 1)));

  rectWidgets_.marginEdit  = new CQRealSpin;
  rectWidgets_.paddingEdit = new CQRealSpin;

  addLabelWidget(frameLayout, "Start"  , rectWidgets_.startEdit  );
  addLabelWidget(frameLayout, "End"    , rectWidgets_.endEdit    );
  addLabelWidget(frameLayout, "Margin" , rectWidgets_.marginEdit );
  addLabelWidget(frameLayout, "Padding", rectWidgets_.paddingEdit);

  addFillWidgets  (rectWidgets_, frameLayout);
  addStrokeWidgets(rectWidgets_, frameLayout);

  rectWidgets_.cornerSizeEdit  = new CQChartsLengthEdit;
  rectWidgets_.borderSidesEdit = new QLineEdit;

  addLabelWidget(frameLayout, "Corner Size" , rectWidgets_.cornerSizeEdit );
  addLabelWidget(frameLayout, "Border Sides", rectWidgets_.borderSidesEdit);

  frameLayout->addStretch(1);
}

void
CQChartsAnnotationDlg::
createEllipseFrame()
{
  ellipseWidgets_.frame = new QFrame;

  QVBoxLayout *frameLayout = new QVBoxLayout(ellipseWidgets_.frame);

  ellipseWidgets_.centerEdit = new CQChartsPositionEdit;
  ellipseWidgets_.rxEdit     = new CQChartsLengthEdit;
  ellipseWidgets_.ryEdit     = new CQChartsLengthEdit;

  addLabelWidget(frameLayout, "Center"  , ellipseWidgets_.centerEdit);
  addLabelWidget(frameLayout, "Radius X", ellipseWidgets_.rxEdit    );
  addLabelWidget(frameLayout, "Radius Y", ellipseWidgets_.ryEdit    );

  addFillWidgets  (ellipseWidgets_, frameLayout);
  addStrokeWidgets(ellipseWidgets_, frameLayout);

  ellipseWidgets_.cornerSizeEdit  = new CQChartsLengthEdit;
  ellipseWidgets_.borderSidesEdit = new QLineEdit;

  addLabelWidget(frameLayout, "Corner Size" , ellipseWidgets_.cornerSizeEdit );
  addLabelWidget(frameLayout, "Border Sides", ellipseWidgets_.borderSidesEdit);

  frameLayout->addStretch(1);
}

void
CQChartsAnnotationDlg::
createPolygonFrame()
{
  polygonWidgets_.frame = new QFrame;

  QVBoxLayout *frameLayout = new QVBoxLayout(polygonWidgets_.frame);

  polygonWidgets_.pointsEdit = new QLineEdit;

  addLabelWidget(frameLayout, "Points", polygonWidgets_.pointsEdit);

  addFillWidgets  (polygonWidgets_, frameLayout);
  addStrokeWidgets(polygonWidgets_, frameLayout);

  frameLayout->addStretch(1);
}

void
CQChartsAnnotationDlg::
createPolyLineFrame()
{
  polylineWidgets_.frame = new QFrame;

  QVBoxLayout *frameLayout = new QVBoxLayout(polylineWidgets_.frame);

  polylineWidgets_.pointsEdit = new QLineEdit;

  addLabelWidget(frameLayout, "Points", polylineWidgets_.pointsEdit);

  addFillWidgets  (polylineWidgets_, frameLayout);
  addStrokeWidgets(polylineWidgets_, frameLayout);

  frameLayout->addStretch(1);
}

void
CQChartsAnnotationDlg::
createTextFrame()
{
  textWidgets_.frame = new QFrame;

  QVBoxLayout *frameLayout = new QVBoxLayout(textWidgets_.frame);

  textWidgets_.positionEdit  = new CQChartsPositionEdit;
  textWidgets_.textEdit      = new QLineEdit;
  textWidgets_.fontEdit      = new CQFontChooser;
  textWidgets_.colorEdit     = new CQColorChooser;
  textWidgets_.alphaEdit     = new CQChartsAlphaEdit;
  textWidgets_.angleEdit     = new CQAngleSpinBox;
  textWidgets_.contrastCheck = new CQCheckBox;
  textWidgets_.alignEdit     = new CQAlignEdit;
  textWidgets_.htmlCheck     = new CQCheckBox;

  textWidgets_.alphaEdit->setRange(0.0, 1.0);
  textWidgets_.alphaEdit->setValue(1.0);

  addLabelWidget(frameLayout, "Position", textWidgets_.positionEdit);
  addLabelWidget(frameLayout, "Text"    , textWidgets_.textEdit);
  addLabelWidget(frameLayout, "Font"    , textWidgets_.fontEdit);
  addLabelWidget(frameLayout, "Color"   , textWidgets_.colorEdit);
  addLabelWidget(frameLayout, "Alpha"   , textWidgets_.alphaEdit);
  addLabelWidget(frameLayout, "Angle"   , textWidgets_.angleEdit);
  addLabelWidget(frameLayout, "Contrast", textWidgets_.contrastCheck);
  addLabelWidget(frameLayout, "Align"   , textWidgets_.alignEdit);
  addLabelWidget(frameLayout, "HTML"    , textWidgets_.htmlCheck);

  addFillWidgets  (textWidgets_, frameLayout);
  addStrokeWidgets(textWidgets_, frameLayout);

  textWidgets_.cornerSizeEdit  = new CQChartsLengthEdit;
  textWidgets_.borderSidesEdit = new QLineEdit;

  addLabelWidget(frameLayout, "Corner Size" , textWidgets_.cornerSizeEdit );
  addLabelWidget(frameLayout, "Border Sides", textWidgets_.borderSidesEdit);

  frameLayout->addStretch(1);
}

void
CQChartsAnnotationDlg::
createArrowFrame()
{
  arrowWidgets_.frame = new QFrame;

  QVBoxLayout *frameLayout = new QVBoxLayout(arrowWidgets_.frame);

  arrowWidgets_.startEdit = new CQChartsPositionEdit;
  arrowWidgets_.endEdit   = new CQChartsPositionEdit;

  arrowWidgets_.startEdit->setPosition(CQChartsPosition(QPointF(0, 0)));
  arrowWidgets_.endEdit  ->setPosition(CQChartsPosition(QPointF(1, 1)));

  addLabelWidget(frameLayout, "Start", arrowWidgets_.startEdit);
  addLabelWidget(frameLayout, "End"  , arrowWidgets_.endEdit  );

  arrowWidgets_.lengthEdit      = new CQChartsLengthEdit;
  arrowWidgets_.angleEdit       = new CQAngleSpinBox;
  arrowWidgets_.backAngleEdit   = new CQAngleSpinBox;
  arrowWidgets_.fheadCheck      = new CQCheckBox;
  arrowWidgets_.theadCheck      = new CQCheckBox;
  arrowWidgets_.emptyCheck      = new CQCheckBox;
  arrowWidgets_.lineWidthEdit   = new CQChartsLengthEdit;
  arrowWidgets_.strokeColorEdit = new CQColorChooser;
  arrowWidgets_.filledCheck     = new CQCheckBox;
  arrowWidgets_.fillColorEdit   = new CQColorChooser;

  addLabelWidget(frameLayout, "Length"      , arrowWidgets_.lengthEdit);
  addLabelWidget(frameLayout, "Angle"       , arrowWidgets_.angleEdit);
  addLabelWidget(frameLayout, "Back Angle"  , arrowWidgets_.backAngleEdit);
  addLabelWidget(frameLayout, "Front Head"  , arrowWidgets_.fheadCheck);
  addLabelWidget(frameLayout, "Tail Head"   , arrowWidgets_.theadCheck);
  addLabelWidget(frameLayout, "Empty"       , arrowWidgets_.emptyCheck);
  addLabelWidget(frameLayout, "Line Width"  , arrowWidgets_.lineWidthEdit);
  addLabelWidget(frameLayout, "Stroke Color", arrowWidgets_.strokeColorEdit);
  addLabelWidget(frameLayout, "Filled"      , arrowWidgets_.filledCheck);
  addLabelWidget(frameLayout, "Fill Color"  , arrowWidgets_.fillColorEdit);

  frameLayout->addStretch(1);
}

void
CQChartsAnnotationDlg::
createPointFrame()
{
  pointWidgets_.frame = new QFrame;

  QVBoxLayout *frameLayout = new QVBoxLayout(pointWidgets_.frame);

  pointWidgets_.positionEdit  = new CQChartsPositionEdit;
  pointWidgets_.sizeEdit      = new CQChartsLengthEdit;
  pointWidgets_.typeEdit      = new QLineEdit;
  pointWidgets_.strokedCheck  = new CQCheckBox;
  pointWidgets_.filledCheck   = new CQCheckBox;
  pointWidgets_.lineWidthEdit = new CQChartsLengthEdit;
  pointWidgets_.lineColorEdit = new CQColorChooser;
  pointWidgets_.lineAlphaEdit = new CQChartsAlphaEdit;
  pointWidgets_.fillColorEdit = new CQColorChooser;
  pointWidgets_.fillAlphaEdit = new CQChartsAlphaEdit;

  pointWidgets_.lineAlphaEdit->setRange(0.0, 1.0);
  pointWidgets_.lineAlphaEdit->setValue(1.0);

  pointWidgets_.fillAlphaEdit->setRange(0.0, 1.0);
  pointWidgets_.fillAlphaEdit->setValue(1.0);

  addLabelWidget(frameLayout, "Position"  , pointWidgets_.positionEdit);
  addLabelWidget(frameLayout, "Size"      , pointWidgets_.sizeEdit);
  addLabelWidget(frameLayout, "Type"      , pointWidgets_.typeEdit);
  addLabelWidget(frameLayout, "Stroked"   , pointWidgets_.strokedCheck);
  addLabelWidget(frameLayout, "Filled"    , pointWidgets_.filledCheck);
  addLabelWidget(frameLayout, "Line Width", pointWidgets_.lineWidthEdit);
  addLabelWidget(frameLayout, "Line Color", pointWidgets_.lineColorEdit);
  addLabelWidget(frameLayout, "Line Alpha", pointWidgets_.lineAlphaEdit);
  addLabelWidget(frameLayout, "Fill Color", pointWidgets_.fillColorEdit);
  addLabelWidget(frameLayout, "Fill Alpha", pointWidgets_.fillAlphaEdit);

  frameLayout->addStretch(1);
}

void
CQChartsAnnotationDlg::
addFillWidgets(Widgets &widgets, QBoxLayout *playout)
{
  QGroupBox *groupBox = new QGroupBox("Background");

  QGridLayout *groupLayout = new QGridLayout(groupBox);

  widgets.backgroundCheck = new CQCheckBox;
  widgets.backgroundColor = new CQColorChooser;
  widgets.backgroundAlpha = new CQChartsAlphaEdit;

  widgets.backgroundAlpha->setRange(0.0, 1.0);
  widgets.backgroundAlpha->setValue(1.0);

  int row = 0;

  addGridLabelWidget(groupLayout, "Visible", widgets.backgroundCheck, row);
  addGridLabelWidget(groupLayout, "Color"  , widgets.backgroundColor, row);
  addGridLabelWidget(groupLayout, "Alpha"  , widgets.backgroundAlpha, row);

  groupLayout->setColumnStretch(2, 1);
  groupLayout->setRowStretch(row, 1);

  playout->addWidget(groupBox);
}

void
CQChartsAnnotationDlg::
addStrokeWidgets(Widgets &widgets, QBoxLayout *playout)
{
  QGroupBox *groupBox = new QGroupBox("Border");

  QGridLayout *groupLayout = new QGridLayout(groupBox);

  widgets.borderCheck = new CQCheckBox;
  widgets.borderColor = new CQColorChooser;
  widgets.borderAlpha = new CQChartsAlphaEdit;
  widgets.borderWidth = new CQChartsLengthEdit;
  widgets.borderDash  = new CQLineDash;

  widgets.borderAlpha->setRange(0.0, 1.0);
  widgets.borderAlpha->setValue(1.0);

  int row = 0;

  addGridLabelWidget(groupLayout, "Visible", widgets.borderCheck, row);
  addGridLabelWidget(groupLayout, "Color"  , widgets.borderColor, row);
  addGridLabelWidget(groupLayout, "Alpha"  , widgets.borderAlpha, row);
  addGridLabelWidget(groupLayout, "Width"  , widgets.borderWidth, row);
  addGridLabelWidget(groupLayout, "Dash"   , widgets.borderDash , row);

  groupLayout->setColumnStretch(2, 1);
  groupLayout->setRowStretch(row, 1);

  playout->addWidget(groupBox);
}

void
CQChartsAnnotationDlg::
addGridLabelWidget(QGridLayout *playout, const QString &label, QWidget *widget, int &row)
{
  QLabel *qlabel = new QLabel(label);
  qlabel->setObjectName("label" + label);

  playout->addWidget(qlabel, row, 0);
  playout->addWidget(widget, row, 1);

  ++row;
}

QHBoxLayout *
CQChartsAnnotationDlg::
addLabelWidget(QBoxLayout *playout, const QString &label, QWidget *widget)
{
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setMargin(0); layout->setSpacing(2);

  QLabel *qlabel = new QLabel(label);
  qlabel->setObjectName("label" + label);

  layout->addWidget (qlabel);
  layout->addWidget (widget);
  layout->addStretch(1);

  playout->addLayout(layout);

  return layout;
}

void
CQChartsAnnotationDlg::
typeSlot(int ind)
{
  typeStack_->setCurrentIndex(ind);
}

void
CQChartsAnnotationDlg::
okSlot()
{
  if (applySlot())
    cancelSlot();
}

bool
CQChartsAnnotationDlg::
applySlot()
{
  int ind = typeStack_->currentIndex();

  bool rc = false;

  if      (ind == 0) rc = createRectAnnotation();
  else if (ind == 1) rc = createEllipseAnnotation();
  else if (ind == 2) rc = createPolygonAnnotation();
  else if (ind == 3) rc = createPolylineAnnotation();
  else if (ind == 4) rc = createTextAnnotation();
  else if (ind == 5) rc = createArrowAnnotation();
  else if (ind == 6) rc = createPointAnnotation();

  return rc;
}

bool
CQChartsAnnotationDlg::
createRectAnnotation()
{
  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsBoxData boxData;

  CQChartsFillData   &background = boxData.shape.background;
  CQChartsStrokeData &border     = boxData.shape.border;

  CQChartsPosition start = rectWidgets_.startEdit->position();
  CQChartsPosition end   = rectWidgets_.endEdit  ->position();

  boxData.margin  = rectWidgets_.marginEdit ->value();
  boxData.padding = rectWidgets_.paddingEdit->value();

  background.visible = rectWidgets_.backgroundCheck->isChecked();
  background.color   = rectWidgets_.backgroundColor->color();
  background.alpha   = rectWidgets_.backgroundAlpha->value();

  CLineDash lineDash = rectWidgets_.borderDash ->getLineDash();

  std::vector<double> lineDashLengths;

  lineDash.getLengths(lineDashLengths);

  border.visible = rectWidgets_.borderCheck->isChecked();
  border.color   = rectWidgets_.borderColor->color();
  border.alpha   = rectWidgets_.borderAlpha->value();
  border.width   = rectWidgets_.borderWidth->length();
  border.dash    = CQChartsLineDash(lineDashLengths, lineDash.getOffset());

  border.cornerSize = rectWidgets_.cornerSizeEdit->length();

  boxData.borderSides = rectWidgets_.borderSidesEdit->text();

  //---

  CQChartsRectAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addRectAnnotation(start, end);
  else if (plot_)
    annotation = plot_->addRectAnnotation(start, end);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsAnnotationDlg::
createEllipseAnnotation()
{
  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsBoxData boxData;

  CQChartsFillData   &background = boxData.shape.background;
  CQChartsStrokeData &border     = boxData.shape.border;

  CQChartsPosition center = ellipseWidgets_.centerEdit->position();

  CQChartsLength rx = ellipseWidgets_.rxEdit->length();
  CQChartsLength ry = ellipseWidgets_.ryEdit->length();

  background.visible = ellipseWidgets_.backgroundCheck->isChecked();
  background.color   = ellipseWidgets_.backgroundColor->color();
  background.alpha   = ellipseWidgets_.backgroundAlpha->value();

  CLineDash lineDash = ellipseWidgets_.borderDash ->getLineDash();

  std::vector<double> lineDashLengths;

  lineDash.getLengths(lineDashLengths);

  border.visible = ellipseWidgets_.borderCheck->isChecked();
  border.color   = ellipseWidgets_.borderColor->color();
  border.alpha   = ellipseWidgets_.borderAlpha->value();
  border.width   = ellipseWidgets_.borderWidth->length();
  border.dash    = CQChartsLineDash(lineDashLengths, lineDash.getOffset());

  border.cornerSize = ellipseWidgets_.cornerSizeEdit->length();

  boxData.borderSides = ellipseWidgets_.borderSidesEdit->text();

  //---

  CQChartsEllipseAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addEllipseAnnotation(center, rx, ry);
  else if (plot_)
    annotation = plot_->addEllipseAnnotation(center, rx, ry);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsAnnotationDlg::
createPolygonAnnotation()
{
  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsShapeData shapeData;

  CQChartsFillData   &background = shapeData.background;
  CQChartsStrokeData &border     = shapeData.border;

  QPolygonF points;

  CQChartsUtil::stringToPolygon(polygonWidgets_.pointsEdit->text(), points);

  if (! points.length())
    return false;

  background.visible = polygonWidgets_.backgroundCheck->isChecked();
  background.color   = polygonWidgets_.backgroundColor->color();
  background.alpha   = polygonWidgets_.backgroundAlpha->value();

  CLineDash lineDash = polygonWidgets_.borderDash ->getLineDash();

  std::vector<double> lineDashLengths;

  lineDash.getLengths(lineDashLengths);

  border.visible = polygonWidgets_.borderCheck->isChecked();
  border.color   = polygonWidgets_.borderColor->color();
  border.alpha   = polygonWidgets_.borderAlpha->value();
  border.width   = polygonWidgets_.borderWidth->length();
  border.dash    = CQChartsLineDash(lineDashLengths, lineDash.getOffset());

  //---

  CQChartsPolygonAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addPolygonAnnotation(points);
  else if (plot_)
    annotation = plot_->addPolygonAnnotation(points);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setShapeData(shapeData);

  return true;
}

bool
CQChartsAnnotationDlg::
createPolylineAnnotation()
{
  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsShapeData shapeData;

  CQChartsFillData   &background = shapeData.background;
  CQChartsStrokeData &border     = shapeData.border;

  QPolygonF points;

  CQChartsUtil::stringToPolygon(polylineWidgets_.pointsEdit->text(), points);

  if (! points.length())
    return false;

  background.visible = polylineWidgets_.backgroundCheck->isChecked();
  background.color   = polylineWidgets_.backgroundColor->color();
  background.alpha   = polylineWidgets_.backgroundAlpha->value();

  CLineDash lineDash = polylineWidgets_.borderDash ->getLineDash();

  std::vector<double> lineDashLengths;

  lineDash.getLengths(lineDashLengths);

  border.visible = polylineWidgets_.borderCheck->isChecked();
  border.color   = polylineWidgets_.borderColor->color();
  border.alpha   = polylineWidgets_.borderAlpha->value();
  border.width   = polylineWidgets_.borderWidth->length();
  border.dash    = CQChartsLineDash(lineDashLengths, lineDash.getOffset());

  //---

  CQChartsPolylineAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addPolylineAnnotation(points);
  else if (plot_)
    annotation = plot_->addPolylineAnnotation(points);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setShapeData(shapeData);

  return true;
}

bool
CQChartsAnnotationDlg::
createTextAnnotation()
{
  CQChartsTextData textData;
  CQChartsBoxData  boxData;

  CQChartsFillData   &background = boxData.shape.background;
  CQChartsStrokeData &border     = boxData.shape.border;

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition pos = textWidgets_.positionEdit->position();

  QString text = textWidgets_.textEdit->text();

  textData.font     = textWidgets_.fontEdit->font();
  textData.color    = textWidgets_.colorEdit->color();
  textData.alpha    = textWidgets_.alphaEdit->value();
  textData.angle    = textWidgets_.angleEdit->value();
  textData.contrast = textWidgets_.contrastCheck->isChecked();
  textData.align    = textWidgets_.alignEdit->align();
  textData.html     = textWidgets_.htmlCheck->isChecked();

  background.visible = textWidgets_.backgroundCheck->isChecked();
  background.color   = textWidgets_.backgroundColor->color();
  background.alpha   = textWidgets_.backgroundAlpha->value();

  CLineDash lineDash = textWidgets_.borderDash ->getLineDash();

  std::vector<double> lineDashLengths;

  lineDash.getLengths(lineDashLengths);

  border.visible = textWidgets_.borderCheck->isChecked();
  border.color   = textWidgets_.borderColor->color();
  border.alpha   = textWidgets_.borderAlpha->value();
  border.width   = textWidgets_.borderWidth->length();
  border.dash    = CQChartsLineDash(lineDashLengths, lineDash.getOffset());

  border.cornerSize = textWidgets_.cornerSizeEdit->length();

  boxData.borderSides = textWidgets_.borderSidesEdit->text();

  //---

  CQChartsTextAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addTextAnnotation(pos, text);
  else if (plot_)
    annotation = plot_->addTextAnnotation(pos, text);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setTextData(textData);
  annotation->setBoxData(boxData);

  return true;
}

bool
CQChartsAnnotationDlg::
createArrowAnnotation()
{
  CQChartsArrowData arrowData;

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition start = arrowWidgets_.startEdit->position();
  CQChartsPosition end   = arrowWidgets_.endEdit  ->position();

  arrowData.length    = arrowWidgets_.lengthEdit->length();
  arrowData.angle     = arrowWidgets_.angleEdit->value();
  arrowData.backAngle = arrowWidgets_.backAngleEdit->value();
  arrowData.fhead     = arrowWidgets_.fheadCheck->isChecked();
  arrowData.thead     = arrowWidgets_.theadCheck->isChecked();
  arrowData.empty     = arrowWidgets_.emptyCheck->isChecked();

  CQChartsShapeData shapeData;

  CQChartsStrokeData &stroke = shapeData.border;
  CQChartsFillData   &fill   = shapeData.background;

  stroke.width = arrowWidgets_.lineWidthEdit->length();
  stroke.color = arrowWidgets_.strokeColorEdit->color();

  fill.visible = arrowWidgets_.filledCheck->isChecked();
  fill.color   = arrowWidgets_.fillColorEdit->color();

  //---

  CQChartsArrowAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addArrowAnnotation(start, end);
  else if (plot_)
    annotation = plot_->addArrowAnnotation(start, end);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setArrowData(arrowData);

  annotation->arrow()->setShapeData(shapeData);

  return true;
}

bool
CQChartsAnnotationDlg::
createPointAnnotation()
{
  CQChartsSymbolData pointData;

  QString id    = idEdit_ ->text();
  QString tipId = tipEdit_->text();

  CQChartsPosition pos = pointWidgets_.positionEdit->position();

  pointData.size = pointWidgets_.sizeEdit->length();

  QString typeStr = pointWidgets_.typeEdit->text();

  if (typeStr.length())
    pointData.type = CQChartsSymbol::nameToType(typeStr);

  pointData.stroke.visible = pointWidgets_.strokedCheck->isChecked();
  pointData.fill  .visible = pointWidgets_.filledCheck->isChecked();

  pointData.stroke.width = pointWidgets_.lineWidthEdit->length();
  pointData.stroke.color = pointWidgets_.lineColorEdit->color();
  pointData.stroke.alpha = pointWidgets_.lineAlphaEdit->value();

  pointData.fill.color = pointWidgets_.fillColorEdit->color();
  pointData.fill.alpha = pointWidgets_.fillAlphaEdit->value();

  //---

  CQChartsPointAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->addPointAnnotation(pos, pointData.type);
  else if (plot_)
    annotation = plot_->addPointAnnotation(pos, pointData.type);

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setPointData(pointData);

  return true;
}

void
CQChartsAnnotationDlg::
cancelSlot()
{
  close();
}
