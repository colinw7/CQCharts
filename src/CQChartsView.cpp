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

  //setPaletteColors1();

  setLightThemeColors();

  //---

  addProperty("", this, "id"            );
  addProperty("", this, "title"         );
  addProperty("", this, "background"    );
  addProperty("", this, "currentPlotInd");
  addProperty("", this, "mode"          );
  addProperty("", this, "selectedMode"  );
  addProperty("", this, "insideMode"    );
  addProperty("", this, "zoomData"      );

  addProperty("scroll", this, "scrolled"      , "enabled" );
  addProperty("scroll", this, "scrollDelta"   , "delta"   );
  addProperty("scroll", this, "scrollNumPages", "numPages");
  addProperty("scroll", this, "scrollPage"    , "page"    );

  //---

  CQToolTip::setToolTip(this, new CQChartsViewToolTip(this));
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
setMode(const Mode &mode)
{
  if (mode != mode_) {
    if      (mode_ == Mode::PROBE) {
      for (auto &probeBand : probeBands_)
        probeBand->hide();
    }
    else if (mode_ == Mode::ZOOM) {
      if (zoomBand_)
        zoomBand_->hide();
    }

    mode_ = mode;

    emit modeChanged();
  }
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
  if (me->button() == Qt::LeftButton) {
    mouseData_.reset();

    mouseData_.pressPoint = me->pos();
    mouseData_.pressed    = true;

    if      (mode_ == Mode::SELECT) {
      CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

      mouseData_.plots.clear();

      plotsAt(w, mouseData_.plots);

      for (auto &plot : mouseData_.plots) {
        CQChartsGeom::Point w;

        plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        if (plot->mousePress(w))
          break;
      }
    }
    else if (mode_ == Mode::ZOOM) {
      CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

      CQChartsPlot *plot = plotAt(w);

      if (plot && plot->isClickZoom()) {
        mouseData_.clickZoom = true;

        plot->clickZoom(w);
      }
      else {
        mouseData_.plots.clear();

        plotsAt(w, mouseData_.plots);

        if (! zoomBand_)
          zoomBand_ = new QRubberBand(QRubberBand::Rectangle, this);

        zoomBand_->setGeometry(QRect(mouseData_.pressPoint, QSize()));
        zoomBand_->show();
      }
    }
    else if (mode_ == Mode::PROBE) {
    }
  }
  else if (me->button() == Qt::RightButton) {
    showMenu(me->pos());

    return;
  }
}

void
CQChartsView::
mouseMoveEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

    if (! mouseData_.pressed) {
      mouseData_.plots.clear();

      plotsAt(w, mouseData_.plots);
    }

    bool found = false;

    CQChartsPlot *plot1 = currentPlot(/*remap*/ false);

    for (auto &plot : mouseData_.plots) {
      if (plot == plot1) {
        found = true;
        break;
      }
    }

    bool first = true;

    if (found) {
      CQChartsGeom::Point w;

      plot1->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

      if (plot1->mouseMove(w, true))
        return;

      first = false;
    }

    for (auto &plot : mouseData_.plots) {
      if (plot1 == plot)
        continue;

      CQChartsGeom::Point w;

      plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

      if (plot->mouseMove(w, first))
        return;

      first = false;
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.clickZoom)
      return;

    if (mouseData_.pressed) {
      mouseData_.movePoint = me->pos();

      if (mouseData_.escape)
        zoomBand_->hide();
      else
        zoomBand_->setGeometry(QRect(mouseData_.pressPoint, mouseData_.movePoint));
    }
  }
  else if (mode_ == Mode::PROBE) {
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

    CQChartsGeom::Point w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

    mouseData_.plots.clear();

    plotsAt(w, mouseData_.plots);

    int probeInd = 0;

    for (auto &plot : mouseData_.plots) {
      CQChartsGeom::Point w;

      plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

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
  }
}

