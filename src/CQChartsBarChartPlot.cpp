#include <CQChartsBarChartPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsDataLabel.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
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
    setString().setTip("Custom group name");
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

  addBoolParameter("colorBySet", "Color by Set", "colorBySet").setTip("Color by value set");

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
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBarChartPlot(view, model);
}

//------

CQChartsBarChartPlot::
CQChartsBarChartPlot(CQChartsView *view, const ModelP &model) :
 CQChartsBarPlot(view, view->charts()->plotType("barchart"), model),
 CQChartsObjDotPointData<CQChartsBarChartPlot>(this)
{
  NoUpdate noUpdate(this);

  //---

  setDotSymbolType(CQChartsSymbol::Type::CIRCLE);
  setDotSymbolSize(CQChartsLength("7px"));
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

  CQChartsPlot::addProperties();

  CQChartsBarPlot::addProperties();

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

  CQChartsGroupPlot::addProperties();
}

//---

void
CQChartsBarChartPlot::
setPlotType(PlotType type)
{
  CQChartsUtil::testAndSet(plotType_, type, [&]() { updateRangeAndObjs(); } );
}

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
setDotLineWidth(const CQChartsLength &l)
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

  //---

  CQChartsGeom::Range dataRange;

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
    BarChartVisitor(const CQChartsBarChartPlot *plot, CQChartsGeom::Range &dataRange) :
     plot_(plot), dataRange_(dataRange) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRow(data, dataRange_);

      return State::OK;
    }

   private:
    const CQChartsBarChartPlot* plot_ { nullptr };
    CQChartsGeom::Range&        dataRange_;
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
      const CQChartsBarChartValueSet &valueSet = this->valueSet(0);

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

  //---

  //dataRange = adjustDataRange(dataRange);

  //---

  initRangeAxes();

  //---

  return dataRange;
}

void
CQChartsBarChartPlot::
initRangeAxes() const
{
  CQChartsBarChartPlot *th = const_cast<CQChartsBarChartPlot *>(this);

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
  CQChartsAxis *xAxis = mappedXAxis();
  CQChartsAxis *yAxis = mappedYAxis();

  xAxis->setColumn(groupColumn().isValid() ? groupColumn() : nameColumn());

  QString xname;

  if (ns > 1) {
    if (! isRowGrouping()) {
      bool ok;

      xname = modelHeaderString(groupColumn().isValid() ? groupColumn() : nameColumn(), ok);
    }
  }
  else {
    if (ng > 1) {
      bool ok;

      xname = modelHeaderString(groupColumn().isValid() ? groupColumn() : nameColumn(), ok);
    }
    else {
    }
  }

  if (xname != "")
    xAxis->setLabel(xname);

  //---

  yAxis->setColumn(valueColumns().column());

  QString yname;

  if (valueColumns().count() <= 1) {
    bool ok;

    yname = modelHeaderString(valueColumns().column(), ok);
  }

  yAxis->setLabel(yname);
}

