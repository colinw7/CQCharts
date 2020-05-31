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

  setOuterMargin(0, 0, 0, 0);

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
  CQChartsGeom::Range dataRange;

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

  //---

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->clearNodes();

  //---

  th->setInsideObj(nullptr);

  //---

  // check columns
  if (! checkColumns())
    return false;

  //---

  // create objects
  bool rc = true;

  if (isHierarchical())
    rc = initHierObjs(objs);
  else {
    if      (linkColumn().isValid() && valueColumn().isValid())
      rc = initLinkObjs(objs);
    else if (connectionsColumn().isValid())
      rc = initConnectionObjs(objs);
    else
      rc = initTableObjs(objs);
  }

  if (! rc)
    return false;

  //---

  return true;
}

bool
CQChartsAdjacencyPlot::
initHierObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::initHierObjs");

  CQChartsConnectionPlot::initHierObjs();

  createNameNodeObjs(objs);

  return true;
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
    srcNode->addNode(destNode, destValue);

  if (! destNode->hasNode(srcNode))
    destNode->addNode(srcNode, destValue);
}

bool
CQChartsAdjacencyPlot::
initLinkObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::initLinkObjs");

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
      separator_ = (plot_->separator().length() ? plot_->separator()[0] : '/');
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // Get group value
      int group = data.row;

      CQChartsModelIndex groupModelInd;

      if (plot_->groupColumn().isValid()) {
        groupModelInd = CQChartsModelIndex(data.row, plot_->groupColumn(), data.parent);

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
      if (! ok1) return addDataError(valueModelInd, "Invalid value");

      //---

      // Get name value
      CQChartsModelIndex nameModelInd;

      if (plot_->nameColumn().isValid())
        nameModelInd = CQChartsModelIndex(data.row, plot_->nameColumn(), data.parent);

      //---

      CQChartsModelIndex modelInd = nameModelInd;

      if (! modelInd.isValid())
        modelInd = linkModelInd;

      if (! modelInd.isValid())
        modelInd = valueModelInd;

      //---

      QString srcStr  = namePair.name1();
      QString destStr = namePair.name2();

      addConnection(srcStr, destStr, value, group, modelInd);

      return State::OK;
    }

    void addConnection(const QString &srcStr, const QString &destStr, double value,
                       int group, const CQChartsModelIndex &modelInd) {
      auto *srcNode  = plot_->findNode(srcStr);
      auto *destNode = plot_->findNode(destStr);

    //assert(srcNode != destNode);

      if (! srcNode->hasNode(destNode))
        srcNode->addNode(destNode, value);

      // connectional is directional (optional ?)
#if 0
      if (! destNode->hasNode(srcNode))
        destNode->addNode(srcNode, value);
#endif

      srcNode->setGroup(group);

      if (modelInd.isValid()) {
        auto modelInd1 = plot_->normalizeIndex(modelInd);

        srcNode ->setInd(destNode->id(), modelInd1);
        destNode->setInd(srcNode ->id(), modelInd1);
      }
    }

   private:
    State addDataError(const CQChartsModelIndex &ind, const QString &msg) {
      const_cast<CQChartsAdjacencyPlot *>(plot_)->addDataError(ind, msg);
      return State::SKIP;
    }

   private:
    const CQChartsAdjacencyPlot *plot_      { nullptr };
    QChar                        separator_ { '/' };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  createNameNodeObjs(objs);

  return true;
}

