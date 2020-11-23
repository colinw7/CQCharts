#include <CQChartsCorrelationPlot.h>
#include <CQChartsCorrelationModel.h>
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
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
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
create(View *view, const ModelP &model) const
{
  return new CQChartsCorrelationPlot(view, model);
}

//------

CQChartsCorrelationPlot::
CQChartsCorrelationPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("correlation"), model),
 CQChartsObjCellShapeData    <CQChartsCorrelationPlot>(this),
 CQChartsObjCellLabelTextData<CQChartsCorrelationPlot>(this),
 CQChartsObjXLabelTextData   <CQChartsCorrelationPlot>(this),
 CQChartsObjYLabelTextData   <CQChartsCorrelationPlot>(this)
{
}

CQChartsCorrelationPlot::
~CQChartsCorrelationPlot()
{
  term();
}

//---

void
CQChartsCorrelationPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  // create correlation model
  CQChartsLoader loader(charts());

  correlationModel_ = loader.createCorrelationModel(model().data());

  baseModel_ = dynamic_cast<CQChartsCorrelationModel *>(correlationModel_->baseModel());

  nc_ = correlationModel_->columnCount();

  //---

  correlationModelP_ = ModelP(correlationModel_);

  auto *modelData = charts()->initModelData(correlationModelP_);

  charts()->setModelName(modelData, "correlationModel");

  //---

  addTitle();

  setCellFillColor(Color(Color::Type::PALETTE));

  setCellLabelTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);

  setXLabelTextAlign(Qt::AlignHCenter | Qt::AlignTop    ); setXLabelTextAngle(Angle(90.0));
  setYLabelTextAlign(Qt::AlignRight   | Qt::AlignVCenter); setYLabelTextAngle(Angle( 0.0));

  setXLabelTextFont(CQChartsFont().decFontSize(4));
  setYLabelTextFont(CQChartsFont().decFontSize(4));
}

void
CQChartsCorrelationPlot::
term()
{
  charts()->removeModel(correlationModelP_);
}

//---

void
CQChartsCorrelationPlot::
setDiagonalType(const DiagonalType &t)
{
  CQChartsUtil::testAndSet(diagonalType_, t, [&]() { drawObjs(); } );
}

void
CQChartsCorrelationPlot::
setLowerDiagonalType(const OffDiagonalType &t)
{
  CQChartsUtil::testAndSet(lowerDiagonalType_, t, [&]() { drawObjs(); } );
}

void
CQChartsCorrelationPlot::
setUpperDiagonalType(const OffDiagonalType &t)
{
  CQChartsUtil::testAndSet(upperDiagonalType_, t, [&]() { drawObjs(); } );
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

  addBaseProperties();

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

  // cell types
  addProp("cell", "diagonalType"     , "diagonal"     , "Diagonal cell type");
  addProp("cell", "lowerDiagonalType", "lowerDiagonal", "Lower Diagonal cell type");
  addProp("cell", "upperDiagonalType", "upperDiagonal", "Upper Diagonal cell type");
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

void
CQChartsCorrelationPlot::
diagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text();

    if      (name == "None"   ) setDiagonalType(DiagonalType::NONE);
    else if (name == "Name"   ) setDiagonalType(DiagonalType::NAME);
    else if (name == "Min/Max") setDiagonalType(DiagonalType::MIN_MAX);
    else if (name == "Density") setDiagonalType(DiagonalType::DENSITY);
  }
  else
    setDiagonalType(DiagonalType::NONE);
}

void
CQChartsCorrelationPlot::
upperDiagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text();

    if      (name == "None"      ) setUpperDiagonalType(OffDiagonalType::NONE);
    else if (name == "Pie"       ) setUpperDiagonalType(OffDiagonalType::PIE);
    else if (name == "Shade"     ) setUpperDiagonalType(OffDiagonalType::SHADE);
    else if (name == "Ellipse"   ) setUpperDiagonalType(OffDiagonalType::ELLIPSE);
    else if (name == "Points"    ) setUpperDiagonalType(OffDiagonalType::POINTS);
    else if (name == "Confidence") setUpperDiagonalType(OffDiagonalType::CONFIDENCE);
  }
  else
    setUpperDiagonalType(OffDiagonalType::NONE);
}

