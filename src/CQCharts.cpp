#include <CQCharts.h>
#include <CQChartsView.h>

// plot types
#include <CQChartsAdjacencyPlot.h>
#include <CQChartsBarChartPlot.h>
#include <CQChartsBoxPlot.h>
#include <CQChartsBubblePlot.h>
#include <CQChartsChordPlot.h>
#include <CQChartsDelaunayPlot.h>
#include <CQChartsDendrogramPlot.h>
#include <CQChartsDistributionPlot.h>
#include <CQChartsForceDirectedPlot.h>
#include <CQChartsGeometryPlot.h>
#include <CQChartsHierBubblePlot.h>
#include <CQChartsHierScatterPlot.h>
#include <CQChartsImagePlot.h>
#include <CQChartsParallelPlot.h>
#include <CQChartsPiePlot.h>
#include <CQChartsRadarPlot.h>
#include <CQChartsSankeyPlot.h>
#include <CQChartsScatterPlot.h>
#include <CQChartsSunburstPlot.h>
#include <CQChartsTreeMapPlot.h>
#include <CQChartsXYPlot.h>

#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>

#include <CQChartsColorEdit.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsColumnsEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsFillPatternEdit.h>
#include <CQChartsFillUnderEdit.h>
#include <CQChartsKeyLocationEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsLineDataEdit.h>
#include <CQChartsLineDashEdit.h>
#include <CQChartsPolygonEdit.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsRectEdit.h>
#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsSymbolEdit.h>

#include <CQChartsPolygonList.h>
#include <CQChartsNamePair.h>
#include <CQChartsSides.h>
#include <CQChartsFillUnder.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsWindow.h>
#include <CQChartsPath.h>

#include <CQChartsOptLength.h>
#include <CQChartsOptReal.h>

#include <CQPropertyView.h>
#include <iostream>

CQCharts::
CQCharts()
{
  // register variant meta types
  CQChartsColor         ::registerMetaType();
  CQChartsColumn        ::registerMetaType();
  CQChartsColumns       ::registerMetaType();
  CQChartsConnectionList::registerMetaType();
  CQChartsFillData      ::registerMetaType();
  CQChartsFillPattern   ::registerMetaType();
  CQChartsFillUnderSide ::registerMetaType();
  CQChartsFillUnderPos  ::registerMetaType();
  CQChartsKeyLocation   ::registerMetaType();
  CQChartsLength        ::registerMetaType();
  CQChartsLineData      ::registerMetaType();
  CQChartsLineDash      ::registerMetaType();
  CQChartsNamePair      ::registerMetaType();
  CQChartsOptLength     ::registerMetaType();
  CQChartsOptReal       ::registerMetaType();
  CQChartsPath          ::registerMetaType();
  CQChartsPolygonList   ::registerMetaType();
  CQChartsPosition      ::registerMetaType();
  CQChartsRect          ::registerMetaType();
  CQChartsPolygon       ::registerMetaType();
  CQChartsShapeData     ::registerMetaType();
  CQChartsSides         ::registerMetaType();
  CQChartsStyle         ::registerMetaType();
  CQChartsSymbol        ::registerMetaType();
  CQChartsTheme         ::registerMetaType();

  //---

  // init theme
  plotTheme_.setName("default");

  interfaceTheme().setDark(false);
}

CQCharts::
~CQCharts()
{
  for (auto &modelData : modelDatas_)
    delete modelData;

  delete plotTypeMgr_;
  delete columnTypeMgr_;
}

