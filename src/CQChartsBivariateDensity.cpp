#include <CQChartsBivariateDensity.h>
#include <CQChartsPaintDevice.h>

#include <CMathCorrelation.h>
#include <CMathRound.h>

void
CQChartsBivariateDensity::
draw(const CQChartsPlot *plot, CQChartsPaintDevice *device, const Data &data)
{
  const int gridSize = std::max(data.gridSize, 1);

  const double delta = data.delta;

  const double xmin = data.xrange.min();
  const double xmax = data.xrange.max();
  const double ymin = data.yrange.min();
  const double ymax = data.yrange.max();

  //---

  // create bivariate map of normalized values
  std::vector<double> xv;
  std::vector<double> yv;

  for (const auto &v : data.values) {
    double x1 = (xmax > xmin ? CMathUtil::norm(v.x(), xmin, xmax) : 0.0);
    double y1 = (ymax > ymin ? CMathUtil::norm(v.y(), ymin, ymax) : 0.0);

    xv.push_back(x1);
    yv.push_back(y1);
  }

  CMathBivariate bivariate(xv, yv);

  //---

  // calc pixel grid
  QRectF pr = device->windowToPixel(QRectF(xmin, ymin, xmax - xmin, ymax - ymin));

  QPointF ll = pr.bottomLeft();
  QPointF ur = pr.topRight  ();

  int x1 = CMathRound::RoundDown(ll.x());
  int x2 = CMathRound::RoundUp  (ur.x());
  int y2 = CMathRound::RoundUp  (ll.y());
  int y1 = CMathRound::RoundDown(ur.y());

  int dx = gridSize;
  int dy = gridSize;

  //---

  // calc values for grid
  for (int y = y1; y <= y2; y += dy) {
    for (int x = x1; x <= x2; x += dx) {
      // calc value at normalized pixel sample point (use box center ?)
      QPointF p = device->pixelToWindow(QPointF(x, y));

      double x1 = (xmax > xmin ? CMathUtil::norm(p.x(), xmin, xmax) : 0.0);
      double y1 = (ymax > ymin ? CMathUtil::norm(p.y(), ymin, ymax) : 0.0);

      double v = bivariate.calc(x1, y1);

      //---

      // set alpha for delta (if defined)
      double a = 1.0;

      if (delta > 0.0) {
        double v1 = CMathRound::RoundDown(v/delta)*delta;

        a = CMathUtil::clamp(sqrt(1.0 - (v - v1)), 0.0, 1.0);
      }

      //---

      // set brush
      QBrush brush;

      QColor c = plot->interpPaletteColor(CQChartsUtil::ColorInd(v));

      plot->setBrush(brush, true, c, a, CQChartsFillPattern());

      //---

      // fill rect
      QRectF pr1 = QRectF(x, y, dx, dy);

      device->fillRect(device->pixelToWindow(pr1), brush);
    }
  }
}
