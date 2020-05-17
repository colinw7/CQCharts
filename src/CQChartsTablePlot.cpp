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
#include <CQChartsPaintDevice.h>
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
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  CQChartsColumns columns;

  int nc = details->numColumns();

  for (int i = 0; i < nc; ++i)
    columns.addColumn(CQChartsColumn(i));

  analyzeModelData.parameterNameColumns["columns"] = columns;
}

CQChartsPlot *
CQChartsTablePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsTablePlot(view, model);
}

//------

CQChartsTablePlot::
CQChartsTablePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("table"), model)
{
  NoUpdate noUpdate(this);

  //---

  // plot, data, fit background
  setPlotFilled(false); setPlotStroked(false);
  setDataFilled(false); setDataStroked(false);
  setFitFilled (false); setFitStroked (false);

  setDataClip(false);

  setPlotFillColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.00));
  setDataFillColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.00));
  setFitFillColor (CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.00));

  //---

  CQChartsFont font;

  font.decFontSize(8);

  font_ = font;

  //---

  if (! CQChartsModelUtil::isHierarchical(model.data()))
    summaryModel_ = new CQSummaryModel(model.data());

  int pageSize = 1024;

  setMaxRows (10*pageSize);
  setPageSize(pageSize);

  setGridColor  (CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.50));
  setTextColor  (CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1.00));
  setHeaderColor(QColor(150, 150, 200));
  setCellColor  (CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.00));

  //---

  setOuterMargin(0, 0, 0, 0);

  addTitle();

  //---

  scrollData_.hbar = new QScrollBar(Qt::Horizontal, view);
  scrollData_.vbar = new QScrollBar(Qt::Vertical  , view);

  scrollData_.hbar->hide();
  scrollData_.vbar->hide();

  connect(scrollData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
  connect(scrollData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));

  scrollData_.pixelBarSize = view->style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2;
}

CQChartsTablePlot::
~CQChartsTablePlot()
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

  if (! CQChartsModelUtil::isHierarchical(model.data()))
    summaryModel_ = new CQSummaryModel(model.data());

  connectModel();

  updateRangeAndObjs();

  emit modelChanged();
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
setColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(columns_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsTablePlot::
setFont(const CQChartsFont &f)
{
  CQChartsUtil::testAndSet(font_, f, [&]() { updateRangeAndObjs(); } );
}

//---

CQChartsTablePlot::Mode
CQChartsTablePlot::
mode() const
{
  return (summaryModel_ ? (CQChartsTablePlot::Mode) summaryModel_->mode() : Mode::NORMAL);
}

void
CQChartsTablePlot::
setMode(const Mode &m)
{
  if (m != mode()) {
    if (summaryModel_)
      summaryModel_->setMode((CQSummaryModel::Mode) m);

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
  return (summaryModel_ ? CQChartsColumnNum(summaryModel_->sortColumn()) : CQChartsColumnNum());
}

void
CQChartsTablePlot::
setSortColumn(const CQChartsColumnNum &c)
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
setHeaderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(headerData_.color, c, [&]() { updateObjs(); } );
}

//---

void
CQChartsTablePlot::
setGridColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(gridColor_, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textColor_, c, [&]() { updateObjs(); } );
}

void
CQChartsTablePlot::
setCellColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(cellColor_, c, [&]() { updateObjs(); } );
}

//---

void
CQChartsTablePlot::
setIndent(double r)
{
  CQChartsUtil::testAndSet(indent_, r, [&]() { updateObjs(); } );
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
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addBaseProperties();

  // columns
  addProp("columns", "columns", "columns", "Columns");

  addProp("options", "mode"       , "mode"       , "Set mode"        );
  addProp("options", "maxRows"    , "maxRows"    , "Set max rows"    )->setMinValue(1);
  addProp("options", "sortColumn" , "sortColumn" , "Set sort column" );
  addProp("options", "sortRole"   , "sortRole"   , "Set sort role"   )->setHidden(true);
  addProp("options", "sortOrder"  , "sortOrder"  , "Set sort order"  );
  addProp("options", "pageSize"   , "pageSize"   , "Set page size"   )->setMinValue(1);
  addProp("options", "currentPage", "currentPage", "Set current page")->setMinValue(0);
  addProp("options", "rowNums"    , "rowNums"    , "Set row numbers" );
  addProp("options", "rowColumn"  , "rowColumn"  , "Display row number column" );

  addProp("header", "headerVisible", "visible", "Header visible");
  addProp("header", "headerColor"  , "color"  , "Header color"  );

  addProp("options", "gridColor"  , "gridColor"  , "Grid color"  );
  addProp("options", "textColor"  , "textColor"  , "Text color"  );
  addProp("options", "cellColor"  , "cellColor"  , "Cell color"  );

  addProp("options", "indent"    , "indent"    , "Hierarchical row indent")->setMinValue(0.0);
  addProp("options", "followView", "followView", "Follow view");
}

