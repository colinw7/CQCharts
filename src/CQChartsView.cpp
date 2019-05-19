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
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQChartsEnv.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsVariant.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsInterfaceTheme.h>
#include <CQChartsTheme.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QSvgGenerator>
#include <QFileDialog>
#include <QRubberBand>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMenu>
#include <QPainter>

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

  setBackgroundFillColor(CQChartsColor(Qt::white));

  //---

  bufferLayers_ = CQChartsEnv::getBool("CQ_CHARTS_BUFFER_LAYERS", bufferLayers_);

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

  connect(CQChartsThemeMgrInst, SIGNAL(themeChanged(const QString &)),
          this, SLOT(themeChangedSlot(const QString &)));
  connect(CQChartsThemeMgrInst, SIGNAL(paletteChanged(const QString &)),
          this, SLOT(paletteChangedSlot(const QString &)));

  // TODO: only connect to current theme ?
  connect(CQChartsThemeMgrInst, SIGNAL(themesChanged()), this, SLOT(updatePlots()));
  connect(CQChartsThemeMgrInst, SIGNAL(palettesChanged()), this, SLOT(updatePlots()));
}

CQChartsView::
~CQChartsView()
{
  if (charts_)
    charts_->removeView(this);

  //---

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

  // data
  addProp("", "mode"          , "", "View mouse mode" )->setHidden(true);
  addProp("", "id"            , "", "View id"         )->setHidden(true);
  addProp("", "currentPlotInd", "", "Current plot ind")->setHidden(true);

  addProp("", "viewSizeHint", "", "View size hint")->setHidden(true);
  addProp("", "zoomData"    , "", "Zoom data"     )->setHidden(true);
  addProp("", "bufferLayers", "", "Buffer layer"  )->setHidden(true);

  addProp("", "showTable"   , "", "Show table of value")->setHidden(true);
  addProp("", "showSettings", "", "Show settings panel")->setHidden(true);

  // options
  addProp("options", "antiAlias", "", "Draw aliased shapes")->setHidden(true);

  // title
  addProp("title", "title", "string", "View title string");

  // theme
  addProp("theme", "theme", "name", "View theme")->
    setValues(QStringList() << "default" << "theme1" << "theme2");
  addProp("theme", "dark" , "dark", "View interface is dark");

  // color
  addProp("color", "defaultPalette", "defaultPalette", "Default palette");

  // text
  addProp("text", "scaleFont" , "scaled", "Scale font to view size");
  addProp("text", "fontFactor", "factor", "Global text scale factor");
  addProp("text", "font"      , "font"  , "Global text font");

  // sizing
  addProp("sizing", "autoSize" , "auto"     , "Auto scale to view size");
  addProp("sizing", "fixedSize", "fixedSize", "Fixed view size");

  // background fill
  addProp("background/fill", "backgroundFillData"   , "style"  , "Fill style"  )->setHidden(true);
  addProp("background/fill", "backgroundFillColor"  , "color"  , "Fill color"  );
  addProp("background/fill", "backgroundFillAlpha"  , "alpha"  , "Fill alpha"  )->setHidden(true);
  addProp("background/fill", "backgroundFillPattern", "pattern", "Fill pattern")->setHidden(true);

  // select mode
  addProp("select", "selectMode"  , "mode"  , "Selection mode");
  addProp("select", "selectInside", "inside", "Select when fully inside select rectangle");

  // select highlight
  addProp("select/highlight"       , "selectedMode"       , "mode"   , "Highlight draw mode");
  addProp("select/highlight"       , "selectedShapeData"  , "style"  , "Highlight shape data");
  addProp("select/highlight/fill"  , "selectedFilled"     , "visible", "Highlight fill is visible");
  addProp("select/highlight/fill"  , "selectedFillColor"  , "color"  , "Highlight fill color");
  addProp("select/highlight/fill"  , "selectedFillAlpha"  , "alpha"  , "Highlight fill alpha");
  addProp("select/highlight/stroke", "selectedBorder"     , "visible", "Highlight stroke visible");
  addProp("select/highlight/stroke", "selectedBorderColor", "color"  , "Highlight stroke color");
  addProp("select/highlight/stroke", "selectedBorderWidth", "width"  , "Highlight stroke width");
  addProp("select/highlight/stroke", "selectedBorderDash" , "dash"   , "Highlight stroke dash");

  // inside highlight
  addProp("inside/highlight"       , "insideMode"       , "mode"   , "Inside draw mode");
  addProp("inside/highlight"       , "insideShapeData"  , "style"  , "Inside shape data");
  addProp("inside/highlight/fill"  , "insideFilled"     , "visible", "Inside fill is visible");
  addProp("inside/highlight/fill"  , "insideFillColor"  , "color"  , "Inside fill color");
  addProp("inside/highlight/fill"  , "insideFillAlpha"  , "alpha"  , "Inside fill alpha");
  addProp("inside/highlight/stroke", "insideBorder"     , "visible", "Inside strok is visible");
  addProp("inside/highlight/stroke", "insideBorderColor", "color"  , "Inside stroke color");
  addProp("inside/highlight/stroke", "insideBorderWidth", "width"  , "Inside stroke width");
  addProp("inside/highlight/stroke", "insideBorderDash" , "dash"   , "Inside stroke dash");

  // status
  addProp("status", "posTextType", "posTextType", "Position text type")->setHidden(true);

  // TODO: remove or make more general
  addProp("scroll", "scrolled"      , "enabled" , "Scrolling enabled");
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
setPainterFont(QPainter *painter, const CQChartsFont &font) const
{
  painter->setFont(viewFont(font));
}

void
CQChartsView::
setPainterFont(QPainter *painter, const QFont &font) const
{
  painter->setFont(viewFont(font));
}

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
deselectAll()
{
  startSelection();

  for (auto &plot : plots_)
    plot->deselectAll();

  for (auto &annotation : annotations())
    annotation->setSelected(false);

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

CQChartsTheme *
CQChartsView::
theme()
{
  return themeName().obj();
}

const CQChartsTheme *
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

CQChartsGradientPalette *
CQChartsView::
interfacePalette() const
{
  return charts()->interfaceTheme()->palette();
}

CQChartsGradientPalette *
CQChartsView::
themeGroupPalette(int i, int /*n*/) const
{
  return theme()->palette(i);
}

CQChartsGradientPalette *
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
getPropertyHidden(const QString &name, bool &hidden) const
{
  hidden = false;

  const CQPropertyViewItem *item = propertyModel()->propertyItem(this, name, /*hidden*/true);
  if (! item) return false;

  hidden = item->isHidden();

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

CQChartsArrowAnnotation *
CQChartsView::
addArrowAnnotation(const CQChartsPosition &start, const CQChartsPosition &end)
{
  CQChartsArrowAnnotation *arrowAnnotation = new CQChartsArrowAnnotation(this, start, end);

  addAnnotation(arrowAnnotation);

  return arrowAnnotation;
}

CQChartsRectAnnotation *
CQChartsView::
addRectAnnotation(const CQChartsRect &rect)
{
  CQChartsRectAnnotation *rectAnnotation = new CQChartsRectAnnotation(this, rect);

  addAnnotation(rectAnnotation);

  return rectAnnotation;
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

CQChartsPointAnnotation *
CQChartsView::
addPointAnnotation(const CQChartsPosition &pos, const CQChartsSymbol &type)
{
  CQChartsPointAnnotation *pointAnnotation = new CQChartsPointAnnotation(this, pos, type);

  addAnnotation(pointAnnotation);

  return pointAnnotation;
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

  if (mouseData_.plot == plot)
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

  mouseData_.pressPoint = me->pos();
  mouseData_.button     = me->button();
  mouseData_.pressed    = true;
  mouseData_.movePoint  = mouseData_.pressPoint;
  mouseData_.selMod     = modifiersToSelMod(me->modifiers());

  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(mouseData_.pressPoint)));

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if (mouseData_.button == Qt::LeftButton) {
    if      (mode() == Mode::SELECT) {
      if      (selectMode_ == SelectMode::POINT) {
        selectPointPress();
      }
      else if (selectMode_ == SelectMode::RECT) {
        startRegionBand(mouseData_.pressPoint);
      }
    }
    else if (mode() == Mode::ZOOM) {
      startRegionBand(mouseData_.pressPoint);
    }
    else if (mode() == Mode::PAN) {
    }
    else if (mode() == Mode::PROBE) {
    }
    else if (mode() == Mode::QUERY) {
    }
    else if (mode() == Mode::EDIT) {
      (void) editMousePress(mouseData_.pressPoint);
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
  }
  else if (mouseData_.button == Qt::RightButton) {
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

  QPointF oldMovePoint = mouseData_.movePoint;

  mouseData_.movePoint = me->pos();

  // select mode and move (not pressed) - update plot positions
  if (mode() == Mode::SELECT && ! mouseData_.pressed) {
    updatePosText(mouseData_.movePoint);

    //---

    searchPos_ = mouseData_.movePoint;

    if (searchTimer_)
      searchTimer_->start();
    else
      searchSlot();

    return;
  }

  //---

  if (mode() == Mode::ZOOM) {
    updatePosText(mouseData_.movePoint);
  }

  //---

  // probe mode and move (pressed or not pressed) - show probe lines
  if (mode() == Mode::PROBE) {
    showProbeLines(mouseData_.movePoint);
    return;
  }

  //---

  if (mode() == Mode::QUERY) {
    updatePosText(mouseData_.movePoint);
    return;
  }

  //---

  if (! mouseData_.pressed) {
    if (mode() == Mode::EDIT)
      editMouseMotion(mouseData_.movePoint);

    return;
  }

  // get plots at point
  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(mouseData_.movePoint)));

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if      (mouseData_.button == Qt::LeftButton) {
    // select plot object
    if      (mode() == Mode::SELECT) {
      if      (selectMode_ == SelectMode::POINT) {
        processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &pos) {
          bool current = (plot == mouseData_.plot);

          return plot->selectMouseMove(pos, current);
        }, searchPos_);
      }
      else if (selectMode_ == SelectMode::RECT) {
        if (mouseData_.escape)
          endRegionBand();
        else
          updateRegionBand(mouseData_.pressPoint, mouseData_.movePoint);
      }

      searchPos_ = mouseData_.movePoint;
    }
    // draw zoom rectangle
    else if (mode() == Mode::ZOOM) {
      mouseData_.movePoint = mouseData_.movePoint;

      if      (mouseData_.escape)
        endRegionBand();
      else if (mouseData_.plot)
        updateRegionBand(mouseData_.plot, mouseData_.pressPoint, mouseData_.movePoint);
    }
    else if (mode() == Mode::PAN) {
      if (mouseData_.plot) {
        CQChartsGeom::Point w1 =
          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(oldMovePoint        )));
        CQChartsGeom::Point w2 =
          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(mouseData_.movePoint)));

        double dx = w1.x - w2.x;
        double dy = w1.y - w2.y;

        mouseData_.plot->pan(dx, dy);
      }
    }
    else if (mode() == Mode::EDIT) {
      processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &pos) {
        bool current = (plot == mouseData_.plot);

        return plot->editMouseMove(pos, current);
      }, mouseData_.movePoint);
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
    if (! mouseData_.pressed)
      return;
  }
  else if (mouseData_.button == Qt::RightButton) {
    if (! mouseData_.pressed)
      return;
  }
}

