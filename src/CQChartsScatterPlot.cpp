#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsColorSet.h>
#include <CQChartsValueSet.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsTip.h>
#include <CQCharts.h>
#include <CMathCorrelation.h>
#include <CMathRound.h>

#include <QPainter>
#include <QMenu>

CQChartsScatterPlotType::
CQChartsScatterPlotType()
{
}

void
CQChartsScatterPlotType::
addParameters()
{
  startParameterGroup("Scatter");

  // columns
  addColumnParameter("x", "X", "xColumn", 0).setTip("X Value").setRequired();
  addColumnParameter("y", "Y", "yColumn", 1).setTip("Y Value").setRequired();

  addColumnParameter("name", "Name", "nameColumn").setTip("Value Name");

  endParameterGroup();

  //---

  // custom columns/map
  startParameterGroup("Points");

  addColumnParameter("symbolType", "Symbol Size", "symbolTypeColumn").
   setTip("Custom Symbol Type").setMapped().setMapMinMax(5, 13);

  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn").
   setTip("Custom Symbol Size").setMapped().setMapMinMax(8, 64);

  addColumnParameter("color", "Color", "colorColumn").
   setTip("Custom Symbol Fill Color").setMapped();

  addColumnParameter("fontSize", "Font Size", "fontSizeColumn"  ).
   setTip("Custom Font Size for Text Label").setMapped().setMapMinMax(8, 48);

  addBoolParameter("textLabels", "Text Labels", "textLabels").
   setTip("Show Text Label at Point");

  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsScatterPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws scatter plot x, y points with support for customization of"
         "point size, color and label font.\n";
}

CQChartsPlot *
CQChartsScatterPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsScatterPlot(view, model);
}

//---

CQChartsScatterPlot::
CQChartsScatterPlot(CQChartsView *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("scatter"), model),
 CQChartsPlotPointData    <CQChartsScatterPlot>(this),
 CQChartsPlotHullShapeData<CQChartsScatterPlot>(this),
 CQChartsPlotRugPointData <CQChartsScatterPlot>(this),
 dataLabel_(this)
{
  // set mapped range
  addValueSet("symbolType", 5, 13)->setMapped(true);
  addValueSet("symbolSize", 8, 64)->setMapped(true);
  addValueSet("fontSize"  , 8, 48)->setMapped(true);

  (void) addColorSet("color")->setMapped(true);

  //---

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setHullFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setRugSymbolType(CQChartsSymbol::Type::NONE);
  setRugSymbolSize(CQChartsLength("5px"));

  setBestFitFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  //---

  addAxes();

  addKey();

  addTitle();
}

//------

