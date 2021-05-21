#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsPlotDrawUtil.h>
#include <CQChartsTextPlacer.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsHtml.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQEnumCombo.h>
#include <CMathRound.h>

#include <QMenu>
#include <QCheckBox>

CQChartsPiePlotType::
CQChartsPiePlotType()
{
}

void
CQChartsPiePlotType::
addParameters()
{
  startParameterGroup("Pie");

  // args: name, desc, propName, attributes, default
  addColumnsParameter("values", "Value", "valueColumns").
    setRequired().setNumeric().setPropPath("columns.values").setTip("Value column(s)");
  addColumnParameter("label", "Label", "labelColumn").
    setString().setBasic().setPropPath("columns.label").setTip("Custom label column");
  addColumnParameter("radius", "Radius", "radiusColumn").
    setNumeric().setPropPath("columns.radius").setTip("Custom radius column");
  addColumnParameter("keyLabel", "Key Label", "keyLabelColumn").
    setString().setPropPath("columns.keyLabel").setTip("Custom key label column");

  addEnumParameter("drawType", "Draw Type", "drawType").
    addNameValue("PIE"    , int(CQChartsPiePlot::DrawType::PIE)).
    addNameValue("TREEMAP", int(CQChartsPiePlot::DrawType::TREEMAP)).
    addNameValue("WAFFLE" , int(CQChartsPiePlot::DrawType::WAFFLE)).
    setTip("Draw type");

  addBoolParameter("separated", "Separated", "separated").setTip("Draw separated");
  addBoolParameter("donut"    , "Donut"    , "donut"    ).setTip("Draw donut");
  addBoolParameter("summary"  , "Summary"  , "summary"  ).setTip("Draw summary");
  addBoolParameter("dumbbell" , "Dumbbell" ,  "dumbbell").setTip("Draw dumbbell");
  addBoolParameter("count"    , "Count"    , "count"    ).setTip("Display value counts");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsPiePlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &str) { return CQChartsHtml::Str::img(str); };

  return CQChartsHtml().
   h2("Pie Plot").
    h3("Summary").
     p("Draw circle segments with diameter from a set of values.").
     p("The segments can be restricted to an inner radius and a label "
       "can be displated at the center of the circle.").
    h3("Columns").
     p("The values come from the " + B("Values") + " column.").
     p("Optional labels crom from the " + B("Labels") + " column.").
     p("A custom pie slice radius can be specified in the " + B("Radius") + " column.").
    h3("Options").
     p("All pie slices can start at a specified inner radius by enabling the " +
       B("Donut") + " option.").
     p("The label can include the value using the " + B("Count") + " option.").
    h3("Limitations").
     p("This plot does not support a user specified range, axes, lograithmic scales, "
       "or probing.").
     p("The plot does not support a X/Y axes.").
    h3("Example").
     p(IMG("images/piechart.png"));
}

// TODO: analyzeModel

CQChartsPlot *
CQChartsPiePlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsPiePlot(view, model);
}

//------

CQChartsPiePlot::
CQChartsPiePlot(View *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("pie"), model),
 CQChartsObjShapeData     <CQChartsPiePlot>(this),
 CQChartsObjGridLineData  <CQChartsPiePlot>(this),
 CQChartsObjGroupShapeData<CQChartsPiePlot>(this)
{
}

CQChartsPiePlot::
~CQChartsPiePlot()
{
  term();
}

//---

void
CQChartsPiePlot::
init()
{
  CQChartsGroupPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  setFillColor(Color(Color::Type::PALETTE));

  setGroupFillColor(Color(Color::Type::PALETTE));

  setGridLines(false);
  setGridLinesColor(Color(Color::Type::INTERFACE_VALUE, 0.5));

  //---

  textBox_ = createTextObj();

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  textBox_->setTextColor(Color(Color::Type::INTERFACE_VALUE, 1));

  //---

  addKey();

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsPiePlot::
term()
{
  delete textBox_;
}

//---

void
CQChartsPiePlot::
setLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();  } );
}

void
CQChartsPiePlot::
setValueColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();  } );
}

//---

void
CQChartsPiePlot::
setRadiusColumn(const Column &c)
{
  CQChartsUtil::testAndSet(radiusColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();  } );
}

void
CQChartsPiePlot::
setKeyLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(keyLabelColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();  } );
}

//---

CQChartsColumn
CQChartsPiePlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "label"   ) c = this->labelColumn();
  else if (name == "radius"  ) c = this->radiusColumn();
  else if (name == "keyLabel") c = this->keyLabelColumn();
  else                         c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsPiePlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "label"   ) this->setLabelColumn(c);
  else if (name == "radius"  ) this->setRadiusColumn(c);
  else if (name == "keyLabel") this->setKeyLabelColumn(c);
  else                         CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsPiePlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "values") c = this->valueColumns();
  else                  c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsPiePlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "values") this->setValueColumns(c);
  else                  CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsPiePlot::
