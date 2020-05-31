#include <CQChartsXYPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsUtil.h>
#include <CQChartsArrow.h>
#include <CQChartsSmooth.h>
#include <CQChartsDataLabel.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsVariant.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQCharts.h>

#include <CQUtil.h>
#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

CQChartsXYPlotType::
CQChartsXYPlotType()
{
}

void
CQChartsXYPlotType::
addParameters()
{
  startParameterGroup("XY");

  // columns
  addColumnParameter ("x", "X", "xColumn").
    setRequired().setMonotonic().setNumeric().setTip("X Value Column");
  addColumnsParameter("y", "Y", "yColumns").
    setRequired().setNumeric().setTip("Y Value Column(s)");

  addColumnParameter("label", "Label", "labelColumn").
    setString().setTip("Optional Label Column");

  //---

  // options
  addBoolParameter("lines" , "Lines" , "lines", true).setTip("Draw Lines");
  addBoolParameter("points", "Points", "points"     ).setTip("Draw Points");

  addBoolParameter("columnSeries", "Column Series", "columnSeries").setTip("Columns are series");

  addBoolParameter("bivariate" , "Bivariate" , "bivariateLines" ).setTip("Draw Bivariate Lines");
  addBoolParameter("stacked"   , "Stacked"   , "stacked"        ).setTip("Stack Points");
  addBoolParameter("cumulative", "Cumulative", "cumulative"     ).setTip("Cumulate Values");
  addBoolParameter("fillUnder" , "Fill Under", "fillUnderFilled").setTip("Fill Under Curve");
  addBoolParameter("impulse"   , "Impulse"   , "impulseLines"   ).setTip("Draw Point Impulse");

  endParameterGroup();

  //---

  addMappingParameters();

  //---

  // vector columns
  startParameterGroup("Vectors");

  addColumnParameter("vectorX", "Vector X", "vectorXColumn").setNumeric();
  addColumnParameter("vectorY", "Vector Y", "vectorYColumn").setNumeric();

  addBoolParameter("vectors", "Vectors", "vectors");

  endParameterGroup();

  //---

  CQChartsPointPlotType::addParameters();
}

QString
CQChartsXYPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("XY Plot Type</h2>").
    h3("Summary").
     p("Draws points at x and y coordinate pairs and optionally connects them with a "
       "continuous line. The connecting lines can be straight or rounded.").
     p("Ideally the x coordinates should be monotonic but this is not required.").
    h3("Columns").
     p("The x and y values come from the values in the " + B("X") + " and " + B("Y") + " columns. "
       "Multiple " + B("Y") + " columns can be specified to create a stack of lines.").
     p("An optional " + B("SymbolType") + " column can be specified to supply the type of the "
       "symbol drawn at the point. An optional " + B("SymbolSize") + " column can be specified "
       "to supply the size of the symbol drawn at the point. An optional " + B("Color") + " "
       "column can be specified to supply the fill color of the symbol drawn at the point.").
     p("An optional point label can be specified using the " + B("Label") + " column. The "
       "font size of the label can be specified using the " + B("FontSize") + " column.").
     p("Optional " + B("VectorX") + " and " + B("VectorY") + " columns can be specified to draw "
       "a vector at the point.").
    h3("Options").
     p("The " + B("Lines") + " option determines whether the points are connected with a line. "
       "The default line style can be separately customized.").
     p("The " + B("Points") + " option determines whether the points are drawn. The default point "
       "symbol can be separately customized.").
     p("Enabling the " + B("Bivariate") + " option fills the area between adjacent sets of x, y "
       "coordinates (two or more y column values should be specified). The bivariate line "
       "style can be separately customized.").
     p("Enabling the " + B("Stacked") + " option stacks the y values on top of each other "
       "so the next set of y values adds onto the previous set of y values.").
     p("Enabling the " + B("Cumulative") + " option treats the y values as an increment "
       "from the previous y value (in each set).").
     p("Enabling the " + B("FillUnder") + " option fills the area under the plot. The "
       "fill under style (fill/stroke) can be separately customized.").
     p("Enabling the " + B("Impulse") + " option draws a line from zero to the "
       "points y value. The impulse line style can be separately customized.").
     p("Enabling the " + B("Best Fit") + " option draws a best fit line between the points.").
     p("The " + B("Vectors") + " option detemines whether the vector specified by the "
       "" + B("VectorX") + " and " + B("VectorY") + " columns are drawn.").
    h3("Customization").
     p("The area under the curve can be filled.").
     p("Impulse lines (from point to y minimum) curve can be added.").
     p("Bivariate lines (between pairs of y values) curve can be added.").
     p("The points can have an overlaid best fit line, statistic lines.").
    h3("Limitations").
     p("None").
    h3("Example").
     p(IMG("images/xychart.png"));
}

void
CQChartsXYPlotType::
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
{
  auto px   = analyzeModelData.parameterNameColumn.find("x");
  bool hasX = (px != analyzeModelData.parameterNameColumn.end());

  auto *details = modelData->details();
  if (! details) return;

  // set x column
  CQChartsColumn xColumn;

  int nc = details->numColumns();

  for (int c = 0; c < nc; ++c) {
    if (! xColumn.isValid()) {
      auto *columnDetails = details->columnDetails(CQChartsColumn(c));
      if (! columnDetails) continue;

      if      (columnDetails->isMonotonic())
        xColumn = columnDetails->column();
      else if (columnDetails->type() == ColumnType::TIME)
        xColumn = columnDetails->column();
      else if (! hasX && columnDetails->isNumeric())
        xColumn = columnDetails->column();
    }
  }

  if      (xColumn.isValid())
    analyzeModelData.parameterNameColumn["x"] = xColumn;
  else if (hasX)
    xColumn = (*px).second;

  CQChartsColumns yColumns;

  for (int c = 0; c < nc; ++c) {
    if (c == xColumn.column())
      continue;

    auto *columnDetails = details->columnDetails(CQChartsColumn(c));
    if (! columnDetails) continue;

    if (columnDetails->isNumeric())
      yColumns.addColumn(columnDetails->column());
  }

  if (yColumns.count())
    analyzeModelData.parameterNameColumns["y"] = yColumns;
}

CQChartsPlot *
CQChartsXYPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsXYPlot(view, model);
}

//---

