#include <CQChartsDendrogram.h>
#include <iostream>
#include <cmath>
#include <cassert>

struct NodeCmp {
  // sort reverse alphabetic no case
  bool operator()(const CQChartsDendrogram::Node *n1, const CQChartsDendrogram::Node *n2) {
    const QString &name1 = n1->name();
    const QString &name2 = n2->name();

    int l1 = name1.size();
    int l2 = name2.size();

    for (int i = 0; i < std::max(l1, l2); ++i) {
      QChar c1 = (i < l1 ? name1[i].toLower() : '\0');
      QChar c2 = (i < l2 ? name2[i].toLower() : '\0');

      if (c1 > c2) return true;
      if (c1 < c2) return false;
    }

    return false;
  }
};

//---

CQChartsDendrogram::Node::
Node(HierNode *parent, const QString &name, double size) :
 parent_(parent), id_(nextId()), name_(name), size_(size), open_(true)
{
}

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
  HierNode *hier;

  if (parent_) {
    hier = parent_;

    while (hier->parent())
      hier = hier->parent();
  }
  else
    hier = dynamic_cast<HierNode *>(this);

  return dynamic_cast<RootNode *>(hier);
}

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
  if (root()->debug())
    std::cerr << "Compress " << name().toStdString() << " by " << d << std::endl;

  setRow(row() - d);
  setGap(gap() - d);
}

bool
CQChartsDendrogram::Node::
isNodeAtPoint(double x, double y, double tol) const
{
  double d = std::hypot(this->x() - x, this->yc() - y);

  return (d <= tol);
}

//---

CQChartsDendrogram::HierNode::
HierNode(HierNode *parent, const QString &name) :
 Node(parent, name)
{
  if (parent_)
    parent_->children_.push_back(this);
}

CQChartsDendrogram::HierNode::
~HierNode()
{
  for (auto &c : children_)
    delete c;

  for (auto &n : nodes_)
    delete n;
}

double
CQChartsDendrogram::HierNode::
size() const
{
  double s = 0.0;

  for (const auto &c : children_)
    s += c->size();

  for (const auto &n : nodes_)
    s += n->size();

  return s;
}

int
CQChartsDendrogram::HierNode::
depth() const
{
  int max_depth = 0;

  if (isOpen()) {
    if (! nodes_.empty())
      max_depth = 1;

    for (const auto &c : children_)
      max_depth = std::max(max_depth, c->depth());
  }

  return max_depth + 1;
}

int
CQChartsDendrogram::HierNode::
numNodes() const
{
  return int(nodes_.size() + children_.size());
}

int
CQChartsDendrogram::HierNode::
maxNodes()
{
  int maxNum = 0;

  if (isOpen()) {
    maxNum = nodes_.size();

    for (const auto &c : children_)
      maxNum += c->maxNodes();
  }

  return std::max(maxNum, 1);
}

bool
CQChartsDendrogram::HierNode::
hasChildren() const
{
  return numNodes() > 0;
}

CQChartsDendrogram::HierNode *
CQChartsDendrogram::HierNode::
findChild(const QString &name) const
{
  for (const auto &c : children_)
    if (c->name() == name)
      return c;

  return nullptr;
}

void
CQChartsDendrogram::HierNode::
resetPlaced()
{
  Node::resetPlaced();

  for (const auto &c : children_)
    c->resetPlaced();

  for (const auto &n : nodes_)
    n->resetPlaced();
}

void
CQChartsDendrogram::HierNode::
placeSubNodes(RootNode *root, int depth, double row)
{
  // make single list of nodes to place
  Nodes nodes;

  for (const auto &c : children_)
    nodes.push_back(c);

  for (const auto &n : nodes_)
    nodes.push_back(n);

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
    auto hierNode = dynamic_cast<HierNode *>(n);

    if (hierNode && hierNode->isOpen())
      hierNode->placeSubNodes(root, depth + 1, row1);

    // move row by max rows
    row1 += maxNodes;
  }
}

void
CQChartsDendrogram::HierNode::
addNode(Node *node)
{
  nodes_.push_back(node);
}

