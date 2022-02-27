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
#include <CQModelUtil.h>
#include <CQTclUtil.h>

#include <cstring>
#include <algorithm>

namespace {
int boolFactor(bool b) { return (b ? 1 : -1); }
}

//------

CQChartsAxis::
CQChartsAxis(const View *view, Qt::Orientation direction, double start, double end) :
 CQChartsObj(view->charts()),
 CQChartsObjAxesLineData         <CQChartsAxis>(this),
 CQChartsObjAxesTickLabelTextData<CQChartsAxis>(this),
 CQChartsObjAxesLabelTextData    <CQChartsAxis>(this),
 CQChartsObjAxesMajorGridLineData<CQChartsAxis>(this),
 CQChartsObjAxesMinorGridLineData<CQChartsAxis>(this),
 CQChartsObjAxesGridFillData     <CQChartsAxis>(this),
 view_(view), direction_(direction),
 start_(std::min(start, end)), end_(std::max(start, end)), calcStart_(start), calcEnd_(end)
{
  init();
}

CQChartsAxis::
CQChartsAxis(const Plot *plot, Qt::Orientation direction, double start, double end) :
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

  auto themeFg    = Color::makeInterfaceValue(1.0);
  auto themeGray1 = Color::makeInterfaceValue(0.7);
  auto themeGray2 = Color::makeInterfaceValue(0.3);
  auto themeGray3 = Color::makeInterfaceValue(0.3);

  setAxesLabelTextColor    (themeFg);
  setAxesTickLabelTextColor(themeFg);

  setAxesLinesColor(themeGray1);

  // init grid
  setAxesMajorGridLinesColor(themeGray2);
  setAxesMajorGridLinesDash (LineDash(LineDash::Lengths({2, 2}), 0));

  setAxesMinorGridLinesColor(themeGray2);
  setAxesMinorGridLinesDash (LineDash(LineDash::Lengths({2, 2}), 0));

  setAxesGridFillColor(themeGray3);
  setAxesGridFillAlpha(Alpha(0.5));

  needsCalc_ = true;

  //---

  setAxesTickLabelTextFont(Font().decFontSize(4));
}

//---

namespace {

template<typename T>
void swapT(CQChartsAxis *lhs, CQChartsAxis *rhs) {
  std::swap(*dynamic_cast<T *>(lhs), *dynamic_cast<T *>(rhs));
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

  std::swap(lhs->start_          , rhs->start_          );
  std::swap(lhs->end_            , rhs->end_            );
  std::swap(lhs->includeZero_    , rhs->includeZero_    );
  std::swap(lhs->allowHtmlLabels_, rhs->allowHtmlLabels_);
  std::swap(lhs->maxMajorTicks_  , rhs->maxMajorTicks_  );
  std::swap(lhs->tickIncrement_  , rhs->tickIncrement_  );
  std::swap(lhs->majorIncrement_ , rhs->majorIncrement_ );

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
  auto id = (plot() ? plot()->id() : view()->id());

  if (isHorizontal())
    return id + "/xaxis";
  else
    return id + "/yaxis";
}

CQChartsView *
CQChartsAxis::
view()
{
  return const_cast<View *>(plot() ? plot()->view() : view_);
}

const CQChartsView *
CQChartsAxis::
view() const
{
  return (plot() ? plot()->view() : view_);
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
addProperties(CQPropertyViewModel *model, const QString &path, const PropertyType &propertyTypes)
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

  if (CQChartsUtil::isFlagSet(propertyTypes, PropertyType::STATE)) {
    addProp(path, "visible"  , "", "Axis visible");
    addProp(path, "editable" , "", "Axis editable");
  }

  //---

  if (! (CQChartsUtil::isFlagSet(propertyTypes, PropertyType::ANNOTATION)))
    addProp(path, "direction", "", "Axis direction", true)->setEditable(false);

  addProp(path, "side"     , "", "Axis plot side");
  addProp(path, "valueType", "", "Axis value type");
  addProp(path, "format"   , "", "Axis tick value format string");

  addProp(path, "tickIncrement" , "", "Axis tick increment");
  addProp(path, "majorIncrement", "", "Axis tick major increment");

  if (! (CQChartsUtil::isFlagSet(propertyTypes, PropertyType::ANNOTATION))) {
    addProp(path, "start", "", "Axis start position", /*hidden*/true);
    addProp(path, "end"  , "", "Axis end position"  , /*hidden*/true);
  }

  addProp(path, "includeZero", "", "Axis force include zero", true);

  addProp(path, "valueStart", "", "Axis custom start position");
  addProp(path, "valueEnd"  , "", "Axis custom end position");

  addProp(path, "tickLabels"      , "", "Indexed Tick Labels", true);
  addProp(path, "customTickLabels", "", "Custom Tick Labels", true);

  addProp(path, "maxFitExtent", "", "Axis maximum extent percent for auto fit")->
    setMinValue(0.0);

  //---

  if (! (CQChartsUtil::isFlagSet(propertyTypes, PropertyType::ANNOTATION))) {
    auto posPath = path + "/position";

    addProp(posPath, "position", "value", "Axis position");
  }

  //---

  if (CQChartsUtil::isFlagSet(propertyTypes, PropertyType::STROKE)) {
    auto linePath = path + "/stroke";

    addStyleProp(linePath, "axesLineData"  , "style"  , "Axis stroke style", true);
    addStyleProp(linePath, "axesLines"     , "visible", "Axis stroke visible");
    addStyleProp(linePath, "axesLinesColor", "color"  , "Axis stroke color");
    addStyleProp(linePath, "axesLinesAlpha", "alpha"  , "Axis stroke alpha");
    addStyleProp(linePath, "axesLinesWidth", "width"  , "Axis stroke width");
    addStyleProp(linePath, "axesLinesDash" , "dash"   , "Axis stroke dash");
    addStyleProp(linePath, "axesLinesCap"  , "cap"    , "Axis stroke cap");
  //addStyleProp(linePath, "axesLinesJoin" , "join"   , "Axis stroke dash");
  }

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

  addProp(labelPath, "scaleLabelFont"  , "", "Scale label font to match length");
  addProp(labelPath, "scaleLabelExtent", "", "Extent to length for scale label font");

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

  addStyleProp(gridMajorStrokePath, "axesMajorGridLineData"  , "style",
               "Axis major grid stroke style", true);
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesColor", "color",
               "Axis major grid stroke color");
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesAlpha", "alpha",
               "Axis major grid stroke alpha");
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesWidth", "width",
               "Axis major grid stroke width");
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesDash" , "dash",
               "Axis major grid stroke dash");
  addStyleProp(gridMajorStrokePath, "axesMajorGridLinesCap"  , "cap",
               "Axis major grid stroke cap");
