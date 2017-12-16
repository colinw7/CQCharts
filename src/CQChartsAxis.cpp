#include <CQChartsAxis.h>
#include <CQChartsAxisObj.h>
#include <CQChartsPlot.h>
#include <CQChartsLineObj.h>
#include <CQChartsFillObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsColumn.h>
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
 plot_(plot), direction_(direction), start_(std::min(start, end)), end_(std::max(start, end)),
 start1_(start), end1_(end)
{
  CQChartsPaletteColor themeFg   (CQChartsPaletteColor::Type::THEME_VALUE, 1);
  CQChartsPaletteColor themeGray1(CQChartsPaletteColor::Type::THEME_VALUE, 0.7);
  CQChartsPaletteColor themeGray2(CQChartsPaletteColor::Type::THEME_VALUE, 0.3);
  CQChartsPaletteColor themeGray3(CQChartsPaletteColor::Type::THEME_VALUE, 0.3);

  label_     = new CQChartsAxisLabel(this);
  tickLabel_ = new CQChartsAxisTickLabel(this);

  label_    ->setColor(themeFg);
  tickLabel_->setColor(themeFg);

  lineObj_ = new CQChartsLineObj(plot);

  lineObj_->setColor(themeGray1);

  // init grid
  majorGridLineObj_ = new CQChartsLineObj(plot);
  minorGridLineObj_ = new CQChartsLineObj(plot);
  gridFill_         = new CQChartsFillObj(plot);

  majorGridLineObj_->setDisplayed(false);
  majorGridLineObj_->setColor(themeGray2);

  minorGridLineObj_->setDisplayed(false);
  minorGridLineObj_->setColor(themeGray2);

  gridFill_->setColor(themeGray3);
  gridFill_->setAlpha(0.5);

  setGridMajorDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));
  setGridMinorDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  calc();
}

CQChartsAxis::
~CQChartsAxis()
{
  delete lineObj_;
  delete majorGridLineObj_;
  delete minorGridLineObj_;
  delete gridFill_;

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
  CQChartsColumnTypeMgr *columnTypeMgr = plot_->charts()->columnTypeMgr();

  CQBaseModel::Type  type;
  CQChartsNameValues nameValues;

  if (! columnTypeMgr->getModelColumnType(plot_->model(), column_, type, nameValues))
    return "";

  return columnTypeMgr->encodeTypeData(type, nameValues);
}

bool
CQChartsAxis::
setFormat(const QString &typeStr)
{
  CQChartsColumnTypeMgr *columnTypeMgr = plot_->charts()->columnTypeMgr();

  CQChartsNameValues nameValues;

  CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(typeStr, nameValues);

  return columnTypeMgr->setModelColumnType(plot_->model(), column_, typeData->type(), nameValues);
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
  return label_->text();
}

void
CQChartsAxis::
setLabel(const QString &str)
{
  label_->setText(str);

  redraw();
}

const QFont &
CQChartsAxis::
labelFont() const
{
  return label_->font();
}

void
CQChartsAxis::
setLabelFont(const QFont &font)
{
  label_->setFont(font);

  redraw();
}

QString
CQChartsAxis::
labelColorStr() const
{
  return label_->colorStr();
}

void
CQChartsAxis::
setLabelColorStr(const QString &str)
{
  label_->setColorStr(str);
}

QColor
CQChartsAxis::
interpLabelColor(int i, int n) const
{
  return label_->interpColor(i, n);
}

//---

bool
CQChartsAxis::
isLineDisplayed() const
{
  return lineObj_->isDisplayed();
}

void
CQChartsAxis::
setLineDisplayed(bool b)
{
  lineObj_->setDisplayed(b); redraw();
}

double
CQChartsAxis::
lineWidth() const
{
  return lineObj_->width();
}

void
CQChartsAxis::
setLineWidth(double r)
{
  lineObj_->setWidth(r); redraw();
}

const CQChartsLineDash &
CQChartsAxis::
lineDash() const
{
  return lineObj_->dash();
}

void
CQChartsAxis::
setLineDash(const CQChartsLineDash &dash)
{
  lineObj_->setDash(dash); redraw();
}

