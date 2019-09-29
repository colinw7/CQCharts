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
#include <CQChartsEditAnnotationDlg.h>
#include <CQChartsAxisEdit.h>
#include <CQChartsKeyEdit.h>
#include <CQChartsTitleEdit.h>
#include <CQChartsEditHandles.h>
#include <CQChartsPaintDevice.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColors.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>

#include <QSvgGenerator>
#include <QFileDialog>
#include <QRubberBand>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMenu>

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

  setSelectedFillAlpha(0.8);
  setInsideFillAlpha  (0.8);

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

  if (searchTimeout_ > 0) {
    searchTimer_ = new QTimer(this);

    searchTimer_->setInterval(searchTimeout_);
    searchTimer_->setSingleShot(true);

    connect(searchTimer_, SIGNAL(timeout()), this, SLOT(searchSlot()));
  }

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
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                    const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden=false) {
    CQPropertyViewItem *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
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
  addStyleProp("text", "fontFactor", "factor", "Global text scale factor");
  addStyleProp("text", "font"      , "font"  , "Global text font");

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
  addStyleProp("select/highlight/stroke", "selectedStroked"    , "visible",
               "Highlight stroke visible");
  addStyleProp("select/highlight/stroke", "selectedStrokeColor", "color"  ,
               "Highlight stroke color");
  addStyleProp("select/highlight/stroke", "selectedStrokeWidth", "width"  ,
               "Highlight stroke width");
  addStyleProp("select/highlight/stroke", "selectedStrokeDash" , "dash"   ,
               "Highlight stroke dash");

  // inside highlight
  addStyleProp("inside/highlight"       , "insideMode"       , "mode"   , "Inside draw mode");
  addStyleProp("inside/highlight"       , "insideShapeData"  , "style"  , "Inside shape data");
  addStyleProp("inside/highlight/fill"  , "insideFilled"     , "visible", "Inside fill visible");
  addStyleProp("inside/highlight/fill"  , "insideFillColor"  , "color"  , "Inside fill color");
  addStyleProp("inside/highlight/fill"  , "insideFillAlpha"  , "alpha"  , "Inside fill alpha");
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
  CQChartsPlot *plot = currentPlot(/*remap*/false);

  setScrolled(true, /*update*/false);

  PlotSet basePlots;

  this->basePlots(basePlots);

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

    this->basePlots(basePlots);

    if (scrollData_.active) {
      scrollData_.plotBBoxMap.clear();

      for (auto &plot : basePlots)
        scrollData_.plotBBoxMap[plot->id()] = plot->viewBBox();

      int pos = 0;

      for (auto &plot : basePlots) {
        plot->setViewBBox(CQChartsGeom::BBox(pos, 0, pos + 100, 100));

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
        placePlots(plots_, /*vertical*/false, /*horizontal*/true, /*rows*/1, /*cols*/1);
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

  CQChartsUtil::testAndSet(sizeData_.autoSize , b, [&]() { resizeEvent(nullptr); } );
}

void
CQChartsView::
setFixedSize(const QSize &size)
{
  bool resize = false;

  CQChartsUtil::testAndSet(sizeData_.width , size.width (), [&]() { resize = true; } );
  CQChartsUtil::testAndSet(sizeData_.height, size.height(), [&]() { resize = true; } );

  sizeData_.sizeSet = true;

  if (resize)
    resizeEvent(nullptr);
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

#if 0
void
CQChartsView::
setPainterFont(QPainter *painter, const CQChartsFont &font) const
{
  painter->setFont(viewFont(font));
}
#endif

#if 0
void
CQChartsView::
setPainterFont(QPainter *painter, const QFont &font) const
{
  painter->setFont(viewFont(font));
}
#endif

void
CQChartsView::
setPlotPainterFont(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                   const CQChartsFont &font) const
{
  device->setFont(plotFont(plot, font));
}

#if 0
void
CQChartsView::
setPlotPainterFont(const CQChartsPlot *plot, QPainter *painter, const CQChartsFont &font) const
{
  painter->setFont(plotFont(plot, font));
}

void
CQChartsView::
setPlotPainterFont(const CQChartsPlot *plot, QPainter *painter, const QFont &font) const
{
  painter->setFont(plotFont(plot, font));
}
#endif

QFont
CQChartsView::
viewFont(const CQChartsFont &font) const
{
  // Calc specified font from view font
  QFont font1 = font.calcFont(font_.font());

  if (isScaleFont())
    return scaledFont(font1, this->size());
  else
    return font1;
}

QFont
CQChartsView::
viewFont(const QFont &font) const
{
  if (isScaleFont())
    return scaledFont(font, this->size());
  else
    return font;
}

QFont
CQChartsView::
plotFont(const CQChartsPlot *plot, const CQChartsFont &font) const
{
  // adjust font by plot font and then by view font
  CQChartsFont font1 = font.calcFont(plot->font());
  QFont        font2 = font1.calcFont(font_.font());

  if (isScaleFont())
    return scaledFont(font2, plot->calcPixelSize());
  else
    return font2;
}

QFont
CQChartsView::
plotFont(const CQChartsPlot *plot, const QFont &font) const
{
  if (isScaleFont())
    return scaledFont(font, plot->calcPixelSize());
  else
    return font;
}

double
CQChartsView::
calcFontScale(const QSizeF &size) const
{
  // calc scale factor
  double sx = (size.width () > 0 ? size.width ()/defSizeHint().width () : 1.0);
  double sy = (size.height() > 0 ? size.height()/defSizeHint().height() : 1.0);

  return fontFactor()*std::min(sx, sy);
}

QFont
CQChartsView::
scaledFont(const QFont &font, const QSizeF &size) const
{
  double s = calcFontScale(size);

  return scaledFont(font, s);
}

QFont
CQChartsView::
scaledFont(const QFont &font, double s) const
{
  double fs = font.pointSizeF()*s;

  QFont font1 = font;

  if (fs > 0)
    font1.setPointSizeF(fs);

  return font1;
}

//---

void
CQChartsView::
setCurrentPlot(CQChartsPlot *plot)
{
  if (plot)
    setCurrentPlotInd(plotInd(plot));
  else
    setCurrentPlotInd(-1);
}

void
CQChartsView::
setCurrentPlotInd(int ind)
{
  if (ind != currentPlotInd_) {
    CQChartsPlot *currentPlot = getPlotForInd(currentPlotInd_);

    if (currentPlot)
      disconnect(currentPlot, SIGNAL(zoomPanChanged()), this, SLOT(currentPlotZoomPanChanged()));

    currentPlotInd_ = ind;

    currentPlot = getPlotForInd(currentPlotInd_);

    if (currentPlot)
      connect(currentPlot, SIGNAL(zoomPanChanged()), this, SLOT(currentPlotZoomPanChanged()));

    //---

    if (isScrolled()) {
      PlotSet basePlots;

      this->basePlots(basePlots);

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
}

void
CQChartsView::
currentPlotZoomPanChanged()
{
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
  setMode(Mode::ZOOM);
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
  CQChartsUtil::testAndSet(selectMode_, selectMode, [&]() { emit selectModeChanged(); } );
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
  for (auto &plot : plots_)
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
  ++selecting_;
}

void
CQChartsView::
endSelection()
{
  --selecting_;

  if (selecting_ == 0) {
    emit selectionChanged();

    updateSelText();
  }
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
setThemeName(const CQChartsThemeName &theme)
{
  charts()->setPlotTheme(theme);

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
    const QString  &name  = nv.first;
    const QVariant &value = nv.second;

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
  const CQPropertyViewItem *item = propertyModel()->propertyItem(this, name, hidden);
  if (! item) return false;

  desc = item->desc();

  return true;
}

bool
CQChartsView::
getPropertyType(const QString &name, QString &type, bool hidden) const
{
  const CQPropertyViewItem *item = propertyModel()->propertyItem(this, name, hidden);
  if (! item) return false;

  type = item->typeName();

  return true;
}

bool
CQChartsView::
getPropertyUserType(const QString &name, QString &type, bool hidden) const
{
  const CQPropertyViewItem *item = propertyModel()->propertyItem(this, name, hidden);
  if (! item) return false;

  type = item->userTypeName();

  return true;
}

bool
CQChartsView::
getPropertyObject(const QString &name, QObject* &object, bool hidden) const
{
  object = nullptr;

  const CQPropertyViewItem *item = propertyModel()->propertyItem(this, name, hidden);
  if (! item) return false;

  object = item->object();

  return true;
}

bool
CQChartsView::
getPropertyIsHidden(const QString &name, bool &is_hidden) const
{
  is_hidden = false;

  const CQPropertyViewItem *item = propertyModel()->propertyItem(this, name, /*hidden*/true);
  if (! item) return false;

  is_hidden = CQCharts::getItemIsHidden(item);

  return true;
}

bool
CQChartsView::
getPropertyIsStyle(const QString &name, bool &is_style) const
{
  is_style = false;

  const CQPropertyViewItem *item = propertyModel()->propertyItem(this, name, /*hidden*/true);
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

//---

CQChartsArrowAnnotation *
CQChartsView::
addArrowAnnotation(const CQChartsPosition &start, const CQChartsPosition &end)
{
  CQChartsArrowAnnotation *arrowAnnotation = new CQChartsArrowAnnotation(this, start, end);

  addAnnotation(arrowAnnotation);

  return arrowAnnotation;
}

CQChartsEllipseAnnotation *
CQChartsView::
addEllipseAnnotation(const CQChartsPosition &center, const CQChartsLength &xRadius,
                     const CQChartsLength &yRadius)
{
  CQChartsEllipseAnnotation *ellipseAnnotation =
    new CQChartsEllipseAnnotation(this, center, xRadius, yRadius);

  addAnnotation(ellipseAnnotation);

  return ellipseAnnotation;
}

CQChartsImageAnnotation *
CQChartsView::
addImageAnnotation(const CQChartsPosition &pos, const QImage &image)
{
  CQChartsImageAnnotation *imageAnnotation = new CQChartsImageAnnotation(this, pos, image);

  addAnnotation(imageAnnotation);

  return imageAnnotation;
}

CQChartsImageAnnotation *
CQChartsView::
addImageAnnotation(const CQChartsRect &rect, const QImage &image)
{
  CQChartsImageAnnotation *imageAnnotation = new CQChartsImageAnnotation(this, rect, image);

  addAnnotation(imageAnnotation);

  return imageAnnotation;
}

CQChartsPointAnnotation *
CQChartsView::
addPointAnnotation(const CQChartsPosition &pos, const CQChartsSymbol &type)
{
  CQChartsPointAnnotation *pointAnnotation = new CQChartsPointAnnotation(this, pos, type);

  addAnnotation(pointAnnotation);

  return pointAnnotation;
}

CQChartsPolygonAnnotation *
CQChartsView::
addPolygonAnnotation(const CQChartsPolygon &points)
{
  CQChartsPolygonAnnotation *polyAnnotation = new CQChartsPolygonAnnotation(this, points);

  addAnnotation(polyAnnotation);

  return polyAnnotation;
}

CQChartsPolylineAnnotation *
CQChartsView::
addPolylineAnnotation(const CQChartsPolygon &points)
{
  CQChartsPolylineAnnotation *polyAnnotation = new CQChartsPolylineAnnotation(this, points);

  addAnnotation(polyAnnotation);

  return polyAnnotation;
}

CQChartsRectangleAnnotation *
CQChartsView::
addRectangleAnnotation(const CQChartsRect &rect)
{
  CQChartsRectangleAnnotation *rectangleAnnotation = new CQChartsRectangleAnnotation(this, rect);

  addAnnotation(rectangleAnnotation);

  return rectangleAnnotation;
}

CQChartsTextAnnotation *
CQChartsView::
addTextAnnotation(const CQChartsPosition &pos, const QString &text)
{
  CQChartsTextAnnotation *textAnnotation = new CQChartsTextAnnotation(this, pos, text);

  addAnnotation(textAnnotation);

  return textAnnotation;
}

CQChartsTextAnnotation *
CQChartsView::
addTextAnnotation(const CQChartsRect &rect, const QString &text)
{
  CQChartsTextAnnotation *textAnnotation = new CQChartsTextAnnotation(this, rect, text);

  addAnnotation(textAnnotation);

  return textAnnotation;
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
addPlot(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox)
{
  CQChartsGeom::BBox bbox1 = bbox;

  if (! bbox1.isSet())
    bbox1 = CQChartsGeom::BBox(0, 0, viewportRange(), viewportRange());

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

  //---

  if (currentPlotInd_ < 0)
    setCurrentPlot(plot);

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

  if (pos < 0)
    return;

  if (pos > 0)
    std::swap(plots_[pos - 1], plots_[pos]);

  update();

  emit plotsReordered();
}

void
CQChartsView::
lowerPlot(CQChartsPlot *plot)
{
  int pos = plotPos(plot);

  if (pos < 0)
    return;

  if (pos < int(plots_.size()) - 1)
    std::swap(plots_[pos + 1], plots_[pos]);

  update();

  emit plotsReordered();
}

int
CQChartsView::
plotPos(CQChartsPlot *plot) const
{
  for (std::size_t i = 0; i < plots_.size(); ++i) {
    if (plots_[i] == plot)
      return i;
  }

  return -1;
}

void
CQChartsView::
removePlot(CQChartsPlot *plot)
{
  bool isCurrent = (plot == currentPlot(/*remap*/false));

  // build new list of plots without plot and check for match
  Plots plots;

  bool found = false;

  for (auto &plot1 : plots_) {
    if (plot1 != plot)
      plots.push_back(plot1);
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

  std::swap(plots, plots_);

  if (mousePlot() == plot)
    mouseData_.reset();

  delete plot;

  //---

  emit plotRemoved(id);
  emit plotsChanged();

  //---

  if (isCurrent) {
    if (plots_.empty())
      setCurrentPlot(nullptr);
    else
      setCurrentPlot(plots_[0]);
  }
}

void
CQChartsView::
removeAllPlots()
{
  for (auto &plot : plots_)
    propertyModel()->removeProperties(plot->id(), plot);

  for (auto &plot : plots_)
    delete plot;

  plots_.clear();

  mouseData_.reset();

  setCurrentPlot(nullptr);

  emit allPlotsRemoved();
  emit plotsChanged();
}

CQChartsPlot *
CQChartsView::
getPlot(const QString &id) const
{
  for (const auto &plot : plots_) {
    if (plot->id() == id)
      return plot;
  }

  return nullptr;
}

void
CQChartsView::
getPlots(Plots &plots) const
{
  for (const auto &plot : plots_)
    plots.push_back(plot);
}

//---

void
CQChartsView::
plotModelChanged()
{
  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(sender());

  if (plot == currentPlot(/*remap*/false))
    emit currentPlotChanged();
}

void
CQChartsView::
plotConnectDataChangedSlot()
{
  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(sender());

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

  for (auto &plot : plots_) {
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

    //plot->setBackground    (true);
    //plot->setDataBackground(true);

    plot->resetKeyItems();
  }

  emit connectDataChanged();
}

void
CQChartsView::
resetConnections(bool notify)
{
  for (const auto &plot : plots_)
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

  CQChartsPlot *firstPlot = plot(0)->firstPlot();

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

    resetConnections(/*notify*/false);
  }

#if 0
  for (std::size_t i = 0; i < plots.size(); ++i) {
    CQChartsPlot *plot = plots[i];

    plot->syncRange();
  }
#endif

  CQChartsPlot *rootPlot = plots[0]->firstPlot();

  for (std::size_t i = 0; i < plots.size(); ++i) {
    CQChartsPlot *plot = plots[i];

    plot->setOverlay(true, /*notify*/false);

    if (i > 0) {
      CQChartsPlot *prevPlot = plots[i - 1];

      plot    ->setPrevPlot(prevPlot);
      prevPlot->setNextPlot(plot);
    }
  }

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

    //---

    CQChartsAxis *xaxis = plot->xAxis();
    CQChartsAxis *yaxis = plot->yAxis();

    if (xaxis)
      xaxis->setVisible(false);

    if (yaxis)
      yaxis->setVisible(false);

    //CQChartsTitle *title = plot->title();

    //if (title)
    //  title->setVisible(false);

    //plot->setBackground    (false);
    //plot->setDataBackground(false);
  }

  //---

  firstPlot->updateOverlay();

  firstPlot->updateObjs();
}

void
CQChartsView::
initX1X2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset)
{
  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetConnections(/*notify*/false);
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

    //if (plot2->key())
    //  plot2->key()->setVisible(false);

    //if (plot2->title())
    //  plot2->title()->setVisible(false);

//  plot2->setBackground    (false);
//  plot2->setDataBackground(false);
  }

  //---

  plot1->updateOverlay();

  emit connectDataChanged();
}

void
CQChartsView::
initY1Y2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset)
{
  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetConnections(/*notify*/false);
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

    //if (plot2->key())
    //  plot2->key()->setVisible(false);

    //if (plot2->title())
    //  plot2->title()->setVisible(false);

//  plot2->setBackground    (false);
//  plot2->setDataBackground(false);
  }

  //---

  plot1->updateOverlay();

  emit connectDataChanged();
}

//------

void
CQChartsView::
placePlots(const Plots &plots, bool vertical, bool horizontal,
           int rows, int columns, bool reset)
{
  if (reset) {
    if (isScrolled())
      setScrolled(false);

    resetConnections(/*notify*/false);
  }

  //---

  int np = plots.size();

  if (np <= 0)
    return;

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
      CQChartsPlot *plot = plots[i];

      CQChartsGeom::BBox bbox(0, 0, vr, vr);

      plot->setViewBBox(bbox);
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
        if (i >= int(plots.size()))
          break;

        CQChartsPlot *plot = plots[i];

        CQChartsGeom::BBox bbox(x, y - dy, x + dx, y);

        plot->setViewBBox(bbox);

        x += dx;
      }

      y -= dy;
    }
  }
}

