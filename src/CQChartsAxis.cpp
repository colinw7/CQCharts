#include <CQChartsAxis.h>
#include <CQChartsPlot.h>
#include <CQChartsModel.h>

#include <CMathGen.h>
#include <CStrUtil.h>
#include <QPainter>

#include <cstring>
#include <algorithm>

namespace {

bool isInteger(double r) {
  return fabs(r - int(r)) < 1E-3;
}

}

//------

struct AxisGoodTicks {
  uint min {  4 };
  uint max { 12 };
  uint opt { 10 };
};

struct AxisIncrementTest {
  double factor    { 1.0 };
  uint   numTicks  { 5   };
  double incFactor { 0.0 };
  bool   integral  { false };

  AxisIncrementTest(double factor, uint numTicks) :
   factor(factor), numTicks(numTicks) {
    integral = isInteger(factor);
  }
};

static AxisIncrementTest
axesIncrementTests[] = {
  {  1.0, 5 },
  {  1.2, 3 },
  {  2.0, 4 },
  {  2.5, 5 },
  {  4.0, 4 },
  {  5.0, 5 },
  {  6.0, 3 },
  {  8.0, 4 },
  { 10.0, 5 },
  { 12.0, 3 },
  { 20.0, 4 },
  { 25.0, 5 },
  { 40.0, 4 },
  { 50.0, 5 }
};

static uint numAxesIncrementTests = sizeof(axesIncrementTests)/sizeof(axesIncrementTests[0]);

AxisGoodTicks axisGoodTicks;

