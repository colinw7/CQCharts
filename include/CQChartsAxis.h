#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <CQChartsAxisBase.h>
#include <COptVal.h>

#include <sys/types.h>

#include <string>
#include <vector>

class CQChartsPlot;
class QPainter;

// Basic Axis Data
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

  uint getTickIncrement() const { return tickIncrement_; }
  void setTickIncrement(uint tickIncrement);

  double getMajorIncrement() const;
  void setMajorIncrement(double i);

  const double *getTickSpaces   () const { return &tickSpaces_[0]; }
  uint          getNumTickSpaces() const { return tickSpaces_.size(); }

  bool getMinorTicksDisplayed() const { return minorTicksDisplayed_; }
  void setMinorTicksDisplayed(bool b) { minorTicksDisplayed_ = b; }

  bool isIntegral() const { return integral_; }
  void setIntegral(bool b);

  bool isDataLabels() const { return dataLabels_; }
  void setDataLabels(bool b) { dataLabels_ = b; }

  double getMinorIncrement() const;

  double getTickSpace(int i) const { return tickSpaces_[i]; }
  void setTickSpaces(double *tickSpaces, uint numTickSpaces);

  void setPos(double r) { pos_ = r; }

  QString getValueStr(double pos) const;

  void draw(CQChartsPlot *plot, QPainter *p);

 private:
  struct AxisGapData {
    double start       { 0.0 };
    double end         { 0.0 };
    double increment   { 0.0 };
    uint   numGaps     { 0 };
    uint   numGapTicks { 0 };
  };

  void calc();

  bool testAxisGaps(double start, double end, double testIncrement, uint testNumGapTicks,
                    AxisGapData &axisGapData);

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
  bool       integral_            { false };
  bool       dataLabels_          { false };
  int        column_              { -1 };
  COptReal   pos_;
};

#endif
