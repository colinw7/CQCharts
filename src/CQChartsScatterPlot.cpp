#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsValueSet.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQChartsTip.h>
#include <CQChartsDataLabel.h>
#include <CQCharts.h>
#include <CQPerfMonitor.h>
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
  addColumnParameter("x", "X", "xColumn").
    setTip("X Value").setRequired().setNumeric();
  addColumnParameter("y", "Y", "yColumn").
    setTip("Y Value").setRequired().setNumeric();

  addColumnParameter("name", "Name", "nameColumn").
    setTip("Value Name").setString();

  endParameterGroup();

  //---

  // custom columns/map
  startParameterGroup("Points");

  addColumnParameter("symbolType", "Symbol Type", "symbolTypeColumn").
   setTip("Custom Symbol Type").setMapped().
   setMapMinMax(CQChartsSymbol::minFillValue(), CQChartsSymbol::maxFillValue());

  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn").
   setTip("Custom Symbol Size").setMapped().
   setMapMinMax(CQChartsSymbolSize::minValue(), CQChartsSymbolSize::maxValue());

  addColumnParameter("fontSize", "Font Size", "fontSizeColumn").
   setTip("Custom Font Size for Text Label").setMapped().
   setMapMinMax(CQChartsFontSize::minValue(), CQChartsFontSize::maxValue());

  addBoolParameter("textLabels", "Text Labels", "textLabels").
   setTip("Show Text Label at Point");

  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();
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
 CQChartsObjPointData        <CQChartsScatterPlot>(this),
 CQChartsObjBestFitShapeData <CQChartsScatterPlot>(this),
 CQChartsObjHullShapeData    <CQChartsScatterPlot>(this),
 CQChartsObjRugPointData     <CQChartsScatterPlot>(this),
 CQChartsObjGridCellShapeData<CQChartsScatterPlot>(this)
{
  NoUpdate noUpdate(this);

  dataLabel_ = new CQChartsDataLabel(this);

  // set mapped range
  setSymbolTypeMapped(true);
  setSymbolTypeMapMin(CQChartsSymbol::minFillValue());
  setSymbolTypeMapMax(CQChartsSymbol::maxFillValue());

  setSymbolSizeMapped(true);
  setSymbolSizeMapMin(CQChartsSymbolSize::minValue());
  setSymbolSizeMapMax(CQChartsSymbolSize::maxValue());

  setFontSizeMapped(true);
  setFontSizeMapMin(CQChartsFontSize::minValue());
  setFontSizeMapMax(CQChartsFontSize::maxValue());

  //---

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setHullFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setRugSymbolType(CQChartsSymbol::Type::NONE);
  setRugSymbolSize(CQChartsLength("5px"));

  setBestFitFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setGridCellBorderColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.1));

  //---

  addAxes();

  addKey();

  addTitle();
}

CQChartsScatterPlot::
~CQChartsScatterPlot()
{
  delete dataLabel_;
}

//------

