#include <CQChartsPlotObjTree.h>
#include <CQChartsPlotObj.h>
#include <CQPerfMonitor.h>
#include <future>

CQChartsPlotObjTree::
CQChartsPlotObjTree(CQChartsPlot *plot, bool wait) :
 plot_(plot), wait_(wait)
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
  clearObjects();

  //---

  if (! plot_->plotObjects().empty() && ! plot_->isNoData()) {
    busy_.store(true);

    plotObjTreeFuture_ = std::async(std::launch::async, addObjectsASync, this);
  }

  //---

  if (wait_)
    waitTree();
}

CQChartsPlotObjTree::PlotObjTree *
CQChartsPlotObjTree::
addObjectsASync(CQChartsPlotObjTree *th)
{
  return th->addObjectsThread();
}

CQChartsPlotObjTree::PlotObjTree *
CQChartsPlotObjTree::
addObjectsThread()
{
  CQPerfTrace trace("CQChartsPlotObjTree::addObjectsThread");

  PlotObjTree *plotObjTree = nullptr;

  CQChartsPlot::PlotObjs plotObjs = plot_->plotObjects();

  if (! plotObjs.empty() && ! plot_->isNoData()) {
    const CQChartsGeom::Range &range = plot_->dataRange();

    if (range.isSet()) {
      CQChartsGeom::BBox bbox(range.xmin(), range.ymin(), range.xmax(), range.ymax());

      plotObjTree = new PlotObjTree(bbox);

      for (const auto &obj : plotObjs) {
        if (interrupt_.load())
          break;

        if (! obj->visible())
          continue;

        if (obj->rect().isSet())
          plotObjTree->add(obj);
      }
    }
  }

  busy_.store(false);

  return plotObjTree;
}

void
CQChartsPlotObjTree::
clearObjects()
{
  interruptTree();

  delete plotObjTree_;

  plotObjTree_ = nullptr;
}

void
CQChartsPlotObjTree::
interruptTree()
{
  interrupt_.store(true);

  waitTree();

  interrupt_.store(false);
}

void
CQChartsPlotObjTree::
waitTree() const
{
  if (plotObjTreeFuture_.valid()) {
    CQChartsPlotObjTree *th = const_cast<CQChartsPlotObjTree *>(this);

    th->plotObjTree_ = th->plotObjTreeFuture_.get();

    assert(! th->plotObjTreeFuture_.valid());
  }
}

void
CQChartsPlotObjTree::
objectsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const
{
  waitTree();

  if (plotObjTree_) {
    PlotObjTree::DataList dataList;

    static_cast<PlotObjTree *>(plotObjTree_)->dataAtPoint(p.x, p.y, dataList);

    for (const auto &obj : dataList) {
      if (obj->inside(p))
        objs.push_back(obj);
    }
  }
}

void
CQChartsPlotObjTree::
objectsTouchingRect(const CQChartsGeom::BBox &r, Objs &objs) const
{
  waitTree();

  if (plotObjTree_) {
    PlotObjTree::DataList dataList;

    static_cast<PlotObjTree *>(plotObjTree_)->dataTouchingRect(r, dataList);

    for (const auto &obj : dataList) {
      if (obj->touching(r))
        objs.push_back(obj);
    }
  }
}
