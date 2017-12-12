#include <CQChartsView.h>
#include <CQChartsViewToolTip.h>
#include <CQChartsProbeBand.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsUtil.h>
#include <CGradientPalette.h>
#include <CQPropertyViewModel.h>
#include <CQChartsDisplayRange.h>

#include <QSvgGenerator>
#include <QRubberBand>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>

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

  palette_       = new CGradientPalette;
  theme_         = new CGradientPalette;
  propertyModel_ = new CQPropertyViewModel;

  //---

  setSelectedMode(HighlightDataMode::FILL);

  //---

  //setPaletteColors1();

  setLightThemeColors();

  //---

  addProperty("", this, "id"            );
  addProperty("", this, "title"         );
  addProperty("", this, "background"    );
  addProperty("", this, "currentPlotInd");
  addProperty("", this, "mode"          );
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
  delete palette_;
  delete theme_;
  delete propertyModel_;

  delete displayRange_;

  for (auto &plotData : plotDatas_)
    delete plotData.plot;

  for (auto &probeBand : probeBands_)
    delete probeBand;

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
setCurrentPlotInd(int ind)
{
  currentPlotInd_ = ind;

  emit currentPlotChanged();
}

void
CQChartsView::
setMode(const Mode &mode)
{
  if (mode != mode_) {
    endRegionBand();

    for (auto &probeBand : probeBands_)
      probeBand->hide();

    mode_ = mode;

    emit modeChanged();
  }
}

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

void
CQChartsView::
addPlot(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox)
{
  if (! plot->id().length()) {
    QString id(QString("%1").arg(numPlots() + 1));

    plot->setId(QString("%1%2").arg(plot->typeName()).arg(id));
  }

  plot->setPalette(gradientPalette());
  plot->setTheme  (themePalette());

  plot->setBBox(bbox);

  plotDatas_.emplace_back(plot, bbox);

  plot->addProperties();

  plot->postInit();

  //---

  if (currentPlotInd_ < 0)
    setCurrentPlotInd(0);
}

CQChartsPlot *
CQChartsView::
getPlot(const QString &id) const
{
  for (const auto &plotData : plotDatas_) {
    if (plotData.plot->id() == id)
      return plotData.plot;
  }

  return nullptr;
}

void
CQChartsView::
initOverlay()
{
  if (! numPlots())
    return;

  CQChartsPlot *firstPlot = plot(0)->firstPlot();

  initOverlay(firstPlot);
}

void
CQChartsView::
initOverlay(const Plots &plots)
{
  assert(plots.size() >= 2);

  CQChartsPlot *rootPlot = plots[0]->firstPlot();

  for (std::size_t i = 0; i < plots.size(); ++i) {
    CQChartsPlot *plot = plots[i];

    plot->setOverlay(true);

    if (i > 0) {
      CQChartsPlot *prevPlot = plots[i - 1];

      plot    ->setPrevPlot(prevPlot);
      prevPlot->setNextPlot(plot);

      //plot->setDataRange(rootPlot->dataRange());

      //rootPlot->applyDataRange();
    }
  }

  initOverlay(rootPlot);
}

