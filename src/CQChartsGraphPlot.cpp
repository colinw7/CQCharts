#include <CQChartsGraphPlot.h>
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

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <deque>

CQChartsGraphPlotType::
CQChartsGraphPlotType()
{
}

void
CQChartsGraphPlotType::
addParameters()
{
  CQChartsConnectionPlotType::addParameters();
}

QString
CQChartsGraphPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Graph Plot").
    h3("Summary").
     p("Draw connected objects as a connected flow graph.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/graph.png"));
}

bool
CQChartsGraphPlotType::
isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsGraphPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
}

CQChartsPlot *
CQChartsGraphPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsGraphPlot(view, model);
}

//------

CQChartsGraphPlot::
CQChartsGraphPlot(View *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("graph"), model),
 CQChartsObjTextData      <CQChartsGraphPlot>(this),
 CQChartsObjNodeShapeData <CQChartsGraphPlot>(this),
 CQChartsObjEdgeShapeData <CQChartsGraphPlot>(this),
 CQChartsObjGraphShapeData<CQChartsGraphPlot>(this)
{
}

CQChartsGraphPlot::
~CQChartsGraphPlot()
{
  term();
}

//---

void
CQChartsGraphPlot::
init()
{
  CQChartsConnectionPlot::init();

  //---

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

void
CQChartsGraphPlot::
term()
{
  // delete objects first to ensure link from edge/node to object reset
  clearPlotObjects();

  clearNodesAndEdges();
}

//---

void
CQChartsGraphPlot::
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
CQChartsGraphPlot::
setNodeXMargin(const Length &l)
{
  CQChartsUtil::testAndSet(nodeXMargin_, l, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphPlot::
setNodeYMargin(const Length &l)
{
  CQChartsUtil::testAndSet(nodeYMargin_, l, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphPlot::
setNodeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(nodeWidth_, l, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphPlot::
setNodeXScaled(bool b)
{
  CQChartsUtil::testAndSet(nodeXScaled_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphPlot::
setNodeShape(const NodeShape &s)
{
  CQChartsUtil::testAndSet(nodeShape_, s, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGraphPlot::
setEdgeShape(const EdgeShape &s)
{
  CQChartsUtil::testAndSet(edgeShape_, s, [&]() { updateObjs(); } );
}

void
CQChartsGraphPlot::
setEdgeScaled(bool b)
{
  CQChartsUtil::testAndSet(edgeScaled_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGraphPlot::
setBlendEdgeColor(bool b)
{
  CQChartsUtil::testAndSet(blendEdgeColor_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsGraphPlot::
setAlign(const Align &a)
{
  CQChartsUtil::testAndSet(align_, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGraphPlot::
setAdjustNodes(bool b)
{
  CQChartsUtil::testAndSet(adjustNodes_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphPlot::
setAutoCreateGraphs(bool b)
{
  CQChartsUtil::testAndSet(autoCreateGraphs_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGraphPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc, bool hidden=false) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  CQChartsConnectionPlot::addProperties();

  //---

  // placment
  addProp("placement", "adjustNodes"     , "adjustNodes"     , "Adjust node placement");
  addProp("placement", "autoCreateGraphs", "autoCreateGraphs", "Auto create graphs");

  // coloring
  addProp("coloring", "blendEdgeColor", "", "Blend Edge Node Colors");

  // node
  addProp("node", "nodeShape"  , "shapeType", "Node shape type");
  addProp("node", "nodeXMargin", "marginX"  , "Node X margin");
  addProp("node", "nodeYMargin", "marginY"  , "Node Y margin");
  addProp("node", "nodeWidth"  , "width"    , "Node width (in pixels)");
  addProp("node", "nodeXScaled", "scaleX"   , "Node is X scaled");

  // node style
  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  //---

  // edge
  addProp("edge", "edgeShape" , "shapeType", "Edge shape type");
  addProp("edge", "edgeScaled", "scaled"   , "Edge is scaled");

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
  addProp("text", "align"      , "align"  , "Text label align");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);
}

//---

CQChartsGeom::Range
CQChartsGraphPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsGraphPlot::calcRange");

//auto *th = const_cast<CQChartsGraphPlot *>(this);

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

#if 0
CQChartsGeom::Range
CQChartsGraphPlot::
getCalcDataRange() const
{
  return Range(bbox_.getXMin(), bbox_.getYMax(), bbox_.getXMax(), bbox_.getYMin());
}
#endif

CQChartsGeom::Range
CQChartsGraphPlot::
objTreeRange() const
{
  auto bbox = nodesBBox();

  return Range(bbox.getXMin(), bbox.getYMax(), bbox.getXMax(), bbox.getYMin());
}

CQChartsGeom::BBox
CQChartsGraphPlot::
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

//------

bool
CQChartsGraphPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsGraphPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsGraphPlot *>(this);

  th->clearErrors();

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
CQChartsGraphPlot::
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

  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    graph->updateRect();
  }
}

//------

bool
CQChartsGraphPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsGraphPlot::initHierObjs");

  return CQChartsConnectionPlot::initHierObjs();
}

void
CQChartsGraphPlot::
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
CQChartsGraphPlot::
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
CQChartsGraphPlot::
initHierObjsAddConnection(const QString &srcStr, const QString &destStr, int srcDepth,
                          double value, Node* &srcNode, Node* &destNode) const
{
  int destDepth = srcDepth + 1;

  if (maxDepth() <= 0 || srcDepth <= maxDepth())
    srcNode = findNode(srcStr);

  if (maxDepth() <= 0 || destDepth <= maxDepth())
    destNode = findNode(destStr);

  //---

  // create edge and link src/dest nodes
  auto *edge = (srcNode && destNode ? createEdge(OptReal(value), srcNode, destNode) : nullptr);

  if (edge) {
    srcNode ->addDestEdge(edge);
    destNode->addSrcEdge (edge);
  }

  //---

  // set node depths
  if (srcNode)
    srcNode->setDepth(srcDepth);

  if (destNode)
    destNode->setDepth(destDepth);
}

//---

bool
CQChartsGraphPlot::
initPathObjs() const
{
  CQPerfTrace trace("CQChartsGraphPlot::initPathObjs");

  //---

  auto *th = const_cast<CQChartsGraphPlot *>(this);

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
CQChartsGraphPlot::
addPathValue(const PathData &pathData) const
{
  int n = pathData.pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsGraphPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  auto separator = (this->separator().length() ? this->separator()[0] : QChar('/'));

  auto path1 = pathData.pathStrs[0];

  for (int i = 1; i < n; ++i) {
    auto path2 = path1 + separator + pathData.pathStrs[i];

    auto *srcNode  = findNode(path1);
    auto *destNode = findNode(path2);

    srcNode ->setLabel(pathData.pathStrs[i - 1]);
    destNode->setLabel(pathData.pathStrs[i    ]);

    srcNode ->setDepth(i - 1);
    destNode->setDepth(i    );

    if (i < n - 1) {
      bool hasEdge = srcNode->hasDestNode(destNode);

      if (! hasEdge) {
        // create edge and link src/dest nodes
        auto *edge = createEdge(OptReal(), srcNode, destNode);

        srcNode ->addDestEdge(edge);
        destNode->addSrcEdge (edge);
      }
    }
    else {
      // create edge and link src/dest nodes
      auto *edge = createEdge(OptReal(), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);

      //---

      // add model indices
      auto addModelInd = [&](const ModelIndex &modelInd) {
        if (modelInd.isValid())
          edge->addModelInd(modelInd);
      };

      addModelInd(pathData.pathModelInd );
      addModelInd(pathData.valueModelInd);

      //---

      // set destination node value (will be propagated up)
      destNode->setValue(pathData.value);
    }

    path1 = path2;
  }
}

void
CQChartsGraphPlot::
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
          //int ns = node->srcEdges().size();

          double value = node->value().realOr(1.0);

          for (auto &srcEdge : node->srcEdges()) {
            if (! srcEdge->hasValue())
              srcEdge->setValue(OptReal(value));

            auto *srcNode = srcEdge->srcNode();

            for (const auto &edge : srcNode->destEdges()) {
              if (edge->destNode() == node)
                edge->setValue(OptReal(value));
            }
          }
        }
      }
    }
  }
}

//---

bool
CQChartsGraphPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsGraphPlot::initFromToObjs");

  return CQChartsConnectionPlot::initFromToObjs();
}

void
CQChartsGraphPlot::
addFromToValue(const FromToData &fromToData) const
{
  // get src node
  auto *srcNode = findNode(fromToData.fromStr);

  if (fromToData.depth > 0)
    srcNode->setDepth(fromToData.depth);

  //---

  // set group
  if (fromToData.groupData.ng > 1) {
    srcNode->setGroup(fromToData.groupData.ig, fromToData.groupData.ng);

    auto *th = const_cast<CQChartsGraphPlot *>(this);

    th->numGroups_ = std::max(numGroups_, fromToData.groupData.ng);
  }
  else
    srcNode->setGroup(-1);

  //---

  // Just node
  if (fromToData.toStr == "") {
    // set node color (if color column specified)
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      srcNode->setFillColor(c);

    //---

    // set node name values (attribute column)
    processNodeNameValues(srcNode, fromToData.nameValues);

    //---

    auto groupNames = fromToData.groupData.value.toString().split("/");

    int graphId = -1, parentGraphId = -1;

    int ng = groupNames.length();

    if (ng > 0) {
      auto *th = const_cast<CQChartsGraphPlot *>(this);

      graphId = th->groupValueInd_.calcId(groupNames[ng - 1]);

      if (ng > 1)
        parentGraphId = th->groupValueInd_.calcId(groupNames[ng - 2]);
    }

    if (graphId >= 0)
      srcNode->setGraphId(graphId);

    if (parentGraphId >= 0)
      srcNode->setParentGraphId(parentGraphId);
  }
  else {
    if (fromToData.fromStr == fromToData.toStr)
      return;

    auto *destNode = findNode(fromToData.toStr);

    if (fromToData.depth > 0)
      destNode->setDepth(fromToData.depth + 1);

    //---

    // create edge and link src/dest nodes
    auto *edge = createEdge(fromToData.value, srcNode, destNode);

    srcNode ->addDestEdge(edge, /*primary*/true );
    destNode->addSrcEdge (edge, /*primary*/false);

    //---

    // add model indices
    auto addModelInd = [&](const ModelIndex &modelInd) {
      if (modelInd.isValid())
        edge->addModelInd(modelInd);
    };

    addModelInd(fromToData.fromModelInd  );
    addModelInd(fromToData.toModelInd    );
    addModelInd(fromToData.valueModelInd );
    addModelInd(fromToData.depthModelInd );

    edge->setNamedColumn("From" , fromToData.fromModelInd  .column());
    edge->setNamedColumn("To"   , fromToData.toModelInd    .column());
    edge->setNamedColumn("Value", fromToData.valueModelInd .column());
    edge->setNamedColumn("Depth", fromToData.depthModelInd .column());

    //---

    // set edge color (if color column specified)
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      edge->setFillColor(c);

    //---

    // set edge name values (attribute column)
    processEdgeNameValues(edge, fromToData.nameValues);
  }
}

//---

bool
CQChartsGraphPlot::
initLinkObjs() const
{
  CQPerfTrace trace("CQChartsGraphPlot::initLinkObjs");

  return CQChartsConnectionPlot::initLinkObjs();
}

void
CQChartsGraphPlot::
addLinkConnection(const LinkConnectionData &linkConnectionData) const
{
  // get src/dest nodes (TODO: allow single source node)
  auto *srcNode  = findNode(linkConnectionData.srcStr);
  auto *destNode = findNode(linkConnectionData.destStr);
//assert(srcNode != destNode);

  //---

  // create edge and link src/dest nodes
  auto *edge = createEdge(linkConnectionData.value, srcNode, destNode);

  srcNode ->addDestEdge(edge);
  destNode->addSrcEdge (edge);

  //---

  // add model indices
  auto addModelInd = [&](const ModelIndex &modelInd) {
    if (modelInd.isValid())
      edge->addModelInd(modelInd);
  };

  addModelInd(linkConnectionData.groupModelInd);
  addModelInd(linkConnectionData.linkModelInd );
  addModelInd(linkConnectionData.valueModelInd);
  addModelInd(linkConnectionData.nameModelInd );
  addModelInd(linkConnectionData.depthModelInd);

  //---

  // set edge color (if color column specified)
  Color c;

  if (colorColumnColor(linkConnectionData.linkModelInd.row(),
                       linkConnectionData.linkModelInd.parent(), c))
    edge->setFillColor(c);

  //---

  // set value on dest node (NEEDED ?)
  destNode->setValue(linkConnectionData.value);

  //---

  // set group
  if (linkConnectionData.groupData.isValid()) {
    srcNode->setGroup(linkConnectionData.groupData.ig, linkConnectionData.groupData.ng);

    auto *th = const_cast<CQChartsGraphPlot *>(this);

    th->numGroups_ = std::max(numGroups_, linkConnectionData.groupData.ng);
  }
  else
    srcNode->setGroup(-1);

  //---

  // set name index
  if (linkConnectionData.nameModelInd.isValid()) {
    auto nameModelInd1 = normalizeIndex(linkConnectionData.nameModelInd);

    srcNode->setInd(nameModelInd1);
  }

  //---

  // set depth
  if (linkConnectionData.depth > 0) {
    srcNode ->setDepth(linkConnectionData.depth);
    destNode->setDepth(linkConnectionData.depth + 1);
  }

  //---

  // set edge name values (attribute column)
  processEdgeNameValues(edge, linkConnectionData.nameValues);
}

//---

bool
CQChartsGraphPlot::
initConnectionObjs() const
{
  CQPerfTrace trace("CQChartsGraphPlot::initConnectionObjs");

  return CQChartsConnectionPlot::initConnectionObjs();
}

void
CQChartsGraphPlot::
addConnectionObj(int id, const ConnectionsData &connectionsData) const
{
  // get src node
  auto srcStr = QString("%1").arg(id);

  auto *srcNode = findNode(srcStr);

  srcNode->setName(connectionsData.name);

  //---

  // set group
  if (connectionsData.groupData.isValid()) {
    srcNode->setGroup(connectionsData.groupData.ig, connectionsData.groupData.ng);

    auto *th = const_cast<CQChartsGraphPlot *>(this);

    th->numGroups_ = std::max(numGroups_, connectionsData.groupData.ng);
  }
  else
    srcNode->setGroup(-1);

  //---

  // add model indices
  auto addModelInd = [&](const ModelIndex &modelInd) {
    if (modelInd.isValid())
      srcNode->addModelInd(modelInd);
  };

  addModelInd(connectionsData.nodeModelInd);
  addModelInd(connectionsData.connectionsModelInd);
  addModelInd(connectionsData.nameModelInd);
  addModelInd(connectionsData.groupModelInd);

  //---

  // set node name values (attribute column)
  processNodeNameValues(srcNode, connectionsData.nameValues);

  //---

  // process connections
  for (const auto &connection : connectionsData.connections) {
    // get destination node
    auto destStr = QString("%1").arg(connection.node);

    auto *destNode = findNode(destStr);

    //---

    // create edge and link src/dest nodes
    auto *edge = createEdge(OptReal(connection.value), srcNode, destNode);

    srcNode ->addDestEdge(edge);
    destNode->addSrcEdge (edge);

    //---

    // set value
    destNode->setValue(OptReal(connection.value));
  }
}

//---

bool
CQChartsGraphPlot::
initTableObjs() const
{
  CQPerfTrace trace("CQChartsGraphPlot::initTableObjs");

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

    auto srcStr = QString("%1").arg(tableConnectionData.from());

    auto *srcNode = findNode(srcStr);

    srcNode->setName (tableConnectionData.name());
    srcNode->setGroup(tableConnectionData.group().ig, tableConnectionData.group().ng);

    for (const auto &value : tableConnectionData.values()) {
      auto destStr = QString("%1").arg(value.to);

      auto *destNode = findNode(destStr);

      // create edge and link src/dest nodes
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
CQChartsGraphPlot::
processNodeNameValues(Node *node, const NameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    auto        value = nv.second.toString();

    processNodeNameValue(node, name, value);
  }
}

void
CQChartsGraphPlot::
processNodeNameValue(Node *node, const QString &name, const QString &value) const
{
  // shape
  if      (name == "shape") {
    if      (value == "diamond")
      node->setShapeType(Node::ShapeType::DIAMOND);
    else if (value == "box")
      node->setShapeType(Node::ShapeType::BOX);
    else if (value == "polygon")
      node->setShapeType(Node::ShapeType::POLYGON);
    else if (value == "circle")
      node->setShapeType(Node::ShapeType::CIRCLE);
    else if (value == "doublecircle")
      node->setShapeType(Node::ShapeType::DOUBLE_CIRCLE);
    else if (value == "record")
      node->setShapeType(Node::ShapeType::BOX);
    else if (value == "plaintext")
      node->setShapeType(Node::ShapeType::BOX);
    else
      node->setShapeType(Node::ShapeType::BOX);
  }
  // num sides
  else if (name == "num_sides") {
    bool ok;

    int n = value.toInt(&ok);

    if (ok)
      node->setNumSides(n);
  }
  else if (name == "label") {
    node->setLabel(value);
  }
  else if (name == "fill_color" || name == "color") {
    node->setFillColor(CQChartsColor(value));
  }
  else if (name == "fill_alpha" || name == "alpha") {
    node->setFillAlpha(CQChartsAlpha(value));
  }
  else if (name == "fill_pattern" || name == "pattern") {
    node->setFillPattern(CQChartsFillPattern(value));
  }
  else if (name == "stroke_color") {
    node->setStrokeColor(CQChartsColor(value));
  }
  else if (name == "stroke_alpha") {
    node->setStrokeAlpha(CQChartsAlpha(value));
  }
  else if (name == "stroke_width" || name == "width") {
    node->setStrokeWidth(CQChartsLength(value));
  }
  else if (name == "stroke_dash" || name == "dash") {
    node->setStrokeDash(CQChartsLineDash(value));
  }
}

void
CQChartsGraphPlot::
processEdgeNameValues(Edge *edge, const NameValues &nameValues) const
{
  auto *srcNode  = edge->srcNode ();
  auto *destNode = edge->destNode();

  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    auto        value = nv.second.toString();

    if      (name == "shape") {
      if (value == "arrow")
        edge->setShapeType(Edge::ShapeType::ARROW);
    }
    else if (name == "label") {
      edge->setLabel(value);
    }
    else if (name == "color") {
      edge->setFillColor(CQChartsColor(value));
    }
    else if (name.left(4) == "src_") {
      processNodeNameValue(srcNode, name.mid(4), value);
    }
    else if (name.left(5) == "dest_") {
      processNodeNameValue(destNode, name.mid(5), value);
    }
  }
}

//---

void
CQChartsGraphPlot::
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
CQChartsGraphPlot::
createObjsGraph(PlotObjs &objs) const
{
  // create graphs
  createGraphs();

  //---

  // place graphs
  placeGraphs();

  //---

  // add objects to plot
  addObjects(objs);
}

void
CQChartsGraphPlot::
addObjects(PlotObjs &objs) const
{
  // add node objects (per graph)
  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    if (graph->nodes().empty() || ! graph->rect().isValid())
      continue;

    //---

    // add node objects
    for (auto *node : graph->nodes()) {
      auto *nodeObj = createObjFromNode(graph, node);

      objs.push_back(nodeObj);
    }

    //---

    // create graph object
    auto *graphObj = createGraphObj(graph->rect(), graph);

    if (graph->parent())
      graphObj->setEditable(true);

    graph->setObj(graphObj);

    objs.push_back(graphObj);
  }

  //---

  // add edge objects
  for (const auto &edge : edges_) {
    if (! edge->srcNode()->isVisible() || ! edge->destNode()->isVisible())
      continue;

    auto *edgeObj = addEdgeObj(edge);

    objs.push_back(edgeObj);
  }
}

// place graphs
void
CQChartsGraphPlot::
placeGraphs() const
{
  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    placeGraph(graph);
  }
}

// place nodes in graph
void
CQChartsGraphPlot::
placeGraph(Graph *graph) const
{
  if (graph->isPlaced())
    return;

  //---

  // place children first
  for (const auto &child : graph->children())
    child->place();

  //---

  // get placeable nodes (nodes and sub graphs)
  auto nodes = graph->placeNodes();

  placeGraphNodes(graph, nodes);
}

void
CQChartsGraphPlot::
placeGraphNodes(Graph *graph, const Nodes &nodes) const
{
  auto *th = const_cast<CQChartsGraphPlot *>(this);

  //---

  // set max depth of all graph nodes
  updateGraphMaxDepth(graph, nodes);

  //---

  // place graph nodes at x position
  for (const auto &node : nodes) {
    int xpos = calcXPos(graph, node);

    graph->addDepthSize(xpos, node->edgeSum());
    graph->addDepthNode(xpos, node);
  }

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

  //---

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
CQChartsGraphPlot::
createGraphs() const
{
  auto *th = const_cast<CQChartsGraphPlot *>(this);

  // Add nodes to graph for group
  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    int graphId       = node->graphId();
    int parentGraphId = node->parentGraphId();

    if (graphId == -1) {
      graphId = 0;

      node->setGraphId(graphId);
    }

    auto *graph = th->getGraph(graphId, parentGraphId);

    graph->addNode(node);
  }

  //---

  if (graphs_.size() == 1 && isAutoCreateGraphs()) {
    autoCreateGraphs();
  }
}

void
CQChartsGraphPlot::
autoCreateGraphs() const
{
  Nodes noSrcNodes;

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    if (node->srcEdges().empty())
      noSrcNodes.push_back(node);
  }

  //---

  using NodeSet     = std::set<Node *>;
  using NodeQueue   = std::deque<Node *>;
  using NodeNodeSet = std::map<Node *, NodeSet>;

  NodeNodeSet nodeNodeSet;

  for (const auto &node : noSrcNodes) {
    auto &nodeSet = nodeNodeSet[node];

    NodeQueue workSet;

    nodeSet.insert(node);

    workSet.push_back(node);

    while (! workSet.empty()) {
      auto *node = workSet.front();

      workSet.pop_front();

#if 0
      for (auto &edge : node->srcEdges()) {
        if (edge->isSelf()) continue;

        auto *node1 = edge->srcNode();

        if (nodeSet.find(node1) == nodeSet.end()) {
          nodeSet.insert(node1);

          workSet.push_back(node1);
        }
      }
#endif

      for (auto &edge : node->destEdges()) {
        if (edge->isSelf()) continue;

        auto *node1 = edge->destNode();

        if (nodeSet.find(node1) == nodeSet.end()) {
          nodeSet.insert(node1);

          workSet.push_back(node1);
        }
      }
    }
  }

  //---

  auto *root = graphs_.begin()->second;

  auto rootNodes = root->nodes();

  root->removeAllNodes();

  for (const auto &node : rootNodes)
    node->setGraphId(-1);

  //---

  // sort by length

  using NodeSetArray = std::vector<NodeSet>;

  using LengthNodeNodeSets = std::map<int, NodeSetArray>;

  LengthNodeNodeSets lengthNodeNodeSets;

  for (const auto &pn : nodeNodeSet) {
    int len = pn.second.size();

    lengthNodeNodeSets[len].push_back(pn.second);
  }

  //---

  int graphId = root->id() + 1;

  for (const auto &pl : lengthNodeNodeSets) {
    for (const auto &nodeSet : pl.second) {
      int graphId1 = graphId++;

      auto *graph1 = getGraph(graphId1, root->id());

      for (auto &node1 : nodeSet) {
        if (node1->graphId() < 0) {
          node1->setGraphId(graphId1);

          graph1->addNode(node1);
        }
      }
    }
  }

  for (const auto &node : rootNodes) {
    if (node->graphId() < 0) {
      node->setGraphId(root->id());

      root->addNode(node);
    }
  }
}

CQChartsGraphPlotGraph *
CQChartsGraphPlot::
getGraph(int graphId, int parentGraphId) const
{
  auto pg = graphs_.find(graphId);

  if (pg == graphs_.end()) {
    auto *th = const_cast<CQChartsGraphPlot *>(this);

    auto name = QString("%1").arg(graphId);

    auto *graph = new Graph(this, name);

    graph->setId(graphId);

    pg = th->graphs_.insert(th->graphs_.end(), Graphs::value_type(graphId, graph));

    //---

    if (parentGraphId >= 0) {
      auto *parentGraph = getGraph(parentGraphId, -1);

      graph->setParent(parentGraph);

      parentGraph->addChild(graph);
    }
  }

  return (*pg).second;
}

void
CQChartsGraphPlot::
clearGraphs()
{
  for (auto &pg : graphs_)
    delete pg.second;

  graphs_.clear();
}

void
CQChartsGraphPlot::
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
  graph->setValueScale (graph->maxHeight() > 0.0 ? ys2/ graph->maxHeight()      : 1.0);
}

double
CQChartsGraphPlot::
calcNodeXMargin() const
{
  double nodeXMargin = lengthPlotWidth(this->nodeXMargin());

  nodeXMargin = std::min(std::max(nodeXMargin, 0.0), 1.0);

  auto pixelNodeXMargin = windowToPixelWidth(nodeXMargin);

  if (pixelNodeXMargin < minNodeMargin())
    nodeXMargin = pixelToWindowWidth(minNodeMargin());

  return nodeXMargin;
}

double
CQChartsGraphPlot::
calcNodeYMargin() const
{
  double nodeYMargin = lengthPlotHeight(this->nodeYMargin());

  nodeYMargin = std::min(std::max(nodeYMargin, 0.0), 1.0);

  auto pixelNodeYMargin = windowToPixelHeight(nodeYMargin);

  if (pixelNodeYMargin < minNodeMargin())
    nodeYMargin = pixelToWindowHeight(minNodeMargin());

  return nodeYMargin;
}

void
CQChartsGraphPlot::
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
CQChartsGraphPlot::
placeDepthSubNodes(Graph *graph, int xpos, const Nodes &nodes) const
{
  double xmargin = calcNodeXMargin();

  // place nodes to fit in bbox
  double xs = bbox_.getWidth ();
  double ys = bbox_.getHeight();

  double dx = 1.0;

  if (graph->maxNodeDepth() > 0) {
    if (isNodeXScaled())
      dx = xs/(graph->maxNodeDepth() + 1);
    else
      dx = xs/graph->maxNodeDepth();
  }

  double xm = lengthPlotWidth (nodeWidth());
  double ym = lengthPlotHeight(nodeWidth());

  if (isNodeXScaled()) {
    xm = dx - xmargin;
    ym = xm;
  }

  //---

  // get sum of margins nodes at depth
  double height = graph->valueMargin()*(graph->maxHeight() - 1);

  height += graph->maxHeight()*graph->valueScale();

  //---

  // calc tip (placing top to bottom)
  double y1 = bbox_.getYMax() - (ys - height)/2.0;

  //---

  double nh = graph->valueScale();

  double dy = nh + graph->valueMargin();

  y1 = bbox_.getYMid() + dy*graph->maxHeight()/2.0;

  y1 -= dy*(graph->maxHeight() - nodes.size())/2.0;

  y1 -= graph->valueMargin()/2.0;

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

    int xpos1 = calcXPos(graph, node);
    assert(xpos == xpos1);

    double x11 = bbox_.getXMin() + xpos*dx; // left
    double x12 = x11 + xm;

    double yc = y1 - h/2.0; // placement center

    double y11 = yc - ym/2.0;
    double y12 = yc + ym/2.0;

    //---

    BBox rect;

    auto shapeType = (NodeObj::ShapeType) node->shapeType();

    if (shapeType == NodeObj::ShapeType::NONE)
      shapeType = (NodeObj::ShapeType) nodeShape();

    if (shapeType == (NodeObj::ShapeType) NODE_SHAPE_DIAMOND ||
        shapeType == (NodeObj::ShapeType) NODE_SHAPE_BOX ||
        shapeType == (NodeObj::ShapeType) NODE_SHAPE_POLYGON ||
        shapeType == (NodeObj::ShapeType) NODE_SHAPE_CIRCLE ||
        shapeType == (NodeObj::ShapeType) NODE_SHAPE_DOUBLE_CIRCLE) {
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

    y1 -= h + graph->valueMargin();
  }
}

CQChartsGraphPlot::NodeObj *
CQChartsGraphPlot::
createObjFromNode(Graph *, Node *node) const
{
//int numNodes = graph->nodes().size(); // node id needs to be per graph
  int numNodes = indNodeMap_.size();

  ColorInd iv(node->id(), numNodes);

  auto *nodeObj = createNodeObj(node->rect(), node, iv);

  NodeObj::ShapeType shapeType = (NodeObj::ShapeType) node->shapeType();

  if (shapeType == NodeObj::ShapeType::NONE)
    shapeType = (NodeObj::ShapeType) nodeShape();

  nodeObj->setShapeType(shapeType);
  nodeObj->setHierName (node->str());

  for (const auto &modelInd : node->modelInds())
    nodeObj->addModelInd(normalizedModelIndex(modelInd));

  node->setObj(nodeObj);

  return nodeObj;
}

int
CQChartsGraphPlot::
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
#if 0
      else if (align() == Align::RAND) {
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

CQChartsGraphEdgeObj *
CQChartsGraphPlot::
addEdgeObj(Edge *edge) const
{
  double xm = bbox_.getHeight()*edgeMargin_;
  double ym = bbox_.getWidth ()*edgeMargin_;

  BBox nodeRect;

  nodeRect += edge->srcNode ()->rect();
  nodeRect += edge->destNode()->rect();

  BBox rect(nodeRect.getXMin() - xm, nodeRect.getYMin() - ym,
            nodeRect.getXMax() + xm, nodeRect.getYMax() + ym);

  auto *edgeObj = createEdgeObj(rect, edge);

  EdgeObj::ShapeType shapeType = (EdgeObj::ShapeType) edge->shapeType();

  if (shapeType == EdgeObj::ShapeType::NONE)
    shapeType = (EdgeObj::ShapeType) edgeShape();

  edgeObj->setShapeType(shapeType);

  edge->setObj(edgeObj);

  return edgeObj;
}

void
CQChartsGraphPlot::
updateGraphMaxDepth(Graph *graph, const Nodes &nodes) const
{
  // calc max depth (source or dest) depending on align for xpos calc
  bool set = false;

  graph->setMinNodeDepth(0);
  graph->setMaxNodeDepth(0);

  auto updateNodeDepth = [&](int depth) {
    if (! set) {
      graph->setMinNodeDepth(depth);
      graph->setMaxNodeDepth(depth);

      set = true;
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
CQChartsGraphPlot::
adjustNodes() const
{
  bool changed = false;

  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    auto nodes = graph->placeNodes();

    if (adjustGraphNodes(graph, nodes))
      changed = true;
  }

  return changed;
}

bool
CQChartsGraphPlot::
adjustGraphNodes(Graph *graph, const Nodes &nodes) const
{
//auto *th = const_cast<CQChartsGraphPlot *>(this);

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
  }

  //---

//placeDepthNodes(graph);

  //---

  return true;
}

void
CQChartsGraphPlot::
initPosNodesMap(Graph *graph, const Nodes &nodes) const
{
  // get nodes by x pos
  graph->resetPosNodes();

  for (const auto &node : nodes)
    graph->addPosNode(node);
}

bool
CQChartsGraphPlot::
adjustNodeCenters(Graph *graph) const
{
  // adjust nodes so centered on src nodes
  bool changed = false;

  // second to last
  int posNodesDepth = graph->posNodesMap().size();

  for (int xpos = 1; xpos <= posNodesDepth; ++xpos) {
    if (adjustPosNodes(graph, xpos))
      changed = true;
  }

  removeOverlaps(graph);

  //---

  // second to last to first
  for (int xpos = posNodesDepth - 1; xpos >= 0; --xpos) {
    if (adjustPosNodes(graph, xpos))
      changed = true;
  }

  removeOverlaps(graph);

  return changed;
}

bool
CQChartsGraphPlot::
adjustPosNodes(Graph *graph, int xpos) const
{
  if (! graph->hasPosNodes(xpos))
    return false;

  bool changed = false;

  const auto &nodes = graph->posNodes(xpos);

  for (const auto &node : nodes) {
    if (adjustNode(node))
      changed = true;
  }

  return changed;
}

bool
CQChartsGraphPlot::
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
CQChartsGraphPlot::
removePosOverlaps(Graph *graph, const Nodes &nodes) const
{
  double ym = pixelToWindowHeight(minNodeMargin());

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
      spreadPosNodes(graph, nodes);
  }

  return changed;
}

bool
CQChartsGraphPlot::
spreadPosNodes(Graph *, const Nodes &nodes) const
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

bool
CQChartsGraphPlot::
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
CQChartsGraphPlot::
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
CQChartsGraphPlot::
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

bool
CQChartsGraphPlot::
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

//---

#if 0
void
CQChartsGraphPlot::
adjustGraphs() const
{
  if (graphs_.size() <= 1)
    return;

  double h = 0;

  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    h += graph->rect().getHeight();
  }

  double f = (h > 0.0 ? 2.0/h : 1.0);

  double y = -1.0;

  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    double dy = y - f*graph->rect().getYMin();

    graph->moveBy(Point(0, dy));
    graph->scale (f, f);

    y += graph->rect().getHeight();
  }
}
#endif

//---

CQChartsGraphPlotNode *
CQChartsGraphPlot::
findNode(const QString &name) const
{
  auto p = nameNodeMap_.find(name);

  if (p != nameNodeMap_.end())
    return (*p).second;

  auto *node = createNode(name);

  return node;
}

CQChartsGraphPlotNode *
CQChartsGraphPlot::
createNode(const QString &name) const
{
  auto *node = new Node(this, name);

  node->setId(nameNodeMap_.size());

  auto *th = const_cast<CQChartsGraphPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(name, node));

  assert(node == (*p1).second);

  th->indNodeMap_[node->id()] = node;

  node->setName(name);

  return node;
}

CQChartsGraphPlotEdge *
CQChartsGraphPlot::
createEdge(const OptReal &value, Node *srcNode, Node *destNode) const
{
  assert(srcNode && destNode);

  auto *edge = new Edge(this, value, srcNode, destNode);

  auto *th = const_cast<CQChartsGraphPlot *>(this);

  th->edges_.push_back(edge);

  edge->setId(th->edges_.size());

  return edge;
}

//---

void
CQChartsGraphPlot::
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
  else
    CQChartsPlot::keyPress(key, modifier);
}

void
CQChartsGraphPlot::
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
    auto       *node     = pn.first;
    const auto &nameData = pn.second;

    for (auto &pd : nameData) {
      std::cerr << node->name().toStdString() << " " << pd.first.toStdString() << "\n";
    }
  }
}

