#include <CQChartsAxis.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelUtil.h>
#include <CQChartsEditHandles.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsRotatedText.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <cstring>
#include <algorithm>

namespace {
int boolFactor(bool b) { return (b ? 1 : -1); }
}

//------

CQChartsAxis::
CQChartsAxis(const CQChartsPlot *plot, Qt::Orientation direction, double start, double end) :
 CQChartsObj(plot->charts()),
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

  editHandles_ = std::make_unique<CQChartsEditHandles>(plot, CQChartsEditHandles::Mode::MOVE);

  CQChartsColor themeFg   (CQChartsColor::Type::INTERFACE_VALUE, 1);
  CQChartsColor themeGray1(CQChartsColor::Type::INTERFACE_VALUE, 0.7);
  CQChartsColor themeGray2(CQChartsColor::Type::INTERFACE_VALUE, 0.3);
  CQChartsColor themeGray3(CQChartsColor::Type::INTERFACE_VALUE, 0.3);

  setAxesLabelTextColor    (themeFg);
  setAxesTickLabelTextColor(themeFg);

  setAxesLinesColor(themeGray1);

  // init grid
  setAxesMajorGridLinesColor(themeGray2);
  setAxesMajorGridLinesDash (CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  setAxesMinorGridLinesColor(themeGray2);
  setAxesMinorGridLinesDash (CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  setAxesGridFillColor(themeGray3);
  setAxesGridFillAlpha(0.5);

  needsCalc_ = true;

  //---

  CQChartsFont font;

  font.decFontSize(4);

  setAxesTickLabelTextFont(font);
}

CQChartsAxis::
~CQChartsAxis()
{
}

//---

namespace {

template<typename T>
void swapT(CQChartsAxis *lhs, CQChartsAxis *rhs) {
  std::swap(*(T*)(lhs), *(T*)(rhs));
}

}
void
CQChartsAxis::
swap(CQChartsAxis *lhs, CQChartsAxis *rhs)
{
  std::swap(lhs->visible_     , rhs->visible_     );
  std::swap(lhs->side_        , rhs->side_        );
  std::swap(lhs->position_    , rhs->position_    );
  std::swap(lhs->valueType_   , rhs->valueType_   );
  std::swap(lhs->dataLabels_  , rhs->dataLabels_  );
  std::swap(lhs->column_      , rhs->column_      );
  std::swap(lhs->formatStr_   , rhs->formatStr_   );
  std::swap(lhs->maxFitExtent_, rhs->maxFitExtent_);

  std::swap(lhs->labelDisplayed_, rhs->labelDisplayed_);
  std::swap(lhs->label_         , rhs->label_         );
  std::swap(lhs->userLabel_     , rhs->userLabel_     );

  std::swap(lhs->gridLinesDisplayed_, rhs->gridLinesDisplayed_);
  std::swap(lhs->gridFillDisplayed_ , rhs->gridFillDisplayed_ );

  std::swap(lhs->gridMid_  , rhs->gridMid_  );
  std::swap(lhs->gridAbove_, rhs->gridAbove_);

  std::swap(lhs->ticksDisplayed_, rhs->ticksDisplayed_);
  std::swap(lhs->majorTickLen_  , rhs->majorTickLen_  );
  std::swap(lhs->minorTickLen_  , rhs->minorTickLen_  );
  std::swap(lhs->tickInside_    , rhs->tickInside_    );
  std::swap(lhs->mirrorTicks_   , rhs->mirrorTicks_   );

  std::swap(lhs->tickLabelAutoHide_ , rhs->tickLabelAutoHide_ );
  std::swap(lhs->tickLabelPlacement_, rhs->tickLabelPlacement_);

  std::swap(lhs->start_         , rhs->start_         );
  std::swap(lhs->end_           , rhs->end_           );
  std::swap(lhs->includeZero_   , rhs->includeZero_   );
  std::swap(lhs->maxMajorTicks_ , rhs->maxMajorTicks_ );
  std::swap(lhs->tickIncrement_ , rhs->tickIncrement_ );
  std::swap(lhs->majorIncrement_, rhs->majorIncrement_);

  std::swap(lhs->tickSpaces_      , rhs->tickSpaces_      );
  std::swap(lhs->tickLabels_      , rhs->tickLabels_      );
  std::swap(lhs->requireTickLabel_, rhs->requireTickLabel_);

  swapT<CQChartsObjAxesLineData         <CQChartsAxis>>(lhs, rhs);
  swapT<CQChartsObjAxesTickLabelTextData<CQChartsAxis>>(lhs, rhs);
  swapT<CQChartsObjAxesLabelTextData    <CQChartsAxis>>(lhs, rhs);
  swapT<CQChartsObjAxesMajorGridLineData<CQChartsAxis>>(lhs, rhs);
  swapT<CQChartsObjAxesMinorGridLineData<CQChartsAxis>>(lhs, rhs);
  swapT<CQChartsObjAxesGridFillData     <CQChartsAxis>>(lhs, rhs);
}

//---

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
  if (isHorizontal())
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc, bool hidden=false) {
    CQPropertyViewItem *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addProp(path, "direction", "", "Axis direction")->setHidden(true).setEditable(false);

  addProp(path, "visible"  , "", "Axis visible");
  addProp(path, "side"     , "", "Axis plot side");
  addProp(path, "valueType", "", "Axis value type");
  addProp(path, "format"   , "", "Axis tick value format string");

  addProp(path, "tickIncrement" , "", "Axis tick increment");
  addProp(path, "majorIncrement", "", "Axis tick major increment");
  addProp(path, "start"         , "", "Axis start position");
  addProp(path, "end"           , "", "Axis end position");
  addProp(path, "includeZero"   , "", "Axis force include zero")->setHidden(true);

  addProp(path, "maxFitExtent", "", "Axis maximum extent percent for auto fit");

  //---

  QString posPath = path + "/position";

  addProp(posPath, "position", "value", "Axis position");

  //---

  QString linePath = path + "/stroke";

  addStyleProp(linePath, "axesLineData"  , "style"  , "Axis stroke style", true);
  addStyleProp(linePath, "axesLines"     , "visible", "Axis stroke visible");
  addStyleProp(linePath, "axesLinesColor", "color"  , "Axis stroke color");
  addStyleProp(linePath, "axesLinesAlpha", "alpha"  , "Axis stroke alpha");
  addStyleProp(linePath, "axesLinesWidth", "width"  , "Axis stroke width");
  addStyleProp(linePath, "axesLinesDash" , "dash"   , "Axis stroke dash");

  //---

  QString ticksPath = path + "/ticks";

  addProp(ticksPath, "ticksDisplayed", "lines", "Axis major and/or minor ticks visible");

  QString majorTicksPath = ticksPath + "/major";
  QString minorTicksPath = ticksPath + "/minor";

  addProp(majorTicksPath, "majorTickLen", "length", "Axis major ticks pixel length");
  addProp(minorTicksPath, "minorTickLen", "length", "Axis minor ticks pixel length");

  //---

  QString ticksLabelPath     = ticksPath + "/label";
  QString ticksLabelTextPath = ticksLabelPath + "/text";

  addProp(ticksLabelPath, "tickLabelAutoHide" , "autoHide", "Axis tick label text is auto hide");
  addProp(ticksLabelPath, "tickLabelPlacement", "placement", "Axis tick label text placement");

  addStyleProp(ticksLabelTextPath, "axesTickLabelTextData"   , "style",
               "Axis tick label text style", true);
  addProp     (ticksLabelTextPath, "axesTickLabelTextVisible", "visible",
               "Axis tick label text visible");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextColor"  , "color",
               "Axis tick label text color");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextAlpha"  , "alpha",
               "Axis tick label text alpha");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextFont"   , "font",
               "Axis tick label text font");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextAngle"  , "angle",
               "Axis tick label text angle");

  addProp(ticksPath, "tickInside" , "inside", "Axis ticks drawn inside plot");
  addProp(ticksPath, "mirrorTicks", "mirror", "Axis tick are mirrored on other side of plot");

  //---

  QString labelPath     = path + "/label";
  QString labelTextPath = labelPath + "/text";

  addProp(labelTextPath, "label", "string", "Axis label text string");

  addStyleProp(labelTextPath, "axesLabelTextData"   , "style"  ,
               "Axis label text style", true);
  addProp     (labelTextPath, "axesLabelTextVisible", "visible", "Axis label text visible");
  addStyleProp(labelTextPath, "axesLabelTextColor"  , "color"  , "Axis label text color");
  addStyleProp(labelTextPath, "axesLabelTextAlpha"  , "alpha"  , "Axis label text alpha");
  addStyleProp(labelTextPath, "axesLabelTextFont"   , "font"   , "Axis label text font");

  //---

  QString gridPath            = path + "/grid";
  QString gridLinePath        = gridPath + "/stroke";
  QString gridMajorPath       = gridPath + "/major";
  QString gridMajorStrokePath = gridMajorPath + "/stroke";
  QString gridMajorFillPath   = gridMajorPath + "/fill";
  QString gridMinorPath       = gridPath + "/minor";
  QString gridMinorStrokePath = gridMinorPath + "/stroke";

  addProp(gridPath, "gridMid"  , "middle", "Grid at make tick mid point");
  addProp(gridPath, "gridAbove", "above" , "Grid is drawn above axes");

  addProp(gridPath, "gridLinesDisplayed", "lines", "Axis major and/or minor grid lines visible");
  addProp(gridPath, "gridFillDisplayed" , "fill" , "Axis major and/or minor fill visible");

  addStyleProp(gridMajorStrokePath, "axesMajorGridLineData"  , "style"  ,
               "Axis major grid stroke style", true);
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesColor", "color"  ,
               "Axis major grid stroke color");
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesAlpha", "alpha"  ,
               "Axis major grid stroke alpha");
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesWidth", "width"  ,
               "Axis major grid stroke width");
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesDash" , "dash"   ,
               "Axis major grid stroke dash");

  addStyleProp(gridMinorStrokePath, "axesMinorGridLineData"  , "style"  ,
               "Axis minor grid stroke style", true);
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesColor", "color"  ,
               "Axis minor grid stroke color");
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesAlpha", "alpha"  ,
               "Axis minor grid stroke alpha");
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesWidth", "width"  ,
               "Axis minor grid stroke width");
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesDash" , "dash"   ,
               "Axis minor grid stroke dash");

  addStyleProp(gridMajorFillPath, "axesGridFillData"   , "style"  ,
               "Axis grid fill style", true);
  addStyleProp(gridMajorFillPath, "axesGridFillColor"  , "color"  , "Axis grid fill color");
  addStyleProp(gridMajorFillPath, "axesGridFillAlpha"  , "alpha"  , "Axis grid fill alpha");
  addStyleProp(gridMajorFillPath, "axesGridFillPattern", "pattern",
               "Axis grid fill pattern", true);
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
setMajorIncrement(const CQChartsOptInt &i)
{
  CQChartsUtil::testAndSet(majorIncrement_, i, [&]() { calcAndRedraw(); } );
}

