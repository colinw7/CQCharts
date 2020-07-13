#include <CQChartsAdjacencyPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsNamePair.h>
#include <CQChartsValueSet.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

CQChartsAdjacencyPlotType::
CQChartsAdjacencyPlotType()
{
}

void
CQChartsAdjacencyPlotType::
addParameters()
{
  CQChartsConnectionPlotType::addParameters();
}

QString
CQChartsAdjacencyPlotType::
description() const
{
  auto B    = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto PARM = [](const QString &str) { return CQChartsHtml::Str::angled(str); };
  auto LI   = [](const QString &str) { return CQChartsHtml::Str(str); };
//auto BR   = []() { return CQChartsHtml::Str(CQChartsHtml::Str::Type::BR); };
  auto IMG  = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Adjacency Plot").
    h3("Summary").
     p("Draws connectivity information between two different sets of data as a "
       "matrix where the color of the cells represents the group and connectivity.").
    h3("Columns").
     p("Connection information can be supplied using:").
     ul({ LI("A list of connections in the " + B("Connections") + " column with the "
             "associated node numbers in the " + B("Node") + " column."),
          LI("A link using " + B("Link") + " column and a value using the " +
             B("Value") + " column.") }).
     p("The connections column is in the form {{" + PARM("id") + " " + PARM("value") + "} ...}.").
     p("The link column is in the form " + PARM("id1") + "/" + PARM("id2")).
     p("The column id is taken from the " + B("Id") + " column and an optional "
       "name for the id can be supplied in the " + B("Name") + " column.").
     p("The group is specified using the " + B("Group") + " column.").
    h3("Options").
     p("The nodes can be sorted by group, name or value using the " + B("sortType") + " option").
     p("The margin around the plot can be specified using the " + B("margin") + " option").
    h3("Styling").
     p("The styling (fill, stroke) of the connection cells, empty (no connection) cell "
       "and background can be set").
    h3("Limitations").
     p("The plot does not support axes, key or logarithmic scales.").
    h3("Example").
     p(IMG("images/adjacency.png"));
}

bool
CQChartsAdjacencyPlotType::
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsAdjacencyPlotType::
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
}

CQChartsPlot *
CQChartsAdjacencyPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsAdjacencyPlot(view, model);
}

//------

CQChartsAdjacencyPlot::
CQChartsAdjacencyPlot(CQChartsView *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("adjacency"), model),
 CQChartsObjBackgroundFillData<CQChartsAdjacencyPlot>(this),
 CQChartsObjShapeData         <CQChartsAdjacencyPlot>(this),
 CQChartsObjTextData          <CQChartsAdjacencyPlot>(this),
 CQChartsObjEmptyCellShapeData<CQChartsAdjacencyPlot>(this)
{
  NoUpdate noUpdate(this);

  setBackgroundFillColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.2));

  setStrokeColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1.0));
  setStrokeAlpha(CQChartsAlpha(0.5));

  setEmptyCellFillColor  (CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.1));
  setEmptyCellStrokeColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.2));

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setOuterMargin(CQChartsPlotMargin(0, 0, 0, 0));

  addTitle();
}

CQChartsAdjacencyPlot::
~CQChartsAdjacencyPlot()
{
  clearNodes();
}

void
CQChartsAdjacencyPlot::
clearNodes()
{
  for (auto &pnode : nodes_)
    delete pnode.second;

  nodes_.clear();

  nameNodeMap_.clear();

  sortedNodes_.clear();
}

//---

