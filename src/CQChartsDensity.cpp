#include <CQChartsDensity.h>
#include <cmath>

double
CQChartsDensity::
eval(double x)
{
  int nx = xvals_.size();

  if (! initialized_) {
    avg_   = 0.0;
    sigma_ = 0.0;

    for (int i = 0; i < nx; i++) {
      avg_   += xvals_[i];
      sigma_ += xvals_[i]*xvals_[i];
    }

    avg_ /= (double) nx;

    sigma_ = sqrt(sigma_/double(nx) - avg_*avg_); /* Standard Deviation */

    initialized_ = true;
  }

  //---

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

void
CQChartsDensity::
calc()
{
  int nx = xvals_.size();

  double xmin = 0, xmax = 0;

  for (int i = 0; i < nx; ++i) {
    if (i == 0) {
      xmin = xvals_[i];
      xmax = xvals_[i];
    }
    else {
      xmin = std::min(xmin, xvals_[i]);
      xmax = std::max(xmax, xvals_[i]);
    }
  }

  double step = (xmax - xmin)/(numSamples_ - 1);

  opoints_.resize(numSamples_);

  for (int i = 0; i < numSamples_; i++) {
    double x = xmin + i*step;
    double y = eval(x);

    opoints_[i] = QPointF(x, y);
  }
}