void
CQChartsView::
initOverlay(CQChartsPlot *firstPlot)
{
  firstPlot->setOverlay(true);

  if (firstPlot->titleObj() && title().length())
    firstPlot->titleObj()->setText(title());

  //---

  CQChartsPlot *plot = firstPlot->nextPlot();

  while (plot) {
    plot->setOverlay(true);

    //---

    CQChartsAxis *xaxis = plot->xAxis();
    CQChartsAxis *yaxis = plot->yAxis();

    if (xaxis)
      xaxis->setVisible(false);

    if (yaxis)
      yaxis->setVisible(false);

    CQChartsTitle *title = plot->titleObj();

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
initY1Y2(CQChartsPlot *plot1, CQChartsPlot *plot2)
{
  assert(plot1 != plot2 && ! plot1->isOverlay() && ! plot2->isOverlay());

  if (plot1->titleObj() && title().length())
    plot1->titleObj()->setText(title());

  plot1->setY1Y2(true);
  plot2->setY1Y2(true);

  plot1->setNextPlot(plot2);
  plot2->setPrevPlot(plot1);

  if (plot2->xAxis())
    plot2->xAxis()->setVisible(false);

  if (plot2->yAxis())
    plot2->yAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);

  if (plot2->key())
    plot2->key()->setVisible(false);

  if (plot2->titleObj())
    plot2->titleObj()->setVisible(false);

  plot2->setBackground    (false);
  plot2->setDataBackground(false);
}

void
CQChartsView::
mousePressEvent(QMouseEvent *me)
{
  mouseData_.reset();

  mouseData_.pressPoint = me->pos();
  mouseData_.button     = me->button();
  mouseData_.pressed    = true;

  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if (me->button() == Qt::LeftButton) {
    if      (mode_ == Mode::SELECT) {
      if (selectMode_ == SelectMode::POINT) {
        CQChartsScopeGuard updateSelTextGuard([&]() { updateSelText(); });

        CQChartsPlot::ModSelect modSelect = CQChartsPlot::ModSelect::REPLACE;

        if      (me->modifiers() & Qt::ControlModifier &&
                 me->modifiers() & Qt::ShiftModifier)
          modSelect = CQChartsPlot::ModSelect::TOGGLE;
        else if (me->modifiers() & Qt::ControlModifier)
          modSelect = CQChartsPlot::ModSelect::ADD;
        else if (me->modifiers() & Qt::ShiftModifier)
          modSelect = CQChartsPlot::ModSelect::REMOVE;

        //---

        if (mouseData_.plot) {
          CQChartsGeom::Point w;

          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

          if (mouseData_.plot->mousePress(w, modSelect))
            return;
        }

        for (auto &plot : mouseData_.plots) {
          if (plot == mouseData_.plot) continue;

          CQChartsGeom::Point w;

          plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

          if (plot->mousePress(w, modSelect))
            return;
        }
      }
      else {
        startRegionBand(mouseData_.pressPoint);
      }
    }
    else if (mode_ == Mode::ZOOM) {
      if (mouseData_.plot && mouseData_.plot->isClickZoom()) {
        CQChartsGeom::Point w;

        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        mouseData_.clickZoom = true;

        mouseData_.plot->clickZoom(w);
      }
      else {
        startRegionBand(mouseData_.pressPoint);
      }
    }
    else if (mode_ == Mode::PROBE) {
    }
  }
  else if (me->button() == Qt::MiddleButton) {
    if      (mode_ == Mode::SELECT) {
      if (mouseData_.plot) {
        CQChartsGeom::Point w;

        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        if (mouseData_.plot->mouseDragPress(w))
          return;
      }

      for (auto &plot : mouseData_.plots) {
        if (plot == mouseData_.plot) continue;

        CQChartsGeom::Point w;

        plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        if (plot->mouseDragPress(w))
          return;
      }
    }
  }
  else if (me->button() == Qt::RightButton) {
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
  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

  //---

  // select mode and move (not pressed) - update plot positions
  if (mode_ == Mode::SELECT && ! mouseData_.pressed) {
    QString posStr;

    if (posTextType() == PosTextType::PLOT) {
      PlotSet plots;

      basePlotsAt(w, plots);

      for (const auto &plot : plots) {
        CQChartsGeom::Point w;

        plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        if (posStr.length())
          posStr += " ";

        posStr += plot->posStr(w);
      }
    }
    else if (posTextType() == PosTextType::VIEW) {
      CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

      posStr = QString("%1 %2").arg(w.x).arg(w.y);
    }
    else {
      posStr = QString("%1 %2").arg(me->x()).arg(me->y());
    }

    setPosText(posStr);

    //---

    searchPos_ = me->pos();

    searchTimer_.start();

    return;
  }

  //---

  // probe move and move (pressed or not pressed) - show probe lines
  if (mode_ == Mode::PROBE) {
    auto addProbeBand = [&](int &ind, CQChartsPlot *plot, const QString &tip, double px,
                            double py1, double py2) -> void {
      while (ind >= int(probeBands_.size())) {
        CQChartsProbeBand *probeBand = new CQChartsProbeBand(this);

        probeBands_.push_back(probeBand);
      }

      probeBands_[ind]->show(plot, tip, px, py1, py2);

      ++ind;
    };

    //int px = me->pos().x();

    Plots         plots;
    CQChartsPlot *plot;

    plotsAt(w, plots, plot);

    int probeInd = 0;

    for (auto &plot : plots) {
      CQChartsGeom::Point w;

      plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

      //---

      CQChartsPlot::ProbeData probeData;

      probeData.x = w.x;
      probeData.y = w.y;

      if (! plot->probe(probeData))
        continue;

      if (probeData.yvals.empty())
        probeData.yvals.emplace_back(w.y);

      CQChartsGeom::BBox dataRange = plot->calcDataRange();

      // add probe lines from ymin to probed y values
      CQChartsGeom::Point p1;

      plot->windowToPixel(CQChartsGeom::Point(probeData.x, dataRange.getYMin()), p1);

      for (const auto &yval : probeData.yvals) {
        CQChartsGeom::Point p2;

        plot->windowToPixel(CQChartsGeom::Point(probeData.x, yval.value), p2);

        QString tip = (yval.label.length() ? yval.label : plot->yStr(yval.value));

        addProbeBand(probeInd, plot, tip, p1.x, p1.y, p2.y);
      }
    }

    for (int i = probeInd; i < int(probeBands_.size()); ++i)
      probeBands_[i]->hide();

    return;
  }

  //---

  // get plots are point
  plotsAt(w, mouseData_.plots, mouseData_.plot);

  //---

  if      (mouseData_.button == Qt::LeftButton) {
    if (! mouseData_.pressed)
      return;

    // select plot object
    if      (mode_ == Mode::SELECT) {
      if (selectMode_ == SelectMode::POINT) {
        if (mouseData_.plot) {
          CQChartsGeom::Point w;

          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

          (void) mouseData_.plot->mouseMove(w, true);
        }

        for (auto &plot : mouseData_.plots) {
          if (plot == mouseData_.plot) continue;

          plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(searchPos_)), w);

          if (plot->mouseMove(w, false))
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
    else if (mode_ == Mode::ZOOM) {
      if (mouseData_.clickZoom)
        return;

      mouseData_.movePoint = me->pos();

      if (mouseData_.escape)
        endRegionBand();
      else
        updateRegionBand(mouseData_.pressPoint, mouseData_.movePoint);
    }
    else if (mode_ == Mode::PROBE) {
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
    if (! mouseData_.pressed)
      return;

    if      (mode_ == Mode::SELECT) {
      // drag plot object
      if (mouseData_.plot) {
        CQChartsGeom::Point w;

        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        (void) mouseData_.plot->mouseDragMove(w, true);
      }

      for (auto &plot : mouseData_.plots) {
        if (plot == mouseData_.plot) continue;

        CQChartsGeom::Point w;

        plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        if (plot->mouseDragMove(w, false))
          return;
      }
    }
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
  CQChartsScopeGuard resetMouseData([&]() { mouseData_.reset(); });

  //CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

  if      (mouseData_.button == Qt::LeftButton) {
    if      (mode_ == Mode::SELECT) {
      if (! mouseData_.pressed)
        return;

      if (selectMode_ == SelectMode::POINT) {
        if (mouseData_.plot) {
          CQChartsGeom::Point w;

          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

          mouseData_.plot->mouseRelease(w);
        }
      }
      else {
        CQChartsScopeGuard updateSelTextGuard([&]() { updateSelText(); });

        mouseData_.movePoint = me->pos();

        endRegionBand();

        CQChartsPlot::ModSelect modSelect = CQChartsPlot::ModSelect::REPLACE;

        if      (me->modifiers() & Qt::ControlModifier &&
                 me->modifiers() & Qt::ShiftModifier)
          modSelect = CQChartsPlot::ModSelect::TOGGLE;
        else if (me->modifiers() & Qt::ControlModifier)
          modSelect = CQChartsPlot::ModSelect::ADD;
        else if (me->modifiers() & Qt::ShiftModifier)
          modSelect = CQChartsPlot::ModSelect::REMOVE;

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

          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.pressPoint), w1);
          mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(mouseData_.movePoint ), w2);

          if (plot->rectSelect(CQChartsGeom::BBox(w1, w2), modSelect))
            return;
        }
      }
    }
    else if (mode_ == Mode::ZOOM) {
      if (mouseData_.clickZoom)
        return;

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
    else if (mode_ == Mode::PROBE) {
    }
  }
  else if (mouseData_.button == Qt::MiddleButton) {
    if      (mode_ == Mode::SELECT) {
      if (! mouseData_.pressed)
        return;

      if (mouseData_.plot) {
        CQChartsGeom::Point w;

        mouseData_.plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        mouseData_.plot->mouseDragRelease(w);
      }

      for (auto &plot : mouseData_.plots) {
        if (plot == mouseData_.plot) continue;

        CQChartsGeom::Point w;

        plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        plot->mouseDragRelease(w);
      }
    }
  }
  else if (mouseData_.button == Qt::RightButton) {
  }
}

void
CQChartsView::
keyPressEvent(QKeyEvent *ke)
{
  if      (ke->key() == Qt::Key_Escape) {
    mouseData_.escape = true;

    if (mouseData_.pressed)
      endRegionBand();

    if      (mode() == Mode::ZOOM) {
      if (! mouseData_.pressed)
        setMode(Mode::SELECT);
    }
    else if (mode() == Mode::PROBE) {
      setMode(Mode::SELECT);
    }

    return;
  }
  else if (ke->key() == Qt::Key_Bar) {
    setMode(Mode::PROBE);

    return;
  }

  //---

  QPoint gpos = QCursor::pos();

  QPointF pos = mapFromGlobal(gpos);

  CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(pos));

  Plots         plots;
  CQChartsPlot *plot;

  plotsAt(w, plots, plot);

  if (plot)
    plot->keyPress(ke->key());
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
updateRegionBand(const QPoint &pressPoint, const QPoint &movePoint)
{
  regionBand_->setGeometry(QRect(pressPoint, movePoint));
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
  int num = 0;

  for (auto &plotData : plotDatas_) {
    CQChartsPlot::PlotObjs objs;

    plotData.plot->selectedObjs(objs);

    num += objs.size();
  }

  setSelText(QString("%1").arg(num));
}

//------

void
CQChartsView::
resizeEvent(QResizeEvent *)
{
  prect_ = CQChartsGeom::BBox(0, 0, width(), height());

  aspect_ = (1.0*prect().getWidth())/prect().getHeight();

  displayRange_->setPixelRange(prect_.getXMin(), prect_.getYMin(),
                               prect_.getXMax(), prect_.getYMax());

  //---

  for (const auto &plotData : plotDatas_)
    plotData.plot->handleResize();
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

  for (const auto &plotData : plotDatas_) {
    if (plotData.plot->isVisible())
      plotData.plot->draw(painter);
  }
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

    if (mouseData_.plot->mouseMove(w, ! handled))
      handled = true;
  }

  // process other plots
  for (auto &plot : mouseData_.plots) {
    if (plot == mouseData_.plot) continue;

    CQChartsGeom::Point w;

    plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(searchPos_)), w);

    if (plot->mouseMove(w, ! handled))
      handled = true;
  }
}

