#include <CQChartsCompositePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsHtml.h>
#include <CQChartsViewPlotPaintDevice.h>

#include <CQPropertyViewItem.h>
#include <CQPropertyViewModel.h>

#include <QMenu>

CQChartsCompositePlotType::
CQChartsCompositePlotType()
{
}

void
CQChartsCompositePlotType::
addParameters()
{
  CQChartsPlotType::addParameters();
}

bool
CQChartsCompositePlotType::
canProbe() const
{
  return true;
}

bool
CQChartsCompositePlotType::
hasObjs() const
{
  return true;
}

QString
CQChartsCompositePlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Composite Plot").
    h3("Summary").
     p("Composite plot.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/composite_plot.png"));
}

CQChartsPlot *
CQChartsCompositePlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsCompositePlot(view, model);
}

//------

CQChartsCompositePlot::
CQChartsCompositePlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("composite"), model)
{
}

CQChartsCompositePlot::
~CQChartsCompositePlot()
{
  term();
}

//---

void
CQChartsCompositePlot::
init()
{
  CQChartsPlot::init();

  //---

  addAxes();

  addKey();

  addTitle();
}

void
CQChartsCompositePlot::
term()
{
}

//---

void
CQChartsCompositePlot::
setCompositeType(const CompositeType &t)
{
  compositeType_ = t;

  updatePlots();
}

void
CQChartsCompositePlot::
setCommonXRange(bool b)
{
  commonXRange_ = b;

  updateRange();
}

void
CQChartsCompositePlot::
setCommonYRange(bool b)
{
  commonYRange_ = b;

  updateRange();
}

//---

void
CQChartsCompositePlot::
addPlot(CQChartsPlot *plot)
{
  plots_.push_back(plot);

  plot->setParentPlot(this);

  if (! currentPlot())
    setCurrentPlot(plot);
  else
    plot->setCurrent(false);

  updatePlots();
}

// called when plot added or composite type changed
void
CQChartsCompositePlot::
updatePlots()
{
  // common x different y axis (first plot x bottom, second x top)
  if      (compositeType_ == CompositeType::X1X2) {
    int i = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (plot->xAxis())
        plot->xAxis()->setSide(CQChartsAxisSide(i == 0 ? CQChartsAxisSide::Type::BOTTOM_LEFT :
                                                         CQChartsAxisSide::Type::TOP_RIGHT));

      if (plot->yAxis())
        plot->yAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));

      ++i;
    }
  }
  // common x different y axis (first plot y left, second y right)
  else if (compositeType_ == CompositeType::Y1Y2) {
    int i = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (plot->xAxis())
        plot->xAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));

      if (plot->yAxis())
        plot->yAxis()->setSide(CQChartsAxisSide(i == 0 ? CQChartsAxisSide::Type::BOTTOM_LEFT :
                                                         CQChartsAxisSide::Type::TOP_RIGHT));

      ++i;
    }
  }
  // common x and y axis (first plot bottom left, second top right)
  else if (compositeType_ == CompositeType::NONE) {
    int i = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (plot->xAxis())
        plot->xAxis()->setSide(CQChartsAxisSide(i == 0 ? CQChartsAxisSide::Type::BOTTOM_LEFT :
                                                         CQChartsAxisSide::Type::TOP_RIGHT));

      if (plot->yAxis())
        plot->yAxis()->setSide(CQChartsAxisSide(i == 0 ? CQChartsAxisSide::Type::BOTTOM_LEFT :
                                                         CQChartsAxisSide::Type::TOP_RIGHT));

      ++i;
    }
  }
  // tabbed (all bottom left)
  else if (compositeType_ == CompositeType::TABBED) {
    for (auto &plot : plots_) {
      if (plot->xAxis())
        plot->xAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));

      if (plot->yAxis())
        plot->yAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
    }
  }
}

//---

void
CQChartsCompositePlot::
addProperties()
{
  addProp("", "compositeType" , "compositeType", "Composite Type");
  addProp("", "commonXRange"  , "commonXRange" , "Common X Range");
  addProp("", "commonYRange"  , "commonYRange" , "Common Y Range");
  addProp("", "currentPlotInd", "currentPlot"  , "Current Plot Index");

  addBaseProperties();

  //---

  //hideProp(this, "range.innerView");
}

CQChartsGeom::Range
CQChartsCompositePlot::
calcRange() const
{
  Range dataRange;
  Range currentDataRange;

  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto dataRange1 = plot->calcRange();

    if (plot == currentPlot())
      currentDataRange = dataRange1;

    dataRange += dataRange1;
  }

  if (! dataRange.isSet()) {
    dataRange.updateRange(0.0, 0.0);
    dataRange.updateRange(1.0, 1.0);
  }

  if (! currentDataRange.isSet()) {
    currentDataRange.updateRange(0.0, 0.0);
    currentDataRange.updateRange(1.0, 1.0);
  }

  if      (! isCommonXRange() && ! isCommonYRange()) {
    return currentDataRange;
  }
  else if (! isCommonXRange()) {
    return Range(currentDataRange.xmin(), dataRange.ymin(),
                 currentDataRange.xmax(), dataRange.ymax());
  }
  else if (! isCommonYRange()) {
    return Range(dataRange.xmin(), currentDataRange.ymin(),
                 dataRange.xmax(), currentDataRange.ymax());
  }
  else {
    return dataRange;
  }

  return dataRange;
}

