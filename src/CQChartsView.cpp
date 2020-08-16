#include <CQChartsView.h>
#include <CQChartsWindow.h>
#include <CQChartsViewToolTip.h>
#include <CQChartsProbeBand.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAnnotation.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQChartsEnv.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsVariant.h>
#include <CQChartsInterfaceTheme.h>
#include <CQChartsHtml.h>
#include <CQChartsJS.h>

#include <CQChartsCreatePlotDlg.h>
#include <CQChartsEditAnnotationDlg.h>
#include <CQChartsHelpDlg.h>

#include <CQChartsAxisEdit.h>
#include <CQChartsKeyEdit.h>
#include <CQChartsTitleEdit.h>
#include <CQChartsEditHandles.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsSVGPaintDevice.h>
#include <CQChartsDocument.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColors.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>

#include <CQPerfMonitor.h>

#include <QSvgGenerator>
#include <QFileDialog>
#include <QRubberBand>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMenu>

#include <svg/models_light_svg.h>
#include <svg/models_dark_svg.h>
#include <svg/charts_light_svg.h>
#include <svg/charts_dark_svg.h>
#include <svg/info_svg.h>

#include <fstream>

namespace {
  CQChartsSelMod modifiersToSelMod(Qt::KeyboardModifiers modifiers) {
    if      ((modifiers & Qt::ControlModifier) && (modifiers & Qt::ShiftModifier))
      return CQChartsSelMod::TOGGLE;
    else if (modifiers & Qt::ControlModifier)
      return CQChartsSelMod::ADD;
    else if (modifiers & Qt::ShiftModifier)
      return CQChartsSelMod::REMOVE;
    else
      return CQChartsSelMod::REPLACE;
  }

  CQChartsSelMod modifiersToClickMod(Qt::KeyboardModifiers modifiers) {
    if      ((modifiers & Qt::ControlModifier) && (modifiers & Qt::ShiftModifier))
      return CQChartsSelMod::ADD;
    else if (modifiers & Qt::ControlModifier)
      return CQChartsSelMod::REPLACE;
    else if (modifiers & Qt::ShiftModifier)
      return CQChartsSelMod::REMOVE;
    else
      return CQChartsSelMod::TOGGLE;
  }
}

//---

QString
CQChartsView::
description()
{
  return CQChartsHtml().
   h2("View").
   p("A view is a container for one or more plots").
   p("Plots can be placed side by side or in a grid or overlaid with shared x and/or "
     "coordinates.");
}

//---

QSize CQChartsView::defSizeHint_ = QSize(1280, 1024);

CQChartsView::
CQChartsView(CQCharts *charts, QWidget *parent) :
 QFrame(parent),
 CQChartsObjBackgroundFillData(this),
 CQChartsObjSelectedShapeData (this),
 CQChartsObjInsideShapeData   (this),
 charts_(charts),
 viewSizeHint_(defSizeHint_)
{
  setObjectName("view");

  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setBackgroundFillColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.0));

  //---

  bufferLayers_ = CQChartsEnv::getBool("CQ_CHARTS_BUFFER_LAYERS", bufferLayers_);

  objectsBuffer_ = new CQChartsBuffer;
  overlayBuffer_ = new CQChartsBuffer;

  //---

  displayRange_ = new CQChartsDisplayRange;

  double vr = viewportRange();

  displayRange_->setWindowRange(0, 0, vr, vr);

  //---

  propertyModel_ = new CQPropertyViewModel;

  //---

  if (charts_->hasViewKey()) {
    keyObj_ = new CQChartsViewKey(this);

    keyObj_->setVisible(false);
  }

  //---

  setSelectedMode(HighlightDataMode::FILL);

  setSelectedFillAlpha(CQChartsAlpha(0.8));
  setInsideFillAlpha  (CQChartsAlpha(0.8));

  //---

  lightPaletteSlot();

  themeSlot("default");

  //---

  font_.setFont(QFont());

  //---

  addProperties();

  //---

  CQToolTip::setToolTip(this, new CQChartsViewToolTip(this));

  //---

  searchTimeout_ = CQChartsEnv::getInt("CQ_CHARTS_SEARCH_TIMEOUT", searchTimeout_);

  setSearchTimeout(searchTimeout_);

  //---

  connect(charts_, SIGNAL(modelDataChanged()), this, SLOT(updateNoData()));

  //---

  connect(CQColorsMgrInst, SIGNAL(themeChanged(const QString &)),
          this, SLOT(themeChangedSlot(const QString &)));
  connect(CQColorsMgrInst, SIGNAL(paletteChanged(const QString &)),
          this, SLOT(paletteChangedSlot(const QString &)));

  // TODO: only connect to current theme ?
  connect(CQColorsMgrInst, SIGNAL(themesChanged()), this, SLOT(updatePlots()));
  connect(CQColorsMgrInst, SIGNAL(palettesChanged()), this, SLOT(updatePlots()));
}

CQChartsView::
~CQChartsView()
{
  if (charts_)
    charts_->removeView(this);

  //---

  delete objectsBuffer_;
  delete overlayBuffer_;

  delete ipainter_;
  delete image_;

  delete keyObj_;

  delete displayRange_;

  for (auto &plot : plots_)
    delete plot;

  for (auto &annotation : annotations())
    delete annotation;

  for (auto &probeBand : probeBands_)
    delete probeBand;

  delete propertyModel_;

  delete popupMenu_;

  CQToolTip::unsetToolTip(this);
}

void
CQChartsView::
setWindow(CQChartsWindow *window)
{
  window_ = window;

  connect(window_, SIGNAL(expansionChanged()), this, SLOT(expansionChangeSlot()));
}

void
CQChartsView::
setId(const QString &s)
{
  id_ = s;

  if (! title_.length())
    setWindowTitle(id_);
}

void
CQChartsView::
setTitle(const QString &s)
{
  CQChartsUtil::testAndSet(title_, s, [&]() { setWindowTitle(title_); } );
}

//---

void
CQChartsView::
expansionChangeSlot()
{
  auto *plot = currentPlot(/*remap*/false);

  if (plot)
    plot->modelViewExpansionChanged();
}

void
CQChartsView::
expandedModelIndices(QModelIndexList &inds)
{
  if (window())
    window()->expandedModelIndices(inds);
}

//---

void
CQChartsView::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                    const QString &desc, bool hidden=false) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden=false) {
    auto *item = addProp(path, name, alias, desc, hidden);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  // data
  addProp("", "mode"          , "", "View mouse mode" )->setHidden(true);
  addProp("", "id"            , "", "View id"         )->setHidden(true);
  addProp("", "currentPlotInd", "", "Current plot ind")->setHidden(true);

  addProp("", "viewSizeHint", "", "View size hint")->setHidden(true);
  addProp("", "bufferLayers", "", "Buffer layer"  )->setHidden(true);

  addProp("", "showTable"   , "", "Show table of value")->setHidden(true);
  addProp("", "showSettings", "", "Show settings panel")->setHidden(true);

  // options
  addStyleProp("options", "antiAlias", "", "Draw aliased shapes", true);

  // title
  addProp("title", "title", "string", "View title string");

  // theme
  addStyleProp("theme", "theme", "name", "View theme")->
    setValues(QStringList() << "default" << "theme1" << "theme2");
  addStyleProp("theme", "dark" , "dark", "View interface is dark");

  // coloring
  addStyleProp("coloring", "defaultPalette", "defaultPalette", "Default palette");

  // text
  addStyleProp("text", "scaleFont" , "scaled", "Scale font to view size");
  addStyleProp("text", "fontFactor", "factor", "Global text scale factor")->
    setMinValue(0.001);
  addStyleProp("text", "font"      , "font"  , "Global text font");

  // handdrawn
  addStyleProp("handdrawn", "handDrawn"    , "enabled"  , "Enable handdraw painter");
  addStyleProp("handdrawn", "handRoughness", "roughness", "Handdraw roughness");
  addStyleProp("handdrawn", "handFillDelta", "fillDelta", "Handdraw fill delta");

  // sizing
  addProp("sizing", "autoSize" , "auto"     , "Auto scale to view size");
  addProp("sizing", "fixedSize", "fixedSize", "Fixed view size");

  // background fill
  addStyleProp("background/fill", "backgroundFillData"   , "style"  , "Fill style"  , true);
  addStyleProp("background/fill", "backgroundFillColor"  , "color"  , "Fill color"  );
  addStyleProp("background/fill", "backgroundFillAlpha"  , "alpha"  , "Fill alpha"  , true);
  addStyleProp("background/fill", "backgroundFillPattern", "pattern", "Fill pattern", true);

  // select mode
  addProp("select", "selectMode"  , "mode"  , "Selection mode");
  addProp("select", "selectInside", "inside", "Select when fully inside select rectangle");

  // select highlight
  addStyleProp("select/highlight"       , "selectedMode"       , "mode"   ,
               "Highlight draw mode");
  addStyleProp("select/highlight"       , "selectedShapeData"  , "style"  ,
               "Highlight shape data");
  addStyleProp("select/highlight/fill"  , "selectedFilled"     , "visible",
               "Highlight fill visible");
  addStyleProp("select/highlight/fill"  , "selectedFillColor"  , "color"  ,
               "Highlight fill color");
  addStyleProp("select/highlight/fill"  , "selectedFillAlpha"  , "alpha"  ,
               "Highlight fill alpha");
  addStyleProp("select/highlight/fill"  , "selectedFillPattern", "pattern",
               "Highlight fill pattern");
  addStyleProp("select/highlight/stroke", "selectedStroked"    , "visible",
               "Highlight stroke visible");
  addStyleProp("select/highlight/stroke", "selectedStrokeColor", "color"  ,
               "Highlight stroke color");
  addStyleProp("select/highlight/stroke", "selectedStrokeWidth", "width"  ,
               "Highlight stroke width");
  addStyleProp("select/highlight/stroke", "selectedStrokeDash" , "dash"   ,
               "Highlight stroke dash");

  // region mode
  addProp("region", "regionMode", "mode", "Region mode", true);

  // inside highlight
  addStyleProp("inside/highlight"       , "insideMode"       , "mode"   , "Inside draw mode");
  addStyleProp("inside/highlight"       , "insideShapeData"  , "style"  , "Inside shape data");
  addStyleProp("inside/highlight/fill"  , "insideFilled"     , "visible", "Inside fill visible");
  addStyleProp("inside/highlight/fill"  , "insideFillColor"  , "color"  , "Inside fill color");
  addStyleProp("inside/highlight/fill"  , "insideFillAlpha"  , "alpha"  , "Inside fill alpha");
  addStyleProp("inside/highlight/fill"  , "insideFillPattern", "pattern", "Inside fill pattern");
  addStyleProp("inside/highlight/stroke", "insideStroked"    , "visible", "Inside stroke visible");
  addStyleProp("inside/highlight/stroke", "insideStrokeColor", "color"  , "Inside stroke color");
  addStyleProp("inside/highlight/stroke", "insideStrokeWidth", "width"  , "Inside stroke width");
  addStyleProp("inside/highlight/stroke", "insideStrokeDash" , "dash"   , "Inside stroke dash");

  // status
  addProp("status", "posTextType", "posTextType", "Position text type")->setHidden(true);

  // TODO: remove or make more general
  addProp("scroll", "scrolled"      , "enabled" , "Scrolling enabled"     )->setHidden(true);
  addProp("scroll", "scrollDelta"   , "delta"   , "Scroll delta"          )->setHidden(true);
  addProp("scroll", "scrollNumPages", "numPages", "Scroll number of pages")->setHidden(true);
  addProp("scroll", "scrollPage"    , "page"    , "Scroll current page"   )->setHidden(true);

  if (key())
    key()->addProperties(propertyModel(), "key");
}

//---

void
CQChartsView::
maximizePlotsSlot()
{
  auto *plot = currentPlot(/*remap*/false);

  maximizePlot(plot);
}

void
CQChartsView::
maximizePlot(CQChartsPlot *plot)
{
  scrollData_.autoInit = true;

  setScrolled(true, /*update*/false);

  PlotSet basePlots;

  addBasePlots(basePlots);

  int i = 0;

  for (auto &basePlot : basePlots) {
    if (basePlot == plot) {
      setScrollPage(i);
      break;
    }

    ++i;
  }

  updateScroll();
}

void
CQChartsView::
restorePlotsSlot()
{
  setScrolled(false);
}

void
CQChartsView::
setScrolled(bool b, bool update)
{
  if (b == scrollData_.active)
    return;

  scrollData_.active = b;

  if (scrollData_.autoInit) {
    PlotSet basePlots;

    addBasePlots(basePlots);

    if (scrollData_.active) {
      scrollData_.plotBBoxMap.clear();

      for (auto &plot : basePlots)
        scrollData_.plotBBoxMap[plot->id()] = plot->calcViewBBox();

      int pos = 0;

      for (auto &plot : basePlots) {
        plot->setViewBBox(BBox(pos, 0, pos + 100, 100));

        pos += 100;
      }

      scrollData_.numPages = basePlots.size();
    }
    else {
      bool allFound = true;

      for (auto &plot : basePlots) {
        auto p = scrollData_.plotBBoxMap.find(plot->id());

        if (p == scrollData_.plotBBoxMap.end()) {
          allFound = false;
          continue;
        }

        plot->setViewBBox((*p).second);
      }

      if (! allFound)
        placePlots(plots(), /*vertical*/false, /*horizontal*/true, /*rows*/1, /*cols*/1);
    }

    if (update)
      updateScroll();
  }

  emit scrollDataChanged();
}

//---

void
CQChartsView::
setAntiAlias(bool b)
{
  CQChartsUtil::testAndSet(antiAlias_, b, [&]() { updatePlots(); } );
}

void
CQChartsView::
setBufferLayers(bool b)
{
  CQChartsUtil::testAndSet(bufferLayers_, b, [&]() { updatePlots(); } );
}

void
CQChartsView::
setPreview(bool b)
{
  CQChartsUtil::testAndSet(preview_, b, [&]() { updatePlots(); } );
}

//---

void
CQChartsView::
setAutoSize(bool b)
{
  // init to current size if turn off for first time
  if (! b && sizeData_.autoSize) {
    sizeData_.width   = width ();
    sizeData_.height  = height();
    sizeData_.sizeSet = true;
  }

  CQChartsUtil::testAndSet(sizeData_.autoSize, b, [&]() { resizeEvent(nullptr); } );
}

void
CQChartsView::
setFixedSize(const QSize &size)
{
  bool resize = false;

  CQChartsUtil::testAndSet(sizeData_.width , size.width (), [&]() { resize = true; } );
  CQChartsUtil::testAndSet(sizeData_.height, size.height(), [&]() { resize = true; } );

  sizeData_.sizeSet = true;

  if (resize) {
    resizeEvent(nullptr);
  }
}

void
CQChartsView::
setScaleFont(bool b)
{
  CQChartsUtil::testAndSet(scaleFont_, b, [&]() { updatePlots(); } );
}

void
CQChartsView::
setFontFactor(double r)
{
  CQChartsUtil::testAndSet(fontFactor_, r, [&]() { updatePlots(); } );
}

void
CQChartsView::
setFont(const CQChartsFont &f)
{
  CQChartsUtil::testAndSet(font_, f, [&]() { updatePlots(); } );
}

double
CQChartsView::
fontEm() const
{
  QFont qfont = this->font().font();
  QFontMetricsF fm(qfont);

  return fm.height();
}

double
CQChartsView::
fontEx() const
{
  QFont qfont = this->font().font();
  QFontMetricsF fm(qfont);

  return fm.width("x");
}

//---

void
CQChartsView::
setHandDrawn(bool b)
{
  CQChartsUtil::testAndSet(handDrawn_, b, [&]() {
    if (handDrawn_) {
      saveFont_ = font_;

      QFont f = font_.font();

      f.setFamily("purisa");

      font_ = CQChartsFont(f);
    }
    else {
      font_ = saveFont_;
    }

    updateAll();
  } );
}

void
CQChartsView::
setHandRoughness(double r)
{
  CQChartsUtil::testAndSet(handRoughness_, r, [&]() { updateAll(); } );
}

void
CQChartsView::
setHandFillDelta(double r)
{
  CQChartsUtil::testAndSet(handFillDelta_, r, [&]() { updateAll(); } );
}

//---

void
CQChartsView::
setSearchTimeout(int i)
{
  searchTimeout_ = i;

  if (searchTimeout_ > 0) {
    if (! searchTimer_) {
      searchTimer_ = new QTimer(this);

      searchTimer_->setSingleShot(true);

      connect(searchTimer_, SIGNAL(timeout()), this, SLOT(searchSlot()));
    }

    searchTimer_->setInterval(searchTimeout_);
  }
  else {
    delete searchTimer_;

    searchTimer_ = nullptr;
  }
}

//---

void
CQChartsView::
setDefaultPalette(const QString &s)
{
  CQChartsUtil::testAndSet(defaultPalette_, s, [&]() { updatePlots(); } );
}

//---

void
CQChartsView::
setPosTextType(const PosTextType &t)
{
  CQChartsUtil::testAndSet(posTextType_, t, [&]() { updatePlots(); } );
}

//---

void
CQChartsView::
setPainterFont(CQChartsPaintDevice *device, const CQChartsFont &font) const
{
  device->setFont(viewFont(font));
}

void
CQChartsView::
setPlotPainterFont(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                   const CQChartsFont &font) const
{
  device->setFont(plotFont(plot, font));
}

QFont
CQChartsView::
viewFont(const CQChartsFont &font) const
{
  // Calc specified font from view font
  QFont font1 = font.calcFont(font_.font());

  if (isScaleFont())
    return scaledFont(font1, Size(this->size()));
  else
    return font1;
}

QFont
CQChartsView::
viewFont(const QFont &font) const
{
  if (isScaleFont())
    return scaledFont(font, Size(this->size()));
  else
    return font;
}

QFont
CQChartsView::
plotFont(const CQChartsPlot *plot, const CQChartsFont &font, bool scaled) const
{
  // adjust font by plot font and then by view font
  CQChartsFont font1 = font.calcFont(plot->font());
  QFont        font2 = font1.calcFont(font_.font());

  if (scaled && isScaleFont())
    return scaledFont(font2, plot->calcPixelSize());
  else
    return font2;
}

QFont
CQChartsView::
plotFont(const CQChartsPlot *plot, const QFont &font, bool scaled) const
{
  if (scaled && isScaleFont())
    return scaledFont(font, plot->calcPixelSize());
  else
    return font;
}

double
CQChartsView::
calcFontScale(const Size &size) const
{
  // calc scale factor
  double sx = (size.width () > 0 ? size.width ()/defSizeHint().width () : 1.0);
  double sy = (size.height() > 0 ? size.height()/defSizeHint().height() : 1.0);

  return std::max(fontFactor()*std::max(sx, sy), 0.01);
}

QFont
CQChartsView::
scaledFont(const QFont &font, const Size &size) const
{
  double s = calcFontScale(size);

  return scaledFont(font, s);
}

QFont
CQChartsView::
scaledFont(const QFont &font, double s) const
{
  return CQChartsUtil::scaleFontSize(font, s);
}

//---

void
CQChartsView::
setCurrentPlot(CQChartsPlot *plot)
{
  if (plot)
    setCurrentPlotInd(getIndForPlot(plot));
  else
    setCurrentPlotInd(-1);
}

