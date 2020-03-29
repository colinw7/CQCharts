#include <CQChartsCompositePlot.h>
#include <CQChartsView.h>
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
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsCompositePlot(view, model);
}

//------

CQChartsCompositePlot::
CQChartsCompositePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("composite"), model)
{
  addAxes();

  addTitle();
}

CQChartsCompositePlot::
~CQChartsCompositePlot()
{
}

//---

void
CQChartsCompositePlot::
addPlot(CQChartsPlot *plot)
{
  plots_.push_back(plot);

  plot->setParentPlot(this);
}

//---

void
CQChartsCompositePlot::
addProperties()
{
  addBaseProperties();
}

CQChartsGeom::Range
CQChartsCompositePlot::
calcRange() const
{
  CQChartsGeom::Range dataRange;

  for (auto &plot : plots_) {
    dataRange += plot->calcRange();
  }

  if (! dataRange.isSet()) {
    dataRange.updateRange(0.0, 0.0);
    dataRange.updateRange(1.0, 1.0);
  }

  return dataRange;
}

bool
CQChartsCompositePlot::
createObjs(PlotObjs &) const
{
  for (auto &plot : plots_) {
    plot->createObjs();
  }

  return true;
}

CQChartsGeom::BBox
CQChartsCompositePlot::
calcAnnotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (auto &plot : plots_) {
    bbox += plot->calcAnnotationBBox();
  }

  return bbox;
}

void
CQChartsCompositePlot::
updateAxisRanges(const CQChartsGeom::BBox &adjustedRange)
{
  for (auto &plot : plots_) {
    plot->updateAxisRanges(adjustedRange);
  }

  CQChartsPlot::updateAxisRanges(adjustedRange);
}

//---

void
CQChartsCompositePlot::
waitRange()
{
  for (auto &plot : plots_)
    plot->execWaitRange();

  CQChartsPlot::execWaitRange();
}

void
CQChartsCompositePlot::
waitObjs()
{
  for (auto &plot : plots_)
    plot->execWaitObjs();

  CQChartsPlot::execWaitObjs();
}

void
CQChartsCompositePlot::
waitDraw()
{
  for (auto &plot : plots_)
    plot->execWaitDraw();

  CQChartsPlot::execWaitDraw();
}

//---

bool
CQChartsCompositePlot::
hasTitle() const
{
  for (auto &plot : plots_) {
    if (plot->hasTitle())
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
hasXAxis() const
{
  for (auto &plot : plots_) {
    if (plot->hasXAxis())
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
hasYAxis() const
{
  for (auto &plot : plots_) {
    if (plot->hasYAxis())
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
hasObjs(const CQChartsLayer::Type &layerType) const
{
  for (auto &plot : plots_) {
    if (plot->hasObjs(layerType))
      return true;
  }

  return false;
}

//------

void
CQChartsCompositePlot::
drawBackgroundDeviceParts(CQChartsPaintDevice *device, bool bgLayer, bool bgAxes,
                          bool bgKey) const
{
  for (auto &plot : plots_) {
    plot->drawBackgroundDeviceParts(device, bgLayer, /*bgAxes*/false, /*bgKey*/false);
  }

  if (bgAxes)
    drawGroupedBgAxes(device);

  if (bgKey)
    drawBgKey(device);
}

void
CQChartsCompositePlot::
drawMiddleDeviceParts(CQChartsPaintDevice *device, bool bg, bool mid,
                      bool fg, bool annotations) const
{
  for (auto &plot : plots_) {
    plot->drawMiddleDeviceParts(device, bg, mid, fg, annotations);
  }
}

void
CQChartsCompositePlot::
drawForegroundDeviceParts(CQChartsPaintDevice *device, bool fgAxes, bool fgKey,
                          bool title, bool foreground, bool tabbed) const
{
  if (fgAxes)
    drawGroupedFgAxes(device);

  if (fgKey)
    drawFgKey(device);

  if (title)
    drawTitle(device);

  for (auto &plot : plots_) {
    plot->drawForegroundDeviceParts(device, /*fgAxes*/false, /*fgKey*/false,
                                    /*title*/false, foreground, /*tabbed*/false);
  }

  //---

  if (tabbed)
    drawTabs(device);
}

void
CQChartsCompositePlot::
drawOverlayDeviceParts(CQChartsPaintDevice *device, bool sel_objs, bool sel_annotations,
                       bool boxes, bool edit_handles, bool over_objs, bool over_annotations) const
{
  for (auto &plot : plots_) {
    plot->drawOverlayDeviceParts(device, sel_objs, sel_annotations, boxes,
                                 edit_handles, over_objs, over_annotations);
  }
}

void
CQChartsCompositePlot::
drawBgAxes(CQChartsPaintDevice *device) const
{
#if 0
  for (auto &plot : plots_) {
    plot->drawBgAxes(device);
  }
#endif

  if (! plots_.empty())
    plots_[0]->drawBgAxes(device);
}

void
CQChartsCompositePlot::
drawFgAxes(CQChartsPaintDevice *device) const
{
#if 0
  for (auto &plot : plots_) {
    plot->drawFgAxes(device);
  }
#endif

  if (! plots_.empty())
    plots_[0]->drawFgAxes(device);
}

void
CQChartsCompositePlot::
drawTitle(CQChartsPaintDevice *device) const
{
#if 0
  for (auto &plot : plots_) {
    plot->drawTitle(device);
  }
#endif

  if (! plots_.empty())
    plots_[0]->drawTitle(device);
}

//------

bool
CQChartsCompositePlot::
addMenuItems(QMenu *menu)
{
  for (auto &plot : plots_) {
    QMenu *plotMenu = new QMenu(plot->id(), menu);

    if (plot->addMenuItems(plotMenu))
      menu->addMenu(plotMenu);
    else
      delete plotMenu;
  }

  return true;
}

//------

bool
CQChartsCompositePlot::
selectPress(const CQChartsGeom::Point &w, SelMod selMod)
{
  if (tabbedSelectPress(w, selMod))
    return true;

  for (auto &plot : plots_) {
    if (plot->selectPress(w, selMod))
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
selectMove(const CQChartsGeom::Point &w, bool first)
{
  for (auto &plot : plots_) {
    if (plot->selectMove(w, first))
      return true;
  }

  return false;
}

bool
CQChartsCompositePlot::
selectRelease(const CQChartsGeom::Point &w)
{
  for (auto &plot : plots_) {
    if (plot->selectRelease(w))
      return true;
  }

  return false;
}

//------

void
CQChartsCompositePlot::
invalidateOverlay()
{
  for (auto &plot : plots_) {
    plot->execInvalidateOverlay();
  }

  CQChartsPlot::execInvalidateOverlay();
}

void
CQChartsCompositePlot::
invalidateLayers()
{
  for (auto &plot : plots_) {
    plot->execInvalidateLayers();
  }

  CQChartsPlot::execInvalidateLayers();
}

void
CQChartsCompositePlot::
invalidateLayer(const CQChartsBuffer::Type &layerType)
{
  for (auto &plot : plots_) {
    plot->execInvalidateLayer(layerType);
  }

  CQChartsPlot::execInvalidateLayer(layerType);
}