//------

QColor
CQChartsView::
interpPaletteColor(int i, int n, bool scale) const
{
  return charts()->interpPaletteColor(i, n, scale);
}

QColor
CQChartsView::
interpPaletteColor(double r, bool scale) const
{
  return charts()->interpPaletteColor(r, scale);
}

QColor
CQChartsView::
interpPaletteColor(const ColorInd &ind, bool scale) const
{
  return charts()->interpPaletteColor(ind, scale);
}

QColor
CQChartsView::
interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv, bool scale) const
{
  return charts()->interpGroupPaletteColor(ig, iv, scale);
}

QColor
CQChartsView::
interpGroupPaletteColor(int ig, int ng, int i, int n, bool scale) const
{
  return charts()->interpGroupPaletteColor(ig, ng, i, n, scale);
}

QColor
CQChartsView::
interpGroupPaletteColor(int ig, int ng, double r, bool scale) const
{
  return charts()->interpGroupPaletteColor(ig, ng, r, scale);
}

QColor
CQChartsView::
interpThemeColor(const ColorInd &ind) const
{
  return (ind.isInt ? interpThemeColor(ind.i, ind.n) : interpThemeColor(ind.r));
}

QColor
CQChartsView::
interpThemeColor(int i, int n) const
{
  return charts()->interpThemeColor(i, n);
}

QColor
CQChartsView::
interpThemeColor(double r) const
{
  return charts()->interpThemeColor(r);
}

QColor
CQChartsView::
interpColor(const CQChartsColor &c, const ColorInd &ind) const
{
  return (ind.isInt ? interpColor(c, ind.i, ind.n) : interpColor(c, ind.r));
}

QColor
CQChartsView::
interpColor(const CQChartsColor &c, int i, int n) const
{
  if (defaultPalette_ != "") {
    CQChartsColor c1 = charts()->adjustDefaultPalette(c, defaultPalette_);

    return charts()->interpColor(c1, i, n);
  }

  return charts()->interpColor(c, i, n);
}

QColor
CQChartsView::
interpColor(const CQChartsColor &c, double r) const
{
  if (defaultPalette_ != "") {
    CQChartsColor c1 = charts()->adjustDefaultPalette(c, defaultPalette_);

    return charts()->interpColor(c1, r);
  }

  return charts()->interpColor(c, r);
}

//------

void
CQChartsView::
mousePressEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

  mouseData_.reset();

  mouseData_.pressPoint = adjustMousePos(me->pos());
  mouseData_.button     = me->button();
  mouseData_.pressed    = true;
  mouseData_.movePoint  = mouseData_.pressPoint;
  mouseData_.selMod     = modifiersToSelMod(me->modifiers());
  mouseData_.clickMod   = modifiersToClickMod(me->modifiers());

  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(QPointF(mousePressPoint())));

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if (mouseButton() == Qt::LeftButton) {
    if      (mode() == Mode::SELECT) {
      selectMousePress();
    }
    else if (mode() == Mode::ZOOM) {
      zoomMousePress();
    }
    else if (mode() == Mode::PAN) {
    }
    else if (mode() == Mode::PROBE) {
    }
    else if (mode() == Mode::QUERY) {
    }
    else if (mode() == Mode::EDIT) {
      (void) editMousePress();
    }
  }
  else if (mouseButton() == Qt::MiddleButton) {
  }
  else if (mouseButton() == Qt::RightButton) {
    mouseData_.pressed    = false;
    mouseData_.pressPoint = me->globalPos();

    showMenu(me->pos());

    return;
  }
}

void
CQChartsView::
mouseMoveEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

  mouseData_.oldMovePoint = mouseMovePoint();
  mouseData_.movePoint    = adjustMousePos(me->pos());

  // select mode and move (not pressed) - update plot positions
  if (mode() == Mode::SELECT && ! mousePressed()) {
    selectMouseMotion();
    return;
  }

  //---

  if (mode() == Mode::ZOOM) {
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
    if (mode() == Mode::EDIT)
      editMouseMotion();

    return;
  }

  // get plots at point
  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(QPointF(mouseMovePoint())));

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if      (mouseButton() == Qt::LeftButton) {
    // select plot object
    if      (mode() == Mode::SELECT) {
      selectMouseMove();

      searchPos_ = mouseMovePoint();
    }
    // draw zoom rectangle
    else if (mode() == Mode::ZOOM) {
      zoomMouseMove();
    }
    else if (mode() == Mode::PAN) {
      panMouseMove();
    }
    else if (mode() == Mode::EDIT) {
      (void) editMouseMove();
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

  mouseData_.oldMovePoint = mouseMovePoint();
  mouseData_.movePoint    = adjustMousePos(me->pos());

  CQChartsScopeGuard resetMouseData([&]() { mouseData_.reset(); });

  //CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(QPointF(mouseMovePoint())));

  if      (mouseButton() == Qt::LeftButton) {
    if      (mode() == Mode::SELECT) {
      if (! mousePressed())
        return;

      selectMouseRelease();
    }
    else if (mode() == Mode::ZOOM) {
      if (! mousePressed())
        return;

      zoomMouseRelease();
    }
    else if (mode() == Mode::PAN) {
    }
    else if (mode() == Mode::PROBE) {
    }
    else if (mode() == Mode::QUERY) {
    }
    else if (mode() == Mode::EDIT) {
      if (! mousePressed())
        return;

      editMouseRelease();
    }
  }
  else if (mouseButton() == Qt::MiddleButton) {
  }
  else if (mouseButton() == Qt::RightButton) {
  }
}

QPoint
CQChartsView::
adjustMousePos(const QPoint &pos) const
{
  return QPoint(pos.x() + sizeData_.xpos, pos.y() + sizeData_.ypos);
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

    if      (mode() == Mode::ZOOM) {
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

  QPoint gpos = QCursor::pos();

  QPointF pos = mapFromGlobal(gpos);

  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(pos));

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
  QPointF p = mouseData_.pressPoint;

  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(p));

  //---

  std::vector<CQChartsAnnotation *> selAnnotations;

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w) ||
        (annotation->editHandles()->inside(w) != CQChartsResizeSide::NONE))
      selAnnotations.push_back(annotation);
  }

  // start drag on already selected annotation handle
  if (! selAnnotations.empty() && selAnnotations[0]->isSelected()) {
    CQChartsAnnotation *annotation = selAnnotations[0];

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

  bool rc = processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &p) {
    return plot->editMousePress(p, /*inside*/false);
  }, p);

  if (rc)
    return true;

  rc = processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &p) {
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
  QPointF p = mouseMovePoint();

  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(p));

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

  processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &pos) {
    bool current = (plot == mousePlot());

    return plot->editMouseMove(pos, current);
  }, mouseMovePoint());

  return true;
}