void
CQChartsScatterPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setGridded(bool b)
{
  CQChartsUtil::testAndSet(gridData_.enabled, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setGridNumX(int n)
{
  CQChartsUtil::testAndSet(gridData_.nx, n, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setGridNumY(int n)
{
  CQChartsUtil::testAndSet(gridData_.ny, n, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setSymbolMapKey(bool b)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.displayed, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolMapKeyAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.alpha, a, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolMapKeyMargin(double m)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.margin, m, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsScatterPlot::
setSymbolTypeColumn(const CQChartsColumn &c)
{
  if (setValueSetColumn("symbolType", c))
    updateRangeAndObjs();
}

void
CQChartsScatterPlot::
setSymbolTypeMapped(bool b)
{
  setValueSetMapped("symbolType", b);

  updateObjs();
}

void
CQChartsScatterPlot::
setSymbolTypeMapMin(double r)
{
  setValueSetMapMin("symbolType", r); updateObjs(); }

void
CQChartsScatterPlot::
setSymbolTypeMapMax(double r)
{
  setValueSetMapMax("symbolType", r);

  updateObjs();
}

//---

void
CQChartsScatterPlot::
setSymbolSizeColumn(const CQChartsColumn &c)
{
  if (setValueSetColumn("symbolSize", c))
    updateRangeAndObjs();
}

void
CQChartsScatterPlot::
setSymbolSizeMapped(bool b)
{
  setValueSetMapped("symbolSize", b);

  updateObjs();
}

void
CQChartsScatterPlot::
setSymbolSizeMapMin(double r)
{
  setValueSetMapMin("symbolSize", r); updateObjs(); }

void
CQChartsScatterPlot::
setSymbolSizeMapMax(double r)
{
  setValueSetMapMax("symbolSize", r);

  updateObjs();
}

//---

void
CQChartsScatterPlot::
setColorColumn(const CQChartsColumn &c)
{
  if (setValueSetColumn("color", c))
    updateRangeAndObjs();
}

void
CQChartsScatterPlot::
setColorMapped(bool b)
{
  setValueSetMapped("color", b);

  updateObjs();
}

void
CQChartsScatterPlot::
setColorMapMin(double r)
{
  setValueSetMapMin("color", r);

  updateObjs();
}

void
CQChartsScatterPlot::
setColorMapMax(double r)
{
  setValueSetMapMax("color", r);

  updateObjs();
}

//---

void
CQChartsScatterPlot::
setFontSizeColumn(const CQChartsColumn &c)
{
  if (setValueSetColumn("fontSize", c))

  updateRangeAndObjs();
}

void
CQChartsScatterPlot::
setFontSizeMapped(bool b)
{
  setValueSetMapped("fontSize", b);

  updateObjs();
}

void
CQChartsScatterPlot::
setFontSizeMapMin(double r)
{
  setValueSetMapMin("fontSize", r);

  updateObjs();
}

void
CQChartsScatterPlot::
setFontSizeMapMax(double r)
{
  setValueSetMapMax("fontSize", r);

  updateObjs();
}

//---

void
CQChartsScatterPlot::
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setBestFitDeviation(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.showDeviation, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setBestFitOrder(int o)
{
  CQChartsUtil::testAndSet(bestFitData_.order, o, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(bestFitData_.fillColor, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitFillAlpha(double a)
{
  CQChartsUtil::testAndSet(bestFitData_.fillAlpha, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setHull(bool b)
{
  CQChartsUtil::testAndSet(hullData_.visible, b, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsScatterPlot::
setXRug(bool b)
{
  CQChartsUtil::testAndSet(axisRugData_.xVisible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYRug(bool b)
{
  CQChartsUtil::testAndSet(axisRugData_.yVisible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setXRugSide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisRugData_.xSide, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYRugSide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisRugData_.ySide, s, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsScatterPlot::
setXDensity(bool b)
{
  CQChartsUtil::testAndSet(axisDensityData_.xVisible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYDensity(bool b)
{
  CQChartsUtil::testAndSet(axisDensityData_.yVisible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setXDensitySide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisDensityData_.xSide, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYDensitySide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisDensityData_.ySide, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setDensityWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisDensityData_.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setDensityAlpha(double a)
{
  CQChartsUtil::testAndSet(axisDensityData_.alpha, a, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsScatterPlot::
setDensityMap(bool b)
{
  CQChartsUtil::testAndSet(densityMapData_.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setDensityMapGridSize(int s)
{
  CQChartsUtil::testAndSet(densityMapData_.gridSize, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setDensityMapDelta(double d)
{
  CQChartsUtil::testAndSet(densityMapData_.delta, d, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsScatterPlot::
setXWhisker(bool b)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.xVisible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYWhisker(bool b)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.yVisible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setXWhiskerSide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.xSide, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYWhiskerSide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.ySide, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setWhiskerWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setWhiskerMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.margin, l, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setWhiskerAlpha(double a)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.alpha, a, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsScatterPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "xColumn", "x");
  addProperty("columns", this, "yColumn", "y");

  addProperty("columns", this, "nameColumn"      , "name"      );
  addProperty("columns", this, "symbolTypeColumn", "symbolType");
  addProperty("columns", this, "symbolSizeColumn", "symbolSize");
  addProperty("columns", this, "fontSizeColumn"  , "fontSize"  );
  addProperty("columns", this, "colorColumn"     , "color"     );

  // best fit line and deviation fill
  addProperty("bestFit"     , this, "bestFit"         , "enabled"  );
  addProperty("bestFit"     , this, "bestFitDeviation", "deviation");
  addProperty("bestFit"     , this, "bestFitOrder"    , "order"    );
  addProperty("bestFit/fill", this, "bestFitFillColor", "color"    );
  addProperty("bestFit/fill", this, "bestFitFillAlpha", "alpha"    );

  // convex hull shape
  addProperty("hull", this, "hull", "enabled");

  addLineProperties("hull/stroke", "hullBorder");
  addFillProperties("hull/fill"  , "hullFill"  );

  // rug axis
  addProperty("rug/x"     , this, "xRug"         , "enabled");
  addProperty("rug/x"     , this, "xRugSide"     , "side"   );
  addProperty("rug/y"     , this, "yRug"         , "enabled");
  addProperty("rug/y"     , this, "yRugSide"     , "side"   );
  addProperty("rug/symbol", this, "rugSymbolType", "type"   );
  addProperty("rug/symbol", this, "rugSymbolSize", "size"   );

  // density axis
  addProperty("density"     , this, "densityWidth", "width"  );
  addProperty("density/x"   , this, "xDensity"    , "enabled");
  addProperty("density/x"   , this, "xDensitySide", "side"   );
  addProperty("density/y"   , this, "yDensity"    , "enabled");
  addProperty("density/y"   , this, "yDensitySide", "side"   );
  addProperty("density/fill", this, "densityAlpha", "alpha"  );

  // density map
  addProperty("densityMap" , this, "densityMap"        , "enabled" );
  addProperty("densityMap" , this, "densityMapGridSize", "gridSize");
  addProperty("densityMap" , this, "densityMapDelta"   , "delta"   );

  // whisker axis
  addProperty("whisker"     , this, "whiskerWidth" , "width"  );
  addProperty("whisker"     , this, "whiskerMargin", "margin" );
  addProperty("whisker/x"   , this, "xWhisker"     , "enabled");
  addProperty("whisker/x"   , this, "xWhiskerSide" , "side"   );
  addProperty("whisker/y"   , this, "yWhisker"     , "enabled");
  addProperty("whisker/y"   , this, "yWhiskerSide" , "side"   );
  addProperty("whisker/fill", this, "whiskerAlpha" , "alpha"  );

  CQChartsGroupPlot::addProperties();

  addSymbolProperties("symbol");

  // point data labels
  dataLabel_.addPathProperties("dataLabel");

  // symbol key
  addProperty("symbolKey", this, "symbolMapKey"      , "enabled");
  addProperty("symbolKey", this, "symbolMapKeyAlpha" , "alpha"  );
  addProperty("symbolKey", this, "symbolMapKeyMargin", "margin" );

  // grid
  addProperty("grid", this, "gridded" , "enabled");
  addProperty("grid", this, "gridNumX", "nx"     );
  addProperty("grid", this, "gridNumY", "ny"     );

  // mapping for columns (symbol type, size, font size, color)
  addProperty("symbol/map/type", this, "symbolTypeMapped", "enabled");
  addProperty("symbol/map/type", this, "symbolTypeMapMin", "min"    );
  addProperty("symbol/map/type", this, "symbolTypeMapMax", "max"    );

  addProperty("symbol/map/size", this, "symbolSizeMapped", "enabled");
  addProperty("symbol/map/size", this, "symbolSizeMapMin", "min"    );
  addProperty("symbol/map/size", this, "symbolSizeMapMax", "max"    );

  addProperty("font/map/size", this, "fontSizeMapped", "enabled");
  addProperty("font/map/size", this, "fontSizeMapMin", "min"    );
  addProperty("font/map/size", this, "fontSizeMapMax", "max"    );

  // color map
  addColorMapProperties();
}

//---

void
CQChartsScatterPlot::
calcRange()
{
  initGroupData(Columns(), CQChartsColumn());

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsScatterPlot *plot) :
     plot_(plot) {
      hasGroups_ = (plot_->numGroups() > 1);
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex ind(data.row, plot_->xColumn(), data.parent);

      // init group
      int groupInd = plot_->rowGroupInd(ind);

      bool hidden = (hasGroups_ && plot_->isSetHidden(groupInd));

      if (! hidden) {
        bool ok1, ok2;

        double x = plot_->modelReal(data.row, plot_->xColumn(), data.parent, ok1);
        double y = plot_->modelReal(data.row, plot_->yColumn(), data.parent, ok2);

        if (! ok1) { x = uniqueId(data.row, plot_->xColumn(), data.parent); ++uniqueX_; }
        if (! ok2) { y = uniqueId(data.row, plot_->yColumn(), data.parent); ++uniqueY_; }

        if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
          return State::SKIP;

        range_.updateRange(x, y);
      }

      return State::OK;
    }

    int uniqueId(int row, const CQChartsColumn &column, const QModelIndex &parent) {
      bool ok;

      QVariant var = plot_->modelValue(row, column, parent, ok);
      if (! var.isValid()) return -1;

      return columnDetails(column)->uniqueId(var);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        CQChartsModelData *modelData = plot_->charts()->getModelData(plot_->model().data());

        details_ = modelData->details();
      }

      return details_->columnDetails(column);
    }

    const CQChartsGeom::Range &range() const { return range_; }

    bool isUniqueX() const { return uniqueX_ == numRows(); }
    bool isUniqueY() const { return uniqueY_ == numRows(); }

   private:
    CQChartsScatterPlot*  plot_      { nullptr };
    int                   hasGroups_ { false };
    CQChartsGeom::Range   range_;
    CQChartsModelDetails* details_   { nullptr };
    int                   uniqueX_   { 0 };
    int                   uniqueY_   { 0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  dataRange_ = visitor.range();

  bool uniqueX = visitor.isUniqueX();
  bool uniqueY = visitor.isUniqueY();

  if (uniqueX || uniqueY) {
    CQChartsModelData *modelData = charts()->getModelData(model().data());

    CQChartsModelDetails *details = modelData->details();

    if (uniqueX) {
      CQChartsModelColumnDetails *columnDetails = details->columnDetails(xColumn());

      for (int i = 0; i < columnDetails->numUnique(); ++i)
        xAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());

      dataRange_.updateRange(dataRange_.xmin() - 0.5, dataRange_.ymin());
      dataRange_.updateRange(dataRange_.xmax() + 0.5, dataRange_.ymin());
    }

    if (uniqueY) {
      CQChartsModelColumnDetails *columnDetails = details->columnDetails(yColumn());

      for (int i = 0; i < columnDetails->numUnique(); ++i)
        yAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());

      dataRange_.updateRange(dataRange_.xmin(), dataRange_.ymin() - 0.5);
      dataRange_.updateRange(dataRange_.xmax(), dataRange_.ymax() + 0.5);
    }
  }

  //---

  adjustDataRange();

  // update data range if unset
  if (dataRange_.isSet() && CMathUtil::isZero(dataRange_.xsize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x - 1.0, y);
    dataRange_.updateRange(x + 1.0, y);
  }

  if (dataRange_.isSet() && CMathUtil::isZero(dataRange_.ysize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x, y - 1.0);
    dataRange_.updateRange(x, y + 1.0);
  }

  //---

  if (dataRange_.isSet()) {
    gridData_.xinterval.setStart   (dataRange_.xmin());
    gridData_.xinterval.setEnd     (dataRange_.xmax());
    gridData_.xinterval.setNumMajor(gridData_.nx);

    gridData_.yinterval.setStart   (dataRange_.ymin());
    gridData_.yinterval.setEnd     (dataRange_.ymax());
    gridData_.yinterval.setNumMajor(gridData_.ny);
  }
  else {
    gridData_.xinterval.setStart   (0);
    gridData_.xinterval.setEnd     (1);
    gridData_.xinterval.setNumMajor(1);

    gridData_.yinterval.setStart   (0);
    gridData_.yinterval.setEnd     (1);
    gridData_.yinterval.setNumMajor(1);
  }

  //---

  if (isGridded()) {
    if (dataRange_.isSet()) {
      dataRange_.updateRange(gridData_.xinterval.calcStart(), gridData_.yinterval.calcStart());
      dataRange_.updateRange(gridData_.xinterval.calcEnd  (), gridData_.yinterval.calcEnd  ());
    }
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  bool ok;

  QString xname = (xLabel().length() ? xLabel() : modelHeaderString(xColumn(), ok));
  QString yname = (yLabel().length() ? yLabel() : modelHeaderString(yColumn(), ok));

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  xAxis_->setIntegral(uniqueX);
  yAxis_->setIntegral(uniqueY);
}

//------

void
CQChartsScatterPlot::
updateObjs()
{
  groupNameValues_  .clear();
  groupNameGridData_.clear();

  clearValueSets();

  CQChartsPlot::updateObjs();
}

bool
CQChartsScatterPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjects().empty())
    return false;

  //---

  // init value set
  initValueSets();

  //---

  // init name values
  gridData_.maxN = 0;

  if (groupNameValues_.empty())
    addNameValues();

  groupPoints_  .clear();
  groupFitData_ .clear();
  groupHull_    .clear();
  groupWhiskers_.clear();

  //---

  // get column titles
  bool ok;

  xname_          = modelHeaderString(xColumn         (), ok);
  yname_          = modelHeaderString(yColumn         (), ok);
  symbolTypeName_ = modelHeaderString(symbolTypeColumn(), ok);
  symbolSizeName_ = modelHeaderString(symbolSizeColumn(), ok);
  fontSizeName_   = modelHeaderString(fontSizeColumn  (), ok);
  colorName_      = modelHeaderString(colorColumn     (), ok);

  if (! xname_         .length()) xname_          = "x";
  if (! yname_         .length()) yname_          = "y";
  if (! symbolTypeName_.length()) symbolTypeName_ = "symbolType";
  if (! symbolSizeName_.length()) symbolSizeName_ = "symbolSize";
  if (! fontSizeName_  .length()) fontSizeName_   = "fontSize";
  if (! colorName_     .length()) colorName_      = "color";

  //---

  CQChartsValueSet *symbolTypeSet = getValueSet("symbolType");
  CQChartsValueSet *symbolSizeSet = getValueSet("symbolSize");
  CQChartsValueSet *fontSizeSet   = getValueSet("fontSize");

  assert(symbolTypeSet && symbolSizeSet && fontSizeSet);

  //---

  int hasGroups = (numGroups() > 1);

  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    int               groupInd   = groupNameValue.first;
    const NameValues &nameValues = groupNameValue.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    Points &points = groupPoints_[groupInd];

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

      const QString &name   = nameValue.first;
      const Values  &values = nameValue.second.values;

      int nv = values.size();

      for (int iv = 0; iv < nv; ++iv) {
        const ValueData &valuePoint = values[iv];

        const QPointF &p = valuePoint.p;

        CQChartsSymbol symbolType = this->symbolType();

        if (symbolTypeColumn().isValid() && symbolTypeSet->hasInd(valuePoint.i)) {
          int ival = symbolTypeSet->imap(valuePoint.i);

          if (ival >= CQChartsSymbol::minValue() && ival <= CQChartsSymbol::maxValue())
            symbolType = CQChartsSymbol((CQChartsSymbol::Type) ival);
        }

        CQChartsLength symbolSize = this->symbolSize();

        if (symbolSizeColumn().isValid() && symbolSizeSet->hasInd(valuePoint.i))
          symbolSize = CQChartsLength(symbolSizeSet->imap(valuePoint.i),
                                      CQChartsLength::Units::PIXEL);

        OptReal fontSize = boost::make_optional(false, 0.0);

        if (fontSizeColumn().isValid() && fontSizeSet->hasInd(valuePoint.i))
          fontSize = fontSizeSet->imap(valuePoint.i);

        CQChartsColor color;

        if (colorColumn().isValid())
          (void) colorSetColor("color", valuePoint.i, color);

        double sx, sy;

        plotSymbolSize(symbolSize, sx, sy);

        CQChartsGeom::BBox bbox(p.x() - sx, p.y() - sy, p.x() + sx, p.y() + sy);

        CQChartsScatterPointObj *pointObj =
          new CQChartsScatterPointObj(this, groupInd, bbox, p, symbolType, symbolSize,
                                      fontSize, color, ig, ng, is, ns, iv, nv);

        //---

        pointObj->setName(name);

        pointObj->setInd(valuePoint.ind);

        addPlotObject(pointObj);

        //---

        points.push_back(p);
      }

      ++is;
    }

    ++ig;
  }

  //---

  ig = 0;
  ng = groupNameGridData_.size();

  for (const auto &pg : groupNameGridData_) {
    int                 groupInd     = pg.first;
    const NameGridData &nameGridData = pg.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    int is = 0;
    int ns = nameGridData.size();

    for (const auto &pn : nameGridData) {
      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

    //const QString       &name          = pn.first;
      const CellPointData &cellPointData = pn.second;

    //int nx = cellPointData.size();

      int maxN = cellPointData.maxN;

      for (const auto &px : cellPointData.xyPoints) {
        int            ix      = px.first;
        const YPoints &yPoints = px.second;

        double xmin, xmax;

        gridData_.xinterval.intervalValues(ix, xmin, xmax);

      //int ny = yPoints.size();

        for (const auto &py : yPoints) {
          int           iy     = py.first;
          const Points &points = py.second;

          double ymin, ymax;

          gridData_.yinterval.intervalValues(iy, ymin, ymax);

          CQChartsGeom::BBox bbox(xmin, ymin, xmax, ymax);

          CQChartsScatterCellObj *cellObj =
            new CQChartsScatterCellObj(this, groupInd, bbox, ig, ng, is, ns, ix, iy,
                                       points, maxN);

          addPlotObject(cellObj);
        }
      }

      ++is;
    }

    ++ig;
  }

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsScatterPlot::
addNameValues()
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex ind(data.row, plot_->xColumn(), data.parent);

      // get group
      int groupInd = plot_->rowGroupInd(ind);

      //---

      // get x, y value
      QModelIndex xInd  = plot_->modelIndex(data.row, plot_->xColumn(), data.parent);
      QModelIndex xInd1 = plot_->normalizeIndex(xInd);

      bool ok1, ok2;

      double x = plot_->modelReal(data.row, plot_->xColumn(), data.parent, ok1);
      double y = plot_->modelReal(data.row, plot_->yColumn(), data.parent, ok2);

      if (! ok1) x = uniqueId(data.row, plot_->xColumn(), data.parent);
      if (! ok2) y = uniqueId(data.row, plot_->yColumn(), data.parent);

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      // get optional name
      QString name;

      if (plot_->nameColumn().isValid()) {
        bool ok;

        name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok);
      }

      if (! name.length())
        name = plot_->title()->textStr();

      if (! name.length())
        name = plot_->xAxis()->label();

      //---

      // get symbol type, size, font size and color
      CQChartsColor color;

      // get color label (needed if not string ?)
      if (plot_->colorColumn().isValid()) {
        bool ok;

        color = plot_->modelColor(data.row, plot_->colorColumn(), data.parent, ok);
      }

      //---

      plot_->addNameValue(groupInd, name, x, y, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(int row, const CQChartsColumn &column, const QModelIndex &parent) {
      bool ok;

      QVariant var = plot_->modelValue(row, column, parent, ok);
      if (! var.isValid()) return -1;

      return columnDetails(column)->uniqueId(var);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        CQChartsModelData *modelData = plot_->charts()->getModelData(plot_->model().data());

        details_ = modelData->details();
      }

      return details_->columnDetails(column);
    }

   private:
    CQChartsScatterPlot*  plot_    { nullptr };
    CQChartsModelDetails* details_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

void
CQChartsScatterPlot::
addNameValue(int groupInd, const QString &name, double x, double y, int row,
             const QModelIndex &xind, const CQChartsColor &color)
{
  if (isGridded()) {
    int ix = gridData_.xinterval.valueInterval(x);
    int iy = gridData_.yinterval.valueInterval(y);

    CellPointData &cellPointData = groupNameGridData_[groupInd][name];

    Points &points = cellPointData.xyPoints[ix][iy];

    points.push_back(QPointF(x, y));

    cellPointData.maxN = std::max(cellPointData.maxN, int(points.size()));

    gridData_.maxN = std::max(gridData_.maxN, cellPointData.maxN);
  }
  else {
    ValuesData &valuesData = groupNameValues_[groupInd][name];

    valuesData.xrange.add(x);
    valuesData.yrange.add(y);

    valuesData.values.emplace_back(QPointF(x, y), row, xind, color);
  }
}

void
CQChartsScatterPlot::
addKeyItems(CQChartsPlotKey *key)
{
  if (! isGridded())
    addPointKeyItems(key);
  else
    addGridKeyItems(key);

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsScatterPlot::
addPointKeyItems(CQChartsPlotKey *key)
{
  int ng = groupNameValues_.size();

  // multiple group - key item per group
  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupNameValue : groupNameValues_) {
      int groupInd = groupNameValue.first;

      QString groupName = groupIndName(groupInd);

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, groupInd, ig, ng);
      CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, groupName);

      key->addItem(colorItem, ig, 0);
      key->addItem(textItem , ig, 1);

      //--

      if (colorColumn().isValid() && colorColumn().isGroup()) {
        CQChartsColorSet *colorSet = getColorSet("color");
        assert(colorSet);

        if (colorSet->isMapped()) {
          double r = CMathUtil::map(groupInd, colorSet->imin(), colorSet->imax(),
                                    colorSet->mapMin(), colorSet->mapMax());

          CQChartsColor color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r);

          if (color.isValid())
            colorItem->setColor(color);
        }
      }

      //--

      ++ig;
    }
  }
  // single group - key item per value set
  else if (ng > 0) {
    const NameValues &nameValues = (*groupNameValues_.begin()).second;

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      const QString &name   = nameValue.first;
      const Values  &values = nameValue.second.values;

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, -1, is, ns);
      CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, name);

      key->addItem(colorItem, is, 0);
      key->addItem(textItem , is, 1);

      //--

      if (colorColumn().isValid()) {
        int nv = values.size();

        if (nv > 0) {
          const ValueData &valuePoint = values[0];

          CQChartsColor color;

          (void) colorSetColor("color", valuePoint.i, color);

          if (color.isValid())
            colorItem->setColor(color);
        }
      }

      //--

      ++is;
    }
  }
}

void
CQChartsScatterPlot::
addGridKeyItems(CQChartsPlotKey *key)
{
  CQChartsScatterGridKeyItem *item = new CQChartsScatterGridKeyItem(this);

  key->addItem(item, 0, 0);
}

//------

bool
CQChartsScatterPlot::
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

  menu->addSeparator();

  (void) addCheckedAction("Best Fit"   , isBestFit   (), SLOT(setBestFit   (bool)));
  (void) addCheckedAction("Hull"       , isHull      (), SLOT(setHull      (bool)));
  (void) addCheckedAction("X Rug"      , isXRug      (), SLOT(setXRug      (bool)));
  (void) addCheckedAction("Y Rug"      , isYRug      (), SLOT(setYRug      (bool)));
  (void) addCheckedAction("X Density"  , isXDensity  (), SLOT(setXDensity  (bool)));
  (void) addCheckedAction("Y Density"  , isYDensity  (), SLOT(setYDensity  (bool)));
  (void) addCheckedAction("Density Map", isDensityMap(), SLOT(setDensityMap(bool)));
  (void) addCheckedAction("X Whisker"  , isXWhisker  (), SLOT(setXWhisker  (bool)));
  (void) addCheckedAction("Y Whisker"  , isYWhisker  (), SLOT(setYWhisker  (bool)));
  (void) addCheckedAction("Gridded"    , isGridded   (), SLOT(setGridded   (bool)));

  return true;
}

//------

CQChartsGeom::BBox
CQChartsScatterPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  if (isXRug() || isYRug() || isXDensity() || isYDensity() || isXWhisker() || isYWhisker()) {
    const CQChartsGeom::Range &dataRange = this->dataRange();

    //---

    // rug axis
    double sx, sy;

    plotSymbolSize(rugSymbolSize(), sx, sy);

    if (isXRug()) {
      double y = (xRugSide() == YSide::BOTTOM ? dataRange.ymin() - 2*sy :
                                                dataRange.ymax() + 2*sy);

      QPointF p(dataRange.xmax(), y);

      bbox += CQChartsUtil::fromQPoint(p);
    }

    if (isYRug()) {
      double x = (yRugSide() == XSide::LEFT ? dataRange.xmin() - 2*sx :
                                              dataRange.xmax() + 2*sx);

      QPointF p(x, dataRange.ymax());

      bbox += CQChartsUtil::fromQPoint(p);
    }

    //---

    // density axis
    if (isXDensity()) {
      double dw = lengthPlotHeight(densityWidth());

      double pos = (xDensitySide() == YSide::BOTTOM ?
        dataRange.ymin() - dw : dataRange.ymax() + dw);

      QPointF p1(dataRange.xmax(), pos);

      bbox += CQChartsUtil::fromQPoint(p1);
    }

    if (isYDensity()) {
      double dw = lengthPlotWidth(densityWidth());

      double pos = (yDensitySide() == XSide::LEFT ?
        dataRange.xmin() - dw : dataRange.xmax() + dw);

      QPointF p2(pos, dataRange.ymin());

      bbox += CQChartsUtil::fromQPoint(p2);
    }

    //---

    // whisker axis
    if (isXWhisker()) {
      int ng = groupNameValues_.size();

      double wm = lengthPlotHeight(whiskerMargin());
      double ww = ng*lengthPlotHeight(whiskerWidth());

      double pos = (xWhiskerSide() == YSide::BOTTOM ?
        dataRange.ymin() - ww - 2*wm : dataRange.ymax() + ww + 2*wm);

      QPointF p1(dataRange.xmax(), pos);

      bbox += CQChartsUtil::fromQPoint(p1);
    }

    if (isYWhisker()) {
      int ng = groupNameValues_.size();

      double wm = lengthPlotWidth(whiskerMargin());
      double ww = ng*lengthPlotWidth(whiskerWidth());

      double pos = (yWhiskerSide() == XSide::LEFT ?
        dataRange.xmin() - ww - 2*wm : dataRange.xmax() + ww + 2*wm);

      QPointF p2(pos, dataRange.ymin());

      bbox += CQChartsUtil::fromQPoint(p2);
    }
  }

  return bbox;
}

//------

bool
CQChartsScatterPlot::
hasBackground() const
{
  if (isHull()) return true;

  if (isBestFit()) return true;

  if (isDensityMap()) return true;

  if (isXRug    ()) return true;
  if (isXDensity()) return true;
  if (isXWhisker()) return true;

  if (isYRug    ()) return true;
  if (isYDensity()) return true;
  if (isYWhisker()) return true;

  return false;
}

void
CQChartsScatterPlot::
drawBackground(QPainter *painter)
{
  CQChartsPlot::drawBackground(painter);

  if (isHull())
    drawHull(painter);

  if (isBestFit())
    drawBestFit(painter);

  if (isDensityMap()) drawDensityMap(painter);

  if (isXRug    ()) drawXRug    (painter);
  if (isXDensity()) drawXDensity(painter);
  if (isXWhisker()) drawXWhisker(painter);

  if (isYRug    ()) drawYRug    (painter);
  if (isYDensity()) drawYDensity(painter);
  if (isYWhisker()) drawYWhisker(painter);
}

bool
CQChartsScatterPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsScatterPlot::
drawForeground(QPainter *painter)
{
  drawSymbolMapKey(painter);
}

void
CQChartsScatterPlot::
drawBestFit(QPainter *painter)
{
  for (const auto &groupNameValue : groupNameValues_) {
    int groupInd = groupNameValue.first;

    CQChartsFitData &fitData = groupFitData_[groupInd];

    if (! fitData.isFitted()) {
      Points &points = groupPoints_[groupInd];

      fitData.calc(points, bestFitOrder());
    }
  }

  //---

  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    int groupInd = groupNameValue.first;

    CQChartsFitData &fitData = groupFitData_[groupInd];

    //---

    double pxl, pyl, pxr, pyr;

    windowToPixel(fitData.xmin(), 0, pxl, pyl);
    windowToPixel(fitData.xmax(), 0, pxr, pyr);

    //---

    // set pen and brush
    QPen   pen;
    QBrush brush;

    QColor borderColor = interpThemeColor(1.0);
    QColor fillColor   = bestFitFillColor().interpColor(this, ig, ng);

    setPen(pen, true, borderColor, 1.0, CQChartsLength("0px"), CQChartsLineDash());

    setBrush(brush, true, fillColor, bestFitFillAlpha(), CQChartsFillPattern());

    //---

    // calc fit shape
    QPolygonF bpoly, poly, tpoly;

    for (int px = pxl; px <= pxr; ++px) {
      double x, y;

      pixelToWindow(px, 0.0, x, y);

      double y2 = fitData.interp(x);

      double px2, py2;

      windowToPixel(x, y2, px2, py2);

      poly << QPointF(px2, py2);

      if (isBestFitDeviation()) {
        windowToPixel(x, y2 - fitData.deviation(), px2, py2);

        bpoly << QPointF(px2, py2);

        windowToPixel(x, y2 + fitData.deviation(), px2, py2);

        tpoly << QPointF(px2, py2);
      }
    }

    //---

    // draw fit shape
    if (isBestFitDeviation()) {
      QPolygonF dpoly;

      for (int i = 0; i < bpoly.size(); ++i) {
        const QPointF &p = bpoly[i];

        dpoly << p;
      }

      for (int i = tpoly.size() - 1; i >= 0; --i) {
        const QPointF &p = tpoly[i];

        dpoly << p;
      }

      painter->setBrush(brush);

      painter->drawPolygon(dpoly);
    }

    //---

    // draw fit line
    QPainterPath path;

    const QPointF &p = poly[0];

    path.moveTo(p);

    for (int i = 1; i < poly.size(); ++i) {
      const QPointF &p = poly[i];

      path.lineTo(p);
    }

    painter->strokePath(path, pen);

    //---

    ++ig;
  }
}

void
CQChartsScatterPlot::
drawHull(QPainter *painter)
{
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    int groupInd = groupNameValue.first;

    CQChartsGrahamHull &hull = groupHull_[groupInd];

    if (hull.numPoints() == 0) {
      Points &points = groupPoints_[groupInd];

      //---

      std::vector<double> x, y;

      for (const auto &p : points)
        hull.addPoint(p);

      hull.calc();
    }

    //---

    QPen   pen;
    QBrush brush;

    setPenBrush(pen, brush,
      isHullBorder(), interpHullBorderColor(ig, ng), hullBorderAlpha(),
      hullBorderWidth(), hullBorderDash(),
      isHullFilled(), interpHullFillColor(ig, ng), hullFillAlpha(), hullFillPattern());

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    hull.draw(this, painter);

    //---

    ++ig;
  }
}

//------

void
CQChartsScatterPlot::
drawXRug(QPainter *painter)
{
  for (const auto &plotObj : plotObjects()) {
    const CQChartsScatterPointObj *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);
    const CQChartsScatterCellObj  *cellObj  = dynamic_cast<CQChartsScatterCellObj  *>(plotObj);

    if (pointObj)
      pointObj->drawDir(painter, CQChartsScatterPointObj::Dir::X, xRugSide() == YSide::TOP);

    if (cellObj)
      cellObj->drawRugSymbol(painter, CQChartsScatterCellObj::Dir::X, xRugSide() == YSide::TOP);
  }
}

void
CQChartsScatterPlot::
drawYRug(QPainter *painter)
{
  for (const auto &plotObj : plotObjects()) {
    const CQChartsScatterPointObj *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);
    const CQChartsScatterCellObj  *cellObj  = dynamic_cast<CQChartsScatterCellObj  *>(plotObj);

    if (pointObj)
      pointObj->drawDir(painter, CQChartsScatterPointObj::Dir::Y, yRugSide() == XSide::RIGHT);

    if (cellObj)
      cellObj->drawRugSymbol(painter, CQChartsScatterCellObj::Dir::Y, yRugSide() == XSide::RIGHT);
  }
}

//------

void
CQChartsScatterPlot::
drawXDensity(QPainter *painter)
{
  initWhiskerData();

  //---

  if (! isGridded()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      int groupInd = groupNameValue.first;

      WhiskerData &whiskerData = groupWhiskers_[groupInd];

      drawXDensityWhisker(painter, whiskerData, ig, ng);

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      int groupInd = pg.first;

      WhiskerData &whiskerData = groupWhiskers_[groupInd];

      drawXDensityWhisker(painter, whiskerData, ig, ng);

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawYDensity(QPainter *painter)
{
  initWhiskerData();

  //---

  if (! isGridded()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      int groupInd = groupNameValue.first;

      WhiskerData &whiskerData = groupWhiskers_[groupInd];

      drawYDensityWhisker(painter, whiskerData, ig, ng);

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      int groupInd = pg.first;

      WhiskerData &whiskerData = groupWhiskers_[groupInd];

      drawYDensityWhisker(painter, whiskerData, ig, ng);

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawXDensityWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng)
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig, ng);
  QColor fillColor   = interpSymbolFillColor  (ig, ng);

  setPenBrush(pen, brush,
              /*stroked*/ true, strokeColor, symbolStrokeAlpha(),
              CQChartsLength("1px"), CQChartsLineDash(),
              /*filled*/ true, fillColor, densityAlpha(),
              CQChartsFillPattern());

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  const CQChartsDensity &density = whiskerData.xWhisker.density();

  double xmin = density.xmin1();
  double xmax = density.xmax1();

  double dh = lengthPlotHeight(densityWidth());

  double pos = (xDensitySide() == YSide::BOTTOM ?  dataRange.ymin() - dh : dataRange.ymax());

  CQChartsGeom::BBox rect(xmin, pos, xmax, pos + dh);

  density.drawWhisker(this, painter, rect, Qt::Horizontal);
}

void
CQChartsScatterPlot::
drawYDensityWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng)
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig, ng);
  QColor fillColor   = interpSymbolFillColor  (ig, ng);

  setPenBrush(pen, brush,
              /*stroked*/ true, strokeColor, symbolStrokeAlpha(),
              CQChartsLength("1px"), CQChartsLineDash(),
              /*filled*/ true, fillColor, densityAlpha(),
              CQChartsFillPattern());

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  const CQChartsDensity &density = whiskerData.yWhisker.density();

  double xmin = density.xmin1();
  double xmax = density.xmax1();

  double dw = lengthPlotWidth(densityWidth());

  double pos = (yDensitySide() == XSide::LEFT ?  dataRange.xmin() - dw : dataRange.xmax());

  CQChartsGeom::BBox rect(pos, xmin, pos + dw, xmax);

  density.drawWhisker(this, painter, rect, Qt::Vertical);
}

void
CQChartsScatterPlot::
drawDensityMap(QPainter *painter)
{
  initWhiskerData();

  //---

  int gridSize = std::max(densityMapGridSize(), 1);

  double delta = densityMapDelta();

  for (const auto &groupNameValue : groupNameValues_) {
    const NameValues &nameValues = groupNameValue.second;

    for (const auto &nameValue : nameValues) {
      const ValuesData &values = nameValue.second;

      double xmin = values.xrange.min();
      double xmax = values.xrange.max();
      double ymin = values.yrange.min();
      double ymax = values.yrange.max();

      QPointF ll = windowToPixel(QPointF(xmin, ymin));
      QPointF ur = windowToPixel(QPointF(xmax, ymax));

      int x1 = CMathRound::RoundDown(ll.x());
      int x2 = CMathRound::RoundUp  (ur.x());
      int y2 = CMathRound::RoundUp  (ll.y());
      int y1 = CMathRound::RoundDown(ur.y());

      int dx = gridSize;
      int dy = gridSize;

      std::vector<double> xv;
      std::vector<double> yv;

      for (const auto &v : values.values) {
        xv.push_back(v.p.x());
        yv.push_back(v.p.y());
      }

      CMathBivariate bivariate(xv, yv);

      for (int y = y1; y <= y2; y += dy) {
        for (int x = x1; x <= x2; x += dx) {
          QPointF p = pixelToWindow(QPointF(x, y));

          double a = 1.0;
          double v = bivariate.calc(p.x(), p.y());

          if (delta > 0.0) {
            double v1 = CMathRound::RoundDown(v/delta)*delta;

            a = CMathUtil::clamp(sqrt(1.0 - (v - v1)), 0.0, 1.0);
          }

          //---

          QBrush brush;

          QColor c = interpPaletteColor(v);

          setBrush(brush, true, c, a, CQChartsFillPattern());

          //---

          painter->fillRect(QRect(x, y, dx, dy), brush);
        }
      }
    }
  }
}

//---

void
CQChartsScatterPlot::
drawXWhisker(QPainter *painter)
{
  initWhiskerData();

  //---

  if (! isGridded()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      int groupInd = groupNameValue.first;

      WhiskerData &whiskerData = groupWhiskers_[groupInd];

      drawXWhiskerWhisker(painter, whiskerData, ig, ng);

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      int groupInd = pg.first;

      WhiskerData &whiskerData = groupWhiskers_[groupInd];

      drawXWhiskerWhisker(painter, whiskerData, ig, ng);

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawYWhisker(QPainter *painter)
{
  initWhiskerData();

  //---

  if (! isGridded()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      int groupInd = groupNameValue.first;

      WhiskerData &whiskerData = groupWhiskers_[groupInd];

      drawYWhiskerWhisker(painter, whiskerData, ig, ng);

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      int groupInd = pg.first;

      WhiskerData &whiskerData = groupWhiskers_[groupInd];

      drawYWhiskerWhisker(painter, whiskerData, ig, ng);

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawXWhiskerWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng)
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig, ng);
  QColor fillColor   = interpSymbolFillColor  (ig, ng);

  setPenBrush(pen, brush,
              /*stroked*/ true, strokeColor, symbolStrokeAlpha(),
              CQChartsLength("1px"), CQChartsLineDash(),
              /*filled*/ true, fillColor, whiskerAlpha(),
              CQChartsFillPattern());

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  double ww = lengthPlotHeight(whiskerWidth());
  double wm = lengthPlotHeight(whiskerMargin());

  const CQChartsGeom::Range &dataRange = this->dataRange();

  double pos = (xWhiskerSide() == YSide::BOTTOM ?
    dataRange.ymin() - ig*ww - wm : dataRange.ymax() + (ig + 1)*ww + wm);

  CQChartsGeom::BBox rect(whiskerData.xWhisker.min(), pos, whiskerData.xWhisker.max(), pos + ww);

  CQChartsBoxWhiskerUtil::drawWhisker(this, painter, whiskerData.xWhisker,
                                      rect, whiskerWidth(), Qt::Horizontal);
}

void
CQChartsScatterPlot::
drawYWhiskerWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng)
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig, ng);
  QColor fillColor   = interpSymbolFillColor  (ig, ng);

  setPenBrush(pen, brush,
              /*stroked*/ true, strokeColor, symbolStrokeAlpha(),
              CQChartsLength("1px"), CQChartsLineDash(),
              /*filled*/ true, fillColor, whiskerAlpha(),
              CQChartsFillPattern());

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  double ww = lengthPlotWidth(whiskerWidth());
  double wm = lengthPlotWidth(whiskerMargin());

  const CQChartsGeom::Range &dataRange = this->dataRange();

  double pos = (yWhiskerSide() == XSide::LEFT ?
    dataRange.xmin() - ig*ww - wm : dataRange.xmax() + (ig + 1)*ww + wm);

  CQChartsGeom::BBox rect(pos, whiskerData.yWhisker.min(), pos + ww, whiskerData.yWhisker.max());

  CQChartsBoxWhiskerUtil::drawWhisker(this, painter, whiskerData.yWhisker,
                                      rect, whiskerWidth(), Qt::Vertical);
}

void
CQChartsScatterPlot::
initWhiskerData()
{
  for (const auto &groupNameValue : groupNameValues_) {
    int groupInd = groupNameValue.first;

    WhiskerData &whiskerData = groupWhiskers_[groupInd];

    // init whisker if needed
    if (! whiskerData.xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd)
          whiskerData.xWhisker.addValue(pointObj->point().x());
      }
    }

    if (! whiskerData.yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd)
          whiskerData.yWhisker.addValue(pointObj->point().y());
      }
    }
  }

  //---

  for (const auto &pg : groupNameGridData_) {
    int groupInd = pg.first;

    WhiskerData &whiskerData = groupWhiskers_[groupInd];

    // init whisker if needed
    if (! whiskerData.xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        const CQChartsScatterCellObj *cellObj =
          dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points())
            whiskerData.xWhisker.addValue(p.x());
        }
      }
    }

    if (! whiskerData.yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        const CQChartsScatterCellObj *cellObj =
          dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points())
            whiskerData.yWhisker.addValue(p.y());
        }
      }
    }
  }
}

