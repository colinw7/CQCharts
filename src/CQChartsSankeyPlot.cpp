#include <CQChartsSankeyPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsConnectionList.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsArrow.h>
#include <CQChartsEditHandles.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsRand.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CMathRound.h>

#include <deque>

CQChartsSankeyPlotType::
CQChartsSankeyPlotType()
{
}

void
CQChartsSankeyPlotType::
addParameters()
{
  CQChartsConnectionPlotType::addParameters();
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
isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsSankeyPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
}

CQChartsPlot *
CQChartsSankeyPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsSankeyPlot(view, model);
}

//------

CQChartsSankeyPlot::
CQChartsSankeyPlot(View *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("sankey"), model),
 CQChartsObjTextData      <CQChartsSankeyPlot>(this),
 CQChartsObjNodeShapeData <CQChartsSankeyPlot>(this),
 CQChartsObjEdgeShapeData <CQChartsSankeyPlot>(this),
 CQChartsObjGraphShapeData<CQChartsSankeyPlot>(this)
{
  NoUpdate noUpdate(this);

  setLayerActive(Layer::Type::FG_PLOT, true);

  //---

  Color bg(Color::Type::PALETTE);

  setNodeFilled(true);
  setNodeFillColor(bg);
  setNodeFillAlpha(Alpha(1.0));

  setNodeStroked(true);
  setNodeStrokeAlpha(Alpha(0.2));

  //---

  setEdgeFilled(true);
  setEdgeFillColor(bg);
  setEdgeFillAlpha(Alpha(0.25));

  setEdgeStroked(true);
  setEdgeStrokeAlpha(Alpha(0.2));

  //---

  setGraphFilled (false);
  setGraphStroked(false);

  //---

  addTitle();

  //---

  bbox_ = targetBBox_;

  setFitMargin(PlotMargin(Length("5%"), Length("5%"), Length("5%"), Length("5%")));
}

CQChartsSankeyPlot::
~CQChartsSankeyPlot()
{
  clearNodesAndEdges();

  clearGraphs();
}

void
CQChartsSankeyPlot::
clearNodesAndEdges()
{
  for (const auto &pn : nameNodeMap_) {
    auto *node = pn.second;

    delete node;
  }

  for (const auto &edge : edges_)
    delete edge;

  nameNodeMap_.clear();
  indNodeMap_ .clear();
  edges_      .clear();

  clearGraphs();

  groupValueInd_.clear();

  maxNodeDepth_ = 0;
}

//---

void
CQChartsSankeyPlot::
setNodeXMargin(double r)
{
  CQChartsUtil::testAndSet(nodeXMargin_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setNodeYMargin(double r)
{
  CQChartsUtil::testAndSet(nodeYMargin_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setNodeWidth(double r)
{
  CQChartsUtil::testAndSet(nodeWidth_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsSankeyPlot::
setEdgeLine(bool b)
{
  CQChartsUtil::testAndSet(edgeLine_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsSankeyPlot::
setSrcColoring(bool b)
{
  CQChartsUtil::testAndSet(srcColoring_, b, [&]() { drawObjs(); } );
}

void
CQChartsSankeyPlot::
setAlign(const Align &a)
{
  CQChartsUtil::testAndSet(align_, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsSankeyPlot::
setAdjustNodes(bool b)
{
  CQChartsUtil::testAndSet(adjustNodes_, b, [&]() { updateRangeAndObjs(); } );
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

  CQChartsConnectionPlot::addProperties();

  //---

  // options
  addProp("options", "adjustNodes", "adjustNodes", "Adjust node placement");
  addProp("options", "align"      , "align"  , "Node alignment");

  // node
  addProp("node", "nodeXMargin", "marginX", "Node X margin");
  addProp("node", "nodeYMargin", "marginY", "Node Y margin");
  addProp("node", "nodeWidth"  , "width"  , "Node width (in pixels)");

  // coloring
  addProp("coloring", "srcColoring"  , "srcColoring"  , "Color by Source Nodes");
  addProp("coloring", "mouseColoring", "mouseColoring", "Mouse Over Connection Coloring");

  // node style
  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  //---

  // edge
  addProp("edge", "edgeLine", "line", "Draw line for edge");

  // edge style
  addProp("edge/stroke", "edgeStroked", "visible", "Edge stroke visible");

  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  addProp("edge/fill", "edgeFilled", "visible", "Edit fill visible");

  addFillProperties("edge/fill", "edgeFill", "Edge");

  //---

  // graph style
  addProp("graph/stroke", "graphStroked", "visible", "Graph stroke visible");

  addLineProperties("graph/stroke", "graphStroke", "Graph");

  addProp("graph/fill", "graphFilled", "visible", "Graph fill visible");

  addFillProperties("graph/fill", "graphFill", "Graph");

  //---

  // text
  addProp("text", "textVisible", "visible", "Text label visible");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH);
}

//---

CQChartsGeom::Range
CQChartsSankeyPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::calcRange");

//auto *th = const_cast<CQChartsSankeyPlot *>(this);

//th->nodeYSet_ = false;

  Range dataRange;

  auto *model = this->model().data();

  if (! model)
    return dataRange;

  //---

  dataRange.updateRange(bbox_.getLL());
  dataRange.updateRange(bbox_.getUR());

  //---

#if 0
  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }
#endif

  //---

#if 0
  double xm = (boxMargin_ > 0.0 ? dataRange.xsize()*boxMargin_ : 0.0);
  double ym = (boxMargin_ > 0.0 ? dataRange.ysize()*boxMargin_ : 0.0);

  dataRange.updateRange(dataRange.xmin() - xm, dataRange.ymin() - ym);
  dataRange.updateRange(dataRange.xmax() + xm, dataRange.ymax() + ym);
#endif

  return dataRange;
}

CQChartsGeom::Range
CQChartsSankeyPlot::
getCalcDataRange() const
{
  //auto range = CQChartsPlot::getCalcDataRange();

#if 0
  if (nodeYSet_) {
    range.setBottom(nodeYMin_);
    range.setTop   (nodeYMax_);
  }
#endif

  //return range;

  return Range(bbox_.getXMin(), bbox_.getYMax(), bbox_.getXMax(), bbox_.getYMin());
}

CQChartsGeom::Range
CQChartsSankeyPlot::
objTreeRange() const
{
  BBox bbox = nodesBBox();

  return Range(bbox.getXMin(), bbox.getYMax(), bbox.getXMax(), bbox.getYMin());
}

CQChartsGeom::BBox
CQChartsSankeyPlot::
nodesBBox() const
{
  // calc bounding box of all nodes (all graphs)
  BBox bbox;

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    bbox += node->rect();
  }

  return bbox;
}

#if 0
void
CQChartsSankeyPlot::
setNodeYRange()
{
  // calc placed range (all graphs) (needed ?)
  nodeYMin_ = 0.0;
  nodeYMax_ = 0.0;

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    nodeYMin_ = std::min(nodeYMin_, node->rect().getYMin());
    nodeYMax_ = std::max(nodeYMax_, node->rect().getYMax());
  }
}
#endif

//------

bool
CQChartsSankeyPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsSankeyPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  th->clearErrors();

  //---

//th->nodeYSet_  = false;

  //---

  // check columns
  if (! checkColumns())
    return false;

  //---

  // init objects
  th->clearNodesAndEdges();

  auto *model = this->model().data();
  if (! model) return false;

  //---

  // create objects
  bool rc = true;

  if (isHierarchical())
    rc = initHierObjs();
  else {
    if      (linkColumn().isValid() && valueColumn().isValid())
      rc = initLinkObjs();
    else if (connectionsColumn().isValid())
      rc = initConnectionObjs();
    else if (pathColumn().isValid())
      rc = initPathObjs();
    else if (fromColumn().isValid() && toColumn().isValid())
      rc = initFromToObjs();
    else
      rc = initTableObjs();
  }

  if (! rc)
    return false;

  //---

  th->filterObjs();

  //---

  // create graph (and place)
  createObjsGraph(objs);

  //---

  return true;
}

void
CQChartsSankeyPlot::
fitToBBox(const BBox &bbox)
{
  // TODO: center at 0, 0 after fit

  // current
  double x1 = bbox_.getXMin  ();
  double y1 = bbox_.getYMin  ();
  double w1 = bbox_.getWidth ();
  double h1 = bbox_.getHeight();

  // target
  double x2 = bbox.getXMin  ();
  double y2 = bbox.getYMin  ();
  double w2 = bbox.getWidth ();
  double h2 = bbox.getHeight();

  double xf = w2/w1;
  double yf = h2/h1;

  double f = std::min(xf, yf);

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    double nx1 = x2 + (node->rect().getXMin() - x1)*f;
    double ny1 = y2 + (node->rect().getYMin() - y1)*f;
  //double nx2 = x2 + (node->rect().getXMax() - x1)*f;
  //double ny2 = y2 + (node->rect().getYMax() - y1)*f;

    node->scale(f, f);

    node->moveBy(Point(nx1 - node->rect().getXMin(), ny1 - node->rect().getYMin()));
  }

  //---

  if (graph_)
    graph_->updateRect();
}

//------

bool
CQChartsSankeyPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initHierObjs");

  return CQChartsConnectionPlot::initHierObjs();
}

void
CQChartsSankeyPlot::
initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  int srcDepth = srcHierData.linkStrs.size();

  Node *srcNode  = nullptr;
  Node *destNode = nullptr;

  initHierObjsAddConnection(srcHierData.parentStr, destHierData.parentStr, srcDepth,
                            destHierData.total, srcNode, destNode);

  if (srcNode) {
    QString srcStr;

    if (! srcHierData.linkStrs.empty())
      srcStr = srcHierData.linkStrs.back();

    srcNode->setValue(OptReal(destHierData.total));
    srcNode->setName (srcStr);
  }
}

void
CQChartsSankeyPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  int srcDepth = srcHierData.linkStrs.size();

  Node *srcNode  = nullptr;
  Node *destNode = nullptr;

  initHierObjsAddConnection(srcHierData.parentStr, destHierData.parentStr, srcDepth,
                            destHierData.total, srcNode, destNode);

  if (destNode) {
    QString destStr;

    if (! destHierData.linkStrs.empty())
      destStr = destHierData.linkStrs.back();

    destNode->setValue(OptReal(destHierData.total));
    destNode->setName (destStr);
  }
}

