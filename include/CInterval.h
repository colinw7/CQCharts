#ifndef CInterval_H
#define CInterval_H

#include <iostream>

class CInterval {
 public:
  CInterval(double min=0.0, double max=1.0, int n=10);

  //! get/set ideal interval start
  double start() const { return data_.start; }
  void setStart(double r) { data_.start = r; valid_ = false; }

  //! get/set ideal interval end
  double end() const { return data_.end; }
  void setEnd(double r) { data_.end = r; valid_ = false; }

  //! get/set ideal number of major ticks
  int numMajor() const { return data_.numMajor; }
  void setNumMajor(int i) { data_.numMajor = i; valid_ = false; }

  //! get/set is integral
  bool isIntegral() const { return integral_; }
  void setIntegral(bool b) { integral_ = b; valid_ = false; }

  //! get/set is log
  bool isLog() const { return log_; }
  void setLog(bool b) { log_ = b; valid_ = false; }

  //! get/set required major increment
  double majorIncrement() const { return majorIncrement_; }
  void setMajorIncrement(double r) { majorIncrement_ = r; valid_ = false; }

  //! get/set required increment multiplier
  double tickIncrement() const { return tickIncrement_; }
  void setTickIncrement(double r) { tickIncrement_ = r; valid_ = false; }

  //---

  // get calculated start/end/increment/num major/num minor
  double calcStart    () const { constInit(); return calcData_.start    ; }
  double calcEnd      () const { constInit(); return calcData_.end      ; }
  double calcIncrement() const { constInit(); return calcData_.increment; }
  double calcNumMajor () const { constInit(); return calcData_.numMajor ; }
  double calcNumMinor () const { constInit(); return calcData_.numMinor ; }

  // get interval for value
  int valueInterval(double r) const;

  // get range of interval
  void intervalValues(int i, double &min, double &max) const;

 private:
  class GapData;

  void constInit() const;

  void init();

  double initIncrement() const;

  bool testAxisGaps(double start, double end, double testIncrement, int testNumGapTicks,
                    GapData &axisGapData);

 private:
  struct GapData {
    double start     { 0.0 };
    double end       { 0.0 };
    double increment { 0.0 };
    int    numMajor  { 0 };
    int    numMinor  { 0 };

    GapData(double start=0.0, double end=0.0, double increment=0.0,
            int numMajor=0, int numMinor=0) :
     start(start), end(end), increment(increment), numMajor(numMajor), numMinor(numMinor) {
    }

    bool isSet() const { return (start != 0.0 || end != 0.0); }

    void print(const std::string &prefix="") {
      std::cerr << prefix << "Start: " << start << " End: " << end << " Incr: " << increment <<
                   " NumMajor: " << numMajor << " NumMinor: " << numMinor << "\n";
    }
  };

  struct GoodTicks {
    int min {  4 };
    int max { 12 };
    int opt { 10 };

    bool isGood(int n) const {
      return (n >= min && n <= max);
    }

    bool isMoreOpt(int n1, int n2) const {
      return (std::abs(n1 - opt) < std::abs(n2 - opt));
    }
  };

  GapData   data_;                     // axis preferred data
  bool      integral_       { false }; // is integral
  bool      log_            { false }; // is log
  int       majorIncrement_ { 0 };     // required major increment (if > 0)
  int       tickIncrement_  { 0 };     // required tick increment (if > 0)
  bool      valid_          { false }; // are calculated values valid
  GoodTicks goodTicks_;                // ideal tick data
  GapData   calcData_;                 // calculated tick data
};

#endif
