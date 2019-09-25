#include <CQChartsImagePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTip.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

CQChartsImagePlotType::
CQChartsImagePlotType()
{
}

void
CQChartsImagePlotType::
addParameters()
{
  CQChartsPlotType::addParameters();
}

QString
CQChartsImagePlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Image Plot").
    h3("Summary").
     p("Draw 2d grid of 'pixels' from values in 2d table.").
     p("The pixels can be drawn as cells or scaled circles (balloon).").
     p("The cells are colored using the scaled value.").
    h3("Columns").
     p("All row column values of the model are used for the pixels of the image.").
     p("The vertical and horizontal headers are used for the row and column labels respetively.").
    h3("Options").
     p("To scale the colors and circle sizes for the cell the user can supply a maximum and/or "
       "minimum value for the values in the value column. If not specifed the compured minimum "
       "and maximum will be used").
     p("X and/or Y labels can be added to the outside of the grid.").
     p("Labels can be added to each grid cell and the labels can be scaled "
       "to represent the size of the associated value.").
    h3("Limitations").
     p("Does not support axes.").
    h3("Example").
     p(IMG("images/imageplot.png"));
}

CQChartsPlot *
CQChartsImagePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsImagePlot(view, model);
}

//------

CQChartsImagePlot::
CQChartsImagePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("image"), model),
 CQChartsObjCellShapeData    <CQChartsImagePlot>(this),
 CQChartsObjCellLabelTextData<CQChartsImagePlot>(this),
 CQChartsObjXLabelTextData   <CQChartsImagePlot>(this),
 CQChartsObjYLabelTextData   <CQChartsImagePlot>(this)
{
  NoUpdate noUpdate(this);

  addTitle();

  setCellFillColor     (CQChartsColor(CQChartsColor::Type::PALETTE));
  setCellStroked       (false);
  setCellLabelTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);
  setXLabelTextAlign   (Qt::AlignHCenter | Qt::AlignVCenter);
  setYLabelTextAlign   (Qt::AlignHCenter | Qt::AlignVCenter);
}

CQChartsImagePlot::
~CQChartsImagePlot()
{
}

void
CQChartsImagePlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  // cell style
  addProp("cell", "cellStyle", "style", "Cell style (rect or balloon)");

  // cell labels
//addProp("cell/labels", "scaleCellLabels", "scaled" , "Scale cell labels");

  // cell fill
  addProp("cell/fill", "cellFilled", "visible", "Cell fill visible");

  addFillProperties("cell/fill", "cellFill", "Cell");

  // cell stroke
  addProp("cell/stroke", "cellStroked", "visible", "Cell stroke visible");

  addLineProperties("cell/stroke", "cellStroke", "Cell");

  // cell label text
  addProp("cell/text", "cellLabels", "visible", "Cell text label visible");

  addAllTextProperties("cell/text", "cellLabelText", "Cell label");

  // x/y axis label text
  addProp("xaxis/text", "xLabels", "visible", "X labels visible");
  addTextProperties("xaxis/text", "xLabelText", "X label");

  addProp("yaxis/text", "yLabels", "visible", "Y labels visible");
  addTextProperties("yaxis/text", "yLabelText", "Y label");
}

//------

void
CQChartsImagePlot::
setMinValue(double r)
{
  CQChartsUtil::testAndSet(minValue_, r, [&]() { drawObjs(); } );
}

void
CQChartsImagePlot::
setMaxValue(double r)
{
  CQChartsUtil::testAndSet(maxValue_, r, [&]() { drawObjs(); } );
}

//---

void
CQChartsImagePlot::
setXLabels(bool b)
{
  CQChartsUtil::testAndSet(xLabels_, b, [&]() { drawObjs(); } );
}

void
CQChartsImagePlot::
setYLabels(bool b)
{
  CQChartsUtil::testAndSet(yLabels_, b, [&]() { drawObjs(); } );
}

void
CQChartsImagePlot::
setCellLabels(bool b)
{
  CQChartsUtil::testAndSet(cellLabels_, b, [&]() { drawObjs(); } );
}

