#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsRand.h>
#include <CQCharts.h>

#include <QPainter>
#include <QMenu>

CQChartsBoxPlotType::
CQChartsBoxPlotType()
{
}

void
CQChartsBoxPlotType::
addParameters()
{
  startParameterGroup("Box Plot");

  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("draw bars horizontal");
  addBoolParameter("normalized", "Normalized", "normalized").setTip("normalize data ranges");
  addBoolParameter("notched"   , "Notched"   , "notched"   ).setTip("notch bar");

  addBoolParameter("colorBySet", "Color by Set", "colorBySet").setTip("Color by value set");

  addBoolParameter("pointsJitter" , "PointsJitter" , "pointsJitter" ).setTip("jitter data points");
  addBoolParameter("pointsStacked", "PointsStacked", "pointsStacked").setTip("stacked data points");

  addBoolParameter("violin"  , "Violin"  , "violin"  ).setTip("violin border");
  addBoolParameter("errorBar", "ErrorBar", "errorBar").setTip("error bar");

  endParameterGroup();

  //---

  startParameterGroup("Raw Values");

  addColumnsParameter("value", "Value", "valueColumns", "1").setTip("value column(s)");
  addColumnParameter ("name" , "Name" , "nameColumn"       ).setTip("Name column");
  addColumnParameter ("set"  , "Set"  , "setColumn"        ).setTip("Set Values");

  endParameterGroup();

  //---

  startParameterGroup("Calculated Values");
  addColumnParameter("x"          , "X"          , "xColumn"          ).
    setTip("X Value");
  addColumnParameter("min"        , "Min"        , "minColumn"        ).
    setTip("Min Value");
  addColumnParameter("lowerMedian", "LowerMedian", "lowerMedianColumn").
    setTip("Lower Median Value");
  addColumnParameter("median"     , "Median"     , "medianColumn"     ).
    setTip("Media Value");
  addColumnParameter("upperMedian", "UpperMedian", "upperMedianColumn").
    setTip("Upper Median Value");
  addColumnParameter("max"        , "Max"        , "maxColumn"        ).
    setTip("Max Value");
  addColumnParameter("outliers"   , "Outliers"   , "outliersColumn"   ).
    setTip("Outlier Values");
  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsBoxPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws box and whiskers for the min, max, median and outlier values of the set "
         "of y values for rows with identical x values.\n"
         "<h2>Columns</h2>\n"
         "<p>Values can be supplied using:</p>\n"
         "<ul>\n"
         "<li>Raw Values with X and Y values in <b>value</b> and <b>set</b> columns.</li>\n"
         "<li>Calculated Values in the <b>min</b>, <b>lowerMedian</b>, <b>median</b>, "
         "<b>upperMedian</b>, <b>max</b> and <b>outliers</b> columns.</li>\n"
         "</ul>";
}

CQChartsPlot *
CQChartsBoxPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBoxPlot(view, model);
}

//---

CQChartsBoxPlot::
CQChartsBoxPlot(CQChartsView *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("box"), model),
 CQChartsPlotBoxShapeData    <CQChartsBoxPlot>(this),
 CQChartsPlotTextData        <CQChartsBoxPlot>(this),
 CQChartsPlotWhiskerLineData <CQChartsBoxPlot>(this),
 CQChartsPlotOutlierPointData<CQChartsBoxPlot>(this),
 CQChartsPlotJitterPointData <CQChartsBoxPlot>(this)
{
  setBoxFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setBoxBorder(true);
  setBoxFilled(true);

  setOutlierSymbolType(CQChartsSymbol::Type::CIRCLE);
  setOutlierSymbolSize(CQChartsLength("4px"));
  setOutlierSymbolFilled(true);
  setOutlierSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setJitterSymbolType(CQChartsSymbol::Type::CIRCLE);
  setJitterSymbolSize(CQChartsLength("4px"));

  addAxes();

  addKey();

  addTitle();
}

CQChartsBoxPlot::
~CQChartsBoxPlot()
{
}

//------

void
CQChartsBoxPlot::
setValueColumn(const CQChartsColumn &c)
{
  if (c != valueColumns_.column()) {
    valueColumns_.setColumn(c);

    updateRangeAndObjs();
  }
}

void
CQChartsBoxPlot::
setValueColumns(const Columns &cols)
{
  if (cols != valueColumns_.columns()) {
    valueColumns_.setColumns(cols);

    updateRangeAndObjs();
  }
}

QString
CQChartsBoxPlot::
valueColumnsStr() const
{
  return valueColumns_.columnsStr();
}

bool
CQChartsBoxPlot::
setValueColumnsStr(const QString &s)
{
  bool rc = true;

  if (s != valueColumnsStr()) {
    CQChartsColumns::Columns columns;

    rc = CQChartsUtil::stringToColumns(model_.data(), s, columns);

    if (rc) {
      valueColumns_.setColumns(columns);

      updateRangeAndObjs();
    }
  }

  return rc;
}

const CQChartsColumn &
CQChartsBoxPlot::
valueColumnAt(int i) const
{
  return valueColumns_.getColumn(i);
}

int
CQChartsBoxPlot::
numValueColumns() const
{
  return valueColumns_.count();
}

//---