void
CQChartsSankeyPlot::
initHierObjsAddConnection(const QString &srcStr, const QString &destStr, int srcDepth,
                          double value, Node* &srcNode, Node* &destNode) const
{
  int destDepth = srcDepth + 1;

  if (maxDepth() <= 0 || srcDepth <= maxDepth())
    srcNode = findNode(srcStr);

  if (maxDepth() <= 0 || destDepth <= maxDepth())
    destNode = findNode(destStr);

  auto *edge = (srcNode && destNode ? createEdge(OptReal(value), srcNode, destNode) : nullptr);

  if (edge) {
    srcNode ->addDestEdge(edge);
    destNode->addSrcEdge (edge);
  }

  if (srcNode)
    srcNode->setDepth(srcDepth);

  if (destNode)
    destNode->setDepth(destDepth);
}

//---

bool
CQChartsSankeyPlot::
initPathObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initPathObjs");

  //---

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  th->maxNodeDepth_ = 0;

  //--

  if (! CQChartsConnectionPlot::initPathObjs())
    return false;

  //---

  if (isPropagate())
    th->propagatePathValues();

  return true;
}

void
CQChartsSankeyPlot::
addPathValue(const QStringList &pathStrs, double value) const
{
  int n = pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  QChar separator = (this->separator().length() ? this->separator()[0] : '/');

  QString path1 = pathStrs[0];

  for (int i = 1; i < n; ++i) {
    QString path2 = path1 + separator + pathStrs[i];

    auto *srcNode  = findNode(path1);
    auto *destNode = findNode(path2);

    srcNode ->setLabel(pathStrs[i - 1]);
    destNode->setLabel(pathStrs[i    ]);

    srcNode ->setDepth(i - 1);
    destNode->setDepth(i    );

    if (i < n - 1) {
      bool hasEdge = srcNode->hasDestNode(destNode);

      if (! hasEdge) {
        auto *edge = createEdge(OptReal(), srcNode, destNode);

        srcNode ->addDestEdge(edge);
        destNode->addSrcEdge (edge);
      }
    }
    else {
      auto *edge = createEdge(OptReal(), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);

      destNode->setValue(OptReal(value));
    }

    path1 = path2;
  }
}

void
CQChartsSankeyPlot::
propagatePathValues()
{
  // propagate node value up through edges and parent nodes
  for (int depth = maxNodeDepth_; depth >= 0; --depth) {
    for (const auto &p : nameNodeMap_) {
      auto *node = p.second;
      if (node->depth() != depth) continue;

      // set node value from sum of dest values
      if (! node->hasValue()) {
        if (! node->destEdges().empty()) {
          OptReal sum;

          for (const auto &edge : node->destEdges()) {
            if (edge->hasValue()) {
              double value = edge->value().real();

              if (sum.isSet())
                sum = OptReal(sum.real() + value);
              else
                sum = OptReal(value);
            }
          }

          if (sum.isSet())
            node->setValue(sum);
        }
      }

      // propagate set node value up to source nodes
      if (node->hasValue()) {
        if (! node->srcEdges().empty()) {
          assert(node->srcEdges().size() == 1);

          auto *srcEdge = *node->srcEdges().begin();

          if (! srcEdge->hasValue())
            srcEdge->setValue(node->value());

          auto *srcNode = srcEdge->srcNode();

          for (const auto &edge : srcNode->destEdges()) {
            if (edge->destNode() == node)
              edge->setValue(node->value());
          }
        }
      }
    }
  }
}

//---

bool
CQChartsSankeyPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initFromToObjs");

  return CQChartsConnectionPlot::initFromToObjs();
}

void
CQChartsSankeyPlot::
addFromToValue(const QString &fromStr, const QString &toStr, double value,
               const CQChartsNameValues &nameValues, const GroupData &groupData) const
{
  auto *srcNode = findNode(fromStr);

  //---

  if (groupData.ng > 1)
    srcNode->setGroup(groupData.ig, groupData.ng);
  else
    srcNode->setGroup(-1);

  //---

  // Just node
  if (toStr == "") {
    for (const auto &nv : nameValues.nameValues()) {
      QString value = nv.second.toString();

      if      (nv.first == "shape") {
      }
      else if (nv.first == "num_sides") {
      }
      else if (nv.first == "label") {
        srcNode->setLabel(value);
      }
      else if (nv.first == "color") {
        srcNode->setColor(QColor(value));
      }
    }
  }
  else {
    auto *destNode = findNode(toStr);

    auto *edge = createEdge(OptReal(value), srcNode, destNode);

    srcNode ->addDestEdge(edge, /*primary*/true );
    destNode->addSrcEdge (edge, /*primary*/false);

    for (const auto &nv : nameValues.nameValues()) {
      QString value = nv.second.toString();

      if      (nv.first == "shape") {
      }
      else if (nv.first == "label") {
      }
    }
  }
}

//---

bool
CQChartsSankeyPlot::
initLinkObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initLinkObjs");

  return CQChartsConnectionPlot::initLinkObjs();
}

void
CQChartsSankeyPlot::
addLinkConnection(const LinkConnectionData &linkConnectionData) const
{
  auto *srcNode  = findNode(linkConnectionData.srcStr);
  auto *destNode = findNode(linkConnectionData.destStr);
//assert(srcNode != destNode);

  auto *edge = createEdge(OptReal(linkConnectionData.value), srcNode, destNode);

  srcNode ->addDestEdge(edge);
  destNode->addSrcEdge (edge);

  destNode->setValue(OptReal(linkConnectionData.value));

  if (linkConnectionData.groupData.isValid())
    srcNode->setGroup(linkConnectionData.groupData.ig, linkConnectionData.groupData.ng);
  else
    srcNode->setGroup(-1);

  if (linkConnectionData.nameModelInd.isValid()) {
    auto nameModelInd1 = normalizeIndex(linkConnectionData.nameModelInd);

    srcNode->setInd(nameModelInd1);
  }
}

//---

bool
CQChartsSankeyPlot::
initConnectionObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initConnectionObjs");

  return CQChartsConnectionPlot::initConnectionObjs();
}

void
CQChartsSankeyPlot::
addConnectionObj(int id, const ConnectionsData &connectionsData) const
{
  QString srcStr = QString("%1").arg(id);

  auto *srcNode = findNode(srcStr);

  srcNode->setName(connectionsData.name);

  if (connectionsData.groupData.isValid())
    srcNode->setGroup(connectionsData.groupData.ig, connectionsData.groupData.ng);
  else
    srcNode->setGroup(-1);

  for (const auto &connection : connectionsData.connections) {
    QString destStr = QString("%1").arg(connection.node);

    auto *destNode = findNode(destStr);

    auto *edge = createEdge(OptReal(connection.value), srcNode, destNode);

    srcNode ->addDestEdge(edge);
    destNode->addSrcEdge (edge);

    destNode->setValue(OptReal(connection.value));
  }
}

//---