void
CQChartsView::
editMouseMotion()
{
  QPointF p = mouseMovePoint();

  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(p));

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

  processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &pos) {
    plot->editMouseRelease(pos); return false;
  }, mouseMovePoint());
}

//------

void
CQChartsView::
showProbeLines(const QPointF &p)
{
  auto addVerticalProbeBand = [&](int &ind, CQChartsPlot *plot, const QString &tip,
                                  double px, double py1, double py2) -> void {
    while (ind >= int(probeBands_.size())) {
      CQChartsProbeBand *probeBand = new CQChartsProbeBand(this);

      probeBands_.push_back(probeBand);
    }

    probeBands_[ind]->showVertical(plot, tip, px, py1, py2);

    ++ind;
  };

  auto addHorizontalProbeBand = [&](int &ind, CQChartsPlot *plot, const QString &tip,
                                    double px1, double px2, double py) -> void {
    while (ind >= int(probeBands_.size())) {
      CQChartsProbeBand *probeBand = new CQChartsProbeBand(this);

      probeBands_.push_back(probeBand);
    }

    probeBands_[ind]->showHorizontal(plot, tip, px1, px2, py);

    ++ind;
  };

  //int px = p.x();

  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(p));

  Plots         plots;
  CQChartsPlot *plot;

  plotsAt(w, plots, plot);

  int probeInd = 0;

  for (auto &plot : plots) {
    CQChartsGeom::Point w = plot->pixelToWindow(CQChartsGeom::Point(p));

    //---

    CQChartsPlot::ProbeData probeData;

    probeData.p = w;

    if (! plot->probe(probeData))
      continue;

    if (probeData.xvals.empty()) probeData.xvals.emplace_back(w.x);
    if (probeData.yvals.empty()) probeData.yvals.emplace_back(w.y);

    CQChartsGeom::BBox dataRange = plot->calcDataRange();

    if      (probeData.both) {
      // add probe lines from xmin to probed x values and from ymin to probed y values
      CQChartsGeom::Point px1 =
        plot->windowToPixel(CQChartsGeom::Point(dataRange.getXMin(), probeData.p.y));
      CQChartsGeom::Point py1 =
        plot->windowToPixel(CQChartsGeom::Point(probeData.p.x, dataRange.getYMin()));

      int nx = probeData.xvals.size();
      int ny = probeData.yvals.size();

      int n = std::min(nx, ny);

      for (int i = 0; i < n; ++i) {
        const auto &xval = probeData.xvals[i];
        const auto &yval = probeData.yvals[i];

        CQChartsGeom::Point px2 =
          plot->windowToPixel(CQChartsGeom::Point(xval.value, probeData.p.y));
        CQChartsGeom::Point py2 =
          plot->windowToPixel(CQChartsGeom::Point(probeData.p.x, yval.value));

        QString tip = QString("%1, %2").
          arg(xval.label.length() ? xval.label : plot->xStr(xval.value)).
          arg(yval.label.length() ? yval.label : plot->yStr(yval.value));

        addVerticalProbeBand  (probeInd, plot, tip, py1.x, py1.y, py2.y);
        addHorizontalProbeBand(probeInd, plot, "" , px1.x, px2.x, px2.y);
      }
    }
    else if (probeData.direction == Qt::Vertical) {
      // add probe lines from ymin to probed y values
      CQChartsGeom::Point p1 =
        plot->windowToPixel(CQChartsGeom::Point(probeData.p.x, dataRange.getYMin()));

      for (const auto &yval : probeData.yvals) {
        CQChartsGeom::Point p2 =
          plot->windowToPixel(CQChartsGeom::Point(probeData.p.x, yval.value));

        QString tip = (yval.label.length() ? yval.label : plot->yStr(yval.value));

        addVerticalProbeBand(probeInd, plot, tip, p1.x, p1.y, p2.y);
      }
    }
    else {
      // add probe lines from xmin to probed x values
      CQChartsGeom::Point p1 =
        plot->windowToPixel(CQChartsGeom::Point(dataRange.getXMin(), probeData.p.y));

      for (const auto &xval : probeData.xvals) {
        CQChartsGeom::Point p2 =
          plot->windowToPixel(CQChartsGeom::Point(xval.value, probeData.p.y));

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
  if      (isPointSelectMode()) {
    selectPointPress();
  }
  else if (isRectSelectMode()) {
    startRegionBand(mousePressPoint());
  }
}

void
CQChartsView::
selectPointPress()
{
  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(QPointF(mousePressPoint())));

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
  std::vector<CQChartsAnnotation *> selAnnotations;

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w))
      selAnnotations.push_back(annotation);
  }

  for (const auto &selAnnotation : selAnnotations) {
    if (! selAnnotation->selectPress(w))
      continue;

    //---

    selectOneObj(selAnnotation);

    update();

    emit annotationPressed  (selAnnotation);
    emit annotationIdPressed(selAnnotation->id());

    return;
  }

  //---

  // select plot objects
  struct SelData {
    QPointF        pos;
    CQChartsSelMod selMod;

    SelData(const QPointF &pos, CQChartsSelMod selMod) :
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

  return;
}

void
CQChartsView::
selectMouseMove()
{
  if      (isPointSelectMode()) {
    if (key()) {
      CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(mouseMovePoint()));

      bool handled = key()->selectMove(w);

      if (handled)
        return;
    }

    //---

    processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &pos) {
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

    double dx = abs(mouseMovePoint().x() - mousePressPoint().x());
    double dy = abs(mouseMovePoint().y() - mousePressPoint().y());

    if (dx < 4 && dy < 4) {
      selectPointPress();

      if (mousePlot())
        mouseData_.plot->selectMouseRelease(mouseMovePoint());
    }
    else {
      processMouseDataPlots([&](CQChartsPlot *plot, const CQChartsSelMod &selMod) {
        CQChartsGeom::Point w1 =
          plot->pixelToWindow(CQChartsGeom::Point(mousePressPoint()));
        CQChartsGeom::Point w2 =
          plot->pixelToWindow(CQChartsGeom::Point(mouseMovePoint()));

        return plot->rectSelect(CQChartsGeom::BBox(w1, w2), selMod);
      }, mouseSelMod());
    }
  }
}

bool
CQChartsView::
isRectSelectMode() const
{
  if (selectMode_ != SelectMode::RECT)
    return false;

  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/false);

  if (currentPlot && ! currentPlot->type()->canRectSelect())
    return false;

  return true;
}

bool
CQChartsView::
isPointSelectMode() const
{
  if (selectMode_ == SelectMode::POINT)
    return true;

  if (selectMode_ == SelectMode::RECT) {
    CQChartsPlot *currentPlot = this->currentPlot(/*remap*/false);

    if (currentPlot && ! currentPlot->type()->canRectSelect())
      return true;
  }

  return false;
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
  for (auto &plot : plots_)
    objs.push_back(plot);

  for (auto &plot : plots_) {
    Objs objs1;

    plot->editObjs(objs1);

    for (auto &obj1 : objs1)
      objs.push_back(obj1);
  }
}

//------

void
CQChartsView::
updatePosText(const QPointF &pos)
{
  QString posStr;

  if (posTextType() == PosTextType::PLOT) {
    CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(pos));

    CQChartsPlot *currentPlot = this->currentPlot(/*remap*/false);

    PlotSet plots;

    if (currentPlot) {
      const CQChartsGeom::BBox &bbox = currentPlot->viewBBox();

      if ( bbox.inside(w))
        plots.insert(currentPlot);
    }

    if (plots.empty())
      basePlotsAt(w, plots);

    for (const auto &plot : plots) {
      CQChartsGeom::Point w = plot->pixelToWindow(CQChartsGeom::Point(pos));

      if (posStr.length())
        posStr += " ";

      posStr += plot->posStr(w);
    }
  }
  else if (posTextType() == PosTextType::VIEW) {
    CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(pos));

    posStr = QString("%1 %2").arg(w.x).arg(w.y);
  }
  else {
    posStr = QString("%1 %2").arg(pos.x()).arg(pos.y());
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
  mouseData_.movePoint = mouseMovePoint();

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
    CQChartsGeom::Point w1 =
      mousePlot()->pixelToWindow(CQChartsGeom::Point(QPointF(mousePressPoint())));
    CQChartsGeom::Point w2 =
      mousePlot()->pixelToWindow(CQChartsGeom::Point(QPointF(mouseMovePoint())));

    CQChartsGeom::BBox bbox(w1, w2);

    mouseData_.plot->zoomTo(bbox);
  }
}

//------

void
CQChartsView::
panMouseMove()
{
  if (mousePlot()) {
    CQChartsGeom::Point w1 =
      mousePlot()->pixelToWindow(CQChartsGeom::Point(QPointF(mouseData_.oldMovePoint)));
    CQChartsGeom::Point w2 =
      mousePlot()->pixelToWindow(CQChartsGeom::Point(QPointF(mouseMovePoint())));

    double dx = w1.x - w2.x;
    double dy = w1.y - w2.y;

    mouseData_.plot->pan(dx, dy);
  }
}

//------

void
CQChartsView::
startRegionBand(const QPoint &pos)
{
  if (! regionBand_)
    regionBand_ = new QRubberBand(QRubberBand::Rectangle, this);

  regionBand_->setGeometry(QRect(pos, QSize()));
  regionBand_->show();
}

void
CQChartsView::
updateRegionBand(CQChartsPlot *plot, const QPoint &pressPoint, const QPoint &movePoint)
{
  updateRegionBand(pressPoint, movePoint);

  if (! plot->allowZoomX() || ! plot->allowZoomY()) {
    int x = regionBand_->x     ();
    int y = regionBand_->y     ();
    int w = regionBand_->width ();
    int h = regionBand_->height();

    CQChartsGeom::BBox pixelRect = plot->calcPlotPixelRect();

    if (! plot->allowZoomX()) {
      x = pixelRect.getXMin();
      w = pixelRect.getWidth();
    }

    if (! plot->allowZoomY()) {
      y = pixelRect.getYMin();
      h = pixelRect.getHeight();
    }

    regionBand_->setGeometry(QRect(x, y, w, h));
    regionBand_->show();
  }
}

void
CQChartsView::
updateRegionBand(const QPoint &pressPoint, const QPoint &movePoint)
{
  double x = std::min(pressPoint.x(), movePoint.x());
  double y = std::min(pressPoint.y(), movePoint.y());
  double w = std::abs(movePoint.x() - pressPoint.x());
  double h = std::abs(movePoint.y() - pressPoint.y());

  regionBand_->setGeometry(QRect(x, y, w, h));
}

