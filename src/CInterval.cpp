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

static int day_seconds    = 24*60*60;
static int hour_seconds   = 60*60;
static int minute_seconds = 60;

inline int timeLengthToYears(time_t t1, time_t t2) {
  struct tm *lt1 = localtime(&t1);

  int y1 = CMathRound::RoundDown(1900 + lt1->tm_year + lt1->tm_yday/365.0);

  struct tm *lt2 = localtime(&t2);

  int y2 = CMathRound::RoundUp(1900 + lt2->tm_year + lt2->tm_yday/365.0);

  return y2 - y1;
}

inline int timeLengthToMonths(time_t t1, time_t t2) {
  static double month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 31, 30, 31, 30, 31 };

  struct tm *lt1 = localtime(&t1);

  int y1 = CMathRound::RoundDown(lt1->tm_year*12 + lt1->tm_mon +
                                 (lt1->tm_mday - 1)/(month_days[lt1->tm_mon] - 1));

  struct tm *lt2 = localtime(&t2);

  int y2 = CMathRound::RoundUp(lt2->tm_year*12 + lt2->tm_mon +
                               (lt2->tm_mday - 1)/(month_days[lt2->tm_mon] - 1));

  return y2 - y1;
}

inline int timeLengthToDays(double t1, double t2) {
  double t = t2 - t1;

  return (t + day_seconds - 1)/day_seconds;
}

inline int timeLengthToHours(double t1, double t2) {
  double t = t2 - t1;

  return (t + hour_seconds - 1)/hour_seconds;
}

inline int timeLengthToMinutes(double t1, double t2) {
  double t = t2 - t1;

  return (t + minute_seconds - 1)/minute_seconds;
}

inline int timeLengthToSeconds(time_t t1, time_t t2) {
  double t = t2 - t1;

  return t;
}

#if 0
inline double roundTimeToYear(time_t t) {
  struct tm *lt = localtime(&t);

  lt->tm_mon   = 0;
  lt->tm_mday  = 1;
  lt->tm_hour  = 0;
  lt->tm_min   = 0;
  lt->tm_sec   = 0;
  lt->tm_wday  = 0;
  lt->tm_yday  = 0;
  lt->tm_isdst = -1; // auto DST

  return mktime(lt);
}

inline double roundTimeToMonth(time_t t) {
  struct tm *lt = localtime(&t);

  lt->tm_mday  = 1;
  lt->tm_hour  = 0;
  lt->tm_min   = 0;
  lt->tm_sec   = 0;
  lt->tm_wday  = 0;
  lt->tm_isdst = -1; // auto DST

  return mktime(lt);
}

inline double roundTimeToDay(time_t t) {
  struct tm *lt = localtime(&t);

  lt->tm_hour  = 0;
  lt->tm_min   = 0;
  lt->tm_sec   = 0;
  lt->tm_isdst = -1; // auto DST

  return mktime(lt);
}

inline double roundTimeToHour(time_t t) {
  struct tm *lt = localtime(&t);

  lt->tm_min   = 0;
  lt->tm_sec   = 0;
  lt->tm_isdst = -1; // auto DST

  return mktime(lt);
}

inline double roundTimeToMinute(time_t t) {
  struct tm *lt = localtime(&t);

  lt->tm_sec   = 0;
  lt->tm_isdst = -1; // auto DST

  return mktime(lt);
}
#endif

inline int timeToYear(time_t t) {
  struct tm *lt = localtime(&t);

  return 1900 + lt->tm_year;
}

inline int timeToMonths(time_t t) {
  struct tm *lt = localtime(&t);

  return lt->tm_mon;
}

inline int timeToDays(time_t t) {
  struct tm *lt = localtime(&t);

  return lt->tm_mday - 1;
}

inline int timeToHours(time_t t) {
  struct tm *lt = localtime(&t);

  return lt->tm_hour;
}

inline int timeToMinutes(time_t t) {
  struct tm *lt = localtime(&t);

  return lt->tm_min;
}

inline int timeToSeconds(time_t t) {
  struct tm *lt = localtime(&t);

  return lt->tm_sec;
}

inline double yearsToTime(int year) {
  struct tm tm;

  tm.tm_year  = year - 1900;
  tm.tm_mon   = 0;
  tm.tm_mday  = 1;
  tm.tm_hour  = 0;
  tm.tm_min   = 0;
  tm.tm_sec   = 0;
  tm.tm_isdst = -1; // auto DST

  return mktime(&tm);
}

inline double monthsToTime(const CInterval::TimeData &timeData, int month) {
  struct tm tm;

  tm.tm_year  = timeData.year - 1900;
  tm.tm_mon   = month;
  tm.tm_mday  = 1;
  tm.tm_hour  = 0;
  tm.tm_min   = 0;
  tm.tm_sec   = 0;
  tm.tm_isdst = -1; // auto DST

  return mktime(&tm);
}

inline double daysToTime(const CInterval::TimeData &timeData, int day) {
  struct tm tm;

  tm.tm_year  = timeData.year - 1900;
  tm.tm_mon   = timeData.month;
  tm.tm_mday  = day + 1;
  tm.tm_hour  = 0;
  tm.tm_min   = 0;
  tm.tm_sec   = 0;
  tm.tm_isdst = -1; // auto DST

  return mktime(&tm);
}

inline double hoursToTime(const CInterval::TimeData &timeData, int hour) {
  struct tm tm;

  tm.tm_year  = timeData.year - 1900;
  tm.tm_mon   = timeData.month;
  tm.tm_mday  = timeData.day + 1;
  tm.tm_hour  = hour;
  tm.tm_min   = 0;
  tm.tm_sec   = 0;
  tm.tm_isdst = -1; // auto DST

  return mktime(&tm);
}