void
CQChartsBarChartPlot::
addRow(const ModelVisitor::VisitData &data, CQChartsGeom::Range &dataRange) const
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
             CQChartsGeom::Range &dataRange) const
{
  auto updateRange = [&](double x, double y) {
    if (! isHorizontal())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  //---

  CQChartsModelIndex ind;

  if (isValueValue()) {
    assert(valueColumns.count() > 0);

    const CQChartsColumn &valueColumn = valueColumns.column();

    ind = CQChartsModelIndex(data.row, valueColumn, data.parent);
  }
  else {
    ind = CQChartsModelIndex(data.row, CQChartsColumn(), data.parent);
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
    bool ok1;

    group = modelHierString(data.row, groupColumn(), data.parent, ok1);

    categoryName = group;
  }

  //---

  // get name string (if defined) and update group name
  QString name;

  if (nameColumn().isValid()) {
    bool ok2;

    QVariant var = modelValue(data.row, nameColumn(), data.parent, ok2);

    double r;

    if (CQChartsVariant::toReal(var, r))
      name = columnStr(nameColumn(), r);
    else
      name = modelString(data.row, nameColumn(), data.parent, ok2);

    if (! categoryName.length())
      categoryName = name;
  }

  //---

  // get optional data label string
  QString labelStr;

  if (labelColumn().isValid()) {
    bool ok3;

    labelStr = modelString(data.row, labelColumn(), data.parent, ok3);
  }

  //---

  // get optional color string
  QString colorStr;

  if (colorColumn().isValid()) {
    bool ok4;

    colorStr = modelString(data.row, colorColumn(), data.parent, ok4);
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
    double r;

    bool ok2 = modelMappedReal(data.row, valueColumn, data.parent, r, isLogY(), data.row);

    if (! ok2)
      r = data.row;

    if (CMathUtil::isNaN(r))
      continue;

    // get associated model index
    QModelIndex valInd  = modelIndex(data.row, valueColumn, data.parent);
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

      const CQChartsColumn &valueColumn = valueColumns.column();

      bool ok;

      valueName = modelHeaderString(valueColumn, ok);
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
  CQChartsBarChartPlot *th = const_cast<CQChartsBarChartPlot *>(this);

  th->valueData_.clear();
}

const CQChartsBarChartValueSet *
CQChartsBarChartPlot::
groupValueSet(int groupInd) const
{
  CQChartsBarChartPlot *th = const_cast<CQChartsBarChartPlot *>(this);

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

  CQChartsBarChartValueSet *valueSet = &valueData_.valueSets.back();

  valueSet->setGroupInd(groupInd);

  return valueSet;
}

//------

CQChartsGeom::BBox
CQChartsBarChartPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  CQChartsDataLabel::Position position = dataLabel()->position();

  if (position != CQChartsDataLabel::TOP_OUTSIDE && position != CQChartsDataLabel::BOTTOM_OUTSIDE)
    return bbox;

  if (dataLabel()->isVisible()) {
    for (const auto &plotObj : plotObjs_) {
      CQChartsBarChartObj *barObj = dynamic_cast<CQChartsBarChartObj *>(plotObj);

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

    const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

    //const QString &setName = valueSet.name;

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

      const CQChartsBarChartValue &ivalue = valueSet.value(ivs);

      CQChartsBarChartValue::ValueInd minInd, maxInd;

      ivalue.calcRange(minInd, maxInd);

      QModelIndex parent; // TODO

      CQChartsColor color;

      (void) columnColor(minInd.vrow, parent, color);

      //---

      // create bar rect
      CQChartsGeom::BBox brect;

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
        barObj = new CQChartsBarChartObj(this, brect,
          ColorInd(ivs, nvs), ColorInd(iv, nv), ColorInd(), minInd.ind);
      }
      else {
        // single set:
        //  . group per group column unique value
        //  . value per grouped values
        barObj = new CQChartsBarChartObj(this, brect,
          ColorInd(), ColorInd(iv, nv), ColorInd(ivs, nvs), minInd.ind);
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
  CQChartsBarChartPlot *th = const_cast<CQChartsBarChartPlot *>(this);

  th->initObjAxesI();
}

void
CQChartsBarChartPlot::
initObjAxesI()
{
  // init axes
  CQChartsAxis *xAxis = mappedXAxis();
  CQChartsAxis *yAxis = mappedYAxis();

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

      const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

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

        const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

        xAxis->setTickLabel(numVisible, valueSet.name());

        ++numVisible;
      }
    }
    else if (nv == 1) {
      std::set<int> positions;

      xAxis->getTickLabelsPositions(positions);

      const CQChartsBarChartValueSet &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      for (int ivs = 0; ivs < nvs; ++ivs) {
        if (isValueHidden(ivs))
          continue;

        if (positions.find(numVisible) != positions.end()) {
          const CQChartsBarChartValue &value = valueSet.value(ivs);

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
  return mappedYAxis()->label();
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
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyRow = [&](const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                       const QString &name, const QColor &c=QColor()) {
    CQChartsBarKeyColor *keyColor = new CQChartsBarKeyColor(this, is, ig, iv);
    CQChartsBarKeyText  *keyText  = new CQChartsBarKeyText (this, name, iv);

    if (c.isValid())
      keyColor->setColor(c);

    if (! key->isHorizontal()) {
      key->addItem(keyColor, row, 0);
      key->addItem(keyText , row, 1);

      ++row;
    }
    else {
      key->addItem(keyColor, 0, col++);
      key->addItem(keyText , 0, col++);
    }
  };

  //---

  int ns = (isValueValue() ? this->valueColumns().count() : 1);
  int nv = numValueSets();

  if (ns > 1) {
    // ivs is 'value set' index (group)
    // iv is 'value set' value index
    if (isColorBySet()) {
      for (int iv = 0; iv < nv; ++iv) {
        const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

        addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), valueSet.name());
      }
    }
    else {
      const CQChartsBarChartValueSet &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      for (int ivs = 0; ivs < nvs; ++ivs) {
        const CQChartsBarChartValue &value = valueSet.value(ivs);

        addKeyRow(ColorInd(), ColorInd(), ColorInd(ivs, nvs), value.valueName());
      }
    }
  }
  else {
    if      (nv > 1) {
      if (isColorBySet()) {
        addKeyRow(ColorInd(), ColorInd(), ColorInd(), valueName());
      }
      else {
        for (int iv = 0; iv < nv; ++iv) {
          const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

          QColor c;

          if (valueSet.numValues() == 1) {
            const CQChartsBarChartValue &ivalue = valueSet.value(0);

            const CQChartsBarChartValue::ValueInds &valueInds = ivalue.valueInds();
            assert(! valueInds.empty());

            const CQChartsBarChartValue::ValueInd &ind0 = valueInds[0];

            QModelIndex   parent; // TODO
            CQChartsColor color;

            if (columnColor(ind0.vrow, parent, color))
              c = interpColor(color, ColorInd());
          }

          addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), valueSet.name(), c);
        }
      }
    }
    else if (nv == 1) {
      const CQChartsBarChartValueSet &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      for (int ivs = 0; ivs < nvs; ++ivs) {
        const CQChartsBarChartValue &ivalue = valueSet.value(ivs);

        const CQChartsBarChartValue::ValueInds &valueInds = ivalue.valueInds();
        assert(! valueInds.empty());

        const CQChartsBarChartValue::ValueInd &ind0 = valueInds[0];

        QColor c;

        QModelIndex   parent; // TODO
        CQChartsColor color;

        if (columnColor(ind0.vrow, parent, color))
          c = interpColor(color, ColorInd());

        addKeyRow(ColorInd(), ColorInd(), ColorInd(ivs, nvs), ivalue.valueName(), c);
      }
    }

#if 0
    if (isGroupHeaders()) {
      for (int iv = 0; iv < nv; ++iv) {
        const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

        QColor c;

        if (valueSet.numValues() == 1) {
          const CQChartsBarChartValue &ivalue = valueSet.value(0);

          const CQChartsBarChartValue::ValueInds &valueInds = ivalue.valueInds();
          assert(! valueInds.empty());

          const CQChartsBarChartValue::ValueInd &ind0 = valueInds[0];

          QModelIndex   parent; // TODO
          CQChartsColor color;

          if (columnColor(ind0.vrow, parent, color))
            c = interpColor(color, ColorInd());
        }

        addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), valueSet.name(), c);
      }
    }
    else {
      QString title = this->titleStr();

      if (! title.length()) {
        bool ok;

        QString yname = modelHeaderString(valueColumns().column(), ok);

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
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name,
                                  bool isSet, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) -> QAction *{
    return addMenuCheckedAction(menu, name, isSet, slot);
  };

  //---

  menu->addSeparator();

  (void) addCheckedAction("Horizontal", isHorizontal(), SLOT(setHorizontal(bool)));

  QMenu *typeMenu = new QMenu("Plot Type");

  (void) addMenuCheckedAction(typeMenu, "Normal" , isNormal (), SLOT(setNormal (bool)));
  (void) addMenuCheckedAction(typeMenu, "Stacked", isStacked(), SLOT(setStacked(bool)));

  menu->addMenu(typeMenu);

  QMenu *valueMenu = new QMenu("Value Type");

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

CQChartsBarChartObj::
CQChartsBarChartObj(const CQChartsBarChartPlot *plot, const CQChartsGeom::BBox &rect,
                    const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                    const QModelIndex &ind) :
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
      headerStr = plot_->columnHeaderName(plot_->nameColumn());

    tableTip.addTableRow(headerStr, nameStr);
  }

  if (valueStr.length()) {
    QString headerStr("Value");

    if (plot_->valueColumns().isValid())
      headerStr = plot_->columnsHeaderName(plot_->valueColumns());

    tableTip.addTableRow(headerStr, valueStr);
  }

  const CQChartsBarChartValue *value = this->value();

  for (const auto &nameValue : value->nameValues()) {
    const QString &name  = nameValue.first;
    const QString &value = nameValue.second;

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
  const CQChartsBarChartValue *value = this->value();

  return value->groupName();
}

