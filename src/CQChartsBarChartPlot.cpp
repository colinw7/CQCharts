#include <CQChartsBarChartPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsDataLabel.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsHtml.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQGroupBox.h>

#include <QMenu>
#include <QCheckBox>
#include <QHBoxLayout>

CQChartsBarChartPlotType::
CQChartsBarChartPlotType()
{
}

void
CQChartsBarChartPlotType::
addParameters()
{
  startParameterGroup("Bar Chart");

  // values, name and label columns
  addColumnsParameter("values", "Values", "valueColumns").
    setRequired().setNumericColumn().setPropPath("columns.values").setTip("Value column(s)");

  addColumnParameter("name", "Name", "nameColumn").
    setStringColumn().setBasic().setPropPath("columns.name").setTip("Custom group name");
  addColumnParameter("label", "Label", "labelColumn").
    setStringColumn().setPropPath("columns.label").setTip("Column to use for bar data label");

  // options
  addEnumParameter("orientation", "Orientation", "orientation").
    addNameValue("HORIZONTAL", int(Qt::Horizontal)).
    addNameValue("VERTICAL"  , int(Qt::Vertical  )).
    setTip("Bars orientation");

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
   addNameValue("SUM"  , int(CQChartsBarChartPlot::ValueType::SUM  )).
   setTip("Bar value type");

  addBoolParameter("percent"  , "Percent"   , "percent"  ).setTip("Show value is percentage");
  addBoolParameter("skipEmpty", "Skip Empty", "skipEmpty").setTip("Skip empty groups");

  addBoolParameter("dotLines", "Dot Lines", "dotLines").
    setTip("Draw bars as lines with dot");

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
     p("The bar heights are taken from the values in the " + B("Values") + " column.").
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
     p("The bars can drawn vertically (default) or horizontally using the " + B("Orientation") +
       " option.").
     p("Selecting the " + B("Stacked") + " Plot Type places grouped bars on top of each other "
      "instead of the " + B("Normal") + " side by side placement.").
     p("Selecting the " + B("Range") + " Value Type draws a bar for the range (min/max) of "
       "the grouped values, selecting the " + B("Min") + " Value Type draws a bar to the "
       "minimum of the grouped values and selecting the " + B("Max") + " Value Type draws a "
       "bar to the maximum of the grouped values.").
     p("Enabling the " + B("Percent") + " option rescales the values to a percentage of the "
       "maximum and minimum of the values.").
     p("Enabling the " + B("Skip Empty") + " option skips bars with zero values.").
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
}

CQChartsBarChartPlot::
~CQChartsBarChartPlot()
{
  term();
}

//---

void
CQChartsBarChartPlot::
init()
{
  CQChartsBarPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  setDotSymbol(Symbol::circle());
  setDotSymbolSize(Length::pixel(7));

  //---

  addColorMapKey();
}

void
CQChartsBarChartPlot::
term()
{
}

//---

void
CQChartsBarChartPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsBarChartPlot::
setLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsBarChartPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "name" ) c = this->nameColumn();
  else if (name == "label") c = this->labelColumn();
  else                      c = CQChartsBarPlot::getNamedColumn(name);

  return c;
}

void
CQChartsBarChartPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "name" ) this->setNameColumn(c);
  else if (name == "label") this->setLabelColumn(c);
  else                      CQChartsBarPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsBarChartPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "values") c = this->valueColumns();
  else                  c = CQChartsBarPlot::getNamedColumns(name);

  return c;
}

void
CQChartsBarChartPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "values") this->setValueColumns(c);
  else                  CQChartsBarPlot::setNamedColumns(name, c);
}

//---

void
CQChartsBarChartPlot::
addProperties()
{
  addBaseProperties();

  addBoxProperties();

  // columns
  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "labelColumn", "label", "Label column");

  // options
  addProp("options", "plotType" , "plotType" , "Plot type");
  addProp("options", "valueType", "valueType", "Value type");

  addProp("options", "percent"  , "", "Use percentage value");
  addProp("options", "skipEmpty", "", "Skip empty groups");

  addProp("options", "sortSets", "", "Sort by sets by key");

  // dot lines
  addProp("dotLines",        "dotLines"    , "visible", "Draw bars as lines with dot");
  addProp("dotLines/stroke", "dotLineWidth", "width"  , "Dot line width");

  addSymbolProperties("dotLines/symbol", "dot", "Dot line");

  // coloring
  addProp("coloring", "colorBySet", "", "Color by value set");

  //---

  addGroupingProperties();

  //---

  // color map key
  addColorMapKeyProperties();
}

//---

bool
CQChartsBarChartPlot::
isLabelsVisible() const
{
  return dataLabel()->isVisible();
}

void
CQChartsBarChartPlot::
setLabelsVisible(bool b)
{
  if (b != isLabelsVisible()) {
    dataLabel()->setVisible(b);

    updateRangeAndObjs(); emit customDataChanged();
  }
}

CQChartsLabelPosition
CQChartsBarChartPlot::
labelPosition() const
{
  return (CQChartsLabelPosition) dataLabel()->position();
}

//---

