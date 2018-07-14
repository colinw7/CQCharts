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

  QString description() const;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsForceDirectedPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(CQChartsColumn valueColumn       READ valueColumn       WRITE setValueColumn      )
  Q_PROPERTY(CQChartsColumn groupColumn       READ groupColumn       WRITE setGroupColumn      )
  Q_PROPERTY(CQChartsColumn nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(bool           running           READ isRunning         WRITE setRunning          )
  Q_PROPERTY(double         nodeRadius        READ nodeRadius        WRITE setNodeRadius       )
  Q_PROPERTY(CQChartsColor  nodeBorderColor   READ nodeBorderColor   WRITE setNodeBorderColor  )
  Q_PROPERTY(CQChartsColor  edgeColor         READ edgeColor         WRITE setEdgeColor        )
  Q_PROPERTY(double         edgeAlpha         READ edgeAlpha         WRITE setEdgeAlpha        )

 public:
  CQChartsForceDirectedPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsForceDirectedPlot();

  //---

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

  const CQChartsColor &nodeBorderColor() const { return nodeData_.border.color; }
  void setNodeBorderColor(const CQChartsColor &c) { nodeData_.border.color = c; }

  QColor interpNodeBorderColor(int i, int n) const {
    return nodeBorderColor().interpColor(this, i, n); }

  const CQChartsColor &edgeColor() const { return edgeStroke_.color; }
  void setEdgeColor(const CQChartsColor &c) { edgeStroke_.color = c; }

  QColor interpEdgeColor(int i, int n) const { return edgeColor().interpColor(this, i, n); }

  double edgeAlpha() const { return edgeStroke_.alpha; }
  void setEdgeAlpha(double r) { edgeStroke_.alpha = r; }

  //---

  void animateStep() override;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

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
    int node;
    int count;

    ConnectionData(int node=-1, int count=-1) :
     node(node), count(count) {
    }
  };

  using ConnectionDataArray = std::vector<ConnectionData>;

  struct ConnectionsData {
    int                 node;
    QString             name;
    int                 group;
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
  CQChartsStrokeData    edgeStroke_;                  // edge stroke
  CQChartsShapeData     nodeData_;                    // node stoke and fill
};

#endif
