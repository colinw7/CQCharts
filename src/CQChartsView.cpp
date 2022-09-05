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
#include <CQChartsViewGLWidget.h>
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
#include <CQChartsStatsPaintDevice.h>

#include <CQChartsDocument.h>
#include <CQChartsRegionMgr.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColors.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>
#include <CQBaseModel.h>

#include <CQPerfMonitor.h>
#include <CQGLControl.h>
#include <CMathRound.h>

#include <QSvgGenerator>
#include <QFileDialog>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMenu>

#include <fstream>

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
  init();
}

CQChartsView::
~CQChartsView()
{
  term();
}

//---

void
CQChartsView::
init()
{
  setObjectName("view");

  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setBackgroundFillColor(Color::makeInterfaceValue(0.0));

  //---

  bufferLayers_ = CQChartsEnv::getBool("CQ_CHARTS_BUFFER_LAYERS", bufferLayers_);

  bgBuffer_      = std::make_unique<CQChartsBuffer>(this);
  fgBuffer_      = std::make_unique<CQChartsBuffer>(this);
  overlayBuffer_ = std::make_unique<CQChartsBuffer>(this);

  //---

  displayRange_ = std::make_unique<CQChartsDisplayRange>();

  double vr = viewportRange();

  displayRange_->setWindowRange(0, 0, vr, vr);

  //---

  propertyModel_ = std::make_unique<CQPropertyViewModel>();

  connect(propertyModel_.get(), SIGNAL(valueChanged(QObject *, const QString &)),
          this, SLOT(propertyItemChanged(QObject *, const QString &)));

  //---

  if (charts_->hasViewKey()) {
    keyObj_ = std::make_unique<ViewKey>(this);

    keyObj_->setVisible(false);
  }

  //---

  setSelectedMode(HighlightDataMode::FILL);

  setSelectedFillColor  (Color::makeContrast());
  setSelectedFillAlpha  (Alpha(0.3));
  setSelectedStrokeColor(Color::makeContrast());
  setSelectedStrokeWidth(Length::pixel(2));

  setInsideFillColor  (Color::makeContrast());
  setInsideFillAlpha  (Alpha(0.3));
  setInsideStrokeColor(Color::makeContrast());
  setInsideStrokeWidth(Length::pixel(2));

  //---

  lightPaletteSlot();

  themeSlot("default");

  //---

  font_.setFont(QFont());

  tipData_.font.decFontSize(4);

  //---

  addProperties();

  //---

  tipData_.toolTip  = new CQChartsViewToolTip (this);
  tipData_.floatTip = new CQChartsViewFloatTip(this);

  if (! isFloatTip())
    CQToolTip::setToolTip(this, tipData_.toolTip);

  {
  auto tipFont = this->tipFont().calcFont(font_.font());

  tipData_.toolTip ->setFont(tipFont);
  tipData_.floatTip->setFont(tipFont);
  }

  //---

  searchData_.timeout = CQChartsEnv::getInt("CQ_CHARTS_SEARCH_TIMEOUT", searchData_.timeout);

  setSearchTimeout(searchData_.timeout);

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

  //---

  regionMgr_ = std::make_unique<CQChartsRegionMgr>(this);

  //---

  registerSlot("set_mode"     , QStringList() << "string");
  registerSlot("fit"          , QStringList());
  registerSlot("zoom_data"    , QStringList());
  registerSlot("zoom_full"    , QStringList());
  registerSlot("show_table"   , QStringList() << "bool");
  registerSlot("show_settings", QStringList() << "bool");
}

void
CQChartsView::
term()
{
  if (charts_)
    charts_->removeView(this);

  //---

  {
  lockPainter(true);

  delete ipainter_;

  lockPainter(false);
  }

  //---

  delete image_;

  for (auto &plot : plots_)
    delete plot;

  for (auto &annotation : annotations())
    delete annotation;

  for (auto &probeBand : probeData_.bands)
    delete probeBand;

  delete popupMenu_;

  if (! isFloatTip())
    CQToolTip::unsetToolTip(this);

  delete tipData_.toolTip;
  delete tipData_.floatTip;
}

//---

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

bool
CQChartsView::
isExpandModelIndex(const QModelIndex &ind) const
{
  return window()->isExpandModelIndex(ind);
}

void
CQChartsView::
expandModelIndex(const QModelIndex &ind, bool b)
{
  if (window())
    window()->expandModelIndex(ind, b);
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
  addProp("", "mode"          , "", "View mouse mode" , true);
  addProp("", "id"            , "", "View id"         , true);
  addProp("", "currentPlotInd", "", "Current plot ind", true);

  addProp("", "viewSizeHint", "", "View size hint", true);
  addProp("", "bufferLayers", "", "Buffer layer"  , true);

  addProp("", "showTable"   , "", "Show table of value", true);
  addProp("", "showSettings", "", "Show settings panel", true);

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
  addStyleProp("text", "fontFactor", "factor", "Global text scale factor")->setMinValue(0.001);
  addStyleProp("text", "font"      , "font"  , "Global text font");

  // tip
  addStyleProp("tip", "floatTip", "float", "Use floating tip");
  addStyleProp("tip", "tipFont" , "font" , "Tip font");
  addStyleProp("tip", "tipDelay", "delay", "Tip hide delay");
  addStyleProp("tip", "tipAlign", "align", "Tip align");

  // plot separators
  addStyleProp("options", "plotSeparators", "", "Show plot separators");

  // handdrawn
  addStyleProp("handdrawn", "handDrawn"    , "enabled"  , "Enable handdraw painter");
  addStyleProp("handdrawn", "handRoughness", "roughness", "Handdraw roughness");
  addStyleProp("handdrawn", "handFillDelta", "fillDelta", "Handdraw fill delta");

  // overview
  addStyleProp("overview", "overviewXSize"     , "xSize"     , "Overview X Size");
  addStyleProp("overview", "overviewYSize"     , "ySize"     , "Overview Y Size");
  addStyleProp("overview", "overviewRangeColor", "rangeColor", "Overview Range Color");
  addStyleProp("overview", "overviewRangeAlpha", "rangeAlpha", "Overview Range ALpha");

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
  auto addSelectProp = [&](const QString &path, const QString &prop, const QString &name,
                           const QString &tip) {
    addStyleProp("select/" + path, "selected" + prop, name, "Highlight " + tip);
  };

  addSelectProp("highlight"       , "Mode"       , "mode"   , "draw mode");
  addSelectProp("highlight"       , "Blend"      , "blend"  ,
                "blend existing color with select color");
  addSelectProp("highlight"       , "ShapeData"  , "style"  , "shape data");
  addSelectProp("highlight/fill"  , "Filled"     , "visible", "fill visible");
  addSelectProp("highlight/fill"  , "FillColor"  , "color"  , "fill color");
  addSelectProp("highlight/fill"  , "FillAlpha"  , "alpha"  , "fill alpha");
  addSelectProp("highlight/fill"  , "FillPattern", "pattern", "fill pattern");
  addSelectProp("highlight/stroke", "Stroked"    , "visible", "stroke visible");
  addSelectProp("highlight/stroke", "StrokeColor", "color"  , "stroke color");
  addSelectProp("highlight/stroke", "StrokeAlpha", "alpha"  , "stroke color");
  addSelectProp("highlight/stroke", "StrokeWidth", "width"  , "stroke width");
  addSelectProp("highlight/stroke", "StrokeDash" , "dash"   , "stroke dash");

  addStyleProp("select", "selectedColor", "selectedColor",
               "Selected Color (if not stroked/filled)");
  addStyleProp("select", "selectedAlpha", "selectedAlpha",
               "Selected Alpha (if not stroked/filled)");

  // region mode
  addProp("region", "regionMode", "mode", "Region mode", true);

  // inside highlight
  auto addInsideProp = [&](const QString &path, const QString &prop, const QString &name,
                           const QString &tip) {
    addStyleProp("inside/" + path, "inside" + prop, name, "Inside " + tip);
  };

  addInsideProp("highlight"       , "Mode"       , "mode"   , "draw mode");
  addInsideProp("highlight"       , "Blend"      , "blend"  ,
                "blend existing color with inside color");
  addInsideProp("highlight"       , "ShapeData"  , "style"  , "shape data");
  addInsideProp("highlight/fill"  , "Filled"     , "visible", "fill visible");
  addInsideProp("highlight/fill"  , "FillColor"  , "color"  , "fill color");
  addInsideProp("highlight/fill"  , "FillAlpha"  , "alpha"  , "fill alpha");
  addInsideProp("highlight/fill"  , "FillPattern", "pattern", "fill pattern");
  addInsideProp("highlight/stroke", "Stroked"    , "visible", "stroke visible");
  addInsideProp("highlight/stroke", "StrokeColor", "color"  , "stroke color");
  addInsideProp("highlight/stroke", "StrokeAlpha", "alpha"  , "stroke alpha");
  addInsideProp("highlight/stroke", "StrokeWidth", "width"  , "stroke width");
  addInsideProp("highlight/stroke", "StrokeDash" , "dash"   , "stroke dash");

  addStyleProp("inside", "insideColor", "insideColor", "Inside Color");
  addStyleProp("inside", "insideAlpha", "insideAlpha", "Inside Alpha");

  // fade
  addStyleProp("fade", "overlayFade"     , "enabled", "Fade non-overlay");
  addStyleProp("fade", "overlayFadeAlpha", "alpha"  , "Fade alpha");

  // status
  addProp("status", "posTextType", "posTextType", "Position text type", true);

  // TODO: remove or make more general
  addProp("scroll", "scrolled"      , "enabled" , "Scrolling enabled"     , true);
  addProp("scroll", "scrollDelta"   , "delta"   , "Scroll delta"          , true);
  addProp("scroll", "scrollNumPages", "numPages", "Scroll number of pages", true);
  addProp("scroll", "scrollPage"    , "page"    , "Scroll current page"   , true);

  if (key())
    key()->addProperties(propertyModel(), "key");

  // probe
  addProp("probe", "probeObjects", "objects", "Probe nearest object");
  addProp("probe", "probeNamed"  , "named"  , "Show value name");
  addProp("probe", "probePos"    , "pos"    , "Probe value position");

  // view settings objects tab
  addProp("objects_tab", "viewSettingsMajorObjects", "major", "Show major objects");
  addProp("objects_tab", "viewSettingsMinorObjects", "minor", "Show minor objects");
  addProp("objects_tab", "viewSettingsMaxObjects"  , "max"  , "Max objects");
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
maximizePlot(Plot *plot)
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

      scrollData_.numPages = int(basePlots.size());
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

  Q_EMIT scrollDataChanged();
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
  auto qfont = this->font().font();
  QFontMetricsF fm(qfont);

  return fm.height();
}

double
CQChartsView::
fontEx() const
{
  auto qfont = this->font().font();
  QFontMetricsF fm(qfont);

  return fm.horizontalAdvance("x");
}

//---

void
CQChartsView::
setFloatTip(bool b)
{
  CQChartsUtil::testAndSet(tipData_.isFloat, b, [&]() {
    if (! isFloatTip()) {
      tipData_.floatTip->setEnabled(false);

      CQToolTip::setToolTip(this, tipData_.toolTip);
    }
    else {
      CQToolTip::unsetToolTip(this);

      tipData_.floatTip->setEnabled(true);
    }
  } );
}

void
CQChartsView::
setTipFont(const CQChartsFont &f)
{
  CQChartsUtil::testAndSet(tipData_.font, f, [&]() {
    auto tipFont = this->tipFont().calcFont(font_.font());

    tipData_.toolTip ->setFont(tipFont);
    tipData_.floatTip->setFont(tipFont);
  } );
}

void
CQChartsView::
setTipDelay(double delay)
{
  CQChartsUtil::testAndSet(tipData_.delay, delay, [&]() {
    tipData_.toolTip ->setHideSecs(tipData_.delay);
    tipData_.floatTip->setHideSecs(tipData_.delay);
  } );
}

void
CQChartsView::
setTipAlign(const Qt::Alignment &align)
{
  CQChartsUtil::testAndSet(tipData_.align, align, [&]() {
  //tipData_.toolTip ->setAlign(tipData_.align);
    tipData_.floatTip->setAlign(tipData_.align);
  } );
}

//---

void
CQChartsView::
setPlotSeparators(bool b)
{
  CQChartsUtil::testAndSet(plotSeparators_, b, [&]() {
    separatorsInvalid_ = true; resizeEvent(nullptr);
  } );
}

//---