bool
CQChartsSankeyPlot::
initTableObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initTableObjs");

  //---

  TableConnectionDatas tableConnectionDatas;
  TableConnectionInfo  tableConnectionInfo;

  if (! processTableModel(tableConnectionDatas, tableConnectionInfo))
    return false;

  //---

  int nv = tableConnectionDatas.size();

  for (int row = 0; row < nv; ++row) {
    const auto &tableConnectionData = tableConnectionDatas[row];

    if (tableConnectionData.values().empty())
      continue;

    QString srcStr = QString("%1").arg(tableConnectionData.from());

    auto *srcNode = findNode(srcStr);

    srcNode->setName (tableConnectionData.name());
    srcNode->setGroup(tableConnectionData.group().ig, tableConnectionData.group().ng);

    for (const auto &value : tableConnectionData.values()) {
      QString destStr = QString("%1").arg(value.to);

      auto *destNode = findNode(destStr);

      auto *edge = createEdge(OptReal(value.value), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);
    }
  }

  //---

  return true;
}

//---

void
CQChartsSankeyPlot::
filterObjs()
{
  // hide nodes below depth
  if (maxDepth() > 0) {
    for (const auto &p : nameNodeMap_) {
      auto *node = p.second;

      if (node->depth() > maxDepth())
        node->setVisible(false);
    }
  }

  // hide nodes less than min value
  if (minValue() > 0) {
    for (const auto &p : nameNodeMap_) {
      auto *node = p.second;

      if (! node->value().isSet() || node->value().real() < minValue())
        node->setVisible(false);
    }
  }
}

//---

// main entry point in creating objects from model data
void
CQChartsSankeyPlot::
createObjsGraph(PlotObjs &objs) const
{
  // create graphs
  createGraphs();

  //---

  placeGraphs();

  //---

#if 0
  //auto *th = const_cast<CQChartsSankeyPlot *>(this);

  // th->calcMaxNodeDepth();
#endif

  //---

  //th->setNodeYRange();

  //---

  addObjects(objs);
}

void
CQChartsSankeyPlot::
addObjects(PlotObjs &objs) const
{
  if (! graph_ || graph_->nodes().empty() || ! graph_->rect().isSet())
    return;

  if (! graph_->rect().isValid()) {
    auto rect = graph_->rect();

    auto *th = const_cast<CQChartsSankeyPlot *>(this);

    if (rect.getWidth() <= 0) {
      double cx = rect.getXMid();

      rect.setXMin(cx - 1.0);
      rect.setXMax(cx + 1.0);
    }

    if (rect.getHeight() <= 0) {
      double cy = rect.getYMid();

      rect.setYMin(cy - 1.0);
      rect.setYMax(cy + 1.0);
    }

    th->graph_->setRect(rect);
  }

  //---

  // add node objects
  for (auto *node : graph_->nodes()) {
    auto *nodeObj = createObjFromNode(graph_, node);

    objs.push_back(nodeObj);
  }

  //---

  // add graph object
  auto *graphObj = createGraphObj(graph_->rect(), graph_);

  graph_->setObj(graphObj);

  objs.push_back(graphObj);

  //---

  // add edge objects
  for (const auto &edge : edges_) {
    if (! edge->srcNode()->isVisible() || ! edge->destNode()->isVisible())
      continue;

    auto *edgeObj = addEdgeObj(edge);

    objs.push_back(edgeObj);
  }
}

void
CQChartsSankeyPlot::
placeGraphs() const
{
  if (graph_)
    placeGraph(graph_);
}

// place nodes in graph
void
CQChartsSankeyPlot::
placeGraph(Graph *graph) const
{
  if (graph->isPlaced())
    return;

  //---

  // get placable nodes (nodes and sub graphs)
  Nodes nodes = graph->placeNodes();

  //---

  placeGraphNodes(graph, nodes);
}

void
CQChartsSankeyPlot::
placeGraphNodes(Graph *graph, const Nodes &nodes) const
{
  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  //---

  // set max depth of all graph nodes
  updateGraphMaxDepth(graph, nodes);

  //---

  // set x pos of nodes
  calcGraphNodesXPos(graph, nodes);

  //---

  // calc max height (fron node count) and max size (from value) for each x
  graph->setMaxHeight(0);
  graph->setTotalSize(0.0);

  for (const auto &depthNodes : graph->depthNodesMap()) {
    const auto &nodes = depthNodes.second.nodes;
    double      size  = depthNodes.second.size;

    graph->setMaxHeight(std::max(graph->maxHeight(), int(nodes.size())));
    graph->setTotalSize(std::max(graph->totalSize(), size));
  }

  //---

  // calc y value scale and margins to fit in bbox
  th->calcValueMarginScale(graph);

  // place node objects at each depth (xpos)
  placeDepthNodes(graph);

  //---

  // adjust nodes in graph
  adjustGraphNodes(graph, nodes);

  //---

  graph->updateRect();

  //---

  graph->setPlaced(true);
}

void
CQChartsSankeyPlot::
calcGraphNodesXPos(Graph *graph, const Nodes &nodes) const
{
  // place graph nodes at x position
  graph->clearDepthNodesMap();

  for (const auto &node : nodes) {
    int xpos = calcXPos(graph, node);

    graph->addDepthSize(xpos, node->edgeSum());
    graph->addDepthNode(xpos, node);
  }

  // check if all nodes at single x
  if (graph->depthNodesMap().size() == 1 && align() != Align::RAND) {
    auto *th = const_cast<CQChartsSankeyPlot *>(this);

    th->align_     = Align::RAND;
    th->alignRand_ = std::max(CMathRound::RoundNearest(sqrt(nodes.size())), 2);

    for (const auto &node : nodes)
      node->setDepth(-1);

    calcGraphNodesXPos(graph, nodes);
  }
}

void
CQChartsSankeyPlot::
createGraphs() const
{
  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  auto *graph = th->getGraph();

  // Add nodes to graph for group
  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    graph->addNode(node);
  }
}

CQChartsSankeyPlotGraph *
CQChartsSankeyPlot::
getGraph() const
{
  if (! graph_) {
    auto *th = const_cast<CQChartsSankeyPlot *>(this);

    th->graph_ = new Graph(this, "root");
  }

  return graph_;
}

void
CQChartsSankeyPlot::
clearGraphs()
{
  delete graph_;

  graph_ = nullptr;
}

void
CQChartsSankeyPlot::
calcValueMarginScale(Graph *graph)
{
  // get node margins
  double nodeYMargin = calcNodeYMargin();

  //---

  double ys = 2.0;

  if (bbox_.isSet()) {
  //ys = bbox.getHeight();
    ys = std::max(bbox_.getWidth(), bbox_.getHeight());
  }

  double ys1 = nodeYMargin*ys;
  double ys2 = ys - ys1;

  //---

  // calc value margin/scale
  graph->setValueMargin(graph->maxHeight() > 1.0 ? ys1/(graph->maxHeight() - 1) : 0.0);
  graph->setValueScale (graph->totalSize() > 0.0 ? ys2/ graph->totalSize()      : 1.0);
}

double
CQChartsSankeyPlot::
calcNodeXMargin() const
{
  double nodeXMargin      = std::min(std::max(this->nodeXMargin(), 0.0), 1.0);
  double pixelNodeXMargin = windowToPixelWidth(nodeXMargin);

  if (pixelNodeXMargin < minNodeMargin())
    nodeXMargin = pixelToWindowWidth(minNodeMargin());

  return nodeXMargin;
}

double
CQChartsSankeyPlot::
calcNodeYMargin() const
{
  double nodeYMargin      = std::min(std::max(this->nodeYMargin(), 0.0), 1.0);
  double pixelNodeYMargin = windowToPixelHeight(nodeYMargin);

  if (pixelNodeYMargin < minNodeMargin())
    nodeYMargin = pixelToWindowHeight(minNodeMargin());

  return nodeYMargin;
}

void
CQChartsSankeyPlot::
placeDepthNodes(Graph *graph) const
{
  // place node objects at each depth (xpos)
  for (const auto &depthNodes : graph->depthNodesMap()) {
    int         xpos  = depthNodes.first;
    const auto &nodes = depthNodes.second.nodes;

    placeDepthSubNodes(graph, xpos, nodes);
  }
}

void
CQChartsSankeyPlot::
placeDepthSubNodes(Graph *graph, int xpos, const Nodes &nodes) const
{
  // place nodes to fit in bbox
  double xs = bbox_.getWidth ();
  double ys = bbox_.getHeight();

  double dx = 1.0;

  if (graph->maxNodeDepth() > 0)
    dx = xs/graph->maxNodeDepth();

  double xm = pixelToWindowWidth(nodeWidth());

  //---

  // get sum of margins nodes at depth
  double height = graph->valueMargin()*(int(nodes.size()) - 1);

  // get sum of scaled values for nodes at depth
  for (const auto &node : nodes)
    height += graph->valueScale()*node->edgeSum();

  //---

  // place top to bottom
  double y1 = bbox_.getYMax() - (ys - height)/2.0;

  //---

  for (const auto &node : nodes) {
    // calc height
    double h = graph->valueScale()*node->edgeSum();

    if (h <= 0.0)
      h = 0.1;

    //---

    // calc rect
    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    int xpos1 = calcXPos(graph, node);
    assert(xpos == xpos1);

    double x11 = bbox_.getXMin() + xpos*dx; // left
    double x12 = x11 + xm;

    double yc = y1 - h/2.0; // placement center

    double y11 = yc - h/2.0;
    double y12 = yc + h/2.0;

    //---

    BBox rect;

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

    //---

    node->setRect(rect);

    //---

    y1 -= h + graph->valueMargin();
  }
}

