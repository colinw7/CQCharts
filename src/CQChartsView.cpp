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
#include <CQChartsRotatedText.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsVariant.h>

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

  addProperty("", this, "title");
  addProperty("", this, "mode" );

  addProperty("", this, "antiAlias");

  addProperty("", this, "id"            )->setHidden(true);
  addProperty("", this, "currentPlotInd")->setHidden(true);

  addProperty("", this, "viewSizeHint")->setHidden(true);
  addProperty("", this, "zoomData"    )->setHidden(true);
  addProperty("", this, "bufferLayers")->setHidden(true);

  addProperty("", this, "showTable"   )->setHidden(true);
  addProperty("", this, "showSettings")->setHidden(true);

  addProperty("theme", this, "theme", "name")->
    setValues(QStringList() << "default" << "palette1" << "palette2");
  addProperty("theme", this, "dark" , "dark");

  addProperty("font", this, "scaleFont" , "scaled");
  addProperty("font", this, "fontFactor", "factor");

  addProperty("sizing", this, "autoSize" , "auto"     );
  addProperty("sizing", this, "fixedSize", "fixedSize");

  addProperty("background", this, "backgroundFillData"   , "fill"   );
  addProperty("background", this, "backgroundFillColor"  , "color"  );
  addProperty("background", this, "backgroundFillPattern", "pattern");

  addProperty("select"                 , this, "selectMode"         , "mode");
  addProperty("select"                 , this, "selectInside"       , "inside");
  addProperty("select/highlight"       , this, "selectedMode"       , "mode");
  addProperty("select/highlight"       , this, "selectedShapeData"  , "style");
  addProperty("select/highlight/stroke", this, "selectedBorder"     , "enabled");
  addProperty("select/highlight/stroke", this, "selectedBorderColor", "color");
  addProperty("select/highlight/stroke", this, "selectedBorderWidth", "width");
  addProperty("select/highlight/stroke", this, "selectedBorderDash" , "dash");
  addProperty("select/highlight/fill"  , this, "selectedFilled"     , "enabled");
  addProperty("select/highlight/fill"  , this, "selectedFillColor"  , "color");
  addProperty("select/highlight/fill"  , this, "selectedFillAlpha"  , "alpha");

  addProperty("inside/highlight"       , this, "insideMode"       , "mode");
  addProperty("inside/highlight"       , this, "insideShapeData"  , "style");
  addProperty("inside/highlight/stroke", this, "insideBorder"     , "enabled");
  addProperty("inside/highlight/stroke", this, "insideBorderColor", "color");
  addProperty("inside/highlight/stroke", this, "insideBorderWidth", "width");
  addProperty("inside/highlight/stroke", this, "insideBorderDash" , "dash");
  addProperty("inside/highlight/fill"  , this, "insideFilled"     , "enabled");
  addProperty("inside/highlight/fill"  , this, "insideFillColor"  , "color");
  addProperty("inside/highlight/fill"  , this, "insideFillAlpha"  , "alpha");

  addProperty("status", this, "posTextType", "posTextType");

  addProperty("scroll", this, "scrolled"      , "enabled" );
  addProperty("scroll", this, "scrollDelta"   , "delta"   );
  addProperty("scroll", this, "scrollNumPages", "numPages");
  addProperty("scroll", this, "scrollPage"    , "page"    );

  if (key())
    key()->addProperties(propertyModel(), "key");

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
}

