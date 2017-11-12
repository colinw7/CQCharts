#ifndef CQChartsForceDirectedPlot_H
#define CQChartsForceDirectedPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

class QTimer;

//------

#include <vector>
#include <map>
#include <cmath>

namespace Springy {
  class Node;

  // Vector
  class Vector {
   public:
    static Vector random() {
      return Vector(10.0*(randDouble() - 0.5), 10.0*(randDouble() - 0.5));
    }

    Vector(double x=0.0, double y=0.0) :
     x_(x), y_(y){
    }

    double x() const { return x_; }
    double y() const { return y_; }

    Vector add(const Vector &v2) const {
      return Vector(x_ + v2.x_, y_ + v2.y_);
    }

    Vector subtract(const Vector &v2) const {
      return Vector(x_ - v2.x_, y_ - v2.y_);
    }

    Vector multiply(double n) const {
      return Vector(x_*n, y_*n);
    }

    Vector divide(double n) const {
      if (n != 0.0)
        return Vector(x_/n, y_/n);
      else
        return Vector(0.0, 0.0);
    }

    double magnitude() const {
      return sqrt(x_*x_ + y_*y_);
    }

    Vector normal() const {
      return Vector(-y_, x_);
    }

    Vector normalise() {
      return divide(magnitude());
    }

   private:
    static double randDouble() {
      return (1.0*rand())/RAND_MAX;
    }

   private:
    double x_ { 0.0 };
    double y_ { 0.0 };
  };

  // Point
  class Point {
   public:
    Point(const Vector &position, double mass) :
     p_(position), mass_(mass), v_(), a_() {
    }

    const Vector &p() const { return p_; }
    void setP(const Vector &p) { p_ = p; }

    double mass() const { return mass_; }
    void setMass(double m) { mass_ = m; }

    const Vector &v() const { return v_; }
    const Vector &a() const { return a_; }

    void setV(const Vector &v) { v_ = v; }
    void setA(const Vector &a) { a_ = a; }

    void applyForce(const Vector &force) {
      a_ = a_.add(force.divide(mass()));
    }

   private:
    Vector p_;            // position
    double mass_ { 1.0 }; // mass
    Vector v_;            // velocity
    Vector a_;            // acceleration
  };

  //-----------

  // Spring
  class Spring {
   public:
    Spring(Point *point1, Point *point2, double length, double k) :
     point1_(point1), point2_(point2), length_(length), k_(k) {
    }

    Point *point1() const { return point1_; }
    Point *point2() const { return point2_; }

    double length() const { return length_; }

    double k() const { return k_; }

/*
  double distanceToPoint(const Point &point) {
    auto n  = point2.p.subtract(point1.p).normalise().normal();
    auto ac = point .p.subtract(point1.p);
    return abs(ac.x * n.x + ac.y * n.y);
  }
*/

   private:
    Point  *point1_ { nullptr };
    Point  *point2_ { nullptr };
    double  length_ { 1.0 }; // spring length at rest
    double  k_      { 1.0 }; // spring constant (See Hooke's law) .. how stiff the spring is
  };

  using NodePoint = std::pair<Node*,Point*>;

  //-----------

  // Node
  class Node {
   public:
    Node(int id) :
     id_(id), mass_(1.0), label_(), value_(0) {
      pos_ = Vector::random();
    }

    int id() const { return id_; }

    const Vector &position() const { return pos_; }
    void setPosition(const Vector &p) { pos_ = p; }

    double mass() const { return mass_; }
    void setMass(double m) { mass_ = m; }

    const std::string &label() const { return label_; }
    void setLabel(const std::string &label) { label_ = label; }

    double value() const { return value_; }
    void setValue(double v) { value_ = v; }

   private:
    int         id_     { 0 };
    Vector      pos_;            // position
    double      mass_   { 1.0 }; // mass
    std::string label_;          // label
    double      value_  { 0.0 }; // value
  };

  using Nodes = std::vector<Node*>;

  //-----------

  // Edge
  class Edge {
   public:
    Edge(int id, Node *source, Node *target) :
     id_(id), source_(source), target_(target), length_(1.0), value_(0) {
    }

