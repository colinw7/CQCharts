#include <CQChartsCompositePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>

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

void
CQChartsCompositePlot::
updatePlots()
{
  // common y different x axis
  if      (compositeType_ == CompositeType::X1X2) {
    int i = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (plot->xAxis())
        plot->xAxis()->setSide(i == 0 ? CQChartsAxisSide::Type::BOTTOM_LEFT :
                                        CQChartsAxisSide::Type::TOP_RIGHT);

      ++i;
    }
  }
  // common x different y axis
  else if (compositeType_ == CompositeType::Y1Y2) {
    int i = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (plot->yAxis())
        plot->yAxis()->setSide(i == 0 ? CQChartsAxisSide::Type::BOTTOM_LEFT :
                                        CQChartsAxisSide::Type::TOP_RIGHT);

      ++i;
    }
  }
  else {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      if (plot->xAxis())
        plot->xAxis()->setSide(CQChartsAxisSide::Type::BOTTOM_LEFT);

      if (plot->yAxis())
        plot->yAxis()->setSide(CQChartsAxisSide::Type::BOTTOM_LEFT);
    }
  }
}

//---

void
CQChartsCompositePlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  addProp("", "compositeType" , "compositeType", "Composite Type");
  addProp("", "commonXRange"  , "commonXRange" , "Common X Range");
  addProp("", "commonYRange"  , "commonYRange" , "Common Y Range");
  addProp("", "currentPlotInd", "currentPlot"  , "Current Plot Index");

  addBaseProperties();
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
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->postUpdateObjs();
  }
}

//---

bool
CQChartsCompositePlot::
hasPlotObjs() const
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    return plot->hasPlotObjs();
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

void
CQChartsCompositePlot::
clearPlotObjects()
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->clearPlotObjects1();
  }

  clearPlotObjects1();
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

  invalidateObjTree1();
}

bool
CQChartsCompositePlot::
updateInsideObjects(const Point &w, Constraints constraints)
{
  bool rc = false;

  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto w1 = plot->pixelToWindow(windowToPixel(w));

    if (! plot->updateInsideObjects1(w1, constraints))
      rc = false;
  }

  if (! updateInsideObjects1(w, constraints))
    rc = false;

  return rc;
}

CQChartsPlot::Obj *
CQChartsCompositePlot::
insideObject() const
{
#if 0
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    return plot->insideObject1();
  }
#endif

  return insideObject1();
}

QString
CQChartsCompositePlot::
insideObjectText() const
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    return plot->insideObjectText1();
  }

  return insideObjectText1();
}

void
CQChartsCompositePlot::
initPlotObjs()
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->initPlotObjs();
  }

  CQChartsPlot::initPlotObjs();
}

void
CQChartsCompositePlot::
initObjTree()
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    plot->execInitObjTree();
  }

  CQChartsPlot::execInitObjTree();
}

CQChartsGeom::BBox
CQChartsCompositePlot::
calcExtraFitBBox() const
{
  BBox bbox;

  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    bbox += plot->calcExtraFitBBox();
  }

  return bbox;
}

void
CQChartsCompositePlot::
updateAxisRanges(const BBox &adjustedRange)
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->updateAxisRanges(adjustedRange);
  }

  CQChartsPlot::updateAxisRanges(adjustedRange);
}

//---

CQChartsGeom::BBox
CQChartsCompositePlot::
adjustedViewBBox(const CQChartsPlot *plot) const
{
  auto bbox = plot->viewBBox();

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
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    plot->execWaitRange();
  }

  CQChartsPlot::execWaitRange();
}

void
CQChartsCompositePlot::
waitObjs()
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    plot->execWaitObjs();
  }

  CQChartsPlot::execWaitObjs();
}

void
CQChartsCompositePlot::
waitDraw()
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    plot->execWaitDraw();
  }

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

  CQChartsPlot::execWaitTree();
}

//---

bool
CQChartsCompositePlot::
hasBackground() const
{
  return checkCurrentPlots([&](const Plot *plot) { return plot->hasBackground(); });
}

bool
CQChartsCompositePlot::
hasForeground() const
{
  return checkCurrentPlots([&](const Plot *plot) { return plot->hasForeground(); });
}

bool
CQChartsCompositePlot::
hasTitle() const
{
  return checkCurrentPlots([&](const Plot *plot) { return plot->hasTitle(); });
}

bool
CQChartsCompositePlot::
hasXAxis() const
{
  return checkCurrentPlots([&](const Plot *plot) { return plot->hasXAxis(); });
}

bool
CQChartsCompositePlot::
hasYAxis() const
{
  return checkCurrentPlots([&](const Plot *plot) { return plot->hasYAxis(); });
}

bool
CQChartsCompositePlot::
hasObjs(const CQChartsLayer::Type &layerType) const
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    if (plot->hasObjs(layerType))
      return true;
  }

  return false;
}

//------

bool
CQChartsCompositePlot::
isBufferLayers() const
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    return plot->isBufferLayers();
  }

  return CQChartsPlot::isBufferLayers();
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
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    BackgroundParts bgParts1 = bgParts;

    bgParts1.axes = false;
    bgParts1.key  = false;

    plot->drawBackgroundDeviceParts(device, bgParts1);
  }

  if (bgParts.axes)
    drawGroupedBgAxes(device);

  if (bgParts.key)
    drawBgKey(device);
}