QString
CQChartsBarChartObj::
nameStr() const
{
  const CQChartsBarChartValue *value = this->value();

  return value->valueName();
}

QString
CQChartsBarChartObj::
valueStr() const
{
  QString valueStr;

  const CQChartsBarChartValue *value = this->value();

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
    return CQChartsGeom::BBox();

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  QRectF qrect = prect.qrect();

  const CQChartsBarChartValue *value = this->value();

  QString label = value->getNameValue("Label");

  if (! plot_->labelColumn().isValid()) {
    const CQChartsBarChartValue::ValueInds &valueInds = value->valueInds();
    assert(! valueInds.empty());

    double value = valueInds[0].value;

    label = plot_->valueStr(value);
  }

  return plot_->dataLabel()->calcRect(qrect, label);
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
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->groupColumn());

  if (plot_->isStacked())
    addColumnSelectIndex(inds, plot_->valueColumns().getColumn(ig_.i));
  else
    addColumnSelectIndex(inds, plot_->valueColumns().getColumn(is_.i));

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

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  //---

  // calc bar borders
  static double minBorderSize = 5.0;
  static double minSize       = 3.0;

  bool skipBorder = false;

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

  double rs = prect.getSize(! plot_->isHorizontal());

  if (rs < minBorderSize)
    skipBorder = true;

  double s1 = rs - 2*m1;

  if (s1 < minSize) {
    m1 = (rs - minSize)/2.0;
    m2 = m1;
  }

  prect.expandExtent(-m1, -m2, ! plot_->isHorizontal());

  //---

  CQChartsGeom::BBox rect = plot_->pixelToWindow(prect);

  //---

  // calc bar color
  QColor barColor = calcBarColor();

  //---

  // calc pen and brush
  ColorInd colorInd = calcColorInd();

  CQChartsPenBrush barPenBrush;

  QColor bc = plot_->interpBarStrokeColor(colorInd);

  plot_->setPenBrush(barPenBrush.pen, barPenBrush.brush,
    plot_->isBarStroked() && ! skipBorder,
    bc, plot_->barStrokeAlpha(), plot_->barStrokeWidth(), plot_->barStrokeDash(),
    plot_->isBarFilled(), barColor, plot_->barFillAlpha(), plot_->barFillPattern());

  plot_->updateObjPenBrushState(this, barPenBrush.pen, barPenBrush.brush);

  //---

  if (! plot_->isDotLines()) {
    // draw rect
    drawRoundedPolygon(device, barPenBrush, rect, plot_->barCornerSize());
  }
  else {
    QRectF qrect  = rect .qrect();
    QRectF pqrect = prect.qrect();

    // draw line
    double lw = plot_->lengthPixelSize(plot_->dotLineWidth(), ! plot_->isHorizontal());

    if (! plot_->isHorizontal()) {
      double xc = qrect.center().x();

      if (lw < 3)
        device->drawLine(QPointF(xc, qrect.bottom()), QPointF(xc, qrect.top()));
      else {
        QRectF qrect1(xc - lw/2, pqrect.top(), lw, pqrect.height());

        CQChartsDrawUtil::drawRoundedPolygon(device, device->pixelToWindow(qrect1));
      }
    }
    else {
      double yc = qrect.center().y();

      if (lw < 3)
        device->drawLine(QPointF(qrect.left(), yc), QPointF(qrect.right(), yc));
      else {
        QRectF qrect1(pqrect.left(), yc - lw/2, pqrect.width(), lw);

        CQChartsDrawUtil::drawRoundedPolygon(device, device->pixelToWindow(qrect1));
      }
    }

    //---

    // draw dot
    CQChartsSymbol symbolType = plot_->dotSymbolType();
    CQChartsLength symbolSize = plot_->dotSymbolSize();

    CQChartsDrawUtil::setPenBrush(device, barPenBrush);

    QPointF p;

    if (! plot_->isHorizontal())
      p = QPointF(qrect.center().x(), qrect.top());
    else
      p = QPointF(qrect.right(), qrect.center().y());

    plot_->drawSymbol(device, p, symbolType, symbolSize, barPenBrush.pen, barPenBrush.brush);
  }
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
drawFg(CQChartsPaintDevice *device) const
{
  // draw data label on foreground layers
  if (! plot_->dataLabel()->isVisible())
    return;

  QRectF qrect = rect().qrect();

  //---

  const CQChartsBarChartValue *value = this->value();

  QString minLabel = value->getNameValue("Label");
  QString maxLabel = minLabel;

  CQChartsBarChartValue::ValueInd minInd, maxInd;

  if (! plot_->labelColumn().isValid()) {
    value->calcRange(minInd, maxInd);

    minLabel = plot_->valueStr(minInd.value);
    maxLabel = plot_->valueStr(maxInd.value);
  }

  CQChartsDataLabel::Position pos = plot_->dataLabel()->position();

  if (minLabel == maxLabel) {
    if (! plot_->labelColumn().isValid() && minInd.value < 0)
      pos = CQChartsDataLabel::flipPosition(pos);

    if (minLabel != "")
      plot_->dataLabel()->draw(device, qrect, minLabel, pos);
  }
  else {
    if (plot_->dataLabel()->isPositionOutside()) {
      CQChartsDataLabel::Position minPos = CQChartsDataLabel::Position::BOTTOM_OUTSIDE;
      CQChartsDataLabel::Position maxPos = CQChartsDataLabel::Position::TOP_OUTSIDE;

      plot_->dataLabel()->draw(device, qrect, minLabel, minPos);
      plot_->dataLabel()->draw(device, qrect, maxLabel, maxPos);
    }
    else {
      CQChartsDataLabel::Position minPos = CQChartsDataLabel::Position::BOTTOM_INSIDE;
      CQChartsDataLabel::Position maxPos = CQChartsDataLabel::Position::TOP_INSIDE;

      plot_->dataLabel()->draw(device, qrect, minLabel, minPos);
      plot_->dataLabel()->draw(device, qrect, maxLabel, maxPos);
    }
  }
}

