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

  for (auto &n : nodes_)
    delete n.node;
}

//---

bool
CQChartsDendrogram::Node::
calcSize(double &s) const
{
  bool set = false;

  s = 0.0;

  if (! children_.empty() || ! nodes_.empty()) {
    for (const auto &c : children_) {
      double s1;

      if (c.node->calcSize(s1)) {
        s += s1;
        set = true;
      }
    }

    for (const auto &n : nodes_) {
      double s1;

      if (n.node->calcSize(s1)) {
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

void
CQChartsDendrogram::Node::
setGap(double gap)
{
  if (parent() && gap != gap_) {
    if (root()->debug())
      std::cerr << "Set gap for " << name().toStdString() <<
                   " Old: " << gap_ << " New: " << gap << std::endl;

    gap_ = gap;
  }
}

const CQChartsDendrogram::RootNode *
CQChartsDendrogram::Node::
root() const
{
  return const_cast<CQChartsDendrogram::Node *>(this)->root();
}

CQChartsDendrogram::RootNode *
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

  return dynamic_cast<RootNode *>(hier);
}

void
CQChartsDendrogram::Node::
clear()
{
  children_.clear();
  nodes_   .clear();
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
  return int(nodes_.size() + children_.size());
}

int
CQChartsDendrogram::Node::
maxNodes(bool ignoreOpen)
{
  int maxNum = 1;

  if (ignoreOpen || isOpen()) {
    maxNum += int(nodes_.size());

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
    maxNum += int(nodes_.size());

    for (const auto &c : children_)
      maxNum += c.node->maxEdges(ignoreOpen) + 1;
  }

  return std::max(maxNum, 0);
}

int
CQChartsDendrogram::Node::
calcDepth(bool ignoreOpen) const
{
  int max_depth = 0;

  if (ignoreOpen || isOpen()) {
    if (! nodes_.empty())
      max_depth = 1;

    for (const auto &c : children_)
      max_depth = std::max(max_depth, c.node->calcDepth(ignoreOpen));
  }

  return max_depth + 1;
}

//---

CQChartsDendrogram::Node *
CQChartsDendrogram::Node::
findChild(const QString &name) const
{
  for (const auto &c : children_)
    if (c.node->name() == name)
      return c.node;

  return nullptr;
}

bool
CQChartsDendrogram::Node::
hasChild(Node *child) const
{
  for (const auto &c : children_)
    if (c.node == child)
      return true;

  return false;
}

//---

void
CQChartsDendrogram::Node::
addChild(Node *child, const OptReal &value)
{
//if (child->parent_)
//  child->parent_->removeChild(child);

  child->setParent(this);

  children_.push_back(Child(child, value));
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
addNode(Node *node, const OptReal &value)
{
//if (node->parent_)
//  node->parent_->removeNode(node);

  node->setParent(this);

  nodes_.push_back(Child(node, value));
}

void
CQChartsDendrogram::Node::
removeNode(Node *node)
{
  auto n = nodes_.size();
  if (n == 0) return;

  size_t i = 0;

  while (i < n) {
    if (nodes_[i].node == node)
      break;

    ++i;
  }

  if (i < n - 1) {
    ++i;

    while (i < n) {
      nodes_[i - 1] = nodes_[i];

      ++i;
    }
  }

  nodes_.pop_back();
}

//---

void
CQChartsDendrogram::Node::
setChildValue(Node *child, double value)
{
  for (auto &c : children_) {
    if (c.node == child) {
      c.value = value;
      return;
    }
  }

  for (auto &n : nodes_) {
    if (n.node == child) {
      n.value = value;
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

  for (const auto &n : nodes_)
    n.node->resetPlaced();
}

//---

void
CQChartsDendrogram::Node::
moveNode(double d)
{
  if (root()->debug())
    std::cerr << "Move " << name().toStdString() << " by " << d << std::endl;

  setRow(row() - d);
}

void
CQChartsDendrogram::Node::
compressNode(double d)
{
  if (children_.empty() && nodes_.empty()) {
    if (root()->debug())
      std::cerr << "Compress " << name().toStdString() << " by " << d << std::endl;

    setRow(row() - d);
    setGap(gap() - d);
  }
  else {
    // reduce number of rows if multi-row item
    if (numRows() > 1) {
      if (root()->debug())
        std::cerr << "Compress " << name().toStdString() << " by " << d << std::endl;

      setNumRows(numRows() - 2*d);

      // if run out of rows then move node (TODO: possible ?)
      if (numRows() < 1) {
        double d1 = 1 - numRows();

        setRow(row() - d1);
      }
    }
    // if single row item just move
    else
      moveNode(d);

    // update gap
    setGap(gap() - d);
  }
}

//---

bool
CQChartsDendrogram::Node::
isNodeAtPoint(double x, double y, double tol) const
{
  double d;

  if (this->bbox().isValid())
    d = std::hypot(this->xc() - x, this->yc() - y);
  else
    d = std::hypot(this->x() - x, this->yc() - y);

  return (d <= tol);
}

const CQChartsDendrogram::Node *
CQChartsDendrogram::Node::
getNodeAtPoint(double x, double y, double tol) const
{
  return const_cast<CQChartsDendrogram::Node *>(this)->getNodeAtPoint(x, y, tol);
}

CQChartsDendrogram::Node *
CQChartsDendrogram::Node::
getNodeAtPoint(double x, double y, double tol)
{
  if (isNodeAtPoint(x, y, tol))
    return this;

  for (const auto &c : children_) {
    auto *node = c.node->getNodeAtPoint(x, y, tol);

    if (node)
      return node;
  }

  for (const auto &n : nodes_) {
    if (n.node->isNodeAtPoint(x, y, tol))
      return n.node;
  }

  return nullptr;
}

//---

void
CQChartsDendrogram::Node::
placeSubNodes(RootNode *root, int depth, double row)
{
  // make single list of nodes to place
  Nodes nodes;

  for (const auto &c : children_)
    nodes.push_back(c.node);

  for (const auto &n : nodes_)
    nodes.push_back(n.node);

  // sort nodes by name
  std::sort(nodes.begin(), nodes.end(), NodeCmp());

  //---

  // place nodes in a grid (depth by max nodes)
  double row1 = row;

  for (const auto &n : nodes) {
    // get maximum number of child nodes
    int maxNodes = n->maxNodes();

    // place node
    root->placeNode(n, depth, row1, maxNodes);

    // place child nodes
    auto *hierNode = n;

    if (hierNode && hierNode->isOpen())
      hierNode->placeSubNodes(root, depth + 1, row1);

    // move row by max rows
    row1 += maxNodes;
  }
}

//---

CQChartsDendrogram::RootNode::
RootNode(const QString &name) :
 Node(nullptr, name)
{
}

// place child nodes in (1.0 by 1.0 rectangle)
void
CQChartsDendrogram::RootNode::
placeNodes()
{
  // reset placement state
  resetPlaced();

  // get depth and init array of nodes at each depth
  int d = calcDepth();

  depthNodes_.clear();

  depthNodes_.resize(size_t(d));

  // calc x delta (width / depth)
  dx_ = 1.0/d;

  //---

  // calc max rows and y delta (height / max rows)
  max_rows_ = maxNodes();

  dy_ = 1.0/max_rows_;

  //---

  // place root (and recursively place children)
  placeNode(this, 0, 0, max_rows_);

  if (isOpen())
    placeSubNodes(this, 1, 0);

  //---

  // compress to remove unused space
  if (! singleStep())
    compressNodes();
}

// set gaps beneath each node
void
CQChartsDendrogram::RootNode::
setGaps()
{
  max_rows_ = 1;

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

      node->setGap(gap);

      // move next row
      row = node->row() + node->numRows();
    }

    //double gap = max_rows_ - row;

    //if (gap > 0) {
    //  if (root()->debug())
    //    std::cerr << "Gap "  << gap << " at " << i << "," << n << std::endl;
    //}

    // update max rows (ignore depth==0 which has a single root node)
    if (i > 0)
      max_rows_ = std::max(max_rows_, row);
  }

  // update dy
  dy_ = 1.0/max_rows_;
}

// print node gaps
void
CQChartsDendrogram::RootNode::
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
                     " (" << i << "," << j << ")" << std::endl;
    }
  }
}

// compress nodes by removing space below
void
CQChartsDendrogram::RootNode::
compressNodes()
{
  // init gaps
  setGaps();

  if (debug())
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
        node->moveNode(move_gap);

        moveHigherNodes(node, move_gap);
      }
      // can't move so skip
      else
        continue;

      moved = true;

      if (singleStep())
        break;
    }

    if (moved && singleStep())
      break;
  }

  if (moved) {
    setGaps(); // TODO: needed, all gaps should have been fixed ?

    // shrink root to new max rows
    root()->setNumRows(max_rows_);
  }
}

