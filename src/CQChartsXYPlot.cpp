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
#include <CQUtil.h>
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
  return "<h2>Summary</h2>\n"
         "<p>Draws points at x and y coordinate pairs and connects them with a continuous "
         "line.</p>\n"
         "<p>The x coordinates should be monotonic.</p>\n"
         "<h2>Columns</h2>\n"
         "<p>The x and y values come from the values in the <b>X</b> and <b>Y</b> columns. "
         "Multiple <b>Y</b> columns can be specified to create a stack of lines.</p>\n"
         "<p>An optional <b>Name</b> column can be specified to supply a name for the "
         "coordinate.</p>\n"
         "<p>An optional <b>Size</b> column can be specified to supply the size of the symbol "
         "drawn at the point.</p>\n"
         "<p>An optional <b>PointLabel</b> column can be specified to add a label next to a "
         "point.</p>\n"
         "<p>An optional <b>PointColor</b> column can be specified to specify the color of "
         "the point symbol.</p>\n"
         "<p>An optional <b>PointSymbol</b> column can be specified to specify the symbol of "
         "the point.</p>\n"
         "<p>Optional <b>VectorX</b> and <b>VectorY</b> columns can be specified to draw a "
         "vector at the point.</p>\n"
         "<h2>Options</h2>\n"
         "<p>The <b>Lines</b> option determines whether the points are connected with a line. "
         "The default line style can be separately customized.</p>"
         "<p>The <b>Points</b> option determines whether the points are drawn. The default point "
         "symbol can be separately customized.</p>"
         "<p>Enabling the <b>Bivariate</b> option fills the area between adjacent sets of x, y "
         "coordinates (two or more y column values should be specified). The bivariate line "
         "style can be separately customized.</p>"
         "<p>Enabling the <b>Stacked</b> option stacks the y values on top of each other "
         "so the next set of y values adds onto the previous set of y values.</p>\n"
         "<p>Enabling the <b>Cumulative</b> option treats the y values as an increment "
         "from the previews y value (in each set).</p>\n"
         "<p>Enabling the <b>FillUnder</b> option fills the area under the plot. The "
         "fill under style (fill/stroke) can be separately customized.<p>\n"
         "<p>Enabling the <b>Impulse</b> option draws a line from zero to the "
         "points y value. The impulse line style can be separately customized.</p> "
         "<p>Enabling the <b>Fitted</b> option draws a best fit line between the points.<p>\n"
         "<p>The <b>Vectors</b> option detemines whether the vector specified by the "
         "<b>VectorX</b> and <b>VectorY</b> columns are drawn.</p>\n";
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
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "xColumn"           , "x"          );
  addProperty("columns", this, "yColumns"          , "y"          );
  addProperty("columns", this, "nameColumn"        , "name"       );
  addProperty("columns", this, "sizeColumn"        , "size"       );
  addProperty("columns", this, "pointLabelColumn"  , "pointLabel" );
  addProperty("columns", this, "pointColorColumn"  , "pointColor" );
  addProperty("columns", this, "pointSymbolColumn" , "pointSymbol");
  addProperty("columns", this, "vectorXColumn"     , "vectorX"    );
  addProperty("columns", this, "vectorYColumn"     , "vectorY"    );

  // bivariate
  addProperty("bivariate", this, "bivariateLines", "visible");

  addLineProperties("bivariate", "bivariateLines");

  // stacked
  addProperty("stacked", this, "stacked", "enabled");

  // cumulative
  addProperty("cumulative", this, "cumulative", "enabled");

  // points
  addProperty("points", this, "points", "visible");

  addSymbolProperties("points/symbol");

  // lines
  addProperty("lines", this, "lines"          , "visible"   );
  addProperty("lines", this, "linesSelectable", "selectable");
  addProperty("lines", this, "roundedLines"   , "rounded"   );
  addProperty("lines", this, "fitted"         , "fitted"    );

  addLineProperties("lines", "lines");

  // fill under
  addProperty("fillUnder", this, "fillUnderFilled"    , "visible"   );
  addProperty("fillUnder", this, "fillUnderSelectable", "selectable");
  addProperty("fillUnder", this, "fillUnderPos"       , "position"  );
  addProperty("fillUnder", this, "fillUnderSide"      , "side"      );

  addFillProperties("fillUnder", "fillUnderFill");

  // impulse
  addProperty("impulse", this, "impulseLines", "visible");

  addLineProperties("impulse", "impulseLines");

  // vectors
  addProperty("vectors", this     , "vectors"  , "visible"  );
  addProperty("vectors", arrowObj_, "length"   , "length"   );
  addProperty("vectors", arrowObj_, "angle"    , "angle"    );
  addProperty("vectors", arrowObj_, "backAngle", "backAngle");
  addProperty("vectors", arrowObj_, "fhead"    , "fhead"    );
  addProperty("vectors", arrowObj_, "thead"    , "thead"    );
  addProperty("vectors", arrowObj_, "empty"    , "empty"    );

  addProperty("vectors/stroke", arrowObj_, "border"     , "visible");
  addProperty("vectors/stroke", arrowObj_, "borderWidth", "width"  );
  addProperty("vectors/fill"  , arrowObj_, "filled"     , "visible");
  addProperty("vectors/fill"  , arrowObj_, "fillColor"  , "color"  );

  // data label
  addProperty("dataLabel", this, "dataLabelTextVisible", "visible");

  addTextProperties("dataLabel" , "dataLabelText");

  addProperty("dataLabel", this, "dataLabelTextAlign", "align");

  CQChartsGroupPlot::addProperties();
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
    arrowObj_->setVisible(b);

    updateObjs();
  }
}

