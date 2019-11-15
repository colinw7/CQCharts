#include <CQChartsCorrelationPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsLoader.h>
#include <CQChartsFilterModel.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsTip.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

CQChartsCorrelationPlotType::
CQChartsCorrelationPlotType()
{
}

void
CQChartsCorrelationPlotType::
addParameters()
{
  CQChartsPlotType::addParameters();
}

QString
CQChartsCorrelationPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Correlation Plot").
    h3("Summary").
     p("Draws correlation data for model.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/correlation.png"));
}

CQChartsPlot *
CQChartsCorrelationPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsCorrelationPlot(view, model);
}

//------

CQChartsCorrelationPlot::
CQChartsCorrelationPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("correlation"), model),
 CQChartsObjCellShapeData    <CQChartsCorrelationPlot>(this),
 CQChartsObjCellLabelTextData<CQChartsCorrelationPlot>(this),
 CQChartsObjXLabelTextData   <CQChartsCorrelationPlot>(this),
 CQChartsObjYLabelTextData   <CQChartsCorrelationPlot>(this)
{
  NoUpdate noUpdate(this);

  //---

  // create correlation model
  CQChartsLoader loader(charts());

  correlationModel_ = loader.createCorrelationModel(model.data(), /*flip*/false);

  nc_ = model.data()->columnCount();

  //---

  addTitle();

  setCellFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setCellLabelTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);
}

CQChartsCorrelationPlot::
~CQChartsCorrelationPlot()
{
  delete correlationModel_;
}

//---

void
CQChartsCorrelationPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  // cell fill
  addProp("cell/fill", "cellFilled", "visible", "Cell fill visible");
  addFillProperties("cell/fill", "cellFill", "Cell");

  // cell stroke
  addProp("cell/stroke", "cellStroked", "visible", "Cell stroke visible");
  addLineProperties("cell/stroke", "cellStroke", "Cell");

  // cell label text
//addProp("cell/text", "cellLabels", "visible", "Cell text label visible");
  addAllTextProperties("cell/text", "cellLabelText", "Cell label");

  // x/y axis label text
//addProp("xaxis/text", "xLabels", "visible", "X labels visible");
  addTextProperties("xaxis/text", "xLabelText", "X label");

//addProp("yaxis/text", "yLabels", "visible", "Y labels visible");
  addTextProperties("yaxis/text", "yLabelText", "Y label");
}

//---

CQChartsGeom::Range
CQChartsCorrelationPlot::
calcRange() const
{
  CQChartsGeom::Range dataRange;

  int nc = std::max(correlationModel_->columnCount(), 1);

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(nc , nc );

  return dataRange;
}

//---

bool
CQChartsCorrelationPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsCorrelationPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  class RowVisitor : public CQModelVisitor {
   public:
    RowVisitor(const CQChartsCorrelationPlot *plot, PlotObjs &objs) :
     plot_(plot), objs_(objs) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      x_ = 0.0;

      for (int col = 0; col < numCols(); ++col) {
        QModelIndex ind = model->index(data.row, col, data.parent);

        bool ok;

        double value = CQChartsModelUtil::modelReal(model, ind, ok);

        //---

        plot_->addCellObj(data.row, col, x_, y_, dx_, dy_, value, ind, objs_);

        //---

        x_ += dx_;
      }

      y_ += dy_;

      return State::OK;
    }

   private:
    const CQChartsCorrelationPlot* plot_ { nullptr };
    PlotObjs&                      objs_;
    double                         x_    { 0.0 };
    double                         y_    { 0.0 };
    double                         dx_   { 1.0 };
    double                         dy_   { 1.0 };
  };

  RowVisitor visitor(this, objs);

  CQModelVisit::exec(correlationModel_, visitor);

  //---

  return true;
}

void
CQChartsCorrelationPlot::
addCellObj(int row, int col, double x, double y, double dx, double dy, double value,
           const QModelIndex &ind, PlotObjs &objs) const
{
  CQChartsGeom::BBox bbox(x, y, x + dx, y + dy);

  ColorInd colorInd(value);

  CQChartsCellObj *cellObj = new CQChartsCellObj(this, bbox, row, col, value, ind, colorInd);

  objs.push_back(cellObj);
}

//---

bool
CQChartsCorrelationPlot::
addMenuItems(QMenu *)
{
  return false;
}