void
CQChartsAdjacencyPlot::
setSortType(const SortType &t)
{
  CQChartsUtil::testAndSet(sortType_, t, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setForceDiagonal(bool b)
{
  CQChartsUtil::testAndSet(forceDiagonal_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setBgMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(bgMargin_, l, [&]() { updateObjs(); } );
}

//---

void
CQChartsAdjacencyPlot::
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
  addProp("options", "sortType"     , "sort"         , "Sort type");
  addProp("options", "forceDiagonal", "forceDiagonal", "Force nodes on diagonal");
  addProp("options", "bgMargin"     , "margin"       , "Background margin");

  // background
  addFillProperties("background/fill", "backgroundFill", "Background");

  // cell style
  addFillProperties("cell/fill"  , "fill"  , "Cell");
  addLineProperties("cell/stroke", "stroke", "Cell");

  addProp("cell/stroke", "cornerSize", "cornerSize", "Cell box corner size");

  // empty cell style
  addFillProperties("emptyCell/fill"  , "emptyCellFill"  , "Empty cell");
  addLineProperties("emptyCell/stroke", "emptyCellStroke", "Empty cell");

  addProp("emptyCell/stroke", "cornerSize", "cornerSize", "Empty cell box corner size");

  // text
  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST);
}

CQChartsGeom::Range
CQChartsAdjacencyPlot::
calcRange() const
{
  // base range always (0,0) - (1,1)
  Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(1.0, 1.0);

  // adjust for equal scale
  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

//------

bool
CQChartsAdjacencyPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->clearErrors();

  //---

  th->clearNodes();

  //---

  th->setInsideObj(nullptr);

  //---

  // check columns
  if (! checkColumns())
    return false;

  //---

  // create objects
  bool rc      = true;
  bool addObjs = true;

  if (isHierarchical())
    rc = initHierObjs();
  else {
    if      (linkColumn().isValid() && valueColumn().isValid())
      rc = initLinkObjs();
    else if (connectionsColumn().isValid()) {
      rc = initConnectionObjs(objs);

      addObjs = false;
    }
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

  if (addObjs) {
    th->filterObjs();

    createNameNodeObjs(objs);
  }

  return true;
}

bool
CQChartsAdjacencyPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::initHierObjs");

  return CQChartsConnectionPlot::initHierObjs();
}

void
CQChartsAdjacencyPlot::
initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  initHierObjsAddConnection(srcHierData .parentStr, srcHierData .total,
                            destHierData.parentStr, destHierData.total);
}

void
CQChartsAdjacencyPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  initHierObjsAddConnection(srcHierData .parentStr, srcHierData .total,
                            destHierData.parentStr, destHierData.total);
}

void
CQChartsAdjacencyPlot::
initHierObjsAddConnection(const QString &srcStr, double /*srcValue*/,
                          const QString &destStr, double destValue) const
{
  // get src and dest nodes
  auto *srcNode  = findNode(srcStr);
  auto *destNode = findNode(destStr);

  assert(srcNode != destNode);

  // create link from src to dest for value
  // (hier always symmetric)
  if (! srcNode->hasNode(destNode))
    srcNode->addEdge(destNode, OptReal(destValue));

  if (! destNode->hasNode(srcNode))
    destNode->addEdge(srcNode, OptReal(destValue));
}

//---

bool
CQChartsAdjacencyPlot::
initPathObjs() const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::initPathObjs");

  //---

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->maxNodeDepth_ = 0;

  //--

  CQChartsConnectionPlot::initPathObjs();

  //---

  if (isPropagate())
    th->propagatePathValues();

  //---

  return true;
}

void
CQChartsAdjacencyPlot::
addPathValue(const QStringList &pathStrs, double value) const
{
  int n = pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  QChar separator = (this->separator().length() ? this->separator()[0] : '/');

  QString path1 = pathStrs[0];

  for (int i = 1; i < n; ++i) {
    QString path2 = path1 + separator + pathStrs[i];

    auto *srcNode  = findNode(path1);
    auto *destNode = findNode(path2);

    srcNode ->setLabel(pathStrs[i - 1]);
    destNode->setLabel(pathStrs[i    ]);

    srcNode->setDepth(i - 1);
    srcNode->setGroup(i - 1);

    destNode->setDepth(i);
    destNode->setGroup(i);

    if (i < n - 1) {
      if (! srcNode->hasNode(destNode)) {
        srcNode->addEdge(destNode);

        destNode->setParent(srcNode);
      }
    }
    else {
      srcNode->addEdge(destNode, OptReal(value));

      destNode->setParent(srcNode);
      destNode->setValue (OptReal(value));
    }

    path1 = path2;
  }
}

