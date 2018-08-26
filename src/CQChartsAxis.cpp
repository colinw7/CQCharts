#include <CQChartsAxis.h>
#include <CQChartsAxisObj.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsEnv.h>

#include <CQPropertyViewModel.h>
#include <CMathRound.h>

#include <QPainter>

#include <cstring>
#include <algorithm>

//------

#if 0
struct AxisGoodTicks {
  uint min {  4 };
  uint max { 12 };
  uint opt { 10 };
};

struct AxisIncrementTest {
  double factor    { 1.0 };
  uint   numTicks  { 5 };
  bool   isLog     { false };
  double incFactor { 0.0 };
  bool   integral  { false };

  AxisIncrementTest(double factor, uint numTicks, bool isLog) :
   factor(factor), numTicks(numTicks), isLog(isLog) {
    integral = CMathUtil::isInteger(factor);
  }
};

static AxisIncrementTest
axesIncrementTests[] = {
  {  1.0, 5, true  },
  {  1.2, 3, false },
  {  2.0, 4, false },
  {  2.5, 5, false },
  {  4.0, 4, false },
  {  5.0, 5, false },
  {  6.0, 3, false },
  {  8.0, 4, false },
  { 10.0, 5, true  },
  { 12.0, 3, false },
  { 20.0, 4, false },
  { 25.0, 5, false },
  { 40.0, 4, false },
  { 50.0, 5, false }
};

static uint numAxesIncrementTests = sizeof(axesIncrementTests)/sizeof(axesIncrementTests[0]);

AxisGoodTicks axisGoodTicks;
#endif

//---

CQChartsAxis::
CQChartsAxis(CQChartsPlot *plot, Direction direction, double start, double end) :
 QObject(plot), plot_(plot), direction_(direction),
 start_(std::min(start, end)), end_(std::max(start, end)), calcStart_(start), calcEnd_(end),
 editHandles_(plot, CQChartsEditHandles::Mode::MOVE)
{
  setObjectName("axis");

  CQChartsColor themeFg   (CQChartsColor::Type::INTERFACE_VALUE, 1);
  CQChartsColor themeGray1(CQChartsColor::Type::INTERFACE_VALUE, 0.7);
  CQChartsColor themeGray2(CQChartsColor::Type::INTERFACE_VALUE, 0.3);
  CQChartsColor themeGray3(CQChartsColor::Type::INTERFACE_VALUE, 0.3);

  label_     = new CQChartsAxisLabel(this);
  tickLabel_ = new CQChartsAxisTickLabel(this);

  label_    ->setTextColor(themeFg);
  tickLabel_->setTextColor(themeFg);

  lineData_.color = themeGray1;

  // init grid
  majorGridLineData_.visible = false;
  majorGridLineData_.color   = themeGray2;

  minorGridLineData_.visible = false;
  minorGridLineData_.color   = themeGray2;

  gridFill_.visible = false;
  gridFill_.color   = themeGray3;
  gridFill_.alpha   = 0.5;

  setGridMajorDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));
  setGridMinorDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  calc();
}

CQChartsAxis::
~CQChartsAxis()
{
  delete label_;
  delete tickLabel_;
}

void
CQChartsAxis::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"  );
  model->addProperty(path, this, "direction");
  model->addProperty(path, this, "side"     );
  model->addProperty(path, this, "integral" );
  model->addProperty(path, this, "log"      );
  model->addProperty(path, this, "format"   );

  model->addProperty(path, this, "tickIncrement" );
  model->addProperty(path, this, "majorIncrement");
  model->addProperty(path, this, "start"         );
  model->addProperty(path, this, "end"           );
  model->addProperty(path, this, "includeZero"   );

  QString posPath = path + "/position";

  model->addProperty(posPath, this, "hasPosition", "enabled");
  model->addProperty(posPath, this, "position"   , "value"  );

  QString linePath = path + "/line";

  model->addProperty(linePath, this, "lineDisplayed", "visible");
  model->addProperty(linePath, this, "lineColor"    , "color"  );
  model->addProperty(linePath, this, "lineWidth"    , "width"  );
  model->addProperty(linePath, this, "lineDash"     , "dash"   );

  QString ticksPath = path + "/ticks";

  QString majorTicksPath = ticksPath + "/major";
  QString minorTicksPath = ticksPath + "/minor";

  model->addProperty(majorTicksPath, this, "majorTicksDisplayed", "visible");
  model->addProperty(majorTicksPath, this, "majorTickLen"       , "length");
  model->addProperty(minorTicksPath, this, "minorTicksDisplayed", "visible");
  model->addProperty(minorTicksPath, this, "minorTickLen"       , "length");

  QString ticksLabelPath = ticksPath + "/label";

  model->addProperty(ticksLabelPath, this, "tickLabelDisplayed", "visible");
  model->addProperty(ticksLabelPath, this, "tickLabelFont"     , "font");
  model->addProperty(ticksLabelPath, this, "tickLabelColor"    , "color");
  model->addProperty(ticksLabelPath, this, "tickLabelAngle"    , "angle");
  model->addProperty(ticksLabelPath, this, "tickLabelAutoHide" , "autoHide");
  model->addProperty(ticksLabelPath, this, "tickLabelPlacement", "placement");

  model->addProperty(ticksPath, this, "tickInside" , "inside");
  model->addProperty(ticksPath, this, "mirrorTicks", "mirror");

  QString labelPath = path + "/label";

  model->addProperty(labelPath, this, "labelDisplayed", "visible");
  model->addProperty(labelPath, this, "label"         , "text"   );
  model->addProperty(labelPath, this, "labelFont"     , "font"   );
  model->addProperty(labelPath, this, "labelColor"    , "color"  );

  QString gridPath          = path + "/grid";
  QString gridLinePath      = gridPath + "/line";
  QString gridMajorLinePath = gridLinePath + "/major";
  QString gridMinorLinePath = gridLinePath + "/minor";
  QString gridFillPath      = gridPath + "/fill";

  model->addProperty(gridPath, this, "gridMid"  , "middle");
  model->addProperty(gridPath, this, "gridAbove", "above" );

  model->addProperty(gridMajorLinePath, this, "gridMajorDisplayed", "visible");
  model->addProperty(gridMajorLinePath, this, "gridMajorColor"    , "color"  );
  model->addProperty(gridMajorLinePath, this, "gridMajorWidth"    , "width"  );
  model->addProperty(gridMajorLinePath, this, "gridMajorDash"     , "dash"   );

  model->addProperty(gridMinorLinePath, this, "gridMinorDisplayed", "visible");
  model->addProperty(gridMinorLinePath, this, "gridMinorColor"    , "color"  );
  model->addProperty(gridMinorLinePath, this, "gridMinorWidth"    , "width"  );
  model->addProperty(gridMinorLinePath, this, "gridMinorDash"     , "dash"   );

  model->addProperty(gridFillPath, this, "gridFill"     , "visible");
  model->addProperty(gridFillPath, this, "gridFillColor", "color"  );
  model->addProperty(gridFillPath, this, "gridFillAlpha", "alpha"  );
}

