#include <CQChartsParallelPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQChartsTip.h>
#include <CQCharts.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CMathRound.h>

#include <QApplication>
#include <QMenu>
#include <QVBoxLayout>

CQChartsParallelPlotType::
CQChartsParallelPlotType()
{
}

void
CQChartsParallelPlotType::
addParameters()
{
  startParameterGroup("Parallel");

  // columns
  addColumnParameter("x", "X", "xColumn").
    setUnique().setPropPath("columns.x").setTip("X value column");
  addColumnsParameter("y", "Y", "yColumns").
    setNumericColumn().setRequired().setPropPath("columns.y").setTip("Y value columns");

  addEnumParameter("orientation", "Orientation", "orientation").
    addNameValue("HORIZONTAL", static_cast<int>(Qt::Horizontal)).
    addNameValue("VERTICAL"  , static_cast<int>(Qt::Vertical  )).
    setTip("Draw orientation");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsParallelPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
    h2("Parallel Plot").
     h3("Summary").
      p("Draws lines through values of multiple column values for each row.").
     h3("Columns").
      p("An axis is drawn for each y column using a label from the x column").
     h3("Limitations").
      p("None.").
     h3("Example").
      p(IMG("images/parallelplot.png"));
}

void
CQChartsParallelPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  using UniqueColumns = std::map<int, Column>;

  UniqueColumns xColumns;
  Columns       yColumns;

  for (int c = 0; c < details->numColumns(); ++c) {
    const auto *columnDetails = details->columnDetails(Column(c));
    if (! columnDetails) continue;

    if      (columnDetails->isNumeric()) {
      yColumns.addColumn(columnDetails->column());
    }
    else if (columnDetails->type() == ColumnType::STRING) {
      int n = columnDetails->numUnique();

      auto p = xColumns.find(-n);

      if (p == xColumns.end())
        xColumns[-n] = columnDetails->column();
    }
  }

  if (! xColumns.empty())
    analyzeModelData.parameterNameColumn["x"] = (*xColumns.begin()).second;

  if (yColumns.count())
    analyzeModelData.parameterNameColumns["y"] = yColumns;
}

CQChartsPlot *
CQChartsParallelPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsParallelPlot(view, model);
}

//---

CQChartsParallelPlot::
CQChartsParallelPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("parallel"), model),
 CQChartsObjLineData <CQChartsParallelPlot>(this),
 CQChartsObjPointData<CQChartsParallelPlot>(this)
{
}

CQChartsParallelPlot::
~CQChartsParallelPlot()
{
  CQChartsParallelPlot::term();
}

//---

void
CQChartsParallelPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  setLinesColor(Color::makePalette());

  setPoints(true);

  setSymbol(Symbol::circle());

  setSymbolStrokeAlpha(Alpha(0.25));
  setSymbolFilled     (true);
  setSymbolFillColor  (Color::makePalette());
  setSymbolFillAlpha  (Alpha(0.5));

  mainXAxis_ = std::make_unique<CQChartsAxis>(this, Qt::Horizontal, 0.0, 1.0);
  mainYAxis_ = std::make_unique<CQChartsAxis>(this, Qt::Vertical  , 0.0, 1.0);

  //--

  //addKey(); TODO

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsParallelPlot::
term()
{
}

//---

