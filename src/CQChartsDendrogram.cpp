#include <CQChartsDendrogram.h>
#include <iostream>
#include <cmath>
#include <cassert>

struct NodeCmp {
  // sort reverse alphabetic no case
  bool operator()(const CQChartsDendrogram::Node *n1, const CQChartsDendrogram::Node *n2) {
    const auto &name1 = n1->name();
    const auto &name2 = n2->name();

    int l1 = name1.size();
    int l2 = name2.size();

    for (int i = 0; i < std::max(l1, l2); ++i) {
      auto c1 = (i < l1 ? name1[i].toLower() : '\0');
      auto c2 = (i < l2 ? name2[i].toLower() : '\0');

      if (c1 > c2) return true;
      if (c1 < c2) return false;
    }

    return false;
  }
};

//---

CQChartsDendrogram::Node::
Node(Node *parent, const QString &name, const OptReal &size) :
 parent_(parent), id_(nextId()), name_(name), size_(size), open_(true)
{
}

CQChartsDendrogram::Node::
~Node()
{
  for (auto &c : children_)
    delete c.node;
}

//---

bool
CQChartsDendrogram::Node::
calcHierSize(double &s) const
{
  bool set = false;

  s = 0.0;

  if (! children_.empty()) {
    for (const auto &c : children_) {
      double s1;

      if (c.node->calcHierSize(s1)) {
        s += s1;
        set = true;
      }
    }
  }
  else {
    if (size_) {
      s   = *size_;
      set = true;
    }
  }

  return set;
}

//---

const CQChartsDendrogram::Node *
CQChartsDendrogram::Node::
root() const
{
  return const_cast<CQChartsDendrogram::Node *>(this)->root();
}

CQChartsDendrogram::Node *
CQChartsDendrogram::Node::
root()
{
  Node *hier;

  if (parent_) {
    hier = parent_;

    while (hier->parent())
      hier = hier->parent();
  }
  else
    hier = this;

  return hier;
}

void
CQChartsDendrogram::Node::
clear()
{
  children_.clear();
}

bool
CQChartsDendrogram::Node::
hasChildren() const
{
  return numNodes() > 0;
}

int
CQChartsDendrogram::Node::
numNodes() const
{
  return children_.size();
}

int
CQChartsDendrogram::Node::
maxNodes(bool ignoreOpen)
{
  int maxNum = 1;

  if (ignoreOpen || isOpen()) {
    for (const auto &c : children_)
      maxNum += c.node->maxNodes(ignoreOpen);
  }

  return std::max(maxNum, 1);
}

int
CQChartsDendrogram::Node::
maxEdges(bool ignoreOpen)
{
  int maxNum = 0;

  if (ignoreOpen || isOpen()) {
    for (const auto &c : children_)
      maxNum += c.node->maxEdges(ignoreOpen) + 1;
  }

  return std::max(maxNum, 0);
}

int
CQChartsDendrogram::Node::
calcDepth(bool ignoreOpen) const
{
  int maxDepth = 0;

  if (ignoreOpen || isOpen()) {
    for (const auto &c : children_)
      maxDepth = std::max(maxDepth, c.node->calcDepth(ignoreOpen));
  }

  return maxDepth + 1;
}

int
CQChartsDendrogram::Node::
calcHierDepth() const
{
  if (parent_)
    return parent_->calcHierDepth() + 1;
  else
    return 1;
}

//---

