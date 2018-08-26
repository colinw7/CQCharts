#ifndef CQChartsForceDirectedPlot_H
#define CQChartsForceDirectedPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsForceDirected.h>

class CQChartsForceDirectedPlotType : public CQChartsPlotType {
 public:
  CQChartsForceDirectedPlotType();

  QString name() const override { return "forcedirected"; }
  QString desc() const override { return "ForceDirected"; }

  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsForceDirectedPlot : public CQChartsPlot,
 public CQChartsPlotNodeShapeData<CQChartsForceDirectedPlot>,
 public CQChartsPlotEdgeLineData <CQChartsForceDirectedPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(CQChartsColumn valueColumn       READ valueColumn       WRITE setValueColumn      )
  Q_PROPERTY(CQChartsColumn groupColumn       READ groupColumn       WRITE setGroupColumn      )
  Q_PROPERTY(CQChartsColumn nameColumn        READ nameColumn        WRITE setNameColumn       )

  // options
  Q_PROPERTY(bool   running    READ isRunning  WRITE setRunning   )
  Q_PROPERTY(double nodeRadius READ nodeRadius WRITE setNodeRadius)

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node,node)

  // edge line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Edge,edge)

 public:
  CQChartsForceDirectedPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsForceDirectedPlot();

  //---

  // columns
  const CQChartsColumn &nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(const CQChartsColumn &c) { nodeColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(const CQChartsColumn &c) {
    connectionsColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c) { valueColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const CQChartsColumn &c) { groupColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c) { nameColumn_ = c; updateRangeAndObjs(); }

  //---

  bool isRunning() const { return running_; }
  void setRunning(bool b) { running_ = b; }

  //---

  double nodeRadius() const { return nodeRadius_; }
  void setNodeRadius(double r) { nodeRadius_ = r; }

  //---

  void animateStep() override;

  //---

  void addProperties() override;

  void calcRange() override;

  bool initObjs() override;

  //---

  bool selectPress  (const CQChartsGeom::Point &p, ModSelect modSelect) override;
  bool selectMove   (const CQChartsGeom::Point &p, bool first=false) override;
  bool selectRelease(const CQChartsGeom::Point &p) override;

  void keyPress(int key, int modifier) override;

  bool tipText(const CQChartsGeom::Point &p, QString &tip) const override;

  //---

  void drawParts(QPainter *painter) override;

 private:
  struct ConnectionData {
    int node  { 0 };
    int count { 0 };

    ConnectionData(int node=-1, int count=-1) :
     node(node), count(count) {
    }
  };

  using ConnectionDataArray = std::vector<ConnectionData>;

  struct ConnectionsData {
    int                 node  { 0 };
    QString             name;
    int                 group { 0 };
    QModelIndex         ind;
    ConnectionDataArray connections;
  };

  using IdConnectionsData = std::map<int,ConnectionsData>;

 private:
  ConnectionsData &getConnections(int id);

  void addConnections(int id, const ConnectionsData &connections);

  bool decodeConnections(const QString &str, ConnectionDataArray &connections);
  bool decodeConnection(const QString &pointStr, ConnectionData &connection);

 private:
  using NodeMap = std::map<int,Springy::Node*>;

  CQChartsColumn        nodeColumn_;                  // node column
  CQChartsColumn        connectionsColumn_;           // connections column
  CQChartsColumn        valueColumn_;                 // value column
  CQChartsColumn        groupColumn_;                 // group column
  CQChartsColumn        nameColumn_;                  // name column
  IdConnectionsData     idConnections_;               // id connections
  NodeMap               nodes_;                       // force directed nodes
  CQChartsForceDirected forceDirected_;               // force directed class
  bool                  running_           { true };  // is running
  bool                  pressed_           { false }; // is pressed
  double                rangeSize_         { 20.0 };  // range size
  double                nodeMass_          { 1.0 };   // node mass
  int                   initSteps_         { 100 };   // initial steps
  double                stepSize_          { 0.01 };  // step size
  double                nodeRadius_        { 6.0 };   // node radius
};

#endif
