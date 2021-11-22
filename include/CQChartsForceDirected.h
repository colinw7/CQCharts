#ifndef CQChartsForceDirected_H
#define CQChartsForceDirected_H

#include <CForceDirected.h>

//---

/*!
 * \brief Spring Node Customization
 * \ingroup Charts
 */
class CQChartsSpringyNode : public Springy::Node {
 public:
  using OptReal = CQChartsOptReal;

 public:
  explicit CQChartsSpringyNode(int id) :
   Springy::Node(id) {
  }

  int group() const { return group_; }
  void setGroup(int i) { group_ = i; }

  const OptReal &nodeValue() const { return nodeValue_; }
  void setNodeValue(const OptReal &v) { nodeValue_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

 private:
  int         group_ { -1 };
  OptReal     nodeValue_;
  QModelIndex ind_;
};

/*!
 * \brief Spring Edge Customization
 * \ingroup Charts
 */
class CQChartsSpringyEdge : public Springy::Edge {
 public:
  using NodeP = Springy::NodeP;

 public:
  CQChartsSpringyEdge(int id, NodeP node1=NodeP(), NodeP node2=NodeP()) :
   Springy::Edge(id, node1, node2) {
  }
};

/*!
 * \brief Spring Graph Customization
 * \ingroup Charts
 */
class CQChartsSpringyGraph : public Springy::Graph {
 public:
  explicit CQChartsSpringyGraph() :
   Springy::Graph() {
  }

  Springy::NodeP makeNode() const override {
    return Springy::NodeP(new CQChartsSpringyNode(++nextNodeId_));
  }

  Springy::EdgeP makeEdge() const override {
    return Springy::EdgeP(new CQChartsSpringyEdge(++nextEdgeId_));
  }
};

//---

class CQChartsForceDirected : public CForceDirected {
 public:
  CQChartsForceDirected() :
   CForceDirected() {
  }

  Springy::GraphP makeGraph() const override {
    return Springy::GraphP(new CQChartsSpringyGraph);
  }
};

#endif
