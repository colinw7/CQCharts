#include <CQChartsBarChartPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsBoxObj.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>
#include <QMenu>

CQChartsBarChartPlotType::
CQChartsBarChartPlotType()
{
}

void
CQChartsBarChartPlotType::
addParameters()
{
  addColumnsParameter("value"   , "Value"   , "valueColumns", "1").
    setRequired().setTip("Distribution value columns");
  addColumnParameter ("category", "Category", "categoryColumn").setTip("Grouping category");
  addColumnParameter ("name"    , "Name"    , "nameColumn"    ).setTip("Grouping name");
  addColumnParameter ("label"   , "Label"   , "labelColumn"   ).setTip("Bar data label");
  addColumnParameter ("color"   , "Color"   , "colorColumn"   ).setTip("Custom bar color");

  addBoolParameter("rowGrouping", "Row Grouping", "rowGrouping").
    setTip("Group by value column instead of category");
  addBoolParameter("colorBySet", "Color by Set", "colorBySet").setTip("Color by value set");
  addBoolParameter("stacked"   , "Stacked"     , "stacked"   ).setTip("Stack grouped values");
  addBoolParameter("percent"   , "Percent"     , "percent"   ).setTip("Show value is percentage");
  addBoolParameter("rangeBar"  , "Range Bar"   , "rangeBar"  ).setTip("show value range in bar");
  addBoolParameter("horizontal", "Horizontal"  , "horizontal").setTip("draw bars horizontal");

  CQChartsPlotType::addParameters();
}

CQChartsPlot *
CQChartsBarChartPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBarChartPlot(view, model);
}

//---

CQChartsBarChartPlot::
CQChartsBarChartPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("barchart"), model), dataLabel_(this)
{
  (void) addColorSet("color");

  boxData_.shape.background.visible = true;
  boxData_.shape.background.color   = CQChartsColor(CQChartsColor::Type::PALETTE);

  //---

  setBorder(true);

  setLayerActive(Layer::FG, true);

  addAxes();

  addKey();

  addTitle();
}

CQChartsBarChartPlot::
~CQChartsBarChartPlot()
{
}

//---

void
CQChartsBarChartPlot::
setCategoryColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(categoryColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setValueColumn(const CQChartsColumn &c)
{
  if (c != valueColumn_) {
    valueColumn_ = c;

    valueColumns_.clear();

    if (valueColumn_.isValid())
      valueColumns_.push_back(valueColumn_);

    updateRangeAndObjs();
  }
}

void
CQChartsBarChartPlot::
setValueColumns(const Columns &valueColumns)
{
  if (valueColumns != valueColumns_) {
    valueColumns_ = valueColumns;

    if (! valueColumns_.empty())
      valueColumn_ = valueColumnAt(0);
    else
      valueColumn_ = -1;

    updateRangeAndObjs();
  }
}

QString
CQChartsBarChartPlot::
valueColumnsStr() const
{
  return CQChartsColumn::columnsToString(valueColumns());
}

bool
CQChartsBarChartPlot::
setValueColumnsStr(const QString &s)
{
  Columns valueColumns;

  if (! CQChartsColumn::stringToColumns(s, valueColumns))
    return false;

  setValueColumns(valueColumns);

  return true;
}

void
CQChartsBarChartPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBarChartPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "valueColumn"   , "value"      );
  addProperty("columns", this, "valueColumns"  , "valuesSet"  );
  addProperty("columns", this, "categoryColumn", "category"   );
  addProperty("columns", this, "nameColumn"    , "name"       );
  addProperty("columns", this, "labelColumn"   , "label"      );
  addProperty("columns", this, "colorColumn"   , "color"      );

  addProperty("options", this, "rowGrouping");
  addProperty("options", this, "colorBySet");
  addProperty("options", this, "stacked"   );
  addProperty("options", this, "percent"   );
  addProperty("options", this, "rangeBar"  );
  addProperty("options", this, "horizontal");
  addProperty("options", this, "margin"    , "barMargin");

  addProperty("stroke", this, "border"     , "visible"   );
  addProperty("stroke", this, "borderColor", "color"     );
  addProperty("stroke", this, "borderAlpha", "alpha"     );
  addProperty("stroke", this, "borderWidth", "width"     );
  addProperty("stroke", this, "borderDash" , "dash"      );
  addProperty("stroke", this, "cornerSize" , "cornerSize");

  addProperty("fill", this, "barFill"   , "visible");
  addProperty("fill", this, "barColor"  , "color"  );
  addProperty("fill", this, "barAlpha"  , "alpha"  );
  addProperty("fill", this, "barPattern", "pattern");

  dataLabel_.addProperties("dataLabel");

  addProperty("color", this, "colorMapped", "mapped");
  addProperty("color", this, "colorMapMin", "mapMin");
  addProperty("color", this, "colorMapMax", "mapMax");
}

//---

