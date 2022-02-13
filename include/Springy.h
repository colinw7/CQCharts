#ifndef Springy_H
#define Springy_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cmath>
#include <cassert>

namespace Springy {
  class Node;
  using NodeP = std::shared_ptr<Node>;

  /*!
   * \brief Vector
   */
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
      return std::hypot(x_, y_);
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

  /*!
   * \brief Point
   */
  class Point {
   public:
    Point(const Vector &position=Vector(), double mass=1.0) :
     p_(position), mass_(mass) {
    }

    const Vector &p() const { return p_; }
    void setP(const Vector &p) { p_ = p; }

    double mass() const { return mass_; }
    void setMass(double m) { mass_ = m; }

    const Vector &v() const { return v_; }
    void setV(const Vector &v) { v_ = v; }

    const Vector &a() const { return a_; }
    void setA(const Vector &a) { a_ = a; }

    bool isFixed() const { return fixed_; }
    void setFixed(bool b) { fixed_ = b; }

    void applyForce(const Vector &force) {
      a_ = a_.add(force.divide(mass()));
    }

   private:
    Vector p_;               //!< position
    double mass_ { 1.0 };    //!< mass
    Vector v_;               //!< velocity
    Vector a_;               //!< acceleration
    bool   fixed_ { false }; //!< is fixed
  };

  using PointP = std::shared_ptr<Point>;

  //-----------

  /*!
   * \brief Spring
   */
  class Spring {
   public:
    Spring(Point *point1=nullptr, Point *point2=nullptr, double length=1.0, double k=1.0) :
     point1_(point1), point2_(point2), length_(length), k_(k) {
    }

    PointP point1() const { return point1_; }
    void setPoint1(PointP p) { point1_ = p; }

    PointP point2() const { return point2_; }
    void setPoint2(PointP p) { point2_ = p; }

    double length() const { return length_; }
    void setLength(double l) { length_ = l; }

    double k() const { return k_; }
    void setK(double k) { k_ = k; }

/*
    double distanceToPoint(const Point &point) {
      auto n  = point2.p.subtract(point1.p).normalise().normal();
      auto ac = point .p.subtract(point1.p);
      return abs(ac.x * n.x + ac.y * n.y);
    }
*/

   private:
    PointP point1_;         //!< start point
    PointP point2_;         //!< end point
    double length_ { 1.0 }; //!< spring length at rest
    double k_      { 1.0 }; //!< spring constant (See Hooke's law) .. how stiff the spring is
  };

  using SpringP = std::shared_ptr<Spring>;

  using NodePoint = std::pair<NodeP, PointP>;

  //-----------

  /*!
   * \brief Node
   */
  class Node {
   public:
    explicit Node(int id) :
     id_(id) {
      pos_ = Vector::random();
    }

    virtual ~Node() { }

    int id() const { return id_; }

    const Vector &position() const { return pos_; }
    void setPosition(const Vector &p) { pos_ = p; }

    double mass() const { return mass_; }
    void setMass(double m) { mass_ = m; }

    const std::string &label() const { return label_; }
    void setLabel(const std::string &label) { label_ = label; }

    double value() const { return value_; }
    void setValue(double v) { value_ = v; }

    bool isFixed() const { return fixed_; }
    void setFixed(bool b) { fixed_ = b; }

   private:
    int         id_     { 0 };     //!< id
    Vector      pos_;              //!< position
    double      mass_   { 1.0 };   //!< mass
    std::string label_;            //!< label
    double      value_  { 0.0 };   //!< value
    bool        fixed_  { false }; //!< is fixed
  };

  using Nodes = std::vector<NodeP>;

  //-----------

  /*!
   * \brief Edge
   */
  class Edge {
   public:
    Edge(int id, NodeP source=NodeP(), NodeP target=NodeP()) :
     id_(id), source_(source), target_(target) {
    }

    virtual ~Edge() { }

    int id() const { return id_; }

    NodeP source() const { return source_; }
    void setSource(NodeP n) { source_ = n; }

    NodeP target() const { return target_; }
    void setTarget(NodeP n) { target_ = n; }

    double length() const { return length_; }
    void setLength(double l) { length_ = l; }

    double value() const { return value_; }
    void setValue(double v) { value_ = v; }

   private:
    int    id_     { 0 };   //!< id
    NodeP  source_;         //!< source node
    NodeP  target_;         //!< target node
    double length_ { 1.0 }; //!< length
    double value_  { 0.0 }; //!< value
  };

  using EdgeP = std::shared_ptr<Edge>;
  using Edges = std::vector<EdgeP>;

  //-----------

  /*!
   * \brief Graph
   */
  class Graph {
   public:
    Graph() { }

    virtual ~Graph() { }

    Nodes nodes() const { return nodes_; }
    Edges edges() const { return edges_; }

    NodeP getNode(int id) const {
      auto p = nodeSet_.find(id);

      if (p == nodeSet_.end())
        return nullptr;

      return (*p).second;
    }

    void addNode(NodeP node) {
      auto p = nodeSet_.find(node->id());
      assert(p == nodeSet_.end());

      nodes_.push_back(node);

      nodeSet_.insert(p, NodeSet::value_type(node->id(), node));
    }

    EdgeP getEdge(int id) const {
      for (auto edge : edges_) {
        if (edge->id() == id)
          return edge;
      }

      return EdgeP();
    }