void
CQChartsParallelPlot::
setXColumn(const Column &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsParallelPlot::
setYColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(yColumns_, c, [&]() {
    visibleYColumns_ = yColumns_;

    resetYColumnVisible();

    updateRangeAndObjs();

    Q_EMIT customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsParallelPlot::
getNamedColumn(const QString &name) const
{
  Column c;

  if (name == "x") c = this->xColumn();
  else             c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsParallelPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if (name == "x") this->setXColumn(c);
  else             CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsParallelPlot::
getNamedColumns(const QString &name) const
{
  Columns c;

  if (name == "y") c = this->yColumns();
  else             c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsParallelPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "y") this->setYColumns(c);
  else             CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsParallelPlot::
setHorizontal(bool b)
{
  setOrientation(b ? Qt::Horizontal : Qt::Vertical);
}

void
CQChartsParallelPlot::
setOrientation(const Qt::Orientation &orientation)
{
  CQChartsUtil::testAndSet(orientation_, orientation, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsParallelPlot::
setNormalized(bool b)
{
  CQChartsUtil::testAndSet(normalized_, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsParallelPlot::
setLinesSelectable(bool b)
{
  CQChartsUtil::testAndSet(linesSelectable_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsParallelPlot::
setAxisLabelPos(const AxisLabelPos &p)
{
  CQChartsUtil::testAndSet(axisLabelPos_, p, [&]() { drawObjs(); } );
}

void
CQChartsParallelPlot::
setAxisLocal(bool b)
{
  CQChartsUtil::testAndSet(axisLocal_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsParallelPlot::
setAxisSpread(bool b)
{
  CQChartsUtil::testAndSet(axisSpread_, b, [&]() { updateRangeAndObjs(); } );
}

//------

void
CQChartsParallelPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "xColumn" , "x", "X column");
  addProp("columns", "yColumns", "y", "Y columns");

  // options
  addProp("options", "orientation", "", "Draw horizontal or vertical");
  addProp("options", "normalized" , "", "Is data normalized");

  // points
  addProp("points", "points", "visible", "Points visible");

  addSymbolProperties("points/symbol", "", "Points");

  // lines
  addProp("lines", "lines"          , "visible"   , "Lines visible");
  addProp("lines", "linesSelectable", "selectable", "Lines selectable");

  addLineProperties("lines/stroke", "lines", "");

  //---

  // axes
  auto addAxisProp = [&](const QString &path, const QString &name, const QString &alias,
                         const QString &desc, bool hidden=false) {
    auto *item = propertyModel()->addProperty(path, mainYAxis_.get(), name, alias);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addAxisStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                              const QString &desc, bool hidden=false) {
    auto *item = addAxisProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  addProp("axis", "axisLabelPos", "labelPos", "Axis label position");
  addProp("axis", "axisLocal"   , "local"   , "Local axis for each range");
  addProp("axis", "axisSpread"  , "spread"  , "Spread axis to global range");

  auto linePath = QString("axis/stroke");

  addAxisStyleProp(linePath, "axesLineData"  , "style"  , "Axis stroke style", true);
  addAxisStyleProp(linePath, "axesLines"     , "visible", "Axis stroke visible");
  addAxisStyleProp(linePath, "axesLinesColor", "color"  , "Axis stroke color");
  addAxisStyleProp(linePath, "axesLinesAlpha", "alpha"  , "Axis stroke alpha");
  addAxisStyleProp(linePath, "axesLinesWidth", "width"  , "Axis stroke width");
  addAxisStyleProp(linePath, "axesLinesDash" , "dash"   , "Axis stroke dash");
  addAxisStyleProp(linePath, "axesLinesCap"  , "cap"    , "Axis stroke cap");
//addAxisStyleProp(linePath, "axesLinesJoin" , "join"   , "Axis stroke join");

  addAxisProp("axis/grid", "gridLinesDisplayed", "visible", "Axis grid visible");

  //---

  auto ticksPath = QString("axis/ticks");

  addAxisProp(ticksPath, "ticksDisplayed", "lines", "Axis major and/or minor ticks visible");

  auto majorTicksPath = ticksPath + "/major";
  auto minorTicksPath = ticksPath + "/minor";

  addAxisProp(majorTicksPath, "majorTickLen", "length", "Axis major ticks pixel length");
  addAxisProp(minorTicksPath, "minorTickLen", "length", "Axis minor ticks pixel length");

  //---

  auto ticksLabelPath     = ticksPath + "/label";
  auto ticksLabelTextPath = ticksLabelPath + "/text";

  addAxisProp(ticksLabelPath, "tickLabelAutoHide" , "autoHide",
              "Axis tick label text is auto hide");
  addAxisProp(ticksLabelPath, "tickLabelPlacement", "placement",
              "Axis tick label text placement");

  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextData"         , "style",
                   "Axis tick label text style", true);
  addAxisProp     (ticksLabelTextPath, "axesTickLabelTextVisible"      , "visible",
                   "Axis tick label text visible");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextColor"        , "color",
                   "Axis tick label text color");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextAlpha"        , "alpha",
                   "Axis tick label text alpha");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextFont"         , "font",
                   "Axis tick label text font");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextAngle"        , "angle",
                   "Axis tick label text angle");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextContrast"     , "contrast",
                   "Axis tick label text contrast");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextContrastAlpha", "contrastAlpha",
                   "Axis tick label text contrast alpha");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextClipLength"   , "clipLength",
                   "Axis tick label text clip length");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextClipElide"    , "clipElide",
                   "Axis tick label text clip elide");

  //---

  auto labelPath     = QString("axis/label");
  auto labelTextPath = labelPath + "/text";

  addAxisStyleProp(labelTextPath, "axesLabelTextData"         , "style",
                   "Axis label text style", true);
  addAxisProp     (labelTextPath, "axesLabelTextVisible"      , "visible",
                   "Axis label text visible");
  addAxisStyleProp(labelTextPath, "axesLabelTextColor"        , "color",
                   "Axis label text color");
  addAxisStyleProp(labelTextPath, "axesLabelTextAlpha"        , "alpha",
                   "Axis label text alpha");
  addAxisStyleProp(labelTextPath, "axesLabelTextFont"         , "font",
                   "Axis label text font");
  addAxisStyleProp(labelTextPath, "axesLabelTextContrast"     , "contrast",
                   "Axis label text contrast");
  addAxisStyleProp(labelTextPath, "axesLabelTextContrastAlpha", "contrastAlpha",
                   "Axis label text contrast alpha");
  addAxisStyleProp(labelTextPath, "axesLabelTextClipLength"   , "clipLength",
                   "Axis label text clip length");
  addAxisStyleProp(labelTextPath, "axesLabelTextClipElide"    , "clipElide",
                   "Axis label text clip elide");

//mainYAxis_->addProperties(propertyModel(), "axis");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

void
CQChartsParallelPlot::
initRange()
{
  if (! currentModelData())
    axes_.clear();
}

CQChartsGeom::Range
CQChartsParallelPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsParallelPlot::calcRange");

  //---

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsParallelPlot *>(this);

  th->clearErrors();

  th->updateVisibleYColumns();

  //---

  // update axes to match columns
  th->updateAxes();

  //---

  // check columns
  bool columnsValid = true;

  if (! checkColumn(xColumn(), "X"))
    columnsValid = false;
  if (! checkColumns(visibleYColumns(), "Y", /*required*/true))
    columnsValid = false;

  if (! columnsValid) {
    auto dataRange = Range(0.0, 0.0, 1.0, 1.0);

    th->dataRange_           = dataRange;
    th->normalizedDataRange_ = dataRange;

    return dataRange;
  }

  //---

  // calc range for each value column (set)
  class RowVisitor : public ModelVisitor {
   public:
    using ParallelPlot = CQChartsParallelPlot;

   public:
    RowVisitor(const ParallelPlot *parallelPlot) :
     parallelPlot_(parallelPlot) {
      ns_ = parallelPlot_->visibleYColumns().count();

      details_.resize(size_t(ns_));

      for (int i = 0; i < ns_; ++i) {
        const auto &yColumn = parallelPlot_->visibleYColumns().getColumn(i);

        details_[size_t(i)] = parallelPlot_->columnDetails(yColumn);

        setRanges_.emplace_back();
      }
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      for (int i = 0; i < ns_; ++i) {
        auto &range = setRanges_[size_t(i)];

        const auto &yColumn = parallelPlot_->visibleYColumns().getColumn(i);

        ModelIndex yColumnInd(parallelPlot_, data.row, yColumn, data.parent);

        //---

        double defVal = parallelPlot_->getModelBadValue(yColumn, data.row);

        double x = 0.0;
        double y = defVal;

        // TODO: control default value ?
        if (! parallelPlot_->rowColValue(details_[size_t(i)], yColumnInd, y, /*defVal*/y))
          continue;

        if (parallelPlot_->isVertical())
          range.updateRange(x, y);
        else
          range.updateRange(y, x);
      }

      return State::OK;
    }

    const Ranges &setRanges() const { return setRanges_; }

   private:
    using ColumnDetailsArray = std::vector<CQChartsModelColumnDetails *>;

    const ParallelPlot* parallelPlot_ { nullptr };
    int                 ns_           { 0 };
    ColumnDetailsArray  details_      { nullptr };
    Ranges              setRanges_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  th->setRanges_ = visitor.setRanges();

  //---

  // set range from data
  th->dataRange_ = Range();

  int ns = visibleYColumns().count();

  for (int j = 0; j < ns; ++j) {
    auto &range = th->setRanges_[size_t(j)];
    if (! range.isSet()) continue;

    auto updateRange = [&](double x, double y) {
      if (isVertical()) {
        range.updateRange(x, y);

        th->dataRange_.updateRange(x, y);
      }
      else {
        range.updateRange(y, x);

        th->dataRange_.updateRange(y, x);
      }
    };

    updateRange(   - 0.5, range.ymin());
    updateRange(ns - 0.5, range.ymax());
  }

  //---

  // set plot range
  Range dataRange;

  auto updateRange = [&](double x, double y) {
    if (isVertical())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  updateRange(   - 0.5, 0.0);
  updateRange(ns - 0.5, 1.0);

  th->normalizedDataRange_ = dataRange;

  //---

  // set axes range and name
  if (isAxisLocal()) {
    for (int j = 0; j < ns; ++j) {
      auto *axis = this->axis(j);

      const auto &range = setRange(j);
      if (! range.isValid()) continue;

      const auto &yColumn = visibleYColumns().getColumn(j);

      bool ok;
      auto name = modelHHeaderString(yColumn, ok);

      //const_cast<CQChartsParallelPlot *>(this)->setDataRange(range);

      if (range.isSet()) {
        if (isVertical()) {
          if (isNormalized()) {
            axis->setRange(0.0, 1.0);
            axis->setValueRange(range.ymin(), range.ymax());
          }
          else {
            if (! isAxisSpread()) {
              axis->setRange     (range.ymin(), range.ymax());
              axis->setValueRange(range.ymin(), range.ymax());
            }
            else {
              axis->setRange     (dataRange_.ymin(), dataRange_.ymax());
              axis->setValueRange(dataRange_.ymin(), dataRange_.ymax());
            }
          }

          axis->setDefLabel(name);
        }
        else {
          if (isNormalized()) {
            axis->setRange(0.0, 1.0);
            axis->setValueRange(range.xmin(), range.xmax());
          }
          else {
            if (! isAxisSpread()) {
              axis->setRange     (range.xmin(), range.xmax());
              axis->setValueRange(range.xmin(), range.xmax());
            }
            else {
              axis->setRange     (dataRange_.xmin(), dataRange_.xmax());
              axis->setValueRange(dataRange_.xmin(), dataRange_.xmax());
            }
          }

          axis->setDefLabel(name);
        }
      }
    }
  }
  else {
    if (isVertical()) {
      mainXAxis_->setRange(0, ns);
      mainYAxis_->setRange(dataRange_.ymin(), dataRange_.ymax());

      mainXAxis_->setValueType(CQChartsAxisValueType::integer());
      mainYAxis_->setValueType(CQChartsAxisValueType::real   ());
    }
    else {
      mainXAxis_->setRange(dataRange_.xmin(), dataRange_.xmax());
      mainYAxis_->setRange(0, ns);

      mainXAxis_->setValueType(CQChartsAxisValueType::real   ());
      mainYAxis_->setValueType(CQChartsAxisValueType::integer());
    }

    //---

    auto *axis = this->axis(0);

    if (isNormalized())
      axis->setRange(0.0, 1.0);
    else {
      if (isVertical())
        axis->setRange(dataRange_.ymin(), dataRange_.ymax());
      else
        axis->setRange(dataRange_.xmin(), dataRange_.xmax());
    }
  }

  //---

  if (isNormalized())
    return normalizedDataRange_;
  else
    return dataRange_;
}

void
CQChartsParallelPlot::
updateAxes()
{
  if (isVertical()) {
    mainXAxis_->setDirection(Qt::Horizontal);
    mainYAxis_->setDirection(Qt::Vertical);
  }
  else {
    mainXAxis_->setDirection(Qt::Vertical);
    mainYAxis_->setDirection(Qt::Horizontal);
  }

  //---

  // create axes
  int ns = visibleYColumns().count();

  auto adir = orientation();

  if (int(axes_.size()) != ns || adir_ != adir) {
    adir_ = adir;

    axes_.clear();

    for (int j = 0; j < ns; ++j) {
      auto *axis = new CQChartsAxis(this, adir_, 0, 1);

      axis->moveToThread(this->thread());

      axis->setParent(this);
      axis->setPlot  (this);

      axis->setUpdatesEnabled(false);

      axes_.push_back(AxisP(axis));
    }
  }

  //---

  // update axis style
  for (int j = 0; j < ns; ++j) {
    auto *axis = this->axis(j);

    const auto &yColumn = visibleYColumns().getColumn(j);

    axis->setColumn(yColumn);

    if (! isAxisLocal())
      setAxisColumnLabels(axis);
  }
}

//------

void
CQChartsParallelPlot::
updateVisibleYColumns()
{
  visibleYColumns_ = Columns();

  int nc = yColumns().count();

  for (int ic = 0; ic < nc; ++ic) {
    if (! isYColumnVisible(ic))
      continue;

    visibleYColumns_.addColumn(yColumns().getColumn(ic));
  }
}

//------

bool
CQChartsParallelPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsParallelPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  // create polyline for value from each set
  using Polygons = std::vector<Polygon>;
  using Indices  = std::vector<QModelIndex>;

  class RowVisitor : public ModelVisitor {
   public:
    using ParallelPlot = CQChartsParallelPlot;

   public:
    RowVisitor(const ParallelPlot *plot) :
     parallelPlot_(plot) {
      ns_ = parallelPlot_->visibleYColumns().count();

      details_.resize(size_t(ns_));

      for (int i = 0; i < ns_; ++i) {
        const auto &yColumn = parallelPlot_->visibleYColumns().getColumn(i);

        details_[size_t(i)] = parallelPlot_->columnDetails(yColumn);
      }
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex xModelInd(parallelPlot_, data.row, parallelPlot_->xColumn(), data.parent);

      Polygon poly;

      auto xind = parallelPlot_->modelIndex(xModelInd);

      xinds_.push_back(xind);

      //---

      for (int i = 0; i < ns_; ++i) {
        const auto &yColumn = parallelPlot_->visibleYColumns().getColumn(i);

        ModelIndex yColumnInd(parallelPlot_, data.row, yColumn, data.parent);

        //---

        double x = i;
        double y = i;

        // TODO: control default value ?
        if (! parallelPlot_->rowColValue(details_[size_t(i)], yColumnInd, y, /*defVal*/y))
          continue;

        if (parallelPlot_->isVertical())
          poly.addPoint(Point(x, y));
        else
          poly.addPoint(Point(y, x));
      }

      polys_.push_back(std::move(poly));

      return State::OK;
    }

    const Polygons &polys() const { return polys_; }

    const Indices &xinds() const { return xinds_; }

   private:
    using ColumnDetailsArray = std::vector<CQChartsModelColumnDetails *>;

    const ParallelPlot* parallelPlot_ { nullptr };
    int                 ns_           { 0 };
    ColumnDetailsArray  details_      { nullptr };
    Polygons            polys_;
    Indices             xinds_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  const auto &polys = visitor.polys();
  const auto &xinds = visitor.xinds();

  auto *th = const_cast<CQChartsParallelPlot *>(this);

  //---

  double sx, sy;

  plotSymbolSize(symbolSize(), sx, sy);

  int n = int(polys.size());

  for (int i = 0; i < n; ++i) {
    const auto &poly = polys[size_t(i)];
    const auto &xind = xinds[size_t(i)];

    auto xind1 = normalizeIndex(xind);

    //---

    // add poly line object
    QString xname;

    if (xind.column() >= 0) {
      ModelIndex xModelInd(th, xind.row(), Column(xind.column()), xind.parent());

      bool ok;

      xname = modelString(xModelInd, ok);
    }
    else
      xname = QString::number(xind.row());

    BBox bbox;

    if (isNormalized())
      bbox = BBox(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                  normalizedDataRange_.xmax(), normalizedDataRange_.ymax());
    else
      bbox = BBox(dataRange_.xmin(), dataRange_.ymin(),
                  dataRange_.xmax(), dataRange_.ymax());

    ColorInd is(i, n);

    auto *lineObj = createLineObj(bbox, poly, xind1, is);

    lineObj->connectDataChanged(this, SLOT(updateSlot()));

    objs.push_back(lineObj);

    //---

    // create point object for each poly point
    int nl = poly.size();

    for (int j = 0; j < nl; ++j) {
      const auto &yColumn = visibleYColumns().getColumn(j);

      ModelIndex yColumnInd(th, i, yColumn, xind.parent());

      auto yind  = modelIndex(yColumnInd);
      auto yind1 = normalizeIndex(yind);

      //---

      const auto &range = setRange(j);
      if (! range.isValid()) continue;

      auto p = poly.point(j);

      double pos = 0.0;

      // normalize point in range
      if (isNormalized())
        pos = (isVertical() ? range.normalizeY(p.y) : range.normalizeX(p.x));
      else
        pos = (isVertical() ? p.y : p.x);

      double x, y;

      if (isVertical()) {
        x = j;
        y = pos;
      }
      else {
        x = pos;
        y = j;
      }

      BBox bbox(x - sx/2, y - sy/2, x + sx/2, y + sy/2);

      ColorInd is(i, n);
      ColorInd iv(j, nl);

      auto *pointObj = createPointObj(bbox, p.y, Point(x, y), yind1, is, iv);

      pointObj->connectDataChanged(this, SLOT(updateSlot()));

      objs.push_back(pointObj);
    }
  }

  //---

  return true;
}

bool
CQChartsParallelPlot::
rowColValue(const CQChartsModelColumnDetails *details,
            const ModelIndex &ind, double &value, double defVal) const
{
  auto useDefault = [&]() {
    value = defVal;
    return true;
  };

  bool ok;

  if (details && details->isNumeric()) {
    value = modelReal(ind, ok);
    if (! ok) return useDefault();
  }
  else {
    auto var = modelValue(ind, ok);
    if (! ok) return useDefault();

    value = (details ? details->uniqueId(var) : 0.0);
  }

  if (CMathUtil::isNaN(value))
    return false;

  return true;
}

bool
CQChartsParallelPlot::
probe(ProbeData &probeData) const
{
  int n = visibleYColumns().count();

  if (isVertical()) {
#if 0
    int x1 = std::min(std::max(CMathRound::RoundDown(probeData.p.x), 0), n - 1);
    int x2 = std::min(std::max(CMathRound::RoundUp  (probeData.p.x), 0), n - 1);

    auto range1 = setRange(x1);
    auto range2 = setRange(x2);
    if (! range1.isValid() || ! range2.isValid()) return false;

    for (const auto &plotObj : plotObjs_) {
      auto *obj = dynamic_cast<CQChartsParallelLineObj *>(plotObj);
      if (! obj) continue;

      std::vector<double> yvals;

      // interpolate to normalized (0-1) y
      if (! obj->interpY(probeData.p.x, yvals))
        continue;

      for (const auto &y : yvals) {
        double y1 = y*range1.ysize() + range1.ymin();
        double y2 = y*range2.ysize() + range2.ymin();

        double dx1 = probeData.p.x - x1;
        double dx2 = x2 - probeData.p.x;

        double yi;

        if (x1 != x2)
          yi = y1*(1 - dx1) + y2*(1 - dx2);
        else
          yi = y1;

        probeData.yvals.emplace_back(y, QString::number(yi));
      }
    }
#else
    int x = CMathRound::RoundNearest(probeData.p.x);

    x = std::min(std::max(x, 0), n - 1);

    auto range = setRange(x);
    if (! range.isValid()) return false;

    probeData.p.x = x;

    probeData.yvals.emplace_back(probeData.p.y, "",
      QString::number(probeData.p.y*range.ysize() + range.ymin()));
#endif
  }
  else {
    int y = CMathRound::RoundNearest(probeData.p.y);

    y = std::min(std::max(y, 0), n - 1);

    auto range = setRange(y);
    if (! range.isValid()) return false;

    probeData.p.y = y;

    probeData.xvals.emplace_back(probeData.p.x, "",
      QString::number(probeData.p.x*range.xsize() + range.xmin()));
  }

  return true;
}

//------

bool
CQChartsParallelPlot::
addMenuItems(QMenu *menu, const Point &)
{
  menu->addSeparator();

  CQUtil::addCheckedAction(menu, "Horizontal", isHorizontal(), this, SLOT(setHorizontal(bool)));

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  //---

  addRootMenuItems(menu);

  return true;
}

//---

void
CQChartsParallelPlot::
redrawAxis(CQChartsAxis *, bool wait)
{
  if (wait)
    drawObjs();
  else
    invalidateLayers();
}

CQChartsGeom::BBox
CQChartsParallelPlot::
axesFitBBox() const
{
  return axesBBox_;
}

CQChartsGeom::BBox
CQChartsParallelPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsParallelPlot::calcExtraFitBBox");

  auto range = (isNormalized() ? normalizedDataRange_ : dataRange_);
  if (! range.isSet()) return BBox();

  //--

  auto font = view()->plotFont(this, view()->font());

  QFontMetricsF fm(font);

  double tm = 4.0;

  double ts;

  if (isVertical())
    ts = pixelToWindowHeight(fm.height() + tm);
  else
    ts = pixelToWindowWidth(max_tw_ + tm);

  double tst = 0.0, tsb = 0.0;
  if (axisLabelPos() == AxisLabelPos::TOP    || axisLabelPos() == AxisLabelPos::ALTERNATE)
    tst = ts;
  if (axisLabelPos() == AxisLabelPos::BOTTOM || axisLabelPos() == AxisLabelPos::ALTERNATE)
    tsb = ts;

  //---

  BBox bbox;

  if (isVertical())
    bbox = BBox(range.xmin(), range.ymin() - tsb, range.xmax(), range.ymax() + tst);
  else
    bbox = BBox(range.xmin() - tsb, range.ymin(), range.xmax() + tst, range.ymax());

  return bbox;
}

//---

bool
CQChartsParallelPlot::
hasFgAxes() const
{
  return true;
}

void
CQChartsParallelPlot::
drawFgAxes(PaintDevice *device) const
{
  if (axes_.empty())
    return;

  //--

  auto *th = const_cast<CQChartsParallelPlot *>(this);

  //---

  th->axesBBox_ = BBox();

  th->max_tw_ = 0.0;

  double tm = 4.0;

  // draw axes
  int ns = visibleYColumns().count();

  if (isAxisLocal()) {
    for (int j = 0; j < ns; ++j) {
      assert(j < int(axes_.size()));

      auto *axis = this->axis(j);

      axis->setAxesLineData         (mainYAxis_->axesLineData());
      axis->setAxesLabelTextData    (mainYAxis_->axesLabelTextData());
      axis->setAxesTickLabelTextData(mainYAxis_->axesTickLabelTextData());

      axis->setAxesLabelTextVisible(axisLabelPos() == AxisLabelPos::AXIS);

      setPainterFont(device, mainYAxis_->axesLabelTextFont());

      //---

      const auto &range = setRange(j);
      if (! range.isValid()) continue;

      //---

      // draw set axis
      axis->setPosition(CQChartsOptReal(j));

      axis->draw(this, device);

      //---

      th->axesBBox_ += windowToPixel(axis->fitBBox());
    }
  }
  else {
    auto *axis = this->axis(0);

    axis->setAxesLineData         (mainYAxis_->axesLineData());
    axis->setAxesLabelTextData    (mainYAxis_->axesLabelTextData());
    axis->setAxesTickLabelTextData(mainYAxis_->axesTickLabelTextData());

    axis->setAxesLabelTextVisible(axisLabelPos() == AxisLabelPos::AXIS);

    axis      ->setGridLinesDisplayed(mainYAxis_->gridLinesDisplayed());
    mainXAxis_->setGridLinesDisplayed(mainYAxis_->gridLinesDisplayed());

    setPainterFont(device, mainYAxis_->axesLabelTextFont());

    //---

    // draw set axis
    axis->setPosition(CQChartsOptReal(-0.5));

    if (mainYAxis_->gridLinesDisplayed() != CQChartsAxis::GridLinesDisplayed::NONE) {
      axis->setGridStart(OptReal(-0.5));
      axis->setGridEnd  (OptReal(ns - 0.5));

      axis->drawGrid(this, device);
    }

    if (mainYAxis_->gridLinesDisplayed() != CQChartsAxis::GridLinesDisplayed::NONE)
      mainXAxis_->drawGrid(this, device);

    axis->draw(this, device);

    //---

    th->axesBBox_ += windowToPixel(axis->fitBBox());
  }

  //---

  // draw axes labels
  for (int j = 0; j < ns; ++j) {
    assert(j < int(axes_.size()));

    auto *axis = this->axis(j);

    //---

    // draw axis labels
    if (axisLabelPos() != AxisLabelPos::AXIS) {
      auto axisLabelPos = this->axisLabelPos();
      auto axisLabelLen = Length::plot(1.0);

      if (axisLabelPos == AxisLabelPos::ALTERNATE) {
        axisLabelPos = (j & 1 ? AxisLabelPos::BOTTOM : AxisLabelPos::TOP);
        axisLabelLen = Length::plot(2.0);
      }

      //---

      QString label;

      if (isAxisLocal())
        label = axis->label().string();
      else {
        const auto &yColumn = visibleYColumns().getColumn(j);

        bool ok;
        label = modelHHeaderString(yColumn, ok);
      }

      //---

      Point p;

      auto textRange = (isNormalized() ? normalizedDataRange_ : dataRange_);

      if (textRange.isSet()) {
        if (axisLabelPos == AxisLabelPos::TOP) {
          if (isVertical())
            p = windowToPixel(Point(j, textRange.ymax()));
          else
            p = windowToPixel(Point(textRange.xmax(), j));
        }
        else {
          if (isVertical())
            p = windowToPixel(Point(j, textRange.ymin()));
          else
            p = windowToPixel(Point(textRange.xmin(), j));
        }
      }

      //---

      QFontMetricsF fm(device->font());

      double tw = fm.horizontalAdvance(label);
      double ta = fm.ascent();
      double td = fm.descent();

      th->max_tw_ = std::max(max_tw_, tw);

      PenBrush tpenBrush;

      auto tc = mainYAxis_->interpAxesLabelTextColor(ColorInd());

      setPen(tpenBrush, PenData(true, tc, mainYAxis_->axesLabelTextAlpha()));

      device->setPen(tpenBrush.pen);

      Point tp;

      if (axisLabelPos == AxisLabelPos::TOP) {
        if (isVertical())
          tp = Point(p.x - tw/2.0, p.y - td - tm);
        else
          tp = Point(p.x + tm, p.y - (ta - td)/2);
      }
      else {
        if (isVertical())
          tp = Point(p.x - tw/2.0, p.y + ta + tm);
        else
          tp = Point(p.x - tw - tm, p.y - (ta - td)/2);
      }

      auto textOptions = mainYAxis_->axesLabelTextOptions();

      textOptions.angle = Angle();
      textOptions.align = Qt::AlignLeft;

      if (textOptions.clipLength <= 0)
        textOptions.clipLength = lengthPixelWidth(axisLabelLen);

      CQChartsDrawUtil::drawTextAtPoint(device, pixelToWindow(tp), label,
                                        textOptions, /*centered*/false);
    }
  }

  //---

  if (axesBBox_.isValid())
    th->axesBBox_ = pixelToWindow(axesBBox_);
}

//---

CQChartsParallelLineObj *
CQChartsParallelPlot::
createLineObj(const BBox &rect, const Polygon &poly, const QModelIndex &ind,
              const ColorInd &is) const
{
  return new CQChartsParallelLineObj(this, rect, poly, ind, is);
}

CQChartsParallelPointObj *
CQChartsParallelPlot::
createPointObj(const BBox &rect, double yval, const Point &p, const QModelIndex &ind,
               const ColorInd &is, const ColorInd &iv) const
{
  return new CQChartsParallelPointObj(this, rect, yval, p, ind, is, iv);
}

//---

bool
CQChartsParallelPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsParallelPlot::
execDrawForeground(PaintDevice *device) const
{
  CQChartsPlot::execDrawForeground(device);
}

//---

void
CQChartsParallelPlot::
resetYColumnVisible()
{
  yColumnVisible_.clear();
}

bool
CQChartsParallelPlot::
isYColumnVisible(int ic) const
{
  auto p = yColumnVisible_.find(ic);
  if (p == yColumnVisible_.end()) return true;

  return (*p).second;
}

void
CQChartsParallelPlot::
setYColumnVisible(int ic, bool visible)
{
  yColumnVisible_[ic] = visible;

  updateRangeAndObjs();

  Q_EMIT customDataChanged();
}

//---

CQChartsPlotCustomControls *
CQChartsParallelPlot::
createCustomControls()
{
  auto *controls = new CQChartsParallelPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsParallelLineObj::
CQChartsParallelLineObj(const CQChartsParallelPlot *parallelPlot, const BBox &rect,
                        const Polygon &poly, const QModelIndex &ind, const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsParallelPlot *>(parallelPlot),
                 rect, is, ColorInd(), ColorInd()),
 parallelPlot_(parallelPlot), poly_(poly)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
    setModelInd(ind);
}

QString
CQChartsParallelLineObj::
calcId() const
{
  QString xname;

  if (parallelPlot_->xColumn().isValid()) {
    ModelIndex xModelInd(plot(), modelInd().row(), parallelPlot_->xColumn(), modelInd().parent());

    bool ok;

    xname = plot()->modelString(xModelInd, ok);
  }
  else {
    xname = QString::number(modelInd().row());
  }

  return QString("%1:%2").arg(typeName()).arg(xname);
}

QString
CQChartsParallelLineObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  auto xname = this->xName();

  tableTip.addBoldLine(xname);

  int nl = poly_.size();

  for (int j = 0; j < nl; ++j) {
    const auto &yColumn = parallelPlot_->visibleYColumns().getColumn(j);

    bool ok;

    auto yname = parallelPlot_->modelHHeaderString(yColumn, ok);

    tableTip.addTableRow(yname, poly_.point(j).y);
  }

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

QString
CQChartsParallelLineObj::
xName() const
{
  QString xname;

  if (parallelPlot_->xColumn().isValid()) {
    ModelIndex xModelInd(plot(), modelInd().row(), parallelPlot_->xColumn(), modelInd().parent());

    bool ok;

    xname = parallelPlot_->modelString(xModelInd, ok);
  }
  else {
    xname = QString::number(modelInd().row());
  }

  return xname;
}

bool
CQChartsParallelLineObj::
isVisible() const
{
  if (! parallelPlot_->isLines())
    return false;

  return CQChartsPlotObj::isVisible();
}

bool
CQChartsParallelLineObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  if (! parallelPlot_->isLinesSelectable())
    return false;

  // TODO: check as lines created, only need to create lines close to point

  // create unnormalized polygon
  Polygon poly;

  getPolyLine(poly);

  // check if close enough to each line to be inside
  for (int i = 1; i < poly.size(); ++i) {
    double x1 = poly.point(i - 1).x;
    double y1 = poly.point(i - 1).y;
    double x2 = poly.point(i    ).x;
    double y2 = poly.point(i    ).y;

    double d;

    Point pl1(x1, y1);
    Point pl2(x2, y2);

    if (! CQChartsUtil::PointLineDistance(p, pl1, pl2, &d))
      continue;

    auto pdx = parallelPlot_->windowToPixelWidth (d);
    auto pdy = parallelPlot_->windowToPixelHeight(d);

    if (pdx < 4 || pdy < 4)
      return true;
  }

  return false;
}

#if 0
bool
CQChartsParallelLineObj::
interpY(double x, std::vector<double> &yvals) const
{
  if (! isVisible())
    return false;

  Polygon poly;

  getPolyLine(poly);

  for (int i = 1; i < poly.count(); ++i) {
    double x1 = poly.point(i - 1).x;
    double y1 = poly.point(i - 1).y;
    double x2 = poly.point(i    ).x;
    double y2 = poly.point(i    ).y;

    if (x >= x1 && x <= x2) {
      double y = (y2 - y1)*(x - x1)/(x2 - x1) + y1;

      yvals.push_back(y);
    }
  }

  return ! yvals.empty();
}
#endif

void
CQChartsParallelLineObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex (inds, parallelPlot_->xColumn());
  addColumnsSelectIndex(inds, parallelPlot_->visibleYColumns());
}

void
CQChartsParallelLineObj::
draw(PaintDevice *device) const
{
  // set pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw polyline (using unnormalized polygon)
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  polyLine_ = Polygon();

  getPolyLine(polyLine_);

  for (int i = 1; i < polyLine_.size(); ++i)
    device->drawLine(polyLine_.point(i - 1), polyLine_.point(i));

  device->resetColorNames();
}

void
CQChartsParallelLineObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  parallelPlot_->setLineDataPen(penBrush.pen, colorInd);

  if (parallelPlot_->colorColumn().isValid() &&
      parallelPlot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    auto ind1 = modelInd();

    Color indColor;

    if (parallelPlot_->colorColumnColor(ind1.row(), ind1.parent(), indColor))
      penBrush.pen.setColor(parallelPlot_->interpColor(indColor, colorInd));
  }

  parallelPlot_->setBrush(penBrush, BrushData(false));

  if (updateState)
    parallelPlot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::LINE);
}