void
CQChartsBarChartPlot::
setStacked(bool b)
{
  CQChartsUtil::testAndSet(stacked_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setRangeBar(bool b)
{
  CQChartsUtil::testAndSet(rangeBar_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setPercent(bool b)
{
  CQChartsUtil::testAndSet(percent_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setHorizontal(bool b)
{
  if (b != horizontal_) {
    horizontal_ = b;

    dataLabel_.setDirection(horizontal_ ?
      CQChartsDataLabel::Direction::HORIZONTAL : CQChartsDataLabel::Direction::VERTICAL);

    updateRangeAndObjs();
  }
}

//---

void
CQChartsBarChartPlot::
setMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(margin_, l, [&]() { update(); } );
}

//---

bool
CQChartsBarChartPlot::
isBorder() const
{
  return boxData_.shape.border.visible;
}

void
CQChartsBarChartPlot::
setBorder(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsBarChartPlot::
borderColor() const
{
  return boxData_.shape.border.color;
}

void
CQChartsBarChartPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.color, c, [&]() { update(); } );
}

QColor
CQChartsBarChartPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsBarChartPlot::
borderAlpha() const
{
  return boxData_.shape.border.alpha;
}

void
CQChartsBarChartPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.alpha, a, [&]() { update(); } );
}

const CQChartsLength &
CQChartsBarChartPlot::
borderWidth() const
{
  return boxData_.shape.border.width;
}

void
CQChartsBarChartPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.width, l, [&]() { update(); } );
}

const CQChartsLineDash &
CQChartsBarChartPlot::
borderDash() const
{
  return boxData_.shape.border.dash;
}

void
CQChartsBarChartPlot::
setBorderDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.dash, d, [&]() { update(); } );
}

const CQChartsLength &
CQChartsBarChartPlot::
cornerSize() const
{
  return boxData_.cornerSize;
}

void
CQChartsBarChartPlot::
setCornerSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(boxData_.cornerSize, s, [&]() { update(); } );
}

//---

bool
CQChartsBarChartPlot::
isBarFill() const
{
  return boxData_.shape.background.visible;
}

void
CQChartsBarChartPlot::
setBarFill(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsBarChartPlot::
barColor() const
{
  return boxData_.shape.background.color;
}

void
CQChartsBarChartPlot::
setBarColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.color, c, [&]() { update(); } );
}

QColor
CQChartsBarChartPlot::
interpBarColor(int i, int n) const
{
  return barColor().interpColor(this, i, n);
}

double
CQChartsBarChartPlot::
barAlpha() const
{
  return boxData_.shape.background.alpha;
}

void
CQChartsBarChartPlot::
setBarAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.alpha, a, [&]() { update(); } );
}

CQChartsBarChartPlot::Pattern
CQChartsBarChartPlot::
barPattern() const
{
  return (Pattern) boxData_.shape.background.pattern;
}

void
CQChartsBarChartPlot::
setBarPattern(Pattern pattern)
{
  if (pattern != (Pattern) boxData_.shape.background.pattern) {
    boxData_.shape.background.pattern = (CQChartsFillPattern::Type) pattern;

    update();
  }
}

//---

