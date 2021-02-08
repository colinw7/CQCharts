#include <CQChartsDataFrame.h>

#ifdef CQCHARTS_DATA_FRAME

#include <CQDataFrame.h>
#include <CQDataFrameWidget.h>
#include <CQChartsModelViewHolder.h>
#include <CQChartsPreviewPlot.h>
#include <CQChartsModelDetailsTable.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsPlotType.h>
#include <CQChartsAnalyzeModel.h>
#include <CQChartsFilterModel.h>
#include <CQChartsVariant.h>
#include <CQChartsMargin.h>
#include <CQCharts.h>

#include <CQChartsCmds.h>

#include <QVBoxLayout>

namespace CQDataFrame {

//---

CQDATA_FRAME_TCL_CMD(Model)
CQDATA_FRAME_TCL_CMD(ModelDetails)
CQDATA_FRAME_TCL_CMD(Plot)

CQDATA_FRAME_INST_TCL_CMD(Model)
CQDATA_FRAME_INST_TCL_CMD(ModelDetails)
CQDATA_FRAME_INST_TCL_CMD(Plot)

//---

QStringList completeFile(const QString &file) {
  return Frame::s_completeFile(file);
}

//---

class ModelWidget : public Widget {
 public:
  ModelWidget(Area *area, CQCharts *charts) :
   Widget(area), charts_(charts) {
    setObjectName("model");
  }

  QString id() const override { return QString("model.%1").arg(pos()); }

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) { charts_ = charts; }

  CQChartsDataFrameCmd *cmd() const { return cmd_; }
  void setCmd(CQChartsDataFrameCmd *cmd) { cmd_ = cmd; }

  CQChartsModelData *modelData() const { return modelData_; }

  CQChartsModelViewHolder *modelView() const { return modelView_; }

  void setModelData(CQChartsModelData *modelData) {
    modelData_ = modelData;

    modelView_->setModel(modelData_->model(), /*hierarchical*/false);
  }

  void addWidgets() override {
    auto *layout = new QVBoxLayout(contents_);
    layout->setMargin(0); layout->setSpacing(0);

    modelView_ = new CQChartsModelViewHolder(charts_);

    layout->addWidget(modelView_);
  }

  void setExpanded(bool b) override {
    modelView_->setVisible(b);

    Widget::setExpanded(b);
  }

  QSize contentsSizeHint() const override { return QSize(-1, 400); }
  QSize contentsSize() const override { return QSize(400, 400); }

 private:
  void draw(QPainter* /*painter*/, int /*dx*/, int /*dy*/) override {
  }

 private:
  CQCharts*                charts_    { nullptr };
  CQChartsDataFrameCmd*    cmd_       { nullptr };
  CQChartsModelData*       modelData_ { nullptr };
  CQChartsModelViewHolder* modelView_ { nullptr };
};

class ModelFactory : public WidgetFactory {
 public:
  ModelFactory(CQChartsDataFrameCmd *cmd) :
   cmd_(cmd) {
  }

  const char *name() const override { return "model"; }

  CQChartsDataFrameCmd *cmd() const { return cmd_; }

  void addTclCommand(Frame *frame) override {
    auto *cmd = new ModelTclCmd(frame);

    frame->tclCmdMgr()->addCommand("model", cmd);

    cmd->setData(this);
  }

  Widget *addWidget(Area *area) override {
    return makeWidget<ModelWidget>(area, cmd_->charts());
  }

 private:
  CQChartsDataFrameCmd *cmd_ { nullptr };
};

//---

ModelWidget *makeModelWidget(CQCharts *charts, CQChartsModelData *modelData,
                             CQChartsDataFrameCmd *cmd, Frame *frame) {
  modelData->addSummaryModel();

  auto *area = frame->larea();

  auto *widget = makeWidget<ModelWidget>(area, charts);

  widget->setModelData(modelData);
  widget->setCmd(cmd);

  //---

  auto *instCmd = new ModelInstTclCmd(frame, widget->id());

  instCmd->setData(widget);

  frame->tclCmdMgr()->addCommand(widget->id(), instCmd);

  return widget;
}

//---

