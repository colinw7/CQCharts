#include <CQChartsXYPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsArrow.h>
#include <CQChartsSmooth.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>

#include <CQUtil.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QPainter>
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
    setRequired().setNumeric().setTip("Y Value Columns");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setTip("Optional Name Column");
  addColumnParameter("size", "Size", "sizeColumn").
    setNumeric().setTip("Symbol Size Column");

  //--

  // bool parameters
  addBoolParameter("lines"     , "Lines"     , "lines", true    ).setTip("Draw Lines");
  addBoolParameter("points"    , "Points"    , "points"         ).setTip("Draw Points");
  addBoolParameter("bivariate" , "Bivariate" , "bivariateLines" ).setTip("Draw Bivariate Lines");
  addBoolParameter("stacked"   , "Stacked"   , "stacked"        ).setTip("Stack Points");
  addBoolParameter("cumulative", "Cumulative", "cumulative"     ).setTip("Cumulate Values");
  addBoolParameter("fillUnder" , "Fill Under", "fillUnderFilled").setTip("Fill Under Curve");
  addBoolParameter("impulse"   , "Impulse"   , "impulseLines"   ).setTip("Draw Point Impulse");

  endParameterGroup();

  //---

  // custom columns/map
  startParameterGroup("Points");

  addColumnParameter("pointLabel" , "Point Label" , "pointLabelColumn" ).setString();
  addColumnParameter("pointColor" , "Point Color" , "pointColorColumn" ).setColor ();
  addColumnParameter("pointSymbol", "Point Symbol", "pointSymbolColumn").setString();

  endParameterGroup();

  //---

  // vector columns
  startParameterGroup("Vectors");

  addColumnParameter("vectorX", "Vector X", "vectorXColumn").setNumeric();
  addColumnParameter("vectorY", "Vector Y", "vectorYColumn").setNumeric();

  addBoolParameter("vectors", "Vectors", "vectors");

  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsXYPlotType::
description() const
{
  auto B = [](const QString &str) { return CQChartsHtml::Str::bold(str); };

  return CQChartsHtml().
   h2("XY Plot Type</h2>").
    h3("Summary").
     p("Draws points at x and y coordinate pairs and connects them with a continuous line.").
     p("The x coordinates should be monotonic.").
    h3("Columns").
     p("The x and y values come from the values in the " + B("X") + " and " + B("Y") + " columns. "
       "Multiple " + B("Y") + " columns can be specified to create a stack of lines.").
     p("An optional " + B("Name") + " column can be specified to supply a name for the "
       "coordinate.").
     p("An optional " + B("Size") + " column can be specified to supply the size of the symbol "
       "drawn at the point.").
     p("An optional " + B("PointLabel") + " column can be specified to add a label next to a "
       "point.").
     p("An optional " + B("PointColor") + " column can be specified to specify the color of "
       "the point symbol.").
     p("An optional " + B("PointSymbol") + " column can be specified to specify the symbol of "
       "the point.").
     p("Optional " + B("VectorX") + " and " + B("VectorY") + " columns can be specified to draw a "
       "vector at the point.").
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
    h3("Limitations").
     p("None");
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
 CQChartsGroupPlot(view, view->charts()->plotType("xy"), model),
 CQChartsObjLineData         <CQChartsXYPlot>(this),
 CQChartsObjPointData        <CQChartsXYPlot>(this),
 CQChartsObjBestFitShapeData <CQChartsXYPlot>(this),
 CQChartsObjStatsLineData    <CQChartsXYPlot>(this),
 CQChartsObjImpulseLineData  <CQChartsXYPlot>(this),
 CQChartsObjBivariateLineData<CQChartsXYPlot>(this),
 CQChartsObjFillUnderFillData<CQChartsXYPlot>(this),
 CQChartsObjDataLabelTextData<CQChartsXYPlot>(this)
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

  setBestFit(false);
  setBestFitStrokeDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));
  setBestFitFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setBestFitFillAlpha(0.5);

  setStatsLines(false);
  setStatsLinesDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  //---

  // arrow object
  arrowObj_ = new CQChartsArrow(this);

  arrowObj_->setVisible(false);

  connect(arrowObj_, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

  //---

  setFillUnderFilled   (false);
  setFillUnderFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setFillUnderFillAlpha(0.5);

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
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setSizeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(sizeColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setPointLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(pointLabelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setPointColorColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(pointColorColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setPointSymbolColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(pointSymbolColumn_, c, [&]() { updateRangeAndObjs(); } );
}

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
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.visible, b, [&]() {
    bestFitData_.visible = b; resetBestFit(); drawObjs();
  } );
}

void
CQChartsXYPlot::
setBestFitOutliers(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.includeOutliers, b, [&]() {
    bestFitData_.includeOutliers = b; resetBestFit(); drawObjs();
  } );
}

void
CQChartsXYPlot::
setBestFitDeviation(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.showDeviation, b, [&]() {
    bestFitData_.showDeviation = b; resetBestFit(); drawObjs();
  } );
}

void
CQChartsXYPlot::
setBestFitOrder(int order)
{
  CQChartsUtil::testAndSet(bestFitData_.order, order, [&]() {
    bestFitData_.order = order; resetBestFit(); drawObjs();
  } );
}

void
CQChartsXYPlot::
resetBestFit()
{
  for (const auto &plotObj : plotObjs_) {
    CQChartsXYPolylineObj *polyObj = dynamic_cast<CQChartsXYPolylineObj *>(plotObj);

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
    disconnect(arrowObj_, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

    arrowObj_->setVisible(b);

    connect(arrowObj_, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

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

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    CQPropertyViewItem *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  auto addArrowProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    return &(this->addProperty(path, arrowObj_, name, alias)->setDesc(desc));
  };

  auto addArrowStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                               const QString &desc) {
    CQPropertyViewItem *item = addArrowProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "xColumn"           , "x"          , "X column");
  addProp("columns", "yColumns"          , "y"          , "Y columns");
  addProp("columns", "nameColumn"        , "name"       , "Name column");
  addProp("columns", "sizeColumn"        , "size"       , "Size column");
  addProp("columns", "pointLabelColumn"  , "pointLabel" , "Point label column");
  addProp("columns", "pointColorColumn"  , "pointColor" , "Point color column");
  addProp("columns", "pointSymbolColumn" , "pointSymbol", "Point symbol column");
  addProp("columns", "vectorXColumn"     , "vectorX"    , "Vector x column");
  addProp("columns", "vectorYColumn"     , "vectorY"    , "Vector y column");

  // bivariate
  addProp("bivariate", "bivariateLines", "visible", "Bivariate lines visible");

  addLineProperties("bivariate/stroke", "bivariateLines", "Bivariate");

  // stacked
  addProp("stacked", "stacked", "enabled", "Stack y values");

  // cumulative
  addProp("cumulative", "cumulative", "enabled", "Cumulative values");

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

  addStyleProp("lines", "roundedLines", "rounded", "Smooth lines");

  addLineProperties("lines/stroke", "lines", "Lines");

  // best fit line and deviation fill
  addProp("bestFit", "bestFit"         , "visible"  , "Show best fit");
  addProp("bestFit", "bestFitOutliers" , "outliers" , "Best fit include outliers");
  addProp("bestFit", "bestFitOrder"    , "order"    , "Best fit curve order");
  addProp("bestFit", "bestFitDeviation", "deviation", "Best fit standard deviation");

  addFillProperties("bestFit/fill"  , "bestFitFill"  , "Best fit");
  addLineProperties("bestFit/stroke", "bestFitStroke", "Best fit");

  // stats
  addProp("statsData", "statsLines", "visible", "Statistic lines visible");

  addLineProperties("statsData/stroke", "statsLines", "Statistic lines");

  // fill under
  addProp("fillUnder", "fillUnderFilled"    , "visible"   , "Fill under polygon visible");
  addProp("fillUnder", "fillUnderSelectable", "selectable", "Fill under polygon selectable");
  addProp("fillUnder", "fillUnderPos"       , "position"  , "Fill under base position");
  addProp("fillUnder", "fillUnderSide"      , "side"      , "Fill under line side");

  addFillProperties("fillUnder/fill", "fillUnderFill", "Fill under");

  // impulse
  addProp("impulse", "impulseLines", "visible", "Impulse lines visible");

  addLineProperties("impulse/stroke", "impulseLines", "Impulse lines");

  // vectors
  addProp("vectors", "vectors", "visible", "Vectors at points visible");

  addArrowStyleProp("vectors", "length"   , "length"   , "Vector arrow length");
  addArrowStyleProp("vectors", "angle"    , "angle"    , "Vector arrow angle");
  addArrowStyleProp("vectors", "backAngle", "backAngle", "Vector arrow back angle");

  addArrowProp("vectors", "fhead", "fhead", "Show vector arrow front head");
  addArrowProp("vectors", "thead", "thead", "Show vector arrow tail head");

  addArrowStyleProp("vectors", "filled"   , "filled"   , "Vector arrow is filled");
  addArrowStyleProp("vectors", "lineEnds" , "lineEnds" , "Draw lines for vector end arrows");
  addArrowStyleProp("vectors", "lineWidth", "lineWidth", "Vector arrow connecting line width");

  addArrowStyleProp("vectors/fill", "filled"   , "visible", "Vector fill visible");
  addArrowStyleProp("vectors/fill", "fillColor", "color"  , "Vector fill color");
  addArrowStyleProp("vectors/fill", "fillAlpha", "alpha"  , "Vector fill alpha");

  addArrowStyleProp("vectors/stroke", "stroked"    , "visible", "Vector stroke visible");
  addArrowStyleProp("vectors/stroke", "strokeColor", "color"  , "Vector stroke color");
  addArrowStyleProp("vectors/stroke", "strokeAlpha", "alpha"  , "Vector stroke alpha");
  addArrowStyleProp("vectors/stroke", "strokeWidth", "width"  , "Vector stroke width");

  // data label
  addProp("labels/text", "dataLabelTextVisible", "visible", "Data labels visible");

  addStyleProp("labels/text", "dataLabelTextAngle", "angle", "Data labels text angle");

  addAllTextProperties("labels/text", "dataLabelText", "Data labels");

  CQChartsGroupPlot::addProperties();
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
      CQChartsPlot *plot1 = prevPlot();
      CQChartsPlot *plot2 = nextPlot();

      while (plot1) { ++n; plot1 = plot1->prevPlot(); }
      while (plot2) { ++n; plot2 = plot2->nextPlot(); }
    }

    //---

    CQChartsPlot *plot1 = prevPlot();

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

  //---

  initGroupData(CQChartsColumns(), CQChartsColumn());

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsXYPlot *plot) :
     plot_(plot) {
      int ns = plot_->yColumns().count();

      sum_.resize(ns);
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

      if (! plot_->rowData(data, x, y, rowInd, /*skipBad*/true))
        return State::SKIP;

      int ny = y.size();

      //---

      if     (plot_->isStacked()) {
        // TODO: support stacked and cmulative
        double sum1 = 0.0;

        for (int i = 0; i < ny; ++i)
          sum1 += y[i];

        range_.updateRange(x, 0.0);
        range_.updateRange(x, sum1);
      }
      else if (plot_->isCumulative()) {
        for (int i = 0; i < ny; ++i) {
          double y1 = y[i] + lastSum_[i];

          sum_[i] += y[i];

          range_.updateRange(x, y1);
        }
      }
      else {
        for (int i = 0; i < ny; ++i)
          range_.updateRange(x, y[i]);
      }

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    using Reals = std::vector<double>;

    const CQChartsXYPlot* plot_ { nullptr };
    CQChartsGeom::Range   range_;
    Reals                 sum_;
    Reals                 lastSum_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  CQChartsGeom::Range dataRange = visitor.range();

  if (isInterrupt())
    return dataRange;

  //---

  //dataRange = adjustDataRange(dataRange);

  //---

  // ensure range non-zero
  dataRange.makeNonZero();

  //---

  const_cast<CQChartsXYPlot *>(this)->initAxes();

  //---

  return dataRange;
}

