#include <CQChartsSankeyPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsModelDetails.h>
#include <CQChartsNamePair.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>


CQChartsSankeyPlotType::
CQChartsSankeyPlotType()
{
}

void
CQChartsSankeyPlotType::
addParameters()
{
  startParameterGroup("Sankey");

  addColumnParameter("link", "Source/Target", "linkColumn").
    setRequired().setTip("Name pair for Source/Target connection");

  addColumnParameter("value", "Value", "valueColumn").
    setRequired().setTip("Connection value");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsSankeyPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Sankey Plot").
    h3("Summary").
     p("Draw connected objects as a connected flow graph.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/sankey.png"));
}

bool
CQChartsSankeyPlotType::
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
{
  if (parameter->name() == "link") {
    if (columnDetails->type() == CQChartsPlot::ColumnType::NAME_PAIR)
      return true;

    return false;
  }

  return CQChartsPlotType::isColumnForParameter(columnDetails, parameter);
}

CQChartsPlot *
CQChartsSankeyPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsSankeyPlot(view, model);
}

//------

CQChartsSankeyPlot::
CQChartsSankeyPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("sankey"), model),
 CQChartsObjTextData     <CQChartsSankeyPlot>(this),
 CQChartsObjNodeShapeData<CQChartsSankeyPlot>(this),
 CQChartsObjEdgeShapeData<CQChartsSankeyPlot>(this)
{
  NoUpdate noUpdate(this);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  CQChartsColor bg(CQChartsColor::Type::PALETTE);

  setNodeFilled(true);
  setNodeFillColor(bg);
  setNodeFillAlpha(1.00);

  setNodeStroked(true);
  setNodeStrokeAlpha(0.2);

  setEdgeFilled(true);
  setEdgeFillColor(bg);
  setEdgeFillAlpha(0.25);

  setEdgeStroked(true);
  setEdgeStrokeAlpha(0.2);

  //---

  bbox_ = CQChartsGeom::BBox(-1.0, -1.0, 1.0, 1.0);
}

CQChartsSankeyPlot::
~CQChartsSankeyPlot()
{
  clearNodesAndEdges();
}

void
CQChartsSankeyPlot::
clearNodesAndEdges()
{
  for (const auto &nameNode : nameNodeMap_)
    delete nameNode.second;

  for (const auto &edge : edges_)
    delete edge;

  nameNodeMap_.clear();
  indNodeMap_ .clear();
  edges_      .clear();
}

//---

void
CQChartsSankeyPlot::
setLinkColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(linkColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsSankeyPlot::
setAlign(const Align &a)
{
  CQChartsUtil::testAndSet(align_, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsSankeyPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  addProp("columns", "linkColumn" , "link" , "Link column");
  addProp("columns", "valueColumn", "value", "Value column");

  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  addProp("edge/stroke", "edgeStroked", "visible", "Edge steoke visible");

  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  addProp("edge/fill", "edgeFilled", "visible", "Edit fill visible");

  addFillProperties("edge/fill", "edgeFill", "Edge");

  addProp("text", "textVisible", "visible", "Text label visible");
  addProp("text", "align"      , "align"  , "Text label align");

  addTextProperties("text", "text", "");
}

CQChartsGeom::Range
CQChartsSankeyPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::calcRange");

  CQChartsGeom::Range dataRange;

  QAbstractItemModel *model = this->model().data();

  if (! model)
    return dataRange;

  if (bbox_.isSet()) {
    double xm = bbox_.getHeight()*0.01;
    double ym = bbox_.getWidth ()*0.01;

    dataRange.updateRange(bbox_.getXMin() - xm, bbox_.getYMin() - ym);
    dataRange.updateRange(bbox_.getXMax() + xm, bbox_.getYMax() + ym);
  }

  return dataRange;
}

bool
CQChartsSankeyPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsSankeyPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  CQChartsSankeyPlot *th = const_cast<CQChartsSankeyPlot *>(this);

  th->clearNodesAndEdges();

  //---

  QAbstractItemModel *model = this->model().data();

  if (! model)
    return false;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsSankeyPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      bool ok1, ok2;

      QString linkStr = plot_->modelString(data.row, plot_->linkColumn (), data.parent, ok1);
      double  value   = plot_->modelReal  (data.row, plot_->valueColumn(), data.parent, ok2);

      if (! ok1 || ! ok2)
        return State::SKIP;

      CQChartsNamePair namePair(linkStr);

      if (! namePair.isValid())
        return State::SKIP;

      QString srcStr  = namePair.name1();
      QString destStr = namePair.name2();

      CQChartsSankeyPlotNode *srcNode  = plot_->findNode(srcStr);
      CQChartsSankeyPlotNode *destNode = plot_->findNode(destStr);

      CQChartsSankeyPlotEdge *edge = plot_->createEdge(value, srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);

      return State::OK;
    }

   private:
    const CQChartsSankeyPlot *plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  createGraph(objs);

  return true;
}