void
CQChartsBoxPlot::
setSetColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(setColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setMinColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(minColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setLowerMedianColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(lowerMedianColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setMedianColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(medianColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setUpperMedianColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(upperMedianColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setMaxColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(maxColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setOutliersColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(outliersColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setShowOutliers(bool b)
{
  CQChartsUtil::testAndSet(showOutliers_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setConnected(bool b)
{
  CQChartsUtil::testAndSet(connected_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setBoxWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(boxWidth_, l, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setColorBySet(bool b)
{
  CQChartsUtil::testAndSet(colorBySet_, b, [&]() {
    resetSetHidden(); updateRangeAndObjs();
  } );
}

//---

const CQChartsLength &
CQChartsBoxPlot::
cornerSize() const
{
  return boxData_.shape.border.cornerSize;
}

void
CQChartsBoxPlot::
setCornerSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.cornerSize, s, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsBoxPlot::
setWhiskerRange(double r)
{
  CQChartsUtil::testAndSet(whiskerRange_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setWhiskerExtent(double r)
{
  CQChartsUtil::testAndSet(whiskerExtent_, r, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsBoxPlot::
setTextMargin(double r)
{
  CQChartsUtil::testAndSet(textMargin_, r, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsBoxPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns/raw", this, "valueColumns", "values");
  addProperty("columns/raw", this, "nameColumn"  , "name"  );
  addProperty("columns/raw", this, "setColumn"   , "set"   );

  addProperty("columns/calculated", this, "xColumn"          , "x"          );
  addProperty("columns/calculated", this, "minColumn"        , "min"        );
  addProperty("columns/calculated", this, "lowerMedianColumn", "lowerMedian");
  addProperty("columns/calculated", this, "medianColumn"     , "media"      );
  addProperty("columns/calculated", this, "upperMedianColumn", "upperMedian");
  addProperty("columns/calculated", this, "maxColumn"        , "max"        );
  addProperty("columns/calculated", this, "outliersColumn"   , "outlier"    );

  CQChartsGroupPlot::addProperties();

  // connect multiple whiskers
  addProperty("options", this, "connected", "connected");

  // options
  addProperty("options", this, "horizontal", "horizontal");
  addProperty("options", this, "normalized", "normalized");
  addProperty("options", this, "colorBySet");

  // jitter
  addProperty("points"       , this, "pointsJitter"    , "jitter" );
  addProperty("points"       , this, "pointsStacked"   , "stacked");
  addProperty("points/symbol", this, "jitterSymbolType", "type"   );
  addProperty("points/symbol", this, "jitterSymbolSize", "size"   );

  // violin
  addProperty("violin", this, "violin"     , "enabled");
  addProperty("violin", this, "violinWidth", "width"  );
  addProperty("violin", this, "violinBox"  , "box"    );

  // error bar
  addProperty("errorBar", this, "errorBar"    , "enabled");
  addProperty("errorBar", this, "errorBarType", "type"   );

  // whisker box
  addProperty("box", this, "whiskerRange", "range"  );
  addProperty("box", this, "boxWidth"    , "width"  );
  addProperty("box", this, "notched"     , "notched");

  // whisker box stroke
  addProperty("box/stroke", this, "boxBorder" , "visible"   );
  addProperty("box/stroke", this, "cornerSize", "cornerSize");

  addLineProperties("box/stroke", "boxBorder");

  // whisker box fill
  addProperty("box/fill", this, "boxFilled", "visible");

  addFillProperties("box/fill", "boxFill");

  // whisker line
  addLineProperties("whisker", "whiskerLines");

  addProperty("whisker", this, "whiskerExtent", "extent");

  // value labels
  addProperty("labels", this, "textVisible", "visible");

  addTextProperties("labels", "text");

  addProperty("labels", this, "textMargin", "margin");

  // outlier
  addProperty("outlier", this, "showOutliers", "visible");

  addSymbolProperties("outlier/symbol", "outlier");
}

//---

void
CQChartsBoxPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setNormalized(bool b)
{
  CQChartsUtil::testAndSet(normalized_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setNotched(bool b)
{
  CQChartsUtil::testAndSet(notched_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setPointsJitter(bool b)
{
  CQChartsUtil::testAndSet(pointsJitter_, b, [&]() {
    updateRangeAndObjs();

    if (b)
      setPointsStacked(false);
  });
}

void
CQChartsBoxPlot::
setPointsStacked(bool b)
{
  CQChartsUtil::testAndSet(pointsStacked_, b, [&]() {
    updateRangeAndObjs();

    if (b)
      setPointsJitter(false);
  });
}

//---

void
CQChartsBoxPlot::
setViolin(bool b)
{
  CQChartsUtil::testAndSet(violin_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setViolinWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(violinWidth_, l, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setViolinBox(bool b)
{
  CQChartsUtil::testAndSet(violinBox_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setErrorBar(bool b)
{
  CQChartsUtil::testAndSet(errorBar_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setErrorBarType(const ErrorBarType &t)
{
  CQChartsUtil::testAndSet(errorBarType_, t, [&]() { updateRangeAndObjs(); } );
}

//---

bool
CQChartsBoxPlot::
isPreCalc() const
{
  return (minColumn        ().isValid() &&
          lowerMedianColumn().isValid() &&
          medianColumn     ().isValid() &&
          upperMedianColumn().isValid() &&
          maxColumn        ().isValid());
}

void
CQChartsBoxPlot::
calcRange()
{
  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  xAxis->setIntegral      (true);
  xAxis->setMajorIncrement(1);

  yAxis->setIntegral      (false);
  yAxis->setMajorIncrement(0);

  //---

  if (! isPreCalc())
    updateRawRange();
  else
    updateCalcRange();
}

// calculate box plot from individual values
void
CQChartsBoxPlot::
updateRawRange()
{
  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  yAxis->setVisible(true);

  //---

  initGroupData(valueColumns(), nameColumn());

  //---

  updateRawWhiskers();

  //---

  dataRange_.reset();

  xrange_ = CQChartsGeom::RMinMax();

  //---

  xAxis->clearTickLabels();

  //---

  bool hasSets   = this->hasSets();
  bool hasGroups = this->hasGroups();

  //---

  int ig = 0;

  for (auto &groupIdWhiskers : groupWhiskers_) {
    int            groupInd      = groupIdWhiskers.first;
    SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    if (! isGrouped() || ! isSetHidden(ig)) {
      int is = 0;

      for (auto &setWhiskers : setWhiskerMap) {
        if (isGrouped() || ! isSetHidden(is)) {
          int                     setId   = setWhiskers.first;
          CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

          //---

          QString name;

          if      (isGroupHeaders()) {
            name = whisker.name();
          }
          else if (hasGroups) {
            name = groupIndName(groupInd);
          }
          else if (hasSets) {
            name = this->setIdName(setId);
          }

          whisker.setName(name);

          //---

          int x;

          if (hasSets && isConnected())
            x = setId;
          else
            x = ig;

          //---

          if (name.length())
            xAxis->setTickLabel(x, name);

          //---

          double min, max;

          if (isErrorBar()) {
            min = whisker.mean() - whisker.stddev();
            max = whisker.mean() + whisker.stddev();
          }
          else {
            if (isShowOutliers()) {
              min = whisker.rvalue(0);
              max = whisker.rvalue(whisker.numValues() - 1);
            }
            else {
              min = whisker.min();
              max = whisker.max();
            }
          }

          if (! isNormalized()) {
            if (! isHorizontal()) {
              dataRange_.updateRange(x - 0.5, min);
              dataRange_.updateRange(x + 0.5, max);
            }
            else {
              dataRange_.updateRange(min, x - 0.5);
              dataRange_.updateRange(max, x + 0.5);
            }
          }
          else {
            xrange_.add(x - 0.5);
            xrange_.add(x + 0.5);
          }

          //---

          if (isViolin()) {
            const CQChartsDensity &density = whisker.density();

            if (! isHorizontal()) {
              dataRange_.updateRange(x, density.xmin1());
              dataRange_.updateRange(x, density.xmax1());
            }
            else {
              dataRange_.updateRange(density.xmin1(), x);
              dataRange_.updateRange(density.xmax1(), x);
            }
          }
        }

        ++is;
      }
    }

    ++ig;
  }

  //---

  if (isNormalized()) {
    if (xrange_.isSet()) {
      if (! isHorizontal()) {
        dataRange_.updateRange(xrange_.min(), 0.0);
        dataRange_.updateRange(xrange_.max(), 1.0);
      }
      else {
        dataRange_.updateRange(0.0, xrange_.min());
        dataRange_.updateRange(1.0, xrange_.max());
      }
    }
    else {
      dataRange_.updateRange(0.0, 0.0);
      dataRange_.updateRange(1.0, 1.0);
    }
  }

  //---

//xAxis->setColumn(setColumn());
  yAxis->setColumn(valueColumn());

  //---

  bool ok;

  QString xname = xLabel();

  if (! xname.length() && groupColumn().isValid())
    xname = modelHeaderString(groupColumn(), ok);

  if (! xname.length() && setColumn().isValid())
    xname = modelHeaderString(setColumn(), ok);

  xAxis->setLabel(xname);

  //---

  QString yname = yLabel();

  if (valueColumns().size() == 1 && ! yname.length())
    yname = modelHeaderString(valueColumn(), ok);

  yAxis->setLabel(yname);
}

bool
CQChartsBoxPlot::
hasSets() const
{
  for (const auto &groupIdWhiskers : groupWhiskers_) {
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    if (setWhiskerMap.size() > 1)
      return true;
  }

  return false;
}

bool
CQChartsBoxPlot::
hasGroups() const
{
  return (groupWhiskers().size() > 1);
}

QString
CQChartsBoxPlot::
setIdName(int setId) const
{
  return setValueInd_.idName(setId);
}

// calculate box plot from pre-calculated values
void
CQChartsBoxPlot::
updateCalcRange()
{
  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  yAxis->setVisible(false);

  //---

  dataRange_.reset();

  xrange_ = CQChartsGeom::RMinMax();

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    using DataList = CQChartsBoxPlot::WhiskerDataList;

   public:
    BoxPlotVisitor(CQChartsBoxPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      plot_->addCalcRow(data, dataList_);

      return State::OK;
    }

    const DataList &dataList() const { return dataList_; }

   private:
    CQChartsBoxPlot *plot_ { nullptr };
    DataList         dataList_;
  };

  BoxPlotVisitor boxPlotVisitor(this);

  visitModel(boxPlotVisitor);

  whiskerDataList_ = boxPlotVisitor.dataList();

  //---

  if (isNormalized()) {
    if (xrange_.isSet()) {
      if (! isHorizontal()) {
        dataRange_.updateRange(xrange_.min(), 0.0);
        dataRange_.updateRange(xrange_.max(), 1.0);
      }
      else {
        dataRange_.updateRange(0.0, xrange_.min());
        dataRange_.updateRange(1.0, xrange_.max());
      }
    }
    else {
      dataRange_.updateRange(0.0, 0.0);
      dataRange_.updateRange(1.0, 1.0);
    }
  }

  //---

  bool ok;

  QString xname = (xLabel().length() ? xLabel() : modelHeaderString(xColumn(), ok));

  xAxis->setLabel(xname);
}

void
CQChartsBoxPlot::
addCalcRow(const ModelVisitor::VisitData &vdata, WhiskerDataList &dataList)
{
  CQChartsBoxWhiskerData data;

  bool ok;

  data.ind = vdata.parent;

  if (xColumn().isValid())
    data.x = modelReal(vdata.row, xColumn(), vdata.parent, ok);
  else
    data.x = vdata.row;

  data.min    = modelReal(vdata.row, minColumn        (), vdata.parent, ok);
  data.lower  = modelReal(vdata.row, lowerMedianColumn(), vdata.parent, ok);
  data.median = modelReal(vdata.row, medianColumn     (), vdata.parent, ok);
  data.upper  = modelReal(vdata.row, upperMedianColumn(), vdata.parent, ok);
  data.max    = modelReal(vdata.row, maxColumn        (), vdata.parent, ok);

  data.dataMin = data.min;
  data.dataMax = data.max;

  if (isShowOutliers()) {
    data.outliers = modelReals(vdata.row, outliersColumn(), vdata.parent, ok);

    for (auto &o : data.outliers) {
      data.dataMin = std::min(data.dataMin, o);
      data.dataMax = std::max(data.dataMax, o);
    }
  }

  if (! isNormalized()) {
    if (! isHorizontal()) {
      dataRange_.updateRange(data.x - 0.5, data.min);
      dataRange_.updateRange(data.x + 0.5, data.max);
    }
    else {
      dataRange_.updateRange(data.min, data.x - 0.5);
      dataRange_.updateRange(data.max, data.x + 0.5);
    }
  }
  else {
    xrange_.add(data.x - 0.5);
    xrange_.add(data.x + 0.5);
  }

  //---

  bool nameValid = true;

  data.name = modelString(vdata.row, idColumn(), vdata.parent, ok);

  if (! ok || ! data.name.length()) {
    data.name = modelHeaderString(vdata.row, Qt::Vertical, ok);

    if (! ok || ! data.name.length()) {
      data.name = QString("%1").arg(vdata.row);
      nameValid = false;
    }
  }

  if (nameValid)
    xAxis_->setTickLabel(vdata.row, data.name);

  //---

  dataList.push_back(data);
}

void
CQChartsBoxPlot::
updateRawWhiskers()
{
  groupWhiskers_.clear();

  //---

  // x data type
  setType_ = columnValueType(setColumn());

  setValueInd_.clear();

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    BoxPlotVisitor(CQChartsBoxPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      plot_->addRawWhiskerRow(data);

      return State::OK;
    }

   private:
    CQChartsBoxPlot *plot_ { nullptr };
  };

  BoxPlotVisitor boxPlotVisitor(this);

  visitModel(boxPlotVisitor);

  //---

  grouped_ = (numGroups() > 1);

  //---

  for (auto &groupIdWhiskers : groupWhiskers_) {
    SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    for (auto &setWhiskers : setWhiskerMap)
      setWhiskers.second.init();
  }
}

void
CQChartsBoxPlot::
addRawWhiskerRow(const ModelVisitor::VisitData &vdata)
{
  // get value set id
  int      setId = -1;
  QVariant setVal;

  if (setColumn().isValid()) {
    bool ok1;

    setVal = modelHierValue(vdata.row, setColumn(), vdata.parent, ok1);

    if (ok1) {
      if      (setType_ == ColumnType::INTEGER) {
        int i = CQChartsVariant::toInt(setVal, ok1);

        if (ok1)
          setId = setValueInd_.calcId(i);
      }
      else if (setType_ == ColumnType::REAL) {
        double r = CQChartsVariant::toReal(setVal, ok1);

        if (ok1 && ! CMathUtil::isNaN(r))
          setId = setValueInd_.calcId(r);
      }
      else {
        QString s;

        ok1 = CQChartsVariant::toString(setVal, s);

        if (ok1)
          setId = setValueInd_.calcId(s);
      }
    }
  }

  //---

  //QModelIndex xind  = modelIndex(vdata.row, setColumn(), vdata.parent);
  //QModelIndex xind1 = normalizeIndex(xind);

  for (const auto &valueColumn : valueColumns()) {
    CQChartsModelIndex ind(vdata.row, valueColumn, vdata.parent);

    // get group
    int groupInd = rowGroupInd(ind);

    //---

    // add value to set
    bool ok2;

    double value = modelReal(vdata.row, valueColumn, vdata.parent, ok2);

    if (! ok2) value = vdata.row;

    if (CMathUtil::isNaN(value))
      return;

    QModelIndex yind  = modelIndex(vdata.row, valueColumn, vdata.parent);
    QModelIndex yind1 = normalizeIndex(yind);

    CQChartsBoxPlotValue wv(value, yind1);

    auto p = groupWhiskers_.find(groupInd);

    if (p == groupWhiskers_.end()) {
      SetWhiskerMap setWhiskerMap;

      p = groupWhiskers_.insert(p, GroupSetWhiskerMap::value_type(groupInd, SetWhiskerMap()));
    }

    SetWhiskerMap &setWhiskerMap = (*p).second;

    auto p1 = setWhiskerMap.find(setId);

    if (p1 == setWhiskerMap.end()) {
      CQChartsBoxPlotWhisker whisker;

      whisker.setRange(whiskerRange());

      QString name;
      bool    ok = false;

      if      (isGroupHeaders()) {
        name = modelHeaderString(valueColumn, ok);
      }
      else if (setColumn().isValid()) {
        ok = CQChartsVariant::toString(setVal, name);
      }

      if (ok && name.length())
        whisker.setName(name);

      p1 = setWhiskerMap.insert(p1, SetWhiskerMap::value_type(setId, whisker));
    }

    (*p1).second.addValue(wv);
  }
}

CQChartsGeom::BBox
CQChartsBoxPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsBoxPlotWhiskerObj *boxObj = dynamic_cast<CQChartsBoxPlotWhiskerObj *>(plotObj);

    if (boxObj)
      bbox += boxObj->annotationBBox();
  }

  return bbox;
}

//------

bool
CQChartsBoxPlot::
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

  if (! isPreCalc())
    return initRawObjs();
  else
    return initCalcObjs();
}

bool
CQChartsBoxPlot::
initRawObjs()
{
  double bw2 = lengthPlotSize(boxWidth   (), ! isHorizontal())/2.0;
  double vw2 = lengthPlotSize(violinWidth(), ! isHorizontal())/2.0;

  //---

  int ig = 0;
  int ng = numGroups();

  for (const auto &groupIdWhiskers : this->groupWhiskers()) {
    int                  groupInd      = groupIdWhiskers.first;
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    if (! isConnected()) {
      int is = 0;
      int ns = setWhiskerMap.size();

      double sf = (ns > 1 ? 1.0/ns : 1.0);

      for (const auto &setWhiskers : setWhiskerMap) {
        bool hidden = (isGrouped() ? isSetHidden(ig) : isSetHidden(is));
        if (hidden) { ++is; continue; }

        int                           setId   = setWhiskers.first;
        const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

        //----

        double pos = ig;
        double sbw = (isViolin() ? vw2 : bw2);

        if (ns > 1) {
          pos += (is + 1.0)/(ns + 1.0) - 0.5;
          sbw *= sf;
        }

        //---

        CQChartsGeom::BBox rect;

        if (! isNormalized()) {
          if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - sbw, whisker.lower(), pos + sbw, whisker.upper());
          else
            rect = CQChartsGeom::BBox(whisker.lower(), pos - sbw, whisker.upper(), pos + sbw);
        }
        else {
           if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - sbw, 0.0, pos + sbw, 1.0);
          else
            rect = CQChartsGeom::BBox(0.0, pos - sbw, 1.0, pos + sbw);
        }

        CQChartsBoxPlotWhiskerObj *boxObj =
          new CQChartsBoxPlotWhiskerObj(this, rect, setId, groupInd, whisker, ig, ng, is, ns);

        addPlotObject(boxObj);

        //---

        if (isPointsJitter()) {
          const CQChartsDensity &density = whisker.density();

          double ymin = density.ymin1();
          double ymax = density.ymax1();

          CQChartsRand::RealInRange rand(-vw2, vw2);

          int nv = whisker.numValues();

          for (int iv = 0; iv < nv; ++iv) {
            const CQChartsBoxPlotValue &value = whisker.value(iv);

            double d = rand.gen();

            double yv = density.yval(value)/(ymax - ymin);

            double x = pos + yv*d;
            double y = value.value;

            double y1 = (isNormalized() ? whisker.normalize(y) : y);

            QPointF pos;

            CQChartsGeom::BBox rect;

            if (! isHorizontal()) {
              pos = QPointF(x, y1);

              if (! isNormalized())
                rect = CQChartsGeom::BBox(x - 0.1, y1 - 0.1, x + 0.1, y1 + 0.1);
              else
                rect = CQChartsGeom::BBox(x - 0.01, y1 - 0.01, x + 0.01, y1 + 0.01);
            }
            else {
              pos = QPointF(y1, x);

              if (! isNormalized())
                rect = CQChartsGeom::BBox(y1 - 0.1, x - 0.1, y1 + 0.1, x + 0.1);
              else
                rect = CQChartsGeom::BBox(y1 - 0.01, x - 0.01, y1 + 0.01, x + 0.01);
            }

            CQChartsBoxPlotPointObj *pointObj =
              new CQChartsBoxPlotPointObj(this, rect, setId, groupInd, pos, value.ind,
                                          ig, ng, is, ns, iv, nv);

            addPlotObject(pointObj);
          }
        }
        else if (isPointsStacked()) {
          using Rects    = std::vector<CQChartsGeom::BBox>;
          using PosRects = std::map<int,Rects>;

          PosRects posRects;

          auto placePosRect = [&](int pos, const CQChartsGeom::BBox &rect) {
            Rects &rects = posRects[pos];

            for (auto &r : rects) {
              if (r.intersect(rect))
                return false;
            }

            rects.push_back(rect);

            return true;
          };

          auto placeRect = [&](const CQChartsGeom::BBox &rect, CQChartsGeom::BBox &prect) {
            if (placePosRect(0, rect))
              return false;

            double w = rect.getWidth();

            int d = 1;

            while (true) {
              CQChartsGeom::BBox rect1 = rect;

              if (! isNormalized())
                rect1.moveBy(CQChartsGeom::Point(-d*w, 0.0));
              else
                rect1.moveBy(CQChartsGeom::Point(0.0, -d*w));

              if (placePosRect(-d, rect1)) {
                prect = rect1;
                return true;
              }

              CQChartsGeom::BBox rect2 = rect;

              if (! isNormalized())
                rect2.moveBy(CQChartsGeom::Point(d*w, 0.0));
              else
                rect2.moveBy(CQChartsGeom::Point(0.0, d*w));

              if (placePosRect(d, rect2)) {
                prect = rect2;
                return true;
              }

              ++d;
            }
          };

          int nv = whisker.numValues();

          for (int iv = 0; iv < nv; ++iv) {
            const CQChartsBoxPlotValue &value = whisker.value(iv);

            double x = pos;
            double y = value.value;

            double y1 = (isNormalized() ? whisker.normalize(y) : y);

            double sx, sy;

            plotSymbolSize(jitterSymbolSize(), sx, sy);

            QPointF pos;

            CQChartsGeom::BBox rect;

            if (! isHorizontal())
              pos  = QPointF(x, y1);
            else
              pos  = QPointF(y1, x);

            rect = CQChartsGeom::BBox(pos.x() - sx, pos.y() - sy, pos.x() + sx, pos.y() + sy);

            CQChartsBoxPlotPointObj *pointObj = nullptr;

            CQChartsGeom::BBox prect;

            if (placeRect(rect, prect)) {
              QPointF ppos = pos;

              if (! isHorizontal())
                ppos.setX(prect.getXMid());
              else
                ppos.setY(prect.getYMid());

              pointObj = new CQChartsBoxPlotPointObj(this, prect, setId, groupInd, ppos,
                                                     value.ind, ig, ng, is, ns, iv, nv);
            }
            else {
              pointObj = new CQChartsBoxPlotPointObj(this, rect, setId, groupInd, pos,
                                                     value.ind, ig, ng, is, ns, iv, nv);
            }

            addPlotObject(pointObj);
          }
        }

        //---

        ++is;
      }
    }
    else {
      bool hidden = (isGrouped() ? isSetHidden(ig) : false);
      if (hidden) { continue; }

      CQChartsGeom::BBox rect(dataRange_.xmin(), dataRange_.ymin(),
                              dataRange_.xmax(), dataRange_.ymax());

      CQChartsBoxPlotConnectedObj *connectedObj =
        new CQChartsBoxPlotConnectedObj(this, rect, groupInd, ig, ng);

      addPlotObject(connectedObj);
    }

    //---

    ++ig;
  }

  //---

  resetKeyItems();

  //---

  return true;
}

bool
CQChartsBoxPlot::
initCalcObjs()
{
  int pos = 0;

  for (const auto &whiskerData : whiskerDataList_) {
    CQChartsGeom::BBox rect;

    if (! isNormalized()) {
      if (! isHorizontal())
        rect = CQChartsGeom::BBox(pos - 0.10, whiskerData.lower, pos + 0.10, whiskerData.upper);
      else
        rect = CQChartsGeom::BBox(whiskerData.lower, pos - 0.10, whiskerData.upper, pos + 0.10);
    }
    else {
      if (! isHorizontal())
        rect = CQChartsGeom::BBox(pos - 0.10, 0.0, pos + 0.10, 1.0);
      else
        rect = CQChartsGeom::BBox(0.0, pos - 0.10, 1.0, pos + 0.10);
    }

    CQChartsBoxPlotDataObj *boxObj = new CQChartsBoxPlotDataObj(this, rect, whiskerData);

    addPlotObject(boxObj);

    ++pos;
  }

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsBoxPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int ng = numGroups();

  // if has groups
  if      (ng > 1) {
    // if color by set add key item per set
    if (hasSets() && isColorBySet()) {
      auto pg = this->groupWhiskers().begin();

      int                  groupInd      = (*pg).first;
      const SetWhiskerMap &setWhiskerMap = (*pg).second;

      QString groupName = groupIndName(groupInd);

      int is = 0;
      int ns = setWhiskerMap.size();

      for (const auto &setWhiskers : setWhiskerMap) {
        int                           setId   = setWhiskers.first;
      //const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

        QString setName = setIdName(setId);

        CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, is, ns);
        CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, is, setName);

        key->addItem(color, is, 0);
        key->addItem(text , is, 1);

        ++is;
      }
    }
    // if not color by set add key item per group
    else {
      int ig = 0;

      for (const auto &groupIdWhiskers : this->groupWhiskers()) {
        int groupInd = groupIdWhiskers.first;

        QString groupName = groupIndName(groupInd);

        CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, ig, ng);
        CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, ig, groupName);

        key->addItem(color, ig, 0);
        key->addItem(text , ig, 1);

        ++ig;
      }
    }
  }
  // if single group then add key per set
  else if (ng > 0) {
    auto pg = this->groupWhiskers().begin();

    int                  groupInd      = (*pg).first;
    const SetWhiskerMap &setWhiskerMap = (*pg).second;

    QString groupName = groupIndName(groupInd);

    int is = 0;
    int ns = setWhiskerMap.size();

    for (const auto &setWhiskers : setWhiskerMap) {
    //int                           setId   = setWhiskers.first;
      const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

      QString name = whisker.name();

      CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, is, ns);
      CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, is, name);

      key->addItem(color, is, 0);
      key->addItem(text , is, 1);

      ++is;
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

bool
CQChartsBoxPlot::
probe(ProbeData &probeData) const
{
  if (! dataRange_.isSet())
    return false;

  if (! isHorizontal()) {
    if (probeData.x < dataRange_.xmin() + 0.5)
      probeData.x = dataRange_.xmin() + 0.5;

    if (probeData.x > dataRange_.xmax() - 0.5)
      probeData.x = dataRange_.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
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
CQChartsBoxPlot::
addMenuItems(QMenu *menu)
{
  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  //---

  menu->addSeparator();

  (void) addCheckedAction("Horizontal", isHorizontal(), SLOT(setHorizontal(bool)));
  (void) addCheckedAction("Normalized", isNormalized(), SLOT(setNormalized(bool)));

  //---

  if (! isPreCalc()) {
    (void) addCheckedAction("Jitter Points" , isPointsJitter() , SLOT(setPointsJitter(bool)));
    (void) addCheckedAction("Stacked Points", isPointsStacked(), SLOT(setPointsStacked(bool)));

    (void) addCheckedAction("Notched"  , isNotched() , SLOT(setNotched(bool)));
    (void) addCheckedAction("Violin"   , isViolin()  , SLOT(setViolin(bool)));
    (void) addCheckedAction("Error Bar", isErrorBar(), SLOT(setErrorBar(bool)));
  }

  return true;
}

//------

CQChartsBoxPlotWhiskerObj::
CQChartsBoxPlotWhiskerObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                          int groupInd, const CQChartsBoxPlotWhisker &whisker,
                          int ig, int ng, int is, int ns) :
 CQChartsBoxPlotObj(plot, rect), setId_(setId), groupInd_(groupInd), whisker_(whisker),
 ig_(ig), ng_(ng), is_(is), ns_(ns)
{
}

double
CQChartsBoxPlotWhiskerObj::
pos() const
{
  return rect_.getXYMid(! plot_->isHorizontal());
}

double
CQChartsBoxPlotWhiskerObj::
min() const
{
  return whisker_.min();
}

double
CQChartsBoxPlotWhiskerObj::
lower() const
{
  return whisker_.lower();
}

double
CQChartsBoxPlotWhiskerObj::
median() const
{
  return whisker_.median();
}

double
CQChartsBoxPlotWhiskerObj::
upper() const
{
  return whisker_.upper();
}

double
CQChartsBoxPlotWhiskerObj::
max() const
{
  return whisker_.max();
}

double
CQChartsBoxPlotWhiskerObj::
mean() const
{
  return whisker_.mean();
}

double
CQChartsBoxPlotWhiskerObj::
stddev() const
{
  return whisker_.stddev();
}

double
CQChartsBoxPlotWhiskerObj::
notch() const
{
  return whisker_.notch();
}

QString
CQChartsBoxPlotWhiskerObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(setId_).arg(groupInd_);
}

QString
CQChartsBoxPlotWhiskerObj::
calcTipId() const
{
  QString setName, groupName, name;

  if (plot_->hasSets())
    setName = plot_->setIdName(setId_);

  if (plot_->hasGroups())
    groupName = plot_->groupIndName(groupInd_);

  if (! setName.length() && ! groupName.length())
    name = whisker_.name();

  CQChartsTableTip tableTip;

  if (setName.length())
    tableTip.addTableRow("Set", setName);

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  if (name.length())
    tableTip.addTableRow("Name", name);

  if (plot_->isErrorBar()) {
    tableTip.addTableRow("Mean"  , mean  ());
    tableTip.addTableRow("StdDev", stddev());
  }
  else {
    tableTip.addTableRow("Min"   , min   ());
    tableTip.addTableRow("Lower" , lower ());
    tableTip.addTableRow("Median", median());
    tableTip.addTableRow("Upper" , upper ());
    tableTip.addTableRow("Max"   , max   ());
  }

  return tableTip.str();
}

void
CQChartsBoxPlotWhiskerObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->setColumn  ());
  addColumnSelectIndex(inds, plot_->groupColumn());

  for (auto &value : whisker_.values())
    addSelectIndex(inds, value.ind.row(), value.ind.column(), value.ind.parent());
}

void
CQChartsBoxPlotWhiskerObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    for (auto &value : whisker_.values()) {
      addSelectIndex(inds, value.ind.row(), column, value.ind.parent());
    }
  }
}

bool
CQChartsBoxPlotWhiskerObj::
inside(const CQChartsGeom::Point &p) const
{
  if (plot_->isViolin()) {
    CQChartsGeom::Point p1 = plot_->windowToPixel(p);

    return ppoly_.containsPoint(CQChartsUtil::toQPoint(p1), Qt::OddEvenFill);
  }

  return CQChartsBoxPlotObj::inside(p);
}

void
CQChartsBoxPlotWhiskerObj::
draw(QPainter *painter)
{
  // get color index
  int ic = is_;
  int nc = ns_;

  if (ng_ > 1) {
    if (! plot_->hasSets() || ! plot_->isColorBySet()) {
      ic = ig_;
      nc = ng_;
    }
  }

  //---

  // set fill and stroke
  QPen   pen;
  QBrush brush;

  plot_->setPenBrush(pen, brush,
                     plot_->isBoxBorder(),
                     plot_->interpBoxBorderColor(ic, nc),
                     plot_->boxBorderAlpha(),
                     plot_->boxBorderWidth(),
                     plot_->boxBorderDash(),
                     plot_->isBoxFilled(),
                     plot_->interpBoxFillColor(ic, nc),
                     plot_->boxFillAlpha(),
                     plot_->boxFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  //---

  QPen   whiskerPen;
  QBrush whiskerBrush;

  plot_->setWhiskerLineDataPen(whiskerPen, ic, nc);

  plot_->setBrush(whiskerBrush, false);

  plot_->updateObjPenBrushState(this, whiskerPen, whiskerBrush);

  //---

  Qt::Orientation orientation = (! plot_->isHorizontal() ? Qt::Vertical : Qt::Horizontal);

  //---

  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  CQChartsWhiskerData data;

  data.min    = remapPos(this->min   ());
  data.lower  = remapPos(this->lower ());
  data.median = remapPos(this->median());
  data.upper  = remapPos(this->upper ());
  data.max    = remapPos(this->max   ());
  data.lnotch = remapPos(this->median() - this->notch());
  data.unotch = remapPos(this->median() + this->notch());

  //---

  bool drawBox       = true;
  bool drawBoxFilled = true;

  // draw violin
  if      (plot_->isViolin()) {
    const CQChartsDensity &density = whisker_.density();

    double vw = plot_->lengthPlotSize(plot_->violinWidth(), plot_->isHorizontal())/2.0;

    CQChartsGeom::BBox rect;

    if (! plot_->isHorizontal())
      rect = CQChartsGeom::BBox(pos - vw, data.min, pos + vw, data.max);
    else
      rect = CQChartsGeom::BBox(data.min, pos - vw, data.max, pos + vw);

    CQChartsWhiskerOpts opts;

    opts.violin = true;

    density.calcWhiskerPoly(ppoly_, plot_, rect, orientation, opts);

    painter->drawPolygon(ppoly_);

    drawBox       = plot_->isViolinBox();
    drawBoxFilled = false;
  }

  //---

  // draw error bar
  if (plot_->isErrorBar()) {
    painter->setPen(whiskerPen);

    //---

    double mean = remapPos(this->mean());
    double dev1 = remapPos(this->mean() - this->stddev());
    double dev2 = remapPos(this->mean() + this->stddev());

    CQChartsGeom::BBox rect;

    if (! plot_->isHorizontal())
      rect = CQChartsGeom::BBox(pos - bw/2.0, dev1, pos + bw/2.0, dev2);
    else
      rect = CQChartsGeom::BBox(dev1, pos - bw/2.0, dev2, pos + bw/2.0);

    if      (plot_->errorBarType() == CQChartsBoxPlot::ErrorBarType::CROSS_BAR) {
      CQChartsDensity::drawCrossBar(plot_, painter, rect, mean, orientation, plot_->cornerSize());
    }
    else if (plot_->errorBarType() == CQChartsBoxPlot::ErrorBarType::ERROR_BAR) {
      CQChartsDensity::drawErrorBar(plot_, painter, rect, orientation);
    }
    else if (plot_->errorBarType() == CQChartsBoxPlot::ErrorBarType::POINT_RANGE) {
      // set fill and stroke
      QPen   symbolPen;
      QBrush symbolBrush;

      QColor boxColor    = plot_->interpBoxFillColor(ic, nc);
      QColor borderColor = plot_->interpBoxBorderColor(ic, nc);

      plot_->setPen(symbolPen, /*stroked*/true, borderColor, plot_->boxBorderAlpha(),
                    plot_->boxBorderWidth(), CQChartsLineDash());

      plot_->setBrush(symbolBrush, /*filled*/true, boxColor, plot_->boxFillAlpha(),
                      CQChartsFillPattern());

      plot_->updateObjPenBrushState(this, symbolPen, symbolBrush);

      //---

      CQChartsSymbolData symbol;

      symbol.type = CQChartsSymbol::Type::CIRCLE;
      symbol.size = plot_->outlierSymbolSize();

      CQChartsDensity::drawPointRange(plot_, painter, rect, mean, orientation, symbol,
                                      symbolPen, symbolBrush);
    }
    else if (plot_->errorBarType() == CQChartsBoxPlot::ErrorBarType::LINE_RANGE) {
      CQChartsDensity::drawLineRange(plot_, painter, rect, orientation);
    }

    drawBox = false;
  }

  //---

  // draw notched box
  if (drawBox) {
    painter->setPen(whiskerPen);

    //---

    if (! drawBoxFilled) {
      QColor boxColor = plot_->interpThemeColor(0.0);

      brush.setColor(boxColor);

      painter->setBrush(brush);
    }

    //---

    CQChartsBoxWhiskerUtil::drawWhiskerBar(plot_, painter, data, pos, orientation,
                                           ww, bw, plot_->cornerSize(), plot_->isNotched());
  }

  //---

  if (plot_->isErrorBar()) {
  }
  else {
    double wd1 = ww/2.0;
    double wd2 = bw/2.0;

    double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

    if (plot_->drawLayerType() == CQChartsLayer::Type::MID_PLOT) {
      if (! plot_->isHorizontal()) {
        plot_->windowToPixel(pos - wd1, data.min   , px1, py1);
        plot_->windowToPixel(pos - wd2, data.lower , px2, py2);
        plot_->windowToPixel(pos      , data.median, px3, py3);
        plot_->windowToPixel(pos + wd2, data.upper , px4, py4);
        plot_->windowToPixel(pos + wd1, data.max   , px5, py5);
      }
      else {
        plot_->windowToPixel(data.min   , pos - wd1, px1, py1);
        plot_->windowToPixel(data.lower , pos - wd2, px2, py2);
        plot_->windowToPixel(data.median, pos      , px3, py3);
        plot_->windowToPixel(data.upper , pos + wd2, px4, py4);
        plot_->windowToPixel(data.max   , pos + wd1, px5, py5);
      }

      // draw labels
      if (plot_->isTextVisible()) {
        plot_->view()->setPlotPainterFont(plot_, painter, plot_->textFont());

        //---

        QPen pen;

        QColor tc = plot_->interpTextColor(ic, nc);

        plot_->setPen(pen, true, tc, plot_->textAlpha(),
                      CQChartsLength("0px"), CQChartsLineDash());

        painter->setPen(pen);

        //---

        bool hasRange = (fabs(this->max() - this->min()) > 1E-6);

        if (hasRange) {
          QString strl = QString("%1").arg(this->min   ());
          QString lstr = QString("%1").arg(this->lower ());
          QString mstr = QString("%1").arg(this->median());
          QString ustr = QString("%1").arg(this->upper ());
          QString strh = QString("%1").arg(this->max   ());

          if (! plot_->isHorizontal()) {
            drawHText(painter, px1, px5, py1, strl, /*onLeft*/true );
            drawHText(painter, px2, px4, py2, lstr, /*onLeft*/false);
            drawHText(painter, px2, px4, py3, mstr, /*onLeft*/true );
            drawHText(painter, px2, px4, py4, ustr, /*onLeft*/false);
            drawHText(painter, px1, px5, py5, strh, /*onLeft*/true );
          }
          else {
            drawVText(painter, py1, py5, px1, strl, /*onBottom*/false);
            drawVText(painter, py2, py4, px2, lstr, /*onBottom*/true );
            drawVText(painter, py2, py4, px3, mstr, /*onBottom*/false);
            drawVText(painter, py2, py4, px4, ustr, /*onBottom*/true );
            drawVText(painter, py1, py5, px5, strh, /*onBottom*/false);
          }
        }
        else {
          QString strl = QString("%1").arg(this->min());

          if (! plot_->isHorizontal())
            drawHText(painter, px1, px5, py1, strl, /*onLeft*/true);
          else
            drawVText(painter, py1, py5, px1, strl, /*onBottom*/false);
        }
      }

      //---

      // draw outlier symbols
      if (plot_->isShowOutliers()) {
        CQChartsSymbolData symbol;

        symbol.type = plot_->outlierSymbolType();
        symbol.size = plot_->outlierSymbolSize();

        //---

        QPen   pen;
        QBrush brush;

        plot_->setOutlierSymbolPenBrush(pen, brush, ic, nc);

        plot_->updateObjPenBrushState(this, pen, brush);

        //---

        std::vector<double> ovalues;

        for (auto &o : whisker_.outliers()) {
          double ovalue = remapPos(whisker_.rvalue(o));

          ovalues.push_back(ovalue);
        }

        CQChartsBoxWhiskerUtil::drawOutliers(plot_, painter, ovalues, pos, symbol,
                                             pen, brush, orientation);
      }
    }
  }
}

CQChartsGeom::BBox
CQChartsBoxPlotWhiskerObj::
annotationBBox() const
{
  if (plot_->isErrorBar())
    return CQChartsGeom::BBox();

  //---

  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  double wd1 = ww/2.0;
  double wd2 = bw/2.0;

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  if (! plot_->isHorizontal()) {
    plot_->windowToPixel(pos - wd1, remapPos(min   ()), px1, py1);
    plot_->windowToPixel(pos - wd2, remapPos(lower ()), px2, py2);
    plot_->windowToPixel(pos      , remapPos(median()), px3, py3);
    plot_->windowToPixel(pos + wd2, remapPos(upper ()), px4, py4);
    plot_->windowToPixel(pos + wd1, remapPos(max   ()), px5, py5);
  }
  else {
    plot_->windowToPixel(remapPos(min   ()), pos - wd1, px1, py1);
    plot_->windowToPixel(remapPos(lower ()), pos - wd2, px2, py2);
    plot_->windowToPixel(remapPos(median()), pos      , px3, py3);
    plot_->windowToPixel(remapPos(upper ()), pos + wd2, px4, py4);
    plot_->windowToPixel(remapPos(max   ()), pos + wd1, px5, py5);
  }

  //---

  CQChartsGeom::BBox pbbox;

  if (plot_->isTextVisible()) {
    bool hasRange = (fabs(max() - min()) > 1E-6);

    if (hasRange) {
      QString strl = QString("%1").arg(min   ());
      QString lstr = QString("%1").arg(lower ());
      QString mstr = QString("%1").arg(median());
      QString ustr = QString("%1").arg(upper ());
      QString strh = QString("%1").arg(max   ());

      if (! plot_->isHorizontal()) {
        addHBBox(pbbox, px1, px5, py1, strl, /*onLeft*/false);
        addHBBox(pbbox, px2, py4, py2, lstr, /*onLeft*/true );
        addHBBox(pbbox, px2, px4, py3, mstr, /*onLeft*/false);
        addHBBox(pbbox, px2, px4, py4, ustr, /*onLeft*/true );
        addHBBox(pbbox, px1, px5, py5, strh, /*onLeft*/false);
      }
      else {
        addVBBox(pbbox, py1, py5, px1, strl, /*onBottom*/true );
        addVBBox(pbbox, py2, py4, px2, lstr, /*onBottom*/false);
        addVBBox(pbbox, py2, py4, px3, mstr, /*onBottom*/true );
        addVBBox(pbbox, py2, py4, px4, ustr, /*onBottom*/false);
        addVBBox(pbbox, py1, py5, px5, strh, /*onBottom*/true );
      }
    }
    else {
      QString strl = QString("%1").arg(min());

      if (! plot_->isHorizontal())
        addHBBox(pbbox, px1, px5, py1, strl, /*onLeft*/false);
      else
        addVBBox(pbbox, py1, py5, px1, strl, /*onBottom*/true);
    }
  }
  else {
    if (! plot_->isHorizontal()) {
      pbbox += CQChartsGeom::Point(px5, py1);
      pbbox += CQChartsGeom::Point(px2, py2);
      pbbox += CQChartsGeom::Point(px4, py3);
      pbbox += CQChartsGeom::Point(px2, py4);
      pbbox += CQChartsGeom::Point(px5, py5);
    }
    else {
      pbbox += CQChartsGeom::Point(px1, py5);
      pbbox += CQChartsGeom::Point(px2, py2);
      pbbox += CQChartsGeom::Point(px3, py4);
      pbbox += CQChartsGeom::Point(px4, py2);
      pbbox += CQChartsGeom::Point(px5, py5);
    }
  }

  //---

  CQChartsGeom::BBox bbox;

  plot_->pixelToWindow(pbbox, bbox);

  return bbox;
}

double
CQChartsBoxPlotWhiskerObj::
remapPos(double y) const
{
  // remap to margin -> 1.0 - margin
  if (! plot_->isNormalized())
    return y;

  double ymargin = 0.0;

  return CMathUtil::map(y, min(), max(), ymargin, 1.0 - ymargin);
}

//------

CQChartsBoxPlotDataObj::
CQChartsBoxPlotDataObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                       const CQChartsBoxWhiskerData &data) :
 CQChartsBoxPlotObj(plot, rect), data_(data)
{
}

double
CQChartsBoxPlotDataObj::
pos() const
{
  return rect_.getXYMid(! plot_->isHorizontal());
}

QString
CQChartsBoxPlotDataObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(data_.name);
}

QString
CQChartsBoxPlotDataObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name"  , data_.name  );
  tableTip.addTableRow("Min"   , data_.min   );
  tableTip.addTableRow("Lower" , data_.lower );
  tableTip.addTableRow("Median", data_.median);
  tableTip.addTableRow("Upper" , data_.upper );
  tableTip.addTableRow("Max"   , data_.max   );

  return tableTip.str();
}

void
CQChartsBoxPlotDataObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn          ());
  addColumnSelectIndex(inds, plot_->minColumn        ());
  addColumnSelectIndex(inds, plot_->lowerMedianColumn());
  addColumnSelectIndex(inds, plot_->medianColumn     ());
  addColumnSelectIndex(inds, plot_->upperMedianColumn());
  addColumnSelectIndex(inds, plot_->maxColumn        ());
  addColumnSelectIndex(inds, plot_->outliersColumn   ());
}

void
CQChartsBoxPlotDataObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    addSelectIndex(inds, data_.ind.row(), column, data_.ind.parent());
  }
}

void
CQChartsBoxPlotDataObj::
draw(QPainter *painter)
{
  QPen   whiskerPen;
  QBrush whiskerBrush;

  plot_->setWhiskerLineDataPen(whiskerPen, 0, 1);

  plot_->setBrush(whiskerBrush, false);

  plot_->updateObjPenBrushState(this, whiskerPen, whiskerBrush);

  //---

  // set fill and stroke
  QPen   pen;
  QBrush brush;

  plot_->setPenBrush(pen, brush,
                     plot_->isBoxBorder(),
                     plot_->interpBoxBorderColor(0, 1),
                     plot_->boxBorderAlpha(),
                     plot_->boxBorderWidth(),
                     plot_->boxBorderDash(),
                     plot_->isBoxFilled(),
                     plot_->interpBoxFillColor(0, 1),
                     plot_->boxFillAlpha(),
                     plot_->boxFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  //---

  Qt::Orientation orientation = (! plot_->isHorizontal() ? Qt::Vertical : Qt::Horizontal);

  //---

  painter->setBrush(brush);
  painter->setPen  (pen);

  painter->setPen(whiskerPen);

  CQChartsWhiskerData data;

  data.min    = remapPos(data_.min   );
  data.lower  = remapPos(data_.lower );
  data.median = remapPos(data_.median);
  data.upper  = remapPos(data_.upper );
  data.max    = remapPos(data_.max   );

  CQChartsBoxWhiskerUtil::drawWhiskerBar(plot_, painter, data, pos, orientation,
                                         ww, bw, plot_->cornerSize(), /*isNotched*/false);

  //---

  // draw labels
  if (plot_->isTextVisible()) {
    double wd1 = ww/2.0;
    double wd2 = bw/2.0;

    double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

    if (! plot_->isHorizontal()) {
      plot_->windowToPixel(pos - wd1, remapPos(data_.min   ), px1, py1);
      plot_->windowToPixel(pos - wd2, remapPos(data_.lower ), px2, py2);
      plot_->windowToPixel(pos      , remapPos(data_.median), px3, py3);
      plot_->windowToPixel(pos + wd2, remapPos(data_.upper ), px4, py4);
      plot_->windowToPixel(pos + wd1, remapPos(data_.max   ), px5, py5);
    }
    else {
      plot_->windowToPixel(remapPos(data_.min   ), pos - wd1, px1, py1);
      plot_->windowToPixel(remapPos(data_.lower ), pos - wd2, px2, py2);
      plot_->windowToPixel(remapPos(data_.median), pos      , px3, py3);
      plot_->windowToPixel(remapPos(data_.upper ), pos + wd2, px4, py4);
      plot_->windowToPixel(remapPos(data_.max   ), pos + wd1, px5, py5);
    }

    //---

    plot_->view()->setPlotPainterFont(plot_, painter, plot_->textFont());

    //---

    QPen pen;

    QColor tc = plot_->interpTextColor(0, 1);

    plot_->setPen(pen, true, tc, plot_->textAlpha(),
                  CQChartsLength("0px"), CQChartsLineDash());

    painter->setPen(pen);

    //---

    QString strl = QString("%1").arg(data_.min   );
    QString lstr = QString("%1").arg(data_.lower );
    QString mstr = QString("%1").arg(data_.median);
    QString ustr = QString("%1").arg(data_.upper );
    QString strh = QString("%1").arg(data_.max   );

    if (! plot_->isHorizontal()) {
      drawHText(painter, px1, px5, py1, strl, /*onLeft*/false);
      drawHText(painter, px2, py4, py2, lstr, /*onLeft*/true );
      drawHText(painter, px2, px4, py3, mstr, /*onLeft*/false);
      drawHText(painter, px2, px4, py4, ustr, /*onLeft*/true );
      drawHText(painter, px1, px5, py5, strh, /*onLeft*/false);
    }
    else {
      drawVText(painter, py1, py5, px1, strl, /*onBottom*/true );
      drawVText(painter, py2, py4, px2, lstr, /*onBottom*/false);
      drawVText(painter, py2, py4, px3, mstr, /*onBottom*/true );
      drawVText(painter, py2, py4, px4, ustr, /*onBottom*/false);
      drawVText(painter, py1, py5, px5, strh, /*onBottom*/true );
    }
  }

  //---

  // draw outlier symbols
  if (plot_->isShowOutliers()) {
    CQChartsSymbolData symbol;

    symbol.type = plot_->outlierSymbolType();
    symbol.size = plot_->outlierSymbolSize();

    //---

    QPen   pen;
    QBrush brush;

    plot_->setOutlierSymbolPenBrush(pen, brush, 0, 1);

    plot_->updateObjPenBrushState(this, pen, brush);

    //---

    std::vector<double> ovalues;

    for (auto &o : data_.outliers) {
      double ovalue = remapPos(o);

      ovalues.push_back(ovalue);
    }

    CQChartsBoxWhiskerUtil::drawOutliers(plot_, painter, ovalues, pos, symbol,
                                         pen, brush, orientation);
  }
}

CQChartsGeom::BBox
CQChartsBoxPlotDataObj::
annotationBBox() const
{
  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  double wd1 = ww/2.0;
  double wd2 = bw/2.0;

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  if (! plot_->isHorizontal()) {
    plot_->windowToPixel(pos - wd1, remapPos(data_.min   ), px1, py1);
    plot_->windowToPixel(pos - wd2, remapPos(data_.lower ), px2, py2);
    plot_->windowToPixel(pos      , remapPos(data_.median), px3, py3);
    plot_->windowToPixel(pos + wd2, remapPos(data_.upper ), px4, py4);
    plot_->windowToPixel(pos + wd1, remapPos(data_.max   ), px5, py5);
  }
  else {
    plot_->windowToPixel(remapPos(data_.min   ), pos - wd1, px1, py1);
    plot_->windowToPixel(remapPos(data_.lower ), pos - wd2, px2, py2);
    plot_->windowToPixel(remapPos(data_.median), pos      , px3, py3);
    plot_->windowToPixel(remapPos(data_.upper ), pos + wd2, px4, py4);
    plot_->windowToPixel(remapPos(data_.max   ), pos + wd1, px5, py5);
  }

  //---

  CQChartsGeom::BBox pbbox;

  if (plot_->isTextVisible()) {
    QString strl = QString("%1").arg(data_.min   );
    QString lstr = QString("%1").arg(data_.lower );
    QString mstr = QString("%1").arg(data_.median);
    QString ustr = QString("%1").arg(data_.upper );
    QString strh = QString("%1").arg(data_.max   );

    if (! plot_->isHorizontal()) {
      addHBBox(pbbox, px1, px5, py1, strl, /*onLeft*/false);
      addHBBox(pbbox, px2, py4, py2, lstr, /*onLeft*/true );
      addHBBox(pbbox, px2, px4, py3, mstr, /*onLeft*/false);
      addHBBox(pbbox, px2, px4, py4, ustr, /*onLeft*/true );
      addHBBox(pbbox, px1, px5, py5, strh, /*onLeft*/false);
    }
    else {
      addVBBox(pbbox, py1, py5, px1, strl, /*onBottom*/true );
      addVBBox(pbbox, py2, py4, px2, lstr, /*onBottom*/false);
      addVBBox(pbbox, py2, py4, px3, mstr, /*onBottom*/true );
      addVBBox(pbbox, py2, py4, px4, ustr, /*onBottom*/false);
      addVBBox(pbbox, py1, py5, px5, strh, /*onBottom*/true );
    }
  }
  else {
    if (! plot_->isHorizontal()) {
      pbbox += CQChartsGeom::Point(px5, py1);
      pbbox += CQChartsGeom::Point(px2, py2);
      pbbox += CQChartsGeom::Point(px4, py3);
      pbbox += CQChartsGeom::Point(px2, py4);
      pbbox += CQChartsGeom::Point(px5, py5);
    }
    else {
      pbbox += CQChartsGeom::Point(px1, py5);
      pbbox += CQChartsGeom::Point(px2, py2);
      pbbox += CQChartsGeom::Point(px3, py4);
      pbbox += CQChartsGeom::Point(px4, py2);
      pbbox += CQChartsGeom::Point(px5, py5);
    }
  }

  //---

  CQChartsGeom::BBox bbox;

  plot_->pixelToWindow(pbbox, bbox);

  return bbox;
}

double
CQChartsBoxPlotDataObj::
remapPos(double y) const
{
  // remap to margin -> 1.0 - margin
  if (! plot_->isNormalized())
    return y;

  return CMathUtil::map(y, data_.dataMin, data_.dataMax, ymargin_, 1.0 - ymargin_);
}

//------

CQChartsBoxPlotConnectedObj::
CQChartsBoxPlotConnectedObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                            int groupInd, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), i_(i), n_(n)
{
  initPolygon();
}

QString
CQChartsBoxPlotConnectedObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(i_);
}

QString
CQChartsBoxPlotConnectedObj::
calcTipId() const
{
  QString groupName = plot_->groupIndName(groupInd_);

  const CQChartsBoxPlotConnectedObj::SetWhiskerMap &setWhiskerMap = this->setWhiskerMap();

  int ns = setWhiskerMap.size();

  CQChartsTableTip tableTip;

  tableTip.addTableRow("Group"   , groupName);
  tableTip.addTableRow("Num Sets", ns);

  return tableTip.str();
}

void
CQChartsBoxPlotConnectedObj::
initPolygon()
{
  QPolygonF maxPoly, minPoly;

  const CQChartsBoxPlotConnectedObj::SetWhiskerMap &setWhiskerMap = this->setWhiskerMap();

  for (const auto &setWhiskers : setWhiskerMap) {
    int                           setId   = setWhiskers.first;
    const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

    double min    = whisker.min   ();
    double max    = whisker.max   ();
    double median = whisker.median();

    line_ << CQChartsUtil::toQPoint(CQChartsGeom::Point(setId, median));

    maxPoly << CQChartsUtil::toQPoint(CQChartsGeom::Point(setId, max));
    minPoly << CQChartsUtil::toQPoint(CQChartsGeom::Point(setId, min));
  }

  //---

  int np = maxPoly.count();

  for (int i = 0; i < np; ++i)
    poly_ << maxPoly.at(i);

  for (int i = 0; i < np; ++i)
    poly_ << minPoly.at(np - 1 - i);
}

const CQChartsBoxPlotConnectedObj::SetWhiskerMap &
CQChartsBoxPlotConnectedObj::
setWhiskerMap() const
{
  static CQChartsBoxPlotConnectedObj::SetWhiskerMap dummy;

  int i = 0;

  for (const auto &groupIdWhiskers : plot_->groupWhiskers()) {
    if (i == i_)
      return groupIdWhiskers.second;

    ++i;
  }

  assert(false);

  return dummy;
}

bool
CQChartsBoxPlotConnectedObj::
inside(const CQChartsGeom::Point &p) const
{
  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsBoxPlotConnectedObj::
draw(QPainter *painter)
{
  // draw range polygon
  int np = poly_.count();

  if (np) {
    // set pen and brush
    QPen   ppen;
    QBrush pbrush;

    plot_->setPenBrush(ppen, pbrush,
                       plot_->isBoxBorder(),
                       plot_->interpBoxBorderColor(i_, n_),
                       plot_->boxBorderAlpha(),
                       plot_->boxBorderWidth(),
                       plot_->boxBorderDash(),
                       plot_->isBoxFilled(),
                       plot_->interpBoxFillColor(i_, n_),
                       plot_->boxFillAlpha(),
                       plot_->boxFillPattern());

    plot_->updateObjPenBrushState(this, ppen, pbrush);

    painter->setPen  (ppen);
    painter->setBrush(pbrush);

    //---

    // draw poly
    QPainterPath path;

    path.moveTo(plot_->windowToPixel(poly_.at(0)));

    for (int i = 1; i < np; ++i)
      path.lineTo(plot_->windowToPixel(poly_.at(i)));

    path.closeSubpath();

    painter->drawPath(path);
  }

  //---

  // set pen
  QPen   lpen;
  QBrush lbrush;

  QColor lineColor = plot_->interpBoxBorderColor(i_, n_);

  plot_->setPen(lpen, true, lineColor, plot_->boxBorderAlpha(),
                plot_->boxBorderWidth(), plot_->boxBorderDash());

  plot_->updateObjPenBrushState(this, lpen, lbrush);

  painter->setPen(lpen);

  //---

  // draw median line
  QPolygonF line;

  for (int i = 0; i < line_.count(); ++i)
    line << plot_->windowToPixel(line_.at(i));

  painter->drawPolyline(line);
}

//------

CQChartsBoxPlotObj::
CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect) :
 CQChartsPlotObj(plot, rect), plot_(plot)
{
}

void
CQChartsBoxPlotObj::
drawHText(QPainter *painter, double xl, double xr, double y, const QString &text, bool onLeft)
{
  double margin = plot_->textMargin();

  bool invertX = plot_->isInvertX();

  if (invertX)
    onLeft = ! onLeft;

  double x = ((onLeft && ! invertX) || (! onLeft && invertX) ? xl : xr);

  plot_->view()->setPlotPainterFont(plot_, painter, plot_->textFont());

  QFontMetricsF fm(painter->font());

  double yf = (fm.ascent() - fm.descent())/2.0;

  if (onLeft)
    painter->drawText(QPointF(x - margin - fm.width(text), y + yf), text);
  else
    painter->drawText(QPointF(x + margin, y + yf), text);
}

void
CQChartsBoxPlotObj::
drawVText(QPainter *painter, double yb, double yt, double x, const QString &text, bool onBottom)
{
  double margin = plot_->textMargin();

  bool invertY = plot_->isInvertY();

  if (invertY)
    onBottom = ! onBottom;

  double y = ((onBottom && ! invertY) || (! onBottom && invertY) ? yb : yt);

  plot_->view()->setPlotPainterFont(plot_, painter, plot_->textFont());

  QFontMetricsF fm(painter->font());

  double xf = fm.width(text)/2.0;
  double fa = fm.ascent ();
  double fd = fm.descent();

  if (onBottom)
    painter->drawText(QPointF(x - xf, y + margin + fa), text);
  else
    painter->drawText(QPointF(x - xf, y - margin - fd), text);
}

void
CQChartsBoxPlotObj::
addHBBox(CQChartsGeom::BBox &pbbox, double xl, double xr, double y,
         const QString &text, bool onLeft) const
{
  double margin = plot_->textMargin();

  bool invertX = plot_->isInvertX();

  if (invertX)
    onLeft = ! onLeft;

  double x = ((onLeft && ! invertX) || (! onLeft && invertX) ? xl : xr);

  QFont font = plot_->view()->plotFont(plot_, plot_->textFont());

  QFontMetricsF fm(font);

  double fa = fm.ascent ();
  double fd = fm.descent();
  double yf = (fa - fd)/2.0;

  double tx;

  if (onLeft)
    tx = x - margin - fm.width(text);
  else
    tx = x + margin + fm.width(text);

  pbbox += CQChartsGeom::Point(tx, y + yf - fa);
  pbbox += CQChartsGeom::Point(tx, y + yf + fd);
}

void
CQChartsBoxPlotObj::
addVBBox(CQChartsGeom::BBox &pbbox, double yb, double yt, double x,
         const QString &text, bool onBottom) const
{
  double margin = plot_->textMargin();

  bool invertY = plot_->isInvertY();

  if (invertY)
    onBottom = ! onBottom;

  double y = ((onBottom && ! invertY) || (! onBottom && invertY) ? yb : yt);

  QFont font = plot_->view()->plotFont(plot_, plot_->textFont());

  QFontMetricsF fm(font);

  double xf = fm.width(text)/2.0;
  double fa = fm.ascent ();
  double fd = fm.descent();

  double ty;

  if (onBottom)
    ty = y + margin + fa;
  else
    ty = y - margin - fd;

  pbbox += CQChartsGeom::Point(x - xf, ty);
  pbbox += CQChartsGeom::Point(x + xf, ty);
}

//------

CQChartsBoxPlotPointObj::
CQChartsBoxPlotPointObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                        int setId, int groupInd, const QPointF &p, const QModelIndex &ind,
                        int ig, int ng, int is, int ns, int iv, int nv) :
 CQChartsPlotObj(plot, rect), plot_(plot), setId_(setId), groupInd_(groupInd), p_(p),
 ind_(ind), ig_(ig), ng_(ng), is_(is), ns_(ns), iv_(iv), nv_(nv)
{
  assert(ig >= 0 && ig < ng);
  assert(is >= 0 && is < ns);
  assert(iv >= 0 && iv < nv);
}

QString
CQChartsBoxPlotPointObj::
calcId() const
{
  return QString("%1:%2:%3:%4").arg(typeName()).arg(ig_).arg(is_).arg(iv_);
}

QString
CQChartsBoxPlotPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  QString setName   = plot_->setIdName   (setId_);
  QString groupName = plot_->groupIndName(groupInd_);

  tableTip.addTableRow("Set"  , setName);
  tableTip.addTableRow("Group", groupName);
  tableTip.addTableRow("Ind"  , iv_);

  return tableTip.str();
}

bool
CQChartsBoxPlotPointObj::
inside(const CQChartsGeom::Point &p) const
{
  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  CQChartsGeom::BBox pbbox(px - 4, py - 4, px + 4, py + 4);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsBoxPlotPointObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, ind_.column());
}

void
CQChartsBoxPlotPointObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
  }
}

void
CQChartsBoxPlotPointObj::
draw(QPainter *painter)
{
  CQChartsSymbol symbol = plot_->jitterSymbolType();

  double sx, sy;

  plot_->pixelSymbolSize(plot_->jitterSymbolSize(), sx, sy);

  //---

  // get color index
  int ic = is_;
  int nc = ns_;

  if (ng_ > 1) {
    if (! plot_->hasSets() || ! plot_->isColorBySet()) {
      ic = ig_;
      nc = ng_;
    }
  }

  //---

  // calc stroke and brush
  QPen   pen;
  QBrush brush;

  plot_->setPenBrush(pen, brush,
                     plot_->isOutlierSymbolStroked(),
                     plot_->interpBoxBorderColor(ic, nc), /*alpha*/1.0,
                     /*width*/ CQChartsLength("1px"), CQChartsLineDash(),
                     plot_->isOutlierSymbolFilled(),
                     plot_->interpBoxFillColor(ic, nc), /*alpha*/1.0,
                     CQChartsFillPattern::Type::SOLID);

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  //---

  // draw symbol
  QRectF erect(px - sx, py - sy, 2*sx, 2*sy);

  plot_->drawSymbol(painter, QPointF(px, py), symbol, CMathUtil::avg(sx, sy), pen, brush);
}

//------

CQChartsBoxKeyColor::
CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsBoxKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsBoxKeyColor::
fillBrush() const
{
  QColor c = CQChartsKeyColorBox::fillBrush().color();

  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}

//------

CQChartsBoxKeyText::
CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsBoxKeyText::
interpTextColor(int i, int n) const
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