//---

CQChartsGraphNodeObj *
CQChartsGraphPlot::
createNodeObj(const BBox &rect, Node *node, const ColorInd &ind) const
{
  return new NodeObj(this, rect, node, ind);
}

CQChartsGraphEdgeObj *
CQChartsGraphPlot::
createEdgeObj(const BBox &rect, Edge *edge) const
{
  return new EdgeObj(this, rect, edge);
}

CQChartsGraphGraphObj *
CQChartsGraphPlot::
createGraphObj(const BBox &rect, Graph *graph) const
{
  return new GraphObj(this, rect, graph);
}

//------

CQChartsGraphPlotNode::
CQChartsGraphPlotNode(const Plot *plot, const QString &str) :
 plot_(plot), str_(str)
{
}

CQChartsGraphPlotNode::
~CQChartsGraphPlotNode()
{
  assert(! obj_);
}

void
CQChartsGraphPlotNode::
addSrcEdge(Edge *edge, bool primary)
{
  edge->destNode()->parent_ = edge->srcNode();

  srcEdges_.push_back(edge);

  srcDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

void
CQChartsGraphPlotNode::
addDestEdge(Edge *edge, bool primary)
{
  edge->destNode()->parent_ = edge->srcNode();

  destEdges_.push_back(edge);

  destDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

bool
CQChartsGraphPlotNode::
hasDestNode(Node *destNode) const
{
  for (auto &destEdge : destEdges()) {
    if (destEdge->destNode() == destNode)
      return true;
  }

  return false;
}

int
CQChartsGraphPlotNode::
srcDepth() const
{
  if (depth() >= 0)
    return depth() - 1;

  NodeSet visited;

  visited.insert(this);

  return calcSrcDepth(visited);
}

int
CQChartsGraphPlotNode::
calcSrcDepth(NodeSet &visited) const
{
  if (srcDepth_ >= 0)
    return srcDepth_;

  auto *th = const_cast<CQChartsGraphPlotNode *>(this);

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
CQChartsGraphPlotNode::
destDepth() const
{
  if (depth() >= 0)
    return depth() + 1;

  NodeSet visited;

  visited.insert(this);

  return calcDestDepth(visited);
}

int
CQChartsGraphPlotNode::
calcDestDepth(NodeSet &visited) const
{
  if (destDepth_ >= 0)
    return destDepth_;

  auto *th = const_cast<CQChartsGraphPlotNode *>(this);

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

const CQChartsGraphPlotNode::BBox &
CQChartsGraphPlotNode::
rect() const
{
  if (obj_) {
    assert(obj_->rect() == rect_);
  }

  return rect_;
}

void
CQChartsGraphPlotNode::
setRect(const BBox &rect)
{
  assert(rect.isSet());

  rect_ = rect;

  if (obj_) // TODO: assert null or use move by
    obj_->setRect(rect);
}

CQChartsGraphPlotGraph *
CQChartsGraphPlotNode::
graph() const
{
  if (graphId_ < 0)
    return nullptr;

  return plot_->getGraph(graphId_, -1);
}

double
CQChartsGraphPlotNode::
edgeSum() const
{
  double sum = std::max(srcEdgeSum(), destEdgeSum());

  if (CMathUtil::realEq(sum, 0.0))
    sum = 1.0;

  return sum;
}

double
CQChartsGraphPlotNode::
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
CQChartsGraphPlotNode::
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
CQChartsGraphPlotNode::
setObj(Obj *obj)
{
  obj_ = obj;
}

//---

void
CQChartsGraphPlotNode::
moveBy(const Point &delta)
{
  rect_.moveBy(delta);

  //---

  if (obj_)
    obj_->moveBy(delta);

  //---

  moveSrcEdgeRectsBy (delta);
  moveDestEdgeRectsBy(delta);
}

void
CQChartsGraphPlotNode::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);

  //---

  if (obj_)
    obj_->scale(fx, fy);
}

//---

void
CQChartsGraphPlotNode::
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
CQChartsGraphPlotNode::
setSrcEdgeRect(Edge *edge, const BBox &bbox)
{
  srcEdgeRect_[edge] = bbox;
}

void
CQChartsGraphPlotNode::
setDestEdgeRect(Edge *edge, const BBox &bbox)
{
  destEdgeRect_[edge] = bbox;
}

//------

CQChartsGraphPlotEdge::
CQChartsGraphPlotEdge(const Plot *plot, const OptReal &value, Node *srcNode, Node *destNode) :
 plot_(plot), value_(value), srcNode_(srcNode), destNode_(destNode)
{
}

CQChartsGraphPlotEdge::
~CQChartsGraphPlotEdge()
{
  assert(! obj_);
}

//---

void
CQChartsGraphPlotEdge::
setObj(Obj *obj)
{
  obj_ = obj;
}

//------

CQChartsGraphNodeObj::
CQChartsGraphNodeObj(const Plot *plot, const BBox &rect, Node *node, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), node_(node)
{
  setDetailHint(DetailHint::MAJOR);

  setEditable(true);

  //---

  placeEdges();
}

CQChartsGraphNodeObj::
~CQChartsGraphNodeObj()
{
  if (node_)
    node_->setObj(nullptr);
}

QString
CQChartsGraphNodeObj::
name() const
{
  return node()->name();
}

void
CQChartsGraphNodeObj::
setName(const QString &s)
{
  node()->setName(s);
}

double
CQChartsGraphNodeObj::
value() const
{
  return node()->value().realOr(0.0);
}

void
CQChartsGraphNodeObj::
setValue(double r)
{
  node()->setValue(CQChartsGraphPlotNode::OptReal(r));
}

int
CQChartsGraphNodeObj::
depth() const
{
  return node()->depth();
}

void
CQChartsGraphNodeObj::
setDepth(int depth)
{
  node()->setDepth(depth);
}

CQChartsGraphNodeObj::ShapeType
CQChartsGraphNodeObj::
shapeType() const
{
  return (CQChartsGraphNodeObj::ShapeType) node()->shapeType();
}

void
CQChartsGraphNodeObj::
setShapeType(const ShapeType &s)
{
  node()->setShapeType((CQChartsGraphPlotNode::ShapeType) s);
}

int
CQChartsGraphNodeObj::
numSides() const
{
  return node()->numSides();
}

void
CQChartsGraphNodeObj::
setNumSides(int n)
{
  node()->setNumSides(n);
}

CQChartsColor
CQChartsGraphNodeObj::
fillColor() const
{
  return node()->fillColor();
}

void
CQChartsGraphNodeObj::
setFillColor(const Color &c)
{
  node()->setFillColor(c);
}

void
CQChartsGraphNodeObj::
placeEdges()
{
  node()->placeEdges();
}

QString
CQChartsGraphNodeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(node()->id());
}

QString
CQChartsGraphNodeObj::
calcTipId() const
{
  Edge *edge = nullptr;

  if      (! node()->srcEdges().empty())
    edge = *node()->srcEdges().begin();
  else if (! node()->destEdges().empty())
    edge = *node()->destEdges().begin();

  auto namedColumn = [&](const QString &name, const QString &defName="") {
    if (edge && edge->hasNamedColumn(name))
      return plot_->columnHeaderName(edge->namedColumn(name));

    QString headerName = (defName.length() ? defName : name);

    return headerName;
  };

  //---

  CQChartsTableTip tableTip;

  auto name = this->name();

  if (name == "")
    name = this->id();

  auto hierName = this->hierName();

  if (hierName != name)
    tableTip.addTableRow("Hier Name", hierName);

  tableTip.addTableRow("Name", name);

  if (node()->hasValue())
    tableTip.addTableRow(namedColumn("Value"), value());

  if (depth() >= 0)
    tableTip.addTableRow(namedColumn("Depth"), depth());

  int ns = node()->srcEdges ().size();
  int nd = node()->destEdges().size();

  tableTip.addTableRow("Edges", QString("In:%1, Out:%2").arg(ns).arg(nd));

  if (plot_->groupColumn().isValid())
    tableTip.addTableRow("Group", node()->group());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsGraphNodeObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "hierName" )->setDesc("Hierarchical Name");
  model->addProperty(path1, this, "name"     )->setDesc("Name");
  model->addProperty(path1, this, "value"    )->setDesc("Value");
  model->addProperty(path1, this, "depth"    )->setDesc("Depth");
  model->addProperty(path1, this, "shapeType")->setDesc("Shape type");
  model->addProperty(path1, this, "numSides" )->setDesc("Number of Shape Sides");
  model->addProperty(path1, this, "color"    )->setDesc("Color");
}