#if 0
void
CQChartsImagePlot::
setScaleCellLabels(bool b)
{
  CQChartsUtil::testAndSet(scaleCellLabels_, b, [&]() { drawObjs(); } );
}
#endif

void
CQChartsImagePlot::
setRectStyle(bool b)
{
  CellStyle cellStyle = (b ? CellStyle::RECT : CellStyle::BALLOON);

  setCellStyle(cellStyle);
}

void
CQChartsImagePlot::
setBalloonStyle(bool b)
{
  CellStyle cellStyle = (b ? CellStyle::BALLOON : CellStyle::RECT);

  setCellStyle(cellStyle);
}

void
CQChartsImagePlot::
setCellStyle(const CellStyle &cellStyle)
{
  CQChartsUtil::testAndSet(cellStyle_, cellStyle, [&]() { drawObjs(); } );
}

//---

CQChartsGeom::Range
CQChartsImagePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsImagePlot::calcRange");

  CQChartsImagePlot *th = const_cast<CQChartsImagePlot *>(this);

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsImagePlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      for (int col = 0; col < numCols(); ++col) {
        bool ok;

        double value = plot_->modelReal(data.row, col, data.parent, ok);

        if (ok && ! CMathUtil::isNaN(value))
          valueRange_.add(value);
      }

      return State::OK;
    }

    double minValue() const { return valueRange_.min(0.0); }
    double maxValue() const { return valueRange_.max(1.0); }

   private:
    const CQChartsImagePlot* plot_     { nullptr };
    CQChartsGeom::RMinMax    valueRange_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  // set value range
  th->minValue_ = visitor.minValue();

  if (xmin().isSet())
    th->minValue_ = xmin().real();

  th->maxValue_ = visitor.maxValue();

  if (xmax().isSet())
    th->maxValue_ = xmax().real();

  //---

  CQChartsGeom::Range dataRange;

  th->nr_ = visitor.numRows();
  th->nc_ = visitor.numCols();

  dataRange.updateRange(  0,   0);
  dataRange.updateRange(nc_, nr_);

  //---

  return dataRange;
}

bool
CQChartsImagePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsImagePlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsImagePlot *plot, PlotObjs &objs) :
     plot_(plot), objs_(objs) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      x_ = 0.0;

      for (int col = 0; col < numCols(); ++col) {
        QModelIndex ind = plot_->modelIndex(data.row, col, data.parent);

        bool ok;

        double value = plot_->modelReal(data.row, col, data.parent, ok);

        //---

        plot_->addImageObj(data.row, col, x_, y_, dx_, dy_, value, ind, objs_);

        //---

        x_ += dx_;
      }

      y_ += dy_;

      return State::OK;
    }

   private:
    const CQChartsImagePlot* plot_ { nullptr };
    PlotObjs&                objs_;
    double                   x_    { 0.0 };
    double                   y_    { 0.0 };
    double                   dx_   { 1.0 };
    double                   dy_   { 1.0 };
  };

  RowVisitor visitor(this, objs);

  visitModel(visitor);

  //---

  return true;
}

void
CQChartsImagePlot::
addImageObj(int row, int col, double x, double y, double dx, double dy, double value,
            const QModelIndex &ind, PlotObjs &objs) const
{
  QModelIndex ind1 = normalizeIndex(ind);

  CQChartsGeom::BBox bbox(x, y, x + dx, y + dy);

  double rv = CMathUtil::map(value, minValue(), maxValue(), 0.0, 1.0);

  CQChartsImageObj *imageObj =
    new CQChartsImageObj(this, bbox, row, col, value, ind1, ColorInd(rv));

  objs.push_back(imageObj);
}

//---

bool
CQChartsImagePlot::
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
CQChartsImagePlot::
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

  QMenu *styleMenu = new QMenu("Cell Style");

  (void) addMenuCheckedAction(styleMenu, "Rect"   , isRectStyle   (), SLOT(setRectStyle   (bool)));
  (void) addMenuCheckedAction(styleMenu, "Balloon", isBalloonStyle(), SLOT(setBalloonStyle(bool)));

  menu->addMenu(styleMenu);

  (void) addCheckedAction("Cell Labels", isCellLabels(), SLOT(setCellLabels(bool)));
  (void) addCheckedAction("X Labels"   , isXLabels   (), SLOT(setXLabels   (bool)));
  (void) addCheckedAction("Y Labels"   , isYLabels   (), SLOT(setYLabels   (bool)));

  return true;
}

