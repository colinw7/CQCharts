#include <CQChartsPlotOverview.h>
#include <CQChartsPlot.h>

#include <CQWidgetUtil.h>
#include <CQUtil.h>

#include <QMenu>
#include <QPainter>
#include <QMouseEvent>

CQChartsPlotOverview::
CQChartsPlotOverview(CQChartsPlot *plot) :
 plot_(plot)
{
  setObjectName("overview");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  setContextMenuPolicy(Qt::DefaultContextMenu);

  if (plot_) {
    connect(plot_, SIGNAL(overviewChanged()), this, SLOT(overviewChanged()));
    connect(plot_, SIGNAL(zoomPanChanged()), this, SLOT(overviewChanged()));
  }
}

CQChartsPlot *
CQChartsPlotOverview::
plot() const
{
  return plot_.data();
}

void
CQChartsPlotOverview::
setPlot(CQChartsPlot *plot)
{
  if (plot_) {
    disconnect(plot_, SIGNAL(overviewChanged()), this, SLOT(overviewChanged()));
    disconnect(plot_, SIGNAL(zoomPanChanged()), this, SLOT(overviewChanged()));
  }

  plot_ = plot;

  if (plot_) {
    connect(plot_, SIGNAL(overviewChanged()), this, SLOT(overviewChanged()));
    connect(plot_, SIGNAL(zoomPanChanged()), this, SLOT(overviewChanged()));
  }
}

void
CQChartsPlotOverview::
overviewChanged()
{
  int s = 256;

  if (plot())
    s = plot()->overviewSize();

  if (overviewSize_ != s) {
    overviewSize_ = s;

    resize(sizeHint());

    if (plot()->isOverviewDisplayed()) {
      setVisible(false);
      setVisible(true);
    }
  }

  update();
}

void
CQChartsPlotOverview::
mousePressEvent(QMouseEvent *e)
{
  pressed_     = true;
  pressButton_ = e->button();

  if (pressButton_ != Qt::LeftButton)
    return;

  auto *plot = this->plot();
  if (! plot) return;

  plot->centerAt(pixelToPlot(Point(e->pos())));
}

void
CQChartsPlotOverview::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_) {
    if (pressButton_ != Qt::LeftButton)
      return;

    auto *plot = this->plot();
    if (! plot) return;

    plot->centerAt(pixelToPlot(Point(e->pos())));
  }
}

void
CQChartsPlotOverview::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_     = false;
  pressButton_ = -1;
}

void
CQChartsPlotOverview::
wheelEvent(QWheelEvent *e)
{
  auto *plot = this->plot();
  if (! plot) return;

  auto delta = CQWidgetUtil::wheelDelta(e);

  double zoomFactor = 1.10;

  auto pp1 = pixelToPlot(Point(e->position()));

  if      (delta > 0)
    plot->updateDataScale(plot->commonDataScale()*zoomFactor);
  else if (delta < 0)
    plot->updateDataScale(plot->commonDataScale()/zoomFactor);

  auto pp2 = pixelToPlot(Point(e->position()));

  plot->pan(pp1.x - pp2.x, pp1.y - pp2.y);
}

void
CQChartsPlotOverview::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *plot = this->plot();
  if (! plot) return;

  auto *menu = new QMenu(this);

  CQUtil::addAction(menu, "Zoom Full", plot, SLOT(zoomFull()));
  CQUtil::addAction(menu, "Update", plot, SLOT(updateOverview()));

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQChartsPlotOverview::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), Qt::white);

  auto *plot = this->plot();
  if (! plot) return;

  auto image = plot->overviewImage();
  if (image.isNull()) return;

  painter.drawImage(0, 0, image);

  auto plotRect1 = plot->overviewPlotRect();
  auto plotRect2 = plot->calcPlotRect();

  auto mapX = [&](double x) {
    return CMathUtil::map(x, plotRect1.getXMin(), plotRect1.getXMax(), 0, image.width() - 1);
  };

  auto mapY = [&](double y) {
    return CMathUtil::map(y, plotRect1.getYMin(), plotRect1.getYMax(), image.height() - 1, 0);
  };

  auto x1 = mapX(plotRect2.getXMin());
  auto y1 = mapY(plotRect2.getYMin());
  auto x2 = mapX(plotRect2.getXMax());
  auto y2 = mapY(plotRect2.getYMax());

  auto plotRect = QRectF(x1, y1, x2 - x1, y2 - y1);

  auto fc = plot->overviewFillColor();
  auto fa = plot->overviewFillAlpha();

  fc.setAlphaF(fa);

  painter.fillRect(plotRect, fc);

  auto sc = plot->overviewStrokeColor();
  auto sa = plot->overviewStrokeAlpha();

  sc.setAlphaF(sa);

  painter.setPen(sc);

  painter.drawRect(plotRect);
}

CQChartsGeom::Point
CQChartsPlotOverview::
pixelToPlot(const Point &pp) const
{
  Point wp;

  auto *plot = this->plot();
  if (! plot) return wp;

  auto image = plot->overviewImage();
  if (image.isNull()) return wp;

  auto plotRect1 = plot->overviewPlotRect();

  auto mapX = [&](double px) {
    return CMathUtil::map(px, 0, image.width() - 1, plotRect1.getXMin(), plotRect1.getXMax());
  };

  auto mapY = [&](double py) {
    return CMathUtil::map(py, image.height() - 1, 0, plotRect1.getYMin(), plotRect1.getYMax());
  };

  return Point(mapX(pp.x), mapY(pp.y));
}

QSize
CQChartsPlotOverview::
sizeHint() const
{
  int s = 256;

  auto *plot = this->plot();
  if (! plot) return QSize(s, s);

  s = plot->overviewSize();

  auto image = plot->overviewImage();
  if (image.isNull()) return QSize(s, s);

  return image.size();
}
