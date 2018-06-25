#include <CQChartsView.h>
#include <CQChartsViewToolTip.h>
#include <CQChartsProbeBand.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAnnotation.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsRotatedText.h>
#include <CQChartsDisplayRange.h>
#include <CQPropertyViewModel.h>

#include <QSvgGenerator>
#include <QRubberBand>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>

namespace {
  CQChartsPlot::ModSelect modifiersToModSelect(Qt::KeyboardModifiers modifiers) {
    if      ((modifiers & Qt::ControlModifier) && (modifiers & Qt::ShiftModifier))
      return CQChartsPlot::ModSelect::TOGGLE;
    else if (modifiers & Qt::ControlModifier)
      return CQChartsPlot::ModSelect::ADD;
    else if (modifiers & Qt::ShiftModifier)
      return CQChartsPlot::ModSelect::REMOVE;
    else
      return CQChartsPlot::ModSelect::REPLACE;
  }
}

//---

QSize CQChartsView::sizeHint_ = QSize(1280, 1024);

CQChartsView::
CQChartsView(CQCharts *charts, QWidget *parent) :
 QFrame(parent), charts_(charts)
{
  setObjectName("view");

  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

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
  addProperty("", this, "background"    );
  addProperty("", this, "currentPlotInd");
  addProperty("", this, "mode"          );
  addProperty("", this, "selectMode"    );
  addProperty("", this, "themeName"     );
  addProperty("", this, "zoomData"      );
  addProperty("", this, "antiAlias"     );
  addProperty("", this, "posTextType"   );

  addProperty("selectedHighlight"       , this, "selectedMode"              , "mode");
  addProperty("selectedHighlight/stroke", this, "selectedStrokeColorEnabled", "enabled");
  addProperty("selectedHighlight/stroke", this, "selectedStrokeColor"       , "color");
  addProperty("selectedHighlight/stroke", this, "selectedStrokeWidth"       , "width");
  addProperty("selectedHighlight/stroke", this, "selectedStrokeDash"        , "dash");
  addProperty("selectedHighlight/fill"  , this, "selectedFillColorEnabled"  , "enabled");
  addProperty("selectedHighlight/fill"  , this, "selectedFillColor"         , "color");

  addProperty("insideHighlight"       , this, "insideMode"              , "mode");
  addProperty("insideHighlight/stroke", this, "insideStrokeColorEnabled", "enabled");
  addProperty("insideHighlight/stroke", this, "insideStrokeColor"       , "color");
  addProperty("insideHighlight/stroke", this, "insideStrokeWidth"       , "width");
  addProperty("insideHighlight/stroke", this, "insideStrokeDash"        , "dash");
  addProperty("insideHighlight/fill"  , this, "insideFillColorEnabled"  , "enabled");
  addProperty("insideHighlight/fill"  , this, "insideFillColor"         , "color");

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
  title_ = s;

  setWindowTitle(title_);
}

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
    currentPlotInd_ = ind;

    emit currentPlotChanged();
  }
}

void
CQChartsView::
setMode(const Mode &mode)
{
  if (mode != mode_) {
    endRegionBand();

    for (auto &probeBand : probeBands_)
      probeBand->hide();

    deselectAll();

    mode_ = mode;

    emit modeChanged();
  }
}