void
ModelTclCmd::
addArgs(CQTclCmd::CmdArgs &argv)
{
  addArg(argv, "-file", ArgType::String, "file name");

  addArg(argv, "-comment_header"   , ArgType::Boolean, "first comment line is header");
  addArg(argv, "-first_line_header", ArgType::Boolean, "first line is header");
}

QStringList
ModelTclCmd::
getArgValues(const QString &option, const NameValueMap &nameValueMap)
{
  QStringList strs;

  if (option == "file") {
    auto p = nameValueMap.find("file");

    QString file = (p != nameValueMap.end() ? (*p).second : "");

    return Frame::s_completeFile(file);
  }

  return strs;
}

bool
ModelTclCmd::
exec(CQTclCmd::CmdArgs &argv)
{
  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto *factory = static_cast<ModelFactory *>(data());
  auto *cmd     = factory->cmd();

  auto fileName = argv.getParseStr("file");

  CQChartsInputData inputData;

  if (argv.hasParseArg("comment_header"))
    inputData.commentHeader = true;

  if (argv.hasParseArg("first_line_header"))
    inputData.firstLineHeader = true;

  CQChartsFileType fileType { CQChartsFileType::CSV };

  if (! cmd->cmds()->loadFileModel(fileName, fileType, inputData))
    return false;

  auto *charts = cmd->charts();

  auto *modelData = charts->currentModelData();
  if (! modelData) return false;

  auto *widget = makeModelWidget(charts, modelData, cmd, frame_);

  return frame_->setCmdRc(widget->id());
}

//---

void
ModelInstTclCmd::
addArgs(CQTclCmd::CmdArgs &argv)
{
  // functions
  addArg(argv, "-get"    , ArgType::String, "get named value");
  addArg(argv, "-set"    , ArgType::String, "set named value");
  addArg(argv, "-analyze", ArgType::String, "analyze model for specified types");
  addArg(argv, "-filter" , ArgType::String, "filter model using specified expression");
  addArg(argv, "-flatten", ArgType::String, "flatten model using specified column");
  addArg(argv, "-sort"   , ArgType::String, "sort model by column in specified direction");
  addArg(argv, "-join"   , ArgType::String, "join models by columns");

  // data
  addArg(argv, "-column" , ArgType::String, "get value for specified column");
  addArg(argv, "-value"  , ArgType::String, "value for set");
  addArg(argv, "-data"   , ArgType::String, "extra data value for get/set");
}