void
CQChartsView::
setCurrentPlotInd(int ind)
{
  if (ind >= 0 && ind == currentPlotInd_)
    return;

  // disconnect previous plot
  auto *currentPlot = getPlotForInd(currentPlotInd_);

  if (currentPlot)
    disconnect(currentPlot, SIGNAL(zoomPanChanged()), this, SLOT(currentPlotZoomPanChanged()));

  //---

  // set and connect to new plot
  if (ind < 0)
    ind = calcCurrentPlotInd(/*remap*/ true);

  currentPlotInd_ = ind;

  currentPlot = getPlotForInd(currentPlotInd_);

  if (currentPlot)
    connect(currentPlot, SIGNAL(zoomPanChanged()), this, SLOT(currentPlotZoomPanChanged()));

  //---

  if (isScrolled()) {
    PlotSet basePlots;

    addBasePlots(basePlots);

    int i = 0;

    for (auto &basePlot : basePlots) {
      if (basePlot == currentPlot) {
        setScrollPage(i);
        break;
      }

      ++i;
    }

    updateScroll();
  }

  //---

  emit currentPlotChanged();
}

void
CQChartsView::
currentPlotZoomPanChanged()
{
  if (window())
    window()->updateRangeMap();
}

//---

void
CQChartsView::
selectModeSlot()
{
  setMode(Mode::SELECT);
}

void
CQChartsView::
zoomModeSlot()
{
  setMode(Mode::ZOOM_IN);
}

void
CQChartsView::
panModeSlot()
{
  setMode(Mode::PAN);
}

void
CQChartsView::
probeModeSlot()
{
  setMode(Mode::PROBE);
}

void
CQChartsView::
queryModeSlot()
{
  setMode(Mode::QUERY);
}

void
CQChartsView::
editModeSlot()
{
  setMode(Mode::EDIT);
}

void
CQChartsView::
regionModeSlot()
{
  setMode(Mode::REGION);
}

void
CQChartsView::
setMode(const Mode &mode)
{
  CQChartsUtil::testAndSet(mode_, mode, [&]() {
    endRegionBand();

    for (auto &probeBand : probeBands_)
      probeBand->hide();

    deselectAll();

    emit modeChanged();
  } );
}

void
CQChartsView::
setSelectMode(const SelectMode &selectMode)
{
  CQChartsUtil::testAndSet(selectData_.mode, selectMode, [&]() { emit selectModeChanged(); } );
}

void
CQChartsView::
selectOneObj(CQChartsObj *obj)
{
  startSelection();

  deselectAll();

  obj->setSelected(true);

  endSelection();

  invalidateOverlay();
}

void
CQChartsView::
deselectAll()
{
  startSelection();

  //---

  // deselect plots and their objects
  for (auto &plot : plots())
    plot->deselectAll();

  //---

  // deselect view annotations
  bool changed = false;

  for (auto &annotation : annotations()) {
    if (annotation->isSelected()) {
      annotation->setSelected(false);

      changed = true;
    }
  }

  // deselect key
  if (key() && key()->isSelected()) {
    key()->setSelected(false);

    changed = true;
  }

  if (changed)
    invalidateOverlay();

  endSelection();
}

void
CQChartsView::
startSelection()
{
  ++selectData_.selectDepth;
}

void
CQChartsView::
endSelection()
{
  --selectData_.selectDepth;

  if (selectData_.selectDepth == 0) {
    emit selectionChanged();

    updateSelText();
  }
}

//---

void
CQChartsView::
setRegionMode(const RegionMode &regionMode)
{
  CQChartsUtil::testAndSet(regionData_.mode, regionMode, [&]() { emit regionModeChanged(); } );
}

//---

CQColorsTheme *
CQChartsView::
theme()
{
  return themeName().obj();
}

const CQColorsTheme *
CQChartsView::
theme() const
{
  return themeName().obj();
}

const CQChartsThemeName &
CQChartsView::
themeName() const
{
  return charts()->plotTheme();
}

void
CQChartsView::
setThemeName(const CQChartsThemeName &themeName)
{
  charts()->setPlotTheme(themeName);

  updateTheme();
}

CQColorsPalette *
CQChartsView::
interfacePalette() const
{
  return charts()->interfaceTheme()->palette();
}

CQColorsPalette *
CQChartsView::
themeGroupPalette(int i, int /*n*/) const
{
  return theme()->palette(i);
}

CQColorsPalette *
CQChartsView::
themePalette(int ind) const
{
  return theme()->palette(ind);
}

//---

bool
CQChartsView::
setProperties(const QString &properties)
{
  bool rc = true;

  CQChartsNameValues nameValues(properties);

  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    const auto &value = nv.second;

    if (! setProperty(name, value))
      rc = false;
  }

  return rc;
}

bool
CQChartsView::
setProperty(const QString &name, const QVariant &value)
{
  return propertyModel()->setProperty(this, name, value);
}

bool
CQChartsView::
getProperty(const QString &name, QVariant &value) const
{
  return propertyModel()->getProperty(this, name, value);
}

bool
CQChartsView::
getTclProperty(const QString &name, QVariant &value) const
{
  return propertyModel()->getTclProperty(this, name, value);
}

bool
CQChartsView::
getPropertyDesc(const QString &name, QString &desc, bool hidden) const
{
  const auto *item = propertyItem(name, hidden);
  if (! item) return false;

  desc = item->desc();

  return true;
}

bool
CQChartsView::
getPropertyType(const QString &name, QString &type, bool hidden) const
{
  const auto *item = propertyItem(name, hidden);
  if (! item) return false;

  type = item->typeName();

  return true;
}

bool
CQChartsView::
getPropertyUserType(const QString &name, QString &type, bool hidden) const
{
  const auto *item = propertyItem(name, hidden);
  if (! item) return false;

  type = item->userTypeName();

  return true;
}

bool
CQChartsView::
getPropertyObject(const QString &name, QObject* &object, bool hidden) const
{
  object = nullptr;

  const auto *item = propertyItem(name, hidden);
  if (! item) return false;

  object = item->object();

  return true;
}

bool
CQChartsView::
getPropertyIsHidden(const QString &name, bool &is_hidden) const
{
  is_hidden = false;

  const auto *item = propertyItem(name, /*hidden*/true);
  if (! item) return false;

  is_hidden = CQCharts::getItemIsHidden(item);

  return true;
}

bool
CQChartsView::
getPropertyIsStyle(const QString &name, bool &is_style) const
{
  is_style = false;

  const auto *item = propertyItem(name, /*hidden*/true);
  if (! item) return false;

  is_style = CQCharts::getItemIsStyle(item);

  return true;
}

CQPropertyViewItem *
CQChartsView::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  assert(CQUtil::hasProperty(object, name));

  return propertyModel()->addProperty(path, object, name, alias);
}

void
CQChartsView::
getPropertyNames(QStringList &names, bool hidden) const
{
  propertyModel()->objectNames(this, names, hidden);

  if (key())
    propertyModel()->objectNames(key(), names, hidden);
}

void
CQChartsView::
hideProperty(const QString &path, QObject *object)
{
  propertyModel()->hideProperty(path, object);
}

const CQPropertyViewItem *
CQChartsView::
propertyItem(const QString &name, bool hidden) const
{
  return propertyModel()->propertyItem(this, name, hidden);
}

//------

CQChartsAnnotationGroup *
CQChartsView::
addAnnotationGroup()
{
  return addAnnotationT<CQChartsAnnotationGroup>(new CQChartsAnnotationGroup(this));
}

CQChartsArrowAnnotation *
CQChartsView::
addArrowAnnotation(const CQChartsPosition &start, const CQChartsPosition &end)
{
  return addAnnotationT<CQChartsArrowAnnotation>(
    new CQChartsArrowAnnotation(this, start, end));
}

CQChartsEllipseAnnotation *
CQChartsView::
addEllipseAnnotation(const CQChartsPosition &center, const CQChartsLength &xRadius,
                     const CQChartsLength &yRadius)
{
  return addAnnotationT<CQChartsEllipseAnnotation>(
    new CQChartsEllipseAnnotation(this, center, xRadius, yRadius));
}

CQChartsImageAnnotation *
CQChartsView::
addImageAnnotation(const CQChartsPosition &pos, const CQChartsImage &image)
{
  return addAnnotationT<CQChartsImageAnnotation>(
    new CQChartsImageAnnotation(this, pos, image));
}

CQChartsImageAnnotation *
CQChartsView::
addImageAnnotation(const CQChartsRect &rect, const CQChartsImage &image)
{
  return addAnnotationT<CQChartsImageAnnotation>(
    new CQChartsImageAnnotation(this, rect, image));
}

CQChartsKeyAnnotation *
CQChartsView::
addKeyAnnotation()
{
  return addAnnotationT<CQChartsKeyAnnotation>(
    new CQChartsKeyAnnotation(this));
}

CQChartsPieSliceAnnotation *
CQChartsView::
addPieSliceAnnotation(const CQChartsPosition &pos, const CQChartsLength &innerRadius,
                      const CQChartsLength &outerRadius, const CQChartsAngle &startAngle,
                      const CQChartsAngle &spanAngle)
{
  return addAnnotationT<CQChartsPieSliceAnnotation>(
    new CQChartsPieSliceAnnotation(this, pos, innerRadius, outerRadius, startAngle, spanAngle));
}

CQChartsPointAnnotation *
CQChartsView::
addPointAnnotation(const CQChartsPosition &pos, const CQChartsSymbol &type)
{
  return addAnnotationT<CQChartsPointAnnotation>(
    new CQChartsPointAnnotation(this, pos, type));
}

CQChartsPointSetAnnotation *
CQChartsView::
addPointSetAnnotation(const CQChartsPoints &values)
{
  return addAnnotationT<CQChartsPointSetAnnotation>(
    new CQChartsPointSetAnnotation(this, values));
}

CQChartsPolygonAnnotation *
CQChartsView::
addPolygonAnnotation(const CQChartsPolygon &points)
{
  return addAnnotationT<CQChartsPolygonAnnotation>(
    new CQChartsPolygonAnnotation(this, points));
}

CQChartsPolylineAnnotation *
CQChartsView::
addPolylineAnnotation(const CQChartsPolygon &points)
{
  return addAnnotationT<CQChartsPolylineAnnotation>(
    new CQChartsPolylineAnnotation(this, points));
}

CQChartsRectangleAnnotation *
CQChartsView::
addRectangleAnnotation(const CQChartsRect &rect)
{
  return addAnnotationT<CQChartsRectangleAnnotation>(
    new CQChartsRectangleAnnotation(this, rect));
}

CQChartsTextAnnotation *
CQChartsView::
addTextAnnotation(const CQChartsPosition &pos, const QString &text)
{
  return addAnnotationT<CQChartsTextAnnotation>(
    new CQChartsTextAnnotation(this, pos, text));
}

CQChartsTextAnnotation *
CQChartsView::
addTextAnnotation(const CQChartsRect &rect, const QString &text)
{
  return addAnnotationT<CQChartsTextAnnotation>(
    new CQChartsTextAnnotation(this, rect, text));
}

CQChartsValueSetAnnotation *
CQChartsView::
addValueSetAnnotation(const CQChartsRect &rectangle, const CQChartsReals &values)
{
  return addAnnotationT<CQChartsValueSetAnnotation>(
    new CQChartsValueSetAnnotation(this, rectangle, values));
}

CQChartsButtonAnnotation *
CQChartsView::
addButtonAnnotation(const CQChartsPosition &pos, const QString &text)
{
  return addAnnotationT<CQChartsButtonAnnotation>(
    new CQChartsButtonAnnotation(this, pos, text));
}

CQChartsWidgetAnnotation *
CQChartsView::
addWidgetAnnotation(const CQChartsPosition &pos, const CQChartsWidget &widget)
{
  return addAnnotationT<CQChartsWidgetAnnotation>(
    new CQChartsWidgetAnnotation(this, pos, widget));
}

CQChartsWidgetAnnotation *
CQChartsView::
addWidgetAnnotation(const CQChartsRect &rect, const CQChartsWidget &widget)
{
  return addAnnotationT<CQChartsWidgetAnnotation>(
    new CQChartsWidgetAnnotation(this, rect, widget));
}

void
CQChartsView::
addAnnotation(CQChartsAnnotation *annotation)
{
  annotations_.push_back(annotation);

  connect(annotation, SIGNAL(idChanged()), this, SLOT(updateAnnotationSlot()));
  connect(annotation, SIGNAL(dataChanged()), this, SLOT(updateAnnotationSlot()));

  annotation->addProperties(propertyModel(), "annotations");

  emit annotationsChanged();
}

CQChartsAnnotation *
CQChartsView::
getAnnotationByName(const QString &id) const
{
  for (auto &annotation : annotations()) {
    if (annotation->id() == id)
      return annotation;
  }

  return nullptr;
}

CQChartsAnnotation *
CQChartsView::
getAnnotationByInd(int ind) const
{
  for (auto &annotation : annotations()) {
    if (annotation->ind() == ind)
      return annotation;
  }

  return nullptr;
}

void
CQChartsView::
removeAnnotation(CQChartsAnnotation *annotation)
{
  int pos = 0;

  for (auto &annotation1 : annotations_) {
    if (annotation1 == annotation)
      break;

    ++pos;
  }

  int n = annotations_.size();

  assert(pos >= 0 && pos < n);

  delete annotation;

  for (int i = pos + 1; i < n; ++i)
    annotations_[i - 1] = annotations_[i];

  annotations_.pop_back();

  emit annotationsChanged();
}

void
CQChartsView::
removeAllAnnotations()
{
  for (auto &annotation : annotations_)
    delete annotation;

  annotations_.clear();

  propertyModel()->removeProperties("annotations");

  emit annotationsChanged();
}

void
CQChartsView::
updateAnnotationSlot()
{
  updateSlot();

  emit annotationsChanged();
}

//---

void
CQChartsView::
addPlot(CQChartsPlot *plot, const BBox &bbox)
{
  auto bbox1 = bbox;

  if (! bbox1.isSet())
    bbox1 = BBox(0, 0, viewportRange(), viewportRange());

  if (! plot->hasId()) {
    QString id(QString("%1").arg(numPlots() + 1));

    plot->setId(QString("%1%2").arg(plot->typeName()).arg(id));

    plot->setObjectName(plot->id());
  }

  plot->setViewBBox(bbox1);

  plots_.push_back(plot);

  plot->addProperties();

  plot->postInit();

  connect(plot, SIGNAL(modelChanged()), this, SLOT(plotModelChanged()));

  connect(plot, SIGNAL(connectDataChanged()), this, SLOT(plotConnectDataChangedSlot()));

  connect(plot, SIGNAL(errorsCleared()), this, SIGNAL(updateErrors()));
  connect(plot, SIGNAL(errorAdded()), this, SIGNAL(updateErrors()));

  connect(plot, SIGNAL(currentPlotIdChanged(const QString &)),
          this, SIGNAL(currentPlotChanged()));

  //---

  if (currentPlotInd_ < 0)
    setCurrentPlot(plot);

  auto *currentPlot = this->currentPlot(/*remap*/false);

  if (currentPlot->parentPlot() && ! plot->parentPlot())
    setCurrentPlot(plot);

  //---

  invalidateObjects();
  invalidateOverlay();

  update();

  //---

  emit plotAdded(plot);
  emit plotAdded(plot->id());
  emit plotsChanged();
}

void
CQChartsView::
raisePlot(CQChartsPlot *plot)
{
  int pos = plotPos(plot);
  if (pos < 0) return; // not found

  int np = plots().size();
  if (np < 2) return;

  if (pos < np - 1)
    std::swap(plots_[pos + 1], plots_[pos]);

  update();

  emit plotsReordered();
}

void
CQChartsView::
lowerPlot(CQChartsPlot *plot)
{
  int pos = plotPos(plot);
  if (pos < 0) return; // not found

  int np = plots().size();
  if (np < 2) return;

  if (pos > 0)
    std::swap(plots_[pos - 1], plots_[pos]);

  update();

  emit plotsReordered();
}

int
CQChartsView::
plotPos(CQChartsPlot *plot) const
{
  int np = plots().size();

  for (int i = 0; i < np; ++i) {
    if (plots_[i] == plot)
      return i;
  }

  return -1;
}

void
CQChartsView::
removePlot(CQChartsPlot *plot)
{
  assert(plot);

  bool isCurrent = (plot == currentPlot(/*remap*/false));

  // build new list of plots without plot and check for match
  Plots plots1;

  bool found = false;

  for (auto &plot1 : plots()) {
    if (plot1 != plot)
      plots1.push_back(plot1);
    else
      found = true;
  }

  // skip if no match found
  if (! found)
    return;

  //---

  // remove plot
  QString id = plot->id();

  propertyModel()->removeProperties(id, plot);

  std::swap(plots1, plots_);

  if (mousePlot() == plot)
    mouseData_.reset();

  delete plot;

  //---

  if (isCurrent) {
    if (plots().empty())
      setCurrentPlot(nullptr);
    else
      setCurrentPlot(plots_[0]);
  }

  //---

  invalidateObjects();
  invalidateOverlay();

  update();

  //---

  emit plotRemoved(id);
  emit plotsChanged();
}

void
CQChartsView::
removeAllPlots()
{
  for (auto &plot : plots())
    propertyModel()->removeProperties(plot->id(), plot);

  for (auto &plot : plots_)
    delete plot;

  plots_.clear();

  mouseData_.reset();

  setCurrentPlot(nullptr);

  //---

  invalidateObjects();
  invalidateOverlay();

  update();

  //---

  emit allPlotsRemoved();
  emit plotsChanged();
}

//---

void
CQChartsView::
plotModelChanged()
{
  auto *plot = qobject_cast<CQChartsPlot *>(sender());

  if (plot == currentPlot(/*remap*/false))
    emit currentPlotChanged();
}

void
CQChartsView::
plotConnectDataChangedSlot()
{
  auto *plot = qobject_cast<CQChartsPlot *>(sender());

  if (plot)
    emit plotConnectDataChanged(plot->id());

  emit connectDataChanged();
}

//---

void
CQChartsView::
resetGrouping()
{
  resetConnections(/*notify*/false);

  resetPlotGrouping();

  emit connectDataChanged();
}

void
CQChartsView::
resetPlotGrouping()
{
  resetPlotGrouping(plots());
}

void
CQChartsView::
resetPlotGrouping(const Plots &plots)
{
  for (auto &plot : plots) {
    if (plot->xAxis()) {
      plot->xAxis()->setSide(CQChartsAxisSide::Type::BOTTOM_LEFT);
      plot->xAxis()->setVisible(true);
    }

    if (plot->yAxis()) {
      plot->yAxis()->setSide(CQChartsAxisSide::Type::BOTTOM_LEFT);
      plot->yAxis()->setVisible(true);
    }

    //if (plot->key())
    //  plot->key()->setVisible(true);

    //if (plot->title())
    //  plot->title()->setVisible(true);

    plot->resetKeyItems();
  }
}

void
CQChartsView::
resetConnections(bool notify)
{
  resetConnections(plots(), notify);
}

void
CQChartsView::
resetConnections(const Plots &plots, bool notify)
{
  for (const auto &plot : plots)
    plot->resetConnectData(/*notify*/false);

  if (notify)
    emit connectDataChanged();
}

#if 0
void
CQChartsView::
initOverlay()
{
  if (! numPlots())
    return;

  auto *firstPlot = plot(0)->firstPlot();

  initOverlayPlot(firstPlot);
}
#endif

void
CQChartsView::
initOverlay(const Plots &plots, bool reset)
{
  assert(plots.size() >= 2);

  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetPlotGrouping(plots);

    resetConnections(plots, /*notify*/false);
  }

#if 0
  for (std::size_t i = 0; i < plots.size(); ++i) {
    auto *plot = plots[i];

    plot->syncRange();
  }
#endif

  for (std::size_t i = 0; i < plots.size(); ++i) {
    auto *plot = plots[i];

    plot->setOverlay(true, /*notify*/false);

    if (i > 0) {
      auto *prevPlot = plots[i - 1];

      plot    ->setPrevPlot(prevPlot);
      prevPlot->setNextPlot(plot);
    }
  }

  auto *rootPlot = plots[0]->firstPlot();

  initOverlayPlot(rootPlot);

  emit connectDataChanged();
}

