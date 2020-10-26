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
#include <CQTclUtil.h>

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
  init();
}

CQChartsAxis::
~CQChartsAxis()
{
}

void
CQChartsAxis::
init()
{
  setObjectName("axis");

  setEditable(true);

  //--

  editHandles_ = std::make_unique<CQChartsEditHandles>(plot_, CQChartsEditHandles::Mode::MOVE);

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
  setAxesGridFillAlpha(CQChartsAlpha(0.5));

  needsCalc_ = true;

  //---

  setAxesTickLabelTextFont(CQChartsFont().decFontSize(4));
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

//---

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

//---

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
                     const QString &desc, bool hidden=false) {
    auto *item = &(model->addProperty(path, this, name, alias)->setDesc(desc));
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc, bool hidden=false) {
    auto *item = addProp(path, name, alias, desc, hidden);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addProp(path, "direction", "", "Axis direction", true)->setEditable(false);

  addProp(path, "visible"  , "", "Axis visible");
  addProp(path, "editable" , "", "Axis editable");
  addProp(path, "side"     , "", "Axis plot side");
  addProp(path, "valueType", "", "Axis value type");
  addProp(path, "format"   , "", "Axis tick value format string");

  addProp(path, "tickIncrement" , "", "Axis tick increment");
  addProp(path, "majorIncrement", "", "Axis tick major increment");
  addProp(path, "start"         , "", "Axis start position");
  addProp(path, "end"           , "", "Axis end position");
  addProp(path, "includeZero"   , "", "Axis force include zero", true);

  addProp(path, "valueStart", "", "Axis custom start position");
  addProp(path, "valueEnd"  , "", "Axis custom end position");

  addProp(path, "tickLabels"      , "", "Indexed Tick Labels", true);
  addProp(path, "customTickLabels", "", "Custom Tick Labels", true);

  addProp(path, "maxFitExtent", "", "Axis maximum extent percent for auto fit")->
    setMinValue(0.0);

  //---

  auto posPath = path + "/position";

  addProp(posPath, "position", "value", "Axis position");

  //---

  auto linePath = path + "/stroke";

  addStyleProp(linePath, "axesLineData"  , "style"  , "Axis stroke style", true);
  addStyleProp(linePath, "axesLines"     , "visible", "Axis stroke visible");
  addStyleProp(linePath, "axesLinesColor", "color"  , "Axis stroke color");
  addStyleProp(linePath, "axesLinesAlpha", "alpha"  , "Axis stroke alpha");
  addStyleProp(linePath, "axesLinesWidth", "width"  , "Axis stroke width");
  addStyleProp(linePath, "axesLinesDash" , "dash"   , "Axis stroke dash");

  //---

  auto ticksPath = path + "/ticks";

  addProp(ticksPath, "ticksDisplayed", "lines", "Axis major and/or minor ticks visible");

  auto majorTicksPath = ticksPath + "/major";
  auto minorTicksPath = ticksPath + "/minor";

  addProp(majorTicksPath, "majorTickLen", "length", "Axis major ticks pixel length");
  addProp(minorTicksPath, "minorTickLen", "length", "Axis minor ticks pixel length");

  //---

  auto ticksLabelPath     = ticksPath + "/label";
  auto ticksLabelTextPath = ticksLabelPath + "/text";

  addProp(ticksLabelPath, "tickLabelAutoHide" , "autoHide", "Axis tick label text is auto hide");
  addProp(ticksLabelPath, "tickLabelPlacement", "placement", "Axis tick label text placement");

  addStyleProp(ticksLabelTextPath, "axesTickLabelTextData"         , "style",
               "Axis tick label text style", true);
  addProp     (ticksLabelTextPath, "axesTickLabelTextVisible"      , "visible",
               "Axis tick label text visible");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextColor"        , "color",
               "Axis tick label text color");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextAlpha"        , "alpha",
               "Axis tick label text alpha");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextFont"         , "font",
               "Axis tick label text font");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextAngle"        , "angle",
               "Axis tick label text angle");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextContrast"     , "contrast",
               "Axis tick label text contrast");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextContrastAlpha", "contrastAlpha",
               "Axis tick label text contrast alpha");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextClipLength"   , "clipLength",
               "Axis tick label text clip length");
  addStyleProp(ticksLabelTextPath, "axesTickLabelTextClipElide"    , "clipElide",
               "Axis tick label text clip elide");

  addProp(ticksPath, "tickInside" , "inside", "Axis ticks drawn inside plot");
  addProp(ticksPath, "mirrorTicks", "mirror", "Axis tick are mirrored on other side of plot");

  //---

  auto labelPath     = path + "/label";
  auto labelTextPath = labelPath + "/text";

  addProp(labelTextPath, "labelStr" , "string"   , "Axis label text string");
  addProp(labelTextPath, "defLabel" , "defString", "Axis label text default string");
