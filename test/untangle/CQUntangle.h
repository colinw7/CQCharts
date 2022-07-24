#ifndef CQUntangle_H
#define CQUntangle_H

#include <QFrame>
#include <vector>
#include <set>

class Edge;

class Node {
 public:
  using Edges = std::vector<Edge *>;

 public:
  Node(uint ind, const QPoint &p) :
   ind_(ind), p_(p) {
  }

  const uint &ind() const { return ind_; }
  void setInd(const uint &i) { ind_ = i; }

  const QPoint &p() const { return p_; }
  void setP(const QPoint &p) { p_ = p; }

  uint x() const { return uint(p_.x()); }

  uint y() const { return uint(p_.y()); }
  void setY(uint y) { p_.setY(int(y)); }

  const Edges &edges() const { return edges_; }

  bool isFixed() const { return fixed_; }
  void setFixed(bool b) { fixed_ = b; }

  uint offset() const { return offset_; }
  void setOffset(uint o) { offset_ = o; }

  void addEdge(Edge *edge) {
    edges_.push_back(edge);
  }

  QString name() const { return QString("%1:%2").arg(p_.x()).arg(ind_); }

 private:
  uint   ind_    { 0 };
  QPoint p_;
  Edges  edges_;
  bool   fixed_  { false };
  uint   offset_ { 0 };
};

class Edge {
 public:
  using EdgeSet = std::set<Edge *>;

 public:
  Edge() { }

  Edge(Node *fromNode, Node *toNode) :
   fromNode_(fromNode), toNode_(toNode) {
  }

  const uint &ind() const { return ind_; }
  void setInd(const uint &i) { ind_ = i; }

  Node *fromNode() const { return fromNode_; }
  Node *toNode  () const { return toNode_; }

  void setFrom(Node *node) { fromNode_ = node; }
  void toNode (Node *node) { toNode_   = node; }

  //---

  bool isSkipOverlap() const { return skipOverlap_; }
  void setSkipOverlap(bool b) { skipOverlap_ = b; }

  void resetOverlaps() { overlapEdges_.clear(); }

  void addOverlap(Edge *edge) { overlapEdges_.insert(edge); }

  uint numOverlaps() { return uint(overlapEdges_.size()); }

  const EdgeSet &overlaps() const { return overlapEdges_; }

  QString nodeNames() const { return fromNode_->name() + "->" + toNode_->name(); }

  QString name() const { return QString::number(ind_); }

 private:
  Node* fromNode_ { nullptr };
  Node* toNode_   { nullptr };

  uint ind_ { 0 };

  EdgeSet overlapEdges_;
  bool    skipOverlap_ = false;
};

class Window : public QFrame {
  Q_OBJECT

 public:
  using Nodes   = std::vector<Node *>;
  using NodeSet = std::set<Node *>;
  using EdgeSet = std::set<Edge *>;

 public:
  Window();

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  void init();

  void autoDetangle();

  void detangleGroups();

  void sortNodes();
  void moveUnconnected();
  void moveSingle();

  void moveGroups();
  void initGroups();

  bool edgeOverlap(Edge *edge1, Edge *edge2) const;

  void resetFixed();
  void calcLastSwap();

  bool moveBottom(uint ind, uint i, bool fixed);

  void swapNodes(Nodes &nodes, uint i1, uint i2) const;

  void paintEvent(QPaintEvent *) override;

  void keyPressEvent(QKeyEvent *e) override;

 private:
  using Edges = std::vector<Edge *>;

  struct OverlapData {
    uint  numOverlaps { 0 };
    Edge *worstEdge   { nullptr };
    uint  worstN      { 0 };
    Edge *bestEdge    { nullptr };
    uint  bestN       { 0 };
    uint  numSkip     { 0 };

    void reset() {
      numOverlaps = 0;
      worstEdge   = nullptr;
      worstN      = 0;
      bestEdge    = nullptr;
      bestN       = 0;
      numSkip     = 0;
    }
  };

  struct NodeGroup {
    NodeGroup(uint x) :
     x(x) {
    }

    uint               x   { 0 };
    uint               ind { 0 };
    std::vector<Nodes> nodes;
    bool               fixed { false };
    Edges              edges;

    OverlapData overlapData;

    QString name() const { return QString::number(x) + ":" + QString::number(ind); }
  };

  using Groups           = std::vector<NodeGroup *>;
  using SizedGroups      = std::map<size_t, Groups>;
  using SizedGroupsArray = std::vector<SizedGroups>;

  QSize sizeHint() const override { return QSize(800, 1600); }

 private:
  void moveGroup(NodeGroup *group, bool fixed);

  void initAllGroupOverlaps(bool move);
  void initGroupOverlaps(NodeGroup *group, bool move);

  void printGroups() const;
  void printGroup(NodeGroup *nodeGroup) const;

  void calcGroupWorstEdge(NodeGroup *group, bool move);
  bool detangleGroup(NodeGroup *group);

  void followNode(uint c, Node *node, NodeGroup *nodeGroup, NodeSet &visited) const;

 private:
  using NodesArray = std::vector<Nodes>;

  size_t nc_ { 3 };
  size_t nr_ { 10 };

  NodesArray nodes_;
  Edges      edges_;

  // last swap per column
  std::vector<int> lastSwap_;

  SizedGroupsArray sizedGroups_;

  bool debug_ { false };
};

#endif