void
CQChartsXYPlot::
setFitted(bool b)
{
  CQChartsUtil::testAndSet(fitted_, b, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsXYPlot::
setPointsSlot(bool b)
{
  setPoints(b);
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
  CQChartsUtil::testAndSet(linesSelectable_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsXYPlot::
setRoundedLines(bool b)
{
  CQChartsUtil::testAndSet(roundedLines_, b, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsXYPlot::
setFillUnderSelectable(bool b)
{
  CQChartsUtil::testAndSet(fillUnderData_.selectable, b, [&]() { invalidateLayers(); } );
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

QColor
CQChartsXYPlot::
interpPaletteColor(int i, int n, bool scale) const
{
  if (isOverlay()) {
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

    return CQChartsPlot::interpPaletteColor(i, n, scale);
  }
  else {
    return CQChartsPlot::interpPaletteColor(i, n, scale);
  }
}

//---

CQChartsGeom::Range
CQChartsXYPlot::
calcRange()
{
  CQPerfTrace trace("CQChartsXYPlot::calcRange");

  //---

  initGroupData(CQChartsColumns(), CQChartsColumn());

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsXYPlot *plot) :
     plot_(plot) {
      int ns = plot_->yColumns().count();

      sum_.resize(ns);
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
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

      if      (plot_->isBivariateLines()) {
        for (int i = 0; i < ny; ++i)
          range_.updateRange(x, y[i]);
      }
      else if (plot_->isStacked()) {
        double sum1 = 0.0;

        for (int i = 0; i < ny; ++i)
          sum1 += y[i];

        range_.updateRange(x, 0.0);
        range_.updateRange(x, sum1);
      }
      else {
        for (int i = 0; i < ny; ++i) {
          double y1 = y[i];

          if (plot_->isCumulative()) {
            y1 = y[i] + lastSum_[i];

            sum_[i] += y[i];
          }

          range_.updateRange(x, y1);
        }
      }

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    using Reals = std::vector<double>;

    CQChartsXYPlot*     plot_ { nullptr };
    CQChartsGeom::Range range_;
    Reals               sum_;
    Reals               lastSum_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  CQChartsGeom::Range dataRange = visitor.range();

  //---

  adjustDataRange();

  if (dataRange.isSet()) {
    if (CMathUtil::isZero(dataRange.dx())) {
      double xm = dataRange.xmid();

      dataRange.setLeft (xm - 1.0);
      dataRange.setRight(xm + 1.0);
    }

    if (CMathUtil::isZero(dataRange.dy())) {
      double ym = dataRange.ymid();

      dataRange.setBottom(ym - 1.0);
      dataRange.setTop   (ym + 1.0);
    }
  }

  //---

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
      xAxis()->setDate(true);
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

  //---

  return dataRange;
}

QString
CQChartsXYPlot::
xAxisName() const
{
  bool ok;

  return modelHeaderString(xColumn(), ok);
}

QString
CQChartsXYPlot::
yAxisName() const
{
  QString name;

  int ns = yColumns().count();

  if      (isBivariateLines() && ns > 1) {
    name = titleStr();

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
  }
  else if (isStacked()) {
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
  }

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

void
CQChartsXYPlot::
postInit()
{
  if      (isBivariateLines()) {
    setLines (true);
    setPoints(false);

    setLinesWidth(CQChartsLength("3px"));
  }
  else if (isStacked()) {
    setFillUnderFilled(true);
    setPoints         (false);
  }
  else {
    setLines (true);
    setPoints(false);

    setLinesWidth(CQChartsLength("3px"));
  }
}

bool
CQChartsXYPlot::
createObjs()
{
  CQPerfTrace trace("CQChartsXYPlot::createObjs");

  const CQChartsGeom::Range &dataRange = this->dataRange();

  // TODO: use actual symbol size
  symbolWidth_  = (dataRange.xmax() - dataRange.xmin())/100.0;
  symbolHeight_ = (dataRange.ymax() - dataRange.ymin())/100.0;

  //---

  if (pointColorColumn().isValid())
    pointColorColumnType_ = columnValueType(pointColorColumn());

  //---

  GroupSetIndPoly groupSetIndPoly;

  createGroupSetIndPoly(groupSetIndPoly);

  (void) createGroupSetObjs(groupSetIndPoly);

  //----

  resetKeyItems();

  //---

  return true;
}

void
CQChartsXYPlot::
createGroupSetIndPoly(GroupSetIndPoly &groupSetIndPoly)
{
  CQPerfTrace trace("CQChartsXYPlot::createGroupSetIndPoly");

  // create line per set
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsXYPlot *plot) :
     plot_(plot) {
      ns_ = plot_->yColumns().count();
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
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
    CQChartsXYPlot* plot_ { nullptr };
    int             ns_;
    GroupSetIndPoly groupSetPoly_;
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
createGroupSetObjs(const GroupSetIndPoly &groupSetIndPoly)
{
  CQPerfTrace trace("CQChartsXYPlot::createGroupSetObjs");

  const CQChartsGeom::Range &dataRange = this->dataRange();

  //---

  double sw = symbolWidth ();
  double sh = symbolHeight();

  //---

  int ig = 0;
  int ng = groupSetIndPoly.size();

  for (auto &p : groupSetIndPoly) {
    int               groupInd = p.first;
    const SetIndPoly &setPoly  = p.second;

    if      (isBivariateLines()) {
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
          double y2 = yVals[j];

          CQChartsGeom::BBox bbox(x - sw/2, y1 - sh/2, x + sw/2, y2 + sh/2);

          if (! isFillUnderFilled()) {
            // use vertical line object for each point pair if not fill under
            CQChartsXYBiLineObj *lineObj =
              new CQChartsXYBiLineObj(this, groupInd, bbox, x, y1, y2, xind1,
                                      j - 1, ny1 - 1, ip, np);

            addPlotObject(lineObj);
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

          addPolyLine(poly1, groupInd, ig, ng, j - 1, ns - 1, name1);
          addPolyLine(poly2, groupInd, ig, ng, j - 1, ns - 1, name1);

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

          addPolygon(poly1, groupInd, ig, ng, j - 1, ns - 1, name1);
        }
      }
    }
    else {
      // convert lines into set polygon and set poly lines (more than one if NaNs)
      int ns = yColumns().count();

      for (int j = 0; j < ns; ++j) {
        bool hidden = isSetHidden(j);

        if (hidden)
          continue;

        //---

        // get column name
        CQChartsColumn yColumn = yColumns().getColumn(j);

        bool ok;

        QString name = modelHeaderString(yColumn, ok);

        //---

        QPolygonF polyShape, polyLine;

        const QPolygonF &poly     = setPoly[j].poly;
        const QPolygonF &prevPoly = (j > 0 ? setPoly[j - 1].poly : poly);

        int np = poly.size();
        assert(prevPoly.size() == np);

        for (int ip = 0; ip < np; ++ip) {
          const QPointF &p = poly[ip];

          double x = p.x(), y = p.y();

          if (CMathUtil::isNaN(x) || CMathUtil::isInf(x) ||
              CMathUtil::isNaN(y) || CMathUtil::isInf(y)) {
            if (polyLine.count()) {
              addPolyLine(polyLine, groupInd, ig, ng, j, ns, name);

              polyLine = QPolygonF();
            }

            continue;
          }

          //---

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
          //QModelIndex parent; // TODO: parent

          QModelIndex xind  = modelIndex(ip, xColumn()); // TODO: parent
          QModelIndex xind1 = normalizeIndex(xind);

          CQChartsGeom::BBox bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

          CQChartsXYPointObj *pointObj =
            new CQChartsXYPointObj(this, groupInd, bbox, x, y, size, xind1, j, ns, ip, np);

          addPlotObject(pointObj);

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

              CQChartsXYLabelObj *labelObj =
                new CQChartsXYLabelObj(this, groupInd, bbox, x, y, label, xind1, j, ns, ip, np);

              addPlotObject(labelObj);
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
            double ys = std::min(y, 0.0);
            double ye = std::max(y, 0.0);

            CQChartsGeom::BBox bbox(x - sw/2, ys, x + sw/2, ye);

            CQChartsXYImpulseLineObj *impulseObj =
              new CQChartsXYImpulseLineObj(this, groupInd, bbox, x, ys, ye, xind1,
                                           j, ns, ip, np);

            addPlotObject(impulseObj);
          }

          //---

          // add point to poly line
          polyLine << p;

          //---

          // add point to polygon

          // if first point then add first point of previous polygon
          if (ip == 0) {
            if (isStacked()) {
              double y1 = (j > 0 ? prevPoly[ip].y() : dataRange.ymin());

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
              double y1 = (j > 0 ? prevPoly[ip].y() : dataRange.ymin());

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
          if (j > 0) {
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

        if (polyLine.count())
          addPolyLine(polyLine, groupInd, ig, ng, j, ns, name);

        //---

        addPolygon(polyShape, groupInd, ig, ng, j, ns, name);
      }
    }

    ++ig;
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

void
CQChartsXYPlot::
addPolyLine(const QPolygonF &polyLine, int groupInd, int ig, int ng, int is, int ns,
            const QString &name)
{
  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(polyLine.boundingRect());

  CQChartsXYPolylineObj *lineObj =
    new CQChartsXYPolylineObj(this, groupInd, bbox, polyLine, name, ig, ng, is, ns);

  addPlotObject(lineObj);
}

void
CQChartsXYPlot::
addPolygon(const QPolygonF &poly, int groupInd, int ig, int ng, int is, int ns,
           const QString &name)
{
  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(poly.boundingRect());

  CQChartsXYPolygonObj *polyObj =
    new CQChartsXYPolygonObj(this, groupInd, bbox, poly, name, ig, ng, is, ns);

  addPlotObject(polyObj);
}

QString
CQChartsXYPlot::
valueName(int iset, int irow) const
{
  QString name;

  if (iset >= 0) {
    CQChartsColumn yColumn = yColumns().getColumn(iset);

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
  int row, col;

  if (! key->isHorizontal()) {
    row = key->maxRow();
    col = 0;
  }
  else {
    row = 0;
    col = key->maxCol();
  }

  int ns = yColumns().count();

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

    CQChartsXYKeyColor *color = new CQChartsXYKeyColor(this, 0, 1);
    CQChartsXYKeyText  *text  = new CQChartsXYKeyText (this, name, 0, 1);

    key->addItem(color, row, col    );
    key->addItem(text , row, col + 1);
  }
  else if (isStacked()) {
    for (int i = 0; i < ns; ++i) {
      CQChartsColumn yColumn = yColumns().getColumn(i);

      bool ok;

      QString name = modelHeaderString(yColumn, ok);

      CQChartsXYKeyLine *line = new CQChartsXYKeyLine(this, i, ns);
      CQChartsXYKeyText *text = new CQChartsXYKeyText(this, name, i, ns);

      key->addItem(line, row + i, col    );
      key->addItem(text, row + i, col + 1);
    }
  }
  else {
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
      if (ns == 1 && ! isOverlay() && (titleStr().length() || fileName().length())) {
        if      (titleStr().length())
          name = titleStr();
        else if (fileName().length())
          name = fileName();
      }

      CQChartsXYKeyLine *line = new CQChartsXYKeyLine(this, i, ns);
      CQChartsXYKeyText *text = new CQChartsXYKeyText(this, name, i, ns);

      if (! key->isHorizontal()) {
        key->addItem(line, row + i, col    );
        key->addItem(text, row + i, col + 1);
      }
      else {
        key->addItem(line, row, col + 2*i    );
        key->addItem(text, row, col + 2*i + 1);
      }
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

bool
CQChartsXYPlot::
probe(ProbeData &probeData) const
{
  std::vector<double> yvals;

  if (! interpY(probeData.x, yvals))
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

bool
CQChartsXYPlot::
addMenuItems(QMenu *menu)
{
  int ns = yColumns().count();

  menu->addSeparator();

  //---

  if (ns > 1) {
    QAction *bivariateAction  = new QAction("Bivariate" , menu);

    bivariateAction->setCheckable(true);
    bivariateAction->setChecked(isBivariateLines());

    connect(bivariateAction, SIGNAL(triggered(bool)), this, SLOT(setBivariateLinesSlot(bool)));

    menu->addAction(bivariateAction);
  }

  //---

  QAction *pointsAction = new QAction("Points", menu);
  QAction *linesAction  = new QAction("Lines" , menu);

  pointsAction->setCheckable(true);
  pointsAction->setChecked(isPoints());

  linesAction->setCheckable(true);
  linesAction->setChecked(isLines());

  menu->addAction(pointsAction);
  menu->addAction(linesAction);

  menu->addSeparator();

  //---

  QAction *stackedAction    = new QAction("Stacked"   , menu);
  QAction *cumulativeAction = new QAction("Cumulative", menu);
  QAction *impulseAction    = new QAction("Impulse"   , menu);
  QAction *fillUnderAction  = new QAction("Fill Under", menu);

  stackedAction->setCheckable(true);
  stackedAction->setChecked(isStacked());

  cumulativeAction->setCheckable(true);
  cumulativeAction->setChecked(isCumulative());

  impulseAction->setCheckable(true);
  impulseAction->setChecked(isImpulseLines());

  fillUnderAction->setCheckable(true);
  fillUnderAction->setChecked(isFillUnderFilled());

  connect(pointsAction    , SIGNAL(triggered(bool)), this, SLOT(setPointsSlot(bool)));
  connect(linesAction     , SIGNAL(triggered(bool)), this, SLOT(setLinesSlot(bool)));
  connect(stackedAction   , SIGNAL(triggered(bool)), this, SLOT(setStacked(bool)));
  connect(cumulativeAction, SIGNAL(triggered(bool)), this, SLOT(setCumulative(bool)));
  connect(impulseAction   , SIGNAL(triggered(bool)), this, SLOT(setImpulseLinesSlot(bool)));
  connect(fillUnderAction , SIGNAL(triggered(bool)), this, SLOT(setFillUnderFilledSlot(bool)));

  menu->addAction(stackedAction);
  menu->addAction(cumulativeAction);
  menu->addAction(impulseAction);
  menu->addAction(fillUnderAction);

  return true;
}

//------

void
CQChartsXYPlot::
drawArrow(QPainter *painter, const QPointF &p1, const QPointF &p2)
{
  arrowObj_->setFrom(p1);
  arrowObj_->setTo  (p2);

  arrowObj_->draw(painter);
}

//------

CQChartsXYBiLineObj::
CQChartsXYBiLineObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                    double x, double y1, double y2, const QModelIndex &ind,
                    int iset, int nset, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), x_(x), y1_(y1), y2_(y2),
 ind_(ind), iset_(iset), nset_(nset), i_(i), n_(n)
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

  return QString("biline:%1:%2").arg(iset()).arg(i());
}

QString
CQChartsXYBiLineObj::
calcTipId() const
{
  QString name  = plot()->valueName(-1, ind().row());
  QString xstr  = plot()->xStr(x());
  QString y1str = plot()->yStr(y1());
  QString y2str = plot()->yStr(y2());

  CQChartsTableTip tableTip;

  if (name.length())
    tableTip.addTableRow("Name", name);

  tableTip.addTableRow("X" , xstr );
  tableTip.addTableRow("Y1", y1str);
  tableTip.addTableRow("Y2", y2str);

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

  double px, py1, py2;

  plot()->windowToPixel(x(), y1(), px, py1);
  plot()->windowToPixel(x(), y2(), px, py2);

  double sx, sy;

  plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(px - sx, py1 - sy, px + sx, py2 + sy);

  CQChartsGeom::Point pp;

  plot()->windowToPixel(p, pp);

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

  double px, py1, py2;

  plot()->windowToPixel(x(), y1(), px, py1);
  plot()->windowToPixel(x(), y2(), px, py2);

  if (plot()->isLines()) {
    QPen   pen;
    QBrush brush;

    QColor lc = plot()->interpFillUnderFillColor(iset(), nset());

    plot()->setPen  (pen, true, lc, 1.0, CQChartsLength("0px"), CQChartsLineDash());
    plot()->setBrush(brush, false);

    plot()->updateObjPenBrushState(this, pen, brush);

    //--

    painter->drawLine(QPointF(px, py1), QPointF(px, py2));
  }

  if (plot()->isPoints()) {
    CQChartsSymbol symbol      = plot()->symbolType();
    bool           stroked     = plot()->isSymbolStroked();
    QColor         strokeColor = plot()->interpSymbolStrokeColor(iset(), nset());
    bool           filled      = plot()->isSymbolFilled();
    QColor         fillColor   = plot()->interpSymbolFillColor(iset(), nset());

    double sx, sy;

    plot()->pixelSymbolSize(plot()->symbolSize(), sx, sy);

    //---

    QPen   pen;
    QBrush brush;

    plot_->setPenBrush(pen, brush,
      stroked, strokeColor, 1.0, plot()->symbolStrokeWidth(), plot()->symbolStrokeDash(),
      filled, fillColor, 1.0, plot()->symbolFillPattern());

    plot_->updateObjPenBrushState(this, pen, brush);

    //---

    plot()->drawSymbol(painter, QPointF(px, py1), symbol, CMathUtil::avg(sx, sy), pen, brush);
    plot()->drawSymbol(painter, QPointF(px, py2), symbol, CMathUtil::avg(sx, sy), pen, brush);
  }
}

//------

CQChartsXYImpulseLineObj::
CQChartsXYImpulseLineObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                         double x, double y1, double y2, const QModelIndex &ind,
                         int iset, int nset, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), x_(x), y1_(y1), y2_(y2),
 ind_(ind), iset_(iset), nset_(nset), i_(i), n_(n)
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

  return QString("impulse:%1:%2").arg(iset()).arg(i());
}

QString
CQChartsXYImpulseLineObj::
calcTipId() const
{
  QString name  = plot()->valueName(iset(), ind().row());
  QString xstr  = plot()->xStr(x());
  QString y1str = plot()->yStr(y1());
  QString y2str = plot()->yStr(y2());

  CQChartsTableTip tableTip;

  if (name.length())
    tableTip.addTableRow("Name", name);

  tableTip.addTableRow("X" , xstr );
  tableTip.addTableRow("Y1", y1str);
  tableTip.addTableRow("Y2", y2str);

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

  double px1, py1, px2, py2;

  plot()->windowToPixel(x(), y1(), px1, py1);
  plot()->windowToPixel(x(), y2(), px2, py2);

  double b = 2;

  double lw = std::max(plot()->lengthPixelWidth(plot()->impulseLinesWidth()), 2*b);

  CQChartsGeom::BBox pbbox(px1 - lw/2, py1 - b, px2 + lw/2, py2 + b);

  CQChartsGeom::Point pp;

  plot()->windowToPixel(p, pp);

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

  int ic = (nset() > 1 ? iset() : i());
  int nc = (nset() > 1 ? nset() : n());

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = plot()->interpImpulseLinesColor(ic, nc);

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

  double px1, py1, px2, py2;

  plot()->windowToPixel(x(), y1(), px1, py1);
  plot()->windowToPixel(x(), y2(), px2, py2);

  if (lw <= 1) {
    painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));
  }
  else {
    QRectF qrect(px1 - lw/2, py1, lw, py2 - py1);

    CQChartsRoundedPolygon::draw(painter, qrect, 0, 0);
  }
}

//------

CQChartsXYPointObj::
CQChartsXYPointObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                   double x, double y, double size, const QModelIndex &ind,
                   int iset, int nset, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), pos_(x, y), size_(size),
 ind_(ind), iset_(iset), nset_(nset), i_(i), n_(n)
{
}

CQChartsXYPointObj::
~CQChartsXYPointObj()
{
  delete edata_;
}

QString
CQChartsXYPointObj::
calcId() const
{
  QModelIndex ind1 = plot()->unnormalizeIndex(ind());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("point:%1:%2").arg(iset()).arg(i());
}

QString
CQChartsXYPointObj::
calcTipId() const
{
  if (plot()->tipColumn().isValid()) {
    QModelIndex tipInd = plot()->modelIndex(ind().row(), plot()->tipColumn(), ind().parent());

    QModelIndex tipInd1 = plot()->unnormalizeIndex(tipInd);

    bool ok;

    QString tipStr = plot()->modelString(tipInd1.row(), plot()->tipColumn(), tipInd1.parent(), ok);

    if (ok)
      return tipStr;
  }

  //---

  CQChartsTableTip tableTip;

  QModelIndex ind1 = plot()->unnormalizeIndex(ind());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    tableTip.addTableRow("Id", idStr);

  QString name = plot()->valueName(iset(), ind().row());
  QString xstr = plot()->xStr(x());
  QString ystr = plot()->yStr(y());

  if (name.length())
    tableTip.addTableRow("Name", name);

  tableTip.addTableRow("X", xstr);
  tableTip.addTableRow("Y", ystr);

  if (size_ > 0)
    tableTip.addTableRow("Size", size_);

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

  double px, py;

  plot()->windowToPixel(x(), y(), px, py);

  double sx = size();
  double sy = sx;

  if (sx <= 0)
    plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(px - sx, py - sy, px + sx, py + sy);

  CQChartsGeom::Point pp;

  plot()->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYPointObj::
getSelectIndices(Indices &inds) const
{
  if (! visible())
    return;

  addColumnSelectIndex(inds, plot()->xColumn());
  addColumnSelectIndex(inds, plot()->yColumns().getColumn(iset()));
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

  // set pen/brush
  QPen   pen;
  QBrush brush;

  plot()->setSymbolPenBrush(pen, brush, iset(), nset());

  if (edata_ && edata_->color.isValid()) {
    QColor strokeColor = edata_->color.interpColor(plot_->charts(), 0, 1);

    pen.setColor(strokeColor);
  }

  plot()->updateObjPenBrushState(this, pen, brush);

  //---

  // draw symbol
  CQChartsSymbol symbol = plot()->symbolType();

  if (edata_ && edata_->symbol.type() != CQChartsSymbol::Type::NONE)
    symbol = edata_->symbol;

  double sx = size();
  double sy = sx;

  if (sx <= 0)
    plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  CQChartsGeom::Point pp = CQChartsUtil::fromQPoint(pos_);

  double px, py;

  plot()->windowToPixel(pp.x, pp.y, px, py);

  plot()->drawSymbol(painter, QPointF(px, py), symbol, CMathUtil::avg(sx, sy), pen, brush);

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
CQChartsXYLabelObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                   double x, double y, const QString &label, const QModelIndex &ind,
                   int iset, int nset, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), pos_(x, y), label_(label),
 ind_(ind), iset_(iset), nset_(nset), i_(i), n_(n)
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

  return QString("label:%1:%2").arg(iset()).arg(i());
}

QString
CQChartsXYLabelObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Label", label_);

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
  addColumnSelectIndex(inds, plot()->yColumns().getColumn(iset()));
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

  QColor tc = plot()->interpDataLabelTextColor(0, 1);

  plot()->setPen(tpen, true, tc, plot()->dataLabelTextAlpha());

  painter->setPen(tpen);

  //--

  // draw text
  CQChartsGeom::Point ppos = plot()->windowToPixel(CQChartsUtil::fromQPoint(pos_));

  plot()->view()->setPlotPainterFont(plot(), painter, plot()->dataLabelTextFont());

  CQChartsRotatedText::drawRotatedText(painter, ppos.x, ppos.y, label_,
                                       plot()->dataLabelTextAngle(),
                                       plot()->dataLabelTextAlign());
}

//------

CQChartsXYPolylineObj::
CQChartsXYPolylineObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                      const QPolygonF &poly, const QString &name, int ig, int ng, int is, int ns) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), poly_(poly), name_(name),
 ig_(ig), ng_(ng), is_(is), ns_(ns)
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
  return QString("polyline:%1:%2").arg(ig()).arg(is());
}

QString
CQChartsXYPolylineObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name());

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

  double px, py;

  plot()->windowToPixel(p.x, p.y, px, py);

  CQChartsGeom::Point pp(px, py);

  for (int i = 1; i < poly_.count(); ++i) {
    double x1 = poly_[i - 1].x();
    double y1 = poly_[i - 1].y();
    double x2 = poly_[i    ].x();
    double y2 = poly_[i    ].y();

    double px1, py1, px2, py2;

    plot()->windowToPixel(x1, y1, px1, py1);
    plot()->windowToPixel(x2, y2, px2, py2);

    CQChartsGeom::Point pl1(px1, py1);
    CQChartsGeom::Point pl2(px2, py2);

    double d;

    if (CQChartsUtil::PointLineDistance(pp, pl1, pl2, &d) && d < 1)
      return true;
  }

  return false;
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

void
CQChartsXYPolylineObj::
getSelectIndices(Indices &) const
{
  if (! visible())
    return;

  // all objects part of line (dont support select)
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
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  if (plot_->isFitted()) {
    if (! fit_.isFitted()) {
      fit_.calc(poly_);
    }
  }

  //---

  int ic = (ng() > 1 ? ig() : is());
  int nc = (ng() > 1 ? ng() : ns());

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor c;

  if      (plot()->isBivariateLines())
    c = plot()->interpThemeColor(1);
  else if (plot()->isStacked())
    c = plot()->interpLinesColor(ic, nc);
  else
    c = plot()->interpLinesColor(ic, nc);

  if (! isInside())
    plot_->setPen(pen, true, c, plot()->linesAlpha(), plot()->linesWidth(), plot_->linesDash());
  else
    plot_->setPen(pen, true, c, plot()->linesAlpha(), CQChartsLength("3px"), plot_->linesDash());

  plot_->setBrush(brush, false);

  //---

  plot()->updateObjPenBrushState(this, pen, brush);

  //---

  // draw lines
  if (plot()->isRoundedLines()) {
    initSmooth();

    QPainterPath path;

    for (int i = 0; i < smooth_->numPoints(); ++i) {
      const CQChartsGeom::Point &p = smooth_->point(i);

      double px, py;

      plot()->windowToPixel(p.x, p.y, px, py);

      if (i == 0)
        path.moveTo(px, py);
      else {
        CQChartsSmooth::SegmentType type = smooth_->segmentType(i - 1);

        if      (type == CQChartsSmooth::SegmentType::CURVE3) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);
          CQChartsGeom::Point c2 = smooth_->controlPoint2(i - 1);

          double pcx1, pcy1, pcx2, pcy2;

          plot()->windowToPixel(c1.x, c1.y, pcx1, pcy1);
          plot()->windowToPixel(c2.x, c2.y, pcx2, pcy2);

          path.cubicTo(pcx1, pcy1, pcx2, pcy2, px, py);
        }
        else if (type == CQChartsSmooth::SegmentType::CURVE2) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);

          double pcx1, pcy1;

          plot()->windowToPixel(c1.x, c1.y, pcx1, pcy1);

          path.quadTo(pcx1, pcy1, px, py);
        }
        else if (type == CQChartsSmooth::SegmentType::LINE) {
          path.lineTo(px, py);
        }
      }
    }

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPath(path);
  }
  else {
    painter->setPen  (pen);
    painter->setBrush(brush);

    int np = poly_.count();

    for (int i = 1; i < np; ++i)
      painter->drawLine(plot()->windowToPixel(poly_[i - 1]), plot()->windowToPixel(poly_[i]));
  }

  //---

  if (plot_->isFitted()) {
    QPolygonF poly;

    double pxl, pyl, pxr, pyr;

    plot_->windowToPixel(fit_.xmin(), 0, pxl, pyl);
    plot_->windowToPixel(fit_.xmax(), 0, pxr, pyr);

    for (int px = pxl; px <= pxr; ++px) {
      double x, y;

      plot_->pixelToWindow(px, 0.0, x, y);

      double y2 = fit_.interp(x);

      double px2, py2;

      plot_->windowToPixel(x, y2, px2, py2);

      poly << QPointF(px2, py2);
    }

    //---

    if (poly.size()) {
      QPainterPath path;

      const QPointF &p = poly[0];

      path.moveTo(p);

      for (int i = 1; i < poly.size(); ++i) {
        const QPointF &p = poly[i];

        path.lineTo(p);
      }

      painter->strokePath(path, pen);
    }
  }
}