void
CQChartsAxis::
setRange(double start, double end)
{
  start_ = std::min(start, end);
  end_   = std::max(start, end);

  calc();

  redraw();
}

void
CQChartsAxis::
setMajorIncrement(double i)
{
  CQChartsUtil::testAndSet(majorIncrement_, i, [&]() { calc(); redraw(); } );
}

void
CQChartsAxis::
setTickIncrement(uint i)
{
  CQChartsUtil::testAndSet(tickIncrement_, i, [&]() { calc(); redraw(); } );
}

//---

void
CQChartsAxis::
clearTickLabels()
{
  tickLabels_.clear();
}

void
CQChartsAxis::
setTickLabel(long i, const QString &label)
{
  CQChartsUtil::testAndSet(tickLabels_[i], label, [&]() { redraw(); } );
}

bool
CQChartsAxis::
hasTickLabel(long i) const
{
  return (tickLabels_.find(i) != tickLabels_.end());
}

const QString &
CQChartsAxis::
tickLabel(long i) const
{
  auto p = tickLabels_.find(i);
  assert(p != tickLabels_.end());

  return (*p).second;
}

//---

QString
CQChartsAxis::
format() const
{
  if (formatStr_.length())
    return formatStr_;

  //---

  if (column().isValid()) {
    QString typeStr;

    if (plot_->columnTypeStr(column(), typeStr))
      return "";

    return typeStr;
  }

  //---

  return "";
}

bool
CQChartsAxis::
setFormat(const QString &typeStr)
{
  formatStr_ = typeStr;

  //---

  if (column().isValid()) {
    if (! plot_->setColumnTypeStr(column(), typeStr))
      return false;
  }

  return true;
}

//---

bool
CQChartsAxis::
isLabelDisplayed() const
{
  return labelDisplayed_;
}

void
CQChartsAxis::
setLabelDisplayed(bool b)
{
  CQChartsUtil::testAndSet(labelDisplayed_, b, [&]() { redraw(); } );
}

const QString &
CQChartsAxis::
label() const
{
  return label_->textStr();
}

void
CQChartsAxis::
setLabel(const QString &str)
{
  label_->setTextStr(str);

  redraw();
}

const QFont &
CQChartsAxis::
labelFont() const
{
  return label_->textFont();
}

void
CQChartsAxis::
setLabelFont(const QFont &font)
{
  label_->setTextFont(font);

  redraw();
}

const CQChartsColor &
CQChartsAxis::
labelColor() const
{
  return label_->textColor();
}

void
CQChartsAxis::
setLabelColor(const CQChartsColor &c)
{
  label_->setTextColor(c);
}

QColor
CQChartsAxis::
interpLabelColor(int i, int n) const
{
  return label_->interpTextColor(i, n);
}

//---

bool
CQChartsAxis::
isLineDisplayed() const
{
  return lineData_.visible;
}

void
CQChartsAxis::
setLineDisplayed(bool b)
{
  CQChartsUtil::testAndSet(lineData_.visible, b, [&]() { redraw(); } );
}

const CQChartsLength &
CQChartsAxis::
lineWidth() const
{
  return lineData_.width;
}

void
CQChartsAxis::
setLineWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(lineData_.width, l, [&]() { redraw(); } );
}

const CQChartsLineDash &
CQChartsAxis::
lineDash() const
{
  return lineData_.dash;
}

void
CQChartsAxis::
setLineDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(lineData_.dash, d, [&]() { redraw(); } );
}

const CQChartsColor &
CQChartsAxis::
lineColor() const
{
  return lineData_.color;
}

void
CQChartsAxis::
setLineColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(lineData_.color, c, [&]() { redraw(); } );
}

QColor
CQChartsAxis::
interpLineColor(int i, int n) const
{
  return lineColor().interpColor(plot_, i, n);
}

//---

bool
CQChartsAxis::
isGridMajorDisplayed() const
{
  return majorGridLineData_.visible;
}

void
CQChartsAxis::
setGridMajorDisplayed(bool b)
{
  CQChartsUtil::testAndSet(majorGridLineData_.visible, b, [&]() { redraw(); } );
}

const CQChartsColor &
CQChartsAxis::
gridMajorColor() const
{
  return majorGridLineData_.color;
}

void
CQChartsAxis::
setGridMajorColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(majorGridLineData_.color, c, [&]() { redraw(); } );
}

QColor
CQChartsAxis::
interpGridMajorColor(int i, int n) const
{
  return gridMajorColor().interpColor(plot_, i, n);
}

const CQChartsLength &
CQChartsAxis::
gridMajorWidth() const
{
  return majorGridLineData_.width;
}

void
CQChartsAxis::
setGridMajorWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(majorGridLineData_.width, l, [&]() { redraw(); } );
}

const CQChartsLineDash &
CQChartsAxis::
gridMajorDash() const
{
  return majorGridLineData_.dash;
}

void
CQChartsAxis::
setGridMajorDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(majorGridLineData_.dash, d, [&]() { redraw(); } );
}

bool
CQChartsAxis::
isGridMinorDisplayed() const
{
  return minorGridLineData_.visible;
}

void
CQChartsAxis::
setGridMinorDisplayed(bool b)
{
  CQChartsUtil::testAndSet(minorGridLineData_.visible, b, [&]() { redraw(); } );
}

const CQChartsColor &
CQChartsAxis::
gridMinorColor() const
{
  return minorGridLineData_.color;
}

void
CQChartsAxis::
setGridMinorColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(minorGridLineData_.color, c, [&]() { redraw(); } );
}

const CQChartsLength &
CQChartsAxis::
gridMinorWidth() const
{
  return minorGridLineData_.width;
}

void
CQChartsAxis::
setGridMinorWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(minorGridLineData_.width, l, [&]() { redraw(); } );
}

const CQChartsLineDash &
CQChartsAxis::
gridMinorDash() const
{
  return minorGridLineData_.dash;
}

void
CQChartsAxis::
setGridMinorDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(minorGridLineData_.dash, d, [&]() { redraw(); } );
}

bool
CQChartsAxis::
isGridFill() const
{
  return gridFill_.visible;
}

void
CQChartsAxis::
setGridFill(bool b)
{
  CQChartsUtil::testAndSet(gridFill_.visible, b, [&]() { redraw(); } );
}

double
CQChartsAxis::
gridFillAlpha() const
{
  return gridFill_.alpha;
}

void
CQChartsAxis::
setGridFillAlpha(double a)
{
  CQChartsUtil::testAndSet(gridFill_.alpha, a, [&]() { redraw(); } );
}

const CQChartsColor &
CQChartsAxis::
gridFillColor() const
{
  return gridFill_.color;
}

void
CQChartsAxis::
setGridFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(gridFill_.color, c, [&]() { redraw(); } );
}

QColor
CQChartsAxis::
interpGridFillColor(int i, int n) const
{
  return gridFillColor().interpColor(plot_, i, n);
}

//---