void
CQChartsScatterPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setGridded(bool b)
{
  CQChartsUtil::testAndSet(gridData_.enabled, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setGridNumX(int n)
{
  CQChartsUtil::testAndSet(gridData_.nx, n, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setGridNumY(int n)
{
  CQChartsUtil::testAndSet(gridData_.ny, n, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setSymbolMapKey(bool b)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.displayed, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolMapKeyAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.alpha, a, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolMapKeyMargin(double m)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.margin, m, [&]() { queueDrawObjs(); } );
}

//---

const CQChartsColumn &
CQChartsScatterPlot::
symbolTypeColumn() const
{
  return symbolTypeData_.column;
}

void
CQChartsScatterPlot::
setSymbolTypeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(symbolTypeData_.column, c, [&]() { queueUpdateObjs(); } );
}

bool
CQChartsScatterPlot::
isSymbolTypeMapped() const
{
  return symbolTypeData_.mapped;
}

void
CQChartsScatterPlot::
setSymbolTypeMapped(bool b)
{
  CQChartsUtil::testAndSet(symbolTypeData_.mapped, b, [&]() { queueUpdateObjs(); } );
}

int
CQChartsScatterPlot::
symbolTypeMapMin() const
{
  return symbolTypeData_.map_min;
}

void
CQChartsScatterPlot::
setSymbolTypeMapMin(int i)
{
  CQChartsUtil::testAndSet(symbolTypeData_.map_min, i, [&]() { queueUpdateObjs(); } );
}

int
CQChartsScatterPlot::
symbolTypeMapMax() const
{
  return symbolTypeData_.map_max;
}

void
CQChartsScatterPlot::
setSymbolTypeMapMax(int i)
{
  CQChartsUtil::testAndSet(symbolTypeData_.map_max, i, [&]() { queueUpdateObjs(); } );
}

//---

const CQChartsColumn &
CQChartsScatterPlot::
symbolSizeColumn() const
{
  return symbolSizeData_.column;
}

void
CQChartsScatterPlot::
setSymbolSizeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(symbolSizeData_.column, c, [&]() { queueUpdateRangeAndObjs(); } );
}

bool
CQChartsScatterPlot::
isSymbolSizeMapped() const
{
  return symbolSizeData_.mapped;
}

void
CQChartsScatterPlot::
setSymbolSizeMapped(bool b)
{
  CQChartsUtil::testAndSet(symbolSizeData_.mapped, b, [&]() { queueUpdateRangeAndObjs(); } );
}

double
CQChartsScatterPlot::
symbolSizeMapMin() const
{
  return symbolSizeData_.map_min;
}

void
CQChartsScatterPlot::
setSymbolSizeMapMin(double r)
{
  CQChartsUtil::testAndSet(symbolSizeData_.map_min, r, [&]() { queueUpdateRangeAndObjs(); } );
}

double
CQChartsScatterPlot::
symbolSizeMapMax() const
{
  return symbolSizeData_.map_max;
}

void
CQChartsScatterPlot::
setSymbolSizeMapMax(double r)
{
  CQChartsUtil::testAndSet(symbolSizeData_.map_max, r, [&]() { queueUpdateRangeAndObjs(); } );
}

const QString &
CQChartsScatterPlot::
symbolSizeMapUnits() const
{
  return symbolSizeData_.units;
}

void
CQChartsScatterPlot::
setSymbolSizeMapUnits(const QString &s)
{
  CQChartsUtil::testAndSet(symbolSizeData_.units, s, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

const CQChartsColumn &
CQChartsScatterPlot::
fontSizeColumn() const
{
  return fontSizeData_.column;
}

void
CQChartsScatterPlot::
setFontSizeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(fontSizeData_.column, c, [&]() { queueUpdateRangeAndObjs(); } );
}

bool
CQChartsScatterPlot::
isFontSizeMapped() const
{
  return fontSizeData_.mapped;
}

void
CQChartsScatterPlot::
setFontSizeMapped(bool b)
{
  CQChartsUtil::testAndSet(fontSizeData_.mapped, b, [&]() { queueUpdateRangeAndObjs(); } );
}

double
CQChartsScatterPlot::
fontSizeMapMin() const
{
  return fontSizeData_.map_min;
}

void
CQChartsScatterPlot::
setFontSizeMapMin(double r)
{
  CQChartsUtil::testAndSet(fontSizeData_.map_min, r, [&]() { queueUpdateRangeAndObjs(); } );
}

double
CQChartsScatterPlot::
fontSizeMapMax() const
{
  return fontSizeData_.map_max;
}

void
CQChartsScatterPlot::
setFontSizeMapMax(double r)
{
  CQChartsUtil::testAndSet(fontSizeData_.map_max, r, [&]() { queueUpdateRangeAndObjs(); } );
}

const QString &
CQChartsScatterPlot::
fontSizeMapUnits() const
{
  return fontSizeData_.units;
}

void
CQChartsScatterPlot::
setFontSizeMapUnits(const QString &s)
{
  CQChartsUtil::testAndSet(fontSizeData_.units, s, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

bool
CQChartsScatterPlot::
isTextLabels() const
{
  return dataLabel_->isVisible();
}

void
CQChartsScatterPlot::
setTextLabels(bool b)
{
  if (b != isTextLabels()) { dataLabel()->setVisible(b); queueDrawObjs(); }
}

void
CQChartsScatterPlot::
setDataLabelFont(const QFont &font)
{
  NoUpdate noUpdate(this);

  CQChartsDataLabel *dataLabel = this->dataLabel();

  dataLabel->setTextFont(font);
}

//---

void
CQChartsScatterPlot::
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.visible, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitDeviation(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.showDeviation, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitOrder(int o)
{
  CQChartsUtil::testAndSet(bestFitData_.order, o, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setHull(bool b)
{
  CQChartsUtil::testAndSet(hullData_.visible, b, [&]() { queueDrawObjs(); } );
}

//---

void
CQChartsScatterPlot::
setXRug(bool b)
{
  CQChartsUtil::testAndSet(axisRugData_.xVisible, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setYRug(bool b)
{
  CQChartsUtil::testAndSet(axisRugData_.yVisible, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setXRugSide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisRugData_.xSide, s, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setYRugSide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisRugData_.ySide, s, [&]() { queueDrawObjs(); } );
}

//------

void
CQChartsScatterPlot::
setXDensity(bool b)
{
  CQChartsUtil::testAndSet(axisDensityData_.xVisible, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setYDensity(bool b)
{
  CQChartsUtil::testAndSet(axisDensityData_.yVisible, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setXDensitySide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisDensityData_.xSide, s, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setYDensitySide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisDensityData_.ySide, s, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setDensityWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisDensityData_.width, l, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setDensityAlpha(double a)
{
  CQChartsUtil::testAndSet(axisDensityData_.alpha, a, [&]() { queueDrawObjs(); } );
}

//------

void
CQChartsScatterPlot::
setDensityMap(bool b)
{
  CQChartsUtil::testAndSet(densityMapData_.visible, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setDensityMapGridSize(int s)
{
  CQChartsUtil::testAndSet(densityMapData_.gridSize, s, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setDensityMapDelta(double d)
{
  CQChartsUtil::testAndSet(densityMapData_.delta, d, [&]() { queueDrawObjs(); } );
}

//------

void
CQChartsScatterPlot::
setXWhisker(bool b)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.xVisible, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setYWhisker(bool b)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.yVisible, b, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setXWhiskerSide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.xSide, s, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setYWhiskerSide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.ySide, s, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setWhiskerWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.width, l, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setWhiskerMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.margin, l, [&]() { queueDrawObjs(); } );
}

void
CQChartsScatterPlot::
setWhiskerAlpha(double a)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.alpha, a, [&]() { queueDrawObjs(); } );
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

  // best fit line and deviation fill
  addProperty("bestFit", this, "bestFit"         , "enabled"  );
  addProperty("bestFit", this, "bestFitDeviation", "deviation");
  addProperty("bestFit", this, "bestFitOrder"    , "order"    );

  addLineProperties("bestFit/stroke", "bestFitBorder");
  addFillProperties("bestFit/fill"  , "bestFitFill"  );

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
  dataLabel_->addPathProperties("dataLabel");

  // grid
  addProperty("grid", this, "gridded" , "enabled");
  addProperty("grid", this, "gridNumX", "nx"     );
  addProperty("grid", this, "gridNumY", "ny"     );

  addFillProperties("grid/fill"  , "gridCellFill"  );
  addProperty      ("grid/stroke", this, "gridCellBorder", "visible");
  addLineProperties("grid/stroke", "gridCellBorder");

  // symbol key
  addProperty("symbol/key", this, "symbolMapKey"      , "enabled");
  addProperty("symbol/key", this, "symbolMapKeyAlpha" , "alpha"  );
  addProperty("symbol/key", this, "symbolMapKeyMargin", "margin" );

  // mapping for columns (symbol type, size, font size, color)
  addProperty("symbol/map/type", this, "symbolTypeMapped", "enabled");
  addProperty("symbol/map/type", this, "symbolTypeMapMin", "min"    );
  addProperty("symbol/map/type", this, "symbolTypeMapMax", "max"    );

  addProperty("symbol/map/size", this, "symbolSizeMapped"  , "enabled");
  addProperty("symbol/map/size", this, "symbolSizeMapMin"  , "min"    );
  addProperty("symbol/map/size", this, "symbolSizeMapMax"  , "max"    );
  addProperty("symbol/map/size", this, "symbolSizeMapUnits", "units"  );

  addProperty("font/map/size", this, "fontSizeMapped"  , "enabled");
  addProperty("font/map/size", this, "fontSizeMapMin"  , "min"    );
  addProperty("font/map/size", this, "fontSizeMapMax"  , "max"    );
  addProperty("font/map/size", this, "fontSizeMapUnits", "units"  );

  // color map
  addColorMapProperties();
}

//---

CQChartsGeom::Range
CQChartsScatterPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsScatterPlot::calcRange");

  initGroupData(CQChartsColumns(), CQChartsColumn());

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsScatterPlot *plot) :
     plot_(plot) {
      hasGroups_ = (plot_->numGroups() > 1);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (plot_->isInterrupt())
        return State::TERMINATE;

      CQChartsModelIndex ind(data.row, plot_->xColumn(), data.parent);

      // init group
      int groupInd = plot_->rowGroupInd(ind);

      bool hidden = (hasGroups_ && plot_->isSetHidden(groupInd));

      if (! hidden) {
        bool ok1, ok2;

        double x = plot_->modelReal(data.row, plot_->xColumn(), data.parent, ok1);
        double y = plot_->modelReal(data.row, plot_->yColumn(), data.parent, ok2);

        if (! ok1) { x = uniqueId(data, plot_->xColumn()); ++uniqueX_; }
        if (! ok2) { y = uniqueId(data, plot_->yColumn()); ++uniqueY_; }

        if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
          return State::SKIP;

        range_.updateRange(x, y);
      }

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      bool ok;

      QVariant var = plot_->modelValue(data.row, column, data.parent, ok);
      if (! var.isValid()) return -1;

      return columnDetails(column)->uniqueId(var);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        CQChartsModelData *modelData = plot_->getModelData();

        details_ = modelData->details();
      }

      return details_->columnDetails(column);
    }

    const CQChartsGeom::Range &range() const { return range_; }

    bool isUniqueX() const { return uniqueX_ == numRows(); }
    bool isUniqueY() const { return uniqueY_ == numRows(); }

   private:
    const CQChartsScatterPlot* plot_      { nullptr };
    int                        hasGroups_ { false };
    CQChartsGeom::Range        range_;
    CQChartsModelDetails*      details_   { nullptr };
    int                        uniqueX_   { 0 };
    int                        uniqueY_   { 0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  CQChartsGeom::Range dataRange = visitor.range();

  bool uniqueX = visitor.isUniqueX();
  bool uniqueY = visitor.isUniqueY();

  if (isInterrupt())
    return dataRange;

  //---

  if (dataRange.isSet()) {
    if (uniqueX || uniqueY) {
      if (uniqueX) {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(xColumn());

        for (int i = 0; i < columnDetails->numUnique(); ++i)
          xAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());

        dataRange.updateRange(dataRange.xmin() - 0.5, dataRange.ymin());
        dataRange.updateRange(dataRange.xmax() + 0.5, dataRange.ymin());
      }

      if (uniqueY) {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(yColumn());

        for (int i = 0; i < columnDetails->numUnique(); ++i)
          yAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());

        dataRange.updateRange(dataRange.xmin(), dataRange.ymin() - 0.5);
        dataRange.updateRange(dataRange.xmax(), dataRange.ymax() + 0.5);
      }
    }
  }

  //---

  dataRange = adjustDataRange(dataRange);

  //---

  // update data range if unset
  if (dataRange.isSet() && CMathUtil::isZero(dataRange.xsize())) {
    double x = dataRange.xmid();
    double y = dataRange.ymid();

    dataRange.updateRange(x - 1.0, y);
    dataRange.updateRange(x + 1.0, y);
  }

  if (dataRange.isSet() && CMathUtil::isZero(dataRange.ysize())) {
    double x = dataRange.xmid();
    double y = dataRange.ymid();

    dataRange.updateRange(x, y - 1.0);
    dataRange.updateRange(x, y + 1.0);
  }

  //---

  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  th->initGridData(dataRange);

  //---

  if (isGridded()) {
    if (dataRange.isSet()) {
      dataRange.updateRange(gridData_.xinterval.calcStart(), gridData_.yinterval.calcStart());
      dataRange.updateRange(gridData_.xinterval.calcEnd  (), gridData_.yinterval.calcEnd  ());
    }
  }

  //---

  th->initAxes(uniqueX, uniqueY);

  return dataRange;
}

void
CQChartsScatterPlot::
initGridData(const CQChartsGeom::Range &dataRange)
{
  if (dataRange.isSet()) {
    gridData_.xinterval.setStart   (dataRange.xmin());
    gridData_.xinterval.setEnd     (dataRange.xmax());
    gridData_.xinterval.setNumMajor(gridData_.nx);

    gridData_.yinterval.setStart   (dataRange.ymin());
    gridData_.yinterval.setEnd     (dataRange.ymax());
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
}

void
CQChartsScatterPlot::
initAxes(bool uniqueX, bool uniqueY)
{
  setXValueColumn(xColumn());
  setYValueColumn(yColumn());

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

  //---

  ColumnType xColumnType = columnValueType(xColumn());

  if (xColumnType == CQBaseModelType::TIME)
    xAxis()->setDate(true);
}

//------

void
CQChartsScatterPlot::
clearPlotObjects()
{
  groupNameValues_  .clear();
  groupNameGridData_.clear();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsScatterPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsScatterPlot::createObjs");

  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  NoUpdate noUpdate(th);

  //---

  // init value set
  //initValueSets();

  //---

  // init name values
  th->gridData_.maxN = 0;

  if (groupNameValues_.empty())
    addNameValues();

  th->groupPoints_  .clear();
  th->groupFitData_ .clear();
  th->groupHull_    .clear();
  th->groupWhiskers_.clear();

  //---

  // get column titles
  bool ok;

  th->xname_          = modelHeaderString(xColumn         (), ok);
  th->yname_          = modelHeaderString(yColumn         (), ok);
  th->symbolTypeName_ = modelHeaderString(symbolTypeColumn(), ok);
  th->symbolSizeName_ = modelHeaderString(symbolSizeColumn(), ok);
  th->fontSizeName_   = modelHeaderString(fontSizeColumn  (), ok);
  th->colorName_      = modelHeaderString(colorColumn     (), ok);

  if (! xname_         .length()) th->xname_          = "x";
  if (! yname_         .length()) th->yname_          = "y";
  if (! symbolTypeName_.length()) th->symbolTypeName_ = "symbolType";
  if (! symbolSizeName_.length()) th->symbolSizeName_ = "symbolSize";
  if (! fontSizeName_  .length()) th->fontSizeName_   = "fontSize";
  if (! colorName_     .length()) th->colorName_      = "color";

  //---

  addPointObjects(objs);

  addGridObjects(objs);

  //---

  return true;
}

void
CQChartsScatterPlot::
addPointObjects(PlotObjs &objs) const
{
  initSymbolTypeData();
  initSymbolSizeData();
  initFontSizeData  ();

  //---

  int hasGroups = (numGroups() > 1);

  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int               groupInd   = groupNameValue.first;
    const NameValues &nameValues = groupNameValue.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    // get group points
    auto pg = groupPoints_.find(groupInd);

    if (pg == groupPoints_.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

      auto pg1 = th->groupPoints_.find(groupInd);

      if (pg1 == th->groupPoints_.end())
        pg1 = th->groupPoints_.insert(pg1, GroupPoints::value_type(groupInd, Points()));

      pg = groupPoints_.find(groupInd);
    }

    Points &points = const_cast<Points &>((*pg).second);

    //---

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        return;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

      const QString &name   = nameValue.first;
      const Values  &values = nameValue.second.values;

      int nv = values.size();

      for (int iv = 0; iv < nv; ++iv) {
        if (isInterrupt())
          return;

        const ValueData &valuePoint = values[iv];

        const QPointF &p = valuePoint.p;

        //---

        CQChartsSymbol symbolType = this->symbolType();

        if (symbolTypeColumn().isValid())
          columnSymbolType(valuePoint.row, valuePoint.ind.parent(), symbolType);

        //---

        CQChartsLength symbolSize = this->symbolSize();

        if (symbolSizeColumn().isValid())
          columnSymbolSize(valuePoint.row, valuePoint.ind.parent(), symbolSize);

        //---

        double dataLabelFontSize = dataLabel()->textFont().pointSizeF();

        CQChartsLength fontSize(dataLabelFontSize, CQChartsUnits::PIXEL);

        if (fontSizeColumn().isValid())
          columnFontSize(valuePoint.row, valuePoint.ind.parent(), fontSize);

        //---

        CQChartsColor color;

        if (colorColumn().isValid()) {
          (void) columnColor(valuePoint.row, valuePoint.ind.parent(), color);
        }

        //---

        double sx, sy;

        plotSymbolSize(symbolSize, sx, sy);

        CQChartsGeom::BBox bbox(p.x() - sx, p.y() - sy, p.x() + sx, p.y() + sy);

        CQChartsScatterPointObj *pointObj =
          new CQChartsScatterPointObj(this, groupInd, bbox, p, symbolType, symbolSize, fontSize,
                                      color, ig, ng, is, ns, iv, nv);

        //---

        pointObj->setName(name);

        pointObj->setInd(valuePoint.ind);

        objs.push_back(pointObj);

        //---

        points.push_back(p);
      }

      ++is;
    }

    ++ig;
  }
}

void
CQChartsScatterPlot::
addGridObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  //---

  int ig = 0;
  int ng = groupNameGridData_.size();

  for (const auto &pg : groupNameGridData_) {
    if (isInterrupt())
      return;

    int                 groupInd     = pg.first;
    const NameGridData &nameGridData = pg.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    int is = 0;
    int ns = nameGridData.size();

    for (const auto &pn : nameGridData) {
      if (isInterrupt())
        return;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

    //const QString       &name          = pn.first;
      const CellPointData &cellPointData = pn.second;

    //int nx = cellPointData.size();

      int maxN = cellPointData.maxN;

      for (const auto &px : cellPointData.xyPoints) {
        if (isInterrupt())
          return;

        int            ix      = px.first;
        const YPoints &yPoints = px.second;

        double xmin, xmax;

        gridData_.xinterval.intervalValues(ix, xmin, xmax);

      //int ny = yPoints.size();

        for (const auto &py : yPoints) {
          if (isInterrupt())
            return;

          int           iy     = py.first;
          const Points &points = py.second;

          double ymin, ymax;

          gridData_.yinterval.intervalValues(iy, ymin, ymax);

          CQChartsGeom::BBox bbox(xmin, ymin, xmax, ymax);

          CQChartsScatterCellObj *cellObj =
            new CQChartsScatterCellObj(this, groupInd, bbox, ig, ng, is, ns, ix, iy, points, maxN);

          objs.push_back(cellObj);
        }
      }

      ++is;
    }

    ++ig;
  }
}

void
CQChartsScatterPlot::
addNameValues() const
{
  CQPerfTrace trace("CQChartsScatterPlot::addNameValues");

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
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

      if (! ok1) x = uniqueId(data, plot_->xColumn());
      if (! ok2) y = uniqueId(data, plot_->yColumn());

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      // get optional name
      QString name;

      if (plot_->nameColumn().isValid()) {
        bool ok;

        name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok);
      }

      if (! name.length() && plot_->title())
        name = plot_->title()->textStr();

      if (! name.length() && plot_->xAxis())
        name = plot_->xAxis()->label();

      //---

      // get symbol type, size, font size and color
      CQChartsColor color;

      // get color label (needed if not string ?)
      if (plot_->colorColumn().isValid()) {
        (void) plot_->columnColor(data.row, data.parent, color);
      }

      //---

      CQChartsScatterPlot *plot = const_cast<CQChartsScatterPlot *>(plot_);

      plot->addNameValue(groupInd, name, x, y, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      bool ok;

      QVariant var = plot_->modelValue(data.row, column, data.parent, ok);
      if (! var.isValid()) return -1;

      return columnDetails(column)->uniqueId(var);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        CQChartsModelData *modelData = plot_->getModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

   private:
    const CQChartsScatterPlot* plot_    { nullptr };
    CQChartsModelDetails*      details_ { nullptr };
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

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, groupInd , ig, ng);
      CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, groupName, ig, ng);

      key->addItem(colorItem, ig, 0);
      key->addItem(textItem , ig, 1);

      //--

      if (colorColumn().isValid() && colorColumn().isGroup()) {
        if (isColorMapped()) {
          double r = CMathUtil::map(groupInd, colorColumnData_.data_min, colorColumnData_.data_max,
                                    colorMapMin(), colorMapMax());

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

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, -1  , is, ns);
      CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, name, is, ns);

      key->addItem(colorItem, is, 0);
      key->addItem(textItem , is, 1);

      //--

      if (colorColumn().isValid()) {
        int nv = values.size();

        if (nv > 0) {
          const ValueData &valuePoint = values[0];

          CQChartsColor color;

          if (columnColor(valuePoint.row, valuePoint.ind.parent(), color))
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
drawBackground(QPainter *painter) const
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
drawForeground(QPainter *painter) const
{
  if (isSymbolMapKey())
    drawSymbolMapKey(painter);
}

void
CQChartsScatterPlot::
drawBestFit(QPainter *painter) const
{
  // init fit data
  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int groupInd = groupNameValue.first;

    CQChartsFitData &fitData = th->groupFitData_[groupInd];

    if (! fitData.isFitted()) {
      auto p = groupPoints_.find(groupInd);

      if (p != groupPoints_.end()) {
        const Points &points = (*p).second;

        fitData.calc(points, bestFitOrder());
      }
    }
  }

  //---

  // draw fit data
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int groupInd = groupNameValue.first;

    auto pf = groupFitData_.find(groupInd);

    if (pf == groupFitData_.end())
      continue;

    const CQChartsFitData &fitData = (*pf).second;

    //---

    double pxl, pyl, pxr, pyr;

    windowToPixel(fitData.xmin(), 0, pxl, pyl);
    windowToPixel(fitData.xmax(), 0, pxr, pyr);

    //---

    // set pen and brush
    QPen   pen;
    QBrush brush;

    QColor borderColor = interpBestFitBorderColor(ig, ng);
    QColor fillColor   = interpBestFitFillColor  (ig, ng);

    setPen(pen, isBestFitBorder(), borderColor, bestFitBorderAlpha(),
           bestFitBorderWidth(), bestFitBorderDash());

    setBrush(brush, isBestFitFilled(), fillColor, bestFitFillAlpha(), bestFitFillPattern());

    //---

    // calc fit shape
    QPolygonF bpoly, poly, tpoly;

    for (int px = pxl; px <= pxr; ++px) {
      if (isInterrupt())
        return;

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
        if (isInterrupt())
          return;

        const QPointF &p = bpoly[i];

        dpoly << p;
      }

      for (int i = tpoly.size() - 1; i >= 0; --i) {
        if (isInterrupt())
          return;

        const QPointF &p = tpoly[i];

        dpoly << p;
      }

      painter->setPen  (pen);
      painter->setBrush(brush);

      painter->drawPolygon(dpoly);
    }

    //---

    // draw fit line
    QPainterPath path;

    const QPointF &p = poly[0];

    path.moveTo(p);

    for (int i = 1; i < poly.size(); ++i) {
      if (isInterrupt())
        return;

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
drawHull(QPainter *painter) const
{
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int groupInd = groupNameValue.first;

    auto ph = groupHull_.find(groupInd);

    if (ph != groupHull_.end()) {
      const CQChartsGrahamHull &hull = (*ph).second;

      if (hull.numPoints() == 0) {
        CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

        CQChartsGrahamHull &hull = th->groupHull_[groupInd];

        const Points &points = th->groupPoints_[groupInd];

        //---

        std::vector<double> x, y;

        for (const auto &p : points) {
          if (isInterrupt())
            return;

          hull.addPoint(p);
        }

        hull.calc();
      }

      ph = groupHull_.find(groupInd);
    }

    const CQChartsGrahamHull &hull = (*ph).second;

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
drawXRug(QPainter *painter) const
{
  for (const auto &plotObj : plotObjects()) {
    if (isInterrupt())
      return;

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
drawYRug(QPainter *painter) const
{
  for (const auto &plotObj : plotObjects()) {
    if (isInterrupt())
      return;

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
drawXDensity(QPainter *painter) const
{
  initWhiskerData();

  //---

  if (! isGridded()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      if (isInterrupt())
        return;

      int groupInd = groupNameValue.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawXDensityWhisker(painter, whiskerData, ig, ng);
      }

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      if (isInterrupt())
        return;

      int groupInd = pg.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawXDensityWhisker(painter, whiskerData, ig, ng);
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawYDensity(QPainter *painter) const
{
  initWhiskerData();

  //---

  if (! isGridded()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      if (isInterrupt())
        return;

      int groupInd = groupNameValue.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawYDensityWhisker(painter, whiskerData, ig, ng);
      }

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      if (isInterrupt())
        return;

      int groupInd = pg.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawYDensityWhisker(painter, whiskerData, ig, ng);
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawXDensityWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng) const
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig, ng);
  QColor fillColor   = interpSymbolFillColor  (ig, ng);

  setPenBrush(pen, brush,
    /*stroked*/ true, strokeColor, symbolStrokeAlpha(), CQChartsLength(), CQChartsLineDash(),
    /*filled*/ true, fillColor, densityAlpha(), CQChartsFillPattern());

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
drawYDensityWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng) const
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig, ng);
  QColor fillColor   = interpSymbolFillColor  (ig, ng);

  setPenBrush(pen, brush,
    /*stroked*/ true, strokeColor, symbolStrokeAlpha(), CQChartsLength(), CQChartsLineDash(),
    /*filled*/ true, fillColor, densityAlpha(), symbolFillPattern());

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
drawDensityMap(QPainter *painter) const
{
  painter->save();

  setClipRect(painter);

  //---

  initWhiskerData();

  //---

  int gridSize = std::max(densityMapGridSize(), 1);

  double delta = densityMapDelta();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    const NameValues &nameValues = groupNameValue.second;

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        return;

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
        if (isInterrupt())
          return;

        double x1 = (xmax > xmin ? (v.p.x() - xmin)/(xmax - xmin) : 0.0);
        double y1 = (ymax > ymin ? (v.p.y() - ymin)/(ymax - ymin) : 0.0);

        xv.push_back(x1);
        yv.push_back(y1);
      }

      CMathBivariate bivariate(xv, yv);

      for (int y = y1; y <= y2; y += dy) {
        if (isInterrupt())
          return;

        for (int x = x1; x <= x2; x += dx) {
          if (isInterrupt())
            return;

          QPointF p = pixelToWindow(QPointF(x, y));

          double x1 = (p.x() - xmin)/(xmax - xmin);
          double y1 = (p.y() - ymin)/(ymax - ymin);

          double a = 1.0;
          double v = bivariate.calc(x1, y1);

          if (delta > 0.0) {
            double v1 = CMathRound::RoundDown(v/delta)*delta;

            a = CMathUtil::clamp(sqrt(1.0 - (v - v1)), 0.0, 1.0);
          }

          //---

          QBrush brush;

          QColor c = interpPaletteColor(v);

          setBrush(brush, true, c, a, CQChartsFillPattern());

          //---

          painter->fillRect(QRectF(x, y, dx, dy), brush);
        }
      }
    }
  }

  //---

  painter->restore();
}

//---

void
CQChartsScatterPlot::
drawXWhisker(QPainter *painter) const
{
  initWhiskerData();

  //---

  if (! isGridded()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      if (isInterrupt())
        return;

      int groupInd = groupNameValue.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawXWhiskerWhisker(painter, whiskerData, ig, ng);
      }

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      if (isInterrupt())
        return;

      int groupInd = pg.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawXWhiskerWhisker(painter, whiskerData, ig, ng);
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawYWhisker(QPainter *painter) const
{
  initWhiskerData();

  //---

  if (! isGridded()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      if (isInterrupt())
        return;

      int groupInd = groupNameValue.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawYWhiskerWhisker(painter, whiskerData, ig, ng);
      }

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      if (isInterrupt())
        return;

      int groupInd = pg.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawYWhiskerWhisker(painter, whiskerData, ig, ng);
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawXWhiskerWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng) const
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig, ng);
  QColor fillColor   = interpSymbolFillColor  (ig, ng);

  setPenBrush(pen, brush,
    /*stroked*/ true, strokeColor, symbolStrokeAlpha(), CQChartsLength(), CQChartsLineDash(),
    /*filled*/ true, fillColor, whiskerAlpha(), symbolFillPattern());

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  double ww = lengthPlotHeight(whiskerWidth ());
  double wm = lengthPlotHeight(whiskerMargin());

  const CQChartsGeom::Range &dataRange = this->dataRange();

  double pos = (xWhiskerSide() == YSide::BOTTOM ?
    dataRange.ymin() - (ig + 1)*ww - wm : dataRange.ymax() + ig*ww + wm);

  CQChartsGeom::BBox rect(whiskerData.xWhisker.min(), pos, whiskerData.xWhisker.max(), pos + ww);

  CQChartsBoxWhiskerUtil::drawWhisker(this, painter, whiskerData.xWhisker,
                                      rect, whiskerWidth(), Qt::Horizontal);
}

void
CQChartsScatterPlot::
drawYWhiskerWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng) const
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig, ng);
  QColor fillColor   = interpSymbolFillColor  (ig, ng);

  setPenBrush(pen, brush,
    /*stroked*/ true, strokeColor, symbolStrokeAlpha(), CQChartsLength(), CQChartsLineDash(),
    /*filled*/ true, fillColor, whiskerAlpha(), symbolFillPattern());

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
initWhiskerData() const
{
  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    //---

    // get group whiskers
    int groupInd = groupNameValue.first;

    auto pw = groupWhiskers_.find(groupInd);

    if (pw == groupWhiskers_.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

      auto pw1 = th->groupWhiskers_.find(groupInd);

      if (pw1 == th->groupWhiskers_.end()) {
        (void) th->groupWhiskers_[groupInd];
        //pw1 = th->groupWhiskers_.insert(pw1, GroupWhiskers::value_type(groupInd, WhiskerData()));
      }

      pw = groupWhiskers_.find(groupInd);
    }

    const WhiskerData &whiskerData = (*pw).second;

    //---

    // init whisker if needed
    if (! whiskerData.xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd) {
          WhiskerData &whiskerData1 = const_cast<WhiskerData &>(whiskerData);

          whiskerData1.xWhisker.addValue(pointObj->point().x());
        }
      }
    }

    if (! whiskerData.yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd) {
          WhiskerData &whiskerData1 = const_cast<WhiskerData &>(whiskerData);

          whiskerData1.yWhisker.addValue(pointObj->point().y());
        }
      }
    }
  }

  //---

  for (const auto &pg : groupNameGridData_) {
    if (isInterrupt())
      return;

    //---

    // get group whiskers
    int groupInd = pg.first;

    auto pw = groupWhiskers_.find(groupInd);

    if (pw == groupWhiskers_.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

      auto pw1 = th->groupWhiskers_.find(groupInd);

      if (pw1 == th->groupWhiskers_.end()) {
        (void) th->groupWhiskers_[groupInd];
        //pw1 = th->groupWhiskers_.insert(pw1, GroupWhiskers::value_type(groupInd, WhiskerData()));
      }

      pw = groupWhiskers_.find(groupInd);
    }

    const WhiskerData &whiskerData = (*pw).second;

    //---

    // init whisker if needed
    if (! whiskerData.xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterCellObj *cellObj =
          dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points()) {
            if (isInterrupt())
              return;

            WhiskerData &whiskerData1 = const_cast<WhiskerData &>(whiskerData);

            whiskerData1.xWhisker.addValue(p.x());
          }
        }
      }
    }

    if (! whiskerData.yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterCellObj *cellObj =
          dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points()) {
            if (isInterrupt())
              return;

            WhiskerData &whiskerData1 = const_cast<WhiskerData &>(whiskerData);

            whiskerData1.yWhisker.addValue(p.y());
          }
        }
      }
    }
  }
}