//---

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
setIntegral(bool b)
{
  integral_ = b;

  calc();
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

  if (isIntegral()) {
    minAxis = std::floor(minAxis);
    maxAxis = std::ceil (maxAxis);
  }

  //------

  /* Calculate Length */

  double length = fabs(maxAxis - minAxis);

  if (length == 0.0)
    return;

  if (isIntegral())
    length = std::ceil(length);

  //------

  // Calculate nearest Power of Ten to Length

  int power = CMathGen::RoundDown(log10(length));

  if (isIntegral()) {
    if (power < 0)
      power = 1;
  }

  //------

  if (majorIncrement_ <= 0.0) {
    // Set Default Increment to 0.1 * Power of Ten
    double increment;

    if (! isIntegral()) {
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

    //------

    // Calculate other test Increments

    for (uint i = 0; i < numAxesIncrementTests; i++) {
      if (isIntegral() && ! isInteger(axesIncrementTests[i].factor)) {
        axesIncrementTests[i].incFactor = 0.0;
        continue;
      }

      axesIncrementTests[i].incFactor = increment*axesIncrementTests[i].factor;
    }

    //------

    // Test each Increment in turn
    // (Set Default Start/End to Force Update)

    AxisGapData axisGapData;

    for (uint i = 0; i < numAxesIncrementTests; i++) {
      if (axesIncrementTests[i].incFactor <= 0)
        continue;

      if (tickIncrement_ > 0) {
        if (! isInteger(axesIncrementTests[i].incFactor))
          continue;

        int incFactor1 = int(axesIncrementTests[i].incFactor);

        if (incFactor1 % tickIncrement_ != 0)
          continue;
      }

      testAxisGaps(minAxis, maxAxis,
                   axesIncrementTests[i].incFactor,
                   axesIncrementTests[i].numTicks,
                   axisGapData);
    }

    start1_   = axisGapData.start;
    end1_     = axisGapData.end;
    increment = axisGapData.increment;

    int numGapTicks = axisGapData.numGapTicks;

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
             AxisGapData &axisGapData)
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

  if (axisGapData.start == 0.0 && axisGapData.end == 0.0) {
    axisGapData.start = newStart;
    axisGapData.end   = newEnd;

    axisGapData.increment   = testIncrement;
    axisGapData.numGaps     = testNumGaps;
    axisGapData.numGapTicks = testNumGapTicks;

    return true;
  }

  //------

  // If the current number of gaps is not within the acceptable range
  // and the new number of gaps is within the acceptable range then
  // update current

  if ((axisGapData.numGaps <  axisGoodTicks.min || axisGapData.numGaps >  axisGoodTicks.max) &&
      (testNumGaps         >= axisGoodTicks.min && testNumGaps         <= axisGoodTicks.max)) {
    axisGapData.start = newStart;
    axisGapData.end   = newEnd;

    axisGapData.increment   = testIncrement;
    axisGapData.numGaps     = testNumGaps;
    axisGapData.numGapTicks = testNumGapTicks;

    return true;
  }

  //------

  // If the current number of gaps is not within the acceptable range
  // and the new number of gaps is not within the acceptable range then
  // consider it for update of current if better fit

  if ((axisGapData.numGaps < axisGoodTicks.min || axisGapData.numGaps > axisGoodTicks.max) &&
      (testNumGaps         < axisGoodTicks.min || testNumGaps         > axisGoodTicks.max)) {
    // Calculate how close fit is to required range

    double delta1 = fabs(newStart - start) + fabs(newEnd - end);

    //------

    // If better fit than current fit or equally good fit and
    // number of gaps is nearer to optimum (axisGoodTicks.opt) then
    // update current

    double delta2 = fabs(axisGapData.start - start) + fabs(axisGapData.end - end);

    if (((fabs(delta1 - delta2) < 1E-6) &&
         (abs(testNumGaps         - axisGoodTicks.opt) <
          abs(axisGapData.numGaps - axisGoodTicks.opt))) ||
        delta1 < delta2) {
      axisGapData.start = newStart;
      axisGapData.end   = newEnd;

      axisGapData.increment   = testIncrement;
      axisGapData.numGaps     = testNumGaps;
      axisGapData.numGapTicks = testNumGapTicks;

      return true;
    }
  }

  //------

  // If the current number of gaps is within the acceptable range
  // and the new number of gaps is within the acceptable range then
  // consider it for update of current if better fit

  if ((axisGapData.numGaps >= axisGoodTicks.min && axisGapData.numGaps <= axisGoodTicks.max) &&
      (testNumGaps         >= axisGoodTicks.min && testNumGaps         <= axisGoodTicks.max)) {
    // Calculate how close fit is to required range

    double delta1 = fabs(newStart - start) + fabs(newEnd - end);

    //------

    // If better fit than current fit or equally good fit and
    // number of gaps is nearer to optimum (axisGoodTicks.opt) then
    // update current

    double delta2 = fabs(axisGapData.start - start) + fabs(axisGapData.end - end);

    if (((fabs(delta1 - delta2) < 1E-6) &&
         (abs(testNumGaps         - axisGoodTicks.opt) <
          abs(axisGapData.numGaps - axisGoodTicks.opt))) ||
        delta1 < delta2) {
      axisGapData.start = newStart;
      axisGapData.end   = newEnd;

      axisGapData.increment   = testIncrement;
      axisGapData.numGaps     = testNumGaps;
      axisGapData.numGapTicks = testNumGapTicks;

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

    if      (model) {
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
    else if (isDataLabels()) {
      int row = int(pos);

      QModelIndex ind = plot_->model()->index(row, column_);

      QVariant header = plot_->model()->data(ind, Qt::DisplayRole);

      if (header.isValid())
        return header.toString();

      return CStrUtil::toString(pos).c_str();
    }
    else {
      return CStrUtil::toString(pos).c_str();
    }
  }
  else
    return CStrUtil::toString(pos).c_str();
}

void
CQChartsAxis::
draw(CQChartsPlot *plot, QPainter *p)
{
  double ax1, ay1, ax2, ay2, ax3, ay3;

  if (direction_ == DIR_HORIZONTAL) {
    double xmin = getStart();
    double xmax = getEnd  ();

    double ymin = plot->dataRange().ymin();
    double ymax = plot->dataRange().ymax();

    plot->windowToPixel(xmin, ymin, ax1, ay1);
    plot->windowToPixel(xmax, ymax, ax2, ay2);

    plot->windowToPixel(xmin, pos_.getValue(ymin), ax3, ay3);
  }
  else {
    double ymin = getStart();
    double ymax = getEnd  ();

    double xmin = plot->dataRange().xmin();
    double xmax = plot->dataRange().xmax();

    plot->windowToPixel(xmin, ymin, ax1, ay1);
    plot->windowToPixel(xmax, ymax, ax2, ay2);

    plot->windowToPixel(pos_.getValue(xmin), ymin, ax3, ay3);
  }

  //------

  QFontMetrics fm(getLabelFont());

  p->save();

  if (getLineDisplayed()) {
    p->setPen(getLineColor());

    if (direction_ == DIR_HORIZONTAL)
      p->drawLine(ax1, ay3, ax2, ay3);
    else
      p->drawLine(ax3, ay1, ax3, ay2);
  }

  double inc  = getMajorIncrement();
  double inc1 = inc/getNumMinorTicks();

  double pos1 = start1_;

  for (uint i = 0; i < getNumMajorTicks() + 1; i++) {
    // draw major line (grid and tick)
    if (pos1 >= getStart() && pos1 <= getEnd()) {
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
        p->drawLine(ppx, ay3, ppx, ay3 + dt1);
      else
        p->drawLine(ax3, ppy, ax3 - dt1, ppy);
    }

    if (getMinorTicksDisplayed() && i < getNumMajorTicks()) {
      for (uint j = 1; j < getNumMinorTicks(); j++) {
        double pos2 = pos1 + j*inc1;

        // draw minor tick line
        if (pos2 >= getStart() && pos2 <= getEnd()) {
          double ppx, ppy;

          plot->windowToPixel(pos2, pos2, ppx, ppy);

          int dt2 = (getSide() == SIDE_BOTTOM_LEFT ? 4 : -4);

          if (direction_ == DIR_HORIZONTAL)
            p->drawLine(ppx, ay3, ppx, ay3 + dt2);
          else
            p->drawLine(ax3, ppy, ax3 - dt2, ppy);
        }
      }
    }

    //---

    if (getLabelDisplayed()) {
      // draw major tick label
      if (pos1 >= getStart() && pos1 <= getEnd()) {
        double ppx, ppy;

        plot->windowToPixel(pos1, pos1, ppx, ppy);

        QString text = getValueStr(pos1);

        int tw = fm.width(text);
        int ta = fm.ascent();
        int td = fm.descent();

        p->setPen (getLabelColor());
        p->setFont(getLabelFont ());

        if (direction_ == DIR_HORIZONTAL) {
          if (getSide() == SIDE_BOTTOM_LEFT)
            p->drawText(ppx - tw/2, ay3 + 10 + ta, text);
          else
            p->drawText(ppx - tw/2, ay3 - 10 - td, text);
        }
        else {
          if (getSide() == SIDE_BOTTOM_LEFT)
            p->drawText(ax3 - tw - 10, ppy + ta/2, text);
          else
            p->drawText(ax3 + tw + 10, ppy + ta/2, text);
        }
      }
    }

    //---

    pos1 += inc;
  }

  p->restore();
}
