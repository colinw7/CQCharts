#include <CQChartsAxis.h>
#include <CQChartsAxisObj.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQPropertyViewModel.h>
#include <QPainter>

#include <cstring>
#include <algorithm>

//------

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
    integral = CQChartsUtil::isInteger(factor);
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

//---

CQChartsAxis::
CQChartsAxis(CQChartsPlot *plot, Direction direction, double start, double end) :
 QObject(plot), plot_(plot), direction_(direction),
 start_(std::min(start, end)), end_(std::max(start, end)), start1_(start), end1_(end),
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

  model->addProperty(gridPath         , this, "gridAbove"         , "above"  );
  model->addProperty(gridMajorLinePath, this, "gridMajorDisplayed", "visible");
  model->addProperty(gridMajorLinePath, this, "gridMajorColor"    , "color"  );
  model->addProperty(gridMajorLinePath, this, "gridMajorWidth"    , "width"  );
  model->addProperty(gridMajorLinePath, this, "gridMajorDash"     , "dash"   );
  model->addProperty(gridMinorLinePath, this, "gridMinorDisplayed", "visible");
  model->addProperty(gridMinorLinePath, this, "gridMinorColor"    , "color"  );
  model->addProperty(gridMinorLinePath, this, "gridMinorWidth"    , "width"  );
  model->addProperty(gridMinorLinePath, this, "gridMinorDash"     , "dash"   );
  model->addProperty(gridFillPath     , this, "gridFill"          , "visible");
  model->addProperty(gridFillPath     , this, "gridFillColor"     , "color"  );
  model->addProperty(gridFillPath     , this, "gridFillAlpha"     , "alpha"  );
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
  majorIncrement_ = i;

  calc();

  redraw();
}

void
CQChartsAxis::
setTickIncrement(uint tickIncrement)
{
  tickIncrement_ = tickIncrement;

  calc();

  redraw();
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
  tickLabels_[i] = label;

  redraw();
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
  labelDisplayed_ = b;

  redraw();
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
  lineData_.visible = b; redraw();
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
  lineData_.width = l; redraw();
}

const CQChartsLineDash &
CQChartsAxis::
lineDash() const
{
  return lineData_.dash;
}

void
CQChartsAxis::
setLineDash(const CQChartsLineDash &dash)
{
  lineData_.dash = dash; redraw();
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
  lineData_.color = c;
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
  majorGridLineData_.visible = b; redraw();
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
  majorGridLineData_.color = c;
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
  majorGridLineData_.width = l; redraw();
}

const CQChartsLineDash &
CQChartsAxis::
gridMajorDash() const
{
  return majorGridLineData_.dash;
}

void
CQChartsAxis::
setGridMajorDash(const CQChartsLineDash &dash)
{
  majorGridLineData_.dash = dash; redraw();
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
  minorGridLineData_.visible = b; redraw();
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
  minorGridLineData_.color = c;
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
  minorGridLineData_.width = l; redraw();
}

const CQChartsLineDash &
CQChartsAxis::
gridMinorDash() const
{
  return minorGridLineData_.dash;
}

void
CQChartsAxis::
setGridMinorDash(const CQChartsLineDash &dash)
{
  minorGridLineData_.dash = dash; redraw();
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
  gridFill_.visible = b; redraw();
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
  gridFill_.alpha = a; redraw();
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
  gridFill_.color = c;
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
  tickLabelDisplayed_ = b;

  redraw();
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
  integral_ = b;

  calc();

  redraw();
}

void
CQChartsAxis::
setLog(bool b)
{
  log_ = b;

  calc();

  redraw();
}

//---

