#include <CQCharts.h>
#include <CQChartsView.h>
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
#include <CQChartsColumnType.h>
#include <CQChartsLineDashEdit.h>
#include <CQChartsLength.h>
#include <CQPropertyView.h>
#include <iostream>

CQCharts::
CQCharts()
{
  CQChartsColumn  ::registerMetaType();
  CQChartsLength  ::registerMetaType();
  CQChartsPosition::registerMetaType();
  CQChartsLineDash::registerMetaType();
  CQChartsColor   ::registerMetaType();
}

CQCharts::
~CQCharts()
{
  delete plotTypeMgr_;
  delete columnTypeMgr_;
}

void
CQCharts::
init()
{
  plotTypeMgr_   = new CQChartsPlotTypeMgr;
  columnTypeMgr_ = new CQChartsColumnTypeMgr;

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

  columnTypeMgr_->addType(CQBaseModel::Type::REAL   , new CQChartsColumnRealType   );
  columnTypeMgr_->addType(CQBaseModel::Type::INTEGER, new CQChartsColumnIntegerType);
  columnTypeMgr_->addType(CQBaseModel::Type::STRING , new CQChartsColumnStringType );
  columnTypeMgr_->addType(CQBaseModel::Type::RECT   , new CQChartsColumnRectType   );
  columnTypeMgr_->addType(CQBaseModel::Type::POLYGON, new CQChartsColumnPolygonType);
  columnTypeMgr_->addType(CQBaseModel::Type::COLOR  , new CQChartsColumnColorType  );
  columnTypeMgr_->addType(CQBaseModel::Type::TIME   , new CQChartsColumnTimeType   );

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
getPlotTypeNames(QStringList &names, QStringList &descs) const
{
  plotTypeMgr_->getTypeNames(names, descs);
}

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

  return view;
}

CQChartsView *
CQCharts::
createView()
{
  CQChartsView *view = new CQChartsView(this);

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

void
CQCharts::
getViewIds(QStringList &names) const
{
  for (const auto &view : views_)
    names.push_back(view.second->id());
}

void
CQCharts::
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}