    void addEdge(EdgeP edge) {
      assert(edge);

      auto eedge = getEdge(edge->id());

      if (! eedge)
        edges_.push_back(edge);

      //---

      bool exists = false;

      Edges &adjacencyEdges = adjacency_[edge->source()->id()][edge->target()->id()];

      for (auto &edge1 : adjacencyEdges) {
        if (edge1->id() == edge->id()) {
          exists = true;
          break;
        }
      }

      if (! exists)
        adjacencyEdges.push_back(edge);
    }

    NodeP newNode() {
      auto node = makeNode();

      addNode(node);

      return node;
    }

    EdgeP newEdge(NodeP source, NodeP target) {
      assert(source && target);

      auto edge = makeEdge();

      edge->setSource(source);
      edge->setTarget(target);

      addEdge(edge);

      return edge;
    }

    //------

    virtual NodeP makeNode() const {
      return NodeP(new Node(nextNodeId_++));
    }

    virtual EdgeP makeEdge() const {
      return EdgeP(new Edge(nextEdgeId_++));
    }

    virtual PointP makePoint() const {
      return PointP(new Point());
    }

    virtual SpringP makeSpring() const {
      return SpringP(new Spring());
    }

    //------

    // find the edges from node1 to node2
    Edges getEdges(NodeP node1, NodeP node2) {
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
          removeEdge(e.get());
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
              edges.erase(pe.base());
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
    using NodeSet       = std::map<int, NodeP>;
    using NodeEdges     = std::map<int, Edges>;
    using NodeNodeEdges = std::map<int, NodeEdges>;

   private:
    NodeSet       nodeSet_;
    Nodes         nodes_;
    Edges         edges_;
    NodeNodeEdges adjacency_;

   protected:
    mutable int nextNodeId_ { 0 };
    mutable int nextEdgeId_ { 0 };
  };

  using GraphP = std::shared_ptr<Graph>;

  //-----------

  /*!
   * \brief Layout
   */
  class Layout {
   public:
    Layout(Graph *graph, double stiffness, double repulsion, double damping) :
     graph_(graph), stiffness_(stiffness), repulsion_(repulsion), damping_(damping) {
    }

   ~Layout() { }

    Graph *graph() const { return graph_; }

    PointP nodePoint(NodeP node) const {
      auto *th = const_cast<Layout *>(this);

      auto p = th->nodePoints_.find(node->id());

      if (p == th->nodePoints_.end()) {
        auto point = graph_->makePoint();

        point->setP   (node->position());
        point->setMass(node->mass());

        if (node->isFixed())
          point->setFixed(true);

        p = th->nodePoints_.insert(p, NodePoints::value_type(node->id(), point));
      }

      return (*p).second;
    }

    SpringP edgeSpring(EdgeP edge, bool &isTemp) {
      isTemp = false;

      auto p = edgeSprings_.find(edge->id());

      if (p == edgeSprings_.end()) {
        SpringP existingSpring;

        const Edges &from = graph_->getEdges(edge->source(), edge->target());

        for (auto e : from) {
          auto p1 = edgeSprings_.find(e->id());

          if (p1 != edgeSprings_.end()) {
            existingSpring = (*p1).second;

            // MLK ?
            auto tspring = graph_->makeSpring();

            tspring->setPoint1(existingSpring->point1());
            tspring->setPoint2(existingSpring->point2());
            tspring->setLength(0.0);
            tspring->setK(0.0);

            isTemp = true;

            return tspring;
          }
        }

        const Edges &to = graph_->getEdges(edge->target(), edge->source());

        for (auto e : to) { // to ?
          auto p1 = edgeSprings_.find(e->id());

          if (p1 != edgeSprings_.end()) {
            existingSpring = (*p1).second;

            // MLK ?
            auto tspring = graph_->makeSpring();

            tspring->setPoint1(existingSpring->point2());
            tspring->setPoint2(existingSpring->point1());
            tspring->setLength(0.0);
            tspring->setK(0.0);

            isTemp = true;

            return tspring;
          }
        }

        auto spring = graph_->makeSpring();

        spring->setPoint1(nodePoint(edge->source()));
        spring->setPoint2(nodePoint(edge->target()));

        spring->setLength(edge->length());

        spring->setK(stiffness_);

        edgeSprings_[edge->id()] = spring;
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
        bool isTemp = false;

        auto spring = this->edgeSpring(edge, isTemp);

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
        point->setA(Vector(0, 0));
      }
    }

    void updatePosition(double timestep) {
      for (auto node : graph_->nodes()) {
        auto point = this->nodePoint(node);

        // Same question as above; along with updateVelocity, is this all of
        // your integration code?
        if (! node->isFixed())
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
      NodeP  minNode;
      PointP minPoint;
      double minDistance = 0.0;

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
    using NodePoints  = std::map<int, PointP>;
    using EdgeSprings = std::map<int, SpringP>;

    Graph*      graph_              { nullptr }; //!< parent graph
    double      stiffness_          { 400.0 };   //!< spring stiffness constant
    double      repulsion_          { 400.0 };   //!< repulsion constant
    double      damping_            { 0.5 };     //!< velocity damping factor
//  double      minEnergyThreshold_ { 0.0 };     //!< min energy threshold
    NodePoints  nodePoints_;                     //!< keep track of points associated with nodes
    EdgeSprings edgeSprings_;                    //!< keep track of springs associated with edges
  };
}

#endif
