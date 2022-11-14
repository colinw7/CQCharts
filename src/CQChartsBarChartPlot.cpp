#include <CQChartsBarChartPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsDataLabel.h>
#include <CQChartsDensity.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
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
    addNameValue("HORIZONTAL", static_cast<int>(Qt::Horizontal)).
    addNameValue("VERTICAL"  , static_cast<int>(Qt::Vertical  )).
    setTip("Bars orientation");

  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("NORMAL" , static_cast<int>(BarChartPlot::PlotType::NORMAL )).
    addNameValue("STACKED", static_cast<int>(BarChartPlot::PlotType::STACKED)).
    setTip("Plot type");

  addEnumParameter("valueType", "Value Type", "valueType").
   addNameValue("VALUE", static_cast<int>(BarChartPlot::ValueType::VALUE)).
   addNameValue("RANGE", static_cast<int>(BarChartPlot::ValueType::RANGE)).
   addNameValue("MIN"  , static_cast<int>(BarChartPlot::ValueType::MIN  )).
   addNameValue("MAX"  , static_cast<int>(BarChartPlot::ValueType::MAX  )).
   addNameValue("MEAN" , static_cast<int>(BarChartPlot::ValueType::MEAN )).
   addNameValue("SUM"  , static_cast<int>(BarChartPlot::ValueType::SUM  )).
   setTip("Bar value type");

  addEnumParameter("shapeType", "Shape Type", "shapeType").
    addNameValue("RECT"    , static_cast<int>(BarChartPlot::ShapeType::RECT    )).
    addNameValue("DOT_LINE", static_cast<int>(BarChartPlot::ShapeType::DOT_LINE)).
    addNameValue("BOX"     , static_cast<int>(BarChartPlot::ShapeType::BOX     )).
    addNameValue("SCATTER" , static_cast<int>(BarChartPlot::ShapeType::SCATTER )).
    addNameValue("VIOLIN"  , static_cast<int>(BarChartPlot::ShapeType::VIOLIN  )).
    setTip("Bar shape type");

  addBoolParameter("percent"  , "Percent"   , "percent"  ).setTip("Show value is percentage");
  addBoolParameter("skipEmpty", "Skip Empty", "skipEmpty").setTip("Skip empty groups");

  addBoolParameter("groupByColumn", "Group by Column", "groupByColumn").
    setTip("Group by column when multiple columns");

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
  CQChartsBarChartPlot::term();
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
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsBarChartPlot::
setLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
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

CQChartsColumns
CQChartsBarChartPlot::
calcValueColumns() const
{
  Columns columns;

  for (const auto &c : valueColumns()) {
    if (c.isValid())
      columns.addColumn(c);
  }

  return columns;
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

  // grouping
  addProp("options", "groupByColumn", "", "Group by column when multiple columns");

  // dot lines
  addProp("dotLines/stroke", "dotLineWidth", "width", "Dot line width");

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

    updateRangeAndObjs(); Q_EMIT customDataChanged();
  }
}

CQChartsLabelPosition
CQChartsBarChartPlot::
labelPosition() const
{
  return static_cast<CQChartsLabelPosition>(dataLabel()->position());
}

//---

