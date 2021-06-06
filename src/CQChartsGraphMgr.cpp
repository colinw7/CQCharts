#include <CQChartsGraphMgr.h>
#include <CQChartsPlot.h>

#include <cassert>

CQChartsGraphMgr::
CQChartsGraphMgr(Plot *plot) :
 plot_(plot)
{
}

CQChartsGraphMgr::
~CQChartsGraphMgr()
{
}

bool
CQChartsGraphMgr::
hasGraph(int graphId) const
{
  return (graphs_.find(graphId) != graphs_.end());
}

void
CQChartsGraphMgr::
addGraph(int graphId, Graph *graph)
{
  graph->setId(graphId);

  graphs_[graphId] = graph;
}

CQChartsGraphMgr::Graph *
CQChartsGraphMgr::
getOrCreateGraph(int graphId, int parentGraphId) const
{
  if (! hasGraph(graphId)) {
    auto *th = const_cast<CQChartsGraphMgr *>(this);

    auto name = QString::number(graphId);

    auto *graph = createGraph(name);

    th->addGraph(graphId, graph);

    //---

    if (parentGraphId >= 0) {
      auto *parentGraph = getOrCreateGraph(parentGraphId, -1);

      graph->setParent(parentGraph);

      parentGraph->addChild(graph);
    }
  }

  return getGraph(graphId);
}

CQChartsGraphMgr::Graph *
CQChartsGraphMgr::
createGraph(const QString &name) const
{
  auto *th = const_cast<CQChartsGraphMgr *>(this);

  return new Graph(th, name);
}

CQChartsGraphMgr::Graph *
CQChartsGraphMgr::
getGraph(int graphId) const
{
  auto p = graphs_.find(graphId);
  assert(p != graphs_.end());

  return (*p).second;
}

void
CQChartsGraphMgr::
clearGraphs()
{
  for (auto &pg : graphs_)
    delete pg.second;

  graphs_.clear();
}

CQChartsGraphNode *
CQChartsGraphMgr::
findNode(const QString &name) const
{
  auto p = nameNodeMap_.find(name);

  if (p == nameNodeMap_.end())
    return nullptr;

  return (*p).second;
}

CQChartsGraphMgr::Node *
CQChartsGraphMgr::
addNode(const QString &name) const
{
  auto *th = const_cast<CQChartsGraphMgr *>(this);

  auto *node = createNode(name);

  node->setId(nameNodeMap_.size());

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(name, node));

  assert(node == (*p1).second);

  th->indNodeMap_[node->id()] = node;

  node->setName(name);

  return node;
}

CQChartsGraphMgr::Node *
CQChartsGraphMgr::
createNode(const QString &name) const
{
  auto *th = const_cast<CQChartsGraphMgr *>(this);

  return new Node(th, name);
}

CQChartsGraphEdge *
CQChartsGraphMgr::
addEdge(const OptReal &value, Node *srcNode, Node *destNode) const
{
  assert(srcNode && destNode);

  auto *th = const_cast<CQChartsGraphMgr *>(this);

  auto *edge = createEdge(value, srcNode, destNode);

  th->edges_.push_back(edge);

  edge->setId(th->edges_.size());

  return edge;
}

CQChartsGraphMgr::Edge *
CQChartsGraphMgr::
createEdge(const OptReal &value, Node *srcNode, Node *destNode) const
{
  auto *th = const_cast<CQChartsGraphMgr *>(this);

  return new Edge(th, value, srcNode, destNode);
}

void
CQChartsGraphMgr::
clearNodesAndEdges()
{
  for (const auto &pn : nameNodeMap_) {
    auto *node = pn.second;

    delete node;
  }

  nameNodeMap_.clear();
  indNodeMap_ .clear();

  //--

  for (auto *edge : edges_)
    delete edge;

  edges_.clear();
}

//----

CQChartsGraphGraph::
CQChartsGraphGraph(GraphMgr *mgr, const QString &str) :
 CQChartsGraphNode(mgr, str)
{
  shapeType_ = ShapeType::BOX;
}