//addStyleProp(gridMajorStrokePath, "axesMajorGridLinesJoin" , "join",
//             "Axis major grid stroke join");

  addStyleProp(gridMinorStrokePath, "axesMinorGridLineData"  , "style",
               "Axis minor grid stroke style", true);
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesColor", "color",
               "Axis minor grid stroke color");
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesAlpha", "alpha",
               "Axis minor grid stroke alpha");
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesWidth", "width",
               "Axis minor grid stroke width");
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesDash" , "dash",
               "Axis minor grid stroke dash");
  addStyleProp(gridMinorStrokePath, "axesMinorGridLinesCap"  , "cap",
               "Axis minor grid stroke cap");
//addStyleProp(gridMinorStrokePath, "axesMinorGridLinesJoin" , "join",
//             "Axis minor grid stroke join");

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

//---

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

void
CQChartsAxis::
setValueRange(double start, double end)
{
  valueStart_ = OptReal(start);
  valueEnd_   = OptReal(end);

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

    strs1 << QString::number(p.first);
    strs1 << p.second;

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

    long value = CQChartsUtil::toInt(strs1[0], ok);
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

    strs1 << CQChartsUtil::realToString(p.first);
    strs1 << p.second;

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

    double value = CQChartsUtil::toReal(strs1[0], ok);
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
      auto *plot = const_cast<Plot *>(this->plot());

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

void
CQChartsAxis::
setScaleLabelFont(bool b)
{
  CQChartsUtil::testAndSet(scaleLabelFont_, b, [&]() { redraw(); } );
}

void
CQChartsAxis::
setScaleLabelExtent(const Length &l)
{
  CQChartsUtil::testAndSet(scaleLabelExtent_, l, [&]() { redraw(); } );
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

  if (isUpdatesEnabled())
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

  numMajorTicks_ = uint(std::max(interval_.calcNumMajor(), 1));
  numMinorTicks_ = uint(std::max(interval_.calcNumMinor(), 1));
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
valueStr(const Plot *plot, double pos) const
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

    valuePos = CQChartsVariant::fromInt(ipos);
  }

  if (formatStr_.length()) {
    QString str;

    if (CQChartsModelUtil::formatColumnTypeValue(plot->charts(), plot->model().data(),
                                                 column(), formatStr_, valuePos, str))
      return str;
  }

  if (column().isValid()) {
    QString str;

    if (plot->formatColumnValue(column(), valuePos, str))
      return str;

    if (isDataLabels()) {
      int row = int(pos);

      QModelIndex parent; // TODO: support parent

      ModelIndex columnInd(plot, row, column(), parent);

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
  auto *plot = const_cast<Plot *>(this->plot());

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
  auto *plot = const_cast<Plot *>(this->plot());

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
  auto *plot = const_cast<Plot *>(this->plot());

  plot->updateRange();
}

void
CQChartsAxis::
updatePlotRangeAndObjs()
{
  auto *plot = const_cast<Plot *>(this->plot());

  plot->updateRangeAndObjs();
}

CQChartsEditHandles *
CQChartsAxis::
editHandles() const
{
  if (! editHandles_) {
    auto *th = const_cast<CQChartsAxis *>(this);

    th->editHandles_ = std::make_unique<EditHandles>(plot(), EditHandles::Mode::MOVE);
  }

  return editHandles_.get();
}

//---

bool
CQChartsAxis::
editPress(const Point &p)
{
  editHandles()->setDragPos(p);

  double apos1, apos2;

  calcPos(plot(), apos1, apos2);

  setPosition(CQChartsOptReal(apos1));

  return true;
}

bool
CQChartsAxis::
editMove(const Point &p)
{
  const auto &dragPos = editHandles()->dragPos();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  double apos;

  if (isHorizontal())
    apos = position().realOr(0.0) + dy;
  else
    apos = position().realOr(0.0) + dx;

  setPosition(CQChartsOptReal(apos));

  editHandles()->setDragPos(p);

  redraw(/*wait*/false);

  return true;
}

bool
CQChartsAxis::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
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

void
CQChartsAxis::
setPen(PenBrush &penBrush, const CQChartsPenData &penData) const
{
  if (plot())
    plot()->setPen(penBrush, penData);
}

bool
CQChartsAxis::
isDrawGrid() const
{
  return (isMajorGridLinesDisplayed() || isMinorGridLinesDisplayed() || isMajorGridFilled());
}

void
CQChartsAxis::
drawGrid(const Plot *plot, PaintDevice *device) const
{
  if (! isDrawGrid())
    return;

  // skip draw if too many ticks
  if (numMajorTicks() >= maxMajorTicks())
    return;

  //---

  auto dataRange = plot->calcDataRange();

  // get axis range
  double amin = start();
  double amax = end  ();

  // get perp data range
  double dmin, dmax;
  dataRange.getXYRange(! isHorizontal(), dmin, dmax);

  //---

  device->save();

  //---

  double inc  = calcIncrement();
  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  //---

  // get start position
  auto getStartPos = [&]() {
    double pos;

    if (isDate()) {
      pos = interval_.interval(0);

      if (isGridMid())
        pos = (pos + interval_.interval(1))/2;
    }
    else {
      pos = calcStart();

      if (isGridMid())
        pos += inc/2.0;
    }

    return pos;
  };

  // get next position
  auto getNextPos = [&](uint i, double lastPos) {
    double nextPos;

    if (isDate()) {
      nextPos = interval_.interval(int(i + 1));

      if (isGridMid())
        nextPos = (nextPos + interval_.interval(int(i + 2)))/2;
    }
    else
      nextPos = lastPos + inc;

    return nextPos;
  };

  //---

  // draw fill
  if (isMajorGridFilled()) {
    auto dataRect = plot->pixelToWindow(plot->calcDataPixelRect()); // TODO: simplify

    device->setClipRect(dataRect);

    //---

    PenBrush penBrush;

    auto fillColor = interpAxesGridFillColor(ColorInd());

    plot->setPenBrush(penBrush,
      PenData(false), BrushData(true, fillColor, axesGridFillAlpha(), axesGridFillPattern()));

    //---

    double pos1 = getStartPos();
    double pos2 = pos1;

    for (uint i = 0; i < numMajorTicks() + 1; i++) {
      // fill on alternate gaps (i = 1, 3, ...)
      if (i & 1) {
        if (pos2 >= amin || pos1 <= amax) {
          // clip to axis range
          double pos3 = std::max(pos1, amin);
          double pos4 = std::min(pos2, amax);

          auto bbox = CQChartsGeom::makeDirBBox(! isHorizontal(), pos3, dmin, pos4, dmax);

          CQChartsDrawUtil::setPenBrush(device, penBrush);

          device->fillRect(bbox);
        }
      }

      //---

      // update start end/pos
      pos1 = pos2;
      pos2 = getNextPos(i, pos1);
    }
  }

  //---

  // draw grid lines
  if (isMajorGridLinesDisplayed() || isMinorGridLinesDisplayed()) {
    double pos1 = getStartPos();

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
          double pos2;

          if (isDate()) {
            auto nextPos = getNextPos(i, pos1);

            pos2 = CMathUtil::map(j, 0, numMinorTicks(), pos1, nextPos);
          }
          else
            pos2 = pos1 + (isLog() ? plot->logValue(j*inc1) : j*inc1);

          if (isIntegral() && ! CQModelUtil::isInteger(pos2))
            continue;

          // draw minor grid line
          if (pos2 >= amin && pos2 <= amax)
            drawMinorGridLine(plot, device, pos2, dmin, dmax);
        }
      }

      //---

      pos1 = getNextPos(i, pos1);
    }
  }

  //---

  device->restore();
}