const CQChartsBarChartValue *
CQChartsBarChartObj::
value() const
{
  if (is_.n > 1) {
    const CQChartsBarChartValueSet &valueSet = plot_->valueSet(ig_.i);

    const CQChartsBarChartValue &ivalue = valueSet.value(is_.i);

    return &ivalue;
  }
  else {
    const CQChartsBarChartValueSet &valueSet = plot_->valueSet(ig_.i);

    const CQChartsBarChartValue &ivalue = valueSet.value(iv_.i);

    return &ivalue;
  }
}

//------

CQChartsBarKeyColor::
CQChartsBarKeyColor(CQChartsBarChartPlot *plot, const ColorInd &is, const ColorInd &ig,
                    const ColorInd &iv) :
 CQChartsKeyColorBox(plot, is, ig, iv), plot_(plot)
{
}

bool
CQChartsBarKeyColor::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod selMod)
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
    barColor = CQChartsUtil::blendColors(barColor, key_->interpBgColor(), key_->hiddenAlpha());

  return barColor;
}

bool
CQChartsBarKeyColor::
tipText(const CQChartsGeom::Point &, QString &tip) const
{
  int    count  = -1;
  bool   hasSum = true;
  double posSum = 0.0, negSum = 0.0;
  double value  = 0.0;

  int ns = (plot_->isValueValue() ? plot_->valueColumns().count() : 1);
  int nv = plot_->numValueSets();

  if (ns > 1) {
    if (plot_->isColorBySet()) {
      const CQChartsBarChartValueSet &valueSet = plot_->valueSet(iv_.i);

      valueSet.calcSums(posSum, negSum);

      count = valueSet.numValues();
    }
    else {
      count = plot_->numSetValues();

      for (int i = 0; i < nv; ++i) {
        const CQChartsBarChartValueSet &valueSet = plot_->valueSet(i);

        const CQChartsBarChartValue &ivalue = valueSet.value(iv_.i);

        const CQChartsBarChartValue::ValueInds &valueInds = ivalue.valueInds();
        assert(! valueInds.empty());

        double value = valueInds[0].value;

        if (value >= 0) posSum += value;
        else            negSum += value;
      }
    }
  }
  else {
    if      (nv > 1) {
      const CQChartsBarChartValueSet &valueSet = plot_->valueSet(iv_.i);

      valueSet.calcSums(posSum, negSum);

      count = valueSet.numValues();
    }
    else if (nv == 1) {
      const CQChartsBarChartValueSet &valueSet = plot_->valueSet(0);

      const CQChartsBarChartValue &ivalue = valueSet.value(iv_.i);

      const CQChartsBarChartValue::ValueInds &valueInds = ivalue.valueInds();
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