void
CQChartsAxis::
setTickSpaces(double *tickSpaces, uint numTickSpaces)
{
  tickSpaces_.resize(numTickSpaces);

  memcpy(&tickSpaces_[0], tickSpaces, numTickSpaces*sizeof(double));
}

//---

bool
CQChartsAxis::
isTickLabelDisplayed() const
{
  return tickLabelDisplayed_;
}

void
CQChartsAxis::
setTickLabelDisplayed(bool b)
{
  CQChartsUtil::testAndSet(tickLabelDisplayed_, b, [&]() { redraw(); } );
}

const QFont &
CQChartsAxis::
tickLabelFont() const
{
  return tickLabel_->textFont();
}

void
CQChartsAxis::
setTickLabelFont(const QFont &font)
{
  tickLabel_->setTextFont(font);

  redraw();
}

double
CQChartsAxis::
tickLabelAngle() const
{
  return tickLabel_->textAngle();
}

void
CQChartsAxis::
setTickLabelAngle(double angle)
{
  tickLabel_->setTextAngle(angle);

  redraw();
}

const CQChartsColor &
CQChartsAxis::
tickLabelColor() const
{
  return tickLabel_->textColor();
}

void
CQChartsAxis::
setTickLabelColor(const CQChartsColor &c)
{
  return tickLabel_->setTextColor(c);
}

QColor
CQChartsAxis::
interpTickLabelColor(int i, int n) const
{
  return tickLabel_->interpTextColor(i, n);
}

//---

void
CQChartsAxis::
setIntegral(bool b)
{
  CQChartsUtil::testAndSet(integral_, b, [&]() { calc(); redraw(); } );
}

void
CQChartsAxis::
setLog(bool b)
{
  CQChartsUtil::testAndSet(log_, b, [&]() { calc(); redraw(); } );
}

//---