//---

void
CQChartsGraphNodeObj::
moveBy(const Point &delta)
{
  //std::cerr << "  Move " << node()->str().toStdString() << " by " << delta.y << "\n";

  rect_.moveBy(delta);
}

void
CQChartsGraphNodeObj::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);

  placeEdges();
}

//---

bool
CQChartsGraphNodeObj::
editPress(const Point &p)
{
  editChanged_ = false;

  editHandles()->setDragPos(p);

  return true;
}

bool
CQChartsGraphNodeObj::
editMove(const Point &p)
{
  const auto &dragPos  = editHandles()->dragPos();
  const auto &dragSide = editHandles()->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles()->updateBBox(dx, dy);

  setEditBBox(editHandles()->bbox(), dragSide);

  editHandles()->setDragPos(p);

  auto *graph = node()->graph();

  if (graph)
    graph->updateRect();

  editChanged_ = true;

  const_cast<CQChartsGraphPlot *>(plot())->drawObjs();

  return true;
}

bool
CQChartsGraphNodeObj::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

bool
CQChartsGraphNodeObj::
editRelease(const Point &)
{
  if (editChanged_)
    const_cast<CQChartsGraphPlot *>(plot())->invalidateObjTree();

  return true;
}

void
CQChartsGraphNodeObj::
setEditBBox(const BBox &bbox, const CQChartsResizeSide &)
{
  assert(bbox.isSet());

  double dx = bbox.getXMin() - rect_.getXMin();
  double dy = bbox.getYMin() - rect_.getYMin();

  node()->moveBy(Point(dx, dy));
}