void
CQChartsSankeyPlot::
createGraph(PlotObjs &objs) const
{
  CQChartsSankeyPlot *th = const_cast<CQChartsSankeyPlot *>(this);

  //---

//double xs = bbox_.getWidth ();
//double ys = bbox_.getHeight();
  double ys = 2.0;

  //---

  updateMaxDepth();

  //---

  using DepthNodesMap = std::map<int,IndNodeMap>;
  using DepthSizeMap  = std::map<int,double>;

  DepthNodesMap depthNodesMap;
  DepthSizeMap  depthSizeMap;

  for (const auto &idNode : indNodeMap_) {
    CQChartsSankeyPlotNode *node = idNode.second;

    int xpos = node->calcXPos();

    depthSizeMap[xpos] += node->edgeSum();

    depthNodesMap[xpos][node->ind()] = node;
  }

  //---

  th->maxHeight_ = 0;

  for (const auto &depthNodes : depthNodesMap)
    th->maxHeight_ = std::max(maxHeight_, int(depthNodes.second.size()));

  //---

  double totalSize = 0.0;

  for (const auto &depthSize : depthSizeMap)
    totalSize = std::max(totalSize, depthSize.second);

  //---

  th->margin_ = (maxHeight_ > 1 ? 0.2*ys/(maxHeight_ - 1) : 0.0);

  th->valueScale_ = (totalSize > 0 ? ys/totalSize : 0.0);

  //---

  for (const auto &depthNodes : depthNodesMap)
    createNodes(depthNodes.second);

  //--

  for (const auto &edge : edges_)
    createEdge(edge);

  //---

  adjustNodes();

  //---

  for (const auto &idNode : indNodeMap_) {
    CQChartsSankeyPlotNode *node = idNode.second;

    objs.push_back(node->obj());
  }

  for (const auto &edge : edges_)
    objs.push_back(edge->obj());
}

void
CQChartsSankeyPlot::
createNodes(const IndNodeMap &nodes) const
{
  double xs = bbox_.getWidth ();
  double ys = bbox_.getHeight();

  double dx = xs/maxDepth();

  double xm = pixelToWindowWidth(16);

  //---

  double height = margin_*(nodes.size() - 1);

  for (const auto &idNode : nodes) {
    CQChartsSankeyPlotNode *node = idNode.second;

    height += valueScale()*node->edgeSum();
  }

  //---

  int numNodes = this->numNodes();

  double y1 = bbox_.getYMax() - (ys - height)/2.0;

  for (const auto &idNode : nodes) {
    CQChartsSankeyPlotNode *node = idNode.second;

    // draw src box
    double h = valueScale()*node->edgeSum();

    //---

    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    int xpos = node->calcXPos();

    double x = bbox_.getXMin() + xpos*dx;

    double y2 = y1 - h;

    CQChartsGeom::BBox rect;

    if      (srcDepth == 0)
      rect = CQChartsGeom::BBox(x, y1, x + xm, y2);
    else if (destDepth == 0)
      rect = CQChartsGeom::BBox(x - xm, y1, x, y2);
    else
      rect = CQChartsGeom::BBox(x - xm/2, y1, x + xm/2, y2);

    ColorInd iv(node->ind(), numNodes);

    CQChartsSankeyNodeObj *nodeObj =
      new CQChartsSankeyNodeObj(this, rect, node, iv);

    node->setObj(nodeObj);

    //---

    y1 = y2 - margin_;
  }
}