//------

void
CQChartsScatterPlot::
initSymbolTypeData() const
{
  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  th->symbolTypeData_.valid = false;

  if (! symbolTypeColumn().isValid())
    return;

  CQChartsModelColumnDetails *columnDetails = this->columnDetails(symbolTypeColumn());
  if (! columnDetails) return;

  if (symbolTypeColumn().isGroup()) {
    th->symbolTypeData_.data_min = 0.0;
    th->symbolTypeData_.data_max = std::max(numGroups() - 1, 0);
  }
  else {
    if (symbolTypeData_.mapped) {
      QVariant minVar = columnDetails->minValue();
      QVariant maxVar = columnDetails->maxValue();

      bool ok;

      th->symbolTypeData_.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) th->symbolTypeData_.data_min = 0.0;

      th->symbolTypeData_.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) th->symbolTypeData_.data_max = 1.0;
    }

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), symbolTypeColumn(),
                                              columnType, columnBaseType, nameValues);

    if (columnType == CQBaseModelType::SYMBOL) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

      const CQChartsColumnSymbolTypeType *symbolTypeType =
        dynamic_cast<const CQChartsColumnSymbolTypeType *>(columnTypeMgr->getType(columnType));
      assert(symbolTypeType);

      symbolTypeType->getMapData(charts(), model().data(), symbolTypeColumn(), nameValues,
                                 th->symbolTypeData_.mapped,
                                 th->symbolTypeData_.map_min, th->symbolTypeData_.map_max,
                                 th->symbolTypeData_.data_min, th->symbolTypeData_.data_max);
    }
  }

  th->symbolTypeData_.valid = true;
}

