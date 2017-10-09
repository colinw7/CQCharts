#include <CQChartsAxis.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsColumn.h>
#include <CQPropertyViewTree.h>

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

  lineObj_.setColor(QColor(128, 128, 128));

  setGridDash(CLineDash(CLineDash::Lengths({2, 2}), 0));

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
addProperties(CQPropertyViewTree *tree, const QString &path)
{
  tree->addProperty(path, this, "visible"  );
  tree->addProperty(path, this, "direction");
  tree->addProperty(path, this, "side"     );
  tree->addProperty(path, this, "integral" );
  tree->addProperty(path, this, "format"   );

  QString linePath = path + "/line";

  tree->addProperty(linePath, this, "lineDisplayed", "displayed");
  tree->addProperty(linePath, this, "lineColor"    , "color");
  tree->addProperty(linePath, this, "lineWidth"    , "width");
  tree->addProperty(linePath, this, "lineDash"     , "dash");

  QString ticksPath = path + "/ticks";

  tree->addProperty(ticksPath, this, "majorTicksDisplayed", "majorDisplayed");
  tree->addProperty(ticksPath, this, "minorTicksDisplayed", "minorDisplayed");
  tree->addProperty(ticksPath, this, "majorTickLen"       , "majorLength");
  tree->addProperty(ticksPath, this, "minorTickLen"       , "minorLength");

  QString ticksLabelPath = ticksPath + "/label";

  tree->addProperty(ticksLabelPath, this, "tickLabelDisplayed", "displayed");
  tree->addProperty(ticksLabelPath, this, "tickLabelFont"     , "font");
  tree->addProperty(ticksLabelPath, this, "tickLabelColor"    , "color");
  tree->addProperty(ticksLabelPath, this, "tickLabelAngle"    , "angle");
  tree->addProperty(ticksLabelPath, this, "tickLabelAutoHide" , "autoHide");

  QString labelPath = path + "/label";

  tree->addProperty(labelPath, this, "labelDisplayed", "displayed");
  tree->addProperty(labelPath, this, "label"         , "text");
  tree->addProperty(labelPath, this, "labelFont"     , "font");
  tree->addProperty(labelPath, this, "labelColor"    , "color");

  QString gridPath     = path + "/grid";
  QString gridLinePath = gridPath + "/line";
  QString gridFillPath = gridPath + "/file";

  tree->addProperty(gridPath    , this, "gridDisplayed", "displayed");
  tree->addProperty(gridPath    , this, "gridAbove"    , "above");
  tree->addProperty(gridLinePath, this, "gridColor"    , "color");
  tree->addProperty(gridLinePath, this, "gridWidth"    , "width");
  tree->addProperty(gridLinePath, this, "gridDash"     , "dash");
  tree->addProperty(gridFillPath, this, "gridFill"     , "displayed");
  tree->addProperty(gridFillPath, this, "gridFillColor", "color");
  tree->addProperty(gridFillPath, this, "gridFillAlpha", "alpha");
}