void
CQChartsXYPlot::
initAxes()
{
  setXValueColumn(xColumn());
  setYValueColumn(yColumns().column());

  //---

  // set x axis name
  if (xAxis()) {
    if (isOverlay()) {
      if (isFirstPlot() || isX1X2())
        xAxis()->setColumn(xColumn());
    }
    else {
      xAxis()->setColumn(xColumn());
    }

    QString xname = xAxisName();

    if (isOverlay()) {
      if (isFirstPlot() || isX1X2())
        xAxis()->setLabel(xname);
    }
    else {
      xAxis()->setLabel(xname);
    }

    ColumnType xColumnType = columnValueType(xColumn());

    if (xColumnType == CQBaseModelType::TIME)
      xAxis()->setValueType(CQChartsAxisValueType::Type::DATE, /*notify*/false);
  }

  // set y axis name(s)
  if (yAxis()) {
    QString yname = yAxisName();

    int ns = yColumns().count();

    if      (isBivariateLines() && ns > 1) {
      if (isOverlay()) {
        if      (isY1Y2()) {
          yAxis()->setLabel(yname);
        }
        else if (isFirstPlot()) {
          yAxis()->setLabel(yname);

          Plots plots;

          overlayPlots(plots);

          for (auto &plot : plots) {
            if (plot == this)
              continue;

            QString yname = yAxisName();

            plot->yAxis()->setLabel(firstPlot()->yAxis()->label() + ", " + yname);
          }
        }
        else
          yAxis()->setLabel("");
      }
      else {
        yAxis()->setLabel(yname);
      }
    }
    else if (isStacked()) {
    }
    else {
      CQChartsColumn yColumn = yColumns().getColumn(0);

      if (isOverlay()) {
        if (isFirstPlot() || isY1Y2())
          yAxis()->setColumn(yColumn);
      }
      else
        yAxis()->setColumn(yColumn);

      if (isOverlay()) {
        if      (isY1Y2()) {
          yAxis()->setLabel(yname);
        }
        else if (isFirstPlot()) {
          yAxis()->setLabel(yname);

          Plots plots;

          overlayPlots(plots);

          for (auto &plot : plots) {
            if (plot == this)
              continue;

            QString yname = yAxisName();

            plot->yAxis()->setLabel(firstPlot()->yAxis()->label() + ", " + yname);
          }
        }
        else
          yAxis()->setLabel("");
      }
      else {
        yAxis()->setLabel(yname);
      }
    }
  }
}

QString
CQChartsXYPlot::
xAxisName(const QString &def) const
{
  bool ok;

  QString name = modelHeaderString(xColumn(), ok);

  if (! ok)
    name = def;

  return name;
}

