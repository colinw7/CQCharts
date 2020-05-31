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
#include <CQChartsTip.h>
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
 CQChartsObjTextData     <CQChartsSankeyPlot>(this),
 CQChartsObjNodeShapeData<CQChartsSankeyPlot>(this),
 CQChartsObjEdgeShapeData<CQChartsSankeyPlot>(this)
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

void
CQChartsSankeyPlot::
setMaxDepth(int d)
{
  CQChartsUtil::testAndSet(maxDepth_, d, [&]() { updateRangeAndObjs(); } );
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

  CQChartsConnectionPlot::addProperties();

  //---

  // options
  addProp("options", "nodeMargin", "nodeMargin", "Node Margin factor");
  addProp("options", "nodeWidth" , "nodeWidth" , "Node width (in pixels)");
  addProp("options", "maxDepth"  , "maxDepth"  , "Max Node depth");

  // node style
  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  // edge style
  addProp("edge/stroke", "edgeStroked", "visible", "Edge steoke visible");

  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  addProp("edge/fill", "edgeFilled", "visible", "Edit fill visible");

  addFillProperties("edge/fill", "edgeFill", "Edge");

  // text
  addProp("text", "textVisible", "visible", "Text label visible");
  addProp("text", "align"      , "align"  , "Text label align");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST);
}

CQChartsGeom::Range
CQChartsSankeyPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::calcRange");

  CQChartsGeom::Range dataRange;

  auto *model = this->model().data();

  if (! model)
    return dataRange;

  if (bbox_.isSet()) {
    double xm = bbox_.getHeight()*boxMargin_;
    double ym = bbox_.getWidth ()*boxMargin_;

    dataRange.updateRange(bbox_.getXMin() - xm, bbox_.getYMin() - ym);
    dataRange.updateRange(bbox_.getXMax() + xm, bbox_.getYMax() + ym);
  }

  return dataRange;
}

CQChartsGeom::Range
CQChartsSankeyPlot::
getCalcDataRange() const
{
  auto range = CQChartsPlot::getCalcDataRange();

  if (nodeYSet_) {
    range.setBottom(nodeYMin_);
    range.setTop   (nodeYMax_);
  }

  return range;
}

//------

bool
CQChartsSankeyPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsSankeyPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  th->nodeYSet_= false;

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
    else
      rc = initTableObjs();
  }

  if (! rc)
    return false;

  //---

  createGraph(objs);

  return true;
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

  CQChartsSankeyPlotNode *srcNode  = nullptr;
  CQChartsSankeyPlotNode *destNode = nullptr;

  initHierObjsAddConnection(srcHierData.parentStr, destHierData.parentStr, srcDepth,
                            destHierData.total, srcNode, destNode);

  if (srcNode) {
    QString srcStr;

    if (! srcHierData.linkStrs.empty())
      srcStr = srcHierData.linkStrs.back();

    srcNode->setValue(destHierData.total);
    srcNode->setName (srcStr);
  }
}

void
CQChartsSankeyPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  int srcDepth = srcHierData.linkStrs.size();

  CQChartsSankeyPlotNode *srcNode  = nullptr;
  CQChartsSankeyPlotNode *destNode = nullptr;

  initHierObjsAddConnection(srcHierData.parentStr, destHierData.parentStr, srcDepth,
                            destHierData.total, srcNode, destNode);

  if (destNode) {
    QString destStr;

    if (! destHierData.linkStrs.empty())
      destStr = destHierData.linkStrs.back();

    destNode->setValue(destHierData.total);
    destNode->setName (destStr);
  }
}

