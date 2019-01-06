#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsRand.h>
#include <CQCharts.h>
#include <CQPerfMonitor.h>

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

  //---

  CQChartsPlotParameterGroup *primaryGroup = startParameterGroup("Raw Values");

  addColumnsParameter("value", "Value", "valueColumns").
    setNumeric().setRequired().setTip("value column(s)");
  addColumnParameter ("name", "Name", "nameColumn").
    setString().setTip("Name column");
  addColumnParameter ("set", "Set", "setColumn").
    setTip("Set Values");

  endParameterGroup();

  //---

  CQChartsPlotParameterGroup *secondaryGroup = startParameterGroup("Calculated Values");

  addColumnParameter("x"          , "X"           , "xColumn"          ).
    setNumeric().setTip("X Value");
  addColumnParameter("min"        , "Min"         , "minColumn"        ).
    setNumeric().setTip("Min Value");
  addColumnParameter("lowerMedian", "Lower Median", "lowerMedianColumn").
    setNumeric().setTip("Lower Median Value");
  addColumnParameter("median"     , "Median"      , "medianColumn"     ).
    setNumeric().setTip("Media Value");
  addColumnParameter("upperMedian", "Upper Median", "upperMedianColumn").
    setNumeric().setTip("Upper Median Value");
  addColumnParameter("max"        , "Max"         , "maxColumn"        ).
    setNumeric().setTip("Max Value");
  addColumnParameter("outliers"   , "Outliers"    , "outliersColumn"   ).
    setTip("Outlier Values");

  endParameterGroup();

  //---

  primaryGroup  ->setType(CQChartsPlotParameterGroup::Type::PRIMARY  );
  secondaryGroup->setType(CQChartsPlotParameterGroup::Type::SECONDARY);

  primaryGroup  ->setOtherGroupId(secondaryGroup->groupId());
  secondaryGroup->setOtherGroupId(primaryGroup  ->groupId());

  //---

  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("draw bars horizontal");
  addBoolParameter("normalized", "Normalized", "normalized").setTip("normalize data ranges");
  addBoolParameter("notched"   , "Notched"   , "notched"   ).setTip("notch bar");

  addBoolParameter("colorBySet", "Color by Set", "colorBySet").setTip("Color by value set");

  addBoolParameter("pointsJitter" , "Points Jitter" , "pointsJitter" ).
    setTip("jitter data points");
  addBoolParameter("pointsStacked", "Points Stacked", "pointsStacked").
    setTip("stacked data points");

  addBoolParameter("violin"  , "Violin"   , "violin"  ).setTip("violin border");
  addBoolParameter("errorBar", "Error Bar", "errorBar").setTip("error bar");

  //---

  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();
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
 CQChartsObjBoxShapeData    <CQChartsBoxPlot>(this),
 CQChartsObjTextData        <CQChartsBoxPlot>(this),
 CQChartsObjWhiskerLineData <CQChartsBoxPlot>(this),
 CQChartsObjOutlierPointData<CQChartsBoxPlot>(this),
 CQChartsObjJitterPointData <CQChartsBoxPlot>(this)
{
  NoUpdate noUpdate(this);

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
  clearRawWhiskers();
}

//------