//------

void
CQChartsView::
showMenu(const QPoint &p)
{
  if (! popupMenu_) {
    popupMenu_ = new QMenu(this);

    //---

    //PlotSet plots;

    //basePlots(plots);

    Plots plots;

    this->plots(plots);

    if (plots.size() > 1) {
      QMenu *plotsMenu = new QMenu("Plots");

      QActionGroup *plotsGroup = new QActionGroup(plotsMenu);

      for (const auto &plot : plots) {
        int ind = plotInd(plot);

        QAction *plotAction = new QAction(plot->id(), plotsMenu);

        plotAction->setCheckable(true);
        plotAction->setChecked(currentPlotInd() == ind);

        plotAction->setData(ind);

        plotsGroup->addAction(plotAction);

        connect(plotAction, SIGNAL(triggered()), this, SLOT(currentPlotSlot()));
      }

      plotsMenu->addActions(plotsGroup->actions());

      popupMenu_->addMenu(plotsMenu);
    }

    //---

    QAction *keyAction = new QAction("Key", popupMenu_);
    QAction *fitAction = new QAction("Fit", popupMenu_);

    keyAction->setCheckable(true);

    CQChartsPlot *currentPlot = this->currentPlot();

    if (currentPlot && currentPlot->key())
      keyAction->setChecked(currentPlot->key()->isVisible());

    connect(keyAction, SIGNAL(triggered(bool)), this, SLOT(keySlot(bool)));
    connect(fitAction, SIGNAL(triggered()), this, SLOT(fitSlot()));

    popupMenu_->addAction(keyAction);
    popupMenu_->addAction(fitAction);

    //---

    QMenu *themeMenu = new QMenu("Theme");

    QActionGroup *themeGroup = new QActionGroup(themeMenu);

    QAction *lightTheme1Action = new QAction("Light 1", themeMenu);
    QAction *lightTheme2Action = new QAction("Light 2", themeMenu);
    QAction *darkTheme1Action  = new QAction("Dark 1" , themeMenu);
    QAction *darkTheme2Action  = new QAction("Dark 2" , themeMenu);

    lightTheme1Action->setCheckable(true);
    lightTheme2Action->setCheckable(true);
    darkTheme1Action ->setCheckable(true);
    darkTheme2Action ->setCheckable(true);

    lightTheme1Action->setChecked(themeType_ == ThemeType::LIGHT1);
    lightTheme2Action->setChecked(themeType_ == ThemeType::LIGHT2);
    darkTheme1Action ->setChecked(themeType_ == ThemeType::DARK1 );
    darkTheme2Action ->setChecked(themeType_ == ThemeType::DARK2 );

    themeGroup->addAction(lightTheme1Action);
    themeGroup->addAction(lightTheme2Action);
    themeGroup->addAction(darkTheme1Action);
    themeGroup->addAction(darkTheme2Action);

    connect(lightTheme1Action, SIGNAL(triggered()), this, SLOT(lightTheme1Slot()));
    connect(lightTheme2Action, SIGNAL(triggered()), this, SLOT(lightTheme2Slot()));
    connect(darkTheme1Action , SIGNAL(triggered()), this, SLOT(darkTheme1Slot()));
    connect(darkTheme2Action , SIGNAL(triggered()), this, SLOT(darkTheme2Slot()));

    themeMenu->addActions(themeGroup->actions());

    popupMenu_->addMenu(themeMenu);

    //---

    for (const auto &plot : plots) {
      (void) plot->addMenuItems(popupMenu_);
    }

    //---

    QAction *printAction = new QAction("Print", popupMenu_);

    connect(printAction, SIGNAL(triggered()), this, SLOT(printSlot()));

    popupMenu_->addAction(printAction);
  }

  popupMenu_->popup(mapToGlobal(p));
}

