#include <CQChartsView.h>
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
#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QSvgGenerator>
#include <QFileDialog>
#include <QRubberBand>
#include <QMouseEvent>
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

  bufferLayers_ = CQChartsEnv::getBool("CQCHARTS_BUFFER_LAYERS", bufferLayers_);

  //---

  displayRange_ = new CQChartsDisplayRange;

  double vr = viewportRange();

  displayRange_->setWindowRange(0, 0, vr, vr);

  //---

  propertyModel_ = new CQPropertyViewModel;

  keyObj_ = new CQChartsViewKey(this);

  keyObj_->setVisible(false);

  //---

  setSelectedMode(HighlightDataMode::FILL);

  //---

  interfacePalette_ = new CQChartsGradientPalette;

  lightPaletteSlot();

  themeSlot("default");

  //---

  addProperty("", this, "id"            );
  addProperty("", this, "title"         );
  addProperty("", this, "currentPlotInd");
  addProperty("", this, "viewSizeHint"  );
  addProperty("", this, "mode"          );
  addProperty("", this, "selectMode"    );
  addProperty("", this, "theme"         )->
    setValues(QStringList() << "default" << "palette1" << "palette2");
  addProperty("", this, "dark"          );
  addProperty("", this, "zoomData"      );
  addProperty("", this, "antiAlias"     );
  addProperty("", this, "bufferLayers"  );
  addProperty("", this, "scaleFont"     );
  addProperty("", this, "fontFactor"    );
  addProperty("", this, "posTextType"   );

  addProperty("background", this, "backgroundFillColor"  , "color" );
  addProperty("background", this, "backgroundFillPattern", "pattern");

  addProperty("selectedHighlight"       , this, "selectedMode"       , "mode");
  addProperty("selectedHighlight/stroke", this, "selectedBorder"     , "enabled");
  addProperty("selectedHighlight/stroke", this, "selectedBorderColor", "color");
  addProperty("selectedHighlight/stroke", this, "selectedBorderWidth", "width");
  addProperty("selectedHighlight/stroke", this, "selectedBorderDash" , "dash");
  addProperty("selectedHighlight/fill"  , this, "selectedFilled"     , "enabled");
  addProperty("selectedHighlight/fill"  , this, "selectedFillColor"  , "color");

  addProperty("insideHighlight"       , this, "insideMode"       , "mode");
  addProperty("insideHighlight/stroke", this, "insideBorder"     , "enabled");
  addProperty("insideHighlight/stroke", this, "insideBorderColor", "color");
  addProperty("insideHighlight/stroke", this, "insideBorderWidth", "width");
  addProperty("insideHighlight/stroke", this, "insideBorderDash" , "dash");
  addProperty("insideHighlight/fill"  , this, "insideFilled"     , "enabled");
  addProperty("insideHighlight/fill"  , this, "insideFillColor"  , "color");

  addProperty("scroll", this, "scrolled"      , "enabled" );
  addProperty("scroll", this, "scrollDelta"   , "delta"   );
  addProperty("scroll", this, "scrollNumPages", "numPages");
  addProperty("scroll", this, "scrollPage"    , "page"    );

  key()->addProperties(propertyModel(), "key");

  //---

  CQToolTip::setToolTip(this, new CQChartsViewToolTip(this));

  //---

  searchTimer_.setInterval(10);
  searchTimer_.setSingleShot(true);

  connect(&searchTimer_, SIGNAL(timeout()), this, SLOT(searchSlot()));
}