//---

CQChartsGraphNodeObj::PlotObjs
CQChartsGraphNodeObj::
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
CQChartsGraphNodeObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw node
  if (rect().isSet()) {
    if      (shapeType() == ShapeType::DIAMOND)
      device->drawDiamond(rect());
    else if (shapeType() == ShapeType::BOX)
      device->drawRect(rect());
    else if (shapeType() == ShapeType::POLYGON)
      device->drawPolygonSides(rect(), numSides());
    else if (shapeType() == ShapeType::CIRCLE)
      device->drawEllipse(rect());
    else if (shapeType() == ShapeType::DOUBLE_CIRCLE) {
      auto rect = this->rect();

      double dx = rect.getWidth ()/10.0;
      double dy = rect.getHeight()/10.0;

      auto rect1 = rect.expanded(dx, dy, -dx, -dy);

      device->drawEllipse(rect );
      device->drawEllipse(rect1);
    }
    else
      device->drawRect(rect());
  }

  //---

  device->resetColorNames();
}

void
CQChartsGraphNodeObj::
drawFg(PaintDevice *device) const
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
  auto ic = calcColorInd();

  PenBrush penBrush;

  auto c = plot_->interpTextColor(ic);

  plot_->setPen(penBrush, PenData(true, c, plot_->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  auto str = node()->label();

  if (! str.length())
    str = node()->name();

  //---

  double ptw = fm.width(str);

  double clipLength = plot_->lengthPixelWidth(plot()->textClipLength());

  if (clipLength > 0.0)
    ptw = std::min(ptw, clipLength);

  double tw = plot_->pixelToWindowWidth(ptw);

  //---

  double xm = plot_->getCalcDataRange().xmid();

  double tx = (rect().getXMid() < xm - tw ?
    prect.getXMax() + textMargin : prect.getXMin() - textMargin - ptw);
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot_->pixelToWindow(Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = Angle();
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();
  options.align         = Qt::AlignLeft;
  options.clipLength    = clipLength;
  options.clipElide     = plot_->textClipElide();

  if (shapeType() == ShapeType::DIAMOND || shapeType() == ShapeType::BOX ||
      shapeType() == ShapeType::POLYGON || shapeType() == ShapeType::CIRCLE ||
      shapeType() == ShapeType::DOUBLE_CIRCLE) {
    options.align = Qt::AlignHCenter | Qt::AlignVCenter;

    CQChartsDrawUtil::drawTextInBox(device, rect(), str, options);
  }
  else {
    options.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, pt, str, options);
  }
}

void
CQChartsGraphNodeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto ic = calcColorInd();

  QColor bc;

  if (node()->strokeColor().isValid())
    bc = plot_->interpColor(node()->strokeColor(), ic);
  else
    bc = plot_->interpNodeStrokeColor(ic);

  auto fc = calcFillColor();

  auto fillAlpha   = (node()->fillAlpha  ().isValid() ?
    node()->fillAlpha() : plot_->nodeFillAlpha());
  auto fillPattern = (node()->fillPattern().isValid() ?
    node()->fillPattern() : plot_->nodeFillPattern());

  auto strokeAlpha = (node()->strokeAlpha().isValid() ?
    node()->strokeAlpha() : plot_->nodeStrokeAlpha());
  auto strokeWidth = (node()->strokeWidth().isValid() ?
    node()->strokeWidth() : plot_->nodeStrokeWidth());
  auto strokeDash  = (node()->strokeDash ().isValid() ?
    node()->strokeDash () : plot_->nodeStrokeDash());

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isNodeStroked(), bc, strokeAlpha, strokeWidth, strokeDash),
    BrushData(plot_->isNodeFilled (), fc, fillAlpha, fillPattern));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsGraphNodeObj::