QStringList
ModelInstTclCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
ModelInstTclCmd::
exec(CQTclCmd::CmdArgs &argv)
{
  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto *widget = static_cast<ModelWidget *>(data());
  if (! widget) return false;

  auto *charts = widget->charts();
  auto *cmd    = widget->cmd();

  auto errorMsg = [&](const QString &msg) {
    charts->errorMsg(msg);
    return false;
  };

  //---

  auto *modelData = widget->modelData();
  if (! modelData) return false;

  auto *pmodel = modelData->model().data();

  auto *details = modelData->details();

  //---

  if      (argv.hasParseArg("get")) {
    auto name = argv.getParseStr("get");

    if (argv.hasParseArg("column")) {
      auto columnName = argv.getParseStr("column");

      CQChartsColumn column;

      if (! CQChartsModelUtil::stringToColumn(pmodel, columnName, column))
        return errorMsg("Invalid column");

      auto *columnDetails = details->columnDetails(column);

      if      (name == "header_name") {
        bool ok;

        auto var = CQChartsModelUtil::modelHeaderValue(pmodel, column, Qt::DisplayRole, ok);

        return frame_->setCmdRc(var);
      }
      else if (name == "num_unique") {
        return frame_->setCmdRc(columnDetails ? columnDetails->numUnique() : 0);
      }
      else if (name == "num_null") {
        return frame_->setCmdRc(columnDetails ? columnDetails->numNull() : 0);
      }
      else if (name == "correlation") {
        if (! argv.hasParseArg("data"))
          return errorMsg("No data specified");

        auto data = argv.getParseStr("data");

        CQChartsColumn column1;

        if (! CQChartsModelUtil::stringToColumn(pmodel, data, column1))
          column1 = CQChartsColumn();

        if (! column1.isValid())
          return errorMsg("Invalid data column specified");

        double c = details->correlation(column, column1);

        return frame_->setCmdRc(c);
      }
      else if (name == "?") {
        static auto names = QStringList() << "column_name" << "num_unique" << "num_null";
        return frame_->setCmdRc(names);
      }
      else
        return errorMsg("Invalid -get name for column");
    }
    else {
      auto modelDetailValues = [&](std::function<QVariant(CQChartsModelColumnDetails *)> f,
                                     const QVariant &defValue=QVariant()) {
        QVariantList vars;

        int nc = modelData->model()->columnCount();

        for (int ic = 0; ic < nc; ++ic) {
          CQChartsColumn c(ic);

          auto *columnDetails = details->columnDetails(c);

          auto var = (columnDetails ? f(columnDetails) : defValue);

          vars.push_back(var);
        }

        return vars;
      };

      //---

      if      (name == "id")
        frame_->setCmdRc(modelData->id());
      else if (name == "num_columns")
        return frame_->setCmdRc(modelData->model()->columnCount());
      else if (name == "num_rows")
        return frame_->setCmdRc(modelData->model()->rowCount());
      else if (name == "column_names") {
        QVariantList vars;

        int nc = modelData->model()->columnCount();

        for (int ic = 0; ic < nc; ++ic) {
          CQChartsColumn c(ic);

          bool ok;

          auto var = CQChartsModelUtil::modelHeaderValue(pmodel, c, Qt::DisplayRole, ok);

          vars.push_back(var);
        }

        return frame_->setCmdRc(vars);
      }
      else if (name == "num_unique") {
        auto vars = modelDetailValues(
          [](CQChartsModelColumnDetails *columnDetails) {
            return columnDetails->numUnique(); },
          QVariant(0)
        );

        return frame_->setCmdRc(vars);
      }
      else if (name == "num_null") {
        auto vars = modelDetailValues(
          [](CQChartsModelColumnDetails *columnDetails) {
            return columnDetails->numNull(); },
          QVariant(0)
        );

        return frame_->setCmdRc(vars);
      }
      else if (name == "?") {
        static auto names = QStringList() <<
          "id" << "num_columns" << "num_rows" << "column_names" << "num_unique" << "num_null";
        return frame_->setCmdRc(names);
      }
      else
        return errorMsg("Invalid -get name for columns");
    }
  }
  else if (argv.hasParseArg("set")) {
    auto name  = argv.getParseStr("set");
    auto value = argv.getParseStr("value");
    auto data  = argv.getParseStr("data");

    if (argv.hasParseArg("column")) {
      auto columnName = argv.getParseStr("column");

      CQChartsColumn column;

      if (! CQChartsModelUtil::stringToColumn(pmodel, columnName, column))
        return errorMsg("Invalid column");

      if      (name == "header_name") {
        CQChartsModelUtil::setModelHeaderValue(pmodel, column.column(), Qt::Horizontal, value);
      }
      else if (name == "column_type") {
        auto typeName = argv.getParseStr("value");

        if (! CQChartsModelUtil::setColumnTypeStr(charts, pmodel, column, typeName))
          return errorMsg(QString("Invalid column type '%1'").arg(typeName));
      }
      else if (name == "replace_data") {
        int n = modelData->replaceValue(column, value, data);

        return frame_->setCmdRc(n);
      }
      else if (name == "replace_null") {
        int n = modelData->replaceNullValues(column, value);

        return frame_->setCmdRc(n);
      }
      else if (name == "column_visible") {
        bool ok;

        bool b = CQChartsUtil::stringToBool(value, &ok);

        if (b)
          widget->modelView()->showColumn(column.column());
        else
          widget->modelView()->hideColumn(column.column());
      }
      else if (name == "?") {
        static auto names = QStringList() << "name" << "replace_null";
        return frame_->setCmdRc(names);
      }
      else
        return errorMsg("invalid -set name for column");
    }
    else {
      return errorMsg("invalid -set name for columns");
    }
  }
  else if (argv.hasParseArg("analyze")) {
    CQChartsAnalyzeModel analyzeModel(widget->charts(), modelData);

    analyzeModel.analyze();

    std::cout << "<html>\n";
    std::cout << "<table>\n";

    std::cout << "<tr>";
    std::cout << "<th>Type</th>";
    std::cout << "<th>Columns</th>";
    std::cout << "</tr>\n";

    const auto &typeAnalyzeModelData = analyzeModel.typeAnalyzeModelData();

    for (const auto &tnc : typeAnalyzeModelData) {
      QVariantList cvars;

      const auto &typeName = tnc.first;

      std::cout << "<tr>";
      std::cout << "<td>" << typeName.toStdString() << "</td>";

      QVariantList tncvars;

      std::cout << "<td>";

      const auto &analyzeModelData = tnc.second;

      for (const auto &nc : analyzeModelData.parameterNameColumn) {
        const auto &paramName = nc.first;
        const auto &column    = nc.second;

        QVariantList ncvars;

        std::cout << " " << paramName.toStdString();
        std::cout << "=" << column.toString().toStdString();
      }

      std::cout << "</td>";
      std::cout << "</tr>\n";
    }

    std::cout << "</table>\n";
    std::cout << "</html>\n";

    return true;
  }
  else if (argv.hasParseArg("filter")) {
    auto filterStr = argv.getParseStr("filter");

    int nr = -1;

    if (filterStr.startsWith("#nr=")) {
      bool ok;
      nr = filterStr.mid(4).toInt(&ok);
      if (! ok) return errorMsg(QString("Invalid number of rows '%1'").arg(filterStr));
      filterStr = "";
    }

    CQChartsModelData::CopyData copyData;

    copyData.filter = filterStr;
    copyData.nr     = nr;

    auto newModel = modelData->copy(copyData);

    CQChartsCmds::ModelP newModelP(newModel);

    auto *newModelData = charts->initModelData(newModelP);

    auto *widget = makeModelWidget(charts, newModelData, cmd, frame_);

    return frame_->setCmdRc(widget->id());
  }
  else if (argv.hasParseArg("flatten")) {
    auto value = argv.getParseStr("value");

    // get column
    auto flattenColumnName = argv.getParseStr("flatten");

    CQChartsColumn flattenColumn;

    if (! CQChartsModelUtil::stringToColumn(pmodel, flattenColumnName, flattenColumn))
      return errorMsg("Invalid flatten column");

    //---

    CQChartsModelUtil::FlattenData flattenData;

    flattenData.groupColumn = flattenColumn;

    //---

    // split value into strings of operation and columns
    QStringList valueStrs;

    if (! CQTcl::splitList(value, valueStrs))
      return errorMsg("Invalid value");

    for (const auto &valueStr : valueStrs) {
      QStringList valueStrs1;

      if (! CQTcl::splitList(valueStr, valueStrs1) || valueStrs1.length() < 2)
        return errorMsg("Invalid value string");

      auto flattenOp = CQChartsModelUtil::flattenStringToOp(valueStrs1[0]);

      if (flattenOp == CQChartsModelUtil::FlattenOp::NONE)
        return errorMsg("Invalid column op");

      CQChartsColumn valueColumn;

      if (! CQChartsModelUtil::stringToColumn(pmodel, valueStrs1[1], valueColumn))
        return errorMsg("Invalid value column");

      flattenData.columnOps.push_back(
        CQChartsModelUtil::FlattenData::ColumnOp(valueColumn, flattenOp));
    }

    auto *filterModel = CQChartsModelUtil::flattenModel(charts, pmodel, flattenData);

    CQChartsCmds::ModelP filterModelP(filterModel);

    auto *filterModelData = charts->initModelData(filterModelP);

    auto *widget = makeModelWidget(charts, filterModelData, cmd, frame_);

    return frame_->setCmdRc(widget->id());
  }
  else if (argv.hasParseArg("sort")) {
    bool decreasing = (argv.getParseStr("sort") == "decreasing");

    Qt::SortOrder order = (decreasing ? Qt::DescendingOrder : Qt::AscendingOrder);

    //---

    // get column
    auto columnName = argv.getParseStr("column");

    CQChartsColumn column;

    if (! CQChartsModelUtil::stringToColumn(pmodel, columnName, column))
      return errorMsg("Invalid column");

    //---

    CQChartsCmds::sortModel(modelData->model(), column.column(), order);
  }
  else if (argv.hasParseArg("join")) {
    auto joinModelStr = argv.getParseStr("join");

    auto *joinModelData = cmd->cmds()->getModelDataOrCurrent(joinModelStr);

    if (! joinModelData)
      return errorMsg(QString("Invalid join model '%1'").arg(joinModelStr));

    //---

    auto columnsStr = argv.getParseStr("column");

    QStringList columnsStrs;

    if (! CQTcl::splitList(columnsStr, columnsStrs))
      return errorMsg(QString("Invalid columns string '%1'").arg(columnsStr));

    using Columns = std::vector<CQChartsColumn>;

    Columns columns;

    for (int i = 0; i < columnsStrs.length(); ++i) {
      CQChartsColumn column;

      if (! CQChartsModelUtil::stringToColumn(pmodel, columnsStrs[i], column))
        return errorMsg(QString("Invalid column '%1'").arg(columnsStrs[i]));

      columns.push_back(column);
    }

    auto *newModel = modelData->join(joinModelData, columns);

    CQChartsCmds::ModelP newModelP(newModel);

    auto *newModelData = charts->initModelData(newModelP);

    auto *newModelWidget = makeModelWidget(charts, newModelData, cmd, frame_);

    return frame_->setCmdRc(newModelWidget->id());
  }
  else
    return errorMsg("invalid argument");

  return true;
}