//------

bool
CQChartsCorrelationPlot::
hasForeground() const
{
  return isLayerActive(CQChartsLayer::Type::FOREGROUND);
}

void
CQChartsCorrelationPlot::
execDrawForeground(CQChartsPaintDevice *device) const
{
  drawXLabels(device);
  drawYLabels(device);
}

void
CQChartsCorrelationPlot::
drawXLabels(CQChartsPaintDevice *device) const
{
  view()->setPlotPainterFont(this, device, xLabelTextFont());

  //---

  QPen tpen;

  QColor tc = interpXLabelTextColor(ColorInd());

  setPen(tpen, true, tc, xLabelTextAlpha());

  device->setPen(tpen);

  //---

  CQChartsTextOptions textOptions;

  textOptions.contrast      = isXLabelTextContrast();
  textOptions.contrastAlpha = xLabelTextContrastAlpha();
  textOptions.formatted     = isXLabelTextFormatted();
  textOptions.scaled        = isXLabelTextScaled();
  textOptions.html          = isXLabelTextHtml();
  textOptions.align         = xLabelTextAlign();
  textOptions.angle         = xLabelTextAngle();
  textOptions.scaled        = isXLabelTextScaled();

  textOptions = adjustTextOptions(textOptions);

  //---

  using ColRects = std::map<int,QRectF>;

  ColRects colRects;

  double tw = 0.0;
  double th = 0.0;

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    QRectF trect = CQChartsRotatedText::bbox(0.0, 0.0, name, device->font(),
                                             textOptions.angle, 0, textOptions.align,
                                             /*alignBBox*/ true);

    colRects[col] = trect;

    tw = std::max(tw, trect.width ());
    th = std::max(th, trect.height());
  }

  //---

  double tm = 4;

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    QPointF p(col + 0.5, 0);

    QPointF p1 = windowToPixel(p);

    QRectF trect = colRects[col];

    double tw1 = trect.width();

    QRectF trect1;

    if (! isInvertY())
      trect1 = QRectF(p1.x() - tw1/2, p1.y() + tm, tw1, th);
    else
      trect1 = QRectF(p1.x() - tw1/2, p1.y() - th - tm, tw1, th);

    CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(trect1), name, textOptions);
  }
}

void
CQChartsCorrelationPlot::
drawYLabels(CQChartsPaintDevice *device) const
{
  view()->setPlotPainterFont(this, device, yLabelTextFont());

  //---

  QPen tpen;

  QColor tc = interpYLabelTextColor(ColorInd());

  setPen(tpen, true, tc, yLabelTextAlpha());

  device->setPen(tpen);

  //---

  CQChartsTextOptions textOptions;

  textOptions.contrast      = isYLabelTextContrast();
  textOptions.contrastAlpha = yLabelTextContrastAlpha();
  textOptions.formatted     = isYLabelTextFormatted();
  textOptions.scaled        = isYLabelTextScaled();
  textOptions.html          = isYLabelTextHtml();
  textOptions.align         = yLabelTextAlign();
  textOptions.angle         = yLabelTextAngle();
  textOptions.scaled        = isYLabelTextScaled();

  textOptions = adjustTextOptions(textOptions);

  //---

  using RowRects = std::map<int,QRectF>;

  RowRects colRects;

  double tw = 0.0;
  double th = 0.0;

  //---

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    QRectF trect = CQChartsRotatedText::bbox(0.0, 0.0, name, device->font(),
                                             textOptions.angle, 0, textOptions.align,
                                             /*alignBBox*/ true);

    colRects[col] = trect;

    tw = std::max(tw, trect.width ());
    th = std::max(th, trect.height());
  }

  //---

  double tm = 4;

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    QPointF p(0, col + 0.5);

    QPointF p1 = windowToPixel(p);

    QRectF trect = colRects[col];

    double th1 = trect.height();

    QRectF trect1;

    if (! isInvertX())
      trect1 = QRectF(p1.x() - tw - tm, p1.y() - th1/2.0, tw, th1);
    else
      trect1 = QRectF(p1.x() + tm, p1.y() - th1/2.0, tw, th1);

    CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(trect1), name, textOptions);
  }
}

//------

