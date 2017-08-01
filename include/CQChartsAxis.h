#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <CQChartsAxisBase.h>

#include <sys/types.h>

#include <string>
#include <vector>

class CQChartsPlot;
class QPainter;

class CQChartsAxis : public CQChartsAxisBase {
  Q_OBJECT

 public:
  CQChartsAxis(CQChartsPlot *plot, Direction direction=DIR_HORIZONTAL,
               double start=0.0, double end=1.0);

 ~CQChartsAxis() { }

  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

  uint getNumMajorTicks() const { return numTicks1_; }
  void setNumMajorTicks(uint n) { numTicks1_ = n; }

  uint getNumMinorTicks() const { return numTicks2_; }
  void setNumMinorTicks(uint n) { numTicks2_ = n; }

  double getMajorIncrement() const;
  void setMajorIncrement(double i);

  double getMinorIncrement() const;

  uint getTickIncrement() const { return tickIncrement_; }
  void setTickIncrement(uint tickIncrement);

  const double *getTickSpaces   () const { return &tickSpaces_[0]; }
  uint          getNumTickSpaces() const { return tickSpaces_.size(); }

  double getTickSpace(int i) const { return tickSpaces_[i]; }
  void setTickSpaces(double *tickSpaces, uint numTickSpaces);

  QString getValueStr(double pos) const;

  bool getMinorTicksDisplayed() const { return minorTicksDisplayed_; }
  void setMinorTicksDisplayed(bool b) { minorTicksDisplayed_ = b; }

  void draw(CQChartsPlot *plot, QPainter *p);

 private:
  void calc();

  bool testAxisGaps(double start, double end, double testIncrement,
                    uint testNumGapTicks, double *start1, double *end1,
                    double *increment, uint *numGaps, uint *numGapTicks);

 private:
  typedef std::vector<double> TickSpaces;

  double     start1_              { 0 };
  double     end1_                { 1 };
  uint       numTicks1_           { 1 };
  uint       numTicks2_           { 0 };
  uint       tickIncrement_       { 0 };
  double     majorIncrement_      { 0 };
  TickSpaces tickSpaces_;
  bool       minorTicksDisplayed_ { true };
  int        column_              { -1 };
};

#endif