QString
CQChartsAxis::
lineColorStr() const
{
  return lineObj_->colorStr();
}

void
CQChartsAxis::
setLineColorStr(const QString &str)
{
  lineObj_->setColorStr(str);
}

QColor
CQChartsAxis::
interpLineColor(int i, int n) const
{
  return lineObj_->interpColor(i, n);
}

//---

bool
CQChartsAxis::
isGridMajorDisplayed() const
{
  return majorGridLineObj_->isDisplayed();
}

void
CQChartsAxis::
setGridMajorDisplayed(bool b)
{
  majorGridLineObj_->setDisplayed(b); redraw();
}

QString
CQChartsAxis::
gridMajorColorStr() const
{
  return majorGridLineObj_->colorStr();
}

void
CQChartsAxis::
setGridMajorColorStr(const QString &str)
{
  majorGridLineObj_->setColorStr(str);
}

QColor
CQChartsAxis::
interpGridMajorColor(int i, int n) const
{
  return majorGridLineObj_->interpColor(i, n);
}

double
CQChartsAxis::
gridMajorWidth() const
{
  return majorGridLineObj_->width();
}

void
CQChartsAxis::
setGridMajorWidth(double r)
{
  majorGridLineObj_->setWidth(r); redraw();
}

const CQChartsLineDash &
CQChartsAxis::
gridMajorDash() const
{
  return majorGridLineObj_->dash();
}

void
CQChartsAxis::
setGridMajorDash(const CQChartsLineDash &dash)
{
  majorGridLineObj_->setDash(dash); redraw();
}

bool
CQChartsAxis::
isGridMinorDisplayed() const
{
  return minorGridLineObj_->isDisplayed();
}

void
CQChartsAxis::
setGridMinorDisplayed(bool b)
{
  minorGridLineObj_->setDisplayed(b); redraw();
}

QString
CQChartsAxis::
gridMinorColorStr() const
{
  return minorGridLineObj_->colorStr();
}

void
CQChartsAxis::
setGridMinorColorStr(const QString &str)
{
  minorGridLineObj_->setColorStr(str);
}

double
CQChartsAxis::
gridMinorWidth() const
{
  return minorGridLineObj_->width();
}

void
CQChartsAxis::
setGridMinorWidth(double r)
{
  minorGridLineObj_->setWidth(r); redraw();
}

const CQChartsLineDash &
CQChartsAxis::
gridMinorDash() const
{
  return minorGridLineObj_->dash();
}

void
CQChartsAxis::
setGridMinorDash(const CQChartsLineDash &dash)
{
  minorGridLineObj_->setDash(dash); redraw();
}

bool
CQChartsAxis::
isGridFill() const
{
  return gridFill_->isVisible();
}

void
CQChartsAxis::
setGridFill(bool b)
{
  gridFill_->setVisible(b); redraw();
}

double
CQChartsAxis::
gridFillAlpha() const
{
  return gridFill_->alpha();
}

void
CQChartsAxis::
setGridFillAlpha(double a)
{
  gridFill_->setAlpha(a); redraw();
}

QString
CQChartsAxis::
gridFillColorStr() const
{
  return gridFill_->colorStr();
}

void
CQChartsAxis::
setGridFillColorStr(const QString &str)
{
  gridFill_->setColorStr(str);
}

QColor
CQChartsAxis::
interpGridFillColor(int i, int n) const
{
  return gridFill_->interpColor(i, n);
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
  return tickLabel_->font();
}

void
CQChartsAxis::
setTickLabelFont(const QFont &font)
{
  tickLabel_->setFont(font);

  redraw();
}

double
CQChartsAxis::
tickLabelAngle() const
{
  return tickLabel_->angle();
}

void
CQChartsAxis::
setTickLabelAngle(double angle)
{
  tickLabel_->setAngle(angle);

  redraw();
}

QString
CQChartsAxis::
tickLabelColorStr() const
{
  return tickLabel_->colorStr();
}

void
CQChartsAxis::
setTickLabelColorStr(const QString &str)
{
  return tickLabel_->setColorStr(str);
}

QColor
CQChartsAxis::
interpTickLabelColor(int i, int n) const
{
  return tickLabel_->interpColor(i, n);
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
}

