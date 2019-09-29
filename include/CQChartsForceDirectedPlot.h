#ifndef CQChartsForceDirectedPlot_H
#define CQChartsForceDirectedPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsConnectionList.h>
#include <CQChartsForceDirected.h>

/*!
 * \brief Force Directed plot type
 * \ingroup Charts
 */
class CQChartsForceDirectedPlotType : public CQChartsPlotType {
 public:
  CQChartsForceDirectedPlotType();

  QString name() const override { return "forcedirected"; }
  QString desc() const override { return "ForceDirected"; }

  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool hasTitle() const override { return false; }

  bool hasAxes() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  bool canRectSelect() const override { return false; }

  QString description() const override;

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Force Directed Plot
 * \ingroup Charts
 */
class CQChartsForceDirectedPlot : public CQChartsPlot,
 public CQChartsObjNodeShapeData<CQChartsForceDirectedPlot>,
 public CQChartsObjEdgeLineData <CQChartsForceDirectedPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(CQChartsColumn nameColumn        READ nameColumn        WRITE setNameColumn       )

  Q_PROPERTY(CQChartsColumn namePairColumn READ namePairColumn WRITE setNamePairColumn)
  Q_PROPERTY(CQChartsColumn countColumn    READ countColumn    WRITE setCountColumn   )

  Q_PROPERTY(CQChartsColumn groupIdColumn READ groupIdColumn WRITE setGroupIdColumn)

  // options
  Q_PROPERTY(bool   running    READ isRunning  WRITE setRunning   )
  Q_PROPERTY(double nodeRadius READ nodeRadius WRITE setNodeRadius)

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node,node)

  // edge line
  Q_PROPERTY(bool edgeLinesValueWidth READ isEdgeLinesValueWidth WRITE setEdgeLinesValueWidth)

  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Edge,edge)

 public:
  CQChartsForceDirectedPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsForceDirectedPlot();

  //----

  // columns
  //  . node connections (node id, connection list, name)
  const CQChartsColumn &nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(const CQChartsColumn &c);

  const CQChartsColumn &connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(const CQChartsColumn &c);

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  //--

  //  . link connections (link name pair, link value)
  const CQChartsColumn &namePairColumn() const { return namePairColumn_; }
  void setNamePairColumn(const CQChartsColumn &c);

  const CQChartsColumn &countColumn() const { return countColumn_; }
  void setCountColumn(const CQChartsColumn &c);

  //--

  //  . group
  const CQChartsColumn &groupIdColumn() const { return groupIdColumn_; }
  void setGroupIdColumn(const CQChartsColumn &c);

  //----

  // is placement running
  bool isRunning() const { return running_; }
  void setRunning(bool b);

  // node size (radius)
  double nodeRadius() const { return nodeRadius_; }
  void setNodeRadius(double r);

  // use edge value for width
  bool isEdgeLinesValueWidth() const { return edgeLinesValueWidth_; }
  void setEdgeLinesValueWidth(bool b);

  //---

  bool isAnimated() const override { return true; }

  void animateStep() override;

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  void postUpdateObjs() override;

  //---

  bool selectPress  (const CQChartsGeom::Point &p, SelMod selMod) override;
  bool selectMove   (const CQChartsGeom::Point &p, bool first=false) override;
  bool selectRelease(const CQChartsGeom::Point &p) override;

  void keyPress(int key, int modifier) override;

  bool tipText(const CQChartsGeom::Point &p, QString &tip) const override;

  //---

  void draw(QPainter *painter) override;

  void drawParts(QPainter *painter) const override;

  void drawDeviceParts(CQChartsPaintDevice *device) const override;

 private:
  using Connections = CQChartsConnectionList::Connections;

  struct ConnectionsData {
    QModelIndex ind;
    int         node  { 0 };
    QString     name;
    int         group { 0 };
    Connections connections;
  };

  using IdConnectionsData = std::map<int,ConnectionsData>;

 private:
  bool getRowConnections(int group, const ModelVisitor::VisitData &data,
                         ConnectionsData &connections) const;

  bool getNameConnections(int group, const ModelVisitor::VisitData &data,
                          ConnectionsData &connections, int &destId, int &value) const;

  const ConnectionsData &getConnections(int id) const;
  ConnectionsData &getConnections(int id);

  void addConnections(int id, const ConnectionsData &connections) const;

  bool decodeConnections(const QString &str, Connections &connections) const;

  int getStringId(const QString &str) const;

 private:
  using NodeMap       = std::map<int,Springy::Node*>;
  using ForceDirected = CQChartsForceDirected;
  using StringIndMap  = std::map<QString,int>;

  CQChartsColumn    nodeColumn_;                                 //!< connection node column
  CQChartsColumn    connectionsColumn_;                          //!< connections node list column
  CQChartsColumn    nameColumn_;                                 //!< connection node name column
  CQChartsColumn    namePairColumn_;                             //!< link name pair column
  CQChartsColumn    countColumn_;                                //!< link count column
  CQChartsColumn    groupIdColumn_;                              //!< group id column
  ColumnType        connectionsColumnType_ { ColumnType::NONE }; //!< connection column type
  ColumnType        namePairColumnType_    { ColumnType::NONE }; //!< name pair column type
  IdConnectionsData idConnections_;                              //!< id connections
  NodeMap           nodes_;                                      //!< force directed nodes
  ForceDirected*    forceDirected_         { nullptr };          //!< force directed class
  StringIndMap      nameNodeMap_;                                //!< node name index map
  bool              running_               { true };             //!< is running
  bool              pressed_               { false };            //!< is pressed
  double            rangeSize_             { 20.0 };             //!< range size
  double            nodeMass_              { 1.0 };              //!< node mass
  bool              edgeLinesValueWidth_   { true };             //!< use value for edge width
  int               initSteps_             { 100 };              //!< initial steps
  double            stepSize_              { 0.01 };             //!< step size
  double            nodeRadius_            { 6.0 };              //!< node radius (pixel)
};

#endif