void
CQChartsSankeyPlot::
initHierObjsAddConnection(const QString &srcStr, const QString &destStr, int srcDepth,
                          double value, CQChartsSankeyPlotNode* &srcNode,
                          CQChartsSankeyPlotNode* &destNode) const
{
  int destDepth = srcDepth + 1;

  if (maxDepth() <= 0 || srcDepth <= maxDepth())
    srcNode = findNode(srcStr);

  if (maxDepth() <= 0 || destDepth <= maxDepth())
    destNode = findNode(destStr);

  auto *edge = (srcNode && destNode ?  createEdge(value, srcNode, destNode) : nullptr);

  if (edge) {
    srcNode ->addDestEdge(edge);
    destNode->addSrcEdge (edge);
  }

  if (srcNode)
    srcNode->setDepth(srcDepth);

  if (destNode)
    destNode->setDepth(destDepth);
}

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
      int group = data.row;

      if (plot_->groupColumn().isValid()) {
        CQChartsModelIndex groupModelInd(data.row, plot_->groupColumn(), data.parent);

        bool ok1;
        group = (int) plot_->modelInteger(groupModelInd, ok1);

        if (! ok1)
          return addDataError(groupModelInd, "Non-integer group value");
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

      addConnection(srcStr, destStr, value, group, nameInd);

      return State::OK;
    }

   private:
    void addConnection(const QString &srcStr, const QString &destStr, double value,
                       int group, const CQChartsModelIndex &nameInd) {
      auto *srcNode  = plot_->findNode(srcStr);
      auto *destNode = plot_->findNode(destStr);

    //assert(srcNode != destNode);

      auto *edge = plot_->createEdge(value, srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);

      destNode->setValue(value);

      srcNode->setGroup(group);

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
    const CQChartsSankeyPlot *plot_      { nullptr };
    QChar                     separator_ { '/' };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  return true;
}

bool
CQChartsSankeyPlot::
initConnectionObjs() const
{
  CQPerfTrace trace("CQChartsSankeyPlot::initConnectionObjs");

  //---

  using Connections = CQChartsConnectionList::Connections;

  struct ConnectionsData {
    QModelIndex ind;
    int         node  { 0 };
    QString     name;
    int         group { 0 };
    double      total { 0.0 };
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
      int group = data.row;

      if (plot_->groupColumn().isValid()) {
        CQChartsModelIndex groupModelInd(data.row, plot_->groupColumn(), data.parent);

        bool ok1;
        group = (int) plot_->modelInteger(groupModelInd, ok1);
        if (! ok1) return addDataError(groupModelInd, "Non-integer group value");
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

      connectionsData.node  = id;
      connectionsData.name  = name;
      connectionsData.group = group;
      connectionsData.total = total;

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
    srcNode->setGroup(connectionsData.group);

    for (const auto &connection : connectionsData.connections) {
      QString destStr = QString("%1").arg(connection.node);

      auto *destNode = findNode(destStr);

      auto *edge = createEdge(connection.value, srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);
    }
  }

  return true;
}

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

      auto *edge = createEdge(value.toValue, srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);
    }
  }

  //---

  return true;
}

void
CQChartsSankeyPlot::
createGraph(PlotObjs &objs) const
{
  auto *th = const_cast<CQChartsSankeyPlot *>(this);

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
    auto *node = idNode.second;

    int xpos = node->calcXPos();

    depthSizeMap[xpos] += node->edgeSum();

    depthNodesMap[xpos][node->id()] = node;
  }

  //---

  // calc max height (fron node count) for each x
  th->maxHeight_ = 0;

  for (const auto &depthNodes : depthNodesMap)
    th->maxHeight_ = std::max(maxHeight_, int(depthNodes.second.size()));

  //---

  // calc max size (from value) for each x
  double totalSize = 0.0;

  for (const auto &depthSize : depthSizeMap)
    totalSize = std::max(totalSize, depthSize.second);

  //---

  double nodeMargin = std::min(std::min(this->nodeMargin(), 0.0), 1.0);

  double pixelNodeMargin = windowToPixelHeight(nodeMargin);

  if (pixelNodeMargin < minNodeMargin_)
    nodeMargin = pixelToWindowHeight(minNodeMargin_);

  double ys1 = nodeMargin*ys;
  double ys2 = ys - ys1;

  th->valueMargin_ = (maxHeight_ > 1 ? ys1/(maxHeight_ - 1) : 0.0);
  th->valueScale_  = (totalSize > 0.0 ? ys2/totalSize : 0.0);

  //---

  for (const auto &depthNodes : depthNodesMap)
    createDepthNodes(depthNodes.second);

  //--

  for (const auto &edge : edges_)
    addEdgeObj(edge);

  //---

  adjustNodes();

  //---

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;

    objs.push_back(node->obj());
  }

  for (const auto &edge : edges_)
    objs.push_back(edge->obj());
}