void
CQCharts::
init()
{
  plotTypeMgr_   = new CQChartsPlotTypeMgr;
  columnTypeMgr_ = new CQChartsColumnTypeMgr(this);

  //---

  // add plot types
  plotTypeMgr_->addType("adjacency"    , new CQChartsAdjacencyPlotType    );
  plotTypeMgr_->addType("barchart"     , new CQChartsBarChartPlotType     );
  plotTypeMgr_->addType("box"          , new CQChartsBoxPlotType          );
  plotTypeMgr_->addType("bubble"       , new CQChartsBubblePlotType       );
  plotTypeMgr_->addType("chord"        , new CQChartsChordPlotType        );
  plotTypeMgr_->addType("delaunay"     , new CQChartsDelaunayPlotType     );
  plotTypeMgr_->addType("dendrogram"   , new CQChartsDendrogramPlotType   );
  plotTypeMgr_->addType("distribution" , new CQChartsDistributionPlotType );
  plotTypeMgr_->addType("forcedirected", new CQChartsForceDirectedPlotType);
  plotTypeMgr_->addType("geometry"     , new CQChartsGeometryPlotType     );
  plotTypeMgr_->addType("hierbubble"   , new CQChartsHierBubblePlotType   );
  plotTypeMgr_->addType("hierscatter"  , new CQChartsHierScatterPlotType  );
  plotTypeMgr_->addType("image"        , new CQChartsImagePlotType        );
  plotTypeMgr_->addType("parallel"     , new CQChartsParallelPlotType     );
  plotTypeMgr_->addType("pie"          , new CQChartsPiePlotType          );
  plotTypeMgr_->addType("radar"        , new CQChartsRadarPlotType        );
  plotTypeMgr_->addType("sankey"       , new CQChartsSankeyPlotType       );
  plotTypeMgr_->addType("scatter"      , new CQChartsScatterPlotType      );
  plotTypeMgr_->addType("sunburst"     , new CQChartsSunburstPlotType     );
  plotTypeMgr_->addType("treemap"      , new CQChartsTreeMapPlotType      );
  plotTypeMgr_->addType("xy"           , new CQChartsXYPlotType           );

  //---

  // add column types
  columnTypeMgr_->addType(CQBaseModelType::STRING         , new CQChartsColumnStringType        );
  columnTypeMgr_->addType(CQBaseModelType::INTEGER        , new CQChartsColumnIntegerType       );
  columnTypeMgr_->addType(CQBaseModelType::REAL           , new CQChartsColumnRealType          );
  columnTypeMgr_->addType(CQBaseModelType::BOOLEAN        , new CQChartsColumnBooleanType       );
  columnTypeMgr_->addType(CQBaseModelType::TIME           , new CQChartsColumnTimeType          );
  columnTypeMgr_->addType(CQBaseModelType::COLOR          , new CQChartsColumnColorType         );
  columnTypeMgr_->addType(CQBaseModelType::RECT           , new CQChartsColumnRectType          );
  columnTypeMgr_->addType(CQBaseModelType::POLYGON        , new CQChartsColumnPolygonType       );
  columnTypeMgr_->addType(CQBaseModelType::POLYGON_LIST   , new CQChartsColumnPolygonListType   );
  columnTypeMgr_->addType(CQBaseModelType::IMAGE          , new CQChartsColumnImageType         );
  columnTypeMgr_->addType(CQBaseModelType::SYMBOL         , new CQChartsColumnSymbolTypeType    );
  columnTypeMgr_->addType(CQBaseModelType::SYMBOL_SIZE    , new CQChartsColumnSymbolSizeType    );
  columnTypeMgr_->addType(CQBaseModelType::FONT_SIZE      , new CQChartsColumnFontSizeType      );
  columnTypeMgr_->addType(CQBaseModelType::PATH           , new CQChartsColumnPathType          );
  columnTypeMgr_->addType(CQBaseModelType::STYLE          , new CQChartsColumnStyleType         );
  columnTypeMgr_->addType(CQBaseModelType::NAME_PAIR      , new CQChartsColumnNamePairType      );
  columnTypeMgr_->addType(CQBaseModelType::CONNECTION_LIST, new CQChartsColumnConnectionListType);

  //---

  // add property types (and editors)
  CQPropertyViewMgr *viewMgr = CQPropertyViewMgrInst;

  viewMgr->addType("CQChartsColor"        , new CQChartsColorPropertyViewType        );
  viewMgr->addType("CQChartsColumn"       , new CQChartsColumnPropertyViewType       );
  viewMgr->addType("CQChartsColumns"      , new CQChartsColumnsPropertyViewType      );
  viewMgr->addType("CQChartsFillData"     , new CQChartsFillDataPropertyViewType     );
  viewMgr->addType("CQChartsFillPattern"  , new CQChartsFillPatternPropertyViewType  );
  viewMgr->addType("CQChartsFillUnderPos" , new CQChartsFillUnderPosPropertyViewType );
  viewMgr->addType("CQChartsFillUnderSide", new CQChartsFillUnderSidePropertyViewType);
  viewMgr->addType("CQChartsKeyLocation"  , new CQChartsKeyLocationPropertyViewType  );
  viewMgr->addType("CQChartsLength"       , new CQChartsLengthPropertyViewType       );
  viewMgr->addType("CQChartsLineData"     , new CQChartsFillDataPropertyViewType     );
  viewMgr->addType("CQChartsLineDash"     , new CQChartsLineDashPropertyViewType     );
  viewMgr->addType("CQChartsPolygon"      , new CQChartsPolygonPropertyViewType      );
  viewMgr->addType("CQChartsPosition"     , new CQChartsPositionPropertyViewType     );
  viewMgr->addType("CQChartsRect"         , new CQChartsRectPropertyViewType         );
  viewMgr->addType("CQChartsShapeData"    , new CQChartsShapeDataPropertyViewType    );
  viewMgr->addType("CQChartsStrokeData"   , new CQChartsStrokeDataPropertyViewType   );
  viewMgr->addType("CQChartsSides"        , new CQChartsSidesPropertyViewType        );
  viewMgr->addType("CQChartsSymbol"       , new CQChartsSymbolPropertyViewType       );
}

