#include <CQChartsDensity.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsBoxWhisker.h>
#include <CQUtil.h>
#include <cassert>

CQChartsDensity::
CQChartsDensity()
{
}

void
CQChartsDensity::
constCalc() const
{
  const_cast<CQChartsDensity *>(this)->calc();
}

void
CQChartsDensity::
calc()
{
  if (calced_)
    return;

  calced_ = true;

  //---

  init();

  ymin_ = 0.0;
  ymax_ = 0.0;

  if (nx_ < 2)
    return;

  // set num samples between end points
  double step = (xmax_ - xmin_)/(numSamples_ - 1);

  Points points;

  points.resize(numSamples_);

  for (int i = 0; i < numSamples_; i++) {
    double x = xmin_ + i*step;
    double y = eval(x);

    points[i] = Point(x, y);

    if (i == 0) {
      ymin_ = y;
      ymax_ = y;
    }
    else {
      ymin_ = std::min(ymin_, y);
      ymax_ = std::max(ymax_, y);
    }
  }

  // extend left/right so endpoints both at ymin
  Points lpoints;

  {
    double step1 = step;

    double x = xmin_;
    double y = eval(x);

    while (step1 > 1E-5 && (y - ymin_) > 1E-5) {
      x -= step1;
      y  = eval(x);

      while (step1 > 1E-5 && (y - ymin_) < 0.0) {
        step1 /= 2.0;

        x += step1;
        y  = eval(x);
      }

      lpoints.emplace_back(x, y);
    }
  }

  int nl = lpoints.size();

  //--

  Points rpoints;

  {
    double step1 = step;

    double x = xmax_;
    double y = eval(x);

    while (step1 > 1E-5 && (y - ymin_) > 1E-5) {
      x += step1;
      y  = eval(x);

      while (step1 > 1E-5 && (y - ymin_) < 0.0) {
        step1 /= 2.0;

        x -= step1;
        y  = eval(x);
      }

      rpoints.emplace_back(x, y);
    }
  }

  int nr = rpoints.size();

  //---

  xmin1_ = (nl > 0 ? lpoints[nl - 1].x : xmin_);
  xmax1_ = (nr > 0 ? rpoints[nr - 1].x : xmax_);

  //---

  // normalize polygon
  double xl = xmax1_ - xmin1_;
  double yl = ymax_ - ymin_;

  Polygon poly;

  int np = numSamples_ + nr + nl;

  poly.resize(np);

  int i = 0;

  for (auto &p : lpoints) {
    poly.setPoint(nl - i - 1, Point((p.x - xmin1_)/xl, std::max(p.y - ymin_, 0.0)/yl));

    ++i;
  }

  for (auto &p : points) {
    poly.setPoint(i, Point((p.x - xmin1_)/xl, std::max(p.y - ymin_, 0.0)/yl));

    ++i;
  }

  for (auto &p : rpoints) {
    poly.setPoint(i, Point((p.x - xmin1_)/xl, std::max(p.y - ymin_, 0.0)/yl));

    ++i;
  }

  //---

  // scale y to area
  area_ = poly.area();

  opoints_.resize(np);

  for (int i = 0; i < np; ++i) {
    auto p = poly.point(i);

    opoints_[i] = Point(p.x*xl + xmin1_, p.y/area_);
  }

  ymin1_ = 0.0;
  ymax1_ = 1.0/area_;
}

//---

void
CQChartsDensity::
constInit() const
{
  const_cast<CQChartsDensity *>(this)->init();
}