QString
CQChartsXYPlot::
yAxisName(const QString &def) const
{
  QString name;
  bool    ok = true;

  int ns = yColumns().count();

  if      (isBivariateLines() && ns > 1) {
    name = titleStr();

    if (! name.length()) {
      CQChartsColumn yColumn1 = yColumns().getColumn(0);

      CQChartsColumn yColumn2;

      if (ns > 1)
        yColumn2 = yColumns().getColumn(1);

      bool ok1, ok2;

      QString yname1 = modelHeaderString(yColumn1, ok1);
      QString yname2 = modelHeaderString(yColumn2, ok2);

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    ok = name.length();
  }
  else if (isStacked()) {
    ok = false;
  }
  else {
    for (int j = 0; j < ns; ++j) {
      bool ok;

      CQChartsColumn yColumn = yColumns().getColumn(j);

      QString name1 = modelHeaderString(yColumn, ok);

      if (name.length())
        name += ", ";

      name += name1;
    }

    ok = name.length();
  }

  if (! ok)
    name = def;

  return name;
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

  CQChartsXYPlot *th = const_cast<CQChartsXYPlot *>(this);

  NoUpdate noUpdate(th);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();
  if (! dataRange.isSet()) return false;

  // TODO: use actual symbol size
  th->symbolWidth_  = (dataRange.xmax() - dataRange.xmin())/100.0;
  th->symbolHeight_ = (dataRange.ymax() - dataRange.ymin())/100.0;

  //---

  if (pointColorColumn().isValid())
    th->pointColorColumnType_ = columnValueType(pointColorColumn());

  //---

  GroupSetIndPoly groupSetIndPoly;

  createGroupSetIndPoly(groupSetIndPoly);

  (void) createGroupSetObjs(groupSetIndPoly, objs);

  //---

  return true;
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
      ns_ = plot_->yColumns().count();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex ind(data.row, plot_->xColumn(), data.parent);

      // get group
      int groupInd = plot_->rowGroupInd(ind);

      SetIndPoly &setPoly = groupSetPoly_[groupInd];

      if (setPoly.empty())
        setPoly.resize(ns_);

      //---

      double x; std::vector<double> y; QModelIndex rowInd;

      (void) plot_->rowData(data, x, y, rowInd, /*skipBad*/false);
      assert(int(y.size()) == ns_);

      for (int i = 0; i < ns_; ++i) {
        setPoly[i].inds.push_back(rowInd);

        setPoly[i].poly << QPointF(x, y[i]);
      }

      return State::OK;
    }

    // stack lines
    void stack() {
      for (auto &p : groupSetPoly_) {
        SetIndPoly &setPoly = p.second;

        for (int i = 1; i < ns_; ++i) {
          QPolygonF &poly1 = setPoly[i - 1].poly;
          QPolygonF &poly2 = setPoly[i    ].poly;

          int np = poly1.size();
          assert(poly2.size() == np);

          for (int j = 0; j < np; ++j) {
            const QPointF &p1 = poly1[j];

            double y1 = p1.y();

            const QPointF &p2 = poly2[j];

            double x2 = p2.x(), y2 = p2.y();

            if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
              continue;

            poly2[j] = QPointF(x2, y2 + y1);
          }
        }
      }
    }

    // cumulate
    void cumulate() {
      for (auto &p : groupSetPoly_) {
        SetIndPoly &setPoly = p.second;

        for (int i = 0; i < ns_; ++i) {
          QPolygonF &poly = setPoly[i].poly;

          int np = poly.size();

          for (int j = 1; j < np; ++j) {
            const QPointF &p1 = poly[j - 1];
            const QPointF &p2 = poly[j    ];

            double y1 = p1.y();
            double x2 = p2.x(), y2 = p2.y();

            if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
              continue;

            poly[j] = QPointF(x2, y1 + y2);
          }
        }
      }
    }

    const GroupSetIndPoly &groupSetPoly() const { return groupSetPoly_; }

   private:
    const CQChartsXYPlot* plot_ { nullptr };
    int                   ns_;
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

  int ns = yColumns().count();

  int ig = 0;
  int ng = groupSetIndPoly.size();

  for (auto &p : groupSetIndPoly) {
    if (isInterrupt())
      return false;

    bool hidden = (ns <= 1 && isSetHidden(ig));

    if (! hidden) {
      int               groupInd = p.first;
      const SetIndPoly &setPoly  = p.second;

      if      (isBivariateLines()) {
        if (! addBivariateLines(groupInd, setPoly, ColorInd(ig, ng), objs))
          return false;
      }
      else {
        if (! addLines(groupInd, setPoly, ColorInd(ig, ng), objs))
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

  const QPolygonF &poly = setPoly[0].poly;

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

      const QPolygonF &poly = setPoly[j].poly;

      const QPointF &p = poly[ip];

      if (CMathUtil::isNaN(p.y()) || CMathUtil::isInf(p.y()))
        continue;

      if (j == 0)
        x = p.x();

      sortedYVals.insert(p.y());
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

    QModelIndex xind  = modelIndex(ip, xColumn(), parent);
    QModelIndex xind1 = normalizeIndex(xind);

    //---

    // connect each y value to next y value
    double y1  = yVals[0];
    int    ny1 = yVals.size();

    for (int j = 1; j < ny1; ++j) {
      if (isInterrupt())
        return false;

      double y2 = yVals[j];

      CQChartsGeom::BBox bbox(x - sw/2, y1 - sh/2, x + sw/2, y2 + sh/2);

      if (! isFillUnderFilled()) {
        // use vertical line object for each point pair if not fill under
        ColorInd is(j - 1, ny1 - 1);
        ColorInd iv(ip, np);

        CQChartsXYBiLineObj *lineObj =
          new CQChartsXYBiLineObj(this, groupInd, bbox, x, y1, y2, xind1, is, iv);

        objs.push_back(lineObj);
      }
      else {
        QPolygonF &poly1 = polygons1[j - 1].poly;
        QPolygonF &poly2 = polygons2[j - 1].poly;

        // build lower and upper poly line for fill under polygon
        poly1 << QPointF(x, y1);
        poly2 << QPointF(x, y2);
      }

      y1 = y2;
    }
  }

  //---

  // add lower, upper and polygon objects for fill under
  if (isFillUnderFilled()) {
    QString name = titleStr();

    if (! name.length()) {
      CQChartsColumn yColumn1 = yColumns().getColumn(0);

      CQChartsColumn yColumn2;

      if (yColumns().count() > 1)
        yColumn2 = yColumns().getColumn(1);

      bool ok;

      QString yname1 = modelHeaderString(yColumn1, ok);
      QString yname2 = modelHeaderString(yColumn2, ok);

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    int ns = yColumns().count();

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

      QPolygonF &poly1 = polygons1[j - 1].poly;
      QPolygonF &poly2 = polygons2[j - 1].poly;

      ColorInd is1(j - 1, ns - 1);

      addPolyLine(poly1, groupInd, is1, ig, name1, pointObjs, objs);
      addPolyLine(poly2, groupInd, is1, ig, name1, pointObjs, objs);

      int len = poly1.size();

      CQChartsFillUnderSide::Type fillUnderSideType = fillUnderSide().type();

      if      (fillUnderSideType == CQChartsFillUnderSide::Type::BOTH) {
        // add upper poly line to lower one (points reversed) to build fill polygon
        for (int k = len - 1; k >= 0; --k)
          poly1 << poly2[k];
      }
      else if (fillUnderSideType == CQChartsFillUnderSide::Type::ABOVE) {
        QPolygonF poly3, poly4;

        QPointF pa1, pb1; bool above1 = true;

        for (int k = 0; k < len; ++k) {
          const QPointF &pa2 = poly1[k];
          const QPointF &pb2 = poly2[k];

          bool above2 = (pa2.y() > pb2.y());

          if (k > 0 && above1 != above2) {
            QPointF pi;

            CQChartsUtil::intersectLines(pa1, pa2, pb1, pb2, pi);

            poly3 << pi;
            poly4 << pi;
          }

          if (above2) {
            poly3 << pa2;
            poly4 << pb2;
          }

          pa1 = pa2; pb1 = pb2; above1 = above2;
        }

        len = poly4.size();

        for (int k = len - 1; k >= 0; --k)
          poly3 << poly4[k];

        poly1 = poly3;
      }
      else if (fillUnderSideType == CQChartsFillUnderSide::Type::BELOW) {
        QPolygonF poly3, poly4;

        QPointF pa1, pb1; bool below1 = true;

        for (int k = 0; k < len; ++k) {
          const QPointF &pa2 = poly1[k];
          const QPointF &pb2 = poly2[k];

          bool below2 = (pa2.y() < pb2.y());

          if (k > 0 && below1 != below2) {
            QPointF pi;

            CQChartsUtil::intersectLines(pa1, pa2, pb1, pb2, pi);

            poly3 << pi;
            poly4 << pi;
          }

          if (below2) {
            poly3 << pa2;
            poly4 << pb2;
          }

          pa1 = pa2; pb1 = pb2; below1 = below2;
        }

        len = poly4.size();

        for (int k = len - 1; k >= 0; --k)
          poly3 << poly4[k];

        poly1 = poly3;
      }

      addPolygon(poly1, groupInd, is1, ig, name1, objs);
    }
  }

  return true;
}

bool
CQChartsXYPlot::
addLines(int groupInd, const SetIndPoly &setPoly, const ColorInd &ig, PlotObjs &objs) const
{
  PlotObjs pointObjs;

  double sw = symbolWidth ();
  double sh = symbolHeight();

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  // convert lines into set polygon and set poly lines (more than one if NaNs)
  int ns = yColumns().count();

  for (int is = 0; is < ns; ++is) {
    if (isInterrupt())
      return false;

    bool hidden = (ns > 1 && isSetHidden(is));

    if (hidden)
      continue;

    //---

    // get column name
    CQChartsColumn yColumn = yColumns().getColumn(is);

    bool ok;

    QString name = modelHeaderString(yColumn, ok);

    if (ig.n > 1)
      name = groupIndName(groupInd);

    //---

    const IndPoly &setPoly1 = setPoly[is];

    const QPolygonF     &poly = setPoly1.poly;
    const IndPoly::Inds &inds = setPoly1.inds;

    const IndPoly &setPoly2 = setPoly[is - 1];

    const QPolygonF &prevPoly = (is > 0 ? setPoly2.poly : poly);

    //---

    QPolygonF polyShape, polyLine;

    int np = poly.size();
    assert(prevPoly.size() == np);

    auto pointStartIndex = [&]() {
      if (pointStart_ == 0)
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
      const QPointF &p = poly[ip];

      double x = p.x(), y = p.y();

      //---

      // if point is invalid start new poly line if needed and skip
      if (CMathUtil::isNaN(x) || CMathUtil::isInf(x) ||
          CMathUtil::isNaN(y) || CMathUtil::isInf(y)) {
        if (polyLine.count()) {
          ColorInd is1(is, ns);

          addPolyLine(polyLine, groupInd, is1, ig, name, pointObjs, objs);

          pointObjs.clear();

          polyLine = QPolygonF();
        }

        continue;
      }

      //---

      bool valid = validPointIndex(ip, np);

      if (valid) {
        // get point size
        double size = -1;

        if (sizeColumn().isValid()) {
          bool ok;

          QModelIndex parent; // TODO: parent

          size = modelReal(ip, sizeColumn(), parent, ok);

          if (! ok)
            size = -1;
        }

        //---

        // create point object
        const QModelIndex &xind = inds[ip];

        QModelIndex xind1 = normalizeIndex(xind);

        CQChartsGeom::BBox bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

        CQChartsXYPointObj *pointObj =
          new CQChartsXYPointObj(this, groupInd, bbox, x, y, size, xind1,
                                 ColorInd(is, ns), ig, ColorInd(ip, np));

        pointObjs.push_back(pointObj);

        objs.push_back(pointObj);

        //---

        // set vector data
        if (isVectors()) {
          double vx = 0.0, vy = 0.0;

          if (vectorXColumn().isValid()) {
            bool ok;

            QModelIndex parent; // TODO: parent

            vx = modelReal(ip, vectorXColumn(), parent, ok);
          }

          if (vectorYColumn().isValid()) {
            bool ok;

            QModelIndex parent; // TODO: parent

            vy = modelReal(ip, vectorYColumn(), parent, ok);
          }

          pointObj->setVector(vx, vy);
        }

        //---

        // add optional point label
        if (pointLabelColumn().isValid()) {
          QModelIndex parent; // TODO: parent

          bool ok;

          QString label = modelString(ip, pointLabelColumn(), parent, ok);

          if (ok && label.length()) {
            CQChartsGeom::BBox bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

            ColorInd is1(is, ns);
            ColorInd iv1(ip, np);

            CQChartsXYLabelObj *labelObj =
              new CQChartsXYLabelObj(this, groupInd, bbox, x, y, label, xind1, is1, iv1);

            objs.push_back(labelObj);
          }
        }

        //---

        // set optional point color and symbol
        if (pointColorColumn().isValid()) {
          QModelIndex parent; // TODO: parent

          CQChartsColor c;

          bool ok;

          if (pointColorColumnType_ == CQBaseModelType::COLOR) {
            c = modelColor(ip, pointColorColumn(), parent, ok);
          }
          else {
            QString str = modelString(ip, pointColorColumn(), parent, ok);

            if (ok && str.length())
              c = QColor(str);
          }

          if (c.isValid())
            pointObj->setColor(c);
        }

        if (pointSymbolColumn().isValid()) {
          QModelIndex parent; // TODO: parent

          bool ok;

          QString pointSymbolStr = modelString(ip, pointSymbolColumn(), parent, ok);

          if (ok && pointSymbolStr.length())
            pointObj->setSymbol(CQChartsSymbol::nameToType(pointSymbolStr));
        }

        //---

        // add impulse line (down to or up to zero)
        if (! isStacked() && isImpulseLines()) {
          double w = lengthPlotWidth(impulseLinesWidth());

          double ys = std::min(y, 0.0);
          double ye = std::max(y, 0.0);

          CQChartsGeom::BBox bbox(x - w/2, ys, x + w/2, ye);

          ColorInd is1(is, ns);
          ColorInd iv1(ip, np);

          CQChartsXYImpulseLineObj *impulseObj =
            new CQChartsXYImpulseLineObj(this, groupInd, bbox, x, ys, ye, xind1, is1, iv1);

          objs.push_back(impulseObj);
        }
      }

      //---

      // add point to poly line
      polyLine << p;

      //---

      // add point to polygon

      // if first point then add first point of previous polygon
      if (ip == 0) {
        if (isStacked()) {
          double y1 = (is > 0 ? prevPoly[ip].y() : dataRange.ymin());

          if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
            y1 = dataRange.ymin();

          polyShape << QPointF(x, y1);
        }
        else
          polyShape << calcFillUnderPos(x, dataRange.ymin());
      }

      polyShape << p;

      // if last point then add last point of previous polygon
      if (ip == np - 1) {
        if (isStacked()) {
          double y1 = (is > 0 ? prevPoly[ip].y() : dataRange.ymin());

          if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
            y1 = dataRange.ymin();

          polyShape << QPointF(x, y1);
        }
        else
          polyShape << calcFillUnderPos(x, dataRange.ymin());
      }
    }

    //---

    if (isStacked()) {
      // add points from previous polygon to bottom of polygon
      if (is > 0) {
        for (int ip = np - 2; ip >= 1; --ip) {
          double x1 = prevPoly[ip].x();
          double y1 = prevPoly[ip].y();

          if (CMathUtil::isNaN(x1) || CMathUtil::isInf(x1))
            x1 = poly[ip].x();

          if (CMathUtil::isNaN(y1) || CMathUtil::isInf(y1))
            y1 = 0.0;

          polyShape << QPointF(x1, y1);
        }
      }
    }

    //---

    if (polyLine.count()) {
      ColorInd is1(is, ns);

      addPolyLine(polyLine, groupInd, is1, ig, name, pointObjs, objs);

      pointObjs.clear();

      //polyLine = QPolygonF();
    }

    //---

    ColorInd is1(is, ns);

    addPolygon(polyShape, groupInd, is1, ig, name, objs);
  }

  return true;
}

bool
CQChartsXYPlot::
rowData(const ModelVisitor::VisitData &data, double &x, std::vector<double> &y,
        QModelIndex &ind, bool skipBad) const
{
  ind = modelIndex(data.row, xColumn(), data.parent);

  bool ok1 = modelMappedReal(data.row, xColumn(), data.parent, x, isLogX(), data.row);

  //---

  bool ok2 = true;

  int ns = yColumns().count();

  for (int i = 0; i < ns; ++i) {
    CQChartsColumn yColumn = yColumns().getColumn(i);

    double y1;

    bool ok3 = modelMappedReal(data.row, yColumn, data.parent, y1, isLogY(), data.row);

    if (! ok3) {
      if (skipBad)
        continue;

      y1  = CMathUtil::getNaN();
      ok2 = false;
    }

    //---

    y.push_back(y1);
  }

  if (ns && y.empty())
    return false;

  return (ok1 && ok2);
}

QPointF
CQChartsXYPlot::
calcFillUnderPos(double x, double y) const
{
  const CQChartsFillUnderPos &pos = fillUnderPos();

  double x1 = x;
  double y1 = y;

  const CQChartsGeom::Range &dataRange = this->dataRange();

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

  return QPointF(x1, y1);
}

CQChartsXYPolylineObj *
CQChartsXYPlot::
addPolyLine(const QPolygonF &polyLine, int groupInd, const ColorInd &is, const ColorInd &ig,
            const QString &name, PlotObjs &pointObjs, PlotObjs &objs) const
{
  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(polyLine.boundingRect());

  CQChartsXYPolylineObj *lineObj =
    new CQChartsXYPolylineObj(this, groupInd, bbox, polyLine, name, is, ig);

  for (auto &pointObj : pointObjs) {
    CQChartsXYPointObj *pointObj1 = dynamic_cast<CQChartsXYPointObj *>(pointObj);

    if (pointObj1)
      pointObj1->setLineObj(lineObj);
  }

  objs.push_back(lineObj);

  return lineObj;
}

void
CQChartsXYPlot::
addPolygon(const QPolygonF &poly, int groupInd, const ColorInd &is, const ColorInd &ig,
           const QString &name, PlotObjs &objs) const
{
  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(poly.boundingRect());

  CQChartsXYPolygonObj *polyObj =
    new CQChartsXYPolygonObj(this, groupInd, bbox, poly, name, is, ig);

  objs.push_back(polyObj);
}

QString
CQChartsXYPlot::
valueName(int is, int ns, int irow) const
{
  QString name;

  if (ns > 1 && is >= 0) {
    CQChartsColumn yColumn = yColumns().getColumn(is);

    bool ok;

    name = modelHeaderString(yColumn, ok);
  }

  if (nameColumn().isValid()) {
    QModelIndex parent; // TODO: parent

    bool ok;

    QString name1 = modelString(irow, nameColumn(), parent, ok);

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
    CQChartsXYKeyColor *color = new CQChartsXYKeyColor(this, is, ig);
    CQChartsXYKeyText  *text  = new CQChartsXYKeyText (this, name, is, ig);

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

  int ns = yColumns().count();
  int ng = numGroups();

  if      (isBivariateLines()) {
    QString name = titleStr();

    if (! name.length()) {
      CQChartsColumn yColumn1 = yColumns().getColumn(0);

      CQChartsColumn yColumn2;

      if (ns > 1)
        yColumn2 = yColumns().getColumn(1);

      bool ok;

      QString yname1 = modelHeaderString(yColumn1, ok);
      QString yname2 = modelHeaderString(yColumn2, ok);

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    ColorInd is, ig;

    addKeyItem(name, is, ig);
  }
  else if (isStacked()) {
    for (int i = 0; i < ns; ++i) {
      CQChartsColumn yColumn = yColumns().getColumn(i);

      bool ok;

      QString name = modelHeaderString(yColumn, ok);

      ColorInd is(i, ns), ig;

      addKeyItem(name, is, ig);
    }
  }
  else {
    if      (ns > 1) {
      for (int i = 0; i < ns; ++i) {
        CQChartsColumn yColumn = yColumns().getColumn(i);

        bool ok;

        QString name = modelHeaderString(yColumn, ok);

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
        name = yAxisName();

      ColorInd is, ig;

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
    CQChartsXYPolylineObj *polyObj = dynamic_cast<CQChartsXYPolylineObj *>(plotObj);

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
    CQChartsXYPolylineObj *polyObj = dynamic_cast<CQChartsXYPolylineObj *>(plotObj);

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
  CQChartsGeom::BBox bbox = CQChartsPlot::dataFitBBox();

  for (const auto &plotObj : plotObjs_) {
    CQChartsXYLabelObj *labelObj = dynamic_cast<CQChartsXYLabelObj *>(plotObj);

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
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name,
                                  bool isSet, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  //--

  int ns = yColumns().count();

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

  return true;
}

//------

void
CQChartsXYPlot::
drawArrow(QPainter *painter, const QPointF &p1, const QPointF &p2) const
{
  disconnect(arrowObj_, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

  arrowObj_->setFrom(p1);
  arrowObj_->setTo  (p2);

  connect(arrowObj_, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

  arrowObj_->draw(painter);
}

//------

CQChartsXYBiLineObj::
CQChartsXYBiLineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                    double x, double y1, double y2, const QModelIndex &ind,
                    const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ColorInd(), iv), plot_(plot),
 groupInd_(groupInd), x_(x), y1_(y1), y2_(y2), ind_(ind)
{
}

QString
CQChartsXYBiLineObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(ind());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3").arg(typeName()).arg(is_.i).arg(iv_.i);
}

QString
CQChartsXYBiLineObj::
calcTipId() const
{
  QString name  = plot()->valueName(-1, -1, ind().row());
  QString xstr  = plot()->xStr(x());
  QString y1str = plot()->yStr(y1());
  QString y2str = plot()->yStr(y2());

  CQChartsTableTip tableTip;

  if (name.length())
    tableTip.addTableRow("Name", name);

  QString xname = plot()->xAxisName("X");

  tableTip.addTableRow(xname, xstr );
  tableTip.addTableRow("Y1" , y1str);
  tableTip.addTableRow("Y2" , y2str);

  //---

  plot()->addTipColumns(tableTip, ind_);

  //---

  return tableTip.str();
}

bool
CQChartsXYBiLineObj::
visible() const
{
  if (! plot()->isLines() && ! plot()->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsXYBiLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  CQChartsGeom::Point p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y1()));
  CQChartsGeom::Point p2 = plot()->windowToPixel(CQChartsGeom::Point(x(), y2()));

  double sx, sy;

  plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(p1.x - sx, p1.y - sy, p2.x + sx, p2.y + sy);

  CQChartsGeom::Point pp = plot()->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsXYBiLineObj::
getSelectIndices(Indices &inds) const
{
  if (! visible())
    return;

  addColumnSelectIndex(inds, ind().column());
}

void
CQChartsXYBiLineObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind().row(), column, ind().parent());
}

void
CQChartsXYBiLineObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  CQChartsGeom::Point p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y1()));
  CQChartsGeom::Point p2 = plot()->windowToPixel(CQChartsGeom::Point(x(), y2()));

  if (plot()->isLines()) {
    // calc pen and brush
    QPen   pen;
    QBrush brush;

    QColor lc = plot()->interpBivariateLinesColor(is_);

    plot()->setPen(pen, true, lc, plot()->bivariateLinesAlpha(), plot()->bivariateLinesWidth(),
                   plot()->bivariateLinesDash());

    plot()->setBrush(brush, false);

    plot()->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //--

    // draw line
    painter->drawLine(QPointF(p1.x, p1.y), QPointF(p2.x, p2.y));
  }

  if (plot()->isPoints()) {
    // get symbol and size
    CQChartsSymbol symbol = plot()->symbolType();

    double sx, sy;

    plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

    //---

    // calc pen and brush
    QPen   pen;
    QBrush brush;

    plot_->setSymbolPenBrush(pen, brush, is_);

    plot_->updateObjPenBrushState(this, pen, brush, CQChartsPlot::DrawType::SYMBOL);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    // draw symbols
    plot()->drawSymbol(painter, QPointF(p1.x, p1.y), symbol, CMathUtil::avg(sx, sy), pen, brush);
    plot()->drawSymbol(painter, QPointF(p2.x, p2.y), symbol, CMathUtil::avg(sx, sy), pen, brush);
  }
}

//------

CQChartsXYImpulseLineObj::
CQChartsXYImpulseLineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                         double x, double y1, double y2, const QModelIndex &ind,
                         const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ColorInd(), iv), plot_(plot),
 groupInd_(groupInd), x_(x), y1_(y1), y2_(y2), ind_(ind)
{
}

QString
CQChartsXYImpulseLineObj::
calcId() const
{
  QModelIndex ind1 = plot()->unnormalizeIndex(ind());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3").arg(typeName()).arg(is_.i).arg(iv_.i);
}

QString
CQChartsXYImpulseLineObj::
calcTipId() const
{
  QString name  = plot()->valueName(is_.i, is_.n, ind().row());
  QString xstr  = plot()->xStr(x());
  QString y1str = plot()->yStr(y1());
  QString y2str = plot()->yStr(y2());

  CQChartsTableTip tableTip;

  if (name.length())
    tableTip.addTableRow("Name", name);

  QString xname = plot()->xAxisName("X");

  tableTip.addTableRow(xname, xstr );
  tableTip.addTableRow("Y1" , y1str);
  tableTip.addTableRow("Y2" , y2str);

  //---

  plot()->addTipColumns(tableTip, ind_);

  //---

  return tableTip.str();
}

bool
CQChartsXYImpulseLineObj::
visible() const
{
  return isVisible();
}

bool
CQChartsXYImpulseLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  CQChartsGeom::Point p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y1()));
  CQChartsGeom::Point p2 = plot()->windowToPixel(CQChartsGeom::Point(x(), y2()));

  double b = 2;

  double lw = std::max(plot()->lengthPixelWidth(plot()->impulseLinesWidth()), 2*b);

  CQChartsGeom::BBox pbbox(p1.x - lw/2, p1.y - b, p2.x + lw/2, p2.y + b);

  CQChartsGeom::Point pp = plot()->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsXYImpulseLineObj::
