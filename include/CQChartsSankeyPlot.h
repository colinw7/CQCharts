#ifndef CQChartsSankeyPlot_H
#define CQChartsSankeyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

class CQChartsSankeyPlot;
class CQChartsSankeyPlotNode;
class CQChartsSankeyPlotEdge;
class CQChartsSankeyNodeObj;
class CQChartsSankeyEdgeObj;
class CQChartsTextBoxObj;

class CQChartsSankeyPlotNode {
 public:
  using Edges   = std::vector<CQChartsSankeyPlotEdge *>;
  using NodeSet = std::set<CQChartsSankeyPlotNode *>;

 public:
  CQChartsSankeyPlotNode(CQChartsSankeyPlot *plot, const QString &str);
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
  CQChartsSankeyPlot*     plot_      { nullptr };
  CQChartsSankeyPlotNode* parent_    { nullptr };
  QString                 str_;
  int                     ind_       { -1 };
  Edges                   srcEdges_;
  Edges                   destEdges_;
  mutable int             srcDepth_  { -1 };
  mutable int             destDepth_ { -1 };
  mutable int             xpos_      { -1 };
  CQChartsSankeyNodeObj*  obj_       { nullptr };
};

//---

class CQChartsSankeyPlotEdge {
 public:
  CQChartsSankeyPlotEdge(CQChartsSankeyPlot *plot, double value,
                         CQChartsSankeyPlotNode *srcNode, CQChartsSankeyPlotNode *destNode);

 ~CQChartsSankeyPlotEdge();

  double value() const { return value_; }

  CQChartsSankeyPlotNode *srcNode () const { return srcNode_ ; }
  CQChartsSankeyPlotNode *destNode() const { return destNode_; }

  CQChartsSankeyEdgeObj *obj() const { return obj_; }

  void setObj(CQChartsSankeyEdgeObj *obj);

 private:
  CQChartsSankeyPlot*     plot_     { nullptr };
  double                  value_    { 0.0 };
  CQChartsSankeyPlotNode* srcNode_  { nullptr };
  CQChartsSankeyPlotNode* destNode_ { nullptr };
  CQChartsSankeyEdgeObj*  obj_      { nullptr };
};

//---

class CQChartsSankeyNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsSankeyNodeObj(CQChartsSankeyPlot *plot, const CQChartsGeom::BBox &rect,
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

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  using EdgeRect = std::map<CQChartsSankeyPlotEdge *,CQChartsGeom::BBox>;

  CQChartsSankeyPlot*     plot_ { nullptr }; // parent plot
  CQChartsSankeyPlotNode* node_ { nullptr };
  EdgeRect                srcEdgeRect_;
  EdgeRect                destEdgeRect_;
};

//---

class CQChartsSankeyEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsSankeyEdgeObj(CQChartsSankeyPlot *plot, const CQChartsGeom::BBox &rect,
                        CQChartsSankeyPlotEdge *edge);

  QString calcId() const override;

  void setSrcRect (const CQChartsGeom::BBox &rect) { srcRect_  = rect; }
  void setDestRect(const CQChartsGeom::BBox &rect) { destRect_ = rect; }

  bool inside(const CQChartsGeom::Point &p) const;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsSankeyPlot*     plot_     { nullptr }; // parent plot
  CQChartsSankeyPlotEdge* edge_     { nullptr };
  CQChartsGeom::BBox      srcRect_;
  CQChartsGeom::BBox      destRect_;
  QPainterPath            path_;
};

//---

class CQChartsSankeyPlotType : public CQChartsPlotType {
 public:
  CQChartsSankeyPlotType();