void
CQChartsView::
endRegionBand()
{
  if (regionBand_)
    regionBand_->hide();
}

//------

void
CQChartsView::
updateSelText()
{
  CQChartsPlot::Objs objs;

  selectedObjs(objs);

  for (auto &plot : plots_) {
    if (plot->isSelected())
      objs.push_back(plot);
  }

  //---

  int num = objs.size();

  for (auto &plot : plots_) {
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
  for (auto &plot : plots_) {
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

  for (auto &plot : plots_) {
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

  image_->fill(QColor(0,0,0,0));

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
      sizeData_.hbar->resize(w - (showVBar ? vw : 0), hh);

      sizeData_.hbar->move(0, h - hh);

      sizeData_.hbar->setRange(0, sizeData_.width - w);
      sizeData_.hbar->setPageStep(w);
    }

    if (showVBar) {
      sizeData_.vbar->resize(vw, h - (showHBar ? hh : 0));

      sizeData_.vbar->move(w - vw, 0);

      sizeData_.vbar->setRange(0, sizeData_.height - h);
      sizeData_.vbar->setPageStep(h);
    }

    // needed if size not changed ?
    //doResize(sizeData_.width, sizeData_.height);

    update();
  }
}

void
CQChartsView::
doResize(int w, int h)
{
  lockPainter(true);

  //---

  for (const auto &plot : plots_)
    plot->preResize();

  //---

  prect_ = CQChartsGeom::BBox(0, 0, w, h);

  if (prect().getHeight() > 0)
    aspect_ = (1.0*prect().getWidth())/prect().getHeight();
  else
    aspect_ = 1.0;

  displayRange_->setPixelRange(prect_.getXMin(), prect_.getYMin(),
                               prect_.getXMax(), prect_.getYMax());

  //---

  lockPainter(false);

  //---

  for (const auto &plot : plots_)
    plot->postResize();
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

  // fill background
  QBrush brush;

  setBrush(brush, true, interpBackgroundFillColor(ColorInd()),
           backgroundFillAlpha(), backgroundFillPattern());

  painter->fillRect(prect_.qrect(), brush);

  //---

  // draw specific plot
  if (plot) {
    plot->draw(painter);
  }
  // draw all plots
  else {
    for (const auto &plot : plots_) {
      if (plot->isVisible())
        plot->draw(painter);
    }

    //---

    QPainter *painter1 = objectsBuffer_->beginPaint(painter, rect());

    if (painter1) {
      CQChartsView *th = const_cast<CQChartsView *>(this);

      CQChartsViewPainter device(th, painter1);

      if (hasAnnotations()) {
        // draw annotations
        drawAnnotations(&device, CQChartsLayer::Type::ANNOTATION);
      }

      //--

      // draw view key
      drawKey(&device, CQChartsLayer::Type::FG_KEY);
    }

    objectsBuffer_->endPaint();

    //---

    painter1 = overlayBuffer_->beginPaint(painter, rect());

    if (painter1) {
      CQChartsView *th = const_cast<CQChartsView *>(this);

      CQChartsViewPainter device(th, painter1);

      if (hasAnnotations()) {
        // draw selected annotations
        drawAnnotations(&device, CQChartsLayer::Type::SELECTION);

        // draw annotations
        drawAnnotations(&device, CQChartsLayer::Type::MOUSE_OVER);
      }

      //--

      // draw view key
      drawKey(&device, CQChartsLayer::Type::SELECTION);

      // draw view key
      drawKey(&device, CQChartsLayer::Type::MOUSE_OVER);
    }

    overlayBuffer_->endPaint();
  }
}

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
        if (device->type() != CQChartsPaintDevice::Type::SCRIPT) {
          CQChartsViewPlotPainter *painter = dynamic_cast<CQChartsViewPlotPainter *>(device);

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
      if (device->type() != CQChartsPaintDevice::Type::SCRIPT) {
        CQChartsViewPlotPainter *painter = dynamic_cast<CQChartsViewPlotPainter *>(device);

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
setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha,
       const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash) const
{
  double width = CQChartsUtil::limitLineWidth(lengthPixelWidth(strokeWidth));

  CQChartsUtil::setPen(pen, stroked, strokeColor, strokeAlpha, width, strokeDash);
}

void
CQChartsView::
setBrush(QBrush &brush, bool filled, const QColor &fillColor, double fillAlpha,
         const CQChartsFillPattern &pattern) const
{
  CQChartsUtil::setBrush(brush, filled, fillColor, fillAlpha, pattern);
}

//------

void
CQChartsView::
updateObjPenBrushState(const CQChartsObj *obj, QPen &pen, QBrush &brush, DrawType drawType) const
{
  updateObjPenBrushState(obj, ColorInd(), pen, brush, drawType);
}

void
CQChartsView::
updateObjPenBrushState(const CQChartsObj *obj, const ColorInd &ic,
                       QPen &pen, QBrush &brush, DrawType drawType) const
{
  if (! isBufferLayers()) {
    // inside and selected
    if      (obj->isInside() && obj->isSelected()) {
      updateSelectedObjPenBrushState(ic, pen, brush, drawType);
      updateInsideObjPenBrushState  (ic, pen, brush, /*outline*/false, drawType);
    }
    // inside
    else if (obj->isInside()) {
      updateInsideObjPenBrushState(ic, pen, brush, /*outline*/true, drawType);
    }
    // selected
    else if (obj->isSelected()) {
      updateSelectedObjPenBrushState(ic, pen, brush, drawType);
    }
  }
  else {
    // inside
    if      (drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
      if (obj->isInside())
        updateInsideObjPenBrushState(ic, pen, brush, /*outline*/true, drawType);
    }
    // selected
    else if (drawLayerType() == CQChartsLayer::Type::SELECTION) {
      if (obj->isSelected())
        updateSelectedObjPenBrushState(ic, pen, brush, drawType);
    }
  }
}

void
CQChartsView::
updateInsideObjPenBrushState(const ColorInd &ic, QPen &pen, QBrush &brush,
                             bool outline, DrawType drawType) const
{
  // fill and stroke
  if (drawType != DrawType::LINE) {
    // outline box, symbol
    if (insideMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor opc;
      double alpha = 1.0;

      if (pen.style() != Qt::NoPen) {
        QColor pc = pen.color();

        if (isInsideStroked())
          opc = interpInsideStrokeColor(ic);
        else
          opc = CQChartsUtil::invColor(pc);

        alpha = pc.alphaF();
      }
      else {
        QColor bc = brush.color();

        if (isInsideStroked())
          opc = interpInsideStrokeColor(ic);
        else
          opc = CQChartsUtil::invColor(bc);
      }

      setPen(pen, true, opc, alpha, insideStrokeWidth(), insideStrokeDash());

      if (outline)
        setBrush(brush, false);
    }
    // fill box, symbol
    else {
      QColor bc = brush.color();

      QColor ibc;

      if (isInsideFilled())
        ibc = interpInsideFillColor(ic);
      else
        ibc = insideColor(bc);

      double alpha = 1.0;

      if (isBufferLayers())
        alpha = insideFillAlpha()*bc.alphaF();
      else
        alpha = bc.alphaF();

      setBrush(brush, true, ibc, alpha, insideFillPattern());
    }
  }
  // just stroke
  else {
    QColor pc = pen.color();

    QColor opc;

    if (isInsideStroked())
      opc = interpInsideStrokeColor(ic);
    else
      opc = CQChartsUtil::invColor(pc);

    setPen(pen, true, opc, pc.alphaF(), insideStrokeWidth(), insideStrokeDash());
  }
}

void
CQChartsView::
updateSelectedObjPenBrushState(const ColorInd &ic, QPen &pen, QBrush &brush,
                               DrawType drawType) const
{
  // fill and stroke
  if      (drawType != DrawType::LINE) {
    // outline box, symbol
    if (selectedMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor opc;
      double alpha = 1.0;

      if (pen.style() != Qt::NoPen) {
        QColor pc = pen.color();

        if (isSelectedStroked())
          opc = interpSelectedStrokeColor(ic);
        else
          opc = selectedColor(pc);

        alpha = pc.alphaF();
      }
      else {
        QColor bc = brush.color();

        if (isSelectedStroked())
          opc = interpSelectedStrokeColor(ic);
        else
          opc = CQChartsUtil::invColor(bc);
      }

      setPen(pen, true, opc, alpha, selectedStrokeWidth(), selectedStrokeDash());

      setBrush(brush, false);
    }
    // fill box, symbol
    else {
      QColor bc = brush.color();

      QColor ibc;

      if (isSelectedFilled())
        ibc = interpSelectedFillColor(ic);
      else
        ibc = selectedColor(bc);

      double alpha = 1.0;

      if (isBufferLayers())
        alpha = selectedFillAlpha()*bc.alphaF();
      else
        alpha = bc.alphaF();

      setBrush(brush, true, ibc, alpha, selectedFillPattern());
    }
  }
  // just stroke
  else if (pen.style() != Qt::NoPen) {
    QColor pc = pen.color();

    QColor opc;

    if (isSelectedStroked())
      opc = interpSelectedStrokeColor(ic);
    else
      opc = CQChartsUtil::invColor(pc);

    setPen(pen, true, opc, pc.alphaF(), selectedStrokeWidth(), selectedStrokeDash());
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
  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(searchPos_));

  plotsAt(w, mouseData_.plots, mouseData_.plot, /*clear*/true, /*first*/true);

  //---

  setStatusText("");

  bool handled = false;

  processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &pos) {
    CQChartsGeom::Point w = plot->pixelToWindow(CQChartsGeom::Point(pos));

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
showMenu(const QPoint &p)
{
  delete popupMenu_;

  popupMenu_ = new QMenu(this);

  //---

  // get all plots
  Plots allPlots;

  this->plots(allPlots);

  //---

  // get base plots
  PlotSet basePlots;

  this->basePlots(basePlots);

  //---

  // get current plot
  CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(p));

  Plots         plots;
  CQChartsPlot* plot { nullptr };

  plotsAt(w, plots, plot);

  if (plot)
    setCurrentPlot(plot);

  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/false);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsPlotType *plotType = (currentPlot ? currentPlot->type() : nullptr);

  //---

  auto addSubMenu = [](QMenu *menu, const QString &name) {
    QMenu *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto createActionGroup = [](QMenu *menu) {
    return new QActionGroup(menu);
  };

  auto addAction = [&](QMenu *menu, const QString &name, const char *slotName) {
    QAction *action = new QAction(name, menu);

    connect(action, SIGNAL(triggered()), this, slotName);

    menu->addAction(action);

    return action;
  };

  auto addCheckAction = [&](QMenu *menu, const QString &name, bool checked, const char *slotName) {
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    connect(action, SIGNAL(triggered(bool)), this, slotName);

    menu->addAction(action);

    return action;
  };

  auto addGroupCheckAction = [&](QActionGroup *group, const QString &name, bool checked,
                                 const char *slotName) {
    QMenu *menu = qobject_cast<QMenu *>(group->parent());

    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    connect(action, SIGNAL(triggered()), this, slotName);

    group->addAction(action);

    return action;
  };

  //---

  addCheckAction(popupMenu_, "Show Table"   , isShowTable   (), SLOT(setShowTable(bool)));
  addCheckAction(popupMenu_, "Show Settings", isShowSettings(), SLOT(setShowSettings(bool)));

  //---

  popupMenu_->addSeparator();

  //---

  QMenu *modeMenu = addSubMenu(popupMenu_, "Mode");

  QActionGroup *modeActionGroup = createActionGroup(modeMenu);

  addGroupCheckAction(modeActionGroup, "Select", mode() == Mode::SELECT, SLOT(selectModeSlot()));
  addGroupCheckAction(modeActionGroup, "Zoom"  , mode() == Mode::ZOOM  , SLOT(zoomModeSlot()));
  addGroupCheckAction(modeActionGroup, "Pan"   , mode() == Mode::PAN   , SLOT(panModeSlot()));

  if (plotType && plotType->canProbe())
    addGroupCheckAction(modeActionGroup, "Probe", mode() == Mode::PROBE, SLOT(probeModeSlot()));

  addGroupCheckAction(modeActionGroup, "Query", mode() == Mode::QUERY, SLOT(queryModeSlot()));
  addGroupCheckAction(modeActionGroup, "Edit" , mode() == Mode::EDIT , SLOT(editModeSlot()));

  modeActionGroup->setExclusive(true);

  modeMenu->addActions(modeActionGroup->actions());

  //---

  popupMenu_->addSeparator();

  //---

  CQChartsPlot::Objs objs;

  allSelectedObjs(objs);

  if (objs.size() == 1) {
    CQChartsAnnotation *annotation = qobject_cast<CQChartsAnnotation *>(objs[0]);
    CQChartsAxis       *axis       = qobject_cast<CQChartsAxis       *>(objs[0]);
    CQChartsKey        *key        = qobject_cast<CQChartsKey        *>(objs[0]);
    CQChartsTitle      *title      = qobject_cast<CQChartsTitle      *>(objs[0]);

    if (annotation || axis || key || title)
      addAction(popupMenu_, "Edit", SLOT(editObjectSlot()));
  }

  //---

  addCheckAction(popupMenu_, "Auto Resize", isAutoSize(), SLOT(setAutoSize(bool)));

  if (! isAutoSize())
    addAction(popupMenu_, "Resize to View", SLOT(resizeToView()));

  //---

  if (basePlots.size() > 1) {
    if (! isScrolled())
      addAction(popupMenu_, "Maximize", SLOT(maximizePlotsSlot()));
    else
      addAction(popupMenu_, "Restore", SLOT(restorePlotsSlot()));
  }

  //---

  if (key() && basePlots.size() > 1) {
    QMenu *viewKeyMenu = addSubMenu(popupMenu_, "View Key");

    addCheckAction(viewKeyMenu, "Visible", key()->isVisible(), SLOT(viewKeyVisibleSlot(bool)));

    //---

    using KeyLocationActionMap = std::map<CQChartsKeyLocation::Type, QAction *>;

    KeyLocationActionMap keyLocationActionMap;

    QMenu *keyLocationMenu = addSubMenu(viewKeyMenu, "Location");

    QActionGroup *keyLocationActionGroup = createActionGroup(keyLocationMenu);

    auto addKeyLocationGroupAction =
     [&](const QString &label, const CQChartsKeyLocation::Type &location) {
      QAction *action = new QAction(label, keyLocationMenu);

      action->setCheckable(true);

      keyLocationActionMap[location] = action;

      keyLocationActionGroup->addAction(action);

      return action;
    };

    addKeyLocationGroupAction("Top Left"         , CQChartsKeyLocation::Type::TOP_LEFT     );
    addKeyLocationGroupAction("Top Center"       , CQChartsKeyLocation::Type::TOP_CENTER   );
    addKeyLocationGroupAction("Top Right"        , CQChartsKeyLocation::Type::TOP_RIGHT    );
    addKeyLocationGroupAction("Center Left"      , CQChartsKeyLocation::Type::CENTER_LEFT  );
    addKeyLocationGroupAction("Center Center"    , CQChartsKeyLocation::Type::CENTER_CENTER);
    addKeyLocationGroupAction("Center Right"     , CQChartsKeyLocation::Type::CENTER_RIGHT );
    addKeyLocationGroupAction("Bottom Left"      , CQChartsKeyLocation::Type::BOTTOM_LEFT  );
    addKeyLocationGroupAction("Bottom Center"    , CQChartsKeyLocation::Type::BOTTOM_CENTER);
    addKeyLocationGroupAction("Bottom Right"     , CQChartsKeyLocation::Type::BOTTOM_RIGHT );
    addKeyLocationGroupAction("Absolute Position", CQChartsKeyLocation::Type::ABS_POSITION );
    addKeyLocationGroupAction("Absolute Rect"    , CQChartsKeyLocation::Type::ABS_RECT     );

    keyLocationActionGroup->setExclusive(true);

    CQChartsKeyLocation::Type location = key()->location().type();

    keyLocationActionMap[location]->setChecked(true);

    connect(keyLocationActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(viewKeyPositionSlot(QAction *)));

    keyLocationMenu->addActions(keyLocationActionGroup->actions());
  }

  //---

  if (allPlots.size() == 1) {
    addCheckAction(popupMenu_, "X Overview", window()->isXRangeMap(), SLOT(xRangeMapSlot(bool)));
    addCheckAction(popupMenu_, "Y Overview", window()->isYRangeMap(), SLOT(yRangeMapSlot(bool)));
  }

  //---

  // Add plots
  if (allPlots.size() > 1) {
    QMenu *plotsMenu = addSubMenu(popupMenu_, "Plots");

    QActionGroup *plotsGroup = createActionGroup(plotsMenu);

    for (const auto &plot : allPlots) {
      int ind = plotInd(plot);

      QAction *plotAction =
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
    QMenu *plotKeyMenu = addSubMenu(popupMenu_, "Plot Key");

    //---

    auto addKeyCheckAction = [&](const QString &label, bool checked, const char *slot) {
      QAction *action = new QAction(label, plotKeyMenu);

      action->setCheckable(true);
      action->setChecked(checked);

      connect(action, SIGNAL(triggered(bool)), this, slot);

      plotKeyMenu->addAction(action);

      return action;
    };

    //---

    CQChartsPlotKey *plotKey = (basePlot ? basePlot->key() : nullptr);

    bool visibleChecked = (plotKey && plotKey->isVisible());

    QAction *keyVisibleAction =
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

    QMenu *keyLocationMenu = addSubMenu(plotKeyMenu, "Location");

    QActionGroup *keyLocationActionGroup = createActionGroup(keyLocationMenu);

    auto addKeyLocationGroupAction =
     [&](const QString &label, const CQChartsKeyLocation::Type &location) {
      QAction *action = new QAction(label, keyLocationMenu);

      action->setCheckable(true);

      keyLocationActionMap[location] = action;

      keyLocationActionGroup->addAction(action);

      return action;
    };

    addKeyLocationGroupAction("Top Left"         , CQChartsKeyLocation::Type::TOP_LEFT     );
    addKeyLocationGroupAction("Top Center"       , CQChartsKeyLocation::Type::TOP_CENTER   );
    addKeyLocationGroupAction("Top Right"        , CQChartsKeyLocation::Type::TOP_RIGHT    );
    addKeyLocationGroupAction("Center Left"      , CQChartsKeyLocation::Type::CENTER_LEFT  );
    addKeyLocationGroupAction("Center Center"    , CQChartsKeyLocation::Type::CENTER_CENTER);
    addKeyLocationGroupAction("Center Right"     , CQChartsKeyLocation::Type::CENTER_RIGHT );
    addKeyLocationGroupAction("Bottom Left"      , CQChartsKeyLocation::Type::BOTTOM_LEFT  );
    addKeyLocationGroupAction("Bottom Center"    , CQChartsKeyLocation::Type::BOTTOM_CENTER);
    addKeyLocationGroupAction("Bottom Right"     , CQChartsKeyLocation::Type::BOTTOM_RIGHT );
    addKeyLocationGroupAction("Absolute Position", CQChartsKeyLocation::Type::ABS_POSITION );
    addKeyLocationGroupAction("Absolute Rect"    , CQChartsKeyLocation::Type::ABS_RECT     );
    addKeyLocationGroupAction("Auto"             , CQChartsKeyLocation::Type::AUTO         );

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
    CQChartsAxis *xAxis = (basePlot ? basePlot->xAxis() : nullptr);

    QMenu *xAxisMenu = addSubMenu(popupMenu_, "X Axis");

    //---

    QAction *xAxisVisibleAction =
      addCheckAction(xAxisMenu, "Visible", false, SLOT(xAxisVisibleSlot(bool)));

    if (xAxis)
      xAxisVisibleAction->setChecked(xAxis->isVisible());

    //---

    QAction *xAxisGridAction = addCheckAction(xAxisMenu, "Grid", false, SLOT(xAxisGridSlot(bool)));

    if (xAxis)
      xAxisGridAction->setChecked(xAxis->isMajorGridLinesDisplayed());

    //---

    AxisSideActionMap xAxisSideActionMap;

    QMenu *xAxisSideMenu = addSubMenu(xAxisMenu, "Side");

    QActionGroup *xAxisSideGroup = createActionGroup(xAxisMenu);

    auto addXAxisSideGroupAction = [&](const QString &label, const CQChartsAxisSide &side) {
      QAction *action = new QAction(label, xAxisSideMenu);

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
    CQChartsAxis *yAxis = (basePlot ? basePlot->yAxis() : nullptr);

    QMenu *yAxisMenu = addSubMenu(popupMenu_, "Y Axis");

    //---

    QAction *yAxisVisibleAction =
      addCheckAction(yAxisMenu, "Visible", false, SLOT(yAxisVisibleSlot(bool)));

    if (yAxis)
      yAxisVisibleAction->setChecked(yAxis->isVisible());

    //---

    QAction *yAxisGridAction = addCheckAction(yAxisMenu, "Grid", false, SLOT(yAxisGridSlot(bool)));

    if (yAxis)
      yAxisGridAction->setChecked(yAxis->isMajorGridLinesDisplayed());

    //---

    AxisSideActionMap yAxisSideActionMap;

    QMenu *yAxisSideMenu = addSubMenu(yAxisMenu, "Side");

    QActionGroup *yAxisSideGroup = createActionGroup(yAxisMenu);

    auto addYAxisSideGroupAction = [&](const QString &label, const CQChartsAxisSide &side) {
      QAction *action = new QAction(label, yAxisSideMenu);

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
    CQChartsTitle *title = (basePlot ? basePlot->title() : nullptr);

    QMenu *titleMenu = addSubMenu(popupMenu_, "Title");

    QAction *titleVisibleAction =
      addCheckAction(titleMenu, "Visible", false, SLOT(titleVisibleSlot(bool)));

    if (title)
      titleVisibleAction->setChecked(title->isVisible());

    //---

    using TitleLocationActionMap = std::map<CQChartsTitleLocation::Type, QAction *>;

    TitleLocationActionMap titleLocationActionMap;

    QMenu *titleLocationMenu = addSubMenu(titleMenu, "Location");

    QActionGroup *titleLocationGroup = createActionGroup(titleMenu);

    auto addTitleLocationGroupAction =
     [&](const QString &label, const CQChartsTitleLocation::Type &location) {
      QAction *action = new QAction(label, titleLocationMenu);

      action->setCheckable(true);

      titleLocationActionMap[location] = action;

      titleLocationGroup->addAction(action);

      return action;
    };

    addTitleLocationGroupAction("Top"     , CQChartsTitleLocation::Type::TOP    );
    addTitleLocationGroupAction("Center"  , CQChartsTitleLocation::Type::CENTER );
    addTitleLocationGroupAction("Bottom"  , CQChartsTitleLocation::Type::BOTTOM );
    addTitleLocationGroupAction("Absolute", CQChartsTitleLocation::Type::ABS_POS);

    if (title)
      titleLocationActionMap[title->location().type()]->setChecked(true);

    connect(titleLocationGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(titleLocationSlot(QAction *)));

    titleLocationMenu->addActions(titleLocationGroup->actions());
  }

  //------

  QAction *invertXAction = addCheckAction(popupMenu_, "Invert X", false, SLOT(invertXSlot(bool)));
  QAction *invertYAction = addCheckAction(popupMenu_, "Invert Y", false, SLOT(invertYSlot(bool)));

  if (basePlot) {
    invertXAction->setChecked(basePlot->isInvertX());
    invertYAction->setChecked(basePlot->isInvertY());
  }

  //------

  addAction(popupMenu_, "Fit", SLOT(fitSlot()));

  //---

  QMenu *themeMenu = addSubMenu(popupMenu_, "Theme");

  QActionGroup *interfaceGroup = createActionGroup(themeMenu);
  QActionGroup *themeGroup     = createActionGroup(themeMenu);

  auto addInterfaceAction = [&](const QString &label, const char *slotName) {
    QAction *action = new QAction(label, themeMenu);

    action->setCheckable(true);

    interfaceGroup->addAction(action);

    connect(action, SIGNAL(triggered()), this, slotName);

    return action;
  };

  auto addThemeAction = [&](const QString &label, const char *slotName) {
    QAction *action = new QAction(label, themeMenu);

    action->setCheckable(true);

    themeGroup->addAction(action);

    connect(action, SIGNAL(triggered()), this, slotName);

    return action;
  };

  QAction *lightPaletteAction = addInterfaceAction("Light", SLOT(lightPaletteSlot()));
  QAction *darkPaletteAction  = addInterfaceAction("Dark" , SLOT(darkPaletteSlot()));

  lightPaletteAction->setChecked(! isDark());
  darkPaletteAction ->setChecked(  isDark());

  themeMenu->addActions(interfaceGroup->actions());

  //---

  QStringList themeNames;

  CQColorsMgrInst->getThemeNames(themeNames);

  for (const auto &themeName : themeNames) {
    CQColorsTheme *theme = CQColorsMgrInst->getNamedTheme(themeName) ;

    QAction *themeAction = addThemeAction(theme->desc(), SLOT(themeNameSlot()));

    themeAction->setData(theme->name());

    themeAction->setChecked(this->themeName().name() == theme->name());
  }

  themeMenu->addActions(themeGroup->actions());

  //---

  // add Menus for current plot

  if (currentPlot) {
    if (currentPlot->addMenuItems(popupMenu_))
      popupMenu_->addSeparator();
  }

  //---

  QMenu *printMenu = addSubMenu(popupMenu_, "Print");

  addAction(printMenu, "PNG", SLOT(printPNGSlot()));
  addAction(printMenu, "SVG", SLOT(printSVGSlot()));

  addAction(printMenu, "Script", SLOT(writeScriptSlot()));

  //---

  if (CQChartsEnv::getBool("CQ_CHARTS_DEBUG", true)) {
    QAction *showBoxesAction =
      addCheckAction(popupMenu_, "Show Boxes", false, SLOT(showBoxesSlot(bool)));

    if (basePlot)
      showBoxesAction->setChecked(basePlot->showBoxes());

    //---

    addCheckAction(popupMenu_, "Buffer Layers", isBufferLayers(), SLOT(bufferLayersSlot(bool)));
  }

  //---

  popupMenu_->popup(mapToGlobal(p));
}

//------

void
CQChartsView::
editObjectSlot()
{
  CQChartsPlot::Objs objs;

  allSelectedObjs(objs);

  CQChartsAnnotation *annotation = qobject_cast<CQChartsAnnotation *>(objs[0]);
  CQChartsAxis       *axis       = qobject_cast<CQChartsAxis       *>(objs[0]);
  CQChartsKey        *key        = qobject_cast<CQChartsKey        *>(objs[0]);
  CQChartsTitle      *title      = qobject_cast<CQChartsTitle      *>(objs[0]);

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
  CQChartsViewKey *viewKey = key();
  if (! viewKey) return;

  if      (action->text() == "Top Left"         )
    viewKey->setLocation(CQChartsKeyLocation::Type::TOP_LEFT     );
  else if (action->text() == "Top Center"       )
    viewKey->setLocation(CQChartsKeyLocation::Type::TOP_CENTER   );
  else if (action->text() == "Top Right"        )
    viewKey->setLocation(CQChartsKeyLocation::Type::TOP_RIGHT    );
  else if (action->text() == "Center Left"      )
    viewKey->setLocation(CQChartsKeyLocation::Type::CENTER_LEFT  );
  else if (action->text() == "Center Center"    )
    viewKey->setLocation(CQChartsKeyLocation::Type::CENTER_CENTER);
  else if (action->text() == "Center Right"     )
    viewKey->setLocation(CQChartsKeyLocation::Type::CENTER_RIGHT );
  else if (action->text() == "Bottom Left"      )
    viewKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_LEFT  );
  else if (action->text() == "Bottom Center"    )
    viewKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_CENTER);
  else if (action->text() == "Bottom Right"     )
    viewKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_RIGHT );
  else if (action->text() == "Absolute Position")
    viewKey->setLocation(CQChartsKeyLocation::Type::ABS_POSITION );
  else if (action->text() == "Absolute Rect")
    viewKey->setLocation(CQChartsKeyLocation::Type::ABS_RECT     );
  else
    assert(false);

  invalidateObjects();

  update();
}

//------

void
CQChartsView::
plotKeyVisibleSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsPlotKey *plotKey = (basePlot ? basePlot->key() : nullptr);

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
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsPlotKey *plotKey = (basePlot ? basePlot->key() : nullptr);

  if (plotKey) {
    if      (action->text() == "Top Left"         )
      plotKey->setLocation(CQChartsKeyLocation::Type::TOP_LEFT     );
    else if (action->text() == "Top Center"       )
      plotKey->setLocation(CQChartsKeyLocation::Type::TOP_CENTER   );
    else if (action->text() == "Top Right"        )
      plotKey->setLocation(CQChartsKeyLocation::Type::TOP_RIGHT    );
    else if (action->text() == "Center Left"      )
      plotKey->setLocation(CQChartsKeyLocation::Type::CENTER_LEFT  );
    else if (action->text() == "Center Center"    )
      plotKey->setLocation(CQChartsKeyLocation::Type::CENTER_CENTER);
    else if (action->text() == "Center Right"     )
      plotKey->setLocation(CQChartsKeyLocation::Type::CENTER_RIGHT );
    else if (action->text() == "Bottom Left"      )
      plotKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_LEFT  );
    else if (action->text() == "Bottom Center"    )
      plotKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_CENTER);
    else if (action->text() == "Bottom Right"     )
      plotKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_RIGHT );
    else if (action->text() == "Absolute Position")
      plotKey->setLocation(CQChartsKeyLocation::Type::ABS_POSITION );
    else if (action->text() == "Absolute Rect")
      plotKey->setLocation(CQChartsKeyLocation::Type::ABS_RECT     );
    else
      assert(false);
  }
}