CQChartsGeom::BBox
CQChartsCorrelationPlot::
annotationBBox() const
{
  QFont font = view()->plotFont(this, cellLabelTextFont());

  QFontMetricsF fm(font);

  CQChartsGeom::BBox bbox;

  double tm = 4;

  {
    double tw = 0.0;

    for (int col = 0; col < numColumns(); ++col) {
      bool ok;

      QString name = modelHeaderString(col, Qt::Horizontal, ok);
      if (! name.length()) continue;

      tw = std::max(tw, fm.width(name));
    }

    double tw1 = pixelToWindowHeight(tw + tm);

    CQChartsGeom::BBox tbbox(0, -tw1, numColumns(), 0);

    bbox += tbbox;
  }

  {
    double tw = 0.0;

    for (int row = 0; row < numColumns(); ++row) {
      bool ok;

      QString name = modelHeaderString(row, Qt::Vertical, ok);
      if (! name.length()) continue;

      tw = std::max(tw, fm.width(name));
    }

    double tw1 = pixelToWindowWidth(tw + tm);

    CQChartsGeom::BBox tbbox(-tw1, 0, 0, numColumns());

    bbox += tbbox;
  }

  return bbox;
}

//------

CQChartsCellObj::
CQChartsCellObj(const CQChartsCorrelationPlot *plot, const CQChartsGeom::BBox &rect,
                 int row, int col, double value, const QModelIndex &ind, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsCorrelationPlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), row_(row), col_(col), value_(value)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(ind);
}

QString
CQChartsCellObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(row_).arg(col_);
}

QString
CQChartsCellObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  bool ok;

  CQChartsFilterModel *model = plot_->correlationModel();

  QString xname = plot_->modelHeaderString(row_, Qt::Horizontal, ok);
  QString yname = plot_->modelHeaderString(col_, Qt::Horizontal, ok);

  if (xname.length())
    tableTip.addTableRow("X", xname);

  if (yname.length())
    tableTip.addTableRow("Y", yname);

  tableTip.addTableRow("Value", value());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

void
CQChartsCellObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, CQChartsColumn(modelInd().column()));
}

void
CQChartsCellObj::
draw(CQChartsPaintDevice *device)
{
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  QRectF qrect = rect().qrect();

  device->drawRect(qrect);

  //---

  // calc pen and brush
  ColorInd ic(value());

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, device, plot_->cellLabelTextFont());

  //---

  // set pen
  CQChartsPenBrush tPenBrush;

  QColor tc = plot_->interpCellLabelTextColor(ic);

  plot_->setPen(tPenBrush.pen, true, tc, plot_->cellLabelTextAlpha());

  plot_->updateObjPenBrushState(this, tPenBrush);

  device->setPen(tPenBrush.pen);

  //---

  QString valueStr = CQChartsUtil::formatReal(value());

  CQChartsTextOptions textOptions;

  textOptions.contrast      = plot_->isCellLabelTextContrast();
  textOptions.contrastAlpha = plot_->cellLabelTextContrastAlpha();
  textOptions.formatted     = plot_->isCellLabelTextFormatted();
  textOptions.scaled        = plot_->isCellLabelTextScaled();
  textOptions.html          = plot_->isCellLabelTextHtml();
  textOptions.align         = plot_->cellLabelTextAlign();
  textOptions.scaled        = plot_->isCellLabelTextScaled();

  textOptions = plot_->adjustTextOptions(textOptions);

  CQChartsDrawUtil::drawTextInBox(device, qrect, valueStr, textOptions);

  //---

  device->resetColorNames();
}

void
CQChartsCellObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // calc pen and brush
  ColorInd ic(value());

  //---

  // set pen and brush
  QColor fc = plot_->interpCellFillColor  (ic);
  QColor bc = plot_->interpCellStrokeColor(ic);

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (plot_->isCellStroked(), bc, plot_->cellStrokeAlpha(),
                      plot_->cellStrokeWidth(), plot_->cellStrokeDash()),
    CQChartsBrushData(plot_->isCellFilled(), fc, plot_->cellFillAlpha(), plot_->cellFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsCellObj::
writeScriptData(CQChartsScriptPainter *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.value = " << value() << ";\n";
}

double
CQChartsCellObj::
xColorValue(bool) const
{
  return col_;
}

double
CQChartsCellObj::
yColorValue(bool) const
{
  return col_;
}