void
CQChartsView::
mouseReleaseEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

//QPointF oldMovePoint = mouseData_.movePoint;

  mouseData_.movePoint = me->pos();

  CQChartsScopeGuard resetMouseData([&]() { mouseData_.reset(); });

  //CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(mouseData_.movePoint)));

  if      (mouseData_.button == Qt::LeftButton) {
    if      (mode() == Mode::SELECT) {
      if (! mouseData_.pressed)
        return;

      if      (selectMode_ == SelectMode::POINT) {
        if (mouseData_.plot)
          mouseData_.plot->selectMouseRelease(mouseData_.movePoint);
      }
      else if (selectMode_ == SelectMode::RECT) {
        endRegionBand();

        //---

        double dx = abs(mouseData_.movePoint.x() - mouseData_.pressPoint.x());
        double dy = abs(mouseData_.movePoint.y() - mouseData_.pressPoint.y());

        if (dx < 4 && dy < 4) {
          selectPointPress();

          if (mouseData_.plot)
            mouseData_.plot->selectMouseRelease(mouseData_.movePoint);
        }
        else {
          processMouseDataPlots([&](CQChartsPlot *plot, const CQChartsSelMod &selMod) {
            CQChartsGeom::Point w1 =
              plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.pressPoint));
            CQChartsGeom::Point w2 =
              plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.movePoint ));

            return plot->rectSelect(CQChartsGeom::BBox(w1, w2), selMod);
          }, mouseData_.selMod);
        }
      }
    }
    else if (mode() == Mode::ZOOM) {
      if (! mouseData_.pressed)
        return;

      endRegionBand();

      if (mouseData_.escape)
        return;

      if (mouseData_.plot) {
        CQChartsGeom::Point w1 =
          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPointF(mouseData_.pressPoint));
        CQChartsGeom::Point w2 =
          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPointF(mouseData_.movePoint ));

        CQChartsGeom::BBox bbox(w1, w2);

        mouseData_.plot->zoomTo(bbox);
      }
    }
    else if (mode() == Mode::PAN) {
    }
    else if (mode() == Mode::PROBE) {
    }
    else if (mode() == Mode::QUERY) {
    }
    else if (mode() == Mode::EDIT) {
      if (! mouseData_.pressed)
        return;

      processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &pos) {
        plot->editMouseRelease(pos); return false;
      }, mouseData_.movePoint);
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
  }
  else if (mouseData_.button == Qt::RightButton) {
  }
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

    if (mouseData_.pressed)
      endRegionBand();

    if      (mode() == Mode::ZOOM) {
      if (! mouseData_.pressed)
        selectModeSlot();
    }
    else if (mode() == Mode::PAN) {
      if (! mouseData_.pressed)
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

  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(pos));

  Plots         plots;
  CQChartsPlot *plot;

  plotsAt(w, plots, plot);

  if (plot)
    plot->keyPress(ke->key(), ke->modifiers());
}