void
CQChartsView::
initOverlayPlot(CQChartsPlot *firstPlot)
{
  firstPlot->setOverlay(true, /*notify*/false);

  if (firstPlot->title() && title().length())
    firstPlot->title()->setTextStr(title());

  //---

  CQChartsPlot::Plots plots;

  firstPlot->overlayPlots(plots);

  for (auto &plot : plots) {
    if (plot == firstPlot)
      continue;

    plot->setOverlay(true, /*notify*/false);
  }

  //---

  firstPlot->updateOverlay();

  //---

  initOverlayAxes();
}

void
CQChartsView::
initOverlayAxes()
{
  auto *firstPlot = plots_[0]->firstPlot();

  if      (firstPlot->isOverlay()) {
    CQChartsPlot::Plots plots;

    firstPlot->overlayPlots(plots);

    for (auto &plot : plots) {
      auto *xaxis = plot->xAxis();
      auto *yaxis = plot->yAxis();

      if (xaxis)
        xaxis->setVisible(plot == firstPlot);

      if (yaxis)
        yaxis->setVisible(plot == firstPlot);
    }

    if      (firstPlot->isX1X2()) {
      auto *plot2 = firstPlot->nextPlot();

      plot2->xAxis()->setSide(CQChartsAxisSide::Type::TOP_RIGHT);
    }
    else if (firstPlot->isY1Y2()) {
      auto *plot2 = firstPlot->nextPlot();

      plot2->yAxis()->setSide(CQChartsAxisSide::Type::TOP_RIGHT);
    }
  }
  else if (firstPlot->isOverlay(/*checkVisible*/false)) {
    CQChartsPlot::Plots plots;

    firstPlot->overlayPlots(plots);

    for (auto &plot : plots) {
      auto *xaxis = plot->xAxis();
      auto *yaxis = plot->yAxis();

      xaxis->setVisible(true);
      yaxis->setVisible(true);

      if      (firstPlot->isX1X2(/*checkVisible*/false)) {
        plot->xAxis()->setSide(CQChartsAxisSide::Type::BOTTOM_LEFT);
      }
      else if (firstPlot->isY1Y2(/*checkVisible*/false)) {
        plot->yAxis()->setSide(CQChartsAxisSide::Type::BOTTOM_LEFT);
      }
    }
  }
}

void
CQChartsView::
initX1X2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset)
{
  if (reset) {
    if (isScrolled())
      setScrolled(false);

    Plots plots {{ plot1, plot2 }};

    resetPlotGrouping(plots);

    resetConnections(plots, /*notify*/false);
  }

  assert(plot1 != plot2 && ! plot1->isOverlay() && ! plot2->isOverlay());

  if (plot1->title() && title().length())
    plot1->title()->setTextStr(title());

  plot1->setX1X2(true, /*notify*/false);
  plot2->setX1X2(true, /*notify*/false);

  plot1->setOverlay(overlay, /*notify*/false);
  plot2->setOverlay(overlay, /*notify*/false);

  plot1->setNextPlot(plot2);
  plot2->setPrevPlot(plot1);

  // first plot x axis BOTTOM/LEFT (set by resetConnectData), second plot x axis TOP/RIGHT
  if (plot2->xAxis()) {
    if (plot2->isOverlay())
      plot2->xAxis()->setSide(CQChartsAxisSide::Type::TOP_RIGHT);
  }

  if (plot2->isOverlay()) {
    if (plot2->yAxis())
      plot2->yAxis()->setVisible(false);
  }

  //---

  if (overlay)
    plot1->updateOverlay();

  plot1->updateObjs();

  plot1->applyDataRange();

  //---

  emit connectDataChanged();
}

void
CQChartsView::
initY1Y2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset)
{
  if (reset) {
    if (isScrolled())
      setScrolled(false);

    Plots plots {{ plot1, plot2 }};

    resetPlotGrouping(plots);

    resetConnections(plots, /*notify*/false);
  }

  assert(plot1 != plot2 && ! plot1->isOverlay() && ! plot2->isOverlay());

  if (plot1->title() && title().length())
    plot1->title()->setTextStr(title());

  plot1->setY1Y2(true, /*notify*/false);
  plot2->setY1Y2(true, /*notify*/false);

  plot1->setOverlay(overlay, /*notify*/false);
  plot2->setOverlay(overlay, /*notify*/false);

  plot1->setNextPlot(plot2);
  plot2->setPrevPlot(plot1);

  // first plot y axis BOTTOM/LEFT (set by resetConnectData), second plot y axis TOP/RIGHT
  if (plot2->yAxis()) {
    if (plot2->isOverlay())
      plot2->yAxis()->setSide(CQChartsAxisSide::Type::TOP_RIGHT);
  }

  if (plot2->isOverlay()) {
    if (plot2->xAxis())
      plot2->xAxis()->setVisible(false);
  }

  //---

  if (overlay)
    plot1->updateOverlay();

  plot1->updateObjs();

  plot1->applyDataRange();

  //---

  emit connectDataChanged();
}

void
CQChartsView::
initTabbed(const Plots &plots, bool reset)
{
  assert(plots.size() >= 2);

  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetPlotGrouping(plots);

    resetConnections(plots, /*notify*/false);
  }

#if 0
  for (std::size_t i = 0; i < plots.size(); ++i) {
    auto *plot = plots[i];

    plot->syncRange();
  }
#endif

  for (std::size_t i = 0; i < plots.size(); ++i) {
    auto *plot = plots[i];

    plot->setTabbed(true, /*notify*/false);

    if (i > 0) {
      auto *prevPlot = plots[i - 1];

      plot    ->setPrevPlot(prevPlot);
      prevPlot->setNextPlot(plot);
    }

    plot->setCurrent(i == 0);
  }

  emit connectDataChanged();
}

//------

void
CQChartsView::
autoPlacePlots()
{
  int np = plots().size();

  int nr = std::max(int(sqrt(np)), 1);
  int nc = (np + nr - 1)/nr;

  placePlots(plots(), /*vertical*/false, /*horizontal*/false, nr, nc, /*reset*/true);
}

void
CQChartsView::
placePlots(const Plots &plots, bool vertical, bool horizontal,
           int rows, int columns, bool reset)
{
  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetConnections(plots, /*notify*/false);
  }

  //---

  PlotSet basePlotSet;

  for (const auto &plot : plots) {
    auto *plot1 = this->basePlot(plot);

    basePlotSet.insert(plot1);
  }

  Plots basePlots;

  for (const auto &basePlot : basePlotSet)
    basePlots.push_back(basePlot);

  Plots plots1;

  for (const auto &plot : plots) {
    auto *plot1 = this->basePlot(plot);

    if (basePlotSet.find(plot1) != basePlotSet.end()) {
      plots1.push_back(plot1);

      basePlotSet.erase(plot1);
    }
  }

  //---

  int np = plots1.size();

  if (np <= 0)
    return;

  //---

  auto setViewBBox = [&](CQChartsPlot *plot, const BBox &bbox) {
    if (plot->isOverlay()) {
      Plots plots;

      plot->overlayPlots(plots);

      for (const auto &plot : plots)
        plot->setViewBBox(bbox);
    }
    else
      plot->setViewBBox(bbox);
  };

  //---

  int  nr = 1, nc = 1;
  bool overlay = false;

  if     (horizontal)
    nc = np;
  else if (vertical)
    nr = np;
  else if (rows > 0) {
    overlay = (rows <= 1 && columns <= 1);

    nr = rows;
    nc = (np + nr - 1)/nr;
  }
  else if (columns > 0) {
    overlay = (rows <= 1 && columns <= 1);

    nc = columns;
    nr = (np + nc - 1)/nc;
  }
  else {
    nr = std::max(int(sqrt(np)), 1);
    nc = (np + nr - 1)/nr;
  }

  double vr = CQChartsView::viewportRange();

  if (overlay) {
    for (int i = 0; i < np; ++i) {
      auto *plot = plots1[i];

      BBox bbox(0, 0, vr, vr);

      setViewBBox(plot, bbox);
    }
  }
  else {
    double dx = vr/nc;
    double dy = vr/nr;

    int    i = 0;
    double y = vr;

    for (int r = 0; r < nr; ++r) {
      double x = 0.0;

      for (int c = 0; c < nc; ++c, ++i) {
        if (i >= int(plots1.size()))
          break;

        auto *plot = plots1[i];

        BBox bbox(x, y - dy, x + dx, y);

        setViewBBox(plot, bbox);

        x += dx;
      }

      y -= dy;
    }
  }

  if (reset)
    emit connectDataChanged();
}

//------

QColor
CQChartsView::
interpPaletteColor(const ColorInd &ind, bool scale) const
{
  Color c(CQChartsColor::Type::PALETTE);

  c.setScale(scale);

  return interpColor(c, ind);

  //return charts()->interpPaletteColor(ind, scale);
}

QColor
CQChartsView::
interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv, bool scale) const
{
  return charts()->interpGroupPaletteColor(ig, iv, scale);
}

QColor
CQChartsView::
interpThemeColor(const ColorInd &ind) const
{
  return charts()->interpThemeColor(ind);
}

QColor
CQChartsView::
interpInterfaceColor(double r) const
{
  CQChartsColor c(CQChartsColor::Type::INTERFACE_VALUE, r);

  return charts()->interpColor(c, ColorInd());
}

QColor
CQChartsView::
interpColor(const CQChartsColor &c, const ColorInd &ind) const
{
  if (defaultPalette_ != "") {
    CQChartsColor c1 = charts()->adjustDefaultPalette(c, defaultPalette_);

    return charts()->interpColor(c1, ind);
  }

  return charts()->interpColor(c, ind);
}

//------

void
CQChartsView::
mousePressEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

  Point mp(me->pos());

  mouseData_.reset();

  mouseData_.pressPoint = adjustMousePos(mp);
  mouseData_.button     = me->button();
  mouseData_.pressed    = true;
  mouseData_.movePoint  = mouseData_.pressPoint;
  mouseData_.selMod     = modifiersToSelMod(me->modifiers());
  mouseData_.clickMod   = modifiersToClickMod(me->modifiers());

  auto w = pixelToWindow(mousePressPoint());

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if (mouseButton() == Qt::LeftButton) {
    if      (mode() == Mode::SELECT)
      selectMousePress();
    else if (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT)
      zoomMousePress();
    else if (mode() == Mode::PAN) {
    }
    else if (mode() == Mode::PROBE) {
    }
    else if (mode() == Mode::QUERY) {
    }
    else if (mode() == Mode::EDIT)
      (void) editMousePress();
    else if (mode() == Mode::REGION)
      regionMousePress();
  }
  else if (mouseButton() == Qt::MiddleButton) {
  }
  else if (mouseButton() == Qt::RightButton) {
    mouseData_.pressed    = false;
    mouseData_.pressPoint = Point(me->globalPos());

    showMenu(mp);

    return;
  }
}

void
CQChartsView::
mouseMoveEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

  Point mp(me->pos());

  mouseData_.oldMovePoint = mouseMovePoint();
  mouseData_.movePoint    = adjustMousePos(mp);

  // select mode and move (not pressed) - update plot positions
  if (! mousePressed()) {
    if      (mode() == Mode::SELECT) {
      selectMouseMotion();
      return;
    }
    else if (mode() == Mode::REGION) {
      regionMouseMotion();
      return;
    }
  }

  //---

  if (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT) {
    updatePosText(mouseMovePoint());
  }

  //---

  // probe mode and move (pressed or not pressed) - show probe lines
  if (mode() == Mode::PROBE) {
    showProbeLines(mouseMovePoint());
    return;
  }

  //---

  if (mode() == Mode::QUERY) {
    updatePosText(mouseMovePoint());
    return;
  }

  //---

  if (! mousePressed()) {
    if (mode() == Mode::EDIT) {
      updatePosText(mouseMovePoint());

      editMouseMotion();
    }

    return;
  }

  //---

  // get plots at point
  auto w = pixelToWindow(mouseMovePoint());

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if      (mouseButton() == Qt::LeftButton) {
    // select plot object
    if      (mode() == Mode::SELECT) {
      selectMouseMove();

      searchPos_ = mouseMovePoint();
    }
    // draw zoom rectangle
    else if (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT)
      zoomMouseMove();
    else if (mode() == Mode::PAN)
      panMouseMove();
    else if (mode() == Mode::EDIT)
      (void) editMouseMove();
    else if (mode() == Mode::REGION)
      selectMouseMove();
  }
  else if (mouseButton() == Qt::MiddleButton) {
    if (! mousePressed())
      return;
  }
  else if (mouseButton() == Qt::RightButton) {
    if (! mousePressed())
      return;
  }
}

void
CQChartsView::
mouseReleaseEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

  Point mp(me->pos());

  mouseData_.oldMovePoint = mouseMovePoint();
  mouseData_.movePoint    = adjustMousePos(mp);

  CQChartsScopeGuard resetMouseData([&]() { mouseData_.reset(); });

  //auto w = pixelToWindow(mouseMovePoint());

  if      (mouseButton() == Qt::LeftButton) {
    if      (mode() == Mode::SELECT) {
      if (mousePressed())
        selectMouseRelease();
    }
    else if (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT) {
      if (mousePressed())
        zoomMouseRelease();
    }
    else if (mode() == Mode::PAN) {
    }
    else if (mode() == Mode::PROBE) {
    }
    else if (mode() == Mode::QUERY) {
    }
    else if (mode() == Mode::EDIT) {
      if (mousePressed())
        editMouseRelease();
    }
    else if (mode() == Mode::REGION) {
      if (mousePressed())
        regionMouseRelease();
    }
  }
  else if (mouseButton() == Qt::MiddleButton) {
  }
  else if (mouseButton() == Qt::RightButton) {
  }
}

CQChartsGeom::Point
CQChartsView::
adjustMousePos(const Point &pos) const
{
  return Point(pos.x + sizeData_.xpos, pos.y + sizeData_.ypos);
}

//------

void
CQChartsView::
keyPressEvent(QKeyEvent *ke)
{
  if (isPreview())
    return;

  if      (ke->key() == Qt::Key_Escape) {
    mouseData_.escape = true;

    if (mousePressed())
      endRegionBand();

    if      (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT) {
      if (! mousePressed())
        selectModeSlot();
    }
    else if (mode() == Mode::PAN) {
      if (! mousePressed())
        selectModeSlot();
    }
    else if (mode() == Mode::PROBE) {
      selectModeSlot();
    }
    else if (mode() == Mode::QUERY) {
      selectModeSlot();
    }
    else if (mode() == Mode::EDIT) {
      selectModeSlot();
    }

    return;
  }
  else if (ke->key() == Qt::Key_Z) {
    zoomModeSlot();

    return;
  }
  else if (ke->key() == Qt::Key_Bar) {
    probeModeSlot();

    // TODO: do mouse move to show probe immediately or add probe move API

    return;
  }
  else if (ke->key() == Qt::Key_Q) {
    queryModeSlot();

    return;
  }
  else if (ke->key() == Qt::Key_Insert) {
    editModeSlot();
  }
  else if (ke->key() == Qt::Key_Tab) {
    if (mode() == Mode::EDIT)
      cycleEdit();
  }
  else if (ke->key() == Qt::Key_F1) {
    if (mode() == Mode::EDIT)
      cycleEdit();
  }

  //---

  auto gpos = QCursor::pos();
  auto pos  = mapFromGlobal(gpos);

  auto w = pixelToWindow(Point(pos));

  Plots         plots;
  CQChartsPlot *plot;

  plotsAt(w, plots, plot);

  if (plot)
    plot->keyPress(ke->key(), ke->modifiers());
}

//------

bool
CQChartsView::
editMousePress()
{
  auto p = mousePressPoint();
  auto w = pixelToWindow(p);

  //---

  Annotations selAnnotations;

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w) ||
        (annotation->editHandles()->inside(w) != CQChartsResizeSide::NONE))
      selAnnotations.push_back(annotation);
  }

  // start drag on already selected annotation handle
  if (! selAnnotations.empty() && selAnnotations[0]->isSelected()) {
    auto *annotation = selAnnotations[0];

    mouseData_.dragSide = annotation->editHandles()->inside(w);

    if (mouseData_.dragSide != CQChartsResizeSide::NONE) {
      mouseData_.dragObj = DragObj::ANNOTATION;

      annotation->editHandles()->setDragSide(mouseData_.dragSide);
      annotation->editHandles()->setDragPos (w);

      invalidateOverlay();

      return true;
    }
  }

  // start drag on already selected key handle
  if (key() && key()->isSelected()) {
    mouseData_.dragSide = key()->editHandles()->inside(w);

    if (mouseData_.dragSide != CQChartsResizeSide::NONE) {
      mouseData_.dragObj = DragObj::KEY;

      key()->editPress(w);

      key()->editHandles()->setDragSide(mouseData_.dragSide);
      key()->editHandles()->setDragPos (w);

      invalidateOverlay();

      return true;
    }
  }

  //---

  // select new annotation
  for (const auto &selAnnotation : selAnnotations) {
    if (! selAnnotation->editPress(w))
      continue;

    //---

    selectOneObj(selAnnotation);

    mouseData_.dragObj = DragObj::ANNOTATION;

    update();

    return true;
  }

  //---

  // select/deselect key
  if (key()) {
    if (key()->contains(w)) {
      if (! key()->isSelected()) {
        selectOneObj(key());

        update();

        return true;
      }

      if (key()->editPress(w)) {
        mouseData_.dragObj = DragObj::KEY;

        invalidateOverlay();

        return true;
      }

      return false;
    }
  }

  //---

  bool rc = processMouseDataPlots([&](CQChartsPlot *plot, const Point &p) {
    return plot->editMousePress(p, /*inside*/false);
  }, p);

  if (rc)
    return true;

  rc = processMouseDataPlots([&](CQChartsPlot *plot, const Point &p) {
    return plot->editMousePress(p, /*inside*/true);
  }, p);

  if (rc)
    return true;

  deselectAll();

  return false;
}

bool
CQChartsView::
editMouseMove()
{
  auto p = mouseMovePoint();
  auto w = pixelToWindow(p);

  if      (mouseData_.dragObj == DragObj::KEY) {
    if (key()->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObj == DragObj::ANNOTATION) {
    bool edited = false;

    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        if (annotation->editMove(w))
          mouseData_.dragged = true;

        invalidateObjects();
        invalidateOverlay();

        edited = true;
      }
    }

    if (! edited)
      return false;
  }

  //---

  processMouseDataPlots([&](CQChartsPlot *plot, const Point &pos) {
    bool current = (plot == mousePlot());

    return plot->editMouseMove(pos, current);
  }, mouseMovePoint());

  return true;
}

void
CQChartsView::
editMouseMotion()
{
  auto p = mouseMovePoint();
  auto w = pixelToWindow(p);

  //---

  if (key() && key()->isSelected()) {
    if (key()->editMotion(w)) {
      invalidateOverlay();

      update();

      return;
    }
  }
  else {
    // update selected annotation inside
    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        if (annotation->editMotion(w)) {
          invalidateOverlay();

          update();

          return;
        }
      }
    }
  }

  //---

  // update plot mouse inside
  Plots         plots;
  CQChartsPlot *plot;

  plotsAt(w, plots, plot);

  if (plot)
    plot->editMouseMotion(p);

  for (auto &plot1 : plots) {
    if (plot1 == plot) continue;

    plot1->editMouseMotion(p);
  }
}

void
CQChartsView::
editMouseRelease()
{
  mouseData_.dragObj = DragObj::NONE;

  processMouseDataPlots([&](CQChartsPlot *plot, const Point &pos) {
    plot->editMouseRelease(pos); return false;
  }, mouseMovePoint());
}