bool
CQChartsScatterPlot::
columnSymbolType(int row, const QModelIndex &parent, CQChartsSymbol &symbolType) const
{
  if (! symbolTypeData_.valid)
    return false;

  bool ok;

  QVariant var = modelValue(row, symbolTypeColumn(), parent, ok);
  if (! ok || ! var.isValid()) return false;

  if (symbolTypeData_.mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      int i = CQChartsVariant::toInt(var, ok);
      if (! ok) return false;

      int i1 = CMathUtil::map(i, symbolTypeData_.data_min, symbolTypeData_.data_max,
                              symbolTypeData_.map_min, symbolTypeData_.map_max);

      symbolType = CQChartsSymbol::outlineFromInt(i1);
    }
    else {
      if (CQChartsVariant::isSymbol(var)) {
        symbolType = CQChartsVariant::toSymbol(var, ok);
      }
      else {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(symbolTypeColumn());
        if (! columnDetails) return false;

        // use unique index/count of edit values (which may have been converted)
        // not same as CQChartsColumnColorType::userData
        int n = columnDetails->numUnique();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        symbolType = CQChartsSymbol::interpOutline(r);
      }
    }
  }
  else {
    if (CQChartsVariant::isSymbol(var)) {
      symbolType = CQChartsVariant::toSymbol(var, ok);
    }
    else {
      QString str = CQChartsVariant::toString(var, ok);

      symbolType = CQChartsSymbol(str);
    }
  }

  return symbolType.isValid();
}

