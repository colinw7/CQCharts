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
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
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
  // connections are list of node ids/counts
  startParameterGroup("Connection List");

  addColumnParameter("node", "Node", "nodeColumn").setBasic().
    setNumeric().setTip("Node Id Column");
  addColumnParameter("connections", "Connections", "connectionsColumn").setBasic().
    setTip("List of Connection Pairs (Ids from id column and connection count)").setDiscriminator();

  endParameterGroup();

  //---

  // connections are id pairs and counts
  startParameterGroup("Name Pair/Count");

  addColumnParameter("namePair", "Name Pair", "namePairColumn").setBasic().
    setTip("Connected Name Pairs (<name1>/<name2>)").setDiscriminator();
  addColumnParameter("count", "Count", "countColumn").setBasic().
    setNumeric().setTip("Connection Count");

  endParameterGroup();

  //---

  startParameterGroup("General");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setTip("Name For Node Id");

  addColumnParameter("groupId", "Group Id", "groupIdColumn").
    setNumeric().setTip("Group Id for Color");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
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
          LI("A name pair using " + B("NamePair") + " column and a count using the " +
             B("Count") + " column.") }).
     p("The connections column is in the form {{" + PARM("id") + " " + PARM("count") + "} ...}.").
     p("The name pair column is in the form " + PARM("id1") + "/" + PARM("id2")).
     p("The column id is taken from the " + B("Id") + " column and an optional "
       "name for the id can be supplied in the " + B("Name") + " column.").
     p("The group is specified using the " + B("Group") + " column.").
    h3("Options").
     p("The nodes can be sorted by group, name or count using the " + B("sortType") + " option").
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
  if      (parameter->name() == "connections") {
    return (columnDetails->type() == CQChartsPlot::ColumnType::CONNECTION_LIST);
  }
  else if (parameter->name() == "namePair") {
    return (columnDetails->type() == CQChartsPlot::ColumnType::NAME_PAIR);
  }

  return CQChartsPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsAdjacencyPlotType::
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
{
  bool hasNode        = (analyzeModelData.parameterNameColumn.find("node") !=
                         analyzeModelData.parameterNameColumn.end());
  bool hasConnections = (analyzeModelData.parameterNameColumn.find("connections") !=
                         analyzeModelData.parameterNameColumn.end());
  bool hasNamePair    = (analyzeModelData.parameterNameColumn.find("namePair") !=
                         analyzeModelData.parameterNameColumn.end());
  bool hasCount       = (analyzeModelData.parameterNameColumn.find("count") !=
                         analyzeModelData.parameterNameColumn.end());

  if (hasConnections || hasNamePair)
    return;

  auto *details = modelData->details();
  if (! details) return;

  auto *charts = modelData->charts();
  auto *model  = modelData->model().data();

  CQChartsColumn connectionsColumn;
  CQChartsColumn namePairColumn;
  CQChartsColumn countColumn;
  CQChartsColumn nodeColumn;

  int nc = details->numColumns();

  for (int c = 0; c < nc; ++c) {
    auto *columnDetails = details->columnDetails(CQChartsColumn(c));
    if (! columnDetails) continue;

    CQChartsModelIndex ind(/*row*/0, columnDetails->column(), /*parent*/QModelIndex());

    if      (columnDetails->type() == ColumnType::STRING) {
      if (! connectionsColumn.isValid()) {
        bool ok;

        auto str = CQChartsModelUtil::modelString(charts, model, ind, ok);
        if (! ok) continue;

        CQChartsConnectionList::Connections connections;

        if (CQChartsConnectionList::stringToConnections(str, connections))
          connectionsColumn = columnDetails->column();
      }

      if (! namePairColumn.isValid()) {
        bool ok;

        auto str = CQChartsModelUtil::modelString(charts, model, ind, ok);
        if (! ok) continue;

        CQChartsNamePair::Names names;

        if (CQChartsNamePair::stringToNames(str, names))
          namePairColumn = columnDetails->column();
      }
    }
    else if (columnDetails->isNumeric()) {
      if (! countColumn.isValid())
        countColumn = columnDetails->column();

      if (! nodeColumn.isValid())
        nodeColumn = columnDetails->column();
    }
  }

  if (! hasConnections && connectionsColumn.isValid()) {
    analyzeModelData.parameterNameColumn["connections"] = connectionsColumn;

    hasConnections = true;
  }

  if (! hasNamePair && namePairColumn.isValid()) {
    analyzeModelData.parameterNameColumn["namePair"] = namePairColumn;

    hasNamePair = true;
  }

  if (hasConnections && ! hasNode && nodeColumn.isValid())
    analyzeModelData.parameterNameColumn["node"] = nodeColumn;

  if (hasNamePair && ! hasCount && countColumn.isValid())
    analyzeModelData.parameterNameColumn["count"] = countColumn;
}

