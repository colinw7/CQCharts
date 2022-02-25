#include <CQChartsPointPlot.h>
#include <CQChartsDataLabel.h>
#include <CQChartsAxisRug.h>
#include <CQChartsAxis.h>
#include <CQChartsMapKey.h>
#include <CQChartsModelDetails.h>
#include <CQChartsVariant.h>
#include <CQChartsFitData.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsTip.h>
#include <CQChartsSymbolSet.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsPointPlotType::
CQChartsPointPlotType() :
 CQChartsGroupPlotType()
{
}

void
CQChartsPointPlotType::
addMappingParameters()
{
  // custom columns/map
  startParameterGroup("Symbols");

  addColumnParameter("symbolType", "Symbol Type", "symbolTypeColumn").
    setTip("Custom Symbol Type").setMapped().
    setMapMinMax(CQChartsSymbolType::minFillValue(), CQChartsSymbolType::maxFillValue());

  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn").
    setTip("Custom Symbol Size").setMapped().
    setMapMinMax(CQChartsSymbolSize::minValue(), CQChartsSymbolSize::maxValue());

  addColumnParameter("fontSize", "Font Size", "fontSizeColumn").
    setTip("Custom Font Size for Label").setMapped().
    setMapMinMax(CQChartsFontSize::minValue(), CQChartsFontSize::maxValue());

  endParameterGroup();
}

//---

CQChartsPointPlot::
CQChartsPointPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsGroupPlot(view, plotType, model),
 CQChartsObjBestFitShapeData<CQChartsPointPlot>(this),
 CQChartsObjHullShapeData   <CQChartsPointPlot>(this),
 CQChartsObjStatsLineData   <CQChartsPointPlot>(this)
{
}

CQChartsPointPlot::
~CQChartsPointPlot()
{
  term();
}

//---

void
CQChartsPointPlot::
init()
{
  CQChartsGroupPlot::init();

  //---

  NoUpdate noUpdate(this);

  // create a data label (shared state for all data labels)
  dataLabel_ = std::make_shared<CQChartsDataLabel>(this);

  dataLabel_->setClip(false);
  dataLabel_->setTextColor(Color::makeInterfaceValue(1.0));
  dataLabel_->setMoveClipped(false);
  dataLabel_->setSendSignal(true);

  connect(dataLabel_.get(), SIGNAL(dataChanged()), this, SLOT(dataLabelChanged()));

  //---

  xRug_ = std::make_unique<CQChartsAxisRug>(this, Qt::Horizontal);
  yRug_ = std::make_unique<CQChartsAxisRug>(this, Qt::Vertical);

  xRug_->setVisible(false);
  yRug_->setVisible(false);

  //---

  // set mapped range
  setSymbolTypeMapped(true);
  setSymbolTypeMapMin(SymbolType::minFillValue());
  setSymbolTypeMapMax(SymbolType::maxFillValue());

  setSymbolSizeMapped(true);
  setSymbolSizeMapMin(CQChartsSymbolSize::minValue());
  setSymbolSizeMapMax(CQChartsSymbolSize::maxValue());

  setFontSizeMapped(true);
  setFontSizeMapMin(CQChartsFontSize::minValue());
  setFontSizeMapMax(CQChartsFontSize::maxValue());

  //---

  // best fit
  setBestFit(false);
  setBestFitStrokeDash(LineDash(LineDash::Lengths({2, 2}), 0));
  setBestFitStrokeWidth(Length::pixel(4));
  setBestFitStrokeColor(Color::makePalette());
  setBestFitFillColor(Color::makePalette());
  setBestFitFillAlpha(Alpha(0.5));

  // hull
  setHullFillColor(Color::makePalette());
  setHullFillAlpha(Alpha(0.5));

  // stats
  setStatsLines(false);
  setStatsLinesDash(LineDash(LineDash::Lengths({2, 2}), 0));
}

void
CQChartsPointPlot::
term()
{
  clearFitData ();
  clearHullData();
}

//---

void
CQChartsPointPlot::
clearFitData()
{
  for (const auto &gfit : groupFitData_)
    delete gfit.second;

  groupFitData_.clear();
}

CQChartsFitData *
CQChartsPointPlot::
getBestFit(int ind, bool &created) const
{
  // get best fit for group or set id
  created = false;

  auto *th = const_cast<CQChartsPointPlot *>(this);

  auto ph = th->groupFitData_.find(ind);

  if (ph == th->groupFitData_.end()) {
    ph = th->groupFitData_.insert(ph, GroupFitData::value_type(ind, new BestFit));

    created = true;
  }

  return (*ph).second;
}

//---

void
CQChartsPointPlot::
clearHullData()
{
  for (const auto &ghull : groupHull_)
    delete ghull.second;

  groupHull_.clear();
}

CQChartsGrahamHull *
CQChartsPointPlot::
getHull(int ind, bool &created) const
{
  // get hull for group or set id
  created = false;

  auto *th = const_cast<CQChartsPointPlot *>(this);

  auto ph = th->groupHull_.find(ind);

  if (ph == th->groupHull_.end()) {
    ph = th->groupHull_.insert(ph, GroupHull::value_type(ind, new Hull));

    created = true;
  }

  return (*ph).second;
}

//---