    int id() const { return id_; }

    Node *source() const { return source_; }
    Node *target() const { return target_; }

    double length() const { return length_; }
    void setLength(double l) { length_ = l; }

    double value() const { return value_; }
    void setValue(double v) { value_ = v; }

   private:
    int    id_     { 0 };
    Node*  source_ { nullptr };
    Node*  target_ { nullptr };
    double length_ { 1.0 }; // length
    double value_  { 0.0 }; // value
  };

  using Edges = std::vector<Edge*>;

  //-----------

  // Graph
  class Graph {
   public:
    Graph() :
     nodeSet_(), nodes_(), edges_(), adjacency_(), nextNodeId_(0), nextEdgeId_(0) {
    }

   ~Graph() {
      for (auto &node : nodes_)
        delete node;

      for (auto &edge : edges_)
        delete edge;
    }

    Nodes nodes() const { return nodes_; }
    Edges edges() const { return edges_; }

    Node *getNode(int id) const {
      auto p = nodeSet_.find(id);

      if (p == nodeSet_.end())
        return nullptr;

      return (*p).second;
    }

    Node *addNode(Node *node) {
      auto p = nodeSet_.find(node->id());

      if (p == nodeSet_.end()) {
        nodes_.push_back(node);

        p = nodeSet_.insert(p, NodeSet::value_type(node->id(), node));
      }

      return (*p).second;
    }

    Edge *getEdge(int id) const {
      for (auto edge : edges_)
        if (edge->id() == id)
          return edge;

      return nullptr;
    }

    Edge *addEdge(Edge *edge) {
      bool exists = getEdge(edge->id());

      if (! exists)
        edges_.push_back(edge);

      //---

      exists = false;

      Edges &adjacencyEdges = adjacency_[edge->source()->id()][edge->target()->id()];

      for (auto edge1 : adjacencyEdges) {
        if (edge1->id() == edge->id()) {
          exists = true;
          break;
        }
      }

      if (! exists)
        adjacencyEdges.push_back(edge);

      //---

      return edge;
    }

    Node *newNode() {
      Node *node = new Node(nextNodeId_++);

      addNode(node);

      return node;
    }

    Edge *newEdge(Node *source, Node *target) {
      Edge *edge = new Edge(nextEdgeId_++, source, target);

      addEdge(edge);

      return edge;
    }

    // find the edges from node1 to node2
    Edges getEdges(Node *node1, Node *node2) {
      auto p = adjacency_.find(node1->id());

      if (p == adjacency_.end())
        return Edges();

      auto p1 = (*p).second.find(node2->id());

      if (p1 == (*p).second.end())
        return Edges();

      return (*p1).second;
    }

    // remove a node and it's associated edges from the graph
    void removeNode(Node *node) {
      auto p = nodeSet_.find(node->id());

      if (p != nodeSet_.end())
        nodeSet_.erase(p);

      for (Nodes::reverse_iterator pn = nodes_.rbegin(); pn != nodes_.rend(); ++pn) {
        if ((*pn)->id() == node->id()) {
          nodes_.erase(pn.base());
          break;
        }
      }

      detachNode(node);
    }

    // removes edges associated with a given node
    void detachNode(Node *node) {
      Edges tmpEdges = edges_;

      for (auto e : tmpEdges) {
        if (e->source()->id() == node->id() || e->target()->id() == node->id()) {
          removeEdge(e);
        }
      }
    }

    // remove a node and it's associated edges from the graph
    void removeEdge(Edge *edge) {
      for (Edges::reverse_iterator pe = edges_.rbegin(); pe != edges_.rend(); ++pe) {
        if ((*pe)->id() == edge->id()) {
          edges_.erase(pe.base());
          break;
        }
      }

      for (auto x : adjacency_) {
        for (auto y : x.second) {
          auto &edges = y.second;

          for (Edges::reverse_iterator pe = edges.rbegin(); pe != edges.rend(); ++pe) {
            if ((*pe)->id() == edge->id()) {
              edges_.erase(pe.base());
              break;
            }
          }

          if (edges.empty())
            x.second.erase(y.first);
        }

        // Clean up empty objects
        if (x.second.empty())
          adjacency_.erase(x.first);
      }
    }

