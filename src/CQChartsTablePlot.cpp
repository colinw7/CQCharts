#include <CQChartsTablePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CMathRound.h>

#include <QMenu>

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

  summaryModel_ = new CQSummaryModel(model.data());

  setMaxRows (60);
  setPageSize(60);

  setGridColor  (QColor(128,128,128));
  setTextColor  (QColor(0,0,0));
  setHeaderColor(QColor(150,150,200));
  setCellColor  (QColor(255,255,255));

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
  return (CQChartsTablePlot::Mode) summaryModel_->mode();
}

void
CQChartsTablePlot::
setMode(const Mode &m)
{
  if (m != mode()) {
    summaryModel_->setMode((CQSummaryModel::Mode) m);

    updateRangeAndObjs();
  }
}

//---

int
CQChartsTablePlot::
maxRows() const
{
  return summaryModel_->maxRows();
}

void
CQChartsTablePlot::
setMaxRows(int i)
{
  if (i != maxRows()) {
    summaryModel_->setMaxRows(i);

    updateRangeAndObjs();
  }
}

//---

int
CQChartsTablePlot::
sortColumn() const
{
  return summaryModel_->sortColumn();
}

void
CQChartsTablePlot::
setSortColumn(int i)
{
  if (i != sortColumn()) {
    summaryModel_->setSortColumn(i);

    updateRangeAndObjs();
  }
}

int
CQChartsTablePlot::
sortRole() const
{
  return summaryModel_->sortRole();
}

void
CQChartsTablePlot::
setSortRole(int r)
{
  if (r != sortRole()) {
    summaryModel_->setSortRole(r);

    updateRangeAndObjs();
  }
}

Qt::SortOrder
CQChartsTablePlot::
sortOrder() const
{
  return summaryModel_->sortOrder();
}

void
CQChartsTablePlot::
setSortOrder(Qt::SortOrder r)
{
  if (r != sortOrder()) {
    summaryModel_->setSortOrder(r);

    updateRangeAndObjs();
  }
}

//---

int
CQChartsTablePlot::
pageSize() const
{
  return summaryModel_->pageSize();
}

void
CQChartsTablePlot::
setPageSize(int i)
{
  if (i != pageSize()) {
    summaryModel_->setPageSize(i);

    updateRangeAndObjs();
  }
}

int
CQChartsTablePlot::
currentPage() const
{
  return summaryModel_->currentPage();
}

void
CQChartsTablePlot::
setCurrentPage(int i)
{
  if (i != currentPage()) {
    summaryModel_->setCurrentPage(i);

    updateRangeAndObjs();
  }
}

//---

const CQChartsTablePlot::RowNums &
CQChartsTablePlot::
rowNums() const
{
  return summaryModel_->rowNums();
}

