#include <CPickoverCalc.h>
#include <cmath>

double CPickoverCalc::xmin_ = -1.96;
double CPickoverCalc::ymin_ = -1.85;
double CPickoverCalc::zmin_ = -1.00;
double CPickoverCalc::xmax_ =  1.96;
double CPickoverCalc::ymax_ =  1.94;
double CPickoverCalc::zmax_ =  1.00;

CPickoverCalc::
CPickoverCalc()
{
  init();
}

CPickoverCalc::
CPickoverCalc(int start, int end) :
 iteration_start_(start), iteration_end_(end)
{
  init();
}

void
CPickoverCalc::
init()
{
  x_ = 0.0;
  y_ = 0.0;
  z_ = 0.0;
}

void
CPickoverCalc::
draw()
{
  for (int i = 0; i < iteration_end_; ++i) {
    double x, y, z;

    nextValue(x, y, z);

    if (i >= iteration_start_)
      drawPoint(x_, y_, z_);
  }
}

void
CPickoverCalc::
nextValue(double &x, double &y, double &z)
{
  static double a =  2.24;
  static double b =  0.43;
  static double c = -0.65;
  static double d = -2.43;
  static double e =  1.00;

  double xt =   sin(a*y_) - z*cos(b*y_);
  double yt = z*sin(c*x_) -   cos(d*y_);
  double zt = e*sin(x_);

  x_ = xt;
  y_ = yt;
  z_ = zt;

  x = x_;
  y = y_;
  z = z_;
}