   public:
    using NodeSet       = std::map<int,Node*>;
    using NodeEdges     = std::map<int,Edges>;
    using NodeNodeEdges = std::map<int,NodeEdges>;

   private:
    NodeSet       nodeSet_;
    Nodes         nodes_;
    Edges         edges_;
    NodeNodeEdges adjacency_;
    int           nextNodeId_ { -1 };
    int           nextEdgeId_ { -1 };
  };

  //-----------

  // Layout
  class Layout {
   public:
    Layout(Graph *graph, double stiffness, double repulsion, double damping) :
     graph_(graph), stiffness_(stiffness), repulsion_(repulsion), damping_(damping) {
    }

   ~Layout() {
      for (auto &nodePoint : nodePoints_)
        delete nodePoint.second;

      for (auto &edgeSpring : edgeSprings_)
        delete edgeSpring.second;
    }

    Graph *graph() const { return graph_; }

    Point *nodePoint(Node *node) const {
      auto p = nodePoints_.find(node->id());

      if (p == nodePoints_.end()) {
        Layout *th = const_cast<Layout *>(this);

        auto point = new Point(node->position(), node->mass());

        p = th->nodePoints_.insert(p, NodePoints::value_type(node->id(), point));
      }

      return (*p).second;
    }

    Spring *edgeSpring(Edge *edge) {
      auto p = edgeSprings_.find(edge->id());

      if (p == edgeSprings_.end()) {
        Spring *existingSpring = nullptr;

        const Edges &from = graph_->getEdges(edge->source(), edge->target());

        for (auto e : from) {
          auto p1 = edgeSprings_.find(e->id());

          if (p1 != edgeSprings_.end()) {
            existingSpring = (*p1).second;

            // MLK ?
            return new Spring(existingSpring->point1(), existingSpring->point2(), 0.0, 0.0);
          }
        }

        const Edges &to = graph_->getEdges(edge->target(), edge->source());

        for (auto e : to) { // to ?
          auto p1 = edgeSprings_.find(e->id());

          if (p1 != edgeSprings_.end()) {
            existingSpring = (*p1).second;

            // MLK ?
            return new Spring(existingSpring->point2(), existingSpring->point1(), 0.0, 0.0);
          }
        }

        edgeSprings_[edge->id()] =
          new Spring(nodePoint(edge->source()), nodePoint(edge->target()),
                     edge->length(), stiffness_);
      }

      return edgeSprings_[edge->id()];
    }

    // Physics stuff
    void applyCoulombsLaw() {
      for (auto n1 : graph_->nodes()) {
        auto point1 = nodePoint(n1);

        for (auto n2 : graph_->nodes()) {
          if (n1 == n2) continue;

          auto point2 = nodePoint(n2);

          if (point1 != point2) {
            auto d = point1->p().subtract(point2->p());

            // avoid massive forces at small distances (and divide by zero)
            double distance = d.magnitude() + 0.1;

            Vector direction = d.normalise();

            // apply force to each end point
            point1->applyForce(direction.multiply(repulsion_).divide(distance*distance* 0.5));
            point2->applyForce(direction.multiply(repulsion_).divide(distance*distance*-0.5));
          }
        }
      }
    }

    void applyHookesLaw() {
      for (auto edge : graph_->edges()) {
        auto spring = this->edgeSpring(edge);

        // the direction of the spring
        Vector d = spring->point2()->p().subtract(spring->point1()->p());

        double displacement = spring->length() - d.magnitude();

        Vector direction = d.normalise();

        // apply force to each end point
        spring->point1()->applyForce(direction.multiply(spring->k()*displacement*-0.5));
        spring->point2()->applyForce(direction.multiply(spring->k()*displacement* 0.5));
      }
    }

    void attractToCentre() {
      for (auto node : graph_->nodes()) {
        auto point = this->nodePoint(node);

        Vector direction = point->p().multiply(-1.0);

        point->applyForce(direction.multiply(repulsion_/50.0));
      }
    }