void
CQChartsTablePlot::
setRowNums(const RowNums &rowNums)
{
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
  QStringList strs = str.split(" ", QString::SkipEmptyParts);

  RowNums rowNums;

  for (const auto &s : strs) {
    bool ok;

    int i = s.toInt(&ok);

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

  addProp("options", "gridColor"  , "gridColor"  , "Grid color" );
  addProp("options", "textColor"  , "textColor"  , "Text color" );
  addProp("options", "headerColor", "headerColor", "Header color" );
  addProp("options", "cellColor"  , "cellColor"  , "Cell color" );
}

CQChartsGeom::Range
CQChartsTablePlot::
calcRange() const
{
  CQChartsTablePlot *th = const_cast<CQChartsTablePlot *>(this);

  int pxm = 2;

  //---

  th->tableData_.font = view()->viewFont(this->font());

  QFontMetricsF fm(th->tableData_.font);

  th->tableData_.nc  = columns_.count();
  th->tableData_.nr  = summaryModel_->rowCount();
  th->tableData_.prh = fm.height() + 2*pxm;

  // calc column widths
  if (isRowColumn()) {
    ColumnData &data = th->tableData_.rowColumnData;

    int power = CMathRound::RoundUp(log10(tableData_.nr));

    data.pwidth  = power*fm.width("X") + 2*pxm;
    data.numeric = false;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const CQChartsColumn &c = columns().getColumn(i);

    CQChartsModelColumnDetails *columnDetails = this->columnDetails(c);

    ColumnData &data = th->tableData_.columnDataMap[c];

    bool ok;

    QString str = CQChartsModelUtil::modelHeaderString(summaryModel(), c, ok);
    if (! ok) continue;

    data.pwidth  = fm.width(str) + 2*pxm;
    data.numeric = columnDetails->isNumeric();

    data.prefWidth = columnDetails->preferredWidth();
  }

  //---

  // update column widths
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsTablePlot *plot, TableData &tableData) :
     plot_(plot), tableData_(tableData), fm_(tableData_.font) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      int pxm = 2;

      for (int i = 0; i < tableData_.nc; ++i) {
        const CQChartsColumn &c = plot_->columns().getColumn(i);

        bool ok;

        QString str = CQChartsModelUtil::modelString(plot_->charts(), plot_->summaryModel(),
                                                     data.row, c, data.parent, ok);
        if (! ok) continue;

        ColumnData &data = tableData_.columnDataMap[c];

        data.pwidth = std::max(data.pwidth, fm_.width(str) + 2*pxm);
      }

      return State::OK;
    }

   private:
    const CQChartsTablePlot* plot_   { nullptr };
    TableData&               tableData_;
    QFontMetricsF            fm_;
  };

  RowVisitor visitor(this, th->tableData_);

  visitor.setPlot(this);

  visitor.init();

  CQChartsModelVisit::exec(charts(), summaryModel_, visitor);

  //---

  // set full table width
  th->tableData_.pcw = 0.0;
  th->tableData_.rcw = 0.0;

  if (isRowColumn()) {
    ColumnData &data = th->tableData_.rowColumnData;

    th->tableData_.pcw += data.pwidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const CQChartsColumn &c = columns().getColumn(i);

    ColumnData &data = th->tableData_.columnDataMap[c];

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
addMenuItems(QMenu *)
{
  return true;
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

  int pxm = 2;

  th->tableData_.rh = pixelToWindowHeight(th->tableData_.prh);

  CQChartsGeom::BBox pixelRect = this->calcPlotPixelRect();

  double pdx = (pixelRect.getWidth () - 2*pxm - th->tableData_.pcw                        )/2.0;
  double pdy = (pixelRect.getHeight() - 2*pxm - th->tableData_.prh*(th->tableData_.nr + 1))/2.0;

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

  double x1 = th->tableData_.xo;                                     // left
  double y1 = th->tableData_.yo + (tableData_.nr + 1)*tableData_.rh; // top
  double x2 = x;                                                     // right
  double y2 = th->tableData_.yo;                                     // bottom

  //---

  // draw header background
  QBrush headerBrush;

  setBrush(headerBrush, true, interpColor(headerColor(), ColorInd()), 1.0, CQChartsFillPattern());

  device->setBrush(headerBrush);

  device->fillRect(QRectF(x1, y1 - th->tableData_.rh, x2 - x1, th->tableData_.rh), device->brush());

  // draw cells background
  QBrush cellBrush;

  setBrush(cellBrush, true, interpColor(cellColor(), ColorInd()), 1.0, CQChartsFillPattern());

  device->setBrush(cellBrush);

  device->fillRect(QRectF(x1, y2, x2 - x1, tableData_.nr*th->tableData_.rh), device->brush());

  //---

  // draw table column lines
  QPen gridPen;

  setPen(gridPen, true, interpColor(gridColor(), ColorInd()), 1.0, 0.0, CQChartsLineDash());

  device->setPen(gridPen);

  x = x1;

  if (isRowColumn()) {
    const ColumnData &data = th->tableData_.rowColumnData;

    device->drawLine(QPointF(x, y1), QPointF(x, y2));

    x += data.drawWidth;
  }

  for (int i = 0; i < tableData_.nc; ++i) {
    const CQChartsColumn &c = columns().getColumn(i);

    const ColumnData &data = th->tableData_.columnDataMap[c];

    device->drawLine(QPointF(x, y1), QPointF(x, y2));

    x += data.drawWidth;
  }

  device->drawLine(QPointF(x, y1), QPointF(x, y2));

  // draw row lines
  double y = th->tableData_.yo;

  for (int i = 0; i < tableData_.nr + 1; ++i) {
    device->drawLine(QPointF(x1, y), QPointF(x2, y));

    y += tableData_.rh;
  }

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
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      int pxm = 2;

      double xm = plot_->pixelToWindowWidth(pxm);

      // draw header
      if (data.row == 0) {
        double x = tableData_.xo;
        double y = tableData_.yo + tableData_.nr*tableData_.rh;

        if (plot_->isRowColumn()) {
          const ColumnData &cdata = tableData_.rowColumnData;

          QRectF rect(x + xm, y, cdata.drawWidth - 2*xm, tableData_.rh);

          CQChartsTextOptions textOptions;

          CQChartsDrawUtil::drawTextInBox(device_, rect, " ", textOptions);

          x += cdata.drawWidth;
        }

        for (int i = 0; i < tableData_.nc; ++i) {
          const CQChartsColumn &c = plot_->columns().getColumn(i);

          bool ok;

          QString str = CQChartsModelUtil::modelHeaderString(plot_->summaryModel(), c, ok);
          if (! ok) continue;

          const ColumnData &cdata = tableData_.columnDataMap[c];

          CQChartsTextOptions textOptions;

          if (cdata.numeric)
            textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
          else
            textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;

          QRectF rect(x + xm, y, cdata.drawWidth - 2*xm, tableData_.rh);

          CQChartsDrawUtil::drawTextInBox(device_, rect, str, textOptions);

          x += cdata.drawWidth;
        }
      }

      //---

      // draw row
      double x = tableData_.xo;
      double y = tableData_.yo + (tableData_.nr - data.row - 1)*tableData_.rh;

      if (plot_->isRowColumn()) {
        const ColumnData &cdata = tableData_.rowColumnData;

        QRectF rect(x + xm, y, cdata.drawWidth - 2*xm, tableData_.rh);

        CQChartsTextOptions textOptions;

        textOptions.align = Qt::AlignRight | Qt::AlignVCenter;

        QString rstr = QString("%1").arg(data.row + 1);

        CQChartsDrawUtil::drawTextInBox(device_, rect, rstr, textOptions);

        x += cdata.drawWidth;
      }

      for (int i = 0; i < tableData_.nc; ++i) {
        const CQChartsColumn &c = plot_->columns().getColumn(i);

        bool ok;

        QString str = CQChartsModelUtil::modelString(plot_->charts(), plot_->summaryModel(),
                                                     data.row, c, data.parent, ok);
        if (! ok) continue;

        const ColumnData &cdata = tableData_.columnDataMap[c];

        CQChartsTextOptions textOptions;

        if (cdata.numeric)
          textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
        else
          textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;

        QRectF rect(x + xm, y, cdata.drawWidth - 2*xm, tableData_.rh);

        CQChartsDrawUtil::drawTextInBox(device_, rect, str, textOptions);

        x += cdata.drawWidth;
      }

      return State::OK;
    }

   private:
    const CQChartsTablePlot* plot_   { nullptr };
    CQChartsPaintDevice*     device_ { nullptr };
    TableData                tableData_;
  };

  RowVisitor visitor(this, device, tableData_);

  visitor.setPlot(this);

  visitor.init();

  CQChartsModelVisit::exec(charts(), summaryModel_, visitor);
}

void
CQChartsTablePlot::
adjustPan()
{
  if (tableData_.dx < 0) {
    double dx = -tableData_.dx;

    if (dataOffsetX() < 0)
      setDataOffsetX(0.0);

    if (dataOffsetX() > 2*dx)
      setDataOffsetX(2*dx);
  }
  else
    setDataOffsetX(0.0);

  if (tableData_.dy < 0) {
    double dy = -tableData_.dy;

    if (dataOffsetY() < 0)
      setDataOffsetY(0.0);

    if (dataOffsetY() > 2*dy)
      setDataOffsetY(2*dy);
  }
  else
    setDataOffsetY(0.0);
}
