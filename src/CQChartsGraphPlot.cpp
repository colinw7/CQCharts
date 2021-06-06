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

#include <QMenu>
#include <QAction>

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
  graphMgr_ = std::make_unique<CQChartsGraphPlotMgr>(this);
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

  addColorMapKey();

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
  graphMgr_->clearNodesAndEdges();

  nodes_.clear();
  edges_.clear();

  clearGraphs();

  groupValueInd_.clear();

  maxNodeDepth_ = 0;
}

//---

const CQChartsLength &
CQChartsGraphPlot::
nodeXMargin() const
{
  return graphMgr_->nodeXMargin();
}

void
CQChartsGraphPlot::
setNodeXMargin(const Length &l)
{
  if (l != nodeXMargin()) {
    graphMgr_->setNodeXMargin(l);

    updateRangeAndObjs();
  }
}

const CQChartsLength &
CQChartsGraphPlot::
nodeYMargin() const
{
  return graphMgr_->nodeYMargin();
}

void
CQChartsGraphPlot::
setNodeYMargin(const Length &l)
{
  if (l != nodeYMargin()) {
    graphMgr_->setNodeYMargin(l);

    updateRangeAndObjs();
  }
}

//---

const CQChartsLength &
CQChartsGraphPlot::
nodeWidth() const
{
  return graphMgr_->nodeWidth();
}

void
CQChartsGraphPlot::
setNodeWidth(const Length &l)
{
  if (l != nodeWidth()) {
    graphMgr_->setNodeWidth(l);

    updateRangeAndObjs();
  }
}

bool
CQChartsGraphPlot::
isNodeXScaled() const
{
  return graphMgr_->isNodeXScaled();
}

void
CQChartsGraphPlot::
setNodeXScaled(bool b)
{
  if (isNodeXScaled() != b) {
    graphMgr_->setNodeXScaled(b);

    updateRangeAndObjs();
  }
}

CQChartsGraphPlot::NodeShape
CQChartsGraphPlot::
nodeShape() const
{
  return (NodeShape) graphMgr_->nodeShape();
}

