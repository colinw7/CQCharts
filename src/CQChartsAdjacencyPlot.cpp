#include <CQChartsAdjacencyPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsValueSet.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPropertyViewModel.h>
#include <CQPerfMonitor.h>

#include <QMenu>
#include <QAction>

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
       "matrix where the color of the cells represents the group (color) and "
       "connectivity (alpha).").
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
isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsAdjacencyPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
}

CQChartsPlot *
CQChartsAdjacencyPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsAdjacencyPlot(view, model);
}

//------

CQChartsAdjacencyPlot::
CQChartsAdjacencyPlot(View *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("adjacency"), model),
 CQChartsObjBackgroundFillData<CQChartsAdjacencyPlot>(this),
 CQChartsObjShapeData         <CQChartsAdjacencyPlot>(this),
 CQChartsObjTextData          <CQChartsAdjacencyPlot>(this),
 CQChartsObjEmptyCellShapeData<CQChartsAdjacencyPlot>(this)
{
}

CQChartsAdjacencyPlot::
~CQChartsAdjacencyPlot()
{
  CQChartsAdjacencyPlot::term();
}

//---

void
CQChartsAdjacencyPlot::
init()
{
  CQChartsConnectionPlot::init();

  //---

  NoUpdate noUpdate(this);

  setBackgroundFillColor(Color::makeInterfaceValue(0.2));

  setStrokeColor(Color::makeInterfaceValue(1.0));
  setStrokeAlpha(Alpha(0.5));

  setEmptyCellFillColor  (Color::makeInterfaceValue(0.1));
  setEmptyCellStrokeColor(Color::makeInterfaceValue(0.2));

  setFillColor(Color::makePalette());

  setOuterMargin(PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0)));

  //---

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsAdjacencyPlot::
term()
{
  clearNodes();
}

//---