CQChartsView::
~CQChartsView()
{
  if (charts_)
    charts_->removeView(this);

  //---

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

  delete interfacePalette_;

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

void
CQChartsView::
setScaleFont(bool b)
{
  CQChartsUtil::testAndSet(scaleFont_, b, [&]() { updatePlots(); } );
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
  CQChartsUtil::testAndSet(currentPlotInd_, ind, [&]() { emit currentPlotChanged(); } );
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
  for (auto &plot : plots_)
    plot->deselectAll();

  for (auto &annotation : annotations())
    annotation->setSelected(false);
}

void
CQChartsView::
setTheme(const CQChartsTheme &theme)
{
  CQChartsUtil::testAndSet(theme_, theme, [&]() { updateTheme(); } );
}

CQChartsGradientPalette *
CQChartsView::
themeGroupPalette(int i, int /*n*/) const
{
  return themeObj()->palette(i);
}

//---

bool
CQChartsView::
setProperties(const QString &properties)
{
  bool rc = true;

  QStringList strs = properties.split(",", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    QString str = strs[i].simplified();

    int pos = str.indexOf("=");

    QString name  = str.mid(0, pos).simplified();
    QString value = str.mid(pos + 1).simplified();

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
getProperty(const QString &name, QVariant &value)
{
  return propertyModel()->getProperty(this, name, value);
}

CQPropertyViewItem *
CQChartsView::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  return propertyModel()->addProperty(path, object, name, alias);
}

void
CQChartsView::
getPropertyNames(QStringList &names) const
{
  propertyModel()->objectNames(const_cast<CQChartsView *>(this), names);
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
addRectAnnotation(const CQChartsPosition &start, const CQChartsPosition &end)
{
  CQChartsRectAnnotation *rectAnnotation = new CQChartsRectAnnotation(this, start, end);

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

  connect(annotation, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

  annotation->addProperties(propertyModel(), "annotations");
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
}

void
CQChartsView::
removeAllAnnotations()
{
  for (auto &annotation : annotations_)
    delete annotation;

  annotations_.clear();
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
  CQChartsGradientPalette *palette = this->themePalette();

  QColor c = palette->getColor(r, scale);

  return c;
}

QColor
CQChartsView::
interpIndPaletteColor(int ind, double r, bool scale) const
{
  CQChartsGradientPalette *palette = this->themePalette(ind);

  QColor c = palette->getColor(r, scale);

  return c;
}

QColor
CQChartsView::
interpThemeColor(double r) const
{
  //CQChartsView *th = const_cast<CQChartsView *>(this);

  QColor c = interfacePalette()->getColor(r, /*scale*/true);

  return c;
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

  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if (me->button() == Qt::LeftButton) {
    if      (mode() == Mode::SELECT) {
      if (selectMode_ == SelectMode::POINT) {
        CQChartsScopeGuard updateSelTextGuard([&]() { updateSelText(); });

        //---

        for (const auto &annotation : annotations()) {
          if (annotation->contains(w)) {
            if (annotation->selectPress(w)) {
              emit annotationPressed  (annotation);
              emit annotationIdPressed(annotation->id());

              return;
            }
          }
        }

        //---

        CQChartsSelMod selMod = modifiersToSelMod(me->modifiers());

        if (mouseData_.plot && mouseData_.plot->selectMousePress(me->pos(), selMod)) {
          setCurrentPlot(mouseData_.plot);
          return;
        }

        for (auto &plot : mouseData_.plots) {
          if (plot == mouseData_.plot) continue;

          if (plot->selectMousePress(me->pos(), selMod)) {
            setCurrentPlot(plot);
            return;
          }
        }

        //---

        CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

        if (keyObj_->isInside(w))
          keyObj_->selectPress(w, SelMod::REPLACE);
      }
      else {
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
    else if (mode() == Mode::EDIT) {
      (void) editMousePress(me->pos());
    }
  }
  else if (me->button() == Qt::MiddleButton) {
  }
  else if (me->button() == Qt::RightButton) {
    mouseData_.pressed    = false;
    mouseData_.pressPoint = me->globalPos();

    showMenu(me->pos());

    return;
  }
}

bool
CQChartsView::
editMousePress(const QPointF &p)
{
  if (mouseData_.plot) {
    if (mouseData_.plot->editMousePress(p))
      return true;
  }

  for (auto &plot : mouseData_.plots) {
    if (plot == mouseData_.plot) continue;

    if (plot->editMousePress(p))
      return true;
  }

  deselectAll();

  return false;
}

void
CQChartsView::
mouseMoveEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

  // select mode and move (not pressed) - update plot positions
  if (mode() == Mode::SELECT && ! mouseData_.pressed) {
    updatePosText(me->pos());

    //---

    searchPos_ = me->pos();

    searchTimer_.start();

    return;
  }

  //---

  if (mode() == Mode::ZOOM) {
    updatePosText(me->pos());
  }

  //---

  // probe mode and move (pressed or not pressed) - show probe lines
  if (mode() == Mode::PROBE) {
    showProbeLines(me->pos());

    return;
  }

  //---

  if (! mouseData_.pressed) {
    if (mode() == Mode::EDIT)
      editMouseMotion(me->pos());

    return;
  }

  // get plots are point
  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if      (mouseData_.button == Qt::LeftButton) {
    // select plot object
    if      (mode() == Mode::SELECT) {
      if (selectMode_ == SelectMode::POINT) {
        if (mouseData_.plot) {
          (void) mouseData_.plot->selectMouseMove(searchPos_, true);
        }

        for (auto &plot : mouseData_.plots) {
          if (plot == mouseData_.plot) continue;

          if (plot->selectMouseMove(searchPos_, false))
            return;
        }
      }
      else {
        mouseData_.movePoint = me->pos();

        if (mouseData_.escape)
          endRegionBand();
        else
          updateRegionBand(mouseData_.pressPoint, mouseData_.movePoint);
      }

      searchPos_ = me->pos();
    }
    // draw zoom rectangle
    else if (mode() == Mode::ZOOM) {
      mouseData_.movePoint = me->pos();

      if      (mouseData_.escape)
        endRegionBand();
      else if (mouseData_.plot)
        updateRegionBand(mouseData_.plot, mouseData_.pressPoint, mouseData_.movePoint);
    }
    else if (mode() == Mode::PAN) {
      if (mouseData_.plot) {
        CQChartsGeom::Point w1, w2;

        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(mouseData_.movePoint)), w1);
        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos()           )), w2);

        double dx = w1.x - w2.x;
        double dy = w1.y - w2.y;

        mouseData_.plot->pan(dx, dy);

        mouseData_.movePoint = me->pos();
      }
    }
    else if (mode() == Mode::PROBE) {
    }
    else if (mode() == Mode::EDIT) {
      if (mouseData_.plot)
        (void) mouseData_.plot->editMouseMove(me->pos(), true);

      for (auto &plot : mouseData_.plots) {
        if (plot == mouseData_.plot) continue;

        if (plot->editMouseMove(me->pos(), false))
          return;
      }
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

    if (probeData.direction == CQChartsPlot::ProbeData::Direction::VERTICAL) {
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

void
CQChartsView::
mouseReleaseEvent(QMouseEvent *me)
{
  if (isPreview())
    return;

  CQChartsScopeGuard resetMouseData([&]() { mouseData_.reset(); });

  //CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

  if      (mouseData_.button == Qt::LeftButton) {
    if      (mode() == Mode::SELECT) {
      if (! mouseData_.pressed)
        return;

      if (selectMode_ == SelectMode::POINT) {
        if (mouseData_.plot)
          mouseData_.plot->selectMouseRelease(me->pos());
      }
      else {
        CQChartsScopeGuard updateSelTextGuard([&]() { updateSelText(); });

        mouseData_.movePoint = me->pos();

        endRegionBand();

        CQChartsSelMod selMod = modifiersToSelMod(me->modifiers());

        if (mouseData_.plot) {
          CQChartsGeom::Point w1, w2;

          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.pressPoint), w1);
          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.movePoint ), w2);

          if (mouseData_.plot->rectSelect(CQChartsGeom::BBox(w1, w2), selMod))
            return;
        }

        for (auto &plot : mouseData_.plots) {
          if (plot == mouseData_.plot) continue;

          CQChartsGeom::Point w1, w2;

          plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.pressPoint), w1);
          plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.movePoint ), w2);

          if (plot->rectSelect(CQChartsGeom::BBox(w1, w2), selMod))
            return;
        }
      }
    }
    else if (mode() == Mode::ZOOM) {
      if (! mouseData_.pressed)
        return;

      mouseData_.movePoint = me->pos();

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
    else if (mode() == Mode::EDIT) {
      if (! mouseData_.pressed)
        return;

      if (mouseData_.plot)
        mouseData_.plot->editMouseRelease(me->pos());

      for (auto &plot : mouseData_.plots) {
        if (plot == mouseData_.plot) continue;

        plot->editMouseRelease(me->pos());
      }
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
  }
  else if (mouseData_.button == Qt::RightButton) {
  }
}

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
  else if (ke->key() == Qt::Key_Insert) {
    setMode(Mode::EDIT);
  }
  else if (ke->key() == Qt::Key_Tab) {
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

  deselectAll();

  selObj->setSelected(true);
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

    PlotSet plots;

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

  int num = 0;

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

//------

void
CQChartsView::
resizeEvent(QResizeEvent *)
{
  prect_ = CQChartsGeom::BBox(0, 0, width(), height());

  if (prect().getHeight() > 0)
    aspect_ = (1.0*prect().getWidth())/prect().getHeight();
  else
    aspect_ = 1.0;

  displayRange_->setPixelRange(prect_.getXMin(), prect_.getYMin(),
                               prect_.getXMax(), prect_.getYMax());

  //---

  for (const auto &plot : plots_)
    plot->handleResize();
}

void
CQChartsView::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  paint(&painter);
}