void
CQChartsAxis::
calc()
{
#if 1
  interval_.setStart(start());
  interval_.setEnd  (end  ());

  interval_.setIntegral(isIntegral());

  interval_.setMajorIncrement(majorIncrement());
  interval_.setTickIncrement (tickIncrement ());

  numMajorTicks_ = interval_.calcNumMajor ();
  numMinorTicks_ = interval_.calcNumMinor ();
  calcIncrement_ = interval_.calcIncrement();
  calcStart_     = interval_.calcStart    ();
  calcEnd_       = interval_.calcEnd      ();
#else
  numMajorTicks_ = 1;
  numMinorTicks_ = 0;

  //------

  // Ensure start and end are in the correct order
  double minAxis = std::min(start(), end());
  double maxAxis = std::max(start(), end());

  if (isIntegral()) {
    minAxis = std::floor(minAxis);
    maxAxis = std::ceil (maxAxis);
  }

  //------

  // Calculate length
  double length = std::abs(maxAxis - minAxis);

  if (length == 0.0)
    return;

  if (isIntegral())
    length = std::ceil(length);

  //------

  // Calculate nearest power of ten to length
  int power = CMathRound::RoundDown(log10(length));

  if (isIntegral()) {
    if (power < 0)
      power = 1;
  }

  //------

  if (majorIncrement_ <= 0.0) {
    // Set Default Increment to 0.1 * Power of Ten
    if (! isIntegral() && ! isLog()) {
      calcIncrement_ = 0.1;

      if      (power < 0) {
        for (int i = 0; i < -power; i++)
          calcIncrement_ /= 10.0;
      }
      else if (power > 0) {
        for (int i = 0; i <  power; i++)
          calcIncrement_ *= 10.0;
      }
    }
    else {
      calcIncrement_ = 1;

      for (int i = 1; i < power; i++)
        calcIncrement_ *= 10.0;
    }

    //------

    // Calculate other test Increments

    for (uint i = 0; i < numAxesIncrementTests; i++) {
      if (isIntegral() && ! CMathUtil::isInteger(axesIncrementTests[i].factor)) {
        axesIncrementTests[i].incFactor = 0.0;
        continue;
      }

      axesIncrementTests[i].incFactor = calcIncrement()*axesIncrementTests[i].factor;
    }

    //------

    // Test each Increment in turn
    // (Set Default Start/End to Force Update)

    AxisGapData axisGapData;

    for (uint i = 0; i < numAxesIncrementTests; i++) {
      if (isLog() && ! axesIncrementTests[i].isLog)
        continue;

      if (axesIncrementTests[i].incFactor <= 0)
        continue;

      if (tickIncrement_ > 0) {
        if (! CMathUtil::isInteger(axesIncrementTests[i].incFactor))
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

    calcStart_     = axisGapData.start;
    calcEnd_       = axisGapData.end;
    calcIncrement_ = axisGapData.increment;

    int numGapTicks = axisGapData.numGapTicks;

    if (isLog())
      numGapTicks = 10;

    //------

    // Set the Gap Positions

    numMajorTicks_ = CMathRound::RoundNearest((calcEnd() - calcStart())/calcIncrement());
    numMinorTicks_ = numGapTicks;
  }
  else {
    calcStart_     = minAxis;
    calcEnd_       = maxAxis;
    calcIncrement_ = majorIncrement_;

    numMajorTicks_ = CMathRound::RoundNearest((calcEnd() - calcStart())/calcIncrement());
    numMinorTicks_ = 5;
  }
#endif
//std::cerr << "numMajorTicks: " << numMajorTicks_  << "\n";
//std::cerr << "numMinorTicks: " << numMinorTicks_  << "\n";
//std::cerr << "calcIncrement: " << calcIncrement() << "\n";
//std::cerr << "calcStart    : " << calcStart()     << "\n";
//std::cerr << "calcEnd      : " << calcEnd()       << "\n";

  emit ticksChanged();
}

#if 0
bool
CQChartsAxis::
testAxisGaps(double start, double end, double testIncrement, uint testNumGapTicks,
             AxisGapData &axisGapData)
{
  // Calculate New Start and End implied by the Test Increment

  double newStart = CMathRound::RoundDownF(start/testIncrement)*testIncrement;
  double newEnd   = CMathRound::RoundUpF  (end  /testIncrement)*testIncrement;

  while (newStart > start)
    newStart -= testIncrement;

  while (newEnd < end)
    newEnd += testIncrement;

  uint testNumGaps = CMathRound::RoundUp((newEnd - newStart)/testIncrement);

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

    double delta1 = std::abs(newStart - start) + std::abs(newEnd - end);

    //------

    // If better fit than current fit or equally good fit and
    // number of gaps is nearer to optimum (axisGoodTicks.opt) then
    // update current

    double delta2 = std::abs(axisGapData.start - start) + std::abs(axisGapData.end - end);

    if (((std::abs(delta1 - delta2) < 1E-6) &&
         (std::abs(int(testNumGaps        ) - int(axisGoodTicks.opt)) <
          std::abs(int(axisGapData.numGaps) - int(axisGoodTicks.opt)))) ||
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

    double delta1 = std::abs(newStart - start) + std::abs(newEnd - end);

    //------

    // If better fit than current fit or equally good fit and
    // number of gaps is nearer to optimum (axisGoodTicks.opt) then
    // update current

    double delta2 = std::abs(axisGapData.start - start) + std::abs(axisGapData.end - end);

    if (((std::abs(delta1 - delta2) < 1E-6) &&
         (std::abs(int(testNumGaps        ) - int(axisGoodTicks.opt)) <
          std::abs(int(axisGapData.numGaps) - int(axisGoodTicks.opt)))) ||
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
#endif

double
CQChartsAxis::
minorIncrement() const
{
  if (numMajorTicks() > 0 && numMinorTicks() > 0)
    return (calcEnd() - calcStart())/(numMajorTicks()*numMinorTicks());

  return 0.0;
}

QString
CQChartsAxis::
valueStr(double pos) const
{
  return valueStr(plot_, pos);
}

QString
CQChartsAxis::
valueStr(CQChartsPlot *plot, double pos) const
{
  if (isLog())
    pos = plot->expValue(pos);

  if (isIntegral()) {
    long ipos = long(pos);

    if (hasTickLabel(ipos))
      return tickLabel(ipos);

    if (isRequireTickLabel())
      return "";
  }

  if (formatStr_.length()) {
    QString str;

    if (CQChartsUtil::formatColumnTypeValue(plot->charts(), formatStr_, pos, str))
      return str;
  }

  if (column().isValid()) {
    QString str;

    if (CQChartsUtil::formatColumnValue(plot->charts(), plot->model().data(), column(), pos, str))
      return str;

    if (isDataLabels()) {
      int row = int(pos);

      QModelIndex parent; // TODO: support parent

      bool ok;

      QVariant header = plot->modelValue(row, column(), parent, ok);

      if (header.isValid()) {
        QString headerStr;

        CQChartsVariant::toString(header, headerStr);

        return headerStr;
      }
    }
  }

  if (isIntegral())
    return CQChartsUtil::toString(long(pos));

  return CQChartsUtil::toString(pos);
}

void
CQChartsAxis::
updatePlotPosition()
{
  plot_->updateMargin();
}

bool
CQChartsAxis::
contains(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  return bbox().inside(p);
}

void
CQChartsAxis::
redraw()
{
  plot_->invalidateLayer(CQChartsLayer::Type::BG_AXES);
  plot_->invalidateLayer(CQChartsLayer::Type::FG_AXES);
}

void
CQChartsAxis::
updatePlotRange()
{
  plot_->updateRange();
}

//---

bool
CQChartsAxis::
editPress(const CQChartsGeom::Point &p)
{
  editHandles_.setDragPos(p);

  double apos1, apos2;

  calcPos(plot_, apos1, apos2);

  pos_ = apos1;

  return true;
}

bool
CQChartsAxis::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point &dragPos = editHandles_.dragPos();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if (direction_ == Direction::HORIZONTAL)
    pos_ = *pos_ + dy;
  else
    pos_ = *pos_ + dx;

  editHandles_.setDragPos(p);

  redraw();

  return true;
}

bool
CQChartsAxis::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_.selectInside(p);
}

bool
CQChartsAxis::
editRelease(const CQChartsGeom::Point &)
{
  return true;
}

void
CQChartsAxis::
editMoveBy(const QPointF &d)
{
  double apos1, apos2;

  calcPos(plot_, apos1, apos2);

  if (direction_ == Direction::HORIZONTAL)
    pos_ = apos1 + d.y();
  else
    pos_ = apos1 + d.x();

  redraw();
}

//---

bool
CQChartsAxis::
isDrawGrid() const
{
  return (isGridMajorDisplayed() || isGridMinorDisplayed() || isGridFill());
}

void
CQChartsAxis::
drawGrid(CQChartsPlot *plot, QPainter *painter)
{
  if (! isDrawGrid())
    return;

  //---

  CQChartsGeom::BBox dataRange = plot->calcDataRange();

  double amin, amax, dmin, dmax;

  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    amin = start();
    amax = end  ();

    dmin = dataRange.getYMin();
    dmax = dataRange.getYMax();

    plot->windowToPixel(amin, dmin, ax1, ay1);
    plot->windowToPixel(amax, dmax, ax2, ay2);
  }
  else {
    amin = start();
    amax = end  ();

    dmin = dataRange.getXMin();
    dmax = dataRange.getXMax();

    plot->windowToPixel(dmin, amin, ax1, ay1);
    plot->windowToPixel(dmax, amax, ax2, ay2);
  }

  //---

  painter->save();

  //---

  double inc  = calcIncrement();
  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  //---

  // draw fill
  if (isGridFill()) {
    QRectF dataRect = CQChartsUtil::toQRect(plot->calcDataPixelRect());

    painter->setClipRect(dataRect);

    //---

    QColor fillColor = interpGridFillColor(0, 1);

    fillColor.setAlphaF(gridFillAlpha());

    QBrush brush(fillColor);

    //---

    double pos1 = calcStart();

    if (isGridMid())
      pos1 += inc/2.0;

    double pos2 = pos1;

    for (uint i = 0; i < numMajorTicks() + 1; i++) {
      // fill on alternate gaps
      if (i & 1) {
        if (pos2 >= amin || pos1 <= amax) {
          double pos3 = std::max(pos1, amin);
          double pos4 = std::min(pos2, amax);

          double ppx1, ppy1, ppx2, ppy2;

          plot->windowToPixel(pos3, pos1, ppx1, ppy1);
          plot->windowToPixel(pos4, pos2, ppx2, ppy2);

          CQChartsGeom::BBox bbox;

          if (direction_ == Direction::HORIZONTAL)
            bbox = CQChartsGeom::BBox(ppx1, ay1, ppx2, ay2);
          else
            bbox = CQChartsGeom::BBox(ax1, ppy1, ax2, ppy2);

          painter->fillRect(CQChartsUtil::toQRect(bbox), brush);
        }
      }

      //---

      pos1 = pos2;
      pos2 = pos1 + inc;
    }
  }

  //---

  // draw grid lines
  if (isGridMajorDisplayed() || isGridMinorDisplayed()) {
    double pos1 = calcStart();

    if (isGridMid())
      pos1 += inc/2.0;

    for (uint i = 0; i < numMajorTicks() + 1; i++) {
      // draw major line (grid and tick)
      if (pos1 >= amin && pos1 <= amax) {
        // draw major grid line if major or minor displayed
        if      (isGridMajorDisplayed())
          drawMajorGridLine(plot, painter, pos1, dmin, dmax);
        else if (isGridMinorDisplayed())
          drawMinorGridLine(plot, painter, pos1, dmin, dmax);
      }

      if (isGridMinorDisplayed()) {
        for (uint j = 1; j < numMinorTicks(); j++) {
          double pos2 = pos1 + (isLog() ? plot_->logValue(j*inc1) : j*inc1);

          if (isIntegral() && ! CMathUtil::isInteger(pos2))
            continue;

          // draw minor grid line
          if (pos2 >= amin && pos2 <= amax)
            drawMinorGridLine(plot, painter, pos2, dmin, dmax);
        }
      }

      //---

      pos1 += inc;
    }
  }

  //---

  painter->restore();
}

void
CQChartsAxis::
draw(CQChartsPlot *plot, QPainter *painter)
{
  fitBBox_ = CQChartsGeom::BBox();
  bbox_    = CQChartsGeom::BBox();

  //---

  double apos1, apos2;

  calcPos(plot, apos1, apos2);

  double amin, amax;

  if (direction_ == Direction::HORIZONTAL) {
    amin = start();
    amax = end  ();

    bbox_ += CQChartsGeom::Point(amin, apos1);
    bbox_ += CQChartsGeom::Point(amax, apos1);
  }
  else {
    amin = start();
    amax = end  ();

    bbox_ += CQChartsGeom::Point(apos1, amin);
    bbox_ += CQChartsGeom::Point(apos1, amax);
  }

  fitBBox_ = bbox_;

  //---

  painter->save();

  //---

  // axis line
  if (isLineDisplayed()) {
    drawLine(plot, painter, apos1, amin, amax);
  }

  //---

  double inc  = calcIncrement();
  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  //---

  double pos1 = calcStart();

  int tlen2 = majorTickLen();
  int tgap  = 2;

  //---

  lbbox_ = CQChartsGeom::BBox();

  //---

  lastTickLabelRect_ = CQChartsGeom::BBox();

  double dt = (tickLabelPlacement() == TickLabelPlacement::BETWEEN ? -0.5 : 0.0);

  for (uint i = 0; i < numMajorTicks() + 1; i++) {
    double pos2 = pos1 + dt;

    // draw major line (grid and tick)
    if (pos2 >= amin && pos2 <= amax) {
      // draw major tick (or minor tick if major ticks off and minor ones on)
      if      (isMajorTicksDisplayed()) {
        drawMajorTickLine(plot, painter, apos1, pos1, isTickInside());

        if (isMirrorTicks())
          drawMajorTickLine(plot, painter, apos2, pos1, ! isTickInside());
      }
      else if (isMinorTicksDisplayed()) {
        drawMinorTickLine(plot, painter, apos1, pos1, isTickInside());

        if (isMirrorTicks())
          drawMinorTickLine(plot, painter, apos2, pos1, ! isTickInside());
      }
    }

    // draw minor tick lines (grid and tick)
    if (isMinorTicksDisplayed() && i < numMajorTicks()) {
      for (uint j = 1; j < numMinorTicks(); j++) {
        double pos2 = pos1 + (isLog() ? plot->logValue(j*inc1) : j*inc1);

        if (isIntegral() && ! CMathUtil::isInteger(pos2))
          continue;

        // draw minor tick line
        if (pos2 >= amin && pos2 <= amax) {
          drawMinorTickLine(plot, painter, apos1, pos2, isTickInside());

          if (isMirrorTicks())
            drawMinorTickLine(plot, painter, apos2, pos2, ! isTickInside());
        }
      }
    }

    //---

    if (isTickLabelDisplayed()) {
      // draw major tick label
      if (pos1 >= amin && pos1 <= amax) {
        drawTickLabel(plot, painter, apos1, pos1, isTickInside());
      }
    }

    //---

    pos1 += inc;
  }

  //---

  // fix range if not set
  if (! lbbox_.isSet()) {
    if (direction_ == Direction::HORIZONTAL) {
      double ax1, ay1, ax2, ay2;

      plot->windowToPixel(amin, apos1, ax1, ay1);
      plot->windowToPixel(amax, apos1, ax2, ay2);

      bool isPixelBottom = (side() == Side::BOTTOM_LEFT && ! plot->isInvertY()) ||
                           (side() == Side::TOP_RIGHT   &&   plot->isInvertY());

      double dys = (isPixelBottom ? 1 : -1);

      ay2 += dys*(tlen2 + tgap);

      lbbox_ += CQChartsGeom::Point(ax1, ay1);
      lbbox_ += CQChartsGeom::Point(ax2, ay2);
    }
    else {
      double ax1, ay1, ax2, ay2;

      plot->windowToPixel(apos1, amin, ax1, ay1);
      plot->windowToPixel(apos1, amax, ax2, ay2);

      bool isPixelLeft = (side() == Side::BOTTOM_LEFT && ! plot->isInvertX()) ||
                         (side() == Side::TOP_RIGHT   &&   plot->isInvertX());

      double dxs = (isPixelLeft ? 1 : -1);

      ax2 += dxs*(tlen2 + tgap);

      lbbox_ += CQChartsGeom::Point(ax1, ay1);
      lbbox_ += CQChartsGeom::Point(ax2, ay2);
    }
  }

  //---

  if (isLabelDisplayed()) {
    QString text = label();

    drawAxisLabel(plot, painter, apos1, amin, amax, text);
  }

  //---

  if (plot->showBoxes()) {
    plot->drawWindowColorBox(painter, bbox_, Qt::blue);

    plot->drawColorBox(painter, lbbox_, Qt::green);
  }

  //---

  painter->restore();
}

void
CQChartsAxis::
drawEditHandles(QPainter *painter)
{
  assert(plot_->view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  editHandles_.setBBox(this->bbox());

  editHandles_.draw(painter);
}

void
CQChartsAxis::
calcPos(CQChartsPlot *plot, double &apos1, double &apos2) const
{
  if (hasPosition()) {
    apos1 = *pos_;
    apos2 = apos1;
    return;
  }

  //---

  CQChartsGeom::BBox dataRange = plot->calcDataRange();

  if (dataRange.isSet())
    dataRange += plot->annotationBBox();
  else
    dataRange = CQChartsGeom::BBox(0, 0, 1, 1);

  //---

  if (direction_ == Direction::HORIZONTAL) {
    bool isWindowBottom = (side() == Side::BOTTOM_LEFT);
    //bool isPixelBottom = (side() == Side::BOTTOM_LEFT && ! plot->isInvertY()) ||
    //                     (side() == Side::TOP_RIGHT   &&   plot->isInvertY());

    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    apos1 = (isWindowBottom ? ymin : ymax);
    apos2 = (isWindowBottom ? ymax : ymin);
  }
  else {
    bool isWindowLeft = (side() == Side::BOTTOM_LEFT);
    //bool isPixelLeft = (side() == Side::BOTTOM_LEFT && ! plot->isInvertX()) ||
    //                   (side() == Side::TOP_RIGHT   &&   plot->isInvertX());

    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    apos1 = (isWindowLeft ? xmin : xmax);
    apos2 = (isWindowLeft ? xmax : xmin);
  }
}

void
CQChartsAxis::
drawLine(CQChartsPlot *plot, QPainter *painter, double apos, double amin, double amax)
{
  painter->setPen(interpLineColor(0, 1));

  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    plot->windowToPixel(amin, apos, ax1, ay1);
    plot->windowToPixel(amax, apos, ax2, ay2);

    plot->drawLine(painter, QPointF(ax1, ay1), QPointF(ax2, ay1), lineData_);
  }
  else {
    plot->windowToPixel(apos, amin, ax1, ay1);
    plot->windowToPixel(apos, amax, ax2, ay2);

    plot->drawLine(painter, QPointF(ax1, ay1), QPointF(ax1, ay2), lineData_);
  }
}

void
CQChartsAxis::
drawMajorGridLine(CQChartsPlot *plot, QPainter *painter, double apos, double dmin, double dmax)
{
  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    plot->windowToPixel(apos, dmin, ax1, ay1);
    plot->windowToPixel(apos, dmax, ax2, ay2);

    plot->drawLine(painter, QPointF(ax1, ay1), QPointF(ax1, ay2), majorGridLineData_);
  }
  else {
    plot->windowToPixel(dmin, apos, ax1, ay1);
    plot->windowToPixel(dmax, apos, ax2, ay2);

    plot->drawLine(painter, QPointF(ax1, ay1), QPointF(ax2, ay1), majorGridLineData_);
  }
}

void
CQChartsAxis::
drawMinorGridLine(CQChartsPlot *plot, QPainter *painter, double apos, double dmin, double dmax)
{
  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    plot->windowToPixel(apos, dmin, ax1, ay1);
    plot->windowToPixel(apos, dmax, ax2, ay2);

    plot->drawLine(painter, QPointF(ax1, ay1), QPointF(ax1, ay2), minorGridLineData_);
  }
  else {
    plot->windowToPixel(dmin, apos, ax1, ay1);
    plot->windowToPixel(dmax, apos, ax2, ay2);

    plot->drawLine(painter, QPointF(ax1, ay1), QPointF(ax2, ay1), minorGridLineData_);
  }
}

