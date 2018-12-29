#ifndef CQChartsSankeyPlot_H
#define CQChartsSankeyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

class CQChartsSankeyPlotType : public CQChartsPlotType {
 public:
  CQChartsSankeyPlotType();

  QString name() const override { return "sankey"; }
  QString desc() const override { return "Sankey"; }

  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  QString description() const override;

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsSankeyPlot;
class CQChartsSankeyPlotNode;
class CQChartsSankeyPlotEdge;
class CQChartsSankeyNodeObj;
class CQChartsSankeyEdgeObj;

class CQChartsSankeyPlotNode {
 public:
  using Edges   = std::vector<CQChartsSankeyPlotEdge *>;
  using NodeSet = std::set<const CQChartsSankeyPlotNode *>;

 public:
  CQChartsSankeyPlotNode(const CQChartsSankeyPlot *plot, const QString &str);
 ~CQChartsSankeyPlotNode();

  CQChartsSankeyPlotNode *parent() const { return parent_; }

  QString str() const { return str_; }

  int ind() const { return ind_; }
  void setInd(int ind) { ind_ = ind; }

  const Edges &srcEdges () const { return srcEdges_ ; }
  const Edges &destEdges() const { return destEdges_; }

  void setSrcEdges (const Edges &edges) { srcEdges_  = edges; }
  void setDestEdges(const Edges &edges) { destEdges_ = edges; }

  void addSrcEdge (CQChartsSankeyPlotEdge *edge);
  void addDestEdge(CQChartsSankeyPlotEdge *edge);

  CQChartsSankeyNodeObj *obj() const { return obj_; }

  int srcDepth () const;
  int destDepth() const;

  int calcXPos() const;

  int xpos() const { return xpos_; }

  double edgeSum    () const;
  double srcEdgeSum () const;
  double destEdgeSum() const;

  void setObj(CQChartsSankeyNodeObj *obj);

 private:
  int srcDepth (NodeSet &visited) const;
  int destDepth(NodeSet &visited) const;

 private:
  const CQChartsSankeyPlot* plot_      { nullptr };
  CQChartsSankeyPlotNode*   parent_    { nullptr };
  QString                   str_;
  int                       ind_       { -1 };
  Edges                     srcEdges_;
  Edges                     destEdges_;
  mutable int               srcDepth_  { -1 };
  mutable int               destDepth_ { -1 };
  mutable int               xpos_      { -1 };
  CQChartsSankeyNodeObj*    obj_       { nullptr };
};

//---

class CQChartsSankeyPlotEdge {
 public:
  CQChartsSankeyPlotEdge(const CQChartsSankeyPlot *plot, double value,
                         CQChartsSankeyPlotNode *srcNode, CQChartsSankeyPlotNode *destNode);

 ~CQChartsSankeyPlotEdge();

  const CQChartsSankeyPlot *plot() const { return plot_; }

  double value() const { return value_; }

  CQChartsSankeyPlotNode *srcNode () const { return srcNode_ ; }
  CQChartsSankeyPlotNode *destNode() const { return destNode_; }

  CQChartsSankeyEdgeObj *obj() const { return obj_; }

  void setObj(CQChartsSankeyEdgeObj *obj);

 private:
  const CQChartsSankeyPlot* plot_     { nullptr };
  double                    value_    { 0.0 };
  CQChartsSankeyPlotNode*   srcNode_  { nullptr };
  CQChartsSankeyPlotNode*   destNode_ { nullptr };
  CQChartsSankeyEdgeObj*    obj_      { nullptr };
};

//---

class CQChartsSankeyNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsSankeyNodeObj(const CQChartsSankeyPlot *plot, const CQChartsGeom::BBox &rect,
                        CQChartsSankeyPlotNode *node);

  const CQChartsGeom::BBox &srcEdgeRect(CQChartsSankeyPlotEdge *edge) const {
    auto p = srcEdgeRect_.find(edge);
    assert(p != srcEdgeRect_.end());

    return (*p).second;
  }

  const CQChartsGeom::BBox &destEdgeRect(CQChartsSankeyPlotEdge *edge) const {
    auto p = destEdgeRect_.find(edge);
    assert(p != destEdgeRect_.end());

    return (*p).second;
  }

  QString calcId() const override;

  void moveBy(const CQChartsGeom::Point &delta);