CQChartsXYPlot::
CQChartsXYPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPointPlot(view, view->charts()->plotType("xy"), model),
 CQChartsObjLineData         <CQChartsXYPlot>(this),
 CQChartsObjPointData        <CQChartsXYPlot>(this),
 CQChartsObjImpulseLineData  <CQChartsXYPlot>(this),
 CQChartsObjBivariateLineData<CQChartsXYPlot>(this),
 CQChartsObjFillUnderFillData<CQChartsXYPlot>(this)
{
  NoUpdate noUpdate(this);

  //---

  setImpulseLines  (false);
  setBivariateLines(false);

  setLinesColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setImpulseLinesColor  (CQChartsColor(CQChartsColor::Type::PALETTE));
  setBivariateLinesColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setLines (true);
  setPoints(false);

  setLinesWidth(CQChartsLength("3px"));

  //---

  // arrow object
  arrowObj_ = new CQChartsArrow(this);

  arrowObj_->setVisible(false);

  connect(arrowObj_, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

  //---

  setFillUnderFilled   (false);
  setFillUnderFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setFillUnderFillAlpha(CQChartsAlpha(0.5));

  //---

  addAxes();

  addKey();

  addTitle();
}

CQChartsXYPlot::
~CQChartsXYPlot()
{
  delete arrowObj_;
}

//---

void
CQChartsXYPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setYColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(yColumns_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsXYPlot::
setVectorXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(vectorXColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setVectorYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(vectorYColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsXYPlot::
resetBestFit()
{
  for (const auto &plotObj : plotObjs_) {
    auto *polyObj = dynamic_cast<CQChartsXYPolylineObj *>(plotObj);

    if (polyObj)
      polyObj->resetBestFit();
  }
}

//---

void
CQChartsXYPlot::
setStacked(bool b)
{
  CQChartsUtil::testAndSet(stacked_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setCumulative(bool b)
{
  CQChartsUtil::testAndSet(cumulative_, b, [&]() { updateRangeAndObjs(); } );
}

bool
CQChartsXYPlot::
isVectors() const
{
  return arrowObj_->isVisible();
}

void
CQChartsXYPlot::
setVectors(bool b)
{
  if (b != isVectors()) {
    CQChartsWidgetUtil::AutoDisconnect arrowDisconnect(
      arrowObj_, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

    arrowObj_->setVisible(b);

    updateObjs();
  }
}

//---

void
CQChartsXYPlot::
setPointsSlot(bool b)
{
  setPoints(b);
}

void
CQChartsXYPlot::
setPointDelta(int i)
{
  CQChartsUtil::testAndSet(pointDelta_, i, [&]() { updateObjs(); } );
}

void
CQChartsXYPlot::
setPointCount(int i)
{
  CQChartsUtil::testAndSet(pointCount_, i, [&]() { updateObjs(); } );
}

void
CQChartsXYPlot::
setPointStart(int i)
{
  CQChartsUtil::testAndSet(pointStart_, i, [&]() { updateObjs(); } );
}

//---

void
CQChartsXYPlot::
setLinesSlot(bool b)
{
  setLines(b);
}

void
CQChartsXYPlot::
setLinesSelectable(bool b)
{
  CQChartsUtil::testAndSet(linesSelectable_, b, [&]() { drawObjs(); } );
}

void
CQChartsXYPlot::
setRoundedLines(bool b)
{
  CQChartsUtil::testAndSet(roundedLines_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsXYPlot::
setFillUnderSelectable(bool b)
{
  CQChartsUtil::testAndSet(fillUnderData_.selectable, b, [&]() { drawObjs(); } );
}

void
CQChartsXYPlot::
setFillUnderPos(const CQChartsFillUnderPos &pos)
{
  CQChartsUtil::testAndSet(fillUnderData_.pos, pos, [&]() { updateObjs(); } );
}

void
CQChartsXYPlot::
setFillUnderSide(const CQChartsFillUnderSide &s)
{
  CQChartsUtil::testAndSet(fillUnderData_.side, s, [&]() { updateObjs(); } );
}

//---

void
CQChartsXYPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addArrowProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    return &(this->addProperty(path, arrowObj_, name, alias)->setDesc(desc));
  };

  auto addArrowStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                               const QString &desc) {
    auto *item = addArrowProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addBaseProperties();

  // columns
  addProp("columns", "xColumn"    , "x"    , "X value column" );
  addProp("columns", "yColumns"   , "y"    , "Y value column(s)");
  addProp("columns", "labelColumn", "label", "Label column");

  addProp("columns", "vectorXColumn", "vectorX", "Vector x column");
  addProp("columns", "vectorYColumn", "vectorY", "Vector y column");

  // bivariate
  addProp("bivariate", "bivariateLines", "visible", "Bivariate lines visible");

  addLineProperties("bivariate/stroke", "bivariateLines", "Bivariate");

  // stacked
  addProp("stacked", "stacked", "enabled", "Stack y values");

  // cumulative
  addProp("cumulative", "cumulative", "enabled", "Cumulative values");

  // column series
  addProp("options", "columnSeries", "timeSeries", "Are y column values a series");

  // points
  addProp("points", "points"         , "visible", "Point symbol visible");
  addProp("points", "pointLineSelect", "lineSelect", "Select point selects line");
  addProp("points", "pointCount"     , "count", "Number of points to show");
  addProp("points", "pointDelta"     , "delta", "Show points delta index");
  addProp("points", "pointStart"     , "start", "Show points start index");

  addSymbolProperties("points/symbol", "", "Points");

  // lines
  addProp("lines", "lines"          , "visible"   , "Lines visible");
  addProp("lines", "linesSelectable", "selectable", "Lines selectable");
  addProp("lines", "roundedLines"   , "rounded"   , "Smooth lines");

  addLineProperties("lines/stroke", "lines", "Lines");

  //---

  // best fit line and deviation fill
  addBestFitProperties();

  // convex hull shape
  addHullProperties();

  // stats
  addStatsProperties();

  //---

  // fill under
  addProp("fillUnder", "fillUnderFilled"    , "visible"   , "Fill under polygon visible");
  addProp("fillUnder", "fillUnderSelectable", "selectable", "Fill under polygon selectable");
  addProp("fillUnder", "fillUnderPos"       , "position"  , "Fill under base position");
  addProp("fillUnder", "fillUnderSide"      , "side"      , "Fill under line side");

  addFillProperties("fillUnder/fill", "fillUnderFill", "Fill under");

  // impulse
  addProp("impulse", "impulseLines", "visible", "Impulse lines visible");

  addLineProperties("impulse/stroke", "impulseLines", "Impulse lines");

  //---

  // vectors
  addProp("vectors", "vectors", "visible", "Vectors at points visible");

  QString vectorLinePath = "vectors/line";

  addArrowStyleProp(vectorLinePath, "lineWidth", "width", "Vector arrow connecting line width");

  QString vectorFrontHeadPath = "vectors/frontHead";

//addArrowProp     (vectorFrontHeadPath, "frontVisible"  , "visible",
//                  "Vector arrow front head visible");
  addArrowProp     (vectorFrontHeadPath, "frontType"     , "type",
                    "Vector arrow front head type");
  addArrowStyleProp(vectorFrontHeadPath, "frontLength"   , "length",
                    "Vector arrow front head length");
  addArrowStyleProp(vectorFrontHeadPath, "frontAngle"    , "angle",
                    "Vector arrow front head angle");
//addArrowStyleProp(vectorFrontHeadPath, "frontBackAngle", "backAngle",
//                  "Vector arrow front head back angle");
//addArrowStyleProp(vectorFrontHeadPath, "frontLineEnds" , "line",
//                  "Vector arrow front head back is drawn using lines");

  QString vectorTailHeadPath = "vectors/tailHead";

//addArrowProp     (vectorTailHeadPath, "tailVisible"  , "visible",
//                  "Vector arrow tail head visible");
  addArrowProp     (vectorTailHeadPath, "tailType"     , "type",
                    "Vector arrow tail head type");
  addArrowStyleProp(vectorTailHeadPath, "tailLength"   , "length",
                    "Vector arrow tail head length");
  addArrowStyleProp(vectorTailHeadPath, "tailAngle"    , "angle",
                    "Vector arrow tail head angle");
//addArrowStyleProp(vectorTailHeadPath, "tailBackAngle", "backAngle",
//                  "Vector arrow tail head back angle");
//addArrowStyleProp(vectorTailHeadPath, "tailLineEnds" , "line",
//                  "Vector arrow tail head back is drawn using lines");

  QString vectorFillPath = "vectors/fill";

  addArrowStyleProp(vectorFillPath, "filled"   , "visible", "Vector arrow fill visible");
  addArrowStyleProp(vectorFillPath, "fillColor", "color"  , "Vector arrow fill color");
  addArrowStyleProp(vectorFillPath, "fillAlpha", "alpha"  , "Vector arrow fill alpha");

  QString vectorStrokePath = "vectors/stroke";

  addArrowStyleProp(vectorStrokePath, "stroked"    , "visible", "Vector stroke visible");
  addArrowStyleProp(vectorStrokePath, "strokeColor", "color"  , "Vector stroke color");
  addArrowStyleProp(vectorStrokePath, "strokeAlpha", "alpha"  , "Vector stroke alpha");
  addArrowStyleProp(vectorStrokePath, "strokeWidth", "width"  , "Vector stroke width");

  // data labels
  dataLabel()->addPathProperties("labels", "Labels");

  //---

  CQChartsPointPlot::addProperties();

  //---

  CQChartsPointPlot::addPointProperties();

  // color map
  addColorMapProperties();
}

//---

QColor
CQChartsXYPlot::
interpPaletteColor(const ColorInd &ind, bool scale) const
{
  if (isOverlay()) {
    int i = ind.i;
    int n = ind.n;

    if (prevPlot() || nextPlot()) {
      auto *plot1 = prevPlot();
      auto *plot2 = nextPlot();

      while (plot1) { ++n; plot1 = plot1->prevPlot(); }
      while (plot2) { ++n; plot2 = plot2->nextPlot(); }
    }

    //---

    auto *plot1 = prevPlot();

    while (plot1) {
      ++i;

      plot1 = plot1->prevPlot();
    }

    return CQChartsPlot::interpPaletteColor(ColorInd(i, n), scale);
  }
  else {
    return CQChartsPlot::interpPaletteColor(ind, scale);
  }
}

//---

CQChartsGeom::Range
CQChartsXYPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsXYPlot::calcRange");

  auto *th = const_cast<CQChartsXYPlot *>(this);

  //---

  // check columns
  bool columnsValid = true;

  th->clearErrors();

  if (! checkColumn (xColumn(), "X", th->xColumnType_, /*required*/true))
    columnsValid = false;
  if (! checkColumns(yColumns(), "Y", /*required*/true))
    columnsValid = false;

  if (! checkColumn(labelColumn(), "Label")) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  initGroupData(CQChartsColumns(), CQChartsColumn());

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsXYPlot *plot) :
     plot_(plot) {
      int ns = plot_->numSets();

      sum_.resize(ns);

      if (plot_->isColumnSeries())
        plot_->headerSeriesData(sx_);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex ind(data.row, plot_->xColumn(), data.parent);

      // init group
      (void) plot_->rowGroupInd(ind);

      //---

      lastSum_ = sum_;

      //---

      // get x and y values
      double x; std::vector<double> y; QModelIndex rowInd;

      if (! plot_->rowData(data, x, y, rowInd, plot_->isSkipBad()))
        return State::SKIP;

      int ny = y.size();

      //---

      if     (plot_->isStacked()) {
        // TODO: support stacked and cumulative
        double sum1 = 0.0;

        for (int i = 0; i < ny; ++i)
          sum1 += y[i];

        range_.updateRange(x, 0.0);
        range_.updateRange(x, sum1);
      }
      else if (plot_->isCumulative()) {
        if (! plot_->isColumnSeries()) {
          for (int i = 0; i < ny; ++i) {
            double y1 = y[i] + lastSum_[i];

            sum_[i] += y[i];

            range_.updateRange(x, y1);
          }
        }
      }
      else {
        if (! plot_->isColumnSeries()) {
          for (int i = 0; i < ny; ++i)
            range_.updateRange(x, y[i]);
        }
        else {
          for (int i = 0; i < ny; ++i)
            range_.updateRange(sx_[i], y[i]);
        }
      }

      return State::OK;
    }

    const Range &range() const { return range_; }

   private:
    using Reals = std::vector<double>;

    const CQChartsXYPlot* plot_ { nullptr };
    Range                 range_;
    Reals                 sum_;
    Reals                 lastSum_;
    std::vector<double>   sx_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  auto dataRange = visitor.range();

  if (isInterrupt())
    return dataRange;

  //---

  //dataRange = adjustDataRange(dataRange);

  //---

  // ensure range non-zero
  dataRange.makeNonZero();

  //---

  th->initAxes();

  //---

  return dataRange;
}

void
CQChartsXYPlot::
initAxes()
{
  setXValueColumn(xColumn());
  setYValueColumn(yColumns().column());

  xAxis()->setDefLabel("", /*notify*/false);
  yAxis()->setDefLabel("", /*notify*/false);

  //---

  // set x axis column and name
  CQChartsColumn xAxisColumn;

  if (! isColumnSeries())
    xAxisColumn = xColumn();
  else {
    xAxisColumn =
      CQChartsColumn(CQChartsColumn::Type::HHEADER, yColumns().getColumn(0).column(), "");
  }

  if (isOverlay()) {
    if (isFirstPlot() || isX1X2())
      xAxis()->setColumn(xAxisColumn);
  }
  else {
    xAxis()->setColumn(xAxisColumn);
  }

  //-

  QString xname;

  (void) xAxisName(xname, "X");

  if (isOverlay()) {
    if (isFirstPlot() || isX1X2())
      xAxis()->setDefLabel(xname, /*notify*/false);
  }
  else {
    xAxis()->setDefLabel(xname, /*notify*/false);
  }

  if (xColumnType_ == ColumnType::TIME)
    xAxis()->setValueType(CQChartsAxisValueType::Type::DATE, /*notify*/false);

  //---

  // set y axis column and name(s)
  QString yname;

  (void) yAxisName(yname, "Y");

  int ns = numSets();

  if      (isBivariateLines() && ns > 1) {
    if (isOverlay()) {
      if (isY1Y2()) {
        yAxis()->setDefLabel(yname, /*notify*/false);
      }
    }
    else {
      yAxis()->setDefLabel(yname, /*notify*/false);
    }
  }
  else if (isStacked()) {
  }
  else {
    auto yColumn = yColumns().getColumn(0);

    if (isOverlay()) {
      if (isFirstPlot() || isY1Y2())
        yAxis()->setColumn(yColumn);
    }
    else
      yAxis()->setColumn(yColumn);

    if (isOverlay()) {
      if (isY1Y2()) {
        yAxis()->setDefLabel(yname, /*notify*/false);
      }
    }
    else {
      yAxis()->setDefLabel(yname, /*notify*/false);
    }
  }

  //---

  if (isOverlay() && isFirstPlot())
    setOverlayPlotsAxisNames();
}

bool
CQChartsXYPlot::
xAxisName(QString &name, const QString &def) const
{
  return xColumnName(name, def, false);
}

bool
CQChartsXYPlot::
xColumnName(QString &name, const QString &def, bool tip) const
{
  bool ok;

  name = (tip ? modelHHeaderTip(xColumn(), ok) : modelHHeaderString(xColumn(), ok));

  if (! ok || ! name.length())
    name = def;

  return name.length();
}

bool
CQChartsXYPlot::
yAxisName(QString &name, const QString &def) const
{
  return yColumnName(name, def, false);
}

bool
CQChartsXYPlot::
yColumnName(QString &name, const QString &def, bool tip) const
{
  int ns = numSets();

  if      (isBivariateLines() && ns > 1) {
    name = titleStr();

    if (! name.length()) {
      auto yColumn1 = yColumns().getColumn(0);

      CQChartsColumn yColumn2;

      if (ns > 1)
        yColumn2 = yColumns().getColumn(1);

      bool ok1, ok2;

      QString yname1 = (tip ? modelHHeaderTip(yColumn1, ok1) : modelHHeaderString(yColumn1, ok1));
      QString yname2 = (tip ? modelHHeaderTip(yColumn2, ok2) : modelHHeaderString(yColumn2, ok2));

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }
  }
  else if (isStacked()) {
  }
  else {
    if (! isColumnSeries()) {
      for (int j = 0; j < ns; ++j) {
        bool ok;

        auto yColumn = yColumns().getColumn(j);

        QString name1 = (tip ? modelHHeaderTip(yColumn, ok) : modelHHeaderString(yColumn, ok));
        if (! ok || ! name1.length()) continue;

        if (name.length())
          name += ", ";

        name += name1;
      }
    }
  }

  if (! name.length())
    name = def;

  return name.length();
}

//---

void
CQChartsXYPlot::
setImpulseLinesSlot(bool b)
{
  setImpulseLines(b);
}

void
CQChartsXYPlot::
setBivariateLinesSlot(bool b)
{
  setBivariateLines(b);
}

void
CQChartsXYPlot::
setFillUnderFilledSlot(bool b)
{
  setFillUnderFilled(b);
}

//---

bool
CQChartsXYPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsXYPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsXYPlot *>(this);

  //---

  const auto &dataRange = this->dataRange();
  if (! dataRange.isSet()) return false;

  // TODO: use actual symbol size
  th->symbolWidth_  = (dataRange.xmax() - dataRange.xmin())/100.0;
  th->symbolHeight_ = (dataRange.ymax() - dataRange.ymin())/100.0;

  //---

  th->updateColumnNames();

  //---

  GroupSetIndPoly groupSetIndPoly;

  createGroupSetIndPoly(groupSetIndPoly);

  (void) createGroupSetObjs(groupSetIndPoly, objs);

  //---

  return true;
}

void
CQChartsXYPlot::
updateColumnNames()
{
  // set column header names
  CQChartsPlot::updateColumnNames();

  setColumnHeaderName(labelColumn     (), "Label"     );
  setColumnHeaderName(symbolTypeColumn(), "SymbolType");
  setColumnHeaderName(symbolSizeColumn(), "SymbolSize");
  setColumnHeaderName(fontSizeColumn  (), "FontSize"  );
}

void
CQChartsXYPlot::
createGroupSetIndPoly(GroupSetIndPoly &groupSetIndPoly) const
{
  CQPerfTrace trace("CQChartsXYPlot::createGroupSetIndPoly");

  // create line per set
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsXYPlot *plot) :
     plot_(plot) {
      ns_ = plot_->numSets();

      if (plot_->isColumnSeries())
        plot_->headerSeriesData(sx_);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex ind(data.row, plot_->xColumn(), data.parent);

      // get group
      int groupInd = plot_->rowGroupInd(ind);

      SetIndPoly &setPoly = groupSetPoly_[groupInd];

      if (setPoly.empty())
        setPoly.resize(ns_);

      //---

      // get x and y values
      double x; std::vector<double> y; QModelIndex rowInd;

      if (! plot_->rowData(data, x, y, rowInd, plot_->isSkipBad()))
        return State::SKIP;

      int ny = y.size();

      //---

      if (! plot_->isColumnSeries()) {
        assert(ny == ns_);
      }

      if (! plot_->isColumnSeries()) {
        for (int i = 0; i < ny; ++i) {
          setPoly[i].inds.push_back(rowInd);

          setPoly[i].poly.addPoint(Point(x, y[i]));
        }
      }
      else {
        for (int i = 0; i < ny; ++i) {
          setPoly[data.row].inds.push_back(rowInd);

          setPoly[data.row].poly.addPoint(Point(sx_[i], y[i]));
        }
      }

      return State::OK;
    }

    // stack lines
    void stack() {
      for (auto &p : groupSetPoly_) {
        SetIndPoly &setPoly = p.second;

        for (int i = 1; i < ns_; ++i) {
          auto &poly1 = setPoly[i - 1].poly;
          auto &poly2 = setPoly[i    ].poly;

          int np = poly1.size();
          assert(poly2.size() == np);

          for (int j = 0; j < np; ++j) {
            auto p1 = poly1.point(j);

            double y1 = p1.y;

            auto p2 = poly2.point(j);

            double x2 = p2.x, y2 = p2.y;

            if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
              continue;

            poly2.setPoint(j, Point(x2, y2 + y1));
          }
        }
      }
    }

    // cumulate
    void cumulate() {
      for (auto &p : groupSetPoly_) {
        SetIndPoly &setPoly = p.second;

        for (int i = 0; i < ns_; ++i) {
          auto &poly = setPoly[i].poly;

          int np = poly.size();

          for (int j = 1; j < np; ++j) {
            auto p1 = poly.point(j - 1);
            auto p2 = poly.point(j    );

            double y1 = p1.y;
            double x2 = p2.x, y2 = p2.y;

            if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
              continue;

            poly.setPoint(j, Point(x2, y1 + y2));
          }
        }
      }
    }

    const GroupSetIndPoly &groupSetPoly() const { return groupSetPoly_; }

   private:
    const CQChartsXYPlot* plot_ { nullptr };
    int                   ns_;
    std::vector<double>   sx_;
    GroupSetIndPoly       groupSetPoly_;
  };

  //---

  RowVisitor visitor(this);

  visitModel(visitor);

  if      (isStacked())
    visitor.stack();
  else if (isCumulative())
    visitor.cumulate();

  groupSetIndPoly = visitor.groupSetPoly();
}

bool
CQChartsXYPlot::
createGroupSetObjs(const GroupSetIndPoly &groupSetIndPoly, PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsXYPlot::createGroupSetObjs");

  //---

  int ns = numSets();

  int ig = 0;
  int ng = groupSetIndPoly.size();

  if (ng <= 1 && parentPlot()) {
    ig = parentPlot()->childPlotIndex(this);
    ng = parentPlot()->numChildPlots();
  }

  for (auto &p : groupSetIndPoly) {
    if (isInterrupt())
      return false;

    ColorInd colorInd(ig, ng);

    bool hidden = (ns <= 1 && isSetHidden(ig));

    if (! hidden) {
      int               groupInd = p.first;
      const SetIndPoly &setPoly  = p.second;

      if      (isBivariateLines()) {
        if (! addBivariateLines(groupInd, setPoly, colorInd, objs))
          return false;
      }
      else {
        if (! addLines(groupInd, setPoly, colorInd, objs))
          return false;
      }
    }

    ++ig;
  }

  return true;
}

bool
CQChartsXYPlot::
addBivariateLines(int groupInd, const SetIndPoly &setPoly,
                  const ColorInd &ig, PlotObjs &objs) const
{
  auto *th = const_cast<CQChartsXYPlot *>(this);

  //---

  double sw = symbolWidth ();
  double sh = symbolHeight();

  PlotObjs pointObjs;

  //---

  // convert lines bivariate lines (line connected each point pair)
  int ns = setPoly.size();

  if (ns < 1)
    return false;

  SetIndPoly polygons1, polygons2;

  polygons1.resize(ns - 1);
  polygons2.resize(ns - 1);

  const auto &poly = setPoly[0].poly;

  int np = poly.size();

  for (int ip = 0; ip < np; ++ip) {
    if (isInterrupt())
      return false;

    double x = 0.0;

    // sorted y vals
    std::set<double> sortedYVals;

    for (int j = 0; j < ns; ++j) {
      bool hidden = isSetHidden(j);

      if (hidden)
        continue;

      //---

      const auto &poly = setPoly[j].poly;

      auto p = poly.point(ip);

      if (CMathUtil::isNaN(p.y) || CMathUtil::isInf(p.y))
        continue;

      if (j == 0)
        x = p.x;

      sortedYVals.insert(p.y);
    }

    // need at least 2 values
    if (sortedYVals.size() < 2)
      continue;

    //---

    std::vector<double> yVals;

    for (const auto &y : sortedYVals)
      yVals.push_back(y);

    //---

    QModelIndex parent; // TODO: parent

    CQChartsModelIndex xModelInd(ip, xColumn(), parent);

    QModelIndex xind  = modelIndex(xModelInd);
    QModelIndex xind1 = normalizeIndex(xind);

    //---

    // connect each y value to next y value
    double y1  = yVals[0];
    int    ny1 = yVals.size();

    for (int j = 1; j < ny1; ++j) {
      if (isInterrupt())
        return false;

      double y2 = yVals[j];

      BBox bbox(x - sw/2, y1 - sh/2, x + sw/2, y2 + sh/2);

      if (! isFillUnderFilled()) {
        // use vertical line object for each point pair if not fill under
        ColorInd is(j - 1, ny1 - 1);
        ColorInd iv(ip, np);

        auto *lineObj = th->createBiLineObj(groupInd, bbox, x, y1, y2, xind1, is, iv);

        objs.push_back(lineObj);
      }
      else {
        auto &poly1 = polygons1[j - 1].poly;
        auto &poly2 = polygons2[j - 1].poly;

        // build lower and upper poly line for fill under polygon
        poly1.addPoint(Point(x, y1));
        poly2.addPoint(Point(x, y2));
      }

      y1 = y2;
    }
  }

  //---

  // add lower, upper and polygon objects for fill under
  if (isFillUnderFilled()) {
    QString name = titleStr();

    if (! name.length()) {
      auto yColumn1 = yColumns().getColumn(0);

      CQChartsColumn yColumn2;

      if (yColumns().count() > 1)
        yColumn2 = yColumns().getColumn(1);

      bool ok;

      QString yname1 = modelHHeaderString(yColumn1, ok);
      QString yname2 = modelHHeaderString(yColumn2, ok);

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    int ns = numSets();

    for (int j = 1; j < ns; ++j) {
      bool hidden = isSetHidden(j);

      if (hidden)
        continue;

      //---

      QString name1;

      if (ns > 2)
        name1 = QString("%1:%2").arg(name).arg(j);
      else
        name1 = name;

      //---

      auto &poly1 = polygons1[j - 1].poly;
      auto &poly2 = polygons2[j - 1].poly;

      ColorInd is1(j - 1, ns - 1);

      addPolyLine(poly1, groupInd, is1, ig, name1, pointObjs, objs);
      addPolyLine(poly2, groupInd, is1, ig, name1, pointObjs, objs);

      int len = poly1.size();

      auto fillUnderSideType = fillUnderSide().type();

      if      (fillUnderSideType == CQChartsFillUnderSide::Type::BOTH) {
        // add upper poly line to lower one (points reversed) to build fill polygon
        for (int k = len - 1; k >= 0; --k)
          poly1.addPoint(poly2.point(k));
      }
      else if (fillUnderSideType == CQChartsFillUnderSide::Type::ABOVE) {
        Polygon poly3, poly4;
        Point   pa1, pb1;

        bool above1 = true;

        for (int k = 0; k < len; ++k) {
          auto pa2 = poly1.point(k);
          auto pb2 = poly2.point(k);

          bool above2 = (pa2.y > pb2.y);

          if (k > 0 && above1 != above2) {
            Point pi;

            CQChartsUtil::intersectLines(pa1, pa2, pb1, pb2, pi);

            poly3.addPoint(pi);
            poly4.addPoint(pi);
          }

          if (above2) {
            poly3.addPoint(pa2);
            poly4.addPoint(pb2);
          }

          pa1 = pa2; pb1 = pb2; above1 = above2;
        }

        len = poly4.size();

        for (int k = len - 1; k >= 0; --k)
          poly3.addPoint(poly4.point(k));

        poly1 = poly3;
      }
      else if (fillUnderSideType == CQChartsFillUnderSide::Type::BELOW) {
        Polygon poly3, poly4;
        Point   pa1, pb1;

        bool below1 = true;

        for (int k = 0; k < len; ++k) {
          auto pa2 = poly1.point(k);
          auto pb2 = poly2.point(k);

          bool below2 = (pa2.y < pb2.y);

          if (k > 0 && below1 != below2) {
            Point pi;

            CQChartsUtil::intersectLines(pa1, pa2, pb1, pb2, pi);

            poly3.addPoint(pi);
            poly4.addPoint(pi);
          }

          if (below2) {
            poly3.addPoint(pa2);
            poly4.addPoint(pb2);
          }

          pa1 = pa2; pb1 = pb2; below1 = below2;
        }

        len = poly4.size();

        for (int k = len - 1; k >= 0; --k)
          poly3.addPoint(poly4.point(k));

        poly1 = poly3;
      }

      addPolygon(poly1, groupInd, is1, ig, name1, objs, /*under*/false);
    }
  }

  return true;
}

bool
CQChartsXYPlot::
addLines(int groupInd, const SetIndPoly &setPoly, const ColorInd &ig, PlotObjs &objs) const
{
  auto *th = const_cast<CQChartsXYPlot *>(this);

  PlotObjs polyLineObjs, pointObjs, labelObjs, impulseLineObjs, polygonObjs;

  //---

  initSymbolTypeData();
  initSymbolSizeData();
  initFontSizeData  ();

  //---

  PlotObjs linePointObjs;

  double sw = symbolWidth ();
  double sh = symbolHeight();

  //---

  const auto &dataRange = this->dataRange();

  // convert lines into set polygon and set poly lines (more than one if NaNs)
  int ns = numSets();

  for (int is = 0; is < ns; ++is) {
    if (isInterrupt())
      return false;

    bool hidden = (ns > 1 && isSetHidden(is));

    if (hidden)
      continue;

    //---

    // get column name
    QString name;

    if (! isColumnSeries()) {
      auto yColumn = yColumns().getColumn(is);

      bool ok;

      name = modelHHeaderString(yColumn, ok);
    }

    if (ig.n > 1)
      name = groupIndName(groupInd);

    //---

    const IndPoly &setPoly1 = setPoly[is];

    const auto &poly = setPoly1.poly;
    const auto &inds = setPoly1.inds;

    const IndPoly &setPoly2 = setPoly[is - 1];

    const auto &prevPoly = (is > 0 ? setPoly2.poly : poly);

    //---

    Polygon polyShape, polyLine;

    int np = poly.size();
    assert(prevPoly.size() == np);

    auto pointStartIndex = [&]() {
      if      (pointStart_ == 0)
        return 0;
      else if (pointStart_ == -1)
        return np - 1;
      else if (pointStart_ == -2)
        return np/2;
      else
        return pointStart_;
    };

    auto validPointIndex = [&](int ip, int np) {
      if      (pointCount_ == 1) {
        return (ip == pointStartIndex());
      }
      else if (pointCount_ > 1) {
        if (ip == 0 || ip == np - 1)
          return true;

        int d = np/(pointCount_ - 1);

        return ((ip % d) == 0);
      }
      else if (pointDelta_ > 1) {
        return ((ip % pointDelta_) == 0);
      }
      else {
        return true;
      }
    };

    // add points to fill under polygon, poly line, and create point objects
    for (int ip = 0; ip < np; ++ip) {
      if (isInterrupt())
        return false;

      //---

      // get polygon point
      auto p = poly.point(ip);

      double x = p.x, y = p.y;

      //---

      // if point is invalid start new poly line if needed and skip
      if (CMathUtil::isNaN(x) || CMathUtil::isInf(x) ||
          CMathUtil::isNaN(y) || CMathUtil::isInf(y)) {
        if (polyLine.size()) {
          ColorInd is1(is, ns);

          addPolyLine(polyLine, groupInd, is1, ig, name, linePointObjs, polyLineObjs);

          linePointObjs.clear();

          polyLine = Polygon();
        }

        continue;
      }

      //---

      bool valid = validPointIndex(ip, np);

      if (valid) {
        const QModelIndex &xind = inds[ip];

        QModelIndex xind1 = normalizeIndex(xind);

        //---

        // get symbol size (needed for bounding box)
        CQChartsLength symbolSize(CQChartsUnits::NONE, 0.0);

        if (symbolSizeColumn().isValid()) {
          if (! columnSymbolSize(ip, xind1.parent(), symbolSize))
            symbolSize = CQChartsLength(CQChartsUnits::NONE, 0.0);
        }

        double sx, sy;

        plotSymbolSize(symbolSize.isValid() ? symbolSize : this->symbolSize(), sx, sy);

        //---

        // create point object
        ColorInd is1(is, ns);
        ColorInd iv1(ip, np);

        BBox bbox(p.x - sx, p.y - sy, p.x + sx, p.y + sy);

        auto *pointObj = th->createPointObj(groupInd, bbox, p, is1, ig, iv1);

        pointObj->setModelInd(xind1);

        if (symbolSize.isValid())
          pointObj->setSymbolSize(symbolSize);

        pointObjs.push_back(pointObj);

        linePointObjs.push_back(pointObj);

        //---

        // set optional symbol type
        CQChartsSymbol symbolType(CQChartsSymbol::Type::NONE);

        if (symbolTypeColumn().isValid()) {
          if (! columnSymbolType(ip, xind1.parent(), symbolType))
            symbolType = CQChartsSymbol(CQChartsSymbol::Type::NONE);
        }

        if (symbolType.isValid())
          pointObj->setSymbolType(symbolType);

        //---

        // set optional font size
        CQChartsLength fontSize(CQChartsUnits::NONE, 0.0);

        if (fontSizeColumn().isValid()) {
          if (! columnFontSize(ip, xind1.parent(), fontSize))
            fontSize = CQChartsLength(CQChartsUnits::NONE, 0.0);
        }

        if (fontSize.isValid())
          pointObj->setFontSize(fontSize);

        //---

        // set optional symbol fill color
        CQChartsColor symbolColor(CQChartsColor::Type::NONE);

        if (colorColumn().isValid()) {
          if (! colorColumnColor(ip, xind1.parent(), symbolColor))
            symbolColor = CQChartsColor(CQChartsColor::Type::NONE);
        }

        if (symbolColor.isValid())
          pointObj->setColor(symbolColor);

        //---

        // set optional point label
        QString pointName;

        if (labelColumn().isValid()) {
          CQChartsModelIndex labelModelInd(ip, labelColumn(), xind1.parent());

          bool ok;

          pointName = modelString(labelModelInd, ok);

          if (! ok)
            pointName = "";
        }

        if (pointName.length()) {
          BBox bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

          auto *labelObj = th->createLabelObj(groupInd, bbox, x, y, pointName, xind1, is1, iv1);

          labelObjs.push_back(labelObj);

          labelObj->setPointObj(pointObj);
          pointObj->setLabelObj(labelObj);
        }

        //---

        // set optional image
        CQChartsImage image;

        if (imageColumn().isValid()) {
          CQChartsModelIndex imageColumnInd(ip, imageColumn(), xind1.parent());

          bool ok;

          QVariant imageVar = modelValue(imageColumnInd, ok);

          if (ok)
            image = CQChartsVariant::toImage(imageVar, ok);
        }

        if (image.isValid())
          pointObj->setImage(image);

        //---

        // set vector data
        if (isVectors()) {
          QModelIndex parent; // TODO: parent

          double vx = 0.0, vy = 0.0;

          if (vectorXColumn().isValid()) {
            bool ok;

            CQChartsModelIndex vectorXInd(ip, vectorXColumn(), parent);

            vx = modelReal(vectorXInd, ok);

            if (! ok)
              th->addDataError(vectorXInd, "Invalid Vector X");
          }

          if (vectorYColumn().isValid()) {
            bool ok;

            CQChartsModelIndex vectorYInd(ip, vectorYColumn(), parent);

            vy = modelReal(vectorYInd, ok);

            if (! ok)
              th->addDataError(vectorYInd, "Invalid Vector Y");
          }

          pointObj->setVector(Point(vx, vy));
        }

        //---

        // add impulse line (down to or up to zero)
        if (! isStacked() && isImpulseLines()) {
          double w = lengthPlotWidth(impulseLinesWidth());

          double ys = std::min(y, 0.0);
          double ye = std::max(y, 0.0);

          BBox bbox(x - w/2, ys, x + w/2, ye);

          auto *impulseObj = th->createImpulseLineObj(groupInd, bbox, x, ys, ye, xind1, is1, iv1);

          impulseLineObjs.push_back(impulseObj);
        }
      }

      //---

      // add point to poly line
      polyLine.addPoint(p);

      //---

      // add point to polygon

      // if first point then add first point of previous polygon
      if (ip == 0) {
        if (isStacked()) {
          double y1 = (is > 0 ? prevPoly.point(ip).y : dataRange.ymin());

          if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
            y1 = dataRange.ymin();

          polyShape.addPoint(Point(x, y1));
        }
        else {
          polyShape.addPoint(calcFillUnderPos(x, dataRange.ymin()));
        }
      }

      polyShape.addPoint(p);

      // if last point then add last point of previous polygon
      if (ip == np - 1) {
        if (isStacked()) {
          double y1 = (is > 0 ? prevPoly.point(ip).y : dataRange.ymin());

          if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
            y1 = dataRange.ymin();

          polyShape.addPoint(Point(x, y1));
        }
        else {
          polyShape.addPoint(calcFillUnderPos(x, dataRange.ymin()));
        }
      }
    }

    //---

    if (isStacked()) {
      // add points from previous polygon to bottom of polygon
      if (is > 0) {
        for (int ip = np - 2; ip >= 1; --ip) {
          double x1 = prevPoly.point(ip).x;
          double y1 = prevPoly.point(ip).y;

          if (CMathUtil::isNaN(x1) || CMathUtil::isInf(x1))
            x1 = poly.point(ip).x;

          if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
            y1 = 0.0;

          polyShape.addPoint(Point(x1, y1));
        }
      }
    }

    //---

    if (polyLine.size()) {
      ColorInd is1(is, ns);

      addPolyLine(polyLine, groupInd, is1, ig, name, linePointObjs, polyLineObjs);

      linePointObjs.clear();

      //polyLine = Polygon();
    }

    //---

    ColorInd is1(is, ns);

    addPolygon(polyShape, groupInd, is1, ig, name, polygonObjs, /*under*/true);
  }

  //---

  for (auto &obj : polygonObjs)
    objs.push_back(obj);

  for (auto &obj : polyLineObjs)
    objs.push_back(obj);

  for (auto &obj : impulseLineObjs)
    objs.push_back(obj);

  for (auto &obj : pointObjs)
    objs.push_back(obj);

  for (auto &obj : labelObjs)
    objs.push_back(obj);

  return true;
}

bool
CQChartsXYPlot::
headerSeriesData(std::vector<double> &x) const
{
  int nc = yColumns().count();

  for (int i = 0; i < nc; ++i) {
    auto yColumn = yColumns().getColumn(i);

    bool ok;

    QVariant var = CQChartsModelUtil::modelHeaderValue(model().data(), yColumn, Qt::EditRole, ok);

    if (! ok)
      var = CQChartsModelUtil::modelHeaderValue(model().data(), yColumn, Qt::DisplayRole, ok);

    double r = CQChartsVariant::toReal(var, ok);

    if (! ok)
      r = i;

    x.push_back(r);
  }

  return true;
}

bool
CQChartsXYPlot::
rowData(const ModelVisitor::VisitData &data, double &x, std::vector<double> &y,
        QModelIndex &ind, bool skipBad) const
{
  auto *th = const_cast<CQChartsXYPlot *>(this);

  //---

  // get x value (must be valid)
  CQChartsModelIndex xModelInd(data.row, xColumn(), data.parent);

  ind = modelIndex(xModelInd);

  bool ok1 = modelMappedReal(xModelInd, x, isLogX(), data.row);

  if (! ok1) {
    th->addDataError(xModelInd, "Invalid X Value");
    return false;
  }

  //---

  // get y values (optionally skip bad)
  // TODO: differentiate between bad value and empty value
  bool ok2 = true;

  int nc = yColumns().count();

  for (int i = 0; i < nc; ++i) {
    auto yColumn = yColumns().getColumn(i);

    CQChartsModelIndex yModelInd(data.row, yColumn, data.parent);

    double y1;

    bool ok3 = modelMappedReal(yModelInd, y1, isLogY(), data.row);

    if (! ok3) {
      y1 = CMathUtil::getNaN();

      if (! skipBad)
        ok2 = false;
    }

    //---

    y.push_back(y1);
  }

  if (nc && y.empty())
    return false;

  return (ok1 && ok2);
}

int
CQChartsXYPlot::
numSets() const
{
  if (isColumnSeries())
    return model()->rowCount();
  else
    return yColumns().count();
}

CQChartsGeom::Point
CQChartsXYPlot::
calcFillUnderPos(double x, double y) const
{
  const auto &pos = fillUnderPos();

  double x1 = x;
  double y1 = y;

  const auto &dataRange = this->dataRange();

  if      (pos.xtype() == CQChartsFillUnderPos::Type::MIN) {
    if (dataRange.isSet())
      x1 = dataRange.xmin();
  }
  else if (pos.xtype() == CQChartsFillUnderPos::Type::MAX) {
    if (dataRange.isSet())
      x1 = dataRange.xmax();
  }
  else if (pos.xtype() == CQChartsFillUnderPos::Type::POS)
    x1 = pos.xpos();

  if      (pos.ytype() == CQChartsFillUnderPos::Type::MIN) {
    if (dataRange.isSet())
      y1 = dataRange.ymin();
  }
  else if (pos.ytype() == CQChartsFillUnderPos::Type::MAX) {
    if (dataRange.isSet())
      y1 = dataRange.ymax();
  }
  else if (pos.ytype() == CQChartsFillUnderPos::Type::POS)
    y1 = pos.ypos();

  return Point(x1, y1);
}

CQChartsXYPolylineObj *
CQChartsXYPlot::
addPolyLine(const Polygon &polyLine, int groupInd, const ColorInd &is,
            const ColorInd &ig, const QString &name, PlotObjs &pointObjs, PlotObjs &objs) const
{
  auto bbox = polyLine.boundingBox();
  if (! bbox.isSet()) return nullptr;

  auto *th = const_cast<CQChartsXYPlot *>(this);

  auto *lineObj = th->createPolylineObj(groupInd, bbox, polyLine, name, is, ig);

  for (auto &pointObj : pointObjs) {
    auto *pointObj1 = dynamic_cast<CQChartsXYPointObj *>(pointObj);

    if (pointObj1)
      pointObj1->setLineObj(lineObj);
  }

  objs.push_back(lineObj);

  return lineObj;
}

void
CQChartsXYPlot::
addPolygon(const Polygon &poly, int groupInd, const ColorInd &is,
           const ColorInd &ig, const QString &name, PlotObjs &objs, bool under) const
{
  auto bbox = poly.boundingBox();
  if (! bbox.isSet()) return;

  auto *th = const_cast<CQChartsXYPlot *>(this);

  auto *polyObj = th->createPolygonObj(groupInd, bbox, poly, name, is, ig, under);

  objs.push_back(polyObj);
}

//---

CQChartsXYPointObj *
CQChartsXYPlot::
createPointObj(int groupInd, const BBox &rect, const Point &p, const ColorInd &is,
               const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsXYPointObj(this, groupInd, rect, p, is, ig, iv);
}

CQChartsXYBiLineObj *
CQChartsXYPlot::
createBiLineObj(int groupInd, const BBox &rect, double x, double y1, double y2,
                const QModelIndex &ind, const ColorInd &is, const ColorInd &iv) const
{
  return new CQChartsXYBiLineObj(this, groupInd, rect, x, y1, y2, ind, is, iv);
}

CQChartsXYLabelObj *
CQChartsXYPlot::
createLabelObj(int groupInd, const BBox &rect, double x, double y, const QString &label,
               const QModelIndex &ind, const ColorInd &is, const ColorInd &iv) const
{
  return new CQChartsXYLabelObj(this, groupInd, rect, x, y, label, ind, is, iv);
}

CQChartsXYImpulseLineObj *
CQChartsXYPlot::
createImpulseLineObj(int groupInd, const BBox &rect, double x, double y1, double y2,
                     const QModelIndex &ind, const ColorInd &is, const ColorInd &iv) const
{
  return new CQChartsXYImpulseLineObj(this, groupInd, rect, x, y1, y2, ind, is, iv);
}

CQChartsXYPolylineObj *
CQChartsXYPlot::
createPolylineObj(int groupInd, const BBox &rect, const Polygon &poly, const QString &name,
                  const ColorInd &is, const ColorInd &ig) const
{
  return new CQChartsXYPolylineObj(this, groupInd, rect, poly, name, is, ig);
}

CQChartsXYPolygonObj *
CQChartsXYPlot::
createPolygonObj(int groupInd, const BBox &rect, const Polygon &poly, const QString &name,
                 const ColorInd &is, const ColorInd &ig, bool under) const
{
  return new CQChartsXYPolygonObj(this, groupInd, rect, poly, name, is, ig, under);
}

//---

QString
CQChartsXYPlot::
valueName(int is, int ns, int irow, bool tip) const
{
  QString name;

  if (ns > 1 && is >= 0) {
    if (! isColumnSeries()) {
      auto yColumn = yColumns().getColumn(is);

      bool ok;

      name = (tip ? modelHHeaderTip(yColumn, ok) : modelHHeaderString(yColumn, ok));
    }
  }

  if (labelColumn().isValid()) {
    QModelIndex parent; // TODO: parent

    CQChartsModelIndex labelModelInd(irow, labelColumn(), parent);

    bool ok;

    QString name1 = modelString(labelModelInd, ok);

    if (ok)
      return name1;
  }

  return name;
}

void
CQChartsXYPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyItem = [&](const QString &name, const ColorInd &is, const ColorInd &ig) {
    auto *color = new CQChartsXYKeyColor(this, is, ig);
    auto *text  = new CQChartsXYKeyText (this, name, is, ig);

    if (! key->isHorizontal()) {
      key->addItem(color, row, col    );
      key->addItem(text , row, col + 1);

      ++row;
    }
    else {
      key->addItem(color, 0, col++);
      key->addItem(text , 0, col++);
    }
  };

  //---

  int ns = numSets();
  int ng = numGroups();

  if      (isBivariateLines()) {
    QString name = titleStr();

    if (! name.length()) {
      auto yColumn1 = yColumns().getColumn(0);

      CQChartsColumn yColumn2;

      if (ns > 1)
        yColumn2 = yColumns().getColumn(1);

      bool ok;

      QString yname1 = modelHHeaderString(yColumn1, ok);
      QString yname2 = modelHHeaderString(yColumn2, ok);

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    ColorInd is, ig;

    addKeyItem(name, is, ig);
  }
  else if (isStacked()) {
    for (int i = 0; i < ns; ++i) {
      QString name;

      if (! isColumnSeries()) {
        auto yColumn = yColumns().getColumn(i);

        bool ok;

        name = modelHHeaderString(yColumn, ok);
      }

      ColorInd is(i, ns), ig;

      addKeyItem(name, is, ig);
    }
  }
  else {
    if      (ns > 1) {
      for (int i = 0; i < ns; ++i) {
        QString name;

        if (! isColumnSeries()) {
          auto yColumn = yColumns().getColumn(i);

          bool ok;

          name = modelHHeaderString(yColumn, ok);
        }

#if 0
        if (ns == 1 && (name == "" || name == QString("%1").arg(yColumn + 1))) {
          if      (titleStr().length())
            name = titleStr();
          else if (fileName().length())
            name = fileName();
        }
#endif
#if 0
        if (ns == 1 && ! isOverlay() && (titleStr().length() || fileName().length())) {
          if      (titleStr().length())
            name = titleStr();
          else if (fileName().length())
            name = fileName();
        }
#endif

        ColorInd is(i, ns), ig;

        addKeyItem(name, is, ig);
      }
    }
    else if (ng > 1) {
      for (int i = 0; i < ng; ++i) {
        QString name = groupIndName(i);

        ColorInd is, ig(i, ng);

        addKeyItem(name, is, ig);
      }
    }
    else {
      QString name = groupIndName(0);

      if (name == "")
        (void) yAxisName(name);

      if (name == "")
        name = titleStr();

      ColorInd is, ig;

      if (parentPlot()) {
        int i = parentPlot()->childPlotIndex(this);
        int n = parentPlot()->numChildPlots();

        ig = ColorInd(i, n);
      }

      addKeyItem(name, is, ig);
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

//---

bool
CQChartsXYPlot::
probe(ProbeData &probeData) const
{
  std::vector<double> yvals;

  if (! interpY(probeData.p.x, yvals))
    return false;

  for (const auto &yval : yvals)
    probeData.yvals.emplace_back(yval);

  return true;
}

bool
CQChartsXYPlot::
interpY(double x, std::vector<double> &yvals) const
{
  if (isBivariateLines()) {
    return false;
  }

  for (const auto &plotObj : plotObjs_) {
    auto *polyObj = dynamic_cast<CQChartsXYPolylineObj *>(plotObj);

    if (! polyObj)
      continue;

    std::vector<double> yvals1;

    polyObj->interpY(x, yvals1);

    for (const auto &y1 : yvals1)
      yvals.push_back(y1);
  }

  return ! yvals.empty();
}

//------

CQChartsPlotObj *
CQChartsXYPlot::
getGroupObj(int ig) const
{
  for (const auto &plotObj : plotObjs_) {
    auto *polyObj = dynamic_cast<CQChartsXYPolylineObj *>(plotObj);

    if (polyObj) {
      if (polyObj->is().n == 1 && polyObj->ig().n > 1 && polyObj->ig().i == ig)
        return polyObj;
    }
  }

  return nullptr;
}

//------

CQChartsGeom::BBox
CQChartsXYPlot::
dataFitBBox() const
{
  auto bbox = CQChartsPlot::dataFitBBox();

  for (const auto &plotObj : plotObjs_) {
    auto *labelObj = dynamic_cast<CQChartsXYLabelObj *>(plotObj);

    if (! labelObj)
      continue;

    bbox += labelObj->rect();
  }

  return bbox;
}

//------

bool
CQChartsXYPlot::
addMenuItems(QMenu *menu)
{
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name, bool isSet, const char *slot) {
    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  //--

  int ns = numSets();

  menu->addSeparator();

  //---

  if (ns > 1)
    addMenuCheckedAction(menu, "Bivariate", isBivariateLines(), SLOT(setBivariateLinesSlot(bool)));

  //---

  addMenuCheckedAction(menu, "Points", isPoints(), SLOT(setPointsSlot(bool)));
  addMenuCheckedAction(menu, "Lines" , isLines (), SLOT(setLinesSlot(bool)));

  menu->addSeparator();

  //---

  addMenuCheckedAction(menu, "Stacked"   , isStacked        (), SLOT(setStacked(bool)));
  addMenuCheckedAction(menu, "Cumulative", isCumulative     (), SLOT(setCumulative(bool)));
  addMenuCheckedAction(menu, "Impulse"   , isImpulseLines   (), SLOT(setImpulseLinesSlot(bool)));
  addMenuCheckedAction(menu, "Fill Under", isFillUnderFilled(), SLOT(setFillUnderFilledSlot(bool)));

  //---

  auto *overlaysMenu = new QMenu("Overlays", menu);

  (void) addMenuCheckedAction(overlaysMenu, "Best Fit"   ,
                              isBestFit   (), SLOT(setBestFit       (bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Hull"       ,
                              isHull      (), SLOT(setHull          (bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Statistic Lines",
                              isStatsLines(), SLOT(setStatsLinesSlot(bool)));

  menu->addMenu(overlaysMenu);

  //---

  return true;
}

//------

void
CQChartsXYPlot::
drawArrow(CQChartsPaintDevice *device, const Point &p1, const Point &p2) const
{
  CQChartsWidgetUtil::AutoDisconnect arrowDisconnect(
    arrowObj_, SIGNAL(dataChanged()), const_cast<CQChartsXYPlot *>(this), SLOT(updateSlot()));

  arrowObj_->setFrom(p1);
  arrowObj_->setTo  (p2);

  arrowObj_->draw(device);
}

//---

void
CQChartsXYPlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPointPlot::write(os, plotVarName, modelVarName, viewVarName);

  arrowObj_->write(os, plotVarName);
}

//------

CQChartsXYBiLineObj::
CQChartsXYBiLineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                    double x, double y1, double y2, const QModelIndex &ind,
                    const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ColorInd(), iv), plot_(plot),
 groupInd_(groupInd), x_(x), y1_(y1), y2_(y2)
{
  setModelInd(ind);
}

QString
CQChartsXYBiLineObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(modelInd());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3").arg(typeName()).arg(is_.i).arg(iv_.i);
}

QString
CQChartsXYBiLineObj::
calcTipId() const
{
  QString name  = plot()->valueName(-1, -1, modelInd().row());
  QString xstr  = plot()->xStr(x());
  QString y1str = plot()->yStr(y1());
  QString y2str = plot()->yStr(y2());

  CQChartsTableTip tableTip;

  if (name.length())
    tableTip.addTableRow("Name", name);

  QString xname;

  (void) plot()->xColumnName(xname, "X", /*tip*/true);

  tableTip.addTableRow(xname, xstr );
  tableTip.addTableRow("Y1" , y1str);
  tableTip.addTableRow("Y2" , y2str);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsXYBiLineObj::
isVisible() const
{
  if (! plot()->isLines() && ! plot()->isPoints())
    return false;

  return CQChartsPlotObj::isVisible();
}

bool
CQChartsXYBiLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  auto p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y1()));
  auto p2 = plot()->windowToPixel(CQChartsGeom::Point(x(), y2()));

  double sx, sy;

  plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(p1.x - sx, p1.y - sy, p2.x + sx, p2.y + sy);

  auto pp = plot()->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsXYBiLineObj::
getObjSelectIndices(Indices &inds) const
{
  if (! isVisible())
    return;

  addColumnSelectIndex(inds, CQChartsColumn(modelInd().column()));
}

//---

void
CQChartsXYBiLineObj::
draw(CQChartsPaintDevice *device)
{
  if (! isVisible())
    return;

  auto p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y1()));
  auto p2 = plot()->windowToPixel(CQChartsGeom::Point(x(), y2()));

  if (plot()->isLines())
    drawLines(device, p1, p2);

  if (plot()->isPoints())
    drawPoints(device, p1, p2);
}

void
CQChartsXYBiLineObj::
drawLines(CQChartsPaintDevice *device, const CQChartsGeom::Point &p1,
          const CQChartsGeom::Point &p2)
{
  // calc pen and brush
  CQChartsPenBrush penBrush;

  QColor lc = plot()->interpBivariateLinesColor(is_);

  plot()->setPenBrush(penBrush,
    CQChartsPenData  (true, lc, plot()->bivariateLinesAlpha(),
                      plot()->bivariateLinesWidth(), plot()->bivariateLinesDash()),
    CQChartsBrushData(false));

  plot()->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //--

  // draw line
  device->drawLine(device->pixelToWindow(p1), device->pixelToWindow(p2));
}

void
CQChartsXYBiLineObj::
drawPoints(CQChartsPaintDevice *device, const CQChartsGeom::Point &p1,
           const CQChartsGeom::Point &p2)
{
  // get symbol and size
  auto symbol = plot()->symbolType();

  double sx, sy;

  plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

  //---

  // calc pen and brush
  CQChartsPenBrush penBrush;

  plot_->setSymbolPenBrush(penBrush, is_);

  plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw symbols
  plot()->drawSymbol(device, p1, symbol, CMathUtil::avg(sx, sy), penBrush);
  plot()->drawSymbol(device, p2, symbol, CMathUtil::avg(sx, sy), penBrush);
}

//------

CQChartsXYImpulseLineObj::
CQChartsXYImpulseLineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                         double x, double y1, double y2, const QModelIndex &ind,
                         const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ColorInd(), iv), plot_(plot),
 groupInd_(groupInd), x_(x), y1_(y1), y2_(y2)
{
  setModelInd(ind);
}

QString
CQChartsXYImpulseLineObj::
calcId() const
{
  QModelIndex ind1 = plot()->unnormalizeIndex(modelInd());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3").arg(typeName()).arg(is_.i).arg(iv_.i);
}

QString
CQChartsXYImpulseLineObj::
calcTipId() const
{
  QString name  = plot()->valueName(is_.i, is_.n, modelInd().row());
  QString xstr  = plot()->xStr(x());
  QString y1str = plot()->yStr(y1());
  QString y2str = plot()->yStr(y2());

  CQChartsTableTip tableTip;

  if (name.length())
    tableTip.addTableRow("Name", name);

  QString xname;

  (void) plot()->xColumnName(xname, "X", /*tip*/true);

  tableTip.addTableRow(xname, xstr );
  tableTip.addTableRow("Y1" , y1str);
  tableTip.addTableRow("Y2" , y2str);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsXYImpulseLineObj::
isVisible() const
{
  return CQChartsPlotObj::isVisible();
}

bool
CQChartsXYImpulseLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  auto p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y1()));
  auto p2 = plot()->windowToPixel(CQChartsGeom::Point(x(), y2()));

  double b = 2;

  double lw = std::max(plot()->lengthPixelWidth(plot()->impulseLinesWidth()), 2*b);

  CQChartsGeom::BBox pbbox(p1.x - lw/2, p1.y - b, p2.x + lw/2, p2.y + b);

  auto pp = plot()->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsXYImpulseLineObj::
getObjSelectIndices(Indices &inds) const
{
  if (! isVisible())
    return;

  addColumnSelectIndex(inds, CQChartsColumn(modelInd().column()));
}

//---

void
CQChartsXYImpulseLineObj::
draw(CQChartsPaintDevice *device)
{
  if (! isVisible())
    return;

  //---

  ColorInd ic = (is_.n > 1 ? is_ : iv_);

  //---

  // calc pen and brush
  CQChartsPenBrush penBrush;

  QColor strokeColor = plot()->interpImpulseLinesColor(ic);

  double lw = plot()->lengthPixelWidth(plot()->impulseLinesWidth());

  if (lw <= 1) {
    plot()->setPenBrush(penBrush,
      CQChartsPenData  (true, strokeColor, plot()->impulseLinesAlpha(),
                        plot()->impulseLinesWidth(), plot()->impulseLinesDash()),
      CQChartsBrushData(false));
  }
  else {
    plot()->setPenBrush(penBrush,
      CQChartsPenData  (false),
      CQChartsBrushData(true, strokeColor, plot()->impulseLinesAlpha()));
  }

  plot()->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw impulse
  auto p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y1()));
  auto p2 = plot()->windowToPixel(CQChartsGeom::Point(x(), y2()));

  if (lw <= 1) {
    device->drawLine(device->pixelToWindow(p1), device->pixelToWindow(p2));
  }
  else {
    CQChartsGeom::BBox bbox(p1.x - lw/2.0, p1.y, p1.x + lw/2.0, p2.y);

    CQChartsDrawUtil::drawRoundedPolygon(device, device->pixelToWindow(bbox));
  }
}