void
CQChartsAxis::
drawMajorTickLine(CQChartsPlot *plot, QPainter *painter, double apos, double tpos, bool inside)
{
  drawTickLine(plot, painter, apos, tpos, inside, /*major*/true);
}

void
CQChartsAxis::
drawMinorTickLine(CQChartsPlot *plot, QPainter *painter, double apos, double tpos, bool inside)
{
  drawTickLine(plot, painter, apos, tpos, inside, /*major*/false);
}

void
CQChartsAxis::
drawTickLine(CQChartsPlot *plot, QPainter *painter, double apos, double tpos,
             bool inside, bool major)
{
  int tlen = (major ? majorTickLen() : minorTickLen());

  double ppx, ppy;

  if (major && tickLabelPlacement() == TickLabelPlacement::BETWEEN) {
    if (direction_ == Direction::HORIZONTAL)
      plot->windowToPixel(tpos - 0.5, apos, ppx, ppy);
    else
      plot->windowToPixel(apos, tpos - 0.5, ppx, ppy);
  }
  else {
    if (direction_ == Direction::HORIZONTAL)
      plot->windowToPixel(tpos, apos, ppx, ppy);
    else
      plot->windowToPixel(apos, tpos, ppx, ppy);
  }

  painter->setPen(interpLineColor(0, 1));

  if (direction_ == Direction::HORIZONTAL) {
    bool isWindowBottom = (side() == Side::BOTTOM_LEFT);
    bool isPixelBottom  = (side() == Side::BOTTOM_LEFT && ! plot->isInvertY()) ||
                          (side() == Side::TOP_RIGHT   &&   plot->isInvertY());

    int pys = (isPixelBottom ? 1 : -1);
    int dt1 = pys*tlen;

    double adt1 = plot->pixelToWindowHeight(dt1);

    if (inside)
      painter->drawLine(QPointF(ppx, ppy), QPointF(ppx, ppy - dt1));
    else {
      painter->drawLine(QPointF(ppx, ppy), QPointF(ppx, ppy + dt1));

      CQChartsGeom::Point p;

      if (isWindowBottom)
        p = CQChartsGeom::Point(tpos, apos - adt1);
      else
        p = CQChartsGeom::Point(tpos, apos + adt1);

      bbox_    += p;
      fitBBox_ += p;
    }
  }
  else {
    bool isWindowLeft = (side() == Side::BOTTOM_LEFT);
    bool isPixelLeft  = (side() == Side::BOTTOM_LEFT && ! plot->isInvertX()) ||
                        (side() == Side::TOP_RIGHT   &&   plot->isInvertX());

    int pxs = (isPixelLeft ? -1 : 1);
    int dt1 = pxs*tlen;

    double adt1 = plot->pixelToWindowWidth(dt1);

    if (inside)
      painter->drawLine(QPointF(ppx, ppy), QPointF(ppx - dt1, ppy));
    else {
      painter->drawLine(QPointF(ppx, ppy), QPointF(ppx + dt1, ppy));

      CQChartsGeom::Point p;

      if (isWindowLeft)
        p = CQChartsGeom::Point(apos - adt1, tpos);
      else
        p = CQChartsGeom::Point(apos + adt1, tpos);

      bbox_    += p;
      fitBBox_ += p;
    }
  }
}