CQChartsGeom::Range
CQChartsTablePlot::
calcRange() const
{
  // always return range (0,0) -> (1,1)
  CQChartsGeom::Range dataRange;

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
  auto *th = const_cast<CQChartsTablePlot *>(this);

  //---

  QModelIndexList expandInds;

  if (isFollowView())
    view()->expandedModelIndices(expandInds);

  //---

  th->tableData_.expandInds = expandInds;

  th->tableData_.font = view()->viewFont(this->font());
  th->tableData_.font = CQChartsUtil::scaleFontSize(th->tableData_.font, fontScale_);

  //th->tabbedFont_ = th->tableData_.font;

  QFontMetricsF fm(th->tableData_.font);

  th->tableData_.nc = columns_.count();

  if (summaryModel_) {
    th->tableData_.nr       = summaryModel_->rowCount();
    th->tableData_.maxDepth = 0;
  }
  else {
    CQChartsModelUtil::hierData(charts(), model().data(), th->tableData_.nr,
                                th->tableData_.maxDepth);
  }

  th->tableData_.prh = fm.height() + 2*tableData_.pmargin;

  // calc column widths
  if (isRowColumn()) {
    ColumnData &data = th->tableData_.rowColumnData;

    const int power = CMathRound::RoundUp(log10(tableData_.nr));

    data.pwidth  = power*fm.width("X") + 2*tableData_.pmargin;
    data.numeric = false;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    auto *columnDetails = this->columnDetails(c);
    if (! columnDetails) continue;

    ColumnData &data = th->tableData_.columnDataMap[c];

    bool ok;

    QString str;

    if (summaryModel())
      str = CQChartsModelUtil::modelHHeaderString(summaryModel(), c, ok);
    else
      str = CQChartsModelUtil::modelHHeaderString(model().data(), c, ok);

    if (! ok) continue;

    double cw = fm.width(str) + 2*tableData_.pmargin;

    if (i == 0)
      cw += tableData_.maxDepth*indent();

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
        QModelIndex ind = model_->index(data.row, 0, data.parent);

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
        const auto &c = plot_->columns().getColumn(i);

        CQChartsModelIndex ind(data.row, c, data.parent);

        bool ok;

        QString str = CQChartsModelUtil::modelString(plot_->charts(), model_, ind, ok);
        if (! ok) continue;

        ColumnData &data = tableData_.columnDataMap[c];

        double cw = fm_.width(str) + 2*tableData_.pmargin;

        if (i == 0)
          cw += tableData_.maxDepth*plot_->indent();

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

  visitor.init();

  if (summaryModel_)
    CQChartsModelVisit::exec(charts(), summaryModel_, visitor);
  else
    CQChartsModelVisit::exec(charts(), model().data(), visitor);

  th->tableData_.nvr = visitor.numProcessedRows();

  //---

  // set full table width
  th->tableData_.pcw = 0.0;

  if (isRowColumn()) {
    const ColumnData &data = th->tableData_.rowColumnData;

    th->tableData_.pcw += data.pwidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    const ColumnData &data = th->tableData_.columnDataMap[c];

    if (data.prefWidth > 0)
      th->tableData_.pcw += data.prefWidth + 2*tableData_.pmargin;
    else
      th->tableData_.pcw += data.pwidth;
  }
}

bool
CQChartsTablePlot::
createObjs(PlotObjs &objs) const
{
  updatePosition();

  initDrawData();

  for (const auto &pc : headerObjMap_) {
    const auto &headerObjData = pc.second;

    auto *obj = new CQChartsTableHeaderObj(this, headerObjData);

    objs.push_back(obj);
  }

  for (const auto &pr : rowObjMap_) {
    const auto &rowObjData = pr.second;

    auto *obj = new CQChartsTableRowObj(this, rowObjData);

    objs.push_back(obj);
  }

  for (const auto &pc : cellObjMap_) {
    const auto &cellObjData = pc.second;

    auto *obj = new CQChartsTableCellObj(this, cellObjData);

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

  QString name = action->text();

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
  setSortColumn(CQChartsColumnNum(sortColumnSpin_->value() - 1));

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
execDrawBackground(CQChartsPaintDevice *device) const
{
  device->save();

  setClipRect(device);

  drawTable(device);

  device->restore();
}

void
CQChartsTablePlot::
drawTable(CQChartsPaintDevice *device) const
{
  updateScrollBars();

  updatePosition();

  drawTableBackground(device);
}

void
CQChartsTablePlot::
initDrawData() const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  th->headerObjMap_.clear();
  th->rowObjMap_   .clear();
  th->cellObjMap_  .clear();

  //---

  th->tableData_.rh = pixelToWindowHeight(th->tableData_.prh);

  //---

  // calc column widths
  double x = th->tableData_.xo;

  if (isRowColumn()) {
    ColumnData &data = th->tableData_.rowColumnData;

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    x += data.drawWidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    ColumnData &data = th->tableData_.columnDataMap[c];

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    if (data.prefWidth > 0)
      data.drawWidth = pixelToWindowWidth(data.prefWidth + 2*tableData_.pmargin);

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
  if (! isVisible()) {
    scrollData_.hbar->setVisible(false);
    scrollData_.vbar->setVisible(false);

    return;
  }

  //---

  auto *th = const_cast<CQChartsTablePlot *>(this);

  //---

  auto pixelRect = calcTablePixelRect();

  double pdx = pixelRect.getWidth () - 2*tableData_.pmargin - tableData_.pcw;
  double pdy = pixelRect.getHeight() - 2*tableData_.pmargin - tableData_.prh*tableData_.nvr;

  if (isHeaderVisible())
    pdy -= tableData_.prh;

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
    scrollData_.hbar->setPageStep(pixelRect.getWidth() - 2*tableData_.pmargin);
  }
  else
    th->scrollData_.hpos = 0;

  if (scrollData_.vbar->isVisible()) {
    scrollData_.vbar->move(pixelRect.getXMax() - scrollData_.pixelBarSize, pixelRect.getYMin());
    scrollData_.vbar->resize(scrollData_.pixelBarSize, pixelRect.getHeight());

    scrollData_.vbar->setRange(0, -pdy);
    scrollData_.vbar->setPageStep(pixelRect.getHeight() - 2*tableData_.pmargin);
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
  auto *th = const_cast<CQChartsTablePlot *>(this);

  //---

  auto pixelRect = calcTablePixelRect();

  double pdx = pixelRect.getWidth () - 2*tableData_.pmargin - tableData_.pcw;
  double pdy = pixelRect.getHeight() - 2*tableData_.pmargin - tableData_.prh*tableData_.nvr;

  if (isHeaderVisible())
    pdy -= tableData_.prh;

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
drawTableBackground(CQChartsPaintDevice *device) const
{
  if (! isVisible())
    return;

  //--

  auto *th = const_cast<CQChartsTablePlot *>(this);

  device->save();

  auto pixelRect = calcTablePixelRect();

  device->setClipRect(pixelToWindow(pixelRect));

  //---

  // calc column widths
  double x = th->tableData_.xo + th->tableData_.sx;

  if (isRowColumn()) {
    ColumnData &data = th->tableData_.rowColumnData;

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    x += data.drawWidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    ColumnData &data = th->tableData_.columnDataMap[c];

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    if (data.prefWidth > 0)
      data.drawWidth = pixelToWindowWidth(data.prefWidth + 2*tableData_.pmargin);

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
    y1 += tableData_.rh;

  // draw header background
  if (isHeaderVisible()) {
    QBrush headerBrush;

    setBrush(headerBrush, true, interpColor(headerColor(), ColorInd()),
             CQChartsAlpha(), CQChartsFillPattern());

    device->setBrush(headerBrush);

    if (x2 > x1) {
      CQChartsGeom::BBox bbox(x1, y1 - th->tableData_.rh, x2, y1);

      device->fillRect(bbox, device->brush());
    }
  }

  // draw cells background
  QBrush cellBrush;

  setBrush(cellBrush, true, interpColor(cellColor(), ColorInd()),
           CQChartsAlpha(), CQChartsFillPattern());

  device->setBrush(cellBrush);

  if (x2 > x1 && tableData_.nvr > 0) {
    CQChartsGeom::BBox bbox(x1, y2, x2, y2 + trh);

    device->fillRect(bbox, device->brush());
  }

  //---

  // draw table column lines
  QPen gridPen;

  setPen(gridPen, true, interpColor(gridColor(), ColorInd()),
         CQChartsAlpha(), 0.0, CQChartsLineDash());

  device->setPen(gridPen);

  x = x1;

  // number column vertical line
  if (isRowColumn()) {
    const ColumnData &data = th->tableData_.rowColumnData;

    device->drawLine(CQChartsGeom::Point(x, y1), CQChartsGeom::Point(x, y2));

    x += data.drawWidth;
  }

  // column vertical lines
  for (int i = 0; i < tableData_.nc; ++i) {
    const auto &c = columns().getColumn(i);

    const ColumnData &data = th->tableData_.columnDataMap[c];

    device->drawLine(CQChartsGeom::Point(x, y1), CQChartsGeom::Point(x, y2));

    x += data.drawWidth;
  }

  // right edge
  device->drawLine(CQChartsGeom::Point(x, y1), CQChartsGeom::Point(x, y2));

  // draw header and row lines
  double y = th->tableData_.yo - th->tableData_.sy;

  if (isHeaderVisible()) {
    device->drawLine(CQChartsGeom::Point(x1, y), CQChartsGeom::Point(x2, y));

    y += tableData_.rh;
  }

  for (int i = 0; i < tableData_.nvr; ++i) {
    device->drawLine(CQChartsGeom::Point(x1, y), CQChartsGeom::Point(x2, y));

    y += tableData_.rh;
  }

  // bottom edge
  device->drawLine(CQChartsGeom::Point(x1, y), CQChartsGeom::Point(x2, y));

  //---

  device->restore();
}

void
CQChartsTablePlot::
createTableObjData() const
{
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

      const double y = tableData_.yo + (tableData_.nvr - data.vrow - 1)*tableData_.rh;

      double x = tableData_.xo;

      //---

      // draw line number
      if (plot_->isRowColumn()) {
        const ColumnData &cdata = tableData_.rowColumnData;

        drawRowNumber(x, y, data.vrow + 1);

        x += cdata.drawWidth;
      }

      //---

      // draw cell values
      drawCellValues(x, y, data);

      //---

      expandStack_.push_back(expanded_);

      if (plot_->isFollowView()) {
        QModelIndex ind = model_->index(data.row, 0, data.parent);

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

      const double y = tableData_.yo + (tableData_.nvr - data.vrow - 1)*tableData_.rh;

      double x = tableData_.xo;

      //---

      // draw line number
      if (plot_->isRowColumn()) {
        const ColumnData &cdata = tableData_.rowColumnData;

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
        CQChartsColumn c;

        auto &headerObjData = plot_->getHeaderObjData(c);

        headerObjData.str = " ";

        //---

        const ColumnData &cdata = tableData_.rowColumnData;

        headerObjData.rect =
          CQChartsGeom::BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.rh);

        //---

        x += cdata.drawWidth;
      }

      // draw column headers
      for (int ic = 0; ic < tableData_.nc; ++ic) {
        const auto &c = plot_->columns().getColumn(ic);

        bool ok;

        QString str = CQChartsModelUtil::modelHHeaderString(model_, c, ok);
        if (! ok) continue;

        //---

        auto &headerObjData = plot_->getHeaderObjData(c);

        headerObjData.str = str;

        //---

        const ColumnData &cdata = tableData_.columnDataMap[c];

        if (cdata.numeric)
          headerObjData.align = Qt::AlignRight | Qt::AlignVCenter;
        else
          headerObjData.align = Qt::AlignLeft | Qt::AlignVCenter;

        headerObjData.rect =
          CQChartsGeom::BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.rh);

        //---

        x += cdata.drawWidth;
      }
    }

    void drawRowNumber(double x, double y, int n) {
      const ColumnData &cdata = tableData_.rowColumnData;

      //---

      auto &rowObjData = plot_->getRowObjData(n);

      rowObjData.align = Qt::AlignRight | Qt::AlignVCenter;

      rowObjData.rect =
        CQChartsGeom::BBox(x + xm_, y, x + cdata.drawWidth - xm_, y + tableData_.rh);

      rowObjData.str = QString("%1").arg(n);
    }

    void drawCellValues(double x, double y, const VisitData &data) {
      for (int ic = 0; ic < tableData_.nc; ++ic) {
        const auto &c = plot_->columns().getColumn(ic);

        const ColumnData &cdata = tableData_.columnDataMap[c];

        drawCellValue(x, y, data, ic);

        x += cdata.drawWidth;
      }
    }

    void drawCellValue(double x, double y, const VisitData &data, int ic) {
      const auto &c = plot_->columns().getColumn(ic);

      //---

      CQChartsModelIndex ind(data.row, c, data.parent);

      bool ok;

      QString str = CQChartsModelUtil::modelString(plot_->charts(), model_, ind, ok);
      if (! ok) return;

      //---

      auto &cellObjData = plot_->getCellObjData(ind);

      cellObjData.str = str;

      //---

      const ColumnData &cdata = tableData_.columnDataMap[c];

      if (cdata.numeric)
        cellObjData.align = Qt::AlignRight | Qt::AlignVCenter;
      else
        cellObjData.align = Qt::AlignLeft | Qt::AlignVCenter;

      //---

      double x1 = x;

      if (ic == 0)
        x1 += depth_*xd_;

      cellObjData.rect =
        CQChartsGeom::BBox(x1 + xm_, y, x1 + cdata.drawWidth - xm_, y + tableData_.rh);
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

  visitor.init();

  if (summaryModel_)
    CQChartsModelVisit::exec(charts(), summaryModel_, visitor);
  else
    CQChartsModelVisit::exec(charts(), model().data(), visitor);
}

CQChartsTablePlot::HeaderObjData &
CQChartsTablePlot::
getHeaderObjData(const CQChartsColumn &c) const
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
getCellObjData(const CQChartsModelIndex &ind) const
{
  auto *th = const_cast<CQChartsTablePlot *>(this);

  QModelIndex modelInd = th->modelIndex(ind);

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

    if (dataOffsetX() > 2*dx)
      setDataOffsetX(2*dx);
  }
  else
    setDataOffsetX(0.0);

  if (tableData_.dy < 0) {
    const double dy = -tableData_.dy;

    if (dataOffsetY() < 0)
      setDataOffsetY(0.0);

    if (dataOffsetY() > 2*dy)
      setDataOffsetY(2*dy);
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
  auto pixelRect = calcDataPixelRect();

  if (! isTabbed())
    return pixelRect;

  int px1 = pixelRect.getXMin();
  int px2 = pixelRect.getXMax();
  int py1 = pixelRect.getYMin();
  int py2 = pixelRect.getYMax();

  auto tabRect = this->calcTabPixelRect();

  double tph = tabRect.getHeight();

  return CQChartsGeom::BBox(px1, py1, px2, py2 - tph);
}

//------

CQChartsTableHeaderObj::
CQChartsTableHeaderObj(const CQChartsTablePlot *plot,
                       const CQChartsTablePlot::HeaderObjData &headerObjData) :
 CQChartsPlotObj(const_cast<CQChartsTablePlot *>(plot), headerObjData.rect,
                 ColorInd(), ColorInd(), ColorInd()),
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
  QString id = headerObjData_.str;

  if (! id.length())
    id = QString("%1").arg(headerObjData_.c.column());

  return id;
}

void
CQChartsTableHeaderObj::
draw(CQChartsPaintDevice *device)
{
  device->save();

  auto pixelRect = plot_->calcTablePixelRect();

  device->setClipRect(plot_->pixelToWindow(pixelRect));

  //---

  device->setFont(plot_->tableFont());

  QPen textPen;

  plot_->setPen(textPen, true, plot_->interpColor(plot_->textColor(), ColorInd()),
                CQChartsAlpha(), 0.0, CQChartsLineDash());

  device->setPen(textPen);

  CQChartsTextOptions textOptions;

  textOptions.align = headerObjData_.align;

  rect_ = headerObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  CQChartsDrawUtil::drawTextInBox(device, rect_, headerObjData_.str, textOptions);

  //---

  device->restore();
}

void
CQChartsTableHeaderObj::
getObjSelectIndices(Indices &inds) const
{
  QModelIndex modelInd = plot_->modelIndex(0, CQChartsColumn(headerObjData_.c));

  inds.insert(modelInd);
}

bool
CQChartsTableHeaderObj::
rectIntersect(const CQChartsGeom::BBox &r, bool inside) const
{
  CQChartsGeom::BBox rect = headerObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}

//------

CQChartsTableRowObj::
CQChartsTableRowObj(const CQChartsTablePlot *plot,
                    const CQChartsTablePlot::RowObjData &rowObjData) :
 CQChartsPlotObj(const_cast<CQChartsTablePlot *>(plot), rowObjData.rect,
                 ColorInd(), ColorInd(), ColorInd()),
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
  QString id = rowObjData_.str;

  if (! id.length())
    id = QString("%1").arg(rowObjData_.r);

  return id;
}

void
CQChartsTableRowObj::
draw(CQChartsPaintDevice *device)
{
  device->save();

  auto pixelRect = plot_->calcTablePixelRect();

  device->setClipRect(plot_->pixelToWindow(pixelRect));

  //---

  device->setFont(plot_->tableFont());

  QPen textPen;

  plot_->setPen(textPen, true, plot_->interpColor(plot_->textColor(), ColorInd()),
                CQChartsAlpha(), 0.0, CQChartsLineDash());

  device->setPen(textPen);

  CQChartsTextOptions textOptions;

  textOptions.align = rowObjData_.align;

  rect_ = rowObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  CQChartsDrawUtil::drawTextInBox(device, rect_, rowObjData_.str, textOptions);

  //---

  device->restore();
}

bool
CQChartsTableRowObj::
rectIntersect(const CQChartsGeom::BBox &r, bool inside) const
{
  CQChartsGeom::BBox rect = rowObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}

//------

CQChartsTableCellObj::
CQChartsTableCellObj(const CQChartsTablePlot *plot,
                     const CQChartsTablePlot::CellObjData &cellObjData) :
 CQChartsPlotObj(const_cast<CQChartsTablePlot *>(plot), cellObjData.rect,
                 ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), cellObjData_(cellObjData)
{
}

QString
CQChartsTableCellObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).
           arg(cellObjData_.ind.row).arg(cellObjData_.ind.column.column());
}

