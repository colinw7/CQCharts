#include <CQChartsConnectionPlot.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsConnectionList.h>
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
  startParameterGroup("Link/Value");

  addColumnParameter("link", "Link", "linkColumn").setBasic().
    setTip("Name pair for Source/Target connection").setDiscriminator();
  addColumnParameter("value", "Value", "valueColumn").setBasic().
    setNumeric().setTip("Connection value");

  endParameterGroup();

  //---

  addColumnParameter("path", "Path", "pathColumn").setBasic().
    setTip("Path column").setDiscriminator();

  //---

  startParameterGroup("General");

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
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
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
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
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

      if (! linkColumn.isValid()) {
        bool ok;

        QString str = CQChartsModelUtil::modelString(charts, model, ind, ok);
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
CQChartsConnectionPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
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
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsConnectionPlot::
setPathColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(pathColumn_, c, [&]() { updateRangeAndObjs(); } );
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
  addProp("columns", "valueColumn", "value", "Value column");

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

  th->clearErrors();

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
  else {
    return th->addError("Required columns not specified");
  }

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
      HierConnectionData hierData = hierData_; // parent hier

      hierDataList_.push_back(hierData);

      //---

      hierData_.parentLinkInd = CQChartsModelIndex(data.row, plot_->linkColumn(), data.parent);

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
      CQChartsModelIndex linkModelInd (data.row, plot_->linkColumn (), data.parent);
      CQChartsModelIndex valueModelInd(data.row, plot_->valueColumn(), data.parent);

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
    State addDataError(const CQChartsModelIndex &ind, const QString &msg) const {
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
      CQChartsModelIndex pathModelInd(data.row, plot_->pathColumn(), data.parent);

      // get hier names from path column
      bool ok;
      QString pathStrs = plot_->modelString(pathModelInd, ok);
      if (! ok) return State::SKIP;

      QStringList pathStringList = pathStrs.split(separator_, QString::SkipEmptyParts);

      //---

      double value = 1.0;

      if (plot_->valueColumn().isValid()) {
        CQChartsModelIndex valueModelInd(data.row, plot_->valueColumn(), data.parent);

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

  //---

  // get values for each row from model
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsConnectionPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      int nc = numCols();

      IndRowData indRowData;

      indRowData.rowData.resize(nc);

      for (int ic = 0; ic < numCols(); ++ic) {
        CQChartsColumn c(ic);

        CQChartsModelIndex columnInd(data.row, c, data.parent);

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

    const QModelIndex &ind = indRowData.ind;

    //---

    // set link data
    if (hasLink) {
      CQChartsModelIndex linkModelInd(ind.row(), linkColumn(), ind.parent());

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
      CQChartsModelIndex groupColumnInd(ind.row(), groupColumn(), ind.parent());

      int igroup = groupColumn().column();
      assert(igroup >= 0);

      QModelIndex groupInd  = modelIndex(groupColumnInd);
      QModelIndex groupInd1 = normalizeIndex(groupInd);

      QVariant groupVar = indRowData.rowData[igroup];

      QString groupStr;

      CQChartsVariant::toString(groupVar, groupStr);

      int ig = groupValues.iset(groupVar);
      int ng = groupValues.numUnique();

      tableConnectionData.setGroup   (TableConnectionData::Group(groupStr, ig, ng));
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
        tableConnectionData.addValue(col1, value, value);

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