void
CQChartsPointPlot::
addPointProperties()
{
  // columns
  addProp("columns", "symbolTypeColumn", "symbolType", "Symbol type column");
  addProp("columns", "symbolSizeColumn", "symbolSize", "Symbol size column");
  addProp("columns", "fontSizeColumn"  , "fontSize"  , "Font size column");

  //---

  // mapping for columns
  addProp("mapping/symbolType", "symbolTypeMapped" , "enabled"   , "Symbol type values mapped");
  addProp("mapping/symbolType", "symbolTypeMapMin" , "min"       , "Symbol type map min value");
  addProp("mapping/symbolType", "symbolTypeMapMax" , "max"       , "Symbol type map max value");
  addProp("mapping/symbolType", "symbolTypeSetName", "set"       , "Symbol type set name");
  addProp("mapping/symbolType", "symbolTypeMap"    , "symbol_map", "Symbol type to value map");

  addProp("mapping/symbolSize", "symbolSizeMapped"  , "enabled" , "Symbol size values mapped");
  addProp("mapping/symbolSize", "symbolSizeMapMin"  , "min"     , "Symbol size map min value");
  addProp("mapping/symbolSize", "symbolSizeMapMax"  , "max"     , "Symbol size map max value");
  addProp("mapping/symbolSize", "symbolSizeMapUnits", "units"   , "Symbol size map units");
  addProp("mapping/symbolSize", "symbolSizeMap"     , "size_map", "Symbol size to value map");

  addProp("mapping/fontSize", "fontSizeMapped"  , "enabled", "Font size value mapped");
  addProp("mapping/fontSize", "fontSizeMapMin"  , "min"    , "Font size map min value");
  addProp("mapping/fontSize", "fontSizeMapMax"  , "max"    , "Font size map max value");
  addProp("mapping/fontSize", "fontSizeMapUnits", "units"  , "Font size map units");
}

void
CQChartsPointPlot::
addBestFitProperties(bool hasLayer)
{
  auto propPath = QString("overlays/bestFit");

  // best fit line and deviation fill
  addProp(propPath, "bestFit"         , "visible"  , "Show best fit overlay");
  addProp(propPath, "bestFitOutliers" , "outliers" , "Best fit include outliers");
  addProp(propPath, "bestFitOrder"    , "order"    , "Best fit curve order");
  addProp(propPath, "bestFitDeviation", "deviation", "Best fit standard deviation");

  if (hasLayer)
    addProp(propPath, "bestFitLayer", "layer", "Best fit draw layer");

  addFillProperties(propPath + "/fill"  , "bestFitFill"  , "Best fit");
  addLineProperties(propPath + "/stroke", "bestFitStroke", "Best fit");
}

void
CQChartsPointPlot::
addHullProperties(bool hasLayer)
{
  auto propPath = QString("overlays/hull");

  // convex hull shape
  addProp(propPath, "hull", "visible", "Show convex hull overlay");

  if (hasLayer)
    addProp(propPath, "hullLayer", "layer", "Convex hull draw layer");

  addFillProperties(propPath + "/fill"  , "hullFill"  , "Convex hull");
  addLineProperties(propPath + "/stroke", "hullStroke", "Convex hull");
}

void
CQChartsPointPlot::
addStatsProperties()
{
  auto propPath = QString("overlays/statsData");

  // stats
  addProp(propPath, "statsLines", "visible", "Statistic lines visible");

  addLineProperties(propPath + "/stroke", "statsLines", "Statistic lines");
}

void
CQChartsPointPlot::
addRugProperties(const QString &path)
{
  xRug_->addProperties(path + "/rug/x", "X Rug");
  yRug_->addProperties(path + "/rug/y", "Y Rug");
}

void
CQChartsPointPlot::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQChartsPlot::getPropertyNames(names, hidden);

  propertyModel()->objectNames(dataLabel(), names, hidden);
}

//---