void
CQChartsAxis::
setTickIncrement(const CQChartsOptInt &i)
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

#if 0
  if (column().isValid()) {
    QString typeStr;

    if (! plot()->columnTypeStr(column(), typeStr))
      return "";

    return typeStr;
  }
#endif

  return "";
}

bool
CQChartsAxis::
setFormat(const QString &formatStr)
{
  CQChartsUtil::testAndSet(formatStr_, formatStr, [&]() {
#if 0
    if (column().isValid()) {
      CQChartsPlot *plot = const_cast<CQChartsPlot *>(plot_);

      if (! plot->setColumnTypeStr(column(), typeStr))
        return false;
    }
#endif

    redraw();
  } );

  return true;
}

//---

void
CQChartsAxis::
setLabel(const QString &str)
{
  CQChartsUtil::testAndSet(label_, str, [&]() { redraw(); } );
}

void
CQChartsAxis::
setUserLabel(const QString &str)
{
  CQChartsUtil::testAndSet(userLabel_, str, [&]() { redraw(); } );
}

//---

void
CQChartsAxis::
setGridLinesDisplayed(const GridLinesDisplayed &d)
{
  CQChartsUtil::testAndSet(gridLinesDisplayed_, d, [&]() { redraw(); } );
}

void
CQChartsAxis::
setGridFillDisplayed(const GridFillDisplayed &d)
{
  CQChartsUtil::testAndSet(gridFillDisplayed_, d, [&]() { redraw(); } );
}