void
CQChartsGraphPlot::
setNodeShape(const NodeShape &s)
{
  if (s != nodeShape()) {
    graphMgr_->setNodeShape((GraphMgr::NodeShape) s);

    updateRangeAndObjs();
  }
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

void
CQChartsGraphPlot::
setEdgeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeWidth_, l, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGraphPlot::
setBlendEdgeColor(bool b)
{
  CQChartsUtil::testAndSet(blendEdgeColor_, b, [&]() { drawObjs(); } );
}

//---

CQChartsGraphPlot::Align
CQChartsGraphPlot::
align() const
{
  return (CQChartsGraphPlot::Align) graphMgr_->align();
}

void
CQChartsGraphPlot::
setAlign(const Align &a)
{
  if (a != align()) {
    graphMgr_->setAlign((CQChartsGraphMgr::Align) a);

    updateRangeAndObjs();
  }
}

//---

bool
CQChartsGraphPlot::
isAdjustNodes() const
{
  return graphMgr_->isAdjustNodes();
}

void
CQChartsGraphPlot::
setAdjustNodes(bool b)
{
  if (b != isAdjustNodes()) {
    graphMgr_->setAdjustNodes(b);

    updateRangeAndObjs();
  }
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

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
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

  for (auto *node : nodes_) {
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
  auto columnDataType = calcColumnDataType();

  bool rc = false;

  if      (columnDataType == ColumnDataType::HIER)
    rc = initHierObjs();
  else if (columnDataType == ColumnDataType::LINK)
    rc = initLinkObjs();
  else if (columnDataType == ColumnDataType::CONNECTIONS)
    rc = initConnectionObjs();
  else if (columnDataType == ColumnDataType::PATH)
    rc = initPathObjs();
  else if (columnDataType == ColumnDataType::FROM_TO)
    rc = initFromToObjs();
  else if (columnDataType == ColumnDataType::TABLE)
    rc = initTableObjs();

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

  for (auto *node : nodes_) {
    if (! node->isVisible()) continue;

    double nx1 = x2 + (node->rect().getXMin() - x1)*f;
    double ny1 = y2 + (node->rect().getYMin() - y1)*f;
  //double nx2 = x2 + (node->rect().getXMax() - x1)*f;
  //double ny2 = y2 + (node->rect().getYMax() - y1)*f;

    node->scale(f, f);

    node->moveBy(Point(nx1 - node->rect().getXMin(), ny1 - node->rect().getYMin()));
  }

  //---

  for (auto &pg : graphMgr_->graphs()) {
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

  auto separator = calcSeparator();

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
    for (const auto &node : nodes_) {
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
addConnectionObj(int id, const ConnectionsData &connectionsData, const NodeIndex &) const
{
  // get src node
  auto srcStr = QString::number(id);

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
    auto destStr = QString::number(connection.node);

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

    auto srcStr = QString::number(tableConnectionData.from());

    auto *srcNode = findNode(srcStr);

    srcNode->setName (tableConnectionData.name());
    srcNode->setGroup(tableConnectionData.group().ig, tableConnectionData.group().ng);

    for (const auto &value : tableConnectionData.values()) {
      auto destStr = QString::number(value.to);

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
    for (const auto &node : nodes_) {
      if (node->depth() > maxDepth())
        node->setVisible(false);
    }
  }

  // hide nodes less than min value
  if (minValue() > 0) {
    for (const auto &node : nodes_) {
      if (! node->value().isSet() || node->value().real() < minValue())
        node->setVisible(false);
    }
  }
}

//---

bool
CQChartsGraphPlot::
addMenuItems(QMenu *menu)
{
  bool added = false;

  if (canDrawColorMapKey()) {
    addColorMapKeyItems(menu);

    added = true;
  }

  return added;
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
  for (auto &pg : graphMgr_->graphs()) {
    auto *graph = dynamic_cast<CQChartsGraphPlotGraph *>(pg.second);
    assert(graph);

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

    if (edgeObj)
      objs.push_back(edgeObj);
  }
}

// place graphs
void
CQChartsGraphPlot::
placeGraphs() const
{
  for (auto &pg : graphMgr_->graphs()) {
    auto *graph = pg.second;

    graph->placeGraph(bbox_);
  }
}

void
CQChartsGraphPlot::
createGraphs() const
{
  auto *th = const_cast<CQChartsGraphPlot *>(this);

  // Add nodes to graph for group
  for (auto *node : nodes_) {
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

  if (graphMgr_->graphs().size() == 1 && isAutoCreateGraphs()) {
    autoCreateGraphs();
  }
}

void
CQChartsGraphPlot::
autoCreateGraphs() const
{
  Nodes noSrcNodes;

  for (auto *node : nodes_) {
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

  auto *root = graphMgr_->graphs().begin()->second;

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

CQChartsGraphGraph *
CQChartsGraphPlot::
getGraph(int graphId, int parentGraphId) const
{
  return graphMgr_->getOrCreateGraph(graphId, parentGraphId);
}

void
CQChartsGraphPlot::
clearGraphs()
{
  graphMgr_->clearGraphs();
}

CQChartsGraphPlot::NodeObj *
CQChartsGraphPlot::
createObjFromNode(Graph *, Node *node) const
{
//int numNodes = graph->nodes().size(); // node id needs to be per graph
  int numNodes = this->numNodes();

  ColorInd iv(node->id(), numNodes);

  auto *nodeObj = createNodeObj(node->rect(), node, iv);

  NodeObj::ShapeType shapeType = (NodeObj::ShapeType) node->shapeType();

  if (shapeType == NodeObj::ShapeType::NONE)
    shapeType = (NodeObj::ShapeType) nodeShape();

  nodeObj->setShapeType(shapeType);
  nodeObj->setHierName (node->str());

  for (const auto &modelInd : node->modelInds())
    nodeObj->addModelInd(normalizedModelIndex(modelInd));

  auto *pnode = dynamic_cast<CQChartsGraphPlotNode *>(node);
  assert(pnode);

  pnode->setObj(nodeObj);

  return nodeObj;
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

  if (! nodeRect.isValid())
    return nullptr;

  BBox rect(nodeRect.getXMin() - xm, nodeRect.getYMin() - ym,
            nodeRect.getXMax() + xm, nodeRect.getYMax() + ym);

  auto *edgeObj = createEdgeObj(rect, edge);

  EdgeObj::ShapeType shapeType = (EdgeObj::ShapeType) edge->shapeType();

  if (shapeType == EdgeObj::ShapeType::NONE)
    shapeType = (EdgeObj::ShapeType) edgeShape();

  edgeObj->setShapeType(shapeType);

  auto *pedge = dynamic_cast<CQChartsGraphPlotEdge *>(edge);
  assert(pedge);

  pedge->setObj(edgeObj);

  return edgeObj;
}

bool
CQChartsGraphPlot::
adjustNodes() const
{
  bool changed = false;

  for (auto &pg : graphMgr_->graphs()) {
    auto *graph = pg.second;

    auto nodes = graph->placeableNodes();

    if (graph->adjustGraphNodes(nodes))
      changed = true;
  }

  return changed;
}

//---

#if 0
void
CQChartsGraphPlot::
adjustGraphs() const
{
  if (graphMgr_->graphs().size() <= 1)
    return;

  double h = 0;

  for (auto &pg : graphMgr_->graphs()) {
    auto *graph = pg.second;

    h += graph->rect().getHeight();
  }

  double f = (h > 0.0 ? 2.0/h : 1.0);

  double y = -1.0;

  for (auto &pg : graphMgr_->graphs()) {
    auto *graph = pg.second;

    double dy = y - f*graph->rect().getYMin();

    graph->moveBy(Point(0, dy));
    graph->scale (f, f);

    y += graph->rect().getHeight();
  }
}
#endif

//---

CQChartsGraphNode *
CQChartsGraphPlot::
findNode(const QString &name) const
{
  auto *node = graphMgr_->findNode(name);

  if (! node)
    node = createNode(name);

  return node;
}

CQChartsGraphNode *
CQChartsGraphPlot::
createNode(const QString &name) const
{
  auto *node = graphMgr_->addNode(name);

  auto *th = const_cast<CQChartsGraphPlot *>(this);

  th->nodes_.push_back(node);

  return node;
}

CQChartsGraphEdge *
CQChartsGraphPlot::
createEdge(const OptReal &value, Node *srcNode, Node *destNode) const
{
  auto *edge = graphMgr_->addEdge(value, srcNode, destNode);

  auto *th = const_cast<CQChartsGraphPlot *>(this);

  th->edges_.push_back(edge);

  return edge;
}

//---

bool
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
  else {
    return CQChartsPlot::keyPress(key, modifier);
  }

  return true;
}

void
CQChartsGraphPlot::
printStats()
{
  using NameData = std::map<QString, QString>;
  using NodeData = std::map<Node *, NameData>;

  NodeData nodeData;

  for (auto *node : nodes_) {
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

//---

bool
CQChartsGraphPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsGraphPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

CQChartsPlotCustomControls *
CQChartsGraphPlot::
createCustomControls()
{
  auto *controls = new CQChartsGraphPlotCustomControls(charts());

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
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
  auto *pnode = dynamic_cast<CQChartsGraphPlotNode *>(node_);

  if (pnode)
    pnode->setObj(nullptr);
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
  node()->setValue(CQChartsGraphNode::OptReal(r));
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
  node()->setShapeType((CQChartsGraphNode::ShapeType) s);
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

    auto headerName = (defName.length() ? defName : name);

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

  if (dragSide != CQChartsResizeSide::MOVE)
    return false;

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

  for (auto &edgeRect : node()->srcEdgeRects()) {
    auto *pedge = dynamic_cast<CQChartsGraphPlotEdge *>(edgeRect.first);
    assert(pedge);

    plotObjs.push_back(pedge->obj());
  }

  for (auto &edgeRect : node()->destEdgeRects()) {
    auto *pedge = dynamic_cast<CQChartsGraphPlotEdge *>(edgeRect.first);
    assert(pedge);

    plotObjs.push_back(pedge->obj());
  }

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

  auto range = plot_->getCalcDataRange();

  double xm = (range.isSet() ? range.xmid() : 0.0);

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
  auto *pedge = dynamic_cast<CQChartsGraphPlotEdge *>(edge_);

  if (pedge)
    pedge->setObj(nullptr);
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
  edge()->setShapeType((CQChartsGraphEdge::ShapeType) s);
}

//---

QString
CQChartsGraphEdgeObj::
calcId() const
{
  auto *srcNode  = dynamic_cast<CQChartsGraphPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

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

    auto headerName = (defName.length() ? defName : name);

    return headerName;
  };

  //---

  CQChartsTableTip tableTip;

  auto *srcNode  = dynamic_cast<CQChartsGraphPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

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

  auto *srcNode  = dynamic_cast<CQChartsGraphPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

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
  auto *srcNode  = dynamic_cast<CQChartsGraphPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

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
  double x1 = srcRect.getXMax(), x2 = destRect.getXMin();

  bool swapped = false;

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

    double lw = plot_->lengthPlotHeight(plot()->edgeWidth());

    if (! isSelf) {
#if 0
      CQChartsArrow arrow(const_cast<CQChartsGraphPlot *>(plot()), Point(x1, y1), Point(x2, y2));

      arrow.setRectilinear (true);
      arrow.setLineWidth   (plot()->edgeWidth());
      arrow.setFrontVisible(false);
      arrow.setFilled      (true);
      arrow.setFillColor   (penBrush.brush.color());
      arrow.setStroked     (true);
      arrow.setStrokeColor (penBrush.pen.color());

      arrow.draw(device);

      path_ = arrow.drawnPath();
#else
      QPainterPath lpath;

      CQChartsDrawUtil::curvePath(lpath, srcRect, destRect, /*rectilinear*/true);

      CQChartsArrowData arrowData;

      arrowData.setFHeadType(CQChartsArrowData::HeadType::ARROW);
      arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);

      CQChartsArrow::pathAddArrows(lpath, arrowData, lw, 1.0, path_);

      device->drawPath(path_);
#endif
    }
    else {
      CQChartsArrow::selfPath(path_, srcRect, /*fhead*/true, /*thead*/true, lw);

      device->drawPath(path_);
    }

    const_cast<CQChartsGraphPlot *>(plot())->setUpdatesEnabled(true);
  }
  else {
    if (plot_->isEdgeScaled()) {
      CQChartsDrawUtil::edgePath(path_, srcRect, destRect, /*isLine*/false);
    }
    else {
      double lw = plot_->lengthPlotHeight(plot()->edgeWidth()); // TODO: config

      if (! isSelf) {
        // start y range from source node, and end y range from dest node
        double y1 = srcRect .getYMid();
        double y2 = destRect.getYMid();

        if (swapped)
          std::swap(y1, y2);

        CQChartsDrawUtil::edgePath(path_, Point(x1, y1), Point(x2, y2), lw);
      }
      else {
        CQChartsDrawUtil::selfEdgePath(path_, srcRect, lw);
      }
    }

    device->drawPath(path_);
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
  auto *srcNode  = dynamic_cast<CQChartsGraphPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

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
  auto *srcNode  = dynamic_cast<CQChartsGraphPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

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
    int numNodes = plot_->numNodes();

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
  auto *pgraph = dynamic_cast<CQChartsGraphPlotGraph *>(graph_);

  if (pgraph)
    pgraph->setObj(nullptr);
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

CQChartsGraphPlotCustomControls::
CQChartsGraphPlotCustomControls(CQCharts *charts) :
 CQChartsConnectionPlotCustomControls(charts, "graph")
{
  addConnectionColumnWidgets();

  addColorColumnWidgets("Cell Color");

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsGraphPlotCustomControls::
connectSlots(bool b)
{
  CQChartsConnectionPlotCustomControls::connectSlots(b);
}

void
CQChartsGraphPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsGraphPlot *>(plot);

  CQChartsConnectionPlotCustomControls::setPlot(plot);
}

void
CQChartsGraphPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsConnectionPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

CQChartsColor
CQChartsGraphPlotCustomControls::
getColorValue()
{
  return plot_->nodeFillColor();
}

void
CQChartsGraphPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setNodeFillColor(c);
}

//---

CQChartsGraphPlotMgr::
CQChartsGraphPlotMgr(CQChartsGraphPlot *plot) :
 CQChartsGraphMgr(plot), plot_(plot)
{
}

CQChartsGraphMgr::Graph *
CQChartsGraphPlotMgr::
createGraph(const QString &name) const
{
  auto *th = const_cast<CQChartsGraphPlotMgr *>(this);

  return new CQChartsGraphPlotGraph(th, name);
}

CQChartsGraphMgr::Node *
CQChartsGraphPlotMgr::
createNode(const QString &name) const
{
  auto *th = const_cast<CQChartsGraphPlotMgr *>(this);

  return new CQChartsGraphPlotNode(th, name);
}

CQChartsGraphMgr::Edge *
CQChartsGraphPlotMgr::
createEdge(const OptReal &value, Node *srcNode, Node *destNode) const
{
  auto *th = const_cast<CQChartsGraphPlotMgr *>(this);

  return new CQChartsGraphPlotEdge(th, value, srcNode, destNode);
}

//---

CQChartsGraphPlotGraph::
CQChartsGraphPlotGraph(GraphMgr *mgr, const QString &name) :
 CQChartsGraphGraph(mgr, name)
{
}

CQChartsGraphPlotGraph::
~CQChartsGraphPlotGraph()
{
  if (obj_)
    obj_->setGraph(nullptr);
}

void
CQChartsGraphPlotGraph::
setObj(Obj *obj)
{
  obj_ = obj;
}

void
CQChartsGraphPlotGraph::
updateRect()
{
  CQChartsGraphGraph::updateRect();

  if (obj_)
    obj_->setRect(rect_);
}

void
CQChartsGraphPlotGraph::
moveBy(const Point &delta)
{
  CQChartsGraphGraph::moveBy(delta);

  if (obj_)
    obj_->moveBy(delta);
}

void
CQChartsGraphPlotGraph::
scale(double fx, double fy)
{
  CQChartsGraphGraph::scale(fx, fy);

  if (obj_)
    obj_->setRect(rect_);
}

//---

CQChartsGraphPlotNode::
CQChartsGraphPlotNode(GraphMgr *mgr, const QString &name) :
 CQChartsGraphNode(mgr, name)
{
}

CQChartsGraphPlotNode::
~CQChartsGraphPlotNode()
{
  assert(! obj_);
}

void
CQChartsGraphPlotNode::
setObj(Obj *obj)
{
  obj_ = obj;
}

const CQChartsGraphNode::BBox &
CQChartsGraphPlotNode::
rect() const
{
  if (obj_) {
    assert(obj_->rect() == rect_);
  }

  return CQChartsGraphNode::rect();
}

void
CQChartsGraphPlotNode::
setRect(const BBox &rect)
{
  CQChartsGraphNode::setRect(rect);

  if (obj_) // TODO: assert null or use move by
    obj_->setRect(rect);
}

void
CQChartsGraphPlotNode::
moveBy(const Point &delta)
{
  CQChartsGraphNode::moveBy(delta);

  if (obj_)
    obj_->moveBy(delta);
}

void
CQChartsGraphPlotNode::
scale(double fx, double fy)
{
  CQChartsGraphNode::scale(fx, fy);

  if (obj_)
    obj_->scale(fx, fy);
}

//---

CQChartsGraphPlotEdge::
CQChartsGraphPlotEdge(GraphMgr *mgr, const OptReal &value, Node *srcNode, Node *destNode) :
 CQChartsGraphEdge(mgr, value, srcNode, destNode)
{
}

CQChartsGraphPlotEdge::
~CQChartsGraphPlotEdge()
{
  assert(! obj_);
}

void
CQChartsGraphPlotEdge::
setObj(Obj *obj)
{
  obj_ = obj;
}