void
CQChartsView::
setHandDrawn(bool b)
{
  CQChartsUtil::testAndSet(handDrawn_, b, [&]() {
    if (handDrawn_) {
      saveFont_ = font_;

      auto f = font_.font();

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
setOverviewXSize(int i)
{
  overviewData_.xsize = i;

  if (window())
    window()->updateOverview();
}

void
CQChartsView::
setOverviewYSize(int i)
{
  overviewData_.ysize = i;

  if (window())
    window()->updateOverview();
}

void
CQChartsView::
setOverviewRangeColor(const QColor &c)
{
  overviewData_.rangeColor = c;

  if (window())
    window()->updateOverview();
}

void
CQChartsView::
setOverviewRangeAlpha(double a)
{
  overviewData_.rangeAlpha = a;

  if (window())
    window()->updateOverview();
}

//---

void
CQChartsView::
setSearchTimeout(int i)
{
  searchData_.timeout = i;

  if (searchData_.timeout > 0) {
    if (! searchData_.timer) {
      searchData_.timer = new QTimer(this);

      searchData_.timer->setSingleShot(true);

      connect(searchData_.timer, SIGNAL(timeout()), this, SLOT(searchSlot()));
    }

    searchData_.timer->setInterval(searchData_.timeout);
  }
  else {
    delete searchData_.timer;

    searchData_.timer = nullptr;
  }
}

//---

void
CQChartsView::
setProbePos(const ProbePos &pos)
{
  CQChartsUtil::testAndSet(probeData_.pos, pos, [&]() {
    probeData_.pos = pos;

    updateProbes();
  } );
}

void
CQChartsView::
updateProbes()
{
  for (auto &probeBand : probeData_.bands)
    probeBand->setLabelPos(static_cast<CQChartsProbeBand::LabelPos>(probeData_.pos));
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
setPainterFont(PaintDevice *device, const CQChartsFont &font) const
{
  device->setFont(viewFont(font));
}

void
CQChartsView::
setPlotPainterFont(const Plot *plot, PaintDevice *device, const CQChartsFont &font) const
{
  device->setFont(plotFont(plot, font));
}

QFont
CQChartsView::
viewFont(const CQChartsFont &font) const
{
  // Calc specified font from view font
  auto font1 = font.calcFont(font_.font());

  if (isScaleFont())
    return scaledFont(font1, Size(this->size()));
  else
    return font1;
}

#if 0
QFont
CQChartsView::
viewFont(const QFont &font) const
{
  if (isScaleFont())
    return scaledFont(font, Size(this->size()));
  else
    return font;
}
#endif

QFont
CQChartsView::
plotFont(const Plot *plot, const CQChartsFont &font, bool scaled) const
{
  // adjust font by plot font and then by view font
  auto font1 = font .calcFont(plot->font());
  auto font2 = font1.calcFont(font_.font());

  if (scaled && isScaleFont())
    return scaledFont(font2, plot->calcPixelSize());
  else
    return font2;
}

#if 0
QFont
CQChartsView::
plotFont(const Plot *plot, const QFont &font, bool scaled) const
{
  if (scaled && isScaleFont())
    return scaledFont(font, plot->calcPixelSize());
  else
    return font;
}
#endif

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
set3D(bool b)
{
  is3D_ = b;

  if      (is3D_ && ! glWidget_) {
    glWidget_ = new CQChartsViewGLWidget(this);

    resizeEvent(nullptr);
  }
  else if (! is3D_ && glWidget_) {
    delete glWidget_;

    glWidget_ = nullptr;
  }
}

QWidget *
CQChartsView::
glWidget() const
{
  return glWidget_;
}

CQGLControl *
CQChartsView::
glControl() const
{
  return (glWidget_ ? glWidget_->control() : nullptr);
}

//---

void
CQChartsView::
setCurrentPlot(Plot *plot)
{
  if (plot) {
    assert(plot->isVisible());

    setCurrentPlotInd(getIndForPlot(plot));
  }
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

  if (currentPlot && ! currentPlot->type()->canRectSelect() && selectMode() == SelectMode::RECT)
    setSelectMode(SelectMode::POINT);

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

  Q_EMIT currentPlotChanged();
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

//---

void
CQChartsView::
rulerModeSlot()
{
  setMode(Mode::RULER);
}

void
CQChartsView::
clearRulerSlot()
{
  rulerData_.set = false;

  invalidateOverlay();

  updatePlots();
}

void
CQChartsView::
setRulerUnits(const Units &units)
{
  rulerData_.units = units;

  invalidateOverlay();

  updatePlots();
}

//---

void
CQChartsView::
setMode(const Mode &mode)
{
  Mode lastMode = mode_;

  CQChartsUtil::testAndSet(mode_, mode, [&]() {
    endRegionBand();

    for (auto &probeBand : probeData_.bands)
      probeBand->hide();

    if (mode == Mode::EDIT && lastMode == Mode::SELECT) {
      invalidateOverlay();

      updatePlots();
    }
    else
      deselectAll();

    clearRulerSlot();

    Q_EMIT modeChanged();
  } );
}

void
CQChartsView::
setKeyBehavior(const KeyBehavior &b)
{
  keyBehavior_ = b;

  for (auto &plot : plots()) {
    auto *key = plot->key();

    if (key)
      key->setPressBehavior(keyBehavior_);
  }
}

void
CQChartsView::
setSelectMode(const SelectMode &selectMode)
{
  CQChartsUtil::testAndSet(selectData_.mode, selectMode, [&]() { Q_EMIT selectModeChanged(); } );
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
    Q_EMIT selectionChanged();

    updateSelText();
  }
}

//---

void
CQChartsView::
setRegionMode(const RegionMode &regionMode)
{
  CQChartsUtil::testAndSet(regionData_.mode, regionMode, [&]() { Q_EMIT regionModeChanged(); } );
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

CQChartsAnnotation *
CQChartsView::
addAnnotation(CQChartsAnnotationType annotationType)
{
  auto type = static_cast<CQChartsAnnotation::Type>(annotationType);

  switch (type) {
    case Annotation::Type::GROUP:
      return addAnnotationT<AnnotationGroup>(this);
    case Annotation::Type::RECT:
      return addAnnotationT<CQChartsRectangleAnnotation>(this);
    case Annotation::Type::ELLIPSE:
      return addAnnotationT<CQChartsEllipseAnnotation>(this);
    case Annotation::Type::POLYGON:
      return addAnnotationT<CQChartsPolygonAnnotation>(this);
    case Annotation::Type::POLYLINE:
      return addAnnotationT<CQChartsPolylineAnnotation>(this);
    case Annotation::Type::TEXT:
      return addAnnotationT<CQChartsTextAnnotation>(this);
    case Annotation::Type::IMAGE:
      return addAnnotationT<CQChartsImageAnnotation>(this);
    case Annotation::Type::PATH:
      return addAnnotationT<CQChartsPathAnnotation>(this);
    case Annotation::Type::ARROW:
      return addAnnotationT<CQChartsArrowAnnotation>(this);
    case Annotation::Type::ARC:
      return addAnnotationT<CQChartsArcAnnotation>(this);
#if 0
    case Annotation::Type::ARC_CONNECTOR:
      return addAnnotationT<CQChartsArcConnectorAnnotation>(this);
#endif
    case Annotation::Type::POINT:
      return addAnnotationT<CQChartsPointAnnotation>(this);
    case Annotation::Type::PIE_SLICE:
      return addAnnotationT<CQChartsPieSliceAnnotation>(this);
#if 0
    case Annotation::Type::AXIS:
      return addAnnotationT<CQChartsAxisAnnotation>(this);
#endif
    case Annotation::Type::KEY:
      return addAnnotationT<CQChartsKeyAnnotation>(this);
    case Annotation::Type::POINT3D_SET:
      return addAnnotationT<CQChartsPoint3DSetAnnotation>(this);
    case Annotation::Type::POINT_SET:
      return addAnnotationT<CQChartsPointSetAnnotation>(this);
    case Annotation::Type::VALUE_SET:
      return addAnnotationT<CQChartsValueSetAnnotation>(this);
    case Annotation::Type::BUTTON:
      return addAnnotationT<CQChartsButtonAnnotation>(this);
    case Annotation::Type::WIDGET:
      return addAnnotationT<CQChartsWidgetAnnotation>(this);
#if 0
    case Annotation::Type::SYMBOL_MAP_KEY:
      return addAnnotationT<CQChartsSymbolSizeMapKeyAnnotation>(this);
#endif
    default:
      assert(false);
  }

  return nullptr;
}

CQChartsAnnotationGroup *
CQChartsView::
addAnnotationGroup()
{
  return addAnnotationT<AnnotationGroup>(new AnnotationGroup(this));
}

CQChartsArrowAnnotation *
CQChartsView::
addArrowAnnotation(const Position &start, const Position &end)
{
  return addAnnotationT<ArrowAnnotation>(new ArrowAnnotation(this, start, end));
}

CQChartsArcAnnotation *
CQChartsView::
addArcAnnotation(const Position &start, const Position &end)
{
  return addAnnotationT<ArcAnnotation>(new ArcAnnotation(this, start, end));
}

CQChartsEllipseAnnotation *
CQChartsView::
addEllipseAnnotation(const Position &center, const Length &xRadius, const Length &yRadius)
{
  return addAnnotationT<EllipseAnnotation>(new EllipseAnnotation(this, center, xRadius, yRadius));
}

CQChartsImageAnnotation *
CQChartsView::
addImageAnnotation(const Position &pos, const Image &image)
{
  return addAnnotationT<ImageAnnotation>(new ImageAnnotation(this, pos, image));
}

CQChartsImageAnnotation *
CQChartsView::
addImageAnnotation(const CQChartsRect &rect, const Image &image)
{
  return addAnnotationT<ImageAnnotation>(new ImageAnnotation(this, rect, image));
}

CQChartsPathAnnotation *
CQChartsView::
addPathAnnotation(const Path &path)
{
  return addAnnotationT<PathAnnotation>(new PathAnnotation(this, path));
}

CQChartsKeyAnnotation *
CQChartsView::
addKeyAnnotation()
{
  return addAnnotationT<KeyAnnotation>(new KeyAnnotation(this));
}

CQChartsPieSliceAnnotation *
CQChartsView::
addPieSliceAnnotation(const Position &pos, const Length &innerRadius,
                      const Length &outerRadius, const CQChartsAngle &startAngle,
                      const CQChartsAngle &spanAngle)
{
  return addAnnotationT<PieSliceAnnotation>(
    new PieSliceAnnotation(this, pos, innerRadius, outerRadius, startAngle, spanAngle));
}

CQChartsPointAnnotation *
CQChartsView::
addPointAnnotation(const Position &pos, const CQChartsSymbol &type)
{
  return addAnnotationT<PointAnnotation>(new PointAnnotation(this, pos, type));
}

CQChartsPointSetAnnotation *
CQChartsView::
addPointSetAnnotation(const Rect &rect, const CQChartsPoints &values)
{
  return addAnnotationT<PointSetAnnotation>(new PointSetAnnotation(this, rect, values));
}

CQChartsPolygonAnnotation *
CQChartsView::
addPolygonAnnotation(const CQChartsPolygon &points)
{
  return addAnnotationT<PolygonAnnotation>(new PolygonAnnotation(this, points));
}

CQChartsPolylineAnnotation *
CQChartsView::
addPolylineAnnotation(const CQChartsPolygon &points)
{
  return addAnnotationT<PolylineAnnotation>(new PolylineAnnotation(this, points));
}

CQChartsRectangleAnnotation *
CQChartsView::
addRectangleAnnotation(const CQChartsRect &rect)
{
  return addAnnotationT<RectangleAnnotation>(new RectangleAnnotation(this, rect));
}

CQChartsShapeAnnotation *
CQChartsView::
addShapeAnnotation(const CQChartsRect &rect)
{
  return addAnnotationT<ShapeAnnotation>(new ShapeAnnotation(this, rect));
}

CQChartsTextAnnotation *
CQChartsView::
addTextAnnotation(const Position &pos, const QString &text)
{
  return addAnnotationT<TextAnnotation>(new TextAnnotation(this, pos, text));
}

CQChartsTextAnnotation *
CQChartsView::
addTextAnnotation(const CQChartsRect &rect, const QString &text)
{
  return addAnnotationT<TextAnnotation>(new TextAnnotation(this, rect, text));
}

CQChartsValueSetAnnotation *
CQChartsView::
addValueSetAnnotation(const CQChartsRect &rectangle, const CQChartsReals &values)
{
  return addAnnotationT<ValueSetAnnotation>(new ValueSetAnnotation(this, rectangle, values));
}

CQChartsButtonAnnotation *
CQChartsView::
addButtonAnnotation(const Position &pos, const QString &text)
{
  return addAnnotationT<ButtonAnnotation>(new ButtonAnnotation(this, pos, text));
}

CQChartsWidgetAnnotation *
CQChartsView::
addWidgetAnnotation(const Position &pos, const CQChartsWidget &widget)
{
  return addAnnotationT<WidgetAnnotation>(new WidgetAnnotation(this, pos, widget));
}

CQChartsWidgetAnnotation *
CQChartsView::
addWidgetAnnotation(const CQChartsRect &rect, const CQChartsWidget &widget)
{
  return addAnnotationT<WidgetAnnotation>(new WidgetAnnotation(this, rect, widget));
}

//------

CQChartsView::Annotation *
CQChartsView::
addAnnotationI(Annotation *annotation)
{
  annotations_.push_back(annotation);

  connect(annotation, SIGNAL(idChanged()), this, SLOT(updateAnnotationSlot()));
  connect(annotation, SIGNAL(dataChanged()), this, SLOT(updateAnnotationSlot()));

  annotation->addProperties(propertyModel(), "annotations");

  //---

  updateView();

  //---

  Q_EMIT annotationsChanged();

  return annotation;
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
raiseAnnotation(Annotation *annotation)
{
  int pos = annotationPos(annotation);
  if (pos < 0) return; // not found

  int np = int(annotations().size());
  if (np < 2) return;

  if (pos < np - 1)
    std::swap(annotations_[size_t(pos + 1)], annotations_[size_t(pos)]);

  doUpdate();

  Q_EMIT annotationsReordered();
}

void
CQChartsView::
lowerAnnotation(Annotation *annotation)
{
  int pos = annotationPos(annotation);
  if (pos < 0) return; // not found

  int np = int(annotations().size());
  if (np < 2) return;

  if (pos > 0)
    std::swap(annotations_[size_t(pos - 1)], annotations_[size_t(pos)]);

  doUpdate();

  Q_EMIT annotationsReordered();
}

int
CQChartsView::
annotationPos(Annotation *annotation) const
{
  int np = int(annotations().size());

  for (int i = 0; i < np; ++i) {
    if (annotations_[size_t(i)] == annotation)
      return i;
  }

  return -1;
}

void
CQChartsView::
removeAnnotation(Annotation *annotation)
{
  int pos = 0;

  for (auto &annotation1 : annotations_) {
    if (annotation1 == annotation)
      break;

    ++pos;
  }

  int n = int(annotations_.size());

  assert(pos >= 0 && pos < n);

  delete annotation;

  for (int i = pos + 1; i < n; ++i)
    annotations_[size_t(i - 1)] = annotations_[size_t(i)];

  annotations_.pop_back();

  //---

  updateView();

  //---

  Q_EMIT annotationsChanged();
}

void
CQChartsView::
removeAllAnnotations()
{
  for (auto &annotation : annotations_)
    delete annotation;

  annotations_.clear();

  propertyModel()->removeProperties("annotations");

  //---

  updateView();

  //---

  Q_EMIT annotationsChanged();
}

void
CQChartsView::
updateAnnotationSlot()
{
  updateSlot();

  Q_EMIT annotationsChanged();
}

//---

void
CQChartsView::
addPlot(Plot *plot, const BBox &bbox)
{
  auto bbox1 = bbox;

  if (! bbox1.isSet())
    bbox1 = BBox(0, 0, viewportRange(), viewportRange());

  if (! plot->hasId()) {
    auto id = QString::number(numPlots() + 1);

    plot->setId(QString("plot:%1:%2").arg(plot->typeName()).arg(id));
    plot->setTipId(plot->id());

    plot->setObjectName(plot->id());
  }

  plot->setViewBBox(bbox1);

  plots_.push_back(plot);

  plot->addProperties();

  plot->postInit();

  connect(plot, SIGNAL(modelChanged()), this, SLOT(plotModelChanged()));

  connect(plot, SIGNAL(viewBoxChanged()), this, SLOT(plotViewBoxChanged()));

  connect(plot, SIGNAL(connectDataChanged()), this, SLOT(plotConnectDataChangedSlot()));

  connect(plot, SIGNAL(errorsCleared()), this, SIGNAL(updateErrors()));
  connect(plot, SIGNAL(errorAdded()), this, SIGNAL(updateErrors()));

  connect(plot, SIGNAL(currentPlotIdChanged(const QString &)),
          this, SIGNAL(currentPlotChanged()));

  //---

  if (currentPlotInd_ < 0) {
    if (plot->isVisible())
      setCurrentPlot(plot);
  }

  auto *currentPlot = this->currentPlot(/*remap*/false);

  if (currentPlot && currentPlot->parentPlot() && ! plot->parentPlot()) {
    if (plot->isVisible())
      setCurrentPlot(plot);
  }

  separatorsInvalid_ = true;

  //---

  updateView();

  //---

  Q_EMIT plotAdded(plot);
  Q_EMIT plotAdded(plot->id());
  Q_EMIT plotsChanged();
}

void
CQChartsView::
raisePlot(Plot *plot)
{
  int pos = plotPos(plot);
  if (pos < 0) return; // not found

  int np = int(plots().size());
  if (np < 2) return;

  if (pos < np - 1)
    std::swap(plots_[size_t(pos + 1)], plots_[size_t(pos)]);

  doUpdate();

  Q_EMIT plotsReordered();
}

void
CQChartsView::
lowerPlot(Plot *plot)
{
  int pos = plotPos(plot);
  if (pos < 0) return; // not found

  int np = int(plots().size());
  if (np < 2) return;

  if (pos > 0)
    std::swap(plots_[size_t(pos - 1)], plots_[size_t(pos)]);

  doUpdate();

  Q_EMIT plotsReordered();
}

int
CQChartsView::
plotPos(Plot *plot) const
{
  int np = int(plots().size());

  for (int i = 0; i < np; ++i) {
    if (plots_[size_t(i)] == plot)
      return i;
  }

  return -1;
}

void
CQChartsView::
removePlot(Plot *plot)
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
  auto id = plot->id();

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

  separatorsInvalid_ = true;

  //---

  updateView();

  //---

  Q_EMIT plotRemoved(id);
  Q_EMIT plotsChanged();
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

  updateView();

  //---

  Q_EMIT allPlotsRemoved();
  Q_EMIT plotsChanged();
}

//---

void
CQChartsView::
plotModelChanged()
{
  auto *plot = qobject_cast<Plot *>(sender());

  if (plot == currentPlot(/*remap*/false))
    Q_EMIT currentPlotChanged();
}

void
CQChartsView::
plotConnectDataChangedSlot()
{
  auto *plot = qobject_cast<Plot *>(sender());

  if (plot)
    Q_EMIT plotConnectDataChanged(plot->id());

  Q_EMIT connectDataChanged();
}

//---

void
CQChartsView::
plotViewBoxChanged()
{
  separatorsInvalid_ = true;
}

//---

void
CQChartsView::
resetGrouping()
{
  resetConnections(/*notify*/false);

  resetPlotGrouping();

  Q_EMIT connectDataChanged();
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
      plot->xAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
      plot->xAxis()->setVisible(true);
    }

    if (plot->yAxis()) {
      plot->yAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
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
    Q_EMIT connectDataChanged();
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

  Q_EMIT connectDataChanged();
}

void
CQChartsView::
initOverlayPlot(Plot *firstPlot)
{
  firstPlot->setOverlay(true, /*notify*/false);

  if (firstPlot->title() && title().length())
    firstPlot->title()->setTextStr(title());

  //---

  Plot::Plots plots;

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
    Plot::Plots plots;

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

      while (plot2) {
        plot2->xAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT));

        plot2 = plot2->nextPlot();
      }
    }
    else if (firstPlot->isY1Y2()) {
      auto *plot2 = firstPlot->nextPlot();

      while (plot2) {
        plot2->yAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT));

        plot2 = plot2->nextPlot();
      }
    }
  }
  else if (firstPlot->isOverlay(/*checkVisible*/false)) {
    Plot::Plots plots;

    firstPlot->overlayPlots(plots);

    for (auto &plot : plots) {
      auto *xaxis = plot->xAxis();
      auto *yaxis = plot->yAxis();

      xaxis->setVisible(true);
      yaxis->setVisible(true);

      if      (firstPlot->isX1X2(/*checkVisible*/false)) {
        plot->xAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
      }
      else if (firstPlot->isY1Y2(/*checkVisible*/false)) {
        plot->yAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
      }
    }
  }
}