void
CQChartsSankeyPlot::
createDepthNodes(const IndNodeMap &nodes) const
{
  double xs = bbox_.getWidth ();
  double ys = bbox_.getHeight();

  double dx = xs/maxNodeDepth();

  double xm = pixelToWindowWidth(nodeWidth());

  //---

  // get sum of margins nodes at depth
  double height = valueMargin_*(int(nodes.size()) - 1);

  // get sum of scaled values for nodes at depth
  for (const auto &idNode : nodes) {
    auto *node = idNode.second;

    height += valueScale()*node->edgeSum();
  }

  //---

  int numNodes = this->numNodes();

  double y1 = bbox_.getYMax() - (ys - height)/2.0;

  for (const auto &idNode : nodes) {
    auto *node = idNode.second;

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
      rect = CQChartsGeom::BBox(x - xm/2.0, y1, x + xm/2.0, y2);

    ColorInd iv(node->id(), numNodes);

    auto *nodeObj = createNodeObj(rect, node, iv);

    nodeObj->setHierName(node->str  ());
    nodeObj->setName    (node->name ());
    nodeObj->setValue   (node->value());
    nodeObj->setDepth   (node->depth());

    node->setObj(nodeObj);

    //---

    y1 = y2 - valueMargin_;
  }
}

CQChartsSankeyEdgeObj *
CQChartsSankeyPlot::
addEdgeObj(CQChartsSankeyPlotEdge *edge) const
{
  double xm = bbox_.getHeight()*edgeMargin_;
  double ym = bbox_.getWidth ()*edgeMargin_;

  CQChartsGeom::BBox rect(bbox_.getXMin() - xm, bbox_.getYMin() - ym,
                          bbox_.getXMax() + xm, bbox_.getYMax() + ym);

  auto *edgeObj = createEdgeObj(rect, edge);

  edge->setObj(edgeObj);

  return edgeObj;
}

void
CQChartsSankeyPlot::
updateMaxDepth() const
{
  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  //---

  th->bbox_ = CQChartsGeom::BBox(-1.0, -1.0, 1.0, 1.0);

  int maxNodeDepth = 0;

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;

    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    if      (align() == CQChartsSankeyPlot::Align::SRC)
      maxNodeDepth = std::max(maxNodeDepth, srcDepth);
    else if (align() == CQChartsSankeyPlot::Align::DEST)
      maxNodeDepth = std::max(maxNodeDepth, destDepth);
    else
      maxNodeDepth = std::max(std::max(maxNodeDepth, srcDepth), destDepth);
  }

  th->maxNodeDepth_ = maxNodeDepth;
}

void
CQChartsSankeyPlot::
adjustNodes() const
{
  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  //---

  // update range
  th->bbox_ = CQChartsGeom::BBox();

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;

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

  //---

  th->nodeYSet_= true;
}

void
CQChartsSankeyPlot::
initPosNodesMap()
{
  // get nodes by x pos
  posNodesMap_ = PosNodesMap();

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;

    posNodesMap_[node->xpos()][node->id()] = node;
  }
}

void
CQChartsSankeyPlot::
adjustNodeCenters()
{
  // adjust nodes so centered on src nodes

  // second to last
  for (int xpos = 1; xpos <= maxNodeDepth(); ++xpos) {
    const IndNodeMap &indNodeMap = posNodesMap_[xpos];

    for (const auto &idNode : indNodeMap) {
      auto *node = idNode.second;

      adjustNode(node);
    }
  }

  removeOverlaps();

  // second to last to first
  for (int xpos = maxNodeDepth() - 1; xpos >= 0; --xpos) {
    const IndNodeMap &indNodeMap = posNodesMap_[xpos];

    for (const auto &idNode : indNodeMap) {
      auto *node = idNode.second;

      adjustNode(node);
    }
  }

  removeOverlaps();

  //---

  nodeYMin_ = 0.0;
  nodeYMax_ = 0.0;

  for (int xpos = 1; xpos <= maxNodeDepth(); ++xpos) {
    const IndNodeMap &indNodeMap = posNodesMap_[xpos];

    for (const auto &idNode : indNodeMap) {
      auto *node = idNode.second;

      nodeYMin_ = std::min(nodeYMin_, node->obj()->rect().getYMin());
      nodeYMax_ = std::max(nodeYMax_, node->obj()->rect().getYMax());
    }
  }
}