//---

class PlotWidget : public Widget {
 public:
  PlotWidget(Area *area, CQCharts *charts) :
   Widget(area), charts_(charts) {
    setObjectName("plot");
  }

  QString id() const override { return QString("plot.%1").arg(pos()); }

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) { charts_ = charts; }

  void setModelData(CQChartsModelData *modelData) {
    modelData_ = modelData;
  }

  bool isPreview() const { return preview_; }
  void setPreview(bool b) { preview_ = b; }

  CQChartsPreviewPlot *previewPlot() { return previewPlot_; }

  CQChartsPlot *plot() { return (previewPlot_ ? previewPlot()->plot() : plot_); }

  void setType(const QString &typeName) {
    typeName_ = typeName;

    if (! charts_->isPlotType(typeName_))
      return;

    auto *plotType = charts_->plotType(typeName_);

    // get model to use (current or summary)
    if (preview_) {
      auto *summaryModel = modelData_->summaryModel();

      CQChartsCmds::ModelP previewModel;

      if (modelData_->isSummaryEnabled() && summaryModel)
        previewModel = modelData_->summaryModelP();
      else
        previewModel = modelData_->model();

      previewPlot_->updatePlot(previewModel, plotType);
    }
    else {
      view_->removeAllPlots();

      plot_ = plotType->createAndInit(view_, modelData_->model());

      view_->addPlot(plot_);
    }
  }

  void addWidgets() override {
    auto *layout = new QVBoxLayout(contents_);
    layout->setMargin(0); layout->setSpacing(0);

    if (preview_) {
      previewPlot_ = new CQChartsPreviewPlot(charts_);

      layout->addWidget(previewPlot_);
    }
    else {
      view_ = charts_->createView();

      layout->addWidget(view_);
    }
  }

  QSize contentsSizeHint() const override { return QSize(-1, 400); }
  QSize contentsSize() const override { return QSize(400, 400); }

 private:
  void draw(QPainter* /*painter*/, int /*dx*/, int /*dy*/) override {
  }

 private:
  CQCharts*            charts_      { nullptr };
  CQChartsModelData*   modelData_   { nullptr };
  QString              typeName_;
  bool                 preview_     { false };
  CQChartsPreviewPlot* previewPlot_ { nullptr };
  CQChartsView*        view_        { nullptr };
  CQChartsPlot*        plot_        { nullptr };
};