void
CQChartsView::
initX1X2(const Plots &plots, bool overlay, bool reset)
{
  assert(plots.size() >= 2);

  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetPlotGrouping(plots);

    resetConnections(plots, /*notify*/false);
  }

  using PlotSet = std::set<Plot *>;

  PlotSet plotSet;

  for (auto &plot : plots) {
    assert(plotSet.find(plot) == plotSet.end() && ! plot->isOverlay());
    plotSet.insert(plot);
  }

  auto *plot1 = plots[0];

  if (plot1->title() && title().length())
    plot1->title()->setTextStr(title());

  for (std::size_t i = 0; i < plots.size(); ++i) {
    auto *plot = plots[i];

    plot->setX1X2   (true   , /*notify*/false);
    plot->setOverlay(overlay, /*notify*/false);

    if (i > 0) {
      auto *prevPlot = plots[i - 1];

      plot    ->setPrevPlot(prevPlot);
      prevPlot->setNextPlot(plot);
    }
  }

  for (std::size_t i = 0; i < plots.size(); ++i) {
    auto *plot = plots[i];

    if (i > 0) {
      // first plot x axis BOTTOM/LEFT (set by resetConnectData), second plot x axis TOP/RIGHT
      if (plot->xAxis()) {
        if (plot->isOverlay())
          plot->xAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT));
      }

      if (plot->isOverlay()) {
        if (plot->yAxis())
          plot->yAxis()->setVisible(false);
      }
    }
  }

  //---

  if (overlay)
    plot1->updateOverlay();

  plot1->updateObjs();

  plot1->applyDataRange();

  //---

  Q_EMIT connectDataChanged();
}

void
CQChartsView::
initY1Y2(const Plots &plots, bool overlay, bool reset)
{
  assert(plots.size() >= 2);

  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetPlotGrouping(plots);

    resetConnections(plots, /*notify*/false);
  }

  using PlotSet = std::set<Plot *>;

  PlotSet plotSet;

  for (auto &plot : plots) {
    assert(plotSet.find(plot) == plotSet.end() && ! plot->isOverlay());
    plotSet.insert(plot);
  }

  auto *plot1 = plots[0];

  if (plot1->title() && title().length())
    plot1->title()->setTextStr(title());

  for (std::size_t i = 0; i < plots.size(); ++i) {
    auto *plot = plots[i];

    plot->setY1Y2   (true   , /*notify*/false);
    plot->setOverlay(overlay, /*notify*/false);

    if (i > 0) {
      auto *prevPlot = plots[i - 1];

      plot    ->setPrevPlot(prevPlot);
      prevPlot->setNextPlot(plot);
    }
  }

  for (std::size_t i = 0; i < plots.size(); ++i) {
    auto *plot = plots[i];

    if (i > 0) {
      // first plot y axis BOTTOM/LEFT (set by resetConnectData), second plot y axis TOP/RIGHT
      if (plot->yAxis()) {
        if (plot->isOverlay())
          plot->yAxis()->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT));
      }

      if (plot->isOverlay()) {
        if (plot->xAxis())
          plot->xAxis()->setVisible(false);
      }
    }
  }

  //---

  if (overlay)
    plot1->updateOverlay();

  plot1->updateObjs();

  plot1->applyDataRange();

  //---

  Q_EMIT connectDataChanged();
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

  Q_EMIT connectDataChanged();
}

//------

void
CQChartsView::
autoPlacePlots()
{
  Plots visiblePlots;

  for (const auto &plot : plots()) {
    if (plot->isVisible())
      visiblePlots.push_back(plot);
  }

  int np = int(visiblePlots.size());

  int nc, nr;

  CQChartsUtil::countToSquareGrid(np, nc, nr);

  placePlots(visiblePlots, /*vertical*/false, /*horizontal*/false, nr, nc, /*reset*/true);
}

void
CQChartsView::
placePlots(const Plots &plots, bool vertical, bool horizontal, int rows, int columns, bool reset)
{
  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetConnections(plots, /*notify*/false);
  }

  //---

  // only place base plots (non-overlay)

  // get set of base plots
  PlotSet basePlotSet;

  for (const auto &plot : plots) {
    auto *plot1 = this->basePlot(plot);

    basePlotSet.insert(plot1);
  }

#if 0
  Plots basePlots;

  for (const auto &basePlot : basePlotSet)
    basePlots.push_back(basePlot);
#endif

  // add plots (only one per unique base plot)
  Plots plots1;

  for (const auto &plot : plots) {
    auto *plot1 = this->basePlot(plot);

    if (! plot1->isVisible())
      continue;

    if (basePlotSet.find(plot1) != basePlotSet.end()) {
      plots1.push_back(plot1);

      basePlotSet.erase(plot1);
    }
  }

  //---

  int np = int(plots1.size());

  if (np <= 0)
    return;

  //---

  auto setViewBBox = [&](Plot *plot, const BBox &bbox) {
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
    CQChartsUtil::countToSquareGrid(np, nc, nr);
  }

  double vr = CQChartsView::viewportRange();

  if (overlay) {
    for (int i = 0; i < np; ++i) {
      auto *plot = plots1[size_t(i)];

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

        auto *plot = plots1[size_t(i)];

        BBox bbox(x, y - dy, x + dx, y);

        setViewBBox(plot, bbox);

        x += dx;
      }

      y -= dy;
    }
  }

  if (reset)
    Q_EMIT connectDataChanged();
}

//------

QColor
CQChartsView::
interpPaletteColor(const ColorInd &ind, bool scale, bool invert) const
{
  auto c = Color::makePalette();

  c.setScale (scale );
  c.setInvert(invert);

  return interpColor(c, ind);

  //return charts()->interpPaletteColor(ind, scale, invert);
}

QColor
CQChartsView::
interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv, bool scale, bool invert) const
{
  return charts()->interpGroupPaletteColor(ig, iv, scale, invert);
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
  auto c = Color::makeInterfaceValue(r);

  return charts()->interpColor(c, ColorInd());
}

QColor
CQChartsView::
interpColor(const CQChartsColor &c, const ColorInd &ind) const
{
  if (defaultPalette_ != "") {
    auto c1 = charts()->adjustDefaultPalette(c, defaultPalette_);

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
  mouseData_.button     = int(me->button());
  mouseData_.pressed    = true;
  mouseData_.movePoint  = mouseData_.pressPoint;
  mouseData_.selMod     = CQChartsUtil::modifiersToSelMod(me->modifiers());
  mouseData_.clickMod   = CQChartsUtil::modifiersToClickMod(me->modifiers());

  auto w = pixelToWindow(mousePressPoint());

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if (mouseButton() == Qt::LeftButton) {
    if      (mode() == Mode::SELECT) {
      selectMousePress();

      updateTip(me->globalPos());
#if 0
      if (tipData_.floatTip->isVisible() && tipData_.floatTip->isLocked())
        tipData_.floatTip->showTip(me->globalPos());
#endif
    }
    else if (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT) {
      zoomMousePress();
    }
    else if (mode() == Mode::PAN) {
    }
    else if (mode() == Mode::PROBE) {
    }
    else if (mode() == Mode::QUERY) {
      queryMousePress();
    }
    else if (mode() == Mode::EDIT) {
      (void) editMousePress();
    }
    else if (mode() == Mode::REGION) {
      regionMousePress();
    }
    else if (mode() == Mode::RULER) {
      rulerMousePress();
    }
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
    else if (mode() == Mode::RULER) {
      rulerMouseMotion();
      return;
    }
  }

  //---

  if (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT) {
    updateMousePosText();
  }

  //---

  // probe mode and move (pressed or not pressed) - show probe lines
  if (mode() == Mode::PROBE) {
    updateMousePosText();

    showProbeLines(mouseMovePoint());

    return;
  }

  //---

  if (mode() == Mode::QUERY) {
    queryMouseMotion();
    return;
  }

  //---

  if (! mousePressed()) {
    if (mode() == Mode::EDIT) {
      updateMousePosText();

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

      searchData_.pos = mouseMovePoint();
    }
    // draw zoom rectangle
    else if (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT)
      zoomMouseMove();
    else if (mode() == Mode::PAN)
      panMouseMove();
    else if (mode() == Mode::EDIT)
      (void) editMouseMove();
    else if (mode() == Mode::REGION) {
      updateMousePosText();

      regionMouseMove();
    }
    else if (mode() == Mode::RULER) {
      updateMousePosText();

      rulerMouseMove();
    }
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
    else if (mode() == Mode::RULER) {
      if (mousePressed())
        rulerMouseRelease();
    }
  }
  else if (mouseButton() == Qt::MiddleButton) {
  }
  else if (mouseButton() == Qt::RightButton) {
  }
}

void
CQChartsView::
mouseDoubleClickEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

  if (me->button() != Qt::LeftButton)
    return;

  Point mp(me->pos());

  mouseData_.reset();

  mouseData_.pressPoint = adjustMousePos(mp);
  mouseData_.button     = int(me->button());
  mouseData_.pressed    = true;
  mouseData_.movePoint  = mouseData_.pressPoint;
  mouseData_.selMod     = CQChartsUtil::modifiersToSelMod(me->modifiers());
  mouseData_.clickMod   = CQChartsUtil::modifiersToClickMod(me->modifiers());

  auto w = pixelToWindow(mousePressPoint());

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  // TODO: other modes ?
  if (mode() == Mode::SELECT) {
    selectMouseDoubleClick();
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

  auto gpos = QCursor::pos();

  if      (ke->key() == Qt::Key_Escape) {
    mouseData_.escape = true;

    if (mousePressed())
      endRegionBand();

    if (mode() == Mode::ZOOM_IN || mode() == Mode::ZOOM_OUT || mode() == Mode::PAN) {
      if (! mousePressed())
        selectModeSlot();
    }
    else {
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
    if (ke->modifiers() & Qt::ControlModifier) {
      if (tipData_.floatTip->isVisible())
        tipData_.floatTip->setLocked(true);
    }
    else {
      if (mode() == Mode::EDIT)
        cycleEdit();
    }
  }
  else if (ke->key() == Qt::Key_Question) {
    if (tipData_.floatTip->isVisible())
      tipData_.floatTip->showQuery(gpos);
  }

  //---

  auto pos = mapFromGlobal(gpos);

  auto w = pixelToWindow(Point(pos));

  Plots plots;
  Plot* plot;

  plotsAt(w, plots, plot);

  if (plot)
    plot->keyPress(ke->key(), int(ke->modifiers()));

  //---

  Q_EMIT keyEventPress(ke->text());
}

//------

void
CQChartsView::
wheelEvent(QWheelEvent *e)
{
  if (isPreview())
    return;

  Point pp(e->pos());

  auto w = pixelToWindow(pp);

  Plots plots;
  Plot* plot;

  plotsAt(w, plots, plot);
  if (! plot) return;

  // scroll vertical
  if      (e->modifiers() & Qt::ShiftModifier) {
    plot->wheelVScroll(e->delta());
  }
  // scroll horizontal
  else if (e->modifiers() & Qt::ControlModifier) {
    plot->wheelHScroll(e->delta());
  }
  else {
    plot->wheelZoom(pp, e->delta());
  }

  doUpdate();
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

  CQChartsEditHandles::InsideData insideData;

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w) || annotation->editHandles()->inside(w, insideData))
      selAnnotations.push_back(annotation);
  }

  // start drag on already selected annotation handle
  if (! selAnnotations.empty() && selAnnotations[0]->isSelected()) {
    auto *annotation = selAnnotations[0];

    mouseData_.dragSide = CQChartsResizeSide::NONE;

    if (annotation->editHandles()->inside(w, insideData)) {
      mouseData_.dragObj  = DragObj::ANNOTATION;
      mouseData_.dragSide = insideData.resizeSide;

      annotation->editHandles()->setDragData(insideData);
      annotation->editHandles()->setDragPos (w);

      invalidateOverlay();

      return true;
    }
  }

  // start drag on already selected key handle
  if (key() && key()->isSelected()) {
    mouseData_.dragSide = CQChartsResizeSide::NONE;

    if (key()->editHandles()->inside(w, insideData)) {
      mouseData_.dragObj  = DragObj::KEY;
      mouseData_.dragSide = insideData.resizeSide;

      key()->editPress(w);

      key()->editHandles()->setDragData(insideData);
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

    doUpdate();

    return true;
  }

  //---

  // select/deselect key
  if (key() && key()->contains(w)) {
    if (! key()->isSelected()) {
      selectOneObj(key());

      doUpdate();

      return true;
    }

    if (key()->editPress(w)) {
      mouseData_.dragObj = DragObj::KEY;

      invalidateOverlay();

      return true;
    }

    return false;
  }

  //---

  bool rc = processMouseDataPlots([&](Plot *plot, const Point &p) {
    return plot->editMousePress(p, /*inside*/false);
  }, p);

  if (rc)
    return true;

  rc = processMouseDataPlots([&](Plot *plot, const Point &p) {
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

  processMouseDataPlots([&](Plot *plot, const Point &pos) {
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

      doUpdate();

      return;
    }
  }
  else {
    // update selected annotation inside
    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        if (annotation->editMotion(w)) {
          invalidateOverlay();

          doUpdate();

          return;
        }
      }
    }
  }

  //---

  // update plot mouse inside
  bool inside = false;

  Plots plots;
  Plot* plot;

  plotsAt(w, plots, plot);

  if (plot && plot->editMouseMotion(p))
    inside = true;

  for (auto &plot1 : plots) {
    if (plot1 == plot) continue;

    if (plot1->editMouseMotion(p))
      inside = true;
  }

  //---

  // nothing inside so do search (TODO: only editable)
  if (! inside)
    searchMouse();
}

void
CQChartsView::
editMouseRelease()
{
  mouseData_.dragObj = DragObj::NONE;

  processMouseDataPlots([&](Plot *plot, const Point &pos) {
    plot->editMouseRelease(pos); return false;
  }, mouseMovePoint());
}

//------

bool
CQChartsView::
calcGlobalTip(const QPoint &gpos, QString &tip)
{
  auto p = this->mapFromGlobal(gpos);

  auto wpos = this->pixelToWindow(Point(p));

  return calcTip(wpos, tip, /*single*/true);
}

bool
CQChartsView::
calcTip(const Point &wpos, QString &tip, bool single)
{
  searchAt(wpos);

  CQChartsView::Plots plots;

  this->plotsAt(wpos, plots);

  if (plots.empty())
    return false;

  auto ppos = windowToPixel(wpos);

  for (const auto &plot : plots) {
    if (! plot->isVisible())
      continue;

    auto w = plot->pixelToWindow(ppos);

    QString tip1;

    if (plot->plotTipText(w, tip1, single)) {
      if (tip.length())
        tip += "\n";

      tip += tip1;
    }
  }

  if (! tip.length()) {
    for (const auto &plot : plots) {
      if (! plot->isVisible())
        continue;

      auto *key = plot->key();
      if (! key) continue;

      auto w = plot->pixelToWindow(ppos);

      if (key->contains(w)) {
        QString tip1;

        if (key->tipText(w, tip1)) {
          if (tip.length())
            tip += "\n";

          tip += tip1;
        }
      }
    }
  }

  if (! tip.length())
    return false;

  return true;
}

//------

