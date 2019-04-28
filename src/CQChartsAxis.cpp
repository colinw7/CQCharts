#include <CQChartsAxis.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelUtil.h>
#include <CQChartsEditHandles.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsRotatedText.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QPainter>

#include <cstring>
#include <algorithm>

namespace {
int boolFactor(bool b) { return (b ? 1 : -1); }
}

//------

CQChartsAxis::
CQChartsAxis(const CQChartsPlot *plot, Qt::Orientation direction, double start, double end) :
 CQChartsObj(const_cast<CQChartsPlot *>(plot)),
 CQChartsObjAxesLineData         <CQChartsAxis>(this),
 CQChartsObjAxesTickLabelTextData<CQChartsAxis>(this),
 CQChartsObjAxesLabelTextData    <CQChartsAxis>(this),
 CQChartsObjAxesMajorGridLineData<CQChartsAxis>(this),
 CQChartsObjAxesMinorGridLineData<CQChartsAxis>(this),
 CQChartsObjAxesGridFillData     <CQChartsAxis>(this),
 plot_(plot), direction_(direction),
 start_(std::min(start, end)), end_(std::max(start, end)), calcStart_(start), calcEnd_(end)
{
  setObjectName("axis");

  editHandles_ = new CQChartsEditHandles(plot, CQChartsEditHandles::Mode::MOVE);

  CQChartsColor themeFg   (CQChartsColor::Type::INTERFACE_VALUE, 1);
  CQChartsColor themeGray1(CQChartsColor::Type::INTERFACE_VALUE, 0.7);
  CQChartsColor themeGray2(CQChartsColor::Type::INTERFACE_VALUE, 0.3);
  CQChartsColor themeGray3(CQChartsColor::Type::INTERFACE_VALUE, 0.3);

  setAxesLabelTextColor    (themeFg);
  setAxesTickLabelTextColor(themeFg);

  setAxesLinesColor(themeGray1);

  // init grid
  setAxesMajorGridLines     (false);
  setAxesMajorGridLinesColor(themeGray2);
  setAxesMajorGridLinesDash (CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  setAxesMinorGridLines     (false);
  setAxesMinorGridLinesColor(themeGray2);
  setAxesMinorGridLinesDash (CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  setAxesGridFilled   (false);
  setAxesGridFillColor(themeGray3);
  setAxesGridFillAlpha(0.5);

  calc();
}

CQChartsAxis::
~CQChartsAxis()
{
  delete editHandles_;
}

CQCharts *
CQChartsAxis::
charts() const
{
  return view()->charts();
}

QString
CQChartsAxis::
calcId() const
{
  if (direction_ == Qt::Horizontal)
    return plot()->id() + "/xaxis";
  else
    return plot()->id() + "/yaxis";
}

CQChartsView *
CQChartsAxis::
view()
{
  return plot()->view();
}

const CQChartsView *
CQChartsAxis::
view() const
{
  return plot()->view();
}

void
CQChartsAxis::
setVisible(bool b)
{
  CQChartsUtil::testAndSet(visible_, b, [&]() { redraw(); } );
}

void
CQChartsAxis::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() { emitSelectionChanged(); } );
}

void
CQChartsAxis::
emitSelectionChanged()
{
  emit selectionChanged();
}