//------

CQChartsXYPointObj::
CQChartsXYPointObj(const CQChartsXYPlot *plot, int groupInd,
                   const CQChartsGeom::BBox &rect, const CQChartsGeom::Point &pos,
                   const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ig, iv),
 plot_(plot), groupInd_(groupInd), pos_(pos)
{
}

CQChartsXYPointObj::
~CQChartsXYPointObj()
{
  delete edata_;
}

//---

void
CQChartsXYPointObj::
setSelected(bool b)
{
  CQChartsPlotObj::setSelected(b);

  if (plot()->isPointLineSelect() && lineObj())
    const_cast<CQChartsXYPolylineObj *>(lineObj())->setSelected(b);
}

//---

CQChartsSymbol
CQChartsXYPointObj::
symbolType() const
{
  CQChartsSymbol symbolType(CQChartsSymbol::Type::NONE);

  if (extraData())
    symbolType = extraData()->symbolType;

  if (! symbolType.isValid())
    symbolType = plot_->symbolType();

  return symbolType;
}

CQChartsLength
CQChartsXYPointObj::
symbolSize() const
{
  CQChartsLength symbolSize(CQChartsUnits::NONE, 0.0);

  if (extraData())
    symbolSize = extraData()->symbolSize;

  if (! symbolSize.isValid())
    symbolSize = plot()->symbolSize();

  return symbolSize;
}

