#include <CInterval.h>
#include <CMathRound.h>
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cmath>

namespace {

inline bool RealEq(double r1, double r2) {
  return (std::abs(r1 - r2) < 1E-6);
}

inline bool isInteger(double r) {
  return std::abs(r - int(r)) < 1E-3;
}

}

//---

struct CIntervalIncrementTest {
  double factor    { 1.0 };
  int    numTicks  { 5 };
  bool   isLog     { false };
  double incFactor { 0.0 };
  bool   integral  { false };

  CIntervalIncrementTest(double factor, int numTicks, bool isLog) :
   factor(factor), numTicks(numTicks), isLog(isLog) {
    integral = isInteger(factor);
  }
};

static CIntervalIncrementTest
incrementTests[] = {
  // factor, num, isLog
  {   1.0  , 5  , true  },
  {   1.2  , 3  , false },
  {   2.0  , 4  , false },
  {   2.5  , 5  , false },
  {   4.0  , 4  , false },
  {   5.0  , 5  , false },
  {   6.0  , 3  , false },
  {   8.0  , 4  , false },
  {  10.0  , 5  , true  },
  {  12.0  , 3  , false },
  {  20.0  , 4  , false },
  {  25.0  , 5  , false },
  {  40.0  , 4  , false },
  {  50.0  , 5  , false }
};

static int numIncrementTests = sizeof(incrementTests)/sizeof(incrementTests[0]);

CInterval::
CInterval(double start, double end, int numMajor) :
 data_(start, end)
{
  data_.numMajor = numMajor;
}

void
CInterval::
constInit() const
{
  CInterval *th = const_cast<CInterval *>(this);

  if (! th->valid_) {
    th->init();

    th->valid_ = true;
  }
}

void
CInterval::
init()
{
  // Ensure min/max are in the correct order
  double min = std::min(data_.start, data_.end);
  double max = std::max(data_.start, data_.end);

  if (isIntegral()) {
    min = std::floor(min);
    max = std::ceil (max);
  }

  //---

  // use fixed increment
  if (majorIncrement_ > 0.0) {
    calcData_.start     = min;
    calcData_.end       = max;
    calcData_.increment = majorIncrement_;

    calcData_.numMajor =
      CMathRound::RoundNearest((calcData_.end - calcData_.start)/calcData_.increment);
    calcData_.numMinor = 5;

    return;
  }

  //---

  if (data_.numMajor > 0) {
    goodTicks_.opt = data_.numMajor;
    goodTicks_.min = std::max(goodTicks_.opt/10, 1);
    goodTicks_.max = goodTicks_.opt*10;
  }
  else {
    goodTicks_ = GoodTicks();
  }

  //---

  calcData_.numMajor = -1;
  calcData_.numMinor = 5;

  //---

  calcData_.increment = data_.increment;

  if (calcData_.increment <= 0.0) {
    calcData_.increment = initIncrement();

    //---

    // Calculate other test increments
    for (int i = 0; i < numIncrementTests; i++) {
      // disable non-integral increments for integral
      if (isIntegral() && ! isInteger(incrementTests[i].factor)) {
        incrementTests[i].incFactor = 0.0;
        continue;
      }

      // disable non-log increments for log
      if (isLog() && ! incrementTests[i].isLog) {
        incrementTests[i].incFactor = 0.0;
        continue;
      }

      incrementTests[i].incFactor = calcData_.increment*incrementTests[i].factor;
    }

    //---

    // Test each increment in turn (Set default start/end to force update)
    GapData axisGapData;

    for (int i = 0; i < numIncrementTests; i++) {
      // skip disable tests
      if (incrementTests[i].incFactor <= 0.0)
        continue;

      // if tick increment set then skip if not multiple of increment
      if (tickIncrement_ > 0) {
        if (! isInteger(incrementTests[i].incFactor))
          continue;

        int incFactor1 = int(incrementTests[i].incFactor);

        if (incFactor1 % tickIncrement_ != 0)
          continue;
      }

      // test factor, ticks and update best
      if (testAxisGaps(min, max,
                       incrementTests[i].incFactor,
                       incrementTests[i].numTicks,
                       axisGapData)) {
        //axisGapData.print("  Best) ");
      }
    }

    //---

    calcData_.start     = axisGapData.start;
    calcData_.end       = axisGapData.end;
    calcData_.increment = axisGapData.increment;
    calcData_.numMinor  = (! isLog() ? axisGapData.numMinor : 10);
  }
  else {
    calcData_.start    = min;
    calcData_.end      = max;
    calcData_.numMinor = 5;
  }

  calcData_.numMajor =
    CMathRound::RoundNearest((calcData_.end - calcData_.start)/calcData_.increment);
}