//------

void
CQChartsView::
showProbeLines(const Point &p)
{
  auto addVerticalProbeBand = [&](int &ind, CQChartsPlot *plot, const QString &tip,
                                  double px, double py1, double py2) -> void {
    while (ind >= int(probeBands_.size())) {
      auto *probeBand = new CQChartsProbeBand(this);

      probeBands_.push_back(probeBand);
    }

    probeBands_[ind]->showVertical(plot, tip, px, py1, py2);

    ++ind;
  };

  auto addHorizontalProbeBand = [&](int &ind, CQChartsPlot *plot, const QString &tip,
                                    double px1, double px2, double py) -> void {
    while (ind >= int(probeBands_.size())) {
      auto *probeBand = new CQChartsProbeBand(this);

      probeBands_.push_back(probeBand);
    }

    probeBands_[ind]->showHorizontal(plot, tip, px1, px2, py);

    ++ind;
  };

  //int px = p.x;

  auto w = pixelToWindow(p);

  Plots         plots;
  CQChartsPlot *plot;

  plotsAt(w, plots, plot);

  int probeInd = 0;

  for (auto &plot : plots) {
    auto w = plot->pixelToWindow(p);

    //---

    CQChartsPlot::ProbeData probeData;

    probeData.p = w;

    if (! plot->probe(probeData))
      continue;

    if (probeData.xvals.empty()) probeData.xvals.emplace_back(w.x);
    if (probeData.yvals.empty()) probeData.yvals.emplace_back(w.y);

    auto dataRange = plot->calcDataRange();

    if      (probeData.both) {
      // add probe lines from xmin to probed x values and from ymin to probed y values
      auto px1 = plot->windowToPixel(Point(dataRange.getXMin(), probeData.p.y));
      auto py1 = plot->windowToPixel(Point(probeData.p.x, dataRange.getYMin()));

      int nx = probeData.xvals.size();
      int ny = probeData.yvals.size();

      int n = std::min(nx, ny);

      for (int i = 0; i < n; ++i) {
        const auto &xval = probeData.xvals[i];
        const auto &yval = probeData.yvals[i];

        auto px2 = plot->windowToPixel(Point(xval.value, probeData.p.y));
        auto py2 = plot->windowToPixel(Point(probeData.p.x, yval.value));

        QString tip = QString("%1, %2").
          arg(xval.label.length() ? xval.label : plot->xStr(xval.value)).
          arg(yval.label.length() ? yval.label : plot->yStr(yval.value));

        addVerticalProbeBand  (probeInd, plot, tip, py1.x, py1.y, py2.y);
        addHorizontalProbeBand(probeInd, plot, "" , px1.x, px2.x, px2.y);
      }
    }
    else if (probeData.direction == Qt::Vertical) {
      // add probe lines from ymin to probed y values
      auto p1 = plot->windowToPixel(Point(probeData.p.x, dataRange.getYMin()));

      for (const auto &yval : probeData.yvals) {
        auto p2 = plot->windowToPixel(Point(probeData.p.x, yval.value));

        QString tip = (yval.label.length() ? yval.label : plot->yStr(yval.value));

        addVerticalProbeBand(probeInd, plot, tip, p1.x, p1.y, p2.y);
      }
    }
    else {
      // add probe lines from xmin to probed x values
      auto p1 = plot->windowToPixel(Point(dataRange.getXMin(), probeData.p.y));

      for (const auto &xval : probeData.xvals) {
        auto p2 = plot->windowToPixel(Point(xval.value, probeData.p.y));

        QString tip = (xval.label.length() ? xval.label : plot->xStr(xval.value));

        addHorizontalProbeBand(probeInd, plot, tip, p1.x, p2.x, p2.y);
      }
    }
  }

  for (int i = probeInd; i < int(probeBands_.size()); ++i)
    probeBands_[i]->hide();
}

//------

void
CQChartsView::
selectMousePress()
{
  if      (isPointSelectMode())
    selectPointPress();
  else if (isRectSelectMode())
    startRegionBand(mousePressPoint());
  else if (regionMode() == RegionMode::RECT)
    startRegionBand(mousePressPoint());
}

void
CQChartsView::
selectPointPress()
{
  auto w = pixelToWindow(mousePressPoint());

  //---

  // select key
  if (key() && key()->contains(w)) {
    bool handled = key()->selectPress(w, mouseClickMod());

    if (handled) {
      emit keyPressed  (key());
      emit keyIdPressed(key()->id());

      return;
    }
  }

  //---

  // select view annotation
  annotationsAtPoint(w, pressAnnotations_);

  for (const auto &annotation : pressAnnotations_) {
    annotation->mousePress(w, mouseSelMod());
  }

  for (const auto &selAnnotation : pressAnnotations_) {
    if (! selAnnotation->selectPress(w, mouseSelMod()))
      continue;

    selectOneObj(selAnnotation);

    update();

    emit annotationPressed  (selAnnotation);
    emit annotationIdPressed(selAnnotation->id());

    return;
  }

  //---

  // select plot objects
  struct SelData {
    Point          pos;
    CQChartsSelMod selMod;

    SelData(const Point &pos, CQChartsSelMod selMod) :
     pos(pos), selMod(selMod) {
    }
  };

  SelData selData(mousePressPoint(), mouseSelMod());

  if (processMouseDataPlots([&](CQChartsPlot *plot, const SelData &data) {
        if (plot->selectMousePress(data.pos, data.selMod)) {
          setCurrentPlot(plot);
          return true;
        }
        return false;
      }, selData)) {
    return;
  }

  //---

  // select view key
  if (key() && key()->contains(w))
    key()->selectPress(w, SelMod::REPLACE);
}

void
CQChartsView::
selectMouseMotion()
{
  updatePosText(mouseMovePoint());

  //---

  searchPos_ = mouseMovePoint();

  if (searchTimer_)
    searchTimer_->start();
  else
    searchSlot();
}

void
CQChartsView::
selectMouseMove()
{
  if      (isPointSelectMode()) {
    if (key()) {
      auto w = pixelToWindow(mouseMovePoint());

      bool handled = key()->selectMove(w);

      if (handled)
        return;
    }

    //---

    processMouseDataPlots([&](CQChartsPlot *plot, const Point &pos) {
      bool current = (plot == mousePlot());

      return plot->selectMouseMove(pos, current);
    }, searchPos_);
  }
  else if (isRectSelectMode()) {
    if (mouseData_.escape)
      endRegionBand();
    else
      updateRegionBand(mousePressPoint(), mouseMovePoint());
  }
}

void
CQChartsView::
selectMouseRelease()
{
  if      (isPointSelectMode()) {
    if (mousePlot())
      mouseData_.plot->selectMouseRelease(mouseMovePoint());
  }
  else if (isRectSelectMode()) {
    endRegionBand();

    //---

    double dx = abs(mouseMovePoint().x - mousePressPoint().x);
    double dy = abs(mouseMovePoint().y - mousePressPoint().y);

    if (dx < 4 && dy < 4) {
      selectPointPress();

      if (mousePlot())
        mouseData_.plot->selectMouseRelease(mouseMovePoint());
    }
    else {
      processMouseDataPlots([&](CQChartsPlot *plot, const CQChartsSelMod &selMod) {
        auto w1 = plot->pixelToWindow(mousePressPoint());
        auto w2 = plot->pixelToWindow(mouseMovePoint());

        return plot->rectSelect(BBox(w1, w2), selMod);
      }, mouseSelMod());
    }
  }
  else if (regionMode() == RegionMode::RECT) {
    endRegionBand();
  }

  //---

  // release pressed annotations
  for (const auto &annotation : pressAnnotations_) {
    auto w = pixelToWindow(mousePressPoint());

    annotation->mouseRelease(w);
  }
}

bool
CQChartsView::
isRectSelectMode() const
{
  if (selectMode() != SelectMode::RECT)
    return false;

  auto *currentPlot = this->currentPlot(/*remap*/false);

  if (currentPlot && ! currentPlot->type()->canRectSelect())
    return false;

  return true;
}

bool
CQChartsView::
isPointSelectMode() const
{
  if (selectMode() == SelectMode::POINT)
    return true;

  if (selectMode() == SelectMode::RECT) {
    auto *currentPlot = this->currentPlot(/*remap*/false);

    if (currentPlot && ! currentPlot->type()->canRectSelect())
      return true;
  }

  return false;
}

//------

void
CQChartsView::
regionMousePress()
{
  if      (regionMode() == RegionMode::POINT) {
  }
  else if (regionMode() == RegionMode::RECT)
    startRegionBand(mousePressPoint());
}

void
CQChartsView::
regionMouseMove()
{
  if      (regionMode() == RegionMode::POINT) {
  }
  else if (regionMode() == RegionMode::RECT) {
    if (mouseData_.escape)
      endRegionBand();
    else
      updateRegionBand(mousePressPoint(), mouseMovePoint());
  }
}

void
CQChartsView::
regionMouseMotion()
{
  updatePosText(mouseMovePoint());
}

void
CQChartsView::
regionMouseRelease()
{
  if      (regionMode() == RegionMode::POINT) {
    auto p = pixelToWindow(mousePressPoint());

    double w = regionData_.size.width ();
    double h = regionData_.size.height();

    BBox r(p.x - w/2.0, p.y - h/2.0, p.x + w/2.0, p.y + h/2.0);

    emit regionPointRelease(p);
    emit regionRectRelease (r);
  }
  else if (regionMode() == RegionMode::RECT) {
    endRegionBand();

    auto r = pixelToWindow(regionBand_.bbox());

    emit regionPointRelease(r.getCenter());
    emit regionRectRelease (r);
  }
}

//------

void
CQChartsView::
cycleEdit()
{
  Objs objs;

  editObjs(objs);

  if (objs.size() < 2)
    return;

  CQChartsObj *selObj = nullptr;
  bool         next   = false;

  for (auto &obj : objs) {
    if (next) {
      selObj = obj;
      break;
    }

    if (obj->isSelected())
      next = true;
  }

  if (! selObj)
    selObj = objs[0];

  //---

  selectOneObj(selObj);

  update();
}

void
CQChartsView::
editObjs(Objs &objs)
{
  for (auto &plot : plots())
    objs.push_back(plot);

  for (auto &plot : plots()) {
    Objs objs1;

    plot->editObjs(objs1);

    for (auto &obj1 : objs1)
      objs.push_back(obj1);
  }
}

//------

void
CQChartsView::
updatePosText(const Point &pos)
{
  QString posStr;

  if (posTextType() == PosTextType::PLOT) {
    auto w = pixelToWindow(pos);

    auto *currentPlot = this->currentPlot(/*remap*/false);

    PlotSet plots;

    if (currentPlot) {
      const auto &bbox = currentPlot->calcViewBBox();

      if ( bbox.inside(w))
        plots.insert(currentPlot);
    }

    if (plots.empty())
      basePlotsAt(w, plots);

    for (const auto &plot : plots) {
      auto w = plot->pixelToWindow(pos);

      if (posStr.length())
        posStr += " ";

      posStr += plot->posStr(w);
    }
  }
  else if (posTextType() == PosTextType::VIEW) {
    auto w = pixelToWindow(pos);

    posStr = QString("%1 %2").arg(w.x).arg(w.y);
  }
  else {
    posStr = QString("%1 %2").arg(pos.x).arg(pos.y);
  }

  setPosText(posStr);
}

//------

void
CQChartsView::
zoomMousePress()
{
  startRegionBand(mousePressPoint());
}

void
CQChartsView::
zoomMouseMove()
{
//mouseData_.movePoint = mouseMovePoint();

  if      (mouseData_.escape)
    endRegionBand();
  else if (mousePlot())
    updateRegionBand(mouseData_.plot, mousePressPoint(), mouseMovePoint());
}

void
CQChartsView::
zoomMouseRelease()
{
  endRegionBand();

  if (mouseData_.escape)
    return;

  if (mousePlot()) {
    auto w1 = mousePlot()->pixelToWindow(mousePressPoint());
    auto w2 = mousePlot()->pixelToWindow(mouseMovePoint ());

    BBox bbox(w1, w2);

    if      (mode() == Mode::ZOOM_IN)
      mouseData_.plot->zoomTo(bbox);
    else if (mode() == Mode::ZOOM_OUT)
      mouseData_.plot->unzoomTo(bbox);
  }
}

//------

void
CQChartsView::
panMouseMove()
{
  if (mousePlot()) {
    auto w1 = mousePlot()->pixelToWindow(mouseData_.oldMovePoint);
    auto w2 = mousePlot()->pixelToWindow(mouseMovePoint());

    double dx = w1.x - w2.x;
    double dy = w1.y - w2.y;

    mouseData_.plot->pan(dx, dy);
  }
}

//------

void
CQChartsView::
startRegionBand(const Point &pos)
{
  regionBand_.init(this);

  regionBand_.setGeometry(BBox(pos, pos));
  regionBand_.show();
}

void
CQChartsView::
updateRegionBand(CQChartsPlot *plot, const Point &pressPoint, const Point &movePoint)
{
  updateRegionBand(pressPoint, movePoint);

  if (! plot->allowZoomX() || ! plot->allowZoomY()) {
    double x = regionBand_.x     ();
    double y = regionBand_.y     ();
    double w = regionBand_.width ();
    double h = regionBand_.height();

    auto pixelRect = plot->calcPlotPixelRect();

    if (! plot->allowZoomX()) {
      x = int(pixelRect.getXMin());
      w = int(pixelRect.getWidth());
    }

    if (! plot->allowZoomY()) {
      y = int(pixelRect.getYMin());
      h = int(pixelRect.getHeight());
    }

    regionBand_.setGeometry(BBox(x, y, x + w, y + h));
    regionBand_.show();
  }
}

void
CQChartsView::
updateRegionBand(const Point &pressPoint, const Point &movePoint)
{
  double x = std::min(pressPoint.x, movePoint.x);
  double y = std::min(pressPoint.y, movePoint.y);
  double w = std::abs(movePoint.x - pressPoint.x);
  double h = std::abs(movePoint.y - pressPoint.y);

  regionBand_.setGeometry(BBox(x, y, x + w, y + h));
}

void
CQChartsView::
endRegionBand()
{
  regionBand_.hide();
}

//------

void
CQChartsView::
updateSelText()
{
  CQChartsPlot::Objs objs;

  selectedObjs(objs);

  for (auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    if (plot->isSelected())
      objs.push_back(plot);
  }

  //---

  int num = objs.size();

  for (auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    CQChartsPlot::Objs objs1;

    plot->selectedObjs(objs1);

    num += objs1.size();

    if (! objs1.empty())
      objs = objs1;
  }

  if      (num == 0)
    setSelText("None");
  else if (num == 1)
    setSelText(objs[0]->id());
  else
    setSelText(QString("%1").arg(num));
}

void
CQChartsView::
selectedPlots(Plots &plots) const
{
  for (auto &plot : this->plots()) {
    if (! plot->isVisible())
      continue;

    if (plot->isSelected())
      plots.push_back(plot);
  }
}

void
CQChartsView::
selectedObjs(Objs &objs) const
{
  for (auto &annotation : annotations()) {
    if (annotation->isSelected())
      objs.push_back(annotation);
  }

  if (key() && key()->isSelected())
    objs.push_back(key());
}

void
CQChartsView::
allSelectedObjs(Objs &objs) const
{
  CQChartsPlot::Objs objs1;

  selectedObjs(objs1);

  for (const auto &obj1 : objs1)
    objs.push_back(obj1);

  for (auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    CQChartsPlot::Objs objs1;

    plot->selectedObjs(objs1);

    for (const auto &obj1 : objs1)
      objs.push_back(obj1);
  }
}

//------

void
CQChartsView::
resizeEvent(QResizeEvent *)
{
  int w = width ();
  int h = height();

  delete ipainter_;
  delete image_;

  int iw = std::min(std::max((! isAutoSize() ? sizeData_.width  : w), 1), 16384);
  int ih = std::min(std::max((! isAutoSize() ? sizeData_.height : h), 1), 16384);

  image_ = CQChartsUtil::newImage(QSize(iw, ih));

  image_->fill(QColor(0, 0, 0, 0));

  ipainter_ = new QPainter(image_);

  //---

  sizeData_.xpos = 0;
  sizeData_.ypos = 0;

  if (isAutoSize()) {
    if (sizeData_.hbar) sizeData_.hbar->setVisible(false);
    if (sizeData_.vbar) sizeData_.vbar->setVisible(false);

    doResize(w, h);
  }
  else {
    bool showHBar = (sizeData_.width  > w);
    bool showVBar = (sizeData_.height > h);

    if (showHBar) {
      if (! sizeData_.hbar) {
        sizeData_.hbar = new QScrollBar(Qt::Horizontal, this);

        connect(sizeData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hbarScrollSlot(int)));
      }
    }

    if (showVBar) {
      if (! sizeData_.vbar) {
        sizeData_.vbar = new QScrollBar(Qt::Vertical, this);

        connect(sizeData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vbarScrollSlot(int)));
      }
    }

    int hh = (sizeData_.hbar ? sizeData_.hbar->sizeHint().height() : 0);
    int vw = (sizeData_.vbar ? sizeData_.vbar->sizeHint().width () : 0);

    if (sizeData_.hbar) sizeData_.hbar->setVisible(showHBar);
    if (sizeData_.vbar) sizeData_.vbar->setVisible(showVBar);

    if (showHBar) {
      assert(sizeData_.hbar);

      sizeData_.hbar->resize(w - (showVBar ? vw : 0), hh);

      sizeData_.hbar->move(0, h - hh);

      sizeData_.hbar->setRange(0, sizeData_.width - w);
      sizeData_.hbar->setPageStep(w);
    }

    if (showVBar) {
      assert(sizeData_.vbar);

      sizeData_.vbar->resize(vw, h - (showHBar ? hh : 0));

      sizeData_.vbar->move(w - vw, 0);

      sizeData_.vbar->setRange(0, sizeData_.height - h);
      sizeData_.vbar->setPageStep(h);
    }

    // needed if size not changed ?
    doResize(sizeData_.width, sizeData_.height);

    update();
  }
}

void
CQChartsView::
doResize(int w, int h)
{
  lockPainter(true);

  //---

  for (const auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    plot->preResize();
  }

  //---

  prect_ = BBox(0, 0, w, h);

  if (prect().getHeight() > 0)
    aspect_ = (1.0*prect().getWidth())/prect().getHeight();
  else
    aspect_ = 1.0;

  displayRange_->setPixelRange(prect_.getXMin(), prect_.getYMin(),
                               prect_.getXMax(), prect_.getYMax());

  //---

  lockPainter(false);

  //---

  for (const auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    plot->postResize();
  }
}

void
CQChartsView::
hbarScrollSlot(int pos)
{
  sizeData_.xpos = pos;

  update();
}

void
CQChartsView::
vbarScrollSlot(int pos)
{
  sizeData_.ypos = pos;

  update();
}

//------

void
CQChartsView::
paintEvent(QPaintEvent *)
{
  lockPainter(true);

  paint(ipainter_);

  QPainter painter(this);

  painter.drawImage(-sizeData_.xpos, -sizeData_.ypos, *image_);

  lockPainter(false);
}

void
CQChartsView::
paint(QPainter *painter, CQChartsPlot *plot)
{
  if (isAntiAlias())
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //---

  // draw background
  CQChartsViewPaintDevice device(this, painter);

  drawBackground(&device);

  //---

  // draw specific plot
  if (plot) {
    plot->draw(painter);
  }
  // draw all plots
  else {
    drawPlots(painter);
  }
}

void
CQChartsView::
drawBackground(CQChartsPaintDevice *device) const
{
  // fill background
  CQChartsPenBrush penBrush;

  CQChartsBrushData brushData(true, interpBackgroundFillColor(ColorInd()),
                              backgroundFillAlpha(), backgroundFillPattern());

  setBrush(penBrush, brushData);

  device->setBrush(penBrush.brush);

  device->fillRect(prect_);
}