CQChartsSankeyPlot::NodeObj *
CQChartsSankeyPlot::
createObjFromNode(Graph *, Node *node) const
{
//int numNodes = graph->nodes().size(); // node id needs to be per graph
  int numNodes = indNodeMap_.size();

  ColorInd ig;

  if (node->ngroup() > 0 && node->group() >= 0 && node->group() < node->ngroup())
    ig = ColorInd(node->group(), node->ngroup());

  ColorInd iv(node->id(), numNodes);

  auto *nodeObj = createNodeObj(node->rect(), node, ig, iv);

  nodeObj->setHierName(node->str ());
  nodeObj->setName    (node->name());

  if (node->hasValue())
    nodeObj->setValue(node->value().real());

  nodeObj->setDepth(node->depth());

  if (node->color().isValid())
    nodeObj->setColor(node->color());

  node->setObj(nodeObj);

  return nodeObj;
}

int
CQChartsSankeyPlot::
calcXPos(Graph *graph, Node *node) const
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
      xpos = graph->maxNodeDepth();
    else {
      if      (align() == Align::SRC)
        xpos = srcDepth;
      else if (align() == Align::DEST)
        xpos = graph->maxNodeDepth() - destDepth;
      else if (align() == Align::JUSTIFY) {
        double f = 1.0*srcDepth/(srcDepth + destDepth);

        xpos = int(f*graph->maxNodeDepth());
      }
      else if (align() == Align::RAND) {
        CQChartsRand::RealInRange rand(0, alignRand_);

        xpos = CMathRound::RoundNearest(rand.gen());

        const_cast<Node *>(node)->setDepth(xpos);
      }
    }
  }

  //--

  node->setXPos(xpos);

  return xpos;
}

CQChartsSankeyEdgeObj *
CQChartsSankeyPlot::
addEdgeObj(Edge *edge) const
{
  double xm = bbox_.getHeight()*edgeMargin_;
  double ym = bbox_.getWidth ()*edgeMargin_;

  BBox rect(bbox_.getXMin() - xm, bbox_.getYMin() - ym,
            bbox_.getXMax() + xm, bbox_.getYMax() + ym);

  auto *edgeObj = createEdgeObj(rect, edge);

  edge->setObj (edgeObj);
  edge->setLine(isEdgeLine());

  return edgeObj;
}

void
CQChartsSankeyPlot::
updateGraphMaxDepth(Graph *graph, const Nodes &nodes) const
{
  // calc max depth (source or dest) depending on align for xpos calc
  bool set = false;

  graph->setMinNodeDepth(0);
  graph->setMaxNodeDepth(0);

  auto updateNodeDepth = [&](int depth) {
    if (set) {
      graph->setMinNodeDepth(depth);
      graph->setMaxNodeDepth(depth);
    }
    else {
      graph->setMinNodeDepth(std::min(graph->minNodeDepth(), depth));
      graph->setMaxNodeDepth(std::max(graph->maxNodeDepth(), depth));
    }
  };

  for (const auto &node : nodes) {
    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    if      (align() == Align::SRC)
      updateNodeDepth(srcDepth);
    else if (align() == Align::DEST)
      updateNodeDepth(destDepth);
    else {
      updateNodeDepth(srcDepth);
      updateNodeDepth(destDepth);
    }
  }
}

bool
CQChartsSankeyPlot::
adjustNodes() const
{
  bool changed = false;

  if (graph_) {
    Nodes nodes = graph_->placeNodes();

    if (adjustGraphNodes(graph_, nodes))
      changed = true;
  }

  return changed;
}

bool
CQChartsSankeyPlot::
adjustGraphNodes(Graph *graph, const Nodes &nodes) const
{
//auto *th = const_cast<CQChartsSankeyPlot *>(this);

  //---

  // update range
//th->bbox_ = nodesBBox();

//th->dataRange_ = calcRange();

  //---

  initPosNodesMap(graph, nodes);

  //---

  if (isAdjustNodes()) {
    int numPasses = 25;

    for (int pass = 0; pass < numPasses; ++pass) {
      //std::cerr << "Pass " << pass << "\n";

      if (! adjustNodeCenters(graph)) {
        //std::cerr << "adjustNodeCenters (#" << pass + 1 << " Passes)\n";
        break;
      }
    }

    //---

    reorderNodeEdges(graph, nodes);

    adjustEdgeOverlaps(graph);
  }

  //---

//placeDepthNodes(graph);

  //---

//th->nodeYSet_ = true;

  //---

  return true;
}

void
CQChartsSankeyPlot::
initPosNodesMap(Graph *graph, const Nodes &nodes) const
{
  // get nodes by x pos
  graph->resetPosNodes();

  for (const auto &node : nodes)
    graph->addPosNode(node);
}

bool
CQChartsSankeyPlot::
adjustNodeCenters(Graph *graph) const
{
  // adjust nodes so centered on src nodes
  bool changed = false;

  // second to last
  int posNodesDepth = graph->posNodesMap().size();

  for (int xpos = 1; xpos <= posNodesDepth; ++xpos) {
    if (! graph->hasPosNodes(xpos)) continue;

    const auto &nodes = graph->posNodes(xpos);

    for (const auto &node : nodes) {
      if (adjustNode(node))
        changed = true;
    }
  }

  removeOverlaps(graph);

  //---

  // second to last to first
  for (int xpos = posNodesDepth - 1; xpos >= 0; --xpos) {
    if (! graph->hasPosNodes(xpos)) continue;

    const auto &nodes = graph->posNodes(xpos);

    for (const auto &node : nodes) {
      if (adjustNode(node))
        changed = true;
    }
  }

  removeOverlaps(graph);

  return changed;
}

bool
CQChartsSankeyPlot::
adjustEdgeOverlaps(Graph *graph) const
{
  auto hasNode = [&](Node *node, const Nodes &nodes) {
    for (auto &node1 : nodes)
      if (node == node1)
        return true;

    return false;
  };

  int posNodesDepth = graph->posNodesMap().size();

  // find first x pos with nodes
  int xpos1 = 0;

  while (xpos1 <= posNodesDepth && ! graph->hasPosNodes(xpos1))
    ++xpos1;

  if (xpos1 > posNodesDepth)
    return false;

  int xpos2 = xpos1 + 1;

  while (xpos2 <= posNodesDepth) {
    // find next x pos with nodes
    while (xpos2 <= posNodesDepth && ! graph->hasPosNodes(xpos2))
      ++xpos2;

    if (xpos2 > posNodesDepth)
      break;

    // get nodes at each pos
    const auto &nodes1 = graph->posNodes(xpos1);
    const auto &nodes2 = graph->posNodes(xpos2);

    // get edges between nodes
    Edges edges;

    for (const auto &node1 : nodes1) {
      for (const auto &edge1 : node1->destEdges()) {
        auto *destNode1 = edge1->destNode();

        if (hasNode(destNode1, nodes2))
          edges.push_back(edge1);
      }
    }

#if 0
    // check edges for overlaps
    int numEdges = edges.size();

    for (int i1 = 0; i1 < numEdges - 1; ++i1) {
      auto *edge1 = edges[i1];

      for (int i2 = i1 + 1; i2 < numEdges; ++i2) {
        auto *edge2 = edges[i2];

        if (edge1->overlaps(edge2)) {
          auto *srcObj1  = edge1->srcNode ();
          auto *destObj1 = edge1->destNode();
          auto *srcObj2  = edge2->srcNode ();
          auto *destObj2 = edge2->destNode();

          std::cerr << "Overlap Edges : " <<
            "[" << srcObj1 ->name().toStdString() << ", " <<
                   destObj1->name().toStdString() << "] " <<
            "[" << srcObj2 ->name().toStdString() << ", " <<
                   destObj2->name().toStdString() << "]\n";
        }
      }
    }
#endif

    xpos1 = xpos2++;
  }

  return true;
}

bool
CQChartsSankeyPlot::
removeOverlaps(Graph *graph) const
{
  bool changed = false;

  for (const auto &posNodes : graph->posNodesMap()) {
    if (removePosOverlaps(graph, posNodes.second))
      changed = true;
  }

  return changed;
}