//addProp(labelTextPath, "userLabel", "string"   , "Axis label text user string");

  addStyleProp(labelTextPath, "axesLabelTextData"         , "style",
               "Axis label text style", true);
  addProp     (labelTextPath, "axesLabelTextVisible"      , "visible",
               "Axis label text visible");
  addStyleProp(labelTextPath, "axesLabelTextColor"        , "color",
               "Axis label text color");
  addStyleProp(labelTextPath, "axesLabelTextAlpha"        , "alpha",
               "Axis label text alpha");
  addStyleProp(labelTextPath, "axesLabelTextFont"         , "font",
               "Axis label text font");
  addStyleProp(labelTextPath, "axesLabelTextContrast"     , "contrast",
               "Axis label text contrast");
  addStyleProp(labelTextPath, "axesLabelTextContrastAlpha", "contrastAlpha",
               "Axis label text contrast alpha");
  addStyleProp(labelTextPath, "axesLabelTextHtml"         , "html",
               "Axis label text is HTML");
  addStyleProp(labelTextPath, "axesLabelTextClipLength"   , "clipLength",
               "Axis label text clip length");
  addStyleProp(labelTextPath, "axesLabelTextClipElide"    , "clipElide",
               "Axis label text clip elide");

  //---

  auto gridPath            = path + "/grid";
  auto gridLinePath        = gridPath + "/stroke";
  auto gridMajorPath       = gridPath + "/major";
  auto gridMajorStrokePath = gridMajorPath + "/stroke";
  auto gridMajorFillPath   = gridMajorPath + "/fill";
  auto gridMinorPath       = gridPath + "/minor";
  auto gridMinorStrokePath = gridMinorPath + "/stroke";

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
  addStyleProp(gridMajorFillPath, "axesGridFillPattern", "pattern", "Axis grid fill pattern");
}

//---

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
setValueStart(const OptReal &v)
{
  valueStart_ = v;

  calcAndRedraw();
}

void
CQChartsAxis::
setValueEnd(const OptReal &v)
{
  valueEnd_ = v;

  calcAndRedraw();
}

//---

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

QString
CQChartsAxis::
tickLabelsStr() const
{
  QStringList strs;

  for (const auto &p : tickLabels_) {
    QStringList strs1;

    strs1 << QString("%1").arg(p.first );
    strs1 << QString("%1").arg(p.second);

    auto str1 = CQTcl::mergeList(strs1);

    strs << str1;
  }

  return CQTcl::mergeList(strs);
}

void
CQChartsAxis::
setTickLabelsStr(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return;

  for (int i = 0; i < strs.length(); ++i) {
    const auto &str1 = strs[i];

    QStringList strs1;

    if (! CQTcl::splitList(str1, strs1))
      continue;

    if (strs1.length() < 1)
      continue;

    bool ok;

    int value = strs1[0].toInt(&ok);
    if (! ok) continue;

    if (strs1.length() > 1)
      setTickLabel(value, strs1[1]);
    else
      setTickLabel(value, strs1[0]);
  }
}

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
customTickLabelsStr() const
{
  QStringList strs;

  for (const auto &p : customTickLabels_) {
    QStringList strs1;

    strs1 << QString("%1").arg(p.first );
    strs1 << QString("%1").arg(p.second);

    auto str1 = CQTcl::mergeList(strs1);

    strs << str1;
  }

  return CQTcl::mergeList(strs);
}