CQChartsLength
CQChartsXYPointObj::
fontSize() const
{
  CQChartsLength fontSize(CQChartsUnits::NONE, 0.0);

  if (extraData())
    fontSize = extraData()->fontSize;

  if (! fontSize.isValid()) {
    double dataLabelFontSize = plot()->dataLabel()->textFont().pointSizeF();

    fontSize = CQChartsLength(dataLabelFontSize, CQChartsUnits::PIXEL);
  }

  return fontSize;
}

CQChartsColor
CQChartsXYPointObj::
color() const
{
  CQChartsColor color;

  if (extraData())
    color = extraData()->color;

  return color;
}

CQChartsImage
CQChartsXYPointObj::
image() const
{
  CQChartsImage image;

  if (extraData())
    image = extraData()->image;

  return image;
}

bool
CQChartsXYPointObj::
isVector() const
{
  return (extraData() ? bool(extraData()->vector) : false);
}

CQChartsGeom::Point
CQChartsXYPointObj::
vector() const
{
  return (extraData() ? *extraData()->vector : CQChartsGeom::Point());
}

//---

CQChartsXYPointObj::ExtraData *
CQChartsXYPointObj::
extraData()
{
  if (! edata_)
    edata_ = new ExtraData;

  return edata_;
}

const CQChartsXYPointObj::ExtraData *
CQChartsXYPointObj::
extraData() const
{
  if (! edata_)
    const_cast<CQChartsXYPointObj *>(this)->edata_ = new ExtraData;

  return edata_;
}