void
CQChartsParallelLineObj::
getPolyLine(Polygon &poly) const
{
  // create normalized polyline
  if (parallelPlot_->isNormalized()) {
    for (int i = 0; i < poly_.size(); ++i) {
      const auto &range = parallelPlot_->setRange(i);
      if (! range.isValid()) continue;

      double x, y;

      if (parallelPlot_->isVertical()) {
        x = poly_.point(i).x;
        y = range.normalizeY(poly_.point(i).y);
      }
      else {
        x = range.normalizeX(poly_.point(i).x);
        y = poly_.point(i).y;
      }

      poly.addPoint(Point(x, y));
    }
  }
  else {
    poly = poly_;
  }
}

//------

CQChartsParallelPointObj::
CQChartsParallelPointObj(const ParallelPlot *parallelPlot, const BBox &rect, double yval,
                         const Point &p, const QModelIndex &ind, const ColorInd &is,
                         const ColorInd &iv) :
 CQChartsPlotPointObj(const_cast<ParallelPlot *>(parallelPlot), rect, p, is, ColorInd(), iv),
 parallelPlot_(parallelPlot), yval_(yval)
{
  if (ind.isValid())
    setModelInd(ind);
}

//---

CQChartsLength
CQChartsParallelPointObj::
calcSymbolSize() const
{
  return parallelPlot()->symbolSize();
}

