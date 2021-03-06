#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsRand.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQEnumCombo.h>
#include <CQChartsWidgetUtil.h>

#include <QMenu>
#include <QHBoxLayout>

CQChartsBoxPlotType::
CQChartsBoxPlotType()
{
}

void
CQChartsBoxPlotType::
addParameters()
{
  startParameterGroup("Box Plot");

  //---

  auto *primaryGroup = startParameterGroup("Raw Values");

  addColumnsParameter("values", "Values", "valueColumns").
    setNumeric().setRequired().setPropPath("columns.raw.values").setTip("Value column(s)");
  addColumnParameter ("name", "Name", "nameColumn").
    setString().setPropPath("columns.raw.name").setTip("Name column");
  addColumnParameter ("set", "Set", "setColumn").
    setPropPath("columns.raw.set").setTip("Set Values");

  endParameterGroup();

  //---

  auto *secondaryGroup = startParameterGroup("Calculated Values");

  addColumnParameter("x"          , "X"           , "xColumn"          ).
    setNumeric().setPropPath("columns.calculated.x").setTip("X Value");
  addColumnParameter("min"        , "Min"         , "minColumn"        ).
    setNumeric().setPropPath("columns.calculated.min").setTip("Min Value");
  addColumnParameter("lowerMedian", "Lower Median", "lowerMedianColumn").
    setNumeric().setPropPath("columns.calculated.lowerMedian").setTip("Lower Median Value");
  addColumnParameter("median"     , "Median"      , "medianColumn"     ).
    setNumeric().setPropPath("columns.calculated.median").setTip("Median Value");
  addColumnParameter("upperMedian", "Upper Median", "upperMedianColumn").
    setNumeric().setPropPath("columns.calculated.upperMedian").setTip("Upper Median Value");
  addColumnParameter("max"        , "Max"         , "maxColumn"        ).
    setNumeric().setPropPath("columns.calculated.max").setTip("Max Value");
  addColumnParameter("outliers"   , "Outliers"    , "outliersColumn"   ).
    setPropPath("columns.calculated.outlier").setTip("Outlier Values");

  endParameterGroup();

  //---

  primaryGroup  ->setType(CQChartsPlotParameterGroup::Type::PRIMARY  );
  secondaryGroup->setType(CQChartsPlotParameterGroup::Type::SECONDARY);

  primaryGroup  ->setOtherGroupId(secondaryGroup->groupId());
  secondaryGroup->setOtherGroupId(primaryGroup  ->groupId());

  //---

  // options
  addBoolParameter("normalized", "Normalized", "normalized").
    setBasic().setTip("Normalize data ranges");

  addEnumParameter("orientation", "Orientation", "orientation").
    addNameValue("HORIZONTAL", int(Qt::Horizontal)).
    addNameValue("VERTICAL"  , int(Qt::Vertical  )).
    setTip("Draw bar orientation");

  addBoolParameter("notched", "Notched", "notched").setTip("Draw notch on bar");

  addBoolParameter("colorBySet", "Color by Set", "colorBySet").setTip("Color by value set");

  addEnumParameter("pointsType", "Points Type", "pointsType").
    addNameValue("NONE"   , int(CQChartsBoxPlot::PointsType::NONE   )).
    addNameValue("JITTER" , int(CQChartsBoxPlot::PointsType::JITTER )).
    addNameValue("STACKED", int(CQChartsBoxPlot::PointsType::STACKED)).
    setTip("Show data points type");

  addBoolParameter("violin"  , "Violin"   , "violin"  ).setTip("Draw distribution outline");
  addBoolParameter("errorBar", "Error Bar", "errorBar").setTip("Error bar (mean +/- std dev)");

  //---

  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsBoxPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto LI  = [](const QString &str) { return CQChartsHtml::Str(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Box Plot").
    h3("Summary").
     p("Draws box and whiskers for the min, max, median and outlier values of the set "
         "of y values for rows with identical x values.").
    h3("Columns").
     p("Values can be supplied using:").
     ul({ LI("Raw Values with X and Y values in " + B("value") + " and " + B("set") + " columns."),
         LI("Calculated Values in the " + B("x") + ", " + B("min") + ", " +
             B("lowerMedian") + ", " + B("median") + ", " + B("upperMedian") + ", " +
             B("max") + " and " + B("outliers") + " columns.") }).
     p("The x value name can be supplied using this " + B("name") + " column.").
    h3("Options").
     p("The outliers values can be shown or hidden..").
     p("Multiple boxes (for each unique x value) can be connected by their y value.").
     p("The box can be drawn vertically or horizontally.").
     p("The y values can be normalized to the range 0-1 so whiskers with different y ranges "
       "can be compared.").
    h3("Draw Styles").
     p("The box can be notched to show the confidence interval around the median.").
     p("The box can be drawn with a violin shape using the distribution curve.").
     p("The box can be drawn as an error bar.").
    h3("Data Points").
     p("The individual points can be displayed (if supplied) using jitter or stacked points.").
     p("The " + B("color") + " column can be used to customize the point color by the "
       "associated data value.").
    h3("Customization").
     p("The box, outlier and data points can be styled (fill and stroke)").
    h3("Limitations").
     p("The plot does not support logarithmic x values.").
    h3("Example").
     p(IMG("images/boxplot.png"));
}

void
CQChartsBoxPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  Columns columns;

  int nc = details->numColumns();

  for (int i = 0; i < nc; ++i) {
    auto *columnDetails = details->columnDetails(Column(i));

    if (columnDetails && columnDetails->isNumeric())
      columns.addColumn(columnDetails->column());
  }

  analyzeModelData.parameterNameColumns["value"] = columns;

  if (columns.count() > 1)
    analyzeModelData.parameterNameBool["normalized"] = true;
}

CQChartsPlot *
CQChartsBoxPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsBoxPlot(view, model);
}

//---

CQChartsBoxPlot::
CQChartsBoxPlot(View *view, const ModelP &model) :
  CQChartsGroupPlot(view, view->charts()->plotType("box"), model),
  CQChartsObjBoxShapeData    <CQChartsBoxPlot>(this),
  CQChartsObjTextData        <CQChartsBoxPlot>(this),
  CQChartsObjWhiskerLineData <CQChartsBoxPlot>(this),
  CQChartsObjOutlierPointData<CQChartsBoxPlot>(this),
  CQChartsObjJitterPointData <CQChartsBoxPlot>(this)
{
}

CQChartsBoxPlot::
~CQChartsBoxPlot()
{
  term();
}

//---

void
CQChartsBoxPlot::
init()
{
  CQChartsGroupPlot::init();

  //---

  NoUpdate noUpdate(this);

  setBoxFillColor(Color(Color::Type::PALETTE));

  setBoxStroked(true);
  setBoxFilled (true);

  setOutlierSymbol(Symbol::circle());
  setOutlierSymbolSize(Length("4px"));
  setOutlierSymbolFilled(true);
  setOutlierSymbolFillColor(Color(Color::Type::PALETTE));

  setJitterSymbol(Symbol::circle());
  setJitterSymbolSize(Length("4px"));
  setJitterSymbolFilled(true);
  setJitterSymbolFillColor(Color(Color::Type::PALETTE));

  addAxes();

  addKey();

  addTitle();

  //---

  addColorMapKey();

  //---

  rawCustomColumns_ = (QStringList() << "values" << "name" << "set");

  calculatedCustomColumns_ = (QStringList() <<
    "x" << "min" << "lowerMedian" << "median" << "upperMedian" << "max" << "outliers");
}

void
CQChartsBoxPlot::
term()
{
  clearRawWhiskers();
}

//------

