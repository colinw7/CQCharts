#ifndef CQChartsForceDirectedPlot_H
#define CQChartsForceDirectedPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPaletteColor.h>
#include <CQChartsForceDirected.h>

class CQChartsForceDirectedPlotType : public CQChartsPlotType {
 public:
  CQChartsForceDirectedPlotType();

  QString name() const override { return "forcedirected"; }
  QString desc() const override { return "ForceDirected"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsForceDirectedPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     nodeColumn        READ nodeColumn         WRITE setNodeColumn        )
  Q_PROPERTY(int     connectionsColumn READ connectionsColumn  WRITE setConnectionsColumn )
  Q_PROPERTY(int     valueColumn       READ valueColumn        WRITE setValueColumn       )
  Q_PROPERTY(int     groupColumn       READ groupColumn        WRITE setGroupColumn       )
  Q_PROPERTY(int     nameColumn        READ nameColumn         WRITE setNameColumn        )
  Q_PROPERTY(bool    autoFit           READ isAutoFit          WRITE setAutoFit           )
  Q_PROPERTY(bool    running           READ isRunning          WRITE setRunning           )
  Q_PROPERTY(double  nodeRadius        READ nodeRadius         WRITE setNodeRadius        )
  Q_PROPERTY(QString nodeBorderColor   READ nodeBorderColorStr WRITE setNodeBorderColorStr)
  Q_PROPERTY(QString edgeColor         READ edgeColorStr       WRITE setEdgeColorStr      )
  Q_PROPERTY(double  edgeAlpha         READ edgeAlpha          WRITE setEdgeAlpha         )

 public:
  CQChartsForceDirectedPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsForceDirectedPlot();

  //---

  int nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(int i) { nodeColumn_ = i; updateRangeAndObjs(); }

  int connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(int i) { connectionsColumn_ = i; updateRangeAndObjs(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; updateRangeAndObjs(); }

  int groupColumn() const { return groupColumn_; }
  void setGroupColumn(int i) { groupColumn_ = i; updateRangeAndObjs(); }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  //---

  bool isAutoFit() const { return autoFit_; }
  void setAutoFit(bool b) { autoFit_ = b; }

  bool isRunning() const { return running_; }
  void setRunning(bool b) { running_ = b; }

  //---

  double nodeRadius() const { return nodeRadius_; }
  void setNodeRadius(double r) { nodeRadius_ = r; }

  QString nodeBorderColorStr() const { return nodeBorderColor_.colorStr(); }
  void setNodeBorderColorStr(const QString &s) { nodeBorderColor_.setColorStr(s); }

  QColor interpNodeBorderColor(int i, int n) const {
    return nodeBorderColor_.interpColor(this, i, n); }

  QString edgeColorStr() const { return edgeColor_.colorStr(); }
  void setEdgeColorStr(const QString &s) { edgeColor_.setColorStr(s); }

  QColor interpEdgeColor(int i, int n) const { return edgeColor_.interpColor(this, i, n); }

  double edgeAlpha() const { return edgeAlpha_; }
  void setEdgeAlpha(double r) { edgeAlpha_ = r; }

  //---

  void animateStep() override;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  bool mousePress  (const CQChartsGeom::Point &p, ModSelect modSelect) override;
  bool mouseMove   (const CQChartsGeom::Point &p, bool first=false) override;
  void mouseRelease(const CQChartsGeom::Point &p) override;

  void keyPress(int key, int modifier) override;

  bool tipText(const CQChartsGeom::Point &p, QString &tip) const override;

  //---

  void draw(QPainter *) override;

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

  int                   nodeColumn_        { -1 };    // node column
  int                   connectionsColumn_ { -1 };    // connections column
  int                   valueColumn_       { -1 };    // value column
  int                   groupColumn_       { -1 };    // group column
  int                   nameColumn_        { -1 };    // name column
  IdConnectionsData     idConnections_;               // id connections
  NodeMap               nodes_;                       // force directed nodes
  CQChartsForceDirected forceDirected_;               // force directed class
  bool                  autoFit_           { true };  // is auto fit
  bool                  running_           { true };  // is running
  bool                  pressed_           { false }; // is pressed
  double                rangeSize_         { 20.0 };  // range size
  double                nodeMass_          { 1.0 };   // node mass
  int                   initSteps_         { 100 };   // initial steps
  double                stepSize_          { 0.01 };  // step size
  CQChartsPaletteColor  nodeBorderColor_;             // node border color
  double                nodeRadius_        { 6.0 };   // node radius
  CQChartsPaletteColor  edgeColor_;                   // edge color
  double                edgeAlpha_         { 0.5 };   // edge alpha
};

#endif