setDrawType(const DrawType &drawType)
{
  CQChartsUtil::testAndSet(drawType_, drawType, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setTreeMap(bool b)
{
  setDrawType(b ? DrawType::TREEMAP : DrawType::PIE);
}

bool
CQChartsPiePlot::
calcTreeMap() const
{
  if (! isTreeMap())
    return false;

  if (! isSeparated() && numGroups() > 1)
    return false;

  return true;
}

//---

void
CQChartsPiePlot::
setWaffle(bool b)
{
  setDrawType(b ? DrawType::WAFFLE : DrawType::PIE);
}

bool
CQChartsPiePlot::
calcWaffle() const
{
  if (! isWaffle())
    return false;

  if (! isSeparated() && numGroups() > 1)
    return false;

  return true;
}

//---

bool
CQChartsPiePlot::
calcPie() const
{
  if (drawType() == DrawType::PIE)
    return true;

  if (calcTreeMap() || calcWaffle())
    return false;

  if (isDumbbell())
    return false;

  return true;
}

//---

void
CQChartsPiePlot::
setDonut(bool b)
{
  CQChartsUtil::testAndSet(donut_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

bool
CQChartsPiePlot::
calcDonut() const
{
  if (! isDonut())
    return false;

  if (calcTreeMap() || calcWaffle())
    return false;

  return true;
}

//---

void
CQChartsPiePlot::
setSummary(bool b)
{
  CQChartsUtil::testAndSet(summary_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setDumbbell(bool b)
{
  CQChartsUtil::testAndSet(dumbbell_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

bool
CQChartsPiePlot::
calcDumbbell() const
{
  if (! isDumbbell())
    return false;

  return (isSeparated() && ! isSummary());
}

//---

void
CQChartsPiePlot::
setCount(bool b)
{
  CQChartsUtil::testAndSet(count_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

void
CQChartsPiePlot::
setMinValue(double r)
{
  CQChartsUtil::testAndSet(minValue_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setMaxValue(double r)
{
  CQChartsUtil::testAndSet(maxValue_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setInnerRadius(double r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setOuterRadius(double r)
{
  CQChartsUtil::testAndSet(outerRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setLabelRadius(double r)
{
  CQChartsUtil::testAndSet(labelRadius_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setStartAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(startAngle_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setAngleExtent(const Angle &a)
{
  CQChartsUtil::testAndSet(angleExtent_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setGapAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(gapAngle_, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setAdjustText(bool b)
{
  CQChartsUtil::testAndSet(adjustText_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setSeparated(bool b)
{
  CQChartsUtil::testAndSet(separated_, b, [&]() { updateRangeAndObjs(); } );
}

bool
CQChartsPiePlot::
calcSeparated() const
{
  if (! isSeparated())
    return false;

  return (numGroups() > 1);
}

//---

bool
CQChartsPiePlot::
isTextVisible() const
{
  return textBox_->isTextVisible();
}

void
CQChartsPiePlot::
setTextVisible(bool b)
{
  textBox_->setTextVisible(b);
}

void
CQChartsPiePlot::
setRotatedText(bool b)
{
  CQChartsUtil::testAndSet(rotatedText_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
setExplodeSelected(bool b)
{
  CQChartsUtil::testAndSet(explodeData_.selected, b, [&]() { drawObjs(); } );
}

void
CQChartsPiePlot::
setExplodeRadius(double r)
{
  CQChartsUtil::testAndSet(explodeData_.radius, r, [&]() { drawObjs(); } );
}

//---

void
CQChartsPiePlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "valueColumns"  , "values"  , "Value columns");
  addProp("columns", "labelColumn"   , "label"   , "Label column");
  addProp("columns", "radiusColumn"  , "radius"  , "Radius column");
  addProp("columns", "keyLabelColumn", "keyLabel", "Key label column");

  addGroupingProperties();

  // options
  addProp("options", "drawType" , "", "Draw type");
  addProp("options", "separated", "", "Draw grouped pie charts separately");
  addProp("options", "donut"    , "", "Display as donut using inner radius");
  addProp("options", "summary"  , "", "Draw summary group");
  addProp("options", "dumbbell" , "", "Draw dumbbell");
  addProp("options", "count"    , "", "Show count of groups");

  addProp("options", "minValue", "", "Custom min value");
  addProp("options", "maxValue", "", "Custom max value");

  addProp("options", "innerRadius", "", "Inner radius for donut")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("options", "outerRadius", "", "Outer radius for donut")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("options", "startAngle" , "", "Start angle for first segment");
  addProp("options", "angleExtent", "", "Angle extent for pie segments");
  addProp("options", "gapAngle"   , "", "Gap angle");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // group style
  addFillProperties("group/fill"  , "groupFill"  , "Group");
  addLineProperties("group/stroke", "groupStroke", "Group");

  // grid
  addProp("grid", "gridLines", "visible", "Grid lines visible");

  addLineProperties("grid/stroke", "gridLines", "Grid");

  // explode
  addProp("explode", "explodeStyle"   , "style"   , "Explode style", true); // TODO
  addProp("explode", "explodeSelected", "selected", "Explode selected segments");
  addProp("explode", "explodeRadius"  , "radius"  , "Explode radius")->setMinValue(0.0);

  // labels
  addProp("labels", "textVisible", "visible", "Labels visible");
  addProp("labels", "labelRadius", "radius" , "Radius labels are drawn at")->setMinValue(0.0);
  addProp("labels", "rotatedText", "rotated", "Labels text is rotated to segment angle");

  addProp("labels", "adjustText", "adjustText", "Adjust text placement");

  textBox_->addTextDataProperties(propertyModel(), "labels/text", "Labels",
                                  CQChartsTextBoxObj::PropertyType::NOT_VISIBLE);

  textBox_->addBoxProperties(propertyModel(), "labels/box", "Labels");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

CQChartsGeom::Range
CQChartsPiePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsPiePlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsPiePlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  // value column required
  // name, id, color columns optional

  if (! checkNumericColumns(valueColumns(), "Value", /*required*/true))
    columnsValid = false;

  if (! checkColumn(labelColumn   (), "Label"    )) columnsValid = false;
  if (! checkColumn(radiusColumn  (), "Radius"   )) columnsValid = false;
  if (! checkColumn(keyLabelColumn(), "Key Label")) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  Range dataRange;

  Point c(0.0, 0.0);

  dataRange.updateRange(c);

  //---

  double r = std::max(1.0, labelRadius());

  auto   angle1 = startAngle();
  double alen   = CQChartsUtil::clampDegrees(angleExtent().value());
  auto   angle2 = CQChartsAngle(angle1.value() + alen);

  // add segment outside points
  dataRange.updateRange(CQChartsAngle::circlePoint(c, r, angle1));
  dataRange.updateRange(CQChartsAngle::circlePoint(c, r, angle2));

  // add intermediate points (every 90 degree point between outside points)
  double a1 = 90.0*CMathRound::RoundDownF(angle1.value()/90.0);

  if (angle1 < angle2) {
    for (double a = a1; a < angle2.value(); a += 90.0) {
      if (a > angle1.value() && a < angle2.value())
        dataRange.updateRange(CQChartsAngle::circlePoint(c, r, CQChartsAngle(a)));
    }
  }
  else {
    for (double a = a1; a > angle2.value(); a -= 90.0) {
      if (a > angle2.value() && a < angle1.value())
        dataRange.updateRange(CQChartsAngle::circlePoint(c, r, CQChartsAngle(a)));
    }
  }

  //---

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  //---

  // init grouping
  initGroupData(valueColumns(), labelColumn());

  //---

  return dataRange;
}

//------

CQChartsGeom::BBox
CQChartsPiePlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsPiePlot::calcExtraFitBBox");

  BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    auto *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

    if (! pieObj || ! pieObj->isVisible())
      continue;

    bbox += pieObj->extraFitBBox();
  }

  return bbox;
}

//------

bool
CQChartsPiePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsPiePlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsPiePlot *>(this);

  //---

  // calc group totals
  calcDataTotal();

  //---

  th->groupObjs_.clear();

  th->updateColumnNames();

  //---

  if (! valueColumns().isValid())
    return false;

  //---

  bool separated = calcSeparated();

  double ri = std::min(std::max(calcDonut() ? innerRadius() : 0.0, 0.0), 1.0);
  double ro = outerRadius();

  int ig = 0;
  int ng = numGroups();

  double dr;

  if (separated)
    dr = ro - ri;
  else
    dr = (ng > 0 ? (ro - ri)/ng : 0.0);

  double r = ro;

  std::vector<int> groupInds;

  this->getGroupInds(groupInds);

  for (const auto &groupInd : groupInds) {
    auto pg = th->groupDatas_.find(groupInd);

    if (pg == th->groupDatas_.end()) {
      auto groupName = groupIndName(groupInd);

      pg = th->groupDatas_.insert(pg, GroupDatas::value_type(groupInd, groupName));
    }

    const auto &groupData = (*pg).second;

    //---

    // create group obj
    BBox rect(center_.x - ro, center_.y - ro, center_.x + ro, center_.y + ro);

    auto *groupObj =
      createGroupObj(rect, ColorInd(groupInd, ng), groupData.name, ColorInd(ig, ng));

    groupObj->setColorIndex(ColorInd(groupInd, ng));

    groupObj->setDataTotal(groupData.dataTotal);
    groupObj->setNumValues(groupData.numValues);

    groupObj->setRadiusMax   (groupData.radiusMax);
    groupObj->setRadiusScaled(groupData.radiusScaled);

    // group object inside donut (objects outside group)
    if      (calcDonut()) {
      groupObj->setInnerRadius(0.0);
      groupObj->setOuterRadius(innerRadius());
    }
    // summary so single all groups at same location
    else if (isSummary()) {
      groupObj->setInnerRadius(ri);
      groupObj->setOuterRadius(ro);
    }
    // no summary then separate circle per group
    // if separated then r and dr are constant
    else {
      groupObj->setInnerRadius(r - dr);
      groupObj->setOuterRadius(r);
    }

  //groupObj->setVisible(isSummary());

    objs.push_back(groupObj);

    th->groupObjs_.push_back(groupObj);

    //---

    if (groupData.groupObj != groupObj) {
      auto &groupData1 = const_cast<GroupData &>(groupData);

      groupData1.groupObj = groupObj;
    }

    //---

    if (! separated)
      r -= dr;

    ++ig;
  }

  //---

  // if group only shows count then no pie objects needed
  bool isSummaryGroup = (isSummary() && ! calcDonut());

  // add individual value objects (not needed for count only - no donut)
  // TODO: separate option for summary count and donut count label

  if (! isSummaryGroup) {
    // init value sets
    //initValueSets();

    //---

    // process model data
    class PieVisitor : public ModelVisitor {
     public:
      PieVisitor(const CQChartsPiePlot *plot, PlotObjs &objs) :
       plot_(plot), objs_(objs) {
      }

      State visit(const QAbstractItemModel *, const VisitData &data) override {
        plot_->addRow(data, objs_);

        return State::OK;
      }

     private:
      const CQChartsPiePlot *plot_ { nullptr };
      PlotObjs&              objs_;
    };

    PieVisitor pieVisitor(this, objs);

    visitModel(pieVisitor);
  }

  //---

  adjustObjAngles();

  //---

  if (separated) {
    bool dumbbell = calcDumbbell();

    int ng = numGroupObjs();

    int nx, ny;

    if (! dumbbell) {
      ny = CMathRound::RoundNearest(std::sqrt(std::max(ng, 1)));
      nx = (ng + ny - 1)/std::max(ny, 1);
    }
    else {
      ny = ng;
      nx = 2;
    }

    double dx = 1.0/nx;
    double dy = 1.0/ny;
    double dr = std::min(dx, dy);

    int ig = 0;

    for (auto &groupObj : groupObjs_) {
      int ix, iy;

      if (! dumbbell) {
        ix = ig % nx;
        iy = ig / nx;
      }
      else {
        ix = 0;
        iy = ig;
      }

      double x = CMathUtil::map(ix, 0, nx - 1, -1.0 + dx, 1.0 - dx);
      double y = CMathUtil::map(iy, 0, ny - 1, -1.0 + dy, 1.0 - dy);

      groupObj->setCenter(Point(x, y));

      groupObj->setInnerRadius(0.0);
      groupObj->setOuterRadius(dr);

      ++ig;
    }
  }

  for (auto &plotObj : objs) {
    auto *groupObj = dynamic_cast<CQChartsPieGroupObj *>(plotObj);
    if (! groupObj) continue;

    int iv = 0;
    int nv = groupObj->numObjs();

    for (auto &pieObj : groupObj->objs()) {
      pieObj->setIv(ColorInd(iv, nv));

      ++iv;
    }
  }

  //---

  if      (calcTreeMap()) {
    using TreeMapPlace = CQChartsPlotDrawUtil::TreeMapPlace;

    for (auto &plotObj : objs) {
      auto *groupObj = dynamic_cast<CQChartsPieGroupObj *>(plotObj);
      if (! groupObj) continue;

      //---

      BBox bbox(0.0, 0.0, 1.0, 1.0);

      TreeMapPlace place(bbox);

      double dataTotal;

      if (maxValue() > 0.0)
        dataTotal = maxValue(); // assume max is sum e.g. values are percent

      for (const auto &obj : groupObj->objs()) {
        auto v = obj->value();

        place.addValue(v);

        dataTotal -= v;
      }

      if (dataTotal > 0)
        place.addValue(dataTotal);

      place.placeValues();

      //---

      place.processAreas([&](const BBox &bbox, const TreeMapPlace::IArea &iarea) {
        if (iarea.i >= 0 && iarea.i < groupObj->numObjs()) {
          auto *obj = groupObj->obj(iarea.i);

          obj->setTreeMapBBox(bbox);
        }
      });
    }
  }
  else if (calcWaffle()) {
    for (auto &plotObj : objs) {
      auto *groupObj = dynamic_cast<CQChartsPieGroupObj *>(plotObj);
      if (! groupObj) continue;

      double dataTotal = groupObj->dataTotal();

      if (maxValue() > 0.0)
        dataTotal = maxValue(); // assume max is sum e.g. values are percent

      int    nsum = 0;
      double err  = 0.0;

      for (const auto &obj : groupObj->objs()) {
        auto v = obj->value() + err;

        auto r = CMathUtil::map(v, 0.0, dataTotal, 0.0, 100.0);
        int  n = CMathRound::RoundNearest(r);

        err = r - n;

        obj->setWaffleStart(nsum);
        obj->setWaffleCount(n);

        nsum += n;
      }

      // TODO: remainder
    }
  }

  //---

  return true;
}

void
CQChartsPiePlot::
addRow(const ModelVisitor::VisitData &data, PlotObjs &objs) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  for (const auto &column : valueColumns()) {
    ModelIndex ind(th, data.row, column, data.parent);

    addRowColumn(ind, objs);
  }
}

void
CQChartsPiePlot::
addRowColumn(const ModelIndex &ind, PlotObjs &objs) const
{
  bool isSummaryGroup = (isSummary() && ! calcDonut());

  assert(! isSummaryGroup);

  auto *th = const_cast<CQChartsPiePlot *>(this);

  //---

  // get column value
  double value        = 1.0;
  bool   valueMissing = false;

  if (! getColumnSizeValue(ind, value, valueMissing))
    return;

  //---

  // get column radius
  double radius        = 0.0;
  bool   radiusMissing = false;

  bool hasRadius = false;

  if (radiusColumn().isValid()) {
    ModelIndex rind(th, ind.row(), radiusColumn(), ind.parent());

    hasRadius = getColumnSizeValue(rind, radius, radiusMissing);
  }

  //---

  // get value label (used for unique values in group)
  auto label = calcIndLabel(modelIndex(ind));

  //---

  // get key label
  auto keyLabel = label;

  if (keyLabelColumn().isValid()) {
    ModelIndex kind(th, ind.row(), keyLabelColumn(), ind.parent());

    bool ok;

    keyLabel = modelString(kind, ok);
  }

  //---

  // get group obj
  int groupInd = rowGroupInd(ind);

  auto pg = groupDatas_.find(groupInd);
  assert(pg != groupDatas_.end());

  const auto &groupData = (*pg).second;

  auto *groupObj = groupData.groupObj;

  //---

  // set radii
  // . if donut then object is from inner radius to outer radius
  // . if NOT donut then object is from 0.0 to outer radius
  double ri = std::min(std::max(calcDonut() ? innerRadius() : 0.0, 0.0), 1.0);
  double ro = outerRadius();
  double rv = ro;
  double rs = 1.0;

  bool radiusScaled = (groupObj && groupObj->isRadiusScaled());

  if (hasRadius && radiusScaled)
    rs = (groupObj->radiusMax() > 0.0 ? radius/groupObj->radiusMax() : 1.0);

  //---

  bool hidden = isIndexHidden(ind);

  //---

  auto dataInd  = modelIndex(ind);
  auto dataInd1 = normalizeIndex(dataInd);

  //---

  // get pie object (by label)
  auto *obj = (groupObj ? groupObj->lookupObj(label) : nullptr);

  if (! obj) {
    BBox rect(center_.x - ro, center_.y - ro, center_.x + ro, center_.y + ro);

    int objInd = (groupObj ? groupObj->numObjs() : 0);

    auto ig = (groupObj ? groupObj->ig() : ColorInd());

    obj = createPieObj(rect, dataInd1, ig);

    if (hidden)
      obj->setVisible(false);

    obj->setColorIndex(ColorInd(objInd, objInd + 1));

    obj->setInnerRadius(ri);
    obj->setOuterRadius(ro);
    obj->setValueRadius(rv);

    obj->setLabel  (label);
    obj->setValue  (value);
    obj->setMissing(valueMissing);

    if (hasRadius)
      obj->setOptRadius(radius);

    obj->setRadiusScale(rs);

    obj->setKeyLabel(keyLabel);

    //---

    Color color;

    if (colorColumnColor(ind.row(), ind.parent(), color))
      obj->setColor(color);

    //---

    objs.push_back(obj);

    if (groupObj)
      groupObj->addObject(obj);
  }
  else {
    // duplicate labels add to value (and radius ?)

    if (! valueMissing)
      obj->setValue(obj->value() + value);

    if (hasRadius) {
      if (obj->optRadius())
        radius += *obj->optRadius();

      if (radiusScaled) {
        double rs = (groupObj->radiusMax() > 0.0 ? radius/groupObj->radiusMax() : 1.0);

        obj->setRadiusScale(rs);
      }

      obj->setOptRadius(radius);
    }

    // TODO: add dataInd
  }
}

QString
CQChartsPiePlot::
calcIndLabel(const QModelIndex &ind) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  // get value label (used for unique values in group)
  ModelIndex lind(th, ind.row(), labelColumn(), ind.parent());

  bool ok;

  QString label;

  if (numGroups() > 1) {
    if (valueColumns().count() > 1 && ! isGroupHeaders())
      label = modelHHeaderString(lind.column(), ok);
    else
      label = modelString(lind, ok);
  }
  else {
    label = modelString(lind, ok);
  }

  if (! label.length())
    label = QString::number(ind.row());

  return label;
}

void
CQChartsPiePlot::
calcDataTotal() const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  th->values_.clear();

  th->groupDatas_.clear();

  // process model data
  class DataTotalVisitor : public ModelVisitor {
   public:
    DataTotalVisitor(const CQChartsPiePlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRowDataTotal(data);

      return State::OK;
    }

   private:
    const CQChartsPiePlot *plot_ { nullptr };
  };

  DataTotalVisitor dataTotalVisitor(this);

  visitModel(dataTotalVisitor);
}

void
CQChartsPiePlot::
addRowDataTotal(const ModelVisitor::VisitData &data) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  for (const auto &column : valueColumns()) {
    ModelIndex ind(th, data.row, column, data.parent);

    addRowColumnDataTotal(ind);
  }
}

void
CQChartsPiePlot::
addRowColumnDataTotal(const ModelIndex &ind) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  //---

  bool hidden = isIndexHidden(ind);

  //---

  // get group data for group ind (add if new)
  int groupInd = rowGroupInd(ind);

  auto pg = th->groupDatas_.find(groupInd);

  if (pg == th->groupDatas_.end()) {
    auto groupName = groupIndName(groupInd);

    pg = th->groupDatas_.insert(pg, GroupDatas::value_type(groupInd, GroupData(groupName)));
  }

  const auto &groupData = (*pg).second;

  //---

  // get value
  double value        = 1.0;
  bool   valueMissing = false;

  if (! getColumnSizeValue(ind, value, valueMissing))
    return;

  //---

  if (! valueMissing)
    th->values_.addValue(value);

  //---

  // sum values
  if (! hidden) {
    auto &groupData1 = const_cast<GroupData &>(groupData);

    ++groupData1.numValues;

    groupData1.dataTotal += value;
  }

  //---

  // get max radius
  if (radiusColumn().isValid()) {
    ModelIndex rind(th, ind.row(), radiusColumn(), ind.parent());

    double radius        = 0.0;
    bool   radiusMissing = false;

    if (getColumnSizeValue(rind, radius, radiusMissing)) {
      if (! hidden) {
        auto &groupData1 = const_cast<GroupData &>(groupData);

        groupData1.radiusScaled = true;
        groupData1.radiusMax    = std::max(groupData.radiusMax, radius);
      }
    }
  }
}

bool
CQChartsPiePlot::
getColumnSizeValue(const ModelIndex &ind, double &value, bool &missing) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  missing = false;
  value   = 1.0;

  auto columnType = columnValueType(ind.column());

  if (columnType == ColumnType::INTEGER || columnType == ColumnType::REAL) {
    bool ok;

    value = modelReal(ind, ok);

    // allow missing value in numeric column
    if (! ok) {
      missing = true;
      return true;
    }

    // TODO: check allow nan
    if (CMathUtil::isNaN(value)) {
      th->addDataError(ind, "Invalid value");
      return false;
    }
  }
  else {
    // try convert model string to real
    bool ok;

    value = modelReal(ind, ok);

    // string non-real -> 1.0
    if (! ok) {
      th->addDataError(ind, "Invalid value");
      return false;
    }
  }

  // size must be positive or zeor
  if (value < 0.0) {
    th->addDataError(ind, "Negative value");
    value = 1.0;
    return false;
  }

  return true;
}

void
CQChartsPiePlot::
adjustObjAngles() const
{
  double ri = std::min(std::max(calcDonut() ? innerRadius() : 0.0, 0.0), 1.0);
  double ro = outerRadius();

  bool isGrouped = (numGroups() > 1);

  bool isSummaryGroup = (isSummary() && ! calcDonut());

  // calc delta radius (grouped)
  int    ng = 1, nh = 0;
  double dr = 0.0;

  if (! isSummaryGroup) {
    if (isGrouped) {
      ng = 0;

      for (auto &groupObj : groupObjs_) {
        if (! isSetHidden(groupObj->groupInd().i))
          ++ng;
        else
          ++nh;
      }
    }

    dr = (ng > 0 ? (ro - ri)/ng : 0.0);
  }

  //---

  // get total values
  int totalValues = 0;

  for (auto &groupObj : groupObjs_) {
    totalValues += groupObj->numValues();
  }

  double da = (totalValues > 0 ? 360.0/totalValues : 0.0);

  //---

  bool separated = calcSeparated();

  // calc angle extents for each group
  double ga1 = startAngle().value();
  double ga2 = ga1;

  double r = ro;

  for (auto &groupObj : groupObjs_) {
    ga1 = ga2;

    // skip hidden groups
    if (! isSummaryGroup) {
      if (isSetHidden(groupObj->groupInd().i))
        continue;
    }

    //---

    // set group angles
    if (! separated) {
      double dga = da*groupObj->numValues();

      ga2 = ga1 - dga;

      groupObj->setAngles(Angle(ga1), Angle(ga2));
    }
    else {
      groupObj->setAngles(Angle(0.0), Angle(360.0));
    }

    //---

    bool radiusScaled = (groupObj && groupObj->isRadiusScaled());

    //---

    if (! isSummaryGroup) {
      // set group object inner/outer radii
      if (! separated && isGrouped) {
        groupObj->setInnerRadius(r - dr);
        groupObj->setOuterRadius(r);
      }

      //---

      // set segment angles
      double ga = gapAngle().value();

      double angle1 = startAngle().value();
      double alen   = CQChartsUtil::clampDegrees(angleExtent().value());

      double dataTotal = groupObj->dataTotal();

      if (maxValue() > 0.0)
        dataTotal = maxValue();

      int numObjs = groupObj->objs().size();

      if (abs(alen) >= 360.0) { // contiguous
        if (alen < 0)
          alen = std::min(alen + numObjs*ga, 0.0);
        else
          alen = std::max(alen - numObjs*ga, 0.0);
      }
      else {
        if (alen < 0)
          alen = std::min(alen + (numObjs - 1)*ga, 0.0);
        else
          alen = std::max(alen - (numObjs - 1)*ga, 0.0);
      }

      double da1 = (dataTotal > 0.0 ? alen/dataTotal : 0.0);

      for (auto &obj : groupObj->objs()) {
        // skip hidden objects
        if (! obj->isVisible())
          continue;

        //---

        // set angle based on value
        double value = obj->value();

        double angle  = da1*value;
        double angle2 = angle1 + angle;

        obj->setAngle1(Angle(angle1));
        obj->setAngle2(Angle(angle2));

        //---

        // set inner/outer radius and value radius
        if (! separated && isGrouped && calcDonut()) {
          obj->setInnerRadius(r - dr);
          obj->setOuterRadius(r);

          obj->setValueRadius(r);
        }

        //---

        if (obj->optRadius() && radiusScaled) {
          double rs = (groupObj->radiusMax() > 0.0 ? *obj->optRadius()/groupObj->radiusMax() : 1.0);

          obj->setRadiusScale(rs);
        }

        //---

        // move to next start angle
        angle1 = angle2 + ga;
      }

      //---

      // move to next radius
      if (! separated && isGrouped)
        r -= dr;
    }
  }
}

bool
CQChartsPiePlot::
isIndexHidden(const ModelIndex &ind) const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

  // hide all objects of group or individual objects of single group
  bool hidden = false;

  if (isColorKey() && colorColumn().isValid()) {
    ModelIndex colorInd(th, ind.row(), colorColumn(), ind.parent());

    bool ok;

    auto colorValue = modelValue(colorInd, ok);

    hidden = (ok && CQChartsVariant::cmp(hideValue(), colorValue) == 0);
  }
  else {
    if (numGroups() > 1) {
      int groupInd = rowGroupInd(ind);

      hidden = isSetHidden(groupInd);
    }
    else
      hidden = isSetHidden(ind.row());
  }

  return hidden;
}

void
CQChartsPiePlot::
addKeyItems(PlotKey *key)
{
  // start at next row (vertical) or next column (horizontal) from previous key
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyRow = [&](CQChartsPlotObj *obj) {
    auto *colorItem = new CQChartsPieKeyColor(this, obj);
    auto *textItem  = new CQChartsPieKeyText (this, obj);

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addRowItems(colorItem, textItem);

    key->addItem(groupItem, row, col);

    key->nextRowCol(row, col);
  };

  //---

  bool isSummaryGroup = (isSummary() && ! calcDonut());

  if (! isSummaryGroup) {
    int ng = groupObjs_.size();

    if (ng > 1) {
      for (const auto &groupObj : groupObjs_)
        addKeyRow(groupObj);
    }
    else {
      for (auto &plotObj : plotObjs_) {
        auto *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

        if (pieObj)
          addKeyRow(plotObj);
      }
    }
  }
  else {
    for (const auto &groupObj : groupObjs_)
      addKeyRow(groupObj);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsPiePlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//------

bool
CQChartsPiePlot::
addMenuItems(QMenu *menu)
{
  auto addCheckAction = [&](const QString &name, bool checked, const char *slotName) {
    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    connect(action, SIGNAL(triggered(bool)), this, slotName);

    menu->addAction(action);

    return action;
  };

  menu->addSeparator();

  addCheckAction("Donut"  , isDonut  (), SLOT(setDonut  (bool)));
  addCheckAction("TreeMap", isTreeMap(), SLOT(setTreeMap(bool)));
  addCheckAction("Waffle" , isWaffle (), SLOT(setWaffle (bool)));
  addCheckAction("Summary", isSummary(), SLOT(setSummary(bool)));
  addCheckAction("Count"  , isCount  (), SLOT(setCount  (bool)));

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  return true;
}

//---

double
CQChartsPiePlot::
calcMinValue() const
{
  return values_.min();
}

double
CQChartsPiePlot::
calcMaxValue() const
{
  return values_.max();
}

//---

void
CQChartsPiePlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPlot::write(os, plotVarName, modelVarName, viewVarName);

  textBox_->write(os, plotVarName);
}

//---

CQChartsPieTextObj *
CQChartsPiePlot::
createTextObj() const
{
  return new CQChartsPieTextObj(this);
}

CQChartsPieGroupObj *
CQChartsPiePlot::
createGroupObj(const BBox &bbox, const ColorInd &groupInd,
               const QString &name, const ColorInd &ig) const
{
  return new CQChartsPieGroupObj(this, bbox, groupInd, name, ig);
}

CQChartsPieObj *
CQChartsPiePlot::
createPieObj(const BBox &rect, const QModelIndex &ind, const ColorInd &ig) const
{
  return new CQChartsPieObj(this, rect, ind, ig);
}

//---

bool
CQChartsPiePlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsPiePlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

void
CQChartsPiePlot::
getGroupObjs(int ig, PlotObjs &objs) const
{
  for (const auto &plotObj : plotObjs_) {
    auto *pointObj = dynamic_cast<PieObj *>(plotObj);
    if (! pointObj) continue;

    if (pointObj->is().n == 1 && pointObj->ig().n > 1 && pointObj->ig().i == ig)
      objs.push_back(pointObj);
  }
}

//---

CQChartsPlotCustomControls *
CQChartsPiePlot::
createCustomControls()
{
  auto *controls = new CQChartsPiePlotCustomControls(charts());

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsPieObj::
CQChartsPieObj(const PiePlot *plot, const BBox &rect, const QModelIndex &ind,
               const ColorInd &ig) :
 CQChartsPlotObj(const_cast<PiePlot *>(plot), rect, ColorInd(), ig),
 plot_(plot)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
    setModelInd(ind);
}

QString
CQChartsPieObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig().i).arg(iv().i);
}

QString
CQChartsPieObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  plot()->addNoTipColumns(tableTip);

  //---

  auto addColumnRowValue = [&](const Column &column, const QString &header,
                               const QString &value="") {
    if (column.isValid() && tableTip.hasColumn(column))
      return;

    auto value1 = value;

    if (! value1.length()) {
      if (column.isValid()) {
        ModelIndex columnInd(plot_, modelInd().row(), column, modelInd().parent());

        bool ok;

        value1 = plot_->modelString(columnInd, ok);
        if (! ok) return;
      }
    }

    if (! value1.length())
      return;

    auto headerStr = header;

    if (column.isValid()) {
      headerStr = plot_->columnHeaderName(column, /*tip*/true);

      if (headerStr == "")
        headerStr = header;
    }

    tableTip.addTableRow(headerStr, value1);

    if (column.isValid())
      tableTip.addColumn(column);
  };

  //---

  // get tip values
  QString groupName, label, valueStr;

  calcTipData(groupName, label, valueStr);

  // add tip values
  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  tableTip.addTableRow("Name" , label);
  tableTip.addTableRow("Value", valueStr);

  if (optRadius()) {
    tableTip.addTableRow("Radius", *optRadius());
  }

  //---

  // add color column
  addColumnRowValue(plot_->colorColumn(), "Color");

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

void
CQChartsPieObj::
calcTipData(QString &groupName, QString &label, QString &valueStr) const
{
  // get group name
  bool hasGroup = (plot_->numGroups() > 1 && groupObj());

  if (hasGroup)
    groupName = groupObj()->name();

  // get label
  auto ind = plot_->unnormalizeIndex(modelInd());

  label = plot_->calcIndLabel(ind);

  // get value string
  valueStr = this->valueStr();
}

QString
CQChartsPieObj::
valueStr() const
{
  int valueColumn = modelInd().column();

  return plot_->columnStr(Column(valueColumn), value_);
}

CQChartsArcData
CQChartsPieObj::
arcData() const
{
  double ri, ro, rv;

  getRadii(ri, ro, rv);

  CQChartsArcData arcData;

  arcData.setCenter     (calcCenter());
  arcData.setInnerRadius(ri);
  arcData.setOuterRadius(rv);

  arcData.setAngle1(angle1());
  arcData.setAngle2(angle2());

  return arcData;
}

void
CQChartsPieObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  PlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
//model->addProperty(path1, this, "selected")->setDesc("Is selected");

//model->addProperty(path1, this, "colorIndex" )->setDesc("Color index");
  model->addProperty(path1, this, "angle1"     )->setDesc("Start angle");
  model->addProperty(path1, this, "angle2"     )->setDesc("End angle");
  model->addProperty(path1, this, "innerRadius")->setDesc("Inner radius");
  model->addProperty(path1, this, "outerRadius")->setDesc("Outer radius");
  model->addProperty(path1, this, "label"      )->setDesc("Label");
  model->addProperty(path1, this, "value"      )->setDesc("Value");
  model->addProperty(path1, this, "missing"    )->setDesc("Value missing");
//model->addProperty(path1, this, "radius"     )->setDesc("Radius");
  model->addProperty(path1, this, "keyLabel"   )->setDesc("Key label");
  model->addProperty(path1, this, "color"      )->setDesc("Color");
  model->addProperty(path1, this, "exploded"   )->setDesc("Is exploded");
}

bool
CQChartsPieObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  if      (plot_->calcTreeMap()) {
    auto bbox = calcTreeMapBBox();

    return bbox.inside(p);
  }
  else if (plot_->calcWaffle()) {
    // TODO
    return false;
  }
  else if (plot_->calcPie()) {
    return arcData().inside(p);
  }
  else {
    return false;
  }
}

void
CQChartsPieObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->labelColumn());

  for (const auto &c : plot_->valueColumns())
    addColumnSelectIndex(inds, c);
}

bool
CQChartsPieObj::
calcExploded() const
{
  bool isExploded = this->isExploded();

  if (isSelected() && plot_->isExplodeSelected())
    isExploded = true;

  return isExploded;
}

CQChartsGeom::BBox
CQChartsPieObj::
extraFitBBox() const
{
  BBox bbox;

  if (! plot_->textBox()->isTextVisible())
    return bbox;

  if (! label().length())
    return bbox;

  //---

  // get pie center (adjusted if exploded)
  auto c = getAdjustedCenter();

  //---

  // if full circle always draw text at center
  if (CQChartsAngle::isCircle(angle1(), angle2())) {
    auto pc = plot_->windowToPixel(c);

    bbox = plot_->textBox()->bbox(pc, label(), 0.0);
  }
  // draw on arc center line
  else {
    // calc label radius
    double ri, ro, rv;

    getRadii(ri, ro, rv);

    double lr = plot_->labelRadius();

    double lr1;

    if (! CMathUtil::isZero(ri))
      lr1 = ri + lr*(ro - ri);
    else
      lr1 = lr*ro;

    lr1 = std::max(lr1, 0.01);

    //---

    // text angle (mid angle)
    auto ta = CQChartsAngle::avg(angle1(), angle2());

    //---

    if (plot_->numGroups() == 1 && lr > 1.0) {
      plot_->textBox()->calcConnectedRadialTextBBox(c, rv, lr1, ta.value(), label(),
                                                    plot_->isRotatedText(), bbox);
    }
    else {
      // calc text position
      auto pt = CQChartsAngle::circlePoint(c, lr1, ta);

      // calc text angle
      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (ta.cos() >= 0.0 ? ta.value() : 180.0 + ta.value());

      // calc text box
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      bbox = plot_->textBox()->bbox(plot_->windowToPixel(pt), label(), angle, align);
    }
  }

  return bbox;
}

void
CQChartsPieObj::
draw(PaintDevice *device) const
{
  if (! isVisible())
    return;

  if (plot()->numGroups() > 1) {
    if (plot()->isSetHidden(groupObj_->groupInd().i))
      return;
  }
  else {
    if (plot()->isSetHidden(colorIndex().i))
      return;
  }

  //---

  if      (plot_->calcTreeMap())
    drawTreeMap(device);
  else if (plot_->calcWaffle())
    drawWaffle(device);
  else if (plot_->calcPie())
    drawSegment(device);
}

void
CQChartsPieObj::
drawSegment(PaintDevice *device) const
{
  auto drawPieSlice = [&](const Point &c, double ri, double ro, const Angle &a1, const Angle &a2) {
    bool isInvertX = plot()->isInvertX();
    bool isInvertY = plot()->isInvertY();

    CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, a1, a2, isInvertX, isInvertY);
  };

  //---

  // get pie center (adjusted if exploded), radii and angles
  auto c = getAdjustedCenter();

//Angle ga = plot_->gapAngle().value()/2.0;
  Angle ga { 0.0 };

  Angle aa1 = angle1() + ga;
  Angle aa2 = angle2() - ga;

  //---

  // draw grid lines (as pie)
  if (plot_->isGridLines()) {
    double ri, ro, rv;

    getRadii(ri, ro, rv, /*scaled*/false);

    //---

    PenBrush penBrush;

    auto gridColor = plot_->interpGridLinesColor(ColorInd());

    plot_->setPenBrush(penBrush,
      PenData  (true, gridColor, plot_->gridLinesAlpha(),
                plot_->gridLinesWidth(), plot_->gridLinesDash()),
      BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    Point c = calcCenter();

    drawPieSlice(c, ri, ro, angle1(), angle2());
  }

  //---

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  //---

  // calc stroke and brush
  PenBrush penBrush;

  calcPenBrush(penBrush, /*updateState*/device->isInteractive(), /*inside*/false);

  //---

  // draw pie slice
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  drawPieSlice(c, ri, ro, aa1, aa2);

  device->resetColorNames();

  //---

  bool separated = plot_->calcSeparated();

  // draw mouse over as arc next to pie slice
  if (isInside() && plot_->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    PenBrush penBrush;

    calcPenBrush(penBrush, /*updateState*/false, /*inside*/true);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    double r1;

    if (! separated && plot_->numGroups() > 1)
      r1 = ro;
    else
      r1 = rv + plot_->insideOffset();

    double r2 = r1 + plot_->insideRadius();

    drawPieSlice(c, r1, r2, aa1, aa2);

    //---

    if (! plot_->textBox()->isTextVisible()) {
      auto labelStr = calcTipId();

      //---

      // set text pen
      PenBrush penBrush;

      auto fg = plot_->interpPlotStrokeColor(ColorInd());

      plot_->setPen(penBrush, PenData(true, fg, Alpha()));

      //---

      device->setPen(penBrush.pen);

      plot_->view()->setPainterFont(device, plot_->textBox()->textFont());

      //---

      auto textOptions = plot_->textBox()->textOptions();

      textOptions = plot_->adjustTextOptions(textOptions);

      Point pt(0.0, 0.0);

      CQChartsDrawUtil::drawTextAtPoint(device, pt, labelStr, textOptions, /*centered*/true);
    }
  }
}

void
CQChartsPieObj::
drawTreeMap(PaintDevice *device) const
{
  // calc stroke and brush
  PenBrush penBrush;

  calcPenBrush(penBrush, /*updateState*/device->isInteractive(), /*inside*/false);

  //---

  // draw rect
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  auto bbox = calcTreeMapBBox();

  if (bbox.isValid())
    device->drawRect(bbox);

  device->resetColorNames();
}

CQChartsGeom::BBox
CQChartsPieObj::
calcTreeMapBBox() const
{
  auto c = calcCenter();

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  //---

  auto rect = treeMapBBox();

  if (! rect.isValid())
    return BBox();

  BBox rect1(rect.getXMin()*2*ro + bbox.getXMin(), rect.getYMin()*2*ro + bbox.getYMin(),
             rect.getXMax()*2*ro + bbox.getXMin(), rect.getYMax()*2*ro + bbox.getYMin());

  return rect1;
}

void
CQChartsPieObj::
drawWaffle(PaintDevice *device) const
{
  assert(groupObj_);

  auto bbox = groupObj_->getBBox();

  //---

  // calc stroke and brush
  PenBrush penBrush;

  calcPenBrush(penBrush, /*updateState*/device->isInteractive(), /*inside*/false);

  //---

  // draw rect
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  BBox wbbox;

  double dx = bbox.getWidth ()/10.0;
  double dy = bbox.getHeight()/10.0;

  for (int i = 0; i < waffleCount(); ++i) {
    int i1 = i + waffleStart();

    int ix = i1 % 10;
    int iy = i1 / 10;

    double x1 = bbox.getXMin() + ix*dx;
    double y1 = bbox.getYMin() + iy*dy;
    double x2 = x1 + dx;
    double y2 = y1 + dy;

    auto bbox1 = BBox(x1, y1, x2, y2);

    wbbox += Point(x1, y1);
    wbbox += Point(x2, y2);

    device->drawRect(bbox1);
  }

  waffleBBox_ = wbbox;

  //---

  device->resetColorNames();
}

void
CQChartsPieObj::
drawFg(PaintDevice *device) const
{
  if (! isVisible())
    return;

  if (plot()->numGroups() > 1) {
    if (plot()->isSetHidden(groupObj_->groupInd().i))
      return;
  }
  else {
    if (plot()->isSetHidden(colorIndex().i))
      return;
  }

  //---

  if (! plot_->textBox()->isTextVisible())
    return;

  if (! label().length())
    return;

  //---

  // draw label
  if      (plot_->calcTreeMap())
    drawTreeMapLabel(device);
  else if (plot_->calcWaffle())
    drawWaffleLabel(device);
  else if (plot_->calcPie())
    drawSegmentLabel(device);
}

void
CQChartsPieObj::
drawSegmentLabel(PaintDevice *device) const
{
  // get pie center (adjusted if exploded)
  auto c = getAdjustedCenter();

  // calc label radius
  double ri, ro, rv;

  getRadii(ri, ro, rv);

  //---

  // get display values
  QStringList labels;

  getDrawLabels(labels);
  if (! labels.length()) return;

  //---

  double lr = plot_->labelRadius();

  double lr1 = lr*ro;

  lr1 = std::max(lr1, 0.01);

  //---

  // calc label pen
  // TODO: label alpha
  PenBrush penBrush;

  auto bg = fillColor();

  plot_->setPen(penBrush, PenData(true, bg, Alpha()));

  //---

  auto drawLabels = [&](const Point &p, double angle=0.0,
                        Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) {
    if      (labels.length() == 1)
      plot_->textBox()->draw(device, p, labels.at(0), angle, align);
    else if (labels.length() == 2)
      plot_->textBox()->draw(device, p, labels.at(0), labels.at(1), angle, align);
  };

  // if full circle draw text at center (non-donut) or top (donut)
  if (CQChartsAngle::isCircle(angle1(), angle2())) {
    if (plot_->calcDonut()) {
      auto pt = CQChartsGeom::circlePoint(c, lr1, M_PI/2.0);

      drawLabels(pt);
    }
    else
      drawLabels(c);
  }
  // draw on arc center line
  else {
    // calc text angle
    auto ta = CQChartsAngle::avg(angle1(), angle2());

    if (plot_->numGroups() == 1 && lr > 1.0) {
      auto label1 = labels.at(0);

      plot_->textBox()->drawConnectedRadialText(device, c, rv, lr1, ta.value(), label1,
                                                penBrush.pen, plot_->isRotatedText());
    }
    else {
      // calc text position
      auto pt = CQChartsAngle::circlePoint(c, lr1, ta);

      // calc text angle
      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (ta.cos() >= 0.0 ? ta.value() : 180.0 + ta.value());

      // draw label
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      drawLabels(pt, angle, align);
    }
  }
}

void
CQChartsPieObj::
drawTreeMapLabel(PaintDevice *device) const
{
  // get display values
  QStringList labels;

  getDrawLabels(labels);
  if (! labels.length()) return;

  //---

  // calc label pen
  PenBrush penBrush;

  auto tc = plot_->textBox()->interpTextColor(calcColorInd());

  plot_->setPen(penBrush, PenData(true, tc, plot_->textBox()->textAlpha()));

  plot_->view()->setPainterFont(device, plot_->textBox()->textFont());

  //---

  auto textOptions = plot_->textBox()->textOptions();

  textOptions = plot_->adjustTextOptions(textOptions);

  auto bbox = calcTreeMapBBox();

  CQChartsDrawUtil::drawStringsInBox(device, bbox, labels, textOptions);
  //plot_->textBox()->draw(device, bbox.getCenter(), label(), 0.0);
}

void
CQChartsPieObj::
drawWaffleLabel(PaintDevice *device) const
{
  // get display values
  QStringList labels;

  getDrawLabels(labels);
  if (! labels.length()) return;

  //---

  // calc label pen
  PenBrush penBrush;

  auto tc = plot_->textBox()->interpTextColor(calcColorInd());

  plot_->setPen(penBrush, PenData(true, tc, plot_->textBox()->textAlpha()));

  plot_->view()->setPainterFont(device, plot_->textBox()->textFont());

  //---

  auto textOptions = plot_->textBox()->textOptions();

  textOptions = plot_->adjustTextOptions(textOptions);

  CQChartsDrawUtil::drawStringsInBox(device, waffleBBox_, labels, textOptions);
  //plot_->textBox()->draw(device, bbox.getCenter(), label(), 0.0);
}

void
CQChartsPieObj::
getDrawLabels(QStringList &labels) const
{
  // get tip values
  QString groupName, label, valueStr;

  calcTipData(groupName, label, valueStr);

  if (label.trimmed().length())
    labels.push_back(label);

  if (plot_->isCount() && valueStr.length())
    labels.push_back(valueStr);
}

void
CQChartsPieObj::
getRadii(double &ri, double &ro, double &rv, bool scaled) const
{
  ri = innerRadius();
  ro = outerRadius();
  rv = valueRadius();

  if (groupObj_) {
    bool separated = plot_->calcSeparated();

    auto mapR = [&](double r) {
      double s = (scaled ? radiusScale() : 1.0);

      // if not separated and donut then object has absolute radii
      if (! separated && plot_->calcDonut()) {
        return CMathUtil::map(r, innerRadius(), outerRadius(), innerRadius(), s*outerRadius());
      }

      // otherwise radii are relative to group inner/outer radii
      auto ro = groupObj_->innerRadius() + s*(groupObj_->outerRadius() - groupObj_->innerRadius());

      return CMathUtil::map(r, 0.0, 1.0, groupObj_->innerRadius(), ro);
    };

    ri = mapR(ri);
    ro = mapR(ro);
    rv = mapR(rv);
  }
}

CQChartsGeom::BBox
CQChartsPieObj::
getBBox() const
{
  auto c = calcCenter();

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  return BBox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);
}

void
CQChartsPieObj::
calcPenBrush(PenBrush &penBrush, bool updateState, bool inside) const
{
  // calc stroke and brush
  auto colorInd = this->calcColorInd();

  auto pc = plot_->interpStrokeColor(colorInd);
  auto pa = (inside ? Alpha(0.0) : plot_->strokeAlpha());
  auto fc = fillColor();
  auto fa = (inside ? Alpha(0.7) : plot_->fillAlpha());

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isStroked(), pc, pa, plot_->strokeWidth(), plot_->strokeDash()),
    BrushData(plot_->isFilled(), fc, fa, plot_->fillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsPieObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/false, /*inside*/false);

  PlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.value = " << value() << ";\n";
}

QColor
CQChartsPieObj::
fillColor() const
{
  auto colorInd = this->calcColorInd();

  QColor fc;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    if      (color().isValid())
      fc = plot_->interpColor(color(), ColorInd());
    else if (plot_->fillColor().type() != Color::Type::PALETTE)
      fc = plot_->interpColor(plot_->fillColor(), iv_);
    else if (ig_.n > 1)
      fc = plot_->interpGroupPaletteColor(ig_, iv_);
    else
      fc = plot_->interpFillColor(iv_);
  }
  else {
    fc = plot_->interpFillColor(colorInd);
  }

  return fc;
}

CQChartsGeom::Point
CQChartsPieObj::
getAdjustedCenter() const
{
  auto c = calcCenter();

  //---

  bool isExploded = calcExploded();

  if (! isExploded)
    return c;

  //---

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  //---

  // get adjusted center (exploded state)
  auto ta = CQChartsAngle::avg(angle1(), angle2());

  double er = std::max(plot_->explodeRadius(), 0.0);

  auto ec = CQChartsAngle::circlePoint(c, er*rv, ta);

  return ec;
}

CQChartsGeom::Point
CQChartsPieObj::
calcCenter() const
{
  return (groupObj_ ? groupObj_->calcCenter() : Point(0.0, 0.0));
}

double
CQChartsPieObj::
xColorValue(bool relative) const
{
  double a1 = angle1().value();
  double a2 = angle2().value();

  double a21 = std::abs(a2 - a1);

  if (relative)
    a21 /= 360.0;

  return a21;
}

double
CQChartsPieObj::
yColorValue(bool relative) const
{
  return xColorValue(relative);
}

//------

CQChartsPieGroupObj::
CQChartsPieGroupObj(const PiePlot *plot, const BBox &bbox, const ColorInd &groupInd,
                    const QString &name, const ColorInd &ig) :
 CQChartsGroupObj(const_cast<PiePlot *>(plot), bbox, ig),
 plot_(plot), groupInd_(groupInd), name_(name)
{
}

QString
CQChartsPieGroupObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(ig_.i);
}

QString
CQChartsPieGroupObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name" , name());
  tableTip.addTableRow("Count", numValues());

  //---

  //plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

void
CQChartsPieGroupObj::
addObject(CQChartsPieObj *obj)
{
  obj->setGroupObj(this);

  objs_.push_back(obj);
}

CQChartsPieObj *
CQChartsPieGroupObj::
lookupObj(const QString &name) const
{
  // TODO: use map
  for (const auto &obj : objs_)
    if (obj->label() == name)
      return obj;

  return nullptr;
}

bool
CQChartsPieGroupObj::
inside(const Point &p) const
{
  // separated
  //  . donut or count has group circle with name and/or count in center
  // non-separated
  //  . count has pie segments per group

  bool separated = plot_->calcSeparated();

  bool drawn = ((plot()->calcDonut() && separated) || plot()->isSummary());

  if (! drawn)
    return false;

  //---

  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  double r = p.distanceTo(c);

  if (r < ri || r > ro)
    return false;

  //---

  auto a1 = startAngle();
  auto a2 = endAngle  ();

  if (CQChartsAngle::isCircle(a1, a2))
   return true;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(CQChartsGeom::pointAngle(c, p));
  a = CMathUtil::normalizeAngle(a);

  double ra1 = a1.value(); ra1 = CMathUtil::normalizeAngle(ra1);
  double ra2 = a2.value(); ra2 = CMathUtil::normalizeAngle(ra2);

  if (ra1 < ra2) {
    // crosses zero
    if (a >= 0.0 && a <= ra1)
      return true;

    if (a <= 360.0 && a >= ra2)
      return true;
  }
  else {
    if (a >= ra2 && a <= ra1)
      return true;
  }

  return false;
}

void
CQChartsPieGroupObj::
draw(PaintDevice *device) const
{
  if (plot()->isSetHidden(groupInd().i))
    return;

  bool separated = plot_->calcSeparated();

  //---

  if (numObjs() == 0) {
    if (! separated || plot()->isSummary())
      drawEmptyGroup(device);

    return;
  }

  //---

  if      (plot()->calcTreeMap()) {
    drawTreeMapHeader(device);
  }
  else if (plot()->calcWaffle()) {
    // TODO
  }
  else if (plot()->calcPie()) {
    bool drawn = ((plot()->calcDonut() && separated) || plot()->isSummary());

    if (drawn)
      drawDonut(device);
    else
      drawPieBorder(device);
  }

  //---

  if (separated && plot_->calcDumbbell())
    drawDumbbell(device);
}

void
CQChartsPieGroupObj::
drawDonut(PaintDevice *device) const
{
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  bool separated = plot_->calcSeparated();

  Angle aa1, aa2;

  if (! separated) {
    double ga = plot_->gapAngle().value()/2.0;

    aa1 = Angle(startAngle().value() + ga);
    aa2 = Angle(endAngle  ().value() - ga);
  }
  else {
    aa1 = Angle(0.0);
    aa2 = Angle(360.0);
  }

  //---

  setPenBrush(device);

  //---

  // draw pie slice
  auto drawPieSlice = [&](const Point &c, double ri, double ro, const Angle &a1, const Angle &a2) {
    bool isInvertX = plot()->isInvertX();
    bool isInvertY = plot()->isInvertY();

    CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, a1, a2, isInvertX, isInvertY);
  };

  drawPieSlice(c, ri, ro, aa1, aa2);
}

void
CQChartsPieGroupObj::
drawTreeMapHeader(PaintDevice *device) const
{
  auto bbox = getBBox();

  auto dx = device->pixelToWindowWidth (16);
  auto dy = device->pixelToWindowHeight(16);

  bbox.expand(-dx, -dy, dx, dy);

  //---

  setPenBrush(device);

  device->drawRect(bbox);
}

void
CQChartsPieGroupObj::
drawPieBorder(PaintDevice *device) const
{
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  setPenBrush(device);

  //---

  auto d = std::min(ro/10.0, device->pixelToWindowWidth(16));

  CQChartsDrawUtil::drawPieSlice(device, c, ro, ro + d, Angle(0), Angle(360.0));
}

void
CQChartsPieGroupObj::
drawEmptyGroup(PaintDevice *device) const
{
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  setPenBrush(device);

  //---

  bool separated = plot_->calcSeparated();

  Angle aa1, aa2;

  if (! separated) {
    double ga = plot_->gapAngle().value()/2.0;

    aa1 = Angle(startAngle().value() + ga);
    aa2 = Angle(endAngle  ().value() - ga);
  }
  else {
    aa1 = Angle(0.0);
    aa2 = Angle(360.0);
  }

  //---

  // draw pie slice
  auto drawPieSlice = [&](const Point &c, double ri, double ro, const Angle &a1, const Angle &a2) {
    bool isInvertX = plot()->isInvertX();
    bool isInvertY = plot()->isInvertY();

    CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, a1, a2, isInvertX, isInvertY);
  };

  drawPieSlice(c, ri, ro, aa1, aa2);
}

void
CQChartsPieGroupObj::
drawDumbbell(PaintDevice *device) const
{
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  auto dxt = device->pixelToWindowWidth (1);
  auto dyt = device->pixelToWindowHeight(1);

  //---

  // draw range line
  double x1 = (plot_->drawType() != CQChartsPiePlot::DrawType::NONE ?
                 c.x + ro + 24*dxt : -1.0 + 8*dxt);
  double x2 = 1.0 - 8*dxt;

  PenBrush groupPenBrush;

  calcPenBrush(groupPenBrush, /*updateState*/device->isInteractive());

  groupPenBrush.pen = QPen(Qt::NoPen);

  CQChartsDrawUtil::setPenBrush(device, groupPenBrush);

  device->drawRect(BBox(Point(x1, c.y - dyt*2), Point(x2, c.y + dyt*2)));

  //--

  // draw symbol for each object on line
  auto symbol = CQChartsSymbol::circle();

  CQChartsLength symbolSize("7px");

  double vmin = plot_->calcMinValue();
  double vmax = plot_->calcMaxValue();

  if (plot_->maxValue() > 0.0)
    vmax = plot_->maxValue();

  plot_->view()->setPainterFont(device, plot_->textBox()->textFont());

  CQChartsTextPlacer textPlacer;

  for (const auto &obj : objs_) {
    PenBrush penBrush;

    obj->calcPenBrush(penBrush, /*updateState*/device->isInteractive(), obj->isInside());

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    // draw symbol
    double r = CMathUtil::map(obj->value(), vmin, vmax, x1, x2);

    auto p = Point(r, c.y);

    CQChartsDrawUtil::drawSymbol(device, penBrush, symbol, p, symbolSize);

    //---

    auto tc = plot_->textBox()->interpTextColor(calcColorInd());

    //---

    // draw text label
    auto valueStr = obj->valueStr();

    auto textOptions = plot_->textBox()->textOptions();

    auto p1 = Point(r, c.y + 8*dxt);

    textOptions.align = Qt::AlignHCenter | Qt::AlignBottom;

    if (plot_->isAdjustText()) {
      auto bbox = CQChartsDrawUtil::calcTextAtPointRect(device, p1, valueStr, textOptions);
      bbox.expand(-8*dxt, -8*dyt, 8*dxt, 8*dyt);
    //device->drawRect(bbox);

      auto *text = new CQChartsTextPlacer::DrawText(valueStr, p1, textOptions, tc,
                                                    plot_->textBox()->textAlpha(), p);

      text->setBBox(bbox);

      textPlacer.addDrawText(text);
    }
    else {
      // set text pen
      PenBrush tpenBrush;

      plot_->setPen(tpenBrush, PenData(true, tc, plot_->textBox()->textAlpha()));

      device->setPen(tpenBrush.pen);

      CQChartsDrawUtil::drawTextAtPoint(device, p1, valueStr, textOptions);
    }
  }

  // draw placed text
  if (plot_->isAdjustText()) {
  //textPlacer.setDebug(true);

    auto clipRect = BBox(x1, c.y - ro, x2, c.y + ro);
  //device->drawRect(clipRect);

    textPlacer.place(clipRect);

    textPlacer.draw(device);
  }
}

void
CQChartsPieGroupObj::
setPenBrush(PaintDevice *device) const
{
  PenBrush groupPenBrush;

  calcPenBrush(groupPenBrush, /*updateState*/device->isInteractive());

  CQChartsDrawUtil::setPenBrush(device, groupPenBrush);
}

void
CQChartsPieGroupObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto pc = plot_->interpGroupStrokeColor(ig_);
  auto bc = plot_->interpGroupFillColor  (ig_);

  plot_->setPenBrush(penBrush,
    PenData  (true, pc, plot_->groupShapeData().stroke()),
    BrushData(true, bc, plot_->groupShapeData().fill  ()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsPieGroupObj::
drawFg(PaintDevice *device) const
{
  if (plot()->isSetHidden(groupInd().i))
    return;

  //---

  // draw text at center if donut or summary
  bool drawText = (plot()->calcDonut() || plot()->isSummary());

  if (drawText)
    drawDonutText(device);
}

void
CQChartsPieGroupObj::
drawDonutText(PaintDevice *device) const
{
  // get text
  QStringList labels;

  if (name().trimmed().length())
    labels.push_back(name());

  if (plot_->isCount()) {
    auto numValuesStr = QString::number(numValues());

    labels.push_back(numValuesStr);
  }

  if (! labels.length())
    return;

  //---

  // get text center and radii
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  //---

  bool separated = plot_->calcSeparated();

  Point pt;

  // if full circle always draw text at center
  if (separated || CQChartsAngle::isCircle(startAngle(), endAngle())) {
    pt = c;
  }
  else {
    auto ta = CQChartsAngle::avg(startAngle(), endAngle());

    pt = CQChartsAngle::circlePoint(c, CMathUtil::avg(ri, ro), ta);
  }

  //---

  // set text pen
  PenBrush penBrush;

  auto tc = plot_->textBox()->interpTextColor(calcColorInd());

  plot_->setPen(penBrush, PenData(true, tc, plot_->textBox()->textAlpha()));

  plot_->view()->setPainterFont(device, plot_->textBox()->textFont());

  //---

  device->setPen(penBrush.pen);

  auto textOptions = plot_->textBox()->textOptions();

  textOptions = plot_->adjustTextOptions(textOptions);

  CQChartsDrawUtil::drawTextsAtPoint(device, pt, labels, textOptions);
}

CQChartsGeom::Point
CQChartsPieGroupObj::
calcCenter() const
{
  auto c = center();

  if (plot_->calcSeparated() && plot_->calcDumbbell()) {
    double ri, ro;

    getRadii(ri, ro);

    c.setX(-1.0 + ro);
  }

  return c;
}

void
CQChartsPieGroupObj::
getRadii(double &ri, double &ro) const
{
  bool separated = plot_->calcSeparated();

  // empty group
  //  . not separated is whole circle
  //  . separated and count then 0.0 to scaled plot inner radius
  if (numObjs() == 0) {
    if (! separated) {
      if (! plot()->isSummary()) {
        ri = innerRadius();
        ro = outerRadius();
      }
      else {
        ri = 0.0;
        ro = plot()->outerRadius();
      }
    }
    else {
      ri = 0.0;
      ro = plot()->outerRadius()*outerRadius();
    }
  }
  else {
    // if donut
    //   . not separated then group is from 0.0 to plot inner radius (already set)
    //   . separeted then group is from 0.0 to scaled plot inner radius
    if      (plot()->calcDonut()) {
      if (! separated) {
        if (! plot()->isSummary()) {
          ri = innerRadius();
          ro = outerRadius();
        }
        else {
          ri = 0.0;
          ro = plot()->innerRadius();
        }
      }
      else {
        ri = 0.0;
        ro = plot()->innerRadius()*outerRadius();
      }
    }
    else if (plot()->calcTreeMap() && separated) {
      ri = 0.0;
      ro = outerRadius()*plot_->outerRadius();
    }
    // not donut then from center to plot outer radius (already set ?)
    else {
      ri = 0.0;

      if (separated)
        ro = outerRadius()*plot_->outerRadius();
      else
        ro = plot()->outerRadius();
    //ri = std::min(std::max(plot()->calcDonut() ? plot()->innerRadius()*ro : 0.0, 0.0), 1.0);
    }
  }
}

CQChartsGeom::BBox
CQChartsPieGroupObj::
getBBox() const
{
  auto c = calcCenter();

  double ri, ro;

  getRadii(ri, ro);

  return BBox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);
}

QColor
CQChartsPieGroupObj::
bgColor() const
{
  return plot_->interpGroupFillColor(ig_);
}

//------

CQChartsPieKeyColor::
CQChartsPieKeyColor(PiePlot *plot, PlotObj *obj) :
 CQChartsKeyColorBox(plot, ColorInd(), ColorInd(), ColorInd()), obj_(obj)
{
  setClickable(true);
}

#if 0
bool
CQChartsPieKeyColor::
selectPress(const Point &, CQChartsSelMod)
{
  auto *plot = qobject_cast<PiePlot *>(plot_);

  auto is = setIndex();

  plot->setSetHidden(is.i, ! plot->isSetHidden(is.i));

  plot->updateObjs();

  return true;
}
#endif

void
CQChartsPieKeyColor::
doSelect(SelMod)
{
  auto *plot = qobject_cast<PiePlot *>(plot_);

  auto is = setIndex();

  CQChartsPlot::PlotObjs objs;

  plot->getGroupObjs(is.i, objs);
  if (objs.empty()) return;

  //---

  plot->selectObjs(objs, /*export*/true);

  key_->redraw(/*wait*/ true);
}

QBrush
CQChartsPieKeyColor::
fillBrush() const
{
  auto *plot = qobject_cast<PiePlot *>(plot_);

  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  QColor c;

  if      (group)
    c = group->bgColor();
  else if (obj)
    c = obj->fillColor();

  auto is = setIndex();

  if (plot->isSetHidden(is.i))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

CQChartsUtil::ColorInd
CQChartsPieKeyColor::
setIndex() const
{
  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  if      (group)
    return group->groupInd();
  else if (obj)
    return obj->colorIndex();

  return ColorInd();
}

//------

CQChartsPieKeyText::
CQChartsPieKeyText(PiePlot *plot, PlotObj *plotObj) :
 CQChartsKeyText(plot, "", ColorInd()), obj_(plotObj)
{
  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  if      (group)
    setText(group->name());
  else if (obj)
    setText(obj->keyLabel());
}

QColor
CQChartsPieKeyText::
interpTextColor(const ColorInd &ind) const
{
  auto *plot = qobject_cast<PiePlot *>(plot_);

  auto c = CQChartsKeyText::interpTextColor(ind);

  auto is = setIndex();

  if (plot && plot->isSetHidden(is.i))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

CQChartsUtil::ColorInd
CQChartsPieKeyText::
setIndex() const
{
  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  if      (group)
    return group->groupInd();
  else if (obj)
    return obj->colorIndex();

  return ColorInd();
}

//------

CQChartsPieTextObj::
CQChartsPieTextObj(const PiePlot *plot) :
 CQChartsRotatedTextBoxObj(const_cast<PiePlot *>(plot)), plot_(plot)
{
}

CQChartsTextOptions
CQChartsPieTextObj::
textOptions() const
{
  CQChartsTextOptions options;

//options.angle         = Angle();
  options.contrast      = isTextContrast();
  options.contrastAlpha = textContrastAlpha();
//options.align         = textAlign();
//options.formatted     = isTextFormatted();
//options.scaled        = isTextScaled();
//options.html          = isTextHtml();
  options.clipLength    = plot_->lengthPixelWidth(textClipLength());
  options.clipElide     = textClipElide();

  return options;
}

//------

CQChartsPiePlotCustomControls::
CQChartsPiePlotCustomControls(CQCharts *charts) :
 CQChartsGroupPlotCustomControls(charts, "pie")
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addColumnWidgets(QStringList() << "values" << "label" << "radius" << "keyLabel", columnsFrame);

  //---

  addGroupColumnWidgets();
  addColorColumnWidgets();

  //---

  // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame", /*stretch*/false);

  drawTypeCombo_ = CQUtil::makeWidget<CQEnumCombo>("drawTypeCombo");

  drawTypeCombo_->setPropName("drawType");

  separatedCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Separated", "separatedCheck");

  addFrameColWidget(optionsFrame, drawTypeCombo_ );
  addFrameColWidget(optionsFrame, separatedCheck_);

  donutCheck_    = CQUtil::makeLabelWidget<QCheckBox>("Donut"   , "donutCheck"   );
  summaryCheck_  = CQUtil::makeLabelWidget<QCheckBox>("Summary" , "summaryCheck" );
  dumbbellCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Dumbbell", "dumbbellCheck");
  countCheck_    = CQUtil::makeLabelWidget<QCheckBox>("Count"   , "countCheck"   );

  addFrameColWidget(optionsFrame, donutCheck_   );
  addFrameColWidget(optionsFrame, summaryCheck_ );
  addFrameColWidget(optionsFrame, dumbbellCheck_);
  addFrameColWidget(optionsFrame, countCheck_   );

  addFrameColWidget(optionsFrame, CQChartsWidgetUtil::createHStretch());

  //---

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsPiePlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    drawTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(drawTypeSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    separatedCheck_, SIGNAL(stateChanged(int)), this, SLOT(separatedSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    donutCheck_, SIGNAL(stateChanged(int)), this, SLOT(donutSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    summaryCheck_, SIGNAL(stateChanged(int)), this, SLOT(summarySlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    dumbbellCheck_, SIGNAL(stateChanged(int)), this, SLOT(dumbbellSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    countCheck_, SIGNAL(stateChanged(int)), this, SLOT(countSlot()));

  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsPiePlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  plot_ = dynamic_cast<PiePlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);

  if (plot_)
    connect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsPiePlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  drawTypeCombo_->setObj(plot_);

  separatedCheck_->setChecked(plot_->isSeparated());

  donutCheck_   ->setChecked(plot_->isDonut   ());
  summaryCheck_ ->setChecked(plot_->isSummary ());
  dumbbellCheck_->setChecked(plot_->isDumbbell());
  countCheck_   ->setChecked(plot_->isCount   ());

  CQChartsGroupPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsPiePlotCustomControls::
drawTypeSlot()
{
  updateWidgets();
}

void
CQChartsPiePlotCustomControls::
separatedSlot()
{
  plot_->setSeparated(separatedCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
donutSlot()
{
  plot_->setDonut(donutCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
summarySlot()
{
  plot_->setSummary(summaryCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
dumbbellSlot()
{
  plot_->setDumbbell(dumbbellCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
countSlot()
{
  plot_->setCount(countCheck_->isChecked());
}