void
CQChartsSankeyPlot::
createEdge(CQChartsSankeyPlotEdge *edge) const
{
  double xm = bbox_.getHeight()*0.01;
  double ym = bbox_.getWidth ()*0.01;

  CQChartsGeom::BBox rect(bbox_.getXMin() - xm, bbox_.getYMin() - ym,
                          bbox_.getXMax() + xm, bbox_.getYMax() + ym);

  CQChartsSankeyEdgeObj *edgeObj = new CQChartsSankeyEdgeObj(this, rect, edge);

  edge->setObj(edgeObj);
}

void
CQChartsSankeyPlot::
updateMaxDepth() const
{
  CQChartsSankeyPlot *th = const_cast<CQChartsSankeyPlot *>(this);

  //---

  th->bbox_ = CQChartsGeom::BBox(-1.0, -1.0, 1.0, 1.0);

  int maxDepth = 0;

  for (const auto &idNode : indNodeMap_) {
    CQChartsSankeyPlotNode *node = idNode.second;

    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    if      (align() == CQChartsSankeyPlot::Align::SRC)
      maxDepth = std::max(maxDepth, srcDepth);
    else if (align() == CQChartsSankeyPlot::Align::DEST)
      maxDepth = std::max(maxDepth, destDepth);
    else
      maxDepth = std::max(std::max(maxDepth, srcDepth), destDepth);
  }

  th->maxDepth_ = maxDepth;
}

void
CQChartsSankeyPlot::
adjustNodes() const
{
  CQChartsSankeyPlot *th = const_cast<CQChartsSankeyPlot *>(this);

  //---

  // update range
  th->bbox_ = CQChartsGeom::BBox();

  for (const auto &idNode : indNodeMap_) {
    CQChartsSankeyPlotNode *node = idNode.second;

    th->bbox_ += node->obj()->rect();
  }

  th->dataRange_ = calcRange();

  //---

  th->initPosNodesMap();

  //---

  int numPasses = 25;

  for (int pass = 0; pass < numPasses; ++pass) {
    //std::cerr << "Pass " << pass << "\n";

    th->adjustNodeCenters();
  }

  //---

  reorderNodeEdges();
}

void
CQChartsSankeyPlot::
initPosNodesMap()
{
  // get nodes by x pos
  posNodesMap_ = PosNodesMap();

  for (const auto &idNode : indNodeMap_) {
    CQChartsSankeyPlotNode *node = idNode.second;

    posNodesMap_[node->xpos()][node->ind()] = node;
  }
}

void
CQChartsSankeyPlot::
adjustNodeCenters()
{
  // adjust nodes so centered on src nodes

  // second to last
  for (int xpos = 1; xpos <= maxDepth(); ++xpos) {
    const IndNodeMap &indNodeMap = posNodesMap_[xpos];

    for (const auto &idNode : indNodeMap) {
      CQChartsSankeyPlotNode *node = idNode.second;

      adjustNode(node);
    }
  }

  removeOverlaps();

  // second to last to first
  for (int xpos = maxDepth() - 1; xpos >= 0; --xpos) {
    const IndNodeMap &indNodeMap = posNodesMap_[xpos];

    for (const auto &idNode : indNodeMap) {
      CQChartsSankeyPlotNode *node = idNode.second;

      adjustNode(node);
    }
  }

  removeOverlaps();
}