calcFillColor() const
{
  QColor fc;

  auto ic = calcColorInd();

  if (fillColor().isValid())
    fc = plot_->interpColor(fillColor(), ic);
  else
    fc = plot_->interpNodeFillColor(ic);

  return fc;
}

void
CQChartsGraphNodeObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

//------

CQChartsGraphEdgeObj::
CQChartsGraphEdgeObj(const Plot *plot, const BBox &rect, Edge *edge) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), edge_(edge)
{
  //setDetailHint(DetailHint::MAJOR);
}

CQChartsGraphEdgeObj::
~CQChartsGraphEdgeObj()
{
  if (edge_)
    edge_->setObj(nullptr);
}

//---

CQChartsGraphEdgeObj::ShapeType
CQChartsGraphEdgeObj::
shapeType() const
{
  return (CQChartsGraphEdgeObj::ShapeType) edge()->shapeType();
}

void
CQChartsGraphEdgeObj::
setShapeType(const ShapeType &s)
{
  edge()->setShapeType((CQChartsGraphPlotEdge::ShapeType) s);
}

//---

QString
CQChartsGraphEdgeObj::
calcId() const
{
  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  return QString("%1:%2:%3:%4").arg(typeName()).
           arg(srcObj->calcId()).arg(destObj->calcId()).arg(edge()->id());
}