void
CQChartsView::
keySlot(bool b)
{
  for (const auto &plotData : plotDatas_) {
    CQChartsKey *key = plotData.plot->key();

    if (key)
      key->setVisible(b);
  }
}

void
CQChartsView::
fitSlot()
{
  for (const auto &plotData : plotDatas_) {
    plotData.plot->autoFit();
  }
}

void
CQChartsView::
lightTheme1Slot()
{
  themeType_ = ThemeType::LIGHT1;

  setPaletteColors1();

  setLightThemeColors();
}

void
CQChartsView::
lightTheme2Slot()
{
  themeType_ = ThemeType::LIGHT2;

  setPaletteColors2();

  setLightThemeColors();
}

void
CQChartsView::
darkTheme1Slot()
{
  themeType_ = ThemeType::DARK1;

  setPaletteColors1();

  setDarkThemeColors();
}

void
CQChartsView::
darkTheme2Slot()
{
  themeType_ = ThemeType::DARK2;

  setPaletteColors2();

  setDarkThemeColors();
}

void
CQChartsView::
setPaletteColors1()
{
  palette_->setRedModel  (1);
  palette_->setGreenModel(7);
  palette_->setBlueModel (4);

  palette_->setBlueNegative(true);

  palette_->setRedMax  (0.8);
  palette_->setGreenMax(0.4);

  palette_->setColorType(CGradientPalette::ColorType::DEFINED);

  palette_->resetDefinedColors();

  palette_->addDefinedColor(0.0, QColor("#6d78ad"));
  palette_->addDefinedColor(1.0, QColor("#51cda0"));
  palette_->addDefinedColor(2.0, QColor("#df7970"));
}