void
CQChartsView::
paint(QPainter *painter, CQChartsPlot *plot)
{
  if (isAntiAlias())
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //---

  QBrush brush;

  setBrush(brush, true, interpBackgroundFillColor(0, 1),
           backgroundFillAlpha(), backgroundFillPattern());

  painter->fillRect(CQChartsUtil::toQRect(prect_), brush);

  //---

  if (plot) {
    plot->draw(painter);
  }
  else {
    for (const auto &plot : plots_) {
      if (plot->isVisible())
        plot->draw(painter);
    }

    //---

    // TODO: allow use extra layer for foreground (annotations, key)

    for (auto &annotation : annotations())
      annotation->draw(painter);

    //---

    key()->draw(painter);
  }
}

//------

void
CQChartsView::
setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha,
       const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash) const
{
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
}

void
CQChartsView::
setBrush(QBrush &brush, bool filled, const QColor &fillColor, double fillAlpha,
         const CQChartsFillPattern &pattern) const
{
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
  QColor icolor = CQChartsUtil::invColor(pen.color());

  icolor.setAlphaF(0.5);

  painter->setPen(icolor);

  painter->drawText(QPointF(x + 1, y + 1), text);

  painter->setPen(pen);

  painter->drawText(QPointF(x, y), text);
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
  QPointF p = mapFromGlobal(QCursor::pos());

  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(p));

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  setStatusText("");

  bool handled = false;

  // process current plot first
  if (mouseData_.plot) {
    CQChartsGeom::Point w;

    mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(searchPos_), w);

    if (mouseData_.plot->selectMove(w, ! handled))
      handled = true;
  }

  // process other plots
  for (auto &plot : mouseData_.plots) {
    if (plot == mouseData_.plot) continue;

    CQChartsGeom::Point w;

    plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(searchPos_)), w);

    if (plot->selectMove(w, ! handled))
      handled = true;
  }
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

  // get current plot
  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(p));

  Plots         plots;
  CQChartsPlot* plot { nullptr };

  plotsAt(w, plots, plot);

  if (plot)
    setCurrentPlot(plot);

  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/false);

  CQChartsPlotType *plotType = (currentPlot ? currentPlot->type() : nullptr);

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
    QMenu *keyMenu = new QMenu("Key", popupMenu_);

    //---

    auto addKeyCheckAction = [&](const QString &label, bool checked, const char *slot) {
      QAction *action = new QAction(label, keyMenu);

      action->setCheckable(true);
      action->setChecked(checked);

      connect(action, SIGNAL(triggered(bool)), this, slot);

      keyMenu->addAction(action);

      return action;
    };

    //---

    bool visibleChecked = (currentPlot && currentPlot->key() && currentPlot->key()->isVisible());

    QAction *keyVisibleAction =
      addKeyCheckAction("Visible", visibleChecked, SLOT(keyVisibleSlot(bool)));

    if (currentPlot && currentPlot->key()) {
      if (currentPlot->key()->isEmpty())
        keyVisibleAction->setEnabled(false);
    }
    else
      keyVisibleAction->setEnabled(false);

    //---

    using KeyLocationActionMap = std::map<CQChartsKeyLocation::Type, QAction *>;

    KeyLocationActionMap keyLocationActionMap;

    QMenu *keyLocationMenu = new QMenu("Location", keyMenu);

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

    if (currentPlot && currentPlot->key()) {
      CQChartsKeyLocation::Type location = currentPlot->key()->location().type();

      keyLocationActionMap[location]->setChecked(true);
    }

    connect(keyLocationActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(keyPositionSlot(QAction *)));

    keyLocationMenu->addActions(keyLocationActionGroup->actions());

    keyMenu->addMenu(keyLocationMenu);

    //---

    bool insideXChecked = (currentPlot && currentPlot->key() && currentPlot->key()->isInsideX());
    bool insideYChecked = (currentPlot && currentPlot->key() && currentPlot->key()->isInsideY());

    (void) addKeyCheckAction("Inside X", insideXChecked, SLOT(keyInsideXSlot(bool)));
    (void) addKeyCheckAction("Inside Y", insideYChecked, SLOT(keyInsideYSlot(bool)));

    //---

    popupMenu_->addMenu(keyMenu);
  }

  //------

  using AxisSideActionMap = std::map<CQChartsAxis::Side, QAction *>;

  //------

  if (plotType && plotType->hasAxes()) {
    QMenu *xAxisMenu = new QMenu("X Axis", popupMenu_);

    //---

    QAction *xAxisVisibleAction = new QAction("Visible", popupMenu_);

    xAxisVisibleAction->setCheckable(true);

    if (currentPlot && currentPlot->xAxis())
      xAxisVisibleAction->setChecked(currentPlot->xAxis()->isVisible());

    connect(xAxisVisibleAction, SIGNAL(triggered(bool)), this, SLOT(xAxisVisibleSlot(bool)));

    xAxisMenu->addAction(xAxisVisibleAction);

    //---

    QAction *xAxisGridAction = new QAction("Grid", popupMenu_);

    xAxisGridAction->setCheckable(true);

    if (currentPlot && currentPlot->xAxis())
      xAxisGridAction->setChecked(currentPlot->xAxis()->isAxesMajorGridLines());

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

    if (currentPlot && currentPlot->xAxis())
      xAxisSideActionMap[currentPlot->xAxis()->side()]->setChecked(true);

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

    if (currentPlot && currentPlot->yAxis())
      yAxisVisibleAction->setChecked(currentPlot->yAxis()->isVisible());

    connect(yAxisVisibleAction, SIGNAL(triggered(bool)), this, SLOT(yAxisVisibleSlot(bool)));

    yAxisMenu->addAction(yAxisVisibleAction);

    //---

    QAction *yAxisGridAction = new QAction("Grid", popupMenu_);

    yAxisGridAction->setCheckable(true);

    if (currentPlot && currentPlot->yAxis())
      yAxisGridAction->setChecked(currentPlot->yAxis()->isAxesMajorGridLines());

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

    if (currentPlot && currentPlot->yAxis())
      yAxisSideActionMap[currentPlot->yAxis()->side()]->setChecked(true);

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

    if (currentPlot && currentPlot->title())
      titleVisibleAction->setChecked(currentPlot->title()->isVisible());

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

    if (currentPlot && currentPlot->title())
      titleLocationActionMap[currentPlot->title()->location()]->setChecked(true);

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

  if (currentPlot) {
    invertXAction->setChecked(currentPlot->isInvertX());
    invertYAction->setChecked(currentPlot->isInvertY());
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

  if (CQChartsEnv::getBool("CQCHARTS_DEBUG", true)) {
    QAction *showBoxesAction = new QAction("Show Boxes", popupMenu_);

    showBoxesAction->setCheckable(true);

    if (currentPlot)
      showBoxesAction->setChecked(currentPlot->showBoxes());

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

void
CQChartsView::
keyVisibleSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (! currentPlot || ! currentPlot->key())
    return;

  if (b != currentPlot->key()->isVisible()) {
    currentPlot->key()->setVisible(b);

    if (b)
      currentPlot->updateKeyPosition(/*force*/true);
  }
}

void
CQChartsView::
keyPositionSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->key()) {
    if      (action->text() == "Top Left"     )
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::TOP_LEFT     );
    else if (action->text() == "Top Center"   )
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::TOP_CENTER   );
    else if (action->text() == "Top Right"    )
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::TOP_RIGHT    );
    else if (action->text() == "Center Left"  )
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::CENTER_LEFT  );
    else if (action->text() == "Center Center")
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::CENTER_CENTER);
    else if (action->text() == "Center Right" )
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::CENTER_RIGHT );
    else if (action->text() == "Bottom Left"  )
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::BOTTOM_LEFT  );
    else if (action->text() == "Bottom Center")
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::BOTTOM_CENTER);
    else if (action->text() == "Bottom Right" )
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::BOTTOM_RIGHT );
    else if (action->text() == "Absolute"     )
      currentPlot->key()->setLocation(CQChartsKeyLocation::Type::ABS_POS      );
    else
      assert(false);
  }
}

