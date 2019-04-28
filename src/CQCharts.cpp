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

#include <CQChartsArrowDataEdit.h>
#include <CQChartsAxisSideEdit.h>
#include <CQChartsAxisTickLabelPlacementEdit.h>
#include <CQChartsBoxDataEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsColumnsEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsFillPatternEdit.h>
#include <CQChartsFillUnderEdit.h>
#include <CQChartsFontEdit.h>
#include <CQChartsKeyLocationEdit.h>
#include <CQChartsKeyPressBehaviorEdit.h>
#include <CQChartsTitleLocationEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsLineDataEdit.h>
#include <CQChartsLineDashEdit.h>
#include <CQChartsPolygonEdit.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsRectEdit.h>
#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsSymbolDataEdit.h>
#include <CQChartsSymbolEdit.h>
#include <CQChartsTextBoxDataEdit.h>
#include <CQChartsTextDataEdit.h>

#include <CQChartsPolygonList.h>
#include <CQChartsNamePair.h>
#include <CQChartsSides.h>
#include <CQChartsFillUnder.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsWindow.h>
#include <CQChartsPath.h>
#include <CQChartsVariant.h>

#include <CQChartsOptLength.h>
#include <CQChartsOptPosition.h>
#include <CQChartsOptReal.h>
#include <CQChartsOptRect.h>
#include <CQChartsColor.h>
#include <CQChartsFont.h>
#include <CQChartsInterfaceTheme.h>
#include <CQChartsTheme.h>

#include <CQPropertyView.h>
#include <iostream>

CQCharts::
CQCharts()
{
  // register variant meta types
  CQChartsArrowData             ::registerMetaType();
  CQChartsAxisSide              ::registerMetaType();
  CQChartsAxisTickLabelPlacement::registerMetaType();
  CQChartsBoxData               ::registerMetaType();
  CQChartsColor                 ::registerMetaType();
  CQChartsColumn                ::registerMetaType();
  CQChartsColumns               ::registerMetaType();
  CQChartsConnectionList        ::registerMetaType();
  CQChartsFillData              ::registerMetaType();
  CQChartsFillPattern           ::registerMetaType();
  CQChartsFillUnderPos          ::registerMetaType();
  CQChartsFillUnderSide         ::registerMetaType();
  CQChartsFont                  ::registerMetaType();
  CQChartsKeyLocation           ::registerMetaType();
  CQChartsKeyPressBehavior      ::registerMetaType();
  CQChartsTitleLocation         ::registerMetaType();
  CQChartsLength                ::registerMetaType();
  CQChartsLineDash              ::registerMetaType();
  CQChartsLineData              ::registerMetaType();
  CQChartsNamePair              ::registerMetaType();
  CQChartsOptLength             ::registerMetaType();
  CQChartsOptPosition           ::registerMetaType();
  CQChartsOptReal               ::registerMetaType();
  CQChartsOptRect               ::registerMetaType();
  CQChartsPath                  ::registerMetaType();
  CQChartsPolygonList           ::registerMetaType();
  CQChartsPolygon               ::registerMetaType();
  CQChartsPosition              ::registerMetaType();
  CQChartsRect                  ::registerMetaType();
  CQChartsShapeData             ::registerMetaType();
  CQChartsSides                 ::registerMetaType();
  CQChartsStrokeData            ::registerMetaType();
  CQChartsStyle                 ::registerMetaType();
  CQChartsSymbolData            ::registerMetaType();
  CQChartsSymbol                ::registerMetaType();
  CQChartsTextBoxData           ::registerMetaType();
  CQChartsTextData              ::registerMetaType();
  CQChartsThemeName             ::registerMetaType();

  //---

  // init theme
  plotTheme_.setName("default");

  //---

  interfaceTheme_ = new CQChartsInterfaceTheme;

  interfaceTheme()->setDark(false);
}