//------

bool
CQChartsView::
editMousePress(const QPointF &p)
{
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

void
CQChartsView::
editMouseMotion(const QPointF &p)
{
  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(p)));

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

  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(p)));

  Plots         plots;
  CQChartsPlot *plot;

  plotsAt(w, plots, plot);

  int probeInd = 0;

  for (auto &plot : plots) {
    CQChartsGeom::Point w = plot->pixelToWindow(CQChartsUtil::fromQPoint(p));

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
selectPointPress()
{
  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(mouseData_.pressPoint)));

  //---

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w)) {
      if (annotation->selectPress(w)) {
        startSelection();

        deselectAll();

        annotation->setSelected(true);

        endSelection();

        update();

        emit annotationPressed  (annotation);
        emit annotationIdPressed(annotation->id());

        return;
      }
    }
  }

  //---

  struct SelData {
    QPointF        pos;
    CQChartsSelMod selMod;

    SelData(const QPointF &pos, CQChartsSelMod selMod) :
     pos(pos), selMod(selMod) {
    }
  };

  SelData selData(mouseData_.pressPoint, mouseData_.selMod);

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

  if (key() && key()->isInside(w))
    key()->selectPress(w, SelMod::REPLACE);
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

  startSelection();

  deselectAll();

  selObj->setSelected(true);

  endSelection();
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
    CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(pos)));

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
      CQChartsGeom::Point w = plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(pos)));

      if (posStr.length())
        posStr += " ";

      posStr += plot->posStr(w);
    }
  }
  else if (posTextType() == PosTextType::VIEW) {
    CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(pos)));

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

  image_ = new QImage(QSize(iw, ih), QImage::Format_ARGB32);

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

  painter->fillRect(CQChartsUtil::toQRect(prect_), brush);

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

    // draw view annotations
    // TODO: allow use extra layer for foreground (annotations, key)
    for (auto &annotation : annotations())
      annotation->draw(painter);

    //---

    // draw view key
    if (key())
      key()->draw(painter);
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
updateSlot()
{
  update();
}

