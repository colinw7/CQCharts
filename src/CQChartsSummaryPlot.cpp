#include <CQChartsSummaryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsSymbolSet.h>
#include <CQChartsValueSet.h>
#include <CQChartsPlotDrawUtil.h>
#include <CQChartsDrawUtil.h>

#include <CQChartsScatterPlot.h>
#include <CQChartsDistributionPlot.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CMathCorrelation.h>

#include <QMenu>
#include <QCheckBox>

CQChartsSummaryPlotType::
CQChartsSummaryPlotType()
{
}

void
CQChartsSummaryPlotType::
addParameters()
{
  addColumnsParameter("columns", "Columns", "columns").
    setRequired().setPropPath("columns.columns").setTip("Columns");

  addColumnParameter("group", "Group", "groupColumn").
   setGroupable().setBasic().setPropPath("columns.group").setTip("Group column");

  // options
  addEnumParameter("diagonalType", "Diagonal Cell Type", "diagonalType").
    addNameValue("NONE"        , int(CQChartsSummaryPlot::DiagonalType::NONE        )).
    addNameValue("BOXPLOT"     , int(CQChartsSummaryPlot::DiagonalType::BOXPLOT     )).
    addNameValue("DISTRIBUTION", int(CQChartsSummaryPlot::DiagonalType::DISTRIBUTION)).
    addNameValue("PIE"         , int(CQChartsSummaryPlot::DiagonalType::PIE         )).
    setTip("Diagonal Cell Type");
  addEnumParameter("upperDiagonalType", "Upper Diagonal Cell Type", "upperDiagonalType").
    addNameValue("NONE"       , int(CQChartsSummaryPlot::OffDiagonalType::NONE       )).
    addNameValue("SCATTER"    , int(CQChartsSummaryPlot::OffDiagonalType::SCATTER    )).
    addNameValue("CORRELATION", int(CQChartsSummaryPlot::OffDiagonalType::CORRELATION)).
    setTip("Upper Diagonal Cell Type");
  addEnumParameter("lowerDiagonalType", "Lower Diagonal Cell Type", "lowerDiagonalType").
    addNameValue("NONE"       , int(CQChartsSummaryPlot::OffDiagonalType::NONE       )).
    addNameValue("SCATTER"    , int(CQChartsSummaryPlot::OffDiagonalType::SCATTER    )).
    addNameValue("CORRELATION", int(CQChartsSummaryPlot::OffDiagonalType::CORRELATION)).
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
 CQChartsPlot(view, view->charts()->plotType("summary"), model)
{
}

CQChartsSummaryPlot::
~CQChartsSummaryPlot()
{
  term();
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

  addTitle();

  //---

  auto createPlot = [&](const QString &typeName) {
    auto *type = charts()->plotType(typeName);
    assert(type);

    auto *plot = type->createAndInit(view(), model());

    int n = view()->numPlots();

    plot->setId(QString("Chart.%1.%2").arg(n + 1).arg(typeName));

    double vr = CQChartsView::viewportRange();

    BBox bbox(0, 0, vr, vr);

    view()->addPlot(plot, bbox);

    plot->setVisible(false);
    plot->setRootPlot(this);

    return plot;
  };

  scatterPlot_      = dynamic_cast<ScatterPlot      *>(createPlot("scatter"));
  distributionPlot_ = dynamic_cast<DistributionPlot *>(createPlot("distribution"));

  scatterPlot_->setXColumn(Column::makeRow());
  scatterPlot_->setYColumn(Column::makeRow());

  distributionPlot_->setValueColumns(Columns(Column::makeRow()));
}

void
CQChartsSummaryPlot::
term()
{
}

//---

void
CQChartsSummaryPlot::
setDiagonalType(const DiagonalType &t)
{
  CQChartsUtil::testAndSet(diagonalType_, t, [&]() {
    drawObjs(); emit customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setLowerDiagonalType(const OffDiagonalType &t)
{
  CQChartsUtil::testAndSet(lowerDiagonalType_, t, [&]() {
    drawObjs(); emit customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setUpperDiagonalType(const OffDiagonalType &t)
{
  CQChartsUtil::testAndSet(upperDiagonalType_, t, [&]() {
    drawObjs(); emit customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFit_, b, [&]() {
    drawObjs(); emit customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setDensity(bool b)
{
  CQChartsUtil::testAndSet(density_, b, [&]() {
    drawObjs(); emit customDataChanged();
  } );
}

//---

void
CQChartsSummaryPlot::
setModel(const ModelP &model)
{
  model_ = model;

  updateRangeAndObjs();

  emit modelChanged();
}

void
CQChartsSummaryPlot::
modelTypeChangedSlot(int modelInd)
{
  auto *modelData = charts()->getModelData(model_);

  if (modelData && modelData->ind() == modelInd)
    updateRangeAndObjs();
}

//---

void
CQChartsSummaryPlot::
setColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(columns_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setGroupColumn(const Column &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() {
    resetSetHidden(); updateRangeAndObjs(); emit customDataChanged();
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

  // x/y axis label text
  addProp("xaxis/text", "xLabels", "visible", "X labels visible");

  addProp("yaxis/text", "yLabels", "visible", "Y labels visible");

  // cell types
  addProp("cell", "diagonalType"     , "diagonal"     , "Diagonal cell type");
  addProp("cell", "lowerDiagonalType", "lowerDiagonal", "Lower Diagonal cell type");
  addProp("cell", "upperDiagonalType", "upperDiagonal", "Upper Diagonal cell type");
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

  // square (nc, nc)
  int nc = std::max(columns().count(), 1);

  Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(nc , nc );

  //---

  return dataRange;
}

bool
CQChartsSummaryPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsSummaryPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  int nc = columns().count();

  for (int ir = 0; ir < nc; ++ir) {
    for (int ic = 0; ic < nc; ++ic) {
      auto *obj = createCellObj(cellBBox(ir, ic), ir, ic);

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
  int nc = columns().count();

  double pymax = nc - row;
  double pymin = pymax - 1;

  double pxmin = col;
  double pxmax = pxmin + 1;

  return BBox(pxmin, pymin, pxmax, pymax);
}

//------

bool
CQChartsSummaryPlot::
addMenuItems(QMenu *menu)
{
  PlotObjs objs;

  selectedPlotObjs(objs);

  if (objs.empty()) {
    auto gpos = view()->menuPos();
    auto pos  = view()->mapFromGlobal(QPoint(gpos.x, gpos.y));

    auto w = pixelToWindow(Point(pos));

    plotObjsAtPoint(w, objs, Constraints::SELECTABLE);
  }

  menuObj_ = (! objs.empty() ? *objs.begin() : nullptr);

  if (menuObj_)
    addMenuAction(menu, "Expand", SLOT(expandSlot()));

  return true;
}

void
CQChartsSummaryPlot::
expandSlot()
{
  auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(menuObj_);
  if (! cellObj) return;

  int row = cellObj->row();
  int col = cellObj->col();

  if (row != col) {
    auto column1 = columns().getColumn(row);
    auto column2 = columns().getColumn(col);

    setVisible(false);

    scatterPlot_->setXColumn(column1);
    scatterPlot_->setYColumn(column2);

    scatterPlot_->setGroupColumn(groupColumn());

    scatterPlot_->setVisible(true);
  }
  else {
    auto column = columns().getColumn(row);

    setVisible(false);

    distributionPlot_->setValueColumns(Columns(column));

    distributionPlot_->setVisible(true);
  }
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

  int nc = columns().count();

  for (int ic = 0; ic < nc; ++ic) {
    auto column = columns().getColumn(ic);

    auto bbox = cellBBox(ic, ic);

    bool ok;
    auto str = modelHHeaderString(column, ok);

    //---

    auto tsize = CQChartsDrawUtil::calcTextSize(str, device->font(), CQChartsTextOptions());

    //---

    Point p1(bbox.getXMid(), -border());

    CQChartsTextOptions options1;

    options1.align = Qt::AlignHCenter | Qt::AlignVCenter;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, str, options1, /*centered*/false);

    //---

    Point p2(pixelToWindowWidth(tsize.height()), bbox.getYMid());

    CQChartsTextOptions options2;

    options2.angle = Angle(90.0);
    options2.align = Qt::AlignHCenter | Qt::AlignVCenter;

    CQChartsDrawUtil::drawTextAtPoint(device, p2, str, options2, /*centered*/false);
  }
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
CQChartsSummaryCellObj(const Plot *plot, const BBox &bbox, int row, int col) :
 CQChartsPlotObj(const_cast<Plot *>(plot), bbox, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), row_(row), col_(col)
{
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
  if (row_ != col_) {
    auto column1 = plot_->columns().getColumn(row_);
    auto column2 = plot_->columns().getColumn(col_);

    bool ok;
    auto xtip = plot_->modelHHeaderTip(column1, ok);
    auto ytip = plot_->modelHHeaderTip(column2, ok);

    return QString("%1 : %2").arg(xtip).arg(ytip);
  }
  else {
    auto column1 = plot_->columns().getColumn(row_);

    bool ok;
    auto tip = plot_->modelHHeaderTip(column1, ok);

    return tip;
  }
}

void
CQChartsSummaryCellObj::
draw(PaintDevice *device) const
{
  auto bbox = this->rect();

  pxmin_ = bbox.getXMin() + plot_->border();
  pxmax_ = bbox.getXMax() - plot_->border();
  pymin_ = bbox.getYMin() + plot_->border();
  pymax_ = bbox.getYMax() - plot_->border();

  //---

  auto bg = plot_->interpInterfaceColor(0.15);
  auto fg = plot_->interpInterfaceColor(1.00);

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, fg), BrushData(true, bg));

  plot_->updateObjPenBrushState(this, penBrush, drawType());

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawRect(bbox);

  //---

  if (row_ != col_) {
    bool lower = (row_ > col_);

    auto isOffDiagonalType = [&](CQChartsSummaryPlot::OffDiagonalType type) {
      return ((  lower && plot_->lowerDiagonalType() == type) ||
              (! lower && plot_->upperDiagonalType() == type));
    };

    bool isScatter     = isOffDiagonalType(CQChartsSummaryPlot::OffDiagonalType::SCATTER);
    bool isCorrelation = isOffDiagonalType(CQChartsSummaryPlot::OffDiagonalType::CORRELATION);

    if      (isScatter)
      drawScatter(device);
    else if (isCorrelation)
      drawCorrelation(device);
  }
  else {
    if      (plot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::BOXPLOT)
      drawBoxPlot(device);
    else if (plot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::DISTRIBUTION)
      drawDistribution(device);
    else if (plot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::PIE)
      drawPie(device);
  }
}

void
CQChartsSummaryCellObj::
drawScatter(PaintDevice *device) const
{
  auto column1 = plot_->columns().getColumn(row_);
  auto column2 = plot_->columns().getColumn(col_);

  auto *details1 = plot_->columnDetails(column1);
  auto *details2 = plot_->columnDetails(column2);

  auto *groupDetails = (plot_->groupColumn().isValid() ?
    plot_->columnDetails(plot_->groupColumn()) : nullptr);

  int ng = (groupDetails ? groupDetails->numUnique() : 0);

  //---

  int nc = plot_->columns().count();

  auto pc = plot_->interpInterfaceColor(1.0);
  auto bc = plot_->interpPaletteColor(ColorInd(row_, nc));

  if (details1->isNumeric()) {
    bool ok;
    xmin_ = details1->minValue().toDouble(&ok);
    xmax_ = details1->maxValue().toDouble(&ok);
  }
  else {
    xmin_ = 0.0;
    xmax_ = details1->numUnique();
  }

  if (details2->isNumeric()) {
    bool ok;
    ymin_ = details2->minValue().toDouble(&ok);
    ymax_ = details2->maxValue().toDouble(&ok);
  }
  else {
    ymin_ = 0.0;
    ymax_ = details2->numUnique();
  }

  int nx = details1->numRows();
  int ny = details2->numRows();
  assert(nx == ny);

  auto symbol = CQChartsSymbol::circle();

  auto ss = CQChartsLength::pixel(5);

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.5)), BrushData(true, bc));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  poly_ = Polygon();

  for (int i = 0; i < nx; ++i) {
    double x, y;

    if (details1->isNumeric()) {
      bool ok;
      double r = details1->value(i).toDouble(&ok);
      if (! ok) continue;

      x = CMathUtil::map(r, xmin_, xmax_, pxmin_, pxmax_);
    }
    else {
      int id = details1->uniqueId(details1->value(i));

      x = CMathUtil::map(id, xmin_, xmax_, pxmin_, pxmax_);
    }

    if (details2->isNumeric()) {
      bool ok;
      double r = details2->value(i).toDouble(&ok);
      if (! ok) continue;

      y = CMathUtil::map(r, ymin_, ymax_, pymin_, pymax_);
    }
    else {
      int id = details2->uniqueId(details2->value(i));

      y = CMathUtil::map(id, ymin_, ymax_, pymin_, pymax_);
    }

    Point ps(x, y);

    //---

    PenBrush       penBrush1;
    CQChartsSymbol symbol1;

    if (groupDetails) {
      auto var = groupDetails->value(i);

      int ig = groupDetails->uniqueId(var);

      auto bc1 = plot_->interpPaletteColor(ColorInd(ig, ng));

      plot_->setPenBrush(penBrush1, PenData(true, pc, Alpha(0.5)), BrushData(true, bc1));

      CQChartsDrawUtil::setPenBrush(device, penBrush1);

      auto *symbolSet = plot_->defaultSymbolSet();

      symbol1 = symbolSet->interpI(ig, 0, ng - 1).symbol;
    }
    else {
      penBrush1 = penBrush;
      symbol1   = symbol;
    }

    //---

    CQChartsDrawUtil::drawSymbol(device, penBrush1, symbol1, ps, ss);

    //---

    poly_.addPoint(ps);
  }

  //---

  if (plot_->isBestFit())
    drawBestFit(device);
}

void
CQChartsSummaryCellObj::
drawBestFit(PaintDevice *device) const
{
  auto bbox = this->rect();

  auto drawFitPoly = [&](const Polygon &polygon) {
    auto pbbox = polygon.boundingBox();

    CQChartsFitData fitData;

    fitData.calc(polygon, 3);

    double dx = pbbox.getWidth()/100.0;

    Polygon fitPoly;

    for (int i = 0; i <= 100; ++i) {
      double x = pbbox.getXMin() + i*dx;
      double y = fitData.interp(x);

      double x1 = CMathUtil::map(x, xmin_, xmax_, pxmin_, pxmax_);
      double y1 = CMathUtil::map(y, ymin_, ymax_, pymin_, pymax_);

      fitPoly.addPoint(Point(x1, y1));
    }

    //---

    device->save();

    auto path = CQChartsDrawUtil::polygonToPath(fitPoly, /*closed*/false);

    device->setClipRect(bbox);

    device->strokePath(path, device->pen());

    device->restore();
  };

  if (! plot_->groupColumn().isValid()) {
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
  auto bbox = this->rect();

  auto column1 = plot_->columns().getColumn(row_);
  auto column2 = plot_->columns().getColumn(col_);

  auto *details1 = plot_->columnDetails(column1);
  auto *details2 = plot_->columnDetails(column2);

  if (details1->isNumeric() && details2->isNumeric()) {
    if (! plot_->groupColumn().isValid()) {
      auto correlation = plot_->modelDetails()->correlation(column1, column2);

      CQChartsTextOptions options;

      options.align = Qt::AlignHCenter | Qt::AlignVCenter;

      auto p = bbox.getCenter();

      CQChartsDrawUtil::drawTextAtPoint(device, p, QString::number(correlation), options);
    }
    else {
      const_cast<CQChartsSummaryCellObj *>(this)->initGroupedValues();

      int ng = groupValues_.groupIndData.size();

      double dy = (ng > 0 ? bbox.getHeight()/(ng + 1) : 0);

      QFontMetricsF fm(device->font());

      double dy1 = plot_->pixelToWindowHeight(fm.height());

      double x = bbox.getXMid();
      double y = bbox.getYMid() - ng*std::min(dy, dy1)/2.0;

      for (const auto &pg : groupValues_.groupIndData) {
        const auto &indData = pg.second;

        auto correlation = CMathCorrelation::calc(indData.x, indData.y);

        CQChartsTextOptions options;

        options.align = Qt::AlignHCenter | Qt::AlignVCenter;

        auto p = Point(x, y);

        CQChartsDrawUtil::drawTextAtPoint(device, p, QString::number(correlation), options);

        y += std::min(dy, dy1);
      }
    }
  }
}

void
CQChartsSummaryCellObj::
drawBoxPlot(PaintDevice *device) const
{
  auto column = plot_->columns().getColumn(row_);

  auto *details = plot_->columnDetails(column);

  int nc = plot_->columns().count();

  auto pc = plot_->interpInterfaceColor(1.0);
  auto bc = plot_->interpPaletteColor(ColorInd(row_, nc));

  if (details->isNumeric()) {
    bool ok;
    double min = details->minValue().toDouble(&ok);
    double max = details->maxValue().toDouble(&ok);

    int n = details->numRows();

    PenBrush penBrush;

    plot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.5)), BrushData(true, bc));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsBoxWhisker whisker;

    for (int i = 0; i < n; ++i) {
      bool ok;
      double r = details->value(i).toDouble(&ok);
      if (! ok) continue;

      double r1 = CMathUtil::map(r, min, max, pymin_, pymax_);

      whisker.addValue(r1);
    }

    BBox bbox(pxmin_, pymin_, pxmax_, pymax_);

    CQChartsLength width(0.1, CQChartsUnits::PLOT);

    CQChartsBoxWhiskerUtil::drawWhisker(device, whisker, bbox, width, Qt::Vertical);
  }
  else {
  }
}

void
CQChartsSummaryCellObj::
drawDistribution(PaintDevice *device) const
{
  auto column = plot_->columns().getColumn(row_);

  auto *details = plot_->columnDetails(column);

  int nc = plot_->columns().count();

  auto pc = plot_->interpInterfaceColor(1.0);
  auto bc = plot_->interpPaletteColor(ColorInd(row_, nc));

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.5)), BrushData(true, bc));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  if (details->isNumeric()) {
    BucketCount bucketCount;
    int         maxCount = 0;

    calcBucketCounts(bucketCount, maxCount, bmin_, bmax_);

    for (const auto &pb : bucketCount) {
      QVariant vmin, vmax;

      details->bucketRange(pb.first, vmin, vmax);

      bool ok;
      double rmin1 = vmin.toDouble(&ok);
      double rmax1 = vmax.toDouble(&ok);

      int n = pb.second;

      double x1 = CMathUtil::map(rmin1, bmin_, bmax_, pxmin_, pxmax_);
      double x2 = CMathUtil::map(rmax1, bmin_, bmax_, pxmin_, pxmax_);

      double y1 = CMathUtil::map(0, 0, maxCount, pymin_, pymax_);
      double y2 = CMathUtil::map(n, 0, maxCount, pymin_, pymax_);

      BBox bbox(x1, y1, x2, y2);

      device->drawRect(bbox);
    }

    //---

    if (plot_->isDensity())
      drawDensity(device);
  }
  else {
    ValueCounts valueCounts;
    int         maxCount = 0;

    calcValueCounts(valueCounts, maxCount);

    int nc = valueCounts.size();

    double dx = (nc > 0 ? (pxmax_ - pxmin_)/nc : 0.0);

    double x = pxmin_;

    for (const auto &vc : valueCounts) {
      auto r = CMathUtil::map(vc.second, 0, maxCount, pymin_, pymax_);

      BBox bbox(x, pymin_, x + dx, r);

      device->drawRect(bbox);

      x += dx;
    }
  }
}

void
CQChartsSummaryCellObj::
drawDensity(PaintDevice *device) const
{
  auto bbox = this->rect();

  //---

  auto column = plot_->columns().getColumn(row_);

  auto *details = plot_->columnDetails(column);

  //---

  auto pc = plot_->interpInterfaceColor(1.0);

  CQChartsDensity density;

  density.setDrawType(CQChartsDensity::DrawType::DISTRIBUTION);

  CQChartsDensity::XVals xvals;

  int n = details->numRows();

  for (int i = 0; i < n; ++i) {
    auto value = details->value(i);

    bool ok;
    double r = value.toDouble(&ok);
    if (! ok) continue;

    double x = CMathUtil::map(r, bmin_, bmax_, pxmin_, pxmax_);

    xvals.push_back(x);
  }

  density.setXVals(xvals);

  //---

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.5)), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->save();

  device->setClipRect(bbox);

  density.draw(plot_, device, bbox, /*scaled*/true);

  device->restore();
}

void
CQChartsSummaryCellObj::
drawPie(PaintDevice *device) const
{
  auto column = plot_->columns().getColumn(row_);

  auto *details = plot_->columnDetails(column);

  int nc = plot_->columns().count();

  auto pc = plot_->interpInterfaceColor(1.0);
  auto bc = plot_->interpPaletteColor(ColorInd(row_, nc));

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.5)), BrushData(true, bc));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  if (details->isNumeric()) {
    BucketCount bucketCount;
    int         maxCount = 0;

    calcBucketCounts(bucketCount, maxCount, bmin_, bmax_);

    CQChartsRValues values;

    for (const auto &pb : bucketCount)
      values.addValue(CQChartsRValues::OptReal(pb.second));

    auto bbox1 = BBox(pxmin_, pymin_, pxmax_, pymax_);

    CQChartsPlotDrawUtil::drawPie(const_cast<CQChartsSummaryPlot *>(plot_), device, values, bbox1,
                                  plot_->defaultPalette(), penBrush.pen);
  }
  else {
  }
}

