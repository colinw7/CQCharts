#include <CQChartsTablePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQIntegerSpin.h>
#include <CMathRound.h>

#include <QMenu>
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
  CQChartsModelDetails *details = modelData->details();
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

  setFont(font);

  //---

  if (! CQChartsModelUtil::isHierarchical(model.data()))
    summaryModel_ = new CQSummaryModel(model.data());

  setMaxRows (60);
  setPageSize(60);

  setGridColor  (QColor(128, 128, 128));
  setTextColor  (QColor(  0,   0,   0));
  setHeaderColor(QColor(150, 150, 200));
  setCellColor  (QColor(255, 255, 255));

  //---

  setOuterMargin(0, 0, 0, 0);

  addTitle();
}

CQChartsTablePlot::
~CQChartsTablePlot()
{
  delete summaryModel_;
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

int
CQChartsTablePlot::
sortColumn() const
{
  return (summaryModel_ ? summaryModel_->sortColumn() : 0);
}

void
CQChartsTablePlot::
setSortColumn(int i)
{
  if (i != sortColumn()) {
    if (summaryModel_)
      summaryModel_->setSortColumn(i);

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
  const QStringList strs = str.split(" ", QString::SkipEmptyParts);

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
setHeaderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(headerColor_, c, [&]() { updateObjs(); } );
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

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "columns", "columns", "Columns");

  addProp("options", "mode"       , "mode"       , "Set mode"        );
  addProp("options", "maxRows"    , "maxRows"    , "Set max rows"    );
  addProp("options", "sortColumn" , "sortColumn" , "Set sort column" );
  addProp("options", "sortRole"   , "sortRole"   , "Set sort role"   );
  addProp("options", "sortOrder"  , "sortOrder"  , "Set sort order"  );
  addProp("options", "pageSize"   , "pageSize"   , "Set page size"   );
  addProp("options", "currentPage", "currentPage", "Set current page");
  addProp("options", "rowNums"    , "rowNums"    , "Set row numbers" );
  addProp("options", "rowColumn"  , "rowColumn"  , "Display row number column" );

  addProp("options", "gridColor"  , "gridColor"  , "Grid color"  );
  addProp("options", "textColor"  , "textColor"  , "Text color"  );
  addProp("options", "headerColor", "headerColor", "Header color");
  addProp("options", "cellColor"  , "cellColor"  , "Cell color"  );

  addProp("options", "indent"    , "indent"    , "Hierarchical row indent");
  addProp("options", "followView", "followView", "Follow view");
}

CQChartsGeom::Range
CQChartsTablePlot::
calcRange() const
{
  CQChartsTablePlot *th = const_cast<CQChartsTablePlot *>(this);

  const int pxm = 2;

  //---

  QModelIndexList expandInds;

  if (isFollowView())
    view()->expandedModelIndices(expandInds);

  //---

  th->tableData_.expandInds = expandInds;

  th->tableData_.font = view()->viewFont(this->font());

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

  th->tableData_.prh = fm.height() + 2*pxm;

  // calc column widths
  if (isRowColumn()) {
    ColumnData &data = th->tableData_.rowColumnData;

    const int power = CMathRound::RoundUp(log10(tableData_.nr));

    data.pwidth  = power*fm.width("X") + 2*pxm;
    data.numeric = false;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const CQChartsColumn &c = columns().getColumn(i);

    CQChartsModelColumnDetails *columnDetails = this->columnDetails(c);
    if (! columnDetails) continue;

    ColumnData &data = th->tableData_.columnDataMap[c];

    bool ok;

    QString str;

    if (summaryModel())
      str = CQChartsModelUtil::modelHeaderString(summaryModel(), c, ok);
    else
      str = CQChartsModelUtil::modelHeaderString(model().data(), c, ok);

    if (! ok) continue;

    double cw = fm.width(str) + 2*pxm;

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

      const int pxm = 2;

      for (int i = 0; i < tableData_.nc; ++i) {
        const CQChartsColumn &c = plot_->columns().getColumn(i);

        bool ok;

        QString str = CQChartsModelUtil::modelString(plot_->charts(), model_,
                                                     data.row, c, data.parent, ok);
        if (! ok) continue;

        ColumnData &data = tableData_.columnDataMap[c];

        double cw = fm_.width(str) + 2*pxm;

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
  th->tableData_.rcw = 0.0;

  if (isRowColumn()) {
    const ColumnData &data = th->tableData_.rowColumnData;

    th->tableData_.pcw += data.pwidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const CQChartsColumn &c = columns().getColumn(i);

    const ColumnData &data = th->tableData_.columnDataMap[c];

    if (data.prefWidth > 0)
      th->tableData_.pcw += data.prefWidth + 2*pxm;
    else
      th->tableData_.pcw += data.pwidth;
  }

  //---

  CQChartsGeom::Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(1.0, 1.0);

  return dataRange;
}

bool
CQChartsTablePlot::
createObjs(PlotObjs &) const
{
  return true;
}

double
CQChartsTablePlot::
getPanY(bool /*is_shift*/) const
{
  return windowToViewHeight(tableData_.rh);
}

//------

bool
CQChartsTablePlot::
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

//auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) -> QAction *{
//  return addMenuCheckedAction(menu, name, isSet, slot);
//};

  //---

  menu_ = menu;

  //---

  menu->addSeparator();

  QMenu *modeMenu = new QMenu("Table Mode");

  for (const auto &mode : modes())
    (void) addMenuCheckedAction(modeMenu, modeName(mode), this->mode() == mode,
                                SLOT(setModeSlot(bool)));

  menu->addMenu(modeMenu);

  //---

  QMenu *maxRowsMenu = new QMenu("Max Rows");

  maxRowsSpin_ = new CQIntegerSpin;

  maxRowsSpin_->setRange(1, 9999);
  maxRowsSpin_->setValue(maxRows());

  connect(maxRowsSpin_, SIGNAL(valueChanged(int)), this, SLOT(maxRowsSlot()));
  connect(maxRowsSpin_, SIGNAL(editingFinished()), this, SLOT(maxRowsSlot()));

  maxRowsMenu->addAction(new CQChartsWidgetAction(maxRowsSpin_));

  menu->addMenu(maxRowsMenu);

  //---

  QMenu *sortColumnMenu = new QMenu("Sort Column");

  sortColumnSpin_ = new CQIntegerSpin;

  sortColumnSpin_->setRange(1, columns_.count());
  sortColumnSpin_->setValue(sortColumn() + 1);

  connect(sortColumnSpin_, SIGNAL(valueChanged(int)), this, SLOT(sortColumnSlot()));
  connect(sortColumnSpin_, SIGNAL(editingFinished()), this, SLOT(sortColumnSlot()));

  sortColumnMenu->addAction(new CQChartsWidgetAction(sortColumnSpin_));

  menu->addMenu(sortColumnMenu);

  //---

  QMenu *pageSizeMenu = new QMenu("Page Size");

  pageSizeSpin_ = new CQIntegerSpin;

  pageSizeSpin_->setRange(1, 9999);
  pageSizeSpin_->setValue(pageSize());

  connect(pageSizeSpin_, SIGNAL(valueChanged(int)), this, SLOT(pageSizeSlot()));
  connect(pageSizeSpin_, SIGNAL(editingFinished()), this, SLOT(pageSizeSlot()));

  pageSizeMenu->addAction(new CQChartsWidgetAction(pageSizeSpin_));

  menu->addMenu(pageSizeMenu);

  //---

  QMenu *pageNumMenu = new QMenu("Page Number");

  pageNumSpin_ = new CQIntegerSpin;

  pageNumSpin_->setRange(1, 9999);
  pageNumSpin_->setValue(currentPage() + 1);

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

  QAction *action = qobject_cast<QAction *>(sender());
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
  setSortColumn(sortColumnSpin_->value() - 1);

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

bool
CQChartsTablePlot::
hasForeground() const
{
  return true;
}

void
CQChartsTablePlot::
execDrawForeground(CQChartsPaintDevice *device) const
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
  CQChartsTablePlot *th = const_cast<CQChartsTablePlot *>(this);

  th->tableData_.font = view()->viewFont(this->font());

  //---

  const int pxm = 2;

  th->tableData_.rh = pixelToWindowHeight(th->tableData_.prh);

  const CQChartsGeom::BBox pixelRect = this->calcPlotPixelRect();

  const double pdx = (pixelRect.getWidth () - 2*pxm -
                      th->tableData_.pcw                         )/2.0;
  const double pdy = (pixelRect.getHeight() - 2*pxm -
                      th->tableData_.prh*(th->tableData_.nvr + 1))/2.0;

  th->tableData_.dx = pixelToSignedWindowWidth (pdx);
  th->tableData_.dy = pixelToSignedWindowHeight(pdy);

  th->tableData_.xo = std::max(th->tableData_.dx, 0.0);
  th->tableData_.yo = std::max(th->tableData_.dy, 0.0);

  //---

  device->setFont(th->tableData_.font);

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
    const CQChartsColumn &c = columns().getColumn(i);

    ColumnData &data = th->tableData_.columnDataMap[c];

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    if (data.prefWidth > 0)
      data.drawWidth = pixelToWindowWidth(data.prefWidth + 2*pxm);

    th->tableData_.columnDataMap[c] = data;

    x += data.drawWidth;
  }

  //---

  const double trh = tableData_.nvr*th->tableData_.rh;

  const double x1 = th->tableData_.xo;                       // left
  const double y1 = th->tableData_.yo + trh + tableData_.rh; // top
  const double x2 = x;                                       // right
  const double y2 = th->tableData_.yo;                       // bottom

  //---

  // draw header background
  QBrush headerBrush;

  setBrush(headerBrush, true, interpColor(headerColor(), ColorInd()), 1.0, CQChartsFillPattern());

  device->setBrush(headerBrush);

  if (x2 > x1)
    device->fillRect(QRectF(x1, y1 - th->tableData_.rh, x2 - x1, th->tableData_.rh),
                     device->brush());

  // draw cells background
  QBrush cellBrush;

  setBrush(cellBrush, true, interpColor(cellColor(), ColorInd()), 1.0, CQChartsFillPattern());

  device->setBrush(cellBrush);

  if (x2 > x1 && tableData_.nvr > 0)
    device->fillRect(QRectF(x1, y2, x2 - x1, trh), device->brush());

  //---

  // draw table column lines
  QPen gridPen;

  setPen(gridPen, true, interpColor(gridColor(), ColorInd()), 1.0, 0.0, CQChartsLineDash());

  device->setPen(gridPen);

  x = x1;

  // number column vertical line
  if (isRowColumn()) {
    const ColumnData &data = th->tableData_.rowColumnData;

    device->drawLine(QPointF(x, y1), QPointF(x, y2));

    x += data.drawWidth;
  }

  // column vertical lines
  for (int i = 0; i < tableData_.nc; ++i) {
    const CQChartsColumn &c = columns().getColumn(i);

    const ColumnData &data = th->tableData_.columnDataMap[c];

    device->drawLine(QPointF(x, y1), QPointF(x, y2));

    x += data.drawWidth;
  }

  // right edge
  device->drawLine(QPointF(x, y1), QPointF(x, y2));

  // draw row lines
  double y = th->tableData_.yo;

  for (int i = 0; i < tableData_.nvr + 1; ++i) {
    device->drawLine(QPointF(x1, y), QPointF(x2, y));

    y += tableData_.rh;
  }

  // bottom edge
  device->drawLine(QPointF(x1, y), QPointF(x2, y));

  //---

  QPen textPen;

  setPen(textPen, true, interpColor(textColor(), ColorInd()), 1.0, 1.0, CQChartsLineDash());

  device->setPen(textPen);

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsTablePlot *plot, CQChartsPaintDevice *device,
               const TableData &tableData_) :
     plot_(plot), device_(device), tableData_(tableData_) {
      const int pxm = 2;

      xm_ = plot_->pixelToWindowWidth(pxm);
      xd_ = plot_->pixelToWindowWidth(plot_->indent());
    }

    // draw hier row
    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      if (! expanded_) return State::SKIP;

      //---

      // draw header
      if (data.vrow == 0)
        drawHeader();

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
      if (data.vrow == 0)
        drawHeader();

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
        const ColumnData &cdata = tableData_.rowColumnData;

        QRectF rect(x + xm_, y, cdata.drawWidth - 2*xm_, tableData_.rh);

        CQChartsTextOptions textOptions;

        CQChartsDrawUtil::drawTextInBox(device_, rect, " ", textOptions);

        x += cdata.drawWidth;
      }

      // draw column headers
      for (int ic = 0; ic < tableData_.nc; ++ic) {
        const CQChartsColumn &c = plot_->columns().getColumn(ic);

        bool ok;

        QString str = CQChartsModelUtil::modelHeaderString(model_, c, ok);
        if (! ok) continue;

        const ColumnData &cdata = tableData_.columnDataMap[c];

        CQChartsTextOptions textOptions;

        if (cdata.numeric)
          textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
        else
          textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;

        QRectF rect(x + xm_, y, cdata.drawWidth - 2*xm_, tableData_.rh);

        CQChartsDrawUtil::drawTextInBox(device_, rect, str, textOptions);

        x += cdata.drawWidth;
      }
    }

    void drawRowNumber(double x, double y, int n) {
      const ColumnData &cdata = tableData_.rowColumnData;

      QRectF rect(x + xm_, y, cdata.drawWidth - 2*xm_, tableData_.rh);

      CQChartsTextOptions textOptions;

      textOptions.align = Qt::AlignRight | Qt::AlignVCenter;

      const QString rstr = QString("%1").arg(n);

      CQChartsDrawUtil::drawTextInBox(device_, rect, rstr, textOptions);
    }

    void drawCellValues(double x, double y, const VisitData &data) {
      for (int ic = 0; ic < tableData_.nc; ++ic) {
        const CQChartsColumn &c = plot_->columns().getColumn(ic);

        const ColumnData &cdata = tableData_.columnDataMap[c];

        drawCellValue(x, y, data, ic);

        x += cdata.drawWidth;
      }
    }

    void drawCellValue(double x, double y, const VisitData &data, int ic) {
      const CQChartsColumn &c = plot_->columns().getColumn(ic);

      bool ok;

      QString str = CQChartsModelUtil::modelString(plot_->charts(), model_,
                                                   data.row, c, data.parent, ok);
      if (! ok) return;

      const ColumnData &cdata = tableData_.columnDataMap[c];

      CQChartsTextOptions textOptions;

      if (cdata.numeric)
        textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
      else
        textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;

      double x1 = x;

      if (ic == 0)
        x1 += depth_*xd_;

      QRectF rect(x1 + xm_, y, cdata.drawWidth - 2*xm_, tableData_.rh);

      CQChartsDrawUtil::drawTextInBox(device_, rect, str, textOptions);
    }

   private:
    const CQChartsTablePlot* plot_     { nullptr };
    CQChartsPaintDevice*     device_   { nullptr };
    TableData                tableData_;
    double                   xm_       { 0.0 };
    double                   xd_       { 0.0 };
    bool                     expanded_ { true };
    std::vector<int>         expandStack_;
  };

  RowVisitor visitor(this, device, tableData_);

  visitor.setPlot(this);

  visitor.init();

  if (summaryModel_)
    CQChartsModelVisit::exec(charts(), summaryModel_, visitor);
  else
    CQChartsModelVisit::exec(charts(), model().data(), visitor);
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
