#ifndef CLeaf3DCalc_H
#define CLeaf3DCalc_H

class CLeaf3DCalc {
 public:
  CLeaf3DCalc();
  CLeaf3DCalc(int start, int end);

  virtual ~CLeaf3DCalc();

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
  int iteration_start_ { 0 };
  int iteration_end_   { 8000 };

  double x_ { 0.0 };
  double y_ { 0.0 };
  double z_ { 0.0 };
};

#endif