void
CQChartsPointPlot::
setSymbolTypeColumn(const Column &c)
{
  CQChartsUtil::testAndSet(symbolTypeData_.column, c, [&]() {
    symbolTypeData_.valid = false; updateRangeAndObjs(); emit symbolTypeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolTypeMapped(bool b)
{
  CQChartsUtil::testAndSet(symbolTypeData_.mapped, b, [&]() {
    symbolTypeData_.valid = false; updateRangeAndObjs(); emit symbolTypeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolTypeMapMin(long i)
{
  CQChartsUtil::testAndSet(symbolTypeData_.map_min, i, [&]() {
    symbolTypeData_.valid = false; updateRangeAndObjs(); emit symbolTypeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolTypeMapMax(long i)
{
  CQChartsUtil::testAndSet(symbolTypeData_.map_max, i, [&]() {
    symbolTypeData_.valid = false; updateRangeAndObjs(); emit symbolTypeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolTypeSetName(const QString &name)
{
  CQChartsUtil::testAndSet(symbolTypeData_.setName, name, [&]() {
    auto *symbolSetMgr = charts()->symbolSetMgr();
    auto *symbolSet    = symbolSetMgr->symbolSet(name);

    if (symbolSet) {
      setSymbolTypeMapMin(0);
      setSymbolTypeMapMax(symbolSet->numSymbols() - 1);
    }
    else {
      setSymbolTypeMapMin(SymbolType::minFillValue());
      setSymbolTypeMapMax(SymbolType::maxFillValue());
    }

    updateObjs();

    emit symbolTypeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolTypeMap(const CQChartsSymbolTypeMap &typeMap)
{
  CQChartsUtil::testAndSet(symbolTypeData_.typeMap, typeMap, [&]() {
    symbolTypeData_.valid = false; updateRangeAndObjs(); emit symbolTypeDetailsChanged();
  } );
}

//---

void
CQChartsPointPlot::
setSymbolSizeColumn(const Column &c)
{
  CQChartsUtil::testAndSet(symbolSizeData_.column, c, [&]() {
    symbolSizeData_.valid = false; updateRangeAndObjs(); emit symbolSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolSizeMapped(bool b)
{
  CQChartsUtil::testAndSet(symbolSizeData_.mapped, b, [&]() {
    symbolSizeData_.valid = false; updateRangeAndObjs(); emit symbolSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolSizeMapMin(double r)
{
  CQChartsUtil::testAndSet(symbolSizeData_.map_min, r, [&]() {
    symbolSizeData_.user_map_min = symbolSizeData_.map_min;
    symbolSizeData_.valid = false; updateRangeAndObjs(); emit symbolSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolSizeMapMax(double r)
{
  CQChartsUtil::testAndSet(symbolSizeData_.map_max, r, [&]() {
    symbolSizeData_.user_map_max = symbolSizeData_.map_max;
    symbolSizeData_.valid = false; updateRangeAndObjs(); emit symbolSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolSizeUserMapMin(double r)
{
  CQChartsUtil::testAndSet(symbolSizeData_.user_map_min, r, [&]() {
    symbolSizeData_.valid = false; updateRangeAndObjs(); emit symbolSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolSizeUserMapMax(double r)
{
  CQChartsUtil::testAndSet(symbolSizeData_.user_map_max, r, [&]() {
    symbolSizeData_.valid = false; updateRangeAndObjs(); emit symbolSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolSizeMapUnits(const CQChartsUnits &u)
{
  CQChartsUtil::testAndSet(symbolSizeData_.units, u, [&]() {
    symbolSizeData_.valid = false; updateRangeAndObjs(); emit symbolSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setSymbolSizeMap(const CQChartsSymbolSizeMap &sizeMap)
{
  CQChartsUtil::testAndSet(symbolSizeData_.sizeMap, sizeMap, [&]() {
    symbolSizeData_.valid = false; updateRangeAndObjs(); emit symbolSizeDetailsChanged();
  } );
}

//---

void
CQChartsPointPlot::
setFontSizeColumn(const Column &c)
{
  CQChartsUtil::testAndSet(fontSizeData_.column, c, [&]() {
    updateRangeAndObjs(); emit fontSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setFontSizeMapped(bool b)
{
  CQChartsUtil::testAndSet(fontSizeData_.mapped, b, [&]() {
    updateRangeAndObjs(); emit fontSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setFontSizeMapMin(double r)
{
  CQChartsUtil::testAndSet(fontSizeData_.map_min, r, [&]() {
    fontSizeData_.user_map_min = fontSizeData_.map_min;
    updateRangeAndObjs(); emit fontSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setFontSizeMapMax(double r)
{
  CQChartsUtil::testAndSet(fontSizeData_.map_max, r, [&]() {
    fontSizeData_.user_map_max = fontSizeData_.map_max;
    updateRangeAndObjs(); emit fontSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setFontSizeUserMapMin(double r)
{
  CQChartsUtil::testAndSet(fontSizeData_.user_map_min, r, [&]() {
    updateRangeAndObjs(); emit fontSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setFontSizeUserMapMax(double r)
{
  CQChartsUtil::testAndSet(fontSizeData_.user_map_max, r, [&]() {
    updateRangeAndObjs(); emit fontSizeDetailsChanged();
  } );
}

void
CQChartsPointPlot::
setFontSizeMapUnits(const CQChartsUnits &u)
{
  CQChartsUtil::testAndSet(fontSizeData_.units, u, [&]() {
    updateRangeAndObjs(); emit fontSizeDetailsChanged();
  } );
}

//---

void
CQChartsPointPlot::
initSymbolTypeData() const
{
  auto *th = const_cast<CQChartsPointPlot *>(this);

  CQChartsPlot::initSymbolTypeData(th->symbolTypeData_);
}

bool
CQChartsPointPlot::
columnSymbolType(int row, const QModelIndex &parent, CQChartsSymbol &symbolType) const
{
  return CQChartsPlot::columnSymbolType(row, parent, symbolTypeData_, symbolType);
}

//------

void
CQChartsPointPlot::
initSymbolSizeData() const
{
  auto *th = const_cast<CQChartsPointPlot *>(this);

  CQChartsPlot::initSymbolSizeData(th->symbolSizeData_);
}

bool
CQChartsPointPlot::
columnSymbolSize(int row, const QModelIndex &parent, Length &symbolSize,
                 Qt::Orientation &sizeDir) const
{
  return CQChartsPlot::columnSymbolSize(row, parent, symbolSizeData_, symbolSize, sizeDir);
}

//------

void
CQChartsPointPlot::
initFontSizeData() const
{
  auto *th = const_cast<CQChartsPointPlot *>(this);

  CQChartsPlot::initFontSizeData(th->fontSizeData_);
}

bool
CQChartsPointPlot::
columnFontSize(int row, const QModelIndex &parent, Length &fontSize,
               Qt::Orientation &sizeDir) const
{
  return CQChartsPlot::columnFontSize(row, parent, fontSizeData_, fontSize, sizeDir);
}

//---

bool
CQChartsPointPlot::
isPointLabels() const
{
  return dataLabel()->isVisible();
}

void
CQChartsPointPlot::
setPointLabels(bool b)
{
  if (b != isPointLabels()) {
    dataLabel()->setVisible(b); updateRangeAndObjs();
  }
}

CQChartsLabelPosition
CQChartsPointPlot::
dataLabelPosition() const
{
  return static_cast<CQChartsLabelPosition>(dataLabel()->position());
}

void
CQChartsPointPlot::
setDataLabelPosition(CQChartsLabelPosition position)
{
  dataLabel()->setPosition(static_cast<CQChartsDataLabel::Position>(position));
}

CQChartsFont
CQChartsPointPlot::
dataLabelFont() const
{
  return dataLabel()->textFont();
}

void
CQChartsPointPlot::
setDataLabelFont(const CQChartsFont &font, bool notify)
{
  auto *dataLabel = this->dataLabel();

  if (! notify) {
    NoUpdate noUpdate(this);

    CQChartsWidgetUtil::AutoDisconnect dataChangeDisconnect(
      dataLabel, SIGNAL(dataChanged()), this, SLOT(dataLabelChanged()));

    dataLabel->setTextFont(font);
  }
  else
    dataLabel->setTextFont(font);
}

CQChartsLength
CQChartsPointPlot::
dataLabelFontSize() const
{
  double dataLabelFontSize = dataLabelFont().pointSizeF();

  return Length::pixel(dataLabelFontSize);
}

void
CQChartsPointPlot::
setDataLabelFontSize(const Length &length)
{
  auto f = dataLabelFont();

  f.setPointSizeF(lengthPixelHeight(length));

  setDataLabelFont(f);
}

//---

void
CQChartsPointPlot::
dataLabelChanged()
{
  // TODO: not enough info to optimize behavior so reload all objects
  updateObjs();

  customDataChanged();
}

//---

void
CQChartsPointPlot::
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.visible, b, [&]() {
    resetBestFit(); updateObjs(); emit customDataChanged();
  } );
}

void
CQChartsPointPlot::
setBestFitOutliers(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.includeOutliers, b, [&]() { resetBestFit(); drawObjs(); } );
}

void
CQChartsPointPlot::
setBestFitDeviation(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.showDeviation, b, [&]() { resetBestFit(); drawObjs(); } );
}

void
CQChartsPointPlot::
setBestFitOrder(int o)
{
  CQChartsUtil::testAndSet(bestFitData_.order, o, [&]() { resetBestFit(); drawObjs(); } );
}

void
CQChartsPointPlot::
setBestFitLayer(const DrawLayer &l)
{
  CQChartsUtil::testAndSet(bestFitData_.layer, l, [&]() {
    for (const auto &plotObj : plotObjs_) {
      auto *bestFitObj = dynamic_cast<CQChartsPointBestFitObj *>(plotObj);

      if (bestFitObj)
        bestFitObj->setDrawLayer(static_cast<CQChartsPlotObj::DrawLayer>(bestFitLayer()));
    }

    drawObjs();
  } );
}

//---

void
CQChartsPointPlot::
setHull(bool b)
{
  CQChartsUtil::testAndSet(hullData_.visible, b, [&]() {
    updateObjs(); emit customDataChanged();
  } );
}

void
CQChartsPointPlot::
setHullLayer(const DrawLayer &l)
{
  CQChartsUtil::testAndSet(hullData_.layer, l, [&]() {
    for (const auto &plotObj : plotObjs_) {
      auto *hullObj = dynamic_cast<CQChartsPointHullObj *>(plotObj);

      if (hullObj)
        hullObj->setDrawLayer(static_cast<CQChartsPlotObj::DrawLayer>(hullLayer()));
    }

    drawObjs();
  } );
}

//---

CQChartsGeom::BBox
CQChartsPointPlot::
fitBBox() const
{
  auto bbox = CQChartsPlot::fitBBox();

  if (symbolTypeMapKey_ && symbolTypeMapKey_->isVisible())
    bbox += symbolTypeMapKey_->bbox();

  if (symbolSizeMapKey_ && symbolSizeMapKey_->isVisible())
    bbox += symbolSizeMapKey_->bbox();

  return bbox;
}

//---

CQChartsPointBestFitObj *
CQChartsPointPlot::
createBestFitObj(int groupInd, const QString &name, const ColorInd &ig, const ColorInd &is,
                 const BBox &rect) const
{
  return new CQChartsPointBestFitObj(this, groupInd, name, ig, is, rect);
}

void
CQChartsPointPlot::
drawBestFit(PaintDevice *device, const BestFit *fitData, const ColorInd &ic) const
{
  // calc fit shape at each pixel
  Polygon bpoly, poly, tpoly;

  auto pl = windowToPixel(Point(fitData->xmin(), 0));
  auto pr = windowToPixel(Point(fitData->xmax(), 0));

  double dx = std::max((pr.x - pl.x)/100, 1.0);

  for (double px = pl.x; px <= pr.x; px += dx) {
    if (isInterrupt())
      return;

    auto p1 = pixelToWindow(Point(px, 0.0));

    double y2 = fitData->interp(p1.x);

    auto p2 = Point(p1.x, y2);

    poly.addPoint(p2);

    // deviation curve above/below
    if (isBestFitDeviation()) {
      p2 = Point(p1.x, y2 - fitData->deviation());

      bpoly.addPoint(p2);

      p2 = Point(p1.x, y2 + fitData->deviation());

      tpoly.addPoint(p2);
    }
  }

  //---

  if (poly.size()) {
    // calc pen and brush
    PenBrush penBrush;

    auto strokeColor = interpBestFitStrokeColor(ic);
    auto fillColor   = interpBestFitFillColor  (ic);

    setPenBrush(penBrush, bestFitPenData(strokeColor), bestFitBrushData(fillColor));

    updateObjPenBrushState(this, ic, penBrush, CQChartsPlot::DrawType::LINE);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    // draw fit deviation shape
    if (isBestFitDeviation()) {
      Polygon dpoly;

      for (int i = 0; i < bpoly.size(); ++i) {
        if (isInterrupt())
          return;

        auto p = bpoly.point(i);

        dpoly.addPoint(p);
      }

      for (int i = tpoly.size() - 1; i >= 0; --i) {
        if (isInterrupt())
          return;

        auto p = tpoly.point(i);

        dpoly.addPoint(p);
      }

      device->drawPolygon(dpoly);
    }

    //---

    // draw fit line
    auto path = CQChartsDrawUtil::polygonToPath(poly, /*closed*/false);

    device->strokePath(path, penBrush.pen);
  }
}

void
CQChartsPointPlot::
initGroupBestFit(BestFit *fitData, int ind, const QVariant &var, bool isGroup) const
{
  // init best fit data
  if (! fitData->isFitted()) {
    auto points = indPoints(var, isGroup);

    if (! isBestFitOutliers()) {
      initGroupStats(ind, var, isGroup);

      //---

      auto ps = groupStatData_.find(ind);
      assert(ps != groupStatData_.end());

      const auto &statData = (*ps).second;

      //---

      Polygon poly;

      for (const auto &p : points) {
        if (! statData.xstat.isOutlier(p.x) && ! statData.ystat.isOutlier(p.y))
          poly.addPoint(p);
      }

      //---

      fitData->calc(poly, bestFitOrder());
    }
    else {
      fitData->calc(points, bestFitOrder());
    }
  }
}

void
CQChartsPointPlot::
initGroupStats(int ind, const QVariant &var, bool isGroup) const
{
  // init stats data
  auto *th = const_cast<CQChartsPointPlot *>(this);

  auto &statData = th->groupStatData_[ind];

  if (! statData.xstat.set || ! statData.ystat.set) {
    auto points = indPoints(var, isGroup);

    std::vector<double> x, y;

    for (std::size_t i = 0; i < points.size(); ++i) {
      x.push_back(points[i].x);
      y.push_back(points[i].y);
    }

    std::sort(x.begin(), x.end());
    std::sort(y.begin(), y.end());

    statData.xstat.calcStatValues(x);
    statData.ystat.calcStatValues(y);
  }
}

//---

CQChartsPointHullObj *
CQChartsPointPlot::
createHullObj(int groupInd, const QString &name, const ColorInd &ig, const ColorInd &is,
              const BBox &rect) const
{
  return new CQChartsPointHullObj(this, groupInd, name, ig, is, rect);
}

CQChartsPointPlot::Points
CQChartsPointPlot::
indPoints(const QVariant &var, int isGroup) const
{
  Points points;

  if (isGroup) {
    bool ok;
    int groupInd = int(CQChartsVariant::toInt(var, ok));

    auto p = groupPoints_.find(groupInd);
    if (p == groupPoints_.end()) return points;

    points = (*p).second;
  }
  else {
    const auto &nameValues = (*groupNameValues_.begin()).second;

    auto pn = nameValues.find(var.toString());
    if (pn == nameValues.end()) return points;

    const auto &values = (*pn).second.values;

    for (const auto &v : values)
      points.push_back(v.p);
  }

  return points;
}

//---

void
CQChartsPointPlot::
setStatsLinesSlot(bool b)
{
  if (b != isStatsLines()) {
    setStatsLines(b);

    drawObjs();
  }
}

//---

bool
CQChartsPointPlot::
isXRug() const
{
  return (xRug_ && xRug_->isVisible());
}

void
CQChartsPointPlot::
setXRug(bool b)
{
  if (b != isXRug()) { xRug_->setVisible(b); drawObjs(); }
}

const CQChartsAxisRug::Side &
CQChartsPointPlot::
xRugSide() const
{
  return xRug_->side();
}

void
CQChartsPointPlot::
setXRugSide(const CQChartsAxisRug::Side &s)
{
  if (s != xRugSide()) { xRug_->setSide(s); drawObjs(); }
}

const CQChartsSymbol &
CQChartsPointPlot::
xRugSymbol() const
{
  return xRug_->symbol();
}

void
CQChartsPointPlot::
setXRugSymbol(const CQChartsSymbol &s)
{
  if (s != xRugSymbol()) { xRug_->setSymbol(s); drawObjs(); }
}

const CQChartsLength &
CQChartsPointPlot::
xRugSymbolSize() const
{
  return xRug_->symbolSize();
}

void
CQChartsPointPlot::
setXRugSymbolSize(const CQChartsLength &l)
{
  if (l != xRugSymbolSize()) { xRug_->setSymbolSize(l); drawObjs(); }
}

//---

bool
CQChartsPointPlot::
isYRug() const
{
  return (yRug_ && yRug_->isVisible());
}

void
CQChartsPointPlot::
setYRug(bool b)
{
  if (b != isYRug()) { yRug_->setVisible(b); drawObjs(); }
}

const CQChartsAxisRug::Side &
CQChartsPointPlot::
yRugSide() const
{
  return yRug_->side();
}

void
CQChartsPointPlot::
setYRugSide(const CQChartsAxisRug::Side &s)
{
  if (s != yRugSide()) { yRug_->setSide(s); drawObjs(); }
}

const CQChartsSymbol &
CQChartsPointPlot::
yRugSymbol() const
{
  return yRug_->symbol();
}

void
CQChartsPointPlot::
setYRugSymbol(const CQChartsSymbol &s)
{
  if (s != yRugSymbol()) { yRug_->setSymbol(s); drawObjs(); }
}

const CQChartsLength &
CQChartsPointPlot::
yRugSymbolSize() const
{
  return yRug_->symbolSize();
}

void
CQChartsPointPlot::
setYRugSymbolSize(const CQChartsLength &l)
{
  if (l != yRugSymbolSize()) { yRug_->setSymbolSize(l); drawObjs(); }
}

//---

void
CQChartsPointPlot::
addSymbolSizeMapKey()
{
  assert(! symbolSizeMapKey_);

  symbolSizeMapKey_ = std::make_unique<CQChartsSymbolSizeMapKey>(this);

  symbolSizeMapKey_->setVisible(false);
  symbolSizeMapKey_->setAlign(Qt::AlignHCenter | Qt::AlignBottom);

  connect(symbolSizeMapKey_.get(), SIGNAL(dataChanged()), this, SLOT(updateSlot()));
  connect(symbolSizeMapKey_.get(), SIGNAL(itemSelected(const CQChartsLength &, bool)),
          this, SLOT(symbolSizeSelected(const CQChartsLength &, bool)));

  mapKeys_.push_back(symbolSizeMapKey_.get());

  //---

  registerSlot("show_symbol_size_key", QStringList() << "bool");
}

bool
CQChartsPointPlot::
isSymbolSizeMapKey() const
{
  return symbolSizeMapKey_->isVisible();
}

void
CQChartsPointPlot::
setSymbolSizeMapKey(bool b)
{
  if (b != symbolSizeMapKey_->isVisible()) {
    symbolSizeMapKey_->setVisible(b);

    drawObjs();
  }
}

const CQChartsAlpha &
CQChartsPointPlot::
symbolSizeMapKeyAlpha() const
{
  return symbolSizeMapKey_->alpha();
}

void
CQChartsPointPlot::
setSymbolSizeMapKeyAlpha(const Alpha &a)
{
  if (a != symbolSizeMapKey_->alpha()) {
    symbolSizeMapKey_->setAlpha(a);

    drawObjs();
  }
}

double
CQChartsPointPlot::
symbolSizeMapKeyMargin() const
{
  return symbolSizeMapKey_->margin();
}

void
CQChartsPointPlot::
setSymbolSizeMapKeyMargin(double m)
{
  if (m != symbolSizeMapKey_->margin()) {
    symbolSizeMapKey_->setMargin(m);

    drawObjs();
  }
}

void
CQChartsPointPlot::
addSymbolSizeMapKeyProperties()
{
  auto symbolSizeMapKeyPath = QString("mapKeys/symbolSize");

  addProp(symbolSizeMapKeyPath, "symbolSizeMapKey", "visible", "Symbol size key visible");

#if 0
  addProp     (symbolSizeMapKeyPath          , "symbolSizeMapKeyMargin", "margin" ,
               "Symbol size key margin in pixels")->setMinValue(0.0);
  addStyleProp(symbolSizeMapKeyPath + "/fill", "symbolSizeMapKeyAlpha" , "alpha"  ,
               "Symbol size key fill alpha");
#endif

  symbolSizeMapKey_->addProperties(propertyModel(), symbolSizeMapKeyPath);
}

bool
CQChartsPointPlot::
canDrawSymbolSizeMapKey() const
{
  return (symbolSizeMapKey_ && symbolSizeColumn().isValid());
}

void
CQChartsPointPlot::
drawSymbolSizeMapKey(PaintDevice *device) const
{
  if (! symbolSizeColumn().isValid())
    return;

  //---

  updateSymbolSizeMapKey();

  symbolSizeMapKey_->draw(device);

  //---

  auto *th = const_cast<CQChartsPointPlot *>(this);

  auto symbolSizeMapKeyPath = QString("mapKeys.symbolSize");

  symbolSizeMapKey_->updateProperties(th->propertyModel(), symbolSizeMapKeyPath);
}

void
CQChartsPointPlot::
updateSymbolSizeMapKey() const
{
  bool         isNumeric    = false;
  bool         isIntegral   = false;
  bool         isSymbolSize = false;
  bool         isMapped     = false;
  int          numUnique    = 0;
  QVariantList uniqueValues;
  QVariantList uniqueCounts;

  auto *columnDetails = this->columnDetails(symbolSizeColumn());

  if (columnDetails) {
    if      (columnDetails->type() == CQBaseModelType::REAL ||
             columnDetails->type() == CQBaseModelType::INTEGER) {
      isNumeric  = true;
      isIntegral = (columnDetails->type() == CQBaseModelType::INTEGER);
    }
    else if (columnDetails->type() == CQBaseModelType::SYMBOL_SIZE)
      isSymbolSize = true;
    else if (columnDetails->type() == CQBaseModelType::STRING)
      isMapped = symbolTypeData_.mapped;

    if (! isNumeric && ! isSymbolSize) {
      numUnique    = columnDetails->numUnique();
      uniqueValues = columnDetails->uniqueValues();
      uniqueCounts = columnDetails->uniqueCounts();
    }
  }

  //---

  auto *th = const_cast<CQChartsPointPlot *>(this);

  CQChartsWidgetUtil::AutoDisconnect autoDisconnect(symbolSizeMapKey_.get(),
    SIGNAL(dataChanged()), th, SLOT(updateSlot()));

  symbolSizeMapKey_->setData(symbolSizeData());

  symbolSizeMapKey_->setPaletteName  (colorMapPalette());
  symbolSizeMapKey_->setPaletteMinMax(RMinMax(colorMapMin(), colorMapMax()));

  symbolSizeMapKey_->setNumeric     (isNumeric);
  symbolSizeMapKey_->setIntegral    (isIntegral);
  symbolSizeMapKey_->setNative      (isSymbolSize);
  symbolTypeMapKey_->setMapped      (isMapped);
  symbolSizeMapKey_->setNumUnique   (numUnique);
  symbolSizeMapKey_->setUniqueValues(uniqueValues);
  symbolSizeMapKey_->setUniqueCounts(uniqueCounts);
}

void
CQChartsPointPlot::
symbolSizeSelected(const Length &size, bool visible)
{
  auto psize = lengthPixelWidth(size);

  if (! visible)
    symbolSizeFilter_.insert(CQChartsGeom::Real(psize));
  else
    symbolSizeFilter_.erase(CQChartsGeom::Real(psize));

  updateRangeAndObjs();
}

bool
CQChartsPointPlot::
symbolSizeVisible(const Length &size) const
{
  auto psize = lengthPixelWidth(size);

  auto p = symbolSizeFilter_.find(Real(psize));

  return (p == symbolSizeFilter_.end());
}

//---

void
CQChartsPointPlot::
addSymbolTypeMapKey()
{
  assert(! symbolTypeMapKey_);

  symbolTypeMapKey_ = std::make_unique<CQChartsSymbolTypeMapKey>(this);

  symbolTypeMapKey_->setVisible(false);
  symbolTypeMapKey_->setAlign(Qt::AlignRight | Qt::AlignBottom);

  connect(symbolTypeMapKey_.get(), SIGNAL(dataChanged()), this, SLOT(updateSlot()));
  connect(symbolTypeMapKey_.get(), SIGNAL(itemSelected(const CQChartsSymbol &, bool)),
          this, SLOT(symbolTypeSelected(const CQChartsSymbol &, bool)));

  mapKeys_.push_back(symbolTypeMapKey_.get());

  //---

  registerSlot("show_symbol_type_key", QStringList() << "bool");
}

bool
CQChartsPointPlot::
isSymbolTypeMapKey() const
{
  assert(symbolTypeMapKey_);

  return symbolTypeMapKey_->isVisible();
}

void
CQChartsPointPlot::
setSymbolTypeMapKey(bool b)
{
  assert(symbolTypeMapKey_);

  if (b != symbolTypeMapKey_->isVisible()) {
    symbolTypeMapKey_->setVisible(b);

    drawObjs();
  }
}

void
CQChartsPointPlot::
addSymbolTypeMapKeyProperties()
{
  auto symbolTypeMapKeyPath = QString("mapKeys/symbolType");

  addProp(symbolTypeMapKeyPath, "symbolTypeMapKey", "visible", "Symbol type key visible");

  symbolTypeMapKey_->addProperties(propertyModel(), symbolTypeMapKeyPath);
}

bool
CQChartsPointPlot::
canDrawSymbolTypeMapKey() const
{
  return (symbolTypeMapKey_ && symbolTypeColumn().isValid());
}

void
CQChartsPointPlot::
drawSymbolTypeMapKey(PaintDevice *device) const
{
  if (! symbolTypeColumn().isValid())
    return;

  //---

  updateSymbolTypeMapKey();

  symbolTypeMapKey_->draw(device);

  //---

  auto *th = const_cast<CQChartsPointPlot *>(this);

  auto symbolTypeMapKeyPath = QString("mapKeys.symbolType");

  symbolTypeMapKey_->updateProperties(th->propertyModel(), symbolTypeMapKeyPath);
}

void
CQChartsPointPlot::
updateSymbolTypeMapKey() const
{
  bool         isNumeric    = false;
  bool         isIntegral   = false;
  bool         isSymbolType = false;
  bool         isMapped     = false;
  int          numUnique    = 0;
  QVariantList uniqueValues;
  QVariantList uniqueCounts;

  auto *columnDetails = this->columnDetails(symbolTypeColumn());

  if (columnDetails) {
    if      (columnDetails->type() == CQBaseModelType::REAL ||
             columnDetails->type() == CQBaseModelType::INTEGER) {
      isNumeric  = true;
      isIntegral = (columnDetails->type() == CQBaseModelType::INTEGER);
    }
    else if (columnDetails->type() == CQBaseModelType::SYMBOL)
      isSymbolType = true;
    else if (columnDetails->type() == CQBaseModelType::STRING)
      isMapped = symbolTypeData_.mapped;

    if (! isNumeric && ! isSymbolType) {
      numUnique    = columnDetails->numUnique();
      uniqueValues = columnDetails->uniqueValues();
      uniqueCounts = columnDetails->uniqueCounts();
    }
  }

  //---

  auto *th = const_cast<CQChartsPointPlot *>(this);

  CQChartsWidgetUtil::AutoDisconnect autoDisconnect(symbolTypeMapKey_.get(),
    SIGNAL(dataChanged()), th, SLOT(updateSlot()));

  symbolTypeMapKey_->setData(symbolTypeData());

  symbolTypeMapKey_->setNumeric     (isNumeric);
  symbolTypeMapKey_->setIntegral    (isIntegral);
  symbolTypeMapKey_->setNative      (isSymbolType);
  symbolTypeMapKey_->setMapped      (isMapped);
  symbolTypeMapKey_->setNumUnique   (numUnique);
  symbolTypeMapKey_->setUniqueValues(uniqueValues);
  symbolTypeMapKey_->setUniqueCounts(uniqueCounts);
}

void
CQChartsPointPlot::
symbolTypeSelected(const Symbol &symbol, bool visible)
{
  if (! visible)
    symbolTypeFilter_.insert(symbol);
  else
    symbolTypeFilter_.erase(symbol);

  updateRangeAndObjs();
}

bool
CQChartsPointPlot::
symbolTypeVisible(const Symbol &symbol) const
{
  auto p = symbolTypeFilter_.find(symbol);

  return (p == symbolTypeFilter_.end());
}

//---

void
CQChartsPointPlot::
updateMapKey(CQChartsMapKey *key) const
{
  if      (key == symbolTypeMapKey())
    updateSymbolTypeMapKey();
  else if (key == symbolSizeMapKey())
    updateSymbolSizeMapKey();
  else
    CQChartsPlot::updateMapKey(key);
}

//---

void
CQChartsPointPlot::
setMinSymbolSize(const Length &l)
{
  CQChartsUtil::testAndSet(minSymbolSize_, l, [&]() { drawObjs(); } );
}

void
CQChartsPointPlot::
setMinLabelSize(const Length &l)
{
  CQChartsUtil::testAndSet(minLabelSize_, l, [&]() { drawObjs(); } );
}

//---

void
CQChartsPointPlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPlot::write(os, plotVarName, modelVarName, viewVarName);

  dataLabel_->write(os, plotVarName);
}

//---

bool
CQChartsPointPlot::
executeSlotFn(const QString &name, const QVariantList &args, QVariant &res)
{
  bool ok;

  if      (name == "show_symbol_type_key")
    setSymbolTypeMapKey(CQChartsVariant::toBool(args[0], ok));
  else if (name == "show_symbol_size_key")
    setSymbolSizeMapKey(CQChartsVariant::toBool(args[0], ok));
  else
    return CQChartsPlot::executeSlotFn(name, args, res);

  return true;
}

//------

CQChartsPointBestFitObj::
CQChartsPointBestFitObj(const CQChartsPointPlot *plot, int groupInd, const QString &name,
                        const ColorInd &ig, const ColorInd &is, const BBox &rect) :
 CQChartsPlotObj(const_cast<CQChartsPointPlot *>(plot), rect, is,
                 ig, ColorInd()), plot_(plot), groupInd_(groupInd), name_(name)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsPointBestFitObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig().i).arg(is().i);
}

QString
CQChartsPointBestFitObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  auto *bestFit = getBestFit();

  auto dev = bestFit->deviation();

  QString groupName;

  if (name_ == "")
    groupName = plot_->groupIndName(groupInd_);
  else {
    ColorInd ind;

    groupName = plot_->singleGroupName(ind);

    if (groupName == "")
      groupName = name_;
  }

  tableTip.addBoldLine("Best Fit");
  tableTip.addTableRow("Group"    , groupName);
  tableTip.addTableRow("Deviation", dev);

  return tableTip.str();
}

void
CQChartsPointBestFitObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsPlotObj::addProperties(model, path);
}

void
CQChartsPointBestFitObj::
draw(PaintDevice *device) const
{
  auto *bestFit = getBestFit();

  if (name_ == "")
    plot_->drawBestFit(device, bestFit, ig_);
  else
    plot_->drawBestFit(device, bestFit, is_);
}

CQChartsFitData *
CQChartsPointBestFitObj::
getBestFit() const
{
  bool created;

  BestFit *fitData = nullptr;

  if (name_ == "") {
    // get fit data for group (add if needed)
    fitData = plot_->getBestFit(groupInd_, created);

    if (created)
      plot_->initGroupBestFit(fitData, ig_.i, QVariant(groupInd_), /*isGroup*/true);
  }
  else {
    // get fit data for set (add if needed)
    fitData = plot_->getBestFit(is_.i, created);

    if (created)
      plot_->initGroupBestFit(fitData, is_.i, QVariant(name_), /*isGroup*/false);
  }

  return fitData;
}

//------

CQChartsPointHullObj::
CQChartsPointHullObj(const CQChartsPointPlot *plot, int groupInd, const QString &name,
                     const ColorInd &ig, const ColorInd &is, const BBox &rect) :
 CQChartsPlotObj(const_cast<CQChartsPointPlot *>(plot), rect, is,
                 ig, ColorInd()), plot_(plot), groupInd_(groupInd), name_(name)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsPointHullObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig().i).arg(is().i);
}

QString
CQChartsPointHullObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  auto *hull = getHull();

  auto area = hull->area();

  QString groupName;

  if (name_ == "")
    groupName = plot_->groupIndName(groupInd_);
  else {
    ColorInd ind;

    groupName = plot_->singleGroupName(ind);

    if (groupName == "")
      groupName = name_;
  }

  tableTip.addBoldLine("Convex Hull");
  tableTip.addTableRow("Group", groupName);
  tableTip.addTableRow("Area" , area);

  return tableTip.str();
}

void
CQChartsPointHullObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsPlotObj::addProperties(model, path);
}

void
CQChartsPointHullObj::
draw(PaintDevice *device) const
{
  // draw hull
  auto drawHullData = [&](const Hull *hull, const ColorInd &ic) {
    // set pen/brush
    PenBrush penBrush;

    plot_->setPenBrush(penBrush, plot_->hullPenData(ic), plot_->hullBrushData(ic));

    Color color1;

    if (plot_->adjustedGroupColor(ig_.i, ig_.n, color1))
      CQChartsDrawUtil::updateBrushColor(penBrush.brush, plot_->interpColor(color1, ig_));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    hull->draw(device);
  };

  //---

  auto *hull = getHull();

  if (name_ != "")
    drawHullData(hull, is_);
  else
    drawHullData(hull, ig_);
}

CQChartsGrahamHull *
CQChartsPointHullObj::
getHull() const
{
  using Points = std::vector<Point>;

  auto addHullPoints = [&](Hull *hull, const Points &points) {
    for (const auto &p : points)
      hull->addPoint(p);
  };

  //---

  bool created;

  Hull *hull = nullptr;

  if (name_ == "") {
    // get hull for group (add if needed)
    hull = plot_->getHull(groupInd_, created);

    if (created) {
      const auto &points = plot_->indPoints(QVariant(groupInd_), /*isGroup*/true);

      addHullPoints(hull, points);
    }
  }
  else {
    // get hull for set (add if needed)
    hull = plot_->getHull(is_.i, created);

    if (created) {
      const auto &points = plot_->indPoints(QVariant(name_), /*isGroup*/false);

      addHullPoints(hull, points);
    }
  }

  return hull;
}