void
CQChartsView::
setSelectMode(const SelectMode &selectMode)
{
  if (selectMode != selectMode_) {
    selectMode_ = selectMode;

    emit selectModeChanged();
  }
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
setThemeName(const QString &str)
{
  if (themeName() != str)
    themeSlot(str);
}

CQChartsGradientPalette *
CQChartsView::
themeGroupPalette(int i, int /*n*/) const
{
  return theme_->palette(i);
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

void
CQChartsView::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  propertyModel()->addProperty(path, object, name, alias);
}

//---

CQChartsTextAnnotation *
CQChartsView::
addTextAnnotation(const QPointF &pos, const QString &text)
{
  CQChartsTextAnnotation *textAnnotation = new CQChartsTextAnnotation(this, pos, text);

  addAnnotation(textAnnotation);

  return textAnnotation;
}

CQChartsArrowAnnotation *
CQChartsView::
addArrowAnnotation(const QPointF &start, const QPointF &end)
{
  CQChartsArrowAnnotation *arrowAnnotation = new CQChartsArrowAnnotation(this, start, end);

  addAnnotation(arrowAnnotation);

  return arrowAnnotation;
}

CQChartsRectAnnotation *
CQChartsView::
addRectAnnotation(const QPointF &start, const QPointF &end)
{
  CQChartsRectAnnotation *rectAnnotation = new CQChartsRectAnnotation(this, start, end);

  addAnnotation(rectAnnotation);

  return rectAnnotation;
}

CQChartsEllipseAnnotation *
CQChartsView::
addEllipseAnnotation(const QPointF &center, double xRadius, double yRadius)
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
addPointAnnotation(const QPointF &pos, const CQChartsSymbol &type)
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

//---

void
CQChartsView::
addPlot(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox)
{
  if (! plot->id().length()) {
    QString id(QString("%1").arg(numPlots() + 1));

    plot->setId(QString("%1%2").arg(plot->typeName()).arg(id));

    plot->setObjectName(plot->id());
  }

  plot->setBBox(bbox);

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

    if (plot->key())
      plot->key()->setVisible(true);

    if (plot->title())
      plot->title()->setVisible(true);

    plot->setBackground    (true);
    plot->setDataBackground(true);

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

      //plot->setDataRange(rootPlot->dataRange());

      //rootPlot->applyDataRange();
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

  CQChartsPlot *plot = firstPlot->nextPlot();

  while (plot) {
    plot->setOverlay(true, /*notify*/false);

    //---

    CQChartsAxis *xaxis = plot->xAxis();
    CQChartsAxis *yaxis = plot->yAxis();

    if (xaxis)
      xaxis->setVisible(false);

    if (yaxis)
      yaxis->setVisible(false);

    CQChartsTitle *title = plot->title();

    if (title)
      title->setVisible(false);

    plot->setBackground    (false);
    plot->setDataBackground(false);

    plot = plot->nextPlot();
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

  if (plot2->xAxis()) {
    if (plot2->isOverlay())
      plot2->xAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);
  }

  if (plot2->yAxis())
    plot2->yAxis()->setVisible(false);

  if (plot2->key())
    plot2->key()->setVisible(false);

  if (plot2->title())
    plot2->title()->setVisible(false);

  if (plot2->isOverlay()) {
    plot2->setBackground    (false);
    plot2->setDataBackground(false);
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

  if (plot2->xAxis())
    plot2->xAxis()->setVisible(false);

  if (plot2->yAxis()) {
    if (plot2->isOverlay())
      plot2->yAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);
  }

  if (plot2->key())
    plot2->key()->setVisible(false);

  if (plot2->title())
    plot2->title()->setVisible(false);

  if (plot2->isOverlay()) {
    plot2->setBackground    (false);
    plot2->setDataBackground(false);
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

      plot->setBBox(bbox);
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
        CQChartsPlot *plot = plots[i];

        CQChartsGeom::BBox bbox(x, y - dy, x + dx, y);

        plot->setBBox(bbox);

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

        CQChartsPlot::ModSelect modSelect = modifiersToModSelect(me->modifiers());

        if (mouseData_.plot && mouseData_.plot->selectMousePress(me->pos(), modSelect)) {
          setCurrentPlot(mouseData_.plot);
          return;
        }

        for (auto &plot : mouseData_.plots) {
          if (plot == mouseData_.plot) continue;

          if (plot->selectMousePress(me->pos(), modSelect)) {
            setCurrentPlot(plot);
            return;
          }
        }

        //---

        CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

        if (keyObj_->isInside(w))
          keyObj_->selectPress(w);
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

        CQChartsPlot::ModSelect modSelect = modifiersToModSelect(me->modifiers());

        if (mouseData_.plot) {
          CQChartsGeom::Point w1, w2;

          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.pressPoint), w1);
          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.movePoint ), w2);

          if (mouseData_.plot->rectSelect(CQChartsGeom::BBox(w1, w2), modSelect))
            return;
        }

        for (auto &plot : mouseData_.plots) {
          if (plot == mouseData_.plot) continue;

          CQChartsGeom::Point w1, w2;

          plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.pressPoint), w1);
          plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.movePoint ), w2);

          if (plot->rectSelect(CQChartsGeom::BBox(w1, w2), modSelect))
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

    CQChartsGeom::BBox pixelRect = plot->calcPixelRect();

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
  CQChartsPlot::PlotObjs objs;

  int num = 0;

  for (auto &plot : plots_) {
    CQChartsPlot::PlotObjs objs1;

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
paint(QPainter *painter)
{
  if (isAntiAlias())
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //---

  painter->fillRect(CQChartsUtil::toQRect(prect_), QBrush(background()));

  //---

  for (const auto &plot : plots_) {
    if (plot->isVisible())
      plot->draw(painter);
  }

  for (auto &annotation : annotations())
    annotation->draw(painter);

  //---

  key()->draw(painter);
}

//------

void
CQChartsView::
drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
              const QPen &pen, const CQChartsTextOptions &options)
{
  painter->save();

  if (CQChartsUtil::isZero(options.angle)) {
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

      fs = std::min(std::max(fs, options.minScaleFontSize), options.maxScaleFontSize);

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

  QMenu *keyMenu = new QMenu("Key", popupMenu_);

  QAction *keyVisibleAction = new QAction("Visible", popupMenu_);

  keyVisibleAction->setCheckable(true);

  if (currentPlot && currentPlot->key())
    keyVisibleAction->setChecked(currentPlot->key()->isVisible());

  connect(keyVisibleAction, SIGNAL(triggered(bool)), this, SLOT(keyVisibleSlot(bool)));

  keyMenu->addAction(keyVisibleAction);

  //---

  using KeyLocationActionMap = std::map<CQChartsPlotKey::LocationType, QAction *>;

  KeyLocationActionMap keyLocationActionMap;

  QMenu *keyLocationMenu = new QMenu("Location", keyMenu);

  QActionGroup *keyLocationActionGroup = new QActionGroup(keyLocationMenu);

  auto addKeyLocationGroupAction =
   [&](const QString &label, const CQChartsPlotKey::LocationType &location) {
    QAction *action = new QAction(label, keyLocationMenu);

    action->setCheckable(true);

    keyLocationActionMap[location] = action;

    keyLocationActionGroup->addAction(action);

    return action;
  };

  addKeyLocationGroupAction("Top Left"     , CQChartsPlotKey::LocationType::TOP_LEFT     );
  addKeyLocationGroupAction("Top Center"   , CQChartsPlotKey::LocationType::TOP_CENTER   );
  addKeyLocationGroupAction("Top Right"    , CQChartsPlotKey::LocationType::TOP_RIGHT    );
  addKeyLocationGroupAction("Center Left"  , CQChartsPlotKey::LocationType::CENTER_LEFT  );
  addKeyLocationGroupAction("Center Center", CQChartsPlotKey::LocationType::CENTER_CENTER);
  addKeyLocationGroupAction("Center Right" , CQChartsPlotKey::LocationType::CENTER_RIGHT );
  addKeyLocationGroupAction("Bottom Left"  , CQChartsPlotKey::LocationType::BOTTOM_LEFT  );
  addKeyLocationGroupAction("Bottom Center", CQChartsPlotKey::LocationType::BOTTOM_CENTER);
  addKeyLocationGroupAction("Bottom Right" , CQChartsPlotKey::LocationType::BOTTOM_RIGHT );
  addKeyLocationGroupAction("Absolute"     , CQChartsPlotKey::LocationType::ABS_POS      );

  keyLocationActionGroup->setExclusive(true);

  if (currentPlot && currentPlot->key()) {
    CQChartsPlotKey::LocationType location = currentPlot->key()->location();

    keyLocationActionMap[location]->setChecked(true);
  }

  connect(keyLocationActionGroup, SIGNAL(triggered(QAction *)),
          this, SLOT(keyPositionSlot(QAction *)));

  keyLocationMenu->addActions(keyLocationActionGroup->actions());

  keyMenu->addMenu(keyLocationMenu);

  //---

  popupMenu_->addMenu(keyMenu);

  //------

  using AxisSideActionMap = std::map<CQChartsAxis::Side, QAction *>;

  //------

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
    xAxisGridAction->setChecked(currentPlot->xAxis()->isGridMajorDisplayed());

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
    yAxisGridAction->setChecked(currentPlot->yAxis()->isGridMajorDisplayed());

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

  //---

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

  defaultThemeAction->setChecked(themeName() == "default" );
  palette1Action    ->setChecked(themeName() == "palette1");
  palette2Action    ->setChecked(themeName() == "palette2");

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

  if (CQChartsUtil::getBoolEnv("CQCHARTS_DEBUG", true)) {
    QAction *showBoxesAction = new QAction("Show Boxes", popupMenu_);

    showBoxesAction->setCheckable(true);

    if (currentPlot)
      showBoxesAction->setChecked(currentPlot->showBoxes());

    popupMenu_->addAction(showBoxesAction);

    connect(showBoxesAction, SIGNAL(triggered(bool)), this, SLOT(showBoxesSlot(bool)));
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
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::TOP_LEFT     );
    else if (action->text() == "Top Center"   )
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::TOP_CENTER   );
    else if (action->text() == "Top Right"    )
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::TOP_RIGHT    );
    else if (action->text() == "Center Left"  )
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::CENTER_LEFT  );
    else if (action->text() == "Center Center")
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::CENTER_CENTER);
    else if (action->text() == "Center Right" )
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::CENTER_RIGHT );
    else if (action->text() == "Bottom Left"  )
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::BOTTOM_LEFT  );
    else if (action->text() == "Bottom Center")
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::BOTTOM_LEFT  );
    else if (action->text() == "Bottom Right" )
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::BOTTOM_RIGHT );
    else if (action->text() == "Absolute"     )
      currentPlot->key()->setLocation(CQChartsPlotKey::LocationType::ABS_POS      );
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
    currentPlot->xAxis()->setGridMajorDisplayed(b);
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
    currentPlot->yAxis()->setGridMajorDisplayed(b);
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
  interfacePalette_->setColorType(CQChartsGradientPalette::ColorType::DEFINED);

  interfacePalette_->resetDefinedColors();

  interfacePalette_->addDefinedColor(0.0, QColor("#ffffff"));
  interfacePalette_->addDefinedColor(1.0, QColor("#000000"));

  isDark_ = false;

  emit interfacePaletteChanged();
}

