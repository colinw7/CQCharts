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
  startParameterGroup("Bar Chart");

  // values
  addColumnsParameter("value", "Value", "valueColumns", "1").
    setRequired().setTip("Value column(s)");

  addColumnParameter("name" , "Name" , "nameColumn" ).setTip("Custom group name");
  addColumnParameter("label", "Label", "labelColumn").setTip("Bar data label");
  addColumnParameter("color", "Color", "colorColumn").setTip("Custom bar color");

  addBoolParameter("horizontal", "Horizontal"  , "horizontal").setTip("draw bars horizontal");
  addBoolParameter("colorBySet", "Color by Set", "colorBySet").setTip("Color by value set");
  addBoolParameter("stacked"   , "Stacked"     , "stacked"   ).setTip("Stack grouped values");
  addBoolParameter("percent"   , "Percent"     , "percent"   ).setTip("Show value is percentage");
  addBoolParameter("rangeBar"  , "Range Bar"   , "rangeBar"  ).setTip("show value range in bar");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsBarChartPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws bars with heights from a set of values.</p>\n"
         "<h2>Columns</h2>\n"
         "<p>The bar heights are taken from the values in the <b>Value</b> columns.</p>\n"
         "<p>Bars can be grouped using standard grouping controls so that related values "
         "can be placed next to each other.</p>\n"
         "<p>An optional name can be supplied in the <b>Name</b> column to specify the label "
         "to use on the axis below the bar.</p>\n"
         "<p>An optional label can be drawn with the bar to show extra values using "
         "the <b>Label</b> column.</p>\n"
         "<p>The color of the bar can be customized using the <b>Color</b> column.</p>\n"
         "<p>A custom id can specified using the <b>Id</b> column.</p>"
         "<h2>Options</h2>\n"
         "<p>Enabling the <b>Row Grouping</b> option groups bars by column header name "
         "instead of the normal <b>Group</b> columns.</p>\n"
         "<p>Enabling the <b>Color by Set</b> option colors bars in the same group the same "
         "color instead using different colors for each bar in the group.</p>\n"
         "<p>Enabling the <b>Stacked</b> option places grouped bars on top of each other "
         "instead of the normal side by side placement.</p>\n"
         "<p>Enabling the <b>Percent</b> option rescales the values to a percentage of the "
         "maximum and minimum of the values.</p>\n"
         "<p>Enabling the <b>Range Bar</b> option draws a bar for the range (min/max) of "
         "the grouped values.</p>\n"
         "<p>Enabling the <b>Horizontal</b> option draws the bars horizontally instead "
         "of vertically.</p>\n";
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
 CQChartsBarPlot(view, view->charts()->plotType("barchart"), model), dataLabel_(this)
{
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
  CQChartsPlot::addProperties();

  CQChartsBarPlot::addProperties();

  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "labelColumn", "label");

  addProperty("options", this, "colorBySet");
  addProperty("options", this, "stacked"   );
  addProperty("options", this, "percent"   );
  addProperty("options", this, "rangeBar"  );

  CQChartsGroupPlot::addProperties();

  dataLabel_.addProperties("dataLabel");
}

//---

void
CQChartsBarChartPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() {
    dataLabel_.setDirection(horizontal_ ?
      CQChartsDataLabel::Direction::HORIZONTAL : CQChartsDataLabel::Direction::VERTICAL);

    updateRangeAndObjs();
  } );
}

