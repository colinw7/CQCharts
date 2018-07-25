#ifndef CQChartsDensity_H
#define CQChartsDensity_H

#include <QPointF>
#include <vector>

class CQChartsDensity {
 public:
  using XVals  = std::vector<double>;
  using Points = std::vector<QPointF>;

 public:
  CQChartsDensity() { }

  void setXVals(const XVals &xvals) {
    xvals_       = xvals;
    initialized_ = false;
  }

  const Points &opoints() const { return opoints_; }

  int numSamples() const { return numSamples_; }
  void setNumSamples(int i) { numSamples_ = i; }

  double xmin() const { return xmin_; }
  double xmax() const { return xmax_; }

  double ymin() const { return ymin_; }
  double ymax() const { return ymax_; }

  void calc();

 private:
  void init();

  double eval(double x);

 public:
  XVals  xvals_;
  Points opoints_;
  double smoothParameter_ { -1.0 };
  int    numSamples_      { 100 };
  bool   initialized_     { false };
  double xmin_            { 0.0 };
  double xmax_            { 0.0 };
  double ymin_            { 0.0 };
  double ymax_            { 0.0 };
  double avg_             { 0.0 };
  double sigma_           { 0.0 };
};

#endif