  QString name() const override { return "sankey"; }
  QString desc() const override { return "Sankey"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsSankeyPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int            linkColumn      READ linkColumn      WRITE setLinkColumn     )
  Q_PROPERTY(int            valueColumn     READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(Align          align           READ align           WRITE setAlign          )
  Q_PROPERTY(bool           nodeFilled      READ isNodeFilled    WRITE setNodeFilled     )
  Q_PROPERTY(CQChartsColor  nodeFillColor   READ nodeFillColor   WRITE setNodeFillColor  )
  Q_PROPERTY(double         nodeFillAlpha   READ nodeFillAlpha   WRITE setNodeFillAlpha  )
  Q_PROPERTY(Pattern        nodeFillPattern READ nodeFillPattern WRITE setNodeFillPattern)
  Q_PROPERTY(bool           nodeStroked     READ isNodeStroked   WRITE setNodeStroked    )
  Q_PROPERTY(CQChartsColor  nodeStrokeColor READ nodeStrokeColor WRITE setNodeStrokeColor)
  Q_PROPERTY(double         nodeStrokeAlpha READ nodeStrokeAlpha WRITE setNodeStrokeAlpha)
  Q_PROPERTY(CQChartsLength nodeStrokeWidth READ nodeStrokeWidth WRITE setNodeStrokeWidth)
  Q_PROPERTY(bool           edgeFilled      READ isEdgeFilled    WRITE setEdgeFilled     )
  Q_PROPERTY(CQChartsColor  edgeFillColor   READ edgeFillColor   WRITE setEdgeFillColor  )
  Q_PROPERTY(double         edgeFillAlpha   READ edgeFillAlpha   WRITE setEdgeFillAlpha  )
  Q_PROPERTY(Pattern        edgeFillPattern READ edgeFillPattern WRITE setEdgeFillPattern)
  Q_PROPERTY(bool           edgeStroked     READ isEdgeStroked   WRITE setEdgeStroked    )
  Q_PROPERTY(CQChartsColor  edgeStrokeColor READ edgeStrokeColor WRITE setEdgeStrokeColor)
  Q_PROPERTY(double         edgeStrokeAlpha READ edgeStrokeAlpha WRITE setEdgeStrokeAlpha)
  Q_PROPERTY(CQChartsLength edgeStrokeWidth READ edgeStrokeWidth WRITE setEdgeStrokeWidth)
  Q_PROPERTY(bool           textVisible     READ isTextVisible   WRITE setTextVisible    )
  Q_PROPERTY(QFont          textFont        READ textFont        WRITE setTextFont       )
  Q_PROPERTY(CQChartsColor  textColor       READ textColor       WRITE setTextColor      )
  Q_PROPERTY(double         textAlpha       READ textAlpha       WRITE setTextAlpha      )
  Q_PROPERTY(bool           textContrast    READ isTextContrast  WRITE setTextContrast   )

  Q_ENUMS(Align)
  Q_ENUMS(Pattern)

 public:
  enum class Align {
    SRC,
    DEST,
    JUSTIFY
  };

  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

  using NameNodeMap = std::map<QString,CQChartsSankeyPlotNode *>;
  using IndNodeMap  = std::map<int,CQChartsSankeyPlotNode *>;
  using NodeSet     = std::set<CQChartsSankeyPlotNode *>;
  using Edges       = std::vector<CQChartsSankeyPlotEdge *>;

 public:
  CQChartsSankeyPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsSankeyPlot();

  //---

  int linkColumn() const { return linkColumn_; }
  void setLinkColumn(int i) { linkColumn_ = i; updateRangeAndObjs(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; updateRangeAndObjs(); }

  //---

  const Align &align() const { return align_; }
  void setAlign(const Align &v) { align_ = v; updateMaxDepth(); updateRangeAndObjs(); }

  //---

  bool isNodeFilled() const;
  void setNodeFilled(bool b);

  const CQChartsColor &nodeFillColor() const;
  void setNodeFillColor(const CQChartsColor &s);

  QColor interpNodeFillColor(int i, int n) const;

  double nodeFillAlpha() const;
  void setNodeFillAlpha(double r);

  Pattern nodeFillPattern() const;
  void setNodeFillPattern(Pattern pattern);

  //---

  bool isNodeStroked() const;
  void setNodeStroked(bool b);

  const CQChartsColor &nodeStrokeColor() const;
  void setNodeStrokeColor(const CQChartsColor &s);

  QColor interpNodeStrokeColor(int i, int n) const;

  double nodeStrokeAlpha() const;
  void setNodeStrokeAlpha(double r);

  const CQChartsLength &nodeStrokeWidth() const;
  void setNodeStrokeWidth(const CQChartsLength &l);

  //---

  bool isEdgeFilled() const;
  void setEdgeFilled(bool b);

  const CQChartsColor &edgeFillColor() const;
  void setEdgeFillColor(const CQChartsColor &s);

  QColor interpEdgeFillColor(int i, int n) const;

  double edgeFillAlpha() const;
  void setEdgeFillAlpha(double r);

  Pattern edgeFillPattern() const;
  void setEdgeFillPattern(Pattern pattern);

  //---

  bool isEdgeStroked() const;
  void setEdgeStroked(bool b);

  const CQChartsColor &edgeStrokeColor() const;
  void setEdgeStrokeColor(const CQChartsColor &s);

  QColor interpEdgeStrokeColor(int i, int n) const;

  double edgeStrokeAlpha() const;
  void setEdgeStrokeAlpha(double r);

  const CQChartsLength &edgeStrokeWidth() const;
  void setEdgeStrokeWidth(const CQChartsLength &l);

  //---

  bool isTextVisible() const;
  void setTextVisible(bool b);

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  const CQChartsColor &textColor() const;
  void setTextColor(const CQChartsColor &c);

  QColor interpTextColor(int i, int n) const;

  double textAlpha() const;
  void setTextAlpha(double r);

  bool isTextContrast() const;
  void setTextContrast(bool b);

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  void updateMaxDepth();

  //---

  int numNodes() const { return nameNodeMap_.size(); }

  int maxHeight() const { return maxHeight_; }

  int maxDepth() const { return maxDepth_; }

  double valueScale() const { return valueScale_; }

  //---

  void keyPress(int key, int modifier);

  //---

  void draw(QPainter *) override;

 private:
  CQChartsSankeyPlotNode *findNode(const QString &name);

  CQChartsSankeyPlotEdge *createEdge(double value, CQChartsSankeyPlotNode *srcNode,
                                     CQChartsSankeyPlotNode *destNode);

  void createGraph();

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

  int                 linkColumn_  { 0 };
  int                 valueColumn_ { 1 };
  Align               align_       { Align::JUSTIFY };
  CQChartsTextBoxObj* nodeBoxObj_  { nullptr };
  CQChartsBoxObj*     edgeBoxObj_  { nullptr };
  NameNodeMap         nameNodeMap_;
  IndNodeMap          indNodeMap_;
  PosNodesMap         posNodesMap_;
  Edges               edges_;
  CQChartsGeom::BBox  bbox_;
  int                 maxHeight_   { 0 };
  int                 maxDepth_    { 0 };
  double              valueScale_  { 1.0 };
  double              margin_      { 0.0 };
  bool                pressed_     { false };
};

#endif