void
CQChartsAxis::
drawAt(double pos, const Plot *plot, PaintDevice *device) const
{
  auto *th = const_cast<CQChartsAxis *>(this);

  auto position = CQChartsOptReal(pos);

  std::swap(th->position_, position);

  draw(plot, device);

  std::swap(th->position_, position);
}

void
CQChartsAxis::
draw(const View *view, PaintDevice *device, bool usePen, bool forceColor) const
{
  drawI(view, nullptr, device, usePen, forceColor);
}

void
CQChartsAxis::
draw(const Plot *plot, PaintDevice *device, bool usePen, bool forceColor) const
{
  drawI(nullptr, plot, device, usePen, forceColor);
}

void
CQChartsAxis::
drawI(const View *view, const Plot *plot, PaintDevice *device, bool usePen, bool forceColor) const
{
  assert(view || plot);

  usePen_     = usePen;
  forceColor_ = forceColor;
  savePen_    = device->pen();

  //---

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

  fitBBox_ = bbox();

  //---

  device->save();

  //---

  // axis line
  bool lineVisible = isAxesLines();

  if (usePen_)
    lineVisible = (savePen_.style() != Qt::NoPen);

  if (lineVisible)
    drawLine(plot, device, apos1, amin, amax);

  //---

  auto mapPos = [&](double pos) {
    if (! valueStart_.isSet() && ! valueEnd_.isSet())
      return pos;

    return CMathUtil::map(pos, valueStart_.realOr(start()), valueEnd_.realOr(end()),
                          start(), end());
  };

  auto mapLen = [&](double len) {
    return std::abs(mapPos(len) - mapPos(0.0));
  };

  //---

  double inc  = calcIncrement();
  double inc1 = (isLog() ? plot->expValue(inc) : inc)/numMinorTicks();

  double pinc1;

  if (isHorizontal())
    pinc1 = windowToPixelWidth(plot, device, mapLen(inc1));
  else
    pinc1 = windowToPixelHeight(plot, device, mapLen(inc1));

  bool hideMinorTicks = (pinc1 < 3);

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

  textPlacer_.clear();

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
      double pos = double(p.first);

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
      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN ? -0.5 : 0.0);

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
        if (isMinorTicksDisplayed() && i < numMajorTicks() && ! hideMinorTicks) {
          for (uint j = 1; j < numMinorTicks(); j++) {
            double pos2 = pos1 + (isLog() ? plot->logValue(j*inc1) : j*inc1);

            if (isIntegral() && ! CQModelUtil::isInteger(pos2))
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
    auto a1 = windowToPixel(plot, device, amin, apos1);
    auto a2 = windowToPixel(plot, device, amax, apos1);

    if (isHorizontal()) {
      bool invertY = (plot ? plot->isInvertY() : false);

      bool isPixelBottom =
        (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! invertY) ||
        (side().type() == CQChartsAxisSide::Type::TOP_RIGHT   &&   invertY);

      double dys = (isPixelBottom ? 1 : -1);

      a2.y += dys*(tlen2 + tgap);

      lbbox_ += Point(a1.x, a1.y);
      lbbox_ += Point(a2.x, a2.y);
    }
    else {
      bool invertX = (plot ? plot->isInvertX() : false);

      bool isPixelLeft =
        (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! invertX) ||
        (side().type() == CQChartsAxisSide::Type::TOP_RIGHT   &&   invertX);

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

  if (plot && plot->showBoxes()) {
    plot->drawWindowColorBox(device, bbox(), Qt::blue);

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

  //---

  usePen_     = false;
  forceColor_ = false;

  //---

  auto *th = const_cast<CQChartsAxis *>(this);

  th->rect_ = bbox_;
}

void
CQChartsAxis::
drawEditHandles(PaintDevice *device) const
{
  assert(view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  setEditHandlesBBox();

  editHandles()->draw(device);
}

void
CQChartsAxis::
setEditHandlesBBox() const
{
  auto *th = const_cast<CQChartsAxis *>(this);

  th->editHandles()->setBBox(this->bbox());
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
calcPos(const Plot *plot, double &apos1, double &apos2) const
{
  if (position().isSet()) {
    apos1 = position().real();
    apos2 = apos1;
    return;
  }

  //---

  if (! plot) {
    apos1 = 0.0;
    apos2 = 1.0;
    return;
  }

  auto dataRange = plot->calcDataRange();

  if (dataRange.isSet())
    dataRange += plot->extraFitBBox();
  else
    dataRange = BBox(0.0, 0.0, 1.0, 1.0);

  //---

  if (isHorizontal()) {
    bool isWindowBottom = (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT);

    double ymin = dataRange.getYMin();
    double ymax = dataRange.getYMax();

    apos1 = (isWindowBottom ? ymin : ymax);
    apos2 = (isWindowBottom ? ymax : ymin);
  }
  else {
    bool isWindowLeft = (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT);

    double xmin = dataRange.getXMin();
    double xmax = dataRange.getXMax();

    apos1 = (isWindowLeft ? xmin : xmax);
    apos2 = (isWindowLeft ? xmax : xmin);
  }
}

void
CQChartsAxis::
drawLine(const Plot *, PaintDevice *device, double apos, double amin, double amax) const
{
  PenBrush penBrush;

  setAxesLineDataPen(penBrush.pen, ColorInd());

  if (! usePen_)
    device->setPen(penBrush.pen);
  else
    device->setPen(savePen_);

  //---

  if (isHorizontal())
    device->drawLine(Point(amin, apos), Point(amax, apos));
  else
    device->drawLine(Point(apos, amin), Point(apos, amax));
}

void
CQChartsAxis::
drawMajorGridLine(const Plot *, PaintDevice *device, double apos, double dmin, double dmax) const
{
  PenBrush penBrush;

  setAxesMajorGridLineDataPen(penBrush.pen, ColorInd());

  if (forceColor_)
    penBrush.pen.setColor(savePen_.color());

  device->setPen(penBrush.pen);

  //---

  if (isHorizontal())
    device->drawLine(Point(apos, dmin), Point(apos, dmax));
  else
    device->drawLine(Point(dmin, apos), Point(dmax, apos));
}

void
CQChartsAxis::
drawMinorGridLine(const Plot *, PaintDevice *device, double apos, double dmin, double dmax) const
{
  PenBrush penBrush;

  setAxesMinorGridLineDataPen(penBrush.pen, ColorInd());

  if (forceColor_)
    penBrush.pen.setColor(savePen_.color());

  device->setPen(penBrush.pen);

  //---

  if (isHorizontal())
    device->drawLine(Point(apos, dmin), Point(apos, dmax));
  else
    device->drawLine(Point(dmin, apos), Point(dmax, apos));
}

void
CQChartsAxis::
drawMajorTickLine(const Plot *plot, PaintDevice *device,
                  double apos, double tpos, bool inside) const
{
  drawTickLine(plot, device, apos, tpos, inside, /*major*/true);
}

void
CQChartsAxis::
drawMinorTickLine(const Plot *plot, PaintDevice *device,
                  double apos, double tpos, bool inside) const
{
  drawTickLine(plot, device, apos, tpos, inside, /*major*/false);
}

void
CQChartsAxis::
drawTickLine(const Plot *plot, PaintDevice *device,
             double apos, double tpos, bool inside, bool major) const
{
  int tlen = (major ? majorTickLen() : minorTickLen());

  Point pp;

  if (isHorizontal()) {
    if (major && tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
      pp = Point(tpos - 0.5, apos);
    else
      pp = Point(tpos, apos);
  }
  else {
    if (major && tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
      pp = Point(apos, tpos - 0.5);
    else
      pp = Point(apos, tpos);
  }

  //---

  PenBrush penBrush;

  setAxesLineDataPen(penBrush.pen, ColorInd());

  if (! usePen_)
    device->setPen(penBrush.pen);
  else
    device->setPen(savePen_);

  //---

  if (isHorizontal()) {
    bool invertY = (plot ? plot->isInvertY() : false);

    bool isWindowBottom = (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT);
    bool isPixelBottom  =
      (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! invertY) ||
      (side().type() == CQChartsAxisSide::Type::TOP_RIGHT   &&   invertY);

    int pys = (isPixelBottom ? 1 : -1);
    int dt1 = pys*tlen;

    double adt1 = pixelToWindowHeight(plot, device, dt1);

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
    bool invertX = (plot ? plot->isInvertX() : false);

    bool isWindowLeft = (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT);
    bool isPixelLeft  =
      (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! invertX) ||
      (side().type() == CQChartsAxisSide::Type::TOP_RIGHT   &&   invertX);

    int pxs = (isPixelLeft ? -1 : 1);
    int dt1 = pxs*tlen;

    double adt1 = pixelToWindowWidth(plot, device, dt1);

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
drawTickLabel(const Plot *plot, PaintDevice *device,
              double apos, double tpos, double value, bool inside) const
{
  auto text = valueStr(plot, value);
  if (! text.length()) return;

  //---

  int tgap  = 2;
  int tlen1 = majorTickLen();
  int tlen2 = minorTickLen();

  auto pp = windowToPixel(plot, device, tpos, apos);

  //---

  if (plot)
    plot->setPainterFont(device, axesTickLabelTextFont());
  else
    device->setFont(axesTickLabelTextFont().font());

  auto angle      = axesTickLabelTextAngle();
  auto clipLength = lengthPixelWidth(plot, device, axesTickLabelTextClipLength());
  auto clipElide  = axesTickLabelTextClipElide();

  QFontMetricsF fm(device->font());

  auto text1 = CQChartsDrawUtil::clipTextToLength(text, device->font(), clipLength, clipElide);

  double ta = fm.ascent();
  double td = fm.descent();

  double tw = fm.horizontalAdvance(text1); // TODO: support HTML

  if (isHorizontal()) {
    bool invertY = (plot ? plot->isInvertY() : false);

    bool isPixelBottom =
      (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! invertY) ||
      (side().type() == CQChartsAxisSide::Type::TOP_RIGHT   &&   invertY);

    double tyo = 0.0;

    if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
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
      if (! invertY)
        align |= Qt::AlignTop;
      else
        align |= Qt::AlignBottom;
*/
      align |= Qt::AlignTop;

      Point pt(pp.x, pp.y + tyo);

      if (angle.isZero()) {
        double atw = pixelToWindowWidth (plot, device, tw);
        double wta = pixelToWindowHeight(plot, device, ta);
        double wtd = pixelToWindowHeight(plot, device, td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
          if (inside)
            atm = pixelToWindowHeight(plot, device, tgap);
          else
            atm = pixelToWindowHeight(plot, device, tlen1 + tgap);
        }
        else {
          if (inside)
            atm = pixelToWindowHeight(plot, device, tgap);
          else
            atm = pixelToWindowHeight(plot, device, tlen2 + tgap);
        }

        lbbox_ += Point(pt.x, pt.y            );
        lbbox_ += Point(pt.x, pt.y + (ta + td));

        bool invertY = (plot ? plot->isInvertY() : false);

        double xpos = 0.0;
        double ypos = apos - boolFactor(! invertY)*(wth + atm);

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          xpos = tpos - atw/2;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          xpos = tpos - atw;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          xpos = tpos;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          xpos = tpos - 0.5;

        if (! invertY)
          tbbox = BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = BBox(xpos, ypos - wth, xpos + atw, ypos);
      }
      else {
        TextOptions options;

        options.angle      = angle;
        options.align      = align;
        options.clipLength = clipLength;
        options.clipElide  = clipElide;

        auto ptbbox = CQChartsRotatedText::calcBBox(pt.x, pt.y, text, device->font(),
                                                    options, 0, /*alignBox*/true);

        lbbox_ += ptbbox;

        tbbox = pixelToWindow(plot, device, ptbbox);
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

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          p = Point(pt.x - tw/2                         , ty);
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          p = Point(pt.x - tw                           , ty);
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          p = Point(pt.x                                , ty);
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          p = Point(pt.x - windowToPixelWidth(plot, device, 0.5), ty);

        textPlacer_.addDrawText(CQChartsAxisTextPlacer::DrawText(p, tbbox, text));
      }
      else {
        textPlacer_.addDrawText(CQChartsAxisTextPlacer::DrawText(pt, tbbox, text, angle, align));
      }

#if 0
      if (plot && plot->showBoxes()) {
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
      bool invertY = (plot ? plot->isInvertY() : false);

      if (! invertY)
        align |= Qt::AlignBottom;
      else
        align |= Qt::AlignTop;
*/
      align |= Qt::AlignBottom;

      Point pt(pp.x, pp.y - tyo);

      if (angle.isZero()) {
        double atw = pixelToWindowWidth (plot, device, tw);
        double wta = pixelToWindowHeight(plot, device, ta);
        double wtd = pixelToWindowHeight(plot, device, td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
          if (inside)
            atm = pixelToWindowHeight(plot, device, tgap);
          else
            atm = pixelToWindowHeight(plot, device, tlen1 + tgap);
        }
        else {
          if (inside)
            atm = pixelToWindowHeight(plot, device, tgap);
          else
            atm = pixelToWindowHeight(plot, device, tlen2 + tgap);
        }

        lbbox_ += Point(pt.x, pt.y            );
        lbbox_ += Point(pt.x, pt.y - (ta + td));

        bool invertY = (plot ? plot->isInvertY() : false);

        double xpos = 0.0;
        double ypos = apos + boolFactor(! invertY)*atm;

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          xpos = tpos - atw/2;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          xpos = tpos - atw;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          xpos = tpos;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          xpos = tpos - 0.5;

        if (! invertY)
          tbbox = BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = BBox(xpos, ypos - wth, xpos + atw, ypos);
      }
      else {
        TextOptions options;

        options.angle      = angle;
        options.align      = align;
        options.clipLength = clipLength;
        options.clipElide  = clipElide;

        auto ptbbox = CQChartsRotatedText::calcBBox(pt.x, pt.y, text, device->font(),
                                                    options, 0, /*alignBox*/true);

        lbbox_ += ptbbox;

        tbbox = pixelToWindow(plot, device, ptbbox);
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

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          p = Point(pt.x - tw/2                         , ty);
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          p = Point(pt.x - tw                           , ty);
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          p = Point(pt.x                                , ty);
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          p = Point(pt.x - windowToPixelWidth(plot, device, 0.5), ty);

        textPlacer_.addDrawText(CQChartsAxisTextPlacer::DrawText(p, tbbox, text));
      }
      else {
        textPlacer_.addDrawText(CQChartsAxisTextPlacer::DrawText(pt, tbbox, text, angle, align));
      }

#if 0
      if (plot && plot->showBoxes()) {
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
    bool invertX = (plot ? plot->isInvertX() : false);

    bool isPixelLeft =
      (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! invertX) ||
      (side().type() == CQChartsAxisSide::Type::TOP_RIGHT   &&   invertX);

    double txo = 0.0;

    if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
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
      if (! invertX)
        align |= Qt::AlignRight;
      else
        align |= Qt::AlignLeft;
*/
      align |= Qt::AlignRight;

      Point pt(pp.x - txo, pp.y);

      if (angle.isZero()) {
        double atw = pixelToWindowWidth (plot, device, tw);
        double wta = pixelToWindowHeight(plot, device, ta);
        double wtd = pixelToWindowHeight(plot, device, td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
          if (inside)
            atm = pixelToWindowWidth(plot, device, tgap);
          else
            atm = pixelToWindowWidth(plot, device, tlen1 + tgap);
        }
        else {
          if (inside)
            atm = pixelToWindowWidth(plot, device, tgap);
          else
            atm = pixelToWindowWidth(plot, device, tlen2 + tgap);
        }

        lbbox_ += Point(pt.x     , pt.y);
        lbbox_ += Point(pt.x - tw, pt.y);

        bool invertX = (plot ? plot->isInvertX() : false);

        double xpos = apos - boolFactor(! invertX)*(atw + atm);
        double ypos = 0.0;

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          ypos = tpos - wth/2;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          ypos = tpos - wth;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          ypos = tpos;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          ypos = tpos - 0.5 - wta;

        if (! invertX)
          tbbox = BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = BBox(xpos - atw, ypos, xpos, ypos + wth);
      }
      else {
        TextOptions options;

        options.angle      = angle;
        options.align      = align;
        options.clipLength = clipLength;
        options.clipElide  = clipElide;

        auto ptbbox = CQChartsRotatedText::calcBBox(pt.x, pt.y, text, device->font(),
                                                    options, 0, /*alignBox*/true);

        lbbox_ += ptbbox;

        tbbox = pixelToWindow(plot, device, ptbbox);
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

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          p = Point(tx, pt.y + ta/2);
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          p = Point(tx, pt.y + ta  );
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          p = Point(tx, pt.y - td  );
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          p = Point(tx, pt.y - windowToPixelHeight(plot, device, 0.5) + ta);

        textPlacer_.addDrawText(CQChartsAxisTextPlacer::DrawText(p, tbbox, text));
      }
      else {
        textPlacer_.addDrawText(CQChartsAxisTextPlacer::DrawText(pt, tbbox, text, angle, align));
      }

#if 0
      if (plot && plot->showBoxes()) {
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
        double atw = pixelToWindowWidth (plot, device, tw);
        double wta = pixelToWindowHeight(plot, device, ta);
        double wtd = pixelToWindowHeight(plot, device, td);
        double wth = wta + wtd;

        double atm;

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
          if (inside)
            atm = pixelToWindowWidth(plot, device, tgap);
          else
            atm = pixelToWindowWidth(plot, device, tlen1 + tgap);
        }
        else {
          if (inside)
            atm = pixelToWindowWidth(plot, device, tgap);
          else
            atm = pixelToWindowWidth(plot, device, tlen2 + tgap);
        }

        lbbox_ += Point(pt.x     , pt.y);
        lbbox_ += Point(pt.x + tw, pt.y);

        bool invertX = (plot ? plot->isInvertX() : false);

        double xpos = apos + boolFactor(! invertX)*atm;
        double ypos = 0.0;

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          ypos = tpos - wth/2;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          ypos = tpos - wth;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          ypos = tpos;
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          ypos = tpos - 0.5 - wta;

        if (! invertX)
          tbbox = BBox(xpos, ypos, xpos + atw, ypos + wth);
        else
          tbbox = BBox(xpos - atw, ypos, xpos, ypos + wth);
      }
      else {
        TextOptions options;

        options.angle      = angle;
        options.align      = align;
        options.clipLength = clipLength;
        options.clipElide  = clipElide;

        auto ptbbox = CQChartsRotatedText::calcBBox(pt.x, pt.y, text, device->font(),
                                                    options, 0, /*alignBox*/true);

        lbbox_ += ptbbox;

        tbbox = pixelToWindow(plot, device, ptbbox);
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

        if      (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::MIDDLE)
          p = Point(tx, pt.y + ta/2);
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BOTTOM_LEFT)
          p = Point(tx, pt.y + ta  );
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::TOP_RIGHT)
          p = Point(tx, pt.y - td  );
        else if (tickLabelPlacement().type() == CQChartsAxisTickLabelPlacement::Type::BETWEEN)
          p = Point(tx, pt.y - windowToPixelHeight(plot, device, 0.5) + ta);

        textPlacer_.addDrawText(CQChartsAxisTextPlacer::DrawText(p, tbbox, text));
      }
      else {
        textPlacer_.addDrawText(CQChartsAxisTextPlacer::DrawText(pt, tbbox, text, angle, align));
      }

#if 0
      if (plot && plot->showBoxes()) {
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
drawAxisTickLabelDatas(const Plot *plot, PaintDevice *device) const
{
  if (textPlacer_.empty())
    return;

  // clip overlapping labels
  if (isTickLabelAutoHide())
    textPlacer_.autoHide();

  //---

  PenBrush tpenBrush;

  auto tc = interpAxesTickLabelTextColor(ColorInd());

  if (plot)
    plot->setPen(tpenBrush, PenData(true, tc, axesTickLabelTextAlpha()));
  else
    CQChartsUtil::setPen(tpenBrush.pen, true, tc, axesTickLabelTextAlpha());

  if (forceColor_)
    tpenBrush.pen.setColor(savePen_.color());

  device->setPen(tpenBrush.pen);

  //---

  // html not supported (axis code controls string)
  auto textOptions = axesTickLabelTextOptions(device);

  textOptions.angle = Angle();
  textOptions.align = Qt::AlignHCenter | Qt::AlignVCenter;
  textOptions.html  = false;

  textPlacer_.draw(device, textOptions, (plot && plot->showBoxes()));
}

void
CQChartsAxis::
drawAxisLabel(const Plot *plot, PaintDevice *device, double apos,
              double amin, double amax, const QString &text, bool allowHtml) const
{
  if (! text.length())
    return;

  //---

  int tgap = 2;

  auto a1 = windowToPixel(plot, device, amin, apos);
  auto a2 = windowToPixel(plot, device, amax, apos);
  auto a3 = windowToPixel(plot, device, amin, apos);

  //---

  PenBrush tpenBrush;

  auto tc = interpAxesLabelTextColor(ColorInd());

  if (plot)
    plot->setPen(tpenBrush, PenData(true, tc, axesLabelTextAlpha()));
  else
    CQChartsUtil::setPen(tpenBrush.pen, true, tc, axesLabelTextAlpha());

  if (forceColor_)
    tpenBrush.pen.setColor(savePen_.color());

  device->setPen(tpenBrush.pen);

  if (plot)
    plot->setPainterFont(device, axesLabelTextFont());
  else
    device->setFont(axesLabelTextFont().font());

  //---

  auto html = ((allowHtml || isAllowHtmlLabels()) && isAxesLabelTextHtml());

  if (isScaleLabelFont()) {
    TextOptions options;

    options.html = html;

    auto psize = CQChartsDrawUtil::calcTextSize(text, device->font(), options);

    auto len = Length::plot(amax - amin);

    double plen = 0.0;

    if (isHorizontal())
      plen = lengthPixelWidth (plot, device, len) +
             lengthPixelWidth (plot, device, scaleLabelExtent());
    else
      plen = lengthPixelHeight(plot, device, len) +
             lengthPixelHeight(plot, device, scaleLabelExtent());

    double fontScale = (psize.width() > 0.0 ? plen/psize.width() : 1.0);

    if (fontScale < 1.0)
      device->setFont(CQChartsUtil::scaleFontSize(device->font(), fontScale));
  }

  //---

  auto clipLength = lengthPixelWidth(plot, device, axesLabelTextClipLength());
  auto clipElide  = axesLabelTextClipElide();

  QFontMetricsF fm(device->font());

  auto text1 = CQChartsDrawUtil::clipTextToLength(text, device->font(), clipLength, clipElide);

  double ta = fm.ascent();
  double td = fm.descent();

  double tw = 0.0;

  if (html) {
    TextOptions options;

    options.html = true;

    tw = CQChartsDrawUtil::calcTextSize(text1, device->font(), options).width();
  }
  else {
    tw = fm.horizontalAdvance(text1);
  }

  BBox bbox;

  // draw label
  if (isHorizontal()) {
    double wfh = pixelToWindowHeight(plot, device, ta + td);

    double axm = (a1.x + a2.x)/2 - tw/2;

    bool invertY = (plot ? plot->isInvertY() : false);

    bool isPixelBottom =
      (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! invertY) ||
      (side().type() == CQChartsAxisSide::Type::TOP_RIGHT   &&   invertY);

    //int pys = (isPixelBottom ? 1 : -1);

    double ath;
    double atw = pixelToWindowWidth(plot, device, tw/2);

    if (isPixelBottom) {
      ath = pixelToWindowHeight(plot, device, (lbbox_.getYMax() - a3.y) + tgap) + wfh;

      Point pt(axm, lbbox_.getYMax() + ta + tgap);

      // angle, align not supported
      // formatted not supported (axis code controls string)
      // html supported only for user label or annotation axis
      auto textOptions = axesLabelTextOptions(device);

      textOptions.angle      = Angle();
      textOptions.align      = Qt::AlignLeft;
      textOptions.html       = html;
      textOptions.clipLength = clipLength;
      textOptions.clipElide  = clipElide;

      if (textOptions.html) {
        textOptions.align = Qt::AlignCenter;

        auto p1 = pixelToWindow(plot, device,
                    Point(axm + tw/2, lbbox_.getYMax() + (ta + td)/2.0 + tgap));

        CQChartsDrawUtil::drawTextAtPoint(device, p1, text, textOptions, /*centered*/true);
      }
      else {
        auto p1 = pixelToWindow(plot, device, pt);

        CQChartsDrawUtil::drawTextAtPoint(device, p1, text, textOptions, /*centered*/false);
      }

      if (! invertY) {
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
      ath = pixelToWindowHeight(plot, device, (a3.y - lbbox_.getYMin()) + tgap) + wfh;

      Point pt(axm, lbbox_.getYMin() - td - tgap);

      // angle, align not supported
      // formatted not supported (axis code controls string)
      // html supported only for user label or annotation axis
      auto textOptions = axesLabelTextOptions(device);

      textOptions.angle      = Angle();
      textOptions.align      = Qt::AlignLeft;
      textOptions.html       = html;
      textOptions.clipLength = clipLength;
      textOptions.clipElide  = clipElide;

      if (textOptions.html) {
        textOptions.align = Qt::AlignCenter;

        auto p1 = pixelToWindow(plot, device,
                    Point(axm + tw/2, lbbox_.getYMin() - (ta + td)/2.0 - tgap));

        CQChartsDrawUtil::drawTextAtPoint(device, p1, text, textOptions, /*centered*/true);
      }
      else {
        auto p1 = pixelToWindow(plot, device, pt);

        CQChartsDrawUtil::drawTextAtPoint(device, p1, text, textOptions, /*centered*/false);
      }

      if (! invertY) {
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
    double wfa = pixelToWindowWidth(plot, device, ta);
    double wfd = pixelToWindowWidth(plot, device, td);
    double wfh = wfa + wfd;

    bool invertX = (plot ? plot->isInvertX() : false);

    bool isPixelLeft =
      (side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT && ! invertX) ||
      (side().type() == CQChartsAxisSide::Type::TOP_RIGHT   &&   invertX);

    //int pxs = (isPixelLeft ? 1 : -1);

    double atw;
    double ath = pixelToWindowHeight(plot, device, tw/2.0);

    if (isPixelLeft) {
      double aym = (a2.y + a1.y)/2.0 + tw/2.0;

      atw = pixelToWindowWidth(plot, device, (a3.x - lbbox_.getXMin()) + tgap) + wfh;

    //double tx = lbbox_.getXMin() - tgap - td;
      double tx = lbbox_.getXMin() - tgap;

      auto textOptions = axesLabelTextOptions(device);

      textOptions.angle      = Angle(90.0);
      textOptions.align      = Qt::AlignLeft | Qt::AlignBottom;
      textOptions.html       = html;
      textOptions.clipLength = clipLength;
      textOptions.clipElide  = clipElide;

      if (textOptions.html) {
        textOptions.align = Qt::AlignCenter;

        auto p1 = pixelToWindow(plot, device,
                    Point(lbbox_.getXMin() - tgap - (ta + td)/2.0, (a2.y + a1.y)/2));

        CQChartsDrawUtil::drawTextAtPoint(device, p1, text, textOptions, /*centered*/true);
      }
      else {
        auto p1 = pixelToWindow(plot, device, Point(tx, aym));

        CQChartsRotatedText::draw(device, p1, text, textOptions, /*alignBBox*/false);
      }

      if (! invertX) {
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
      atw = pixelToWindowWidth(plot, device, (lbbox_.getXMax() - a3.x) + tgap) + wfh;

#if 0
      double aym = (a2.y + a1.y)/2 - tw/2;

      double tx = lbbox_.getXMax() + tgap + td;
#else
      double aym = (a2.y + a1.y)/2 + tw/2;

      double tx = lbbox_.getXMax() + tgap + ta + td;
#endif

      auto textOptions = axesLabelTextOptions(device);

      textOptions.angle      = Angle(90.0);
      textOptions.align      = Qt::AlignLeft | Qt::AlignBottom;
      textOptions.html       = html;
      textOptions.clipLength = clipLength;
      textOptions.clipElide  = clipElide;

      if (textOptions.html) {
        textOptions.align = Qt::AlignCenter;

        auto p1 = pixelToWindow(plot, device,
                    Point(lbbox_.getXMax() + tgap + (ta + td)/2.0, (a2.y + a1.y)/2));

        CQChartsDrawUtil::drawTextAtPoint(device, p1, text, textOptions, /*centered*/true);
      }
      else {
        auto p1 = pixelToWindow(plot, device, Point(tx, aym));

        CQChartsRotatedText::draw(device, p1, text, textOptions, /*alignBBox*/false);
      }

      if (! invertX) {
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

  if (plot && plot->showBoxes()) {
    plot->drawWindowColorBox(device, bbox);
  }

  bbox_ += bbox;
}

//---

void
CQChartsAxis::
write(const CQPropertyViewModel *propertyModel, const QString &plotName, std::ostream &os)
{
  CQPropertyViewModel::NameValues nameValues;

  propertyModel->getChangedNameValues(this, nameValues, /*tcl*/true);

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str.clear();

    os << "set_charts_property -plot $" << plotName.toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}

//---

CQChartsGeom::Point
CQChartsAxis::
windowToPixel(const Plot *plot, PaintDevice *device, double x, double y) const
{
  if (isHorizontal())
    return windowToPixel(plot, device, Point(x, y));
  else
    return windowToPixel(plot, device, Point(y, x));
}

//---

CQChartsGeom::BBox
CQChartsAxis::
windowToPixel(const Plot *plot, PaintDevice *device, const BBox &p) const
{
  return (plot ? plot->windowToPixel(p) : device->windowToPixel(p));
}

CQChartsGeom::Point
CQChartsAxis::
windowToPixel(const Plot *plot, PaintDevice *device, const Point &p) const
{
  return (plot ? plot->windowToPixel(p) : device->windowToPixel(p));
}

double
CQChartsAxis::
windowToPixelWidth(const Plot *plot, PaintDevice *device, double p) const
{
  return (plot ? plot->windowToPixelWidth(p) : device->windowToPixelWidth(p));
}

double
CQChartsAxis::
windowToPixelHeight(const Plot *plot, PaintDevice *device, double p) const
{
  return (plot ? plot->windowToPixelHeight(p) : device->windowToPixelHeight(p));
}

CQChartsGeom::BBox
CQChartsAxis::
pixelToWindow(const Plot *plot, PaintDevice *device, const BBox &p) const
{
  return (plot ? plot->pixelToWindow(p) : device->pixelToWindow(p));
}

CQChartsGeom::Point
CQChartsAxis::
pixelToWindow(const Plot *plot, PaintDevice *device, const Point &p) const
{
  return (plot ? plot->pixelToWindow(p) : device->pixelToWindow(p));
}

double
CQChartsAxis::
pixelToWindowWidth(const Plot *plot, PaintDevice *device, double p) const
{
  return (plot ? plot->pixelToWindowWidth(p) : device->pixelToWindowWidth(p));
}

double
CQChartsAxis::
pixelToWindowHeight(const Plot *plot, PaintDevice *device, double p) const
{
  return (plot ? plot->pixelToWindowHeight(p) : device->pixelToWindowHeight(p));
}

double
CQChartsAxis::
lengthPixelWidth(const Plot *plot, PaintDevice *device, const Length &len) const
{
  return (plot ? plot->lengthPixelWidth(len) : device->lengthPixelWidth(len));
}

double
CQChartsAxis::
lengthPixelHeight(const Plot *plot, PaintDevice *device, const Length &len) const
{
  return (plot ? plot->lengthPixelHeight(len) : device->lengthPixelHeight(len));
}

//---

CQChartsGeom::BBox
CQChartsAxis::
fitBBox() const
{
  return fitBBox_;
}
