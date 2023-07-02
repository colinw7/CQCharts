#include <CQChartsSummaryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsSymbolSet.h>
#include <CQChartsValueSet.h>
#include <CQChartsDrawObj.h>
#include <CQChartsPlotDrawUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTip.h>
#include <CQChartsFitData.h>

#include <CQChartsScatterPlot.h>
#include <CQChartsDistributionPlot.h>
#include <CQChartsParallelPlot.h>
#include <CQChartsBoxPlot.h>
#include <CQChartsPiePlot.h>

#include <CQPropertyViewItem.h>
#include <CQTableWidget.h>
#include <CQPerfMonitor.h>
#include <CMathCorrelation.h>

#include <QMenu>
#include <QCheckBox>
#include <QVBoxLayout>

CQChartsSummaryPlotType::
CQChartsSummaryPlotType()
{
}

void
CQChartsSummaryPlotType::
addParameters()
{
  using SummaryPlot = CQChartsSummaryPlot;

  addColumnsParameter("columns", "Columns", "columns").
    setRequired().setPropPath("columns.columns").setTip("Columns");

  addColumnParameter("group", "Group", "groupColumn").
   setGroupable().setBasic().setPropPath("columns.group").setTip("Group column");

  // options
  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("MATRIX"  , static_cast<int>(SummaryPlot::PlotType::MATRIX  )).
    addNameValue("PARALLEL", static_cast<int>(SummaryPlot::PlotType::PARALLEL)).
    setTip("Plot Type");

  addEnumParameter("diagonalType", "Diagonal Cell Type", "diagonalType").
    addNameValue("NONE"        , static_cast<int>(SummaryPlot::DiagonalType::NONE        )).
    addNameValue("BOXPLOT"     , static_cast<int>(SummaryPlot::DiagonalType::BOXPLOT     )).
    addNameValue("DISTRIBUTION", static_cast<int>(SummaryPlot::DiagonalType::DISTRIBUTION)).
    addNameValue("PIE"         , static_cast<int>(SummaryPlot::DiagonalType::PIE         )).
    setTip("Diagonal Cell Type");
  addEnumParameter("upperDiagonalType", "Upper Diagonal Cell Type", "upperDiagonalType").
    addNameValue("NONE"       , static_cast<int>(SummaryPlot::OffDiagonalType::NONE       )).
    addNameValue("SCATTER"    , static_cast<int>(SummaryPlot::OffDiagonalType::SCATTER    )).
    addNameValue("CORRELATION", static_cast<int>(SummaryPlot::OffDiagonalType::CORRELATION)).
    setTip("Upper Diagonal Cell Type");
  addEnumParameter("lowerDiagonalType", "Lower Diagonal Cell Type", "lowerDiagonalType").
    addNameValue("NONE"       , static_cast<int>(SummaryPlot::OffDiagonalType::NONE       )).
    addNameValue("SCATTER"    , static_cast<int>(SummaryPlot::OffDiagonalType::SCATTER    )).
    addNameValue("CORRELATION", static_cast<int>(SummaryPlot::OffDiagonalType::CORRELATION)).
    setTip("Lower Diagonal Cell Type");

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsSummaryPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Table Plot").
    h3("Summary").
     p("Draws summary of columns in grid.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/summary_plot.png"));
}

void
CQChartsSummaryPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  Columns columns;

  int nc = details->numColumns();

  for (int i = 0; i < nc; ++i)
    columns.addColumn(CQChartsColumn(i));

  analyzeModelData.parameterNameColumns["columns"] = columns;
}

CQChartsPlot *
CQChartsSummaryPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsSummaryPlot(view, model);
}

//------

CQChartsSummaryPlot::
CQChartsSummaryPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("summary"), model),
 CQChartsObjScatterPointData     <CQChartsSummaryPlot>(this),
 CQChartsObjDistributionShapeData<CQChartsSummaryPlot>(this),
 CQChartsObjBoxPlotShapeData     <CQChartsSummaryPlot>(this),
 CQChartsObjBoxPlotTextData      <CQChartsSummaryPlot>(this),
 CQChartsObjPieShapeData         <CQChartsSummaryPlot>(this),
 CQChartsObjCorrelationTextData  <CQChartsSummaryPlot>(this),
 CQChartsObjXLabelTextData       <CQChartsSummaryPlot>(this),
 CQChartsObjYLabelTextData       <CQChartsSummaryPlot>(this)
{
}

CQChartsSummaryPlot::
~CQChartsSummaryPlot()
{
  CQChartsSummaryPlot::term();
}

//---

void
CQChartsSummaryPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  setId("summaryPlot");

  addTitle();

  setXLabelTextAlign(Qt::AlignHCenter | Qt::AlignTop);
  setYLabelTextAlign(Qt::AlignRight | Qt::AlignVCenter);

  setXLabelTextFont(CQChartsFont().decFontSize(4));
  setYLabelTextFont(CQChartsFont().decFontSize(4));

  //---

  auto createPlot = [&](const QString &typeName) {
    auto *type = charts()->plotType(typeName);
    assert(type);

    const auto &model = this->currentModel();

    auto *plot = type->createAndInit(view(), model);

    plot->setVisible(false);

    int n = view()->numPlots();

    plot->setId(QString("Chart.%1.%2").arg(n + 1).arg(typeName));

    double vr = CQChartsView::viewportRange();

    BBox bbox(0, 0, vr, vr);

    view()->addPlot(plot, bbox);

    plot->setRootPlot(this);

    return plot;
  };

  scatterPlot_      = dynamic_cast<ScatterPlot      *>(createPlot("scatter"));
  distributionPlot_ = dynamic_cast<DistributionPlot *>(createPlot("distribution"));
  parallelPlot_     = dynamic_cast<ParallelPlot     *>(createPlot("parallel"));
  boxPlot_          = dynamic_cast<BoxPlot          *>(createPlot("box"));
  piePlot_          = dynamic_cast<PiePlot          *>(createPlot("pie"));

  assert(scatterPlot_ && distributionPlot_ && parallelPlot_ && boxPlot_ && piePlot_);

  scatterPlot_->setXColumn(Column::makeRow());
  scatterPlot_->setYColumn(Column::makeRow());

  distributionPlot_->setValueColumns(Columns(Column::makeRow()));

  parallelPlot_->setYColumns(Columns(Column::makeRow()));

  boxPlot_->setValueColumns(Columns(Column::makeRow()));

  piePlot_->setValueColumns(Columns(Column::makeRow()));
  piePlot_->setBucketed(true);

  //---

  setScatterSymbol(Symbol::circle());

  setScatterSymbolStroked(true);
  setScatterSymbolStrokeAlpha(Alpha(0.3));
  setScatterSymbolFilled (true);
  setScatterSymbolFillColor(Color::makePalette());

  setDistributionStroked(true);
  setDistributionStrokeAlpha(Alpha(0.3));
  setDistributionFilled (true);
  setDistributionFillColor(Color::makePalette());

  setBoxPlotStroked(true);
  setBoxPlotStrokeAlpha(Alpha(0.3));
  setBoxPlotFilled (true);
  setBoxPlotFillColor(Color::makePalette());
  setBoxPlotTextFont(CQChartsFont().decFontSize(8));

  setPieStroked(true);
  setPieStrokeAlpha(Alpha(0.3));
  setPieFilled (true);
  setPieFillColor(Color::makePalette());

  setCorrelationTextFont(CQChartsFont().decFontSize(8));

  //---

  // left, top, right, bottom
  setOuterMargin(PlotMargin(Length::plot(0.1), Length::plot(0.1),
                            Length::plot(0.1), Length::plot(0.1)));
}

void
CQChartsSummaryPlot::
term()
{
}

//---