//------

void
CQChartsScatterPlot::
drawSymbolMapKey(QPainter *painter)
{
  // draw size key
  if (symbolSizeColumn().isValid() && isSymbolMapKey()) {
    CQChartsValueSet *symbolSizeSet = getValueSet("symbolSize");

    OptReal min  = boost::make_optional(false, 0.0);
    OptReal mean = boost::make_optional(false, 0.0);
    OptReal max  = boost::make_optional(false, 0.0);

    if (symbolSizeSet->canMap()) {
      min  = symbolSizeSet->rmin();
      mean = symbolSizeSet->rmean();
      max  = symbolSizeSet->rmax();
    }

    QString  typeName;
    QVariant minValue, maxValue;

    columnDetails(symbolSizeColumn(), typeName, minValue, maxValue);

    bool ok;

    if (minValue.isValid())
      min = CQChartsVariant::toReal(minValue, ok);

    if (maxValue.isValid())
      max = CQChartsVariant::toReal(maxValue, ok);

    CQChartsGeom::BBox pbbox = calcPlotPixelRect();

    //double px, py;

    //double vx = view()->viewportRange();
    //double vy = 0.0;

    //view()->windowToPixel(vx, vy, px, py);

    double px = pbbox.getXMax();
    double py = pbbox.getYMax();

    double pm = symbolMapKeyMargin();

    double pr1 = symbolSizeSet->mapMax();
    double pr3 = symbolSizeSet->mapMin();
    double pr2 = (pr1 + pr3)/2;

    QColor borderColor = interpThemeColor(1.0);

    painter->setPen(borderColor);

    double xm = px - pr1 - pm;
    double ym = py - pm;

    QRectF r1(xm - pr1, ym - 2*pr1, 2*pr1, 2*pr1);
    QRectF r2(xm - pr2, ym - 2*pr2, 2*pr2, 2*pr2);
    QRectF r3(xm - pr3, ym - 2*pr3, 2*pr3, 2*pr3);

    double a = symbolMapKeyAlpha();

    QColor fillColor1 = interpSymbolFillColor(1.0); fillColor1.setAlphaF(a);
    QColor fillColor2 = interpSymbolFillColor(0.5); fillColor2.setAlphaF(a);
    QColor fillColor3 = interpSymbolFillColor(0.0); fillColor3.setAlphaF(a);

    painter->setBrush(fillColor1); painter->drawEllipse(r1);
    painter->setBrush(fillColor2); painter->drawEllipse(r2);
    painter->setBrush(fillColor3); painter->drawEllipse(r3);

    auto drawText = [&](QPainter *painter, const QPointF &p, const QString &text) {
      QFontMetricsF fm(painter->font());

      painter->drawText(p.x() - fm.width(text)/2, p.y(), text);
    };

    if (max)
      drawText(painter, QPointF(r1.center().x(), r1.top()), QString("%1").arg(*max));

    if (mean)
      drawText(painter, QPointF(r2.center().x(), r2.top()), QString("%1").arg(*mean));

    if (min)
      drawText(painter, QPointF(r3.center().x(), r3.top()), QString("%1").arg(*min));
  }
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(CQChartsScatterPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                        const QPointF &p, const CQChartsSymbol &symbolType,
                        const CQChartsLength &symbolSize, const OptReal &fontSize,
                        const CQChartsColor &color,
                        int ig, int ng, int is, int ns, int iv, int nv) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), p_(p), symbolType_(symbolType),
 symbolSize_(symbolSize), fontSize_(fontSize), color_(color),
 ig_(ig), ng_(ng), is_(is), ns_(ns), iv_(iv), nv_(nv)
{
  assert(ig >= 0 && ig < ng);
  assert(is >= 0 && is < ns);
  assert(iv >= 0 && iv < nv);
}