getSelectIndices(Indices &inds) const
{
  if (! visible())
    return;

  addColumnSelectIndex(inds, ind().column());
}

void
CQChartsXYImpulseLineObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind().row(), column, ind().parent());
}

void
CQChartsXYImpulseLineObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  ColorInd ic = (is_.n > 1 ? is_ : iv_);

  //---

  // calc pen and brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = plot()->interpImpulseLinesColor(ic);

  double lw = plot()->lengthPixelWidth(plot()->impulseLinesWidth());

  if (lw <= 1) {
    plot()->setPen(pen, true, strokeColor, plot()->impulseLinesAlpha(),
                   plot()->impulseLinesWidth(), plot()->impulseLinesDash());

    plot()->setBrush(brush, false);
  }
  else {
    plot()->setPen(pen, false);

    plot()->setBrush(brush, true, strokeColor, plot()->impulseLinesAlpha());
  }

  plot()->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw impulse
  CQChartsGeom::Point p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y1()));
  CQChartsGeom::Point p2 = plot()->windowToPixel(CQChartsGeom::Point(x(), y2()));

  if (lw <= 1) {
    painter->drawLine(QPointF(p1.x, p1.y), QPointF(p2.x, p2.y));
  }
  else {
    QRectF qrect(p1.x - lw/2, p1.y, lw, p2.y - p1.y);

    CQChartsRoundedPolygon::draw(painter, qrect, 0, 0);
  }
}

