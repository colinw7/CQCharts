#ifndef CMANDELBROT_H
#define CMANDELBROT_H

#include <CPointFractal.h>
#include <vector>

class CMandelbrot : public CPointFractalCalc {
 public:
  CMandelbrot();
  CMandelbrot(const CMandelbrot &m);

  CMandelbrot *dup() const override;

  double getXMin() const override { return -2.0; }
  double getYMin() const override { return -1.2; }
  double getXMax() const override { return  1.2; }
  double getYMax() const override { return  1.2; }

  void setDistance(bool distance) { distance_ = distance; }

  void initCalc(int pixel_xmin, int pixel_ymin, int pixel_xmax, int pixel_ymax,
                double xmin=-2.0, double ymin=-1.2, double xmax=1.2, double ymax=1.2,
                int max_iterations=1000) override;

  int calc(double x, double y, int max_iterations=1000) const override;

  int calc_iterations(double x, double y, int max_iterations) const;
  int calc_distance(double x, double y, int max_iterations) const;

  int iterate(double x, double y, int max_iterations) const;

  double distance(double x, double y, int iterations) const;

 private:
  bool   distance_ { false };
  double d_        { 0.0 };

  mutable double              zr_ { 0.0 }, zi_ { 0.0 };
  mutable std::vector<double> save_x_;
  mutable std::vector<double> save_y_;
};

#endif