    void updateVelocity(double timestep) {
      for (auto node : graph_->nodes()) {
        auto point = this->nodePoint(node);

        // Is this, along with updatePosition below, the only places that your
        // integration code exist?
        point->setV(point->v().add(point->a().multiply(timestep)).multiply(damping_));
        point->setA(Vector(0,0));
      }
    }

    void updatePosition(double timestep) {
      for (auto node : graph_->nodes()) {
        auto point = this->nodePoint(node);

        // Same question as above; along with updateVelocity, is this all of
        // your integration code?
        point->setP(point->p().add(point->v().multiply(timestep)));
      }
    }

    // Calculate the total kinetic energy of the system
    double totalEnergy(double /*timestep*/) {
      double energy = 0.0;

      for (auto node : graph_->nodes()) {
        auto point = this->nodePoint(node);

        double speed = point->v().magnitude();

        energy += 0.5*point->mass()*speed*speed;
      }

      return energy;
    }

    void step(double t) {
      applyCoulombsLaw();
      applyHookesLaw();
      attractToCentre();
      updateVelocity(t);
      updatePosition(t);
    }

    // Find the nearest point to a particular position
    NodePoint nearest(const Vector &p) const {
      Node*  minNode     = nullptr;
      Point* minPoint    = nullptr;
      double minDistance = 0;

      for (auto n : graph_->nodes()) {
        auto point = this->nodePoint(n);

        double distance = point->p().subtract(p).magnitude();

        if (minDistance == 0 || distance < minDistance) {
          minNode     = n;
          minPoint    = point;
          minDistance = distance;
        }
      }

      return NodePoint(minNode, minPoint);
    }

    void calcRange(double &xmin, double &ymin, double &xmax, double &ymax) const {
      for (auto n : graph_->nodes()) {
        auto point = this->nodePoint(n);

        const Vector &p = point->p();

        xmin = std::min(xmin, p.x());
        ymin = std::min(ymin, p.y());
        xmax = std::max(xmax, p.x());
        ymax = std::max(ymax, p.y());
      }
    }

   private:
    using NodePoints  = std::map<int,Point*>;
    using EdgeSprings = std::map<int,Spring*>;

    Graph*      graph_              { nullptr };
    double      stiffness_          { 400.0 };   // spring stiffness constant
    double      repulsion_          { 400.0 };   // repulsion constant
    double      damping_            { 0.5 };     // velocity damping factor
    double      minEnergyThreshold_ { 0.0 };
    NodePoints  nodePoints_;                     // keep track of points associated with nodes
    EdgeSprings edgeSprings_;                    // keep track of springs associated with edges
  };
}

//------

class CForceDirected {
 public:
  CForceDirected() {
    graph_  = new Springy::Graph;
    layout_ = new Springy::Layout(graph_, stiffness_, repulsion_, damping_);
  }

 ~CForceDirected() {
    delete graph_;
    delete layout_;
  }

  void reset() {
    delete graph_;
    delete layout_;

    graph_  = new Springy::Graph;
    layout_ = new Springy::Layout(graph_, stiffness_, repulsion_, damping_);
  }

  Springy::Node *newNode() {
    return graph_->newNode();
  }

  Springy::Node *getNode(int id) {
    return graph_->getNode(id);
  }

  Springy::Edge *newEdge(Springy::Node *node1, Springy::Node *node2) {
    return graph_->newEdge(node1, node2);
  }

  Springy::Edges edges() const { return graph_->edges(); }
  Springy::Nodes nodes() const { return graph_->nodes(); }

  void step(double dt) {
    layout_->step(dt);
  }

  Springy::Spring *spring(Springy::Edge *edge) {
    return layout_->edgeSpring(edge);
  }

  Springy::Point *point(Springy::Node *node) {
    return layout_->nodePoint(node);
  }

  Springy::NodePoint nearest(const Springy::Vector &v) const {
    return layout_->nearest(v);
  }

  Springy::Node *currentNode() const { return currentNode_; }
  void setCurrentNode(Springy::Node *node) { currentNode_ = node; }

  Springy::Point *currentPoint() const { return currentPoint_; }
  void setCurrentPoint(Springy::Point *point) { currentPoint_ = point; }