void
CQChartsSankeyPlot::
removeOverlaps() const
{
  using PosNodeMap = std::map<double,CQChartsSankeyPlotNode *>;

  double ym = pixelToWindowHeight(4);

  for (const auto &posNodes : posNodesMap_) {
    const IndNodeMap &indNodeMap = posNodes.second;

    // get nodes sorted by y (max to min)
    PosNodeMap posNodeMap;

    for (const auto &idNode : indNodeMap) {
      CQChartsSankeyPlotNode *node = idNode.second;

      const CQChartsGeom::BBox &rect = node->obj()->rect();

      double y = bbox_.getYMax() - rect.getYMid();

      auto p = posNodeMap.find(y);

      while (p != posNodeMap.end()) {
        y -= 0.001;

        p = posNodeMap.find(y);
      }

      posNodeMap[y] = node;
    }

    //---

    // remove overlaps between nodes
    CQChartsSankeyPlotNode *node1 = nullptr;

    for (const auto &posNode : posNodeMap) {
      CQChartsSankeyPlotNode *node2 = posNode.second;

      if (node1) {
        const CQChartsGeom::BBox &rect1 = node1->obj()->rect();
        const CQChartsGeom::BBox &rect2 = node2->obj()->rect();

        if (rect2.getYMax() >= rect1.getYMin() - ym) {
          double dy = rect1.getYMin() - ym - rect2.getYMax();

          node2->obj()->moveBy(CQChartsGeom::Point(0, dy));
        }
      }

      node1 = node2;
    }

    // move back inside bbox
    if (node1) {
      const CQChartsGeom::BBox &rect1 = node1->obj()->rect();

      if (rect1.getYMin() < bbox_.getYMin()) {
        double dy = bbox_.getYMin() - rect1.getYMin();

        for (const auto &idNode : indNodeMap) {
          CQChartsSankeyPlotNode *node = idNode.second;

          node->obj()->moveBy(CQChartsGeom::Point(0, dy));
        }
      }
    }
  }
}

void
CQChartsSankeyPlot::
reorderNodeEdges() const
{
  // sort node edges nodes by bbox
  using PosEdgeMap = std::map<double,CQChartsSankeyPlotEdge *>;

  for (const auto &idNode : indNodeMap_) {
    CQChartsSankeyPlotNode *node = idNode.second;

    //---

    PosEdgeMap srcPosEdgeMap;

    for (const auto &edge : node->srcEdges()) {
      CQChartsSankeyPlotNode *srcNode = edge->srcNode();

      const CQChartsGeom::BBox &rect = srcNode->obj()->rect();

      double y = bbox_.getYMax() - rect.getYMid();

      auto p = srcPosEdgeMap.find(y);

      while (p != srcPosEdgeMap.end()) {
        y -= 0.001;

        p = srcPosEdgeMap.find(y);
      }

      srcPosEdgeMap[y] = edge;
    }

    Edges srcEdges;

    for (const auto &srcPosNode : srcPosEdgeMap)
      srcEdges.push_back(srcPosNode.second);

    //---

    PosEdgeMap destPosEdgeMap;

    for (const auto &edge : node->destEdges()) {
      CQChartsSankeyPlotNode *destNode = edge->destNode();

      const CQChartsGeom::BBox &rect = destNode->obj()->rect();

      double y = bbox_.getYMax() - rect.getYMid();

      auto p = destPosEdgeMap.find(y);

      while (p != destPosEdgeMap.end()) {
        y -= 0.001;

        p = destPosEdgeMap.find(y);
      }

      destPosEdgeMap[y] = edge;
    }

    Edges destEdges;

    for (const auto &destPosNode : destPosEdgeMap)
      destEdges.push_back(destPosNode.second);

    //---

    node->setSrcEdges (srcEdges);
    node->setDestEdges(destEdges);
  }
}

void
CQChartsSankeyPlot::
adjustNode(CQChartsSankeyPlotNode *node) const
{
  CQChartsGeom::BBox bbox;

  for (const auto &edge : node->srcEdges()) {
    CQChartsSankeyPlotNode *srcNode = edge->srcNode();

    bbox += srcNode->obj()->rect();
  }

  for (const auto &edge : node->destEdges()) {
    CQChartsSankeyPlotNode *destNode = edge->destNode();

    bbox += destNode->obj()->rect();
  }

  double dy = bbox.getYMid() - node->obj()->rect().getYMid();

  node->obj()->moveBy(CQChartsGeom::Point(0, dy));
}

CQChartsSankeyPlotNode *
CQChartsSankeyPlot::
findNode(const QString &name) const
{
  auto p = nameNodeMap_.find(name);

  if (p != nameNodeMap_.end())
    return (*p).second;

  //---

  CQChartsSankeyPlotNode *node = new CQChartsSankeyPlotNode(this, name);

  node->setInd(nameNodeMap_.size());

  CQChartsSankeyPlot *th = const_cast<CQChartsSankeyPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(name, node));

  th->indNodeMap_[node->ind()] = node;

  return (*p1).second;
}

