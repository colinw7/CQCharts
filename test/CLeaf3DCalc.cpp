#include <CLeaf3DCalc.h>
#include <CMathRand.h>

CLeaf3DCalc::
CLeaf3DCalc()
{
  init();
}

CLeaf3DCalc::
CLeaf3DCalc(int start, int end) :
 iteration_start_(start), iteration_end_(end)
{
  init();
}

void
CLeaf3DCalc::
init()
{
  x_ = 0.0;
  y_ = 0.0;
  z_ = 0.0;
}

CLeaf3DCalc::
~CLeaf3DCalc()
{
}

void
CLeaf3DCalc::
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
CLeaf3DCalc::
nextValue(double &x, double &y, double &z)
{
  using uint = unsigned int;

  static uint   nd   = 4;
  static double a[4] = { 0.00,  0.83,  0.22, -0.22};
  static double b[4] = { 0.00,  0.00, -0.23,  0.23};
  static double c[4] = { 0.00,  0.00,  0.00,  0.00};
  static double d[4] = { 0.06,  0.00,  0.24,  0.24};
  static double e[4] = { 0.18,  0.86,  0.22,  0.22};
  static double f[4] = { 0.00,  0.10,  0.00,  0.00};
  static double g[4] = { 0.00,  0.00,  0.00,  0.00};
  static double h[4] = { 0.00, -0.12,  0.00,  0.00};
  static double m[4] = { 0.00,  0.84,  0.32,  0.32};
  static double n[4] = { 0.00,  0.00,  0.00,  0.00};
  static double q[4] = { 0.00,  1.62,  0.82,  0.82};
  static double r[4] = { 0.00,  0.00,  0.00,  0.00};
  static uint   p[4] = {    1,    85,    92,   100};

  uint percent = CMathRand::randInRange(0, 99);

  double x1 { 0 }, y1 { 0 }, z1 { 0 };

  //CRGBA rgba(0.4, 1.0, 0.4);

  for (uint j1 = 0; j1 < nd; ++j1) {
    if (percent < p[j1]) {
      x1 = a[j1]*x_ + b[j1]*y_ + c[j1]*z_ + n[j1];
      y1 = d[j1]*x_ + e[j1]*y_ + f[j1]*z_ + q[j1];
      z1 = g[j1]*x_ + h[j1]*y_ + m[j1]*z_ + r[j1];
      break;
    }
  }

  x_ = x1;
  y_ = y1;
  z_ = z1;

  x = x_;
  y = y_;
  z = z_;
}