//---

QString
CQChartsParallelPointObj::
calcId() const
{
  auto xname = this->xName();

  const auto &yColumn = parallelPlot_->visibleYColumns().getColumn(iv_.i);

  bool ok1;

  auto yname = parallelPlot_->modelHHeaderString(yColumn, ok1);

  return QString("%1:%2:%3=%4").arg(typeName()).arg(xname).arg(yname).arg(yval_);
}

QString
CQChartsParallelPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  auto xname = this->xName();

  tableTip.addBoldLine(xname);

  const auto &yColumn = parallelPlot_->visibleYColumns().getColumn(iv_.i);

  bool ok1;

  auto yname = parallelPlot_->modelHHeaderString(yColumn, ok1);

  tableTip.addTableRow(yname, yval_);

  //---

  parallelPlot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

QString
CQChartsParallelPointObj::
xName() const
{
  QString xname;

  if (parallelPlot_->xColumn().isValid()) {
    ModelIndex xModelInd(plot(), modelInd().row(), parallelPlot_->xColumn(), modelInd().parent());

    bool ok;

    xname = parallelPlot_->modelString(xModelInd, ok);
  }
  else {
    xname = QString::number(modelInd().row());
  }

  return xname;
}

//---

bool
CQChartsParallelPointObj::
isVisible() const
{
  if (! parallelPlot_->isPoints())
    return false;

  return CQChartsPlotPointObj::isVisible();
}