void
CQChartsView::
showProbeLines(const Point &p)
{
  auto addVerticalProbeBand = [&](int &ind, Plot *plot, const QString &tip,
                                  double px, double py1, double py2, double py3) {
    while (ind >= int(probeData_.bands.size())) {
      auto *probeBand = new CQChartsProbeBand(this);

      probeBand->setLabelPos(static_cast<CQChartsProbeBand::LabelPos>(probeData_.pos));

      probeData_.bands.push_back(probeBand);
    }

    probeData_.bands[size_t(ind)]->showVertical(plot, tip, px, py1, py2, py3);

    ++ind;
  };

  auto addHorizontalProbeBand = [&](int &ind, Plot *plot, const QString &tip,
                                    double px1, double px2, double px3, double py) {
    while (ind >= int(probeData_.bands.size())) {
      auto *probeBand = new CQChartsProbeBand(this);

      probeBand->setLabelPos(static_cast<CQChartsProbeBand::LabelPos>(probeData_.pos));

      probeData_.bands.push_back(probeBand);
    }

    probeData_.bands[size_t(ind)]->showHorizontal(plot, tip, px1, px2, px3, py);

    ++ind;
  };

  //---

  auto w = pixelToWindow(p);

  int probeInd = 0;

  Plots plots;
  Plot* plot;

  plotsAt(w, plots, plot);

  //---

  if (! isProbeObjects()) {
    if (plot) {
      auto dataRange = plot->calcDataRange();

      auto pp = plot->pixelToWindow(p);

      auto p1 = plot->windowToPixel(Point(dataRange.getXMin(), dataRange.getYMin()));
      auto p2 = plot->windowToPixel(Point(dataRange.getXMax(), dataRange.getYMax()));

      auto tip = QString("%1, %2").arg(plot->xStr(pp.x)).arg(plot->yStr(pp.y));

      addVerticalProbeBand  (probeInd, plot, "" , p.x, p1.y, p2.y, p2.y);
      addHorizontalProbeBand(probeInd, plot, "" , p1.x, p2.x, p2.x, p.y);
      addVerticalProbeBand  (probeInd, plot, tip, p.x, p.y, p.y + 4, p.y + 4);
    }
  }
  else {
    for (auto &plot : plots) {
      auto w = plot->pixelToWindow(p);

      //---

      Plot::ProbeData probeData;

      probeData.p = w;

      if (! plot->probe(probeData))
        continue;

      if (probeData.xvals.empty()) probeData.xvals.emplace_back(w.x, "", "");
      if (probeData.yvals.empty()) probeData.yvals.emplace_back(w.y, "", "");

      auto dataRange = plot->calcDataRange();

      if      (probeData.both) {
        // add probe lines from xmin to probed x values and from ymin to probed y values
        auto px1 = plot->windowToPixel(Point(dataRange.getXMin(), probeData.p.y));
        auto px2 = plot->windowToPixel(Point(dataRange.getXMax(), probeData.p.y));

        auto py1 = plot->windowToPixel(Point(probeData.p.x, dataRange.getYMin()));
        auto py2 = plot->windowToPixel(Point(probeData.p.x, dataRange.getYMax()));

        int nx = int(probeData.xvals.size());
        int ny = int(probeData.yvals.size());

        int n = std::min(nx, ny);

        for (int i = 0; i < n; ++i) {
          const auto &xval = probeData.xvals[size_t(i)];
          const auto &yval = probeData.yvals[size_t(i)];

          auto px3 = plot->windowToPixel(Point(xval.value, probeData.p.y));
          auto py3 = plot->windowToPixel(Point(probeData.p.x, yval.value));

          QString tip;

          if (isProbeNamed()) {
            tip = QString("%1: %2, %3: %4").
              arg(xval.label.length() ? xval.label : "X").
              arg(xval.valueStr.length() ? xval.valueStr : plot->xStr(xval.value)).
              arg(yval.label.length() ? yval.label : "Y").
              arg(yval.valueStr.length() ? yval.valueStr : plot->yStr(yval.value));
          }
          else {
            tip = QString("%1, %2").
              arg(xval.valueStr.length() ? xval.valueStr : plot->xStr(xval.value)).
              arg(yval.valueStr.length() ? yval.valueStr : plot->yStr(yval.value));
          }

          addVerticalProbeBand  (probeInd, plot, tip, py1.x, py1.y, py3.y, py2.y);
          addHorizontalProbeBand(probeInd, plot, "" , px1.x, px3.x, px2.x, px1.y);
        }
      }
      else if (probeData.direction == Qt::Vertical) {
        // add probe lines from ymin to probed y values
        auto p1 = plot->windowToPixel(Point(probeData.p.x, dataRange.getYMin()));
        auto p2 = plot->windowToPixel(Point(probeData.p.x, dataRange.getYMax()));

        for (const auto &yval : probeData.yvals) {
          auto p3 = plot->windowToPixel(Point(probeData.p.x, yval.value));

          QString tip;

          if (isProbeNamed())
            tip = QString("%1: %2").arg(yval.label.length() ? yval.label : "Y").
                    arg(yval.valueStr.length() ? yval.valueStr : plot->yStr(yval.value));
          else
            tip = (yval.valueStr.length() ? yval.valueStr : plot->yStr(yval.value));

          addVerticalProbeBand(probeInd, plot, tip, p1.x, p1.y, p3.y, p2.y);
        }
      }
      else {
        // add probe lines from xmin to probed x values
        auto p1 = plot->windowToPixel(Point(dataRange.getXMin(), probeData.p.y));
        auto p2 = plot->windowToPixel(Point(dataRange.getXMax(), probeData.p.y));

        for (const auto &xval : probeData.xvals) {
          auto p3 = plot->windowToPixel(Point(xval.value, probeData.p.y));

          QString tip;

          if (isProbeNamed())
            tip = QString("%1: %2").arg(xval.label.length() ? xval.label : "X").
                    arg(xval.valueStr.length() ? xval.valueStr : plot->xStr(xval.value));
          else
            tip = (xval.valueStr.length() ? xval.valueStr : plot->xStr(xval.value));

          addHorizontalProbeBand(probeInd, plot, tip, p1.x, p3.x, p2.x, p1.y);
        }
      }
    }
  }

  //---

  for (int i = probeInd; i < int(probeData_.bands.size()); ++i)
    probeData_.bands[size_t(i)]->hide();

  //---

  removeProbeOverlaps();
}

void
CQChartsView::
removeProbeOverlaps()
{
  using PosBands = std::map<double, ProbeBand *>;

  // get vertical and horizontal bands in value order (low to high)
  PosBands vertBands;
  PosBands horiBands;

  for (auto &probeBand : probeData_.bands) {
    if (! probeBand->isVisible())
      continue;

    auto value = probeBand->value();

    if (probeBand->orientation() == Qt::Vertical) {
      auto p = vertBands.find(value);

      while (p != vertBands.end()) {
        value += 0.001;

        p = vertBands.find(value);
      }

      vertBands[value] = probeBand;
    }
    else {
      auto p = horiBands.find(value);

      while (p != horiBands.end()) {
        value += 0.001;

        p = horiBands.find(value);
      }

      horiBands[value] = probeBand;
    }
  }

  //---

  int d = 4;

  {
  // remove vertical overlaps

  int  lastPos    { 0 };
  bool lastPosSet { false };
  int  delta      { 0 };

  for (const auto &pb : vertBands) {
    auto rect = pb.second->labelRect();

    if (lastPosSet) {
      if (rect.bottom() > lastPos) {
        delta = lastPos - rect.bottom();

        pb.second->moveLabel(0, delta);
      }
    }

    lastPos    = rect.top() - d;
    lastPosSet = true;
  }
  }

  //---

  {
  // remove horizontal overlaps

  int  lastPos    { 0 };
  bool lastPosSet { false };
  int  delta      { 0 };

  for (const auto &pb : horiBands) {
    auto rect = pb.second->labelRect();

    if (lastPosSet) {
      if (rect.left() < lastPos) {
        delta = lastPos - rect.left();

        pb.second->moveLabel(0, delta);
      }
    }

    lastPos    = rect.right() + d;
    lastPosSet = true;
  }
  }
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
    CQChartsSelectableIFace::SelData selData(mouseClickMod());

    bool handled = key()->selectPress(w, selData);

    if (handled) {
      Q_EMIT keyPressed  (key());
      Q_EMIT keyIdPressed(key()->id());

      return;
    }
  }

  //---

  // select view annotation
  annotationsAtPoint(w, pressAnnotations_);

#if 0
  for (const auto &annotation : pressAnnotations_) {
    CQChartsSelectableIFace::SelData selData(mouseSelMod());

    annotation->selectPress(w, selData);
  }
#endif

  for (const auto &selAnnotation : pressAnnotations_) {
    CQChartsSelectableIFace::SelData selData(mouseSelMod());

    if (! selAnnotation->selectPress(w, selData))
      continue;

    selectOneObj(selAnnotation);

    doUpdate();

    Q_EMIT annotationPressed  (selAnnotation);
    Q_EMIT annotationIdPressed(selAnnotation->id());

    return;
  }

  //---

  // select plot objects
  struct SelData {
    Point  pos;
    SelMod selMod;

    SelData(const Point &pos, SelMod selMod) :
     pos(pos), selMod(selMod) {
    }
  };

  SelData selData(mousePressPoint(), mouseSelMod());

  if (processMouseDataPlots([&](Plot *plot, const SelData &data) {
    if (plot->selectMousePress(data.pos, data.selMod)) {
      auto *selPlot = plot->selectionPlot();
      setCurrentPlot(selPlot);
      return true;
    }
    return false;
  }, selData)) {
    return;
  }

  //---

#if 0
  // select view key
  if (key() && key()->contains(w)) {
    CQChartsSelectableIFace::SelData selData;

    key()->selectPress(w, selData);
  }
#endif
}

void
CQChartsView::
selectMouseMotion()
{
  updateMousePosText();

  searchMouse();
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

    processMouseDataPlots([&](Plot *plot, const Point &pos) {
      bool current = (plot == mousePlot());

      return plot->selectMouseMove(pos, current);
    }, searchData_.pos);
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
      processMouseDataPlots([&](Plot *plot, const SelMod &selMod) {
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

    annotation->selectRelease(w);
  }
}

void
CQChartsView::
selectMouseDoubleClick()
{
  auto w = pixelToWindow(mousePressPoint());

  //---

  // key
  if (key() && key()->contains(w)) {
    if (key()->selectDoubleClick(w, mouseClickMod()))
      return;
  }

  //---

  // select view annotation
  annotationsAtPoint(w, pressAnnotations_);

  for (const auto &annotation : pressAnnotations_) {
    annotation->selectDoubleClick(w, mouseSelMod());
  }

  //---

  // select plot objects
  struct SelData {
    Point  pos;
    SelMod selMod;

    SelData(const Point &pos, SelMod selMod) :
     pos(pos), selMod(selMod) {
    }
  };

  SelData selData(mousePressPoint(), mouseSelMod());

  if (processMouseDataPlots([&](Plot *plot, const SelData &data) {
    if (plot->selectMouseDoubleClick(data.pos, data.selMod)) {
      auto *selPlot = plot->selectionPlot();
      setCurrentPlot(selPlot);
      return true;
    }
    return false;
  }, selData)) {
    return;
  }

  //---

  // select view key
  if (key() && key()->contains(w))
    key()->selectDoubleClick(w, SelMod::REPLACE);
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
  updateMousePosText();
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

    Q_EMIT regionPointRelease(p);
    Q_EMIT regionRectRelease (r);
  }
  else if (regionMode() == RegionMode::RECT) {
    endRegionBand();

    auto r = pixelToWindow(probeData_.regionBand.bbox());

    Q_EMIT regionPointRelease(r.getCenter());
    Q_EMIT regionRectRelease (r);
  }
}

//------

void
CQChartsView::
rulerMousePress()
{
  auto w = pixelToWindow(mousePressPoint());

  rulerData_.start = w;
  rulerData_.end   = rulerData_.end;

  invalidateOverlay();

  updatePlots();
}

void
CQChartsView::
rulerMouseMotion()
{
  updateMousePosText();
}

void
CQChartsView::
rulerMouseMove()
{
  auto w = pixelToWindow(mouseMovePoint());

  rulerData_.set = true;
  rulerData_.end = w;

  invalidateOverlay();

  updatePlots();
}

void
CQChartsView::
rulerMouseRelease()
{
  auto w = pixelToWindow(mouseMovePoint());

  rulerData_.set = true;
  rulerData_.end = w;

  invalidateOverlay();

  updatePlots();
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

  doUpdate();
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
updateMousePosText()
{
  updatePosText(mouseMovePoint());
}

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
  probeData_.regionBand.init(this);

  probeData_.regionBand.setGeometry(BBox(pos, pos));
  probeData_.regionBand.show();
}

