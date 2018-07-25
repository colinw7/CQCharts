#include <CQChartsDensity.h>
#include <cmath>
#include <cassert>

void
CQChartsDensity::
calc()
{
  init();

  ymin_ = 0.0;
  ymax_ = 0.0;

  int nx = xvals_.size();

  if (nx < 2)
    return;

  double step = (xmax_ - xmin_)/(numSamples_ - 1);

  opoints_.resize(numSamples_);

  for (int i = 0; i < numSamples_; i++) {
    double x = xmin_ + i*step;
    double y = eval(x);

    opoints_[i] = QPointF(x, y);

    if (i == 0) {
      ymin_ = y;
      ymax_ = y;
    }
    else {
      ymin_ = std::min(ymin_, y);
      ymax_ = std::max(ymax_, y);
    }
  }
}

//---

void
CQChartsDensity::
init()
{
  if (initialized_)
    return;

  xmin_  = 0.0;
  xmax_  = 0.0;
  avg_   = 0.0;
  sigma_ = 0.0;
  ymin_  = 0.0;
  ymax_  = 0.0;

  int nx = xvals_.size();

  for (int i = 0; i < nx; i++) {
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

  if (nx > 0)
    avg_ /= (double) nx;

  sigma_ = sqrt(sigma_/double(nx) - avg_*avg_); /* Standard Deviation */

  initialized_ = true;
}

//---

double
CQChartsDensity::
eval(double x)
{
  assert(initialized_);

  int nx = xvals_.size();

  double bandwidth;

  if (smoothParameter_ <= 0) {
    /* This is the optimal bandwidth if the point distribution is Gaussian.
       (Applied Smoothing Techniques for Data Analysis
       by Adrian W, Bowman & Adelchi Azzalini (1997)) */
    /* If the supplied bandwidth is zero of less, the default bandwidth is used. */
    double defaultBandwidth = pow(4.0/(3.0*nx), 1.0/5.0)*sigma_;

    bandwidth = defaultBandwidth;
  }
  else {
    bandwidth = smoothParameter_;
  }

  //---

  double y = 0;

  for (int i = 0; i < nx; i++) {
    double z = (x - xvals_[i])/bandwidth;

    y += exp(-0.5*z*z)/bandwidth;
  }

  y /= sqrt(2.0*M_PI);

  return y;
}