CQCharts::
~CQCharts()
{
  delete interfaceTheme_;

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

  static bool typesInitialized = false;

  if (! typesInitialized) {
    typesInitialized = true;

    //---

    // add property types (and editors)
    CQPropertyViewMgr *viewMgr = CQPropertyViewMgrInst;

    viewMgr->addType("CQChartsArrowData"       , new CQChartsArrowDataPropertyViewType       );
    viewMgr->addType("CQChartsAxisSide"        , new CQChartsAxisSidePropertyViewType        );
    viewMgr->addType("CQChartsAxisTickLabelPlacement",
                     new CQChartsAxisTickLabelPlacementPropertyViewType);
    viewMgr->addType("CQChartsBoxData"         , new CQChartsBoxDataPropertyViewType         );
    viewMgr->addType("CQChartsColor"           , new CQChartsColorPropertyViewType           );
    viewMgr->addType("CQChartsColumn"          , new CQChartsColumnPropertyViewType          );
    viewMgr->addType("CQChartsColumns"         , new CQChartsColumnsPropertyViewType         );
    viewMgr->addType("CQChartsFillData"        , new CQChartsFillDataPropertyViewType        );
    viewMgr->addType("CQChartsFillPattern"     , new CQChartsFillPatternPropertyViewType     );
    viewMgr->addType("CQChartsFillUnderPos"    , new CQChartsFillUnderPosPropertyViewType    );
    viewMgr->addType("CQChartsFillUnderSide"   , new CQChartsFillUnderSidePropertyViewType   );
    viewMgr->addType("CQChartsFont"            , new CQChartsFontPropertyViewType            );
    viewMgr->addType("CQChartsKeyLocation"     , new CQChartsKeyLocationPropertyViewType     );
    viewMgr->addType("CQChartsKeyPressBehavior", new CQChartsKeyPressBehaviorPropertyViewType);
    viewMgr->addType("CQChartsTitleLocation"   , new CQChartsTitleLocationPropertyViewType   );
    viewMgr->addType("CQChartsLength"          , new CQChartsLengthPropertyViewType          );
    viewMgr->addType("CQChartsLineDash"        , new CQChartsLineDashPropertyViewType        );
    viewMgr->addType("CQChartsLineData"        , new CQChartsLineDataPropertyViewType        );
    viewMgr->addType("CQChartsPolygon"         , new CQChartsPolygonPropertyViewType         );
    viewMgr->addType("CQChartsPosition"        , new CQChartsPositionPropertyViewType        );
    viewMgr->addType("CQChartsRect"            , new CQChartsRectPropertyViewType            );
    viewMgr->addType("CQChartsShapeData"       , new CQChartsShapeDataPropertyViewType       );
    viewMgr->addType("CQChartsSides"           , new CQChartsSidesPropertyViewType           );
    viewMgr->addType("CQChartsStrokeData"      , new CQChartsStrokeDataPropertyViewType      );
    viewMgr->addType("CQChartsSymbolData"      , new CQChartsSymbolDataPropertyViewType      );
    viewMgr->addType("CQChartsSymbol"          , new CQChartsSymbolPropertyViewType          );
    viewMgr->addType("CQChartsTextBoxData"     , new CQChartsTextBoxDataPropertyViewType     );
    viewMgr->addType("CQChartsTextData"        , new CQChartsTextDataPropertyViewType        );
  }
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

  return interpColor(c, r);
}

QColor
CQCharts::
interpColor(const CQChartsColor &c, double value) const
{
  return interpColorValue(c, /*ig*/0, /*ng*/-1, value);
}

QColor
CQCharts::
interpColorValue(const CQChartsColor &c, int ig, int ng, double value) const
{
  assert(c.isValid());

  if      (c.type() == CQChartsColor::Type::COLOR)
    return c.color();
  else if (c.type() == CQChartsColor::Type::PALETTE) {
    if (c.ind() < 0)
      return interpPaletteColorValue(ig, ng, value, c.isScale());
    else
      return interpIndPaletteColorValue(c.ind(), ig, ng, value, c.isScale());
  }
  else if (c.type() == CQChartsColor::Type::PALETTE_VALUE) {
    if (c.ind() < 0)
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
setPlotTheme(const CQChartsThemeName &theme)
{
  CQChartsUtil::testAndSet(plotTheme_, theme, [&]() { emit themeChanged(); } );
}

//---

QColor
CQCharts::
interpPaletteColor(int i, int n, bool scale) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpPaletteColor(r, scale);
}

QColor
CQCharts::
interpPaletteColor(double r, bool scale) const
{
  return interpIndPaletteColor(/*palette_ind*/-1, r, scale);
}

QColor
CQCharts::
interpIndPaletteColor(int ind, int i, int n, bool scale) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpIndPaletteColor(ind, r, scale);
}

