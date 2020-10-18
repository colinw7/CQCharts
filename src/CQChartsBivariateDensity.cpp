#include <CQChartsBivariateDensity.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsPlot.h>

#include <CMathCorrelation.h>
#include <CMathRound.h>

CQChartsBivariateDensity::
CQChartsBivariateDensity()
{
}

CQChartsBivariateDensity::
~CQChartsBivariateDensity()
{
  delete bivariate_;
}

void
CQChartsBivariateDensity::
calc(const CQChartsPlot *plot, const Data &data)
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
    double x1 = (xmax > xmin ? CMathUtil::norm(v.x, xmin, xmax) : 0.0);
    double y1 = (ymax > ymin ? CMathUtil::norm(v.y, ymin, ymax) : 0.0);

    xv.push_back(x1);
    yv.push_back(y1);
  }

  delete bivariate_;

  bivariate_ = new CMathBivariate(xv, yv);

  //---

  // calc pixel grid
  BBox bbox(xmin, ymin, xmax, ymax);

  auto pbbox = plot->windowToPixel(bbox);

  auto ll = pbbox.getLL();
  auto ur = pbbox.getUR();

  int x1 = CMathRound::RoundDown(ll.x);
  int x2 = CMathRound::RoundUp  (ur.x);
  int y1 = CMathRound::RoundUp  (ll.y);
  int y2 = CMathRound::RoundDown(ur.y);

  assert(x1 <= x2);
  assert(y1 <= y2);

  int dx = gridSize;
  int dy = gridSize;

  //---

  // calc values for grid
  cells_.clear();

  for (int y = y1; y <= y2; y += dy) {
    for (int x = x1; x <= x2; x += dx) {
      // calc value at normalized pixel sample point (use box center ?)
      auto p = plot->pixelToWindow(Point(x, y));

      double x1 = (xmax > xmin ? CMathUtil::norm(p.x, xmin, xmax) : 0.0);
      double y1 = (ymax > ymin ? CMathUtil::norm(p.y, ymin, ymax) : 0.0);

      //---

      Cell cell;

      cell.bbox  = plot->pixelToWindow(BBox(x, y, x + dx, y + dy));
      cell.value = bivariate_->calc(x1, y1);

      //---

      // set alpha for delta (if defined)
      cell.alpha = 1.0;

      if (delta > 0.0) {
        double v1 = CMathRound::RoundDown(cell.value/delta)*delta;

        cell.alpha = CMathUtil::clamp(sqrt(1.0 - (cell.value - v1)), 0.0, 1.0);
      }

      //---

      cells_.push_back(cell);
    }
  }
}

void
CQChartsBivariateDensity::
draw(const CQChartsPlot *plot, CQChartsPaintDevice *device)
{
  for (const auto &cell : cells_) {
    // set brush
    CQChartsPenBrush penBrush;

    auto c = plot->interpPaletteColor(CQChartsUtil::ColorInd(cell.value));

    plot->setPenBrush(penBrush,
      CQChartsPenData  (false),
      CQChartsBrushData(true, c, CQChartsAlpha(cell.alpha), CQChartsFillPattern()));

    //---

    // fill rect
    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->fillRect(cell.bbox);
  }
}

double
CQChartsBivariateDensity::
xStdDev() const
{
  return (bivariate_ ? bivariate_->xStdDev() : 0.0);
}

double
CQChartsBivariateDensity::
yStdDev() const
{
  return (bivariate_ ? bivariate_->yStdDev() : 0.0);
}

