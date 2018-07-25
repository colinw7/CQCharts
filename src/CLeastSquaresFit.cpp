#include <CLeastSquaresFit.h>
#include <COSNaN.h>
#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

namespace CLeastSquaresFit {

static double TOL    = 5.0e-12;
static int    MAXITS = 30;

template<typename T>
T SABS(T a, T b) {
  return (b >= 0 ? std::abs(a) : -std::abs(a));
}

template<typename T>
int SIGN(T a) {
  return (a > 0 ? 1 : (a < 0 ? -1 : 0));
}

template<typename T>
T SUM(T a, T b) {
  int sum_exp_a, sum_exp_b;

  double sum_val_a = frexp(double(a), &sum_exp_a);
  double sum_val_b = frexp(double(b), &sum_exp_b);

  return (sum_exp_a == sum_exp_b && std::abs(sum_val_a + sum_val_b) < TOL ? 0.0 : (a + b));
}

using LDbl = long double;

template<typename T>
T PYTHAG(T x, T y) {
  LDbl pythag_x = std::abs(x);
  LDbl pythag_y = std::abs(y);

  return (pythag_x > pythag_y ?
           (pythag_x*sqrt(double(1.0 + (pythag_y*pythag_y)/(pythag_x*pythag_x)))) :
           (pythag_y == pythag_x ?
             (pythag_x*sqrt(2.0)) :
             (pythag_y*sqrt(double(1.0 + (pythag_x*pythag_x)/(pythag_y*pythag_y))))));
}

/*
 **  backSubstitution  Matrix Back Substitution Solution
 **
 **  Back Substitution solution of A.X = B where A is defined in U, diag W and V returned
 **  from singleValueDecomposition.
 **
 **  ENTRY
 **    u : dimensioned u[0:m-1][0:n-1]
 **    w : dimensioned w[0:n-1]
 **    v : dimensioned v[0:n-1][0:n-1]
 **    m : dimension
 **    n : dimension
 **    b : dimensioned b[0:m-1]
 **
 **  RETURN
 **    x           : dimensioned x[0:n-1]
 **    return_code : 0 OK
 **                  1 Allocation error
 */
void
backSubstitution(double **u, double *w, double **v, int m, int n, double *b,
                 double x[], int *return_code)
{
  /****                                                            ****/
  /* create the work arrays needed to solve the fit                   */
  /****                                                            ****/
  std::vector<double> temp;
  temp.resize(n);

  /****                                                            ****/
  /* calculate u-transposed multiplied by b                           */
  /****                                                            ****/
  for (int j = 0; j < n; j++) {
    if (w[j] != 0.0) {
      LDbl s = 0.0;

      for (int i = 0; i < m; i++)
        s += LDbl(u[i][j]*b[i]);

      temp[j] = s/w[j];
    }
    else
      temp[j] = 0.0;
  }

  /****                                                            ****/
  /* multiply by v                                                    */
  /****                                                            ****/
  for (int j = 0; j < n; j++) {
    LDbl s = 0.0;

    for (int k = 0; k < n; k++)
      s += LDbl(v[j][k]*temp[k]);

    x[j] = s;
  }

  /****                                                            ****/
  /* clean up                                                         */
  /****                                                            ****/
  *return_code = 0;
}

/*
 **  singleValueDecomposition  Single Value Decomposition
 **
 **  Single Value Decomposition of matrix A into matrices A, W and V.
 **  The fitting routine is a polynomial of degree m-1.
 **
 **  ENTRY
 **    a           : dimensioned a[0:m-1][0:n-1]
 **    m           : dimension
 **    n           : dimension
 **
 **  RETURN
 **    a           : dimensioned a[0:m-1][0:n-1]
 **    w           : dimensioned w[0:n-1]
 **    v           : dimensioned v[0:n-1][0:n-1]
 **    return_code : 0 OK
 **                  1 Allocation error
 **                  2 Convergence error
 */
void
singleValueDecomposition(double **a, int m, int n, double w[], double **v, int *return_code)
{
  int l, nm;
  LDbl c, f, g, h, s, scale, x, y, z;

  /****                                                            ****/
  /* create the work arrays needed to solve the fit                   */
  /****                                                            ****/
  std::vector<double> rv1;
  rv1.resize(n);

  /****                                                            ****/
  /* Householder reduction to binormal form                           */
  /****                                                            ****/
  g = scale = 0.0;

  LDbl anorm = 0.0;

  for (int i = 0; i < n; i++) {
    l = i + 1;

    rv1[i] = scale*g;

    g = s = scale = 0.0;

    if (i < m) {
      for (int k = i; k < m; k++)
        scale += std::abs(a[k][i]);

      if (scale != 0.0) {
        for (int k = i; k < m; k++) {
          a[k][i] /= scale;

          s += (LDbl)a[k][i]*a[k][i];
        }

        f = a[i][i];

        g = -SABS(LDbl(sqrt(double(s))), f);

        h = f*g - s;

        a[i][i] = f - g;

        for (int j = l; j < n; j++) {
          s = 0.0;

          for (int k = i; k < m; k++)
            s += (LDbl)a[k][i]*a[k][j];

          f = s/h;

          for (int k = i; k < m; k++)
            a[k][j] += f*a[k][i];
        }

        for (int k = i; k < m; k++)
          a[k][i] *= scale;
      }
    }

    w[i] = scale*g;

    g = s = scale = 0.0;

    if (i < m && i != n - 1) {
      for (int k = l; k < n; k++)
        scale += std::abs(a[i][k]);

      if (scale != 0.0) {
        for (int k = l; k < n; k++) {
          a[i][k] /= scale;

          s += (LDbl)a[i][k]*a[i][k];
        }

        f = a[i][l];
        g = -SABS(LDbl(sqrt(double(s))), f);
        h = f*g - s;

        a[i][l] = f - g;

        for (int k = l; k < n; k++)
          rv1[k] = a[i][k]/h;

        for (int j = l; j < m; j++) {
          s = 0.0;

          for (int k = l; k < n; k++)
            s += (LDbl)a[j][k]*a[i][k];

          for (int k = l; k < n; k++)
            a[j][k] += s*rv1[k];
        }

        for (int k = l; k < n; k++)
          a[i][k] *= scale;
      }
    }

    anorm = std::max(anorm, LDbl(std::abs(w[i]) + std::abs(rv1[i])));
  }

  /****                                                            ****/
  /* Accumulation of RHS transformations                              */
  /****                                                            ****/
  for (int i = n - 1; i >= 0; i--) {
    if (i < n - 1) {
      if (g != 0.0) {
        for (int j = l; j < n; j++)
          v[j][i] = (LDbl)(a[i][j]/a[i][l])/g;

        for (int j = l; j < n; j++) {
          s = 0.0;

          for (int k = l; k < n; k++)
            s += (LDbl)a[i][k]*v[k][j];

          for (int k = l; k < n; k++)
            v[k][j] += s*v[k][i];
        }
      }

      for (int j = l; j < n; j++)
        v[i][j] = v[j][i] = 0.0;
    }

    v[i][i] = 1.0;
    g = rv1[i];
    l = i;
  }

  /****                                                            ****/
  /* Accumulation of LHS transformations                              */
  /****                                                            ****/
  for (int i = std::min(m, n) - 1; i >= 0; i--) {
    l = i+1;
    g = w[i];

    for (int j = l; j < n; j++)
      a[i][j] = 0.0;

    if (g != 0.0) {
      g = 1.0/g;

      for (int j = l; j < n; j++) {
        s = 0.0;

        for (int k = l; k < m; k++)
          s += (LDbl)a[k][i]*a[k][j];

        f = (s/a[i][i])*g;

        for (int k = i; k < m; k++)
          a[k][j] += f*a[k][i];
      }

      for (int j = i; j < m; j++)
        a[j][i] *= g;
    }
    else {
      for (int j = i; j < m; j++)
        a[j][i] = 0.0;
    }

    ++a[i][i];
  }

  /****                                                            ****/
  /* Diagonalization of the bidiagonal form                           */
  /****                                                            ****/
  for (int k = n - 1; k >= 0; k--) {
    for (int its = 1; its <= MAXITS; its++) {
      int flag = 1;

      for (l = k; l >= 0; l--) {
        nm = l - 1;

        if (double(std::abs(rv1[l]) + anorm) == double(anorm)) {
          flag = 0;
          break;
        }

        if (double(std::abs(w[nm]) + anorm) == double(anorm))
          break;
      }

      if (flag != 0) {
        c = 0.0;
        s = 1.0;
        for (int i = l; i <= k; i++) {
          f = s*rv1[i];

          rv1[i] = c*rv1[i];

          if (double(std::abs(f) + anorm) == double(anorm))
            break;

          g = w[i];
          h = PYTHAG(f, g);

          w[i] = h;

          c = g/h;
          s = -f/h;

          h = 1.0/h;

          for (int j = 0; j < m; j++) {
            y = a[j][nm];
            z = a[j][i];

            a[j][nm] = y*c + z*s;
            a[j][i ] = z*c - y*s;
          }
        }
      }

      z = w[k];

      if (l == k) { /* convergence */
        if (z < 0.0) {
          w[k] = -z;

          for (int j = 0; j < n; j++)
            v[j][k] = -v[j][k];
        }

        break;
      }

      if (its == MAXITS) {
        *return_code = 2;
        return;
      }

      /* Shift from bottom 2-by-2 minor                               */
      x = w[l];
      nm = k - 1;
      y = w[nm];

      g = rv1[nm];
      h = rv1[k];
      f = ((y - z)*(y + z) + (g - h)*(g + h))/(2.0*h*y);
      g = PYTHAG(f, LDbl(1.0));
      f = ((x - z)*(x + z) + h*((y/(f + SABS(g, f))) - h))/x;
      c = s = 1.0;

      for (int j = l; j <= nm; j++) {
        int i = j + 1;

        g = rv1[i];

        y = w[i];

        h = s*g;
        g = c*g;

        z = PYTHAG(f, h);

        rv1[j] = z;

        c = f/z;
        s = h/z;

        f = x*c + g*s;

        g = g*c - x*s;
        h = y*s;

        y *= c;

        for (int jj = 0; jj < n; jj++) {
          x = v[jj][j];
          z = v[jj][i];

          v[jj][j] = x*c + z*s;
          v[jj][i] = z*c - x*s;
        }

        z = PYTHAG(f, h);
        w[j] = z;

        if (z != 0.0) {
          c = f/z;
          s = h/z;

          //z = 1.0/z;
        }

        f = c*g + s*y;
        x = c*y - s*g;

        for (int jj = 0; jj < m; jj++) {
          y = a[jj][j];
          z = a[jj][i];

          a[jj][j] = y*c + z*s;
          a[jj][i] = z*c - y*s;
        }
      }

      rv1[l] = 0.0;
      rv1[k] = f;
      w  [k] = x;
    }
  }

  /****                                                            ****/
  /* clean up                                                         */
  /****                                                            ****/
  *return_code = 0;
}

/*
 **  leastSquaresFit  Least Squares Fit of a polynomial
 **
 **  Least Squares Fit of polynomial in powers of x_point to any fixed degree.
 **  A singular value decomposition is used to remove failures due to close or
 **  equal points or fewer points than coefficients.
 **  Some coefficients can be held fixed whilst the others are free to be solved
 **  using the condition of minimising the Sum{i:y_point[i]-Y(x_point[i])}.
 **
 **   ENTRY
 **     x_point     : dimensioned x_point[0:num_points-1]
 **     y_point     : dimensioned y_point[0:num_points-1]
 **     num_points  : dimension
 **     coeff       : dimensioned coeff[0:num_coeffs-1]
 **     coeff_free  : dimensioned coeff_free[0:num_coeffs-1]
 **                    coeff_free[i] = 0 => coeff[i] input and fixed
 **                    else coeff[i] output and free to be calculated
 **                    The count of all free coefficients is called num_free below
 **     num_coeffs  : dimension
 **
 **   RETURN
 **     coeff       : dimensioned coeff[0:num_coeffs-1]
 **                    Y(x_point[i]) = Sum{j:coeff[j]*x_point[i]**i}
 **                    .                0<=j<num_coeffs
 **     deviation   : standard y deviation of num_points
 **                   with num_free degrees of freedom
 **                    < 0 no fit
 **                    = 0 num_free >= num_points
 **                    > 0 sqrt(Sum{i:y_point[i]-Y(x_point[i])}/(num_points-num_free))
 **                    .         0<=i<num_points
 **     return_code : 0 OK
 **                   1 num_points <= 0 or x_point/y_point contains Not-a-Number
 **                   2 num_coeffs <= 0
 **                   3 Non-free (input) coeff contains Not-a-Number
 **                   4 Allocation error
 **                   5 singleValueDecomposition Allocation or Convergence error
 **                   6 backSubstitution Allocation error
 **                   7 free (output) coeff contains Not-a-Number
 **
 **  NOTES
 **   . See interpolatePoly to solve y_point for any x_points.
 **   . The input points need NOT be monotonic in x.
 **   . 1 solution is returned EVEN when the points do not have a unique solution
 **     (e.g. a square of 4 points).
 **   . Lower power polynomials may make a "better" fit (See bestLeastSquaresFit).
 **   . Sum{i:y_point[i]-Y(x_point[i])} may not be 0 even when the returned deviation = 0.
 **   . When the solution is not unique (e.g. num_coeffs > number of unique x points) the
 **     polynomial is still of the specified degree.
 **   . When the solution is not unique fixing a coefficient may produce another solution
 **     even if fixed to the same value.
 */
void leastSquaresFit(double x_point[], double y_point[], int num_points,
                     double coeff[], int coeff_free[], int num_coeffs,
                     double *deviation, int *return_code)
{
  *deviation = -1.0;

  /****                                                            ****/
  /* Check point values                                               */
  /****                                                            ****/
  if (num_points <= 0) { /* no stddev possible */
    *return_code = 1;
    return;
  }

  for (int sub_point = 0; sub_point < num_points; sub_point++) {
    if (COSNaN::is_nan(x_point[sub_point]) || COSNaN::is_nan(y_point[sub_point])) {
      *return_code = 1;
      return;
    }
  }

  /****                                                            ****/
  /* Check coefficient values                                         */
  /****                                                            ****/
  if (num_coeffs <= 0) { /* no fit possible */
    *return_code = 2;
    return;
  }

  int num_free = 0;

  for (int sub_coeff = 0; sub_coeff < num_coeffs; sub_coeff++) {
    if (coeff_free[sub_coeff]) {
      coeff[sub_coeff] = 0.0;
      num_free++;
    }
    else if (COSNaN::is_nan(coeff[sub_coeff])) {
      *return_code = 3;
      return;
    }
  }

  LDbl y_diff = 0.0;

  int    sub_free = 0;
  double thresh   = 0.0;
  double max_w    = 0.0;

  double **u = 0;
  double **v = 0;

  std::vector<double> b;
  std::vector<double> w;
  std::vector<double> coeff_temp;

  if (num_free <= 0) {
    *return_code = 0;
    goto stddev;
  }

  /****                                                            ****/
  /* create the work arrays needed to solve the fit                   */
  /****                                                            ****/
  b         .resize(num_points);
  w         .resize(num_free);
  coeff_temp.resize(num_free);

  /****                                                            ****/
  /* create the work matrices needed to solve the fit                 */
  /****                                                            ****/
  u = new double * [num_points];
  if (! u) {
    *return_code = 4;
    return;
  }

  u[0] = new double [num_points*num_free];
  if (! u[0]) {
    delete [] u;
    *return_code = 4;
    return;
  }

  for (int sub_point = 1; sub_point < num_points; sub_point++)
    u[sub_point] = u[sub_point - 1] + num_free;

  v = new double * [num_free];
  if (! v) {
    delete [] u[0];
    delete [] u;
    *return_code = 4;
    return;
  }

  v[0] = new double [num_free*num_free];
  if (! v[0]) {
    delete [] v;
    delete [] u[0];
    delete [] u;
    *return_code = 4;
    return;
  }

  for (int sub_coeff = 1; sub_coeff < num_free; sub_coeff++)
    v[sub_coeff] = v[sub_coeff - 1] + num_free;

  /****                                                            ****/
  /* Accumulate coefficients of the fitting matrix                    */
  /****                                                            ****/
  for (int sub_point = 0; sub_point < num_points; sub_point++) {
    int sub_coeff = sub_free = 0;

    LDbl x_power_coeff = 1.0;

    b[sub_point] = y_point[sub_point];

    if (coeff_free[sub_coeff])
      u[sub_point][sub_free++] = x_power_coeff;
    else
      b[sub_point] -= coeff[sub_coeff]*x_power_coeff;

    for (sub_coeff = 1; sub_coeff < num_coeffs; sub_coeff++) {
      x_power_coeff *= x_point[sub_point];

      if (coeff_free[sub_coeff] && sub_free < num_free)
        u[sub_point][sub_free++] = x_power_coeff;
      else
        b[sub_point] -= coeff[sub_coeff]*x_power_coeff;
    }
  }

  /****                                                            ****/
  /* Perform Singular Value Decomposition                             */
  /****                                                            ****/
  singleValueDecomposition(u, num_points, num_free, &w[0], v, return_code);

  if (*return_code != 0) {
    delete [] v[0];
    delete [] v;
    delete [] u[0];
    delete [] u;
    *return_code = 5;
    return;
  }

  /****                                                            ****/
  /* Edit the singular values to tolerance                            */
  /****                                                            ****/
  max_w = 0.0;

  for (int sub_coeff = 0; sub_coeff < num_free; sub_coeff++) {
    if (w[sub_coeff] > max_w)
      max_w = w[sub_coeff];
  }

  thresh = TOL*max_w;
  for (int sub_coeff = 0; sub_coeff < num_free; sub_coeff++) {
    if (w[sub_coeff] < thresh)
      w[sub_coeff] = 0.0;
  }

  /****                                                            ****/
  /* Solve by back substitution                                       */
  /****                                                            ****/
  backSubstitution(u, &w[0], v, num_points, num_free, &b[0], &coeff_temp[0], return_code);
  if (*return_code != 0) {
    delete [] v[0];
    delete [] v;
    delete [] u[0];
    delete [] u;
    *return_code = 6;
    return;
  }

  /****                                                            ****/
  /* Add the calculated free coefficients to the fixed ones           */
  /****                                                            ****/
  *return_code = 0;

  sub_free = 0;

  for (int sub_coeff = 0; sub_coeff < num_coeffs; sub_coeff++) {
    if (coeff_free[sub_coeff]) {
      coeff[sub_coeff] = coeff_temp[sub_free++];

      if (COSNaN::is_nan(coeff[sub_coeff]))
        *return_code = 7;

      if (sub_free == num_free)
        break;
    }
  }

  /****                                                            ****/
  /* clean up                                                         */
  /****                                                            ****/
  delete [] v[0];
  delete [] v;
  delete [] u[0];
  delete [] u;

  /****                                                            ****/
  /* Evaluate standard deviation                                      */
  /****                                                            ****/
 stddev:
  LDbl sum_y_diff_sq = 0.0;

  for (int sub_point = 0; sub_point < num_points; sub_point++) {
    y_diff = y_point[sub_point] - coeff[0];

    LDbl x_power_coeff = 1.0;

    for (int sub_coeff = 1; sub_coeff < num_coeffs; sub_coeff++) {
      x_power_coeff *= x_point[sub_point];

      y_diff -= coeff[sub_coeff]*x_power_coeff;
    }

    sum_y_diff_sq += y_diff*y_diff;
  }

  if (num_points > num_free)
    *deviation = sqrt(double(sum_y_diff_sq/(num_points - num_free)));
  else
    *deviation = 0.0;
}

/*
 **  bestLeastSquaresFit  Best Least Squares Polynomial Fit
 **
 **  The routine call leastSquaresFit with num_free (the count of all free coefficients)
 **  varying between appropriate values <= *num_coeffs and returning the fit with the
 **  lowest deviation.
 **
 **   ENTRY
 **     x_point     : dimensioned x_point[0:num_points-1]
 **     y_point     : dimensioned y_point[0:num_points-1]
 **     num_points  : dimension
 **     coeff       : dimensioned coeff[0:*num_coeffs-1]
 **     coeff_free  : dimensioned coeff_free[0:*num_coeffs-1]
 **                    coeff_free[i] = 0 => coeff[i] input and fixed
 **                    else coeff[i] output and free to be calculated
 **     num_coeffs  : dimension
 **
 **   RETURN
 **     coeff       : dimensioned coeff[0:*num_coeffs-1]
 **                    Y(x_point[i]) = Sum{j:coeff[j]*x_point[i]**i}
 **                    .                0<=j<*num_coeffs
 **     num_coeffs  : num_free used.
 **     deviation   : standard y deviation of num_points
 **                   with num_free degrees of freedom
 **                    < 0 no fit
 **                    = 0 num_free == num_points
 **                    > 0 sqrt(Sum{i:y_point[i]-Y(x_point[i])}/(num_points-num_free))
 **                    .         0<=i<num_points
 **     return_code : 0 OK
 **                   See leastSquaresFit
 **
 **  NOTES
 **   . See interpolatePoly to solve y_point for any x_points.
 **   . This routine looks for minimum sqrt(D/(num_points-num_free))
 **     NOT minimum sqrt(D). D= Sum{i:y_point[i]-Y(x_point[i])}.
 **   . if num_coeffs >= num_points the routine will be faster if
 **     x_points could be made strictly monotonic.
 **   . If num_free >= the number of unique x_points a single fit
 **     is tried else fits from 0 to num_free are tried.
 **   . The fixed points may appear anywhere in the coeffs array.
 */
void bestLeastSquaresFit(double x_point[], double y_point[], int num_points,
                         double coeff[], int coeff_free[], int *num_coeffs,
                         double *deviation, int *return_code)
{
  /****                                                            ****/
  /* Check coefficient values                                         */
  /****                                                            ****/
  *deviation   = -1.0;
  *return_code = 2;

  if (*num_coeffs <= 0) { /* no fit possible */
    *return_code = 2;
    return;
  }

  std::vector<double> fit_coeff;
  fit_coeff.resize(*num_coeffs);

  std::vector<int> fit_free;
  fit_free.resize(*num_coeffs);

  int num_free = 0;

  for (int sub_coeff = 0; sub_coeff < *num_coeffs; sub_coeff++) {
    if (coeff_free[sub_coeff]) {
      coeff[sub_coeff] = 0.0;
      num_free++;
    }
    else if (COSNaN::is_nan(coeff[sub_coeff])) {
      *return_code = 3;
      return;
    }

    fit_coeff[sub_coeff] = coeff[sub_coeff];
    fit_free [sub_coeff] = coeff_free[sub_coeff];
  }

  /****                                                            ****/
  /* Check point values                                               */
  /****                                                            ****/
  if (num_points <= 0) { /* no stddev possible */
    *return_code = 1;
    return;
  }

  int contig;

  if (num_points == 1)
    contig = 1;
  else
    contig = SIGN(x_point[1]-x_point[0]);

  if (COSNaN::is_nan(x_point[0]) || COSNaN::is_nan(y_point[0])) {
    *return_code = 1;
    return;
  }

  double max_x = x_point[0];

  for (int sub_point = 1; sub_point < num_points; sub_point++) {
    if (COSNaN::is_nan(x_point[sub_point]) || COSNaN::is_nan(y_point[sub_point])) {
      *return_code = 1;
      return;
    }

    if (SIGN(x_point[sub_point]-x_point[sub_point-1]) != contig)
      contig = 0;

    if (std::abs(x_point[sub_point]) > std::abs(max_x))
      max_x = x_point[sub_point];
  }

  /****                                                            ****/
  /* Find the number of unique x_point values                         */
  /****                                                            ****/
  int num_uniq;

  if (contig == 0) {
    num_uniq = num_points;
    for (int sub_point2 = 0; sub_point2 < num_points-1; sub_point2++) {
      for (int sub_point = sub_point2 + 1; sub_point < num_points; sub_point++) {
        if (x_point[sub_point2] == x_point[sub_point]) {
          num_uniq--;
          break;
        }
      }
    }
  }
  else {
    num_uniq = num_points;
  }

  int min_fit, max_fit;

  if (num_free >= num_uniq) {
    if (num_uniq == num_points)
      min_fit = max_fit = num_uniq;
    else
      min_fit = 0, max_fit = num_uniq;

    num_free = num_uniq;

    for (int sub_coeff = num_free; sub_coeff < *num_coeffs; sub_coeff++) {
      if (fit_free[sub_coeff]) {
        fit_free [sub_coeff] = 0;
        fit_coeff[sub_coeff] = 0.0;
      }
    }
  }
  else {
    min_fit = 0;
    max_fit = num_free;
  }

  /****                                                            ****/
  /* try all allowed fits                                             */
  /****                                                            ****/
  int best_fit = 0;

  for (int num_fit = min_fit; num_fit <= max_fit; num_fit++) {
    if (num_fit != min_fit) {
      for (int sub_coeff = *num_coeffs - 1; sub_coeff >= 0; sub_coeff--) {
        if (fit_free[sub_coeff]) {
          fit_free [sub_coeff] = 0;
          fit_coeff[sub_coeff] = 0.0;
          num_free--;
          break;
        }
      }
    }

    double fit_deviation;

    leastSquaresFit(x_point, y_point, num_points, &fit_coeff[0], &fit_free[0], *num_coeffs,
                    &fit_deviation, return_code);

    if (*return_code == 0) {
      if (*deviation < 0.0 || fit_deviation < *deviation) {
        *deviation = fit_deviation;

        best_fit = num_free;

        for (int sub_coeff = 0; sub_coeff < *num_coeffs; sub_coeff++)
          coeff[sub_coeff] = fit_coeff[sub_coeff];

        if (fit_deviation == 0.0) /* stop now */
          break;
      }
    }
    else if (*deviation >= 0.0) /* ignore error */
      *return_code = 0;
  }

  /****                                                            ****/
  /* clean up                                                         */
  /****                                                            ****/
  if (*deviation >= 0.0) {
    /****                                                      ****/
    /* Remove unwanted negligable high power coefficients         */
    /****                                                      ****/
    double max_y = std::abs(coeff[0]);

    LDbl x_power_coeff = 1.0;

    for (int sub_coeff = 1; sub_coeff < *num_coeffs; sub_coeff++) {
      x_power_coeff *= max_x;

      max_y += coeff[sub_coeff]*x_power_coeff;
    }

    if (coeff_free[0] && std::abs(coeff[0]) < TOL)
      coeff[0] = 0.0;

    x_power_coeff = 1.0;

    for (int sub_coeff = 1; sub_coeff < *num_coeffs; sub_coeff++) {
      x_power_coeff *= max_x;

      if (coeff_free[sub_coeff] &&
          ! SUM(std::abs(coeff[sub_coeff]*x_power_coeff) - max_y, LDbl(max_y)))
        coeff[sub_coeff] = 0.0;
    }

    *num_coeffs = best_fit;
  }
}