//---

void
CQChartsCompositePlot::
postUpdateObjs()
{
  if (compositeType_ == CompositeType::TABBED) {
    if (currentPlot())
      currentPlot()->postUpdateObjs();
    else
      CQChartsPlot::postUpdateObjs();
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      plot->postUpdateObjs();
    }
  }
}

//---

void
CQChartsCompositePlot::
updateAndAdjustRanges()
{
  if (currentPlot())
    currentPlot()->updateAndAdjustRanges();
  else
    CQChartsPlot::updateAndAdjustRanges();
}

const CQChartsGeom::Range &
CQChartsCompositePlot::
dataRange() const
{
  return (currentPlot() ? currentPlot()->dataRange() : CQChartsPlot::dataRange());
}

void
CQChartsCompositePlot::
setDataRange(const Range &r, bool update)
{
  if (currentPlot())
    currentPlot()->setDataRange(r, update);
  else
    CQChartsPlot::setDataRange(r, update);
}

void
CQChartsCompositePlot::
resetDataRange(bool updateRange, bool updateObjs)
{
  if (currentPlot())
    currentPlot()->resetDataRange(updateRange, updateObjs);
  else
    CQChartsPlot::resetDataRange(updateRange, updateObjs);
}

//---

bool
CQChartsCompositePlot::
hasPlotObjs() const
{
  if (compositeType_ == CompositeType::TABBED) {
    return (currentPlot() ? currentPlot()->hasPlotObjs() : CQChartsPlot::hasPlotObjs());
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      return plot->hasPlotObjs();
    }
  }

  return false;
}

bool
CQChartsCompositePlot::
createObjs(PlotObjs &) const
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->createObjs();
  }

  return true;
}

void
CQChartsCompositePlot::
resetInsideObjs()
{
  if (currentPlot())
    currentPlot()->resetInsideObjs1();
}

//---

void
CQChartsCompositePlot::
doPostObjTree()
{
  return (currentPlot() ? currentPlot()->doPostObjTree() : CQChartsPlot::doPostObjTree());
}

bool
CQChartsCompositePlot::
isPlotObjTreeSet() const
{
  return (currentPlot() ? currentPlot()->isPlotObjTreeSet() : CQChartsPlot::isPlotObjTreeSet());
}

void
CQChartsCompositePlot::
setPlotObjTreeSet(bool b)
{
  return (currentPlot() ? currentPlot()->setPlotObjTreeSet(b) : CQChartsPlot::setPlotObjTreeSet(b));
}

//---

void
CQChartsCompositePlot::
clearPlotObjects()
{
  if (compositeType_ == CompositeType::TABBED) {
    return (currentPlot() ? currentPlot()->clearPlotObjects() :
                            CQChartsPlot::clearPlotObjects1());
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      plot->clearPlotObjects();
    }
  }
}

void
CQChartsCompositePlot::
clearInsideObjects()
{
  for (auto &plot : plots_)
    plot->clearInsideObjects1();

  clearInsideObjects1();
}

void
CQChartsCompositePlot::
invalidateObjTree()
{
  for (auto &plot : plots_)
    plot->invalidateObjTree1();
}

bool
CQChartsCompositePlot::
updateInsideObjects(const Point &w, Constraints constraints)
{
  Objs objs;

  groupedObjsAtPoint(w, objs, constraints);

  if (compositeType_ == CompositeType::TABBED) {
    if (currentPlot()) {
      auto w1 = currentPlot()->pixelToWindow(windowToPixel(w));

      currentPlot()->groupedObjsAtPoint(w1, objs, constraints);
    }
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      // get objects and annotations at point
      auto w1 = plot->pixelToWindow(windowToPixel(w));

      plot->groupedObjsAtPoint(w1, objs, constraints);
    }
  }

  return setInsideObjects(w, objs);
}

CQChartsPlot::Obj *
CQChartsCompositePlot::
insideObject() const
{
  return insideObject1();
}

QString
CQChartsCompositePlot::
insideObjectText() const
{
  return insideObjectText1();
}

void
CQChartsCompositePlot::
initPlotObjs()
{
  if (compositeType_ == CompositeType::TABBED) {
    return (currentPlot() ? currentPlot()->initPlotObjs() : CQChartsPlot::initPlotObjs());
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      plot->initPlotObjs();
    }
  }
}

