#include <CQChartsAxis.h>
#include <CQChartsAxisObj.h>
#include <CQChartsPlot.h>
#include <CQChartsLineObj.h>
#include <CQChartsFillObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsColumn.h>
#include <CQPropertyViewModel.h>

#include <CQRotatedText.h>
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
  uint   numTicks  { 5   };
  double incFactor { 0.0 };
  bool   integral  { false };

  AxisIncrementTest(double factor, uint numTicks) :
   factor(factor), numTicks(numTicks) {
    integral = CQChartsUtil::isInteger(factor);
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
 plot_(plot), direction_(direction), start_(std::min(start, end)), end_(std::max(start, end)),
 start1_(start), end1_(end)
{
  label_     = new CQChartsAxisLabel(this);
  tickLabel_ = new CQChartsAxisTickLabel(this);

  lineObj_ = new CQChartsLineObj;

  lineObj_->setColor(QColor(128, 128, 128));

  // init grid
  gridLineObj_ = new CQChartsLineObj;
  gridFill_    = new CQChartsFillObj;

  gridLineObj_->setDisplayed(false);

  gridFill_->setColor(QColor(128, 128, 128));
  gridFill_->setAlpha(0.5);

  setGridDash(CLineDash(CLineDash::Lengths({2, 2}), 0));

  calc();
}

CQChartsAxis::
~CQChartsAxis()
{
  delete lineObj_;
  delete gridLineObj_;
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
  model->addProperty(path, this, "format"   );

  model->addProperty(path, this, "tickIncrement" );
  model->addProperty(path, this, "majorIncrement");
  model->addProperty(path, this, "start"         );
  model->addProperty(path, this, "end"           );

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

  model->addProperty(ticksPath, this, "tickInside" , "inside");
  model->addProperty(ticksPath, this, "mirrorTicks", "mirror");

  QString labelPath = path + "/label";

  model->addProperty(labelPath, this, "labelDisplayed", "visible");
  model->addProperty(labelPath, this, "label"         , "text"   );
  model->addProperty(labelPath, this, "labelFont"     , "font"   );
  model->addProperty(labelPath, this, "labelColor"    , "color"  );

  QString gridPath     = path + "/grid";
  QString gridLinePath = gridPath + "/line";
  QString gridFillPath = gridPath + "/fill";

  model->addProperty(gridPath    , this, "gridAbove"    , "above"  );
  model->addProperty(gridLinePath, this, "gridDisplayed", "visible");
  model->addProperty(gridLinePath, this, "gridColor"    , "color"  );
  model->addProperty(gridLinePath, this, "gridWidth"    , "width"  );
  model->addProperty(gridLinePath, this, "gridDash"     , "dash"   );
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

QString
CQChartsAxis::
format() const
{
  CQChartsColumnTypeMgr *columnTypeMgr = plot_->charts()->columnTypeMgr();

  CQBaseModel::Type  type;
  CQChartsNameValues nameValues;

  (void) columnTypeMgr->getModelColumnType(plot_->model(), column_, type, nameValues);

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

const QColor &
CQChartsAxis::
labelColor() const
{
  return label_->color();
}

void
CQChartsAxis::
setLabelColor(const QColor &color)
{
  label_->setColor(color);

  redraw();
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

const QColor &
CQChartsAxis::
lineColor() const
{
  return lineObj_->color();
}

void
CQChartsAxis::
setLineColor(const QColor &c)
{
  lineObj_->setColor(c); redraw();
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

const CLineDash &
CQChartsAxis::
lineDash() const
{
  return lineObj_->dash();
}

void
CQChartsAxis::
setLineDash(const CLineDash &dash)
{
  lineObj_->setDash(dash); redraw();
}

//---

bool
CQChartsAxis::
isGridDisplayed() const
{
  return gridLineObj_->isDisplayed();
}

void
CQChartsAxis::
setGridDisplayed(bool b)
{
  gridLineObj_->setDisplayed(b); redraw();
}

const QColor &
CQChartsAxis::
gridColor() const
{
  return gridLineObj_->color();
}

void
CQChartsAxis::
setGridColor(const QColor &c)
{
  gridLineObj_->setColor(c); redraw();
}

double
CQChartsAxis::
gridWidth() const
{
  return gridLineObj_->width();
}

void
CQChartsAxis::
setGridWidth(double r)
{
  gridLineObj_->setWidth(r); redraw();
}

const CLineDash &
CQChartsAxis::
gridDash() const
{
  return gridLineObj_->dash();
}

void
CQChartsAxis::
setGridDash(const CLineDash &dash)
{
  gridLineObj_->setDash(dash); redraw();
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

const QColor &
CQChartsAxis::
gridFillColor() const
{
  return gridFill_->color();
}

void
CQChartsAxis::
setGridFillColor(const QColor &c)
{
  gridFill_->setColor(c); redraw();
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

const QColor &
CQChartsAxis::
tickLabelColor() const
{
  return tickLabel_->color();
}

void
CQChartsAxis::
setTickLabelColor(const QColor &color)
{
  tickLabel_->setColor(color);

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

    //------

    // Set the Gap Positions

    numMajorTicks_ = CQChartsUtil::RoundDown((end1_ - start1_)/increment + 0.5);
    numMinorTicks_ = numGapTicks;
  }
  else {
    start1_ = start();
    end1_   = end  ();

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
    if (numMajorTicks_ > 0)
      return (end1_ - start1_)/numMajorTicks_;
    else
      return 0.0;
  }
}

double
CQChartsAxis::
minorIncrement() const
{
  if (numMajorTicks_ > 0 && numMinorTicks_ > 0)
    return (end1_ - start1_)/(numMajorTicks_*numMinorTicks_);
  else
    return 0.0;
}

QString
CQChartsAxis::
valueStr(double pos) const
{
  if (isIntegral()) {
    long ipos = long(pos);

    if (hasTickLabel(ipos))
      return tickLabel(ipos);
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

void
CQChartsAxis::
redraw()
{
  plot_->update();
}

void
CQChartsAxis::
drawGrid(CQChartsPlot *plot, QPainter *p)
{
  if (! isGridDisplayed() && ! isGridFill())
    return;

  //---

  CQChartsGeom::BBox dataRange = plot->calcDataRange();

  double amin, amax;

  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    amin = start();
    amax = end  ();

    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    plot->windowToPixel(amin, ymin, ax1, ay1);
    plot->windowToPixel(amax, ymax, ax2, ay2);
  }
  else {
    amin = start();
    amax = end  ();

    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    plot->windowToPixel(xmin, amin, ax1, ay1);
    plot->windowToPixel(xmax, amax, ax2, ay2);
  }

  //---

  p->save();

  //---

  double inc = majorIncrement();

  //---

  // draw fill
  if (isGridFill()) {
    QRectF dataRect = plot_->calcRect();

    p->setClipRect(dataRect, Qt::ReplaceClip);

    //---

    QColor fillColor = gridFillColor();

    fillColor.setAlpha(255*gridFillAlpha());

    QBrush brush(fillColor);

    //---

    double pos1 = start1_;
    double pos2 = pos1;

    for (uint i = 0; i < numMajorTicks() + 1; i++) {
      if (i & 1) {
        if (pos2 >= start() || pos1 <= end()) {
          double pos3 = std::max(pos1, start());
          double pos4 = std::min(pos2, end  ());

          double ppx1, ppy1, ppx2, ppy2;

          plot->windowToPixel(pos3, pos1, ppx1, ppy1);
          plot->windowToPixel(pos4, pos2, ppx2, ppy2);

          CQChartsGeom::BBox bbox;

          if (direction_ == Direction::HORIZONTAL)
            bbox = CQChartsGeom::BBox(ppx1, ay1, ppx2, ay2);
          else
            bbox = CQChartsGeom::BBox(ax1, ppy1, ax2, ppy2);

          p->fillRect(CQChartsUtil::toQRect(bbox), brush);
        }
      }

      //---

      pos1 = pos2;
      pos2 = pos1 + inc;
    }
  }

  //---

  // draw grid lines
  if (isGridDisplayed()) {
    double pos1 = start1_;

    for (uint i = 0; i < numMajorTicks() + 1; i++) {
      // draw major line (grid and tick)
      if (pos1 >= start() && pos1 <= end()) {
        double ppx, ppy;

        plot->windowToPixel(pos1, pos1, ppx, ppy);

        if (direction_ == Direction::HORIZONTAL)
          gridLineObj_->draw(p, QPointF(ppx, ay1), QPointF(ppx, ay2));
        else
          gridLineObj_->draw(p, QPointF(ax1, ppy), QPointF(ax2, ppy));
      }

      //---

      pos1 += inc;
    }
  }

  //---

  p->restore();
}

void
CQChartsAxis::
draw(CQChartsPlot *plot, QPainter *p)
{
  bbox_ = CQChartsGeom::BBox();

  //---

  CQChartsGeom::BBox dataRange = plot->calcDataRange();

  bool hasPos = !!pos_;

  double amin, amax, apos1, apos2;

  if (hasPos) {
    apos1 = *pos_;
    apos2 = apos1;
  }

  if (direction_ == Direction::HORIZONTAL) {
    amin = start();
    amax = end  ();

    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    if (! hasPos) {
      apos1 = (side() == Side::BOTTOM_LEFT ? ymin : ymax);
      apos2 = (side() == Side::BOTTOM_LEFT ? ymax : ymin);
    }

    bbox_ += CQChartsGeom::Point(amin, apos1);
    bbox_ += CQChartsGeom::Point(amax, apos1);
  }
  else {
    amin = start();
    amax = end  ();

    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    if (! hasPos) {
      apos1 = (side() == Side::BOTTOM_LEFT ? xmin : xmax);
      apos2 = (side() == Side::BOTTOM_LEFT ? xmax : xmin);
    }

    bbox_ += CQChartsGeom::Point(apos1, amin);
    bbox_ += CQChartsGeom::Point(apos1, amax);
  }

  //---

  p->save();

  //---

  // axis line
  if (isLineDisplayed()) {
    drawLine(plot, p, apos1, amin, amax);
  }

  //---

  double inc  = majorIncrement();
  double inc1 = inc/numMinorTicks();

  double pos1 = start1_;

  int tlen2 = majorTickLen();
  int tgap  = 2;

  lmin_              = INT_MAX;
  lmax_              = INT_MIN;
  lastTickLabelRect_ = CQChartsGeom::BBox();

  double minAxis = std::min(start(), end());
  double maxAxis = std::max(start(), end());

  if (isIntegral()) {
    minAxis = std::floor(minAxis);
    maxAxis = std::ceil (maxAxis);
  }

  for (uint i = 0; i < numMajorTicks() + 1; i++) {
    // draw major line (grid and tick)
    if (pos1 >= minAxis && pos1 <= maxAxis) {
      double ppx, ppy;

      plot->windowToPixel(pos1, pos1, ppx, ppy);

      //---

      // draw major tick (or minor tick if major ticks off and minor ones on)
      if      (isMajorTicksDisplayed()) {
        drawTickLine(plot, p, apos1, pos1, isTickInside(), /*major*/true);

        if (isMirrorTicks())
          drawTickLine(plot, p, apos2, pos1, ! isTickInside(), /*major*/true);
      }
      else if (isMinorTicksDisplayed()) {
        drawTickLine(plot, p, apos1, pos1, isTickInside(), /*major*/false);

        if (isMirrorTicks())
          drawTickLine(plot, p, apos2, pos1, ! isTickInside(), /*major*/false);
      }
    }

    if (isMinorTicksDisplayed() && i < numMajorTicks()) {
      for (uint j = 1; j < numMinorTicks(); j++) {
        double pos2 = pos1 + j*inc1;

        if (isIntegral() && ! CQChartsUtil::isInteger(pos2))
          continue;

        // draw minor tick line
        if (pos2 >= minAxis && pos2 <= maxAxis) {
          drawTickLine(plot, p, apos1, pos2, isTickInside(), /*major*/false);

          if (isMirrorTicks())
            drawTickLine(plot, p, apos2, pos2, ! isTickInside(), /*major*/false);
        }
      }
    }

    //---

    if (isTickLabelDisplayed()) {
      // draw major tick label
      if (pos1 >= minAxis && pos1 <= maxAxis) {
        drawTickLabel(plot, p, apos1, pos1, isTickInside());
      }
    }

    //---

    pos1 += inc;
  }

  //---

  // fix range if not set
  if (direction_ == Direction::HORIZONTAL) {
    double ax3, ay3;

    plot->windowToPixel(amin, apos1, ax3, ay3);

    if (side() == Side::BOTTOM_LEFT) {
      if (lmax_ == INT_MIN)
        lmax_ = ay3 + tlen2 + tgap;
    }
    else {
      if (lmin_ == INT_MAX)
        lmin_ = ay3 - tlen2 - tgap;
    }
  }
  else {
    double ax3, ay3;

    plot->windowToPixel(apos1, amin, ax3, ay3);

    if (side() == Side::BOTTOM_LEFT) {
      if (lmin_ == INT_MAX)
        lmin_ = ax3 - tlen2 - tgap;
    }
    else {
      if (lmax_ == INT_MIN)
        lmax_ = ax3 + tlen2 + tgap;
    }
  }

  //---

  if (isLabelDisplayed()) {
    QString text = label();

    drawAxisLabel(plot, p, apos1, amin, amax, text);
  }

  //---

  if (plot_->showBoxes())
    plot_->drawWindowRedBox(p, bbox_);

  //---

  p->restore();
}

void
CQChartsAxis::
drawLine(CQChartsPlot *plot, QPainter *p, double apos, double amin, double amax)
{
  p->setPen(lineColor());

  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    plot->windowToPixel(amin, apos, ax1, ay1);
    plot->windowToPixel(amax, apos, ax2, ay2);

    lineObj_->draw(p, QPointF(ax1, ay1), QPointF(ax2, ay1));
  }
  else {
    plot->windowToPixel(apos, amin, ax1, ay1);
    plot->windowToPixel(apos, amax, ax2, ay2);

    lineObj_->draw(p, QPointF(ax1, ay1), QPointF(ax1, ay2));
  }
}

void
CQChartsAxis::
drawTickLine(CQChartsPlot *plot, QPainter *p, double apos, double tpos, bool inside, bool major)
{
  int tlen = (major ? majorTickLen() : minorTickLen());

  double ppx, ppy;

  if (direction_ == Direction::HORIZONTAL)
    plot->windowToPixel(tpos, apos, ppx, ppy);
  else
    plot->windowToPixel(apos, tpos, ppx, ppy);

  int dt1 = (side() == Side::BOTTOM_LEFT ? tlen : -tlen);

  p->setPen(lineColor());

  if (direction_ == Direction::HORIZONTAL) {
    double adt1 = plot->pixelToWindowHeight(dt1);

    if (inside)
      p->drawLine(ppx, ppy, ppx, ppy - dt1);
    else {
      p->drawLine(ppx, ppy, ppx, ppy + dt1);

      if (side() == Side::BOTTOM_LEFT)
        bbox_ += CQChartsGeom::Point(tpos, apos - adt1);
      else
        bbox_ += CQChartsGeom::Point(tpos, apos + adt1);
    }
  }
  else {
    double adt1 = plot->pixelToWindowWidth(dt1);

    if (inside)
      p->drawLine(ppx, ppy, ppx + dt1, ppy);
    else {
      p->drawLine(ppx, ppy, ppx - dt1, ppy);

      if (side() == Side::BOTTOM_LEFT)
        bbox_ += CQChartsGeom::Point(apos - adt1, tpos);
      else
        bbox_ += CQChartsGeom::Point(apos + adt1, tpos);
    }
  }
}

void
CQChartsAxis::
drawTickLabel(CQChartsPlot *plot, QPainter *p, double apos, double tpos, bool inside)
{
  int tgap = 2;
  int tlen = majorTickLen();

  double ppx, ppy;

  if (direction_ == Direction::HORIZONTAL)
    plot->windowToPixel(tpos, apos, ppx, ppy);
  else
    plot->windowToPixel(apos, tpos, ppx, ppy);

  QString text = valueStr(tpos);

  p->setPen (tickLabelColor());
  p->setFont(tickLabelFont ());

  QFontMetricsF fm(p->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  double angle = tickLabelAngle();

  if (direction_ == Direction::HORIZONTAL) {
    CQChartsGeom::BBox tbbox;

    bool visible = true;

    if (side() == Side::BOTTOM_LEFT) {
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignTop;

      QPointF pt;

      if (inside)
        pt = QPointF(ppx, ppy + tgap);
      else
        pt = QPointF(ppx, ppy + tlen + tgap);

      if (CQChartsUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double ath = plot->pixelToWindowHeight(ta + td);
        double atm = plot->pixelToWindowHeight(tlen + tgap);

        lmax_ = std::max(lmax_, pt.y() + ta + td);

        tbbox = CQChartsGeom::BBox(tpos - atw/2, apos - ath - atm, tpos + atw/2, apos - atm);
      }
      else {
        QRectF rrect = CQRotatedText::bbox(pt.x(), pt.y(), text, p->font(),
                                           angle, 0, align, /*alignBox*/true);

        lmax_ = std::max(lmax_, rrect.bottom());

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle))
          p->drawText(pt.x() - tw/2, pt.y() + ta, text);
        else
          CQRotatedText::drawRotatedText(p, pt.x(), pt.y(), text,
                                         angle, align, /*alignBox*/true);

        if (plot_->showBoxes())
          plot_->drawWindowRedBox(p, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }
    else {
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignBottom;

      QPointF pt;

      if (inside)
        pt = QPointF(ppx, ppy - tgap);
      else
        pt = QPointF(ppx, ppy - tlen - tgap);

      if (CQChartsUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double ath = plot->pixelToWindowHeight(ta + td);
        double atm = plot->pixelToWindowHeight(tlen + tgap);

        lmin_ = std::min(lmin_, pt.y() - ta - td);

        tbbox = CQChartsGeom::BBox(tpos - atw/2, apos + atm, tpos + atw/2, apos + ath + atm);
      }
      else {
        QRectF rrect = CQRotatedText::bbox(pt.x(), pt.y(), text, p->font(),
                                           angle, 0, align, /*alignBox*/true);

        lmin_ = std::min(lmin_, rrect.top());

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle))
          p->drawText(pt.x() - tw/2, pt.y() - td, text);
        else
          CQRotatedText::drawRotatedText(p, pt.x(), pt.y(), text,
                                         angle, align, /*alignBox*/true);

        if (plot_->showBoxes())
          plot_->drawWindowRedBox(p, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }

    bbox_ += tbbox;
  }
  else {
    CQChartsGeom::BBox tbbox;

    bool visible = true;

    if (side() == Side::BOTTOM_LEFT) {
      Qt::Alignment align = Qt::AlignVCenter | Qt::AlignRight;

      QPointF pt;

      if (inside)
        pt = QPointF(ppx - tgap, ppy);
      else
        pt = QPointF(ppx - tlen - tgap, ppy);

      if (CQChartsUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double ath = plot->pixelToWindowHeight(ta + td);
        double atm = plot->pixelToWindowWidth (tlen + tgap);

        lmin_ = std::min(lmin_, pt.x() - tw);

        tbbox = CQChartsGeom::BBox(apos - atw - atm, tpos - ath/2, apos - atm, tpos + ath/2);
      }
      else {
        QRectF rrect = CQRotatedText::bbox(pt.x(), pt.y(), text, p->font(),
                                           angle, 0, align, /*alignBox*/true);

        lmin_ = std::min(lmin_, rrect.left());

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle))
          p->drawText(pt.x() - tw, pt.y() + ta/2, text);
        else
          CQRotatedText::drawRotatedText(p, pt.x(), pt.y(), text,
                                         angle, align, /*alignBox*/true);

        if (plot_->showBoxes())
          plot_->drawWindowRedBox(p, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }
    else {
      Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft;

      QPointF pt;

      if (inside)
        pt = QPointF(ppx + tgap, ppy);
      else
        pt = QPointF(ppx + tlen + tgap, ppy);

      if (CQChartsUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double ath = plot->pixelToWindowHeight(ta + td);
        double atm = plot->pixelToWindowWidth (tlen + tgap);

        lmax_ = std::max(lmax_, ppx + tlen + tgap + tw);

        tbbox = CQChartsGeom::BBox(apos + atm, tpos - ath/2, apos + atw + atm, tpos + ath/2);
      }
      else {
        QRectF rrect = CQRotatedText::bbox(pt.x(), pt.y(), text, p->font(),
                                           angle, 0, align, /*alignBox*/true);

        lmax_ = std::max(lmax_, rrect.right());

        plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CQChartsUtil::isZero(angle))
          p->drawText(pt.x(), pt.y() + ta/2, text);
        else
          CQRotatedText::drawRotatedText(p, pt.x(), pt.y(), text,
                                         angle, align, /*alignBox*/true);

        if (plot_->showBoxes())
          plot_->drawWindowRedBox(p, tbbox);

        lastTickLabelRect_ = tbbox;
      }
    }

    bbox_ += tbbox;
  }
}

void
CQChartsAxis::
drawAxisLabel(CQChartsPlot *plot, QPainter *p, double apos, double amin, double amax,
              const QString &text)
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

  p->setPen (labelColor());
  p->setFont(labelFont ());

  QFontMetricsF fm(p->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  // draw label
  if (direction_ == Direction::HORIZONTAL) {
    double axm = (ax1 + ax2)/2 - tw/2;

    if (side() == Side::BOTTOM_LEFT) {
      double atw = plot->pixelToWindowWidth (tw/2);
      double ath = plot->pixelToWindowHeight((lmax_ - ay3) + tgap + ta + td);

      p->drawText(axm, lmax_ + ta + tgap, text);

      bbox_ += CQChartsGeom::Point((amin + amax)/2 - atw, apos - ath);
      bbox_ += CQChartsGeom::Point((amin + amax)/2 + atw, apos - ath);
    }
    else {
      double atw = plot->pixelToWindowWidth (tw/2);
      double ath = plot->pixelToWindowHeight((ay3 - lmin_) + tgap + ta + td);

      p->drawText(axm, lmin_ - td - tgap, text);

      bbox_ += CQChartsGeom::Point((amin + amax)/2 - atw, apos + ath);
      bbox_ += CQChartsGeom::Point((amin + amax)/2 + atw, apos + ath);
    }
  }
  else {
    if (side() == Side::BOTTOM_LEFT) {
      double aym = (ay2 + ay1)/2 + tw/2;

      double atw = plot->pixelToWindowWidth ((ax3 - lmin_) + tgap + ta + td);
      double ath = plot->pixelToWindowHeight(tw/2);

      CQRotatedText::drawRotatedText(p, lmin_ - tgap - td, aym, text, 90.0);

      bbox_ += CQChartsGeom::Point(apos - atw, (amin + amax)/2 - ath);
      bbox_ += CQChartsGeom::Point(apos - atw, (amin + amax)/2 + ath);
    }
    else {
      double aym = (ay2 + ay1)/2 - tw/2;

      double atw = plot->pixelToWindowWidth ((lmax_ - ax3) + tgap + ta + td);
      double ath = plot->pixelToWindowHeight(tw/2);

      CQRotatedText::drawRotatedText(p, lmax_ + tgap + td, aym, text, -90.0);

      bbox_ += CQChartsGeom::Point(apos + atw, (amin + amax)/2 - ath);
      bbox_ += CQChartsGeom::Point(apos + atw, (amin + amax)/2 + ath);
    }
  }
}

//------

CQChartsAxisLabel::
CQChartsAxisLabel(CQChartsAxis *axis) :
 axis_(axis)
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
 axis_(axis)
{
}

void
CQChartsAxisTickLabel::
redrawBoxObj()
{
  axis_->redraw();
}