void
CQChartsDensity::
init()
{
  if (initialized_)
    return;

  initialized_ = true;

  //---

  sxvals_ = xvals_;

  std::sort(sxvals_.begin(), sxvals_.end());

  statData_.calcStatValues(sxvals_);

  //---

  // calc x range, average and sigma
  xmin_  = 0.0;
  xmax_  = 0.0;
  avg_   = 0.0;
  sigma_ = 0.0;

  nx_ = xvals_.size();

  for (int i = 0; i < nx_; i++) {
    double x = xvals_[i];

    if (i == 0) {
      xmin_ = x;
      xmax_ = x;
    }
    else {
      xmin_ = std::min(xmin_, x);
      xmax_ = std::max(xmax_, x);
    }

    avg_   += x;
    sigma_ += x*x;
  }

  if (nx_ > 0) {
    avg_ /= double(nx_);

    sigma_ = sqrt(sigma_/double(nx_) - avg_*avg_); /* Standard Deviation */
  }

  //---

  /* This is the optimal bandwidth if the point distribution is Gaussian.
     (Applied Smoothing Techniques for Data Analysis
     by Adrian W, Bowman & Adelchi Azzalini (1997)) */
  defaultBandwidth_ = pow(4.0/(3.0*nx_), 1.0/5.0)*sigma_;

  ymin_ = 0.0;
  ymax_ = 0.0;
}

//---

double
CQChartsDensity::
yval(double x) const
{
  double y = eval(x);

  return (y - ymin())/area();
}

//---

double
CQChartsDensity::
eval(double x) const
{
  assert(initialized_ && calced_);

  double bandwidth;

  /* If the supplied bandwidth is zero of less, the default bandwidth is used. */
  if (smoothParameter_ <= 0)
    bandwidth = defaultBandwidth_;
  else
    bandwidth = smoothParameter_;

  //---

  double y = 0;

  for (int i = 0; i < nx_; i++) {
    double z = (x - xvals_[i])/bandwidth;

    y += exp(-0.5*z*z)/bandwidth;
  }

  y /= sqrt(2.0*M_PI);

  return y;
}

//---

void
CQChartsDensity::
draw(const CQChartsPlot *plot, PaintDevice *device, const BBox &rect)
{
  constInit();

  BBox rect1;

  if (isHorizontal())
    rect1 = BBox(statData_.min, rect.getYMin(), statData_.max, rect.getYMax());
  else
    rect1 = BBox(rect.getXMin(), statData_.min, rect.getXMax(), statData_.max);

  //---

  CQChartsWhiskerOpts opts;
  CQChartsSymbolData  symbolData;

  double mean = avg_;

  switch (drawType()) {
    case DrawType::WHISKER:
      drawWhisker(device, rect1, orientation());
      break;
    case DrawType::WHISKER_BAR:
      drawWhiskerBar(device, rect1, orientation());
      break;
    case DrawType::DISTRIBUTION:
      drawDistribution(plot, device, rect1, orientation(), opts);
      break;
    case DrawType::CROSS_BAR:
      drawCrossBar(device, rect1, mean, orientation(), CQChartsLength());
      break;
    case DrawType::POINT_RANGE:
      drawPointRange(device, rect1, mean, orientation(), symbolData);
      break;
    case DrawType::ERROR_BAR:
      drawErrorBar(device, rect1, mean, orientation(), symbolData);
      break;
    default:
      break;
  }
}

CQChartsGeom::BBox
CQChartsDensity::
bbox(const BBox &rect) const
{
  BBox rect1;

  if (isHorizontal())
    rect1 = BBox(statData_.min, rect.getYMin(), statData_.max, rect.getYMax());
  else
    rect1 = BBox(rect.getXMin(), statData_.min, rect.getXMax(), statData_.max);

  return rect1;
}

//---

void
CQChartsDensity::
drawWhisker(PaintDevice *device, const BBox &rect, const Qt::Orientation &orientation) const
{
  CQChartsLength ws;

  if (orientation == Qt::Horizontal)
    ws = CQChartsLength(rect.getHeight(), CQChartsUnits::PLOT);
  else
    ws = CQChartsLength(rect.getWidth (), CQChartsUnits::PLOT);

  std::vector<double> outliers;

  CQChartsBoxWhiskerUtil::drawWhisker(device, statData_, outliers, rect, ws, orientation);
}