void
CQChartsCompositePlot::
initObjTree()
{
  if (compositeType_ == CompositeType::TABBED) {
    return (currentPlot() ? currentPlot()->initObjTree() : CQChartsPlot::initObjTree());
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      plot->initObjTree();
    }
  }
}

//---

void
CQChartsCompositePlot::
updatePlotKeyPosition(Plot *plot, bool force)
{
  auto *currentPlot = this->currentPlot();
  if (plot != currentPlot) return;

  return CQChartsPlot::updatePlotKeyPosition(plot, force);
}

//---

CQChartsGeom::BBox
CQChartsCompositePlot::
calcExtraFitBBox() const
{
  if (compositeType_ == CompositeType::TABBED) {
    return (currentPlot() ? currentPlot()->calcExtraFitBBox() : CQChartsPlot::calcExtraFitBBox());
  }
  else {
    BBox bbox;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      bbox += plot->calcExtraFitBBox();
    }

    return bbox;
  }
}

void
CQChartsCompositePlot::
updateAxisRanges(const BBox &adjustedRange)
{
  if (compositeType_ == CompositeType::TABBED) {
    return (currentPlot() ? currentPlot()->updateAxisRanges(adjustedRange) :
                            CQChartsPlot::updateAxisRanges(adjustedRange));
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      plot->updateAxisRanges(adjustedRange);
    }
  }
}

void
CQChartsCompositePlot::
applyDataRange(bool propagate)
{
  return (currentPlot() ? currentPlot()->applyDataRange(propagate) :
                          CQChartsPlot::applyDataRange(propagate));
}

//---

#if 0
bool
CQChartsCompositePlot::
isKeyVisible() const
{
  return (currentPlot() ? currentPlot()->isKeyVisible() : CQChartsPlot::isKeyVisible());
}

void
CQChartsCompositePlot::
setKeyVisible(bool b)
{
  return (currentPlot() ? currentPlot()->setKeyVisible(b) : CQChartsPlot::setKeyVisible(b));
}

bool
CQChartsCompositePlot::
isKeyVisibleAndNonEmpty() const
{
  return (currentPlot() ? currentPlot()->isKeyVisibleAndNonEmpty() :
                          CQChartsPlot::isKeyVisibleAndNonEmpty());
}
#endif

#if 0
bool
CQChartsCompositePlot::
isColorKey() const
{
  return (currentPlot() ? currentPlot()->isColorKey() : CQChartsPlot::isColorKey());
}

void
CQChartsCompositePlot::
setColorKey(bool b)
{
  return (currentPlot() ? currentPlot()->setColorKey(b) : CQChartsPlot::setColorKey(b));
}
#endif

//---

bool
CQChartsCompositePlot::
isEqualScale() const
{
  return (currentPlot() ? currentPlot()->isEqualScale() : CQChartsPlot::isEqualScale());
}

void
CQChartsCompositePlot::
setEqualScale(bool b)
{
  return (currentPlot() ? currentPlot()->setEqualScale(b) : CQChartsPlot::setEqualScale(b));
}

//---

void
CQChartsCompositePlot::
autoFitOne()
{
  return (currentPlot() ? currentPlot()->autoFitOne() : CQChartsPlot::autoFitOne());
}

void
CQChartsCompositePlot::
autoFit()
{
  return (currentPlot() ? currentPlot()->autoFit() : CQChartsPlot::autoFit());
}

bool
CQChartsCompositePlot::
isAutoFit() const
{
  return (currentPlot() ? currentPlot()->isAutoFit() : CQChartsPlot::isAutoFit());
}

void
CQChartsCompositePlot::
setAutoFit(bool b)
{
  return (currentPlot() ? currentPlot()->setAutoFit(b) : CQChartsPlot::setAutoFit(b));
}

const CQChartsPlotMargin &
CQChartsCompositePlot::
fitMargin() const
{
  return (currentPlot() ? currentPlot()->fitMargin() : CQChartsPlot::fitMargin());
}

void
CQChartsCompositePlot::
setFitMargin(const PlotMargin &m)
{
  return (currentPlot() ? currentPlot()->setFitMargin(m) : CQChartsPlot::setFitMargin(m));
}

void
CQChartsCompositePlot::
resetExtraBBox() const
{
  return (currentPlot() ? currentPlot()->resetExtraBBox() : CQChartsPlot::resetExtraBBox());
}

bool
CQChartsCompositePlot::
needsAutoFit() const
{
  return (currentPlot() ? currentPlot()->needsAutoFit() : CQChartsPlot::needsAutoFit());
}

void
CQChartsCompositePlot::
setNeedsAutoFit(bool b)
{
  return (currentPlot() ? currentPlot()->setNeedsAutoFit(b) : CQChartsPlot::setNeedsAutoFit(b));
}

//---