void
CQChartsBarChartPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model().data();

  if (! model)
    return;

  //---

  dataRange_.reset();

  if (! isHorizontal())
    dataRange_.updateRange(-0.5, 0);
  else
    dataRange_.updateRange(0, -0.5);

  //---

  xAxis_->clearTickLabels();
  yAxis_->clearTickLabels();

  valueSets_    .clear();
  valueGroupInd_.clear();

  //---

  clearPlotObjects();

  //---

  // non-range and multiple value columns use category or name column for grouping
  GroupData groupData;

  groupData.defaultRow = true;

  if      (! isRangeBar() && valueColumns().size() > 1) {
    groupData.columns     = valueColumns();
    groupData.rowGrouping = isRowGrouping();

    if (categoryColumn().isValid())
      groupData.column = categoryColumn();
    else
      groupData.column = nameColumn();
  }
  // if category column use that for group
  else if (categoryColumn().isValid()) {
    groupData.column = categoryColumn();
  }
  // if hierarchical use parent path
  else if (isHierarchical()) {
    //initGroup(groupData);
  }
  // if no range use name or value columns for group
  else if (! isRangeBar()) {
    groupData.column      = nameColumn();
    groupData.columns     = valueColumns();
    groupData.rowGrouping = isRowGrouping();
  }
  // default no name column if defined
  else {
    groupData.column      = nameColumn();
    groupData.rowGrouping = isRowGrouping();
  }

  initGroup(groupData);

  //groupBucket().print(std::cerr);

  //---

  // process model data
  class BarChartVisitor : public ModelVisitor {
   public:
    BarChartVisitor(CQChartsBarChartPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &ind, int row) override {
      plot_->addRow(model, ind, row);

      return State::OK;
    }

   private:
    CQChartsBarChartPlot *plot_ { nullptr };
  };

  BarChartVisitor barChartVisitor(this);

  visitModel(barChartVisitor);

  //---

  // get value column names
  valueNames_.clear();

  for (const auto &valueColumn : valueColumns_) {
    bool ok;

    QString valueName = modelHeaderString(valueColumn, ok);

    valueNames_.push_back(valueName);
  }

  //---

  int nv = numValueSets();

  if (nv > 0) {
    int numVisible = 0;

    for (int j = 0; j < nv; ++j)
      numVisible += ! isSetHidden(j);

    //---

    if (! isHorizontal())
      dataRange_.updateRange(numVisible - 0.5, dataRange_.ymin());
    else
      dataRange_.updateRange(dataRange_.xmin(), numVisible - 0.5);
  }
  else {
    if (! isHorizontal())
      dataRange_.updateRange(0.5, 1.0);
    else
      dataRange_.updateRange(1.0, 0.5);
  }

  //---

  adjustDataRange();

  //---

  // needed ?
  if (! isHorizontal()) {
    if (categoryColumn().isValid())
      setXValueColumn(categoryColumn());
    else
      setXValueColumn(nameColumn());

    setYValueColumn(valueColumn());
  }
  else {
    if (categoryColumn().isValid())
      setYValueColumn(categoryColumn());
    else
      setYValueColumn(nameColumn());

    setXValueColumn(valueColumn());
  }

  //---

  // set axis column and labels
  int ns = (! isRangeBar() ? numValueColumns() : 1);

  CQChartsAxis *xAxis = (! isHorizontal() ? xAxis_ : yAxis_);
  CQChartsAxis *yAxis = (! isHorizontal() ? yAxis_ : xAxis_);

  if (categoryColumn().isValid())
    xAxis->setColumn(categoryColumn());
  else
    xAxis->setColumn(nameColumn());

  bool ok;

  QString xname;

  if (ns > 1 && isRowGrouping()) {
    xname = ""; // No name for row grouping
  }
  else {
    if (categoryColumn().isValid())
      xname = modelHeaderString(categoryColumn(), ok);
    else
      xname = modelHeaderString(nameColumn(), ok);
  }

  xAxis->setLabel(xname);

  //---

  yAxis->setColumn(valueColumn());

  QString yname;

  if (valueColumns().size() <= 1) {
    bool ok;

    yname = modelHeaderString(valueColumn(), ok);
  }

  yAxis->setLabel(yname);

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsBarChartPlot::
addRow(QAbstractItemModel *model, const QModelIndex &parent, int row)
{
  // add value for each column (non-range)
  if (! isRangeBar()) {
    for (const auto &column : valueColumns()) {
      Columns columns;

      columns.push_back(column);

      addRowColumn(model, parent, row, columns);
    }
  }
  // add all values for columns (range)
  else {
    addRowColumn(model, parent, row, this->valueColumns());
  }
}

void
CQChartsBarChartPlot::
addRowColumn(QAbstractItemModel *model, const QModelIndex &parent, int row,
             const Columns &valueColumns)
{
  // get group ind
  int groupInd = -1;

  if (! isRangeBar()) {
    assert(! valueColumns.empty());

    const CQChartsColumn &valueColumn = valueColumns[0];

    groupInd = rowGroupInd(model, parent, row, valueColumn);
  }
  else {
    groupInd = rowGroupInd(model, parent, row);
  }

  // get group name
  QString groupName = groupBucket_.indName(groupInd);

  //---

  QString categoryName;

  //---

  // get category string (if defined) and update category name
  QString category;

  if (categoryColumn().isValid()) {
    bool ok1;

    category = modelHierString(row, categoryColumn(), parent, ok1);

    categoryName = category;
  }

  //---

  // get name string (if defined) and update category name
  QString name;

  if (nameColumn().isValid()) {
    bool ok2;

    name = modelString(row, nameColumn(), parent, ok2);

    if (! categoryName.length())
      categoryName = name;
  }

  //---

  // get optional data label string
  QString label;

  if (labelColumn().isValid()) {
    bool ok3;

    label = modelString(row, labelColumn(), parent, ok3);
  }

  //---

  // get value set for group
  CQChartsBarChartValueSet *valueSet = groupValueSet(groupInd);

  using ValueInd  = CQChartsBarChartValue::ValueInd;
  using ValueInds = CQChartsBarChartValue::ValueInds;

  ValueInds valueInds;

  // add values for columns (1 column normally, all columns when grouped)
  for (const auto &valueColumn : valueColumns) {
    double r;

    bool ok2 = modelMappedReal(row, valueColumn, parent, r, isLogY(), row);

    if (! ok2)
      r = row;

    if (CQChartsUtil::isNaN(r))
      continue;

    // get associated model index
    QModelIndex valInd  = model->index(row, valueColumn.column(), parent);
    QModelIndex valInd1 = normalizeIndex(valInd);

    // add value and index
    ValueInd valueInd;

    valueInd.ind   = valInd1;
    valueInd.value = r;

    valueInds.push_back(valueInd);
  }

  if (valueInds.empty())
    return;

  //---

  // store values in data
  CQChartsBarChartValue valueData;

  for (const auto &valueInd : valueInds)
    valueData.addValueInd(valueInd);

  //---

  int ns = (! isRangeBar() ? numValueColumns() : 1);

  if (ns > 1) {
    assert(! valueColumns.empty());

    const CQChartsColumn &valueColumn = valueColumns[0];

    //---

    // set value data group name and value name

    if (groupName.length())
      valueData.setGroupName(groupName);

    QString valueName;

    // not row grouping so value name is column header
    if (! isRowGrouping()) {
      bool ok;

      valueName = modelHeaderString(valueColumn, ok);
    }
    // row grouping so value name is category/name column name
    else {
      valueName = categoryName;
    }

    // save value name
    valueData.setValueName(valueName);
  }
  else {
    // if path grouping (hierarchical) then value name is group name
    if (groupBucket().dataType() == CQChartsColumnBucket::DataType::PATH) {
      if (groupName.length())
        valueData.setGroupName(groupName);
    }

    // save other name values for tip
    if (category.length()) valueData.setNameValue("Category", category);
    if (name    .length()) valueData.setNameValue("Name"    , name);
    if (label   .length()) valueData.setNameValue("Label"   , label);
  }

  // add value(s) to value set
  valueSet->addValue(valueData);

  //---

  // calc pos/neg sums
  double posSum, negSum;

  valueSet->calcSums(posSum, negSum);

  // scale for percent
  double scale = 1.0;

  if (isPercent()) {
    double total = posSum - negSum;

    scale = (total > 0.0 ? 100.0/total : 1.0);
  }

  // update range for scale and sums
  if (! isHorizontal()) {
    if (isStacked()) {
      dataRange_.updateRange(0, scale*posSum);
      dataRange_.updateRange(0, scale*negSum);
    }
    else {
      for (const auto &valueInd : valueInds)
        dataRange_.updateRange(0, scale*valueInd.value);
    }
  }
  else {
    if (isStacked()) {
      dataRange_.updateRange(scale*posSum, 0);
      dataRange_.updateRange(scale*negSum, 0);
    }
    else {
      for (const auto &valueInd : valueInds)
        dataRange_.updateRange(scale*valueInd.value, 0);
    }
  }
}