//---

void
CQChartsAxis::
setTicksDisplayed(const TicksDisplayed &d)
{
  CQChartsUtil::testAndSet(ticksDisplayed_, d, [&]() { redraw(); } );
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
setIncludeZero(bool b)
{
  CQChartsUtil::testAndSet(includeZero_, b, [&]() { updatePlotRange(); } );
}

//---

void
CQChartsAxis::
setValueType(const CQChartsAxisValueType &v, bool notify)
{
  CQChartsUtil::testAndSet(valueType_, v, [&]() {
    needsCalc_ = true;

    if (notify) updatePlotRangeAndObjs();
  } );
}

//---

void
CQChartsAxis::
calcAndRedraw()
{
  needsCalc_ = true;

  redraw();
}

void
CQChartsAxis::
updateCalc() const
{
  if (needsCalc_) {
    CQChartsAxis *th = const_cast<CQChartsAxis *>(this);

    th->needsCalc_ = false;

    th->calc();
  }
}

void
CQChartsAxis::
calc()
{
  interval_.setStart(start());
  interval_.setEnd  (end  ());

  interval_.setIntegral(isIntegral());
  interval_.setDate    (isDate    ());
  interval_.setLog     (isLog     ());

  if (majorIncrement().isSet())
    interval_.setMajorIncrement(majorIncrement().integer());
  else
    interval_.setMajorIncrement(0);

  if (tickIncrement().isSet())
    interval_.setTickIncrement(tickIncrement().integer());
  else
    interval_.setTickIncrement(0);

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
    return CQChartsUtil::formatInteger(long(pos));
  else
    return CQChartsUtil::formatReal(pos);
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

void
CQChartsAxis::
updatePlotRangeAndObjs()
{
  CQChartsPlot *plot = const_cast<CQChartsPlot *>(plot_);

  plot->updateRangeAndObjs();
}

CQChartsEditHandles *
CQChartsAxis::
editHandles() const
{
  return editHandles_.get();
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

  if (isHorizontal())
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

  if (isHorizontal())
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
  return (isMajorGridLinesDisplayed() || isMinorGridLinesDisplayed() || isMajorGridFilled());
}

void
CQChartsAxis::
drawGrid(const CQChartsPlot *plot, CQChartsPaintDevice *device)
{
  if (! isDrawGrid())
    return;

  //---

  CQChartsGeom::BBox dataRange = plot->calcDataRange();

  double amin = start();
  double amax = end  ();

  double dmin, dmax;

  if (isHorizontal()) {
    dmin = dataRange.getYMin();
    dmax = dataRange.getYMax();
  }
  else {
    dmin = dataRange.getXMin();
    dmax = dataRange.getXMax();
  }

  CQChartsGeom::Point a1 = windowToPixel(plot, amin, dmin);
  CQChartsGeom::Point a2 = windowToPixel(plot, amax, dmax);

  //---

  device->save();

  //---

  double inc  = calcIncrement();
  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  //---

  // draw fill
  if (isMajorGridFilled()) {
    QRectF dataRect = plot->calcDataPixelRect().qrect();

    device->setClipRect(dataRect);

    //---

    QBrush brush;

    QColor fillColor = interpAxesGridFillColor(ColorInd());

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

            if (isHorizontal())
              bbox = CQChartsGeom::BBox(pp1.x, a1.y, pp2.x, a2.y);
            else
              bbox = CQChartsGeom::BBox(a1.x, pp1.y, a2.x, pp2.y);

            device->fillRect(bbox.qrect(), brush);
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
  if (isMajorGridLinesDisplayed() || isMinorGridLinesDisplayed()) {
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

      // TODO: draw minor then major in case of overlap (e.g. log axis)

      for (uint i = 0; i < numMajorTicks() + 1; i++) {
        // draw major line (grid and tick)
        if (pos1 >= amin && pos1 <= amax) {
          // draw major grid line if major or minor displayed
          if      (isMajorGridLinesDisplayed())
            drawMajorGridLine(plot, device, pos1, dmin, dmax);
          else if (isMinorGridLinesDisplayed())
            drawMinorGridLine(plot, device, pos1, dmin, dmax);
        }

        if (isMinorGridLinesDisplayed()) {
          for (uint j = 1; j < numMinorTicks(); j++) {
            double pos2 = pos1 + (isLog() ? plot->logValue(j*inc1) : j*inc1);

            if (isIntegral() && ! CMathUtil::isInteger(pos2))
              continue;

            // draw minor grid line
            if (pos2 >= amin && pos2 <= amax)
              drawMinorGridLine(plot, device, pos2, dmin, dmax);
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

  device->restore();
}

void
CQChartsAxis::
draw(const CQChartsPlot *plot, CQChartsPaintDevice *device)
{
  fitBBox_   = CQChartsGeom::BBox(); // fit box
  fitLBBox_  = CQChartsGeom::BBox(); // label fit box
  fitTLBBox_ = CQChartsGeom::BBox(); // tick label fit box

  bbox_ = CQChartsGeom::BBox();

  //---

  double apos1, apos2;

  calcPos(plot, apos1, apos2);

  double amin = start();
  double amax = end  ();

  if (isHorizontal()) {
    bbox_ += CQChartsGeom::Point(amin, apos1);
    bbox_ += CQChartsGeom::Point(amax, apos1);
  }
  else {
    bbox_ += CQChartsGeom::Point(apos1, amin);
    bbox_ += CQChartsGeom::Point(apos1, amax);
  }

  fitBBox_ = bbox_;

  //---

  device->save();

  //---

  // axis line
  if (isAxesLines()) {
    drawLine(plot, device, apos1, amin, amax);
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

  //lastTickLabelRect_ = CQChartsGeom::BBox();

  axisTickLabelDrawDatas_.clear();

  if (isRequireTickLabel() && tickLabels_.size()) {
    for (const auto &p : tickLabels_) {
      double pos = p.first;

      if (pos < amin || pos > amax)
        continue;

      // draw major line (grid and tick)
      if (isMajorTicksDisplayed()) {
        drawMajorTickLine(plot, device, apos1, pos, isTickInside());

        if (isMirrorTicks())
          drawMajorTickLine(plot, device, apos2, pos, ! isTickInside());
      }

      //---

      // draw major tick label
      if (isAxesTickLabelTextVisible())
        drawTickLabel(plot, device, apos1, pos, isTickInside());
    }
  }
  else {
    double dt =
      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN ? -0.5 : 0.0);

    if (numMajorTicks() < maxMajorTicks()) {
      for (uint i = 0; i < numMajorTicks() + 1; i++) {
        double pos2 = pos1 + dt;

        // draw major line (grid and tick)
        if (pos2 >= amin && pos2 <= amax) {
          // draw major tick (or minor tick if major ticks off and minor ones on)
          if      (isMajorTicksDisplayed()) {
            drawMajorTickLine(plot, device, apos1, pos1, isTickInside());

            if (isMirrorTicks())
              drawMajorTickLine(plot, device, apos2, pos1, ! isTickInside());
          }
          else if (isMinorTicksDisplayed()) {
            drawMinorTickLine(plot, device, apos1, pos1, isTickInside());

            if (isMirrorTicks())
              drawMinorTickLine(plot, device, apos2, pos1, ! isTickInside());
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
              drawMinorTickLine(plot, device, apos1, pos2, isTickInside());

              if (isMirrorTicks())
                drawMinorTickLine(plot, device, apos2, pos2, ! isTickInside());
            }
          }
        }

        //---

        if (isAxesTickLabelTextVisible()) {
          // draw major tick label
          if (pos1 >= amin && pos1 <= amax) {
            drawTickLabel(plot, device, apos1, pos1, isTickInside());
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

  drawAxisTickLabelDatas(plot, device);

  //---

  // fix range if not set
  if (! lbbox_.isSet()) {
    CQChartsGeom::Point a1 = windowToPixel(plot, amin, apos1);
    CQChartsGeom::Point a2 = windowToPixel(plot, amax, apos1);

    if (isHorizontal()) {
      bool isPixelBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                           (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

      double dys = (isPixelBottom ? 1 : -1);

      a2.y += dys*(tlen2 + tgap);

      lbbox_ += CQChartsGeom::Point(a1.x, a1.y);
      lbbox_ += CQChartsGeom::Point(a2.x, a2.y);
    }
    else {
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

    drawAxisLabel(plot, device, apos1, amin, amax, text);
  }

  //---

  if (plot->showBoxes()) {
    plot->drawWindowColorBox(device, bbox_, Qt::blue);

    plot->drawColorBox(device, lbbox_, Qt::green);
  }

  //---

  device->restore();

  //----

  double extent = maxFitExtent_/100.0;

  double fitMin = fitBBox_.getMinExtent(isHorizontal());
  double fitMax = fitBBox_.getMaxExtent(isHorizontal());
  double fitLen = fitMax - fitMin;

  auto adjustLabelFitBox = [&](CQChartsGeom::BBox &bbox) {
    if (fitLen <= 0) return bbox;

    double boxMin = bbox.getMinExtent(isHorizontal());
    double boxMax = bbox.getMaxExtent(isHorizontal());

    double f1 = (fitMin - boxMin)/fitLen;
    double f2 = (boxMax - fitMax)/fitLen;

    if (f1 <= extent && f2 <= extent)
      return bbox;

    CQChartsGeom::BBox bbox1 = bbox;

    if (f1 > extent)
      bbox1.setMinExtent(isHorizontal(), fitMin - extent*fitLen);

    if (f2 > extent)
      bbox1.setMaxExtent(isHorizontal(), fitMax + extent*fitLen);

    return bbox1;
  };

  if (fitTLBBox_.isSet())
    fitBBox_ += adjustLabelFitBox(fitTLBBox_);

  if (fitLBBox_.isSet())
    fitBBox_ += adjustLabelFitBox(fitLBBox_);
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
    dataRange = CQChartsGeom::BBox(0.0, 0.0, 1.0, 1.0);

  //---

  if (isHorizontal()) {
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
drawLine(const CQChartsPlot *, CQChartsPaintDevice *device,
         double apos, double amin, double amax)
{
  QPen pen;

  QColor lc = interpAxesLinesColor(ColorInd());

  plot_->setPen(pen, true, lc, axesLinesAlpha(), axesLinesWidth(), axesLinesDash());

  device->setPen(pen);

  //---

  if (isHorizontal())
    device->drawLine(QPointF(amin, apos), QPointF(amax, apos));
  else
    device->drawLine(QPointF(apos, amin), QPointF(apos, amax));
}

void
CQChartsAxis::
drawMajorGridLine(const CQChartsPlot *, CQChartsPaintDevice *device,
                  double apos, double dmin, double dmax)
{
  QPen pen;

  QColor lc = interpAxesMajorGridLinesColor(ColorInd());

  plot_->setPen(pen, true, lc, axesMajorGridLinesAlpha(),
                axesMajorGridLinesWidth(), axesMajorGridLinesDash());

  device->setPen(pen);

  //---

  if (isHorizontal())
    device->drawLine(QPointF(apos, dmin), QPointF(apos, dmax));
  else
    device->drawLine(QPointF(dmin, apos), QPointF(dmax, apos));
}

void
CQChartsAxis::
drawMinorGridLine(const CQChartsPlot *, CQChartsPaintDevice *device,
                  double apos, double dmin, double dmax)
{
  QPen pen;

  QColor lc = interpAxesMinorGridLinesColor(ColorInd());

  plot_->setPen(pen, true, lc, axesMinorGridLinesAlpha(),
                axesMinorGridLinesWidth(), axesMinorGridLinesDash());

  device->setPen(pen);

  //---

  if (isHorizontal())
    device->drawLine(QPointF(apos, dmin), QPointF(apos, dmax));
  else
    device->drawLine(QPointF(dmin, apos), QPointF(dmax, apos));
}

void
CQChartsAxis::
drawMajorTickLine(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                  double apos, double tpos, bool inside)
{
  drawTickLine(plot, device, apos, tpos, inside, /*major*/true);
}

void
CQChartsAxis::
drawMinorTickLine(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                  double apos, double tpos, bool inside)
{
  drawTickLine(plot, device, apos, tpos, inside, /*major*/false);
}

void
CQChartsAxis::
drawTickLine(const CQChartsPlot *plot, CQChartsPaintDevice *device,
             double apos, double tpos, bool inside, bool major)
{
  int tlen = (major ? majorTickLen() : minorTickLen());

  CQChartsGeom::Point pp;

  if (major && tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
    pp = CQChartsGeom::Point(tpos - 0.5, apos);
  else
    pp = CQChartsGeom::Point(tpos, apos);

  //---

  QPen pen;

  QColor lc = interpAxesLinesColor(ColorInd());

  plot_->setPen(pen, true, lc, axesLinesAlpha(), axesLinesWidth(), axesLinesDash());

  device->setPen(pen);

  //---

  if (isHorizontal()) {
    bool isWindowBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT);
    bool isPixelBottom  = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                          (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

    int pys = (isPixelBottom ? 1 : -1);
    int dt1 = pys*tlen;

    double adt1 = plot->pixelToWindowHeight(dt1);

    if (inside)
      device->drawLine(QPointF(pp.x, pp.y), QPointF(pp.x, pp.y + adt1));
    else {
      device->drawLine(QPointF(pp.x, pp.y), QPointF(pp.x, pp.y - adt1));

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
      device->drawLine(QPointF(pp.x, pp.y), QPointF(pp.x - adt1, pp.y));
    else {
      device->drawLine(QPointF(pp.x, pp.y), QPointF(pp.x + adt1, pp.y));

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
drawTickLabel(const CQChartsPlot *plot, CQChartsPaintDevice *device,
              double apos, double tpos, bool inside)
{
  int tgap  = 2;
  int tlen1 = majorTickLen();
  int tlen2 = minorTickLen();

  CQChartsGeom::Point pp = windowToPixel(plot, tpos, apos);

  QString text = valueStr(plot, tpos);

  if (! text.length())
    return;

  //---

#if 0
  QPen tpen;

  QColor tc = interpAxesTickLabelTextColor(ColorInd());

  plot->setPen(tpen, true, tc, axesTickLabelTextAlpha());

  device->setPen(tpen);
#endif

  view()->setPlotPainterFont(plot, device, axesTickLabelTextFont());

  QFontMetricsF fm(device->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  double angle = axesTickLabelTextAngle();

  if (isHorizontal()) {
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

    //bool visible = true;

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
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, device->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsGeom::BBox(rrect);

        tbbox = plot->pixelToWindow(CQChartsGeom::BBox(rrect));
      }

#if 0
      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }
#endif

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

#if 0
        if (visible)
          CQChartsDrawUtil::drawSimpleText(device, p, text);
#endif

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(p, tbbox, text));
      }
      else {
#if 0
        if (visible) {
          QPointF p1 = plot_->pixelToWindow(pt);

          CQChartsRotatedText::draw(device, p1, text, angle, align,
                                    /*alignBox*/true, isAxesTickLabelTextContrast());
        }
#endif

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(pt, tbbox, text, angle, align));
      }

#if 0
      if (plot->showBoxes()) {
        if (visible)
          plot->drawWindowColorBox(device, tbbox);
      }
#endif

#if 0
      if (visible)
        lastTickLabelRect_ = tbbox;
#endif
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
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, device->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsGeom::BBox(rrect);

        tbbox = plot->pixelToWindow(CQChartsGeom::BBox(rrect));
      }

#if 0
      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }
#endif

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

#if 0
        if (visible)
          CQChartsDrawUtil::drawSimpleText(device, p, text);
#endif

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(p, tbbox, text));
      }
      else {
#if 0
        if (visible) {
          QPointF p1 = plot_->pixelToWindow(pt);

          CQChartsRotatedText::draw(device, p1, text, angle, align,
                                    /*alignBox*/true, isAxesTickLabelTextContrast());
        }
#endif

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(pt, tbbox, text, angle, align));
      }

#if 0
      if (plot->showBoxes()) {
        if (visible)
          plot->drawWindowColorBox(device, tbbox);
      }
#endif

#if 0
      if (visible)
        lastTickLabelRect_ = tbbox;
#endif
    }

    bbox_      += tbbox;
    fitTLBBox_ += tbbox;
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

    //bool visible = true;

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
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, device->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsGeom::BBox(rrect);

        tbbox = plot->pixelToWindow(CQChartsGeom::BBox(rrect));
      }

#if 0
      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }
#endif

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

#if 0
        if (visible)
          CQChartsDrawUtil::drawSimpleText(device, p, text);
#endif

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(p, tbbox, text));
      }
      else {
#if 0
        if (visible) {
          QPointF p1 = plot_->pixelToWindow(pt);

          CQChartsRotatedText::draw(device, p1, text, angle, align,
                                    /*alignBox*/true, isAxesTickLabelTextContrast());
        }
#endif

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(pt, tbbox, text, angle, align));
      }

#if 0
      if (plot->showBoxes()) {
        if (visible)
          plot->drawWindowColorBox(device, tbbox);
      }
#endif

#if 0
      if (visible)
        lastTickLabelRect_ = tbbox;
#endif
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
        QRectF rrect = CQChartsRotatedText::bbox(pt.x(), pt.y(), text, device->font(),
                                                 angle, 0, align, /*alignBox*/true);

        lbbox_ += CQChartsGeom::BBox(rrect);

        tbbox = plot->pixelToWindow(CQChartsGeom::BBox(rrect));
      }

#if 0
      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }
#endif

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

#if 0
        if (visible)
          CQChartsDrawUtil::drawSimpleText(device, p, text);
#endif

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(p, tbbox, text));
      }
      else {
#if 0
        if (visible) {
          QPointF p1 = plot_->pixelToWindow(pt);

          CQChartsRotatedText::draw(device, p1, text, angle, align,
                                    /*alignBox*/true, isAxesTickLabelTextContrast());
        }
#endif

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(pt, tbbox, text, angle, align));
      }

#if 0
      if (plot->showBoxes()) {
        if (visible)
          plot->drawWindowColorBox(device, tbbox);
      }
#endif

#if 0
      if (visible)
        lastTickLabelRect_ = tbbox;
#endif
    }

    bbox_      += tbbox;
    fitTLBBox_ += tbbox;
  }
}