// check if we can move node and children by delta
bool
CQChartsDendrogram::RootNode::
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
CQChartsDendrogram::RootNode::
getLowestChild(Node *hierNode)
{
  Node *lowestNode = nullptr;

  const auto &children = hierNode->getChildren();

  for (const auto &c : children) {
    if (! lowestNode || c.node->row() < lowestNode->row())
      lowestNode = c.node;
  }

  const auto &nodes = hierNode->getNodes();

  for (const auto &n : nodes) {
    if (! lowestNode || n.node->row() < lowestNode->row())
      lowestNode = n.node;
  }

  return lowestNode;
}

// open all nodes down to depth and close all node below depth
void
CQChartsDendrogram::RootNode::
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
CQChartsDendrogram::RootNode::
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

// compress node and children
void
CQChartsDendrogram::RootNode::
compressNodeAndChildren(Node *node, const Nodes &lowestChildren, double d)
{
  if (debug()) {
    std::cerr << "Compress " << node->name().toStdString() << " by " << d;

    if (! lowestChildren.empty()) {
      std::cerr << " (Lowest:";

      for (const auto &c : lowestChildren) {
        std::cerr << " " << c->name().toStdString() << "," << c->gap();
      }

      std::cerr << ")";
    }

    std::cerr << std::endl;
  }

  //---

  // compress node and parents
  compressNodeUp(node, d);

  // move children
  for (const auto &c : lowestChildren) {
    c->moveNode(d);

    moveHigherNodes(c, d);
  }

  //if (node->hasChildren())
  //  moveChildNodes(node, d);
}