CQChartsPlot *
CQChartsAdjacencyPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsAdjacencyPlot(view, model);
}

//---

CQChartsAdjacencyPlot::
CQChartsAdjacencyPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("adjacency"), model),
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
setNodeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nodeColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setConnectionsColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(connectionsColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setNamePairColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(namePairColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setCountColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(countColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setGroupIdColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupIdColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsAdjacencyPlot::
setSortType(const SortType &t)
{
  CQChartsUtil::testAndSet(sortType_, t, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsAdjacencyPlot::
setBgMargin(const CQChartsLength &l)
{
  bgMargin_ = l;

  updateObjs();
}

//---

void
CQChartsAdjacencyPlot::
addProperties()
{
  addBaseProperties();

  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  // columns
  addProp("columns", "nodeColumn"       , "node"       , "Node column");
  addProp("columns", "connectionsColumn", "connections", "Connections column");
  addProp("columns", "nameColumn"       , "name"       , "Node name column");

  addProp("columns", "namePairColumn", "namePair", "Name/Value column");
  addProp("columns", "countColumn"   , "count"   , "Count column");

  addProp("columns", "groupIdColumn", "groupId", "Grouping column");

  // options
  addProp("options", "sortType", "sort"  , "Sort type");
  addProp("options", "bgMargin", "margin", "Background margin");

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

  //---

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
  bool columnsValid = true;

  th->clearErrors();

  if      (namePairColumn().isValid() && countColumn().isValid()) {
    // name pair and count required
    // groupId, name optional
    if (! checkColumn(namePairColumn(), "Name Pair",
                      th->namePairColumnType_, /*required*/true))
      columnsValid = false;

    if (! checkColumn(groupIdColumn(), "Group Id")) columnsValid = false;
    if (! checkColumn(nameColumn   (), "Name"    )) columnsValid = false;
  }
  else if (connectionsColumn().isValid()) {
    // connection required
    // groupId, node, name optional
    if (! checkColumn(connectionsColumn(), "Connections",
                      th->connectionsColumnType_, /*required*/true))
      columnsValid = false;

    if (! checkColumn(groupIdColumn(), "Group Id")) columnsValid = false;
    if (! checkColumn(nodeColumn   (), "Node"    )) columnsValid = false;
    if (! checkColumn(nameColumn   (), "Name"    )) columnsValid = false;
  }
  else {
    return th->addError("Required columns not specified");
  }

  if (! columnsValid)
    return false;

  //---

  // create objects
  bool rc = true;

  if (isHierarchical())
    rc = createHierObjs(objs);
  else {
    if      (namePairColumn().isValid() && countColumn().isValid())
      rc = initNamePairObjs(objs);
    else if (connectionsColumn().isValid())
      rc = initConnectionObjs(objs);
  }

  if (! rc)
    return false;

  //---

  return true;
}

bool
CQChartsAdjacencyPlot::
createHierObjs(PlotObjs &objs) const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
    }

    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex namePairInd(data.row, plot_->namePairColumn(), data.parent);

      bool ok;

      QString namePairStr = plot_->modelString(namePairInd, ok);

      namePairStrs_.push_back(namePairStr);

      parentStr_ = namePairStrs_.join(separator_);

      total_ = 0.0;

      return State::OK;
    }

    State hierPostVisit(const QAbstractItemModel *, const VisitData &) override {
      QString hierLinkStr = parentStr_;

      namePairStrs_.pop_back();

      parentStr_ = namePairStrs_.join(separator_);

      QString namePairStr = (namePairStrs_.length() ? namePairStrs_.back() : QString());

      //---

      auto *srcNode  = plot_->findNode(parentStr_);
      auto *destNode = plot_->findNode(hierLinkStr);

      if (! srcNode->hasNode(destNode))
        srcNode->addNode(destNode, total_);

      if (! destNode->hasNode(srcNode))
        destNode->addNode(srcNode, total_);

      return State::OK;
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex namePairInd  (data.row, plot_->namePairColumn(), data.parent);
      CQChartsModelIndex countModelInd(data.row, plot_->countColumn(), data.parent);

      bool ok1, ok2;

      QString namePairStr = plot_->modelString(namePairInd  , ok1);
      double  count       = plot_->modelReal  (countModelInd, ok2);

      if (! ok1) return addDataError(namePairInd  , "Invalid Name Pair");
      if (! ok2) return addDataError(countModelInd, "Invalid Count");

      //---

      QString hierLinkStr = parentStr_ + separator_ + namePairStr;

      auto *srcNode  = plot_->findNode(parentStr_);
      auto *destNode = plot_->findNode(hierLinkStr);

      if (! srcNode->hasNode(destNode))
        srcNode->addNode(destNode, total_);

      if (! destNode->hasNode(srcNode))
        destNode->addNode(srcNode, total_);

      //---

      total_ += count;

      return State::OK;
    }

   private:
     State addDataError(const CQChartsModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsAdjacencyPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsAdjacencyPlot* plot_ { nullptr };
    QChar                        separator_ { '/' };
    QStringList                  namePairStrs_;
    QString                      parentStr_;
    double                       total_ { 0.0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  createNameNodeObjs(objs);

  return true;
}

bool
CQChartsAdjacencyPlot::
initNamePairObjs(PlotObjs &objs) const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      int group = data.row;

      // Get group value
      if (plot_->groupIdColumn().isValid()) {
        CQChartsModelIndex groupInd(data.row, plot_->groupIdColumn(), data.parent);

        bool ok1;

        group = (int) plot_->modelInteger(groupInd, ok1);

        if (! ok1)
          return addDataError(groupInd, "Non-integer group value");
      }

      //---

      // Get name pair value
      CQChartsModelIndex namePairInd(data.row, plot_->namePairColumn(), data.parent);

      bool ok2;

      CQChartsNamePair namePair;

      if (plot_->namePairColumnType() == ColumnType::NAME_PAIR) {
        QVariant namePairVar = plot_->modelValue(namePairInd, ok2);
        assert(ok2);

        namePair = namePairVar.value<CQChartsNamePair>();
        assert(namePair.isValid());
      }
      else {
        QString namePairStr = plot_->modelString(namePairInd, ok2);
        if (! ok2) return addDataError(namePairInd, "Invalid name pair");

        namePair = CQChartsNamePair(namePairStr);
        if (! namePair.isValid()) return addDataError(namePairInd, "Invalid name pair");
      }

      //---

      // Get count value
      CQChartsModelIndex countInd(data.row, plot_->countColumn(), data.parent);

      bool ok3;

      double count = plot_->modelReal(countInd, ok3);
      if (! ok3) return addDataError(countInd, "Invalid count value");

      //---

      QString srcStr  = namePair.name1();
      QString destStr = namePair.name2();

      auto *srcNode  = plot_->findNode(srcStr );
      auto *destNode = plot_->findNode(destStr);

      if (! srcNode->hasNode(destNode))
        srcNode->addNode(destNode, count);

      if (! destNode->hasNode(srcNode))
        destNode->addNode(srcNode, count);

      //---

      srcNode->setGroup(group);

      //---

      // Get name value
      if (plot_->nameColumn().isValid()) {
        CQChartsModelIndex nameInd(data.row, plot_->nameColumn(), data.parent);

        auto nameInd1 = plot_->normalizeIndex(nameInd);

        srcNode->setInd(nameInd1);
      }

      return State::OK;
    }

   private:
    State addDataError(const CQChartsModelIndex &ind, const QString &msg) {
      const_cast<CQChartsAdjacencyPlot *>(plot_)->addDataError(ind, msg);
      return State::SKIP;
    }

   private:
    const CQChartsAdjacencyPlot *plot_ { nullptr };
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

  int nn = numNodes();

  if (nn + maxLen()*factor_ > 0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*factor_*scale();

  //---

  double y = 1.0 - tsize;

  for (auto &node1 : sortedNodes_) {
    double x = tsize;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || ! CMathUtil::isZero(value)) {
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

  const IdConnectionsData &idConnectionsData = visitor.idConnections();

  //---

  for (const auto &idConnections : idConnectionsData) {
    int         id    = idConnections.first;
    const auto &ind   = idConnections.second.ind;
    const auto &name  = idConnections.second.name;
    int         group = idConnections.second.group;

    auto *node = new CQChartsAdjacencyNode(id, name, group, ind);

    th->nodes_[id] = node;
  }

  //---

  for (const auto &idConnections : idConnectionsData) {
    int                    id          = idConnections.first;
    const ConnectionsData &connections = idConnections.second;

    auto node = th->nodes_[id];

    for (const auto &connection : connections.connections) {
      auto node1 = th->nodes_[connection.node];

      node->addNode(node1, connection.count);
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

  int nn = numNodes();

  if (nn + maxLen()*factor_ > 0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*factor_*scale();

  //---

  double y = 1.0 - tsize - yb;

  for (auto &node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || ! CMathUtil::isZero(value)) {
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

CQChartsAdjacencyObj *
CQChartsAdjacencyPlot::
createObj(CQChartsAdjacencyNode *node1, CQChartsAdjacencyNode *node2, double value,
          const CQChartsGeom::BBox &rect, const ColorInd &ig)
{
  return new CQChartsAdjacencyObj(this, node1, node2, value, rect, ig);
}

bool
CQChartsAdjacencyPlot::
getRowConnections(const ModelVisitor::VisitData &data, ConnectionsData &connections) const
{
  // get optional group id
  int group = data.row;

  if (groupIdColumn().isValid()) {
    CQChartsModelIndex groupInd(data.row, groupIdColumn(), data.parent);

    bool ok1;

    int group1 = (int) modelInteger(groupInd, ok1);

    if (ok1)
      group = group1;
  }

  //---

  // get optional id
  int id = data.row;

  CQChartsModelIndex nodeInd;

  if (nodeColumn().isValid()) {
    nodeInd = CQChartsModelIndex(data.row, nodeColumn(), data.parent);

    bool ok2;

    int id1 = (int) modelInteger(nodeInd, ok2);

    if (ok2)
      id = id1;
  }

  //---

  // get connections
  CQChartsModelIndex connectionsInd(data.row, connectionsColumn(), data.parent);

  bool ok3;

  if (connectionsColumnType() == ColumnType::CONNECTION_LIST) {
    QVariant connectionsVar = modelValue(connectionsInd, ok3);

    connections.connections = connectionsVar.value<CQChartsConnectionList>().connections();
  }
  else {
    QString connectionsStr = modelString(connectionsInd, ok3);

    if (! ok3)
      return false;

    decodeConnections(connectionsStr, connections.connections);
  }

  //----

  // get optional name
  QString name;

  if (nameColumn().isValid()) {
    CQChartsModelIndex nameInd(data.row, nameColumn(), data.parent);

    bool ok4;

    QString name1 = modelString(nameInd, ok4);

    if (ok4)
      name = name1;
  }

  if (! name.length())
    name = QString("%1").arg(id);

  //---

  // return connections data
  if (nodeInd.isValid()) {
    auto nodeInd1 = normalizeIndex(nodeInd);

    connections.ind = nodeInd1;
  }

  connections.node  = id;
  connections.name  = name;
  connections.group = group;

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

    nodeData.maxValue = std::max(nodeData.maxValue, node->maxCount());
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
        if (lhs->count() != rhs->count())
          return lhs->count() < rhs->count();

        return lhs->name() < rhs->name();
      });
  }
}

bool
CQChartsAdjacencyPlot::
decodeConnections(const QString &str, Connections &connections) const
{
  return CQChartsConnectionList::stringToConnections(str, connections);
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

  auto *node = new CQChartsAdjacencyNode(id, str, 0, CQChartsModelIndex());

  auto *th = const_cast<CQChartsAdjacencyPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(str, node));

  return (*p1).second;
}

//---

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
  QPen tpen;

  QColor tc = interpTextColor(ColorInd());

  setPen(tpen, true, tc, textAlpha());

  device->setPen(tpen);

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

  QBrush fillBrush;

  QColor fc = interpBackgroundFillColor(ColorInd());

  setBrush(fillBrush, true, fc, backgroundFillAlpha(), backgroundFillPattern());

  CQChartsGeom::BBox cellBBox(px, py, px + std::max(nn, 1)*pxs, py + std::max(nn, 1)*pys);

  device->fillRect(device->pixelToWindow(cellBBox), fillBrush);

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

  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node1 : sortedNodes_) {
    double px = po.x + lengthPixelWidth(bgMargin()) + xts;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      bool empty = (node1 != node2 && CMathUtil::isZero(value));

      if (empty) {
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

  addModelInd(plot->modelIndex(node1->ind()));
  addModelInd(plot->modelIndex(node2->ind()));
}

QString
CQChartsAdjacencyObj::
calcId() const
{
  QString groupStr1 = QString("(%1)").arg(node1_->group());
  QString groupStr2 = QString("(%1)").arg(node2_->group());

  return QString("%1:%2%3:%4%5:%6").arg(typeName()).
           arg(node1_->name()).arg(groupStr1).arg(node2_->name()).arg(groupStr2).arg(value());
}

QString
CQChartsAdjacencyObj::
calcTipId() const
{
  QString groupStr1 = QString("(%1)").arg(node1_->group());
  QString groupStr2 = QString("(%1)").arg(node2_->group());

  CQChartsTableTip tableTip;

  tableTip.addTableRow("From" , node1_->name(), groupStr1);
  tableTip.addTableRow("To"   , node2_->name(), groupStr2);
  tableTip.addTableRow("Value", value());

  //---

  //plot()->addTipColumns(tableTip, node1_->ind());

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
  if (node1_ == node2_) {
    bc = interpGroupColor(node1_);
  }
  // node to other node (scale to connections)
  else {
    QColor c1 = interpGroupColor(node1_);
    QColor c2 = interpGroupColor(node2_);

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
writeScriptData(CQChartsScriptPainter *device) const
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
    return node1_->id();
  else
    return CMathUtil::map(node1_->id(), 0.0, plot_->maxNode(), 0.0, 1.0);
}

double
CQChartsAdjacencyObj::
yColorValue(bool relative) const
{
  if (! relative)
    return node2_->id();
  else
    return CMathUtil::map(node2_->id(), 0.0, plot_->maxNode(), 0.0, 1.0);
}