QString
CQChartsScatterPointObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(ind_);

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("point:%1:%2:%3").arg(ig_).arg(is_).arg(iv_);
}

QString
CQChartsScatterPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addBoldLine(name_);

  if (ng_ > 1) {
    QString groupName = plot_->groupIndName(groupInd_);

    tableTip.addTableRow("Group", groupName);
  }

  tableTip.addTableRow(plot_->xname(), p_.x());
  tableTip.addTableRow(plot_->yname(), p_.y());

  auto pg = plot_->groupNameValues().find(groupInd_);
  assert(pg != plot_->groupNameValues().end());

  auto p = (*pg).second.find(name_);
  assert(p != (*pg).second.end());

  const CQChartsScatterPlot::Values &values = (*p).second.values;

  const CQChartsScatterPlot::ValueData &valuePoint = values[iv_];

  if (plot_->symbolTypeColumn().isValid()) {
    bool ok;

    QString symbolTypeStr =
      plot_->modelString(ind_.row(), plot_->symbolTypeColumn(), ind_.parent(), ok);

    if (ok)
      tableTip.addTableRow(plot_->symbolTypeName(), symbolTypeStr);
  }

  if (plot_->symbolSizeColumn().isValid()) {
    bool ok;

    QString symbolSizeStr =
      plot_->modelString(ind_.row(), plot_->symbolSizeColumn(), ind_.parent(), ok);

    if (ok)
      tableTip.addTableRow(plot_->symbolSizeName(), symbolSizeStr);
  }

  if (plot_->fontSizeColumn().isValid()) {
    bool ok;

    QString fontSizeStr =
      plot_->modelString(ind_.row(), plot_->fontSizeColumn(), ind_.parent(), ok);

    if (ok)
      tableTip.addTableRow(plot_->fontSizeName(), fontSizeStr);
  }

  if (valuePoint.color.isValid())
    tableTip.addTableRow(plot_->colorName(), valuePoint.color.colorStr());

  return tableTip.str();
}