double
CInterval::
initIncrement() const
{
  // Calculate length
  double min = std::min(data_.start, data_.end);
  double max = std::max(data_.start, data_.end);

  double length = max - min;

  if (isIntegral())
    length = CMathRound::RoundNearest(length);

  if (length == 0.0)
    return 1.0;

  //---

  double length1 = length;

  if (data_.numMajor > 0)
    length1 = length/data_.numMajor;

  //---

  // Calculate nearest Power of Ten to Length
  int power = CMathRound::RoundDown(log10(length1));

  if (isIntegral()) {
    if (power < 0) {
      length1 = 1.0;
      power   = 1;
    }
  }

  // Set Default Increment to 0.1 * Power of Ten
  double increment;

  if (! isIntegral() && ! isLog()) {
    if (data_.numMajor > 0)
      increment = 1;
    else
      increment = 0.1;

    if      (power < 0) {
      for (int i = 0; i < -power; i++)
        increment /= 10.0;
    }
    else if (power > 0) {
      for (int i = 0; i <  power; i++)
        increment *= 10.0;
    }
  }
  else {
    increment = 1;

    for (int i = 1; i < power; i++)
      increment *= 10.0;
  }

//std::cerr << "initIncrement: " << increment << "\n";
  return increment;
}

bool
CInterval::
testAxisGaps(double start, double end, double testIncrement, int testNumGapTicks,
             GapData &axisGapData)
{
//std::cerr << "testAxisGaps> Start: " << start << " End: " << end << " Incr: " << testIncrement <<
//             " Num: " << testNumGapTicks << "\n";

  // Calculate new start and end implied by the test increment
  double newStart = CMathRound::RoundDownF(start/testIncrement)*testIncrement;
  double newEnd   = CMathRound::RoundUpF  (end  /testIncrement)*testIncrement;

  while (newStart > start)
    newStart -= testIncrement;

  while (newEnd < end)
    newEnd += testIncrement;

  int testNumGaps = CMathRound::RoundUp((newEnd - newStart)/testIncrement);

//std::cerr << "  Adjusted) Start: " << newStart << " End: " << newEnd << " Num: " << testNumGaps << "\n";

  //---

  // If nothing set yet just update values and return
  if (! axisGapData.isSet()) {
    axisGapData = GapData(newStart, newEnd, testIncrement, testNumGaps, testNumGapTicks);
    return true;
  }

  //---

  // If the current number of gaps is not within the acceptable range and the
  // new number of gaps is within the acceptable range then update current
  if (! goodTicks_.isGood(axisGapData.numMajor) && goodTicks_.isGood(testNumGaps)) {
    axisGapData = GapData(newStart, newEnd, testIncrement, testNumGaps, testNumGapTicks);
    return true;
  }

  //---

  // If the current number of gaps is not within the acceptable range and the
  // new number of gaps is not within the acceptable range then consider it for
  // update of current if better fit
  if (! goodTicks_.isGood(axisGapData.numMajor) && ! goodTicks_.isGood(testNumGaps)) {
    // Calculate how close fit is to required range
    double delta1 = std::abs(newStart - start) + std::abs(newEnd - end);

    //---

    // If better fit than current fit or equally good fit and number of gaps
    // is nearer to optimum (goodTicks_.opt) then update current
    double delta2 = std::abs(axisGapData.start - start) + std::abs(axisGapData.end - end);

    if ((RealEq(delta1, delta2) &&
         goodTicks_.isMoreOpt(testNumGaps, axisGapData.numMajor)) ||
        delta1 < delta2) {
      axisGapData = GapData(newStart, newEnd, testIncrement, testNumGaps, testNumGapTicks);
      return true;
    }
  }

  //---

  // If the current number of gaps is within the acceptable range and the
  // new number of gaps is within the acceptable range then consider it for
  // update of current if better fit
  if (goodTicks_.isGood(axisGapData.numMajor) && goodTicks_.isGood(testNumGaps)) {
    // Calculate how close fit is to required range
    double f1 = std::abs(newEnd - newStart)/std::abs(end - start);

    //---

    // If better fit than current fit or equally good fit and number of
    // gaps is nearer to optimum (goodTicks_.opt) then update current
    double f2 = std::abs(axisGapData.end - axisGapData.start)/std::abs(end - start);

    if      (RealEq(f1, f2)) {
      if (goodTicks_.isMoreOpt(testNumGaps, axisGapData.numMajor)) {
        axisGapData = GapData(newStart, newEnd, testIncrement, testNumGaps, testNumGapTicks);
        return true;
      }
    }
    else if (testNumGaps == axisGapData.numMajor) {
      if (f1 < f2) {
        axisGapData = GapData(newStart, newEnd, testIncrement, testNumGaps, testNumGapTicks);
        return true;
      }
    }
    else {
      double opt = goodTicks_.opt;

      double f3 = std::abs(opt - testNumGaps         )/opt;
      double f4 = std::abs(opt - axisGapData.numMajor)/opt;

      if (f1 + f3 < f2 + f4) {
        axisGapData = GapData(newStart, newEnd, testIncrement, testNumGaps, testNumGapTicks);
        return true;
      }
    }
  }

  return false;
}

int
CInterval::
valueInterval(double r) const
{
  return CMathRound::RoundDown((r - calcStart())/calcIncrement());
}

void
CInterval::
intervalValues(int i, double &min, double &max) const
{
  min = calcStart() + i*calcIncrement();
  max = min + calcIncrement();
}
