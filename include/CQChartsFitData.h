#ifndef CQChartsFitData_H
#define CQChartsFitData_H

#include <CLeastSquaresFit.h>

/*!
 * \brief least squares fit data
 * \ingroup Charts
 */
class CQChartsFitData {
 public:
  CQChartsFitData() { }

  //---

  bool isFitted() const { return fitted_; }

  void resetFitted() { fitted_ = false; }

  //---

  double xmin() const { return xmin_; }
  double xmax() const { return xmax_; }

  //---

  int numCoeffs() const { return num_coeffs_; }

  double coeff(int i) const { return coeffs_[i]; }

  //---

  double deviation() const { return deviation_; }

  //---

  double interp(double x) const {
    double y = 0.0;

    for (int i = num_coeffs_ - 1; i >= 0; --i)
      y = x*y + coeffs_[i];

    return y;
  }

  //---

  void calc(const std::vector<QPointF> &points, int order=-1) {
    int np = points.size();

    std::vector<double> x, y;

    if (np > 0) {
      xmin_ = points[0].x();
      xmax_ = xmin_;

      for (const auto &p : points) {
        x.push_back(p.x());
        y.push_back(p.y());

        xmin_ = std::min(xmin_, p.x());
        xmax_ = std::max(xmax_, p.x());
      }
    }
    else {
      xmin_ = 0.0;
      xmax_ = 0.0;
    }

    //---

    int return_code;

    if (order <= 0) {
      num_coeffs_ = 8;

      CLeastSquaresFit::bestLeastSquaresFit(&x[0], &y[0], np,
        coeffs_, coeffs_free_, &num_coeffs_, &deviation_, &return_code);
    }
    else {
      num_coeffs_ = std::min(std::max(order, 1), 8);

      CLeastSquaresFit::leastSquaresFit(&x[0], &y[0], np,
        coeffs_, coeffs_free_, num_coeffs_, &deviation_, &return_code);
    }

    fitted_ = true;
  }

  void calc(const QPolygonF &points, int order=-1) {
    int np = points.size();

    std::vector<double> x, y;

    if (np > 0) {
      xmin_ = points[0].x();
      xmax_ = xmin_;

      for (int i = 0; i < np; ++i) {
        const QPointF &p = points[i];

        x.push_back(p.x());
        y.push_back(p.y());

        xmin_ = std::min(xmin_, p.x());
        xmax_ = std::max(xmax_, p.x());
      }
    }
    else {
      xmin_ = 0.0;
      xmax_ = 0.0;
    }

    //---

    int return_code;

    if (order <= 0) {
      num_coeffs_ = 8;

      CLeastSquaresFit::bestLeastSquaresFit(&x[0], &y[0], np,
        coeffs_, coeffs_free_, &num_coeffs_, &deviation_, &return_code);
    }
    else {
      num_coeffs_ = std::min(std::max(order, 1), 8);

      CLeastSquaresFit::leastSquaresFit(&x[0], &y[0], np,
        coeffs_, coeffs_free_, num_coeffs_, &deviation_, &return_code);
    }

    fitted_ = true;
  }

 private:
  bool   fitted_         { false };
  double coeffs_     [8] { 0, 0, 0, 0, 0, 0, 0, 0 };
  int    coeffs_free_[8] { 1, 1, 1, 1, 1, 1, 1, 1 };
  int    num_coeffs_     { 3 };
  double deviation_      { 0.0 };
  double xmin_           { 0.0 };
  double xmax_           { 0.0 };
};

#endif
