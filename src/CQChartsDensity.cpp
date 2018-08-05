#include <CQChartsDensity.h>
#include <QPolygonF>
#include <cmath>
#include <cassert>

namespace {

double polygonArea2(const QPolygonF &poly) {
  int n = poly.length();

  double area = 0.0;

  int i1 = n - 1;
  int i2 = 0;

  for ( ; i2 < n; i1 = i2++)
    area += poly[i1].x()*poly[i2].y() - poly[i1].y()*poly[i2].x();

  return area;
}

double polygonArea(const QPolygonF &poly) {
  return fabs(0.5*polygonArea2(poly));
}

}

//---

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

  area_ = polygonArea(poly);

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

  // calc x range, avgr and sigma
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

  if (nx_ > 0)
    avg_ /= (double) nx_;

  sigma_ = sqrt(sigma_/double(nx_) - avg_*avg_); /* Standard Deviation */

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
