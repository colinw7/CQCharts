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

  setXLabelTextAlign(Qt::AlignHCenter | Qt::AlignTop    ); setXLabelTextAngle(90);
  setYLabelTextAlign(Qt::AlignRight   | Qt::AlignVCenter); setYLabelTextAngle( 0);

  setXLabelTextFont(CQChartsFont().decFontSize(4));
  setYLabelTextFont(CQChartsFont().decFontSize(4));
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
  addProp("cell/text", "cellLabels", "visible", "Cell text label visible");
  addTextProperties("cell/text", "cellLabelText", "Cell label",
                    CQChartsTextOptions::ValueType::ALL);

  // x/y axis label text
  addProp("xaxis/text", "xLabels", "visible", "X labels visible");
  addTextProperties("xaxis/text", "xLabelText", "X label",
                    CQChartsTextOptions::ValueType::ALL);

  addProp("yaxis/text", "yLabels", "visible", "Y labels visible");
  addTextProperties("yaxis/text", "yLabelText", "Y label",
                    CQChartsTextOptions::ValueType::ALL);
}

//---

void
CQChartsCorrelationPlot::
setXLabels(bool b)
{
  CQChartsUtil::testAndSet(xLabels_, b, [&]() { drawObjs(); } );
}

void
CQChartsCorrelationPlot::
setYLabels(bool b)
{
  CQChartsUtil::testAndSet(yLabels_, b, [&]() { drawObjs(); } );
}

void
CQChartsCorrelationPlot::
setCellLabels(bool b)
{
  CQChartsUtil::testAndSet(cellLabels_, b, [&]() { drawObjs(); } );
}

//---

CQChartsGeom::Range
CQChartsCorrelationPlot::
calcRange() const
{
  CQChartsGeom::Range dataRange;

  // square (nc, nc)
  int nc = std::max(correlationModel_->columnCount(), 1);

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(nc , nc );

  //---

  return dataRange;
}

//---

void
CQChartsCorrelationPlot::
preDrawObjs(CQChartsPaintDevice *device) const
{
  if (isCellLabelTextScaled()) {
    bool first = true;

    double maxWidth = 0.0, maxHeight = 0.0;

    QSizeF cellSize;

    for (auto &plotObj : plotObjects()) {
      CQChartsCellObj *cellObj = dynamic_cast<CQChartsCellObj *>(plotObj);
      if (! cellObj) continue;

      QSizeF s = cellObj->calcTextSize();

      maxWidth  = std::max(maxWidth , s.width ());
      maxHeight = std::max(maxHeight, s.height());

      if (first) {
        cellSize = cellObj->rect().qrect().size();
        first    = false;
      }
    }

    double cw = windowToPixelWidth (cellSize.width ());
    double ch = windowToPixelHeight(cellSize.height());

    double xs = (maxWidth  > 0.0 ? cw/maxWidth  : 1.0);
    double ys = (maxHeight > 0.0 ? ch/maxHeight : 1.0);

    if (xs > 1.0 && ys > 1.0)
      labelScale_ = std::max(xs, ys);
    else
      labelScale_ = std::min(xs, ys);
  }
  else {
    labelScale_ = 1.0;
  }

  //---

  CQChartsPlot::preDrawObjs(device);
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
probe(ProbeData &probeData) const
{
  CQChartsPlotObj *obj;

  if (! objNearestPoint(probeData.p, obj))
    return false;

  CQChartsGeom::Point c = obj->rect().getCenter();

  probeData.p    = c;
  probeData.both = true;

  probeData.xvals.push_back(c.x);
  probeData.yvals.push_back(c.y);

  return true;
}

//---

bool
CQChartsCorrelationPlot::
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

  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) -> QAction *{
    return addMenuCheckedAction(menu, name, isSet, slot);
  };

  //---

  menu->addSeparator();

  (void) addCheckedAction("Cell Labels", isCellLabels(), SLOT(setCellLabels(bool)));
  (void) addCheckedAction("X Labels"   , isXLabels   (), SLOT(setXLabels   (bool)));
  (void) addCheckedAction("Y Labels"   , isYLabels   (), SLOT(setYLabels   (bool)));

  return true;
}

//------

bool
CQChartsCorrelationPlot::
hasForeground() const
{
  if (! isXLabels() && ! isYLabels())
    return false;

  return isLayerActive(CQChartsLayer::Type::FOREGROUND);
}