//------

CQChartsXYPolygonObj::
CQChartsXYPolygonObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                     const QPolygonF &poly, const QString &name, int ig, int ng, int is, int ns) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), poly_(poly), name_(name),
 ig_(ig), ng_(ng), is_(is), ns_(ns)
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
  return QString("polygon:%1:%2").arg(ig()).arg(is());
}

QString
CQChartsXYPolygonObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name());
  tableTip.addTableRow("Area", CQUtil::polygonArea(poly_));

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

void
CQChartsXYPolygonObj::
getSelectIndices(Indices &) const
{
  if (! visible())
    return;

  // all objects part of polygon (dont support select)
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

  int ic = (ng() > 1 ? ig() : is());
  int nc = (ng() > 1 ? ng() : ns());

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor fillColor = plot()->interpFillUnderFillColor(ic, nc);

  plot()->setPen(pen, false);

  plot()->setBrush(brush, true, fillColor, plot()->fillUnderFillAlpha(),
                   plot()->fillUnderFillPattern());

  plot()->updateObjPenBrushState(this, pen, brush);

  //---

  int np = poly_.count();

  if (plot()->isRoundedLines()) {
    initSmooth();

    QPainterPath path;

    if (np > 0) {
      double px, py;

      plot()->windowToPixel(poly_[0].x(), poly_[0].y(), px, py);

      path.moveTo(px, py);
    }

    for (int i = 0; i < smooth_->numPoints(); ++i) {
      const CQChartsGeom::Point &p = smooth_->point(i);

      double px, py;

      plot()->windowToPixel(p.x, p.y, px, py);

      if (i == 0)
        path.lineTo(px, py);
      else {
        CQChartsSmooth::SegmentType type = smooth_->segmentType(i - 1);

        if      (type == CQChartsSmooth::SegmentType::CURVE3) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);
          CQChartsGeom::Point c2 = smooth_->controlPoint2(i - 1);

          double pcx1, pcy1, pcx2, pcy2;

          plot()->windowToPixel(c1.x, c1.y, pcx1, pcy1);
          plot()->windowToPixel(c2.x, c2.y, pcx2, pcy2);

          path.cubicTo(pcx1, pcy1, pcx2, pcy2, px, py);
        }
        else if (type == CQChartsSmooth::SegmentType::CURVE2) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);

          double pcx1, pcy1;

          plot()->windowToPixel(c1.x, c1.y, pcx1, pcy1);

          path.quadTo(pcx1, pcy1, px, py);
        }
        else if (type == CQChartsSmooth::SegmentType::LINE) {
          path.lineTo(px, py);
        }
      }
    }

    if (np > 0) {
      double px, py;

      plot()->windowToPixel(poly_[np - 1].x(), poly_[np - 1].y(), px, py);

      path.lineTo(px, py);
    }

    path.closeSubpath();

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPath(path);
  }
  else {
    QPolygonF poly;

    for (int i = 0; i < np; ++i) {
      double px, py;

      plot()->windowToPixel(poly_[i].x(), poly_[i].y(), px, py);

      poly << QPointF(px, py);
    }

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPolygon(poly);
  }
}