CQChartsGeom::BBox
CQChartsCompositePlot::
calcViewBBox() const
{
  return (currentPlot() ? currentPlot()->calcViewBBox() : CQChartsPlot::calcViewBBox());
}

CQChartsGeom::BBox
CQChartsCompositePlot::
adjustedViewBBox(const CQChartsPlot *plot) const
{
  auto bbox = plot->calcViewBBox();

  if (compositeType_ == CompositeType::TABBED) {
    calcTabData(plots_);

    double h = view()->pixelToWindowHeight(tabData_.pth*plot->plotDepth());

    bbox = BBox(bbox.getXMin(), bbox.getYMin() + h, bbox.getXMax(), bbox.getYMax());
  }

  return bbox;
}

//---

void
CQChartsCompositePlot::
waitRange()
{
  CQChartsPlot::execWaitRange();
}

void
CQChartsCompositePlot::
waitObjs()
{
  CQChartsPlot::execWaitObjs();
}

void
CQChartsCompositePlot::
waitDraw()
{
  CQChartsPlot::execWaitDraw();
}

void
CQChartsCompositePlot::
waitTree()
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    plot->execWaitTree();
  }
}

//---

bool
CQChartsCompositePlot::
isInvertX() const
{
  return (currentPlot() ? currentPlot()->isInvertX() : CQChartsPlot::isInvertX());
}

void
CQChartsCompositePlot::
setInvertX(bool b)
{
  if (currentPlot())
    currentPlot()->setInvertX(b);
  else
    CQChartsPlot::setInvertX(b);
}

bool
CQChartsCompositePlot::
isInvertY() const
{
  return (currentPlot() ? currentPlot()->isInvertY() : CQChartsPlot::isInvertY());
}

void
CQChartsCompositePlot::
setInvertY(bool b)
{
  if (currentPlot())
    currentPlot()->setInvertY(b);
  else
    CQChartsPlot::setInvertY(b);
}

//---

bool
CQChartsCompositePlot::
isLogX() const
{
  return (currentPlot() ? currentPlot()->isLogX() : CQChartsPlot::isLogX());
}

void
CQChartsCompositePlot::
setLogX(bool b)
{
  if (currentPlot())
    currentPlot()->setLogX(b);
  else
    CQChartsPlot::setLogX(b);
}

bool
CQChartsCompositePlot::
isLogY() const
{
  return (currentPlot() ? currentPlot()->isLogY() : CQChartsPlot::isLogY());
}

void
CQChartsCompositePlot::
setLogY(bool b)
{
  if (currentPlot())
    currentPlot()->setLogY(b);
  else
    CQChartsPlot::setLogY(b);
}

//---

bool
CQChartsCompositePlot::
hasBackground() const
{
  return (currentPlot() ? currentPlot()->hasBackground() : CQChartsPlot::hasBackground());
}

bool
CQChartsCompositePlot::
hasForeground() const
{
  return (currentPlot() ? currentPlot()->hasForeground() : CQChartsPlot::hasForeground());
}

bool
CQChartsCompositePlot::
hasBgAxes() const
{
  return (currentPlot() ? currentPlot()->hasBgAxes() : CQChartsPlot::hasBgAxes());
}

bool
CQChartsCompositePlot::
hasFgAxes() const
{
  return (currentPlot() ? currentPlot()->hasFgAxes() : CQChartsPlot::hasFgAxes());
}

bool
CQChartsCompositePlot::
hasGroupedBgKey() const
{
  return (currentPlot() ? currentPlot()->hasGroupedBgKey() : CQChartsPlot::hasGroupedBgKey());
}

bool
CQChartsCompositePlot::
hasGroupedFgKey() const
{
  return (currentPlot() ? currentPlot()->hasGroupedFgKey() : CQChartsPlot::hasGroupedFgKey());
}

bool
CQChartsCompositePlot::
hasObjs(const CQChartsLayer::Type &layerType) const
{
  if (compositeType_ == CompositeType::TABBED) {
    return (currentPlot() ? currentPlot()->hasObjs(layerType) : CQChartsPlot::hasObjs(layerType));
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (plot->hasObjs(layerType))
        return true;
    }

    return false;
  }
}

//------

bool
CQChartsCompositePlot::
isBufferLayers() const
{
  if (compositeType_ == CompositeType::TABBED) {
    return (currentPlot() ? currentPlot()->isBufferLayers() : CQChartsPlot::isBufferLayers());
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      return plot->isBufferLayers();
    }

    return CQChartsPlot::isBufferLayers();
  }
}

void
CQChartsCompositePlot::
drawPlotParts(QPainter *painter) const
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->drawPlotParts(painter);

    if (! plot->isBufferLayers())
      return;
  }

  CQChartsPlot::drawPlotParts(painter);
}