bool
CQChartsSankeyPlot::
removePosOverlaps(Graph *graph, const Nodes &nodes) const
{
  double ym = pixelToWindowHeight(minNodeMargin());

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

  // move nodes back inside bbox (needed ?)
  if (node1) {
    const auto &rect1 = node1->rect();

    if (rect1.getYMin() < bbox_.getYMin())
      spreadPosNodes(graph, nodes);
  }

  return changed;
}

void
CQChartsSankeyPlot::
spreadPosNodes(Graph *, const Nodes &nodes) const
{
  PosNodeMap posNodeMap;

  createPosNodeMap(nodes, posNodeMap);

  BBox bbox;

  Node *node1 = nullptr, *node2 = nullptr;

  for (const auto &posNode : posNodeMap) {
    node2 = posNode.second;

    if (! node1) node1 = node2;

    bbox += node2->rect();
  }

  double dy1 = node1->rect().getHeight()/2.0; // top
  double dy2 = node2->rect().getHeight()/2.0; // bottom

  if (! bbox.isValid() || (bbox.getHeight() - dy1 - dy2) <= 0.0)
    return;

  double ymin = bbox_.getYMin() + dy2;
  double ymax = bbox_.getYMax() - dy1;

  double dy = ymin - node2->rect().getYMid();
  double ys = (ymax - ymin)/(bbox.getHeight() - dy1 - dy2);

  for (const auto &posNode : posNodeMap) {
    auto *node = posNode.second;

    node->moveBy(Point(0, dy));

    double y1 = ys*(node->rect().getYMid() - ymin) + ymin;

    node->moveBy(Point(0, y1 - node->rect().getYMid()));
  }
}

bool
CQChartsSankeyPlot::
reorderNodeEdges(Graph *, const Nodes &nodes) const
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

void
CQChartsSankeyPlot::
createPosNodeMap(const Nodes &nodes, PosNodeMap &posNodeMap) const
{
  for (const auto &node : nodes) {
    const auto &rect = node->rect();
    if (! rect.isValid()) continue;

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
CQChartsSankeyPlot::
createPosEdgeMap(const Edges &edges, PosEdgeMap &posEdgeMap, bool isSrc) const
{
  for (const auto &edge : edges) {
    auto *node = (isSrc ? edge->srcNode() : edge->destNode());
    if (! node->isVisible()) continue;

    const auto &rect = node->rect();

    double y = 0.0;

    if (rect.isValid())
      y = bbox_.getYMax() - rect.getYMid();

    auto p = posEdgeMap.find(y);

    while (p != posEdgeMap.end()) {
      y -= 0.001;

      p = posEdgeMap.find(y);
    }

    posEdgeMap[y] = edge;
  }
}

bool
CQChartsSankeyPlot::
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

  if       (srcBBox.isValid() && destBBox.isValid())
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

  if (std::abs(dy) < 1E-6) // better tolerance ?
    return false;

  node->moveBy(Point(0, dy));

  return true;
}

//---

CQChartsSankeyPlotNode *
CQChartsSankeyPlot::
findNode(const QString &name) const
{
  auto p = nameNodeMap_.find(name);

  if (p != nameNodeMap_.end())
    return (*p).second;

  auto *node = createNode(name);

  return node;
}

CQChartsSankeyPlotNode *
CQChartsSankeyPlot::
createNode(const QString &name) const
{
  auto *node = new Node(this, name);

  node->setId(nameNodeMap_.size());

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(name, node));

  assert(node == (*p1).second);

  th->indNodeMap_[node->id()] = node;

  node->setName(name);

  return node;
}

CQChartsSankeyPlotEdge *
CQChartsSankeyPlot::
createEdge(const OptReal &value, Node *srcNode, Node *destNode) const
{
  assert(srcNode && destNode);

  auto *edge = new Edge(this, value, srcNode, destNode);

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  th->edges_.push_back(edge);

  edge->setId(th->edges_.size());

  return edge;
}

//---

void
CQChartsSankeyPlot::
keyPress(int key, int modifier)
{
  if      (key == Qt::Key_A) {
    if (adjustNodes())
      drawObjs();
  }
  else if (key == Qt::Key_F) {
    bbox_ = nodesBBox(); // current

    fitToBBox(targetBBox_);

    drawObjs();
  }
  else if (key == Qt::Key_S) {
    printStats();
  }
  else if (key == Qt::Key_V) {
    if (graph_)
      adjustEdgeOverlaps(graph_);
  }
  else
    CQChartsPlot::keyPress(key, modifier);
}

void
CQChartsSankeyPlot::
printStats()
{
  using NameData = std::map<QString, QString>;
  using NodeData = std::map<Node *, NameData>;

  NodeData nodeData;

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    if (node->srcEdges().empty())
      nodeData[node]["No Src" ] = "";

    if (node->destEdges().empty())
      nodeData[node]["No Dest"] = "";
  }

  for (auto &pn : nodeData) {
    Node       *node     = pn.first;
    const auto &nameData = pn.second;

    for (auto &pd : nameData) {
      std::cerr << node->name().toStdString() << " " << pd.first.toStdString() << "\n";
    }
  }
}

//---

CQChartsSankeyNodeObj *
CQChartsSankeyPlot::
createNodeObj(const BBox &rect, Node *node, const ColorInd &ig, const ColorInd &iv) const
{
  return new NodeObj(this, rect, node, ig, iv);
}

CQChartsSankeyEdgeObj *
CQChartsSankeyPlot::
createEdgeObj(const BBox &rect, Edge *edge) const
{
  return new EdgeObj(this, rect, edge);
}

CQChartsSankeyGraphObj *
CQChartsSankeyPlot::
createGraphObj(const BBox &rect, Graph *graph) const
{
  return new GraphObj(this, rect, graph);
}

//------

CQChartsSankeyPlotNode::
CQChartsSankeyPlotNode(const Plot *plot, const QString &str) :
 plot_(plot), str_(str)
{
}

CQChartsSankeyPlotNode::
~CQChartsSankeyPlotNode()
{
}

