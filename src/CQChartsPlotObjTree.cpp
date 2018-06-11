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
addObjects()
{
  lock_.lock();

  delete plotObjTree_;

  plotObjTree_ = nullptr;

  if (! plot_->plotObjects().empty() && ! plot_->isNoData()) {
    plotObjTreeFuture_ = std::async(std::launch::async, addObjectsASync, this);
  }

  lock_.unlock();
}

CQChartsPlotObjTree::PlotObjTree *
CQChartsPlotObjTree::
addObjectsASync(CQChartsPlotObjTree *th)
{
  th->lock_.lock();

  PlotObjTree *plotObjTree = nullptr;

  CQChartsPlot::PlotObjs plotObjs = th->plot_->plotObjects();

  if (! plotObjs.empty() && ! th->plot_->isNoData()) {
    const CQChartsGeom::Range &range = th->plot_->dataRange();

    CQChartsGeom::BBox bbox(range.xmin(), range.ymin(), range.xmax(), range.ymax());

    plotObjTree = new PlotObjTree(bbox);

    for (const auto &obj : plotObjs) {
      if (obj->rect().isSet())
        plotObjTree->add(obj);
    }

    th->lock_.unlock();
  }

  return plotObjTree;
}

void
CQChartsPlotObjTree::
initTree() const
{
  lock_.lock();

  if (! plotObjTree_ && plotObjTreeFuture_.valid()) {
    CQChartsPlotObjTree *th = const_cast<CQChartsPlotObjTree *>(this);

    plotObjTreeFuture_.wait();

    th->plotObjTree_ = th->plotObjTreeFuture_.get();
  }

  lock_.unlock();
}

void
CQChartsPlotObjTree::
clearObjects()
{
  initTree();

  lock_.lock();

  delete plotObjTree_;

  plotObjTree_ = nullptr;

  lock_.unlock();
}

void
CQChartsPlotObjTree::
objectsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const
{
  initTree();

  lock_.lock();

  if (plotObjTree_) {
    PlotObjTree::DataList dataList;

    static_cast<PlotObjTree *>(plotObjTree_)->dataAtPoint(p.x, p.y, dataList);

    for (const auto &obj : dataList) {
      if (obj->inside(p))
        objs.push_back(obj);
    }
  }

  lock_.unlock();
}

void
CQChartsPlotObjTree::
objectsTouchingRect(const CQChartsGeom::BBox &r, Objs &objs) const
{
  initTree();

  lock_.lock();

  if (plotObjTree_) {
    PlotObjTree::DataList dataList;

    static_cast<PlotObjTree *>(plotObjTree_)->dataTouchingRect(r, dataList);

    for (const auto &obj : dataList) {
      if (obj->touching(r))
        objs.push_back(obj);
    }
  }

  lock_.unlock();
}