CQChartsSankeyPlotEdge *
CQChartsSankeyPlot::
createEdge(double value, CQChartsSankeyPlotNode *srcNode, CQChartsSankeyPlotNode *destNode) const
{
  CQChartsSankeyPlotEdge *edge = new CQChartsSankeyPlotEdge(this, value, srcNode, destNode);

  CQChartsSankeyPlot *th = const_cast<CQChartsSankeyPlot *>(this);

  th->edges_.push_back(edge);

  return edge;
}

void
CQChartsSankeyPlot::
keyPress(int key, int modifier)
{
  if (key == Qt::Key_A) {
    adjustNodes();

    drawObjs();
  }
  else
    CQChartsPlot::keyPress(key, modifier);
}

//------

CQChartsSankeyPlotNode::
CQChartsSankeyPlotNode(const CQChartsSankeyPlot *plot, const QString &str) :
 plot_(plot), str_(str)
{
}

CQChartsSankeyPlotNode::
~CQChartsSankeyPlotNode()
{
}

void
CQChartsSankeyPlotNode::
addSrcEdge(CQChartsSankeyPlotEdge *edge)
{
  edge->destNode()->parent_ = edge->srcNode();

  srcEdges_.push_back(edge);

  srcDepth_ = -1;
}

void
CQChartsSankeyPlotNode::
addDestEdge(CQChartsSankeyPlotEdge *edge)
{
  edge->destNode()->parent_ = edge->srcNode();

  destEdges_.push_back(edge);

  destDepth_ = -1;
}

int
CQChartsSankeyPlotNode::
srcDepth() const
{
  NodeSet visited;

  visited.insert(this);

  return srcDepth(visited);
}

int
CQChartsSankeyPlotNode::
srcDepth(NodeSet &visited) const
{
  if (srcDepth_ >= 0)
    return srcDepth_;

  CQChartsSankeyPlotNode *th = const_cast<CQChartsSankeyPlotNode *>(this);

  if (srcEdges_.empty())
    th->srcDepth_ = 0;
  else {
    int depth = 0;

    for (const auto &edge : srcEdges_) {
      CQChartsSankeyPlotNode *node = edge->srcNode();

      auto p = visited.find(node);

      if (p == visited.end()) {
        visited.insert(node);

        depth = std::max(depth, node->srcDepth(visited));
      }
    }

    th->srcDepth_ = depth + 1;
  }

  return srcDepth_;
}

int
CQChartsSankeyPlotNode::
destDepth() const
{
  NodeSet visited;

  visited.insert(this);

  return destDepth(visited);
}

int
CQChartsSankeyPlotNode::
destDepth(NodeSet &visited) const
{
  if (destDepth_ >= 0)
    return destDepth_;

  CQChartsSankeyPlotNode *th = const_cast<CQChartsSankeyPlotNode *>(this);

  if (destEdges_.empty())
    th->destDepth_ = 0;
  else {
    int depth = 0;

    for (const auto &edge : destEdges_) {
      CQChartsSankeyPlotNode *node = edge->destNode();

      auto p = visited.find(node);

      if (p == visited.end()) {
        visited.insert(node);

        depth = std::max(depth, node->destDepth(visited));
      }
    }

    th->destDepth_ = depth + 1;
  }

  return destDepth_;
}

int
CQChartsSankeyPlotNode::
calcXPos() const
{
  int srcDepth  = this->srcDepth ();
  int destDepth = this->destDepth();

  int xpos = 0;

  if      (srcDepth == 0)
    xpos = 0;
  else if (destDepth == 0)
    xpos = plot_->maxDepth();
  else {
    if      (plot_->align() == CQChartsSankeyPlot::Align::SRC)
      xpos = srcDepth;
    else if (plot_->align() == CQChartsSankeyPlot::Align::DEST)
      xpos = plot_->maxDepth() - destDepth;
    else if (plot_->align() == CQChartsSankeyPlot::Align::JUSTIFY) {
      double f = 1.0*srcDepth/(srcDepth + destDepth);

      xpos = int(f*plot_->maxDepth());
    }
  }

  CQChartsSankeyPlotNode *th = const_cast<CQChartsSankeyPlotNode *>(this);

  th->xpos_ = xpos;

  return xpos;
}