void
CQChartsView::
drawPlots(QPainter *painter)
{
  bool hasPlots       = ! plots().empty();
  bool hasAnnotations = this->hasAnnotations();

  //---

  // draw no data
  if (! hasPlots && ! hasAnnotations && ! isPreview()) {
    showNoData(true);

    auto *painter1 = objectsBuffer_->beginPaint(painter, rect());

    if (painter1) {
      auto *th = const_cast<CQChartsView *>(this);

      CQChartsViewPaintDevice device(th, painter1);

      drawNoData(&device);
    }

    objectsBuffer_->endPaint();

    return;
  }

  showNoData(false);

  //---

  // draw plots
  if (hasPlots) {
    Plots drawPlots;

    getDrawPlots(drawPlots);

    for (const auto &plot : drawPlots) {
      if (plot->isVisible())
        plot->draw(painter);
    }
  }

  //---

  // draw annotations and key
  if (hasAnnotations || hasPlots) {
    auto *painter1 = objectsBuffer_->beginPaint(painter, rect());

    if (painter1) {
      auto *th = const_cast<CQChartsView *>(this);

      CQChartsViewPaintDevice device(th, painter1);

      if (hasAnnotations) {
        // draw annotations
        drawAnnotations(&device, CQChartsLayer::Type::ANNOTATION);
      }

      //---

      // draw view key
      if (hasPlots) {
        drawKey(&device, CQChartsLayer::Type::FG_KEY);
      }
    }

    objectsBuffer_->endPaint();
  }

  //---

  // draw overlay (annotations and key)
  if (hasPlots || hasAnnotations) {
    auto *painter1 = overlayBuffer_->beginPaint(painter, rect());

    if (painter1) {
      auto *th = const_cast<CQChartsView *>(this);

      CQChartsViewPaintDevice device(th, painter1);

      if (hasAnnotations) {
        // draw selected annotations
        drawAnnotations(&device, CQChartsLayer::Type::SELECTION);

        // draw annotations
        drawAnnotations(&device, CQChartsLayer::Type::MOUSE_OVER);
      }

      //---

      // draw view key
      drawKey(&device, CQChartsLayer::Type::SELECTION);

      // draw view key
      drawKey(&device, CQChartsLayer::Type::MOUSE_OVER);
    }

    overlayBuffer_->endPaint();
  }
}

//---

void
CQChartsView::
updateNoData()
{
  updateNoData_ = true;

  bool hasPlots       = ! plots().empty();
  bool hasAnnotations = this->hasAnnotations();

  if (! hasPlots && ! hasAnnotations) {
    invalidateObjects();

    update();
  }
}

void
CQChartsView::
showNoData(bool show)
{
  if (show) {
    if (! noDataText_) {
      noDataText_ = CQUtil::makeWidget<CQChartsDocument>(this, "noDataText");

      noDataText_->document()->setDocumentMargin(8);

      connect(noDataText_, SIGNAL(linkClicked(const QString &)),
              SLOT(noDataTextClicked(const QString &)));
    }

    noDataText_->setVisible(true);
  }
  else {
    if (noDataText_)
      noDataText_->setVisible(false);
  }
}

void
CQChartsView::
drawNoData(CQChartsPaintDevice *)
{
  QFont p_font = this->font().font();
  QFontMetricsF p_fm(p_font);

  int is = int(4*p_fm.height());

  auto *modelData = charts()->currentModelData();

  noDataText_->move(0, 0);
  noDataText_->resize(width(), height());

  //---

  auto liHtml = [](const QString &text) {
    return QString("<li>%1</li>").arg(text);
  };

  auto refHtml = [](const QString &ref, const QString &text) {
    return QString("<a href=\"charts://%1\">%2</a>").arg(ref).arg(text);
  };

  auto imgHtml = [](const QString &src, int s) {
    return QString("<img width=\"%1\" height=\"%1\" src=\"%2\">").arg(s).arg(src);
  };

  auto imgLiHtml = [&](const QString &ref, const QString &img, const QString &text) {
    return liHtml(refHtml(ref, imgHtml(img, is) + "&nbsp;" + text)) + "\n";
  };

  if (updateNoData_) {
    updateNoData_ = false;

    //---

    QString text;

    text += QString("<style type=\"text/css\">\n"
                    "h1 { color:#5279cd; font-size:32px; }\n"
                    "h2 { font-size:24px; }\n"
                    "ul { list-style-type: none; }\n"
                    "ul li { font-size:24px; display: block; height: %1px; line-height: %1px; }\n"
                    "ul li img { vertical-align: middle; }\n"
                    "</style>\n").arg(is);

    text += "<h1>Charts View</h1>\n";
    text += "<h2>No Plot Specified</h2>\n";

    text += "<p>&nbsp;</p>\n";

    if (! modelData) {
      text += "<p>No Model Data</p>\n";

      text += "<ul>\n";
      text += imgLiHtml("model", "nodata_models.svg", "Load Model");
      text += imgLiHtml("help" , "nodata_help.svg"  , "Help"      );
      text += "</ul>\n";
    }
    else {
      text += "<p>Create Plot and/or Model</p>\n";

      text += "<ul>\n";
      text += imgLiHtml("model", "nodata_models.svg", "Manage Models");
      text += imgLiHtml("plot" , "nodata_plot.svg"  , "Create Plot"  );
      text += imgLiHtml("help" , "nodata_help.svg"  , "Help"         );
      text += "</ul>\n";
    }

    noDataText_->setHtml(text);
  }
}

//---

bool
CQChartsView::
hasAnnotations() const
{
  return annotations().size();
}

void
CQChartsView::
drawAnnotations(CQChartsPaintDevice *device, const CQChartsLayer::Type &layerType)
{
  // set draw layer
  setDrawLayerType(layerType);

  for (auto &annotation : annotations()) {
    if      (layerType == CQChartsLayer::Type::SELECTION) {
      if (! annotation->isSelected())
        continue;
    }
    else if (layerType == CQChartsLayer::Type::MOUSE_OVER) {
      if (! annotation->isInside())
        continue;
    }

    annotation->draw(device);

    if (layerType == CQChartsLayer::Type::SELECTION) {
      if (mode() == CQChartsView::Mode::EDIT && annotation->isSelected())
        if (device->isInteractive()) {
          auto *painter = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

          annotation->drawEditHandles(painter->painter());
        }
    }
  }
}

void
CQChartsView::
drawKey(CQChartsPaintDevice *device, const CQChartsLayer::Type &layerType)
{
  // draw view key
  if (! key())
    return;

  if      (layerType == CQChartsLayer::Type::SELECTION) {
    if (! key()->isSelected())
      return;
  }
  else if (layerType == CQChartsLayer::Type::MOUSE_OVER) {
    if (! key()->isInside())
      return;
  }

  key()->draw(device);

  if (layerType == CQChartsLayer::Type::SELECTION) {
    if (mode() == CQChartsView::Mode::EDIT && key()->isSelected()) {
      if (device->isInteractive()) {
        auto *painter = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

        key()->drawEditHandles(painter->painter());
      }
    }
  }
}

void
CQChartsView::
lockPainter(bool lock)
{
  if (lock)
    painterMutex_.lock();
  else
    painterMutex_.unlock();
}

//------

void
CQChartsView::
setPenBrush(CQChartsPenBrush &penBrush, const CQChartsPenData &penData,
            const CQChartsBrushData &brushData) const
{
  setPen  (penBrush, penData  );
  setBrush(penBrush, brushData);
}

void
CQChartsView::
setPen(CQChartsPenBrush &penBrush, const CQChartsPenData &penData) const
{
  double width = CQChartsUtil::limitLineWidth(lengthPixelWidth(penData.width()));

  CQChartsUtil::setPen(penBrush.pen, penData.isVisible(), penData.color(), penData.alpha(),
                       width, penData.dash());
}

void
CQChartsView::
setBrush(CQChartsPenBrush &penBrush, const CQChartsBrushData &brushData) const
{
  CQChartsDrawUtil::setBrush(penBrush.brush, brushData);
}

//------

void
CQChartsView::
updateObjPenBrushState(const CQChartsObj *obj, CQChartsPenBrush &penBrush,
                       DrawType drawType) const
{
  updateObjPenBrushState(obj, ColorInd(), penBrush, drawType);
}

void
CQChartsView::
updateObjPenBrushState(const CQChartsObj *obj, const ColorInd &ic,
                       CQChartsPenBrush &penBrush, DrawType drawType) const
{
  if (! isBufferLayers()) {
    // inside and selected
    if      (obj->isInside() && obj->isSelected()) {
      if (selectedMode() != CQChartsView::HighlightDataMode::DIM_OTHER)
        updateSelectedObjPenBrushState(ic, penBrush, drawType);

      updateInsideObjPenBrushState(ic, penBrush, /*outline*/false, drawType);
    }
    // inside
    else if (obj->isInside()) {
      updateInsideObjPenBrushState(ic, penBrush, /*outline*/true, drawType);
    }
    // selected
    else if (obj->isSelected()) {
      if (selectedMode() != CQChartsView::HighlightDataMode::DIM_OTHER)
        updateSelectedObjPenBrushState(ic, penBrush, drawType);
    }
    else {
      if (selectedMode() == CQChartsView::HighlightDataMode::DIM_OTHER)
        updateSelectedObjPenBrushState(ic, penBrush, drawType);
    }
  }
  else {
    // inside
    if      (drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
      if (obj->isInside())
        updateInsideObjPenBrushState(ic, penBrush, /*outline*/true, drawType);
    }
    // selected
    else if (drawLayerType() == CQChartsLayer::Type::SELECTION) {
      if (obj->isSelected()) {
        if (selectedMode() != CQChartsView::HighlightDataMode::DIM_OTHER)
          updateSelectedObjPenBrushState(ic, penBrush, drawType);
      }
      else {
        if (selectedMode() == CQChartsView::HighlightDataMode::DIM_OTHER)
          updateSelectedObjPenBrushState(ic, penBrush, drawType);
      }
    }
  }
}

void
CQChartsView::
updateInsideObjPenBrushState(const ColorInd &ic, CQChartsPenBrush &penBrush,
                             bool outline, DrawType drawType) const
{
  // fill and stroke
  if (drawType != DrawType::LINE) {
    // outline box, symbol
    if      (insideMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor        opc;
      CQChartsAlpha alpha;

      if (penBrush.pen.style() != Qt::NoPen) {
        QColor pc = penBrush.pen.color();

        if (isInsideStroked())
          opc = interpInsideStrokeColor(ic);
        else
          opc = CQChartsUtil::invColor(pc);

        alpha = CQChartsAlpha(pc.alphaF());
      }
      else {
        QColor bc = penBrush.brush.color();

        if (isInsideStroked())
          opc = interpInsideStrokeColor(ic);
        else
          opc = CQChartsUtil::invColor(bc);
      }

      setPen(penBrush,
        CQChartsPenData(true, opc, alpha, insideStrokeWidth(), insideStrokeDash()));

      if (outline)
        setBrush(penBrush, CQChartsBrushData(false));
    }
    // fill box, symbol
    else if (insideMode() == CQChartsView::HighlightDataMode::FILL) {
      QColor bc = penBrush.brush.color();

      QColor ibc;

      if (isInsideFilled()) {
        ColorInd ic1 = ic; ic1.c = bc;

        ibc = interpInsideFillColor(ic1);
      }
      else
        ibc = insideColor(bc);

      CQChartsAlpha alpha;

      if (isBufferLayers())
        alpha = CQChartsAlpha(insideFillAlpha().value()*bc.alphaF());
      else
        alpha = CQChartsAlpha(bc.alphaF());

      setBrush(penBrush, CQChartsBrushData(true, ibc, alpha, insideFillPattern()));
    }
  }
  // just stroke
  else {
    QColor pc = penBrush.pen.color();

    QColor opc;

    if (isInsideStroked())
      opc = interpInsideStrokeColor(ic);
    else
      opc = CQChartsUtil::invColor(pc);

    CQChartsAlpha alpha(pc.alphaF());

    setPen(penBrush,
      CQChartsPenData(true, opc, alpha, insideStrokeWidth(), insideStrokeDash()));
  }
}

void
CQChartsView::
updateSelectedObjPenBrushState(const ColorInd &ic, CQChartsPenBrush &penBrush,
                               DrawType drawType) const
{
  // fill and stroke
  if      (drawType != DrawType::LINE) {
    // outline box, symbol
    if       (selectedMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor        opc;
      CQChartsAlpha alpha;

      if (penBrush.pen.style() != Qt::NoPen) {
        QColor pc = penBrush.pen.color();

        if (isSelectedStroked())
          opc = interpSelectedStrokeColor(ic);
        else
          opc = selectedColor(pc);

        alpha = CQChartsAlpha(pc.alphaF());
      }
      else {
        QColor bc = penBrush.brush.color();

        if (isSelectedStroked())
          opc = interpSelectedStrokeColor(ic);
        else
          opc = CQChartsUtil::invColor(bc);
      }

      setPen(penBrush,
        CQChartsPenData(true, opc, alpha, selectedStrokeWidth(), selectedStrokeDash()));

      setBrush(penBrush, CQChartsBrushData(false));
    }
    // fill box, symbol
    else if (selectedMode() == CQChartsView::HighlightDataMode::FILL) {
      QColor bc = penBrush.brush.color();

      QColor ibc;

      if (isSelectedFilled())
        ibc = interpSelectedFillColor(ic);
      else
        ibc = selectedColor(bc);

      CQChartsAlpha alpha;

      if (isBufferLayers())
        alpha = CQChartsAlpha(selectedFillAlpha().value()*bc.alphaF());
      else
        alpha = CQChartsAlpha(bc.alphaF());

      setBrush(penBrush, CQChartsBrushData(true, ibc, alpha, selectedFillPattern()));
    }
    // dim others
    else if (selectedMode() == CQChartsView::HighlightDataMode::DIM_OTHER) {
      QColor bc = penBrush.brush.color();

      QColor ibc;

      if (isSelectedFilled())
        ibc = interpSelectedFillColor(ic);
      else
        ibc = selectedColor(bc);

      CQChartsAlpha alpha;

      if (isBufferLayers())
        alpha = CQChartsAlpha(selectedFillAlpha().value()*bc.alphaF());
      else
        alpha = CQChartsAlpha(bc.alphaF());

      setBrush(penBrush, CQChartsBrushData(true, ibc, alpha, selectedFillPattern()));
    }
  }
  // just stroke
  else if (penBrush.pen.style() != Qt::NoPen) {
    QColor pc = penBrush.pen.color();

    QColor opc;

    if (isSelectedStroked())
      opc = interpSelectedStrokeColor(ic);
    else
      opc = CQChartsUtil::invColor(pc);

    CQChartsAlpha alpha(pc.alphaF());

    setPen(penBrush,
      CQChartsPenData(true, opc, alpha, selectedStrokeWidth(), selectedStrokeDash()));
  }
}

QColor
CQChartsView::
insideColor(const QColor &c) const
{
  return CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.8);
}

QColor
CQChartsView::
selectedColor(const QColor &c) const
{
  return CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.6);
}

//------

void
CQChartsView::
updateSlot()
{
  update();
}

//------

void
CQChartsView::
searchSlot()
{
  auto w = pixelToWindow(searchPos_);

  plotsAt(w, mouseData_.plots, mouseData_.plot, /*clear*/true, /*first*/true);

  //---

  setStatusText("");

  bool handled = false;

  processMouseDataPlots([&](CQChartsPlot *plot, const Point &pos) {
    auto w = plot->pixelToWindow(pos);

    if (plot->selectMove(w, ! handled))
      handled = true;

    return false;
  }, searchPos_);

  //---

  bool changed = false;

  for (auto &annotation : annotations()) {
    bool inside = annotation->contains(w);

    if (inside != annotation->isInside()) {
      annotation->setInside(inside);

      changed = true;
    }
  }

  if (changed) {
    invalidateOverlay();

    update();
  }
}

//------

void
CQChartsView::
annotationsAtPoint(const Point &w, Annotations &annotations) const
{
  annotations.clear();

  for (const auto &annotation : this->annotations()) {
    if (! annotation->contains(w))
      continue;

    annotations.push_back(annotation);
  }
}

//------

void
CQChartsView::
invalidateObjects()
{
  objectsBuffer_->setValid(false);
}

void
CQChartsView::
invalidateOverlay()
{
  overlayBuffer_->setValid(false);
}

//------

void
CQChartsView::
themeChangedSlot(const QString &name)
{
#if 0
  if (name == theme()->name()) {
    setSelectedFillColor(theme()->selectColor());
    setInsideFillColor  (theme()->insideColor());
  }
#else
  Q_UNUSED(name);
#endif
}

void
CQChartsView::
paletteChangedSlot(const QString &)
{
}

//------