void
CQChartsAxis::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"  )->setDesc("Is visible");
  model->addProperty(path, this, "direction")->setDesc("Direction").setHidden(true);
  model->addProperty(path, this, "side"     )->setDesc("Side");
  model->addProperty(path, this, "integral" )->setDesc("Is integral");
  model->addProperty(path, this, "date"     )->setDesc("Is date");
  model->addProperty(path, this, "log"      )->setDesc("Is log values");
  model->addProperty(path, this, "format"   )->setDesc("Format string");

  model->addProperty(path, this, "tickIncrement" )->setDesc("Tick increment");
  model->addProperty(path, this, "majorIncrement")->setDesc("Tick major increment");
  model->addProperty(path, this, "start"         )->setDesc("Start position");
  model->addProperty(path, this, "end"           )->setDesc("End position");
  model->addProperty(path, this, "includeZero"   )->setDesc("Force include zero");

  QString posPath = path + "/position";

  model->addProperty(posPath, this, "position", "value")->setDesc("Position");

  QString linePath = path + "/line";

  model->addProperty(linePath, this, "axesLineData"  , "style"  )->setDesc("Axis line style");
  model->addProperty(linePath, this, "axesLines"     , "visible")->setDesc("Axis line is visible");
  model->addProperty(linePath, this, "axesLinesColor", "color"  )->setDesc("Axis line color");
  model->addProperty(linePath, this, "axesLinesAlpha", "alpha"  )->setDesc("Axis line alpha");
  model->addProperty(linePath, this, "axesLinesWidth", "width"  )->setDesc("Axis line width");
  model->addProperty(linePath, this, "axesLinesDash" , "dash"   )->setDesc("Axis line dash");

  QString ticksPath = path + "/ticks";

  QString majorTicksPath = ticksPath + "/major";
  QString minorTicksPath = ticksPath + "/minor";

  model->addProperty(majorTicksPath, this, "majorTicksDisplayed", "visible")->
    setDesc("Major ticks visible");
  model->addProperty(majorTicksPath, this, "majorTickLen"       , "length" )->
    setDesc("Major ticks pixel length");
  model->addProperty(minorTicksPath, this, "minorTicksDisplayed", "visible")->
    setDesc("Minor ticks visible");
  model->addProperty(minorTicksPath, this, "minorTickLen"       , "length" )->
    setDesc("Minor ticks pixel length");

  QString ticksLabelPath = ticksPath + "/label";

  model->addProperty(ticksLabelPath, this, "axesTickLabelTextData"   , "style")->
    setDesc("Axis tick label text style");
  model->addProperty(ticksLabelPath, this, "axesTickLabelTextVisible", "visible")->
    setDesc("Axis tick label text is visible");
  model->addProperty(ticksLabelPath, this, "axesTickLabelTextColor"  , "color")->
    setDesc("Axis tick label text color");
  model->addProperty(ticksLabelPath, this, "axesTickLabelTextAlpha"  , "alpha")->
    setDesc("Axis tick label text alpha");
  model->addProperty(ticksLabelPath, this, "axesTickLabelTextFont"   , "font")->
    setDesc("Axis tick label text font");
  model->addProperty(ticksLabelPath, this, "axesTickLabelTextAngle"  , "angle")->
    setDesc("Axis tick label text angle");
  model->addProperty(ticksLabelPath, this, "tickLabelAutoHide"       , "autoHide")->
    setDesc("Axis tick label text is auto hide");
  model->addProperty(ticksLabelPath, this, "tickLabelPlacement"      , "placement")->
    setDesc("Axis tick label text placement");

  model->addProperty(ticksPath, this, "tickInside" , "inside")->setDesc("Tick is inside");
  model->addProperty(ticksPath, this, "mirrorTicks", "mirror")->setDesc("Mirror tick position");

  QString labelPath = path + "/label";

  model->addProperty(labelPath, this, "label"               , "text"   )->
    setDesc("Label text string");
  model->addProperty(labelPath, this, "axesLabelTextData"   , "style"  )->
    setDesc("Label text style");
  model->addProperty(labelPath, this, "axesLabelTextVisible", "visible")->
    setDesc("Label text is visible");
  model->addProperty(labelPath, this, "axesLabelTextColor"  , "color"  )->
    setDesc("Label text color");
  model->addProperty(labelPath, this, "axesLabelTextAlpha"  , "alpha"  )->
    setDesc("Label text alpha");
  model->addProperty(labelPath, this, "axesLabelTextFont"   , "font"   )->
    setDesc("Label text font");

  QString gridPath          = path + "/grid";
  QString gridLinePath      = gridPath + "/line";
  QString gridMajorLinePath = gridLinePath + "/major";
  QString gridMinorLinePath = gridLinePath + "/minor";
  QString gridFillPath      = gridPath + "/fill";

  model->addProperty(gridPath, this, "gridMid"  , "middle")->setDesc("Grid at mid point");
  model->addProperty(gridPath, this, "gridAbove", "above" )->setDesc("Grid above axes");

  model->addProperty(gridMajorLinePath, this, "axesMajorGridLineData"  , "style"  )->
    setDesc("Axis major grid line style");
  model->addProperty(gridMajorLinePath, this, "axesMajorGridLines"     , "visible")->
    setDesc("Axis major grid line is visible");
  model->addProperty(gridMajorLinePath, this, "axesMajorGridLinesColor", "color"  )->
    setDesc("Axis major grid line color");
  model->addProperty(gridMajorLinePath, this, "axesMajorGridLinesAlpha", "alpha"  )->
    setDesc("Axis major grid line alpha");
  model->addProperty(gridMajorLinePath, this, "axesMajorGridLinesWidth", "width"  )->
    setDesc("Axis major grid line width");
  model->addProperty(gridMajorLinePath, this, "axesMajorGridLinesDash" , "dash"   )->
    setDesc("Axis major grid line dash");

  model->addProperty(gridMinorLinePath, this, "axesMinorGridLineData"  , "style"  )->
    setDesc("Axis minor grid line style");
  model->addProperty(gridMinorLinePath, this, "axesMinorGridLines"     , "visible")->
    setDesc("Axis minor grid line is visible");
  model->addProperty(gridMinorLinePath, this, "axesMinorGridLinesColor", "color"  )->
    setDesc("Axis minor grid line color");
  model->addProperty(gridMinorLinePath, this, "axesMinorGridLinesAlpha", "alpha"  )->
    setDesc("Axis minor grid line alpha");
  model->addProperty(gridMinorLinePath, this, "axesMinorGridLinesWidth", "width"  )->
    setDesc("Axis minor grid line width");
  model->addProperty(gridMinorLinePath, this, "axesMinorGridLinesDash" , "dash"   )->
    setDesc("Axis minor grid line  dash");

  model->addProperty(gridFillPath, this, "axesGridFillData"   , "style"  )->
    setDesc("Axis grid fill style");
  model->addProperty(gridFillPath, this, "axesGridFilled"     , "visible")->
    setDesc("Axis grid fill is visible");
  model->addProperty(gridFillPath, this, "axesGridFillColor"  , "color"  )->
    setDesc("Axis grid fill color");
  model->addProperty(gridFillPath, this, "axesGridFillAlpha"  , "alpha"  )->
    setDesc("Axis grid fill alpha");
  model->addProperty(gridFillPath, this, "axesGridFillPattern", "pattern")->
    setDesc("Axis grid fill pattern");
}

