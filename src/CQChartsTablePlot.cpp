#include <CQChartsTablePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetAction.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQIntegerSpin.h>
#include <CQTclUtil.h>
#include <CMathRound.h>

#include <QMenu>
#include <QScrollBar>
#include <QMetaMethod>

CQChartsTablePlotType::
CQChartsTablePlotType()
{
}

void
CQChartsTablePlotType::
addParameters()
{
  startParameterGroup("Data");

  addColumnsParameter("columns", "Columns", "columns").
    setRequired().setPropPath("columns.columns").setTip("Columns");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsTablePlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
    h2("Table Plot").
     h3("Summary").
      p("Draws data in table.").
     h3("Limitations").
      p("None.").
     h3("Example").
      p(IMG("images/table.png"));
}

void
CQChartsTablePlotType::
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
CQChartsTablePlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsTablePlot(view, model);
}

//------

CQChartsTablePlot::
CQChartsTablePlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("table"), model)
{
}

CQChartsTablePlot::
~CQChartsTablePlot()
{
  CQChartsTablePlot::term();
}

//---

void
CQChartsTablePlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  // plot, data, fit background
  setPlotFilled(false); setPlotStroked(false);
  setDataFilled(false); setDataStroked(false);
  setFitFilled (false); setFitStroked (false);

  setDataClip(false);

  setPlotFillColor(Color::makeInterfaceValue(0.00));
  setDataFillColor(Color::makeInterfaceValue(0.00));
  setFitFillColor (Color::makeInterfaceValue(0.00));

  //---

  font_ = Font().decFontSize(8);

  hheaderData_  .font = font_;
  vheaderData_  .font = font_;
  rowColumnData_.font = font_;

  //---

  const auto &model = this->currentModel();

  if (! CQChartsModelUtil::isHierarchical(model.data())) {
    summaryModel_ = new CQSummaryModel(model.data());

    summaryModel_->setMode(static_cast<CQSummaryModel::Mode>(mode_));
  }

  int pageSize = 1024;

  setMaxRows (10*pageSize);
  setPageSize(pageSize);

  setGridColor(Color::makeInterfaceValue(0.50));
  setGridWidth(Length::pixel(1));

  setHHeaderColor  (Color(QColor("#c0c8dc"))); // TODO: from style
  setVHeaderColor  (Color(QColor("#aaaabb"))); // TODO: from style
  setRowColumnColor(Color(QColor("#aaaabb"))); // TODO: from style

  setCellColor    (Color::makeInterfaceValue(0.00));
  setInsideColor  (Color(QColor("#c5c8bc"))); // TODO: from style
  setSelectedColor(Color(QColor("#bcc5c8"))); // TODO: from style

  //---

  setOuterMargin(PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0)));

  addTitle();

  //---

  scrollData_.hbar = new QScrollBar(Qt::Horizontal, view());
  scrollData_.vbar = new QScrollBar(Qt::Vertical  , view());

  scrollData_.hbar->hide();
  scrollData_.vbar->hide();

  connect(scrollData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
  connect(scrollData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));

  scrollData_.pixelBarSize = view()->style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2;

  //---

  connect(charts(), SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
}

void
CQChartsTablePlot::
term()
{
  delete summaryModel_;

  delete scrollData_.hbar;
  delete scrollData_.vbar;
}

//---

void
CQChartsTablePlot::
addModelI(const ModelP &model)
{
  CQChartsPlot::addModelI(model);

  delete summaryModel_;

  if (! CQChartsModelUtil::isHierarchical(model.data())) {
    summaryModel_ = new CQSummaryModel(model.data());

    summaryModel_->setMode(static_cast<CQSummaryModel::Mode>(mode_));
  }
}

void
CQChartsTablePlot::
modelTypeChangedSlot(int modelInd)
{
  auto *modelData = currentModelData();

  if (modelData && modelData->isInd(modelInd))
    updateRangeAndObjs();
}

//---

void
CQChartsTablePlot::
hscrollSlot(int v)
{
  scrollData_.hpos = v;

  drawObjs();
}

void
CQChartsTablePlot::
vscrollSlot(int v)
{
  scrollData_.vpos = v;

  drawObjs();
}

//---

void
CQChartsTablePlot::
setColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(columns_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

CQChartsColumns
CQChartsTablePlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "columns") c = this->columns();
  else                   c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsTablePlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "columns") this->setColumns(c);
  else                   CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsTablePlot::
setFont(const Font &f)
{
  CQChartsUtil::testAndSet(font_, f, [&]() { updateRangeAndObjs(); } );
}

//---

CQChartsTablePlot::Mode
CQChartsTablePlot::
mode() const
{
  return (summaryModel_ ? static_cast<CQChartsTablePlot::Mode>(summaryModel_->mode()) : mode_);
}

void
CQChartsTablePlot::
setMode(const Mode &m)
{
  if (m != mode()) {
    mode_ = m;

    if (summaryModel_)
      summaryModel_->setMode(static_cast<CQSummaryModel::Mode>(mode_));

    updateRangeAndObjs();
  }
}

//---

int
CQChartsTablePlot::
maxRows() const
{
  return (summaryModel_ ? summaryModel_->maxRows() : -1);
}

void
CQChartsTablePlot::
setMaxRows(int i)
{
  if (i != maxRows()) {
    if (summaryModel_)
      summaryModel_->setMaxRows(i);

    updateRangeAndObjs();
  }
}

//---

CQChartsColumnNum
CQChartsTablePlot::
sortColumn() const
{
  return (summaryModel_ ? ColumnNum(summaryModel_->sortColumn()) : ColumnNum());
}

void
CQChartsTablePlot::
setSortColumn(const ColumnNum &c)
{
  if (c != sortColumn()) {
    if (summaryModel_)
      summaryModel_->setSortColumn(c.column());

    updateRangeAndObjs();
  }
}

int
CQChartsTablePlot::
sortRole() const
{
  return (summaryModel_ ? summaryModel_->sortRole() : Qt::DisplayRole);
}

void
CQChartsTablePlot::
setSortRole(int r)
{
  if (r != sortRole()) {
    if (summaryModel_)
      summaryModel_->setSortRole(r);

    updateRangeAndObjs();
  }
}

Qt::SortOrder
CQChartsTablePlot::
sortOrder() const
{
  return (summaryModel_ ? summaryModel_->sortOrder() : Qt::AscendingOrder);
}

void
CQChartsTablePlot::
setSortOrder(Qt::SortOrder r)
{
  if (r != sortOrder()) {
    if (summaryModel_)
      summaryModel_->setSortOrder(r);

    updateRangeAndObjs();
  }
}

//---

int
CQChartsTablePlot::
pageSize() const
{
  return (summaryModel_ ? summaryModel_->pageSize() : -1);
}

void
CQChartsTablePlot::
setPageSize(int i)
{
  if (i != pageSize()) {
    if (summaryModel_)
      summaryModel_->setPageSize(i);

    updateRangeAndObjs();
  }
}

int
CQChartsTablePlot::
currentPage() const
{
  return (summaryModel_ ? summaryModel_->currentPage() : 0);
}

void
CQChartsTablePlot::
setCurrentPage(int i)
{
  if (i != currentPage()) {
    if (summaryModel_)
      summaryModel_->setCurrentPage(i);

    updateRangeAndObjs();
  }
}

//---

const CQChartsTablePlot::RowNums &
CQChartsTablePlot::
rowNums() const
{
  static CQChartsTablePlot::RowNums dummyRowNums;

  return (summaryModel_ ? summaryModel_->rowNums() : dummyRowNums);
}

void
CQChartsTablePlot::
setRowNums(const RowNums &rowNums)
{
  if (summaryModel_)
    summaryModel_->setRowNums(rowNums);

  updateRangeAndObjs();
}

QString
CQChartsTablePlot::
rowNumsStr() const
{
  QStringList strs;

  for (const auto &r : rowNums())
    strs << QString::number(r);

  return strs.join(" ");
}

void
CQChartsTablePlot::
setRowNumsStr(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return;

  RowNums rowNums;

  for (const auto &s : strs) {
    bool ok;

    long i = CQChartsUtil::toInt(s, ok);

    if (ok)
      rowNums.push_back(int(i));
  }

  setRowNums(rowNums);
}

//---

