#include <CQChartsAxis.h>
#include <CQChartsPlot.h>
#include <CQChartsModel.h>

#include <CMathGen.h>
#include <CStrUtil.h>
#include <QPainter>

#include <cstring>
#include <algorithm>

#define MIN_GOOD_TICKS 4
#define MAX_GOOD_TICKS 12
#define OPTIMUM_TICKS  10

#define MAX_GAP_TESTS 14

struct AxisIncrementTest {
  double factor;
  uint   numTicks;
  double incFactor;
};

static AxisIncrementTest
axesIncrementTests[MAX_GAP_TESTS] = {
  {  1.0, 5, 0 },
  {  1.2, 3, 0 },
  {  2.0, 4, 0 },
  {  2.5, 5, 0 },
  {  4.0, 4, 0 },
  {  5.0, 5, 0 },
  {  6.0, 3, 0 },
  {  8.0, 4, 0 },
  { 10.0, 5, 0 },
  { 12.0, 3, 0 },
  { 20.0, 4, 0 },
  { 25.0, 5, 0 },
  { 40.0, 4, 0 },
  { 50.0, 5, 0 }
};

CQChartsAxis::
CQChartsAxis(CQChartsPlot *plot, Direction direction, double start, double end) :
 CQChartsAxisBase(plot, direction, start, end), start1_(start), end1_(end)
{
  calc();
}

void
CQChartsAxis::
setMajorIncrement(double i)
{
  majorIncrement_ = i;

  calc();
}

void
CQChartsAxis::
setTickIncrement(uint tickIncrement)
{
  tickIncrement_ = tickIncrement;

  calc();
}

void
CQChartsAxis::
setTickSpaces(double *tickSpaces, uint numTickSpaces)
{
  tickSpaces_.resize(numTickSpaces);

  memcpy(&tickSpaces_[0], tickSpaces, numTickSpaces*sizeof(double));
}

void
CQChartsAxis::
calc()
{
  numTicks1_ = 1;
  numTicks2_ = 0;

  //------

  // Ensure Axis Start and End are in the Correct Order

  double minAxis = std::min(start_, end_);
  double maxAxis = std::max(start_, end_);

  //------

  /* Calculate Length */

  double length = fabs(maxAxis - minAxis);

  if (length == 0.0)
    return;

  //------

  // Calculate nearest Power of Ten to Length

  int power = CMathGen::RoundDown(log10(length));

  //------

  if (majorIncrement_ <= 0.0) {
    // Set Default Increment to 0.1 * Power of Ten
    double increment = 0.1;

    if      (power < 0) {
      for (int i = 0; i < -power; i++)
        increment /= 10.0;
    }
    else if (power > 0) {
      for (int i = 0; i <  power; i++)
        increment *= 10.0;
    }

    //------

    // Calculate other test Increments

    for (int i = 0; i < MAX_GAP_TESTS; i++)
      axesIncrementTests[i].incFactor = increment*axesIncrementTests[i].factor;

    //------

    // Set Default Start/End to Force Update

    start1_ = 0.0;
    end1_   = 0.0;

    //------

    // Test each Increment in turn

    uint numGaps, numGapTicks;

    for (int i = 0; i < MAX_GAP_TESTS; i++) {
      if (tickIncrement_ > 0) {
        int incFactor1 = (int) axesIncrementTests[i].incFactor;

        if (((double) incFactor1) != axesIncrementTests[i].incFactor)
          continue;

        if (incFactor1 % tickIncrement_ != 0)
          continue;
      }

      testAxisGaps(minAxis, maxAxis,
                   axesIncrementTests[i].incFactor,
                   axesIncrementTests[i].numTicks,
                   &start1_, &end1_, &increment,
                   &numGaps, &numGapTicks);
    }

    //------

    // Set the Gap Positions

    numTicks1_ = CMathGen::RoundDown((end1_ - start1_)/increment + 0.5);
    numTicks2_ = numGapTicks;
  }
  else {
    start1_    = start_;
    end1_      = end_;
    numTicks1_ = CMathGen::RoundDown((end1_ - start1_)/majorIncrement_ + 0.5);
    numTicks2_ = 5;
  }
}

