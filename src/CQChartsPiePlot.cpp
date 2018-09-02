#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CMathRound.h>

#include <QPainter>
#include <QMenu>

CQChartsPiePlotType::
CQChartsPiePlotType()
{
}

void
CQChartsPiePlotType::
addParameters()
{
  startParameterGroup("Pie");

  // name, desc, propName, attributes, default
  addColumnsParameter("value", "Value", "valueColumns", "1").
    setRequired().setTip("Value column(s)");

  addColumnParameter("label"   , "Label"    , "labelColumn"   , 0).setTip("Custom label");
  addColumnParameter("radius"  , "Radius"   , "radiusColumn"     ).setTip("Custom radius");
  addColumnParameter("keyLabel", "Key Label", "keyLabelColumn"   ).setTip("Custom key label");
  addColumnParameter("color"   , "Color"    , "colorColumn"      ).setTip("Custom slice color");

  addBoolParameter("donut", "Donut", "donut").setTip("Draw donut");
  addBoolParameter("count", "Count", "count").setTip("Display value counts");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsPiePlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draw circle segments with diameter from a set of values.</p>\n";
}

CQChartsPlot *
CQChartsPiePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsPiePlot(view, model);
}

//------

CQChartsPiePlot::
CQChartsPiePlot(CQChartsView *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("pie"), model),
 CQChartsPlotGridLineData<CQChartsPiePlot>(this)
{
  (void) addColorSet("color");

  //---

  setValueColumnsStr("1");

  setGridLinesColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.5));

  textBox_ = new CQChartsPieTextObj(this);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  addKey();

  addTitle();
}

CQChartsPiePlot::
~CQChartsPiePlot()
{
  delete textBox_;
}

//---

void
CQChartsPiePlot::
setLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

const CQChartsColumn &
CQChartsPiePlot::
valueColumn() const
{
  return valueColumns_.column();
}

void
CQChartsPiePlot::
setValueColumn(const CQChartsColumn &c)
{
  if (c != valueColumns_.column()) {
    valueColumns_.setColumn(c);

    updateRangeAndObjs();
  }
}

void
CQChartsPiePlot::
setValueColumns(const Columns &cols)
{
  if (cols != valueColumns_.columns()) {
    valueColumns_.setColumns(cols);

    updateRangeAndObjs();
  }
}

QString
CQChartsPiePlot::
valueColumnsStr() const
{
  return valueColumns_.columnsStr();
}

bool
CQChartsPiePlot::
setValueColumnsStr(const QString &s)
{
  bool rc = true;

  if (s != valueColumnsStr()) {
    rc = valueColumns_.setColumnsStr(s);

    updateRangeAndObjs();
  }

  return rc;
}

//---

