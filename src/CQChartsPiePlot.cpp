#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <QPainter>

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
  addColumnsParameter("data"    , "Data"     , "dataColumns"   , "1").setRequired();
  addColumnParameter ("label"   , "Label"    , "labelColumn"   , 0);
  addColumnParameter ("radius"  , "Radius"   , "radiusColumn"  );
  addColumnParameter ("keyLabel", "Key Label", "keyLabelColumn");
  addColumnParameter ("color"   , "Color"    , "colorColumn"   ).setTip("Custom slice color");
  addBoolParameter   ("donut"   , "Donut"    , "donut"         ).setTip("Draw donut");

  endParameterGroup();

  //---

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
 CQChartsGroupPlot(view, view->charts()->plotType("pie"), model)
{
  (void) addColorSet("color");

  //---

  setDataColumnsStr("1");

  gridData_.color = CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.5);

  textBox_ = new CQChartsPieTextObj(this);

  setLayerActive(Layer::FG, true);

  addKey();

  addTitle();
}

CQChartsPiePlot::
~CQChartsPiePlot()
{
  for (auto &groupObj : groupObjs_)
    delete groupObj;

  delete textBox_;
}

//---

void
CQChartsPiePlot::
setLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

const CQChartsColumn &
CQChartsPiePlot::
dataColumn() const
{
  assert(! dataColumns_.empty());

  return dataColumns_[0];
}

void
CQChartsPiePlot::
setDataColumn(const CQChartsColumn &c)
{
  if (dataColumns_.size() != 1 || c != dataColumn()) {
    dataColumns_.clear();

    dataColumns_.push_back(c);

    updateRangeAndObjs();
  }
}

void
CQChartsPiePlot::
setDataColumns(const Columns &dataColumns)
{
  if (dataColumns != dataColumns_) {
    dataColumns_ = dataColumns;

    if (dataColumns_.empty())
      dataColumns_.push_back(CQChartsColumn());

    updateRangeAndObjs();
  }
}

QString
CQChartsPiePlot::
dataColumnsStr() const
{
  return CQChartsColumn::columnsToString(dataColumns());
}

bool
CQChartsPiePlot::
setDataColumnsStr(const QString &s)
{
  Columns dataColumns;

  if (! CQChartsColumn::stringToColumns(s, dataColumns))
    return false;

  setDataColumns(dataColumns);

  return true;
}

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

//---

void
CQChartsPiePlot::
setGrid(bool b)
{
  CQChartsUtil::testAndSet(gridData_.visible, b, [&]() { update(); } );
}

void
CQChartsPiePlot::
setGridColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(gridData_.color, c, [&]() { update(); } );
}

QColor
CQChartsPiePlot::
interpGridColor(int i, int n)
{
  return gridColor().interpColor(this, i, n);
}

void
CQChartsPiePlot::
setGridAlpha(double r)
{
  CQChartsUtil::testAndSet(gridData_.alpha, r, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(rotatedText_, b, [&]() { update(); } );
}

void
CQChartsPiePlot::
setExplodeSelected(bool b)
{
  CQChartsUtil::testAndSet(explodeSelected_, b, [&]() { update(); } );
}

void
CQChartsPiePlot::
setExplodeRadius(double r)
{
  CQChartsUtil::testAndSet(explodeRadius_, r, [&]() { update(); } );
}

//---

void
CQChartsPiePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "dataColumn"    , "data"    );
  addProperty("columns", this, "dataColumns"   , "dataSet" );
  addProperty("columns", this, "labelColumn"   , "label"   );
  addProperty("columns", this, "radiusColumn"  , "radius"  );
  addProperty("columns", this, "keyLabelColumn", "keyLabel");
  addProperty("columns", this, "colorColumn"   , "color"   );

  CQChartsGroupPlot::addProperties();

  // general
  addProperty("options", this, "donut"      );
  addProperty("options", this, "innerRadius");
  addProperty("options", this, "startAngle" );
  addProperty("options", this, "angleExtent");

  // grid
  addProperty("grid", this, "grid"     , "visible");
  addProperty("grid", this, "gridColor", "color");
  addProperty("grid", this, "gridAlpha", "alpha");

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

  // colormap
  addProperty("color", this, "colorMapped", "mapped");
  addProperty("color", this, "colorMapMin", "mapMin");
  addProperty("color", this, "colorMapMax", "mapMax");
}