CQChartsView::
~CQChartsView()
{
  if (charts_)
    charts_->removeView(this);

  //---

  delete ipainter_;
  delete image_;

  delete propertyModel_;

  delete keyObj_;

  delete displayRange_;

  for (auto &plot : plots_)
    delete plot;

  for (auto &annotation : annotations())
    delete annotation;

  for (auto &probeBand : probeBands_)
    delete probeBand;

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
setPosTextType(const PosTextType &t)
{
  CQChartsUtil::testAndSet(posTextType_, t, [&]() { updatePlots(); } );
}

//---

void
CQChartsView::
setPainterFont(QPainter *painter, const QFont &font) const
{
  painter->setFont(viewFont(font));
}

void
CQChartsView::
setPlotPainterFont(const CQChartsPlot *plot, QPainter *painter, const QFont &font) const
{
  painter->setFont(plotFont(plot, font));
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

CQChartsThemeObj *
CQChartsView::
themeObj()
{
  return theme().obj();
}

const CQChartsThemeObj *
CQChartsView::
themeObj() const
{
  return theme().obj();
}

const CQChartsTheme &
CQChartsView::
theme() const
{
  return charts()->plotTheme();
}

void
CQChartsView::
setTheme(const CQChartsTheme &theme)
{
  charts()->setPlotTheme(theme);

  updateTheme();
}

CQChartsGradientPalette *
CQChartsView::
interfacePalette() const
{
  return charts()->interfaceTheme().palette();
}

CQChartsGradientPalette *
CQChartsView::
themeGroupPalette(int i, int /*n*/) const
{
  return themeObj()->palette(i);
}

CQChartsGradientPalette *
CQChartsView::
themePalette(int ind) const
{
  return themeObj()->palette(ind);
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

CQPropertyViewItem *
CQChartsView::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  assert(CQUtil::hasProperty(object, name));

  return propertyModel()->addProperty(path, object, name, alias);
}

void
CQChartsView::
getPropertyNames(QStringList &names) const
{
  propertyModel()->objectNames(this, names);
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
addPolygonAnnotation(const QPolygonF &points)
{
  CQChartsPolygonAnnotation *polyAnnotation = new CQChartsPolygonAnnotation(this, points);

  addAnnotation(polyAnnotation);

  return polyAnnotation;
}

CQChartsPolylineAnnotation *
CQChartsView::
addPolylineAnnotation(const QPolygonF &points)
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
      plot->xAxis()->setSide(CQChartsAxis::Side::BOTTOM_LEFT);
      plot->xAxis()->setVisible(true);
    }

    if (plot->yAxis()) {
      plot->yAxis()->setSide(CQChartsAxis::Side::BOTTOM_LEFT);
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

  if (reset)
    resetConnections(/*notify*/false);

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

  //---

  firstPlot->applyDataRange();
}

void
CQChartsView::
initX1X2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset)
{
  if (reset)
    resetConnections(/*notify*/false);

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
      plot2->xAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);
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
  if (reset)
    resetConnections(/*notify*/false);

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
      plot2->yAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);
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
placePlots(const Plots &plots, bool vertical, bool horizontal, int rows, int columns)
{
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
interpPaletteColor(double r, bool scale) const
{
  return interpIndPaletteColor(0, r, scale);
}

QColor
CQChartsView::
interpIndPaletteColor(int ind, double r, bool scale) const
{
  return charts()->themePalette(ind)->getColor(r, scale);
}

QColor
CQChartsView::
interpGroupPaletteColor(int ig, int ng, double r, bool scale) const
{
  return charts()->themeGroupPalette(ig, ng)->getColor(r, scale);
}

QColor
CQChartsView::
interpThemeColor(double r) const
{
  return charts()->interfaceTheme().interpColor(r, /*scale*/true);
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
        CQChartsGeom::Point w1, w2;

        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(oldMovePoint        )), w1);
        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(mouseData_.movePoint)), w2);

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
            CQChartsGeom::Point w1, w2;

            plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.pressPoint), w1);
            plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.movePoint ), w2);

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
        CQChartsGeom::Point w1, w2;

        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPointF(mouseData_.pressPoint), w1);
        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPointF(mouseData_.movePoint ), w2);

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
        setMode(Mode::SELECT);
    }
    else if (mode() == Mode::PAN) {
      if (! mouseData_.pressed)
        setMode(Mode::SELECT);
    }
    else if (mode() == Mode::PROBE) {
      setMode(Mode::SELECT);
    }
    else if (mode() == Mode::QUERY) {
      setMode(Mode::SELECT);
    }
    else if (mode() == Mode::EDIT) {
      setMode(Mode::SELECT);
    }

    return;
  }
  else if (ke->key() == Qt::Key_Z) {
    setMode(Mode::ZOOM);

    return;
  }
  else if (ke->key() == Qt::Key_Bar) {
    setMode(Mode::PROBE);

    // TODO: do mouse move to show probe immediately or add probe move API

    return;
  }
  else if (ke->key() == Qt::Key_Q) {
    setMode(Mode::QUERY);

    return;
  }
  else if (ke->key() == Qt::Key_Insert) {
    setMode(Mode::EDIT);
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
    CQChartsGeom::Point w;

    plot->pixelToWindow(CQChartsUtil::fromQPoint(p), w);

    //---

    CQChartsPlot::ProbeData probeData;

    probeData.x = w.x;
    probeData.y = w.y;

    if (! plot->probe(probeData))
      continue;

    CQChartsGeom::BBox dataRange = plot->calcDataRange();

    if (probeData.direction == Qt::Vertical) {
      if (probeData.yvals.empty())
        probeData.yvals.emplace_back(w.y);

      // add probe lines from ymin to probed y values
      CQChartsGeom::Point p1;

      plot->windowToPixel(CQChartsGeom::Point(probeData.x, dataRange.getYMin()), p1);

      for (const auto &yval : probeData.yvals) {
        CQChartsGeom::Point p2;

        plot->windowToPixel(CQChartsGeom::Point(probeData.x, yval.value), p2);

        QString tip = (yval.label.length() ? yval.label : plot->yStr(yval.value));

        addVerticalProbeBand(probeInd, plot, tip, p1.x, p1.y, p2.y);
      }
    }
    else {
      if (probeData.xvals.empty())
        probeData.xvals.emplace_back(w.x);

      // add probe lines from xmin to probed x values
      CQChartsGeom::Point p1;

      plot->windowToPixel(CQChartsGeom::Point(dataRange.getXMin(), probeData.y), p1);

      for (const auto &xval : probeData.xvals) {
        CQChartsGeom::Point p2;

        plot->windowToPixel(CQChartsGeom::Point(xval.value, probeData.y), p2);

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
      CQChartsGeom::Point w;

      plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(pos)), w);

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
selectedObjs(Objs &objs) const
{
  for (auto &annotation : annotations()) {
    if (annotation->isSelected())
      objs.push_back(annotation);
  }
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

  setBrush(brush, true, interpBackgroundFillColor(0, 1),
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

#if 0
  // calc pen (stroke)
  if (stroked) {
    QColor color = strokeColor;

    color.setAlphaF(CMathUtil::clamp(strokeAlpha, 0.0, 1.0));

    pen.setColor(color);

    double width = lengthPixelWidth(strokeWidth);

    if (width > 0)
      pen.setWidthF(width);
    else
      pen.setWidthF(0.0);

    CQChartsUtil::penSetLineDash(pen, strokeDash);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }
#endif
}

void
CQChartsView::
setBrush(QBrush &brush, bool filled, const QColor &fillColor, double fillAlpha,
         const CQChartsFillPattern &pattern) const
{
  CQChartsUtil::setBrush(brush, filled, fillColor, fillAlpha, pattern);

#if 0
  // calc brush (fill)
  if (filled) {
    QColor color = fillColor;

    color.setAlphaF(CMathUtil::clamp(fillAlpha, 0.0, 1.0));

    brush.setColor(color);

    brush.setStyle(pattern.style());
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }
#endif
}

//------

void
CQChartsView::
drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
              const QPen &pen, const CQChartsTextOptions &options)
{
  painter->save();

  if (CMathUtil::isZero(options.angle)) {
    QFontMetricsF fm(painter->font());

    if (options.clipped)
      painter->setClipRect(rect);

    if (! options.contrast)
      painter->setPen(pen);

    //---

    QStringList strs;

    if (options.formatted)
      CQChartsUtil::formatStringInRect(text, painter->font(), rect, strs);
    else
      strs << text;

    //---

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i)
      tw = std::max(tw, fm.width(strs[i]));

    double th = strs.size()*fm.height();

    if (options.scaled) {
      double sx = (tw > 0 ? rect.width ()/tw : 1);
      double sy = (th > 0 ? rect.height()/th : 1);

      double s = std::min(sx, sy);

      double fs = painter->font().pointSizeF()*s;

      fs = CMathUtil::clamp(fs, options.minScaleFontSize, options.maxScaleFontSize);

      QFont font1 = painter->font();

      font1.setPointSizeF(fs);

      painter->setFont(font1);

      fm = QFontMetricsF(painter->font());

      th = strs.size()*fm.height();
    }

    //---

    double dy = 0.0;

    if      (options.align & Qt::AlignVCenter)
      dy = (rect.height() - th)/2.0;
    else if (options.align & Qt::AlignBottom)
      dy = rect.height() - th;

    double y = rect.top() + dy + fm.ascent();

    for (int i = 0; i < strs.size(); ++i) {
      double dx = 0.0;

      double tw = fm.width(strs[i]);

      if      (options.align & Qt::AlignHCenter)
         dx = (rect.width() - tw)/2;
      else if (options.align & Qt::AlignRight)
         dx = rect.width() - tw;

      double x = rect.left() + dx;

      if (options.contrast)
        drawContrastText(painter, x, y, strs[i], pen);
      else
        painter->drawText(x, y, strs[i]);

      y += fm.height();
    }
  }
  else {
    painter->setPen(pen);

    // TODO: support align and contrast
    CQChartsRotatedText::drawRotatedText(painter, rect.center().x(), rect.center().y(),
                                         text, options.angle, Qt::AlignHCenter | Qt::AlignVCenter,
                                         /*alignBox*/false);
  }

  painter->restore();
}

