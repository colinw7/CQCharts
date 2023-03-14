#ifndef CQChartsForceDirectedPlot_H
#define CQChartsForceDirectedPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsForceDirected.h>
#include <CForceDirected.h>

class CQBusyButton;

/*!
 * \brief Force Directed plot type
 * \ingroup Charts
 */
class CQChartsForceDirectedPlotType : public CQChartsConnectionPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsForceDirectedPlotType();

  QString name() const override { return "forcedirected"; }
  QString desc() const override { return "ForceDirected"; }

  void addParameters() override;
  void addGeneralParameters() override;

  bool hasAxes() const override { return false; }

  bool canProbe() const override { return false; }

//bool canRectSelect() const override { return false; }

  bool canEqualScale() const override { return true; }

  QString description() const override;

  //---

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  bool hasDepthColumn() const override { return false; }

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsForceDirectedPlot;

/*!
 * \brief Force Directed Plot Node object
 * \ingroup Charts
 */
class CQChartsForceDirectedNodeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString label READ label)

 public:
  using ForceDirectedPlot = CQChartsForceDirectedPlot;
  using Node              = CQChartsForceDirectedNode;
  using ForceNodeP        = CForceDirected::NodeP;

 public:
  CQChartsForceDirectedNodeObj(const ForceDirectedPlot *plot, ForceNodeP node, const BBox &bbox);

  virtual ~CQChartsForceDirectedNodeObj() { }

  QString typeName() const override { return "node"; }

  const ForceNodeP &node() const { return node_; }

  QString label() const;

  QString calcId() const override;
  QString calcTipId() const override;

  bool isSelected() const override;
  void setSelected(bool b) override;

  void getObjSelectIndices(Indices &inds) const override;

  void calcPenBrush(PenBrush & /*penBrush*/, bool /*updateState*/) const override { }

  ObjShapeType objShapeType() const override;

 protected:
  const ForceDirectedPlot* forceDirectedPlot_ { nullptr }; //!< parent plot
  ForceNodeP               node_;                          //!< associated node
};

/*!
 * \brief Force Directed Plot Edge object
 * \ingroup Charts
 */
class CQChartsForceDirectedEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString label READ label)

 public:
  using ForceDirectedPlot = CQChartsForceDirectedPlot;
  using Edge              = CQChartsForceDirectedEdge;
  using ForceEdgeP        = CForceDirected::EdgeP;
  using ForceNodeP        = CForceDirected::NodeP;

 public:
  CQChartsForceDirectedEdgeObj(const ForceDirectedPlot *plot, ForceEdgeP edge, const BBox &bbox);

  virtual ~CQChartsForceDirectedEdgeObj() { }

  QString typeName() const override { return "edge"; }

  const ForceEdgeP &edge() const { return edge_; }

  QString label() const;

  QString calcId() const override;
  QString calcTipId() const override;

  bool isSelected() const override;
  void setSelected(bool b) override;

  void getObjSelectIndices(Indices &inds) const override;

  void calcPenBrush(PenBrush & /*penBrush*/, bool /*updateState*/) const override { }

 protected:
  const ForceDirectedPlot* forceDirectedPlot_ { nullptr }; //!< parent plot
  ForceEdgeP               edge_;                          //!< associated edge
};

//---

/*!
 * \brief Force Directed Plot
 * \ingroup Charts
 */