void
CQChartsAxis::
setRange(double start, double end)
{
  start_ = std::min(start, end);
  end_   = std::max(start, end);

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

QString
CQChartsAxis::
format() const
{
  QVariant columnTypeVar =
    plot_->model()->headerData(column_, Qt::Horizontal, CQCharts::Role::ColumnType);

  return columnTypeVar.toString();
}

bool
CQChartsAxis::
setFormat(const QString &s)
{
  return plot_->model()->setHeaderData(column_, Qt::Horizontal, s, CQCharts::Role::ColumnType);
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
getLabel() const
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
getLabelFont() const
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
getLabelColor() const
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
getTickLabelFont() const
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
getTickLabelColor() const
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
getTickLabelAngle() const
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

    numTicks1_ = CQChartsUtil::RoundDown((end1_ - start1_)/increment + 0.5);
    numTicks2_ = numGapTicks;
  }
  else {
    start1_    = start_;
    end1_      = end_;
    numTicks1_ = CQChartsUtil::RoundDown((end1_ - start1_)/majorIncrement_ + 0.5);
    numTicks2_ = 5;
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
  if (isIntegral()) {
    long ipos = long(pos);

    if (hasTickLabel(ipos))
      return getTickLabel(ipos);
  }

  if (column_ >= 0) {
    QVariant columnTypeVar =
      plot_->model()->headerData(column_, Qt::Horizontal, CQCharts::Role::ColumnType);

    if (columnTypeVar.isValid()) {
      QString columnType = columnTypeVar.toString();

      CQChartsNameValues nameValues;

      CQChartsColumnType *typeData =
        plot_->charts()->columnTypeMgr()->decodeTypeData(columnType, nameValues);

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
  if (! getGridDisplayed())
    return;

  //---

  CBBox2D dataRange = plot->calcDataRange();

  double amin, amax;

  double ax1, ay1, ax2, ay2;

  if (direction_ == Direction::HORIZONTAL) {
    amin = getStart();
    amax = getEnd  ();

    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    plot->windowToPixel(amin, ymin, ax1, ay1);
    plot->windowToPixel(amax, ymax, ax2, ay2);
  }
  else {
    amin = getStart();
    amax = getEnd  ();

    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    plot->windowToPixel(xmin, amin, ax1, ay1);
    plot->windowToPixel(xmax, amax, ax2, ay2);
  }

  //---

  p->save();

  //---

  double inc = getMajorIncrement();

  //---

  if (isGridFill()) {
    QColor fillColor = gridFillColor();

    fillColor.setAlpha(255*gridFillAlpha());

    QBrush brush(fillColor);

    //---

    double pos1 = start1_;
    double pos2 = pos1;

    for (uint i = 0; i < getNumMajorTicks() + 1; i++) {
      if (i & 1) {
        if (pos2 >= getStart() || pos1 <= getEnd()) {
          double pos3 = std::max(pos1, getStart());
          double pos4 = std::min(pos2, getEnd  ());

          double ppx1, ppy1, ppx2, ppy2;

          plot->windowToPixel(pos3, pos1, ppx1, ppy1);
          plot->windowToPixel(pos4, pos2, ppx2, ppy2);

          CBBox2D bbox;

          if (direction_ == Direction::HORIZONTAL)
            bbox = CBBox2D(ppx1, ay1, ppx2, ay2);
          else
            bbox = CBBox2D(ax1, ppy1, ax2, ppy2);

          p->fillRect(CQChartsUtil::toQRect(bbox), brush);
        }
      }

      //---

      pos1 = pos2;
      pos2 = pos1 + inc;
    }
  }

  //---

  double pos1 = start1_;

  for (uint i = 0; i < getNumMajorTicks() + 1; i++) {
    // draw major line (grid and tick)
    if (pos1 >= getStart() && pos1 <= getEnd()) {
      double ppx, ppy;

      plot->windowToPixel(pos1, pos1, ppx, ppy);

      if (direction_ == Direction::HORIZONTAL)
        gridLineObj_.draw(p, QPointF(ppx, ay1), QPointF(ppx, ay2));
      else
        gridLineObj_.draw(p, QPointF(ax1, ppy), QPointF(ax2, ppy));
    }

    //---

    pos1 += inc;
  }

  //---

  p->restore();
}

void
CQChartsAxis::
draw(CQChartsPlot *plot, QPainter *p)
{
  bbox_ = CBBox2D();

  //---

  CBBox2D dataRange = plot->calcDataRange();

  double amin, amax, apos;

  double ax1, ay1, ax2, ay2, ax3, ay3;

  if (direction_ == Direction::HORIZONTAL) {
    amin = getStart();
    amax = getEnd  ();

    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    apos = pos_.value_or(getSide() == Side::BOTTOM_LEFT ? ymin : ymax);

    plot->windowToPixel(amin, ymin, ax1, ay1);
    plot->windowToPixel(amax, ymax, ax2, ay2);
    plot->windowToPixel(amin, apos, ax3, ay3);

    bbox_ += CPoint2D(amin, apos);
    bbox_ += CPoint2D(amax, apos);
  }
  else {
    amin = getStart();
    amax = getEnd  ();

    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    apos = pos_.value_or(getSide() == Side::BOTTOM_LEFT ? xmin : xmax);

    plot->windowToPixel(xmin, amin, ax1, ay1);
    plot->windowToPixel(xmax, amax, ax2, ay2);
    plot->windowToPixel(apos, amin, ax3, ay3);

    bbox_ += CPoint2D(apos, amin);
    bbox_ += CPoint2D(apos, amax);
  }

  //---

  p->save();

  //---

  // axis line
  if (getLineDisplayed()) {
    p->setPen(getLineColor());

    if (direction_ == Direction::HORIZONTAL)
      lineObj_.draw(p, QPointF(ax1, ay3), QPointF(ax2, ay3));
    else
      lineObj_.draw(p, QPointF(ax3, ay1), QPointF(ax3, ay2));
  }

  //---

  double inc  = getMajorIncrement();
  double inc1 = inc/getNumMinorTicks();

  double pos1 = start1_;

  int tlen1 = getMinorTickLen();
  int tlen2 = getMajorTickLen();
  int tgap  = 2;

  double lmin = INT_MAX, lmax = INT_MIN;

  CBBox2D lastTickLabelRect;

  for (uint i = 0; i < getNumMajorTicks() + 1; i++) {
    // draw major line (grid and tick)
    if (pos1 >= getStart() && pos1 <= getEnd()) {
      double ppx, ppy;

      plot->windowToPixel(pos1, pos1, ppx, ppy);

      //---

      // draw major tick (or minor tick if major ticks off and minor ones on)
      if      (getMajorTicksDisplayed()) {
        int dt1 = (getSide() == Side::BOTTOM_LEFT ? tlen2 : -tlen2);

        p->setPen(getLineColor());

        if (direction_ == Direction::HORIZONTAL) {
          double adt1 = plot->pixelToWindowHeight(dt1);

          p->drawLine(ppx, ay3, ppx, ay3 + dt1);

          if (getSide() == Side::BOTTOM_LEFT)
            bbox_ += CPoint2D(pos1, apos - adt1);
          else
            bbox_ += CPoint2D(pos1, apos + adt1);
        }
        else {
          double adt1 = plot->pixelToWindowWidth(dt1);

          p->drawLine(ax3, ppy, ax3 - dt1, ppy);

          if (getSide() == Side::BOTTOM_LEFT)
            bbox_ += CPoint2D(apos - adt1, pos1);
          else
            bbox_ += CPoint2D(apos + adt1, pos1);
        }
      }
      else if (getMinorTicksDisplayed()) {
        int dt1 = (getSide() == Side::BOTTOM_LEFT ? tlen1 : -tlen1);

        p->setPen(getLineColor());

        if (direction_ == Direction::HORIZONTAL) {
          double adt1 = plot->pixelToWindowHeight(dt1);

          p->drawLine(ppx, ay3, ppx, ay3 + dt1);

          if (getSide() == Side::BOTTOM_LEFT)
            bbox_ += CPoint2D(pos1, apos - adt1);
          else
            bbox_ += CPoint2D(pos1, apos + adt1);
        }
        else {
          double adt1 = plot->pixelToWindowWidth(dt1);

          p->drawLine(ax3, ppy, ax3 - dt1, ppy);

          if (getSide() == Side::BOTTOM_LEFT)
            bbox_ += CPoint2D(apos - adt1, pos1);
          else
            bbox_ += CPoint2D(apos + adt1, pos1);
        }
      }
    }

    if (getMinorTicksDisplayed() && i < getNumMajorTicks()) {
      for (uint j = 1; j < getNumMinorTicks(); j++) {
        double pos2 = pos1 + j*inc1;

        if (isIntegral() && ! CQChartsUtil::isInteger(pos2))
          continue;

        // draw minor tick line
        if (pos2 >= getStart() && pos2 <= getEnd()) {
          double ppx, ppy;

          plot->windowToPixel(pos2, pos2, ppx, ppy);

          int dt2 = (getSide() == Side::BOTTOM_LEFT ? tlen1 : -tlen1);

          p->setPen(getLineColor());

          if (direction_ == Direction::HORIZONTAL) {
            double adt2 = plot->pixelToWindowHeight(dt2);

            p->drawLine(ppx, ay3, ppx, ay3 + dt2);

            if (getSide() == Side::BOTTOM_LEFT)
              bbox_ += CPoint2D(pos2, apos - adt2);
            else
              bbox_ += CPoint2D(pos2, apos + adt2);
          }
          else {
            double adt2 = plot->pixelToWindowWidth(dt2);

            p->drawLine(ax3, ppy, ax3 - dt2, ppy);

            if (getSide() == Side::BOTTOM_LEFT)
              bbox_ += CPoint2D(apos - adt2, pos2);
            else
              bbox_ += CPoint2D(apos + adt2, pos2);
          }
        }
      }
    }

    //---

    if (isTickLabelDisplayed()) {
      // draw major tick label
      if (pos1 >= getStart() && pos1 <= getEnd()) {
        double ppx, ppy;

        plot->windowToPixel(pos1, pos1, ppx, ppy);

        QString text = getValueStr(pos1);

        p->setPen (getTickLabelColor());
        p->setFont(getTickLabelFont ());

        QFontMetricsF fm(p->font());

        double tw = fm.width(text);
        double ta = fm.ascent();
        double td = fm.descent();

        double angle = getTickLabelAngle();

        if (direction_ == Direction::HORIZONTAL) {
          CBBox2D tbbox;

          bool visible = true;

          if (getSide() == Side::BOTTOM_LEFT) {
            Qt::Alignment align = Qt::AlignHCenter | Qt::AlignTop;

            QPointF pt(ppx, ay3 + tlen2 + tgap);

            if (CQChartsUtil::isZero(angle)) {
              double atw = plot->pixelToWindowWidth (tw);
              double ath = plot->pixelToWindowHeight(ta + td);
              double atm = plot->pixelToWindowHeight(tlen2 + tgap);

              lmax = std::max(lmax, pt.y() + ta + td);

              tbbox = CBBox2D(pos1 - atw/2, apos - ath - atm, pos1 + atw/2, apos - atm);
            }
            else {
              QRectF rrect = CQRotatedText::bbox(pt.x(), pt.y(), text, p->font(),
                                                 angle, 0, align, /*alignBox*/true);

              lmax = std::max(lmax, rrect.bottom());

              plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
            }

            if (isTickLabelAutoHide()) {
              if (lastTickLabelRect.isSet() && lastTickLabelRect.overlaps(tbbox))
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

              lastTickLabelRect = tbbox;
            }
          }
          else {
            Qt::Alignment align = Qt::AlignHCenter | Qt::AlignBottom;

            QPointF pt(ppx, ay3 - tlen2 - tgap);

            if (CQChartsUtil::isZero(angle)) {
              double atw = plot->pixelToWindowWidth (tw);
              double ath = plot->pixelToWindowHeight(ta + td);
              double atm = plot->pixelToWindowHeight(tlen2 + tgap);

              lmin = std::min(lmin, pt.y() - ta - td);

              tbbox = CBBox2D(pos1 - atw/2, apos + atm, pos1 + atw/2, apos + ath + atm);
            }
            else {
              QRectF rrect = CQRotatedText::bbox(pt.x(), pt.y(), text, p->font(),
                                                 angle, 0, align, /*alignBox*/true);

              lmin = std::min(lmin, rrect.top());

              plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
            }

            if (isTickLabelAutoHide()) {
              if (lastTickLabelRect.isSet() && lastTickLabelRect.overlaps(tbbox))
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

              lastTickLabelRect = tbbox;
            }
          }

          bbox_ += tbbox;
        }
        else {
          CBBox2D tbbox;

          bool visible = true;

          if (getSide() == Side::BOTTOM_LEFT) {
            Qt::Alignment align = Qt::AlignVCenter | Qt::AlignRight;

            QPointF pt(ax3 - tlen2 - tgap, ppy);

            if (CQChartsUtil::isZero(angle)) {
              double atw = plot->pixelToWindowWidth (tw);
              double ath = plot->pixelToWindowHeight(ta + td);
              double atm = plot->pixelToWindowWidth (tlen2 + tgap);

              lmin = std::min(lmin, pt.x() - tw);

              tbbox = CBBox2D(apos - atw - atm, pos1 - ath/2, apos - atm, pos1 + ath/2);
            }
            else {
              QRectF rrect = CQRotatedText::bbox(pt.x(), pt.y(), text, p->font(),
                                                 angle, 0, align, /*alignBox*/true);

              lmin = std::min(lmin, rrect.left());

              plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
            }

            if (isTickLabelAutoHide()) {
              if (lastTickLabelRect.isSet() && lastTickLabelRect.overlaps(tbbox))
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

              lastTickLabelRect = tbbox;
            }
          }
          else {
            Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft;

            QPointF pt(ax3 + tlen2 + tgap, ppy);

            if (CQChartsUtil::isZero(angle)) {
              double atw = plot->pixelToWindowWidth (tw);
              double ath = plot->pixelToWindowHeight(ta + td);
              double atm = plot->pixelToWindowWidth (tlen2 + tgap);

              lmax = std::max(lmax, ax3 + tlen2 + tgap + tw);

              tbbox = CBBox2D(apos + atm, pos1 - ath/2, apos + atw + atm, pos1 + ath/2);
            }
            else {
              QRectF rrect = CQRotatedText::bbox(pt.x(), pt.y(), text, p->font(),
                                                 angle, 0, align, /*alignBox*/true);

              lmax = std::max(lmax, rrect.right());

              plot->pixelToWindow(CQChartsUtil::fromQRect(rrect), tbbox);
            }

            if (isTickLabelAutoHide()) {
              if (lastTickLabelRect.isSet() && lastTickLabelRect.overlaps(tbbox))
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

              lastTickLabelRect = tbbox;
            }
          }

          bbox_ += tbbox;
        }
      }
    }

    //---

    pos1 += inc;
  }

  if (direction_ == Direction::HORIZONTAL) {
    if (getSide() == Side::BOTTOM_LEFT) {
      if (lmax == INT_MIN)
        lmax = ay3 + tlen2 + tgap;
    }
    else {
      if (lmin == INT_MAX)
        lmin = ay3 - tlen2 - tgap;
    }
  }
  else {
    if (getSide() == Side::BOTTOM_LEFT) {
      if (lmin == INT_MAX)
        lmin = ax3 - tlen2 - tgap;
    }
    else {
      if (lmax == INT_MIN)
        lmax = ax3 + tlen2 + tgap;
    }
  }

  //---

  if (isLabelDisplayed()) {
    QString text = getLabel();

    if (text.length()) {
      p->setPen (getLabelColor());
      p->setFont(getLabelFont ());

      QFontMetricsF fm(p->font());

      double tw = fm.width(text);
      double ta = fm.ascent();
      double td = fm.descent();

      // draw label
      if (direction_ == Direction::HORIZONTAL) {
        double axm = (ax1 + ax2)/2 - tw/2;

        if (getSide() == Side::BOTTOM_LEFT) {
          double atw = plot->pixelToWindowWidth (tw/2);
          double ath = plot->pixelToWindowHeight((lmax - ay3) + tgap + ta + td);

          p->drawText(axm, lmax + ta + tgap, text);

          bbox_ += CPoint2D((amin + amax)/2 - atw, apos - ath);
          bbox_ += CPoint2D((amin + amax)/2 + atw, apos - ath);
        }
        else {
          double atw = plot->pixelToWindowWidth (tw/2);
          double ath = plot->pixelToWindowHeight((ay3 - lmin) + tgap + ta + td);

          p->drawText(axm, lmin - td - tgap, text);

          bbox_ += CPoint2D((amin + amax)/2 - atw, apos + ath);
          bbox_ += CPoint2D((amin + amax)/2 + atw, apos + ath);
        }
      }
      else {
        if (getSide() == Side::BOTTOM_LEFT) {
          double aym = (ay2 + ay1)/2 + tw/2;

          double atw = plot->pixelToWindowWidth ((ax3 - lmin) + tgap + ta + td);
          double ath = plot->pixelToWindowHeight(tw/2);

          CQRotatedText::drawRotatedText(p, lmin - tgap - td, aym, text, 90.0);

          bbox_ += CPoint2D(apos - atw, (amin + amax)/2 - ath);
          bbox_ += CPoint2D(apos - atw, (amin + amax)/2 + ath);
        }
        else {
          double aym = (ay2 + ay1)/2 - tw/2;

          double atw = plot->pixelToWindowWidth ((lmax - ax3) + tgap + ta + td);
          double ath = plot->pixelToWindowHeight(tw/2);

          CQRotatedText::drawRotatedText(p, lmax + tgap + td, aym, text, -90.0);

          bbox_ += CPoint2D(apos + atw, (amin + amax)/2 - ath);
          bbox_ += CPoint2D(apos + atw, (amin + amax)/2 + ath);
        }
      }
    }
  }

  //---

  if (plot_->showBoxes())
    plot_->drawWindowRedBox(p, bbox_);

  //---

  p->restore();
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