void
CQChartsSankeyPlot::
removeOverlaps() const
{
  using PosNodeMap = std::map<double,CQChartsSankeyPlotNode *>;

  double ym = pixelToWindowHeight(minNodeMargin_);

  for (const auto &posNodes : posNodesMap_) {
    const IndNodeMap &indNodeMap = posNodes.second;

    // get nodes sorted by y (max to min)
    PosNodeMap posNodeMap;

    for (const auto &idNode : indNodeMap) {
      auto *node = idNode.second;

      const auto &rect = node->obj()->rect();

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
      auto *node2 = posNode.second;

      if (node1) {
        const auto &rect1 = node1->obj()->rect();
        const auto &rect2 = node2->obj()->rect();

        if (rect2.getYMax() >= rect1.getYMin() - ym) {
          double dy = rect1.getYMin() - ym - rect2.getYMax();

          node2->obj()->moveBy(CQChartsGeom::Point(0, dy));
        }
      }

      node1 = node2;
    }

    // move back inside bbox
    if (node1) {
      const auto &rect1 = node1->obj()->rect();

      if (rect1.getYMin() < bbox_.getYMin()) {
        double dy = bbox_.getYMin() - rect1.getYMin();

        for (const auto &idNode : indNodeMap) {
          auto *node = idNode.second;

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
    auto *node = idNode.second;

    //---

    PosEdgeMap srcPosEdgeMap;

    for (const auto &edge : node->srcEdges()) {
      auto *srcNode = edge->srcNode();

      const auto &rect = srcNode->obj()->rect();

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
      auto *destNode = edge->destNode();

      const auto &rect = destNode->obj()->rect();

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
    auto *srcNode = edge->srcNode();

    bbox += srcNode->obj()->rect();
  }

  for (const auto &edge : node->destEdges()) {
    auto *destNode = edge->destNode();

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

  auto *node = new CQChartsSankeyPlotNode(this, name);

  node->setId(nameNodeMap_.size());

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(name, node));

  th->indNodeMap_[node->id()] = node;

  node->setName(name);

  return (*p1).second;
}

CQChartsSankeyPlotEdge *
CQChartsSankeyPlot::
createEdge(double value, CQChartsSankeyPlotNode *srcNode, CQChartsSankeyPlotNode *destNode) const
{
  assert(srcNode && destNode);

  auto *edge = new CQChartsSankeyPlotEdge(this, value, srcNode, destNode);

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

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

//---

CQChartsSankeyNodeObj *
CQChartsSankeyPlot::
createNodeObj(const CQChartsGeom::BBox &rect, CQChartsSankeyPlotNode *node,
              const ColorInd &ind) const
{
  return new CQChartsSankeyNodeObj(this, rect, node, ind);
}

CQChartsSankeyEdgeObj *
CQChartsSankeyPlot::
createEdgeObj(const CQChartsGeom::BBox &rect, CQChartsSankeyPlotEdge *edge) const
{
  return new CQChartsSankeyEdgeObj(this, rect, edge);
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
  if (depth() >= 0)
    return depth() - 1;

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

  auto *th = const_cast<CQChartsSankeyPlotNode *>(this);

  if (srcEdges_.empty()) {
    th->srcDepth_ = 0;
  }
  else {
    int depth = 0;

    for (const auto &edge : srcEdges_) {
      auto *node = edge->srcNode();

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
  if (depth() >= 0)
    return depth() + 1;

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

  auto *th = const_cast<CQChartsSankeyPlotNode *>(this);

  if (destEdges_.empty()) {
    th->destDepth_ = 0;
  }
  else {
    int depth = 0;

    for (const auto &edge : destEdges_) {
      auto *node = edge->destNode();

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
  int xpos = 0;

  if (depth() >= 0) {
    xpos = depth();
  }
  else {
    int srcDepth  = this->srcDepth ();
    int destDepth = this->destDepth();

    if      (srcDepth == 0)
      xpos = 0;
    else if (destDepth == 0)
      xpos = plot_->maxNodeDepth();
    else {
      if      (plot_->align() == CQChartsSankeyPlot::Align::SRC)
        xpos = srcDepth;
      else if (plot_->align() == CQChartsSankeyPlot::Align::DEST)
        xpos = plot_->maxNodeDepth() - destDepth;
      else if (plot_->align() == CQChartsSankeyPlot::Align::JUSTIFY) {
        double f = 1.0*srcDepth/(srcDepth + destDepth);

        xpos = int(f*plot_->maxNodeDepth());
      }
    }
  }

  //--

  auto *th = const_cast<CQChartsSankeyPlotNode *>(this);

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
  setDetailHint(DetailHint::MAJOR);

  //---

  double x1 = rect.getXMin();
  double x2 = rect.getXMax();
  double y1 = rect.getYMin();
  double y2 = rect.getYMax();

  if (node_->srcEdges().size() == 1) {
    auto *edge = *node_->srcEdges().begin();

    srcEdgeRect_[edge] = CQChartsGeom::BBox(x1, y1, x2, y2);
  }
  else {
    double total = 0.0;

    for (const auto &edge : node_->srcEdges())
      total += edge->value();

    double y3 = y2;

    for (const auto &edge : node_->srcEdges()) {
      double h1 = (y2 - y1)*edge->value()/total;

      double y4 = y3 - h1;

      auto p = srcEdgeRect_.find(edge);

      if (p == srcEdgeRect_.end())
        srcEdgeRect_[edge] = CQChartsGeom::BBox(x1, y4, x2, y3);

      y3 = y4;
    }
  }

  //---

  if (node_->destEdges().size() == 1) {
    auto *edge = *node_->destEdges().begin();

    destEdgeRect_[edge] = CQChartsGeom::BBox(x1, y1, x2, y2);
  }
  else {
    double total = 0.0;

    for (const auto &edge : node_->destEdges())
      total += edge->value();

    double y3 = y2;

    for (const auto &edge : node->destEdges()) {
      double h1 = (y2 - y1)*edge->value()/total;

      double y4 = y3 - h1;

      auto p = destEdgeRect_.find(edge);

      if (p == destEdgeRect_.end())
        destEdgeRect_[edge] = CQChartsGeom::BBox(x1, y4, x2, y3);

      y3 = y4;
    }
  }
}

QString
CQChartsSankeyNodeObj::
calcId() const
{
  //double value = node_->edgeSum();

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

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
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
  double x1 = rect().getXMin(), y1 = rect().getYMin();
  double x2 = rect().getXMax(), y2 = rect().getYMax();

  device->drawRect(CQChartsGeom::BBox(x1, y1, x2, y2));

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

  QPen pen;

  QColor c = plot_->interpTextColor(ic);

  plot_->setPen(pen, true, c, plot_->textAlpha());

  device->setPen(pen);

  //---

  double textMargin = 4; // pixels

  QString str = node_->name();

  double tx = (rect().getXMid() < 0.5 ? prect.getXMax() + textMargin :
                                        prect.getXMin() - textMargin - fm.width(str));
  double ty = prect.getYMid() + fm.ascent()/2;

  auto pt = device->pixelToWindow(CQChartsGeom::Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = CQChartsAngle(0);
  options.align         = Qt::AlignLeft;
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();

  CQChartsDrawUtil::drawTextAtPoint(device, pt, str, options);
}

void
CQChartsSankeyNodeObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  ColorInd ic = calcColorInd();

  QColor bc = plot_->interpNodeStrokeColor(ic);
  QColor fc = plot_->interpNodeFillColor  (ic);

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
CQChartsSankeyEdgeObj(const CQChartsSankeyPlot *plot, const CQChartsGeom::BBox &rect,
                      CQChartsSankeyPlotEdge *edge) :
 CQChartsPlotObj(const_cast<CQChartsSankeyPlot *>(plot), rect), plot_(plot), edge_(edge)
{
  setDetailHint(DetailHint::MAJOR);
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
  tableTip.addTableRow("Value", edge()->value());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsSankeyEdgeObj::
inside(const CQChartsGeom::Point &p) const
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

  // draw edge
  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  const auto &srcRect  = srcObj ->destEdgeRect(edge());
  const auto &destRect = destObj->srcEdgeRect (edge());

  // x from right of source rect to left of dest rect
  double x1 = srcRect .getXMax(), x2 = destRect.getXMin();

  // start y range from source node, and end y range fron dest node
  double y11 = srcRect .getYMax(), y12 = srcRect .getYMin();
  double y21 = destRect.getYMax(), y22 = destRect.getYMin();

  path_ = QPainterPath();

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

  device->resetColorNames();
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

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.value = " << edge()->value() << ";\n";
}