void
CQChartsView::
keyInsideXSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (! currentPlot || ! currentPlot->key())
    return;

  if (b != currentPlot->key()->isInsideX()) {
    currentPlot->key()->setInsideX(b);

    if (b)
      currentPlot->updateKeyPosition(/*force*/true);
  }
}

void
CQChartsView::
keyInsideYSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (! currentPlot || ! currentPlot->key())
    return;

  if (b != currentPlot->key()->isInsideY()) {
    currentPlot->key()->setInsideY(b);

    if (b)
      currentPlot->updateKeyPosition(/*force*/true);
  }
}

void
CQChartsView::
xAxisVisibleSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->xAxis())
    currentPlot->xAxis()->setVisible(b);
}

void
CQChartsView::
xAxisGridSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->xAxis())
    currentPlot->xAxis()->setAxesMajorGridLines(b);
}

void
CQChartsView::
xAxisSideSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->xAxis()) {
    if      (action->text() == "Bottom")
      currentPlot->xAxis()->setSide(CQChartsAxis::Side::BOTTOM_LEFT);
    else if (action->text() == "Top")
      currentPlot->xAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);
  }
}

void
CQChartsView::
yAxisVisibleSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->yAxis())
    currentPlot->yAxis()->setVisible(b);
}

void
CQChartsView::
yAxisGridSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->yAxis())
    currentPlot->yAxis()->setAxesMajorGridLines(b);
}

