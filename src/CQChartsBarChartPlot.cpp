#include <CQChartsBarChartPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsDataLabel.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

CQChartsBarChartPlotType::
CQChartsBarChartPlotType()
{
}

void
CQChartsBarChartPlotType::
addParameters()
{
  startParameterGroup("Bar Chart");

  // values
  addColumnsParameter("value", "Value", "valueColumns").
    setRequired().setNumeric().setTip("Value column(s)");

  addColumnParameter("name" , "Name" , "nameColumn" ).
    setString().setBasic().setTip("Custom group name");
  addColumnParameter("label", "Label", "labelColumn").
    setString().setTip("Bar data label");

  // options
  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("Draw bars horizontal");

  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("NORMAL" , int(CQChartsBarChartPlot::PlotType::NORMAL )).
    addNameValue("STACKED", int(CQChartsBarChartPlot::PlotType::STACKED)).
    setTip("Plot type");

  addEnumParameter("valueType", "Value Type", "valueType").
   addNameValue("VALUE", int(CQChartsBarChartPlot::ValueType::VALUE)).
   addNameValue("RANGE", int(CQChartsBarChartPlot::ValueType::RANGE)).
   addNameValue("MIN"  , int(CQChartsBarChartPlot::ValueType::MIN  )).
   addNameValue("MAX"  , int(CQChartsBarChartPlot::ValueType::MAX  )).
   addNameValue("MEAN" , int(CQChartsBarChartPlot::ValueType::MEAN )).
   setTip("Bar value type");

  addBoolParameter("percent" , "Percent"  , "percent" ).setTip("Show value is percentage");
  addBoolParameter("dotLines", "Dot Lines", "dotLines").setTip("Draw bars as lines with dot");

  addBoolParameter("colorBySet", "Color by Set", "colorBySet").
    setTip("Color by value set");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsBarChartPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Bar Chart Plot").
    h3("Summary").
     p("Draws bars with heights from a set of values.").
    h3("Columns").
     p("The bar heights are taken from the values in the " + B("Value") + " column.").
     p("An optional name can be supplied in the " + B("Name") + " column to specify the label "
       "to use on the axis below the bar.").
     p("An optional label can be drawn with the bar to show extra values using "
       "the " + B("Label") + " column.").
     p("The color of the bar can be customized using the " + B("Color") + " column.").
     p("A custom id can specified using the " + B("Id") + " column.").
    h3("Grouping").
     p("Bars can be grouped using standard grouping controls so that related values "
       "can be placed next to each other.").
     p("Enabling the " + B("Row Grouping") + " option groups bars by column header name "
       "instead of the normal " + B("Group") + " columns.").
    h3("Options").
     p("Selecting the " + B("Stacked") + " Plot Type places grouped bars on top of each other "
      "instead of the " + B("Normal") + " side by side placement.").
     p("Selecting the " + B("Range") + " Value Type draws a bar for the range (min/max) of "
       "the grouped values, selecting the " + B("Min") + " Value Type draws a bar to the "
       "minimum of the grouped values and selecting the " + B("Max") + " Value Type draws a "
       "bar to the maximum of the grouped values.").
     p("Enabling the " + B("Percent") + " option rescales the values to a percentage of the "
       "maximum and minimum of the values.").
     p("Enabling the " + B("Horizontal") + " option draws the bars horizontally instead "
       "of vertically.").
     p("Enabling the " + B("Dot Lines") + " option draws the bars as a single line with a "
       "circle symbol at the end.").
     p("Enabling the " + B("Color by Set") + " option colors bars in the same group the same "
      "color instead using different colors for each bar in the group.").
    h3("Limitations").
     p("The plot does not support logarithmic x scales").
    h3("Example").
     p(IMG("images/barchart.png"));
}

CQChartsPlot *
CQChartsBarChartPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsBarChartPlot(view, model);
}

//------

CQChartsBarChartPlot::
CQChartsBarChartPlot(View *view, const ModelP &model) :
 CQChartsBarPlot(view, view->charts()->plotType("barchart"), model),
 CQChartsObjDotPointData<CQChartsBarChartPlot>(this)
{
  NoUpdate noUpdate(this);

  //---

  setDotSymbolType(CQChartsSymbol::Type::CIRCLE);
  setDotSymbolSize(Length("7px"));
}

CQChartsBarChartPlot::
~CQChartsBarChartPlot()
{
}

//---

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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addBaseProperties();

  addBoxProperties();

  // columns
  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "labelColumn", "label", "Label column");

  // options
  addProp("options", "plotType" , "plotType" , "Plot type");
  addProp("options", "valueType", "valueType", "Value type");
  addProp("options", "percent"  , ""         , "Use percentage value");

  // dot lines
  addProp("dotLines",        "dotLines"    , "visible", "Draw bars as lines with dot");
  addProp("dotLines/stroke", "dotLineWidth", "width"  , "Dot line width");

  addSymbolProperties("dotLines/symbol", "dot", "Dot line");

  // coloring
  addProp("coloring", "colorBySet", "", "Color by value set");

  //---

  addGroupingProperties();
}

//---