void
CQChartsView::
showMenu(const Point &p)
{
  auto *popupMenu = createPopupMenu();

  //---

  // get all plots
  Plots allPlots;

  addPlots(allPlots);

  bool hasPlots = ! allPlots.empty();

  //---

  // get base plots
  PlotSet basePlots;

  addBasePlots(basePlots);

  //---

  // get current plot
  auto w = pixelToWindow(p);

  Plots         plots;
  CQChartsPlot* plot { nullptr };

  plotsAt(w, plots, plot);

  if (plot)
    setCurrentPlot(plot);

  auto *currentPlot = this->currentPlot(/*remap*/false);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *plotType = (currentPlot ? currentPlot->type() : nullptr);

  //---

  auto addSubMenu = [](QMenu *menu, const QString &name) {
    auto *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto createActionGroup = [](QMenu *menu) {
    return new QActionGroup(menu);
  };

  auto addAction = [&](QMenu *menu, const QString &name, const char *slotName) {
    auto *action = new QAction(name, menu);

    connect(action, SIGNAL(triggered()), this, slotName);

    menu->addAction(action);

    return action;
  };

  auto addCheckAction = [&](QMenu *menu, const QString &name, bool checked, const char *slotName) {
    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    connect(action, SIGNAL(triggered(bool)), this, slotName);

    menu->addAction(action);

    return action;
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

  //---

  if (hasPlots)
    addCheckAction(popupMenu, "Show Table", isShowTable(), SLOT(setShowTable(bool)));

  addCheckAction(popupMenu, "Show Settings", isShowSettings(), SLOT(setShowSettings(bool)));

  //---

  if (hasPlots) {
    popupMenu->addSeparator();

    //---

    auto *modeMenu = addSubMenu(popupMenu, "Mode");

    auto *modeActionGroup = createActionGroup(modeMenu);

    addGroupCheckAction(modeActionGroup, "Select", mode() == Mode::SELECT , SLOT(selectModeSlot()));
    addGroupCheckAction(modeActionGroup, "Zoom"  , mode() == Mode::ZOOM_IN, SLOT(zoomModeSlot()));
    addGroupCheckAction(modeActionGroup, "Pan"   , mode() == Mode::PAN    , SLOT(panModeSlot()));

    if (plotType && plotType->canProbe())
      addGroupCheckAction(modeActionGroup, "Probe", mode() == Mode::PROBE, SLOT(probeModeSlot()));

    addGroupCheckAction(modeActionGroup, "Query" , mode() == Mode::QUERY , SLOT(queryModeSlot()));
    addGroupCheckAction(modeActionGroup, "Edit"  , mode() == Mode::EDIT  , SLOT(editModeSlot()));
    addGroupCheckAction(modeActionGroup, "Region", mode() == Mode::REGION, SLOT(regionModeSlot()));

    modeActionGroup->setExclusive(true);

    modeMenu->addActions(modeActionGroup->actions());
  }

  //---

  CQChartsPlot::Objs objs;

  allSelectedObjs(objs);

  if (objs.size() == 1) {
    popupMenu->addSeparator();

    //---

    auto *annotation = qobject_cast<CQChartsAnnotation *>(objs[0]);
    auto *axis       = qobject_cast<CQChartsAxis       *>(objs[0]);
    auto *key        = qobject_cast<CQChartsKey        *>(objs[0]);
    auto *title      = qobject_cast<CQChartsTitle      *>(objs[0]);

    if (annotation || axis || key || title)
      addAction(popupMenu, "Edit", SLOT(editObjectSlot()));
  }

  //---

  if (hasPlots) {
    addCheckAction(popupMenu, "Auto Resize", isAutoSize(), SLOT(setAutoSize(bool)));

    if (! isAutoSize())
      addAction(popupMenu, "Resize to View", SLOT(resizeToView()));
  }

  //---

  if (basePlots.size() > 1) {
    if (! isScrolled())
      addAction(popupMenu, "Maximize", SLOT(maximizePlotsSlot()));
    else
      addAction(popupMenu, "Restore", SLOT(restorePlotsSlot()));
  }

  //---

  if (key() && basePlots.size() > 1) {
    auto *viewKeyMenu = addSubMenu(popupMenu, "View Key");

    addCheckAction(viewKeyMenu, "Visible", key()->isVisible(), SLOT(viewKeyVisibleSlot(bool)));

    //---

    using KeyLocationActionMap = std::map<CQChartsKeyLocation::Type, QAction *>;

    KeyLocationActionMap keyLocationActionMap;

    auto *keyLocationMenu = addSubMenu(viewKeyMenu, "Location");

    auto *keyLocationActionGroup = createActionGroup(keyLocationMenu);

    auto addKeyLocationGroupAction =
     [&](const QString &label, const CQChartsKeyLocation::Type &location) {
      auto *action = new QAction(label, keyLocationMenu);

      action->setCheckable(true);

      keyLocationActionMap[location] = action;

      keyLocationActionGroup->addAction(action);

      return action;
    };

    QStringList locationNames = QStringList() <<
      "Top Left"    << "Top Center"    << "Top Right"    <<
      "Center Left" << "Center Center" << "Center Right" <<
      "Bottom Left" << "Bottom Center" << "Bottom Right" <<
      "Absolute Position" << "Absolute Rectangle";

    for (const auto &name : locationNames) {
      CQChartsKeyLocation::Type type;

      if (! CQChartsKeyLocation::decodeString(name, type))
        assert(false);

      addKeyLocationGroupAction(name, type);
    }

    keyLocationActionGroup->setExclusive(true);

    CQChartsKeyLocation::Type location = key()->location().type();

    keyLocationActionMap[location]->setChecked(true);

    connect(keyLocationActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(viewKeyPositionSlot(QAction *)));

    keyLocationMenu->addActions(keyLocationActionGroup->actions());
  }

  //---

  if (allPlots.size() == 1 && window()) {
    addCheckAction(popupMenu, "X Overview", window()->isXRangeMap(), SLOT(xRangeMapSlot(bool)));
    addCheckAction(popupMenu, "Y Overview", window()->isYRangeMap(), SLOT(yRangeMapSlot(bool)));
  }

  //---

  // Add plots
  if (allPlots.size() > 1) {
    auto *plotsMenu = addSubMenu(popupMenu, "Plots");

    auto *plotsGroup = createActionGroup(plotsMenu);

    for (const auto &plot : allPlots) {
      int ind = getIndForPlot(plot);

      auto *plotAction =
        addGroupCheckAction(plotsGroup, plot->id(), false, SLOT(currentPlotSlot()));

      if (currentPlot)
        plotAction->setChecked(plot == currentPlot);
      else
        plotAction->setChecked(currentPlotInd() == ind);

      plotAction->setData(ind);
    }

    plotsMenu->addActions(plotsGroup->actions());
  }

  //------

  if (plotType && plotType->hasKey()) {
    auto *plotKeyMenu = addSubMenu(popupMenu, "Plot Key");

    //---

    auto addKeyCheckAction = [&](const QString &label, bool checked, const char *slot) {
      auto *action = new QAction(label, plotKeyMenu);

      action->setCheckable(true);
      action->setChecked(checked);

      connect(action, SIGNAL(triggered(bool)), this, slot);

      plotKeyMenu->addAction(action);

      return action;
    };

    //---

    auto *plotKey = (basePlot ? basePlot->key() : nullptr);

    bool visibleChecked = (plotKey && plotKey->isVisible());

    auto *keyVisibleAction =
      addKeyCheckAction("Visible", visibleChecked, SLOT(plotKeyVisibleSlot(bool)));

    if (plotKey) {
      if (plotKey->isEmpty())
        keyVisibleAction->setEnabled(false);
    }
    else
      keyVisibleAction->setEnabled(false);

    //---

    using KeyLocationActionMap = std::map<CQChartsKeyLocation::Type, QAction *>;

    KeyLocationActionMap keyLocationActionMap;

    auto *keyLocationMenu = addSubMenu(plotKeyMenu, "Location");

    auto *keyLocationActionGroup = createActionGroup(keyLocationMenu);

    auto addKeyLocationGroupAction =
     [&](const QString &label, const CQChartsKeyLocation::Type &location) {
      auto *action = new QAction(label, keyLocationMenu);

      action->setCheckable(true);

      keyLocationActionMap[location] = action;

      keyLocationActionGroup->addAction(action);

      return action;
    };

    QStringList locationNames = QStringList() <<
      "Top Left"    << "Top Center"    << "Top Right"    <<
      "Center Left" << "Center Center" << "Center Right" <<
      "Bottom Left" << "Bottom Center" << "Bottom Right" <<
      "Absolute Position" << "Absolute Rectangle" << "Auto";

    for (const auto &name : locationNames) {
      CQChartsKeyLocation::Type type;

      if (! CQChartsKeyLocation::decodeString(name, type))
        assert(false);

      addKeyLocationGroupAction(name, type);
    }

    keyLocationActionGroup->setExclusive(true);

    if (plotKey) {
      CQChartsKeyLocation::Type location = plotKey->location().type();

      auto p = keyLocationActionMap.find(location);

      if (p != keyLocationActionMap.end())
        (*p).second->setChecked(true);
    }

    connect(keyLocationActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(plotKeyPositionSlot(QAction *)));

    keyLocationMenu->addActions(keyLocationActionGroup->actions());

    //---

    bool insideXChecked = (plotKey && plotKey->isInsideX());
    bool insideYChecked = (plotKey && plotKey->isInsideY());

    (void) addKeyCheckAction("Inside X", insideXChecked, SLOT(plotKeyInsideXSlot(bool)));
    (void) addKeyCheckAction("Inside Y", insideYChecked, SLOT(plotKeyInsideYSlot(bool)));
  }

  //------

  using AxisSideActionMap = std::map<CQChartsAxisSide, QAction *>;

  //------

  if (currentPlot && currentPlot->hasXAxis()) {
    auto *xAxis = (basePlot ? basePlot->xAxis() : nullptr);

    auto *xAxisMenu = addSubMenu(popupMenu, "X Axis");

    //---

    auto *xAxisVisibleAction =
      addCheckAction(xAxisMenu, "Visible", false, SLOT(xAxisVisibleSlot(bool)));

    if (xAxis)
      xAxisVisibleAction->setChecked(xAxis->isVisible());

    //---

    auto *xAxisGridAction = addCheckAction(xAxisMenu, "Grid", false, SLOT(xAxisGridSlot(bool)));

    if (xAxis)
      xAxisGridAction->setChecked(xAxis->isMajorGridLinesDisplayed());

    //---

    AxisSideActionMap xAxisSideActionMap;

    auto *xAxisSideMenu = addSubMenu(xAxisMenu, "Side");

    auto *xAxisSideGroup = createActionGroup(xAxisMenu);

    auto addXAxisSideGroupAction = [&](const QString &label, const CQChartsAxisSide &side) {
      auto *action = new QAction(label, xAxisSideMenu);

      action->setCheckable(true);

      xAxisSideActionMap[side] = action;

      xAxisSideGroup->addAction(action);

      return action;
    };

    addXAxisSideGroupAction("Bottom", CQChartsAxisSide::Type::BOTTOM_LEFT);
    addXAxisSideGroupAction("Top"   , CQChartsAxisSide::Type::TOP_RIGHT  );

    if (xAxis)
      xAxisSideActionMap[xAxis->side()]->setChecked(true);

    connect(xAxisSideGroup, SIGNAL(triggered(QAction *)), this, SLOT(xAxisSideSlot(QAction *)));

    xAxisSideMenu->addActions(xAxisSideGroup->actions());
  }

  //------

  if (currentPlot && currentPlot->hasYAxis()) {
    auto *yAxis = (basePlot ? basePlot->yAxis() : nullptr);

    auto *yAxisMenu = addSubMenu(popupMenu, "Y Axis");

    //---

    auto *yAxisVisibleAction =
      addCheckAction(yAxisMenu, "Visible", false, SLOT(yAxisVisibleSlot(bool)));

    if (yAxis)
      yAxisVisibleAction->setChecked(yAxis->isVisible());

    //---

    auto *yAxisGridAction = addCheckAction(yAxisMenu, "Grid", false, SLOT(yAxisGridSlot(bool)));

    if (yAxis)
      yAxisGridAction->setChecked(yAxis->isMajorGridLinesDisplayed());

    //---

    AxisSideActionMap yAxisSideActionMap;

    auto *yAxisSideMenu = addSubMenu(yAxisMenu, "Side");

    auto *yAxisSideGroup = createActionGroup(yAxisMenu);

    auto addYAxisSideGroupAction = [&](const QString &label, const CQChartsAxisSide &side) {
      auto *action = new QAction(label, yAxisSideMenu);

      action->setCheckable(true);

      yAxisSideActionMap[side] = action;

      yAxisSideGroup->addAction(action);

      return action;
    };

    addYAxisSideGroupAction("Left" , CQChartsAxisSide::Type::BOTTOM_LEFT);
    addYAxisSideGroupAction("Right", CQChartsAxisSide::Type::TOP_RIGHT  );

    if (yAxis)
      yAxisSideActionMap[yAxis->side()]->setChecked(true);

    connect(yAxisSideGroup, SIGNAL(triggered(QAction *)), this, SLOT(yAxisSideSlot(QAction *)));

    yAxisSideMenu->addActions(yAxisSideGroup->actions());
  }

  //---

  if (plotType && plotType->hasTitle()) {
    auto *title = (basePlot ? basePlot->title() : nullptr);

    auto *titleMenu = addSubMenu(popupMenu, "Title");

    auto *titleVisibleAction =
      addCheckAction(titleMenu, "Visible", false, SLOT(titleVisibleSlot(bool)));

    if (title)
      titleVisibleAction->setChecked(title->isVisible());

    //---

    using TitleLocationActionMap = std::map<CQChartsTitleLocation::Type, QAction *>;

    TitleLocationActionMap titleLocationActionMap;

    auto *titleLocationMenu = addSubMenu(titleMenu, "Location");

    auto *titleLocationGroup = createActionGroup(titleMenu);

    auto addTitleLocationGroupAction =
     [&](const QString &label, const CQChartsTitleLocation::Type &location) {
      auto *action = new QAction(label, titleLocationMenu);

      action->setCheckable(true);

      titleLocationActionMap[location] = action;

      titleLocationGroup->addAction(action);

      return action;
    };

    QStringList locationNames = QStringList() <<
      "Top" << "Center" << "Bottom" << "Absolute Position" << "Absolute Rectangle";

    for (const auto &name : locationNames) {
      CQChartsTitleLocation::Type type;

      if (! CQChartsTitleLocation::decodeString(name, type))
        assert(false);

      addTitleLocationGroupAction(name, type);
    }

    if (title)
      titleLocationActionMap[title->location().type()]->setChecked(true);

    connect(titleLocationGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(titleLocationSlot(QAction *)));

    titleLocationMenu->addActions(titleLocationGroup->actions());
  }

  //------

  if (hasPlots) {
    auto *invertXAction = addCheckAction(popupMenu, "Invert X", false, SLOT(invertXSlot(bool)));
    auto *invertYAction = addCheckAction(popupMenu, "Invert Y", false, SLOT(invertYSlot(bool)));

    if (basePlot) {
      invertXAction->setChecked(basePlot->isInvertX());
      invertYAction->setChecked(basePlot->isInvertY());
    }
  }

  //------

  if (basePlot && basePlot->isTabbed()) {
    popupMenu->addSeparator();

    addAction(popupMenu, "Next", SLOT(nextSlot()));
    addAction(popupMenu, "Prev", SLOT(prevSlot()));
  }

  //---

  if (hasPlots) {
    popupMenu->addSeparator();

    addAction(popupMenu, "Fit"      , SLOT(fitSlot()));
    addAction(popupMenu, "Zoom Full", SLOT(zoomFullSlot()));
  }

  //---

  popupMenu->addSeparator();

  auto *themeMenu = addSubMenu(popupMenu, "Theme");

  auto *interfaceGroup = createActionGroup(themeMenu);
  auto *themeGroup     = createActionGroup(themeMenu);

  auto addInterfaceAction = [&](const QString &label, const char *slotName) {
    auto *action = new QAction(label, themeMenu);

    action->setCheckable(true);

    interfaceGroup->addAction(action);

    connect(action, SIGNAL(triggered()), this, slotName);

    return action;
  };

  auto addThemeAction = [&](const QString &label, const char *slotName) {
    auto *action = new QAction(label, themeMenu);

    action->setCheckable(true);

    themeGroup->addAction(action);

    connect(action, SIGNAL(triggered()), this, slotName);

    return action;
  };

  auto *lightPaletteAction = addInterfaceAction("Light", SLOT(lightPaletteSlot()));
  auto *darkPaletteAction  = addInterfaceAction("Dark" , SLOT(darkPaletteSlot()));

  lightPaletteAction->setChecked(! isDark());
  darkPaletteAction ->setChecked(  isDark());

  themeMenu->addActions(interfaceGroup->actions());

  //---

  QStringList themeNames;

  CQColorsMgrInst->getThemeNames(themeNames);

  for (const auto &themeName : themeNames) {
    auto *theme = CQColorsMgrInst->getNamedTheme(themeName) ;

    auto *themeAction = addThemeAction(theme->desc(), SLOT(themeNameSlot()));

    themeAction->setData(theme->name());

    themeAction->setChecked(this->themeName().name() == theme->name());
  }

  themeMenu->addActions(themeGroup->actions());

  //---

  // add Menus for current plot

  if (currentPlot) {
    if (currentPlot->addMenuItems(popupMenu))
      popupMenu->addSeparator();
  }

  //---

  if (hasPlots) {
    auto *printMenu = addSubMenu(popupMenu, "Print");

    addAction(printMenu, "PNG", SLOT(printPNGSlot()));
    addAction(printMenu, "SVG", SLOT(printSVGSlot()));

    addAction(printMenu, "SVG/Html", SLOT(writeSVGSlot()));
    addAction(printMenu, "JS/Html" , SLOT(writeScriptSlot()));
  }

  //---

  if (hasPlots) {
    if (CQChartsEnv::getBool("CQ_CHARTS_DEBUG", true)) {
      auto *showBoxesAction =
        addCheckAction(popupMenu, "Show Boxes", false, SLOT(showBoxesSlot(bool)));

      if (basePlot)
        showBoxesAction->setChecked(basePlot->showBoxes());

      //---

      addCheckAction(popupMenu, "Buffer Layers", isBufferLayers(), SLOT(bufferLayersSlot(bool)));
    }
  }

  //---

  popupMenu->popup(mapToGlobal(p.qpointi()));
}

QMenu *
CQChartsView::
createPopupMenu()
{
  delete popupMenu_;

  popupMenu_ = new QMenu(this);

  return popupMenu_;
}

//------

void
CQChartsView::
editObjectSlot()
{
  CQChartsPlot::Objs objs;

  allSelectedObjs(objs);

  auto *annotation = qobject_cast<CQChartsAnnotation *>(objs[0]);
  auto *axis       = qobject_cast<CQChartsAxis       *>(objs[0]);
  auto *key        = qobject_cast<CQChartsKey        *>(objs[0]);
  auto *title      = qobject_cast<CQChartsTitle      *>(objs[0]);

  if (annotation) {
    delete editAnnotationDlg_;

    editAnnotationDlg_ = new CQChartsEditAnnotationDlg(this, annotation);

    editAnnotationDlg_->show();
    editAnnotationDlg_->raise();
  }
  else if (axis) {
    delete editAxisDlg_;

    editAxisDlg_ = new CQChartsEditAxisDlg(this, axis);

    editAxisDlg_->show();
    editAxisDlg_->raise();
  }
  else if (key) {
    delete editKeyDlg_;

    editKeyDlg_ = new CQChartsEditKeyDlg(this, key);

    editKeyDlg_->show();
    editKeyDlg_->raise();
  }
  else if (title) {
    delete editTitleDlg_;

    editTitleDlg_ = new CQChartsEditTitleDlg(this, title);

    editTitleDlg_->show();
    editTitleDlg_->raise();
  }
}

//------

void
CQChartsView::
viewKeyVisibleSlot(bool b)
{
  if (key() && b != key()->isVisible()) {
    key()->setVisible(b);

    invalidateObjects();

    update();
  }
}

void
CQChartsView::
viewKeyPositionSlot(QAction *action)
{
  auto *viewKey = key();
  if (! viewKey) return;

  CQChartsKeyLocation::Type location;

  if (! CQChartsKeyLocation::decodeString(action->text(), location))
    assert(false);

  viewKey->setLocation(location);

  invalidateObjects();

  update();
}

//------

void
CQChartsView::
plotKeyVisibleSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *plotKey = (basePlot ? basePlot->key() : nullptr);

  if (! plotKey)
    return;

  if (b != plotKey->isVisible()) {
    plotKey->setVisible(b);

    if (b)
      basePlot->updateKeyPosition(/*force*/true);
  }
}

void
CQChartsView::
plotKeyPositionSlot(QAction *action)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *plotKey = (basePlot ? basePlot->key() : nullptr);

  if (plotKey) {
    CQChartsKeyLocation::Type location;

    if (! CQChartsKeyLocation::decodeString(action->text(), location))
      assert(false);

    plotKey->setLocation(location);
  }
}

void
CQChartsView::
plotKeyInsideXSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *plotKey = (basePlot ? basePlot->key() : nullptr);

  if (! plotKey)
    return;

  if (b != plotKey->isInsideX()) {
    plotKey->setInsideX(b);

    if (b)
      basePlot->updateKeyPosition(/*force*/true);
  }
}

void
CQChartsView::
plotKeyInsideYSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *plotKey = (basePlot ? basePlot->key() : nullptr);

  if (! plotKey)
    return;

  if (b != plotKey->isInsideY()) {
    plotKey->setInsideY(b);

    if (b)
      basePlot->updateKeyPosition(/*force*/true);
  }
}

void
CQChartsView::
xAxisVisibleSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *xAxis = (basePlot ? basePlot->xAxis() : nullptr);

  if (xAxis)
    xAxis->setVisible(b);
}

void
CQChartsView::
xAxisGridSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *xAxis = (basePlot ? basePlot->xAxis() : nullptr);

  if (xAxis)
    xAxis->setGridLinesDisplayed(b ? CQChartsAxis::GridLinesDisplayed::MAJOR :
                                     CQChartsAxis::GridLinesDisplayed::NONE);
}

void
CQChartsView::
xAxisSideSlot(QAction *action)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *xAxis = (basePlot ? basePlot->xAxis() : nullptr);

  if (xAxis) {
    if      (action->text() == "Bottom")
      xAxis->setSide(CQChartsAxisSide::Type::BOTTOM_LEFT);
    else if (action->text() == "Top")
      xAxis->setSide(CQChartsAxisSide::Type::TOP_RIGHT);
  }
}