QString
CQChartsGraphEdgeObj::
calcTipId() const
{
  auto namedColumn = [&](const QString &name, const QString &defName="") {
    if (edge()->hasNamedColumn(name))
      return plot_->columnHeaderName(edge()->namedColumn(name));

    QString headerName = (defName.length() ? defName : name);

    return headerName;
  };

  //---

  CQChartsTableTip tableTip;

  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  auto srcName  = srcObj ->hierName();
  auto destName = destObj->hierName();

  if (srcName  == "") srcName  = srcObj ->id();
  if (destName == "") destName = destObj->id();

  //---

  tableTip.addTableRow(namedColumn("From"), srcName);
  tableTip.addTableRow(namedColumn("To"  ), destName);

  if (edge()->hasValue())
    tableTip.addTableRow(namedColumn("Value"), edge()->value().real());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsGraphEdgeObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "shapeType")->setDesc("Shape");
}

//---

bool
CQChartsGraphEdgeObj::
inside(const Point &p) const
{
  return path_.contains(p.qpoint());
}

//---

CQChartsGraphEdgeObj::PlotObjs
CQChartsGraphEdgeObj::
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
CQChartsGraphEdgeObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  device->setColorNames();

  //---

  // get connection rect of source and destination object
  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  bool isSelf = (srcObj == destObj);

  auto srcRect  = srcObj ->node()->destEdgeRect(edge());
  auto destRect = destObj->node()->srcEdgeRect (edge());

  if (! srcRect.isSet())
    srcRect = edge()->srcNode()->rect();

  if (! destRect.isSet())
    destRect = edge()->destNode()->rect();

  if (shapeType() == ShapeType::ARROW || ! plot_->isEdgeScaled()) {
    if (edge()->srcNode()->shapeType() == Node::ShapeType::DIAMOND ||
        edge()->srcNode()->shapeType() == Node::ShapeType::POLYGON ||
        edge()->srcNode()->shapeType() == Node::ShapeType::CIRCLE ||
        edge()->srcNode()->shapeType() == Node::ShapeType::DOUBLE_CIRCLE) {
      srcRect = srcObj->rect();
    }

    if (edge()->destNode()->shapeType() == Node::ShapeType::DIAMOND ||
        edge()->destNode()->shapeType() == Node::ShapeType::POLYGON ||
        edge()->destNode()->shapeType() == Node::ShapeType::CIRCLE ||
        edge()->destNode()->shapeType() == Node::ShapeType::DOUBLE_CIRCLE) {
      destRect = destObj->rect();
    }
  }

  if (! srcRect.isSet() || ! destRect.isSet())
    return;

  //---

  // x from right of source rect to left of dest rect
  bool swapped = false;

  double x1 = srcRect.getXMax(), x2 = destRect.getXMin();

  if (x1 > x2) {
    x1 = destRect.getXMax(), x2 = srcRect.getXMin();
    swapped = true;
  }

  //---

  // draw edge
  path_ = QPainterPath();

  if (shapeType() == ShapeType::ARROW) {
    double y1 = srcRect .getYMid();
    double y2 = destRect.getYMid();

    if (swapped)
      std::swap(y1, y2);

    const_cast<CQChartsGraphPlot *>(plot())->setUpdatesEnabled(false);

    if (! isSelf) {
      CQChartsArrow arrow(const_cast<CQChartsGraphPlot *>(plot()), Point(x1, y1), Point(x2, y2));

      arrow.setRectilinear (true);
      arrow.setLineWidth   (Length(8, CQChartsUnits::PIXEL));
      arrow.setFrontVisible(false);
      arrow.setFilled      (true);
      arrow.setFillColor   (penBrush.brush.color());
      arrow.setStroked     (true);
      arrow.setStrokeColor (penBrush.pen.color());

      arrow.draw(device);

      path_ = arrow.drawnPath();
    }
    else {
      CQChartsArrowData arrowData;

      arrowData.setFHead(true);
      arrowData.setTHead(true);
      arrowData.setLineWidth(Length(8, CQChartsUnits::PIXEL));

      double xr = srcRect.getWidth ()/2.0;
      double yr = srcRect.getHeight()/2.0;

      double a = M_PI/4.0;

      double c = std::cos(a);
      double s = std::sin(a);

      double xm = srcRect.getXMid();
      double ym = srcRect.getYMid();

      double yt = srcRect.getYMax() + yr/2.0;

      double x1 = xm - xr*c, y1 = ym + xr*s;
      double x2 = xm + xr*c, y2 = y1;

      QPainterPath path;

      path.moveTo (QPointF(x1, y1));
      path.cubicTo(QPointF(x1, yt), QPointF(x2, yt), QPointF(x2, y2));

      //---

      CQChartsArrow::pathAddArrows(const_cast<CQChartsGraphPlot *>(plot()), path, arrowData, path_);

      device->drawPath(path_);
    }

    const_cast<CQChartsGraphPlot *>(plot())->setUpdatesEnabled(true);
  }
  else {
    if (plot_->isEdgeScaled()) {
      // start y range from source node, and end y range from dest node
      double y11 = srcRect .getYMax(), y12 = srcRect .getYMin();
      double y21 = destRect.getYMax(), y22 = destRect.getYMin();

      if (swapped) {
        std::swap(y11, y21);
        std::swap(y12, y22);
      }

      // curve control point x at 1/3 and 2/3
      double x3 = CMathUtil::lerp(1.0/3.0, x1, x2);
      double x4 = CMathUtil::lerp(2.0/3.0, x1, x2);

      path_.moveTo (QPointF(x1, y11));
      path_.cubicTo(QPointF(x3, y11), QPointF(x4, y21), QPointF(x2, y21));
      path_.lineTo (QPointF(x2, y22));
      path_.cubicTo(QPointF(x4, y22), QPointF(x3, y12), QPointF(x1, y12));

      path_.closeSubpath();

      //---

      device->drawPath(path_);
    }
    else {
      double lw = plot_->pixelToWindowWidth(8);

      if (! isSelf) {
        // start y range from source node, and end y range from dest node
        double y1 = srcRect .getYMid();
        double y2 = destRect.getYMid();

        double y11 = y1 + lw/2.0, y12 = y1 - lw/2.0;
        double y21 = y2 + lw/2.0, y22 = y2 - lw/2.0;

        if (swapped) {
          std::swap(y11, y21);
          std::swap(y12, y22);
        }

        // curve control point x at 1/3 and 2/3
        double x3 = CMathUtil::lerp(1.0/3.0, x1, x2);
        double x4 = CMathUtil::lerp(2.0/3.0, x1, x2);

        path_.moveTo (QPointF(x1, y11));
        path_.cubicTo(QPointF(x3, y11), QPointF(x4, y21), QPointF(x2, y21));
        path_.lineTo (QPointF(x2, y22));
        path_.cubicTo(QPointF(x4, y22), QPointF(x3, y12), QPointF(x1, y12));

        path_.closeSubpath();
      }
      else {
        double xr = srcRect.getWidth ()/2.0;
        double yr = srcRect.getHeight()/2.0;

        double a = M_PI/4.0;

        double c = std::cos(a);
        double s = std::sin(a);

        double xm = srcRect.getXMid();
        double ym = srcRect.getYMid();

        double yt = srcRect.getYMax() + yr/2.0;
        double yt1 = yt - lw/2.0;
        double yt2 = yt + lw/2.0;

        double x1 = xm - xr*c, y1 = ym + xr*s;
        double x2 = xm + xr*c, y2 = y1;

        double lw1 = sqrt(2)*lw/2.0;

        path_.moveTo (QPointF(x1 - lw1, y1 - lw1));
        path_.cubicTo(QPointF(x1 - lw1, yt2), QPointF(x2 + lw1, yt2), QPointF(x2 + lw1, y2 - lw1));
        path_.lineTo (QPointF(x2 - lw1, y2 + lw1));
        path_.cubicTo(QPointF(x2 - lw1, yt1), QPointF(x1 + lw1, yt1), QPointF(x1 + lw1, y1 + lw1));

        path_.closeSubpath();
      }

      //---

      device->drawPath(path_);
    }
  }

  device->resetColorNames();
}

