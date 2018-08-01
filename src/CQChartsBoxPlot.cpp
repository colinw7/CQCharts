#include <CQChartsBoxPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsTip.h>
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

  addBoolParameter("horizontal"  , "Horizontal"  , "horizontal"  ).setTip("draw bars horizontal");
  addBoolParameter("normalized"  , "Normalized"  , "normalized"  ).setTip("normalize data ranges");
  addBoolParameter("jitterPoints", "JitterPoints", "jitterPoints").setTip("jitter data points");

  endParameterGroup();

  //---

  startParameterGroup("Raw Values");

  addColumnsParameter("value", "Value", "valueColumns", "1").setTip("value column(s)");
  addColumnParameter ("name" , "Name" , "nameColumn"       ).setTip("Name column");
  addColumnParameter ("set"  , "Set"  , "setColumn"        ).setTip("X Values");

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
         "<li>Raw Values with X and Y values in <b>x</b> and <b>y</b> columns.</li>\n"
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
 CQChartsGroupPlot(view, view->charts()->plotType("box"), model)
{
  setBoxColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setBorderStroked(true);
  setBoxFilled    (true);

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolSize(CQChartsLength("4px"));
  setSymbolFilled(true);
  setSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

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
setWhiskerRange(double r)
{
  CQChartsUtil::testAndSet(whiskerRange_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBoxPlot::
setBoxWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(boxWidth_, l, [&]() { invalidateLayers(); } );
}

//---

bool
CQChartsBoxPlot::
isBoxFilled() const
{
  return boxData_.shape.background.visible;
}

void
CQChartsBoxPlot::
setBoxFilled(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.visible, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
boxColor() const
{
  return boxData_.shape.background.color;
}

void
CQChartsBoxPlot::
setBoxColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBoxPlot::
interpBoxColor(int i, int n) const
{
  return boxColor().interpColor(this, i, n);
}

double
CQChartsBoxPlot::
boxAlpha() const
{
  return boxData_.shape.background.alpha;
}

void
CQChartsBoxPlot::
setBoxAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.alpha, a, [&]() { invalidateLayers(); } );
}

CQChartsBoxPlot::Pattern
CQChartsBoxPlot::
boxPattern() const
{
  return (Pattern) boxData_.shape.background.pattern;
}

void
CQChartsBoxPlot::
setBoxPattern(Pattern pattern)
{
  if (pattern != (Pattern) boxData_.shape.background.pattern) {
    boxData_.shape.background.pattern = (CQChartsFillData::Pattern) pattern;

    invalidateLayers();
  }
}

//------

bool
CQChartsBoxPlot::
isBorderStroked() const
{
  return boxData_.shape.border.visible;
}

void
CQChartsBoxPlot::
setBorderStroked(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.visible, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
borderColor() const
{
  return boxData_.shape.border.color;
}

void
CQChartsBoxPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBoxPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsBoxPlot::
borderAlpha() const
{
  return boxData_.shape.border.alpha;
}

void
CQChartsBoxPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.alpha, a, [&]() { invalidateLayers(); } );
}

const CQChartsLength &
CQChartsBoxPlot::
borderWidth() const
{
  return boxData_.shape.border.width;
}

void
CQChartsBoxPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.width, l, [&]() { invalidateLayers(); } );
}

const CQChartsLineDash &
CQChartsBoxPlot::
borderDash() const
{
  return boxData_.shape.border.dash;
}

void
CQChartsBoxPlot::
setBorderDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.dash, d, [&]() { invalidateLayers(); } );
}

const CQChartsLength &
CQChartsBoxPlot::
cornerSize() const
{
  return boxData_.cornerSize;
}

void
CQChartsBoxPlot::
setCornerSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(boxData_.cornerSize, s, [&]() { invalidateLayers(); } );
}

//------

const CQChartsColor &
CQChartsBoxPlot::
whiskerColor() const
{
  return whiskerData_.color;
}

void
CQChartsBoxPlot::
setWhiskerColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(whiskerData_.color, c, [&]() { invalidateLayers(); } );
}