bool
CQCharts::
isPlotType(const QString &name) const
{
  return plotTypeMgr_->isType(name);
}

CQChartsPlotType *
CQCharts::
plotType(const QString &name) const
{
  return plotTypeMgr_->type(name);
}

void
CQCharts::
getPlotTypes(PlotTypes &types) const
{
  plotTypeMgr_->getTypes(types);
}

void
CQCharts::
getPlotTypeNames(QStringList &names, QStringList &descs) const
{
  plotTypeMgr_->getTypeNames(names, descs);
}

//---

QColor
CQCharts::
interpColor(const CQChartsColor &c, int i, int n) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpColorValue(c, i, n, r);
}

QColor
CQCharts::
interpColor(const CQChartsColor &c, double value) const
{
  return interpColorValue(c, 0, -1, value);
}

QColor
CQCharts::
interpColorValue(const CQChartsColor &c, int i, int n, double value) const
{
  assert(c.isValid());

  if      (c.type() == CQChartsColor::Type::COLOR)
    return c.color();
  else if (c.type() == CQChartsColor::Type::PALETTE) {
    if (c.ind() == 0)
      return interpPaletteColorValue(i, n, value);
    else
      return interpIndPaletteColorValue(c.ind(), i, n, value);
  }
  else if (c.type() == CQChartsColor::Type::PALETTE_VALUE) {
    if (c.ind() == 0)
      return interpPaletteColor(c.value(), c.isScale());
    else
      return interpIndPaletteColor(c.ind(), c.value(), c.isScale());
  }
  else if (c.type() == CQChartsColor::Type::INTERFACE)
    return interpThemeColor(value);
  else if (c.type() == CQChartsColor::Type::INTERFACE_VALUE)
    return interpThemeColor(c.value());

  return QColor(0, 0, 0);
}

//---

void
CQCharts::
setPlotTheme(const CQChartsTheme &theme)
{
  CQChartsUtil::testAndSet(plotTheme_, theme, [&]() { emit themeChanged(); } );
}

QColor
CQCharts::
interpPaletteColor(double r, bool scale) const
{
  return interpIndPaletteColor(/*palette_ind*/0, r, scale);
}

QColor
CQCharts::
interpIndPaletteColor(int ind, double r, bool scale) const
{
  return interpIndPaletteColorValue(ind, 0, -1, r, scale);
}

QColor
CQCharts::
interpPaletteColorValue(int i, int n, double r, bool scale) const
{
  return interpIndPaletteColorValue(/*palette_ind*/0, i, n, r, scale);
}

QColor
CQCharts::
interpIndPaletteColorValue(int ind, int i, int n, double r, bool scale) const
{
  CQChartsGradientPalette *palette = this->themePalette(ind);

  if (! palette->isDistinct() || n <= 0)
    return palette->getColor(r, scale);

  int nc = palette->numColors();
  assert(nc > 0);

  int i1 = (i % nc);

  double r1 = CMathUtil::norm(i1, 0, nc - 1);

  return palette->getColor(r1, /*scale*/false);
}

QColor
CQCharts::
interpThemeColor(double r) const
{
  return this->interfaceTheme().interpColor(r, /*scale*/true);
}

CQChartsGradientPalette *
CQCharts::
themeGroupPalette(int i, int /*n*/) const
{
  return themeObj()->palette(i);
}

CQChartsGradientPalette *
CQCharts::
themePalette(int ind) const
{
  return themeObj()->palette(ind);
}

const CQChartsThemeObj *
CQCharts::
themeObj() const
{
  return plotTheme().obj();
}

CQChartsThemeObj *
CQCharts::
themeObj()
{
  return plotTheme().obj();
}

//---