void
CQChartsView::
plotKeyInsideXSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsPlotKey *plotKey = (basePlot ? basePlot->key() : nullptr);

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
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsPlotKey *plotKey = (basePlot ? basePlot->key() : nullptr);

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
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsAxis *xAxis = (basePlot ? basePlot->xAxis() : nullptr);

  if (xAxis)
    xAxis->setVisible(b);
}

void
CQChartsView::
xAxisGridSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsAxis *xAxis = (basePlot ? basePlot->xAxis() : nullptr);

  if (xAxis)
    xAxis->setGridLinesDisplayed(b ? CQChartsAxis::GridLinesDisplayed::MAJOR :
                                     CQChartsAxis::GridLinesDisplayed::NONE);
}

void
CQChartsView::
xAxisSideSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsAxis *xAxis = (basePlot ? basePlot->xAxis() : nullptr);

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
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsAxis *yAxis = (basePlot ? basePlot->yAxis() : nullptr);

  if (yAxis)
    yAxis->setVisible(b);
}

void
CQChartsView::
yAxisGridSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsAxis *yAxis = (basePlot ? basePlot->yAxis() : nullptr);

  if (yAxis)
    yAxis->setGridLinesDisplayed(b ? CQChartsAxis::GridLinesDisplayed::MAJOR :
                                     CQChartsAxis::GridLinesDisplayed::NONE);
}