void
CQChartsDendrogram::HierNode::
compressNode(double d)
{
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

const CQChartsDendrogram::Node *
CQChartsDendrogram::HierNode::
getNodeAtPoint(double x, double y, double tol) const
{
  return const_cast<CQChartsDendrogram::HierNode *>(this)->getNodeAtPoint(x, y, tol);
}

CQChartsDendrogram::Node *
CQChartsDendrogram::HierNode::
getNodeAtPoint(double x, double y, double tol)
{
  if (isNodeAtPoint(x, y, tol))
    return this;

  // make single list of nodes to place
  for (const auto &c : children_) {
    Node *node = c->getNodeAtPoint(x, y, tol);

    if (node)
      return node;
  }

  for (const auto &n : nodes_) {
    if (n->isNodeAtPoint(x, y, tol))
      return n;
  }

  return nullptr;
}

//---

CQChartsDendrogram::RootNode::
RootNode(const QString &name) :
 HierNode(0, name)
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
  int d = depth();

  depthNodes_.clear();

  depthNodes_.resize(d);

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
  int d = depthNodes_.size();

  for (int i = 0; i < d; ++i) {
    Nodes &dnodes = depthNodes_[i];

    int n = dnodes.size();

    // process nodes at depth
    double row = 0;

    for (int j = 0; j < n; ++j) {
      Node *node = dnodes[j];

      // calc space from multi-row node
      double ns = (node->numRows() - 1.0)/2.0;

      // calc space from multi-row node below
      if (j > 0) {
        Node *node1 = dnodes[j - 1];

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
  int d = depthNodes_.size();

  for (int i = 0; i < d; ++i) {
    const Nodes &dnodes = depthNodes_[i];

    int n = dnodes.size();

    for (int j = 0; j < n; ++j) {
      Node *node = dnodes[j];

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
  int d = depthNodes_.size();

  for (int i = d - 1; i > 0; --i) {
    Nodes &dnodes = depthNodes_[i];

    int n = dnodes.size();

    for (int j = 0; j < n; ++j) {
      Node *node = dnodes[j];

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

      HierNode *parent = node->parent();

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
  Nodes &dnodes = depthNodes_[node->depth()];

  int n = dnodes.size();

  int i = 0;

  for ( ; i < n; ++i)
    if (dnodes[i] == node)
      break;

  // check gaps of child nodes for node and all siblings above it
  Node *lowestChild = 0;

  for ( ; i < n; ++i) {
    Node *node1 = dnodes[i];

    if (node1->isOpen() && node1->hasChildren()) {
      auto hierNode = dynamic_cast<HierNode *>(node1);

      Node *child = getLowestChild(hierNode);

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
getLowestChild(HierNode *hierNode)
{
  Node *lowestNode = 0;

  const Children &children = hierNode->getChildren();

  for (const auto &c : children) {
    if (! lowestNode || c->row() < lowestNode->row())
      lowestNode = c;
  }

  const Nodes &nodes = hierNode->getNodes();

  for (const auto &n : nodes) {
    if (! lowestNode || n->row() < lowestNode->row())
      lowestNode = n;
  }

  return lowestNode;
}

// open all nodes down to depth and close all node below depth
void
CQChartsDendrogram::RootNode::
setOpenDepth(int depth)
{
  int d = depthNodes_.size();

  for (int i = 0; i < d; ++i) {
    Nodes &dnodes = depthNodes_[i];

    int n = dnodes.size();

    for (int j = 0; j < n; ++j) {
      Node *node = dnodes[j];

      node->setOpen(i < depth);
    }
  }
}

// open node with name at depth
void
CQChartsDendrogram::RootNode::
openNode(int depth, const QString &name)
{
  Nodes &dnodes = depthNodes_[depth];

  int n = dnodes.size();

  for (int j = 0; j < n; ++j) {
    Node *node = dnodes[j];

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
  //  moveChildNodes(dynamic_cast<HierNode *>(node), d);
}

#if 0
void
CQChartsDendrogram::RootNode::
moveChildNodes(HierNode *hierNode, double d)
{
  if (! hierNode->isOpen())
    return;

  const Children &children = hierNode->getChildren();

  for (const auto &c : children_) {
    c->moveNode(d);

    moveChildNodes(c, d);
  }

  const Nodes &nodes = hierNode->getNodes();

  for (const auto &n : nodes) {
    n->moveNode(d);
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

  HierNode *parent = node->parent();

  if (node->depth() > 1)
    compressNodeUp(parent, d);
}

// move all nodes higher than specified node
void
CQChartsDendrogram::RootNode::
moveHigherNodes(Node *node, double d)
{
  Nodes &dnodes = depthNodes_[node->depth()];

  // work down from top until hit node
  int n = dnodes.size();

  for (int i = n - 1; i >= 0; --i) {
    Node *dnode = dnodes[i];

    if (dnode == node) break;

    //if (dnode->hasChildren())
    //  moveChildNodes(dynamic_cast<HierNode *>(dnode), d);
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

  depthNodes_[depth].push_back(node);
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

bool
CQChartsDendrogram::
debug() const
{
  if (root_)
    return root_->debug();
  else
    return false;
}

void
CQChartsDendrogram::
setDebug(bool b)
{
  if (root_)
    root_->setDebug(b);
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

CQChartsDendrogram::HierNode *
CQChartsDendrogram::
createRootNode(const QString &name)
{
  assert(! root_);

  root_ = new RootNode(name);

  return root_;
}

CQChartsDendrogram::HierNode *
CQChartsDendrogram::
createHierNode(HierNode *hier, const QString &name)
{
  assert(hier);

  return new HierNode(hier, name);
}

CQChartsDendrogram::Node *
CQChartsDendrogram::
createNode(HierNode *hier, const QString &name, double size)
{
  Node *node = new Node(hier, name, size);

  hier->addNode(node);

  return node;
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