  void calcRange(double &xmin, double &ymin, double &xmax, double &ymax) const {
    layout_->calcRange(xmin, ymin, xmax, ymax);
  }

 private:
  double           stiffness_    { 400.0 };
  double           repulsion_    { 400.0 };
  double           damping_      { 0.5 };
  Springy::Graph  *graph_        { nullptr };
  Springy::Layout *layout_       { nullptr };
  Springy::Node   *currentNode_  { nullptr };
  Springy::Point  *currentPoint_ { nullptr };
};

//------

class CQChartsForceDirectedPlotType : public CQChartsPlotType {
 public:
  CQChartsForceDirectedPlotType();

  QString name() const override { return "forcedirected"; }
  QString desc() const override { return "ForceDirected"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsForceDirectedPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(int    connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(int    groupColumn       READ groupColumn       WRITE setGroupColumn      )
  Q_PROPERTY(int    nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(bool   autoFit           READ isAutoFit         WRITE setAutoFit          )
  Q_PROPERTY(bool   running           READ isRunning         WRITE setRunning          )
  Q_PROPERTY(double nodeRadius        READ nodeRadius        WRITE setNodeRadius       )
  Q_PROPERTY(QColor nodeBorderColor   READ nodeBorderColor   WRITE setNodeBorderColor  )
  Q_PROPERTY(QColor edgeColor         READ edgeColor         WRITE setEdgeColor        )
  Q_PROPERTY(double edgeAlpha         READ edgeAlpha         WRITE setEdgeAlpha        )

 public:
  CQChartsForceDirectedPlot(CQChartsView *view, const ModelP &model);

  int nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(int i) { nodeColumn_ = i; update(); }

  int groupColumn() const { return groupColumn_; }
  void setGroupColumn(int i) { groupColumn_ = i; update(); }

  int connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(int i) { connectionsColumn_ = i; update(); }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  //---

  bool isAutoFit() const { return autoFit_; }
  void setAutoFit(bool b) { autoFit_ = b; }

  bool isRunning() const { return running_; }
  void setRunning(bool b) { running_ = b; }

  double nodeRadius() const { return nodeRadius_; }
  void setNodeRadius(double r) { nodeRadius_ = r; }

  const QColor &nodeBorderColor() const { return nodeBorderColor_; }
  void setNodeBorderColor(const QColor &v) { nodeBorderColor_ = v; }

  const QColor &edgeColor() const { return edgeColor_; }
  void setEdgeColor(const QColor &v) { edgeColor_ = v; }

  double edgeAlpha() const { return edgeAlpha_; }
  void setEdgeAlpha(double r) { edgeAlpha_ = r; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  bool mousePress  (const CQChartsGeom::Point &p) override;
  bool mouseMove   (const CQChartsGeom::Point &p, bool first=false) override;
  void mouseRelease(const CQChartsGeom::Point &p) override;

  void keyPress(int key) override;

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
  bool decodeConnections(const QString &str, ConnectionDataArray &connections);
  bool decodeConnection(const QString &pointStr, ConnectionData &connection);

 private slots:
  void animateSlot();

 private:
  using NodeMap = std::map<int,Springy::Node*>;

  int               nodeColumn_        { 0 };
  int               connectionsColumn_ { 1 };
  int               nameColumn_        { -1 };
  int               groupColumn_       { -1 };
  IdConnectionsData idConnections_;
  NodeMap           nodes_;
  QTimer*           timer_             { nullptr };
  CForceDirected    forceDirected_;
  bool              autoFit_           { true };
  bool              running_           { true };
  bool              pressed_           { false };
  int               tickLen_           { 30 };
  double            rangeSize_         { 20 };
  double            nodeMass_          { 1.0 };
  int               initSteps_         { 100 };
  double            stepSize_          { 0.01 };
  QColor            nodeColor_         { 0, 100, 255 };
  QColor            nodeBorderColor_   { 0, 0, 0 };
  double            nodeRadius_        { 6 };
  QColor            edgeColor_         { 0, 0, 0 };
  double            edgeAlpha_         { 0.5 };
};

#endif