bool
CQChartsAxis::
testAxisGaps(double start, double end, double testIncrement, uint testNumGapTicks,
             double *start1, double *end1, double *increment, uint *numGaps, uint *numGapTicks)
{
  // Calculate New Start and End implied by the Test Increment

  double newStart = CMathGen::RoundDown(start/testIncrement)*testIncrement;
  double newEnd   = CMathGen::RoundUp  (end  /testIncrement)*testIncrement;

  while (newStart > start)
    newStart -= testIncrement;

  while (newEnd < end)
    newEnd += testIncrement;

  uint testNumGaps = CMathGen::RoundUp((newEnd - newStart)/testIncrement);

  //------

  // If nothing set yet just update values and return

  if (*start1 == 0.0 && *end1 == 0.0) {
    *start1 = newStart;
    *end1   = newEnd;

    *increment   = testIncrement;
    *numGaps     = testNumGaps;
    *numGapTicks = testNumGapTicks;

    return true;
  }

  //------

  // If the current number of gaps is not within the acceptable range
  // and the new number of gaps is within the acceptable range then
  // update current

  if ((   *numGaps <  MIN_GOOD_TICKS ||    *numGaps >  MAX_GOOD_TICKS) &&
      (testNumGaps >= MIN_GOOD_TICKS && testNumGaps <= MAX_GOOD_TICKS)) {
    *start1 = newStart;
    *end1   = newEnd;

    *increment   = testIncrement;
    *numGaps     = testNumGaps;
    *numGapTicks = testNumGapTicks;

    return true;
  }

  //------

  // If the current number of gaps is not within the acceptable range
  // and the new number of gaps is not within the acceptable range then
  // consider it for update of current if better fit

  if ((   *numGaps < MIN_GOOD_TICKS ||    *numGaps > MAX_GOOD_TICKS) &&
      (testNumGaps < MIN_GOOD_TICKS || testNumGaps > MAX_GOOD_TICKS)) {
    // Calculate how close fit is to required range

    double delta1 = fabs(newStart - start) + fabs(newEnd - end);

    //------

    // If better fit than current fit or equally good fit and
    // number of gaps is nearer to optimum (OPTIMUM_TICKS) then
    // update current

    double delta2 = fabs(*start1 - start) + fabs(*end1 - end);

    if (((fabs(delta1 - delta2) < 1E-6) &&
         (abs(testNumGaps - OPTIMUM_TICKS) < abs(*numGaps - OPTIMUM_TICKS))) ||
        delta1 < delta2) {
      *start1 = newStart;
      *end1   = newEnd;

      *increment   = testIncrement;
      *numGaps     = testNumGaps;
      *numGapTicks = testNumGapTicks;

      return true;
    }
  }

  //------

  // If the current number of gaps is within the acceptable range
  // and the new number of gaps is within the acceptable range then
  // consider it for update of current if better fit

  if ((   *numGaps >= MIN_GOOD_TICKS &&    *numGaps <= MAX_GOOD_TICKS) &&
      (testNumGaps >= MIN_GOOD_TICKS && testNumGaps <= MAX_GOOD_TICKS)) {
    // Calculate how close fit is to required range

    double delta1 = fabs(newStart - start) + fabs(newEnd - end);

    //------

    // If better fit than current fit or equally good fit and
    // number of gaps is nearer to optimum (OPTIMUM_TICKS) then
    // update current

    double delta2 = fabs(*start1 - start) + fabs(*end1 - end);

    if (((fabs(delta1 - delta2) < 1E-6) &&
         (abs(testNumGaps - OPTIMUM_TICKS) < abs(*numGaps - OPTIMUM_TICKS))) ||
        delta1 < delta2) {
      *start1 = newStart;
      *end1   = newEnd;

      *increment   = testIncrement;
      *numGaps     = testNumGaps;
      *numGapTicks = testNumGapTicks;

      return true;
    }
  }

  return false;
}

double
CQChartsAxis::
getMajorIncrement() const
{
  if (majorIncrement_ > 0.0)
    return majorIncrement_;
  else {
    if (numTicks1_ > 0)
      return (end1_ - start1_)/numTicks1_;
    else
      return 0.0;
  }
}

double
CQChartsAxis::
getMinorIncrement() const
{
  if (numTicks1_ > 0 && numTicks2_ > 0)
    return (end1_ - start1_)/(numTicks1_*numTicks2_);
  else
    return 0.0;
}