CQChartsGraphGraph::
~CQChartsGraphGraph()
{
}

void
CQChartsGraphGraph::
addChild(Graph *graph)
{
  children_.push_back(graph);
}

void
CQChartsGraphGraph::
addNode(Node *node)
{
  srcDepth_  = -1;
  destDepth_ = -1;

  nodes_.push_back(node);
}

void
CQChartsGraphGraph::
removeAllNodes()
{
  srcDepth_  = -1;
  destDepth_ = -1;

  nodes_.clear();
}

//---

void
CQChartsGraphGraph::
setRect(const BBox &rect)
{
  // rect is always from nodes so adjust nodes to give rect
  updateRect();

  double fx = rect.getWidth ()/rect_.getWidth ();
  double fy = rect.getHeight()/rect_.getHeight();

  scale(fx, fy);

  updateRect();

  double dx = rect.getXMin() - rect_.getXMin();
  double dy = rect.getYMin() - rect_.getYMin();

  moveBy(Point(dx, dy));

  updateRect();
}

void
CQChartsGraphGraph::
updateRect()
{
  BBox bbox;

  for (const auto &child : children())
    bbox += child->rect();

  for (const auto &node : nodes())
    bbox += node->rect().getCenter();

  rect_ = bbox;
}

//---

// place nodes in graph
void
CQChartsGraphGraph::
placeGraph(const BBox &bbox) const
{
  if (isPlaced())
    return;

  //---

  auto *th = const_cast<CQChartsGraphGraph *>(this);

  th->bbox_ = bbox;

  //---

  // place children first
  for (const auto &child : children())
    child->place(bbox_);

  //---

  // get placeable nodes (nodes and sub graphs)
  auto nodes = placeableNodes();

  placeNodes(nodes);
}

void
CQChartsGraphGraph::
placeNodes(const Nodes &nodes) const
{
  auto *th = const_cast<CQChartsGraphGraph *>(this);

  // set max depth of all graph nodes
  updateMaxDepth(nodes);

  //---

  // place graph nodes at x position
  for (const auto &node : nodes) {
    int xpos = calcXPos(node);

    th->addDepthSize(xpos, node->edgeSum());
    th->addDepthNode(xpos, node);
  }

  //---

  // calc max height (fron node count) and max size (from value) for each x
  th->setMaxHeight(0);
  th->setTotalSize(0.0);

  for (const auto &depthNodes : depthNodesMap()) {
    const auto &nodes = depthNodes.second.nodes;
    double      size  = depthNodes.second.size;

    th->setMaxHeight(std::max(maxHeight(), int(nodes.size())));
    th->setTotalSize(std::max(totalSize(), size));
  }

  //---

  // calc y value scale and margins to fit in bbox
  th->calcValueMarginScale();

  //---

  // place node objects at each depth (xpos)
  placeDepthNodes();

  //---

  // adjust nodes in graph
  adjustGraphNodes(nodes);

  //---

  th->updateRect();

  //---

  th->setPlaced(true);
}

void
CQChartsGraphGraph::
updateMaxDepth(const Nodes &nodes) const
{
  auto *th = const_cast<CQChartsGraphGraph *>(this);

  // calc max depth (source or dest) depending on align for xpos calc
  bool set = false;

  th->setMinNodeDepth(0);
  th->setMaxNodeDepth(0);

  auto updateNodeDepth = [&](int depth) {
    if (! set) {
      th->setMinNodeDepth(depth);
      th->setMaxNodeDepth(depth);

      set = true;
    }
    else {
      th->setMinNodeDepth(std::min(minNodeDepth(), depth));
      th->setMaxNodeDepth(std::max(maxNodeDepth(), depth));
    }
  };

  for (const auto &node : nodes) {
    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    if      (mgr_->align() == GraphMgr::Align::SRC)
      updateNodeDepth(srcDepth);
    else if (mgr_->align() == GraphMgr::Align::DEST)
      updateNodeDepth(destDepth);
    else {
      updateNodeDepth(srcDepth);
      updateNodeDepth(destDepth);
    }
  }
}