//------

CQChartsXYPointObj::
CQChartsXYPointObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                   double x, double y, double size, const QModelIndex &ind,
                   const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ig, iv),
 plot_(plot), groupInd_(groupInd), pos_(x, y), size_(size), ind_(ind)
{
}

CQChartsXYPointObj::
~CQChartsXYPointObj()
{
  delete edata_;
}

void
CQChartsXYPointObj::
setSelected(bool b)
{
  CQChartsPlotObj::setSelected(b);

  if (plot()->isPointLineSelect() && lineObj())
    const_cast<CQChartsXYPolylineObj *>(lineObj())->setSelected(b);
}

QString
CQChartsXYPointObj::
calcId() const
{
  QModelIndex ind1 = plot()->unnormalizeIndex(ind());

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

  QModelIndex ind1 = plot()->unnormalizeIndex(ind());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    tableTip.addTableRow("Id", idStr);

  if (ig_.n > 1) {
    QString groupName = plot()->groupIndName(ig_.i);

    tableTip.addTableRow("Group", groupName);
  }

  QString name = plot()->valueName(is_.i, is_.n, ind().row());
  QString xstr = plot()->xStr(x());
  QString ystr = plot()->yStr(y());

  if (name.length())
    tableTip.addTableRow("Name", name);

  QString xname = plot()->xAxisName("X");
  QString yname = plot()->yAxisName("Y");

  tableTip.addTableRow(xname, xstr);
  tableTip.addTableRow(yname, ystr);

  if (size_ > 0)
    tableTip.addTableRow("Size", size_);

  //---

  plot()->addTipColumns(tableTip, ind());

  //---

  return tableTip.str();
}