void
CQChartsDensity::
drawWhiskerBar(PaintDevice *device, const BBox &rect, const Qt::Orientation &orientation) const
{
  std::vector<double> outliers;

  for (const auto &x : sxvals_) {
    if (statData_.isOutlier(x))
      outliers.push_back(x);
  }

  CQChartsLength cornerSize;
  bool           notched { false };
  bool           median  { true };

  if (orientation == Qt::Horizontal)
    CQChartsBoxWhiskerUtil::drawWhiskerBar(device, statData_, rect.getYMid(),
                                           orientation, rect.getHeight(), rect.getHeight(),
                                           cornerSize, notched, median, outliers);
  else
    CQChartsBoxWhiskerUtil::drawWhiskerBar(device, statData_, rect.getXMid(),
                                           orientation, rect.getWidth(), rect.getWidth(),
                                           cornerSize, notched, median, outliers);
}

//---

void
CQChartsDensity::
drawDistribution(const Plot *plot, PaintDevice *device, const BBox &rect,
                 const Qt::Orientation &orientation, const CQChartsWhiskerOpts &opts) const
{
  Polygon poly;

  calcDistributionPoly(poly, plot, rect, orientation, opts);

  device->drawPolygon(poly);
}

void
CQChartsDensity::
calcDistributionPoly(Polygon &poly, const CQChartsPlot *plot, const BBox &rect,
                     const Qt::Orientation &orientation, const CQChartsWhiskerOpts &opts) const
{
  assert(rect.isSet());

  const auto &dataRange = plot->dataRange();

  const auto &opoints = this->opoints();

  double xmin = this->xmin();
  double xmax = this->xmax();

  double xmin1 = this->xmin1();
  double xmax1 = this->xmax1();

  double ymin1 = this->ymin1();
  double ymax1 = this->ymax1();

  double dw = rect.getWidth ();
  double dh = rect.getHeight();

  bool bottomLeft = false;

  double vxs, vys;

  if (orientation != Qt::Horizontal) {
    bottomLeft = (rect.getXMid() < dataRange.xmid());

    if (opts.fitTail)
      vxs = dh/(xmax1 - xmin1);
    else
      vxs = dh/(xmax - xmin);

    vys = dw/(ymax1 - ymin1);
  }
  else {
    bottomLeft = (rect.getYMid() < dataRange.ymid());

    if (opts.fitTail)
      vxs = dw/(xmax1 - xmin1);
    else
      vxs = dw/(xmax - xmin);

    vys = dh/(ymax1 - ymin1);
  }

  if (opts.violin)
    vys /= 2.0;

  int no = opoints.size();
  int np = (opts.violin ? 2*no + 2 : no + 2);

  poly.resize(np);

  double px, py;

  if (orientation != Qt::Horizontal) {
    if (! opts.violin)
      px = (bottomLeft ? rect.getXMax() : rect.getXMin());
    else
      px = rect.getXMid();

    py = rect.getYMin();
  }
  else {
    px = rect.getXMin();

    if (! opts.violin)
      py = rect.getYMin();
    else
      py = rect.getYMid();
  }

  Point p1, p2;

  double x0, xn;

  if (opts.fitTail) {
    x0 = (opoints[0     ].x - xmin1)*vxs;
    xn = (opoints[no - 1].x - xmin1)*vxs;
  }
  else {
    x0 = (opoints[0     ].x - xmin )*vxs;
    xn = (opoints[no - 1].x - xmin )*vxs;
  }

  double y0 = (opoints[0     ].y - ymin1)*vys;
  double yn = (opoints[no - 1].y - ymin1)*vys;

  if (orientation != Qt::Horizontal) {
    if (bottomLeft) {
      p1 = Point(px - y0, py + x0);
      p2 = Point(px - yn, py + xn);
    }
    else {
      p1 = Point(px + y0, py + x0);
      p2 = Point(px + yn, py + xn);
    }
  }
  else {
    p1 = Point(px + x0, py + y0);
    p2 = Point(px + xn, py + yn);
  }

  poly.setPoint(0     , Point(p1.x, p1.y));
  poly.setPoint(no + 1, Point(p2.x, p2.y));

  int ip = 0;

  for (auto &p : opoints) {
    double x1;

    if (opts.fitTail)
      x1 = (p.x - xmin1)*vxs;
    else
      x1 = (p.x - xmin )*vxs;

    double y1 = (p.y - ymin1)*vys;

    if (! opts.violin) {
      Point p1;

      if (orientation != Qt::Horizontal) {
        if (bottomLeft)
          p1 = Point(px - y1, py + x1);
        else
          p1 = Point(px + y1, py + x1);
      }
      else {
        p1 = Point(px + x1, py + y1);
      }

      poly.setPoint(ip + 1, Point(p1.x, p1.y));
    }
    else {
      Point p1, p2;

      if (orientation != Qt::Horizontal) {
        p1 = Point(px - y1, py + x1);
        p2 = Point(px + y1, py + x1);
      }
      else {
        p1 = Point(px + x1, py - y1);
        p2 = Point(px + x1, py + y1);
      }

      poly.setPoint(ip + 1     , Point(p1.x, p1.y));
      poly.setPoint(np - ip - 1, Point(p2.x, p2.y));
    }

    ++ip;
  }
}