double
CQChartsBoxPlot::
whiskerAlpha() const
{
  return whiskerData_.alpha;
}

void
CQChartsBoxPlot::
setWhiskerAlpha(double a)
{
  CQChartsUtil::testAndSet(whiskerData_.alpha, a, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBoxPlot::
interpWhiskerColor(int i, int n) const
{
  return whiskerColor().interpColor(this, i, n);
}

void
CQChartsBoxPlot::
setWhiskerLineWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(whiskerData_.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsBoxPlot::
setWhiskerExtent(double r)
{
  CQChartsUtil::testAndSet(whiskerExtent_, r, [&]() { invalidateLayers(); } );
}

//------

bool
CQChartsBoxPlot::
isTextVisible() const
{
  return textData_.visible;
}

void
CQChartsBoxPlot::
setTextVisible(bool b)
{
  CQChartsUtil::testAndSet(textData_.visible, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
textColor() const
{
  return textData_.color;
}

void
CQChartsBoxPlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { invalidateLayers(); } );
}

double
CQChartsBoxPlot::
textAlpha() const
{
  return textData_.alpha;
}

void
CQChartsBoxPlot::
setTextAlpha(double a)
{
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBoxPlot::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(this, i, n);
}

const QFont &
CQChartsBoxPlot::
textFont() const
{
  return textData_.font;
}

void
CQChartsBoxPlot::
setTextFont(const QFont &f)
{
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { invalidateLayers(); } );
}

void
CQChartsBoxPlot::
setTextMargin(double r)
{
  CQChartsUtil::testAndSet(textMargin_, r, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsBoxPlot::
setSymbolType(const CQChartsSymbol &t)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.type, t, [&]() { invalidateLayers(); } );
}

void
CQChartsBoxPlot::
setSymbolSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.size, s, [&]() { invalidateLayers(); } );
}

void
CQChartsBoxPlot::
setSymbolStroked(bool b)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.stroke.visible, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
symbolStrokeColor() const
{
  return outlierSymbolData_.stroke.color;
}

void
CQChartsBoxPlot::
setSymbolStrokeColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.stroke.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBoxPlot::
interpSymbolStrokeColor(int i, int n) const
{
  return symbolStrokeColor().interpColor(this, i, n);
}

double
CQChartsBoxPlot::
symbolStrokeAlpha() const
{
  return outlierSymbolData_.stroke.alpha;
}

void
CQChartsBoxPlot::
setSymbolStrokeAlpha(double a)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.stroke.alpha, a, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsBoxPlot::
symbolFillColor() const
{
  return outlierSymbolData_.fill.color;
}

void
CQChartsBoxPlot::
setSymbolStrokeWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.stroke.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsBoxPlot::
setSymbolFilled(bool b)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.fill.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsBoxPlot::
setSymbolFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.fill.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBoxPlot::
interpSymbolFillColor(int i, int n) const
{
  return symbolFillColor().interpColor(this, i, n);
}

double
CQChartsBoxPlot::
symbolFillAlpha() const
{
  return outlierSymbolData_.fill.alpha;
}

void
CQChartsBoxPlot::
setSymbolFillAlpha(double a)
{
  CQChartsUtil::testAndSet(outlierSymbolData_.fill.alpha, a, [&]() { invalidateLayers(); } );
}

CQChartsBoxPlot::Pattern
CQChartsBoxPlot::
symbolFillPattern() const
{
  return (Pattern) outlierSymbolData_.fill.pattern;
}

void
CQChartsBoxPlot::
setSymbolFillPattern(const Pattern &pattern)
{
  if (pattern != (Pattern) outlierSymbolData_.fill.pattern) {
    outlierSymbolData_.fill.pattern = (CQChartsFillData::Pattern) pattern;

    invalidateLayers();
  }
}

//---

void
CQChartsBoxPlot::
setJitterSymbolType(const CQChartsSymbol &t)
{
  CQChartsUtil::testAndSet(jitterSymbolData_.type, t, [&]() { invalidateLayers(); } );
}