void
CQChartsView::
updateRegionBand(Plot *plot, const Point &pressPoint, const Point &movePoint)
{
  updateRegionBand(pressPoint, movePoint);

  if (! plot->allowZoomX() || ! plot->allowZoomY()) {
    double x = probeData_.regionBand.x     ();
    double y = probeData_.regionBand.y     ();
    double w = probeData_.regionBand.width ();
    double h = probeData_.regionBand.height();

    auto pixelRect = plot->calcPlotPixelRect();

    if (! plot->allowZoomX()) {
      x = int(pixelRect.getXMin());
      w = int(pixelRect.getWidth());
    }

    if (! plot->allowZoomY()) {
      y = int(pixelRect.getYMin());
      h = int(pixelRect.getHeight());
    }

    probeData_.regionBand.setGeometry(BBox(x, y, x + w, y + h));
    probeData_.regionBand.show();
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

  probeData_.regionBand.setGeometry(BBox(x, y, x + w, y + h));
}

void
CQChartsView::
endRegionBand()
{
  probeData_.regionBand.hide();
}

//---

void
CQChartsView::
queryMousePress()
{
  auto wpos = this->pixelToWindow(mouseData_.pressPoint);

  showQueryAt(wpos);
}

void
CQChartsView::
queryMouseMotion()
{
  updateMousePosText();

  searchMouse();
}

void
CQChartsView::
showQueryAt(const Point &wpos)
{
  QString tip;

  calcTip(wpos, tip, /*single*/false);

  showQuery(tip);
}

void
CQChartsView::
showQuery(const QString &text)
{
  Q_EMIT showQueryText(text);
}

//------

void
CQChartsView::
updateSelText()
{
  Plot::Objs objs;

  selectedObjs(objs);

  for (auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    if (plot->isSelected())
      objs.push_back(plot);
  }

  //---

  int num = int(objs.size());

  for (auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    Plot::Objs objs1;

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
    setSelText(QString::number(num));
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
  Plot::Objs objs1;

  selectedObjs(objs1);

  for (const auto &obj1 : objs1)
    objs.push_back(obj1);

  for (auto &plot : plots()) {
    if (! plot->isVisible())
      continue;

    Plot::Objs objs1;

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
  if (is3D()) {
    assert(glWidget_);

    glWidget_->move(0, 0);
    glWidget_->resize(width(), height());

    glWidget_->setVisible(true);
    glWidget_->raise();

    for (auto *o : glWidget_->children()) {
      auto *w = qobject_cast<QWidget *>(o);

      if (w) {
        w->setVisible(true);
        w->raise();
      }
    }

    return;
  }

  updateSeparators();

  //---

  int w, h;

  {
  lockPainter(true);

  w = width ();
  h = height();

  delete ipainter_;
  delete image_;

  int iw = std::min(std::max((! isAutoSize() ? sizeData_.width  : w), 1), 16384);
  int ih = std::min(std::max((! isAutoSize() ? sizeData_.height : h), 1), 16384);

  image_ = CQChartsUtil::newImage(QSize(iw, ih));

  image_->fill(Qt::transparent);

  ipainter_ = new QPainter(image_);

  lockPainter(false);
  }

  //---

  sizeData_.xpos = 0;
  sizeData_.ypos = 0;

  if (isAutoSize()) {
    if (sizeData_.hbar) sizeData_.hbar->setVisible(false);
    if (sizeData_.vbar) sizeData_.vbar->setVisible(false);

    invalidateObjects();
    invalidateOverlay();

    doResize(w, h);
  }
  else {
    bool showHBar = (sizeData_.width  > w);
    bool showVBar = (sizeData_.height > h);

    if (showHBar) {
      if (! sizeData_.hbar) {
        sizeData_.hbar = new QScrollBar(Qt::Horizontal, this);
        sizeData_.hbar->setObjectName("viewHBar");

        connect(sizeData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hbarScrollSlot(int)));
      }
    }

    if (showVBar) {
      if (! sizeData_.vbar) {
        sizeData_.vbar = new QScrollBar(Qt::Vertical, this);
        sizeData_.vbar->setObjectName("viewVBar");

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

    invalidateObjects();
    invalidateOverlay();

    // needed if size not changed ?
    doResize(sizeData_.width, sizeData_.height);

    doUpdate();
  }
}

void
CQChartsView::
doResize(int w, int h)
{
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

  doUpdate();
}

void
CQChartsView::
vbarScrollSlot(int pos)
{
  sizeData_.ypos = pos;

  doUpdate();
}

void
CQChartsView::
updateSeparators()
{
  auto clearSeparators = [&]() {
    for (auto &separator : separators_)
      delete separator;

    separators_.clear();
  };

  if (! isPlotSeparators()) {
    clearSeparators();
  }
  else {
    Plots plots;

    this->getPlots(plots);

    int np = int(plots.size());

    if (np < 2) {
      clearSeparators();
      return;
    }

    using PosPlots = std::map<int, Plot *>;

    PosPlots xPlots, yPlots;

    for (const auto &plot : plots) {
      const auto &bbox = plot->viewBBox();

      xPlots[CMathRound::Round(bbox.getXMin())] = plot;
      yPlots[CMathRound::Round(bbox.getYMin())] = plot;
    }

    double vr = CQChartsView::viewportRange();

    if (separatorsInvalid_) {
      plotsHorizontal_ = true;

      double x = 0.0;

      for (const auto &pp : xPlots) {
        const auto &bbox = pp.second->viewBBox();

        if (plotsHorizontal_) {
          if (! CMathUtil::realEq(bbox.getXMin(), x) ||
              ! CMathUtil::realEq(bbox.getYMin(), 0.0) ||
              ! CMathUtil::realEq(bbox.getHeight(), vr)) {
            plotsHorizontal_ = false;
          }
        }

        x += bbox.getWidth();
      }

      plotsVertical_ = true;

      double y = 0.0;

      for (const auto &pp : yPlots) {
        const auto &bbox = pp.second->viewBBox();

        if (plotsVertical_) {
          if (! CMathUtil::realEq(bbox.getXMin(), 0.0) ||
              ! CMathUtil::realEq(bbox.getYMin(), y) ||
              ! CMathUtil::realEq(bbox.getWidth(), vr)) {
            plotsVertical_ = false;
          }
        }

        y += bbox.getHeight();
      }

      if (! plotsHorizontal_ && ! plotsVertical_) {
        for (auto &separator : separators_)
          delete separator;

        separators_.clear();

        return;
      }

      CQChartsUtil::makeArraySize(separators_, size_t(np - 1), [&]() {
        auto *sep = new CQChartsSplitter(this, Qt::Vertical);

        sep->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        sep->setObjectName(QString("splitter.%1").arg(separators_.size() + 1));

        return sep;
      } );

      separatorsInvalid_ = false;
    }

    if      (plotsHorizontal_) {
      double x = 0.0;
      int    i = 0;

      Plot *lastPlot = nullptr;

      for (const auto &pp : xPlots) {
        const auto &bbox = pp.second->viewBBox();

        if (i > 0) {
          auto *sep = separators_[size_t(i - 1)];

          sep->setOrientation(Qt::Vertical);

          double px1 = this->width()*x/vr;

          sep->move(int(px1 - 4), 0);
          sep->resize(8, height());

          sep->setVisible(true);
          sep->raise();

          sep->setPlot1(lastPlot);
          sep->setPlot2(pp.second);
        }

        lastPlot = pp.second; x += bbox.getWidth(); ++i;
      }
    }
    else if (plotsVertical_) {
      double y = 0.0;
      int    i = 0;

      Plot *lastPlot = nullptr;

      for (const auto &pp : yPlots) {
        const auto &bbox = pp.second->viewBBox();

        if (i > 0) {
          auto *sep = separators_[size_t(i - 1)];

          sep->setOrientation(Qt::Horizontal);

          double py1 = this->height()*y/vr;

          sep->move(0, int(py1 - 4));
          sep->resize(width(), 8);

          sep->setVisible(true);
          sep->raise();

          sep->setPlot1(lastPlot);
          sep->setPlot2(pp.second);
        }

        lastPlot = pp.second; y += bbox.getHeight(); ++i;
      }
    }
  }
}

//------

void
CQChartsView::
paintEvent(QPaintEvent *)
{
  if (is3D())
    return;

  //---

  if (mode() == Mode::RULER)
    invalidateOverlay();

  //---

  lockPainter(true);

  paint(ipainter_);

  QPainter painter(this);

  painter.drawImage(-sizeData_.xpos, -sizeData_.ypos, *image_);

  lockPainter(false);
}

void
CQChartsView::
paint(QPainter *painter, Plot *plot)
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
drawBackground(PaintDevice *device) const
{
  // fill background
  PenBrush penBrush;

  BrushData brushData(true, interpBackgroundFillColor(ColorInd()),
                      backgroundFillAlpha(), backgroundFillPattern());

  setBrush(penBrush, brushData);

  device->setBrush(penBrush.brush);

  //device->fillRect(prect_);

  double vr = CQChartsView::viewportRange();

  BBox bbox(0, 0, vr, vr);

  device->fillRect(bbox);
}

void
CQChartsView::
drawPlots(QPainter *painter)
{
  bool hasPlots         = ! plots().empty();
  bool hasBgAnnotations = this->hasAnnotations(Layer::Type::BG_ANNOTATION);
  bool hasFgAnnotations = this->hasAnnotations(Layer::Type::FG_ANNOTATION);

  //---

  // draw no data
  if (! hasPlots && ! hasBgAnnotations && ! hasFgAnnotations && ! isPreview()) {
    showNoData(true);

    auto *painter1 = bgBuffer_->beginPaint(painter, rect());

    if (painter1) {
      auto *th = const_cast<CQChartsView *>(this);

      CQChartsViewPaintDevice device(th, painter1);

      drawNoData(&device);
    }

    bgBuffer_->endPaint();

    return;
  }

  showNoData(false);

  //---

  // draw bg annotations
  if (hasBgAnnotations) {
    auto *painter1 = bgBuffer_->beginPaint(painter, rect());

    if (painter1) {
      auto *th = const_cast<CQChartsView *>(this);

      CQChartsViewPaintDevice device(th, painter1);

      // draw bg annotations
      drawAnnotations(&device, CQChartsLayer::Type::BG_ANNOTATION);
    }

    bgBuffer_->endPaint();
  }

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

  // draw fg annotations and key
  if (hasFgAnnotations || hasPlots) {
    auto *painter1 = fgBuffer_->beginPaint(painter, rect());

    if (painter1) {
      auto *th = const_cast<CQChartsView *>(this);

      CQChartsViewPaintDevice device(th, painter1);

      // draw fg annotations
      if (hasFgAnnotations)
        drawAnnotations(&device, CQChartsLayer::Type::FG_ANNOTATION);

      //---

      // draw view key
      if (hasPlots)
        drawKey(&device, CQChartsLayer::Type::FG_KEY);
    }

    fgBuffer_->endPaint();
  }

  //---

  // draw overlay (annotations and key)
  if (hasPlots || hasBgAnnotations || hasFgAnnotations || mode() == Mode::RULER)
    drawOverlay(painter);
}

void
CQChartsView::
drawOverlay(QPainter *painter)
{
  auto *painter1 = overlayBuffer_->beginPaint(painter, rect());

  if (painter1) {
    bool hasBgAnnotations = this->hasAnnotations(Layer::Type::BG_ANNOTATION);
    bool hasFgAnnotations = this->hasAnnotations(Layer::Type::FG_ANNOTATION);

    auto *th = const_cast<CQChartsView *>(this);

    CQChartsViewPaintDevice device(th, painter1);

    if (hasBgAnnotations || hasFgAnnotations) {
      // draw selected/mouse over annotations
      drawAnnotations(&device, CQChartsLayer::Type::SELECTION);
      drawAnnotations(&device, CQChartsLayer::Type::MOUSE_OVER);
    }

    //---

    // draw selected/mouse over view key
    drawKey(&device, CQChartsLayer::Type::SELECTION);
    drawKey(&device, CQChartsLayer::Type::MOUSE_OVER);

    //---

    if (mode() == Mode::RULER)
      drawRuler(&device);
  }

  overlayBuffer_->endPaint();
}

void
CQChartsView::
drawRuler(PaintDevice *device)
{
  if (! rulerData_.set)
    return;

  auto *currentPlot = this->currentPlot(/*remap*/true);

  Point p1, p2;

  if (currentPlot) {
    if      (rulerData_.units == Units::PLOT) {
      p1 = currentPlot->viewToWindow(rulerData_.start);
      p2 = currentPlot->viewToWindow(rulerData_.end);
    }
    else if (rulerData_.units == Units::VIEW) {
      p1 = rulerData_.start;
      p2 = rulerData_.end;
    }
    else if (rulerData_.units == Units::PIXEL) {
      p1 = currentPlot->windowToPixel(currentPlot->viewToWindow(rulerData_.start));
      p2 = currentPlot->windowToPixel(currentPlot->viewToWindow(rulerData_.end  ));
    }
    else
      return;
  }
  else {
    if      (rulerData_.units == Units::VIEW) {
      p1 = rulerData_.start;
      p2 = rulerData_.end;
    }
    else if (rulerData_.units == Units::PIXEL) {
      p1 = windowToPixel(rulerData_.start);
      p2 = windowToPixel(rulerData_.end);
    }
    else
      return;
  }

  //---

  auto unitsStr = " (" + CQChartsUnits::unitsString(rulerData_.units) + ")";

  //---

  bool usePen     = true;
  bool forceColor = false;

  CQChartsAxis xaxis(this, Qt::Horizontal);
  CQChartsAxis yaxis(this, Qt::Vertical  );

  auto rs = Point(std::min(rulerData_.start.x, rulerData_.end.x),
                  std::min(rulerData_.start.y, rulerData_.end.y));

  xaxis.setPosition  (CQChartsOptReal(rs.y));
  xaxis.setRange     (rulerData_.start.x, rulerData_.end.x);
  xaxis.setValueRange(0.0, std::abs(p2.x - p1.x));
  xaxis.setLabel     (CQChartsOptString(QString::number(std::abs(p2.x - p1.x)) + unitsStr));

  yaxis.setPosition  (CQChartsOptReal(rs.x));
  yaxis.setRange     (rulerData_.start.y, rulerData_.end.y);
  yaxis.setValueRange(0.0, std::abs(p2.y - p1.y));
  yaxis.setLabel     (CQChartsOptString(QString::number(std::abs(p2.y - p1.y)) + unitsStr));

  CQChartsViewPaintDevice pdevice(this, device->painter());

  xaxis.draw(this, &pdevice, usePen, forceColor);
  yaxis.draw(this, &pdevice, usePen, forceColor);

  auto d = std::hypot(p2.x - p1.x, p2.y - p1.y);

  pdevice.drawLine(rulerData_.start, rulerData_.end);
  pdevice.drawText((rulerData_.start + rulerData_.end)/2, QString::number(d) + unitsStr);
}

//---

void
CQChartsView::
propertyItemChanged(QObject *, const QString &)
{
}

//---

void
CQChartsView::
updateNoData()
{
  updateNoData_ = true;

  bool hasPlots         = ! plots().empty();
  bool hasBgAnnotations = hasAnnotations(Layer::Type::BG_ANNOTATION);
  bool hasFgAnnotations = hasAnnotations(Layer::Type::FG_ANNOTATION);

  if (! hasPlots && ! hasBgAnnotations && ! hasFgAnnotations) {
    invalidateObjects();

    doUpdate();
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
drawNoData(PaintDevice *)
{
  auto p_font = this->font().font();
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
hasAnnotations(const Layer::Type &layerType) const
{
  auto isAnnotationLayer = [&](const Annotation *annotation, const CQChartsLayer::Type &layerType) {
    return ((layerType == Layer::Type::BG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::BACKGROUND) ||
            (layerType == Layer::Type::FG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::FOREGROUND));
  };

  //---

  bool anyObjs = false;

  for (const auto &annotation : annotations()) {
    if (! annotation->isVisible())
      continue;

    if      (layerType == Layer::Type::SELECTION) {
      if (! annotation->isSelected())
        continue;
    }
    else if (layerType == Layer::Type::MOUSE_OVER) {
      if (! annotation->isInside())
        continue;
    }
    else {
      if (! isAnnotationLayer(annotation, layerType))
        continue;
    }

    anyObjs = true;

    break;
  }

  if (! anyObjs)
    return false;

  //---

  return true;
}

void
CQChartsView::
drawAnnotations(PaintDevice *device, const CQChartsLayer::Type &layerType)
{
  auto isAnnotationLayer = [&](const Annotation *annotation, const CQChartsLayer::Type &layerType) {
    return ((layerType == Layer::Type::BG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::BACKGROUND) ||
            (layerType == Layer::Type::FG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::FOREGROUND));
  };

  //---

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
    else {
      if (! isAnnotationLayer(annotation, layerType))
        continue;
    }

    annotation->draw(device);

    if (layerType == CQChartsLayer::Type::SELECTION) {
      if (mode() == CQChartsView::Mode::EDIT && annotation->isSelected())
        if (device->isInteractive())
          annotation->drawEditHandles(device);
    }
  }
}

void
CQChartsView::
drawKey(PaintDevice *device, const CQChartsLayer::Type &layerType)
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
      if (device->isInteractive())
        key()->drawEditHandles(device);
    }
  }
}

bool
CQChartsView::
lockPainter(bool lock)
{
  if (lock) {
    //if (painterLocked_)
    //  return false;

    painterMutex_.lock();
  }
  else
    painterMutex_.unlock();

  //painterLocked_ = lock;

  return true;
}

//-----

double
CQChartsView::
limitLineWidth(double w) const
{
  // ensure not a crazy number : TODO: property for limits
  return CMathUtil::clamp(w, 0.0, charts()->maxLineWidth());
}

//------

void
CQChartsView::
setPenBrush(PenBrush &penBrush, const PenData &penData, const BrushData &brushData) const
{
  setPen  (penBrush, penData  );
  setBrush(penBrush, brushData);
}

void
CQChartsView::
setPen(PenBrush &penBrush, const PenData &penData) const
{
  double width = limitLineWidth(lengthPixelWidth(penData.width()));

  CQChartsUtil::setPen(penBrush.pen, penData.isVisible(), penData.color(), penData.alpha(),
                       width, penData.dash(), penData.lineCap(), penData.lineJoin());
}

void
CQChartsView::
setBrush(PenBrush &penBrush, const BrushData &brushData) const
{
  CQChartsDrawUtil::setBrush(penBrush.brush, brushData);

  if (brushData.pattern().altColor().isValid())
    penBrush.altColor = brushData.pattern().altColor().color();
  else
    penBrush.altColor = QColor(Qt::transparent);

  if (brushData.pattern().altAlpha().isSet())
    penBrush.altAlpha = brushData.pattern().altAlpha().value();
  else
    penBrush.altAlpha = 1.0;

  penBrush.fillAngle = brushData.pattern().angle().degrees();
  penBrush.fillType  = brushData.pattern().type();
}

//------

void
CQChartsView::
updateObjPenBrushState(const CQChartsObj *obj, PenBrush &penBrush, DrawType drawType) const
{
  updateObjPenBrushState(obj, ColorInd(), penBrush, drawType);
}

void
CQChartsView::
updateObjPenBrushState(const CQChartsObj *obj, const ColorInd &colorInd,
                       PenBrush &penBrush, DrawType drawType) const
{
  if (isOverlayFade())
    return;

  //---

  if (! isBufferLayers()) {
    // inside and selected
    if      (obj->isInside() && obj->isSelected()) {
      updateSelectedPenBrushState(colorInd, penBrush, drawType);
      updateInsidePenBrushState  (colorInd, penBrush, /*outline*/false, drawType);
    }
    // inside
    else if (obj->isInside()) {
      updateInsidePenBrushState(colorInd, penBrush, /*outline*/true, drawType);
    }
    // selected
    else if (obj->isSelected()) {
      updateSelectedPenBrushState(colorInd, penBrush, drawType);
    }
    else {
      updateSelectedPenBrushState(colorInd, penBrush, drawType);
    }
  }
  else {
    // inside
    if      (drawLayerType() == CQChartsLayer::Type::MOUSE_OVER ||
             drawLayerType() == CQChartsLayer::Type::MOUSE_OVER_EXTRA) {
      if (obj->isInside())
        updateInsidePenBrushState(colorInd, penBrush, /*outline*/true, drawType);
    }
    // selected
    else if (drawLayerType() == CQChartsLayer::Type::SELECTION ||
             drawLayerType() == CQChartsLayer::Type::SELECTION_EXTRA) {
      if (obj->isSelected()) {
        updateSelectedPenBrushState(colorInd, penBrush, drawType);
      }
      else {
        updateSelectedPenBrushState(colorInd, penBrush, drawType);
      }
    }
  }
}

void
CQChartsView::
updatePenBrushState(const ColorInd &colorInd, PenBrush &penBrush, bool selected, bool inside)
{
  if      (inside) {
    if (selected) {
      updateSelectedPenBrushState(colorInd, penBrush);
      updateInsidePenBrushState  (colorInd, penBrush);
    }
    else
      updateInsidePenBrushState(colorInd, penBrush);
  }
  else if (selected)
    updateSelectedPenBrushState(colorInd, penBrush);
}

void
CQChartsView::
updateInsidePenBrushState(const ColorInd &colorInd, PenBrush &penBrush,
                          bool outline, DrawType drawType) const
{
  // fill and stroke
  if (drawType != DrawType::LINE) {
    // outline box, symbol
    if      (insideMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor opc;
      Alpha  alpha;

      if (penBrush.pen.style() != Qt::NoPen) {
        auto pc = penBrush.pen.color();

        charts()->setContrastColor(pc);

        if (isInsideStroked())
          opc = interpInsideStrokeColor(colorInd);
        else
          opc = CQChartsUtil::invColor(pc);

      //alpha = Alpha(pc.alphaF());
      }
      else {
        auto bc = penBrush.brush.color();

        charts()->setContrastColor(bc);

        if (isInsideStroked())
          opc = interpInsideStrokeColor(colorInd);
        else
          opc = CQChartsUtil::invColor(bc);
      }

      setPen(penBrush, PenData(true, opc, alpha, insideStrokeWidth(), insideStrokeDash()));

      if (outline)
        setBrush(penBrush, BrushData(false));

      charts()->resetContrastColor();
    }
    // fill box, symbol
    else if (insideMode() == CQChartsView::HighlightDataMode::FILL) {
      auto bc = penBrush.brush.color();

      charts()->setContrastColor(bc);

      QColor ibc;

      if (isInsideFilled()) {
        auto ic1 = colorInd; ic1.c = bc;

        ibc = interpInsideFillColor(ic1);
      }
      else
        ibc = calcInsideColor(bc);

      Alpha alpha;

      if (isBufferLayers() && isInsideFilled()) {
        if (isInsideBlend())
          alpha = Alpha(insideFillAlpha().value()*bc.alphaF());
        else
          alpha = insideFillAlpha();
      }
      else {
        if (isInsideBlend())
          alpha = Alpha(bc.alphaF());
        else
          alpha = Alpha();
      }

      setBrush(penBrush, BrushData(true, ibc, alpha, insideFillPattern()));

      charts()->resetContrastColor();
    }
  }
  // just stroke
  else {
    auto pc = penBrush.pen.color();

    charts()->setContrastColor(pc);

    QColor opc;

    if (isInsideStroked())
      opc = interpInsideStrokeColor(colorInd);
    else
      opc = CQChartsUtil::invColor(pc);

    Alpha alpha(pc.alphaF());

    setPen(penBrush, PenData(true, opc, alpha, insideStrokeWidth(), insideStrokeDash()));

    charts()->resetContrastColor();
  }
}

void
CQChartsView::
updateSelectedPenBrushState(const ColorInd &colorInd, PenBrush &penBrush, DrawType drawType) const
{
  // fill and stroke
  if      (drawType != DrawType::LINE) {
    // outline box, symbol
    if      (selectedMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor opc;
      Alpha  alpha;

      if (penBrush.pen.style() != Qt::NoPen) {
        auto pc = penBrush.pen.color();

        charts()->setContrastColor(pc);

        if (isSelectedStroked())
          opc = interpSelectedStrokeColor(colorInd);
        else
          opc = calcSelectedColor(pc);

        alpha = Alpha(pc.alphaF());
      }
      else {
        auto bc = penBrush.brush.color();

        charts()->setContrastColor(bc);

        if (isSelectedStroked())
          opc = interpSelectedStrokeColor(colorInd);
        else
          opc = CQChartsUtil::invColor(bc);
      }

      setPen(penBrush, PenData(true, opc, alpha, selectedStrokeWidth(), selectedStrokeDash()));

      setBrush(penBrush, BrushData(false));

      charts()->resetContrastColor();
    }
    // fill box, symbol
    else if (selectedMode() == CQChartsView::HighlightDataMode::FILL) {
      auto bc = penBrush.brush.color();

      charts()->setContrastColor(bc);

      QColor ibc;

      if (isSelectedFilled())
        ibc = interpSelectedFillColor(colorInd);
      else
        ibc = calcSelectedColor(bc);

      Alpha alpha;

      if (isBufferLayers() && isSelectedFilled()) {
        if (isInsideBlend())
          alpha = Alpha(selectedFillAlpha().value()*bc.alphaF());
        else
          alpha = selectedFillAlpha();
      }
      else {
        if (isInsideBlend())
          alpha = Alpha(bc.alphaF());
        else
          alpha = Alpha();
      }

      setBrush(penBrush, BrushData(true, ibc, alpha, selectedFillPattern()));

      charts()->resetContrastColor();
    }
  }
  // just stroke
  else if (penBrush.pen.style() != Qt::NoPen) {
    auto pc = penBrush.pen.color();

    charts()->setContrastColor(pc);

    QColor opc;

    if (isSelectedStroked())
      opc = interpSelectedStrokeColor(colorInd);
    else
      opc = CQChartsUtil::invColor(pc);

    Alpha alpha(pc.alphaF());

    setPen(penBrush, PenData(true, opc, alpha, selectedStrokeWidth(), selectedStrokeDash()));

    charts()->resetContrastColor();
  }
}

QColor
CQChartsView::
calcInsideColor(const QColor &c) const
{
  QColor c1;

  if (insideColor().isValid()) {
    charts()->setContrastColor(c);

    c1 = interpColor(insideColor(), ColorInd());

    charts()->resetContrastColor();
  }
  else
    c1 = c;

  return CQChartsUtil::blendColors(c1, CQChartsUtil::bwColor(c1), insideAlpha());
}

QColor
CQChartsView::
calcSelectedColor(const QColor &c) const
{
  QColor c1;

  if (selectedColor().isValid()) {
    charts()->setContrastColor(c);

    c1 = interpColor(selectedColor(), ColorInd());

    charts()->resetContrastColor();
  }
  else
    c1 = c;

  return CQChartsUtil::blendColors(c1, CQChartsUtil::bwColor(c1), selectedAlpha());
}

//------

void
CQChartsView::
updateSlot()
{
  doUpdate();
}

void
CQChartsView::
doUpdate()
{
  update();
}

//------

void
CQChartsView::
searchMouse()
{
  searchData_.pos = mouseMovePoint();

  if (searchData_.timer)
    searchData_.timer->start();
  else
    searchSlot();
}

void
CQChartsView::
searchSlot()
{
  setStatusText("");

  auto w = pixelToWindow(searchData_.pos);

  searchAt(w);
}

void
CQChartsView::
searchAt(const Point &w)
{
  plotsAt(w, mouseData_.plots, mouseData_.plot, /*clear*/true, /*first*/true);

  //---

  Plot::Constraints constraints = Plot::Constraints::SELECTABLE;

  if (mode() == Mode::EDIT)
    constraints = Plot::Constraints::EDITABLE;

  //---

  auto p = windowToPixel(w);

  bool handled = false;

  processMouseDataPlots([&](Plot *plot, const Point &pos) {
    auto w = plot->pixelToWindow(pos);

    if (plot->handleSelectMove(w, constraints, ! handled))
      handled = true;

    return false;
  }, p);

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

    doUpdate();
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
  bgBuffer_->setValid(false);
  fgBuffer_->setValid(false);
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

  Plots plots;
  Plot* plot { nullptr };

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
    return CQUtil::createActionGroup(menu);
  };

  auto addAction = [&](QMenu *menu, const QString &name, const char *slotName) {
    return CQUtil::addAction(menu, name, this, slotName);
  };

  auto addCheckAction = [&](QMenu *menu, const QString &name, bool checked, const char *slotName) {
    return CQUtil::addCheckedAction(menu, name, checked, this, slotName);
  };

  auto addGroupCheckAction = [&](QActionGroup *group, const QString &name, bool checked,
                                 const char *slotName) {
    return CQUtil::addGroupCheckAction(group, name, checked, this, slotName);
  };

  //---

  // Mode Menu
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
    addGroupCheckAction(modeActionGroup, "Ruler" , mode() == Mode::RULER , SLOT(rulerModeSlot()));

    modeActionGroup->setExclusive(true);

    CQUtil::addActionGroupToMenu(modeActionGroup);
  }

  //---

  // Edit Object
  Plot::Objs objs;

  allSelectedObjs(objs);

  auto *plotObj = (objs.size() ? objs[0] : nullptr);

  auto *selAnnotation = qobject_cast<Annotation    *>(plotObj);
  auto *selAxis       = qobject_cast<CQChartsAxis  *>(plotObj);
  auto *selKey        = qobject_cast<CQChartsKey   *>(plotObj);
  auto *selTitle      = qobject_cast<CQChartsTitle *>(plotObj);

  if (selAnnotation || selAxis || selKey || selTitle) {
    auto *objMenu = addSubMenu(popupMenu, "Selected");

    addAction(objMenu, "Edit", SLOT(editObjectSlot()));

    if (selAnnotation)
      addAction(objMenu, "Remove", SLOT(removeObjectSlot()));
  }

  //---

  popupMenu->addSeparator();

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

    addKeyLocationActions(viewKeyMenu, key()->location(),
                          this, SLOT(viewKeyPositionSlot(QAction *)),
                          /*includeAuto*/false);
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
      // skip invisible plots
      if (! plot->isVisible())
        continue;

      //---

      int ind = getIndForPlot(plot);

      auto *plotAction =
        addGroupCheckAction(plotsGroup, plot->id(), false, SLOT(currentPlotSlot()));

      if (currentPlot)
        plotAction->setChecked(plot == currentPlot);
      else
        plotAction->setChecked(currentPlotInd() == ind);

      plotAction->setData(ind);
    }

    CQUtil::addActionGroupToMenu(plotsGroup);
  }

  //------

  if (plotType && plotType->hasKey()) {
    auto *plotKeyMenu = addSubMenu(popupMenu, "Plot Key");

    //---

    auto addKeyCheckAction = [&](const QString &label, bool checked, const char *slot) {
      return CQUtil::addCheckedAction(plotKeyMenu, label, checked, this, slot);
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

    auto location = (plotKey ? plotKey->location() : CQChartsKeyLocation());

    addKeyLocationActions(plotKeyMenu, location,
                          this, SLOT(plotKeyPositionSlot(QAction *)),
                          /*includeAuto*/true);

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

    auto *xAxisSideGroup = createActionGroup(xAxisSideMenu);

    auto addXAxisSideGroupAction = [&](const QString &label, const CQChartsAxisSide &side) {
      auto *action = CQUtil::addGroupCheckAction(xAxisSideGroup, label, false);

      xAxisSideActionMap[side] = action;

      return action;
    };

    addXAxisSideGroupAction("Bottom", CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
    addXAxisSideGroupAction("Top"   , CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT  ));

    if (xAxis)
      xAxisSideActionMap[xAxis->side()]->setChecked(true);

    connect(xAxisSideGroup, SIGNAL(triggered(QAction *)), this, SLOT(xAxisSideSlot(QAction *)));

    CQUtil::addActionGroupToMenu(xAxisSideGroup);
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

    auto *yAxisSideGroup = createActionGroup(yAxisSideMenu);

    auto addYAxisSideGroupAction = [&](const QString &label, const CQChartsAxisSide &side) {
      auto *action = CQUtil::addGroupCheckAction(yAxisSideGroup, label, false);

      yAxisSideActionMap[side] = action;

      return action;
    };

    addYAxisSideGroupAction("Left" , CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
    addYAxisSideGroupAction("Right", CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT  ));

    if (yAxis)
      yAxisSideActionMap[yAxis->side()]->setChecked(true);

    connect(yAxisSideGroup, SIGNAL(triggered(QAction *)), this, SLOT(yAxisSideSlot(QAction *)));

    CQUtil::addActionGroupToMenu(yAxisSideGroup);
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

    auto *titleLocationGroup = createActionGroup(titleLocationMenu);

    auto addTitleLocationGroupAction =
     [&](const QString &label, const CQChartsTitleLocation::Type &location) {
      auto *action = CQUtil::addGroupCheckAction(titleLocationGroup, label, false);

      titleLocationActionMap[location] = action;

      return action;
    };

    auto locationNames = QStringList() <<
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

    CQUtil::addActionGroupToMenu(titleLocationGroup);
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
    addAction(popupMenu, "Zoom Data", SLOT(zoomDataSlot()));
    addAction(popupMenu, "Zoom Full", SLOT(zoomFullSlot()));
  }

  //---

  popupMenu->addSeparator();

  if (hasPlots && window())
    addCheckAction(popupMenu, "Show Table", isShowTable(), SLOT(setShowTable(bool)));

  if (window())
    addCheckAction(popupMenu, "Show Settings", isShowSettings(), SLOT(setShowSettings(bool)));

  //---

  popupMenu->addSeparator();

  auto *themeMenu = addSubMenu(popupMenu, "Theme");

  auto *interfaceGroup = createActionGroup(themeMenu);

  auto addInterfaceAction = [&](const QString &label, const char *slotName) {
    return CQUtil::addGroupCheckAction(interfaceGroup, label, false, this, slotName);
  };

  auto *themeGroup = createActionGroup(themeMenu);

  auto addThemeAction = [&](const QString &label, const char *slotName) {
    return CQUtil::addGroupCheckAction(themeGroup, label, false, this, slotName);
  };

  auto *lightPaletteAction = addInterfaceAction("Light", SLOT(lightPaletteSlot()));
  auto *darkPaletteAction  = addInterfaceAction("Dark" , SLOT(darkPaletteSlot()));

  lightPaletteAction->setChecked(! isDark());
  darkPaletteAction ->setChecked(  isDark());

  CQUtil::addActionGroupToMenu(interfaceGroup);

  //---

  QStringList themeNames;

  CQColorsMgrInst->getThemeNames(themeNames);

  for (const auto &themeName : themeNames) {
    auto *theme = CQColorsMgrInst->getNamedTheme(themeName) ;

    auto *themeAction = addThemeAction(theme->desc(), SLOT(themeNameSlot()));

    themeAction->setData(theme->name());

    themeAction->setChecked(this->themeName().name() == theme->name());
  }

  CQUtil::addActionGroupToMenu(themeGroup);

  //---

  // add Menus for current plot

  if (currentPlot) {
    if (currentPlot->addMenuItems(popupMenu, w))
      popupMenu->addSeparator();
  }

  //---

  if (hasPlots) {
    auto *printMenu = addSubMenu(popupMenu, "Print");

    addAction(printMenu, "PNG", SLOT(printPNGSlot()));
    addAction(printMenu, "SVG", SLOT(printSVGSlot()));

    addAction(printMenu, "SVG/Html", SLOT(writeSVGSlot()));
    addAction(printMenu, "JS/Html" , SLOT(writeScriptSlot()));

    addAction(printMenu, "Stats" , SLOT(writeStatsSlot()));
  }

  //---

  if (hasPlots) {
    if (CQChartsEnv::getBool("CQ_CHARTS_DEBUG", true)) {
      auto *showBoxesAction =
        addCheckAction(popupMenu, "Show Boxes", false, SLOT(showBoxesSlot(bool)));

      if (basePlot)
        showBoxesAction->setChecked(basePlot->showBoxes());

      auto *showSelectedBoxesAction =
        addCheckAction(popupMenu, "Show Selected Boxes", false, SLOT(showSelectedBoxesSlot(bool)));

      if (basePlot)
        showSelectedBoxesAction->setChecked(basePlot->showSelectedBoxes());

      //---

      addCheckAction(popupMenu, "Buffer Layers", isBufferLayers(), SLOT(bufferLayersSlot(bool)));
    }
  }

  //---

  if (hasPlots) {
    popupMenu->addSeparator();

    addAction(popupMenu, "Help", SLOT(helpSlot()));
  }

  //---

  popupMenu->popup(mapToGlobal(p.qpointi()));
}