void
CQChartsSankeyPlotNode::
addSrcEdge(Edge *edge, bool primary)
{
  edge->destNode()->parent_ = edge->srcNode();

  srcEdges_.push_back(edge);

  srcDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

void
CQChartsSankeyPlotNode::
addDestEdge(Edge *edge, bool primary)
{
  edge->destNode()->parent_ = edge->srcNode();

  destEdges_.push_back(edge);

  destDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

bool
CQChartsSankeyPlotNode::
hasDestNode(Node *destNode) const
{
  for (auto &destEdge : destEdges()) {
    if (destEdge->destNode() == destNode)
      return true;
  }

  return false;
}

int
CQChartsSankeyPlotNode::
srcDepth() const
{
  if (depth() >= 0)
    return depth() - 1;

  NodeSet visited;

  visited.insert(this);

  return calcSrcDepth(visited);
}

int
CQChartsSankeyPlotNode::
calcSrcDepth(NodeSet &visited) const
{
  if (srcDepth_ >= 0)
    return srcDepth_;

  auto *th = const_cast<CQChartsSankeyPlotNode *>(this);

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
CQChartsSankeyPlotNode::
destDepth() const
{
  if (depth() >= 0)
    return depth() + 1;

  NodeSet visited;

  visited.insert(this);

  return calcDestDepth(visited);
}

int
CQChartsSankeyPlotNode::
calcDestDepth(NodeSet &visited) const
{
  if (destDepth_ >= 0)
    return destDepth_;

  auto *th = const_cast<CQChartsSankeyPlotNode *>(this);

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

const CQChartsSankeyPlotNode::BBox &
CQChartsSankeyPlotNode::
rect() const
{
  if (obj_) {
    assert(obj_->rect() == rect_);
  }

  return rect_;
}

void
CQChartsSankeyPlotNode::
setRect(const BBox &r)
{
  rect_ = r;

  if (obj_) // TODO: assert null or use move by
    obj_->setRect(r);
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

  for (const auto &edge : srcEdges_) {
    if (edge->hasValue())
      value += edge->value().real();
  }

  return value;
}

double
CQChartsSankeyPlotNode::
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
CQChartsSankeyPlotNode::
setObj(Obj *obj)
{
  obj_ = obj;
}

//---

void
CQChartsSankeyPlotNode::
moveBy(const Point &delta)
{
  rect_.moveBy(delta);

  if (obj_)
    obj_->moveBy(delta);

  //---

  moveSrcEdgeRectsBy (delta);
  moveDestEdgeRectsBy(delta);
}

void
CQChartsSankeyPlotNode::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);

  if (obj_)
    obj_->scale(fx, fy);

  //---

  placeEdges();
}

//---

void
CQChartsSankeyPlotNode::
placeEdges(bool reset)
{
  if (! reset) {
    if (! srcEdgeRects().empty() || ! destEdgeRects().empty())
      return;
  }

  //---

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

    double y3 = y2;

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

    double y3 = y2;

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

//---

void
CQChartsSankeyPlotNode::
allSrcNodesAndEdges(NodeSet &nodeSet, EdgeSet &edgeSet) const
{
  for (const auto &edge : this->srcEdges()) {
    edgeSet.insert(edge);
  }

  for (const auto &edge : this->srcEdges()) {
    auto *node = edge->srcNode();

    if (nodeSet.find(node) == nodeSet.end())
      node->allSrcNodesAndEdges(nodeSet, edgeSet);
  }
}

void
CQChartsSankeyPlotNode::
allDestNodesAndEdges(NodeSet &nodeSet, EdgeSet &edgeSet) const
{
  for (const auto &edge : this->destEdges()) {
    edgeSet.insert(edge);
  }

  for (const auto &edge : this->destEdges()) {
    auto *node = edge->destNode();

    if (nodeSet.find(node) == nodeSet.end())
      node->allDestNodesAndEdges(nodeSet, edgeSet);
  }
}

//---

QColor
CQChartsSankeyPlotNode::
calcColor() const
{
  if (color_.isValid())
    return color_;

  CQChartsUtil::ColorInd ic;

  if (ngroup() > 0 && group() >= 0 && group() < ngroup())
    ic = CQChartsUtil::ColorInd(group(), ngroup());
  else
    ic = CQChartsUtil::ColorInd(id(), plot_->numNodes());

  return plot_->interpNodeFillColor(ic);
}

//------

CQChartsSankeyPlotEdge::
CQChartsSankeyPlotEdge(const Plot *plot, const OptReal &value, Node *srcNode, Node *destNode) :
 plot_(plot), value_(value), srcNode_(srcNode), destNode_(destNode)
{
}

CQChartsSankeyPlotEdge::
~CQChartsSankeyPlotEdge()
{
}

//---

void
CQChartsSankeyPlotEdge::
setObj(Obj *obj)
{
  obj_ = obj;
}

//---

bool
CQChartsSankeyPlotEdge::
overlaps(const Edge *edge) const
{
  QPainterPath path1, path2;

  if (this->edgePath(path1, /*isLine*/true) && edge->edgePath(path2, /*isLine*/true))
    return path1.intersects(path2);

  //---

  auto p11 = this->srcNode ()->rect().getCenter();
  auto p12 = this->destNode()->rect().getCenter();

  auto p21 = edge->srcNode ()->rect().getCenter();
  auto p22 = edge->destNode()->rect().getCenter();

  Point pi;

  if (! CQChartsUtil::intersectLines(p11, p12, p21, p22, pi))
    return false;

  return true;
}

bool
CQChartsSankeyPlotEdge::
edgePath(QPainterPath &path, bool isLine) const
{
  auto *th = const_cast<CQChartsSankeyPlotEdge *>(this);

  path = QPainterPath();

  // get connection rect of source and destination object
  auto *srcObj  = this->srcNode ()->obj();
  auto *destObj = this->destNode()->obj();

  BBox srcRect, destRect;

  if (srcObj && destObj) {
    srcRect  = srcObj ->node()->destEdgeRect(th);
    destRect = destObj->node()->srcEdgeRect (th);
  }
  else {
    th->srcNode ()->placeEdges(/*reset*/false);
    th->destNode()->placeEdges(/*reset*/false);

    srcRect  = this->srcNode ()->destEdgeRect(th);
    destRect = this->destNode()->srcEdgeRect (th);
  }

  if (! srcRect.isSet() || ! destRect.isSet())
    return false;

  //---

  // x from right of source rect to left of dest rect
  bool swapped = false;

  double x1 = srcRect.getXMax(), x2 = destRect.getXMin();

  if (x1 > x2) {
    x1 = destRect.getXMax(), x2 = srcRect.getXMin();
    swapped = true;
  }

  // start y range from source node, and end y range fron dest node
  double y11 = srcRect .getYMax(), y12 = srcRect .getYMin();
  double y21 = destRect.getYMax(), y22 = destRect.getYMin();

  if (swapped) {
    std::swap(y11, y21);
    std::swap(y12, y22);
  }

  // curve control point x at 1/3 and 2/3
  double x3 = CMathUtil::lerp(1.0/3.0, x1, x2);
  double x4 = CMathUtil::lerp(2.0/3.0, x1, x2);

  if (isLine) {
    double y1m = CMathUtil::avg(y11, y12);
    double y2m = CMathUtil::avg(y21, y22);

    path.moveTo (QPointF(x1, y1m));
    path.cubicTo(QPointF(x3, y1m), QPointF(x4, y2m), QPointF(x2, y2m));
  }
  else {
    path.moveTo (QPointF(x1, y11));
    path.cubicTo(QPointF(x3, y11), QPointF(x4, y21), QPointF(x2, y21));
    path.lineTo (QPointF(x2, y22));
    path.cubicTo(QPointF(x4, y22), QPointF(x3, y12), QPointF(x1, y12));

    path.closeSubpath();
  }

  return true;
}

//------

CQChartsSankeyNodeObj::
CQChartsSankeyNodeObj(const Plot *plot, const BBox &rect, Node *node,
                      const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, ColorInd(), ig, iv),
 plot_(plot), node_(node)
{
  setDetailHint(DetailHint::MAJOR);

  setEditable(true);

  //---

  placeEdges();
}

CQChartsSankeyNodeObj::
~CQChartsSankeyNodeObj()
{
  if (node_)
    node_->setObj(nullptr);
}

void
CQChartsSankeyNodeObj::
placeEdges(bool reset)
{
  node_->placeEdges(reset);
}

QString
CQChartsSankeyNodeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(iv_.i);
}

QString
CQChartsSankeyNodeObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  QString name = this->name();

  if (name == "")
    name = this->id();

  tableTip.addTableRow("Hier Name", hierName());
  tableTip.addTableRow("Name"     , name      );
  tableTip.addTableRow("Value"    , value   ());

  if (depth() >= 0)
    tableTip.addTableRow("Depth", depth());

  int ns = node()->srcEdges ().size();
  int nd = node()->destEdges().size();

  tableTip.addTableRow("Edges", QString("%1|%2").arg(ns).arg(nd));

  if (plot_->groupColumn().isValid()) {
    tableTip.addTableRow("Group", node()->group());
  }

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsSankeyNodeObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "hierName")->setDesc("Hierarchical Name");
  model->addProperty(path1, this, "name"    )->setDesc("Name");
  model->addProperty(path1, this, "value"   )->setDesc("Value");
  model->addProperty(path1, this, "depth"   )->setDesc("Depth");
  model->addProperty(path1, this, "color"   )->setDesc("Color");
}

//---

void
CQChartsSankeyNodeObj::
moveBy(const Point &delta)
{
  //std::cerr << "  Move " << node()->str().toStdString() << " by " << delta.y << "\n";

  rect_.moveBy(delta);
}

void
CQChartsSankeyNodeObj::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);
}

//---

bool
CQChartsSankeyNodeObj::
editPress(const Point &p)
{
  editChanged_ = false;

  editHandles()->setDragPos(p);

  return true;
}

bool
CQChartsSankeyNodeObj::
editMove(const Point &p)
{
  const auto &dragPos  = editHandles()->dragPos();
  const auto &dragSide = editHandles()->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles()->updateBBox(dx, dy);

  setEditBBox(editHandles()->bbox(), dragSide);

  editHandles()->setDragPos(p);

  auto *graph = plot_->getGraph();

  if (graph)
    graph->updateRect();

  editChanged_ = true;

  plot()->drawObjs();

  return true;
}

bool
CQChartsSankeyNodeObj::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

bool
CQChartsSankeyNodeObj::
editRelease(const Point &)
{
  if (editChanged_)
    plot()->invalidateObjTree();

  return true;
}

void
CQChartsSankeyNodeObj::
setEditBBox(const BBox &bbox, const CQChartsResizeSide &)
{
  double dx = bbox.getXMin() - rect_.getXMin();
  double dy = bbox.getYMin() - rect_.getYMin();

  node()->moveBy(Point(dx, dy));
}

//---

CQChartsSankeyNodeObj::PlotObjs
CQChartsSankeyNodeObj::
getConnected() const
{
  PlotObjs plotObjs;

  for (auto &edgeRect : node()->srcEdgeRects())
    plotObjs.push_back(edgeRect.first->obj());

  for (auto &edgeRect : node()->destEdgeRects())
    plotObjs.push_back(edgeRect.first->obj());

  return plotObjs;
}

//---

