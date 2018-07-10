#ifndef leastSquaresFit_H
#define leastSquaresFit_H

namespace CLeastSquaresFit {

void backSubstitution(double **u, double *w, double **v, int m, int n, double *b,
                      double x[], int *return_code);

void singleValueDecomposition(double **a, int m, int n, double w[], double **v, int *return_code);

void leastSquaresFit(double x_point[], double y_point[], int num_points,
                     double coeff[], int coeff_free[], int num_coeffs,
                     double *deviation, int *return_code);

void bestLeastSquaresFit(double x_point[], double y_point[], int num_points,
                         double coeff[], int coeff_free[], int *num_coeffs,
                         double *deviation, int *return_code);

void interpolatePoly(double x_point[], double y_point[], int num_points,
                     double coeff[], int num_coeffs, int *return_code);

}

#endif
