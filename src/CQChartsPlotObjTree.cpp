#include <CQChartsPlotObjTree.h>
#include <CQChartsPlotObj.h>
#include <future>

CQChartsPlotObjTree::
CQChartsPlotObjTree(CQChartsPlot *plot) :
 plot_(plot)
{
}

CQChartsPlotObjTree::
~CQChartsPlotObjTree()
{
  delete plotObjTree_;
}

void
CQChartsPlotObjTree::
addObjects(const Objs &objs)
{
  delete plotObjTree_;

  plotObjTree_ = nullptr;

  objs_ = objs;

  plotObjTreeFuture_ = std::async(std::launch::async, addObjectsASync, plot_, objs);
}

CQChartsPlotObjTree::PlotObjTree *
CQChartsPlotObjTree::
addObjectsASync(CQChartsPlot *plot, const Objs &objs)
{
  const CQChartsGeom::Range &range = plot->dataRange();

  CQChartsGeom::BBox bbox(range.xmin(), range.ymin(), range.xmax(), range.ymax());

  PlotObjTree *plotObjTree = new PlotObjTree(bbox);

  for (const auto &obj : objs) {
    if (obj->rect().isSet())
      plotObjTree->add(obj);
  }

  return plotObjTree;
}

void
CQChartsPlotObjTree::
initTree() const
{
  if (! plotObjTree_ && plotObjTreeFuture_.valid()) {
    CQChartsPlotObjTree *th = const_cast<CQChartsPlotObjTree *>(this);

    plotObjTreeFuture_.wait();

    th->plotObjTree_ = th->plotObjTreeFuture_.get();
  }
}

void
CQChartsPlotObjTree::
clearObjects()
{
  delete plotObjTree_;

  plotObjTree_ = nullptr;
}

void
CQChartsPlotObjTree::
objectsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const
{
  initTree();

  PlotObjTree::DataList dataList;

  static_cast<PlotObjTree *>(plotObjTree_)->dataAtPoint(p.x, p.y, dataList);

  for (const auto &obj : dataList) {
    if (obj->inside(p))
      objs.push_back(obj);
  }
}
