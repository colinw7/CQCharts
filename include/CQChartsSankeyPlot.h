#ifndef CQChartsSankeyPlot_H
#define CQChartsSankeyPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Sankey plot type
 * \ingroup Charts
 */
class CQChartsSankeyPlotType : public CQChartsConnectionPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsSankeyPlotType();

  QString name() const override { return "sankey"; }
  QString desc() const override { return "Sankey"; }

  void addParameters() override;

  bool hasTitle() const override { return false; }

  bool hasAxes() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  //---

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;

  //---

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsSankeyPlot;
class CQChartsSankeyPlotNode;
class CQChartsSankeyPlotEdge;
class CQChartsSankeyNodeObj;
class CQChartsSankeyEdgeObj;

/*!
 * \brief Sankey plot node
 * \ingroup Charts
 */
class CQChartsSankeyPlotNode {
 public:
  using Edges   = std::vector<CQChartsSankeyPlotEdge *>;
  using NodeSet = std::set<const CQChartsSankeyPlotNode *>;

 public:
  CQChartsSankeyPlotNode(const CQChartsSankeyPlot *plot, const QString &str);
 ~CQChartsSankeyPlotNode();

  CQChartsSankeyPlotNode *parent() const { return parent_; }

  QString str() const { return str_; }

  int id() const { return id_; }
  void setId(int id) { id_ = id; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  int group() const { return group_; }
  void setGroup(int i) { group_ = i; }

  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  const CQChartsModelIndex &ind() const { return ind_; }
  void setInd(const CQChartsModelIndex &ind) { ind_ = ind; }

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
  int                       id_        { -1 };
  CQChartsModelIndex        ind_;
  QString                   name_;
  double                    value_     { 0.0 };
  int                       group_     { -1 };
  int                       depth_     { -1 };
  Edges                     srcEdges_;
  Edges                     destEdges_;
  int                       srcDepth_  { -1 };
  int                       destDepth_ { -1 };
  int                       xpos_      { -1 };
  CQChartsSankeyNodeObj*    obj_       { nullptr };
};

//---

/*!
 * \brief Sankey plot edge
 * \ingroup Charts
 */
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

/*!
 * \brief Sankey Plot Node object
 * \ingroup Charts
 */
class CQChartsSankeyNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsSankeyNodeObj(const CQChartsSankeyPlot *plot, const BBox &rect,
                        CQChartsSankeyPlotNode *node, const ColorInd &ind);

  const QString &hierName() const { return hierName_; }
  void setHierName(const QString &s) { hierName_ = s; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  const BBox &srcEdgeRect(CQChartsSankeyPlotEdge *edge) const {
    auto p = srcEdgeRect_.find(edge);
    assert(p != srcEdgeRect_.end());

    return (*p).second;
  }

  const BBox &destEdgeRect(CQChartsSankeyPlotEdge *edge) const {
    auto p = destEdgeRect_.find(edge);
    assert(p != destEdgeRect_.end());

    return (*p).second;
  }

  QString typeName() const override { return "node"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void moveBy(const Point &delta);

  //---

  PlotObjs getConnected() const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  //---

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void writeScriptData(CQChartsScriptPaintDevice *device) const override;

 private:
  using EdgeRect = std::map<CQChartsSankeyPlotEdge *,BBox>;

  const CQChartsSankeyPlot* plot_ { nullptr }; //!< parent plot
  CQChartsSankeyPlotNode*   node_ { nullptr }; //!< node
  EdgeRect                  srcEdgeRect_;      //!< edge to src
  EdgeRect                  destEdgeRect_;     //!< edge to dest
  QString                   hierName_;         //!< node hier name
  QString                   name_;             //!< node name
  double                    value_ { 0.0 };    //!< node value
  int                       depth_ { -1 };     //!< node depth
};

//---

/*!
 * \brief Sankey Plot Edge object
 * \ingroup Charts
 */
class CQChartsSankeyEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsSankeyEdgeObj(const CQChartsSankeyPlot *plot, const BBox &rect,
                        CQChartsSankeyPlotEdge *edge);

  QString typeName() const override { return "edge"; }

  CQChartsSankeyPlotEdge *edge() const { return edge_; }

  QString calcId() const override;

  QString calcTipId() const override;

  void setSrcRect (const BBox &rect) { srcRect_  = rect; }
  void setDestRect(const BBox &rect) { destRect_ = rect; }

  //---

  bool inside(const Point &p) const override;

  PlotObjs getConnected() const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  //---

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void writeScriptData(CQChartsScriptPaintDevice *device) const override;

 private:
  const CQChartsSankeyPlot* plot_     { nullptr }; //!< parent plot
  CQChartsSankeyPlotEdge*   edge_     { nullptr }; //!< edge
  BBox                      srcRect_;              //!< src rect
  BBox                      destRect_;             //!< dest rect
  QPainterPath              path_;                 //!< painter path
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Edge,edge)

/*!
 * \brief Sankey Plot
 * \ingroup Charts
 */
class CQChartsSankeyPlot : public CQChartsConnectionPlot,
 public CQChartsObjTextData<CQChartsSankeyPlot>,
 public CQChartsObjNodeShapeData<CQChartsSankeyPlot>,
 public CQChartsObjEdgeShapeData<CQChartsSankeyPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(double nodeMargin READ nodeMargin WRITE setNodeMargin)
  Q_PROPERTY(double nodeWidth  READ nodeWidth  WRITE setNodeWidth )
  Q_PROPERTY(int    maxDepth   READ maxDepth   WRITE setMaxDepth  )