CQChartsBarChartValueSet *
CQChartsBarChartPlot::
groupValueSet(int groupInd)
{
  auto p = valueGroupInd_.find(groupInd);

  if (p != valueGroupInd_.end()) {
    int ind = (*p).second;

    assert(ind >= 0 && ind < int(valueSets_.size()));

    return &valueSets_[ind];
  }

  int ind = numValueSets();

  QString name = groupBucket_.indName(groupInd);

  valueSets_.emplace_back(name, ind);

  valueGroupInd_[groupInd] = ind;

  return &valueSets_.back();
}

//------

CQChartsGeom::BBox
CQChartsBarChartPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  CQChartsDataLabel::Position position = dataLabel().position();

  if (position != CQChartsDataLabel::TOP_OUTSIDE && position != CQChartsDataLabel::BOTTOM_OUTSIDE)
    return bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsBarChartObj *barObj = dynamic_cast<CQChartsBarChartObj *>(plotObj);

    if (barObj)
      bbox += barObj->dataLabelRect();
  }

  return bbox;
}

//------

void
CQChartsBarChartPlot::
updateObjs()
{
  clearValueSets();

  CQChartsPlot::updateObjs();
}

bool
CQChartsBarChartPlot::
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

  // init value sets
  initValueSets();

  //---

  CQChartsAxis *xAxis = (! isHorizontal() ? xAxis_ : yAxis_);
  CQChartsAxis *yAxis = (! isHorizontal() ? yAxis_ : xAxis_);

  xAxis->setIntegral           (true);
//xAxis->setDataLabels         (true);
  xAxis->setMinorTicksDisplayed(false);

  yAxis->setIntegral           (false);
