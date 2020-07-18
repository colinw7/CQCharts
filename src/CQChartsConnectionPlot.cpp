#include <CQChartsConnectionPlot.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsNamePair.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <CQPropertyViewItem.h>

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
    setNumeric().setTip("Node Id Column");
  addColumnParameter("connections", "Connections", "connectionsColumn").setBasic().
    setTip("List of Connection Pairs (Ids from id column and connection value)").setDiscriminator();

  endParameterGroup();

  //---

  // connections are id pairs and values
  startParameterGroup("Link/Path Value");

  addColumnParameter("link", "Link", "linkColumn").setBasic().
    setTip("Name pair for Source/Target connection").setDiscriminator();
  addColumnParameter("path", "Path", "pathColumn").setBasic().
    setTip("Path for connection hierarchy").setDiscriminator();
  addColumnParameter("from", "From", "fromColumn").setBasic().
    setTip("From connection node").setDiscriminator();
  addColumnParameter("to"  , "To", "toColumn").setBasic().
    setTip("To connection node").setDiscriminator();

  addColumnParameter("value", "Value", "valueColumn").setBasic().
    setNumeric().setTip("Connection value");

  endParameterGroup();

  //---

  startParameterGroup("General");

  addColumnParameter("attributes", "Attributes", "attributesColumn").setBasic().
    setString().setTip("Node/Edge attributes");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setTip("Optional node name");
  addColumnParameter("group", "Group", "groupColumn").
    setNumeric().setTip("Group column");

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

  CQChartsColumn nodeColumn;
  CQChartsColumn connectionsColumn;
  CQChartsColumn linkColumn;
  CQChartsColumn valueColumn;

  int nc = details->numColumns();

  for (int c = 0; c < nc; ++c) {
    auto *columnDetails = details->columnDetails(CQChartsColumn(c));
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

        QString str = CQChartsModelUtil::modelString(charts, model, row, columnDetails->column(),
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
    int nc = details->numColumns();

    if (nr != nc - 1 && nr != nc - 2)
      return;

    int skip = (nr == nc - 2 ? 1 : 0);

    bool allNumeric = true;

    for (int c = skip + 1; c < nc; ++c) {
      auto *columnDetails = details->columnDetails(CQChartsColumn(c));
      if (! columnDetails) continue;

      if (! columnDetails->isNumeric()) {
        allNumeric = false;
        break;
      }
    }

    if (! allNumeric)
      return;

    analyzeModelData.parameterNameColumn["link"] = CQChartsColumn(0);

    if (skip == 1)
      analyzeModelData.parameterNameColumn["group"] = CQChartsColumn(1);
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
}

//---

void
CQChartsConnectionPlot::
setNodeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nodeColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setConnectionsColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(connectionsColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setLinkColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(linkColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setPathColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(pathColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setFromColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(fromColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setToColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(toColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setGroupColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsConnectionPlot::
setAttributesColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(attributesColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsConnectionPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
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

//---

void
CQChartsConnectionPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addBaseProperties();

  // columns
  addProp("columns", "nodeColumn"       , "node"      , "Node column");
  addProp("columns", "connectionsColumn", "connection", "Connections column");

  addProp("columns", "linkColumn" , "link" , "Link column");
  addProp("columns", "pathColumn" , "path" , "Path column");
  addProp("columns", "fromColumn" , "from" , "From column");
  addProp("columns", "toColumn"   , "to"   , "To column");
  addProp("columns", "valueColumn", "value", "Value column");

  addProp("columns", "attributesColumn", "attributes", "Attributes column");

  addProp("columns", "groupColumn", "group", "Grouping column");
  addProp("columns", "nameColumn" , "name" , "Node name column");

  addProp("options", "separator", "", "Model link value separator");
  addProp("options", "symmetric", "", "Model values are symmetric");
  addProp("options", "sorted"   , "", "Sort values by size");
  addProp("options", "maxDepth" , "", "Max Node depth");
  addProp("options", "minValue" , "", "Min Node value");
}

//---

bool
CQChartsConnectionPlot::
checkColumns() const
{
  auto *th = const_cast<CQChartsConnectionPlot *>(this);

  // check columns
  bool columnsValid = true;

  if      (linkColumn().isValid()) {
    // link required
    if (! checkColumn(linkColumn(), "Link", th->linkColumnType_, /*required*/true))
      columnsValid = false;
  }
  else if (connectionsColumn().isValid()) {
    // connection required
    if (! checkColumn(connectionsColumn(), "Connections",
                      th->connectionsColumnType_, /*required*/true))
      columnsValid = false;

    if (! checkColumn(nodeColumn(), "Node")) columnsValid = false;
  }
  else if (pathColumn().isValid()) {
  }
  else if (fromColumn().isValid()) {
  }
  else if (toColumn().isValid()) {
  }
  else {
    return th->addError("Required columns not specified");
  }

  // attributes optional
  if (! checkColumn(attributesColumn(), "Attributes")) columnsValid = false;

  // value optional
  if (! checkColumn(valueColumn(), "Value")) columnsValid = false;

  // group, name optional
  if (! checkColumn(groupColumn(), "Group")) columnsValid = false;
  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;

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
      separator_ = (plot_->separator().length() ? plot_->separator()[0] : '/');
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsConnectionPlot *>(plot_);

      LinkConnectionData linkConnectionData;

      // Get group value
      linkConnectionData.groupData = GroupData(data.row, "");

      if (plot_->groupColumn().isValid()) {
        linkConnectionData.groupModelInd =
          ModelIndex(plot, data.row, plot_->groupColumn(), data.parent);

        if (! plot_->groupColumnData(linkConnectionData.groupModelInd,
                                     linkConnectionData.groupData)) {
          return addDataError(linkConnectionData.groupModelInd, "Invalid group value");
        }
      }

      //---

      // Get link value
      linkConnectionData.linkModelInd =
        ModelIndex(plot, data.row, plot_->linkColumn(), data.parent);

      CQChartsNamePair namePair;

      if (plot_->linkColumnType() == ColumnType::NAME_PAIR) {
        bool ok;
        QVariant linkVar = plot_->modelValue(linkConnectionData.linkModelInd, ok);
        if (! ok) return addDataError(linkConnectionData.linkModelInd, "Invalid Link");

        namePair = linkVar.value<CQChartsNamePair>();
      }
      else {
        bool ok;
        QString linkStr = plot_->modelString(linkConnectionData.linkModelInd, ok);
        if (! ok) return addDataError(linkConnectionData.linkModelInd, "Invalid Link");

        namePair = CQChartsNamePair(linkStr, separator_);
      }

      if (! namePair.isValid())
        return addDataError(linkConnectionData.linkModelInd, "Invalid Link");

      //---

      // Get value value
      linkConnectionData.valueModelInd =
        ModelIndex(plot, data.row, plot_->valueColumn(), data.parent);

      bool ok1;
      linkConnectionData.value = plot_->modelReal(linkConnectionData.valueModelInd, ok1);
      if (! ok1) return addDataError(linkConnectionData.valueModelInd, "Invalid Value");

      //---

      // Get name value
      if (plot_->nameColumn().isValid())
        linkConnectionData.nameModelInd =
          ModelIndex(plot, data.row, plot_->nameColumn(), data.parent);

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
    QChar                         separator_ { '/' };
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
      auto *plot = const_cast<CQChartsConnectionPlot *>(plot_);

      // get group value
      GroupData groupData(data.row, "");

      if (plot_->groupColumn().isValid()) {
        ModelIndex groupModelInd(plot, data.row, plot_->groupColumn(), data.parent);

        if (! plot_->groupColumnData(groupModelInd, groupData))
          return addDataError(groupModelInd, "Invalid group value");
      }

      //---

      // get optional node id (default to row)
      ModelIndex nodeModelInd;

      int id = data.row;

      if (plot_->nodeColumn().isValid()) {
        nodeModelInd = ModelIndex(plot, data.row, plot_->nodeColumn(), data.parent);

        bool ok2;
        id = (int) plot_->modelInteger(nodeModelInd, ok2);
        if (! ok2) return addDataError(nodeModelInd, "Non-integer node value");
      }

      //---

      // get connections
      ConnectionsData connectionsData;

      ModelIndex connectionsModelInd(plot, data.row, plot_->connectionsColumn(), data.parent);

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
        ModelIndex nameModelInd(plot, data.row, plot_->nameColumn(), data.parent);

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

  const auto &idConnectionsData = visitor.idConnectionsData();

  for (const auto &idConnections : idConnectionsData) {
    int         id              = idConnections.first;
    const auto &connectionsData = idConnections.second;

    addConnectionObj(id, connectionsData);
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
      separator_ = (plot_->separator().length() ? plot_->separator()[0] : '/');
    }

    // enter hierarchical row
    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsConnectionPlot *>(plot_);

      HierConnectionData hierData = hierData_; // parent hier

      hierDataList_.push_back(hierData);

      //---

      hierData_.parentLinkInd = ModelIndex(plot, data.row, plot_->linkColumn(), data.parent);

      bool ok;
      QString linkStr = plot_->modelString(hierData_.parentLinkInd, ok);

      hierData_.linkStrs.push_back(linkStr);

      hierData_.parentStr = hierData_.linkStrs.join(separator_);
      hierData_.total      = 0.0;
      hierData_.childTotal = 0.0;

      //---

      return State::OK;
    }

    // leave hierarchical row
    State hierPostVisit(const QAbstractItemModel *, const VisitData &) override {
      HierConnectionData hierData = hierData_; // save current hier

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
      auto *plot = const_cast<CQChartsConnectionPlot *>(plot_);

      ModelIndex linkModelInd (plot, data.row, plot_->linkColumn (), data.parent);
      ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

      bool ok1, ok2;

      QString linkStr = plot_->modelString(linkModelInd , ok1);
      double  value   = plot_->modelReal  (valueModelInd, ok2);

      if (! ok1) return addDataError(linkModelInd , "Invalid Link");
      if (! ok2) return addDataError(valueModelInd, "Invalid Value");

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
    QChar                         separator_ { '/' };
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
      separator_ = (plot_->separator().length() ? plot_->separator()[0] : '/');
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsConnectionPlot *>(plot_);

      ModelIndex pathModelInd(plot, data.row, plot_->pathColumn(), data.parent);

      // get hier names from path column
      bool ok;
      QString pathStrs = plot_->modelString(pathModelInd, ok);
      if (! ok) return State::SKIP;

      QStringList pathStringList = pathStrs.split(separator_, QString::SkipEmptyParts);

      //---

      double value = 1.0;

      if (plot_->valueColumn().isValid()) {
        ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

        bool ok1;
        value = plot_->modelReal(valueModelInd, ok1);
        if (! ok1) return State::SKIP;
      }

      //---

      plot_->addPathValue(pathStringList, value);

      return State::OK;
    }

   private:
    const CQChartsConnectionPlot* plot_      { nullptr };
    QChar                         separator_ { '/' };
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
      auto *plot = const_cast<CQChartsConnectionPlot *>(plot_);

      ModelIndex fromModelInd(plot, data.row, plot_->fromColumn(), data.parent);
      ModelIndex toModelInd  (plot, data.row, plot_->toColumn  (), data.parent);

      // get from/to node names
      bool ok1;
      QString fromName = plot_->modelString(fromModelInd, ok1);
      if (! ok1) return State::SKIP;

      bool ok2;
      QString toName = plot_->modelString(toModelInd, ok2);
      if (! ok2) return State::SKIP;

      //---

      // get value from optional value column
      double value = 1.0;

      if (plot_->valueColumn().isValid()) {
        ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

        bool ok3;
        value = plot_->modelReal(valueModelInd, ok3);
        if (! ok3) return State::SKIP;
      }

      //---

      // get attributes from optional column
      CQChartsNameValues nameValues;

      if (plot_->attributesColumn().isValid()) {
        ModelIndex attributesModelInd(plot, data.row, plot_->attributesColumn(), data.parent);

        bool ok4;
        QString attributesStr = plot_->modelString(attributesModelInd, ok4);
        if (! ok4) return State::SKIP;

        nameValues = CQChartsNameValues(attributesStr);
      }

      //---

      // Get group value
      GroupData groupData(data.row, "");

      if (plot_->groupColumn().isValid()) {
        ModelIndex groupModelInd(plot, data.row, plot_->groupColumn(), data.parent);

        if (! plot_->groupColumnData(groupModelInd, groupData))
          return addDataError(groupModelInd, "Invalid group value");
      }

      //---

      plot_->addFromToValue(fromName, toName, value, nameValues, groupData);

      return State::OK;
    }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsConnectionPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsConnectionPlot* plot_      { nullptr };
    QChar                         separator_ { '/' };
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
    RowVisitor(const CQChartsConnectionPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsConnectionPlot *>(plot_);

      int nc = numCols();

      IndRowData indRowData;

      indRowData.rowData.resize(nc);

      for (int ic = 0; ic < numCols(); ++ic) {
        CQChartsColumn c(ic);

        ModelIndex columnInd(plot, data.row, c, data.parent);

        QModelIndex ind = plot_->modelIndex(columnInd);

        // save index for first column
        if (ic == 0)
          indRowData.ind = ind;

        bool ok;

        indRowData.rowData[ic] = plot_->modelValue(columnInd, ok);
      }

      indRowDatas_.push_back(indRowData);

      return State::OK;
    }

    const IndRowDatas &indRowDatas() const { return indRowDatas_; }

   private:
    const CQChartsConnectionPlot* plot_ { nullptr };
    IndRowDatas                   indRowDatas_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  const auto &indRowDatas = visitor.indRowDatas();

  int nr = indRowDatas.size();
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
  tableConnectionDatas.resize(nv);

  //---

  // add group values from group column
  CQChartsValueSet groupValues(this);

  if (hasGroup) {
    int igroup = groupColumn().column();
    assert(igroup >= 0);

    for (int row = 0; row < nv; ++row) {
      QVariant group = indRowDatas[row].rowData[igroup];

      groupValues.addValue(group);
    }
  }

  //---

  tableConnectionInfo.numNonZero = 0;

  tableConnectionInfo.total = 0.0;

  for (int row = 0; row < nv; ++row) {
    auto &tableConnectionData = tableConnectionDatas[row];

    // from id is row
    tableConnectionData.setFrom(row);

    //---

    // get index for first column for row number
    auto &indRowData = indRowDatas[row];

    const auto &ind = indRowData.ind;

    //---

    // set link data
    if (hasLink) {
      ModelIndex linkModelInd(th, ind.row(), linkColumn(), ind.parent());

      int ilink = linkColumn().column();
      assert(ilink >= 0);

      QModelIndex linkInd  = modelIndex(linkModelInd);
      QModelIndex linkInd1 = normalizeIndex(linkInd);

      QVariant linkVar = indRowData.rowData[ilink];

      QString linkStr;

      CQChartsVariant::toString(linkVar, linkStr);

      tableConnectionData.setName   (linkStr);
      tableConnectionData.setLinkInd(linkInd1);
    }

    //---

    // set group data
    if (hasGroup) {
      ModelIndex groupColumnInd(th, ind.row(), groupColumn(), ind.parent());

      int igroup = groupColumn().column();
      assert(igroup >= 0);

      QModelIndex groupInd  = modelIndex(groupColumnInd);
      QModelIndex groupInd1 = normalizeIndex(groupInd);

      QVariant groupVar = indRowData.rowData[igroup];

      QString groupStr;

      CQChartsVariant::toString(groupVar, groupStr);

      int ig = groupValues.iset(groupVar);
      int ng = groupValues.numUnique();

      tableConnectionData.setGroup   (GroupData(groupStr, ig, ng));
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
      double value = CQChartsVariant::toReal(indRowData.rowData[col], ok);

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

  bool ok1;
  QVariant groupVar = modelValue(groupModelInd, ok1);
  if (! ok1) return false;

  //---

  groupData.ig = groupDetails->uniqueId(groupVar);
  groupData.ng = groupDetails->numUnique();

  //---

  bool ok2;
  groupData.id = (int) modelInteger(groupModelInd, ok2);
  if (! ok2) groupData.id = groupData.ig;

  //---

  groupData.name = groupVar.toString();

  if (! groupData.name.length())
    groupData.name = QString("%1").arg(groupData.id);

  return true;
}
