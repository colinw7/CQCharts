#include <CQChartsView.h>
#include <CQChartsViewToolTip.h>
#include <CQChartsProbeBand.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsTitle.h>
#include <CQChartsUtil.h>
#include <CGradientPalette.h>
#include <CQPropertyViewModel.h>
#include <CDisplayRange2D.h>

#include <svg/select_svg.h>
#include <svg/zoom_svg.h>
#include <svg/probe_svg.h>
#include <svg/zoom_fit_svg.h>

#include <QHBoxLayout>
#include <QRubberBand>
#include <QMouseEvent>
#include <QPainter>

CQChartsView::
CQChartsView(CQCharts *charts, QWidget *parent) :
 QFrame(parent), charts_(charts)
{
  setObjectName("view");

  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  //---

  displayRange_ = new CDisplayRange2D;

  double vr = viewportRange();

  displayRange_->setWindowRange(0, 0, vr, vr);

  //---

  palette_       = new CGradientPalette;
  propertyModel_ = new CQPropertyViewModel;

  //---

  addProperty("", this, "id"            );
  addProperty("", this, "title"         );
  addProperty("", this, "background"    );
  addProperty("", this, "currentPlotInd");
  addProperty("", this, "mode"          );
  addProperty("", this, "zoomData"      );

  //---

  CQToolTip::setToolTip(this, new CQChartsViewToolTip(this));
}

CQChartsView::
~CQChartsView()
{
  delete palette_;
  delete propertyModel_;

  delete displayRange_;

  for (auto &plot : plotDatas_)
    delete plot.plot;

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
addPlot(CQChartsPlot *plot, const CBBox2D &bbox)
{
  plot->setPalette(gradientPalette());

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

  if (title().length())
    firstPlot->titleObj()->setText(title());

  //---

  for (const auto &plotData : plotDatas_) {
    if (plotData.plot == firstPlot)
      continue;

    CQChartsAxis *xaxis = plotData.plot->xAxis();
    CQChartsAxis *yaxis = plotData.plot->yAxis();

    if (xaxis)
      xaxis->setVisible(false);

    if (yaxis)
      yaxis->setVisible(false);

    CQChartsTitle *title = plotData.plot->titleObj();

    if (title)
      title->setVisible(false);
  }

  firstPlot->updateObjs();
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
      CPoint2D w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

      mouseData_.plots.clear();

      plotsAt(w, mouseData_.plots);

      for (auto &plot : mouseData_.plots) {
        CPoint2D w;

        plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        if (plot->mousePress(w))
          break;
      }
    }
    else if (mode_ == Mode::ZOOM) {
      CPoint2D w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

      CQChartsPlot *plot = plotAt(w);

      if (plot && plot->isClickZoom()) {
        mouseData_.clickZoom = true;

        plot->clickZoom(w);
      }
      else {
        if (! zoomBand_)
          zoomBand_ = new QRubberBand(QRubberBand::Rectangle, this);

        zoomBand_->setGeometry(QRect(mouseData_.pressPoint, QSize()));
        zoomBand_->show();
      }
    }
    else if (mode_ == Mode::PROBE) {
    }
  }
}

void
CQChartsView::
mouseMoveEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    CPoint2D w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

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
      CPoint2D w;

      plot1->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

      if (plot1->mouseMove(w, true))
        return;

      first = false;
    }

    for (auto &plot : mouseData_.plots) {
      if (plot1 == plot)
        continue;

      CPoint2D w;

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
    else {
      CPoint2D w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

      mouseData_.plots.clear();

      plotsAt(w, mouseData_.plots);

      for (auto &plot : mouseData_.plots) {
        CPoint2D w;

        plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

        if (plot->mouseMove(w))
          break;
      }
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

    int px = me->pos().x();

    CPoint2D w = pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())));

    mouseData_.plots.clear();

    plotsAt(w, mouseData_.plots);

    int probeInd = 0;

    for (auto &plot : mouseData_.plots) {
      CPoint2D w;

      plot->pixelToWindow(CQChartsUtil::fromQPoint(QPointF(me->pos())), w);

      std::vector<double> yvals1;

      if (! plot->interpY(w.x, yvals1))
        continue;

      CBBox2D dataRange = plot->calcDataRange();

      CPoint2D p1;

      plot->windowToPixel(CPoint2D(w.x, dataRange.getYMin()), p1);

      for (const auto &y1 : yvals1) {
        CPoint2D p2;

        plot->windowToPixel(CPoint2D(w.x, y1), p2);

        QString tip = plot->yStr(y1);

        addProbeBand(probeInd, plot, tip, px, p1.y, p2.y);
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
        CPoint2D w;

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

          CPoint2D w1, w2;

          plot->pixelToWindow(CQChartsUtil::fromQPointF(mouseData_.pressPoint), w1);
          plot->pixelToWindow(CQChartsUtil::fromQPointF(mouseData_.movePoint ), w2);

          CBBox2D bbox(w1, w2);

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

  CPoint2D w = pixelToWindow(CQChartsUtil::fromQPoint(pos));

  CQChartsPlot *plot = plotAt(w);

  if (plot)
    plot->keyPress(ke->key());
}

void
CQChartsView::
resizeEvent(QResizeEvent *)
{
  prect_ = CBBox2D(0, 0, width(), height());

  aspect_ = (1.0*prect().getWidth())/prect().getHeight();

  displayRange_->setPixelRange(prect_.getXMin(), prect_.getYMin(),
                               prect_.getXMax(), prect_.getYMax());

  //---

  for (const auto &plot : plotDatas_)
    plot.plot->handleResize();
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

  for (const auto &plot : plotDatas_) {
    if (plot.plot->isVisible())
      plot.plot->draw(&painter);
  }
}

CQChartsPlot *
CQChartsView::
plotAt(const CPoint2D &p) const
{
  Plots plots;

  if (! plotsAt(p, plots))
    return nullptr;

  CQChartsPlot *plot = plots[0];

  return plot->firstPlot();
}

bool
CQChartsView::
plotsAt(const CPoint2D &p, Plots &plots) const
{
  for (const auto &plot : plotDatas_)
    if (plot.bbox.inside(p))
      plots.push_back(plot.plot);

  return ! plots.empty();
}

CBBox2D
CQChartsView::
plotBBox(CQChartsPlot *plot) const
{
  for (const auto &plotData : plotDatas_)
    if (plotData.plot == plot)
      return plotData.bbox;

  return CBBox2D();
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

CPoint2D
CQChartsView::
windowToPixel(const CPoint2D &w) const
{
  CPoint2D p;

  windowToPixel(w.x, w.y, p.x, p.y);

  return p;
}

CPoint2D
CQChartsView::
pixelToWindow(const CPoint2D &p) const
{
  CPoint2D w;

  pixelToWindow(p.x, p.y, w.x, w.y);

  return w;
}

CBBox2D
CQChartsView::
windowToPixel(const CBBox2D &wrect) const
{
  double px1, py1, px2, py2;

  windowToPixel(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixel(wrect.getXMax(), wrect.getYMax(), px2, py1);

  return CBBox2D(px1, py1, px2, py2);
}

CBBox2D
CQChartsView::
pixelToWindow(const CBBox2D &prect) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(prect.getXMin(), prect.getYMin(), wx1, wy2);
  pixelToWindow(prect.getXMax(), prect.getYMax(), wx2, wy1);

  return CBBox2D(wx1, wy1, wx2, wy2);
}

QSize
CQChartsView::
sizeHint() const
{
  return QSize(1280, 1024);
}