//------

CQChartsXYKeyColor::
CQChartsXYKeyColor(CQChartsXYPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsXYKeyColor::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsXYKeyColor::
fillBrush() const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  QBrush brush;

  QColor              c;
  double              alpha   = 1.0;
  CQChartsFillPattern pattern = CQChartsFillPattern::Type::SOLID;

  if      (plot->isBivariateLines()) {
    c = plot->interpFillUnderFillColor(i_, n_);

    alpha = plot->fillUnderFillAlpha();

    pattern = plot->fillUnderFillPattern();
  }
  else if (plot_->isOverlay()) {
    if (plot->prevPlot() || plot->nextPlot()) {
      c = plot->interpLinesColor(i_, n_);

      alpha = plot->linesAlpha();
    }
    else
      c = CQChartsKeyColorBox::fillBrush().color();
  }
  else
    c = CQChartsKeyColorBox::fillBrush().color();

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  plot->setBrush(brush, true, c, alpha, pattern);

  return brush;
}

//------

CQChartsXYKeyLine::
CQChartsXYKeyLine(CQChartsXYPlot *plot, int i, int n) :
 CQChartsKeyItem(plot->key()), plot_(plot), i_(i), n_(n)
{
}

QSizeF
CQChartsXYKeyLine::
size() const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  CQChartsXYPlot *keyPlot = qobject_cast<CQChartsXYPlot *>(key_->plot());

  if (! keyPlot)
    keyPlot = plot;

  QFont font = plot->view()->plotFont(plot, plot->view()->font());

  QFontMetricsF fm(font);

  double w = fm.width("-X-");
  double h = fm.height();

  double ww = keyPlot->pixelToWindowWidth (w + 8);
  double wh = keyPlot->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