//---

void
CQChartsParallelPointObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, Column(modelInd().column()));
}

//---

void
CQChartsParallelPointObj::
draw(PaintDevice *device) const
{
  auto symbol = parallelPlot()->symbol();

  if (! symbol.isValid())
    return;

  //---

  // get symbol size
  double sx, sy;

  calcSymbolPixelSize(sx, sy, /*square*/false, /*enforceMinSize*/false);

  //---

  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw symbol
  plot()->drawSymbol(device, point(), symbol, sx, sy, penBrush, /*scaled*/false);
}

void
CQChartsParallelPointObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  parallelPlot()->setSymbolPenBrush(penBrush, colorInd);

  if (parallelPlot_->colorColumn().isValid() &&
      parallelPlot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    auto ind1 = modelInd();

    Color indColor;

    if (parallelPlot_->colorColumnColor(ind1.row(), ind1.parent(), indColor))
      penBrush.brush.setColor(parallelPlot_->interpColor(indColor, colorInd));
  }

  if (updateState)
    plot()->updateObjPenBrushState(this, penBrush, drawType());
}

//------

CQChartsParallelPlotColumnChooser::
CQChartsParallelPlotColumnChooser(CQChartsParallelPlot *plot) :
 CQChartsPlotColumnChooser(plot)
{
}