void
CQChartsAdjacencyPlot::
createNameNodeObjs(PlotObjs &objs) const
{
  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  //---

  for (const auto &nameNode : nameNodeMap_) {
    const auto &node = nameNode.second;

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

  int nn = numNodes();

  if (nn + maxLen()*factor_ > 0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*factor_*scale();

  //---

  double equalValue = 0.0;

  if (isForceDiagonal())
    equalValue = 1.0;

  double y = 1.0 - tsize;

  for (auto &node1 : sortedNodes_) {
    double x = tsize;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2, equalValue);

      // skip unconnected
      bool connected = ! CMathUtil::isZero(value);

      if (connected) {
        CQChartsGeom::BBox bbox(x, y - scale(), x + scale(), y);

        ColorInd ig(node1->group(), maxGroup() + 1);

        auto *obj = th->createObj(node1, node2, value, bbox, ig);

        objs.push_back(obj);
      }

      x += scale();
    }

    y -= scale();
  }
}

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

      node->addNode(node1, connection.value);
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

  int nn = numNodes();

  if (nn + maxLen()*factor_ > 0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*factor_*scale();

  //---

  double equalValue = 0.0;

  if (isForceDiagonal())
    equalValue = 1.0;

  double y = 1.0 - tsize - yb;

  for (auto &node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2, equalValue);

      // skip unconnected
      bool connected = ! CMathUtil::isZero(value);

      if (connected) {
        CQChartsGeom::BBox bbox(x, y - scale(), x + scale(), y);

        ColorInd ig(node1->group(), maxGroup() + 1);

        auto *obj = th->createObj(node1, node2, value, bbox, ig);

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
initTableObjs(PlotObjs &objs) const
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
    srcNode->setGroup(tableConnectionData.group().i);

    for (const auto &value : tableConnectionData.values()) {
      QString destStr = QString("%1").arg(value.to);

      auto *destNode = findNode(destStr);

      if (! srcNode->hasNode(destNode))
        srcNode->addNode(destNode, value.toValue);
    }
  }

  //---

  createNameNodeObjs(objs);

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
    CQChartsModelIndex groupInd(data.row, groupColumn(), data.parent);

    bool ok1;

    int group1 = (int) modelInteger(groupInd, ok1);

    if (ok1)
      group = group1;
  }

  //---

  // get optional node id (default to row)
  CQChartsModelIndex nodeModelInd;

  int id = data.row;

  if (nodeColumn().isValid()) {
    nodeModelInd = CQChartsModelIndex(data.row, nodeColumn(), data.parent);

    bool ok2;
    id = (int) modelInteger(nodeModelInd, ok2);
    if (! ok2) return th->addDataError(nodeModelInd, "Non-integer node value");
  }

  //---

  // get connections
  CQChartsModelIndex connectionsInd(data.row, connectionsColumn(), data.parent);

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
  CQChartsModelIndex nameModelInd;

  QString name = QString("%1").arg(id);

  if (nameColumn().isValid()) {
    nameModelInd = CQChartsModelIndex(data.row, nameColumn(), data.parent);

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
        if (lhs->value() != rhs->value())
          return lhs->value() < rhs->value();

        return lhs->name() < rhs->name();
      });
  }
}

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

