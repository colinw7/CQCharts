#ifndef CQChartsForceDirectedPlot_H
#define CQChartsForceDirectedPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsForceDirected.h>

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

  bool hasTitle() const override { return false; }

  bool hasAxes() const override { return false; }

  bool canProbe() const override { return false; }

  bool canRectSelect() const override { return false; }

  QString description() const override;

  //---

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Spring Node Customization
 * \ingroup Charts
 */
class CQChartsSpringyNode : public Springy::Node {
 public:
  using OptReal = CQChartsOptReal;

 public:
  CQChartsSpringyNode(int id) :
   Springy::Node(id) {
  }

  int group() const { return group_; }
  void setGroup(int i) { group_ = i; }

  const OptReal &nodeValue() const { return nodeValue_; }
  void setNodeValue(const OptReal &v) { nodeValue_ = v; }

 private:
  int     group_ { -1 };
  OptReal nodeValue_;
};

/*!
 * \brief Spring Edge Customization
 * \ingroup Charts
 */
class CQChartsSpringyEdge : public Springy::Edge {
 public:
  CQChartsSpringyEdge(int id, Springy::Node *node1, Springy::Node *node2) :
   Springy::Edge(id, node1, node2) {
  }
};

//---

/*!
 * \brief Force Directed Plot
 * \ingroup Charts
 */