void
CQChartsPiePlot::
setRadiusColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(radiusColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setKeyLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(keyLabelColumn_, c, [&]() { updateRangeAndObjs(); } );
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
setStartAngle(double r)
{
  CQChartsUtil::testAndSet(startAngle_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPiePlot::
setAngleExtent(double r)
{
  CQChartsUtil::testAndSet(angleExtent_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPiePlot::
setRotatedText(bool b)
{
  CQChartsUtil::testAndSet(rotatedText_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsPiePlot::
setExplodeSelected(bool b)
{
  CQChartsUtil::testAndSet(explodeSelected_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsPiePlot::
setExplodeRadius(double r)
{
  CQChartsUtil::testAndSet(explodeRadius_, r, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsPiePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "valueColumn"   , "value"   );
  addProperty("columns", this, "valueColumns"  , "valueSet");
  addProperty("columns", this, "labelColumn"   , "label"   );
  addProperty("columns", this, "radiusColumn"  , "radius"  );
  addProperty("columns", this, "keyLabelColumn", "keyLabel");
  addProperty("columns", this, "colorColumn"   , "color"   );

  CQChartsGroupPlot::addProperties();

  // general
  addProperty("options", this, "donut"      );
  addProperty("options", this, "count"      );
  addProperty("options", this, "innerRadius");
  addProperty("options", this, "startAngle" );
  addProperty("options", this, "angleExtent");

  // grid
  addProperty("grid", this, "gridLines", "visible");

  addLineProperties("grid", "gridLines");

  // explode
  addProperty("explode", this, "explodeSelected", "selected");
  addProperty("explode", this, "explodeRadius"  , "radius"  );

  // label
  addProperty("label", textBox_, "textVisible", "visible");
  addProperty("label", textBox_, "textFont"   , "font"   );
  addProperty("label", textBox_, "textColor"  , "color"  );
  addProperty("label", textBox_, "textAlpha"  , "alpha"  );
  addProperty("label", this    , "labelRadius", "radius" );
  addProperty("label", this    , "rotatedText", "rotated");

  QString labelBoxPath = id() + "/label/box";

  textBox_->CQChartsBoxObj::addProperties(propertyModel(), labelBoxPath);

  // color map
  addColorMapProperties();
}

//---

void
CQChartsPiePlot::
calcRange()
{
  dataRange_.reset();

  CQChartsGeom::Point c(0.0, 0.0);

  dataRange_.updateRange(c);

  //---

  double r = std::max(1.0, labelRadius());

  double angle1 = startAngle();
  double alen   = CMathUtil::clamp(angleExtent(), -360.0, 360.0);
  double angle2 = angle1 - alen;

  // add segment outside points
  dataRange_.updateRange(CQChartsUtil::AngleToPoint(c, r, angle1));
  dataRange_.updateRange(CQChartsUtil::AngleToPoint(c, r, angle2));

  // add intermediate points (every 90 degree point between outside points)
  double a1 = 90.0*CMathRound::RoundDownF(angle1/90.0);

  if (angle1 < angle2) {
    for (double a = a1; a < angle2; a += 90.0) {
      if (a > angle1 && a < angle2)
        dataRange_.updateRange(CQChartsUtil::AngleToPoint(c, r, a));
    }
  }
  else {
    for (double a = a1; a > angle2; a -= 90.0) {
      if (a > angle2 && a < angle1)
        dataRange_.updateRange(CQChartsUtil::AngleToPoint(c, r, a));
    }
  }

  //---

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange_.equalScale(aspect);
  }

  //---

  // init grouping
  initGroupData(valueColumns(), labelColumn());
}

//------

CQChartsGeom::BBox
CQChartsPiePlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsPieObj *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

    if (pieObj)
      bbox += pieObj->annotationBBox();
  }

  return bbox;
}

//------

void
CQChartsPiePlot::
updateObjs()
{
  clearValueSets();

  CQChartsPlot::updateObjs();
}

bool
CQChartsPiePlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  // calc group totals
  calcDataTotal();

  //---

  groupObjs_.clear();

  //---

  double ro = outerRadius();
  double ri = (isDonut() ? innerRadius()*outerRadius() : 0.0);

  int ig = 0;
  int ng = numGroups();

  double dr = (ng > 0 ? (ro - ri)/ng : 0.0);

  double r = ro;

  std::vector<int> groupInds;

  this->getGroupInds(groupInds);

  for (const auto &groupInd : groupInds) {
    auto pg = groupDatas_.find(groupInd);

    if (pg == groupDatas_.end()) {
      pg = groupDatas_.find(-1); // no group ind is (-1)

      assert(pg != groupDatas_.end());
    }

    GroupData &groupData = (*pg).second;

    //---

    // create group obj
    CQChartsGeom::BBox rect(center_.x - ro, center_.y - ro, center_.x + ro, center_.y + ro);

    CQChartsPieGroupObj *groupObj =
      new CQChartsPieGroupObj(this, rect, groupInd, groupData.name, ig, ng);

    groupObj->setColorInd(groupInd);

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

    groupObj->setVisible(isCount());

    addPlotObject(groupObj);

    groupObjs_.push_back(groupObj);

    //---

    groupData.groupObj = groupObj;

    //---

    r -= dr;

    ++ig;
  }

  //---

  // add individual values (not needed for count)

  if (! isCount()) {
    // init value sets
    initValueSets();

    //---

    // process model data
    class PieVisitor : public ModelVisitor {
     public:
      PieVisitor(CQChartsPiePlot *plot) :
       plot_(plot) {
      }

      State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
        plot_->addRow(parent, row);

        return State::OK;
      }

     private:
      CQChartsPiePlot *plot_ { nullptr };
    };

    PieVisitor pieVisitor(this);

    visitModel(pieVisitor);
  }

  //---

  adjustObjAngles();

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsPiePlot::
addRow(const QModelIndex &parent, int row)
{
  for (const auto &column : valueColumns()) {
    CQChartsModelIndex ind(row, column, parent);

    addRowColumn(ind);
  }
}

void
CQChartsPiePlot::
addRowColumn(const CQChartsModelIndex &ind)
{
  assert(! isCount());

  // get group ind
  int groupInd = rowGroupInd(ind);

  //---

  // hide all objects of group or individual objects of single group
  bool hidden = false;

  if (numGroups() > 1)
    hidden = isSetHidden(groupInd);
  else
    hidden = isSetHidden(ind.row);

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
    CQChartsModelIndex rind(ind.row, radiusColumn(), ind.parent);

    hasRadius = getColumnSizeValue(rind, radius, radiusMissing);
  }

  //---

  // get value label (used for unique values in group)
  bool ok;

  QString label;

  if (numGroups() > 1) {
    if (valueColumns().size() > 1 && ! isGroupHeaders())
      label = modelHeaderString(ind.column, ok);
    else
      label = modelString(ind.row, labelColumn(), ind.parent, ok);
  }
  else {
    label = modelString(ind.row, labelColumn(), ind.parent, ok);
  }

  if (! label.length())
    label = QString("%1").arg(ind.row);

  //---

  // get key label
  QString keyLabel = label;

  if (keyLabelColumn().isValid()) {
    bool ok;

    keyLabel = modelString(ind.row, keyLabelColumn(), ind.parent, ok);
  }

  //---

  // get group obj
  auto pg = groupDatas_.find(groupInd);
  assert(pg != groupDatas_.end());

  GroupData &groupData = (*pg).second;

  CQChartsPieGroupObj *groupObj = groupData.groupObj;

  //---

  // set radii
  double ri = (groupObj ? groupObj->innerRadius() : innerRadius());
  double ro = (groupObj ? groupObj->outerRadius() : outerRadius());
  double rv = ro;

  if (hasRadius && groupObj && groupObj->isRadiusScaled()) {
    double dr = ro - ri;
    double s  = (groupObj->radiusMax() > 0.0 ? radius/groupObj->radiusMax() : 1.0);

    rv = ri + s*dr;
  }

  //---

  QModelIndex dataInd  = modelIndex(ind.row, ind.column, ind.parent);
  QModelIndex dataInd1 = normalizeIndex(dataInd);

  //---

  // get pie object (by label)
  CQChartsPieObj *obj = (groupObj ? groupObj->lookupObj(label) : nullptr);

  if (! obj) {
    CQChartsGeom::BBox rect(center_.x - ro, center_.y - ro, center_.x + ro, center_.y + ro);

    int objInd = (groupObj ? groupObj->numObjs() : 0);

    obj = new CQChartsPieObj(this, rect, dataInd1);

    if (hidden)
      obj->setVisible(false);

    obj->setColorInd(objInd);

    obj->setInnerRadius(ri);
    obj->setOuterRadius(ro);
    obj->setValueRadius(rv);

    obj->setLabel  (label);
    obj->setValue  (value);
    obj->setMissing(valueMissing);

    if (hasRadius)
      obj->setRadius(radius);

    obj->setKeyLabel(keyLabel);

    CQChartsColor color;

    if (colorSetColor("color", ind.row, color))
      obj->setColor(color);

    addPlotObject(obj);

    if (groupObj)
      groupObj->addObject(obj);
  }
  else {
    // duplicate labels add to value (and radius ?)

    if (! valueMissing)
      obj->setValue(obj->value() + value);

    if (hasRadius)
      obj->setRadius(*obj->radius() + radius);

    // TODO: add dataInd
  }
}

void
CQChartsPiePlot::
calcDataTotal()
{
  groupDatas_.clear();

  // process model data
  class DataTotalVisitor : public ModelVisitor {
   public:
    DataTotalVisitor(CQChartsPiePlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      plot_->addRowDataTotal(parent, row);

      return State::OK;
    }

   private:
    CQChartsPiePlot *plot_ { nullptr };
  };

  DataTotalVisitor dataTotalVisitor(this);

  visitModel(dataTotalVisitor);
}

void
CQChartsPiePlot::
addRowDataTotal(const QModelIndex &parent, int row)
{
  for (const auto &column : valueColumns()) {
    CQChartsModelIndex ind(row, column, parent);

    addRowColumnDataTotal(ind);
  }
}

void
CQChartsPiePlot::
addRowColumnDataTotal(const CQChartsModelIndex &ind)
{
  // get group ind
  int groupInd = rowGroupInd(ind);

  //---

  // hide all objects of group or individual objects of single group
  bool hidden = false;

  if (! isCount()) {
    if (numGroups() > 1)
      hidden = isSetHidden(groupInd);
    else
      hidden = isSetHidden(ind.row);
  }
  else {
    hidden = isSetHidden(groupInd);
  }

  //---

  // get value
  double value        = 1.0;
  bool   valueMissing = false;

  if (! getColumnSizeValue(ind, value, valueMissing))
    return;

  //---

  // get group data for group ind (add if new)
  auto pg = groupDatas_.find(groupInd);

  if (pg == groupDatas_.end()) {
    QString groupName = groupIndName(groupInd);

    pg = groupDatas_.insert(pg, GroupDatas::value_type(groupInd, GroupData(groupName)));
  }

  GroupData &groupData = (*pg).second;

  //---

  // sum values
  if (! hidden) {
    ++groupData.numValues;

    groupData.dataTotal += value;
  }

  //---

  // get max radius
  if (radiusColumn().isValid()) {
    CQChartsModelIndex rind(ind.row, radiusColumn(), ind.parent);

    double radius        = 0.0;
    bool   radiusMissing = false;

    if (getColumnSizeValue(rind, radius, radiusMissing)) {
      if (! hidden) {
        groupData.radiusScaled = true;
        groupData.radiusMax    = std::max(groupData.radiusMax, radius);
      }
    }
  }
}

bool
CQChartsPiePlot::
getColumnSizeValue(const CQChartsModelIndex &ind, double &value, bool &missing) const
{
  missing = false;

  ColumnType columnType = columnValueType(ind.column);

  if (columnType == ColumnType::INTEGER || columnType == ColumnType::REAL) {
    bool ok;

    value = modelReal(ind.row, ind.column, ind.parent, ok);

    // allow missing value in numeric column
    if (! ok) {
      missing = true;
      value   = 1.0;

      return true;
    }

    // TODO: check allow nan
    if (CMathUtil::isNaN(value))
      return false;

    // size must be positive
    if (value <= 0.0)
      return false;
  }
  else {
    // try convert model string to real
    bool ok;

    value = modelReal(ind.row, ind.column, ind.parent, ok);

    // string non-real -> 1.0
    if (! ok) {
      value = 1.0;
      return true;
    }

    // string bad size -> 1.0 (assume bad conversion)
    if (value <= 0.0) {
      value = 1.0;
      return true;
    }
  }

  return true;
}

void
CQChartsPiePlot::
adjustObjAngles()
{
  double ro = outerRadius();
  double ri = (isDonut() ? innerRadius()*outerRadius() : 0.0);

  bool isGrouped = (numGroups() > 1);

  int    ng = 1, nh = 0;
  double dr = 0.0;

  if (! isCount()) {
    if (isGrouped) {
      ng = 0;

      for (auto &groupObj : groupObjs_) {
        if (! isSetHidden(groupObj->groupInd()))
          ++ng;
        else
          ++nh;
      }
    }

    dr = (ng > 0 ? (ro - ri)/ng : 0.0);
  }

  //---

  int totalValues = 0;

  for (auto &groupObj : groupObjs_) {
    totalValues += groupObj->numValues();
  }

  //---

  double ga1 = startAngle();
  double ga2 = ga1;

  double r = ro;

  for (auto &groupObj : groupObjs_) {
    ga1 = ga2;

    if (! isCount()) {
      if (isGrouped && nh > 0) {
        if (isSetHidden(groupObj->groupInd()))
          continue;
      }
    }
    else {
      if (isSetHidden(groupObj->groupInd()))
        continue;
    }

    //---

    double dga = (totalValues > 0 ? 360.0*groupObj->numValues()/totalValues : 0);

    ga2 = ga1 - dga;

    groupObj->setAngles(ga1, ga2);

    //---

    if (! isCount()) {
      if (isGrouped && nh > 0) {
        groupObj->setInnerRadius(r - dr);
        groupObj->setOuterRadius(r);
      }

      //---

      double angle1    = startAngle();
      double alen      = CMathUtil::clamp(angleExtent(), -360.0, 360.0);
      double dataTotal = groupObj->dataTotal();

      for (auto &obj : groupObj->objs()) {
        if (! obj->isVisible())
          continue;

        double value = obj->value();

        double angle = (dataTotal > 0.0 ? alen*value/dataTotal : 0.0);

        double angle2 = angle1 - angle;

        obj->setAngle1(angle1);
        obj->setAngle2(angle2);

        if (isGrouped && nh > 0) {
          obj->setInnerRadius(r - dr);
          obj->setOuterRadius(r);

          double rv = r;

          if (obj->radius() && groupObj->isRadiusScaled()) {
            double s = (groupObj->radiusMax() > 0.0 ? *obj->radius()/groupObj->radiusMax() : 1.0);

            rv = r + (s - 1)*dr;
          }

          obj->setValueRadius(rv);
        }

        angle1 = angle2;
      }

      //---

      if (isGrouped && nh > 0)
        r -= dr;
    }
  }
}

void
CQChartsPiePlot::
addKeyItems(CQChartsPlotKey *key)
{
  int row = 0;

  auto addKeyRow = [&](CQChartsPlotObj *obj) {
    CQChartsPieKeyColor *keyColor = new CQChartsPieKeyColor(this, obj);
    CQChartsPieKeyText  *keyText  = new CQChartsPieKeyText (this, obj);

    key->addItem(keyColor, row, 0);
    key->addItem(keyText , row, 1);

    ++row;
  };

  //---

  if (! isCount()) {
    int ng = groupObjs_.size();

    if (ng > 1) {
      for (const auto &groupObj : groupObjs_)
        addKeyRow(groupObj);
    }
    else {
      for (auto &plotObj : plotObjs_) {
        CQChartsPieObj *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

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
handleResize()
{
  CQChartsPlot::handleResize();

  dataRange_.reset();
}

//------

bool
CQChartsPiePlot::
addMenuItems(QMenu *menu)
{
  QAction *donutAction = new QAction("Donut", menu);

  donutAction->setCheckable(true);
  donutAction->setChecked(isDonut());

  connect(donutAction, SIGNAL(triggered(bool)), this, SLOT(setDonut(bool)));

  //---

  QAction *countAction = new QAction("Count", menu);

  countAction->setCheckable(true);
  countAction->setChecked(isCount());

  connect(countAction, SIGNAL(triggered(bool)), this, SLOT(setCount(bool)));

  //---

  menu->addSeparator();

  menu->addAction(donutAction);
  menu->addAction(countAction);

  return true;
}

//------

CQChartsPieObj::
CQChartsPieObj(CQChartsPiePlot *plot, const CQChartsGeom::BBox &rect, const QModelIndex &ind) :
 CQChartsPlotObj(plot, rect), plot_(plot), ind_(ind)
{
}

QString
CQChartsPieObj::
calcId() const
{
  return QString("pie:%1").arg(colorInd());
}

QString
CQChartsPieObj::
calcTipId() const
{
  QModelIndex ind = plot_->unnormalizeIndex(ind_);

  bool hasGroup = (plot_->numGroups() > 1 && groupObj_);

  QString groupName, label;

  bool ok;

  if (hasGroup) {
    CQChartsPieGroupObj *groupObj = this->groupObj();

    groupName = groupObj->name();

    if (plot_->isGroupHeaders())
      label = plot_->modelHeaderString(ind.column(), ok);
    else
      label = plot_->modelString(ind.row(), plot_->labelColumn(), ind.parent(), ok);
  }
  else {
    label = plot_->modelString(ind.row(), plot_->labelColumn(), ind.parent(), ok);
  }

  int valueColumn = ind_.column();

  QString valueStr = plot_->columnStr(valueColumn, value_);

  //---

  CQChartsTableTip tableTip;

  if (groupName.length())
    tableTip.addTableRow("Group", tableTip.escapeText(groupName));

  tableTip.addTableRow("Name" , tableTip.escapeText(label));
  tableTip.addTableRow("Value", valueStr);

  if (radius()) {
    tableTip.addTableRow("Radius", *radius());
  }

  return tableTip.str();
}

bool
CQChartsPieObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  CQChartsGeom::Point center(0, 0);

  double r = p.distanceTo(center);

  double ri = innerRadius();
  double ro = valueRadius();

  if (r < ri || r > ro)
    return false;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(atan2(p.y - center.y, p.x - center.x));
  a = CMathUtil::normalizeAngle(a);

  double a1 = angle1(); a1 = CMathUtil::normalizeAngle(a1);
  double a2 = angle2(); a2 = CMathUtil::normalizeAngle(a2);

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
CQChartsPieObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->labelColumn());
  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsPieObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
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
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  if (! plot_->textBox()->isTextVisible())
    return bbox;

  if (! label().length())
    return bbox;

  //---

  CQChartsGeom::Point c = getCenter();

  QPointF center(c.x, c.y);

  double ri = innerRadius();
  double ro = outerRadius();
  double rv = valueRadius();
  double lr = plot_->labelRadius();

  double a1 = angle1();
  double a2 = angle2();

  double lr1;

  if (! CMathUtil::isZero(ri))
    lr1 = ri + lr*(ro - ri);
  else
    lr1 = lr*ro;

  if (lr1 < 0.01)
    lr1 = 0.01;

  double ta = CMathUtil::avg(a1, a2);

  double a21 = a2 - a1;

  // if full circle always draw text at center
  if (CMathUtil::realEq(std::abs(a21), 360.0)) {
    CQChartsGeom::Point pc;

    plot_->windowToPixel(c, pc);

    //---

    bbox = plot_->textBox()->bbox(CQChartsUtil::toQPoint(pc), label(), 0.0);
  }
  // draw on arc center line
  else {
    if (plot_->numGroups() == 1 && lr > 1.0) {
      plot_->textBox()->calcConnectedRadialTextBBox(center, rv, lr1, ta, label(),
                                                    plot_->isRotatedText(), bbox);
    }
    else {
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      double tangle = CMathUtil::Deg2Rad(ta);

      double tc = cos(tangle);
      double ts = sin(tangle);

      double tx = center.x() + lr1*tc;
      double ty = center.y() + lr1*ts;

      double ptx, pty;

      plot_->windowToPixel(tx, ty, ptx, pty);

      QPointF pt(ptx, pty);

      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (tc >= 0 ? ta : 180.0 + ta);

      bbox = plot_->textBox()->bbox(pt, label(), angle, align);
    }
  }

  return bbox;
}

void
CQChartsPieObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  CQChartsPieGroupObj *groupObj = this->groupObj();

  int ng = plot_->numGroupObjs();
  int no = (groupObj ? groupObj->numObjs() : 0);

  //---

  CQChartsGeom::Point c = getCenter();

  double ri = innerRadius();
  double ro = outerRadius();
  double rv = valueRadius();

  double a1 = angle1();
  double a2 = angle2();

  //---

  if (plot_->isGridLines()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor gridColor = plot_->interpGridLinesColor(0, 1);

    plot_->setPen(pen, true, gridColor, plot_->gridLinesAlpha(),
                  plot_->gridLinesWidth(), plot_->gridLinesDash());

    painter->setPen  (pen);
    painter->setBrush(brush);

    plot_->drawPieSlice(painter, c, ri, ro, a1, a2);
  }

  //---

  QColor bg;

  if      (color().isValid())
    bg = color().interpColor(plot_, colorInd(), no);
  else if (groupObj)
    bg = plot_->interpGroupPaletteColor(groupObj->colorInd(), ng, colorInd(), no);

  QColor fg = plot_->calcTextColor(bg);

  QPen   pen;
  QBrush brush;

  plot_->setPen  (pen  , true, fg, 1.0, CQChartsLength("0px"), CQChartsLineDash());
  plot_->setBrush(brush, true, bg, 1.0, CQChartsFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  plot_->drawPieSlice(painter, c, ri, rv, a1, a2);
}

void
CQChartsPieObj::
drawFg(QPainter *painter)
{
  if (! visible())
    return;

  CQChartsGeom::Point c = getCenter();

  drawSegmentLabel(painter, c);
}

CQChartsGeom::Point
CQChartsPieObj::
getCenter() const
{
  CQChartsGeom::Point c(0.0, 0.0);

  //---

  bool isExploded = calcExploded();

  if (! isExploded)
    return c;

  //---

  double rv = valueRadius();

  double a1 = angle1();
  double a2 = angle2();

  double angle = CMathUtil::Deg2Rad(CMathUtil::avg(a1, a2));

  double dx = plot_->explodeRadius()*rv*cos(angle);
  double dy = plot_->explodeRadius()*rv*sin(angle);

  c.x += dx;
  c.y += dy;

  return c;
}

void
CQChartsPieObj::
drawSegmentLabel(QPainter *painter, const CQChartsGeom::Point &c)
{
  if (! plot_->textBox()->isTextVisible())
    return;

  if (! label().length())
    return;

  //---

  CQChartsPieGroupObj *groupObj = this->groupObj();

  QPointF center(c.x, c.y);

  int ng = plot_->numGroupObjs();
  int no = (groupObj ? groupObj->numObjs() : 0);

  double ri = innerRadius();
  double ro = outerRadius();
  double rv = valueRadius();
  double lr = plot_->labelRadius();

  double a1 = angle1();
  double a2 = angle2();

  double lr1;

  if (! CMathUtil::isZero(ri))
    lr1 = ri + lr*(ro - ri);
  else
    lr1 = lr*ro;

  if (lr1 < 0.01)
    lr1 = 0.01;

  double ta = CMathUtil::avg(a1, a2);

  //---

  QPen lpen;

  QColor bg;

  if (groupObj)
    bg = plot_->interpGroupPaletteColor(groupObj->colorInd(), ng, colorInd(), no);

  plot_->setPen(lpen, true, bg, 1.0, CQChartsLength("0px"), CQChartsLineDash());

  //---

  double a21 = a2 - a1;

  // if full circle always draw text at center
  if (CMathUtil::realEq(std::abs(a21), 360.0)) {
    CQChartsGeom::Point pc;

    plot_->windowToPixel(c, pc);

    //---

    plot_->textBox()->draw(painter, CQChartsUtil::toQPoint(pc), label(), 0.0);
  }
  // draw on arc center line
  else {
    if (plot_->numGroups() == 1 && lr > 1.0) {
      plot_->textBox()->drawConnectedRadialText(painter, center, rv, lr1, ta, label(),
                                                lpen, plot_->isRotatedText());
    }
    else {
      //plot_->textBox()->drawConnectedRadialText(painter, center, rv, lr1, ta, label(),
      //                                          lpen, plot_->isRotatedText());

      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      double tangle = CMathUtil::Deg2Rad(ta);

      double tc = cos(tangle);
      double ts = sin(tangle);

      double tx = center.x() + lr1*tc;
      double ty = center.y() + lr1*ts;

      double ptx, pty;

      plot_->windowToPixel(tx, ty, ptx, pty);

      QPointF pt(ptx, pty);

      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (tc >= 0 ? ta : 180.0 + ta);

      plot_->textBox()->draw(painter, pt, label(), angle, align);
    }
  }
}

//------

CQChartsPieGroupObj::
CQChartsPieGroupObj(CQChartsPiePlot *plot, const CQChartsGeom::BBox &bbox,
                    int groupInd, const QString &name, int ig, int ng) :
 CQChartsGroupObj(plot, bbox), plot_(plot), groupInd_(groupInd), name_(name), ig_(ig), ng_(ng)
{
}

QString
CQChartsPieGroupObj::
calcId() const
{
  return QString("group:%1").arg(ig_);
}

QString
CQChartsPieGroupObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name" , name());
  tableTip.addTableRow("Count", numValues());

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
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  CQChartsGeom::Point center(0, 0);

  double r = p.distanceTo(center);

  double ro = plot_->outerRadius();
  double ri = (plot_->isDonut() ? plot_->innerRadius()*plot_->outerRadius() : 0.0);

  if (r < ri || r > ro)
    return false;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(atan2(p.y - center.y, p.x - center.x));
  a = CMathUtil::normalizeAngle(a);

  double a1 = startAngle_; a1 = CMathUtil::normalizeAngle(a1);
  double a2 = endAngle_  ; a2 = CMathUtil::normalizeAngle(a2);

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
draw(QPainter *painter)
{
  if (! visible())
    return;

  CQChartsGeom::Point c(0, 0);

  double ro = plot_->outerRadius();
  double ri = (plot_->isDonut() ? plot_->innerRadius()*plot_->outerRadius() : 0.0);

  double a1 = startAngle_;
  double a2 = endAngle_;

  QColor bg = bgColor();
  QColor fg = plot_->interpPlotBorderColor(0, 1);

  QPen   pen;
  QBrush brush;

  plot_->setPen  (pen  , true, fg, 1.0, CQChartsLength("0px"), CQChartsLineDash());
  plot_->setBrush(brush, true, bg, 1.0, CQChartsFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  plot_->drawPieSlice(painter, c, ri, ro, a1, a2);
}

  //---

void
CQChartsPieGroupObj::
drawFg(QPainter *painter)
{
  if (! visible())
    return;

  CQChartsGeom::Point c(0, 0);

  double a1 = startAngle_;
  double a2 = endAngle_;

  double ta = CMathUtil::avg(a1, a2);

  double tangle = CMathUtil::Deg2Rad(ta);

  double tc = cos(tangle);
  double ts = sin(tangle);

  double tx = c.getX() + 0.5*tc;
  double ty = c.getY() + 0.5*ts;

  double ptx, pty;

  plot_->windowToPixel(tx, ty, ptx, pty);

  QPointF pt(ptx, pty);

  QString label = QString("%1").arg(numValues());

  //---

  QPen pen;

  QColor fg = plot_->interpPlotBorderColor(0, 1);

  plot_->setPen(pen, true, fg, 1.0, CQChartsLength("0px"), CQChartsLineDash());

  //---

  CQChartsTextOptions textOptions;

  plot_->drawTextAtPoint(painter, pt, label, pen, textOptions);
}

QColor
CQChartsPieGroupObj::
bgColor() const
{
  return plot_->interpPaletteColor(ig_, ng_);
}

//------

CQChartsPieKeyColor::
CQChartsPieKeyColor(CQChartsPiePlot *plot, CQChartsPlotObj *obj) :
 CQChartsKeyColorBox(plot, 0, 1), obj_(obj)
{
}

bool
CQChartsPieKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  CQChartsPieGroupObj *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  CQChartsPieObj      *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  int ih = 0;

  if      (group)
    ih = group->groupInd();
  else if (obj)
    ih = obj->colorInd();

  plot->setSetHidden(ih, ! plot->isSetHidden(ih));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsPieKeyColor::
fillBrush() const
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  CQChartsPieGroupObj *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  CQChartsPieObj      *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  int ng = plot->numGroups();

  QColor c;
  int    no = 1;

  if      (group) {
    if (! plot->isCount()) {
      int ig = group->groupInd();

      no = group->numObjs();

      c = plot->interpGroupPaletteColor(ig, ng, 0, no);

      if (plot->isSetHidden(ig))
        c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);
    }
    else
      c = group->bgColor();
  }
  else if (obj) {
    CQChartsPieGroupObj *group = obj->groupObj();

    no = group->numObjs();

    int ig = group->colorInd();
    int io = obj->colorInd();

    c = plot->interpGroupPaletteColor(ig, ng, io, no);

    if (plot->isSetHidden(io))
      c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);
  }

  if (obj && obj->color().isValid())
    c = obj->color().interpColor(plot_, obj->colorInd(), no);

  return c;
}

//------

CQChartsPieKeyText::
CQChartsPieKeyText(CQChartsPiePlot *plot, CQChartsPlotObj *plotObj) :
 CQChartsKeyText(plot, ""), obj_(plotObj)
{
  CQChartsPieGroupObj *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  CQChartsPieObj      *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  if (group)
    setText(group->name());
  else
    setText(obj->keyLabel());
}

QColor
CQChartsPieKeyText::
interpTextColor(int i, int n) const
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  CQChartsPieGroupObj *group = dynamic_cast<CQChartsPieGroupObj *>(obj_);
  CQChartsPieObj      *obj   = dynamic_cast<CQChartsPieObj      *>(obj_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  int ih = 0;

  if      (group)
    ih = group->groupInd();
  else if (obj)
    ih = obj->colorInd();

  if (plot && plot->isSetHidden(ih))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}

//------

CQChartsPieTextObj::
CQChartsPieTextObj(CQChartsPiePlot *plot) :
 CQChartsRotatedTextBoxObj(plot), plot_(plot)
{
}
