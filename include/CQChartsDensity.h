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

  void calc();

  double eval(double x);

 public:
  XVals  xvals_;
  Points opoints_;
  double smoothParameter_ { -1.0 };
  int    numSamples_      { 100 };
  bool   initialized_     { false };
  double avg_             { 0.0 };
  double sigma_           { 0.0 };
};

#endif
