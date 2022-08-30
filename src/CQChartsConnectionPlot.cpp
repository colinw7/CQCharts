#include <CQChartsConnectionPlot.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsNamePair.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQEnumCombo.h>

#include <QLabel>
#include <QGridLayout>

CQChartsConnectionPlotType::
CQChartsConnectionPlotType()
{
}

void
CQChartsConnectionPlotType::
addParameters()
{
  // connections are list of node ids/values
  startParameterGroup("Connection List");

  addColumnParameter("node", "Node", "nodeColumn").setBasic().
    setNumericColumn().setPropPath("columns.node").setTip("Node Id Column");
  addColumnParameter("connections", "Connections", "connectionsColumn").setBasic().
    setPropPath("columns.connections").
    setTip("List of Connection Pairs (Ids from id column and connection value)").setDiscriminator();

  endParameterGroup();

  //---

  // connections are id pairs and values
  startParameterGroup("Link/Path Value");

  addColumnParameter("link", "Link", "linkColumn").setBasic().
    setPropPath("columns.link").setTip("Name pair for Source/Target connection").setDiscriminator();
  addColumnParameter("path", "Path", "pathColumn").setBasic().
    setPropPath("columns.path").setTip("Path for connection hierarchy").setDiscriminator();
  addColumnParameter("from", "From", "fromColumn").setBasic().
    setPropPath("columns.from").setTip("From connection node").setDiscriminator();
  addColumnParameter("to"  , "To", "toColumn").setBasic().
    setPropPath("columns.to").setTip("To connection node").setDiscriminator();

  addColumnParameter("value", "Value", "valueColumn").setBasic().
    setNumericColumn().setPropPath("columns.value").setTip("Connection value");

  endParameterGroup();

  //---

  startParameterGroup("General");

  addColumnParameter("depth", "Depth", "depthColumn").setBasic().
    setNumericColumn().setPropPath("columns.depth").setTip("Connection depth");
#ifdef CQCHARTS_GRAPH_PATH_ID
  addColumnParameter("pathId", "PathId", "pathIdColumn").
    setStringColumn().setPropPath("columns.pathId").setTip("Path Id");
#endif
  addColumnParameter("attributes", "Attributes", "attributesColumn").
    setStringColumn().setPropPath("columns.attributes").setTip("Node/Edge attributes");

  addColumnParameter("group", "Group", "groupColumn").
    setNumericColumn().setPropPath("columns.group").setTip("Group column");
  addColumnParameter("name", "Name", "nameColumn").
    setStringColumn().setPropPath("columns.name").setTip("Optional node name");

  endParameterGroup();

  //---

  addStringParameter("separator", "Separator", "separator", "/").setBasic().
   setTip("Separator for name pair in link column");

  //---

  CQChartsPlotType::addParameters();
}

bool
CQChartsConnectionPlotType::
isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const
{
  if      (parameter->name() == "connections") {
    return (columnDetails->type() == CQChartsPlot::ColumnType::CONNECTION_LIST);
  }
  else if (parameter->name() == "link") {
    return (columnDetails->type() == CQChartsPlot::ColumnType::NAME_PAIR);
  }

  return CQChartsPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsConnectionPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  bool hasNode        = (analyzeModelData.parameterNameColumn.find("node") !=
                         analyzeModelData.parameterNameColumn.end());
  bool hasConnections = (analyzeModelData.parameterNameColumn.find("connections") !=
                         analyzeModelData.parameterNameColumn.end());
  bool hasLink        = (analyzeModelData.parameterNameColumn.find("link") !=
                         analyzeModelData.parameterNameColumn.end());
  bool hasValue       = (analyzeModelData.parameterNameColumn.find("value") !=
                         analyzeModelData.parameterNameColumn.end());

  if (hasConnections || hasLink)
    return;

  auto *details = modelData->details();
  if (! details) return;

  auto *charts = modelData->charts();
  auto *model  = modelData->model().data();

  Column nodeColumn;
  Column connectionsColumn;
  Column linkColumn;
  Column valueColumn;

  int nc = details->numColumns();

  for (int c = 0; c < nc; ++c) {
    auto *columnDetails = details->columnDetails(Column(c));
    if (! columnDetails) continue;

    int         row = 0;
    QModelIndex parent;

    //ModelIndex ind(nullptr, row, columnDetails->column(), parent);

    if      (columnDetails->type() == ColumnType::STRING) {
      if (! connectionsColumn.isValid()) {
        bool ok;

        auto str = CQChartsModelUtil::modelString(charts, model, row, columnDetails->column(),
                                                  parent, ok);
        if (! ok) continue;

        CQChartsConnectionList::Connections connections;

        if (CQChartsConnectionList::stringToConnections(str, connections))
          connectionsColumn = columnDetails->column();
      }

      if (! linkColumn.isValid()) {
        bool ok;

        auto str = CQChartsModelUtil::modelString(charts, model, row, columnDetails->column(),
                                                  parent, ok);
        if (! ok) continue;

        CQChartsNamePair::Names names;

        if (CQChartsNamePair::stringToNames(str, names))
          linkColumn = columnDetails->column();
      }
    }
    else if (columnDetails->isNumeric()) {
      if (! valueColumn.isValid())
        valueColumn = columnDetails->column();

      if (! nodeColumn.isValid())
        nodeColumn = columnDetails->column();
    }
  }

  if (! hasConnections && connectionsColumn.isValid()) {
    analyzeModelData.parameterNameColumn["connections"] = connectionsColumn;

    hasConnections = true;
  }

  if (! hasLink && linkColumn.isValid()) {
    analyzeModelData.parameterNameColumn["link"] = linkColumn;

    hasLink = true;
  }

  if (hasConnections && ! hasNode && nodeColumn.isValid())
    analyzeModelData.parameterNameColumn["node"] = nodeColumn;

  if (! hasValue && valueColumn.isValid())
    analyzeModelData.parameterNameColumn["value"] = valueColumn;

  //---

  if (! hasLink) {
    if (details->isHierarchical())
      return;

    int nr = details->numRows();

    if (nr != nc - 1 && nr != nc - 2)
      return;

    int skip = (nr == nc - 2 ? 1 : 0);

    bool allNumeric = true;

    for (int c = skip + 1; c < nc; ++c) {
      auto *columnDetails = details->columnDetails(Column(c));
      if (! columnDetails) continue;

      if (! columnDetails->isNumeric()) {
        allNumeric = false;
        break;
      }
    }

    if (! allNumeric)
      return;

    analyzeModelData.parameterNameColumn["link"] = Column(0);

    if (skip == 1)
      analyzeModelData.parameterNameColumn["group"] = Column(1);
  }
}