void
CQChartsCompositePlot::
drawMiddleDeviceParts(PaintDevice *device, bool bg, bool mid, bool fg) const
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->drawMiddleDeviceParts(device, bg, mid, fg);
  }
}

void
CQChartsCompositePlot::
drawForegroundDeviceParts(PaintDevice *device, const ForegroundParts &fgParts) const
{
  if (fgParts.axes)
    drawGroupedFgAxes(device);

  if (fgParts.key)
    drawFgKey(device);

  if (fgParts.title)
    drawTitle(device);

  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    ForegroundParts fgParts1 = fgParts;

    fgParts1.axes   = false;
    fgParts1.key    = false;
    fgParts1.title  = false;
    fgParts1.tabbed = false;

    plot->drawForegroundDeviceParts(device, fgParts1);
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
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->drawOverlayDeviceParts(device, overlayParts);
  }
}

void
CQChartsCompositePlot::
drawBgAxes(PaintDevice *device) const
{
  // common y different x axis
  if      (compositeType_ == CompositeType::X1X2) {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      plot->drawBgAxes(device);
    }
  }
  // common x different y axis
  else if (compositeType_ == CompositeType::Y1Y2) {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      plot->drawBgAxes(device);
    }
  }
  else {
#if 0
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      plot->drawBgAxes(device);
    }
#endif

    if (currentPlot() && currentPlot()->isVisible()) {
      const_cast<CQChartsCompositePlot *>(this)->setOverlayAxisLabels();

      currentPlot()->drawBgAxes(device);
    }
  }
}

void
CQChartsCompositePlot::
drawFgAxes(PaintDevice *device) const
{
  // common y different x axis
  if      (compositeType_ == CompositeType::X1X2) {
    int i = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      plot->drawFgAxes(device);

      ++i;

      if (i == 2)
        break;
    }
  }
  // common x different y axis
  else if (compositeType_ == CompositeType::Y1Y2) {
    int i = 0;

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      plot->drawFgAxes(device);

      ++i;

      if (i == 2)
        break;
    }
  }
  else {
#if 0
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      plot->drawFgAxes(device);
    }
#endif

    if (currentPlot() && currentPlot()->isVisible()) {
      const_cast<CQChartsCompositePlot *>(this)->setOverlayAxisLabels();

      currentPlot()->drawFgAxes(device);
    }
  }
}

void
CQChartsCompositePlot::
drawTitle(PaintDevice *device) const
{
#if 0
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->drawTitle(device);
  }
#endif

  if (currentPlot() && currentPlot()->isVisible())
    currentPlot()->drawTitle(device);
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
currentPlotSlot()
{
  auto *action = qobject_cast<QAction *>(sender());

  bool ok;
  int ind = (int) action->data().toInt(&ok);
  if (! ok) return;

  setCurrentPlot(childPlot(ind));
}

//------

void
CQChartsCompositePlot::
resetKeyItems()
{
  if (compositeType_ == CompositeType::TABBED) {
    // key per plot
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      plot->resetKeyItems();
    }
  }
  else {
    // shared key
    if (! key())
      return;

    key()->clearItems();

    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      plot->doAddKeyItems(key());
    }
  }
}

//------

bool
CQChartsCompositePlot::
selectPress(const Point &w, SelMod selMod)
{
  if (tabbedSelectPress(w, selMod))
    return true;

  if (compositeType_ == CompositeType::TABBED) {
    auto *pressPlot = tabbedPressPlot(w, plots_);

    if (pressPlot) {
      setCurrentPlot(pressPlot);

      pressPlot->updateRangeAndObjs();

      return true;
    }
  }

  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto w1 = plot->pixelToWindow(windowToPixel(w));

    if (plot->selectPress(w1, selMod))
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
selectMove(const Point &w, Constraints constraints, bool first)
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto w1 = plot->pixelToWindow(windowToPixel(w));

    if (plot->selectMove(w1, constraints, first))
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
selectRelease(const Point &w)
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    auto w1 = plot->pixelToWindow(windowToPixel(w));

    if (plot->selectRelease(w1))
      return true;
  }

  return false;
}

//------

bool
CQChartsCompositePlot::
isLayerActive(const Layer::Type &type) const
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    if (plot->isLayerActive1(type))
      return true;
  }

  return false;
}

void
CQChartsCompositePlot::
invalidateOverlay()
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->execInvalidateOverlay();
  }

  CQChartsPlot::execInvalidateOverlay();
}

void
CQChartsCompositePlot::
invalidateLayers()
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->execInvalidateLayers();
  }

  CQChartsPlot::execInvalidateLayers();
}

void
CQChartsCompositePlot::
invalidateLayer(const CQChartsBuffer::Type &layerType)
{
  for (auto &plot : plots_) {
    if (! plot->isVisible())
      continue;

    if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
      continue;

    plot->execInvalidateLayer(layerType);
  }

  CQChartsPlot::execInvalidateLayer(layerType);
}

//---

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

    updateRange();

    emit currentPlotChanged(currentPlot_);
    emit currentPlotIdChanged(currentPlot_ ? currentPlot_->id() : "");
  }
}

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

CQChartsPlotCustomControls *
CQChartsCompositePlot::
createCustomControls(CQCharts *charts)
{
  if (currentPlot())
    return currentPlot()->createCustomControls(charts);

  return nullptr;
}