void
CQChartsBarChartPlot::
setPlotType(PlotType type)
{
  CQChartsUtil::testAndSet(plotType_, type, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

void
CQChartsBarChartPlot::
setNormal(bool /*b*/)
{
  setPlotType(PlotType::NORMAL);
}

void
CQChartsBarChartPlot::
setStacked(bool b)
{
  setPlotType(b ? PlotType::STACKED : PlotType::NORMAL);
}

void
CQChartsBarChartPlot::
setPercent(bool b)
{
  CQChartsUtil::testAndSet(percent_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsBarChartPlot::
setSkipEmpty(bool b)
{
  CQChartsUtil::testAndSet(skipEmpty_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

void
CQChartsBarChartPlot::
setValueType(ValueType type)
{
  CQChartsUtil::testAndSet(valueType_, type, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsBarChartPlot::
setValueValue(bool /*b*/)
{
  setValueType(ValueType::VALUE);
}

void
CQChartsBarChartPlot::
setValueRange(bool b)
{
  setValueType(b ? ValueType::RANGE : ValueType::VALUE);
}

void
CQChartsBarChartPlot::
setValueMin(bool b)
{
  setValueType(b ? ValueType::MIN : ValueType::VALUE);
}

void
CQChartsBarChartPlot::
setValueMax(bool b)
{
  setValueType(b ? ValueType::MAX : ValueType::VALUE);
}

void
CQChartsBarChartPlot::
setValueMean(bool b)
{
  setValueType(b ? ValueType::MEAN : ValueType::VALUE);
}

void
CQChartsBarChartPlot::
setValueSum(bool b)
{
  setValueType(b ? ValueType::SUM : ValueType::VALUE);
}

//---

void
CQChartsBarChartPlot::
setColorBySet(bool b)
{
  CQChartsUtil::testAndSet(colorBySet_, b, [&]() {
    resetSetHidden(); updateRangeAndObjs(); emit customDataChanged();
  });
}

void
CQChartsBarChartPlot::
setSortSets(bool b)
{
  CQChartsUtil::testAndSet(sortSets_, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  });
}

//---

void
CQChartsBarChartPlot::
setDotLines(bool b)
{
  CQChartsUtil::testAndSet(dotLineData_.enabled, b, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
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
    if (isVertical())
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
  //   individual values use group columns for grouping
  if (isValueValue())
    initGroupData(valueColumns(), nameColumn());
  else
    initGroupData(Columns(), nameColumn());

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
    // if single value column (ns == 1) and summary value (range, min, max, mean) for values
    // per group then single bar
    if (ns == 1) {
      if (! isValueValue()) {
        dataRange = Range();

        updateRange(-0.5, 0);

        numVisible = 0;

        for (const auto &pg : valueData_.valueGroupInd) {
          const auto &valueSet = valueData_.valueSets[pg.second];

          double min = 0.0, max = 0.0, mean = 0.0, sum = 0.0;

          if (valueSet.calcStats(min, max, mean, sum)) {
            if      (isValueMin())
              updateRange(0.0, min);
            else if (isValueMax() || isValueRange())
              updateRange(0.0, max);
            else if (isValueMean())
              updateRange(0.0, mean);
            else if (isValueSum())
              updateRange(0.0, sum);

            ++numVisible;
          }
          else {
            if (! isSkipEmpty())
              ++numVisible;
          }
        }
      }
      else {
        if (isStacked())
          numVisible = nv;
      }
    }

    //---

    double ymin = (isVertical() ? dataRange.ymin() : dataRange.xmin());

    updateRange(numVisible - 0.5, ymin);

    if (nv == 0) {
      double xmin = (isVertical() ? dataRange.xmin() : dataRange.ymin());

      updateRange(xmin, 1.0);
    }
  }
  else {
    updateRange(0.5, 1.0);
  }

  dataRange.makeNonZero();

  //---

  return dataRange;
}

void
CQChartsBarChartPlot::
postCalcRange()
{
  initRangeAxes();
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
  if (isVertical()) {
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
  // add value for each column (individual values)
  if (isValueValue()) {
    int ic = 0;

    for (const auto &column : valueColumns()) {
      Columns columns { column };

      addRowColumn(data, columns, dataRange, ic);

      ++ic;
    }
  }
  // add all values for columns (range)
  else {
    int ng = numGroups();

    if (ng > 1) {
      addRowColumn(data, this->valueColumns(), dataRange, -1);
    }
    else {
      int ic = 0;

      for (const auto &column : valueColumns()) {
        Columns columns { column };

        addRowColumn(data, columns, dataRange, ic);

        ++ic;
      }
    }
  }
}

void
CQChartsBarChartPlot::
addRowColumn(const ModelVisitor::VisitData &data, const Columns &valueColumns,
             Range &dataRange, int columnInd) const
{
  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  if (isColorKey() && colorColumn().isValid()) {
    ModelIndex colorModelInd(th, data.row, colorColumn(), data.parent);

    bool ok;

    auto colorValue = modelValue(colorModelInd, ok);

    bool hidden = (ok && CQChartsVariant::cmp(hideValue(), colorValue) == 0);

    if (hidden)
      return;
  }

  //---

  auto updateRange = [&](double x, double y) {
    if (isVertical())
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
    ind = ModelIndex(th, data.row, Column(), data.parent);
  }

  //---

  // get optional group for value
  int groupInd = rowGroupInd(ind);

  if (groupInd < 0)
    groupInd = columnInd;

  // get group name
  auto groupName = groupIndName(groupInd);

  if (groupName == "")
    groupName = QString::number(groupInd);

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

    auto var = modelValue(nameInd, ok2);

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
  auto *valueSet = const_cast<CQChartsBarChartValueSet *>(groupValueSet(groupInd));

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
      if (isSkipBad())
        continue;

      th->addDataError(valueModelInd, "Invalid value");

      r = data.row;
    }

    if (CMathUtil::isNaN(r))
      continue;

    //---

    // get associated model index
    auto valInd  = modelIndex(valueModelInd);
    auto valInd1 = normalizeIndex(valInd);

    // add value and index
    ValueInd valueInd;

    valueInd.value = r;
    valueInd.ind   = valInd1;
    valueInd.vrow  = data.vrow;

    valueInds.push_back(std::move(valueInd));
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

    if (labelColumn().isValid())
      valueData.setNameColumnValue("Label", labelColumn(), labelStr);
  }
  else {
    auto setColumnNameValue = [&](const Column &column, const QString &header,
                                  const QString &value) {
      if (! value.length()) return;

      auto headerStr = header;

      if (column.isValid()) {
        headerStr = columnHeaderName(column, /*tip*/true);

        if (headerStr == "")
          headerStr = header;
      }

      valueData.setNameColumnValue(headerStr, column, value);
    };

    int ng = numGroups();

    if (ng > 1) {
      // if path grouping (hierarchical) then value name is group name
      if (isGroupPathType()) {
        if (groupName.length())
          valueData.setGroupName(groupName);
      }

      // save other name values for tip
      setColumnNameValue(groupColumn(), "Group", group   );
      setColumnNameValue(nameColumn (), "Name" , name    );
      setColumnNameValue(labelColumn(), "Label", labelStr);
      setColumnNameValue(colorColumn(), "Color", colorStr);
    }
    else {
      valueData.setValueName(name);

      if (labelColumn().isValid())
        valueData.setNameColumnValue("Label", labelColumn(), labelStr);
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

  // update range for scale and sums (TODO: only after all rows processed ?)
  if (isStacked()) {
    if (isValueValue()) {
      updateRange(0, scale*posSum);
      updateRange(0, scale*negSum);
    }
  }
  else {
    for (const auto &valueInd : valueInds)
      updateRange(0, scale*valueInd.value);
  }
}

//----

int
CQChartsBarChartPlot::
numValueSets() const
{
  return valueData_.valueSets.size();
}

const CQChartsBarChartPlot::ValueSet &
CQChartsBarChartPlot::
valueSet(int i) const
{
  assert(i >= 0 && i < int(valueData_.valueSets.size()));

  if (isSortSets()) {
    if (valueData_.sortedInds.empty()) {
      auto *th = const_cast<CQChartsBarChartPlot *>(this);

      for (const auto &vi : valueData_.valueInds)
        th->valueData_.sortedInds.push_back(vi.second);
    }

    int i1 = valueData_.sortedInds[i];

    return valueData_.valueSets[i1];
  }
  else
    return valueData_.valueSets[i];
}

int
CQChartsBarChartPlot::
numSetValues() const
{
  return (! valueData_.valueSets.empty() ? valueData_.valueSets[0].numValues() : 0);
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

  auto name = groupIndName(groupInd);

  if (name == "")
    name = QString::number(groupInd);

  auto type = groupType();

  if (type == ColumnType::REAL || type == ColumnType::INTEGER) {
    auto value = groupIndValue(groupInd);

    valueData_.valueInds[value] = ind;
  }
  else
    valueData_.valueInds[name] = ind;

  valueData_.valueSets.emplace_back(name, ind);

  valueData_.valueGroupInd[groupInd] = ind;

  auto *valueSet = &valueData_.valueSets.back();

  valueSet->setGroupInd(groupInd);

  assert(valueData_.valueInds.size() == valueData_.valueSets.size());

  return valueSet;
}

//------

CQChartsGeom::BBox
CQChartsBarChartPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsBarChartPlot::calcExtraFitBBox");

  BBox bbox;

  auto position = labelPosition();

  if (position != CQChartsLabelPosition::TOP_OUTSIDE &&
      position != CQChartsLabelPosition::BOTTOM_OUTSIDE)
    return bbox;

  if (isLabelsVisible()) {
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
      double posSum = 0.0, negSum = 0.0;

      valueSet.calcSums(posSum, negSum);

      double total = posSum - negSum;

      scale = (total > 0.0 ? 100.0/total : 1.0);
    }

    //---

    double bx1 = bx;

    // calc bar width (side by side)
    double bw1 = 1.0;

    if (! isStacked())
      bw1 = 1.0/numVisible1;

    //---

    // if single value column (ns == 1) and summary value (range, min, max, mean) for values
    // per group then single bar
    if (ns == 1 && ! isValueValue()) {
      double min = 0.0, max = 0.0, mean = 0.0, sum = 0.0;

      if (valueSet.calcStats(min, max, mean, sum)) {
        double value1 = (isValueRange() ? min : 0.0);
        double value2 = value1;

        if      (isValueMin())
          value2 = min;
        else if (isValueMax() || isValueRange())
          value2 = max;
        else if (isValueMean())
          value2 = mean;
        else if (isValueSum())
          value2 = sum;

        auto brect = CQChartsGeom::makeDirBBox(isHorizontal(), bx, value1, bx + 1.0, value2);

        //---

        const auto &ivalue    = valueSet.value(0);
        const auto &valueInds = ivalue.valueInds();

        CQChartsBarChartValue::ValueInd ind;

        if (! valueInds.empty())
          ind = valueInds[0];

        //---

        auto *barObj = createBarObj(brect, ColorInd(), ColorInd(iv, nv), ColorInd(), ind.ind);

        barObj->setValueSet(true);

        objs.push_back(barObj);
      }
      else {
        if (isSkipEmpty())
          continue; // no inc bx
      }
    }
    else {
      // NOTE: summary values (min, max, mean, range, sum are across the multiple value columns)
      double lastPosValue = 0.0, lastNegValue = 0.0;

      for (int ivs = 0; ivs < nvs; ++ivs) {
        if (isValueHidden(ivs))
          continue;

        //---

        const auto &ivalue = valueSet.value(ivs);

        CQChartsBarChartValue::ValueInd minInd, maxInd;
        double                          mean = 0.0, sum = 0.0;

        ivalue.calcRange(minInd, maxInd, mean, sum);

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
        else if (isValueMean()) {
          if (isStacked()) {
            value1 = lastPosValue;
            value2 = value1 + scale*mean;
          }
          else {
            value1 = 0.0;
            value2 = scale*mean;
          }
        }

        if (value1 == value2) {
          if (isSkipEmpty())
            continue;
        }

        if (isStacked())
          brect = CQChartsGeom::makeDirBBox(isHorizontal(), bx, value1, bx + 1.0, value2);
        else
          brect = CQChartsGeom::makeDirBBox(isHorizontal(), bx1, value1, bx1 + bw1, value2);

        if (isVertical())
          barWidth_ = std::min(barWidth_, brect.getWidth());
        else
          barWidth_ = std::min(barWidth_, brect.getHeight());

        CQChartsBarChartObj *barObj = nullptr;

        if (ns > 1) {
          // multiple sets:
          //  . set per value column
          //  . group per group column unique value
          barObj = createBarObj(brect, ColorInd(ivs, nvs), ColorInd(iv, nv),
                                ColorInd(), minInd.ind);
        }
        else {
          // single set:
          //  . group per group column unique value
          //  . value per grouped values
          barObj = createBarObj(brect, ColorInd(), ColorInd(iv, nv),
                                ColorInd(ivs, nvs), minInd.ind);
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
        else if (isValueMean()) {
          lastPosValue = scale*mean;
        }

        bx1 += bw1;
      }
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

  xAxis->setValueType     (CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER),
                           /*notify*/false);
  xAxis->setGridMid       (true);
//xAxis->setMajorIncrement(1);
  xAxis->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR);

  yAxis->setValueType     (CQChartsAxisValueType(isLogY() ?
                             CQChartsAxisValueType::Type::LOG :
                             CQChartsAxisValueType::Type::REAL), /*notify*/false);
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

        // skip empty value sets (TODO: all value types ?)
        if (valueSet.numValues() == 0 && isSkipEmpty()) {
          if (! isValueValue())
            continue;
        }

        xAxis->setTickLabel(numVisible, valueSet.name());

        ++numVisible;
      }
    }
    else if (nv == 1) {
      std::set<int> positions;

      xAxis->getTickLabelsPositions(positions);

      const auto &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      if (isValueValue()) {
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
      else {
        QStringList names;

        for (int ivs = 0; ivs < nvs; ++ivs) {
          if (isValueHidden(ivs))
            continue;

          const auto &value = valueSet.value(ivs);

          names += value.valueName();
        }

        xAxis->setTickLabel(numVisible, names.join(", "));

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
  return (isVertical() ? xAxis() : yAxis());
}

CQChartsAxis *
CQChartsBarChartPlot::
mappedYAxis() const
{
  return (isVertical() ? yAxis() : xAxis());
}

void
CQChartsBarChartPlot::
addKeyItems(PlotKey *key)
{
  // start at next row (vertical) or next column (horizontal) from previous key
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyRow = [&](const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                       const QString &name, const QColor &c=QColor()) {
    auto *colorItem = new CQChartsBarColorKeyItem(this, name, is, ig, iv);
    auto *textItem  = new CQChartsBarTextKeyItem (this, name, iv);

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addRowItems(colorItem, textItem);

    if (c.isValid())
      colorItem->setColor(Color(c));

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

        auto name = valueSet.name();

        addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), name);
      }
    }
    else {
      const auto &valueSet = this->valueSet(0);

      int nvs = valueSet.numValues();

      for (int ivs = 0; ivs < nvs; ++ivs) {
        const auto &value = valueSet.value(ivs);

        auto name = value.valueName();

        addKeyRow(ColorInd(), ColorInd(), ColorInd(ivs, nvs), name);
      }
    }
  }
  else {
    if      (nv > 1) {
      if (isColorBySet()) {
        auto name = this->valueName();

        addKeyRow(ColorInd(), ColorInd(), ColorInd(), name);
      }
      else {
        for (int iv = 0; iv < nv; ++iv) {
          const auto &valueSet = this->valueSet(iv);

          // skip empty value sets (TODO: all value types ?)
          if (valueSet.numValues() == 0 && isSkipEmpty()) {
            if (! isValueValue())
              continue;
          }

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

          auto name = valueSet.name();

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

        auto iname = ivalue.valueName();

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

        auto name = valueSet.name();

        addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), name, c);
      }
    }
    else {
      auto title = this->titleStr();

      if (! title.length()) {
        bool ok;

        auto yname = modelHHeaderString(valueColumns().column(), ok);

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
  (void) addMenuCheckedAction(valueMenu, "Mean" , isValueMean (), SLOT(setValueMean (bool)));
  (void) addMenuCheckedAction(valueMenu, "Sum"  , isValueSum  (), SLOT(setValueSum  (bool)));

  menu->addMenu(valueMenu);

  (void) addCheckedAction("Percent"  , isPercent (), SLOT(setPercent(bool)));
  (void) addCheckedAction("Dot Lines", isDotLines(), SLOT(setDotLines(bool)));

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  //---

  addRootMenuItems(menu);

  return true;
}

//---

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

//---

bool
CQChartsBarChartPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsBarChartPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

CQChartsBarChartObj *
CQChartsBarChartPlot::
createBarObj(const BBox &rect, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
             const QModelIndex &ind) const
{
  return new CQChartsBarChartObj(this, rect, is, ig, iv, ind);
}

//---

CQChartsPlotCustomControls *
CQChartsBarChartPlot::
createCustomControls()
{
  auto *controls = new CQChartsBarChartPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsBarChartObj::
CQChartsBarChartObj(const CQChartsBarChartPlot *plot, const BBox &rect, const ColorInd &is,
                    const ColorInd &ig, const ColorInd &iv, const QModelIndex &ind) :
 CQChartsPlotObj(const_cast<CQChartsBarChartPlot *>(plot), rect, is, ig, iv),
 plot_(plot)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
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

  plot()->addNoTipColumns(tableTip);

  //---

  auto addOptColumnRow = [&](const CQChartsColumn &column, const QString &header,
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

  auto addOptColumnsRow = [&](const CQChartsColumns &columns, const QString &header,
                              const QString &value) {
    if (! value.length())
      return;

    auto headerStr = header;

    if (columns.isValid()) {
      headerStr = plot_->columnsHeaderName(columns, /*tip*/true);

      if (headerStr == "")
        headerStr = header;
    }

    tableTip.addTableRow(headerStr, value);
  };

  //---

  addOptColumnRow (plot_->groupColumn (), "Group", this->groupStr());
  addOptColumnRow (plot_->nameColumn  (), "Name" , this->nameStr ());
  addOptColumnsRow(plot_->valueColumns(), "Value", this->valueStr());
  addOptColumnRow (plot_->colorColumn (), "Color");

  //---

  const auto *value = this->value();

  for (const auto &pcv : value->nameColumnValueMap()) {
    const auto &column = pcv.second.column;

    if (column.isValid() && tableTip.hasColumn(column))
      continue;

    const auto &name  = pcv.first;
    const auto &value = pcv.second.value;

    if (value.length())
      tableTip.addTableRow(name, value);

    if (column.isValid())
      tableTip.addColumn(column);
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
  const auto *value = this->value();

  return value->groupName();
}

QString
CQChartsBarChartObj::
nameStr() const
{
  const auto *value = this->value();

  return value->valueName();
}

QString
CQChartsBarChartObj::
valueStr() const
{
  QString valueStr;

  if (isValueSet()) {
    const auto *valueSet = this->valueSet();

    double min = 0.0, max = 0.0, mean = 0.0, sum = 0.0;

    if (! valueSet->calcStats(min, max, mean, sum))
      return "";

    if      (plot_->isValueRange()) {
      auto minValueStr = plot_->valueStr(min);
      auto maxValueStr = plot_->valueStr(max);

      valueStr = QString("%1-%2").arg(minValueStr).arg(maxValueStr);
    }
    else if (plot_->isValueMin())
      valueStr = plot_->valueStr(min);
    else if (plot_->isValueMax())
      valueStr = plot_->valueStr(max);
    else if (plot_->isValueMean())
      valueStr = plot_->valueStr(mean);
    else if (plot_->isValueSum())
      valueStr = plot_->valueStr(sum);
  }
  else {
    const auto *value = this->value();

    CQChartsBarChartValue::ValueInd minInd, maxInd;
    double                          mean = 0.0, sum = 0.0;

    value->calcRange(minInd, maxInd, mean, sum);

    if      (plot_->isValueValue()) {
      valueStr = plot_->valueStr(minInd.value);
    }
    else if (plot_->isValueRange()) {
      auto minValueStr = plot_->valueStr(minInd.value);
      auto maxValueStr = plot_->valueStr(maxInd.value);

      valueStr = QString("%1-%2").arg(minValueStr).arg(maxValueStr);
    }
    else if (plot_->isValueMin())
      valueStr = plot_->valueStr(minInd.value);
    else if (plot_->isValueMax())
      valueStr = plot_->valueStr(maxInd.value);
    else if (plot_->isValueMean())
      valueStr = plot_->valueStr(mean);
    else if (plot_->isValueSum())
      valueStr = plot_->valueStr(sum);
  }

  return valueStr;
}

CQChartsGeom::BBox
CQChartsBarChartObj::
dataLabelRect() const
{
  if (! plot_->isLabelsVisible())
    return BBox();

  const auto *value = this->value();

  auto label = value->getNameValue("Label");

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
  auto path1 = path + "/" + propertyId();

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
draw(PaintDevice *device) const
{
  //if (isHidden())
  //  return;

  //---

  // calc bar borders
  double m1 = plot_->lengthPixelSize(plot_->margin(), plot_->isVertical());
  double m2 = m1;

  if (! plot_->isStacked()) {
    if      (is_.n > 1) {
      if      (ig_.i == 0)
        m1 = plot_->lengthPixelSize(plot_->groupMargin(), plot_->isVertical());
      else if (ig_.i == ig_.n - 1)
        m2 = plot_->lengthPixelSize(plot_->groupMargin(), plot_->isVertical());
    }
    else if (ig_.n > 1) {
      if      (iv_.i == 0)
        m1 = plot_->lengthPixelSize(plot_->groupMargin(), plot_->isVertical());
      else if (iv_.i == iv_.n - 1)
        m2 = plot_->lengthPixelSize(plot_->groupMargin(), plot_->isVertical());
    }
  }

  //---

  // adjust border sizes and rect
  static double minSize = 3.0;

  auto prect = plot_->windowToPixel(rect());

  double rs = prect.getSize(plot_->isVertical());

  double s1 = rs - 2*m1;

  if (s1 < minSize) {
    m1 = (rs - minSize)/2.0;
    m2 = m1;
  }

  prect.expandExtent(-m1, -m2, plot_->isVertical());

  auto rect = plot_->pixelToWindow(prect);

  //---

  // calc pen and brush
  PenBrush barPenBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(barPenBrush, updateState);

  //---

  device->setColorNames();

  if (! plot_->isDotLines()) {
    // draw rect
    CQChartsDrawUtil::drawRoundedRect(device, barPenBrush, rect, plot_->barCornerSize());
  }
  else {
    // draw dot line
    CQChartsDrawUtil::drawDotLine(device, barPenBrush, rect, plot_->dotLineWidth(),
                                  plot_->isHorizontal(), plot_->dotSymbol(),
                                  plot_->dotSymbolSize());
  }

  device->resetColorNames();
}

void
CQChartsBarChartObj::
drawFg(PaintDevice *device) const
{
  // draw data label on foreground layers
  if (! plot_->isLabelsVisible())
    return;

  //---

  QString minLabel, maxLabel;
  double  minValue = 0;

  // calc min/max value
  if (plot_->isValueValue()) {
    const auto *value = this->value();

    minLabel = value->getNameValue("Label");
    maxLabel = minLabel;

    CQChartsBarChartValue::ValueInd minInd, maxInd;
    double                          mean = 0.0, sum = 0.0;

    if (! plot_->labelColumn().isValid()) {
      value->calcRange(minInd, maxInd, mean, sum);

      double scale = 1.0;

      if (plot_->isPercent()) {
        const auto *valueSet = this->valueSet();

        double posSum = 0.0, negSum = 0.0;

        valueSet->calcSums(posSum, negSum);

        double total = posSum - negSum;

        scale = (total > 0.0 ? 100.0/total : 1.0);
      }

      minLabel = plot_->valueStr(scale*minInd.value);
      maxLabel = plot_->valueStr(scale*maxInd.value);
    }

    minValue = minInd.value;
  }
  else {
    const auto *valueSet = this->valueSet();

    double min = 0.0, max = 0.0, mean = 0.0, sum = 0.0;

    (void) valueSet->calcStats(min, max, mean, sum);

    if      (plot_->isValueRange()) {
      minLabel = plot_->valueStr(min);
      maxLabel = plot_->valueStr(max);
    }
    else if (plot_->isValueMin())
      minLabel = plot_->valueStr(min);
    else if (plot_->isValueMax())
      minLabel = plot_->valueStr(max);
    else if (plot_->isValueMean())
      minLabel = plot_->valueStr(mean);
    else if (plot_->isValueSum())
      minLabel = plot_->valueStr(sum);

    if (maxLabel == "")
      maxLabel = minLabel;

    minValue = min;
  }

  //---

  if (! plot_->dataLabel()->isPositionOutside()) {
    PenBrush barPenBrush;

    calcPenBrush(barPenBrush, /*updateState*/false);

    plot_->charts()->setContrastColor(barPenBrush.brush.color());
  }

  //---

  if (minLabel == maxLabel) {
    auto pos = plot_->labelPosition();

    if (! plot_->labelColumn().isValid() && minValue < 0)
      pos = CQChartsDataLabel::flipPosition(pos);

    if (minLabel != "")
      plot_->dataLabel()->draw(device, rect(), minLabel, (CQChartsDataLabel::Position) pos);
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

  //---

  plot_->charts()->resetContrastColor();
}

void
CQChartsBarChartObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  static double minBorderSize = 5.0;

  auto prect = plot_->windowToPixel(rect());

  double rs = prect.getSize(plot_->isVertical());

  bool skipBorder = (rs < minBorderSize);

  //---

  auto colorInd = calcColorInd();

  auto bc = plot_->interpBarStrokeColor(colorInd);

  auto barColor = calcBarColor();

  auto penData = plot_->barPenData(bc);

  if (skipBorder)
    penData.setVisible(false);

  plot_->setPenBrush(penBrush, penData, plot_->barBrushData(barColor));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsBarChartObj::
calcBarColor() const
{
  // calc bar color
  auto colorInd = calcColorInd();

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

          auto barColor1 = plot_->interpBarFillColor(ig1);
          auto barColor2 = plot_->interpBarFillColor(ig2);

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
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

const CQChartsBarChartValue *
CQChartsBarChartObj::
value() const
{
  const auto *valueSet = this->valueSet();

  if (is_.n > 1) {
    const auto &ivalue = valueSet->value(is_.i);

    return &ivalue;
  }
  else {
    const auto &ivalue = valueSet->value(iv_.i);

    return &ivalue;
  }
}

const CQChartsBarChartValueSet *
CQChartsBarChartObj::
valueSet() const
{
  if (is_.n > 1) {
    const auto &valueSet = plot_->valueSet(ig_.i);

    return &valueSet;
  }
  else {
    const auto &valueSet = plot_->valueSet(ig_.i);

    return &valueSet;
  }
}

//------

CQChartsBarColorKeyItem::
CQChartsBarColorKeyItem(Plot *plot, const QString &name, const ColorInd &is,
                        const ColorInd &ig, const ColorInd &iv) :
 CQChartsColorBoxKeyItem(plot, is, ig, iv), plot_(plot), name_(name)
{
  setClickable(true);
}

#if 0
bool
CQChartsBarColorKeyItem::
selectPress(const Point &, SelMod selMod)
{
  if (selMod == SelMod::ADD) {
    for (int i = 0; i < iv_.n; ++i) {
      plot_->CQChartsPlot::setSetHidden(i, i != iv_.i);
    }
  }
  else {
    setSetHidden(! isSetHidden());
  }

  return true;
}
#endif

#if 0
void
CQChartsBarColorKeyItem::
doSelect(SelMod)
{
  CQChartsPlot::PlotObjs objs;

  plot()->getGroupObjs(ig_.i, objs);
  if (objs.empty()) return;

  //---

  plot()->selectObjs(objs, /*export*/true);

  key_->redraw(/*wait*/ true);
}
#endif

QBrush
CQChartsBarColorKeyItem::
fillBrush() const
{
  // calc bar color
  auto colorInd = calcColorInd();

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

          auto barColor1 = plot_->interpBarFillColor(ig1);
          auto barColor2 = plot_->interpBarFillColor(ig2);

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

  adjustFillColor(barColor);

  QBrush brush;

  auto barBrushData = plot_->barBrushData(barColor);

  CQChartsDrawUtil::setBrush(brush, barBrushData);

  return brush;
}

QPen
CQChartsBarColorKeyItem::
strokePen() const
{
  auto colorInd = calcColorInd();

  auto bc = plot_->interpBarStrokeColor(colorInd);

  auto penData = plot_->barPenData(bc);

  PenBrush penBrush;

  plot_->setPen(penBrush, penData);

  return penBrush.pen;
}

bool
CQChartsBarColorKeyItem::
tipText(const Point &, QString &tip) const
{
  int    count  = -1;
  bool   hasSum = true;
  double value  = 0.0;

  double posSum = 0.0, negSum = 0.0;

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
      sumStr = QString::number(posSum);
    else if (CMathUtil::isZero(posSum))
      sumStr = QString::number(negSum);
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

#if 0
bool
CQChartsBarColorKeyItem::
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
CQChartsBarColorKeyItem::
setSetHidden(bool b)
{
  if      (is_.n > 1)
    plot_->CQChartsPlot::setSetHidden(is_.i, b);
  else if (ig_.n > 1)
    plot_->CQChartsPlot::setSetHidden(ig_.i, b);
  else if (iv_.n > 1)
    plot_->CQChartsPlot::setSetHidden(iv_.i, b);
}
#endif

//------

CQChartsBarTextKeyItem::
CQChartsBarTextKeyItem(Plot *plot, const QString &text, const ColorInd &ic) :
 CQChartsTextKeyItem(plot, text, ic)
{
}

QColor
CQChartsBarTextKeyItem::
interpTextColor(const ColorInd &ind) const
{
  auto c = CQChartsTextKeyItem::interpTextColor(ind);

  adjustFillColor(c);

  return c;
}

#if 0
bool
CQChartsBarTextKeyItem::
isSetHidden() const
{
  return plot_->CQChartsPlot::isSetHidden(ic_.i);
}
#endif

//------

CQChartsBarChartPlotCustomControls::
CQChartsBarChartPlotCustomControls(CQCharts *charts) :
 CQChartsGroupPlotCustomControls(charts, "barchart")
{
}

void
CQChartsBarChartPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsBarChartPlotCustomControls::
addWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  // values, name and label columns
  addColumnWidgets(QStringList() << "values" << "name", columnsFrame);

  //---

  // label group
  labelFrame_ = createGroupFrame("Bar Label", "labelFrame");

  auto *labelCornerFrame  = CQUtil::makeWidget<QFrame>("labelCornerFrame");
  auto *labelCornerLayout = CQUtil::makeLayout<QHBoxLayout>(labelCornerFrame, 0, 2);

  labelCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Visible", "labelVisible");

  labelCornerLayout->addWidget(labelCheck_);

  labelColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("labelColumnCombo");

  labelColumnCombo_->setToolTip("Column to use for bar data label");

  labelFrame_.groupBox->setCornerWidget(labelCornerFrame);

  addFrameWidget(labelFrame_, "Column", labelColumnCombo_);

  //---

  // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame");

  orientationCombo_ = createEnumEdit("orientation");
  plotTypeCombo_    = createEnumEdit("plotType");
  valueTypeCombo_   = createEnumEdit("valueType");

  addFrameWidget(optionsFrame, "Orientation", orientationCombo_);
  addFrameWidget(optionsFrame, "Plot Type"  , plotTypeCombo_);
  addFrameWidget(optionsFrame, "Value Type" , valueTypeCombo_);

  percentCheck_    = createBoolEdit("percent");
  skipEmptyCheck_  = createBoolEdit("skipEmpty");
  dotLinesCheck_   = createBoolEdit("dotLines");
  colorBySetCheck_ = createBoolEdit("colorBySet");

  addFrameWidget(optionsFrame, "Percent"     , percentCheck_);
  addFrameWidget(optionsFrame, "Skip Empty"  , skipEmptyCheck_);
  addFrameWidget(optionsFrame, "Dot Lines"   , dotLinesCheck_);
  addFrameWidget(optionsFrame, "Color by Set", colorBySetCheck_);

  //---

  addGroupColumnWidgets();
  addColorColumnWidgets();

  //---

  addKeyList();
}

void
CQChartsBarChartPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    labelColumnCombo_, SIGNAL(columnChanged()), this, SLOT(labelColumnSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    labelCheck_, SIGNAL(stateChanged(int)), this, SLOT(labelVisibleSlot(int)));

  CQChartsWidgetUtil::connectDisconnect(b,
    orientationCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    plotTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotTypeSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    valueTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(valueTypeSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    percentCheck_, SIGNAL(stateChanged(int)), this, SLOT(percentSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    skipEmptyCheck_, SIGNAL(stateChanged(int)), this, SLOT(skipEmptySlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    dotLinesCheck_, SIGNAL(stateChanged(int)), this, SLOT(dotLinesSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    colorBySetCheck_, SIGNAL(stateChanged(int)), this, SLOT(colorBySetSlot()));

  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsBarChartPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  plot_ = dynamic_cast<CQChartsBarChartPlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);

  if (plot_)
    connect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsBarChartPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  labelColumnCombo_->setModelColumn(plot_->getModelData(), plot_->labelColumn());

  labelCheck_->setChecked(plot_->isLabelsVisible());

  //---

  orientationCombo_->setCurrentValue((int) plot_->orientation());
  plotTypeCombo_   ->setCurrentValue((int) plot_->plotType());
  valueTypeCombo_  ->setCurrentValue((int) plot_->valueType());

  percentCheck_   ->setChecked(plot_->isPercent());
  skipEmptyCheck_ ->setChecked(plot_->isSkipEmpty());
  dotLinesCheck_  ->setChecked(plot_->isDotLines());
  colorBySetCheck_->setChecked(plot_->isColorBySet());

  //---

  CQChartsGroupPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

//---

void
CQChartsBarChartPlotCustomControls::
labelColumnSlot()
{
  plot_->setLabelColumn(labelColumnCombo_->getColumn());
}

void
CQChartsBarChartPlotCustomControls::
labelVisibleSlot(int b)
{
  plot_->setLabelsVisible(b);
}

void
CQChartsBarChartPlotCustomControls::
orientationSlot()
{
  plot_->setOrientation((Qt::Orientation) orientationCombo_->currentValue());
}

void
CQChartsBarChartPlotCustomControls::
plotTypeSlot()
{
  plot_->setPlotType((CQChartsBarChartPlot::PlotType) plotTypeCombo_->currentValue());
}

void
CQChartsBarChartPlotCustomControls::
valueTypeSlot()
{
  plot_->setValueType((CQChartsBarChartPlot::ValueType) valueTypeCombo_->currentValue());
}

void
CQChartsBarChartPlotCustomControls::
percentSlot()
{
  plot_->setPercent(percentCheck_->isChecked());
}

void
CQChartsBarChartPlotCustomControls::
skipEmptySlot()
{
  plot_->setSkipEmpty(skipEmptyCheck_->isChecked());
}

void
CQChartsBarChartPlotCustomControls::
dotLinesSlot()
{
  plot_->setDotLines(dotLinesCheck_->isChecked());
}

void
CQChartsBarChartPlotCustomControls::
colorBySetSlot()
{
  plot_->setColorBySet(colorBySetCheck_->isChecked());
}

CQChartsColor
CQChartsBarChartPlotCustomControls::
getColorValue()
{
  return plot_->barFillColor();
}

void
CQChartsBarChartPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setBarFillColor(c);
}