//------

void
CQChartsScatterPlot::
initSymbolSizeData() const
{
  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  th->symbolSizeData_.valid = false;

  if (! symbolSizeColumn().isValid())
    return;

  CQChartsModelColumnDetails *columnDetails = this->columnDetails(symbolSizeColumn());
  if (! columnDetails) return;

  if (symbolSizeColumn().isGroup()) {
    th->symbolSizeData_.data_min  = 0.0;
    th->symbolSizeData_.data_max  = std::max(numGroups() - 1, 0);
    th->symbolSizeData_.data_mean = symbolSizeData_.data_max/2.0;
  }
  else {
    if (symbolSizeData_.mapped) {
      QVariant minVar  = columnDetails->minValue();
      QVariant maxVar  = columnDetails->maxValue();
      QVariant meanVar = columnDetails->meanValue();

      bool ok;

      th->symbolSizeData_.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) th->symbolSizeData_.data_min = 0.0;

      th->symbolSizeData_.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) th->symbolSizeData_.data_max = 1.0;

      th->symbolSizeData_.data_mean = CQChartsVariant::toReal(meanVar, ok);
      if (! ok) th->symbolSizeData_.data_mean =
                  CMathUtil::avg(symbolSizeData_.data_min, symbolSizeData_.data_max);
    }

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), symbolSizeColumn(),
                                              columnType, columnBaseType, nameValues);

    if (columnType == CQBaseModelType::SYMBOL_SIZE) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

      const CQChartsColumnSymbolSizeType *symbolSizeType =
        dynamic_cast<const CQChartsColumnSymbolSizeType *>(columnTypeMgr->getType(columnType));
      assert(symbolSizeType);

      symbolSizeType->getMapData(charts(), model().data(), symbolSizeColumn(), nameValues,
                                 th->symbolSizeData_.mapped,
                                 th->symbolSizeData_.map_min, th->symbolSizeData_.map_max,
                                 th->symbolSizeData_.data_min, th->symbolSizeData_.data_max);
    }
  }

  th->symbolSizeData_.valid = true;
}