//------

void
CQChartsView::
searchSlot()
{
  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(searchPos_));

  plotsAt(w, mouseData_.plots, mouseData_.plot, /*clear*/true, /*first*/true);

  //---

  setStatusText("");

  bool handled = false;

  processMouseDataPlots([&](CQChartsPlot *plot, const QPointF &pos) {
    CQChartsGeom::Point w = plot->pixelToWindow(CQChartsUtil::fromQPoint(pos));

    if (plot->selectMove(w, ! handled))
      handled = true;

    return false;
  }, searchPos_);
}

//------

void
CQChartsView::
themeChangedSlot(const QString &name)
{
  if (name == theme()->name()) {
    setSelectedFillColor(theme()->selectColor());
    setInsideFillColor  (theme()->insideColor());
  }
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
  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(p));

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

  modeMenu->addActions(modeActionGroup->actions());

  //---

  popupMenu_->addSeparator();

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
    addKeyLocationGroupAction("Absolute Rect"    , CQChartsKeyLocation::Type::ABS_RECT      );

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

    keyLocationActionGroup->setExclusive(true);

    if (plotKey) {
      CQChartsKeyLocation::Type location = plotKey->location().type();

      keyLocationActionMap[location]->setChecked(true);
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

  CQChartsThemeMgrInst->getThemeNames(themeNames);

  for (const auto &themeName : themeNames) {
    CQChartsTheme *theme = CQChartsThemeMgrInst->getTheme(themeName) ;

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
viewKeyVisibleSlot(bool b)
{
  if (key() && b != key()->isVisible())
    key()->setVisible(b);
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
  setSelectedFillColor(theme()->selectColor());

  setInsideFillColor  (theme()->insideColor());
  setInsideBorderWidth(CQChartsLength("2px"));

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

bool
CQChartsView::
printPNG(const QString &filename, CQChartsPlot *plot)
{
  int w = width ();
  int h = height();

  QImage image = QImage(QSize(w, h), QImage::Format_ARGB32);

  QPainter painter;

  if (! painter.begin(&image))
    return false;

  paint(&painter, plot);

  painter.end();

  if (plot) {
    CQChartsGeom::BBox pixelRect = plot->calcPlotPixelRect();

    image = image.copy(CQChartsUtil::toQRectI(pixelRect));
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
write(std::ostream &os) const
{
  os << "set view [create_view]\n";

  CQPropertyViewModel::NameValues nameValues;

  propertyModel()->getChangedNameValues(this, nameValues);

  QString propertiesStr;

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
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos.p());

  CQChartsGeom::Point p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = p;
  else if (pos.units() == CQChartsUnits::PERCENT) {
    p1.setX(p.getX()*width ()/100.0);
    p1.setY(p.getY()*height()/100.0);
  }

  return CQChartsUtil::toQPoint(p1);
}

QPointF
CQChartsView::
positionToPixel(const CQChartsPosition &pos) const
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos.p());

  CQChartsGeom::Point p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = p;
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = windowToPixel(p);
  else if (pos.units() == CQChartsUnits::PERCENT) {
    p1.setX(p.getX()*width ()/100.0);
    p1.setY(p.getY()*height()/100.0);
  }

  return CQChartsUtil::toQPoint(p1);
}

//------

QRectF
CQChartsView::
rectToView(const CQChartsRect &rect) const
{
  CQChartsGeom::BBox r = CQChartsUtil::fromQRect(rect.rect());

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

  return CQChartsUtil::toQRect(r1);
}

QRectF
CQChartsView::
rectToPixel(const CQChartsRect &rect) const
{
  CQChartsGeom::BBox r = CQChartsUtil::fromQRect(rect.rect());

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

  return CQChartsUtil::toQRect(r1);
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