void
CQChartsAdjacencyPlot::
clearNodes()
{
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
setBgMargin(const Length &l)
{
  CQChartsUtil::testAndSet(bgMargin_, l, [&]() { updateObjs(); } );
}

//---

void
CQChartsAdjacencyPlot::
addProperties()
{
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

  addProp("emptyCell/stroke", "emptyCellCornerSize", "cornerSize",
          "Empty cell box corner size");

  // text
  addProp("text", "textVisible", "visible", "Text is visible");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  //---

  hideProp(this, "text.font");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

CQChartsGeom::Range
CQChartsAdjacencyPlot::
calcRange() const
{
  // base range always (0, 0) - (1, 1)
  Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(1.0, 1.0);

  return dataRange;
}

//------

void
CQChartsAdjacencyPlot::
clearPlotObjList()
{
  clearNodes();

  clearErrors();

  setInsideObj(nullptr);

  CQChartsPlot::clearPlotObjList();
}

bool
CQChartsAdjacencyPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  //---

  // check columns
  if (! checkColumns())
    return false;

  //---

  // create objects
  auto columnDataType = calcColumnDataType();

  bool rc      = false;
  bool addObjs = true;

  if      (columnDataType == ColumnDataType::HIER)
    rc = initHierObjs();
  else if (columnDataType == ColumnDataType::LINK)
    rc = initLinkObjs();
  else if (columnDataType == ColumnDataType::CONNECTIONS) {
    rc = initConnectionObjs(objs);

    addObjs = false;
  }
  else if (columnDataType == ColumnDataType::PATH)
    rc = initPathObjs();
  else if (columnDataType == ColumnDataType::FROM_TO)
    rc = initFromToObjs();
  else if (columnDataType == ColumnDataType::TABLE)
    rc = initTableObjs();

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
  auto srcNode  = findNode(srcStr);
  auto destNode = findNode(destStr);

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
addPathValue(const PathData &pathData) const
{
  int n = pathData.pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  auto separator = calcSeparator();

  auto path1 = pathData.pathStrs[0];

  for (int i = 1; i < n; ++i) {
    auto path2 = path1 + separator + pathData.pathStrs[i];

    auto srcNode  = findNode(path1);
    auto destNode = findNode(path2);

    srcNode ->setLabel(pathData.pathStrs[i - 1]);
    destNode->setLabel(pathData.pathStrs[i    ]);

    srcNode->setDepth(i - 1);
    srcNode->setGroup(i - 1);

    destNode->setDepth(i);
    destNode->setGroup(i);

    if (i < n - 1) {
      if (! srcNode->hasNode(destNode)) {
        srcNode->addEdge(destNode);

        destNode->setParent(srcNode.get());
      }
    }
    else {
      srcNode->addEdge(destNode, OptReal(pathData.value));

      destNode->setParent(srcNode.get());
      destNode->setValue (OptReal(pathData.value));
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
      auto &node = p.second;
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
addFromToValue(const FromToData &fromToData) const
{
  auto srcNode = findNode(fromToData.fromStr);

  if (fromToData.depth > 0)
    srcNode->setDepth(fromToData.depth);

  // Just node
  if (fromToData.toStr == "") {
    for (const auto &nv : fromToData.nameValues.nameValues()) {
      auto value = nv.second.toString();

      if      (nv.first == "label") {
        srcNode->setLabel(value);
      }
      else if (nv.first == "color") {
        //srcNode->setColor(CQChartsUtil::stringToColor(value));
      }
    }
  }
  else {
    if (fromToData.fromStr == fromToData.toStr)
      return;

    auto destNode = findNode(fromToData.toStr);

    if (fromToData.depth > 0)
      destNode->setDepth(fromToData.depth + 1);

    auto value = fromToData.value;

    if (! value.isSet())
      value = OptReal(1.0);

    srcNode->addEdge(destNode, value);

    for (const auto &nv : fromToData.nameValues.nameValues()) {
      //auto value1 = nv.second.toString();

      if      (nv.first == "label") {
      }
      else if (nv.first == "color") {
      }
    }

    auto fromModelInd = normalizeIndex(fromToData.fromModelInd);
    auto toModelInd   = normalizeIndex(fromToData.toModelInd  );

    srcNode ->setInd(destNode->id(), toModelInd  );
    destNode->setInd(srcNode ->id(), fromModelInd);
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
      const auto &node = p.second;

      if (node->depth() > maxDepth())
        node->setVisible(false);
    }
  }

  // hide nodes less than min value
  if (minValue() > 0) {
    for (const auto &p : nameNodeMap_) {
      const auto &node = p.second;

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
  long group = linkConnectionData.valueModelInd.row();

  if (groupColumn().isValid()) {
    bool ok1;

    group = modelInteger(linkConnectionData.groupModelInd, ok1);

    if (! ok1) {
      auto *th = const_cast<CQChartsAdjacencyPlot *>(this);
      th->addDataError(linkConnectionData.groupModelInd, "Non-integer group value");
      return;
    }
  }

  //---

  auto modelInd = linkConnectionData.nameModelInd;

  if (! modelInd.isValid())
    modelInd = linkConnectionData.linkModelInd;

  if (! modelInd.isValid())
    modelInd = linkConnectionData.valueModelInd;

  //---

  auto srcNode  = findNode(linkConnectionData.srcStr );
  auto destNode = findNode(linkConnectionData.destStr);
//assert(srcNode != destNode);

  if (! srcNode->hasNode(destNode))
    srcNode->addEdge(destNode, OptReal(linkConnectionData.value));

  // connectional is directional (optional ?)
  if (isSymmetric()) {
    if (! destNode->hasNode(srcNode))
      destNode->addEdge(srcNode, OptReal(linkConnectionData.value));
  }

  srcNode->setGroup(static_cast<int>(group));

  if (modelInd.isValid()) {
    auto modelInd1 = normalizeIndex(modelInd);

    srcNode ->setInd(destNode->id(), modelInd1);
    destNode->setInd(srcNode ->id(), modelInd1);
  }

  //---

  if (linkConnectionData.depth > 0) {
    srcNode ->setDepth(linkConnectionData.depth);
    destNode->setDepth(linkConnectionData.depth + 1);
  }
}

//------

bool
CQChartsAdjacencyPlot::
initConnectionObjs(PlotObjs &objs) const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsAdjacencyPlot *adjacencyPlot) :
     adjacencyPlot_(adjacencyPlot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ConnectionsData connections;

      if (! adjacencyPlot_->getRowConnections(data, connections))
        return State::SKIP;

      idConnections_[connections.node] = connections;

      return State::OK;
    }

    const IdConnectionsData &idConnections() const { return idConnections_; }

   private:
    const CQChartsAdjacencyPlot* adjacencyPlot_ { nullptr };
    IdConnectionsData            idConnections_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  //---

  const auto &idConnectionsData = visitor.idConnections();

  //---

  for (const auto &idConnections : idConnectionsData) {
    int         id    = idConnections.first;
    const auto &ind   = idConnections.second.ind;
    const auto &name  = idConnections.second.name;
    int         group = idConnections.second.group;

    auto node = std::make_shared<AdjacencyNode>(id, name, group);

    if (ind.isValid())
      node->setInd(0, ind);

    th->nodes_[id] = node;
  }

  //---

  for (const auto & [id, connections] : idConnectionsData) {
    auto &node = th->nodes_[id];

    for (const auto &connection : connections.connections) {
      auto &node1 = th->nodes_[connection.node];

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

  if (isTextVisible()) {
    if (fontFactor_ <= 0.0)
      th->initFontFactor();
  }
  else
    th->fontFactor_ = 0.0;

  //---

  int nn = numVisibleNodes();

  if (nn + maxLen()*fontFactor_ > 0.0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*fontFactor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*fontFactor_*scale();

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

        auto ig = groupColorInd(node1->group());

        auto *obj = th->createCellObj(node1, node2, value, bbox, ig);

        obj->connectDataChanged(this, SLOT(updateSlot()));

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

  auto nv = tableConnectionDatas.size();

  for (size_t row = 0; row < nv; ++row) {
    const auto &tableConnectionData = tableConnectionDatas[row];

    auto srcStr = QString::number(tableConnectionData.from());

    auto srcNode = findNode(srcStr);

    srcNode->setName (tableConnectionData.name());
    srcNode->setGroup(tableConnectionData.group().ig);

    for (const auto &value : tableConnectionData.values()) {
      auto destStr = QString::number(value.to);

      auto destNode = findNode(destStr);

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
  long group = data.row;

  if (groupColumn().isValid()) {
    ModelIndex groupInd(th, data.row, groupColumn(), data.parent);

    bool ok1;

    long group1 = modelInteger(groupInd, ok1);

    if (ok1)
      group = group1;
  }

  //---

  // get optional node id (default to row)
  ModelIndex nodeModelInd;

  long id = data.row;

  if (nodeColumn().isValid()) {
    nodeModelInd = ModelIndex(th, data.row, nodeColumn(), data.parent);

    bool ok2;
    id = modelInteger(nodeModelInd, ok2);
    if (! ok2) return th->addDataError(nodeModelInd, "Non-integer node value");
  }

  //---

  // get connections
  ModelIndex connectionsInd(th, data.row, connectionsColumn(), data.parent);

  if (connectionsColumnType() == ColumnType::CONNECTION_LIST) {
    bool ok3;
    auto connectionsVar = modelValue(connectionsInd, ok3);
    if (! ok3) return false;

    connectionsData.connections = ConnectionList::fromVariant(connectionsVar).connections();
  }
  else {
    bool ok3;
    auto connectionsStr = modelString(connectionsInd, ok3);
    if (! ok3) return false;

    ConnectionList::stringToConnections(connectionsStr, connectionsData.connections);
  }

  //---

  // get optional name
  ModelIndex nameModelInd;

  auto name = QString::number(id);

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

  connectionsData.node  = static_cast<int>(id);
  connectionsData.name  = name;
  connectionsData.group = static_cast<int>(group);

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
    auto &node = pnode.second;

    sortedNodes.push_back(node);

    nodeData.maxValue = std::max(nodeData.maxValue, node->maxValue());
    nodeData.maxGroup = std::max(nodeData.maxGroup, node->group());
    nodeData.maxNode  = std::max(nodeData.maxNode , node->id());
  }

  if      (sortType() == SortType::NAME) {
    std::sort(sortedNodes.begin(), sortedNodes.end(), [](const AdjacencyNodeP &lhs,
                                                         const AdjacencyNodeP &rhs) {
      return lhs->name() < rhs->name();
    });
  }
  else if (sortType() == SortType::GROUP) {
    std::sort(sortedNodes.begin(), sortedNodes.end(), [](const AdjacencyNodeP &lhs,
                                                         const AdjacencyNodeP &rhs) {
      if (lhs->group() != rhs->group())
        return (lhs->group() < rhs->group());

      return lhs->name() < rhs->name();
    });
  }
  else if (sortType() == SortType::COUNT) {
    std::sort(sortedNodes.begin(), sortedNodes.end(), [](const AdjacencyNodeP &lhs,
                                                         const AdjacencyNodeP &rhs) {
      if (lhs->value() != rhs->value())
        return (lhs->value() < rhs->value());

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
    const auto &node = nameNode.second;
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

  if (isTextVisible()) {
    if (fontFactor_ <= 0.0)
      th->initFontFactor();
  }
  else
    th->fontFactor_ = 0.0;

  //---

  int nn = numVisibleNodes();

  if (nn + maxLen()*fontFactor_ > 0.0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*fontFactor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*fontFactor_*scale();

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

        auto ig = groupColorInd(node1->group());

        auto *obj = th->createCellObj(node1, node2, value, bbox, ig);

        obj->connectDataChanged(this, SLOT(updateSlot()));

        objs.push_back(obj);
      }

      x += scale();
    }

    y -= scale();
  }
}

//---

CQChartsAdjacencyPlot::AdjacencyNodeP
CQChartsAdjacencyPlot::
findNode(const QString &str) const
{
  auto p = nameNodeMap_.find(str);

  if (p != nameNodeMap_.end())
    return (*p).second;

  //---

  int id = int(nameNodeMap_.size());

  auto node = std::make_shared<AdjacencyNode>(id, str, 0);

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(str, node));

  return (*p1).second;
}

//---

bool
CQChartsAdjacencyPlot::
addMenuItems(QMenu *menu, const Point &)
{
  bool added = false;

  if (canDrawColorMapKey()) {
    addColorMapKeyItems(menu);

    added = true;
  }

  return added;
}

//---

CQChartsAdjacencyCellObj *
CQChartsAdjacencyPlot::
createCellObj(const AdjacencyNodeP &node1, const AdjacencyNodeP &node2, double value,
              const BBox &rect, const ColorInd &ig)
{
  return new CellObj(this, node1, node2, value, rect, ig);
}

//---

void
CQChartsAdjacencyPlot::
initFontFactor()
{
  QFontMetricsF fm(view_->QWidget::font());

  // get height
  double th = fm.height();

  // get max width
  double twMax = 0.0;

  for (auto &node : sortedNodes_) {
    const auto &str = node->name();

    // TODO: use clipped text width
    double tw = fm.horizontalAdvance(str) + 4;

    twMax = std::max(twMax, tw);
  }

  // calc font factor from max width and max label length
  fontFactor_ = 1.1*twMax/(maxLen()*th);
}

void
CQChartsAdjacencyPlot::
autoFit()
{
  CQChartsPlot::autoFit();

  int tries = 3;

  for (int i = 0; i < tries; ++i) {
    fontFactor_ = drawFontFactor();

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
execDrawBackground(PaintDevice *device) const
{
  rowNodeLabels_.clear();
  colNodeLabels_.clear();

  // calc text size
  auto po = windowToPixel(Point(0.0, 1.0));

  pxs_ = windowToPixelWidth (scale());
  pys_ = windowToPixelHeight(scale());

  xts_ = maxLen()*fontFactor_*pxs_;
  yts_ = maxLen()*fontFactor_*pys_;

  //---

  if (sortedNodes_.empty())
    return;

  //---

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  if (isTextVisible()) {
    // set font
    double ts = std::min(pxs_, pys_);

    auto font = this->textFont().calcFont();

    font.setPixelSize(ts >= 1.0 ? int(ts) : 1);

    device->setFont(font);

    //---

    twMax_ = 0.0;

    // draw row labels
    double px = po.x + lengthPixelWidth (bgMargin());
    double py = po.y + lengthPixelHeight(bgMargin()) + yts_;

    for (auto &node : sortedNodes_) {
      drawRowNodeLabel(device, Point(px, py), node.get());

      py += pys_;
    }

    // save draw font factor
    th->nodeData_.drawFontFactor = twMax_/std::min(maxLen()*pxs_, maxLen()*pys_);

    // draw column labels
    px = po.x + lengthPixelWidth (bgMargin()) + xts_;
    py = po.y + lengthPixelHeight(bgMargin()) + yts_;

    for (auto &node : sortedNodes_) {
      drawColNodeLabel(device, Point(px, py), node.get());

      px += pxs_;
    }
  }

  //---

  QFontMetricsF fm(device->font());

  for (const auto &pr : rowNodeLabels_) {
    for (const auto &pr1 : pr.second) {
      const auto &str = pr1.first;

      double tw = fm.horizontalAdvance(str) + 4;

      BBox bbox1, bbox2;
      int  group = -1;

      for (const auto &gp : pr1.second) {
        const auto &p = gp.point;

        group = gp.group;

        auto p1 = Point(p.x + xts_ - tw - 4, p.y       );
        auto p2 = Point(p.x + xts_         , p.y + pys_);

        bbox1 += p1;
        bbox1 += p2;

        bbox2 += p;
      }

      //--

      auto brush = device->brush();

      device->setBrush(QBrush(Qt::NoBrush));

      device->drawRect(device->pixelToWindow(bbox1));

      device->setBrush(brush);

      //--

      drawRowNodeLabelStr(device, bbox2.getCenter(), str, group);
    }
  }

  for (const auto &pc : colNodeLabels_) {
    for (const auto &pc1 : pc.second) {
      const auto &str = pc1.first;

      BBox bbox1, bbox2;
      int  group = -1;

      for (const auto &gp : pc1.second) {
        const auto &p = gp.point;

        group = gp.group;

        auto p1 = Point(p.x, p.y       );
        auto p2 = Point(p.x, p.y - pys_);

        bbox1 += p1;
        bbox1 += p2;

        bbox2 += p;
      }

      //--

      auto brush = device->brush();

      device->setBrush(QBrush(Qt::NoBrush));

      device->drawRect(device->pixelToWindow(bbox1));

      device->setBrush(brush);

      //--

      drawColNodeLabelStr(device, bbox2.getCenter(), str, group);
    }
  }

  //---

  int nn = numVisibleNodes();

  double px = po.x + lengthPixelWidth (bgMargin()) + xts_;
  double py = po.y + lengthPixelHeight(bgMargin()) + yts_;

  //---

  auto fc = interpBackgroundFillColor(ColorInd());

  setPenBrush(device, PenData(false), BrushData(true, fc, backgroundFillData()));

  BBox cellBBox(px, py, px + std::max(nn, 1)*pxs_, py + std::max(nn, 1)*pys_);

  device->fillRect(pixelToWindow(cellBBox));

  //---

  // draw empty cells
  PenBrush emptyPenBrush;

  auto pc = interpEmptyCellStrokeColor(ColorInd());
  auto bc = interpEmptyCellFillColor  (ColorInd());

  setPenBrush(emptyPenBrush,
    PenData  (true, pc, emptyCellShapeData().stroke()),
    BrushData(true, bc, emptyCellShapeData().fill  ()));

  auto cornerSize = emptyCellCornerSize();

  double equalValue = 0.0;

  py = po.y + lengthPixelHeight(bgMargin()) + yts_;

  for (auto &node1 : sortedNodes_) {
    double px1 = po.x + lengthPixelWidth(bgMargin()) + xts_;

    for (auto &node2 : sortedNodes_) {
      double value = node1->edgeValue(node2, equalValue);

      // draw unconnected
      bool connected;

      if (node1 == node2)
        connected = (value > 0.0 || isForceDiagonal());
      else
        connected = ! CMathUtil::isZero(value);

      if (! connected) {
        auto cellBBox1 = pixelToWindow(BBox(px1, py, px1 + pxs_, py + pys_));

        CQChartsDrawUtil::drawRoundedRect(device, emptyPenBrush, cellBBox1, cornerSize);
      }

      px1 += pxs_;
    }

    py += pys_;
  }

  if (insideObject()) {
    th->setInsideObj(nullptr);

    th->drawForeground();
  }
}

void
CQChartsAdjacencyPlot::
drawRowNodeLabel(PaintDevice *device, const Point &p, AdjacencyNode *node) const
{
  int group = node->group();

  const auto &name = node->name();

  if (isHierName()) {
    auto strs = name.split(calcSeparator(), Qt::SkipEmptyParts);

    int n = strs.length();

    for (int i = 0; i < n; ++i) {
      if (i == n - 1)
        drawRowNodeLabelStr(device, p, strs[i], group);
      else
        addRowNodeLabelStr(p, strs[i], -i - 1, group);
    }
  }
  else
    drawRowNodeLabelStr(device, p, name, group);
}

void
CQChartsAdjacencyPlot::
drawRowNodeLabelStr(PaintDevice *device, const Point &p, const QString &str, int group) const
{
  QFontMetricsF fm(device->font());

  double tw = fm.horizontalAdvance(str) + 4;

  twMax_ = std::max(twMax_, tw);

  //---

  // draw text
  auto tc = interpTextColor(ColorInd());

  PenData penData(true, tc, textAlpha());

  setPen(device, penData);

  //--

  Point pt(p.x + xts_ - tw - 2, p.y + pys_ - fm.descent()); // align right

  auto textOptions = this->textOptions(device);

  textOptions.angle = Angle();
  textOptions.align = Qt::AlignLeft;

  CQChartsDrawUtil::drawTextAtPoint(device, pixelToWindow(pt), str, textOptions, /*centered*/false);

  //--

  auto gc = interpGroupColor(group);

  auto p1 = pixelToWindow(Point(p.x + xts_ - 4, p.y + pys_/2.0 - fm.height()/2.0));
  auto p2 = pixelToWindow(Point(p.x + xts_    , p.y + pys_/2.0 + fm.height()/2.0));

  PenBrush penBrush;

  setPenBrush(penBrush, PenData(false), BrushData(true, gc, Alpha(1.0)));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawRect(BBox(p1, p2));
}

void
CQChartsAdjacencyPlot::
addRowNodeLabelStr(const Point &p, const QString &str, int depth, int group) const
{
  Point p1(p.x + depth*pxs_, p.y);

  rowNodeLabels_[depth][str].push_back(GroupPoint(group, p1));
}

void
CQChartsAdjacencyPlot::
drawColNodeLabel(PaintDevice *device, const Point &p, AdjacencyNode *node) const
{
  int group = node->group();

  const auto &name = node->name();

  if (isHierName()) {
    auto strs = name.split(calcSeparator(), Qt::SkipEmptyParts);

    int n = strs.length();

    for (int i = 0; i < n; ++i) {
      if (i == n - 1)
        drawColNodeLabelStr(device, p, strs[i], group);
      else
        addColNodeLabelStr(p, strs[i], -i - 1, group);
    }
  }
  else
    drawColNodeLabelStr(device, p, name, group);
}

void
CQChartsAdjacencyPlot::
drawColNodeLabelStr(PaintDevice *device, const Point &p, const QString &str, int group) const
{
  QFontMetricsF fm(device->font());

  //---

  // draw text
  auto tc = interpTextColor(ColorInd());

  PenData penData(true, tc, textAlpha());

  setPen(device, penData);

  //--

  Point pt(p.x + pxs_/2.0, p.y - 2);

  auto textOptions = this->textOptions(device);

  textOptions.angle = Angle(90.0);
  textOptions.align = Qt::AlignHCenter | Qt::AlignBottom;

  CQChartsDrawUtil::drawTextAtPoint(device, pixelToWindow(pt), str, textOptions, /*centered*/ true);

  //--

  auto gc = interpGroupColor(group);

  auto p1 = pixelToWindow(Point(pt.x - fm.height()/2.0, pt.y - 2));
  auto p2 = pixelToWindow(Point(pt.x + fm.height()/2.0, pt.y + 2));

  PenBrush penBrush;

  setPenBrush(penBrush, PenData(false), BrushData(true, gc, Alpha(1.0)));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawRect(BBox(p1, p2));
}

void
CQChartsAdjacencyPlot::
addColNodeLabelStr(const Point &p, const QString &str, int depth, int group) const
{
  auto p1 = Point(p.x, p.y + depth*pys_);

  colNodeLabels_[depth][str].push_back(GroupPoint(group, p1));
}

//---

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
execDrawForeground(PaintDevice *device) const
{
  if (insideObj())
    insideObj()->draw(device);

  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

QColor
CQChartsAdjacencyPlot::
interpGroupColor(int group) const
{
  return interpFillColor(groupColorInd(group));
  //return interpPaletteColor(groupColorInd(group));
}

CQChartsUtil::ColorInd
CQChartsAdjacencyPlot::
groupColorInd(int group) const
{
  return ColorInd(group, maxGroup() + 1);
}

//---

CQChartsPlotCustomControls *
CQChartsAdjacencyPlot::
createCustomControls()
{
  auto *controls = new CQChartsAdjacencyPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsAdjacencyCellObj::
CQChartsAdjacencyCellObj(const AdjacencyPlot *adjacencyPlot, const AdjacencyNodeP &node1,
                         const AdjacencyNodeP &node2, double value, const BBox &rect,
                         const ColorInd &ig) :
 CQChartsPlotObj(const_cast<AdjacencyPlot *>(adjacencyPlot), rect, ColorInd(), ig, ColorInd()),
 adjacencyPlot_(adjacencyPlot), node1_(node1), node2_(node2), value_(value)
{
  setDetailHint(DetailHint::MAJOR);

  auto ind1 = node1->ind(node2->id());
  auto ind2 = node2->ind(node1->id());

  if      (ind1.isValid())
    addModelInd(adjacencyPlot->modelIndex(ind1));
  else if (ind2.isValid())
    addModelInd(adjacencyPlot->modelIndex(ind2));
}

QString
CQChartsAdjacencyCellObj::
calcId() const
{
  auto groupStr1 = QString("(%1)").arg(node1()->group());
  auto groupStr2 = QString("(%1)").arg(node2()->group());

  return QString("%1:%2%3:%4%5:%6").arg(typeName()).
           arg(node1()->name()).arg(groupStr1).arg(node2()->name()).arg(groupStr2).arg(value());
}

QString
CQChartsAdjacencyCellObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  if (node1() != node2()) {
    auto groupStr1 = QString("(%1)").arg(node1()->group());
    auto groupStr2 = QString("(%1)").arg(node2()->group());

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
draw(PaintDevice *device) const
{
  // draw inside object
  if (isInside()) {
    if (adjacencyPlot_->insideObj() != this) {
      auto *plot = const_cast<AdjacencyPlot *>(adjacencyPlot_);

      plot->setInsideObj(const_cast<CQChartsAdjacencyCellObj *>(this));

      plot->drawForeground();
    }
  }

  //---

  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw box
  device->setColorNames();

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, rect(), adjacencyPlot_->cornerSize());

  device->resetColorNames();
}

void
CQChartsAdjacencyCellObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  //---

  // get fill color for node
  auto nodeFillColor = [&](const AdjacencyNodeP &srcNode,
                           const AdjacencyNodeP &destNode, bool &scaled) {
    auto colorType = adjacencyPlot_->colorType();

    if      (colorType == CQChartsPlot::ColorType::AUTO ||
             colorType == CQChartsPlot::ColorType::GROUP) {
      if (adjacencyPlot_->colorColumn().isValid()) {
        scaled = false;

        auto ind1 = srcNode->ind(destNode->id());

        Color indColor;

        if (adjacencyPlot_->colorColumnColor(ind1.row(), ind1.parent(), indColor))
          return adjacencyPlot_->interpColor(indColor, ColorInd());
      }

      return adjacencyPlot_->interpFillColor(adjacencyPlot_->groupColorInd(srcNode->group()));
    }
    else if (colorType == CQChartsPlot::ColorType::INDEX)
      return adjacencyPlot_->interpFillColor(ColorInd(srcNode->id(), adjacencyPlot_->numNodes()));
    else
      return adjacencyPlot_->interpFillColor(colorInd);
  };

  auto nodesFillColor = [&](const AdjacencyNodeP &srcNode,
                            const AdjacencyNodeP &destNode, bool &scaled) {
    if  (srcNode == destNode)
      return nodeFillColor(srcNode, destNode, scaled);
    else {
      return CQChartsUtil::blendColors(nodeFillColor(srcNode , destNode, scaled),
                                       nodeFillColor(destNode, srcNode , scaled), 0.5);
    }
  };

  //---

  // get stroke color for node
  auto nodeStrokeColor = [&](const AdjacencyNodeP &srcNode, const AdjacencyNodeP & /*destNode*/) {
    auto colorType = adjacencyPlot_->colorType();

    if      (colorType == CQChartsPlot::ColorType::AUTO ||
             colorType == CQChartsPlot::ColorType::GROUP)
      return adjacencyPlot_->interpStrokeColor(adjacencyPlot_->groupColorInd(srcNode->group()));
    else if (colorType == CQChartsPlot::ColorType::INDEX)
      return adjacencyPlot_->interpStrokeColor(ColorInd(srcNode->id(), adjacencyPlot_->numNodes()));
    else
      return adjacencyPlot_->interpStrokeColor(colorInd);
  };

  auto nodesStrokeColor = [&](const AdjacencyNodeP &srcNode, const AdjacencyNodeP &destNode) {
    if  (srcNode == destNode)
      return nodeStrokeColor(srcNode, destNode);
    else
      return CQChartsUtil::blendColors(nodeStrokeColor(srcNode, destNode),
                                       nodeStrokeColor(destNode, srcNode), 0.5);
  };

  //---

  // get background color
  auto bg = adjacencyPlot_->interpEmptyCellFillColor(ColorInd());

  //--

  // calc brush color (scaled to value)
  bool scaled = true;

  auto bc = nodesFillColor(node1(), node2(), scaled);

  if (scaled) {
    if (node1() != node2()) {
      double s = CMathUtil::map(value(), 0.0, adjacencyPlot_->maxValue(), 0.0, 1.0);

      bc = CQChartsUtil::blendColors(bc, bg, s);
    }
  }

  // calc pen color (not scaled)
  auto pc = nodesStrokeColor(node1(), node2());

  //---

  // calc pen and brush
  adjacencyPlot_->setPenBrush(penBrush,
    PenData  (true, pc, adjacencyPlot_->shapeData().stroke()),
    BrushData(true, bc, adjacencyPlot_->shapeData().fill  ()));

  if (updateState)
    adjacencyPlot_->updateObjPenBrushState(this, penBrush, drawType());
}

double
CQChartsAdjacencyCellObj::
xColorValue(bool relative) const
{
  if (! relative)
    return node1()->id();
  else
    return CMathUtil::map(node1()->id(), 0.0, adjacencyPlot_->maxNode(), 0.0, 1.0);
}

double
CQChartsAdjacencyCellObj::
yColorValue(bool relative) const
{
  if (! relative)
    return node2()->id();
  else
    return CMathUtil::map(node2()->id(), 0.0, adjacencyPlot_->maxNode(), 0.0, 1.0);
}

//------

CQChartsAdjacencyPlotCustomControls::
CQChartsAdjacencyPlotCustomControls(CQCharts *charts) :
 CQChartsConnectionPlotCustomControls(charts, "adjacency")
{
}

void
CQChartsAdjacencyPlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  //---

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsAdjacencyPlotCustomControls::
addWidgets()
{
  addConnectionColumnWidgets();

  addColorColumnWidgets("Cell Color");
}

void
CQChartsAdjacencyPlotCustomControls::
connectSlots(bool b)
{
  CQChartsConnectionPlotCustomControls::connectSlots(b);
}

void
CQChartsAdjacencyPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  adjacencyPlot_ = dynamic_cast<CQChartsAdjacencyPlot *>(plot);

  CQChartsConnectionPlotCustomControls::setPlot(plot);
}

void
CQChartsAdjacencyPlotCustomControls::
updateWidgets()
{
  CQChartsConnectionPlotCustomControls::updateWidgets();
}

CQChartsColor
CQChartsAdjacencyPlotCustomControls::
getColorValue()
{
  return adjacencyPlot_->fillColor();
}

void
CQChartsAdjacencyPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  adjacencyPlot_->setFillColor(c);
}