void
CQChartsBarChartPlot::
setPlotType(PlotType type)
{
  CQChartsUtil::testAndSet(plotType_, type, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
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
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsBarChartPlot::
setSkipEmpty(bool b)
{
  CQChartsUtil::testAndSet(skipEmpty_, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsBarChartPlot::
setValueType(ValueType type)
{
  CQChartsUtil::testAndSet(valueType_, type, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
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
setShapeType(ShapeType type)
{
  CQChartsUtil::testAndSet(shapeType_, type, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsBarChartPlot::
setGroupByColumn(bool b)
{
  CQChartsUtil::testAndSet(groupByColumn_, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  });
}

void
CQChartsBarChartPlot::
setColorBySet(bool b)
{
  CQChartsUtil::testAndSet(colorBySet_, b, [&]() {
    resetSetHidden(); updateRangeAndObjs(); Q_EMIT customDataChanged();
  });
}

void
CQChartsBarChartPlot::
setSortSets(bool b)
{
  CQChartsUtil::testAndSet(sortSets_, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  });
}

//---

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

  RangeData rangeData;

  //---

  // check columns
  bool columnsValid = true;

  // value columns required
  // name, label, group, color columns optional

  auto valueColumns = calcValueColumns();

  if (! checkColumns(valueColumns, "Values", /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(labelColumn(), "Label")) columnsValid = false;
  if (! checkColumn(groupColumn(), "Group")) columnsValid = false;
  if (! checkColumn(colorColumn(), "Color")) columnsValid = false;

  if (! columnsValid)
    return rangeData.dataRange;

  //---

  auto updateRange = [&](double x, double y) {
    if (isVertical())
      rangeData.dataRange.updateRange(x, y);
    else
      rangeData.dataRange.updateRange(y, x);
  };

  //---

  updateRange(-0.5, 0);

  //---

  initGroupValueSet();

  //---

  // init grouping
  //   individual values use group columns for grouping
#if 0
  if (isValueValue())
    initGroupData(valueColumns, nameColumn());
  else
    initGroupData(Columns(), nameColumn());
#else
  if (isGroupByColumn())
    initGroupData(valueColumns, nameColumn());
  else
    initGroupData(Columns(), nameColumn());
#endif

  //---

  if (valueColumns.count() == 1) {
    auto valueColumn = valueColumns.column();

    const auto *columnDetails = this->columnDetails(valueColumn);

    if (columnDetails) {
      bool ok;

      double r1 = CQChartsVariant::toReal(columnDetails->minValue(), ok);
      if (ok) rangeData.minValue = OptReal(r1);

      double r2 = CQChartsVariant::toReal(columnDetails->maxValue(), ok);
      if (ok) rangeData.maxValue = OptReal(r2);

      double sum = CQChartsVariant::toReal(columnDetails->sumValue(), ok);
      if (ok) rangeData.sumValue = OptReal(sum);
    }
  }

  //---

  // process model data
  class BarChartVisitor : public ModelVisitor {
   public:
    BarChartVisitor(const CQChartsBarChartPlot *barChartPlot, RangeData &rangeData) :
     barChartPlot_(barChartPlot), rangeData_(rangeData) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      barChartPlot_->addRow(data, rangeData_);

      return State::OK;
    }

   private:
    const CQChartsBarChartPlot* barChartPlot_ { nullptr };
    RangeData&                  rangeData_;
  };

  BarChartVisitor barChartVisitor(this, rangeData);

  visitModel(barChartVisitor);

  //---

  int ns = calcNumSets();
  int nv = numValueSets();
  int ng = numGroups();

  int numVisible = 0;

  // if multiple sets then set per column
  if      (ns > 1) {
    for (int is = 0; is < ns; ++is)
      numVisible += ! isSetHidden(is);
  }
  // if single set then set per value set
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
        rangeData.dataRange = Range();

        updateRange(-0.5, 0);

        numVisible = 0;

        for (const auto &pg : valueData_.valueGroupInd) {
          const auto &valueSet = valueData_.valueSets[size_t(pg.second)];

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

    double ymin = (isVertical() ? rangeData.dataRange.ymin() : rangeData.dataRange.xmin());

    updateRange(numVisible - 0.5, ymin);

    if (nv == 0) {
      double xmin = (isVertical() ? rangeData.dataRange.xmin() : rangeData.dataRange.ymin());

      updateRange(xmin, 1.0);
    }
  }
  else {
    updateRange(0.5, 1.0);
  }

  rangeData.dataRange.makeNonZero();

  //---

  return rangeData.dataRange;
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
  auto valueColumns = calcValueColumns();

  int ns = calcNumSets();
//int nv = numValueSets();
  int ng = numGroups();

  // needed ?
  if (isVertical()) {
    setXValueColumn(groupColumn().isValid() ? groupColumn() : nameColumn());
    setYValueColumn(valueColumns.column());
  }
  else {
    setYValueColumn(groupColumn().isValid() ? groupColumn() : nameColumn());
    setXValueColumn(valueColumns.column());
  }

  //---

  // set axis column and labels
  auto *xAxis = mappedXAxis();
  auto *yAxis = mappedYAxis();

  xAxis->setColumn(groupColumn().isValid() ? groupColumn() : nameColumn());

  QString xname;

  if (ns > 1) {
    // grouped by value column headers
    if (isGroupByColumn()) {
      xname = "Values";
    }
    // grouped by group column values
    else {
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

  yAxis->setColumn(valueColumns.column());

  QString yname;

  if (valueColumns.count() <= 1) {
    bool ok;

    yname = modelHHeaderString(valueColumns.column(), ok);

    if (isPercent())
      yname += " (%)";
  }

  yAxis->setDefLabel(yname);
}

void
CQChartsBarChartPlot::
addRow(const ModelVisitor::VisitData &data, RangeData &rangeData) const
{
  auto valueColumns = calcValueColumns();

  // add value for each column (individual values)
  if (isValueValue()) {
    int ic = 0;

    for (const auto &column : valueColumns) {
      Columns columns { column };

      addRowColumn(data, columns, column, rangeData, ic);

      ++ic;
    }
  }
  else {
    // add value set per column
    if (isGroupByColumn()) {
      int ic = 0;

      for (const auto &column : valueColumns) {
        Columns columns { column };

        addRowColumn(data, columns, column, rangeData, ic);

        ++ic;
      }
    }
    // add all values for columns (range)
    else {
      int ng = numGroups();

      if (ng > 1) {
        addRowColumn(data, valueColumns, Column(), rangeData, -1);
      }
      else {
        int ic = 0;

        for (const auto &column : valueColumns) {
          Columns columns { column };

          addRowColumn(data, columns, Column(), rangeData, ic);

          ++ic;
        }
      }
    }
  }
}

void
CQChartsBarChartPlot::
addRowColumn(const ModelVisitor::VisitData &data, const Columns &valueColumns,
             const Column &valueColumn, RangeData &rangeData, int columnInd) const
{
  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  if (isColorKey() && colorColumn().isValid()) {
    ModelIndex colorModelInd(th, data.row, colorColumn(), data.parent);

    bool ok;

    auto colorValue = modelValue(colorModelInd, ok);

    bool hidden = (ok && isHideValue(colorValue));

    if (hidden)
      return;
  }

  //---

  auto updateRange = [&](double x, double y) {
    if (isVertical())
      rangeData.dataRange.updateRange(x, y);
    else
      rangeData.dataRange.updateRange(y, x);
  };

  //---

  ModelIndex ind;

  if (valueColumn.isValid())
    ind = ModelIndex(th, data.row, valueColumn, data.parent);
  else
    ind = ModelIndex(th, data.row, Column(), data.parent);

  //---

  // get optional group for value
  bool useColumnInd = false;

  int groupInd = rowGroupInd(ind);

  if (groupInd < 0 && columnInd >= 0) {
    groupInd     = columnInd;
    useColumnInd = true;
  }

  // get group name
  auto groupName = groupIndName(groupInd);

  if (groupName == "") {
    if (useColumnInd)
      groupName = columnHeaderName(valueColumns.getColumn(columnInd), /*tip*/false);
    else
      groupName = QString::number(groupInd);
  }

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
  auto *valueSet =
    const_cast<ValueSet *>(groupValueSet(groupInd, (useColumnInd ? columnInd : -1)));

  //---

  using VValueInd  = BarValue::ValueInd;
  using VValueInds = BarValue::ValueInds;

  VValueInds valueInds;

  // add values for columns (1 column normally, all columns when grouped)
  for (const auto &valueColumn : valueColumns) {
    ModelIndex valueModelInd(th, data.row, valueColumn, data.parent);

    // get bad value for row
    auto defVal = getModelBadValue(valueColumn, data.row);

    double r;

    bool ok2 = modelMappedReal(valueModelInd, r, isLogY(), defVal);

    if (! ok2) {
      if (isSkipBad())
        continue;

      th->addDataError(valueModelInd, "Invalid value");

      r = defVal;
    }

    if (CMathUtil::isNaN(r))
      continue;

    //---

    // get associated model index
    auto valInd  = modelIndex(valueModelInd);
    auto valInd1 = normalizeIndex(valInd);

    // add value and index
    VValueInd valueInd;

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

  int ns = calcNumSets();

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

      if (labelColumn().isValid())
        valueData.setNameColumnValue("Label", labelColumn(), labelStr);
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

  valueSet->setMinValue(rangeData.minValue);
  valueSet->setMaxValue(rangeData.maxValue);
  valueSet->setSumValue(rangeData.sumValue);

  //---

  // calc pos/neg sums
  double posSum = 0.0, negSum = 0.0;

  if (isPercent() || isStacked()) {
    valueSet->calcSums(posSum, negSum);
  }

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
  return int(valueData_.valueSets.size());
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

    int i1 = valueData_.sortedInds[size_t(i)];

    return valueData_.valueSets[size_t(i1)];
  }
  else
    return valueData_.valueSets[size_t(i)];
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
groupValueSet(int groupInd, int columnInd) const
{
  auto *th = const_cast<CQChartsBarChartPlot *>(this);

  return th->groupValueSetI(groupInd, columnInd);
}

CQChartsBarChartValueSet *
CQChartsBarChartPlot::
groupValueSetI(int groupInd, int columnInd)
{
  auto p = valueData_.valueGroupInd.find(groupInd);

  if (p != valueData_.valueGroupInd.end()) {
    int ind = (*p).second;

    assert(ind >= 0 && ind < int(valueData_.valueSets.size()));

    return &valueData_.valueSets[size_t(ind)];
  }

  //---

  auto valueColumns = calcValueColumns();

  int ind = numValueSets();

  auto name = groupIndName(groupInd);

  if (name == "") {
    if (columnInd >= 0)
      name = columnHeaderName(valueColumns.getColumn(columnInd), /*tip*/false);
    else
      name = QString::number(groupInd);
  }

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

int
CQChartsBarChartPlot::
calcNumSets() const
{
  auto valueColumns = calcValueColumns();

#if 0
  return (isValueValue() ? valueColumns.count() : 1);
#else
  return (isGroupByColumn() ? valueColumns.count() : 1);
#endif
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

  int ns = calcNumSets();
  int nv = numValueSets();
  int ng = numGroups();

  //---

  // start at px1 - bar width
  double bx = -0.5;

  //---

  auto addValueSetBarObj = [&](const ValueSet &valueSet, int iv) {
    double min = 0.0, max = 0.0, mean = 0.0, sum = 0.0;

    if (! valueSet.calcStats(min, max, mean, sum))
      return false;

    double value1 = (isValueRange() ? min : 0.0);
    double value2 = value1;

    if      (isValueMin  ()) value2 = min;
    else if (isValueMax  ()) value2 = max;
    else if (isValueRange()) value2 = max;
    else if (isValueMean ()) value2 = mean;
    else if (isValueSum  ()) value2 = sum;

    auto brect = CQChartsGeom::makeDirBBox(isHorizontal(), bx, value1, bx + 1.0, value2);

    //---

    const auto &ivalue    = valueSet.value(0);
    const auto &valueInds = ivalue.valueInds();

    BarValue::ValueInd ind;

    if (! valueInds.empty())
      ind = valueInds[0];

    //---

    auto *barObj = createBarObj(brect, /*valueSet*/true, iv,
                                ColorInd(), ColorInd(iv, nv), ColorInd(), ind.ind);

    connect(barObj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

    objs.push_back(barObj);

    return true;
  };

  auto addValueBarObj = [&](int groupInd, const BarValue &ivalue, int iv, int ivs, int nvs,
                            double scale, double bx1, double bw1, double &lastPosValue,
                            double &lastNegValue) {
    BarValue::ValueInd minInd, maxInd;
    double             mean = 0.0, sum = 0.0;

    ivalue.calcRange(minInd, maxInd, mean, sum);

    QModelIndex parent; // TODO

    Color  color;
    QColor c;

    if (colorColumnColor(minInd.vrow, parent, color))
      c = interpColor(color, ColorInd());

    if (! colorVisible(c))
      return false;

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
        return false;
    }

    if (isStacked())
      brect = CQChartsGeom::makeDirBBox(isHorizontal(), bx, value1, bx + 1.0, value2);
    else
      brect = CQChartsGeom::makeDirBBox(isHorizontal(), bx1, value1, bx1 + bw1, value2);

    if (isVertical())
      barWidth_ = std::min(barWidth_, brect.getWidth());
    else
      barWidth_ = std::min(barWidth_, brect.getHeight());

    ColorInd cis, cig, civ;

    if      (ns > 1) {
      // multiple sets:
      //  . set per value column (ns == nvs)
      //  . group per group column unique value
      if (isGroupByColumn()) {
        cis = ColorInd(iv, nv);
        cig = ColorInd(iv, nv);
        civ = ColorInd(ivs, nvs);
      }
      // multiple sets:
      //  . set per group unique index
      //  . group per group column unique value
      else {
        cis = ColorInd(groupInd, ng);
        cig = ColorInd(iv, nv);
        civ = ColorInd(ivs, nvs);
      }
    }
    else if (nv > 1) {
      // single set, multiple groups :
      cis = ColorInd(0, 1);
      cig = ColorInd(iv, nv);
      civ = ColorInd(ivs, nvs);
    }
    else {
      // single set, single group:
      //  . group per group column unique value
      //  . value per grouped values
      cis = ColorInd(0, 1);
      cig = ColorInd(0, 1);
      civ = ColorInd(ivs, nvs);
    }

    auto *barObj = createBarObj(brect, /*valueSet*/false, iv, cis, cig, civ, minInd.ind);

    connect(barObj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

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

    return true;
  };

  //---

  barWidth_ = 1.0;

  // per group (column, or group column unique value)
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
    if (! isValueValue() && (ns == 1 || isGroupByColumn())) {
      if (! addValueSetBarObj(valueSet, iv)) {
        if (isSkipEmpty())
          continue; // no inc bx
      }
    }
    else {
      // NOTE: summary values (min, max, mean, range, sum are across the multiple value columns)
      double lastPosValue = 0.0, lastNegValue = 0.0;

      // per value
      for (int ivs = 0; ivs < nvs; ++ivs) {
        if (isValueHidden(ivs))
          continue;

        //---

        const auto &ivalue = valueSet.value(ivs);

        int groupInd = valueSet.groupInd();

        if (! addValueBarObj(groupInd, ivalue, iv, ivs, nvs, scale, bx1, bw1,
                             lastPosValue, lastNegValue))
          continue;

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

  int ns = calcNumSets();
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
      // no group, multiple columns, value type (bar per column)
      if (isValueValue()) {
        for (int iv = 0; iv < nv; ++iv) {
          if (isSetHidden(iv))
            continue;

          const auto &valueSet = this->valueSet(iv);

          xAxis->setTickLabel(numVisible, valueSet.name());

          ++numVisible;
        }
      }
      else {
        for (int iv = 0; iv < nv; ++iv) {
          if (isSetHidden(iv))
            continue;

          const auto &valueSet = this->valueSet(iv);

          // skip empty value sets (TODO: all value types ?)
          if (valueSet.numValues() == 0 && isSkipEmpty())
            continue;

          xAxis->setTickLabel(numVisible, valueSet.name());

          ++numVisible;
        }
      }
    }
    else if (nv == 1) {
      // no group, single column, value type (bar per value)
      if (isValueValue()) {
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
      else {
        auto name = this->valueName();

        xAxis->setTickLabel(0, name);
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

    auto *groupItem = new CQChartsGroupKeyItem(this);

    groupItem->addRowItems(colorItem, textItem);

    if (c.isValid())
      colorItem->setColor(Color(c));

    key->addItem(groupItem, row, col);

    key->nextRowCol(row, col);
  };

  //---

  int ns = calcNumSets();
  int nv = numValueSets();

  if (ns > 1) {
    // ivs is 'value set' index (group)
    // iv is 'value set' value index
    if (isColorBySet()) {
      for (int iv = 0; iv < nv; ++iv) {
        const auto &valueSet = this->valueSet(iv);

        auto name = valueSet.name();

        addKeyRow(ColorInd(iv, nv), ColorInd(iv, nv), ColorInd(), name);
      }
    }
    else {
      // color by column
      if (isGroupByColumn()) {
        for (int iv = 0; iv < nv; ++iv) {
          const auto &valueSet = this->valueSet(iv);

          auto name = valueSet.name();

          addKeyRow(ColorInd(iv, nv), ColorInd(iv, nv), ColorInd(), name);
        }
      }
      // color by group unique value
      else {
        int ng = numGroups();

        for (int ig = 0; ig < ng; ++ig) {
          auto name = groupIndName(ig);

          addKeyRow(ColorInd(), ColorInd(ig, ng), ColorInd(), name);
        }
      }
    }
  }
  else {
    if      (nv > 1) {
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

          if (! colorVisible(c))
            continue;
        }

        auto name = valueSet.name();

        addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), name, c);
      }
    }
    else if (nv == 1) {
      // no group, single column, value type (bar per value)
      if (isValueValue()) {
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

          if (! colorVisible(c))
            continue;

          auto iname = ivalue.valueName();

          if (iname == "")
            iname = CQChartsUtil::realToString(ind0.value);

          addKeyRow(ColorInd(), ColorInd(), ColorInd(ivs, nvs), iname, c);
        }
      }
      else {
        auto name = this->valueName();

        addKeyRow(ColorInd(), ColorInd(), ColorInd(0, 1), name);
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

          if (! colorVisible(c))
            continue;
        }

        auto name = valueSet.name();

        addKeyRow(ColorInd(), ColorInd(iv, nv), ColorInd(), name, c);
      }
    }
    else {
      auto title = this->titleStr();

      if (! title.length()) {
        bool ok;

        auto yname = modelHHeaderString(valueColumns.column(), ok);

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
  int ns = calcNumSets();
  int nv = numValueSets();

  // if multiple sets then set per column
  if      (ns > 1) {
    if (CQChartsPlot::isSetHidden(i))
      return true;

    return false;
  }
  // if single set then set per value set
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
  int ns = calcNumSets();
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
addMenuItems(QMenu *menu, const Point &)
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

#if 0
  auto *shapeMenu = new QMenu("Shape Type", menu);

  (void) addMenuCheckedAction(shapeMenu, "Rect"    , isRect   (), SLOT(setRect(bool)));
  (void) addMenuCheckedAction(shapeMenu, "Dot Line", isDotLine(), SLOT(setDotLine(bool)));
  (void) addMenuCheckedAction(shapeMenu, "Box"     , isBox    (), SLOT(setBox(bool)));
  (void) addMenuCheckedAction(shapeMenu, "Scatter" , isScatter(), SLOT(setScatter(bool)));
  (void) addMenuCheckedAction(shapeMenu, "Violin"  , isViolin (), SLOT(setViolin(bool))).

  menu->addMenu(shapeMenu);
#endif

  (void) addCheckedAction("Percent", isPercent(), SLOT(setPercent(bool)));

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
createBarObj(const BBox &rect, bool valueSet, int valueSetInd, const ColorInd &is,
             const ColorInd &ig, const ColorInd &iv, const QModelIndex &ind) const
{
  return new CQChartsBarChartObj(this, rect, valueSet, valueSetInd, is, ig, iv, ind);
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
CQChartsBarChartObj(const CQChartsBarChartPlot *barChartPlot, const BBox &rect, bool valueSet,
                    int valueSetInd, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                    const QModelIndex &ind) :
 CQChartsPlotObj(const_cast<CQChartsBarChartPlot *>(barChartPlot), rect, is, ig, iv),
 barChartPlot_(barChartPlot), valueSet_(valueSet), valueSetInd_(valueSetInd)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
    setModelInd(ind);
}

CQChartsBarChartObj::
~CQChartsBarChartObj()
{
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
        ModelIndex columnInd(barChartPlot_, modelInd().row(), column, modelInd().parent());

        bool ok;

        value1 = barChartPlot_->modelString(columnInd, ok);
        if (! ok) return;
      }
    }

    if (! value1.length())
      return;

    auto headerStr = header;

    if (column.isValid()) {
      headerStr = barChartPlot_->columnHeaderName(column, /*tip*/true);

      if (headerStr == "")
        headerStr = header;
    }

    tableTip.addTableRow(headerStr, value1);

    if (column.isValid())
      tableTip.addColumn(column);
  };

  auto addOptColumnsRow = [&](const CQChartsColumns &columns, const QString &header,
                              const QString &value) {
    if (columns.isValid() && tableTip.hasColumns(columns))
      return;

    if (! value.length())
      return;

    auto headerStr = header;

    if (columns.isValid()) {
      headerStr = barChartPlot_->columnsHeaderName(columns, /*tip*/true);

      if (headerStr == "")
        headerStr = header;
    }

    tableTip.addTableRow(headerStr, value);

    if (columns.isValid())
      tableTip.addColumns(columns);
  };

  //---

  auto valueColumns = barChartPlot_->calcValueColumns();

  addOptColumnRow (barChartPlot_->groupColumn (), "Group", this->groupStr());
  addOptColumnRow (barChartPlot_->nameColumn  (), "Name" , this->nameStr ());
  addOptColumnsRow(valueColumns         , "Value", this->valueStr());
  addOptColumnRow (barChartPlot_->colorColumn (), "Color");

  //---

  const auto *value = this->value();

  for (const auto &pcv : value->nameColumnValueMap()) {
    const auto &column = pcv.second.column;

    if (column.isValid() && tableTip.hasColumn(column))
      continue;

    const auto &name   = pcv.first;
    const auto &value1 = pcv.second.value;

    if (value1.length())
      tableTip.addTableRow(name, value1);

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

    if      (barChartPlot_->isValueRange()) {
      auto minValueStr = barChartPlot_->valueStr(min);
      auto maxValueStr = barChartPlot_->valueStr(max);

      valueStr = QString("%1-%2").arg(minValueStr).arg(maxValueStr);
    }
    else if (barChartPlot_->isValueMin())
      valueStr = barChartPlot_->valueStr(min);
    else if (barChartPlot_->isValueMax())
      valueStr = barChartPlot_->valueStr(max);
    else if (barChartPlot_->isValueMean())
      valueStr = barChartPlot_->valueStr(mean);
    else if (barChartPlot_->isValueSum())
      valueStr = barChartPlot_->valueStr(sum);
  }
  else {
    const auto *value = this->value();

    BarValue::ValueInd minInd, maxInd;
    double             mean = 0.0, sum = 0.0;

    value->calcRange(minInd, maxInd, mean, sum);

    if      (barChartPlot_->isValueValue()) {
      valueStr = barChartPlot_->valueStr(minInd.value);
    }
    else if (barChartPlot_->isValueRange()) {
      auto minValueStr = barChartPlot_->valueStr(minInd.value);
      auto maxValueStr = barChartPlot_->valueStr(maxInd.value);

      valueStr = QString("%1-%2").arg(minValueStr).arg(maxValueStr);
    }
    else if (barChartPlot_->isValueMin())
      valueStr = barChartPlot_->valueStr(minInd.value);
    else if (barChartPlot_->isValueMax())
      valueStr = barChartPlot_->valueStr(maxInd.value);
    else if (barChartPlot_->isValueMean())
      valueStr = barChartPlot_->valueStr(mean);
    else if (barChartPlot_->isValueSum())
      valueStr = barChartPlot_->valueStr(sum);
  }

  return valueStr;
}

CQChartsGeom::BBox
CQChartsBarChartObj::
dataLabelRect() const
{
  if (! barChartPlot_->isLabelsVisible())
    return BBox();

  const auto *value = this->value();

  auto label = value->getNameValue("Label");

  if (! barChartPlot_->labelColumn().isValid()) {
    const auto &valueInds = value->valueInds();
    assert(! valueInds.empty());

    double value1 = valueInds[0].value;

    label = barChartPlot_->valueStr(value1);
  }

  return barChartPlot_->dataLabel()->calcRect(rect(), label);
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
  auto valueColumns = barChartPlot_->calcValueColumns();

  addColumnSelectIndex(inds, barChartPlot_->groupColumn());

  int nv = valueColumns.count();

  if (barChartPlot_->isStacked()) {
    if (ig_.i < nv)
      addColumnSelectIndex(inds, valueColumns.getColumn(ig_.i));
  }
  else {
    if (is_.i < nv)
      addColumnSelectIndex(inds, valueColumns.getColumn(is_.i));
  }

  addColumnSelectIndex(inds, barChartPlot_->nameColumn());
  addColumnSelectIndex(inds, barChartPlot_->labelColumn());
}

#if 0
bool
CQChartsBarChartObj::
isHidden() const
{
  if (is_.n > 1)
    return barChartPlot_->isSetHidden(is_.i);
  else
    return barChartPlot_->isSetHidden(ig_.i);
}
#endif

void
CQChartsBarChartObj::
draw(PaintDevice *device) const
{
  // calc bar borders
  double m1 = barChartPlot_->lengthPixelSize(barChartPlot_->margin(), barChartPlot_->isVertical());
  double m2 = m1;

  if (! barChartPlot_->isStacked()) {
    if      (is_.n > 1) {
      if      (ig_.i == 0)
        m1 = barChartPlot_->lengthPixelSize(barChartPlot_->groupMargin(),
                                            barChartPlot_->isVertical());
      else if (ig_.i == ig_.n - 1)
        m2 = barChartPlot_->lengthPixelSize(barChartPlot_->groupMargin(),
                                            barChartPlot_->isVertical());
    }
    else if (ig_.n > 1) {
      if      (iv_.i == 0)
        m1 = barChartPlot_->lengthPixelSize(barChartPlot_->groupMargin(),
                                            barChartPlot_->isVertical());
      else if (iv_.i == iv_.n - 1)
        m2 = barChartPlot_->lengthPixelSize(barChartPlot_->groupMargin(),
                                            barChartPlot_->isVertical());
    }
  }

  //---

  // adjust border sizes and rect
  static double minSize = 3.0;

  auto prect = barChartPlot_->windowToPixel(rect());

  double rs = prect.getSize(barChartPlot_->isVertical());

  double s1 = rs - 2*m1;

  if (s1 < minSize) {
    m1 = (rs - minSize)/2.0;
    m2 = m1;
  }

  prect.expandExtent(-m1, -m2, barChartPlot_->isVertical());

  auto rect = barChartPlot_->pixelToWindow(prect);

  //---

  drawShape(device, rect);
}

void
CQChartsBarChartObj::
drawShape(PaintDevice *device, const BBox &bbox) const
{
  // calc pen and brush
  PenBrush barPenBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(barPenBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, barPenBrush);

  //---

  if      (barChartPlot_->shapeType() == CQChartsBarChartPlot::ShapeType::DOT_LINE) {
    drawDotLine(device, bbox, barPenBrush);
  }
  else if (barChartPlot_->shapeType() == CQChartsBarChartPlot::ShapeType::BOX) {
    drawBox(device, bbox);
  }
  else if (barChartPlot_->shapeType() == CQChartsBarChartPlot::ShapeType::SCATTER) {
    drawScatter(device, bbox);
  }
  else if (barChartPlot_->shapeType() == CQChartsBarChartPlot::ShapeType::VIOLIN) {
    drawViolin(device, bbox);
  }
  else {
    drawRect(device, bbox, barPenBrush);
  }

  //---

  device->resetColorNames();
}

void
CQChartsBarChartObj::
drawRect(PaintDevice *device, const BBox &bbox, const CQChartsPenBrush &barPenBrush) const
{
  // draw rect (TODO: line for thin bar)
  CQChartsDrawUtil::drawRoundedRect(device, barPenBrush, bbox, barChartPlot_->barCornerSize());
}

void
CQChartsBarChartObj::
drawDotLine(PaintDevice *device, const BBox &bbox, const PenBrush &barPenBrush) const
{
  // set dot pen and brush
  auto ic = (ig_.n > 1 ? ig_ : iv_);

  PenBrush dotPenBrush;

  barChartPlot_->setDotSymbolPenBrush(dotPenBrush, ic);

  //---

  CQChartsDrawUtil::drawDotLine(device, barPenBrush, bbox, barChartPlot_->dotLineWidth(),
                                barChartPlot_->isHorizontal(), barChartPlot_->dotSymbol(),
                                barChartPlot_->dotSymbolSize(), dotPenBrush);
}

void
CQChartsBarChartObj::
drawBox(PaintDevice *device, const BBox &bbox) const
{
  if (! density_) {
    density_ = std::make_unique<CQChartsDensity>();

    density_->setOrientation(barChartPlot_->isVertical() ? Qt::Vertical : Qt::Horizontal);
  }

  std::vector<double> rvalues;
  barChartPlot_->valueSet(valueSetInd_).getValues(rvalues);

  density_->setXVals(rvalues);

  density_->setDrawType(CQChartsDensity::DrawType::WHISKER);

  CQChartsDensity::DrawData drawData;

  drawData.scaled = true;

  density_->draw(plot(), device, bbox, drawData);
}

void
CQChartsBarChartObj::
drawScatter(PaintDevice *device, const BBox &bbox) const
{
  if (! density_) {
    density_ = std::make_unique<CQChartsDensity>();

    density_->setOrientation(barChartPlot_->isVertical() ? Qt::Vertical : Qt::Horizontal);
  }

  std::vector<double> rvalues;
  barChartPlot_->valueSet(valueSetInd_).getValues(rvalues);

  density_->setXVals(rvalues);

  density_->setDrawType(CQChartsDensity::DrawType::SCATTER);

  CQChartsDensity::DrawData drawData;

  drawData.scaled = true;

  density_->draw(plot(), device, bbox, drawData);
}

void
CQChartsBarChartObj::
drawViolin(PaintDevice *device, const BBox &bbox) const
{
  if (! density_) {
    density_ = std::make_unique<CQChartsDensity>();

    density_->setOrientation(barChartPlot_->isVertical() ? Qt::Vertical : Qt::Horizontal);
  }

  std::vector<double> rvalues;
  barChartPlot_->valueSet(valueSetInd_).getValues(rvalues);

  density_->setXVals(rvalues);

  density_->setDrawType(CQChartsDensity::DrawType::VIOLIN);

  CQChartsDensity::DrawData drawData;

  drawData.scaled = true;

  density_->draw(plot(), device, bbox, drawData);
}

void
CQChartsBarChartObj::
drawFg(PaintDevice *device) const
{
  // draw data label on foreground layers
  if (! barChartPlot_->isLabelsVisible())
    return;

  //---

  QString minLabel, maxLabel;
  double  minValue = 0;

  // calc min/max value
  if (barChartPlot_->isValueValue()) {
    const auto *value = this->value();

    minLabel = value->getNameValue("Label");
    maxLabel = minLabel;

    BarValue::ValueInd minInd, maxInd;
    double             mean = 0.0, sum = 0.0;

    if (! barChartPlot_->labelColumn().isValid()) {
      value->calcRange(minInd, maxInd, mean, sum);

      double scale = 1.0;

      if (barChartPlot_->isPercent()) {
        const auto *valueSet = this->valueSet();

        double posSum = 0.0, negSum = 0.0;

        valueSet->calcSums(posSum, negSum);

        double total = posSum - negSum;

        scale = (total > 0.0 ? 100.0/total : 1.0);
      }

      minLabel = barChartPlot_->valueStr(scale*minInd.value);
      maxLabel = barChartPlot_->valueStr(scale*maxInd.value);
    }

    minValue = minInd.value;
  }
  else {
    const auto *valueSet = this->valueSet();

    double min = 0.0, max = 0.0, mean = 0.0, sum = 0.0;

    (void) valueSet->calcStats(min, max, mean, sum);

    if      (barChartPlot_->isValueRange()) {
      minLabel = barChartPlot_->valueStr(min);
      maxLabel = barChartPlot_->valueStr(max);
    }
    else if (barChartPlot_->isValueMin())
      minLabel = barChartPlot_->valueStr(min);
    else if (barChartPlot_->isValueMax())
      minLabel = barChartPlot_->valueStr(max);
    else if (barChartPlot_->isValueMean())
      minLabel = barChartPlot_->valueStr(mean);
    else if (barChartPlot_->isValueSum())
      minLabel = barChartPlot_->valueStr(sum);

    if (maxLabel == "")
      maxLabel = minLabel;

    minValue = min;
  }

  //---

  if (! barChartPlot_->dataLabel()->isPositionOutside()) {
    PenBrush barPenBrush;

    calcPenBrush(barPenBrush, /*updateState*/false);

    barChartPlot_->charts()->setContrastColor(barPenBrush.brush.color());
  }
  else {
    barChartPlot_->charts()->setContrastColor(barChartPlot_->plotBgColor());
  }

  //---

  if (minLabel == maxLabel) {
    auto pos = barChartPlot_->labelPosition();

    if (! barChartPlot_->labelColumn().isValid() && minValue < 0)
      pos = CQChartsDataLabel::flipPosition(pos);

    if (minLabel != "")
      barChartPlot_->dataLabel()->draw(device, rect(), minLabel,
                                       static_cast<CQChartsDataLabel::Position>(pos));
  }
  else {
    if (barChartPlot_->dataLabel()->isPositionOutside()) {
      auto minPos = CQChartsDataLabel::Position::BOTTOM_OUTSIDE;
      auto maxPos = CQChartsDataLabel::Position::TOP_OUTSIDE;

      barChartPlot_->dataLabel()->draw(device, rect(), minLabel, minPos);
      barChartPlot_->dataLabel()->draw(device, rect(), maxLabel, maxPos);
    }
    else {
      auto minPos = CQChartsDataLabel::Position::BOTTOM_INSIDE;
      auto maxPos = CQChartsDataLabel::Position::TOP_INSIDE;

      barChartPlot_->dataLabel()->draw(device, rect(), minLabel, minPos);
      barChartPlot_->dataLabel()->draw(device, rect(), maxLabel, maxPos);
    }
  }

  //---

  barChartPlot_->charts()->resetContrastColor();
}

void
CQChartsBarChartObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  static double minBorderSize = 5.0;

  auto prect = barChartPlot_->windowToPixel(rect());

  double rs = prect.getSize(barChartPlot_->isVertical());

  bool skipBorder = (rs < minBorderSize);

  //---

  auto colorInd = calcColorInd();

  auto bc = barChartPlot_->interpBarStrokeColor(colorInd);

  auto barColor = calcBarColor();

  auto penData = barChartPlot_->barPenData(bc);

  if (skipBorder)
    penData.setVisible(false);

  barChartPlot_->setPenBrush(penBrush, penData, barChartPlot_->barBrushData(barColor));

  // adjust pen/brush for selected/mouse over
  if (updateState)
    barChartPlot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsBarChartObj::
calcBarColor() const
{
  // calc bar color
  auto colorInd = calcColorInd();

  QColor barColor;

  if (barChartPlot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    int ns = barChartPlot_->calcNumSets();

    if (ns > 1) {
      if (isValueSet()) {
        barColor = barChartPlot_->interpBarFillColor(ig_);
      }
      else {
        if (barChartPlot_->isColorBySet())
          barColor = barChartPlot_->interpBarFillColor(ig_);
        else
          barColor = barChartPlot_->interpBarFillColor(is_);
      }
    }
    else {
      if (! color_.isValid()) {
        if      (ig_.n > 1)
          barColor = barChartPlot_->interpBarFillColor(ig_);
        else if (iv_.n > 1)
          barColor = barChartPlot_->interpBarFillColor(iv_);
        else {
          ColorInd ig1(ig_.i    , ig_.n + 1);
          ColorInd ig2(ig_.i + 1, ig_.n + 1);

          auto barColor1 = barChartPlot_->interpBarFillColor(ig1);
          auto barColor2 = barChartPlot_->interpBarFillColor(ig2);

          barColor = CQChartsUtil::blendColors(barColor1, barColor2, iv_.value());
        }
      }
      else {
        barColor = barChartPlot_->interpColor(color_, colorInd);
      }
    }
  }
  else {
    barColor = barChartPlot_->interpBarFillColor(colorInd);
  }

  return barColor;
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
    const auto &valueSet = barChartPlot_->valueSet(ig_.i);

    return &valueSet;
  }
  else {
    const auto &valueSet = barChartPlot_->valueSet(ig_.i);

    return &valueSet;
  }
}

//------

CQChartsBarColorKeyItem::
CQChartsBarColorKeyItem(BarChartPlot *barChartPlot, const QString &name, const ColorInd &is,
                        const ColorInd &ig, const ColorInd &iv) :
 CQChartsColorBoxKeyItem(barChartPlot, is, ig, iv), barChartPlot_(barChartPlot), name_(name)
{
  setClickable(true);
}

#if 0
bool
CQChartsBarColorKeyItem::
selectPress(const Point &, SelData &selData)
{
  if (selData.selMod == SelMod::ADD) {
    for (int i = 0; i < iv_.n; ++i) {
      barChartPlot_->CQChartsPlot::setSetHidden(i, i != iv_.i);
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

  if (barChartPlot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    int ns = barChartPlot_->calcNumSets();

    if (ns > 1) {
      if (barChartPlot_->isColorBySet()) {
        if (ig_.n > 1)
          barColor = barChartPlot_->interpBarFillColor(ig_);
        else
          barColor = barChartPlot_->interpBarFillColor(iv_);
      }
      else {
        if      (is_.n > 1)
          barColor = barChartPlot_->interpBarFillColor(is_);
        else if (ig_.n > 1)
          barColor = barChartPlot_->interpBarFillColor(ig_);
        else
          barColor = barChartPlot_->interpBarFillColor(iv_);
      }
    }
    else {
      if (! color_.isValid()) {
        if      (ig_.n > 1)
          barColor = barChartPlot_->interpBarFillColor(ig_);
        else if (iv_.n > 1)
          barColor = barChartPlot_->interpBarFillColor(iv_);
        else {
          ColorInd ig1(ig_.i    , ig_.n + 1);
          ColorInd ig2(ig_.i + 1, ig_.n + 1);

          auto barColor1 = barChartPlot_->interpBarFillColor(ig1);
          auto barColor2 = barChartPlot_->interpBarFillColor(ig2);

          barColor = CQChartsUtil::blendColors(barColor1, barColor2, iv_.value());
        }
      }
      else {
        barColor = barChartPlot_->interpColor(color_, colorInd);
      }
    }
  }
  else {
    barColor = barChartPlot_->interpBarFillColor(colorInd);
  }

  //---

  adjustFillColor(barColor);

  QBrush brush;

  auto barBrushData = barChartPlot_->barBrushData(barColor);

  CQChartsDrawUtil::setBrush(brush, barBrushData);

  return brush;
}

QPen
CQChartsBarColorKeyItem::
strokePen() const
{
  auto colorInd = calcColorInd();

  auto bc = barChartPlot_->interpBarStrokeColor(colorInd);

  auto penData = barChartPlot_->barPenData(bc);

  PenBrush penBrush;

  barChartPlot_->setPen(penBrush, penData);

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

  int ns = barChartPlot_->calcNumSets();
  int nv = barChartPlot_->numValueSets();

  if (ns > 1) {
    if (barChartPlot_->isColorBySet()) {
      const auto &valueSet = barChartPlot_->valueSet(iv_.i);

      valueSet.calcSums(posSum, negSum);

      count = valueSet.numValues();
    }
    else {
      count = barChartPlot_->numSetValues();

      for (int i = 0; i < nv; ++i) {
        const auto &valueSet = barChartPlot_->valueSet(i);

        const auto &ivalue = valueSet.value(iv_.i);

        const auto &valueInds = ivalue.valueInds();
        assert(! valueInds.empty());

        double value1 = valueInds[0].value;

        if (value1 >= 0) posSum += value1;
        else             negSum += value1;
      }
    }
  }
  else {
    if      (nv > 1) {
      const auto &valueSet = barChartPlot_->valueSet(iv_.i);

      valueSet.calcSums(posSum, negSum);

      count = valueSet.numValues();
    }
    else if (nv == 1) {
      const auto &valueSet = barChartPlot_->valueSet(0);

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
    return barChartPlot_->CQChartsPlot::isSetHidden(is_.i);
  else if (ig_.n > 1)
    return barChartPlot_->CQChartsPlot::isSetHidden(ig_.i);
  else if (iv_.n > 1)
    return barChartPlot_->CQChartsPlot::isSetHidden(iv_.i);
  else
    return false;
}

void
CQChartsBarColorKeyItem::
setSetHidden(bool b)
{
  if      (is_.n > 1)
    barChartPlot_->CQChartsPlot::setSetHidden(is_.i, b);
  else if (ig_.n > 1)
    barChartPlot_->CQChartsPlot::setSetHidden(ig_.i, b);
  else if (iv_.n > 1)
    barChartPlot_->CQChartsPlot::setSetHidden(iv_.i, b);
}
#endif

//------

CQChartsBarTextKeyItem::
CQChartsBarTextKeyItem(BarChartPlot *barChartPlot, const QString &text, const ColorInd &ic) :
 CQChartsTextKeyItem(barChartPlot, text, ic)
{
}

QColor
CQChartsBarTextKeyItem::
interpTextColor(const ColorInd &ind) const
{
  auto c = CQChartsTextKeyItem::interpTextColor(ind);

  //adjustFillColor(c);

  return c;
}

#if 0
bool
CQChartsBarTextKeyItem::
isSetHidden() const
{
  return barChartPlot_->CQChartsPlot::isSetHidden(ic_.i);
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
  addColumnWidgets();

  addLabelGroup();

  addGroupColumnWidgets();

  addOptionsWidgets();

  addColorColumnWidgets();

  addKeyList();
}

void
CQChartsBarChartPlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  // values, name and label columns
  addNamedColumnWidgets(QStringList() << "values" << "name", columnsFrame);
}

void
CQChartsBarChartPlotCustomControls::
addLabelGroup()
{
  // label group
  labelFrame_ = createGroupFrame("Bar Label", "labelFrame");

  auto *labelCornerFrame  = CQUtil::makeWidget<QFrame>("labelCornerFrame");
  auto *labelCornerLayout = CQUtil::makeLayout<QHBoxLayout>(labelCornerFrame, 0, 2);

  labelCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Visible", "labelVisible");

  labelCheck_->setToolTip("Show Bar Label");

  labelCornerLayout->addWidget(labelCheck_);

  labelColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("labelColumnCombo");

  labelColumnCombo_->setToolTip("Column to use for bar data label");

  labelFrame_.groupBox->setCornerWidget(labelCornerFrame);

  addFrameWidget(labelFrame_, "Column", labelColumnCombo_);
}

void
CQChartsBarChartPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  orientationCombo_ = createEnumEdit("orientation");
  plotTypeCombo_    = createEnumEdit("plotType");
  valueTypeCombo_   = createEnumEdit("valueType");
  shapeTypeCombo_   = createEnumEdit("shapeType");

  addFrameWidget(optionsFrame_, "Orientation", orientationCombo_);
  addFrameWidget(optionsFrame_, "Plot Type"  , plotTypeCombo_);
  addFrameWidget(optionsFrame_, "Value Type" , valueTypeCombo_);
  addFrameWidget(optionsFrame_, "Shape Type" , shapeTypeCombo_);

  //---

  groupByColumnCheck_ = createBoolEdit("groupByColumn");

  addFrameWidget(optionsFrame_, "Group By Column", groupByColumnCheck_);

  colorBySetCheck_ = createBoolEdit("colorBySet");

  addFrameWidget(optionsFrame_, "Color By Set", colorBySetCheck_);

  //---

  auto *optionsFrame1  = CQUtil::makeWidget<QFrame>("optionsFrame1");
  auto *optionsLayout1 = CQUtil::makeLayout<QGridLayout>(optionsFrame1, 2, 2);

  addFrameWidget(optionsFrame_, optionsFrame1);

  percentCheck_   = createBoolEdit("percent"  , /*choice*/false);
  skipEmptyCheck_ = createBoolEdit("skipEmpty", /*choice*/false);

  optionsLayout1->addWidget(percentCheck_  , 0, 0);
  optionsLayout1->addWidget(skipEmptyCheck_, 0, 1);

  optionsLayout1->setColumnStretch(2, 1);
}

void
CQChartsBarChartPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  barChartPlot_ = dynamic_cast<CQChartsBarChartPlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);
}

void
CQChartsBarChartPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  if (labelColumnCombo_)
    labelColumnCombo_->setModelColumn(barChartPlot_->getModelData(), barChartPlot_->labelColumn());

  if (labelCheck_) labelCheck_->setChecked(barChartPlot_->isLabelsVisible());

  //---

  if (orientationCombo_)
    orientationCombo_->setCurrentValue(static_cast<int>(barChartPlot_->orientation()));
  if (plotTypeCombo_   )
    plotTypeCombo_   ->setCurrentValue(static_cast<int>(barChartPlot_->plotType()));
  if (valueTypeCombo_  )
    valueTypeCombo_  ->setCurrentValue(static_cast<int>(barChartPlot_->valueType()));
  if (shapeTypeCombo_  )
    shapeTypeCombo_  ->setCurrentValue(static_cast<int>(barChartPlot_->shapeType()));

  //---

  if (percentCheck_      ) percentCheck_      ->setChecked(barChartPlot_->isPercent());
  if (skipEmptyCheck_    ) skipEmptyCheck_    ->setChecked(barChartPlot_->isSkipEmpty());
  if (groupByColumnCheck_) groupByColumnCheck_->setChecked(barChartPlot_->isGroupByColumn());
  if (colorBySetCheck_   ) colorBySetCheck_   ->setChecked(barChartPlot_->isColorBySet());

  //---

  CQChartsGroupPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsBarChartPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::optConnectDisconnect(b,
    labelColumnCombo_, SIGNAL(columnChanged()), this, SLOT(labelColumnSlot()));

  CQChartsWidgetUtil::optConnectDisconnect(b,
    labelCheck_, SIGNAL(stateChanged(int)), this, SLOT(labelVisibleSlot(int)));

  CQChartsWidgetUtil::optConnectDisconnect(b,
    orientationCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationSlot()));
  CQChartsWidgetUtil::optConnectDisconnect(b,
    plotTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotTypeSlot()));
  CQChartsWidgetUtil::optConnectDisconnect(b,
    valueTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(valueTypeSlot()));
  CQChartsWidgetUtil::optConnectDisconnect(b,
    shapeTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(shapeTypeSlot()));

  CQChartsWidgetUtil::optConnectDisconnect(b,
    percentCheck_, SIGNAL(stateChanged(int)), this, SLOT(percentSlot()));
  CQChartsWidgetUtil::optConnectDisconnect(b,
    skipEmptyCheck_, SIGNAL(stateChanged(int)), this, SLOT(skipEmptySlot()));
  CQChartsWidgetUtil::optConnectDisconnect(b,
    groupByColumnCheck_, SIGNAL(stateChanged(int)), this, SLOT(groupByColumnSlot()));
  CQChartsWidgetUtil::optConnectDisconnect(b,
    colorBySetCheck_, SIGNAL(stateChanged(int)), this, SLOT(colorBySetSlot()));

  CQChartsGroupPlotCustomControls::connectSlots(b);
}

//---

void
CQChartsBarChartPlotCustomControls::
labelColumnSlot()
{
  barChartPlot_->setLabelColumn(labelColumnCombo_->getColumn());
}

void
CQChartsBarChartPlotCustomControls::
labelVisibleSlot(int b)
{
  barChartPlot_->setLabelsVisible(b);
}

void
CQChartsBarChartPlotCustomControls::
orientationSlot()
{
  barChartPlot_->setOrientation(static_cast<Qt::Orientation>(orientationCombo_->currentValue()));
}

void
CQChartsBarChartPlotCustomControls::
plotTypeSlot()
{
  barChartPlot_->setPlotType(
    static_cast<CQChartsBarChartPlot::PlotType>(plotTypeCombo_->currentValue()));
}

void
CQChartsBarChartPlotCustomControls::
valueTypeSlot()
{
  barChartPlot_->setValueType(
    static_cast<CQChartsBarChartPlot::ValueType>(valueTypeCombo_->currentValue()));
}

void
CQChartsBarChartPlotCustomControls::
shapeTypeSlot()
{
  barChartPlot_->setShapeType(
    static_cast<CQChartsBarChartPlot::ShapeType>(shapeTypeCombo_->currentValue()));
}

void
CQChartsBarChartPlotCustomControls::
percentSlot()
{
  barChartPlot_->setPercent(percentCheck_->isChecked());
}

void
CQChartsBarChartPlotCustomControls::
skipEmptySlot()
{
  barChartPlot_->setSkipEmpty(skipEmptyCheck_->isChecked());
}

void
CQChartsBarChartPlotCustomControls::
groupByColumnSlot()
{
  barChartPlot_->setGroupByColumn(groupByColumnCheck_->isChecked());
}

void
CQChartsBarChartPlotCustomControls::
colorBySetSlot()
{
  barChartPlot_->setColorBySet(colorBySetCheck_->isChecked());
}

CQChartsColor
CQChartsBarChartPlotCustomControls::
getColorValue()
{
  return barChartPlot_->barFillColor();
}

void
CQChartsBarChartPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  barChartPlot_->setBarFillColor(c);
}