  void getSelectIndices(Indices &) const override { }

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter) override;

  void drawFg(QPainter *painter) override;

 private:
  using EdgeRect = std::map<CQChartsSankeyPlotEdge *,CQChartsGeom::BBox>;

  const CQChartsSankeyPlot* plot_ { nullptr }; // parent plot
  CQChartsSankeyPlotNode*   node_ { nullptr };
  EdgeRect                  srcEdgeRect_;
  EdgeRect                  destEdgeRect_;
};

//---

class CQChartsSankeyEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsSankeyEdgeObj(const CQChartsSankeyPlot *plot, const CQChartsGeom::BBox &rect,
                        CQChartsSankeyPlotEdge *edge);

  QString calcId() const override;

  void setSrcRect (const CQChartsGeom::BBox &rect) { srcRect_  = rect; }
  void setDestRect(const CQChartsGeom::BBox &rect) { destRect_ = rect; }

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &) const override { }

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter) override;

 private:
  const CQChartsSankeyPlot* plot_     { nullptr }; // parent plot
  CQChartsSankeyPlotEdge*   edge_     { nullptr };
  CQChartsGeom::BBox        srcRect_;
  CQChartsGeom::BBox        destRect_;
  QPainterPath              path_;
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Edge,edge)

class CQChartsSankeyPlot : public CQChartsPlot,
 public CQChartsObjTextData<CQChartsSankeyPlot>,
 public CQChartsObjNodeShapeData<CQChartsSankeyPlot>,
 public CQChartsObjEdgeShapeData<CQChartsSankeyPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn linkColumn  READ linkColumn  WRITE setLinkColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)

  Q_PROPERTY(Align align READ align WRITE setAlign)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node,node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge,edge)

  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_ENUMS(Align)

 public:
  enum class Align {
    SRC,
    DEST,
    JUSTIFY
  };

  using NameNodeMap = std::map<QString,CQChartsSankeyPlotNode *>;
  using IndNodeMap  = std::map<int,CQChartsSankeyPlotNode *>;
  using NodeSet     = std::set<CQChartsSankeyPlotNode *>;
  using Edges       = std::vector<CQChartsSankeyPlotEdge *>;

 public:
  CQChartsSankeyPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsSankeyPlot();

  //---

  const CQChartsColumn &linkColumn() const { return linkColumn_; }
  void setLinkColumn(const CQChartsColumn &c) { linkColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c) { valueColumn_ = c; updateRangeAndObjs(); }

  //---

  const Align &align() const { return align_; }
  void setAlign(const Align &v) { align_ = v; updateMaxDepth(); updateRangeAndObjs(); }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() override;

  bool createObjs(PlotObjs &objs) override;

  void updateMaxDepth();

  //---

  int numNodes() const { return nameNodeMap_.size(); }

  int maxHeight() const { return maxHeight_; }

  int maxDepth() const { return maxDepth_; }

  double valueScale() const { return valueScale_; }

  //---

  void keyPress(int key, int modifier) override;

  //---

 private:
  CQChartsSankeyPlotNode *findNode(const QString &name) const;

  CQChartsSankeyPlotEdge *createEdge(double value, CQChartsSankeyPlotNode *srcNode,
                                     CQChartsSankeyPlotNode *destNode);

  void createGraph(PlotObjs &objs);

  void createNodes(const IndNodeMap &nodes);

  void createEdge(CQChartsSankeyPlotEdge *edge);

  void adjustNodes();

  void initPosNodesMap();

  void adjustNodeCenters();

  void removeOverlaps();

  void reorderNodeEdges();

  void adjustNode(CQChartsSankeyPlotNode *node);

 private:
  using PosNodesMap = std::map<int,IndNodeMap>;

  CQChartsColumn     linkColumn_;                     // link column
  CQChartsColumn     valueColumn_;                    // value column
  Align              align_       { Align::JUSTIFY }; // align
  NameNodeMap        nameNodeMap_;                    // name node map
  IndNodeMap         indNodeMap_;                     // ind node map
  PosNodesMap        posNodesMap_;                    // pos node map
  Edges              edges_;                          // edges
  CQChartsGeom::BBox bbox_;                           // bbox
  int                maxHeight_   { 0 };              // max height
  int                maxDepth_    { 0 };              // max depth
  double             valueScale_  { 1.0 };            // value scale
  double             margin_      { 0.0 };            // margin
  bool               pressed_     { false };          // mouse pressed
};

#endif