//yAxis->setDataLabels         (true);
  xAxis->setMinorTicksDisplayed(true);

  //---

  int nv = numValueSets();

  //---

  // set x axis labels
  int numVisible = 0;

  for (int j = 0; j < nv; ++j) {
    if (isSetHidden(j))
      continue;

    const CQChartsBarChartValueSet &valueSet = this->valueSet(j);

    xAxis->setTickLabel(numVisible, valueSet.name());

    ++numVisible;
  }

  //---

  int ns = (! isRangeBar() ? numValueColumns() : 1);

  // start at px1 - bar width
  double bx = -0.5;

  for (int j = 0; j < nv; ++j) {
    if (isSetHidden(j))
      continue;

    //---

    const CQChartsBarChartValueSet &valueSet = this->valueSet(j);

    //const QString &setName = valueSet.name;

    //---

    int nvs = valueSet.numValues();

    int numVisible1 = 0;

    if (ns > 1) {
      for (int i = 0; i < nvs; ++i)
        numVisible1 += ! isValueHidden(i);
    }
    else {
      numVisible1 = nvs;
    }

    //---

    double scale = 1.0;

    if (isPercent()) {
      double posSum, negSum;

      valueSet.calcSums(posSum, negSum);

      double total = posSum - negSum;

      scale = (total > 0.0 ? 100.0/total : 1.0);
    }

    //---

    double bx1 = bx;

    double bw1 = 1.0;

    if (! isStacked()) {
      if (numVisible1 > 0)
        bw1 = 1.0/numVisible1;
    }

    double lastPosValue = 0.0, lastNegValue = 0.0;

    for (int i = 0; i < nvs; ++i) {
      if (ns > 1) {
        if (isValueHidden(i))
          continue;
      }

      const CQChartsBarChartValue &ivalue = valueSet.value(i);

      CQChartsBarChartValue::ValueInd minInd, maxInd;

      ivalue.calcRange(minInd, maxInd);

      OptColor color;

      if (colorColumn().isValid())
        (void) colorSetColor("color", minInd.ind.row(), color);

      //---

      // create bar rect
      CQChartsGeom::BBox brect;

      double value1, value2;

      if (! isRangeBar()) {
        if (isStacked()) {
          if (minInd.value >= 0) {
            value1 = lastPosValue;
            value2 = value1 + scale*minInd.value;
          }
          else {
            value2 = lastNegValue;
            value1 = value2 + scale*minInd.value;
          }
        }
        else {
          if (minInd.value >= 0) {
            value1 = 0.0;
            value2 = scale*minInd.value;
          }
          else {
            value2 = 0.0;
            value1 = scale*minInd.value;
          }
        }
      }
      else {
        if (isStacked()) {
          value1 = lastPosValue;
          value2 = value1 + scale*(maxInd.value - minInd.value);
        }
        else {
          value1 = scale*minInd.value;
          value2 = scale*maxInd.value;
        }
      }

      if (! isHorizontal()) {
        if (isStacked())
          brect = CQChartsGeom::BBox(bx, value1, bx + 1.0, value2);
        else
          brect = CQChartsGeom::BBox(bx1, value1, bx1 + bw1, value2);
      }
      else {
        if (isStacked())
          brect = CQChartsGeom::BBox(value1, bx, value2, bx + 1.0);
        else
          brect = CQChartsGeom::BBox(value1, bx1, value2, bx1 + bw1);
      }

      CQChartsBarChartObj *barObj = nullptr;

      if (ns > 1)
        barObj = new CQChartsBarChartObj(this, brect, i, nvs, j, nv, 0, 1, &ivalue, minInd.ind);
      else
        barObj = new CQChartsBarChartObj(this, brect, 0, 1, j, nv, i, nvs, &ivalue, minInd.ind);

      if (color)
        barObj->setColor(*color);

      addPlotObject(barObj);

      //---

      if (! isRangeBar()) {
        if (minInd.value >= 0) {
          lastPosValue = lastPosValue + scale*minInd.value;
        }
        else {
          lastNegValue = lastNegValue + scale*minInd.value;
        }
      }
      else {
        lastPosValue = scale*maxInd.value;
      }

      bx1 += bw1;
    }

    bx += 1.0;
  }

  //----

  resetKeyItems();

  //----

  return true;
}

QString
CQChartsBarChartPlot::
valueStr(double v) const
{
  CQChartsAxis *yAxis = (! isHorizontal() ? yAxis_ : xAxis_);

  return yAxis->valueStr(v);
}