bool
CQChartsDendrogram::Node::
hasChild(Node *child) const
{
  for (const auto &c : children_)
    if (c.node == child)
      return true;

  return false;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::Node::
findChild(const QString &name) const
{
  for (const auto &c : children_)
    if (c.node->name() == name)
      return c.node;

  return nullptr;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::Node::
findHierChild(const QString &name) const
{
  if (this->name() == name)
    return const_cast<Node *>(this);

  auto *node = findChild(name);
  if (node) return node;

  for (const auto &c : children_) {
    node = c.node->findHierChild(name);
    if (node) return node;
  }

  return nullptr;
}

//---

void
CQChartsDendrogram::Node::
addChild(Node *child, const OptReal &value)
{
//if (child->parent_)
//  child->parent_->removeChild(child);

  child->setParent(this);

  children_.push_back(Edge(child, value));
}

void
CQChartsDendrogram::Node::
removeChild(Node *child)
{
  auto n = children_.size();
  if (n == 0) return;

  size_t i = 0;

  while (i < n) {
    if (children_[i].node == child)
      break;

    ++i;
  }

  if (i < n - 1) {
    ++i;

    while (i < n) {
      children_[i - 1] = children_[i];

      ++i;
    }
  }

  children_.pop_back();
}

void
CQChartsDendrogram::Node::
removeAll()
{
  children_.clear();
}

//---

CQChartsDendrogram::OptReal
CQChartsDendrogram::Node::
childValue(const Node *child) const
{
  for (auto &c : children_) {
    if (c.node == child)
      return c.value;
  }

  return OptReal();
}

void
CQChartsDendrogram::Node::
setChildValue(const Node *child, double value)
{
  for (auto &c : children_) {
    if (c.node == child) {
      c.value = value;
      return;
    }
  }
}

//---

void
CQChartsDendrogram::Node::
resetPlaced()
{
  bbox_   = BBox();
  placed_ = false;

  for (const auto &c : children_)
    c.node->resetPlaced();
}

//---

CQChartsDendrogram::
CQChartsDendrogram()
{
}

CQChartsDendrogram::
~CQChartsDendrogram()
{
  delete root_;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
addRootNode(const QString &name)
{
  assert(! root_);

  root_ = createRootNode(name);

  return root_;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
addEdge(Node *parentHier, const QString &name, const OptReal &edgeValue)
{
  auto *toNode = createNode(parentHier, name);

  parentHier->addChild(toNode, edgeValue);

  return toNode;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
addNode(Node *parentHier, const QString &name, const OptReal &nodeValue)
{
  auto *toNode = createNode(parentHier, name, nodeValue);

  parentHier->addChild(toNode);

  return toNode;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
createRootNode(const QString &name) const
{
  auto *root = createNode(nullptr, name);

  root->setRoot(true);

  return root;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
createNode(Node *hier, const QString &name, const OptReal &size) const
{
  return new Node(hier, name, size);
}

// place child nodes in (1.0 by 1.0 rectangle)
void
CQChartsDendrogram::
placeNodes()
{
  if (! root_) return;

  // reset placement state
  root_->resetPlaced();

  // get depth and init array of nodes at each depth
  int d = root_->calcDepth();

  depthNodes_.clear();

  depthNodes_.resize(size_t(d));

  // calc x delta (width / depth)
  dx_ = 1.0/d;

  //---

  // calc max rows and y delta (height / max rows)
  maxRows_ = root_->maxNodes();

  dy_ = 1.0/maxRows_;

  //---

  // place root (and recursively place children)
  placeNode(root_, 0, 0, maxRows_);

  if (root_->isOpen())
    placeSubNodes(root_, root_, 1, 0);

  //---

  // compress to remove unused space
  if (! isSingleStep())
    compressNodes();
}

void
CQChartsDendrogram::
placeSubNodes(Node *root, Node *node, int depth, double row)
{
  // make single list of nodes to place
  Nodes nodes;

  for (const auto &c : node->getChildren())
    nodes.push_back(c.node);

  // sort nodes by name
  std::sort(nodes.begin(), nodes.end(), NodeCmp());

  //---

  // place nodes in a grid (depth by max nodes)
  double row1 = row;

  for (const auto &n : nodes) {
    // get maximum number of child nodes
    int maxNodes = n->maxNodes();

    // place node
    placeNode(n, depth, row1, maxNodes);

    // place child nodes
    auto *hierNode = n;

    if (hierNode && hierNode->isOpen())
      placeSubNodes(root, hierNode, depth + 1, row1);

    // move row by max rows
    row1 += maxNodes;
  }
}

// compress nodes by removing space below
void
CQChartsDendrogram::
compressNodes()
{
  // init gaps
  setGaps();

  if (isDebug())
    printGaps();

  //---

  bool moved = false;

  // work from lowest depth up
  auto d = depthNodes_.size();
  if (d <= 1) return;

  for (int i = int(d - 1); i > 0; --i) {
    auto &dnodes = depthNodes_[size_t(i)];

    auto n = dnodes.size();

    for (uint j = 0; j < n; ++j) {
      auto *node = dnodes[j];

      //---

      // ignore if can't move (no gap or at root)
      double move_gap = node->gap();

      if (move_gap == 0 || ! node->parent())
        continue;

      //---

      // check gaps of child nodes
      Nodes lowestChildren;

      if (! canMoveNode(node, move_gap, lowestChildren))
        continue;

      if (move_gap <= 0)
        continue;

      //---

      // check gaps of parent nodes
      double move_gap1 = move_gap;

      auto *parent = node->parent();

      while (move_gap > 0 && parent) {
        move_gap1 = std::min(move_gap1, parent->gap());

        parent = parent->parent();
      }

      // if can move node, children and parent then compress node
      // (move node and parents down)
      if      (move_gap1 > 0)
        compressNodeAndChildren(node, lowestChildren, move_gap1);
      // if can't move parent but can move node and children, and
      // node is not the first child of it's parent, then move node
      // and higher child nodes
      else if (j > 0 && dnodes[j - 1]->parent() == node->parent()) {
        moveNode(node, move_gap);

        moveHigherNodes(node, move_gap);
      }
      // can't move so skip
      else
        continue;

      moved = true;

      if (isSingleStep())
        break;
    }

    if (moved && isSingleStep())
      break;
  }

  if (moved) {
    setGaps(); // TODO: needed, all gaps should have been fixed ?

    // shrink root to new max rows
    root()->setNumRows(maxRows_);
  }
}

// set gaps beneath each node
void
CQChartsDendrogram::
setGaps()
{
  maxRows_ = 1;

  // process nodes at each depth
  auto d = depthNodes_.size();

  for (uint i = 0; i < d; ++i) {
    auto &dnodes = depthNodes_[i];

    auto n = dnodes.size();

    // process nodes at depth
    double row = 0;

    for (uint j = 0; j < n; ++j) {
      auto *node = dnodes[j];

      // calc space from multi-row node
      double ns = (node->numRows() - 1.0)/2.0;

      // calc space from multi-row node below
      if (j > 0) {
        auto *node1 = dnodes[j - 1];

        ns += (node1->numRows() - 1.0)/2.0;
      }

      // gap is delta to previous row plus extra multi-row space
      double gap = (node->row() - row) + ns;

      setGap(node, gap);

      // move next row
      row = node->row() + node->numRows();
    }

    //double gap = maxRows_ - row;

    //if (gap > 0) {
    //  if (isDebug())
    //    std::cerr << "Gap "  << gap << " at " << i << "," << n << "\n";
    //}

    // update max rows (ignore depth==0 which has a single root node)
    if (i > 0)
      maxRows_ = std::max(maxRows_, row);
  }

  // update dy
  dy_ = 1.0/maxRows_;
}

// check if we can move node and children by delta
bool
CQChartsDendrogram::
canMoveNode(Node *node, double &move_gap, Nodes &lowestChildren)
{
  // find location of node in depth nodes array
  auto &dnodes = depthNodes_[size_t(node->depth())];

  auto n = dnodes.size();

  uint i = 0;

  for ( ; i < n; ++i)
    if (dnodes[i] == node)
      break;

  // check gaps of child nodes for node and all siblings above it
  Node *lowestChild = nullptr;

  for ( ; i < n; ++i) {
    auto *node1 = dnodes[i];

    if (node1->isOpen() && node1->hasChildren()) {
      auto *hierNode = node1;

      auto *child = getLowestChild(hierNode);

      if (! lowestChild || child->row() < lowestChild->row())
        lowestChild = child;
    }
  }

  if (! lowestChild)
    return true;

  // save child and update gap
  lowestChildren.push_back(lowestChild);

  move_gap = std::min(move_gap, lowestChild->gap());

  if (move_gap == 0)
    return false;

  // recurse move check at lowest child
  return canMoveNode(lowestChild, move_gap, lowestChildren);
}

// get the child of node with lowest row
CQChartsDendrogram::Node *
CQChartsDendrogram::
getLowestChild(Node *hierNode)
{
  Node *lowestNode = nullptr;

  const auto &children = hierNode->getChildren();

  for (const auto &c : children) {
    if (! lowestNode || c.node->row() < lowestNode->row())
      lowestNode = c.node;
  }

  return lowestNode;
}

// compress node and children
void
CQChartsDendrogram::
compressNodeAndChildren(Node *node, const Nodes &lowestChildren, double d)
{
  if (isDebug()) {
    std::cerr << "Compress " << node->name().toStdString() << " by " << d;

    if (! lowestChildren.empty()) {
      std::cerr << " (Lowest:";

      for (const auto &c : lowestChildren) {
        std::cerr << " " << c->name().toStdString() << "," << c->gap();
      }

      std::cerr << ")";
    }

    std::cerr << "\n";
  }

  //---

  // compress node and parents
  compressNodeUp(node, d);

  // move children
  for (const auto &c : lowestChildren) {
    moveNode(c, d);

    moveHigherNodes(c, d);
  }

  //if (node->hasChildren())
  //  moveChildNodes(node, d);
}

// recursively move node, higher nodes and parent
void
CQChartsDendrogram::
compressNodeUp(Node *node, double d)
{
  compressNode(node, d);

  moveHigherNodes(node, d);

  auto *parent = node->parent();

  if (node->depth() > 1)
    compressNodeUp(parent, d);
}

void
CQChartsDendrogram::
compressNode(Node *node, double d)
{
  if (! node->isHier()) {
    if (isDebug())
      std::cerr << "Compress " << node->name().toStdString() << " by " << d << "\n";

    node->setRow(node->row() - d);
    setGap(node, node->gap() - d);
  }
  else {
    // reduce number of rows if multi-row item
    if (node->numRows() > 1) {
      if (isDebug())
        std::cerr << "Compress " << node->name().toStdString() << " by " << d << "\n";

      node->setNumRows(node->numRows() - 2*d);

      // if run out of rows then move node (TODO: possible ?)
      if (node->numRows() < 1) {
        double d1 = 1 - node->numRows();

        node->setRow(node->row() - d1);
      }
    }
    // if single row item just move
    else
      moveNode(node, d);

    // update gap
    setGap(node, node->gap() - d);
  }
}

void
CQChartsDendrogram::
setGap(Node *node, double gap)
{
  if (node->parent() && gap != node->gap()) {
    if (isDebug())
      std::cerr << "Set gap for " << node->name().toStdString() <<
                   " Old: " << node->gap() << " New: " << gap << "\n";

    node->setGap(gap);
  }
}

// move all nodes higher than specified node
void
CQChartsDendrogram::
moveHigherNodes(Node *node, double d)
{
  auto &dnodes = depthNodes_[size_t(node->depth())];

  // work down from top until hit node
  auto n = dnodes.size();
  if (n < 2) return;

  for (int i = int(n - 1); i >= 0; --i) {
    auto *dnode = dnodes[size_t(i)];
    if (dnode == node) break;

    //if (dnode->hasChildren())
    //  moveChildNodes(dnode, d);
    //else
    moveNode(dnode, d);
  }
}

#if 0
void
CQChartsDendrogram::
moveChildNodes(Node *hierNode, double d)
{
  if (! hierNode->isOpen())
    return;

  const auto &children = hierNode->getChildren();

  for (const auto &c : children) {
    moveNode(c.node, d);

    moveChildNodes(c, d);
  }
}
#endif

void
CQChartsDendrogram::
moveNode(Node *node, double d)
{
  if (isDebug())
    std::cerr << "Move " << node->name().toStdString() << " by " << d << "\n";

  node->setRow(node->row() - d);
}

// place node (set depth, row and number of rows)
void
CQChartsDendrogram::
placeNode(Node *node, int depth, double row, double num_rows)
{
  node->setDepth  (depth);
  node->setRow    (row);
  node->setNumRows(num_rows);
  node->setPlaced (true);

  depthNodes_[size_t(depth)].push_back(node);
}

// print node gaps
void
CQChartsDendrogram::
printGaps() const
{
  auto d = depthNodes_.size();

  for (uint i = 0; i < d; ++i) {
    const auto &dnodes = depthNodes_[i];

    auto n = dnodes.size();

    for (uint j = 0; j < n; ++j) {
      auto *node = dnodes[j];

      if (i > 0 && node->gap() > 0)
        std::cerr << "Gap " << node->gap() << " below " << node->name().toStdString() <<
                     " (" << i << "," << j << ")\n";
    }
  }
}

// open all nodes down to depth and close all node below depth
void
CQChartsDendrogram::
setOpenDepth(int depth)
{
  auto d = depthNodes_.size();

  for (uint i = 0; i < d; ++i) {
    auto &dnodes = depthNodes_[i];

    auto n = dnodes.size();

    for (uint j = 0; j < n; ++j) {
      auto *node = dnodes[j];

      node->setOpen(i < size_t(depth));
    }
  }
}

// open node with name at depth
void
CQChartsDendrogram::
openNode(int depth, const QString &name)
{
  auto &dnodes = depthNodes_[size_t(depth)];

  auto n = dnodes.size();

  for (uint j = 0; j < n; ++j) {
    auto *node = dnodes[j];

    if (node->name() == name)
      node->setOpen(true);
  }
}

bool
CQChartsDendrogram::
isNodeAtPoint(double x, double y, double tol) const
{
  assert(root_);

  return isNodeAtPoint(root_, x, y, tol);
}

bool
CQChartsDendrogram::
isNodeAtPoint(Node *node, double x, double y, double tol) const
{
  double d;

  if (node->bbox().isValid())
    d = std::hypot(nodeXC(node) - x, nodeYC(node) - y);
  else
    d = std::hypot(nodeX (node) - x, nodeYC(node) - y);

  return (d <= tol);
}

const CQChartsDendrogram::Node *
CQChartsDendrogram::
getNodeAtPoint(double x, double y, double tol) const
{
  return const_cast<CQChartsDendrogram *>(this)->getNodeAtPoint(x, y, tol);
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
getNodeAtPoint(double x, double y, double tol)
{
  assert(root_);

  return getNodeAtPoint(root_, x, y, tol);
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
getNodeAtPoint(Node *node, double x, double y, double tol)
{
  if (isNodeAtPoint(node, x, y, tol))
    return node;

  for (const auto &c : node->getChildren()) {
    auto *node = getNodeAtPoint(c.node, x, y, tol);

    if (node)
      return node;
  }

  return nullptr;
}

double
CQChartsDendrogram::
nodeX(Node *node) const
{
  if (node->hasBBox()) return node->bbox().getXMin();

  return node->depth()*dx();
}

double
CQChartsDendrogram::
nodeY(Node *node) const
{
  if (node->hasBBox()) return node->bbox().getYMin();

  return node->row()*dy();
}

double
CQChartsDendrogram::
nodeW(Node *node) const
{
  if (node->hasBBox()) return node->bbox().getWidth();

  return dx();
}

double
CQChartsDendrogram::
nodeH(Node *node) const
{
  if (node->hasBBox()) return node->bbox().getHeight();

  return node->numRows()*dy();
}

double
CQChartsDendrogram::
nodeXC(Node *node) const
{
  return nodeX(node) + nodeW(node)/2.0;
}

double
CQChartsDendrogram::
nodeYC(Node *node) const
{
  return nodeY(node) + nodeH(node)/2.0;
}