void
CQChartsAdjacencyPlot::
propagatePathValues()
{
  // propagate node value up through edges and parent nodes
  for (int depth = maxNodeDepth_; depth >= 0; --depth) {
    for (auto &p : nameNodeMap_) {
      auto *node = p.second;
      if (node->depth() != depth) continue;

      // set node value from sum of dest values
      if (! node->hasValue()) {
        if (! node->edges().empty()) {
          OptReal sum;

          for (const auto &pn : node->edges()) {
            const auto &edgeData = pn.second;

            if (edgeData.value.isSet()) {
              double value = edgeData.value.real();

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
        auto *parentNode = node->parent();

        if (parentNode)
          parentNode->setEdgeValue(node, node->value());
      }
    }
  }
}

//---

bool
CQChartsAdjacencyPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::initFromToObjs");

  CQChartsConnectionPlot::initFromToObjs();

  return true;
}

void
CQChartsAdjacencyPlot::
addFromToValue(const QString &fromStr, const QString &toStr, double value,
               const CQChartsNameValues &nameValues, const GroupData &) const
{
  auto *srcNode = findNode(fromStr);

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
        //srcNode->setColor(QColor(value));
      }
    }
  }
  else {
    auto *destNode = findNode(toStr);

    srcNode->addEdge(destNode, OptReal(value));

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

void
CQChartsAdjacencyPlot::
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

bool
CQChartsAdjacencyPlot::
initLinkObjs() const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::initLinkObjs");

  return CQChartsConnectionPlot::initLinkObjs();
}

void
CQChartsAdjacencyPlot::
addLinkConnection(const LinkConnectionData &linkConnectionData) const
{
  // Get group value
  int group = linkConnectionData.valueModelInd.row();

  if (groupColumn().isValid()) {
    bool ok1;

    group = (int) modelInteger(linkConnectionData.groupModelInd, ok1);

    if (! ok1) {
      auto *th = const_cast<CQChartsAdjacencyPlot *>(this);
      th->addDataError(linkConnectionData.groupModelInd, "Non-integer group value");
      return;
    }
  }

  //---

  ModelIndex modelInd = linkConnectionData.nameModelInd;

  if (! modelInd.isValid())
    modelInd = linkConnectionData.linkModelInd;

  if (! modelInd.isValid())
    modelInd = linkConnectionData.valueModelInd;

  //---

  auto *srcNode  = findNode(linkConnectionData.srcStr);
  auto *destNode = findNode(linkConnectionData.destStr);
//assert(srcNode != destNode);

  if (! srcNode->hasNode(destNode))
    srcNode->addEdge(destNode, OptReal(linkConnectionData.value));

  // connectional is directional (optional ?)
  if (isSymmetric()) {
    if (! destNode->hasNode(srcNode))
      destNode->addEdge(srcNode, OptReal(linkConnectionData.value));
  }

  srcNode->setGroup(group);

  if (modelInd.isValid()) {
    auto modelInd1 = normalizeIndex(modelInd);

    srcNode ->setInd(destNode->id(), modelInd1);
    destNode->setInd(srcNode ->id(), modelInd1);
  }
}

//------