//------

bool
CQChartsImagePlot::
hasForeground() const
{
  if (! isXLabels() && ! isYLabels())
    return false;

  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsImagePlot::
execDrawForeground(CQChartsPaintDevice *device) const
{
  if (isXLabels())
    drawXLabels(device);

  if (isYLabels())
    drawYLabels(device);
}

void
CQChartsImagePlot::
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

  textOptions.contrast  = isXLabelTextContrast();
  textOptions.formatted = isXLabelTextFormatted();
  textOptions.scaled    = isXLabelTextScaled();
  textOptions.html      = isXLabelTextHtml();
  textOptions.align     = xLabelTextAlign();
  textOptions.angle     = xLabelTextAngle();
  textOptions.scaled    = isXLabelTextScaled();

  //---

  QFontMetricsF fm(device->font());

  double tw = 0.0;
  double th = fm.height();
  double tm = 4;

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    tw = std::max(tw, fm.width(name));
  }

  for (int col = 0; col < numColumns(); ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    double tw1 = fm.width(name);

    QPointF p(col + 0.5, 0);

    QPointF p1 = windowToPixel(p);

    QRectF trect;

    if (! isInvertY())
      trect = QRectF(p1.x() - tw1/2, p1.y() + tm, tw1, th);
    else
      trect = QRectF(p1.x() - tw1/2, p1.y() - th - tm, tw1, th);

    CQChartsTextOptions textOptions1 = adjustTextOptions(textOptions);

    CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(trect), name, textOptions1);
  }
}

void
CQChartsImagePlot::
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

  textOptions.contrast  = isYLabelTextContrast();
  textOptions.formatted = isYLabelTextFormatted();
  textOptions.scaled    = isYLabelTextScaled();
  textOptions.html      = isYLabelTextHtml();
  textOptions.align     = yLabelTextAlign();
  textOptions.angle     = yLabelTextAngle();
  textOptions.scaled    = isYLabelTextScaled();

  //---

  QFontMetricsF fm(device->font());

  double tw = 0.0;
  double th = fm.height();
  double tm = 4;

  for (int row = 0; row < numRows(); ++row) {
    bool ok;

    QString name = modelHeaderString(row, Qt::Vertical, ok);
    if (! name.length()) continue;

    tw = std::max(tw, fm.width(name));
  }

  for (int row = 0; row < numRows(); ++row) {
    bool ok;

    QString name = modelHeaderString(row, Qt::Vertical, ok);
    if (! name.length()) continue;

    QPointF p(0, row + 0.5);

    QPointF p1 = windowToPixel(p);

    QRectF trect;

    if (! isInvertX())
      trect = QRectF(p1.x() - tw - tm, p1.y() - th/2.0, tw, th);
    else
      trect = QRectF(p1.x() + tm, p1.y() - th/2.0, tw, th);

    CQChartsTextOptions textOptions1 = adjustTextOptions(textOptions);

    CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(trect), name, textOptions1);
  }
}

//------

CQChartsGeom::BBox
CQChartsImagePlot::
annotationBBox() const
{
  QFont font = view()->plotFont(this, cellLabelTextFont());

  QFontMetricsF fm(font);

  CQChartsGeom::BBox bbox;

  double tm = 4;

  if (isXLabels()) {
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

  if (isYLabels()) {
    double tw = 0.0;

    for (int row = 0; row < numRows(); ++row) {
      bool ok;

      QString name = modelHeaderString(row, Qt::Vertical, ok);
      if (! name.length()) continue;

      tw = std::max(tw, fm.width(name));
    }

    double tw1 = pixelToWindowWidth(tw + tm);

    CQChartsGeom::BBox tbbox(-tw1, 0, 0, numRows());

    bbox += tbbox;
  }

  return bbox;
}

//------

CQChartsImageObj::
CQChartsImageObj(const CQChartsImagePlot *plot, const CQChartsGeom::BBox &rect,
                 int row, int col, double value, const QModelIndex &ind, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsImagePlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), row_(row), col_(col), value_(value)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(ind);
}

