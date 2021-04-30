#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsHtml.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
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

  addBoolParameter("donut", "Donut", "donut").setTip("Draw donut");
  addBoolParameter("count", "Count", "count").setTip("Display value counts");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsPiePlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Pie Plot").
    h3("Summary").
     p("Draw circle segments with diameter from a set of values.").
     p("The segments can be restricted to an inner radius and a label "
       "can be displated at the center of the circle.").
    h3("Limitations").
     p("None.").
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
 CQChartsObjShapeData   <CQChartsPiePlot>(this),
 CQChartsObjGridLineData<CQChartsPiePlot>(this)
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
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setValueColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setRadiusColumn(const Column &c)
{
  CQChartsUtil::testAndSet(radiusColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setKeyLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(keyLabelColumn_, c, [&]() { updateRangeAndObjs(); } );
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
  else             CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsPiePlot::
setDonut(bool b)
{
  CQChartsUtil::testAndSet(donut_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setCount(bool b)
{
  CQChartsUtil::testAndSet(count_, b, [&]() { updateRangeAndObjs(); } );
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

void
CQChartsPiePlot::
setSeparated(bool b)
{
  CQChartsUtil::testAndSet(separated_, b, [&]() { updateRangeAndObjs(); } );
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
  addProp("options", "donut"      , "", "Display as donut using inner radius");
  addProp("options", "count"      , "", "Show count of groups");
  addProp("options", "innerRadius", "", "Inner radius for donut")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("options", "outerRadius", "", "Outer radius for donut")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("options", "startAngle" , "", "Start angle for first segment");
  addProp("options", "angleExtent", "", "Angle extent for pie segments");
  addProp("options", "gapAngle"   , "", "Gap angle");
  addProp("options", "separated"  , "", "Draw grouped pie charts separately");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

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

  if (! checkColumns(valueColumns(), "Value", /*required*/true))
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

  double angle1 = startAngle().value();
  double alen   = CQChartsUtil::clampDegrees(angleExtent().value());
  double angle2 = angle1 + alen;

  // add segment outside points
  dataRange.updateRange(CQChartsGeom::circlePoint(c, r, CMathUtil::Deg2Rad(angle1)));
  dataRange.updateRange(CQChartsGeom::circlePoint(c, r, CMathUtil::Deg2Rad(angle2)));

  // add intermediate points (every 90 degree point between outside points)
  double a1 = 90.0*CMathRound::RoundDownF(angle1/90.0);

  if (angle1 < angle2) {
    for (double a = a1; a < angle2; a += 90.0) {
      if (a > angle1 && a < angle2)
        dataRange.updateRange(CQChartsGeom::circlePoint(c, r, CMathUtil::Deg2Rad(a)));
    }
  }
  else {
    for (double a = a1; a > angle2; a -= 90.0) {
      if (a > angle2 && a < angle1)
        dataRange.updateRange(CQChartsGeom::circlePoint(c, r, CMathUtil::Deg2Rad(a)));
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

  double ro = outerRadius();
  double ri = std::min(std::max(isDonut() ? innerRadius()*ro : 0.0, 0.0), 1.0);

  int ig = 0;
  int ng = numGroups();

  double dr;

  if (isSeparated())
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

    if (! isCount()) {
      groupObj->setInnerRadius(r - dr);
      groupObj->setOuterRadius(r);
    }
    else {
      groupObj->setInnerRadius(ri);
      groupObj->setOuterRadius(ro);
    }

  //groupObj->setVisible(isCount());

    objs.push_back(groupObj);

    th->groupObjs_.push_back(groupObj);

    //---

    if (groupData.groupObj != groupObj) {
      auto &groupData1 = const_cast<GroupData &>(groupData);

      groupData1.groupObj = groupObj;
    }

    //---

    if (! isSeparated())
      r -= dr;

    ++ig;
  }

  //---

  // add individual values (not needed for count)

  if (! isCount() || isDonut()) {
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

  if (isSeparated()) {
    int ng = numGroupObjs();

    int ny = CMathRound::RoundNearest(std::sqrt(std::max(ng, 1)));
    int nx = (ng + ny - 1)/std::max(ny, 1);

    double dx = 1.0/nx;
    double dy = 1.0/ny;
    double dr = std::min(dx, dy);

    int ig = 0;

    for (auto &groupObj : groupObjs_) {
      int ix = ig % nx;
      int iy = ig / nx;

      double x = CMathUtil::map(ix, 0, nx - 1, -1.0 + dx, 1.0 - dx);
      double y = CMathUtil::map(iy, 0, ny - 1, -1.0 + dy, 1.0 - dy);

      groupObj->setCenter(Point(x, y));
      groupObj->setInnerRadius(0.0);
      groupObj->setOuterRadius(dr);

#if 0
      for (auto &pieObj : groupObj->objs()) {
        if (isDonut())
          pieObj->setInnerRadius(dr*innerRadius());
        else
          pieObj->setInnerRadius(0.0);

        pieObj->setOuterRadius(dr);

        double rv = dr*outerRadius();

        if (pieObj->optRadius() && groupObj->isRadiusScaled()) {
          double s = (groupObj->radiusMax() > 0.0 ?
                        *pieObj->optRadius()/groupObj->radiusMax() : 1.0);

          rv = CMathUtil::lerp(s, pieObj->innerRadius(), pieObj->outerRadius());
        }

        pieObj->setValueRadius(rv);
      //pieObj->setValueRadius(dr*outerRadius());
      }
#endif

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
  assert(! isCount() || isDonut());

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
  ModelIndex lind(th, ind.row(), labelColumn(), ind.parent());

  bool ok;

  QString label;

  if (numGroups() > 1) {
    if (valueColumns().count() > 1 && ! isGroupHeaders())
      label = modelHHeaderString(ind.column(), ok);
    else
      label = modelString(lind, ok);
  }
  else {
    label = modelString(lind, ok);
  }

  if (! label.length())
    label = QString("%1").arg(ind.row());

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
  double ri = std::min(std::max(groupObj ? groupObj->innerRadius() : innerRadius(), 0.0), 1.0);
  double ro = (groupObj ? groupObj->outerRadius() : outerRadius());
  double rv = ro;

  if (hasRadius && groupObj && groupObj->isRadiusScaled()) {
    double s = (groupObj->radiusMax() > 0.0 ? radius/groupObj->radiusMax() : 1.0);

    rv = CMathUtil::lerp(s, ri, ro);
  }

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

    if (hasRadius)
      obj->setOptRadius(*obj->optRadius() + radius);

    // TODO: add dataInd
  }
}

void
CQChartsPiePlot::
calcDataTotal() const
{
  auto *th = const_cast<CQChartsPiePlot *>(this);

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

  // size must be positive
  if (value <= 0.0) {
    th->addDataError(ind, "Non-positive value");
    value = 1.0;
    return false;
  }

  return true;
}

void
CQChartsPiePlot::
adjustObjAngles() const
{
  double ro = outerRadius();
  double ri = std::min(std::max(isDonut() ? innerRadius()*ro : 0.0, 0.0), 1.0);

  bool isGrouped = (numGroups() > 1);

  // calc delta radius (grouped)
  int    ng = 1, nh = 0;
  double dr = 0.0;

  if (! isCount() || isDonut()) {
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

  //---

  // calc angle extents for each group
  double ga1 = startAngle().value();
  double ga2 = ga1;

  double r = ro;

  for (auto &groupObj : groupObjs_) {
    ga1 = ga2;

    // skip hidden groups
    if (! isCount() || isDonut()) {
      if (isGrouped && nh > 0) {
        if (isSetHidden(groupObj->groupInd().i))
          continue;
      }
    }
    else {
      if (isSetHidden(groupObj->groupInd().i))
        continue;
    }

    //---

    // set group angles
    double dga = (totalValues > 0 ? 360.0*groupObj->numValues()/totalValues : 0);

    ga2 = ga1 - dga;

    groupObj->setAngles(Angle(ga1), Angle(ga2));

    //---

    if (! isCount() || isDonut()) {
      // set group object inner/outer radii
      if (isGrouped && nh > 0) {
        groupObj->setInnerRadius(r - dr);
        groupObj->setOuterRadius(r);
      }

      //---

      // set segment angles
      double ga = gapAngle().value();

      double angle1    = startAngle().value();
      double alen      = CQChartsUtil::clampDegrees(angleExtent().value());
      double dataTotal = groupObj->dataTotal();

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

      for (auto &obj : groupObj->objs()) {
        // skip hidden objects
        if (! obj->isVisible())
          continue;

        //---

        // set angle based on value
        double value = obj->value();

        double angle  = (dataTotal > 0.0 ? alen*value/dataTotal : 0.0);
        double angle2 = angle1 + angle;

        obj->setAngle1(Angle(angle1));
        obj->setAngle2(Angle(angle2));

        //---

        // set inner/outer radius and value radius
        if (isGrouped && nh > 0) {
          obj->setInnerRadius(r - dr);
          obj->setOuterRadius(r);

          double rv = r;

          if (obj->optRadius() && groupObj->isRadiusScaled()) {
            double s = (groupObj->radiusMax() > 0.0 ?
                          *obj->optRadius()/groupObj->radiusMax() : 1.0);

            rv = CMathUtil::lerp(s, r - dr, r);
          }

          obj->setValueRadius(rv);
        }

        //---

        // move to next start angle
        angle1 = angle2 + ga;
      }

      //---

      // move to next radius
      if (isGrouped && nh > 0)
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

  if (! isCount() || isDonut()) {
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
  auto *donutAction = new QAction("Donut", menu);

  donutAction->setCheckable(true);
  donutAction->setChecked(isDonut());

  connect(donutAction, SIGNAL(triggered(bool)), this, SLOT(setDonut(bool)));

  //---

  auto *countAction = new QAction("Count", menu);

  countAction->setCheckable(true);
  countAction->setChecked(isCount());

  connect(countAction, SIGNAL(triggered(bool)), this, SLOT(setCount(bool)));

  //---

  menu->addSeparator();

  menu->addAction(donutAction);
  menu->addAction(countAction);

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  return true;
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

  auto addColumnRowValue = [&](const CQChartsColumn &column, const QString &header,
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
  auto ind = plot_->unnormalizeIndex(modelInd());

  // get group name and label
  bool hasGroup = (plot_->numGroups() > 1 && groupObj_);

  bool ok;

  ModelIndex lind(plot(), ind.row(), plot_->labelColumn(), ind.parent());

  if (hasGroup) {
    auto *groupObj = this->groupObj();

    groupName = groupObj->name();

    if (plot_->isGroupHeaders())
      label = plot_->modelHHeaderString(CQChartsColumn(ind.column()), ok);
    else
      label = plot_->modelString(lind, ok);
  }
  else {
    label = plot_->modelString(lind, ok);
  }

  //---

  // get value string
  int valueColumn = modelInd().column();

  valueStr = plot_->columnStr(CQChartsColumn(valueColumn), value_);
}

CQChartsArcData
CQChartsPieObj::
arcData() const
{
  double ri, ro, rv;

  getRadii(ri, ro, rv);

  CQChartsArcData arcData;

  arcData.setCenter     (getCenter());
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

  return arcData().inside(p);
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

  // calc angle extent
  double a1 = angle1().value();
  double a2 = angle2().value();

  double a21 = a2 - a1;

  // if full circle always draw text at center
  if (CMathUtil::realEq(std::abs(a21), 360.0)) {
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
    double ta = CMathUtil::avg(a1, a2);

    //---

    if (plot_->numGroups() == 1 && lr > 1.0) {
      plot_->textBox()->calcConnectedRadialTextBBox(c, rv, lr1, ta, label(),
                                                    plot_->isRotatedText(), bbox);
    }
    else {
      // calc text position
      double tangle = CMathUtil::Deg2Rad(ta);

      auto pt = CQChartsGeom::circlePoint(c, lr1, tangle);

      // calc text angle
      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (std::cos(tangle) >= 0.0 ? ta : 180.0 + ta);

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

  auto drawPieSlice = [&](const Point &c, double ri, double ro, const Angle &a1, const Angle &a2) {
    bool isInvertX = plot()->isInvertX();
    bool isInvertY = plot()->isInvertY();

    CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, a1, a2, isInvertX, isInvertY);
  };

  //---

  // get pie center (adjusted if exploded), radii and angles
  auto c = getAdjustedCenter();

  double ri, ro, rv;

  getRadii(ri, ro, rv);

//Angle ga = plot_->gapAngle().value()/2.0;
  Angle ga { 0.0 };

  Angle aa1 = angle1() + ga;
  Angle aa2 = angle2() - ga;

  //---

  // draw grid lines (as pie)
  if (plot_->isGridLines()) {
    PenBrush penBrush;

    auto gridColor = plot_->interpGridLinesColor(ColorInd());

    plot_->setPenBrush(penBrush,
      PenData  (true, gridColor, plot_->gridLinesAlpha(),
                plot_->gridLinesWidth(), plot_->gridLinesDash()),
      BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    Point c = getCenter();

    drawPieSlice(c, ri, ro, angle1(), angle2());
  }

  //---

  // calc stroke and brush
  PenBrush penBrush;

  calcPenBrush(penBrush, /*updateState*/device->isInteractive(), /*inside*/false);

  //---

  // draw pie slice
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  drawPieSlice(c, ri, rv, aa1, aa2);

  device->resetColorNames();

  //---

  if (isInside() && plot_->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    PenBrush penBrush;

    calcPenBrush(penBrush, /*updateState*/false, /*inside*/true);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    double r1 = rv + plot_->insideOffset();
    double r2 = r1 + plot_->insideRadius();

    drawPieSlice(c, r1, r2, aa1, aa2);

    if (! plot_->textBox()->isTextVisible()) {
      auto labelStr = calcTipId();

      //---

      // set text pen
      PenBrush penBrush;

      auto fg = plot_->interpPlotStrokeColor(ColorInd());

      plot_->setPen(penBrush, PenData(true, fg, Alpha()));

      //---

      device->setPen(penBrush.pen);

      auto textOptions = plot_->adjustTextOptions();

      Point pt(0.0, 0.0);

      textOptions.html = true;

      CQChartsDrawUtil::drawTextAtPoint(device, pt, labelStr, textOptions, /*centered*/true);
    }
  }
}

void
CQChartsPieObj::
drawFg(PaintDevice *device) const
{
  if (! isVisible())
    return;

  // get pie center (adjusted if exploded)
  auto c = getAdjustedCenter();

  // draw segment label
  drawSegmentLabel(device, c);
}

void
CQChartsPieObj::
drawSegmentLabel(PaintDevice *device, const Point &c) const
{
  if (! plot_->textBox()->isTextVisible())
    return;

  if (! label().length())
    return;

  //---

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

  // calc text angle
  double a1 = angle1().value();
  double a2 = angle2().value();

  double ta = CMathUtil::avg(a1, a2);

  //---

  // calc label pen
  // TODO: label alpha
  PenBrush lenBrush;

  auto bg = fillColor();

  plot_->setPen(lenBrush, PenData(true, bg, Alpha()));

  //---

  // calc angle extent
  double a21 = a2 - a1;

  // if full circle draw text at center (non-donut) or top (donut)
  if (CMathUtil::realEq(std::abs(a21), 360.0)) {
    if (plot_->isDonut()) {
      auto pt = CQChartsGeom::circlePoint(c, lr1, M_PI/2.0);

      plot_->textBox()->draw(device, pt, label(), 0.0);
    }
    else
      plot_->textBox()->draw(device, c, label(), 0.0);
  }
  // draw on arc center line
  else {
    if (plot_->numGroups() == 1 && lr > 1.0) {
      plot_->textBox()->drawConnectedRadialText(device, c, rv, lr1, ta, label(),
                                                lenBrush.pen, plot_->isRotatedText());
    }
    else {
      // calc text position
      double tangle = CMathUtil::Deg2Rad(ta);

      auto pt = CQChartsGeom::circlePoint(c, lr1, tangle);

      // calc text angle
      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (std::cos(tangle) >= 0.0 ? ta : 180.0 + ta);

      // draw label
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      plot_->textBox()->draw(device, pt, label(), angle, align);
    }
  }
}

void
CQChartsPieObj::
getRadii(double &ri, double &ro, double &rv) const
{
  ri = std::min(std::max(innerRadius(), 0.0), 1.0);
  ro = outerRadius();
  rv = valueRadius();

  if (groupObj_) {
    ri *= groupObj_->outerRadius();
    ro *= groupObj_->outerRadius();
    rv *= groupObj_->outerRadius();
  }
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
  auto c = getCenter();

  //---

  bool isExploded = calcExploded();

  if (! isExploded)
    return c;

  //---

  double ri, ro, rv;

  getRadii(ri, ro, rv);

  //---

  // get adjusted center (exploded state)
  double a1 = angle1().value();
  double a2 = angle2().value();

  double angle = CMathUtil::Deg2Rad(CMathUtil::avg(a1, a2));

  double er = std::max(plot_->explodeRadius(), 0.0);

  auto ec = CQChartsGeom::circlePoint(c, er*rv, angle);

  return ec;
}

CQChartsGeom::Point
CQChartsPieObj::
getCenter() const
{
  return (groupObj_ ? groupObj_->center() : Point(0.0, 0.0));
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
  if (! plot()->isCount())
    return false;

  //---

  Point center(0, 0);

  double r = p.distanceTo(center);

  double ro = plot_->outerRadius();
  double ri = std::min(std::max(plot_->isDonut() ? plot_->innerRadius()*ro : 0.0, 0.0), 1.0);

  if (r < ri || r > ro)
    return false;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(CQChartsGeom::pointAngle(center, p));
  a = CMathUtil::normalizeAngle(a);

  double a1 = startAngle().value(); a1 = CMathUtil::normalizeAngle(a1);
  double a2 = endAngle  ().value(); a2 = CMathUtil::normalizeAngle(a2);

  if (a1 < a2) {
    // crosses zero
    if (a >= 0.0 && a <= a1)
      return true;

    if (a <= 360.0 && a >= a2)
      return true;
  }
  else {
    if (a >= a2 && a <= a1)
      return true;
  }

  return false;
}

void
CQChartsPieGroupObj::
draw(PaintDevice *device) const
{
  if (numObjs() == 0) {
    if (! plot()->isSeparated())
      drawEmptyGroup(device);

    return;
  }

  //---

  if ((plot_->isDonut() && plot()->isSeparated()) || plot()->isCount())
    drawDonut(device);
}

void
CQChartsPieGroupObj::
drawDonut(PaintDevice *device) const
{
  auto c = center();

  double ri = 0.0, ro = 1.0;

  if (plot_->isDonut()) {
    ri = 0.0;
    ro = std::min(std::max(plot_->innerRadius()*outerRadius(), 0.0), 1.0);
  }
  else {
    ri = 0.0;
    ro = plot_->outerRadius();
  //ri = std::min(std::max(plot_->isDonut() ? plot_->innerRadius()*ro : 0.0, 0.0), 1.0);
  }

  //---

  Angle aa1, aa2;

  if (! plot_->isSeparated()) {
    double ga = plot_->gapAngle().value()/2.0;

    aa1 = Angle(startAngle().value() + ga);
    aa2 = Angle(endAngle  ().value() - ga);
  }
  else {
    aa1 = Angle(0.0);
    aa2 = Angle(360.0);
  }

  //---

  // set pen and brush
  // TODO: more customization support

  auto bg = bgColor();
  auto fg = plot_->interpPlotStrokeColor(ColorInd());

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, fg), BrushData(true, bg));

  plot_->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw pie slice
  bool isInvertX = plot()->isInvertX();
  bool isInvertY = plot()->isInvertY();

  CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, aa1, aa2, isInvertX, isInvertY);
}

void
CQChartsPieGroupObj::
drawEmptyGroup(PaintDevice *device) const
{
  auto c = center();

  double ri, ro;

  getRadii(ri, ro);

  //---

  // set pen and brush
  // TODO: more customization support
  auto bg = bgColor();
  auto fg = plot_->interpPlotStrokeColor(ColorInd());

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, fg), BrushData(true, bg));

  plot_->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  CQChartsDrawUtil::drawPieSlice(device, c, ri, ro, Angle(0.0), Angle(360.0), false, false);
}

void
CQChartsPieGroupObj::
drawFg(PaintDevice *device) const
{
  if ((plot_->isDonut() && plot()->isSeparated()) || plot()->isCount())
    drawDonutText(device);
}

void
CQChartsPieGroupObj::
drawDonutText(PaintDevice *device) const
{
  if (numObjs() == 0) {
    if (plot_->isSeparated())
      return;
  }

  // Get donut pie slice angles
  double a1, a2;

  if (! plot_->isSeparated()) {
    a1 = startAngle().value();
    a2 = endAngle  ().value();
  }
  else {
    a1 = 0.0;
    a2 = 360.0;
  }

  double a21 = a2 - a1;

  //---

  // get text center
  auto c = center();

  Point pt;

  // if full circle always draw text at center
  if (plot_->isSeparated() || CMathUtil::realEq(std::abs(a21), 360.0)) {
    pt = c;
  }
  else {
    double ta = CMathUtil::avg(a1, a2);

    double tangle = CMathUtil::Deg2Rad(ta);

    pt = CQChartsGeom::circlePoint(c, 0.5, tangle);
  }

  //---

  // get text
  QStringList labels;

  if (plot_->isSeparated())
    labels.push_back(name());

  if (plot_->isCount()) {
    auto numValuesStr = QString("%1").arg(numValues());

    labels.push_back(numValuesStr);
  }

  //---

  // set text pen
  PenBrush penBrush;

  auto fg = plot_->interpPlotStrokeColor(ColorInd());

  plot_->setPen(penBrush, PenData(true, fg, Alpha()));

  //---

  device->setPen(penBrush.pen);

  auto textOptions = plot_->adjustTextOptions();

  CQChartsDrawUtil::drawTextsAtPoint(device, pt, labels, textOptions);
}

void
CQChartsPieGroupObj::
getRadii(double &ri, double &ro) const
{
  ri = innerRadius();
  ro = outerRadius();
}

QColor
CQChartsPieGroupObj::
bgColor() const
{
  return plot_->interpColor(plot_->fillColor(), ig_);
}

//------

CQChartsPieKeyColor::
CQChartsPieKeyColor(PiePlot *plot, PlotObj *obj) :
 CQChartsKeyColorBox(plot, ColorInd(), ColorInd(), ColorInd()), obj_(obj)
{
}

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

QBrush
CQChartsPieKeyColor::
fillBrush() const
{
  auto *plot = qobject_cast<PiePlot *>(plot_);

  auto *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  auto *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  QColor c;

  if      (group) {
    if (! plot->isCount()) {
      auto ig = group->groupInd();

      c = plot->interpGroupPaletteColor(ig, ColorInd());
    }
    else
      c = group->bgColor();
  }
  else if (obj) {
    c = obj->fillColor();
  }

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

//------

CQChartsPiePlotCustomControls::
CQChartsPiePlotCustomControls(CQCharts *charts) :
 CQChartsGroupPlotCustomControls(charts, "pie")
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addColumnWidgets(QStringList() << "values" << "label" << "radius" << "keyLabel", columnsFrame);

//addFrameRowStretch(optionsFrame);

  //---

  addGroupColumnWidgets();
  addColorColumnWidgets();

  //---

  // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame");

  donutCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Donut", "donutCheck");
  countCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Count", "countCheck");

  addFrameColWidget(optionsFrame, donutCheck_);
  addFrameColWidget(optionsFrame, countCheck_);

  //---

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsPiePlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    donutCheck_, SIGNAL(stateChanged(int)), this, SLOT(donutSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    countCheck_, SIGNAL(stateChanged(int)), this, SLOT(countSlot()));

  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsPiePlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<PiePlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);
}

void
CQChartsPiePlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  donutCheck_->setChecked(plot_->isDonut());
  countCheck_->setChecked(plot_->isCount());

  CQChartsGroupPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsPiePlotCustomControls::
donutSlot()
{
  plot_->setDonut(donutCheck_->isChecked());
}

void
CQChartsPiePlotCustomControls::
countSlot()
{
  plot_->setCount(countCheck_->isChecked());
}