bool
CQChartsScatterPointObj::
inside(const CQChartsGeom::Point &p) const
{
  const CQChartsLength &s = this->symbolSize(); // TODO: ensure not a crazy number

  double sx = plot_->lengthPixelWidth (s);
  double sy = plot_->lengthPixelHeight(s);

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  CQChartsGeom::BBox pbbox(px - sx, py - sy, px + sx, py + sy);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());
}

void
CQChartsScatterPointObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsScatterPointObj::
draw(QPainter *painter)
{
  drawDir(painter, Dir::XY);
}

void
CQChartsScatterPointObj::
drawDir(QPainter *painter, const Dir &dir, bool flip) const
{
  int ic = 0;
  int nc = 0;

  if (ng_ > 0) {
    ic = ig_;
    nc = ng_;
  }
  else {
    ic = is_;
    nc = ns_;
  }

  //---

  // calc stroke and brush
  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, ic, nc);

  if (color_.isValid())
    brush.setColor(color_.interpColor(plot_, ic, nc));

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // get symbol, data
  CQChartsSymbol symbol;
  double         sx, sy;

  if (dir != Dir::XY) {
    symbol = plot_->rugSymbolType();

    if (symbol == CQChartsSymbol::Type::NONE)
      symbol = (dir == Dir::X ? CQChartsSymbol::Type::VLINE : CQChartsSymbol::Type::HLINE);

    plot_->pixelSymbolSize(plot_->rugSymbolSize(), sx, sy);
  }
  else {
    symbol = this->symbolType();

    if (symbol == CQChartsSymbol::Type::NONE)
      symbol = plot_->symbolType();

    plot_->pixelSymbolSize(symbolSize(), sx, sy);
  }

  //---

  // get point
  QPointF ps = plot_->windowToPixel(p_);

  if (dir != Dir::XY) {
    // Dir::X and Dir::Y are X/Y Rug Symbols
    CQChartsGeom::BBox pbbox = plot_->calcDataPixelRect();

    if      (dir == Dir::X) {
      if (! flip)
        ps.setY(pbbox.getYMax() + sy);
      else
        ps.setY(pbbox.getYMin() - sy);
    }
    else if (dir == Dir::Y) {
      if (! flip)
        ps.setX(pbbox.getXMin() - sx);
      else
        ps.setX(pbbox.getXMax() + sx);
    }
  }

  //---

  // draw symbol
  QRectF erect(ps.x() - sx, ps.y() - sy, 2*sx, 2*sy);

  plot_->drawSymbol(painter, ps, symbol, CMathUtil::avg(sx, sy), pen, brush);

  //---

  // draw text labels
  if (plot_->dataLabel().isVisible()) {
    CQChartsDataLabel &dataLabel = plot_->dataLabel();

    //---

    QPen tpen;

    QColor tc = plot_->dataLabel().interpTextColor(ic, nc);

    plot_->setPen(tpen, true, tc, plot_->dataLabel().textAlpha(),
                  CQChartsLength("0px"), CQChartsLineDash());

    //---

    double fontSize = plot_->dataLabel().textFont().pointSizeF();

    if (fontSize_)
      fontSize = *fontSize_;

    //---

    QFont font = dataLabel.textFont();

    if (fontSize > 0) {
      // scale to font size
      fontSize = plot_->limitFontSize(fontSize);

      QFont font1 = font;

      font1.setPointSizeF(fontSize);

      dataLabel.setTextFont(font1);
    }

    dataLabel.draw(painter, erect, name_, dataLabel.position(), tpen);

    if (fontSize > 0)
      dataLabel.setTextFont(font);
  }
}

