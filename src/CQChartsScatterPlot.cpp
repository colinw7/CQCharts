#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsColorSet.h>
#include <CQChartsValueSet.h>
#include <CQChartsUtil.h>
#include <CQChartsTip.h>
#include <CQCharts.h>

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

  addColumnParameter("x", "X", "xColumn", 0).setTip("X Value").setRequired();
  addColumnParameter("y", "Y", "yColumn", 1).setTip("Y Value").setRequired();

  addColumnParameter("name", "Name", "nameColumn").setTip("Value Name");

  endParameterGroup();

  //---

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
 CQChartsGroupPlot(view, view->charts()->plotType("scatter"), model), dataLabel_(this)
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

  setRugSymbolType(CQChartsSymbol::Type::NONE);

  setBestFitFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setHullFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

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
setSymbolSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(symbolData_.size, s, [&]() { updateObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolType(const CQChartsSymbol &t)
{
  CQChartsUtil::testAndSet(symbolData_.type, t, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStroked(bool b)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.color, c, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.alpha, a, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.dash, d, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolFilled(bool b)
{
  CQChartsUtil::testAndSet(symbolData_.fill.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(symbolData_.fill.color, c, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolFillAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolData_.fill.alpha, a, [&]() { invalidateLayers(); } );
}

CQChartsScatterPlot::Pattern
CQChartsScatterPlot::
symbolFillPattern() const
{
  return (Pattern) symbolData_.fill.pattern;
}

void
CQChartsScatterPlot::
setSymbolFillPattern(const Pattern &pattern)
{
  if (pattern != (Pattern) symbolData_.fill.pattern) {
    symbolData_.fill.pattern = (CQChartsFillData::Pattern) pattern;

    invalidateLayers();
  }
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
  CQChartsUtil::testAndSet(bestFit_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setBestFitDeviation(bool b)
{
  CQChartsUtil::testAndSet(bestFitDeviation_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setBestFitOrder(int o)
{
  CQChartsUtil::testAndSet(bestFitOrder_, o, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(bestFitFillColor_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitFillAlpha(double a)
{
  CQChartsUtil::testAndSet(bestFitFillAlpha_, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setHull(bool b)
{
  CQChartsUtil::testAndSet(hull_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setHullFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(hullFillColor_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setHullFillAlpha(double a)
{
  CQChartsUtil::testAndSet(hullFillAlpha_, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setXRug(bool b)
{
  CQChartsUtil::testAndSet(xRug_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYRug(bool b)
{
  CQChartsUtil::testAndSet(yRug_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setXRugSide(const YSide &s)
{
  CQChartsUtil::testAndSet(xRugSide_, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYRugSide(const XSide &s)
{
  CQChartsUtil::testAndSet(yRugSide_, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setRugSymbolType(const CQChartsSymbol &s)
{
  CQChartsUtil::testAndSet(rugSymbolType_, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setRugSymbolSize(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(rugSymbolSize_, l, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsScatterPlot::
setXDensity(bool b)
{
  CQChartsUtil::testAndSet(xDensity_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYDensity(bool b)
{
  CQChartsUtil::testAndSet(yDensity_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setXDensitySide(const YSide &s)
{
  CQChartsUtil::testAndSet(xDensitySide_, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYDensitySide(const XSide &s)
{
  CQChartsUtil::testAndSet(yDensitySide_, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setDensityWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(densityWidth_, l, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setDensityAlpha(double a)
{
  CQChartsUtil::testAndSet(densityAlpha_, a, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsScatterPlot::
setXWhisker(bool b)
{
  CQChartsUtil::testAndSet(xWhisker_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYWhisker(bool b)
{
  CQChartsUtil::testAndSet(yWhisker_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setXWhiskerSide(const YSide &s)
{
  CQChartsUtil::testAndSet(xWhiskerSide_, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setYWhiskerSide(const XSide &s)
{
  CQChartsUtil::testAndSet(yWhiskerSide_, s, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setWhiskerWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(whiskerWidth_, l, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setWhiskerMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(whiskerMargin_, l, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setWhiskerAlpha(double a)
{
  CQChartsUtil::testAndSet(whiskerAlpha_, a, [&]() { invalidateLayers(); } );
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
  addProperty("hull"     , this, "hull"         , "enabled");
  addProperty("hull/fill", this, "hullFillColor", "color");
  addProperty("hull/fill", this, "hullFillAlpha", "alpha");

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

  addProperty("color/map", this, "colorMapped", "enabled");
  addProperty("color/map", this, "colorMapMin", "min"    );
  addProperty("color/map", this, "colorMapMax", "max"    );
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
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      CQChartsModelIndex ind(row, plot_->xColumn(), parent);

      // init group
      (void) plot_->rowGroupInd(ind);

      //---

      bool ok1, ok2;

      double x = plot_->modelReal(row, plot_->xColumn(), parent, ok1);
      double y = plot_->modelReal(row, plot_->yColumn(), parent, ok2);

      if (! ok1) x = row;
      if (! ok2) y = row;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
        return State::SKIP;

      range_.updateRange(x, y);

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    CQChartsScatterPlot *plot_ { nullptr };
    CQChartsGeom::Range  range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  dataRange_ = visitor.range();

  //---

  // update data range if unset
  if (dataRange_.isSet() && CQChartsUtil::isZero(dataRange_.xsize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x - 1, y);
    dataRange_.updateRange(x + 1, y);
  }

  if (dataRange_.isSet() && CQChartsUtil::isZero(dataRange_.ysize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x, y - 1);
    dataRange_.updateRange(x, y + 1);
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  bool ok;

  QString xname = (xLabel().length() ? xLabel() : modelHeaderString(xColumn(), ok));
  QString yname = (yLabel().length() ? yLabel() : modelHeaderString(yColumn(), ok));

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);
}

//------

void
CQChartsScatterPlot::
updateObjs()
{
  groupNameValues_.clear();

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

  //double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  //double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    bool hidden = (ng > 1 && isSetHidden(ig));

    if (hidden) { ++ig; continue; }

    //---

    int               groupInd   = groupNameValue.first;
    const NameValues &nameValues = groupNameValue.second;

    Points &points = groupPoints_[groupInd];

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      bool hidden = (ng == 1 && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

      const QString &name   = nameValue.first;
      const Values  &values = nameValue.second;

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

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      CQChartsModelIndex ind(row, plot_->xColumn(), parent);

      // get group
      int groupInd = plot_->rowGroupInd(ind);

      //---

      // get x, y value
      QModelIndex xInd  = plot_->modelIndex(row, plot_->xColumn(), parent);
      QModelIndex xInd1 = plot_->normalizeIndex(xInd);

      bool ok1, ok2;

      double x = plot_->modelReal(row, plot_->xColumn(), parent, ok1);
      double y = plot_->modelReal(row, plot_->yColumn(), parent, ok2);

      if (! ok1) x = row;
      if (! ok2) y = row;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
        return State::SKIP;

      //---

      // get optional name
      QString name;

      if (plot_->nameColumn().isValid()) {
        bool ok;

        name = plot_->modelString(row, plot_->nameColumn(), parent, ok);
      }

      if (! name.length())
        name = plot_->title()->textStr();

      if (! name.length())
        name = plot_->xAxis()->label();

      //---

      // get symbol type, size, font size and color
      QString       symbolTypeStr;
      QString       symbolSizeStr;
      QString       fontSizeStr;
      CQChartsColor color;

      // get symbol type label (needed if not string ?)
      if (plot_->symbolTypeColumn().isValid()) {
        bool ok3;

        symbolTypeStr = plot_->modelString(row, plot_->symbolTypeColumn(), parent, ok3);
      }

      // get symbol size label (needed if not string ?)
      if (plot_->symbolSizeColumn().isValid()) {
        bool ok3;

        symbolSizeStr = plot_->modelString(row, plot_->symbolSizeColumn(), parent, ok3);
      }

      // get font size label (needed if not string ?)
      if (plot_->fontSizeColumn().isValid()) {
        bool ok4;

        fontSizeStr = plot_->modelString(row, plot_->fontSizeColumn(), parent, ok4);
      }

      // get color label (needed if not string ?)
      if (plot_->colorColumn().isValid()) {
        bool ok5;

        color = plot_->modelColor(row, plot_->colorColumn(), parent, ok5);
      }

      //---

      plot_->addNameValue(groupInd, name, x, y, row, xInd1, symbolTypeStr,
                          symbolSizeStr, fontSizeStr, color);

      return State::OK;
    }

   private:
    CQChartsScatterPlot *plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

void
CQChartsScatterPlot::
addNameValue(int groupInd, const QString &name, double x, double y, int row,
             const QModelIndex &xind, const QString &symbolTypeStr, const QString &symbolSizeStr,
             const QString &fontSizeStr, const CQChartsColor &color)
{
  groupNameValues_[groupInd][name].emplace_back(x, y, row, xind, symbolTypeStr, symbolSizeStr,
                                                fontSizeStr, color);
}

void
CQChartsScatterPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int ng = groupNameValues_.size();

  // multiple group - key item per group
  if (ng > 1) {
    int ig = 0;

    for (const auto &groupNameValue : groupNameValues_) {
      int groupInd = groupNameValue.first;

      QString groupName = groupIndName(groupInd);

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, ig, ng);
      CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, groupName);

      key->addItem(colorItem, ig, 0);
      key->addItem(textItem , ig, 1);

      //--

      if (colorColumn().isValid() && colorColumn().isGroup()) {
        CQChartsColorSet *colorSet = getColorSet("color");
        assert(colorSet);

        if (colorSet->isMapped()) {
          double r = CQChartsUtil::map(groupInd, colorSet->imin(), colorSet->imax(),
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
  else {
    const NameValues &nameValues = (*groupNameValues_.begin()).second;

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      const QString &name   = nameValue.first;
      const Values  &values = nameValue.second;

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, is, ns);
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

  key->plot()->updateKeyPosition(/*force*/true);
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

  (void) addCheckedAction("Best Fit" , isBestFit (), SLOT(setBestFit (bool)));
  (void) addCheckedAction("Hull"     , isHull    (), SLOT(setHull    (bool)));
  (void) addCheckedAction("X Rug"    , isXRug    (), SLOT(setXRug    (bool)));
  (void) addCheckedAction("Y Rug"    , isYRug    (), SLOT(setYRug    (bool)));
  (void) addCheckedAction("X Density", isXDensity(), SLOT(setXDensity(bool)));
  (void) addCheckedAction("Y Density", isYDensity(), SLOT(setYDensity(bool)));
  (void) addCheckedAction("X Whisker", isXWhisker(), SLOT(setXWhisker(bool)));
  (void) addCheckedAction("Y Whisker", isYWhisker(), SLOT(setYWhisker(bool)));

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
      double x = (yRugSide() == XSide::LEFT ? dataRange.ymin() - 2*sx :
                                              dataRange.ymax() + 2*sx);

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

void
CQChartsScatterPlot::
drawBackground(QPainter *painter)
{
  CQChartsPlot::drawBackground(painter);

  if (isHull())
    drawHull(painter);

  if (isBestFit())
    drawBestFit(painter);

  if (isXRug    ()) drawXRug    (painter);
  if (isXDensity()) drawXDensity(painter);
  if (isXWhisker()) drawXWhisker(painter);

  if (isYRug    ()) drawYRug    (painter);
  if (isYDensity()) drawYDensity(painter);
  if (isYWhisker()) drawYWhisker(painter);
}

void
CQChartsScatterPlot::
drawForeground(QPainter *painter)
{
  drawSymbolMapKey(painter);

  CQChartsPlot::drawForeground(painter);
}

void
CQChartsScatterPlot::
drawBestFit(QPainter *painter)
{
  for (const auto &groupNameValue : groupNameValues_) {
    int groupInd = groupNameValue.first;

    CQChartsLeastSquaresFit &fitData = groupFitData_[groupInd];

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

    CQChartsLeastSquaresFit &fitData = groupFitData_[groupInd];

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

    fillColor.setAlphaF(bestFitFillAlpha());

    pen.setColor(borderColor);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(fillColor);

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

    QColor borderColor = interpThemeColor(1.0);
    QColor fillColor   = hullFillColor().interpColor(this, ig, ng);

    fillColor.setAlphaF(hullFillAlpha());

    pen.setColor(borderColor);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(fillColor);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    hull.draw(this, painter);

    //---

    ++ig;
  }
}

void
CQChartsScatterPlot::
drawXRug(QPainter *painter)
{
  for (const auto &plotObj : plotObjects()) {
    const CQChartsScatterPointObj *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);

    if (! pointObj)
      continue;

    pointObj->drawDir(painter, CQChartsScatterPointObj::Dir::X, xRugSide() == YSide::TOP);
  }
}

void
CQChartsScatterPlot::
drawYRug(QPainter *painter)
{
  for (const auto &plotObj : plotObjects()) {
    const CQChartsScatterPointObj *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);

    if (! pointObj)
      continue;

    pointObj->drawDir(painter, CQChartsScatterPointObj::Dir::Y, yRugSide() == XSide::RIGHT);
  }
}

void
CQChartsScatterPlot::
drawXDensity(QPainter *painter)
{
  int ig = 0;
  int ng = groupNameValues_.size();

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

    //---

    // calc pen/brush
    QColor penColor   = interpSymbolStrokeColor(ig, ng);
    QColor brushColor = interpSymbolFillColor  (ig, ng);

    penColor  .setAlphaF(symbolStrokeAlpha());
    brushColor.setAlphaF(densityAlpha());

    QPen   pen;
    QBrush brush;

    pen.setColor(penColor);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(brushColor);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    const CQChartsGeom::Range &dataRange = this->dataRange();

    const CQChartsDensity &density = whiskerData.xWhisker.density();

    double xmin = density.xmin1();
    double xmax = density.xmax1();

    double dh = lengthPlotHeight(densityWidth());

    double pos = (xDensitySide() == YSide::BOTTOM ?
      dataRange.ymin() - dh : dataRange.ymax());

    CQChartsGeom::BBox rect(xmin, pos, xmax, pos + dh);

    density.drawWhisker(this, painter, rect, Qt::Horizontal);

    ++ig;
  }
}

void
CQChartsScatterPlot::
drawYDensity(QPainter *painter)
{
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    int groupInd = groupNameValue.first;

    WhiskerData &whiskerData = groupWhiskers_[groupInd];

    // init whisker if needed
    if (! whiskerData.yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd)
          whiskerData.yWhisker.addValue(pointObj->point().y());
      }
    }

    //---

    // calc pen/brush
    QColor penColor   = interpSymbolStrokeColor(ig, ng);
    QColor brushColor = interpSymbolFillColor  (ig, ng);

    penColor  .setAlphaF(symbolStrokeAlpha());
    brushColor.setAlphaF(densityAlpha());

    QPen   pen;
    QBrush brush;

    pen.setColor(penColor);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(brushColor);

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    const CQChartsGeom::Range &dataRange = this->dataRange();

    const CQChartsDensity &density = whiskerData.yWhisker.density();

    double xmin = density.xmin1();
    double xmax = density.xmax1();

    double dw = lengthPlotWidth(densityWidth());

    double pos = (yDensitySide() == XSide::LEFT ?
      dataRange.xmin() - dw : dataRange.xmax());

    CQChartsGeom::BBox rect(pos, xmin, pos + dw, xmax);

    density.drawWhisker(this, painter, rect, Qt::Vertical);

    ++ig;
  }
}

void
CQChartsScatterPlot::
drawXWhisker(QPainter *painter)
{
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    int groupInd = groupNameValue.first;

    WhiskerData &whiskerData = groupWhiskers_[groupInd];

    if (! whiskerData.xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd)
          whiskerData.xWhisker.addValue(pointObj->point().x());
      }
    }

    //---

    // calc pen/brush
    QColor penColor   = interpSymbolStrokeColor(ig, ng);
    QColor brushColor = interpSymbolFillColor  (ig, ng);

    penColor  .setAlphaF(symbolStrokeAlpha());
    brushColor.setAlphaF(whiskerAlpha());

    QPen   pen;
    QBrush brush;

    pen.setColor(penColor);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(brushColor);

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

    ++ig;
  }
}

void
CQChartsScatterPlot::
drawYWhisker(QPainter *painter)
{
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    int groupInd = groupNameValue.first;

    WhiskerData &whiskerData = groupWhiskers_[groupInd];

    if (! whiskerData.yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd)
          whiskerData.yWhisker.addValue(pointObj->point().y());
      }
    }

    //---

    // calc pen/brush
    QColor penColor   = interpSymbolStrokeColor(ig, ng);
    QColor brushColor = interpSymbolFillColor  (ig, ng);

    penColor  .setAlphaF(symbolStrokeAlpha());
    brushColor.setAlphaF(whiskerAlpha());

    QPen   pen;
    QBrush brush;

    pen.setColor(penColor);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(brushColor);

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

    ++ig;
  }
}

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
      min = CQChartsUtil::toReal(minValue, ok);

    if (maxValue.isValid())
      max = CQChartsUtil::toReal(maxValue, ok);

    double px, py;

    double vx = view()->viewportRange();
    double vy = 0.0;

    view()->windowToPixel(vx, vy, px, py);

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

  const CQChartsScatterPlot::Values &values = (*p).second;

  const CQChartsScatterPlot::ValueData &valuePoint = values[iv_];

  if (valuePoint.symbolTypeStr != "")
    tableTip.addTableRow(plot_->symbolTypeName(), valuePoint.symbolTypeStr);

  if (valuePoint.symbolSizeStr != "")
    tableTip.addTableRow(plot_->symbolSizeName(), valuePoint.symbolSizeStr);

  if (valuePoint.fontSizeStr != "")
    tableTip.addTableRow(plot_->fontSizeName(), valuePoint.fontSizeStr);

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

  QColor fc;

  if (color_.isValid())
    fc = color_.interpColor(plot_, ic, nc);
  else
    fc = plot_->interpSymbolFillColor(ic, nc);

  plot_->setPenBrush(pen, brush,
                     plot_->isSymbolStroked(),
                     plot_->interpSymbolStrokeColor(ic, nc),
                     plot_->symbolStrokeAlpha(),
                     plot_->symbolStrokeWidth(),
                     plot_->symbolStrokeDash(),
                     plot_->isSymbolFilled(),
                     fc,
                     plot_->symbolFillAlpha(),
                     (CQChartsFillPattern::Type) plot_->symbolFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  CQChartsSymbol symbol = this->symbolType();

  if (symbol == CQChartsSymbol::Type::NONE)
    symbol = plot_->symbolType();

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize(), sx, sy);

  //---

  QPointF ps = plot_->windowToPixel(p_);

  if (dir != Dir::XY) {
    // Dir::X and Dir::Y are X/Y Rug Symbols
    CQChartsGeom::BBox pbbox = plot_->calcDataPixelRect();

    if      (dir == Dir::X) {
      if (! flip)
        ps.setY(pbbox.getYMax());
      else
        ps.setY(pbbox.getYMin());
    }
    else if (dir == Dir::Y) {
      if (! flip)
        ps.setX(pbbox.getXMin());
      else
        ps.setX(pbbox.getXMax());
    }

    symbol = plot_->rugSymbolType();

    if (symbol == CQChartsSymbol::Type::NONE)
      symbol = (dir == Dir::X ? CQChartsSymbol::Type::VLINE : CQChartsSymbol::Type::HLINE);

    plot_->pixelSymbolSize(plot_->rugSymbolSize(), sx, sy);
  }

  //---

  // draw symbol
  painter->setPen  (pen);
  painter->setBrush(brush);

  QRectF erect(ps.x() - sx, ps.y() - sy, 2*sx, 2*sy);

  plot_->drawSymbol(painter, ps, symbol, CQChartsUtil::avg(sx, sy), pen, brush);

  //---

  // draw text labels
  if (plot_->dataLabel().isVisible()) {
    CQChartsDataLabel &dataLabel = plot_->dataLabel();

    //---

    QColor tc = plot_->dataLabel().interpTextColor(ic, nc);

    tc.setAlphaF(plot_->dataLabel().textAlpha());

    //---

    double fontSize = plot_->dataLabel().textFont().pointSizeF();

    if (fontSize_)
      fontSize = *fontSize_;

    //---

    if (fontSize > 0) {
      fontSize = plot_->limitFontSize(fontSize);

      QFont font = dataLabel.textFont();

      QFont font1 = font;

      font1.setPointSizeF(fontSize);

      dataLabel.setTextFont(font1);

      dataLabel.draw(painter, erect, name_, dataLabel.position(), tc);

      dataLabel.setTextFont(font);
    }
    else {
      dataLabel.draw(painter, erect, name_, dataLabel.position(), tc);
    }
  }
}

//------

CQChartsScatterKeyColor::
CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsScatterKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

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

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
