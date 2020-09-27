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
#include <CQChartsDrawUtil.h>
#include <CQChartsEditHandles.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsRand.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CMathRound.h>

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
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Sankey Plot").
    h3("Summary").
     p("Draw connected objects as a connected flow graph.").
    h3("Columns").
     p("Connections between two nodes (edge) are described using " + B("Node/Connections") +
       " columns, a " + B("Link") + " column, a " + B("Path") + " column or " + B("From/To") +
       " columns").
     p("The connection size can be specified using the " + B("Value") + " column.").
     p("The depth (x postion) can be specified using the " + B("Depth") + " column.").
     p("Extra attributes (color, label) for the source node, destination node or edge "
       "can be specified using the " + B("Attributes") + " column.").
     p("The depth (x postion) can be specified using the " + B("Depth") + " column.").
     p("The source node group can be specified using the " + B("Group") + " column.").
     p("The source node name can be specified using the " + B("Name") + " column.").
    h3("Options").
     p("The separator character for hierarchical names (Link or Path columns) can be "
       "specified using the " + B("separator") + " option.").
     p("The " + B("symmetric") + " option can be used to specify that the connection is "
       "symmetric (value of connection for from/to is the same as to/from)").
     p("The " + B("sorted") + " option can be used to sort the connections by value").
     p("The " + B("maxDepth") + " option can be used to filter out connections greater than "
       "the specified depth.").
     p("The " + B("minValue") + " option can be used filter out connections less than the "
       "specified value.").
    h3("Limitations").
     p("As each model row is an edge it is hard to specify data on just a node.").
     p("The From/To columns do allow the To column to be empty to define attributes "
       "on just the from node but this is not ideal as it makes the raw data hard to "
       "manipulate").
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
 CQChartsObjEdgeShapeData <CQChartsSankeyPlot>(this)
{
}

CQChartsSankeyPlot::
~CQChartsSankeyPlot()
{
  term();
}

//---

void
CQChartsSankeyPlot::
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

  addTitle();

  //---

  bbox_ = targetBBox_;

  setFitMargin(PlotMargin(Length("5%"), Length("5%"), Length("5%"), Length("5%")));
}

void
CQChartsSankeyPlot::
term()
{
  // delete objects first to ensure link from edge/node to object reset
  clearPlotObjects();

  clearNodesAndEdges();

  clearGraph();
}

//---

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

  clearGraph();

  maxNodeDepth_ = 0;
}

//---

