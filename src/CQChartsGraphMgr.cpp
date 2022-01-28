#include <CQChartsGraphMgr.h>
#include <CQChartsPlot.h>
#include <CQChartsRand.h>

#include <CMathRound.h>

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
  auto *node = createNode(name);

  node->setName(name);

  addNode(node);

  return node;
}

void
CQChartsGraphMgr::
addNode(Node *node) const
{
  auto *th = const_cast<CQChartsGraphMgr *>(this);

  node->setId(nameNodeMap_.size());

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(),
              NameNodeMap::value_type(node->str(), node));
  assert(node == (*p1).second);

  th->indNodeMap_[node->id()] = node;
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

  auto *edge = createEdge(value, srcNode, destNode);

  addEdge(edge);

  return edge;
}

void
CQChartsGraphMgr::
addEdge(Edge *edge) const
{
  auto *th = const_cast<CQChartsGraphMgr *>(this);

  th->edges_.push_back(edge);

  edge->setId(th->edges_.size());
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
  for (const auto &pn : nameNodeMap_)
    delete pn.second;

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

  //---

  // set max depth of all graph nodes
  updateMaxDepth(nodes);

  //---

  // place graph nodes at each position
  for (const auto &node : nodes) {
    int pos = calcPos(node);

    th->addDepthSize(pos, node->edgeSum());
    th->addDepthNode(pos, node);
  }

  //---

  // calc max height (fron node count) and max size (from value) for each x
  th->setMaxHeight(0);
  th->setTotalSize(0.0);

  for (const auto &depthNodes : depthNodesMap()) {
    const auto &nodes = depthNodes.second.nodes;
    double      size  = depthNodes.second.size;

    th->setMaxHeight(std::max(maxHeight(), int(nodes.size()))); // max number of nodes at pos
    th->setTotalSize(std::max(totalSize(), size));              // max sum of node sizes at pos
  }

  //---

  // calc perp value scale and margins to fit in bbox
  th->calcValueMarginScale();

  //---

  // place node objects at each depth (position)
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

  // calc max depth (source or dest) depending on align for pos calc
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
calcPos(Node *node) const
{
  int pos = 0;

  if (node->depth() >= 0) {
    pos = node->depth();
  }
  else {
    int maxNodeDepth = this->maxNodeDepth();

    int srcDepth  = node->srcDepth (); // min depth of previous nodes
    int destDepth = node->destDepth(); // max depth of subsequent nodes

    if      (srcDepth == 0) {
      if (mgr_->isAlignFirstLast()) {
        if (! node->destEdges().empty()) {
          node->setCalculating(true);

          int minDest = node->destDepth();

          for (const auto &edge : node->destEdges()) {
            auto *node1 = edge->destNode();

            if (! node1->isCalculating())
              minDest = std::min(minDest, calcPos(node1));
          }

          pos = std::max(minDest - 1, 0);

          node->setCalculating(false);
        }
        else
          pos = 0;
      }
      else
        pos = 0;
    }
    else if (destDepth == 0) {
      if (mgr_->isAlignFirstLast()) {
        if (! node->srcEdges().empty()) {
          node->setCalculating(true);

          int maxSrc = 0;

          for (const auto &edge : node->srcEdges()) {
            auto *node1 = edge->srcNode();

            if (! node1->isCalculating())
              maxSrc = std::max(maxSrc, calcPos(node1));
          }

          pos = std::min(maxSrc + 1, maxNodeDepth);

          node->setCalculating(false);
        }
        else
          pos = maxNodeDepth;
      }
      else
        pos = maxNodeDepth;
    }
    else {
      if      (mgr_->align() == GraphMgr::Align::SRC)
        pos = srcDepth;
      else if (mgr_->align() == GraphMgr::Align::DEST)
        pos = maxNodeDepth - destDepth;
      else if (mgr_->align() == GraphMgr::Align::JUSTIFY) {
        double f = 1.0*srcDepth/(srcDepth + destDepth);

        pos = int(f*maxNodeDepth);
      }
      else if (mgr_->align() == GraphMgr::Align::RAND) {
        CQChartsRand::RealInRange rand(0, mgr_->alignRand());

        pos = CMathRound::RoundNearest(rand.gen());
        assert(pos >= 0 && pos <= mgr_->alignRand());

        const_cast<Node *>(node)->setDepth(pos);
      }
    }
  }

  //--

  node->setPos(pos);

  return pos;
}

void
CQChartsGraphGraph::
calcValueMarginScale()
{
  // get perp node margins (in window coords ?)
  double nodeMargin = calcNodeMargin();

  //---

  double boxSize = 2.0; // default size of bbox

  if (bbox_.isSet()) {
  //boxSize = bbox_.getHeight();
    boxSize = std::max(bbox_.getWidth(), bbox_.getHeight());
  }

  double boxSize1 = nodeMargin*boxSize;
  double boxSize2 = boxSize - boxSize1; // size minus margin

  //---

  // calc value margin/scale
  setValueMargin(maxHeight() > 1.0 ? boxSize1/(maxHeight() - 1) : 0.0);
  setValueScale (totalSize() > 0.0 ? boxSize2/ totalSize()      : 1.0);
}

void
CQChartsGraphGraph::
placeDepthNodes() const
{
  // place node objects at each depth (pos)
  for (const auto &depthNodes : depthNodesMap()) {
    int         pos   = depthNodes.first;
    const auto &nodes = depthNodes.second.nodes;

    placeDepthSubNodes(pos, nodes);
  }
}

void
CQChartsGraphGraph::
placeDepthSubNodes(int pos, const Nodes &nodes) const
{
  auto *plot = mgr_->plot();

  // get node spacing (parallel to flow)
  double parallelMargin = calcNodeSpacing();

  // get delta to next node for each position (node size plus margin)
  //double posDelta;

  int maxNodeDepth = this->maxNodeDepth(); // max depth (one less the depth count)

  // get parallel size
  double parallelSize = (mgr_->isHorizontal() ? bbox_.getWidth() : bbox_.getHeight());

  //if (mgr_->isNodeScaled())
  //  posDelta = parallelSize/(maxNodeDepth + 1);
  //else
  //  posDelta = (maxNodeDepth > 0 ? parallelSize/maxNodeDepth : 0.0);

  // get node size in parallel and perp directions
  double nodePosSize, nodePerpSize;

  if (mgr_->isNodeScaled())
    nodePosSize = (parallelSize - maxNodeDepth*parallelMargin)/(maxNodeDepth + 1);
  else
    nodePosSize = (mgr_->isHorizontal() ? plot->lengthPlotWidth (mgr_->nodeWidth()) :
                                          plot->lengthPlotHeight(mgr_->nodeWidth()));

  if (mgr_->isNodePerpScaled()) {
    // get perp size
    double perpSize = (mgr_->isHorizontal() ? bbox_.getHeight() : bbox_.getWidth());

    int maxHeight = this->maxHeight();

    double nodeMargin = calcNodeMargin();

    if (maxHeight > 0)
      nodePerpSize = (perpSize - (maxHeight - 1)*nodeMargin)/maxHeight;
    else
      nodePerpSize = 0.0;
  }
  else {
    nodePerpSize = (mgr_->isHorizontal() ? plot->lengthPlotHeight(mgr_->nodeHeight()) :
                                           plot->lengthPlotWidth (mgr_->nodeHeight()));
  }

  //---

  // get scale factor for node value
  double valueScale = 1.0;

  if (mgr_->isNodeValueScaled())
    valueScale = std::max(this->valueScale(), 1.0);

  //---

  auto valueMargin = this->valueMargin();

  // calc start perp pos (placing top to bottom, left to right)
  double perpPos1;

  if (mgr_->isHorizontal()) {
    perpPos1  = bbox_.getYMid() + (nodePerpSize + valueMargin)*nodes.size()/2.0; // top
    perpPos1 -= valueMargin/2.0;
  }
  else {
    perpPos1  = bbox_.getXMid() - (nodePerpSize + valueMargin)*nodes.size()/2.0; // left
    perpPos1 += valueMargin/2.0;
  }

  //---

  for (const auto &node : nodes) {
    int pos1 = calcPos(node);
    assert(pos == pos1);

    //---

    // calc perp node size
    //double nodePerpSize1 = valueScale;

    //if (nodePerpSize1 <= 0.0)
    //  nodePerpSize1 = 0.1;

    //---

    // calc perp node size (scale by value if set)
    double nodePerpSize2 = nodePerpSize;

    if (mgr_->isNodeValueScaled())
      nodePerpSize2 = valueScale*node->edgeSum();

    //---

    // calc min/max node perp pos
    //double nodePerpMid = perpPos1 - nodePerpSize/2.0; // placement center

    double nodePerpPos1 = perpPos1;
    double nodePerpPos2 = perpPos1 + nodePerpSize2;

    //---

    // calc min/max parallel pos
    // (adjust align for first left/top edge (minPos) or right/bottom edge (maxPos))
    double posMid = (mgr_->isHorizontal() ?
     CMathUtil::map(pos, 0, maxNodeDepth,
                    bbox_.getXMin() + nodePosSize/2.0, bbox_.getXMax() - nodePosSize/2.0) :
     CMathUtil::map(pos, 0, maxNodeDepth,
                    bbox_.getYMax() - nodePosSize/2.0, bbox_.getYMin() + nodePosSize/2.0));

    double posStart = posMid - nodePosSize/2.0;
    double posEnd   = posMid + nodePosSize/2.0;

    if (posStart > posEnd) std::swap(posStart, posEnd);

    //---

    // get node shape
    auto shapeType = node->shapeType();

    if (shapeType == Node::ShapeType::NONE)
      shapeType = static_cast<Node::ShapeType>(mgr_->nodeShape());

    //---

    // set node rect based on shape
    BBox rect;

    if (shapeType != Node::ShapeType::NONE) {
      rect = CQChartsGeom::makeDirBBox(! mgr_->isHorizontal(),
        posStart, nodePerpPos1, posEnd, nodePerpPos2);
    }
    else {
      int srcDepth  = node->srcDepth ();
      int destDepth = node->destDepth();

      if      (srcDepth == 0) {
        rect = CQChartsGeom::makeDirBBox(! mgr_->isHorizontal(),
          posStart, nodePerpPos1, posEnd, nodePerpPos2); // no inputs (left/bottom align)
      }
      else if (destDepth == 0) {
        posStart -= nodePosSize; posEnd -= nodePosSize;

        rect = CQChartsGeom::makeDirBBox(! mgr_->isHorizontal(),
          posStart, nodePerpPos1, posEnd, nodePerpPos2); // no outputs (right/top align)
      }
      else {
        posStart -= nodePosSize/2.0; posEnd -= nodePosSize/2.0;

        rect = CQChartsGeom::makeDirBBox(! mgr_->isHorizontal(),
          posStart, nodePerpPos1, posEnd, nodePerpPos2); // center align
      }
    }

    //---

    node->setRect(rect);

    //---

    // move to next perp pos (down or right)
    if (mgr_->isHorizontal())
      perpPos1 -= nodePerpSize + valueMargin;
    else
      perpPos1 += nodePerpSize + valueMargin;
  }
}

double
CQChartsGraphGraph::
calcNodeMargin() const
{
  auto *plot = mgr_->plot();

  double margin = (mgr_->isHorizontal() ? plot->lengthPlotHeight(mgr_->nodeMargin()) :
                                          plot->lengthPlotWidth (mgr_->nodeMargin()));

  margin = std::min(std::max(margin, 0.0), 1.0);

  // get pixel margin perp to position axis
  auto pixelMargin = (mgr_->isHorizontal() ? plot->windowToPixelHeight(margin) :
                                             plot->windowToPixelWidth (margin));

  // stop margin from being too small
  if (pixelMargin < mgr_->minNodeMargin())
    margin = (mgr_->isHorizontal() ? plot->pixelToWindowHeight(mgr_->minNodeMargin()) :
                                     plot->pixelToWindowWidth (mgr_->minNodeMargin()));

  return margin;
}

double
CQChartsGraphGraph::
calcNodeSpacing() const
{
  auto *plot = mgr_->plot();

  double spacing = (mgr_->isHorizontal() ? plot->lengthPlotWidth (mgr_->nodeSpacing()) :
                                           plot->lengthPlotHeight(mgr_->nodeSpacing()));

  spacing = std::min(std::max(spacing, 0.0), 1.0);

  auto pixelSpacing = (mgr_->isHorizontal() ? plot->windowToPixelWidth (spacing) :
                                              plot->windowToPixelHeight(spacing));

  if (pixelSpacing < mgr_->minNodeMargin())
    spacing = (mgr_->isHorizontal() ? plot->pixelToWindowWidth (mgr_->minNodeMargin()) :
                                      plot->pixelToWindowHeight(mgr_->minNodeMargin()));

  return spacing;
}

bool
CQChartsGraphGraph::
adjustGraphNodes(const Nodes &nodes) const
{
  if (! mgr_->isAdjustNodes())
    return false;

  initPosNodesMap(nodes);

  //---

  int numPasses = mgr_->adjustIterations();

  for (int pass = 0; pass < numPasses; ++pass) {
    if (! adjustNodeCenters()) {
      //std::cerr << "adjustNodeCenters (#" << pass + 1 << " Passes)\n";
      break;
    }
  }

  //---

  removeOverlaps();

  //---

  reorderNodeEdges(nodes);

  //---

  return true;
}

void
CQChartsGraphGraph::
initPosNodesMap(const Nodes &nodes) const
{
  auto *th = const_cast<CQChartsGraphGraph *>(this);

  // get nodes by pos
  th->resetPosNodes();

  for (const auto &node : nodes)
    th->addPosNode(node);
}

bool
CQChartsGraphGraph::
adjustNodeCenters() const
{
  if (! mgr_->isAdjustCenters())
    return false;

  bool changed = false;

  if (adjustNodeCentersLtoR())
    changed = true;

  if (adjustNodeCentersRtoL())
    changed = true;

  return changed;
}

bool
CQChartsGraphGraph::
adjustNodeCentersLtoR() const
{
  if (! mgr_->isAdjustCenters())
    return false;

  if (mgr_->align() == CQChartsGraphMgr::Align::DEST)
    return false;

  // adjust nodes so centered on src nodes
  bool changed = false;

  // second to last minus one (last if SRC align)
  int startPos = 1;
  int endPos   = posNodesMap().size() - 1;

  if (mgr_->align() == CQChartsGraphMgr::Align::SRC)
    ++endPos;

  for (int pos = startPos; pos <= endPos; ++pos) {
    if (adjustPosNodes(pos))
      changed = true;
  }

  return changed;
}

bool
CQChartsGraphGraph::
adjustNodeCentersRtoL() const
{
  if (! mgr_->isAdjustCenters())
    return false;

  if (mgr_->align() == CQChartsGraphMgr::Align::SRC)
    return false;

  // adjust nodes so centered on dest nodes
  bool changed = false;

  // last minus one to second (first if DEST align)
  int startPos = 1;
  int endPos   = posNodesMap().size() - 1;

  if (mgr_->align() == CQChartsGraphMgr::Align::DEST)
    --startPos;

  for (int pos = endPos; pos >= startPos; --pos) {
    if (adjustPosNodes(pos))
      changed = true;
  }

  return changed;
}

bool
CQChartsGraphGraph::
reorderNodeEdges(const Nodes &nodes) const
{
  if (! mgr_->isReorderEdges())
    return false;

  //---

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
adjustPosNodes(int pos) const
{
  if (! hasPosNodes(pos))
    return false;

  bool changed = false;

  const auto &nodes = posNodes(pos);

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
  if (node->isFixed())
    return false;

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

  // calc average perp position
  double midPerpPos = 0.0;

  if (mgr_->isHorizontal()) {
    if      (srcBBox.isValid() && destBBox.isValid())
      midPerpPos = CMathUtil::avg(srcBBox.getYMid(), destBBox.getYMid());
    else if (srcBBox.isValid())
      midPerpPos = srcBBox.getYMid();
    else if (destBBox.isValid())
      midPerpPos = destBBox.getYMid();
    else
      return false;
  }
  else {
    if      (srcBBox.isValid() && destBBox.isValid())
      midPerpPos = CMathUtil::avg(srcBBox.getXMid(), destBBox.getXMid());
    else if (srcBBox.isValid())
      midPerpPos = srcBBox.getXMid();
    else if (destBBox.isValid())
      midPerpPos = destBBox.getXMid();
    else
      return false;
  }

  //---

  // move node to average
  if (mgr_->isHorizontal()) {
    double dy = midPerpPos - node->rect().getYMid();

    if (std::abs(dy) < 1E-6) // better tolerance
      return false;

    node->moveBy(Point(0.0, dy));
  }
  else {
    double dx = midPerpPos - node->rect().getXMid();

    if (std::abs(dx) < 1E-6) // better tolerance ?
      return false;

    node->moveBy(Point(dx, 0.0));
  }

  return true;
}

bool
CQChartsGraphGraph::
removeOverlaps() const
{
  if (! mgr_->isRemoveOverlaps())
    return false;

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

  double perpMargin = (mgr_->isHorizontal() ? plot->pixelToWindowHeight(mgr_->minNodeMargin()) :
                                              plot->pixelToWindowWidth (mgr_->minNodeMargin()));

  //---

  // get nodes sorted by perp position (max to min)
  PosNodeMap posNodeMap;

  createPosNodeMap(nodes, posNodeMap);

  //---

  // remove overlaps between nodes
  bool changed = false;

  Node *node1 = nullptr;

  for (const auto &posNode : posNodeMap) {
    auto *node2 = posNode.second;

    if (node1 && ! node2->isFixed()) {
      const auto &rect1 = node1->rect();
      const auto &rect2 = node2->rect();

      if (mgr_->isHorizontal()) {
        if (rect2.getYMax() >= rect1.getYMin() - perpMargin) {
          double dy = rect1.getYMin() - perpMargin - rect2.getYMax();

          if (std::abs(dy) > 1E-6) {
            node2->moveBy(Point(0.0, dy));
            changed = true;
          }
        }
      }
      else {
        if (rect2.getXMax() >= rect1.getXMin() - perpMargin) {
          double dx = rect1.getXMin() - perpMargin - rect2.getXMax();

          if (std::abs(dx) > 1E-6) {
            node2->moveBy(Point(dx, 0.0));
            changed = true;
          }
        }
      }
    }

    node1 = node2;
  }

  //---

  // move nodes back inside bbox (needed ?)
  if (node1) {
    const auto &rect1 = node1->rect();

    bool spread1 = false;

    if (mgr_->isHorizontal())
      spread1 = (rect1.getYMin() < bbox_.getYMin());
    else
      spread1 = (rect1.getXMin() < bbox_.getXMin());

    if (spread1)
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

  if (mgr_->isHorizontal()) {
    double dy1 = node1->rect().getHeight()/2.0; // top
    double dy2 = node2->rect().getHeight()/2.0; // bottom

    if (! spreadBBox.isValid() || (spreadBBox.getHeight() - dy1 - dy2) <= 0.0)
      return false;

    double ymin = bbox_.getYMin() + dy2;
    double ymax = bbox_.getYMax() - dy1;

    double dy      = ymin - node2->rect().getYMid();
    double boxSize = (ymax - ymin)/(spreadBBox.getHeight() - dy1 - dy2);

    if (CMathUtil::realEq(dy, 0.0) && CMathUtil::realEq(boxSize, 1.0))
      return false;

    for (const auto &posNode : posNodeMap) {
      auto *node = posNode.second;

      if (node->isFixed())
        continue;

      node->moveBy(Point(0.0, dy));

      double y1 = boxSize*(node->rect().getYMid() - ymin) + ymin;

      node->moveBy(Point(0.0, y1 - node->rect().getYMid()));
    }
  }
  else {
    double dx1 = node1->rect().getWidth()/2.0; // right
    double dx2 = node2->rect().getWidth()/2.0; // left

    if (! spreadBBox.isValid() || (spreadBBox.getWidth() - dx1 - dx2) <= 0.0)
      return false;

    double xmin = bbox_.getXMin() + dx2;
    double xmax = bbox_.getXMax() - dx1;

    double dx      = xmin - node2->rect().getXMid();
    double boxSize = (xmax - xmin)/(spreadBBox.getWidth() - dx1 - dx2);

    if (CMathUtil::realEq(dx, 0.0) && CMathUtil::realEq(boxSize, 1.0))
      return false;

    for (const auto &posNode : posNodeMap) {
      auto *node = posNode.second;

      if (node->isFixed())
        continue;

      node->moveBy(Point(dx, 0.0));

      double x1 = boxSize*(node->rect().getXMid() - xmin) + xmin;

      node->moveBy(Point(x1 - node->rect().getXMid(), 0.0));
    }
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

    double dist;

    // use distance from top/left (decreasing)
    if (mgr_->isHorizontal())
      dist = bbox_.getYMax() - rect.getYMid();
    else
      dist = bbox_.getXMin() - rect.getXMid();

    NodePos perpPos(dist, node->id());

    auto p = posNodeMap.find(perpPos);
    assert(p == posNodeMap.end());

    posNodeMap[perpPos] = node;
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

    double dist;

    // use distance from top/left (decreasing)
    if (mgr_->isHorizontal())
      dist = bbox_.getYMax() - rect.getYMid();
    else
      dist = bbox_.getXMin() - rect.getXMid();

    NodePos pos(dist, edge->id());

    auto p = posEdgeMap.find(pos);
    assert(p == posEdgeMap.end());

    posEdgeMap[pos] = edge;
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

  for (const auto &node : nodes()) {
    if (node->isFixed())
      continue;

    node->moveBy(delta);
  }
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
    if (node->isFixed())
      continue;

    auto p1 = node->rect().getCenter();

    double xc = p.x + (p1.x - p.x)*fx;
    double yc = p.y + (p1.y - p.y)*fy;

    node->moveBy(Point(xc - p1.x, yc - p1.y));
  }
}

//------

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
  assert(edge->destNode());

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
  assert(edge->destNode());

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

  // use visited to detect loops
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
      else {
        depth = std::max(depth, int(visited.size() - 1));
      //depth = std::max(depth, node->srcDepth());
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

  // use visited to detect loops
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
      else {
        depth = std::max(depth, int(visited.size() - 1));
      //depth = std::max(depth, node->destDepth());
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

  double nodeSize;

  if (mgr_->isHorizontal())
    nodeSize = rect().getHeight();
  else
    nodeSize = rect().getWidth();

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

    double perpPos3 = (mgr_->isHorizontal() ? y2 : x1); // top/left

    for (const auto &edge : this->srcEdges()) {
      if (! edge->hasValue()) {
        setSrcEdgeRect(edge, BBox());
        continue;
      }

      double perpSize2 = (total > 0.0 ? nodeSize*edge->value().real()/total : 0.0);

      double perpPos4 = (mgr_->isHorizontal() ? perpPos3 - perpSize2 : perpPos3 + perpSize2);

      if (! hasSrcEdgeRect(edge)) {
        BBox rect1;

        if (mgr_->isHorizontal())
          rect1 = BBox(x1, perpPos4, x2, perpPos3);
        else
          rect1 = BBox(perpPos3, y1, perpPos4, y2);

        setSrcEdgeRect(edge, rect1);
      }

      perpPos3 = perpPos4;
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

    double perpPos3 = (mgr_->isHorizontal() ? y2 : x1); // top/left

    for (const auto &edge : this->destEdges()) {
      if (! edge->hasValue()) {
        setDestEdgeRect(edge, BBox());
        continue;
      }

      double perpSize2 = (total > 0.0 ? nodeSize*edge->value().real()/total : 0.0);

      double perpPos4 = (mgr_->isHorizontal() ? perpPos3 - perpSize2 : perpPos3 + perpSize2);

      if (! hasDestEdgeRect(edge)) {
        BBox rect1;

        if (mgr_->isHorizontal())
          rect1 = BBox(x1, perpPos4, x2, perpPos3);
        else
          rect1 = BBox(perpPos3, y1, perpPos4, y2);

        setDestEdgeRect(edge, rect1);
      }

      perpPos3 = perpPos4;
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