class CQChartsForceDirectedPlot : public CQChartsConnectionPlot,
 public CQChartsObjNodeShapeData<CQChartsForceDirectedPlot>,
 public CQChartsObjEdgeLineData <CQChartsForceDirectedPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(bool   running      READ isRunning    WRITE setRunning     )
  Q_PROPERTY(double nodeRadius   READ nodeRadius   WRITE setNodeRadius  )
  Q_PROPERTY(bool   nodeScaled   READ isNodeScaled WRITE setNodeScaled  )
  Q_PROPERTY(double rangeSize    READ rangeSize    WRITE setRangeSize   )
  Q_PROPERTY(double maxLineWidth READ maxLineWidth WRITE setMaxLineWidth)

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)

  // edge line
  Q_PROPERTY(bool edgeLinesValueWidth READ isEdgeLinesValueWidth WRITE setEdgeLinesValueWidth)

  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Edge, edge)

  // info
  Q_PROPERTY(int numNodes READ numNodes)
  Q_PROPERTY(int numEdges READ numEdges)

 public:
  using Length   = CQChartsLength;
  using Alpha    = CQChartsAlpha;
  using ColorInd = CQChartsUtil::ColorInd;
  using PenBrush = CQChartsPenBrush;

 public:
  CQChartsForceDirectedPlot(View *view, const ModelP &model);
 ~CQChartsForceDirectedPlot();

  //---

  void init() override;
  void term() override;

  //----

  // get/set is placement running
  bool isRunning() const { return running_; }
  void setRunning(bool b);

  // get/set node size (radius)
  double nodeRadius() const { return nodeRadius_; }
  void setNodeRadius(double r);

  // get/set node scaled
  bool isNodeScaled() const { return nodeScaled_; }
  void setNodeScaled(bool b);

  // get/set use edge value for width
  bool isEdgeLinesValueWidth() const { return edgeLinesValueWidth_; }
  void setEdgeLinesValueWidth(bool b);

  // get/set range size
  double rangeSize() const { return rangeSize_; }
  void setRangeSize(double r);

  // get/set max line width
  double maxLineWidth() const { return maxLineWidth_; }
  void setMaxLineWidth(double r);

  //---

  double maxValue() const { return maxValue_; }

  double maxDataValue() const { return maxDataValue_; }

  //---

  int numNodes() const;
  int numEdges() const;

  //---

  bool isAnimated() const override { return true; }

  void animateStep() override;

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool initHierObjs() const;

  void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;
  void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;

  void initHierObjsAddConnection(const QString &srcStr, const ModelIndex &srcLinkInd,
                                 double srcTotal,
                                 const QString &destStr, const ModelIndex &destLinkInd,
                                 double destTotal, int depth) const;

  //---

  bool initPathObjs() const;

  void addPathValue(const PathData &pathData) const override;

  void propagatePathValues();

  //---

  bool initFromToObjs() const;

  void addFromToValue(const FromToData &fromToData) const override;

  //---

  bool initLinkConnectionObjs() const;

  void addLinkConnection(const LinkConnectionData &) const override { }

  bool initTableObjs() const;

  //---

  void postUpdateObjs() override;

  void filterObjs();

  void addIdConnections() const;

  //---

  bool selectPress  (const Point &p, SelMod selMod) override;
  bool selectMove   (const Point &p, Constraints constraints, bool first=false) override;
  bool selectRelease(const Point &p) override;

  void keyPress(int key, int modifier) override;

  bool plotTipText(const Point &p, QString &tip, bool single) const override;

  //---

  void draw(QPainter *painter) override;

  void drawParts(QPainter *painter) const override;

  void drawDeviceParts(PaintDevice *device) const override;

 private:
  // connection between nodes (edge)
  struct Connection {
    int     node  { -1 };
    OptReal value;

    Connection() = default;

    Connection(int node, double value) :
     node(node), value(value) {
    }
  };

  using Connections = std::vector<Connection>;

  // node connection data
  struct ConnectionsData {
    QModelIndex ind;                   //!< model index
    int         node         { 0 };    //!< unique index
    QString     name;                  //!< name
    QString     label;                 //!< label
    int         group        { 0 };    //!< group
    OptReal     value;                 //!< value
    OptReal     total;                 //!< total
    int         depth        { -1 };   //!< depth
    bool        visible      { true }; //!< is visible
    int         parentId     { -1 };   //!< parent
    Connections connections;           //!< connections
  };

  using IdConnectionsData = std::map<int, ConnectionsData>;

 private:
  bool getNameConnections(int group, const ModelVisitor::VisitData &data, int &srcId, int &destId,
                          double &value, const QChar &separator) const;

  bool getRowConnections(int group, const ModelVisitor::VisitData &data) const;

  //! get connections data for unique id
  ConnectionsData &getConnections(const QString &str);

  const ConnectionsData &getConnections(int id) const;
  ConnectionsData &getConnections(int id);

  void addEdge(ConnectionsData &srcConnectionsData,
               ConnectionsData &destConnectionsData, double value) const;

  //! get unique index for string
  int getStringId(const QString &str) const;

 private:
  using Node            = CQChartsSpringyNode;
  using NodeMap         = std::map<int, Node *>;
  using ConnectionNodes = std::map<int, int>;
  using ForceDirected   = CQChartsForceDirected;
  using StringIndMap    = std::map<QString, int>;

  // options
  bool   running_             { true }; //!< is running
  double nodeRadius_          { 6.0 };  //!< node radius (pixel)
  bool   nodeScaled_          { true }; //!< node radius scaled
  bool   edgeLinesValueWidth_ { true }; //!< use value for edge width
  int    initSteps_           { 100 };  //!< initial steps
  double stepSize_            { 0.01 }; //!< step size
  double maxLineWidth_        { 8.0 };  //!< max line width

  // data
  IdConnectionsData idConnections_;              //!< id connections
  StringIndMap      nameNodeMap_;                //!< node name index map
  NodeMap           nodes_;                      //!< force directed nodes
  ConnectionNodes   connectionNodes_;            //!< ids of force directed nodes
  int               maxGroup_       { 0 };       //!< max group
  double            maxValue_       { 0.0 };     //!< max connection value
  double            maxDataValue_   { 0.0 };     //!< max data value
  ForceDirected*    forceDirected_  { nullptr }; //!< force directed class

  // state
  bool   pressed_      { false }; //!< is pressed
  double rangeSize_    { 20.0 };  //!< range size
  double nodeMass_     { 1.0 };   //!< node mass
  double widthScale_   { 1.0 };   //!< width scale
  int    maxNodeDepth_ { 0 };     //!< max node depth
};

#endif