const CQChartsColumns &
CQChartsParallelPlotColumnChooser::
getColumns() const
{
  auto *plot = dynamic_cast<CQChartsParallelPlot *>(this->plot());
  assert(plot);

  return plot->yColumns();
}

bool
CQChartsParallelPlotColumnChooser::
isColumnVisible(int ic) const
{
  auto *plot = dynamic_cast<CQChartsParallelPlot *>(this->plot());

  return (plot ? plot->isYColumnVisible(ic) : false);
}

void
CQChartsParallelPlotColumnChooser::
setColumnVisible(int ic, bool visible)
{
  auto *plot = dynamic_cast<CQChartsParallelPlot *>(this->plot());

  if (plot)
    plot->setYColumnVisible(ic, visible);
}

//------

CQChartsParallelPlotCustomControls::
CQChartsParallelPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "parallel")
{
}

void
CQChartsParallelPlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsParallelPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addOptionsWidgets();

  addColorColumnWidgets("Line Color");
}

void
CQChartsParallelPlotCustomControls::
addColumnWidgets()
{
  // columns frame
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  // x and y columns
  addNamedColumnWidgets(QStringList() << "x" << "y", columnsFrame);

  // column chooser
  chooser_ = new CQChartsParallelPlotColumnChooser;

  addFrameWidget(columnsFrame, chooser_);
}

