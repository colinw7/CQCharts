#include <CQChartsDensity.h>
#include <CQChartsPlot.h>
#include <CQChartsRoundedPolygon.h>
#include <CQUtil.h>
#include <QPainter>
#include <cassert>

void
CQChartsDensity::
calc()
{
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

    points[i] = QPointF(x, y);

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

    while (y > ymin_) {
      x -= step1;
      y  = eval(x);

      while (step1 > 1E-5 && y < ymin_) {
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

    while (y > ymin_) {
      x += step1;
      y  = eval(x);

      while (step1 > 1E-5 && y < ymin_) {
        step1 /= 2.0;

        x -= step1;
        y  = eval(x);
      }

      rpoints.emplace_back(x, y);
    }
  }

  int nr = rpoints.size();

  //---

  xmin1_ = (nl > 0 ? lpoints[nl - 1].x() : xmin_);
  xmax1_ = (nr > 0 ? rpoints[nr - 1].x() : xmax_);

  //---

  // normalize polygon
  double xl = xmax1_ - xmin1_;

  QPolygonF poly;

  int np = numSamples_ + nr + nl;

  poly.resize(np);

  int i = 0;

  for (auto &p : lpoints) {
    poly[nl - i - 1] = QPointF((p.x() - xmin1_)/xl, p.y() - ymin_);

    ++i;
  }

  for (auto &p : points) {
    poly[i] = QPointF((p.x() - xmin1_)/xl, p.y() - ymin_);

    ++i;
  }

  for (auto &p : rpoints) {
    poly[i] = QPointF((p.x() - xmin1_)/xl, p.y() - ymin_);

    ++i;
  }

  area_ = CQUtil::polygonArea(poly);

  opoints_.resize(np);

  for (int i = 0; i < np; ++i) {
    opoints_[i] = QPointF(poly[i].x()*xl + xmin1_, poly[i].y()/area_);
  }

  ymin1_ = 0.0;
  ymax1_ = (ymax_ - ymin_)/area_;
}

//---

void
CQChartsDensity::
init()
{
  if (initialized_)
    return;

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

  initialized_ = true;
}

//---

double
CQChartsDensity::
yval(double x) const
{
  double y = eval(x);

  return (y - ymin_)/area_;
}

//---

double
CQChartsDensity::
eval(double x) const
{
  assert(initialized_);

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
drawWhisker(const CQChartsPlot *plot, QPainter *painter, const CQChartsGeom::BBox &rect,
            const Qt::Orientation &orientation, const CQChartsWhiskerOpts &opts) const
{
  QPolygonF ppoly;

  calcWhiskerPoly(ppoly, plot, rect, orientation, opts);

  painter->drawPolygon(ppoly);
}

//---

void
CQChartsDensity::
calcWhiskerPoly(QPolygonF &ppoly, const CQChartsPlot *plot, const CQChartsGeom::BBox &rect,
                const Qt::Orientation &orientation, const CQChartsWhiskerOpts &opts) const
{
  const CQChartsGeom::Range &dataRange = plot->dataRange();

  const CQChartsDensity::Points &opoints = this->opoints();

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

  ppoly.resize(np);

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

  double px1, py1, px2, py2;

  if (orientation != Qt::Horizontal) {
    plot->windowToPixel(px, xmin1, px1, py1);
    plot->windowToPixel(px, xmax1, px2, py2);
  }
  else {
    plot->windowToPixel(xmin1, py, px1, py1);
    plot->windowToPixel(xmax1, py, px2, py2);
  }

  ppoly[0     ] = QPointF(px1, py1);
  ppoly[no + 1] = QPointF(px2, py2);

  int ip = 0;

  for (auto &p : opoints) {
    double x1;

    if (opts.fitTail)
      x1 = (p.x() - xmin1)*vxs;
    else
      x1 = (p.x() - xmin )*vxs;

    double y1 = (p.y() - ymin1)*vys;

    if (! opts.violin) {
      double px1, py1;

      if (orientation != Qt::Horizontal) {
        if (bottomLeft)
          plot->windowToPixel(px - y1, py + x1, px1, py1);
        else
          plot->windowToPixel(px + y1, py + x1, px1, py1);
      }
      else {
        plot->windowToPixel(px + x1, py + y1, px1, py1);
      }

      ppoly[ip + 1] = QPointF(px1, py1);
    }
    else {
      double px1, py1, px2, py2;

      if (orientation != Qt::Horizontal) {
        plot->windowToPixel(px - y1, py + x1, px1, py1);
        plot->windowToPixel(px + y1, py + x1, px2, py2);
      }
      else {
        plot->windowToPixel(px + x1, py - y1, px1, py1);
        plot->windowToPixel(px + x1, py + y1, px2, py2);
      }

      ppoly[ip + 1     ] = QPointF(px1, py1);
      ppoly[np - ip - 1] = QPointF(px2, py2);
    }

    ++ip;
  }
}

//----

void
CQChartsDensity::
drawCrossBar(const CQChartsPlot *plot, QPainter *painter, const CQChartsGeom::BBox &rect,
             double mean, const Qt::Orientation &orientation, const CQChartsLength &cornerSize)
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

  double px1, py1, px2, py2, px3, py3;

  if (orientation != Qt::Horizontal) {
    plot->windowToPixel(bl, lpos, px1, py1);
    plot->windowToPixel(bl, mean, px2, py2);
    plot->windowToPixel(br, tpos, px3, py3);
  }
  else {
    plot->windowToPixel(lpos, bl, px1, py1);
    plot->windowToPixel(mean, bl, px2, py2);
    plot->windowToPixel(tpos, br, px3, py3);
  }

  //---

  // draw box
  QRectF prect(px1, py1, px3 - px1, py3 - py1);

  double cxs = plot->lengthPixelWidth (cornerSize);
  double cys = plot->lengthPixelHeight(cornerSize);

  CQChartsRoundedPolygon::draw(painter, prect, cxs, cys);

  //---

  // draw mean line
  if (orientation != Qt::Horizontal)
    painter->drawLine(QPointF(px1, py2), QPointF(px3, py2));
  else
    painter->drawLine(QPointF(px2, py1), QPointF(px2, py3));
}

void
CQChartsDensity::
drawPointRange(const CQChartsPlot *plot, QPainter *painter, const CQChartsGeom::BBox &rect,
               double mean, const Qt::Orientation &orientation, const CQChartsSymbolData &symbol,
               const QPen &pen, const QBrush &brush)
{
  // painter already pen set for drawLine

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

  double px1, py1, px2, py2, px3, py3;

  if (orientation != Qt::Horizontal) {
    plot->windowToPixel(bm, lpos, px1, py1);
    plot->windowToPixel(bm, mean, px2, py2);
    plot->windowToPixel(bm, tpos, px3, py3);
  }
  else {
    plot->windowToPixel(lpos, bm, px1, py1);
    plot->windowToPixel(mean, bm, px2, py2);
    plot->windowToPixel(tpos, bm, px3, py3);
  }

  //---

  // draw mid line
  if (orientation != Qt::Horizontal)
    painter->drawLine(QPointF(px1, py1), QPointF(px1, py3));
  else
    painter->drawLine(QPointF(px1, py1), QPointF(px3, py1));

  //---

  // draw symbol
  double sx, sy;

  plot->pixelSymbolSize(symbol.size(), sx, sy);

  plot->drawSymbol(painter, QPointF(px2, py2), symbol.type(), CMathUtil::avg(sx, sy), pen, brush);
}

void
CQChartsDensity::
drawErrorBar(const CQChartsPlot *plot, QPainter *painter, const CQChartsGeom::BBox &rect,
             const Qt::Orientation &orientation)
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

  double px1, py1, px2, py2, px3, py3;

  if (orientation != Qt::Horizontal) {
    plot->windowToPixel(bl, lpos, px1, py1);
    plot->windowToPixel(bm, lpos, px2, py2);
    plot->windowToPixel(br, tpos, px3, py3);
  }
  else {
    plot->windowToPixel(lpos, bl, px1, py1);
    plot->windowToPixel(lpos, bm, px2, py2);
    plot->windowToPixel(tpos, br, px3, py3);
  }

  //---

  // draw error bar
  if (orientation != Qt::Horizontal) {
    painter->drawLine(QPointF(px1, py1), QPointF(px3, py1)); // htop
    painter->drawLine(QPointF(px1, py3), QPointF(px3, py3)); // hbottom
    painter->drawLine(QPointF(px2, py1), QPointF(px2, py3)); // vline
  }
  else {
    painter->drawLine(QPointF(px1, py1), QPointF(px1, py3)); // vleft
    painter->drawLine(QPointF(px3, py1), QPointF(px2, py3)); // vright
    painter->drawLine(QPointF(px1, py2), QPointF(px3, py2)); // hline
  }
}

void
CQChartsDensity::
drawLineRange(const CQChartsPlot *plot, QPainter *painter, const CQChartsGeom::BBox &rect,
              const Qt::Orientation &orientation)
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

  double px1, py1, px2, py2;

  if (orientation != Qt::Horizontal) {
    plot->windowToPixel(bm, lpos, px1, py1);
    plot->windowToPixel(bm, tpos, px2, py2);
  }
  else {
    plot->windowToPixel(lpos, bm, px1, py1);
    plot->windowToPixel(tpos, bm, px2, py2);
  }

  //---

  // draw error line
  if (orientation != Qt::Horizontal)
    painter->drawLine(QPointF(px1, py1), QPointF(px1, py2)); // vline
  else
    painter->drawLine(QPointF(px1, py1), QPointF(px2, py1)); // hline
}