void
CQChartsSummaryCellObj::
calcBucketCounts(BucketCount &bucketCount, int &maxCount, double &rmin, double &rmax) const
{
  auto column = plot_->columns().getColumn(row_);

  auto *details = plot_->columnDetails(column);

  bool ok;
  double min = details->minValue().toDouble(&ok);
  double max = details->maxValue().toDouble(&ok);

  int n = details->numRows();

  for (int i = 0; i < n; ++i) {
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
    double rmin1 = vmin.toDouble(&ok);
    double rmax1 = vmax.toDouble(&ok);

    rmin = std::min(rmin, rmin1);
    rmax = std::max(rmax, rmax1);
  }
}

void
CQChartsSummaryCellObj::
calcValueCounts(ValueCounts &valueCounts, int &maxCount) const
{
  auto column = plot_->columns().getColumn(row_);

  auto *details = plot_->columnDetails(column);

  maxCount = 0;

  valueCounts = details->uniqueValueCounts();

  for (const auto &vc : valueCounts)
    maxCount = std::max(maxCount, vc.second);
}

void
CQChartsSummaryCellObj::
initGroupedValues()
{
  if (groupValues_.set)
    return;

  groupValues_.set = true;

  if (! plot_->groupColumn().isValid())
    return;

  auto *groupDetails = plot_->columnDetails(plot_->groupColumn());
  if (! groupDetails) return;

  int nr = groupDetails->numRows();

  if (row_ != col_) {
    auto column1 = plot_->columns().getColumn(row_);
    auto column2 = plot_->columns().getColumn(col_);

    auto *details1 = plot_->columnDetails(column1);
    auto *details2 = plot_->columnDetails(column2);

    for (int ir = 0; ir < nr; ++ir) {
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
        x = value1.toDouble(&ok);

        indData.x.push_back(x);
      }

      if (details2->isNumeric()) {
        bool ok;
        y = value2.toDouble(&ok);

        indData.y.push_back(y);
      }

      indData.poly.addPoint(Point(x, y));
    }
  }
  else {
    auto column = plot_->columns().getColumn(row_);

    auto *details = plot_->columnDetails(column);

    for (int ir = 0; ir < nr; ++ir) {
      auto var = groupDetails->value(ir);

      int ig = groupDetails->uniqueId(var);

      auto &indData = groupValues_.groupIndData[ig];

      auto value = details->value(ir);

      indData.xvals.push_back(value);
      indData.yvals.push_back(value);

      if (details->isNumeric()) {
        bool ok;
        double r = value.toDouble(&ok);

        indData.x.push_back(r);
        indData.y.push_back(r);

        indData.poly.addPoint(Point(r, r));
      }
    }
  }
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

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsSummaryPlotCustomControls::
addWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addColumnWidgets(QStringList() << "columns" << "group", columnsFrame);

  //---

  // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame");

  diagonalTypeCombo_      = createEnumEdit("diagonalType");
  upperDiagonalTypeCombo_ = createEnumEdit("upperDiagonalType");
  lowerDiagonalTypeCombo_ = createEnumEdit("lowerDiagonalType");

  addFrameWidget(optionsFrame, "Diagonal Type"  , diagonalTypeCombo_);
  addFrameWidget(optionsFrame, "Upper Cell Type", upperDiagonalTypeCombo_);
  addFrameWidget(optionsFrame, "Lower Cell Type", lowerDiagonalTypeCombo_);

  bestFitCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Best Fit", "bestFitCheck");
  densityCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Density" , "densityCheck");

  addFrameColWidget(optionsFrame, bestFitCheck_);
  addFrameColWidget(optionsFrame, densityCheck_);
}

