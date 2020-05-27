#ifndef CQChartsForceDirectedPlot_H
#define CQChartsForceDirectedPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsConnectionList.h>
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

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;

  //---

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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
  Q_PROPERTY(bool   running    READ isRunning  WRITE setRunning   )
  Q_PROPERTY(double nodeRadius READ nodeRadius WRITE setNodeRadius)
  Q_PROPERTY(double rangeSize  READ rangeSize  WRITE setRangeSize )

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node,node)

  // edge line
  Q_PROPERTY(bool edgeLinesValueWidth READ isEdgeLinesValueWidth WRITE setEdgeLinesValueWidth)

  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Edge,edge)

  // info
  Q_PROPERTY(int numNodes READ numNodes)
  Q_PROPERTY(int numEdges READ numEdges)

 public:
  CQChartsForceDirectedPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsForceDirectedPlot();

  //----

  // set/set is placement running
  bool isRunning() const { return running_; }
  void setRunning(bool b);

  // set/set node size (radius)
  double nodeRadius() const { return nodeRadius_; }
  void setNodeRadius(double r);

  // set/set use edge value for width
  bool isEdgeLinesValueWidth() const { return edgeLinesValueWidth_; }
  void setEdgeLinesValueWidth(bool b);

  // get/set range size
  double rangeSize() const { return rangeSize_; }
  void setRangeSize(double r);

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

  void initHierObjsAddConnection(const QString &srcStr, const CQChartsModelIndex &srcLinkInd,
                                 double srcTotal,
                                 const QString &destStr, const CQChartsModelIndex &destLinkInd,
                                 double destTotal, int depth) const;

  //---

  bool initLinkConnectionObjs() const;
  bool initTableObjs         () const;

  void postUpdateObjs() override;

  void addIdConnections() const;

  //---

  bool selectPress  (const Point &p, SelMod selMod) override;
  bool selectMove   (const Point &p, bool first=false) override;
  bool selectRelease(const Point &p) override;

  void keyPress(int key, int modifier) override;

  bool tipText(const Point &p, QString &tip) const override;

  //---

  void draw(QPainter *painter) override;

  void drawParts(QPainter *painter) const override;

  void drawDeviceParts(CQChartsPaintDevice *device) const override;

 private:
  using Connections = CQChartsConnectionList::Connections;

  struct ConnectionsData {
    int         node  { 0 };
    QModelIndex ind;
    QString     name;
    int         group { 0 };
    double      total { 0.0 };
    Connections connections;
  };

  using IdConnectionsData = std::map<int,ConnectionsData>;

 private:
  bool getNameConnections(int group, const ModelVisitor::VisitData &data, int &srcId, int &destId,
                          double &value, const QChar &separator) const;

  bool getRowConnections(int group, const ModelVisitor::VisitData &data) const;

  const ConnectionsData &getConnections(int id) const;
  ConnectionsData &getConnections(int id);

  int getStringId(const QString &str) const;

 private:
  using NodeMap         = std::map<int,Springy::Node*>;
  using ConnectionNodes = std::map<int,int>;
  using ForceDirected   = CQChartsForceDirected;
  using StringIndMap    = std::map<QString,int>;

  // data
  IdConnectionsData idConnections_;              //!< id connections
  StringIndMap      nameNodeMap_;                //!< node name index map
  NodeMap           nodes_;                      //!< force directed nodes
  ConnectionNodes   connectionNodes_;            //!< ids of force directed nodes
  int               maxGroup_       { 0 };       //!< max group
  double            maxValue_       { 0.0 };     //!< max value
  ForceDirected*    forceDirected_  { nullptr }; //!< force directed class

  // options
  bool   running_             { true }; //!< is running
  double nodeRadius_          { 6.0 };  //!< node radius (pixel)
  bool   edgeLinesValueWidth_ { true }; //!< use value for edge width
  int    initSteps_           { 100 };  //!< initial steps
  double stepSize_            { 0.01 }; //!< step size

  // state
  bool   pressed_    { false }; //!< is pressed
  double rangeSize_  { 20.0 };  //!< range size
  double nodeMass_   { 1.0 };   //!< node mass
  double widthScale_ { 1.0 };  //!< width scale
};

#endif