bool
CQChartsAdjacencyPlot::
initConnectionObjs(PlotObjs &objs) const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ConnectionsData connections;

      if (! plot_->getRowConnections(data, connections))
        return State::SKIP;

      idConnections_[connections.node] = connections;

      return State::OK;
    }

    const IdConnectionsData &idConnections() const { return idConnections_; }

   private:
    const CQChartsAdjacencyPlot* plot_ { nullptr };
    IdConnectionsData            idConnections_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  //---

  const IdConnectionsData &idConnectionsData = visitor.idConnections();

  //---

  for (const auto &idConnections : idConnectionsData) {
    int         id    = idConnections.first;
    const auto &ind   = idConnections.second.ind;
    const auto &name  = idConnections.second.name;
    int         group = idConnections.second.group;

    auto *node = new CQChartsAdjacencyNode(id, name, group);

    if (ind.isValid())
      node->setInd(0, ind);

    th->nodes_[id] = node;
  }

  //---

  for (const auto &idConnections : idConnectionsData) {
    int                    id          = idConnections.first;
    const ConnectionsData &connections = idConnections.second;

    auto node = th->nodes_[id];

    for (const auto &connection : connections.connections) {
      auto node1 = th->nodes_[connection.node];

      node->addEdge(node1, OptReal(connection.value));
    }
  }

  //---

  sortNodes(nodes_, th->sortedNodes_, th->nodeData_);

  //---

  double xb = lengthPlotWidth (bgMargin());
  double yb = lengthPlotHeight(bgMargin());

  th->nodeData_.maxLen = 0;

  for (auto &node1 : sortedNodes_) {
    th->nodeData_.maxLen = std::max(th->nodeData_.maxLen, int(node1->name().size()));
  }

  //---

  if (factor_ < 0.0)
    th->initFactor();

  //---

  int nn = numVisibleNodes();

  if (nn + maxLen()*factor_ > 0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*factor_*scale();

  //---

  double equalValue = 0.0;

  double y = 1.0 - tsize - yb;

  for (auto &node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto &node2 : sortedNodes_) {
      double value = node1->edgeValue(node2, equalValue);

      // skip unconnected
      bool connected;

      if (node1 == node2)
        connected = (value > 0.0 || isForceDiagonal());
      else
        connected = ! CMathUtil::isZero(value);

      if (connected) {
        BBox bbox(x, y - scale(), x + scale(), y);

        ColorInd ig(node1->group(), maxGroup() + 1);

        auto *obj = th->createCellObj(node1, node2, value, bbox, ig);

        objs.push_back(obj);
      }

      x += scale();
    }

    y -= scale();
  }

  //---

  return true;
}

bool
CQChartsAdjacencyPlot::
initTableObjs() const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::initTableObjs");

  //---

  TableConnectionDatas tableConnectionDatas;
  TableConnectionInfo  tableConnectionInfo;

  if (! processTableModel(tableConnectionDatas, tableConnectionInfo))
    return false;

  //---

  int nv = tableConnectionDatas.size();

  for (int row = 0; row < nv; ++row) {
    const auto &tableConnectionData = tableConnectionDatas[row];

    QString srcStr = QString("%1").arg(tableConnectionData.from());

    auto *srcNode = findNode(srcStr);

    srcNode->setName (tableConnectionData.name());
    srcNode->setGroup(tableConnectionData.group().ig);

    for (const auto &value : tableConnectionData.values()) {
      QString destStr = QString("%1").arg(value.to);

      auto *destNode = findNode(destStr);

      if (! srcNode->hasNode(destNode))
        srcNode->addEdge(destNode, OptReal(value.value));
    }
  }

  return true;
}

//---

bool
CQChartsAdjacencyPlot::
getRowConnections(const ModelVisitor::VisitData &data, ConnectionsData &connectionsData) const
{
  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  //---

  // get optional group
  int group = data.row;

  if (groupColumn().isValid()) {
    ModelIndex groupInd(th, data.row, groupColumn(), data.parent);

    bool ok1;

    int group1 = (int) modelInteger(groupInd, ok1);

    if (ok1)
      group = group1;
  }

  //---

  // get optional node id (default to row)
  ModelIndex nodeModelInd;

  int id = data.row;

  if (nodeColumn().isValid()) {
    nodeModelInd = ModelIndex(th, data.row, nodeColumn(), data.parent);

    bool ok2;
    id = (int) modelInteger(nodeModelInd, ok2);
    if (! ok2) return th->addDataError(nodeModelInd, "Non-integer node value");
  }

  //---

  // get connections
  ModelIndex connectionsInd(th, data.row, connectionsColumn(), data.parent);

  if (connectionsColumnType() == ColumnType::CONNECTION_LIST) {
    bool ok3;
    QVariant connectionsVar = modelValue(connectionsInd, ok3);

    connectionsData.connections = connectionsVar.value<CQChartsConnectionList>().connections();
  }
  else {
    bool ok3;
    QString connectionsStr = modelString(connectionsInd, ok3);
    if (! ok3) return false;

    CQChartsConnectionList::stringToConnections(connectionsStr, connectionsData.connections);
  }

  //---

  // get optional name
  ModelIndex nameModelInd;

  QString name = QString("%1").arg(id);

  if (nameColumn().isValid()) {
    nameModelInd = ModelIndex(th, data.row, nameColumn(), data.parent);

    bool ok4;
    name = modelString(nameModelInd, ok4);
    if (! ok4) return th->addDataError(nameModelInd, "Invalid name string");
  }

  //---

  // return connections data
  if (nodeModelInd.isValid()) {
    auto nodeModelInd1 = normalizeIndex(nodeModelInd);

    connectionsData.ind = nodeModelInd1;
  }

  connectionsData.node  = id;
  connectionsData.name  = name;
  connectionsData.group = group;

  return true;
}

