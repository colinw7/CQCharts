#ifndef CQChartsPlotObjTree_H
#define CQChartsPlotObjTree_H

#include <CQChartsQuadTree.h>
#include <CQChartsGeom.h>
#include <vector>
#include <future>

class CQChartsPlot;
class CQChartsPlotObj;
class QPainter;

/*!
 * \brief Charts Plot object quad tree
 * \ingroup Charts
 */
class CQChartsPlotObjTree {
 public:
  using Objs = std::vector<CQChartsPlotObj*>;

 public:
  CQChartsPlotObjTree(CQChartsPlot *plot, bool wait=false);

 ~CQChartsPlotObjTree();

  void addObjects();

  void clearObjects();

  void objectsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const;

  void objectsIntersectRect(const CQChartsGeom::BBox &r, Objs &objs, bool inside) const;

  bool objectNearest(const CQChartsGeom::Point &p, double searchX, double searchY,
                     CQChartsPlotObj* &obj) const;

  bool isBusy() const { return busy_.load(); }

  CQChartsGeom::BBox findEmptyBBox(double w, double h) const;

  bool waitTree() const;

  void draw(QPainter *painter);

 private:
  using PlotObjTree       = CQChartsQuadTree<CQChartsPlotObj,CQChartsGeom::BBox>;
  using PlotObjTreeFuture = std::future<PlotObjTree*>;

 private:
  static PlotObjTree *addObjectsASync(CQChartsPlotObjTree *plotObjTree);

  PlotObjTree *addObjectsThread();

  void interruptTree();

 private:
  CQChartsPlot*      plot_              { nullptr }; //!< parent plot
  PlotObjTree*       plotObjTree_       { nullptr }; //!< object tree
  PlotObjTreeFuture  plotObjTreeFuture_;             //!< future
  bool               wait_              { false };   //!< wait for thread
  std::atomic<bool>  busy_              { false };   //!< busy flag
  std::atomic<bool>  interrupt_         { false };   //!< interrupt flag
};

#endif