CQChartsModelData *
CQCharts::
initModelData(ModelP &model)
{
  int ind;

  if (! getModelInd(model.data(), ind)) {
    ind = addModelData(model);

    emit modelDataAdded(ind);
  }

  return getModelData(ind);
}

CQChartsModelData *
CQCharts::
getModelData(const QAbstractItemModel *model) const
{
  if (! model)
    return nullptr;

  int ind;

  if (! getModelInd(model, ind))
    return nullptr;

  return getModelData(ind);
}

void
CQCharts::
setCurrentModelData(CQChartsModelData *modelData)
{
  if (modelData)
    setCurrentModelInd(modelData->ind());
  else
    setCurrentModelInd(-1);
}

void
CQCharts::
setCurrentModelInd(int ind)
{
  currentModelInd_ = ind;

  emit currentModelChanged(currentModelInd_);
}

CQChartsModelData *
CQCharts::
currentModelData() const
{
  if (modelDatas_.empty())
    return nullptr;

  if (currentModelInd_ >= 0 ) {
    for (auto &modelData : modelDatas_) {
      if (modelData->ind() == currentModelInd_)
        return modelData;
    }
  }

  return modelDatas_.back();
}

int
CQCharts::
addModelData(ModelP &model)
{
  CQChartsModelData *modelData = new CQChartsModelData(this, model);

  int ind;

  bool rc = assignModelInd(model.data(), ind);

  assert(rc);

  modelData->setInd(ind);

  modelDatas_.push_back(modelData);

  return modelData->ind();
}

bool
CQCharts::
getModelInd(const QAbstractItemModel *model, int &ind) const
{
  ind = -1;

  if (! model)
    return false;

  bool ok;

  ind = model->property("modelInd").toInt(&ok);

  if (! ok)
    return false;

  return true;
}

bool
CQCharts::
assignModelInd(QAbstractItemModel *model, int &ind)
{
  ind = ++lastModelInd_;

  return setModelInd(model, ind);
}

bool
CQCharts::
setModelInd(QAbstractItemModel *model, int ind)
{
  if (! model)
    return false;

  model->setProperty("modelInd", ind);

  return true;
}

CQChartsModelData *
CQCharts::
getModelData(int ind) const
{
  for (auto &modelData : modelDatas_)
    if (modelData->ind() == ind)
      return modelData;

  return nullptr;
}

void
CQCharts::
getModelDatas(ModelDatas &modelDatas) const
{
  modelDatas = modelDatas_;
}

void
CQCharts::
setModelName(CQChartsModelData *modelData, const QString &name)
{
  modelData->setName(name);

  emit modelNameChanged(name);
}

//---

CQChartsView *
CQCharts::
addView(const QString &id)
{
  CQChartsView *view = createView();

  QString id1 = id;

  if (id1 == "")
    id1 = QString("view%1").arg(views_.size() + 1);

  assert(! getView(id1));

  view->setId(id1);

  view->setObjectName(view->id());

  views_[id1] = view;

  emit viewAdded(view);

  return view;
}

CQChartsView *
CQCharts::
createView()
{
  CQChartsView *view = new CQChartsView(this);

  connect(view, SIGNAL(plotAdded(CQChartsPlot *)), this, SIGNAL(plotAdded(CQChartsPlot *)));

  return view;
}

CQChartsView *
CQCharts::
getView(const QString &id) const
{
  auto p = views_.find(id);

  if (p == views_.end())
    return nullptr;

  return (*p).second;
}

CQChartsView *
CQCharts::
currentView() const
{
  if (views_.empty())
    return nullptr;

  return views_.rbegin()->second;
}

void
CQCharts::
getViews(Views &views) const
{
  for (const auto &view : views_)
    views.push_back(view.second);
}

void
CQCharts::
getViewIds(QStringList &names) const
{
  for (const auto &view : views_)
    names.push_back(view.second->id());
}

void
CQCharts::
removeView(CQChartsView *view)
{
  views_.erase(view->id());
}

//---

CQChartsWindow *
CQCharts::
createWindow(CQChartsView *view)
{
  CQChartsWindow *window = CQChartsWindowMgrInst->createWindow(view);

  emit windowCreated(window);

  return window;
}

//---

void
CQCharts::
addProc(const QString &name, const QString &args, const QString &body)
{
  procs_[name] = ProcData(name, args, body);
}

//---

void
CQCharts::
emitModelTypeChanged(int modelId)
{
  emit modelTypeChanged(modelId);
}

//---

void
CQCharts::
errorMsg(const QString &msg) const
{
  std::cerr << msg.toStdString() << "\n";
}