bool
CQChartsScatterPlot::
columnSymbolSize(int row, const QModelIndex &parent, CQChartsLength &symbolSize) const
{
  if (! symbolSizeData_.valid)
    return false;

  CQChartsUnits units = CQChartsUnits::PIXEL;

  (void) CQChartsUtil::decodeUnits(symbolSizeMapUnits(), units);

  bool ok;

  QVariant var = modelValue(row, symbolSizeColumn(), parent, ok);
  if (! ok || ! var.isValid()) return false;

  if (symbolSizeData_.mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, symbolSizeData_.data_min, symbolSizeData_.data_max,
                                 symbolSizeData_.map_min, symbolSizeData_.map_max);

      symbolSize = CQChartsLength(r1, units);
    }
    else {
      if (CQChartsVariant::isLength(var)) {
        symbolSize = CQChartsVariant::toLength(var, ok);
      }
      else {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(symbolSizeColumn());
        if (! columnDetails) return false;

        // use unique index/count of edit values (which may have been converted)
        // not same as CQChartsColumnColorSize::userData
        int n = columnDetails->numUnique();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        double r1 = CMathUtil::map(r, 0.0, 1.0,
                                   CQChartsSymbolSize::minValue(),
                                   CQChartsSymbolSize::maxValue());

        symbolSize = CQChartsLength(r1, units);
      }
    }
  }
  else {
    if (CQChartsVariant::isLength(var)) {
      symbolSize = CQChartsVariant::toLength(var, ok);
    }
    else {
      QString str = CQChartsVariant::toString(var, ok);

      symbolSize = CQChartsLength(str, units);
    }
  }

  return symbolSize.isValid();
}

