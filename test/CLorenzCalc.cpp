#include <CLorenzCalc.h>
#include <CRungeKutta.h>

double CLorenzCalc::A { 10.0 };
double CLorenzCalc::B { 28.0 };
double CLorenzCalc::C { 8.0/3.0 };

double CLorenzCalc::xmin_ = -17.99;
double CLorenzCalc::ymin_ = -24.15;
double CLorenzCalc::zmin_ =   0.00;
double CLorenzCalc::xmax_ =  19.83;
double CLorenzCalc::ymax_ =  27.64;
double CLorenzCalc::zmax_ =  48.31;

/*
 * dx / dt = a (y - x)
 * dy / dt = x (b - z) - y
 * dz / dt = xy - c z
 *
 * a = 10, b = 28   , c = 8 / 3
 * a = 28, b = 46.92, c = 4
 */

class FnX {
 public:
  double operator()(double, double x, double y, double) {
    return CLorenzCalc::A*(y - x);
  }
};

class FnY {
 public:
  double operator()(double, double x, double y, double z) {
    return (x*(CLorenzCalc::B - z) - y);
  }
};

class FnZ {
 public:
  double operator()(double, double x, double y, double z) {
    return (x*y - CLorenzCalc::C*z);
  }
};

using RungeKutta = CRungeKuttaXYZ<FnX, FnY, FnZ>;

CLorenzCalc::
CLorenzCalc()
{
  init();
}

CLorenzCalc::
CLorenzCalc(int start, int end) :
 iteration_start_(start), iteration_end_(end)
{
  init();
}

void
CLorenzCalc::
init()
{
  double ix = 0.0;
  double iy = 1.0;
  double iz = 0.0;

  double dt = 0.01;

  runge_kutta_ = new RungeKutta(0, ix, iy, iz, dt);
}

CLorenzCalc::
~CLorenzCalc()
{
  delete static_cast<RungeKutta *>(runge_kutta_);
}

void
CLorenzCalc::
draw()
{
  for (int i = 0; i < iteration_end_; i++) {
    double x, y, z;

    nextValue(x, y, z);

    if (i >= iteration_start_)
      drawPoint(x, y, z);
  }
}

void
CLorenzCalc::
nextValue(double &x, double &y, double &z)
{
  auto *runge_kutta = static_cast<RungeKutta *>(runge_kutta_);

  runge_kutta->step();

  x = runge_kutta->getX();
  y = runge_kutta->getY();
  z = runge_kutta->getZ();
}