void
CQChartsSankeyNodeObj::
draw(CQChartsPaintDevice *device)
{
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw node
  if (rect().isSet())
    device->drawRect(rect());

  //---

  device->resetColorNames();

  //---

  // show source and destination nodes on inside
  if (plot_->view()->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    if (plot_->mouseColoring() != CQChartsSankeyPlot::ConnectionType::NONE) {
      plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER_EXTRA);

      drawConnectionMouseOver(device, (int) plot_->mouseColoring());

      plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
    }
  }
}

void
CQChartsSankeyNodeObj::
drawConnectionMouseOver(CQChartsPaintDevice *device, int imouseColoring) const
{
  auto mouseColoring = (CQChartsSankeyPlot::ConnectionType) imouseColoring;

  if      (mouseColoring == CQChartsSankeyPlot::ConnectionType::SRC ||
           mouseColoring == CQChartsSankeyPlot::ConnectionType::SRC_DEST) {
    for (const auto &edge : node()->srcEdges()) {
      auto *edgeObj = edge->obj();
      edgeObj->setInside(true); edgeObj->draw(device); edgeObj->setInside(false);
    }
  }
  else if (mouseColoring == CQChartsSankeyPlot::ConnectionType::ALL_SRC ||
           mouseColoring == CQChartsSankeyPlot::ConnectionType::ALL_SRC_DEST) {
    CQChartsSankeyPlotNode::NodeSet nodeSet;
    CQChartsSankeyPlotNode::EdgeSet edgeSet;

    node()->allSrcNodesAndEdges(nodeSet, edgeSet);

    for (const auto &edge : edgeSet) {
      auto *edgeObj = edge->obj();
      edgeObj->setInside(true); edgeObj->draw(device); edgeObj->setInside(false);
    }

    for (const auto &node : nodeSet) {
      auto *nodeObj = node->obj();
      nodeObj->setInside(true); nodeObj->draw(device); nodeObj->setInside(false);
    }
  }

  if      (mouseColoring == CQChartsSankeyPlot::ConnectionType::DEST ||
           mouseColoring == CQChartsSankeyPlot::ConnectionType::SRC_DEST) {
    for (const auto &edge : node()->destEdges()) {
      auto *edgeObj = edge->obj();
      edgeObj->setInside(true); edgeObj->draw(device); edgeObj->setInside(false);
    }
  }
  else if (mouseColoring == CQChartsSankeyPlot::ConnectionType::ALL_DEST ||
           mouseColoring == CQChartsSankeyPlot::ConnectionType::ALL_SRC_DEST) {
    CQChartsSankeyPlotNode::NodeSet nodeSet;
    CQChartsSankeyPlotNode::EdgeSet edgeSet;

    node()->allDestNodesAndEdges(nodeSet, edgeSet);

    for (const auto &edge : edgeSet) {
      auto *edgeObj = edge->obj();
      edgeObj->setInside(true); edgeObj->draw(device); edgeObj->setInside(false);
    }

    for (const auto &node : nodeSet) {
      auto *nodeObj = node->obj();
      nodeObj->setInside(true); nodeObj->draw(device); nodeObj->setInside(false);
    }
  }

  plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
}

void
CQChartsSankeyNodeObj::
drawFg(CQChartsPaintDevice *device) const
{
  if (! plot_->isTextVisible())
    return;

  auto prect = plot_->windowToPixel(rect());

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  QFontMetricsF fm(device->font());

  //---

  // set text pen
  ColorInd ic = calcColorInd();

  CQChartsPenBrush penBrush;

  QColor c = plot_->interpTextColor(ic);

  plot_->setPen(penBrush, PenData(true, c, plot_->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  QString str = node()->label();

  if (! str.length())
    str = node()->name();

  //---

  double ptw = fm.width(str);

  double clipLength = plot_->lengthPixelWidth(plot_->textClipLength());

  if (clipLength > 0.0)
    ptw = std::min(ptw, clipLength);

  double tw = plot_->pixelToWindowWidth(ptw);

  //---

  double xm = plot_->getCalcDataRange().xmid();

  double tx;

  if (rect().getXMid() < xm - tw)
    tx = prect.getXMax() + textMargin;
  else
    tx = prect.getXMin() - textMargin - ptw;

  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot_->pixelToWindow(Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = CQChartsAngle(0);
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();
  options.align         = Qt::AlignLeft;
  options.clipLength    = plot_->textClipLength();

  CQChartsDrawUtil::drawTextAtPoint(device, pt, str, options);
}

void
CQChartsSankeyNodeObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  ColorInd ic = calcColorInd();

  QColor bc = plot_->interpNodeStrokeColor(ic);
  QColor fc = calcFillColor();

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isNodeStroked(), bc, plot_->nodeStrokeAlpha(),
              plot_->nodeStrokeWidth(), plot_->nodeStrokeDash()),
    BrushData(plot_->isNodeFilled(), fc, plot_->nodeFillAlpha(),
              plot_->nodeFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsSankeyNodeObj::
calcFillColor() const
{
  QColor fc;

  if (plot_->isSrcColoring()) {
    using Colors = std::vector<QColor>;

    Colors colors;

    for (const auto &edge : node()->srcEdges()) {
      auto *srcNode = edge->srcNode();

      auto c = srcNode->calcColor();

      colors.push_back(c);
    }

    if (! colors.empty())
      fc = CQChartsUtil::blendColors(colors);
    else
      fc = node()->calcColor();
  }
  else {
    ColorInd ic = calcColorInd();

    fc = plot_->interpNodeFillColor(ic);

    if (color_.isValid())
      fc = color_;
  }

  return fc;
}

void
CQChartsSankeyNodeObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

//------

CQChartsSankeyEdgeObj::
CQChartsSankeyEdgeObj(const Plot *plot, const BBox &rect, Edge *edge) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), edge_(edge)
{
  //setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsSankeyEdgeObj::
calcId() const
{
  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  return QString("%1:%2:%3").arg(typeName()).arg(srcObj->calcId()).arg(destObj->calcId());
}

QString
CQChartsSankeyEdgeObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  QString srcName  = srcObj ->hierName();
  QString destName = destObj->hierName();

  if (srcName  == "") srcName  = srcObj ->id();
  if (destName == "") destName = destObj->id();

  tableTip.addTableRow("Src"  , srcName);
  tableTip.addTableRow("Dest" , destName);

  if (edge()->hasValue())
    tableTip.addTableRow("Value", edge()->value().real());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsSankeyEdgeObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);
}

//---

bool
CQChartsSankeyEdgeObj::
inside(const Point &p) const
{
  return path_.contains(p.qpoint());
}

CQChartsSankeyEdgeObj::PlotObjs
CQChartsSankeyEdgeObj::
getConnected() const
{
  PlotObjs plotObjs;

  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  plotObjs.push_back(srcObj);
  plotObjs.push_back(destObj);

  return plotObjs;
}

//---

void
CQChartsSankeyEdgeObj::
draw(CQChartsPaintDevice *device)
{
  // get edge path
  if (! edgePath(path_, edge()->isLine()))
    return;

  //---

  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw path
  device->setColorNames();

  if (! edge()->isLine())
    device->drawPath(path_);
  else
    device->strokePath(path_, penBrush.pen);

  device->resetColorNames();

  //---

  //---

  // show source and destination nodes on inside
  if (plot_->view()->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    if (plot_->mouseColoring() != CQChartsSankeyPlot::ConnectionType::NONE) {
      auto *srcNode  = edge()->srcNode ();
      auto *destNode = edge()->destNode();

      auto *srcNodeObj  = srcNode ->obj();
      auto *destNodeObj = destNode->obj();

      plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER_EXTRA);

      if      (plot_->mouseColoring() == CQChartsSankeyPlot::ConnectionType::SRC) {
        srcNodeObj ->setInside(true); srcNodeObj ->draw(device); srcNodeObj ->setInside(false);
      }
      else if (plot_->mouseColoring() == CQChartsSankeyPlot::ConnectionType::DEST) {
        destNodeObj->setInside(true); destNodeObj->draw(device); destNodeObj->setInside(false);
      }
      else if (plot_->mouseColoring() == CQChartsSankeyPlot::ConnectionType::SRC_DEST) {
        srcNodeObj ->setInside(true); srcNodeObj ->draw(device); srcNodeObj ->setInside(false);
        destNodeObj->setInside(true); destNodeObj->draw(device); destNodeObj->setInside(false);
      }
      else if (plot_->mouseColoring() == CQChartsSankeyPlot::ConnectionType::ALL_SRC) {
        srcNodeObj ->setInside(true); srcNodeObj ->draw(device); srcNodeObj ->setInside(false);
        srcNodeObj ->drawConnectionMouseOver(device,
          (int) CQChartsSankeyPlot::ConnectionType::ALL_SRC);
      }
      else if (plot_->mouseColoring() == CQChartsSankeyPlot::ConnectionType::ALL_DEST) {
        destNodeObj->setInside(true); destNodeObj->draw(device); destNodeObj->setInside(false);
        destNodeObj->drawConnectionMouseOver(device,
          (int) CQChartsSankeyPlot::ConnectionType::ALL_DEST);
      }
      else if (plot_->mouseColoring() == CQChartsSankeyPlot::ConnectionType::ALL_SRC_DEST) {
        srcNodeObj ->setInside(true); srcNodeObj ->draw(device); srcNodeObj ->setInside(false);
        srcNodeObj ->drawConnectionMouseOver(device,
          (int) CQChartsSankeyPlot::ConnectionType::ALL_SRC);

        destNodeObj->setInside(true); destNodeObj->draw(device); destNodeObj->setInside(false);
        destNodeObj->drawConnectionMouseOver(device,
          (int) CQChartsSankeyPlot::ConnectionType::ALL_DEST);
      }

      plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
    }
  }
}