//------

void
CQChartsScatterPlot::
initFontSizeData() const
{
  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  th->fontSizeData_.valid = false;

  if (! fontSizeColumn().isValid())
    return;

  CQChartsModelColumnDetails *columnDetails = this->columnDetails(fontSizeColumn());
  if (! columnDetails) return;

  if (fontSizeColumn().isGroup()) {
    th->fontSizeData_.data_min = 0.0;
    th->fontSizeData_.data_max = std::max(numGroups() - 1, 0);
  }
  else {
    if (fontSizeData_.mapped) {
      QVariant minVar = columnDetails->minValue();
      QVariant maxVar = columnDetails->maxValue();

      bool ok;

      th->fontSizeData_.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) th->fontSizeData_.data_min = 0.0;

      th->fontSizeData_.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) th->fontSizeData_.data_max = 1.0;
    }

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), fontSizeColumn(),
                                              columnType, columnBaseType, nameValues);

    if (columnType == CQBaseModelType::FONT_SIZE) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

      const CQChartsColumnFontSizeType *fontSizeType =
        dynamic_cast<const CQChartsColumnFontSizeType *>(columnTypeMgr->getType(columnType));
      assert(fontSizeType);

      fontSizeType->getMapData(charts(), model().data(), fontSizeColumn(), nameValues,
                               th->fontSizeData_.mapped,
                               th->fontSizeData_.map_min, th->fontSizeData_.map_max,
                               th->fontSizeData_.data_min, th->fontSizeData_.data_max);
    }
  }

  th->fontSizeData_.valid = true;
}

bool
CQChartsScatterPlot::
columnFontSize(int row, const QModelIndex &parent, CQChartsLength &fontSize) const
{
  if (! fontSizeData_.valid)
    return false;

  CQChartsUnits units = CQChartsUnits::PIXEL;

  (void) CQChartsUtil::decodeUnits(fontSizeMapUnits(), units);

  bool ok;

  QVariant var = modelValue(row, fontSizeColumn(), parent, ok);
  if (! ok || ! var.isValid()) return false;

  if (fontSizeData_.mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, fontSizeData_.data_min, fontSizeData_.data_max,
                                 fontSizeData_.map_min, fontSizeData_.map_max);

      fontSize = CQChartsLength(r1, units);
    }
    else {
      if (CQChartsVariant::isLength(var)) {
        fontSize = CQChartsVariant::toLength(var, ok);
      }
      else {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(fontSizeColumn());
        if (! columnDetails) return false;

        // use unique index/count of edit values (which may have been converted)
        // not same as CQChartsColumnColorSize::userData
        int n = columnDetails->numUnique();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        double r1 = CMathUtil::map(r, 0.0, 1.0,
                                   CQChartsFontSize::minValue(),
                                   CQChartsFontSize::maxValue());

        fontSize = CQChartsLength(r1, units);
      }
    }
  }
  else {
    if (CQChartsVariant::isLength(var)) {
      fontSize = CQChartsVariant::toLength(var, ok);
    }
    else {
      QString str = CQChartsVariant::toString(var, ok);

      fontSize = CQChartsLength(str, units);
    }
  }

  return fontSize.isValid();
}

//------