void
CQChartsCorrelationPlot::
lowerDiagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text();

    if      (name == "None"      ) setLowerDiagonalType(OffDiagonalType::NONE);
    else if (name == "Pie"       ) setLowerDiagonalType(OffDiagonalType::PIE);
    else if (name == "Shade"     ) setLowerDiagonalType(OffDiagonalType::SHADE);
    else if (name == "Ellipse"   ) setLowerDiagonalType(OffDiagonalType::ELLIPSE);
    else if (name == "Points"    ) setLowerDiagonalType(OffDiagonalType::POINTS);
    else if (name == "Confidence") setLowerDiagonalType(OffDiagonalType::CONFIDENCE);
  }
  else
    setLowerDiagonalType(OffDiagonalType::NONE);
}

//---

CQChartsGeom::Range
CQChartsCorrelationPlot::
calcRange() const
{
  Range dataRange;

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
preDrawObjs(PaintDevice *device) const
{
  if (! isVisible())
    return;

  if (isCellLabelTextScaled()) {
    bool first = true;

    double maxWidth = 0.0, maxHeight = 0.0;

    Size cellSize;

    for (auto &plotObj : plotObjects()) {
      auto *cellObj = dynamic_cast<CQChartsCorrelationCellObj *>(plotObj);
      if (! cellObj) continue;

      auto s = cellObj->calcTextSize();

      maxWidth  = std::max(maxWidth , s.width ());
      maxHeight = std::max(maxHeight, s.height());

      if (first) {
        cellSize = cellObj->rect().size();
        first    = false;
      }
    }

    auto cw = windowToPixelWidth (cellSize.width ());
    auto ch = windowToPixelHeight(cellSize.height());

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
      nc_ = plot_->numColumns();
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      double y = (nc_ - 1 - data.row)*dy_;

      for (int col = 0; col < numCols(); ++col) {
        auto ind = model->index(data.row, col, data.parent);

        bool ok;

        double value = CQChartsModelUtil::modelReal(model, ind, ok);

        //---

        double x = col*dx_;

        plot_->addCellObj(data.row, col, x, y, dx_, dy_, value, ind, objs_);
      }

      return State::OK;
    }

   private:
    const CQChartsCorrelationPlot* plot_ { nullptr };
    PlotObjs&                      objs_;
    int                            nc_   { 0 };
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
  BBox bbox(x, y, x + dx, y + dy);

  auto *cellObj = createCellObj(bbox, row, col, value, ind);

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

  auto c = obj->rect().getCenter();

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
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name, bool isSet, const char *slot) {
    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

#if 0
  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) {
    return addMenuCheckedAction(menu, name, isSet, slot);
  };
#endif

  //---

  menu->addSeparator();

  //---

  auto *labelsMenu = new QMenu("Labels", menu);

  (void) addMenuCheckedAction(labelsMenu, "Cell", isCellLabels(), SLOT(setCellLabels(bool)));
  (void) addMenuCheckedAction(labelsMenu, "X"   , isXLabels   (), SLOT(setXLabels   (bool)));
  (void) addMenuCheckedAction(labelsMenu, "Y"   , isYLabels   (), SLOT(setYLabels   (bool)));

  menu->addMenu(labelsMenu);

  //---

  auto isDiagonalType = [&](const DiagonalType &t) { return diagonalType() == t; };

  auto *diagMenu = new QMenu("Diagonal", menu);

  (void) addMenuCheckedAction(diagMenu, "None"   , isDiagonalType(DiagonalType::NONE),
                              SLOT(diagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(diagMenu, "Name"   , isDiagonalType(DiagonalType::NAME),
                              SLOT(diagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(diagMenu, "Min/Max", isDiagonalType(DiagonalType::MIN_MAX),
                              SLOT(diagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(diagMenu, "Density", isDiagonalType(DiagonalType::DENSITY),
                              SLOT(diagonalTypeSlot(bool)));

  menu->addMenu(diagMenu);

  //---

  auto isUpperDiagonalType = [&](const OffDiagonalType &t) { return upperDiagonalType() == t; };

  auto *udiagMenu = new QMenu("Upper Diagonal", menu);

  (void) addMenuCheckedAction(udiagMenu, "None"      ,
           isUpperDiagonalType(OffDiagonalType::NONE), SLOT(upperDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(udiagMenu, "Pie"       ,
           isUpperDiagonalType(OffDiagonalType::PIE), SLOT(upperDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(udiagMenu, "Shade"     ,
           isUpperDiagonalType(OffDiagonalType::SHADE), SLOT(upperDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(udiagMenu, "Ellipse"   ,
           isUpperDiagonalType(OffDiagonalType::ELLIPSE), SLOT(upperDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(udiagMenu, "Points"    ,
           isUpperDiagonalType(OffDiagonalType::POINTS), SLOT(upperDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(udiagMenu, "Confidence",
           isUpperDiagonalType(OffDiagonalType::CONFIDENCE), SLOT(upperDiagonalTypeSlot(bool)));

  menu->addMenu(udiagMenu);

  //---

  auto isLowerDiagonalType = [&](const OffDiagonalType &t) { return lowerDiagonalType() == t; };

  auto *ldiagMenu = new QMenu("Lower Diagonal", menu);

  (void) addMenuCheckedAction(ldiagMenu, "None"      ,
           isLowerDiagonalType(OffDiagonalType::NONE), SLOT(lowerDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(ldiagMenu, "Pie"       ,
           isLowerDiagonalType(OffDiagonalType::PIE), SLOT(lowerDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(ldiagMenu, "Shade"     ,
           isLowerDiagonalType(OffDiagonalType::SHADE), SLOT(lowerDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(ldiagMenu, "Ellipse"   ,
           isLowerDiagonalType(OffDiagonalType::ELLIPSE), SLOT(lowerDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(ldiagMenu, "Points"    ,
           isLowerDiagonalType(OffDiagonalType::POINTS), SLOT(lowerDiagonalTypeSlot(bool)));
  (void) addMenuCheckedAction(ldiagMenu, "Confidence",
           isLowerDiagonalType(OffDiagonalType::CONFIDENCE), SLOT(lowerDiagonalTypeSlot(bool)));

  menu->addMenu(ldiagMenu);

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
execDrawForeground(PaintDevice *device) const
{
  if (isXLabels())
    drawXLabels(device);

  if (isYLabels())
    drawYLabels(device);
}

void
CQChartsCorrelationPlot::
drawXLabels(PaintDevice *device) const
{
  view()->setPlotPainterFont(this, device, xLabelTextFont());

  //---

  PenBrush tpenBrush;

  auto tc = interpXLabelTextColor(ColorInd());

  setPen(tpenBrush, PenData(true, tc, xLabelTextAlpha()));

  device->setPen(tpenBrush.pen);

  //---

  CQChartsTextOptions textOptions;

  textOptions.angle         = xLabelTextAngle();
  textOptions.align         = xLabelTextAlign();
  textOptions.contrast      = isXLabelTextContrast();
  textOptions.contrastAlpha = xLabelTextContrastAlpha();
  textOptions.formatted     = isXLabelTextFormatted();
  textOptions.scaled        = isXLabelTextScaled();
  textOptions.html          = isXLabelTextHtml();
  textOptions.clipLength    = lengthPixelWidth(xLabelTextClipLength());
  textOptions.clipElide     = xLabelTextClipElide();
  textOptions.clipped       = false;

  textOptions = adjustTextOptions(textOptions);

  //---

  using ColRects = std::map<int, BBox>;

  ColRects colRects;

  double tw = 0.0;
  double th = 0.0;

  int nc = numColumns();

  for (int c = 0; c < nc; ++c) {
    Column col(c);

    bool ok;

    QString name =
      CQChartsModelUtil::modelHHeaderString(correlationModel_, col, Qt::DisplayRole, ok);
    if (! name.length()) continue;

    double px = 0.0;
    double py = 0.0;

    auto trect = CQChartsRotatedText::calcBBox(px, py, name, device->font(),
                                               textOptions, 0, /*alignBBox*/ true);

    colRects[c] = trect;

    tw = std::max(tw, trect.getWidth ());
    th = std::max(th, trect.getHeight());
  }

  //---

  auto cw = windowToPixelWidth(1.0);

  double tm = 4;

  for (int c = 0; c < nc; ++c) {
    Column col(c);

    bool ok;

    QString name =
      CQChartsModelUtil::modelHHeaderString(correlationModel_, col, Qt::DisplayRole, ok);
    if (! name.length()) continue;

    Point p(c + 0.5, 0);

    auto p1 = windowToPixel(p);

    auto trect = colRects[c];

    double tw1 = std::max(trect.getWidth(), cw);

    BBox tbbox1;

    if (! isInvertY())
      tbbox1 = BBox(p1.x - tw1/2, p1.y + tm, p1.x + tw1/2, p1.y + tm + th);
    else
      tbbox1 = BBox(p1.x - tw1/2, p1.y - th - tm, p1.x + tw1/2, p1.y - tm);

    CQChartsDrawUtil::drawTextInBox(device, pixelToWindow(tbbox1), name, textOptions);
  }
}

void
CQChartsCorrelationPlot::
drawYLabels(PaintDevice *device) const
{
  view()->setPlotPainterFont(this, device, yLabelTextFont());

  //---

  PenBrush tpenBrush;

  auto tc = interpYLabelTextColor(ColorInd());

  setPen(tpenBrush, PenData(true, tc, yLabelTextAlpha()));

  device->setPen(tpenBrush.pen);

  //---

  CQChartsTextOptions textOptions;

  textOptions.angle         = yLabelTextAngle();
  textOptions.align         = yLabelTextAlign();
  textOptions.contrast      = isYLabelTextContrast();
  textOptions.contrastAlpha = yLabelTextContrastAlpha();
  textOptions.formatted     = isYLabelTextFormatted();
  textOptions.scaled        = isYLabelTextScaled();
  textOptions.html          = isYLabelTextHtml();
  textOptions.clipLength    = lengthPixelWidth(yLabelTextClipLength());
  textOptions.clipElide     = yLabelTextClipElide();
  textOptions.clipped       = false;

  textOptions = adjustTextOptions(textOptions);

  //---

  using RowRects = std::map<int, BBox>;

  RowRects colRects;

  double tw = 0.0;
  double th = 0.0;

  int nc = numColumns();

  for (int c = 0; c < nc; ++c) {
    Column col(c);

    bool ok;

    QString name =
      CQChartsModelUtil::modelHHeaderString(correlationModel_, col, Qt::DisplayRole, ok);
    if (! name.length()) continue;

    double px = 0.0;
    double py = 0.0;

    auto trect = CQChartsRotatedText::calcBBox(px, py, name, device->font(),
                                               textOptions, 0, /*alignBBox*/ true);

    colRects[c] = trect;

    tw = std::max(tw, trect.getWidth ());
    th = std::max(th, trect.getHeight());
  }

  //---

  auto ch = windowToPixelHeight(1.0);

  double tm = 4;

  for (int c = 0; c < nc; ++c) {
    Column col(c);

    bool ok;

    QString name =
      CQChartsModelUtil::modelHHeaderString(correlationModel_, col, Qt::DisplayRole, ok);
    if (! name.length()) continue;

    Point p(0, (nc - 1 - c) + 0.5);

    auto p1 = windowToPixel(p);

    auto trect = colRects[c];

    double th1 = std::max(trect.getHeight(), ch);

    BBox tbbox1;

    if (! isInvertX())
      tbbox1 = BBox(p1.x - tw - tm, p1.y - th1/2, p1.x - tm, p1.y + th1/2);
    else
      tbbox1 = BBox(p1.x + tm, p1.y - th1/2, p1.x + tm + tw, p1.y + th1/2);

    CQChartsDrawUtil::drawTextInBox(device, pixelToWindow(tbbox1), name, textOptions);
  }
}

//------

CQChartsGeom::BBox
CQChartsCorrelationPlot::
calcAnnotationBBox() const
{
  CQPerfTrace trace("CQChartsCorrelationPlot::calcAnnotationBBox");

  auto xfont = view()->plotFont(this, xLabelTextFont());
  auto yfont = view()->plotFont(this, yLabelTextFont());

  BBox bbox;

  double tm = 4;

  int nc = numColumns();

  //---

  if (isXLabels()) {
    double th = 0.0;

    for (int c = 0; c < nc; ++c) {
      Column col(c);

      bool ok;

      QString name =
        CQChartsModelUtil::modelHHeaderString(correlationModel_, col, Qt::DisplayRole, ok);
      if (! name.length()) continue;

      CQChartsTextOptions options;

      options.angle = xLabelTextAngle();
      options.align = xLabelTextAlign();

      double px = 0.0;
      double py = 0.0;

      auto trect = CQChartsRotatedText::calcBBox(px, py, name, xfont,
                                                 options, 0, /*alignBBox*/ true);

      th = std::max(th, trect.getHeight());
    }

    double th1 = pixelToWindowHeight(th + tm);

    BBox tbbox(0, -th1, nc, 0);

    bbox += tbbox;
  }

  //---

  if (isYLabels()) {
    double tw = 0.0;

    for (int c = 0; c < nc; ++c) {
      Column col(c);

      bool ok;

      QString name =
        CQChartsModelUtil::modelHHeaderString(correlationModel_, col, Qt::DisplayRole, ok);
      if (! name.length()) continue;

      CQChartsTextOptions options;

      options.angle = yLabelTextAngle();
      options.align = yLabelTextAlign();

      double px = 0.0;
      double py = 0.0;

      auto trect = CQChartsRotatedText::calcBBox(px, py, name, yfont,
                                                 options, 0, /*alignBBox*/ true);

      tw = std::max(tw, trect.getWidth());
    }

    double tw1 = pixelToWindowWidth(tw + tm);

    BBox tbbox(-tw1, 0, 0, nc);

    bbox += tbbox;
  }

  //---

  return bbox;
}

//---

CQChartsCorrelationCellObj *
CQChartsCorrelationPlot::
createCellObj(const BBox &rect, int row, int col, double value, const QModelIndex &ind) const
{
  return new CQChartsCorrelationCellObj(this, rect, row, col, value, ind);
}

//------

CQChartsCorrelationCellObj::
CQChartsCorrelationCellObj(const CQChartsCorrelationPlot *plot, const BBox &rect,
                           int row, int col, double value, const QModelIndex &ind) :
 CQChartsPlotObj(const_cast<CQChartsCorrelationPlot *>(plot), rect),
 plot_(plot), row_(row), col_(col), value_(value)
{
  setDetailHint(DetailHint::MAJOR);

  valueColorInd(iv_);

  if (ind.isValid()) {
    //setModelInd(ind);
  }
}

QString
CQChartsCorrelationCellObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(row_).arg(col_);
}

QString
CQChartsCorrelationCellObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  bool ok;

  if (row_ != col_) {
    QString xname =
      CQChartsModelUtil::modelHHeaderString(plot_->correlationModel(),
        CQChartsColumn(row_), Qt::DisplayRole, ok);
    QString yname =
      CQChartsModelUtil::modelHHeaderString(plot_->correlationModel(),
        CQChartsColumn(col_), Qt::DisplayRole, ok);

    if (xname.length())
      tableTip.addTableRow("X", xname);

    if (yname.length())
      tableTip.addTableRow("Y", yname);

    tableTip.addTableRow("Value", value());
  }
  else {
    QString xname =
      CQChartsModelUtil::modelHHeaderString(plot_->correlationModel(),
        CQChartsColumn(row_), Qt::DisplayRole, ok);

    if (xname.length())
      tableTip.addTableRow("Name", xname);

    const auto &minMax = plot_->baseModel()->minMax(row_);

    tableTip.addTableRow("Min", minMax.min());
    tableTip.addTableRow("Max", minMax.max());
  }

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

void
CQChartsCorrelationCellObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, CQChartsColumn(modelInd().column()));
}

CQChartsGeom::Size
CQChartsCorrelationCellObj::
calcTextSize() const
{
  if (! plot_->isCellLabels())
    return Size();

  auto font = plot_->view()->plotFont(plot_, plot_->cellLabelTextFont());

  auto valueStr = CQChartsUtil::formatReal(value());

  CQChartsTextOptions options;

  options.angle      = plot_->cellLabelTextAngle();
  options.contrast   = plot_->isCellLabelTextContrast();
  options.formatted  = plot_->isCellLabelTextFormatted();
  options.html       = plot_->isCellLabelTextHtml();
  options.clipLength = plot_->lengthPixelWidth(plot_->cellLabelTextClipLength());
  options.clipElide  = plot_->cellLabelTextClipElide();

  return CQChartsDrawUtil::calcTextSize(valueStr, font, options);
}

void
CQChartsCorrelationCellObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  //---

  bool skipLabel = false;

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  // diagonal
  if (row_ == col_) {
    auto type = plot_->diagonalType();

    if      (type == CQChartsCorrelationPlot::DiagonalType::NAME) {
      bool ok;

      QString xname =
        CQChartsModelUtil::modelHHeaderString(plot_->correlationModel(),
          CQChartsColumn(row_), Qt::DisplayRole, ok);

      if (xname.length())
        drawCellLabel(device, xname);

      skipLabel = true;
    }
    else if (type == CQChartsCorrelationPlot::DiagonalType::MIN_MAX) {
      const auto &minMax = plot_->baseModel()->minMax(row_);

      double x1 = rect().getXMin();
      double x2 = rect().getXMax();
      double y1 = rect().getYMin();
      double y2 = rect().getYMax();
      double w  = rect().getWidth ();
      double h  = rect().getHeight();

      BBox rect1(x1, y1, x1 + w/2.0, y1 + h/2.0);
      BBox rect2(x2 - w/2.0, y2 - h/2.0, x2, y2);

      drawCellLabel(device, QString("%1").arg(minMax.min()), rect1, -4);
      drawCellLabel(device, QString("%1").arg(minMax.max()), rect2, -4);

      //---

      bool ok;

      QString xname =
        CQChartsModelUtil::modelHHeaderString(plot_->correlationModel(),
          CQChartsColumn(row_), Qt::DisplayRole, ok);

      if (xname.length())
        drawCellLabel(device, xname);

      //---

      skipLabel = true;
    }
    else if (type == CQChartsCorrelationPlot::DiagonalType::DENSITY) {
      const auto *density = plot_->baseModel()->density(row_);

      double s  = rect().getMinSize()/2.0;
      auto   rc = rect().getCenter();
      auto   ps = Point(s, s);

      BBox rect1(rc - ps, rc + ps);

      //---

      Polygon poly;

      CQChartsWhiskerOpts opts;

      opts.fitTail = true;

      density->calcDistributionPoly(poly, plot(), rect1, Qt::Horizontal, opts);

      device->drawPolygon(poly);
    }
    else {
      device->drawRect(rect());
    }
  }
  // upper/lower diagonal
  else {
    bool isLower = (row_ > col_);

    CQChartsCorrelationPlot::OffDiagonalType type =
      (isLower ? plot_->lowerDiagonalType() : plot_->upperDiagonalType());

    if      (type == CQChartsCorrelationPlot::OffDiagonalType::PIE) {
      Angle a1, a2;

      a1 = Angle(90);
      a2 = a1 - Angle(360.0*value());

      auto rc = rect().getCenter();

      double s  = rect().getMinSize()/2.0;
      auto   ps = Point(s, s);

      BBox rect1(rc - ps, rc + ps);

      auto penBrush1 = penBrush;

      plot_->setBrush(penBrush1, BrushData(false));

      CQChartsDrawUtil::setPenBrush(device, penBrush1);

      device->drawEllipse(rect1);

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      CQChartsDrawUtil::drawPieSlice(device, rc, 0.0, s, a1, a2);
    }
    else if (type == CQChartsCorrelationPlot::OffDiagonalType::SHADE) {
      device->drawRect(rect());

      //---

      auto lc = Qt::white;

      auto fillPattern = CQChartsFillPattern(CQChartsFillPattern::Type::SOLID);

      if      (value() > 0)
        fillPattern = CQChartsFillPattern(CQChartsFillPattern::Type::FDIAG);
      else if (value() < 0)
        fillPattern = CQChartsFillPattern(CQChartsFillPattern::Type::BDIAG);

      fillPattern.setScale(4.0);

      plot_->setPenBrush(penBrush,
        PenData  (true, lc, Alpha(1.0)),
        BrushData(true, lc, Alpha(1.0), fillPattern));

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      device->drawRect(rect());
    }
    else if (type == CQChartsCorrelationPlot::OffDiagonalType::ELLIPSE) {
      const auto &rminMax = plot_->baseModel()->minMax(row_);
      const auto &cminMax = plot_->baseModel()->minMax(col_);
      const auto &bestFit = plot_->baseModel()->bestFit(row_, col_);

      double xdev, ydev;

      plot_->baseModel()->devData(row_, col_, xdev, ydev);

      double rmin = rminMax.min();
      double rmax = rminMax.max();
      double cmin = cminMax.min();
      double cmax = cminMax.max();

      double s = rect().getMinSize()/2.0;

      //---

      // draw devation ellipse ???
      double dx1 = CMathUtil::map(xdev, 0, (rmax - rmin)/2.0, 0, s);
      double dy1 = CMathUtil::map(ydev, 0, (cmax - cmin)/2.0, 0, s);

      BBox ebbox(Point(rect().getXMid() - dx1, rect().getYMid() - dy1),
                 Point(rect().getXMid() + dx1, rect().getYMid() + dy1));

      device->drawEllipse(ebbox, Angle(45));

      //---

      // draw fit (calc fit shape at each pixel)
      auto prect = plot()->windowToPixel(rect());

      double dx = std::max(prect.getWidth()/100, 1.0);

      Polygon poly;

      for (double px = prect.getXMin(); px <= prect.getXMax(); px += dx) {
        double px1 = CMathUtil::map(px, prect.getXMin(), prect.getXMax(), rmin, rmax);
        double px2 = CMathUtil::map(px, prect.getXMin(), prect.getXMax(), -s, s);

        double py1 = bestFit.interp(px1);
        double py2 = CMathUtil::map(py1, cmin, cmax, -s, s);

        poly.addPoint(Point(rect().getXMid() + px2, rect().getYMid() + py2));
      }

      auto path = CQChartsDrawUtil::polygonToPath(poly, /*closed*/false);

      device->strokePath(path, penBrush.pen);
    }
    else if (type == CQChartsCorrelationPlot::OffDiagonalType::POINTS) {
      const auto &rminMax = plot_->baseModel()->minMax(row_);
      const auto &cminMax = plot_->baseModel()->minMax(col_);
      const auto &points  = plot_->baseModel()->points(row_, col_);

      double rmin = rminMax.min();
      double rmax = rminMax.max();
      double cmin = cminMax.min();
      double cmax = cminMax.max();

      double sx = plot()->pixelToWindowWidth (3);
      double sy = plot()->pixelToWindowHeight(3);

      double s = rect().getMinSize()/2.0 - std::min(sx, sy);

      CQChartsLength ss(3, CQChartsUnits::PIXEL);

      //---

      // draw row/col points
      for (const auto &p : points) {
        double x1 = rect().getXMid() + CMathUtil::map(p.x, rmin, rmax, -s, s);
        double y1 = rect().getYMid() + CMathUtil::map(p.y, cmin, cmax, -s, s);

        Point ps(x1, y1);

        CQChartsDrawUtil::drawSymbol(device, penBrush, CQChartsSymbol::Type::CIRCLE, ps, ss);
      }

      skipLabel = true;
    }
    else if (type == CQChartsCorrelationPlot::OffDiagonalType::CONFIDENCE) {
    }
    else if (type == CQChartsCorrelationPlot::OffDiagonalType::NONE) {
      skipLabel = true;
    }
  }

  //---

  if (plot_->isCellLabels() && ! skipLabel) {
    auto valueStr = CQChartsUtil::formatReal(value());

    drawCellLabel(device, valueStr);
  }

  //---

  device->resetColorNames();
}

void
CQChartsCorrelationCellObj::
drawCellLabel(PaintDevice *device, const QString &str) const
{
  drawCellLabel(device, str, rect());
}

void
CQChartsCorrelationCellObj::
drawCellLabel(PaintDevice *device, const QString &str, const BBox &rect, double fontInc) const
{
  // calc pen and brush
  ColorInd ic;

  valueColorInd(ic);

  //---

  // set font
  auto font = plot_->cellLabelTextFont();

  if (fontInc != 0.0)
    font.incFontSize(fontInc);

  plot_->view()->setPlotPainterFont(plot_, device, font);

  //---

  // set pen
  PenBrush tPenBrush;

  auto tc = plot_->interpCellLabelTextColor(ic);

  plot_->setPen(tPenBrush, PenData(true, tc, plot_->cellLabelTextAlpha()));

  plot_->updateObjPenBrushState(this, tPenBrush);

  device->setPen(tPenBrush.pen);

  //---

  CQChartsTextOptions textOptions;

  textOptions.angle         = plot_->cellLabelTextAngle();
  textOptions.align         = plot_->cellLabelTextAlign();
  textOptions.contrast      = plot_->isCellLabelTextContrast();
  textOptions.contrastAlpha = plot_->cellLabelTextContrastAlpha();
  textOptions.formatted     = plot_->isCellLabelTextFormatted();
  textOptions.scaled        = plot_->isCellLabelTextScaled();
  textOptions.html          = plot_->isCellLabelTextHtml();
  textOptions.clipLength    = plot_->lengthPixelWidth(plot_->cellLabelTextClipLength());
  textOptions.clipElide     = plot_->cellLabelTextClipElide();
  textOptions.scale         = plot_->labelScale(); // TODO: optional

  textOptions = plot_->adjustTextOptions(textOptions);

  CQChartsDrawUtil::drawTextInBox(device, rect, str, textOptions);
}

void
CQChartsCorrelationCellObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // calc pen and brush
  ColorInd ic;

  valueColorInd(ic);

  //---

  // set pen and brush
  auto fc = plot_->interpCellFillColor  (ic);
  auto bc = plot_->interpCellStrokeColor(ic);

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isCellStroked(), bc, plot_->cellStrokeAlpha(),
              plot_->cellStrokeWidth(), plot_->cellStrokeDash()),
    BrushData(plot_->isCellFilled(), fc, plot_->cellFillAlpha(),
              plot_->cellFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsCorrelationCellObj::
valueColorInd(ColorInd &ic) const
{
  ic = ColorInd(std::abs(value()));
}

void
CQChartsCorrelationCellObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.value = " << value() << ";\n";
}

double
CQChartsCorrelationCellObj::
xColorValue(bool) const
{
  return col_;
}

double
CQChartsCorrelationCellObj::
yColorValue(bool) const
{
  return col_;
}