//------

CQChartsScatterCellObj::
CQChartsScatterCellObj(CQChartsScatterPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                       int ig, int ng, int is, int ns, int ix, int iy,
                       const Points &points, int maxn) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd),
 ig_(ig), ng_(ng), is_(is), ns_(ns), ix_(ix), iy_(iy), points_(points), maxn_(maxn)
{
  assert(ig >= 0 && ig < ng);
  assert(is >= 0 && is < ns);
}

QString
CQChartsScatterCellObj::
calcId() const
{
  return QString("point:%1:%2:%3:%4").arg(ig_).arg(is_).arg(ix_).arg(iy_);
}

QString
CQChartsScatterCellObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  double xmin, xmax, ymin, ymax;

  plot_->gridData().xinterval.intervalValues(ix_, xmin, xmax);
  plot_->gridData().yinterval.intervalValues(iy_, ymin, ymax);

  tableTip.addTableRow("X Range", QString("%1 %2").arg(xmin).arg(xmax));
  tableTip.addTableRow("Y Range", QString("%1 %2").arg(ymin).arg(ymax));
  tableTip.addTableRow("Count"  , points_.size());

  return tableTip.str();
}

bool
CQChartsScatterCellObj::
inside(const CQChartsGeom::Point &p) const
{
  return CQChartsPlotObj::inside(p);
}