void
CQChartsView::
yAxisSideSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsAxis *yAxis = (basePlot ? basePlot->yAxis() : nullptr);

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
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsTitle *title = (basePlot ? basePlot->title() : nullptr);

  if (title)
    title->setVisible(b);
}

void
CQChartsView::
titleLocationSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  CQChartsTitle *title = (basePlot ? basePlot->title() : nullptr);

  if (title) {
    if      (action->text() == "Top")
      title->setLocation(CQChartsTitleLocation::Type::TOP);
    else if (action->text() == "Center")
      title->setLocation(CQChartsTitleLocation::Type::CENTER);
    else if (action->text() == "Bottom")
      title->setLocation(CQChartsTitleLocation::Type::BOTTOM);
    else if (action->text() == "Absolute")
      title->setLocation(CQChartsTitleLocation::Type::ABS_POS);
  }
}

void
CQChartsView::
invertXSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot)
    basePlot->setInvertX(b);
}

void
CQChartsView::
invertYSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot)
    basePlot->setInvertY(b);
}

void
CQChartsView::
fitSlot()
{
  for (const auto &plot : plots_) {
    plot->autoFit();
  }
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
  QAction *action = qobject_cast<QAction *>(sender());
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

  updatePlots();

  update();

  emit themePalettesChanged();
}

bool
CQChartsView::
isDark() const
{
  return charts()->interfaceTheme()->isDark();
}