void
CQChartsAxis::
setCustomTickLabelsStr(const QString &str)
{
  customTickLabels_.clear();

  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return;

  for (int i = 0; i < strs.length(); ++i) {
    const auto &str1 = strs[i];

    QStringList strs1;

    if (! CQTcl::splitList(str1, strs1))
      continue;

    if (strs1.length() < 1)
      continue;

    bool ok;

    double value = strs1[0].toDouble(&ok);
    if (! ok) continue;

    if (strs1.length() > 1)
      customTickLabels_[value] = strs1[1];
    else
      customTickLabels_[value] = strs1[0];
  }
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
      auto *plot = const_cast<CQChartsPlot *>(plot_);

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
setMaxFitExtent(double r)
{
  CQChartsUtil::testAndSet(maxFitExtent_, r, [&]() { redraw(); } );
}

//---

void
CQChartsAxis::
setLabel(const CQChartsOptString &str)
{
  CQChartsUtil::testAndSet(label_, str, [&]() { redraw(); } );
}

void
CQChartsAxis::
setLabelStr(const QString &str)
{
  if (label_.stringOr() != str) {
    label_.setString(str); redraw();
  }
}

void
CQChartsAxis::
setDefLabel(const QString &str, bool notify)
{
  if (label_.defValue() != str) {
    label_.setDefValue(str);

    if (notify)
      redraw();
  }
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
setGridMid(bool b)
{
  CQChartsUtil::testAndSet(gridMid_, b, [&]() { redraw(); } );
}

void
CQChartsAxis::
setGridAbove(bool b)
{
  CQChartsUtil::testAndSet(gridAbove_, b, [&]() { redraw(); } );
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
setMajorTickLen(int i)
{
  CQChartsUtil::testAndSet(majorTickLen_, i, [&]() { redraw(); } );
}

void
CQChartsAxis::
setMinorTickLen(int i)
{
  CQChartsUtil::testAndSet(minorTickLen_, i, [&]() { redraw(); } );
}

void
CQChartsAxis::
setTickInside(bool b)
{
  CQChartsUtil::testAndSet(tickInside_, b, [&]() { redraw(); } );
}

void
CQChartsAxis::
setMirrorTicks(bool b)
{
  CQChartsUtil::testAndSet(mirrorTicks_, b, [&]() { redraw(); } );
}

//---

void
CQChartsAxis::
setTickLabelAutoHide(bool b)
{
  CQChartsUtil::testAndSet(tickLabelAutoHide_, b, [&]() { redraw(); } );
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
setTickLabelPlacement(const CQChartsAxisTickLabelPlacement &p)
{
  CQChartsUtil::testAndSet(tickLabelPlacement_, p, [&]() {
    emit tickPlacementChanged();

    redraw();
  } );
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
setAnnotation(bool b)
{
  CQChartsUtil::testAndSet(annotation_, b, [&]() { redraw(); } );
}

void
CQChartsAxis::
setAllowHtmlLabels(bool b)
{
  CQChartsUtil::testAndSet(allowHtmlLabels_, b, [&]() { redraw(); } );
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
    auto *th = const_cast<CQChartsAxis *>(this);

    th->needsCalc_ = false;

    th->calc();
  }
}

void
CQChartsAxis::
calc()
{
  interval_.setStart(valueStart_.realOr(start()));
  interval_.setEnd  (valueEnd_  .realOr(end  ()));

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
  using ModelIndex = CQChartsModelIndex;

  if (isLog())
    pos = plot->expValue(pos);

  QVariant valuePos(pos);

  if (isIntegral()) {
    long ipos = long(pos);

    if (hasTickLabel(ipos))
      return tickLabel(ipos);

    if (isRequireTickLabel())
      return "";

    valuePos = QVariant(int(pos));
  }

  if (formatStr_.length()) {
    QString str;

    if (CQChartsModelUtil::formatColumnTypeValue(plot->charts(), plot->model().data(),
                                                 column(), formatStr_, valuePos, str))
      return str;
  }

  if (column().isValid()) {
    QString str;

    if (CQChartsModelUtil::formatColumnValue(plot->charts(), plot->model().data(),
                                             column(), valuePos, str))
      return str;

    if (isDataLabels()) {
      int row = int(pos);

      QModelIndex parent; // TODO: support parent

      ModelIndex columnInd(const_cast<CQChartsPlot *>(plot), row, column(), parent);

      bool ok;

      auto header = plot->modelValue(columnInd, ok);

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
  auto *plot = const_cast<CQChartsPlot *>(plot_);

  plot->updateMargins();
}

bool
CQChartsAxis::
contains(const Point &p) const
{
  if (! isVisible())
    return false;

  return bbox().inside(p);
}

void
CQChartsAxis::
redraw(bool wait)
{
  auto *plot = const_cast<CQChartsPlot *>(plot_);

  if (plot) {
    if (wait) {
      if (isDrawAll()) {
        plot->drawObjs();
      }
      else {
        plot->drawBackground();
        plot->drawForeground();
      }
    }
    else {
      if (isDrawAll()) {
        plot->invalidateLayers();
      }
      else {
        plot->invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
        plot->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
      }
    }
  }

  emit appearanceChanged();
}

void
CQChartsAxis::
updatePlotRange()
{
  auto *plot = const_cast<CQChartsPlot *>(plot_);

  plot->updateRange();
}

void
CQChartsAxis::
updatePlotRangeAndObjs()
{
  auto *plot = const_cast<CQChartsPlot *>(plot_);

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
editPress(const Point &p)
{
  editHandles_->setDragPos(p);

  double apos1, apos2;

  calcPos(plot(), apos1, apos2);

  setPosition(CQChartsOptReal(apos1));

  return true;
}

bool
CQChartsAxis::
editMove(const Point &p)
{
  const auto &dragPos = editHandles_->dragPos();

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
editMotion(const Point &p)
{
  return editHandles_->selectInside(p);
}

void
CQChartsAxis::
editMoveBy(const Point &d)
{
  double apos1, apos2;

  calcPos(plot(), apos1, apos2);

  double apos;

  if (isHorizontal())
    apos = apos1 + d.y;
  else
    apos = apos1 + d.x;

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

  auto dataRange = plot->calcDataRange();

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

  auto a1 = windowToPixel(plot, amin, dmin);
  auto a2 = windowToPixel(plot, amax, dmax);

  //---

  device->save();

  //---

  double inc  = calcIncrement();
  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  //---

  // draw fill
  if (isMajorGridFilled()) {
    auto dataRect = plot->calcDataPixelRect();

    device->setClipRect(dataRect);

    //---

    CQChartsPenBrush penBrush;

    auto fillColor = interpAxesGridFillColor(ColorInd());

    plot->setPenBrush(penBrush,
      PenData(false), BrushData(true, fillColor, axesGridFillAlpha(), axesGridFillPattern()));

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

            auto pp1 = plot->windowToPixel(Point(pos3, pos1));
            auto pp2 = plot->windowToPixel(Point(pos4, pos2));

            BBox bbox;

            if (isHorizontal())
              bbox = BBox(pp1.x, a1.y, pp2.x, a2.y);
            else
              bbox = BBox(a1.x, pp1.y, a2.x, pp2.y);

            CQChartsDrawUtil::setPenBrush(device, penBrush);

            device->fillRect(bbox);
          }
        }

        //---

        pos1 = pos2;

        if (isDate())
          pos2 = interval_.interval(int(i + 1));
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
          pos1 = interval_.interval(int(i + 1));
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
drawAt(double pos, const CQChartsPlot *plot, CQChartsPaintDevice *device)
{
  auto position = CQChartsOptReal(pos);

  std::swap(position_, position);

  draw(plot, device);

  std::swap(position_, position);
}

void
CQChartsAxis::
draw(const CQChartsPlot *plot, CQChartsPaintDevice *device)
{
  fitBBox_   = BBox(); // fit box
  fitLBBox_  = BBox(); // label fit box
  fitTLBBox_ = BBox(); // tick label fit box

  bbox_ = BBox();

  //---

  double apos1, apos2;

  calcPos(plot, apos1, apos2);

  double amin = start();
  double amax = end  ();

  if (isHorizontal()) {
    bbox_ += Point(amin, apos1);
    bbox_ += Point(amax, apos1);
  }
  else {
    bbox_ += Point(apos1, amin);
    bbox_ += Point(apos1, amax);
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

  auto mapPos = [&](double pos) {
    if (! valueStart_.isSet() && ! valueEnd_.isSet())
      return pos;

    return CMathUtil::map(pos, valueStart_.realOr(start()), valueEnd_.realOr(end()),
                          start(), end());
  };

  //---

  double pos1;

  if (isDate())
    pos1 = interval_.interval(0);
  else
    pos1 = calcStart();

  int tlen2 = majorTickLen();
  int tgap  = 2;

  //---

  lbbox_ = BBox();

  //---

  //lastTickLabelRect_ = BBox();

  axisTickLabelDrawDatas_.clear();

  if      (customTickLabels_.size()) {
    for (const auto &p : customTickLabels_) {
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
        drawTickLabel(plot, device, apos1, pos, pos, isTickInside());
    }
  }
  else if (isRequireTickLabel() && tickLabels_.size()) {
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
        drawTickLabel(plot, device, apos1, pos, pos, isTickInside());
    }
  }
  else {
    double dt =
      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN ? -0.5 : 0.0);

    if (numMajorTicks() < maxMajorTicks()) {
      for (uint i = 0; i < numMajorTicks() + 1; i++) {
        double pos2 = pos1 + dt;

        double mpos1 = mapPos(pos1);
        double mpos2 = mapPos(pos2);

        // draw major line (grid and tick)
        if (mpos2 >= amin && mpos2 <= amax) {
          // draw major tick (or minor tick if major ticks off and minor ones on)
          if      (isMajorTicksDisplayed()) {
            drawMajorTickLine(plot, device, apos1, mpos1, isTickInside());

            if (isMirrorTicks())
              drawMajorTickLine(plot, device, apos2, mpos1, ! isTickInside());
          }
          else if (isMinorTicksDisplayed()) {
            drawMinorTickLine(plot, device, apos1, mpos1, isTickInside());

            if (isMirrorTicks())
              drawMinorTickLine(plot, device, apos2, mpos1, ! isTickInside());
          }
        }

        // draw minor tick lines (grid and tick)
        if (isMinorTicksDisplayed() && i < numMajorTicks()) {
          for (uint j = 1; j < numMinorTicks(); j++) {
            double pos2 = pos1 + (isLog() ? plot->logValue(j*inc1) : j*inc1);

            if (isIntegral() && ! CMathUtil::isInteger(pos2))
              continue;

             double mpos2 = mapPos(pos2);

            // draw minor tick line
            if (mpos2 >= amin && mpos2 <= amax) {
              drawMinorTickLine(plot, device, apos1, mpos2, isTickInside());

              if (isMirrorTicks())
                drawMinorTickLine(plot, device, apos2, mpos2, ! isTickInside());
            }
          }
        }

        //---

        if (isAxesTickLabelTextVisible()) {
          // draw major tick label
          double mpos1 = mapPos(pos1);

          if (mpos1 >= amin && mpos1 <= amax) {
            drawTickLabel(plot, device, apos1, mpos1, pos1, isTickInside());
          }
        }

        //---

        if (isDate())
          pos1 = interval_.interval(int(i + 1));
        else
          pos1 += inc;
      }
    }
  }

  drawAxisTickLabelDatas(plot, device);

  //---

  // fix range if not set
  if (! lbbox_.isSet()) {
    auto a1 = windowToPixel(plot, amin, apos1);
    auto a2 = windowToPixel(plot, amax, apos1);

    if (isHorizontal()) {
      bool isPixelBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                           (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

      double dys = (isPixelBottom ? 1 : -1);

      a2.y += dys*(tlen2 + tgap);

      lbbox_ += Point(a1.x, a1.y);
      lbbox_ += Point(a2.x, a2.y);
    }
    else {
      bool isPixelLeft = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertX()) ||
                         (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertX());

      double dxs = (isPixelLeft ? 1 : -1);

      a2.x += dxs*(tlen2 + tgap);

      lbbox_ += Point(a1.x, a1.y);
      lbbox_ += Point(a2.x, a2.y);
    }
  }

  //---

  if (isAxesLabelTextVisible()) {
    auto text      = userLabel();
    bool allowHtml = true;

    if (! text.length()) {
      text      = label().string();
      allowHtml = false;
    }

    if (! text.length())
      text = label().defValue();

    drawAxisLabel(plot, device, apos1, amin, amax, text, allowHtml);
  }

  //---

  if (plot->showBoxes()) {
    plot->drawWindowColorBox(device, bbox_, Qt::blue);

    plot->drawColorBox(device, lbbox_, Qt::green);
  }

  //---

  device->restore();

  //----

  double extent = std::max(maxFitExtent()/100.0, 0.0);

  double fitMin = fitBBox_.getMinExtent(isHorizontal());
  double fitMax = fitBBox_.getMaxExtent(isHorizontal());
  double fitLen = fitMax - fitMin;

  auto adjustLabelFitBox = [&](BBox &bbox) {
    if (fitLen <= 0) return bbox;

    double boxMin = bbox.getMinExtent(isHorizontal());
    double boxMax = bbox.getMaxExtent(isHorizontal());

    double f1 = (fitMin - boxMin)/fitLen;
    double f2 = (boxMax - fitMax)/fitLen;

    if (f1 <= extent && f2 <= extent)
      return bbox;

    auto bbox1 = bbox;

    if (f1 > extent)
      bbox1.setMinExtent(isHorizontal(), fitMin - extent*fitLen);

    if (f2 > extent)
      bbox1.setMaxExtent(isHorizontal(), fitMax + extent*fitLen);

    bbox1.update();

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

  auto *th = const_cast<CQChartsAxis *>(this);

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
      pos = interval_.interval(int(i + 1));
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

  auto dataRange = plot->calcDataRange();

  if (dataRange.isSet())
    dataRange += plot->annotationBBox();
  else
    dataRange = BBox(0.0, 0.0, 1.0, 1.0);

  //---

  if (isHorizontal()) {
    bool isWindowBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT);

    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    apos1 = (isWindowBottom ? ymin : ymax);
    apos2 = (isWindowBottom ? ymax : ymin);
  }
  else {
    bool isWindowLeft = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT);

    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    apos1 = (isWindowLeft ? xmin : xmax);
    apos2 = (isWindowLeft ? xmax : xmin);
  }
}

void
CQChartsAxis::
drawLine(const CQChartsPlot *, CQChartsPaintDevice *device, double apos, double amin, double amax)
{
  CQChartsPenBrush penBrush;

  auto lc = interpAxesLinesColor(ColorInd());

  plot_->setPen(penBrush,
    PenData(true, lc, axesLinesAlpha(), axesLinesWidth(), axesLinesDash()));

  device->setPen(penBrush.pen);

  //---

  if (isHorizontal())
    device->drawLine(Point(amin, apos), Point(amax, apos));
  else
    device->drawLine(Point(apos, amin), Point(apos, amax));
}

void
CQChartsAxis::
drawMajorGridLine(const CQChartsPlot *, CQChartsPaintDevice *device,
                  double apos, double dmin, double dmax)
{
  CQChartsPenBrush penBrush;

  auto lc = interpAxesMajorGridLinesColor(ColorInd());

  plot_->setPen(penBrush,
    PenData(true, lc, axesMajorGridLinesAlpha(), axesMajorGridLinesWidth(),
            axesMajorGridLinesDash()));

  device->setPen(penBrush.pen);

  //---

  if (isHorizontal())
    device->drawLine(Point(apos, dmin), Point(apos, dmax));
  else
    device->drawLine(Point(dmin, apos), Point(dmax, apos));
}

void
CQChartsAxis::
drawMinorGridLine(const CQChartsPlot *, CQChartsPaintDevice *device,
                  double apos, double dmin, double dmax)
{
  CQChartsPenBrush penBrush;

  auto lc = interpAxesMinorGridLinesColor(ColorInd());

  plot_->setPen(penBrush,
    PenData(true, lc, axesMinorGridLinesAlpha(), axesMinorGridLinesWidth(),
            axesMinorGridLinesDash()));

  device->setPen(penBrush.pen);

  //---

  if (isHorizontal())
    device->drawLine(Point(apos, dmin), Point(apos, dmax));
  else
    device->drawLine(Point(dmin, apos), Point(dmax, apos));
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

  Point pp;

  if (isHorizontal()) {
    if (major && tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
      pp = Point(tpos - 0.5, apos);
    else
      pp = Point(tpos, apos);
  }
  else {
    if (major && tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
      pp = Point(apos, tpos - 0.5);
    else
      pp = Point(apos, tpos);
  }

  //---

  CQChartsPenBrush penBrush;

  auto lc = interpAxesLinesColor(ColorInd());

  plot_->setPen(penBrush,
   PenData(true, lc, axesLinesAlpha(), axesLinesWidth(), axesLinesDash()));

  device->setPen(penBrush.pen);

  //---

  if (isHorizontal()) {
    bool isWindowBottom = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT);
    bool isPixelBottom  = (side() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! plot->isInvertY()) ||
                          (side() == CQChartsAxisSide::Type::TOP_RIGHT   &&   plot->isInvertY());

    int pys = (isPixelBottom ? 1 : -1);
    int dt1 = pys*tlen;

    double adt1 = plot->pixelToWindowHeight(dt1);

    if (inside)
      device->drawLine(Point(pp.x, pp.y), Point(pp.x, pp.y + adt1));
    else {
      device->drawLine(Point(pp.x, pp.y), Point(pp.x, pp.y - adt1));

      Point p;

      if (isWindowBottom)
        p = Point(tpos, apos - adt1);
      else
        p = Point(tpos, apos + adt1);

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
      device->drawLine(Point(pp.x, pp.y), Point(pp.x + adt1, pp.y));
    else {
      device->drawLine(Point(pp.x, pp.y), Point(pp.x - adt1, pp.y));

      Point p;

      if (isWindowLeft)
        p = Point(apos - adt1, tpos);
      else
        p = Point(apos + adt1, tpos);

      bbox_    += p;
      fitBBox_ += p;
    }
  }
}

void
CQChartsAxis::
drawTickLabel(const CQChartsPlot *plot, CQChartsPaintDevice *device,
              double apos, double tpos, double value, bool inside)
{
  auto text = valueStr(plot, value);
  if (! text.length()) return;

  //---

  int tgap  = 2;
  int tlen1 = majorTickLen();
  int tlen2 = minorTickLen();

  auto pp = windowToPixel(plot, tpos, apos);

  //---

#if 0
  QPen tpen;

  auto tc = interpAxesTickLabelTextColor(ColorInd());

  plot->setPen(tpen, true, tc, axesTickLabelTextAlpha());

  device->setPen(tpen);
#endif

  view()->setPlotPainterFont(plot, device, axesTickLabelTextFont());

  auto angle      = axesTickLabelTextAngle();
  auto clipLength = plot_->lengthPixelWidth(axesTickLabelTextClipLength());
  auto clipElide  = axesTickLabelTextClipElide();

  QFontMetricsF fm(device->font());

  auto text1 = CQChartsDrawUtil::clipTextToLength(text, device->font(), clipLength, clipElide);

  double ta = fm.ascent();
  double td = fm.descent();

  double tw = fm.width(text1); // TODO: support HTML

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

    BBox tbbox;

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

      Point pt(pp.x, pp.y + tyo);

      if (angle.isZero()) {
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

        lbbox_ += Point(pt.x, pt.y            );
        lbbox_ += Point(pt.x, pt.y + (ta + td));

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
          tbbox = BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = BBox(xpos, ypos - wth, xpos + atw, ypos);
      }
      else {
        CQChartsTextOptions options;

        options.angle      = angle;
        options.align      = align;
        options.clipLength = clipLength;
        options.clipElide  = clipElide;

        auto rrect = CQChartsRotatedText::calcBBox(pt.x, pt.y, text, device->font(),
                                                   options, 0, /*alignBox*/true);

        lbbox_ += rrect;

        tbbox = plot->pixelToWindow(rrect);
      }

#if 0
      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }
#endif

      if (angle.isZero()) {
        double ty = pt.y + ta;

        Point p;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          p = Point(pt.x - tw/2                         , ty);
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          p = Point(pt.x - tw                           , ty);
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          p = Point(pt.x                                , ty);
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          p = Point(pt.x - plot->windowToPixelWidth(0.5), ty);

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(p, tbbox, text));
      }
      else {
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

      Point pt(pp.x, pp.y - tyo);

      if (angle.isZero()) {
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

        lbbox_ += Point(pt.x, pt.y            );
        lbbox_ += Point(pt.x, pt.y - (ta + td));

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
          tbbox = BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = BBox(xpos, ypos - wth, xpos + atw, ypos);
      }
      else {
        CQChartsTextOptions options;

        options.angle      = angle;
        options.align      = align;
        options.clipLength = clipLength;
        options.clipElide  = clipElide;

        auto rrect = CQChartsRotatedText::calcBBox(pt.x, pt.y, text, device->font(),
                                                   options, 0, /*alignBox*/true);

        lbbox_ += rrect;

        tbbox = plot->pixelToWindow(rrect);
      }

#if 0
      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }
#endif

      if (angle.isZero()) {
        double ty = pt.y - td;

        Point p;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          p = Point(pt.x - tw/2                         , ty);
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          p = Point(pt.x - tw                           , ty);
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          p = Point(pt.x                                , ty);
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          p = Point(pt.x - plot->windowToPixelWidth(0.5), ty);

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(p, tbbox, text));
      }
      else {
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

    BBox tbbox;

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

      Point pt(pp.x - txo, pp.y);

      if (angle.isZero()) {
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

        lbbox_ += Point(pt.x     , pt.y);
        lbbox_ += Point(pt.x - tw, pt.y);

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
          tbbox = BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = BBox(xpos - atw, ypos, xpos, ypos + wth);
      }
      else {
        CQChartsTextOptions options;

        options.angle      = angle;
        options.align      = align;
        options.clipLength = clipLength;
        options.clipElide  = clipElide;

        auto rrect = CQChartsRotatedText::calcBBox(pt.x, pt.y, text, device->font(),
                                                   options, 0, /*alignBox*/true);

        lbbox_ += rrect;

        tbbox = plot->pixelToWindow(rrect);
      }

#if 0
      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }
#endif

      if (angle.isZero()) {
        double tx = pt.x - tw;

        Point p;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          p = Point(tx, pt.y + ta/2);
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          p = Point(tx, pt.y + ta  );
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          p = Point(tx, pt.y - td  );
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          p = Point(tx, pt.y - plot->windowToPixelHeight(0.5) + ta);

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(p, tbbox, text));
      }
      else {
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

      Point pt(pp.x + txo, pp.y);

      if (angle.isZero()) {
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

        lbbox_ += Point(pt.x     , pt.y);
        lbbox_ += Point(pt.x + tw, pt.y);

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
          tbbox = BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = BBox(xpos - atw, ypos, xpos, ypos + wth);
      }
      else {
        CQChartsTextOptions options;

        options.angle      = angle;
        options.align      = align;
        options.clipLength = clipLength;
        options.clipElide  = clipElide;

        auto rrect = CQChartsRotatedText::calcBBox(pt.x, pt.y, text, device->font(),
                                                   options, 0, /*alignBox*/true);

        lbbox_ += rrect;

        tbbox = plot->pixelToWindow(rrect);
      }

#if 0
      if (isTickLabelAutoHide()) {
        if (lastTickLabelRect_.isSet() && lastTickLabelRect_.overlaps(tbbox))
          visible = false;
      }
#endif

      if (angle.isZero()) {
        double tx = pt.x;

        Point p;

        if      (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          p = Point(tx, pt.y + ta/2);
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          p = Point(tx, pt.y + ta  );
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          p = Point(tx, pt.y - td  );
        else if (tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          p = Point(tx, pt.y - plot->windowToPixelHeight(0.5) + ta);

        axisTickLabelDrawDatas_.push_back(AxisTickLabelDrawData(p, tbbox, text));
      }
      else {
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
      const auto &firstBBox = axisTickLabelDrawDatas_[0    ].bbox;
      const auto &lastBBox  = axisTickLabelDrawDatas_[n - 1].bbox;

      // if first and last labels overlap then only draw first
      if (lastBBox.overlaps(firstBBox)) {
        for (int i = 1; i < n; ++i)
          axisTickLabelDrawDatas_[i].visible = false;
      }
      // otherwise draw first and last and clip others
      else {
        auto prevBBox = firstBBox;

        for (int i = 1; i < n - 1; ++i) {
          auto &data = axisTickLabelDrawDatas_[i];

          if (data.bbox.overlaps(prevBBox) || data.bbox.overlaps(lastBBox))
            data.visible = false;

          if (data.visible)
            prevBBox = data.bbox;
        }
      }
    }
  }

  //---

  CQChartsPenBrush tpenBrush;

  auto tc = interpAxesTickLabelTextColor(ColorInd());

  plot->setPen(tpenBrush, PenData(true, tc, axesTickLabelTextAlpha()));

  device->setPen(tpenBrush.pen);

  //view()->setPlotPainterFont(plot, device, axesTickLabelTextFont());

  //---

  QFontMetricsF fm(device->font());

  for (const auto &data : axisTickLabelDrawDatas_) {
    if (! data.visible)
      continue;

    auto p1 = plot->pixelToWindow(data.p);

    CQChartsTextOptions options;

    // html not supported (axis code controls string)
    options.angle         = data.angle;
    options.align         = data.align;
    options.contrast      = isAxesTickLabelTextContrast();
    options.contrastAlpha = axesTickLabelTextContrastAlpha();
    options.formatted     = isAxesTickLabelTextFormatted();
    options.clipLength    = plot_->lengthPixelWidth(axesTickLabelTextClipLength());
    options.clipElide     = axesTickLabelTextClipElide();

    CQChartsDrawUtil::drawTextAtPoint(device, p1, data.text, options, /*centered*/true);
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
drawAxisLabel(const CQChartsPlot *plot, CQChartsPaintDevice *device, double apos,
              double amin, double amax, const QString &text, bool allowHtml)
{
  if (! text.length())
    return;

  //---

  int tgap = 2;

  auto a1 = windowToPixel(plot, amin, apos);
  auto a2 = windowToPixel(plot, amax, apos);
  auto a3 = windowToPixel(plot, amin, apos);

  //---

  CQChartsPenBrush tpenBrush;

  auto tc = interpAxesLabelTextColor(ColorInd());

  plot->setPen(tpenBrush, PenData(true, tc, axesLabelTextAlpha()));

  device->setPen(tpenBrush.pen);

  view()->setPlotPainterFont(plot, device, axesLabelTextFont());

  auto clipLength = plot_->lengthPixelWidth(axesLabelTextClipLength());
  auto clipElide  = axesLabelTextClipElide();
  auto html       = ((allowHtml || isAllowHtmlLabels()) && isAxesLabelTextHtml());

  QFontMetricsF fm(device->font());

  auto text1 = CQChartsDrawUtil::clipTextToLength(text, device->font(), clipLength, clipElide);

  double ta = fm.ascent();
  double td = fm.descent();

  double tw = 0.0;

  if (html) {
    CQChartsTextOptions options;

    options.html = html;

    tw = CQChartsDrawUtil::calcTextSize(text1, device->font(), options).width();
  }
  else {
    tw = fm.width(text1);
  }

  BBox bbox;

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

      Point pt(axm, lbbox_.getYMax() + ta + tgap);

      // angle, align not supported
      // html and formatted not supported (axis code controls string)
      CQChartsTextOptions options;

      options.angle         = Angle();
      options.align         = Qt::AlignLeft;
      options.contrast      = isAxesLabelTextContrast();
      options.contrastAlpha = axesLabelTextContrastAlpha();
      options.html          = html;
      options.clipLength    = clipLength;
      options.clipElide     = clipElide;

      CQChartsDrawUtil::drawTextAtPoint(device, plot->pixelToWindow(pt), text,
                                        options, /*centered*/false);

      if (! plot_->isInvertY()) {
        bbox += Point((amin + amax)/2 - atw, apos - (ath      ));
        bbox += Point((amin + amax)/2 + atw, apos - (ath - wfh));
      }
      else {
        bbox += Point((amin + amax)/2 - atw, apos + (ath      ));
        bbox += Point((amin + amax)/2 + atw, apos + (ath - wfh));
      }

      fitLBBox_ += Point((amin + amax)/2, apos - (ath      ));
      fitLBBox_ += Point((amin + amax)/2, apos - (ath - wfh));
    }
    else {
      ath = plot->pixelToWindowHeight((a3.y - lbbox_.getYMin()) + tgap) + wfh;

      Point pt(axm, lbbox_.getYMin() - td - tgap);

      // angle, align not supported
      // html and formatted not supported (axis code controls string)
      CQChartsTextOptions options;

      options.angle         = Angle();
      options.align         = Qt::AlignLeft;
      options.contrast      = isAxesLabelTextContrast();
      options.contrastAlpha = axesLabelTextContrastAlpha();
      options.html          = html;
      options.clipLength    = clipLength;
      options.clipElide     = clipElide;

      CQChartsDrawUtil::drawTextAtPoint(device, plot->pixelToWindow(pt), text,
                                        options, /*centered*/false);

      if (! plot_->isInvertY()) {
        bbox += Point((amin + amax)/2 - atw, apos + (ath      ));
        bbox += Point((amin + amax)/2 + atw, apos + (ath - wfh));
      }
      else {
        bbox += Point((amin + amax)/2 - atw, apos - (ath      ));
        bbox += Point((amin + amax)/2 + atw, apos - (ath - wfh));
      }

      fitLBBox_ += Point((amin + amax)/2, apos + (ath      ));
      fitLBBox_ += Point((amin + amax)/2, apos + (ath - wfh));
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
    double ath = plot->pixelToWindowHeight(tw/2.0);

    if (isPixelLeft) {
      double aym = (a2.y + a1.y)/2.0 + tw/2.0;

      atw = plot->pixelToWindowWidth((a3.x - lbbox_.getXMin()) + tgap) + wfh;

    //double tx = lbbox_.getXMin() - tgap - td;
      double tx = lbbox_.getXMin() - tgap;

      CQChartsTextOptions options;

      options.angle         = Angle(90.0);
      options.align         = Qt::AlignLeft | Qt::AlignBottom;
      options.contrast      = isAxesLabelTextContrast();
      options.contrastAlpha = axesLabelTextContrastAlpha();
      options.html          = html;
      options.clipLength    = clipLength;
      options.clipElide     = clipElide;

      if (options.html) {
        auto p1 = plot->pixelToWindow(Point(lbbox_.getXMin(), (a2.y + a1.y)/2));

        CQChartsDrawUtil::drawTextAtPoint(device, p1, text, options, /*centered*/false,
                                          -tgap - td, 0.0);
      }
      else {
        auto p1 = plot->pixelToWindow(Point(tx, aym));

        CQChartsRotatedText::draw(device, p1, text, options, /*alignBBox*/false);
      }

      if (! plot_->isInvertX()) {
        bbox += Point(apos - (atw      ), (amin + amax)/2 - ath);
        bbox += Point(apos - (atw - wfh), (amin + amax)/2 + ath);
      }
      else {
        bbox += Point(apos + (atw      ), (amin + amax)/2 - ath);
        bbox += Point(apos + (atw - wfh), (amin + amax)/2 + ath);
      }

      fitLBBox_ += Point(apos - (atw      ), (amin + amax)/2);
      fitLBBox_ += Point(apos - (atw - wfh), (amin + amax)/2);
    }
    else {
      atw = plot->pixelToWindowWidth((lbbox_.getXMax() - a3.x) + tgap) + wfh;

#if 0
      double aym = (a2.y + a1.y)/2 - tw/2;

      double tx = lbbox_.getXMax() + tgap + td;
#else
      double aym = (a2.y + a1.y)/2 + tw/2;

      double tx = lbbox_.getXMax() + tgap + ta + td;
#endif

      CQChartsTextOptions options;

      options.angle         = Angle(90.0);
      options.align         = Qt::AlignLeft | Qt::AlignBottom;
      options.contrast      = isAxesLabelTextContrast();
      options.contrastAlpha = axesLabelTextContrastAlpha();
      options.html          = html;
      options.clipLength    = clipLength;
      options.clipElide     = clipElide;

      if (options.html) {
        auto p1 = plot->pixelToWindow(Point(lbbox_.getXMax(), (a2.y + a1.y)/2));

        CQChartsDrawUtil::drawTextAtPoint(device, p1, text, options, /*centered*/false,
                                          tgap + ta, 0.0);
      }
      else {
        auto p1 = plot->pixelToWindow(Point(tx, aym));

        CQChartsRotatedText::draw(device, p1, text, options, /*alignBBox*/false);
      }

      if (! plot_->isInvertX()) {
        bbox += Point(apos + (atw      ), (amin + amax)/2 - ath);
        bbox += Point(apos + (atw - wfh), (amin + amax)/2 + ath);
      }
      else {
        bbox += Point(apos - (atw      ), (amin + amax)/2 - ath);
        bbox += Point(apos - (atw - wfh), (amin + amax)/2 + ath);
      }

      fitLBBox_ += Point(apos + (atw      ), (amin + amax)/2);
      fitLBBox_ += Point(apos + (atw - wfh), (amin + amax)/2);
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
    return plot->windowToPixel(Point(x, y));
  else
    return plot->windowToPixel(Point(y, x));
}

//---

void
CQChartsAxis::
setBBox(const BBox &b)
{
  CQChartsUtil::testAndSet(bbox_, b, [&]() { redraw(); } );
}

CQChartsGeom::BBox
CQChartsAxis::
fitBBox() const
{
  return fitBBox_;
}