bool
CQChartsSankeyEdgeObj::
edgePath(QPainterPath &path, bool isLine) const
{
  return edge()->edgePath(path, isLine);
}

void
CQChartsSankeyEdgeObj::
drawFg(CQChartsPaintDevice *device) const
{
  if (! plot_->isTextVisible())
    return;

  //---

  // get connection rect of source and destination object
  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  bool isSelf = (srcObj == destObj);

  auto srcRect  = srcObj ->node()->destEdgeRect(edge());
  auto destRect = destObj->node()->srcEdgeRect (edge());

  if (! srcRect.isSet() || ! destRect.isSet())
    return;

  auto rect = (isSelf ? srcRect : srcRect + destRect);

  auto prect = plot_->windowToPixel(rect);

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  QFontMetricsF fm(device->font());

  //---

  // set text pen
  ColorInd ic = calcColorInd();

  CQChartsPenBrush penBrush;

  QColor c = plot_->interpTextColor(ic);

  plot_->setPen(penBrush, PenData(true, c, plot_->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  QString str = edge()->label();

  if (! str.length())
    return;

  double ptw = fm.width(str);

  double tx = prect.getXMid() - textMargin - ptw/2.0;
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot_->pixelToWindow(Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = CQChartsAngle(0);
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();
  options.align         = Qt::AlignLeft;
  options.clipLength    = plot_->textClipLength();

  CQChartsDrawUtil::drawTextAtPoint(device, pt, str, options);
}

void
CQChartsSankeyEdgeObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto *srcNode  = edge()->srcNode ();
  auto *destNode = edge()->destNode();

  QColor fc1 = srcNode ->obj()->calcFillColor();
  QColor fc2 = destNode->obj()->calcFillColor();

  QColor fc = CQChartsUtil::blendColors(fc1, fc2, 0.5);

  int numNodes = plot_->numNodes();

  ColorInd ic1(srcNode ->id(), numNodes);
  ColorInd ic2(destNode->id(), numNodes);

  QColor sc1 = plot_->interpEdgeStrokeColor(ic1);
  QColor sc2 = plot_->interpEdgeStrokeColor(ic2);

  QColor sc = CQChartsUtil::blendColors(sc1, sc2, 0.5);

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isEdgeStroked(), sc, plot_->edgeStrokeAlpha(),
              plot_->edgeStrokeWidth(), plot_->edgeStrokeDash()),
    BrushData(plot_->isEdgeFilled(), fc, plot_->edgeFillAlpha(),
              plot_->edgeFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsSankeyEdgeObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  if (edge()->hasValue()) {
    std::ostream &os = device->os();

    os << "\n";
    os << "  this.value = " << edge()->value().real() << ";\n";
  }
}

//------

CQChartsSankeyGraphObj::
CQChartsSankeyGraphObj(const Plot *plot, const BBox &rect, Graph *graph) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), graph_(graph)
{
  setDetailHint(DetailHint::MAJOR);

  setEditable  (false);
  setSelectable(false);
}

CQChartsSankeyGraphObj::
~CQChartsSankeyGraphObj()
{
}

QString
CQChartsSankeyGraphObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(graph_->id());
}

QString
CQChartsSankeyGraphObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Id", graph_->id());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsSankeyGraphObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);
}

//---

void
CQChartsSankeyGraphObj::
moveBy(const Point &delta)
{
  //std::cerr << "  Move " << node()->str().toStdString() << " by " << delta.y << "\n";

  rect_.moveBy(delta);
}

void
CQChartsSankeyGraphObj::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);
}

//---

bool
CQChartsSankeyGraphObj::
editPress(const Point &p)
{
  editChanged_ = false;

  editHandles()->setDragPos(p);

  return true;
}

bool
CQChartsSankeyGraphObj::
editMove(const Point &p)
{
  const auto &dragPos  = editHandles()->dragPos();
  const auto &dragSide = editHandles()->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles()->updateBBox(dx, dy);

  setEditBBox(editHandles()->bbox(), dragSide);

  editHandles()->setDragPos(p);

  if (graph())
    graph()->updateRect();

  editChanged_ = true;

  plot()->drawObjs();

  return true;
}

bool
CQChartsSankeyGraphObj::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

bool
CQChartsSankeyGraphObj::
editRelease(const Point &)
{
  if (editChanged_)
    plot()->invalidateObjTree();

  return true;
}

void
CQChartsSankeyGraphObj::
setEditBBox(const BBox &bbox, const CQChartsResizeSide &)
{
  graph_->setRect(bbox);
}

//---

void
CQChartsSankeyGraphObj::
draw(CQChartsPaintDevice *device)
{
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw rect
  if (rect().isSet())
    device->drawRect(rect());

  //---

  device->resetColorNames();
}

void
CQChartsSankeyGraphObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  ColorInd ic = calcColorInd();

  QColor bc = plot_->interpGraphStrokeColor(ic);
  QColor fc = plot_->interpGraphFillColor  (ic);

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isGraphStroked(), bc, plot_->graphStrokeAlpha(),
              plot_->graphStrokeWidth(), plot_->graphStrokeDash()),
    BrushData(plot_->isGraphFilled(), fc, plot_->graphFillAlpha(),
              plot_->graphFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsSankeyGraphObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

//------

CQChartsSankeyPlotGraph::
CQChartsSankeyPlotGraph(const Plot *plot, const QString &str) :
 CQChartsSankeyPlotNode(plot, str)
{
}

void
CQChartsSankeyPlotGraph::
addNode(Node *node)
{
  srcDepth_  = -1;
  destDepth_ = -1;

  nodes_.push_back(node);
}

//---

void
CQChartsSankeyPlotGraph::
setObj(Obj *obj)
{
  obj_ = obj;
}

//---

void
CQChartsSankeyPlotGraph::
setRect(const BBox &rect)
{
  // rect is always from nodes so adjust nodes to give rect
  updateRect();

  double fx = (rect_.getWidth () > 0.0 ? rect.getWidth ()/rect_.getWidth () : 1.0);
  double fy = (rect_.getHeight() > 0.0 ? rect.getHeight()/rect_.getHeight() : 1.0);

  scale(fx, fy);

  updateRect();

  double dx = rect.getXMin() - rect_.getXMin();
  double dy = rect.getYMin() - rect_.getYMin();

  moveBy(Point(dx, dy));

  updateRect();
}

void
CQChartsSankeyPlotGraph::
updateRect()
{
  BBox bbox;

  for (const auto &node : nodes())
    bbox += node->rect().getCenter();

  rect_ = bbox;

  if (obj_)
    obj_->setRect(rect_);
}

//---

CQChartsSankeyPlotGraph::Nodes
CQChartsSankeyPlotGraph::
placeNodes() const
{
  Nodes nodes;

  for (auto &node : this->nodes())
    nodes.push_back(node);

  return nodes;
}

void
CQChartsSankeyPlotGraph::
place() const
{
  if (isPlaced())
    return;

  auto *th = const_cast<CQChartsSankeyPlotGraph *>(this);

  plot_->placeGraph(th);
}

void
CQChartsSankeyPlotGraph::
moveBy(const Point &delta)
{
  Node::moveBy(delta);

  for (const auto &node : nodes())
    node->moveBy(delta);

  //---

  if (obj_)
    obj_->moveBy(delta);
}

void
CQChartsSankeyPlotGraph::
scale(double fx, double fy)
{
  auto p = rect().getCenter();

  for (const auto &node : nodes()) {
    auto p1 = node->rect().getCenter();

    double xc = p.x + (p1.x - p.x)*fx;
    double yc = p.y + (p1.y - p.y)*fy;

    node->moveBy(Point(xc - p1.x, yc - p1.y));
  }

  //---

  if (obj_)
    obj_->setRect(rect_);
}