void
CQChartsParallelPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  //---

  orientationCombo_ = createEnumEdit("orientation");

  addFrameWidget(optionsFrame_, "Orientation", orientationCombo_);

  //addFrameRowStretch(optionsFrame_);

  // lines selectable
}

void
CQChartsParallelPlotCustomControls::
connectSlots(bool b)
{
  CQUtil::optConnectDisconnect(b,
    orientationCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationSlot()));

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsParallelPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && parallelPlot_)
    disconnect(parallelPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  parallelPlot_ = dynamic_cast<CQChartsParallelPlot *>(plot);

  chooser_->setPlot(parallelPlot_);

  CQChartsPlotCustomControls::setPlot(plot);

  if (parallelPlot_)
    connect(parallelPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsParallelPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  orientationCombo_->setCurrentValue(static_cast<int>(parallelPlot_->orientation()));

  chooser_->updateWidgets();

  //---

  connectSlots(true);

  //---

  CQChartsPlotCustomControls::updateWidgets();
}

void
CQChartsParallelPlotCustomControls::
orientationSlot()
{
  parallelPlot_->setOrientation(static_cast<Qt::Orientation>(orientationCombo_->currentValue()));
}

CQChartsColor
CQChartsParallelPlotCustomControls::
getColorValue()
{
  return parallelPlot_->linesColor();
}

void
CQChartsParallelPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  parallelPlot_->setLinesColor(c);
}