void
CQChartsView::
yAxisVisibleSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *yAxis = (basePlot ? basePlot->yAxis() : nullptr);

  if (yAxis)
    yAxis->setVisible(b);
}

void
CQChartsView::
yAxisGridSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *yAxis = (basePlot ? basePlot->yAxis() : nullptr);

  if (yAxis)
    yAxis->setGridLinesDisplayed(b ? CQChartsAxis::GridLinesDisplayed::MAJOR :
                                     CQChartsAxis::GridLinesDisplayed::NONE);
}

void
CQChartsView::
yAxisSideSlot(QAction *action)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *yAxis = (basePlot ? basePlot->yAxis() : nullptr);

  if (yAxis) {
    if      (action->text() == "Left")
      yAxis->setSide(CQChartsAxisSide::Type::BOTTOM_LEFT);
    else if (action->text() == "Right")
      yAxis->setSide(CQChartsAxisSide::Type::TOP_RIGHT);
  }
}

void
CQChartsView::
titleVisibleSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *title = (basePlot ? basePlot->title() : nullptr);

  if (title)
    title->setVisible(b);
}

void
CQChartsView::
titleLocationSlot(QAction *action)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  auto *title = (basePlot ? basePlot->title() : nullptr);

  if (title) {
    CQChartsTitleLocation::Type location;

    if (! CQChartsTitleLocation::decodeString(action->text(), location))
      assert(false);

    title->setLocation(location);
  }
}

void
CQChartsView::
invertXSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->setInvertX(b);
}

void
CQChartsView::
invertYSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->setInvertY(b);
}

//------

void
CQChartsView::
nextSlot()
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->cycleNextPlot();
}

void
CQChartsView::
prevSlot()
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->cyclePrevPlot();
}

//------

void
CQChartsView::
fitSlot()
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->autoFit();
}

void
CQChartsView::
zoomFullSlot()
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->zoomFull();
}

//------

void
CQChartsView::
noDataTextClicked(const QString &name)
{
  if      (name == "model")
    manageModelsSlot();
  else if (name == "plot")
    addPlotSlot();
  else if (name == "help")
    helpSlot();
}

void
CQChartsView::
manageModelsSlot()
{
  auto *modelData = charts()->currentModelData();

  if (! modelData)
    (void) charts()->loadModelDlg();
  else
    (void) charts()->manageModelsDlg();
}

void
CQChartsView::
addPlotSlot()
{
  auto *charts = this->charts();

  auto *modelData = charts->currentModelData();
  if (! modelData) return;

  auto *createPlotDlg = charts->createPlotDlg(modelData);

  createPlotDlg->setViewName(this->id());
}

void
CQChartsView::
helpSlot()
{
  auto *charts = this->charts();

  CQChartsHelpDlgMgrInst->showDialog(charts);
}

//------

void
CQChartsView::
lightPaletteSlot()
{
  setDark(false);
}

void
CQChartsView::
darkPaletteSlot()
{
  setDark(true);
}

void
CQChartsView::
themeNameSlot()
{
  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  QString name = action->data().toString();

  themeSlot(name);
}

void
CQChartsView::
themeSlot(const QString &name)
{
  setThemeName(CQChartsThemeName(name));

  updateTheme();
}

void
CQChartsView::
updateTheme()
{
#if 0
  setSelectedFillColor(theme()->selectColor());
  setInsideFillColor  (theme()->insideColor());
#endif

  setInsideStrokeWidth(CQChartsLength("2px"));

  updateAll();

  emit themePalettesChanged();
}

bool
CQChartsView::
isDark() const
{
  return charts()->isDark();
}

void
CQChartsView::
setDark(bool b)
{
  charts()->setDark(b);

  updateAll();
}

//------

bool
CQChartsView::
printFile(const QString &filename, CQChartsPlot *plot)
{
  auto p = filename.lastIndexOf(".");

  if (p > 0) {
    QString suffix = filename.mid(p + 1).toLower();

    if      (suffix == "png")
      return printPNG(filename, plot);
    else if (suffix == "svg")
      return printSVG(filename, plot);
    else if (suffix == "js")
      return writeScript(filename, plot);
    else
      return printPNG(filename, plot);
  }
  else {
    return printPNG(filename, plot);
  }
}

void
CQChartsView::
printPNGSlot()
{
  QString dir = QDir::current().dirName() + "/charts.png";

  QString fileName = QFileDialog::getSaveFileName(this, "Print PNG", dir, "Files (*.png)");
  if (! fileName.length()) return; // cancelled

  printPNGSlot(fileName);
}

void
CQChartsView::
printPNGSlot(const QString &filename)
{
  printPNG(filename);
}

void
CQChartsView::
printSVGSlot()
{
  QString dir = QDir::current().dirName() + "/charts.svg";

  QString fileName = QFileDialog::getSaveFileName(this, "Print SVG", dir, "Files (*.svg)");
  if (! fileName.length()) return; // cancelled

  printSVGSlot(fileName);
}

void
CQChartsView::
printSVGSlot(const QString &filename)
{
  printSVG(filename);
}

void
CQChartsView::
writeSVGSlot()
{
  QString dir = QDir::current().dirName() + "/charts_svg.html";

  QString fileName = QFileDialog::getSaveFileName(this, "Write SVG/Html", dir, "Files (*.html)");
  if (! fileName.length()) return; // cancelled

  writeSVGSlot(fileName);
}

void
CQChartsView::
writeSVGSlot(const QString &filename)
{
  writeSVG(filename);
}

void
CQChartsView::
writeScriptSlot(const QString &filename)
{
  writeScript(filename);
}

void
CQChartsView::
writeScriptSlot()
{
  QString dir = QDir::current().dirName() + "/charts_js.html";

  QString fileName = QFileDialog::getSaveFileName(this, "Write JS/Html", dir, "Files (*.html)");
  if (! fileName.length()) return; // cancelled

  writeScriptSlot(fileName);
}

bool
CQChartsView::
printPNG(const QString &filename, CQChartsPlot *plot)
{
  int w = width ();
  int h = height();

  QImage image = CQChartsUtil::initImage(QSize(w, h));

  QPainter painter;

  if (! painter.begin(&image))
    return false;

  paint(&painter, plot);

  painter.end();

  if (plot) {
    auto pixelRect = plot->calcPlotPixelRect();

    image = image.copy(pixelRect.qrecti());
  }

  return image.save(filename);
}

bool
CQChartsView::
printSVG(const QString &filename, CQChartsPlot *plot)
{
  bool buffer = isBufferLayers();

  setBufferLayers(false);

  QSvgGenerator generator;

  generator.setFileName(filename);

  QPainter painter;

  bool rc = painter.begin(&generator);

  if (rc) {
    paint(&painter, plot);

    painter.end();
  }

  setBufferLayers(buffer);

  return rc;
}

bool
CQChartsView::
writeSVG(const QString &filename, CQChartsPlot *plot)
{
  auto os = std::ofstream(filename.toStdString(), std::ofstream::out);

  //---

  CQChartsJS::writeHeadTitle(os, title());

  //---

  os << "<style>\n";

  // css (tooltip)
  CQChartsJS::writeToolTipCSS(os);

  os << "</style>\n";

  //---

  os << "<script type=\"text/javascript\">\n";
  os << "'use strict';\n";

  //---

  // tooltip procs
  CQChartsJS::writeToolTipProcs(os, CQChartsPaintDevice::Type::SVG);

  //---

  // write defined SVG procs
  os << "\n";

  for (const auto &pp : charts()->procs(CQCharts::ProcType::SVG)) {
    const auto &proc = pp.second;

    os << "function " << proc.name.toStdString() << "(" << proc.args.toStdString() << ") {\n";
    os << "  " << proc.body.toStdString() << "\n";
    os << "}\n";
    os << "\n";
  }

  //---

  os << "</script>\n";

  //---

  os << "</head>\n";

  os << "<body>\n";

  //---

  auto *th = const_cast<CQChartsView *>(this);

  CQChartsSVGPaintDevice device(th, os);

  //---

  // write custom html for annotations
  if (hasAnnotations()) {
    for (auto &annotation : annotations()) {
      annotation->writeHtml(&device);
    }
  }

  // write custom html for plots
  if (plot) {
    CQChartsSVGPaintDevice device(const_cast<CQChartsPlot *>(plot), os);

    plot->writeHtml(&device);
  }
  else {
    for (auto &plot : plots()) {
      CQChartsSVGPaintDevice device(const_cast<CQChartsPlot *>(plot), os);

      plot->writeHtml(&device);
    }
  }

  //---

  // svg block
  os << "<svg xmlns=\"http://www.w3.org/2000/svg\""
        " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
        " version=\"1.2\""
        " width=\"" << width() << "\""
        " height=\"" << height() << "\">\n";

  //---

  // draw background
  drawBackground(&device);

  //---

  // draw specific plot
  if (plot) {
    CQChartsSVGPaintDevice device(const_cast<CQChartsPlot *>(plot), os);

    plot->writeSVG(&device);
  }
  // draw all plots
  else {
    for (auto &plot : plots()) {
      CQChartsSVGPaintDevice device(const_cast<CQChartsPlot *>(plot), os);

      plot->writeSVG(&device);
    }

    //---

    if (hasAnnotations()) {
      CQChartsSVGPaintDevice device(th, os);

      // draw annotations
      drawAnnotations(&device, CQChartsLayer::Type::ANNOTATION);
    }
  }

  os << "</svg>\n";

  //---

  // tooltip div
  os << "<div id=\"tooltip\" display=\"none\" style=\"position: absolute; display: none;\">";
  os << "</div>\n";
  os << "\n";

  //---

  // log text
  os << "<p id=\"log_message\"></p>\n";

  //---

  os << "</body>\n";
  os << "</html>\n";

  return true;
}

bool
CQChartsView::
writeScript(const QString &filename, CQChartsPlot *plot)
{
  auto os = std::ofstream(filename.toStdString(), std::ofstream::out);

  //---

  CQChartsJS::writeHeadTitle(os, title());

  //---

  os << "<style>\n";

  // css (canvas, tooltip)
  CQChartsJS::writeCanvasCSS (os);
  CQChartsJS::writeToolTipCSS(os);

  os << "</style>\n";

  //---

  os << "<script type=\"text/javascript\">\n";
  os << "'use strict';\n";

  //---

  // tooltip procs
  CQChartsJS::writeToolTipProcs(os, CQChartsPaintDevice::Type::SCRIPT);

  //---

  // write defined SCRIPT procs
  for (const auto &pp : charts()->procs(CQCharts::ProcType::SCRIPT)) {
    const auto &proc = pp.second;

    os << "function " << proc.name.toStdString() << "(" << proc.args.toStdString() << ") {\n";
    os << "  " << proc.body.toStdString() << "\n";
    os << "}\n";
    os << "\n";
  }

  //---

  CQChartsJS::writeLogProc(os);

  //---

  // write class object and procs
  os << "var charts = new Charts();\n";
  os << "\n";

  CQChartsJS::writeWindowFunctions(os);

  os << "function Charts () {\n";
  os << "  this.plots = [];\n";
  os << "}\n";

  //---

  os << "\n";
  os <<
  "Charts.prototype.init = function() {\n"
  "  this.canvas = document.getElementById(\"canvas\");\n"
  "  this.gc = this.canvas.getContext(\"2d\");\n"
  "\n";

  os << "  this.pwidth = " << width(); os << ";\n";
  os << "  this.pheight = " << height(); os << ";\n";
  os << "  this.aspect = " << aspect(); os << ";\n";
  os << "\n";

  os <<
  "  this.canvas.width  = window.innerWidth;\n"
  "  this.canvas.height = window.innerHeight;\n"
  "\n"
  "  this.invertX = 0;\n"
  "  this.invertY = 0;\n"
  "\n"
  "  this.vxmin = 0.0;\n"
  "  this.vymin = 0.0;\n"
  "  this.vxmax = 100.0;\n"
  "  this.vymax = 100.0;\n"
  "\n"
  "  this.xmin = 0.0;\n"
  "  this.ymin = 0.0;\n"
  "  this.xmax = 1.0;\n"
  "  this.ymax = 1.0;\n"
  "\n"
  "  this.canvas.addEventListener(\"mousedown\", this.eventMouseDown, false);\n"
  "  this.canvas.addEventListener(\"mousemove\", this.eventMouseMove, false);\n"
  "  this.canvas.addEventListener(\"mouseup\"  , this.eventMouseUp  , false);\n"
  "\n";

  //---

  // create specific plot object
  if (plot) {
    std::string plotId = "plot_" + plot->id().toStdString();

    os << "  this." << plotId << " = new Charts_" << plotId << "();\n";
    os << "  this.plots.push(this." << plotId << ");\n";
    os << "  this." << plotId << ".init();\n";
  }
  // create plot objects
  else {
    int i = 0;

    for (auto &plot : plots()) {
      std::string plotId = "plot_" + plot->id().toStdString();

      if (i > 0)
        os << "\n";

      os << "  this." << plotId << " = new Charts_" << plotId << "();\n";
      os << "  this.plots.push(this." << plotId << ");\n";
      os << "  this." << plotId << ".init();\n";

      ++i;
    }
  }

  os << "}\n";
  os << "\n";

  //---

  // write procs
  CQChartsJS::writeEventProcs(os);
  CQChartsJS::writeConvProcs (os);
  CQChartsJS::writeDrawProcs (os);

  //---

  os << "\n";

  CQChartsJS::writeInsideProcs(os);

  //---

  // draw background proc
  CQChartsScriptPaintDevice device(const_cast<CQChartsView *>(this), os);

  os << "Charts.prototype.drawBackground = function() {\n";

  drawBackground(&device);

  os << "}\n";

  //---

  // draw annotations proc
  if (hasAnnotations()) {
    os << "\n";
    os << "Charts.prototype.drawAnnotations = function() {\n";

    drawAnnotations(&device, CQChartsLayer::Type::ANNOTATION);

    os << "}\n";
  }

  //---

  os << "\n";
  os << "Charts.prototype.update = function() {\n";

  os << "  this.drawBackground();\n";
  os << "\n";

  // draw specific plot
  if (plot) {
    std::string plotId = "plot_" + plot->id().toStdString();

    os << "  this." << plotId << ".draw();\n";
  }
  // draw all plots
  else {
    for (auto &plot : plots()) {
      std::string plotId = "plot_" + plot->id().toStdString();

      os << "  this." << plotId << ".draw();\n";
    }
  }

  //---

  // draw view annotations
  if (hasAnnotations()) {
    os << "\n";
    os << "  this.invertX = 0;\n";
    os << "  this.invertY = 0;\n";
    os << "\n";
    os << "  this.vxmin = 0.0;\n";
    os << "  this.vymin = 0.0;\n";
    os << "  this.vxmax = 100.0;\n";
    os << "  this.vymax = 100.0;\n";
    os << "\n";

    os << "  this.xmin = 0.0;\n";
    os << "  this.ymin = 0.0;\n";
    os << "  this.xmax = 100.0;\n";
    os << "  this.ymax = 100.0;\n";
    os << "\n";

    os << "  this.drawAnnotations();\n";
  }

  os << "}\n";

  //---

  os << "\n";

  if (plot) {
    CQChartsScriptPaintDevice device(const_cast<CQChartsPlot *>(plot), os);

    plot->writeScript(&device);
  }
  else {
    for (auto &plot : plots()) {
      CQChartsScriptPaintDevice device(const_cast<CQChartsPlot *>(plot), os);

      plot->writeScript(&device);
    }
  }

  //---

  os << "</script>\n";

  //---

  os << "</head>\n";

  os << "<body>\n";

  //---

  // canvas
  int cw = 1600;
  int ch = 1600;

  os << "<div style=\"position: absolute; top: 0px; left: 0px;\">\n";
  os << "<canvas id=\"canvas\" width=\"" << cw << "\" height=\"" << ch << "\">\n";
  os << "Your browser does not support HTML 5 Canvas.\n";
  os << "</canvas>\n";
  os << "</div>\n";

  //---

  // write custom html for annotations
  if (hasAnnotations()) {
    for (auto &annotation : annotations()) {
      annotation->writeHtml(&device);
    }
  }

  // write custom html for plots
  if (plot) {
    CQChartsScriptPaintDevice device(const_cast<CQChartsPlot *>(plot), os);

    plot->writeHtml(&device);
  }
  else {
    for (auto &plot : plots()) {
      CQChartsScriptPaintDevice device(const_cast<CQChartsPlot *>(plot), os);

      plot->writeHtml(&device);
    }
  }

  //---

  // tooltip div
  os << "<div id=\"tooltip\" display=\"none\" style=\"position: absolute; display: none;\">";
  os << "</div>\n";
  os << "\n";

  //---

  // log text
  os << "<p style=\"position: absolute; top: " << ch << "px; left: 0px;\""
        " id=\"log_message\"></p>\n";

  //---

  os << "</body>\n";
  os << "</html>\n";

  return true;
}

//------

void
CQChartsView::
showBoxesSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->setShowBoxes(b);
}

void
CQChartsView::
bufferLayersSlot(bool b)
{
  setBufferLayers(b);
}

//------

void
CQChartsView::
xRangeMapSlot(bool b)
{
  if (window())
    window()->setXRangeMap(b);
}

void
CQChartsView::
yRangeMapSlot(bool b)
{
  if (window())
    window()->setYRangeMap(b);
}

bool
CQChartsView::
isShowTable() const
{
  return (window() ? window()->isDataTable() : false);
}

void
CQChartsView::
setShowTable(bool b)
{
  if (window())
    window()->setDataTable(b);
}

bool
CQChartsView::
isShowSettings() const
{
  return (window() ? window()->isViewSettings() : false);
}

void
CQChartsView::
setShowSettings(bool b)
{
  if (window())
    window()->setViewSettings(b);
}

//------

void
CQChartsView::
resizeToView()
{
  bool b = true;

  std::swap(sizeData_.autoSize, b);

  resizeEvent(nullptr);

  sizeData_.width   = width ();
  sizeData_.height  = height();
  sizeData_.sizeSet = true;

  std::swap(sizeData_.autoSize, b);
}

//------

void
CQChartsView::
currentPlotSlot()
{
  auto *action = qobject_cast<QAction *>(sender());

  bool ok;
  int plotInd = (int) CQChartsVariant::toInt(action->data(), ok);
  assert(ok);

  setCurrentPlotInd(plotInd);
}

//------

void
CQChartsView::
updateAll()
{
  updatePlots();

  invalidateObjects();
  invalidateOverlay();

  update();
}

void
CQChartsView::
updatePlots()
{
  for (auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    plot->drawObjs();
  }

  update();
}

//------

CQChartsPlot *
CQChartsView::
getPlotForId(const QString &id) const
{
  for (const auto &plot : plots()) {
    if (plot->id() == id)
      return plot;
  }

  return nullptr;
}

void
CQChartsView::
getDrawPlots(Plots &plots) const
{
  for (const auto &plot : this->plots()) {
    if      (plot->isOverlay()) {
      if (! plot->isFirstPlot())
        continue;

      if      (plot->isX1X2()) {
        CQChartsPlot *plot1, *plot2;

        plot->x1x2Plots(plot1, plot2);

        plots.push_back(plot1);
        plots.push_back(plot2);
      }
      else if (plot->isY1Y2()) {
        CQChartsPlot *plot1, *plot2;

        plot->y1y2Plots(plot1, plot2);

        plots.push_back(plot1);
        plots.push_back(plot2);
      }
      else if (plot->isOverlay()) {
        Plots oplots;

        plot->overlayPlots(oplots);

        for (const auto &oplot : oplots)
          plots.push_back(oplot);
      }
    }
    else if (plot->parentPlot()) {
      continue;
    }
    else
      plots.push_back(plot);
  }
}