void
CQChartsCompositePlot::
drawBackgroundDeviceParts(PaintDevice *device, const BackgroundParts &bgParts) const
{
  auto *painter = dynamic_cast<CQChartsPlotPaintDevice *>(device)->painter();

  // draw all plots background parts except axis and key
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    CQChartsPlotPaintDevice device1(plot, painter);

    BackgroundParts bgParts1 = bgParts;

    bgParts1.axes = false;
    bgParts1.key  = false;

    plot->drawBackgroundDeviceParts(&device1, bgParts1);
  }

  //---

  if (bgParts.axes || bgParts.key) {
    // draw current plot axis and key for tabbed
    if (compositeType_ == CompositeType::TABBED) {
      auto *currentPlot = this->currentPlot();

      if (currentPlot) {
        CQChartsPlotPaintDevice device1(currentPlot, painter);

        if (bgParts.axes)
          currentPlot->drawBgAxes(&device1);

        if (bgParts.key)
          currentPlot->drawBgKey(&device1);
      }
    }
    else {
      // for overlay use our custom draw code
      if (bgParts.axes)
        drawBgAxes(device);

      if (bgParts.key)
        drawBgKey(device);
    }
  }
}

void
CQChartsCompositePlot::
drawMiddleDeviceParts(PaintDevice *device, bool bg, bool mid, bool fg) const
{
  auto *painter = dynamic_cast<CQChartsPlotPaintDevice *>(device)->painter();

  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    CQChartsPlotPaintDevice device1(plot, painter);

    plot->drawMiddleDeviceParts(&device1, bg, mid, fg);
  }
}

void
CQChartsCompositePlot::
drawForegroundDeviceParts(PaintDevice *device, const ForegroundParts &fgParts) const
{
  auto *painter = dynamic_cast<CQChartsPlotPaintDevice *>(device)->painter();

  // draw all plots background parts except axis, key and title
  if (fgParts.axes || fgParts.key || fgParts.title) {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      CQChartsPlotPaintDevice device1(plot, painter);

      ForegroundParts fgParts1 = fgParts;

      fgParts1.axes  = false;
      fgParts1.key   = false;
      fgParts1.title = false;

      plot->drawForegroundDeviceParts(&device1, fgParts1);
    }
  }

  //---

  if (fgParts.axes || fgParts.key || fgParts.title) {
    // draw current plot axis and key for tabbed
    if (compositeType_ == CompositeType::TABBED) {
      auto *currentPlot = this->currentPlot();

      if (currentPlot) {
        CQChartsPlotPaintDevice device1(currentPlot, painter);

        if (fgParts.axes)
          currentPlot->drawFgAxes(&device1);

        if (fgParts.key)
          currentPlot->drawFgKey(&device1);

        if (fgParts.title)
          currentPlot->drawTitle(&device1);
      }
    }
    else {
      // for overlay use our custom draw code
      if (fgParts.axes)
        drawFgAxes(device);

      if (fgParts.key)
        drawFgKey(device);

      if (fgParts.title)
        drawTitle(device);
    }
  }

  //---

  if (fgParts.tabbed)
    drawTabs(device);

  if (compositeType_ == CompositeType::TABBED)
    drawTabs(device, plots_, currentPlot());
}

void
CQChartsCompositePlot::
drawOverlayDeviceParts(PaintDevice *device, const OverlayParts &overlayParts) const
{
  auto *painter = dynamic_cast<CQChartsPlotPaintDevice *>(device)->painter();

  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    CQChartsPlotPaintDevice device1(plot, painter);

    plot->drawOverlayDeviceParts(&device1, overlayParts);
  }
}

void
CQChartsCompositePlot::
drawBgAxes(PaintDevice *device) const
{
  auto *painter = dynamic_cast<CQChartsPlotPaintDevice *>(device)->painter();

  // common y different x axis
  if      (compositeType_ == CompositeType::X1X2) {
    int ix = 0, iy = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      CQChartsPlotPaintDevice device1(plot, painter);

      if (ix < 2) {
        if (plot->drawBgXAxis(&device1))
          ++ix;
      }

      if (iy < 1) {
        if (plot->drawBgYAxis(&device1))
          ++iy;
      }
    }
  }
  // common x different y axis
  else if (compositeType_ == CompositeType::Y1Y2) {
    int ix = 0, iy = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      CQChartsPlotPaintDevice device1(plot, painter);

      if (ix < 1) {
        if (plot->drawBgXAxis(&device1))
          ++ix;
      }

      if (iy < 2) {
        if (plot->drawBgYAxis(&device1))
          ++iy;
      }
    }
  }
  // tabbed
  else if (compositeType_ == CompositeType::TABBED) {
    auto *currentPlot = this->currentPlot();

    if (currentPlot) {
      CQChartsPlotPaintDevice device1(currentPlot, painter);

      currentPlot->drawBgAxes(&device1);
    }
  }
  // common x and y axis
  else {
    int ix = 0, iy = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      CQChartsPlotPaintDevice device1(plot, painter);

      if (ix < 2) {
        if (plot->drawBgXAxis(&device1))
          ++ix;
      }

      if (iy < 2) {
        if (plot->drawBgYAxis(&device1))
          ++iy;
      }
    }
  }
}