//------

CQChartsConnectionPlot::
CQChartsConnectionPlot(View *view, PlotType *plotType, const ModelP &model) :
 CQChartsPlot(view, plotType, model)
{
}

CQChartsConnectionPlot::
~CQChartsConnectionPlot()
{
  CQChartsConnectionPlot::term();
}

//---

void
CQChartsConnectionPlot::
init()
{
  CQChartsPlot::init();
}

void
CQChartsConnectionPlot::
term()
{
}

//---

void
CQChartsConnectionPlot::
setNodeColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nodeColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsConnectionPlot::
setConnectionsColumn(const Column &c)
{
  CQChartsUtil::testAndSet(connectionsColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsConnectionPlot::
setLinkColumn(const Column &c)
{
  CQChartsUtil::testAndSet(linkColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsConnectionPlot::
setPathColumn(const Column &c)
{
  CQChartsUtil::testAndSet(pathColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsConnectionPlot::
setFromColumn(const Column &c)
{
  CQChartsUtil::testAndSet(fromColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsConnectionPlot::
setToColumn(const Column &c)
{
  CQChartsUtil::testAndSet(toColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsConnectionPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsConnectionPlot::
setDepthColumn(const Column &c)
{
  CQChartsUtil::testAndSet(depthColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsConnectionPlot::
setGroupColumn(const Column &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

#ifdef CQCHARTS_GRAPH_PATH_ID
void
CQChartsConnectionPlot::
setPathIdColumn(const Column &c)
{
  CQChartsUtil::testAndSet(pathIdColumn_, c, [&]() { updateRangeAndObjs(); } );
}
#endif

void
CQChartsConnectionPlot::
setAttributesColumn(const Column &c)
{
  CQChartsUtil::testAndSet(attributesColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsConnectionPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsConnectionPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "group"      ) c = this->groupColumn();
  else if (name == "node"       ) c = this->nodeColumn();
  else if (name == "connections") c = this->connectionsColumn();
  else if (name == "link"       ) c = this->linkColumn();
  else if (name == "path"       ) c = this->pathColumn();
  else if (name == "from"       ) c = this->fromColumn();
  else if (name == "to"         ) c = this->toColumn();
  else if (name == "value"      ) c = this->valueColumn();
  else if (name == "depth"      ) c = this->depthColumn();
  else if (name == "name"       ) c = this->nameColumn();
  else                            c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsConnectionPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "group"      ) this->setGroupColumn(c);
  else if (name == "node"       ) this->setNodeColumn(c);
  else if (name == "connections") this->setConnectionsColumn(c);
  else if (name == "link"       ) this->setLinkColumn(c);
  else if (name == "path"       ) this->setPathColumn(c);
  else if (name == "from"       ) this->setFromColumn(c);
  else if (name == "to"         ) this->setToColumn(c);
  else if (name == "value"      ) this->setValueColumn(c);
  else if (name == "depth"      ) this->setDepthColumn(c);
  else if (name == "name"       ) this->setNameColumn(c);
  else                            CQChartsPlot::setNamedColumn(name, c);
}

//---

void
CQChartsConnectionPlot::
setSeparator(const QString &s)
{
  CQChartsUtil::testAndSet(separator_, s, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setSymmetric(bool b)
{
  CQChartsUtil::testAndSet(symmetric_, b, [&]() { updateObjs(); } );
}

void
CQChartsConnectionPlot::
setSorted(bool b)
{
  CQChartsUtil::testAndSet(sorted_, b, [&]() { updateObjs(); } );
}

void
CQChartsConnectionPlot::
setPropagate(bool b)
{
  CQChartsUtil::testAndSet(propagateData_.active, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setMaxDepth(int d)
{
  CQChartsUtil::testAndSet(maxDepth_, d, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setMinValue(double r)
{
  CQChartsUtil::testAndSet(minValue_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setHierName(bool b)
{
  CQChartsUtil::testAndSet(hierName_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsConnectionPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "nodeColumn"       , "node"      , "Node column");
  addProp("columns", "connectionsColumn", "connection", "Connections column");

  addProp("columns", "linkColumn" , "link" , "Link column");
  addProp("columns", "pathColumn" , "path" , "Path column");
  addProp("columns", "fromColumn" , "from" , "From column");
  addProp("columns", "toColumn"   , "to"   , "To column");
  addProp("columns", "valueColumn", "value", "Value column");
  addProp("columns", "depthColumn", "depth", "Depth column");

#ifdef CQCHARTS_GRAPH_PATH_ID
  addProp("columns", "pathIdColumn"    , "pathId"    , "Path Id column");
#endif
  addProp("columns", "attributesColumn", "attributes", "Attributes column");

  addProp("columns", "groupColumn", "group", "Grouping column");
  addProp("columns", "nameColumn" , "name" , "Node name column");

  addProp("options", "separator", "", "Model link value separator");
  addProp("options", "symmetric", "", "Model values are symmetric");
  addProp("options", "sorted"   , "", "Sort values by size (table columns)");
  addProp("options", "maxDepth" , "", "Max Node depth");
  addProp("options", "minValue" , "", "Min Node value");
  addProp("options", "propagate", "", "Propagate values up hierarchy");
  addProp("options", "hierName" , "", "Name is hierarchical");
}

//---

CQChartsConnectionPlot::ColumnDataType
CQChartsConnectionPlot::
calcColumnDataType() const
{
  if (isHierarchical()) {
    if (linkColumn().isValid())
      return ColumnDataType::HIER;
  }
  else {
    if      (linkColumn().isValid() && valueColumn().isValid())
      return ColumnDataType::LINK;
    else if (connectionsColumn().isValid())
      return ColumnDataType::CONNECTIONS;
    else if (pathColumn().isValid())
      return ColumnDataType::PATH;
    else if (fromColumn().isValid() && toColumn().isValid())
      return ColumnDataType::FROM_TO;
    else if (linkColumn().isValid())
      return ColumnDataType::TABLE;
  }

  return defaultColumnDataType_;
}

void
CQChartsConnectionPlot::
setCalcColumnDataType(const ColumnDataType &columnDataType)
{
  if (columnDataType != calcColumnDataType()) {
    if      (columnDataType == ColumnDataType::HIER) {
      connectionsColumn_ = Column();
      pathColumn_        = Column();
      fromColumn_        = Column();
      toColumn_          = Column();
    }
    else if (columnDataType == ColumnDataType::LINK) {
      connectionsColumn_ = Column();
      pathColumn_        = Column();
      fromColumn_        = Column();
      toColumn_          = Column();
    }
    else if (columnDataType == ColumnDataType::CONNECTIONS) {
      linkColumn_        = Column();
      valueColumn_       = Column();
      pathColumn_        = Column();
      fromColumn_        = Column();
      toColumn_          = Column();
    }
    else if (columnDataType == ColumnDataType::PATH) {
      linkColumn_        = Column();
      connectionsColumn_ = Column();
      fromColumn_        = Column();
      toColumn_          = Column();
    }
    else if (columnDataType == ColumnDataType::FROM_TO) {
      linkColumn_        = Column();
      connectionsColumn_ = Column();
      pathColumn_        = Column();
    }
    else if (columnDataType == ColumnDataType::TABLE) {
      valueColumn_       = Column();
      connectionsColumn_ = Column();
      pathColumn_        = Column();
      fromColumn_        = Column();
      toColumn_          = Column();
    }

    updateRangeAndObjs();
  }

  defaultColumnDataType_ = columnDataType;
}

//---

bool
CQChartsConnectionPlot::
checkColumns() const
{
  auto *th = const_cast<CQChartsConnectionPlot *>(this);

  // check columns
  bool columnsValid = true;

  modelColumns_.clear();

  if      (linkColumn().isValid()) {
    // link required
    if (checkColumn(linkColumn(), "Link", th->linkColumnType_, /*required*/true))
      modelColumns_.push_back(linkColumn());
    else
      columnsValid = false;
  }
  else if (connectionsColumn().isValid()) {
    // connection required
    if (checkColumn(connectionsColumn(), "Connections",
                    th->connectionsColumnType_, /*required*/true))
      modelColumns_.push_back(connectionsColumn());
    else
      columnsValid = false;

    if (checkColumn(nodeColumn(), "Node"))
      modelColumns_.push_back(nodeColumn());
    else
      columnsValid = false;
  }
#ifdef CQCHARTS_GRAPH_PATH_ID
  else if (pathColumn().isValid()) {
    modelColumns_.push_back(pathColumn());
  }
#endif
  else if (fromColumn().isValid() && toColumn().isValid()) {
    modelColumns_.push_back(fromColumn());
    modelColumns_.push_back(toColumn  ());
  }
  else {
    return th->addError("Required columns not specified");
  }

#ifdef CQCHARTS_GRAPH_PATH_ID
  // pathId optional
  if (checkColumn(pathIdColumn(), "PathId"))
    modelColumns_.push_back(pathIdColumn());
  else
    columnsValid = false;
#endif

  // attributes optional
  if (checkColumn(attributesColumn(), "Attributes"))
    modelColumns_.push_back(attributesColumn());
  else
    columnsValid = false;

  // value optional
  if (checkNumericColumn(valueColumn(), "Value"))
    modelColumns_.push_back(valueColumn());
  else
    columnsValid = false;

  // depth optional
  if (checkColumn(depthColumn(), "Depth"))
    modelColumns_.push_back(depthColumn());
  else
    columnsValid = false;

  // group, name optional
  if (checkColumn(groupColumn(), "Group"))
    modelColumns_.push_back(groupColumn());
  else
    columnsValid = false;

  if (checkColumn(nameColumn(), "Name" ))
    modelColumns_.push_back(nameColumn());
  else
    columnsValid = false;

  return columnsValid;
}

//---

bool
CQChartsConnectionPlot::
initLinkObjs() const
{
  CQPerfTrace trace("CQChartsConnectionPlot::initLinkObjs");

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsConnectionPlot *plot) :
     plot_(plot) {
      separator_ = plot_->calcSeparator();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      LinkConnectionData linkConnectionData;

      // Get group value
      if (plot_->groupColumn().isValid()) {
        linkConnectionData.groupModelInd =
          ModelIndex(plot_, data.row, plot_->groupColumn(), data.parent);

        if (! plot_->groupColumnData(linkConnectionData.groupModelInd,
                                     linkConnectionData.groupData)) {
          return addDataError(linkConnectionData.groupModelInd, "Invalid group value");
        }
      }
      else {
        linkConnectionData.groupData = GroupData(QVariant(), data.row, numRows());
      }

      //---

      // Get link value
      linkConnectionData.linkModelInd =
        ModelIndex(plot_, data.row, plot_->linkColumn(), data.parent);

      CQChartsNamePair namePair;

      if (plot_->linkColumnType() == ColumnType::NAME_PAIR) {
        bool ok;
        auto linkVar = plot_->modelValue(linkConnectionData.linkModelInd, ok);
        if (! ok) return addDataError(linkConnectionData.linkModelInd, "Invalid Link");

        namePair = CQChartsNamePair::fromVariant(linkVar);
      }
      else {
        bool ok;
        auto linkStr = plot_->modelString(linkConnectionData.linkModelInd, ok);
        if (! ok) return addDataError(linkConnectionData.linkModelInd, "Invalid Link");

        namePair = CQChartsNamePair(linkStr, separator_);
      }

      if (! namePair.isValid())
        return addDataError(linkConnectionData.linkModelInd, "Invalid Link");

      //---

      // Get value value
      if (plot_->valueColumn().isValid()) {
        linkConnectionData.valueModelInd =
          ModelIndex(plot_, data.row, plot_->valueColumn(), data.parent);

        bool ok1;
        double value = plot_->modelReal(linkConnectionData.valueModelInd, ok1);
        if (! ok1) return addDataError(linkConnectionData.valueModelInd, "Invalid Value");

        linkConnectionData.value = OptReal(value);
      }

      //---

      // Get name value
      if (plot_->nameColumn().isValid())
        linkConnectionData.nameModelInd =
          ModelIndex(plot_, data.row, plot_->nameColumn(), data.parent);

      //---

      // Get depth value
      if (plot_->depthColumn().isValid()) {
        linkConnectionData.depthModelInd =
          ModelIndex(plot_, data.row, plot_->depthColumn(), data.parent);

        bool ok2;
        long depth = plot_->modelInteger(linkConnectionData.depthModelInd, ok2);
        if (! ok2) return addDataError(linkConnectionData.depthModelInd, "Non-integer depth value");

        linkConnectionData.depth = static_cast<int>(depth);
      }

      //---

      // get attributes from optional column
      if (plot_->attributesColumn().isValid()) {
        ModelIndex attributesModelInd(plot_, data.row, plot_->attributesColumn(), data.parent);

        bool ok4;
        auto attributesStr = plot_->modelString(attributesModelInd, ok4);
        if (! ok4) return State::SKIP;

        linkConnectionData.nameValues = NameValues(attributesStr);
      }

      //---

      linkConnectionData.srcStr  = namePair.name1();
      linkConnectionData.destStr = namePair.name2();

      plot_->addLinkConnection(linkConnectionData);

      return State::OK;
    }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsConnectionPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

  private:
    const CQChartsConnectionPlot* plot_      { nullptr };
    QString                       separator_ { "/" };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  return true;
}

//---

bool
CQChartsConnectionPlot::
initConnectionObjs() const
{
  CQPerfTrace trace("CQChartsConnectionPlot::initConnectionObjs");

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsConnectionPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ConnectionsData connectionsData;

      // get group value
      GroupData groupData;

      if (plot_->groupColumn().isValid()) {
        connectionsData.groupModelInd =
          ModelIndex(plot_, data.row, plot_->groupColumn(), data.parent);

        if (! plot_->groupColumnData(connectionsData.groupModelInd, groupData))
          return addDataError(connectionsData.groupModelInd, "Invalid group value");
      }
      else {
        groupData = GroupData(QVariant(), data.row, numRows());
      }

      connectionsData.groupData = groupData;

      //---

      // get optional node id (default to row)
      connectionsData.node = data.row;

      if (plot_->nodeColumn().isValid()) {
        connectionsData.nodeModelInd =
          ModelIndex(plot_, data.row, plot_->nodeColumn(), data.parent);

        bool ok2;
        long inode = plot_->modelInteger(connectionsData.nodeModelInd, ok2);
        if (! ok2) return addDataError(connectionsData.nodeModelInd, "Non-integer node value");

        connectionsData.node = static_cast<int>(inode);
      }

      //---

      // get connections
      connectionsData.connectionsModelInd =
        ModelIndex(plot_, data.row, plot_->connectionsColumn(), data.parent);

      if (plot_->connectionsColumnType() == ColumnType::CONNECTION_LIST) {
        bool ok3;
        auto connectionsVar = plot_->modelValue(connectionsData.connectionsModelInd, ok3);

        connectionsData.connections =
          CQChartsConnectionList::fromVariant(connectionsVar).connections();
      }
      else {
        bool ok3;
        auto connectionsStr = plot_->modelString(connectionsData.connectionsModelInd, ok3);
        if (! ok3) return addDataError(connectionsData.connectionsModelInd,
                                       "Invalid connection string");

        CQChartsConnectionList::stringToConnections(connectionsStr, connectionsData.connections);
      }

      //----

      // get name
      connectionsData.name = QString::number(connectionsData.node);

      if (plot_->nameColumn().isValid()) {
        connectionsData.nameModelInd =
          ModelIndex(plot_, data.row, plot_->nameColumn(), data.parent);

        bool ok4;
        connectionsData.name = plot_->modelString(connectionsData.nameModelInd, ok4);
        if (! ok4) return addDataError(connectionsData.nameModelInd, "Invalid name string");
      }

      //---

      // get attributes from optional column
      if (plot_->attributesColumn().isValid()) {
        ModelIndex attributesModelInd(plot_, data.row, plot_->attributesColumn(), data.parent);

        bool ok4;
        auto attributesStr = plot_->modelString(attributesModelInd, ok4);
        if (! ok4) return State::SKIP;

        connectionsData.nameValues = NameValues(attributesStr);
      }

      //---

      // calc total
      connectionsData.total = 0.0;

      for (const auto &connection : connectionsData.connections)
        connectionsData.total += connection.value;

      //---

      // store connections data by node id
      if (connectionsData.nodeModelInd.isValid()) {
        auto nodeInd  = plot_->modelIndex(connectionsData.nodeModelInd);
        auto nodeInd1 = plot_->normalizeIndex(nodeInd);

        connectionsData.ind = nodeInd1;
      }

      idConnectionsData_[connectionsData.node] = connectionsData;

      return State::OK;
    }

    const IdConnectionsData &idConnectionsData() const { return idConnectionsData_; }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsConnectionPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsConnectionPlot* plot_ { nullptr };
    IdConnectionsData             idConnectionsData_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  // add connections
  const auto &idConnectionsData = visitor.idConnectionsData();

  NodeIndex nodeIndex;

  for (const auto &idConnections : idConnectionsData) {
    int         id              = idConnections.first;
    const auto &connectionsData = idConnections.second;

    nodeIndex[id] = connectionsData.ind;
  }

  for (const auto &idConnections : idConnectionsData) {
    int         id              = idConnections.first;
    const auto &connectionsData = idConnections.second;

    addConnectionObj(id, connectionsData, nodeIndex);
  }

  return true;
}

//---

bool
CQChartsConnectionPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsConnectionPlot::initHierObjs");

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsConnectionPlot *plot) :
     plot_(plot) {
      separator_ = plot_->calcSeparator();
    }

    // enter hierarchical row
    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      auto hierData = hierData_; // parent hier

      hierDataList_.push_back(hierData);

      //---

      hierData_.parentLinkInd = ModelIndex(plot_, data.row, plot_->linkColumn(), data.parent);

      bool ok;
      auto linkStr = plot_->modelString(hierData_.parentLinkInd, ok);

      hierData_.linkStrs.push_back(linkStr);

      hierData_.parentStr = hierData_.linkStrs.join(separator_);
      hierData_.total      = 0.0;
      hierData_.childTotal = 0.0;

      //---

      return State::OK;
    }

    // leave hierarchical row
    State hierPostVisit(const QAbstractItemModel *, const VisitData &) override {
      auto hierData = hierData_; // save current hier

      hierData_ = hierDataList_.back(); // set current to parent hier

      hierDataList_.pop_back();

      //---

      // update totals
      hierData.total = hierData.childTotal;

      hierData_.total += hierData.total;

      //---

      if (! hierData_.parentLinkInd.isValid())
        return State::OK;

      if (hierData.total <= 0)
        hierData.total = 1;

      plot_->initHierObjsAddHierConnection(hierData_, hierData);

      //---

      hierData_.childTotal += hierData_.total;

      return State::OK;
    }

    // visit leaf row
    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex linkModelInd (plot_, data.row, plot_->linkColumn (), data.parent);
      ModelIndex valueModelInd(plot_, data.row, plot_->valueColumn(), data.parent);

      bool ok1;

      auto linkStr = plot_->modelString(linkModelInd, ok1);
      if (! ok1) return addDataError(linkModelInd, "Invalid Link");

      auto value = plot_->modelReal(valueModelInd, ok1);
      if (! ok1) return addDataError(valueModelInd, "Invalid Value");

      //---

      HierConnectionData hierData;

      hierData.linkStrs = hierData_.linkStrs;

      hierData.linkStrs.push_back(linkStr);

      hierData.parentLinkInd = linkModelInd;
      hierData.parentStr     = hierData.linkStrs.join(separator_);
      hierData.total         = value;

      plot_->initHierObjsAddLeafConnection(hierData_, hierData);

      //---

      hierData_.childTotal += value;

      return State::OK;
    }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsConnectionPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsConnectionPlot* plot_      { nullptr };
    QString                       separator_ { "/" };
    HierConnectionDataList        hierDataList_;
    HierConnectionData            hierData_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  return true;
}

//---

bool
CQChartsConnectionPlot::
initPathObjs() const
{
  CQPerfTrace trace("CQChartsConnectionPlot::initPathObjs");

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsConnectionPlot *plot) :
     plot_(plot) {
      separator_ = plot_->calcSeparator();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      PathData pathData;

      pathData.pathModelInd = ModelIndex(plot_, data.row, plot_->pathColumn(), data.parent);

      // get hier names from path column
      bool ok;
      auto pathStrs = plot_->modelString(pathData.pathModelInd, ok);
      if (! ok) return State::SKIP;

      pathData.pathStrs = pathStrs.split(separator_, Qt::SkipEmptyParts);

      //---

      if (plot_->valueColumn().isValid()) {
        pathData.valueModelInd = ModelIndex(plot_, data.row, plot_->valueColumn(), data.parent);

        bool ok1;
        double value = plot_->modelReal(pathData.valueModelInd, ok1);
        if (! ok1) return State::SKIP;

        pathData.value = OptReal(value);
      }

      //---

      plot_->addPathValue(pathData);

      return State::OK;
    }

   private:
    const CQChartsConnectionPlot* plot_      { nullptr };
    QString                       separator_ { "/" };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  return true;
}

//---

bool
CQChartsConnectionPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsConnectionPlot::initFromToObjs");

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsConnectionPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      FromToData fromToData;

      //---

      fromToData.fromModelInd = ModelIndex(plot_, data.row, plot_->fromColumn(), data.parent);
      fromToData.toModelInd   = ModelIndex(plot_, data.row, plot_->toColumn  (), data.parent);

      // get from/to node names
      bool ok1, ok2;
      fromToData.fromStr = plot_->modelString(fromToData.fromModelInd, ok1);
      fromToData.toStr   = plot_->modelString(fromToData.toModelInd  , ok2);

      if (! ok1 && ! ok2) return State::SKIP;

      //---

      // get value from optional value column
      if (plot_->valueColumn().isValid()) {
        fromToData.valueModelInd = ModelIndex(plot_, data.row, plot_->valueColumn(), data.parent);

        bool ok3;
        auto value = plot_->modelReal(fromToData.valueModelInd, ok3);
        if (! ok3) return State::SKIP;

        fromToData.value = OptReal(value);
      }

      //---

#ifdef CQCHARTS_GRAPH_PATH_ID
      // get pathId from optional column
      if (plot_->pathIdColumn().isValid()) {
        fromToData.pathIdModelInd = ModelIndex(plot_, data.row, plot_->pathIdColumn(), data.parent);

        bool ok4;
        auto value = plot_->modelInteger(fromToData.pathIdModelInd, ok4);
        if (! ok4) return State::SKIP;

        fromToData.pathId = OptInt(int(value));
      }
#endif

      //---

      // get attributes from optional column
      if (plot_->attributesColumn().isValid()) {
        ModelIndex attributesModelInd(plot_, data.row, plot_->attributesColumn(), data.parent);

        bool ok5;
        auto attributesStr = plot_->modelString(attributesModelInd, ok5);
        if (! ok5) return State::SKIP;

        fromToData.nameValues = NameValues(attributesStr);
      }

      //---

      // Get group value
      if (plot_->groupColumn().isValid()) {
        ModelIndex groupModelInd(plot_, data.row, plot_->groupColumn(), data.parent);

        if (! plot_->groupColumnData(groupModelInd, fromToData.groupData))
          return addDataError(groupModelInd, "Invalid group value");
      }
      else {
        fromToData.groupData = GroupData(QVariant(), data.row, numRows());
      }

      //---

      // Get depth value
      if (plot_->depthColumn().isValid()) {
        fromToData.depthModelInd =
          ModelIndex(plot_, data.row, plot_->depthColumn(), data.parent);

        bool ok3;
        long depth = plot_->modelInteger(fromToData.depthModelInd, ok3);
        if (! ok3) return addDataError(fromToData.depthModelInd, "Non-integer depth value");

        fromToData.depth = static_cast<int>(depth);
      }

      //---

      plot_->addFromToValue(fromToData);

      return State::OK;
    }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsConnectionPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsConnectionPlot* plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  return true;
}

//---

bool
CQChartsConnectionPlot::
processTableModel(TableConnectionDatas &tableConnectionDatas,
                  TableConnectionInfo &tableConnectionInfo) const
{
  CQPerfTrace trace("CQChartsConnectionPlo::processTableModel");

  //---

  using RowData = std::vector<QVariant>;

  struct IndRowData {
    QModelIndex ind;
    RowData     rowData;
  };

  using IndRowDatas = std::vector<IndRowData>;

  auto *th = const_cast<CQChartsConnectionPlot *>(this);

  //---

  // get values for each row from model
  class RowVisitor : public ModelVisitor {
   public:
    using Plot = CQChartsConnectionPlot;

   public:
    RowVisitor(const Plot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      int nc = numCols();

      IndRowData indRowData;

      indRowData.rowData.resize(size_t(nc));

      for (int ic = 0; ic < nc; ++ic) {
        Column c(ic);

        ModelIndex columnInd(plot_, data.row, c, data.parent);

        auto ind = plot_->modelIndex(columnInd);

        // save index for first column
        if (ic == 0)
          indRowData.ind = ind;

        bool ok;

        indRowData.rowData[size_t(ic)] = plot_->modelValue(columnInd, ok);
      }

      indRowDatas_.push_back(std::move(indRowData));

      return State::OK;
    }

    const IndRowDatas &indRowDatas() const { return indRowDatas_; }

   private:
    const Plot* plot_ { nullptr };
    IndRowDatas indRowDatas_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  const auto &indRowDatas = visitor.indRowDatas();

  //---

  // calc table size
  int nr = int(indRowDatas.size());
  int nc = (nr > 0 ? int(indRowDatas[0].rowData.size()) : 0);

  //---

  // get number of non-value (extra) columns
  bool hasLink  = (linkColumn ().isValid() && linkColumn ().column() < nc);
  bool hasGroup = (groupColumn().isValid() && groupColumn().column() < nc);

  int numExtraColumns = 0;

  if (hasLink ) ++numExtraColumns;
  if (hasGroup) ++numExtraColumns;

  //---

  // get number of values (table square so use min of row and column count (minus extra columns))
  int nv = std::min(nr, nc - numExtraColumns);

  //---

  // declare connection data per value
  tableConnectionDatas.resize(size_t(nv));

  //---

  // add group values from group column
  CQChartsValueSet groupValues(this);

  if (hasGroup) {
    int igroup = groupColumn().column();
    assert(igroup >= 0);

    for (int row = 0; row < nv; ++row) {
      auto group = indRowDatas[size_t(row)].rowData[size_t(igroup)];

      groupValues.addValue(group);
    }
  }

  //---

  // load table rows from model
  tableConnectionInfo.numNonZero = 0;

  tableConnectionInfo.total = 0.0;

  for (int row = 0; row < nv; ++row) {
    auto &tableConnectionData = tableConnectionDatas[size_t(row)];

    // from id is row
    tableConnectionData.setFrom(row);

    //---

    // get index for first column for row number
    auto &indRowData = indRowDatas[size_t(row)];

    const auto &ind = indRowData.ind;

    //---

    // set link data
    if (hasLink) {
      ModelIndex linkModelInd(th, ind.row(), linkColumn(), ind.parent());

      int ilink = linkColumn().column();
      assert(ilink >= 0);

      auto linkInd  = modelIndex(linkModelInd);
      auto linkInd1 = normalizeIndex(linkInd);

      auto linkVar = indRowData.rowData[size_t(ilink)];

      QString linkStr;

      CQChartsVariant::toString(linkVar, linkStr);

      tableConnectionData.setName   (linkStr);
      tableConnectionData.setNameInd(linkInd1);
    }

    //---

    // set group data
    if (hasGroup) {
      ModelIndex groupColumnInd(th, ind.row(), groupColumn(), ind.parent());

      int igroup = groupColumn().column();
      assert(igroup >= 0);

      auto groupInd  = modelIndex(groupColumnInd);
      auto groupInd1 = normalizeIndex(groupInd);

      auto groupVar = indRowData.rowData[size_t(igroup)];

      QString groupStr;

      int ig = groupValues.iset(groupVar);
      int ng = int(groupValues.numUnique());

      tableConnectionData.setGroup   (GroupData(groupVar, ig, ng));
      tableConnectionData.setGroupInd(groupInd1);
    }

    //---

    // get values
    int col1 = 0;

    for (int col = 0; col < nv + numExtraColumns; ++col) {
      // skip link or group column
      if (col == linkColumn().column() || col == groupColumn().column())
        continue;

      //---

      // get connection size
      bool ok;
      double value = CQChartsVariant::toReal(indRowData.rowData[size_t(col)], ok);

      //---

      // add non-empty connection
      if (ok && ! CMathUtil::isZero(value))
        tableConnectionData.addValue(col1, OptReal(value), /*primary*/true);

      //---

      ++col1;
    }

    //---

    // add to total
    double total1 = tableConnectionData.total();

    if (! CMathUtil::isZero(total1))
      ++tableConnectionInfo.numNonZero;

    tableConnectionInfo.total += total1;
  }

  //---

  // sort
  if (isSorted()) {
    std::sort(tableConnectionDatas.begin(), tableConnectionDatas.end(),
      [](const TableConnectionData &lhs, const TableConnectionData &rhs) {
        return lhs.total() < rhs.total();
      });

    for (auto &tableConnectionData : tableConnectionDatas)
      tableConnectionData.sort();
  }

  //---

  return true;
}

//---

bool
CQChartsConnectionPlot::
groupColumnData(const ModelIndex &groupModelInd, GroupData &groupData) const
{
  auto *groupDetails = columnDetails(groupColumn());
  if (! groupDetails) return false;

  bool ok1;
  auto groupVar = modelValue(groupModelInd, ok1);
  if (! ok1) return false;

  //---

#if 0
  if (groupDetails->type() == CQBaseModelType::INTEGER) {
    bool ok;
    groupData.ig = int(CQChartsVariant::toInt(groupVar                , ok));
    groupData.ng = int(CQChartsVariant::toInt(groupDetails->minValue(), ok));
  }
  else
#endif
  {
    // get unique id and count (ig is -1 if not set)
    groupData.ig = groupDetails->uniqueId(groupVar);
    groupData.ng = groupDetails->numUnique();
  }

  //---

  // save variant value
  groupData.value = groupVar;

  return true;
}

//------

CQChartsConnectionPlotCustomControls::
CQChartsConnectionPlotCustomControls(CQCharts *charts, const QString &plotType) :
 CQChartsPlotCustomControls(charts, plotType)
{
}

void
CQChartsConnectionPlotCustomControls::
addConnectionColumnWidgets()
{
  // connections group
  auto connectionsFrame = createGroupFrame("Connections", "connectionsFrame");

  //---

  // columns type
  columnsTypeCombo_ = CQUtil::makeWidget<CQEnumCombo>("columnsTypeCombo");

  columnsTypeCombo_->setPropName("columnDataType");

  addFrameWidget(connectionsFrame, "Columns Type", columnsTypeCombo_);

  //---

  // value columns
  static auto columnNames = QStringList() <<
   "group" << "node" << "connections" << "link" << "path" << "from" << "to" <<
   "value" << "depth" << "name";

  addNamedColumnWidgets(columnNames, connectionsFrame);

  //---

  //addFrameRowStretch(connectionsFrame);
}

void
CQChartsConnectionPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::optConnectDisconnect(b,
    columnsTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(columnsTypeSlot()));

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsConnectionPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsConnectionPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsConnectionPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  columnsTypeCombo_->setObj(plot_);

  auto type = plot_->calcColumnDataType();

  if      (type == CQChartsConnectionPlot::ColumnDataType::HIER) {
    showColumnWidgets(QStringList() << "link" << "value");
  }
  else if (type == CQChartsConnectionPlot::ColumnDataType::LINK) {
    showColumnWidgets(QStringList() << "group" << "link" << "value" << "name" << "depth");
  }
  else if (type == CQChartsConnectionPlot::ColumnDataType::CONNECTIONS) {
    showColumnWidgets(QStringList() << "group" << "node" << "connections" << "name");
  }
  else if (type == CQChartsConnectionPlot::ColumnDataType::PATH) {
    showColumnWidgets(QStringList() << "path" << "value");
  }
  else if (type == CQChartsConnectionPlot::ColumnDataType::FROM_TO) {
    showColumnWidgets(QStringList() << "group" << "from" << "to" << "value" << "depth");
  }
  else if (type == CQChartsConnectionPlot::ColumnDataType::TABLE) {
    showColumnWidgets(QStringList() << "group" << "link");
  }

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsConnectionPlotCustomControls::
columnsTypeSlot()
{
  updateWidgets();
}
