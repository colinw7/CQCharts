#include <CQCharts.h>
#include <CQChartsAdjacencyPlot.h>
#include <CQChartsBarChartPlot.h>
#include <CQChartsBoxPlot.h>
#include <CQChartsBubblePlot.h>
#include <CQChartsDelaunayPlot.h>
#include <CQChartsGeometryPlot.h>
#include <CQChartsHierBubblePlot.h>
#include <CQChartsParallelPlot.h>
#include <CQChartsPiePlot.h>
#include <CQChartsScatterPlot.h>
#include <CQChartsSunburstPlot.h>
#include <CQChartsTreeMapPlot.h>
#include <CQChartsXYPlot.h>
#include <CQChartsColumn.h>

CQCharts::
CQCharts()
{
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

  plotTypeMgr_->addType("adjacency" , new CQChartsAdjacencyPlotType );
  plotTypeMgr_->addType("barchart"  , new CQChartsBarChartPlotType  );
  plotTypeMgr_->addType("box"       , new CQChartsBoxPlotType       );
  plotTypeMgr_->addType("bubble"    , new CQChartsBubblePlotType    );
  plotTypeMgr_->addType("delaunay"  , new CQChartsDelaunayPlotType  );
  plotTypeMgr_->addType("geometry"  , new CQChartsGeometryPlotType  );
  plotTypeMgr_->addType("hierbubble", new CQChartsHierBubblePlotType);
  plotTypeMgr_->addType("parallel"  , new CQChartsParallelPlotType  );
  plotTypeMgr_->addType("pie"       , new CQChartsPiePlotType       );
  plotTypeMgr_->addType("scatter"   , new CQChartsScatterPlotType   );
  plotTypeMgr_->addType("sunburst"  , new CQChartsSunburstPlotType  );
  plotTypeMgr_->addType("treemap"   , new CQChartsTreeMapPlotType   );
  plotTypeMgr_->addType("xy"        , new CQChartsXYPlotType        );

  columnTypeMgr_->addType("real"   , new CQChartsColumnRealType   );
  columnTypeMgr_->addType("integer", new CQChartsColumnIntegerType);
  columnTypeMgr_->addType("string" , new CQChartsColumnStringType );
  columnTypeMgr_->addType("time"   , new CQChartsColumnTimeType   );
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
getPlotTypes(QStringList &names, QStringList &descs) const
{
 plotTypeMgr_->getTypes(names, descs);
}
