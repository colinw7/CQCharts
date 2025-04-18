#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsModelDetails.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>
#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsPlotDrawUtil.h>
#include <CQChartsTextPlacer.h>
#include <CQChartsHtml.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsTreeMapPlace.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQEnumCombo.h>
#include <CMathRound.h>
#include <CXYVals.h>

#include <QMenu>
#include <QCheckBox>

using RotatedTextBoxObj = CQChartsRotatedTextBoxObj;

CQChartsPiePlotType::
CQChartsPiePlotType()
{
}

void
CQChartsPiePlotType::
addParameters()
{
  startParameterGroup("Pie");

  // args: name, desc, propName, attributes, default
  addColumnsParameter("values", "Value", "valueColumns").
    setRequired().setNumericColumn().setPropPath("columns.values").setTip("Value column(s)");
  addColumnParameter("label", "Label", "labelColumn").
    setStringColumn().setBasic().setPropPath("columns.label").setTip("Custom label column");
  addColumnParameter("radius", "Radius", "radiusColumn").
    setNumericColumn().setPropPath("columns.radius").setTip("Custom radius column");
  addColumnParameter("keyLabel", "Key Label", "keyLabelColumn").
    setStringColumn().setPropPath("columns.keyLabel").setTip("Custom key label column");

  addEnumParameter("drawType", "Draw Type", "drawType").
    addNameValue("PIE"    , static_cast<int>(CQChartsPiePlot::DrawType::PIE)).
    addNameValue("TREEMAP", static_cast<int>(CQChartsPiePlot::DrawType::TREEMAP)).
    addNameValue("WAFFLE" , static_cast<int>(CQChartsPiePlot::DrawType::WAFFLE)).
    setPropPath("options.drawType").setTip("Draw type");

  addBoolParameter("separated", "Separated", "separated" ).
    setTip("Draw grouped pie charts separately");

  addBoolParameter("donut"         , "Donut"           , "donut"         ).
    setPropPath("donut.visible").setTip("Display pie as donut using inner radius");
  addEnumParameter("donutValueType", "Donut Value Type", "donutValueType").
    addNameValue("NONE" , static_cast<int>(CQChartsPiePlot::DonutValueType::NONE)).
    addNameValue("MIN"  , static_cast<int>(CQChartsPiePlot::DonutValueType::MIN)).
    addNameValue("MAX"  , static_cast<int>(CQChartsPiePlot::DonutValueType::MAX)).
    addNameValue("MEAN" , static_cast<int>(CQChartsPiePlot::DonutValueType::MEAN)).
    addNameValue("SUM"  , static_cast<int>(CQChartsPiePlot::DonutValueType::SUM)).
    addNameValue("COUNT", static_cast<int>(CQChartsPiePlot::DonutValueType::COUNT)).
    addNameValue("TITLE", static_cast<int>(CQChartsPiePlot::DonutValueType::TITLE)).
    setPropPath("donut.valueType").setTip("Value type shown in donut center");

  addBoolParameter("dumbbell"    , "Dumbbell"     , "dumbbell"    ).setTip("Draw group dumbbell");
  addBoolParameter("dumbbellType", "Dumbbell Type", "dumbbellType").setTip("Draw dumbbell type");

  addBoolParameter("summary", "Summary", "summary").setTip("Draw summary group");

  addBoolParameter("showLabel"   , "Show Label"   , "showLabel"   ).setTip("Show pie label");
  addBoolParameter("showValue"   , "Show Value"   , "showValue"   ).setTip("Show pie value");
  addBoolParameter("valuePercent", "Value Percent", "valuePercent").setTip("Value is percent");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsPiePlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &str) { return CQChartsHtml::Str::img(str); };

  return CQChartsHtml().
    h2("Pie Plot").
     h3("Summary").
      p("Draw circle segments with diameter from a set of values.").
      p("The segments can be restricted to an inner radius and a label "
        "can be displayed at the center of the circle.").
     h3("Columns").
      p("The values come from the " + B("Values") + " column.").
      p("Optional labels come from the " + B("Labels") + " column.").
      p("A custom pie slice radius can be specified in the " + B("Radius") + " column.").
     h3("Options").
      p("All pie slices can start at a specified inner radius by enabling the " +
        B("Donut") + " option.").
      p("The label can include the value using the " + B("Summary") + " option.").
      p("Grouped data can be displayed as separate circular regions or as separated "
        "pie charts using the " + B("Separated") + " option.").
      p("The pie can be displayed alternatively as a treemap or waffle plot.").
     h3("Limitations").
      p("This plot does not support a user specified range, axes, logarithmic scales, "
        "or probing.").
      p("The plot does not support X/Y axes.").
     h3("Example").
      p(IMG("images/piechart.png"));
}

// TODO: analyzeModel

CQChartsPlot *
CQChartsPiePlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsPiePlot(view, model);
}

//------

CQChartsPiePlot::
CQChartsPiePlot(View *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("pie"), model),
 CQChartsObjShapeData          <CQChartsPiePlot>(this),
 CQChartsObjGridLineData       <CQChartsPiePlot>(this),
 CQChartsObjGroupShapeData     <CQChartsPiePlot>(this),
 CQChartsObjGroupTextData      <CQChartsPiePlot>(this),
 CQChartsObjTextLabelBoxData   <CQChartsPiePlot>(this),
 CQChartsObjTextLabelTextData  <CQChartsPiePlot>(this),
 CQChartsObjRadiusLabelTextData<CQChartsPiePlot>(this),
 CQChartsObjDialLineData       <CQChartsPiePlot>(this)
{
}

CQChartsPiePlot::
~CQChartsPiePlot()
{
  CQChartsPiePlot::term();
}

//---

void
CQChartsPiePlot::
init()
{
  CQChartsGroupPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  setFillColor(Color::makePalette());

  setGroupFillColor(Color::makePalette());

  setGridLines(false);
  setGridLinesColor(Color::makeInterfaceValue(0.5));

  setTextLabelFilled(false);
  setTextLabelStroked(false);
  setTextLabelTextColor(Color::makeInterfaceValue(1.0));

  setRadiusLabelTextColor(Color::makeContrast());
  setRadiusLabelTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);

  //---

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  //---

  addKey();

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsPiePlot::
term()
{
}

//---

void
CQChartsPiePlot::
setLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

void
CQChartsPiePlot::
setValueColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setRadiusColumn(const Column &c)
{
  CQChartsUtil::testAndSet(radiusColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

void
CQChartsPiePlot::
setKeyLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(keyLabelColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

//---

CQChartsColumn
CQChartsPiePlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "label"   ) c = this->labelColumn();
  else if (name == "radius"  ) c = this->radiusColumn();
  else if (name == "keyLabel") c = this->keyLabelColumn();
  else                         c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsPiePlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "label"   ) this->setLabelColumn(c);
  else if (name == "radius"  ) this->setRadiusColumn(c);
  else if (name == "keyLabel") this->setKeyLabelColumn(c);
  else                         CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsPiePlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "values") c = this->valueColumns();
  else                  c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsPiePlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "values") this->setValueColumns(c);
  else                  CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsPiePlot::
