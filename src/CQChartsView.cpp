#include <CQChartsView.h>
#include <CQChartsViewExpander.h>
#include <CQChartsViewSettings.h>
#include <CQChartsViewStatus.h>
#include <CQChartsViewToolBar.h>
#include <CQChartsViewToolTip.h>
#include <CQChartsProbeBand.h>
#include <CQChartsPlot.h>
#include <CQPropertyView.h>
#include <CQGradientPalette.h>
#include <CQGradientPaletteControl.h>
#include <CQUtil.h>

#include <svg/select_svg.h>
#include <svg/zoom_svg.h>
#include <svg/probe_svg.h>
#include <svg/zoom_fit_svg.h>

#include <QToolButton>
#include <QHBoxLayout>
#include <QRubberBand>
#include <QMouseEvent>
#include <QPainter>

CQChartsView::
CQChartsView(QWidget *parent) :
 parent_(parent)
{
  setObjectName("view");

  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  //---

  displayRange_.setWindowRange(0, 0, 1000, 1000);

  //---

  expander_ = new CQChartsViewExpander(this);
  settings_ = new CQChartsViewSettings(this);
  status_   = new CQChartsViewStatus(this);
  toolbar_  = new CQChartsViewToolBar(this);

  //---

  addProperty("", this, "background");
  addProperty("", this, "mode"      );
  addProperty("", this, "zoomData"  );

  //---

  updateMargins();

  CQToolTip::setToolTip(this, new CQChartsViewToolTip(this));
}

CQChartsView::
~CQChartsView()
{
  delete expander_;
  delete settings_;
  delete status_;
  delete toolbar_;
}

CQPropertyView *
CQChartsView::
propertyView() const
{
  return settings_->propertyView();
}

void
CQChartsView::
setMode(const Mode &mode)
{
  if (mode != mode_) {
    if      (mode_ == Mode::PROBE) {
      for (int i = 0; i < int(probeBands_.size()); ++i)
        probeBands_[i]->hide();
    }
    else if (mode_ == Mode::ZOOM) {
      if (zoomBand_)
        zoomBand_->hide();
    }

    mode_ = mode;

    toolbar_->updateMode();
  }
}

void
CQChartsView::
updateMargins()
{
  statusHeight_  = status_ ->sizeHint().height();
  toolBarHeight_ = toolbar_->sizeHint().height();
}

void
CQChartsView::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  propertyView()->addProperty(path, object, name, alias);
}

void
CQChartsView::
addPlot(CQChartsPlot *plot, const CBBox2D &bbox)
{
  plot->setPalette(settings_->palettePlot()->gradientPalette());

  plot->setBBox(bbox);

  plotDatas_.push_back(PlotData(plot, bbox));

  plot->addProperties();
}

void
CQChartsView::
mousePressEvent(QMouseEvent *me)
{
  if (me->button() == Qt::LeftButton) {
    mouseData_.pressPoint = me->pos();
    mouseData_.pressed    = true;
    mouseData_.escape     = false;

    if      (mode_ == Mode::SELECT) {
      CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

      mouseData_.plots.clear();

      plotsAt(w, mouseData_.plots);

      for (auto &plot : mouseData_.plots) {
        CPoint2D w;

        plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

        if (plot->mousePress(w))
          break;
      }
    }
    else if (mode_ == Mode::ZOOM) {
      if (! zoomBand_)
        zoomBand_ = new QRubberBand(QRubberBand::Rectangle, this);

      zoomBand_->setGeometry(QRect(mouseData_.pressPoint, QSize()));
      zoomBand_->show();
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
    CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

    if (! mouseData_.pressed) {
      mouseData_.plots.clear();

      plotsAt(w, mouseData_.plots);
    }

    for (auto &plot : mouseData_.plots) {
      CPoint2D w;

      plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

      if (plot->mouseMove(w))
        break;
    }
  }
  else if (mode_ == Mode::ZOOM) {
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

    int px = me->pos().x();

    CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

    mouseData_.plots.clear();

    plotsAt(w, mouseData_.plots);

    int probeInd = 0;

    for (auto &plot : mouseData_.plots) {
      CPoint2D w;

      plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

      std::vector<double> yvals1;

      if (! plot->interpY(w.x, yvals1))
        continue;

      CBBox2D dataRange = plot->calcDataRange();

      CPoint2D p1;

      plot->windowToPixel(CPoint2D(w.x, dataRange.getYMin()), p1);

      for (const auto &y1 : yvals1) {
        CPoint2D p2;

        plot->windowToPixel(CPoint2D(w.x, y1), p2);

        QString tip = QString("%1").arg(y1);

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

        plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

        plot->mouseRelease(w);
      }
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.pressed) {
      mouseData_.movePoint = me->pos();

      zoomBand_->hide();

      if (! mouseData_.escape) {
        if (! mouseData_.plots.empty()) {
          CQChartsPlot *plot = mouseData_.plots[0]->firstPlot();

          CPoint2D w1, w2;

          plot->pixelToWindow(CQUtil::fromQPointF(mouseData_.pressPoint), w1);
          plot->pixelToWindow(CQUtil::fromQPointF(mouseData_.movePoint ), w2);

          CBBox2D bbox(w1, w2);

          plot->zoomTo(bbox);
        }
      }
    }
  }
  else if (mode_ == Mode::PROBE) {
  }

  mouseData_.pressed = false;
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

  CPoint2D w = pixelToWindow(CQUtil::fromQPoint(pos));

  CQChartsPlot *plot = plotAt(w);

  if (plot)
    plot->keyPress(ke->key());
}

void
CQChartsView::
resizeEvent(QResizeEvent *)
{
  updateGeometry();

  for (const auto &plot : plotDatas_)
    plot.plot->handleResize();
}

void
CQChartsView::
updateGeometry()
{
  setMinimumSize(16 + expander_->width(), 16 + statusHeight_ + toolBarHeight_);

  //---

  prect_ = CBBox2D(0, toolBarHeight_, width() - expander_->width(), height() - statusHeight_);

  //---

  displayRange_.setPixelRange(prect_.getXMin(), prect_.getYMin(),
                              prect_.getXMax(), prect_.getYMax());

  settings_->setVisible(expander_->isExpanded());

  if (expander_->isExpanded()) {
    settings_->move  (width() - settings_->width(), 0);
    settings_->resize(settings_->width(), height());
  }

  //---

  expander_->setVisible(true);

  if (expander_->isExpanded())
    expander_->move(width() - settings_->width() - expander_->width(), 0);
  else
    expander_->move(width() - expander_->width(), 0);

  expander_->resize(expander_->width(), height());

  //---

  toolbar_->move(0, 0);
  toolbar_->resize(width(), toolBarHeight_);

  //---

  status_->move(0, height() - statusHeight_);
  status_->resize(width(), statusHeight_);
}

void
CQChartsView::
moveExpander(int dx)
{
  expander_->move(expander_->x() - dx, expander_->y());

  settings_->resize(settings_->width() + dx, settings_->height());

  settings_->move(settings_->x() - dx, settings_->y());
}

void
CQChartsView::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //---

  painter.fillRect(CQUtil::toQRect(prect_), QBrush(background()));

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
currentPlot() const
{
  if (plotDatas_.empty())
    return nullptr;

  CQChartsPlot *plot = plotDatas_[0].plot;

  return plot->firstPlot();
}

void
CQChartsView::
setStatusText(const QString &text)
{
  status_->setText(text);
}

void
CQChartsView::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  displayRange_.windowToPixel(wx, wy, &px, &py);
}

void
CQChartsView::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  displayRange_.pixelToWindow(px, py, &wx, &wy);
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
