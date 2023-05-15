#include <CMandelbrot.h>
#include <cmath>

CMandelbrot::
CMandelbrot()
{
}

CMandelbrot::
CMandelbrot(const CMandelbrot &m) :
 CPointFractalCalc(m), distance_(m.distance_), d_(m.d_)
{
}

CMandelbrot *
CMandelbrot::
dup() const
{
  CMandelbrot *m = new CMandelbrot(*this);

  return m;
}

void
CMandelbrot::
initCalc(int /*pixel_xmin*/, int /*pixel_ymin*/, int /*pixel_xmax*/, int /*pixel_ymax*/,
         double xmin, double ymin, double xmax, double ymax, int max_iterations)
{
  save_x_.resize(size_t(max_iterations + 1));
  save_y_.resize(size_t(max_iterations + 1));

  d_ = std::max(xmax - xmin, ymax - ymin)/4;
}

int
CMandelbrot::
calc(double x, double y, int max_iterations) const
{
  save_x_.resize(size_t(max_iterations + 1));
  save_y_.resize(size_t(max_iterations + 1));

  if (! distance_)
    return calc_iterations(x, y, max_iterations);
  else
    return calc_distance(x, y, max_iterations);
}

int
CMandelbrot::
calc_iterations(double x, double y, int max_iterations) const
{
  int num_iterations = iterate(x, y, max_iterations);

  if (num_iterations >= max_iterations)
    return max_iterations;

  int color = 254*num_iterations/(max_iterations - 1) + 1;

  if (show_vector_) {
    double angle = atan2(zr_, zi_);

    if (angle >= M_PI)
      color = 256 - color;
  }

  return color;
}

int
CMandelbrot::
calc_distance(double x, double y, int max_iterations) const
{
  int num_iterations = iterate(x, y, max_iterations);

  if (num_iterations >= max_iterations)
    return max_iterations;

  double dist = distance(x, y, num_iterations);

  int color = std::min(int(254*fabs(dist)/d_), 254) + 1;

  return color;
}

int
CMandelbrot::
iterate(double x, double y, int max_iterations) const
{
  double zr2 = 0.0;
  double zi2 = 0.0;
  double zri = 0.0;

  int num_iterations = -1;

  while (zi2 + zr2 < 4.0 && num_iterations < max_iterations) {
    zr_ = zr2 - zi2 + x;
    zi_ = zri + zri + y;

    zr2 = zr_*zr_;
    zi2 = zi_*zi_;
    zri = zr_*zi_;

    ++num_iterations;

    save_x_[size_t(num_iterations)] = zr_;
    save_y_[size_t(num_iterations)] = zi_;
  }

  return num_iterations;
}

double
CMandelbrot::
distance(double, double, int iterations) const
{
  if (iterations == 0) return 0.0;

  double x1 = 0;
  double y1 = 0;

  for (int i = 0; i < iterations; ++i) {
    double x2 = 2*(save_x_[size_t(i)]*x1 - save_y_[size_t(i)]*y1) + 1;
    double y2 = 2*(save_y_[size_t(i)]*x1 + save_x_[size_t(i)]*y1);

    x1 = x2;
    y1 = y2;

    if (std::max(::fabs(x1), ::fabs(y1)) > 1e14)
      return 0.0;
  }

  double zr2 = zr_*zr_;
  double zi2 = zi_*zi_;

  double dist = ::log(zr2 + zi2) * sqrt((zr2 + zi2)/(x1*x1 + y1*y1));

  return dist;
}