void
CQChartsBarChartPlot::
setStacked(bool b)
{
  CQChartsUtil::testAndSet(stacked_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setPercent(bool b)
{
  CQChartsUtil::testAndSet(percent_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBarChartPlot::
setRangeBar(bool b)
{
  CQChartsUtil::testAndSet(rangeBar_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBarChartPlot::
updateRange(bool apply)
{
  dataRange_.reset();

  if (! isHorizontal())
    dataRange_.updateRange(-0.5, 0);
  else
    dataRange_.updateRange(0, -0.5);

  //---

  valueSets_    .clear();
  valueGroupInd_.clear();

  //---

  // init grouping
  //   non-range use group columns for grouping
  if (! isRangeBar())
    initGroupData(valueColumns(), nameColumn());
  else
    initGroupData(Columns(), nameColumn());

  //---

  // process model data
  class BarChartVisitor : public ModelVisitor {
   public:
    BarChartVisitor(CQChartsBarChartPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &ind, int row) override {
      plot_->addRow(ind, row);

      return State::OK;
    }

   private:
    CQChartsBarChartPlot *plot_ { nullptr };
  };

  BarChartVisitor barChartVisitor(this);

  visitModel(barChartVisitor);

  //---

  int nv = numValueSets();

  int numVisible = 0;

  for (int j = 0; j < nv; ++j)
    numVisible += ! isSetHidden(j);

  //---

  if (nv > 0) {
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
    setXValueColumn(groupColumn().isValid() ? groupColumn() : nameColumn());
    setYValueColumn(valueColumn());
  }
  else {
    setYValueColumn(groupColumn().isValid() ? groupColumn() : nameColumn());
    setXValueColumn(valueColumn());
  }

  //---

  // set axis column and labels
  int ns = (! isRangeBar() ? numValueColumns() : 1);

  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  xAxis->setColumn(groupColumn().isValid() ? groupColumn() : nameColumn());

  QString xname;

  if (ns > 1 && isRowGrouping())
    xname = ""; // No name for row grouping
  else {
    bool ok;

    xname = modelHeaderString(groupColumn().isValid() ? groupColumn() : nameColumn(), ok);
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
addRow(const QModelIndex &parent, int row)
{
  // add value for each column (non-range)
  if (! isRangeBar()) {
    for (const auto &column : valueColumns()) {
      Columns columns { column };

      addRowColumn(parent, row, columns);
    }
  }
  // add all values for columns (range)
  else {
    addRowColumn(parent, row, this->valueColumns());
  }
}

void
CQChartsBarChartPlot::
addRowColumn(const QModelIndex &parent, int row, const Columns &valueColumns)
{
  CQChartsModelIndex ind;

  if (! isRangeBar()) {
    assert(! valueColumns.empty());

    const CQChartsColumn &valueColumn = valueColumns[0];

    ind = CQChartsModelIndex(row, valueColumn, parent);
  }
  else {
    ind = CQChartsModelIndex(row, CQChartsColumn(), parent);
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

    group = modelHierString(row, groupColumn(), parent, ok1);

    categoryName = group;
  }

  //---

  // get name string (if defined) and update group name
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

  //---

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
    QModelIndex valInd  = modelIndex(row, valueColumn, parent);
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
    // set value data group name and value name

    if (groupName.length())
      valueData.setGroupName(groupName);

    QString valueName;

    // not row grouping so value name is column header
    if (! isGroupHeaders()) {
      assert(! valueColumns.empty());

      const CQChartsColumn &valueColumn = valueColumns[0];

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
    // if path grouping (hierarchical) then value name is group name
    if (isGroupPathType()) {
      if (groupName.length())
        valueData.setGroupName(groupName);
    }

    // save other name values for tip
    if (group.length()) valueData.setNameValue("Group", group);
    if (name .length()) valueData.setNameValue("Name" , name);
    if (label.length()) valueData.setNameValue("Label", label);
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

  //---

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

  QString name = groupIndName(groupInd);

  valueSets_.emplace_back(name, ind);

  valueGroupInd_[groupInd] = ind;

  CQChartsBarChartValueSet *valueSet = &valueSets_.back();

  valueSet->setGroupInd(groupInd);

  return valueSet;
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

  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  xAxis->clearTickLabels();
  yAxis->clearTickLabels();

  xAxis->setIntegral           (true);
  xAxis->setGridMid            (true);
  xAxis->setMajorIncrement     (1);
  xAxis->setMinorTicksDisplayed(false);
//xAxis->setDataLabels         (true);

  yAxis->setIntegral           (false);
  yAxis->setGridMid            (false);
  yAxis->setMajorIncrement     (0);
  yAxis->setMinorTicksDisplayed(true);
//yAxis->setDataLabels         (true);

  //---

  int nv = numValueSets();

  //---

  // set x axis labels
  int numVisible = 0;

  for (int iv = 0; iv < nv; ++iv) {
    if (isSetHidden(iv))
      continue;

    const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

    xAxis->setTickLabel(numVisible, valueSet.name());

    ++numVisible;
  }

  //---

  barWidth_ = 1.0;

  int ns = (! isRangeBar() ? numValueColumns() : 1);

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

    int numVisible1 = 0;

    if (ns > 1) {
      for (int ivs = 0; ivs < nvs; ++ivs)
        numVisible1 += ! isValueHidden(ivs);
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

    for (int ivs = 0; ivs < nvs; ++ivs) {
      if (ns > 1) {
        if (isValueHidden(ivs))
          continue;
      }

      const CQChartsBarChartValue &ivalue = valueSet.value(ivs);

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

        barWidth_ = std::min(barWidth_, brect.getWidth());
      }
      else {
        if (isStacked())
          brect = CQChartsGeom::BBox(value1, bx, value2, bx + 1.0);
        else
          brect = CQChartsGeom::BBox(value1, bx1, value2, bx1 + bw1);

        barWidth_ = std::min(barWidth_, brect.getHeight());
      }

      CQChartsBarChartObj *barObj = nullptr;

      if (ns > 1)
        barObj = new CQChartsBarChartObj(this, brect, ivs, nvs, iv, nv, 0, 1, &ivalue, minInd.ind);
      else
        barObj = new CQChartsBarChartObj(this, brect, 0, 1, iv, nv, ivs, nvs, &ivalue, minInd.ind);

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
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  return yAxis->valueStr(v);
}

void
CQChartsBarChartPlot::
addKeyItems(CQChartsPlotKey *key)
{
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

      for (int iv = 0; iv < nv; ++iv) {
        const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

        addKeyRow(iv, nv, valueSet.name());
      }
    }
    else {
      const CQChartsBarChartValueSet &valueSet = this->valueSet(0);

      int nv = valueSet.numValues();

      for (int iv = 0; iv < valueSet.numValues(); ++iv) {
        const CQChartsBarChartValue &value = valueSet.value(iv);

        addKeyRow(iv, nv, value.valueName());
      }
    }
  }
  else {
    int nv = numValueSets();

    for (int iv = 0; iv < nv; ++iv) {
      const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

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

      addKeyRow(iv, nv, valueSet.name(), c);
    }

#if 0
    if (isGroupHeaders()) {
      int nv = numValueSets();

      for (int iv = 0; iv < nv; ++iv) {
        const CQChartsBarChartValueSet &valueSet = this->valueSet(iv);

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

        addKeyRow(iv, nv, valueSet.name(), c);
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
#endif
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
  addColumnSelectIndex(inds, plot_->groupColumn());
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

  static double minBorderSize = 5.0;
  static double minSize       = 3.0;

  bool skipBorder = false;

  if (layer == CQChartsPlot::Layer::MID) {
    double m1 = plot_->lengthPixelSize(plot_->margin(), ! plot_->isHorizontal());
    double m2 = m1;

    if      (nset_ > 1) {
      if      (ival_ == 0)
        m1 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
      else if (ival_ == nval_ - 1)
        m2 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
    }
    else if (nval_ > 1) {
      if      (isval_ == 0)
        m1 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
      else if (isval_ == nsval_ - 1)
        m2 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
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

    qrect = CQChartsUtil::toQRect(prect);

    //---

    // calc pen (stroke)
    QPen pen;

    if (plot_->isBorder() && ! skipBorder) {
      QColor bc = plot_->interpBorderColor(0, 1);

      bc.setAlphaF(plot_->borderAlpha());

      double bw = plot_->lengthPixelWidth(plot_->borderWidth()); // TODO: width, height or both

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
  assert(i >= 0 && i < n);
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