QString
CQChartsAxis::
getValueStr(double pos) const
{
  if (column_ >= 0) {
    CQChartsModel *model = qobject_cast<CQChartsModel *>(plot_->model());

    if (model) {
      QString type = model->columnType(column_);

      QString            baseType;
      CQChartsNameValues nameValues;

      CQChartsColumn::decodeType(type, baseType, nameValues);

      CQChartsColumnType *typeData = CQChartsColumnTypeMgrInst->getType(baseType);

      if (typeData) {
        return typeData->dataName(pos, nameValues);
      }
      else
        return CStrUtil::toString(pos).c_str();
    }
    else
      return CStrUtil::toString(pos).c_str();
  }
  else
    return CStrUtil::toString(pos).c_str();
}

void
CQChartsAxis::
draw(CQChartsPlot *plot, QPainter *p)
{
  double ax1, ay1, ax2, ay2;

  if (direction_ == DIR_HORIZONTAL) {
    double xmin = getStart();
    double xmax = getEnd  ();

    double ymin = plot->dataRange().ymin();
    double ymax = plot->dataRange().ymax();

    plot->windowToPixel(xmin, ymin, ax1, ay1);
    plot->windowToPixel(xmax, ymax, ax2, ay2);
  }
  else {
    double ymin = getStart();
    double ymax = getEnd  ();

    double xmin = plot->dataRange().xmin();
    double xmax = plot->dataRange().xmax();

    plot->windowToPixel(xmin, ymin, ax1, ay1);
    plot->windowToPixel(xmax, ymax, ax2, ay2);
  }

  //------

  QFontMetrics fm(getLabelFont());

  p->save();

  if (getLineDisplayed()) {
    p->setPen(getLineColor());

    if (direction_ == DIR_HORIZONTAL)
      p->drawLine(ax1, ay1, ax2, ay1);
    else
      p->drawLine(ax1, ay1, ax1, ay2);
  }

  double inc  = getMajorIncrement();
  double inc1 = inc/getNumMinorTicks();

  double pos1 = getStart();

  for (uint i = 0; i < getNumMajorTicks() + 1; i++) {
    if (pos1 > end1_) continue;

    double ppx, ppy;

    plot->windowToPixel(pos1, pos1, ppx, ppy);

    int dt1 = (getSide() == SIDE_BOTTOM_LEFT ? 8 : -8);

    if (getGridDisplayed()) {
      p->setPen(QPen(getGridColor(), 0.0, Qt::DotLine));

      if (direction_ == DIR_HORIZONTAL)
        p->drawLine(ppx, ay1, ppx, ay2);
      else
        p->drawLine(ax1, ppy, ax2, ppy);
    }

    p->setPen(getLineColor());

    if (direction_ == DIR_HORIZONTAL)
      p->drawLine(ppx, ay1, ppx, ay1 + dt1);
    else
      p->drawLine(ax1, ppy, ax1 - dt1, ppy);

    if (getMinorTicksDisplayed() && i < getNumMajorTicks()) {
      for (uint j = 1; j < getNumMinorTicks(); j++) {
        double pos2 = pos1 + j*inc1;

        if (pos2 > end1_) continue;

        double ppx, ppy;

        plot->windowToPixel(pos2, pos2, ppx, ppy);

        int dt2 = (getSide() == SIDE_BOTTOM_LEFT ? 4 : -4);

        if (direction_ == DIR_HORIZONTAL)
          p->drawLine(ppx, ay1, ppx, ay1 + dt2);
        else
          p->drawLine(ax1, ppy, ax1 - dt2, ppy);
      }
    }

    if (getLabelDisplayed()) {
      QString text = getValueStr(pos1);

      int tw = fm.width(text);
      int ta = fm.ascent();
      int td = fm.descent();

      p->setPen (getLabelColor());
      p->setFont(getLabelFont ());

      if (direction_ == DIR_HORIZONTAL) {
        if (getSide() == SIDE_BOTTOM_LEFT)
          p->drawText(ppx - tw/2, ay1 + 10 + ta, text);
        else
          p->drawText(ppx - tw/2, ay1 - 10 - td, text);
      }
      else {
        if (getSide() == SIDE_BOTTOM_LEFT)
          p->drawText(ax1 - tw - 10, ppy + ta/2, text);
        else
          p->drawText(ax1 + tw + 10, ppy + ta/2, text);
      }
    }

    pos1 += inc;
  }

  p->restore();
}
