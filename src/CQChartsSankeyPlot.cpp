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
setNodeScaled(bool b)
{
  CQChartsUtil::testAndSet(nodeScaled_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setNodeShape(const NodeShape &s)
{
  CQChartsUtil::testAndSet(nodeShape_, s, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsSankeyPlot::
setEdgeShape(const EdgeShape &s)
{
  CQChartsUtil::testAndSet(edgeShape_, s, [&]() { updateRangeAndObjs(); } );
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
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsConnectionPlot::addProperties();

  //---

  // node
  addProp("node", "nodeShape" , "shape" , "Node shape");
  addProp("node", "nodeMargin", "margin", "Node Margin factor");
  addProp("node", "nodeWidth" , "width" , "Node width (in pixels)");
  addProp("node", "nodeScaled", "scaled", "Node is scaled");

  // node style
  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  //---

  // edge
  addProp("edge", "edgeShape" , "shape" , "Edge shape");
  addProp("edge", "edgeScaled", "scaled", "Edge is scaled");

  // edge style
  addProp("edge/stroke", "edgeStroked", "visible", "Edge steoke visible");

  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  addProp("edge/fill", "edgeFilled", "visible", "Edit fill visible");

  addFillProperties("edge/fill", "edgeFill", "Edge");

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

  auto *th = const_cast<CQChartsSankeyPlot *>(this);

  th->nodeYSet_ = false;

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

  //---

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
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

  th->nodeYSet_ = false;

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
               const CQChartsNameValues &nameValues) const
{
  auto *srcNode = findNode(fromStr);

  // Just node
  if (toStr == "") {
    for (const auto &nv : nameValues.nameValues()) {
      QString value = nv.second.toString();

      if      (nv.first == "shape") {
        if      (value == "diamond")
          srcNode->setShape(CQChartsSankeyPlotNode::SHAPE_DIAMOND);
        else if (value == "box")
          srcNode->setShape(CQChartsSankeyPlotNode::SHAPE_BOX);
        else if (value == "polygon")
          srcNode->setShape(CQChartsSankeyPlotNode::SHAPE_POLYGON);
        else if (value == "circle")
          srcNode->setShape(CQChartsSankeyPlotNode::SHAPE_CIRCLE);
        else if (value == "doublecircle")
          srcNode->setShape(CQChartsSankeyPlotNode::SHAPE_DOUBLE_CIRCLE);
        else if (value == "record")
          srcNode->setShape(CQChartsSankeyPlotNode::SHAPE_BOX);
        else if (value == "plaintext")
          srcNode->setShape(CQChartsSankeyPlotNode::SHAPE_BOX);
        else
          srcNode->setShape(CQChartsSankeyPlotNode::SHAPE_BOX);
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
        if (value == "arrow")
          edge->setShape(CQChartsSankeyPlotEdge::SHAPE_ARROW);
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

      auto *edge = plot_->createEdge(OptReal(value), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);

      destNode->setValue(OptReal(value));

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
    if (! node->isVisible()) continue;

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

  if (isNodeScaled())
    th->valueScale_ = (totalSize > 0.0 ? ys2/totalSize : 1.0);
  else
    th->valueScale_ = (maxHeight_ > 0 ? ys2/maxHeight_ : 1.0);

  //---

  for (const auto &depthNodes : depthNodesMap) {
    createDepthNodes(depthNodes.second);
  }

  //--

  for (const auto &edge : edges_) {
    if (! edge->srcNode()->isVisible() || ! edge->destNode()->isVisible())
      continue;

    addEdgeObj(edge);
  }

  //---

  adjustNodes();

  //---

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    assert(node->obj());

    objs.push_back(node->obj());
  }

  for (const auto &edge : edges_) {
    if (! edge->srcNode()->isVisible() || ! edge->destNode()->isVisible())
      continue;

    assert(edge->obj());

    objs.push_back(edge->obj());
  }
}

void
CQChartsSankeyPlot::
createDepthNodes(const IndNodeMap &nodes) const
{
  double xs = bbox_.getWidth ();
  double ys = bbox_.getHeight();

  double dx = (maxNodeDepth() > 0 ? xs/maxNodeDepth() : 1.0);

  double xm = pixelToWindowWidth (nodeWidth());
//double ym = pixelToWindowHeight(nodeWidth());

  //---

  // get sum of margins nodes at depth
  double height = 0.0;

  if (isNodeScaled())
    height = valueMargin_*(int(nodes.size()) - 1);
  else
    height = valueMargin_*(maxHeight_ - 1);

  // get sum of scaled values for nodes at depth
  if (isNodeScaled()) {
    for (const auto &idNode : nodes) {
      auto *node = idNode.second;
      if (! node->isVisible()) continue;

      height += valueScale()*node->edgeSum();
    }
  }
  else {
    height += maxHeight_*valueScale();
  }

  //---

  int numNodes = this->numNodes();

  double y1 = bbox_.getYMax() - (ys - height)/2.0;

  for (const auto &idNode : nodes) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    // draw src box
    double h = 0.0;

    if (isNodeScaled())
      h = valueScale()*node->edgeSum();
    else {
      h = valueScale();

      h = std::min(0.8*dx, h);
    }

    if (h <= 0.0)
      h = 0.1;

    //---

    int srcDepth  = node->srcDepth ();
    int destDepth = node->destDepth();

    int xpos = node->calcXPos();

    double x = bbox_.getXMin() + xpos*dx;

    double y2 = y1 - h;

    CQChartsGeom::BBox rect;

    NodeObj::Shape shape = (NodeObj::Shape) node->shape();

    if (shape == NodeObj::SHAPE_NONE)
      shape = (NodeObj::Shape) nodeShape();

    if (shape == (NodeObj::Shape) NODE_SHAPE_DIAMOND ||
        shape == (NodeObj::Shape) NODE_SHAPE_BOX ||
        shape == (NodeObj::Shape) NODE_SHAPE_POLYGON ||
        shape == (NodeObj::Shape) NODE_SHAPE_CIRCLE ||
        shape == (NodeObj::Shape) NODE_SHAPE_DOUBLE_CIRCLE) {
      double y12 = (y1 + y2)/2.0;

      rect = CQChartsGeom::BBox(x - h/2.0, y12 - h/2.0, x + h/2.0, y12 + h/2.0);
    }
    else {
      if      (srcDepth == 0)
        rect = CQChartsGeom::BBox(x         , y1, x + xm    , y2); // no inputs (left align)
      else if (destDepth == 0)
        rect = CQChartsGeom::BBox(x - xm    , y1, x         , y2); // no outputs (right align)
      else
        rect = CQChartsGeom::BBox(x - xm/2.0, y1, x + xm/2.0, y2); // center align
    }

    ColorInd iv(node->id(), numNodes);

    auto *nodeObj = createNodeObj(rect, node, iv);

    nodeObj->setShape   (shape);
    nodeObj->setNumSides(node->numSides());
    nodeObj->setHierName(node->str  ());
    nodeObj->setName    (node->name ());

    if (node->hasValue())
      nodeObj->setValue(node->value().real());

    nodeObj->setDepth(node->depth());

    if (node->color().isValid())
      nodeObj->setColor(node->color());

    node->setObj(nodeObj);

    //---

    y1 = y2 - valueMargin_;
  }
}

CQChartsSankeyEdgeObj *
CQChartsSankeyPlot::
addEdgeObj(Edge *edge) const
{
  double xm = bbox_.getHeight()*edgeMargin_;
  double ym = bbox_.getWidth ()*edgeMargin_;

  CQChartsGeom::BBox rect(bbox_.getXMin() - xm, bbox_.getYMin() - ym,
                          bbox_.getXMax() + xm, bbox_.getYMax() + ym);

  auto *edgeObj = createEdgeObj(rect, edge);

  EdgeObj::Shape shape = (EdgeObj::Shape) edge->shape();

  if (shape == EdgeObj::SHAPE_NONE)
    shape = (EdgeObj::Shape) edgeShape();

  edgeObj->setShape(shape);

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
    if (! node->isVisible()) continue;

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
    if (! node->isVisible()) continue;

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

  th->nodeYSet_ = true;
}

void
CQChartsSankeyPlot::
initPosNodesMap()
{
  // get nodes by x pos
  posNodesMap_ = PosNodesMap();

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    posNodesMap_[node->xpos()][node->id()] = node;
  }
}

void
CQChartsSankeyPlot::
adjustNodeCenters()
{
  // adjust nodes so centered on src nodes

  // second to last
  int posNodesDepth = posNodesMap_.size();

  for (int xpos = 1; xpos <= posNodesDepth; ++xpos) {
    const auto &indNodeMap = posNodesMap_[xpos];

    for (const auto &idNode : indNodeMap) {
      auto *node = idNode.second;
      if (! node->isVisible()) continue;

      adjustNode(node);
    }
  }

  removeOverlaps();

  // second to last to first
  for (int xpos = posNodesDepth - 1; xpos >= 0; --xpos) {
    const auto &indNodeMap = posNodesMap_[xpos];

    for (const auto &idNode : indNodeMap) {
      auto *node = idNode.second;
      if (! node->isVisible()) continue;

      adjustNode(node);
    }
  }

  removeOverlaps();

  //---

  nodeYMin_ = 0.0;
  nodeYMax_ = 0.0;

  for (int xpos = 0; xpos <= posNodesDepth; ++xpos) {
    const auto &indNodeMap = posNodesMap_[xpos];

    for (const auto &idNode : indNodeMap) {
      auto *node = idNode.second;
      if (! node->isVisible()) continue;

      nodeYMin_ = std::min(nodeYMin_, node->obj()->rect().getYMin());
      nodeYMax_ = std::max(nodeYMax_, node->obj()->rect().getYMax());
    }
  }
}

void
CQChartsSankeyPlot::
removeOverlaps() const
{
  using PosNodeMap = std::map<double, Node *>;

  double ym = pixelToWindowHeight(minNodeMargin_);

  for (const auto &posNodes : posNodesMap_) {
    const IndNodeMap &indNodeMap = posNodes.second;

    // get nodes sorted by y (max to min)
    PosNodeMap posNodeMap;

    for (const auto &idNode : indNodeMap) {
      auto *node = idNode.second;
      if (! node->isVisible()) continue;

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
    Node *node1 = nullptr;

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
          if (! node->isVisible()) continue;

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
  using PosEdgeMap = std::map<double, Edge *>;

  for (const auto &idNode : indNodeMap_) {
    auto *node = idNode.second;
    if (! node->isVisible()) continue;

    //---

    PosEdgeMap srcPosEdgeMap;

    for (const auto &edge : node->srcEdges()) {
      auto *srcNode = edge->srcNode();
      if (! srcNode->isVisible()) continue;

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
      if (! destNode->isVisible()) continue;

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
adjustNode(Node *node) const
{
  CQChartsGeom::BBox bbox;

  for (const auto &edge : node->srcEdges()) {
    auto *srcNode = edge->srcNode();
    if (! srcNode->isVisible()) continue;

    bbox += srcNode->obj()->rect();
  }

  for (const auto &edge : node->destEdges()) {
    auto *destNode = edge->destNode();
    if (! destNode->isVisible()) continue;

    bbox += destNode->obj()->rect();
  }

  if (! bbox.isValid())
    return;

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
createNodeObj(const CQChartsGeom::BBox &rect, Node *node, const ColorInd &ind) const
{
  return new NodeObj(this, rect, node, ind);
}

CQChartsSankeyEdgeObj *
CQChartsSankeyPlot::
createEdgeObj(const CQChartsGeom::BBox &rect, Edge *edge) const
{
  return new EdgeObj(this, rect, edge);
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

void
CQChartsSankeyPlotNode::
setObj(CQChartsSankeyNodeObj *obj)
{
  obj_ = obj;
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

    for (const auto &edge : node_->srcEdges()) {
      if (edge->hasValue())
        total += edge->value().real();
    }

    double y3 = y2;

    for (const auto &edge : node_->srcEdges()) {
      if (! edge->hasValue()) {
        srcEdgeRect_[edge] = CQChartsGeom::BBox();
        continue;
      }

      double h1 = (total > 0.0 ? (y2 - y1)*edge->value().real()/total : 0.0);

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

    for (const auto &edge : node_->destEdges()) {
      if (edge->hasValue())
        total += edge->value().real();
    }

    double y3 = y2;

    for (const auto &edge : node->destEdges()) {
      if (! edge->hasValue()) {
        destEdgeRect_[edge] = CQChartsGeom::BBox();
        continue;
      }

      double h1 = (total > 0.0 ? (y2 - y1)*edge->value().real()/total : 0.0);

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
    if      (shape() == SHAPE_DIAMOND)
      device->drawDiamond(rect());
    else if (shape() == SHAPE_BOX)
      device->drawRect(rect());
    else if (shape() == SHAPE_POLYGON)
      device->drawPolygonSides(rect(), numSides());
    else if (shape() == SHAPE_CIRCLE)
      device->drawEllipse(rect());
    else if (shape() == SHAPE_DOUBLE_CIRCLE) {
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

  QPen pen;

  QColor c = plot_->interpTextColor(ic);

  plot_->setPen(pen, true, c, plot_->textAlpha());

  device->setPen(pen);

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

  auto pt = device->pixelToWindow(CQChartsGeom::Point(tx, ty));

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = CQChartsAngle(0);
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();

  if (shape() == SHAPE_DIAMOND || shape() == SHAPE_BOX ||
      shape() == SHAPE_POLYGON || shape() == SHAPE_CIRCLE ||
      shape() == SHAPE_DOUBLE_CIRCLE) {
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

  if (edge()->hasValue())
    tableTip.addTableRow("Value", edge()->value().real());

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

  // get connection rect of source and destination object
  auto *srcObj  = edge()->srcNode ()->obj();
  auto *destObj = edge()->destNode()->obj();

  bool isSelf = (srcObj == destObj);

  auto srcRect  = srcObj ->destEdgeRect(edge());
  auto destRect = destObj->srcEdgeRect (edge());

  if (! srcRect.isSet() || ! destRect.isSet())
    return;

  if (shape() == SHAPE_ARROW || ! plot_->isEdgeScaled()) {
    if (edge()->srcNode()->shape() == Node::SHAPE_DIAMOND ||
        edge()->srcNode()->shape() == Node::SHAPE_POLYGON ||
        edge()->srcNode()->shape() == Node::SHAPE_CIRCLE ||
        edge()->srcNode()->shape() == Node::SHAPE_DOUBLE_CIRCLE) {
      srcRect = srcObj->rect();
    }

    if (edge()->destNode()->shape() == Node::SHAPE_DIAMOND ||
        edge()->destNode()->shape() == Node::SHAPE_POLYGON ||
        edge()->destNode()->shape() == Node::SHAPE_CIRCLE ||
        edge()->destNode()->shape() == Node::SHAPE_DOUBLE_CIRCLE) {
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

  if (shape() == SHAPE_ARROW) {
    double y1 = srcRect .getYMid();
    double y2 = destRect.getYMid();

    if (swapped)
      std::swap(y1, y2);

    plot()->setUpdatesEnabled(false);

    CQChartsArrow arrow(plot(), CQChartsGeom::Point(x1, y1), CQChartsGeom::Point(x2, y2));

    arrow.setRectilinear (true);
    arrow.setLineWidth   (CQChartsLength(8, CQChartsUnits::PIXEL));
    arrow.setFrontVisible(false);
    arrow.setFilled      (true);
    arrow.setFillColor   (penBrush.brush.color());
    arrow.setStroked     (true);
    arrow.setStrokeColor (penBrush.pen.color());

    arrow.draw(device);

    path_ = arrow.drawnPath();

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

  QPen pen;

  QColor c = plot_->interpTextColor(ic);

  plot_->setPen(pen, true, c, plot_->textAlpha());

  device->setPen(pen);

  //---

  double textMargin = 4; // pixels

  QString str = edge()->label();

  if (! str.length())
    return;

  double ptw = fm.width(str);

  double tx = prect.getXMid() - textMargin - ptw/2.0;
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = device->pixelToWindow(CQChartsGeom::Point(tx, ty));

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