  // align
  Q_PROPERTY(Align align READ align WRITE setAlign)

  // node/edge shape style
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node,node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge,edge)

  // text style
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

  //! get/set node margin
  double nodeMargin() const { return nodeMargin_; }
  void setNodeMargin(double r);

  //---

  //! get/set x margin
  double nodeWidth() const { return nodeWidth_; }
  void setNodeWidth(double r);

  //---

  //! get/set max depth
  int maxDepth() const { return maxDepth_; }
  void setMaxDepth(int d);

  //---

  //! get/set text align
  const Align &align() const { return align_; }
  void setAlign(const Align &a);

  //---

  // add properties
  void addProperties() override;

  //---

  Range calcRange() const override;

  Range getCalcDataRange() const override;

  //---

  bool createObjs(PlotObjs &objs) const override;

  bool initHierObjs() const;

  void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;
  void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;

  void initHierObjsAddConnection(const QString &srcStr, const QString &destStr, int srcDepth,
                                 double value, CQChartsSankeyPlotNode* &srcNode,
                                 CQChartsSankeyPlotNode* &destNode) const;

  bool initLinkObjs      () const;
  bool initConnectionObjs() const;
  bool initTableObjs     () const;

  //---

  void updateMaxDepth() const;

  //---

  int numNodes() const { return nameNodeMap_.size(); }

  int maxHeight() const { return maxHeight_; }

  int maxNodeDepth() const { return maxNodeDepth_; }

  double valueScale() const { return valueScale_; }

  //---

  void keyPress(int key, int modifier) override;

  //---

 protected:
  void clearNodesAndEdges();

  CQChartsSankeyPlotNode *findNode(const QString &name) const;

  CQChartsSankeyPlotEdge *createEdge(double value, CQChartsSankeyPlotNode *srcNode,
                                     CQChartsSankeyPlotNode *destNode) const;

  void createGraph(PlotObjs &objs) const;

  void createDepthNodes(const IndNodeMap &nodes) const;

  CQChartsSankeyEdgeObj *addEdgeObj(CQChartsSankeyPlotEdge *edge) const;

  void adjustNodes() const;

  void initPosNodesMap();

  void adjustNodeCenters();

  void removeOverlaps() const;

  void reorderNodeEdges() const;

  void adjustNode(CQChartsSankeyPlotNode *node) const;

  //---

  virtual CQChartsSankeyNodeObj *createNodeObj(const BBox &rect, CQChartsSankeyPlotNode *node,
                                               const ColorInd &ind) const;

  virtual CQChartsSankeyEdgeObj *createEdgeObj(const BBox &rect,
                                               CQChartsSankeyPlotEdge *edge) const;

 private:
  using PosNodesMap = std::map<int,IndNodeMap>;

  // options
  Align align_ { Align::JUSTIFY }; //!< align

  // data
  NameNodeMap nameNodeMap_;             //!< name node map
  IndNodeMap  indNodeMap_;              //!< ind node map
  PosNodesMap posNodesMap_;             //!< pos node map
  Edges       edges_;                   //!< edges
  BBox        bbox_;                    //!< bbox
  int         maxHeight_     { 0 };     //!< max height
  int         maxNodeDepth_  { 0 };     //!< max node depth
  double      nodeMargin_    { 0.2 };   //!< node margin
  double      minNodeMargin_ { 4 };     //!< min node margin in pixels
  double      nodeWidth_     { 16 };    //!< x margin in pixels
  int         maxDepth_      { -1 };    //!< max depth
  double      boxMargin_     { 0.01 };  //!< bounding box margin
  double      edgeMargin_    { 0.01 };  //!< edge bounding box margin
  double      valueScale_    { 1.0 };   //!< value scale
  double      valueMargin_   { 0.0 };   //!< value margin
  bool        pressed_       { false }; //!< mouse pressed
  bool        nodeYSet_      { false }; //!< node y set
  double      nodeYMin_      { 0.0 };   //!< node y min
  double      nodeYMax_      { 0.0 };   //!< node y max
};

#endif
