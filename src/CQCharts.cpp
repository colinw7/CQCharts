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
#include <CQChartsLineDashEdit.h>
#include <CQChartsLength.h>
#include <CQChartsPolygonList.h>
#include <CQChartsRect.h>
#include <CQChartsNamePair.h>
#include <CQChartsSides.h>
#include <CQChartsFillUnder.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsWindow.h>
#include <CQPropertyView.h>
#include <iostream>

CQCharts::
CQCharts()
{
  CQChartsColor         ::registerMetaType();
  CQChartsColumn        ::registerMetaType();
  CQChartsColumns       ::registerMetaType();
  CQChartsConnectionList::registerMetaType();
  CQChartsFillPattern   ::registerMetaType();
  CQChartsFillUnderSide ::registerMetaType();
  CQChartsFillUnderPos  ::registerMetaType();
  CQChartsKeyLocation   ::registerMetaType();
  CQChartsLength        ::registerMetaType();
  CQChartsLineDash      ::registerMetaType();
  CQChartsNamePair      ::registerMetaType();
  CQChartsPath          ::registerMetaType();
  CQChartsPolygonList   ::registerMetaType();
  CQChartsPosition      ::registerMetaType();
  CQChartsRect          ::registerMetaType();
  CQChartsSides         ::registerMetaType();
  CQChartsStyle         ::registerMetaType();
  CQChartsSymbol        ::registerMetaType();
  CQChartsTheme         ::registerMetaType();

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
  columnTypeMgr_->addType(CQBaseModel::Type::STRING         , new CQChartsColumnStringType        );
  columnTypeMgr_->addType(CQBaseModel::Type::INTEGER        , new CQChartsColumnIntegerType       );
  columnTypeMgr_->addType(CQBaseModel::Type::REAL           , new CQChartsColumnRealType          );
  columnTypeMgr_->addType(CQBaseModel::Type::BOOLEAN        , new CQChartsColumnBooleanType       );
  columnTypeMgr_->addType(CQBaseModel::Type::TIME           , new CQChartsColumnTimeType          );
  columnTypeMgr_->addType(CQBaseModel::Type::COLOR          , new CQChartsColumnColorType         );
  columnTypeMgr_->addType(CQBaseModel::Type::RECT           , new CQChartsColumnRectType          );
  columnTypeMgr_->addType(CQBaseModel::Type::POLYGON        , new CQChartsColumnPolygonType       );
  columnTypeMgr_->addType(CQBaseModel::Type::POLYGON_LIST   , new CQChartsColumnPolygonListType   );
  columnTypeMgr_->addType(CQBaseModel::Type::IMAGE          , new CQChartsColumnImageType         );
  columnTypeMgr_->addType(CQBaseModel::Type::SYMBOL         , new CQChartsColumnSymbolType        );
  columnTypeMgr_->addType(CQBaseModel::Type::SYMBOL_SIZE    , new CQChartsColumnSymbolSizeType    );
  columnTypeMgr_->addType(CQBaseModel::Type::FONT_SIZE      , new CQChartsColumnFontSizeType      );
  columnTypeMgr_->addType(CQBaseModel::Type::PATH           , new CQChartsColumnPathType          );
  columnTypeMgr_->addType(CQBaseModel::Type::STYLE          , new CQChartsColumnStyleType         );
  columnTypeMgr_->addType(CQBaseModel::Type::NAME_PAIR      , new CQChartsColumnNamePairType      );
  columnTypeMgr_->addType(CQBaseModel::Type::CONNECTION_LIST, new CQChartsColumnConnectionListType);

  //---

  CQPropertyViewMgrInst->addType("CQChartsLineDash", new CQChartsLineDashPropertyViewType);
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
  return interpIndPaletteColor(0, r, scale);
}

QColor
CQCharts::
interpIndPaletteColor(int ind, double r, bool scale) const
{
  return this->themePalette(ind)->getColor(r, scale);
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
  bool ok;

  int ind = model->property("modelInd").toInt(&ok);

  if (! ok) {
    ind = addModelData(model);

    emit modelDataAdded(ind);
  }

  return getModelData(ind);
}

CQChartsModelData *
CQCharts::
getModelData(QAbstractItemModel *model) const
{
  if (! model)
    return nullptr;

  bool ok;

  int ind = model->property("modelInd").toInt(&ok);

  if (! ok)
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
  int ind = modelDatas_.size() + 1;

  CQChartsModelData *modelData = new CQChartsModelData(this, model);

  model->setProperty("modelInd", ind);

  modelData->setInd(ind);

  modelDatas_.push_back(modelData);

  return modelData->ind();
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