void
CQChartsBarChartPlot::
setPlotType(PlotType type)
{
  CQChartsUtil::testAndSet(plotType_, type, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBarChartPlot::
setNormal(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setStacked(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::STACKED, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setPercent(bool b)
{
  CQChartsUtil::testAndSet(percent_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBarChartPlot::
setValueType(ValueType type)
{
  CQChartsUtil::testAndSet(valueType_, type, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setValueValue(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::VALUE, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::VALUE, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setValueRange(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::RANGE, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::VALUE, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setValueMin(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MIN, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::VALUE, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setValueMax(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MAX, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::VALUE, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setValueMean(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MEAN, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::VALUE, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBarChartPlot::
setColorBySet(bool b)
{
  CQChartsUtil::testAndSet(colorBySet_, b, [&]() { resetSetHidden(); updateRangeAndObjs(); });
}

//---

void
CQChartsBarChartPlot::
setDotLines(bool b)
{
  CQChartsUtil::testAndSet(dotLineData_.enabled, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setDotLineWidth(const Length &l)
{
  CQChartsUtil::testAndSet(dotLineData_.width, l, [&]() { drawObjs(); } );
}

//---

CQChartsGeom::Range
CQChartsBarChartPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsBarChartPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  th->clearErrors();

  //---

  Range dataRange;

  //---

  // check columns
  bool columnsValid = true;

  // value columns required
  // name, label, group, color columns optional

  if (! checkColumns(valueColumns(), "Values", /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(labelColumn(), "Label")) columnsValid = false;
  if (! checkColumn(groupColumn(), "Group")) columnsValid = false;
  if (! checkColumn(colorColumn(), "Color")) columnsValid = false;

  if (! columnsValid)
    return dataRange;

  //---

  auto updateRange = [&](double x, double y) {
    if (! isHorizontal())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  //---

  updateRange(-0.5, 0);

  //---

  initGroupValueSet();

  //---

  // init grouping
  //   non-range use group columns for grouping
  if (isValueValue())
    initGroupData(valueColumns(), nameColumn());
  else
    initGroupData(CQChartsColumns(), nameColumn());

  //---

  // process model data
  class BarChartVisitor : public ModelVisitor {
   public:
    BarChartVisitor(const CQChartsBarChartPlot *plot, Range &dataRange) :
     plot_(plot), dataRange_(dataRange) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRow(data, dataRange_);

      return State::OK;
    }

   private:
    const CQChartsBarChartPlot* plot_ { nullptr };
    Range&                      dataRange_;
  };

  BarChartVisitor barChartVisitor(this, dataRange);

  visitModel(barChartVisitor);

  //---

  int ns = (isValueValue() ? valueColumns().count() : 1);
  int nv = numValueSets();

  int ng = numGroups();

  int numVisible = 0;

  if      (ns > 1 && ! isRowGrouping()) {
    for (int is = 0; is < ns; ++is)
      numVisible += ! isSetHidden(is);
  }
  else if (ng > 1) {
    for (int ig = 0; ig < ng; ++ig)
      numVisible += ! isSetHidden(ig);
  }
  else if (ns == 1) {
    if      (nv > 1) {
      for (int iv = 0; iv < nv; ++iv)
        numVisible += ! isSetHidden(iv);
    }
    else if (nv == 1) {
      const auto &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      for (int ivs = 0; ivs < nvs; ++ivs)
        numVisible += ! isValueHidden(ivs);
    }
  }

  //---

  if (ng > 0) {
    double ymin = (! isHorizontal() ? dataRange.ymin() : dataRange.xmin());

    updateRange(numVisible - 0.5, ymin);

    if (nv == 0) {
      double xmin = (! isHorizontal() ? dataRange.xmin() : dataRange.ymin());

      updateRange(xmin, 1.0);
    }
  }
  else {
    updateRange(0.5, 1.0);
  }

  dataRange.makeNonZero();

  //---

  initRangeAxes();

  //---

  return dataRange;
}

void
CQChartsBarChartPlot::
initRangeAxes() const
{
  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  th->initRangeAxesI();
}

void
CQChartsBarChartPlot::
initRangeAxesI()
{
  int ns = (isValueValue() ? valueColumns().count() : 1);
//int nv = numValueSets();

  int ng = numGroups();

  // needed ?
  if (! isHorizontal()) {
    setXValueColumn(groupColumn().isValid() ? groupColumn() : nameColumn());
    setYValueColumn(valueColumns().column());
  }
  else {
    setYValueColumn(groupColumn().isValid() ? groupColumn() : nameColumn());
    setXValueColumn(valueColumns().column());
  }

  //---

  // set axis column and labels
  auto *xAxis = mappedXAxis();
  auto *yAxis = mappedYAxis();

  xAxis->setColumn(groupColumn().isValid() ? groupColumn() : nameColumn());

  QString xname;

  if (ns > 1) {
    if (! isRowGrouping()) {
      bool ok;

      xname = modelHHeaderString(groupColumn().isValid() ? groupColumn() : nameColumn(), ok);
    }
  }
  else {
    if (ng > 1) {
      bool ok;

      xname = modelHHeaderString(groupColumn().isValid() ? groupColumn() : nameColumn(), ok);
    }
    else {
    }
  }

  if (xname != "")
    xAxis->setDefLabel(xname);

  //---

  yAxis->setColumn(valueColumns().column());

  QString yname;

  if (valueColumns().count() <= 1) {
    bool ok;

    yname = modelHHeaderString(valueColumns().column(), ok);
  }

  yAxis->setDefLabel(yname);
}

void
CQChartsBarChartPlot::
addRow(const ModelVisitor::VisitData &data, Range &dataRange) const
{
  // add value for each column (non-range)
  if (isValueValue()) {
    for (const auto &column : valueColumns()) {
      CQChartsColumns columns { column };

      addRowColumn(data, columns, dataRange);
    }
  }
  // add all values for columns (range)
  else {
    addRowColumn(data, this->valueColumns(), dataRange);
  }
}

void
CQChartsBarChartPlot::
addRowColumn(const ModelVisitor::VisitData &data, const CQChartsColumns &valueColumns,
             Range &dataRange) const
{
  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  if (isColorKey() && colorColumn().isValid()) {
    ModelIndex colorModelInd(th, data.row, colorColumn(), data.parent);

    bool ok;

    QVariant colorValue = modelValue(colorModelInd, ok);

    bool hidden = (ok && CQChartsVariant::cmp(hideValue(), colorValue) == 0);

    if (hidden)
      return;
  }

  //---

  auto updateRange = [&](double x, double y) {
    if (! isHorizontal())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  //---

  ModelIndex ind;

  if (isValueValue()) {
    assert(valueColumns.count() > 0);

    const auto &valueColumn = valueColumns.column();

    ind = ModelIndex(th, data.row, valueColumn, data.parent);
  }
  else {
    ind = ModelIndex(th, data.row, CQChartsColumn(), data.parent);
  }

  //---

  // get optional group for value
  int groupInd = rowGroupInd(ind);

  // get group name
  QString groupName = groupIndName(groupInd);

  //---

  QString categoryName;

  //---

  // get group string (if defined) and update group name
  QString group;

  if (groupColumn().isValid()) {
    ModelIndex groupModelInd(th, data.row, groupColumn(), data.parent);

    bool ok1;

    group = modelHierString(groupModelInd, ok1);

    categoryName = group;
  }

  //---

  // get name string (if defined) and update group name
  QString name;

  if (nameColumn().isValid()) {
    ModelIndex nameInd(th, data.row, nameColumn(), data.parent);

    bool ok2;

    QVariant var = modelValue(nameInd, ok2);

    // convert real value to string for real value name (e.g. date)
    double r;

    if (CQChartsVariant::toReal(var, r))
      name = columnStr(nameColumn(), r);
    else
      name = modelString(nameInd, ok2);

    if (! categoryName.length())
      categoryName = name;
  }

  //---

  // get optional data label string
  QString labelStr;

  if (labelColumn().isValid()) {
    ModelIndex labelInd(th, data.row, labelColumn(), data.parent);

    bool ok3;

    labelStr = modelString(labelInd, ok3);
  }

  //---

  // get optional color string
  QString colorStr;

  if (colorColumn().isValid()) {
    ModelIndex colorModelInd(th, data.row, colorColumn(), data.parent);

    bool ok4;

    colorStr = modelString(colorModelInd, ok4);
  }

  //---

  // get value set for group
  CQChartsBarChartValueSet *valueSet =
    const_cast<CQChartsBarChartValueSet *>(groupValueSet(groupInd));

  //---

  using ValueInd  = CQChartsBarChartValue::ValueInd;
  using ValueInds = CQChartsBarChartValue::ValueInds;

  ValueInds valueInds;

  // add values for columns (1 column normally, all columns when grouped)
  for (const auto &valueColumn : valueColumns) {
    ModelIndex valueModelInd(th, data.row, valueColumn, data.parent);

    double r;

    bool ok2 = modelMappedReal(valueModelInd, r, isLogY(), data.row);

    if (! ok2) {
      th->addDataError(valueModelInd, "Invalid value");

      if (isSkipBad())
        continue;

      r = data.row;
    }

    if (CMathUtil::isNaN(r))
      continue;

    //---

    // get associated model index
    QModelIndex valInd  = modelIndex(valueModelInd);
    QModelIndex valInd1 = normalizeIndex(valInd);

    // add value and index
    ValueInd valueInd;

    valueInd.value = r;
    valueInd.ind   = valInd1;
    valueInd.vrow  = data.vrow;

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

  int ns = (isValueValue() ? this->valueColumns().count() : 1);

  if (ns > 1) {
    // set value data group name and value name

    if (groupName.length())
      valueData.setGroupName(groupName);

    QString valueName;

    // not row grouping so value name is column header
    if (! isGroupHeaders()) {
      assert(valueColumns.count() > 0);

      const auto &valueColumn = valueColumns.column();

      bool ok;

      valueName = modelHHeaderString(valueColumn, ok);
    }
    // row grouping so value name is group/name column name
    else {
      valueName = categoryName;
    }

    // save value name
    valueData.setValueName(valueName);
  }
  else {
    int ng = numGroups();

    if (ng > 1) {
      // if path grouping (hierarchical) then value name is group name
      if (isGroupPathType()) {
        if (groupName.length())
          valueData.setGroupName(groupName);
      }

      // save other name values for tip
      if (group   .length()) valueData.setNameValue("Group", group);
      if (name    .length()) valueData.setNameValue("Name" , name);
      if (labelStr.length()) valueData.setNameValue("Label", labelStr);
      if (colorStr.length()) valueData.setNameValue("Color", colorStr);
    }
    else {
      valueData.setValueName(name);
    }
  }

  // add value(s) to value set
  valueSet->addValue(valueData);

  //---

  // calc pos/neg sums
  double posSum = 0.0, negSum = 0.0;

  if (isPercent() || isStacked())
    valueSet->calcSums(posSum, negSum);

  // scale for percent
  double scale = 1.0;

  if (isPercent()) {
    double total = posSum - negSum;

    scale = (total > 0.0 ? 100.0/total : 1.0);
  }

  //---

  // update range for scale and sums
  if (isStacked()) {
    updateRange(0, scale*posSum);
    updateRange(0, scale*negSum);
  }
  else {
    for (const auto &valueInd : valueInds)
      updateRange(0, scale*valueInd.value);
  }
}

void
CQChartsBarChartPlot::
initGroupValueSet() const
{
  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  th->valueData_.clear();
}

const CQChartsBarChartValueSet *
CQChartsBarChartPlot::
groupValueSet(int groupInd) const
{
  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  return th->groupValueSetI(groupInd);
}

CQChartsBarChartValueSet *
CQChartsBarChartPlot::
groupValueSetI(int groupInd)
{
  auto p = valueData_.valueGroupInd.find(groupInd);

  if (p != valueData_.valueGroupInd.end()) {
    int ind = (*p).second;

    assert(ind >= 0 && ind < int(valueData_.valueSets.size()));

    return &valueData_.valueSets[ind];
  }

  //---

  int ind = numValueSets();

  QString name = groupIndName(groupInd);

  valueData_.valueSets.emplace_back(name, ind);

  valueData_.valueGroupInd[groupInd] = ind;

  const auto &valueSet = &valueData_.valueSets.back();

  valueSet->setGroupInd(groupInd);

  return valueSet;
}

//------

CQChartsGeom::BBox
CQChartsBarChartPlot::
calcAnnotationBBox() const
{
  CQPerfTrace trace("CQChartsBarChartPlot::calcAnnotationBBox");

  BBox bbox;

  auto position = dataLabel()->position();

  if (position != CQChartsDataLabel::TOP_OUTSIDE && position != CQChartsDataLabel::BOTTOM_OUTSIDE)
    return bbox;

  if (dataLabel()->isVisible()) {
    for (const auto &plotObj : plotObjs_) {
      auto *barObj = dynamic_cast<CQChartsBarChartObj *>(plotObj);

      if (barObj)
        bbox += barObj->dataLabelRect();
    }
  }

  return bbox;
}

//------

bool
CQChartsBarChartPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsBarChartPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  // init value sets
//initValueSets();

  //---

  initObjAxes();

  //---

  int ns = (isValueValue() ? this->valueColumns().count() : 1);
  int nv = numValueSets();

  //---

  barWidth_ = 1.0;

  // start at px1 - bar width
  double bx = -0.5;

  for (int iv = 0; iv < nv; ++iv) {
    if (isSetHidden(iv))
      continue;

    //---

    const auto &valueSet = this->valueSet(iv);

    //const auto &setName = valueSet.name;

    //---

    int nvs = valueSet.numValues();

    // get num visible in each set
    int numVisible1 = 0;

    if      (ns > 1) {
      for (int ivs = 0; ivs < nvs; ++ivs)
        numVisible1 += ! isValueHidden(ivs);
    }
    else if (nv > 1)
      numVisible1 = nvs;
    else
      numVisible1 = 1;

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

    if (! isStacked())
      bw1 = 1.0/numVisible1;

    double lastPosValue = 0.0, lastNegValue = 0.0;

    for (int ivs = 0; ivs < nvs; ++ivs) {
      if (isValueHidden(ivs))
        continue;

      //---

      const auto &ivalue = valueSet.value(ivs);

      CQChartsBarChartValue::ValueInd minInd, maxInd;

      ivalue.calcRange(minInd, maxInd);

      QModelIndex parent; // TODO

      Color color;

      (void) colorColumnColor(minInd.vrow, parent, color);

      //---

      // create bar rect
      BBox brect;

      double value1 { 0.0 }, value2 { 0.0 };

      if      (isValueValue()) {
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
      else if (isValueRange()) {
        if (isStacked()) {
          value1 = lastPosValue;
          value2 = value1 + scale*(maxInd.value - minInd.value);
        }
        else {
          value1 = scale*minInd.value;
          value2 = scale*maxInd.value;
        }
      }
      else if (isValueMin()) {
        if (isStacked()) {
          value1 = lastPosValue;
          value2 = value1 + scale*minInd.value;
        }
        else {
          value1 = 0.0;
          value2 = scale*minInd.value;
        }
      }
      else if (isValueMax()) {
        if (isStacked()) {
          value1 = lastPosValue;
          value2 = value1 + scale*maxInd.value;
        }
        else {
          value1 = 0.0;
          value2 = scale*maxInd.value;
        }
      }

      if (value1 == value2)
        continue;

      if (isStacked())
        brect = CQChartsGeom::makeDirBBox(isHorizontal(), bx, value1, bx + 1.0, value2);
      else
        brect = CQChartsGeom::makeDirBBox(isHorizontal(), bx1, value1, bx1 + bw1, value2);

      if (! isHorizontal())
        barWidth_ = std::min(barWidth_, brect.getWidth());
      else
        barWidth_ = std::min(barWidth_, brect.getHeight());

      CQChartsBarChartObj *barObj = nullptr;

      if (ns > 1) {
        // multiple sets:
        //  . set per value column
        //  . group per group column unique value
        barObj = createBarObj(brect, ColorInd(ivs, nvs), ColorInd(iv, nv), ColorInd(), minInd.ind);
      }
      else {
        // single set:
        //  . group per group column unique value
        //  . value per grouped values
        barObj = createBarObj(brect, ColorInd(), ColorInd(iv, nv), ColorInd(ivs, nvs), minInd.ind);
      }

      if (color.isValid())
        barObj->setColor(color);

      objs.push_back(barObj);

      //---

      if      (isValueValue()) {
        if (minInd.value >= 0) {
          lastPosValue = lastPosValue + scale*minInd.value;
        }
        else {
          lastNegValue = lastNegValue + scale*minInd.value;
        }
      }
      else if (isValueRange()) {
        lastPosValue = scale*(maxInd.value - minInd.value);
      }
      else if (isValueMin()) {
        lastPosValue = scale*minInd.value;
      }
      else if (isValueMax()) {
        lastPosValue = scale*maxInd.value;
      }

      bx1 += bw1;
    }

    bx += 1.0;
  }

  //----

  return true;
}

void
CQChartsBarChartPlot::
initObjAxes() const
{
  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  th->initObjAxesI();
}

void
CQChartsBarChartPlot::
initObjAxesI()
{
  // init axes
  auto *xAxis = mappedXAxis();
  auto *yAxis = mappedYAxis();

  xAxis->clearTickLabels();
  yAxis->clearTickLabels();

  xAxis->setValueType     (CQChartsAxisValueType::Type::INTEGER, /*notify*/false);
  xAxis->setGridMid       (true);
//xAxis->setMajorIncrement(1);
  xAxis->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR);

  yAxis->setValueType     (isLogY() ? CQChartsAxisValueType::Type::LOG :
                                           CQChartsAxisValueType::Type::REAL, /*notify*/false);
  yAxis->setGridMid       (false);
//yAxis->setMajorIncrement(0);
  yAxis->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR_AND_MINOR);

  //---

  int ns = (isValueValue() ? this->valueColumns().count() : 1);
  int nv = numValueSets();

  //---

  // set x axis labels
  if (ns > 1) {
    int numVisible = 0;

    for (int iv = 0; iv < nv; ++iv) {
      if (isSetHidden(iv))
        continue;

      const auto &valueSet = this->valueSet(iv);

      xAxis->setTickLabel(numVisible, valueSet.name());

      ++numVisible;
    }
  }
  else {
    int numVisible = 0;

    if      (nv > 1) {
      for (int iv = 0; iv < nv; ++iv) {
        if (isSetHidden(iv))
          continue;

        const auto &valueSet = this->valueSet(iv);

        xAxis->setTickLabel(numVisible, valueSet.name());

        ++numVisible;
      }
    }
    else if (nv == 1) {
      std::set<int> positions;

      xAxis->getTickLabelsPositions(positions);

      const auto &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      for (int ivs = 0; ivs < nvs; ++ivs) {
        if (isValueHidden(ivs))
          continue;

        if (positions.find(numVisible) != positions.end()) {
          const auto &value = valueSet.value(ivs);

          xAxis->setTickLabel(numVisible, value.valueName());
        }

        ++numVisible;
      }
    }
  }
}

QString
CQChartsBarChartPlot::
valueName() const
{
  return mappedYAxis()->label().string();
}

QString
CQChartsBarChartPlot::
valueStr(double v) const
{
  return mappedYAxis()->valueStr(v);
}

CQChartsAxis *
CQChartsBarChartPlot::
mappedXAxis() const
{
  return (! isHorizontal() ? xAxis() : yAxis());
}

CQChartsAxis *
CQChartsBarChartPlot::
mappedYAxis() const
{
  return (! isHorizontal() ? yAxis() : xAxis());
}

void
CQChartsBarChartPlot::
addKeyItems(CQChartsPlotKey *key)
{
  // start at next row (vertical) or next column (horizontal) from previous key
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyRow = [&](const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                       const QString &name, const QColor &c=QColor()) {
    auto *colorItem = new CQChartsBarKeyColor(this, name, is, ig, iv);
    auto *textItem  = new CQChartsBarKeyText (this, name, iv);

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addItem(colorItem);
    groupItem->addItem(textItem );

    if (c.isValid())
      colorItem->setColor(c);

    key->addItem(groupItem, row, col);

    key->nextRowCol(row, col);
  };

  //---

  int ns = (isValueValue() ? this->valueColumns().count() : 1);
  int nv = numValueSets();

  if (ns > 1) {
    // ivs is 'value set' index (group)
    // iv is 'value set' value index
    if (isColorBySet()) {
      for (int iv = 0; iv < nv; ++iv) {
        const auto &valueSet = this->valueSet(iv);

        QString name = valueSet.name();

        addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), name);
      }
    }
    else {
      const auto &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      for (int ivs = 0; ivs < nvs; ++ivs) {
        const auto &value = valueSet.value(ivs);

        QString name = value.valueName();

        addKeyRow(ColorInd(), ColorInd(), ColorInd(ivs, nvs), name);
      }
    }
  }
  else {
    if      (nv > 1) {
      if (isColorBySet()) {
        QString name = this->valueName();

        addKeyRow(ColorInd(), ColorInd(), ColorInd(), name);
      }
      else {
        for (int iv = 0; iv < nv; ++iv) {
          const auto &valueSet = this->valueSet(iv);

          QColor c;

          if (valueSet.numValues() == 1) {
            const auto &ivalue = valueSet.value(0);

            const auto &valueInds = ivalue.valueInds();
            assert(! valueInds.empty());

            const auto &ind0 = valueInds[0];

            QModelIndex parent; // TODO
            Color       color;

            if (colorColumnColor(ind0.vrow, parent, color))
              c = interpColor(color, ColorInd());
          }

          QString name = valueSet.name();

          addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), name, c);
        }
      }
    }
    else if (nv == 1) {
      const auto &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      for (int ivs = 0; ivs < nvs; ++ivs) {
        const auto &ivalue = valueSet.value(ivs);

        const auto &valueInds = ivalue.valueInds();
        assert(! valueInds.empty());

        const auto &ind0 = valueInds[0];

        QColor c;

        QModelIndex parent; // TODO
        Color       color;

        if (colorColumnColor(ind0.vrow, parent, color))
          c = interpColor(color, ColorInd());

        QString iname = ivalue.valueName();

        addKeyRow(ColorInd(), ColorInd(), ColorInd(ivs, nvs), iname, c);
      }
    }

#if 0
    if (isGroupHeaders()) {
      for (int iv = 0; iv < nv; ++iv) {
        const auto &valueSet = this->valueSet(iv);

        QColor c;

        if (valueSet.numValues() == 1) {
          const auto &ivalue = valueSet.value(0);

          const auto &valueInds = ivalue.valueInds();
          assert(! valueInds.empty());

          const auto &ind0 = valueInds[0];

          QModelIndex parent; // TODO
          Color       color;

          if (colorColumnColor(ind0.vrow, parent, color))
            c = interpColor(color, ColorInd());
        }

        QString name = valueSet.name();

        addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), name, c);
      }
    }
    else {
      QString title = this->titleStr();

      if (! title.length()) {
        bool ok;

        QString yname = modelHHeaderString(valueColumns().column(), ok);

        title = yname;
      }

      addKeyRow(ColorInd(), ColorInd(), ColorInd(), title);
    }
#endif
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

//---

bool
CQChartsBarChartPlot::
isSetHidden(int i) const
{
  int ns = (isValueValue() ? this->valueColumns().count() : 1);
  int nv = numValueSets();

  if      (ns > 1) {
    // if color by set then key hides set
    if (isColorBySet()) {
      if (CQChartsPlot::isSetHidden(i))
        return true;
    }
    // if not color by set then set also hidden if all values are hidden
    else {
      if (nv > 1) {
        for (int i = 0; i < nv; ++i) {
          if (! isValueHidden(i))
            return false;
        }

        return true;
      }
    }

    return false;
  }
  else if (nv > 1) {
    return CQChartsPlot::isSetHidden(i);
  }
  else {
    return false;
  }
}

bool
CQChartsBarChartPlot::
isValueHidden(int i) const
{
  int ns = (isValueValue() ? this->valueColumns().count() : 1);
  int nv = numValueSets();

  if      (ns > 1) {
    // if not color by set then key hides set values
    if (! isColorBySet())
      return CQChartsPlot::isSetHidden(i);

    return false;
  }
  else if (nv > 1) {
    return false;
  }
  else {
    return CQChartsPlot::isSetHidden(i);
  }
}

//------

bool
CQChartsBarChartPlot::
addMenuItems(QMenu *menu)
{
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name, bool isSet, const char *slot) {
    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) {
    return addMenuCheckedAction(menu, name, isSet, slot);
  };

  //---

  menu->addSeparator();

  (void) addCheckedAction("Horizontal", isHorizontal(), SLOT(setHorizontal(bool)));

  auto *typeMenu = new QMenu("Plot Type", menu);

  (void) addMenuCheckedAction(typeMenu, "Normal" , isNormal (), SLOT(setNormal (bool)));
  (void) addMenuCheckedAction(typeMenu, "Stacked", isStacked(), SLOT(setStacked(bool)));

  menu->addMenu(typeMenu);

  auto *valueMenu = new QMenu("Value Type", menu);

  (void) addMenuCheckedAction(valueMenu, "Value", isValueValue(), SLOT(setValueValue(bool)));
  (void) addMenuCheckedAction(valueMenu, "Range", isValueRange(), SLOT(setValueRange(bool)));
  (void) addMenuCheckedAction(valueMenu, "Min"  , isValueMin  (), SLOT(setValueMin  (bool)));
  (void) addMenuCheckedAction(valueMenu, "Max"  , isValueMax  (), SLOT(setValueMax  (bool)));

  menu->addMenu(valueMenu);

  (void) addCheckedAction("Percent"  , isPercent (), SLOT(setPercent(bool)));
  (void) addCheckedAction("Dot Lines", isDotLines(), SLOT(setDotLines(bool)));

  return true;
}

//------

double
CQChartsBarChartPlot::
getPanX(bool is_shift) const
{
  return windowToViewWidth(is_shift ? 2.0*barWidth_ : 1.0*barWidth_);
}

double
CQChartsBarChartPlot::
getPanY(bool is_shift) const
{
  return windowToViewHeight(is_shift ? 2.0*barWidth_ : 1.0*barWidth_);
}

//------

CQChartsBarChartObj *
CQChartsBarChartPlot::
createBarObj(const BBox &rect, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
             const QModelIndex &ind) const
{
  return new CQChartsBarChartObj(this, rect, is, ig, iv, ind);
}

//------

CQChartsBarChartObj::
CQChartsBarChartObj(const CQChartsBarChartPlot *plot, const BBox &rect, const ColorInd &is,
                    const ColorInd &ig, const ColorInd &iv, const QModelIndex &ind) :
 CQChartsPlotObj(const_cast<CQChartsBarChartPlot *>(plot), rect, is, ig, iv),
 plot_(plot)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(ind);
}

QString
CQChartsBarChartObj::
calcId() const
{
  return QString("%1:%2:%3:%4").arg(typeName()).arg(is_.i).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsBarChartObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  QString nameStr  = this->nameStr ();
  QString groupStr = this->groupStr();
  QString valueStr = this->valueStr();

  if (groupStr.length())
    tableTip.addTableRow("Group", groupStr);

  if (nameStr.length()) {
    QString headerStr("Name");

    if (plot_->nameColumn().isValid())
      headerStr = plot_->columnHeaderName(plot_->nameColumn(), /*tip*/true);

    tableTip.addTableRow(headerStr, nameStr);
  }

  if (valueStr.length()) {
    QString headerStr("Value");

    if (plot_->valueColumns().isValid())
      headerStr = plot_->columnsHeaderName(plot_->valueColumns(), /*tip*/true);

    tableTip.addTableRow(headerStr, valueStr);
  }

  const auto &value = this->value();

  for (const auto &nameValue : value->nameValues()) {
    const auto &name  = nameValue.first;
    const auto &value = nameValue.second;

    if (value.length())
      tableTip.addTableRow(name, value);
  }

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

QString
CQChartsBarChartObj::
groupStr() const
{
  const auto &value = this->value();

  return value->groupName();
}

QString
CQChartsBarChartObj::
nameStr() const
{
  const auto &value = this->value();

  return value->valueName();
}

QString
CQChartsBarChartObj::
valueStr() const
{
  QString valueStr;

  const auto &value = this->value();

  CQChartsBarChartValue::ValueInd minInd, maxInd;

  value->calcRange(minInd, maxInd);

  if      (plot_->isValueValue()) {
    valueStr = plot_->valueStr(minInd.value);
  }
  else if (plot_->isValueRange()) {
    QString minValueStr = plot_->valueStr(minInd.value);
    QString maxValueStr = plot_->valueStr(maxInd.value);

    valueStr = QString("%1-%2").arg(minValueStr).arg(maxValueStr);
  }
  else if (plot_->isValueMin()) {
    valueStr = plot_->valueStr(minInd.value);
  }
  else if (plot_->isValueMax()) {
    valueStr = plot_->valueStr(maxInd.value);
  }

  return valueStr;
}

CQChartsGeom::BBox
CQChartsBarChartObj::
dataLabelRect() const
{
  if (! plot_->dataLabel()->isVisible())
    return BBox();

  const auto &value = this->value();

  QString label = value->getNameValue("Label");

  if (! plot_->labelColumn().isValid()) {
    const auto &valueInds = value->valueInds();
    assert(! valueInds.empty());

    double value = valueInds[0].value;

    label = plot_->valueStr(value);
  }

  return plot_->dataLabel()->calcRect(rect(), label);
}

//---

void
CQChartsBarChartObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
//model->addProperty(path1, this, "selected")->setDesc("Is selected");

  model->addProperty(path1, this, "group")->setDesc("Group");
  model->addProperty(path1, this, "name" )->setDesc("Name");
  model->addProperty(path1, this, "value")->setDesc("Value");
  model->addProperty(path1, this, "color")->setDesc("Color");
}

//---

void
CQChartsBarChartObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->groupColumn());

  int nv = plot_->valueColumns().count();

  if (plot_->isStacked()) {
    if (ig_.i < nv)
      addColumnSelectIndex(inds, plot_->valueColumns().getColumn(ig_.i));
  }
  else {
    if (is_.i < nv)
      addColumnSelectIndex(inds, plot_->valueColumns().getColumn(is_.i));
  }

  addColumnSelectIndex(inds, plot_->nameColumn());
  addColumnSelectIndex(inds, plot_->labelColumn());
}

#if 0
bool
CQChartsBarChartObj::
isHidden() const
{
  if (is_.n > 1)
    return plot_->isSetHidden(is_.i);
  else
    return plot_->isSetHidden(ig_.i);
}
#endif

void
CQChartsBarChartObj::
draw(CQChartsPaintDevice *device)
{
  //if (isHidden())
  //  return;

  //---

  // calc bar borders
  double m1 = plot_->lengthPixelSize(plot_->margin(), ! plot_->isHorizontal());
  double m2 = m1;

  if (! plot_->isStacked()) {
    if      (is_.n > 1) {
      if      (ig_.i == 0)
        m1 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
      else if (ig_.i == ig_.n - 1)
        m2 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
    }
    else if (ig_.n > 1) {
      if      (iv_.i == 0)
        m1 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
      else if (iv_.i == iv_.n - 1)
        m2 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
    }
  }

  //---

  // adjust border sizes and rect
  static double minSize = 3.0;

  auto prect = plot_->windowToPixel(rect());

  double rs = prect.getSize(! plot_->isHorizontal());

  double s1 = rs - 2*m1;

  if (s1 < minSize) {
    m1 = (rs - minSize)/2.0;
    m2 = m1;
  }

  prect.expandExtent(-m1, -m2, ! plot_->isHorizontal());

  auto rect = plot_->pixelToWindow(prect);

  //---

  // calc pen and brush
  CQChartsPenBrush barPenBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(barPenBrush, updateState);

  //---

  if (! plot_->isDotLines()) {
    // draw rect
    device->setColorNames();

    drawRoundedPolygon(device, barPenBrush, rect, plot_->barCornerSize());

    device->resetColorNames();
  }
  else {
    // draw line
    device->setColorNames();

    double lw = plot_->lengthPixelSize(plot_->dotLineWidth(), ! plot_->isHorizontal());

    CQChartsDrawUtil::setPenBrush(device, barPenBrush);

    if (! plot_->isHorizontal()) {
      if (lw < 3) {
        double xc = rect.getXMid();

        device->drawLine(Point(xc, rect.getYMin()), Point(xc, rect.getYMax()));
      }
      else {
        double xc = prect.getXMid();

        BBox pbbox1(xc - lw/2, prect.getYMin(), xc + lw/2, prect.getYMax());

        CQChartsDrawUtil::drawRoundedPolygon(device, plot_->pixelToWindow(pbbox1));
      }
    }
    else {
      if (lw < 3) {
        double yc = rect.getYMid();

        device->drawLine(Point(rect.getXMin(), yc), Point(rect.getXMax(), yc));
      }
      else {
        double yc = prect.getYMid();

        BBox pbbox1(prect.getXMid(), yc - lw/2, prect.getXMax(), yc + lw/2);

        CQChartsDrawUtil::drawRoundedPolygon(device, plot_->pixelToWindow(pbbox1));
      }
    }

    device->resetColorNames();

    //---

    // draw dot
    device->setColorNames();

    auto symbolType = plot_->dotSymbolType();
    auto symbolSize = plot_->dotSymbolSize();

    CQChartsDrawUtil::setPenBrush(device, barPenBrush);

    Point p;

    if (! plot_->isHorizontal())
      p = Point(rect.getXMid(), rect.getYMax());
    else
      p = Point(rect.getXMax(), rect.getYMid());

    plot_->drawSymbol(device, p, symbolType, symbolSize, barPenBrush);

    device->resetColorNames();
  }
}

void
CQChartsBarChartObj::
drawFg(CQChartsPaintDevice *device) const
{
  // draw data label on foreground layers
  if (! plot_->dataLabel()->isVisible())
    return;

  //---

  const auto *value = this->value();

  QString minLabel = value->getNameValue("Label");
  QString maxLabel = minLabel;

  CQChartsBarChartValue::ValueInd minInd, maxInd;

  if (! plot_->labelColumn().isValid()) {
    value->calcRange(minInd, maxInd);

    minLabel = plot_->valueStr(minInd.value);
    maxLabel = plot_->valueStr(maxInd.value);
  }

  auto pos = plot_->dataLabel()->position();

  if (! plot_->dataLabel()->isPositionOutside()) {
    CQChartsPenBrush barPenBrush;

    calcPenBrush(barPenBrush, /*updateState*/false);

    plot_->charts()->setContrastColor(barPenBrush.brush.color());
  }

  if (minLabel == maxLabel) {
    if (! plot_->labelColumn().isValid() && minInd.value < 0)
      pos = CQChartsDataLabel::flipPosition(pos);

    if (minLabel != "")
      plot_->dataLabel()->draw(device, rect(), minLabel, pos);
  }
  else {
    if (plot_->dataLabel()->isPositionOutside()) {
      auto minPos = CQChartsDataLabel::Position::BOTTOM_OUTSIDE;
      auto maxPos = CQChartsDataLabel::Position::TOP_OUTSIDE;

      plot_->dataLabel()->draw(device, rect(), minLabel, minPos);
      plot_->dataLabel()->draw(device, rect(), maxLabel, maxPos);
    }
    else {
      auto minPos = CQChartsDataLabel::Position::BOTTOM_INSIDE;
      auto maxPos = CQChartsDataLabel::Position::TOP_INSIDE;

      plot_->dataLabel()->draw(device, rect(), minLabel, minPos);
      plot_->dataLabel()->draw(device, rect(), maxLabel, maxPos);
    }
  }

  plot_->charts()->resetContrastColor();
}

void
CQChartsBarChartObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  static double minBorderSize = 5.0;

  auto prect = plot_->windowToPixel(rect());

  double rs = prect.getSize(! plot_->isHorizontal());

  bool skipBorder = (rs < minBorderSize);

  //---

  ColorInd colorInd = calcColorInd();

  QColor bc = plot_->interpBarStrokeColor(colorInd);

  QColor barColor = calcBarColor();

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isBarStroked() && ! skipBorder, bc, plot_->barStrokeAlpha(),
              plot_->barStrokeWidth(), plot_->barStrokeDash()),
    BrushData(plot_->isBarFilled(), barColor, plot_->barFillAlpha(),
              plot_->barFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsBarChartObj::
calcBarColor() const
{
  // calc bar color
  ColorInd colorInd = calcColorInd();

  QColor barColor;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    int ns = (plot_->isValueValue() ? plot_->valueColumns().count() : 1);

    if (ns > 1) {
      if (plot_->isColorBySet())
        barColor = plot_->interpBarFillColor(ig_);
      else
        barColor = plot_->interpBarFillColor(is_);
    }
    else {
      if (! color_.isValid()) {
        if      (ig_.n > 1)
          barColor = plot_->interpBarFillColor(ig_);
        else if (iv_.n > 1)
          barColor = plot_->interpBarFillColor(iv_);
        else {
          ColorInd ig1(ig_.i    , ig_.n + 1);
          ColorInd ig2(ig_.i + 1, ig_.n + 1);

          QColor barColor1 = plot_->interpBarFillColor(ig1);
          QColor barColor2 = plot_->interpBarFillColor(ig2);

          barColor = CQChartsUtil::blendColors(barColor1, barColor2, iv_.value());
        }
      }
      else {
        barColor = plot_->interpColor(color_, colorInd);
      }
    }
  }
  else {
    barColor = plot_->interpBarFillColor(colorInd);
  }

  return barColor;
}

void
CQChartsBarChartObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

const CQChartsBarChartValue *
CQChartsBarChartObj::
value() const
{
  if (is_.n > 1) {
    const auto &valueSet = plot_->valueSet(ig_.i);
    const auto &ivalue   = valueSet.value(is_.i);

    return &ivalue;
  }
  else {
    const auto &valueSet = plot_->valueSet(ig_.i);
    const auto &ivalue   = valueSet.value(iv_.i);

    return &ivalue;
  }
}

//------

CQChartsBarKeyColor::
CQChartsBarKeyColor(CQChartsBarChartPlot *plot, const QString &name, const ColorInd &is,
                    const ColorInd &ig, const ColorInd &iv) :
 CQChartsKeyColorBox(plot, is, ig, iv), plot_(plot), name_(name)
{
}

bool
CQChartsBarKeyColor::
selectPress(const Point &, CQChartsSelMod selMod)
{
  if (selMod == CQChartsSelMod::ADD) {
    for (int i = 0; i < iv_.n; ++i) {
      plot_->CQChartsPlot::setSetHidden(i, i != iv_.i);
    }
  }
  else {
    setSetHidden(! isSetHidden());
  }

  plot_->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsBarKeyColor::
fillBrush() const
{
  // calc bar color
  ColorInd colorInd = calcColorInd();

  QColor barColor;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    int ns = (plot_->isValueValue() ? plot_->valueColumns().count() : 1);

    if (ns > 1) {
      if (plot_->isColorBySet()) {
        if (ig_.n > 1)
          barColor = plot_->interpBarFillColor(ig_);
        else
          barColor = plot_->interpBarFillColor(iv_);
      }
      else {
        if (is_.n > 1)
          barColor = plot_->interpBarFillColor(is_);
        else
          barColor = plot_->interpBarFillColor(iv_);
      }
    }
    else {
      if (! color_.isValid()) {
        if      (ig_.n > 1)
          barColor = plot_->interpBarFillColor(ig_);
        else if (iv_.n > 1)
          barColor = plot_->interpBarFillColor(iv_);
        else {
          ColorInd ig1(ig_.i    , ig_.n + 1);
          ColorInd ig2(ig_.i + 1, ig_.n + 1);

          QColor barColor1 = plot_->interpBarFillColor(ig1);
          QColor barColor2 = plot_->interpBarFillColor(ig2);

          barColor = CQChartsUtil::blendColors(barColor1, barColor2, iv_.value());
        }
      }
      else {
        barColor = plot_->interpColor(color_, colorInd);
      }
    }
  }
  else {
    barColor = plot_->interpBarFillColor(colorInd);
  }

  //---

  if (isSetHidden())
    barColor = CQChartsUtil::blendColors(barColor, key_->interpBgColor(),
                                         key_->hiddenAlpha().value());

  QBrush brush;

  BrushData barBrushData(plot_->isBarFilled(), barColor,
                         plot_->barFillAlpha(), plot_->barFillPattern());

  CQChartsDrawUtil::setBrush(brush, barBrushData);

  return brush;
}

QPen
CQChartsBarKeyColor::
strokePen() const
{
  ColorInd colorInd = calcColorInd();

  QColor bc = plot_->interpBarStrokeColor(colorInd);

  QPen pen;

  double width = plot_->lengthPixelWidth(plot_->barStrokeWidth());

  CQChartsUtil::setPen(pen, plot_->isBarStroked(), bc, plot_->barStrokeAlpha(),
                       width, plot_->barStrokeDash());

  return pen;
}

bool
CQChartsBarKeyColor::
tipText(const Point &, QString &tip) const
{
  int    count  = -1;
  bool   hasSum = true;
  double posSum = 0.0, negSum = 0.0;
  double value  = 0.0;

  int ns = (plot_->isValueValue() ? plot_->valueColumns().count() : 1);
  int nv = plot_->numValueSets();

  if (ns > 1) {
    if (plot_->isColorBySet()) {
      const auto &valueSet = plot_->valueSet(iv_.i);

      valueSet.calcSums(posSum, negSum);

      count = valueSet.numValues();
    }
    else {
      count = plot_->numSetValues();

      for (int i = 0; i < nv; ++i) {
        const auto &valueSet = plot_->valueSet(i);

        const auto &ivalue = valueSet.value(iv_.i);

        const auto &valueInds = ivalue.valueInds();
        assert(! valueInds.empty());

        double value = valueInds[0].value;

        if (value >= 0) posSum += value;
        else            negSum += value;
      }
    }
  }
  else {
    if      (nv > 1) {
      const auto &valueSet = plot_->valueSet(iv_.i);

      valueSet.calcSums(posSum, negSum);

      count = valueSet.numValues();
    }
    else if (nv == 1) {
      const auto &valueSet = plot_->valueSet(0);

      const auto &ivalue = valueSet.value(iv_.i);

      const auto &valueInds = ivalue.valueInds();
      assert(! valueInds.empty());

      value  = valueInds[0].value;
      hasSum = false;
    }
    else {
      hasSum = false;
    }
  }

  QString sumStr;

  if (hasSum) {
    if      (CMathUtil::isZero(negSum))
      sumStr = QString("%1").arg(posSum);
    else if (CMathUtil::isZero(posSum))
      sumStr = QString("%1").arg(negSum);
    else
      sumStr = QString("%1 -> %2").arg(negSum).arg(posSum);
  }

  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name_);

  if (count >= 0)
    tableTip.addTableRow("Count", count);

  if (sumStr.length())
    tableTip.addTableRow("Total", sumStr);
  else
    tableTip.addTableRow("Value", value);

  if (isSetHidden())
    tableTip.addTableRow("Hidden", "true");

  tip = tableTip.str();

  return true;
}

bool
CQChartsBarKeyColor::
isSetHidden() const
{
  if      (is_.n > 1)
    return plot_->CQChartsPlot::isSetHidden(is_.i);
  else if (ig_.n > 1)
    return plot_->CQChartsPlot::isSetHidden(ig_.i);
  else if (iv_.n > 1)
    return plot_->CQChartsPlot::isSetHidden(iv_.i);
  else
    return false;
}

void
CQChartsBarKeyColor::
setSetHidden(bool b)
{
  if      (is_.n > 1)
    plot_->CQChartsPlot::setSetHidden(is_.i, b);
  else if (ig_.n > 1)
    plot_->CQChartsPlot::setSetHidden(ig_.i, b);
  else if (iv_.n > 1)
    plot_->CQChartsPlot::setSetHidden(iv_.i, b);
}

//------

CQChartsBarKeyText::
CQChartsBarKeyText(CQChartsBarChartPlot *plot, const QString &text, const ColorInd &ic) :
 CQChartsKeyText(plot, text, ic)
{
}

QColor
CQChartsBarKeyText::
interpTextColor(const ColorInd &ind) const
{
  QColor c = CQChartsKeyText::interpTextColor(ind);

  if (isSetHidden())
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

bool
CQChartsBarKeyText::
isSetHidden() const
{
  return plot_->CQChartsPlot::isSetHidden(ic_.i);
}
