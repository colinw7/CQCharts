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
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetUtil.h>

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

  addColumnsParameter("columns", "Columns", "columns").setRequired().setTip("Columns");

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
  term();
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

  setPlotFillColor(Color(Color::Type::INTERFACE_VALUE, 0.00));
  setDataFillColor(Color(Color::Type::INTERFACE_VALUE, 0.00));
  setFitFillColor (Color(Color::Type::INTERFACE_VALUE, 0.00));

  //---

  font_       = Font().decFontSize(8);
  headerFont_ = font_;

  //---

  if (! CQChartsModelUtil::isHierarchical(model().data())) {
    summaryModel_ = new CQSummaryModel(model().data());

    summaryModel_->setMode((CQSummaryModel::Mode) mode_);
  }

  int pageSize = 1024;

  setMaxRows (10*pageSize);
  setPageSize(pageSize);

  setGridColor(Color(Color::Type::INTERFACE_VALUE, 0.50));

  setHeaderColor(QColor("#c0c8dc"));

  setCellColor    (Color(Color::Type::INTERFACE_VALUE, 0.00));
  setInsideColor  (QColor("#c5c8bc"));
  setSelectedColor(QColor("#bcc5c8"));

  //---

  setOuterMargin(PlotMargin(Length("0P"), Length("0P"), Length("0P"), Length("0P")));

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
setModel(const ModelP &model)
{
  CQChartsPlot::disconnectModel();

  model_ = model;

  delete summaryModel_;

  if (! CQChartsModelUtil::isHierarchical(model.data())) {
    summaryModel_ = new CQSummaryModel(model.data());

    summaryModel_->setMode((CQSummaryModel::Mode) mode_);
  }

  CQChartsPlot::connectModel();

  updateRangeAndObjs();

  emit modelChanged();
}

void
CQChartsTablePlot::
modelTypeChangedSlot(int modelInd)
{
  auto *modelData = charts()->getModelData(model_);

  if (modelData && modelData->ind() == modelInd)
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
  CQChartsUtil::testAndSet(columns_, c, [&]() { updateRangeAndObjs(); } );
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
  return (summaryModel_ ? (CQChartsTablePlot::Mode) summaryModel_->mode() : mode_);
}

void
CQChartsTablePlot::
setMode(const Mode &m)
{
  if (m != mode()) {
    mode_ = m;

    if (summaryModel_)
      summaryModel_->setMode((CQSummaryModel::Mode) mode_);

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
    strs << QString("%1").arg(r);

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

    const int i = s.toInt(&ok);

    if (ok)
      rowNums.push_back(i);
  }

  setRowNums(rowNums);
}

//---