void
CQChartsSummaryPlot::
setPlotType(const PlotType &t, bool update)
{
  CQChartsUtil::testAndSet(plotType_, t, [&]() {
    if (update) updatePlots();

    Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setExpanded(bool b)
{
  CQChartsUtil::testAndSet(expanded_, b, [&]() {
    updatePlots(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
updatePlotsSlot()
{
  updatePlots();
}

void
CQChartsSummaryPlot::
updatePlots()
{
  //NoUpdate parallelNoUpdate    (parallelPlot_);
  //NoUpdate scatterNoUpdate     (scatterPlot_);
  //NoUpdate distributionNoUpdate(distributionPlot_);
  //NoUpdate boxNoUpdate         (boxPlot_);
  //NoUpdate pieNoUpdate         (piePlot_);

  //---

  Plot *currentPlot = this;

  if (! isExpanded() && (plotType() == PlotType::PARALLEL)) {
    parallelPlot_->setYColumns(columns());

    for (const auto &cv : columnVisible_) {
      if (! cv.second)
        parallelPlot_->setYColumnVisible(cv.first, false);
    }

    currentPlot = parallelPlot_;
  }
  else
    parallelPlot_->setYColumns(Columns(Column::makeRow()));

  if (isExpanded() && (expandRow_ != expandCol_)) {
    auto column1 = visibleColumns().getColumn(expandRow_);
    auto column2 = visibleColumns().getColumn(expandCol_);

    scatterPlot_->setXColumn(column1);
    scatterPlot_->setYColumn(column2);
    scatterPlot_->setGroupColumn(groupColumn());
    scatterPlot_->setTipColumns(visibleColumns());

    currentPlot = scatterPlot_;
  }
  else {
    scatterPlot_->setXColumn(Column::makeRow());
    scatterPlot_->setYColumn(Column::makeRow());
  }

  if (isExpanded() && (expandRow_ == expandCol_)) {
    auto column = visibleColumns().getColumn(expandRow_);

    if      (diagonalType() == CQChartsSummaryPlot::DiagonalType::DISTRIBUTION) {
      distributionPlot_->setValueColumns(Columns(column));
      distributionPlot_->setTipColumns(visibleColumns());

      currentPlot = distributionPlot_;
    }
    else if (diagonalType() == CQChartsSummaryPlot::DiagonalType::BOXPLOT) {
      boxPlot_->setValueColumns(Columns(column));
      boxPlot_->setTipColumns(visibleColumns());

      currentPlot = boxPlot_;
    }
    else if (diagonalType() == CQChartsSummaryPlot::DiagonalType::PIE) {
      piePlot_->setValueColumns(Columns(column));
      piePlot_->setTipColumns(visibleColumns());

      currentPlot = piePlot_;
    }
  }
  else {
    distributionPlot_->setValueColumns(Columns(Column::makeRow()));
  }

  currentPlot->setVisible(true);

  view()->setCurrentPlot(currentPlot);

  if (this != currentPlot)
    this->setVisible(false);

  if (parallelPlot_ != currentPlot)
    parallelPlot_->setVisible(false);

  if (scatterPlot_ != currentPlot)
    scatterPlot_->setVisible(false);

  if (distributionPlot_ != currentPlot)
    distributionPlot_->setVisible(false);

  if (boxPlot_ != currentPlot)
    boxPlot_->setVisible(false);

  if (piePlot_ != currentPlot)
    piePlot_->setVisible(false);
}

//---

void
CQChartsSummaryPlot::
setDiagonalType(const DiagonalType &t)
{
  CQChartsUtil::testAndSet(diagonalType_, t, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setLowerDiagonalType(const OffDiagonalType &t)
{
  CQChartsUtil::testAndSet(lowerDiagonalType_, t, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setUpperDiagonalType(const OffDiagonalType &t)
{
  CQChartsUtil::testAndSet(upperDiagonalType_, t, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

//---

CQChartsLength
CQChartsSummaryPlot::
calcScatterSymbolSize() const
{
  auto symbolSize = this->scatterSymbolSize();

  if (! symbolSize.isSet()) {
    int nc = std::max(visibleColumns().count(), 1);

    symbolSize = Length::plot(1.0/(nc*20.0));
  }

  return symbolSize;
}

//---

void
CQChartsSummaryPlot::
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFit_, b, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setDensity(bool b)
{
  CQChartsUtil::testAndSet(density_, b, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsSummaryPlot::
modelTypeChangedSlot(int modelInd)
{
  auto *modelData = charts()->currentModelData();

  if (modelData && modelData->isInd(modelInd))
    updateRangeAndObjs();
}

//---

void
CQChartsSummaryPlot::
setColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(columns_, c, [&]() {
    visibleColumns_ = columns_;

    resetColumnVisible();

    updateRangeAndObjs();

    Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setGroupColumn(const Column &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() {
    resetSetHidden();

    if (isExpanded() && (expandRow_ != expandCol_)) {
      scatterPlot_->setGroupColumn(groupColumn());
    }

    Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsSummaryPlot::
setBorder(const Length &l)
{
  CQChartsUtil::testAndSet(border_, l, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsSummaryPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if (name == "group") c = this->groupColumn();
  else                 c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsSummaryPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if (name == "group") this->setGroupColumn(c);
  else                 CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsSummaryPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "columns") c = this->columns();
  else                   c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsSummaryPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "columns") this->setColumns(c);
  else                   CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsSummaryPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "columns"    , "columns", "Columns");
  addProp("columns", "groupColumn", "group"  , "Group column");

  // style
  addProp("options", "plotType", "plotType", "Plot type");

  // x/y axis label text
  addProp("xaxis/text", "xLabels", "visible", "X labels visible");

  addTextProperties("xaxis/text", "xLabelText", "X label",
                    CQChartsTextOptions::ValueType::ALL);

  addProp("yaxis/text", "yLabels", "visible", "Y labels visible");

  addTextProperties("yaxis/text", "yLabelText", "Y label",
                    CQChartsTextOptions::ValueType::ALL);

  // cell types
  addProp("cell", "diagonalType"     , "diagonal"     , "Diagonal cell type");
  addProp("cell", "lowerDiagonalType", "lowerDiagonal", "Lower Diagonal cell type");
  addProp("cell", "upperDiagonalType", "upperDiagonal", "Upper Diagonal cell type");
  addProp("cell", "border"           , "border"       , "Border Size");

  // scatter plot
  addSymbolProperties("scatter/symbol", "scatter", "Scatter Symbol");

  // distribution plot
  addProp("distribution/fill", "distributionFilled", "visible", "Distribution fill visible");

  addFillProperties("distribution/fill", "distributionFill", "Distribution");

  addProp("distribution/stroke", "distributionStroked", "visible", "Distribution stroke visible");

  addLineProperties("distribution/stroke", "distributionStroke", "Distribution");

  // box plot
  addProp("box/fill", "boxPlotFilled", "visible", "Box fill visible");

  addFillProperties("box/fill", "boxPlotFill", "Box");

  addProp("box/stroke", "boxPlotStroked", "visible", "Box stroke visible");

  addLineProperties("box/stroke", "boxPlotStroke", "Box");

  addTextProperties("box/text", "boxPlotText", "Box value",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  // pie plot
  addProp("pie/fill", "pieFilled", "visible", "Box fill visible");

  addFillProperties("pie/fill", "pieFill", "Box");

  addProp("pie/stroke", "pieStroked", "visible", "Box stroke visible");

  addLineProperties("pie/stroke", "pieStroke", "Box");

  // correlation plot
  addTextProperties("correlation/text", "correlationText", "Correlation",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  // overlays
  addProp("overlays", "bestFit", "bestFit", "Show best fit on scatter");
  addProp("overlays", "density", "density", "Show density on distribution");
}

//---

void
CQChartsSummaryPlot::
setXLabels(bool b)
{
  CQChartsUtil::testAndSet(xLabels_, b, [&]() { drawObjs(); } );
}

void
CQChartsSummaryPlot::
setYLabels(bool b)
{
  CQChartsUtil::testAndSet(yLabels_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsSummaryPlot::
diagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text().toLower();

    if      (name == "none"        ) setDiagonalType(DiagonalType::NONE);
    else if (name == "distribution") setDiagonalType(DiagonalType::DISTRIBUTION);
    else if (name == "pie"         ) setDiagonalType(DiagonalType::PIE);
  }
  else
    setDiagonalType(DiagonalType::NONE);
}

void
CQChartsSummaryPlot::
upperDiagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text().toLower();

    if      (name == "none"       ) setUpperDiagonalType(OffDiagonalType::NONE);
    else if (name == "scatter"    ) setUpperDiagonalType(OffDiagonalType::SCATTER);
    else if (name == "correlation") setUpperDiagonalType(OffDiagonalType::CORRELATION);
  }
  else
    setUpperDiagonalType(OffDiagonalType::NONE);
}

void
CQChartsSummaryPlot::
lowerDiagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text();

    if      (name == "None"       ) setLowerDiagonalType(OffDiagonalType::NONE);
    else if (name == "Scatter"    ) setUpperDiagonalType(OffDiagonalType::SCATTER);
    else if (name == "Correlation") setUpperDiagonalType(OffDiagonalType::CORRELATION);
  }
  else
    setLowerDiagonalType(OffDiagonalType::NONE);
}

//---

CQChartsGeom::Range
CQChartsSummaryPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSummaryPlot::calcRange");

  //---

  const_cast<CQChartsSummaryPlot *>(this)->updateVisibleColumns();

  // square (nc, nc)
  int nc = std::max(visibleColumns().count(), 1);

  Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(nc , nc );

  //---

  return dataRange;
}

void
CQChartsSummaryPlot::
updateVisibleColumns()
{
  visibleColumns_ = Columns();

  int nc = columns().count();

  for (int ic = 0; ic < nc; ++ic) {
    if (! isColumnVisible(ic))
      continue;

    visibleColumns_.addColumn(columns().getColumn(ic));
  }
}

bool
CQChartsSummaryPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsSummaryPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  int nc = visibleColumns().count();

  for (int ir = 0; ir < nc; ++ir) {
    for (int ic = 0; ic < nc; ++ic) {
      auto *obj = createCellObj(cellBBox(ir, ic), ir, ic);

      obj->connectDataChanged(this, SLOT(updateSlot()));

      objs.push_back(obj);
    }
  }

  //---

  return true;
}

CQChartsGeom::BBox
CQChartsSummaryPlot::
cellBBox(int row, int col) const
{
  int nc = visibleColumns().count();

  double pymax = nc - row;
  double pymin = pymax - 1;

  double pxmin = col;
  double pxmax = pxmin + 1;

  return BBox(pxmin, pymin, pxmax, pymax);
}

//---

QString
CQChartsSummaryPlot::
posStr(const Point &w) const
{
  auto *obj = cellObjAtPoint(w);
  if (! obj) return "";

  auto p = obj->parentToPlot(w);

  return xStr(p.x) + " " + yStr(p.y);
}

//---

bool
CQChartsSummaryPlot::
addMenuItems(QMenu *menu, const Point &)
{
  menuObj_ = selectedCellObj();

  if (! menuObj_) {
    auto gpos = view()->menuPos();
    auto pos  = view()->mapFromGlobal(QPointF(gpos.x, gpos.y).toPoint());

    auto wpos = pixelToWindow(Point(pos));

    menuObj_ = cellObjAtPoint(wpos);
  }

  if (menuObj_) {
    menu->addSeparator();

    addMenuAction(menu, "Expand", SLOT(expandSlot()));
  }

  return true;
}

//---

bool
CQChartsSummaryPlot::
pointSelect(const Point &, SelMod)
{
  return true;
}

bool
CQChartsSummaryPlot::
rectSelect(const BBox &r, SelMod)
{
  auto p = r.getCenter();

  auto *obj = cellObjAtPoint(p);
  if (! obj) return false;

  auto p1 = obj->parentToPlot(r.getLL());
  auto p2 = obj->parentToPlot(r.getUR());

  auto r1 = BBox(p1, p2);

  auto cellType = obj->getCellType();

  if      (cellType == CellType::SCATTER) {
    auto column1 = obj->rowColumn();
    auto column2 = obj->colColumn();

    columnRange_[column1] = MinMax(r1.getXMin(), r1.getXMax());
    columnRange_[column2] = MinMax(r1.getYMin(), r1.getYMax());

    drawObjs();
  }
  else if (cellType == CellType::DISTRIBUTION) {
    auto column = obj->rowColumn();

    columnRange_[column] = MinMax(r1.getXMin(), r1.getXMax());

    drawObjs();
  }
  else if (cellType == CellType::BOXPLOT) {
    auto column = obj->rowColumn();

    columnRange_[column] = MinMax(r1.getYMin(), r1.getYMax());

    drawObjs();
  }

  return true;
}

//---

CQChartsSummaryCellObj *
CQChartsSummaryPlot::
selectedCellObj() const
{
  PlotObjs objs;

  selectedPlotObjs(objs);
  if (objs.empty()) return nullptr;

  auto *obj = *objs.begin();

  return dynamic_cast<CQChartsSummaryCellObj *>(obj);
}

CQChartsSummaryPlot::CellObj *
CQChartsSummaryPlot::
cellObjAtPoint(const Point &p) const
{
  PlotObjs objs;

  plotObjsAtPoint(p, objs, Constraints::SELECTABLE);

  return dynamic_cast<CellObj *>(! objs.empty() ? *objs.begin() : nullptr);
}

CQChartsSummaryPlot::MinMax
CQChartsSummaryPlot::
columnRange(const Column &c) const
{
  auto pc = columnRange_.find(c);

  return (pc != columnRange_.end() ? (*pc).second : MinMax());
}

void
CQChartsSummaryPlot::
clearColumnRanges()
{
  columnRange_.clear();

  drawObjs();
}

void
CQChartsSummaryPlot::
selectColumnRanges()
{
  auto *modelData = charts()->currentModelData();

  QItemSelection sel;

  int nc = visibleColumns().count();

  for (int ic = 0; ic < nc; ++ic) {
    auto column = visibleColumns().getColumn(ic);

    auto range = columnRange(column);
    if (! range.isSet()) continue;

    auto *details = columnDetails(column);
    if (! details) continue;

    int n = details->numRows();

    std::set<int> rows;

    for (uint i = 0; i < uint(n); ++i) {
      double x = 0.0;

      if (details->isNumeric()) {
        bool ok;
        x = CQChartsVariant::toReal(details->value(i), ok);
        if (! ok) continue;
      }
      else
        x = details->uniqueId(details->value(i));

      if (range.inside(x))
        rows.insert(i);
    }

    int icolumn = std::max(column.column(), 0);

    for (const int &row : rows) {
      auto ind = modelData->model()->index(row, icolumn, QModelIndex());

      sel.select(ind, ind);
    }
  }

  modelData->select(sel);
}

//---

void
CQChartsSummaryPlot::
expandSlot()
{
  auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(menuObj_);
  if (! cellObj) return;

  expandCell(cellObj);
}

void
CQChartsSummaryPlot::
expandCell(CellObj *cellObj)
{
  expandRow_ = cellObj->row();
  expandCol_ = cellObj->col();

  setExpanded(true);
}

void
CQChartsSummaryPlot::
collapseCell()
{
  if (isExpanded()) {
    if (expandRow_ != expandCol_) {
      scatterPlot_->collapseRoot();
    }
    else {
      if      (diagonalType() == CQChartsSummaryPlot::DiagonalType::DISTRIBUTION)
        distributionPlot_->collapseRoot();
      else if (diagonalType() == CQChartsSummaryPlot::DiagonalType::BOXPLOT)
        boxPlot_->collapseRoot();
      else if (diagonalType() == CQChartsSummaryPlot::DiagonalType::PIE)
        piePlot_->collapseRoot();
    }
  }
}

void
CQChartsSummaryPlot::
notifyCollapse()
{
  setPlotType(PlotType::MATRIX, /*update*/false);

  setExpanded(false);
}

//------

void
CQChartsSummaryPlot::
postResize()
{
  CQChartsPlot::postResize();

  updateRangeAndObjs();
}

//------

bool
CQChartsSummaryPlot::
hasBackground() const
{
  return true;
}

void
CQChartsSummaryPlot::
execDrawBackground(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryPlot::execDrawBackground");

  //---

  int nc = visibleColumns().count();

  for (int ic = 0; ic < nc; ++ic) {
    auto column = visibleColumns().getColumn(ic);

    auto bbox = cellBBox(ic, ic);

    bool ok;
    auto str = modelHHeaderString(column, ok);

    //---

    if (isXLabels()) {
      setPainterFont(device, xLabelTextFont());

      auto tsize = CQChartsDrawUtil::calcTextSize(str, device->font(), CQChartsTextOptions());

      //---

      ColorInd colorInd;
      PenBrush tpenBrush;

      setXLabelTextPenBrush(tpenBrush, colorInd);

      CQChartsDrawUtil::setPenBrush(device, tpenBrush);

      //---

      auto thh = pixelToWindowHeight(tsize.height());

      Point p1(bbox.getXMid(), -thh/2.0);

      auto textOptions = xLabelTextOptions(device);

    //textOptions.angle      = Angle();
      textOptions.clipLength = lengthPixelWidth(Length::plot(1.0));
      textOptions.clipped    = false;

      textOptions = adjustTextOptions(textOptions);

      //---

      CQChartsDrawUtil::drawTextAtPoint(device, p1, str, textOptions);
    }

    //---

    if (isYLabels()) {
      setPainterFont(device, yLabelTextFont());

      auto tsize = CQChartsDrawUtil::calcTextSize(str, device->font(), CQChartsTextOptions());

      //---

      ColorInd colorInd;
      PenBrush tpenBrush;

      setYLabelTextPenBrush(tpenBrush, colorInd);

      CQChartsDrawUtil::setPenBrush(device, tpenBrush);

      //---

      auto thw = pixelToWindowWidth(tsize.height());

      Point p2(-thw/2.0, bbox.getYMid());

      auto textOptions = yLabelTextOptions(device);

    //textOptions.angle      = Angle(90.0);
      textOptions.clipLength = lengthPixelWidth(Length::plot(1.0));
      textOptions.clipped    = false;

      textOptions = adjustTextOptions(textOptions);

      //---

      CQChartsDrawUtil::drawTextAtPoint(device, p2, str, textOptions);
    }
  }
}

//------

CQChartsGeom::BBox
CQChartsSummaryPlot::
fitBBox() const
{
  auto font = view()->viewFont(this->font());

  auto tsize = CQChartsDrawUtil::calcTextSize("X", font, CQChartsTextOptions());

  auto thw = pixelToWindowWidth (tsize.height());
  auto thh = pixelToWindowHeight(tsize.height());

  //---

  double maxWidth = 0.0;

  int nc = visibleColumns().count();

  for (int ic = 0; ic < nc; ++ic) {
    auto column = visibleColumns().getColumn(ic);

    bool ok;
    auto str = modelHHeaderString(column, ok);

    auto font = view()->viewFont(xLabelTextFont());

    auto tsize = CQChartsDrawUtil::calcTextSize(str, font, CQChartsTextOptions());

    maxWidth = std::max(maxWidth, pixelToWindowWidth(tsize.width()));
  }

  if (maxWidth > 1.0)
    maxWidth = 1.0;

  //---

  auto bbox = CQChartsPlot::fitBBox();

  bbox += Point(-maxWidth - thw/2.0, -thh - thh/2.0);

  return bbox;
}

//-----

void
CQChartsSummaryPlot::
resetColumnVisible()
{
  columnVisible_.clear();
}

bool
CQChartsSummaryPlot::
isColumnVisible(int ic) const
{
  auto p = columnVisible_.find(ic);
  if (p == columnVisible_.end()) return true;

  return (*p).second;
}

void
CQChartsSummaryPlot::
setColumnVisible(int ic, bool visible)
{
  columnVisible_[ic] = visible;

  if (! isExpanded() && (plotType() == PlotType::PARALLEL))
    parallelPlot_->setYColumnVisible(ic, visible);

  updateRangeAndObjs();

  Q_EMIT customDataChanged();
}

//------

void
CQChartsSummaryPlot::
calcBucketCounts(int ic, BucketCount &bucketCount, int &maxCount,
                 double &rmin, double &rmax) const
{
  auto column = visibleColumns().getColumn(ic);

  calcBucketCounts(column, bucketCount, maxCount, rmin, rmax);
}

void
CQChartsSummaryPlot::
calcBucketCounts(const Column &column, BucketCount &bucketCount, int &maxCount,
                 double &rmin, double &rmax) const
{
  bucketCount = BucketCount();
  maxCount    = 0;
  rmin        = 0.0;
  rmax        = 0.0;

  auto *details = columnDetails(column);
  if (! details) return;

  bool ok;
  double min = CQChartsVariant::toReal(details->minValue(), ok);
  double max = CQChartsVariant::toReal(details->maxValue(), ok);

  int n = details->numRows();

  for (uint i = 0; i < uint(n); ++i) {
    auto value = details->value(i);

    int bucket = details->bucket(value);

    ++bucketCount[bucket];
  }

  maxCount = 0;

  rmin = min;
  rmax = max;

  for (const auto &pb : bucketCount) {
    maxCount = std::max(maxCount, pb.second);

    QVariant vmin, vmax;

    details->bucketRange(pb.first, vmin, vmax);

    bool ok;
    double rmin1 = CQChartsVariant::toReal(vmin, ok);
    double rmax1 = CQChartsVariant::toReal(vmax, ok);

    rmin = std::min(rmin, rmin1);
    rmax = std::max(rmax, rmax1);
  }
}

void
CQChartsSummaryPlot::
calcValueCounts(int ic, ValueCounts &valueCounts, int &maxCount) const
{
  auto column = visibleColumns().getColumn(ic);

  calcValueCounts(column, valueCounts, maxCount);
}

void
CQChartsSummaryPlot::
calcValueCounts(const Column &column, ValueCounts &valueCounts, int &maxCount) const
{
  valueCounts = ValueCounts();
  maxCount    = 0;

  auto *details = columnDetails(column);
  if (! details) return;

  maxCount = 0;

  valueCounts = details->uniqueValueCounts();

  for (const auto &vc : valueCounts)
    maxCount = std::max(maxCount, vc.second);
}

//------

bool
CQChartsSummaryPlot::
handleSelectDoubleClick(const Point &p, SelMod /*selMod*/)
{
  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs, Constraints::SELECTABLE);

  CQChartsSummaryCellObj *cellObj = nullptr;

  for (auto *plotObj : plotObjs) {
    cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (cellObj) break;
  }

  if (cellObj)
    expandCell(cellObj);

  return true;
}

//------

CQChartsSummaryCellObj *
CQChartsSummaryPlot::
createCellObj(const BBox &bbox, int row, int col) const
{
  return new CQChartsSummaryCellObj(this, bbox, row, col);
}

//---

CQChartsPlotCustomControls *
CQChartsSummaryPlot::
createCustomControls()
{
  auto *controls = new CQChartsSummaryPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsSummaryCellObj::
CQChartsSummaryCellObj(const SummaryPlot *summaryPlot, const BBox &bbox, int row, int col) :
 CQChartsPlotObj(const_cast<SummaryPlot *>(summaryPlot), bbox, ColorInd(), ColorInd(), ColorInd()),
 summaryPlot_(summaryPlot), row_(row), col_(col)
{
}

CQChartsColumn
CQChartsSummaryCellObj::
rowColumn() const
{
  return visibleColumn(row_);
}

CQChartsColumn
CQChartsSummaryCellObj::
colColumn() const
{
  return visibleColumn(col_);
}

CQChartsColumn
CQChartsSummaryCellObj::
visibleColumn(int i) const
{
  return summaryPlot_->visibleColumns().getColumn(i);
}

QString
CQChartsSummaryCellObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(row_).arg(col_);
}

QString
CQChartsSummaryCellObj::
calcTipId() const
{
  auto cellType = getCellType();

  CQChartsTableTip tableTip;

  // off diagonal
  if (row_ != col_) {
    // column info
    auto column1 = rowColumn();
    auto column2 = colColumn();

    bool ok;
    auto xtip = summaryPlot_->modelHHeaderTip(column1, ok);
    auto ytip = summaryPlot_->modelHHeaderTip(column2, ok);

    tableTip.addTableRow("X Column", xtip);
    tableTip.addTableRow("Y Column", ytip);

    //---

    // data details
    auto *details1 = summaryPlot_->columnDetails(column1);
    auto *details2 = summaryPlot_->columnDetails(column2);

    if (details1 && details2 && details1->isNumeric() && details2->isNumeric()) {
      if (! summaryPlot_->groupColumn().isValid()) {
        const auto &model = summaryPlot_->currentModel();

        auto correlation = summaryPlot_->modelDetails(model)->correlation(column1, column2);

        tableTip.addTableRow("Correlation", correlation);
      }
      else {
        auto *groupDetails = summaryPlot_->columnDetails(summaryPlot_->groupColumn());

        const_cast<CQChartsSummaryCellObj *>(this)->initGroupedValues();

        for (const auto &pg : groupValues_.groupIndData) {
          int         ig      = pg.first;
          const auto &indData = pg.second;

          if (indData.x.size() == indData.y.size()) {
            auto correlation = CMathCorrelation::calc(indData.x, indData.y);

            auto groupVar = (groupDetails ? groupDetails->uniqueValue(ig) : QVariant());

            tableTip.addTableRow(QString("Correlation (%1)").arg(groupVar.toString()), correlation);
          }
        }
      }
    }
  }
  // diagonal
  else {
    // column info
    auto column = rowColumn();

    bool ok;
    auto tip = summaryPlot_->modelHHeaderTip(column, ok);

    tableTip.addTableRow("Column", tip);

    //---

    // data details
    auto *details = summaryPlot_->columnDetails(column);

    if (details) {
      tableTip.addTableRow("Num Values", details->valueCount());
      tableTip.addTableRow("Num Unique", details->numUnique());
      tableTip.addTableRow("Max Unique", details->maxUnique());
      tableTip.addTableRow("Num Null"  , details->numNull());

      if (details->isNumeric()) {
        tableTip.addTableRow("Min"   , details->minValue());
        tableTip.addTableRow("Max"   , details->maxValue());
        tableTip.addTableRow("Mean"  , details->meanValue());
        tableTip.addTableRow("StdDev", details->stdDevValue());
      }
    }

    //---

    // plot data
    if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION)
      tableTip.addTableRow("Count Max", maxCount_);
  }

  return tableTip.str();
}

void
CQChartsSummaryCellObj::
draw(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::draw");

  //---

  bx_ = summaryPlot_->lengthPlotWidth (summaryPlot_->border());
  by_ = summaryPlot_->lengthPlotHeight(summaryPlot_->border());

  auto bbox = this->rect();

  pxmin_ = bbox.getXMin() + bx_;
  pymin_ = bbox.getYMin() + bx_;
  pxmax_ = bbox.getXMax() - by_;
  pymax_ = bbox.getYMax() - by_;

  //---

  rangeBox_ = BBox();

  if (row_ != col_) {
    auto column1 = rowColumn();
    auto column2 = colColumn();

    auto range1 = summaryPlot_->columnRange(column1);
    auto range2 = summaryPlot_->columnRange(column2);

    if (range1.isSet() || range2.isSet()) {
      rangeBox_ = BBox(xmin_, ymin_, xmax_, ymax_);

      if (range1.isSet()) {
        rangeBox_.setXMin(range1.min());
        rangeBox_.setXMax(range1.max());
      }

      if (range2.isSet()) {
        rangeBox_.setYMin(range2.min());
        rangeBox_.setYMax(range2.max());
      }
    }
  }
  else {
    auto column1 = rowColumn();

    auto range1 = summaryPlot_->columnRange(column1);

    if (range1.isSet()) {
      rangeBox_ = BBox(xmin_, ymin_, xmax_, ymax_);

      if (range1.isSet()) {
        rangeBox_.setXMin(range1.min());
        rangeBox_.setXMax(range1.max());
      }
    }
  }

  //---

  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawRect(bbox);

  //---

  auto cellType = getCellType();

  if      (cellType == CQChartsSummaryPlot::CellType::SCATTER)
    drawScatter(device);
  else if (cellType == CQChartsSummaryPlot::CellType::CORRELATION)
    drawCorrelation(device);
  else if (cellType == CQChartsSummaryPlot::CellType::BOXPLOT)
    drawBoxPlot(device);
  else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION)
    drawDistribution(device);
  else if (cellType == CQChartsSummaryPlot::CellType::PIE)
    drawPie(device);
}

CQChartsSummaryPlot::CellType
CQChartsSummaryCellObj::
getCellType() const
{
  if (row_ != col_) {
    bool lower = (row_ > col_);

    auto isOffDiagonalType = [&](CQChartsSummaryPlot::OffDiagonalType type) {
      return ((  lower && summaryPlot_->lowerDiagonalType() == type) ||
              (! lower && summaryPlot_->upperDiagonalType() == type));
    };

    bool isScatter     = isOffDiagonalType(CQChartsSummaryPlot::OffDiagonalType::SCATTER);
    bool isCorrelation = isOffDiagonalType(CQChartsSummaryPlot::OffDiagonalType::CORRELATION);

    if      (isScatter)
      return CQChartsSummaryPlot::CellType::SCATTER;
    else if (isCorrelation)
      return CQChartsSummaryPlot::CellType::CORRELATION;
  }
  else {
    if      (summaryPlot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::BOXPLOT)
      return CQChartsSummaryPlot::CellType::BOXPLOT;
    else if (summaryPlot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::DISTRIBUTION)
      return CQChartsSummaryPlot::CellType::DISTRIBUTION;
    else if (summaryPlot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::PIE)
      return CQChartsSummaryPlot::CellType::PIE;
  }

  return CQChartsSummaryPlot::CellType::NONE;
}

void
CQChartsSummaryCellObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto bg = summaryPlot_->interpInterfaceColor(0.15);
  auto fg = summaryPlot_->interpInterfaceColor(1.00);

  summaryPlot_->setPenBrush(penBrush, PenData(true, fg, Alpha(0.3)), BrushData(true, bg));

  if (updateState)
    summaryPlot_->updateObjPenBrushState(this, penBrush, drawType());
}

void
CQChartsSummaryCellObj::
drawScatter(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawScatter");

  //---

  auto column1 = rowColumn();
  auto column2 = colColumn();

  auto *details1 = summaryPlot_->columnDetails(column1);
  auto *details2 = summaryPlot_->columnDetails(column2);
  if (! details1 || ! details2) return;

  auto *groupDetails = (summaryPlot_->groupColumn().isValid() ?
    summaryPlot_->columnDetails(summaryPlot_->groupColumn()) : nullptr);

  int ng = (groupDetails ? groupDetails->numUnique() : 0);

  //---

  int nc = summaryPlot_->visibleColumns().count();

  if (details1->isNumeric()) {
    bool ok;
    xmin_ = CQChartsVariant::toReal(details1->minValue(), ok);
    xmax_ = CQChartsVariant::toReal(details1->maxValue(), ok);
  }
  else {
    xmin_ = 0.0;
    xmax_ = details1->numUnique();
  }

  if (details2->isNumeric()) {
    bool ok;
    ymin_ = CQChartsVariant::toReal(details2->minValue(), ok);
    ymax_ = CQChartsVariant::toReal(details2->maxValue(), ok);
  }
  else {
    ymin_ = 0.0;
    ymax_ = details2->numUnique();
  }

  int nx = details1->numRows();
  int ny = details2->numRows();
  if (nx != ny) return;

  //---

  auto symbol     = summaryPlot_->scatterSymbol();
  auto symbolSize = summaryPlot_->calcScatterSymbolSize();

  PenBrush penBrush;

  ColorInd colorInd(row_, nc);

  summaryPlot_->setScatterSymbolPenBrush(penBrush, colorInd);

  poly_ = Polygon();

  for (uint i = 0; i < uint(nx); ++i) {
    // get scatter point
    double x = 0.0, y = 0.0;

    if (details1->isNumeric()) {
      bool ok;
      x = CQChartsVariant::toReal(details1->value(i), ok);
      if (! ok) continue;
    }
    else
      x = details1->uniqueId(details1->value(i));

    if (details2->isNumeric()) {
      bool ok;
      y = CQChartsVariant::toReal(details2->value(i), ok);
      if (! ok) continue;
    }
    else
      y = details2->uniqueId(details2->value(i));

    auto p = Point(x, y);

    //---

    bool selected = (rangeBox_.isSet() && rangeBox_.inside(p));

    //---

    // set pen/brush
    PenBrush       penBrush1;
    CQChartsSymbol symbol1;

    if (groupDetails) {
      auto var = groupDetails->value(i);

      int ig = groupDetails->uniqueId(var);

      ColorInd colorInd1(ig, ng);

      summaryPlot_->setScatterSymbolPenBrush(penBrush1, colorInd1);

      // get index symbol
      auto *symbolSet = summaryPlot_->defaultSymbolSet();

      symbol1 = symbolSet->interpI(ig).symbol;
    }
    else {
      penBrush1 = penBrush;
      symbol1   = symbol;
    }

    //---

    auto ps = plotToParent(p);

    CQChartsDrawUtil::setPenBrush(device, penBrush1);

    CQChartsDrawUtil::drawSymbol(device, penBrush1, symbol1, ps, symbolSize, /*scale*/true);


    //---

    // highlight selected
    if (selected) {
      auto penBrush2 = penBrush1;

      summaryPlot_->updatePenBrushState(ColorInd(), penBrush2, /*selected*/true, /*inside*/false);

      CQChartsDrawUtil::setPenBrush(device, penBrush2);

      double ss = 0.75*summaryPlot_->lengthPixelWidth(symbolSize);

      auto pps = summaryPlot_->windowToPixel(ps);

      auto pbbox = BBox(pps.x - ss, pps.y - ss, pps.x + ss, pps.y + ss);

      CQChartsDrawUtil::drawSelectedOutline(device, summaryPlot_->pixelToWindow(pbbox));
    }

    //---

    poly_.addPoint(Point(x, y)); // untransformed point
  }

  //---

  if (summaryPlot_->isBestFit())
    drawBestFit(device);

  //---

  if (rangeBox_.isSet()) {
    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->setBrush(QBrush(Qt::NoBrush));

    auto rbbox = BBox(plotToParent(rangeBox_.getLL()), plotToParent(rangeBox_.getUR()));

    device->drawRect(rbbox);
  }
}

void
CQChartsSummaryCellObj::
drawBestFit(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawBestFit");

  //---

  auto bbox = this->rect();

  auto drawFitPoly = [&](const Polygon &polygon) {
    auto pbbox = polygon.boundingBox();
    if (! pbbox.isValid()) return;

    CQChartsFitData fitData;

    fitData.calc(polygon, 3);

    double dx = pbbox.getWidth()/100.0;

    Polygon fitPoly;

    for (int i = 0; i <= 100; ++i) {
      double x = pbbox.getXMin() + i*dx;
      double y = fitData.interp(x);

      auto p = plotToParent(Point(x, y));

      fitPoly.addPoint(p);
    }

    //---

    device->save();

    auto path = CQChartsDrawUtil::polygonToPath(fitPoly, /*closed*/false);

    device->setClipRect(bbox);

    device->strokePath(path, device->pen());

    device->restore();
  };

  if (! summaryPlot_->groupColumn().isValid()) {
    drawFitPoly(poly_);
  }
  else {
    const_cast<CQChartsSummaryCellObj *>(this)->initGroupedValues();

    for (const auto &pg : groupValues_.groupIndData) {
      const auto &indData = pg.second;

      drawFitPoly(indData.poly);
    }
  }
}

void
CQChartsSummaryCellObj::
drawCorrelation(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawCorrelation");

  //---

  auto column1 = rowColumn();
  auto column2 = colColumn();

  auto *details1 = summaryPlot_->columnDetails(column1);
  auto *details2 = summaryPlot_->columnDetails(column2);
  if (! details1 || ! details2) return;

  //---

  summaryPlot_->setPainterFont(device, summaryPlot_->correlationTextFont());

  //---

  PenBrush tpenBrush;

  summaryPlot_->setCorrelationTextPenBrush(tpenBrush, ColorInd());

  CQChartsDrawUtil::setPenBrush(device, tpenBrush);

  //---

  auto textOptions = summaryPlot_->correlationTextOptions(device);

  textOptions.angle     = CQChartsAngle();
  textOptions.formatted = false;
  textOptions.scaled    = false;
  textOptions.html      = false;

  //---

  if (details1->isNumeric() && details2->isNumeric()) {
    auto *drawObj = new CQChartsDrawObj;

    double bx = summaryPlot_->pixelToWindowWidth (2);
    double by = summaryPlot_->pixelToWindowHeight(2);

    double x = bx;
    double y = by;

    if (! summaryPlot_->groupColumn().isValid()) {
      const auto &model = summaryPlot_->currentModel();

      auto correlation = summaryPlot_->modelDetails(model)->correlation(column1, column2);

      auto cstr = CQChartsUtil::realToString(correlation, 5);

      auto pt = Point(x, y);

      auto *drawText = new CQChartsDrawText(pt, cstr, device->font(), tpenBrush,
                                            textOptions, /*scale*/false);

      drawObj->addChild(drawText);
    }
    else {
      const_cast<CQChartsSummaryCellObj *>(this)->initGroupedValues();

      int ng = int(groupValues_.groupIndData.size());

      QFontMetricsF fm(device->font());

      double dx = summaryPlot_->pixelToWindowWidth (fm.height());
      double dy = summaryPlot_->pixelToWindowHeight(fm.height());

      y += double(groupValues_.groupIndData.size() - 1)*(dy + by);

      for (const auto &pg : groupValues_.groupIndData) {
        int         ig      = pg.first;
        const auto &indData = pg.second;

        PenBrush rpenBrush;

        auto fc = summaryPlot_->interpPaletteColor(ColorInd(ig, ng));

        summaryPlot_->setPenBrush(rpenBrush, PenData(false), BrushData(true, fc));

        auto *drawRect = new CQChartsDrawRect(BBox(x, y, x + dx, y + dy), rpenBrush);

        drawObj->addChild(drawRect);

        //---

        if (indData.x.size() == indData.y.size()) {
          auto correlation = CMathCorrelation::calc(indData.x, indData.y);

          auto cstr = CQChartsUtil::realToString(correlation, 5);

          auto pt = Point(x + dx + 2*bx, y + dy/2.0);

          auto *drawText = new CQChartsDrawText(pt, cstr, device->font(), tpenBrush,
                                                textOptions, /*scale*/false);

          drawObj->addChild(drawText);
        }

        //---

        y -= dy + by;
      }
    }

    //---

    BBox pbbox(pxmin_, pymin_, pxmax_, pymax_);

    drawObj->setMaxScale(1.5);

    drawObj->place(device, pbbox);

    drawObj->draw(device);

    delete drawObj;
  }
}

void
CQChartsSummaryCellObj::
drawBoxPlot(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawBoxPlot");

  //---

  auto column = rowColumn();

  auto *details = summaryPlot_->columnDetails(column);
  if (! details) return;

  BBox bbox(pxmin_, pymin_, pxmax_, pymax_);

  int nc = summaryPlot_->visibleColumns().count();

  //---

  PenBrush penBrush;

  ColorInd colorInd(row_, nc);

  auto bc = summaryPlot_->interpBoxPlotStrokeColor(colorInd);
  auto fc = summaryPlot_->interpBoxPlotFillColor  (colorInd);

  summaryPlot_->setPenBrush(penBrush,
    summaryPlot_->boxPlotPenData(bc), summaryPlot_->boxPlotBrushData(fc));

  //---

  if (details->isNumeric()) {
    bool ok;
    double min  = CQChartsVariant::toReal(details->minValue (), ok);
    double max  = CQChartsVariant::toReal(details->maxValue (), ok);
    double mean = CQChartsVariant::toReal(details->meanValue(), ok);

    int n = details->numRows();

    CQChartsBoxWhisker whisker;

    for (uint i = 0; i < uint(n); ++i) {
      bool ok;
      double r = CQChartsVariant::toReal(details->value(i), ok);
      if (! ok) continue;

      double r1 = CMathUtil::map(r, min, max, pymin_, pymax_);

      whisker.addValue(r1);
    }

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsBoxWhiskerUtil::DrawData  drawData;
    CQChartsBoxWhiskerUtil::PointData pointData;

    drawData.width       = Length::plot(0.1);
    drawData.orientation = Qt::Vertical;
    drawData.median      = true;

    CQChartsBoxWhiskerUtil::drawWhisker(device, whisker, bbox, drawData, pointData);

    //---

    if (summaryPlot_->isBoxPlotTextVisible()) {
      summaryPlot_->setPainterFont(device, summaryPlot_->boxPlotTextFont());

      //---

      PenBrush tpenBrush;

      summaryPlot_->setBoxPlotTextPenBrush(tpenBrush, ColorInd());

      CQChartsDrawUtil::setPenBrush(device, tpenBrush);

      //---

      auto textOptions = summaryPlot_->boxPlotTextOptions(device);

      textOptions.angle     = CQChartsAngle();
      textOptions.formatted = false;
      textOptions.scaled    = false;
      textOptions.html      = false;

      textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
      CQChartsDrawUtil::drawTextAtPoint(device, pointData.min, QString::number(min ), textOptions);

      textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;
      CQChartsDrawUtil::drawTextAtPoint(device, pointData.med, QString::number(mean), textOptions);
      CQChartsDrawUtil::drawTextAtPoint(device, pointData.max, QString::number(max ), textOptions);
    }

    //---

    bmin_ = min;
    bmax_ = max;
  }
  else {
    CQChartsTextOptions options;

    CQChartsDrawUtil::drawTextInBox(device, bbox, "N/A", options);

    bmin_ = 0.0;
    bmax_ = 1.0;

    return;
  }

  //---

  if (rangeBox_.isSet()) {
    double ymin = CMathUtil::map(rangeBox_.getXMin(), bmin_, bmax_, 0.0, 1.0);
    double ymax = CMathUtil::map(rangeBox_.getXMax(), bmin_, bmax_, 0.0, 1.0);

    auto brect = BBox(0.0, ymin, 1.0, ymax);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->setBrush(QBrush(Qt::NoBrush));

    auto rbbox = BBox(plotToParent(brect.getLL()), plotToParent(brect.getUR()));

    device->drawRect(rbbox);
  }
}

void
CQChartsSummaryCellObj::
drawDistribution(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawDistribution");

  //---

  auto column = rowColumn();

  auto *details = summaryPlot_->columnDetails(column);
  if (! details) return;

  bool isGroup = (summaryPlot_->groupColumn() == column);

  int nc = summaryPlot_->visibleColumns().count();

  //---

  PenBrush penBrush;

  ColorInd colorInd(row_, nc);

  auto bc = summaryPlot_->interpDistributionStrokeColor(colorInd);
  auto fc = summaryPlot_->interpDistributionFillColor  (colorInd);

  summaryPlot_->setPenBrush(penBrush,
    summaryPlot_->distributionPenData(bc), summaryPlot_->distributionBrushData(fc));

  //---

  auto drawRect = [&](const BBox &bbox) {
    auto pw = device->windowToSignedPixelWidth(bbox.getWidth());

    if (pw <= 2) {
      auto xm = bbox.getXMid();

      device->setPen(device->brush().color());

      device->drawLine(Point(xm, bbox.getYMin()), Point(xm, bbox.getYMax()));
    }
    else
      device->drawRect(bbox);
  };

  if (details->isNumeric()) {
    CQChartsSummaryPlot::BucketCount bucketCount;

    summaryPlot_->calcBucketCounts(row_, bucketCount, maxCount_, bmin_, bmax_);

    int ig = 0;
    int ng = int(bucketCount.size());

    for (const auto &pb : bucketCount) {
      PenBrush penBrush1;

      if (isGroup) {
        ColorInd colorInd(ig, ng);

        auto fc1 = summaryPlot_->interpDistributionFillColor(colorInd);

        summaryPlot_->setPenBrush(penBrush1,
          summaryPlot_->distributionPenData(bc), summaryPlot_->distributionBrushData(fc1));
      }
      else {
        penBrush1 = penBrush;
      }

      //---

      QVariant vmin, vmax;

      details->bucketRange(pb.first, vmin, vmax);

      bool ok;
      double rmin1 = CQChartsVariant::toReal(vmin, ok);
      double rmax1 = CQChartsVariant::toReal(vmax, ok);

      int n = pb.second;

      double x1 = CMathUtil::map(rmin1, bmin_, bmax_, pxmin_, pxmax_);
      double x2 = CMathUtil::map(rmax1, bmin_, bmax_, pxmin_, pxmax_);

      double y1 = CMathUtil::map(0, 0, maxCount_, pymin_, pymax_);
      double y2 = CMathUtil::map(n, 0, maxCount_, pymin_, pymax_);

      BBox bbox(x1, y1, x2, y2);

      CQChartsDrawUtil::setPenBrush(device, penBrush1);

      drawRect(bbox);

      //--

      ++ig;
    }

    //---

    if (summaryPlot_->isDensity())
      drawDensity(device);
  }
  else {
    CQChartsSummaryPlot::ValueCounts valueCounts;

    summaryPlot_->calcValueCounts(row_, valueCounts, maxCount_);

    int nc = int(valueCounts.size());

    double dx = (nc > 0 ? (pxmax_ - pxmin_)/nc : 0.0);

    double x = pxmin_;

    for (const auto &vc : valueCounts) {
      PenBrush penBrush1;

      if (isGroup) {
        int ig = details->uniqueId(vc.first);

        ColorInd colorInd(ig, nc);

        auto fc1 = summaryPlot_->interpDistributionFillColor(colorInd);

        summaryPlot_->setPenBrush(penBrush1,
          summaryPlot_->distributionPenData(bc), summaryPlot_->distributionBrushData(fc1));
      }
      else {
        penBrush1 = penBrush;
      }

      auto r = CMathUtil::map(vc.second, 0, maxCount_, pymin_, pymax_);

      BBox bbox(x, pymin_, x + dx, r);

      CQChartsDrawUtil::setPenBrush(device, penBrush1);

      drawRect(bbox);

      x += dx;
    }
  }

  //---

  if (rangeBox_.isSet()) {
    double xmin = CMathUtil::map(rangeBox_.getXMin(), bmin_, bmax_, 0.0, 1.0);
    double xmax = CMathUtil::map(rangeBox_.getXMax(), bmin_, bmax_, 0.0, 1.0);

    auto brect = BBox(xmin, 0.0, xmax, 1.0);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->setBrush(QBrush(Qt::NoBrush));

    auto rbbox = BBox(plotToParent(brect.getLL()), plotToParent(brect.getUR()));

    device->drawRect(rbbox);
  }
}

void
CQChartsSummaryCellObj::
drawDensity(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawDensity");

  //---

  auto bbox = this->rect();

  //---

  auto column = rowColumn();

  auto *details = summaryPlot_->columnDetails(column);
  if (! details) return;

  //---

  auto pc = summaryPlot_->interpInterfaceColor(1.0);

  CQChartsDensity density;

  density.setDrawType(CQChartsDensity::DrawType::DISTRIBUTION);

  CQChartsDensity::XVals xvals;

  int n = details->numRows();

  for (uint i = 0; i < uint(n); ++i) {
    auto value = details->value(i);

    bool ok;
    double r = CQChartsVariant::toReal(value, ok);
    if (! ok) continue;

    double x = CMathUtil::map(r, bmin_, bmax_, pxmin_, pxmax_);

    xvals.push_back(x);
  }

  density.setXVals(xvals);

  //---

  PenBrush penBrush;

  // TODO: condig ?
  summaryPlot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.5)), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->save();

  device->setClipRect(bbox);

  CQChartsDensity::DrawData drawData;

  drawData.scaled = true;

  density.draw(summaryPlot_, device, bbox, drawData);

  device->restore();
}

void
CQChartsSummaryCellObj::
drawPie(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawPie");

  //---

  auto column = rowColumn();

  auto *details = summaryPlot_->columnDetails(column);
  if (! details) return;

  int nc = summaryPlot_->visibleColumns().count();

  //---

  PenBrush penBrush;

  ColorInd colorInd(row_, nc);

  auto bc = summaryPlot_->interpPieStrokeColor(colorInd);
  auto fc = summaryPlot_->interpPieFillColor  (colorInd);

  summaryPlot_->setPenBrush(penBrush,
    summaryPlot_->piePenData(bc), summaryPlot_->pieBrushData(fc));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  if (details->isNumeric()) {
    CQChartsSummaryPlot::BucketCount bucketCount;
    int                              maxCount = 0;

    summaryPlot_->calcBucketCounts(row_, bucketCount, maxCount, bmin_, bmax_);

    CQChartsRValues values;

    for (const auto &pb : bucketCount)
      values.addValue(CQChartsRValues::OptReal(pb.second));

    auto bbox1 = BBox(pxmin_, pymin_, pxmax_, pymax_);

    CQChartsPlotDrawUtil::drawPie(const_cast<CQChartsSummaryPlot *>(summaryPlot_), device,
                                  values, bbox1, summaryPlot_->defaultPalette(), penBrush);
  }
  else {
    BBox bbox(pxmin_, pymin_, pxmax_, pymax_);

    CQChartsTextOptions options;

    CQChartsDrawUtil::drawTextInBox(device, bbox, "N/A", options);

    bmin_ = 0.0;
    bmax_ = 1.0;
  }
}

CQChartsGeom::Point
CQChartsSummaryCellObj::
plotToParent(const Point &w) const
{
  auto px = CMathUtil::map(w.x, xmin_, xmax_, pxmin_, pxmax_);
  auto py = CMathUtil::map(w.y, ymin_, ymax_, pymin_, pymax_);

  return Point(px, py);
}

CQChartsGeom::Point
CQChartsSummaryCellObj::
parentToPlot(const Point &p) const
{
  auto cellType = getCellType();

  auto r = this->rect();

  auto wx = CMathUtil::map(p.x, r.getXMin(), r.getXMax(), 0.0, 1.0);
  auto wy = CMathUtil::map(p.y, r.getYMin(), r.getYMax(), 0.0, 1.0);

  if      (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    auto bx = CMathUtil::map(wx, bx_, 1.0 - bx_, bmin_, bmax_);
    auto by = CMathUtil::map(wy, by_, 1.0 - by_, 0, maxCount_);

    return Point(bx, by);
  }
  else if (cellType == CQChartsSummaryPlot::CellType::BOXPLOT) {
    auto py = CMathUtil::map(wy, by_, 1.0 - by_, bmin_, bmax_);

    return Point(wx, py);
  }
  else {
    auto px = CMathUtil::map(wx, bx_, 1.0 - bx_, xmin_, xmax_);
    auto py = CMathUtil::map(wy, by_, 1.0 - by_, ymin_, ymax_);

    return Point(px, py);
  }
}

void
CQChartsSummaryCellObj::
initGroupedValues()
{
  CQPerfTrace trace("CQChartsSummaryCellObj::initGroupedValues");

  //---

  if (groupValues_.set)
    return;

  groupValues_.set = true;

  if (! summaryPlot_->groupColumn().isValid())
    return;

  auto *groupDetails = summaryPlot_->columnDetails(summaryPlot_->groupColumn());
  if (! groupDetails) return;

  int nr = groupDetails->numRows();

  if (row_ != col_) {
    auto column1 = rowColumn();
    auto column2 = colColumn();

    auto *details1 = summaryPlot_->columnDetails(column1);
    auto *details2 = summaryPlot_->columnDetails(column2);
    if (! details1 || ! details2) return;

    for (uint ir = 0; ir < uint(nr); ++ir) {
      auto var = groupDetails->value(ir);

      int ig = groupDetails->uniqueId(var);

      auto &indData = groupValues_.groupIndData[ig];

      auto value1 = details1->value(ir);
      auto value2 = details2->value(ir);

      indData.xvals.push_back(value1);
      indData.yvals.push_back(value2);

      double x = 0.0, y = 0.0;

      if (details1->isNumeric()) {
        bool ok;
        x = CQChartsVariant::toReal(value1, ok);

        indData.x.push_back(x);
      }

      if (details2->isNumeric()) {
        bool ok;
        y = CQChartsVariant::toReal(value2, ok);

        indData.y.push_back(y);
      }

      indData.poly.addPoint(Point(x, y));
    }
  }
  else {
    auto column = rowColumn();

    auto *details = summaryPlot_->columnDetails(column);
    if (! details) return;

    for (uint ir = 0; ir < uint(nr); ++ir) {
      auto var = groupDetails->value(ir);

      int ig = groupDetails->uniqueId(var);

      auto &indData = groupValues_.groupIndData[ig];

      auto value = details->value(ir);

      indData.xvals.push_back(value);
      indData.yvals.push_back(value);

      if (details->isNumeric()) {
        bool ok;
        double r = CQChartsVariant::toReal(value, ok);

        indData.x.push_back(r);
        indData.y.push_back(r);

        indData.poly.addPoint(Point(r, r));
      }
    }
  }
}

//------

CQChartsSummaryPlotGroupStats::
CQChartsSummaryPlotGroupStats(CQChartsSummaryPlot *summaryPlot) :
 summaryPlot_(summaryPlot)
{
  setObjectName("groupStats");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  valueList_ = CQUtil::makeWidget<CQTableWidget>("valueList");

  layout->addWidget(valueList_);
}

void
CQChartsSummaryPlotGroupStats::
updateWidgets()
{
  auto groupColumn = (summaryPlot_ ? summaryPlot_->groupColumn() : CQChartsColumn());

  setVisible(groupColumn.isValid());

  valueList_->clear();

  if (! groupColumn.isValid())
    return;

  auto *groupDetails = summaryPlot_->columnDetails(summaryPlot_->groupColumn());
  if (! groupDetails) return;

  //---

  auto createHeaderItem = [&](int c, const QString &name) {
    valueList_->setHorizontalHeaderItem(c, new QTableWidgetItem(name));
  };

  valueList_->setColumnCount(3);

  createHeaderItem(0, "Color");
  createHeaderItem(1, "Value");
  createHeaderItem(2, "Count");

  //---

  auto createColorTableItem = [&](const QColor &color) {
    auto *item = new CQTableWidgetColorItem(valueList_, color);

    item->setToolTip(color.name());
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    return item;
  };

  auto createStringTableItem = [&](const QString &str) {
    auto *item = new QTableWidgetItem(str);

    item->setToolTip(str);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    return item;
  };

  if (groupDetails->isNumeric()) {
    CQChartsSummaryPlot::BucketCount bucketCount;
    int                              maxCount = 0;
    double                           bmin, bmax;

    summaryPlot_->calcBucketCounts(summaryPlot_->groupColumn(), bucketCount,
                                   maxCount, bmin, bmax);

    int ig = 0;
    int ng = int(bucketCount.size());

    valueList_->setRowCount(ng);

    for (const auto &bc : bucketCount) {
      QVariant vmin, vmax;

      groupDetails->bucketRange(bc.first, vmin, vmax);

      bool ok;
      auto rmin = CQChartsVariant::toReal(vmin, ok);
      auto rmax = CQChartsVariant::toReal(vmax, ok);

      auto c = summaryPlot_->interpPaletteColor(CQChartsUtil::ColorInd(ig, ng));

      auto *colorItem = createColorTableItem(c);
      auto *valueItem = createStringTableItem(QString("[%1,%2)").arg(rmin).arg(rmax));
      auto *countItem = createStringTableItem(QString::number(bc.second));

      valueList_->setItem(ig, 0, colorItem);
      valueList_->setItem(ig, 1, valueItem);
      valueList_->setItem(ig, 2, countItem);

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupDetails->numUnique();

    valueList_->setRowCount(ng);

    for (const auto &valueCount : groupDetails->uniqueValueCounts()) {
      auto c = summaryPlot_->interpPaletteColor(CQChartsUtil::ColorInd(ig, ng));

      auto *colorItem = createColorTableItem(c);
      auto *valueItem = createStringTableItem(valueCount.first.toString());
      auto *countItem = createStringTableItem(QString::number(valueCount.second));

      valueList_->setItem(ig, 0, colorItem);
      valueList_->setItem(ig, 1, valueItem);
      valueList_->setItem(ig, 2, countItem);

      ++ig;
    }
  }

  setMinimumHeight(sizeHint().height());

  valueList_->fixTableColumnWidths();
}

QSize
CQChartsSummaryPlotGroupStats::
sizeHint() const
{
  QFontMetrics fm(font());

  int nr = std::min(valueList_->rowCount() + 1, 6);

  return QSize(fm.horizontalAdvance("X")*40, (fm.height() + 6)*nr + 8);
}

//------

CQChartsSummaryPlotColumnChooser::
CQChartsSummaryPlotColumnChooser(CQChartsSummaryPlot *plot) :
 CQChartsPlotColumnChooser(plot)
{
}

const CQChartsColumns &
CQChartsSummaryPlotColumnChooser::
getColumns() const
{
  auto *plot = dynamic_cast<CQChartsSummaryPlot *>(this->plot());
  assert(plot);

  return plot->columns();
}

bool
CQChartsSummaryPlotColumnChooser::
isColumnVisible(int ic) const
{
  auto *plot = dynamic_cast<CQChartsSummaryPlot *>(this->plot());

  return (plot ? plot->isColumnVisible(ic) : false);
}

void
CQChartsSummaryPlotColumnChooser::
setColumnVisible(int ic, bool visible)
{
  auto *plot = dynamic_cast<CQChartsSummaryPlot *>(this->plot());

  if (plot)
    plot->setColumnVisible(ic, visible);
}

//------

CQChartsSummaryPlotCustomControls::
CQChartsSummaryPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "summary")
{
}

void
CQChartsSummaryPlotCustomControls::
init()
{
  addWidgets();

  connectSlots(true);
}

void
CQChartsSummaryPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addGroupStatsWidgets();

  addOptionsWidgets();

  addOverview();

  addLayoutStretch();

  addExpandControls();
}

void
CQChartsSummaryPlotCustomControls::
addColumnWidgets()
{
  // columns frame
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addNamedColumnWidgets(QStringList() << "columns", columnsFrame);

  chooser_ = new CQChartsSummaryPlotColumnChooser;

  addFrameWidget(columnsFrame, chooser_);
}

void
CQChartsSummaryPlotCustomControls::
addGroupStatsWidgets()
{
  // group frame
  auto groupFrame = createGroupFrame("Group", "groupFrame");

  addNamedColumnWidgets(QStringList() << "group", groupFrame);

  stats_ = new CQChartsSummaryPlotGroupStats;

  addFrameWidget(groupFrame, stats_);
}

void
CQChartsSummaryPlotCustomControls::
addOptionsWidgets()
{
  // options frame
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  plotTypeCombo_ = createEnumEdit("plotType");

  diagonalTypeCombo_      = createEnumEdit("diagonalType");
  upperDiagonalTypeCombo_ = createEnumEdit("upperDiagonalType");
  lowerDiagonalTypeCombo_ = createEnumEdit("lowerDiagonalType");

  addFrameWidget(optionsFrame_, "Plot Type", plotTypeCombo_);

  addFrameWidget(optionsFrame_, "Diagonal Type"  , diagonalTypeCombo_);
  addFrameWidget(optionsFrame_, "Upper Cell Type", upperDiagonalTypeCombo_);
  addFrameWidget(optionsFrame_, "Lower Cell Type", lowerDiagonalTypeCombo_);

  bestFitCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Best Fit", "bestFitCheck");
  densityCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Density" , "densityCheck");

  bestFitCheck_->setToolTip("Show Best Fit");
  densityCheck_->setToolTip("Show Density");

  addFrameColWidget(optionsFrame_, bestFitCheck_);
  addFrameColWidget(optionsFrame_, densityCheck_);
}

void
CQChartsSummaryPlotCustomControls::
addExpandControls()
{
  // buttons group
  buttonsFrame_ = createGroupFrame("Functions", "buttonsFrame", FrameOpts::makeHBox());

  expandButton_   = CQUtil::makeLabelWidget<QPushButton>("Expand"       , "expandButton");
  deselectButton_ = CQUtil::makeLabelWidget<QPushButton>("Clear Ranges" , "deselectButton");
  selectButton_   = CQUtil::makeLabelWidget<QPushButton>("Select Ranges", "selectButton");

  deselectButton_->setToolTip("Reset Ranges");
  selectButton_  ->setToolTip("Select Model Items from Range");

  buttonsFrame_.box->addWidget (expandButton_);
  addFrameSpacer(buttonsFrame_);

  buttonsFrame_.box->addWidget (deselectButton_);
  buttonsFrame_.box->addWidget (selectButton_);
  buttonsFrame_.box->addStretch(1);
}

void
CQChartsSummaryPlotCustomControls::
connectSlots(bool b)
{
  CQUtil::optConnectDisconnect(b,
    plotTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotTypeSlot()));
  CQUtil::optConnectDisconnect(b,
    diagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(diagonalTypeSlot()));
  CQUtil::optConnectDisconnect(b,
    upperDiagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(upperDiagonalTypeSlot()));
  CQUtil::optConnectDisconnect(b,
    lowerDiagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(lowerDiagonalTypeSlot()));

  CQUtil::optConnectDisconnect(b,
    bestFitCheck_, SIGNAL(stateChanged(int)), this, SLOT(bestFitSlot(int)));
  CQUtil::optConnectDisconnect(b,
    densityCheck_, SIGNAL(stateChanged(int)), this, SLOT(densitySlot(int)));

  CQUtil::optConnectDisconnect(b,
    expandButton_, SIGNAL(clicked()), this, SLOT(expandSlot()));
  CQUtil::optConnectDisconnect(b,
    deselectButton_, SIGNAL(clicked()), this, SLOT(deselectSlot()));
  CQUtil::optConnectDisconnect(b,
    selectButton_, SIGNAL(clicked()), this, SLOT(selectSlot()));

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsSummaryPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && summaryPlot_) {
    disconnect(summaryPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
    disconnect(summaryPlot_, SIGNAL(selectionChanged()), this, SLOT(updateWidgets()));
  }

  summaryPlot_ = dynamic_cast<CQChartsSummaryPlot *>(plot);

  stats_  ->setPlot(summaryPlot_);
  chooser_->setPlot(summaryPlot_);

  CQChartsPlotCustomControls::setPlot(plot);

  if (summaryPlot_) {
    connect(summaryPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
    connect(summaryPlot_, SIGNAL(selectionChanged()), this, SLOT(updateWidgets()));
  }
}

void
CQChartsSummaryPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  plotTypeCombo_         ->setCurrentValue(static_cast<int>(summaryPlot_->plotType()));
  diagonalTypeCombo_     ->setCurrentValue(static_cast<int>(summaryPlot_->diagonalType()));
  upperDiagonalTypeCombo_->setCurrentValue(static_cast<int>(summaryPlot_->upperDiagonalType()));
  lowerDiagonalTypeCombo_->setCurrentValue(static_cast<int>(summaryPlot_->lowerDiagonalType()));

  bestFitCheck_->setChecked(summaryPlot_->isBestFit());
  densityCheck_->setChecked(summaryPlot_->isDensity());

  stats_  ->updateWidgets();
  chooser_->updateWidgets();

  //---

  if (! summaryPlot_->isExpanded()) {
    expandButton_->setText("Expand Selected Plot");
    expandButton_->setEnabled(summaryPlot_->selectedCellObj());
  }
  else {
    expandButton_->setText("Collapse back to Summary");
    expandButton_->setEnabled(true);
  }

  //---

  connectSlots(true);

  //---

  CQChartsPlotCustomControls::updateWidgets();
}

void
CQChartsSummaryPlotCustomControls::
plotTypeSlot()
{
  summaryPlot_->setPlotType(
    static_cast<CQChartsSummaryPlot::PlotType>(plotTypeCombo_->currentValue()), /*update*/false);

  QTimer::singleShot(10, summaryPlot_, SLOT(updatePlotsSlot()));
}

void
CQChartsSummaryPlotCustomControls::
diagonalTypeSlot()
{
  summaryPlot_->setDiagonalType(
    static_cast<CQChartsSummaryPlot::DiagonalType>(diagonalTypeCombo_->currentValue()));
}

void
CQChartsSummaryPlotCustomControls::
upperDiagonalTypeSlot()
{
  summaryPlot_->setUpperDiagonalType(
    static_cast<CQChartsSummaryPlot::OffDiagonalType>(upperDiagonalTypeCombo_->currentValue()));
}

void
CQChartsSummaryPlotCustomControls::
lowerDiagonalTypeSlot()
{
  summaryPlot_->setLowerDiagonalType(
    static_cast<CQChartsSummaryPlot::OffDiagonalType>(lowerDiagonalTypeCombo_->currentValue()));
}

void
CQChartsSummaryPlotCustomControls::
bestFitSlot(int state)
{
  summaryPlot_->setBestFit(state);
}

void
CQChartsSummaryPlotCustomControls::
densitySlot(int state)
{
  summaryPlot_->setDensity(state);
}

void
CQChartsSummaryPlotCustomControls::
expandSlot()
{
  if (! summaryPlot_->isExpanded())
    summaryPlot_->expandCell(summaryPlot_->selectedCellObj());
  else
    summaryPlot_->collapseCell();
}

void
CQChartsSummaryPlotCustomControls::
deselectSlot()
{
  summaryPlot_->clearColumnRanges();
}

void
CQChartsSummaryPlotCustomControls::
selectSlot()
{
  summaryPlot_->selectColumnRanges();
}