void
CQChartsView::
setPaletteColors2()
{
  palette_->setRedModel  (1);
  palette_->setGreenModel(7);
  palette_->setBlueModel (4);

  palette_->setBlueNegative(true);

  palette_->setRedMax  (0.8);
  palette_->setGreenMax(0.4);

  palette_->setColorType(CGradientPalette::ColorType::DEFINED);

  palette_->resetDefinedColors();

  palette_->addDefinedColor(0.0, QColor("#4d81bc"));
  palette_->addDefinedColor(1.0, QColor("#c0504e"));
  palette_->addDefinedColor(2.0, QColor("#9bbb58"));
}

void
CQChartsView::
setLightThemeColors()
{
  theme_->setColorType(CGradientPalette::ColorType::DEFINED);

  theme_->resetDefinedColors();

  theme_->addDefinedColor(0.0, QColor("#ffffff"));
  theme_->addDefinedColor(1.0, QColor("#000000"));
}

void
CQChartsView::
setDarkThemeColors()
{
  theme_->setColorType(CGradientPalette::ColorType::DEFINED);

  theme_->resetDefinedColors();

  theme_->addDefinedColor(0.0, QColor("#222222"));
  theme_->addDefinedColor(1.0, QColor("#dddddd"));
}

//------

void
CQChartsView::
printSlot()
{
  QSvgGenerator generator;

  generator.setFileName("charts.svg");

  QPainter painter;

  painter.begin(&generator);

  paint(&painter);

  painter.end();
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
  for (auto &plotData : plotDatas_) {
    plotData.plot->update();
  }
}