QColor
CQCharts::
interpIndPaletteColor(int ind, double r, bool scale) const
{
  return interpIndPaletteColorValue(ind, 0, -1, r, scale);
}

QColor
CQCharts::
interpGroupPaletteColor(int ig, int ng, int i, int n, bool scale) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpGroupPaletteColor(ig, ng, r, scale);
}

QColor
CQCharts::
interpGroupPaletteColor(int ig, int ng, double r, bool scale) const
{
  return themeGroupPalette(ig, ng)->getColor(r, scale);
}

QColor
CQCharts::
interpPaletteColorValue(int ig, int ng, int i, int n, bool scale) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpIndPaletteColorValue(/*palette_ind*/-1, ig, ng, r, scale);
}

QColor
CQCharts::
interpPaletteColorValue(int ig, int ng, double r, bool scale) const
{
  return interpIndPaletteColorValue(/*palette_ind*/-1, ig, ng, r, scale);
}

QColor
CQCharts::
interpIndPaletteColorValue(int ind, int ig, int ng, double r, bool scale) const
{
  // if ind unset then use default palette number
  if (ind < 0)
    ind = 0;

  CQChartsGradientPalette *palette = this->themePalette(ind);

  if (! palette->isDistinct() || ng <= 0)
    return palette->getColor(r, scale);

  int nc = palette->numColors();
  assert(nc > 0);

  int i1 = (ig % nc);

  double r1 = CMathUtil::norm(i1, 0, nc - 1);

  return palette->getColor(r1, /*scale*/false);
}

QColor
CQCharts::
interpThemeColor(double r) const
{
  return this->interfaceTheme()->interpColor(r, /*scale*/true);
}

CQChartsGradientPalette *
CQCharts::
themeGroupPalette(int ig, int /*ng*/) const
{
  return themeObj()->palette(ig);
}

CQChartsGradientPalette *
CQCharts::
themePalette(int ind) const
{
  // if ind unset then use default palette number
  if (ind < 0)
    ind = 0;

  return themeObj()->palette(ind);
}

const CQChartsTheme *
CQCharts::
themeObj() const
{
  return plotTheme().obj();
}

CQChartsTheme *
CQCharts::
themeObj()
{
  return plotTheme().obj();
}

CQChartsColor
CQCharts::
adjustDefaultPalette(const CQChartsColor &c, const QString &defaultPalette) const
{
  if ((c.type() == CQChartsColor::Type::PALETTE ||
       c.type() == CQChartsColor::Type::PALETTE_VALUE) &&
      c.ind() < 0) {
    CQChartsColor c1 = c;

    int ind = themeObj()->paletteInd(defaultPalette);

    c1.setInd(ind);

    return c1;
  }

  return c;
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
removeModelData(ModelP &model)
{
  CQChartsModelData *modelData = getModelData(model.data());

  if (! modelData)
    return false;

  int i = 0;
  int n = modelDatas_.size();

  for ( ; i < n; ++i) {
    if (modelDatas_[i] == modelData)
      break;
  }

  if (i >= n)
    return false;

  ++i;

  for ( ; i < n; ++i)
    modelDatas_[i - 1] = modelDatas_[i];

  modelDatas_.pop_back();

  delete modelData;

  return true;
}

bool
CQCharts::
getModelInd(const QAbstractItemModel *model, int &ind) const
{
  ind = -1;

  if (! model)
    return false;

  bool ok;

  ind = CQChartsVariant::toInt(model->property("modelInd"), ok);

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

  if (id.length())
    view->setId(id);

  addView(view);

  return view;
}

void
CQCharts::
addView(CQChartsView *view)
{
  connect(view, SIGNAL(plotAdded(CQChartsPlot *)), this, SIGNAL(plotAdded(CQChartsPlot *)));

  //---

  QString id = view->id();

  if (id == "")
    id = QString("view%1").arg(views_.size() + 1);

  assert(! getView(id));

  view->setId(id);

  view->setObjectName(id);

  views_[id] = view;

  emit viewAdded(view);
}

CQChartsView *
CQCharts::
createView()
{
  CQChartsView *view = new CQChartsView(this);

  return view;
}

void
CQCharts::
deleteView(CQChartsView *view)
{
  delete view;
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
  emit viewRemoved(view);

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

void
CQCharts::
deleteWindow(CQChartsWindow *window)
{
  emit windowRemoved(window);

  CQChartsWindowMgrInst->removeWindow(window);
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
