#ifndef CQChartsForceDirected_H
#define CQChartsForceDirected_H

#include <CQChartsSpringy.h>

/*!
 * \brief Force directed graph data
 * \ingroup Charts
 */
class CQChartsForceDirected {
 public:
  CQChartsForceDirected() {
    graph_  = new Springy::Graph;
    layout_ = new Springy::Layout(graph_, stiffness_, repulsion_, damping_);
  }

 ~CQChartsForceDirected() {
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

  Springy::Spring *spring(Springy::Edge *edge, bool &isTemp) {
    return layout_->edgeSpring(edge, isTemp);
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
  Springy::Graph*  graph_        { nullptr };
  Springy::Layout* layout_       { nullptr };
  Springy::Node*   currentNode_  { nullptr };
  Springy::Point*  currentPoint_ { nullptr };
};

#endif