void
CQChartsCompositePlot::
drawFgAxes(PaintDevice *device) const
{
  auto *painter = dynamic_cast<CQChartsPlotPaintDevice *>(device)->painter();

  // common y different x axis
  if      (compositeType_ == CompositeType::X1X2) {
    int ix = 0, iy = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      CQChartsPlotPaintDevice device1(currentPlot(), painter);

      if (ix < 2) {
        if (plot->drawFgXAxis(&device1))
          ++ix;
      }

      if (iy < 1) {
        if (plot->drawFgYAxis(&device1))
          ++iy;
      }
    }
  }
  // common x different y axis
  else if (compositeType_ == CompositeType::Y1Y2) {
    int ix = 0, iy = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      CQChartsPlotPaintDevice device1(plot, painter);

      if (ix < 1) {
        if (plot->drawFgXAxis(&device1))
          ++ix;
      }

      if (iy < 2) {
        if (plot->drawFgYAxis(&device1))
          ++iy;
      }
    }
  }
  // tabbed
  else if (compositeType_ == CompositeType::TABBED) {
    auto *currentPlot = this->currentPlot();

    if (currentPlot) {
      CQChartsPlotPaintDevice device1(currentPlot, painter);

      currentPlot->drawFgAxes(&device1);
    }
  }
  // common x and y axis
  else {
    int ix = 0, iy = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      CQChartsPlotPaintDevice device1(plot, painter);

      if (ix < 2) {
        if (plot->drawFgXAxis(&device1))
          ++ix;
      }

      if (iy < 2) {
        if (plot->drawFgYAxis(&device1))
          ++iy;
      }
    }
  }
}

void
CQChartsCompositePlot::
drawTitle(PaintDevice *device) const
{
  auto *painter = dynamic_cast<CQChartsPlotPaintDevice *>(device)->painter();

  // for tabbed draw title for current plot
  if (compositeType_ == CompositeType::TABBED) {
    auto *currentPlot = this->currentPlot();

    if (currentPlot) {
      CQChartsPlotPaintDevice device1(currentPlot, painter);

      currentPlot->drawTitle(&device1);
    }
  }
  // otherwise draw on first visible plot
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      CQChartsPlotPaintDevice device1(plot, painter);

      plot->drawTitle(&device1);

      break;
    }
  }
}

void
CQChartsCompositePlot::
setOverlayAxisLabels()
{
  if (currentPlot()->yAxis())
    setPlotsAxisNames(plots_, currentPlot());
}

//------

bool
CQChartsCompositePlot::
addMenuItems(QMenu *menu)
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto *plotMenu = new QMenu(plot->id(), menu);

    if (plot->addMenuItems(plotMenu))
      menu->addMenu(plotMenu);
    else
      delete plotMenu;
  }

  //--

  auto addSubMenu = [](QMenu *menu, const QString &name) {
    auto *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto createActionGroup = [](QMenu *menu) {
    return new QActionGroup(menu);
  };

  auto addGroupCheckAction = [&](QActionGroup *group, const QString &name, bool checked,
                                 const char *slotName) {
    auto *menu = qobject_cast<QMenu *>(group->parent());

    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    connect(action, SIGNAL(triggered()), this, slotName);

    group->addAction(action);

    return action;
  };

  if (compositeType_ == CompositeType::NONE) {
    auto *currentPlotMenu = addSubMenu(menu, "Current Plot");

    auto *currentPlotGroup = createActionGroup(currentPlotMenu);

    int ind = 0;

    for (const auto &plot : plots_) {
      auto *plotAction =
        addGroupCheckAction(currentPlotGroup, plot->id(), false, SLOT(currentPlotSlot()));

      plotAction->setChecked(plot == currentPlot());

      plotAction->setData(ind++);
    }

    currentPlotMenu->addActions(currentPlotGroup->actions());
  }

  return true;
}

void
CQChartsCompositePlot::
zoomFull(bool notify)
{
  if (currentPlot())
    currentPlot()->zoomFull1(notify);
}

void
CQChartsCompositePlot::
currentPlotSlot()
{
  auto *action = qobject_cast<QAction *>(sender());

  bool ok;
  int ind = (int) action->data().toInt(&ok);
  if (! ok) return;

  setCurrentPlot(childPlot(ind));
}

//------

// add key items for specified child plot
void
CQChartsCompositePlot::
resetPlotKeyItems(Plot *plot)
{
  if (compositeType_ == CompositeType::TABBED) {
    // key for current plot
    auto *currentPlot = this->currentPlot();
    if (plot != currentPlot) return;

    CQChartsPlot::resetPlotKeyItems(currentPlot);
  }
  else {
    // shared key (add only for first visible (current) plot)
    auto *currentPlot = this->currentPlot();
    if (plot != currentPlot) return;

    auto *key = this->key();
    if (! key) return;

    key->clearItems();

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      plot->doAddKeyItems(key);
    }
  }
}

