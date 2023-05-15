#ifndef CPOINT_FRACTAL_H
#define CPOINT_FRACTAL_H

class CPointFractalCalc {
 public:
  CPointFractalCalc() { }

  virtual ~CPointFractalCalc() { }

  virtual CPointFractalCalc *dup() const = 0;

  void setAngle(double angle) { a_ = angle; }
  double getAngle() const { return a_; }

  virtual double getXMin() const = 0;
  virtual double getYMin() const = 0;
  virtual double getXMax() const = 0;
  virtual double getYMax() const = 0;

  bool getShowVector() const { return show_vector_; }
  void setShowVector(bool show_vector) { show_vector_ = show_vector; }

  virtual void setRange(double, double, double, double) { }

  virtual void initCalc(int, int, int, int, double, double, double, double, int) { }

  virtual int calc(double x, double y, int max_iterations) const = 0;

 protected:
  double a_           { 0.0 };
  bool   show_vector_ { false };
};

#endif
