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

  visitModel(visitor);

  //---

  // set full table width
  th->tableData_.pcw = 0.0;

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

  double pdx = (pixelRect.getWidth () - th->tableData_.pcw                        )/2.0;
  double pdy = (pixelRect.getHeight() - th->tableData_.prh*(th->tableData_.nr + 1))/2.0;

  double dx = pixelToSignedWindowWidth (pdx);
  double dy = pixelToSignedWindowHeight(pdy);

  th->tableData_.xo = std::max(dx, 0.0);
  th->tableData_.yo = std::max(dy, 0.0);

  //---

  device->setFont(th->tableData_.font);

  //---

  // calc column widths
  for (int i = 0; i < tableData_.nc; ++i) {
    const CQChartsColumn &c = columns().getColumn(i);

    ColumnData &data = th->tableData_.columnDataMap[c];

    data.width     = pixelToWindowWidth(data.pwidth);
    data.drawWidth = data.width;

    if (data.prefWidth > 0)
      data.drawWidth = pixelToWindowWidth(data.prefWidth + 2*pxm);

    th->tableData_.columnDataMap[c] = data;
  }

  //---

  // draw table column lines
  device->setPen(QColor(128,128,128));

  double x  = th->tableData_.xo;
  double y1 = th->tableData_.yo + (tableData_.nr + 1)*tableData_.rh;
  double y2 = th->tableData_.yo;

  for (int i = 0; i < tableData_.nc; ++i) {
    const CQChartsColumn &c = columns().getColumn(i);

    device->drawLine(QPointF(x, y1), QPointF(x, y2));

    const ColumnData &data = th->tableData_.columnDataMap[c];

    x += data.drawWidth;
  }

  device->drawLine(QPointF(x, y1), QPointF(x, y2));

  // drable row lines
  double y  = th->tableData_.yo;
  double x1 = th->tableData_.xo;
  double x2 = x;

  for (int i = 0; i < tableData_.nr + 1; ++i) {
    device->drawLine(QPointF(x1, y), QPointF(x2, y));

    y += tableData_.rh;
  }

  device->drawLine(QPointF(x1, y), QPointF(x2, y));

  //---

  device->setPen(QColor(0,0,0));

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsTablePlot *plot, CQChartsPaintDevice *device,
               const TableData &tableData_) :
     plot_(plot), device_(device), tableData_(tableData_) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      int pxm = 2;

      double xm = plot_->pixelToWindowWidth(pxm);

      if (data.row == 0) {
        double x = tableData_.xo;
        double y = tableData_.yo + tableData_.nr*tableData_.rh;

        for (int i = 0; i < tableData_.nc; ++i) {
          const CQChartsColumn &c = plot_->columns().getColumn(i);

          bool ok;

          QString str = CQChartsModelUtil::modelHeaderString(plot_->summaryModel(), c, ok);
          if (! ok) continue;

          const ColumnData &data = tableData_.columnDataMap[c];

          CQChartsTextOptions textOptions;

          if (data.numeric)
            textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
          else
            textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;

          QRectF rect(x + xm, y, data.drawWidth - 2*xm, tableData_.rh);

          CQChartsDrawUtil::drawTextInBox(device_, rect, str, textOptions);

          x += data.drawWidth;
        }
      }

      double x = tableData_.xo;
      double y = tableData_.yo + (tableData_.nr - data.row - 1)*tableData_.rh;

      for (int i = 0; i < tableData_.nc; ++i) {
        const CQChartsColumn &c = plot_->columns().getColumn(i);

        bool ok;

        QString str = CQChartsModelUtil::modelString(plot_->charts(), plot_->summaryModel(),
                                                     data.row, c, data.parent, ok);
        if (! ok) continue;

        const ColumnData &data = tableData_.columnDataMap[c];

        CQChartsTextOptions textOptions;

        if (data.numeric)
          textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
        else
          textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;

        QRectF rect(x + xm, y, data.drawWidth - 2*xm, tableData_.rh);

        CQChartsDrawUtil::drawTextInBox(device_, rect, str, textOptions);

        x += data.drawWidth;
      }

      return State::OK;
    }

   private:
    const CQChartsTablePlot* plot_   { nullptr };
    CQChartsPaintDevice*     device_ { nullptr };
    TableData                tableData_;
  };

  RowVisitor visitor(this, device, tableData_);

  visitModel(visitor);
}