//------

bool
CQChartsView::
plots(Plots &plots, bool clear) const
{
  if (clear)
    plots.clear();

  for (const auto &plotData : plotDatas_) {
    CQChartsPlot *plot1 = plotData.plot;

    plots.push_back(plot1);
  }

  return ! plots.empty();
}

bool
CQChartsView::
basePlots(PlotSet &plots, bool clear) const
{
  if (clear)
    plots.clear();

  for (const auto &plotData : plotDatas_) {
    CQChartsPlot *plot1 = plotData.plot->firstPlot();

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

  for (const auto &plotData : plotDatas_) {
    if (! plotData.bbox.inside(p))
      continue;

    plots.push_back(plotData.plot);

    if (plotData.plot == currentPlot)
      plot = plotData.plot;
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

  for (const auto &plotData : plotDatas_) {
    if (! plotData.bbox.inside(p))
      continue;

    CQChartsPlot *plot1 = plotData.plot->firstPlot();

    plots.insert(plot1);
  }

  return ! plots.empty();
}

CQChartsGeom::BBox
CQChartsView::
plotBBox(CQChartsPlot *plot) const
{
  for (const auto &plotData : plotDatas_)
    if (plotData.plot == plot)
      return plotData.bbox;

  return CQChartsGeom::BBox();
}

//---

int
CQChartsView::
plotInd(CQChartsPlot *plot) const
{
  int ind = 0;

  for (const auto &plotData : plotDatas_) {
    if (plotData.plot == plot)
      return ind;

    ++ind;
  }

  return -1;
}

CQChartsPlot *
CQChartsView::
currentPlot(bool remap) const
{
  if (plotDatas_.empty())
    return nullptr;

  int ind = currentPlotInd();

  if (ind < 0 || ind >= int(plotDatas_.size()))
    ind = 0;

  CQChartsPlot *plot = plotDatas_[ind].plot;

  if (remap)
    plot = plot->firstPlot();

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

QSize
CQChartsView::
sizeHint() const
{
  return QSize(1280, 1024);
}
