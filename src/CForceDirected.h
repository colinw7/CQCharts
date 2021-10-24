#ifndef CForceDirected_H
#define CForceDirected_H

#include <Springy.h>

/*!
 * \brief Force directed graph data
 */
class CForceDirected {
 public:
  CForceDirected() {
    initialized_ = false;
  }

  virtual ~CForceDirected() {
    delete graph_;
    delete layout_;
  }

  double stiffness() const { return stiffness_; }
  void setStiffness(double r) { stiffness_ = r; }

  double repulsion() const { return repulsion_; }
  void setRepulsion(double r) { repulsion_ = r; }

  double damping() const { return damping_; }
  void setDamping(double r) { damping_ = r; }

  void reset() {
    delete graph_;
    delete layout_;

    initialized_ = false;

    graph_  = nullptr;
    layout_ = nullptr;
  }

  Springy::Node *newNode() {
    init();

    return graph_->newNode();
  }

  Springy::Node *getNode(int id) {
    init();

    return graph_->getNode(id);
  }

  Springy::Edge *newEdge(Springy::Node *node1, Springy::Node *node2) {
    assert(node1 && node2);

    init();

    return graph_->newEdge(node1, node2);
  }

  void addNode(Springy::Node *node) {
    init();

    graph_->addNode(node);
  }

  void addEdge(Springy::Edge *edge) {
    init();

    graph_->addEdge(edge);
  }

  Springy::Edges edges() const { init(); return graph_->edges(); }
  Springy::Nodes nodes() const { init(); return graph_->nodes(); }

  void step(double dt) {
    init();

    layout_->step(dt);
  }

  Springy::Spring *spring(Springy::Edge *edge, bool &isTemp) {
    init();

    return layout_->edgeSpring(edge, isTemp);
  }

  Springy::Point *point(Springy::Node *node) {
    init();

    return layout_->nodePoint(node);
  }

  Springy::NodePoint nearest(const Springy::Vector &v) const {
    init();

    return layout_->nearest(v);
  }

  Springy::Node *currentNode() const { return currentNode_; }
  void setCurrentNode(Springy::Node *node) { currentNode_ = node; }

  Springy::Point *currentPoint() const { return currentPoint_; }
  void setCurrentPoint(Springy::Point *point) { currentPoint_ = point; }

  void calcRange(double &xmin, double &ymin, double &xmax, double &ymax) const {
    init();

    layout_->calcRange(xmin, ymin, xmax, ymax);
  }

 protected:
  void init() const {
    if (! initialized_) {
      auto *th = const_cast<CForceDirected *>(this);

      th->graph_  = makeGraph();
      th->layout_ = makeLayout();

      th->initialized_ = true;
    }
  }

  virtual Springy::Graph *makeGraph() const {
    return new Springy::Graph;
  }

  virtual Springy::Layout *makeLayout() const {
    return new Springy::Layout(graph_, stiffness_, repulsion_, damping_);
  }

 private:
  double           stiffness_    { 400.0 };
  double           repulsion_    { 400.0 };
  double           damping_      { 0.5 };
  bool             initialized_  { false };
  Springy::Graph*  graph_        { nullptr };
  Springy::Layout* layout_       { nullptr };
  Springy::Node*   currentNode_  { nullptr };
  Springy::Point*  currentPoint_ { nullptr };
};

#endif