void
CQChartsAxis::
setRange(double start, double end)
{
  start_ = std::min(start, end);
  end_   = std::max(start, end);

  calcAndRedraw();
}

void
CQChartsAxis::
setMajorIncrement(double i)
{
  CQChartsUtil::testAndSet(majorIncrement_, i, [&]() { calcAndRedraw(); } );
}

void
CQChartsAxis::
setTickIncrement(uint i)
{
  CQChartsUtil::testAndSet(tickIncrement_, i, [&]() { calcAndRedraw(); } );
}

double
CQChartsAxis::
majorTickIncrement() const
{
  return calcIncrement();
}

double
CQChartsAxis::
minorTickIncrement() const
{
  return majorTickIncrement()/numMinorTicks();
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

void
CQChartsAxis::
setPosition(const CQChartsOptReal &r)
{
  CQChartsUtil::testAndSet(position_, r, [&]() { redraw(); } );
}

//---

void
CQChartsAxis::
setColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(column_, c, [&]() { redraw(); } );
}

void
CQChartsAxis::
setDataLabels(bool b)
{
  CQChartsUtil::testAndSet(dataLabels_, b, [&]() { redraw(); } );
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

    if (plot()->columnTypeStr(column(), typeStr))
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
    CQChartsPlot *plot = const_cast<CQChartsPlot *>(plot_);

    if (! plot->setColumnTypeStr(column(), typeStr))
      return false;
  }

  return true;
}

//---

const QString &
CQChartsAxis::
label() const
{
  return label_;
}

void
CQChartsAxis::
setLabel(const QString &str)
{
  CQChartsUtil::testAndSet(label_, str, [&]() { redraw(); } );
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

void
CQChartsAxis::
setIntegral(bool b)
{
  CQChartsUtil::testAndSet(integral_, b, [&]() { calcAndRedraw(); } );
}

void
CQChartsAxis::
setDate(bool b)
{
  CQChartsUtil::testAndSet(date_, b, [&]() { calcAndRedraw(); } );
}

void
CQChartsAxis::
setLog(bool b)
{
  CQChartsUtil::testAndSet(log_, b, [&]() { calcAndRedraw(); } );
}

//---

void
CQChartsAxis::
calcAndRedraw()
{
  calc();

  redraw();
}

void
CQChartsAxis::
calc()
{
  interval_.setStart(start());
  interval_.setEnd  (end  ());

  interval_.setIntegral(isIntegral());
  interval_.setDate    (isDate    ());

  interval_.setMajorIncrement(majorIncrement());
  interval_.setTickIncrement (tickIncrement ());

  numMajorTicks_ = std::max(interval_.calcNumMajor(), 1);
  numMinorTicks_ = std::max(interval_.calcNumMinor(), 1);
  calcIncrement_ = interval_.calcIncrement();
  calcStart_     = interval_.calcStart    ();
  calcEnd_       = interval_.calcEnd      ();
//std::cerr << "numMajorTicks: " << numMajorTicks_  << "\n";
//std::cerr << "numMinorTicks: " << numMinorTicks_  << "\n";
//std::cerr << "calcIncrement: " << calcIncrement() << "\n";
//std::cerr << "calcStart    : " << calcStart()     << "\n";
//std::cerr << "calcEnd      : " << calcEnd()       << "\n";

  emit ticksChanged();
}

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
  return valueStr(plot(), pos);
}

QString
CQChartsAxis::
valueStr(const CQChartsPlot *plot, double pos) const
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

    if (CQChartsModelUtil::formatColumnTypeValue(plot->charts(), plot->model().data(),
                                                 column(), formatStr_, pos, str))
      return str;
  }

  if (column().isValid()) {
    QString str;

    if (CQChartsModelUtil::formatColumnValue(plot->charts(), plot->model().data(),
                                             column(), pos, str))
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
  CQChartsPlot *plot = const_cast<CQChartsPlot *>(plot_);

  plot->updateMargins();
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
redraw(bool wait)
{
  CQChartsPlot *plot = const_cast<CQChartsPlot *>(plot_);
  if (! plot) return;

  if (wait) {
    plot->drawBackground();
    plot->drawForeground();
  }
  else {
    plot->invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
    plot->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
  }
}

void
CQChartsAxis::
updatePlotRange()
{
  CQChartsPlot *plot = const_cast<CQChartsPlot *>(plot_);

  plot->updateRange();
}

//---

bool
CQChartsAxis::
editPress(const CQChartsGeom::Point &p)
{
  editHandles_->setDragPos(p);

  double apos1, apos2;

  calcPos(plot(), apos1, apos2);

  setPosition(CQChartsOptReal(apos1));

  return true;
}

bool
CQChartsAxis::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point &dragPos = editHandles_->dragPos();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  double apos;

  if (direction_ == Qt::Horizontal)
    apos = position().realOr(0.0) + dy;
  else
    apos = position().realOr(0.0) + dx;

  setPosition(CQChartsOptReal(apos));

  editHandles_->setDragPos(p);

  redraw(/*wait*/false);

  return true;
}

bool
CQChartsAxis::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_->selectInside(p);
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

  calcPos(plot(), apos1, apos2);

  double apos;

  if (direction_ == Qt::Horizontal)
    apos = apos1 + d.y();
  else
    apos = apos1 + d.x();

  setPosition(CQChartsOptReal(apos));

  redraw(/*wait*/false);
}

//---

