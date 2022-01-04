#include <CDragon3DCalc.h>
#include <CMathRand.h>
#include <cmath>

CDragon3DCalc::
CDragon3DCalc()
{
  init();
}

CDragon3DCalc::
CDragon3DCalc(int start, int end) :
 iteration_start_(start), iteration_end_(end)
{
  init();
}

void
CDragon3DCalc::
init()
{
  qval_ = 0.967;

  i_ = 0;

  kmin_ = -3.0;
  kmax_ =  3.0;
  kd_   =  0.1;

  k_ = kmin_;
}

CDragon3DCalc::
~CDragon3DCalc()
{
}

void
CDragon3DCalc::
draw()
{
  double x, y, z, g;

  while (nextValue(x, y, z, g)) {
    setGray(g);

    drawPoint(x, y, z);
  }
}

bool
CDragon3DCalc::
nextValue(double &x, double &y, double &z, double &g)
{
  if (i_ == 0) {
    g = (k_ - kmin_)/(kmax_ - kmin_);

    x_ = 0.500001;
    y_ = 0.0;

    double mag;

    if (qval_ == 0.0) {
      mag = 1.0;
      q_  = 4*std::sqrt(1 - k_*k_);
    }
    else {
      mag = k_*k_ + qval_*qval_;
      q_  = -4*qval_/mag;
    }

    p_ = 4*k_/mag;
  }

  int ni = (i_ < iteration_start_ ? iteration_start_ : 1);

  for (int ii = 0; ii < ni; ++ii) {
    double tx = x_*p_ - y_*q_;

    y_ = x_*q_ + y_*p_;

    double ty = y_;

    x_ = 1.0 - tx;

    double mag = std::sqrt(x_*x_ + y_*y_);

    y_ = std::sqrt((-x_ + mag)/2);
    x_ = std::sqrt(( x_ + mag)/2);

    if (ty < 0)
      x_ = -x_;

    int b = CMathRand::randInRange(0, 1);

    if (b) {
      x_ = -x_;
      y_ = -y_;
    }

    x_ = (1 - x_)/2;
    y_ = y_/2;
    z_ = p_/2;

    ++i_;
  }

  if (i_ >= iteration_end_) {
    i_ = 0;

    k_ += kd_;

    if (k_ > kmax_)
      return false;
  }

  x = x_;
  y = y_;
  z = z_;

  return true;
}

int
CDragon3DCalc::
numValues() const
{
  int nk = (kmax_ - kmin_)/kd_;

  return nk*(getIterationEnd() - getIterationStart() + 1);
}