void
CQChartsScatterCellObj::
getSelectIndices(Indices &) const
{
}

void
CQChartsScatterCellObj::
addColumnSelectIndex(Indices &, const CQChartsColumn &) const
{
}

void
CQChartsScatterCellObj::
draw(QPainter *painter)
{
  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  QPen   pen;
  QBrush brush;

  QColor sc = plot_->interpThemeColor(0.0);
  QColor fc = plot_->interpPaletteColor(points_.size(), maxn_);

  plot_->setPenBrush(pen, brush,
                     /*stroked*/true, sc, 1.0, CQChartsLength("1px"), CQChartsLineDash(),
                     /*filled*/true, fc, 1.0, CQChartsFillPattern::Type::SOLID);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawRect(CQChartsUtil::toQRect(prect));
}

void
CQChartsScatterCellObj::
drawRugSymbol(QPainter *painter, const Dir &dir, bool flip) const
{
  int ic = 0;
  int nc = 0;

  if (ng_ > 0) {
    ic = ig_;
    nc = ng_;
  }
  else {
    ic = is_;
    nc = ns_;
  }

  //---

  // calc stroke and brush
  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, ic, nc);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  CQChartsSymbol symbol = plot_->rugSymbolType();

  if (symbol == CQChartsSymbol::Type::NONE)
    symbol = (dir == Dir::X ? CQChartsSymbol::Type::VLINE : CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(plot_->rugSymbolSize(), sx, sy);

  //---

  for (const auto &p : points_) {
    QPointF ps = plot_->windowToPixel(p);

    // Dir::X and Dir::Y are X/Y Rug Symbols
    CQChartsGeom::BBox pbbox = plot_->calcDataPixelRect();

    if      (dir == Dir::X) {
      if (! flip)
        ps.setY(pbbox.getYMax() + sy);
      else
        ps.setY(pbbox.getYMin() - sy);
    }
    else if (dir == Dir::Y) {
      if (! flip)
        ps.setX(pbbox.getXMin() - sx);
      else
        ps.setX(pbbox.getXMax() + sx);
    }

    // draw symbol

    QRectF erect(ps.x() - sx, ps.y() - sy, 2*sx, 2*sy);

    plot_->drawSymbol(painter, ps, symbol, CMathUtil::avg(sx, sy), pen, brush);
  }
}