bool
CQChartsXYPointObj::
isVisible() const
{
  if (! plot()->isPoints())
    return false;

  return CQChartsPlotObj::isVisible();
}

//---

QString
CQChartsXYPointObj::
calcId() const
{
  QModelIndex ind1 = plot()->unnormalizeIndex(modelInd());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3:%4").arg(typeName()).arg(is_.i).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsXYPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  // add label (name column) as header
  if (labelObj_ && labelObj_->label().length())
    tableTip.addBoldLine(labelObj_->label());

  //---

  // add id column
  QModelIndex ind1 = plot()->unnormalizeIndex(modelInd());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    tableTip.addTableRow(plot()->idHeaderName(), idStr);

  //---

  // add group column
  if (ig_.n > 1) {
    QString groupName = plot()->groupIndName(ig_.i);

    tableTip.addTableRow("Group", groupName);
  }

  //---

  // add name column (TODO: needed or combine with header)
  if (! labelObj_ || ! labelObj_->label().length()) {
    QString name = plot()->valueName(is_.i, is_.n, modelInd().row());

    if (name.length())
      tableTip.addTableRow("Name", name);
  }

  //---

  // add x, y columns
  QString xstr = plot()->xStr(x());
  QString ystr = plot()->yStr(y());

  QString xname, yname;

  (void) plot()->xColumnName(xname, "X", /*tip*/true);
  (void) plot()->yColumnName(yname, "Y", /*tip*/true);

  tableTip.addTableRow(xname, xstr);
  tableTip.addTableRow(yname, ystr);

  //---

  auto addColumnRowValue = [&](const CQChartsColumn &column) {
    if (! column.isValid()) return;

    CQChartsModelIndex columnModelInd(modelInd().row(), column, modelInd().parent());

    bool ok;

    QString str = plot_->modelString(columnModelInd, ok);
    if (! ok) return;

    tableTip.addTableRow(plot_->columnHeaderName(column), str);
  };

  //---

  // add symbol type, symbol size and font size columns
  addColumnRowValue(plot_->symbolTypeColumn());
  addColumnRowValue(plot_->symbolSizeColumn());
  addColumnRowValue(plot_->fontSizeColumn  ());

  //---

  // add color column
  addColumnRowValue(plot_->colorColumn());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsXYPointObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  double sx, sy;

  plot()->pixelSymbolSize(this->symbolSize(), sx, sy);

  auto p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y()));

  CQChartsGeom::BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pp = plot()->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsXYPointObj::