//----

void
CQChartsDensity::
drawBuckets(PaintDevice *device, const BBox &rect, const Qt::Orientation &orientation) const
{
  CQBucketer bucketer;

  bucketer.setType(CQBucketer::Type::REAL_AUTO);
  bucketer.setNumAuto(20);
  bucketer.setRMin(xmin());
  bucketer.setRMax(xmax());

  using BucketCount = std::map<int, int>;

  BucketCount bucketCount;

  for (const auto &x : xvals()) {
    int bucket = bucketer.bucket(x);

    bucketCount[bucket]++;
  }

  int maxCount = 0;

  for (const auto &p : bucketCount)
    maxCount = std::max(maxCount, p.second);

  if (bucketCount.empty() || maxCount <= 0)
    return;

  if (orientation == Qt::Horizontal) {
    double dx = rect.getWidth ()/bucketCount.size();
    double dy = rect.getHeight()/maxCount;

    double x = rect.getXMin();
    double y = rect.getYMin();

    for (const auto &p : bucketCount) {
      int count = p.second;

      if (count > 0) {
        BBox bbox(x, y, x + dx, y + count*dy);

        device->drawRect(bbox);
      }

      x += dx;
    }
  }
  else {
    double dx = rect.getWidth ()/maxCount;
    double dy = rect.getHeight()/bucketCount.size();

    double x = rect.getXMin();
    double y = rect.getYMin();

    for (const auto &p : bucketCount) {
      int count = p.second;

      if (count > 0) {
        BBox bbox(x, y, x + count*dx, y + dy);

        device->drawRect(bbox);
      }

      y += dy;
    }
  }
}

//----

void
CQChartsDensity::
drawCrossBar(PaintDevice *device, const BBox &rect, double mean,
             const Qt::Orientation &orientation, const CQChartsLength &cornerSize)
{
  double lpos, tpos, bl, br;

  if (orientation != Qt::Horizontal) {
    lpos = rect.getYMin();
    tpos = rect.getYMax();

    bl = rect.getXMin();
    br = rect.getXMax();
  }
  else {
    lpos = rect.getXMin();
    tpos = rect.getXMax();

    bl = rect.getYMin();
    br = rect.getYMax();
  }

  Point p1, p2, pm;

  if (orientation != Qt::Horizontal) {
    p1 = Point(bl, lpos);
    p2 = Point(br, tpos);
    pm = Point(bl, mean);
  }
  else {
    p1 = Point(lpos, bl);
    p2 = Point(tpos, br);
    pm = Point(mean, bl);
  }

  //---

  // draw box
  BBox pbbox(p1, p2);

  CQChartsDrawUtil::drawRoundedRect(device, pbbox, cornerSize);

  //---

  // draw mean line
  if (orientation != Qt::Horizontal)
    device->drawLine(Point(p1.x, pm.y), Point(p2.x, pm.y));
  else
    device->drawLine(Point(pm.x, p1.y), Point(pm.x, p2.y));
}

