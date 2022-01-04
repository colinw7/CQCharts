#ifndef CLORENZ_CALC_H
#define CLORENZ_CALC_H

class CDragon3DCalc {
 public:
  CDragon3DCalc();
  CDragon3DCalc(int start, int end);

  virtual ~CDragon3DCalc();

  int getIterationStart() const { return iteration_start_; }
  void setIterationStart(int iteration_start) { iteration_start_ = iteration_start; }

  void setQ(double r) { qval_ = r; }

  int getIterationEnd() const { return iteration_end_; }
  void setIterationEnd(int iteration_end) { iteration_end_ = iteration_end; }

  bool nextValue(double &x, double &y, double &z, double &g);

  void draw();

  virtual void setGray(double /*g*/) { }

  virtual void drawPoint(double /*x*/, double /*y*/, double /*z*/) { }

  int numValues() const;

 private:
  void init();

 private:
  int iteration_start_ { 20 };
  int iteration_end_   { 5000 };

  double qval_ { 0.0 };

  int i_ { 0 };

  double x_ { 0.0 };
  double y_ { 0.0 };
  double z_ { 0.0 };

  double p_ { 0.0 };
  double q_ { 0.0 };

  double kd_ { 0.0 };
  double k_  { 0.0 };

  double kmin_ { 0.0 };
  double kmax_ { 0.0 };
};

#endif
