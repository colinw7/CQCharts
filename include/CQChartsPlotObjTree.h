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

  void addObjects();

  void clearObjects();

  void objectsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const;

  void objectsTouchingRect(const CQChartsGeom::BBox &r, Objs &objs) const;

 private:
  using PlotObjTree       = CQChartsQuadTree<CQChartsPlotObj,CQChartsGeom::BBox>;
  using PlotObjTreeFuture = std::future<PlotObjTree*>;

 private:
  void initTree() const;

  static PlotObjTree *addObjectsASync(CQChartsPlot *plot);

 private:
  CQChartsPlot*     plot_        { nullptr }; // parent plot
  PlotObjTree*      plotObjTree_ { nullptr }; // object tree
  PlotObjTreeFuture plotObjTreeFuture_;       // future
};

#endif