int
CQChartsGraphGraph::
calcXPos(Node *node) const
{
  int xpos = 0;

  if (node->depth() >= 0) {
    xpos = node->depth();
  }
  else {
    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    if      (srcDepth == 0)
      xpos = 0;
    else if (destDepth == 0)
      xpos = maxNodeDepth();
    else {
      if      (mgr_->align() == GraphMgr::Align::SRC)
        xpos = srcDepth;
      else if (mgr_->align() == GraphMgr::Align::DEST)
        xpos = maxNodeDepth() - destDepth;
      else if (mgr_->align() == GraphMgr::Align::JUSTIFY) {
        double f = 1.0*srcDepth/(srcDepth + destDepth);

        xpos = int(f*maxNodeDepth());
      }
#if 0
      else if (ngr_->align() == GraphMgr::Align::RAND) {
        CQChartsRand::RealInRange rand(0, alignRand_);

        xpos = CMathRound::RoundNearest(rand.gen());

        const_cast<Node *>(node)->setDepth(xpos);
      }
#endif
    }
  }

  //--

  node->setXPos(xpos);

  return xpos;
}

void
CQChartsGraphGraph::
calcValueMarginScale()
{
  // get node margins
  double nodeYMargin = calcNodeYMargin();

  //---

  double boxSize = 2.0; // default size of bbox

  if (bbox_.isSet()) {
  //boxSize = bbox_.getHeight();
    boxSize = std::max(bbox_.getWidth(), bbox_.getHeight());
  }

  double boxSize1 = nodeYMargin*boxSize;
  double boxSize2 = boxSize - boxSize1; // size minus margin

  //---

  // calc value margin/scale
  setValueMargin(maxHeight() > 1.0 ? boxSize1/(maxHeight() - 1) : 0.0);
  setValueScale (maxHeight() > 0.0 ? boxSize2/ totalSize()      : 1.0);
}

double
CQChartsGraphGraph::
calcNodeYMargin() const
{
  auto *plot = mgr_->plot();

  double nodeYMargin = plot->lengthPlotHeight(mgr_->nodeYMargin());

  nodeYMargin = std::min(std::max(nodeYMargin, 0.0), 1.0);

  // get pixel margin perp to position axis
  auto pixelNodeYMargin = plot->windowToPixelHeight(nodeYMargin);

  // stop margin from being too small
  if (pixelNodeYMargin < mgr_->minNodeMargin())
    nodeYMargin = plot->pixelToWindowHeight(mgr_->minNodeMargin());

  return nodeYMargin;
}

void
CQChartsGraphGraph::
placeDepthNodes() const
{
  // place node objects at each depth (xpos)
  for (const auto &depthNodes : depthNodesMap()) {
    int         xpos  = depthNodes.first;
    const auto &nodes = depthNodes.second.nodes;

    placeDepthSubNodes(xpos, nodes);
  }
}