void
CQChartsTablePlot::
setRowColumnVisible(bool b)
{
  CQChartsUtil::testAndSet(rowColumnData_.visible, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTablePlot::
setRowColumnColor(const Color &c)
{
  CQChartsUtil::testAndSet(rowColumnData_.color, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setRowColumnFont(const Font &f)
{
  CQChartsUtil::testAndSet(rowColumnData_.font, f, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
setHHeaderVisible(bool b)
{
  CQChartsUtil::testAndSet(hheaderData_.visible, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTablePlot::
setHHeaderFormatted(bool b)
{
  CQChartsUtil::testAndSet(hheaderData_.formatted, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTablePlot::
setHHeaderColor(const Color &c)
{
  CQChartsUtil::testAndSet(hheaderData_.color, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setHHeaderFont(const Font &f)
{
  CQChartsUtil::testAndSet(hheaderData_.font, f, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
setVHeaderVisible(bool b)
{
  CQChartsUtil::testAndSet(vheaderData_.visible, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTablePlot::
setVHeaderFormatted(bool b)
{
  CQChartsUtil::testAndSet(vheaderData_.formatted, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTablePlot::
setVHeaderColor(const Color &c)
{
  CQChartsUtil::testAndSet(vheaderData_.color, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setVHeaderFont(const Font &f)
{
  CQChartsUtil::testAndSet(vheaderData_.font, f, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTablePlot::
setVHeaderWidth(const Length &l)
{
  CQChartsUtil::testAndSet(vheaderData_.size, l, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
setGridColor(const Color &c)
{
  CQChartsUtil::testAndSet(gridData_.color, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setGridWidth(const Length &l)
{
  CQChartsUtil::testAndSet(gridData_.width, l, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
setCellColor(const Color &c)
{
  CQChartsUtil::testAndSet(cellColor_, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setInsideColor(const Color &c)
{
  CQChartsUtil::testAndSet(insideColor_, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setSelectedColor(const Color &c)
{
  CQChartsUtil::testAndSet(selectedColor_, c, [&]() { updateObjs(); } );
}

//---

void
CQChartsTablePlot::
setIndent(double r)
{
  CQChartsUtil::testAndSet(indent_, r, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setCellMargin(int i)
{
  CQChartsUtil::testAndSet(cellMargin_, i, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTablePlot::
setBarXMargin(int i)
{
  CQChartsUtil::testAndSet(barXMargin_, i, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTablePlot::
setBarYMargin(int i)
{
  CQChartsUtil::testAndSet(barYMargin_, i, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
setFollowView(bool b)
{
  CQChartsUtil::testAndSet(followView_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "columns", "columns", "Columns");

  // mode
  addProp("options", "mode"   , "mode"   , "Set mode"    );
  addProp("options", "maxRows", "maxRows", "Set max rows")->setMinValue(1);

  // sort mode
  addProp("options", "sortColumn", "sortColumn", "Set sort column");
  addProp("options", "sortRole"  , "sortRole"  , "Set sort role"  , true);
  addProp("options", "sortOrder" , "sortOrder" , "Set sort order" );

  // page mode
  addProp("options", "pageSize"   , "pageSize"   , "Set page size"   )->setMinValue(1);
  addProp("options", "currentPage", "currentPage", "Set current page")->setMinValue(0);

  // rows mode
  addProp("options", "rowNums", "rowNums", "Explicit row numbers for ROWS mode");

  // horizontal header
  addProp     ("hheader", "hheaderVisible"  , "visible"  , "Horizontal Header visible");
  addProp     ("hheader", "hheaderFormatted", "formatted", "Horizontal Header text formatted");
  addStyleProp("hheader", "hheaderColor"    , "color"    , "Horizontal Header color");
  addStyleProp("hheader", "hheaderFont"     , "font"     , "Horizontal Header font" );

  // vertical header
  addProp     ("vheader", "vheaderVisible"  , "visible"  , "Vertical Header visible");
  addProp     ("vheader", "vheaderFormatted", "formatted", "Vertical Header text formatted");
  addStyleProp("vheader", "vheaderColor"    , "color"    , "Vertical Header color");
  addStyleProp("vheader", "vheaderFont"     , "font"     , "Vertical Header font" );
  addStyleProp("vheader", "vheaderWidth"    , "width"    , "Vertical Header width" );

  // row column
  addProp     ("rowColumn", "rowColumnVisible", "visible"  , "Row Column visible");
  addStyleProp("rowColumn", "rowColumnColor"  , "color"    , "Row Column color");
  addStyleProp("rowColumn", "rowColumnFont"   , "font"     , "Row Column font" );

  // cells
  addStyleProp("cells", "cellColor" , "color"     , "Cells color");
  addStyleProp("cells", "cellMargin", "margin"    , "Cells margin")->setMinValue(0);
  addStyleProp("cells", "barXMargin", "barXMargin", "Cells bar x margin")->setMinValue(0);
  addStyleProp("cells", "barYMargin", "barYMargin", "Cells bar y margin")->setMinValue(0);

  // grid
  addStyleProp("grid", "gridColor", "color", "Grid color");
  addStyleProp("grid", "gridWidth", "width", "Grid line width");

  // options
  addStyleProp("options", "insideColor"  , "insideColor"  , "Cell inside color");
  addStyleProp("options", "selectedColor", "selectedColor", "Cell selected color");

  addProp("options", "followView", "followView", "Follow view");

  addStyleProp("options", "indent", "indent", "Hierarchical row indent")->setMinValue(0.0);
}

CQChartsGeom::Range
CQChartsTablePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsTablePlot::calcRange");

  //---

  // always return range (0, 0) -> (1, 1)
  Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(1.0, 1.0);

  //---

  calcTableSize();

  return dataRange;
}

void
CQChartsTablePlot::
calcTableSize() const
{
  CQPerfTrace trace("CQChartsTablePlot::calcTableSize");

  if (! currentModelData())
    return;

  //---

  auto *th = const_cast<CQChartsTablePlot *>(this);

  //---

  QModelIndexList expandInds;

  if (isFollowView())
    view()->expandedModelIndices(expandInds);

  //---

  th->tableData_.expandInds = expandInds;

  th->tableData_.font = view()->viewFont(this->font());
  th->tableData_.font = CQChartsUtil::scaleFontSize(th->tableData_.font, fontScale_);

  th->tableData_.hheaderFont = view()->viewFont(this->hheaderFont());
  th->tableData_.hheaderFont =
    CQChartsUtil::scaleFontSize(th->tableData_.hheaderFont, fontScale_);

  th->tableData_.vheaderFont = view()->viewFont(this->vheaderFont());
  th->tableData_.vheaderFont =
    CQChartsUtil::scaleFontSize(th->tableData_.vheaderFont, fontScale_);

  th->tableData_.rowColumnFont = view()->viewFont(this->rowColumnFont());
  th->tableData_.rowColumnFont =
    CQChartsUtil::scaleFontSize(th->tableData_.rowColumnFont, fontScale_);

  //th->tabbedFont_ = th->tableData_.font;

  QFontMetricsF hfm(th->tableData_.hheaderFont);
  QFontMetricsF vfm(th->tableData_.vheaderFont);
  QFontMetricsF rfm(th->tableData_.rowColumnFont);
  QFontMetricsF fm (th->tableData_.font);

  th->tableData_.nc = columns_.count();

  const auto &model = this->currentModel();

  if (summaryModel_) {
    th->tableData_.nr       = summaryModel_->rowCount();
    th->tableData_.maxDepth = 0;
  }
  else {
    CQChartsModelUtil::hierData(charts(), model.data(), th->tableData_.nr,
                                th->tableData_.maxDepth);
  }

  th->tableData_.pmargin    = cellMargin();
  th->tableData_.pSortWidth = int(hfm.horizontalAdvance("X") + 4);

  th->tableData_.phrh = hfm.height() + 2.0*tableData_.pmargin;
  th->tableData_.prh  =  fm.height() + 2.0*tableData_.pmargin;

  // calc column widths
  if (isRowColumnVisible()) {
    auto &data = th->tableData_.rowNumberColumnData;

    const int power = CMathRound::RoundUp(log10(tableData_.nr));

    data.pwidth  = power*rfm.horizontalAdvance("X") + 2.0*tableData_.pmargin;
    data.numeric = false;
  }

  if (isVHeaderVisible()) {
    auto &data = th->tableData_.verticalHeaderColumnData;

    data.pwidth  = 0.0;
    data.numeric = false;

    auto hw = vheaderWidth();

    if (! hw.isValid()) {
      for (int r = 0; r < tableData_.nr; ++r) {
        bool ok;
        auto str = getVerticalHeader(r, ok);
        if (! ok) continue;

        double cw = vfm.horizontalAdvance(str) + 2.0*tableData_.pmargin;

        data.pwidth = std::max(data.pwidth, cw);
      }
    }
    else {
      data.pwidth = lengthPixelWidth(hw);

      if (isVHeaderFormatted()) {
        for (int r = 0; r < tableData_.nr; ++r) {
          bool ok;
          auto str = getVerticalHeader(r, ok);
          if (! ok) continue;


          auto pbbox = BBox(0, 0, data.pwidth, 9999);

          QStringList              strs;
          CQChartsUtil::FormatData formatData;

          if (! CQChartsUtil::formatStringInRect(str, tableData_.vheaderFont,
                                                 pbbox, strs, formatData))
            continue;

          auto n = strs.length();
          auto h = n*vfm.height() + 2.0*tableData_.pmargin;

          th->tableData_.prh = std::max(th->tableData_.prh, h);
        }
      }
    }
  }

  int maxHeaderStrs = 1;

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    auto *columnDetails = this->columnDetails(c);
    if (! columnDetails) continue;

    auto &data = th->tableData_.columnDataMap[c];

    bool ok;
    auto str = getHorizontalHeader(c, ok);
    if (! ok) continue;

    double cw = hfm.horizontalAdvance(str) + 2.0*tableData_.pmargin + tableData_.pSortWidth;

    if (i == 0)
      cw += tableData_.maxDepth*indent(); // add hierarchical indent

    data.pwidth  = cw;
    data.numeric = columnDetails->isNumeric();

    data.prefWidth = columnDetails->preferredWidth();

    if (data.prefWidth > 0 && isHHeaderFormatted()) {
      auto pbbox = BBox(0, 0, data.prefWidth, 9999);

      QStringList              strs;
      CQChartsUtil::FormatData formatData;

      if (! CQChartsUtil::formatStringInRect(str, tableData_.hheaderFont,
                                             pbbox, strs, formatData))
        continue;

      maxHeaderStrs = std::max(maxHeaderStrs, strs.length());
    }
  }

  if (maxHeaderStrs > 1)
    th->tableData_.phrh = maxHeaderStrs*hfm.height() + 2*tableData_.pmargin;

  //---

  // update column widths and number of visible rows
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsTablePlot *tablePlot, TableData &tableData) :
     tablePlot_(tablePlot), tableData_(tableData), fm_(tableData_.font) {
    }

    // process hier row
    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      if (! expanded_) return State::SKIP;

      //---

      expandStack_.push_back(expanded_);

      if (tablePlot_->isFollowView()) {
        auto ind = model_->index(data.row, 0, data.parent);

        expanded_ = tableData_.expandInds.contains(ind);
      }
      else
        expanded_ = true;

      return State::OK;
    }

    // post process hier row
    State hierPostVisit(const QAbstractItemModel *, const VisitData &) override {
      expanded_ = expandStack_.back();

      expandStack_.pop_back();

      return State::OK;
    }

    // process leaf row
    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (! expanded_) return State::SKIP;

      //---

      for (int i = 0; i < tableData_.nc; ++i) {
        const auto &c = tablePlot_->columns().getColumn(i);

        ModelIndex ind(tablePlot_, data.row, c, data.parent);

        bool ok;

        auto str = tablePlot_->modelString(const_cast<QAbstractItemModel *>(model_), ind,
                                           Qt::DisplayRole, ok);
        if (! ok) continue;

        auto &data = tableData_.columnDataMap[c];

        double cw = fm_.horizontalAdvance(str) + 2.0*tableData_.pmargin;

        if (i == 0)
          cw += tableData_.maxDepth*tablePlot_->indent(); // add hierarchical indent

        data.pwidth = std::max(data.pwidth, cw);
      }

      return State::OK;
    }

   private:
    const CQChartsTablePlot* tablePlot_ { nullptr };
    TableData&               tableData_;
    QFontMetricsF            fm_;
    bool                     expanded_  { true };
    std::vector<int>         expandStack_;
  };

  RowVisitor visitor(this, th->tableData_);

  visitor.setPlot(this);

  //visitor.init();

  if (summaryModel_)
    CQChartsModelVisit::exec(charts(), summaryModel_, visitor);
  else {
    const auto &model = this->currentModel();

    CQChartsModelVisit::exec(charts(), model.data(), visitor);
  }

  th->tableData_.nvr = visitor.numProcessedRows();

  //---

  // set full table width
  th->tableData_.pcw = 0.0;

  if (isRowColumnVisible()) {
    const auto &data = th->tableData_.rowNumberColumnData;

    th->tableData_.pcw += data.pwidth;
  }

  if (isVHeaderVisible()) {
    const auto &data = th->tableData_.verticalHeaderColumnData;

    th->tableData_.pcw += data.pwidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    const auto &data = th->tableData_.columnDataMap[c];

    if (data.prefWidth > 0)
      th->tableData_.pcw += data.prefWidth + 2.0*tableData_.pmargin;
    else
      th->tableData_.pcw += data.pwidth;
  }
}

QString
CQChartsTablePlot::
getHorizontalHeader(const Column &c, bool &ok) const
{
  if (summaryModel())
    return CQChartsModelUtil::modelHHeaderString(summaryModel(), c, ok);
  else {
    const auto &model = this->currentModel();

    return CQChartsModelUtil::modelHHeaderString(model.data(), c, ok);
  }
}

QString
CQChartsTablePlot::
getVerticalHeader(int r, bool &ok) const
{
  if (summaryModel())
    return CQChartsModelUtil::modelVHeaderString(summaryModel(), r, ok);
  else {
    const auto &model = this->currentModel();

    return CQChartsModelUtil::modelVHeaderString(model.data(), r, ok);
  }
}

bool
CQChartsTablePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsTablePlot::createObjs");

  //---

  updatePosition();

  initDrawData();

  for (const auto &pc : hheaderObjMap_) {
    const auto &headerObjData = pc.second;

    auto *obj = createHHeaderObj(headerObjData);

    obj->connectDataChanged(this, SLOT(updateSlot()));

    objs.push_back(obj);
  }

  for (const auto &pr : rowNumberObjMap_) {
    const auto &headerObjData = pr.second;

    auto *obj = createRowNumberObj(headerObjData);

    obj->connectDataChanged(this, SLOT(updateSlot()));

    objs.push_back(obj);
  }

  for (const auto &pr : vheaderObjMap_) {
    const auto &headerObjData = pr.second;

    auto *obj = createVHeaderObj(headerObjData);

    obj->connectDataChanged(this, SLOT(updateSlot()));

    objs.push_back(obj);
  }

  for (const auto &pc : cellObjMap_) {
    const auto &cellObjData = pc.second;

    auto *obj = createCellObj(cellObjData);

    obj->connectDataChanged(this, SLOT(updateSlot()));

    objs.push_back(obj);
  }

  return true;
}

double
CQChartsTablePlot::
getPanY(bool /*is_shift*/) const
{
  return windowToViewHeight(tableData_.rh);
}

//------

void
CQChartsTablePlot::
autoFit()
{
  if (fitData_.fitHorizontal || fitData_.fitVertical) {
    calcTableSize();

    auto pixelRect = calcTablePixelRect();

    double xscale { 1.0 };
    double yscale { 1.0 };

    if (fitData_.fitHorizontal)
      xscale = pixelRect.getWidth()/(2.0*tableData_.pmargin + tableData_.pcw);

    if (fitData_.fitVertical)
      yscale = pixelRect.getHeight()/(2.0*tableData_.pmargin + tableData_.prh*tableData_.nvr);

    double scale = 1.0;

    if      (fitData_.fitHorizontal && fitData_.fitVertical)
      scale = std::min(xscale, yscale);
    else if (fitData_.fitHorizontal)
      scale = xscale;
    else if (fitData_.fitVertical)
      scale = yscale;

    if (scale != 1.0) {
      fontScale_ *= scale;

      //---

      CQChartsPlot::autoFit();

      updateObjs();
    }
  }
  else {
    CQChartsPlot::autoFit();

    updateObjs();
  }
}

//------

bool
CQChartsTablePlot::
addMenuItems(QMenu *menu, const Point &)
{
  menu_ = menu;

  //---

  menu->addSeparator();

  auto *modeMenu = new QMenu("Table Mode", menu);

  for (const auto &mode : modes())
    (void) addMenuCheckedAction(modeMenu, modeName(mode), this->mode() == mode,
                                SLOT(setModeSlot(bool)));

  menu->addMenu(modeMenu);

  //---

  auto *maxRowsMenu = new QMenu("Max Rows", menu);

  maxRowsSpin_ = new CQIntegerSpin;

  maxRowsSpin_->setRange(1, 9999);
  maxRowsSpin_->setValue(maxRows());
  maxRowsSpin_->setToolTip("Maximum Number of Rows");

  connect(maxRowsSpin_, SIGNAL(valueChanged(int)), this, SLOT(maxRowsSlot()));
  connect(maxRowsSpin_, SIGNAL(editingFinished()), this, SLOT(maxRowsSlot()));

  maxRowsMenu->addAction(new CQChartsWidgetAction(maxRowsSpin_));

  menu->addMenu(maxRowsMenu);

  //---

  auto *sortColumnMenu = new QMenu("Sort Column", menu);

  sortColumnSpin_ = new CQIntegerSpin;

  sortColumnSpin_->setRange(1, columns_.count());
  sortColumnSpin_->setValue(std::max(sortColumn().column(), 0) + 1);
  sortColumnSpin_->setToolTip("Sort Column Number");

  connect(sortColumnSpin_, SIGNAL(valueChanged(int)), this, SLOT(sortColumnSlot()));
  connect(sortColumnSpin_, SIGNAL(editingFinished()), this, SLOT(sortColumnSlot()));

  sortColumnMenu->addAction(new CQChartsWidgetAction(sortColumnSpin_));

  menu->addMenu(sortColumnMenu);

  //---

  auto *pageSizeMenu = new QMenu("Page Size", menu);

  pageSizeSpin_ = new CQIntegerSpin;

  pageSizeSpin_->setRange(1, 9999);
  pageSizeSpin_->setValue(pageSize());
  pageSizeSpin_->setToolTip("Page Size");

  connect(pageSizeSpin_, SIGNAL(valueChanged(int)), this, SLOT(pageSizeSlot()));
  connect(pageSizeSpin_, SIGNAL(editingFinished()), this, SLOT(pageSizeSlot()));

  pageSizeMenu->addAction(new CQChartsWidgetAction(pageSizeSpin_));

  menu->addMenu(pageSizeMenu);

  //---

  auto *pageNumMenu = new QMenu("Page Number", menu);

  pageNumSpin_ = new CQIntegerSpin;

  pageNumSpin_->setRange(1, 9999);
  pageNumSpin_->setValue(currentPage() + 1);
  pageNumSpin_->setToolTip("Page Number");

  connect(pageNumSpin_, SIGNAL(valueChanged(int)), this, SLOT(pageNumSlot()));
  connect(pageNumSpin_, SIGNAL(editingFinished()), this, SLOT(pageNumSlot()));

  pageNumMenu->addAction(new CQChartsWidgetAction(pageNumSpin_));

  menu->addMenu(pageNumMenu);

  //---

  return true;
}

//------

void
CQChartsTablePlot::
setModeSlot(bool b)
{
  if (! b) return;

  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  auto name = action->text();

  for (const auto &mode : modes()) {
    if (modeName(mode) == name) {
      setMode(mode);
      return;
    }
  }
}

void
CQChartsTablePlot::
maxRowsSlot()
{
  setMaxRows(maxRowsSpin_->value());

  if (sender()->metaObject()->method(senderSignalIndex()).name() == "editingFinished")
    menu_->close();
}

void
CQChartsTablePlot::
sortColumnSlot()
{
  setSortColumn(ColumnNum(sortColumnSpin_->value() - 1));

  if (sender()->metaObject()->method(senderSignalIndex()).name() == "editingFinished")
    menu_->close();
}

void
CQChartsTablePlot::
pageSizeSlot()
{
  setPageSize(pageSizeSpin_->value());

  if (sender()->metaObject()->method(senderSignalIndex()).name() == "editingFinished")
    menu_->close();
}

void
CQChartsTablePlot::
pageNumSlot()
{
  setCurrentPage(pageNumSpin_->value() - 1);

  if (sender()->metaObject()->method(senderSignalIndex()).name() == "editingFinished")
    menu_->close();
}

//------

void
CQChartsTablePlot::
wheelHScroll(int delta)
{
  auto font = view()->plotFont(this, this->font());

  QFontMetrics fm(font);

  auto xw = fm.horizontalAdvance("X");

  //hscrollSlot(scrollData_.hpos + (delta > 0 ? xw : -xm));
  scrollData_.hbar->setValue(scrollData_.hpos + (delta > 0 ? xw : -xw));
}

void
CQChartsTablePlot::
wheelVScroll(int)
{
}

void
CQChartsTablePlot::
wheelZoom(const Point &, int delta)
{
  auto font = view()->viewFont(this->font());

  QFontMetrics fm(font);

  //vscrollSlot(scrollData_.vpos + (delta > 0 ? -fm.height() : fm.height()));
  scrollData_.vbar->setValue(scrollData_.vpos + (delta > 0 ? -fm.height() : fm.height()));
}

//---

void
CQChartsTablePlot::
postResize()
{
  calcTableSize();

  CQChartsPlot::postResize();

  updateRangeAndObjs();
}

//------

bool
CQChartsTablePlot::
hasBackground() const
{
  return true;
}

void
CQChartsTablePlot::
execDrawBackground(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsTablePlot::execDrawBackground");

  //---

  if (! currentModelData())
    return;

  device->save();

  setClipRect(device);

  drawTable(device);

  device->restore();
}

void
CQChartsTablePlot::
drawTable(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsTablePlot::drawTable");

  updateScrollBars();

  updatePosition();

  drawTableBackground(device);
}

void
CQChartsTablePlot::
initDrawData() const
{
  CQPerfTrace trace("CQChartsTablePlot::initDrawData");

  //---

  auto *th = const_cast<CQChartsTablePlot *>(this);

  th->hheaderObjMap_  .clear();
  th->vheaderObjMap_  .clear();
  th->rowNumberObjMap_.clear();
  th->cellObjMap_     .clear();

  //---

  th->tableData_.hrh = pixelToWindowHeight(th->tableData_.phrh);
  th->tableData_.rh  = pixelToWindowHeight(th->tableData_.prh);

  //---

  // calc column widths
  double x = th->tableData_.xo;

  if (isRowColumnVisible()) {
    auto &data = th->tableData_.rowNumberColumnData;

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawX     = x;
    data.drawWidth = data.width;

    x += data.drawWidth;
  }

  if (isVHeaderVisible()) {
    auto &data = th->tableData_.verticalHeaderColumnData;

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawX     = x;
    data.drawWidth = data.width;

    x += data.drawWidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    auto &data = th->tableData_.columnDataMap[c];

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawX     = x;
    data.drawWidth = data.width;

    if (data.prefWidth > 0)
      data.drawWidth = pixelToWindowWidth(data.prefWidth + 2.0*tableData_.pmargin);

    th->tableData_.columnDataMap[c] = data;

    x += data.drawWidth;
  }

  //---

  assert(x); // ?

  createTableObjData();
}

void
CQChartsTablePlot::
updateScrollBars() const
{
  CQPerfTrace trace("CQChartsTablePlot::updateScrollBars");

  //---

  if (! isVisible()) {
    scrollData_.hbar->setVisible(false);
    scrollData_.vbar->setVisible(false);

    return;
  }

  //---

  auto *th = const_cast<CQChartsTablePlot *>(this);

  //---

  auto pixelRect = calcTablePixelRect();

  double pdx = pixelRect.getWidth () - 2.0*tableData_.pmargin - tableData_.pcw;
  double pdy = pixelRect.getHeight() - 2.0*tableData_.pmargin - tableData_.prh*tableData_.nvr;

  if (isHHeaderVisible())
    pdy -= tableData_.phrh;

  //---

  bool changed = false;

  bool hbarVisible = (pdx < 0.0);
  bool vbarVisible = (pdy < 0.0);

  if (hbarVisible != scrollData_.hbar->isVisible()) {
    scrollData_.hbar->setVisible(hbarVisible);
    changed = true;
  }

  if (vbarVisible != scrollData_.vbar->isVisible()) {
    scrollData_.vbar->setVisible(vbarVisible);
    changed = true;
  }

  if (scrollData_.hbar->isVisible()) {
    scrollData_.hbar->move  (int(pixelRect.getXMin()),
                             int(pixelRect.getYMax() - scrollData_.pixelBarSize));
    scrollData_.hbar->resize(int(pixelRect.getWidth()), int(scrollData_.pixelBarSize));

    scrollData_.hbar->setRange(0, int(-pdx));
    scrollData_.hbar->setPageStep(int(pixelRect.getWidth() - 2.0*tableData_.pmargin));
  }
  else
    th->scrollData_.hpos = 0;

  if (scrollData_.vbar->isVisible()) {
    scrollData_.vbar->move(int(pixelRect.getXMax() - scrollData_.pixelBarSize),
                           int(pixelRect.getYMin()));
    scrollData_.vbar->resize(int(scrollData_.pixelBarSize), int(pixelRect.getHeight()));

    scrollData_.vbar->setRange(0, int(-pdy));
    scrollData_.vbar->setPageStep(int(pixelRect.getHeight() - 2.0*tableData_.pmargin));
  }
  else
    th->scrollData_.vpos = 0;

  //---

  if (changed)
    th->updateRangeAndObjs();
}

void
CQChartsTablePlot::
updatePosition() const
{
  CQPerfTrace trace("CQChartsTablePlot::updatePosition");

  //---

  auto *th = const_cast<CQChartsTablePlot *>(this);

  //---

  auto pixelRect = calcTablePixelRect();

  double pdx = pixelRect.getWidth () - 2.0*tableData_.pmargin - tableData_.pcw;
  double pdy = pixelRect.getHeight() - 2.0*tableData_.pmargin - tableData_.prh*tableData_.nvr;

  if (isHHeaderVisible())
    pdy -= tableData_.phrh;

  //---

  if (scrollData_.hbar && scrollData_.hbar->isVisible()) {
    th->tableData_.sx = pixelToSignedWindowWidth(-scrollData_.hpos);
    th->tableData_.dx = 0.0;
    th->tableData_.xo = th->tableData_.dx;
  }
  else {
    th->tableData_.sx = 0.0;
    th->tableData_.dx = pixelToSignedWindowWidth(pdx/2.0);
    th->tableData_.xo = std::max(th->tableData_.dx, 0.0);
  }

  if (scrollData_.vbar && scrollData_.vbar->isVisible()) {
    th->tableData_.sy = pixelToSignedWindowHeight(-scrollData_.vpos);
    th->tableData_.dy = 0.0;
    th->tableData_.yo = pixelToSignedWindowHeight(pdy) - th->tableData_.dy;
  }
  else {
    th->tableData_.sy = 0.0;
    th->tableData_.dy = pixelToSignedWindowHeight(pdy/2.0);
    th->tableData_.yo = std::max(th->tableData_.dy, 0.0);
  }
}

void
CQChartsTablePlot::
drawTableBackground(PaintDevice *device) const
{
  if (! isVisible())
    return;

  //---

  CQPerfTrace trace("CQChartsTablePlot::drawTableBackground");

  //---

  auto *th = const_cast<CQChartsTablePlot *>(this);

  device->save();

  auto pixelRect = calcTablePixelRect();

  device->setClipRect(pixelToWindow(pixelRect));

  //---

  // calc column widths
  double x = th->tableData_.xo + th->tableData_.sx;

  if (isRowColumnVisible()) {
    auto &data = th->tableData_.rowNumberColumnData;

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawX     = x;
    data.drawWidth = data.width;

    x += data.drawWidth;
  }

  if (isVHeaderVisible()) {
    auto &data = th->tableData_.verticalHeaderColumnData;

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawX     = x;
    data.drawWidth = data.width;

    x += data.drawWidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    auto &data = th->tableData_.columnDataMap[c];

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawX     = x;
    data.drawWidth = data.width;

    if (data.prefWidth > 0)
      data.drawWidth = pixelToWindowWidth(data.prefWidth + 2.0*tableData_.pmargin);

    th->tableData_.columnDataMap[c] = data;

    x += data.drawWidth;
  }

  //---

  // size of all rows
  const double trh = tableData_.nvr*th->tableData_.rh;

  double x1 = th->tableData_.xo + th->tableData_.sx;       // left
  double y1 = th->tableData_.yo + trh - th->tableData_.sy; // top
  double x2 = x;                                           // right
  double y2 = th->tableData_.yo;                           // bottom

  //---

  // add optional header height
  if (isHHeaderVisible())
    y1 += th->tableData_.hrh;

  // draw horizontal header background
  if (isHHeaderVisible()) {
    PenBrush headerPenBrush;

    setPenBrush(headerPenBrush,
      PenData(false), BrushData(true, interpColor(hheaderColor(), ColorInd())));

    CQChartsDrawUtil::setPenBrush(device, headerPenBrush);

    if (x2 > x1) {
      BBox bbox(x1, y1 - th->tableData_.hrh, x2, y1);

      device->fillRect(bbox);
    }
  }

  // draw cells background
  PenBrush cellPenBrush;

  setPenBrush(cellPenBrush,
    PenData(false), BrushData(true, interpColor(cellColor(), ColorInd())));

  CQChartsDrawUtil::setPenBrush(device, cellPenBrush);

  if (x2 > x1 && tableData_.nvr > 0) {
    BBox bbox(x1, y2, x2, y2 + trh);

    device->fillRect(bbox);
  }

  // draw row column background
  if (isRowColumnVisible()) {
    const auto &data = th->tableData_.rowNumberColumnData;

    PenBrush headerPenBrush;

    setPenBrush(headerPenBrush,
      PenData(false), BrushData(true, interpColor(rowColumnColor(), ColorInd())));

    CQChartsDrawUtil::setPenBrush(device, headerPenBrush);

    if (x2 > x1) {
      BBox bbox(data.drawX, y1 - th->tableData_.hrh, data.drawX + data.drawWidth, y2);

      device->fillRect(bbox);
    }
  }

  // draw vertical header background
  if (isVHeaderVisible()) {
    const auto &data = th->tableData_.verticalHeaderColumnData;

    PenBrush headerPenBrush;

    setPenBrush(headerPenBrush,
      PenData(false), BrushData(true, interpColor(vheaderColor(), ColorInd())));

    CQChartsDrawUtil::setPenBrush(device, headerPenBrush);

    if (x2 > x1) {
      BBox bbox(data.drawX, y1 - th->tableData_.hrh, data.drawX + data.drawWidth, y2);

      device->fillRect(bbox);
    }
  }

  //---

  // draw table column lines
  PenBrush gridPenBrush;

  setPenBrush(gridPenBrush,
    PenData(true, interpColor(gridColor(), ColorInd()), Alpha(), gridWidth()),
    BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, gridPenBrush);

  x = x1;

  // row column vertical line
  if (isRowColumnVisible()) {
    const auto &data = th->tableData_.rowNumberColumnData;

    device->drawLine(Point(x, y1), Point(x, y2));

    x += data.drawWidth;
  }

  // vertical header column vertical line
  if (isVHeaderVisible()) {
    const auto &data = th->tableData_.verticalHeaderColumnData;

    device->drawLine(Point(x, y1), Point(x, y2));

    x += data.drawWidth;
  }

  // column vertical lines
  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    const auto &data = th->tableData_.columnDataMap[c];

    device->drawLine(Point(x, y1), Point(x, y2));

    x += data.drawWidth;
  }

  // right edge
  device->drawLine(Point(x, y1), Point(x, y2));

  // draw header and row lines (bottom to top)
  double y = th->tableData_.yo - th->tableData_.sy;

  for (int i = 0; i < tableData_.nvr; ++i) {
    device->drawLine(Point(x1, y), Point(x2, y));

    y += tableData_.rh;
  }

  if (isHHeaderVisible()) {
    device->drawLine(Point(x1, y), Point(x2, y));

    y += tableData_.hrh;
  }

  // top edge
  device->drawLine(Point(x1, y), Point(x2, y));

  //---

  device->restore();
}

void
CQChartsTablePlot::
createTableObjData() const
{
  CQPerfTrace trace("CQChartsTablePlot::createTableObjData");

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsTablePlot *tablePlot, const TableData &tableData_) :
     tablePlot_(tablePlot), tableData_(tableData_) {
      xm_ = tablePlot_->pixelToWindowWidth(tableData_.pmargin);
      xd_ = tablePlot_->pixelToWindowWidth(tablePlot_->indent());
    }

    // draw hier row
    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      if (! expanded_) return State::SKIP;

      //---

      // draw header
      if (tablePlot_->isHHeaderVisible())
        if (data.vrow == 0) {
          drawHeader();
      }

      //---

    //const double y = tableData_.yo + (tableData_.nvr - data.vrow)*tableData_.rh - tableData_.hrh;
      const double y = tableData_.yo + (tableData_.nvr - data.vrow - 1)*tableData_.rh;

      double x = tableData_.xo;

      //---

      // draw row number
      if (tablePlot_->isRowColumnVisible()) {
        const auto &cdata = tableData_.rowNumberColumnData;

        drawRowNumber(x, y, data.vrow + 1);

        x += cdata.drawWidth;
      }

      //---

      // draw vertical header
      if (tablePlot_->isVHeaderVisible()) {
        const auto &cdata = tableData_.verticalHeaderColumnData;

        bool ok;
        auto str = tablePlot_->getVerticalHeader(data.vrow, ok);

        drawVerticalHeader(x, y, data.vrow, str);

        x += cdata.drawWidth;
      }

      //---

      // draw cell values
      drawCellValues(x, y, data);

      //---

      expandStack_.push_back(expanded_);

      if (tablePlot_->isFollowView()) {
        auto ind = model_->index(data.row, 0, data.parent);

        expanded_ = tableData_.expandInds.contains(ind);
      }
      else
        expanded_ = true;

      return State::OK;
    }

    // post process hier row
    State hierPostVisit(const QAbstractItemModel *, const VisitData &) override {
      expanded_ = expandStack_.back();

      expandStack_.pop_back();

      return State::OK;
    }

    // draw leaf row
    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (! expanded_) return State::SKIP;

      //---

      // draw header
      if (tablePlot_->isHHeaderVisible())
        if (data.vrow == 0) {
          drawHeader();
      }

      //---

    //const double y = tableData_.yo + (tableData_.nvr - data.vrow)*tableData_.rh - tableData_.hrh;
      const double y = tableData_.yo + (tableData_.nvr - data.vrow - 1)*tableData_.rh;

      double x = tableData_.xo;

      //---

      // draw row number
      if (tablePlot_->isRowColumnVisible()) {
        const auto &cdata = tableData_.rowNumberColumnData;

        drawRowNumber(x, y, data.vrow + 1);

        x += cdata.drawWidth;
      }

      //---

      // draw vertical header
      if (tablePlot_->isVHeaderVisible()) {
        const auto &cdata = tableData_.verticalHeaderColumnData;

        bool ok;
        auto str = tablePlot_->getVerticalHeader(data.vrow, ok);

        drawVerticalHeader(x, y, data.vrow, str);

        x += cdata.drawWidth;
      }

      //---

      // draw cell values
      drawCellValues(x, y, data);

      //---

      return State::OK;
    }

    void drawHeader() {
      const double y = tableData_.yo + tableData_.nvr*tableData_.rh;

      double x = tableData_.xo;

      // draw empty row number area
      if (tablePlot_->isRowColumnVisible()) {
        auto &headerObjData = tablePlot_->getRowColumnHeaderObjData();

        headerObjData.str = " ";

        //---

        const auto &cdata = tableData_.rowNumberColumnData;

        headerObjData.rect = BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.hrh);

        //---

        x += cdata.drawWidth;
      }

      // draw empty vertical header area
      if (tablePlot_->isVHeaderVisible()) {
        auto &headerObjData = tablePlot_->getVHeaderHeaderObjData();

        headerObjData.str = " ";

        //---

        const auto &cdata = tableData_.verticalHeaderColumnData;

        headerObjData.rect = BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.hrh);

        //--

        x += cdata.drawWidth;
      }

      // draw column headers
      for (int ic = 0; ic < tableData_.nc; ++ic) {
        const auto &c = tablePlot_->columns().getColumn(ic);

        bool ok;

        auto str = tablePlot_->getHorizontalHeader(c, ok);
        if (! ok) continue;

        //---

        auto &headerObjData = tablePlot_->getHHeaderHeaderObjData(c);

        headerObjData.str = str;

        //---

        const auto &cdata = tableData_.columnDataMap[c];

        headerObjData.align = Qt::AlignHCenter | Qt::AlignVCenter;

        headerObjData.rect = BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.hrh);

        //---

        x += cdata.drawWidth;
      }
    }

    void drawRowNumber(double x, double y, int n) {
      const auto &cdata = tableData_.rowNumberColumnData;

      //---

      auto &objData = tablePlot_->getRowNumberObjData(n);

      objData.align = Qt::AlignRight | Qt::AlignVCenter;

      objData.rect = BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.rh);

      objData.str = QString::number(n);
    }

    void drawVerticalHeader(double x, double y, int r, const QString &str) {
      const auto &cdata = tableData_.verticalHeaderColumnData;

      //---

      auto &objData = tablePlot_->getVHeaderObjData(r);

      objData.align = Qt::AlignRight | Qt::AlignVCenter;

      objData.rect = BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.rh);

      objData.str = str;
    }

    void drawCellValues(double x, double y, const VisitData &data) {
      for (int ic = 0; ic < tableData_.nc; ++ic) {
        const auto &c = tablePlot_->columns().getColumn(ic);

        const auto &cdata = tableData_.columnDataMap[c];

        drawCellValue(x, y, data, ic);

        x += cdata.drawWidth;
      }
    }

    void drawCellValue(double x, double y, const VisitData &data, int ic) {
      const auto &c = tablePlot_->columns().getColumn(ic);

      //---

      ModelIndex ind(tablePlot_, data.row, c, data.parent);

      bool ok;

      auto str = tablePlot_->modelString(const_cast<QAbstractItemModel *>(model_), ind,
                                         Qt::DisplayRole, ok);
      if (! ok) str.clear();

      //---

      auto &cellObjData = tablePlot_->getCellObjData(ind);

      cellObjData.str = str;

      //---

      const auto &cdata = tableData_.columnDataMap[c];

      if (cdata.numeric)
        cellObjData.align = Qt::AlignRight | Qt::AlignVCenter;
      else
        cellObjData.align = Qt::AlignLeft | Qt::AlignVCenter;

      //---

      auto bgVar = tablePlot_->modelValue(ind, Qt::BackgroundRole, ok);
      auto fgVar = tablePlot_->modelValue(ind, Qt::ForegroundRole, ok);

      auto bgColor = CQChartsVariant::toColor(bgVar, ok);
      auto fgColor = CQChartsVariant::toColor(fgVar, ok);

      if (bgColor.isValid()) cellObjData.bgColor = bgColor;
      if (fgColor.isValid()) cellObjData.fgColor = fgColor;

      //---

      double x1 = x;

      if (ic == 0)
        x1 += depth_*xd_;

      cellObjData.rect = BBox(x1 + xm_, y, x1 + cdata.drawWidth - xm_, y + tableData_.rh);
    }

   private:
    const CQChartsTablePlot* tablePlot_ { nullptr };
    TableData                tableData_;
    double                   xm_        { 0.0 };
    double                   xd_        { 0.0 };
    bool                     expanded_  { true };
    std::vector<int>         expandStack_;
  };

  RowVisitor visitor(this, tableData_);

  visitor.setPlot(this);

  //visitor.init();

  if (summaryModel_)
    CQChartsModelVisit::exec(charts(), summaryModel_, visitor);
  else {
    const auto &model = this->currentModel();

    CQChartsModelVisit::exec(charts(), model.data(), visitor);
  }
}

CQChartsTablePlot::HHeaderObjData &
CQChartsTablePlot::
getRowColumnHeaderObjData() const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  int ic = -2;

  auto pc = th->hheaderObjMap_.find(ic);

  if (pc == th->hheaderObjMap_.end())
    pc = th->hheaderObjMap_.emplace_hint(pc, ic, HHeaderObjData(ic));

  return (*pc).second;
}

CQChartsTablePlot::HHeaderObjData &
CQChartsTablePlot::
getVHeaderHeaderObjData() const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  int ic = -1;

  auto pc = th->hheaderObjMap_.find(ic);

  if (pc == th->hheaderObjMap_.end())
    pc = th->hheaderObjMap_.emplace_hint(pc, ic, HHeaderObjData(ic));

  return (*pc).second;
}

CQChartsTablePlot::HHeaderObjData &
CQChartsTablePlot::
getHHeaderHeaderObjData(const Column &c) const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  int ic = c.column();

  auto pc = th->hheaderObjMap_.find(ic);

  if (pc == th->hheaderObjMap_.end())
    pc = th->hheaderObjMap_.emplace_hint(pc, ic, HHeaderObjData(c));

  return (*pc).second;
}

CQChartsTablePlot::RowNumberObjData &
CQChartsTablePlot::
getRowNumberObjData(int r) const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  auto pc = th->rowNumberObjMap_.find(r);

  if (pc == th->rowNumberObjMap_.end())
    pc = th->rowNumberObjMap_.emplace_hint(pc, r, RowNumberObjData(r));

  return (*pc).second;
}

CQChartsTablePlot::VHeaderObjData &
CQChartsTablePlot::
getVHeaderObjData(int r) const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  auto pc = th->vheaderObjMap_.find(r);

  if (pc == th->vheaderObjMap_.end())
    pc = th->vheaderObjMap_.emplace_hint(pc, r, VHeaderObjData(r));

  return (*pc).second;
}

CQChartsTablePlot::CellObjData &
CQChartsTablePlot::
getCellObjData(const ModelIndex &ind) const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  auto modelInd = th->modelIndex(ind);

  auto pc = th->cellObjMap_.find(modelInd);

  if (pc == th->cellObjMap_.end())
    pc = th->cellObjMap_.emplace_hint(pc, modelInd, CellObjData(ind));

  return (*pc).second;
}

void
CQChartsTablePlot::
adjustPan()
{
  if (tableData_.dx < 0) {
    const double dx = -tableData_.dx;

    if (dataOffsetX() < 0)
      setDataOffsetX(0.0);

    if (dataOffsetX() > 2.0*dx)
      setDataOffsetX(2.0*dx);
  }
  else
    setDataOffsetX(0.0);

  if (tableData_.dy < 0) {
    const double dy = -tableData_.dy;

    if (dataOffsetY() < 0)
      setDataOffsetY(0.0);

    if (dataOffsetY() > 2.0*dy)
      setDataOffsetY(2.0*dy);
  }
  else
    setDataOffsetY(0.0);
}

QString
CQChartsTablePlot::
modeName(const Mode &mode) const
{
  switch (mode) {
    case Mode::NORMAL: return "Normal";
    case Mode::RANDOM: return "Random";
    case Mode::SORTED: return "Sorted";
    case Mode::PAGED : return "Paged";
    case Mode::ROWS  : return "Rows";
    default          : assert(false); return "";
  };
}

void
CQChartsTablePlot::
modelViewExpansionChanged()
{
  if (isFollowView())
    updateRangeAndObjs();
}

double
CQChartsTablePlot::
scrollX() const
{
#if 0
  if (scrollData_.hbar->isVisible())
    return pixelToSignedWindowWidth(-scrollData_.hpos);
  else
    return 0.0;
#else
  return tableData_.sx;
#endif
}

double
CQChartsTablePlot::
scrollY() const
{
#if 0
  if (scrollData_.vbar->isVisible())
    return pixelToSignedWindowWidth(-scrollData_.vpos);
  else
    return 0.0;
#else
  return tableData_.sy;
#endif
}

CQChartsGeom::BBox
CQChartsTablePlot::
calcTablePixelRect() const
{
  CQPerfTrace trace("CQChartsTablePlot::calcTablePixelRect");

  //---

  auto pixelRect = calcDataPixelRect();

  if (! isTabbed())
    return pixelRect;

  int px1 = int(pixelRect.getXMin());
  int px2 = int(pixelRect.getXMax());
  int py1 = int(pixelRect.getYMin());
  int py2 = int(pixelRect.getYMax());

  auto tabRect = this->calcTabPixelRect();

  double tph = tabRect.getHeight();

  return BBox(px1, py1, px2, py2 - tph);
}

CQChartsTableHHeaderObj *
CQChartsTablePlot::
createHHeaderObj(const HHeaderObjData &headerObjData) const
{
  return new CQChartsTableHHeaderObj(this, headerObjData);
}

CQChartsTableVHeaderObj *
CQChartsTablePlot::
createVHeaderObj(const VHeaderObjData &objData) const
{
  return new CQChartsTableVHeaderObj(this, objData);
}

CQChartsTableRowNumberObj *
CQChartsTablePlot::
createRowNumberObj(const RowNumberObjData &objData) const
{
  return new CQChartsTableRowNumberObj(this, objData);
}

CQChartsTableCellObj *
CQChartsTablePlot::
createCellObj(const CellObjData &cellObjData) const
{
  return new CQChartsTableCellObj(this, cellObjData);
}

//---

CQChartsPlotCustomControls *
CQChartsTablePlot::
createCustomControls()
{
  auto *controls = new CQChartsTablePlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsTableHHeaderObj::
CQChartsTableHHeaderObj(const TablePlot *tablePlot, const ObjData &objData) :
 CQChartsPlotObj(const_cast<TablePlot *>(tablePlot), objData.rect, ColorInd(), ColorInd(),
 ColorInd()), tablePlot_(tablePlot), objData_(objData)
{
}

QString
CQChartsTableHHeaderObj::
calcId() const
{
  QString str;

  if      (objData_.isColumnHeader())
    str = QString("%1:%2").arg(typeName()).arg(objData_.c.column());
  else if (objData_.ic == -1)
    str = QString("%1:row_number").arg(typeName());
  else if (objData_.ic == -2)
    str = QString("%1:vertical_header").arg(typeName());
  else
    assert(false);

  return str;
}

QString
CQChartsTableHHeaderObj::
calcTipId() const
{
  auto id = objData_.str;

  if (! id.length()) {
    if (objData_.isColumnHeader())
      id = QString::number(objData_.c.column());
  }

  return id;
}

bool
CQChartsTableHHeaderObj::
selectPress(const Point & /*p*/, SelData & /*selData*/)
{
  if (! objData_.isColumnHeader())
    return false;

  auto *tablePlot = const_cast<CQChartsTablePlot *>(tablePlot_);

  if (tablePlot->sortColumn() != CQChartsColumnNum(objData_.c.column()))
    tablePlot->setSortColumn(CQChartsColumnNum(objData_.c.column()));
  else {
    if (tablePlot_->sortOrder() == Qt::AscendingOrder)
      tablePlot->setSortOrder(Qt::DescendingOrder);
    else
      tablePlot->setSortOrder(Qt::AscendingOrder);
  }

  return true;
}

void
CQChartsTableHHeaderObj::
draw(PaintDevice *device) const
{
  device->save();

  auto pixelRect = tablePlot_->calcTablePixelRect();

  device->setClipRect(tablePlot_->pixelToWindow(pixelRect));

  //---

  auto *th = const_cast<CQChartsTableHHeaderObj *>(this);

  th->rect_ = objData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  //---

  int pSortWidth = tablePlot_->sortPixelWidth();

  auto trect = rect_;

  if (objData_.isColumnHeader() &&
      tablePlot_->sortColumn() == CQChartsColumnNum(objData_.c.column())) {
    auto sortWidth = tablePlot_->pixelToWindowWidth(pSortWidth);

    trect = BBox(rect_.getXMin()            , rect_.getYMin(),
                 rect_.getXMax() - sortWidth, rect_.getYMax());
  }

  //---

  // draw horizontal header text
  device->setFont(tablePlot_->tableHHeaderFont());

  auto bg = tablePlot_->interpColor(tablePlot_->hheaderColor(), ColorInd());
  auto tc = tablePlot_->calcTextColor(bg);

  PenBrush textPenBrush;

  tablePlot_->setPen(textPenBrush, PenData(true, tc));

  device->setPen(textPenBrush.pen);

  CQChartsTextOptions textOptions;

  textOptions.align     = objData_.align;
  textOptions.formatted = tablePlot_->isHHeaderFormatted();

  CQChartsDrawUtil::drawTextInBox(device, trect, objData_.str, textOptions);

  //---

  if (objData_.isColumnHeader() &&
      tablePlot_->sortColumn() == CQChartsColumnNum(objData_.c.column())) {
    PenBrush sortPenBrush;

    // 1 (top/bottom), 4 mid
    int pSortHeight = int((tablePlot_->windowToPixelHeight(trect.getHeight()) - 6.0)/2.0);

    int px1 = int(tablePlot_->windowToPixel(Point(trect.getXMax(), 0)).x + 2.0);
    int py1 = int(tablePlot_->windowToPixel(Point(0, trect.getYMax())).y + 1.0);
    int px2 = px1 + pSortWidth - 4;
    int py2 = int(tablePlot_->windowToPixel(Point(0, trect.getYMin())).y - 1.0);

    // ascending
    if (tablePlot_->sortOrder() == Qt::AscendingOrder)
      tablePlot_->setPenBrush(sortPenBrush, PenData(false), BrushData(true, tc));
    else
      tablePlot_->setPenBrush(sortPenBrush, PenData(true, tc), BrushData(false));

    auto p1 = tablePlot_->pixelToWindow(Point((px1 + px2)/2.0, py1));
    auto p2 = tablePlot_->pixelToWindow(Point(px1, py1 + pSortHeight));
    auto p3 = tablePlot_->pixelToWindow(Point(px2, py1 + pSortHeight));

    Polygon poly1; poly1.addPoint(p1); poly1.addPoint(p2); poly1.addPoint(p3);

    CQChartsDrawUtil::drawRoundedPolygon(device, sortPenBrush, poly1);

    // descending
    if (tablePlot_->sortOrder() == Qt::AscendingOrder)
      tablePlot_->setPenBrush(sortPenBrush, PenData(true, tc), BrushData(false));
    else
      tablePlot_->setPenBrush(sortPenBrush, PenData(false), BrushData(true, tc));

    auto p4 = tablePlot_->pixelToWindow(Point((px1 + px2)/2.0, py2));
    auto p5 = tablePlot_->pixelToWindow(Point(px1, py2 - pSortHeight));
    auto p6 = tablePlot_->pixelToWindow(Point(px2, py2 - pSortHeight));

    Polygon poly2; poly2.addPoint(p4); poly2.addPoint(p5); poly2.addPoint(p6);

    CQChartsDrawUtil::drawRoundedPolygon(device, sortPenBrush, poly2);
  }

  //---

  device->restore();
}

void
CQChartsTableHHeaderObj::
calcPenBrush(PenBrush &penBrush, bool /*updateState*/) const
{
  auto bg = tablePlot_->interpColor(tablePlot_->hheaderColor(), ColorInd());
  auto tc = tablePlot_->calcTextColor(bg);

  tablePlot_->setPen(penBrush, PenData(true, tc));
}

void
CQChartsTableHHeaderObj::
getObjSelectIndices(Indices &inds) const
{
  if (objData_.isColumnHeader()) {
    ModelIndex ind(tablePlot_, 0, objData_.c, QModelIndex());

    auto modelInd = tablePlot_->modelIndex(ind);

    inds.insert(modelInd);
  }
}

bool
CQChartsTableHHeaderObj::
rectIntersect(const BBox &r, bool inside) const
{
  auto rect = objData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}

//------

CQChartsTableRowNumberObj::
CQChartsTableRowNumberObj(const TablePlot *tablePlot, const ObjData &objData) :
 CQChartsPlotObj(const_cast<TablePlot *>(tablePlot), objData.rect, ColorInd(), ColorInd(),
 ColorInd()), tablePlot_(tablePlot), objData_(objData)
{
}

QString
CQChartsTableRowNumberObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(objData_.r);
}

QString
CQChartsTableRowNumberObj::
calcTipId() const
{
  auto id = objData_.str;

  if (! id.length())
    id = QString::number(objData_.r);

  return id;
}

void
CQChartsTableRowNumberObj::
draw(PaintDevice *device) const
{
  device->save();

  auto pixelRect = tablePlot_->calcTablePixelRect();

  device->setClipRect(tablePlot_->pixelToWindow(pixelRect));

  //---

  auto *th = const_cast<CQChartsTableRowNumberObj *>(this);

  th->rect_ = objData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  //---

  // draw row text
  device->setFont(tablePlot_->tableRowColumnFont());

  auto bg = tablePlot_->interpColor(tablePlot_->rowColumnColor(), ColorInd());
  auto tc = tablePlot_->calcTextColor(bg);

  PenBrush textPenBrush;

  tablePlot_->setPen(textPenBrush, PenData(true, tc));

  device->setPen(textPenBrush.pen);

  CQChartsTextOptions textOptions;

  textOptions.align = objData_.align;

  CQChartsDrawUtil::drawTextInBox(device, rect_, objData_.str, textOptions);

  //---

  device->restore();
}

void
CQChartsTableRowNumberObj::
calcPenBrush(PenBrush &penBrush, bool /*updateState*/) const
{
  auto bg = tablePlot_->interpColor(tablePlot_->cellColor(), ColorInd());
  auto tc = tablePlot_->calcTextColor(bg);

  tablePlot_->setPen(penBrush, PenData(true, tc));
}

bool
CQChartsTableRowNumberObj::
rectIntersect(const BBox &r, bool inside) const
{
  auto rect = objData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}

//------

CQChartsTableVHeaderObj::
CQChartsTableVHeaderObj(const TablePlot *tablePlot, const ObjData &objData) :
 CQChartsPlotObj(const_cast<TablePlot *>(tablePlot), objData.rect, ColorInd(), ColorInd(),
 ColorInd()), tablePlot_(tablePlot), objData_(objData)
{
}

QString
CQChartsTableVHeaderObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(objData_.r);
}

QString
CQChartsTableVHeaderObj::
calcTipId() const
{
  auto id = objData_.str;

  if (! id.length())
    id = QString::number(objData_.r);

  return id;
}

void
CQChartsTableVHeaderObj::
draw(PaintDevice *device) const
{
  device->save();

  auto pixelRect = tablePlot_->calcTablePixelRect();

  device->setClipRect(tablePlot_->pixelToWindow(pixelRect));

  //---

  auto *th = const_cast<CQChartsTableVHeaderObj *>(this);

  th->rect_ = objData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  //---

  // draw vertical header text
  device->setFont(tablePlot_->tableVHeaderFont());

  auto bg = tablePlot_->interpColor(tablePlot_->vheaderColor(), ColorInd());
  auto tc = tablePlot_->calcTextColor(bg);

  PenBrush textPenBrush;

  tablePlot_->setPen(textPenBrush, PenData(true, tc));

  device->setPen(textPenBrush.pen);

  CQChartsTextOptions textOptions;

  textOptions.align     = objData_.align;
  textOptions.formatted = tablePlot_->isVHeaderFormatted();

  CQChartsDrawUtil::drawTextInBox(device, rect_, objData_.str, textOptions);

  //---

  device->restore();
}

void
CQChartsTableVHeaderObj::
calcPenBrush(PenBrush &penBrush, bool /*updateState*/) const
{
  auto bg = tablePlot_->interpColor(tablePlot_->vheaderColor(), ColorInd());
  auto tc = tablePlot_->calcTextColor(bg);

  tablePlot_->setPen(penBrush, PenData(true, tc));
}

bool
CQChartsTableVHeaderObj::
rectIntersect(const BBox &r, bool inside) const
{
  auto rect = objData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}

//------

CQChartsTableCellObj::
CQChartsTableCellObj(const TablePlot *tablePlot, const TablePlot::CellObjData &cellObjData) :
 CQChartsPlotObj(const_cast<TablePlot *>(tablePlot), cellObjData.rect, ColorInd(), ColorInd(),
 ColorInd()), tablePlot_(tablePlot), cellObjData_(cellObjData)
{
}

QString
CQChartsTableCellObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(cellObjData_.ind.id());
}

QString
CQChartsTableCellObj::
calcTipId() const
{
  auto id = cellObjData_.str;

  if (! id.length())
    id = QString::number(cellObjData_.ind.column().column());

  return id;
}

void
CQChartsTableCellObj::
draw(PaintDevice *device) const
{
  device->save();

  auto pixelRect = tablePlot_->calcTablePixelRect();

  device->setClipRect(tablePlot_->pixelToWindow(pixelRect));

  //---

  auto *th = const_cast<CQChartsTableCellObj *>(this);

  th->rect_ = cellObjData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  //---

  QColor bg;
  bool   bgSet { false };
  bool   useDrawColor { true };
  bool   drawBar { false };
  BBox   lrect, rrect;
  QColor bg1, bg2;

  auto *columnDetails = tablePlot_->columnDetails(cellObjData_.ind.column());

  auto tableDrawType = (columnDetails ? columnDetails->tableDrawType() :
                         CQChartsModelColumnDetails::TableDrawType::NORMAL);

  // calc background pen and brush and draw
  if      (isInside()) {
    PenBrush bgPenBrush;

    bg    = tablePlot_->interpColor(tablePlot_->insideColor(), ColorInd());
    bgSet = true;
  }
  else if (isSelected()) {
    PenBrush bgPenBrush;

    bg    = tablePlot_->interpColor(tablePlot_->selectedColor(), ColorInd());
    bgSet = true;
  }
  else {
    bool   hasRange = false;
    double value { 0.0 }, min { 0.0 }, max { 0.0 }, norm { 0.0 };

    if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP ||
        tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART) {
      // get column range (numeric)
      auto type = (columnDetails ? columnDetails->type() : CQBaseModelType::STRING);

      if (type == CQBaseModelType::REAL || type == CQBaseModelType::INTEGER) {
        // get cell value
        bool ok;
        value = CQChartsUtil::toReal(cellObjData_.str, ok);

        // get column min/max
        auto *columnTypeMgr = charts()->columnTypeMgr();

        const auto *columnType = columnTypeMgr->getType(type);

        auto minVar = columnType->minValue(columnDetails->nameValues());
        if (! minVar.isValid()) minVar = columnDetails->minValue();

        auto maxVar = columnType->maxValue(columnDetails->nameValues());
        if (! maxVar.isValid()) maxVar = columnDetails->maxValue();

        min = CQChartsVariant::toReal(minVar, ok);
        max = CQChartsVariant::toReal(maxVar, ok);

        // normalize value (
        norm = CMathUtil::clamp(CMathUtil::map(value, min, max, 0.0, 1.0), 0.0, 1.0);

        hasRange = true;
      }
    }

    if (hasRange) {
      if      (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP) {
        // calc background color
        bg = columnDetails->heatmapColor(value, min, max,
               tablePlot_->interpColor(tablePlot_->cellColor(), ColorInd()));
        bgSet = true;
      }
      else if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART) {
        // get bar margins
        auto xm = tablePlot_->pixelToWindowWidth (tablePlot_->barXMargin());
        auto ym = tablePlot_->pixelToWindowHeight(tablePlot_->barYMargin());

        double barWidth = rect_.getWidth() - 2.0*xm;

        // draw bar for normalized value
        double lw =        norm *barWidth; // bar width
        double rw = (1.0 - norm)*barWidth; // non-bar width

        bg1 = columnDetails->barchartColor();
        bg2 = tablePlot_->interpColor(tablePlot_->cellColor(), ColorInd());

        // calc left and right bar bboxes
        double x1 = rect_.getXMin() + xm;
        double x2 = x1 + lw;
        double x3 = x2 + rw;
        double y1 = rect_.getYMin() + ym;
        double y2 = rect_.getYMax() - ym;

        lrect = BBox(x1, y1, x2, y2);
        rrect = BBox(x2, y1, x3, y2);

        //---

        PenBrush bgPenBrush;

        // draw lrect
        if (lrect.isValid()) {
          tablePlot_->setPenBrush(bgPenBrush, PenData(false), BrushData(true, bg1));
          CQChartsDrawUtil::setPenBrush(device, bgPenBrush);

          device->drawRect(lrect);
        }

        // draw rrect
        if (rrect.isValid()) {
          tablePlot_->setPenBrush(bgPenBrush, PenData(false), BrushData(true, bg2));
          CQChartsDrawUtil::setPenBrush(device, bgPenBrush);

          device->drawRect(rrect);
        }

        //---

        useDrawColor = false;
        drawBar      = true;
      }
    }
  }

  // set background color (if not already set and cell has background color)
  if (! bgSet && cellObjData_.bgColor.isValid()) {
    bg    = tablePlot_->interpColor(cellObjData_.bgColor, ColorInd());
    bgSet = true;
  }

  // draw backgroup
  if (bgSet) {
    PenBrush bgPenBrush;

    tablePlot_->setPenBrush(bgPenBrush, PenData(false), BrushData(true, bg));

    CQChartsDrawUtil::setPenBrush(device, bgPenBrush);

    device->drawRect(rect_);
  }

  //---

  // set foreground (text) color
  QColor fg;
  bool   fgSet = false;

  if (cellObjData_.fgColor.isValid()) {
    fg    = tablePlot_->interpColor(cellObjData_.fgColor, ColorInd());
    fgSet = true;
  }

  if (! fgSet) {
    CQChartsColor textColor;

    if (! bgSet) {
      textColor = tablePlot_->calcTextColor(tablePlot_->cellColor());

      if (useDrawColor && columnDetails) {
        const auto &drawColor = columnDetails->tableDrawColor();

        if (drawColor.isValid())
          textColor = drawColor;
      }

      if      (isInside())
        textColor = tablePlot_->calcTextColor(tablePlot_->insideColor());
      else if (isSelected())
        textColor = tablePlot_->calcTextColor(tablePlot_->selectedColor());
    }
    else {
      textColor = CQChartsColor(tablePlot_->calcTextColor(bg));
    }

    fg = tablePlot_->interpColor(textColor, ColorInd());
  }

  //---

  // draw cell text
  CQChartsTextOptions textOptions;

  textOptions.align = cellObjData_.align;

  auto optAlign = columnDetails->alignment();

  if (optAlign)
    textOptions.align = *optAlign;

  device->setFont(tablePlot_->tableFont());

  PenBrush textPenBrush;

  auto setPenColor = [&](const QColor &c) {
    tablePlot_->setPen(textPenBrush, PenData(true, c));
    device->setPen(textPenBrush.pen);
  };

  if (drawBar) {
    auto tc1 = CQChartsUtil::bwColor(bg1);
    auto tc2 = CQChartsUtil::bwColor(bg2);

    if (tc1 != tc2) {
      textOptions.clipped = false;

      if (lrect.isValid()) {
        setPenColor(tc1);

        device->save();
        device->setClipRect(lrect);
        CQChartsDrawUtil::drawTextInBox(device, rect_, cellObjData_.str, textOptions);
        device->restore();
      }

      if (rrect.isValid()) {
        setPenColor(tc2);

        device->save();
        device->setClipRect(rrect);
        CQChartsDrawUtil::drawTextInBox(device, rect_, cellObjData_.str, textOptions);
        device->restore();
      }
    }
    else {
      setPenColor(tc1);

      CQChartsDrawUtil::drawTextInBox(device, rect_, cellObjData_.str, textOptions);
    }
  }
  else {
    tablePlot_->setPen(textPenBrush, PenData(true, fg));
    device->setPen(textPenBrush.pen);

    CQChartsDrawUtil::drawTextInBox(device, rect_, cellObjData_.str, textOptions);
  }

  //---

  device->restore();
}

void
CQChartsTableCellObj::
calcPenBrush(PenBrush &, bool /*updateState*/) const
{
}

void
CQChartsTableCellObj::
getObjSelectIndices(Indices &inds) const
{
  auto modelInd = tablePlot_->modelIndex(cellObjData_.ind);

  inds.insert(modelInd);
}

bool
CQChartsTableCellObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  auto rect = cellObjData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  return rect.inside(p);
}

bool
CQChartsTableCellObj::
rectIntersect(const BBox &r, bool inside) const
{
  auto rect = cellObjData_.rect.translated(tablePlot_->scrollX(), -tablePlot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}

//------

CQChartsTablePlotCustomControls::
CQChartsTablePlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "table")
{
}

void
CQChartsTablePlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsTablePlotCustomControls::
addWidgets()
{
  addColumnWidgets();
}

void
CQChartsTablePlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  addNamedColumnWidgets(QStringList() << "columns", columnsFrame);
}

void
CQChartsTablePlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsTablePlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && tablePlot_)
    disconnect(tablePlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  tablePlot_ = dynamic_cast<CQChartsTablePlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);

  if (tablePlot_)
    connect(tablePlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsTablePlotCustomControls::
updateWidgets()
{
  CQChartsPlotCustomControls::updateWidgets();
}