void
CQChartsView::
setDark(bool b)
{
  charts()->interfaceTheme()->setDark(b);

  updatePlots();

  emit interfacePaletteChanged();
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

  if (! fileName.isNull())
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

  if (! fileName.isNull())
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
writeScriptSlot(const QString &filename)
{
  writeScript(filename);
}

void
CQChartsView::
writeScriptSlot()
{
  QString dir = QDir::current().dirName() + "/charts.js";

  QString fileName = QFileDialog::getSaveFileName(this, "Print Script", dir, "Files (*.js)");

  if (! fileName.isNull())
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
    CQChartsGeom::BBox pixelRect = plot->calcPlotPixelRect();

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
writeScript(const QString &filename, CQChartsPlot *plot)
{
  auto os = std::ofstream(filename.toStdString(), std::ofstream::out);

  //---

  CQChartsScriptPainter device(const_cast<CQChartsView *>(this), os);

  //---

  os <<
  "'use strict';\n"
  "\n"
  "var charts = new Charts();\n"
  "\n"
  "window.addEventListener(\"load\", eventWindowLoaded, false);\n"
  "\n"
  "//------\n"
  "\n"
  "function eventWindowLoaded () {\n"
  "  if (canvasSupport()) {\n"
  "    charts.init();\n"
  "\n"
  "    (function drawFrame () {\n"
  "      var canvas = document.getElementById(\"canvas\");\n"
  "\n"
  "      window.requestAnimationFrame(drawFrame, canvas);\n"
  "\n"
  "      charts.update();\n"
  "    }());\n"
  "  }\n"
  "}\n"
  "\n"
  "function canvasSupport() {\n"
  "  return true;\n"
  "  //return Modernizr.canvas;\n"
  "}\n"
  "\n"
  "function Charts () {\n"
  "  this.plots = [];\n"
  "}\n"
  "\n";

  os <<
  "Charts.prototype.init = function() {\n"
  "  this.canvas = document.getElementById(\"canvas\");\n"
  "  this.gc     = this.canvas.getContext(\"2d\");\n"
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

  if (plot) {
    std::string plotId = "plot_" + plot->id().toStdString();

    os << "  this." << plotId << " = new Charts_" << plotId << "();\n";
    os << "  this.plots.push(this." << plotId << ");\n";
    os << "\n";
    os << "  this." << plotId << ".init();\n";
  }
  else {
    for (auto &plot : plots_) {
      std::string plotId = "plot_" + plot->id().toStdString();

      os << "  this." << plotId << " = new Charts_" << plotId << "();\n";
      os << "  this.plots.push(this." << plotId << ");\n";
      os << "\n";
      os << "  this." << plotId << ".init();\n";
    }
  }

  os << "}\n";
  os << "\n";

  os <<
  "Charts.prototype.eventMouseDown = function(e) {\n"
  "  charts.plots.forEach(plot => plot.eventMouseDown(e));\n"
  "}\n"
  "\n"
  "Charts.prototype.eventMouseMove = function(e) {\n"
  "  charts.plots.forEach(plot => plot.eventMouseMove(e));\n"
  "}\n"
  "\n"
  "Charts.prototype.eventMouseUp = function(e) {\n"
  "  charts.plots.forEach(plot => plot.eventMouseMove(e));\n"
  "}\n"
  "\n";

  os <<
  "Charts.prototype.plotXToPixel = function(x) {\n"
  "  var sx = (x - this.xmin)/(this.xmax - this.xmin);\n"
  "\n"
  "  var x1 = this.vxmin*this.canvas.width/100.0;\n"
  "  var x2 = this.vxmax*this.canvas.width/100.0;\n"
  "\n"
  "  return sx*(x2 - x1) + x1;\n"
  "}\n"
  "\n"
  "Charts.prototype.plotYToPixel = function(y) {\n"
  "  var sy = (y - this.ymax)/(this.ymin - this.ymax);\n"
  "\n"
  "  var y1 = this.vymin*this.canvas.height/100.0;\n"
  "  var y2 = this.vymax*this.canvas.height/100.0;\n"
  "\n"
  "  return sy*(y2 - y1) + y1;\n"
  "}\n"
  "\n"
  "Charts.prototype.moveTo = function(x, y) {\n"
  "  var px = this.plotXToPixel(x);\n"
  "  var py = this.plotYToPixel(y);\n"
  "\n"
  "  this.gc.moveTo(px, py);\n"
  "}\n"
  "\n"
  "Charts.prototype.lineTo = function(x, y) {\n"
  "  var px = this.plotXToPixel(x);\n"
  "  var py = this.plotYToPixel(y);\n"
  "\n"
  "  this.gc.lineTo(px, py);\n"
  "}\n"
  "\n"
  "Charts.prototype.quadTo = function(x1, y1, x2, y2) {\n"
  "  var px1 = this.plotXToPixel(x1);\n"
  "  var py1 = this.plotYToPixel(y1);\n"
  "  var px2 = this.plotXToPixel(x2);\n"
  "  var py2 = this.plotYToPixel(y2);\n"
  "\n"
  "  this.gc.quadraticCurveTo(px, py);\n"
  "}\n"
  "\n"
  "Charts.prototype.curveTo = function(x1, y1, x2, y2, x3, y3) {\n"
  "  var px1 = this.plotXToPixel(x1);\n"
  "  var py1 = this.plotYToPixel(y1);\n"
  "  var px2 = this.plotXToPixel(x2);\n"
  "  var py2 = this.plotYToPixel(y2);\n"
  "  var px3 = this.plotXToPixel(x3);\n"
  "  var py3 = this.plotYToPixel(y3);\n"
  "\n"
  "  this.gc.bezierCurveTo(px1, py1, px2, py2, px3, py3);\n"
  "}\n"
  "\n"
  "Charts.prototype.drawRoundedPolygon = function(x1, y1, x2, y2, xs, ys) {\n"
  "  var px1 = this.plotXToPixel(x1);\n"
  "  var py1 = this.plotYToPixel(y1);\n"
  "  var px2 = this.plotXToPixel(x2);\n"
  "  var py2 = this.plotYToPixel(y2);\n"
  "\n"
  "  this.gc.beginPath();\n"
  "\n"
  "  this.gc.moveTo(px1, py1);\n"
  "  this.gc.lineTo(px2, py1);\n"
  "  this.gc.lineTo(px2, py2);\n"
  "  this.gc.lineTo(px1, py2);\n"
  "\n"
  "  this.gc.closePath();\n"
  "\n"
  "  this.gc.fill();\n"
  "  this.gc.stroke();\n"
  "}\n"
  "\n"
  "Charts.prototype.drawPoint = function(x, y) { \n"
  "  var px = this.plotXToPixel(x);\n"
  "  var py = this.plotYToPixel(y);\n"
  "\n"
  "  this.gc.beginPath();\n"
  "\n"
  "  this.gc.moveTo(px, py);\n"
  "  this.gc.lineTo(px, py);\n"
  "\n"
  "  this.gc.stroke();\n"
  "}\n"
  "\n"
  "Charts.prototype.drawLine = function(x1, y1, x2, y2) { \n"
  "  var px1 = this.plotXToPixel(x1);\n"
  "  var py1 = this.plotYToPixel(y1);\n"
  "  var px2 = this.plotXToPixel(x2);\n"
  "  var py2 = this.plotYToPixel(y2);\n"
  "\n"
  "  this.gc.beginPath();\n"
  "\n"
  "  this.gc.moveTo(px1, py1);\n"
  "  this.gc.lineTo(px2, py2);\n"
  "\n"
  "  this.gc.stroke();\n"
  "}\n"
  "\n"
  "Charts.prototype.strokeRect = function(x1, y1, x2, y2) {\n"
  "  var px1 = this.plotXToPixel(x1);\n"
  "  var py1 = this.plotYToPixel(y1);\n"
  "  var px2 = this.plotXToPixel(x2);\n"
  "  var py2 = this.plotYToPixel(y2);\n"
  "\n"
  "  this.gc.beginPath();\n"
  "\n"
  "  this.gc.moveTo(px1, py1);\n"
  "  this.gc.lineTo(px2, py1);\n"
  "  this.gc.lineTo(px2, py2);\n"
  "  this.gc.lineTo(px1, py2);\n"
  "\n"
  "  this.gc.closePath();\n"
  "\n"
  "  this.gc.stroke();\n"
  "}\n"
  "\n"
  "Charts.prototype.fillRect = function(x1, y1, x2, y2) {\n"
  "  var px1 = this.plotXToPixel(x1);\n"
  "  var py1 = this.plotYToPixel(y1);\n"
  "  var px2 = this.plotXToPixel(x2);\n"
  "  var py2 = this.plotYToPixel(y2);\n"
  "\n"
  "  this.gc.beginPath();\n"
  "\n"
  "  this.gc.moveTo(px1, py1);\n"
  "  this.gc.lineTo(px2, py1);\n"
  "  this.gc.lineTo(px2, py2);\n"
  "  this.gc.lineTo(px1, py2);\n"
  "\n"
  "  this.gc.closePath();\n"
  "\n"
  "  this.gc.fill();\n"
  "}\n"
  "\n"
  "Charts.prototype.drawRect = function(x1, y1, x2, y2) {\n"
  "  var px1 = this.plotXToPixel(x1);\n"
  "  var py1 = this.plotYToPixel(y1);\n"
  "  var px2 = this.plotXToPixel(x2);\n"
  "  var py2 = this.plotYToPixel(y2);\n"
  "\n"
  "  this.gc.beginPath();\n"
  "\n"
  "  this.gc.moveTo(px1, py1);\n"
  "  this.gc.lineTo(px2, py1);\n"
  "  this.gc.lineTo(px2, py2);\n"
  "  this.gc.lineTo(px1, py2);\n"
  "\n"
  "  this.gc.closePath();\n"
  "\n"
  "  this.gc.fill();\n"
  "  this.gc.stroke();\n"
  "}\n"
  "\n"
  "Charts.prototype.drawEllipse = function(x1, y1, x2, y2) {\n"
  "  var px1 = this.plotXToPixel(x1);\n"
  "  var py1 = this.plotYToPixel(y1);\n"
  "  var px2 = this.plotXToPixel(x2);\n"
  "  var py2 = this.plotYToPixel(y2);\n"
  "\n"
  "  var xc = (px1 + px2)/2;\n"
  "  var yc = (py1 + py2)/2;\n"
  "  var w  = Math.abs(px2 - px1);\n"
  "  var h  = Math.abs(py2 - py1);\n"
  "\n"
  "  this.gc.beginPath();\n"
  "\n"
  "  this.gc.moveTo(xc, yc - h/2);\n"
  "\n"
  "  this.gc.bezierCurveTo(xc + w/2, yc - h/2, xc + w/2, yc + h/2, xc, yc + h/2);\n"
  "  this.gc.bezierCurveTo(xc - w/2, yc + h/2, xc - w/2, yc - h/2, xc, yc - h/2);\n"
  "\n"
  "  this.gc.closePath();\n"
  "\n"
  "  this.gc.fill();\n"
  "  this.gc.stroke();\n"
  "}\n"
  "\n"
  "Charts.prototype.drawPolygon = function(poly) {\n"
  "  var np = poly.length;\n"
  "  this.gc.beginPath();\n"
  "  for (var i = 0; i < np; ++i) {\n"
  "    var px = this.plotXToPixel(poly[2*i    ]);\n"
  "    var py = this.plotYToPixel(poly[2*i + 1]);\n"
  "    if (i == 0)\n"
  "      this.gc.moveTo(px, py);\n"
  "    else\n"
  "      this.gc.lineTo(px, py);\n"
  "  }\n"
  "  this.gc.closePath();\n"
  "  this.gc.fill();\n"
  "  this.gc.stroke();\n"
  "}\n"
  "\n"
  "Charts.prototype.drawPolyline = function(poly) {\n"
  "  var np = poly.length;\n"
  "  this.gc.beginPath();\n"
  "  for (var i = 0; i < np; ++i) {\n"
  "    var px = this.plotXToPixel(poly[2*i    ]);\n"
  "    var py = this.plotYToPixel(poly[2*i + 1]);\n"
  "    if (i == 0)\n"
  "      this.gc.moveTo(px, py);\n"
  "    else\n"
  "      this.gc.lineTo(px, py);\n"
  "  }\n"
  "  this.gc.stroke();\n"
  "}\n"
  "\n"
  "Charts.prototype.drawText = function(x, y, text) {\n"
  "  var px = this.plotXToPixel(x);\n"
  "  var py = this.plotYToPixel(y);\n"
  "\n"
  "  var fillStyle = this.gc.fillStyle;\n"
  "  this.gc.fillStyle = this.gc.strokeStyle;\n"
  "  this.gc.fillText(text, px, py);\n"
  "  this.gc.fillStyle = fillStyle;\n"
  "}\n"
  "\n"
  "Charts.prototype.drawRotatedText = function(x, y, text, a) {\n"
  "  var px = this.plotXToPixel(x);\n"
  "  var py = this.plotYToPixel(y);\n"
  "\n"
  "  var c = Math.cos(a);\n"
  "  var s = Math.sin(a);\n"
  "\n"
  "  this.gc.setTransform(c, -s, s, c, px, py);\n"
  "\n"
  "  var fillStyle = this.gc.fillStyle;\n"
  "  this.gc.fillStyle = this.gc.strokeStyle;\n"
  "  this.gc.fillText(text, 0, 0);\n"
  "  this.gc.fillStyle = fillStyle;\n"
  "\n"
  "  this.gc.setTransform(1, 0, 0, 1, 0, 0);\n"
  "}\n"
  "\n";

  //---

  os <<
  "Charts.prototype.pointInsideRect = function(x, y, xmin, ymin, xmax, ymax) {\n"
  "  var pxmin = this.plotXToPixel(xmin);\n"
  "  var pymax = this.plotYToPixel(ymin);\n"
  "  var pxmax = this.plotXToPixel(xmax);\n"
  "  var pymin = this.plotYToPixel(ymax);\n"
  "  return (x >= pxmin && x <= pxmax && y >= pymin && y <= pymax);\n"
  "}\n"
  "\n"
  "Charts.prototype.pointInsidePoly = function(x, y, poly) {\n"
  "  var np = poly.length;\n"
  "  var counter = 0;\n"
  "  var i2 = np - 1;\n"
  "  for (var i1 = 0; i1 < np; ++i1) {\n"
  "    var px1 = this.plotXToPixel(poly[2*i1    ]);\n"
  "    var py1 = this.plotYToPixel(poly[2*i1 + 1]);\n"
  "    var px2 = this.plotXToPixel(poly[2*i2    ]);\n"
  "    var py2 = this.plotYToPixel(poly[2*i2 + 1]);\n"
  "    if (y > Math.min(py1, py2)) {\n"
  "      if (y <= Math.max(py1, py2)) {\n"
  "        if (x <= Math.max(px1, px2)) {\n"
  "          if (py1 != py2) {\n"
  "            var xinters = (y - py1)*(px2 - px1)/(py2 - py1) + px1;\n"
  "            if (px1 == px2 || x <= xinters)\n"
  "              ++counter;\n"
  "          }\n"
  "        }\n"
  "      }\n"
  "    }\n"
  "    i2 = i1;\n"
  "  }\n"
  "  return ((counter % 2) != 0);\n"
  "}\n"
  "\n";

  //---

  os << "Charts.prototype.update = function() {\n";

  if (plot) {
    std::string plotId = "plot_" + plot->id().toStdString();

    os << "  this." << plotId << ".draw();\n";
  }
  else {
    for (auto &plot : plots_) {
      std::string plotId = "plot_" + plot->id().toStdString();

      os << "  this." << plotId << ".draw();\n";
    }
  }

  if (hasAnnotations()) {
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

  if (hasAnnotations()) {
    os << "\n";
    os << "Charts.prototype.drawAnnotations = function() {\n";

    drawAnnotations(&device, CQChartsLayer::Type::ANNOTATION);

    os << "}\n";
  }

  os << "\n";
  os << "Charts.prototype.log = function(s) {\n";
  os << "  document.getElementById(\"canvas_log\").innerHTML = s;\n";
  os << "}\n";

  //---

  os << "\n";

  if (plot) {
    plot->writeScript(os);
  }
  else {
    for (auto &plot : plots_)
      plot->writeScript(os);
  }

  //---

  return true;
}

//------

void
CQChartsView::
showBoxesSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

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
  window()->setXRangeMap(b);
}

void
CQChartsView::
yRangeMapSlot(bool b)
{
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
  QAction *action = qobject_cast<QAction *>(sender());

  bool ok;

  int plotInd = CQChartsVariant::toInt(action->data(), ok);
  assert(ok);

  setCurrentPlotInd(plotInd);
}

//------

void
CQChartsView::
updatePlots()
{
  for (auto &plot : plots_) {
    plot->drawObjs();
  }

  update();
}

//------

bool
CQChartsView::
plots(Plots &plots, bool clear) const
{
  if (clear)
    plots.clear();

  for (const auto &plot : plots_)
    plots.push_back(plot);

  return ! plots.empty();
}

bool
CQChartsView::
basePlots(PlotSet &plots, bool clear) const
{
  if (clear)
    plots.clear();

  for (const auto &plot : plots_) {
    CQChartsPlot *plot1 = plot->firstPlot();

    plots.insert(plot1);
  }

  return ! plots.empty();
}

bool
CQChartsView::
plotsAt(const CQChartsGeom::Point &p, Plots &plots, CQChartsPlot* &plot,
        bool clear, bool first) const
{
  if (clear)
    plots.clear();

  using PlotSet = std::set<CQChartsPlot*>;

  PlotSet plotSet;

  plot = nullptr;

  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/false);

  for (const auto &plot1 : plots_) {
    const CQChartsGeom::BBox &bbox = plot1->viewBBox();

    if (! bbox.inside(p))
      continue;

    CQChartsPlot *plot2 = (first && plot1->isOverlay() ? plot1->firstPlot() : plot1);

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
plotAt(const CQChartsGeom::Point &p) const
{
  Plots         plots;
  CQChartsPlot *plot;

  (void) plotsAt(p, plots, plot, true);

  return plot;
}

bool
CQChartsView::
plotsAt(const CQChartsGeom::Point &p, Plots &plots, bool clear) const
{
  CQChartsPlot *plot;

  return plotsAt(p, plots, plot, clear);
}

bool
CQChartsView::
basePlotsAt(const CQChartsGeom::Point &p, PlotSet &plots, bool clear) const
{
  if (clear)
    plots.clear();

  for (const auto &plot : plots_) {
    const CQChartsGeom::BBox &bbox = plot->viewBBox();

    if (! bbox.inside(p))
      continue;

    CQChartsPlot *plot1 = plot->firstPlot();

    plots.insert(plot1);
  }

  return ! plots.empty();
}

CQChartsGeom::BBox
CQChartsView::
plotBBox(CQChartsPlot *plot) const
{
  for (const auto &plot1 : plots_) {
    if (plot1 != plot)
      continue;

    const CQChartsGeom::BBox &bbox = plot1->viewBBox();

    return bbox;
  }

  return CQChartsGeom::BBox();
}

//---

int
CQChartsView::
plotInd(CQChartsPlot *plot) const
{
  int ind = 0;

  for (const auto &plot1 : plots_) {
    if (plot1 == plot)
      return ind;

    ++ind;
  }

  return -1;
}

CQChartsPlot *
CQChartsView::
currentPlot(bool remap) const
{
  if (plots_.empty())
    return nullptr;

  int ind = currentPlotInd();

  if (ind < 0 || ind >= int(plots_.size()))
    ind = 0;

  CQChartsPlot *plot = getPlotForInd(ind);

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
  if (ind < 0 || ind >= int(plots_.size()))
    return nullptr;

  CQChartsPlot *plot = plots_[ind];

  return plot;
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

  const CQChartsView::Annotations &viewAnnotations = this->annotations();

  int annotationId = 1;

  for (const auto &annotation : viewAnnotations) {
    os << "\n";

    annotation->write(os, "view", QString("annotation%1").arg(annotationId));

    ++annotationId;
  }

  //---

  Plots plots;

  this->getPlots(plots);

  using ModelVars = std::map<QString,QString>;
  using PlotVars  = std::map<CQChartsPlot*,QString>;

  ModelVars modelVars;
  PlotVars  plotVars;

  for (const auto &plot : plots) {
    CQChartsModelData *modelData = plot->getModelData();

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

    plot->write(os, plotVarName, modelVarName);

    //---

    const CQChartsPlot::Annotations &plotAnnotations = plot->annotations();

    for (const auto &annotation : plotAnnotations) {
      os << "\n";

      annotation->write(os, plotVarName, QString("annotation%1").arg(annotationId));

      ++annotationId;
    }
  }

  CQChartsView::PlotSet basePlots;

  this->basePlots(basePlots);

  for (const auto &plot : basePlots) {
    if      (plot->isX1X2()) {
      CQChartsPlot *plot1, *plot2;

      plot->x1x2Plots(plot1, plot2);

      os << "\n";
      os << "group_charts_plots -x1x2 -overlay";

      os << " $" << plotVars[plot1].toStdString();
      os << " $" << plotVars[plot2].toStdString();
      os << "\n";
    }
    else if (plot->isY1Y2()) {
      CQChartsPlot *plot1, *plot2;

      plot->y1y2Plots(plot1, plot2);

      os << "\n";
      os << "group_charts_plots -y1y2 -overlay";

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

QPointF
CQChartsView::
positionToView(const CQChartsPosition &pos) const
{
  CQChartsGeom::Point p = CQChartsGeom::Point(pos.p());

  CQChartsGeom::Point p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = p;
  else if (pos.units() == CQChartsUnits::PERCENT) {
    p1.setX(p.getX()*width ()/100.0);
    p1.setY(p.getY()*height()/100.0);
  }

  return p1.qpoint();
}

QPointF
CQChartsView::
positionToPixel(const CQChartsPosition &pos) const
{
  CQChartsGeom::Point p = CQChartsGeom::Point(pos.p());

  CQChartsGeom::Point p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = p;
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = windowToPixel(p);
  else if (pos.units() == CQChartsUnits::PERCENT) {
    p1.setX(p.getX()*width ()/100.0);
    p1.setY(p.getY()*height()/100.0);
  }

  return p1.qpoint();
}

//------

QRectF
CQChartsView::
rectToView(const CQChartsRect &rect) const
{
  CQChartsGeom::BBox r = CQChartsGeom::BBox(rect.rect());

  CQChartsGeom::BBox r1 = r;

  if      (rect.units() == CQChartsUnits::PIXEL)
    r1 = pixelToWindow(r);
  else if (rect.units() == CQChartsUnits::VIEW)
    r1 = r;
  else if (rect.units() == CQChartsUnits::PERCENT) {
    r1.setXMin(r.getXMin()*width ()/100.0);
    r1.setYMin(r.getYMin()*height()/100.0);
    r1.setXMax(r.getXMax()*width ()/100.0);
    r1.setYMax(r.getYMax()*height()/100.0);
  }

  return r1.qrect();
}

QRectF
CQChartsView::
rectToPixel(const CQChartsRect &rect) const
{
  CQChartsGeom::BBox r = CQChartsGeom::BBox(rect.rect());

  CQChartsGeom::BBox r1 = r;

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

  return r1.qrect();
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
  else
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
  else
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
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*width()/100.0;
  else
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
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*height()/100.0;
  else
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
windowToPixel(const CQChartsGeom::Point &w) const
{
  CQChartsGeom::Point p;

  windowToPixelI(w.x, w.y, p.x, p.y);

  return p;
}

CQChartsGeom::Point
CQChartsView::
pixelToWindow(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::Point w;

  pixelToWindowI(p.x, p.y, w.x, w.y);

  return w;
}

QPointF
CQChartsView::
windowToPixel(const QPointF &w) const
{
  double px, py;

  windowToPixelI(w.x(), w.y(), px, py);

  return QPointF(px, py);
}

QPointF
CQChartsView::
pixelToWindow(const QPointF &p) const
{
  double wx, wy;

  pixelToWindowI(p.x(), p.y(), wx, wy);

  return QPointF(wx, wy);
}

CQChartsGeom::BBox
CQChartsView::
windowToPixel(const CQChartsGeom::BBox &wrect) const
{
  double px1, py1, px2, py2;

  windowToPixelI(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixelI(wrect.getXMax(), wrect.getYMax(), px2, py1);

  return CQChartsGeom::BBox(px1, py1, px2, py2);
}

CQChartsGeom::BBox
CQChartsView::
pixelToWindow(const CQChartsGeom::BBox &prect) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindowI(prect.getXMin(), prect.getYMin(), wx1, wy2);
  pixelToWindowI(prect.getXMax(), prect.getYMax(), wx2, wy1);

  return CQChartsGeom::BBox(wx1, wy1, wx2, wy2);
}

QRectF
CQChartsView::
windowToPixel(const QRectF &w) const
{
  return windowToPixel(CQChartsGeom::BBox(w)).qrect();
}

QRectF
CQChartsView::
pixelToWindow(const QRectF &p) const
{
  return pixelToWindow(CQChartsGeom::BBox(p)).qrect();
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

QSizeF
CQChartsView::
pixelToWindowSize(const QSizeF &ps) const
{
  double w = pixelToWindowWidth (ps.width ());
  double h = pixelToWindowHeight(ps.height());

  return QSizeF(w, h);
}

//------

QSize
CQChartsView::
sizeHint() const
{
  return viewSizeHint();
}