void
CQChartsGraphGraph::
placeDepthSubNodes(int xpos, const Nodes &nodes) const
{
  auto *plot = mgr_->plot();

  double xmargin = calcNodeXMargin();

  // place nodes to fit in bbox
  double xs = bbox_.getWidth ();
  double ys = bbox_.getHeight();

  double dx = 1.0;

  if (maxNodeDepth() > 0) {
    if (mgr_->isNodeXScaled())
      dx = xs/(maxNodeDepth() + 1);
    else
      dx = xs/maxNodeDepth();
  }

  double xm = plot->lengthPlotWidth (mgr_->nodeWidth());
  double ym = plot->lengthPlotHeight(mgr_->nodeWidth());

  if (mgr_->isNodeXScaled()) {
    xm = dx - xmargin;
    ym = xm;
  }

  //---

  double vs = valueScale();

  if (vs <= 0.0)
    vs = 1.0;

  // get sum of margins nodes at depth
  double height = valueMargin()*(maxHeight() - 1);

  height += maxHeight()*vs;

  //---

  // calc tip (placing top to bottom)
  double y1 = bbox_.getYMax() - (ys - height)/2.0;

  //---

  double nh = vs;

  double dy = nh + valueMargin();

  y1 = bbox_.getYMid() + dy*maxHeight()/2.0;

  y1 -= dy*(maxHeight() - nodes.size())/2.0;

  y1 -= valueMargin()/2.0;

  //---

  for (const auto &node : nodes) {
    // calc height
    double h = nh;

    if (h <= 0.0)
      h = 0.1;

    //---

    // calc rect
    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    int xpos1 = calcXPos(node);
    assert(xpos == xpos1);

    double x11 = bbox_.getXMin() + xpos*dx; // left
    double x12 = x11 + xm;

    double yc = y1 - h/2.0; // placement center

    double ym1 = ym;

    if (mgr_->isNodeYScaled())
      ym1 = vs*node->edgeSum();

    double y11 = yc - ym1/2.0;
    double y12 = yc + ym1/2.0;

    //---

    BBox rect;

    auto shapeType = node->shapeType();

    if (shapeType == Node::ShapeType::NONE)
      shapeType = (Node::ShapeType) mgr_->nodeShape();

    if (shapeType != Node::ShapeType::NONE) {
      rect = BBox(x11, y11, x12, y12);
    }
    else {
      if      (srcDepth == 0)
        rect = BBox(x11, y11, x12, y12); // no inputs (left align)
      else if (destDepth == 0) {
        x11 -= xm; x12 -= xm;

        rect = BBox(x11, y11, x12, y12); // no outputs (right align)
      }
      else {
        x11 -= xm/2.0; x12 -= xm/2.0;

        rect = BBox(x11, y11, x12, y12); // center align
      }
    }

    //---

    node->setRect(rect);

    //---

    y1 -= h + valueMargin();
  }
}

double
CQChartsGraphGraph::
calcNodeXMargin() const
{
  auto *plot = mgr_->plot();

  double nodeXMargin = plot->lengthPlotWidth(mgr_->nodeXMargin());

  nodeXMargin = std::min(std::max(nodeXMargin, 0.0), 1.0);

  auto pixelNodeXMargin = plot->windowToPixelWidth(nodeXMargin);

  if (pixelNodeXMargin < mgr_->minNodeMargin())
    nodeXMargin = plot->pixelToWindowWidth(mgr_->minNodeMargin());

  return nodeXMargin;
}

bool
CQChartsGraphGraph::
adjustGraphNodes(const Nodes &nodes) const
{
  initPosNodesMap(nodes);

  //---

  if (mgr_->isAdjustNodes()) {
    int numPasses = 25;

    for (int pass = 0; pass < numPasses; ++pass) {
      //std::cerr << "Pass " << pass << "\n";

      if (! adjustNodeCenters()) {
        //std::cerr << "adjustNodeCenters (#" << pass + 1 << " Passes)\n";
        break;
      }
    }

    //---

    reorderNodeEdges(nodes);
  }

  //---

//placeDepthNodes();

  //---

  return true;
}

void
CQChartsGraphGraph::
initPosNodesMap(const Nodes &nodes) const
{
  auto *th = const_cast<CQChartsGraphGraph *>(this);

  // get nodes by x pos
  th->resetPosNodes();

  for (const auto &node : nodes)
    th->addPosNode(node);
}

bool
CQChartsGraphGraph::
adjustNodeCenters() const
{
  // adjust nodes so centered on src nodes
  bool changed = false;

  // second to last
  int posNodesDepth = posNodesMap().size();

  for (int xpos = 1; xpos <= posNodesDepth; ++xpos) {
    if (adjustPosNodes(xpos))
      changed = true;
  }

  removeOverlaps();

  //---

  // second to last to first
  for (int xpos = posNodesDepth - 1; xpos >= 0; --xpos) {
    if (adjustPosNodes(xpos))
      changed = true;
  }

  removeOverlaps();

  return changed;
}