QString
CQChartsImageObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(row_).arg(col_);
}

QString
CQChartsImageObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  bool ok;

  QString xname = plot_->modelHeaderString(modelInd().column(), ok);
  QString yname = plot_->modelHeaderString(modelInd().row(), Qt::Vertical, ok);

  if (xname.length())
    tableTip.addTableRow("X", xname);

  if (yname.length())
    tableTip.addTableRow("Y", yname);

  tableTip.addTableRow("Value", value_);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

void
CQChartsImageObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, modelInd().column());
}

void
CQChartsImageObj::
draw(CQChartsPaintDevice *device)
{
  ColorInd ic;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    double v = CMathUtil::norm(value_, plot_->minValue(), plot_->maxValue());

    ic = ColorInd(v);
  }
  else
    ic = calcColorInd();

  //---

  // set pen and brush
  QColor fc = plot_->interpCellFillColor  (ic);
  QColor bc = plot_->interpCellStrokeColor(ic);

  QPen   pen;
  QBrush brush;

  plot_->setPen(pen, plot_->isCellStroked(), bc, plot_->cellStrokeAlpha(),
                plot_->cellStrokeWidth(), plot_->cellStrokeDash());

  plot_->setBrush(brush, plot_->isCellFilled(), fc, plot_->cellFillAlpha(),
                  plot_->cellFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  if      (plot_->cellStyle() == CQChartsImagePlot::CellStyle::RECT) {
    QRectF qrect = rect().qrect();

    device->drawRect(qrect);

    //---

    if (plot_->isCellLabels()) {
      // set font
      plot_->view()->setPlotPainterFont(plot_, device, plot_->cellLabelTextFont());

      //---

      // set pen
      QPen   tpen;
      QBrush tbrush;

      QColor tc = plot_->interpCellLabelTextColor(ic);

      plot_->setPen(tpen, true, tc, plot_->cellLabelTextAlpha());

      plot_->updateObjPenBrushState(this, tpen, tbrush);

      device->setPen(tpen);

      //---

      QString valueStr = CQChartsUtil::formatReal(value_);

      CQChartsTextOptions textOptions;

      textOptions.contrast  = plot_->isCellLabelTextContrast();
      textOptions.formatted = plot_->isCellLabelTextFormatted();
      textOptions.scaled    = plot_->isCellLabelTextScaled();
      textOptions.html      = plot_->isCellLabelTextHtml();
      textOptions.align     = plot_->cellLabelTextAlign();
      textOptions.scaled    = plot_->isCellLabelTextScaled();

      textOptions = plot_->adjustTextOptions(textOptions);

      CQChartsDrawUtil::drawTextInBox(device, qrect, valueStr, textOptions);
    }
  }
  else if  (plot_->cellStyle() == CQChartsImagePlot::CellStyle::BALLOON) {
    CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

    QRectF qrect = prect.qrect();

    double s = std::min(qrect.width(), qrect.height());

    double minSize = s*plot_->minBalloonSize();
    double maxSize = s*plot_->maxBalloonSize();

    double s1 = CMathUtil::map(value_, plot_->minValue(), plot_->maxValue(), minSize, maxSize);

    QPointF center = qrect.center();

    //---

    QRectF erect(center.x() - s1/2, center.y() - s1/2, s1, s1);

    device->drawEllipse(device->pixelToWindow(erect));
  }
}

double
CQChartsImageObj::
xColorValue(bool relative) const
{
  if (! relative)
    return col_;
  else
    return CMathUtil::map(col_, 0.0, 1.0*plot_->numColumns(), 0.0, 1.0);
}

double
CQChartsImageObj::
yColorValue(bool relative) const
{
  if (! relative)
    return col_;
  else
    return CMathUtil::map(row_, 0.0, 1.0*plot_->numRows(), 0.0, 1.0);
}