double
CQChartsSankeyPlotNode::
edgeSum() const
{
  return std::max(srcEdgeSum(), destEdgeSum());
}

double
CQChartsSankeyPlotNode::
srcEdgeSum() const
{
  double value = 0.0;

  for (const auto &edge : srcEdges_)
    value += edge->value();

  return value;
}

double
CQChartsSankeyPlotNode::
destEdgeSum() const
{
  double value = 0.0;

  for (const auto &edge : destEdges_)
    value += edge->value();

  return value;
}

void
CQChartsSankeyPlotNode::
setObj(CQChartsSankeyNodeObj *obj)
{
  obj_ = obj;
}

//------

CQChartsSankeyPlotEdge::
CQChartsSankeyPlotEdge(const CQChartsSankeyPlot *plot, double value,
                       CQChartsSankeyPlotNode *srcNode, CQChartsSankeyPlotNode *destNode) :
 plot_(plot), value_(value), srcNode_(srcNode), destNode_(destNode)
{
}

CQChartsSankeyPlotEdge::
~CQChartsSankeyPlotEdge()
{
}

void
CQChartsSankeyPlotEdge::
setObj(CQChartsSankeyEdgeObj *obj)
{
  obj_ = obj;
}

//------

CQChartsSankeyNodeObj::
CQChartsSankeyNodeObj(const CQChartsSankeyPlot *plot, const CQChartsGeom::BBox &rect,
                      CQChartsSankeyPlotNode *node, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsSankeyPlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), node_(node)
{
  double x1 = rect.getXMin();
  double x2 = rect.getXMax();
  double y3 = rect.getYMax();

  for (const auto &edge : node_->srcEdges()) {
    double h1 = plot_->valueScale()*edge->value();

    double y4 = y3 - h1;

    auto p = srcEdgeRect_.find(edge);

    if (p == srcEdgeRect_.end())
      srcEdgeRect_[edge] = CQChartsGeom::BBox(x1, y4, x2, y3);

    y3 = y4;
  }

  y3 = rect.getYMax();

  for (const auto &edge : node->destEdges()) {
    double h1 = plot_->valueScale()*edge->value();

    double y4 = y3 - h1;

    auto p = destEdgeRect_.find(edge);

    if (p == destEdgeRect_.end())
      destEdgeRect_[edge] = CQChartsGeom::BBox(x1, y4, x2, y3);

    y3 = y4;
  }
}

QString
CQChartsSankeyNodeObj::
calcId() const
{
  //double value = node_->edgeSum();

  return QString("%1:%2").arg(typeName()).arg(iv_.i);
}

void
CQChartsSankeyNodeObj::
moveBy(const CQChartsGeom::Point &delta)
{
  //std::cerr << "  Move " << node_->str().toStdString() << " by " << delta.y << "\n";

  rect_.moveBy(delta);

  for (auto &edgeRect : srcEdgeRect_)
    edgeRect.second.moveBy(delta);

  for (auto &edgeRect : destEdgeRect_)
    edgeRect.second.moveBy(delta);
}