bool
CQChartsGraphGraph::
reorderNodeEdges(const Nodes &nodes) const
{
  bool changed = false;

  // sort node edges nodes by bbox
  for (const auto &node : nodes) {
    PosEdgeMap srcPosEdgeMap;

    createPosEdgeMap(node->srcEdges(), srcPosEdgeMap, /*isSrc*/true);

    if (srcPosEdgeMap.size() > 1) {
      Edges srcEdges;

      for (const auto &srcPosNode : srcPosEdgeMap)
        srcEdges.push_back(srcPosNode.second);

      node->setSrcEdges(srcEdges);

      changed = true;
    }

    //---

    PosEdgeMap destPosEdgeMap;

    createPosEdgeMap(node->destEdges(), destPosEdgeMap, /*isSrc*/false);

    if (destPosEdgeMap.size() > 1) {
      Edges destEdges;

      for (const auto &destPosNode : destPosEdgeMap)
        destEdges.push_back(destPosNode.second);

      node->setDestEdges(destEdges);

      changed = true;
    }
  }

  return changed;
}

bool
CQChartsGraphGraph::
adjustPosNodes(int xpos) const
{
  if (! hasPosNodes(xpos))
    return false;

  bool changed = false;

  const auto &nodes = posNodes(xpos);

  for (const auto &node : nodes) {
    if (adjustNode(node))
      changed = true;
  }

  return changed;
}

bool
CQChartsGraphGraph::
adjustNode(Node *node) const
{
  // get bounds of source edges
  BBox srcBBox;

  for (const auto &edge : node->srcEdges()) {
    if (edge->isSelf()) continue;

    auto *srcNode = edge->srcNode();
    if (! srcNode->isVisible()) continue;

    srcBBox += srcNode->rect();
  }

  //---

  // get bounds of dest edges
  BBox destBBox;

  for (const auto &edge : node->destEdges()) {
    if (edge->isSelf()) continue;

    auto *destNode = edge->destNode();
    if (! destNode->isVisible()) continue;

    destBBox += destNode->rect();
  }

  //---

  // calc average y
  double midY = 0.0;

  if      (srcBBox.isValid() && destBBox.isValid())
    midY = CMathUtil::avg(srcBBox.getYMid(), destBBox.getYMid());
  else if (srcBBox.isValid())
    midY = srcBBox.getYMid();
  else if (destBBox.isValid())
    midY = destBBox.getYMid();
  else
    return false;

  //---

  // move node to average
  double dy = midY - node->rect().getYMid();

  if (std::abs(dy) < 1E-6) // better tolerance
    return false;

  node->moveBy(Point(0.0, dy));

  return true;
}

bool
CQChartsGraphGraph::
removeOverlaps() const
{
  bool changed = false;

  for (const auto &posNodes : posNodesMap()) {
    if (removePosOverlaps(posNodes.second))
      changed = true;
  }

  return changed;
}

bool
CQChartsGraphGraph::
removePosOverlaps(const Nodes &nodes) const
{
  auto *plot = mgr_->plot();

  double ym = plot->pixelToWindowHeight(mgr_->minNodeMargin());

  //---

  // get nodes sorted by y (max to min)
  PosNodeMap posNodeMap;

  createPosNodeMap(nodes, posNodeMap);

  //---

  // remove overlaps between nodes
  bool changed = false;

  Node *node1 = nullptr;

  for (const auto &posNode : posNodeMap) {
    auto *node2 = posNode.second;

    if (node1) {
      const auto &rect1 = node1->rect();
      const auto &rect2 = node2->rect();

      if (rect2.getYMax() >= rect1.getYMin() - ym) {
        double dy = rect1.getYMin() - ym - rect2.getYMax();

        if (std::abs(dy) > 1E-6) {
          node2->moveBy(Point(0, dy));
          changed = true;
        }
      }
    }

    node1 = node2;
  }

  //---

  // move back inside bbox (needed ?)
  if (node1) {
    const auto &rect1 = node1->rect();

    if (rect1.getYMin() < bbox_.getYMin())
      spreadPosNodes(nodes);
  }

  return changed;
}