void
CQChartsGraphEdgeObj::
drawFg(PaintDevice *device) const
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
  auto ic = calcColorInd();

  PenBrush penBrush;

  auto c = plot_->interpTextColor(ic);

  plot_->setPen(penBrush, PenData(true, c, plot_->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  auto str = edge()->label();

  if (! str.length())
    return;

  double ptw = fm.width(str);

  double tx = prect.getXMid() - textMargin - ptw/2.0;
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot_->pixelToWindow(Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = Angle();
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();
  options.align         = Qt::AlignLeft;
  options.clipLength    = plot_->lengthPixelWidth(plot_->textClipLength());
  options.clipElide     = plot_->textClipElide();

  CQChartsDrawUtil::drawTextAtPoint(device, pt, str, options);
}

void
CQChartsGraphEdgeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto *srcNode  = edge()->srcNode ();
  auto *destNode = edge()->destNode();

  //---

  ColorInd colorInd;

  //---

  // calc fill color
  QColor fc;

  if (! edge()->fillColor().isValid()) {
    if (plot()->isBlendEdgeColor()) {
      auto fc1 = srcNode ->obj()->calcFillColor();
      auto fc2 = destNode->obj()->calcFillColor();

      fc = CQChartsUtil::blendColors(fc1, fc2, 0.5);
    }
    else
      fc = plot()->interpEdgeFillColor(colorInd);
  }
  else {
    fc = plot()->interpColor(edge()->fillColor(), colorInd);
  }

  //---

  // calc stroke color
  QColor sc;

  if (plot()->isBlendEdgeColor()) {
    int numNodes = plot()->numNodes();

    ColorInd ic1(srcNode ->id(), numNodes);
    ColorInd ic2(destNode->id(), numNodes);

    auto sc1 = plot()->interpEdgeStrokeColor(ic1);
    auto sc2 = plot()->interpEdgeStrokeColor(ic2);

    sc = CQChartsUtil::blendColors(sc1, sc2, 0.5);
  }
  else {
    sc = plot()->interpEdgeStrokeColor(colorInd);
  }

  //---

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isEdgeStroked(), sc, plot_->edgeStrokeAlpha(),
              plot_->edgeStrokeWidth(), plot_->edgeStrokeDash()),
    BrushData(plot_->isEdgeFilled(), fc, plot_->edgeFillAlpha(),
              plot_->edgeFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsGraphEdgeObj::
writeScriptData(ScriptPaintDevice *device) const
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

CQChartsGraphGraphObj::
CQChartsGraphGraphObj(const Plot *plot, const BBox &rect, Graph *graph) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), graph_(graph)
{
  setDetailHint(DetailHint::MAJOR);

  setEditable  (false);
  setSelectable(false);
}

CQChartsGraphGraphObj::
~CQChartsGraphGraphObj()
{
  if (graph_)
    graph_->setObj(nullptr);
}

QString
CQChartsGraphGraphObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(graph_->id());
}