//---

void
CQChartsPiePlot::
updateRange(bool apply)
{
  double r = std::max(1.0, labelRadius());

  //---

  CQChartsGeom::Point c(0.0, 0.0);

  dataRange_.reset();

  //dataRange_.updateRange(-r, -r);
  //dataRange_.updateRange( r,  r);

  dataRange_.updateRange(c);

  //---

  double angle1 = startAngle();
  double alen   = std::min(std::max(angleExtent(), -360.0), 360.0);
  double angle2 = angle1 - alen;

  dataRange_.updateRange(CQChartsUtil::AngleToPoint(c, r, angle1));
  dataRange_.updateRange(CQChartsUtil::AngleToPoint(c, r, angle2));

  double a1 = 90.0*CQChartsUtil::RoundDownF(angle1/90.0);

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

  //initGroupData(dataColumns(), labelColumn());
  initGroupData(dataColumns(), CQChartsColumn());

#if 0
  // if group column defined use that
  // if multiple data columns then use label column and data labels
  //   if row grouping we are creating a value set per row (1 value per data column)
  //   if column grouping we are creating a value set per data column (1 value per row)
  // otherwise (single data column) just use dummy group (column -1)
  CQChartsGroupData groupData;

  groupData.exactValue = isExactValue();
  groupData.bucketer   = groupData_.bucketer;
  groupData.usePath    = false;

  // use multiple group columns
  if      (dataColumns().size() > 1) {
    groupData.columns     = dataColumns();
    groupData.rowGrouping = isRowGrouping(); // only used if multiple groups

    if      (groupColumn().isValid())
      groupData.column = groupColumn();
    else if (labelColumn().isValid())
      groupData.column = labelColumn();
  }
  // use single group column
  else if (groupColumn().isValid()) {
    groupData.column = groupColumn();
  }
  // use path and hierarchical
  else if (isUsePath() && isHierarchical()) {
    groupData.usePath = true;
  }
  // use row
  else if (isUseRow()) {
    groupData.useRow = true;
  }
  // default use label column if defined
  else if (labelColumn().isValid()) {
    //groupData.column = labelColumn();
  }

  initGroup(groupData);
#endif

#if 0
  if      (dataColumns().size() > 1) {
    groupData.columns     = dataColumns();
    groupData.rowGrouping = isRowGrouping();
    groupData.column      = labelColumn();
  }
  else if (groupColumn().isValid()) {
    groupData.column = groupColumn();
  }

  initGroup(groupData);
#endif

  //---

  if (apply)
    applyDataRange();
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

  for (auto &groupObj : groupObjs_)
    delete groupObj;

  groupObjs_.clear();

  //---

  double ro = outerRadius();
  double ri = 0.0;

  if (isDonut())
    ri = innerRadius()*outerRadius();

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
    CQChartsPieGroupObj *groupObj = new CQChartsPieGroupObj(this, groupData.name);

    groupObj->setColorInd(groupInd);

    groupObj->setDataTotal(groupData.dataTotal);

    groupObj->setRadiusMax   (groupData.radiusMax);
    groupObj->setRadiusScaled(groupData.radiusScaled);

    groupObj->setInnerRadius(r - dr);
    groupObj->setOuterRadius(r);

    groupObjs_.push_back(groupObj);

    //---

    groupData.groupObj = groupObj;

    //---

    r -= dr;
  }

  //---

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
  for (const auto &column : dataColumns())
    addRowColumn(parent, row, column);
}