void
CQChartsBarChartPlot::
addKeyItems(CQChartsPlotKey *key)
{
  QAbstractItemModel *model = this->model().data();

  if (! model)
    return;

  //---

  int row = key->maxRow();

  auto addKeyRow = [&](int i, int n, const QString &name, const QColor &c=QColor()) {
    CQChartsBarKeyColor *keyColor = new CQChartsBarKeyColor(this, i, n);
    CQChartsBarKeyText  *keyText  = new CQChartsBarKeyText (this, i, name);

    key->addItem(keyColor, row, 0);
    key->addItem(keyText , row, 1);

    if (c.isValid())
      keyColor->setColor(c);

    ++row;
  };

  //---

  int ns = (! isRangeBar() ? numValueColumns() : 1);

  if (ns > 1) {
    if (isColorBySet()) {
      int nv = numValueSets();

      for (int i = 0; i < nv; ++i) {
        const CQChartsBarChartValueSet &valueSet = this->valueSet(i);

        addKeyRow(i, nv, valueSet.name());
      }
    }
    else {
      const CQChartsBarChartValueSet &valueSet = this->valueSet(0);

      for (int i = 0; i < valueSet.numValues(); ++i) {
        const CQChartsBarChartValue &value = valueSet.value(i);

        addKeyRow(i, ns, value.valueName());
      }
    }
  }
  else {
    if (! isRowGrouping()) {
      int nv = numValueSets();

      for (int i = 0; i < nv; ++i) {
        const CQChartsBarChartValueSet &valueSet = this->valueSet(i);

        QColor c;

        if (valueSet.numValues() == 1) {
          const CQChartsBarChartValue &ivalue = valueSet.value(0);

          const CQChartsBarChartValue::ValueInds &valueInds = ivalue.valueInds();
          assert(! valueInds.empty());

          const QModelIndex &ind = valueInds[0].ind;

          OptColor color;

          if (colorColumn().isValid() && colorSetColor("color", ind.row(), color))
            c = (*color).interpColor(this, 0, 1);
        }

        addKeyRow(i, nv, valueSet.name(), c);
      }
    }
    else {
      QString title = this->titleStr();

      if (! title.length()) {
        bool ok;

        QString yname = modelHeaderString(valueColumn(), ok);

        title = yname;
      }

      addKeyRow(0, 1, title);
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

//---

bool
CQChartsBarChartPlot::
isSetHidden(int i) const
{
  int ns = (! isRangeBar() ? numValueColumns() : 1);

  if (ns > 1) {
    // if color by set then key hides set
    if (isColorBySet()) {
      if (CQChartsPlot::isSetHidden(i))
        return true;
    }
    // if not color by set then set also hidden if all values are hidden
    else {
      int nv = numSetValues();

      for (int i = 0; i < nv; ++i) {
        if (! isValueHidden(i))
          return false;
      }

      return true;
    }
  }
  else {
    if (CQChartsPlot::isSetHidden(i))
      return true;
  }

  return false;
}

bool
CQChartsBarChartPlot::
isValueHidden(int i) const
{
  int ns = (! isRangeBar() ? numValueColumns() : 1);

  if (ns > 1) {
    // if not color by set then key hides set values
    if (! isColorBySet())
      return CQChartsPlot::isSetHidden(i);

    return false;
  }
  else
    return false;
}

//---

bool
CQChartsBarChartPlot::
probe(ProbeData &probeData) const
{
  if (! dataRange_.isSet())
    return false;

  if (! isHorizontal()) {
    probeData.direction = ProbeData::Direction::VERTICAL;

    if (probeData.x < dataRange_.xmin() + 0.5)
      probeData.x = dataRange_.xmin() + 0.5;

    if (probeData.x > dataRange_.xmax() - 0.5)
      probeData.x = dataRange_.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
    probeData.direction = ProbeData::Direction::HORIZONTAL;

    if (probeData.y < dataRange_.ymin() + 0.5)
      probeData.y = dataRange_.ymin() + 0.5;

    if (probeData.y > dataRange_.ymax() - 0.5)
      probeData.y = dataRange_.ymax() - 0.5;

    probeData.y = std::round(probeData.y);
  }

  return true;
}

//------

bool
CQChartsBarChartPlot::
addMenuItems(QMenu *menu)
{
  QAction *stackedAction    = new QAction("Stacked"   , menu);
  QAction *percentAction    = new QAction("Percent"   , menu);
  QAction *rangeAction      = new QAction("Range"     , menu);
  QAction *horizontalAction = new QAction("Horizontal", menu);

  stackedAction->setCheckable(true);
  stackedAction->setChecked(isStacked());

  percentAction->setCheckable(true);
  percentAction->setChecked(isPercent());

  rangeAction->setCheckable(true);
  rangeAction->setChecked(isRangeBar());

  horizontalAction->setCheckable(true);
  horizontalAction->setChecked(isHorizontal());

  connect(stackedAction   , SIGNAL(triggered(bool)), this, SLOT(setStacked(bool)));
  connect(percentAction   , SIGNAL(triggered(bool)), this, SLOT(setPercent(bool)));
  connect(rangeAction     , SIGNAL(triggered(bool)), this, SLOT(setRangeBar(bool)));
  connect(horizontalAction, SIGNAL(triggered(bool)), this, SLOT(setHorizontal(bool)));

  menu->addSeparator();

  menu->addAction(stackedAction);
  menu->addAction(percentAction);
  menu->addAction(rangeAction);
  menu->addAction(horizontalAction);

  return true;
}

//------

void
CQChartsBarChartPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

CQChartsBarChartObj::
CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CQChartsGeom::BBox &rect,
                    int iset, int nset, int ival, int nval, int isval, int nsval,
                    const CQChartsBarChartValue *value, const QModelIndex &ind) :
 CQChartsPlotObj(plot, rect), plot_(plot), iset_(iset), nset_(nset), ival_(ival), nval_(nval),
 isval_(isval), nsval_(nsval), value_(value), ind_(ind)
{
  assert(iset  >= 0 && iset  < nset);
  assert(ival  >= 0 && ival  < nval);
  assert(isval >= 0 && isval < nsval);
}

QString
CQChartsBarChartObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(ind_);

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  //---

  const CQChartsBarChartValueSet &valueSet = plot_->valueSet(ival_);

  const QString &setName   = valueSet.name();
  QString        valueName = value_->getNameValue("Name");

  CQChartsBarChartValue::ValueInd minInd, maxInd;

  value_->calcRange(minInd, maxInd);

  QString valueStr;

  if (! plot_->isRangeBar()) {
    valueStr = plot_->valueStr(minInd.value);
  }
  else {
    QString minValueStr = plot_->valueStr(minInd.value);
    QString maxValueStr = plot_->valueStr(maxInd.value);

    valueStr = QString("%1-%2").arg(minValueStr).arg(maxValueStr);
  }

  if (valueName.length())
    return QString("%1:%2=%3").arg(setName).arg(valueName).arg(valueStr);
  else
    return QString("%1:%2").arg(setName).arg(valueStr);
}

QString
CQChartsBarChartObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  if (value_->groupName().length())
    tableTip.addTableRow("Group", tableTip.escapeText(value_->groupName()));

  if (value_->valueName().length())
    tableTip.addTableRow("Name", tableTip.escapeText(value_->valueName()));

  CQChartsBarChartValue::ValueInd minInd, maxInd;

  value_->calcRange(minInd, maxInd);

  QString valueStr;

  if (! plot_->isRangeBar()) {
    valueStr = plot_->valueStr(minInd.value);
  }
  else {
    QString minValueStr = plot_->valueStr(minInd.value);
    QString maxValueStr = plot_->valueStr(maxInd.value);

    valueStr = QString("%1-%2").arg(minValueStr).arg(maxValueStr);
  }

  tableTip.addTableRow("Value", valueStr);

  for (const auto &nameValue : value_->nameValues()) {
    if (nameValue.second.length())
      tableTip.addTableRow(nameValue.first, tableTip.escapeText(nameValue.second));
  }

  return tableTip.str();
}