void
CQChartsView::
mouseReleaseEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    if (mouseData_.pressed) {
      for (auto &plot : mouseData_.plots) {
        CQChartsGeom::Point w;

        plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        plot->mouseRelease(w);
      }
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.clickZoom)
      return;

    if (mouseData_.pressed) {
      mouseData_.movePoint = me->pos();

      zoomBand_->hide();

      if (! mouseData_.escape) {
        if (! mouseData_.plots.empty()) {
          CQChartsPlot *plot = mouseData_.plots[0]->firstPlot();

          CQChartsGeom::Point w1, w2;

          plot->pixelToWindow(CQChartsUtil::fromQPointF(mouseData_.pressPoint), w1);
          plot->pixelToWindow(CQChartsUtil::fromQPointF(mouseData_.movePoint ), w2);

          CQChartsGeom::BBox bbox(w1, w2);

          plot->zoomTo(bbox);
        }
      }
    }
  }
  else if (mode_ == Mode::PROBE) {
  }

  mouseData_.reset();
}

void
CQChartsView::
keyPressEvent(QKeyEvent *ke)
{
  if      (ke->key() == Qt::Key_Escape) {
    mouseData_.escape = true;

    if      (mode() == Mode::ZOOM) {
      if (mouseData_.pressed)
        zoomBand_->hide();
      else
        setMode(Mode::SELECT);
    }
    else if (mode() == Mode::PROBE)
      setMode(Mode::SELECT);

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

  CQChartsPlot *plot = plotAt(w);

  if (plot)
    plot->keyPress(ke->key());
}

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

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //---

  painter.fillRect(CQChartsUtil::toQRect(prect_), QBrush(background()));

  //---

  for (const auto &plotData : plotDatas_) {
    if (plotData.plot->isVisible())
      plotData.plot->draw(&painter);
  }
}

//------

void
CQChartsView::
showMenu(const QPoint &p)
{
  if (! popupMenu_) {
    popupMenu_ = new QMenu(this);

    QAction *keyAction = new QAction("Key", popupMenu_);
    QAction *fitAction = new QAction("Fit", popupMenu_);

    keyAction->setCheckable(true);

    connect(keyAction, SIGNAL(triggered(bool)), this, SLOT(keySlot(bool)));
    connect(fitAction, SIGNAL(triggered()), this, SLOT(fitSlot()));

    popupMenu_->addAction(keyAction);
    popupMenu_->addAction(fitAction);

    QMenu *themeMenu = new QMenu("Theme");

    QAction *lightTheme1Action = new QAction("Light 1", themeMenu);
    QAction *lightTheme2Action = new QAction("Light 2", themeMenu);
    QAction *darkTheme1Action  = new QAction("Dark 1" , themeMenu);
    QAction *darkTheme2Action  = new QAction("Dark 2" , themeMenu);

    connect(lightTheme1Action, SIGNAL(triggered()), this, SLOT(lightTheme1Slot()));
    connect(lightTheme2Action, SIGNAL(triggered()), this, SLOT(lightTheme2Slot()));
    connect(darkTheme1Action , SIGNAL(triggered()), this, SLOT(darkTheme1Slot()));
    connect(darkTheme2Action , SIGNAL(triggered()), this, SLOT(darkTheme2Slot()));

    themeMenu->addAction(lightTheme1Action);
    themeMenu->addAction(lightTheme2Action);
    themeMenu->addAction(darkTheme1Action);
    themeMenu->addAction(darkTheme2Action);

    popupMenu_->addMenu(themeMenu);
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
  setPaletteColors1();

  setLightThemeColors();
}

void
CQChartsView::
lightTheme2Slot()
{
  setPaletteColors2();

  setLightThemeColors();
}

void
CQChartsView::
darkTheme1Slot()
{
  setPaletteColors1();

  setDarkThemeColors();
}

void
CQChartsView::
darkTheme2Slot()
{
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

  theme_->addDefinedColor(0.0, QColor("#000000"));
  theme_->addDefinedColor(1.0, QColor("#ffffff"));
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

CQChartsPlot *
CQChartsView::
plotAt(const CQChartsGeom::Point &p) const
{
  Plots plots;

  if (! plotsAt(p, plots))
    return nullptr;

  CQChartsPlot *plot = plots[0];

  return plot->firstPlot();
}

bool
CQChartsView::
plotsAt(const CQChartsGeom::Point &p, Plots &plots) const
{
  for (const auto &plotData : plotDatas_)
    if (plotData.bbox.inside(p))
      plots.push_back(plotData.plot);

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

void
CQChartsView::
setStatusText(const QString &text)
{
  emit statusTextChanged(text);
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