void
CQChartsAxis::
calc()
{
  numMajorTicks_ = 1;
  numMinorTicks_ = 0;

  //------

  // Ensure Axis Start and End are in the Correct Order

  double minAxis = std::min(start(), end());
  double maxAxis = std::max(start(), end());

  if (isIntegral()) {
    minAxis = std::floor(minAxis);
    maxAxis = std::ceil (maxAxis);
  }

  //------

  /* Calculate Length */

  double length = std::abs(maxAxis - minAxis);

  if (length == 0.0)
    return;

  if (isIntegral())
    length = std::ceil(length);

  //------

  // Calculate nearest Power of Ten to Length

  int power = CQChartsUtil::RoundDown(log10(length));

  if (isIntegral()) {
    if (power < 0)
      power = 1;
  }

  //------

  if (majorIncrement_ <= 0.0) {
    // Set Default Increment to 0.1 * Power of Ten
    double increment;

    if (! isIntegral() && ! isLog()) {
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
      if (isIntegral() && ! CQChartsUtil::isInteger(axesIncrementTests[i].factor)) {
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
      if (isLog() && ! axesIncrementTests[i].isLog)
        continue;

      if (axesIncrementTests[i].incFactor <= 0)
        continue;

      if (tickIncrement_ > 0) {
        if (! CQChartsUtil::isInteger(axesIncrementTests[i].incFactor))
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

    if (isLog())
      numGapTicks = 10;

    //------

    // Set the Gap Positions

    numMajorTicks_ = CQChartsUtil::RoundDown((end1_ - start1_)/increment + 0.5);
    numMinorTicks_ = numGapTicks;
  }
  else {
    start1_ = minAxis;
    end1_   = maxAxis;
  //start1_ = start();
  //end1_   = end  ();

    numMajorTicks_ = CQChartsUtil::RoundDown((end1_ - start1_)/majorIncrement_ + 0.5);
    numMinorTicks_ = 5;
  }

  emit ticksChanged();
}

bool
CQChartsAxis::
testAxisGaps(double start, double end, double testIncrement, uint testNumGapTicks,
             AxisGapData &axisGapData)
{
  // Calculate New Start and End implied by the Test Increment

  double newStart = CQChartsUtil::RoundDownF(start/testIncrement)*testIncrement;
  double newEnd   = CQChartsUtil::RoundUpF  (end  /testIncrement)*testIncrement;

  while (newStart > start)
    newStart -= testIncrement;

  while (newEnd < end)
    newEnd += testIncrement;

  uint testNumGaps = CQChartsUtil::RoundUp((newEnd - newStart)/testIncrement);

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
         (std::abs(testNumGaps         - axisGoodTicks.opt) <
          std::abs(axisGapData.numGaps - axisGoodTicks.opt))) ||
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
         (std::abs(testNumGaps         - axisGoodTicks.opt) <
          std::abs(axisGapData.numGaps - axisGoodTicks.opt))) ||
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
majorIncrement() const
{
  if (majorIncrement_ > 0.0)
    return majorIncrement_;
  else {
    if (numMajorTicks() > 0)
      return (end1_ - start1_)/numMajorTicks();
    else
      return 0.0;
  }
}

double
CQChartsAxis::
minorIncrement() const
{
  if (numMajorTicks() > 0 && numMinorTicks() > 0)
    return (end1_ - start1_)/(numMajorTicks()*numMinorTicks());
  else
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

    if (CQChartsUtil::formatColumnValue(plot->charts(), plot->model(), column(), pos, str))
      return str;

    if (isDataLabels()) {
      int row = int(pos);

      QModelIndex parent; // TODO: support parent

      bool ok;

      QVariant header = plot->modelValue(plot->model(), row, column(), parent, ok);

      if (header.isValid()) {
        QString headerStr;

        CQChartsUtil::variantToString(header, headerStr);

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
  plot_->update();
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

void
CQChartsAxis::
drawGrid(CQChartsPlot *plot, QPainter *painter)
{
  if (! isGridMajorDisplayed() && ! isGridMinorDisplayed() && ! isGridFill())
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

  double inc = majorIncrement();

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

    double pos1 = start1_;
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
    double pos1 = start1_;

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

          if (isIntegral() && ! CQChartsUtil::isInteger(pos2))
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
  bbox_ = CQChartsGeom::BBox();

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

  //---

  painter->save();

  //---

  // axis line
  if (isLineDisplayed()) {
    drawLine(plot, painter, apos1, amin, amax);
  }

  //---

  double inc = majorIncrement();

  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  //---

  double pos1 = start1_;

  int tlen2 = majorTickLen();
  int tgap  = 2;

  //---

  lbbox_ = CQChartsGeom::BBox();

#if 0
  lmin_ = INT_MAX;
  lmax_ = INT_MIN;

  if ((direction_ == Direction::HORIZONTAL && plot->isInvertY()) ||
      (direction_ == Direction::VERTICAL   && plot->isInvertX())) {
    lmin_ = INT_MIN;
    lmax_ = INT_MAX;
  }
#endif

  //---

  lastTickLabelRect_ = CQChartsGeom::BBox();

#if 0
  double minAxis = std::min(amin, amax);
  double maxAxis = std::max(amin, amax);

  if (isIntegral()) {
    minAxis = std::floor(minAxis);
    maxAxis = std::ceil (maxAxis);
  }
#endif

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

        if (isIntegral() && ! CQChartsUtil::isInteger(pos2))
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

#if 0
  if (direction_ == Direction::HORIZONTAL) {
    int pys = (plot->isInvertY() ? -1 : 1);

    double ax3, ay3;

    plot->windowToPixel(amin, apos1, ax3, ay3);

    if (side() == Side::BOTTOM_LEFT) {
      if (! plot->isInvertY()) {
        if (lmax_ == INT_MIN)
          lmax_ = ay3 + pys*(tlen2 + tgap);
      }
      else {
        if (lmax_ == INT_MAX)
          lmax_ = ay3 + pys*(tlen2 + tgap);
      }
    }
    else {
      if (! plot->isInvertY()) {
        if (lmin_ == INT_MAX)
          lmin_ = ay3 - pys*(tlen2 + tgap);
      }
      else {
        if (lmin_ == INT_MIN)
          lmin_ = ay3 - pys*(tlen2 + tgap);
      }
    }
  }
  else {
    int pxs = (plot->isInvertX() ? -1 : 1);

    double ax3, ay3;

    plot->windowToPixel(apos1, amin, ax3, ay3);

    if (side() == Side::BOTTOM_LEFT) {
      if (! plot->isInvertX()) {
        if (lmin_ == INT_MAX)
          lmin_ = ax3 - pxs*(tlen2 + tgap);
      }
      else {
        if (lmin_ == INT_MIN)
          lmin_ = ax3 - pxs*(tlen2 + tgap);
      }
    }
    else {
      if (! plot->isInvertX()) {
        if (lmax_ == INT_MIN)
          lmax_ = ax3 + pxs*(tlen2 + tgap);
      }
      else {
        if (lmax_ == INT_MAX)
          lmax_ = ax3 + pxs*(tlen2 + tgap);
      }
    }
  }
#endif

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

  if (isSelected()) {
    if (plot->view()->mode() == CQChartsView::Mode::EDIT) {
      editHandles_.setBBox(this->bbox());

      editHandles_.draw(painter);
    }
  }

  //---

  painter->restore();
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

      if (isWindowBottom)
        bbox_ += CQChartsGeom::Point(tpos, apos - adt1);
      else
        bbox_ += CQChartsGeom::Point(tpos, apos + adt1);
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

      if (isWindowLeft)
        bbox_ += CQChartsGeom::Point(apos - adt1, tpos);
      else
        bbox_ += CQChartsGeom::Point(apos + adt1, tpos);
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

      if (CQChartsUtil::isZero(angle)) {
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

#if 0
        int pys = (isPixelBottom ? -1 : 1);

        if (! plot->isInvertY())
          lmax_ = std::max(lmax_, pt.y() + pys*(ta + td));
        else
          lmax_ = std::min(lmax_, pt.y() + pys*(ta + td));
#endif

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

#if 0
        if (! plot->isInvertY())
          lmax_ = std::max(lmax_, rrect.bottom());
        else
          lmax_ = std::min(lmax_, rrect.top());
#endif

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle)) {
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

      if (CQChartsUtil::isZero(angle)) {
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

#if 0
        if (! plot->isInvertY())
          lmin_ = std::min(lmin_, pt.y() - pys*(ta + td));
        else
          lmin_ = std::max(lmin_, pt.y() - pys*(ta + td));
#endif

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

#if 0
        if (! plot->isInvertY())
          lmin_ = std::min(lmin_, rrect.top());
        else
          lmin_ = std::min(lmin_, rrect.bottom());
#endif

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle)) {
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

    bbox_ += tbbox;
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

      if (CQChartsUtil::isZero(angle)) {
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

#if 0
        int pxs = (isPixelLeft ? -1 : 1);

        if (! plot->isInvertX())
          lmin_ = std::min(lmin_, pt.x() - pxs*tw);
        else
          lmin_ = std::max(lmin_, pt.x() - pxs*tw);
#endif

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

#if 0
        if (! plot->isInvertX())
          lmin_ = std::min(lmin_, rrect.left());
        else
          lmin_ = std::min(lmin_, rrect.right());
#endif

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle)) {
          double tx = pt.x() - (isPixelLeft ? tw : 0.0);

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

      if (CQChartsUtil::isZero(angle)) {
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

#if 0
        if (! plot->isInvertX())
          lmax_ = std::max(lmax_, pt.x() + pxs*tw);
        else
          lmax_ = std::min(lmax_, pt.x() + pxs*tw);
#endif

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

#if 0
        if (! plot->isInvertX())
          lmax_ = std::max(lmax_, rrect.right());
        else
          lmax_ = std::min(lmax_, rrect.left());
#endif

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle)) {
          double tx = pt.x() - (! isPixelLeft ? 0.0 : tw);

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

    bbox_ += tbbox;
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

    int pys = (isPixelBottom ? 1 : -1);

    if (isPixelBottom) {
      double atw = plot->pixelToWindowWidth (tw/2);
      double ath = plot->pixelToWindowHeight((lbbox_.getYMax() - ay3) + tgap) + wfh;

      painter->drawText(QPointF(axm, lbbox_.getYMax() + ta + tgap), text);

      bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos - pys*(ath      ));
      bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos - pys*(ath - wfh));
    }
    else {
      double atw = plot->pixelToWindowWidth (tw/2);
      double ath = plot->pixelToWindowHeight((ay3 - lbbox_.getYMin()) + tgap) + wfh;

      painter->drawText(QPointF(axm, lbbox_.getYMin() - td - tgap), text);

      bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos + pys*(ath      ));
      bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos + pys*(ath - wfh));
    }
  }
  else {
    double wfa = plot->pixelToWindowWidth(ta);
    double wfd = plot->pixelToWindowWidth(td);
    double wfh = wfa + wfd;

    bool isPixelLeft = (side() == Side::BOTTOM_LEFT && ! plot->isInvertX()) ||
                       (side() == Side::TOP_RIGHT   &&   plot->isInvertX());

    int pxs = (isPixelLeft ? 1 : -1);

    if (isPixelLeft) {
      double aym = (ay2 + ay1)/2 + tw/2;

      double atw = plot->pixelToWindowWidth ((ax3 - lbbox_.getXMin()) + tgap) + wfh;
      double ath = plot->pixelToWindowHeight(tw/2);

      double tx = lbbox_.getXMin() - tgap - td;

      CQChartsRotatedText::drawRotatedText(painter, tx, aym, text, 90.0);

      bbox += CQChartsGeom::Point(apos - pxs*(atw      ), (amin + amax)/2 - ath);
      bbox += CQChartsGeom::Point(apos - pxs*(atw - wfh), (amin + amax)/2 + ath);
    }
    else {
      double aym = (ay2 + ay1)/2 - tw/2;

      double atw = plot->pixelToWindowWidth ((lbbox_.getXMax() - ax3) + tgap) + wfh;
      double ath = plot->pixelToWindowHeight(tw/2);

      double tx = lbbox_.getXMax() + tgap + td;

      CQChartsRotatedText::drawRotatedText(painter, tx, aym, text, -90.0);

      bbox += CQChartsGeom::Point(apos + pxs*(atw      ), (amin + amax)/2 - ath);
      bbox += CQChartsGeom::Point(apos + pxs*(atw - wfh), (amin + amax)/2 + ath);
    }
  }

  if (plot->showBoxes())
    plot->drawWindowColorBox(painter, bbox);

  bbox_ += bbox;
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