bool
CQChartsGraphGraph::
spreadPosNodes(const Nodes &nodes) const
{
  PosNodeMap posNodeMap;

  createPosNodeMap(nodes, posNodeMap);

  BBox spreadBBox;

  Node *node1 = nullptr, *node2 = nullptr;

  for (const auto &posNode : posNodeMap) {
    node2 = posNode.second;

    if (! node1) node1 = node2;

    spreadBBox += node2->rect();
  }

  if (! node1 || ! node2)
    return false;

  //---

  double dy1 = node1->rect().getHeight()/2.0; // top
  double dy2 = node2->rect().getHeight()/2.0; // bottom

  if (! spreadBBox.isValid() || (spreadBBox.getHeight() - dy1 - dy2) <= 0.0)
    return false;

  double ymin = bbox_.getYMin() + dy2;
  double ymax = bbox_.getYMax() - dy1;

  double dy = ymin - node2->rect().getYMid();
  double ys = (ymax - ymin)/(spreadBBox.getHeight() - dy1 - dy2);

  if (CMathUtil::realEq(dy, 0.0) && CMathUtil::realEq(ys, 1.0))
    return false;

  for (const auto &posNode : posNodeMap) {
    auto *node = posNode.second;

    node->moveBy(Point(0, dy));

    double y1 = ys*(node->rect().getYMid() - ymin) + ymin;

    node->moveBy(Point(0, y1 - node->rect().getYMid()));
  }

  return true;
}

void
CQChartsGraphGraph::
createPosNodeMap(const Nodes &nodes, PosNodeMap &posNodeMap) const
{
  for (const auto &node : nodes) {
    if (! node->isVisible()) continue;

    const auto &rect = node->rect();
    if (! rect.isValid()) continue;

    // use distance from top (decreasing)
    double y = bbox_.getYMax() - rect.getYMid();

    auto p = posNodeMap.find(y);

    while (p != posNodeMap.end()) {
      y -= 0.001;

      p = posNodeMap.find(y);
    }

    posNodeMap[y] = node;
  }
}

void
CQChartsGraphGraph::
createPosEdgeMap(const Edges &edges, PosEdgeMap &posEdgeMap, bool isSrc) const
{
  for (const auto &edge : edges) {
    auto *node = (isSrc ? edge->srcNode() : edge->destNode());
    if (! node->isVisible()) continue;

    const auto &rect = node->rect();
    if (! rect.isValid()) continue;

    // use distance from top (decreasing)
    double y = bbox_.getYMax() - rect.getYMid();

    auto p = posEdgeMap.find(y);

    while (p != posEdgeMap.end()) {
      y -= 0.001;

      p = posEdgeMap.find(y);
    }

    posEdgeMap[y] = edge;
  }
}

//---

int
CQChartsGraphGraph::
srcDepth() const
{
  if (srcDepth_ < 0) {
    auto *th = const_cast<CQChartsGraphGraph *>(this);

    th->srcDepth_ = 0;

    for (const auto &node : nodes()) {
      for (const auto &edge : node->srcEdges()) {
        auto *srcNode = edge->srcNode();
        if (! srcNode->isVisible()) continue;

        if (srcNode->graphId() != id())
          ++th->srcDepth_;
      }
    }
  }

  return srcDepth_;
}

int
CQChartsGraphGraph::
destDepth() const
{
  if (destDepth_ < 0) {
    auto *th = const_cast<CQChartsGraphGraph *>(this);

    th->destDepth_ = 0;

    for (const auto &node : nodes()) {
      for (const auto &edge : node->destEdges()) {
        auto *destNode = edge->destNode();

        if (! destNode->isVisible()) continue;

        if (destNode->graphId() != id())
          ++th->destDepth_;
      }
    }
  }

  return destDepth_;
}

//---

CQChartsGraphGraph::Nodes
CQChartsGraphGraph::
placeableNodes() const
{
  Nodes nodes;

  for (auto &child : this->children())
    nodes.push_back(child);

  for (auto &node : this->nodes())
    nodes.push_back(node);

  return nodes;
}