void
CQChartsSummaryPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    diagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(diagonalTypeSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    upperDiagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(upperDiagonalTypeSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    lowerDiagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(lowerDiagonalTypeSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    bestFitCheck_, SIGNAL(stateChanged(int)), this, SLOT(bestFitSlot(int)));
  CQChartsWidgetUtil::connectDisconnect(b,
    densityCheck_, SIGNAL(stateChanged(int)), this, SLOT(densitySlot(int)));

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsSummaryPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsSummaryPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsSummaryPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  diagonalTypeCombo_     ->setCurrentValue((int) plot_->diagonalType());
  upperDiagonalTypeCombo_->setCurrentValue((int) plot_->upperDiagonalType());
  lowerDiagonalTypeCombo_->setCurrentValue((int) plot_->lowerDiagonalType());

  bestFitCheck_->setChecked(plot_->isBestFit());
  densityCheck_->setChecked(plot_->isDensity());

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsSummaryPlotCustomControls::
diagonalTypeSlot()
{
  plot_->setDiagonalType((CQChartsSummaryPlot::DiagonalType)
                         diagonalTypeCombo_->currentValue());

  updateWidgets();
}

void
CQChartsSummaryPlotCustomControls::
upperDiagonalTypeSlot()
{
  plot_->setUpperDiagonalType((CQChartsSummaryPlot::OffDiagonalType)
                              upperDiagonalTypeCombo_->currentValue());

  updateWidgets();
}

void
CQChartsSummaryPlotCustomControls::
lowerDiagonalTypeSlot()
{
  plot_->setLowerDiagonalType((CQChartsSummaryPlot::OffDiagonalType)
                              lowerDiagonalTypeCombo_->currentValue());

  updateWidgets();
}

void
CQChartsSummaryPlotCustomControls::
bestFitSlot(int state)
{
  plot_->setBestFit(state);
}

void
CQChartsSummaryPlotCustomControls::
densitySlot(int state)
{
  plot_->setDensity(state);
}