void
CQChartsAxis::
drawAxisTickLabelDatas(const CQChartsPlot *plot, CQChartsPaintDevice *device)
{
  int n = axisTickLabelDrawDatas_.size();
  if (n < 1) return;

  //---

  // clip overlapping labels
  if (isTickLabelAutoHide()) {
    if (n > 1) {
      const CQChartsGeom::BBox &firstBBox = axisTickLabelDrawDatas_[0    ].bbox;
      const CQChartsGeom::BBox &lastBBox  = axisTickLabelDrawDatas_[n - 1].bbox;

      // if first and last labels overlap then only draw first
      if (lastBBox.overlaps(firstBBox)) {
        for (int i = 1; i < n; ++i)
          axisTickLabelDrawDatas_[i].visible = false;
      }
      // otherwise draw first and last and clip others
      else {
        CQChartsGeom::BBox prevBBox = firstBBox;

        for (int i = 1; i < n - 1; ++i) {
          AxisTickLabelDrawData &data = axisTickLabelDrawDatas_[i];

          if (data.bbox.overlaps(prevBBox) || data.bbox.overlaps(lastBBox))
            data.visible = false;

          if (data.visible)
            prevBBox = data.bbox;
        }
      }
    }
  }

  //---

  QPen tpen;

  QColor tc = interpAxesTickLabelTextColor(ColorInd());

  plot->setPen(tpen, true, tc, axesTickLabelTextAlpha());

  device->setPen(tpen);

  //view()->setPlotPainterFont(plot, device, axesTickLabelTextFont());

  //---

  for (const auto &data : axisTickLabelDrawDatas_) {
    if (! data.visible)
      continue;

    QPointF p1 = plot_->pixelToWindow(data.p);

    if (CMathUtil::isZero(data.angle))
      CQChartsDrawUtil::drawSimpleText(device, p1, data.text);
    else
      CQChartsRotatedText::draw(device, p1, data.text, data.angle, data.align,
                                /*alignBox*/true, isAxesTickLabelTextContrast());
  }

  if (plot->showBoxes()) {
    for (const auto &data : axisTickLabelDrawDatas_) {
      if (! data.visible)
        continue;

      plot->drawWindowColorBox(device, data.bbox);
    }
  }
}