void
CQChartsSankeyPlot::
setNodeMargin(double r)
{
  CQChartsUtil::testAndSet(nodeMargin_, r, [&]() { updateRangeAndObjs(); } );
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
setBlendEdgeColor(bool b)
{
  CQChartsUtil::testAndSet(blendEdgeColor_, b, [&]() { drawObjs(); } );
}

void
CQChartsSankeyPlot::
setAlign(const Align &a)
{
  CQChartsUtil::testAndSet(align_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setSpread(const Spread &s)
{
  CQChartsUtil::testAndSet(spread_, s, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsSankeyPlot::
setSortPathIdNodes(bool b)
{
  CQChartsUtil::testAndSet(sortPathIdNodes_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setSortPathIdEdges(bool b)
{
  CQChartsUtil::testAndSet(sortPathIdEdges_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setAdjustNodes(bool b)
{
  CQChartsUtil::testAndSet(adjustNodes_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setAdjustCenters(bool b)
{
  CQChartsUtil::testAndSet(adjustCenters_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setRemoveOverlaps(bool b)
{
  CQChartsUtil::testAndSet(removeOverlaps_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setReorderEdges(bool b)
{
  CQChartsUtil::testAndSet(reorderEdges_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setAdjustEdgeOverlaps(bool b)
{
  CQChartsUtil::testAndSet(adjustEdgeOverlaps_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setAdjustSelected(bool b)
{
  CQChartsUtil::testAndSet(adjustSelected_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setAdjustIterations(int n)
{
  CQChartsUtil::testAndSet(adjustIterations_, n, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setAdjustText(bool b)
{
  CQChartsUtil::testAndSet(adjustText_, b, [&]() { updateRangeAndObjs(); } );
}


//---

void
CQChartsSankeyPlot::
setUseMaxTotals(bool b)
{
  CQChartsUtil::testAndSet(useMaxTotals_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsSankeyPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc, bool hidden=false) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    item->setHidden(hidden);
    return item;
  };

  //---

  CQChartsConnectionPlot::addProperties();

  //---

  // placement
  addProp("placement", "sortPathIdNodes"   , "sortPathIdNodes"   , "Sort depth nodes by path id");
  addProp("placement", "sortPathIdEdges"   , "sortPathIdEdges"   , "Sort node edges by path id");
  addProp("placement", "adjustNodes"       , "adjustNodes"       , "Adjust node placement");
  addProp("placement", "adjustCenters"     , "adjustCenters"     , "Adjust node centers");
  addProp("placement", "removeOverlaps"    , "removeOverlaps"    , "Remove overlapping nodes");
  addProp("placement", "reorderEdges"      , "reorderEdges"      , "Reorder edges");
  addProp("placement", "adjustEdgeOverlaps", "adjustEdgeOverlaps", "Adjust edge overlaps");
  addProp("placement", "adjustSelected"    , "adjustSelected"    , "Adjust only selected");
  addProp("placement", "adjustIterations"  , "adjustIterations"  , "Adjust iterations");
  addProp("placement", "adjustText"        , "adjustText"        , "Adjust text placement");

  // options
  addProp("options", "align"        , "align"        , "Node alignment");
  addProp("options", "spread"       , "spread"       , "Node spread");
  addProp("options", "useMaxTotals" , "useMaxTotals" , "Use max of src/dest totals for edge scaling");

  // coloring
  addProp("coloring", "srcColoring"      , "", "Color by Source Nodes");
  addProp("coloring", "blendEdgeColor"   , "", "Blend Edge Node Colors");
  addProp("coloring", "mouseColoring"    , "", "Mouse Over Connection Coloring");
  addProp("coloring", "mouseNodeColoring", "", "Mouse Over Node Coloring");

  // node
  addProp("node", "nodeMargin", "margin", "Node margin (Y)");
  addProp("node", "nodeWidth" , "width" , "Node width (in pixels)");

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

  // text
  addProp("text", "textVisible"        , "visible"        , "Text label visible");
  addProp("text", "insideTextVisible"  , "insideVisible"  , "Inside text label visible");
  addProp("text", "selectedTextVisible", "selectedVisible", "Selected text label visible");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  //---

  // add color map properties
  addColorMapProperties();
}

//---

CQChartsGeom::Range
CQChartsSankeyPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::calcRange");

//auto *th = const_cast<CQChartsSankeyPlot *>(this);

  Range dataRange;

  auto *model = this->model().data();

  if (! model)
    return dataRange;

  //---

  dataRange.updateRange(bbox_.getLL());
  dataRange.updateRange(bbox_.getUR());

  //---

  return dataRange;
}

CQChartsGeom::Range
CQChartsSankeyPlot::
getCalcDataRange() const
{
  double xm = pixelToWindowWidth(nodeWidth());

  return Range(bbox_.getXMin() - xm/2.0, bbox_.getYMax(),
               bbox_.getXMax() + xm/2.0, bbox_.getYMin());
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

  // check columns
  if (! checkColumns())
    return false;

  //---

  // init objects
  th->clearNodesAndEdges();

  auto *model = this->model().data();
  if (! model) return false;

  //---

  pathIdMinMax_.reset();

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

void
CQChartsSankeyPlot::
preDrawFgObjs(CQChartsPaintDevice *) const
{
  if (! isAdjustText())
    return;

  for (const auto &drawText : drawTexts_)
    delete drawText;

  drawTexts_.clear();
}

void
CQChartsSankeyPlot::
postDrawFgObjs(CQChartsPaintDevice *device) const
{
  if (! isAdjustText())
    return;

  BBox rect = this->calcDataRect();

  CQChartsRectPlacer placer;

  placer.setClipRect(CQChartsRectPlacer::Rect(rect.getXMin(), rect.getYMin(),
                                              rect.getXMax(), rect.getYMax()));

  for (const auto &drawText : drawTexts_)
    placer.addRect(drawText);

  placer.place();

  for (const auto &drawText : drawTexts_) {
    PenBrush penBrush;

    setPen(penBrush, PenData(true, drawText->color, drawText->alpha));

    device->setPen(penBrush.pen);

    CQChartsDrawUtil::drawTextAtPoint(device, drawText->point, drawText->str, drawText->options);

    if (drawText->point != drawText->origPoint) {
      setPen(penBrush, PenData(true, drawText->color, Alpha(0.4)));

      device->setPen(penBrush.pen);

      auto bbox = CQChartsDrawUtil::calcTextAtPointRect(device, drawText->point, drawText->str,
                                                        drawText->options);

      auto p = CQChartsUtil::nearestRectPoint(bbox, drawText->targetPoint);

      device->drawLine(p, drawText->targetPoint);
    }
  }
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
addPathValue(const PathData &pathData) const
{
  int n = pathData.pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  QChar separator = (this->separator().length() ? this->separator()[0] : '/');

  QString path1 = pathData.pathStrs[0];

  for (int i = 1; i < n; ++i) {
    QString path2 = path1 + separator + pathData.pathStrs[i];

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
      auto edgeValue = (! isPropagate() ? pathData.value : OptReal());

      auto *edge = createEdge(edgeValue, srcNode, destNode);

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
CQChartsSankeyPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initFromToObjs");

  return CQChartsConnectionPlot::initFromToObjs();
}

void
CQChartsSankeyPlot::
addFromToValue(const FromToData &fromToData) const
{
  // get src node
  auto *srcNode = findNode(fromToData.fromStr);

  if (fromToData.depth >= 0)
    srcNode->setDepth(fromToData.depth);

  //---

  // set group
  if (fromToData.groupData.ng > 1)
    srcNode->setGroup(fromToData.groupData.ig, fromToData.groupData.ng);
  else
    srcNode->setGroup(-1);

  //---

  // Just node
  if (fromToData.toStr == "") {
    // set node color (if color column specified)
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      srcNode->setColor(c);

    //---

    // set node name values (attribute column)
    processNodeNameValues(srcNode, fromToData.nameValues);
  }
  else {
    if (fromToData.fromStr == fromToData.toStr)
      return;

    auto *destNode = findNode(fromToData.toStr);

    if (fromToData.depth >= 0)
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

    addModelInd(fromToData.fromModelInd );
    addModelInd(fromToData.toModelInd   );
    addModelInd(fromToData.valueModelInd);
    addModelInd(fromToData.depthModelInd);

    edge->setValueColumn(fromToData.valueModelInd.column());

    //---

    // set edge color (if color column specified)
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      edge->setColor(c);

    //---

    // set edge name values (attribute column)
    processEdgeNameValues(edge, fromToData.nameValues);
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
    edge->setColor(c);

  //---

  // set value on dest node (NEEDED ?)
  destNode->setValue(linkConnectionData.value);

  //---

  // set group
  if (linkConnectionData.groupData.isValid())
    srcNode->setGroup(linkConnectionData.groupData.ig, linkConnectionData.groupData.ng);
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
  // get src node
  QString srcStr = QString("%1").arg(id);

  auto *srcNode = findNode(srcStr);

  srcNode->setName(connectionsData.name);

  //---

  // set group
  if (connectionsData.groupData.isValid())
    srcNode->setGroup(connectionsData.groupData.ig, connectionsData.groupData.ng);
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
    QString destStr = QString("%1").arg(connection.node);

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

      // create edge and link src/dest nodes
      auto *edge = createEdge(OptReal(value.value), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);
    }
  }

  //---

  return true;
}

//----

void
CQChartsSankeyPlot::
processNodeNameValues(Node *node, const NameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    QString value = nv.second.toString();

    if      (nv.first == "label") {
      node->setLabel(value);
    }
    else if (nv.first == "color") {
      node->setColor(CQChartsColor(value));
    }
  }
}

void
CQChartsSankeyPlot::
processEdgeNameValues(Edge *edge, const NameValues &nameValues) const
{
  auto *srcNode  = edge->srcNode ();
  auto *destNode = edge->destNode();

  for (const auto &nv : nameValues.nameValues()) {
    QString value = nv.second.toString();

    if      (nv.first == "path_id") {
      bool ok;
      int pathId = value.toInt(&ok);
      if (! ok || pathId < 0) continue;

      edge->setPathId(pathId);

      pathIdMinMax_.add(pathId);
    }
    else if (nv.first == "color") {
      edge->setColor(CQChartsColor(value));
    }
#if 0
    else if (nv.first == "label") {
      edge->setLabel(value);
    }
#endif
    else if (nv.first == "src_label") {
      srcNode->setLabel(value);
    }
    else if (nv.first == "dest_label") {
      destNode->setLabel(value);
    }
    else if (nv.first == "src_color") {
      srcNode->setColor(CQChartsColor(value));
    }
    else if (nv.first == "dest_color") {
      destNode->setColor(CQChartsColor(value));
    }
  }
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
  // create graph
  createGraph();

  //---

  // place graph
  placeGraph();

  //---

  // add objects to plot
  addObjects(objs);
}

void
CQChartsSankeyPlot::
addObjects(PlotObjs &objs) const
{
  assert(graph_);

  if (graph_->nodes().empty() || ! graph_->rect().isSet())
    return;

  // if graph rectangle invalid (zero width/height) then adjust to valid rect
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
    auto *nodeObj = createObjFromNode(node);

    objs.push_back(nodeObj);
  }

  //---

  // add edge objects
  for (const auto &edge : edges_) {
    if (! edge->srcNode()->isVisible() || ! edge->destNode()->isVisible())
      continue;

    auto *edgeObj = addEdgeObj(edge);

    objs.push_back(edgeObj);
  }

  //---

  // adjust node rects to align on matching path id
  if (hasAnyPathId()) {
    for (auto *node : graph_->nodes()) {
      node->adjustSrcDestRects();
    }
  }
}

// place nodes in graph
void
CQChartsSankeyPlot::
placeGraph() const
{
  assert(graph_);

  // get placeable nodes (nodes and sub graphs)
  Nodes nodes = graph_->placeableNodes();

  placeGraphNodes(nodes);
}

void
CQChartsSankeyPlot::
placeGraphNodes(const Nodes &nodes) const
{
  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  //---

  // set max depth of all graph nodes
  updateGraphMaxDepth(nodes);

  //---

  // set x pos of nodes
  calcGraphNodesXPos(nodes);

  //---

  // sort x nodes by associated path ids
  if (hasAnyPathId())
    th->sortDepthNodes();

  //---

  // calc max height (fron node count) and max size (from value) for each x
  graph_->setMaxHeight(0);
  graph_->setTotalSize(0.0);

  for (const auto &depthNodes : graph_->depthNodesMap()) {
    const auto &nodes = depthNodes.second.nodes;
    double      size  = depthNodes.second.size;

    graph_->setMaxHeight(std::max(graph_->maxHeight(), int(nodes.size())));
    graph_->setTotalSize(std::max(graph_->totalSize(), size));
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

  graph_->updateRect();
}

void
CQChartsSankeyPlot::
placeEdges()
{
  for (auto *node : graph_->nodes())
    node->placeEdges(/*reset*/true);
}

void
CQChartsSankeyPlot::
calcGraphNodesXPos(const Nodes &nodes) const
{
  // place graph nodes at x position
  graph_->clearDepthNodesMap();

  for (const auto &node : nodes) {
    int xpos = calcXPos(node);

    graph_->addDepthSize(xpos, node->edgeSum());
    graph_->addDepthNode(xpos, node);
  }

  //----

  // check if all nodes at single x
  if (graph_->depthNodesMap().size() == 1 && align() != Align::RAND) {
    auto *th = const_cast<CQChartsSankeyPlot *>(this);

    // TODO: stable pos (spiral/circle/grid ?)
    th->align_     = Align::RAND;
    th->alignRand_ = std::max(CMathRound::RoundNearest(sqrt(nodes.size())), 2);

    for (const auto &node : nodes)
      node->setDepth(-1);

    calcGraphNodesXPos(nodes);
  }
}

void
CQChartsSankeyPlot::
sortDepthNodes(bool force)
{
  if (! force && ! isSortPathIdNodes())
    return;

  using PathIdNodes = std::map<int, Nodes>;

  for (auto &depthNodes : graph_->depthNodesMap()) {
    PathIdNodes pathIdNodes;

    for (auto &node : depthNodes.second.nodes)
      pathIdNodes[-node->minPathId()].push_back(node);

    Nodes nodes;

    for (const auto &pn : pathIdNodes)
      for (const auto &node : pn.second)
        nodes.push_back(node);

    depthNodes.second.nodes = nodes;
  }
}

void
CQChartsSankeyPlot::
createGraph() const
{
  if (! graph_) {
    auto *th = const_cast<CQChartsSankeyPlot *>(this);

    th->graph_ = new Graph(this);
  }

  assert(graph_);

  // Add nodes to graph for group
  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    graph_->addNode(node);
  }
}

void
CQChartsSankeyPlot::
clearGraph()
{
  delete graph_;

  graph_ = nullptr;
}

void
CQChartsSankeyPlot::
calcValueMarginScale()
{
  // get node margin (in window coords ?)
  double nodeMargin = calcNodeMargin();

  //---

  double ys = 2.0; // default size of bbox

  if (bbox_.isSet()) {
  //ys = bbox.getHeight();
    ys = std::max(bbox_.getWidth(), bbox_.getHeight());
  }

  double ys1 = nodeMargin*ys;
  double ys2 = ys - ys1; // size minus margin

  //---

  // calc value margin (per node)/scale (to fit nodes in box)
  graph_->setValueMargin(graph_->maxHeight() > 1.0 ? ys1/(graph_->maxHeight() - 1) : 0.0);
  graph_->setValueScale (graph_->totalSize() > 0.0 ? ys2/ graph_->totalSize()      : 1.0);
}

double
CQChartsSankeyPlot::
calcNodeMargin() const
{
  double nodeMargin      = std::min(std::max(this->nodeMargin(), 0.0), 1.0);
  double pixelNodeMargin = windowToPixelHeight(nodeMargin);

  if (pixelNodeMargin < minNodeMargin())
    nodeMargin = pixelToWindowHeight(minNodeMargin());

  return nodeMargin;
}

void
CQChartsSankeyPlot::
placeDepthNodes() const
{
  // place node objects at each depth (xpos)
  for (const auto &depthNodes : graph_->depthNodesMap()) {
    int         xpos  = depthNodes.first;
    const auto &nodes = depthNodes.second.nodes;

    placeDepthSubNodes(xpos, nodes);
  }
}

void
CQChartsSankeyPlot::
placeDepthSubNodes(int xpos, const Nodes &nodes) const
{
  // place nodes to fit in bbox
  double ys = bbox_.getHeight();

  int minX = this->minX();
  int maxX = this->maxX();

  double xm = pixelToWindowWidth(nodeWidth());

  //---

  // get sum of margins nodes at depth
  double height = graph_->valueMargin()*(int(nodes.size()) - 1);

  // get sum of scaled values for nodes at depth
  for (const auto &node : nodes)
    height += graph_->valueScale()*node->edgeSum();

  // TODO: assert matches bbox height (with tolerance)

  //---

  // calc top (placing top to bottom)
  double y1 = bbox_.getYMax() - (ys - height)/2.0;

  //---

  for (const auto &node : nodes) {
    // calc height
    double h = graph_->valueScale()*node->edgeSum();

    if (h <= 0.0)
      h = 0.1;

    //---

    // calc rect
    int xpos1 = calcXPos(node);
    assert(xpos == xpos1);

    // map xpos to bbox range (use for left)
    double x11 = CMathUtil::map(xpos1, minX, maxX, bbox_.getXMin(), bbox_.getXMax());
    double x12 = x11 + xm;

    double yc = y1 - h/2.0; // placement center

    double y11 = yc - h/2.0; // y1 - h
    double y12 = yc + h/2.0; // y1

    //---

    // calc bbox (adjust align for first left edge (minX) or right edge (maxX))
    BBox rect;

    if      (xpos1 == minX)
      rect = BBox(x11, y11, x12, y12); // left edge (left align)
    else if (xpos1 == maxX) {
      x11 -= xm; x12 -= xm;

      rect = BBox(x11, y11, x12, y12); // right edge (right align)
    }
    else {
      x11 -= xm/2.0; x12 -= xm/2.0;

      rect = BBox(x11, y11, x12, y12); // center align
    }

    //---

    node->setRect(rect);

    //---

    y1 -= h + graph_->valueMargin();
  }
}

CQChartsSankeyPlot::NodeObj *
CQChartsSankeyPlot::
createObjFromNode(Node *node) const
{
//int numNodes = graph_->nodes().size(); // node id needs to be per graph
  int numNodes = indNodeMap_.size();

  ColorInd ig;

  if (node->ngroup() > 0 && node->group() >= 0 && node->group() < node->ngroup())
    ig = ColorInd(node->group(), node->ngroup());

  ColorInd iv(node->id(), numNodes);

  auto *nodeObj = createNodeObj(node->rect(), node, ig, iv);

  nodeObj->setHierName(node->str());

  for (const auto &modelInd : node->modelInds())
    nodeObj->addModelInd(normalizedModelIndex(modelInd));

  node->setObj(nodeObj);

  nodeObj->setSelected(node->isSelected());

  return nodeObj;
}

int
CQChartsSankeyPlot::
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
      xpos = graph_->maxNodeDepth();
    else {
      if      (align() == Align::SRC)
        xpos = srcDepth;
      else if (align() == Align::DEST)
        xpos = graph_->maxNodeDepth() - destDepth;
      else if (align() == Align::JUSTIFY) {
        double f = 1.0*srcDepth/(srcDepth + destDepth);

        xpos = int(f*graph_->maxNodeDepth());
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

  BBox nodeRect;

  nodeRect += edge->srcNode ()->rect();
  nodeRect += edge->destNode()->rect();

  BBox rect(nodeRect.getXMin() - xm, nodeRect.getYMin() - ym,
            nodeRect.getXMax() + xm, nodeRect.getYMax() + ym);

  auto *edgeObj = createEdgeObj(rect, edge);

  for (const auto &modelInd : edge->modelInds())
    edgeObj->addModelInd(normalizedModelIndex(modelInd));

  edge->setObj (edgeObj);
  edge->setLine(isEdgeLine());

  return edgeObj;
}

void
CQChartsSankeyPlot::
updateGraphMaxDepth(const Nodes &nodes) const
{
  // calc max depth (source or dest) depending on align for xpos calc
  bool set = false;

  graph_->setMaxNodeDepth(0);

  auto updateNodeDepth = [&](int depth) {
    if (! set) {
      graph_->setMaxNodeDepth(depth);

      set = true;
    }
    else
      graph_->setMaxNodeDepth(std::max(graph_->maxNodeDepth(), depth));
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
adjustNodes(bool force) const
{
  bool changed = false;

  if (graph_) {
    Nodes nodes = graph_->placeableNodes();

    if (adjustGraphNodes(nodes, force))
      changed = true;
  }

  return changed;
}

bool
CQChartsSankeyPlot::
adjustGraphNodes(const Nodes &nodes, bool force) const
{
  if (! force && ! isAdjustNodes())
    return false;

  initPosNodesMap(nodes);

  //---

  int numPasses = adjustIterations();

  for (int pass = 0; pass < numPasses; ++pass) {
    if (! adjustNodeCenters())
      break;
  }

  //---

  reorderNodeEdges(nodes);

  //---

  adjustEdgeOverlaps();

  return true;
}

void
CQChartsSankeyPlot::
initPosNodesMap(const Nodes &nodes) const
{
  // get nodes by x pos
  graph_->resetPosNodes();

  for (const auto &node : nodes)
    graph_->addPosNode(node);
}

bool
CQChartsSankeyPlot::
adjustNodeCenters(bool removeOverlaps, bool force) const
{
  if (! force && ! isAdjustCenters())
    return false;

  bool changed = false;

  if (adjustNodeCentersLtoR(removeOverlaps, force))
    changed = true;

  if (adjustNodeCentersRtoL(removeOverlaps, force))
    changed = true;

  return changed;
}

bool
CQChartsSankeyPlot::
adjustNodeCentersLtoR(bool remove, bool force) const
{
  if (! force && ! isAdjustCenters())
    return false;

  // adjust nodes so centered on src nodes
  bool changed = false;

  int minX = this->minX();
  int maxX = this->maxX();

  // second to last minus one (last if SRC align)
  int startX = minX + 1;
  int endX   = maxX - 1;

  if (align() == Align::SRC)
    endX = maxX;

  for (int xpos = startX; xpos <= endX; ++xpos) {
    if (! graph_->hasPosNodes(xpos)) continue;

    const auto &nodes = graph_->posNodes(xpos);

    for (const auto &node : nodes) {
      if (isAdjustSelected() && ! node->isSelected())
        continue;

      if (adjustNode(node))
        changed = true;
    }
  }

  if (remove) {
    if (removeOverlaps())
      changed = true;
  }

  return changed;
}

bool
CQChartsSankeyPlot::
adjustNodeCentersRtoL(bool remove, bool force) const
{
  if (! force && ! isAdjustCenters())
    return false;

  // adjust nodes so centered on src nodes
  bool changed = false;

  int minX = this->minX();
  int maxX = this->maxX();

  // last minus one to second (first if DEST align)
  int startX = minX + 1;
  int endX   = maxX - 1;

  if (align() == Align::DEST)
    startX = minX;

  // second to last to second
  for (int xpos = endX; xpos >= startX; --xpos) {
    if (! graph_->hasPosNodes(xpos)) continue;

    const auto &nodes = graph_->posNodes(xpos);

    for (const auto &node : nodes) {
      if (isAdjustSelected() && ! node->isSelected())
        continue;

      if (adjustNode(node))
        changed = true;
    }
  }

  if (remove) {
    if (removeOverlaps())
      changed = true;
  }

  return changed;
}

bool
CQChartsSankeyPlot::
adjustEdgeOverlaps(bool force) const
{
  if (! force && ! isAdjustEdgeOverlaps())
    return false;

  //---

  auto hasNode = [&](Node *node, const Nodes &nodes) {
    for (auto &node1 : nodes)
      if (node == node1)
        return true;

    return false;
  };

  int posNodesDepth = graph_->posNodesMap().size();

  // find first x pos with nodes
  int xpos1 = 0;

  while (xpos1 <= posNodesDepth && ! graph_->hasPosNodes(xpos1))
    ++xpos1;

  if (xpos1 > posNodesDepth)
    return false;

  int xpos2 = xpos1 + 1;

  while (xpos2 <= posNodesDepth) {
    // find next x pos with nodes
    while (xpos2 <= posNodesDepth && ! graph_->hasPosNodes(xpos2))
      ++xpos2;

    if (xpos2 > posNodesDepth)
      break;

    // get nodes at each pos
    const auto &nodes1 = graph_->posNodes(xpos1);
    const auto &nodes2 = graph_->posNodes(xpos2);

    // get edges between nodes
    Edges edges;

    for (const auto &node1 : nodes1) {
      for (const auto &edge1 : node1->destEdges()) {
        auto *destNode1 = edge1->destNode();

        if (hasNode(destNode1, nodes2))
          edges.push_back(edge1);
      }
    }

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

    xpos1 = xpos2++;
  }

  return true;
}

bool
CQChartsSankeyPlot::
removeOverlaps(bool spread, bool constrain, bool force) const
{
  if (! force && ! isRemoveOverlaps())
    return false;

  bool changed = false;

  Nodes nodes = graph_->placeableNodes();

  initPosNodesMap(nodes);

  for (const auto &posNodes : graph_->posNodesMap()) {
    if (removePosOverlaps(posNodes.first, posNodes.second, spread, constrain))
      changed = true;
  }

  return changed;
}

bool
CQChartsSankeyPlot::
removePosOverlaps(int pos, const Nodes &nodes, bool spread, bool constrain) const
{
  double ym = pixelToWindowHeight(minNodeMargin());

  //---

  auto removeNodesOverlaps = [&](bool increasing) {
    // get nodes sorted by y (min->max or max->min)
    PosNodeMap posNodeMap;

    createPosNodeMap(nodes, posNodeMap, increasing);

    //---

    // remove overlaps between nodes
    bool changed = false;

    Node *node1 = nullptr;

    for (const auto &posNode : posNodeMap) {
      auto *node2 = posNode.second;

      if (node1) {
        const auto &rect1 = node1->rect();
        const auto &rect2 = node2->rect();

        if (increasing) {
          if (rect2.getYMin() <= rect1.getYMax() + ym) {
            double dy = rect1.getYMax() + ym - rect2.getYMin();

            if (std::abs(dy) > 1E-6) {
              node2->moveBy(Point(0, dy));
              changed = true;
            }
          }
        }
        else {
          if (rect2.getYMax() >= rect1.getYMin() - ym) {
            double dy = rect1.getYMin() - ym - rect2.getYMax();

            if (std::abs(dy) > 1E-6) {
              node2->moveBy(Point(0, dy));
              changed = true;
            }
          }
        }
      }

      node1 = node2;
    }

    return changed;
  };

  //---

  bool spread1 = spread;
  bool center  = false;

  if (spread1) {
    if      (this->spread() == Spread::NONE) {
      spread1 = false;
      center  = true;
    }
    else if (this->spread() == Spread::FIRST)
      spread1 = (pos == minX());
    else if (this->spread() == Spread::LAST)
      spread1 = (pos == maxX());
    else if (this->spread() == Spread::FIRST_LAST)
      spread1 = (pos == minX() || pos == maxX());
  }

  //---

  bool changed = false;

  if (removeNodesOverlaps(/*increasing*/false))
    changed = true;

  // move nodes back inside bbox (needed ?)
  if (spread1) {
    if (spreadPosNodes(nodes))
      changed = true;
  }

  if (constrain) {
    if (constrainPosNodes(nodes, center))
      changed = true;
  }

  if (removeNodesOverlaps(/*increasing*/true))
    changed = true;

  // move nodes back inside bbox (needed ?)
  if (spread1) {
    if (spreadPosNodes(nodes))
      changed = true;
  }

  if (constrain) {
    if (constrainPosNodes(nodes, center))
      changed = true;
  }

  return changed;
}

bool
CQChartsSankeyPlot::
spreadNodes() const
{
  bool changed = false;

  Nodes nodes = graph_->placeableNodes();

  initPosNodesMap(nodes);

  for (const auto &posNodes : graph_->posNodesMap()) {
    int pos = posNodes.first;

    if (pos == minX() || pos == maxX()) {
      if (spreadPosNodes(posNodes.second))
        changed = true;
    }
  }

  return changed;
}

bool
CQChartsSankeyPlot::
spreadPosNodes(const Nodes &nodes) const
{
  PosNodeMap posNodeMap;

  createPosNodeMap(nodes, posNodeMap, /*increasing*/false);

  BBox bbox;

  Node *node1 = nullptr, *node2 = nullptr;

  for (const auto &posNode : posNodeMap) {
    node2 = posNode.second;

    if (! node1) node1 = node2;

    bbox += node2->rect();
  }

  if (! node1 || ! node2)
    return false;

  double dy1 = node1->rect().getHeight()/2.0; // top
  double dy2 = node2->rect().getHeight()/2.0; // bottom

  if (! bbox.isValid() || (bbox.getHeight() - dy1 - dy2) <= 0.0)
    return false;

  double ymin = bbox_.getYMin() + dy2;
  double ymax = bbox_.getYMax() - dy1;

  double dy = ymin - node2->rect().getYMid();
  double ys = (ymax - ymin)/(bbox.getHeight() - dy1 - dy2);

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
CQChartsSankeyPlot::
constrainNodes(bool center) const
{
  bool changed = false;

  Nodes nodes = graph_->placeableNodes();

  initPosNodesMap(nodes);

  for (const auto &posNodes : graph_->posNodesMap()) {
    if (constrainPosNodes(posNodes.second, center))
      changed = true;
  }

  return changed;
}

bool
CQChartsSankeyPlot::
constrainPosNodes(const Nodes &nodes, bool center) const
{
  PosNodeMap posNodeMap;

  createPosNodeMap(nodes, posNodeMap, /*increasing*/false);

  BBox bbox;

  Node *node1 = nullptr, *node2 = nullptr;

  for (const auto &posNode : posNodeMap) {
    node2 = posNode.second;

    if (! node1) node1 = node2;

    bbox += node2->rect();
  }

  if (! node1 || ! node2)
    return false;

  double dy1 = node2->rect().getYMin() - bbox_.getYMin();
  double dy2 = bbox_.getYMax() - node1->rect().getYMax();

  double dy = 0.0;

  if (dy1 >= 0 && dy2 >= 0) {
    if (! center)
      return false;

    dy = bbox_.getYMid() - bbox.getYMid();
  }
  else {
    if      (dy1 < 0 && dy2 < 0) {
      dy = bbox_.getYMid() - bbox.getYMid();
    }
    else if (dy1 < 0) {
      dy = -dy1;
    }
    else {
      dy = dy2;
    }
  }

  if (CMathUtil::realEq(dy, 0.0))
    return false;

  for (const auto &posNode : posNodeMap) {
    auto *node = posNode.second;

    node->moveBy(Point(0, dy));
  }

  return true;
}

bool
CQChartsSankeyPlot::
reorderNodeEdges(const Nodes &nodes, bool force) const
{
  if (! force && ! isReorderEdges())
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

void
CQChartsSankeyPlot::
createPosNodeMap(const Nodes &nodes, PosNodeMap &posNodeMap, bool increasing) const
{
  for (const auto &node : nodes) {
    const auto &rect = node->rect();
    if (! rect.isValid()) continue;

    double y = (increasing ? rect.getYMid() - bbox_.getYMin() : bbox_.getYMax() - rect.getYMid());

    auto p = posNodeMap.find(y);

    double dy = 0.001;

    while (p != posNodeMap.end()) {
      double y1 = y - dy;

      while (y1 == y) {
        dy *= 10;

        y1 = y - dy;
      }

      p = posNodeMap.find(y1);
      y = y1;
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

    double dy = 0.001;

    while (p != posEdgeMap.end()) {
      double y1 = y - dy;

      while (y1 == y) {
        dy *= 10;

        y1 = y - dy;
      }

      p = posEdgeMap.find(y1);
      y = y1;
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
    if (adjustNodes(/*force*/true))
      drawObjs();
  }
  else if (key == Qt::Key_C) {
    if (adjustNodeCenters(/*removeOverlaps*/ false, /*force*/true))
      drawObjs();
  }
  else if (key == Qt::Key_L) {
    if (adjustNodeCentersLtoR(/*removeOverlaps*/false, /*force*/true))
      drawObjs();
  }
  else if (key == Qt::Key_R) {
    if (adjustNodeCentersRtoL(/*removeOverlaps*/false, /*force*/true))
      drawObjs();
  }
  else if (key == Qt::Key_O) {
    if (removeOverlaps(/*spread*/false, /*constrain*/false, /*force*/true))
      drawObjs();
  }
  else if (key == Qt::Key_I) {
    if (constrainNodes())
      drawObjs();
  }
  else if (key == Qt::Key_S) {
    if (spreadNodes())
      drawObjs();
  }
  else if (key == Qt::Key_F) {
    bbox_ = nodesBBox(); // current

    fitToBBox(targetBBox_);

    drawObjs();
  }
  else if (key == Qt::Key_P) {
    placeGraph();

    placeEdges();

    drawObjs();
  }
  else if (key == Qt::Key_0) {
    printStats();
  }
  else if (key == Qt::Key_V) {
    adjustEdgeOverlaps(/*force*/true);
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

//------

CQChartsSankeyPlotNode::
CQChartsSankeyPlotNode(const Plot *plot, const QString &str) :
 plot_(plot), str_(str)
{
}

CQChartsSankeyPlotNode::
~CQChartsSankeyPlotNode()
{
  assert(! obj_);
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

void
CQChartsSankeyPlotNode::
sortPathIdEdges(bool force)
{
  if (! force && ! plot()->isSortPathIdEdges())
    return;

  //---

  using PathIdEdges = std::map<int, Edges>;

  PathIdEdges srcPathIdEdges, destPathIdEdges;

  for (auto &edge : srcEdges_)
    srcPathIdEdges[-edge->pathId()].push_back(edge);

  for (auto &edge : destEdges_)
    destPathIdEdges[-edge->pathId()].push_back(edge);

  srcEdges_.clear();

  for (const auto &pe : srcPathIdEdges)
    for (const auto &edge : pe.second)
      srcEdges_.push_back(edge);

  destEdges_.clear();

  for (const auto &pe : destPathIdEdges)
    for (const auto &edge : pe.second)
      destEdges_.push_back(edge);
}

int
CQChartsSankeyPlotNode::
minPathId() const
{
  int  minPathId    = -1;
  bool minPathIdSet = false;

  auto updateMinPathId = [&](int pathId) {
    if (pathId < 0) return;

    if (! minPathIdSet || pathId < minPathId) {
      minPathId    = pathId;
      minPathIdSet = true;
    }
  };

  for (const auto &edge : srcEdges_)
    updateMinPathId(edge->pathId());

  for (const auto &edge : destEdges_)
    updateMinPathId(edge->pathId());

  return minPathId;
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
setRect(const BBox &rect)
{
  assert(rect.isSet());

  rect_ = rect;

  if (obj_) // TODO: assert null or use move by
    obj_->setRect(rect);
}

double
CQChartsSankeyPlotNode::
edgeSum() const
{
  double sum = std::max(srcEdgeSum(), destEdgeSum());

  if (CMathUtil::realEq(sum, 0.0))
    sum = 1.0;

  return sum;
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

  if (plot()->hasAnyPathId())
    sortPathIdEdges();

  //---

  double x1 = rect().getXMin();
  double x2 = rect().getXMax();
  double y1 = rect().getYMin();
  double y2 = rect().getYMax();

  clearSrcEdgeRects ();
  clearDestEdgeRects();

  double srcTotal  = srcEdgeSum ();
  double destTotal = destEdgeSum();

  if (! plot_->useMaxTotals()) {
    if (this->srcEdges().size() == 1) {
      auto *edge = *this->srcEdges().begin();

      setSrcEdgeRect(edge, BBox(x1, y1, x2, y2));
    }
    else {
      double ys = (srcTotal > 0.0 ? (y2 - y1)/srcTotal : 0.0);

      double y3 = y2; // top

      for (const auto &edge : this->srcEdges()) {
        if (! edge->hasValue()) {
          setSrcEdgeRect(edge, BBox());
          continue;
        }

        double h1 = ys*edge->value().real();
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
      double ys = (destTotal > 0.0 ? (y2 - y1)/destTotal : 0.0);

      double y3 = y2; // top

      for (const auto &edge : this->destEdges()) {
        if (! edge->hasValue()) {
          setDestEdgeRect(edge, BBox());
          continue;
        }

        double h1 = ys*edge->value().real();
        double y4 = y3 - h1;

        if (! hasDestEdgeRect(edge))
          setDestEdgeRect(edge, BBox(x1, y4, x2, y3));

        y3 = y4;
      }
    }
  }
  else {
    double maxTotal = std::max(srcTotal, destTotal);

    double ys = (maxTotal > 0.0 ? (y2 - y1)/maxTotal : 0.0);

    double dy1 = ((y2 - y1) - ys*srcTotal )/2.0;
    double dy2 = ((y2 - y1) - ys*destTotal)/2.0;

    double y3 = y2 - dy1; // top

    for (const auto &edge : this->srcEdges()) {
      if (! edge->hasValue()) {
        setSrcEdgeRect(edge, BBox());
        continue;
      }

      double h1 = ys*edge->value().real();
      double y4 = y3 - h1;

      if (! hasSrcEdgeRect(edge))
        setSrcEdgeRect(edge, BBox(x1, y4, x2, y3));

      y3 = y4;
    }

    //---

    y3 = y2 - dy2; // top

    for (const auto &edge : this->destEdges()) {
      if (! edge->hasValue()) {
        setDestEdgeRect(edge, BBox());
        continue;
      }

      double h1 = ys*edge->value().real();
      double y4 = y3 - h1;

      if (! hasDestEdgeRect(edge))
        setDestEdgeRect(edge, BBox(x1, y4, x2, y3));

      y3 = y4;
    }
  }
}

void
CQChartsSankeyPlotNode::
setSrcEdgeRect(Edge *edge, const BBox &bbox)
{
  srcEdgeRect_[edge] = bbox;

  if (edge->pathId() >= 0)
    srcPathIdRect_[edge->pathId()] = bbox;
}

void
CQChartsSankeyPlotNode::
setDestEdgeRect(Edge *edge, const BBox &bbox)
{
  destEdgeRect_[edge] = bbox;

  if (edge->pathId() >= 0)
    destPathIdRect_[edge->pathId()] = bbox;
}

void
CQChartsSankeyPlotNode::
adjustSrcDestRects()
{
  if (! plot_->useMaxTotals())
    return;

  // align each source path id rect with destination path id rects
  for (const auto &pathIdRect : srcPathIdRect_) {
    int pathId = pathIdRect.first;

    auto p = destPathIdRect_.find(pathId);
    if (p == destPathIdRect_.end()) continue;

    BBox srcRect  = pathIdRect.second;
    BBox destRect = (*p).second;

    if (! srcRect.isSet() || ! destRect.isSet())
      continue;

    double y1 = srcRect .getYMid();
    double y2 = destRect.getYMid();

    double dy = y1 - y2;

    if (CMathUtil::realEq(dy, 0.0))
      continue;

    if (srcEdgeSum() > destEdgeSum()) {
      destRect.moveBy(Point(0, dy));

      for (auto &edge : this->destEdges()) {
        if (edge->pathId() == pathId)
          setDestEdgeRect(edge, destRect);
      }
    }
    else {
      srcRect.moveBy(Point(0, -dy));

      for (auto &edge : this->srcEdges()) {
        if (edge->pathId() == pathId)
          setSrcEdgeRect(edge, srcRect);
      }
    }
  }
}

//---

void
CQChartsSankeyPlotNode::
allSrcNodesAndEdges(NodeSet &nodeSet, EdgeSet &edgeSet) const
{
  if (nodeSet.find(this) != nodeSet.end())
    return;

  nodeSet.insert(this);

  for (const auto &edge : this->srcEdges()) {
    edgeSet.insert(edge);
  }

  for (const auto &edge : this->srcEdges()) {
    auto *node = edge->srcNode();

    node->allSrcNodesAndEdges(nodeSet, edgeSet);
  }
}

void
CQChartsSankeyPlotNode::
allDestNodesAndEdges(NodeSet &nodeSet, EdgeSet &edgeSet) const
{
  if (nodeSet.find(this) != nodeSet.end())
    return;

  nodeSet.insert(this);

  for (const auto &edge : this->destEdges()) {
    edgeSet.insert(edge);
  }

  for (const auto &edge : this->destEdges()) {
    auto *node = edge->destNode();

    node->allDestNodesAndEdges(nodeSet, edgeSet);
  }
}

//---

QColor
CQChartsSankeyPlotNode::
calcColor() const
{
  CQChartsUtil::ColorInd ic;

  if (ngroup() > 0 && group() >= 0 && group() < ngroup())
    ic = CQChartsUtil::ColorInd(group(), ngroup());
  else
    ic = CQChartsUtil::ColorInd(id(), plot_->numNodes());

  if (color_.isValid())
    return plot_->interpColor(color_, ic);
  else
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
  assert(! obj_);
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

  if (! srcRect.isSet())
    srcRect = this->srcNode()->rect();

  if (! destRect.isSet())
    destRect = this->destNode()->rect();

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
  if (node())
    node()->setObj(nullptr);
}

QString
CQChartsSankeyNodeObj::
name() const
{
  return node()->name();
}

void
CQChartsSankeyNodeObj::
setName(const QString &s)
{
  node()->setName(s);
}

double
CQChartsSankeyNodeObj::
value() const
{
  return node()->value().realOr(0.0);
}

void
CQChartsSankeyNodeObj::
setValue(double r)
{
  node()->setValue(CQChartsSankeyPlotNode::OptReal(r));
}

int
CQChartsSankeyNodeObj::
depth() const
{
  return node()->depth();
}

void
CQChartsSankeyNodeObj::
setDepth(int depth)
{
  node()->setDepth(depth);
}

CQChartsColor
CQChartsSankeyNodeObj::
color() const
{
  return node()->color();
}

void
CQChartsSankeyNodeObj::
setColor(const CQChartsColor &c)
{
  node()->setColor(c);
}

void
CQChartsSankeyNodeObj::
placeEdges(bool reset)
{
  node()->placeEdges(reset);
}

QString
CQChartsSankeyNodeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(node()->id());
}

QString
CQChartsSankeyNodeObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  QString name = this->name();

  if (name == "")
    name = this->id();

  QString hierName = this->hierName();

  if (hierName != name)
    tableTip.addTableRow("Hier Name", hierName);

  tableTip.addTableRow("Name", name);

  if (node()->hasValue())
    tableTip.addTableRow("Value", value());

  if (depth() >= 0)
    tableTip.addTableRow("Depth", depth());

  int ns = node()->srcEdges ().size();
  int nd = node()->destEdges().size();

  tableTip.addTableRow("Edges", QString("In:%1, Out:%2").arg(ns).arg(nd));

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

  auto *graph = plot_->graph();

  if (graph)
    graph->updateRect();

  editChanged_ = true;

  const_cast<CQChartsSankeyPlot *>(plot())->drawObjs();

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
    const_cast<CQChartsSankeyPlot *>(plot())->invalidateObjTree();

  return true;
}

void
CQChartsSankeyNodeObj::
setEditBBox(const BBox &bbox, const CQChartsResizeSide &)
{
  assert(bbox.isSet());

  double dx = bbox.getXMin() - rect_.getXMin();
  double dy = bbox.getYMin() - rect_.getYMin();

  node()->moveBy(Point(dx, dy));
}

//---

void
CQChartsSankeyNodeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->modelColumns())
    addColumnSelectIndex(inds, c);
}

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
  PenBrush penBrush;

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

      drawConnectionMouseOver(device, (int) plot_->mouseColoring(), -1);

      plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
    }
  }
}

void
CQChartsSankeyNodeObj::
drawConnectionMouseOver(CQChartsPaintDevice *device, int imouseColoring, int pathId) const
{
  auto mouseColoring = (CQChartsSankeyPlot::ConnectionType) imouseColoring;

  //---

  auto drawEdgeInside = [&](const Edge *edge) {
    auto *edgeObj = edge->obj(); if (! edgeObj) return;

    edgeObj->setInside(true); edgeObj->draw(device); edgeObj->setInside(false);
  };

  auto drawNodeInside = [&](const Node *node, bool isSrc) {
    auto *nodeObj = node->obj(); if (! nodeObj) return;

    nodeObj->setInside(true);

    if (plot_->isMouseNodeColoring())
      nodeObj->draw(device);

    if (pathId >= 0) {
      Edge *edge = nullptr;

      if (isSrc) {
        for (const auto &edge1 : node->destEdges()) {
          if (edge1->pathId() == pathId) {
            edge = edge1;
            break;
          }
        }
      }
      else {
        for (const auto &edge1 : node->srcEdges()) {
          if (edge1->pathId() == pathId) {
            edge = edge1;
            break;
          }
        }
      }

      if (edge) {
        auto rect = (isSrc ? node->destEdgeRect(edge) : node->srcEdgeRect(edge));

        if (! plot()->isTextVisible() && rect.isSet())
          nodeObj->drawFgRect(device, rect);
      }
      else {
        //nodeObj->drawFg(device);
      }
    }
    else
      nodeObj->drawFg(device);

    nodeObj->setInside(false);
  };

  //---

  if      (mouseColoring == CQChartsSankeyPlot::ConnectionType::SRC ||
           mouseColoring == CQChartsSankeyPlot::ConnectionType::SRC_DEST) {
    for (const auto &edge : node()->srcEdges()) {
      if (pathId < 0 || edge->pathId() == pathId)
        drawEdgeInside(edge);
    }
  }
  else if (mouseColoring == CQChartsSankeyPlot::ConnectionType::ALL_SRC ||
           mouseColoring == CQChartsSankeyPlot::ConnectionType::ALL_SRC_DEST) {
    CQChartsSankeyPlotNode::NodeSet nodeSet;
    CQChartsSankeyPlotNode::EdgeSet edgeSet;

    node()->allSrcNodesAndEdges(nodeSet, edgeSet);

    for (const auto &edge : edgeSet) {
      if (pathId < 0 || edge->pathId() == pathId)
        drawEdgeInside(edge);
    }

    for (const auto &node : nodeSet)
      drawNodeInside(node, /*isSrc*/true);
  }

  if      (mouseColoring == CQChartsSankeyPlot::ConnectionType::DEST ||
           mouseColoring == CQChartsSankeyPlot::ConnectionType::SRC_DEST) {
    for (const auto &edge : node()->destEdges()) {
      if (pathId < 0 || edge->pathId() == pathId)
        drawEdgeInside(edge);
    }
  }
  else if (mouseColoring == CQChartsSankeyPlot::ConnectionType::ALL_DEST ||
           mouseColoring == CQChartsSankeyPlot::ConnectionType::ALL_SRC_DEST) {
    CQChartsSankeyPlotNode::NodeSet nodeSet;
    CQChartsSankeyPlotNode::EdgeSet edgeSet;

    node()->allDestNodesAndEdges(nodeSet, edgeSet);

    for (const auto &edge : edgeSet) {
      if (pathId < 0 || edge->pathId() == pathId)
        drawEdgeInside(edge);
    }

    for (const auto &node : nodeSet)
      drawNodeInside(node, /*isSrc*/false);
  }
}

void
CQChartsSankeyNodeObj::
drawFg(CQChartsPaintDevice *device) const
{
  drawFgRect(device, rect());
}

void
CQChartsSankeyNodeObj::
drawFgRect(CQChartsPaintDevice *device, const BBox &rect) const
{
  if (! plot_->isTextVisible()) {
    bool visible = false;

    if (plot_->isInsideTextVisible() && isInside())
      visible = true;

    if (plot_->isSelectedTextVisible() && isSelected())
      visible = true;

    if (! visible)
      return;
  }

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

  QString str = node()->label();

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

  double tx;

  if (rect.getXMid() < xm - tw)
    tx = prect.getXMax() + textMargin;
  else
    tx = prect.getXMin() - textMargin - ptw;

  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot()->pixelToWindow(Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = Angle();
  options.contrast      = plot()->isTextContrast();
  options.contrastAlpha = plot()->textContrastAlpha();
  options.align         = Qt::AlignLeft;
  options.clipLength    = clipLength;
  options.clipElide     = plot()->textClipElide();

  if (plot_->isAdjustText()) {
    auto *drawText =
      new CQChartsSankeyPlot::DrawText(str, pt, options, c, plot_->textAlpha(), rect.getCenter());

    auto bbox = CQChartsDrawUtil::calcTextAtPointRect(device, drawText->point, drawText->str,
                                                      drawText->options);

    drawText->setBBox(bbox);

    plot_->addDrawText(drawText);
  }
  else {
    CQChartsDrawUtil::drawTextAtPoint(device, pt, str, options);
  }
}

void
CQChartsSankeyNodeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto ic = calcColorInd();

  auto bc = plot_->interpNodeStrokeColor(ic);
  auto fc = calcFillColor();

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
    auto ic = calcColorInd();

    if (color().isValid())
      fc = plot_->interpColor(color(), ic);
    else
      fc = plot_->interpNodeFillColor(ic);
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

CQChartsSankeyEdgeObj::
~CQChartsSankeyEdgeObj()
{
  if (edge_)
    edge_->setObj(nullptr);
}

QString
CQChartsSankeyEdgeObj::
calcId() const
{
  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  return QString("%1:%2:%3:%4").arg(typeName()).
           arg(srcObj->calcId()).arg(destObj->calcId()).arg(edge()->id());
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

  if (edge()->hasValue()) {
    QString headerStr("Value");

    if (edge()->valueColumn().isValid())
      headerStr = plot_->columnHeaderName(edge()->valueColumn());

    tableTip.addTableRow(headerStr, edge()->value().real());
  }

  if (edge()->pathId() >= 0)
    tableTip.addTableRow("Path Id", edge()->pathId());

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

//---

void
CQChartsSankeyEdgeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : plot()->modelColumns())
    addColumnSelectIndex(inds, c);
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
  PenBrush penBrush;

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

  // show source and destination nodes on inside
  if (plot()->view()->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    if (plot()->mouseColoring() != CQChartsSankeyPlot::ConnectionType::NONE) {
      auto drawNodeInside = [&](const Node *node, bool isSrc) {
        auto *nodeObj = node->obj(); if (! nodeObj) return;

        auto rect = (isSrc ? node->destEdgeRect(edge()) : node->srcEdgeRect(edge()));
        if (! rect.isSet()) return;

        nodeObj->setInside(true);

        if (plot()->isMouseNodeColoring())
          nodeObj->draw(device);

        if (! plot()->isTextVisible())
          nodeObj->drawFgRect(device, rect);

        nodeObj->setInside(false);
      };

      //---

      auto *srcNode  = edge()->srcNode ();
      auto *destNode = edge()->destNode();

      auto *srcNodeObj  = srcNode ->obj();
      auto *destNodeObj = destNode->obj();

      plot()->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER_EXTRA);

      if      (plot()->mouseColoring() == CQChartsSankeyPlot::ConnectionType::SRC) {
        drawNodeInside(srcNode , /*isSrc*/true );
      }
      else if (plot()->mouseColoring() == CQChartsSankeyPlot::ConnectionType::DEST) {
        drawNodeInside(destNode, /*isSrc*/false);
      }
      else if (plot()->mouseColoring() == CQChartsSankeyPlot::ConnectionType::SRC_DEST) {
        drawNodeInside(srcNode , /*isSrc*/true );
        drawNodeInside(destNode, /*isSrc*/false);
      }
      else if (plot()->mouseColoring() == CQChartsSankeyPlot::ConnectionType::ALL_SRC) {
        drawNodeInside(srcNode , /*isSrc*/true );

        if (srcNodeObj)
          srcNodeObj->drawConnectionMouseOver(device,
            (int) CQChartsSankeyPlot::ConnectionType::ALL_SRC, edge()->pathId());
      }
      else if (plot()->mouseColoring() == CQChartsSankeyPlot::ConnectionType::ALL_DEST) {
        drawNodeInside(destNode, /*isSrc*/false);

        if (destNodeObj)
          destNodeObj->drawConnectionMouseOver(device,
            (int) CQChartsSankeyPlot::ConnectionType::ALL_DEST, edge()->pathId());
      }
      else if (plot()->mouseColoring() == CQChartsSankeyPlot::ConnectionType::ALL_SRC_DEST) {
        drawNodeInside(srcNode , /*isSrc*/true );
        drawNodeInside(destNode, /*isSrc*/false);

        if (srcNodeObj)
          srcNodeObj->drawConnectionMouseOver(device,
            (int) CQChartsSankeyPlot::ConnectionType::ALL_SRC, edge()->pathId());

        if (destNodeObj)
          destNodeObj->drawConnectionMouseOver(device,
            (int) CQChartsSankeyPlot::ConnectionType::ALL_DEST, edge()->pathId());
      }

      plot()->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
    }
  }
}

bool
CQChartsSankeyEdgeObj::
edgePath(QPainterPath &path, bool isLine) const
{
  return edge()->edgePath(path, isLine);
}

#if 0
void
CQChartsSankeyEdgeObj::
drawFg(CQChartsPaintDevice *device) const
{
  if (! plot()->isTextVisible())
    return;

  QString str = edge()->label();

  if (! str.length())
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

  auto prect = plot()->windowToPixel(rect);

  //---

  // set font
  plot()->view()->setPlotPainterFont(plot(), device, plot()->textFont());

  QFontMetricsF fm(device->font());

  //---

  // set text pen
  auto ic = calcColorInd();

  PenBrush penBrush;

  auto c = plot()->interpTextColor(ic);

  plot()->setPen(penBrush, PenData(true, c, plot()->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  double ptw = fm.width(str);

  double tx = prect.getXMid() - textMargin - ptw/2.0;
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot()->pixelToWindow(Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = Angle();
  options.contrast      = plot()->isTextContrast();
  options.contrastAlpha = plot()->textContrastAlpha();
  options.align         = Qt::AlignLeft;
  options.clipLength    = plot()->lengthPixelWidth(plot()->textClipLength());
  options.clipElide     = plot()->textClipElide();

  CQChartsDrawUtil::drawTextAtPoint(device, pt, str, options);
}
#endif

void
CQChartsSankeyEdgeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto *srcNode  = edge()->srcNode ();
  auto *destNode = edge()->destNode();

  //---

  ColorInd colorInd;

  if (edge()->pathId() >= 0) {
    const auto &pathIdMinMax = plot()->pathIdMinMax();

    if (pathIdMinMax.isSet())
      colorInd = ColorInd(edge()->pathId() - pathIdMinMax.min(),
                          pathIdMinMax.max() - pathIdMinMax.min() + 1);
  }

  //---

  // calc fill color
  QColor fc;

  if (! edge()->color().isValid()) {
    if (plot()->isBlendEdgeColor()) {
      auto fc1 = srcNode ->obj()->calcFillColor();
      auto fc2 = destNode->obj()->calcFillColor();

      fc = CQChartsUtil::blendColors(fc1, fc2, 0.5);
    }
    else
      fc = plot()->interpEdgeFillColor(colorInd);
  }
  else {
    fc = plot()->interpColor(edge()->color(), colorInd);
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

  plot()->setPenBrush(penBrush,
    PenData  (plot()->isEdgeStroked(), sc, plot()->edgeStrokeAlpha(),
              plot()->edgeStrokeWidth(), plot()->edgeStrokeDash()),
    BrushData(plot()->isEdgeFilled(), fc, plot()->edgeFillAlpha(),
              plot()->edgeFillPattern()));

  if (updateState)
    plot()->updateObjPenBrushState(this, penBrush);
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

CQChartsSankeyPlotGraph::
CQChartsSankeyPlotGraph(const Plot *plot) :
 plot_(plot)
{
}

void
CQChartsSankeyPlotGraph::
addNode(Node *node)
{
  nodes_.push_back(node);
}

//---

void
CQChartsSankeyPlotGraph::
addDepthNode(int depth, Node *node)
{
  depthNodesMap_[depth].nodes.push_back(node);
}

void
CQChartsSankeyPlotGraph::
addDepthSize(int depth, double size)
{
  depthNodesMap_[depth].size += size;
}

void
CQChartsSankeyPlotGraph::
addPosNode(Node *node)
{
  posNodesMap_[node->xpos()].push_back(node);
}

bool
CQChartsSankeyPlotGraph::
hasPosNodes(int pos) const
{
  return (posNodesMap_.find(pos) != posNodesMap_.end());
}

const CQChartsSankeyPlotGraph::Nodes &
CQChartsSankeyPlotGraph::
posNodes(int pos) const
{
  auto p = posNodesMap_.find(pos);
  assert(p != posNodesMap_.end());

  return (*p).second;
}

//---

void
CQChartsSankeyPlotGraph::
setRect(const BBox &rect)
{
  assert(rect.isSet());

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
    bbox += node->rect();

  rect_ = bbox;
}

//---

CQChartsSankeyPlotGraph::Nodes
CQChartsSankeyPlotGraph::
placeableNodes() const
{
  Nodes nodes;

  for (auto &node : this->nodes())
    nodes.push_back(node);

  return nodes;
}

void
CQChartsSankeyPlotGraph::
moveBy(const Point &delta)
{
  rect_.moveBy(delta);

  for (const auto &node : nodes())
    node->moveBy(delta);
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
}