class PlotFactory : public WidgetFactory {
 public:
  PlotFactory(CQChartsDataFrameCmd *cmd) :
   cmd_(cmd) {
  }

  const char *name() const override { return "plot"; }

  CQChartsDataFrameCmd *cmd() const { return cmd_; }

  void addTclCommand(Frame *frame) override {
    auto *cmd = new PlotTclCmd(frame);

    frame->tclCmdMgr()->addCommand("plot", cmd);

    cmd->setData(this);
  }

  Widget *addWidget(Area *area) override {
    return makeWidget<PlotWidget>(area, cmd_->charts());
  }

 private:
  CQChartsDataFrameCmd *cmd_ { nullptr };
};

void
PlotTclCmd::
addArgs(CQTclCmd::CmdArgs &argv)
{
  addArg(argv, "-model"  , ArgType::String , "model name");
  addArg(argv, "-type"   , ArgType::String , "plot type");
  addArg(argv, "-columns", ArgType::String , "plot columns");
  addArg(argv, "-preview", ArgType::Boolean, "plot is preview");
}

QStringList
PlotTclCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
PlotTclCmd::
exec(CQTclCmd::CmdArgs &argv)
{
  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto *factory = static_cast<PlotFactory *>(data());
  auto *cmd     = factory->cmd();

  auto *charts = cmd->charts();

  auto errorMsg = [&](const QString &msg) {
    charts->errorMsg(msg);
    return false;
  };

  //---

  // get model
  auto modelName = argv.getParseStr("model");

  CQChartsModelData *modelData = nullptr;

  if (modelName != "") {
    modelData = cmd->cmds()->getModelDataOrCurrent(modelName);

    if (! modelData) {
      auto *modelWidget = dynamic_cast<ModelWidget *>(frame_->getWidget(modelName));

      if (! modelWidget)
        return errorMsg(QString("No model '%1'").arg(modelName));

      modelData = modelWidget->modelData();
    }
  }
  else {
    modelData = charts->currentModelData();

    if (! modelData)
      return errorMsg("No current model");
  }

  auto *pmodel = modelData->model().data();

  //---

  auto typeName = argv.getParseStr("type");

  typeName = CQChartsCmds::fixTypeName(typeName);

  if (! charts->isPlotType(typeName))
    return errorMsg("Invalid type '" + typeName + "'");

  auto *plotType = charts->plotType(typeName);

  //---

  auto *area = frame_->larea();

  auto *widget = makeWidget<PlotWidget>(area, charts);

  widget->setModelData(modelData);

  if (argv.hasParseArg("preview"))
    widget->setPreview(true);

  widget->setType(typeName);

  //---

  auto *plot = widget->plot();

  CQChartsAnalyzeModel analyzeModel(charts, modelData);

  analyzeModel.initPlot(plot);

  //---

  if (argv.hasParseArg("columns")) {
    auto columnsStr = argv.getParseStr("columns");

    QStringList columnsStrs;

    if (! CQTcl::splitList(columnsStr, columnsStrs))
      return errorMsg(QString("Invalid columns string '%1'").arg(columnsStr));

    for (int i = 0; i < columnsStrs.length(); ++i) {
      QStringList columnValueStrs;

      if (! CQTcl::splitList(columnsStrs[i], columnValueStrs))
        return errorMsg(QString("Invalid column value '%1'").arg(columnsStrs[i]));

      if (columnValueStrs.size() != 2)
        return errorMsg(QString("Invalid column value '%1'").arg(columnsStrs[i]));

      auto *parameter = plotType->getColumnParameter(columnValueStrs[0]);

      if (! parameter)
        return errorMsg(QString("Invalid parameter '%1'").arg(columnValueStrs[0]));

      if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN) {
        CQChartsColumn column;

        if (! CQChartsModelUtil::stringToColumn(pmodel, columnValueStrs[1], column))
          return errorMsg(QString("Invalid column '%1'").arg(columnValueStrs[1]));

        if (! plot->setProperty(parameter->propPath(), column.toString()))
          return errorMsg(QString("Failed to set column property '%1'").
                           arg(parameter->propPath()));
      }
      else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST) {
        CQChartsColumns columns;

        QStringList columnListStrs;

        if (! CQTcl::splitList(columnValueStrs[1], columnListStrs))
          return errorMsg(QString("Invalid column list string '%1'").arg(columnValueStrs[1]));

        for (int j = 0; j < columnListStrs.length(); ++j) {
          CQChartsColumn column1;

          if (! CQChartsModelUtil::stringToColumn(pmodel, columnListStrs[j], column1))
            return errorMsg(QString("Invalid column '%1'").arg(columnValueStrs[1]));

          columns.addColumn(column1);
        }

        if (! plot->setProperty(parameter->propPath(), columns.toString()))
          return errorMsg(QString("Failed to set columns property '%1'").
                           arg(parameter->propPath()));
      }
    }
  }

  //---

  auto *instCmd = new PlotInstTclCmd(frame_, widget->id());

  instCmd->setData(widget);

  frame_->tclCmdMgr()->addCommand(widget->id(), instCmd);

  return frame_->setCmdRc(widget->id());
}