void
CQChartsAxis::
drawAxisLabel(const CQChartsPlot *plot, CQChartsPaintDevice *device,
              double apos, double amin, double amax, const QString &text)
{
  if (! text.length())
    return;

  //---

  int tgap = 2;

  CQChartsGeom::Point a1 = windowToPixel(plot, amin, apos);
  CQChartsGeom::Point a2 = windowToPixel(plot, amax, apos);
  CQChartsGeom::Point a3 = windowToPixel(plot, amin, apos);

  //---

  QPen tpen;

  QColor tc = interpAxesLabelTextColor(ColorInd());

  plot->setPen(tpen, true, tc, axesLabelTextAlpha());

  device->setPen(tpen);

  view()->setPlotPainterFont(plot, device, axesLabelTextFont());

  QFontMetricsF fm(device->font());

  double tw = fm.width(text);
  double ta = fm.ascent();
  double td = fm.descent();

  CQChartsGeom::BBox bbox;

  // draw label
  if (isHorizontal()) {
    double wfh = plot->pixelToWindowHeight(ta + td);

    double axm = (a1.x + a2.x)/2 - tw/2;

    bool isPixelBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                         (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

    //int pys = (isPixelBottom ? 1 : -1);

    double ath;
    double atw = plot->pixelToWindowWidth(tw/2);

    if (isPixelBottom) {
      ath = plot->pixelToWindowHeight((lbbox_.getYMax() - a3.y) + tgap) + wfh;

      QPointF pt(axm, lbbox_.getYMax() + ta + tgap);

      CQChartsDrawUtil::drawSimpleText(device, plot->pixelToWindow(pt), text);

      if (! plot_->isInvertY()) {
        bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos - (ath      ));
        bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos - (ath - wfh));
      }
      else {
        bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos + (ath      ));
        bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos + (ath - wfh));
      }

      fitLBBox_ += CQChartsGeom::Point((amin + amax)/2, apos - (ath      ));
      fitLBBox_ += CQChartsGeom::Point((amin + amax)/2, apos - (ath - wfh));
    }
    else {
      ath = plot->pixelToWindowHeight((a3.y - lbbox_.getYMin()) + tgap) + wfh;

      QPointF pt(axm, lbbox_.getYMin() - td - tgap);

      CQChartsDrawUtil::drawSimpleText(device, plot->pixelToWindow(pt), text);

      if (! plot_->isInvertY()) {
        bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos + (ath      ));
        bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos + (ath - wfh));
      }
      else {
        bbox += CQChartsGeom::Point((amin + amax)/2 - atw, apos - (ath      ));
        bbox += CQChartsGeom::Point((amin + amax)/2 + atw, apos - (ath - wfh));
      }

      fitLBBox_ += CQChartsGeom::Point((amin + amax)/2, apos + (ath      ));
      fitLBBox_ += CQChartsGeom::Point((amin + amax)/2, apos + (ath - wfh));
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

      QPointF p1 = plot_->pixelToWindow(QPointF(tx, aym));

      CQChartsRotatedText::draw(device, p1, text, 90.0, Qt::AlignLeft | Qt::AlignBottom,
                                /*alignBBox*/false, isAxesLabelTextContrast());

      if (! plot_->isInvertX()) {
        bbox += CQChartsGeom::Point(apos - (atw      ), (amin + amax)/2 - ath);
        bbox += CQChartsGeom::Point(apos - (atw - wfh), (amin + amax)/2 + ath);
      }
      else {
        bbox += CQChartsGeom::Point(apos + (atw      ), (amin + amax)/2 - ath);
        bbox += CQChartsGeom::Point(apos + (atw - wfh), (amin + amax)/2 + ath);
      }

      fitLBBox_ += CQChartsGeom::Point(apos - (atw      ), (amin + amax)/2);
      fitLBBox_ += CQChartsGeom::Point(apos - (atw - wfh), (amin + amax)/2);
    }
    else {
      atw = plot->pixelToWindowWidth((lbbox_.getXMax() - a3.x) + tgap) + wfh;

#if 0
      double aym = (a2.y + a1.y)/2 - tw/2;

      double tx = lbbox_.getXMax() + tgap + td;

      QPointF p1 = plot_->pixelToWindow(QPointF(tx, aym));

      CQChartsRotatedText::draw(device, p1, text, -90.0, Qt::AlignLeft | Qt::AlignBottom,
                                /*alignBBox*/false, isAxesLabelTextContrast());
#else
      double aym = (a2.y + a1.y)/2 + tw/2;

      double tx = lbbox_.getXMax() + tgap + ta + td;

      QPointF p1 = plot_->pixelToWindow(QPointF(tx, aym));

      CQChartsRotatedText::draw(device, p1, text, 90.0, Qt::AlignLeft | Qt::AlignBottom,
                                /*alignBBox*/false, isAxesLabelTextContrast());
#endif

      if (! plot_->isInvertX()) {
        bbox += CQChartsGeom::Point(apos + (atw      ), (amin + amax)/2 - ath);
        bbox += CQChartsGeom::Point(apos + (atw - wfh), (amin + amax)/2 + ath);
      }
      else {
        bbox += CQChartsGeom::Point(apos - (atw      ), (amin + amax)/2 - ath);
        bbox += CQChartsGeom::Point(apos - (atw - wfh), (amin + amax)/2 + ath);
      }

      fitLBBox_ += CQChartsGeom::Point(apos + (atw      ), (amin + amax)/2);
      fitLBBox_ += CQChartsGeom::Point(apos + (atw - wfh), (amin + amax)/2);
    }
  }

  if (plot->showBoxes()) {
    plot->drawWindowColorBox(device, bbox);
  }

  bbox_ += bbox;
}

CQChartsGeom::Point
CQChartsAxis::
windowToPixel(const CQChartsPlot *plot, double x, double y) const
{
  if (isHorizontal())
    return plot->windowToPixel(CQChartsGeom::Point(x, y));
  else
    return plot->windowToPixel(CQChartsGeom::Point(y, x));
}

CQChartsGeom::BBox
CQChartsAxis::
fitBBox() const
{
  return fitBBox_;
}