void
CQChartsBoxPlot::
setJitterSymbolSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(jitterSymbolData_.size, s, [&]() { invalidateLayers(); } );
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

  addProperty("options", this, "horizontal"  , "horizontal"  );
  addProperty("options", this, "normalized"  , "normalized"  );
  addProperty("options", this, "jitterPoints", "jitterPoints");

  // whisker box
  addProperty("box", this, "whiskerRange", "range");
  addProperty("box", this, "boxWidth"    , "width");

  // whisker box stroke
  addProperty("box/stroke", this, "boxStroked" , "visible"   );
  addProperty("box/stroke", this, "cornerSize" , "cornerSize");

  addLineProperties("box/stroke", "border");

  // whisker box fill
  addProperty("box/fill", this, "boxFilled", "visible");

  addFillProperties("box/fill", "box");

  // whisker line
  addProperty("whisker", this, "whiskerColor"    , "color" );
  addProperty("whisker", this, "whiskerAlpha"    , "alpha" );
  addProperty("whisker", this, "whiskerLineWidth", "width" );
  addProperty("whisker", this, "whiskerExtent"   , "extent");

  // value labels
  addProperty("labels", this, "textVisible", "visible");
  addProperty("labels", this, "textFont"   , "font"   );
  addProperty("labels", this, "textColor"  , "color"  );
  addProperty("labels", this, "textAlpha"  , "alpha"  );
  addProperty("labels", this, "textMargin" , "margin" );

  // outlier
  addProperty("outlier", this, "showOutliers", "visible");

  addSymbolProperties("outlier/symbol");
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
setJitterPoints(bool b)
{
  CQChartsUtil::testAndSet(jitterPoints_, b, [&]() { updateRangeAndObjs(); } );
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
updateRange(bool apply)
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

  if (apply)
    applyDataRange();
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

  int pos = 0;
  int ig  = 0;

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
          else if (hasSets) {
            name = this->setIdName(setId);
          }
          else if (hasGroups) {
            name = groupIndName(groupInd);
          }

          whisker.setName(name);

          //---

          int x;

          if (hasSets && isConnected())
            x = setId;
          else
            x = pos;

          //---

          if (name.length())
            xAxis->setTickLabel(x, name);

          //---

          double min, max;

          if (isShowOutliers()) {
            min = whisker.rvalue(0);
            max = whisker.rvalue(whisker.numValues() - 1);
          }
          else {
            min = whisker.min();
            max = whisker.max();
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

          ++pos;
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

  QString xname = (xLabel().length() ? xLabel() : modelHeaderString(setColumn(), ok));

  xAxis->setLabel(xname);

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

    State visit(QAbstractItemModel *, const QModelIndex &ind, int row) override {
      plot_->addCalcRow(ind, row, dataList_);

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
addCalcRow(const QModelIndex &ind, int row, WhiskerDataList &dataList)
{
  CQChartsBoxWhiskerData data;

  bool ok;

  data.ind = ind;

  if (xColumn().isValid())
    data.x = modelReal(row, xColumn(), ind, ok);
  else
    data.x = row;

  data.min    = modelReal(row, minColumn        (), ind, ok);
  data.lower  = modelReal(row, lowerMedianColumn(), ind, ok);
  data.median = modelReal(row, medianColumn     (), ind, ok);
  data.upper  = modelReal(row, upperMedianColumn(), ind, ok);
  data.max    = modelReal(row, maxColumn        (), ind, ok);

  data.dataMin = data.min;
  data.dataMax = data.max;

  if (isShowOutliers()) {
    data.outliers = modelReals(row, outliersColumn(), ind, ok);

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

  data.name = modelString(row, idColumn(), ind, ok);

  if (! ok || ! data.name.length()) {
    data.name = modelHeaderString(row, Qt::Vertical, ok);

    if (! ok || ! data.name.length()) {
      data.name = QString("%1").arg(row);
      nameValid = false;
    }
  }

  if (nameValid)
    xAxis_->setTickLabel(row, data.name);

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

    State visit(QAbstractItemModel *, const QModelIndex &ind, int row) override {
      plot_->addRawWhiskerRow(ind, row);

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
addRawWhiskerRow(const QModelIndex &parent, int row)
{
  // get value set id
  int      setId = -1;
  QVariant setVal;

  if (setColumn().isValid()) {
    bool ok1;

    setVal = modelHierValue(row, setColumn(), parent, ok1);

    if (ok1) {
      if      (setType_ == ColumnType::INTEGER) {
        int i = CQChartsUtil::toInt(setVal, ok1);

        if (ok1)
          setId = setValueInd_.calcId(i);
      }
      else if (setType_ == ColumnType::REAL) {
        double r = CQChartsUtil::toReal(setVal, ok1);

        if (ok1 && ! CQChartsUtil::isNaN(r))
          setId = setValueInd_.calcId(r);
      }
      else {
        QString s;

        ok1 = CQChartsUtil::variantToString(setVal, s);

        if (ok1)
          setId = setValueInd_.calcId(s);
      }
    }
  }

  //---

  //QModelIndex xind  = modelIndex(row, setColumn(), parent);
  //QModelIndex xind1 = normalizeIndex(xind);

  for (const auto &valueColumn : valueColumns()) {
    CQChartsModelIndex ind(row, valueColumn, parent);

    // get group
    int groupInd = rowGroupInd(ind);

    //---

    // add value to set
    bool ok2;

    double value = modelReal(row, valueColumn, parent, ok2);

    if (! ok2) value = row;

    if (CQChartsUtil::isNaN(value))
      return;

    QModelIndex yind  = modelIndex(row, valueColumn, parent);
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
        ok = CQChartsUtil::variantToString(setVal, name);
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
  int ig = 0;
  int ng = numGroups();

  int pos = 0;

  for (const auto &groupIdWhiskers : this->groupWhiskers()) {
    int                  groupInd      = groupIdWhiskers.first;
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    if (! isConnected()) {
      int is = 0;
      int ns = setWhiskerMap.size();

      for (const auto &setWhiskers : setWhiskerMap) {
        bool hidden = (isGrouped() ? isSetHidden(ig) : isSetHidden(is));
        if (hidden) { ++is; continue; }

        int                           setId   = setWhiskers.first;
        const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

        //----

        CQChartsGeom::BBox rect;

        if (! isNormalized()) {
          if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - 0.10, whisker.lower(), pos + 0.10, whisker.upper());
          else
            rect = CQChartsGeom::BBox(whisker.lower(), pos - 0.10, whisker.upper(), pos + 0.10);
        }
        else {
           if (! isHorizontal())
            rect = CQChartsGeom::BBox(pos - 0.10, 0.0, pos + 0.10, 1.0);
          else
            rect = CQChartsGeom::BBox(0.0, pos - 0.10, 1.0, pos + 0.10);
        }

        CQChartsBoxPlotWhiskerObj *boxObj =
          new CQChartsBoxPlotWhiskerObj(this, rect, setId, groupInd, whisker, ig, ng, is, ns);

        addPlotObject(boxObj);

        //---

        if (isJitterPoints()) {
          CQChartsRand::RealInRange rand(-0.4, 0.4);

          int nv = whisker.numValues();

          for (int iv = 0; iv < nv; ++iv) {
            double d = rand.gen();

            const CQChartsBoxPlotValue &value = whisker.value(iv);

            double x = pos + d;
            double y = value.value;

            double y1;

            if (! isNormalized())
              y1 = y;
            else
              y1 = CQChartsUtil::map(y, whisker.min(), whisker.max(), 0.0, 1.0);

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

        //---

        ++pos;

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
  int ig = 0;
  int ng = numGroups();

  //bool hasSets = this->hasSets();

  for (const auto &groupIdWhiskers : this->groupWhiskers()) {
    int                  groupInd      = groupIdWhiskers.first;
    const SetWhiskerMap &setWhiskerMap = groupIdWhiskers.second;

    QString groupName = groupIndName(groupInd);

    int is = 0;
    int ns = setWhiskerMap.size();

    for (const auto &setWhiskers : setWhiskerMap) {
    //int                           setId   = setWhiskers.first;
      const CQChartsBoxPlotWhisker &whisker = setWhiskers.second;

      QString name;

      if (isGrouped())
        name = groupName;
      else
        name = whisker.name();

      int i = (isGrouped() ? ig : is);
      int n = (isGrouped() ? ng : ns);

      CQChartsBoxKeyColor *color = new CQChartsBoxKeyColor(this, i, n);
      CQChartsBoxKeyText  *text  = new CQChartsBoxKeyText (this, i, name);

      key->addItem(color, i, 0);
      key->addItem(text , i, 1);

      if (isGrouped())
        break;

      ++is;
    }

    ++ig;
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
  QAction *horizontalAction = new QAction("Horizontal", menu);

  horizontalAction->setCheckable(true);
  horizontalAction->setChecked(isHorizontal());

  connect(horizontalAction, SIGNAL(triggered(bool)), this, SLOT(setHorizontal(bool)));

  //---

  QAction *normalizedAction = new QAction("Normalized", menu);

  normalizedAction->setCheckable(true);
  normalizedAction->setChecked(isNormalized());

  connect(normalizedAction, SIGNAL(triggered(bool)), this, SLOT(setNormalized(bool)));

  //---

  menu->addSeparator();

  menu->addAction(horizontalAction);
  menu->addAction(normalizedAction);

  //---

  if (! isPreCalc()) {
    QAction *jitterAction = new QAction("Jitter", menu);

    jitterAction->setCheckable(true);
    jitterAction->setChecked(isJitterPoints());

    connect(jitterAction, SIGNAL(triggered(bool)), this, SLOT(setJitterPoints(bool)));

    //---

    menu->addAction(jitterAction);
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

QString
CQChartsBoxPlotWhiskerObj::
calcId() const
{
  return QString("box:%1:%2").arg(setId_).arg(groupInd_);
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

  tableTip.addTableRow("Min"   , min   ());
  tableTip.addTableRow("Lower" , lower ());
  tableTip.addTableRow("Median", median());
  tableTip.addTableRow("Upper" , upper ());
  tableTip.addTableRow("Max"   , max   ());

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

void
CQChartsBoxPlotWhiskerObj::
draw(QPainter *painter)
{
  double pos = rect_.getXYMid(! plot_->isHorizontal());

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

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

  bool hasRange = (fabs(max() - min()) > 1E-6);

  //---

  QColor whiskerColor = plot_->interpWhiskerColor(0, 1);

  whiskerColor.setAlphaF(plot_->whiskerAlpha());

  double whiskerLineSize =
    plot_->lengthPixelSize(plot_->whiskerLineWidth(), ! plot_->isHorizontal());

  //---

  // draw extent line
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal())
    painter->drawLine(QPointF(px3, py1), QPointF(px3, py5));
  else
    painter->drawLine(QPointF(px1, py3), QPointF(px5, py3));

  //---

  // draw lower/upper horizontal lines
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal()) {
    painter->drawLine(QPointF(px1, py1), QPointF(px5, py1));
    painter->drawLine(QPointF(px1, py5), QPointF(px5, py5));
  }
  else {
    painter->drawLine(QPointF(px1, py1), QPointF(px1, py5));
    painter->drawLine(QPointF(px5, py1), QPointF(px5, py5));
  }

  //---

  // draw box
  QRectF rect(px2, py2, px4 - px2, py4 - py2);

  // set fill and stroke
  QBrush brush;

  if (plot_->isBoxFilled()) {
    QColor boxColor = (plot_->isGrouped() ? plot_->interpBoxColor(ig_, ng_) :
                                            plot_->interpBoxColor(is_, ns_));

    boxColor.setAlphaF(plot_->boxAlpha());

    brush.setColor(boxColor);

    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot_->boxPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorderStroked()) {
    QColor borderColor = (plot_->isGrouped() ? plot_->interpBorderColor(ig_, ng_) :
                                               plot_->interpBorderColor(is_, ns_));

    borderColor.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (borderColor);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  double cxs = plot_->lengthPixelWidth (plot_->cornerSize());
  double cys = plot_->lengthPixelHeight(plot_->cornerSize());

  CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);

  //---

  // draw median line
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal())
    painter->drawLine(QPointF(px2, py3), QPointF(px4, py3));
  else
    painter->drawLine(QPointF(px3, py2), QPointF(px3, py4));

  //---

  // draw labels
  if (plot_->isTextVisible()) {
    painter->setFont(plot_->textFont());

    QColor tc = plot_->interpTextColor(0, 1);

    tc.setAlphaF(plot_->textAlpha());

    painter->setPen(tc);

    if (hasRange) {
      QString strl = QString("%1").arg(min   ());
      QString lstr = QString("%1").arg(lower ());
      QString mstr = QString("%1").arg(median());
      QString ustr = QString("%1").arg(upper ());
      QString strh = QString("%1").arg(max   ());

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
      QString strl = QString("%1").arg(min());

      if (! plot_->isHorizontal())
        drawHText(painter, px1, px5, py1, strl, /*onLeft*/true);
      else
        drawVText(painter, py1, py5, px1, strl, /*onBottom*/false);
    }
  }

  //---

  // draw outlier symbols
  if (plot_->isShowOutliers()) {
    CQChartsSymbol symbol      = plot_->symbolType();
    bool           stroked     = plot_->isSymbolStroked();
    QColor         strokeColor = plot_->interpSymbolStrokeColor(0, 1);
    double         strokeAlpha = plot_->symbolStrokeAlpha();
    double         strokeWidth = plot_->lengthPixelWidth(plot_->symbolStrokeWidth());
    bool           filled      = plot_->isSymbolFilled();
    QColor         fillColor   = plot_->interpSymbolFillColor(0, 1);
    double         fillAlpha   = plot_->symbolFillAlpha();

    double sx, sy;

    plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

    strokeColor.setAlphaF(strokeAlpha);
    fillColor  .setAlphaF(fillAlpha);

    //---

    QPen   pen;
    QBrush brush;

    if (stroked)
      pen.setColor(strokeColor);
    else
      pen.setStyle(Qt::NoPen);

    if (filled) {
      brush.setColor(fillColor);
      brush.setStyle(Qt::SolidPattern);
    }
    else
      brush.setStyle(Qt::NoBrush);

    plot_->updateObjPenBrushState(this, pen, brush);

    filled  = (brush.style() != Qt::NoBrush);
    stroked = (pen  .style() != Qt::NoPen  );

    //---

    for (auto &o : whisker_.outliers()) {
      double px1, py1;

      if (! plot_->isHorizontal())
        plot_->windowToPixel(pos, remapPos(whisker_.rvalue(o)), px1, py1);
      else
        plot_->windowToPixel(remapPos(whisker_.rvalue(o)), pos, px1, py1);

      plot_->drawSymbol(painter, QPointF(px1, py1), symbol, CQChartsUtil::avg(sx, sy),
                        stroked, pen.color(), strokeWidth, filled, brush.color());
    }
  }
}

CQChartsGeom::BBox
CQChartsBoxPlotWhiskerObj::
annotationBBox() const
{
  double pos = rect_.getXYMid(! plot_->isHorizontal());

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

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

  bool hasRange = (fabs(max() - min()) > 1E-6);

  //---

  CQChartsGeom::BBox pbbox;

  if (plot_->isTextVisible()) {
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

  return CQChartsUtil::map(y, min(), max(), ymargin, 1.0 - ymargin);
}

//------

CQChartsBoxPlotDataObj::
CQChartsBoxPlotDataObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                       const CQChartsBoxWhiskerData &data) :
 CQChartsBoxPlotObj(plot, rect), data_(data)
{
}

QString
CQChartsBoxPlotDataObj::
calcId() const
{
  return QString("data:%1").arg(data_.name);
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
  double pos = rect_.getXYMid(! plot_->isHorizontal());

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

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

  QColor whiskerColor = plot_->interpWhiskerColor(0, 1);

  whiskerColor.setAlphaF(plot_->whiskerAlpha());

  double whiskerLineSize =
    plot_->lengthPixelSize(plot_->whiskerLineWidth(), ! plot_->isHorizontal());

  //---

  // draw extent line
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal())
    painter->drawLine(QPointF(px3, py1), QPointF(px3, py5));
  else
    painter->drawLine(QPointF(px1, py3), QPointF(px5, py3));

  //---

  // draw lower/upper horizontal lines
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal()) {
    painter->drawLine(QPointF(px1, py1), QPointF(px5, py1));
    painter->drawLine(QPointF(px1, py5), QPointF(px5, py5));
  }
  else {
    painter->drawLine(QPointF(px1, py1), QPointF(px1, py5));
    painter->drawLine(QPointF(px5, py1), QPointF(px5, py5));
  }

  //---

  // draw box
  QRectF rect(px2, py2, px4 - px2, py4 - py2);

  // set fill and stroke
  QBrush brush;

  if (plot_->isBoxFilled()) {
    QColor boxColor = plot_->interpBoxColor(0, 1);

    boxColor.setAlphaF(plot_->boxAlpha());

    brush.setColor(boxColor);

    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot_->boxPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorderStroked()) {
    QColor borderColor = plot_->interpBorderColor(0, 1);

    borderColor.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (borderColor);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setBrush(brush);
  painter->setPen  (pen);

  double cxs = plot_->lengthPixelWidth (plot_->cornerSize());
  double cys = plot_->lengthPixelHeight(plot_->cornerSize());

  CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);

  //---

  // draw median line
  painter->setPen(QPen(whiskerColor, whiskerLineSize, Qt::SolidLine));

  if (! plot_->isHorizontal())
    painter->drawLine(QPointF(px2, py3), QPointF(px4, py3));
  else
    painter->drawLine(QPointF(px3, py2), QPointF(px3, py4));

  //---

  // draw labels
  if (plot_->isTextVisible()) {
    painter->setFont(plot_->textFont());

    QColor tc = plot_->interpTextColor(0, 1);

    tc.setAlphaF(plot_->textAlpha());

    painter->setPen(tc);

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
    CQChartsSymbol symbol      = plot_->symbolType();
    bool           stroked     = plot_->isSymbolStroked();
    QColor         strokeColor = plot_->interpSymbolStrokeColor(0, 1);
    double         strokeAlpha = plot_->symbolStrokeAlpha();
    double         strokeWidth = plot_->lengthPixelWidth(plot_->symbolStrokeWidth());
    bool           filled      = plot_->isSymbolFilled();
    QColor         fillColor   = plot_->interpSymbolFillColor(0, 1);
    double         fillAlpha   = plot_->symbolFillAlpha();

    double sx, sy;

    plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

    strokeColor.setAlphaF(strokeAlpha);
    fillColor  .setAlphaF(fillAlpha);

    //---

    QPen   pen;
    QBrush brush;

    if (stroked)
      pen.setColor(strokeColor);
    else
      pen.setStyle(Qt::NoPen);

    if (filled) {
      brush.setColor(fillColor);
      brush.setStyle(Qt::SolidPattern);
    }
    else
      brush.setStyle(Qt::NoBrush);

    plot_->updateObjPenBrushState(this, pen, brush);

    //---

    for (auto &o : data_.outliers) {
      double px1, py1;

      if (! plot_->isHorizontal())
        plot_->windowToPixel(pos, remapPos(o), px1, py1);
      else
        plot_->windowToPixel(remapPos(o), pos, px1, py1);

      plot_->drawSymbol(painter, QPointF(px1, py1), symbol, CQChartsUtil::avg(sx, sy),
                        stroked, pen.color(), strokeWidth, filled, brush.color());
    }
  }
}

CQChartsGeom::BBox
CQChartsBoxPlotDataObj::
annotationBBox() const
{
  double pos = rect_.getXYMid(! plot_->isHorizontal());

  double wd1 = plot_->whiskerExtent()/2.0;
  double wd2 = plot_->lengthPlotWidth(plot_->boxWidth())/2;

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

  return CQChartsUtil::map(y, data_.dataMin, data_.dataMax, ymargin_, 1.0 - ymargin_);
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
  return QString("connected:%1").arg(i_);
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
    QPainterPath path;

    path.moveTo(plot_->windowToPixel(poly_.at(0)));

    for (int i = 1; i < np; ++i)
      path.lineTo(plot_->windowToPixel(poly_.at(i)));

    path.closeSubpath();

    QColor fillColor = plot_->interpBoxColor(i_, n_);

    QBrush pbrush;

    if (plot_->isBoxFilled()) {
      fillColor.setAlphaF(plot_->boxAlpha());

      pbrush.setColor(fillColor);

      pbrush.setStyle(CQChartsFillPattern::toStyle(
       (CQChartsFillPattern::Type) plot_->boxPattern()));
    }

    QPen ppen;

    if (plot_->isBorderStroked()) {
      QColor borderColor = plot_->interpBorderColor(i_, n_);

      borderColor.setAlphaF(plot_->borderAlpha());

      double bw = plot_->lengthPixelWidth(plot_->borderWidth());

      ppen.setColor (borderColor);
      ppen.setWidthF(bw);
    }
    else {
      ppen.setStyle(Qt::NoPen);
    }

    plot_->updateObjPenBrushState(this, ppen, pbrush);

    painter->setPen  (ppen);
    painter->setBrush(pbrush);

    painter->drawPath(path);
  }

  //---

  // draw median line
  QPolygonF line;

  for (int i = 0; i < line_.count(); ++i)
    line << plot_->windowToPixel(line_.at(i));

  QPen lpen;

  QColor lineColor = plot_->interpBorderColor(i_, n_);

  lineColor.setAlphaF(plot_->borderAlpha());

  double bw = plot_->lengthPixelWidth(plot_->borderWidth());

  lpen.setColor (lineColor);
  lpen.setWidthF(bw);

  QBrush lbrush;

  plot_->updateObjPenBrushState(this, lpen, lbrush);

  painter->setPen(lpen);

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

  QFontMetricsF fm(plot_->textFont());

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

  QFontMetricsF fm(plot_->textFont());

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

  QFontMetricsF fm(plot_->textFont());

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

  QFontMetricsF fm(plot_->textFont());

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
  return QString("point:%1:%2:%3").arg(ig_).arg(is_).arg(iv_);
}

QString
CQChartsBoxPlotPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Set"  , setId_);
  tableTip.addTableRow("Group", groupInd_);
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

  bool stroked = plot_->isSymbolStroked();
  bool filled  = plot_->isSymbolFilled ();

  double sx, sy;

  plot_->pixelSymbolSize(plot_->jitterSymbolSize(), sx, sy);

  //---

  // calc stroke and brush
  QBrush brush;

  if (filled) {
    QColor c;

    if (ng_ > 1)
      c = plot_->interpBoxColor(ig_, ng_);
    else
      c = plot_->interpBoxColor(is_, ns_);

    c.setAlphaF(1.0);

    brush.setColor(c);
    brush.setStyle(Qt::SolidPattern);
  //brush.setStyle(CQChartsFillPattern::toStyle(
  //  (CQChartsFillPattern::Type) plot_->symbolFillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (stroked) {
    QColor c = plot_->interpBorderColor(0, 1);

    c.setAlphaF(1.0);

    double bw = 1;

    pen.setColor (c);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  //---

  // draw symbol
  painter->setPen  (pen);
  painter->setBrush(brush);

  QRectF erect(px - sx, py - sy, 2*sx, 2*sy);

  plot_->drawSymbol(painter, QPointF(px, py), symbol, CQChartsUtil::avg(sx, sy), pen, brush);
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