//------

bool
CQChartsCompositePlot::
handleSelectPress(const Point &w, SelMod selMod)
{
  if (tabbedSelectPress(w, selMod))
    return true;

  if (compositeType_ == CompositeType::TABBED) {
    auto *pressPlot = tabbedPressPlot(w, plots_);

    if (pressPlot) {
      setCurrentPlot(pressPlot);

      return true;
    }
  }

  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto w1 = plot->pixelToWindow(windowToPixel(w));

    if (plot->handleSelectPress(w1, selMod))
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
handleSelectMove(const Point &w, Constraints constraints, bool first)
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto w1 = plot->pixelToWindow(windowToPixel(w));

    if (plot->handleSelectMove(w1, constraints, first))
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
handleSelectRelease(const Point &w)
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto w1 = plot->pixelToWindow(windowToPixel(w));

    if (plot->handleSelectRelease(w1))
      return true;
  }

  return false;
}

//------

bool
CQChartsCompositePlot::
isLayerActive(const Layer::Type &type) const
{
  return CQChartsPlot::isLayerActive(type);
}

void
CQChartsCompositePlot::
invalidateOverlay()
{
  CQChartsPlot::execInvalidateOverlay();
}

void
CQChartsCompositePlot::
invalidateLayers()
{
  CQChartsPlot::execInvalidateLayers();
}

void
CQChartsCompositePlot::
invalidateLayer(const CQChartsBuffer::Type &layerType)
{
  CQChartsPlot::execInvalidateLayer(layerType);
}

//---

CQChartsGeom::BBox
CQChartsCompositePlot::
adjustDataRangeBBox(const BBox &bbox) const
{
  return (currentPlot() ? currentPlot()->adjustDataRangeBBox(bbox) :
                          CQChartsPlot::adjustDataRangeBBox(bbox));
}

//---

bool
CQChartsCompositePlot::
isComposite() const
{
  return currentPlot();
}

CQChartsPlot *
CQChartsCompositePlot::
currentPlot() const
{
  if (currentPlot_ && currentPlot_->isVisible())
    return currentPlot_;

  for (auto &plot : plots_) {
    if (plot->isVisible())
      return plot;
  }

  return nullptr;
}

void
CQChartsCompositePlot::
setCurrentPlot(CQChartsPlot *currentPlot)
{
  if (currentPlot != currentPlot_) {
    for (auto &plot : plots_)
      plot->setCurrent(plot == currentPlot);

    currentPlot_ = currentPlot;

    if (currentPlot_)
      currentPlot_->updateRangeAndObjs();

    updateRange();

    emit currentPlotChanged(currentPlot_);
    emit currentPlotIdChanged(currentPlot_ ? currentPlot_->id() : "");
  }
}

//---

bool
CQChartsCompositePlot::
hasTitle() const
{
  return (currentPlot() ? currentPlot()->hasTitle() : CQChartsPlot::hasTitle());
}

bool
CQChartsCompositePlot::
hasXAxis() const
{
  return (currentPlot() ? currentPlot()->hasXAxis() : CQChartsPlot::hasXAxis());
}

bool
CQChartsCompositePlot::
hasYAxis() const
{
  return (currentPlot() ? currentPlot()->hasYAxis() : CQChartsPlot::hasYAxis());
}

CQChartsTitle *
CQChartsCompositePlot::
title() const
{
  return (currentPlot() ? currentPlot()->title() : CQChartsPlot::title());
}

CQChartsAxis *
CQChartsCompositePlot::
xAxis() const
{
  return (currentPlot() ? currentPlot()->xAxis() : CQChartsPlot::xAxis());
}

CQChartsAxis *
CQChartsCompositePlot::
yAxis() const
{
  return (currentPlot() ? currentPlot()->yAxis() : CQChartsPlot::yAxis());
}

#if 0
CQChartsPlotKey *
CQChartsCompositePlot::
key() const
{
  return (currentPlot() ? currentPlot()->key() : CQChartsPlot::key());
}
#endif

//---

int
CQChartsCompositePlot::
childPlotIndex(const CQChartsPlot *childPlot) const
{
  int i = 0;

  for (auto &plot : plots_) {
    if (plot == childPlot)
      return i;

    ++i;
  }

  return -1;
}

int
CQChartsCompositePlot::
numChildPlots() const
{
  return plots_.size();
}

CQChartsPlot *
CQChartsCompositePlot::
childPlot(int i) const
{
  if (i < 0 || i >= int(plots_.size()))
    return nullptr;

  return plots_[i];
}

