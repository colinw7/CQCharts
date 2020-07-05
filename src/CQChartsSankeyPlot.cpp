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

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

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
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsSankeyPlotType::
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
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
 CQChartsConnectionPlot(view, view->charts()->plotType("sankey"), model),
 CQChartsObjTextData      <CQChartsSankeyPlot>(this),
 CQChartsObjNodeShapeData <CQChartsSankeyPlot>(this),
 CQChartsObjEdgeShapeData <CQChartsSankeyPlot>(this),
 CQChartsObjGraphShapeData<CQChartsSankeyPlot>(this)
{
  NoUpdate noUpdate(this);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  //---

  CQChartsColor bg(CQChartsColor::Type::PALETTE);

  setNodeFilled(true);
  setNodeFillColor(bg);
  setNodeFillAlpha(CQChartsAlpha(1.0));

  setNodeStroked(true);
  setNodeStrokeAlpha(CQChartsAlpha(0.2));

  //---

  setEdgeFilled(true);
  setEdgeFillColor(bg);
  setEdgeFillAlpha(CQChartsAlpha(0.25));

  setEdgeStroked(true);
  setEdgeStrokeAlpha(CQChartsAlpha(0.2));

  //---

  setGraphFilled (false);
  setGraphStroked(false);

  //---

  addTitle();

  //---

  bbox_ = targetBBox_;

  setFitMargin(CQChartsPlotMargin(Length("5%"), Length("5%"), Length("5%"), Length("5%")));
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

void
CQChartsSankeyPlot::
setNodeXScaled(bool b)
{
  CQChartsUtil::testAndSet(nodeXScaled_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setNodeYScaled(bool b)
{
  CQChartsUtil::testAndSet(nodeYScaled_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setNodeShape(const NodeShape &s)
{
  CQChartsUtil::testAndSet(nodeShape_, s, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsSankeyPlot::
setEdgeShape(const EdgeShape &s)
{
  CQChartsUtil::testAndSet(edgeShape_, s, [&]() { updateObjs(); } );
}

void
CQChartsSankeyPlot::
setEdgeScaled(bool b)
{
  CQChartsUtil::testAndSet(edgeScaled_, b, [&]() { updateRangeAndObjs(); } );
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
setAdjustNodes(bool b)
{
  CQChartsUtil::testAndSet(adjustNodes_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setAutoCreateGraphs(bool b)
{
  CQChartsUtil::testAndSet(autoCreateGraphs_, b, [&]() { updateRangeAndObjs(); } );
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
  addProp("options", "adjustNodes"     , "adjustNodes"     , "Adjust node placement");
  addProp("options", "autoCreateGraphs", "autoCreateGraphs", "Auto create graphs");

  // node
  addProp("node", "nodeShape"  , "shapeType", "Node shape type");
  addProp("node", "nodeXMargin", "marginX"  , "Node X margin");
  addProp("node", "nodeYMargin", "marginY"  , "Node Y margin");
  addProp("node", "nodeWidth"  , "width"    , "Node width (in pixels)");
  addProp("node", "nodeXScaled", "scaleX"   , "Node is X scaled");
  addProp("node", "nodeYScaled", "scaleY"   , "Node is Y scaled");

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

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST);
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

#if 0
void
CQChartsSankeyPlot::
calcMaxNodeDepth()
{
  // set overall max node depth of all graphs (needed ?)
  maxNodeDepth_ = 0;

  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    maxNodeDepth_ = std::max(th->maxNodeDepth_, graph->maxNodeDepth());
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

  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    graph->updateRect();
  }
}

bool
CQChartsSankeyPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initHierObjs");

  //---

  CQChartsConnectionPlot::initHierObjs();

  return true;
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

  CQChartsConnectionPlot::initPathObjs();

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
      bool hasEdge = false;

      for (auto &destEdge : srcNode->destEdges()) {
        if (destEdge->destNode() == destNode) {
          hasEdge = true;
          break;
        }
      }

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

  CQChartsConnectionPlot::initFromToObjs();

  return true;
}

void
CQChartsSankeyPlot::
addFromToValue(const QString &fromStr, const QString &toStr, double value,
               const CQChartsNameValues &nameValues, const GroupData &groupData) const
{
  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  auto *srcNode = findNode(fromStr);

  // Just node
  if (toStr == "") {
    for (const auto &nv : nameValues.nameValues()) {
      QString value = nv.second.toString();

      if      (nv.first == "shape") {
        if      (value == "diamond")
          srcNode->setShapeType(CQChartsSankeyPlotNode::ShapeType::DIAMOND);
        else if (value == "box")
          srcNode->setShapeType(CQChartsSankeyPlotNode::ShapeType::BOX);
        else if (value == "polygon")
          srcNode->setShapeType(CQChartsSankeyPlotNode::ShapeType::POLYGON);
        else if (value == "circle")
          srcNode->setShapeType(CQChartsSankeyPlotNode::ShapeType::CIRCLE);
        else if (value == "doublecircle")
          srcNode->setShapeType(CQChartsSankeyPlotNode::ShapeType::DOUBLE_CIRCLE);
        else if (value == "record")
          srcNode->setShapeType(CQChartsSankeyPlotNode::ShapeType::BOX);
        else if (value == "plaintext")
          srcNode->setShapeType(CQChartsSankeyPlotNode::ShapeType::BOX);
        else
          srcNode->setShapeType(CQChartsSankeyPlotNode::ShapeType::BOX);
      }
      else if (nv.first == "num_sides") {
        bool ok;

        int n = value.toInt(&ok);

        if (ok)
          srcNode->setNumSides(n);
      }
      else if (nv.first == "label") {
        srcNode->setLabel(value);
      }
      else if (nv.first == "color") {
        srcNode->setColor(QColor(value));
      }
      else if (nv.first == "fillcolor") {
        QStringList colors = value.split(":");

        if (colors.length() > 0)
          srcNode->setColor(QColor(colors[0]));
      }
      else if (nv.first == "gradientangle") {
      }
    }

    //---

    QStringList groupNames = groupData.name.split("/");

    int graphId = -1, parentGraphId = -1;

    int ng = groupNames.length();

    if (ng > 0) {
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
    auto *destNode = findNode(toStr);

    auto *edge = createEdge(OptReal(value), srcNode, destNode);

    srcNode ->addDestEdge(edge, /*primary*/true );
    destNode->addSrcEdge (edge, /*primary*/false);

    for (const auto &nv : nameValues.nameValues()) {
      QString value = nv.second.toString();

      if      (nv.first == "shape") {
        if (value == "arrow")
          edge->setShapeType(CQChartsSankeyPlotEdge::ShapeType::ARROW);
      }
      else if (nv.first == "label") {
        edge->setLabel(value);
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

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsSankeyPlot *plot) :
     plot_(plot) {
      separator_ = (plot_->separator().length() ? plot_->separator()[0] : '/');
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // Get group value
      GroupData groupData(data.row);

      if (plot_->groupColumn().isValid()) {
        CQChartsModelIndex groupModelInd(data.row, plot_->groupColumn(), data.parent);

        plot_->groupColumnData(groupModelInd, groupData);
      }

      //---

      // Get link value
      CQChartsModelIndex linkModelInd(data.row, plot_->linkColumn(), data.parent);

      CQChartsNamePair namePair;

      if (plot_->linkColumnType() == ColumnType::NAME_PAIR) {
        bool ok;
        QVariant linkVar = plot_->modelValue(linkModelInd, ok);
        if (! ok) return addDataError(linkModelInd, "Invalid Link");

        namePair = linkVar.value<CQChartsNamePair>();
      }
      else {
        bool ok;
        QString linkStr = plot_->modelString(linkModelInd, ok);
        if (! ok) return addDataError(linkModelInd, "Invalid Link");

        namePair = CQChartsNamePair(linkStr, separator_);
      }

      if (! namePair.isValid())
        return addDataError(linkModelInd, "Invalid Link");

      //---

      // Get value value
      CQChartsModelIndex valueModelInd(data.row, plot_->valueColumn(), data.parent);

      bool ok1;
      double value = plot_->modelReal(valueModelInd, ok1);
      if (! ok1) return addDataError(valueModelInd, "Invalid Value");

      //---

      // Get name value
      CQChartsModelIndex nameInd;

      if (plot_->nameColumn().isValid())
        nameInd = CQChartsModelIndex(data.row, plot_->nameColumn(), data.parent);

      //---

      QString srcStr  = namePair.name1();
      QString destStr = namePair.name2();

      addConnection(srcStr, destStr, value, groupData, nameInd);

      return State::OK;
    }

   private:
    void addConnection(const QString &srcStr, const QString &destStr, double value,
                       const GroupData &groupData, const CQChartsModelIndex &nameInd) {
      auto *srcNode  = plot_->findNode(srcStr);
      auto *destNode = plot_->findNode(destStr);

    //assert(srcNode != destNode);

      auto *edge = plot_->createEdge(OptReal(value), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);

      destNode->setValue(OptReal(value));

      srcNode->setGroup(groupData.id);

      if (nameInd.isValid()) {
        auto nameInd1 = plot_->normalizeIndex(nameInd);

        srcNode->setInd(nameInd1);
      }
    }

    State addDataError(const CQChartsModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsSankeyPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsSankeyPlot* plot_      { nullptr };
    QChar                     separator_ { '/' };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  return true;
}

//---

bool
CQChartsSankeyPlot::
initConnectionObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initConnectionObjs");

  //---

  using Connections = CQChartsConnectionList::Connections;

  struct ConnectionsData {
    QModelIndex ind;
    int         node    { 0 };
    QString     name;
    GroupData   groupData;
    double      total   { 0.0 };
    Connections connections;
  };

  using IdConnectionsData = std::map<int,ConnectionsData>;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsSankeyPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get group value
      GroupData groupData(data.row);

      if (plot_->groupColumn().isValid()) {
        CQChartsModelIndex groupModelInd(data.row, plot_->groupColumn(), data.parent);

        plot_->groupColumnData(groupModelInd, groupData);
      }

      //---

      // get optional node id (default to row)
      CQChartsModelIndex nodeModelInd;

      int id = data.row;

      if (plot_->nodeColumn().isValid()) {
        nodeModelInd = CQChartsModelIndex(data.row, plot_->nodeColumn(), data.parent);

        bool ok2;
        id = (int) plot_->modelInteger(nodeModelInd, ok2);
        if (! ok2) return addDataError(nodeModelInd, "Non-integer node value");
      }

      //---

      // get connections
      ConnectionsData connectionsData;

      CQChartsModelIndex connectionsModelInd(data.row, plot_->connectionsColumn(), data.parent);

      if (plot_->connectionsColumnType() == ColumnType::CONNECTION_LIST) {
        bool ok3;
        QVariant connectionsVar = plot_->modelValue(connectionsModelInd, ok3);

        connectionsData.connections = connectionsVar.value<CQChartsConnectionList>().connections();
      }
      else {
        bool ok3;
        QString connectionsStr = plot_->modelString(connectionsModelInd, ok3);
        if (! ok3) return addDataError(connectionsModelInd, "Invalid connection string");

        CQChartsConnectionList::stringToConnections(connectionsStr, connectionsData.connections);
      }

      //----

      // get name
      QString name = QString("%1").arg(id);

      if (plot_->nameColumn().isValid()) {
        CQChartsModelIndex nameModelInd(data.row, plot_->nameColumn(), data.parent);

        bool ok4;
        name = plot_->modelString(nameModelInd, ok4);
        if (! ok4) return addDataError(nameModelInd, "Invalid name string");
      }

      //---

      // calc total
      double total = 0.0;

      for (const auto &connection : connectionsData.connections)
        total += connection.value;

      //---

      // return connections data
      if (nodeModelInd.isValid()) {
        auto nodeInd  = plot_->modelIndex(nodeModelInd);
        auto nodeInd1 = plot_->normalizeIndex(nodeInd);

        connectionsData.ind = nodeInd1;
      }

      connectionsData.node      = id;
      connectionsData.name      = name;
      connectionsData.groupData = groupData;
      connectionsData.total     = total;

      idConnectionsData_[connectionsData.node] = connectionsData;

      return State::OK;
    }

    const IdConnectionsData &idConnectionsData() const { return idConnectionsData_; }

   private:
    State addDataError(const CQChartsModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsSankeyPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsSankeyPlot* plot_ { nullptr };
    IdConnectionsData         idConnectionsData_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  const IdConnectionsData &idConnectionsData = visitor.idConnectionsData();

  for (const auto &idConnections : idConnectionsData) {
    int         id              = idConnections.first;
    const auto &connectionsData = idConnections.second;

    QString srcStr = QString("%1").arg(id);

    auto *srcNode = findNode(srcStr);

    srcNode->setName (connectionsData.name);
    srcNode->setGroup(connectionsData.groupData.id);

    for (const auto &connection : connectionsData.connections) {
      QString destStr = QString("%1").arg(connection.node);

      auto *destNode = findNode(destStr);

      auto *edge = createEdge(OptReal(connection.value), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);
    }
  }

  return true;
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
    srcNode->setGroup(tableConnectionData.group().i);

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

  //adjustGraphs();

  //---

  //th->setNodeYRange();

  //---

  addObjects(objs);
}

void
CQChartsSankeyPlot::
addObjects(PlotObjs &objs) const
{
  // add node objects (per graphs)
  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    if (graph->nodes().empty() || ! graph->rect().isValid())
      continue;

    //---

    for (auto *node : graph->nodes()) {
      auto *nodeObj = createObjFromNode(graph, node);

      objs.push_back(nodeObj);
    }

    //---

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

void
CQChartsSankeyPlot::
placeGraphs() const
{
  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    placeGraph(graph);
  }
}

// place nodes in graph
void
CQChartsSankeyPlot::
placeGraph(Graph *graph) const
{
  if (graph->isPlaced())
    return;

  //---

  // place children first
  for (const auto &child : graph->children())
    child->place();

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
    const Nodes &nodes = depthNodes.second.nodes;
    double       size  = depthNodes.second.size;

    graph->setMaxHeight(std::max(graph->maxHeight(), int(nodes.size())));
    graph->setTotalSize(std::max(graph->totalSize(), size));
  }

  //---

  // calc y value scale and margins to fit in bbox
  th->calcValueMarginScale(graph);

  // place node objects at each depth (xpos)
  placeDepthNodes(graph);

  //---

#if 0
  th->bbox_ = nodesBBox();

  th->calcValueMarginScale(graph);

  placeDepthNodes(graph);
#endif

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
createGraphs() const
{
  auto *th = const_cast<CQChartsSankeyPlot *>(this);

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
CQChartsSankeyPlot::
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
    NodeSet &nodeSet = nodeNodeSet[node];

    NodeQueue workSet;

    nodeSet.insert(node);

    workSet.push_back(node);

    while (! workSet.empty()) {
      Node *node = workSet.front();

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

CQChartsSankeyPlotGraph *
CQChartsSankeyPlot::
getGraph(int graphId, int parentGraphId) const
{
  auto pg = graphs_.find(graphId);

  if (pg == graphs_.end()) {
    auto *th = const_cast<CQChartsSankeyPlot *>(this);

    QString name = QString("%1").arg(graphId);

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
CQChartsSankeyPlot::
clearGraphs()
{
  for (auto &pg : graphs_)
    delete pg.second;

  graphs_.clear();
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
  graph->setValueMargin(graph->maxHeight() > 1 ? ys1/(graph->maxHeight() - 1) : 0.0);

  if (isNodeYScaled())
    graph->setValueScale(graph->totalSize() > 0.0 ? ys2/graph->totalSize() : 1.0);
  else
    graph->setValueScale(graph->maxHeight() > 0 ? ys2/graph->maxHeight() : 1.0);
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
    int          xpos  = depthNodes.first;
    const Nodes &nodes = depthNodes.second.nodes;

    placeDepthSubNodes(graph, xpos, nodes);
  }
}

void
CQChartsSankeyPlot::
placeDepthSubNodes(Graph *graph, int xpos, const Nodes &nodes) const
{
  double xmargin = calcNodeXMargin();

  // place nodes to fit in bbox
  double xs = bbox_.getWidth ();
  double ys = bbox_.getHeight();

#if 0
  if (! isNodeYScaled()) {
    xs = std::max(xs, ys);
    ys = xs;
  }
#endif

  double dx = 1.0;

  if (graph->maxNodeDepth() > 0) {
    if (isNodeXScaled())
      dx = xs/(graph->maxNodeDepth() + 1);
    else
      dx = xs/graph->maxNodeDepth();
  }

  double xm = pixelToWindowWidth (nodeWidth());
  double ym = pixelToWindowHeight(nodeWidth());

  if (isNodeXScaled()) {
    xm = dx - xmargin;
    ym = xm;
  }

  //---

  // get sum of margins nodes at depth (height always ys for ! isNodeYScaled() ?)
  double height = 0.0;

  if (isNodeYScaled())
    height = graph->valueMargin()*(int(nodes.size()) - 1);
  else
    height = graph->valueMargin()*(graph->maxHeight() - 1);

  // get sum of scaled values for nodes at depth
  if (isNodeYScaled()) {
    for (const auto &node : nodes)
      height += graph->valueScale()*node->edgeSum();
  }
  else {
    height += graph->maxHeight()*graph->valueScale();
  }

  //---

  // place top to bottom
  double y1 = bbox_.getYMax() - (ys - height)/2.0;

  //---

#if 0
  double nh = 0.0;

  if (! isNodeYScaled()) {
    if (height < ys) {
      nh     = ys/graph->maxHeight() - graph->valueMargin();
      height = ys;

      y1 = ys;
    }
    else
      nh = graph->valueScale();
  }
#else
  double nh = graph->valueScale();
#endif

  if (! isNodeYScaled()) {
    double dy = nh + graph->valueMargin();

    y1 = bbox_.getYMid() + dy*graph->maxHeight()/2.0;

    y1 -= dy*(graph->maxHeight() - nodes.size())/2.0;

    y1 -= graph->valueMargin()/2.0;
  }

  //---

  for (const auto &node : nodes) {
    // calc height
    double h = 0.0;

    if (isNodeYScaled())
      h = graph->valueScale()*node->edgeSum();
    else
      h = nh;

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

    double y11, y12;

    if (isNodeYScaled()) {
      y11 = yc - h/2.0;
      y12 = yc + h/2.0;
    }
    else {
      y11 = yc - ym/2.0;
      y12 = yc + ym/2.0;
    }

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

CQChartsSankeyPlot::NodeObj *
CQChartsSankeyPlot::
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
  nodeObj->setNumSides (node->numSides());
  nodeObj->setHierName (node->str  ());
  nodeObj->setName     (node->name ());

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
      if      (align() == CQChartsSankeyPlot::Align::SRC)
        xpos = srcDepth;
      else if (align() == CQChartsSankeyPlot::Align::DEST)
        xpos = graph->maxNodeDepth() - destDepth;
      else if (align() == CQChartsSankeyPlot::Align::JUSTIFY) {
        double f = 1.0*srcDepth/(srcDepth + destDepth);

        xpos = int(f*graph->maxNodeDepth());
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

  EdgeObj::ShapeType shapeType = (EdgeObj::ShapeType) edge->shapeType();

  if (shapeType == EdgeObj::ShapeType::NONE)
    shapeType = (EdgeObj::ShapeType) edgeShape();

  edgeObj->setShapeType(shapeType);

  edge->setObj(edgeObj);

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

    if      (align() == CQChartsSankeyPlot::Align::SRC)
      updateNodeDepth(srcDepth);
    else if (align() == CQChartsSankeyPlot::Align::DEST)
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

  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    Nodes nodes = graph->placeNodes();

    if (adjustGraphNodes(graph, nodes))
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
        std::cerr << "adjustNodeCenters (#" << pass + 1 << " Passes)\n";
        break;
      }
    }

    //---

    reorderNodeEdges(graph, nodes);
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
removeOverlaps(Graph *graph) const
{
  bool changed = false;

  double ym = pixelToWindowHeight(minNodeMargin());

  for (const auto &posNodes : graph->posNodesMap()) {
    const auto &nodes = posNodes.second;

    // get nodes sorted by y (max to min)
    PosNodeMap posNodeMap;

    createPosNodeMap(nodes, posNodeMap);

    //---

    // remove overlaps between nodes
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

    // move back inside bbox (needed ?)
    if (node1) {
      const auto &rect1 = node1->rect();

      if (rect1.getYMin() < bbox_.getYMin()) {
        double dy = bbox_.getYMin() - rect1.getYMin();

        for (const auto &node : nodes) {
          if (std::abs(dy) > 1E-6) {
            node->moveBy(Point(0, dy));
            changed = true;
          }
        }
      }
    }
  }

  return changed;
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
  BBox bbox;

  for (const auto &edge : node->srcEdges()) {
    if (edge->isSelf()) continue;

    auto *srcNode = edge->srcNode();
    if (! srcNode->isVisible()) continue;

    bbox += srcNode->rect();
  }

  for (const auto &edge : node->destEdges()) {
    if (edge->isSelf()) continue;

    auto *destNode = edge->destNode();
    if (! destNode->isVisible()) continue;

    bbox += destNode->rect();
  }

  if (! bbox.isValid())
    return false;

  double dy = bbox.getYMid() - node->rect().getYMid();

  if (std::abs(dy) < 1E-6)
    return false;

  node->moveBy(Point(0, dy));

  return true;
}

//---

void
CQChartsSankeyPlot::
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
    Node           *node     = pn.first;
    const NameData &nameData = pn.second;

    for (auto &pd : nameData) {
      std::cerr << node->name().toStdString() << " " << pd.first.toStdString() << "\n";
    }
  }
}

//---

CQChartsSankeyNodeObj *
CQChartsSankeyPlot::
createNodeObj(const BBox &rect, Node *node, const ColorInd &ind) const
{
  return new NodeObj(this, rect, node, ind);
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
addSrcEdge(CQChartsSankeyPlotEdge *edge, bool primary)
{
  edge->destNode()->parent_ = edge->srcNode();

  srcEdges_.push_back(edge);

  srcDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

void
CQChartsSankeyPlotNode::
addDestEdge(CQChartsSankeyPlotEdge *edge, bool primary)
{
  edge->destNode()->parent_ = edge->srcNode();

  destEdges_.push_back(edge);

  destDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
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

CQChartsSankeyPlotGraph *
CQChartsSankeyPlotNode::
graph() const
{
  if (graphId_ < 0)
    return nullptr;

  return plot_->getGraph(graphId_, -1);
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
setObj(CQChartsSankeyNodeObj *obj)
{
  obj_ = obj;
}

//---

void
CQChartsSankeyPlotNode::
moveBy(const Point &delta)
{
  rect_.moveBy(delta);

  //---

  if (obj_)
    obj_->moveBy(delta);
}

void
CQChartsSankeyPlotNode::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);

  //---

  if (obj_)
    obj_->scale(fx, fy);
}

//------

CQChartsSankeyPlotEdge::
CQChartsSankeyPlotEdge(const CQChartsSankeyPlot *plot, const OptReal &value,
                       CQChartsSankeyPlotNode *srcNode, CQChartsSankeyPlotNode *destNode) :
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
setObj(CQChartsSankeyEdgeObj *obj)
{
  obj_ = obj;
}

//------

CQChartsSankeyNodeObj::
CQChartsSankeyNodeObj(const CQChartsSankeyPlot *plot, const BBox &rect,
                      CQChartsSankeyPlotNode *node, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsSankeyPlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), node_(node)
{
  setDetailHint(DetailHint::MAJOR);

  setEditable(true);

  //---

  placeEdges();
}

void
CQChartsSankeyNodeObj::
placeEdges()
{
  double x1 = rect().getXMin();
  double x2 = rect().getXMax();
  double y1 = rect().getYMin();
  double y2 = rect().getYMax();

  srcEdgeRect_ .clear();
  destEdgeRect_.clear();

  if (node_->srcEdges().size() == 1) {
    auto *edge = *node_->srcEdges().begin();

    srcEdgeRect_[edge] = BBox(x1, y1, x2, y2);
  }
  else {
    double total = 0.0;

    for (const auto &edge : node_->srcEdges()) {
      if (edge->hasValue())
        total += edge->value().real();
    }

    double y3 = y2;

    for (const auto &edge : node_->srcEdges()) {
      if (! edge->hasValue()) {
        srcEdgeRect_[edge] = BBox();
        continue;
      }

      double h1 = (total > 0.0 ? (y2 - y1)*edge->value().real()/total : 0.0);

      double y4 = y3 - h1;

      auto p = srcEdgeRect_.find(edge);

      if (p == srcEdgeRect_.end())
        srcEdgeRect_[edge] = BBox(x1, y4, x2, y3);

      y3 = y4;
    }
  }

  //---

  if (node_->destEdges().size() == 1) {
    auto *edge = *node_->destEdges().begin();

    destEdgeRect_[edge] = BBox(x1, y1, x2, y2);
  }
  else {
    double total = 0.0;

    for (const auto &edge : node_->destEdges()) {
      if (edge->hasValue())
        total += edge->value().real();
    }

    double y3 = y2;

    for (const auto &edge : node_->destEdges()) {
      if (! edge->hasValue()) {
        destEdgeRect_[edge] = BBox();
        continue;
      }

      double h1 = (total > 0.0 ? (y2 - y1)*edge->value().real()/total : 0.0);

      double y4 = y3 - h1;

      auto p = destEdgeRect_.find(edge);

      if (p == destEdgeRect_.end())
        destEdgeRect_[edge] = BBox(x1, y4, x2, y3);

      y3 = y4;
    }
  }
}

CQChartsSankeyNodeObj::
~CQChartsSankeyNodeObj()
{
  if (node_)
    node_->setObj(nullptr);
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
CQChartsSankeyNodeObj::
moveBy(const Point &delta)
{
  //std::cerr << "  Move " << node()->str().toStdString() << " by " << delta.y << "\n";

  rect_.moveBy(delta);

  for (auto &edgeRect : srcEdgeRect_) {
    if (edgeRect.second.isSet())
      edgeRect.second.moveBy(delta);
  }

  for (auto &edgeRect : destEdgeRect_) {
    if (edgeRect.second.isSet())
      edgeRect.second.moveBy(delta);
  }
}

void
CQChartsSankeyNodeObj::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);

  placeEdges();
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

  auto *graph = node()->graph();

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

  for (auto &edgeRect : srcEdgeRect_)
    plotObjs.push_back(edgeRect.first->obj());

  for (auto &edgeRect : destEdgeRect_)
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

  plot_->setPen(penBrush, CQChartsPenData(true, c, plot_->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  QString str = node()->label();

  if (! str.length())
    str = node()->name();

  double ptw = fm.width(str);
  double tw  = plot_->pixelToWindowWidth(ptw);

  double xm = plot_->getCalcDataRange().xmid();

  double tx = (rect().getXMid() < xm - tw ?
    prect.getXMax() + textMargin : prect.getXMin() - textMargin - ptw);
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = device->pixelToWindow(Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = CQChartsAngle(0);
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();

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
CQChartsSankeyNodeObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  ColorInd ic = calcColorInd();

  QColor bc = plot_->interpNodeStrokeColor(ic);
  QColor fc = plot_->interpNodeFillColor  (ic);

  if (color_.isValid())
    fc = color_;

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (plot_->isNodeStroked(), bc, plot_->nodeStrokeAlpha(),
                      plot_->nodeStrokeWidth(), plot_->nodeStrokeDash()),
    CQChartsBrushData(plot_->isNodeFilled(), fc, plot_->nodeFillAlpha(),
                      plot_->nodeFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
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
CQChartsSankeyEdgeObj(const CQChartsSankeyPlot *plot, const BBox &rect,
                      CQChartsSankeyPlotEdge *edge) :
 CQChartsPlotObj(const_cast<CQChartsSankeyPlot *>(plot), rect), plot_(plot), edge_(edge)
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

  model->addProperty(path1, this, "shapeType")->setDesc("Shape");
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
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->setColorNames();

  //---

  // get connection rect of source and destination object
  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  bool isSelf = (srcObj == destObj);

  auto srcRect  = srcObj ->destEdgeRect(edge());
  auto destRect = destObj->srcEdgeRect (edge());

  if (! srcRect.isSet() || ! destRect.isSet())
    return;

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

    plot()->setUpdatesEnabled(false);

    if (! isSelf) {
      CQChartsArrow arrow(plot(), Point(x1, y1), Point(x2, y2));

      arrow.setRectilinear (true);
      arrow.setLineWidth   (CQChartsLength(8, CQChartsUnits::PIXEL));
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
      arrowData.setLineWidth(CQChartsLength(8, CQChartsUnits::PIXEL));

      double xr = srcRect.getWidth ()/2.0;
      double yr = srcRect.getHeight()/2.0;

      double a = M_PI/4.0;

      double c = cos(a);
      double s = sin(a);

      double xm = srcRect.getXMid();
      double ym = srcRect.getYMid();

      double yt = srcRect.getYMax() + yr/2.0;

      double x1 = xm - xr*c, y1 = ym + xr*s;
      double x2 = xm + xr*c, y2 = y1;

      QPainterPath path;

      path.moveTo (QPointF(x1, y1));
      path.cubicTo(QPointF(x1, yt), QPointF(x2, yt), QPointF(x2, y2));

      //---

      CQChartsArrow::pathAddArrows(plot(), path, arrowData, path_);

      device->drawPath(path_);
    }

    plot()->setUpdatesEnabled(true);
  }
  else {
    if (plot_->isEdgeScaled()) {
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
        // start y range from source node, and end y range fron dest node
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

        //---

        device->drawPath(path_);
      }
      else {
        double xr = srcRect.getWidth ()/2.0;
        double yr = srcRect.getHeight()/2.0;

        double a = M_PI/4.0;

        double c = cos(a);
        double s = sin(a);

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

        //---

        device->drawPath(path_);
      }
    }
  }

  device->resetColorNames();
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

  auto srcRect  = srcObj ->destEdgeRect(edge());
  auto destRect = destObj->srcEdgeRect (edge());

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

  plot_->setPen(penBrush, CQChartsPenData(true, c, plot_->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  QString str = edge()->label();

  if (! str.length())
    return;

  double ptw = fm.width(str);

  double tx = prect.getXMid() - textMargin - ptw/2.0;
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = device->pixelToWindow(Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = CQChartsAngle(0);
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();

  CQChartsDrawUtil::drawTextAtPoint(device, pt, str, options);
}

void
CQChartsSankeyEdgeObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto *srcNode  = edge()->srcNode ();
  auto *destNode = edge()->destNode();

  int numNodes = plot_->numNodes();

  ColorInd ic1(srcNode ->id(), numNodes);
  ColorInd ic2(destNode->id(), numNodes);

  QColor fc1 = plot_->interpEdgeFillColor(ic1);
  QColor fc2 = plot_->interpEdgeFillColor(ic2);

  QColor fc = CQChartsUtil::blendColors(fc1, fc2, 0.5);

  QColor sc1 = plot_->interpEdgeStrokeColor(ic1);
  QColor sc2 = plot_->interpEdgeStrokeColor(ic2);

  QColor sc = CQChartsUtil::blendColors(sc1, sc2, 0.5);

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (plot_->isEdgeStroked(), sc, plot_->edgeStrokeAlpha(),
                      plot_->edgeStrokeWidth(), plot_->edgeStrokeDash()),
    CQChartsBrushData(plot_->isEdgeFilled(), fc, plot_->edgeFillAlpha(),
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
CQChartsSankeyGraphObj(const CQChartsSankeyPlot *plot, const BBox &rect,
                       CQChartsSankeyPlotGraph *graph) :
 CQChartsPlotObj(const_cast<CQChartsSankeyPlot *>(plot), rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), graph_(graph)
{
  setDetailHint(DetailHint::MAJOR);

  setEditable(false);
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
    CQChartsPenData  (plot_->isGraphStroked(), bc, plot_->graphStrokeAlpha(),
                      plot_->graphStrokeWidth(), plot_->graphStrokeDash()),
    CQChartsBrushData(plot_->isGraphFilled(), fc, plot_->graphFillAlpha(),
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
  shapeType_ = ShapeType::BOX;
}

void
CQChartsSankeyPlotGraph::
addChild(Graph *graph)
{
  children_.push_back(graph);
}

void
CQChartsSankeyPlotGraph::
addNode(Node *node)
{
  srcDepth_  = -1;
  destDepth_ = -1;

  nodes_.push_back(node);
}

void
CQChartsSankeyPlotGraph::
removeAllNodes()
{
  srcDepth_  = -1;
  destDepth_ = -1;

  nodes_.clear();
}

//---

void
CQChartsSankeyPlotGraph::
setObj(CQChartsSankeyGraphObj *obj)
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
CQChartsSankeyPlotGraph::
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
CQChartsSankeyPlotGraph::
srcDepth() const
{
  if (srcDepth_ < 0) {
    auto *th = const_cast<CQChartsSankeyPlotGraph *>(this);

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
CQChartsSankeyPlotGraph::
destDepth() const
{
  if (destDepth_ < 0) {
    auto *th = const_cast<CQChartsSankeyPlotGraph *>(this);

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

CQChartsSankeyPlotGraph::Nodes
CQChartsSankeyPlotGraph::
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
  CQChartsSankeyPlotNode::moveBy(delta);

  for (const auto &child : children())
    child->moveBy(delta);

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
