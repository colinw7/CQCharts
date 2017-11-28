#ifndef CQChartsPlotObjTree_H
#define CQChartsPlotObjTree_H

#include <CQChartsQuadTree.h>
#include <CQChartsGeom.h>
#include <vector>
#include <future>

class CQChartsPlot;
class CQChartsPlotObj;

class CQChartsPlotObjTree {
 public:
  using Objs = std::vector<CQChartsPlotObj*>;

 public:
  CQChartsPlotObjTree(CQChartsPlot *plot);

 ~CQChartsPlotObjTree();

  void addObjects(const Objs &objs);

  void clearObjects();

  void objectsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const;

 private:
  using PlotObjTree       = CQChartsQuadTree<CQChartsPlotObj,CQChartsGeom::BBox>;
  using PlotObjTreeFuture = std::future<PlotObjTree*>;

 private:
  void initTree() const;

  static PlotObjTree *addObjectsASync(CQChartsPlot *plot, const Objs &objs);

 private:
  CQChartsPlot*     plot_        { nullptr };
  PlotObjTree*      plotObjTree_ { nullptr };
  Objs              objs_;
  PlotObjTreeFuture plotObjTreeFuture_;
};

#endif