class CQChartsForceDirectedPlot : public CQChartsConnectionPlot,
 public CQChartsObjNodeShapeData<CQChartsForceDirectedPlot>,
 public CQChartsObjNodeTextData <CQChartsForceDirectedPlot>,
 public CQChartsObjEdgeShapeData<CQChartsForceDirectedPlot>,
 public CQChartsObjEdgeTextData <CQChartsForceDirectedPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn edgeWidthColumn READ edgeWidthColumn WRITE setEdgeWidthColumn)

  // animation
  Q_PROPERTY(int    initSteps    READ initSteps    WRITE setInitSteps)
  Q_PROPERTY(int    animateSteps READ animateSteps WRITE setAnimateSteps)
  Q_PROPERTY(double stepSize     READ stepSize     WRITE setStepSize)
  Q_PROPERTY(double rangeSize    READ rangeSize    WRITE setRangeSize)
  Q_PROPERTY(int    numSteps     READ numSteps)
  Q_PROPERTY(double minDelta     READ minDelta     WRITE setMinDelta)
  Q_PROPERTY(int    maxSteps     READ maxSteps     WRITE setMaxSteps)

  // placement
  Q_PROPERTY(double stiffness READ stiffness WRITE setStiffness)
  Q_PROPERTY(double repulsion READ repulsion WRITE setRepulsion)
  Q_PROPERTY(double damping   READ damping   WRITE setDamping)
  Q_PROPERTY(bool   reset     READ isReset   WRITE setReset)

  Q_PROPERTY(double minSpringLength READ minSpringLength WRITE setMinSpringLength)
  Q_PROPERTY(double maxSpringLength READ maxSpringLength WRITE setMaxSpringLength)

  Q_PROPERTY(bool unitRange READ isUnitRange WRITE setUnitRange)

  // node data
  Q_PROPERTY(NodeShape      nodeShape         READ nodeShape           WRITE setNodeShape        )
  Q_PROPERTY(bool           nodeScaled        READ isNodeScaled        WRITE setNodeScaled       )
  Q_PROPERTY(CQChartsLength nodeSize          READ nodeSize            WRITE setNodeSize         )
  Q_PROPERTY(CQChartsLength minNodeSize       READ minNodeSize         WRITE setMinNodeSize      )
  Q_PROPERTY(bool           nodeValueColored  READ isNodeValueColored  WRITE setNodeValueColored )
  Q_PROPERTY(bool           nodeValueLabel    READ isNodeValueLabel    WRITE setNodeValueLabel   )
  Q_PROPERTY(NodeEdgeType   nodeEdgeValueType READ nodeEdgeValueType   WRITE setNodeEdgeValueType)
  Q_PROPERTY(bool           nodeUseEdgeValue  READ isNodeUseEdgeValue  WRITE setNodeUseEdgeValue )

  Q_PROPERTY(bool          nodeMouseColoring  READ isNodeMouseColoring  WRITE setNodeMouseColoring )
  Q_PROPERTY(NodeEdgeType  nodeMouseColorType READ nodeMouseColorType   WRITE setNodeMouseColorType)
  Q_PROPERTY(bool          nodeMouseValue     READ isNodeMouseValue     WRITE setNodeMouseValue    )
  Q_PROPERTY(bool          nodeMouseEdgeColor READ isNodeMouseEdgeColor WRITE setNodeMouseEdgeColor)
  Q_PROPERTY(QString       nodeTipNameLabel   READ nodeTipNameLabel     WRITE setNodeTipNameLabel  )
  Q_PROPERTY(QString       nodeTipValueLabel  READ nodeTipValueLabel    WRITE setNodeTipValueLabel )

  // edge data
  Q_PROPERTY(EdgeShape      edgeShape         READ edgeShape           WRITE setEdgeShape        )
  Q_PROPERTY(EdgeArrow      edgeArrow         READ edgeArrow           WRITE setEdgeArrow        )
  Q_PROPERTY(bool           edgeScaled        READ isEdgeScaled        WRITE setEdgeScaled       )
  Q_PROPERTY(CQChartsLength edgeWidth         READ edgeWidth           WRITE setEdgeWidth        )
  Q_PROPERTY(CQChartsLength arrowWidth        READ arrowWidth          WRITE setArrowWidth       )
  Q_PROPERTY(bool           edgeValueColored  READ isEdgeValueColored  WRITE setEdgeValueColored )
  Q_PROPERTY(bool           edgeValueLabel    READ isEdgeValueLabel    WRITE setEdgeValueLabel   )
  Q_PROPERTY(bool           edgeMouseColoring READ isEdgeMouseColoring WRITE setEdgeMouseColoring)
  Q_PROPERTY(bool           edgeMouseValue    READ isEdgeMouseValue    WRITE setEdgeMouseValue   )
  Q_PROPERTY(double         edgeMinWidth      READ edgeMinWidth        WRITE setEdgeMinWidth     )

  // node/edge shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)

  // node/edge text style
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Edge, edge)

  // text visible on mouse inside/selected (when text invisible)
  Q_PROPERTY(bool insideTextVisible   READ isInsideTextVisible   WRITE setInsideTextVisible  )
  Q_PROPERTY(bool selectedTextVisible READ isSelectedTextVisible WRITE setSelectedTextVisible)

  // text no elide on mouse inside/selected (when text clipped)
  Q_PROPERTY(bool insideTextNoElide   READ isInsideTextNoElide   WRITE setInsideTextNoElide  )
  Q_PROPERTY(bool selectedTextNoElide READ isSelectedTextNoElide WRITE setSelectedTextNoElide)

  // text no scale on mouse inside/selected (when text scaled)
  Q_PROPERTY(bool insideTextNoScale   READ isInsideTextNoScale   WRITE setInsideTextNoScale  )
  Q_PROPERTY(bool selectedTextNoScale READ isSelectedTextNoScale WRITE setSelectedTextNoScale)

  // busy state
  Q_PROPERTY(bool showBusyButton     READ isShowBusyButton     WRITE setShowBusyButton    )
  Q_PROPERTY(bool autoHideBusyButton READ isAutoHideBusyButton WRITE setAutoHideBusyButton)

  // info
  Q_PROPERTY(int numNodes READ numNodes)
  Q_PROPERTY(int numEdges READ numEdges)

  Q_ENUMS(NodeShape)
  Q_ENUMS(EdgeShape)
  Q_ENUMS(EdgeArrow)

  Q_ENUMS(NodeEdgeType)

 public:
  enum class NodeShape {
    NONE          = int(CQChartsNodeType::NONE),
    BOX           = int(CQChartsNodeType::BOX),
    CIRCLE        = int(CQChartsNodeType::CIRCLE),
    DOUBLE_CIRCLE = int(CQChartsNodeType::DOUBLE_CIRCLE)
  };

  enum class EdgeShape {
    NONE        = int(CQChartsEdgeType::NONE),
    LINE        = int(CQChartsEdgeType::LINE),
    RECTILINEAR = int(CQChartsEdgeType::RECTILINEAR),
    ARC         = int(CQChartsEdgeType::ARC)
  };

  enum class EdgeArrow {
    NONE,
    END,
    MID
  };

  enum class NodeEdgeType {
    NONE     = int(CQChartsForceDirectedEdgeValueType::NONE),
    SRC      = int(CQChartsForceDirectedEdgeValueType::SRC),
    DEST     = int(CQChartsForceDirectedEdgeValueType::DEST),
    SRC_DEST = int(CQChartsForceDirectedEdgeValueType::SRC_DEST)
  };

  enum class MouseOver {
    NONE,
    NODE,
    EDGE
  };

  using NodeObj    = CQChartsForceDirectedNodeObj;
  using EdgeObj    = CQChartsForceDirectedEdgeObj;
  using SpringVec  = Springy::Vector;
  using ForceEdgeP = CForceDirected::EdgeP;
  using ForceNodeP = CForceDirected::NodeP;
  using Length     = CQChartsLength;
  using Color      = CQChartsColor;
  using Alpha      = CQChartsAlpha;
  using Angle      = CQChartsAngle;
  using PenBrush   = CQChartsPenBrush;
  using PenData    = CQChartsPenData;
  using BrushData  = CQChartsBrushData;
  using ColorInd   = CQChartsUtil::ColorInd;

  struct DrawTextData {
    CQChartsFont        font;
    PenBrush            penBrush;
    CQChartsTextOptions textOptions;
    Point               point;
    BBox                bbox;
    QStringList         strs;
    NodeShape           shape { NodeShape::NONE };
    QColor              contrastColor;
  };

  struct InsideDrawData {
    PenBrush penBrush;

    InsideDrawData() { }
    InsideDrawData(const PenBrush &pb) : penBrush(pb) { }
  };

 private:
  struct Connection;
  struct ConnectionsData;

  using Node = CQChartsForceDirectedNode;
  using Edge = CQChartsForceDirectedEdge;

  struct NodeShapeBBox {
    Node::Shape shape;
    BBox        bbox;
  };

  using NodeSet = std::set<Node *>;
  using EdgeSet = std::set<Edge *>;

 public:
  CQChartsForceDirectedPlot(View *view, const ModelP &model);
 ~CQChartsForceDirectedPlot();

  //---

  void init() override;
  void term() override;

  //---

  bool isBufferLayers() const override { return false; }

  bool useObjTree() const override { return false; }

  //----

  //! get/set edge width column
  const Column &edgeWidthColumn() const { return edgeWidthColumn_; }
  void setEdgeWidthColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //--

  //! set animating
  void setAnimating(bool b) override;

  //! get/set init steps
  int initSteps() const { return initSteps_; }
  void setInitSteps(int i);

  //! get/set animate steps
  int animateSteps() const { return animateSteps_; }
  void setAnimateSteps(int i);

  //! get number of steps
  int numSteps() const { return numSteps_; }

  //! get/set step size
  double stepSize() const { return stepSize_; }
  void setStepSize(double s);

  //! get/set range size
  double rangeSize() const { return rangeSize_; }
  void setRangeSize(double r);

  //! get/set min delta
  double minDelta() const { return minDelta_; }
  void setMinDelta(double r);

  //! get/set min delta
  int maxSteps() const { return maxSteps_; }
  void setMaxSteps(int n);

  //----

  //! get/set placement stiffness
  double stiffness() const { return stiffness_; }
  void setStiffness(double r);

  //! get/set placement repulsion
  double repulsion() const { return repulsion_; }
  void setRepulsion(double r);

  //! get/set placement damping
  double damping() const { return damping_; }
  void setDamping(double r);

  //! get/set reset placement
  bool isReset() const { return false; }
  void setReset(bool b);

  double minSpringLength() const { return minSpringLength_; }
  void setMinSpringLength(double r);

  double maxSpringLength() const { return maxSpringLength_; }
  void setMaxSpringLength(double r);

  bool isUnitRange() const { return unitRange_; }
  void setUnitRange(bool b) { unitRange_ = b; }

  //---

  //! get/set node shape
  NodeShape nodeShape() const { return nodeDrawData_.shape; }
  void setNodeShape(const NodeShape &s);

  //! get/set node scaled
  bool isNodeScaled() const { return nodeDrawData_.scaled; }
  void setNodeScaled(bool b);

  //! get/set node size
  const Length &nodeSize() const { return nodeDrawData_.size; }
  void setNodeSize(const Length &s);

  //! get/set min node size
  const Length &minNodeSize() const { return nodeDrawData_.minSize; }
  void setMinNodeSize(const Length &s);

  //! get/set node value colored
  bool isNodeValueColored() const { return nodeDrawData_.valueColored; }
  void setNodeValueColored(bool b);

  //! get/set node value label
  bool isNodeValueLabel() const { return nodeDrawData_.valueLabel; }
  void setNodeValueLabel(bool b);

  //! get/set which edges are used to node value
  const NodeEdgeType &nodeEdgeValueType() const { return nodeEdgeValueType_; }
  void setNodeEdgeValueType(const NodeEdgeType &t);

  //! get/set node uses edges for value
  bool isNodeUseEdgeValue() const { return nodeUseEdgeValue_; }
  void setNodeUseEdgeValue(bool b);

  //---

  //! get/set node edges colored on mouse over
  bool isNodeMouseColoring() const { return nodeDrawData_.mouseColoring; }
  void setNodeMouseColoring(bool b);

  //! get/set node edges colored on mouse over
  NodeEdgeType nodeMouseColorType() const { return nodeDrawData_.mouseColorType; }
  void setNodeMouseColorType(const NodeEdgeType &type);

  //! get/set show node value on mouse over
  bool isNodeMouseValue() const { return nodeDrawData_.mouseValue; }
  void setNodeMouseValue(bool b);

  //! get/set edges use node color on mouse over
  bool isNodeMouseEdgeColor() const { return nodeDrawData_.mouseEdgeColor; }
  void setNodeMouseEdgeColor(bool b);

  //! get/set node tip name label
  const QString &nodeTipNameLabel() const { return nodeDrawData_.tipNameLabel; }
  void setNodeTipNameLabel(const QString &s) { nodeDrawData_.tipNameLabel = s; }

  //! get/set node tip value label
  const QString &nodeTipValueLabel() const { return nodeDrawData_.tipValueLabel; }
  void setNodeTipValueLabel(const QString &s) { nodeDrawData_.tipValueLabel = s; }

  //---

  //! get/set edge shape
  const EdgeShape &edgeShape() const { return edgeDrawData_.shape; }
  void setEdgeShape(const EdgeShape &s);

  //! get/set has arrow
  const EdgeArrow &edgeArrow() const { return edgeDrawData_.arrow; }
  void setEdgeArrow(const EdgeArrow &arrow);

  //! get/set is edge scaled
  bool isEdgeScaled() const { return edgeDrawData_.scaled; }
  void setEdgeScaled(bool b);

  //! get/set edge width (in pixels)
  const Length &edgeWidth() const { return edgeDrawData_.width; }
  void setEdgeWidth(const Length &l);

  //! get/set edge directed arrow width
  const Length &arrowWidth() const { return edgeDrawData_.arrowWidth; }
  void setArrowWidth(const Length &w);

  //! get/set edge value colored
  bool isEdgeValueColored() const { return edgeDrawData_.valueColored; }
  void setEdgeValueColored(bool b);

  //! get/set edge value label
  bool isEdgeValueLabel() const { return edgeDrawData_.valueLabel; }
  void setEdgeValueLabel(bool b);

  //! get/set edge nodes colored on mouse over
  bool isEdgeMouseColoring() const { return edgeDrawData_.mouseColoring; }
  void setEdgeMouseColoring(bool b);

  //! get/set show edge value on mouse over
  bool isEdgeMouseValue() const { return edgeDrawData_.mouseValue; }
  void setEdgeMouseValue(bool b);

  //! get/set edge min width
  double edgeMinWidth() const { return edgeMinWidth_; }
  void setEdgeMinWidth(double r) { edgeMinWidth_ = r; }

  //---

  //! text visible on inside (when text invisible)
  bool isInsideTextVisible() const { return insideTextVisible_; }
  void setInsideTextVisible(bool b) { insideTextVisible_ = b; }

  //! text visible on selected (when text invisible)
  bool isSelectedTextVisible() const { return selectedTextVisible_; }
  void setSelectedTextVisible(bool b) { selectedTextVisible_ = b; }

  //! text no elide on inside (when text clipped)
  bool isInsideTextNoElide() const { return insideTextNoElide_; }
  void setInsideTextNoElide(bool b) { insideTextNoElide_ = b; }

  //! text no elide on selected (when text clipped)
  bool isSelectedTextNoElide() const { return selectedTextNoElide_; }
  void setSelectedTextNoElide(bool b) { selectedTextNoElide_ = b; }

  //! text no scale on inside (when text scaled)
  bool isInsideTextNoScale() const { return insideTextNoScale_; }
  void setInsideTextNoScale(bool b) { insideTextNoScale_ = b; }

  //! text no scale on selected (when text scaled)
  bool isSelectedTextNoScale() const { return selectedTextNoScale_; }
  void setSelectedTextNoScale(bool b) { selectedTextNoScale_ = b; }

  //---

  bool isShowBusyButton() const { return showBusyButton_; }
  void setShowBusyButton(bool b);

  bool isAutoHideBusyButton() const { return autoHideBusyButton_; }
  void setAutoHideBusyButton(bool b);

  //---

  double maxNodeValue() const { return maxNodeValue_; }
  double maxEdgeValue() const { return maxEdgeValue_; }

  double maxEdgeWidthValue() const { return maxEdgeWidthValue_; }

  //---

  void execInitSteps();

  //---

  int numNodes() const;
  int numEdges() const;

  //---

  bool isAnimated() const override { return true; }

  void animateStep() override;

  //---

  // add properties
  void addProperties() override;

  //---

  void checkExtraColumns(bool &columnsValid) const override;

  //---

  Range calcRange() const override;

  //--

  void clearPlotObjList() override;

  bool createObjs(PlotObjs &objs) const override;

  void addPlotObjects();
  void removePlotObjects();

  //---

  bool initHierObjs() const;

  void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;
  void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;

  void initHierObjsAddConnection(const HierConnectionData &srcHierData,
                                 const HierConnectionData &destHierData) const;

  //---

  bool initPathObjs() const;

  void addPathValue(const PathData &pathData) const override;

  void propagatePathValues();

  //---

  bool initFromToObjs() const;

  void addFromToValue(const FromToData &fromToData) const override;

  //---

  bool initLinkObjs() const;
  bool initConnectionObjs() const;
  bool initLinkConnectionObjs() const;

  void addLinkConnection(const LinkConnectionData &) const override { }

  //---

  bool initTableObjs() const;

  //---

  void initEdgeValueName(const Column &c);

  //---

  void processNodeNameValues(ConnectionsData &connectionsData,
                             const NameValues &nameValues) const;
  void processNodeNameValue(ConnectionsData &connectionsData, const QString &name,
                            const QString &valueStr) const;

  void processEdgeNameValues(Connection *, const NameValues &nameValues) const;

  //---

  static void stringToShapeType(const QString &str, NodeShape &shapeType);
  static void stringToShapeType(const QString &str, EdgeShape &shapeType);

  //---

  void postUpdateObjs() override;

  void filterObjs();

  void processMetaData() const;

  void addIdConnections() const;

  //---

  //! plot select interface
  bool handleSelectPress  (const Point &p, SelMod selMod) override;
  bool handleSelectMove   (const Point &p, Constraints constraints, bool first=false) override;
  bool handleSelectRelease(const Point &p) override;

  // plot edit interface
  bool handleEditPress  (const Point &p, const Point &w, bool inside=false) override;
  bool handleEditMove   (const Point &p, const Point &w, bool first=false) override;
  bool handleEditRelease(const Point &p, const Point &w) override;

  bool pointSelect(const Point &p, SelMod selMod) override;
  bool rectSelect(const BBox &r, SelMod selMod) override;

  //---

  void updatePlotObjs();

  //---

  void setCurrentNode(const Point &p);
  void resetCurrentNode();

  bool selectPoint(const Point &p, SelMod selMod);
  bool selectRect(const BBox &r, SelMod selMod, bool inside);

  bool selectGeom(const BBox &r, const Point &p, SelMod selMod, bool inside, bool isRect);

  bool updateInside(const Point &p);

  void selectedNodesAndEdges(NodeSet &selectedNodes, EdgeSet &selectedEdges) const;

  void updateSelText();

  //---

  bool keyPress(int key, int modifier) override;

  bool plotTipText(const Point &p, QString &tip, bool single) const override;

  void nodeTipText(Node *node, CQChartsTableTip &tableTip) const;
  void edgeTipText(Edge *edge, CQChartsTableTip &tableTip) const;

  void nearestNodeEdge(const Point &p, Node* &insideNode, Edge* &insideEdge) const;

  void insideNodesAndEdges(const BBox &r, NodeSet &insideNodes,
                           EdgeSet &insideEdges, bool inside) const;

  //---

  void drawParts(QPainter *painter) const override;

  void drawDeviceParts(PaintDevice *device) const;

  //---

  void drawEdge(PaintDevice *device, const ForceEdgeP &edge, Edge *sedge) const;

  bool isEdgeLine(Edge *sedge, double &lw, double tol) const;

  PenBrush calcEdgePenBrush(Edge *sedge, const ColorInd &colorInd,
                            bool &isLine, double &lw) const;

  void drawEdgeText(PaintDevice *device, Edge *sedge, const ColorInd &colorInd,
                    const MouseOver &mouseOver) const;

  //---

  void drawNode(PaintDevice *device, const ForceNodeP &node, Node *snode) const;

  void calcNodePenBrush(Node *snode, const ColorInd &colorInd,
                        PenBrush &penBrush) const;

  void drawNodeText(PaintDevice *device, Node *snode, const ColorInd &colorInd,
                    const MouseOver &mouseOver) const;

  void drawNodeShape(PaintDevice *device, Node *snode) const;

  //---

  void drawNodeInside(PaintDevice *device, const ForceNodeP &node,
                      const InsideDrawData &drawData) const;
  void drawEdgeInside(PaintDevice *device, const ForceEdgeP &edge,
                      const InsideDrawData &drawData) const;

  //---

  void drawTextData(PaintDevice *device, const DrawTextData &textData,
                    const MouseOver &mouseOver) const;

  //---

  void postResize() override;

  bool postResizeUpdateRange() override;

  //---

  void visibleChanged(bool) override;

  //---

  QString calcNodeLabel(Node *nodes) const;

  //---

  void execAnimateStep();

  void doAutoFit();

  void autoFitUpdate() override;

  //---

  Node::Shape calcNodeShape(Node *snode) const;
  Edge::Shape calcEdgeShape(Edge *sedge) const;

 private:
  struct FillData {
    Color color;
  };

  // connection between nodes (edge)
  struct Connection {
    int         srcNode   { -1 };              //!< source node index
    int         destNode  { -1 };              //!< destination node index
    OptReal     value;                         //!< custom value
    OptReal     edgeWidth;                     //!< custom edge width
    QString     label;                         //!< custom label
    EdgeShape   shapeType { EdgeShape::NONE }; //!< edge shape
    FillData    fillData;                      //!< fill data
    QModelIndex ind;                           //!< model index

    Connection() = default;

    Connection(int srcNode, int destNode, double value) :
     srcNode(srcNode), destNode(destNode), value(value) {
    }
  };

  using Connections = std::vector<Connection>;

  // node connection data
  struct ConnectionsData {
    QModelIndex ind;                              //!< model index
    int         node         { 0 };               //!< unique index
    QString     name;                             //!< name
    QString     label;                            //!< label
    int         group        { 0 };               //!< group
    OptReal     value;                            //!< value
    OptReal     total;                            //!< total
    int         depth        { -1 };              //!< depth
    bool        visible      { true };            //!< is visible
    int         parentId     { -1 };              //!< parent
    NodeShape   shapeType    { NodeShape::NONE }; //!< shape
    FillData    fillData;                         //!< fill data
    Connections connections;                      //!< connections
  };

  using IdConnectionsData = std::map<int, ConnectionsData>;

 private:
  bool getNameConnections(int group, const ModelVisitor::VisitData &data, int &srcId, int &destId,
                          double &value, const QString &separator) const;

  bool getRowConnections(int group, const ModelVisitor::VisitData &data) const;

  //! get connections data for unique id
  ConnectionsData &getConnections(const QString &str);

  const ConnectionsData &getConnections(int id) const;
  ConnectionsData &getConnections(int id);

  ConnectionsData &getConnections1(int id, const QString &str);

  Connection *addEdge(ConnectionsData &srcConnectionsData,
                      ConnectionsData &destConnectionsData, const OptReal &value) const;

  //! get unique index for string
  int getStringId(const QString &str) const;
  //! get string for unique index
  QString getIdString(int id) const;

  BBox nodeBBox(const ForceNodeP &node, Node *snode) const;
  BBox edgeBBox(const ForceEdgeP &edge, Edge *sedge) const;

  void updateMaxNodeValue();

  OptReal calcNodeValue(Node *node) const;
  OptReal calcNormalizedNodeValue(Node *node) const;
  OptReal calcScaledNodeValue(Node *node) const;

  QColor calcNodeFillColor(Node *node) const;

  //---

  Point forcePointToPlot(const Point &p) const;
  Point plotToForcePoint(const Point &p) const;

  //---

  bool hasEdgeWidth(Edge *edge) const;
  OptReal calcNormalizedEdgeWidth(Edge *edge) const;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  virtual NodeObj *createNodeObj(ForceNodeP node, const BBox &bbox) const;
  virtual EdgeObj *createEdgeObj(ForceEdgeP edge, const BBox &bbox) const;

 private Q_SLOTS:
  void busyButtonSlot(bool);

 protected:
  void updateBusyButton();
  void placeBusyButton();

  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using NodeMap         = std::map<int, ForceNodeP>;
  using ConnectionNodes = std::map<int, int>;
  using ForceDirected   = CQChartsForceDirected;
  using ForceDirectedP  = std::unique_ptr<ForceDirected>;
  using StringIndMap    = std::map<QString, int>;
  using IndStringMap    = std::map<int, QString>;

  // columns
  Column edgeWidthColumn_; //!< edge width column

  // animation data
  int         initSteps_    { 500 };  //!< initial steps
  int         animateSteps_ { 10 };   //!< animate steps
  mutable int numSteps_     { 0 };    //!< number of steps
  double      stepSize_     { 0.01 }; //!< step size

  // node data
  struct NodeDrawData {
    NodeShape    shape          { NodeShape::CIRCLE };  //!< node shape
    bool         scaled         { true };               //!< is node scaled
    Length       size           { Length::plot(0.2) };  //!< node size
    Length       minSize        { Length::pixel(32) };  //!< min node size
    bool         valueColored   { false };              //!< is node colored by value
    bool         valueLabel     { false };              //!< show value as label
    bool         mouseColoring  { false };              //!< is node edges colored on mouse over
    NodeEdgeType mouseColorType { NodeEdgeType::DEST }; //!< node coloring type
    bool         mouseValue     { false };              //!< show node value on mouse over
    bool         mouseEdgeColor { false };              //!< show mouse edges in node color
    QString      tipNameLabel;                          //!< tip label for node name
    QString      tipValueLabel;                         //!< tip label for node value
  };

  NodeDrawData nodeDrawData_; //!< node draw data

  NodeEdgeType nodeEdgeValueType_ { NodeEdgeType::DEST };
  bool         nodeUseEdgeValue_  { false };

  // edge data
  struct EdgeDrawData {
    EdgeShape shape         { EdgeShape::LINE };      //!< edge shape
    EdgeArrow arrow         { EdgeArrow::NONE };      //!< edge arrow
    bool      scaled        { true };                 //!< scale width by value
    Length    width         { Length::pixel(16) };    //!< max edge width
    Length    arrowWidth    { Length::percent(100) }; //!< edge arrow width
    bool      valueColored  { false };                //!< is edge colored by value
    bool      valueLabel    { false };                //!< show value as label
    bool      mouseColoring { false };                //!< is edge nodes colored on mouse over
    bool      mouseValue    { false };                //!< show edge value on mouse over
  };

  EdgeDrawData edgeDrawData_; //!< edge draw data

  double edgeMinWidth_ { 0.0 };

  // mouse inside/selected text visible
  bool insideTextVisible_   { false }; //!< is inside text visible (when text invisible)
  bool selectedTextVisible_ { false }; //!< is selected text visible (when text invisible)

  // mouse inside/selected text no elide
  bool insideTextNoElide_   { false }; //!< is inside text no elide (when text clipped)
  bool selectedTextNoElide_ { false }; //!< is selected text no elide (when text clipped)

  // mouse inside/selected text no scale
  bool insideTextNoScale_   { false }; //!< is inside text no scale (when text scaled)
  bool selectedTextNoScale_ { false }; //!< is selected text no scale (when text scaled)

  // busy button data
  bool showBusyButton_     { true }; //!< show busy button when animating
  bool autoHideBusyButton_ { true }; //!< auto hide busy button when not animating

  CQBusyButton *busyButton_ { nullptr }; //!< busy button

  // connection data
  IdConnectionsData idConnections_;             //!< id connections
  StringIndMap      nameIdMap_;                 //!< node name index map
  IndStringMap      idNameMap_;                 //!< node name index map
  NodeMap           nodes_;                     //!< force directed nodes
  ConnectionNodes   connectionNodes_;           //!< ids of force directed nodes
  int               maxGroup_          { 0 };   //!< max group
  double            maxNodeValue_      { 0.0 }; //!< max node value
  double            maxEdgeValue_      { 0.0 }; //!< max edge value
  double            maxEdgeWidthValue_ { 0.0 }; //!< max edge value

  ForceDirectedP forceDirected_; //!< force directed class

  // tip data
  QString edgeValueName_;

  // state
  bool   pressed_        { false }; //!< is pressed
  double rangeSize_      { 20.0 };  //!< range size
  double nodeMass_       { 1.0 };   //!< node mass
  double edgeScale_      { 1.0 };   //!< edge scale
  double edgeWidthScale_ { 1.0 };   //!< edge width scale
  int    maxNodeDepth_   { 0 };     //!< max node depth
  double minDelta_       { 0.001 }; //!< min delta
  int    maxSteps_       { -1 };    //!< max steps

  // placement
  double stiffness_ { 400.0 };
  double repulsion_ { 400.0 };
  double damping_   { 0.5 };

  double minSpringLength_ { 0.05 };
  double maxSpringLength_ { 5.00 };

  bool   unitRange_   { true };
  Range  forceRange_;
  double forceAspect_ { 1.0 };

  //---

  mutable std::mutex createMutex_; //!< create mutex

  //---

  using DrawTextDatas = std::vector<DrawTextData>;

  mutable DrawTextDatas drawTextDatas_;

  //---

  using EdgePaths  = std::map<int, QPainterPath>;
  using NodeShapes = std::map<int, NodeShapeBBox>;

  mutable EdgePaths  edgePaths_;
  mutable NodeShapes nodeShapes_;

  //---

  using InsideDrawEdges = std::map<ForceEdgeP, InsideDrawData>;
  using InsideDrawNodes = std::map<ForceNodeP, InsideDrawData>;

  mutable InsideDrawEdges insideDrawEdges_;
  mutable InsideDrawNodes insideDrawNodes_;

  mutable InsideDrawEdges insideEdgeData_;
  mutable InsideDrawNodes insideNodeData_;
};

//---

/*!
 * \brief Force Directed Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsForceDirectedPlotCustomControls : public CQChartsConnectionPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsForceDirectedPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addExtraColumnNames(QStringList &) override;
  void addExtraShowColumns(QStringList &) override;

  void addOptionsWidgets() override;

  virtual void addRunWidgets();

  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected Q_SLOTS:
  void runningSlot(int);
  void stepSlot();

 protected:
  CQChartsForceDirectedPlot* forceDirectedPlot_ { nullptr };

  FrameData optionsFrame_;

  QCheckBox* runningCheck_ { nullptr };
};

#endif