void
CQChartsAxis::
drawTickLabel(CQChartsPlot *plot, QPainter *painter, double apos, double tpos, bool inside)
{
  int tgap  = 2;
  int tlen1 = majorTickLen();
  int tlen2 = minorTickLen();

  double ppx, ppy;

  if (direction_ == Direction::HORIZONTAL)
    plot->windowToPixel(tpos, apos, ppx, ppy);
  else
    plot->windowToPixel(apos, tpos, ppx, ppy);

  QString text = valueStr(plot, tpos);

  if (! text.length())
    return;

  painter->setPen(interpTickLabelColor(0, 1));

  painter->setFont(tickLabelFont());

  QFontMetricsF fm(painter->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  double angle = tickLabelAngle();

  if (direction_ == Direction::HORIZONTAL) {
    bool isPixelBottom = (side() == Side::BOTTOM_LEFT && ! plot->isInvertY()) ||
                         (side() == Side::TOP_RIGHT   &&   plot->isInvertY());

    double tyo = 0.0;

    if (tickLabelPlacement() == TickLabelPlacement::MIDDLE) {
      if (inside)
        tyo = tgap;
      else
        tyo = tlen1 + tgap;
    }
    else {
      if (inside)
        tyo = tgap;
      else
        tyo = tlen2 + tgap;
     }

    //---

    CQChartsGeom::BBox tbbox;

    bool visible = true;

    if (isPixelBottom) {
      Qt::Alignment align = Qt::AlignHCenter;

/*
      if (! plot->isInvertY())
        align |= Qt::AlignTop;
      else
        align |= Qt::AlignBottom;
*/
      align |= Qt::AlignTop;

      QPointF pt(ppx, ppy + tyo);

      if (CMathUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE) {
          if (inside)
            atm = plot->pixelToWindowHeight(tgap);
          else
            atm = plot->pixelToWindowHeight(tlen1 + tgap);
        }
        else {
          if (inside)
            atm = plot->pixelToWindowHeight(tgap);
          else
            atm = plot->pixelToWindowHeight(tlen2 + tgap);
        }

        lbbox_ += CQChartsGeom::Point(pt.x(), pt.y()          );
        lbbox_ += CQChartsGeom::Point(pt.x(), pt.y() + ta + td);

        double xpos = 0.0;
        double ypos = apos - wth - atm;

        if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE)
          xpos = tpos - atw/2;
        else if (tickLabelPlacement() == TickLabelPlacement::BOTTOM_LEFT)
          xpos = tpos - atw;
        else if (tickLabelPlacement() == TickLabelPlacement::TOP_RIGHT)
          xpos = tpos;
        else if (tickLabelPlacement() == TickLabelPlacement::BETWEEN)
          xpos = tpos - 0.5;

        tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CMathUtil::isZero(angle)) {
          double ty = pt.y() + ta;

          QPointF p;

          if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE)
            p = QPointF(pt.x() - tw/2                         , ty);
          else if (tickLabelPlacement() == TickLabelPlacement::BOTTOM_LEFT)
            p = QPointF(pt.x() - tw                           , ty);
          else if (tickLabelPlacement() == TickLabelPlacement::TOP_RIGHT)
            p = QPointF(pt.x()                                , ty);
          else if (tickLabelPlacement() == TickLabelPlacement::BETWEEN)
            p = QPointF(pt.x() - plot->windowToPixelWidth(0.5), ty);

          painter->drawText(p, text);
        }
        else {
          CQChartsRotatedText::drawRotatedText(painter, pt.x(), pt.y(), text,
                                               angle, align, /*alignBox*/true);
        }

        if (plot->showBoxes())
          plot->drawWindowColorBox(painter, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }
    else {
      Qt::Alignment align = Qt::AlignHCenter;

/*
      if (! plot->isInvertY())
        align |= Qt::AlignBottom;
      else
        align |= Qt::AlignTop;
*/
      align |= Qt::AlignBottom;

      QPointF pt(ppx, ppy - tyo);

      if (CMathUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE) {
          if (inside)
            atm = plot->pixelToWindowHeight(tgap);
          else
            atm = plot->pixelToWindowHeight(tlen1 + tgap);
        }
        else {
          if (inside)
            atm = plot->pixelToWindowHeight(tgap);
          else
            atm = plot->pixelToWindowHeight(tlen2 + tgap);
        }

        lbbox_ += CQChartsGeom::Point(pt.x(), pt.y()            );
        lbbox_ += CQChartsGeom::Point(pt.x(), pt.y() - (ta + td));

        double xpos = 0.0;
        double ypos = apos + atm;

        if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE)
          xpos = tpos - atw/2;
        else if (tickLabelPlacement() == TickLabelPlacement::BOTTOM_LEFT)
          xpos = tpos - atw;
        else if (tickLabelPlacement() == TickLabelPlacement::TOP_RIGHT)
          xpos = tpos;
        else if (tickLabelPlacement() == TickLabelPlacement::BETWEEN)
          xpos = tpos - 0.5;

        tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CMathUtil::isZero(angle)) {
          double ty = pt.y() - td;

          QPointF p;

          if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE)
            p = QPointF(pt.x() - tw/2                         , ty);
          else if (tickLabelPlacement() == TickLabelPlacement::BOTTOM_LEFT)
            p = QPointF(pt.x() - tw                           , ty);
          else if (tickLabelPlacement() == TickLabelPlacement::TOP_RIGHT)
            p = QPointF(pt.x()                                , ty);
          else if (tickLabelPlacement() == TickLabelPlacement::BETWEEN)
            p = QPointF(pt.x() - plot->windowToPixelWidth(0.5), ty);

          painter->drawText(p, text);
        }
        else {
          CQChartsRotatedText::drawRotatedText(painter, pt.x(), pt.y(), text,
                                               angle, align, /*alignBox*/true);
        }

        if (plot->showBoxes())
          plot->drawWindowColorBox(painter, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }

    bbox_    += tbbox;
    fitBBox_ += tbbox;
  }
  else {
    bool isPixelLeft = (side() == Side::BOTTOM_LEFT && ! plot->isInvertX()) ||
                       (side() == Side::TOP_RIGHT   &&   plot->isInvertX());

    double txo = 0.0;

    if (tickLabelPlacement() == TickLabelPlacement::MIDDLE) {
      if (inside)
        txo = tgap;
      else
        txo = tlen1 + tgap;
    }
    else {
      if (inside)
        txo = tgap;
      else
        txo = tlen2 + tgap;
    }

    //---

    CQChartsGeom::BBox tbbox;

    bool visible = true;

    if (isPixelLeft) {
      Qt::Alignment align = Qt::AlignVCenter;

/*
      if (! plot->isInvertX())
        align |= Qt::AlignRight;
      else
        align |= Qt::AlignLeft;
*/
      align |= Qt::AlignRight;

      QPointF pt(ppx - txo, ppy);

      if (CMathUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE) {
          if (inside)
            atm = plot->pixelToWindowWidth(tgap);
          else
            atm = plot->pixelToWindowWidth(tlen1 + tgap);
        }
        else {
          if (inside)
            atm = plot->pixelToWindowWidth(tgap);
          else
            atm = plot->pixelToWindowWidth(tlen2 + tgap);
        }

        lbbox_ += CQChartsGeom::Point(pt.x()     , pt.y());
        lbbox_ += CQChartsGeom::Point(pt.x() - tw, pt.y());

        double xpos = apos - atw - atm;
        double ypos = 0.0;

        if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE)
          ypos = tpos - wth/2;
        else if (tickLabelPlacement() == TickLabelPlacement::BOTTOM_LEFT)
          ypos = tpos - wth;
        else if (tickLabelPlacement() == TickLabelPlacement::TOP_RIGHT)
          ypos = tpos;
        else if (tickLabelPlacement() == TickLabelPlacement::BETWEEN)
          ypos = tpos - 0.5 - wta;

        tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CMathUtil::isZero(angle)) {
          //double tx = pt.x() - (isPixelLeft ? tw : 0.0);
          double tx = pt.x() - tw;

          QPointF p;

          if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE)
            p = QPointF(tx, pt.y() + ta/2);
          else if (tickLabelPlacement() == TickLabelPlacement::BOTTOM_LEFT)
            p = QPointF(tx, pt.y() + ta  );
          else if (tickLabelPlacement() == TickLabelPlacement::TOP_RIGHT)
            p = QPointF(tx, pt.y() - td  );
          else if (tickLabelPlacement() == TickLabelPlacement::BETWEEN)
            p = QPointF(tx, pt.y() - plot->windowToPixelHeight(0.5) + ta);

          painter->drawText(p, text);
        }
        else {
          CQChartsRotatedText::drawRotatedText(painter, pt.x(), pt.y(), text,
                                               angle, align, /*alignBox*/true);
        }

        if (plot->showBoxes())
          plot->drawWindowColorBox(painter, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }
    else {
      Qt::Alignment align = Qt::AlignVCenter;

/*
      if (! isPixelLeft)
        align |= Qt::AlignLeft;
      else
        align |= Qt::AlignRight;
*/
      align |= Qt::AlignLeft;

      QPointF pt(ppx + txo, ppy);

      if (CMathUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE) {
          if (inside)
            atm = plot->pixelToWindowWidth(tgap);
          else
            atm = plot->pixelToWindowWidth(tlen1 + tgap);
        }
        else {
          if (inside)
            atm = plot->pixelToWindowWidth(tgap);
          else
            atm = plot->pixelToWindowWidth(tlen2 + tgap);
        }

        lbbox_ += CQChartsGeom::Point(pt.x()     , pt.y());
        lbbox_ += CQChartsGeom::Point(pt.x() + tw, pt.y());

        double xpos = apos + atm;
        double ypos = 0.0;

        if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE)
          ypos = tpos - wth/2;
        else if (tickLabelPlacement() == TickLabelPlacement::BOTTOM_LEFT)
          ypos = tpos - wth;
        else if (tickLabelPlacement() == TickLabelPlacement::TOP_RIGHT)
          ypos = tpos;
        else if (tickLabelPlacement() == TickLabelPlacement::BETWEEN)
          ypos = tpos - 0.5 - wta;

        tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CMathUtil::isZero(angle)) {
          //double tx = pt.x() - (! isPixelLeft ? 0.0 : tw);
          double tx = pt.x();

          QPointF p;

          if      (tickLabelPlacement() == TickLabelPlacement::MIDDLE)
            p = QPointF(tx, pt.y() + ta/2);
          else if (tickLabelPlacement() == TickLabelPlacement::BOTTOM_LEFT)
            p = QPointF(tx, pt.y() + ta  );
          else if (tickLabelPlacement() == TickLabelPlacement::TOP_RIGHT)
            p = QPointF(tx, pt.y() - td  );
          else if (tickLabelPlacement() == TickLabelPlacement::BETWEEN)
            p = QPointF(tx, pt.y() - plot->windowToPixelHeight(0.5) + ta);

          painter->drawText(p, text);
        }
        else {
          CQChartsRotatedText::drawRotatedText(painter, pt.x(), pt.y(), text,
                                               angle, align, /*alignBox*/true);
        }

        if (plot->showBoxes())
          plot->drawWindowColorBox(painter, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }

    bbox_    += tbbox;
    fitBBox_ += tbbox;
  }
}