void
CQChartsView::
yAxisSideSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->yAxis()) {
    if      (action->text() == "Left")
      currentPlot->yAxis()->setSide(CQChartsAxis::Side::BOTTOM_LEFT);
    else if (action->text() == "Right")
      currentPlot->yAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);
  }
}

void
CQChartsView::
titleVisibleSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->yAxis())
    currentPlot->title()->setVisible(b);
}

void
CQChartsView::
titleLocationSlot(QAction *action)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot && currentPlot->title()) {
    if      (action->text() == "Top")
      currentPlot->title()->setLocation(CQChartsTitle::LocationType::TOP);
    else if (action->text() == "Center")
      currentPlot->title()->setLocation(CQChartsTitle::LocationType::CENTER);
    else if (action->text() == "Bottom")
      currentPlot->title()->setLocation(CQChartsTitle::LocationType::BOTTOM);
    else if (action->text() == "Absolute")
      currentPlot->title()->setLocation(CQChartsTitle::LocationType::ABS_POS);
  }
}

void
CQChartsView::
invertXSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot)
    currentPlot->setInvertX(b);
}

void
CQChartsView::
invertYSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot)
    currentPlot->setInvertY(b);
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
  theme_ = CQChartsTheme(name);

  updateTheme();
}

void
CQChartsView::
updateTheme()
{
  setSelectedFillColor(themeObj()->selectColor());

  updatePlots();

  update();

  emit themePalettesChanged();
}