inline double minutesToTime(const CInterval::TimeData &timeData, int minute) {
  struct tm tm;

  tm.tm_year  = timeData.year - 1900;
  tm.tm_mon   = timeData.month;
  tm.tm_mday  = timeData.day + 1;
  tm.tm_hour  = timeData.hour;
  tm.tm_min   = minute;
  tm.tm_sec   = 0;
  tm.tm_isdst = -1; // auto DST

  return mktime(&tm);
}

inline double secondsToTime(const CInterval::TimeData &timeData, int second) {
  struct tm tm;

  tm.tm_year  = timeData.year - 1900;
  tm.tm_mon   = timeData.month;
  tm.tm_mday  = timeData.day + 1;
  tm.tm_hour  = timeData.hour;
  tm.tm_min   = timeData.minute;
  tm.tm_sec   = second;
  tm.tm_isdst = -1; // auto DST

  return mktime(&tm);
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
  bool integral = isIntegral();

  timeType_ = TimeType::SECONDS;

  startTime_.year   = 0;
  startTime_.month  = 0;
  startTime_.day    = 0;
  startTime_.hour   = 0;
  startTime_.minute = 0;
  startTime_.second = 0;

  // Ensure min/max are in the correct order
  double min = std::min(data_.start, data_.end);
  double max = std::max(data_.start, data_.end);

  if      (isIntegral()) {
    min = std::floor(min);
    max = std::ceil (max);
  }
  else if (isDate()) {
    int y = timeLengthToYears  (min, max);
    int m = timeLengthToMonths (min, max);
    int d = timeLengthToDays   (min, max);

    startTime_.year  = timeToYear   (min);
    startTime_.month = timeToMonths (min);
    startTime_.day   = timeToDays   (min);

    min = 0;

    if      (y >= 5) {
      //std::cout << "years\n";
      timeType_ = TimeType::YEARS;
      max       = y;
    }
    else if (m >= 3) {
      //std::cout << "months\n";
      timeType_ = TimeType::MONTHS;
      max       = m;
    }
    else if (d >= 4) {
      //std::cout << "days\n";
      timeType_ = TimeType::DAYS;
      max       = d;
    }

    integral = true;
  }
  else if (isTime()) {
    int h = timeLengthToHours  (min, max);
    int m = timeLengthToMinutes(min, max);
    int s = timeLengthToSeconds(min, max);

    startTime_.hour   = timeToHours  (min);
    startTime_.minute = timeToMinutes(min);
    startTime_.second = timeToSeconds(min);

    min = 0;

    if      (h >= 12) {
      //std::cout << "hours\n";
      timeType_ = TimeType::HOURS;
      max       = h;
    }
    else if (m >= 10) {
      //std::cout << "minutes\n";
      timeType_ = TimeType::MINUTES;
      max       = m;
    }
    else {
      //std::cout << "seconds\n";
      timeType_ = TimeType::SECONDS;
      max       = s;
    }

    integral = true;
  }

  //---

  // use fixed increment
  if (majorIncrement_ > 0.0 && (! isDate() && ! isTime())) {
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
    calcData_.increment = initIncrement(min, max, integral);

    //---

    // Calculate other test increments
    for (int i = 0; i < numIncrementTests; i++) {
      // disable non-integral increments for integral
      if (integral && ! isInteger(incrementTests[i].factor)) {
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

  if      (isDate()) {
    if      (timeType_ == TimeType::YEARS) {
      calcData_.numMinor = 12;
    }
    else if (timeType_ == TimeType::MONTHS) {
      calcData_.numMinor = 4;
    }
    else if (timeType_ == TimeType::DAYS) {
      calcData_.numMinor = 4;
    }
  }
  else if (isTime()) {
    if      (timeType_ == TimeType::HOURS) {
      calcData_.numMinor = 6;
    }
    else if (timeType_ == TimeType::MINUTES) {
      calcData_.numMinor = 12;
    }
    else if (timeType_ == TimeType::SECONDS) {
      calcData_.numMinor = 12;
    }
  }
}

double
CInterval::
initIncrement(double imin, double imax, bool integral) const
{
  // Calculate length
  double min = std::min(imin, imax);
  double max = std::max(imin, imax);

  double length = max - min;

  if (integral)
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

  if (integral) {
    if (power < 0) {
      length1 = 1.0;
      power   = 1;
    }
  }

  // Set Default Increment to 0.1 * Power of Ten
  double increment;

  if (! integral && ! isLog()) {
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

//std::cerr << "  Adjusted) Start: " << newStart << " End: " << newEnd << " Num: " <<
//             testNumGaps << "\n";

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

double
CInterval::
interval(int i) const
{
  if       (isDate()) {
    if      (timeType_ == TimeType::YEARS) {
      return yearsToTime(startTime_.year + i*calcIncrement());
    }
    else if (timeType_ == TimeType::MONTHS) {
      return monthsToTime(startTime_, startTime_.month + i*calcIncrement());
    }
    else if (timeType_ == TimeType::DAYS) {
      return daysToTime(startTime_, startTime_.day + i*calcIncrement());
    }
    else {
      return 0;
    }
  }
  else if (isTime()) {
    if      (timeType_ == TimeType::HOURS) {
      return hoursToTime(startTime_, startTime_.hour + i*calcIncrement());
    }
    else if (timeType_ == TimeType::MINUTES) {
      return minutesToTime(startTime_, startTime_.minute + i*calcIncrement());
    }
    else if (timeType_ == TimeType::SECONDS) {
      return secondsToTime(startTime_, startTime_.second + i*calcIncrement());
    }
    else {
      return 0;
    }
  }
  else {
    return calcStart() + i*calcIncrement();
  }
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