void
CQChartsView::
getPlots(Plots &plots) const
{
  for (const auto &plot : this->plots())
    plots.push_back(plot);
}

bool
CQChartsView::
addPlots(Plots &plots, bool clear) const
{
  if (clear)
    plots.clear();

  for (const auto &plot : this->plots())
    plots.push_back(plot);

  return ! plots.empty();
}

CQChartsPlot *
CQChartsView::
basePlot(CQChartsPlot *plot) const
{
  if      (plot->isOverlay())
    return plot->firstPlot();
  else if (plot->parentPlot())
    return plot->parentPlot();
  else
    return plot;
}

bool
CQChartsView::
addBasePlots(PlotSet &plots, bool clear) const
{
  if (clear)
    plots.clear();

  for (const auto &plot : this->plots()) {
    auto *plot1 = this->basePlot(plot);

    plots.insert(plot1);
  }

  return ! plots.empty();
}

bool
CQChartsView::
plotsAt(const Point &p, Plots &plots, CQChartsPlot* &plot, bool clear, bool first) const
{
  if (clear)
    plots.clear();

  PlotSet plotSet;

  plot = nullptr;

  auto *currentPlot = this->currentPlot(/*remap*/false);

  for (const auto &plot1 : this->plots()) {
    if (! plot1->isVisible())
      continue;

    if (plot1->parentPlot())
      continue;

    const auto &bbox = plot1->calcViewBBox();

    if (! bbox.inside(p))
      continue;

    auto *plot2 = plot1;

    if (first) {
      if (plot1->parentPlot())
        plot2 = plot1->parentPlot();

      if (plot1->isOverlay())
        plot2 = plot1->firstPlot();
    }

    if (plot1 == currentPlot)
      plot = plot2;

    if (plotSet.find(plot2) != plotSet.end())
      continue;

    plots.push_back(plot2);

    plotSet.insert(plot2);
  }

  if (! plot && ! plots.empty())
    plot = plots[0];

  return ! plots.empty();
}

CQChartsPlot *
CQChartsView::
plotAt(const Point &p) const
{
  Plots         plots;
  CQChartsPlot *plot;

  (void) plotsAt(p, plots, plot, true);

  return plot;
}

bool
CQChartsView::
plotsAt(const Point &p, Plots &plots, bool clear) const
{
  CQChartsPlot *plot;

  return plotsAt(p, plots, plot, clear);
}

bool
CQChartsView::
basePlotsAt(const Point &p, PlotSet &plots, bool clear) const
{
  if (clear)
    plots.clear();

  for (const auto &plot : this->plots()) {
    if (! plot->isVisible())
      continue;

    const auto &bbox = plot->calcViewBBox();

    if (! bbox.inside(p))
      continue;

    auto *plot1 = this->basePlot(plot);

    plots.insert(plot1);
  }

  return ! plots.empty();
}

CQChartsGeom::BBox
CQChartsView::
plotBBox(CQChartsPlot *plot) const
{
  for (const auto &plot1 : plots()) {
    if (plot1 != plot)
      continue;

    const auto &bbox = plot1->calcViewBBox();

    return bbox;
  }

  return BBox();
}

//---

CQChartsPlot *
CQChartsView::
currentPlot(bool remap) const
{
  if (plots().empty())
    return nullptr;

  int ind = currentPlotInd();

  auto *plot = getPlotForInd(ind);

  if (! plot)
    return nullptr;

  if (remap) {
    if (plot->isOverlay())
      plot = plot->firstPlot();
  }

  return plot;
}

CQChartsPlot *
CQChartsView::
getPlotForInd(int ind) const
{
  int np = plots().size();

  if (ind < 0 || ind >= np)
    return nullptr;

  auto *plot = plots_[ind];

  return plot;
}

int
CQChartsView::
getIndForPlot(const CQChartsPlot *plot) const
{
  if (! plot)
    return -1;

  int np = plots().size();

  for (int ind = 0; ind < np; ++ind) {
    if (plots_[ind] == plot)
      return ind;
  }

  return -1;
}

int
CQChartsView::
calcCurrentPlotInd(bool remap) const
{
  if (plots().empty())
    return -1;

  CQChartsPlot *plot = nullptr;

  for (auto &plot1 : plots()) {
    if (plot1->isVisible()) {
      plot = plot1;
      break;
    }
  }

  if (! plot)
    return -1;

  if (remap) {
    if (plot->isOverlay())
      plot = plot->firstPlot();
  }

  return getIndForPlot(plot);
}

//---

void
CQChartsView::
setStatusText(const QString &text)
{
  emit statusTextChanged(text);
}

void
CQChartsView::
setPosText(const QString &text)
{
  emit posTextChanged(text);
}

void
CQChartsView::
setSelText(const QString &text)
{
  emit selTextChanged(text);
}

//------

void
CQChartsView::
scrollLeft()
{
  if (scrollData_.page > 0) {
    --scrollData_.page;

    updateScroll();
  }
}

void
CQChartsView::
scrollRight()
{
  if (scrollData_.page < scrollNumPages() - 1) {
    ++scrollData_.page;

    updateScroll();
  }
}

void
CQChartsView::
updateScroll()
{
  double vr = viewportRange();

  if (isScrolled()) {
    double dx = scrollData_.page*scrollData_.delta;

    displayRange_->setWindowRange(dx, 0, dx + vr, vr);
  }
  else
    displayRange_->setWindowRange(0, 0, vr, vr);

  updatePlots();

  emit scrollDataChanged();
}

//------

void
CQChartsView::
writeAll(std::ostream &os) const
{
  this->write(os);

  //---

  const auto &viewAnnotations = this->annotations();

  int annotationId = 1;

  for (const auto &annotation : viewAnnotations) {
    os << "\n";

    annotation->write(os, "view", QString("annotation%1").arg(annotationId));

    ++annotationId;
  }

  //---

  Plots plots;

  this->getPlots(plots);

  using ModelVars = std::map<QString, QString>;
  using PlotVars  = std::map<CQChartsPlot*, QString>;

  ModelVars modelVars;
  PlotVars  plotVars;

  for (const auto &plot : plots) {
    auto *modelData = plot->getModelData();

    QString modelVarName;

    if (modelData) {
      QString id = modelData->id();

      auto p = modelVars.find(id);

      if (p == modelVars.end()) {
        os << "\n";

        QString modelVarName = QString("model%1").arg(modelVars.size() + 1);

        p = modelVars.insert(p, ModelVars::value_type(id, modelVarName));

        modelData->write(os, modelVarName);
      }

      modelVarName = (*p).second;
    }

    os << "\n";

    QString plotVarName = QString("plot%1").arg(plotVars.size() + 1);

    plotVars[plot] = plotVarName;

    plot->write(os, plotVarName, modelVarName, "view");

    //---

    const auto &plotAnnotations = plot->annotations();

    for (const auto &annotation : plotAnnotations) {
      os << "\n";

      annotation->write(os, plotVarName, QString("annotation%1").arg(annotationId));

      ++annotationId;
    }
  }

  CQChartsView::PlotSet basePlots;

  addBasePlots(basePlots);

  for (const auto &plot : basePlots) {
    if      (plot->isX1X2()) {
      CQChartsPlot *plot1, *plot2;

      plot->x1x2Plots(plot1, plot2);

      os << "\n";
      os << "group_charts_plots -x1x2";

      if (plot->isOverlay())
        os << " -overlay";

      os << " $" << plotVars[plot1].toStdString();
      os << " $" << plotVars[plot2].toStdString();
      os << "\n";
    }
    else if (plot->isY1Y2()) {
      CQChartsPlot *plot1, *plot2;

      plot->y1y2Plots(plot1, plot2);

      os << "\n";
      os << "group_charts_plots -y1y2";

      if (plot->isOverlay())
        os << " -overlay";

      os << " $" << plotVars[plot1].toStdString();
      os << " $" << plotVars[plot2].toStdString();
      os << "\n";
    }
    else if (plot->isOverlay()) {
      Plots oplots;

      plot->overlayPlots(oplots);

      os << "\n";
      os << "group_charts_plots -overlay ";

      for (const auto &oplot : oplots)
        os << " $" << plotVars[oplot].toStdString();

      os << "\n";
    }
  }
}

void
CQChartsView::
write(std::ostream &os) const
{
  os << "set view [create_charts_view]\n";

  CQPropertyViewModel::NameValues nameValues;

  propertyModel()->getChangedNameValues(this, nameValues, /*tcl*/true);

  if (nameValues.size())
    os << "\n";

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str = "";

    os << "set_charts_property -view $view -name " << nv.first.toStdString() <<
          " -value {" << str.toStdString() << "}\n";
  }
}

//------

CQChartsGeom::Point
CQChartsView::
positionToView(const CQChartsPosition &pos) const
{
  auto p  = pos.p();
  auto p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = p;
  else if (pos.units() == CQChartsUnits::PERCENT) {
    p1.setX(p.getX()*viewportRange()/100.0);
    p1.setY(p.getY()*viewportRange()/100.0);
  }
  else if (pos.units() == CQChartsUnits::EM) {
    double x = pixelToWindowWidth (p.getX()*fontEm());
    double y = pixelToWindowHeight(p.getY()*fontEm());

    return Point(x, y);
  }
  else if (pos.units() == CQChartsUnits::EX) {
    double x = pixelToWindowWidth (p.getX()*fontEx());
    double y = pixelToWindowHeight(p.getY()*fontEx());

    return Point(x, y);
  }

  return p1;
}

CQChartsGeom::Point
CQChartsView::
positionToPixel(const CQChartsPosition &pos) const
{
  auto p  = pos.p();
  auto p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = p;
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = windowToPixel(p);
  else if (pos.units() == CQChartsUnits::PERCENT) {
    p1.setX(p.getX()*width ()/100.0);
    p1.setY(p.getY()*height()/100.0);
  }
  else if (pos.units() == CQChartsUnits::EM) {
    double x = p.getX()*fontEm();
    double y = p.getY()*fontEm();

    return Point(x, y);
  }
  else if (pos.units() == CQChartsUnits::EX) {
    double x = p.getX()*fontEx();
    double y = p.getY()*fontEx();

    return Point(x, y);
  }

  return p1;
}

//------

CQChartsGeom::BBox
CQChartsView::
rectToView(const CQChartsRect &rect) const
{
  auto r  = rect.bbox();
  auto r1 = r;

  if      (rect.units() == CQChartsUnits::PIXEL)
    r1 = pixelToWindow(r);
  else if (rect.units() == CQChartsUnits::VIEW)
    r1 = r;
  else if (rect.units() == CQChartsUnits::PERCENT) {
    r1.setXMin(r.getXMin()*viewportRange()/100.0);
    r1.setYMin(r.getYMin()*viewportRange()/100.0);
    r1.setXMax(r.getXMax()*viewportRange()/100.0);
    r1.setYMax(r.getYMax()*viewportRange()/100.0);
  }
  else if (rect.units() == CQChartsUnits::EM) {
    double x1 = pixelToWindowWidth (r.getXMin()*fontEm());
    double y1 = pixelToWindowHeight(r.getYMin()*fontEm());
    double x2 = pixelToWindowWidth (r.getXMax()*fontEm());
    double y2 = pixelToWindowHeight(r.getYMax()*fontEm());

    return BBox(x1, y1, x2, y2);
  }
  else if (rect.units() == CQChartsUnits::EX) {
    double x1 = pixelToWindowWidth (r.getXMin()*fontEx());
    double y1 = pixelToWindowHeight(r.getYMin()*fontEx());
    double x2 = pixelToWindowWidth (r.getXMax()*fontEx());
    double y2 = pixelToWindowHeight(r.getYMax()*fontEx());

    return BBox(x1, y1, x2, y2);
  }

  return r1;
}

CQChartsGeom::BBox
CQChartsView::
rectToPixel(const CQChartsRect &rect) const
{
  auto r  = rect.bbox();
  auto r1 = r;

  if      (rect.units() == CQChartsUnits::PIXEL)
    r1 = r;
  else if (rect.units() == CQChartsUnits::VIEW)
    r1 = windowToPixel(r);
  else if (rect.units() == CQChartsUnits::PERCENT) {
    r1.setXMin(r.getXMin()*width ()/100.0);
    r1.setYMin(r.getYMin()*height()/100.0);
    r1.setXMax(r.getXMax()*width ()/100.0);
    r1.setYMax(r.getYMax()*height()/100.0);
  }
  else if (rect.units() == CQChartsUnits::EM) {
    double x1 = r.getXMin()*fontEm();
    double y1 = r.getYMin()*fontEm();
    double x2 = r.getXMax()*fontEm();
    double y2 = r.getYMax()*fontEm();

    return BBox(x1, y1, x2, y2);
  }
  else if (rect.units() == CQChartsUnits::EX) {
    double x1 = r.getXMin()*fontEx();
    double y1 = r.getYMin()*fontEx();
    double x2 = r.getXMax()*fontEx();
    double y2 = r.getYMax()*fontEx();

    return BBox(x1, y1, x2, y2);
  }

  return r1;
}

//------

double
CQChartsView::
lengthViewWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return pixelToWindowWidth(len.value());
  else if (len.units() == CQChartsUnits::VIEW)
    return len.value();
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*viewportRange()/100.0;
  else if (len.units() == CQChartsUnits::EM)
    return pixelToWindowWidth(len.value()*fontEm());
  else if (len.units() == CQChartsUnits::EX)
    return pixelToWindowWidth(len.value()*fontEx());

  return len.value();
}

double
CQChartsView::
lengthViewHeight(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return pixelToWindowHeight(len.value());
  else if (len.units() == CQChartsUnits::VIEW)
    return len.value();
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*viewportRange()/100.0;
  else if (len.units() == CQChartsUnits::EM)
    return pixelToWindowHeight(len.value()*fontEm());
  else if (len.units() == CQChartsUnits::EX)
    return pixelToWindowHeight(len.value()*fontEx());

  return len.value();
}

double
CQChartsView::
lengthPixelWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return len.value();
  else if (len.units() == CQChartsUnits::VIEW)
    return windowToPixelWidth(len.value());
  else if (len.units() == CQChartsUnits::PERCENT) {
    int w = (isAutoSize() ? width() : sizeData_.width);

    return len.value()*w/100.0;
  }
  else if (len.units() == CQChartsUnits::EM)
    return len.value()*fontEm();
  else if (len.units() == CQChartsUnits::EX)
    return len.value()*fontEx();

  return len.value();
}

double
CQChartsView::
lengthPixelHeight(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return len.value();
  else if (len.units() == CQChartsUnits::VIEW)
    return windowToPixelHeight(len.value());
  else if (len.units() == CQChartsUnits::PERCENT) {
    int h = (isAutoSize() ? height() : sizeData_.height);

    return len.value()*h/100.0;
  }
  else if (len.units() == CQChartsUnits::EM)
    return len.value()*fontEm();
  else if (len.units() == CQChartsUnits::EX)
    return len.value()*fontEx();

  return len.value();
}

//------

void
CQChartsView::
windowToPixelI(double wx, double wy, double &px, double &py) const
{
  displayRange_->windowToPixel(wx, wy, &px, &py);
}

void
CQChartsView::
pixelToWindowI(double px, double py, double &wx, double &wy) const
{
  displayRange_->pixelToWindow(px, py, &wx, &wy);
}

CQChartsGeom::Point
CQChartsView::
windowToPixel(const Point &w) const
{
  Point p;

  windowToPixelI(w.x, w.y, p.x, p.y);

  return p;
}

CQChartsGeom::Point
CQChartsView::
pixelToWindow(const Point &p) const
{
  Point w;

  pixelToWindowI(p.x, p.y, w.x, w.y);

  return w;
}

CQChartsGeom::BBox
CQChartsView::
windowToPixel(const BBox &wrect) const
{
  double px1, py1, px2, py2;

  windowToPixelI(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixelI(wrect.getXMax(), wrect.getYMax(), px2, py1);

  return BBox(px1, py1, px2, py2);
}

CQChartsGeom::BBox
CQChartsView::
pixelToWindow(const BBox &prect) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindowI(prect.getXMin(), prect.getYMin(), wx1, wy2);
  pixelToWindowI(prect.getXMax(), prect.getYMax(), wx2, wy1);

  return BBox(wx1, wy1, wx2, wy2);
}

double
CQChartsView::
pixelToSignedWindowWidth(double ww) const
{
  return CMathUtil::sign(ww)*pixelToWindowWidth(ww);
}

double
CQChartsView::
pixelToSignedWindowHeight(double wh) const
{
  return -CMathUtil::sign(wh)*pixelToWindowHeight(wh);
}

double
CQChartsView::
pixelToWindowWidth(double pw) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindowI( 0, 0, wx1, wy1);
  pixelToWindowI(pw, 0, wx2, wy2);

  return std::abs(wx2 - wx1);
}

double
CQChartsView::
pixelToWindowHeight(double ph) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindowI(0, 0 , wx1, wy1);
  pixelToWindowI(0, ph, wx2, wy2);

  return std::abs(wy2 - wy1);
}

double
CQChartsView::
windowToSignedPixelWidth(double ww) const
{
  return CMathUtil::sign(ww)*windowToPixelWidth(ww);
}

double
CQChartsView::
windowToSignedPixelHeight(double wh) const
{
  return -CMathUtil::sign(wh)*windowToPixelHeight(wh);
}

double
CQChartsView::
windowToPixelWidth(double ww) const
{
  double px1, py1, px2, py2;

  windowToPixelI( 0, 0, px1, py1);
  windowToPixelI(ww, 0, px2, py2);

  return std::abs(px2 - px1);
}

double
CQChartsView::
windowToPixelHeight(double wh) const
{
  double px1, py1, px2, py2;

  windowToPixelI(0, 0 , px1, py1);
  windowToPixelI(0, wh, px2, py2);

  return std::abs(py2 - py1);
}

CQChartsGeom::Size
CQChartsView::
pixelToWindowSize(const Size &ps) const
{
  double w = pixelToWindowWidth (ps.width ());
  double h = pixelToWindowHeight(ps.height());

  return Size(w, h);
}

CQChartsGeom::Polygon
CQChartsView::
windowToPixel(const Polygon &poly) const
{
  Polygon ppoly;

  int np = poly.size();

  for (int i = 0; i < np; ++i)
    ppoly.addPoint(windowToPixel(poly.point(i)));

  return ppoly;
}

QPainterPath
CQChartsView::
windowToPixel(const QPainterPath &path) const
{
  QPainterPath ppath;

  int n = path.elementCount();

  for (int i = 0; i < n; ++i) {
    const auto &e = path.elementAt(i);

    if      (e.isMoveTo()) {
      auto p1 = windowToPixel(Point(e.x, e.y));

      ppath.moveTo(p1.qpoint());
    }
    else if (e.isLineTo()) {
      auto p1 = windowToPixel(Point(e.x, e.y));

      ppath.lineTo(p1.qpoint());
    }
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      auto p1 = windowToPixel(Point(e.x, e.y));

      if (i < n - 1) {
        e1  = path.elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path.elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        auto p2 = windowToPixel(Point(e1.x, e1.y)); ++i;

        if (e2t == QPainterPath::CurveToDataElement) {
          auto p3 = windowToPixel(Point(e2.x, e2.y)); ++i;

          ppath.cubicTo(p1.qpoint(), p2.qpoint(), p3.qpoint());
        }
        else {
          ppath.quadTo(p1.qpoint(), p2.qpoint());
        }
      }
    }
    else {
      assert(false);
    }
  }

  return ppath;
}

//------

QSize
CQChartsView::
sizeHint() const
{
  return viewSizeHint();
}