void
CQChartsPiePlot::
addRowColumn(const QModelIndex &parent, int row, const CQChartsColumn &dataColumn)
{
  // get group ind
  int groupInd = rowGroupInd(parent, row, dataColumn);

  //---

  bool hidden = false;

  if (numGroups() > 1)
    hidden = isSetHidden(groupInd);
  else
    hidden = isSetHidden(row);

  //---

  double value        = 1.0;
  bool   valueMissing = false;

  if (! getColumnSizeValue(row, dataColumn, parent, value, valueMissing))
    return;

  //---

  double radius        = 0.0;
  bool   radiusMissing = false;

  bool hasRadius = false;

  if (radiusColumn().isValid())
    hasRadius = getColumnSizeValue(row, radiusColumn(), parent, radius, radiusMissing);

  //---

  // get value label (used for unique values in group)
  bool ok;

  QString label;

  if (numGroups() > 1) {
    if (dataColumns().size() > 1 && ! isRowGrouping())
      label = modelHeaderString(dataColumn, ok);
    else
      label = modelString(row, labelColumn(), parent, ok);
  }
  else {
    label = modelString(row, labelColumn(), parent, ok);
  }

  if (! label.length())
    label = QString("%1").arg(row);

  //---

  QString keyLabel = label;

  if (keyLabelColumn().isValid()) {
    bool ok;

    keyLabel = modelString(row, keyLabelColumn(), parent, ok);
  }

  //---

  auto pg = groupDatas_.find(groupInd);
  assert(pg != groupDatas_.end());

  GroupData &groupData = (*pg).second;

  CQChartsPieGroupObj *groupObj = groupData.groupObj;

  double ri = (groupObj ? groupObj->innerRadius() : innerRadius());
  double ro = (groupObj ? groupObj->outerRadius() : outerRadius());
  double rv = ro;

  if (hasRadius && groupObj && groupObj->isRadiusScaled()) {
    double dr = ro - ri;
    double s  = (groupObj->radiusMax() > 0.0 ? radius/groupObj->radiusMax() : 1.0);

    rv = ri + s*dr;
  }

  //---

  QModelIndex dataInd  = modelIndex(row, dataColumn, parent);
  QModelIndex dataInd1 = normalizeIndex(dataInd);

  //---

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

    OptColor color;

    if (colorSetColor("color", row, color))
      obj->setColor(*color);

    addPlotObject(obj);

    if (groupObj)
      groupObj->addObject(obj);
  }
  else {
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
  for (const auto &column : dataColumns())
    addRowColumnDataTotal(parent, row, column);
}

void
CQChartsPiePlot::
addRowColumnDataTotal(const QModelIndex &parent, int row, const CQChartsColumn &dataColumn)
{
  // get group ind
  int groupInd = rowGroupInd(parent, row, dataColumn);

  //---

  bool hidden = false;

  if (numGroups() > 1)
    hidden = isSetHidden(groupInd);
  else
    hidden = isSetHidden(row);

  //---

  double value        = 1.0;
  bool   valueMissing = false;

  if (! getColumnSizeValue(row, dataColumn, parent, value, valueMissing))
    return;

  //---

  // get group data for group ind (add if new)
  auto pg = groupDatas_.find(groupInd);

  if (pg == groupDatas_.end()) {
    QString groupName = groupIndName(groupInd);

    pg = groupDatas_.insert(pg, GroupDatas::value_type(groupInd, GroupData(groupName)));
  }

  GroupData &groupData = (*pg).second;

  if (! hidden)
    groupData.dataTotal += value;

  //---

  if (radiusColumn().isValid()) {
    double radius        = 0.0;
    bool   radiusMissing = false;

    if (getColumnSizeValue(row, radiusColumn(), parent, radius, radiusMissing)) {
      if (! hidden) {
        groupData.radiusScaled = true;
        groupData.radiusMax    = std::max(groupData.radiusMax, radius);
      }
    }
  }
}

bool
CQChartsPiePlot::
getColumnSizeValue(int row, const CQChartsColumn &column, const QModelIndex &parent,
                   double &value, bool &missing) const
{
  missing = false;

  ColumnType columnType = columnValueType(column);

  if (columnType == ColumnType::INTEGER || columnType == ColumnType::REAL) {
    bool ok;

    value = modelReal(row, column, parent, ok);

    if (! ok)
      missing = true; // allow missing value

    if (CQChartsUtil::isNaN(value))
      return false;

    if (value <= 0.0)
      return false;
  }
  else {
    bool ok;

    value = modelReal(row, column, parent, ok);

    if (! ok)
      value = 1.0; // string non-real -> 1.0

    if (value <= 0.0)
      value = 1.0;
  }

  return true;
}