bool
CQChartsAxis::
isDrawGrid() const
{
  return (isAxesMajorGridLines() || isAxesMinorGridLines() || isAxesGridFilled());
}

void
CQChartsAxis::
drawGrid(const CQChartsPlot *plot, QPainter *painter)
{
  if (! isDrawGrid())
    return;

  //---

  CQChartsGeom::BBox dataRange = plot->calcDataRange();

  double amin, amax, dmin, dmax;

  CQChartsGeom::Point a1, a2;

  if (direction_ == Qt::Horizontal) {
    amin = start();
    amax = end  ();

    dmin = dataRange.getYMin();
    dmax = dataRange.getYMax();

    a1 = plot->windowToPixel(CQChartsGeom::Point(amin, dmin));
    a2 = plot->windowToPixel(CQChartsGeom::Point(amax, dmax));
  }
  else {
    amin = start();
    amax = end  ();

    dmin = dataRange.getXMin();
    dmax = dataRange.getXMax();

    a1 = plot->windowToPixel(CQChartsGeom::Point(dmin, amin));
    a2 = plot->windowToPixel(CQChartsGeom::Point(dmax, amax));
  }

  //---

  painter->save();

  //---

  double inc  = calcIncrement();
  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  //---

  // draw fill
  if (isAxesGridFilled()) {
    QRectF dataRect = CQChartsUtil::toQRect(plot->calcDataPixelRect());

    painter->setClipRect(dataRect);

    //---

    QBrush brush;

    QColor fillColor = interpAxesGridFillColor(0, 1);

    plot->setBrush(brush, true, fillColor, axesGridFillAlpha(), axesGridFillPattern());

    //---

    if (numMajorTicks() < maxMajorTicks()) {
      double pos1;

      if (isDate()) {
        pos1 = interval_.interval(0);

        if (isGridMid())
          pos1 = (pos1 + interval_.interval(1))/2;
      }
      else {
        pos1 = calcStart();

        if (isGridMid())
          pos1 += inc/2.0;
      }

      double pos2 = pos1;

      for (uint i = 0; i < numMajorTicks() + 1; i++) {
        // fill on alternate gaps
        if (i & 1) {
          if (pos2 >= amin || pos1 <= amax) {
            double pos3 = std::max(pos1, amin);
            double pos4 = std::min(pos2, amax);

            CQChartsGeom::Point pp1 = plot->windowToPixel(CQChartsGeom::Point(pos3, pos1));
            CQChartsGeom::Point pp2 = plot->windowToPixel(CQChartsGeom::Point(pos4, pos2));

            CQChartsGeom::BBox bbox;

            if (direction_ == Qt::Horizontal)
              bbox = CQChartsGeom::BBox(pp1.x, a1.y, pp2.x, a2.y);
            else
              bbox = CQChartsGeom::BBox(a1.x, pp1.y, a2.x, pp2.y);

            painter->fillRect(CQChartsUtil::toQRect(bbox), brush);
          }
        }

        //---

        pos1 = pos2;

        if (isDate())
          pos2 = interval_.interval(i + 1);
        else
          pos2 = pos1 + inc;
      }
    }
  }

  //---

  // draw grid lines
  if (isAxesMajorGridLines() || isAxesMinorGridLines()) {
    if (numMajorTicks() < maxMajorTicks()) {
      double pos1;

      if (isDate()) {
        pos1 = interval_.interval(0);

        if (isGridMid())
          pos1 = (pos1 + interval_.interval(1))/2;
      }
      else {
        pos1 = calcStart();

        if (isGridMid())
          pos1 += inc/2.0;
      }

      for (uint i = 0; i < numMajorTicks() + 1; i++) {
        // draw major line (grid and tick)
        if (pos1 >= amin && pos1 <= amax) {
          // draw major grid line if major or minor displayed
          if      (isAxesMajorGridLines())
            drawMajorGridLine(plot, painter, pos1, dmin, dmax);
          else if (isAxesMinorGridLines())
            drawMinorGridLine(plot, painter, pos1, dmin, dmax);
        }

        if (isAxesMinorGridLines()) {
          for (uint j = 1; j < numMinorTicks(); j++) {
            double pos2 = pos1 + (isLog() ? plot->logValue(j*inc1) : j*inc1);

            if (isIntegral() && ! CMathUtil::isInteger(pos2))
              continue;

            // draw minor grid line
            if (pos2 >= amin && pos2 <= amax)
              drawMinorGridLine(plot, painter, pos2, dmin, dmax);
          }
        }

        //---

        if (isDate())
          pos1 = interval_.interval(i + 1);
        else
          pos1 += inc;
      }
    }
  }

  //---

  painter->restore();
}