void
CQChartsBoxPlot::
setValueColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setSetColumn(const Column &c)
{
  CQChartsUtil::testAndSet(setColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

void
CQChartsBoxPlot::
setXColumn(const Column &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setMinColumn(const Column &c)
{
  CQChartsUtil::testAndSet(minColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setLowerMedianColumn(const Column &c)
{
  CQChartsUtil::testAndSet(lowerMedianColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setMedianColumn(const Column &c)
{
  CQChartsUtil::testAndSet(medianColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setUpperMedianColumn(const Column &c)
{
  CQChartsUtil::testAndSet(upperMedianColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setMaxColumn(const Column &c)
{
  CQChartsUtil::testAndSet(maxColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setOutliersColumn(const Column &c)
{
  CQChartsUtil::testAndSet(outliersColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

CQChartsColumn
CQChartsBoxPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "name"       ) c = this->nameColumn();
  else if (name == "set"        ) c = this->setColumn();
  else if (name == "x"          ) c = this->xColumn();
  else if (name == "min"        ) c = this->minColumn();
  else if (name == "lowerMedian") c = this->lowerMedianColumn();
  else if (name == "median"     ) c = this->medianColumn();
  else if (name == "upperMedian") c = this->upperMedianColumn();
  else if (name == "max"        ) c = this->maxColumn();
  else if (name == "outliers"   ) c = this->outliersColumn();
  else                            c = CQChartsGroupPlot::getNamedColumn(name);

  return c;
}

void
CQChartsBoxPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "name"       ) this->setNameColumn(c);
  else if (name == "set"        ) this->setSetColumn(c);
  else if (name == "x"          ) this->setXColumn(c);
  else if (name == "min"        ) this->setMinColumn(c);
  else if (name == "lowerMedian") this->setLowerMedianColumn(c);
  else if (name == "median"     ) this->setMedianColumn(c);
  else if (name == "upperMedian") this->setUpperMedianColumn(c);
  else if (name == "max"        ) this->setMaxColumn(c);
  else if (name == "outliers"   ) this->setOutliersColumn(c);
  else                            CQChartsGroupPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsBoxPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "values") c = this->valueColumns();
  else                  c = CQChartsGroupPlot::getNamedColumns(name);

  return c;
}

void
CQChartsBoxPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "values") this->setValueColumns(c);
  else                  CQChartsGroupPlot::setNamedColumns(name, c);
}

//---

void
CQChartsBoxPlot::
setShowOutliers(bool b)
{
  CQChartsUtil::testAndSet(showOutliers_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setConnected(bool b)
{
  CQChartsUtil::testAndSet(connected_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setBoxWidth(const Length &l)
{
  CQChartsUtil::testAndSet(boxWidth_, l, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setColorBySet(bool b)
{
  CQChartsUtil::testAndSet(colorBySet_, b, [&]() {
    resetSetHidden(); updateRangeAndObjs(); emit customDataChanged();
  } );
}

bool
CQChartsBoxPlot::
canColorBySet() const
{
  return hasSets();
}

//---

void
CQChartsBoxPlot::
setWhiskerRange(double r)
{
  CQChartsUtil::testAndSet(whiskerRange_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setWhiskerExtent(double r)
{
  CQChartsUtil::testAndSet(whiskerExtent_, r, [&]() { drawObjs(); } );
}

//------

void
CQChartsBoxPlot::
setTextMargin(double r)
{
  CQChartsUtil::testAndSet(textMargin_, r, [&]() { drawObjs(); } );
}

//------

void
CQChartsBoxPlot::
setYMargin(double r)
{
  CQChartsUtil::testAndSet(ymargin_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns/raw", "valueColumns", "values", "Value columns");
  addProp("columns/raw", "nameColumn"  , "name"  , "Name column");
  addProp("columns/raw", "setColumn"   , "set"   , "Set column");

  addProp("columns/calculated", "xColumn"          , "x"          ,
      "Precalculated x column");
  addProp("columns/calculated", "minColumn"        , "min"        ,
      "Precalculated min column");
  addProp("columns/calculated", "lowerMedianColumn", "lowerMedian",
      "Precalculated lower median column");
  addProp("columns/calculated", "medianColumn"     , "median"     ,
      "Precalculated median column");
  addProp("columns/calculated", "upperMedianColumn", "upperMedian",
      "Precalculated upper median column");
  addProp("columns/calculated", "maxColumn"        , "max"        ,
      "Precalculated max column");
  addProp("columns/calculated", "outliersColumn"   , "outlier"    ,
      "Precalculated outliers column");

  addGroupingProperties();

  // options
  addProp("options", "connected"  , "connected"  , "Connect across multiple whiskers");
  addProp("options", "orientation", "orientation", "Draw bar direction");
  addProp("options", "normalized" , "normalized" , "Normalize bar ranges to 0-1");

  // margins
  addProp("margins", "ymargin", "ybar", "Margin above/below bar when normalized")->
    setMinValue(0.0).setMaxValue(0.5);

  // jitter
  addProp("points", "pointsType", "type", "Draw jitter or scatter points");

  addSymbolProperties("points/symbol", "jitter", "Points");

  // violin
  addProp("violin", "violin"     , "visible", "Display distribution for box as violin");
  addProp("violin", "violinWidth", "width"  , "Width of violin");
  addProp("violin", "violinBox"  , "box"    , "Draw box as well as violin");

  // error bar
  addProp("errorBar", "errorBar"    , "visible", "Draw error bars");
  addProp("errorBar", "errorBarType", "type"   , "Error bar type (mean +/- std dev)");

  // whisker box
  addProp("box", "whiskerRange", "range"  , "Whisker interquartile range factor")->
    setMinValue(1.0);
  addProp("box", "boxWidth"    , "width"  , "Box width");
  addProp("box", "notched"     , "notched", "Box notched at median");

  // whisker box fill
  addProp("box/fill", "boxFilled", "visible", "Box fill visible");

  addFillProperties("box/fill", "boxFill", "Box");

  // whisker box stroke
  addProp("box/stroke", "boxStroked"   , "visible"   , "Box stroke visible");
  addProp("box/stroke", "boxCornerSize", "cornerSize", "Box corner size");

  addLineProperties("box/stroke", "boxStroke", "Box");

  // whisker line
  addLineProperties("whisker/stroke", "whiskerLines", "Whisker");

  addProp("whisker", "whiskerExtent", "extent", "Box whisker line extent")->
    setMinValue(0.0).setMaxValue(1.0);

  // value labels
  addProp("labels", "textVisible", "visible", "Value labels visible");

  addTextProperties("labels/text", "text", "Value", CQChartsTextOptions::ValueType::CONTRAST |
      CQChartsTextOptions::ValueType::CLIP_LENGTH |
      CQChartsTextOptions::ValueType::CLIP_ELIDE);

  addProp("labels", "textMargin", "margin", "Value text margin in pixels")->setMinValue(0.0);

  // outlier
  addProp("outlier", "showOutliers", "visible", "Outlier points visible");

  addSymbolProperties("outlier/symbol", "outlier", "Outlier");

  // coloring
  addProp("coloring", "colorBySet", "", "Color by value set");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

void
CQChartsBoxPlot::
setHorizontal(bool b)
{
  setOrientation(b ? Qt::Horizontal : Qt::Vertical);
}

void
CQChartsBoxPlot::
setOrientation(const Qt::Orientation &orient)
{
  CQChartsUtil::testAndSet(orientation_, orient, [&]() {
    CQChartsAxis::swap(xAxis(), yAxis());

    updateRangeAndObjs();

    emit customDataChanged();
  } );
}

//---

void
CQChartsBoxPlot::
setNormalized(bool b)
{
  CQChartsUtil::testAndSet(normalized_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

void
CQChartsBoxPlot::
setNotched(bool b)
{
  CQChartsUtil::testAndSet(notched_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

void
CQChartsBoxPlot::
setPointsJitter(bool b)
{
  setPointsType(b ? PointsType::JITTER : PointsType::NONE);
}

void
CQChartsBoxPlot::
setPointsStacked(bool b)
{
  setPointsType(b ? PointsType::STACKED : PointsType::NONE);
}

void
CQChartsBoxPlot::
setPointsType(const PointsType &pointsType)
{
  if (pointsType != pointsType_) {
    pointsType_ = pointsType;

    updateRangeAndObjs();

    emit customDataChanged();
  }
}

//---

void
CQChartsBoxPlot::
setViolin(bool b)
{
  CQChartsUtil::testAndSet(violin_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setViolinWidth(const Length &l)
{
  CQChartsUtil::testAndSet(violinWidth_, l, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setViolinBox(bool b)
{
  CQChartsUtil::testAndSet(violinBox_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setErrorBar(bool b)
{
  CQChartsUtil::testAndSet(errorBar_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsBoxPlot::
setErrorBarType(const ErrorBarType &t)
{
  CQChartsUtil::testAndSet(errorBarType_, t, [&]() { updateRangeAndObjs(); } );
}

//---

CQChartsBoxPlot::ColumnDataType
CQChartsBoxPlot::
calcColumnDataType() const
{
  if      (isPreCalc())
    return ColumnDataType::CALCULATED;
  else if (isRawCalc())
    return ColumnDataType::RAW;
  else
    return defaultColumnDataType_;
}

void
CQChartsBoxPlot::
setCalcColumnDataType(const ColumnDataType &columnDataType)
{
  if (columnDataType != calcColumnDataType()) {
    if      (columnDataType == ColumnDataType::CALCULATED) {
      valueColumns_ = Columns();
      nameColumn_   = Column();
      setColumn_    = Column();
    }
    else if (columnDataType == ColumnDataType::RAW) {
      xColumn_           = Column();
      minColumn_         = Column();
      lowerMedianColumn_ = Column();
      medianColumn_      = Column();
      upperMedianColumn_ = Column();
      maxColumn_         = Column();
      outliersColumn_    = Column();
    }

    updateRangeAndObjs();
  }

  defaultColumnDataType_ = columnDataType;
}

//---

bool
CQChartsBoxPlot::
isPreCalc() const
{
  return (minColumn        ().isValid() &&
          lowerMedianColumn().isValid() &&
          medianColumn     ().isValid() &&
          upperMedianColumn().isValid() &&
          maxColumn        ().isValid());
}

bool
CQChartsBoxPlot::
isRawCalc() const
{
  return valueColumns().isValid();
}

//---

CQChartsGeom::Range
CQChartsBoxPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsBoxPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsBoxPlot *>(this);

  th->clearErrors();

  //---

  Range dataRange;

  //---

  if      (isPreCalc())
    dataRange = updateCalcRange();
  else if (isRawCalc())
    dataRange = updateRawRange();
  else
    dataRange = Range(0, 0, 1, 1);

  return dataRange;
}

void
CQChartsBoxPlot::
postCalcRange()
{
  // x-axis must be integer, y-axis must be real
  auto *xAxis = mappedXAxis();
  auto *yAxis = mappedYAxis();

  xAxis->setValueType     (CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER),
                           /*notify*/false);
  xAxis->setMajorIncrement(1);

  yAxis->setValueType     (CQChartsAxisValueType(CQChartsAxisValueType::Type::REAL),
                           /*notify*/false);
  yAxis->setMajorIncrement(0);
}

// calculate box plot from individual values
CQChartsGeom::Range
CQChartsBoxPlot::
updateRawRange() const
{
  auto *th = const_cast<CQChartsBoxPlot *>(this);

  Range dataRange;

  //---

  // check columns
  bool columnsValid = true;

  // value columns required
  // name, group and set column optional

  if (! checkNumericColumns(valueColumns(), "Values", /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(groupColumn(), "Group")) columnsValid = false;

  if (! checkColumn(setColumn(), "Set", th->setType_))
    columnsValid = false;

  if (! columnsValid)
    return dataRange;

  //---

  auto *xAxis = mappedXAxis();
  auto *yAxis = mappedYAxis();

  th->forceNoYAxis_ = false;

  yAxis->setVisible(true);

  //---

  initGroupData(valueColumns(), nameColumn());

  //---

  updateRawWhiskers();

  //---

  RMinMax xrange;

  auto updateRange = [&](double x, double y) {
    if (isVertical())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  //---

  xAxis->clearTickLabels();

  //---

  bool hasSets   = this->hasSets();
  bool hasGroups = this->hasGroups();

  //---

  int ig = 0;

  for (auto &groupIdWhiskers : groupWhiskers_) {
    int         groupInd      = groupIdWhiskers.first;
    const auto &setWhiskerMap = groupIdWhiskers.second;

    if (! isWhiskersGrouped() || ! isSetHidden(ig)) {
      int is = 0;

      for (auto &setWhiskers : setWhiskerMap) {
        if (isWhiskersGrouped() || ! isSetHidden(is)) {
          int   setId   = setWhiskers.first;
          auto *whisker = setWhiskers.second;

          //---

          QString name;
          bool    allowNoName = false;

          if      (isGroupHeaders()) {
            name = whisker->name();
          }
          else if (hasGroups) {
            name = groupIndName(groupInd);
          }
          else if (hasSets) {
            name = this->setIdName(setId);
          }
          else {
            allowNoName = true;
          }

          whisker->setName(name);

          //---

          int x;

          if      (hasSets && isConnected())
            x = setId;
          else if (hasSets && ! hasGroups)
            x = setId;
          else
            x = ig;

          //---

          if (allowNoName || name.length())
            xAxis->setTickLabel(x, name);

          //---

          double min, max;

          if (isErrorBar()) {
            min = whisker->mean() - whisker->stddev();
            max = whisker->mean() + whisker->stddev();
          }
          else {
            if (isShowOutliers()) {
              min = whisker->vmin();
              max = whisker->vmax();
            }
            else {
              min = whisker->min();
              max = whisker->max();
            }
          }

          if (! isNormalized()) {
            updateRange(x - 0.5, min);
            updateRange(x + 0.5, max);
          }
          else {
            xrange.add(x - 0.5);
            xrange.add(x + 0.5);
          }

          //---

          if (isViolin() && ! isNormalized()) {
            const auto &density = whisker->density();

            updateRange(x, density.xmin1());
            updateRange(x, density.xmax1());
          }
        }

        ++is;
      }
    }

    ++ig;
  }

  //---

  if (isNormalized()) {
    if (xrange.isSet()) {
      updateRange(xrange.min(), 0.0);
      updateRange(xrange.max(), 1.0);
    }
    else {
      updateRange(0.0, 0.0);
      updateRange(1.0, 1.0);
    }
  }

  //---

//xAxis->setColumn(setColumn());
  yAxis->setColumn(valueColumns().column());

  //---

  xAxis->setDefLabel(groupSetColumnName(""));
  yAxis->setDefLabel(valueColumnName   (""));

  //---

  return dataRange;
}

QString
CQChartsBoxPlot::
groupSetColumnName(const QString &def) const
{
  if (numGroupWhiskers() <= 1)
    return "";

  bool ok;

  auto xname = xLabel();

  if (! xname.length() && groupColumn().isValid())
    xname = groupColumnName();

  if (! xname.length() && setColumn().isValid())
    xname = modelHHeaderString(setColumn(), ok);

  if (! xname.length())
    xname = def;

  return xname;
}

QString
CQChartsBoxPlot::
valueColumnName(const QString &def) const
{
  bool ok;

  auto yname = yLabel();

  if (valueColumns().count() == 1 && ! yname.length())
    yname = modelHHeaderString(valueColumns().column(), ok);

  if (! yname.length())
    yname = def;

  return yname;
}

QString
CQChartsBoxPlot::
groupColumnName(const QString &def) const
{
  bool ok;

  QString groupName;

  if (groupColumn().isValid())
    groupName = modelHHeaderString(groupColumn(), ok);

  if (! groupName.length())
    groupName = def;

  return groupName;
}

//---

bool
CQChartsBoxPlot::
hasSets() const
{
  for (const auto &groupIdWhiskers : groupWhiskers_) {
    const auto &setWhiskerMap = groupIdWhiskers.second;

    if (setWhiskerMap.size() > 1)
      return true;
  }

  return false;
}

bool
CQChartsBoxPlot::
hasGroups() const
{
  return (groupWhiskers().size() > 1);
}

QString
CQChartsBoxPlot::
setIdName(int setId) const
{
  return setValueInd_.idName(setId);
}

// calculate box plot from pre-calculated values
CQChartsGeom::Range
CQChartsBoxPlot::
updateCalcRange() const
{
  auto *th = const_cast<CQChartsBoxPlot *>(this);

  Range dataRange;

  //---

  // check columns
  bool columnsValid = true;

  // min, lowerMedian, median, upperMedia, max required (already checked)
  // x, outliers optional (value list)

  if (! checkColumn(xColumn(), "X", th->xType_))
    columnsValid = false;

  if (! checkNumericColumn(minColumn        (), "Min"         )) columnsValid = false;
  if (! checkNumericColumn(lowerMedianColumn(), "Lower Median")) columnsValid = false;
  if (! checkNumericColumn(medianColumn     (), "Median"      )) columnsValid = false;
  if (! checkNumericColumn(upperMedianColumn(), "Upper Median")) columnsValid = false;
  if (! checkNumericColumn(maxColumn        (), "Max"         )) columnsValid = false;
  if (! checkNumericColumn(outliersColumn   (), "Outliers"    )) columnsValid = false;
  if (! checkNumericColumn(idColumn         (), "Id"          )) columnsValid = false;

  if (outliersColumn().isValid()) {
    // TODO: check is numeric array
    if (! checkColumn(outliersColumn(), "Outliers", th->setType_))
      columnsValid = false;
  }

  if (! columnsValid)
    return dataRange;

  //---

  th->xValueInd_.clear();

  //---

  auto *xAxis = mappedXAxis();
  auto *yAxis = mappedYAxis();

  th->forceNoYAxis_ = true;

  yAxis->setVisible(false);

  //---

  RMinMax xrange;

  auto updateRange = [&](double x, double y) {
    if (isVertical())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    using DataList = CQChartsBoxPlot::WhiskerDataList;

   public:
    BoxPlotVisitor(const CQChartsBoxPlot *plot, Range &dataRange, RMinMax &xrange) :
     plot_(plot), dataRange_(dataRange), xrange_(xrange) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addCalcRow(data, dataList_, dataRange_, xrange_);

      return State::OK;
    }

    const DataList &dataList() const { return dataList_; }

   private:
    const CQChartsBoxPlot* plot_ { nullptr };
    Range&                 dataRange_;
    RMinMax&               xrange_;
    DataList               dataList_;
  };

  BoxPlotVisitor boxPlotVisitor(this, dataRange, xrange);

  visitModel(boxPlotVisitor);

  th->whiskerDataList_ = boxPlotVisitor.dataList();

  //---

  if (isNormalized()) {
    if (xrange.isSet()) {
      updateRange(xrange.min(), 0.0);
      updateRange(xrange.max(), 1.0);
    }
    else {
      updateRange(0.0, 0.0);
      updateRange(1.0, 1.0);
    }
  }

  //---

  bool ok;

  auto xname = (xLabel().length() ? xLabel() : modelHHeaderString(xColumn(), ok));

  xAxis->setDefLabel(xname);

  //---

  return dataRange;
}

void
CQChartsBoxPlot::
addCalcRow(const ModelVisitor::VisitData &vdata, WhiskerDataList &dataList,
           Range &dataRange, RMinMax &xrange) const
{
  auto updateRange = [&](double x, double y) {
    if (isVertical())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  auto *th = const_cast<CQChartsBoxPlot *>(this);

  //---

  WhiskerData data;

  data.ind = vdata.parent;

  bool ok;

  //---

  if (xColumn().isValid()) {
    ModelIndex xInd(th, vdata.row, xColumn(), vdata.parent);

    // x column can be string or real
    if (xType_ == ColumnType::STRING) {
      auto xname = modelString(xInd, ok);

      if (ok)
        data.name = xname;
    }

    data.x = modelNumericValue(xInd, ok);

    if (! ok) {
      th->addDataError(xInd, "Invalid x value");
      return;
    }
  }
  else {
    data.x = vdata.row;
  }

  //---

  ModelIndex minInd        (th, vdata.row, minColumn        (), vdata.parent);
  ModelIndex lowerMedianInd(th, vdata.row, lowerMedianColumn(), vdata.parent);
  ModelIndex medianInd     (th, vdata.row, medianColumn     (), vdata.parent);
  ModelIndex upperMedianInd(th, vdata.row, upperMedianColumn(), vdata.parent);
  ModelIndex maxInd        (th, vdata.row, maxColumn        (), vdata.parent);

  data.statData.min         = modelReal(minInd        , ok);
  if (! ok) { th->addDataError(minInd        , "Invalid min value"); return; }
  data.statData.lowerMedian = modelReal(lowerMedianInd, ok);
  if (! ok) { th->addDataError(lowerMedianInd, "Invalid lower median value"); return; }
  data.statData.median      = modelReal(medianInd     , ok);
  if (! ok) { th->addDataError(medianInd     , "Invalid median value"); return; }
  data.statData.upperMedian = modelReal(upperMedianInd, ok);
  if (! ok) { th->addDataError(upperMedianInd, "Invalid upper median value"); return; }
  data.statData.max         = modelReal(maxInd        , ok);
  if (! ok) { th->addDataError(maxInd        , "Invalid max value"); return; }

  data.dataMin = data.statData.min;
  data.dataMax = data.statData.max;

  if (isShowOutliers() && outliersColumn().isValid()) {
    data.outliersInd = ModelIndex(th, vdata.row, outliersColumn(), vdata.parent);

    data.outliers = modelReals(data.outliersInd, ok);

    if (! ok) {
      th->addDataError(data.outliersInd, "Invalid outlier real values");
      return;
    }

    for (auto &o : data.outliers) {
      data.dataMin = std::min(data.dataMin, o);
      data.dataMax = std::max(data.dataMax, o);
    }
  }

  if (! isNormalized()) {
    updateRange(data.x - 0.5, data.statData.min);
    updateRange(data.x + 0.5, data.statData.max);
  }
  else {
    xrange.add(data.x - 0.5);
    xrange.add(data.x + 0.5);
  }

  //---

  bool nameValid = true;

  if (! data.name.length()) {
    if (idColumn().isValid()) {
      ModelIndex idInd(th, vdata.row, idColumn(), vdata.parent);

      auto id = modelString(idInd, ok);

      if (ok)
        data.name = id;
      else
        th->addDataError(idInd, "Invalid id value");
    }
  }

  if (! data.name.length()) {
    data.name = modelVHeaderString(vdata.row, Qt::Vertical, ok); // ignore fail

    if (! data.name.length()) {
      data.name = QString::number(vdata.row);
      nameValid = false;
    }
  }

  if (nameValid)
    xAxis_->setTickLabel(int(data.x), data.name);

  //---

  dataList.push_back(std::move(data));
}

void
CQChartsBoxPlot::
updateRawWhiskers() const
{
  auto *th = const_cast<CQChartsBoxPlot *>(this);

  th->clearRawWhiskers();

  //---

  // set data type
  th->setValueInd_.clear();

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    BoxPlotVisitor(const CQChartsBoxPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRawWhiskerRow(data);

      return State::OK;
    }

   private:
    const CQChartsBoxPlot *plot_ { nullptr };
  };

  BoxPlotVisitor boxPlotVisitor(this);

  visitModel(boxPlotVisitor);

  //---

  th->isWhiskersGrouped_ = (numGroupWhiskers() > 1);

  //---

  for (auto &groupIdWhiskers : groupWhiskers_) {
    const auto &setWhiskerMap = groupIdWhiskers.second;

    for (auto &setWhiskers : setWhiskerMap)
      setWhiskers.second->init();
  }
}

void
CQChartsBoxPlot::
clearRawWhiskers()
{
  for (auto &groupWhisker : groupWhiskers_) {
    const auto &setWhiskerMap = groupWhisker.second;

    for (auto &setWhisker : setWhiskerMap)
      delete setWhisker.second;
  }

  groupWhiskers_.clear();
}

void
CQChartsBoxPlot::
addRawWhiskerRow(const ModelVisitor::VisitData &vdata) const
{
  auto *th = const_cast<CQChartsBoxPlot *>(this);

  // get value set id
  int      setId = -1;
  QVariant setVal;

  if (setColumn().isValid()) {
    ModelIndex setInd(th, vdata.row, setColumn(), vdata.parent);

    bool ok1;

    setVal = modelHierValue(setInd, ok1);

    if (! ok1) {
      th->addDataError(setInd, "Invalid set value");
      return;
    }

    setId = th->setValueInd_.calcId(setVal, setType_);
  }

  //---

  for (const auto &valueColumn : valueColumns()) {
    ModelIndex ind(th, vdata.row, valueColumn, vdata.parent);

    // get group
    int groupInd = rowGroupInd(ind);

    //---

    // add value to set
    bool ok2;

    double value = modelReal(ind, ok2);

    if (! ok2) {
      th->addDataError(ind, "Invalid value real");
      continue;
    }

    if (CMathUtil::isNaN(value))
      continue;

    auto yind  = modelIndex(ind);
    auto yind1 = normalizeIndex(yind);

    CQChartsBoxPlotValue wv(value, yind1);

    auto pg = groupWhiskers_.find(groupInd);

    if (pg == groupWhiskers_.end()) {
      auto pg1 = th->groupWhiskers_.find(groupInd);

      if (pg1 == th->groupWhiskers_.end())
        pg1 = th->groupWhiskers_.insert(pg1,
          GroupSetWhiskerMap::value_type(groupInd, SetWhiskerMap()));

      pg = groupWhiskers_.find(groupInd);
    }

    const auto &setWhiskerMap = (*pg).second;

    auto ps = setWhiskerMap.find(setId);

    if (ps == setWhiskerMap.end()) {
      auto &setWhiskerMap1 = const_cast<SetWhiskerMap &>(setWhiskerMap);

      auto ps1 = setWhiskerMap1.find(setId);

      if (ps1 == setWhiskerMap1.end()) {
        auto *whisker = new Whisker;

        whisker->setRange(whiskerRange());

        QString name;
        bool    ok = false;

        if      (isGroupHeaders()) {
          name = modelHHeaderString(valueColumn, ok);
        }
        else if (setColumn().isValid()) {
          ok = CQChartsVariant::toString(setVal, name);
        }

        if (ok && name.length())
          whisker->setName(name);

        ps1 = setWhiskerMap1.insert(ps1, SetWhiskerMap::value_type(setId, whisker));
      }

      ps = setWhiskerMap.find(setId);
    }

    (*ps).second->addValue(wv);
  }
}

CQChartsAxis *
CQChartsBoxPlot::
mappedXAxis() const
{
  return (isVertical() ? xAxis() : yAxis());
}

CQChartsAxis *
CQChartsBoxPlot::
mappedYAxis() const
{
  return (isVertical() ? yAxis() : xAxis());
}

CQChartsGeom::BBox
CQChartsBoxPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsBoxPlot::calcExtraFitBBox");

  BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    auto *boxObj = dynamic_cast<CQChartsBoxPlotWhiskerObj *>(plotObj);

    if (boxObj)
      bbox += boxObj->extraFitBBox();
  }

  return bbox;
}

//------

bool
CQChartsBoxPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsBoxPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  bool rc = false;

  if      (isPreCalc())
    rc = initCalcObjs(objs);
  else if (isRawCalc())
    rc = initRawObjs(objs);

  //---

  return rc;
}

bool
CQChartsBoxPlot::
initRawObjs(PlotObjs &objs) const
{
  bool hasSets   = this->hasSets();
  bool hasGroups = this->hasGroups();

  double bw2 = lengthPlotSize(boxWidth   (), isVertical())/2.0;
  double vw2 = lengthPlotSize(violinWidth(), isVertical())/2.0;

  //---

  int ig = 0;
  int ng = numGroupWhiskers();

  for (const auto &groupIdWhiskers : this->groupWhiskers()) {
    int         groupInd      = groupIdWhiskers.first;
    const auto &setWhiskerMap = groupIdWhiskers.second;

    if (! isConnected()) {
      int is = 0;
      int ns = setWhiskerMap.size();

      double sf = (ns > 1 ? 1.0/ns : 1.0);

      for (const auto &setWhiskers : setWhiskerMap) {
        bool hidden = (isWhiskersGrouped() ? isSetHidden(ig) : isSetHidden(is));
        if (hidden) { ++is; continue; }

        int   setId   = setWhiskers.first;
        auto *whisker = setWhiskers.second;

        if (whisker->values().empty())
          continue;

#if 0
        if (whisker->lowerMedian() >= whisker->upperMedian())
          continue;
#endif

        //----

        double pos = ig;
        double sbw = (isViolin() ? vw2 : bw2);

        if      (hasSets && hasGroups) {
          pos += (is + 1.0)/(ns + 1.0) - 0.5;
          sbw *= sf;
        }
        else if (hasSets)
          pos = setId;

        //---

        // create whisker object
        BBox rect;

        if (! isNormalized())
          rect = CQChartsGeom::makeDirBBox(/*flipped*/ isHorizontal(),
                   pos - sbw, whisker->lowerMedian(), pos + sbw, whisker->upperMedian());
        else
          rect = CQChartsGeom::makeDirBBox(/*flipped*/ isHorizontal(),
                   pos - sbw, 0.0, pos + sbw, 1.0);

        auto *boxObj = createWhiskerObj(rect, setId, groupInd, whisker,
                                        ColorInd(is, ns), ColorInd(ig, ng));

        objs.push_back(boxObj);

        //---

        if (isShowOutliers()) {
          double ymargin = this->ymargin();

          double osx, osy;

          plotSymbolSize(outlierSymbolSize(), osx, osy);

          for (auto &o : whisker->outliers()) {
            auto ovalue = whisker->value(o);

            auto orvalue = double(ovalue);

            BBox rect;

            if (! isNormalized()) {
              if (isVertical())
                rect = BBox(pos - osx, orvalue - osy, pos + osx, orvalue + osy);
              else
                rect = BBox(orvalue - osx, pos - osy, orvalue + osx, pos + osy);
            }
            else {
              double orvalue1 =
                CMathUtil::map(orvalue, whisker->vmin(), whisker->vmax(), ymargin, 1.0 - ymargin);

              if (isVertical())
                rect = BBox(pos - osx, orvalue1 - osy, pos + osx, orvalue1 + osy);
              else
                rect = BBox(orvalue1 - osx, pos - osy, orvalue1 + osx, pos + osy);
            }

            auto *outlierObj = createOutlierObj(rect, setId, groupInd, whisker,
                                                ColorInd(is, ns), ColorInd(ig, ng), o);

            Color pointColor;

            if (colorColumn().isValid()) {
              if (! colorColumnColor(ovalue.ind.row(), ovalue.ind.parent(), pointColor))
                pointColor = Color(Color::Type::NONE);
            }

            if (pointColor.isValid())
              outlierObj->setColor(pointColor);

            objs.push_back(outlierObj);
          }
        }

        //---

        // create jitter or stacked points
        if      (isPointsJitter()) {
          addJitterPoints(groupInd, setId, pos, whisker, ColorInd(is, ns), ColorInd(ig, ng), objs);
        }
        else if (isPointsStacked()) {
          addStackedPoints(groupInd, setId, pos, whisker, ColorInd(is, ns), ColorInd(ig, ng), objs);
        }

        //---

        ++is;
      }
    }
    else {
      bool hidden = (isWhiskersGrouped() ? isSetHidden(ig) : false);
      if (hidden) { continue; }

      auto rect = getDataRange();

      auto *connectedObj = createConnectedObj(rect, groupInd, ColorInd(ig, ng));

      objs.push_back(connectedObj);
    }

    //---

    ++ig;
  }

  //---

  return true;
}

void
CQChartsBoxPlot::
addJitterPoints(int groupInd, int setId, double pos, const Whisker *whisker,
                const ColorInd &is, const ColorInd &ig, PlotObjs &objs) const
{
  double vw2 = lengthPlotSize(violinWidth(), isVertical())/2.0;

  const auto &density = whisker->density();

  //double ymin = density.ymin1();
  //double ymax = density.ymax1();
  double ymin = density.ymin();
  double ymax = density.ymax();

  CQChartsRand::RealInRange rand(-vw2, vw2);

  int nv = whisker->numValues();

  for (int iv = 0; iv < nv; ++iv) {
    const auto &value = whisker->value(iv);

    double d = rand.gen();

    //double yv = density.yval(value)/(ymax - ymin);
    double yv = density.eval(value)/(ymax - ymin);

    double x = pos + yv*d;
    double y = value.value;

    double y1 = (isNormalized() ? whisker->normalize(y, isShowOutliers()) : y);

    Point pos1;
    BBox  rect;

    if (isVertical()) {
      pos1 = Point(x, y1);

      if (! isNormalized())
        rect = BBox(x - 0.1, y1 - 0.1, x + 0.1, y1 + 0.1);
      else
        rect = BBox(x - 0.01, y1 - 0.01, x + 0.01, y1 + 0.01);
    }
    else {
      pos1 = Point(y1, x);

      if (! isNormalized())
        rect = BBox(y1 - 0.1, x - 0.1, y1 + 0.1, x + 0.1);
      else
        rect = BBox(y1 - 0.01, x - 0.01, y1 + 0.01, x + 0.01);
    }

    auto *pointObj = createPointObj(rect, setId, groupInd, pos1, value.ind,
                                    is, ig, ColorInd(iv, nv));

    Color pointColor;

    if (colorColumn().isValid()) {
      if (! colorColumnColor(value.ind.row(), value.ind.parent(), pointColor))
        pointColor = Color(Color::Type::NONE);
    }

    if (pointColor.isValid())
      pointObj->setColor(pointColor);

    objs.push_back(pointObj);
  }
}

void
CQChartsBoxPlot::
addStackedPoints(int groupInd, int setId, double pos, const Whisker *whisker,
                 const ColorInd &is, const ColorInd &ig, PlotObjs &objs) const
{
  using Rects    = std::vector<BBox>;
  using PosRects = std::map<int, Rects>;

  PosRects posRects;

  auto placePosRect = [&](int pos, const BBox &rect) {
    auto &rects = posRects[pos];

    for (auto &r : rects) {
      if (r.intersect(rect))
        return false;
    }

    rects.push_back(rect);

    return true;
  };

  auto placeRect = [&](const BBox &rect, BBox &prect) {
    if (placePosRect(0, rect))
      return false;

    double w = (isVertical() ? rect.getWidth() : rect.getHeight());

    int d = 1;

    while (true) {
      auto rect1 = rect;

      if (isVertical())
        rect1.moveBy(Point(-d*w, 0.0));
      else
        rect1.moveBy(Point(0.0, -d*w));

      if (placePosRect(-d, rect1)) {
        prect = rect1;
        return true;
      }

      auto rect2 = rect;

      if (isVertical())
        rect2.moveBy(Point(d*w, 0.0));
      else
        rect2.moveBy(Point(0.0, d*w));

      if (placePosRect(d, rect2)) {
        prect = rect2;
        return true;
      }

      ++d;
    }
  };

  std::map<double, int> valueCount;

  int nv = whisker->numValues();

  for (int iv = 0; iv < nv; ++iv) {
    const auto &value = whisker->value(iv);

    valueCount[value.value]++; // TODO: tolerance
  }

  int maxV = 0;

  for (const auto &vc : valueCount)
    maxV = std::max(maxV, vc.second);

  double ds = (maxV > 0 ? 1.0/maxV : 0.0);

  double sx, sy;

  plotSymbolSize(jitterSymbolSize(), sx, sy);

  if (isVertical()) {
    if (ds < sx) {
      double s = (sx > 0.0 ? ds/sx : 1.0);

      sx *= s;
      sy *= s;
    }
  }
  else {
    if (ds < sy) {
      double s = (sy > 0.0 ? ds/sy : 1.0);

      sx *= s;
      sy *= s;
    }
  }

  for (int iv = 0; iv < nv; ++iv) {
    const auto &value = whisker->value(iv);

    double x = pos;
    double y = value.value;

    double y1 = (isNormalized() ? whisker->normalize(y, isShowOutliers()) : y);

    Point pos;
    BBox  rect;

    if (isVertical())
      pos = Point(x, y1);
    else
      pos = Point(y1, x);

    rect = BBox(pos.x - sx, pos.y - sy, pos.x + sx, pos.y + sy);

    CQChartsBoxPlotPointObj *pointObj = nullptr;

    BBox prect;

    if (placeRect(rect, prect)) {
      auto ppos = pos;

      if (isVertical())
        ppos.setX(prect.getXMid());
      else
        ppos.setY(prect.getYMid());

      pointObj = createPointObj(prect, setId, groupInd, ppos, value.ind,
                                is, ig, ColorInd(iv, nv));
    }
    else {
      pointObj = createPointObj(rect, setId, groupInd, pos, value.ind,
                                is, ig, ColorInd(iv, nv));
    }

    Color pointColor;

    if (colorColumn().isValid()) {
      if (! colorColumnColor(value.ind.row(), value.ind.parent(), pointColor))
        pointColor = Color(Color::Type::NONE);
    }

    if (pointColor.isValid())
      pointObj->setColor(pointColor);

    objs.push_back(pointObj);
  }
}

bool
CQChartsBoxPlot::
initCalcObjs(PlotObjs &objs) const
{
  int is = 0;
  int ns = whiskerDataList_.size();

  //int ipos = 0;

  double bw = 0.1;

  for (const auto &whiskerData : whiskerDataList_) {
    double pos = whiskerData.x;

    BBox rect;

    if (! isNormalized())
      rect = CQChartsGeom::makeDirBBox(/*flipped*/isHorizontal(),
               pos - bw, whiskerData.statData.lowerMedian,
               pos + bw, whiskerData.statData.upperMedian);
    else
      rect = CQChartsGeom::makeDirBBox(/*flipped*/isHorizontal(),
               pos - bw, 0.0, pos + bw, 1.0);

    auto *boxObj = createDataObj(rect, whiskerData, ColorInd(is, ns));

    objs.push_back(boxObj);

    //++ipos;

    //---

    if (isShowOutliers()) {
      double ymargin = this->ymargin();

      double osx, osy;

      plotSymbolSize(outlierSymbolSize(), osx, osy);

      int io = 0;

      for (auto &ovalue : whiskerData.outliers) {
        BBox rect;

        if (! isNormalized()) {
          if (isVertical())
            rect = BBox(pos - osx, ovalue - osy, pos + osx, ovalue + osy);
          else
            rect = BBox(ovalue - osx, pos - osy, ovalue + osx, pos + osy);
        }
        else {
          double ovalue1 =
            CMathUtil::map(ovalue,
                           whiskerData.statData.lowerMedian, whiskerData.statData.upperMedian,
                           ymargin, 1.0 - ymargin);

          if (isVertical())
            rect = BBox(pos - osx, ovalue1 - osy, pos + osx, ovalue1 + osy);
          else
            rect = BBox(ovalue1 - osx, pos - osy, ovalue1 + osx, pos + osy);
        }

        auto *outlierObj = createOutlierObj(rect, -1, -1, nullptr,
                                            ColorInd(is, ns), ColorInd(), io);

        Color pointColor;

        if (colorColumn().isValid()) {
          const auto &ind = whiskerData.outliersInd;

          if (! colorColumnColor(ind.row(), ind.parent(), pointColor))
            pointColor = Color(Color::Type::NONE);
        }

        if (pointColor.isValid())
          outlierObj->setColor(pointColor);

        objs.push_back(outlierObj);

        ++io;
      }
    }

    //---

    ++is;
  }

  return true;
}

void
CQChartsBoxPlot::
addKeyItems(PlotKey *key)
{
  // start at next row (vertical) or next column (horizontal) from previous key
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  int ng = numGroupWhiskers();

  // if has groups
  if      (ng > 1) {
    // if color by set add key item per set
    if (hasSets() && isColorBySet()) {
      auto pg = this->groupWhiskers().begin();

      int         groupInd      = (*pg).first;
      const auto &setWhiskerMap = (*pg).second;

      auto groupName = groupIndName(groupInd);

      int is = 0;
      int ns = setWhiskerMap.size();

      for (const auto &setWhiskers : setWhiskerMap) {
        int   setId   = setWhiskers.first;
      //auto *whisker = setWhiskers.second;

        auto setName = setIdName(setId);

        ColorInd sc(is, ns), gc;

        auto *colorItem = new CQChartsBoxKeyColor(this, sc, gc);
        auto *textItem  = new CQChartsBoxKeyText (this, setName, sc, gc);

        auto *groupItem = new CQChartsKeyItemGroup(this);

        groupItem->addRowItems(colorItem, textItem);

        key->addItem(groupItem, row, col);

        key->nextRowCol(row, col);

        ++is;
      }
    }
    // if not color by set add key item per group
    else {
      int ig = 0;

      for (const auto &groupIdWhiskers : this->groupWhiskers()) {
        int groupInd = groupIdWhiskers.first;

        auto groupName = groupIndName(groupInd);

        ColorInd sc, gc(ig, ng);

        auto *colorItem = new CQChartsBoxKeyColor(this, sc, gc);
        auto *textItem  = new CQChartsBoxKeyText (this, groupName, sc, gc);

        auto *groupItem = new CQChartsKeyItemGroup(this);

        groupItem->addRowItems(colorItem, textItem);

        key->addItem(groupItem, row, col);

        key->nextRowCol(row, col);

        ++ig;
      }
    }
  }
  // if single group then add key per set
  else if (ng > 0) {
    auto pg = this->groupWhiskers().begin();

    int         groupInd      = (*pg).first;
    const auto &setWhiskerMap = (*pg).second;

    auto groupName = groupIndName(groupInd);

    int is = 0;
    int ns = setWhiskerMap.size();

    for (const auto &setWhiskers : setWhiskerMap) {
    //int   setId   = setWhiskers.first;
      auto *whisker = setWhiskers.second;

      auto name = whisker->name();

      ColorInd sc(is, ns), gc;

      auto *colorItem = new CQChartsBoxKeyColor(this, sc, gc);
      auto *textItem  = new CQChartsBoxKeyText (this, name, sc, gc);

      auto *groupItem = new CQChartsKeyItemGroup(this);

      groupItem->addRowItems(colorItem, textItem);

      key->addItem(groupItem, row, col);

      key->nextRowCol(row, col);

      ++is;
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

bool
CQChartsBoxPlot::
probe(ProbeData &probeData) const
{
  const auto &dataRange = this->dataRange();

  if (! dataRange.isSet())
    return false;

  if (isVertical()) {
    probeData.p.x =
      std::min(std::max(probeData.p.x, dataRange.xmin() + 0.5), dataRange.xmax() - 0.5);
    probeData.p.x = std::round(probeData.p.x);
  }
  else {
    probeData.p.y =
      std::min(std::max(probeData.p.y, dataRange.ymin() + 0.5), dataRange.ymax() - 0.5);
    probeData.p.y = std::round(probeData.p.y);
  }

  return true;
}

//------

bool
CQChartsBoxPlot::
addMenuItems(QMenu *menu)
{
  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) {
    return addMenuCheckedAction(menu, name, isSet, slot);
  };

  //---

  menu->addSeparator();

  (void) addCheckedAction("Horizontal", isHorizontal(), SLOT(setHorizontal(bool)));
  (void) addCheckedAction("Normalized", isNormalized(), SLOT(setNormalized(bool)));

  //---

  // following items only allowed if we have individual data points
  if (isRawCalc()) {
    auto *pointsMenu = new QMenu("Points", menu);

    (void) addMenuCheckedAction(pointsMenu, "Jitter" , isPointsJitter(),
                                SLOT(setPointsJitter(bool)));
    (void) addMenuCheckedAction(pointsMenu, "Stacked", isPointsStacked(),
                                SLOT(setPointsStacked(bool)));

    menu->addMenu(pointsMenu);

    //---

    (void) addCheckedAction("Notched"  , isNotched() , SLOT(setNotched(bool)));
    (void) addCheckedAction("Violin"   , isViolin()  , SLOT(setViolin(bool)));
    (void) addCheckedAction("Error Bar", isErrorBar(), SLOT(setErrorBar(bool)));
  }

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  return true;
}

bool
CQChartsBoxPlot::
hasXAxis() const
{
  if (isHorizontal() && forceNoYAxis_)
    return false;

  return CQChartsPlot::hasXAxis();
}

bool
CQChartsBoxPlot::
hasYAxis() const
{
  if (isVertical() && forceNoYAxis_)
    return false;

  return CQChartsPlot::hasYAxis();
}

//----

CQChartsBoxPlotWhiskerObj *
CQChartsBoxPlot::
createWhiskerObj(const BBox &rect, int setId, int groupInd, const Whisker *whisker,
                 const ColorInd &is, const ColorInd &ig) const
{
  return new CQChartsBoxPlotWhiskerObj(this, rect, setId, groupInd, whisker, is, ig);
}

CQChartsBoxPlotOutlierObj *
CQChartsBoxPlot::
createOutlierObj(const BBox &rect, int setId, int groupInd, const Whisker *whisker,
                 const ColorInd &is, const ColorInd &ig, int io) const
{
  return new CQChartsBoxPlotOutlierObj(this, rect, setId, groupInd, whisker, is, ig, io);
}

CQChartsBoxPlotDataObj *
CQChartsBoxPlot::
createDataObj(const BBox &rect, const WhiskerData &data, const ColorInd &is) const
{
  return new CQChartsBoxPlotDataObj(this, rect, data, is);
}

CQChartsBoxPlotConnectedObj *
CQChartsBoxPlot::
createConnectedObj(const BBox &rect, int groupInd, const ColorInd &ig) const
{
  return new CQChartsBoxPlotConnectedObj(this, rect, groupInd, ig);
}

CQChartsBoxPlotPointObj *
CQChartsBoxPlot::
createPointObj(const BBox &rect, int setId, int groupInd, const Point &p, const QModelIndex &ind,
               const ColorInd &is, const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsBoxPlotPointObj(this, rect, setId, groupInd, p, ind, is, ig, iv);
}

//---

CQChartsPlotCustomControls *
CQChartsBoxPlot::
createCustomControls()
{
  auto *controls = new CQChartsBoxPlotCustomControls(charts());

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsBoxPlotWhiskerObj::
CQChartsBoxPlotWhiskerObj(const Plot *plot, const BBox &rect, int setId, int groupInd,
                          const Whisker *whisker, const ColorInd &is, const ColorInd &ig) :
 CQChartsBoxPlotObj(plot, rect, is, ig, ColorInd()), setId_(setId), groupInd_(groupInd),
 whisker_(whisker)
{
  assert(whisker_);

  setDetailHint(DetailHint::MAJOR);

  for (auto &value : whisker_->values())
    addModelInd(value.ind);
}

double
CQChartsBoxPlotWhiskerObj::
pos() const
{
  return rect_.getXYMid(plot_->isVertical());
}

double
CQChartsBoxPlotWhiskerObj::
min() const
{
  return (whisker_ ? whisker_->min() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
lowerMedian() const
{
  return (whisker_ ? whisker_->lowerMedian() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
median() const
{
  return (whisker_ ? whisker_->median() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
upperMedian() const
{
  return (whisker_ ? whisker_->upperMedian() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
max() const
{
  return (whisker_ ? whisker_->max() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
mean() const
{
  return (whisker_ ? whisker_->mean() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
stddev() const
{
  return (whisker_ ? whisker_->stddev() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
notch() const
{
  return (whisker_ ? whisker_->notch() : 0.0);
}

QString
CQChartsBoxPlotWhiskerObj::
calcId() const
{
  if (setId_ >= 0)
    return QString("%1:%2:%3").arg(typeName()).arg(setId_).arg(groupInd_);
  else
    return QString("%1:%2").arg(typeName()).arg(groupInd_);
}

QString
CQChartsBoxPlotWhiskerObj::
calcTipId() const
{
  QString setName, groupName, name;

  if (plot_->hasSets())
    setName = plot_->setIdName(setId_);

  if (plot_->hasGroups())
    groupName = plot_->groupIndName(groupInd_);

  if (! setName.length() && ! groupName.length())
    name = (whisker_ ? whisker_->name() : "");

  CQChartsTableTip tableTip;

  if (setName.length())
    tableTip.addTableRow("Set", setName);

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  if (name.length())
    tableTip.addTableRow("Name", name);

  if (plot_->isErrorBar()) {
    tableTip.addTableRow("Mean"  , mean  ());
    tableTip.addTableRow("StdDev", stddev());
  }
  else {
    tableTip.addTableRow("Min"         , min        ());
    tableTip.addTableRow("Lower Median", lowerMedian());
    tableTip.addTableRow("Median"      , median     ());
    tableTip.addTableRow("Upper Median", upperMedian());
    tableTip.addTableRow("Max"         , max        ());
  }

  //---

  //plot()->addTipColumns(tableTip, node1_->ind());

  //---

  return tableTip.str();
}

//---

void
CQChartsBoxPlotWhiskerObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
//model->addProperty(path1, this, "selected")->setDesc("Is selected");

  model->addProperty(path1, this, "pos"        )->setDesc("Position");
  model->addProperty(path1, this, "min"        )->setDesc("Minimum");
  model->addProperty(path1, this, "lowerMedian")->setDesc("Lower median");
  model->addProperty(path1, this, "median"     )->setDesc("Median");
  model->addProperty(path1, this, "upperMedian")->setDesc("Upper median");
  model->addProperty(path1, this, "max"        )->setDesc("Maximum");
  model->addProperty(path1, this, "mean"       )->setDesc("Mean");
  model->addProperty(path1, this, "stddev"     )->setDesc("Standard deviation");
  model->addProperty(path1, this, "notch"      )->setDesc("Notch");
}

//---

void
CQChartsBoxPlotWhiskerObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->setColumn  ());
  addColumnSelectIndex(inds, plot_->groupColumn());
}

bool
CQChartsBoxPlotWhiskerObj::
inside(const Point &p) const
{
  if (plot_->isViolin())
    return poly_.containsPoint(p, Qt::OddEvenFill);

  return CQChartsBoxPlotObj::inside(p);
}

void
CQChartsBoxPlotWhiskerObj::
draw(PaintDevice *device) const
{
  device->setColorNames();

  //---

  // get color index
  auto colorInd = this->calcColorInd();

  if (plot_->hasSets() && plot_->isColorBySet())
    colorInd = is_;

  //---

  // set fill and stroke
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // set whisker fill and stroke
  PenBrush whiskerPenBrush;

  plot_->setWhiskerLineDataPen(whiskerPenBrush.pen, colorInd);

  plot_->setBrush(whiskerPenBrush, BrushData(false));

  plot_->updateObjPenBrushState(this, whiskerPenBrush, drawType());

  //---

  auto orientation = (plot_->isVertical() ? Qt::Vertical : Qt::Horizontal);

  //---

  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  CQStatData statData;

  statData.min         = remapPos(this->min());
  statData.lowerMedian = remapPos(this->lowerMedian());
  statData.median      = remapPos(this->median());
  statData.upperMedian = remapPos(this->upperMedian());
  statData.max         = remapPos(this->max());

  statData.lnotch = remapPos(this->median() - this->notch());
  statData.unotch = remapPos(this->median() + this->notch());

  //---

  bool drawBox       = true;
  bool drawBoxFilled = true;

  // draw violin
  if (plot_->isViolin()) {
    const auto &density = whisker_->density();

    double vw = plot_->lengthPlotSize(plot_->violinWidth(), plot_->isHorizontal())/2.0;

    auto rect = CQChartsGeom::makeDirBBox(/*flipped*/plot_->isHorizontal(),
                                          pos - vw, statData.min, pos + vw, statData.max);

    CQChartsWhiskerOpts opts;

    opts.violin = true;

    density.calcDistributionPoly(poly_, plot_, rect, orientation, opts);

    device->drawPolygon(poly_);

    drawBox       = plot_->isViolinBox();
    drawBoxFilled = false;
  }

  //---

  // draw error bar
  if (plot_->isErrorBar()) {
    device->setPen(whiskerPenBrush.pen);

    //---

    double mean = remapPos(this->mean());
    double dev1 = remapPos(this->mean() - this->stddev());
    double dev2 = remapPos(this->mean() + this->stddev());

    auto rect = CQChartsGeom::makeDirBBox(/*flipped*/plot_->isHorizontal(),
                                          pos - bw/2.0, dev1, pos + bw/2.0, dev2);

    if      (plot_->errorBarType() == CQChartsBoxPlot::ErrorBarType::CROSS_BAR) {
      CQChartsDensity::drawCrossBar(device, rect, mean, orientation, plot_->boxCornerSize());
    }
    else if (plot_->errorBarType() == CQChartsBoxPlot::ErrorBarType::ERROR_BAR) {
      CQChartsSymbolData symbol;

      symbol.setSymbol(Symbol::circle());
      symbol.setSize(plot_->outlierSymbolSize());

      CQChartsDensity::drawErrorBar(device, rect, mean, orientation, symbol);
    }
    else if (plot_->errorBarType() == CQChartsBoxPlot::ErrorBarType::POINT_RANGE) {
      // set fill and stroke
      PenBrush symbolPenBrush;

      auto boxColor    = plot_->interpBoxFillColor  (colorInd);
      auto strokeColor = plot_->interpBoxStrokeColor(colorInd);

      plot_->setPenBrush(symbolPenBrush,
        PenData  (true, strokeColor, plot_->boxShapeData().stroke()),
        BrushData(true, boxColor   , plot_->boxShapeData().fill  ()));

      plot_->updateObjPenBrushState(this, symbolPenBrush, drawType());

      CQChartsDrawUtil::setPenBrush(device, symbolPenBrush);

      //---

      CQChartsSymbolData symbol;

      symbol.setSymbol(Symbol::circle());
      symbol.setSize(plot_->outlierSymbolSize());

      CQChartsDensity::drawPointRange(device, rect, mean, orientation, symbol);
    }
    else if (plot_->errorBarType() == CQChartsBoxPlot::ErrorBarType::LINE_RANGE) {
      CQChartsDensity::drawLineRange(device, rect, orientation);
    }

    drawBox = false;
  }

  //---

  // draw notched box
  if (drawBox) {
    device->setPen(whiskerPenBrush.pen);

    //---

    if (! drawBoxFilled) {
      auto boxColor = plot_->interpThemeColor(ColorInd());

      penBrush.brush.setColor(boxColor);

      device->setBrush(penBrush.brush);
    }

    //---

    bool median = true;

    std::vector<double> outliers;

    CQChartsBoxWhiskerUtil::drawWhiskerBar(device, statData, pos, orientation, ww, bw,
                                           plot_->boxCornerSize(), plot_->isNotched(),
                                           median, outliers);
  }

  //---

  HTexts htexts;
  VTexts vtexts;

  auto addHText = [&](double xl, double xr, double y, const QString &text, bool onLeft) {
    htexts.emplace_back(xl, xr, y, text, onLeft);
  };

  auto addVText = [&](double yb, double yt, double x, const QString &text, bool onBottom) {
    vtexts.emplace_back(yb, yt, x, text, onBottom);
  };

  //---

  if (plot_->isErrorBar()) {
  }
  else {
    double wd1 = ww/2.0;
    double wd2 = bw/2.0;

    if (! device->isInteractive() ||
        plot_->drawLayerType() == CQChartsLayer::Type::MID_PLOT) {
      auto posToPixel = [&](double pos, double value) {
        if (plot_->isVertical())
          return plot_->windowToPixel(Point(pos, value));
        else
          return plot_->windowToPixel(Point(value, pos));
      };

      auto p1 = posToPixel(pos - wd1, statData.min        );
      auto p2 = posToPixel(pos - wd2, statData.lowerMedian);
      auto p3 = posToPixel(pos      , statData.median     );
      auto p4 = posToPixel(pos + wd2, statData.upperMedian);
      auto p5 = posToPixel(pos + wd1, statData.max        );

      // draw labels
      if (plot_->isTextVisible()) {
        plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

        //---

        PenBrush penBrush;

        auto tc = plot_->interpTextColor(colorInd);

        plot_->setPen(penBrush, PenData(true, tc, plot_->textAlpha()));

        device->setPen(penBrush.pen);

        //---

        bool hasRange = (fabs(this->max() - this->min()) > 1E-6);

        if (hasRange) {
          auto strl = QString::number(this->min        ());
          auto lstr = QString::number(this->lowerMedian());
          auto mstr = QString::number(this->median     ());
          auto ustr = QString::number(this->upperMedian());
          auto strh = QString::number(this->max        ());

          if (plot_->isVertical()) {
            addHText(p1.x, p5.x, p1.y, strl, /*onLeft*/true );
            addHText(p2.x, p4.x, p2.y, lstr, /*onLeft*/false);
            addHText(p2.x, p4.x, p3.y, mstr, /*onLeft*/true );
            addHText(p2.x, p4.x, p4.y, ustr, /*onLeft*/false);
            addHText(p1.x, p5.x, p5.y, strh, /*onLeft*/true );
          }
          else {
            addVText(p1.y, p5.y, p1.x, strl, /*onBottom*/false);
            addVText(p2.y, p4.y, p2.x, lstr, /*onBottom*/true );
            addVText(p2.y, p4.y, p3.x, mstr, /*onBottom*/false);
            addVText(p2.y, p4.y, p4.x, ustr, /*onBottom*/true );
            addVText(p1.y, p5.y, p5.x, strh, /*onBottom*/false);
          }
        }
        else {
          auto strl = QString::number(this->min());

          if (plot_->isVertical())
            addHText(p1.x, p5.x, p1.y, strl, /*onLeft*/true);
          else
            addVText(p1.y, p5.y, p1.x, strl, /*onBottom*/false);
        }
      }
    }
  }

  auto *th = const_cast<CQChartsBoxPlotWhiskerObj *>(this);

  th->clearDrawBBoxes();

  for (const auto &t : htexts) {
    if (drawHText(device, t.xl, t.xr, t.y, t.text, t.onLeft, t.bbox))
      th->addDrawBBox(t.bbox);
  }

  for (const auto &t : vtexts) {
    if (drawVText(device, t.yb, t.yt, t.x, t.text, t.onBottom, t.bbox))
      th->addDrawBBox(t.bbox);
  }

  //---

  device->resetColorNames();
}

void
CQChartsBoxPlotWhiskerObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // get color index
  auto colorInd = this->calcColorInd();

  if (plot_->hasSets() && plot_->isColorBySet())
    colorInd = is_;

  //---

  // set fill and stroke
  auto bc = plot_->interpBoxStrokeColor(colorInd);
  auto fc = plot_->interpBoxFillColor  (colorInd);

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isBoxStroked(), bc, plot_->boxStrokeAlpha(),
              plot_->boxStrokeWidth(), plot_->boxStrokeDash()),
    BrushData(plot_->isBoxFilled(), fc, plot_->boxFillAlpha(), plot_->boxFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush, drawType());
}

void
CQChartsBoxPlotWhiskerObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  //---

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.pos         = " << pos        () << ";\n";
  os << "  this.min         = " << min        () << ";\n";
  os << "  this.lowerMedian = " << lowerMedian() << ";\n";
  os << "  this.median      = " << median     () << ";\n";
  os << "  this.upperMedian = " << upperMedian() << ";\n";
  os << "  this.max         = " << max        () << ";\n";
  os << "  this.mean        = " << mean       () << ";\n";
  os << "  this.stddev      = " << stddev     () << ";\n";
  os << "  this.notch       = " << notch      () << ";\n";
}

CQChartsGeom::BBox
CQChartsBoxPlotWhiskerObj::
extraFitBBox() const
{
  if (plot_->isErrorBar())
    return BBox();

  //---

  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  double wd1 = ww/2.0;
  double wd2 = bw/2.0;

  auto posToRemapPixel = [&](double pos, double value) {
    if (plot_->isVertical())
      return plot_->windowToPixel(Point(pos, remapPos(value)));
    else
      return plot_->windowToPixel(Point(remapPos(value), pos));
  };

  auto p1 = posToRemapPixel(pos - wd1, min        ());
  auto p2 = posToRemapPixel(pos - wd2, lowerMedian());
  auto p3 = posToRemapPixel(pos      , median     ());
  auto p4 = posToRemapPixel(pos + wd2, upperMedian());
  auto p5 = posToRemapPixel(pos + wd1, max        ());

  //---

  BBox pbbox;

  if (plot_->isTextVisible()) {
    bool hasRange = (fabs(max() - min()) > 1E-6);

    if (hasRange) {
      auto strl = QString::number(min        ());
      auto lstr = QString::number(lowerMedian());
      auto mstr = QString::number(median     ());
      auto ustr = QString::number(upperMedian());
      auto strh = QString::number(max        ());

      if (plot_->isVertical()) {
        addHBBox(pbbox, p1.x, p5.x, p1.y, strl, /*onLeft*/false);
        addHBBox(pbbox, p2.x, p4.x, p2.y, lstr, /*onLeft*/true );
        addHBBox(pbbox, p2.x, p4.x, p3.y, mstr, /*onLeft*/false);
        addHBBox(pbbox, p2.x, p4.x, p4.y, ustr, /*onLeft*/true );
        addHBBox(pbbox, p1.x, p5.x, p5.y, strh, /*onLeft*/false);
      }
      else {
        addVBBox(pbbox, p1.y, p5.y, p1.x, strl, /*onBottom*/true );
        addVBBox(pbbox, p2.y, p4.y, p2.x, lstr, /*onBottom*/false);
        addVBBox(pbbox, p2.y, p4.y, p3.x, mstr, /*onBottom*/true );
        addVBBox(pbbox, p2.y, p4.y, p4.x, ustr, /*onBottom*/false);
        addVBBox(pbbox, p1.y, p5.y, p5.x, strh, /*onBottom*/true );
      }
    }
    else {
      auto strl = QString::number(min());

      if (plot_->isVertical())
        addHBBox(pbbox, p1.x, p5.x, p1.y, strl, /*onLeft*/false);
      else
        addVBBox(pbbox, p1.y, p5.y, p1.x, strl, /*onBottom*/true);
    }
  }
  else {
    pbbox += p1;
    pbbox += p2;
    pbbox += p3;
    pbbox += p4;
    pbbox += p5;
  }

  //---

  auto bbox = plot_->pixelToWindow(pbbox);

  return bbox;
}

double
CQChartsBoxPlotWhiskerObj::
remapPos(double y) const
{
  // remap to margin -> 1.0 - margin
  if (! plot_->isNormalized())
    return y;

  double ymargin = plot_->ymargin();

  if (plot_->isShowOutliers())
    return CMathUtil::map(y, whisker_->vmin(), whisker_->vmax(), ymargin, 1.0 - ymargin);
  else
    return CMathUtil::map(y, min(), max(), ymargin, 1.0 - ymargin);
}

//------

CQChartsBoxPlotOutlierObj::
CQChartsBoxPlotOutlierObj(const Plot *plot, const BBox &rect, int setId, int groupInd,
                          const Whisker *whisker, const ColorInd &is, const ColorInd &ig, int io) :
 CQChartsBoxPlotObj(plot, rect, is, ig, ColorInd()), setId_(setId), groupInd_(groupInd),
 whisker_(whisker), io_(io)
{
  if (whisker_) {
    const auto &ovalue = whisker_->value(io_);

    setModelInd(ovalue.ind);
  }
}

QString
CQChartsBoxPlotOutlierObj::
calcId() const
{
  return QString("%1:%2:%3:%4").arg(typeName()).arg(setId_).arg(groupInd_).arg(io_);
}

QString
CQChartsBoxPlotOutlierObj::
calcTipId() const
{
  QString setName, groupName, name;

  if (plot_->hasSets())
    setName = plot_->setIdName(setId_);

  if (plot_->hasGroups())
    groupName = plot_->groupIndName(groupInd_);

  if (whisker_ && ! setName.length() && ! groupName.length())
    name = whisker_->name();

  CQChartsTableTip tableTip;

  if (setName.length())
    tableTip.addTableRow("Set", setName);

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  if (name.length())
    tableTip.addTableRow("Name", name);

  //---

  if (whisker_) {
    const auto &ovalue = whisker_->value(io_);

    double rvalue = double(ovalue);

    tableTip.addTableRow("Value", rvalue);

    //---

    plot()->addTipColumns(tableTip, ovalue.ind);
  }

  //---

  return tableTip.str();
}

void
CQChartsBoxPlotOutlierObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->setColumn  ());
  addColumnSelectIndex(inds, plot_->groupColumn());

  //---

  if (whisker_) {
    const auto &ovalue = whisker_->value(io_);

    addSelectIndex(inds, ovalue.ind.row(), CQChartsColumn(ovalue.ind.column()),
                   ovalue.ind.parent());
  }
}

void
CQChartsBoxPlotOutlierObj::
draw(PaintDevice *device) const
{
  auto symbol     = plot_->outlierSymbol();
  auto symbolSize = plot_->outlierSymbolSize();

  //---

  // get color index
  auto colorInd = this->calcColorInd();

  if (plot_->hasSets() && plot_->isColorBySet())
    colorInd = is_;

  //---

  // set fill and stroke
  PenBrush penBrush;

  plot_->setOutlierSymbolPenBrush(penBrush, colorInd);

  if (color_.isValid())
    CQChartsDrawUtil::updateBrushColor(penBrush.brush, plot_->interpColor(color_, colorInd));

  plot_->updateObjPenBrushState(this, penBrush, drawType());

  //---

  // draw symbol
  double ox = rect_.getXYMid(plot_->isVertical());
  double oy = rect_.getXYMid(plot_->isHorizontal());

  Point pos(ox, oy);

  if (plot_->outlierSymbol().isValid())
    CQChartsDrawUtil::drawSymbol(device, penBrush, symbol, pos, symbolSize);
}

double
CQChartsBoxPlotOutlierObj::
remapPos(double y) const
{
  // remap to margin -> 1.0 - margin
  if (! whisker_ || ! plot_->isNormalized())
    return y;

  double ymargin = plot_->ymargin();

  return CMathUtil::map(y, whisker_->vmin(), whisker_->vmax(), ymargin, 1.0 - ymargin);
}

//------

CQChartsBoxPlotDataObj::
CQChartsBoxPlotDataObj(const Plot *plot, const BBox &rect, const WhiskerData &data,
                       const ColorInd &is) :
 CQChartsBoxPlotObj(plot, rect, is, ColorInd(), ColorInd()), data_(data)
{
  if (data_.ind.isValid())
    setModelInd(data_.ind);
}

double
CQChartsBoxPlotDataObj::
pos() const
{
  return rect_.getXYMid(plot_->isVertical());
}

QString
CQChartsBoxPlotDataObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(data_.name);
}

QString
CQChartsBoxPlotDataObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name"        , data_.name                );
  tableTip.addTableRow("Min"         , data_.statData.min        );
  tableTip.addTableRow("Lower Median", data_.statData.lowerMedian);
  tableTip.addTableRow("Median"      , data_.statData.median     );
  tableTip.addTableRow("Upper Median", data_.statData.upperMedian);
  tableTip.addTableRow("Max"         , data_.statData.max        );

  //---

  plot()->addTipColumns(tableTip, data_.ind);

  //---

  return tableTip.str();
}

void
CQChartsBoxPlotDataObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn          ());
  addColumnSelectIndex(inds, plot_->minColumn        ());
  addColumnSelectIndex(inds, plot_->lowerMedianColumn());
  addColumnSelectIndex(inds, plot_->medianColumn     ());
  addColumnSelectIndex(inds, plot_->upperMedianColumn());
  addColumnSelectIndex(inds, plot_->maxColumn        ());
  addColumnSelectIndex(inds, plot_->outliersColumn   ());
}

void
CQChartsBoxPlotDataObj::
draw(PaintDevice *device) const
{
  // set whisker fill and stroke
  PenBrush whiskerPenBrush;

  plot_->setWhiskerLineDataPen(whiskerPenBrush.pen, ColorInd());

  plot_->setBrush(whiskerPenBrush, BrushData(false));

  plot_->updateObjPenBrushState(this, whiskerPenBrush, drawType());

  //---

  // set fill and stroke
  PenBrush penBrush;

  auto bc = plot_->interpBoxStrokeColor(ColorInd());
  auto fc = plot_->interpBoxFillColor(ColorInd());

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isBoxStroked(), bc, plot_->boxStrokeAlpha(),
              plot_->boxStrokeWidth(), plot_->boxStrokeDash()),
    BrushData(plot_->isBoxFilled(), fc, plot_->boxFillAlpha(), plot_->boxFillPattern()));

  plot_->updateObjPenBrushState(this, penBrush, drawType());

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  //---

  auto orientation = plot_->orientation();

  //---

  device->setPen(whiskerPenBrush.pen);

  CQStatData statData;

  statData.min         = remapPos(data_.statData.min);
  statData.lowerMedian = remapPos(data_.statData.lowerMedian);
  statData.median      = remapPos(data_.statData.median     );
  statData.upperMedian = remapPos(data_.statData.upperMedian);
  statData.max         = remapPos(data_.statData.max);

  bool notched = false;
  bool median  = true;

  std::vector<double> outliers;

  CQChartsBoxWhiskerUtil::drawWhiskerBar(device, statData, pos, orientation, ww, bw,
                                         plot_->boxCornerSize(), notched, median, outliers);

  //---

  HTexts htexts;
  VTexts vtexts;

  auto addHText = [&](double xl, double xr, double y, const QString &text, bool onLeft) {
    htexts.emplace_back(xl, xr, y, text, onLeft);
  };

  auto addVText = [&](double yb, double yt, double x, const QString &text, bool onBottom) {
    vtexts.emplace_back(yb, yt, x, text, onBottom);
  };

  //---

  // draw labels
  if (plot_->isTextVisible()) {
    double wd1 = ww/2.0;
    double wd2 = bw/2.0;

    auto posToRemapPixel = [&](double pos, double value) {
      if (plot_->isVertical())
        return plot_->windowToPixel(Point(pos, remapPos(value)));
      else
        return plot_->windowToPixel(Point(remapPos(value), pos));
    };

    auto p1 = posToRemapPixel(pos - wd1, data_.statData.min        );
    auto p2 = posToRemapPixel(pos - wd2, data_.statData.lowerMedian);
    auto p3 = posToRemapPixel(pos      , data_.statData.median     );
    auto p4 = posToRemapPixel(pos + wd2, data_.statData.upperMedian);
    auto p5 = posToRemapPixel(pos + wd1, data_.statData.max        );

    //---

    plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

    //---

    PenBrush penBrush;

    auto tc = plot_->interpTextColor(ColorInd());

    plot_->setPen(penBrush, PenData(true, tc, plot_->textAlpha()));

    device->setPen(penBrush.pen);

    //---

    auto strl = QString::number(data_.statData.min        );
    auto lstr = QString::number(data_.statData.lowerMedian);
    auto mstr = QString::number(data_.statData.median     );
    auto ustr = QString::number(data_.statData.upperMedian);
    auto strh = QString::number(data_.statData.max        );

    if (plot_->isVertical()) {
      addHText(p1.x, p5.x, p1.y, strl, /*onLeft*/false);
      addHText(p2.x, p4.x, p2.y, lstr, /*onLeft*/true );
      addHText(p2.x, p4.x, p3.y, mstr, /*onLeft*/false);
      addHText(p2.x, p4.x, p4.y, ustr, /*onLeft*/true );
      addHText(p1.x, p5.x, p5.y, strh, /*onLeft*/false);
    }
    else {
      addVText(p1.y, p5.y, p1.x, strl, /*onBottom*/true );
      addVText(p2.y, p4.y, p2.x, lstr, /*onBottom*/false);
      addVText(p2.y, p4.y, p3.x, mstr, /*onBottom*/true );
      addVText(p2.y, p4.y, p4.x, ustr, /*onBottom*/false);
      addVText(p1.y, p5.y, p5.x, strh, /*onBottom*/true );
    }
  }

  //---

  auto *th = const_cast<CQChartsBoxPlotDataObj *>(this);

  th->clearDrawBBoxes();

  for (const auto &t : htexts) {
    if (drawHText(device, t.xl, t.xr, t.y, t.text, t.onLeft, t.bbox))
      th->addDrawBBox(t.bbox);
  }

  for (const auto &t : vtexts) {
    if (drawVText(device, t.yb, t.yt, t.x, t.text, t.onBottom, t.bbox))
      th->addDrawBBox(t.bbox);
  }
}

CQChartsGeom::BBox
CQChartsBoxPlotDataObj::
extraFitBBox() const
{
  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  double wd1 = ww/2.0;
  double wd2 = bw/2.0;

  auto posToRemapPixel = [&](double pos, double value) {
    if (plot_->isVertical())
      return plot_->windowToPixel(Point(pos, remapPos(value)));
    else
      return plot_->windowToPixel(Point(remapPos(value), pos));
  };

  auto p1 = posToRemapPixel(pos - wd1, data_.statData.min        );
  auto p2 = posToRemapPixel(pos - wd2, data_.statData.lowerMedian);
  auto p3 = posToRemapPixel(pos      , data_.statData.median     );
  auto p4 = posToRemapPixel(pos + wd2, data_.statData.upperMedian);
  auto p5 = posToRemapPixel(pos + wd1, data_.statData.max        );

  //---

  BBox pbbox;

  if (plot_->isTextVisible()) {
    auto strl = QString::number(data_.statData.min        );
    auto lstr = QString::number(data_.statData.lowerMedian);
    auto mstr = QString::number(data_.statData.median     );
    auto ustr = QString::number(data_.statData.upperMedian);
    auto strh = QString::number(data_.statData.max        );

    if (plot_->isVertical()) {
      addHBBox(pbbox, p1.x, p5.x, p1.y, strl, /*onLeft*/false);
      addHBBox(pbbox, p2.x, p4.x, p2.y, lstr, /*onLeft*/true );
      addHBBox(pbbox, p2.x, p4.x, p3.y, mstr, /*onLeft*/false);
      addHBBox(pbbox, p2.x, p4.x, p4.y, ustr, /*onLeft*/true );
      addHBBox(pbbox, p1.x, p5.x, p5.y, strh, /*onLeft*/false);
    }
    else {
      addVBBox(pbbox, p1.y, p5.y, p1.x, strl, /*onBottom*/true );
      addVBBox(pbbox, p2.y, p4.y, p2.x, lstr, /*onBottom*/false);
      addVBBox(pbbox, p2.y, p4.y, p3.x, mstr, /*onBottom*/true );
      addVBBox(pbbox, p2.y, p4.y, p4.x, ustr, /*onBottom*/false);
      addVBBox(pbbox, p1.y, p5.y, p5.x, strh, /*onBottom*/true );
    }
  }
  else {
    pbbox += p1;
    pbbox += p2;
    pbbox += p3;
    pbbox += p4;
    pbbox += p5;
  }

  //---

  auto bbox = plot_->pixelToWindow(pbbox);

  return bbox;
}

double
CQChartsBoxPlotDataObj::
remapPos(double y) const
{
  // remap to margin -> 1.0 - margin
  if (! plot_->isNormalized())
    return y;

  double ymargin = plot_->ymargin();

  return CMathUtil::map(y, data_.dataMin, data_.dataMax, ymargin, 1.0 - ymargin);
}

//------

CQChartsBoxPlotConnectedObj::
CQChartsBoxPlotConnectedObj(const CQChartsBoxPlot *plot, const BBox &rect, int groupInd,
                            const ColorInd &ig) :
 CQChartsPlotObj(const_cast<CQChartsBoxPlot *>(plot), rect, ColorInd(), ig, ColorInd()),
 plot_(plot), groupInd_(groupInd)
{
  initPolygon();
}

QString
CQChartsBoxPlotConnectedObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(ig_.i);
}

QString
CQChartsBoxPlotConnectedObj::
calcTipId() const
{
  auto groupName = plot_->groupIndName(groupInd_);

  const auto &setWhiskerMap = this->setWhiskerMap();

  int ns = setWhiskerMap.size();

  CQChartsTableTip tableTip;

  tableTip.addTableRow("Group"   , groupName);
  tableTip.addTableRow("Num Sets", ns);

  //---

  //plot()->addTipColumns(tableTip, node1_->ind());

  //---

  return tableTip.str();
}

void
CQChartsBoxPlotConnectedObj::
initPolygon()
{
  Polygon maxPoly, minPoly;

  const auto &setWhiskerMap = this->setWhiskerMap();

  for (const auto &setWhiskers : setWhiskerMap) {
    int   setId   = setWhiskers.first;
    auto *whisker = setWhiskers.second;

    double min    = whisker->min   ();
    double max    = whisker->max   ();
    double median = whisker->median();

    line_.addPoint(Point(setId, median));

    maxPoly.addPoint(Point(setId, max));
    minPoly.addPoint(Point(setId, min));
  }

  //---

  int np = maxPoly.size();

  for (int i = 0; i < np; ++i)
    poly_.addPoint(maxPoly.point(i));

  for (int i = 0; i < np; ++i)
    poly_.addPoint(minPoly.point(np - 1 - i));
}

const CQChartsBoxPlotConnectedObj::SetWhiskerMap &
CQChartsBoxPlotConnectedObj::
setWhiskerMap() const
{
  static CQChartsBoxPlotConnectedObj::SetWhiskerMap dummy;

  int i = 0;

  for (const auto &groupIdWhiskers : plot_->groupWhiskers()) {
    if (i == ig_.i)
      return groupIdWhiskers.second;

    ++i;
  }

  assert(false);

  return dummy;
}

bool
CQChartsBoxPlotConnectedObj::
inside(const Point &p) const
{
  return poly_.containsPoint(p, Qt::OddEvenFill);
}

void
CQChartsBoxPlotConnectedObj::
draw(PaintDevice *device) const
{
  // draw range polygon
  int np = poly_.size();

  if (np) {
    // set pen and brush
    PenBrush pPenBrush;

    auto bc = plot_->interpBoxStrokeColor(ig_);
    auto fc = plot_->interpBoxFillColor  (ig_);

    plot_->setPenBrush(pPenBrush,
      PenData  (plot_->isBoxStroked(), bc, plot_->boxStrokeAlpha(),
                plot_->boxStrokeWidth(), plot_->boxStrokeDash()),
      BrushData(plot_->isBoxFilled(), fc, plot_->boxFillAlpha(), plot_->boxFillPattern()));

    plot_->updateObjPenBrushState(this, pPenBrush, drawType());

    CQChartsDrawUtil::setPenBrush(device, pPenBrush);

    //---

    // draw poly
    auto path = CQChartsDrawUtil::polygonToPath(poly_, /*closed*/true);

    device->drawPath(path);
  }

  //---

  // set pen
  PenBrush lPenBrush;

  auto lineColor = plot_->interpBoxStrokeColor(ig_);

  plot_->setPen(lPenBrush,
    PenData(true, lineColor, plot_->boxShapeData().stroke()));

  plot_->updateObjPenBrushState(this, lPenBrush, drawType());

  device->setPen(lPenBrush.pen);

  //---

  // draw connected line
  Polygon line;

  for (int i = 0; i < line_.size(); ++i)
    line.addPoint(line_.point(i));

  device->drawPolyline(line);
}

//------

CQChartsBoxPlotObj::
CQChartsBoxPlotObj(const CQChartsBoxPlot *plot, const BBox &rect, const ColorInd &is,
                   const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsBoxPlot *>(plot), rect, is, ig, iv), plot_(plot)
{
}

void
CQChartsBoxPlotObj::
clearDrawBBoxes()
{
  drawBBoxes_.clear();
}

void
CQChartsBoxPlotObj::
addDrawBBox(const BBox &bbox)
{
  drawBBoxes_.push_back(bbox);
}

bool
CQChartsBoxPlotObj::
checkDrawBBox(const BBox &bbox) const
{
  for (const auto &bbox1 : drawBBoxes_) {
    if (bbox1.intersect(bbox))
      return false;
  }

  return true;
}

bool
CQChartsBoxPlotObj::
drawHText(PaintDevice *device, double pxl, double pxr, double py,
          const QString &text, bool onLeft, BBox &bbox) const
{
  double margin  = plot_->textMargin();
  bool   invertX = plot_->isInvertX();

  if (invertX)
    onLeft = ! onLeft;

  double px = ((onLeft && ! invertX) || (! onLeft && invertX) ? pxl : pxr);

  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  QFontMetricsF fm(device->font());

  auto tp = (onLeft ? Point(px - margin - fm.width(text), py) : Point(px + margin, py));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = Angle();
  options.align         = Qt::AlignLeft | Qt::AlignVCenter;
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();
  options.clipLength    = plot_->lengthPixelWidth(plot_->textClipLength());
  options.clipElide     = plot_->textClipElide();

  auto tw = plot_->pixelToWindow(tp); // left/midy

  auto psize = CQChartsDrawUtil::calcTextSize(text, device->font(), options);

  auto pbbox = BBox(tp.x                , tp.y - psize.height()/2.0,
                    tp.x + psize.width(), tp.y + psize.height()/2.0);

  bbox = plot_->pixelToWindow(pbbox);

  if (! checkDrawBBox(bbox))
    return false;

  CQChartsDrawUtil::drawTextAtPoint(device, tw, text, options);

  return true;
}

bool
CQChartsBoxPlotObj::
drawVText(PaintDevice *device, double pyb, double pyt, double px, const QString &text,
          bool onBottom, BBox &bbox) const
{
  double margin  = plot_->textMargin();
  bool   invertY = plot_->isInvertY();

  if (invertY)
    onBottom = ! onBottom;

  double py = ((onBottom && ! invertY) || (! onBottom && invertY) ? pyb : pyt);

  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  QFontMetricsF fm(device->font());

  double xf = fm.width(text)/2.0;
  double yf = fm.height()/2.0;

  auto tp = (onBottom ? Point(px - xf, py + margin + yf) : Point(px - xf, py - margin - yf));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = Angle();
  options.align         = Qt::AlignLeft | Qt::AlignVCenter;
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();
  options.clipLength    = plot_->lengthPixelWidth(plot_->textClipLength());
  options.clipElide     = plot_->textClipElide();

  auto tw = plot_->pixelToWindow(tp); // left/midy

  auto psize = CQChartsDrawUtil::calcTextSize(text, device->font(), options);
  auto size  = plot()->pixelToWindowSize(psize);

  bbox = BBox(tw.x, tw.y, tw.x + size.width(), tw.y + size.height());

  if (! checkDrawBBox(bbox))
    return false;

  CQChartsDrawUtil::drawTextAtPoint(device, tw, text, options);

  return true;
}

void
CQChartsBoxPlotObj::
addHBBox(BBox &pbbox, double xl, double xr, double y, const QString &text, bool onLeft) const
{
  double margin  = plot_->textMargin();
  bool   invertX = plot_->isInvertX();

  if (invertX)
    onLeft = ! onLeft;

  double x = ((onLeft && ! invertX) || (! onLeft && invertX) ? xl : xr);

  auto font = plot_->qfont(plot_->textFont());

  QFontMetricsF fm(font);

  double fa = fm.ascent ();
  double fd = fm.descent();
  double yf = (fa - fd)/2.0;

  double tx;

  if (onLeft)
    tx = x - margin - fm.width(text);
  else
    tx = x + margin + fm.width(text);

  pbbox += Point(tx, y + yf - fa);
  pbbox += Point(tx, y + yf + fd);
}

void
CQChartsBoxPlotObj::
addVBBox(BBox &pbbox, double yb, double yt, double x, const QString &text, bool onBottom) const
{
  double margin  = plot_->textMargin();
  bool   invertY = plot_->isInvertY();

  if (invertY)
    onBottom = ! onBottom;

  double y = ((onBottom && ! invertY) || (! onBottom && invertY) ? yb : yt);

  auto font = plot_->qfont(plot_->textFont());

  QFontMetricsF fm(font);

  double xf = fm.width(text)/2.0;
  double fa = fm.ascent ();
  double fd = fm.descent();

  double ty;

  if (onBottom)
    ty = y + margin + fa;
  else
    ty = y - margin - fd;

  pbbox += Point(x - xf, ty);
  pbbox += Point(x + xf, ty);
}

//------

CQChartsBoxPlotPointObj::
CQChartsBoxPlotPointObj(const Plot *plot, const BBox &rect, int setId, int groupInd,
                        const Point &p, const QModelIndex &ind, const ColorInd &is,
                        const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsBoxPlot *>(plot), rect, is, ig, iv), plot_(plot),
 setId_(setId), groupInd_(groupInd), p_(p)
{
  setModelInd(ind);
}

QString
CQChartsBoxPlotPointObj::
calcId() const
{
  return QString("%1:%2:%3:%4").arg(typeName()).arg(is_.i).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsBoxPlotPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  QString setName, groupName;

  if (plot_->hasSets())
    setName = plot_->setIdName(setId_);

  if (plot_->hasGroups())
    groupName = plot_->groupIndName(groupInd_);

  if (setName.length())
    tableTip.addTableRow("Set", setName);

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  tableTip.addTableRow("Ind", iv_.i);

  //---

  auto addColumnRowValue = [&](const Column &column) {
    if (! column.isValid()) return;

    if (tableTip.hasColumn(column))
      return;

    ModelIndex columnInd(plot_, modelInd().row(), column, modelInd().parent());

    bool ok;

    auto str = plot_->modelString(columnInd, ok);
    if (! ok) return;

    tableTip.addTableRow(plot_->columnHeaderName(column, /*tip*/true), str);

    tableTip.addColumn(column);
  };

  //---

  if (plot_->valueColumns().count() == 1)
    addColumnRowValue(plot_->valueColumns().column());

  addColumnRowValue(plot_->colorColumn());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsBoxPlotPointObj::
inside(const Point &p) const
{
  auto p1 = plot_->windowToPixel(Point(p_.x, p_.y));

  BBox pbbox(p1.x - 4, p1.y - 4, p1.x + 4, p1.y + 4);

  auto pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsBoxPlotPointObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, CQChartsColumn(modelInd().column()));
}

void
CQChartsBoxPlotPointObj::
draw(PaintDevice *device) const
{
  auto symbol     = plot_->jitterSymbol();
  auto symbolSize = plot_->jitterSymbolSize();

  //---

  // get color index
  auto colorInd = this->calcColorInd();

  if (plot_->hasSets() && plot_->isColorBySet())
    colorInd = is_;

  //---

  // calc stroke and brush
  PenBrush penBrush;

  plot_->setJitterSymbolPenBrush(penBrush, colorInd);

  if (color_.isValid())
    CQChartsDrawUtil::updateBrushColor(penBrush.brush, plot_->interpColor(color_, colorInd));

  plot_->updateObjPenBrushState(this, penBrush, drawType());

  //---

  // draw symbol
  if (symbol.isValid())
    CQChartsDrawUtil::drawSymbol(device, penBrush, symbol, p_, symbolSize);
}

//------

CQChartsBoxKeyColor::
CQChartsBoxKeyColor(CQChartsBoxPlot *plot, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyColorBox(plot, is, ig, ColorInd())
{
}

bool
CQChartsBoxKeyColor::
selectPress(const Point &, CQChartsSelMod)
{
  auto *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  auto ic = (is_.n > 1 ? is_ : ig_);

  plot->setSetHidden(ic.i, ! plot->isSetHidden(ic.i));

  plot->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsBoxKeyColor::
fillBrush() const
{
  auto c = CQChartsKeyColorBox::fillBrush().color();

  auto *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  auto ic = (is_.n > 1 ? is_ : ig_);

  if (plot->isSetHidden(ic.i))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

double
CQChartsBoxKeyColor::
xColorValue(bool relative) const
{
  auto *boxObj = this->boxObj();

  return (boxObj ? boxObj->xColorValue(relative) : 0.0);
}

double
CQChartsBoxKeyColor::
yColorValue(bool relative) const
{
  auto *boxObj = this->boxObj();

  return (boxObj ? boxObj->yColorValue(relative) : 0.0);
}

CQChartsBoxPlotWhiskerObj *
CQChartsBoxKeyColor::
boxObj() const
{
  for (const auto &plotObj : plot_->plotObjects()) {
    auto *boxObj = dynamic_cast<CQChartsBoxPlotWhiskerObj *>(plotObj);
    if (! boxObj) continue;

    if (boxObj->is() == is_ && boxObj->ig() == ig_)
      return boxObj;
  }

  return nullptr;
}

//------

CQChartsBoxKeyText::
CQChartsBoxKeyText(CQChartsBoxPlot *plot, const QString &text,
                   const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyText(plot, text, (is.n > 1 ? is : ig))
{
}

QColor
CQChartsBoxKeyText::
interpTextColor(const ColorInd &ind) const
{
  auto *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  auto c = CQChartsKeyText::interpTextColor(ind);

  if (plot->isSetHidden(ic_.i))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

//------

CQChartsBoxPlotCustomControls::
CQChartsBoxPlotCustomControls(CQCharts *charts) :
 CQChartsGroupPlotCustomControls(charts, "box")
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  // columns type
  columnsTypeCombo_ = CQUtil::makeWidget<CQEnumCombo>("columnsTypeCombo");

  columnsTypeCombo_->setPropName("columnDataType");

  addFrameWidget(columnsFrame, "Columns Type", columnsTypeCombo_);

  //---

  // values, name and label columns
  static auto columnNames = QStringList() <<
    "values" << "name" << "set" <<
    "x" << "min" << "lowerMedian" << "median" << "upperMedian" << "max" << "outliers";

  addColumnWidgets(columnNames, columnsFrame);

  //---

  // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame");

  orientationCombo_ = createEnumEdit("orientation");
  pointsTypeCombo_  = createEnumEdit("pointsType");

  addFrameWidget(optionsFrame, "Orientation", orientationCombo_);
  addFrameWidget(optionsFrame, "Points Type", pointsTypeCombo_ );

  //---

  colorBySetCheck_ = createBoolEdit("colorBySet");

  addFrameWidget(optionsFrame, "Color By Set", colorBySetCheck_);

  //---

  normalizedCheck_ = createBoolEdit("normalized", /*choice*/false);
  notchedCheck_    = createBoolEdit("notched"   , /*choice*/false);
  violinCheck_     = createBoolEdit("violin"    , /*choice*/false);
  errorBarCheck_   = createBoolEdit("errorBar"  , /*choice*/false);

  auto *optionsFrame1  = CQUtil::makeWidget<QFrame>("optionsFrame1");
  auto *optionsLayout1 = CQUtil::makeLayout<QGridLayout>(optionsFrame1, 0, 2);

  //auto *optionsFrame2  = CQUtil::makeWidget<QFrame>("optionsFrame2");
  //auto *optionsLayout2 = CQUtil::makeLayout<QHBoxLayout>(optionsFrame2, 0, 2);

  addFrameWidget(optionsFrame, optionsFrame1);
  //addFrameWidget(optionsFrame, optionsFrame2);

  optionsLayout1->addWidget(normalizedCheck_, 0, 0);
  optionsLayout1->addWidget(notchedCheck_   , 0, 1);

  //addFrameWidget(optionsFrame, "Normalized", normalizedCheck_);
  //addFrameWidget(optionsFrame, "Notched"   , notchedCheck_);

  optionsLayout1->addWidget(violinCheck_  , 0, 2);
  optionsLayout1->addWidget(errorBarCheck_, 0, 3);

  optionsLayout1->setColumnStretch(4, 1);
//optionsLayout2->addStretch(1);

  //addFrameWidget(optionsFrame, "Violin"   , violinCheck_);
  //addFrameWidget(optionsFrame, "Error Bar", errorBarCheck_);

  //---

  //addFrameRowStretch(optionsFrame);

  //---

  addGroupColumnWidgets();

  addColorColumnWidgets("Point Color");

  //---

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsBoxPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    columnsTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(columnsTypeSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    orientationCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    pointsTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(pointsTypeSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    normalizedCheck_, SIGNAL(stateChanged(int)), this, SLOT(normalizedSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    notchedCheck_, SIGNAL(stateChanged(int)), this, SLOT(notchedSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    colorBySetCheck_, SIGNAL(stateChanged(int)), this, SLOT(colorBySetSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    violinCheck_, SIGNAL(stateChanged(int)), this, SLOT(violinSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    errorBarCheck_, SIGNAL(stateChanged(int)), this, SLOT(errorBarSlot()));

  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsBoxPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsBoxPlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);
}

void
CQChartsBoxPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  columnsTypeCombo_->setObj(plot_);

  auto type = plot_->calcColumnDataType();

  if (type == CQChartsBoxPlot::ColumnDataType::RAW)
    showColumnWidgets(plot_->rawCustomColumns());
  else
    showColumnWidgets(plot_->calculatedCustomColumns());

  //---

  orientationCombo_->setCurrentValue((int) plot_->orientation());
  pointsTypeCombo_ ->setCurrentValue((int) plot_->pointsType());

  colorBySetCheck_->setChecked(plot_->isColorBySet());

  normalizedCheck_->setChecked(plot_->isNormalized());
  notchedCheck_   ->setChecked(plot_->isNotched());
  violinCheck_    ->setChecked(plot_->isViolin());
  errorBarCheck_  ->setChecked(plot_->isErrorBar());

  colorBySetCheck_->setEnabled(plot_->canColorBySet());

  if (plot_->calcColumnDataType() == CQChartsBoxPlot::ColumnDataType::RAW) {
    notchedCheck_ ->setEnabled(! violinCheck_->isChecked() && ! errorBarCheck_->isChecked());
    violinCheck_  ->setEnabled(true);
    errorBarCheck_->setEnabled(true);
  }
  else {
    notchedCheck_ ->setEnabled(false);
    violinCheck_  ->setEnabled(false);
    errorBarCheck_->setEnabled(false);
  }

  //---

  CQChartsGroupPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsBoxPlotCustomControls::
columnsTypeSlot()
{
  updateWidgets();
}

void
CQChartsBoxPlotCustomControls::
orientationSlot()
{
  plot_->setOrientation((Qt::Orientation) orientationCombo_->currentValue());

  updateWidgets();
}

void
CQChartsBoxPlotCustomControls::
pointsTypeSlot()
{
  plot_->setPointsType((CQChartsBoxPlot::PointsType) pointsTypeCombo_->currentValue());

  updateWidgets();
}

void
CQChartsBoxPlotCustomControls::
normalizedSlot()
{
  plot_->setNormalized(normalizedCheck_->isChecked());

  updateWidgets();
}

void
CQChartsBoxPlotCustomControls::
notchedSlot()
{
  plot_->setNotched(notchedCheck_->isChecked());

  updateWidgets();
}

void
CQChartsBoxPlotCustomControls::
colorBySetSlot()
{
  plot_->setColorBySet(colorBySetCheck_->isChecked());

  updateWidgets();
}

void
CQChartsBoxPlotCustomControls::
violinSlot()
{
  plot_->setViolin(violinCheck_->isChecked());

  updateWidgets();
}

void
CQChartsBoxPlotCustomControls::
errorBarSlot()
{
  plot_->setErrorBar(errorBarCheck_->isChecked());

  updateWidgets();
}

CQChartsColor
CQChartsBoxPlotCustomControls::
getColorValue()
{
  return plot_->jitterSymbolFillColor();
}

void
CQChartsBoxPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setJitterSymbolFillColor(c);
}
