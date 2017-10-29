#include <COSNaN.h>

#define USE_FP_CLASSIFY 1

#ifdef USE_FP_CLASSIFY
#include <cmath>
#else
#include <std_os.h>
#include <NaN.h>
#endif

bool
COSNaN::
has_nan()
{
#ifdef USE_FP_CLASSIFY
  return true;
#endif

#ifdef OS_UNIX
  return true;
#endif

  return false;
}

bool
COSNaN::
is_nan(double real)
{
#ifdef USE_FP_CLASSIFY
  int fpc = std::fpclassify(real);

  return (fpc == FP_NAN);
#endif

#ifdef OS_UNIX
  if (IsNaN(real))
    return true;
#endif

  return false;
}

bool
COSNaN::
set_nan(double *real)
{
#ifdef USE_FP_CLASSIFY
  *real = NAN;
  return true;
#endif

#ifdef OS_UNIX
  SetNaN(*real);
  return true;
#endif

  return false;
}

bool
COSNaN::
set_nan(double &real)
{
#ifdef USE_FP_CLASSIFY
  real = NAN;
  return true;
#endif

#ifdef OS_UNIX
  SetNaN(real);
  return true;
#endif

  return false;
}

double
COSNaN::
get_nan()
{
  double real;

  set_nan(real);

  return real;
}

//------

bool
COSNaN::
is_inf(double real)
{
#ifdef USE_FP_CLASSIFY
  int fpc = std::fpclassify(real);

  return (fpc == FP_INFINITE);
#endif

#ifdef OS_UNIX
  return IsInf(real);
#endif

  return false;
}

bool
COSNaN::
is_pos_inf(double real)
{
#ifdef USE_FP_CLASSIFY
  int fpc = std::fpclassify(real);

  return (fpc == FP_INFINITE && real > 0);
#endif

#ifdef OS_UNIX
  return (IsPosInf(real))
#endif

  return false;
}

bool
COSNaN::
is_neg_inf(double real)
{
#ifdef USE_FP_CLASSIFY
  int fpc = std::fpclassify(real);

  return (fpc == FP_INFINITE && real < 0);
#endif

#ifdef OS_UNIX
  return (IsNegInf(real));
#endif

  return false;
}

bool
COSNaN::
set_pos_inf(double &real)
{
#ifdef USE_FP_CLASSIFY
  real = INFINITY;
  return true;
#endif

#ifdef OS_UNIX
  SetPosInf(real);
  return true;
#endif

  return false;
}

bool
COSNaN::
set_neg_inf(double &real)
{
#ifdef USE_FP_CLASSIFY
  real = -INFINITY;
  return true;
#endif

#ifdef OS_UNIX
  SetNegInf(real);
  return true;
#endif

  return false;
}