getObjSelectIndices(Indices &inds) const
{
  if (! isVisible())
    return;

  addColumnSelectIndex(inds, plot()->xColumn());
  addColumnSelectIndex(inds, plot()->yColumns().getColumn(is_.i));

  addColumnSelectIndex(inds, plot_->symbolTypeColumn());
  addColumnSelectIndex(inds, plot_->symbolSizeColumn());
  addColumnSelectIndex(inds, plot_->fontSizeColumn  ());
  addColumnSelectIndex(inds, plot_->colorColumn     ());
}

//---

void
CQChartsXYPointObj::
draw(CQChartsPaintDevice *device)
{
  bool isVector = this->isVector();

  if (! isVisible() && ! isVector)
    return;

  //---

  ColorInd ic;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    // default for xy is set or group color (not value color !!)
    if      (is_.n > 1)
      ic = is_;
    else if (ig_.n > 1)
      ic = ig_;
  }
  else
    ic = calcColorInd();

  //---

  // calc pen and brush
  CQChartsPenBrush penBrush;

  plot()->setSymbolPenBrush(penBrush, ic);

  // override symbol fill color for custom color
  auto color = this->color();

  if (color.isValid()) {
    QColor c = plot()->interpColor(color, ic);

    c.setAlphaF(plot_->symbolFillAlpha().value());

    penBrush.brush.setColor(c);
  }

  plot()->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  if (isVisible()) {
    // override symbol type for custom symbol
    auto symbolType = this->symbolType();
    auto symbolSize = this->symbolSize();

    // draw symbol or image
    CQChartsImage image = this->image();

    if (! image.isValid()) {
      plot()->drawSymbol(device, pos_, symbolType, symbolSize, penBrush);
    }
    else {
      // get point
      auto ps = plot()->windowToPixel(pos_);

      double sx, sy;

      plot()->pixelSymbolSize(symbolSize, sx, sy);

      CQChartsGeom::BBox ibbox(ps.x - sx, ps.y - sy, ps.x + 2*sx, ps.y + 2*sy);

      device->drawImageInRect(plot()->pixelToWindow(ibbox), image);
    }
  }

  //---

  // draw optional vector
  // TODO: custom color for this sets what ?
  if (isVector) {
    auto p1 = pos_;
    auto p2 = p1 + this->vector();

    plot()->drawArrow(device, p1, p2);
  }
}