void
CQChartsView::
addKeyLocationActions(QMenu *menu, const CQChartsKeyLocation &location,
                      QObject *slotObj, const char *slotName, bool includeAuto)
{
  auto addSubMenu = [](QMenu *menu, const QString &name) {
    auto *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto createActionGroup = [](QMenu *menu) {
    return CQUtil::createActionGroup(menu);
  };

  //---

  using KeyLocationActionMap = std::map<CQChartsKeyLocation::Type, QAction *>;

  KeyLocationActionMap keyLocationActionMap;

  auto *keyLocationMenu = addSubMenu(menu, "Location");

  auto *keyLocationActionGroup = createActionGroup(keyLocationMenu);

  auto addKeyLocationGroupAction =
   [&](const QString &label, const CQChartsKeyLocation::Type &location) {
    auto *action = CQUtil::addGroupCheckAction(keyLocationActionGroup, label, false);

    keyLocationActionMap[location] = action;

    return action;
  };

  auto locationNames = QStringList() <<
    "Top Left"    << "Top Center"    << "Top Right"    <<
    "Center Left" << "Center Center" << "Center Right" <<
    "Bottom Left" << "Bottom Center" << "Bottom Right" <<
    "Absolute Position" << "Absolute Rectangle";

  if (includeAuto)
    locationNames << "Auto";

  for (const auto &name : locationNames) {
    CQChartsKeyLocation::Type type;

    if (! CQChartsKeyLocation::decodeString(name, type))
      assert(false);

    addKeyLocationGroupAction(name, type);
  }

  keyLocationActionGroup->setExclusive(true);

  auto p = keyLocationActionMap.find(location.type());

  if (p != keyLocationActionMap.end())
    (*p).second->setChecked(true);

  connect(keyLocationActionGroup, SIGNAL(triggered(QAction *)), slotObj, slotName);

  CQUtil::addActionGroupToMenu(keyLocationActionGroup);
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
  Plot::Objs objs;

  allSelectedObjs(objs);

  auto *plotObj = (objs.size() ? objs[0] : nullptr);

  auto *selAnnotation = qobject_cast<Annotation    *>(plotObj);
  auto *selAxis       = qobject_cast<CQChartsAxis  *>(plotObj);
  auto *selKey        = qobject_cast<CQChartsKey   *>(plotObj);
  auto *selTitle      = qobject_cast<CQChartsTitle *>(plotObj);

  if      (selAnnotation) {
    delete editAnnotationDlg_;

    editAnnotationDlg_ = new CQChartsEditAnnotationDlg(this, selAnnotation);

    editAnnotationDlg_->show();
    editAnnotationDlg_->raise();
  }
  else if (selAxis) {
    delete editAxisDlg_;

    editAxisDlg_ = new CQChartsEditAxisDlg(this, selAxis);

    editAxisDlg_->show();
    editAxisDlg_->raise();
  }
  else if (selKey) {
    delete editKeyDlg_;

    editKeyDlg_ = new CQChartsEditKeyDlg(this, selKey);

    editKeyDlg_->show();
    editKeyDlg_->raise();
  }
  else if (selTitle) {
    delete editTitleDlg_;

    editTitleDlg_ = new CQChartsEditTitleDlg(this, selTitle);

    editTitleDlg_->show();
    editTitleDlg_->raise();
  }
}

void
CQChartsView::
removeObjectSlot()
{
  Plot::Objs objs;

  allSelectedObjs(objs);

  auto *plotObj = (objs.size() ? objs[0] : nullptr);

  auto *selAnnotation = qobject_cast<Annotation *>(plotObj);

  if (selAnnotation) {
    if (selAnnotation->plot()) {
      selAnnotation->plot()->removeAnnotation(selAnnotation);

      selAnnotation->plot()->drawObjs();
    }
    else
      selAnnotation->view()->removeAnnotation(selAnnotation);

    doUpdate();
  }
}

//------

void
CQChartsView::
viewKeyVisibleSlot(bool b)
{
  if (key() && b != key()->isVisible()) {
    key()->setVisible(b);

    updateView();
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

  viewKey->setLocation(CQChartsKeyLocation(location));

  updateView();
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

    plotKey->setLocation(CQChartsKeyLocation(location));
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
      xAxis->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
    else if (action->text() == "Top")
      xAxis->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT  ));
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
      yAxis->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::BOTTOM_LEFT));
    else if (action->text() == "Right")
      yAxis->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT  ));
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

    title->setLocation(CQChartsTitleLocation(location));
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
zoomDataSlot()
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->zoomToData();
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

  auto *dlg = CQChartsHelpDlgMgrInst->showDialog(charts);

  auto *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot)
    dlg->setCurrentSection(QString("plot_types/%1").arg(currentPlot->type()->name()));
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

  auto name = action->data().toString();

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

  setInsideStrokeWidth(Length::pixel(2));

  updateAll();

  Q_EMIT themePalettesChanged();
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
printFile(const QString &filename, Plot *plot)
{
  auto p = filename.lastIndexOf(".");

  if (p > 0) {
    auto suffix = filename.mid(p + 1).toLower();

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
  auto dir = QDir::current().dirName() + "/charts.png";

  auto filename = QFileDialog::getSaveFileName(this, "Print PNG", dir, "Files (*.png)");
  if (! filename.length()) return; // cancelled

  printPNGSlot(filename);
}

void
CQChartsView::
printPNGSlot(const QString &filename)
{
  (void) printPNG(filename);
}

void
CQChartsView::
printSVGSlot()
{
  auto dir = QDir::current().dirName() + "/charts.svg";

  auto filename = QFileDialog::getSaveFileName(this, "Print SVG", dir, "Files (*.svg)");
  if (! filename.length()) return; // cancelled

  printSVGSlot(filename);
}

void
CQChartsView::
printSVGSlot(const QString &filename)
{
  (void) printSVG(filename);
}

void
CQChartsView::
writeSVGSlot()
{
  auto dir = QDir::current().dirName() + "/charts_svg.html";

  auto filename = QFileDialog::getSaveFileName(this, "Write SVG/Html", dir, "Files (*.html)");
  if (! filename.length()) return; // cancelled

  writeSVGSlot(filename);
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
  auto dir = QDir::current().dirName() + "/charts_js.html";

  auto filename = QFileDialog::getSaveFileName(this, "Write JS/Html", dir, "Files (*.html)");
  if (! filename.length()) return; // cancelled

  writeScriptSlot(filename);
}

bool
CQChartsView::
printPNG(const QString &filename, Plot *plot)
{
  int w = width ();
  int h = height();

  auto image = CQChartsUtil::initImage(QSize(w, h));

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
printSVG(const QString &filename, Plot *plot)
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
writeSVG(const QString &filename, Plot *plot)
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
  CQChartsJS::writeToolTipProcs(os, PaintDevice::Type::SVG);

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
  bool hasBgAnnotations = hasAnnotations(Layer::Type::BG_ANNOTATION);
  bool hasFgAnnotations = hasAnnotations(Layer::Type::FG_ANNOTATION);

  if (hasBgAnnotations || hasFgAnnotations) {
    for (auto &annotation : annotations()) {
      annotation->writeHtml(&device);
    }
  }

  // write custom html for plots
  if (plot) {
    CQChartsSVGPaintDevice device(const_cast<Plot *>(plot), os);

    plot->writeHtml(&device);
  }
  else {
    for (auto &plot : plots()) {
      CQChartsSVGPaintDevice device(const_cast<Plot *>(plot), os);

      plot->writeHtml(&device);
    }
  }

  //---

  // svg block
  os << "<svg xmlns=\"http://www.w3.org/2000/svg\""
        " xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.2\""
        " width=\"" << width() << "\" height=\"" << height() << "\">\n";

  //---

  // draw background
  drawBackground(&device);

  //---

  // draw specific plot
  if (plot) {
    CQChartsSVGPaintDevice device(const_cast<Plot *>(plot), os);

    plot->writeSVG(&device);
  }
  // draw all plots
  else {
    for (auto &plot : plots()) {
      CQChartsSVGPaintDevice device(const_cast<Plot *>(plot), os);

      plot->writeSVG(&device);
    }

    //---

    if (hasBgAnnotations || hasFgAnnotations) {
      CQChartsSVGPaintDevice device(th, os);

      // draw annotations
      drawAnnotations(&device, CQChartsLayer::Type::BG_ANNOTATION);
      drawAnnotations(&device, CQChartsLayer::Type::FG_ANNOTATION);
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
writeScript(const QString &filename, Plot *plot)
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
  CQChartsJS::writeToolTipProcs(os, PaintDevice::Type::SCRIPT);

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
  os << "Charts.prototype.init = function() {\n";
  os << "  this.canvas = document.getElementById(\"canvas\");\n";
  os << "  this.gc = this.canvas.getContext(\"2d\");\n";
  os << "\n";

  os << "  this.pwidth = " << width(); os << ";\n";
  os << "  this.pheight = " << height(); os << ";\n";
  os << "  this.aspect = " << aspect(); os << ";\n";
  os << "\n";

  os << "  this.canvas.width  = window.innerWidth;\n";
  os << "  this.canvas.height = window.innerHeight;\n";
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
  os << "  this.xmax = 1.0;\n";
  os << "  this.ymax = 1.0;\n";
  os << "\n";
  os << "  this.canvas.addEventListener(\"mousedown\", this.eventMouseDown, false);\n";
  os << "  this.canvas.addEventListener(\"mousemove\", this.eventMouseMove, false);\n";
  os << "  this.canvas.addEventListener(\"mouseup\"  , this.eventMouseUp  , false);\n";
  os << "\n";

  //---

  // create specific plot object
  if (plot) {
    std::string plotId = CQChartsJS::encodeId("plot_" + plot->id().toStdString());

    os << "  this." << plotId << " = new Charts_" << plotId << "();\n";
    os << "  this.plots.push(this." << plotId << ");\n";
    os << "  this." << plotId << ".init();\n";
  }
  // create plot objects
  else {
    int i = 0;

    for (auto &plot : plots()) {
      std::string plotId = CQChartsJS::encodeId("plot_" + plot->id().toStdString());

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

  device.startGroup("background");

  drawBackground(&device);

  device.endGroup();

  //---

  // draw annotations proc
  bool hasBgAnnotations = hasAnnotations(Layer::Type::BG_ANNOTATION);
  bool hasFgAnnotations = hasAnnotations(Layer::Type::FG_ANNOTATION);

  if (hasBgAnnotations) {
    device.startGroup("bgAnnotations");

    drawAnnotations(&device, CQChartsLayer::Type::BG_ANNOTATION);

    device.endGroup();
  }

  if (hasFgAnnotations) {
    device.startGroup("fgAnnotations");

    drawAnnotations(&device, CQChartsLayer::Type::FG_ANNOTATION);

    device.endGroup();
  }

  //---

  os << "\n";
  os << "Charts.prototype.update = function() {\n";

  os << "  this.drawBackground();\n";
  os << "\n";

  // draw specific plot
  if (plot) {
    std::string plotId = CQChartsJS::encodeId("plot_" + plot->id().toStdString());

    os << "  this." << plotId << ".draw();\n";
  }
  // draw all plots
  else {
    for (auto &plot : plots()) {
      std::string plotId = CQChartsJS::encodeId("plot_" + plot->id().toStdString());

      os << "  this." << plotId << ".draw();\n";
    }
  }

  //---

  // draw view annotations
  if (hasBgAnnotations || hasFgAnnotations) {
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

    if (hasBgAnnotations)
      os << "  this.drawBgAnnotations();\n";

    if (hasFgAnnotations)
      os << "  this.drawFgAnnotations();\n";
  }

  os << "}\n";

  //---

  os << "\n";

  if (plot) {
    CQChartsScriptPaintDevice device(const_cast<Plot *>(plot), os);

    plot->writeScript(&device);
  }
  else {
    for (auto &plot : plots()) {
      CQChartsScriptPaintDevice device(const_cast<Plot *>(plot), os);

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
  if (hasBgAnnotations || hasFgAnnotations) {
    for (auto &annotation : annotations()) {
      annotation->writeHtml(&device);
    }
  }

  // write custom html for plots
  if (plot) {
    CQChartsScriptPaintDevice device(const_cast<Plot *>(plot), os);

    plot->writeHtml(&device);
  }
  else {
    for (auto &plot : plots()) {
      CQChartsScriptPaintDevice device(const_cast<Plot *>(plot), os);

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

//---

void
CQChartsView::
writeStatsSlot()
{
  writeStats(BBox());
}

void
CQChartsView::
writeStats(const BBox &bbox)
{
  auto *th = const_cast<CQChartsView *>(this);

  CQChartsStatsPaintDevice device(th);

  for (auto &plot : plots())
    plot->writeStats(&device);

  device.print(bbox);
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
showSelectedBoxesSlot(bool b)
{
  auto *currentPlot = this->currentPlot(/*remap*/true);
  auto *basePlot    = (currentPlot ? this->basePlot(currentPlot) : nullptr);

  if (basePlot)
    basePlot->setShowSelectedBoxes(b);
}

//---

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

//------

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

bool
CQChartsView::
isViewSettingsMajorObjects() const
{
  return (window() ? window()->isViewSettingsMajorObjects() : false);
}

void
CQChartsView::
setViewSettingsMajorObjects(bool b)
{
  if (window())
    window()->setViewSettingsMajorObjects(b);
}

bool
CQChartsView::
isViewSettingsMinorObjects() const
{
  return (window() ? window()->isViewSettingsMinorObjects() : false);
}

void
CQChartsView::
setViewSettingsMinorObjects(bool b)
{
  if (window())
    window()->setViewSettingsMinorObjects(b);
}

int
CQChartsView::
viewSettingsMaxObjects() const
{
  return (window() ? window()->viewSettingsMaxObjects() : 100);
}

void
CQChartsView::
setViewSettingsMaxObjects(int n)
{
  if (window())
    window()->setViewSettingsMaxObjects(n);
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
  auto plotInd = CQChartsVariant::toInt(action->data(), ok);
  assert(ok);

  setCurrentPlotInd(int(plotInd));
}

//------

void
CQChartsView::
updateView()
{
  invalidateObjects();
  invalidateOverlay();

  doUpdate();
}

void
CQChartsView::
updateAll()
{
  updatePlots();

  invalidateObjects();
  invalidateOverlay();

  doUpdate();
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

  doUpdate();
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

      Plots oplots;

      if      (plot->isX1X2())
        plot->x1x2Plots(oplots);
      else if (plot->isY1Y2())
        plot->y1y2Plots(oplots);
      else if (plot->isOverlay())
        plot->overlayPlots(oplots);

      for (const auto &oplot : oplots)
        plots.push_back(oplot);
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
basePlot(Plot *plot) const
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
plotsAt(const Point &p, Plots &plots, Plot* &plot, bool clear, bool first) const
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
  Plots plots;
  Plot* plot;

  (void) plotsAt(p, plots, plot, true);

  return plot;
}

bool
CQChartsView::
plotsAt(const Point &p, Plots &plots, bool clear) const
{
  Plot *plot;

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
plotBBox(Plot *plot) const
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
  int np = int(plots().size());

  if (ind < 0 || ind >= np)
    return nullptr;

  auto *plot = plots_[size_t(ind)];

  return plot;
}

int
CQChartsView::
getIndForPlot(const Plot *plot) const
{
  if (! plot)
    return -1;

  auto np = plots().size();

  for (size_t ind = 0; ind < np; ++ind) {
    if (plots_[ind] == plot)
      return int(ind);
  }

  return -1;
}

int
CQChartsView::
calcCurrentPlotInd(bool remap) const
{
  if (plots().empty())
    return -1;

  Plot *plot = nullptr;

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
updateTip()
{
  updateTip(QCursor::pos());
}

void
CQChartsView::
updateTip(const QPoint &gpos)
{
  if      (tipData_.toolTip->isVisible())
    tipData_.toolTip->showTip(gpos);
  else if (tipData_.floatTip->isVisible())
    tipData_.floatTip->updateTip();
}

//---

void
CQChartsView::
setStatusText(const QString &text)
{
  Q_EMIT statusTextChanged(text);
}

void
CQChartsView::
setPosText(const QString &text)
{
  Q_EMIT posTextChanged(text);
}

void
CQChartsView::
setSelText(const QString &text)
{
  Q_EMIT selTextChanged(text);
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

  Q_EMIT scrollDataChanged();
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
  using PlotVars  = std::map<Plot*, QString>;

  ModelVars modelVars;
  PlotVars  plotVars;

  for (const auto &plot : plots) {
    auto *modelData = plot->getModelData();

    QString modelVarName;

    if (modelData) {
      auto id = modelData->id();

      auto p = modelVars.find(id);

      if (p == modelVars.end()) {
        os << "\n";

        auto modelVarName = QString("model%1").arg(modelVars.size() + 1);

        p = modelVars.insert(p, ModelVars::value_type(id, modelVarName));

        modelData->write(os, modelVarName);
      }

      modelVarName = (*p).second;
    }

    os << "\n";

    auto plotVarName = QString("plot%1").arg(plotVars.size() + 1);

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
    if (plot->isX1X2() || plot->isY1Y2() || plot->isOverlay()) {
      Plots oplots;

      os << "\n";
      os << "group_charts_plots ";

      if      (plot->isX1X2())
        plot->x1x2Plots(oplots);
      else if (plot->isY1Y2())
        plot->y1y2Plots(oplots);
      else
        plot->overlayPlots(oplots);

      if      (plot->isX1X2())
        os << "-x1x2";
      else if (plot->isY1Y2())
        os << "-y1y2";

      if (plot->isOverlay())
        os << " -overlay";

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
      str.clear();

    os << "set_charts_property -view $view -name " << nv.first.toStdString() <<
          " -value {" << str.toStdString() << "}\n";
  }
}

//------

CQChartsGeom::Point
CQChartsView::
positionToView(const Position &pos) const
{
  auto p  = pos.p();
  auto p1 = p;

  if      (pos.units() == Units::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == Units::VIEW)
    p1 = p;
  else if (pos.units() == Units::PERCENT) {
    p1.setX(p.getX()*viewportRange()/100.0);
    p1.setY(p.getY()*viewportRange()/100.0);
  }
  else if (pos.units() == Units::EM) {
    double x = pixelToWindowWidth (p.getX()*fontEm());
    double y = pixelToWindowHeight(p.getY()*fontEm());

    return Point(x, y);
  }
  else if (pos.units() == Units::EX) {
    double x = pixelToWindowWidth (p.getX()*fontEx());
    double y = pixelToWindowHeight(p.getY()*fontEx());

    return Point(x, y);
  }

  return p1;
}

CQChartsGeom::Point
CQChartsView::
positionToPixel(const Position &pos) const
{
  auto p  = pos.p();
  auto p1 = p;

  if      (pos.units() == Units::PIXEL)
    p1 = p;
  else if (pos.units() == Units::VIEW)
    p1 = windowToPixel(p);
  else if (pos.units() == Units::PERCENT) {
    p1.setX(p.getX()*width ()/100.0);
    p1.setY(p.getY()*height()/100.0);
  }
  else if (pos.units() == Units::EM) {
    double x = p.getX()*fontEm();
    double y = p.getY()*fontEm();

    return Point(x, y);
  }
  else if (pos.units() == Units::EX) {
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

  if      (rect.units() == Units::PIXEL)
    r1 = pixelToWindow(r);
  else if (rect.units() == Units::VIEW)
    r1 = r;
  else if (rect.units() == Units::PERCENT) {
    r1.setXMin(r.getXMin()*viewportRange()/100.0);
    r1.setYMin(r.getYMin()*viewportRange()/100.0);
    r1.setXMax(r.getXMax()*viewportRange()/100.0);
    r1.setYMax(r.getYMax()*viewportRange()/100.0);
  }
  else if (rect.units() == Units::EM) {
    double x1 = pixelToWindowWidth (r.getXMin()*fontEm());
    double y1 = pixelToWindowHeight(r.getYMin()*fontEm());
    double x2 = pixelToWindowWidth (r.getXMax()*fontEm());
    double y2 = pixelToWindowHeight(r.getYMax()*fontEm());

    return BBox(x1, y1, x2, y2);
  }
  else if (rect.units() == Units::EX) {
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

  if      (rect.units() == Units::PIXEL)
    r1 = r;
  else if (rect.units() == Units::VIEW)
    r1 = windowToPixel(r);
  else if (rect.units() == Units::PERCENT) {
    r1.setXMin(r.getXMin()*width ()/100.0);
    r1.setYMin(r.getYMin()*height()/100.0);
    r1.setXMax(r.getXMax()*width ()/100.0);
    r1.setYMax(r.getYMax()*height()/100.0);
  }
  else if (rect.units() == Units::EM) {
    double x1 = r.getXMin()*fontEm();
    double y1 = r.getYMin()*fontEm();
    double x2 = r.getXMax()*fontEm();
    double y2 = r.getYMax()*fontEm();

    return BBox(x1, y1, x2, y2);
  }
  else if (rect.units() == Units::EX) {
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
lengthViewWidth(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return pixelToWindowWidth(len.value());
  else if (len.units() == Units::VIEW)
    return len.value();
  else if (len.units() == Units::PERCENT)
    return len.value()*viewportRange()/100.0;
  else if (len.units() == Units::EM)
    return pixelToWindowWidth(len.value()*fontEm());
  else if (len.units() == Units::EX)
    return pixelToWindowWidth(len.value()*fontEx());

  return len.value();
}

double
CQChartsView::
lengthViewHeight(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return pixelToWindowHeight(len.value());
  else if (len.units() == Units::VIEW)
    return len.value();
  else if (len.units() == Units::PERCENT)
    return len.value()*viewportRange()/100.0;
  else if (len.units() == Units::EM)
    return pixelToWindowHeight(len.value()*fontEm());
  else if (len.units() == Units::EX)
    return pixelToWindowHeight(len.value()*fontEx());

  return len.value();
}

double
CQChartsView::
lengthViewSignedWidth(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return pixelToSignedWindowWidth(len.value());
  else if (len.units() == Units::VIEW)
    return len.value();
  else if (len.units() == Units::PERCENT)
    return len.value()*viewportRange()/100.0;
  else if (len.units() == Units::EM)
    return pixelToSignedWindowWidth(len.value()*fontEm());
  else if (len.units() == Units::EX)
    return pixelToSignedWindowWidth(len.value()*fontEx());

  return len.value();
}

double
CQChartsView::
lengthViewSignedHeight(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return pixelToSignedWindowHeight(len.value());
  else if (len.units() == Units::VIEW)
    return len.value();
  else if (len.units() == Units::PERCENT)
    return len.value()*viewportRange()/100.0;
  else if (len.units() == Units::EM)
    return pixelToSignedWindowHeight(len.value()*fontEm());
  else if (len.units() == Units::EX)
    return pixelToSignedWindowHeight(len.value()*fontEx());

  return len.value();
}

double
CQChartsView::
lengthPixelWidth(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return len.value();
  else if (len.units() == Units::VIEW)
    return windowToPixelWidth(len.value());
  else if (len.units() == Units::PERCENT) {
    int w = (isAutoSize() ? width() : sizeData_.width);

    return len.value()*w/100.0;
  }
  else if (len.units() == Units::EM)
    return len.value()*fontEm();
  else if (len.units() == Units::EX)
    return len.value()*fontEx();

  return len.value();
}

double
CQChartsView::
lengthPixelHeight(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return len.value();
  else if (len.units() == Units::VIEW)
    return windowToPixelHeight(len.value());
  else if (len.units() == Units::PERCENT) {
    int h = (isAutoSize() ? height() : sizeData_.height);

    return len.value()*h/100.0;
  }
  else if (len.units() == Units::EM)
    return len.value()*fontEm();
  else if (len.units() == Units::EX)
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

  int np = int(poly.size());

  for (int i = 0; i < np; ++i)
    ppoly.addPoint(windowToPixel(poly.point(i)));

  return ppoly;
}

QPainterPath
CQChartsView::
windowToPixel(const QPainterPath &path) const
{
  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(const View *view) :
     view_(view) {
    }

    void moveTo(const Point &p) override {
      auto pp = view_->windowToPixel(p);

      path_.moveTo(pp.qpoint());
    }

    void lineTo(const Point &p) override {
      auto pp = view_->windowToPixel(p);

      path_.lineTo(pp.qpoint());
    }

    void quadTo(const Point &p1, const Point &p2) override {
      auto pp1 = view_->windowToPixel(p1);
      auto pp2 = view_->windowToPixel(p2);

      path_.quadTo(pp1.qpoint(), pp2.qpoint());
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      auto pp1 = view_->windowToPixel(p1);
      auto pp2 = view_->windowToPixel(p2);
      auto pp3 = view_->windowToPixel(p3);

      path_.cubicTo(pp1.qpoint(), pp2.qpoint(), pp3.qpoint());
    }

    const QPainterPath &path() const { return path_; }

   private:
    const View*  view_ { nullptr };
    QPainterPath path_;
  };

  PathVisitor visitor(this);

  CQChartsDrawUtil::visitPath(path, visitor);

  return visitor.path();
}

//---

QSize
CQChartsView::
sizeHint() const
{
  return viewSizeHint();
}

//------

CQChartsSplitter::
CQChartsSplitter(CQChartsView *view, Qt::Orientation orientation) :
 QFrame(view), view_(view), orientation_(orientation)
{
}

void
CQChartsSplitter::
setOrientation(const Qt::Orientation &o)
{
  orientation_ = o;

  setCursor(orientation_ != Qt::Horizontal ? Qt::SplitHCursor : Qt::SplitVCursor);
}

void
CQChartsSplitter::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QStyleOption opt(0);

  opt.rect    = rect();
  opt.palette = palette();

  if (isHorizontal())
    opt.state = QStyle::State_Horizontal;
  else
    opt.state = QStyle::State_None;

  if (hover_)
    opt.state |= QStyle::State_MouseOver;
  if (pressed_)
    opt.state |= QStyle::State_Sunken;
  if (isEnabled())
    opt.state |= QStyle::State_Enabled;

  style()->drawControl(QStyle::CE_Splitter, &opt, &painter, this);

  painter.setPen(opt.palette.color(QPalette::Dark));

  painter.drawRect(rect());
}

void
CQChartsSplitter::
mousePressEvent(QMouseEvent *event)
{
  pressed_ = true;

  initPos_  = pos();
  pressPos_ = event->globalPos();
}

void
CQChartsSplitter::
mouseMoveEvent(QMouseEvent *event)
{
  if (! pressed_) return;

  movePos_ = event->globalPos();

  if (isHorizontal()) {
    int dy = movePos_.y() - pressPos_.y();

    int y1 = initPos_.y() + dy;

    y1 = std::min(std::max(y1, 0), view_->height() -1);

    move(x(), y1);
  }
  else {
    int dx = movePos_.x() - pressPos_.x();

    int x1 = initPos_.x() + dx;

    x1 = std::min(std::max(x1, 0), view_->width() -1);

    move(x1, y());
  }
}

void
CQChartsSplitter::
mouseReleaseEvent(QMouseEvent *event)
{
  using BBox = CQChartsGeom::BBox;

  pressed_ = false;

  movePos_ = event->globalPos();

  auto bbox1 = plot1_->viewBBox();
  auto bbox2 = plot2_->viewBBox();

  double vr = view_->viewportRange();

  if (isHorizontal()) {
    int dy = movePos_.y() - pressPos_.y();

    double ph1 = view_->height()*bbox1.getHeight()/vr;
    double ph2 = view_->height()*bbox2.getHeight()/vr;

    ph1 = ph1 - dy; // bottom
    ph2 = ph2 + dy; // top

    double h1 = ph1*vr/view_->height();
    double h2 = ph2*vr/view_->height();

    bbox1 = BBox(bbox1.getXMin(), bbox1.getYMin(), bbox1.getXMax(), bbox1.getYMin() + h1);
    bbox2 = BBox(bbox2.getXMin(), bbox1.getYMax(), bbox2.getXMax(), bbox1.getYMax() + h2);
  }
  else {
    int dx = movePos_.x() - pressPos_.x();

    double pw1 = view_->width()*bbox1.getWidth()/vr;
    double pw2 = view_->width()*bbox2.getWidth()/vr;

    pw1 = pw1 + dx; // left
    pw2 = pw2 - dx; // right

    double w1 = pw1*vr/view_->width();
    double w2 = pw2*vr/view_->width();

    bbox1 = BBox(bbox1.getXMin(), bbox1.getYMin(), bbox1.getXMin() + w1, bbox1.getYMax());
    bbox2 = BBox(bbox1.getXMax(), bbox2.getYMax(), bbox1.getXMax() + w2, bbox2.getYMax());
  }

  plot1_->setViewBBox(bbox1);
  plot2_->setViewBBox(bbox2);
}

bool
CQChartsSplitter::
event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::HoverEnter: hover_ = true ; update(); break;
    case QEvent::HoverLeave: hover_ = false; update(); break;
    default:                 break;
  }

  return QFrame::event(event);
}

//---

bool
CQChartsView::
executeSlot(const QString &name, const QStringList &args, QVariant &res)
{
  res = QVariant();

  //---

  auto p = namedSlots_.find(name);
  if (p == namedSlots_.end()) return false;

  const auto &argTypes = (*p).second;

  QVariantList values;

  processSlotArgs(args, argTypes, values);

  return executeSlotFn(name, values, res);
}

bool
CQChartsView::
processSlotArgs(const QStringList &args, const QStringList &argTypes, QVariantList &values) const
{
  bool rc = true;

  int numArgs = argTypes.length();

  auto args1 = args;

  while (args1.length() < numArgs)
    args1.push_back(QString());

  for (int i = 0; i < numArgs; ++i) {
    auto argType = argTypes[i].toLower();

    const auto &argValue = args1[i];

    auto type = CQBaseModel::nameType(argType);

    QVariant value;

    bool ok;

    if      (type == CQBaseModelType::INTEGER) {
      long i = CQChartsUtil::toInt(argValue, ok);

      if (ok)
        value = CQChartsVariant::fromInt(i);
      else {
        std::cerr << "Failed to convert '" << argValue.toStdString() << "' to integer\n";
        rc = false;
      }
    }
    else if (type == CQBaseModelType::REAL) {
      double r = CQChartsUtil::toReal(argValue, ok);

      if (ok)
        value = CQChartsVariant::fromReal(r);
      else {
        std::cerr << "Failed to convert '" << argValue.toStdString() << "' to real\n";
        rc = false;
      }
    }
    else if (type == CQBaseModelType::STRING) {
      value = argValue;
    }
    else if (type == CQBaseModelType::BOOLEAN) {
      bool b = CQChartsUtil::stringToBool(argValue, &ok);

      if (ok)
        value = CQChartsVariant::fromBool(b);
      else {
        std::cerr << "Failed to convert '" << argValue.toStdString() << "' to boolean\n";
        rc = false;
      }
    }
    else {
      std::cerr << "Unhandled type '" << argType.toStdString() << "'\n";
      rc = false;
    }

    values.push_back(value);
  }

  return rc;
}

bool
CQChartsView::
executeSlotFn(const QString &name, const QVariantList &args, QVariant &)
{
  bool ok;

  if      (name == "set_mode") {
    auto modeStr = CQChartsVariant::toString(args[0], ok);
    if      (modeStr == "select"  ) setMode(Mode::SELECT);
    else if (modeStr == "zoom_in" ) setMode(Mode::ZOOM_IN);
    else if (modeStr == "zoom_out") setMode(Mode::ZOOM_OUT);
    else if (modeStr == "pan"     ) setMode(Mode::PAN);
    else if (modeStr == "probe"   ) setMode(Mode::PROBE);
    else if (modeStr == "query"   ) setMode(Mode::QUERY);
    else if (modeStr == "edit"    ) setMode(Mode::EDIT);
    else if (modeStr == "region"  ) setMode(Mode::REGION);
    else if (modeStr == "ruler"   ) setMode(Mode::RULER);
    else { std::cerr << "Invalid mode '" << modeStr.toStdString() << "'\n"; return false; }
  }
  else if (name == "fit")
    fitSlot();
  else if (name == "zoom_data")
    zoomDataSlot();
  else if (name == "zoom_full")
    zoomFullSlot();
  else if (name == "show_table")
    setShowTable(CQChartsVariant::toBool(args[0], ok));
  else if (name == "show_settings")
    setShowSettings(CQChartsVariant::toBool(args[0], ok));
  else
    return false;

  return true;
}

void
CQChartsView::
registerSlot(const QString &name, const QStringList &argTypes)
{
  namedSlots_[name] = argTypes;
}

QStringList
CQChartsView::
getSlotNames() const
{
  QStringList names;

  for (const auto &p : namedSlots_)
    names.push_back(p.first);

  return names;
}

//---