void
CQChartsView::
darkPaletteSlot()
{
  interfacePalette_->setColorType(CQChartsGradientPalette::ColorType::DEFINED);

  interfacePalette_->resetDefinedColors();

  interfacePalette_->addDefinedColor(0.0, QColor("#222222"));
  interfacePalette_->addDefinedColor(1.0, QColor("#dddddd"));

  isDark_ = true;

  emit interfacePaletteChanged();
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
  CQChartsTheme *theme = CQChartsThemeMgrInst->getTheme(name);
  if (! theme) return;

  theme_ = theme;

  setSelectedFillColor(theme_->selectColor());

  updatePlots();

  update();

  emit themePalettesChanged();
}

//------

void
CQChartsView::
printFile(const QString &filename)
{
  auto p = filename.lastIndexOf(".");

  if (p > 0) {
    QString suffix = filename.mid(p + 1).toLower();

    if      (suffix == "png")
      printPNGSlot(filename);
    else if (suffix == "svg")
      printSVGSlot(filename);
    else
      printPNGSlot(filename);
  }
  else {
    printPNGSlot(filename);
  }
}

void
CQChartsView::
printPNGSlot(const QString &filename)
{
  int w = width ();
  int h = height();

  QImage image = QImage(QSize(w, h), QImage::Format_ARGB32);

  QPainter painter;

  painter.begin(&image);

  paint(&painter);

  painter.end();

  image.save(filename);
}

