#ifndef CLORENZ_CALC_H
#define CLORENZ_CALC_H

class CLorenzCalc {
 public:
  static double getXMin() { return xmin_; }
  static double getYMin() { return ymin_; }
  static double getZMin() { return zmin_; }
  static double getXMax() { return xmax_; }
  static double getYMax() { return ymax_; }
  static double getZMax() { return zmax_; }

  CLorenzCalc();
  CLorenzCalc(int start, int end);

  virtual ~CLorenzCalc();

  int getIterationStart() { return iteration_start_; }
  void setIterationStart(int iteration_start) { iteration_start_ = iteration_start; }

  int getIterationEnd() { return iteration_end_; }
  void setIterationEnd(int iteration_end) { iteration_end_ = iteration_end; }

  void nextValue(double &x, double &y, double &z);

  void draw();

  virtual void drawPoint(double /*x*/, double /*y*/, double /*z*/) { }

 private:
  void init();

 public:
  static double A;
  static double B;
  static double C;

 private:
  static double xmin_, ymin_, zmin_;
  static double xmax_, ymax_, zmax_;

  int iteration_start_ { 0 };
  int iteration_end_   { 8000 };

  void *runge_kutta_ { nullptr };
};

#endif