void
CQChartsView::
drawContrastText(QPainter *painter, double x, double y, const QString &text, const QPen &pen)
{
  CQChartsUtil::drawContrastText(painter, x, y, text, pen);
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
  //QPointF p = mapFromGlobal(QCursor::pos());

  //CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(p));
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

  QAction *dataTableAction = new QAction("Show Table");

  dataTableAction->setCheckable(true);
  dataTableAction->setChecked(isShowTable());

  connect(dataTableAction, SIGNAL(triggered(bool)), this, SLOT(setShowTable(bool)));

  popupMenu_->addAction(dataTableAction);

  //---

  QAction *viewSettingsAction = new QAction("Show Settings");

  viewSettingsAction->setCheckable(true);

  viewSettingsAction->setChecked(isShowSettings());

  connect(viewSettingsAction, SIGNAL(triggered(bool)), this, SLOT(setShowSettings(bool)));

  popupMenu_->addAction(viewSettingsAction);

  //---

  popupMenu_->addSeparator();

  //---

  QAction *autoResizeAction = new QAction("Auto Resize", popupMenu_);

  autoResizeAction->setCheckable(true);
  autoResizeAction->setChecked(isAutoSize());

  connect(autoResizeAction, SIGNAL(triggered(bool)), this, SLOT(setAutoSize(bool)));

  popupMenu_->addAction(autoResizeAction);

  if (! isAutoSize()) {
    QAction *resizeViewAction = new QAction("Resize to View", popupMenu_);

    connect(resizeViewAction, SIGNAL(triggered()), this, SLOT(resizeToView()));

    popupMenu_->addAction(resizeViewAction);
  }

  //---

  if (key() && basePlots.size() > 1) {
    QMenu *viewKeyMenu = new QMenu("View Key", popupMenu_);

    QAction *viewKeyAction = new QAction("Visible");

    viewKeyAction->setCheckable(true);
    viewKeyAction->setChecked(key()->isVisible());

    viewKeyMenu->addAction(viewKeyAction);

    connect(viewKeyAction, SIGNAL(triggered(bool)), this, SLOT(viewKeyVisibleSlot(bool)));

    //---

    using KeyLocationActionMap = std::map<CQChartsKeyLocation::Type, QAction *>;

    KeyLocationActionMap keyLocationActionMap;

    QMenu *keyLocationMenu = new QMenu("Location", viewKeyMenu);

    QActionGroup *keyLocationActionGroup = new QActionGroup(keyLocationMenu);

    auto addKeyLocationGroupAction =
     [&](const QString &label, const CQChartsKeyLocation::Type &location) {
      QAction *action = new QAction(label, keyLocationMenu);

      action->setCheckable(true);

      keyLocationActionMap[location] = action;

      keyLocationActionGroup->addAction(action);

      return action;
    };

    addKeyLocationGroupAction("Top Left"     , CQChartsKeyLocation::Type::TOP_LEFT     );
    addKeyLocationGroupAction("Top Center"   , CQChartsKeyLocation::Type::TOP_CENTER   );
    addKeyLocationGroupAction("Top Right"    , CQChartsKeyLocation::Type::TOP_RIGHT    );
    addKeyLocationGroupAction("Center Left"  , CQChartsKeyLocation::Type::CENTER_LEFT  );
    addKeyLocationGroupAction("Center Center", CQChartsKeyLocation::Type::CENTER_CENTER);
    addKeyLocationGroupAction("Center Right" , CQChartsKeyLocation::Type::CENTER_RIGHT );
    addKeyLocationGroupAction("Bottom Left"  , CQChartsKeyLocation::Type::BOTTOM_LEFT  );
    addKeyLocationGroupAction("Bottom Center", CQChartsKeyLocation::Type::BOTTOM_CENTER);
    addKeyLocationGroupAction("Bottom Right" , CQChartsKeyLocation::Type::BOTTOM_RIGHT );
    addKeyLocationGroupAction("Absolute"     , CQChartsKeyLocation::Type::ABS_POS      );

    keyLocationActionGroup->setExclusive(true);

    CQChartsKeyLocation::Type location = key()->location().type();

    keyLocationActionMap[location]->setChecked(true);

    connect(keyLocationActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(viewKeyPositionSlot(QAction *)));

    keyLocationMenu->addActions(keyLocationActionGroup->actions());

    viewKeyMenu->addMenu(keyLocationMenu);

    //---

    popupMenu_->addMenu(viewKeyMenu);
  }

  //---

  if (allPlots.size() == 1) {
    QAction *xRangeAction = new QAction("X Overview");
    QAction *yRangeAction = new QAction("Y Overview");

    xRangeAction->setCheckable(true);
    yRangeAction->setCheckable(true);

    xRangeAction->setChecked(window()->isXRangeMap());
    yRangeAction->setChecked(window()->isYRangeMap());

    connect(xRangeAction, SIGNAL(triggered(bool)), this, SLOT(xRangeMapSlot(bool)));
    connect(yRangeAction, SIGNAL(triggered(bool)), this, SLOT(yRangeMapSlot(bool)));

    popupMenu_->addAction(xRangeAction);
    popupMenu_->addAction(yRangeAction);
  }

  //---

  // Add plots
  if (allPlots.size() > 1) {
    QMenu *plotsMenu = new QMenu("Plots", popupMenu_);

    QActionGroup *plotsGroup = new QActionGroup(plotsMenu);

    for (const auto &plot : allPlots) {
      int ind = plotInd(plot);

      QAction *plotAction = new QAction(plot->id(), plotsMenu);

      plotAction->setCheckable(true);

      if (currentPlot)
        plotAction->setChecked(plot == currentPlot);
      else
        plotAction->setChecked(currentPlotInd() == ind);

      plotAction->setData(ind);

      plotsGroup->addAction(plotAction);

      connect(plotAction, SIGNAL(triggered()), this, SLOT(currentPlotSlot()));
    }

    plotsMenu->addActions(plotsGroup->actions());

    popupMenu_->addMenu(plotsMenu);
  }

  //------

  if (plotType && plotType->hasKey()) {
    QMenu *plotKeyMenu = new QMenu("Plot Key", popupMenu_);

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

    QMenu *keyLocationMenu = new QMenu("Location", plotKeyMenu);

    QActionGroup *keyLocationActionGroup = new QActionGroup(keyLocationMenu);

    auto addKeyLocationGroupAction =
     [&](const QString &label, const CQChartsKeyLocation::Type &location) {
      QAction *action = new QAction(label, keyLocationMenu);

      action->setCheckable(true);

      keyLocationActionMap[location] = action;

      keyLocationActionGroup->addAction(action);

      return action;
    };

    addKeyLocationGroupAction("Top Left"     , CQChartsKeyLocation::Type::TOP_LEFT     );
    addKeyLocationGroupAction("Top Center"   , CQChartsKeyLocation::Type::TOP_CENTER   );
    addKeyLocationGroupAction("Top Right"    , CQChartsKeyLocation::Type::TOP_RIGHT    );
    addKeyLocationGroupAction("Center Left"  , CQChartsKeyLocation::Type::CENTER_LEFT  );
    addKeyLocationGroupAction("Center Center", CQChartsKeyLocation::Type::CENTER_CENTER);
    addKeyLocationGroupAction("Center Right" , CQChartsKeyLocation::Type::CENTER_RIGHT );
    addKeyLocationGroupAction("Bottom Left"  , CQChartsKeyLocation::Type::BOTTOM_LEFT  );
    addKeyLocationGroupAction("Bottom Center", CQChartsKeyLocation::Type::BOTTOM_CENTER);
    addKeyLocationGroupAction("Bottom Right" , CQChartsKeyLocation::Type::BOTTOM_RIGHT );
    addKeyLocationGroupAction("Absolute"     , CQChartsKeyLocation::Type::ABS_POS      );

    keyLocationActionGroup->setExclusive(true);

    if (plotKey) {
      CQChartsKeyLocation::Type location = plotKey->location().type();

      keyLocationActionMap[location]->setChecked(true);
    }

    connect(keyLocationActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(plotKeyPositionSlot(QAction *)));

    keyLocationMenu->addActions(keyLocationActionGroup->actions());

    plotKeyMenu->addMenu(keyLocationMenu);

    //---

    bool insideXChecked = (plotKey && plotKey->isInsideX());
    bool insideYChecked = (plotKey && plotKey->isInsideY());

    (void) addKeyCheckAction("Inside X", insideXChecked, SLOT(plotKeyInsideXSlot(bool)));
    (void) addKeyCheckAction("Inside Y", insideYChecked, SLOT(plotKeyInsideYSlot(bool)));

    //---

    popupMenu_->addMenu(plotKeyMenu);
  }

  //------

  using AxisSideActionMap = std::map<CQChartsAxis::Side, QAction *>;

  //------

  if (plotType && plotType->hasAxes()) {
    QMenu *xAxisMenu = new QMenu("X Axis", popupMenu_);

    //---

    QAction *xAxisVisibleAction = new QAction("Visible", popupMenu_);

    xAxisVisibleAction->setCheckable(true);

    if (basePlot && basePlot->xAxis())
      xAxisVisibleAction->setChecked(basePlot->xAxis()->isVisible());

    connect(xAxisVisibleAction, SIGNAL(triggered(bool)), this, SLOT(xAxisVisibleSlot(bool)));

    xAxisMenu->addAction(xAxisVisibleAction);

    //---

    QAction *xAxisGridAction = new QAction("Grid", popupMenu_);

    xAxisGridAction->setCheckable(true);

    if (basePlot && basePlot->xAxis())
      xAxisGridAction->setChecked(basePlot->xAxis()->isAxesMajorGridLines());

    connect(xAxisGridAction, SIGNAL(triggered(bool)), this, SLOT(xAxisGridSlot(bool)));

    xAxisMenu->addAction(xAxisGridAction);

    //---

    AxisSideActionMap xAxisSideActionMap;

    QMenu *xAxisSideMenu = new QMenu("Side", xAxisMenu);

    QActionGroup *xAxisSideGroup = new QActionGroup(xAxisMenu);

    auto addXAxisSideGroupAction = [&](const QString &label, const CQChartsAxis::Side &side) {
      QAction *action = new QAction(label, xAxisSideMenu);

      action->setCheckable(true);

      xAxisSideActionMap[side] = action;

      xAxisSideGroup->addAction(action);

      return action;
    };

    addXAxisSideGroupAction("Bottom", CQChartsAxis::Side::BOTTOM_LEFT);
    addXAxisSideGroupAction("Top"   , CQChartsAxis::Side::TOP_RIGHT  );

    if (basePlot && basePlot->xAxis())
      xAxisSideActionMap[basePlot->xAxis()->side()]->setChecked(true);

    connect(xAxisSideGroup, SIGNAL(triggered(QAction *)), this, SLOT(xAxisSideSlot(QAction *)));

    xAxisSideMenu->addActions(xAxisSideGroup->actions());

    xAxisMenu->addMenu(xAxisSideMenu);

    //---

    popupMenu_->addMenu(xAxisMenu);

    //------

    QMenu *yAxisMenu = new QMenu("Y Axis", popupMenu_);

    //---

    QAction *yAxisVisibleAction = new QAction("Visible", popupMenu_);

    yAxisVisibleAction->setCheckable(true);

    if (basePlot && basePlot->yAxis())
      yAxisVisibleAction->setChecked(basePlot->yAxis()->isVisible());

    connect(yAxisVisibleAction, SIGNAL(triggered(bool)), this, SLOT(yAxisVisibleSlot(bool)));

    yAxisMenu->addAction(yAxisVisibleAction);

    //---

    QAction *yAxisGridAction = new QAction("Grid", popupMenu_);

    yAxisGridAction->setCheckable(true);

    if (basePlot && basePlot->yAxis())
      yAxisGridAction->setChecked(basePlot->yAxis()->isAxesMajorGridLines());

    connect(yAxisGridAction, SIGNAL(triggered(bool)), this, SLOT(yAxisGridSlot(bool)));

    yAxisMenu->addAction(yAxisGridAction);

    //---

    AxisSideActionMap yAxisSideActionMap;

    QMenu *yAxisSideMenu = new QMenu("Side", yAxisMenu);

    QActionGroup *yAxisSideGroup = new QActionGroup(yAxisMenu);

    auto addYAxisSideGroupAction = [&](const QString &label, const CQChartsAxis::Side &side) {
      QAction *action = new QAction(label, yAxisSideMenu);

      action->setCheckable(true);

      yAxisSideActionMap[side] = action;

      yAxisSideGroup->addAction(action);

      return action;
    };

    addYAxisSideGroupAction("Left" , CQChartsAxis::Side::BOTTOM_LEFT);
    addYAxisSideGroupAction("Right", CQChartsAxis::Side::TOP_RIGHT  );

    if (basePlot && basePlot->yAxis())
      yAxisSideActionMap[basePlot->yAxis()->side()]->setChecked(true);

    connect(yAxisSideGroup, SIGNAL(triggered(QAction *)), this, SLOT(yAxisSideSlot(QAction *)));

    yAxisSideMenu->addActions(yAxisSideGroup->actions());

    yAxisMenu->addMenu(yAxisSideMenu);

    //---

    popupMenu_->addMenu(yAxisMenu);
  }

  //---

  if (plotType && plotType->hasTitle()) {
    QMenu *titleMenu = new QMenu("Title", popupMenu_);

    QAction *titleVisibleAction = new QAction("Visible", popupMenu_);

    titleVisibleAction->setCheckable(true);

    if (basePlot && basePlot->title())
      titleVisibleAction->setChecked(basePlot->title()->isVisible());

    connect(titleVisibleAction, SIGNAL(triggered(bool)), this, SLOT(titleVisibleSlot(bool)));

    titleMenu->addAction(titleVisibleAction);

    //---

    using TitleLocationActionMap = std::map<CQChartsTitle::LocationType, QAction *>;

    TitleLocationActionMap titleLocationActionMap;

    QMenu *titleLocationMenu = new QMenu("Location", titleMenu);

    QActionGroup *titleLocationGroup = new QActionGroup(titleMenu);

    auto addTitleLocationGroupAction =
     [&](const QString &label, const CQChartsTitle::LocationType &location) {
      QAction *action = new QAction(label, titleLocationMenu);

      action->setCheckable(true);

      titleLocationActionMap[location] = action;

      titleLocationGroup->addAction(action);

      return action;
    };

    addTitleLocationGroupAction("Top"     , CQChartsTitle::LocationType::TOP    );
    addTitleLocationGroupAction("Center"  , CQChartsTitle::LocationType::CENTER );
    addTitleLocationGroupAction("Bottom"  , CQChartsTitle::LocationType::BOTTOM );
    addTitleLocationGroupAction("Absolute", CQChartsTitle::LocationType::ABS_POS);

    if (basePlot && basePlot->title())
      titleLocationActionMap[basePlot->title()->location()]->setChecked(true);

    connect(titleLocationGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(titleLocationSlot(QAction *)));

    titleLocationMenu->addActions(titleLocationGroup->actions());

    titleMenu->addMenu(titleLocationMenu);

    //---

    popupMenu_->addMenu(titleMenu);
  }

  //------

  QAction *invertXAction = new QAction("Invert X", popupMenu_);
  QAction *invertYAction = new QAction("Invert Y", popupMenu_);

  invertXAction->setCheckable(true);
  invertYAction->setCheckable(true);

  if (basePlot) {
    invertXAction->setChecked(basePlot->isInvertX());
    invertYAction->setChecked(basePlot->isInvertY());
  }

  connect(invertXAction, SIGNAL(triggered(bool)), this, SLOT(invertXSlot(bool)));
  connect(invertYAction, SIGNAL(triggered(bool)), this, SLOT(invertYSlot(bool)));

  popupMenu_->addAction(invertXAction);
  popupMenu_->addAction(invertYAction);

  //------

  QAction *fitAction = new QAction("Fit", popupMenu_);

  connect(fitAction, SIGNAL(triggered()), this, SLOT(fitSlot()));

  popupMenu_->addAction(fitAction);

  //---

  QMenu *themeMenu = new QMenu("Theme", popupMenu_);

  QActionGroup *interfaceGroup = new QActionGroup(themeMenu);
  QActionGroup *themeGroup     = new QActionGroup(themeMenu);

  auto addInterfaceAction =
   [&](const QString &label, const char *slotName) {
    QAction *action = new QAction(label, themeMenu);

    action->setCheckable(true);

    interfaceGroup->addAction(action);

    connect(action, SIGNAL(triggered()), this, slotName);
    return action;
  };

  auto addThemeAction =
   [&](const QString &label, const char *slotName) {
    QAction *action = new QAction(label, themeMenu);

    action->setCheckable(true);

    themeGroup->addAction(action);

    connect(action, SIGNAL(triggered()), this, slotName);
    return action;
  };

  QAction *lightPaletteAction = addInterfaceAction("Light", SLOT(lightPaletteSlot()));
  QAction *darkPaletteAction  = addInterfaceAction("Dark" , SLOT(darkPaletteSlot()));

  lightPaletteAction->setChecked(! isDark());
  darkPaletteAction ->setChecked(isDark());

  themeMenu->addActions(interfaceGroup->actions());

  QAction *defaultThemeAction = addThemeAction("Default"  , SLOT(defaultThemeSlot()));
  QAction *palette1Action     = addThemeAction("Palette 1", SLOT(palette1Slot()));
  QAction *palette2Action     = addThemeAction("Palette 2", SLOT(palette2Slot()));

  defaultThemeAction->setChecked(theme().name() == "default" );
  palette1Action    ->setChecked(theme().name() == "palette1");
  palette2Action    ->setChecked(theme().name() == "palette2");

  themeMenu->addActions(themeGroup->actions());

  popupMenu_->addMenu(themeMenu);

  //---

  // add Menus for current plot

  if (currentPlot) {
    if (currentPlot->addMenuItems(popupMenu_))
      popupMenu_->addSeparator();
  }

  //---

  QMenu *printMenu = new QMenu("Print", popupMenu_);

  QAction *pngAction = new QAction("PNG", popupMenu_);
  QAction *svgAction = new QAction("SVG", popupMenu_);

  printMenu->addAction(pngAction);
  printMenu->addAction(svgAction);

  connect(pngAction, SIGNAL(triggered()), this, SLOT(printPNGSlot()));
  connect(svgAction, SIGNAL(triggered()), this, SLOT(printSVGSlot()));

  popupMenu_->addMenu(printMenu);

  //---

  if (CQChartsEnv::getBool("CQ_CHARTS_DEBUG", true)) {
    QAction *showBoxesAction = new QAction("Show Boxes", popupMenu_);

    showBoxesAction->setCheckable(true);

    if (basePlot)
      showBoxesAction->setChecked(basePlot->showBoxes());

    popupMenu_->addAction(showBoxesAction);

    connect(showBoxesAction, SIGNAL(triggered(bool)), this, SLOT(showBoxesSlot(bool)));

    //---

    QAction *bufferLayersAction = new QAction("Buffer Layers", popupMenu_);

    bufferLayersAction->setCheckable(true);
    bufferLayersAction->setChecked(isBufferLayers());

    popupMenu_->addAction(bufferLayersAction);

    connect(bufferLayersAction, SIGNAL(triggered(bool)), this, SLOT(bufferLayersSlot(bool)));
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

  if      (action->text() == "Top Left"     )
    viewKey->setLocation(CQChartsKeyLocation::Type::TOP_LEFT     );
  else if (action->text() == "Top Center"   )
    viewKey->setLocation(CQChartsKeyLocation::Type::TOP_CENTER   );
  else if (action->text() == "Top Right"    )
    viewKey->setLocation(CQChartsKeyLocation::Type::TOP_RIGHT    );
  else if (action->text() == "Center Left"  )
    viewKey->setLocation(CQChartsKeyLocation::Type::CENTER_LEFT  );
  else if (action->text() == "Center Center")
    viewKey->setLocation(CQChartsKeyLocation::Type::CENTER_CENTER);
  else if (action->text() == "Center Right" )
    viewKey->setLocation(CQChartsKeyLocation::Type::CENTER_RIGHT );
  else if (action->text() == "Bottom Left"  )
    viewKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_LEFT  );
  else if (action->text() == "Bottom Center")
    viewKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_CENTER);
  else if (action->text() == "Bottom Right" )
    viewKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_RIGHT );
  else if (action->text() == "Absolute"     )
    viewKey->setLocation(CQChartsKeyLocation::Type::ABS_POS      );
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
    if      (action->text() == "Top Left"     )
      plotKey->setLocation(CQChartsKeyLocation::Type::TOP_LEFT     );
    else if (action->text() == "Top Center"   )
      plotKey->setLocation(CQChartsKeyLocation::Type::TOP_CENTER   );
    else if (action->text() == "Top Right"    )
      plotKey->setLocation(CQChartsKeyLocation::Type::TOP_RIGHT    );
    else if (action->text() == "Center Left"  )
      plotKey->setLocation(CQChartsKeyLocation::Type::CENTER_LEFT  );
    else if (action->text() == "Center Center")
      plotKey->setLocation(CQChartsKeyLocation::Type::CENTER_CENTER);
    else if (action->text() == "Center Right" )
      plotKey->setLocation(CQChartsKeyLocation::Type::CENTER_RIGHT );
    else if (action->text() == "Bottom Left"  )
      plotKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_LEFT  );
    else if (action->text() == "Bottom Center")
      plotKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_CENTER);
    else if (action->text() == "Bottom Right" )
      plotKey->setLocation(CQChartsKeyLocation::Type::BOTTOM_RIGHT );
    else if (action->text() == "Absolute"     )
      plotKey->setLocation(CQChartsKeyLocation::Type::ABS_POS      );
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

  if (basePlot && basePlot->xAxis())
    basePlot->xAxis()->setVisible(b);
}