int
CQChartsCompositePlot::
currentPlotInd() const
{
  return childPlotIndex(currentPlot());
}

void
CQChartsCompositePlot::
setCurrentPlotInd(int i)
{
  setCurrentPlot(childPlot(i));
}

//---

void
CQChartsCompositePlot::
groupedObjsAtPoint(const Point &p, Objs &objs, const Constraints &constraints) const
{
  auto *currentPlot = this->currentPlot();

  if (currentPlot) {
    auto p1 = currentPlot->pixelToWindow(windowToPixel(p));

    currentPlot->groupedObjsAtPoint(p1, objs, constraints);
  }
}

void
CQChartsCompositePlot::
groupedObjsIntersectRect(const BBox &r, Objs &objs, bool inside, bool select) const
{
  auto *currentPlot = this->currentPlot();

  if (currentPlot) {
    auto r1 = currentPlot->pixelToWindow(windowToPixel(r));

    currentPlot->groupedObjsIntersectRect(r1, objs, inside, select);
  }
}

//---

bool
CQChartsCompositePlot::
keyPress(int key, int modifier)
{
  auto *currentPlot = this->currentPlot();

  if (currentPlot && currentPlot->keyPress(key, modifier))
    return true;

  return CQChartsPlot::keyPress(key, modifier);
}

//---

bool
CQChartsCompositePlot::
allowZoomX() const
{
  return (currentPlot() ? currentPlot()->allowZoomX() : false);
}

bool
CQChartsCompositePlot::
allowZoomY() const
{
  return (currentPlot() ? currentPlot()->allowZoomY() : false);
}

bool
CQChartsCompositePlot::
allowPanX() const
{
  return (currentPlot() ? currentPlot()->allowPanX() : false);
}

bool
CQChartsCompositePlot::
allowPanY() const
{
  return (currentPlot() ? currentPlot()->allowPanY() : false);
}

//---

double
CQChartsCompositePlot::
dataScaleX() const
{
  return (currentPlot() ? currentPlot()->dataScaleX() : 1.0);
}

void
CQChartsCompositePlot::
setDataScaleX(double r)
{
  if (currentPlot())
    currentPlot()->setDataScaleX(r);
}

double
CQChartsCompositePlot::
dataScaleY() const
{
  return (currentPlot() ? currentPlot()->dataScaleY() : 1.0);
}

void
CQChartsCompositePlot::
setDataScaleY(double r)
{
  if (currentPlot())
    currentPlot()->setDataScaleY(r);
}

double
CQChartsCompositePlot::
dataOffsetX() const
{
  return (currentPlot() ? currentPlot()->dataOffsetX() : 0.0);
}

void
CQChartsCompositePlot::
setDataOffsetX(double x)
{
  if (currentPlot())
    currentPlot()->setDataOffsetX(x);
}

double
CQChartsCompositePlot::
dataOffsetY() const
{
  return (currentPlot() ? currentPlot()->dataOffsetY() : 0.0);
}

void
CQChartsCompositePlot::
setDataOffsetY(double y)
{
  if (currentPlot())
    currentPlot()->setDataOffsetY(y);
}

//---

const CQChartsPlot::ZoomData &
CQChartsCompositePlot::
zoomData() const
{
  return (currentPlot() ? currentPlot()->zoomData() : zoomData_);
}

void
CQChartsCompositePlot::
setZoomData(const ZoomData &zoomData)
{
  if (currentPlot())
    currentPlot()->setZoomData(zoomData);
  else
    zoomData_ = zoomData;
}

//---

void
CQChartsCompositePlot::
pixelToWindowI(double px, double py, double &wx, double &wy) const
{
  if (currentPlot())
    currentPlot()->pixelToWindowI(px, py, wx, wy);
  else
    CQChartsPlot::pixelToWindowI(px, py, wx, wy);
}

void
CQChartsCompositePlot::
viewToWindowI(double vx, double vy, double &wx, double &wy) const
{
  if (currentPlot())
    currentPlot()->viewToWindowI(vx, vy, wx, wy);
  else
    CQChartsPlot::viewToWindowI(vx, vy, wx, wy);
}

void
CQChartsCompositePlot::
windowToPixelI(double wx, double wy, double &px, double &py) const
{
  if (currentPlot())
    currentPlot()->windowToPixelI(wx, wy, px, py);
  else
    CQChartsPlot::windowToPixelI(wx, wy, px, py);
}

void
CQChartsCompositePlot::
windowToViewI(double wx, double wy, double &vx, double &vy) const
{
  if (currentPlot())
    currentPlot()->windowToViewI(wx, wy, vx, vy);
  else
    CQChartsPlot::windowToViewI(wx, wy, vx, vy);
}

//---

CQChartsPlotCustomControls *
CQChartsCompositePlot::
createCustomControls()
{
  if (currentPlot())
    return currentPlot()->createCustomControls();

  return nullptr;
}