void
CQChartsGraphGraph::
place(const BBox &bbox) const
{
  if (isPlaced())
    return;

  placeGraph(bbox);
}

void
CQChartsGraphGraph::
moveBy(const Point &delta)
{
  Node::moveBy(delta);

  for (const auto &child : children())
    child->moveBy(delta);

  for (const auto &node : nodes())
    node->moveBy(delta);
}

void
CQChartsGraphGraph::
scale(double fx, double fy)
{
  auto p = rect().getCenter();

  for (const auto &child : children()) {
    auto p1 = child->rect().getCenter();

    child->scale(fx, fy);

    auto p2 = child->rect().getCenter();

    double xc = p.x + (p1.x - p.x)*fx;
    double yc = p.y + (p1.y - p.y)*fy;

    child->moveBy(Point(xc - p2.x, yc - p2.y));
  }

  for (const auto &node : nodes()) {
    auto p1 = node->rect().getCenter();

    double xc = p.x + (p1.x - p.x)*fx;
    double yc = p.y + (p1.y - p.y)*fy;

    node->moveBy(Point(xc - p1.x, yc - p1.y));
  }
}

//---

CQChartsGraphNode::
CQChartsGraphNode(GraphMgr *mgr, const QString &str) :
 mgr_(mgr), str_(str)
{
}

CQChartsGraphNode::
~CQChartsGraphNode()
{
}

void
CQChartsGraphNode::
addSrcEdge(Edge *edge, bool primary)
{
  edge->destNode()->parent_ = edge->srcNode();

  srcEdges_.push_back(edge);

  srcDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

void
CQChartsGraphNode::
addDestEdge(Edge *edge, bool primary)
{
  edge->destNode()->parent_ = edge->srcNode();

  destEdges_.push_back(edge);

  destDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

bool
CQChartsGraphNode::
hasDestNode(Node *destNode) const
{
  for (auto &destEdge : destEdges()) {
    if (destEdge->destNode() == destNode)
      return true;
  }

  return false;
}

int
CQChartsGraphNode::
srcDepth() const
{
  if (depth() >= 0)
    return depth() - 1;

  NodeSet visited;

  visited.insert(this);

  return calcSrcDepth(visited);
}

int
CQChartsGraphNode::
calcSrcDepth(NodeSet &visited) const
{
  if (srcDepth_ >= 0)
    return srcDepth_;

  auto *th = const_cast<CQChartsGraphNode *>(this);

  if (srcEdges_.empty()) {
    th->srcDepth_ = 0;
  }
  else {
    int depth = 0;

    for (const auto &edge : srcEdges_) {
      if (edge->isSelf()) continue;

      auto *node = edge->srcNode();

      auto p = visited.find(node);

      if (p == visited.end()) {
        visited.insert(node);

        depth = std::max(depth, node->calcSrcDepth(visited));
      }
    }

    th->srcDepth_ = depth + 1;
  }

  return srcDepth_;
}

int
CQChartsGraphNode::
destDepth() const
{
  if (depth() >= 0)
    return depth() + 1;

  NodeSet visited;

  visited.insert(this);

  return calcDestDepth(visited);
}

int
CQChartsGraphNode::
calcDestDepth(NodeSet &visited) const
{
  if (destDepth_ >= 0)
    return destDepth_;

  auto *th = const_cast<CQChartsGraphNode *>(this);

  if (destEdges_.empty()) {
    th->destDepth_ = 0;
  }
  else {
    int depth = 0;

    for (const auto &edge : destEdges_) {
      if (edge->isSelf()) continue;

      auto *node = edge->destNode();

      auto p = visited.find(node);

      if (p == visited.end()) {
        visited.insert(node);

        depth = std::max(depth, node->calcDestDepth(visited));
      }
    }

    th->destDepth_ = depth + 1;
  }

  return destDepth_;
}

const CQChartsGraphNode::BBox &
CQChartsGraphNode::
rect() const
{
  return rect_;
}

void
CQChartsGraphNode::
setRect(const BBox &rect)
{
  assert(rect.isSet());

  rect_ = rect;
}

CQChartsGraphGraph *
CQChartsGraphNode::
graph() const
{
  if (graphId_ < 0)
    return nullptr;

  return mgr_->getOrCreateGraph(graphId_, -1);
}

double
CQChartsGraphNode::
edgeSum() const
{
  double sum = std::max(srcEdgeSum(), destEdgeSum());

  if (CMathUtil::realEq(sum, 0.0))
    sum = 1.0;

  return sum;
}

double
CQChartsGraphNode::
srcEdgeSum() const
{
  double value = 0.0;

  for (const auto &edge : srcEdges_) {
    if (edge->hasValue())
      value += edge->value().real();
  }

  return value;
}

double
CQChartsGraphNode::
destEdgeSum() const
{
  double value = 0.0;

  for (const auto &edge : destEdges_) {
    if (edge->hasValue())
      value += edge->value().real();
  }

  return value;
}

//---

void
CQChartsGraphNode::
moveBy(const Point &delta)
{
  rect_.moveBy(delta);

  //---

  moveSrcEdgeRectsBy (delta);
  moveDestEdgeRectsBy(delta);
}

void
CQChartsGraphNode::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);
}