QString
CQChartsGraphGraphObj::
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
CQChartsGraphGraphObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);
}

//---

void
CQChartsGraphGraphObj::
moveBy(const Point &delta)
{
  //std::cerr << "  Move " << node()->str().toStdString() << " by " << delta.y << "\n";

  rect_.moveBy(delta);
}

void
CQChartsGraphGraphObj::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);
}

//---

bool
CQChartsGraphGraphObj::
editPress(const Point &p)
{
  editChanged_ = false;

  editHandles()->setDragPos(p);

  return true;
}

bool
CQChartsGraphGraphObj::
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
CQChartsGraphGraphObj::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

bool
CQChartsGraphGraphObj::
editRelease(const Point &)
{
  if (editChanged_)
    plot()->invalidateObjTree();

  return true;
}

void
CQChartsGraphGraphObj::
setEditBBox(const BBox &bbox, const CQChartsResizeSide &)
{
  graph_->setRect(bbox);
}

//---

void
CQChartsGraphGraphObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

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
CQChartsGraphGraphObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto ic = calcColorInd();

  auto bc = plot_->interpGraphStrokeColor(ic);
  auto fc = plot_->interpGraphFillColor  (ic);

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isGraphStroked(), bc, plot_->graphStrokeAlpha(),
              plot_->graphStrokeWidth(), plot_->graphStrokeDash()),
    BrushData(plot_->isGraphFilled(), fc, plot_->graphFillAlpha(),
              plot_->graphFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsGraphGraphObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

//------

CQChartsGraphPlotGraph::
CQChartsGraphPlotGraph(const Plot *plot, const QString &str) :
 CQChartsGraphPlotNode(plot, str)
{
  shapeType_ = ShapeType::BOX;
}

CQChartsGraphPlotGraph::
~CQChartsGraphPlotGraph()
{
  if (obj_)
    obj_->setGraph(nullptr);
}

void
CQChartsGraphPlotGraph::
addChild(Graph *graph)
{
  children_.push_back(graph);
}

void
CQChartsGraphPlotGraph::
addNode(Node *node)
{
  srcDepth_  = -1;
  destDepth_ = -1;

  nodes_.push_back(node);
}

void
CQChartsGraphPlotGraph::
removeAllNodes()
{
  srcDepth_  = -1;
  destDepth_ = -1;

  nodes_.clear();
}

//---

void
CQChartsGraphPlotGraph::
setObj(Obj *obj)
{
  obj_ = obj;
}

//---

void
CQChartsGraphPlotGraph::
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
CQChartsGraphPlotGraph::
updateRect()
{
  BBox bbox;

  for (const auto &child : children())
    bbox += child->rect();

  for (const auto &node : nodes())
    bbox += node->rect().getCenter();

  rect_ = bbox;

  if (obj_)
    obj_->setRect(rect_);
}

//---

int
CQChartsGraphPlotGraph::
srcDepth() const
{
  if (srcDepth_ < 0) {
    auto *th = const_cast<CQChartsGraphPlotGraph *>(this);

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
CQChartsGraphPlotGraph::
destDepth() const
{
  if (destDepth_ < 0) {
    auto *th = const_cast<CQChartsGraphPlotGraph *>(this);

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

CQChartsGraphPlotGraph::Nodes
CQChartsGraphPlotGraph::
placeNodes() const
{
  Nodes nodes;

  for (auto &child : this->children())
    nodes.push_back(child);

  for (auto &node : this->nodes())
    nodes.push_back(node);

  return nodes;
}

void
CQChartsGraphPlotGraph::
place() const
{
  if (isPlaced())
    return;

  auto *th = const_cast<CQChartsGraphPlotGraph *>(this);

  plot_->placeGraph(th);
}

void
CQChartsGraphPlotGraph::
moveBy(const Point &delta)
{
  Node::moveBy(delta);

  for (const auto &child : children())
    child->moveBy(delta);

  for (const auto &node : nodes())
    node->moveBy(delta);

  //---

  if (obj_)
    obj_->moveBy(delta);
}

void
CQChartsGraphPlotGraph::
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

  //---

  if (obj_)
    obj_->setRect(rect_);
}