setDrawType(const DrawType &drawType)
{
  CQChartsUtil::testAndSet(drawType_, drawType, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setTreeMap(bool b)
{
  setDrawType(b ? DrawType::TREEMAP : DrawType::PIE);
}

bool
CQChartsPiePlot::
calcTreeMap() const
{
  if (! isTreeMap())
    return false;

  if (! isSeparated() && numGroups() > 1)
    return false;

  return true;
}

//---

void
CQChartsPiePlot::
setWaffle(bool b)
{
  setDrawType(b ? DrawType::WAFFLE : DrawType::PIE);
}

bool
CQChartsPiePlot::
calcWaffle() const
{
  if (! isWaffle())
    return false;

  if (! isSeparated() && numGroups() > 1)
    return false;

  return true;
}

//---

bool
CQChartsPiePlot::
calcPie() const
{
  if (drawType() == DrawType::PIE)
    return true;

  if (calcTreeMap() || calcWaffle())
    return false;

  if (isDumbbell())
    return false;

  return true;
}

//---

void
CQChartsPiePlot::
setDonut(bool b)
{
  CQChartsUtil::testAndSet(donutData_.visible, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

bool
CQChartsPiePlot::
calcDonut() const
{
  if (! isDonut())
    return false;

  if (calcTreeMap() || calcWaffle())
    return false;

  return true;
}

//---

void
CQChartsPiePlot::
setSummary(bool b)
{
  CQChartsUtil::testAndSet(summary_, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setDumbbell(bool b)
{
  CQChartsUtil::testAndSet(dumbbellData_.visible, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

bool
CQChartsPiePlot::
calcDumbbell() const
{
  if (! isDumbbell())
    return false;

  return (isSeparated() && ! isSummary());
}

void
CQChartsPiePlot::
setDumbbellType(const DumbbellType &t)
{
  CQChartsUtil::testAndSet(dumbbellData_.type, t, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setShowLabel(bool b)
{
  CQChartsUtil::testAndSet(showLabel_, b, [&]() {
    drawObjs(); Q_EMIT customDataChanged(); } );
}

void
CQChartsPiePlot::
setShowValue(bool b)
{
  CQChartsUtil::testAndSet(showValue_, b, [&]() {
    drawObjs(); Q_EMIT customDataChanged(); } );
}

void
CQChartsPiePlot::
setValuePercent(bool b)
{
  CQChartsUtil::testAndSet(valuePercent_, b, [&]() {
    drawObjs(); Q_EMIT customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setBucketed(bool b)
{
  CQChartsUtil::testAndSet(bucketed_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setNumBuckets(int n)
{
  CQChartsUtil::testAndSet(numBuckets_, n, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setDonutValueType(const DonutValueType &t)
{
  CQChartsUtil::testAndSet(donutData_.valueType, t, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

void
CQChartsPiePlot::
setDonutTypeLabel(bool b)
{
  CQChartsUtil::testAndSet(donutData_.typeLabel, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setValueType(const ValueType &t)
{
  CQChartsUtil::testAndSet(valueType_, t, [&]() { updateRangeAndObjs(); } );
}

QString
CQChartsPiePlot::
valueTypeName() const
{
  if      (valueType() == ValueType::MIN ) return "Min";
  else if (valueType() == ValueType::MAX ) return "Max";
  else if (valueType() == ValueType::MEAN) return "Mean";
  else if (valueType() == ValueType::SUM ) return "Sum";

  return "";
}

//---

void
CQChartsPiePlot::
setMinValue(const OptReal &r)
{
  CQChartsUtil::testAndSet(minValue_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setMaxValue(const OptReal &r)
{
  CQChartsUtil::testAndSet(maxValue_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setInnerRadius(double r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setOuterRadius(double r)
{
  CQChartsUtil::testAndSet(outerRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setLabelRadius(double r)
{
  CQChartsUtil::testAndSet(labelRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setStartAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(startAngle_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setAngleExtent(const Angle &a)
{
  CQChartsUtil::testAndSet(angleExtent_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setGapAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(gapAngle_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setClockwise(bool b)
{
  CQChartsUtil::testAndSet(clockwise_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setAdjustText(bool b)
{
  CQChartsUtil::testAndSet(adjustText_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setSeparated(bool b)
{
  CQChartsUtil::testAndSet(separated_, b, [&]() { updateRangeAndObjs(); } );
}

bool
CQChartsPiePlot::
calcSeparated() const
{
  if (! isSeparated())
    return false;

  return (numGroups() > 1);
}

//---

void
CQChartsPiePlot::
setRotatedText(bool b)
{
  CQChartsUtil::testAndSet(rotatedText_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
setExplodeSelected(bool b)
{
  CQChartsUtil::testAndSet(explodeData_.selected, b, [&]() { drawObjs(); } );
}

void
CQChartsPiePlot::
setExplodeRadius(double r)
{
  CQChartsUtil::testAndSet(explodeData_.radius, r, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
setTextLabels(bool b)
{
  CQChartsUtil::testAndSet(textLabels_, b, [&]() { drawObjs(); } );
}

void
CQChartsPiePlot::
setRadiusLabels(bool b)
{
  CQChartsUtil::testAndSet(radiusLabels_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
setShowDial(bool b)
{
  CQChartsUtil::testAndSet(showDial_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
setWaffleType(const WaffleType &t)
{
  CQChartsUtil::testAndSet(waffleData_.type, t, [&]() { drawObjs(); } );
}

void
CQChartsPiePlot::
setWaffleRows(int n)
{
  CQChartsUtil::testAndSet(waffleData_.rows, n, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setWaffleCols(int n)
{
  CQChartsUtil::testAndSet(waffleData_.cols, n, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setWaffleBorder(const Length &l)
{
  CQChartsUtil::testAndSet(waffleData_.border, l, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "valueColumns"  , "values"  , "Value columns");
  addProp("columns", "labelColumn"   , "label"   , "Label column");
  addProp("columns", "radiusColumn"  , "radius"  , "Radius column");
  addProp("columns", "keyLabelColumn", "keyLabel", "Key label column");

  addGroupingProperties();

  // options
  addProp("options", "drawType"    , "", "Draw type");
  addProp("options", "separated"   , "", "Draw grouped pie charts separately");
  addProp("options", "summary"    , "", "Draw summary group");

  addProp("options", "showLabel"   , "", "Show pie label");
  addProp("options", "showValue"   , "", "Show pie value");
  addProp("options", "valuePercent", "", "Show value as percent");

  addProp("options", "valueType", "", "Value type (when multiple values per name)");
  addProp("options", "minValue" , "", "Custom min value");
  addProp("options", "maxValue" , "", "Custom max value");

  addProp("options", "innerRadius", "", "Inner radius for donut")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("options", "outerRadius", "", "Outer radius for donut")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("options", "startAngle" , "", "Start angle for first segment");
  addProp("options", "angleExtent", "", "Angle extent for pie segments");
  addProp("options", "gapAngle"   , "", "Gap angle");
  addProp("options", "clockwise"  , "", "Clockwise");

  // donut
  addProp("donut", "donut"         , "visible"  , "Display donut using inner radius");
  addProp("donut", "donutValueType", "valueType", "Value type shown in donut center");
  addProp("donut", "donutTypeLabel", "typeLabel", "Value type label shown in donut center");

  // dumbell
  addProp("dumbbell", "dumbbell"    , "visible", "Draw group dumbbell");
  addProp("dumbbell", "dumbbellType", "type"   , "Draw group dumbbell type");

  // buckets
  addProp("bucket", "bucketed"  , "enabled", "Is value bucketing enabled");
  addProp("bucket", "numBuckets", "count", "Number of buckets");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // group style
  addFillProperties("group/fill"  , "groupFill"  , "Group");
  addLineProperties("group/stroke", "groupStroke", "Group");

  addTextProperties("group/text", "groupText", "Group",
                    CQChartsTextOptions::ValueType::ALL);

  // grid
  addProp("grid", "gridLines", "visible", "Grid lines visible");

  addLineProperties("grid/stroke", "gridLines", "Grid");

  // explode
  addPropI("explode", "explodeStyle"   , "style"   , "Explode style"); // TODO
  addProp ("explode", "explodeSelected", "selected", "Explode selected segments");
  addProp ("explode", "explodeRadius"  , "radius"  , "Explode radius")->setMinValue(0.0);

  // waffle
  addProp("waffle", "waffleType"  , "type"  , "Waffle type");
  addProp("waffle", "waffleRows"  , "rows"  , "Waffle box rows");
  addProp("waffle", "waffleCols"  , "cols"  , "Waffle box cols");
  addProp("waffle", "waffleBorder", "border", "Waffle border");

  // labels
  addProp("labels", "textLabels" , "visible", "Labels visible");
  addProp("labels", "labelRadius", "radius" , "Radius labels are drawn at")->setMinValue(0.0);
  addProp("labels", "rotatedText", "rotated", "Labels text is rotated to segment angle");
  addProp("labels", "adjustText" , ""       , "Adjust text placement");

  addProp("labels/box", "textLabelMargin" , "margin" , "Label box outer margin");
  addProp("labels/box", "textLabelPadding", "padding", "Label box inner padding");

  addProp("labels/box/fill", "textLabelFilled", "visible", "Label box fill visible");

  addFillProperties("labels/box/fill", "textLabelFill"  , "Label box");

  addProp("labels/box/stroke", "textLabelStroked", "visible", "Label box stroke visible");

  addLineProperties("labels/box/stroke", "textLabelStroke", "Label box");

  addStyleProp("labels/box/cornerSize" , "textLabelCornerSize" ,
               "cornerSize", "Label box corner size");
  addStyleProp("labels/box/borderSides", "textLabelBorderSides",
               "sides"     , "Label box visible sides");

  addTextProperties("labels/text", "textLabelText", "Labels",
                    CQChartsTextOptions::ValueType::ALL);

  // radius label text
  addProp("radius/text", "radiusLabels", "visible", "Radius label visible");

  addTextProperties("radius/text", "radiusLabelText", "Radius label",
                    CQChartsTextOptions::ValueType::ALL);

  //---

  addProp("dial", "showDial", "visible", "Show dial for current value");

  addLineProperties("dial", "dialLines", "Lines");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

CQChartsGeom::Range
CQChartsPiePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsPiePlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsPiePlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  // value column required
  // name, id, color columns optional

  if (! checkNumericColumns(valueColumns(), "Value", /*required*/true))
    columnsValid = false;

  if (! checkColumn(labelColumn   (), "Label"    )) columnsValid = false;
  if (! checkColumn(radiusColumn  (), "Radius"   )) columnsValid = false;
  if (! checkColumn(keyLabelColumn(), "Key Label")) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  Range dataRange;

  Point c(0.0, 0.0);

  dataRange.updateRange(c);

  //---

  double r = std::max(1.0, labelRadius());

  auto   angle1 = startAngle();
  double alen   = CQChartsUtil::clampDegrees(angleExtent().value());
  auto   angle2 = Angle(angle1.value() + alen);

  // add segment outside points
  dataRange.updateRange(Angle::circlePoint(c, r, angle1));
  dataRange.updateRange(Angle::circlePoint(c, r, angle2));

  // add intermediate points (every 90 degree point between outside points)
  double a1 = 90.0*CMathRound::RoundDownF(angle1.value()/90.0);

  if (angle1 < angle2) {
    for (double a = a1; a < angle2.value(); a += 90.0) {
      if (a > angle1.value() && a < angle2.value())
        dataRange.updateRange(Angle::circlePoint(c, r, Angle(a)));
    }
  }
  else {
    for (double a = a1; a > angle2.value(); a -= 90.0) {
      if (a > angle2.value() && a < angle1.value())
        dataRange.updateRange(Angle::circlePoint(c, r, Angle(a)));
    }
  }

  //---

  // init grouping
  initGroupData(valueColumns(), labelColumn());

  //---

  return dataRange;
}

//------

CQChartsGeom::BBox
CQChartsPiePlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsPiePlot::calcExtraFitBBox");

  BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    auto *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

    if (! pieObj || ! pieObj->isVisible())
      continue;

    bbox += pieObj->extraFitBBox();
  }

  return bbox;
}

//------

bool
CQChartsPiePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsPiePlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsPiePlot *>(this);

  //---

  // calc group totals
  calcDataTotal();

  //---

  th->groupObjs_.clear();

  th->updateColumnNames();

  //---

  if (! valueColumns().isValid())
    return false;

  //---

  bool separated = calcSeparated();

  double ri = std::min(std::max(calcDonut() ? innerRadius() : 0.0, 0.0), 1.0);
  double ro = outerRadius();

  int ig = 0;
  int ng = numGroups();

  double dr;

  if (separated)
    dr = ro - ri;
  else
    dr = (ng > 0 ? (ro - ri)/ng : 0.0);

  double r = ro;

  std::vector<int> groupInds;

  this->getGroupInds(groupInds);

  int ng1 = int(groupInds.size()); // assert (ng1 == ng) ?

  for (const auto &groupInd : groupInds) {
    auto pg = th->groupDatas_.find(groupInd);

    if (pg == th->groupDatas_.end()) {
      auto groupName = groupIndName(groupInd);

      pg = th->groupDatas_.emplace_hint(pg, groupInd, groupName);
    }

    const auto &groupData = (*pg).second;

    //---

    // create group obj
    BBox rect(center_.x - ro, center_.y - ro, center_.x + ro, center_.y + ro);

    auto *groupObj =
      createGroupObj(rect, ColorInd(groupInd, ng), groupData.name, ColorInd(ig, ng1));

    groupObj->setColorIndex(ColorInd(groupInd, ng));

    groupObj->setValues(groupData.values);

    groupObj->setRadiusMax   (groupData.radiusMax);
    groupObj->setRadiusScaled(groupData.radiusScaled);

    // group object inside donut (objects outside group)
    if      (calcDonut()) {
      groupObj->setInnerRadius(0.0);
      groupObj->setOuterRadius(innerRadius());
    }
    // summary so place all groups at same location
    else if (isSummary()) {
      groupObj->setInnerRadius(ri);
      groupObj->setOuterRadius(ro);
    }
    // no summary then separate circle per group
    // if separated then r and dr are constant
    else {
      groupObj->setInnerRadius(r - dr);
      groupObj->setOuterRadius(r);
    }

  //groupObj->setVisible(isSummary());

    objs.push_back(groupObj);

    th->groupObjs_.push_back(groupObj);

    //---

    if (groupData.groupObj != groupObj) {
      auto &groupData1 = const_cast<GroupData &>(groupData);

      groupData1.groupObj = groupObj;
    }

    //---

    if (! separated)
      r -= dr;

    ++ig;
  }

  //---

  // if group only shows summary then no pie objects needed
  bool isSummaryGroup = (isSummary() && ! calcDonut());

  // add individual value objects (not needed for summary only - no donut)
  // TODO: separate option for summary and donut summary label

  if (! isSummaryGroup) {
    // init value sets
    //initValueSets();

    //---

    // process model data
    class PieVisitor : public ModelVisitor {
     public:
      PieVisitor(const CQChartsPiePlot *piePlot, PlotObjs &objs) :
       piePlot_(piePlot), objs_(objs) {
      }

      State visit(const QAbstractItemModel *, const VisitData &data) override {
        piePlot_->addRow(data, objs_);

        return State::OK;
      }

     private:
      const CQChartsPiePlot *piePlot_ { nullptr };
      PlotObjs&              objs_;
    };

    PieVisitor pieVisitor(this, objs);

    visitModel(pieVisitor);
  }

  //---

  adjustObjAngles();

  //---

  if (separated) {
    bool dumbbell = calcDumbbell();

    int ng = numGroupObjs();

    int nx, ny;

    if (! dumbbell) {
      CQChartsUtil::countToSquareGrid(ng, nx, ny);
    }
    else {
      ny = ng;
      nx = 2;
    }

    double dx = 1.0/nx;
    double dy = 1.0/ny;
    double dr = std::min(dx, dy);

    int ig = 0;

    for (auto &groupObj : groupObjs_) {
      int ix, iy;

      if (! dumbbell) {
        ix = ig % nx;
        iy = ig / nx;
      }
      else {
        ix = 0;
        iy = ig;
      }

      double x = CMathUtil::map(ix, 0, nx - 1, -1.0 + dx, 1.0 - dx);
      double y = CMathUtil::map(iy, 0, ny - 1, -1.0 + dy, 1.0 - dy);

      groupObj->setCenter(Point(x, y));

      groupObj->setInnerRadius(0.0);
      groupObj->setOuterRadius(dr);

      ++ig;
    }
  }

  for (auto &plotObj : objs) {
    auto *groupObj = dynamic_cast<CQChartsPieGroupObj *>(plotObj);
    if (! groupObj) continue;

    int iv = 0;
    int nv = groupObj->numObjs();

    for (auto &pieObj : groupObj->objs()) {
      pieObj->setIv(ColorInd(iv, nv));

      ++iv;
    }
  }

  //---

  if      (calcTreeMap()) {
    for (auto &plotObj : objs) {
      auto *groupObj = dynamic_cast<CQChartsPieGroupObj *>(plotObj);
      if (! groupObj) continue;

      //---

      BBox bbox(0.0, 0.0, 1.0, 1.0);

      CQChartsTreeMapPlace place(bbox);

      double dataTotal { 0.0 };

      auto valueSum = calcValueSum();

      if (valueSum > 0.0)
        dataTotal = valueSum;

      for (const auto &obj : groupObj->objs()) {
        auto v = obj->value();

        place.addValue(v);

        dataTotal -= v;
      }

      if (dataTotal > 0.0)
        place.addValue(dataTotal);

      place.placeValues();

      //---

      place.processAreas([&](const BBox &bbox, const CQChartsTreeMapPlace::IArea &iarea) {
        if (iarea.i >= 0 && iarea.i < groupObj->numObjs()) {
          auto *obj = groupObj->obj(iarea.i);

          obj->setTreeMapBBox(bbox);
        }
      });
    }
  }
  else if (calcWaffle()) {
    bool hbars = (waffleRows() <= 0 && waffleCols() > 0);
    bool vbars = (waffleCols() <= 0 && waffleRows() > 0);

    if (hbars || vbars) {
      using WaffleBarType = CQChartsPieGroupObj::WaffleBarType;

      for (auto &plotObj : objs) {
        auto *groupObj = dynamic_cast<CQChartsPieGroupObj *>(plotObj);
        if (! groupObj) continue;

        RMinMax minMax;

        for (const auto &obj : groupObj->objs()) {
          auto v = obj->value();

          minMax.add(v);
        }

        auto maxValue = minMax.max();

        int nr, nc;

        if (hbars) {
          nr = groupObj->objs().size();
          nc = waffleCols();
        }
        else {
          nc = groupObj->objs().size();
          nr = waffleRows();
        }

        groupObj->setWaffleBarType(hbars ? WaffleBarType::HORIZONTAL : WaffleBarType::VERTICAL);
        groupObj->setWaffleRows(nr);
        groupObj->setWaffleCols(nc);

        int ig = 0;

        for (const auto &obj : groupObj->objs()) {
          auto v = obj->value();

          if (hbars) {
            auto c = CMathUtil::map(v, 0.0, maxValue, 0.0, double(nc));

            int ic = std::min(std::max(int(CMathRound::RoundNearest(c)), 0), nc);

            obj->setWaffleStart(ig*nc);
            obj->setWaffleCount(ic);
          }
          else {
            auto r = CMathUtil::map(v, 0.0, maxValue, 0.0, double(nr));

            int ir = std::min(std::max(int(CMathRound::RoundNearest(r)), 0), nr);

            obj->setWaffleStart(ig*nr);
            obj->setWaffleCount(ir);
          }

          ++ig;
        }
      }
    }
    else {
      int nr = std::max(waffleCols(), 1);
      int nc = std::max(waffleRows(), 1);

      for (auto &plotObj : objs) {
        auto *groupObj = dynamic_cast<CQChartsPieGroupObj *>(plotObj);
        if (! groupObj) continue;

        groupObj->setWaffleRows(nr);
        groupObj->setWaffleCols(nc);

        auto dataTotal = groupObj->calcDataTotal();

        int    start = 0;
        double pos   = 0.0;

        int ncells = nr*nc;

        for (const auto &obj : groupObj->objs()) {
          auto v = obj->value();

          pos += v;

          auto r = CMathUtil::map(pos, 0.0, dataTotal, 0.0, double(ncells));

          int end = std::min(std::max(int(CMathRound::RoundNearest(r)), 0), ncells);

          obj->setWaffleStart(start);
          obj->setWaffleCount(end - start);

          start = end;
        }
      }
    }
  }

  //---

  return true;
}

void
CQChartsPiePlot::
addRow(const ModelVisitor::VisitData &data, PlotObjs &objs) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  for (const auto &column : valueColumns()) {
    ModelIndex ind(th, data.row, column, data.parent);

    addRowColumn(ind, objs);
  }
}

void
CQChartsPiePlot::
addRowColumn(const ModelIndex &ind, PlotObjs &objs) const
{
  bool isSummaryGroup = (isSummary() && ! calcDonut());

  assert(! isSummaryGroup);

  auto *th = const_cast<CQChartsPiePlot *>(this);

  //---

  // get column value
  OptReal value;

  if (! getColumnSizeValue(ind, value))
    return;

  //---

  // get column radius
  OptReal radius;

  if (radiusColumn().isValid()) {
    ModelIndex rind(th, ind.row(), radiusColumn(), ind.parent());

    if (! getColumnSizeValue(rind, radius))
      radius = OptReal();
  }

  //---

  // get value label (used for unique values in group)
  QString labelName;

  auto label = calcIndLabel(modelIndex(ind), labelName);

  //---

  // get group obj
  int groupInd = rowGroupInd(ind);

  auto pg = groupDatas_.find(groupInd);
  assert(pg != groupDatas_.end());

  const auto &groupData = (*pg).second;

  auto *groupObj = groupData.groupObj;

  //---

  // set radii
  // . if donut then object is from inner radius to outer radius
  // . if NOT donut then object is from 0.0 to outer radius
  double ri = std::min(std::max(calcDonut() ? innerRadius() : 0.0, 0.0), 1.0);
  double ro = outerRadius();
  double rv = ro;
  double rs = 1.0;

  bool radiusScaled = (groupObj && groupObj->isRadiusScaled());

  if (radius.isSet() && radiusScaled) {
    auto r = radius.real();

    rs = (groupObj->radiusMax() > 0.0 ? r/groupObj->radiusMax() : 1.0);
  }

  //---

  bool hidden = isIndexHidden(ind);

  //---

  auto dataInd  = modelIndex(ind);
  auto dataInd1 = normalizeIndex(dataInd);

  //---

  // get pie object (by label)
  PieObj *obj       = nullptr;
  int     bucketInd = -1;

  if (isBucketed() && value.isSet()) {
    bucketInd = groupObj->bucketValue(ind, value.real(), label);

    obj = (groupObj ? groupObj->lookupObjByInd(bucketInd) : nullptr);
  }
  else {
    obj = (groupObj ? groupObj->lookupObjByName(label) : nullptr);
  }

  //---

  // get key label
  auto keyLabel = label;

  if (keyLabelColumn().isValid()) {
    ModelIndex kind(th, ind.row(), keyLabelColumn(), ind.parent());

    bool ok;

    keyLabel = modelString(kind, ok);
  }

  //---

  if (! obj) {
    BBox rect(center_.x - ro, center_.y - ro, center_.x + ro, center_.y + ro);

    int objInd = (groupObj ? groupObj->numObjs() : 0);

    auto ig = (groupObj ? groupObj->ig() : ColorInd());

    obj = createPieObj(rect, dataInd1, ig);

    obj->connectDataChanged(this, SLOT(updateSlot()));

    if (hidden)
      obj->setVisible(false);

    obj->setColorIndex(ColorInd(objInd, objInd + 1));

    obj->setInnerRadius(ri);
    obj->setOuterRadius(ro);
    obj->setValueRadius(rv);

    obj->setLabel(label);
    obj->setInd(bucketInd);

    if (value.isSet())
      obj->addValue(value.real());

    if (radius.isSet())
      obj->setOptRadius(radius.real());

    obj->setRadiusScale(rs);

    obj->setKeyLabel(keyLabel);

    //---

    Color color;

    if (colorColumnColor(ind.row(), ind.parent(), color))
      obj->setColor(color);

    //---

    objs.push_back(obj);

    if (groupObj)
      groupObj->addObject(obj);
  }
  else {
    // duplicate labels add to value (and radius ?)

    obj->addValue(value.value());

    if (radius.isSet()) {
      auto r = radius.real();

      if (obj->optRadius())
        r += *obj->optRadius();

      if (radiusScaled) {
        double rs = (groupObj->radiusMax() > 0.0 ? r/groupObj->radiusMax() : 1.0);

        obj->setRadiusScale(rs);
      }

      obj->setOptRadius(r);
    }

    // TODO: add dataInd
  }
}

QString
CQChartsPiePlot::
calcIndLabel(const QModelIndex &ind, QString &labelName) const
{
  QString label;

  if (! labelColumn().isValid()) {
    label     = QString::number(ind.row());
    labelName = "Row";

    return label;
  }

  auto *th = const_cast<CQChartsPiePlot *>(this);

  // get value label (used for unique values in group)
  ModelIndex lind(th, ind.row(), labelColumn(), ind.parent());

  bool ok;
  labelName = modelHHeaderString(lind.column(), ok);

  if (numGroups() > 1) {
    if (valueColumns().count() > 1 && ! isGroupHeaders()) {
      labelName = "Name";
      label     = labelName;
    }
    else
      label = modelString(lind, ok);
  }
  else {
    label = modelString(lind, ok);
  }

  if (! label.length()) {
    label     = QString::number(ind.row());
    labelName = "Row";
  }

  return label;
}

void
CQChartsPiePlot::
calcDataTotal() const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  th->values_.clear();

  th->groupDatas_.clear();

  // process model data
  class DataTotalVisitor : public ModelVisitor {
   public:
    DataTotalVisitor(const CQChartsPiePlot *piePlot) :
     piePlot_(piePlot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      piePlot_->addRowDataTotal(data);

      return State::OK;
    }

   private:
    const CQChartsPiePlot *piePlot_ { nullptr };
  };

  DataTotalVisitor dataTotalVisitor(this);

  visitModel(dataTotalVisitor);

  for (auto &pg : th->groupDatas_) {
    auto &groupData = pg.second;

    groupData.values = CQChartsRValues();

    for (auto &nv : groupData.nameValueData) {
    //const auto &name      = nv.first;
      auto       &valueData = nv.second;

      double value = 0.0;

      if      (valueType() == ValueType::MIN ) value = valueData.values.min();
      else if (valueType() == ValueType::MAX ) value = valueData.values.max();
      else if (valueType() == ValueType::MEAN) value = valueData.values.mean();
      else if (valueType() == ValueType::SUM ) value = valueData.values.sum();

      valueData.dataTotal = value;

      groupData.values.addValue(value);
;
      //std::cerr << name.toStdString() << " : " << valueData.dataTotal << "\n";
    }

    //std::cerr << groupData.dataTotal << "\n";
  }
}

void
CQChartsPiePlot::
addRowDataTotal(const ModelVisitor::VisitData &data) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  for (const auto &column : valueColumns()) {
    ModelIndex ind(th, data.row, column, data.parent);

    addRowColumnDataTotal(ind);
  }
}

void
CQChartsPiePlot::
addRowColumnDataTotal(const ModelIndex &ind) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  //---

  bool hidden = isIndexHidden(ind);

  //---

  // get group data for group ind (add if new)
  int groupInd = rowGroupInd(ind);

  auto pg = th->groupDatas_.find(groupInd);

  if (pg == th->groupDatas_.end()) {
    auto groupName = groupIndName(groupInd);

    pg = th->groupDatas_.emplace_hint(pg, groupInd, GroupData(groupName));
  }

  auto &groupData = (*pg).second;

  //---

  // get column value
  OptReal value;

  if (! getColumnSizeValue(ind, value))
    return;

  //---

  // get value label (used for unique values in group)
  QString labelName;

  auto label = calcIndLabel(modelIndex(ind), labelName);

  //---

  auto pv = groupData.nameValueData.find(label);

  if (pv == groupData.nameValueData.end())
    pv = groupData.nameValueData.emplace_hint(pv, label, ValueData());

  auto &valueData = (*pv).second;

  if (! hidden)
    valueData.values.addValue(value.value());

  th->values_.addValue(value.value());

  //---

  // get max radius
  if (radiusColumn().isValid()) {
    ModelIndex rind(th, ind.row(), radiusColumn(), ind.parent());

    OptReal radius;

    if (getColumnSizeValue(rind, radius)) {
      if (! hidden) {
        groupData.radiusScaled = true;
        groupData.radiusMax    = std::max(groupData.radiusMax, radius.real());
      }
    }
  }
}

bool
CQChartsPiePlot::
getColumnSizeValue(const ModelIndex &ind, OptReal &value) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  value = OptReal();

  auto columnType = columnValueType(ind.column());

  if (columnType == ColumnType::INTEGER || columnType == ColumnType::REAL) {
    bool ok;

    auto r = modelReal(ind, ok);

    // allow missing value in numeric column
    if (! ok) {
      value = OptReal();
      return true;
    }

    // TODO: check allow nan
    if (CMathUtil::isNaN(r)) {
      th->addDataError(ind, "Invalid value");
      return false;
    }

    value = OptReal(r);
  }
  else {
    // if has radius column then assume named segments are equal size
    if (radiusColumn().isValid()) {
      value = OptReal(1.0);
      return true;
    }

    // try convert model string to real
    bool ok;

    auto r = modelReal(ind, ok);

    // string non-real -> 1.0
    if (! ok) {
      th->addDataError(ind, "Invalid value");
      return false;
    }

    value = OptReal(r);
  }

  // size must be positive or zero
  if (value.isSet() && value.real() < 0.0) {
    th->addDataError(ind, "Negative value");
    value = OptReal(1.0);
    return false;
  }

  return true;
}

void
CQChartsPiePlot::
adjustObjAngles() const
{
  double ri = std::min(std::max(calcDonut() ? innerRadius() : 0.0, 0.0), 1.0);
  double ro = outerRadius();

  bool isGrouped = (numGroups() > 1);

  bool isSummaryGroup = (isSummary() && ! calcDonut());

  // calc delta radius (grouped)
  int    ng = 1, nh = 0;
  double dr = 0.0;

  if (! isSummaryGroup) {
    if (isGrouped) {
      ng = 0;

      for (auto &groupObj : groupObjs_) {
        if (! isGroupHidden(groupObj->groupInd().i))
          ++ng;
        else
          ++nh;
      }
    }

    dr = (ng > 0 ? (ro - ri)/ng : 0.0);
  }

  //---

  // get total number of values
  int totalValues = 0;

  for (auto &groupObj : groupObjs_) {
    totalValues += groupObj->values().size();
  }

  double da = (totalValues > 0 ? 360.0/totalValues : 0.0);

  //---

  bool separated = calcSeparated();

  // calc angle extents for each group
  double ga1 = startAngle().value();
  double ga2 = ga1;

  double r = ro;

  for (auto &groupObj : groupObjs_) {
    ga1 = ga2;

    // skip hidden groups
    if (! isSummaryGroup) {
      if (isGroupHidden(groupObj->groupInd().i))
        continue;
    }

    //---

    // set group angles
    if (! separated) {
      double dga = da*groupObj->values().size();

      ga2 = ga1 - dga;

      groupObj->setAngles(Angle(ga1), Angle(ga2));
    }
    else {
      groupObj->setAngles(Angle(0.0), Angle(360.0));
    }

    //---

    bool radiusScaled = (groupObj && groupObj->isRadiusScaled());

    //---

    if (! isSummaryGroup) {
      // set group object inner/outer radii
      if (! separated && isGrouped) {
        groupObj->setInnerRadius(r - dr);
        groupObj->setOuterRadius(r);
      }

      //---

      // set segment angles
      double ga = gapAngle().value();

      double angle1 = startAngle().value();
      double alen   = CQChartsUtil::clampDegrees(angleExtent().value());

      auto dataTotal = groupObj->calcDataTotal();

      // count visible
      int numObjs = 0;

      for (auto &obj : groupObj->objs()) {
        if (obj->isVisible())
          ++numObjs;
      }

      if (abs(alen) >= 360.0) { // contiguous
        if (alen < 0)
          alen = std::min(alen + numObjs*ga, 0.0);
        else
          alen = std::max(alen - numObjs*ga, 0.0);
      }
      else {
        if (alen < 0)
          alen = std::min(alen + (numObjs - 1)*ga, 0.0);
        else
          alen = std::max(alen - (numObjs - 1)*ga, 0.0);
      }

      double da1 = (dataTotal > 0.0 ? alen/dataTotal : 0.0);

      for (auto &obj : groupObj->objs()) {
        // skip hidden objects
        if (! obj->isVisible())
          continue;

        //---

        // set angle based on value
        double value = obj->value();

        double angle  = (isClockwise() ? 1.0 : -1.0)*da1*value;
        double angle2 = angle1 + angle;

        obj->setAngle1(Angle(angle1));
        obj->setAngle2(Angle(angle2));

        //---

        // set inner/outer radius and value radius
        if (! separated && isGrouped && calcDonut()) {
          obj->setInnerRadius(r - dr);
          obj->setOuterRadius(r);

          obj->setValueRadius(r);
        }

        //---

        if (obj->optRadius() && radiusScaled) {
          double rs = (groupObj->radiusMax() > 0.0 ? *obj->optRadius()/groupObj->radiusMax() : 1.0);

          obj->setRadiusScale(rs);
        }

        //---

        // move to next start angle
        angle1 = angle2 + ga;
      }

      //---

      // move to next radius
      if (! separated && isGrouped)
        r -= dr;
    }
  }
}

bool
CQChartsPiePlot::
isGroupHidden(int groupInd) const
{
  if (numGroups() > 1)
    return isSetHidden(groupInd);

  return false;
}

bool
CQChartsPiePlot::
isIndexHidden(const ModelIndex &ind) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  bool isColorKey = (groupObjs_.size() <= 1 && colorColumn().isValid());

  // hide all objects of group or individual objects of single group
  bool hidden = false;

  if (isColorKey) {
    ModelIndex colorModelInd(th, ind.row(), colorColumn(), ind.parent());

    bool ok;

    auto colorValue = modelValue(colorModelInd, ok);

    hidden = (ok && isHideValue(colorValue));
  }
  else {
    if (numGroups() > 1) {
      int groupInd = rowGroupInd(ind);

      hidden = isGroupHidden(groupInd);
    }
    else
      hidden = isSetHidden(ind.row());
  }

  return hidden;
}

void
CQChartsPiePlot::
addKeyItems(PlotKey *key)
{
  // start at next row (vertical) or next column (horizontal) from previous key
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addObjKeyRow = [&](CQChartsPlotObj *obj) {
    auto *colorItem = new CQChartsPieColorKeyItem(this, obj);
    auto *textItem  = new CQChartsPieTextKeyItem (this, obj);

    auto *groupItem = new CQChartsGroupKeyItem(this);

    groupItem->addRowItems(colorItem, textItem);

    key->addItem(groupItem, row, col);

    key->nextRowCol(row, col);
  };

  auto addColorKeyRow = [&](const QVariant &value, int i, int n) {
    ColorInd ic(i, n);

    QString name;
    if (! CQChartsVariant::toString(value, name) || name == "")
      name = QString::number(i);

    auto *colorItem = new CQChartsColorBoxKeyItem(this, ColorInd(), ColorInd(), ic);
    auto *textItem  = new CQChartsTextKeyItem    (this, name, ic);

    colorItem->setValue(value);

    bool ok;
    auto c = CQChartsVariant::toColor(value, ok);
    if (ok) colorItem->setColor(c);

    auto *groupItem = new CQChartsGroupKeyItem(this);

    groupItem->addRowItems(colorItem, textItem);

    key->addItem(groupItem, row, col);

    key->nextRowCol(row, col);
  };

  //---

  bool isSummaryGroup = (isSummary() && ! calcDonut());

  if (! isSummaryGroup) {
    int ng = int(groupObjs_.size());

    // add key line per group
    if (ng > 1) {
      for (const auto &groupObj : groupObjs_)
        addObjKeyRow(groupObj);
    }
    // add key line per unique color or per object
    else {
      if (colorColumn().isValid()) {
        const auto *columnDetails = this->columnDetails(colorColumn());

        int n = columnDetails->numUnique();

        for (int i = 0; i < n; ++i) {
          const auto &value = columnDetails->uniqueValue(i);

          addColorKeyRow(value, i, n);
        }
      }
      else {
        for (auto &plotObj : plotObjs_) {
          auto *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

          if (pieObj)
            addObjKeyRow(plotObj);
        }
      }
    }
  }
  else {
    for (const auto &groupObj : groupObjs_)
      addObjKeyRow(groupObj);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsPiePlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//------

bool
CQChartsPiePlot::
addMenuItems(QMenu *menu, const Point &)
{
  auto addCheckAction = [&](const QString &name, bool checked, const char *slotName) {
    return CQUtil::addCheckedAction(menu, name, checked, this, slotName);
  };

  menu->addSeparator();

  addCheckAction("Donut"     , isDonut     (), SLOT(setDonut     (bool)));
  addCheckAction("TreeMap"   , isTreeMap   (), SLOT(setTreeMap   (bool)));
  addCheckAction("Waffle"    , isWaffle    (), SLOT(setWaffle    (bool)));
  addCheckAction("Summary"   , isSummary   (), SLOT(setSummary   (bool)));
  addCheckAction("Show Label", isShowLabel (), SLOT(setShowLabel (bool)));
  addCheckAction("Show Value", isShowValue (), SLOT(setShowValue (bool)));

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  return true;
}

//---

double
CQChartsPiePlot::
calcMinValue() const
{
  // TODO: use column details ?
  return values_.min();
}

double
CQChartsPiePlot::
calcMaxValue() const
{
  // TODO: use column details ?
  return values_.max();
}

double
CQChartsPiePlot::
calcValueSum() const
{
  if (valueColumns().count() == 1) {
    const auto *columnDetails = this->columnDetails(valueColumns().column());

    bool ok;
    double sum = CQChartsVariant::toReal(columnDetails->sumValue(), ok);
    if (ok) return sum;
  }

  // assume max is sum e.g. values are percent
  return maxValue().realOr(0.0);
}

//---

void
CQChartsPiePlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPlot::write(os, plotVarName, modelVarName, viewVarName);
}

//---

CQChartsPieGroupObj *
CQChartsPiePlot::
createGroupObj(const BBox &bbox, const ColorInd &groupInd,
               const QString &name, const ColorInd &ig) const
{
  return new CQChartsPieGroupObj(this, bbox, groupInd, name, ig);
}

CQChartsPieObj *
CQChartsPiePlot::
createPieObj(const BBox &rect, const QModelIndex &ind, const ColorInd &ig) const
{
  return new CQChartsPieObj(this, rect, ind, ig);
}

//---

bool
CQChartsPiePlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsPiePlot::
execDrawForeground(PaintDevice *device) const
{
  CQChartsPlot::execDrawForeground(device);
}

//---

void
CQChartsPiePlot::
getGroupObjs(int ig, PlotObjs &objs) const
{
  for (const auto &plotObj : plotObjs_) {
    auto *pointObj = dynamic_cast<PieObj *>(plotObj);
    if (! pointObj) continue;

    if (pointObj->is().n == 1 && pointObj->ig().n > 1 && pointObj->ig().i == ig)
      objs.push_back(pointObj);
  }
}

//---

CQChartsPlotCustomControls *
CQChartsPiePlot::
createCustomControls()
{
  auto *controls = new CQChartsPiePlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsPieObj::
CQChartsPieObj(const PiePlot *piePlot, const BBox &rect, const QModelIndex &ind,
               const ColorInd &ig) :
 CQChartsPlotObj(const_cast<PiePlot *>(piePlot), rect, ColorInd(), ig),
 piePlot_(piePlot)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
    setModelInd(ind);
}

QString
CQChartsPieObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig().i).arg(iv().i);
}

QString
CQChartsPieObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  piePlot()->addNoTipColumns(tableTip);

  //---

  auto addColumnRowValue = [&](const Column &column, const QString &header,
                               const QString &value="") {
    if (column.isValid() && tableTip.hasColumn(column))
      return;

    auto value1 = value;

    if (! value1.length()) {
      if (column.isValid()) {
        auto modelInd1 = piePlot_->unnormalizeIndex(modelInd());

        ModelIndex columnInd(piePlot_, modelInd1.row(), column, modelInd1.parent());

        bool ok;

        value1 = piePlot_->modelString(columnInd, ok);
        if (! ok) return;
      }
    }

    if (! value1.length())
      return;

    auto headerStr = header;

    if (column.isValid()) {
      headerStr = piePlot_->columnHeaderName(column, /*tip*/true);

      if (headerStr == "")
        headerStr = header;
    }

    tableTip.addTableRow(headerStr, value1);

    if (column.isValid())
      tableTip.addColumn(column);
  };

  //---

  // get tip values
  QString groupName, labelName, label, valueStr;
  calcTipData(groupName, labelName, label, valueStr);

  // add tip values
  if (groupName.length())
    addColumnRowValue(piePlot_->groupColumn(), groupName);

  addColumnRowValue(piePlot_->labelColumn(), labelName, label);

  if (piePlot_->valueColumns().count() == 1)
    addColumnRowValue(piePlot_->valueColumns().column(), "Value", valueStr);
  else
    tableTip.addTableRow("Value", valueStr);

  if (optRadius())
    addColumnRowValue(piePlot_->radiusColumn(), "Radius", CQChartsUtil::realToString(*optRadius()));

  //---

  // add color column
  addColumnRowValue(piePlot_->colorColumn(), "Color");

  //---

  if (values_.size() > 1)
    tableTip.addTableRow("Count", values_.size());

  //---

  piePlot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

void
CQChartsPieObj::
calcTipData(QString &groupName, QString &labelName, QString &label, QString &valueStr) const
{
  // get group name
  bool hasGroup = (piePlot_->numGroups() > 1 && groupObj());

  if (hasGroup)
    groupName = groupObj()->name();

  // get label
  auto ind = piePlot_->unnormalizeIndex(modelInd());

  label = this->label();

  if (label == "")
    label = piePlot_->calcIndLabel(ind, labelName);

  // get value string
  valueStr = this->valueStr();
}

QString
CQChartsPieObj::
valueStr() const
{
  int valueColumn = modelInd().column();

  QString str;

  if (piePlot_->isValuePercent()) {
    auto dataTotal = groupObj_->values().sum();

    auto percent = 100.0*value()/dataTotal;

    str = QString::asprintf("%.2f%%", percent);
  }
  else
    str = piePlot_->columnStr(Column(valueColumn), value());

  return str;
}

CQChartsArcData
CQChartsPieObj::
arcData() const
{
  double ri, ro, rv;

  getRadii(ri, ro, rv);

  CQChartsArcData arcData;

  arcData.setCenter     (calcCenter());
  arcData.setInnerRadius(ri);
  arcData.setOuterRadius(rv);

  arcData.setAngle1(angle1());
  arcData.setAngle2(angle2());

  return arcData;
}

double
CQChartsPieObj::
value() const
{
  return calcValue(static_cast<CQChartsPieObj::ValueType>(piePlot_->valueType()));
}

void
CQChartsPieObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  PlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
//model->addProperty(path1, this, "selected")->setDesc("Is selected");

//model->addProperty(path1, this, "colorIndex" )->setDesc("Color index");
  model->addProperty(path1, this, "angle1"     )->setDesc("Start angle");
  model->addProperty(path1, this, "angle2"     )->setDesc("End angle");
  model->addProperty(path1, this, "innerRadius")->setDesc("Inner radius");
  model->addProperty(path1, this, "outerRadius")->setDesc("Outer radius");
  model->addProperty(path1, this, "label"      )->setDesc("Label");
//model->addProperty(path1, this, "radius"     )->setDesc("Radius");
  model->addProperty(path1, this, "keyLabel"   )->setDesc("Key label");
  model->addProperty(path1, this, "color"      )->setDesc("Color");
  model->addProperty(path1, this, "exploded"   )->setDesc("Is exploded");
}

bool
CQChartsPieObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  if      (piePlot_->calcTreeMap()) {
    auto bbox = calcTreeMapBBox();

    return bbox.inside(p);
  }
  else if (piePlot_->calcWaffle()) {
    auto bbox = waffleBBox();

    return bbox.inside(p);
  }
  else if (piePlot_->calcPie()) {
    return arcData().inside(p);
  }
  else {
    return false;
  }
}

void
CQChartsPieObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex (inds, piePlot_->labelColumn());
  addColumnsSelectIndex(inds, piePlot_->valueColumns());
}

void
CQChartsPieObj::
addValue(const OptReal &r)
{
  values_.addValue(r);
}

double
CQChartsPieObj::
calcValue(const ValueType &valueType) const
{
  if      (valueType == ValueType::MIN ) return values_.min();
  else if (valueType == ValueType::MAX ) return values_.max();
  else if (valueType == ValueType::MEAN) return values_.mean();
  else if (valueType == ValueType::SUM ) return values_.sum();

  return 0.0;
}

bool
CQChartsPieObj::
calcExploded() const
{
  bool isExploded = this->isExploded();

  if (isSelected() && piePlot_->isExplodeSelected())
    isExploded = true;

  return isExploded;
}

CQChartsGeom::BBox
CQChartsPieObj::
extraFitBBox() const
{
  BBox bbox;

  if (! piePlot_->isTextLabels())
    return bbox;

  if (! label().length())
    return bbox;

  //---

  // get pie center (adjusted if exploded)
  auto c = getAdjustedCenter();

  //---

  auto textOptions = piePlot_->textLabelTextOptions();

  // if full circle always draw text at center
  if (Angle::isCircle(angle1(), angle2())) {
    auto pc = piePlot_->windowToPixel(c);

    auto textOptions1 = textOptions;

    textOptions1.angle = Angle();

    bbox = RotatedTextBoxObj::bbox(const_cast<CQChartsPiePlot *>(piePlot_), pc, label(),
                                   /*isRadial*/false, textOptions1, CQChartsMargin());
  }
  // draw on arc center line
  else {
    // calc label radius
    double ri, ro, rv;

    getRadii(ri, ro, rv);

    double lr = piePlot_->labelRadius();

    double lr1 = lr*ro;
#if 0
    double lr1;

    if (! CMathUtil::isZero(ri))
      lr1 = ri + lr*(ro - ri);
    else
      lr1 = lr*ro;
#endif

    lr1 = std::max(lr1, 0.01);

    //---

    // text angle (mid angle)
    auto ta = Angle::avg(angle1(), angle2());

    //---

    if (piePlot_->numGroups() == 1 && lr > 1.0) {
      auto textOptions1 = textOptions;

      if (piePlot_->isRotatedText())
        textOptions1.angle = ta;

      RotatedTextBoxObj::calcConnectedRadialTextBBox(const_cast<CQChartsPiePlot *>(piePlot_),
                                                     c, rv, lr1, ta, label(), textOptions1,
                                                     piePlot_->textLabelMargin(), bbox);
    }
    else {
      // calc text position
      auto pt = Angle::circlePoint(c, lr1, ta);

      // calc text angle
      Angle angle = ta;

      if (piePlot_->isRotatedText() && ta.cos() < 0.0)
        angle.flipX();

      // calc text box
      auto textOptions1 = textOptions;

      textOptions1.angle = angle;
      textOptions1.align = Qt::AlignHCenter | Qt::AlignVCenter;

      bbox = RotatedTextBoxObj::bbox(const_cast<CQChartsPiePlot *>(piePlot_),
                                     piePlot_->windowToPixel(pt), label(),
                                     piePlot_->isRotatedText(), textOptions1,
                                     piePlot_->textLabelMargin());
    }
  }

  return bbox;
}

void
CQChartsPieObj::
draw(PaintDevice *device) const
{
  if (isHidden())
    return;

  //---

  if      (piePlot_->calcTreeMap())
    drawTreeMap(device);
  else if (piePlot_->calcWaffle())
    drawWaffle(device);
  else if (piePlot_->calcPie())
    drawSegment(device);
}

void
CQChartsPieObj::
drawSegment(PaintDevice *device) const
{
  auto drawPieSlice = [&](const Point &c, double ri, double ro, const Angle &a1, const Angle &a2) {
    bool isInvertX = piePlot()->isInvertX();
    bool isInvertY = piePlot()->isInvertY();

    CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, a1, a2, isInvertX, isInvertY);
  };

  //---

  // get pie center (adjusted if exploded), radii and angles
  auto c = getAdjustedCenter();

//Angle ga = piePlot_->gapAngle().value()/2.0;
  Angle ga { 0.0 };

  auto aa1 = angle1() + ga;
  auto aa2 = angle2() - ga;

  //---

  // draw grid lines (as pie)
  // TODO: if grid lines draw text at max radius ?
  if (piePlot_->isGridLines()) {
    double ri, ro, rv;

    getRadii(ri, ro, rv, /*scaled*/false);

    //---

    PenBrush penBrush;

    piePlot_->setGridLineDataPen(penBrush.pen, ColorInd());

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    Point c = calcCenter();

    drawPieSlice(c, ri, ro, angle1(), angle2()); // no gap
  }

  //---

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  //---

  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw pie slice
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  drawPieSlice(c, ri, ro, aa1, aa2);

  device->resetColorNames();

  //---

  if (piePlot_->isRadiusLabels() && optRadius()) {
    // set text pen and font
    PenBrush tpenBrush;

    auto tc = piePlot_->interpRadiusLabelTextColor(calcColorInd());

    piePlot_->setPen(tpenBrush, PenData(true, tc, piePlot_->radiusLabelTextAlpha()));

    piePlot_->setPainterFont(device, piePlot_->radiusLabelTextFont());

    //---

    // set text options
    auto textOptions = piePlot_->radiusLabelTextOptions(device);

    textOptions.angle = Angle::avg(aa1, aa2);

    textOptions = piePlot_->adjustTextOptions(textOptions);

    //---

    double r = optRadius().value();

    auto str = CQChartsUtil::realToString(r);

    auto pt = Angle::circlePoint(c, (ri + rv)/2.0, textOptions.angle);

    bool labelRight = (pt.x >= c.x);

    if (piePlot_->isInvertX())
      labelRight = ! labelRight;

    if (piePlot_->isRotatedText() && ! labelRight)
      textOptions.angle.flipX();

    device->setPen(tpenBrush.pen);

    CQChartsDrawUtil::drawTextAtPoint(device, pt, str, textOptions, /*centered*/true);
  }

  //---

  bool separated = piePlot_->calcSeparated();

  // draw mouse over as arc next to pie slice
  if (isInside() && piePlot_->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    PenBrush penBrush;

    calcPenBrushInside(penBrush, /*updateState*/false, /*inside*/true);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    double r1;

    if (! separated && piePlot_->numGroups() > 1)
      r1 = ro;
    else
      r1 = rv + piePlot_->insideOffset();

    double r2 = r1 + piePlot_->insideRadius();

    drawPieSlice(c, r1, r2, aa1, aa2);

    //---

    if (! piePlot_->isTextLabels()) {
      auto labelStr = calcTipId();

      //---

      // set text pen
      PenBrush penBrush;

      auto fg = piePlot_->interpTextLabelTextColor(calcColorInd());

      piePlot_->setPen(penBrush, PenData(true, fg, piePlot_->textLabelTextAlpha()));

      //---

      device->setPen(penBrush.pen);

      piePlot_->setPainterFont(device, piePlot_->textLabelTextFont());

      //---

      auto textOptions = piePlot_->textLabelTextOptions();

      textOptions.html = true; // calcTipId is HTML

      textOptions = piePlot_->adjustTextOptions(textOptions);

      Point pt(0.0, 0.0);

      CQChartsDrawUtil::drawTextAtPoint(device, pt, labelStr, textOptions, /*centered*/true);
    }

    //----

    if (piePlot()->isSummary())
      groupObj_->drawDonutInsideText(device, this);
  }
}

void
CQChartsPieObj::
drawTreeMap(PaintDevice *device) const
{
  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw rect
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  auto bbox = calcTreeMapBBox();

  if (bbox.isValid())
    device->drawRect(bbox);

  device->resetColorNames();
}

CQChartsGeom::BBox
CQChartsPieObj::
calcTreeMapBBox() const
{
  auto c = calcCenter();

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  //---

  auto rect = treeMapBBox();

  if (! rect.isValid())
    return BBox();

  BBox rect1(rect.getXMin()*2*ro + bbox.getXMin(), rect.getYMin()*2*ro + bbox.getYMin(),
             rect.getXMax()*2*ro + bbox.getXMin(), rect.getYMax()*2*ro + bbox.getYMin());

  return rect1;
}

void
CQChartsPieObj::
drawWaffle(PaintDevice *device) const
{
  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw polygons
  buildWaffleGeom();

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  if (piePlot()->waffleType() == CQChartsPiePlot::WaffleType::BOX) {
    auto w = piePlot()->lengthPlotWidth (piePlot()->waffleBorder());
    auto h = piePlot()->lengthPlotHeight(piePlot()->waffleBorder());

    for (const auto &bbox : waffleData_.bboxes) {
      auto bbox1 = bbox.adjusted(w, h, -w, -h);

      device->drawRect(bbox1);
    }
  }
  else {
    for (const auto &poly : waffleData_.polygons)
      device->drawPolygon(poly);
  }

  //---

  device->resetColorNames();
}

void
CQChartsPieObj::
buildWaffleGeom() const
{
  using WaffleBarType = CQChartsPieGroupObj::WaffleBarType;

  if (waffleData_.geomBuilt)
    return;

  auto *th = const_cast<CQChartsPieObj *>(this);

  th->waffleData_.geomBuilt = true;

  assert(groupObj_);

  auto bbox = groupObj_->getBBox();

  std::vector<double> xvals, yvals;

  int nc = std::max(groupObj_->waffleCols(), 1);
  int nr = std::max(groupObj_->waffleRows(), 1);

  double dx = bbox.getWidth ()/nc;
  double dy = bbox.getHeight()/nr;

  BBox wbbox;

  th->waffleData_.bboxes.clear();

  for (int i = 0; i < waffleCount(); ++i) {
    int i1 = i + waffleStart();

    int ix, iy;

    if (groupObj_->waffleBarType() != WaffleBarType::HORIZONTAL) {
      ix = i1 % nc;
      iy = i1 / nc;
    }
    else {
      iy = i1 % nr;
      ix = i1 / nr;
    }

    double x1 = bbox.getXMin() + ix*dx;
    double y1 = bbox.getYMin() + iy*dy;
    double x2 = x1 + dx;
    double y2 = y1 + dy;

    xvals.push_back(x1); yvals.push_back(y1);
    xvals.push_back(x2); yvals.push_back(y2);

    wbbox += Point(x1, y1);
    wbbox += Point(x2, y2);

    auto bbox1 = BBox(x1, y1, x2, y2);

    th->waffleData_.bboxes.push_back(bbox1);
  }

  //---

  CXYValsInside xyvals(xvals, yvals);

  for (int iy = 0; iy < xyvals.numYVals() - 1; ++iy) {
    auto ym = (xyvals.yval(iy) + xyvals.yval(iy + 1))/2.0;

    for (int ix = 0; ix < xyvals.numXVals() - 1; ++ix) {
      auto xm = (xyvals.xval(ix) + xyvals.xval(ix + 1))/2.0;

      bool inside = false;

      for (const auto &bbox : waffleData_.bboxes) {
        if (bbox.inside(Point(xm, ym))) {
          inside = true;
          break;
        }
      }

      if (inside)
        xyvals.setInsideVal(ix, iy);
    }
  }

  th->waffleData_.bbox = wbbox;

  //---

  CXYVals::Polygons polygons;

  xyvals.getPolygons(polygons);

  th->waffleData_.polygons.clear();

  for (const auto &poly : polygons) {
    Polygon gpoly;

    for (int i = 0; i < poly.size(); ++i)
      gpoly.addPoint(Point(poly.x[i], poly.y[i]));

    th->waffleData_.polygons.push_back(gpoly);
  }

  //---

  std::set<double> ymvals;

  for (const auto &bbox : waffleData_.bboxes) {
    auto ym = bbox.getYMid();

    ymvals.insert(ym);
  }

  double dty = 0.0;

  if (waffleData_.bbox.isSet())
    dty = waffleData_.bbox.getHeight()/100.0;

  struct YBox {
    double y1 { 0.0 };
    double y2 { 0.0 };
    BBox   bbox;
  };

  std::vector<YBox> yboxes;

  for (const auto &ym : ymvals) {
    YBox ybox;

    ybox.y1 = ym - dty;
    ybox.y2 = ym + dty;

    yboxes.push_back(ybox);
  }

  std::set<double> tyvals;

  for (const auto &bbox : waffleData_.bboxes) {
    auto ym = bbox.getYMid();

    for (auto &ybox : yboxes) {
      if (ym >= ybox.y1 && ym <= ybox.y2)
        ybox.bbox += bbox;
    }
  }

  th->waffleData_.tbbox = BBox();

  for (const auto &ybox : yboxes) {
    if      (! th->waffleData_.tbbox.isSet())
      th->waffleData_.tbbox = ybox.bbox;
    else if (ybox.bbox.getWidth() > waffleData_.tbbox.getWidth())
      th->waffleData_.tbbox = ybox.bbox;
  }
}

void
CQChartsPieObj::
drawFg(PaintDevice *device) const
{
  if (isHidden())
    return;

  //---

  if (! piePlot_->isTextLabels())
    return;

  if (! label().length())
    return;

  //---

  // draw label
  if      (piePlot_->calcTreeMap())
    drawTreeMapLabel(device);
  else if (piePlot_->calcWaffle())
    drawWaffleLabel(device);
  else if (piePlot_->calcPie())
    drawSegmentLabel(device);
}

void
CQChartsPieObj::
drawSegmentLabel(PaintDevice *device) const
{
  // get pie center (adjusted if exploded)
  auto c = getAdjustedCenter();

  // calc label radius
  double ri, ro, rv;

  getRadii(ri, ro, rv);

  //---

  // get display values
  QStringList labels;

  getDrawLabels(labels);
  if (! labels.length()) return;

  //---

  double lr = piePlot_->labelRadius();

  double lr1 = lr*ro;

  lr1 = std::max(lr1, 0.01);

  //---

  // calc label pen
  // TODO: label alpha
  PenBrush penBrush;

  auto bg = fillColor();

  piePlot_->setPen(penBrush, PenData(true, bg, Alpha()));

  //---

  auto textOptions = piePlot_->textLabelTextOptions();

  textOptions = piePlot_->adjustTextOptions(textOptions);

  PenBrush lpenBrush;

  piePlot_->setTextLabelPenBrush(lpenBrush, calcColorInd());

  textOptions.color = piePlot_->interpTextLabelTextColor(calcColorInd());
  textOptions.alpha = piePlot_->textLabelTextAlpha();
  textOptions.font  = piePlot_->textLabelTextFont();

  //---

  auto drawLabels = [&](const Point &p, const Angle angle=Angle(),
                        Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) {
    auto textOptions1 = textOptions;

    textOptions1.angle = angle;
    textOptions1.align = align;

    QString label1, label2;

    if (labels.length() >= 1)
      label1 = labels.at(0);
    if (labels.length() >= 2)
      label2 = labels.at(1);

    BBox drawBBox;

    //bool isRadial = piePlot_->isRotatedText();
    bool isRadial = false;

    RotatedTextBoxObj::draw(device, p, label1, label2, isRadial, lpenBrush,
                            textOptions1, piePlot_->textLabelMargin(),
                            piePlot_->textLabelCornerSize(), piePlot_->textLabelBorderSides(),
                            drawBBox);
  };

  // if full circle draw text at center (non-donut) or top (donut)
  if (Angle::isCircle(angle1(), angle2())) {
    if (piePlot_->calcDonut()) {
      auto pt = Angle::circlePoint(c, lr1, Angle(90));

      drawLabels(pt);
    }
    else
      drawLabels(c);
  }
  // draw on arc center line
  else {
    // calc text angle
    auto ta = Angle::avg(angle1(), angle2());

    // get label
    QString label;

    if      (labels.length() == 1)
      label = labels.at(0);
    else if (labels.length() >= 2)
      label = QString("%1 (%2)").arg(labels.at(0)).arg(labels[1]);

    if (piePlot_->numGroups() == 1 && lr > 1.0) {
      auto textOptions1 = textOptions;

      if (piePlot_->isRotatedText())
        textOptions1.angle = ta;

      RotatedTextBoxObj::drawConnectedRadialText(device, c, rv, lr1, ta, label,
                                                 penBrush.pen, lpenBrush, textOptions1,
                                                 piePlot_->textLabelMargin(),
                                                 piePlot_->textLabelCornerSize(),
                                                 piePlot_->textLabelBorderSides());

      if (piePlot_->isShowBoxes()) {
        BBox bbox;

        RotatedTextBoxObj::calcConnectedRadialTextBBox(const_cast<CQChartsPiePlot *>(piePlot_),
                                                       c, rv, lr1, ta, label, textOptions1,
                                                       piePlot_->textLabelMargin(), bbox);

        piePlot_->drawWindowColorBox(device, bbox);
      }
    }
    else {
      // calc text position
      auto pt = Angle::circlePoint(c, lr1, ta);

      // calc text angle
      auto angle = (piePlot_->isRotatedText() ? ta : Angle());

      bool labelRight = (ta.cos() >= 0.0);

      if (piePlot_->isInvertX())
        labelRight = ! labelRight;

      if (piePlot_->isRotatedText() && ! labelRight)
        angle.flipX();

      // draw label
      auto align = Qt::Alignment(Qt::AlignHCenter | Qt::AlignVCenter);

      drawLabels(pt, angle, align);

      if (piePlot_->isShowBoxes()) {
        // calc text box
        auto textOptions1 = textOptions;

        textOptions1.angle = angle;
        textOptions1.align = align;

        auto bbox = RotatedTextBoxObj::bbox(const_cast<CQChartsPiePlot *>(piePlot_),
                                            piePlot_->windowToPixel(pt), label,
                                            piePlot_->isRotatedText(), textOptions1,
                                            piePlot_->textLabelMargin());

        piePlot_->drawWindowColorBox(device, bbox);
      }
    }
  }
}

void
CQChartsPieObj::
drawTreeMapLabel(PaintDevice *device) const
{
  // get display values
  QStringList labels;

  getDrawLabels(labels);
  if (! labels.length()) return;

  //---

  // calc label pen
  PenBrush penBrush;

  auto tc = piePlot_->interpTextLabelTextColor(calcColorInd());

  piePlot_->setPen(penBrush, PenData(true, tc, piePlot_->textLabelTextAlpha()));

  piePlot_->setPainterFont(device, piePlot_->textLabelTextFont());

  //---

  auto textOptions = piePlot_->textLabelTextOptions();

  textOptions = piePlot_->adjustTextOptions(textOptions);

  auto bbox = calcTreeMapBBox();

  CQChartsDrawUtil::drawTextsInBox(device, bbox, labels, textOptions);
}

void
CQChartsPieObj::
drawWaffleLabel(PaintDevice *device) const
{
  using WaffleBarType = CQChartsPieGroupObj::WaffleBarType;

  if (! waffleData_.bbox.isSet()) return;

  // get display values
  QStringList labels;

  getDrawLabels(labels);
  if (! labels.length()) return;

  //---

  // calc label pen
  PenBrush penBrush;

  auto tc = piePlot_->interpTextLabelTextColor(calcColorInd());

  piePlot_->setPen(penBrush, PenData(true, tc, piePlot_->textLabelTextAlpha()));

  piePlot_->setPainterFont(device, piePlot_->textLabelTextFont());

  //---

  auto textOptions = piePlot_->textLabelTextOptions();

  textOptions = piePlot_->adjustTextOptions(textOptions);

  if (groupObj_->waffleBarType() == WaffleBarType::NONE) {
    device->save();

    device->setClipRect(waffleData_.bbox);

    CQChartsDrawUtil::drawTextsInBox(device, waffleData_.tbbox, labels, textOptions);

    device->restore();
  }
  else {
    auto p = Point(waffleData_.tbbox.getXMin(), waffleData_.tbbox.getYMid());

    textOptions.align = (Qt::AlignLeft | Qt::AlignVCenter);

    CQChartsDrawUtil::drawTextsAtPoint(device, p, labels, textOptions);
  }
}

void
CQChartsPieObj::
getDrawLabels(QStringList &labels) const
{
  // get tip values
  QString groupName, labelName, label, valueStr;
  calcTipData(groupName, labelName, label, valueStr);

  if (label.trimmed().length() && piePlot_->isShowLabel())
    labels.push_back(label);

  if (valueStr.length() && piePlot_->isShowValue())
    labels.push_back(valueStr);
}

void
CQChartsPieObj::
getRadii(double &ri, double &ro, double &rv, bool scaled) const
{
  ri = innerRadius();
  ro = outerRadius();
  rv = valueRadius();

  if (groupObj_) {
    bool separated = piePlot_->calcSeparated();

    auto mapR = [&](double r) {
      double s = (scaled ? radiusScale() : 1.0);

      // if not separated and donut then object has absolute radii
      if (! separated && piePlot_->calcDonut()) {
        return CMathUtil::map(r, innerRadius(), outerRadius(), innerRadius(), s*outerRadius());
      }

      // otherwise radii are relative to group inner/outer radii
      auto ro = groupObj_->innerRadius() + s*(groupObj_->outerRadius() - groupObj_->innerRadius());

      return CMathUtil::map(r, 0.0, 1.0, groupObj_->innerRadius(), ro);
    };

    ri = mapR(ri);
    ro = mapR(ro);
    rv = mapR(rv);
  }
}

CQChartsGeom::BBox
CQChartsPieObj::
getBBox() const
{
  auto c = calcCenter();

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  return BBox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);
}

void
CQChartsPieObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  calcPenBrushInside(penBrush, updateState, /*inside*/false);
}

void
CQChartsPieObj::
calcPenBrushInside(PenBrush &penBrush, bool updateState, bool inside) const
{
  // calc stroke and brush
  auto colorInd = this->calcColorInd();

  auto pc = piePlot_->interpStrokeColor(colorInd);
  auto pa = (inside ? Alpha(0.0) : piePlot_->strokeAlpha());
  auto fc = fillColor();
  auto fa = (inside ? Alpha(0.7) : piePlot_->fillAlpha());

  piePlot_->setPenBrush(penBrush, piePlot_->penData(pc, pa), piePlot_->brushData(fc, fa));

  if (updateState)
    piePlot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsPieObj::
fillColor() const
{
  auto colorInd = this->calcColorInd();

  QColor fc;

  if (piePlot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    if      (color().isValid())
      fc = piePlot_->interpColor(color(), ColorInd());
    else if (piePlot_->fillColor().type() != Color::Type::PALETTE)
      fc = piePlot_->interpColor(piePlot_->fillColor(), iv_);
    else if (ig_.n > 1)
      fc = piePlot_->interpGroupPaletteColor(ig_, iv_);
    else
      fc = piePlot_->interpFillColor(iv_);
  }
  else {
    fc = piePlot_->interpFillColor(colorInd);
  }

  return fc;
}

CQChartsGeom::Point
CQChartsPieObj::
getAdjustedCenter() const
{
  auto c = calcCenter();

  //---

  bool isExploded = calcExploded();

  if (! isExploded)
    return c;

  //---

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  //---

  // get adjusted center (exploded state)
  auto ta = Angle::avg(angle1(), angle2());

  double er = std::max(piePlot_->explodeRadius(), 0.0);

  auto ec = Angle::circlePoint(c, er*rv, ta);

  return ec;
}

CQChartsGeom::Point
CQChartsPieObj::
calcCenter() const
{
  return (groupObj_ ? groupObj_->calcCenter() : Point(0.0, 0.0));
}

double
CQChartsPieObj::
xColorValue(bool relative) const
{
  double a1 = angle1().value();
  double a2 = angle2().value();

  double a21 = std::abs(a2 - a1);

  if (relative)
    a21 /= 360.0;

  return a21;
}

double
CQChartsPieObj::
yColorValue(bool relative) const
{
  return xColorValue(relative);
}

bool
CQChartsPieObj::
isHidden() const
{
  if (piePlot()->numGroups() > 1) {
    if (groupObj_)
      return piePlot()->isGroupHidden(groupObj_->groupInd().i);
    else
      return false;
  }

  if (modelInd().isValid()) {
    auto modelInd1 = piePlot()->unnormalizeIndex(modelInd());

    ModelIndex ind(piePlot(), modelInd1.row(), piePlot()->colorColumn(), modelInd1.parent());

    return piePlot()->isIndexHidden(ind);
  }

  return piePlot()->isSetHidden(colorIndex().i);
}

//------

CQChartsPieGroupObj::
CQChartsPieGroupObj(const PiePlot *piePlot, const BBox &bbox, const ColorInd &groupInd,
                    const QString &name, const ColorInd &ig) :
 CQChartsGroupObj(const_cast<PiePlot *>(piePlot), bbox, ig),
 piePlot_(piePlot), groupInd_(groupInd), name_(name)
{
}

QString
CQChartsPieGroupObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(ig_.i);
}

QString
CQChartsPieGroupObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name" , name());
  tableTip.addTableRow("Count", values().size());

  //---

  //piePlot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

void
CQChartsPieGroupObj::
addObject(CQChartsPieObj *obj)
{
  obj->setGroupObj(this);

  objs_.push_back(obj);
}

int
CQChartsPieGroupObj::
bucketValue(const ModelIndex &ind, double v, QString &label) const
{
  auto *details = plot()->columnDetails(ind.column());
  if (! details) return int(v);

  details->setNumBuckets(piePlot_->numBuckets());

  int bucket = details->bucket(v);

  QVariant vmin, vmax;
  details->bucketRange(bucket, vmin, vmax);

  label = QString("%1-%2").arg(QString::number(vmin.toDouble())).
                           arg(QString::number(vmax.toDouble()));

  return bucket;
}

CQChartsPieObj *
CQChartsPieGroupObj::
lookupObjByName(const QString &name) const
{
  // TODO: use map
  for (const auto &obj : objs_)
    if (obj->label() == name)
      return obj;

  return nullptr;
}

CQChartsPieObj *
CQChartsPieGroupObj::
lookupObjByInd(int ind) const
{
  // TODO: use map
  for (const auto &obj : objs_)
    if (obj->ind() == ind)
      return obj;

  return nullptr;
}

bool
CQChartsPieGroupObj::
inside(const Point &p) const
{
  // separated
  //  . donut or summary has group circle with name and/or summary in center
  // non-separated
  //  . summary has pie segments per group

  bool separated = piePlot_->calcSeparated();

  bool drawn = ((piePlot()->calcDonut() && separated) || piePlot()->isSummary());

  if (! drawn)
    return false;

  //---

  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  double r = p.distanceTo(c);

  if (r < ri || r > ro)
    return false;

  //---

  auto a1 = startAngle();
  auto a2 = endAngle  ();

  if (Angle::isCircle(a1, a2))
    return true;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(CQChartsGeom::pointAngle(c, p));
  a = CMathUtil::normalizeAngle(a);

  double ra1 = a1.value(); ra1 = CMathUtil::normalizeAngle(ra1);
  double ra2 = a2.value(); ra2 = CMathUtil::normalizeAngle(ra2);

  if (ra1 < ra2) {
    // crosses zero
    if (a >= 0.0 && a <= ra1)
      return true;

    if (a <= 360.0 && a >= ra2)
      return true;
  }
  else {
    if (a >= ra2 && a <= ra1)
      return true;
  }

  return false;
}

void
CQChartsPieGroupObj::
draw(PaintDevice *device) const
{
  if (piePlot()->isGroupHidden(groupInd().i))
    return;

  bool separated = piePlot_->calcSeparated();

  //---

  if (numObjs() == 0) {
    if (! separated || piePlot()->isSummary())
      drawEmptyGroup(device);

    return;
  }

  //---

  if      (piePlot()->calcTreeMap()) {
    drawTreeMapHeader(device);
  }
  else if (piePlot()->calcWaffle()) {
    // TODO
  }
  else if (piePlot()->calcPie()) {
    bool drawn = ((piePlot()->calcDonut() && separated) || piePlot()->isSummary());

    if (drawn)
      drawDonut(device);
    else
      drawPieBorder(device);
  }

  //---

  if (separated && piePlot_->calcDumbbell())
    drawDumbbell(device);
}

void
CQChartsPieGroupObj::
drawDonut(PaintDevice *device) const
{
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  bool separated = piePlot_->calcSeparated();

  Angle aa1, aa2;

  if (! separated) {
    if (Angle::isCircle(startAngle(), endAngle())) {
      aa1 = Angle(0.0);
      aa2 = Angle(360.0);
    }
    else {
      double ga = piePlot_->gapAngle().value()/2.0;

      aa1 = Angle(startAngle().value() + ga);
      aa2 = Angle(endAngle  ().value() - ga);
    }
  }
  else {
    aa1 = Angle(0.0);
    aa2 = Angle(360.0);
  }

  //---

  setPenBrush(device);

  //---

  // draw pie slice
  auto drawPieSlice = [&](const Point &c, double ri, double ro, const Angle &a1, const Angle &a2) {
    bool isInvertX = piePlot()->isInvertX();
    bool isInvertY = piePlot()->isInvertY();

    CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, a1, a2, isInvertX, isInvertY);
  };

  drawPieSlice(c, ri, ro, aa1, aa2);
}

void
CQChartsPieGroupObj::
drawTreeMapHeader(PaintDevice *device) const
{
  auto bbox = getBBox();

  auto dx = device->pixelToWindowWidth (16);
  auto dy = device->pixelToWindowHeight(16);

  bbox.expand(-dx, -dy, dx, dy);

  //---

  setPenBrush(device);

  device->drawRect(bbox);
}

void
CQChartsPieGroupObj::
drawPieBorder(PaintDevice *device) const
{
  if (piePlot_->numGroups() <= 1)
    return;

  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  setPenBrush(device);

  //---

  // TODO: config
  auto d = std::min(ro/10.0, device->pixelToWindowWidth(16));

  CQChartsDrawUtil::drawPieSlice(device, c, ro, ro + d, Angle(0), Angle(360.0));
}

void
CQChartsPieGroupObj::
drawEmptyGroup(PaintDevice *device) const
{
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  setPenBrush(device);

  //---

  bool separated = piePlot_->calcSeparated();

  Angle aa1, aa2;

  if (! separated) {
    double ga = piePlot_->gapAngle().value()/2.0;

    aa1 = Angle(startAngle().value() + ga);
    aa2 = Angle(endAngle  ().value() - ga);
  }
  else {
    aa1 = Angle(0.0);
    aa2 = Angle(360.0);
  }

  //---

  // draw pie slice
  auto drawPieSlice = [&](const Point &c, double ri, double ro, const Angle &a1, const Angle &a2) {
    bool isInvertX = piePlot()->isInvertX();
    bool isInvertY = piePlot()->isInvertY();

    CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, a1, a2, isInvertX, isInvertY);
  };

  drawPieSlice(c, ri, ro, aa1, aa2);
}

void
CQChartsPieGroupObj::
drawDumbbell(PaintDevice *device) const
{
  using DumbbellType = CQChartsPiePlot::DumbbellType;

  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  auto dxt = device->pixelToWindowWidth (1);
  auto dyt = device->pixelToWindowHeight(1);

  //---

  // draw range line
  bool showPie = (piePlot_->dumbbellType() == DumbbellType::PIE);

  double x1 = (showPie ? c.x + ro + 24*dxt : -1.0 + 8*dxt);
  double x2 = 1.0 - 8*dxt;

  PenBrush groupPenBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(groupPenBrush, updateState);

  groupPenBrush.pen = QPen(Qt::NoPen);

  CQChartsDrawUtil::setPenBrush(device, groupPenBrush);

  device->drawRect(BBox(Point(x1, c.y - dyt*2), Point(x2, c.y + dyt*2)));

  //--

  // draw symbol for each object on line
  auto symbol = CQChartsSymbol::circle();

  auto symbolSize = Length::pixel(7);

  double vmin = piePlot_->calcMinValue();
  double vmax = piePlot_->calcMaxValue();

  auto valueSum = piePlot_->calcValueSum();

  if (valueSum > 0.0)
    vmax = valueSum;

  piePlot_->setPainterFont(device, piePlot_->groupTextFont());

  CQChartsTextPlacer textPlacer;

  for (const auto &obj : objs_) {
    PenBrush penBrush;

    obj->calcPenBrushInside(penBrush, updateState, obj->isInside());

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    // draw symbol
    double r = CMathUtil::map(obj->value(), vmin, vmax, x1, x2);

    auto p = Point(r, c.y);

    CQChartsDrawUtil::drawSymbol(device, penBrush, symbol, p, symbolSize, /*scale*/true);

    //---

    auto tc = piePlot_->interpGroupTextColor(calcColorInd());

    //---

    // set text pen
    PenBrush tpenBrush;

    piePlot_->setPen(tpenBrush, PenData(true, tc, piePlot_->groupTextAlpha()));

    device->setPen(tpenBrush.pen);

    //---

    // draw text label
    auto valueStr = obj->valueStr();

    auto textOptions = piePlot_->groupTextOptions();

    auto p1 = Point(r, c.y + 8*dxt);

    textOptions.align = Qt::AlignHCenter | Qt::AlignBottom;

    if (piePlot_->isAdjustText())
      textPlacer.addDrawText(device, valueStr, p1, textOptions, p, /*margin*/8);
    else
      CQChartsDrawUtil::drawTextAtPoint(device, p1, valueStr, textOptions);
  }

  // draw placed text
  if (piePlot_->isAdjustText()) {
  //textPlacer.setDebug(true);

    auto clipRect = BBox(x1, c.y - ro, x2, c.y + ro);
  //device->drawRect(clipRect);

    textPlacer.place(clipRect);

    textPlacer.draw(device);
  }
}

void
CQChartsPieGroupObj::
setPenBrush(PaintDevice *device) const
{
  PenBrush groupPenBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(groupPenBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, groupPenBrush);
}

void
CQChartsPieGroupObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto pc = piePlot_->interpGroupStrokeColor(ig_);
  auto bc = piePlot_->interpGroupFillColor  (ig_);

  piePlot_->setPenBrush(penBrush,
    PenData  (true, pc, piePlot_->groupShapeData().stroke()),
    BrushData(true, bc, piePlot_->groupShapeData().fill  ()));

  if (updateState)
    piePlot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsPieGroupObj::
drawFg(PaintDevice *device) const
{
  if (piePlot()->isGroupHidden(groupInd().i))
    return;

  //---

  // draw text at center of donut or summary
  bool drawText = piePlot()->isSummary();

  if (drawText)
    drawDonutText(device);

  //---

  if (piePlot()->isShowDial()) {
    auto c = calcCenter();

    double ri, ro;

    getRadii(ri, ro);

    auto valueSum = calcDataTotal();

    auto valueColumn = piePlot()->valueColumns().column();

    auto *details = piePlot()->columnDetails(valueColumn);

    double currentValue = 0.0;

    if (details) {
      bool ok;
      currentValue = details->currentValue().toDouble(&ok);
    }

    auto dv = (valueSum != 0.0 ? currentValue/valueSum : 0.0);

    auto a1 = piePlot()->startAngle ().value();
    auto da = piePlot()->angleExtent().value();

    auto a = Angle(a1 + dv*da);

    //---

    // calc pen and brush
    PenBrush penBrush;

    bool updateState = device->isInteractive();

    piePlot()->setDialLineDataPen(penBrush.pen, ig_);

    if (updateState)
      piePlot()->updateObjPenBrushState(this, penBrush);

    //---

    // draw line
    CQChartsDrawUtil::setPenBrush(device, penBrush);

    auto p1 = Angle::circlePoint(c, ri, a);
    auto p2 = Angle::circlePoint(c, ro, a);

    device->drawLine(p1, p2);
  }
}

void
CQChartsPieGroupObj::
drawDonutText(PaintDevice *device) const
{
  using DonutValueType = CQChartsPiePlot::DonutValueType;

  //---

  // get text
  QStringList labels;

  if (piePlot_->donutValueType() == DonutValueType::TITLE) {
    labels.push_back(piePlot_->titleStr());
  }
  else {
    if (name().trimmed().length())
      labels.push_back(name());

    QString label, typeLabel;

    if      (piePlot_->donutValueType() == DonutValueType::COUNT) {
      label     = QString::number(values().size());
      typeLabel = "Count";
    }
    else if (piePlot_->donutValueType() == DonutValueType::MIN) {
      label     = QString::number(values().min());
      typeLabel = "Min";
    }
    else if (piePlot_->donutValueType() == DonutValueType::MAX) {
      label     = QString::number(values().max());
      typeLabel = "Max";
    }
    else if (piePlot_->donutValueType() == DonutValueType::MEAN) {
      label     = QString::number(values().mean());
      typeLabel = "Mean";
    }
    else if (piePlot_->donutValueType() == DonutValueType::SUM) {
      label     = QString::number(values().sum());
      typeLabel = "Sum";
    }

    if (label.length()) {
      if (piePlot_->isDonutTypeLabel())
        label += QString(" (%1)").arg(typeLabel);

      labels.push_back(label);
    }
  }

  if (! labels.length())
    return;

  //---

  // get text center and radii
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  bool separated = piePlot_->calcSeparated();

  Point pt;

  // if full circle always draw text at center
  if (separated || Angle::isCircle(startAngle(), endAngle())) {
    pt = c;
  }
  else {
    auto ta = Angle::avg(startAngle(), endAngle());

    pt = Angle::circlePoint(c, CMathUtil::avg(ri, ro), ta);
  }

  //---

  // set text pen and font
  PenBrush penBrush;

  auto tc = piePlot_->interpGroupTextColor(calcColorInd());

  piePlot_->setPen(penBrush, PenData(true, tc, piePlot_->groupTextAlpha()));

  piePlot_->setPainterFont(device, piePlot_->groupTextFont());

  //---

  // set text options
  auto textOptions = piePlot_->groupTextOptions();

  textOptions = piePlot_->adjustTextOptions(textOptions);

  //---

  device->setPen(penBrush.pen);

  if (piePlot_->donutValueType() == DonutValueType::TITLE) {
    auto bbox = getBBox();

    auto textOptions1 = textOptions;

    textOptions1.align     = Qt::AlignHCenter | Qt::AlignVCenter;
    textOptions1.formatted = true;

    CQChartsDrawUtil::drawTextInBox(device, bbox, labels[0], textOptions1);
  }
  else {
    auto textOptions1 = textOptions;

    textOptions1.align = Qt::AlignHCenter | Qt::AlignVCenter;

    CQChartsDrawUtil::drawTextsAtPoint(device, pt, labels, textOptions1);
  }
}

void
CQChartsPieGroupObj::
drawDonutInsideText(PaintDevice *device, const CQChartsPieObj *obj) const
{
  // draw background
  drawDonut(device);

  //---

  // get text
  QString groupName, labelName, label, valueStr;
  obj->calcTipData(groupName, labelName, label, valueStr);

  QStringList labels;

  labels << valueStr;

  //---

  // get text center and radii
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  bool separated = piePlot_->calcSeparated();

  Point pt;

  // if full circle always draw text at center
  if (separated || Angle::isCircle(startAngle(), endAngle())) {
    pt = c;
  }
  else {
    auto ta = Angle::avg(startAngle(), endAngle());

    pt = Angle::circlePoint(c, CMathUtil::avg(ri, ro), ta);
  }

  //---

  // set text pen and font
  PenBrush penBrush;

  auto tc = piePlot_->interpGroupTextColor(calcColorInd());

  piePlot_->setPen(penBrush, PenData(true, tc, piePlot_->groupTextAlpha()));

  piePlot_->setPainterFont(device, piePlot_->groupTextFont());

  //---

  // set text options
  auto textOptions = piePlot_->groupTextOptions();

  textOptions = piePlot_->adjustTextOptions(textOptions);

  //---

  device->setPen(penBrush.pen);

  auto textOptions1 = textOptions;

  textOptions1.align = Qt::AlignHCenter | Qt::AlignVCenter;

  CQChartsDrawUtil::drawTextsAtPoint(device, pt, labels, textOptions1);
}

double
CQChartsPieGroupObj::
calcDataTotal() const
{
  double dataTotal = this->values().sum();

  auto valueSum = piePlot_->calcValueSum();

  if (valueSum > 0.0)
    dataTotal = valueSum;

  return dataTotal;
}

CQChartsGeom::Point
CQChartsPieGroupObj::
calcCenter() const
{
  auto c = center();

  if (piePlot_->calcSeparated() && piePlot_->calcDumbbell()) {
    double ri, ro;

    getRadii(ri, ro);

    c.setX(-1.0 + ro);
  }

  return c;
}

void
CQChartsPieGroupObj::
getRadii(double &ri, double &ro) const
{
  bool separated = piePlot_->calcSeparated();
  bool dumbbell  = piePlot_->calcDumbbell();
  bool donut     = piePlot_->calcDonut();
  bool treemap   = piePlot_->calcTreeMap();

  // empty group
  //  . not separated is whole circle
  //  . separated and summary then 0.0 to scaled plot inner radius
  if (numObjs() == 0) {
    if (! separated) {
      if (! piePlot()->isSummary()) {
        ri = innerRadius();
        ro = outerRadius();
      }
      else {
        ri = 0.0;
        ro = piePlot()->outerRadius();
      }
    }
    else {
      ri = 0.0;
      ro = piePlot()->outerRadius()*outerRadius();
    }
  }
  else {
    // if donut
    //   . not separated then group is from 0.0 to plot inner radius (already set)
    //   . separated then group is from 0.0 to scaled plot inner radius
    if      (donut && ! dumbbell) {
      if (! separated) {
        if (! piePlot()->isSummary()) {
          ri = innerRadius();
          ro = outerRadius();
        }
        else {
          ri = 0.0;
          ro = piePlot()->innerRadius();
        }
      }
      else {
        ri = 0.0;
        ro = piePlot()->innerRadius()*outerRadius();
      }
    }
    else if (treemap && separated) {
      ri = 0.0;
      ro = outerRadius()*piePlot_->outerRadius();
    }
    // not donut then from center to plot outer radius (already set ?)
    else {
      ri = 0.0;

      if (separated)
        ro = outerRadius()*piePlot_->outerRadius();
      else
        ro = piePlot()->outerRadius();
    //ri = std::min(std::max(donut ? piePlot()->innerRadius()*ro : 0.0, 0.0), 1.0);
    }
  }
}

CQChartsGeom::BBox
CQChartsPieGroupObj::
getBBox() const
{
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  return BBox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);
}

QColor
CQChartsPieGroupObj::
bgColor() const
{
  return piePlot_->interpGroupFillColor(ig_);
}

//------

CQChartsPieColorKeyItem::
CQChartsPieColorKeyItem(PiePlot *piePlot, PlotObj *obj) :
 CQChartsColorBoxKeyItem(piePlot, ColorInd(), ColorInd(), ColorInd()), obj_(obj)
{
  setClickable(true);
}

void
CQChartsPieColorKeyItem::
doSelect(SelMod)
{
  auto *piePlot = qobject_cast<PiePlot *>(plot_);

  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  CQChartsPlot::PlotObjs objs;

  if (group) {
    auto is = setIndex();

    piePlot->getGroupObjs(is.i, objs);
  }
  else
    objs.push_back(obj);

  if (objs.empty()) return;

  //---

  piePlot->selectObjs(objs, /*export*/true);

  key_->redraw(/*wait*/ true);
}

QBrush
CQChartsPieColorKeyItem::
fillBrush() const
{
  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  QColor c;

  if      (group)
    c = group->bgColor();
  else if (obj)
    c = obj->fillColor();

  adjustFillColor(c);

  return c;
}

bool
CQChartsPieColorKeyItem::
calcHidden() const
{
  auto *piePlot = qobject_cast<PiePlot *>(plot_);

  auto is = setIndex();

  return piePlot->isSetHidden(is.i);
}

CQChartsUtil::ColorInd
CQChartsPieColorKeyItem::
setIndex() const
{
  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  if      (group)
    return group->groupInd();
  else if (obj)
    return obj->colorIndex();

  return ColorInd();
}

//------

CQChartsPieTextKeyItem::
CQChartsPieTextKeyItem(PiePlot *piePlot, PlotObj *plotObj) :
 CQChartsTextKeyItem(piePlot, "", ColorInd()), obj_(plotObj)
{
  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  if      (group)
    setText(group->name());
  else if (obj)
    setText(obj->keyLabel());
}

QColor
CQChartsPieTextKeyItem::
interpTextColor(const ColorInd &ind) const
{
  auto c = CQChartsTextKeyItem::interpTextColor(ind);

  //adjustFillColor(c);

  return c;
}

bool
CQChartsPieTextKeyItem::
calcHidden() const
{
  auto *piePlot = qobject_cast<PiePlot *>(plot_);

  auto is = setIndex();

  return (piePlot && piePlot->isSetHidden(is.i));
}

CQChartsUtil::ColorInd
CQChartsPieTextKeyItem::
setIndex() const
{
  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  if      (group)
    return group->groupInd();
  else if (obj)
    return obj->colorIndex();

  return ColorInd();
}

//------

CQChartsPiePlotCustomControls::
CQChartsPiePlotCustomControls(CQCharts *charts) :
 CQChartsGroupPlotCustomControls(charts, "pie")
{
}

void
CQChartsPiePlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsPiePlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addGroupColumnWidgets();

  addOptionsWidgets();

  addColorColumnWidgets();

  addKeyList();
}

void
CQChartsPiePlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addNamedColumnWidgets(QStringList() <<
    "values" << "label" << "radius" << "keyLabel", columnsFrame);
}

void
CQChartsPiePlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame", "groupBox",
                                   FrameOpts::makeNoStretch());

  drawTypeCombo_ = CQUtil::makeWidget<CQEnumCombo>("drawTypeCombo");

  drawTypeCombo_->setPropName("drawType");
  drawTypeCombo_->setToolTip("Draw Type");

  addFrameColWidget(optionsFrame_, drawTypeCombo_ );

  separatedCheck_ = createBoolEdit("separated", /*choice*/false);
  donutCheck_     = createBoolEdit("donut"    , /*choice*/false);
  summaryCheck_   = createBoolEdit("summary"  , /*choice*/false);
  dumbbellCheck_  = createBoolEdit("dumbbell" , /*choice*/false);

  addFrameColWidget(optionsFrame_, separatedCheck_);
  addFrameColWidget(optionsFrame_, donutCheck_);
  addFrameColWidget(optionsFrame_, summaryCheck_);
  addFrameColWidget(optionsFrame_, dumbbellCheck_, 1, true);

  showLabelCheck_    = createBoolEdit("showLabel"   , /*choice*/false);
  showValueCheck_    = createBoolEdit("showValue"   , /*choice*/false);
  valuePercentCheck_ = createBoolEdit("valuePercent", /*choice*/false);

  addFrameColWidget(optionsFrame_, showLabelCheck_);
  addFrameColWidget(optionsFrame_, showValueCheck_);
  addFrameColWidget(optionsFrame_, valuePercentCheck_);

  addFrameColWidget(optionsFrame_, CQChartsWidgetUtil::createHStretch());
}

void
CQChartsPiePlotCustomControls::
connectSlots(bool b)
{
  CQUtil::optConnectDisconnect(b,
    drawTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(drawTypeSlot()));
  CQUtil::optConnectDisconnect(b,
    separatedCheck_, SIGNAL(stateChanged(int)), this, SLOT(separatedSlot()));

  CQUtil::optConnectDisconnect(b,
    donutCheck_, SIGNAL(stateChanged(int)), this, SLOT(donutSlot()));
  CQUtil::optConnectDisconnect(b,
    summaryCheck_, SIGNAL(stateChanged(int)), this, SLOT(summarySlot()));
  CQUtil::optConnectDisconnect(b,
    dumbbellCheck_, SIGNAL(stateChanged(int)), this, SLOT(dumbbellSlot()));
  CQUtil::optConnectDisconnect(b,
    showLabelCheck_, SIGNAL(stateChanged(int)), this, SLOT(showLabelSlot()));
  CQUtil::optConnectDisconnect(b,
    showValueCheck_, SIGNAL(stateChanged(int)), this, SLOT(showValueSlot()));
  CQUtil::optConnectDisconnect(b,
    valuePercentCheck_, SIGNAL(stateChanged(int)), this, SLOT(valuePercentSlot()));

  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsPiePlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && piePlot_)
    disconnect(piePlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  piePlot_ = dynamic_cast<PiePlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);

  if (piePlot_)
    connect(piePlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsPiePlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  if (drawTypeCombo_) drawTypeCombo_->setObj(piePlot_);

  if (separatedCheck_) separatedCheck_->setChecked(piePlot_->isSeparated());

  if (donutCheck_    ) donutCheck_    ->setChecked(piePlot_->isDonut   ());
  if (summaryCheck_  ) summaryCheck_  ->setChecked(piePlot_->isSummary ());
  if (dumbbellCheck_ ) dumbbellCheck_ ->setChecked(piePlot_->isDumbbell());

  if (showLabelCheck_   ) showLabelCheck_   ->setChecked(piePlot_->isShowLabel   ());
  if (showValueCheck_   ) showValueCheck_   ->setChecked(piePlot_->isShowValue   ());
  if (valuePercentCheck_) valuePercentCheck_->setChecked(piePlot_->isValuePercent());

  //---

  connectSlots(true);

  CQChartsGroupPlotCustomControls::updateWidgets();
}

void
CQChartsPiePlotCustomControls::
drawTypeSlot()
{
  updateWidgets();
}

void
CQChartsPiePlotCustomControls::
separatedSlot()
{
  piePlot_->setSeparated(separatedCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
donutSlot()
{
  piePlot_->setDonut(donutCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
summarySlot()
{
  piePlot_->setSummary(summaryCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
dumbbellSlot()
{
  piePlot_->setDumbbell(dumbbellCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
showLabelSlot()
{
  piePlot_->setShowLabel(showLabelCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
showValueSlot()
{
  piePlot_->setShowValue(showValueCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
valuePercentSlot()
{
  piePlot_->setValuePercent(valuePercentCheck_->isChecked());
}

CQChartsColor
CQChartsPiePlotCustomControls::
getColorValue()
{
  return piePlot_->fillColor();
}

void
CQChartsPiePlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  piePlot_->setFillColor(c);
}