bool
CQChartsXYKeyLine::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

void
CQChartsXYKeyLine::
draw(QPainter *painter, const CQChartsGeom::BBox &rect)
{
  painter->save();

  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  CQChartsPlot *keyPlot = qobject_cast<CQChartsPlot *>(key_->plot());

  //CQChartsXYPlot *xyKeyPlot = qobject_cast<CQChartsXYPlot *>(keyPlot);
  //if (! xyKeyPlot) xyKeyPlot = plot;

  CQChartsGeom::BBox prect;

  keyPlot->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  painter->setClipRect(prect1);

  QColor pointStrokeColor = plot->interpSymbolStrokeColor(i_, n_);
  QColor pointFillColor   = plot->interpSymbolFillColor  (i_, n_);
  QColor lineColor        = plot->interpLinesColor       (i_, n_);
  QColor impulseColor     = plot->interpImpulseLinesColor(i_, n_);

  if (plot->isSetHidden(i_)) {
    QColor bg = key_->interpBgColor();
    double a  = key_->hiddenAlpha();

    pointStrokeColor = CQChartsUtil::blendColors(pointStrokeColor, bg, a);
    pointFillColor   = CQChartsUtil::blendColors(pointFillColor  , bg, a);
    lineColor        = CQChartsUtil::blendColors(lineColor       , bg, a);
    impulseColor     = CQChartsUtil::blendColors(impulseColor    , bg, a);
  }

  QPen linePen;
  QPen impulsePen;

  plot->setPen(linePen, true, lineColor, plot->linesAlpha(),
               plot->linesWidth(), plot->linesDash());
  plot->setPen(impulsePen, true, impulseColor, plot->impulseLinesAlpha(),
               plot->impulseLinesWidth(), plot->impulseLinesDash());

  if (plot->isFillUnderFilled()) {
    QBrush fillBrush;

    QColor fillColor = plot->interpFillUnderFillColor(i_, n_);

    plot->setBrush(fillBrush, true, fillColor, plot->fillUnderFillAlpha(),
                   plot->fillUnderFillPattern());

    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y1 = prect.getYMin() + 4;
    double y2 = prect.getYMax() - 4;

    if (isInside())
      fillBrush.setColor(plot->insideColor(fillBrush.color()));

    painter->fillRect(CQChartsUtil::toQRect(CQChartsGeom::BBox(x1, y1, x2, y2)), fillBrush);
  }

  if (plot->isLines() || plot->isImpulseLines()) {
    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y  = prect.getYMid();

    if (! plot->isLines())
      linePen = impulsePen;

    if (isInside())
      linePen = plot->insideColor(linePen.color());

    painter->setPen(linePen);

    painter->drawLine(QPointF(x1, y), QPointF(x2, y));
  }

  if (plot->isPoints()) {
    double dx = keyPlot->pixelToWindowWidth(4);

    double x1 = rect.getXMin() + dx;
    double x2 = rect.getXMax() - dx;
    double y  = rect.getYMid();

    double px1, px2, py;

    keyPlot->windowToPixel(x1, y, px1, py);
    keyPlot->windowToPixel(x2, y, px2, py);

    //---

    QPen   pen;
    QBrush brush;

    plot->setPenBrush(pen, brush,
      plot->isSymbolStroked(), pointStrokeColor, plot->symbolStrokeAlpha(),
      plot->symbolStrokeWidth(), plot->symbolStrokeDash(),
      plot->isSymbolFilled(), pointFillColor, plot->symbolFillAlpha(),
      plot->symbolFillPattern());

    //---

    CQChartsSymbol symbol = plot->symbolType();

    double sx, sy;

    plot->pixelSymbolSize(plot->symbolSize(), sx, sy);

    if (plot->isLines() || plot->isImpulseLines()) {
      double px = CMathUtil::avg(px1, px2);

      plot_->drawSymbol(painter, QPointF(px, py), symbol, CMathUtil::avg(sx, sy), pen, brush);
    }
    else {
      double px = CMathUtil::avg(px1, px2);

      plot_->drawSymbol(painter, QPointF(px, py), symbol, CMathUtil::avg(sx, sy), pen, brush);
    }
  }

  painter->restore();
}

//------

CQChartsXYKeyText::
CQChartsXYKeyText(CQChartsXYPlot *plot, const QString &text, int i, int n) :
 CQChartsKeyText(plot, text, i, n)
{
}

QColor
CQChartsXYKeyText::
interpTextColor(int i, int n) const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), key_->hiddenAlpha());

  return c;
}