CQChartsAdjacencyObj *
CQChartsAdjacencyPlot::
createObj(CQChartsAdjacencyNode *node1, CQChartsAdjacencyNode *node2, double value,
          const CQChartsGeom::BBox &rect, const ColorInd &ig)
{
  return new CQChartsAdjacencyObj(this, node1, node2, value, rect, ig);
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
  auto po = windowToPixel(CQChartsGeom::Point(0.0, 1.0));

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

    CQChartsGeom::Point pt(px + xts - tw - 2, py + pys - fm.descent()); // align right

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
    CQChartsGeom::Point p1(px + pxs/2, py - 2);

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

  int nn = numNodes();

  px = po.x + lengthPixelWidth (bgMargin()) + xts;
  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  //---

  QColor fc = interpBackgroundFillColor(ColorInd());

  setPenBrush(device,
   CQChartsPenData  (false),
   CQChartsBrushData(true, fc, backgroundFillAlpha(), backgroundFillPattern()));

  CQChartsGeom::BBox cellBBox(px, py, px + std::max(nn, 1)*pxs, py + std::max(nn, 1)*pys);

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

  if (isForceDiagonal())
    equalValue = 1.0;

  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node1 : sortedNodes_) {
    double px = po.x + lengthPixelWidth(bgMargin()) + xts;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2, equalValue);

      // draw unconnected
      bool connected = ! CMathUtil::isZero(value);

      if (! connected) {
        CQChartsGeom::BBox cellBBox =
          device->pixelToWindow(CQChartsGeom::BBox(px, py, px + pxs, py + pys));

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

CQChartsAdjacencyObj::
CQChartsAdjacencyObj(const CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                     CQChartsAdjacencyNode *node2, double value, const CQChartsGeom::BBox &rect,
                     const ColorInd &ig) :
 CQChartsPlotObj(const_cast<CQChartsAdjacencyPlot *>(plot), rect, ColorInd(), ig, ColorInd()),
 plot_(plot), node1_(node1), node2_(node2), value_(value)
{
  setDetailHint(DetailHint::MAJOR);

  CQChartsModelIndex ind1 = node1->ind(node2->id());
  CQChartsModelIndex ind2 = node2->ind(node1->id());

  if      (ind1.isValid())
    addModelInd(plot->modelIndex(ind1));
  else if (ind2.isValid())
    addModelInd(plot->modelIndex(ind2));
}

QString
CQChartsAdjacencyObj::
calcId() const
{
  QString groupStr1 = QString("(%1)").arg(node1()->group());
  QString groupStr2 = QString("(%1)").arg(node2()->group());

  return QString("%1:%2%3:%4%5:%6").arg(typeName()).
           arg(node1()->name()).arg(groupStr1).arg(node2()->name()).arg(groupStr2).arg(value());
}

QString
CQChartsAdjacencyObj::
calcTipId() const
{
  QString groupStr1 = QString("(%1)").arg(node1()->group());
  QString groupStr2 = QString("(%1)").arg(node2()->group());

  CQChartsTableTip tableTip;

  tableTip.addTableRow("From" , node1()->name(), groupStr1);
  tableTip.addTableRow("To"   , node2()->name(), groupStr2);
  tableTip.addTableRow("Value", value());

  if (node1() == node2())
    tableTip.addTableRow("Group", node1()->group());

  //---

  //plot()->addTipColumns(tableTip, node1()->ind());

  //---

  return tableTip.str();
}

void
CQChartsAdjacencyObj::
getObjSelectIndices(Indices &inds) const
{
  for (auto &ind : modelInds())
    inds.insert(ind);
}

void
CQChartsAdjacencyObj::
draw(CQChartsPaintDevice *device)
{
  // draw inside object
  if (isInside()) {
    if (plot_->insideObj() != this) {
      auto *plot = const_cast<CQChartsAdjacencyPlot *>(plot_);

      plot->setInsideObj(const_cast<CQChartsAdjacencyObj *>(this));

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

  drawRoundedPolygon(device, penBrush, CQChartsGeom::BBox(rect()), plot_->cornerSize());

  device->resetColorNames();
}

void
CQChartsAdjacencyObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  ColorInd colorInd = calcColorInd();

  //---

  auto interpGroupColor = [&](CQChartsAdjacencyNode *node) {
    if (plot_->colorType() == CQChartsPlot::ColorType::AUTO)
      return plot_->interpGroupColor(node->group());
    else
      return plot_->interpFillColor(colorInd);
  };

  //---

  // calc fill color
  QColor bc = plot_->interpEmptyCellFillColor(ColorInd());

  // node to self (diagonal)
  if (node1() == node2()) {
    bc = interpGroupColor(node1());
  }
  // node to other node (scale to connections)
  else {
    QColor c1 = interpGroupColor(node1());
    QColor c2 = interpGroupColor(node2());

    double s = CMathUtil::map(value(), 0.0, plot_->maxValue(), 0.0, 1.0);

    double r = (c1.redF  () + c2.redF  () + s*bc.redF  ())/3.0;
    double g = (c1.greenF() + c2.greenF() + s*bc.greenF())/3.0;
    double b = (c1.blueF () + c2.blueF () + s*bc.blueF ())/3.0;

    bc = QColor::fromRgbF(r, g, b);
  }

  //---

  // calc pen and brush
  QColor pc = plot_->interpStrokeColor(colorInd);

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (true, pc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash()),
    CQChartsBrushData(true, bc, plot_->fillAlpha(), plot_->fillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsAdjacencyObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.value = " << value() << ";\n";
}

double
CQChartsAdjacencyObj::
xColorValue(bool relative) const
{
  if (! relative)
    return node1()->id();
  else
    return CMathUtil::map(node1()->id(), 0.0, plot_->maxNode(), 0.0, 1.0);
}

double
CQChartsAdjacencyObj::
yColorValue(bool relative) const
{
  if (! relative)
    return node2()->id();
  else
    return CMathUtil::map(node2()->id(), 0.0, plot_->maxNode(), 0.0, 1.0);
}