void
CQChartsXYPointObj::
setColor(const CQChartsColor &c)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->color = c;
}

void
CQChartsXYPointObj::
setSymbol(CQChartsSymbol symbol)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->symbol = symbol;
}

void
CQChartsXYPointObj::
setVector(double vx, double vy)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->vector = QPointF(vx, vy);
}

bool
CQChartsXYPointObj::
visible() const
{
  if (! plot()->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsXYPointObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  CQChartsGeom::Point p1 = plot()->windowToPixel(CQChartsGeom::Point(x(), y()));

  double sx = size();
  double sy = sx;

  if (sx <= 0)
    plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  CQChartsGeom::Point pp = plot()->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsXYPointObj::
getSelectIndices(Indices &inds) const
{
  if (! visible())
    return;

  addColumnSelectIndex(inds, plot()->xColumn());
  addColumnSelectIndex(inds, plot()->yColumns().getColumn(is_.i));
}

void
CQChartsXYPointObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind().row(), column, ind().parent());
}

void
CQChartsXYPointObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  // calc pen and brush
  QPen   pen;
  QBrush brush;

  ColorInd ic;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO)
    ic = is_;
  else
    ic = calcColorInd();

  plot()->setSymbolPenBrush(pen, brush, ic);

  if (edata_ && edata_->color.isValid()) {
    QColor strokeColor = plot()->interpColor(edata_->color, ColorInd());

    pen.setColor(strokeColor);
  }

#if 0
  if (plot()->isStatsLines() && lineObj()->isOutlier(pos_.y())) {
    brush.setColor(Qt::red);
    pen  .setColor(Qt::red);
  }
#endif

  plot()->updateObjPenBrushState(this, pen, brush, CQChartsPlot::DrawType::SYMBOL);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw symbol
  CQChartsSymbol symbol = plot()->symbolType();

  if (edata_ && edata_->symbol.type() != CQChartsSymbol::Type::NONE)
    symbol = edata_->symbol;

  double sx = size();
  double sy = sx;

  if (sx <= 0)
    plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

  CQChartsGeom::Point pp = CQChartsUtil::fromQPoint(pos_);

  CQChartsGeom::Point p = plot()->windowToPixel(pp);

  plot()->drawSymbol(painter, QPointF(p.x, p.y), symbol, CMathUtil::avg(sx, sy), pen, brush);

  //---

  // draw optional vector
  if (edata_ && edata_->vector) {
    QPointF p1(pp.x, pp.y);

    QPointF p2 = p1 + QPointF(*edata_->vector);

    plot()->drawArrow(painter, p1, p2);
  }
}

//------

CQChartsXYLabelObj::
CQChartsXYLabelObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                   double x, double y, const QString &label, const QModelIndex &ind,
                   const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ColorInd(), iv), plot_(plot),
 groupInd_(groupInd), pos_(x, y), label_(label), ind_(ind)
{
}

CQChartsXYLabelObj::
~CQChartsXYLabelObj()
{
}

QString
CQChartsXYLabelObj::
calcId() const
{
  QModelIndex ind1 = plot()->unnormalizeIndex(ind());

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

  tableTip.addTableRow("Label", label_);

  //---

  plot()->addTipColumns(tableTip, ind_);

  //---

  return tableTip.str();
}

bool
CQChartsXYLabelObj::
visible() const
{
  if (! plot()->isDataLabelTextVisible())
    return false;

  return isVisible();
}

bool
CQChartsXYLabelObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  CQChartsGeom::Point ppos = plot()->windowToPixel(CQChartsUtil::fromQPoint(pos_));

  double sx = 16;
  double sy = 16;

  CQChartsGeom::BBox pbbox(ppos.x - sx, ppos.y - sy, ppos.x + sx, ppos.y + sy);

  CQChartsGeom::Point pp = plot()->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsXYLabelObj::
getSelectIndices(Indices &inds) const
{
  if (! visible())
    return;

  addColumnSelectIndex(inds, plot()->xColumn());
  addColumnSelectIndex(inds, plot()->yColumns().getColumn(is_.i));
}

void
CQChartsXYLabelObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind().row(), column, ind().parent());
}

void
CQChartsXYLabelObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  // set pen
  QPen tpen;

  QColor tc = plot()->interpDataLabelTextColor(ColorInd());

  plot()->setPen(tpen, true, tc, plot()->dataLabelTextAlpha());

  painter->setPen(tpen);

  //--

  // draw text
  CQChartsGeom::Point ppos = plot()->windowToPixel(CQChartsUtil::fromQPoint(pos_));

  plot()->view()->setPlotPainterFont(plot(), painter, plot()->dataLabelTextFont());

  CQChartsRotatedText::draw(painter, ppos.x, ppos.y, label_, plot()->dataLabelTextAngle(),
                            plot()->dataLabelTextAlign(), plot()->isDataLabelTextContrast());
}

//------

CQChartsXYPolylineObj::
CQChartsXYPolylineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                      const QPolygonF &poly, const QString &name, const ColorInd &is,
                      const ColorInd &ig) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
 groupInd_(groupInd), poly_(poly), name_(name)
{
}