void
CQChartsAdjacencyPlot::
sortNodes(const NodeMap &nodes, NodeArray &sortedNodes, NodeData &nodeData) const
{
  assert(sortedNodes.empty());

  nodeData.maxValue = 0;
  nodeData.maxGroup = 0;
  nodeData.maxNode  = 0;

  for (auto &pnode : nodes) {
    auto *node = const_cast<CQChartsAdjacencyNode *>(pnode.second);

    sortedNodes.push_back(node);

    nodeData.maxValue = std::max(nodeData.maxValue, node->maxValue());
    nodeData.maxGroup = std::max(nodeData.maxGroup, node->group());
    nodeData.maxNode  = std::max(nodeData.maxNode , node->id());
  }

  if      (sortType() == SortType::NAME) {
    std::sort(sortedNodes.begin(), sortedNodes.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        return lhs->name() < rhs->name();
      });
  }
  else if (sortType() == SortType::GROUP) {
    std::sort(sortedNodes.begin(), sortedNodes.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        if (lhs->group() != rhs->group())
          return lhs->group() < rhs->group();

        return lhs->name() < rhs->name();
      });
  }
  else if (sortType() == SortType::COUNT) {
    std::sort(sortedNodes.begin(), sortedNodes.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        if (lhs->value().real() != rhs->value().real())
          return lhs->value().real() < rhs->value().real();

        return lhs->name() < rhs->name();
      });
  }
}

//---

void
CQChartsAdjacencyPlot::
createNameNodeObjs(PlotObjs &objs) const
{
  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  //---

  for (const auto &nameNode : nameNodeMap_) {
    auto *node = nameNode.second;
    if (! node->isVisible()) continue;

    th->nodes_[node->id()] = node;
  }

  //---

  sortNodes(nodes_, th->sortedNodes_, th->nodeData_);

  //---

  double xb = lengthPlotWidth (bgMargin());
  double yb = lengthPlotHeight(bgMargin());

  th->nodeData_.maxLen = 0;

  for (auto &node1 : sortedNodes_) {
    th->nodeData_.maxLen = std::max(th->nodeData_.maxLen, int(node1->name().size()));
  }

  //---

  if (factor_ < 0.0)
    th->initFactor();

  //---

  int nn = numVisibleNodes();

  if (nn + maxLen()*factor_ > 0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*factor_*scale();

  //---

  double equalValue = 0.0;

  double y = 1.0 - tsize;

  for (auto &node1 : sortedNodes_) {
    double x = tsize;

    for (auto &node2 : sortedNodes_) {
      double value = node1->edgeValue(node2, equalValue);

      // skip unconnected
      bool connected;

      if (node1 == node2)
        connected = (value > 0.0 || isForceDiagonal());
      else
        connected = ! CMathUtil::isZero(value);

      if (connected) {
        BBox bbox(x, y - scale(), x + scale(), y);

        ColorInd ig(node1->group(), maxGroup() + 1);

        auto *obj = th->createCellObj(node1, node2, value, bbox, ig);

        objs.push_back(obj);
      }

      x += scale();
    }

    y -= scale();
  }
}

//---

CQChartsAdjacencyNode *
CQChartsAdjacencyPlot::
findNode(const QString &str) const
{
  auto p = nameNodeMap_.find(str);

  if (p != nameNodeMap_.end())
    return (*p).second;

  //---

  int id = nameNodeMap_.size();

  auto *node = new CQChartsAdjacencyNode(id, str, 0);

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(str, node));

  return (*p1).second;
}