void
CQChartsAxis::
drawAxisLabel(CQChartsPlot *plot, QPainter *painter, double apos,
              double amin, double amax, const QString &text)
{
  if (! text.length())
    return;

  //---

  int tgap = 2;

  double ax1, ay1, ax2, ay2, ax3, ay3;

  if (direction_ == Direction::HORIZONTAL) {
    plot->windowToPixel(amin, apos, ax1, ay1);
    plot->windowToPixel(amax, apos, ax2, ay2);
    plot->windowToPixel(amin, apos, ax3, ay3);
  }
  else {
    plot->windowToPixel(apos, amin, ax1, ay1);
    plot->windowToPixel(apos, amax, ax2, ay2);
    plot->windowToPixel(apos, amin, ax3, ay3);
  }

  //---

  painter->setPen(interpLabelColor(0, 1));

  painter->setFont(labelFont());

  QFontMetricsF fm(painter->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  CQChartsGeom::BBox bbox;

  // draw label
  if (direction_ == Direction::HORIZONTAL) {
    double wfh = plot->pixelToWindowHeight(ta + td);

    double axm = (ax1 + ax2)/2 - tw/2;

    bool isPixelBottom = (side() == Side::BOTTOM_LEFT && ! plot->isInvertY()) ||
                         (side() == Side::TOP_RIGHT   &&   plot->isInvertY());

    //int pys = (isPixelBottom ? 1 : -1);

    double ath;
    double atw = plot->pixelToWindowWidth(tw/2);

    if (isPixelBottom) {
      ath = plot->pixelToWindowHeight((lbbox_.getYMax() - ay3) + tgap) + wfh;

      painter->drawText(QPointF(axm, lbbox_.getYMax() + ta + tgap), text);
    }
    else {
      ath = plot->pixelToWindowHeight((ay3 - lbbox_.getYMin()) + tgap) + wfh;

      painter->drawText(QPointF(axm, lbbox_.getYMin() - td - tgap), text);
    }

    bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos - (ath      ));
    bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos - (ath - wfh));

    fitBBox_ += CQChartsGeom::Point((amin + amax)/2, apos - (ath      ));
    fitBBox_ += CQChartsGeom::Point((amin + amax)/2, apos - (ath - wfh));
  }
  else {
    double wfa = plot->pixelToWindowWidth(ta);
    double wfd = plot->pixelToWindowWidth(td);
    double wfh = wfa + wfd;

    bool isPixelLeft = (side() == Side::BOTTOM_LEFT && ! plot->isInvertX()) ||
                       (side() == Side::TOP_RIGHT   &&   plot->isInvertX());

    //int pxs = (isPixelLeft ? 1 : -1);

    double atw;
    double ath = plot->pixelToWindowHeight(tw/2);

    if (isPixelLeft) {
      double aym = (ay2 + ay1)/2 + tw/2;

      atw = plot->pixelToWindowWidth((ax3 - lbbox_.getXMin()) + tgap) + wfh;

      double tx = lbbox_.getXMin() - tgap - td;

      CQChartsRotatedText::drawRotatedText(painter, tx, aym, text, 90.0);
    }
    else {
      double aym = (ay2 + ay1)/2 - tw/2;

      atw = plot->pixelToWindowWidth((lbbox_.getXMax() - ax3) + tgap) + wfh;

      double tx = lbbox_.getXMax() + tgap + td;

      CQChartsRotatedText::drawRotatedText(painter, tx, aym, text, -90.0);
    }

    bbox += CQChartsGeom::Point(apos - (atw      ), (amin + amax)/2 - ath);
    bbox += CQChartsGeom::Point(apos - (atw - wfh), (amin + amax)/2 + ath);

    fitBBox_ += CQChartsGeom::Point(apos - (atw      ), (amin + amax)/2);
    fitBBox_ += CQChartsGeom::Point(apos - (atw - wfh), (amin + amax)/2);
  }

  if (plot->showBoxes())
    plot->drawWindowColorBox(painter, bbox);

  bbox_ += bbox;
}

CQChartsGeom::BBox
CQChartsAxis::
fitBBox() const
{
  return fitBBox_;
}

//------

CQChartsAxisLabel::
CQChartsAxisLabel(CQChartsAxis *axis) :
 CQChartsTextBoxObj(axis->plot()), axis_(axis)
{
}

void
CQChartsAxisLabel::
redrawBoxObj()
{
  axis_->redraw();
}

//------

CQChartsAxisTickLabel::
CQChartsAxisTickLabel(CQChartsAxis *axis) :
 CQChartsTextBoxObj(axis->plot()), axis_(axis)
{
}

void
CQChartsAxisTickLabel::
redrawBoxObj()
{
  axis_->redraw();
}