QString
CQChartsTableCellObj::
calcTipId() const
{
  QString id = cellObjData_.str;

  if (! id.length())
    id = QString("%1").arg(cellObjData_.ind.column.column());

  return id;
}

void
CQChartsTableCellObj::
draw(CQChartsPaintDevice *device)
{
  device->save();

  auto pixelRect = plot_->calcTablePixelRect();

  device->setClipRect(plot_->pixelToWindow(pixelRect));

  //---

  CQChartsColor textColor = plot_->textColor();

  auto *columnDetails = plot_->columnDetails(cellObjData_.ind.column);

  if (columnDetails) {
    const auto &drawColor = columnDetails->tableDrawColor();

    if (drawColor.isValid())
      textColor = drawColor;
  }

  //---

  device->setFont(plot_->tableFont());

  QPen textPen;

  plot_->setPen(textPen, true, plot_->interpColor(textColor, ColorInd()),
                CQChartsAlpha(), 0.0, CQChartsLineDash());

  device->setPen(textPen);

  CQChartsTextOptions textOptions;

  textOptions.align = cellObjData_.align;

  rect_ = cellObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  CQChartsDrawUtil::drawTextInBox(device, rect_, cellObjData_.str, textOptions);

  //---

  device->restore();
}

void
CQChartsTableCellObj::
getObjSelectIndices(Indices &inds) const
{
  QModelIndex modelInd = plot_->modelIndex(cellObjData_.ind);

  inds.insert(modelInd);
}

bool
CQChartsTableCellObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! isVisible()) return false;

  CQChartsGeom::BBox rect = cellObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  return rect.inside(p);
}

bool
CQChartsTableCellObj::
rectIntersect(const CQChartsGeom::BBox &r, bool inside) const
{
  CQChartsGeom::BBox rect = cellObjData_.rect.translated(plot_->scrollX(), -plot_->scrollY());

  if (inside)
    return r.inside(rect);
  else
    return r.overlaps(rect);
}