void
CQChartsTablePlot::
setRowColumn(bool b)
{
  CQChartsUtil::testAndSet(rowColumn_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
setHeaderVisible(bool b)
{
  CQChartsUtil::testAndSet(headerData_.visible, b, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setHeaderColor(const Color &c)
{
  CQChartsUtil::testAndSet(headerData_.color, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setHeaderFont(const Font &f)
{
  CQChartsUtil::testAndSet(headerFont_, f, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
setGridColor(const Color &c)
{
  CQChartsUtil::testAndSet(gridColor_, c, [&]() { updateObjs(); } );
}

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
  CQChartsUtil::testAndSet(cellMargin_, i, [&]() { updateObjs(); } );
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc, bool hidden=false) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addBaseProperties();

  // columns
  addProp("columns", "columns", "columns", "Columns");

  // mode
  addProp("options", "mode"   , "mode"   , "Set mode"    );
  addProp("options", "maxRows", "maxRows", "Set max rows")->setMinValue(1);

  // sort mode
  addProp("options", "sortColumn" , "sortColumn" , "Set sort column");
  addProp("options", "sortRole"   , "sortRole"   , "Set sort role"  , true);
  addProp("options", "sortOrder"  , "sortOrder"  , "Set sort order" );

  // page mode
  addProp("options", "pageSize"   , "pageSize"   , "Set page size"   )->setMinValue(1);
  addProp("options", "currentPage", "currentPage", "Set current page")->setMinValue(0);

  // rows mode
  addProp("options", "rowNums"  , "rowNums"  , "Explicit row numbers for ROWS mode");

  // header
  addProp("header", "headerVisible", "visible"   , "Header visible");
  addProp("header", "headerColor"  , "color"     , "Header color"  );
  addProp("header", "headerFont"   , "headerFont", "Header font"  );

  // grid
  addProp("options", "gridColor"    , "gridColor"    , "Grid color");

  // cells
  addProp("options", "cellColor"    , "cellColor"    , "Cell color");
  addProp("options", "insideColor"  , "insideColor"  , "Cell inside color");
  addProp("options", "selectedColor", "selectedColor", "Cell selected color");

  addProp("options", "rowColumn" , "rowColumn" , "Display row number column");
  addProp("options", "indent"    , "indent"    , "Hierarchical row indent")->setMinValue(0.0);
  addProp("options", "cellMargin", "cellMargin", "Cell margin")->setMinValue(0);
  addProp("options", "followView", "followView", "Follow view");
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

  //---

  auto *th = const_cast<CQChartsTablePlot *>(this);

  //---

  QModelIndexList expandInds;

  if (isFollowView())
    view()->expandedModelIndices(expandInds);

  //---

  th->tableData_.expandInds = expandInds;

  th->tableData_.headerFont = view()->viewFont(this->headerFont());
  th->tableData_.headerFont = CQChartsUtil::scaleFontSize(th->tableData_.headerFont, fontScale_);

  th->tableData_.font = view()->viewFont(this->font());
  th->tableData_.font = CQChartsUtil::scaleFontSize(th->tableData_.font, fontScale_);

  //th->tabbedFont_ = th->tableData_.font;

  QFontMetricsF hfm(th->tableData_.headerFont);
  QFontMetricsF fm (th->tableData_.font);

  th->tableData_.nc = columns_.count();

  if (summaryModel_) {
    th->tableData_.nr       = summaryModel_->rowCount();
    th->tableData_.maxDepth = 0;
  }
  else {
    CQChartsModelUtil::hierData(charts(), model().data(), th->tableData_.nr,
                                th->tableData_.maxDepth);
  }

  th->tableData_.pmargin    = cellMargin();
  th->tableData_.pSortWidth = hfm.width("X") + 4;

  th->tableData_.phrh = hfm.height() + 2.0*tableData_.pmargin;
  th->tableData_.prh  = fm.height() + 2.0*tableData_.pmargin;

  // calc column widths
  if (isRowColumn()) {
    auto &data = th->tableData_.rowColumnData;

    const int power = CMathRound::RoundUp(log10(tableData_.nr));

    data.pwidth  = power*fm.width("X") + 2.0*tableData_.pmargin;
    data.numeric = false;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    auto *columnDetails = this->columnDetails(c);
    if (! columnDetails) continue;

    auto &data = th->tableData_.columnDataMap[c];

    bool ok;

    QString str;

    if (summaryModel())
      str = CQChartsModelUtil::modelHHeaderString(summaryModel(), c, ok);
    else
      str = CQChartsModelUtil::modelHHeaderString(model().data(), c, ok);

    if (! ok) continue;

    double cw = hfm.width(str) + 2.0*tableData_.pmargin + tableData_.pSortWidth;

    if (i == 0)
      cw += tableData_.maxDepth*indent(); // add hierarchical indent

    data.pwidth  = cw;
    data.numeric = columnDetails->isNumeric();

    data.prefWidth = columnDetails->preferredWidth();
  }

  //---

  // update column widths and number of visible rows
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsTablePlot *plot, TableData &tableData) :
     plot_(plot), tableData_(tableData), fm_(tableData_.font) {
    }

    // process hier row
    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      if (! expanded_) return State::SKIP;

      //---

      expandStack_.push_back(expanded_);

      if (plot_->isFollowView()) {
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

      auto *plot = const_cast<CQChartsTablePlot *>(plot_);

      for (int i = 0; i < tableData_.nc; ++i) {
        const auto &c = plot_->columns().getColumn(i);

        ModelIndex ind(plot, data.row, c, data.parent);

        bool ok;

        auto str = plot_->modelString(const_cast<QAbstractItemModel *>(model_), ind,
                                      Qt::DisplayRole, ok);
        if (! ok) continue;

        auto &data = tableData_.columnDataMap[c];

        double cw = fm_.width(str) + 2.0*tableData_.pmargin;

        if (i == 0)
          cw += tableData_.maxDepth*plot_->indent(); // add hierarchical indent

        data.pwidth = std::max(data.pwidth, cw);
      }

      return State::OK;
    }

   private:
    const CQChartsTablePlot* plot_   { nullptr };
    TableData&               tableData_;
    QFontMetricsF            fm_;
    bool                     expanded_ { true };
    std::vector<int>         expandStack_;
  };

  RowVisitor visitor(this, th->tableData_);

  visitor.setPlot(this);

  //visitor.init();

  if (summaryModel_)
    CQChartsModelVisit::exec(charts(), summaryModel_, visitor);
  else
    CQChartsModelVisit::exec(charts(), model().data(), visitor);

  th->tableData_.nvr = visitor.numProcessedRows();

  //---

  // set full table width
  th->tableData_.pcw = 0.0;

  if (isRowColumn()) {
    const auto &data = th->tableData_.rowColumnData;

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

bool
CQChartsTablePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsTablePlot::createObjs");

  //---

  updatePosition();

  initDrawData();

  for (const auto &pc : headerObjMap_) {
    const auto &headerObjData = pc.second;

    auto *obj = createHeaderObj(headerObjData);

    objs.push_back(obj);
  }

  for (const auto &pr : rowObjMap_) {
    const auto &rowObjData = pr.second;

    auto *obj = createRowObj(rowObjData);

    objs.push_back(obj);
  }

  for (const auto &pc : cellObjMap_) {
    const auto &cellObjData = pc.second;

    auto *obj = createCellObj(cellObjData);

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

//auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) {
//  return addMenuCheckedAction(menu, name, isSet, slot);
//};

  //---

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

  //hscrollSlot(scrollData_.hpos + (delta > 0 ? fm.width("X") : -fm.width("X")));
  scrollData_.hbar->setValue(scrollData_.hpos + (delta > 0 ? fm.width("X") : -fm.width("X")));
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

  th->headerObjMap_.clear();
  th->rowObjMap_   .clear();
  th->cellObjMap_  .clear();

  //---

  th->tableData_.hrh = pixelToWindowHeight(th->tableData_.phrh);
  th->tableData_.rh  = pixelToWindowHeight(th->tableData_.prh);

  //---

  // calc column widths
  double x = th->tableData_.xo;

  if (isRowColumn()) {
    auto &data = th->tableData_.rowColumnData;

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    x += data.drawWidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    auto &data = th->tableData_.columnDataMap[c];

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    if (data.prefWidth > 0)
      data.drawWidth = pixelToWindowWidth(data.prefWidth + 2.0*tableData_.pmargin);

    th->tableData_.columnDataMap[c] = data;

    x += data.drawWidth;
  }

  //---

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

  if (isHeaderVisible())
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
    scrollData_.hbar->move  (pixelRect.getXMin(), pixelRect.getYMax() - scrollData_.pixelBarSize);
    scrollData_.hbar->resize(pixelRect.getWidth(), scrollData_.pixelBarSize);

    scrollData_.hbar->setRange(0, -pdx);
    scrollData_.hbar->setPageStep(pixelRect.getWidth() - 2.0*tableData_.pmargin);
  }
  else
    th->scrollData_.hpos = 0;

  if (scrollData_.vbar->isVisible()) {
    scrollData_.vbar->move(pixelRect.getXMax() - scrollData_.pixelBarSize, pixelRect.getYMin());
    scrollData_.vbar->resize(scrollData_.pixelBarSize, pixelRect.getHeight());

    scrollData_.vbar->setRange(0, -pdy);
    scrollData_.vbar->setPageStep(pixelRect.getHeight() - 2.0*tableData_.pmargin);
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

  if (isHeaderVisible())
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

  if (isRowColumn()) {
    auto &data = th->tableData_.rowColumnData;

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    x += data.drawWidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    auto &data = th->tableData_.columnDataMap[c];

    data.width     = pixelToWindowWidth(data.pwidth);
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
  if (isHeaderVisible())
    y1 += th->tableData_.hrh;

  // draw header background
  if (isHeaderVisible()) {
    PenBrush headerPenBrush;

    setPenBrush(headerPenBrush,
      PenData(false), BrushData(true, interpColor(headerColor(), ColorInd())));

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

  //---

  // draw table column lines
  PenBrush gridPenBrush;

  setPenBrush(gridPenBrush,
    PenData(true, interpColor(gridColor(), ColorInd())), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, gridPenBrush);

  x = x1;

  // number column vertical line
  if (isRowColumn()) {
    const auto &data = th->tableData_.rowColumnData;

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

  if (isHeaderVisible()) {
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
    RowVisitor(const CQChartsTablePlot *plot, const TableData &tableData_) :
     plot_(plot), tableData_(tableData_) {
      xm_ = plot_->pixelToWindowWidth(tableData_.pmargin);
      xd_ = plot_->pixelToWindowWidth(plot_->indent());
    }

    // draw hier row
    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      if (! expanded_) return State::SKIP;

      //---

      // draw header
      if (plot_->isHeaderVisible())
        if (data.vrow == 0) {
          drawHeader();
      }

      //---

    //const double y = tableData_.yo + (tableData_.nvr - data.vrow)*tableData_.rh - tableData_.hrh;
      const double y = tableData_.yo + (tableData_.nvr - data.vrow - 1)*tableData_.rh;

      double x = tableData_.xo;

      //---

      // draw line number
      if (plot_->isRowColumn()) {
        const auto &cdata = tableData_.rowColumnData;

        drawRowNumber(x, y, data.vrow + 1);

        x += cdata.drawWidth;
      }

      //---

      // draw cell values
      drawCellValues(x, y, data);

      //---

      expandStack_.push_back(expanded_);

      if (plot_->isFollowView()) {
        auto ind = model_->index(data.row, 0, data.parent);

        expanded_ = tableData_.expandInds.contains(ind);
      }
      else
        expanded_ = true;

      return State::OK;
    }

    // post hier row
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
      if (plot_->isHeaderVisible())
        if (data.vrow == 0) {
          drawHeader();
      }

      //---

    //const double y = tableData_.yo + (tableData_.nvr - data.vrow)*tableData_.rh - tableData_.hrh;
      const double y = tableData_.yo + (tableData_.nvr - data.vrow - 1)*tableData_.rh;

      double x = tableData_.xo;

      //---

      // draw line number
      if (plot_->isRowColumn()) {
        const auto &cdata = tableData_.rowColumnData;

        drawRowNumber(x, y, data.vrow + 1);

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

      // draw empty line number area
      if (plot_->isRowColumn()) {
        Column c;

        auto &headerObjData = plot_->getHeaderObjData(c);

        headerObjData.str = " ";

        //---

        const auto &cdata = tableData_.rowColumnData;

        headerObjData.rect = BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.hrh);

        //---

        x += cdata.drawWidth;
      }

      // draw column headers
      for (int ic = 0; ic < tableData_.nc; ++ic) {
        const auto &c = plot_->columns().getColumn(ic);

        bool ok;

        auto str = CQChartsModelUtil::modelHHeaderString(model_, c, ok);
        if (! ok) continue;

        //---

        auto &headerObjData = plot_->getHeaderObjData(c);

        headerObjData.str = str;

        //---

        const auto &cdata = tableData_.columnDataMap[c];

        if (cdata.numeric)
          headerObjData.align = Qt::AlignRight | Qt::AlignVCenter;
        else
          headerObjData.align = Qt::AlignLeft | Qt::AlignVCenter;

        headerObjData.rect = BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.hrh);

        //---

        x += cdata.drawWidth;
      }
    }

    void drawRowNumber(double x, double y, int n) {
      const auto &cdata = tableData_.rowColumnData;

      //---

      auto &rowObjData = plot_->getRowObjData(n);

      rowObjData.align = Qt::AlignRight | Qt::AlignVCenter;

      rowObjData.rect = BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.rh);

      rowObjData.str = QString("%1").arg(n);
    }

    void drawCellValues(double x, double y, const VisitData &data) {
      for (int ic = 0; ic < tableData_.nc; ++ic) {
        const auto &c = plot_->columns().getColumn(ic);

        const auto &cdata = tableData_.columnDataMap[c];

        drawCellValue(x, y, data, ic);

        x += cdata.drawWidth;
      }
    }

    void drawCellValue(double x, double y, const VisitData &data, int ic) {
      const auto &c = plot_->columns().getColumn(ic);

      //---

      auto *plot = const_cast<CQChartsTablePlot *>(plot_);

      ModelIndex ind(plot, data.row, c, data.parent);

      bool ok;

      auto str = plot_->modelString(const_cast<QAbstractItemModel *>(model_), ind,
                                    Qt::DisplayRole, ok);
      if (! ok) str = "";

      //---

      auto &cellObjData = plot_->getCellObjData(ind);

      cellObjData.str = str;

      //---

      const auto &cdata = tableData_.columnDataMap[c];

      if (cdata.numeric)
        cellObjData.align = Qt::AlignRight | Qt::AlignVCenter;
      else
        cellObjData.align = Qt::AlignLeft | Qt::AlignVCenter;

      //---

      double x1 = x;

      if (ic == 0)
        x1 += depth_*xd_;

      cellObjData.rect = BBox(x1 + xm_, y, x1 + cdata.drawWidth - xm_, y + tableData_.rh);
    }

   private:
    const CQChartsTablePlot* plot_     { nullptr };
    TableData                tableData_;
    double                   xm_       { 0.0 };
    double                   xd_       { 0.0 };
    bool                     expanded_ { true };
    std::vector<int>         expandStack_;
  };

  RowVisitor visitor(this, tableData_);

  visitor.setPlot(this);

  //visitor.init();

  if (summaryModel_)
    CQChartsModelVisit::exec(charts(), summaryModel_, visitor);
  else
    CQChartsModelVisit::exec(charts(), model().data(), visitor);
}

CQChartsTablePlot::HeaderObjData &
CQChartsTablePlot::
getHeaderObjData(const Column &c) const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  int ic = c.column();

  auto pc = th->headerObjMap_.find(ic);

  if (pc == th->headerObjMap_.end())
    pc = th->headerObjMap_.insert(pc, HeaderObjMap::value_type(ic, HeaderObjData(c)));

  return (*pc).second;
}

CQChartsTablePlot::RowObjData &
CQChartsTablePlot::
getRowObjData(int r) const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  auto pc = th->rowObjMap_.find(r);

  if (pc == th->rowObjMap_.end())
    pc = th->rowObjMap_.insert(pc, RowObjMap::value_type(r, RowObjData(r)));

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
    pc = th->cellObjMap_.insert(pc, CellObjMap::value_type(modelInd, CellObjData(ind)));

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

  int px1 = pixelRect.getXMin();
  int px2 = pixelRect.getXMax();
  int py1 = pixelRect.getYMin();
  int py2 = pixelRect.getYMax();

  auto tabRect = this->calcTabPixelRect();

  double tph = tabRect.getHeight();

  return BBox(px1, py1, px2, py2 - tph);
}

CQChartsTableHeaderObj *
CQChartsTablePlot::
createHeaderObj(const HeaderObjData &headerObjData) const
{
  return new CQChartsTableHeaderObj(this, headerObjData);
}

CQChartsTableRowObj *
CQChartsTablePlot::
createRowObj(const RowObjData &rowObjData) const
{
  return new CQChartsTableRowObj(this, rowObjData);
}

CQChartsTableCellObj *
CQChartsTablePlot::
createCellObj(const CellObjData &cellObjData) const
{
  return new CQChartsTableCellObj(this, cellObjData);
}

//------

CQChartsTableHeaderObj::
CQChartsTableHeaderObj(const Plot *plot, const Plot::HeaderObjData &headerObjData) :
 CQChartsPlotObj(const_cast<Plot *>(plot), headerObjData.rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), headerObjData_(headerObjData)
{
}

QString
CQChartsTableHeaderObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(headerObjData_.c.column());
}

QString
CQChartsTableHeaderObj::
calcTipId() const
{
  auto id = headerObjData_.str;

  if (! id.length())
    id = QString("%1").arg(headerObjData_.c.column());

  return id;
}

bool
CQChartsTableHeaderObj::
selectPress(const Point & /*p*/, SelMod /*selMod*/)
{
  auto *plot = const_cast<CQChartsTablePlot *>(plot_);

  if (plot->sortColumn() != CQChartsColumnNum(headerObjData_.c.column()))
    plot->setSortColumn(CQChartsColumnNum(headerObjData_.c.column()));
  else {
    if (plot_->sortOrder() == Qt::AscendingOrder)
      plot->setSortOrder(Qt::DescendingOrder);
    else
      plot->setSortOrder(Qt::AscendingOrder);
  }

  return true;
}

void
CQChartsTableHeaderObj::
draw(PaintDevice *device) const
{
  device->save();

  auto pixelRect = plot_->calcTablePixelRect();

  device->setClipRect(plot_->pixelToWindow(pixelRect));

  //---

  auto *th = const_cast<CQChartsTableHeaderObj *>(this);

  th->rect_ = headerObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  //---

  int pSortWidth = plot_->sortPixelWidth();

  auto trect = rect_;

  if (plot_->sortColumn() == CQChartsColumnNum(headerObjData_.c.column())) {
    auto sortWidth = plot_->pixelToWindowWidth(pSortWidth);

    trect = BBox(rect_.getXMin()            , rect_.getYMin(),
                 rect_.getXMax() - sortWidth, rect_.getYMax());
  }

  //---

  // draw header text
  device->setFont(plot_->tableHeaderFont());

  auto bg = plot_->interpColor(plot_->headerColor(), ColorInd());
  auto tc = plot_->calcTextColor(bg);

  PenBrush textPenBrush;

  plot_->setPen(textPenBrush, PenData(true, tc));

  device->setPen(textPenBrush.pen);

  CQChartsTextOptions textOptions;

  textOptions.align = headerObjData_.align;

  CQChartsDrawUtil::drawTextInBox(device, trect, headerObjData_.str, textOptions);

  //---

  if (plot_->sortColumn() == CQChartsColumnNum(headerObjData_.c.column())) {
    PenBrush sortPenBrush;

    // 1 (top/bottom), 4 mid
    int pSortHeight = (plot_->windowToPixelHeight(trect.getHeight()) - 6)/2;

    int px1 = plot_->windowToPixel(Point(trect.getXMax(), 0)).x + 2;
    int py1 = plot_->windowToPixel(Point(0, trect.getYMax())).y + 1;
    int px2 = px1 + pSortWidth - 4;
    int py2 = plot_->windowToPixel(Point(0, trect.getYMin())).y - 1;

    // ascending
    if (plot_->sortOrder() == Qt::AscendingOrder)
      plot_->setPenBrush(sortPenBrush, PenData(false), BrushData(true, tc));
    else
      plot_->setPenBrush(sortPenBrush, PenData(true, tc), BrushData(false));

    Point p1 = plot_->pixelToWindow(Point((px1 + px2)/2.0, py1));
    Point p2 = plot_->pixelToWindow(Point(px1, py1 + pSortHeight));
    Point p3 = plot_->pixelToWindow(Point(px2, py1 + pSortHeight));

    Polygon poly1; poly1.addPoint(p1); poly1.addPoint(p2); poly1.addPoint(p3);

    CQChartsDrawUtil::drawRoundedPolygon(device, sortPenBrush, poly1);

    // descending
    if (plot_->sortOrder() == Qt::AscendingOrder)
      plot_->setPenBrush(sortPenBrush, PenData(true, tc), BrushData(false));
    else
      plot_->setPenBrush(sortPenBrush, PenData(false), BrushData(true, tc));

    Point p4 = plot_->pixelToWindow(Point((px1 + px2)/2.0, py2));
    Point p5 = plot_->pixelToWindow(Point(px1, py2 - pSortHeight));
    Point p6 = plot_->pixelToWindow(Point(px2, py2 - pSortHeight));

    Polygon poly2; poly2.addPoint(p4); poly2.addPoint(p5); poly2.addPoint(p6);

    CQChartsDrawUtil::drawRoundedPolygon(device, sortPenBrush, poly2);
  }

  //---

  device->restore();
}

void
CQChartsTableHeaderObj::
getObjSelectIndices(Indices &inds) const
{
  auto modelInd = plot_->modelIndex(0, headerObjData_.c);

  inds.insert(modelInd);
}

bool
CQChartsTableHeaderObj::
rectIntersect(const BBox &r, bool inside) const
{
  auto rect = headerObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}

//------

CQChartsTableRowObj::
CQChartsTableRowObj(const Plot *plot, const Plot::RowObjData &rowObjData) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rowObjData.rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), rowObjData_(rowObjData)
{
}

QString
CQChartsTableRowObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(rowObjData_.r);
}

