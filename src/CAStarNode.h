#ifndef CASTAR_NODE_H
#define CASTAR_NODE_H

#include <list>
#include <map>
#include <set>

#include <cassert>
#include <iostream>

template<typename NODE>
class CAStar {
 public:
  struct NodeData {
    NODE*  parent              { nullptr };
    double costFromStart       { 0.0 };
    double estimatedCostToGoal { 0.0 };
    double totalCost           { 0.0 };
  };

 public:
  using NodeList  = std::list  <NODE *>;
  using NodeArray = std::vector<NODE *>;

 public:
  CAStar() { }

  virtual ~CAStar() { }

  //--- By Node

  // do A* search from start to goal
  bool search(NODE *startNode, NODE *goalNode, NodeList &pathNodes);

  // smallest/optimal cost to goal
  virtual double pathCostEstimate(NODE *startNode, NODE *goalNode);

  // cost to move from specified location to new location
  virtual double traverseCost(NODE *node, NODE *newNode);

  virtual NodeList getNextNodes(NODE *node) const;

  //---

  void getPathNodes(NODE *node, NodeArray &pathNodes) const;

  //------

  bool isOpenNode(NODE *node) const;
  bool isClosedNode(NODE *node) const;

  virtual NODE *getBestOpenNode();

  //------

  void printNode(NODE *node) const;

 protected:
  NodeData &getNodeData(NODE *node) const;

  NodeData *getNodeDataP(NODE *node) const;

 protected:
  using NodeSet     = std::set<NODE *>;
  using NodeDataMap = std::map<NODE *, NodeData>;

  NodeSet openNodes_;   // open nodes
  NodeSet closedNodes_; // closed nodes

  mutable NodeDataMap nodeDataMap_;
};

//---

template<typename NODE>
bool
CAStar<NODE>::
search(NODE *startNode, NODE *goalNode, NodeList &pathNodes)
{
  // clear open and closed
  openNodes_  .clear();
  closedNodes_.clear();

  auto &startNodeData = getNodeData(startNode);

  startNodeData.parent              = nullptr;
  startNodeData.costFromStart       = 0.0;
  startNodeData.estimatedCostToGoal = pathCostEstimate(startNode, goalNode);
  startNodeData.totalCost           = startNodeData.estimatedCostToGoal;

  openNodes_.insert(startNode);

  // process the list until we get to the goal or fail
  while (! openNodes_.empty()) {
    // remove node from open with lowest total cost
    auto *node = getBestOpenNode();

    // if at goal then we're done
    if (node == goalNode) {
      // construct path backward from Node to start
      pathNodes.push_front(node);

      auto &nodeData = getNodeData(node);

      while (nodeData.parent) {
        node = nodeData.parent;

        pathNodes.push_front(node);

        nodeData = getNodeData(node);
      }

      return true;
    }

    // remove open node
    openNodes_.erase(node);

    // push node onto closed
    closedNodes_.insert(node);

    auto &nodeData = getNodeData(node);

    // get successor nodes of this node
    NodeList nextNodes = getNextNodes(node);

    for (auto *nextNode : nextNodes) {
      // if closed then skip
      if (isClosedNode(nextNode))
        continue;

      // get cost to this next node
      double nextCost = nodeData.costFromStart + traverseCost(node, nextNode);

      bool isOpen = isOpenNode(nextNode);

      auto &nextNodeData = getNodeData(nextNode);

      // better if not already open, or already open and this is a better path to this node
      bool isBetter = (! isOpen || nextCost < nextNodeData.costFromStart);

      if (isBetter) {
        if (isOpen)
          openNodes_.erase(nextNode);

        nextNodeData.parent              = node;
        nextNodeData.costFromStart       = nextCost;
        nextNodeData.estimatedCostToGoal = pathCostEstimate(nextNode, goalNode);
        nextNodeData.totalCost           = nextNodeData.costFromStart +
                                           nextNodeData.estimatedCostToGoal;

        openNodes_.insert(nextNode);
      }
    }
  }

  return false;
}

template<typename NODE>
void
CAStar<NODE>::
getPathNodes(NODE *node, NodeArray &pathNodes) const
{
  NodeList pathNodes1;

  pathNodes1.push_front(node);

  auto *nodeData = getNodeDataP(node);

  while (nodeData && nodeData->parent) {
    node = nodeData->parent;

    pathNodes1.push_front(node);

    nodeData = getNodeDataP(node);
  }

  for (auto *node1 : pathNodes1)
    pathNodes.push_back(node1);
}

template<typename NODE>
bool
CAStar<NODE>::
isOpenNode(NODE *node) const
{
  return openNodes_.find(node) != openNodes_.end();
}

template<typename NODE>
bool
CAStar<NODE>::
isClosedNode(NODE *node) const
{
  return closedNodes_.find(node) != closedNodes_.end();
}

template<typename NODE>
NODE *
CAStar<NODE>::
getBestOpenNode()
{
  NODE   *minNode = nullptr;
  double  minCost = 0.0;

  for (auto *node : openNodes_) {
    auto &nodeData = getNodeData(node);

    double cost = nodeData.totalCost;

    if (minNode == nullptr || cost < minCost) {
      minNode = node;
      minCost = cost;
    }
  }

  //printNode(minNode);

  return minNode;
}

template<typename NODE>
double
CAStar<NODE>::
pathCostEstimate(NODE *, NODE *)
{
  assert(false);

  return 0.0;
}

template<typename NODE>
double
CAStar<NODE>::
traverseCost(NODE *, NODE *)
{
  assert(false);

  return 0.0;
}

template<typename NODE>
typename CAStar<NODE>::NodeList
CAStar<NODE>::
getNextNodes(NODE *) const
{
  assert(false);

  return NodeList();
}

template<typename NODE>
void
CAStar<NODE>::
printNode(NODE *node) const
{
  node->print(std::cout);

  std::cout << "\n";
}

template<typename NODE>
typename CAStar<NODE>::NodeData &
CAStar<NODE>::
getNodeData(NODE *node) const
{
  using ValueType = typename NodeDataMap::value_type;

  auto *th = const_cast<CAStar *>(this);

  auto p = th->nodeDataMap_.find(node);

  if (p == th->nodeDataMap_.end())
    p = th->nodeDataMap_.insert(p, ValueType(node, NodeData()));

  return (*p).second;
}

template<typename NODE>
typename CAStar<NODE>::NodeData *
CAStar<NODE>::
getNodeDataP(NODE *node) const
{
  auto p = nodeDataMap_.find(node);

  if (p == nodeDataMap_.end()) return nullptr;

  return &(*p).second;
}

#endif
