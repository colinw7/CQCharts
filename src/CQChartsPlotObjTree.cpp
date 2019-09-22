#include <CQChartsPlotObjTree.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPlot.h>
#include <CQPerfMonitor.h>
#include <QPainter>
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

bool
CQChartsPlotObjTree::
waitTree() const
{
  if (plotObjTreeFuture_.valid()) {
    CQChartsPlotObjTree *th = const_cast<CQChartsPlotObjTree *>(this);

    th->plotObjTree_ = th->plotObjTreeFuture_.get();

    assert(! th->plotObjTreeFuture_.valid());

    if (plotObjTree_)
      plot_->setPlotObjTreeSet(true);
  }

  return plotObjTree_;
}

void
CQChartsPlotObjTree::
objectsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const
{
  if (! waitTree()) return;

  PlotObjTree::DataList dataList;

  static_cast<PlotObjTree *>(plotObjTree_)->dataAtPoint(p.x, p.y, dataList);

  for (const auto &obj : dataList) {
    if (! obj->visible())
      continue;

    if (obj->inside(p))
      objs.push_back(obj);
  }
}

void
CQChartsPlotObjTree::
objectsIntersectRect(const CQChartsGeom::BBox &r, Objs &objs, bool inside) const
{
  if (! waitTree()) return;

  PlotObjTree::DataList dataList;

  if (inside)
    static_cast<PlotObjTree *>(plotObjTree_)->dataInsideRect(r, dataList);
  else
    static_cast<PlotObjTree *>(plotObjTree_)->dataTouchingRect(r, dataList);

  for (const auto &obj : dataList) {
    if (! obj->visible())
      continue;

    if (obj->rectIntersect(r, inside))
      objs.push_back(obj);
  }
}

bool
CQChartsPlotObjTree::
objectNearest(const CQChartsGeom::Point &p, double searchX, double searchY,
              CQChartsPlotObj* &obj) const
{
  obj = nullptr;

  if (! waitTree()) return false;

  CQChartsGeom::BBox bbox(p.x - searchX, p.y - searchY, p.x + searchX, p.y + searchY);

  Objs objs;

  objectsIntersectRect(bbox, objs, /*inside*/false);

  double r = 0.0;

  for (const auto &obj1 : objs) {
    double r1 = obj1->rect().distanceTo(p);

    if (! obj || r1 < r) {
      obj = obj1;
      r   = r1;
    }
  }

  return obj;
}

CQChartsGeom::BBox
CQChartsPlotObjTree::
findEmptyBBox(double w, double h) const
{
  if (! waitTree())
    return CQChartsGeom::BBox();

  CQChartsGeom::BBox rect = plotObjTree_->fitRect(w, h);

  return rect;
}

void
CQChartsPlotObjTree::
draw(QPainter *painter)
{
  if (! waitTree()) return;

  auto drawRect = [&](const CQChartsGeom::BBox &bbox, int n) {
    CQChartsGeom::BBox pbbox = plot_->windowToPixel(bbox);

    double g = 1.0 - std::min(std::max((n/16.0), 0.0), 1.0);

    painter->setPen  (Qt::black);
    painter->setBrush(QColor(g*255, g*255, g*255));

    painter->drawRect(pbbox.qrect());
  };

  plotObjTree_->processRect(drawRect);
}