void
CQChartsDensity::
drawPointRange(PaintDevice *device, const BBox &rect, double mean,
               const Qt::Orientation &orientation, const CQChartsSymbolData &symbol)
{
  // draw line
  drawLineRange(device, rect, orientation);

  // draw symbol at mean
  Point pm;

  if (orientation != Qt::Horizontal)
    pm = Point(rect.getXMid(), mean);
  else
    pm = Point(mean, rect.getYMid());

  CQChartsDrawUtil::drawSymbol(device, symbol.type(), Point(pm.x, pm.y), symbol.size());
}

void
CQChartsDensity::
drawErrorBar(PaintDevice *device, const BBox &rect, double mean,
             const Qt::Orientation &orientation, const CQChartsSymbolData &symbol)
{
  double lpos, tpos, bl, bm, br;

  if (orientation != Qt::Horizontal) {
    lpos = rect.getYMin();
    tpos = rect.getYMax();

    bl = rect.getXMin();
    bm = rect.getXMid();
    br = rect.getXMax();
  }
  else {
    lpos = rect.getXMin();
    tpos = rect.getXMax();

    bl = rect.getYMin();
    bm = rect.getYMid();
    br = rect.getYMax();
  }

  Point p1, p2, p3;

  if (orientation != Qt::Horizontal) {
    p1 = Point(bl, lpos);
    p2 = Point(bm, lpos);
    p3 = Point(br, tpos);
  }
  else {
    p1 = Point(lpos, bl);
    p2 = Point(lpos, bm);
    p3 = Point(tpos, br);
  }

  //---

  // draw error bar
  if (orientation != Qt::Horizontal) {
    device->drawLine(Point(p1.x, p1.y), Point(p3.x, p1.y)); // htop
    device->drawLine(Point(p1.x, p3.y), Point(p3.x, p3.y)); // hbottom
    device->drawLine(Point(p2.x, p1.y), Point(p2.x, p3.y)); // vline
  }
  else {
    device->drawLine(Point(p1.x, p1.y), Point(p1.x, p3.y)); // vleft
    device->drawLine(Point(p3.x, p1.y), Point(p3.x, p3.y)); // vright
    device->drawLine(Point(p1.x, p2.y), Point(p3.x, p2.y)); // hline
  }

  //---

  // draw symbol at mean
  Point pm;

  if (orientation != Qt::Horizontal)
    pm = Point(rect.getXMid(), mean);
  else
    pm = Point(mean, rect.getYMid());

  CQChartsDrawUtil::drawSymbol(device, symbol.type(), Point(pm.x, pm.y), symbol.size());
}

void
CQChartsDensity::
drawLineRange(PaintDevice *device, const BBox &rect, const Qt::Orientation &orientation)
{
  double lpos, tpos, bm;

  if (orientation != Qt::Horizontal) {
    lpos = rect.getYMin();
    tpos = rect.getYMax();

    bm = rect.getXMid();
  }
  else {
    lpos = rect.getXMin();
    tpos = rect.getXMax();

    bm = rect.getYMid();
  }

  Point p1, p2;

  if (orientation != Qt::Horizontal) {
    p1 = Point(bm, lpos);
    p2 = Point(bm, tpos);
  }
  else {
    p1 = Point(lpos, bm);
    p2 = Point(tpos, bm);
  }

  //---

  // draw error line
  if (orientation != Qt::Horizontal)
    device->drawLine(Point(p1.x, p1.y), Point(p1.x, p2.y)); // vline
  else
    device->drawLine(Point(p1.x, p1.y), Point(p2.x, p1.y)); // hline
}