void
CQChartsCorrelationPlot::
execDrawForeground(CQChartsPaintDevice *device) const
{
  if (isXLabels())
    drawXLabels(device);

  if (isYLabels())
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
  textOptions.clipped       = false;

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

    QRectF trect = CQChartsRotatedText::calcBBox(0.0, 0.0, name, device->font(),
                                                 textOptions, 0, /*alignBBox*/ true);

    colRects[col] = trect;

    tw = std::max(tw, trect.width ());
    th = std::max(th, trect.height());
  }

  //---

  double cw = windowToPixelWidth(1.0);

  double tm = 4;

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    QPointF p(col + 0.5, 0);

    QPointF p1 = windowToPixel(p);

    QRectF trect = colRects[col];

    double tw1 = std::max(trect.width(), cw);

    QRectF trect1;

    if (! isInvertY())
      trect1 = QRectF(p1.x() - tw1/2.0, p1.y() + tm, tw1, th);
    else
      trect1 = QRectF(p1.x() - tw1/2.0, p1.y() - th - tm, tw1, th);

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
  textOptions.clipped       = false;

  textOptions = adjustTextOptions(textOptions);

  //---

  using RowRects = std::map<int,QRectF>;

  RowRects colRects;

  double tw = 0.0;
  double th = 0.0;

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    QRectF trect = CQChartsRotatedText::calcBBox(0.0, 0.0, name, device->font(),
                                                 textOptions, 0, /*alignBBox*/ true);

    colRects[col] = trect;

    tw = std::max(tw, trect.width ());
    th = std::max(th, trect.height());
  }

  //---

  double ch = windowToPixelHeight(1.0);

  double tm = 4;

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    QPointF p(0, col + 0.5);

    QPointF p1 = windowToPixel(p);

    QRectF trect = colRects[col];

    double th1 = std::max(trect.height(), ch);

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
calcAnnotationBBox() const
{
  CQPerfTrace trace("CQChartsCorrelationPlot::annotationBBox");

  QFont xfont = view()->plotFont(this, xLabelTextFont());
  QFont yfont = view()->plotFont(this, yLabelTextFont());

  CQChartsGeom::BBox bbox;

  double tm = 4;

  //---

  if (isXLabels()) {
    double th = 0.0;

    for (int col = 0; col < numColumns(); ++col) {
      bool ok;

      QString name = modelHeaderString(col, Qt::Horizontal, ok);
      if (! name.length()) continue;

      CQChartsTextOptions options;

      options.angle = xLabelTextAngle();
      options.align = xLabelTextAlign();

      QRectF trect = CQChartsRotatedText::calcBBox(0.0, 0.0, name, xfont,
                                                   options, 0, /*alignBBox*/ true);

      th = std::max(th, trect.height());
    }

    double th1 = pixelToWindowHeight(th + tm);

    CQChartsGeom::BBox tbbox(0, -th1, numColumns(), 0);

    bbox += tbbox;
  }

  //---

  if (isYLabels()) {
    double tw = 0.0;

    for (int row = 0; row < numColumns(); ++row) {
      bool ok;

      QString name = modelHeaderString(row, Qt::Horizontal, ok);
      if (! name.length()) continue;

      CQChartsTextOptions options;

      options.angle = yLabelTextAngle();
      options.align = yLabelTextAlign();

      QRectF trect = CQChartsRotatedText::calcBBox(0.0, 0.0, name, yfont,
                                                   options, 0, /*alignBBox*/ true);

      tw = std::max(tw, trect.width());
    }

    double tw1 = pixelToWindowWidth(tw + tm);

    CQChartsGeom::BBox tbbox(-tw1, 0, 0, numColumns());

    bbox += tbbox;
  }

  //---

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

QSizeF
CQChartsCellObj::
calcTextSize() const
{
  if (! plot_->isCellLabels())
    return QSizeF();

  QFont font = plot_->view()->plotFont(plot_, plot_->cellLabelTextFont());

  QString valueStr = CQChartsUtil::formatReal(value());

  CQChartsTextOptions options;

  options.contrast  = plot_->isCellLabelTextContrast();
  options.formatted = plot_->isCellLabelTextFormatted();
  options.html      = plot_->isCellLabelTextHtml();
  options.angle     = plot_->cellLabelTextAngle();

  return CQChartsDrawUtil::calcTextSize(valueStr, font, options);
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

  if (plot_->isCellLabels()) {
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
    textOptions.scale         = plot_->labelScale(); // TODO: optional
    textOptions.html          = plot_->isCellLabelTextHtml();
    textOptions.align         = plot_->cellLabelTextAlign();
    textOptions.angle         = plot_->cellLabelTextAngle();

    textOptions = plot_->adjustTextOptions(textOptions);

    CQChartsDrawUtil::drawTextInBox(device, qrect, valueStr, textOptions);
  }

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