//------

CQChartsScatterKeyColor::
CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int groupInd, int i, int n) :
 CQChartsKeyColorBox(plot, i, n), groupInd_(groupInd)
{
}

bool
CQChartsScatterKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  int ih = hideIndex();

  plot->setSetHidden(ih, ! plot->isSetHidden(ih));

  plot->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsScatterKeyColor::
fillBrush() const
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  QColor c;

  if (color_.isValid())
    c = color_.interpColor(plot_, 0, 1);
  else {
    c = plot->interpSymbolFillColor(i_, n_);

    //c = CQChartsKeyColorBox::fillBrush().color();
  }

  int ih = hideIndex();

  if (plot->isSetHidden(ih))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}

int
CQChartsScatterKeyColor::
hideIndex() const
{
  return (groupInd_ >= 0 ? groupInd_ : i_);
}

//---

CQChartsScatterGridKeyItem::
CQChartsScatterGridKeyItem(CQChartsScatterPlot *plot) :
 CQChartsKeyItem(plot->key()), plot_(plot)
{
}

QSizeF
CQChartsScatterGridKeyItem::
size() const
{
  QFont font = plot_->view()->plotFont(plot_, key_->textFont());

  QFontMetricsF fm(font);

  double fw = fm.width("X");
  double fh = fm.height();

  int n = plot_->gridData().maxN;

  double tw = fm.width(QString("%1").arg(n));

  double ww = plot_->pixelToWindowWidth (2*fw + tw + 6);
  double wh = plot_->pixelToWindowHeight(7*fh + fh + 4);

  return QSizeF(ww, wh);
}

void
CQChartsScatterGridKeyItem::
draw(QPainter *painter, const CQChartsGeom::BBox &rect)
{
  plot_->view()->setPlotPainterFont(plot_, painter, key_->textFont());

  QFontMetricsF fm(painter->font());

  double fh = fm.height();

  int n = plot_->gridData().maxN;

  double tw = plot_->pixelToWindowWidth(fm.width(QString("%1").arg(n)) - 3);

  CQChartsGeom::BBox lrect(rect.getXMin(), rect.getYMin(), rect.getXMax() - tw, rect.getYMax());
  CQChartsGeom::BBox rrect(rect.getXMax() - tw, rect.getYMin(), rect.getXMax(), rect.getYMax());

  CQChartsGeom::BBox lprect = plot_->windowToPixel(lrect);
  CQChartsGeom::BBox rprect = plot_->windowToPixel(rrect);

  //---

  QPointF pg1 = QPointF(lprect.getXMin() + 2, lprect.getYMax() - 2 - fh/2);
  QPointF pg2 = QPointF(lprect.getXMin() + 2, lprect.getYMin() + 2 + fh/2);

  QLinearGradient lg(pg1.x(), pg1.y(), pg2.x(), pg2.y());

  plot_->view()->themePalette()->setLinearGradient(lg, 1.0);

  QBrush brush(lg);

  //---

  QRectF frect(pg1.x(), pg2.y(), lprect.getWidth() - 4, lprect.getHeight() - 4 - fh);

  painter->fillRect(frect, brush);

  //---

  int n1 = 0;
  int n5 = n;

  double dn = (n5 - n1)/4.0;

  int n2 = n1 + dn;
  int n4 = n5 - dn;
  int n3 = (n5 + n1)/2.0;

  double y1 = rprect.getYMax() - 2 - fh/2;
  double y5 = rprect.getYMin() + 2 + fh/2;
  double dy = (y1 - y5)/4.0;

  double y2 = y1 - dy;
  double y4 = y5 + dy;
  double y3 = (y5 + y1)/2.0;

  //---

  QPen pen;

  QColor tc = plot_->interpThemeColor(1.0);

  plot_->setPen(pen, true, tc, 1.0, CQChartsLength("0px"), CQChartsLineDash());

  painter->setPen(pen);

  //---

  double df = (fm.ascent() - fm.descent())/2.0;

  painter->drawText(rprect.getXMin() + 1, y1 + df, QString("%1").arg(n1));
  painter->drawText(rprect.getXMin() + 1, y2 + df, QString("%1").arg(n2));
  painter->drawText(rprect.getXMin() + 1, y3 + df, QString("%1").arg(n3));
  painter->drawText(rprect.getXMin() + 1, y4 + df, QString("%1").arg(n4));
  painter->drawText(rprect.getXMin() + 1, y5 + df, QString("%1").arg(n5));
}