bool
CQChartsAxis::
testAxisGaps(double start, double end, double testIncrement, uint testNumGapTicks,
             AxisGapData &axisGapData)
{
  // Calculate New Start and End implied by the Test Increment

  double newStart = CQChartsUtil::RoundDown(start/testIncrement)*testIncrement;
  double newEnd   = CQChartsUtil::RoundUp  (end  /testIncrement)*testIncrement;

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
  if (isLog())
    pos = plot_->expValue(pos);

  if (isIntegral()) {
    long ipos = long(pos);

    if (hasTickLabel(ipos))
      return tickLabel(ipos);

    if (isRequireTickLabel())
      return "";
  }

  if (column_ >= 0) {
    CQChartsColumnTypeMgr *columnTypeMgr = plot_->charts()->columnTypeMgr();

    CQBaseModel::Type  columnType;
    CQChartsNameValues nameValues;

    if (columnTypeMgr->getModelColumnType(plot_->model(), column_, columnType, nameValues)) {
      CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

      if (typeData)
        return typeData->dataName(pos, nameValues).toString();
    }
    else if (isDataLabels()) {
      int row = int(pos);

      QModelIndex ind = plot_->model()->index(row, column_);

      QVariant header = plot_->model()->data(ind, Qt::DisplayRole);

      if (header.isValid())
        return header.toString();
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
mouseDragPress(const CQChartsGeom::Point &p)
{
  dragPos_ = p;

  double apos1, apos2;

  calcPos(apos1, apos2);

  pos_ = apos1;

  return true;
}

bool
CQChartsAxis::
mouseDragMove(const CQChartsGeom::Point &p)
{
  double dx = p.x - dragPos_.x;
  double dy = p.y - dragPos_.y;

  if (direction_ == Direction::HORIZONTAL)
    pos_ = *pos_ + dy;
  else
    pos_ = *pos_ + dx;

  dragPos_ = p;

  redraw();

  return true;
}

void
CQChartsAxis::
mouseDragRelease(const CQChartsGeom::Point &)
{
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

  double inc1 = (isLog() ? plot_->expValue(inc) : inc)/numMinorTicks();

  //---

  // draw fill
  if (isGridFill()) {
    QRectF dataRect = plot_->calcRect();

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

  calcPos(apos1, apos2);

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

  double inc1 = (isLog() ? plot_->expValue(inc) : inc)/numMinorTicks();

  //---

  double pos1 = start1_;

  int tlen2 = majorTickLen();
  int tgap  = 2;

  //---

  lmin_ = INT_MAX;
  lmax_ = INT_MIN;

  if ((direction_ == Direction::HORIZONTAL && plot->isInvertY()) ||
      (direction_ == Direction::VERTICAL   && plot->isInvertX())) {
    lmin_ = INT_MIN;
    lmax_ = INT_MAX;
  }

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

  double dt = (tickLabelPlacement_ == TickLabelPlacement::BETWEEN ? -0.5 : 0.0);

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
        double pos2 = pos1 + (isLog() ? plot_->logValue(j*inc1) : j*inc1);

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
  int pxs = (plot->isInvertX() ? -1 : 1);
  int pys = (plot->isInvertY() ? -1 : 1);

  if (direction_ == Direction::HORIZONTAL) {
    double ax3, ay3;

    plot->windowToPixel(amin, apos1, ax3, ay3);

    if (side() == Side::BOTTOM_LEFT) {
      if (lmax_ == INT_MIN)
        lmax_ = ay3 + pys*(tlen2 + tgap);
    }
    else {
      if (lmin_ == INT_MAX)
        lmin_ = ay3 - pys*(tlen2 + tgap);
    }
  }
  else {
    double ax3, ay3;

    plot->windowToPixel(apos1, amin, ax3, ay3);

    if (side() == Side::BOTTOM_LEFT) {
      if (lmin_ == INT_MAX)
        lmin_ = ax3 - pxs*(tlen2 + tgap);
    }
    else {
      if (lmax_ == INT_MIN)
        lmax_ = ax3 + pxs*(tlen2 + tgap);
    }
  }

  //---

  if (isLabelDisplayed()) {
    QString text = label();

    drawAxisLabel(plot, painter, apos1, amin, amax, text);
  }

  //---

  if (plot_->showBoxes())
    plot_->drawWindowRedBox(painter, bbox_);

  //---

  painter->restore();
}

void
CQChartsAxis::
calcPos(double &apos1, double &apos2) const
{
  if (hasPosition()) {
    apos1 = *pos_;
    apos2 = apos1;
    return;
  }

  CQChartsGeom::BBox dataRange = plot_->calcDataRange();

  if (direction_ == Direction::HORIZONTAL) {
    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    apos1 = (side() == Side::BOTTOM_LEFT ? ymin : ymax);
    apos2 = (side() == Side::BOTTOM_LEFT ? ymax : ymin);
  }
  else {
    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    apos1 = (side() == Side::BOTTOM_LEFT ? xmin : xmax);
    apos2 = (side() == Side::BOTTOM_LEFT ? xmax : xmin);
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

    lineObj_->draw(painter, QPointF(ax1, ay1), QPointF(ax2, ay1));
  }
  else {
    plot->windowToPixel(apos, amin, ax1, ay1);
    plot->windowToPixel(apos, amax, ax2, ay2);

    lineObj_->draw(painter, QPointF(ax1, ay1), QPointF(ax1, ay2));
  }
}

void
CQChartsAxis::
drawMajorGridLine(CQChartsPlot *plot, QPainter *painter, double apos,
                  double dmin, double dmax)
{
  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    plot->windowToPixel(apos, dmin, ax1, ay1);
    plot->windowToPixel(apos, dmax, ax2, ay2);

    majorGridLineObj_->draw(painter, QPointF(ax1, ay1), QPointF(ax1, ay2));
  }
  else {
    plot->windowToPixel(dmin, apos, ax1, ay1);
    plot->windowToPixel(dmax, apos, ax2, ay2);

    majorGridLineObj_->draw(painter, QPointF(ax1, ay1), QPointF(ax2, ay1));
  }
}

void
CQChartsAxis::
drawMinorGridLine(CQChartsPlot *plot, QPainter *painter, double apos,
                  double dmin, double dmax)
{
  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    plot->windowToPixel(apos, dmin, ax1, ay1);
    plot->windowToPixel(apos, dmax, ax2, ay2);

    minorGridLineObj_->draw(painter, QPointF(ax1, ay1), QPointF(ax1, ay2));
  }
  else {
    plot->windowToPixel(dmin, apos, ax1, ay1);
    plot->windowToPixel(dmax, apos, ax2, ay2);

    minorGridLineObj_->draw(painter, QPointF(ax1, ay1), QPointF(ax2, ay1));
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

  if (major && tickLabelPlacement_ == TickLabelPlacement::BETWEEN) {
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

  int dt1 = (side() == Side::BOTTOM_LEFT ? tlen : -tlen);

  painter->setPen(interpLineColor(0, 1));

  int pxs = (plot->isInvertX() ? -1 : 1);
  int pys = (plot->isInvertY() ? -1 : 1);

  if (direction_ == Direction::HORIZONTAL) {
    double adt1 = plot->pixelToWindowHeight(dt1);

    if (inside)
      painter->drawLine(QPointF(ppx, ppy), QPointF(ppx, ppy - pys*dt1));
    else {
      painter->drawLine(QPointF(ppx, ppy), QPointF(ppx, ppy + pys*dt1));

      if (side() == Side::BOTTOM_LEFT)
        bbox_ += CQChartsGeom::Point(tpos, apos - adt1);
      else
        bbox_ += CQChartsGeom::Point(tpos, apos + adt1);
    }
  }
  else {
    double adt1 = plot->pixelToWindowWidth(dt1);

    if (inside)
      painter->drawLine(QPointF(ppx, ppy), QPointF(ppx + pxs*dt1, ppy));
    else {
      painter->drawLine(QPointF(ppx, ppy), QPointF(ppx - pxs*dt1, ppy));

      if (side() == Side::BOTTOM_LEFT)
        bbox_ += CQChartsGeom::Point(apos - adt1, tpos);
      else
        bbox_ += CQChartsGeom::Point(apos + adt1, tpos);
    }
  }
}

void
CQChartsAxis::
drawTickLabel(CQChartsPlot *plot, QPainter *painter, double apos, double tpos,
              bool inside)
{
  int tgap  = 2;
  int tlen1 = majorTickLen();
  int tlen2 = minorTickLen();

  double ppx, ppy;

  if (direction_ == Direction::HORIZONTAL)
    plot->windowToPixel(tpos, apos, ppx, ppy);
  else
    plot->windowToPixel(apos, tpos, ppx, ppy);

  QString text = valueStr(tpos);

  if (! text.length())
    return;

  painter->setPen(interpTickLabelColor(0, 1));

  painter->setFont(tickLabelFont());

  QFontMetricsF fm(painter->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  double angle = tickLabelAngle();

  int pxs = (plot->isInvertX() ? -1 : 1);
  int pys = (plot->isInvertY() ? -1 : 1);

  if (direction_ == Direction::HORIZONTAL) {
    double tyo = 0.0;

    if (tickLabelPlacement_ == TickLabelPlacement::MIDDLE) {
      if (inside)
        tyo = pys*tgap;
      else
        tyo = pys*(tlen1 + tgap);
    }
    else {
      if (inside)
        tyo = pys*tgap;
      else
        tyo = pys*(tlen2 + tgap);
     }

    //---

    CQChartsGeom::BBox tbbox;

    bool visible = true;

    if (side() == Side::BOTTOM_LEFT) {
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignTop;

      QPointF pt(ppx, ppy + tyo);

      if (CQChartsUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE) {
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

        if (! plot->isInvertY())
          lmax_ = std::max(lmax_, pt.y() + pys*(ta + td));
        else
          lmax_ = std::min(lmax_, pt.y() + pys*(ta + td));

        double xpos = 0.0;
        double ypos = apos - wth - atm;

        if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE)
          xpos = tpos - atw/2;
        else if (tickLabelPlacement_ == TickLabelPlacement::BOTTOM_LEFT)
          xpos = tpos - atw;
        else if (tickLabelPlacement_ == TickLabelPlacement::TOP_RIGHT)
          xpos = tpos;
        else if (tickLabelPlacement_ == TickLabelPlacement::BETWEEN)
          xpos = tpos - 0.5;

        tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        if (! plot->isInvertY())
          lmax_ = std::max(lmax_, rrect.bottom());
        else
          lmax_ = std::min(lmax_, rrect.top());

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle)) {
          double ty = pt.y() + (plot->isInvertY() ? -td : ta);

          QPointF p;

          if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE)
            p = QPointF(pt.x() - tw/2                         , ty);
          else if (tickLabelPlacement_ == TickLabelPlacement::BOTTOM_LEFT)
            p = QPointF(pt.x() - tw                           , ty);
          else if (tickLabelPlacement_ == TickLabelPlacement::TOP_RIGHT)
            p = QPointF(pt.x()                                , ty);
          else if (tickLabelPlacement_ == TickLabelPlacement::BETWEEN)
            p = QPointF(pt.x() - plot->windowToPixelWidth(0.5), ty);

          painter->drawText(p, text);
        }
        else {
          CQChartsRotatedText::drawRotatedText(painter, pt.x(), pt.y(), text,
                                               angle, align, /*alignBox*/true);
        }

        if (plot_->showBoxes())
          plot_->drawWindowRedBox(painter, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }
    else {
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignBottom;

      QPointF pt(ppx, ppy - tyo);

      if (CQChartsUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE) {
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

        if (! plot->isInvertY())
          lmin_ = std::min(lmin_, pt.y() - pys*(ta + td));
        else
          lmin_ = std::max(lmin_, pt.y() - pys*(ta + td));

        double xpos = 0.0;
        double ypos = apos + atm;

        if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE)
          xpos = tpos - atw/2;
        else if (tickLabelPlacement_ == TickLabelPlacement::BOTTOM_LEFT)
          xpos = tpos - atw;
        else if (tickLabelPlacement_ == TickLabelPlacement::TOP_RIGHT)
          xpos = tpos;
        else if (tickLabelPlacement_ == TickLabelPlacement::BETWEEN)
          xpos = tpos - 0.5;

        tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        if (! plot->isInvertY())
          lmin_ = std::min(lmin_, rrect.top());
        else
          lmin_ = std::min(lmin_, rrect.bottom());

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle)) {
          double ty = pt.y() - (plot->isInvertY() ? -ta : td);

          QPointF p;

          if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE)
            p = QPointF(pt.x() - tw/2                         , ty);
          else if (tickLabelPlacement_ == TickLabelPlacement::BOTTOM_LEFT)
            p = QPointF(pt.x() - tw                           , ty);
          else if (tickLabelPlacement_ == TickLabelPlacement::TOP_RIGHT)
            p = QPointF(pt.x()                                , ty);
          else if (tickLabelPlacement_ == TickLabelPlacement::BETWEEN)
            p = QPointF(pt.x() - plot->windowToPixelWidth(0.5), ty);

          painter->drawText(p, text);
        }
        else {
          CQChartsRotatedText::drawRotatedText(painter, pt.x(), pt.y(), text,
                                               angle, align, /*alignBox*/true);
        }

        if (plot_->showBoxes())
          plot_->drawWindowRedBox(painter, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }

    bbox_ += tbbox;
  }
  else {
    double txo = 0.0;

    if (tickLabelPlacement_ == TickLabelPlacement::MIDDLE) {
      if (inside)
        txo = pxs*tgap;
      else
        txo = pxs*(tlen1 + tgap);
    }
    else
      txo = pxs;

    //---

    CQChartsGeom::BBox tbbox;

    bool visible = true;

    if (side() == Side::BOTTOM_LEFT) {
      Qt::Alignment align = Qt::AlignVCenter | Qt::AlignRight;

      QPointF pt(ppx - txo, ppy);

      if (CQChartsUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double ath = plot->pixelToWindowHeight(ta + td);
        double atm = plot->pixelToWindowWidth (tlen1 + tgap);

        if (plot->isInvertX())
          lmin_ = std::max(lmin_, pt.x() - pxs*tw);
        else
          lmin_ = std::min(lmin_, pt.x() - pxs*tw);

        double xpos = apos - atw - atm;
        double ypos = 0.0;

        if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE)
          ypos = tpos - ath/2;
        else if (tickLabelPlacement_ == TickLabelPlacement::BOTTOM_LEFT)
          ypos = tpos - ath;
        else if (tickLabelPlacement_ == TickLabelPlacement::TOP_RIGHT)
          ypos = tpos;
        else if (tickLabelPlacement_ == TickLabelPlacement::BETWEEN)
          ypos = tpos - 0.5;

        tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + ath);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        if (! plot->isInvertX())
          lmin_ = std::min(lmin_, rrect.left());
        else
          lmin_ = std::min(lmin_, rrect.right());

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle)) {
          double tx = pt.x() - (! plot->isInvertX() ? tw : 0.0);

          QPointF p;

          if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE)
            p = QPointF(tx, pt.y() + ta/2);
          else if (tickLabelPlacement_ == TickLabelPlacement::BOTTOM_LEFT)
            p = QPointF(tx, pt.y() + ta  );
          else if (tickLabelPlacement_ == TickLabelPlacement::TOP_RIGHT)
            p = QPointF(tx - pxs*(tlen2 + tgap), pt.y() - td  );
          else if (tickLabelPlacement_ == TickLabelPlacement::BETWEEN) {
            double pb = plot->windowToPixelHeight(0.5);

            p = QPointF(tx, pt.y() - pb + ta);
          }

          painter->drawText(p, text);
        }
        else {
          CQChartsRotatedText::drawRotatedText(painter, pt.x(), pt.y(), text,
                                               angle, align, /*alignBox*/true);
        }

        if (plot_->showBoxes())
          plot_->drawWindowRedBox(painter, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }
    else {
      Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft;

      QPointF pt(ppx + txo, ppy);

      if (CQChartsUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double ath = plot->pixelToWindowHeight(ta + td);
        double atm = plot->pixelToWindowWidth (tlen1 + tgap);

        if (! plot->isInvertX())
          lmax_ = std::max(lmax_, ppx + pxs*(tlen1 + tgap) + tw);
        else
          lmax_ = std::min(lmax_, ppx + pxs*(tlen1 + tgap) + tw);

        tbbox = CQChartsGeom::BBox(apos + atm, tpos - ath/2, apos + atw + atm, tpos + ath/2);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        if (! plot->isInvertX())
          lmax_ = std::max(lmax_, rrect.right());
        else
          lmax_ = std::min(lmax_, rrect.left());

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle)) {
          double tx = pt.x() - (! plot->isInvertX() ? 0.0 : tw);

          QPointF p;

          if      (tickLabelPlacement_ == TickLabelPlacement::MIDDLE)
            p = QPointF(tx, pt.y() + ta/2);
          else if (tickLabelPlacement_ == TickLabelPlacement::BOTTOM_LEFT)
            p = QPointF(tx, pt.y() + ta  );
          else if (tickLabelPlacement_ == TickLabelPlacement::TOP_RIGHT)
            p = QPointF(tx + pxs*(tlen2 + tgap), pt.y() - td  );
          else if (tickLabelPlacement_ == TickLabelPlacement::BETWEEN) {
            double pb = plot->windowToPixelHeight(0.5);

            p = QPointF(tx, pt.y() - pb + ta);
          }

          painter->drawText(p, text);
        }
        else {
          CQChartsRotatedText::drawRotatedText(painter, pt.x(), pt.y(), text,
                                               angle, align, /*alignBox*/true);
        }

        if (plot_->showBoxes())
          plot_->drawWindowRedBox(painter, tbbox);

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

  int pxs = (plot->isInvertX() ? -1 : 1);
  int pys = (plot->isInvertY() ? -1 : 1);

  CQChartsGeom::BBox bbox;

  // draw label
  if (direction_ == Direction::HORIZONTAL) {
    double wfh = plot->pixelToWindowHeight(ta + td);

    double axm = (ax1 + ax2)/2 - tw/2;

    if (side() == Side::BOTTOM_LEFT) {
      double atw = plot->pixelToWindowWidth (tw/2);
      double ath = plot->pixelToWindowHeight((lmax_ - ay3) + tgap) + wfh;

      painter->drawText(QPointF(axm, lmax_ + (plot->isInvertY() ? -td : ta) + pys*tgap), text);

      bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos - ath      );
      bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos - ath + wfh);
    }
    else {
      double atw = plot->pixelToWindowWidth (tw/2);
      double ath = plot->pixelToWindowHeight((ay3 - lmin_) + tgap) + wfh;

      painter->drawText(QPointF(axm, lmin_ - pys*td - pys*tgap), text);

      bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos + ath      );
      bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos + ath - wfh);
    }
  }
  else {
    double wfa = plot->pixelToWindowWidth(ta);
    double wfd = plot->pixelToWindowWidth(td);
    double wfh = wfa + wfd;

    if (side() == Side::BOTTOM_LEFT) {
      double aym = (ay2 + ay1)/2 + tw/2;

      double atw = plot->pixelToWindowWidth ((ax3 - lmin_) + tgap) + wfh;
      double ath = plot->pixelToWindowHeight(tw/2);

      double tx = lmin_ - pxs*(tgap + (plot->isInvertX() ? ta : td));

      CQChartsRotatedText::drawRotatedText(painter, tx + td, aym, text, 90.0);

      bbox += CQChartsGeom::Point(apos - atw      , (amin + amax)/2 - ath);
      bbox += CQChartsGeom::Point(apos - atw + wfh, (amin + amax)/2 + ath);
    }
    else {
      double aym = (ay2 + ay1)/2 - tw/2;

      double atw = plot->pixelToWindowWidth ((lmax_ - ax3) + tgap) + wfh;
      double ath = plot->pixelToWindowHeight(tw/2);

      double tx = lmax_ + pxs*(tgap + (plot->isInvertX() ? ta : td));

      CQChartsRotatedText::drawRotatedText(painter, tx - td, aym, text, -90.0);

      bbox += CQChartsGeom::Point(apos + atw      , (amin + amax)/2 - ath);
      bbox += CQChartsGeom::Point(apos + atw - wfh, (amin + amax)/2 + ath);
    }
  }

  if (plot_->showBoxes())
    plot_->drawWindowRedBox(painter, bbox);

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