void
CQChartsAxis::
draw(const CQChartsPlot *plot, QPainter *painter)
{
  fitBBox_ = CQChartsGeom::BBox();
  bbox_    = CQChartsGeom::BBox();

  //---

  double apos1, apos2;

  calcPos(plot, apos1, apos2);

  double amin, amax;

  if (direction_ == Qt::Horizontal) {
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
  if (isAxesLines()) {
    drawLine(plot, painter, apos1, amin, amax);
  }

  //---

  double inc  = calcIncrement();
  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  //---

  double pos1;

  if (isDate())
    pos1 = interval_.interval(0);
  else
    pos1 = calcStart();

  int tlen2 = majorTickLen();
  int tgap  = 2;

  //---

  lbbox_ = CQChartsGeom::BBox();

  //---

  lastTickLabelRect_ = CQChartsGeom::BBox();

  double dt = (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN ? -0.5 : 0.0);

  if (numMajorTicks() < maxMajorTicks()) {
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

      if (isAxesTickLabelTextVisible()) {
        // draw major tick label
        if (pos1 >= amin && pos1 <= amax) {
          drawTickLabel(plot, painter, apos1, pos1, isTickInside());
        }
      }

      //---

      if (isDate())
        pos1 = interval_.interval(i + 1);
      else
        pos1 += inc;
    }
  }

  //---

  // fix range if not set
  if (! lbbox_.isSet()) {
    if (direction_ == Qt::Horizontal) {
      CQChartsGeom::Point a1 = plot->windowToPixel(CQChartsGeom::Point(amin, apos1));
      CQChartsGeom::Point a2 = plot->windowToPixel(CQChartsGeom::Point(amax, apos1));

      bool isPixelBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                           (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

      double dys = (isPixelBottom ? 1 : -1);

      a2.y += dys*(tlen2 + tgap);

      lbbox_ += CQChartsGeom::Point(a1.x, a1.y);
      lbbox_ += CQChartsGeom::Point(a2.x, a2.y);
    }
    else {
      CQChartsGeom::Point a1 = plot->windowToPixel(CQChartsGeom::Point(apos1, amin));
      CQChartsGeom::Point a2 = plot->windowToPixel(CQChartsGeom::Point(apos1, amax));

      bool isPixelLeft = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertX()) ||
                         (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertX());

      double dxs = (isPixelLeft ? 1 : -1);

      a2.x += dxs*(tlen2 + tgap);

      lbbox_ += CQChartsGeom::Point(a1.x, a1.y);
      lbbox_ += CQChartsGeom::Point(a2.x, a2.y);
    }
  }

  //---

  if (isAxesLabelTextVisible()) {
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
drawEditHandles(QPainter *painter) const
{
  assert(view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  CQChartsAxis *th = const_cast<CQChartsAxis *>(this);

  th->editHandles_->setBBox(this->bbox());

  editHandles_->draw(painter);
}

void
CQChartsAxis::
getTickLabelsPositions(std::set<int> &positions) const
{
  if (numMajorTicks() >= maxMajorTicks())
    return;

  double pos;

  if (isDate())
    pos = interval_.interval(0);
  else
    pos = calcStart();

  double inc = calcIncrement();

  for (uint i = 0; i < numMajorTicks() + 1; i++) {
    positions.insert(int(pos));

    if (isDate())
      pos = interval_.interval(i + 1);
    else
      pos += inc;
  }
}

void
CQChartsAxis::
calcPos(const CQChartsPlot *plot, double &apos1, double &apos2) const
{
  if (position().isSet()) {
    apos1 = position().real();
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

  if (direction_ == Qt::Horizontal) {
    bool isWindowBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT);
    //bool isPixelBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
    //                     (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    apos1 = (isWindowBottom ? ymin : ymax);
    apos2 = (isWindowBottom ? ymax : ymin);
  }
  else {
    bool isWindowLeft = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT);
    //bool isPixelLeft = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertX()) ||
    //                   (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertX());

    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    apos1 = (isWindowLeft ? xmin : xmax);
    apos2 = (isWindowLeft ? xmax : xmin);
  }
}

void
CQChartsAxis::
drawLine(const CQChartsPlot *plot, QPainter *painter, double apos, double amin, double amax)
{
  QPen pen;

  QColor lc = interpAxesLinesColor(0, 1);

  plot_->setPen(pen, true, lc, axesLinesAlpha(), axesLinesWidth(), axesLinesDash());

  painter->setPen(pen);

  //---

  if (direction_ == Qt::Horizontal) {
    CQChartsGeom::Point a1 = plot->windowToPixel(CQChartsGeom::Point(amin, apos));
    CQChartsGeom::Point a2 = plot->windowToPixel(CQChartsGeom::Point(amax, apos));

    painter->drawLine(QPointF(a1.x, a1.y), QPointF(a2.x, a1.y));
  }
  else {
    CQChartsGeom::Point a1 = plot->windowToPixel(CQChartsGeom::Point(apos, amin));
    CQChartsGeom::Point a2 = plot->windowToPixel(CQChartsGeom::Point(apos, amax));

    painter->drawLine(QPointF(a1.x, a1.y), QPointF(a1.x, a2.y));
  }
}

void
CQChartsAxis::
drawMajorGridLine(const CQChartsPlot *plot, QPainter *painter, double apos,
                  double dmin, double dmax)
{
  QPen pen;

  QColor lc = interpAxesMajorGridLinesColor(0, 1);

  plot_->setPen(pen, true, lc, axesMajorGridLinesAlpha(),
                axesMajorGridLinesWidth(), axesMajorGridLinesDash());

  painter->setPen(pen);

  //---

  if (direction_ == Qt::Horizontal) {
    CQChartsGeom::Point a1 = plot->windowToPixel(CQChartsGeom::Point(apos, dmin));
    CQChartsGeom::Point a2 = plot->windowToPixel(CQChartsGeom::Point(apos, dmax));

    painter->drawLine(QPointF(a1.x, a1.y), QPointF(a1.x, a2.y));
  }
  else {
    CQChartsGeom::Point a1 = plot->windowToPixel(CQChartsGeom::Point(dmin, apos));
    CQChartsGeom::Point a2 = plot->windowToPixel(CQChartsGeom::Point(dmax, apos));

    painter->drawLine(QPointF(a1.x, a1.y), QPointF(a2.x, a1.y));
  }
}

void
CQChartsAxis::
drawMinorGridLine(const CQChartsPlot *plot, QPainter *painter, double apos,
                  double dmin, double dmax)
{
  QPen pen;

  QColor lc = interpAxesMinorGridLinesColor(0, 1);

  plot_->setPen(pen, true, lc, axesMinorGridLinesAlpha(),
                axesMinorGridLinesWidth(), axesMinorGridLinesDash());

  painter->setPen(pen);

  //---

  if (direction_ == Qt::Horizontal) {
    CQChartsGeom::Point a1 = plot->windowToPixel(CQChartsGeom::Point(apos, dmin));
    CQChartsGeom::Point a2 = plot->windowToPixel(CQChartsGeom::Point(apos, dmax));

    painter->drawLine(QPointF(a1.x, a1.y), QPointF(a1.x, a2.y));
  }
  else {
    CQChartsGeom::Point a1 = plot->windowToPixel(CQChartsGeom::Point(dmin, apos));
    CQChartsGeom::Point a2 = plot->windowToPixel(CQChartsGeom::Point(dmax, apos));

    painter->drawLine(QPointF(a1.x, a1.y), QPointF(a2.x, a1.y));
  }
}

void
CQChartsAxis::
drawMajorTickLine(const CQChartsPlot *plot, QPainter *painter, double apos,
                  double tpos, bool inside)
{
  drawTickLine(plot, painter, apos, tpos, inside, /*major*/true);
}

void
CQChartsAxis::
drawMinorTickLine(const CQChartsPlot *plot, QPainter *painter, double apos,
                  double tpos, bool inside)
{
  drawTickLine(plot, painter, apos, tpos, inside, /*major*/false);
}

void
CQChartsAxis::
drawTickLine(const CQChartsPlot *plot, QPainter *painter, double apos, double tpos,
             bool inside, bool major)
{
  int tlen = (major ? majorTickLen() : minorTickLen());

  CQChartsGeom::Point pp;

  if (major && tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN) {
    if (direction_ == Qt::Horizontal)
      pp = plot->windowToPixel(CQChartsGeom::Point(tpos - 0.5, apos));
    else
      pp = plot->windowToPixel(CQChartsGeom::Point(apos, tpos - 0.5));
  }
  else {
    if (direction_ == Qt::Horizontal)
      pp = plot->windowToPixel(CQChartsGeom::Point(tpos, apos));
    else
      pp = plot->windowToPixel(CQChartsGeom::Point(apos, tpos));
  }

  //---

  QPen pen;

  QColor lc = interpAxesLinesColor(0, 1);

  plot_->setPen(pen, true, lc, axesLinesAlpha(), axesLinesWidth(), axesLinesDash());

  painter->setPen(pen);

  //---

  if (direction_ == Qt::Horizontal) {
    bool isWindowBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT);
    bool isPixelBottom  = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                          (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

    int pys = (isPixelBottom ? 1 : -1);
    int dt1 = pys*tlen;

    double adt1 = plot->pixelToWindowHeight(dt1);

    if (inside)
      painter->drawLine(QPointF(pp.x, pp.y), QPointF(pp.x, pp.y - dt1));
    else {
      painter->drawLine(QPointF(pp.x, pp.y), QPointF(pp.x, pp.y + dt1));

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
    bool isWindowLeft = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT);
    bool isPixelLeft  = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertX()) ||
                        (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertX());

    int pxs = (isPixelLeft ? -1 : 1);
    int dt1 = pxs*tlen;

    double adt1 = plot->pixelToWindowWidth(dt1);

    if (inside)
      painter->drawLine(QPointF(pp.x, pp.y), QPointF(pp.x - dt1, pp.y));
    else {
      painter->drawLine(QPointF(pp.x, pp.y), QPointF(pp.x + dt1, pp.y));

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
drawTickLabel(const CQChartsPlot *plot, QPainter *painter, double apos, double tpos, bool inside)
{
  int tgap  = 2;
  int tlen1 = majorTickLen();
  int tlen2 = minorTickLen();

  CQChartsGeom::Point pp;

  if (direction_ == Qt::Horizontal)
    pp = plot->windowToPixel(CQChartsGeom::Point(tpos, apos));
  else
    pp = plot->windowToPixel(CQChartsGeom::Point(apos, tpos));

  QString text = valueStr(plot, tpos);

  if (! text.length())
    return;

  //---

  QPen tpen;

  QColor tc = interpAxesTickLabelTextColor(0, 1);

  plot->setPen(tpen, true, tc, axesTickLabelTextAlpha());

  painter->setPen(tpen);

  view()->setPlotPainterFont(plot, painter, axesTickLabelTextFont());

  QFontMetricsF fm(painter->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  double angle = axesTickLabelTextAngle();

  if (direction_ == Qt::Horizontal) {
    bool isPixelBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                         (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

    double tyo = 0.0;

    if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
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

      QPointF pt(pp.x, pp.y + tyo);

      if (CMathUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
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
        lbbox_ += CQChartsGeom::Point(pt.x(), pt.y() + (ta + td));

        double xpos = 0.0;
        double ypos = apos - boolFactor(! plot_->isInvertY())*(wth + atm);

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          xpos = tpos - atw/2;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          xpos = tpos - atw;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          xpos = tpos;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          xpos = tpos - 0.5;

        if (! plot_->isInvertY())
          tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = CQChartsGeom::BBox(xpos, ypos - wth, xpos + atw, ypos);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        tbbox = plot->pixelToWindow(CQChartsUtil::fromQRect(rrect));
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CMathUtil::isZero(angle)) {
          double ty = pt.y() + ta;

          QPointF p;

          if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
            p = QPointF(pt.x() - tw/2                         , ty);
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
            p = QPointF(pt.x() - tw                           , ty);
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
            p = QPointF(pt.x()                                , ty);
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
            p = QPointF(pt.x() - plot->windowToPixelWidth(0.5), ty);

          CQChartsDrawUtil::drawSimpleText(painter, p, text);
        }
        else {
          CQChartsRotatedText::draw(painter, pt.x(), pt.y(), text, angle, align,
                                    /*alignBox*/true, isAxesTickLabelTextContrast());
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

      QPointF pt(pp.x, pp.y - tyo);

      if (CMathUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
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
        double ypos = apos + boolFactor(! plot_->isInvertY())*atm;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          xpos = tpos - atw/2;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          xpos = tpos - atw;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          xpos = tpos;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          xpos = tpos - 0.5;

        if (! plot_->isInvertY())
          tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = CQChartsGeom::BBox(xpos, ypos - wth, xpos + atw, ypos);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        tbbox = plot->pixelToWindow(CQChartsUtil::fromQRect(rrect));
      }

      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }

      if (visible) {
        if (CMathUtil::isZero(angle)) {
          double ty = pt.y() - td;

          QPointF p;

          if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
            p = QPointF(pt.x() - tw/2                         , ty);
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
            p = QPointF(pt.x() - tw                           , ty);
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
            p = QPointF(pt.x()                                , ty);
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
            p = QPointF(pt.x() - plot->windowToPixelWidth(0.5), ty);

          CQChartsDrawUtil::drawSimpleText(painter, p, text);
        }
        else {
          CQChartsRotatedText::draw(painter, pt.x(), pt.y(), text, angle, align,
                                    /*alignBox*/true, isAxesTickLabelTextContrast());
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
    bool isPixelLeft = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertX()) ||
                       (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertX());

    double txo = 0.0;

    if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
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

      QPointF pt(pp.x - txo, pp.y);

      if (CMathUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
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

        double xpos = apos - boolFactor(! plot_->isInvertX())*(atw + atm);
        double ypos = 0.0;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          ypos = tpos - wth/2;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          ypos = tpos - wth;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          ypos = tpos;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          ypos = tpos - 0.5 - wta;

        if (! plot_->isInvertX())
          tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = CQChartsGeom::BBox(xpos - atw, ypos, xpos, ypos + wth);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        tbbox = plot->pixelToWindow(CQChartsUtil::fromQRect(rrect));
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

          if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
            p = QPointF(tx, pt.y() + ta/2);
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
            p = QPointF(tx, pt.y() + ta  );
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
            p = QPointF(tx, pt.y() - td  );
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
            p = QPointF(tx, pt.y() - plot->windowToPixelHeight(0.5) + ta);

          CQChartsDrawUtil::drawSimpleText(painter, p, text);
        }
        else {
          CQChartsRotatedText::draw(painter, pt.x(), pt.y(), text, angle, align,
                                    /*alignBox*/true, isAxesTickLabelTextContrast());
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

      QPointF pt(pp.x + txo, pp.y);

      if (CMathUtil::isZero(angle)) {
        double atw = plot->pixelToWindowWidth (tw);
        double wta = plot->pixelToWindowHeight(ta);
        double wtd = plot->pixelToWindowHeight(td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
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

        double xpos = apos + boolFactor(! plot_->isInvertX())*atm;
        double ypos = 0.0;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          ypos = tpos - wth/2;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          ypos = tpos - wth;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          ypos = tpos;
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          ypos = tpos - 0.5 - wta;

        if (! plot_->isInvertX())
          tbbox = CQChartsGeom::BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = CQChartsGeom::BBox(xpos - atw, ypos, xpos, ypos + wth);
      }
      else {
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, painter->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsUtil::fromQRect(rrect);

        tbbox = plot->pixelToWindow(CQChartsUtil::fromQRect(rrect));
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

          if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
            p = QPointF(tx, pt.y() + ta/2);
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
            p = QPointF(tx, pt.y() + ta  );
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
            p = QPointF(tx, pt.y() - td  );
          else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
            p = QPointF(tx, pt.y() - plot->windowToPixelHeight(0.5) + ta);

          CQChartsDrawUtil::drawSimpleText(painter, p, text);
        }
        else {
          CQChartsRotatedText::draw(painter, pt.x(), pt.y(), text, angle, align,
                                    /*alignBox*/true, isAxesTickLabelTextContrast());
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
drawAxisLabel(const CQChartsPlot *plot, QPainter *painter, double apos,
              double amin, double amax, const QString &text)
{
  if (! text.length())
    return;

  //---

  int tgap = 2;

  CQChartsGeom::Point a1, a2, a3;

  if (direction_ == Qt::Horizontal) {
    a1 = plot->windowToPixel(CQChartsGeom::Point(amin, apos));
    a2 = plot->windowToPixel(CQChartsGeom::Point(amax, apos));
    a3 = plot->windowToPixel(CQChartsGeom::Point(amin, apos));
  }
  else {
    a1 = plot->windowToPixel(CQChartsGeom::Point(apos, amin));
    a2 = plot->windowToPixel(CQChartsGeom::Point(apos, amax));
    a3 = plot->windowToPixel(CQChartsGeom::Point(apos, amin));
  }

  //---

  QPen tpen;

  QColor tc = interpAxesLabelTextColor(0, 1);

  plot->setPen(tpen, true, tc, axesLabelTextAlpha());

  painter->setPen(tpen);

  view()->setPlotPainterFont(plot, painter, axesLabelTextFont());

  QFontMetricsF fm(painter->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  CQChartsGeom::BBox bbox;

  // draw label
  if (direction_ == Qt::Horizontal) {
    double wfh = plot->pixelToWindowHeight(ta + td);

    double axm = (a1.x + a2.x)/2 - tw/2;

    bool isPixelBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                         (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

    //int pys = (isPixelBottom ? 1 : -1);

    double ath;
    double atw = plot->pixelToWindowWidth(tw/2);

    if (isPixelBottom) {
      ath = plot->pixelToWindowHeight((lbbox_.getYMax() - a3.y) + tgap) + wfh;

      CQChartsDrawUtil::drawSimpleText(painter, QPointF(axm, lbbox_.getYMax() + ta + tgap), text);

      if (! plot_->isInvertY()) {
        bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos - (ath      ));
        bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos - (ath - wfh));
      }
      else {
        bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos + (ath      ));
        bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos + (ath - wfh));
      }

      fitBBox_ += CQChartsGeom::Point((amin + amax)/2, apos - (ath      ));
      fitBBox_ += CQChartsGeom::Point((amin + amax)/2, apos - (ath - wfh));
    }
    else {
      ath = plot->pixelToWindowHeight((a3.y - lbbox_.getYMin()) + tgap) + wfh;

      CQChartsDrawUtil::drawSimpleText(painter, QPointF(axm, lbbox_.getYMin() - td - tgap), text);

      if (! plot_->isInvertY()) {
        bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos + (ath      ));
        bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos + (ath - wfh));
      }
      else {
        bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos - (ath      ));
        bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos - (ath - wfh));
      }

      fitBBox_ += CQChartsGeom::Point((amin + amax)/2, apos + (ath      ));
      fitBBox_ += CQChartsGeom::Point((amin + amax)/2, apos + (ath - wfh));
    }
  }
  else {
    double wfa = plot->pixelToWindowWidth(ta);
    double wfd = plot->pixelToWindowWidth(td);
    double wfh = wfa + wfd;

    bool isPixelLeft = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertX()) ||
                       (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertX());

    //int pxs = (isPixelLeft ? 1 : -1);

    double atw;
    double ath = plot->pixelToWindowHeight(tw/2);

    if (isPixelLeft) {
      double aym = (a2.y + a1.y)/2 + tw/2;

      atw = plot->pixelToWindowWidth((a3.x - lbbox_.getXMin()) + tgap) + wfh;

      double tx = lbbox_.getXMin() - tgap - td;

      CQChartsRotatedText::draw(painter, tx, aym, text, 90.0, Qt::AlignLeft | Qt::AlignBottom,
                                /*alignBBox*/false, isAxesLabelTextContrast());

      if (! plot_->isInvertX()) {
        bbox += CQChartsGeom::Point(apos - (atw      ), (amin + amax)/2 - ath);
        bbox += CQChartsGeom::Point(apos - (atw - wfh), (amin + amax)/2 + ath);
      }
      else {
        bbox += CQChartsGeom::Point(apos + (atw      ), (amin + amax)/2 - ath);
        bbox += CQChartsGeom::Point(apos + (atw - wfh), (amin + amax)/2 + ath);
      }

      fitBBox_ += CQChartsGeom::Point(apos - (atw      ), (amin + amax)/2);
      fitBBox_ += CQChartsGeom::Point(apos - (atw - wfh), (amin + amax)/2);
    }
    else {
      double aym = (a2.y + a1.y)/2 - tw/2;

      atw = plot->pixelToWindowWidth((lbbox_.getXMax() - a3.x) + tgap) + wfh;

      double tx = lbbox_.getXMax() + tgap + td;

      CQChartsRotatedText::draw(painter, tx, aym, text, -90.0, Qt::AlignLeft | Qt::AlignBottom,
                                /*alignBBox*/false, isAxesLabelTextContrast());

      if (! plot_->isInvertX()) {
        bbox += CQChartsGeom::Point(apos + (atw      ), (amin + amax)/2 - ath);
        bbox += CQChartsGeom::Point(apos + (atw - wfh), (amin + amax)/2 + ath);
      }
      else {
        bbox += CQChartsGeom::Point(apos - (atw      ), (amin + amax)/2 - ath);
        bbox += CQChartsGeom::Point(apos - (atw - wfh), (amin + amax)/2 + ath);
      }

      fitBBox_ += CQChartsGeom::Point(apos + (atw      ), (amin + amax)/2);
      fitBBox_ += CQChartsGeom::Point(apos + (atw - wfh), (amin + amax)/2);
    }
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