QString
CQChartsTableRowObj::
calcTipId() const
{
  auto id = rowObjData_.str;

  if (! id.length())
    id = QString("%1").arg(rowObjData_.r);

  return id;
}

void
CQChartsTableRowObj::
draw(PaintDevice *device) const
{
  device->save();

  auto pixelRect = plot_->calcTablePixelRect();

  device->setClipRect(plot_->pixelToWindow(pixelRect));

  //---

  auto *th = const_cast<CQChartsTableRowObj *>(this);

  th->rect_ = rowObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  //---

  // draw row text
  device->setFont(plot_->tableFont());

  auto bg = plot_->interpColor(plot_->cellColor(), ColorInd());
  auto tc = plot_->calcTextColor(bg);

  PenBrush textPenBrush;

  plot_->setPen(textPenBrush, PenData(true, tc));

  device->setPen(textPenBrush.pen);

  CQChartsTextOptions textOptions;

  textOptions.align = rowObjData_.align;

  CQChartsDrawUtil::drawTextInBox(device, rect_, rowObjData_.str, textOptions);

  //---

  device->restore();
}

bool
CQChartsTableRowObj::
rectIntersect(const BBox &r, bool inside) const
{
  auto rect = rowObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}

//------

CQChartsTableCellObj::
CQChartsTableCellObj(const Plot *plot, const Plot::CellObjData &cellObjData) :
 CQChartsPlotObj(const_cast<Plot *>(plot), cellObjData.rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), cellObjData_(cellObjData)
{
}