//------

CQChartsXYLabelObj::
CQChartsXYLabelObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                   double x, double y, const QString &label, const QModelIndex &ind,
                   const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ColorInd(), iv), plot_(plot),
 groupInd_(groupInd), pos_(x, y), label_(label)
{
  setModelInd(ind);
}

CQChartsXYLabelObj::
~CQChartsXYLabelObj()
{
}

QString
CQChartsXYLabelObj::
calcId() const
{
  QModelIndex ind1 = plot()->unnormalizeIndex(modelInd());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3").arg(typeName()).arg(is_.i).arg(iv_.i);
}

QString
CQChartsXYLabelObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow(plot_->columnHeaderName(plot_->labelColumn()), label_);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsXYLabelObj::
isVisible() const
{
  if (! plot()->dataLabel()->isVisible())
    return false;

  return CQChartsPlotObj::isVisible();
}

bool
CQChartsXYLabelObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  auto ppos = plot()->windowToPixel(CQChartsGeom::Point(pos_));

  // TODO: better text bounding box
  double sx = 16;
  double sy = 16;

  CQChartsGeom::BBox pbbox(ppos.x - sx, ppos.y - sy, ppos.x + sx, ppos.y + sy);

  auto pp = plot()->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsXYLabelObj::
getObjSelectIndices(Indices &inds) const
{
  if (! isVisible())
    return;

  addColumnSelectIndex(inds, plot()->xColumn());
  addColumnSelectIndex(inds, plot()->yColumns().getColumn(is_.i));
}

//---

void
CQChartsXYLabelObj::
draw(CQChartsPaintDevice *device)
{
  if (! isVisible())
    return;

  auto *dataLabel = plot_->dataLabel();

  //---

  // text font color
  CQChartsPenBrush penBrush;

  QColor tc = dataLabel->interpTextColor(ColorInd());

  plot()->setPenBrush(penBrush,
    CQChartsPenData(true, tc, dataLabel->textAlpha()), CQChartsBrushData(false));

  //---

  // get font size
  auto fontSize = pointObj()->fontSize();

  //---

  // set (temp) font
  auto font = dataLabel->textFont();

  if (fontSize.isValid()) {
    double fontPixelSize = plot_->lengthPixelHeight(fontSize);

    // scale to font size
    fontPixelSize = plot_->limitFontSize(fontPixelSize);

    auto font1 = font;

    font1.setPointSizeF(fontPixelSize);

    const_cast<CQChartsXYPlot *>(plot_)->setDataLabelFont(font1);
  }

  //---

  // draw text
  auto ps = plot()->windowToPixel(pos_);

  // TODO: better symbol bounding box
  double sx = 16.0;
  double sy = 16.0;

  CQChartsGeom::BBox ebbox(ps.x - sx, ps.y - sy, ps.x + sx, ps.y + sy);

  dataLabel->draw(device, ebbox, label_, dataLabel->position(), penBrush);

  //---

  // reset font
  if (fontSize.isValid()) {
    const_cast<CQChartsXYPlot *>(plot_)->setDataLabelFont(font);
  }

  // draw text
  plot()->view()->setPlotPainterFont(plot(), device, font);
}

//------

CQChartsXYPolylineObj::
CQChartsXYPolylineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                      const CQChartsGeom::Polygon &poly, const QString &name, const ColorInd &is,
                      const ColorInd &ig) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
 groupInd_(groupInd), poly_(poly), name_(name)
{
  setDetailHint(DetailHint::MAJOR);
}

CQChartsXYPolylineObj::
~CQChartsXYPolylineObj()
{
  delete smooth_;
  delete hull_;
}

QString
CQChartsXYPolylineObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig_.i).arg(is_.i);
}

QString
CQChartsXYPolylineObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name());

  //---

  //plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

#if 0
bool
CQChartsXYPolylineObj::
isVisible() const
{
  if (! plot()->isLines())
    return false;

  return CQChartsPlotObj::isVisible();
}
#endif

bool
CQChartsXYPolylineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! plot()->isLines())
    return false;

  if (! plot()->isLinesSelectable())
    return false;

  auto pp = plot()->windowToPixel(p);

  for (int i = 1; i < poly_.size(); ++i) {
    auto p1 = poly_.point(i - 1);
    auto p2 = poly_.point(i    );

    auto pl1 = plot()->windowToPixel(p1);
    auto pl2 = plot()->windowToPixel(p2);

    double d;

    if (CQChartsUtil::PointLineDistance(pp, pl1, pl2, &d) && d < 1)
      return true;
  }

  return false;
}

bool
CQChartsXYPolylineObj::
rectIntersect(const CQChartsGeom::BBox &r, bool inside) const
{
  return CQChartsPlotObj::rectIntersect(r, inside);
}

bool
CQChartsXYPolylineObj::
canSelect() const
{
  return plot()->isLinesSelectable();
}

bool
CQChartsXYPolylineObj::
interpY(double x, std::vector<double> &yvals) const
{
  if (! plot()->isLines())
    return false;

  if (plot()->isRoundedLines()) {
    initSmooth();

    double y = smooth_->interp(x);

    yvals.push_back(y);
  }
  else {
    for (int i = 1; i < poly_.size(); ++i) {
      double x1 = poly_.point(i - 1).x;
      double y1 = poly_.point(i - 1).y;
      double x2 = poly_.point(i    ).x;
      double y2 = poly_.point(i    ).y;

      if (x >= x1 && x <= x2) {
        double y = (y2 - y1)*(x - x1)/(x2 - x1) + y1;

        yvals.push_back(y);
      }
    }
  }

  return ! yvals.empty();
}

bool
CQChartsXYPolylineObj::
isOutlier(double y) const
{
  const_cast<CQChartsXYPolylineObj *>(this)->initStats();

  return statData_.isOutlier(y);
}

void
CQChartsXYPolylineObj::
getObjSelectIndices(Indices &) const
{
  if (! plot()->isLines())
    return;

  // all objects part of line (don't support select)
}

void
CQChartsXYPolylineObj::
initSmooth() const
{
  // init smooth if needed
  if (! smooth_) {
    auto *th = const_cast<CQChartsXYPolylineObj *>(this);

    th->smooth_ = new CQChartsSmooth(poly_, /*sorted*/false);
  }
}

void
CQChartsXYPolylineObj::
resetBestFit()
{
  bestFit_.resetFitted();
}

void
CQChartsXYPolylineObj::
initBestFit()
{
  if (! bestFit_.isFitted()) {
    if (! plot()->isBestFitOutliers()) {
      initStats();

      //---

      CQChartsGeom::Polygon poly;

      for (const auto &p : poly_.qpoly()) {
        if (! statData_.isOutlier(p.y()))
          poly.addPoint(p);
      }

      //---

      bestFit_.calc(poly, plot()->bestFitOrder());
    }
    else
      bestFit_.calc(poly_, plot()->bestFitOrder());
  }
}

void
CQChartsXYPolylineObj::
initStats()
{
  if (! statData_.set) {
    std::vector<double> y;

    for (int i = 0; i < poly_.size(); ++i)
      y.push_back(poly_.point(i).y);

    std::sort(y.begin(), y.end());

    statData_.calcStatValues(y);
  }
}

//---

void
CQChartsXYPolylineObj::
draw(CQChartsPaintDevice *device)
{
  if (! plot()->isLines() && ! plot()->isBestFit() && ! plot()->isStatsLines())
    return;

  //---

  if (plot()->isHull()) {
    if (! hull_)
      hull_ = new CQChartsGrahamHull;

    //---

    hull_->clear();

    for (int i = 0; i < poly_.size(); ++i) {
      if (plot()->isInterrupt())
        return;

      hull_->addPoint(poly_.point(i));
    }

    //---

    // set pen/brush
    ColorInd ic = (ig_.n > 1 ? ig_ : is_);

    CQChartsPenBrush penBrush;

    plot()->setPenBrush(penBrush, plot()->hullPenData(ic), plot_->hullBrushData(ic));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    hull_->draw(plot(), device);
  }

  //---

  // draw lines
  if (plot()->isLines()) {
    // calc pen and brush
    CQChartsPenBrush penBrush;

    bool updateState = device->isInteractive();

    calcPenBrush(penBrush, updateState);

    //---

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    if (plot()->isRoundedLines()) {
      initSmooth();

      // draw path
      QPainterPath path = smooth_->createPath(/*closed*/false);

      device->setColorNames();

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      device->drawPath(path);

      device->resetColorNames();
    }
    else {
      // draw path
      device->setColorNames();

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      int np = poly_.size();

      for (int i = 1; i < np; ++i)
        device->drawLine(poly_.point(i - 1), poly_.point(i));

      device->resetColorNames();
    }
  }

  //---

  if (plot()->isBestFit()) {
    const_cast<CQChartsXYPolylineObj *>(this)->initBestFit();

    //---

    ColorInd ic = (ig_.n > 1 ? ig_ : is_);

    plot()->drawBestFit(device, bestFit_, ic);
  }

  //---

  if (plot()->isStatsLines()) {
    const_cast<CQChartsXYPolylineObj *>(this)->initStats();

    //---

    // calc pen and brush
    ColorInd ic = (ig_.n > 1 ? ig_ : is_);

    CQChartsPenBrush penBrush;

    QColor c = plot()->interpStatsLinesColor(ic);

    plot()->setPenBrush(penBrush,
      CQChartsPenData  (true, c, plot()->statsLinesAlpha(),
                        plot()->statsLinesWidth(), plot()->statsLinesDash()),
      CQChartsBrushData(false));

    plot()->updateObjPenBrushState(this, ic, penBrush, CQChartsPlot::DrawType::LINE);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    const auto &dataRange = plot()->dataRange();

    auto drawStatLine = [&](double y) {
      CQChartsGeom::Point p1(dataRange.xmin(), y);
      CQChartsGeom::Point p2(dataRange.xmax(), y);

      device->drawLine(p1, p2);
    };

    drawStatLine(statData_.loutlier   );
    drawStatLine(statData_.lowerMedian);
    drawStatLine(statData_.median     );
    drawStatLine(statData_.upperMedian);
    drawStatLine(statData_.uoutlier   );
  }
}

void
CQChartsXYPolylineObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  ColorInd ic = (ig_.n > 1 ? ig_ : is_);

  QColor c = plot()->interpLinesColor(ic);

  plot()->setPenBrush(penBrush,
    CQChartsPenData  (true, c, plot()->linesAlpha(), plot()->linesWidth(), plot()->linesDash()),
    CQChartsBrushData(false));

  if (updateState)
    plot()->updateObjPenBrushState(this, ic, penBrush, CQChartsPlot::DrawType::LINE);
}

void
CQChartsXYPolylineObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

//------

CQChartsXYPolygonObj::
CQChartsXYPolygonObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                     const CQChartsGeom::Polygon &poly, const QString &name, const ColorInd &is,
                     const ColorInd &ig, bool under) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
 groupInd_(groupInd), poly_(poly), name_(name), under_(under)
{
  setDetailHint(DetailHint::MAJOR);
}

CQChartsXYPolygonObj::
~CQChartsXYPolygonObj()
{
  delete smooth_;
}

QString
CQChartsXYPolygonObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig_.i).arg(is_.i);
}

QString
CQChartsXYPolygonObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name());
  tableTip.addTableRow("Area", poly_.area());

  //---

  //plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

bool
CQChartsXYPolygonObj::
isVisible() const
{
  if (! plot()->isFillUnderFilled())
    return false;

  return CQChartsPlotObj::isVisible();
}