void
CQChartsView::
setDark(bool b)
{
  isDark_ = b;

  interfacePalette_->setColorType(CQChartsGradientPalette::ColorType::DEFINED);

  interfacePalette_->resetDefinedColors();

  if (! isDark_) {
    interfacePalette_->addDefinedColor(0.0, QColor("#ffffff"));
    interfacePalette_->addDefinedColor(1.0, QColor("#000000"));
  }
  else {
    interfacePalette_->addDefinedColor(0.0, QColor("#222222"));
    interfacePalette_->addDefinedColor(1.0, QColor("#dddddd"));
  }

  updatePlots();

  emit interfacePaletteChanged();
}

//------

void
CQChartsView::
printFile(const QString &filename, CQChartsPlot *plot)
{
  auto p = filename.lastIndexOf(".");

  if (p > 0) {
    QString suffix = filename.mid(p + 1).toLower();

    if      (suffix == "png")
      printPNG(filename, plot);
    else if (suffix == "svg")
      printSVG(filename, plot);
    else
      printPNG(filename, plot);
  }
  else {
    printPNG(filename, plot);
  }
}

void
CQChartsView::
printPNGSlot()
{
  QString dir = QDir::current().dirName() + "/charts.png";

  QString fileName = QFileDialog::getSaveFileName(this, "Print PNG", dir, "Files (*.png)");

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
printPNG(const QString &filename, CQChartsPlot *plot)
{
  int w = width ();
  int h = height();

  QImage image = QImage(QSize(w, h), QImage::Format_ARGB32);

  QPainter painter;

  painter.begin(&image);

  paint(&painter, plot);

  painter.end();

  if (plot) {
    CQChartsGeom::BBox pixelRect = plot->calcPlotPixelRect();

    image = image.copy(CQChartsUtil::toQRectI(pixelRect));
  }

  image.save(filename);
}

void
CQChartsView::
printSVG(const QString &filename, CQChartsPlot *plot)
{
  bool buffer = isBufferLayers();

  setBufferLayers(false);

  QSvgGenerator generator;

  generator.setFileName(filename);

  QPainter painter;

  painter.begin(&generator);

  paint(&painter, plot);

  painter.end();

  setBufferLayers(buffer);
}

//------

void
CQChartsView::
showBoxesSlot(bool b)
{
  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/true);

  if (currentPlot)
    currentPlot->setShowBoxes(b);
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
currentPlotSlot()
{
  QAction *action = qobject_cast<QAction *>(sender());

  setCurrentPlotInd(action->data().toInt());
}

//------

void
CQChartsView::
updatePlots()
{
  for (auto &plot : plots_) {
    plot->invalidateLayers();
  }
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
plotsAt(const CQChartsGeom::Point &p, Plots &plots, CQChartsPlot* &plot, bool clear) const
{
  if (clear)
    plots.clear();

  plot = nullptr;

  CQChartsPlot *currentPlot = this->currentPlot(/*remap*/false);

  for (const auto &plot1 : plots_) {
    const CQChartsGeom::BBox &bbox = plot1->viewBBox();

    if (! bbox.inside(p))
      continue;

    plots.push_back(plot1);

    if (plot1 == currentPlot)
      plot = plot1;
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

  CQChartsPlot *plot = plots_[ind];

  if (remap) {
    if (plot->isOverlay())
      plot = plot->firstPlot();
  }

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

  if      (pos.units() == CQChartsPosition::Units::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == CQChartsPosition::Units::VIEW)
    p1 = p;
  else if (pos.units() == CQChartsPosition::Units::PERCENT) {
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

  if      (pos.units() == CQChartsPosition::Units::PIXEL)
    p1 = p;
  else if (pos.units() == CQChartsPosition::Units::VIEW)
    p1 = windowToPixel(p);
  else if (pos.units() == CQChartsPosition::Units::PERCENT) {
    p1.setX(p.getX()*width ()/100.0);
    p1.setY(p.getY()*height()/100.0);
  }

  return CQChartsUtil::toQPoint(p1);
}

//------

double
CQChartsView::
lengthViewWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return pixelToWindowWidth(len.value());
  else if (len.units() == CQChartsLength::Units::VIEW)
    return len.value();
  else if (len.units() == CQChartsLength::Units::PERCENT)
    return len.value()*viewportRange()/100.0;
  else
    return len.value();
}

double
CQChartsView::
lengthViewHeight(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return pixelToWindowHeight(len.value());
  else if (len.units() == CQChartsLength::Units::VIEW)
    return len.value();
  else if (len.units() == CQChartsLength::Units::PERCENT)
    return len.value()*viewportRange()/100.0;
  else
    return len.value();
}

double
CQChartsView::
lengthPixelWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return len.value();
  else if (len.units() == CQChartsLength::Units::VIEW)
    return windowToPixelWidth(len.value());
  else if (len.units() == CQChartsLength::Units::PERCENT)
    return len.value()*width()/100.0;
  else
    return len.value();
}

double
CQChartsView::
lengthPixelHeight(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return len.value();
  else if (len.units() == CQChartsLength::Units::VIEW)
    return windowToPixelHeight(len.value());
  else if (len.units() == CQChartsLength::Units::PERCENT)
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

QSize
CQChartsView::
sizeHint() const
{
  return viewSizeHint();
}