CQChartsGeom::BBox
CQChartsBarChartObj::
dataLabelRect() const
{
  if (! plot_->dataLabel().isVisible())
    return CQChartsGeom::BBox();

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(rect(), prect);

  QRectF qrect = CQChartsUtil::toQRect(prect);

  QString label = value_->getNameValue("Label");

  if (! plot_->labelColumn().isValid()) {
    const CQChartsBarChartValue::ValueInds &valueInds = value_->valueInds();
    assert(! valueInds.empty());

    double value = valueInds[0].value;

    label = plot_->valueStr(value);
  }

  return plot_->dataLabel().calcRect(qrect, label);
}

void
CQChartsBarChartObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->categoryColumn());
  addColumnSelectIndex(inds, plot_->valueColumnAt(iset_));
  addColumnSelectIndex(inds, plot_->nameColumn());
  addColumnSelectIndex(inds, plot_->labelColumn());
  addColumnSelectIndex(inds, plot_->colorColumn());
}

void
CQChartsBarChartObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

#if 0
bool
CQChartsBarChartObj::
isHidden() const
{
  if (nset_ > 1) {
    return plot_->isSetHidden(iset_);
  }
  else
    return plot_->isSetHidden(ival_);
  return true;
}
#endif

void
CQChartsBarChartObj::
draw(QPainter *painter, const CQChartsPlot::Layer &layer)
{
  static double minBarSize = 1.0/64.0;

  //if (isHidden())
  //  return;

  //---

  painter->save();

  //---

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(rect(), prect);

  QRectF qrect = CQChartsUtil::toQRect(prect);

  //---

  static double minWidth = 4.0;

  if (layer == CQChartsPlot::Layer::MID) {
    if (! plot_->isHorizontal()) {
      double m = plot_->lengthPixelWidth(plot_->margin());

      double w1 = prect.getWidth() - 2*m;

      if (w1 < minWidth)
        m = (prect.getWidth() - minWidth)/2.0;

      prect.setXMin(prect.getXMin() + m);
      prect.setXMax(prect.getXMax() - m);

      qrect = CQChartsUtil::toQRect(prect);
    }
    else {
      double m = plot_->lengthPixelHeight(plot_->margin());

      double h1 = prect.getHeight() - 2*m;

      if (h1 < minWidth)
        m = (prect.getHeight() - minWidth)/2.0;

      prect.setYMin(prect.getYMin() + m);
      prect.setYMax(prect.getYMax() - m);

      qrect = CQChartsUtil::toQRect(prect);
    }

    //---

    // calc pen (stroke)
    QPen pen;

    if (plot_->isBorder()) {
      QColor bc = plot_->interpBorderColor(0, 1);

      bc.setAlphaF(plot_->borderAlpha());

      double bw = plot_->lengthPixelWidth(plot_->borderWidth());

      pen.setColor (bc);
      pen.setWidthF(bw);

      CQChartsUtil::penSetLineDash(pen, plot_->borderDash());
    }
    else {
      pen.setStyle(Qt::NoPen);
    }

    // calc brush (fill)
    QBrush barBrush;

    if (plot_->isBarFill()) {
      QColor barColor;

      if (nset_ > 1) {
        if (plot_->isColorBySet())
          barColor = plot_->interpBarColor(ival_, nval_);
        else
          barColor = plot_->interpBarColor(iset_, nset_);
      }
      else {
        if (! color_) {
          if (nsval_ > 1) {
            QColor barColor1 = plot_->interpBarColor(ival_    , nval_ + 1);
            QColor barColor2 = plot_->interpBarColor(ival_ + 1, nval_ + 1);

            double f = (1.0*isval_)/nsval_;

            barColor = CQChartsUtil::blendColors(barColor1, barColor2, f);
          }
          else
            barColor = plot_->interpBarColor(ival_, nval_);
        }
        else {
          barColor = (*color_).interpColor(plot_, 0.0);
        }
      }

      barColor.setAlphaF(plot_->barAlpha());

      barBrush.setColor(barColor);
      barBrush.setStyle(CQChartsFillPattern::toStyle(
       (CQChartsFillPattern::Type) plot_->barPattern()));
    }
    else {
      barBrush.setStyle(Qt::NoBrush);
    }

    plot_->updateObjPenBrushState(this, pen, barBrush);

    //---

    // draw rect
    if (qrect.width() > minBarSize && qrect.height() > minBarSize) {
      painter->setPen(pen);
      painter->setBrush(barBrush);

      double cxs = plot_->lengthPixelWidth (plot_->cornerSize());
      double cys = plot_->lengthPixelHeight(plot_->cornerSize());

      CQChartsRoundedPolygon::draw(painter, qrect, cxs, cys);
    }
    else {
      if (! plot_->isBorder())
        painter->setPen(barBrush.color());

      painter->drawLine(QPointF(qrect.left (), qrect.bottom()),
                        QPointF(qrect.right(), qrect.top   ()));
    }
  }
  else {
    QString minLabel = value_->getNameValue("Label");
    QString maxLabel = minLabel;

    CQChartsBarChartValue::ValueInd minInd, maxInd;

    value_->calcRange(minInd, maxInd);

    if (! plot_->labelColumn().isValid()) {
      minLabel = plot_->valueStr(minInd.value);
      maxLabel = plot_->valueStr(maxInd.value);
    }

    CQChartsDataLabel::Position pos = plot_->dataLabel().position();

    if (minLabel == maxLabel) {
      if (minInd.value < 0)
        pos = CQChartsDataLabel::flipPosition(pos);

      plot_->dataLabel().draw(painter, qrect, minLabel, pos);
    }
    else {
      if (plot_->dataLabel().isPositionOutside()) {
        CQChartsDataLabel::Position minPos = CQChartsDataLabel::Position::BOTTOM_OUTSIDE;
        CQChartsDataLabel::Position maxPos = CQChartsDataLabel::Position::TOP_OUTSIDE;

        plot_->dataLabel().draw(painter, qrect, minLabel, minPos);
        plot_->dataLabel().draw(painter, qrect, maxLabel, maxPos);
      }
      else {
        CQChartsDataLabel::Position minPos = CQChartsDataLabel::Position::BOTTOM_INSIDE;
        CQChartsDataLabel::Position maxPos = CQChartsDataLabel::Position::TOP_INSIDE;

        plot_->dataLabel().draw(painter, qrect, minLabel, minPos);
        plot_->dataLabel().draw(painter, qrect, maxLabel, maxPos);
      }
    }
  }

  painter->restore();
}