void
CQChartsSankeyNodeObj::
draw(CQChartsPaintDevice *device)
{
  //int numNodes = plot_->numNodes();

  // set fill and stroke
  ColorInd ic = calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpNodeStrokeColor(ic);
  QColor fc = plot_->interpNodeFillColor  (ic);

  plot_->setPenBrush(pen, brush,
    plot_->isNodeStroked(), bc, plot_->nodeStrokeAlpha(),
    plot_->nodeStrokeWidth(), plot_->nodeStrokeDash(),
    plot_->isNodeFilled(), fc, plot_->nodeFillAlpha(), plot_->nodeFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setBrush(brush);
  device->setPen  (pen);

  //---

  // draw node
  double x1 = rect_.getXMin();
  double y1 = rect_.getYMin();
  double x2 = rect_.getXMax();
  double y2 = rect_.getYMax();

  QPainterPath path;

  path.moveTo(QPointF(x1, y1));
  path.lineTo(QPointF(x2, y1));
  path.lineTo(QPointF(x2, y2));
  path.lineTo(QPointF(x1, y2));

  path.closeSubpath();

  device->drawPath(path);
}

void
CQChartsSankeyNodeObj::
drawFg(CQChartsPaintDevice *device) const
{
  if (! plot_->isTextVisible())
    return;

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect_);

  //int numNodes = plot_->numNodes();

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  QFontMetricsF fm(device->font());

  //---

  // set text pen
  ColorInd ic = calcColorInd();

  QPen pen;

  QColor c = plot_->interpTextColor(ic);

  plot_->setPen(pen, true, c, plot_->textAlpha());

  device->setPen(pen);

  //---

  QString str = node_->str();

  double tx = (rect_.getXMid() < 0.5 ? prect.getXMax() + 4 : prect.getXMin() - 4 - fm.width(str));
  double ty = prect.getYMid() + fm.ascent()/2;

  QPointF pt = device->pixelToWindow(QPointF(tx, ty));

  if (plot_->isTextContrast())
    CQChartsDrawUtil::drawContrastText(device, pt, str);
  else
    CQChartsDrawUtil::drawSimpleText(device, pt, str);
}

//------

CQChartsSankeyEdgeObj::
CQChartsSankeyEdgeObj(const CQChartsSankeyPlot *plot, const CQChartsGeom::BBox &rect,
                      CQChartsSankeyPlotEdge *edge) :
 CQChartsPlotObj(const_cast<CQChartsSankeyPlot *>(plot), rect), plot_(plot), edge_(edge)
{
}

QString
CQChartsSankeyEdgeObj::
calcId() const
{
  return QString("%1:%2:%3:%4").arg(typeName()).arg(edge_->srcNode()->str()).
          arg(edge_->destNode()->str()).arg(edge_->value());
}

bool
CQChartsSankeyEdgeObj::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::Point p1 = plot_->windowToPixel(p);

  return path_.contains(p1.qpoint());
}

void
CQChartsSankeyEdgeObj::
draw(CQChartsPaintDevice *device)
{
  int numNodes = plot_->numNodes();

  // set fill and stroke
  ColorInd ic1(edge_->srcNode ()->ind(), numNodes);
  ColorInd ic2(edge_->destNode()->ind(), numNodes);

  QPen   pen;
  QBrush brush;

  QColor fc1 = plot_->interpEdgeFillColor(ic1);
  QColor fc2 = plot_->interpEdgeFillColor(ic2);

  QColor fc = CQChartsUtil::blendColors(fc1, fc2, 0.5);

  QColor sc1 = plot_->interpEdgeStrokeColor(ic1);
  QColor sc2 = plot_->interpEdgeStrokeColor(ic2);

  QColor sc = CQChartsUtil::blendColors(sc1, sc2, 0.5);

  plot_->setPenBrush(pen, brush,
    plot_->isEdgeStroked(), sc, plot_->edgeStrokeAlpha(),
    plot_->edgeStrokeWidth(), plot_->edgeStrokeDash(),
    plot_->isEdgeFilled(), fc, plot_->edgeFillAlpha(), plot_->edgeFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setBrush(brush);
  device->setPen  (pen);

  //---

  // draw edge
  const CQChartsGeom::BBox &srcRect  = edge_->srcNode ()->obj()->destEdgeRect(edge_);
  const CQChartsGeom::BBox &destRect = edge_->destNode()->obj()->srcEdgeRect (edge_);

  double x1 = srcRect .getXMax();
  double x2 = destRect.getXMin();

  double y11 = srcRect .getYMax();
  double y12 = srcRect .getYMin();
  double y21 = destRect.getYMax();
  double y22 = destRect.getYMin();

  path_ = QPainterPath();

  double x3 = x1 + (x2 - x1)/3.0;
  double x4 = x2 - (x2 - x1)/3.0;

  path_.moveTo (QPointF(x1, y11));
  path_.cubicTo(QPointF(x3, y11), QPointF(x4, y21), QPointF(x2, y21));
  path_.lineTo (QPointF(x2, y22));
  path_.cubicTo(QPointF(x4, y22), QPointF(x3, y12), QPointF(x1, y12));

  path_.closeSubpath();

  //---

  device->drawPath(path_);
}