void
CQChartsBoxPlot::
setValueColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setSetColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(setColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setMinColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(minColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setLowerMedianColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(lowerMedianColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setMedianColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(medianColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setUpperMedianColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(upperMedianColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setMaxColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(maxColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setOutliersColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(outliersColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setShowOutliers(bool b)
{
  CQChartsUtil::testAndSet(showOutliers_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setConnected(bool b)
{
  CQChartsUtil::testAndSet(connected_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setBoxWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(boxWidth_, l, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setColorBySet(bool b)
{
  CQChartsUtil::testAndSet(colorBySet_, b, [&]() { resetSetHidden(); queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setWhiskerRange(double r)
{
  CQChartsUtil::testAndSet(whiskerRange_, r, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setWhiskerExtent(double r)
{
  CQChartsUtil::testAndSet(whiskerExtent_, r, [&]() { queueDrawObjs(); } );
}

//------

void
CQChartsBoxPlot::
setTextMargin(double r)
{
  CQChartsUtil::testAndSet(textMargin_, r, [&]() { queueDrawObjs(); } );
}

//------

void
CQChartsBoxPlot::
setYMargin(double r)
{
  CQChartsUtil::testAndSet(ymargin_, r, [&]() { queueDrawObjs(); } );
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
  addProperty("options", this, "ymargin");

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
  addProperty("box/stroke", this, "boxBorder"    , "visible"   );
  addProperty("box/stroke", this, "boxCornerSize", "cornerSize");

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
  CQChartsUtil::testAndSet(horizontal_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setNormalized(bool b)
{
  CQChartsUtil::testAndSet(normalized_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setNotched(bool b)
{
  CQChartsUtil::testAndSet(notched_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setPointsJitter(bool b)
{
  CQChartsUtil::testAndSet(pointsJitter_, b, [&]() {
    queueUpdateRangeAndObjs();

    if (b)
      setPointsStacked(false);
  });
}

void
CQChartsBoxPlot::
setPointsStacked(bool b)
{
  CQChartsUtil::testAndSet(pointsStacked_, b, [&]() {
    queueUpdateRangeAndObjs();

    if (b)
      setPointsJitter(false);
  });
}

//---

void
CQChartsBoxPlot::
setViolin(bool b)
{
  CQChartsUtil::testAndSet(violin_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setViolinWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(violinWidth_, l, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setViolinBox(bool b)
{
  CQChartsUtil::testAndSet(violinBox_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsBoxPlot::
setErrorBar(bool b)
{
  CQChartsUtil::testAndSet(errorBar_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setErrorBarType(const ErrorBarType &t)
{
  CQChartsUtil::testAndSet(errorBarType_, t, [&]() { queueUpdateRangeAndObjs(); } );
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

CQChartsGeom::Range
CQChartsBoxPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsBoxPlot::calcRange");

  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  xAxis->setIntegral      (true);
  xAxis->setMajorIncrement(1);

  yAxis->setIntegral      (false);
  yAxis->setMajorIncrement(0);

  //---

  if (! isPreCalc())
    return updateRawRange();
  else
    return updateCalcRange();
}

// calculate box plot from individual values
CQChartsGeom::Range
CQChartsBoxPlot::
updateRawRange() const
{
  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  yAxis->setVisible(true);

  //---

  initGroupData(valueColumns(), nameColumn());

  //---

  updateRawWhiskers();

  //---

  CQChartsGeom::Range dataRange;

  RMinMax xrange;

  //---

  xAxis->clearTickLabels();

  //---

  bool hasSets   = this->hasSets();
  bool hasGroups = this->hasGroups();

  //---

  int ig = 0;

  for (auto &groupIdWhiskers : groupWhiskers_) {
    int                  groupInd      = groupIdWhiskers.first;
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    if (! isWhiskersGrouped() || ! isSetHidden(ig)) {
      int is = 0;

      for (auto &setWhiskers : setWhiskerMap) {
        if (isWhiskersGrouped() || ! isSetHidden(is)) {
          int                     setId   = setWhiskers.first;
          CQChartsBoxPlotWhisker *whisker = setWhiskers.second;

          //---

          QString name;

          if      (isGroupHeaders()) {
            name = whisker->name();
          }
          else if (hasGroups) {
            name = groupIndName(groupInd);
          }
          else if (hasSets) {
            name = this->setIdName(setId);
          }

          whisker->setName(name);

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
            min = whisker->mean() - whisker->stddev();
            max = whisker->mean() + whisker->stddev();
          }
          else {
            if (isShowOutliers()) {
              min = whisker->vmin();
              max = whisker->vmax();
            }
            else {
              min = whisker->min();
              max = whisker->max();
            }
          }

          if (! isNormalized()) {
            if (! isHorizontal()) {
              dataRange.updateRange(x - 0.5, min);
              dataRange.updateRange(x + 0.5, max);
            }
            else {
              dataRange.updateRange(min, x - 0.5);
              dataRange.updateRange(max, x + 0.5);
            }
          }
          else {
            xrange.add(x - 0.5);
            xrange.add(x + 0.5);
          }

          //---

          if (isViolin()) {
            const CQChartsDensity &density = whisker->density();

            if (! isHorizontal()) {
              dataRange.updateRange(x, density.xmin1());
              dataRange.updateRange(x, density.xmax1());
            }
            else {
              dataRange.updateRange(density.xmin1(), x);
              dataRange.updateRange(density.xmax1(), x);
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
    if (xrange.isSet()) {
      if (! isHorizontal()) {
        dataRange.updateRange(xrange.min(), 0.0);
        dataRange.updateRange(xrange.max(), 1.0);
      }
      else {
        dataRange.updateRange(0.0, xrange.min());
        dataRange.updateRange(1.0, xrange.max());
      }
    }
    else {
      dataRange.updateRange(0.0, 0.0);
      dataRange.updateRange(1.0, 1.0);
    }
  }

  //---

//xAxis->setColumn(setColumn());
  yAxis->setColumn(valueColumns().column());

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

  if (valueColumns().count() == 1 && ! yname.length())
    yname = modelHeaderString(valueColumns().column(), ok);

  yAxis->setLabel(yname);

  //---

  return dataRange;
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
CQChartsGeom::Range
CQChartsBoxPlot::
updateCalcRange() const
{
  CQChartsBoxPlot *th = const_cast<CQChartsBoxPlot *>(this);

  CQChartsAxis *xAxis = (! isHorizontal() ? this->xAxis() : this->yAxis());
  CQChartsAxis *yAxis = (! isHorizontal() ? this->yAxis() : this->xAxis());

  yAxis->setVisible(false);

  //---

  CQChartsGeom::Range dataRange;

  RMinMax xrange;

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    using DataList = CQChartsBoxPlot::WhiskerDataList;

   public:
    BoxPlotVisitor(const CQChartsBoxPlot *plot, CQChartsGeom::Range &dataRange,
                   CQChartsGeom::RMinMax &xrange) :
     plot_(plot), dataRange_(dataRange), xrange_(xrange) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addCalcRow(data, dataList_, dataRange_, xrange_);

      return State::OK;
    }

    const DataList &dataList() const { return dataList_; }

   private:
    const CQChartsBoxPlot* plot_ { nullptr };
    CQChartsGeom::Range&   dataRange_;
    CQChartsGeom::RMinMax& xrange_;
    DataList               dataList_;
  };

  BoxPlotVisitor boxPlotVisitor(this, dataRange, xrange);

  visitModel(boxPlotVisitor);

  th->whiskerDataList_ = boxPlotVisitor.dataList();

  //---

  if (isNormalized()) {
    if (xrange.isSet()) {
      if (! isHorizontal()) {
        dataRange.updateRange(xrange.min(), 0.0);
        dataRange.updateRange(xrange.max(), 1.0);
      }
      else {
        dataRange.updateRange(0.0, xrange.min());
        dataRange.updateRange(1.0, xrange.max());
      }
    }
    else {
      dataRange.updateRange(0.0, 0.0);
      dataRange.updateRange(1.0, 1.0);
    }
  }

  //---

  bool ok;

  QString xname = (xLabel().length() ? xLabel() : modelHeaderString(xColumn(), ok));

  xAxis->setLabel(xname);

  //---

  return dataRange;
}

void
CQChartsBoxPlot::
addCalcRow(const ModelVisitor::VisitData &vdata, WhiskerDataList &dataList,
           CQChartsGeom::Range &dataRange, CQChartsGeom::RMinMax &xrange) const
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
      dataRange.updateRange(data.x - 0.5, data.min);
      dataRange.updateRange(data.x + 0.5, data.max);
    }
    else {
      dataRange.updateRange(data.min, data.x - 0.5);
      dataRange.updateRange(data.max, data.x + 0.5);
    }
  }
  else {
    xrange.add(data.x - 0.5);
    xrange.add(data.x + 0.5);
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
updateRawWhiskers() const
{
  CQChartsBoxPlot *th = const_cast<CQChartsBoxPlot *>(this);

  th->clearRawWhiskers();

  //---

  // x data type
  th->setType_ = columnValueType(setColumn());

  th->setValueInd_.clear();

  //---

  // process model data
  class BoxPlotVisitor : public ModelVisitor {
   public:
    BoxPlotVisitor(const CQChartsBoxPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRawWhiskerRow(data);

      return State::OK;
    }

   private:
    const CQChartsBoxPlot *plot_ { nullptr };
  };

  BoxPlotVisitor boxPlotVisitor(this);

  visitModel(boxPlotVisitor);

  //---

  th->isWhiskersGrouped_ = (numGroupWhiskers() > 1);

  //---

  for (auto &groupIdWhiskers : groupWhiskers_) {
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    for (auto &setWhiskers : setWhiskerMap)
      setWhiskers.second->init();
  }
}

void
CQChartsBoxPlot::
clearRawWhiskers()
{
  for (auto &groupWhisker : groupWhiskers_) {
    const SetWhiskerMap &setWhiskerMap = groupWhisker.second;

    for (auto &setWhisker : setWhiskerMap)
      delete setWhisker.second;
  }

  groupWhiskers_.clear();
}

void
CQChartsBoxPlot::
addRawWhiskerRow(const ModelVisitor::VisitData &vdata) const
{
  CQChartsBoxPlot *th = const_cast<CQChartsBoxPlot *>(this);

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
          setId = th->setValueInd_.calcId(i);
      }
      else if (setType_ == ColumnType::REAL) {
        double r = CQChartsVariant::toReal(setVal, ok1);

        if (ok1 && ! CMathUtil::isNaN(r))
          setId = th->setValueInd_.calcId(r);
      }
      else {
        QString s;

        ok1 = CQChartsVariant::toString(setVal, s);

        if (ok1)
          setId = th->setValueInd_.calcId(s);
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

    auto pg = groupWhiskers_.find(groupInd);

    if (pg == groupWhiskers_.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      auto pg1 = th->groupWhiskers_.find(groupInd);

      if (pg1 == th->groupWhiskers_.end())
        pg1 = th->groupWhiskers_.insert(pg1,
          GroupSetWhiskerMap::value_type(groupInd, SetWhiskerMap()));

      pg = groupWhiskers_.find(groupInd);
    }

    const SetWhiskerMap &setWhiskerMap = (*pg).second;

    auto ps = setWhiskerMap.find(setId);

    if (ps == setWhiskerMap.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      SetWhiskerMap &setWhiskerMap1 = const_cast<SetWhiskerMap &>(setWhiskerMap);

      auto ps1 = setWhiskerMap1.find(setId);

      if (ps1 == setWhiskerMap1.end()) {
        CQChartsBoxPlotWhisker *whisker = new CQChartsBoxPlotWhisker;

        whisker->setRange(whiskerRange());

        QString name;
        bool    ok = false;

        if      (isGroupHeaders()) {
          name = modelHeaderString(valueColumn, ok);
        }
        else if (setColumn().isValid()) {
          ok = CQChartsVariant::toString(setVal, name);
        }

        if (ok && name.length())
          whisker->setName(name);

        ps1 = setWhiskerMap1.insert(ps1, SetWhiskerMap::value_type(setId, whisker));
      }

      ps = setWhiskerMap.find(setId);
    }

    (*ps).second->addValue(wv);
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
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsBoxPlot::createObjs");

  NoUpdate noUpdate(const_cast<CQChartsBoxPlot *>(this));

  //---

  bool rc;

  if (! isPreCalc())
    rc = initRawObjs(objs);
  else
    rc = initCalcObjs(objs);

  //---

  return rc;
}

bool
CQChartsBoxPlot::
initRawObjs(PlotObjs &objs) const
{
  double bw2 = lengthPlotSize(boxWidth   (), ! isHorizontal())/2.0;
  double vw2 = lengthPlotSize(violinWidth(), ! isHorizontal())/2.0;

  //---

  int ig = 0;
  int ng = numGroupWhiskers();

  for (const auto &groupIdWhiskers : this->groupWhiskers()) {
    int                  groupInd      = groupIdWhiskers.first;
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    if (! isConnected()) {
      int is = 0;
      int ns = setWhiskerMap.size();

      double sf = (ns > 1 ? 1.0/ns : 1.0);

      for (const auto &setWhiskers : setWhiskerMap) {
        bool hidden = (isWhiskersGrouped() ? isSetHidden(ig) : isSetHidden(is));
        if (hidden) { ++is; continue; }

        int                           setId   = setWhiskers.first;
        const CQChartsBoxPlotWhisker *whisker = setWhiskers.second;

        //----

        double pos = ig;
        double sbw = (isViolin() ? vw2 : bw2);

        if (ns > 1) {
          pos += (is + 1.0)/(ns + 1.0) - 0.5;
          sbw *= sf;
        }

        //---

        // create whisker object
        CQChartsGeom::BBox rect;

        if (! isNormalized()) {
          if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - sbw, whisker->lower(), pos + sbw, whisker->upper());
          else
            rect = CQChartsGeom::BBox(whisker->lower(), pos - sbw, whisker->upper(), pos + sbw);
        }
        else {
          if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - sbw, 0.0, pos + sbw, 1.0);
          else
            rect = CQChartsGeom::BBox(0.0, pos - sbw, 1.0, pos + sbw);
        }

        CQChartsBoxPlotWhiskerObj *boxObj =
          new CQChartsBoxPlotWhiskerObj(this, rect, setId, groupInd, whisker, ig, ng, is, ns);

        objs.push_back(boxObj);

        //---

        if (isShowOutliers()) {
          double ymargin = this->ymargin();

          double osx, osy;

          pixelSymbolSize(outlierSymbolSize(), osx, osy);

          for (auto &o : whisker->outliers()) {
            double ovalue = whisker->rvalue(o);

            CQChartsGeom::BBox rect;

            if (! isNormalized()) {
              if (! isHorizontal())
                rect = CQChartsGeom::BBox(pos - osx, ovalue - osy, pos + osx, ovalue + osy);
              else
                rect = CQChartsGeom::BBox(ovalue - osx, pos - osy, ovalue + osx, pos + osy);
            }
            else {
              double ovalue1 =
                CMathUtil::map(ovalue, whisker->vmin(), whisker->vmax(), ymargin, 1.0 - ymargin);

              if (! isHorizontal())
                rect = CQChartsGeom::BBox(pos - osx, ovalue1 - osy, pos + osx, ovalue1 + osy);
              else
                rect = CQChartsGeom::BBox(ovalue1 - osx, pos - osy, ovalue1 + osx, pos + osy);
            }

            CQChartsBoxPlotOutlierObj *outlierObj =
              new CQChartsBoxPlotOutlierObj(this, rect, setId, groupInd, whisker,
                                            ig, ng, is, ns, o);

            objs.push_back(outlierObj);
          }
        }

        //---

        // create jitter or stacked points
        if      (isPointsJitter()) {
          addJitterPoints(groupInd, setId, pos, whisker, ig, ng, is, ns, objs);
        }
        else if (isPointsStacked()) {
          addStackedPoints(groupInd, setId, pos, whisker, ig, ng, is, ns, objs);
        }

        //---

        ++is;
      }
    }
    else {
      bool hidden = (isWhiskersGrouped() ? isSetHidden(ig) : false);
      if (hidden) { continue; }

      CQChartsGeom::BBox rect = getDataRange();

      CQChartsBoxPlotConnectedObj *connectedObj =
        new CQChartsBoxPlotConnectedObj(this, rect, groupInd, ig, ng);

      objs.push_back(connectedObj);
    }

    //---

    ++ig;
  }

  //---

  return true;
}

void
CQChartsBoxPlot::
addJitterPoints(int groupInd, int setId, double pos, const CQChartsBoxPlotWhisker *whisker,
                int ig, int ng, int is, int ns, PlotObjs &objs) const
{
  double vw2 = lengthPlotSize(violinWidth(), ! isHorizontal())/2.0;

  const CQChartsDensity &density = whisker->density();

  double ymin = density.ymin1();
  double ymax = density.ymax1();

  CQChartsRand::RealInRange rand(-vw2, vw2);

  int nv = whisker->numValues();

  for (int iv = 0; iv < nv; ++iv) {
    const CQChartsBoxPlotValue &value = whisker->value(iv);

    double d = rand.gen();

    double yv = density.yval(value)/(ymax - ymin);

    double x = pos + yv*d;
    double y = value.value;

    double y1 = (isNormalized() ? whisker->normalize(y, isShowOutliers()) : y);

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

    objs.push_back(pointObj);
  }
}

void
CQChartsBoxPlot::
addStackedPoints(int groupInd, int setId, double pos, const CQChartsBoxPlotWhisker *whisker,
                 int ig, int ng, int is, int ns, PlotObjs &objs) const
{
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

  int nv = whisker->numValues();

  for (int iv = 0; iv < nv; ++iv) {
    const CQChartsBoxPlotValue &value = whisker->value(iv);

    double x = pos;
    double y = value.value;

    double y1 = (isNormalized() ? whisker->normalize(y, isShowOutliers()) : y);

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

    objs.push_back(pointObj);
  }
}

bool
CQChartsBoxPlot::
initCalcObjs(PlotObjs &objs) const
{
  int is = 0;
  int ns = whiskerDataList_.size();

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

    CQChartsBoxPlotDataObj *boxObj =
      new CQChartsBoxPlotDataObj(this, rect, whiskerData, is, ns);

    objs.push_back(boxObj);

    ++pos;

    //---

    if (isShowOutliers()) {
      double ymargin = this->ymargin();

      double osx, osy;

      pixelSymbolSize(outlierSymbolSize(), osx, osy);

      int io = 0;

      for (auto &ovalue : whiskerData.outliers) {
        CQChartsGeom::BBox rect;

        if (! isNormalized()) {
          if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - osx, ovalue - osy, pos + osx, ovalue + osy);
          else
            rect = CQChartsGeom::BBox(ovalue - osx, pos - osy, ovalue + osx, pos + osy);
        }
        else {
          double ovalue1 =
            CMathUtil::map(ovalue, whiskerData.lower, whiskerData.upper, ymargin, 1.0 - ymargin);

          if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - osx, ovalue1 - osy, pos + osx, ovalue1 + osy);
          else
            rect = CQChartsGeom::BBox(ovalue1 - osx, pos - osy, ovalue1 + osx, pos + osy);
        }

        CQChartsBoxPlotOutlierObj *outlierObj =
          new CQChartsBoxPlotOutlierObj(this, rect, -1, -1, nullptr, 0, 1, is, ns, io);

        objs.push_back(outlierObj);

        ++io;
      }
    }

    //---

    ++is;
  }

  return true;
}

void
CQChartsBoxPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int ng = numGroupWhiskers();

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
      //const CQChartsBoxPlotWhisker *whisker = setWhiskers.second;

        QString setName = setIdName(setId);

        CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, is, ns);
        CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, setName, is, ns);

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
        CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, groupName, ig, ng);

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
      const CQChartsBoxPlotWhisker *whisker = setWhiskers.second;

      QString name = whisker->name();

      CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, is, ns);
      CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, name, is, ns);

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
  const CQChartsGeom::Range &dataRange = this->dataRange();

  if (! dataRange.isSet())
    return false;

  if (! isHorizontal()) {
    if (probeData.x < dataRange.xmin() + 0.5)
      probeData.x = dataRange.xmin() + 0.5;

    if (probeData.x > dataRange.xmax() - 0.5)
      probeData.x = dataRange.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
    if (probeData.y < dataRange.ymin() + 0.5)
      probeData.y = dataRange.ymin() + 0.5;

    if (probeData.y > dataRange.ymax() - 0.5)
      probeData.y = dataRange.ymax() - 0.5;

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
CQChartsBoxPlotWhiskerObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                          int groupInd, const CQChartsBoxPlotWhisker *whisker,
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
  return (whisker_ ? whisker_->min() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
lower() const
{
  return (whisker_ ? whisker_->lower() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
median() const
{
  return (whisker_ ? whisker_->median() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
upper() const
{
  return (whisker_ ? whisker_->upper() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
max() const
{
  return (whisker_ ? whisker_->max() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
mean() const
{
  return (whisker_ ? whisker_->mean() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
stddev() const
{
  return (whisker_ ? whisker_->stddev() : 0.0);
}

double
CQChartsBoxPlotWhiskerObj::
notch() const
{
  return (whisker_ ? whisker_->notch() : 0.0);
}

QString
CQChartsBoxPlotWhiskerObj::
calcId() const
{
  return QString("whisker:%1:%2:%3").arg(typeName()).arg(setId_).arg(groupInd_);
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
    name = (whisker_ ? whisker_->name() : "");

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

#if 0
  if (plot_->isShowOutliers()) {
    for (auto &value : whisker_->values())
      addSelectIndex(inds, value.ind.row(), value.ind.column(), value.ind.parent());
  }
#endif
}

void
CQChartsBoxPlotWhiskerObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    for (auto &value : whisker_->values()) {
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

  QColor bc = plot_->interpBoxBorderColor(ic, nc);
  QColor fc = plot_->interpBoxFillColor(ic, nc);

  plot_->setPenBrush(pen, brush,
    plot_->isBoxBorder(), bc, plot_->boxBorderAlpha(),
    plot_->boxBorderWidth(), plot_->boxBorderDash(),
    plot_->isBoxFilled(), fc, plot_->boxFillAlpha(), plot_->boxFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  //---

  // set whisker fill and stroke
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
    const CQChartsDensity &density = whisker_->density();

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
      CQChartsDensity::drawCrossBar(plot_, painter, rect, mean, orientation,
                                    plot_->boxCornerSize());
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
                    plot_->boxBorderWidth(), plot_->boxBorderDash());

      plot_->setBrush(symbolBrush, /*filled*/true, boxColor, plot_->boxFillAlpha(),
                      plot_->boxFillPattern());

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
                                           ww, bw, plot_->boxCornerSize(), plot_->isNotched());
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

        plot_->setPen(pen, true, tc, plot_->textAlpha());

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

#if 0
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

        for (auto &o : whisker_->outliers()) {
          double ovalue = remapPos(whisker_->rvalue(o));

          ovalues.push_back(ovalue);
        }

        CQChartsBoxWhiskerUtil::drawOutliers(plot_, painter, ovalues, pos, symbol,
                                             pen, brush, orientation);
      }
#endif
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

  double ymargin = plot_->ymargin();

  if (plot_->isShowOutliers())
    return CMathUtil::map(y, whisker_->vmin(), whisker_->vmax(), ymargin, 1.0 - ymargin);
  else
    return CMathUtil::map(y, min(), max(), ymargin, 1.0 - ymargin);
}

//------

CQChartsBoxPlotOutlierObj::
CQChartsBoxPlotOutlierObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                          int groupInd, const CQChartsBoxPlotWhisker *whisker,
                          int ig, int ng, int is, int ns, int io) :
 CQChartsBoxPlotObj(plot, rect), setId_(setId), groupInd_(groupInd), whisker_(whisker),
 ig_(ig), ng_(ng), is_(is), ns_(ns), io_(io)
{
}

QString
CQChartsBoxPlotOutlierObj::
calcId() const
{
  return QString("outlier:%1:%2:%3:%4").arg(typeName()).arg(setId_).arg(groupInd_).arg(io_);
}

QString
CQChartsBoxPlotOutlierObj::
calcTipId() const
{
  QString setName, groupName, name;

  if (plot_->hasSets())
    setName = plot_->setIdName(setId_);

  if (plot_->hasGroups())
    groupName = plot_->groupIndName(groupInd_);

  if (! setName.length() && ! groupName.length())
    name = whisker_->name();

  CQChartsTableTip tableTip;

  if (setName.length())
    tableTip.addTableRow("Set", setName);

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  if (name.length())
    tableTip.addTableRow("Name", name);

  double ovalue = whisker_->rvalue(io_);

  tableTip.addTableRow("Value", ovalue);

  return tableTip.str();
}

void
CQChartsBoxPlotOutlierObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->setColumn  ());
  addColumnSelectIndex(inds, plot_->groupColumn());
}

void
CQChartsBoxPlotOutlierObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    const CQChartsBoxPlotValue &ovalue = whisker_->value(io_);

    addSelectIndex(inds, ovalue.ind.row(), column, ovalue.ind.parent());
  }
}

void
CQChartsBoxPlotOutlierObj::
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

  plot_->setOutlierSymbolPenBrush(pen, brush, ic, nc);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  //---

  // draw symbol
  double sx, sy;

  plot_->pixelSymbolSize(plot_->outlierSymbolSize(), sx, sy);

  double ox = rect_.getXYMid(! plot_->isHorizontal());
  double oy = rect_.getXYMid(  plot_->isHorizontal());

  QPointF ppos = plot_->windowToPixel(QPointF(ox, oy));

  plot_->drawSymbol(painter, ppos, plot_->outlierSymbolType(),
                    CMathUtil::avg(sx, sy), pen, brush);
}

double
CQChartsBoxPlotOutlierObj::
remapPos(double y) const
{
  // remap to margin -> 1.0 - margin
  if (! plot_->isNormalized())
    return y;

  double ymargin = plot_->ymargin();

  return CMathUtil::map(y, whisker_->vmin(), whisker_->vmax(), ymargin, 1.0 - ymargin);
}

//------

CQChartsBoxPlotDataObj::
CQChartsBoxPlotDataObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                       const CQChartsBoxWhiskerData &data, int is, int ns) :
 CQChartsBoxPlotObj(plot, rect), data_(data), is_(is), ns_(ns)
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
  return QString("whisker:%1:%2").arg(typeName()).arg(data_.name);
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
  // set whisker fill and stroke
  QPen   whiskerPen;
  QBrush whiskerBrush;

  plot_->setWhiskerLineDataPen(whiskerPen, 0, 1);

  plot_->setBrush(whiskerBrush, false);

  plot_->updateObjPenBrushState(this, whiskerPen, whiskerBrush);

  //---

  // set fill and stroke
  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpBoxBorderColor(0, 1);
  QColor fc = plot_->interpBoxFillColor(0, 1);

  plot_->setPenBrush(pen, brush,
    plot_->isBoxBorder(), bc, plot_->boxBorderAlpha(),
    plot_->boxBorderWidth(), plot_->boxBorderDash(),
    plot_->isBoxFilled(), fc, plot_->boxFillAlpha(), plot_->boxFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  //---

  double pos = this->pos();

  double ww = plot_->whiskerExtent();
  double bw = plot_->lengthPlotSize(plot_->boxWidth(), plot_->isHorizontal());

  //---

  Qt::Orientation orientation = (! plot_->isHorizontal() ? Qt::Vertical : Qt::Horizontal);

  //---

  painter->setPen(whiskerPen);

  CQChartsWhiskerData data;

  data.min    = remapPos(data_.min   );
  data.lower  = remapPos(data_.lower );
  data.median = remapPos(data_.median);
  data.upper  = remapPos(data_.upper );
  data.max    = remapPos(data_.max   );

  CQChartsBoxWhiskerUtil::drawWhiskerBar(plot_, painter, data, pos, orientation,
                                         ww, bw, plot_->boxCornerSize(), /*isNotched*/false);

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

    plot_->setPen(pen, true, tc, plot_->textAlpha());

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

#if 0
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
#endif
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

  double ymargin = plot_->ymargin();

  return CMathUtil::map(y, data_.dataMin, data_.dataMax, ymargin, 1.0 - ymargin);
}

//------

CQChartsBoxPlotConnectedObj::
CQChartsBoxPlotConnectedObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                            int groupInd, int i, int n) :
 CQChartsPlotObj(const_cast<CQChartsBoxPlot *>(plot), rect), plot_(plot),
 groupInd_(groupInd), i_(i), n_(n)
{
  initPolygon();
}

QString
CQChartsBoxPlotConnectedObj::
calcId() const
{
  return QString("connected:%1:%2").arg(typeName()).arg(i_);
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
    const CQChartsBoxPlotWhisker *whisker = setWhiskers.second;

    double min    = whisker->min   ();
    double max    = whisker->max   ();
    double median = whisker->median();

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

    QColor bc = plot_->interpBoxBorderColor(i_, n_);
    QColor fc = plot_->interpBoxFillColor(i_, n_);

    plot_->setPenBrush(ppen, pbrush,
      plot_->isBoxBorder(), bc, plot_->boxBorderAlpha(),
      plot_->boxBorderWidth(), plot_->boxBorderDash(),
      plot_->isBoxFilled(), fc, plot_->boxFillAlpha(), plot_->boxFillPattern());

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
CQChartsBoxPlotObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect) :
 CQChartsPlotObj(const_cast<CQChartsBoxPlot *>(plot), rect), plot_(plot)
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
CQChartsBoxPlotPointObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                        int setId, int groupInd, const QPointF &p, const QModelIndex &ind,
                        int ig, int ng, int is, int ns, int iv, int nv) :
 CQChartsPlotObj(const_cast<CQChartsBoxPlot *>(plot), rect), plot_(plot), setId_(setId),
 groupInd_(groupInd), p_(p), ind_(ind), ig_(ig), ng_(ng), is_(is), ns_(ns), iv_(iv), nv_(nv)
{
  assert(ig >= 0 && ig < ng);
  assert(is >= 0 && is < ns);
  assert(iv >= 0 && iv < nv);
}

QString
CQChartsBoxPlotPointObj::
calcId() const
{
  return QString("point:%1:%2:%3:%4").arg(typeName()).arg(ig_).arg(is_).arg(iv_);
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

  QColor bc = plot_->interpBoxBorderColor(ic, nc);
  QColor fc = plot_->interpBoxFillColor(ic, nc);

  plot_->setPenBrush(pen, brush,
    plot_->isOutlierSymbolStroked(), bc, /*alpha*/1.0, CQChartsLength(), CQChartsLineDash(),
    plot_->isOutlierSymbolFilled(), fc, /*alpha*/1.0, plot_->boxFillPattern());

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
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->queueUpdateRangeAndObjs();

  return true;
}

QBrush
CQChartsBoxKeyColor::
fillBrush() const
{
  QColor c = CQChartsKeyColorBox::fillBrush().color();

  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

//------

CQChartsBoxKeyText::
CQChartsBoxKeyText(CQChartsBoxPlot *plot, const QString &text, int i, int n) :
 CQChartsKeyText(plot, text, i, n)
{
}

QColor
CQChartsBoxKeyText::
interpTextColor(int i, int n) const
{
  CQChartsBoxPlot *plot = qobject_cast<CQChartsBoxPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}