//---

void
PlotInstTclCmd::
addArgs(CQTclCmd::CmdArgs &argv)
{
  addArg(argv, "-get"         , ArgType::String, "get named value");
  addArg(argv, "-set"         , ArgType::String, "set named value");
  addArg(argv, "-get_property", ArgType::String, "get named property");
  addArg(argv, "-set_property", ArgType::String, "set named property");
  addArg(argv, "-value"       , ArgType::String, "value for set");
}

QStringList
PlotInstTclCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
PlotInstTclCmd::
exec(CQTclCmd::CmdArgs &argv)
{
  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto *widget = static_cast<PlotWidget *>(data());
  assert(widget);

  auto *charts = widget->charts();

  auto errorMsg = [&](const QString &msg) {
    charts->errorMsg(msg);
    return false;
  };

  //---

  auto *plot = widget->plot();
  assert(plot);

  if      (argv.hasParseArg("get")) {
    auto name = argv.getParseStr("get");

    if      (name == "type") {
      auto *type = plot->type();

      return frame_->setCmdRc(type->name());
    }
    else if (name == "?") {
      static auto names = QStringList() << "type";
      return frame_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid -get name");
  }
  else if (argv.hasParseArg("set")) {
    auto name  = argv.getParseStr("set");
    auto value = argv.getParseStr("value");

    if      (name == "type") {
      auto typeName = CQChartsCmds::fixTypeName(value);

      if (! charts->isPlotType(typeName))
        return errorMsg(QString("Invalid plot type '%1'").arg(typeName));

      widget->setType(typeName);

      plot = widget->plot(); // new plot

      //---

      CQChartsAnalyzeModel analyzeModel(charts, plot->getModelData());

      analyzeModel.initPlot(plot);
    }
    else if (name == "?") {
      static auto names = QStringList() << "type";
      return frame_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid -set name");
  }
  else if (argv.hasParseArg("get_property")) {
    auto name = argv.getParseStr("get_property");

    QVariant value;

    if (! plot->getTclProperty(name, value))
      return errorMsg(QString("Failed to get property '%1'").arg(name));

    bool rc;

    return frame_->setCmdRc(CQChartsVariant::toString(value, rc));
  }
  else if (argv.hasParseArg("set_property")) {
    auto name  = argv.getParseStr("set_property");
    auto value = argv.getParseStr("value");

    if (! plot->setProperty(name, value))
      return errorMsg(QString("Failed to set property '%1' to '%2'").arg(name).arg(value));
  }

  return true;
}

//---

class ModelDetailsWidget : public Widget {
 public:
  ModelDetailsWidget(Area *area, CQCharts *charts) :
   Widget(area), charts_(charts) {
    setObjectName("model_details");
  }

  QString id() const override { return QString("model_details.%1").arg(pos()); }

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) { charts_ = charts; }

  CQChartsModelData *modelData() const { return modelData_; }

  void setModelData(CQChartsModelData *modelData) {
    modelData_ = modelData;

    modelDetails_->setModelData(modelData_);
  }

  void addWidgets() override {
    auto *layout = new QVBoxLayout(contents_);
    layout->setMargin(0); layout->setSpacing(0);

    modelDetails_ = new CQChartsModelDetailsTable();

    layout->addWidget(modelDetails_);
  }

  void setExpanded(bool b) override {
    modelDetails_->setVisible(b);

    Widget::setExpanded(b);
  }

  QSize contentsSizeHint() const override { return QSize(-1, 400); }
  QSize contentsSize() const override { return QSize(400, 400); }

 private:
  void draw(QPainter* /*painter*/, int /*dx*/, int /*dy*/) override {
  }

 private:
  CQCharts*                  charts_       { nullptr };
  CQChartsModelData*         modelData_    { nullptr };
  CQChartsModelDetailsTable* modelDetails_ { nullptr };
};

class ModelDetailsFactory : public WidgetFactory {
 public:
  ModelDetailsFactory(CQChartsDataFrameCmd *cmd) :
   cmd_(cmd) {
  }

  const char *name() const override { return "model_details"; }

  CQChartsDataFrameCmd *cmd() const { return cmd_; }

  void addTclCommand(Frame *frame) override {
    auto *cmd = new ModelDetailsTclCmd(frame);

    frame->tclCmdMgr()->addCommand("model_details", cmd);

    cmd->setData(this);
  }

  Widget *addWidget(Area *area) override {
    return makeWidget<ModelDetailsWidget>(area, cmd_->charts());
  }

 private:
  CQChartsDataFrameCmd *cmd_ { nullptr };
};

void
ModelDetailsTclCmd::
addArgs(CQTclCmd::CmdArgs &argv)
{
  addArg(argv, "-model", ArgType::String, "model name");
}

QStringList
ModelDetailsTclCmd::
getArgValues(const QString &option, const NameValueMap &nameValueMap)
{
  QStringList strs;

  if (option == "file") {
    auto p = nameValueMap.find("file");

    QString file = (p != nameValueMap.end() ? (*p).second : "");

    return Frame::s_completeFile(file);
  }

  return strs;
}

bool
ModelDetailsTclCmd::
exec(CQTclCmd::CmdArgs &argv)
{
  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto *factory = static_cast<ModelFactory *>(data());
  auto *cmd     = factory->cmd();

  auto *charts = cmd->charts();

  auto errorMsg = [&](const QString &msg) {
    charts->errorMsg(msg);
    return false;
  };

  //---

  // get model
  auto modelName = argv.getParseStr("model");

  CQChartsModelData *modelData = nullptr;

  if (modelName != "") {
    modelData = cmd->cmds()->getModelDataOrCurrent(modelName);

    if (! modelData) {
      auto *modelWidget = dynamic_cast<ModelWidget *>(frame_->getWidget(modelName));

      if (! modelWidget)
        return errorMsg(QString("No model '%1'").arg(modelName));

      modelData = modelWidget->modelData();
    }
  }
  else {
    modelData = charts->currentModelData();

    if (! modelData)
      return errorMsg("No current model");
  }

  //---

  auto *area = frame_->larea();

  auto *widget = makeWidget<ModelDetailsWidget>(area, charts);

  widget->setModelData(modelData);

  //---

  auto *instCmd = new ModelDetailsInstTclCmd(frame_, widget->id());

  instCmd->setData(widget);

  frame_->tclCmdMgr()->addCommand(widget->id(), instCmd);

  return frame_->setCmdRc(widget->id());
}

//---

void
ModelDetailsInstTclCmd::
addArgs(CQTclCmd::CmdArgs &)
{
}

QStringList
ModelDetailsInstTclCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
ModelDetailsInstTclCmd::
exec(CQTclCmd::CmdArgs &argv)
{
  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  return true;
}

//---

Frame *initFrame(CQChartsDataFrameCmd *cmd) {
  static Frame                *frame;
  static CQChartsDataFrameCmd *dataFrameCmd;

  if (! frame) {
    assert(cmd);

    dataFrameCmd = cmd;

    //---

    frame = new Frame;

    frame->addWidgetFactory(new ModelFactory       (dataFrameCmd));
    frame->addWidgetFactory(new PlotFactory        (dataFrameCmd));
    frame->addWidgetFactory(new ModelDetailsFactory(dataFrameCmd));
  }

  return frame;
}

void showFrame(CQChartsDataFrameCmd *cmd) {
  auto *frame = initFrame(cmd);

  frame->show();
}

void loadFrameFile(const QString &fileName) {
  auto *frame = initFrame(nullptr);

  frame->load(fileName);
}

}

#endif