//------

CQChartsBarKeyColor::
CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n), plot_(plot)
{
}

bool
CQChartsBarKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  setSetHidden(! isSetHidden());

  plot_->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsBarKeyColor::
fillBrush() const
{
  QColor c;

  if (color_)
    c = (*color_).interpColor(plot_, 0.0);
  else
    c = plot_->interpBarColor(i_, n_);

  if (isSetHidden())
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}

bool
CQChartsBarKeyColor::
tipText(const CQChartsGeom::Point &, QString &tip) const
{
  int nv = 0;

  double posSum = 0.0, negSum = 0.0;

  int ns = (! plot_->isRangeBar() ? plot_->numValueColumns() : 1);

  if (ns > 1) {
    if (plot_->isColorBySet()) {
      const CQChartsBarChartValueSet &valueSet = plot_->valueSet(i_);

      valueSet.calcSums(posSum, negSum);

      nv = valueSet.numValues();
    }
    else {
      nv = plot_->numSetValues();

      for (int i = 0; i < plot_->numValueSets(); ++i) {
        const CQChartsBarChartValueSet &valueSet = plot_->valueSet(i);

        const CQChartsBarChartValue &ivalue = valueSet.value(i_);

        const CQChartsBarChartValue::ValueInds &valueInds = ivalue.valueInds();
        assert(! valueInds.empty());

        double value = valueInds[0].value;

        if (value >= 0) posSum += value;
        else            negSum += value;
      }
    }
  }
  else {
    const CQChartsBarChartValueSet &valueSet = plot_->valueSet(i_);

    valueSet.calcSums(posSum, negSum);

    nv = valueSet.numValues();
  }

  QString sumStr;

  if      (CQChartsUtil::isZero(negSum))
    sumStr = QString("%1").arg(posSum);
  else if (CQChartsUtil::isZero(posSum))
    sumStr = QString("%1").arg(negSum);
  else
    sumStr = QString("%1 -> %2").arg(negSum).arg(posSum);

  CQChartsTableTip tableTip;

  tableTip.addTableRow("Count", nv);
  tableTip.addTableRow("Total", sumStr);

  if (isSetHidden())
    tableTip.addTableRow("Hidden", "true");

  tip = tableTip.str();

  return true;
}

bool
CQChartsBarKeyColor::
isSetHidden() const
{
  return plot_->CQChartsPlot::isSetHidden(i_);
}

void
CQChartsBarKeyColor::
setSetHidden(bool b)
{
  plot_->CQChartsPlot::setSetHidden(i_, b);
}

//------

CQChartsBarKeyText::
CQChartsBarKeyText(CQChartsBarChartPlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsBarKeyText::
interpTextColor(int i, int n) const
{
  CQChartsBarChartPlot *plot = qobject_cast<CQChartsBarChartPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.5);

  return c;
}