bool
CQChartsXYPolygonObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  if (! plot()->isFillUnderSelectable())
    return false;

  return poly_.containsPoint(p, Qt::OddEvenFill);
}

bool
CQChartsXYPolygonObj::
rectIntersect(const CQChartsGeom::BBox &r, bool inside) const
{
  return CQChartsPlotObj::rectIntersect(r, inside);
}

bool
CQChartsXYPolygonObj::
canSelect() const
{
  return plot()->isFillUnderSelectable();
}

void
CQChartsXYPolygonObj::
getObjSelectIndices(Indices &) const
{
  if (! isVisible())
    return;

  // all objects part of polygon (don't support select)
}

void
CQChartsXYPolygonObj::
initSmooth() const
{
  // init smooth if needed
  // (not first point and last point are the extra points to make the square protrusion
  if (! smooth_) {
    auto *th = const_cast<CQChartsXYPolygonObj *>(this);

    th->smooth_ = new CQChartsSmooth(poly_, /*sorted*/false);

    th->smooth_->setUnder(under_);
  }
}

//---

void
CQChartsXYPolygonObj::
draw(CQChartsPaintDevice *device)
{
  if (! isVisible())
    return;

  //---

  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // create polygon path (rounded or normal)
  if (plot()->isRoundedLines()) {
    initSmooth();

    // draw polygon
    QPainterPath path = smooth_->createPath(/*closed*/true);

    device->setColorNames();

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->drawPath(path);

    device->resetColorNames();
  }
  else {
    // draw polygon
    device->setColorNames();

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->drawPolygon(poly_);

    device->resetColorNames();
  }
}

void
CQChartsXYPolygonObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // calc pen and brush
  ColorInd ic = (ig_.n > 1 ? ig_ : is_);

  QColor fillColor = plot()->interpFillUnderFillColor(ic);

  plot()->setPenBrush(penBrush,
    CQChartsPenData  (false),
    CQChartsBrushData(true, fillColor, plot()->fillUnderFillAlpha(),
                      plot()->fillUnderFillPattern()));

  if (updateState)
    plot()->updateObjPenBrushState(this, penBrush);
}

void
CQChartsXYPolygonObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

//------

CQChartsXYKeyColor::
CQChartsXYKeyColor(CQChartsXYPlot *plot, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyColorBox(plot, is, ig, ColorInd()), plot_(plot)
{
  setClickable(true);
}

void
CQChartsXYKeyColor::
doSelect(CQChartsSelMod selMod)
{
  auto *obj = plotObj();
  if (! obj) return;

  if      (selMod == CQChartsSelMod::REPLACE) {
    for (int ig = 0; ig < ig_.n; ++ig) {
      auto *obj1 = plot()->getGroupObj(ig);

      if (obj1)
        obj1->setSelected(obj1 == obj);
    }
  }
  else if (selMod == CQChartsSelMod::ADD)
    obj->setSelected(true);
  else if (selMod == CQChartsSelMod::REMOVE)
    obj->setSelected(false);
  else if (selMod == CQChartsSelMod::TOGGLE)
    obj->setSelected(! obj->isSelected());

  plot()->invalidateOverlay();

  key_->redraw(/*wait*/ true);
}

QBrush
CQChartsXYKeyColor::
fillBrush() const
{
  QBrush brush;

  QColor              c;
  CQChartsAlpha       alpha;
  CQChartsFillPattern pattern = CQChartsFillPattern::Type::SOLID;

  if      (plot()->isBivariateLines()) {
    c = plot()->interpBivariateLinesColor(is_);

    alpha = plot()->bivariateLinesAlpha();
  }
  else if (plot()->isOverlay()) {
    if (plot()->prevPlot() || plot()->nextPlot()) {
      c = plot()->interpLinesColor(is_);

      alpha = plot()->linesAlpha();
    }
    else
      c = CQChartsKeyColorBox::fillBrush().color();
  }
  else
    c = CQChartsKeyColorBox::fillBrush().color();

  if (plot()->isSetHidden(is_.i))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  plot()->setBrush(brush, true, c, alpha, pattern);

  return brush;
}

CQChartsPlotObj *
CQChartsXYKeyColor::
plotObj() const
{
  if (ig_.n <= 1)
    return nullptr;

  auto *obj = plot()->getGroupObj(ig_.i);

  return obj;
}

//------

#if 0
CQChartsXYKeyLine::
CQChartsXYKeyLine(CQChartsXYPlot *plot, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyItem(plot->key(), is.n > 1 ? is : ig), plot_(plot), is_(is), ig_(ig)
{
  setClickable(true);
}

CQChartsGeom::Size
CQChartsXYKeyLine::
size() const
{
  auto *keyPlot = qobject_cast<CQChartsXYPlot *>(key_->plot());

  if (! keyPlot)
    keyPlot = plot();

  QFont font = plot()->view()->plotFont(plot(), key_->textFont());

  QFontMetricsF fm(font);

  double w = fm.width("-X-");
  double h = fm.height();

  double ww = keyPlot->pixelToWindowWidth (w + 8);
  double wh = keyPlot->pixelToWindowHeight(h + 2);

  return CQChartsGeom::Size(ww, wh);
}

void
CQChartsXYKeyLine::
doSelect(CQChartsSelMod selMod)
{
  auto *obj = plotObj();
  if (! obj) return;

  if      (selMod == CQChartsSelMod::REPLACE) {
    for (int ig = 0; ig < ig_.n; ++ig) {
      auto *obj1 = plot()->getGroupObj(ig);

      if (obj1)
        obj1->setSelected(obj1 == obj);
    }
  }
  else if (selMod == CQChartsSelMod::ADD)
    obj->setSelected(true);
  else if (selMod == CQChartsSelMod::REMOVE)
    obj->setSelected(false);
  else if (selMod == CQChartsSelMod::TOGGLE)
    obj->setSelected(! obj->isSelected());

  plot()->invalidateOverlay();

  key_->redraw(/*wait*/ true);
}

//---

void
CQChartsXYKeyLine::
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const
{
  device->save();

  auto *keyPlot = qobject_cast<CQChartsPlot *>(key_->plot());

  //auto *xyKeyPlot = qobject_cast<CQChartsXYPlot *>(keyPlot);
  //if (! xyKeyPlot) xyKeyPlot = plot();

  auto prect = keyPlot->windowToPixel(rect);

  bool swapped;
  auto pbbox1 = prect.adjusted(2, 2, -2, -2, swapped);
  if (swapped) rerurn;

  device->setClipRect(pbbox1, Qt::IntersectClip);

  QColor        hideBg;
  CQChartsAlpha hideAlpha;

  if (plot()->isSetHidden(ic_.i)) {
    hideBg    = key_->interpBgColor();
    hideAlpha = key_->hiddenAlpha();
  }

  if (plot()->isFillUnderFilled()) {
    QBrush fillBrush;

    QColor fillColor = plot()->interpFillUnderFillColor(ic_);

    plot()->setBrush(fillBrush, true, fillColor, plot()->fillUnderFillAlpha(),
                     plot()->fillUnderFillPattern());

    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y1 = prect.getYMin() + 4;
    double y2 = prect.getYMax() - 4;

    if (isInside())
      fillBrush.setColor(plot()->insideColor(fillBrush.color()));

    CQChartsGeom::BBox pbbox1(x1, y1, x2, y2);

    device->fillRect(pbbox1, fillBrush);
  }

  if (plot()->isLines() || plot()->isBestFit() || plot()->isImpulseLines()) {
    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y  = prect.getYMid();

    CQChartsPenBrush linePenBrush;

    if      (plot()->isLines()) {
      QColor lineColor = plot()->interpLinesColor(ic_);

      if (plot()->isSetHidden(ic_.i))
        lineColor = CQChartsUtil::blendColors(lineColor, hideBg, hideAlpha);

      plot()->setPen(linePenBrush.pen, true, lineColor, plot()->linesAlpha(),
                     plot()->linesWidth(), plot()->linesDash());
    }
    else if (plot()->isBestFit()) {
      QColor fitColor = plot()->interpBestFitStrokeColor(ic_);

      if (plot()->isSetHidden(ic_.i))
        fitColor = CQChartsUtil::blendColors(fitColor, hideBg, hideAlpha);

      plot()->setPen(linePenBrush.pen, true, fitColor, plot()->bestFitStrokeAlpha(),
                     plot()->bestFitStrokeWidth(), plot()->bestFitStrokeDash());
    }
    else {
      QColor impulseColor = plot()->interpImpulseLinesColor(ic_);

      if (plot()->isSetHidden(ic_.i))
        impulseColor = CQChartsUtil::blendColors(impulseColor, hideBg, hideAlpha);

      plot()->setPen(linePenBrush.pen, true, impulseColor, plot()->impulseLinesAlpha(),
                     plot()->impulseLinesWidth(), plot()->impulseLinesDash());
    }

    linePenBrush.brush = QBrush(Qt::NoBrush);

    auto *obj = plotObj();

    if (obj)
      plot()->updateObjPenBrushState(obj, ig_, linePenBrush, CQChartsPlot::DrawType::LINE);

    if (isInside())
      linePenBrush.pen = plot()->insideColor(linePenBrush.pen.color());

    device->setPen(linePenBrush.pen);

    device->drawLine(device->pixelToWindow(CQChartsGeom::Point(x1, y)),
                     device->pixelToWindow(CQChartsGeom::Point(x2, y)));
  }

  if (plot()->isPoints()) {
    double dx = keyPlot->pixelToWindowWidth(4);

    double x1 = rect.getXMin() + dx;
    double x2 = rect.getXMax() - dx;
    double y  = rect.getYMid();

    auto p1 = keyPlot->windowToPixel(CQChartsGeom::Point(x1, y));
    auto p2 = keyPlot->windowToPixel(CQChartsGeom::Point(x2, y));

    //---

    QColor pointStrokeColor = plot()->interpSymbolStrokeColor(ic_);
    QColor pointFillColor   = plot()->interpSymbolFillColor  (ic_);

    if (plot()->isSetHidden(ic_.i)) {
      pointStrokeColor = CQChartsUtil::blendColors(pointStrokeColor, hideBg, hideAlpha);
      pointFillColor   = CQChartsUtil::blendColors(pointFillColor  , hideBg, hideAlpha);
    }

    //---

    CQChartsPenBrush penBrush;

    plot()->setPenBrush(penBrush,
      CQChartsPenData  (plot()->isSymbolStroked(), pointStrokeColor, plot()->symbolStrokeAlpha(),
                        plot()->symbolStrokeWidth(), plot()->symbolStrokeDash()),
      CQChartsBrushData(plot()->isSymbolFilled(), pointFillColor, plot()->symbolFillAlpha(),
                        plot()->symbolFillPattern()));

    auto *obj = plotObj();

    if (obj)
      plot()->updateObjPenBrushState(obj, ig_, penBrush, CQChartsPlot::DrawType::SYMBOL);

    //---

    auto symbolType = plot()->symbolType();
    auto symbolSize = plot()->symbolSize();

    CQChartsGeom::Point ps(CMathUtil::avg(p1.x, p2.x), CMathUtil::avg(p1.y, p2.y));

    plot()->drawSymbol(device, device->pixelToWindow(ps), symbolType, symbolSize, penBrush);
  }

  device->restore();
}

CQChartsPlotObj *
CQChartsXYKeyLine::
plotObj() const
{
  if (ig_.n <= 1)
    return nullptr;

  auto *obj = plot()->getGroupObj(ig_.i);

  return obj;
}
#endif

//------

CQChartsXYKeyText::
CQChartsXYKeyText(CQChartsXYPlot *plot, const QString &text,
                  const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyText(plot, text, is.n > 1 ? is : ig), plot_(plot)
{
}

QColor
CQChartsXYKeyText::
interpTextColor(const ColorInd &ind) const
{
  QColor c = CQChartsKeyText::interpTextColor(ind);

  if (plot()->isSetHidden(ic_.i))
    c = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), key_->hiddenAlpha());

  return c;
}