void
CQChartsView::
xAxisGridSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot && basePlot->xAxis())
    basePlot->xAxis()->setAxesMajorGridLines(b);
}

void
CQChartsView::
xAxisSideSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot && basePlot->xAxis()) {
    if      (action->text() == "Bottom")
      basePlot->xAxis()->setSide(CQChartsAxis::Side::BOTTOM_LEFT);
    else if (action->text() == "Top")
      basePlot->xAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);
  }
}

void
CQChartsView::
yAxisVisibleSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot && basePlot->yAxis())
    basePlot->yAxis()->setVisible(b);
}

void
CQChartsView::
yAxisGridSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot && basePlot->yAxis())
    basePlot->yAxis()->setAxesMajorGridLines(b);
}

void
CQChartsView::
yAxisSideSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot && basePlot->yAxis()) {
    if      (action->text() == "Left")
      basePlot->yAxis()->setSide(CQChartsAxis::Side::BOTTOM_LEFT);
    else if (action->text() == "Right")
      basePlot->yAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);
  }
}

void
CQChartsView::
titleVisibleSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot && basePlot->yAxis())
    basePlot->title()->setVisible(b);
}

void
CQChartsView::
titleLocationSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  CQChartsPlot *basePlot = (currentPlot ? currentPlot->firstPlot() : nullptr);

  if (basePlot && basePlot->title()) {
    if      (action->text() == "Top")
      basePlot->title()->setLocation(CQChartsTitle::LocationType::TOP);
    else if (action->text() == "Center")
      basePlot->title()->setLocation(CQChartsTitle::LocationType::CENTER);
    else if (action->text() == "Bottom")
      basePlot->title()->setLocation(CQChartsTitle::LocationType::BOTTOM);
    else if (action->text() == "Absolute")
      basePlot->title()->setLocation(CQChartsTitle::LocationType::ABS_POS);
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
defaultThemeSlot()
{
  themeSlot("default");
}

void
CQChartsView::
palette1Slot()
{
  themeSlot("palette1");
}

void
CQChartsView::
palette2Slot()
{
  themeSlot("palette2");
}

void
CQChartsView::
themeSlot(const QString &name)
{
  setTheme(CQChartsTheme(name));

  updateTheme();
}

void
CQChartsView::
updateTheme()
{
  setSelectedFillColor(themeObj()->selectColor());

  setInsideFillColor  (themeObj()->insideColor());
  setInsideBorderWidth(CQChartsLength("2px"));

  updatePlots();

  update();

  emit themePalettesChanged();
}

bool
CQChartsView::
isDark() const
{
  return charts()->interfaceTheme().isDark();
}

void
CQChartsView::
setDark(bool b)
{
  charts()->interfaceTheme().setDark(b);

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
    plot->queueDrawObjs();
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
  double dx = scrollData_.page*scrollData_.delta;

  double vr = viewportRange();

  displayRange_->setWindowRange(dx, 0, dx + vr, vr);

  update();
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
windowToPixel(double wx, double wy, double &px, double &py) const
{
  displayRange_->windowToPixel(wx, wy, &px, &py);
}

void
CQChartsView::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  displayRange_->pixelToWindow(px, py, &wx, &wy);
}

CQChartsGeom::Point
CQChartsView::
windowToPixel(const CQChartsGeom::Point &w) const
{
  CQChartsGeom::Point p;

  windowToPixel(w.x, w.y, p.x, p.y);

  return p;
}

CQChartsGeom::Point
CQChartsView::
pixelToWindow(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::Point w;

  pixelToWindow(p.x, p.y, w.x, w.y);

  return w;
}

CQChartsGeom::BBox
CQChartsView::
windowToPixel(const CQChartsGeom::BBox &wrect) const
{
  double px1, py1, px2, py2;

  windowToPixel(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixel(wrect.getXMax(), wrect.getYMax(), px2, py1);

  return CQChartsGeom::BBox(px1, py1, px2, py2);
}

CQChartsGeom::BBox
CQChartsView::
pixelToWindow(const CQChartsGeom::BBox &prect) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(prect.getXMin(), prect.getYMin(), wx1, wy2);
  pixelToWindow(prect.getXMax(), prect.getYMax(), wx2, wy1);

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

  pixelToWindow( 0, 0, wx1, wy1);
  pixelToWindow(pw, 0, wx2, wy2);

  return std::abs(wx2 - wx1);
}

double
CQChartsView::
pixelToWindowHeight(double ph) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(0, 0 , wx1, wy1);
  pixelToWindow(0, ph, wx2, wy2);

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

  windowToPixel( 0, 0, px1, py1);
  windowToPixel(ww, 0, px2, py2);

  return std::abs(px2 - px1);
}

double
CQChartsView::
windowToPixelHeight(double wh) const
{
  double px1, py1, px2, py2;

  windowToPixel(0, 0 , px1, py1);
  windowToPixel(0, wh, px2, py2);

  return std::abs(py2 - py1);
}

//------

QSize
CQChartsView::
sizeHint() const
{
  return viewSizeHint();
}