CQChartsXYPolylineObj::
~CQChartsXYPolylineObj()
{
  delete smooth_;
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

bool
CQChartsXYPolylineObj::
visible() const
{
  if (! plot()->isLines())
    return false;

  return isVisible();
}

bool
CQChartsXYPolylineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  if (! plot()->isLinesSelectable())
    return false;

  CQChartsGeom::Point pp = plot()->windowToPixel(p);

  for (int i = 1; i < poly_.count(); ++i) {
    CQChartsGeom::Point p1(poly_[i - 1]);
    CQChartsGeom::Point p2(poly_[i    ]);

    CQChartsGeom::Point pl1 = plot()->windowToPixel(p1);
    CQChartsGeom::Point pl2 = plot()->windowToPixel(p2);

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
  if (! plot()->isLinesSelectable())
    return false;

  return CQChartsPlotObj::rectIntersect(r, inside);
}

bool
CQChartsXYPolylineObj::
interpY(double x, std::vector<double> &yvals) const
{
  if (! visible())
    return false;

  for (int i = 1; i < poly_.count(); ++i) {
    double x1 = poly_[i - 1].x();
    double y1 = poly_[i - 1].y();
    double x2 = poly_[i    ].x();
    double y2 = poly_[i    ].y();

    if (x >= x1 && x <= x2) {
      double y = (y2 - y1)*(x - x1)/(x2 - x1) + y1;

      yvals.push_back(y);
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
getSelectIndices(Indices &) const
{
  if (! visible())
    return;

  // all objects part of line (don't support select)
}

void
CQChartsXYPolylineObj::
initSmooth()
{
  // init smooth if needed
  if (! smooth_) {
    CQChartsSmooth::Points points;

    int np = poly_.count();

    for (int i = 0; i < np; ++i) {
      const QPointF &p = poly_[i];

      points.emplace_back(p.x(), p.y());
    }

    smooth_ = new CQChartsSmooth(points, /*sorted*/false);
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

      QPolygonF poly;

      for (const auto &p : poly_) {
        if (! statData_.isOutlier(p.y()))
          poly.push_back(p);
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
      y.push_back(poly_[i].y());

    std::sort(y.begin(), y.end());

    statData_.calcStatValues(y);
  }
}

void
CQChartsXYPolylineObj::
draw(QPainter *painter)
{
  if (! visible() && ! plot()->isBestFit() && ! plot()->isStatsLines())
    return;

  //---

  ColorInd ic = (ig_.n > 1 ? ig_ : is_);

  //---

  // draw lines
  if (visible()) {
    // calc pen and brush
    QPen   pen;
    QBrush brush;

    QColor c = plot()->interpLinesColor(ic);

    plot()->setPen(pen, true, c, plot()->linesAlpha(), plot()->linesWidth(), plot()->linesDash());

    plot()->setBrush(brush, false);

    plot()->updateObjPenBrushState(this, ic, pen, brush, CQChartsPlot::DrawType::LINE);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    if (plot()->isRoundedLines()) {
      initSmooth();

      QPainterPath path;

      for (int i = 0; i < smooth_->numPoints(); ++i) {
        if (plot()->isInterrupt())
          return;

        const CQChartsGeom::Point &p = smooth_->point(i);

        CQChartsGeom::Point p1 = plot()->windowToPixel(p);

        if (i == 0)
          path.moveTo(p1.x, p1.y);
        else {
          CQChartsSmooth::SegmentType type = smooth_->segmentType(i - 1);

          if      (type == CQChartsSmooth::SegmentType::CURVE3) {
            CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);
            CQChartsGeom::Point c2 = smooth_->controlPoint2(i - 1);

            CQChartsGeom::Point pc1 = plot()->windowToPixel(c1);
            CQChartsGeom::Point pc2 = plot()->windowToPixel(c2);

            path.cubicTo(pc1.x, pc1.y, pc2.x, pc2.y, p1.x, p1.y);
          }
          else if (type == CQChartsSmooth::SegmentType::CURVE2) {
            CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);

            CQChartsGeom::Point pc1 = plot()->windowToPixel(c1);

            path.quadTo(pc1.x, pc1.y, p1.x, p1.y);
          }
          else if (type == CQChartsSmooth::SegmentType::LINE) {
            path.lineTo(p1.x, p1.y);
          }
        }
      }

      painter->drawPath(path);
    }
    else {
      int np = poly_.count();

      for (int i = 1; i < np; ++i) {
        if (plot()->isInterrupt())
          return;

        painter->drawLine(plot()->windowToPixel(poly_[i - 1]), plot()->windowToPixel(poly_[i]));
      }
    }
  }

  //---

  if (plot()->isBestFit()) {
    initBestFit();

    //---

    // calc fit shape at each pixel
    QPolygonF bpoly, poly, tpoly;

    CQChartsGeom::Point pl = plot()->windowToPixel(CQChartsGeom::Point(bestFit_.xmin(), 0));
    CQChartsGeom::Point pr = plot()->windowToPixel(CQChartsGeom::Point(bestFit_.xmax(), 0));

    for (int px = pl.x; px <= pr.x; ++px) {
      if (plot()->isInterrupt())
        return;

      CQChartsGeom::Point p1 = plot()->pixelToWindow(CQChartsGeom::Point(px, 0.0));

      double y2 = bestFit_.interp(p1.x);

      CQChartsGeom::Point p2 = plot()->windowToPixel(CQChartsGeom::Point(p1.x, y2));

      poly << QPointF(p2.x, p2.y);

      // deviation curve above/below
      if (plot()->isBestFitDeviation()) {
        p2 = plot()->windowToPixel(CQChartsGeom::Point(p1.x, y2 - bestFit_.deviation()));

        bpoly << QPointF(p2.x, p2.y);

        p2 = plot()->windowToPixel(CQChartsGeom::Point(p1.x, y2 + bestFit_.deviation()));

        tpoly << QPointF(p2.x, p2.y);
      }
    }

    //---

    if (poly.size()) {
      // calc pen and brush
      QPen   pen;
      QBrush brush;

      QColor strokeColor = plot()->interpBestFitStrokeColor(ic);
      QColor fillColor   = plot()->interpBestFitFillColor  (ic);

      plot()->setPen(pen, true, strokeColor, plot()->bestFitStrokeAlpha(),
                     plot()->bestFitStrokeWidth(), plot()->bestFitStrokeDash());

      plot()->setBrush(brush, plot()->isBestFitFilled(), fillColor, plot()->bestFitFillAlpha(),
                       plot()->bestFitFillPattern());

      plot()->updateObjPenBrushState(this, ic, pen, brush, CQChartsPlot::DrawType::LINE);

      painter->setPen  (pen);
      painter->setBrush(brush);

      //---

      // draw fit deviation shape
      if (plot()->isBestFitDeviation()) {
        QPolygonF dpoly;

        for (int i = 0; i < bpoly.size(); ++i) {
          if (plot()->isInterrupt())
            return;

          const QPointF &p = bpoly[i];

          dpoly << p;
        }

        for (int i = tpoly.size() - 1; i >= 0; --i) {
          if (plot()->isInterrupt())
            return;

          const QPointF &p = tpoly[i];

          dpoly << p;
        }

        painter->drawPolygon(dpoly);
      }

      //---

      // draw fit line
      QPainterPath path;

      const QPointF &p = poly[0];

      path.moveTo(p);

      for (int i = 1; i < poly.size(); ++i) {
        if (plot()->isInterrupt())
          return;

        const QPointF &p = poly[i];

        path.lineTo(p);
      }

      painter->strokePath(path, pen);
    }
  }

  //---

  if (plot()->isStatsLines()) {
    initStats();

    //---

    // calc pen and brush
    QPen   pen;
    QBrush brush;

    QColor c = plot()->interpStatsLinesColor(ic);

    plot()->setPen(pen, true, c, plot()->statsLinesAlpha(),
                   plot()->statsLinesWidth(), plot()->statsLinesDash());

    plot()->setBrush(brush, false);

    plot()->updateObjPenBrushState(this, ic, pen, brush, CQChartsPlot::DrawType::LINE);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    const CQChartsGeom::Range &dataRange = plot()->dataRange();

    auto drawStatLine = [&](double y) {
      QPointF p1 = plot()->windowToPixel(QPointF(dataRange.xmin(), y));
      QPointF p2 = plot()->windowToPixel(QPointF(dataRange.xmax(), y));

      painter->drawLine(p1, p2);
    };

    drawStatLine(statData_.loutlier   );
    drawStatLine(statData_.lowerMedian);
    drawStatLine(statData_.median     );
    drawStatLine(statData_.upperMedian);
    drawStatLine(statData_.uoutlier   );
  }
}

//------

CQChartsXYPolygonObj::
CQChartsXYPolygonObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                     const QPolygonF &poly, const QString &name, const ColorInd &is,
                     const ColorInd &ig) :
 CQChartsPlotObj(const_cast<CQChartsXYPlot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
 groupInd_(groupInd), poly_(poly), name_(name)
{
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
  tableTip.addTableRow("Area", CQUtil::polygonArea(poly_));

  //---

  //plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

bool
CQChartsXYPolygonObj::
visible() const
{
  if (! plot()->isFillUnderFilled())
    return false;

  return isVisible();
}

bool
CQChartsXYPolygonObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  if (! plot()->isFillUnderSelectable())
    return false;

  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

bool
CQChartsXYPolygonObj::
rectIntersect(const CQChartsGeom::BBox &r, bool inside) const
{
  if (! plot()->isFillUnderSelectable())
    return false;

  return CQChartsPlotObj::rectIntersect(r, inside);
}

void
CQChartsXYPolygonObj::
getSelectIndices(Indices &) const
{
  if (! visible())
    return;

  // all objects part of polygon (don't support select)
}

void
CQChartsXYPolygonObj::
initSmooth()
{
  // init smooth if needed
  // (not first point and last point are the extra points to make the square protrusion
  if (! smooth_) {
    CQChartsSmooth::Points points;

    int np = poly_.count();

    for (int i = 1; i < np - 1; ++i) {
      const QPointF &p = poly_[i];

      points.emplace_back(p.x(), p.y());
    }

    smooth_ = new CQChartsSmooth(points, /*sorted*/false);
  }
}

void
CQChartsXYPolygonObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  ColorInd ic = (ig_.n > 1 ? ig_ : is_);

  //---

  // calc pen and brush
  QPen   pen;
  QBrush brush;

  QColor fillColor = plot()->interpFillUnderFillColor(ic);

  plot()->setPen(pen, false);

  plot()->setBrush(brush, true, fillColor, plot()->fillUnderFillAlpha(),
                   plot()->fillUnderFillPattern());

  plot()->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // create polygon path (rounded or normal)
  int np = poly_.count();

  if (plot()->isRoundedLines()) {
    initSmooth();

    QPainterPath path;

    if (np > 0) {
      CQChartsGeom::Point p =
        plot()->windowToPixel(CQChartsGeom::Point(poly_[0].x(), poly_[0].y()));

      path.moveTo(p.x, p.y);
    }

    for (int i = 0; i < smooth_->numPoints(); ++i) {
      const CQChartsGeom::Point &p = smooth_->point(i);

      CQChartsGeom::Point p1 = plot()->windowToPixel(p);

      if (i == 0)
        path.lineTo(p1.x, p1.y);
      else {
        CQChartsSmooth::SegmentType type = smooth_->segmentType(i - 1);

        if      (type == CQChartsSmooth::SegmentType::CURVE3) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);
          CQChartsGeom::Point c2 = smooth_->controlPoint2(i - 1);

          CQChartsGeom::Point pc1 = plot()->windowToPixel(c1);
          CQChartsGeom::Point pc2 = plot()->windowToPixel(c2);

          path.cubicTo(pc1.x, pc1.y, pc2.x, pc2.y, p1.x, p1.y);
        }
        else if (type == CQChartsSmooth::SegmentType::CURVE2) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);

          CQChartsGeom::Point pc1 = plot()->windowToPixel(c1);

          path.quadTo(pc1.x, pc1.y, p1.x, p1.y);
        }
        else if (type == CQChartsSmooth::SegmentType::LINE) {
          path.lineTo(p1.x, p1.y);
        }
      }
    }

    if (np > 0) {
      CQChartsGeom::Point p =
        plot()->windowToPixel(CQChartsGeom::Point(poly_[np - 1].x(), poly_[np - 1].y()));

      path.lineTo(p.x, p.y);
    }

    path.closeSubpath();

    //---

    // draw polygon
    painter->drawPath(path);
  }
  else {
    // draw polygon
    QPolygonF poly;

    for (int i = 0; i < np; ++i) {
      CQChartsGeom::Point p =
        plot()->windowToPixel(CQChartsGeom::Point(poly_[i].x(), poly_[i].y()));

      poly << QPointF(p.x, p.y);
    }

    painter->drawPolygon(poly);
  }
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
  CQChartsPlotObj *obj = plotObj();
  if (! obj) return;

  if      (selMod == CQChartsSelMod::REPLACE) {
    for (int ig = 0; ig < ig_.n; ++ig) {
      CQChartsPlotObj *obj1 = plot()->getGroupObj(ig);

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
  double              alpha   = 1.0;
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

  CQChartsPlotObj *obj = plot()->getGroupObj(ig_.i);

  return obj;
}

//------

CQChartsXYKeyLine::
CQChartsXYKeyLine(CQChartsXYPlot *plot, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyItem(plot->key(), is.n > 1 ? is : ig), plot_(plot), is_(is), ig_(ig)
{
  setClickable(true);
}

QSizeF
CQChartsXYKeyLine::
size() const
{
  CQChartsXYPlot *keyPlot = qobject_cast<CQChartsXYPlot *>(key_->plot());

  if (! keyPlot)
    keyPlot = plot();

  QFont font = plot()->view()->plotFont(plot(), key_->textFont());

  QFontMetricsF fm(font);

  double w = fm.width("-X-");
  double h = fm.height();

  double ww = keyPlot->pixelToWindowWidth (w + 8);
  double wh = keyPlot->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

void
CQChartsXYKeyLine::
doSelect(CQChartsSelMod selMod)
{
  CQChartsPlotObj *obj = plotObj();
  if (! obj) return;

  if      (selMod == CQChartsSelMod::REPLACE) {
    for (int ig = 0; ig < ig_.n; ++ig) {
      CQChartsPlotObj *obj1 = plot()->getGroupObj(ig);

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

void
CQChartsXYKeyLine::
draw(QPainter *painter, const CQChartsGeom::BBox &rect) const
{
  painter->save();

  CQChartsPlot *keyPlot = qobject_cast<CQChartsPlot *>(key_->plot());

  //CQChartsXYPlot *xyKeyPlot = qobject_cast<CQChartsXYPlot *>(keyPlot);
  //if (! xyKeyPlot) xyKeyPlot = plot();

  CQChartsGeom::BBox prect = keyPlot->windowToPixel(rect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  painter->setClipRect(prect1, Qt::IntersectClip);

  QColor hideBg;
  double hideAlpha { 1.0 };

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

    painter->fillRect(CQChartsUtil::toQRect(CQChartsGeom::BBox(x1, y1, x2, y2)), fillBrush);
  }

  if (plot()->isLines() || plot()->isBestFit() || plot()->isImpulseLines()) {
    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y  = prect.getYMid();

    QPen linePen;

    if      (plot()->isLines()) {
      QColor lineColor = plot()->interpLinesColor(ic_);

      if (plot()->isSetHidden(ic_.i))
        lineColor = CQChartsUtil::blendColors(lineColor, hideBg, hideAlpha);

      plot()->setPen(linePen, true, lineColor, plot()->linesAlpha(),
                     plot()->linesWidth(), plot()->linesDash());
    }
    else if (plot()->isBestFit()) {
      QColor fitColor = plot()->interpBestFitStrokeColor(ic_);

      if (plot()->isSetHidden(ic_.i))
        fitColor = CQChartsUtil::blendColors(fitColor, hideBg, hideAlpha);

      plot()->setPen(linePen, true, fitColor, plot()->bestFitStrokeAlpha(),
                     plot()->bestFitStrokeWidth(), plot()->bestFitStrokeDash());
    }
    else {
      QColor impulseColor = plot()->interpImpulseLinesColor(ic_);

      if (plot()->isSetHidden(ic_.i))
        impulseColor = CQChartsUtil::blendColors(impulseColor, hideBg, hideAlpha);

      plot()->setPen(linePen, true, impulseColor, plot()->impulseLinesAlpha(),
                     plot()->impulseLinesWidth(), plot()->impulseLinesDash());
    }

    QBrush lineBrush(Qt::NoBrush);

    CQChartsPlotObj *obj = plotObj();

    if (obj)
      plot()->updateObjPenBrushState(obj, ig_, linePen, lineBrush, CQChartsPlot::DrawType::LINE);

    if (isInside())
      linePen = plot()->insideColor(linePen.color());

    painter->setPen(linePen);

    painter->drawLine(QPointF(x1, y), QPointF(x2, y));
  }

  if (plot()->isPoints()) {
    double dx = keyPlot->pixelToWindowWidth(4);

    double x1 = rect.getXMin() + dx;
    double x2 = rect.getXMax() - dx;
    double y  = rect.getYMid();

    CQChartsGeom::Point p1 = keyPlot->windowToPixel(CQChartsGeom::Point(x1, y));
    CQChartsGeom::Point p2 = keyPlot->windowToPixel(CQChartsGeom::Point(x2, y));

    //---

    QColor pointStrokeColor = plot()->interpSymbolStrokeColor(ic_);
    QColor pointFillColor   = plot()->interpSymbolFillColor  (ic_);

    if (plot()->isSetHidden(ic_.i)) {
      pointStrokeColor = CQChartsUtil::blendColors(pointStrokeColor, hideBg, hideAlpha);
      pointFillColor   = CQChartsUtil::blendColors(pointFillColor  , hideBg, hideAlpha);
    }

    //---

    QPen   pen;
    QBrush brush;

    plot()->setPenBrush(pen, brush,
      plot()->isSymbolStroked(), pointStrokeColor, plot()->symbolStrokeAlpha(),
      plot()->symbolStrokeWidth(), plot()->symbolStrokeDash(),
      plot()->isSymbolFilled(), pointFillColor, plot()->symbolFillAlpha(),
      plot()->symbolFillPattern());

    CQChartsPlotObj *obj = plotObj();

    if (obj)
      plot()->updateObjPenBrushState(obj, ig_, pen, brush, CQChartsPlot::DrawType::SYMBOL);

    //---

    CQChartsSymbol symbol = plot()->symbolType();

    double sx, sy;

    plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

    if (plot()->isLines() || plot()->isImpulseLines()) {
      double px = CMathUtil::avg(p1.x, p2.x);
      double py = CMathUtil::avg(p1.y, p2.y);

      plot()->drawSymbol(painter, QPointF(px, py), symbol, CMathUtil::avg(sx, sy), pen, brush);
    }
    else {
      double px = CMathUtil::avg(p1.x, p2.x);
      double py = CMathUtil::avg(p1.y, p2.y);

      plot()->drawSymbol(painter, QPointF(px, py), symbol, CMathUtil::avg(sx, sy), pen, brush);
    }
  }

  painter->restore();
}

CQChartsPlotObj *
CQChartsXYKeyLine::
plotObj() const
{
  if (ig_.n <= 1)
    return nullptr;

  CQChartsPlotObj *obj = plot()->getGroupObj(ig_.i);

  return obj;
}

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