void
CQChartsPiePlot::
adjustObjAngles()
{
  for (auto &groupObj : groupObjs_) {
    double angle1    = startAngle();
    double alen      = std::min(std::max(angleExtent(), -360.0), 360.0);
    double dataTotal = groupObj->dataTotal();

    for (auto &obj : groupObj->objs()) {
      if (! obj->isVisible())
        continue;

      double value = obj->value();

      double angle = (dataTotal > 0.0 ? alen*value/dataTotal : 0.0);

      double angle2 = angle1 - angle;

      obj->setAngle1(angle1);
      obj->setAngle2(angle2);

      angle1 = angle2;
    }
  }
}

void
CQChartsPiePlot::
addKeyItems(CQChartsPlotKey *key)
{
  int ng = groupObjs_.size();

  if (ng > 1) {
    int i = 0;

    for (const auto &groupObj : groupObjs_) {
      CQChartsPieGroupObj *pieObj = dynamic_cast<CQChartsPieGroupObj *>(groupObj);

      if (! pieObj)
        continue;

      CQChartsPieKeyColor *color = new CQChartsPieKeyColor(this, groupObj);
      CQChartsPieKeyText  *text  = new CQChartsPieKeyText (this, groupObj);

      key->addItem(color, i, 0);
      key->addItem(text , i, 1);

      ++i;
    }
  }
  else {
    int i = 0;

    for (auto &plotObj : plotObjs_) {
      CQChartsPieObj *pieObj = dynamic_cast<CQChartsPieObj *>(plotObj);

      if (! pieObj)
        continue;

      CQChartsPieKeyColor *color = new CQChartsPieKeyColor(this, plotObj);
      CQChartsPieKeyText  *text  = new CQChartsPieKeyText (this, plotObj);

      key->addItem(color, i, 0);
      key->addItem(text , i, 1);

      ++i;
    }
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

void
CQChartsPiePlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

CQChartsPieObj::
CQChartsPieObj(CQChartsPiePlot *plot, const CQChartsGeom::BBox &rect, const QModelIndex &ind) :
 CQChartsPlotObj(plot, rect), plot_(plot), ind_(ind)
{
}

QString
CQChartsPieObj::
calcTipId() const
{
  QModelIndex ind = plot_->unnormalizeIndex(ind_);

  QString groupName, label;

  bool ok;

  if (plot_->dataColumns().size() > 1) {
    CQChartsPieGroupObj *groupObj = this->groupObj();

    groupName = groupObj->name();

    if (! plot_->isRowGrouping()) {
      label = plot_->modelHeaderString(ind.column(), ok);
    }
    else {
      label = plot_->modelString(ind.row(), plot_->labelColumn(), ind.parent(), ok);
    }
  }
  else {
    label = plot_->modelString(ind.row(), plot_->labelColumn(), ind.parent(), ok);
  }

  int dataColumn = ind_.column();

  QString valueStr = plot_->columnStr(dataColumn, value_);

  //---

  CQChartsTableTip tableTip;

  if (plot_->dataColumns().size() > 1)
    tableTip.addTableRow("Group", tableTip.escapeText(groupName));

  tableTip.addTableRow("Name" , tableTip.escapeText(label));
  tableTip.addTableRow("Value", valueStr);

  if (radius()) {
    tableTip.addTableRow("Radius", *radius());
  }

  return tableTip.str();
}

QString
CQChartsPieObj::
calcId() const
{
  QModelIndex ind = plot_->unnormalizeIndex(ind_);

  bool ok;

  QString label = plot_->modelString(ind.row(), plot_->labelColumn(), ind.parent(), ok);

  int dataColumn = ind_.column();

  QString valueStr = plot_->columnStr(dataColumn, value_);

  return QString("%1:%2").arg(label).arg(valueStr);
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
  double a = CQChartsUtil::Rad2Deg(atan2(p.y - center.y, p.x - center.x));
  a = CQChartsUtil::normalizeAngle(a);

  double a1 = angle1(); a1 = CQChartsUtil::normalizeAngle(a1);
  double a2 = angle2(); a2 = CQChartsUtil::normalizeAngle(a2);

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
  addColumnSelectIndex(inds, plot_->dataColumn ());
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

void
CQChartsPieObj::
draw(QPainter *painter, const CQChartsPlot::Layer &layer)
{
  if (! visible())
    return;

  CQChartsPieGroupObj *groupObj = this->groupObj();

  int ng = plot_->numGroupObjs();
  int no = (groupObj ? groupObj->numObjs() : 0);

  //---

  CQChartsGeom::Point center(0.0, 0.0);

  CQChartsGeom::Point c = center;

  double ri = innerRadius();
  double ro = outerRadius();
  double rv = valueRadius();

  double a1 = angle1();
  double a2 = angle2();

  //---

  bool isExploded = calcExploded();

  if (isExploded) {
    double angle = CQChartsUtil::Deg2Rad(CQChartsUtil::avg(a1, a2));

    double dx = plot_->explodeRadius()*rv*cos(angle);
    double dy = plot_->explodeRadius()*rv*sin(angle);

    c.x += dx;
    c.y += dy;
  }

  //---

  //CQChartsGeom::Point pc;

  //plot_->windowToPixel(c, pc);

  //---

  //CQChartsGeom::BBox bbox(c.x - rv, c.y - rv, c.x + rv, c.y + rv);

  //CQChartsGeom::BBox pbbox;

  //plot_->windowToPixel(bbox, pbbox);

  //---

  if (layer == CQChartsPlot::Layer::MID) {
    if (plot_->isGrid()) {
      QColor gridColor = plot_->interpGridColor(0, 1);

      gridColor.setAlphaF(plot_->gridAlpha());

      QPen   pen  (gridColor);
      QBrush brush(Qt::NoBrush);

      painter->setPen  (pen);
      painter->setBrush(brush);

      plot_->drawPieSlice(painter, c, ri, ro, a1, a2);
    }

    //---

    QColor bg;

    if      (color())
      bg = color()->interpColor(plot_, colorInd(), no);
    else if (groupObj)
      bg = plot_->interpGroupPaletteColor(groupObj->colorInd(), ng, colorInd(), no);

    QColor fg = plot_->textColor(bg);

    QPen   pen  (fg);
    QBrush brush(bg);

    plot_->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    plot_->drawPieSlice(painter, c, ri, rv, a1, a2);
  }

  //---

  if (layer == CQChartsPlot::Layer::FG) {
    drawSegmentLabel(painter, c);
  }
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

  if (! CQChartsUtil::isZero(ri))
    lr1 = ri + lr*(ro - ri);
  else
    lr1 = lr*ro;

  if (lr1 < 0.01)
    lr1 = 0.01;

  double ta = CQChartsUtil::avg(a1, a2);

  QColor bg;

  if (groupObj)
    bg = plot_->interpGroupPaletteColor(groupObj->colorInd(), ng, colorInd(), no);

  QPen lpen(bg);

  double a21 = a2 - a1;

  // if full circle always draw text at center
  if (CQChartsUtil::realEq(std::abs(a21), 360.0)) {
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

      double tangle = CQChartsUtil::Deg2Rad(ta);

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
CQChartsPieGroupObj(CQChartsPiePlot *plot, const QString &name) :
 CQChartsGroupObj(plot), plot_(plot), name_(name)
{
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
  for (const auto &obj : objs_)
    if (obj->label() == name)
      return obj;

  return nullptr;
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
    ih = group->colorInd();
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
    int ig = group->colorInd();

    no = group->numObjs();

    c = plot->interpGroupPaletteColor(ig, ng, 0, no);

    if (plot->isSetHidden(ig))
      c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);
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

  if (obj && obj->color())
    c = obj->color()->interpColor(plot_, obj->colorInd(), no);

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
    ih = group->colorInd();
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