#if 0
void
CQChartsDendrogram::RootNode::
moveChildNodes(Node *hierNode, double d)
{
  if (! hierNode->isOpen())
    return;

  const auto &children = hierNode->getChildren();

  for (const auto &c : children_) {
    c.node->moveNode(d);

    moveChildNodes(c, d);
  }

  const auto &nodes = hierNode->getNodes();

  for (const auto &n : nodes) {
    n.node->moveNode(d);
  }
}
#endif

// recursively move node, higher nodes and parent
void
CQChartsDendrogram::RootNode::
compressNodeUp(Node *node, double d)
{
  node->compressNode(d);

  moveHigherNodes(node, d);

  auto *parent = node->parent();

  if (node->depth() > 1)
    compressNodeUp(parent, d);
}

// move all nodes higher than specified node
void
CQChartsDendrogram::RootNode::
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
    dnode->moveNode(d);
  }
}

// place node (set depth, row and number of rows)
void
CQChartsDendrogram::RootNode::
placeNode(Node *node, int depth, double row, double num_rows)
{
  node->setDepth  (depth);
  node->setRow    (row);
  node->setNumRows(num_rows);
  node->setPlaced (true);

  depthNodes_[size_t(depth)].push_back(node);
}

//------

CQChartsDendrogram::
CQChartsDendrogram()
{
}

CQChartsDendrogram::
~CQChartsDendrogram()
{
  delete root_;
}

void
CQChartsDendrogram::
setDebug(bool b)
{
  debug_ = b;

  if (root_)
    root_->setDebug(debug());
}

bool
CQChartsDendrogram::
singleStep() const
{
  if (root_)
    return root_->singleStep();
  else
    return false;
}

void
CQChartsDendrogram::
setSingleStep(bool b)
{
  if (root_)
    root_->setSingleStep(b);
}

CQChartsDendrogram::RootNode *
CQChartsDendrogram::
addRootNode(const QString &name)
{
  assert(! root_);

  root_ = createRootNode(name);

  return root_;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
addHierNode(Node *parentHier, const QString &name, const OptReal &value)
{
  auto *hier = createNode(parentHier, name);

  parentHier->addChild(hier, value);

  return hier;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
addNode(Node *hier, const QString &name, const OptReal &size)
{
  auto *node = createNode(hier, name, size);

  hier->addNode(node);

  return node;
}

CQChartsDendrogram::RootNode *
CQChartsDendrogram::
createRootNode(const QString &name) const
{
  auto *root = new RootNode(name);

  if (debug())
    root->setDebug(true);

  return root;
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
createNode(Node *hier, const QString &name, const OptReal &size) const
{
  return new Node(hier, name, size);
}

void
CQChartsDendrogram::
placeNodes()
{
  if (root_)
    root_->placeNodes();
}

void
CQChartsDendrogram::
compressNodes()
{
  if (root_)
    root_->compressNodes();
}

void
CQChartsDendrogram::
setOpenDepth(int depth)
{
  if (root_)
    root_->setOpenDepth(depth);
}

void
CQChartsDendrogram::
openNode(int depth, const QString &name)
{
  if (root_)
    root_->openNode(depth, name);
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
  if (root_)
    return root_->getNodeAtPoint(x, y, tol);

  return nullptr;
}

void
CQChartsDendrogram::
printGaps()
{
  if (root_)
    root_->printGaps();
}
