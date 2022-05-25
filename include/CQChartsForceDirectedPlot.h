#ifndef CQChartsForceDirectedPlot_H
#define CQChartsForceDirectedPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsForceDirected.h>
#include <CForceDirected.h>

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

  bool hasAxes() const override { return false; }

  bool canProbe() const override { return false; }

  bool canRectSelect() const override { return false; }

  bool canEqualScale() const override { return true; }

  QString description() const override;

  //---

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
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

  // control
  Q_PROPERTY(bool   running      READ isRunning    WRITE setRunning)
  Q_PROPERTY(int    initSteps    READ initSteps    WRITE setInitSteps)
  Q_PROPERTY(int    animateSteps READ animateSteps WRITE setAnimateSteps)
  Q_PROPERTY(double stepSize     READ stepSize     WRITE setStepSize)
  Q_PROPERTY(double rangeSize    READ rangeSize    WRITE setRangeSize)
  Q_PROPERTY(int    numSteps     READ numSteps)

  // node data
  Q_PROPERTY(NodeShape      nodeShape         READ nodeShape           WRITE setNodeShape        )
  Q_PROPERTY(bool           nodeScaled        READ isNodeScaled        WRITE setNodeScaled       )
  Q_PROPERTY(CQChartsLength nodeSize          READ nodeSize            WRITE setNodeSize         )
  Q_PROPERTY(CQChartsLength minNodeSize       READ minNodeSize         WRITE setMinNodeSize      )
  Q_PROPERTY(bool           nodeValueColored  READ isNodeValueColored  WRITE setNodeValueColored )
  Q_PROPERTY(bool           nodeMouseColoring READ isNodeMouseColoring WRITE setNodeMouseColoring)

  // edge data
  Q_PROPERTY(EdgeShape      edgeShape         READ edgeShape           WRITE setEdgeShape        )
  Q_PROPERTY(bool           edgeArrow         READ isEdgeArrow         WRITE setEdgeArrow        )
  Q_PROPERTY(bool           edgeScaled        READ isEdgeScaled        WRITE setEdgeScaled       )
  Q_PROPERTY(CQChartsLength edgeWidth         READ edgeWidth           WRITE setEdgeWidth        )
  Q_PROPERTY(double         arrowWidth        READ arrowWidth          WRITE setArrowWidth       )
  Q_PROPERTY(bool           edgeValueColored  READ isEdgeValueColored  WRITE setEdgeValueColored )
  Q_PROPERTY(bool           edgeMouseColoring READ isEdgeMouseColoring WRITE setEdgeMouseColoring)


  // node/edge shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)

  // node/edge text style
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Edge, edge)

  // info
  Q_PROPERTY(int numNodes READ numNodes)
  Q_PROPERTY(int numEdges READ numEdges)

  Q_ENUMS(NodeShape)
  Q_ENUMS(EdgeShape)

 public:
  enum class NodeShape {
    NONE          = int(CQChartsNodeType::NONE),
    BOX           = int(CQChartsNodeType::BOX),
    CIRCLE        = int(CQChartsNodeType::CIRCLE),
    DOUBLE_CIRCLE = int(CQChartsNodeType::DOUBLE_CIRCLE)
  };

  enum class EdgeShape {
    NONE        = int(CQChartsEdgeType::NONE),
    ARC         = int(CQChartsEdgeType::ARC),
    RECTILINEAR = int(CQChartsEdgeType::RECTILINEAR),
    LINE        = int(CQChartsEdgeType::LINE)
  };

  using Length    = CQChartsLength;
  using Color     = CQChartsColor;
  using Alpha     = CQChartsAlpha;
  using Angle     = CQChartsAngle;
  using PenBrush  = CQChartsPenBrush;
  using PenData   = CQChartsPenData;
  using BrushData = CQChartsBrushData;
  using ColorInd  = CQChartsUtil::ColorInd;

 private:
  struct Connection;
  struct ConnectionsData;

  using Node = CQChartsSpringyNode;
  using Edge = CQChartsSpringyEdge;

  struct NodeShapeBBox {
    Node::Shape shape;
    BBox        bbox;
  };

 public:
  CQChartsForceDirectedPlot(View *view, const ModelP &model);
 ~CQChartsForceDirectedPlot();

  //---

  void init() override;
  void term() override;

  //---

  bool isBufferLayers() const override { return false; }

  //----

  //! get/set is placement running
  bool isRunning() const { return running_; }
  void setRunning(bool b);

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

  //----

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

  //! get/set node edges colored on mouse over
  bool isNodeMouseColoring() const { return nodeDrawData_.mouseColoring; }
  void setNodeMouseColoring(bool b);

  //---

  //! get/set edge shape
  const EdgeShape &edgeShape() const { return edgeDrawData_.shape; }
  void setEdgeShape(const EdgeShape &s);

  //! get/set has arrow
  bool isEdgeArrow() const { return edgeDrawData_.arrow; }
  void setEdgeArrow(bool b);

  //! get/set is edge scaled
  bool isEdgeScaled() const { return edgeDrawData_.scaled; }
  void setEdgeScaled(bool b);

  //! get/set edge width (in pixels)
  const Length &edgeWidth() const { return edgeDrawData_.width; }
  void setEdgeWidth(const Length &l);

  //! get/set edge directed arrow width
  double arrowWidth() const { return edgeDrawData_.arrowWidth; }
  void setArrowWidth(double r);

  //! get/set edge value colored
  bool isEdgeValueColored() const { return edgeDrawData_.valueColored; }
  void setEdgeValueColored(bool b);

  //! get/set edge nodes colored on mouse over
  bool isEdgeMouseColoring() const { return edgeDrawData_.mouseColoring; }
  void setEdgeMouseColoring(bool b);

  //---

  double maxNodeValue() const { return maxNodeValue_; }
  double maxEdgeValue() const { return maxEdgeValue_; }

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

  Range calcRange() const override;

  //--

  bool createObjs(PlotObjs &objs) const override;

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

  //---

  void setCurrentNode(const Point &p);
  void resetCurrentNode();

  bool selectAt(const Point &p);

  bool updateInside(const Point &p);

  void updateSelText();

  //---

  bool keyPress(int key, int modifier) override;

  bool plotTipText(const Point &p, QString &tip, bool single) const override;

  void nodeTipText(Node *node, CQChartsTableTip &tableTip) const;
  void edgeTipText(Edge *edge, CQChartsTableTip &tableTip) const;

  void nearestNodeEdge(const Point &p, Node* &insideNode, Edge* &insideEdge) const;

  //---

  void drawParts(QPainter *painter) const override;

  void drawDeviceParts(PaintDevice *device) const;

  void drawEdge(PaintDevice *device, const CForceDirected::EdgeP &edge, Edge *sedge,
                const ColorInd &colorInd) const;
  void drawNode(PaintDevice *device, const CForceDirected::NodeP &node, Node *snode,
                const ColorInd &colorInd) const;

  void drawNodeShape(PaintDevice *device, const NodeShapeBBox &nodeShape) const;

  void drawNodeEdges(PaintDevice *device, const CForceDirected::NodeP &node) const;
  void drawEdgeNodes(PaintDevice *device, const CForceDirected::EdgeP &edge) const;

  void postResize() override;

  //---

  QString calcNodeLabel(Node *nodes) const;

  //---

  void execAnimateStep();

  void doAutoFit();

  void autoFitUpdate() override;

 private:
  // connection between nodes (edge)
  struct Connection {
    int         srcNode   { -1 };
    int         destNode  { -1 };
    OptReal     value;
    QString     label;
    EdgeShape   shapeType { EdgeShape::NONE }; //!< edge shape
    Color       fillColor;
    QModelIndex ind;

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
    Color       fillColor;                        //!< fillColor
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

  Node::Shape calcNodeShape(Node *snode) const;
  Edge::Shape calcEdgeShape(Edge *sedge) const;

  BBox nodeBBox(const CForceDirected::NodeP &node, Node *snode) const;

  OptReal calcNodeValue(Node *node) const;
  OptReal calcNormalizedNodeValue(Node *node) const;
  OptReal calcScaledNodeValue(Node *node) const;

  QColor calcNodeFillColor(Node *node) const;

  OptReal calcNormalizedEdgeValue(Edge *edge) const;

  //---

  bool addMenuItems(QMenu *menu) override;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using NodeP           = CForceDirected::NodeP;
  using EdgeP           = CForceDirected::EdgeP;
  using NodeMap         = std::map<int, NodeP>;
  using ConnectionNodes = std::map<int, int>;
  using ForceDirected   = CQChartsForceDirected;
  using ForceDirectedP  = std::unique_ptr<ForceDirected>;
  using StringIndMap    = std::map<QString, int>;
  using IndStringMap    = std::map<int, QString>;

  // animation data
  bool        running_      { true }; //!< is running
  int         initSteps_    { 500 };  //!< initial steps
  int         animateSteps_ { 10 };   //!< animate steps
  mutable int numSteps_     { 0 };    //!< number of steps
  double      stepSize_     { 0.01 }; //!< step size

  // node data
  struct NodeDrawData {
    NodeShape shape         { NodeShape::CIRCLE }; //!< node shape
    bool      scaled        { true };              //!< is node scaled
    Length    size          { Length::plot(1) };   //!< node size
    Length    minSize       { Length::plot(0.1) }; //!< min node size
    bool      valueColored  { false };             //!< is node colored by value
    bool      mouseColoring { false };             //!< is node edges colored on mouse over
  };

  NodeDrawData nodeDrawData_; //!< node draw data

  // edge data
  struct EdgeDrawData {
    EdgeShape shape         { EdgeShape::LINE };   //!< edge shape
    bool      arrow         { false };             //!< edge arrow
    bool      scaled        { true };              //!< scale width by value
    Length    width         { Length::pixel(16) }; //!< max edge width
    double    arrowWidth    { 1.0 };               //!< edge arrow size factorr
    bool      valueColored  { false };             //!< is edge colored by value
    bool      mouseColoring { false };             //!< is edge nodes colored on mouse over
  };

  EdgeDrawData edgeDrawData_; //!< edge draw data

  // connection data
  IdConnectionsData idConnections_;          //!< id connections
  StringIndMap      nameIdMap_;              //!< node name index map
  IndStringMap      idNameMap_;              //!< node name index map
  NodeMap           nodes_;                  //!< force directed nodes
  ConnectionNodes   connectionNodes_;        //!< ids of force directed nodes
  int               maxGroup_       { 0 };   //!< max group
  double            maxNodeValue_   { 0.0 }; //!< max node value
  double            maxEdgeValue_   { 0.0 }; //!< max edge value

  ForceDirectedP forceDirected_; //!< force directed class

  // state
  bool   pressed_      { false }; //!< is pressed
  double rangeSize_    { 20.0 };  //!< range size
  double nodeMass_     { 1.0 };   //!< node mass
  double edgeScale_    { 1.0 };   //!< edge scale
  int    maxNodeDepth_ { 0 };     //!< max node depth

  mutable std::mutex createMutex_; //!< create mutex

  //---

  using EdgePaths  = std::map<int, QPainterPath>;
  using NodeShapes = std::map<int, NodeShapeBBox>;

  mutable EdgePaths  edgePaths_;
  mutable NodeShapes nodeShapes_;
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

  void setPlot(CQChartsPlot *plot) override;

 public slots:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addOptionsWidgets() override;

  virtual void addRunWidgets();

  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected slots:
  void runningSlot(int);
  void stepSlot();

 protected:
  CQChartsForceDirectedPlot* plot_ { nullptr };

  FrameData optionsFrame_;

  QCheckBox* runningCheck_ { nullptr };
};

#endif