 /*
 **  interpolatePoly  Interpolate Polynomial Y values
 **
 **  Interpolate y_point values given x_point values and
 **  coefficients of a polynomial in powers of x_point.
 **
 **  ENTRY
 **    x_point     : dimensioned x_point[0:num_points-1]
 **    num_points  : dimension
 **    coeff       : dimensioned coeff[0:num_coeffs-1]
 **    num_coeffs  : dimension
 **
 **  RETURN
 **    y_point     : dimensioned y_point[0:num_points-1]
 **    return_code : 0 OK
 **                  1 num_points <= 0
 **                  2 num_coeffs <= 0
 **
 **  LIMITATIONS
 **   No checking is done for Not-a-number input or output
 */
void interpolatePoly(double x_point[], double y_point[], int num_points,
                     double coeff[], int num_coeffs, int *return_code)
{
  if (num_points <= 0) {
    *return_code = 1;
    return;
  }

  if (num_coeffs <= 0) { /* no fit possible */
    for (int sub_point = 0; sub_point < num_points; sub_point++)
      y_point[sub_point] = 0.0;

    *return_code = 2;

    return;
  }

  for (int sub_point = 0; sub_point < num_points; sub_point++) {
    LDbl y = coeff[0];

    LDbl x_power_coeff = 1.0;

    for (int sub_coeff = 1; sub_coeff < num_coeffs; sub_coeff++) {
      x_power_coeff *= x_point[sub_point];

      y += coeff[sub_coeff]*x_power_coeff;
    }

    y_point[sub_point] = y;
  }

  *return_code = 0;

  return;
}

}