QString
CQChartsTableCellObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).
           arg(cellObjData_.ind.row()).arg(cellObjData_.ind.column().column());
}

QString
CQChartsTableCellObj::
calcTipId() const
{
  auto id = cellObjData_.str;

  if (! id.length())
    id = QString("%1").arg(cellObjData_.ind.column().column());

  return id;
}

void
CQChartsTableCellObj::
draw(PaintDevice *device) const
{
  device->save();

  auto pixelRect = plot_->calcTablePixelRect();

  device->setClipRect(plot_->pixelToWindow(pixelRect));

  //---

  auto *th = const_cast<CQChartsTableCellObj *>(this);

  th->rect_ = cellObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  //---

  QColor bg;
  bool   bgSet { false };
  bool   useDrawColor { true };

  auto *columnDetails = plot_->columnDetails(cellObjData_.ind.column());

  // calc background pen and brush and draw
  if      (isInside()) {
    PenBrush bgPenBrush;

    bg    = plot_->interpColor(plot_->insideColor(), ColorInd());
    bgSet = true;
  }
  else if (isSelected()) {
    PenBrush bgPenBrush;

    bg    = plot_->interpColor(plot_->selectedColor(), ColorInd());
    bgSet = true;
  }
  else {
    auto tableDrawType = (columnDetails ? columnDetails->tableDrawType() :
                           CQChartsModelColumnDetails::TableDrawType::NORMAL);

    bool   hasRange = false;
    double value { 0.0 }, min { 0.0 }, max { 0.0 }, norm { 0.0 };

    if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP ||
        tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART) {
      auto type = (columnDetails ? columnDetails->type() : CQBaseModelType::STRING);

      if (type == CQBaseModelType::REAL || type == CQBaseModelType::INTEGER) {
        bool ok;
        value = cellObjData_.str.toDouble(&ok);

        // get min/max
        auto *columnTypeMgr = charts()->columnTypeMgr();

        const auto *columnType = columnTypeMgr->getType(type);

        auto minVar = columnType->minValue(columnDetails->nameValues());
        if (! minVar.isValid()) minVar = columnDetails->minValue();

        auto maxVar = columnType->maxValue(columnDetails->nameValues());
        if (! maxVar.isValid()) maxVar = columnDetails->maxValue();

        min = minVar.toReal(&ok);
        max = maxVar.toReal(&ok);

        norm = (max > min ? (value - min)/(max - min) : 0.0);

        hasRange = true;
      }
    }

    if (hasRange) {
      if      (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP) {
        bg = columnDetails->heatmapColor(value, min, max,
               plot_->interpColor(plot_->cellColor(), ColorInd()));
        bgSet = true;
      }
      else if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART) {
        auto xm = plot_->pixelToWindowWidth (1.0);
        auto ym = plot_->pixelToWindowHeight(2.0);

        double barWidth = rect_.getWidth() - 2.0*xm;

        // draw bar for normalized value
        double lw =      norm *barWidth;
        double rw = (1 - norm)*barWidth;

        auto bg1 = columnDetails->barchartColor();
        auto bg2 = plot_->interpColor(plot_->cellColor(), ColorInd());

        BBox lrect(rect_.getXMin() + xm          , rect_.getYMin() + ym,
                   rect_.getXMin() + xm + lw     , rect_.getYMax() - ym);
        BBox rrect(rect_.getXMin() + xm + lw     , rect_.getYMin() + ym,
                   rect_.getXMin() + xm + lw + rw, rect_.getYMax() - ym);

        PenBrush bgPenBrush;

        plot_->setPenBrush(bgPenBrush, PenData(false), BrushData(true, bg1));
        CQChartsDrawUtil::setPenBrush(device, bgPenBrush);

        device->drawRect(lrect);

        plot_->setPenBrush(bgPenBrush, PenData(false), BrushData(true, bg2));
        CQChartsDrawUtil::setPenBrush(device, bgPenBrush);

        device->drawRect(rrect);

        useDrawColor = false;
      }
    }
  }

  if (bgSet) {
    PenBrush bgPenBrush;

    plot_->setPenBrush(bgPenBrush, PenData(false), BrushData(true, bg));

    CQChartsDrawUtil::setPenBrush(device, bgPenBrush);

    device->drawRect(rect_);
  }

  //---

  CQChartsColor textColor;

  if (! bgSet) {
    textColor = plot_->calcTextColor(plot_->cellColor());

    if (useDrawColor && columnDetails) {
      const auto &drawColor = columnDetails->tableDrawColor();

      if (drawColor.isValid())
        textColor = drawColor;
    }

    if      (isInside())
      textColor = plot_->calcTextColor(plot_->insideColor());
    else if (isSelected())
      textColor = plot_->calcTextColor(plot_->selectedColor());
  }
  else {
    textColor = CQChartsColor(plot_->calcTextColor(bg));
  }

  //---

  // draw cell text
  device->setFont(plot_->tableFont());

  PenBrush textPenBrush;

  plot_->setPen(textPenBrush,
    PenData(true, plot_->interpColor(textColor, ColorInd())));

  device->setPen(textPenBrush.pen);

  CQChartsTextOptions textOptions;

  textOptions.align = cellObjData_.align;

  CQChartsDrawUtil::drawTextInBox(device, rect_, cellObjData_.str, textOptions);

  //---

  device->restore();
}

void
CQChartsTableCellObj::
getObjSelectIndices(Indices &inds) const
{
  auto modelInd = plot_->modelIndex(cellObjData_.ind);

  inds.insert(modelInd);
}

bool
CQChartsTableCellObj::
inside(const Point &p) const
{
  if (! isVisible()) return false;

  auto rect = cellObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  return rect.inside(p);
}

bool
CQChartsTableCellObj::
rectIntersect(const BBox &r, bool inside) const
{
  auto rect = cellObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}
