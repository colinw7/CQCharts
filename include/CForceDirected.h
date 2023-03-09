#ifndef CForceDirected_H
#define CForceDirected_H

#include <Springy.h>

/*!
 * \brief Force directed graph data
 */
class CForceDirected {
 public:
  using NodeP   = std::shared_ptr<Springy::Node>;
  using EdgeP   = std::shared_ptr<Springy::Edge>;
  using GraphP  = std::shared_ptr<Springy::Graph>;
  using LayoutP = std::shared_ptr<Springy::Layout>;
  using SpringP = std::shared_ptr<Springy::Spring>;
  using PointP  = std::shared_ptr<Springy::Point>;

 public:
  CForceDirected() {
    initialized_ = false;
  }

  virtual ~CForceDirected() { }

  double stiffness() const { return stiffness_; }
  void setStiffness(double r) { stiffness_ = r; if (layout_) layout_->setStiffness(stiffness_); }

  double repulsion() const { return repulsion_; }
  void setRepulsion(double r) { repulsion_ = r; if (layout_) layout_->setRepulsion(repulsion_); }

  double damping() const { return damping_; }
  void setDamping(double r) { damping_ = r; if (layout_) layout_->setDamping(damping_); }

  void resetPlacement() {
    layout_->resetNodes();
  }

  void reset() {
    initialized_ = false;

    graph_  = GraphP();
    layout_ = LayoutP();
  }

  NodeP newNode() {
    init();

    return graph_->newNode();
  }

  NodeP getNode(int id) {
    init();

    return graph_->getNode(id);
  }

  EdgeP newEdge(NodeP node1, NodeP node2) {
    assert(node1 && node2);

    init();

    return graph_->newEdge(node1, node2);
  }

  void addNode(NodeP node) {
    init();

    graph_->addNode(NodeP(node));
  }

  void addEdge(EdgeP edge) {
    init();

    graph_->addEdge(EdgeP(edge));
  }

  Springy::Edges edges() const { init(); return graph_->edges(); }
  Springy::Nodes nodes() const { init(); return graph_->nodes(); }

  Springy::Edges getEdges(NodeP node) const {
    init();

    return graph_->getEdges(node);
  }

  Springy::Edges getEdges(NodeP node1, NodeP node2) const {
    init();

    return graph_->getEdges(node1, node2);
  }

  double step(double dt) {
    init();

    return layout_->step(dt);
  }

  SpringP spring(EdgeP edge, bool &isTemp) {
    init();

    return layout_->edgeSpring(edge, isTemp);
  }

  PointP point(NodeP node) {
    init();

    return layout_->nodePoint(node);
  }

  Springy::NodePoint nearest(const Springy::Vector &v) const {
    init();

    return layout_->nearest(v);
  }

  NodeP currentNode() const { return currentNode_; }
  void setCurrentNode(NodeP node) { currentNode_ = node; }

  PointP currentPoint() const { return currentPoint_; }
  void setCurrentPoint(PointP point) { currentPoint_ = point; }

  void calcRange(double &xmin, double &ymin, double &xmax, double &ymax) const {
    init();

    layout_->calcRange(xmin, ymin, xmax, ymax);
  }

  void adjustRange(double &xmin, double &ymin, double &xmax, double &ymax) {
    init();

    layout_->adjustRange(xmin, ymin, xmax, ymax);
  }

 protected:
  void init() const {
    if (! initialized_) {
      auto *th = const_cast<CForceDirected *>(this);

      th->graph_  = GraphP(makeGraph());
      th->layout_ = LayoutP(makeLayout());

      th->initialized_ = true;
    }
  }

  virtual GraphP makeGraph() const {
    return GraphP(new Springy::Graph);
  }

  virtual LayoutP makeLayout() const {
    return LayoutP(new Springy::Layout(graph_.get(), stiffness_, repulsion_, damping_));
  }

 private:
  double  stiffness_    { 400.0 };
  double  repulsion_    { 400.0 };
  double  damping_      { 0.5 };
  bool    initialized_  { false };
  GraphP  graph_;
  LayoutP layout_;
  NodeP   currentNode_  { nullptr };
  PointP  currentPoint_ { nullptr };
};

#endif