void
CQChartsScatterPlot::
drawSymbolMapKey(QPainter *painter) const
{
  if (! symbolSizeColumn().isValid())
    return;

  //---

  // draw size key
  double min  = symbolSizeData_.data_min;
  double mean = symbolSizeData_.data_mean;
  double max  = symbolSizeData_.data_max;

  CQChartsGeom::BBox pbbox = calcPlotPixelRect();

  //double px, py;

  //double vx = view()->viewportRange();
  //double vy = 0.0;

  //view()->windowToPixel(vx, vy, px, py);

  double px = pbbox.getXMax();
  double py = pbbox.getYMax();

  double pm = symbolMapKeyMargin();

  double pr1 = symbolSizeData_.map_max;
  double pr3 = symbolSizeData_.map_min;

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

  drawText(painter, QPointF(r1.center().x(), r1.top()), QString("%1").arg(max ));
  drawText(painter, QPointF(r2.center().x(), r2.top()), QString("%1").arg(mean));
  drawText(painter, QPointF(r3.center().x(), r3.top()), QString("%1").arg(min ));
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(const CQChartsScatterPlot *plot, int groupInd,
                        const CQChartsGeom::BBox &rect, const QPointF &p,
                        const CQChartsSymbol &symbolType, const CQChartsLength &symbolSize,
                        const CQChartsLength &fontSize, const CQChartsColor &color,
                        int ig, int ng, int is, int ns, int iv, int nv) :
 CQChartsPlotObj(const_cast<CQChartsScatterPlot *>(plot), rect), plot_(plot), groupInd_(groupInd),
 p_(p), symbolType_(symbolType), symbolSize_(symbolSize), fontSize_(fontSize), color_(color),
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

  QString xstr = plot()->xStr(p_.x());
  QString ystr = plot()->yStr(p_.y());

  tableTip.addTableRow(plot_->xname(), xstr);
  tableTip.addTableRow(plot_->yname(), ystr);

  auto pg = plot_->groupNameValues().find(groupInd_);
  assert(pg != plot_->groupNameValues().end());

  auto p = (*pg).second.find(name_);
  assert(p != (*pg).second.end());

  const CQChartsScatterPlot::Values &values = (*p).second.values;

  const CQChartsScatterPlot::ValueData &valuePoint = values[iv_];

  //---

  if (plot_->symbolTypeColumn().isValid()) {
    bool ok;

    QString symbolTypeStr =
      plot_->modelString(ind_.row(), plot_->symbolTypeColumn(), ind_.parent(), ok);

    if (ok)
      tableTip.addTableRow(plot_->symbolTypeName(), symbolTypeStr);
  }

  //---

  if (plot_->symbolSizeColumn().isValid()) {
    bool ok;

    QString symbolSizeStr =
      plot_->modelString(ind_.row(), plot_->symbolSizeColumn(), ind_.parent(), ok);

    if (ok)
      tableTip.addTableRow(plot_->symbolSizeName(), symbolSizeStr);
  }

  //---

  if (plot_->fontSizeColumn().isValid()) {
    bool ok;

    QString fontSizeStr =
      plot_->modelString(ind_.row(), plot_->fontSizeColumn(), ind_.parent(), ok);

    if (ok)
      tableTip.addTableRow(plot_->fontSizeName(), fontSizeStr);
  }

  //---

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

  if (color_.isValid()) {
    QColor c = plot_->charts()->interpColor(color_, ic, nc);

    c.setAlphaF(plot_->symbolFillAlpha());

    brush.setColor(c);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // get symbol type and size
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
  if (plot_->dataLabel()->isVisible()) {
    const CQChartsDataLabel *dataLabel = plot_->dataLabel();

    //---

    QPen tpen;

    QColor tc = dataLabel->interpTextColor(ic, nc);

    plot_->setPen(tpen, true, tc, dataLabel->textAlpha());

    //---

    double fontSize = dataLabel->textFont().pointSizeF();

    if (fontSize_.isValid())
      fontSize = plot_->lengthPixelHeight(fontSize_);

    //---

    QFont font = dataLabel->textFont();

    if (fontSize > 0) {
      // scale to font size
      fontSize = plot_->limitFontSize(fontSize);

      QFont font1 = font;

      font1.setPointSizeF(fontSize);

      const_cast<CQChartsScatterPlot *>(plot_)->setDataLabelFont(font1);
    }

    dataLabel->draw(painter, erect, name_, dataLabel->position(), tpen);

    if (fontSize > 0) {
      const_cast<CQChartsScatterPlot *>(plot_)->setDataLabelFont(font);
    }
  }
}

//------

CQChartsScatterCellObj::
CQChartsScatterCellObj(const CQChartsScatterPlot *plot, int groupInd,
                       const CQChartsGeom::BBox &rect, int ig, int ng, int is, int ns,
                       int ix, int iy, const Points &points, int maxn) :
 CQChartsPlotObj(const_cast<CQChartsScatterPlot *>(plot), rect), plot_(plot), groupInd_(groupInd),
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

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor pc = plot_->interpGridCellBorderColor(0, 1);
  QColor fc = plot_->interpPaletteColor(points_.size(), maxn_);

  plot_->setPenBrush(pen, brush,
    plot_->isGridCellBorder(), pc, plot_->gridCellBorderAlpha(),
    plot_->gridCellBorderWidth(), plot_->gridCellBorderDash(),
    /*filled*/true, fc, plot_->gridCellFillAlpha(),
    plot_->gridCellFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw rect
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

  // set symbol type and size
  CQChartsSymbol symbol = plot_->rugSymbolType();

  if (symbol == CQChartsSymbol::Type::NONE)
    symbol = (dir == Dir::X ? CQChartsSymbol::Type::VLINE : CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(plot_->rugSymbolSize(), sx, sy);

  //---

  // draw symbols
  for (const auto &p : points_) {
    if (plot_->isInterrupt())
      return;

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
selectPress(const CQChartsGeom::Point &, CQChartsSelMod selMod)
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  int ih = hideIndex();

  if (selMod == CQChartsSelMod::ADD) {
    for (int i = 0; i < n_; ++i) {
      plot_->CQChartsPlot::setSetHidden(i, i != ih);
    }
  }
  else {
    plot->setSetHidden(ih, ! plot->isSetHidden(ih));
  }

  plot->queueUpdateRangeAndObjs();

  return true;
}

QBrush
CQChartsScatterKeyColor::
fillBrush() const
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  QColor c;

  if (color_.isValid())
    c = plot_->charts()->interpColor(color_, 0, 1);
  else {
    c = plot->interpSymbolFillColor(i_, n_);

    //c = CQChartsKeyColorBox::fillBrush().color();
  }

  c.setAlphaF(plot->symbolFillAlpha());

  int ih = hideIndex();

  if (plot->isSetHidden(ih))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

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
  // calc text width
  plot_->view()->setPlotPainterFont(plot_, painter, key_->textFont());

  QFontMetricsF fm(painter->font());

  double fh = fm.height();

  int n = plot_->gridData().maxN;

  double tw = plot_->pixelToWindowWidth(fm.width(QString("%1").arg(n)) - 3);

  // calc left/right boxes
  CQChartsGeom::BBox lrect(rect.getXMin(), rect.getYMin(), rect.getXMax() - tw, rect.getYMax());
  CQChartsGeom::BBox rrect(rect.getXMax() - tw, rect.getYMin(), rect.getXMax(), rect.getYMax());

  CQChartsGeom::BBox lprect = plot_->windowToPixel(lrect);
  CQChartsGeom::BBox rprect = plot_->windowToPixel(rrect);

  //---

  // draw gradient in left box
  QPointF pg1 = QPointF(lprect.getXMin() + 2, lprect.getYMax() - 2 - fh/2);
  QPointF pg2 = QPointF(lprect.getXMin() + 2, lprect.getYMin() + 2 + fh/2);

  QLinearGradient lg(pg1.x(), pg1.y(), pg2.x(), pg2.y());

  plot_->view()->themePalette()->setLinearGradient(lg, 1.0);

  QBrush brush(lg);

  QRectF frect(pg1.x(), pg2.y(), lprect.getWidth() - 4, lprect.getHeight() - 4 - fh);

  painter->fillRect(frect, brush);

  //---

  // calc label positions
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

  // set text pen
  QPen pen;

  QColor tc = plot_->interpThemeColor(1.0);

  plot_->setPen(pen, true, tc, 1.0);

  painter->setPen(pen);

  //---

  // draw key labels
  double df = (fm.ascent() - fm.descent())/2.0;

  painter->drawText(rprect.getXMin() + 1, y1 + df, QString("%1").arg(n1));
  painter->drawText(rprect.getXMin() + 1, y2 + df, QString("%1").arg(n2));
  painter->drawText(rprect.getXMin() + 1, y3 + df, QString("%1").arg(n3));
  painter->drawText(rprect.getXMin() + 1, y4 + df, QString("%1").arg(n4));
  painter->drawText(rprect.getXMin() + 1, y5 + df, QString("%1").arg(n5));
}