CQChartsAdjacencyCellObj *
CQChartsAdjacencyPlot::
createCellObj(CQChartsAdjacencyNode *node1, CQChartsAdjacencyNode *node2, double value,
              const BBox &rect, const ColorInd &ig)
{
  return new CQChartsAdjacencyCellObj(this, node1, node2, value, rect, ig);
}

//---

void
CQChartsAdjacencyPlot::
initFactor()
{
  QFontMetricsF fm(view_->QWidget::font());

  double th = fm.height();

  double twMax = 0.0;

  for (auto &node : sortedNodes_) {
    const QString &str = node->name();

    double tw = fm.width(str) + 4;

    twMax = std::max(twMax, tw);
  }

  factor_ = 1.1*twMax/(maxLen()*th);
}

void
CQChartsAdjacencyPlot::
autoFit()
{
  int tries = 3;

  for (int i = 0; i < tries; ++i) {
    factor_ = drawFactor();

    updateObjs();
  }
}

void
CQChartsAdjacencyPlot::
postResize()
{
  CQChartsPlot::postResize();

  updateRangeAndObjs();

  setInsideObj(nullptr);
}

bool
CQChartsAdjacencyPlot::
hasBackground() const
{
  return true;
}

void
CQChartsAdjacencyPlot::
execDrawBackground(CQChartsPaintDevice *device) const
{
  // calc text size
  auto po = windowToPixel(Point(0.0, 1.0));

  double pxs = windowToPixelWidth (scale());
  double pys = windowToPixelHeight(scale());

  double xts = maxLen()*factor_*pxs;
  double yts = maxLen()*factor_*pys;

  //---

  // set font
  double ts = std::min(pxs, pys);

  QFont font = this->textFont().calcFont();

  font.setPixelSize(ts >= 1.0 ? int(ts) : 1);

  device->setFont(font);

  QFontMetricsF fm(device->font());

  //---

  // draw text
  QColor tc = interpTextColor(ColorInd());

  setPen(device, CQChartsPenData(true, tc, textAlpha()));

  //---

  double twMax = 0.0;

  // draw row labels
  double px = po.x + lengthPixelWidth (bgMargin());
  double py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node : sortedNodes_) {
    const QString &str = node->name();

    double tw = fm.width(str) + 4;

    twMax = std::max(twMax, tw);

    Point pt(px + xts - tw - 2, py + pys - fm.descent()); // align right

    CQChartsTextOptions options;

    options.angle         = CQChartsAngle(0);
    options.align         = Qt::AlignLeft;
    options.contrast      = isTextContrast();
    options.contrastAlpha = textContrastAlpha();

    CQChartsDrawUtil::drawTextAtPoint(device, device->pixelToWindow(pt), str,
                                      options, /*centered*/false);

    py += pys;
  }

  // save draw factor
  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->nodeData_.drawFactor = twMax/std::min(maxLen()*pxs, maxLen()*pys);

  // draw column labels
  px = po.x + lengthPixelWidth (bgMargin()) + xts;
  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node : sortedNodes_) {
    Point p1(px + pxs/2, py - 2);

    CQChartsTextOptions options;

    options.angle         = CQChartsAngle(90);
    options.align         = Qt::AlignHCenter | Qt::AlignBottom;
    options.contrast      = isTextContrast();
    options.contrastAlpha = textContrastAlpha();

    CQChartsDrawUtil::drawTextAtPoint(device, device->pixelToWindow(p1), node->name(),
                                      options, /*centered*/ true);

    px += pxs;
  }

  //---

  int nn = numVisibleNodes();

  px = po.x + lengthPixelWidth (bgMargin()) + xts;
  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  //---

  QColor fc = interpBackgroundFillColor(ColorInd());

  setPenBrush(device,
   CQChartsPenData  (false),
   CQChartsBrushData(true, fc, backgroundFillAlpha(), backgroundFillPattern()));

  BBox cellBBox(px, py, px + std::max(nn, 1)*pxs, py + std::max(nn, 1)*pys);

  device->fillRect(device->pixelToWindow(cellBBox));

  //---

  // draw empty cells
  CQChartsPenBrush emptyPenBrush;

  QColor pc = interpEmptyCellStrokeColor(ColorInd());
  QColor bc = interpEmptyCellFillColor  (ColorInd());

  setPenBrush(emptyPenBrush,
    CQChartsPenData  (true, pc, emptyCellStrokeAlpha(), emptyCellStrokeWidth(),
                      emptyCellStrokeDash()),
    CQChartsBrushData(true, bc, emptyCellFillAlpha(), emptyCellFillPattern()));

  auto cornerSize = emptyCellCornerSize();

  double equalValue = 0.0;

  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node1 : sortedNodes_) {
    double px = po.x + lengthPixelWidth(bgMargin()) + xts;

    for (auto &node2 : sortedNodes_) {
      double value = node1->edgeValue(node2, equalValue);

      // draw unconnected
      bool connected;

      if (node1 == node2)
        connected = (value > 0.0 || isForceDiagonal());
      else
        connected = ! CMathUtil::isZero(value);

      if (! connected) {
        BBox cellBBox = device->pixelToWindow(BBox(px, py, px + pxs, py + pys));

        CQChartsDrawUtil::setPenBrush(device, emptyPenBrush);

        CQChartsDrawUtil::drawRoundedPolygon(device, cellBBox, cornerSize);
      }

      px += pxs;
    }

    py += pys;
  }

  if (insideObject()) {
    auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

    th->setInsideObj(nullptr);

    th->drawForeground();
  }
}