void
CQChartsView::
printSVGSlot(const QString &filename)
{
  QSvgGenerator generator;

  generator.setFileName(filename);

  QPainter painter;

  painter.begin(&generator);

  paint(&painter);

  painter.end();
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
    plot->update();
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
    const CQChartsGeom::BBox &bbox = plot1->bbox();

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
    const CQChartsGeom::BBox &bbox = plot->bbox();

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

    const CQChartsGeom::BBox &bbox = plot1->bbox();

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
  if (scrollPage_ > 0) {
    --scrollPage_;

    updateScroll();
  }
}

void
CQChartsView::
scrollRight()
{
  if (scrollPage_ < scrollNumPages_ - 1) {
    ++scrollPage_;

    updateScroll();
  }
}

void
CQChartsView::
updateScroll()
{
  double dx = scrollPage_*scrollDelta_;

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
    p1.setY(p.getX()*height()/100.0);
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
    p1.setY(p.getX()*height()/100.0);
  }

  return CQChartsUtil::toQPoint(p1);
}

//------

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
pixelToSignedWindowWidth (double ww) const
{
  return CQChartsUtil::sign(ww)*pixelToWindowWidth(ww);
}

double
CQChartsView::
pixelToSignedWindowHeight(double wh) const
{
  return -CQChartsUtil::sign(wh)*pixelToWindowHeight(wh);
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
  return CQChartsUtil::sign(ww)*windowToPixelWidth(ww);
}

double
CQChartsView::
windowToSignedPixelHeight(double wh) const
{
  return -CQChartsUtil::sign(wh)*windowToPixelHeight(wh);
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