//---

void
CQChartsGraphNode::
placeEdges()
{
  double x1 = rect().getXMin();
  double x2 = rect().getXMax();
  double y1 = rect().getYMin();
  double y2 = rect().getYMax();

  clearSrcEdgeRects ();
  clearDestEdgeRects();

  if (this->srcEdges().size() == 1) {
    auto *edge = *this->srcEdges().begin();

    setSrcEdgeRect(edge, BBox(x1, y1, x2, y2));
  }
  else {
    double total = 0.0;

    for (const auto &edge : this->srcEdges()) {
      if (edge->hasValue())
        total += edge->value().real();
    }

    double y3 = y2; // top

    for (const auto &edge : this->srcEdges()) {
      if (! edge->hasValue()) {
        setSrcEdgeRect(edge, BBox());
        continue;
      }

      double h1 = (total > 0.0 ? (y2 - y1)*edge->value().real()/total : 0.0);
      double y4 = y3 - h1;

      if (! hasSrcEdgeRect(edge))
        setSrcEdgeRect(edge, BBox(x1, y4, x2, y3));

      y3 = y4;
    }
  }

  //---

  if (this->destEdges().size() == 1) {
    auto *edge = *this->destEdges().begin();

    setDestEdgeRect(edge, BBox(x1, y1, x2, y2));
  }
  else {
    double total = 0.0;

    for (const auto &edge : this->destEdges()) {
      if (edge->hasValue())
        total += edge->value().real();
    }

    double y3 = y2; // top

    for (const auto &edge : this->destEdges()) {
      if (! edge->hasValue()) {
        setDestEdgeRect(edge, BBox());
        continue;
      }

      double h1 = (total > 0.0 ? (y2 - y1)*edge->value().real()/total : 0.0);
      double y4 = y3 - h1;

      if (! hasDestEdgeRect(edge))
        setDestEdgeRect(edge, BBox(x1, y4, x2, y3));

      y3 = y4;
    }
  }
}

void
CQChartsGraphNode::
setSrcEdgeRect(Edge *edge, const BBox &bbox)
{
  srcEdgeRect_[edge] = bbox;
}

void
CQChartsGraphNode::
setDestEdgeRect(Edge *edge, const BBox &bbox)
{
  destEdgeRect_[edge] = bbox;
}

//------

CQChartsGraphEdge::
CQChartsGraphEdge(GraphMgr *mgr, const OptReal &value, Node *srcNode, Node *destNode) :
 mgr_(mgr), value_(value), srcNode_(srcNode), destNode_(destNode)
{
}

CQChartsGraphEdge::
~CQChartsGraphEdge()
{
}