bool
CQChartsAdjacencyPlot::
hasForeground() const
{
  if (! insideObj())
    return true;

  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsAdjacencyPlot::
execDrawForeground(CQChartsPaintDevice *device) const
{
  if (insideObj())
    insideObj()->draw(device);
}

QColor
CQChartsAdjacencyPlot::
interpGroupColor(int group) const
{
  ColorInd ig(group, maxGroup() + 1);

  return interpPaletteColor(ig);
}

//------

CQChartsAdjacencyCellObj::
CQChartsAdjacencyCellObj(const CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                         CQChartsAdjacencyNode *node2, double value, const BBox &rect,
                         const ColorInd &ig) :
 CQChartsPlotObj(const_cast<CQChartsAdjacencyPlot *>(plot), rect, ColorInd(), ig, ColorInd()),
 plot_(plot), node1_(node1), node2_(node2), value_(value)
{
  setDetailHint(DetailHint::MAJOR);

  ModelIndex ind1 = node1->ind(node2->id());
  ModelIndex ind2 = node2->ind(node1->id());

  if      (ind1.isValid())
    addModelInd(plot->modelIndex(ind1));
  else if (ind2.isValid())
    addModelInd(plot->modelIndex(ind2));
}

QString
CQChartsAdjacencyCellObj::
calcId() const
{
  QString groupStr1 = QString("(%1)").arg(node1()->group());
  QString groupStr2 = QString("(%1)").arg(node2()->group());

  return QString("%1:%2%3:%4%5:%6").arg(typeName()).
           arg(node1()->name()).arg(groupStr1).arg(node2()->name()).arg(groupStr2).arg(value());
}

QString
CQChartsAdjacencyCellObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  if (node1() != node2()) {
    QString groupStr1 = QString("(%1)").arg(node1()->group());
    QString groupStr2 = QString("(%1)").arg(node2()->group());

    tableTip.addTableRow("From", node1()->name(), groupStr1);
    tableTip.addTableRow("To"  , node2()->name(), groupStr2);
  }
  else {
    tableTip.addTableRow("Name" , node1()->name());
    tableTip.addTableRow("Group", node1()->group());
    tableTip.addTableRow("Total", node1()->totalValue());
  }

  if (value() > 0.0)
    tableTip.addTableRow("Value", value());

  //---

  //plot()->addTipColumns(tableTip, node1()->ind());

  //---

  return tableTip.str();
}

void
CQChartsAdjacencyCellObj::
getObjSelectIndices(Indices &inds) const
{
  for (auto &ind : modelInds())
    inds.insert(ind);
}

void
CQChartsAdjacencyCellObj::
draw(CQChartsPaintDevice *device)
{
  // draw inside object
  if (isInside()) {
    if (plot_->insideObj() != this) {
      auto *plot = const_cast<CQChartsAdjacencyPlot *>(plot_);

      plot->setInsideObj(const_cast<CQChartsAdjacencyCellObj *>(this));

      plot->drawForeground();
    }
  }

  //---

  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw box
  device->setColorNames();

  drawRoundedPolygon(device, penBrush, BBox(rect()), plot_->cornerSize());

  device->resetColorNames();
}

void
CQChartsAdjacencyCellObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  ColorInd colorInd = calcColorInd();

  //---

  // get fill color for node
  auto nodeFillColor = [&](CQChartsAdjacencyNode *node) {
    if      (plot_->colorType() == CQChartsPlot::ColorType::AUTO ||
             plot_->colorType() == CQChartsPlot::ColorType::GROUP)
      return plot_->interpFillColor(ColorInd(node->group(), plot_->maxGroup() + 1));
    else if (plot_->colorType() == CQChartsPlot::ColorType::INDEX)
      return plot_->interpFillColor(ColorInd(node->id(), plot_->numNodes()));
    else
      return plot_->interpFillColor(colorInd);
  };

  auto nodesFillColor = [&](CQChartsAdjacencyNode *node1, CQChartsAdjacencyNode *node2) {
    if  (node1 == node2)
      return nodeFillColor(node1);
    else
      return CQChartsUtil::blendColors(nodeFillColor(node1), nodeFillColor(node2), 0.5);
  };

  //---

  // get stroke color for node
  auto nodeStrokeColor = [&](CQChartsAdjacencyNode *node) {
    if      (plot_->colorType() == CQChartsPlot::ColorType::AUTO ||
             plot_->colorType() == CQChartsPlot::ColorType::GROUP)
      return plot_->interpStrokeColor(ColorInd(node->group(), plot_->maxGroup() + 1));
    else if (plot_->colorType() == CQChartsPlot::ColorType::INDEX)
      return plot_->interpStrokeColor(ColorInd(node->id(), plot_->numNodes()));
    else
      return plot_->interpStrokeColor(colorInd);
  };

  auto nodesStrokeColor = [&](CQChartsAdjacencyNode *node1, CQChartsAdjacencyNode *node2) {
    if  (node1 == node2)
      return nodeStrokeColor(node1);
    else
      return CQChartsUtil::blendColors(nodeStrokeColor(node1), nodeStrokeColor(node2), 0.5);
  };

  //---

  // get background color
  QColor bg = plot_->interpEmptyCellFillColor(ColorInd());

  //--

  // calc brush color (scaled to value)
  QColor bc = nodesFillColor(node1(), node2());

  if (node1() != node2()) {
    double s = CMathUtil::map(value(), 0.0, plot_->maxValue(), 0.0, 1.0);

    bc = CQChartsUtil::blendColors(bc, bg, s);
  }

  // calc pen color (not scaled)
  QColor pc = nodesStrokeColor(node1(), node2());

  //---

  // calc pen and brush
  plot_->setPenBrush(penBrush,
    CQChartsPenData  (true, pc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash()),
    CQChartsBrushData(true, bc, plot_->fillAlpha(), plot_->fillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsAdjacencyCellObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.value = " << value() << ";\n";
}

double
CQChartsAdjacencyCellObj::
xColorValue(bool relative) const
{
  if (! relative)
    return node1()->id();
  else
    return CMathUtil::map(node1()->id(), 0.0, plot_->maxNode(), 0.0, 1.0);
}

double
CQChartsAdjacencyCellObj::
yColorValue(bool relative) const
{
  if (! relative)
    return node2()->id();
  else
    return CMathUtil::map(node2()->id(), 0.0, plot_->maxNode(), 0.0, 1.0);
}
