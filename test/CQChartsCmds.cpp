#include <CQChartsCmds.h>
#include <CQChartsCmdArgs.h>
#include <CQChartsLoader.h>

#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsCompositePlot.h>
#include <CQChartsTitle.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsAnnotation.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColor.h>
#include <CQChartsLineDash.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQChartsValueSet.h>
#include <CQChartsArrow.h>
#include <CQChartsDataLabel.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsInterfaceTheme.h>
#include <CQChartsSymbolSet.h>
#include <CQChartsSVGUtil.h>
#include <CQChartsFile.h>
#include <CQChartsPointPlot.h>

#include <CQChartsLoadModelDlg.h>
#include <CQChartsManageModelsDlg.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQChartsFilterModel.h>
#include <CQChartsAnalyzeModel.h>
#include <CQChartsTextDlg.h>
#include <CQChartsHelpDlg.h>
#include <CQChartsViewSettings.h>

#ifdef CQCHARTS_DATA_FRAME
#include <CQChartsDataFrame.h>
#endif

#include <CQChartsMetaObj.h>

#include <CQColors.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>

#include <CQBucketModel.h>
#include <CQCollapseModel.h>
#include <CQFoldedModel.h>
#include <CQHierSepModel.h>
#include <CQPivotModel.h>
#include <CQSubSetModel.h>
#include <CQSummaryModel.h>
#include <CQTransposeModel.h>
#include <CQLorenzModel.h>
#include <CQPickoverModel.h>
#include <CQDragon3DModel.h>
#include <CQLeaf3DModel.h>

#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQSortModel.h>
#include <CQDataModel.h>

#include <CQPerfMonitor.h>
#include <CQUtil.h>
#include <CUnixFile.h>
#include <CHRTimer.h>

#include <CQTclUtil.h>

#include <QApplication>
#include <QStackedWidget>
#include <QSortFilterProxyModel>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QLabel>
#include <QGridLayout>
#include <QFont>
#include <QTemporaryFile>
#include <QDir>

#include <fstream>

//------

CQChartsCmds::
CQChartsCmds(CQCharts *charts) :
 charts_(charts)
{
  cmdBase_ = new CQChartsCmdBase(charts_);

  addCommands();
}

CQChartsCmds::
~CQChartsCmds()
{
}

void
CQChartsCmds::
addCommands()
{
  static bool cmdsAdded;

  if (! cmdsAdded) {
    // load, process, sort, fold, filter, flatten, copy, export, write model
    addCommand("load_charts_model"   , new CQChartsLoadChartsModelCmd   (this));
    addCommand("process_charts_model", new CQChartsProcessChartsModelCmd(this));

    addCommand("sort_charts_model"      , new CQChartsSortChartsModelCmd      (this));
    addCommand("fold_charts_model"      , new CQChartsFoldChartsModelCmd      (this));
    addCommand("connection_charts_model", new CQChartsConnectionChartsModelCmd(this));
    addCommand("filter_charts_model"    , new CQChartsFilterChartsModelCmd    (this));
    addCommand("flatten_charts_model"   , new CQChartsFlattenChartsModelCmd   (this));
    addCommand("copy_charts_model"      , new CQChartsCopyChartsModelCmd      (this));
    addCommand("join_charts_model"      , new CQChartsJoinChartsModelCmd      (this));
    addCommand("group_charts_model"     , new CQChartsGroupChartsModelCmd     (this));
    addCommand("export_charts_model"    , new CQChartsExportChartsModelCmd    (this));
    addCommand("write_charts_model"     , new CQChartsWriteChartsModelCmd     (this));

    addCommand("remove_charts_model" , new CQChartsRemoveChartsModelCmd (this));

    // define charts tcl proc
    addCommand("define_charts_proc", new CQChartsDefineChartsProcCmd(this));

    // correlation, bucket, folded, subset, transpose, summary, collapse, pivot, stats,
    // data, fractal
    addCommand("create_charts_correlation_model",
               new CQChartsCreateChartsCorrelationModelCmd(this));
    addCommand("create_charts_bucket_model"     ,
               new CQChartsCreateChartsBucketModelCmd     (this));
    addCommand("create_charts_folded_model"     ,
               new CQChartsCreateChartsFoldedModelCmd     (this));
    addCommand("create_charts_subset_model"     ,
               new CQChartsCreateChartsSubsetModelCmd     (this));
    addCommand("create_charts_transpose_model"  ,
               new CQChartsCreateChartsTransposeModelCmd  (this));
    addCommand("create_charts_summary_model"    ,
               new CQChartsCreateChartsSummaryModelCmd    (this));
    addCommand("create_charts_collapse_model"   ,
               new CQChartsCreateChartsCollapseModelCmd   (this));
    addCommand("create_charts_pivot_model"      ,
               new CQChartsCreateChartsPivotModelCmd      (this));
    addCommand("create_charts_stats_model"      ,
               new CQChartsCreateChartsStatsModelCmd      (this));
    addCommand("create_charts_data_model"       ,
               new CQChartsCreateChartsDataModelCmd       (this));
    addCommand("create_charts_fractal_model"    ,
               new CQChartsCreateChartsFractalModelCmd    (this));

    // add/remove view
    addCommand("create_charts_view", new CQChartsCreateChartsViewCmd(this));
    addCommand("remove_charts_view", new CQChartsRemoveChartsViewCmd(this));

    // add/remove plot
    addCommand("create_charts_plot", new CQChartsCreateChartsPlotCmd(this));
    addCommand("remove_charts_plot", new CQChartsRemoveChartsPlotCmd(this));

    // group/place plots
    addCommand("group_charts_plots", new CQChartsGroupChartsPlotsCmd(this));
    addCommand("place_charts_plots", new CQChartsPlaceChartsPlotsCmd(this));

    // get/set charts property
    addCommand("get_charts_property", new CQChartsGetChartsPropertyCmd(this));
    addCommand("set_charts_property", new CQChartsSetChartsPropertyCmd(this));

    // get/set charts data
    addCommand("get_charts_data", new CQChartsGetChartsDataCmd(this));
    addCommand("set_charts_data", new CQChartsSetChartsDataCmd(this));

    // print variant
    addCommand("print_charts_variant", new CQChartsPrintChartsVariantCmd(this));

    // execute charts slot
    addCommand("execute_charts_slot", new CQChartsExecuteChartsSlotCmd(this));

    // annotations
    addCommand("create_charts_annotation_group",
               new CQChartsCreateChartsAnnotationGroupCmd       (this));
    addCommand("create_charts_annotation",
               new CQChartsCreateChartsAnnotationCmd            (this));
    addCommand("create_charts_arc_annotation",
               new CQChartsCreateChartsArcAnnotationCmd         (this));
    addCommand("create_charts_arc_connector_annotation",
               new CQChartsCreateChartsArcConnectorAnnotationCmd(this));
    addCommand("create_charts_arrow_annotation",
               new CQChartsCreateChartsArrowAnnotationCmd       (this));
    addCommand("create_charts_axis_annotation",
               new CQChartsCreateChartsAxisAnnotationCmd        (this));
    addCommand("create_charts_button_annotation",
               new CQChartsCreateChartsButtonAnnotationCmd      (this));
    addCommand("create_charts_ellipse_annotation",
               new CQChartsCreateChartsEllipseAnnotationCmd     (this));
    addCommand("create_charts_image_annotation",
               new CQChartsCreateChartsImageAnnotationCmd       (this));
    addCommand("create_charts_path_annotation",
               new CQChartsCreateChartsPathAnnotationCmd        (this));
    addCommand("create_charts_key_annotation",
               new CQChartsCreateChartsKeyAnnotationCmd         (this));
    addCommand("create_charts_pie_slice_annotation",
               new CQChartsCreateChartsPieSliceAnnotationCmd    (this));
    addCommand("create_charts_point_annotation",
               new CQChartsCreateChartsPointAnnotationCmd       (this));
    addCommand("create_charts_point_set_annotation",
               new CQChartsCreateChartsPointSetAnnotationCmd    (this));
    addCommand("create_charts_point3d_set_annotation",
               new CQChartsCreateChartsPoint3DSetAnnotationCmd  (this));
    addCommand("create_charts_polygon_annotation",
               new CQChartsCreateChartsPolygonAnnotationCmd     (this));
    addCommand("create_charts_polyline_annotation",
               new CQChartsCreateChartsPolylineAnnotationCmd    (this));
    addCommand("create_charts_rectangle_annotation",
               new CQChartsCreateChartsRectangleAnnotationCmd   (this));
    addCommand("create_charts_shape_annotation",
               new CQChartsCreateChartsShapeAnnotationCmd       (this));
    addCommand("create_charts_text_annotation",
               new CQChartsCreateChartsTextAnnotationCmd        (this));
    addCommand("create_charts_value_set_annotation",
               new CQChartsCreateChartsValueSetAnnotationCmd    (this));
    addCommand("create_charts_widget_annotation",
               new CQChartsCreateChartsWidgetAnnotationCmd      (this));
    addCommand("create_charts_symbol_map_key_annotation",
               new CQChartsCreateChartsSymbolMapKeyAnnotationCmd(this));
    addCommand("remove_charts_annotation",
               new CQChartsRemoveChartsAnnotationCmd            (this));

    // key
    addCommand("add_charts_key_item", new CQChartsAddChartsKeyItemCmd(this));

    // theme/palette
    addCommand("create_charts_palette", new CQChartsCreateChartsPaletteCmd(this));
    addCommand("get_charts_palette"   , new CQChartsGetChartsPaletteCmd   (this));
    addCommand("set_charts_palette"   , new CQChartsSetChartsPaletteCmd   (this));

    // symbols
    addCommand("create_charts_symbol_set", new CQChartsCreateChartsSymbolSetCmd(this));
    addCommand("add_charts_symbol"       , new CQChartsAddChartsSymbolCmd      (this));

    // connect
    addCommand("connect_charts_signal", new CQChartsConnectChartsSignalCmd(this));

    // widgets
    addCommand("add_custom_widget", new CQChartsAddCustomWidgetCmd(this));

    // print, write
    addCommand("print_charts_image", new CQChartsPrintChartsImageCmd(this));
    addCommand("write_charts_data" , new CQChartsWriteChartsDataCmd(this));
    addCommand("write_charts_stats", new CQChartsWriteChartsStatsCmd(this));

    // measure/encode text
    addCommand("measure_charts_text", new CQChartsMeasureChartsTextCmd(this));
    addCommand("encode_charts_text" , new CQChartsEncodeChartsTextCmd (this));

    addCommand("bucket_charts_values", new CQChartsBucketChartsValuesCmd(this));

    // dialogs
    addCommand("show_charts_load_model_dlg"   , new CQChartsShowChartsLoadModelDlgCmd(this));
    addCommand("show_charts_manage_models_dlg", new CQChartsShowChartsManageModelsDlgCmd(this));
    addCommand("show_charts_create_plot_dlg"  , new CQChartsShowChartsCreatePlotDlgCmd(this));
    addCommand("show_charts_text_dlg"         , new CQChartsShowChartsTextDlgCmd(this));
    addCommand("show_charts_help_dlg"         , new CQChartsShowChartsHelpDlgCmd(this));

    // test
    //addCommand("charts::test_edit", new CQChartsTestEditCmd(this));
    addCommand("test_charts_edit", new CQChartsTestEditCmd(this));

#ifdef CQCHARTS_DATA_FRAME
    // data frame
    addCommand("data_frame", new CQChartsDataFrameCmd(this));
#endif

    //---

    cmdBase_->addCommands();

    //---

    cmdsAdded = true;
  }
}

void
CQChartsCmds::
addCommand(const QString &name, CQChartsCmdProc *proc)
{
  cmdBase_->addCommand(name, proc);

  CQChartsHelpDlgMgrInst->addTclCommand(name);
}

//------

// load_charts_model command

void
CQChartsLoadChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  // input data type
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-csv" , ArgType::Boolean, "load csv file");
  addArg(argv, "-tsv" , ArgType::Boolean, "load tsv file");
  addArg(argv, "-json", ArgType::Boolean, "load json file");
  addArg(argv, "-data", ArgType::Boolean, "load gnuplot file");
  addArg(argv, "-expr", ArgType::Boolean, "use expression model");
  addArg(argv, "-var" , ArgType::String , "load from tcl variable(s)");
  addArg(argv, "-tcl" , ArgType::String , "load from tcl data");
  argv.endCmdGroup();

  addArg(argv, "-spreadsheet", ArgType::Boolean, "convert to spreadsheet model");

  // input data control
  addArg(argv, "-comment_header"     , ArgType::Boolean, "first comment is horizontal header");
  addArg(argv, "-first_line_header"  , ArgType::Boolean, "first line is horizontal header");
  addArg(argv, "-first_column_header", ArgType::Boolean, "first column is vertical header");

  addArg(argv, "-separator", ArgType::String , "separator char for csv");
  addArg(argv, "-columns"  , ArgType::String , "columns to load");
  addArg(argv, "-transpose", ArgType::Boolean, "transpose tcl data");

  addArg(argv, "-num_rows"   , ArgType::Integer, "number of expression rows");
  addArg(argv, "-max_rows"   , ArgType::Integer, "maximum number of file rows");
  addArg(argv, "-filter"     , ArgType::String , "filter expression");
  addArg(argv, "-filter_type", ArgType::String , "filter expression type");
  addArg(argv, "-column_type", ArgType::String , "column type");
  addArg(argv, "-name"       , ArgType::String , "name for model");

  addArg(argv, "filename", ArgType::String, "file name");
}

QStringList
CQChartsLoadChartsModelCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

// load model from data
bool
CQChartsLoadChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsLoadChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsInputData inputData;

  CQChartsFileType fileType { CQChartsFileType::NONE };

  if      (argv.getParseBool("csv" )) fileType = CQChartsFileType::CSV;
  else if (argv.getParseBool("tsv" )) fileType = CQChartsFileType::TSV;
  else if (argv.getParseBool("json")) fileType = CQChartsFileType::JSON;
  else if (argv.getParseBool("data")) fileType = CQChartsFileType::DATA;
  else if (argv.getParseBool("expr")) fileType = CQChartsFileType::EXPR;
  else if (argv.hasParseArg ("var") ) {
    auto strs = argv.getParseStrs("var");

    for (int i = 0; i < strs.length(); ++i)
      inputData.vars.push_back(strs[i]);

    fileType = CQChartsFileType::VARS;
  }
  else if (argv.hasParseArg("tcl") ) {
    auto strs = argv.getParseStrs("tcl");

    // { { <column_values> } { <column_values> } ... }
    for (int i = 0; i < strs.length(); ++i) {
      // split into strings per column
      QStringList columnsStrs;

      if (! CQTcl::splitList(strs[i], columnsStrs))
        continue;

      for (int j = 0; j < columnsStrs.length(); ++j) {
        // split into strings per column
        QStringList columnStrs;

        if (! CQTcl::splitList(columnsStrs[j], columnStrs))
          continue;

        inputData.vars.emplace_back(columnStrs);
      }
    }

    fileType = CQChartsFileType::TCL;
  }

  inputData.commentHeader     = argv.getParseBool("comment_header"     );
  inputData.firstLineHeader   = argv.getParseBool("first_line_header"  );
  inputData.firstColumnHeader = argv.getParseBool("first_column_header");

  inputData.separator = argv.getParseStr("separator");

  //---

  // column names
  auto columnsStr = argv.getParseStr("columns");

  if (! CQTcl::splitList(columnsStr, inputData.columns))
    return errorMsg(QString("Invalid columns string '%1'").arg(columnsStr));

  //---

  inputData.transpose = argv.getParseBool("transpose");

  if (argv.hasParseArg("num_rows"))
    inputData.numRows = std::max(argv.getParseInt("num_rows"), 1);

  if (argv.hasParseArg("max_rows"))
    inputData.maxRows = std::max(argv.getParseInt("max_rows"), 1);

  inputData.filter = argv.getParseStr("filter");

  if (argv.hasParseArg("filter_type")) {
    auto filterTypeStr = argv.getParseStr("filter_type").toLower();

    if      (filterTypeStr == "expression")
      inputData.filterType = CQChartsFilterModelType::EXPRESSION;
    else if (filterTypeStr == "regexp")
      inputData.filterType = CQChartsFilterModelType::REGEXP;
    else if (filterTypeStr == "wildcard")
      inputData.filterType = CQChartsFilterModelType::WILDCARD;
    else if (filterTypeStr == "simple")
      inputData.filterType = CQChartsFilterModelType::SIMPLE;
    else
      return errorMsg("Invalid filter type '" + filterTypeStr + "'");
  }

  auto columnTypes = argv.getParseStrs("column_type");

  auto name = argv.getParseStr("name");

  // TODO: columns (filter to columns)

  const auto &filenameArgs = argv.getParseArgs();

  auto filename = (! filenameArgs.empty() ? filenameArgs[0].toString() : QString());

  //---

  if (fileType == CQChartsFileType::NONE)
    return errorMsg("No file type");

  if (fileType != CQChartsFileType::EXPR &&
      fileType != CQChartsFileType::VARS &&
      fileType != CQChartsFileType::TCL) {
    if (filename == "")
      return errorMsg("No filename");
  }
  else {
    if (filename != "")
      return errorMsg("Extra filename");
  }

  //---

  bool spreadsheet = argv.getParseBool("spreadsheet");

  inputData.spreadsheet = spreadsheet;

  //---

  auto *charts = this->charts();

  if (filename != "" && filename[0] == '@') {
#if 1
    QTemporaryFile tempFile(QDir::tempPath() + "/XXXXXX.tcl");

    if (! tempFile.open())
      return errorMsg("Can't open temp file");
#else
    QFile tempFile("/tmp/charts_model.tcl");

    if (! tempFile.open(QIODevice::WriteOnly))
      return false;
#endif

    auto data = cmdBase_->qtcl()->getVar(filename.mid(1)).toString();
    //std::cerr << data.toStdString() << "\n";

    tempFile.write(data.toLatin1().constData());

    tempFile.close();

    CQChartsFile file(charts, tempFile.fileName());

    auto rc = cmds()->loadFileModel(file, fileType, inputData);

    if (! rc)
      return false;
  }
  else {
    CQChartsFile file(charts, filename);

    if (! cmds()->loadFileModel(file, fileType, inputData))
      return false;
  }

  //---

  auto *modelData = charts->currentModelData();

  if (! modelData)
    return false;

  if (columnTypes.length()) {
    auto model = modelData->currentModel();

    for (int i = 0; i < columnTypes.length(); ++i) {
      const auto &columnType = columnTypes[i];

      if (! CQChartsModelUtil::setColumnTypeStrs(charts, model.data(), columnType))
        return errorMsg(QString("Invalid column type string '%1'").arg(columnType));
    }
  }

  if (name.length())
    modelData->setName(name);

  return cmdBase_->setCmdRc(modelData->id());
}

//------

// process_charts_model command

void
CQChartsProcessChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model" , ArgType::String, "model_id").setRequired();
  addArg(argv, "-column", ArgType::Column, "column for delete, modify, calc, query, analyze");

  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-add"         , ArgType::Boolean, "add column");
  addArg(argv, "-delete"      , ArgType::Boolean, "delete column");
  addArg(argv, "-modify"      , ArgType::Boolean, "modify column values");
  addArg(argv, "-calc"        , ArgType::Boolean, "calc column");
  addArg(argv, "-query"       , ArgType::Boolean, "query column");
  addArg(argv, "-analyze"     , ArgType::Boolean, "analyze data");
  addArg(argv, "-replace"     , ArgType::String , "replace value");
  addArg(argv, "-replace_null", ArgType::Boolean, "replace null values");
  argv.endCmdGroup();

  addArg(argv, "-header", ArgType::String, "header label for add/modify");
  addArg(argv, "-type"  , ArgType::String, "type data for add/modify");
  addArg(argv, "-expr"  , ArgType::String, "expression for add/modify/calc/query");
  addArg(argv, "-vars"  , ArgType::String, "variables for expression");
  addArg(argv, "-tcl"   , ArgType::String, "tcl data for add/modify");
  addArg(argv, "-value" , ArgType::String, "value for replace and replace null");

  addArg(argv, "-force", ArgType::Boolean, "force modify of original data");
  addArg(argv, "-debug", ArgType::Boolean, "debug expression evaulation");
}

QStringList
CQChartsProcessChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsProcessChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsProcessChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  class AutoExprDebug {
   public:
    AutoExprDebug(CQChartsExprModel *exprModel, bool debug) :
     exprModel_(exprModel) {
      if (exprModel_) {
        oldDebug_ = exprModel_->isDebug();

        exprModel_->setDebug(debug);
      }
    }

   ~AutoExprDebug() {
      if (exprModel_)
        exprModel_->setDebug(oldDebug_);
    }

   private:
    CQChartsExprModel *exprModel_ { nullptr };
    bool               oldDebug_  { false };
  };

  //---

  auto header = argv.getParseStr("header");
  auto type   = argv.getParseStr("type");
  auto expr   = argv.getParseStr("expr");

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  //---

  // get expr model
  auto model = modelData->currentModel();

  auto *exprModel = CQChartsModelUtil::getExprModel(model.data());

  bool debug = argv.getParseBool("debug");

  AutoExprDebug autoExprDebug(exprModel, debug);

  //---

  CQChartsExprModel::NameValues varNameValues;

  if (argv.hasParseArg("vars")) {
    if (! exprModel)
      return errorMsg("Vars not supported for model");

    auto vars = argv.getParseStr("vars");

    // split into variable names
    QStringList varsStrs;

    if (! CQTcl::splitList(vars, varsStrs))
      varsStrs = QStringList();

    for (auto &varStr : varsStrs) {
      // split into name values
      QStringList nameValueStrs;

      if (! CQTcl::splitList(varStr, nameValueStrs))
        nameValueStrs = QStringList();

      if (nameValueStrs.length() != 2)
        return errorMsg(QString("Invalid variable name/value '%1'").arg(varStr));

      varNameValues[nameValueStrs[0]] = nameValueStrs[1];
    }
  }

  //---

  // add new column (values from result of expression)
  if      (argv.getParseBool("add")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    int column;

    if      (argv.hasParseArg("expr")) {
      if (! exprModel->addExtraColumnExpr(header, expr, column))
        return errorMsg("Failed to add column");
    }
    else if (argv.hasParseArg("tcl")) {
      auto str = argv.getParseStr("tcl");

      // split into extra column strings
      QStringList strs;

      (void) CQTcl::splitList(str, strs);

      if (! exprModel->addExtraColumnStrs(header, strs, column))
        return errorMsg("Failed to add column");
    }
    else {
      return errorMsg("Missing -expr or -tcl value");
    }

    //---

    if (type.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts(), model.data(),
                                                CQChartsColumn(column), type))
        return errorMsg(QString("Invalid column type '%1'").arg(type));
    }

    return cmdBase_->setCmdRc(column);
  }
  // remove column (must be an added one)
  else if (argv.getParseBool("delete")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    auto column = argv.getParseColumn("column", model.data());

    if (! exprModel->removeExtraColumn(column.column()))
      return errorMsg("Failed to delete column");

    return cmdBase_->setCmdRc(-1);
  }
  // modify column (values from result of expression)
  else if (argv.getParseBool("modify")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");

    auto column = argv.getParseColumn("column", model.data());

    if (exprModel->isOrigColumn(column.column())) {
      if (argv.getParseBool("force")) {
        bool rc;

        if (exprModel->isReadOnly()) {
          exprModel->setReadOnly(false);

          rc = exprModel->assignColumn(header, column.column(), expr);

          exprModel->setReadOnly(true);
        }
        else
          rc = exprModel->assignColumn(header, column.column(), expr);

        if (! rc)
          return errorMsg(QString("Failed to modify column '%1'").arg(column.column()));
      }
      else
        return errorMsg("Use -force to modify original model data");
    }
    else {
      if (! exprModel->assignExtraColumn(header, column.column(), expr))
        return errorMsg(QString("Failed to modify column '%1'").arg(column.column()));
    }

    //---

    if (type.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts(), model.data(), column, type))
        return errorMsg(QString("Invalid column type '%1'").arg(type));
    }

    return cmdBase_->setCmdRc(column.column());
  }
  // calculate values from result of expression
  else if (argv.getParseBool("calc")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");

    auto column = argv.getParseColumn("column", model.data());

    CQChartsExprModel::Values values;

    exprModel->calcColumn(column.column(), expr, values, varNameValues);

    QVariantList vars;

    for (const auto &var : values)
      vars.push_back(var);

    return cmdBase_->setCmdRc(vars);
  }
  // query rows where result of expression is true
  else if (argv.getParseBool("query")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");

    auto column = argv.getParseColumn("column", model.data());

    CQChartsExprModel::Rows rows;

    exprModel->queryColumn(column.column(), expr, rows);

    QVariantList vars;

    for (const auto &row : rows)
      vars.push_back(row);

    return cmdBase_->setCmdRc(vars);
  }
  else if (argv.getParseBool("analyze")) {
    CQChartsAnalyzeModel analyzeModel(charts(), modelData);

    if (type != "") {
      if (! charts()->isPlotType(type))
        return errorMsg("Invalid type '" + type + "'");

      auto *plotType = charts()->plotType(type);
      assert(plotType);

      analyzeModel.analyzeType(plotType);

      const auto &analyzeModelData = analyzeModel.analyzeModelData(plotType);

      QVariantList tncvars;

      for (const auto &nc : analyzeModelData.parameterNameColumn) {
        const auto &name   = nc.first;
        const auto &column = nc.second;

        QVariantList ncvars;

        ncvars.push_back(name);
        ncvars.push_back(column.toString());

        tncvars.push_back(std::move(ncvars));
      }

      return cmdBase_->setCmdRc(tncvars);
    }
    else {
      analyzeModel.analyze();

      const auto &typeAnalyzeModelData = analyzeModel.typeAnalyzeModelData();

      QVariantList tvars;

      for (const auto &tnc : typeAnalyzeModelData) {
        QVariantList cvars;

        const auto &typeName = tnc.first;

        cvars.push_back(typeName);

        QVariantList tncvars;

        const auto &analyzeModelData = tnc.second;

        for (const auto &nc : analyzeModelData.parameterNameColumn) {
          const auto &name   = nc.first;
          const auto &column = nc.second;

          QVariantList ncvars;

          ncvars.push_back(name);
          ncvars.push_back(column.toString());

          tncvars.push_back(std::move(ncvars));
        }

        cvars.push_back(std::move(tncvars));
        tvars.push_back(std::move(cvars));
      }

      return cmdBase_->setCmdRc(tvars);
    }

    analyzeModel.print(std::cerr);
  }
  else if (argv.hasParseArg("replace")) {
    auto oldValue = argv.getParseStr("replace");

    //---

    if (! argv.hasParseArg("column"))
      return errorMsg("Missing column for -replace");

    auto column = argv.getParseColumn("column", model.data());

    if (! column.isValid())
      return errorMsg("Invalid/missing column for -replace");

    //---

    if (! argv.hasParseArg("value"))
      return errorMsg("Missing value for -replace");

    auto newValue = argv.getParseStr("value");

    //---

    int n = modelData->replaceValue(column, oldValue, newValue);

    return cmdBase_->setCmdRc(n);
  }
  else if (argv.getParseBool("replace_null")) {
    if (! argv.hasParseArg("column"))
      return errorMsg("Missing column for -replace_null");

    auto column = argv.getParseColumn("column", model.data());

    if (! column.isValid())
      return errorMsg("Invalid/missing column for -replace_null");

    //---

    if (! argv.hasParseArg("value"))
      return errorMsg("Missing value for -replace_null");

    auto value = argv.getParseStr("value");

    //---

    int n = modelData->replaceNullValues(column, value);

    return cmdBase_->setCmdRc(n);
  }
  else {
#if 0
    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");

    auto function = CQChartsExprModel::Function::EVAL;

    if (expr.trimmed().length())
      processExpression(model.data(), expr);
#endif
  }

  return true;
}

//------

// define_charts_proc command

void
CQChartsDefineChartsProcCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-svg"   , ArgType::Boolean, "define svg proc");
  addArg(argv, "-script", ArgType::Boolean, "define script proc");

  addArg(argv, "name", ArgType::String, "proc name").setRequired();
  addArg(argv, "args", ArgType::String, "proc args").setRequired();
  addArg(argv, "body", ArgType::String, "proc body").setRequired();
}

QStringList
CQChartsDefineChartsProcCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsDefineChartsProcCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsDefineChartsProcCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  bool svgFlag    = argv.getParseBool("svg");
  bool scriptFlag = argv.getParseBool("script");

  const auto &pargs = argv.getParseArgs();

  if (pargs.size() != 3)
    return errorMsg("Usage: define_charts_proc [-svg|-script] <name> <args> <body>");

  //---

  auto name = pargs[0].toString();
  auto args = pargs[1].toString();
  auto body = pargs[2].toString();

  //---

  if      (svgFlag)
    charts()->addProc(CQCharts::ProcType::SVG, name, args, body);
  else if (scriptFlag)
    charts()->addProc(CQCharts::ProcType::SCRIPT, name, args, body);
  else
    charts()->addProc(CQCharts::ProcType::TCL, name, args, body);

  return true;
}

//------

// measure_charts_text command

void
CQChartsMeasureChartsTextCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-name", ArgType::String , "value name").setRequired();
  addArg(argv, "-text", ArgType::String , "text string");
  addArg(argv, "-html", ArgType::Boolean, "is html");
}

QStringList
CQChartsMeasureChartsTextCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);
  else if (arg == "name") {
    return QStringList() << "width" << "height" << "ascent" << "descent";
  }

  return QStringList();
}

bool
CQChartsMeasureChartsTextCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsMeasureChartsTextCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  auto name = argv.getParseStr ("name");
  auto text = argv.getParseStr ("text");
  bool html = argv.getParseBool("html");

  //---

  QFont font;

  if      (plot)
    font = plot->view()->plotFont(plot, plot->view()->font());
  else if (view)
    font = view->viewFont(view->font());

  double tw = 0.0, ta = 0.0, td = 0.0;

  if (! html) {
    QFontMetricsF fm(font);

    tw = fm.horizontalAdvance(text);
    ta = fm.ascent();
    td = fm.descent();
  }
  else {
    QTextDocument tdoc;

    tdoc.setHtml(text);
    tdoc.setDefaultFont(font);

    auto *layout = tdoc.documentLayout();

    auto size = layout->documentSize();

    tw = size.width ();
    ta = size.height();
    td = 0.0;
  }

  if      (name == "width") {
    if      (plot)
      return cmdBase_->setCmdRc(plot->pixelToWindowWidth(tw));
    else if (view)
      return cmdBase_->setCmdRc(view->pixelToWindowWidth(tw));
  }
  else if (name == "height") {
    if      (plot)
      return cmdBase_->setCmdRc(plot->pixelToWindowHeight(ta + td));
    else if (view)
      return cmdBase_->setCmdRc(view->pixelToWindowHeight(ta + td));
  }
  else if (name == "ascent") {
    if      (plot)
      return cmdBase_->setCmdRc(plot->pixelToWindowHeight(ta));
    else if (view)
      return cmdBase_->setCmdRc(view->pixelToWindowHeight(ta));
  }
  else if (name == "descent") {
    if      (plot)
      return cmdBase_->setCmdRc(plot->pixelToWindowHeight(td));
    else if (view)
      return cmdBase_->setCmdRc(view->pixelToWindowHeight(td));
  }
  else if (name == "?")
    return cmdBase_->setCmdRc(getArgValues("name"));
  else {
    return errorMsg(QString("Invalid value name '%1'").arg(name));
  }

  return true;
}

//------

// encode_charts_text command

void
CQChartsEncodeChartsTextCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-csv", ArgType::Boolean, "encode text for csv");
  addArg(argv, "-tsv", ArgType::Boolean, "encode text for tsv");
  argv.endCmdGroup();

  addArg(argv, "-text", ArgType::String, "text string");
}

QStringList
CQChartsEncodeChartsTextCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsEncodeChartsTextCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsEncodeChartsTextCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool csv  = argv.getParseBool("csv" );
  bool tsv  = argv.getParseBool("tsv" );
  auto text = argv.getParseStr ("text");

  auto text1 = text;

  if      (csv)
    text1 = CQCsvModel::encodeString(text);
  else if (tsv)
    text1 = CQTsvModel::encodeString(text);

  return cmdBase_->setCmdRc(text1);
}

//------

// bucket_charts_values command

void
CQChartsBucketChartsValuesCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-values", ArgType::String, "values to bucket");
}

QStringList
CQChartsBucketChartsValuesCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBucketChartsValuesCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBucketChartsValuesCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto str = argv.getParseStr("values");

  // split into values
  QStringList strs;

  (void) CQTcl::splitList(str, strs);

  CQChartsRValues rvals;

  for (const auto &str : strs) {
    bool ok;
    double r = CQChartsUtil::toReal(str, ok);

    rvals.addValue(r);
  }

  CQBucketer bucketer;

  bucketer.setType(CQBucketer::Type::REAL_AUTO);
  bucketer.setNumAuto(20);
  bucketer.setRMin(rvals.min());
  bucketer.setRMax(rvals.max());

  using BucketCount = std::map<int, int>;

  BucketCount bucketCount;

  for (const auto &v : rvals.values()) {
    int bucket = bucketer.bucket(v.value());

    bucketCount[bucket]++;
  }

  QVariantList vars;

  for (const auto &bc : bucketCount) {
    double rmin, rmax;

    bucketer.bucketValues(bc.first, rmin, rmax);

    QVariantList vars1;

    vars1.push_back(CQModelUtil::realVariant(rmin));
    vars1.push_back(CQModelUtil::realVariant(rmax));
    vars1.push_back(CQModelUtil::intVariant(bc.second));

    vars.push_back(vars1);
  }

  return cmdBase_->setCmdRc(vars);
}

//------

// create_charts_view command

void
CQChartsCreateChartsViewCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-3d", ArgType::Boolean, "3d view");
}

QStringList
CQChartsCreateChartsViewCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsCreateChartsViewCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCreateChartsViewCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool is3D = argv.getParseBool("3d");

  auto *view = cmds()->addView();

  view->set3D(is3D);

  //---

  return cmdBase_->setCmdRc(view->id());
}

//------

// remove_charts_view command

void
CQChartsRemoveChartsViewCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-view", ArgType::String, "view_id").setRequired();
}

QStringList
CQChartsRemoveChartsViewCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "view") return cmds()->viewArgValues();

  return QStringList();
}

bool
CQChartsRemoveChartsViewCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsRemoveChartsViewCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view;

  if (! cmds()->getViewArg(argv, view))
    return false;

  //---

  auto *window = CQChartsWindowMgrInst->getWindowForView(view);

  charts()->deleteWindow(window);

  return true;
}

//------

// create_charts_plot command

void
CQChartsCreateChartsPlotCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model", ArgType::String, "model_id").setRequired();
  addArg(argv, "-view" , ArgType::String, "view_id" ).setRequired();
  addArg(argv, "-type" , ArgType::String, "type"    ).setRequired();
  addArg(argv, "-id"   , ArgType::String, "plot id" );

  addArg(argv, "-where"    , ArgType::String, "filter");
  addArg(argv, "-columns"  , ArgType::String, "columns");
  addArg(argv, "-parameter", ArgType::String, "name value").setHidden(true);

  addArg(argv, "-xintegral", ArgType::SBool).setHidden(true);
  addArg(argv, "-yintegral", ArgType::SBool).setHidden(true);
  addArg(argv, "-xlog"     , ArgType::SBool).setHidden(true);
  addArg(argv, "-ylog"     , ArgType::SBool).setHidden(true);

  addArg(argv, "-title"     , ArgType::String, "title");
  addArg(argv, "-properties", ArgType::String, "name_values");
  addArg(argv, "-position"  , ArgType::String, "position box");
  addArg(argv, "-xmin"      , ArgType::Real  , "x");
  addArg(argv, "-ymin"      , ArgType::Real  , "y");
  addArg(argv, "-xmax"      , ArgType::Real  , "x");
  addArg(argv, "-ymax"      , ArgType::Real  , "y");
}

QStringList
CQChartsCreateChartsPlotCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "model") return cmds()->modelArgValues();
  else if (arg == "view" ) return cmds()->viewArgValues();
  else if (arg == "type" ) return cmds()->plotTypeArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsPlotCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPlotCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto viewName    = argv.getParseStr    ("view");
  auto typeName    = argv.getParseStr    ("type");
  auto id          = argv.getParseStr    ("id");
  auto filterStr   = argv.getParseStr    ("where");
  auto title       = argv.getParseStr    ("title");
  auto properties  = argv.getParseStrs   ("properties");
  auto positionStr = argv.getParseStr    ("position");
  auto xmin        = argv.getParseOptReal("xmin");
  auto ymin        = argv.getParseOptReal("ymin");
  auto xmax        = argv.getParseOptReal("xmax");
  auto ymax        = argv.getParseOptReal("ymax");

  //---

  // get view
  auto *view = cmds()->getViewByName(viewName);
  if (! view) return false;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);

  CQChartsCmds::ModelP model;

  if (modelData) {
    model = modelData->currentModel();
  }
  else {
    if (typeName != "empty")
      return errorMsg("No model data");
  }

  //------

  CQChartsNameValueData nameValueData;

  //--

  // plot columns (name (plot column) and value (model column))
  auto columnsStrs = argv.getParseStrs("columns");

  for (int i = 0; i < columnsStrs.length(); ++i) {
    const auto &columnsStr = columnsStrs[i];

    if (! columnsStr.length())
      continue;

    // split into columns
    QStringList strs;

    if (! CQTcl::splitList(columnsStr, strs))
      return errorMsg(QString("Invalid columns string '%1'").arg(columnsStr));

    for (int j = 0; j < strs.size(); ++j) {
      const auto &nameValue = strs[j];

      // split into name values
      QStringList strs1;

      if (! CQTcl::splitList(nameValue, strs1))
        return errorMsg(QString("Invalid column name/value string '%1'").arg(nameValue));

      if (strs1.length() != 2)
        return errorMsg(QString("Invalid column name/value string '%1'").arg(nameValue));

      auto name  = strs1[0];
      auto value = strs1[1];

      nameValueData.columns[name] = value;
    }
  }

  //--

  // plot parameter
  auto parameterStrs = argv.getParseStrs("parameter");

  for (int i = 0; i < parameterStrs.length(); ++i) {
    const auto &parameterStr = parameterStrs[i];

    QString name, value;

    // split into parameter name/value
    QStringList strs1;

    if (! CQTcl::splitList(parameterStr, strs1))
      return errorMsg(QString("Invalid parameter name/value string '%1'").arg(parameterStr));

    if (strs1.length() != 2)
      return errorMsg(QString("Invalid parameter name/value string '%1'").arg(parameterStr));

    name  = strs1[0];
    value = strs1[1];

    nameValueData.parameters[name] = value;
  }

  //------

  if (typeName == "")
    return errorMsg("No type specified for plot");

  typeName = CQChartsCmds::fixTypeName(typeName);

  // ignore if bad type
  auto *charts = this->charts();

  if (! charts->isPlotType(typeName))
    return errorMsg("Invalid type '" + typeName + "' for plot");

  auto *type = charts->plotType(typeName);
  assert(type);

  //------

  double vr = CQChartsView::viewportRange();

  CQChartsGeom::BBox bbox(0, 0, vr, vr);

  if (positionStr != "") {
    // split into position values (xmin, ymin, xmax, ymax)
    QStringList positionStrs;

    if (! CQTcl::splitList(positionStr, positionStrs))
      return errorMsg(QString("Invalid position string '%1'").arg(positionStr));

    if (positionStrs.length() != 4)
      return errorMsg("Invalid position '" + positionStr + "'");

    bool ok1, ok2, ok3, ok4;

    double pxmin = CQChartsUtil::toReal(positionStrs[0], ok1);
    double pymin = CQChartsUtil::toReal(positionStrs[1], ok2);
    double pxmax = CQChartsUtil::toReal(positionStrs[2], ok3);
    double pymax = CQChartsUtil::toReal(positionStrs[3], ok4);

    if (! ok1 || ! ok2 || ! ok3 || ! ok4)
      return errorMsg("Invalid position '" + positionStr + "'");

    bbox = CQChartsGeom::BBox(pxmin, pymin, pxmax, pymax);
  }

  //------

  // create plot from init (argument) data
  auto *plot = cmds()->createPlot(view, model, type, true);

  if (! plot)
    return errorMsg("Failed to create plot");

  //---

  plot->setUpdatesEnabled(false);

  if (! cmds()->initPlot(plot, nameValueData, bbox))
    return false;

  //---

  // init plot
  if (title != "")
    plot->setTitleStr(title);

  //---

  // set x/y log if allowed)
  if (argv.hasParseArg("xlog")) {
    bool xlog = argv.getParseBool("xlog");

    if (! type->allowXLog())
      return errorMsg("plot type does not support x log option");

    plot->setLogX(xlog);
  }

  if (argv.hasParseArg("ylog")) {
    bool ylog = argv.getParseBool("ylog");

    if (! type->allowYLog())
      return errorMsg("plot type does not support y log option");

    plot->setLogY(ylog);
  }

  //---

  // set x/y integral if allowed)
  if (argv.hasParseArg("xintegral")) {
    bool xintegral = argv.getParseBool("xintegral");

    if (! type->allowXAxisIntegral() || ! plot->xAxis())
      return errorMsg("plot type does not support x integral option");

    plot->xAxis()->setValueType(
      CQChartsAxisValueType(xintegral ? CQChartsAxisValueType::Type::INTEGER :
                                        CQChartsAxisValueType::Type::REAL));
  }

  if (argv.hasParseArg("yintegral")) {
    bool yintegral = argv.getParseBool("yintegral");

    if (! type->allowYAxisIntegral() || ! plot->yAxis())
      return errorMsg("plot type does not support y integral option");

    plot->yAxis()->setValueType(
      CQChartsAxisValueType(yintegral ? CQChartsAxisValueType::Type::INTEGER :
                                        CQChartsAxisValueType::Type::REAL));
  }

  //---

  if (filterStr.length())
    plot->setFilterStr(filterStr);

  if (xmin) plot->setXMin(xmin);
  if (ymin) plot->setYMin(ymin);
  if (xmax) plot->setXMax(xmax);
  if (ymax) plot->setYMax(ymax);

  //---

  for (int i = 0; i < properties.length(); ++i) {
    // split into properites name values
    QStringList strs;

    if (! CQTcl::splitList(properties[i], strs))
      return errorMsg(QString("Invalid properties string '%1'").arg(properties[i]));

    for (int j = 0; j < strs.length(); ++j) {
      // split into name value
      QStringList strs1;

      if (! CQTcl::splitList(strs[j], strs1))
        return errorMsg(QString("Invalid property string '%1'").arg(strs[j]));

      if (strs1.size() != 2)
        return errorMsg(QString("Invalid property string '%1'").arg(strs[j]));

      plot->setProperty(strs1[0], strs1[1]);
    }
  }

  //---

  if (id != "")
    plot->setId(id);

  plot->setUpdatesEnabled(true);

  //---

  return cmdBase_->setCmdRc(plot->pathId());
}

//------

// remove_charts_plot command

void
CQChartsRemoveChartsPlotCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-view", ArgType::String, "view_id").setRequired();

  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-plot", ArgType::String, "plot_id");
  addArg(argv, "-all" , ArgType::Boolean);
  argv.endCmdGroup();
}

QStringList
CQChartsRemoveChartsPlotCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view" ) return cmds()->viewArgValues();
  else if (arg == "plot" ) return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsRemoveChartsPlotCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsRemoveChartsPlotCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto viewName = argv.getParseStr ("view");
  auto plotName = argv.getParseStr ("plot");
  bool all      = argv.getParseBool("all");

  //---

  // get view
  auto *view = cmds()->getViewByName(viewName);
  if (! view) return false;

  //---

  if (all) {
    view->removeAllPlots();
  }
  else {
    auto *plot = cmds()->getPlotByName(view, plotName);
    if (! plot) return false;

    view->removePlot(plot);
  }

  return true;
}

//------

// get_charts_property command

void
CQChartsGetChartsPropertyCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-model"     , ArgType::String, "model_id");
  addArg(argv, "-view"      , ArgType::String, "view name");
  addArg(argv, "-plot"      , ArgType::String, "plot name");
  addArg(argv, "-annotation", ArgType::String, "annotation name");
  addArg(argv, "-key_item"  , ArgType::String, "key item name");
  argv.endCmdGroup();

  addArg(argv, "-object", ArgType::String , "object id");
  addArg(argv, "-name"  , ArgType::String , "property name");
  addArg(argv, "-data"  , ArgType::String , "return property name data");
  addArg(argv, "-hidden", ArgType::Boolean, "include hidden data").setHidden(true);
}

QStringList
CQChartsGetChartsPropertyCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "model"     ) return cmds()->modelArgValues();
  else if (arg == "view"      ) return cmds()->viewArgValues();
  else if (arg == "plot"      ) return cmds()->plotArgValues(nullptr);
  else if (arg == "annotation") return cmds()->annotationArgValues(nullptr, nullptr);

  return QStringList();
}

bool
CQChartsGetChartsPropertyCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsGetChartsPropertyCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto objToType = [](QObject *obj) {
    assert(obj);

    if (dynamic_cast<CQChartsView       *>(obj)) return QString("view");
    if (dynamic_cast<CQChartsPlot       *>(obj)) return QString("plot");
    if (dynamic_cast<CQChartsAnnotation *>(obj)) return QString("annotation");
    if (dynamic_cast<CQChartsTitle      *>(obj)) return QString("title");
    if (dynamic_cast<CQChartsAxis       *>(obj)) return QString("axis");
    if (dynamic_cast<CQChartsKey        *>(obj)) return QString("key");
    if (dynamic_cast<CQChartsArrow      *>(obj)) return QString("arrow");
    if (dynamic_cast<CQChartsDataLabel  *>(obj)) return QString("data_label");
    if (dynamic_cast<CQChartsTextBoxObj *>(obj)) return QString("text_box");
    if (dynamic_cast<CQChartsBoxObj     *>(obj)) return QString("box");
    if (dynamic_cast<CQChartsPlotObj    *>(obj)) return QString("plot_obj");

    assert(false);

    return QString();
  };

  //---

  auto objectId = argv.getParseStr ("object"); // plot object
  auto name     = argv.getParseStr ("name");
  bool hidden   = argv.getParseBool("hidden");

  // get view property
  if      (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    auto *view = cmds()->getViewByName(viewName);
    if (! view) return errorMsg("Invalid view '" + viewName + "'");

    if      (name == "?") {
      QStringList names;

      view->getPropertyNames(names, hidden);

      return cmdBase_->setCmdRc(names);
    }
    else if (argv.hasParseArg("data")) {
      auto data = argv.getParseStr("data");

      if      (data == "desc") {
        QString desc;

        if (! view->getPropertyDesc(name, desc, /*hidden*/true))
          return errorMsg("Failed to get view parameter description '" + name + "'");

        return cmdBase_->setCmdRc(desc);
      }
      else if (data == "type") {
        QString type;

        if (! view->getPropertyType(name, type, /*hidden*/true))
          return errorMsg("Failed to get view parameter type '" + name + "'");

        return cmdBase_->setCmdRc(type);
      }
      else if (data == "user_type") {
        QString type;

        if (! view->getPropertyUserType(name, type, /*hidden*/true))
          return errorMsg("Failed to get view parameter user type '" + name + "'");

        return cmdBase_->setCmdRc(type);
      }
      else if (data == "owner") {
        QObject *obj = nullptr;

        if (! view->getPropertyObject(name, obj, /*hidden*/true))
          return errorMsg("Failed to get view parameter owner '" + name + "'");

        return cmdBase_->setCmdRc(objToType(obj));
      }
      else if (data == "is_hidden") {
        bool hidden = false;

        if (! view->getPropertyIsHidden(name, hidden))
          return errorMsg("Failed to get view parameter hidden '" + name + "'");

        return cmdBase_->setCmdRc(hidden);
      }
      else if (data == "is_style") {
        bool is_style = false;

        if (! view->getPropertyIsStyle(name, is_style))
          return errorMsg("Failed to get plot parameter is_style '" + name + "'");

        return cmdBase_->setCmdRc(is_style);
      }
      else if (data == "?") {
        static auto names = QStringList() <<
          "desc" << "type" << "user_type" << "owner" << "is_hidden" << "is_style";
        return cmdBase_->setCmdRc(names);
      }
      else
        return errorMsg("Invalid view property name data '" + data + "'");
    }
    else {
      QVariant value;

      if (! view->getTclProperty(name, value))
        return errorMsg("Failed to get view parameter '" + name + "'");

      return cmdBase_->setCmdRc(value);
    }
  }
  // get plot property
  else if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    auto *plot = cmds()->getPlotByName(nullptr, plotName);
    if (! plot) return errorMsg("Invalid plot '" + plotName + "'");

    CQChartsPlotObj *plotObj = nullptr;
    CQChartsObj     *obj     = nullptr;

    if (objectId.length()) {
      plotObj = plot->getPlotObject(objectId);

      if (! plotObj) {
        auto *key   = plot->key();
        auto *xaxis = plot->xAxis();
        auto *yaxis = plot->yAxis();
        auto *title = plot->title();

        if      (key && key->id() == objectId)
          obj = key;
        else if (xaxis && xaxis->id() == objectId)
          obj = xaxis;
        else if (yaxis && yaxis->id() == objectId)
          obj = yaxis;
        else if (title && title->id() == objectId)
          obj = title;
      }
      else
        obj = plotObj;

      if (! obj)
        return errorMsg("Invalid object id '" + objectId + "'");
    }

    // plot object property
    if (plotObj) {
      if (name == "?") {
        QStringList names;

        plot->getObjectPropertyNames(plotObj, names);

        return cmdBase_->setCmdRc(names);
      }
      else {
        QVariant value;

        if (! plotObj->getProperty(name, value))
          return errorMsg("Failed to get plot object property '" + name + "'");

        bool rc;

        return cmdBase_->setCmdRc(CQChartsVariant::toString(value, rc));
      }
    }
    // object property
    else if (obj) {
      if (name == "?") {
        auto names = CQUtil::getPropertyList(obj, /*inherited*/true);

        return cmdBase_->setCmdRc(names);
      }
      else {
        QVariant value;

        if (! CQUtil::getTclProperty(obj, name, value))
          return errorMsg("Failed to get object property '" + name + "'");

        bool rc;

        return cmdBase_->setCmdRc(CQChartsVariant::toString(value, rc));
      }
    }
    // plot property
    else {
      if      (name == "?") {
        QStringList names;

        plot->getPropertyNames(names, hidden);

        return cmdBase_->setCmdRc(names);
      }
      else if (argv.hasParseArg("data")) {
        auto data = argv.getParseStr("data");

        if      (data == "desc") {
          QString desc;

          if (! plot->getPropertyDesc(name, desc, /*hidden*/true))
            return errorMsg("Failed to get plot parameter description '" + name + "'");

          return cmdBase_->setCmdRc(desc);
        }
        else if (data == "type") {
          QString type;

          if (! plot->getPropertyType(name, type, /*hidden*/true))
            return errorMsg("Failed to get plot parameter type '" + name + "'");

          return cmdBase_->setCmdRc(type);
        }
        else if (data == "user_type") {
          QString type;

          if (! plot->getPropertyUserType(name, type, /*hidden*/true))
            return errorMsg("Failed to get plot parameter user type '" + name + "'");

          return cmdBase_->setCmdRc(type);
        }
        else if (data == "owner") {
          QObject *obj = nullptr;

          if (! plot->getPropertyObject(name, obj, /*hidden*/true))
            return errorMsg("Failed to get plot parameter owner '" + name + "'");

          return cmdBase_->setCmdRc(objToType(obj));
        }
        else if (data == "is_hidden") {
          bool hidden = false;

          if (! plot->getPropertyIsHidden(name, hidden))
            return errorMsg("Failed to get plot parameter is_hidden '" + name + "'");

          return cmdBase_->setCmdRc(hidden);
        }
        else if (data == "is_style") {
          bool is_style = false;

          if (! plot->getPropertyIsStyle(name, is_style))
            return errorMsg("Failed to get plot parameter is_style '" + name + "'");

          return cmdBase_->setCmdRc(is_style);
        }
        else if (data == "?") {
          static auto names = QStringList() <<
            "desc" << "type" << "user_type" << "owner" << "is_hidden" << "is_style";
          return cmdBase_->setCmdRc(names);
        }
        else
          return errorMsg("Invalid plot property name data '" + data + "'");
      }
      else {
        QVariant value;

        if (! plot->getTclProperty(name, value))
          return errorMsg("Failed to get plot parameter '" + name + "'");

        return cmdBase_->setCmdRc(value);
      }
    }
  }
  // get annotation property
  else if (argv.hasParseArg("annotation")) {
    auto annotationName = argv.getParseStr("annotation");

    auto *annotation = cmds()->getAnnotationByName(annotationName);
    if (! annotation) return false;

    if      (name == "?") {
      QStringList names;

      annotation->getPropertyNames(names, hidden);

      return cmdBase_->setCmdRc(names);
    }
    else if (argv.hasParseArg("data")) {
      auto data = argv.getParseStr("data");

      if      (data == "desc") {
        QString desc;

        if (! annotation->getPropertyDesc(name, desc, /*hidden*/true))
          return errorMsg("Failed to get annotation parameter description '" + name + "'");

        return cmdBase_->setCmdRc(desc);
      }
      else if (data == "type") {
        QString type;

        if (! annotation->getPropertyType(name, type, /*hidden*/true))
          return errorMsg("Failed to get annotation parameter type '" + name + "'");

        return cmdBase_->setCmdRc(type);
      }
      else if (data == "user_type") {
        QString type;

        if (! annotation->getPropertyUserType(name, type, /*hidden*/true))
          return errorMsg("Failed to get annotation parameter user type '" + name + "'");

        return cmdBase_->setCmdRc(type);
      }
      else if (data == "owner") {
        QObject *obj = nullptr;

        // view or plot
        if (! annotation->getPropertyObject(name, obj, /*hidden*/true))
          return errorMsg("Failed to get annotation parameter owner '" + name + "'");

        return cmdBase_->setCmdRc(objToType(obj));
      }
      else if (data == "is_hidden") {
        bool hidden = false;

        if (! annotation->getPropertyIsHidden(name, hidden))
          return errorMsg("Failed to get annotation parameter hidden '" + name + "'");

        return cmdBase_->setCmdRc(hidden);
      }
      else if (data == "is_style") {
        bool is_style = false;

        if (! annotation->getPropertyIsStyle(name, is_style))
          return errorMsg("Failed to get plot parameter is_style '" + name + "'");

        return cmdBase_->setCmdRc(is_style);
      }
      else if (data == "?") {
        static auto names = QStringList() <<
          "desc" << "type" << "user_type" << "owner" << "is_hidden" << "is_style";
        return cmdBase_->setCmdRc(names);
      }
      else
        return errorMsg("Invalid annotation property name data '" + data + "'");
    }
    else {
      QVariant value;

      if (! annotation->getTclProperty(name, value))
        return errorMsg("Failed to get annotation property '" + name + "'");

      return cmdBase_->setCmdRc(CQChartsVariant::toString(value, rc));
      //return cmdBase_->setCmdRc(value);
    }
  }
  // get model property
  else if (argv.hasParseArg("model")) {
    auto modelId = argv.getParseStr("model");

    auto *modelData = cmds()->getModelDataOrCurrent(modelId);
    if (! modelData) return errorMsg("No model data for '" + modelId + "'");

    if (name == "?") {
      QStringList names;

      modelData->getPropertyNames(names);

      return cmdBase_->setCmdRc(names);
    }
    else {
      QVariant value;

      if (! modelData->getPropertyData(name, value))
        return errorMsg("Failed to get model property '" + name + "'");

      return cmdBase_->setCmdRc(value);
    }
  }
  // get key item property
  else if (argv.hasParseArg("key_item")) {
    auto keyItemId = argv.getParseStr("key_item");

    auto *keyItem = cmds()->getKeyItemById(keyItemId);
    if (! keyItem) return false;

    QVariant value;

    if (! CQUtil::getProperty(keyItem, name, value))
      return errorMsg("Failed to get key item property '" + name + "'");

    return cmdBase_->setCmdRc(value);
  }

  return true;
}

//------

// set_charts_property command

void
CQChartsSetChartsPropertyCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-model"     , ArgType::String, "model_id");
  addArg(argv, "-view"      , ArgType::String, "view name");
  addArg(argv, "-plot"      , ArgType::String, "plot name");
  addArg(argv, "-annotation", ArgType::String, "annotation name");
  addArg(argv, "-key_item"  , ArgType::String, "key item name");
  argv.endCmdGroup();

  addArg(argv, "-object", ArgType::String , "object id");
  addArg(argv, "-name"  , ArgType::String , "property name");
  addArg(argv, "-value" , ArgType::String , "property view");
//addArg(argv, "-hidden", ArgType::Boolean, "include hidden data").setHidden(true);
}

QStringList
CQChartsSetChartsPropertyCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "model"     ) return cmds()->modelArgValues();
  else if (arg == "view"      ) return cmds()->viewArgValues();
  else if (arg == "plot"      ) return cmds()->plotArgValues(nullptr);
  else if (arg == "annotation") return cmds()->annotationArgValues(nullptr, nullptr);

  return QStringList();
}

bool
CQChartsSetChartsPropertyCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsSetChartsPropertyCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto objectId = argv.getParseStr ("object"); // plot object
  auto name     = argv.getParseStr ("name");
  auto value    = argv.getParseStr ("value");
//bool hidden   = argv.getParseBool("hidden");

  // set view property
  if      (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    auto *view = cmds()->getViewByName(viewName);
    if (! view) return errorMsg("Invalid view '" + viewName + "'");

    if (! view->setProperty(name, value))
      return errorMsg("Failed to set view property '" + name + "' '" + value + "'");
  }
  // set plot property
  else if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    auto *plot = cmds()->getPlotByName(nullptr, plotName);
    if (! plot) return errorMsg("Invalid plot '" + plotName + "'");

    CQChartsPlotObj *plotObj = nullptr;
    CQChartsObj     *obj     = nullptr;

    if (objectId.length()) {
      plotObj = plot->getPlotObject(objectId);

      if (! plotObj) {
        auto *key   = plot->key();
        auto *xaxis = plot->xAxis();
        auto *yaxis = plot->yAxis();
        auto *title = plot->title();

        if      (key && key->id() == objectId)
          obj = key;
        else if (xaxis && xaxis->id() == objectId)
          obj = xaxis;
        else if (yaxis && yaxis->id() == objectId)
          obj = yaxis;
        else if (title && title->id() == objectId)
          obj = title;
      }
      else
        obj = plotObj;

      if (! obj)
        return errorMsg("Invalid object id '" + objectId + "'");
    }

    const auto &plotModel = plot->currentModel();

    CQChartsModelUtil::setContext(plotModel.data());

    CQChartsColumn::setStringToColumnProc(CQChartsModelUtil::stringToColumnProc);

    // plot object property
    if (plotObj) {
      if (! CQUtil::setProperty(plotObj, name, value))
        rc = errorMsg("Failed to set plot object property '" + name + "' '" + value + "'");
    }
    else {
      if (! plot->setProperty(name, value))
        rc = errorMsg("Failed to set plot property '" + name + "' '" + value + "'");
    }

    CQChartsModelUtil::setContext(nullptr);

    CQChartsColumn::setStringToColumnProc(nullptr);

    if (! rc)
      return false;
  }
  // set annotation property
  else if (argv.hasParseArg("annotation")) {
    auto annotationName = argv.getParseStr("annotation");

    auto *annotation = cmds()->getAnnotationByName(annotationName);
    if (! annotation) return false;

    if (! annotation->setProperty(name, value))
      return errorMsg("Failed to set annotation property '" + name + "' '" + value + "'");
  }
  // set model property
  else if (argv.hasParseArg("model")) {
    auto modelId = argv.getParseStr("model");

    auto *modelData = cmds()->getModelDataOrCurrent(modelId);
    if (! modelData) return errorMsg("No model data for '" + modelId + "'");

    if (! modelData->setPropertyData(name, value))
      return errorMsg("Failed to set model property '" + name + "' '" + value + "'");
  }
  // set key item property
  else if (argv.hasParseArg("key_item")) {
    auto keyItemId = argv.getParseStr("key_item");

    auto *keyItem = cmds()->getKeyItemById(keyItemId);
    if (! keyItem) return false;

    if (! CQUtil::setProperty(keyItem, name, value))
      return errorMsg("Failed to set key item property '" + name + "' '" + value + "'");
  }

  return true;
}

//------

// create_charts_palette command

void
CQChartsCreateChartsPaletteCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-theme"  , ArgType::String , "new theme name");
  addArg(argv, "-palette", ArgType::String , "new named name");
  argv.endCmdGroup();
}

QStringList
CQChartsCreateChartsPaletteCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsCreateChartsPaletteCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPaletteCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool themeFlag   = argv.hasParseArg("theme"  );
  auto themeStr    = argv.getParseStr("theme"  );
  bool paletteFlag = argv.hasParseArg("palette");
  auto paletteStr  = argv.getParseStr("palette");

  if      (themeFlag) {
    if (CQColorsMgrInst->getNamedTheme(themeStr))
      return errorMsg(QString("Theme %1 already exists").arg(themeStr));

    (void) CQColorsMgrInst->createTheme(themeStr);

    return cmdBase_->setCmdRc(themeStr);
  }
  else if (paletteFlag) {
    if (CQColorsMgrInst->getNamedPalette(paletteStr))
      return errorMsg(QString("Palette %1 already exists").arg(paletteStr));

    (void) CQColorsMgrInst->createPalette(paletteStr);

    return cmdBase_->setCmdRc(paletteStr);
  }

  return true;
}

//------

// get_charts_palette command

void
CQChartsGetChartsPaletteCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-theme"    , ArgType::String , "get theme data");
  addArg(argv, "-palette"  , ArgType::String , "get named palette data");
  addArg(argv, "-interface", ArgType::Boolean, "get interface data");
  argv.endCmdGroup();

  addArg(argv, "-name", ArgType::String, "value name").setRequired();
  addArg(argv, "-data", ArgType::String, "value name data");
}

QStringList
CQChartsGetChartsPaletteCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsGetChartsPaletteCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsGetChartsPaletteCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool themeFlag     = argv.hasParseArg ("theme"    );
  auto themeStr      = argv.getParseStr ("theme"    );
  bool paletteFlag   = argv.hasParseArg ("palette"  );
  auto paletteStr    = argv.getParseStr ("palette"  );
  bool interfaceFlag = argv.getParseBool("interface");

  auto nameStr = argv.getParseStr("name");

  bool dataFlag = argv.hasParseArg("data");
  auto dataStr  = argv.getParseStr("data");

  //---

  // get global data
  if      (! themeFlag && ! paletteFlag && ! interfaceFlag) {
    if      (nameStr == "palettes") {
      QStringList names;

      CQColorsMgrInst->getPaletteNames(names);

      return cmdBase_->setCmdRc(names);
    }
    else if (nameStr == "themes") {
      QStringList names;

      CQColorsMgrInst->getThemeNames(names);

      return cmdBase_->setCmdRc(names);
    }
    else if (nameStr == "color_models") {
      int n = CQColorsPalette::numModels();

      QStringList names;

      for (int i = 0; i < n; ++i)
        names << CQColorsPalette::modelName(i).c_str();

      return cmdBase_->setCmdRc(names);
    }
    else if (nameStr == "?") {
      static auto names = QStringList() << "palettes" << "themes" << "color_models";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid theme value name '%1'").arg(nameStr));
  }
  // get theme data
  else if (themeFlag) {
    auto *theme = CQColorsMgrInst->getNamedTheme(themeStr);
    if (! theme) return errorMsg(QString("Invalid theme '%1'").arg(themeStr));

    if      (nameStr == "name") {
      return cmdBase_->setCmdRc(theme->name());
    }
    else if (nameStr == "desc") {
      return cmdBase_->setCmdRc(theme->desc());
    }
    else if (nameStr == "palettes") {
      int n = theme->numPalettes();

      QStringList names;

      for (int i = 0; i < n; ++i)
        names << theme->palette(i)->name();

      return cmdBase_->setCmdRc(names);
    }

#if 0
    else if (nameStr == "select_color") {
      return cmdBase_->setCmdRc(theme->selectColor());
    }
    else if (nameStr == "inside_color") {
      return cmdBase_->setCmdRc(theme->insideColor());
    }
#endif

    else if (nameStr == "?") {
#if 0
      static auto names = QStringList() <<
        "name" << "desc" << "palettes" << "select_color" << "inside_color";
#else
      static auto names = QStringList() << "name" << "desc" << "palettes";
#endif
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid theme value name '%1'").arg(nameStr));
  }
  // get palette data
  else if (paletteFlag) {
    auto *palette = CQColorsMgrInst->getNamedPalette(paletteStr);
    if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(themeStr));

    if      (nameStr == "name") {
      return cmdBase_->setCmdRc(palette->name());
    }
    else if (nameStr == "desc") {
      return cmdBase_->setCmdRc(palette->desc());
    }

    else if (nameStr == "color_type") {
      return cmdBase_->setCmdRc(CQColorsPalette::colorTypeToString(palette->colorType()));
    }
    else if (nameStr == "color_model") {
      return cmdBase_->setCmdRc(CQColorsPalette::colorModelToString(palette->colorModel()));
    }

    // model
    else if (nameStr == "red_model"     ) { return cmdBase_->setCmdRc(palette->redModel       ()); }
    else if (nameStr == "green_model"   ) { return cmdBase_->setCmdRc(palette->greenModel     ()); }
    else if (nameStr == "blue_model"    ) { return cmdBase_->setCmdRc(palette->blueModel      ()); }
    else if (nameStr == "gray"          ) { return cmdBase_->setCmdRc(palette->isGray         ()); }
    else if (nameStr == "red_negative"  ) { return cmdBase_->setCmdRc(palette->isRedNegative  ()); }
    else if (nameStr == "green_negative") { return cmdBase_->setCmdRc(palette->isGreenNegative()); }
    else if (nameStr == "blue_negative" ) { return cmdBase_->setCmdRc(palette->isBlueNegative ()); }
    else if (nameStr == "red_min"       ) { return cmdBase_->setCmdRc(palette->redMin         ()); }
    else if (nameStr == "red_max"       ) { return cmdBase_->setCmdRc(palette->redMax         ()); }
    else if (nameStr == "green_min"     ) { return cmdBase_->setCmdRc(palette->greenMin       ()); }
    else if (nameStr == "green_max"     ) { return cmdBase_->setCmdRc(palette->greenMax       ()); }
    else if (nameStr == "blue_min"      ) { return cmdBase_->setCmdRc(palette->blueMin        ()); }
    else if (nameStr == "blue_max"      ) { return cmdBase_->setCmdRc(palette->blueMax        ()); }

    // defined colors
    else if (nameStr == "defined_colors") {
      QVariantList vars;

      for (const auto &rc : palette->definedValueColors()) {
        double      r = rc.first;
        const auto &c = rc.second;

        QStringList strs;

        strs << QString::number(r) << c.name();

        vars << strs;
      }

      return cmdBase_->setCmdRc(vars);
    }
    else if (nameStr == "color") {
      if (! dataFlag) return errorMsg("Missing data for palette color");

      bool ok;

      long i = CQChartsUtil::toInt(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid color index '%1'").arg(dataStr));

      int n = palette->numDefinedColors();

      if (i < 0 || i >= n) return errorMsg(QString("Invalid color index '%1'").arg(dataStr));

      return cmdBase_->setCmdRc(palette->definedColor(int(i)));
    }

    else if (nameStr == "distinct") {
      return cmdBase_->setCmdRc(palette->isDistinct());
     }

    else if (nameStr == "interp_color") {
      if (! dataFlag) return errorMsg("Missing data for palette interp color");

      bool ok;

      double r = CQChartsUtil::toReal(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid interp color value '%1'").arg(dataStr));

      bool scale = false;

      auto c = palette->getColor(r, scale);

      return cmdBase_->setCmdRc(c);
    }

    else if (nameStr == "red_function"  ) { return cmdBase_->setCmdRc(palette->redFunction  ()); }
    else if (nameStr == "green_function") { return cmdBase_->setCmdRc(palette->greenFunction()); }
    else if (nameStr == "blue_function" ) { return cmdBase_->setCmdRc(palette->blueFunction ()); }

    else if (nameStr == "cube_start"     ) { return cmdBase_->setCmdRc(palette->cbStart       ()); }
    else if (nameStr == "cube_cycles"    ) { return cmdBase_->setCmdRc(palette->cbCycles      ()); }
    else if (nameStr == "cube_saturation") { return cmdBase_->setCmdRc(palette->cbSaturation  ()); }
    else if (nameStr == "cube_negative"  ) { return cmdBase_->setCmdRc(palette->isCubeNegative()); }

    else if (nameStr == "?") {
      static auto names = QStringList() <<
        "name" << "desc" << "color_type" << "color_model" << "red_model" << "green_model" <<
        "blue_model" << "gray" << "red_negative" << "green_negative" << "blue_negative" <<
        "red_min" << "red_max" << "green_min" << "green_max" << "blue_min" << "blue_max" <<
        "defined_colors" << "color" << "distinct" << "interp_color" << "red_function" <<
        "green_function" << "blue_function" << "cube_start" << "cube_cycles" <<
        "cube_saturation" << "cube_negative";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid palette value name '%1'").arg(nameStr));
  }
  // get interface data
  else if (interfaceFlag) {
    auto *interface = charts()->interfaceTheme();
    assert(interface);

    if      (nameStr == "is_dark") {
      return cmdBase_->setCmdRc(interface->isDark());
    }
    else if (nameStr == "interp_color") {
      if (! dataFlag) return errorMsg("Missing data for interface interp color");

      bool ok;

      double r = CQChartsUtil::toReal(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid interp color value '%1'").arg(dataStr));

      bool scale = false;

      auto c = interface->interpColor(r, scale);

      return cmdBase_->setCmdRc(c);
    }
    else if (nameStr == "?") {
      static auto names = QStringList() << "is_dark" << "interp_color";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid interface value name '%1'").arg(nameStr));
  }

  return true;
}

//------

// set_charts_palette command

void
CQChartsSetChartsPaletteCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-theme"    , ArgType::String , "get theme data");
  addArg(argv, "-palette"  , ArgType::String , "get named palette data");
  addArg(argv, "-interface", ArgType::Boolean, "get interface data");
  argv.endCmdGroup();

  addArg(argv, "-name" , ArgType::String, "value name").setRequired();
  addArg(argv, "-value", ArgType::String, "value").setRequired();
  addArg(argv, "-data" , ArgType::String, "value name data");
}

QStringList
CQChartsSetChartsPaletteCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsSetChartsPaletteCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsSetChartsPaletteCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool themeFlag     = argv.hasParseArg ("theme"    );
  auto themeStr      = argv.getParseStr ("theme"    );
  bool paletteFlag   = argv.hasParseArg ("palette"  );
  auto paletteStr    = argv.getParseStr ("palette"  );
  bool interfaceFlag = argv.getParseBool("interface");

  auto nameStr  = argv.getParseStr("name" );
  auto valueStr = argv.getParseStr("value");

//bool dataFlag = argv.hasParseArg("data");
  auto dataStr  = argv.getParseStr("data");

  //---

  // set global data
  if      (! themeFlag && ! paletteFlag && ! interfaceFlag) {
    if (nameStr == "?") {
      QStringList names;

      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid value name '%1'").arg(nameStr));
  }
  // set theme data
  else if (themeFlag) {
    auto *theme = CQColorsMgrInst->getNamedTheme(themeStr);
    if (! theme) return errorMsg(QString("Invalid theme '%1'").arg(themeStr));

    if      (nameStr == "name") theme->setName(valueStr);
    else if (nameStr == "desc") theme->setDesc(valueStr);

#if 0
    else if (nameStr == "select_color") { theme->setSelectColor(QColor(valueStr)); }
    else if (nameStr == "inside_color") { theme->setInsideColor(QColor(valueStr)); }
#endif

    else if (nameStr == "palettes") {
      // split into palette names
      QStringList strs;

      CQTcl::splitList(valueStr, strs);

      for (int i = 0; i < strs.length(); ++i) {
        auto *palette = CQColorsMgrInst->getNamedPalette(strs[i]);
        if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(strs[i]));
      }

      theme->setNamedPalettes(strs);
    }
    else if (nameStr == "add_palette") {
      auto *palette = CQColorsMgrInst->getNamedPalette(valueStr);
      if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(valueStr));

      theme->addNamedPalette(valueStr);
    }
    else if (nameStr == "remove_palette") {
      auto *palette = CQColorsMgrInst->getNamedPalette(valueStr);
      if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(valueStr));

      theme->removeNamedPalette(valueStr);
    }
    else if (nameStr == "set_palette") {
      auto *palette = CQColorsMgrInst->getNamedPalette(valueStr);
      if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(valueStr));

      bool ok;
      long pos = CQChartsUtil::toInt(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid position '%1'").arg(dataStr));

      theme->setNamedPalette(int(pos), valueStr);
    }
    else if (nameStr == "move_palette") {
      auto *palette = CQColorsMgrInst->getNamedPalette(valueStr);
      if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(valueStr));

      bool ok;
      long pos = CQChartsUtil::toInt(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid position '%1'").arg(dataStr));

      theme->moveNamedPalette(valueStr, int(pos));
    }
    else if (nameStr == "?") {
      static auto names = QStringList() << "name" << "desc" << "palettes" <<
#if 0
        "select_color" << "inside_color" <<
#endif
        "add_palette" << "remove_palette" << "set_palette" << "move_palette";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid theme value name '%1'").arg(nameStr));
  }
  // set palette data
  else if (paletteFlag) {
    auto *palette = CQColorsMgrInst->getNamedPalette(paletteStr);
    if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(themeStr));

    if      (nameStr == "name") palette->setName(valueStr);
    else if (nameStr == "desc") palette->setDesc(valueStr);

    else if (nameStr == "color_type") {
      palette->setColorType(CQColorsPalette::stringToColorType(valueStr));
    }
    else if (nameStr == "color_model") {
      palette->setColorModel(CQColorsPalette::stringToColorModel(valueStr));
    }

    // model
    else if (nameStr == "red_model" || nameStr == "green_model" || nameStr == "blue_model") {
      bool ok;
      long i = CQChartsUtil::toInt(valueStr, ok);
      if (! ok) return errorMsg(QString("Invalid model index '%1'").arg(valueStr));

      if      (nameStr == "red_model"  ) palette->setRedModel  (int(i));
      else if (nameStr == "green_model") palette->setGreenModel(int(i));
      else if (nameStr == "blue_model" ) palette->setBlueModel (int(i));
    }

    else if (nameStr == "gray" || nameStr == "red_negative" ||
             nameStr == "green_negative" || nameStr == "blue_negative") {
      bool ok;
      bool b = CQChartsUtil::stringToBool(valueStr, &ok);
      if (! ok) return errorMsg(QString("Invalid boolean '%1'").arg(valueStr));

      if      (nameStr == "gray"          ) palette->setGray         (b);
      else if (nameStr == "red_negative"  ) palette->setRedNegative  (b);
      else if (nameStr == "green_negative") palette->setGreenNegative(b);
      else if (nameStr == "blue_negative" ) palette->setBlueNegative (b);
    }

    else if (nameStr == "red_min"   || nameStr == "red_max"   ||
             nameStr == "green_min" || nameStr == "green_max" ||
             nameStr == "blue_min"  || nameStr == "blue_max") {
      bool ok;
      double r = CQChartsUtil::toReal(valueStr, ok);
      if (! ok) return errorMsg(QString("Invalid real '%1'").arg(valueStr));

      if      (nameStr == "red_min"  ) palette->setRedMin  (r);
      else if (nameStr == "red_max"  ) palette->setRedMax  (r);
      else if (nameStr == "green_min") palette->setGreenMin(r);
      else if (nameStr == "green_max") palette->setGreenMax(r);
      else if (nameStr == "blue_min" ) palette->setBlueMin (r);
      else if (nameStr == "blue_max" ) palette->setBlueMax (r);
    }

    else if (nameStr == "distinct") {
      bool ok;

      bool b = CQChartsUtil::stringToBool(valueStr, &ok);
      if (! ok) return errorMsg(QString("Invalid boolean '%1'").arg(valueStr));

      palette->setDistinct(b);
    }

    else if (nameStr == "red_function"  ) { palette->setRedFunction  (valueStr.toStdString()); }
    else if (nameStr == "green_function") { palette->setGreenFunction(valueStr.toStdString()); }
    else if (nameStr == "blue_function" ) { palette->setBlueFunction (valueStr.toStdString()); }

    else if (nameStr == "cube_start" || nameStr == "cube_cycles" || nameStr == "cube_saturation") {
      bool ok;

      double r = CQChartsUtil::toReal(valueStr, ok);
      if (! ok) return errorMsg(QString("Invalid real '%1'").arg(valueStr));

      if      (nameStr == "cube_start"     ) { palette->setCbStart     (r); }
      else if (nameStr == "cube_cycles"    ) { palette->setCbCycles    (r); }
      else if (nameStr == "cube_saturation") { palette->setCbSaturation(r); }
    }

    else if (nameStr == "cube_negative") {
      bool ok;

      bool b = CQChartsUtil::stringToBool(valueStr, &ok);
      if (! ok) return errorMsg(QString("Invalid boolean '%1'").arg(valueStr));

      palette->setCubeNegative(b);
    }

    // set colors
    else if (nameStr == "defined_colors") {
      using DefinedColors = CQColorsPalette::DefinedColors;

      DefinedColors definedColors;

      //--

      // split into color names
      QStringList strs;

      CQTcl::splitList(valueStr, strs);

      if (! strs.length()) return errorMsg(QString("Invalid defined colors '%1'").arg(valueStr));

      double dv = (strs.length() > 1 ? 1.0/(strs.length() - 1) : 0.0);

      for (int j = 0; j < strs.length(); ++j) {
        double v = j*dv;
        QColor c;

        // split color name/value
        QStringList strs1;

        if (! CQTcl::splitList(strs[j], strs1))
          return errorMsg(QString("Invalid defined color string '%1'").arg(strs[j]));

        if (strs1.length() != 1 && strs1.length() != 2)
          return errorMsg(QString("Invalid defined color string '%1'").arg(strs[j]));

        QString lhs, rhs;
        bool    isPair = true;

        if (strs1.length() == 2) {
          lhs = strs1[0];
          rhs = strs1[1];
        }
        else {
          lhs    = strs1[0];
          isPair = false;
        }

        if (isPair) {
          bool ok;

          v = CQChartsUtil::toReal(lhs, ok);

          if (! ok) {
            errorMsg(QString("Invalid color value '%1'").arg(lhs));
            continue;
          }

          c = QColor(rhs);

          if (! c.isValid()) {
            errorMsg(QString("Invalid color '%1'").arg(rhs));
            continue;
          }
        }
        else {
          c = QColor(lhs);

          if (! c.isValid()) {
            errorMsg(QString("Invalid color '%1'").arg(lhs));
            continue;
          }
        }

        definedColors.emplace_back(v, c);
      }

      if (! definedColors.empty())
        palette->setDefinedColors(definedColors);
    }

    else if (nameStr == "?") {
      static auto names = QStringList() <<
        "name" << "desc" << "color_type" << "color_model" <<
        "red_model" << "green_model" << "blue_model" <<
        "red_min" << "red_max" << "green_min" << "green_max" << "blue_min" << "blue_max" <<
        "distinct" << "red_function" << "green_function" << "blue_function" <<
        "cube_start" << "cube_cycles" << "cube_saturation" << "cube_negative" <<
        "defined_colors";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid palette value name '%1'").arg(nameStr));
  }
  // set interface data
  else if (interfaceFlag) {
    auto *interface = charts()->interfaceTheme();
    assert(interface);

    if      (nameStr == "dark") {
      bool ok;

      bool b = CQChartsUtil::stringToBool(valueStr, &ok);
      if (! ok) return errorMsg(QString("Invalid boolean '%1'").arg(valueStr));

      interface->setDark(b);
    }

    else if (nameStr == "?") {
      static auto names = QStringList() << "dark";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid interface value name '%1'").arg(nameStr));
  }

  //---

#if 0
  view->updatePlots();

  auto *window = CQChartsWindowMgrInst->getWindowForView(view);

  if (window) {
    if (interface)
      window->updateInterfacePalette();
    else
      window->updateThemePalettes();
  }
#endif

  return true;
}

//------

// create_charts_symbol_set command

void
CQChartsCreateChartsSymbolSetCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-name", ArgType::String , "symbol set name").setRequired();
}

QStringList
CQChartsCreateChartsSymbolSetCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsCreateChartsSymbolSetCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsSymbolSetCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto nameStr = argv.getParseStr("name");

  if (charts()->hasSymbolSet(nameStr))
    return errorMsg(QString("Symbol Set '%1' already exists").arg(nameStr));

  (void) charts()->createSymbolSet(nameStr);

  return true;
}

//------

// add_charts_symbol command

void
CQChartsAddChartsSymbolCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-set", ArgType::String, "symbol set name").setRequired();

  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-symbol", ArgType::String , "symbol name");
  addArg(argv, "-char"  , ArgType::String , "char string");
  addArg(argv, "-path"  , ArgType::String , "path string");
  addArg(argv, "-svg"   , ArgType::String , "svg file");
  argv.endCmdGroup();

  addArg(argv, "-name"  , ArgType::String, "optional char name");
  addArg(argv, "-filled", ArgType::SBool , "is symbol filled");
  addArg(argv, "-styled", ArgType::SBool , "is svg styled");
}

QStringList
CQChartsAddChartsSymbolCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsAddChartsSymbolCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsAddChartsSymbolCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto setStr = argv.getParseStr("set");

  auto *symbolSet = charts()->symbolSetMgr()->symbolSet(setStr);

  if (! symbolSet)
    return errorMsg(QString("No Symbol Set '%1'").arg(setStr));

  //---

  CQChartsSymbol symbol;

  if      (argv.hasParseArg("symbol")) {
    auto symbolStr = argv.getParseStr("symbol");

    symbol = CQChartsSymbol(symbolStr);

    if (! symbol.isValid())
      return errorMsg(QString("Invalid Symbol '%1'").arg(symbolStr));
  }
  else if (argv.hasParseArg("char")) {
    auto charStr = argv.getParseStr("char");
    auto nameStr = argv.getParseStr("name");

    symbol = CQChartsSymbol(CQChartsSymbol::CharData(charStr, nameStr));
  }
  else if (argv.hasParseArg("path")) {
    auto pathStr = argv.getParseStr("path");
    auto nameStr = argv.getParseStr("name");

    CQChartsSymbol::PathData pathData;

    CQChartsPath path(pathStr);

    if (! path.isValid())
      return errorMsg(QString("Invalid Path '%1'").arg(pathStr));

    pathData.path = path;
    pathData.name = nameStr;
    pathData.src  = pathStr;

    symbol = CQChartsSymbol(pathData);
  }
  else if (argv.hasParseArg("svg")) {
    auto svgStr  = argv.getParseStr("svg");
    auto nameStr = argv.getParseStr("name");
    bool styled  = argv.getParseBool("styled");

    CQChartsFile file(charts(), svgStr);

    symbol = CQChartsSymbol::fromSVGFile(file, nameStr, styled);

    if (! symbol.isValid())
      return errorMsg(QString("Invalid SVG File '%1'").arg(svgStr));
  }
  else {
    return false;
  }

  if (argv.hasParseArg("filled"))
    symbol.setFilled(argv.getParseBool("filled"));

  auto ssymbol = symbolSet->addSymbol(symbol);

  return cmdBase_->setCmdRc(ssymbol.toString());
}

//------

// group_charts_plots command

void
CQChartsGroupChartsPlotsCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-view", ArgType::String, "view name").setRequired();

  addArg(argv, "-x1x2"     , ArgType::Boolean, "use shared x range");
  addArg(argv, "-y1y2"     , ArgType::Boolean, "use shared y range");
  addArg(argv, "-overlay"  , ArgType::Boolean, "overlay (shared x and/or y range)");
  addArg(argv, "-tabbed"   , ArgType::Boolean, "tabbed (one shown at a time)");
  addArg(argv, "-composite", ArgType::Boolean, "create composite plot");
  addArg(argv, "-force"    , ArgType::Boolean, "allow place plot on different view");
}

QStringList
CQChartsGroupChartsPlotsCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "view") return cmds()->viewArgValues();

  return QStringList();
}

bool
CQChartsGroupChartsPlotsCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsGroupChartsPlotsCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto viewName  = argv.getParseStr ("view");
  bool x1x2      = argv.getParseBool("x1x2");
  bool y1y2      = argv.getParseBool("y1y2");
  bool overlay   = argv.getParseBool("overlay");
  bool tabbed    = argv.getParseBool("tabbed");
  bool composite = argv.getParseBool("composite");
  bool force     = argv.getParseBool("force");

  const auto &plotNames = argv.getParseArgs();

  //---

  // get view
  auto *view = cmds()->getViewByName(viewName);
  if (! view) return false;

  //---

  CQChartsCmds::Plots plots, extraPlots;

  if (! cmds()->getPlotsByName(view, plotNames, plots, extraPlots, ! force))
    return errorMsg("Failed to get named plots for view");

  //---

  if (force) {
    for (auto *plot : extraPlots) {
      view->movePlot(plot);

      plots.push_back(plot);
    }
  }

  //---

  if (composite) {
    auto *type = charts()->plotType("composite");
    assert(type);

    auto *plot = cmds()->createPlot(view, nullptr, type, true);

    if (! plot)
      return errorMsg("Failed to create plot");

    //---

    double vr = CQChartsView::viewportRange();

    CQChartsGeom::BBox bbox(0, 0, vr, vr);

    view->addPlot(plot, bbox);

    //---

    auto *compositePlot = qobject_cast<CQChartsCompositePlot *>(plot);
    assert(compositePlot);

    for (auto &plot : plots)
      compositePlot->addPlot(plot);

    if      (tabbed) {
      compositePlot->setCompositeType(CQChartsCompositePlot::CompositeType::TABBED);

      compositePlot->setCommonXRange(false);
      compositePlot->setCommonYRange(false);
    }
    else if (x1x2) {
      compositePlot->setCompositeType(CQChartsCompositePlot::CompositeType::X1X2);

      compositePlot->setCommonXRange(false);
    }
    else if (y1y2) {
      compositePlot->setCompositeType(CQChartsCompositePlot::CompositeType::Y1Y2);

      compositePlot->setCommonYRange(false);
    }

    //---

    return cmdBase_->setCmdRc(compositePlot->pathId());
  }
  else {
    if      (x1x2) {
      if (plots.size() < 2)
        return errorMsg("Need 2 (or more) plots for x1x2");

      view->initX1X2(plots, overlay, /*reset*/true);
    }
    else if (y1y2) {
      if (plots.size() < 2)
        return errorMsg("Need 2 (or more) plots for y1y2");

      view->initY1Y2(plots, overlay, /*reset*/true);
    }
    else if (overlay) {
      if (plots.size() < 2)
        return errorMsg("Need 2 or more plots for overlay");

      view->initOverlay(plots, /*reset*/true);
    }
    else if (tabbed) {
      if (plots.size() < 2)
        return errorMsg("Need 2 or more plots for overlay");

      view->initTabbed(plots, /*reset*/true);
    }
    else
      return errorMsg("No grouping specified");
  }

  return true;
}

//------

// place_charts_plots command

void
CQChartsPlaceChartsPlotsCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-view", ArgType::String, "view name").setRequired();

  addArg(argv, "-vertical"  , ArgType::Boolean, "place vertical");
  addArg(argv, "-horizontal", ArgType::Boolean, "place horizontal");
  addArg(argv, "-rows"      , ArgType::Integer, "place using n rows");
  addArg(argv, "-columns"   , ArgType::Integer, "place using n columns");
  addArg(argv, "-force"     , ArgType::Boolean, "allow place plot on different view");
}

QStringList
CQChartsPlaceChartsPlotsCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "view") return cmds()->viewArgValues();

  return QStringList();
}

bool
CQChartsPlaceChartsPlotsCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsPlaceChartsPlotsCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto viewName   = argv.getParseStr ("view");
  bool vertical   = argv.getParseBool("vertical");
  bool horizontal = argv.getParseBool("horizontal");
  int  rows       = argv.getParseInt ("rows"   , -1); // number of rows
  int  columns    = argv.getParseInt ("columns", -1); // number of columns
  bool force      = argv.getParseBool("force");

  const auto &plotNames = argv.getParseArgs();

  //---

  // get view
  auto *view = cmds()->getViewByName(viewName);
  if (! view) return false;

  //---

  CQChartsCmds::Plots plots, extraPlots;

  if (! cmds()->getPlotsByName(view, plotNames, plots, extraPlots, ! force))
    return errorMsg("Failed to get named plots for view");

  //---

  if (force) {
    for (auto *plot : extraPlots) {
      view->movePlot(plot);

      plots.push_back(plot);
    }
  }

  //---

  view->placePlots(plots, vertical, horizontal, rows, columns, /*reset*/true);

  return true;
}

//------

// fold_charts_model command

void
CQChartsFoldChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"    , ArgType::String , "model_id");
  addArg(argv, "-column"   , ArgType::Column , "column to fold");
  addArg(argv, "-separator", ArgType::String , "hier separator char");
  addArg(argv, "-keep"     , ArgType::Boolean, "keep fold column");
}

QStringList
CQChartsFoldChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsFoldChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsFoldChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->model();

  //---

  // get fold column (default to first column)
  int icolumn = 0;

  if (argv.hasParseArg("column")) {
    auto column = argv.getParseColumn("column", model.data());

    icolumn = column.column();
  }

  //---

  // get separator
  QString separator;

  if (argv.hasParseArg("separator"))
    separator = argv.getParseStr("separator");

  //---

  auto *proxyModel = new QSortFilterProxyModel;

  proxyModel->setObjectName("foldProxyModel");

  proxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  if (separator == "") {
    CQFoldData foldData(icolumn);

    if (argv.hasParseArg("keep"))
      foldData.setKeepColumn(true);

    auto *foldedModel = new CQFoldedModel(model.data(), foldData);

    //modelData->copyHeaderRoles(foldedModel);

    proxyModel->setSourceModel(foldedModel);
  }
  else {
    CQHierSepData data(icolumn, separator[0]);

    auto *hierSepModel = new CQHierSepModel(model.data(), data);

    //modelData->copyHeaderRoles(hierSepModel);

    proxyModel->setSourceModel(hierSepModel);
  }

  //---

  CQChartsCmds::ModelP proxyModelP(proxyModel);

  auto *proxyModelData = charts()->initModelData(proxyModelP);

  return cmdBase_->setCmdRc(proxyModelData->id());
}

//------

// connection_charts_model command

void
CQChartsConnectionChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"      , ArgType::String , "model_id");
  addArg(argv, "-column"     , ArgType::Column , "column with node data");
  addArg(argv, "-node"       , ArgType::Column , "column with node id");
  addArg(argv, "-connections", ArgType::Column , "column with connections");
}

QStringList
CQChartsConnectionChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsConnectionChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsConnectionChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->model();

  //---

  // get connection column (default to first column)
  int icolumn = 0;

  if (argv.hasParseArg("column")) {
    auto column = argv.getParseColumn("column", model.data());

    if (column.column() < 0)
      return errorMsg("Invalid connections column");

    icolumn = column.column();
  }

  int iconnections = -1;
  int inode        = -1;

  if (argv.hasParseArg("connections")) {
    auto column = argv.getParseColumn("connections", model.data());

    if (column.column() < 0)
      return errorMsg("Invalid connections column");

    iconnections = column.column();
  }

  if (argv.hasParseArg("node")) {
    auto column = argv.getParseColumn("node", model.data());

    if (column.column() < 0)
      return errorMsg("Invalid node column");

    inode = column.column();
  }

  //---

  auto *proxyModel = new QSortFilterProxyModel;

  proxyModel->setObjectName("foldProxyModel");

  proxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  CQHierSepData data(icolumn);

  if (iconnections >= 0) {
    data.connectionType   = CQHierConnectionType::CONNECTIONS;
    data.connectionColumn = iconnections;
    data.nodeColumn       = inode;
  }
  else {
    data.connectionType = CQHierConnectionType::HIER;
  }

  auto *hierSepModel = new CQHierSepModel(model.data(), data);

  //modelData->copyHeaderRoles(hierSepModel);

  proxyModel->setSourceModel(hierSepModel);

  //---

  CQChartsCmds::ModelP proxyModelP(proxyModel);

  auto *proxyModelData = charts()->initModelData(proxyModelP);

  return cmdBase_->setCmdRc(proxyModelData->id());
}

//------

// flatten_charts_model command

void
CQChartsFlattenChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"       , ArgType::String, "model_id");
  addArg(argv, "-group"       , ArgType::Column, "grouping column id");
  addArg(argv, "-sum_columns" , ArgType::String, "columns to calculate sum");
  addArg(argv, "-mean_columns", ArgType::String, "columns to calculate mean");
  addArg(argv, "-default_op"  , ArgType::String, "default operation");
  addArg(argv, "-column_ops"  , ArgType::String, "operations and columns to use");
}

QStringList
CQChartsFlattenChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsFlattenChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsFlattenChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  auto argStringToColumns = [&](const QString &name, bool &ok) {
    ok = true;

    std::vector<CQChartsColumn> columns;

    auto columnsStrs = argv.getParseStrs(name);

    for (int i = 0; i < columnsStrs.length(); ++i) {
      const auto &columnsStr = columnsStrs[i];

      if (! columnsStr.length())
        continue;

      std::vector<CQChartsColumn> columns1;

      if (! CQChartsModelUtil::stringToColumns(model.data(), columnsStr, columns1)) {
        ok = errorMsg("Bad columns name '" + columnsStr + "'");
        continue;
      }

      for (const auto &column : columns1)
        columns.push_back(column);
    }

    return columns;
  };

  // get sum and mean columns
  bool ok;
  auto sumColumns  = argStringToColumns("sum_columns" , ok); if (! ok) return false;
  auto meanColumns = argStringToColumns("mean_columns", ok); if (! ok) return false;

  //---

  auto defOp = CQChartsModelUtil::FlattenOp::NONE;

  if (argv.hasParseArg("default_op"))
    defOp = CQChartsModelUtil::flattenStringToOp(argv.getParseStr("default_op"));

  //---

  using ColumnOps = CQChartsModelUtil::FlattenData::ColumnOps;

  auto stringToColumnOps = [&](const QString &opStr) {
    ColumnOps columnOps;

    // split column ops
    QStringList strs;

    if (! CQTcl::splitList(opStr, strs))
      return columnOps;

    for (const auto &str : strs) {
      // split op/column
      QStringList strs1;

      if (! CQTcl::splitList(str, strs1))
        continue;

      if (strs1.size() != 2) {
        errorMsg(QString("Invalid column op '%1'").arg(str));
        continue;
      }

      auto flattenOp = CQChartsModelUtil::flattenStringToOp(strs1[0]);

      if (flattenOp == CQChartsModelUtil::FlattenOp::NONE) {
        errorMsg(QString("Invalid column op '%1'").arg(strs1[0]));
        continue;
      }

      CQChartsColumn column;

      if (! CQChartsModelUtil::stringToColumn(model.data(), strs1[1], column)) {
        errorMsg("Bad column '" + strs1[1] + "'");
        continue;
      }

      columnOps.emplace_back(column, flattenOp);
    }

    return columnOps;
  };

  // get sum and mean columns
  ColumnOps columnOps;

  if (argv.hasParseArg("column_ops"))
    columnOps = stringToColumnOps(argv.getParseStr("column_ops"));

  //---

  auto groupColumn = argv.getParseColumn("group", model.data());

  //---

  CQChartsModelUtil::FlattenData flattenData;

  flattenData.groupColumn = groupColumn;
  flattenData.defOp       = defOp;

  for (const auto &c : sumColumns)
    flattenData.columnOpMap[c] = CQChartsModelUtil::FlattenOp::SUM;

  for (const auto &c : meanColumns)
    flattenData.columnOpMap[c] = CQChartsModelUtil::FlattenOp::MEAN;

  flattenData.columnOps = columnOps;

  auto *filterModel = CQChartsModelUtil::flattenModel(charts(), model.data(), flattenData);

  CQChartsCmds::ModelP filterModelP(filterModel);

  auto *filterModelData = charts()->initModelData(filterModelP);

  //---

  return cmdBase_->setCmdRc(filterModelData->id());
}

//------

// copy_charts_model command
//   copy model to new model

void
CQChartsCopyChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  // TODO: columns, allow add row index ?

  addArg(argv, "-model" , ArgType::String , "model_id");
  addArg(argv, "-filter", ArgType::String , "filter expression");
  addArg(argv, "-rows"  , ArgType::Integer, "number of rows");
  addArg(argv, "-debug" , ArgType::Boolean, "debug expression evaulation");
}

QStringList
CQChartsCopyChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCopyChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCopyChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  bool debug = argv.getParseBool("debug");

  //--

  int nr = -1;

  if (argv.hasParseArg("rows"))
    nr = argv.getParseInt("rows", -1);

  //---

  CQChartsModelData::CopyData copyData;

  copyData.filter = argv.getParseStr("filter");
  copyData.nr     = nr;
  copyData.debug  = debug;

  //------

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto newModel = modelData->copy(copyData);

  CQChartsCmds::ModelP newModelP(newModel);

  auto *newModelData = charts()->initModelData(newModelP);

  //---

  return cmdBase_->setCmdRc(newModelData->id());
}

//------

// join_charts_model command
//   Join models to create new model

void
CQChartsJoinChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-models"  , ArgType::String, "model_ids");
  addArg(argv, "-lcolumns", ArgType::String, "lcolumns");
  addArg(argv, "-rcolumns", ArgType::String, "rcolumns");
  addArg(argv, "-type"    , ArgType::String, "type");
  addArg(argv, "-keys"    , ArgType::String, "keys");
  addArg(argv, "-axis"    , ArgType::String, "axis name");
}

QStringList
CQChartsJoinChartsModelCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsJoinChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsJoinChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto modelsStr = argv.getParseStr("models");

  using ModelDatas = std::vector<CQChartsModelData *>;

  ModelDatas modelDatas;

  // split into strings per model
  QStringList modelStrs;

  if (! CQTcl::splitList(modelsStr, modelStrs))
    return errorMsg("Bad model ids '" + modelsStr + "'");

  for (const auto &modelStr : modelStrs) {
    auto *modelData = cmds()->getModelDataOrCurrent(modelStr);
    if (! modelData) return errorMsg("No model data for '" + modelStr + "'");

    modelDatas.push_back(modelData);
  }

  //---

  // get join type
  CQChartsModelData::JoinType joinType = CQChartsModelData::JoinType::NONE;

  if (argv.hasParseArg("type")) {
    auto typeStr = argv.getParseStr("type").toLower();

    if      (typeStr == "left"  ) joinType = CQChartsModelData::JoinType::LEFT;
    else if (typeStr == "right" ) joinType = CQChartsModelData::JoinType::RIGHT;
    else if (typeStr == "outer" ) joinType = CQChartsModelData::JoinType::OUTER;
    else if (typeStr == "inner" ) joinType = CQChartsModelData::JoinType::INNER;
    else if (typeStr == "cross" ) joinType = CQChartsModelData::JoinType::CROSS;
    else if (typeStr == "concat") joinType = CQChartsModelData::JoinType::CONCAT;
  }

  bool needsColumns = ! (joinType == CQChartsModelData::JoinType::CROSS ||
                         joinType == CQChartsModelData::JoinType::CONCAT);

  //---

  if (modelDatas.size() < 2)
    return errorMsg("Need at least two models to join");

  auto model0 = modelDatas[0]->currentModel();
  auto model1 = modelDatas[1]->currentModel();

  //---

  // split left columns string into left model columns
  CQChartsCmds::Columns lcolumns;

  auto lcolumnsStr = argv.getParseStr("lcolumns");

  if (! cmds()->stringToModelColumns(model0, lcolumnsStr, lcolumns))
    return false;

  if (needsColumns && lcolumns.size() < 1)
    return errorMsg("Need one or more column to join");

  //---

  // split right columns string into right model columns
  CQChartsCmds::Columns rcolumns;

  if (argv.hasParseArg("rcolumns")) {
    auto rcolumnsStr = argv.getParseStr("rcolumns");

    if (! cmds()->stringToModelColumns(model1, rcolumnsStr, rcolumns))
      return false;

    if (needsColumns && rcolumns.size() < 1)
      return errorMsg("Need one or more column to join");
  }

  //---

  QStringList keysStrs;

  if (argv.hasParseArg("keys")) {
    auto keysStr = argv.getParseStr("keys");

    if (! CQTcl::splitList(keysStr, keysStrs))
      return errorMsg(QString("Invalid keys strings '%1'").arg(keysStr));
  }

  //---

  auto axis = Qt::Vertical;

  if (argv.hasParseArg("axis")) {
    auto axisStr = argv.getParseStr("axis").toLower();

    if      (axisStr == "v" || axisStr == "vertical")
      axis = Qt::Vertical;
    else if (axisStr == "h" || axisStr == "horizontal")
      axis = Qt::Horizontal;
    else
      return errorMsg(QString("Invalid axis string '%1'").arg(axisStr));
  }

  //---

  QAbstractItemModel *newModel;

  if      (joinType == CQChartsModelData::JoinType::CROSS) {
    newModel = modelDatas[0]->cross(modelDatas[1]);
  }
  else if (joinType == CQChartsModelData::JoinType::CONCAT) {
    newModel = CQChartsModelData::concat(modelDatas, keysStrs, axis);
  }
  else {
    if (! rcolumns.empty())
      newModel = modelDatas[0]->join(modelDatas[1], lcolumns, rcolumns, joinType);
    else
      newModel = modelDatas[0]->join(modelDatas[1], lcolumns, joinType);
  }

  if (! newModel)
    return errorMsg("Join failed");

  CQChartsCmds::ModelP newModelP(newModel);

  auto *newModelData = charts()->initModelData(newModelP);

  //---

  return cmdBase_->setCmdRc(newModelData->id());
}

//------

// group_charts_model command

void
CQChartsGroupChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"  , ArgType::String, "model_id");
  addArg(argv, "-columns", ArgType::String, "columns");
}

QStringList
CQChartsGroupChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsGroupChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsGroupChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  CQChartsCmds::Columns columns;

  auto columnsStr = argv.getParseStr("columns");

  if (! cmds()->stringToModelColumns(model, columnsStr, columns))
    return false;

  //---

  auto *newModel = modelData->groupColumns(columns);

  if (! newModel)
    return errorMsg("Grouping failed");

  CQChartsCmds::ModelP newModelP(newModel);

  auto *newModelData = charts()->initModelData(newModelP);

  //---

  return cmdBase_->setCmdRc(newModelData->id());
}

//------

// write_charts_model command
// TODO: combine with export. Support vertical header

void
CQChartsWriteChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"    , ArgType::String , "model_id");
  addArg(argv, "-file"     , ArgType::String , "file name");
  addArg(argv, "-header"   , ArgType::SBool  , "show header");
  addArg(argv, "-index"    , ArgType::SBool  , "show index");
  addArg(argv, "-columns"  , ArgType::String , "columns to output");
  addArg(argv, "-max_rows" , ArgType::Integer, "maximum number of rows to write");
  addArg(argv, "-max_width", ArgType::Integer, "maximum column width");
  addArg(argv, "-hier"     , ArgType::SBool  , "output hierarchically");
  addArg(argv, "-separator", ArgType::String , "separator");
  addArg(argv, "-formatted", ArgType::SBool  , "columns are formatted");
}

QStringList
CQChartsWriteChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsWriteChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsWriteChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  CQChartsCmds::Columns columns;

  if (argv.hasParseArg("columns")) {
    auto columnsStr = argv.getParseStr("columns");

    if (! cmds()->stringToModelColumns(model, columnsStr, columns))
      return false;
  }

  //---

  // get max rows
  int maxRows = -1;

  if (argv.hasParseArg("max_rows")) {
    int maxRows1 = argv.getParseInt("max_rows", -1);

    if (maxRows1 > 0)
      maxRows = maxRows1;
  }

  //------

  // get max column width
  int maxWidth = -1;

  if (argv.hasParseArg("max_width")) {
    int maxWidth1 = argv.getParseInt("max_width", -1);

    if (maxWidth1 > 0)
      maxWidth = maxWidth1;
  }

  //------

  bool hier = true;

  if (argv.hasParseArg("hier")) {
    hier = argv.getParseBool("hier");
  }

  //------

  bool header = true, index = false;

  if (argv.hasParseArg("header"))
    header = argv.getParseBool("header");

  if (argv.hasParseArg("index"))
    index = argv.getParseBool("index");

  auto sep = QString("|");

  if (argv.hasParseArg("separator")) {
    sep = argv.getParseStr("separator");

    if (sep == "\\t")
      sep = "\t";
  }

  bool formatted = false;

  if (argv.hasParseArg("formatted"))
    formatted = argv.getParseBool("formatted");

  //------

  // set output stream
  auto filename = argv.getParseStr("file", "");

  std::ofstream fos;
  std::ostream* os = nullptr;

  if (filename != "") {
    fos.open(filename.toLatin1().constData(), std::ios_base::out);

    if (fos.fail())
      return errorMsg("Failed to open '" + filename + "'");

    os = &fos;
  }
  else
    os = &std::cout;

  //------

  class OutputRows {
   public:
    OutputRows(std::ostream *os, int nc, int maxWidth=-1, const QString &sep="|",
               bool index=false, bool formatted=false) :
     os_(os), nc_(nc), maxWidth_(maxWidth), sep_(sep), index_(index), formatted_(formatted) {
    }

    void setHeader(const QStringList &strs) {
      assert(strs.size() == nc_);

      int c = 0;

      if (index_)
        columnWidths_[c++] = 1;

      for (int i = 0; i < nc_; ++i) {
        const auto &str = strs[i];

        columnWidths_[c] = std::max(columnWidths_[c], str.length());

        if (maxWidth_ > 0)
          columnWidths_[c] = std::min(columnWidths_[c], maxWidth_);

        ++c;
      }

      header_ = strs;
    }

    void addRow(int depth, const QStringList &strs) {
      assert(strs.size() == nc_);

      int c = (index_ ? 1 : 0);

      for (int i = 0; i < nc_; ++i) {
        const auto &str = strs[i];

        columnWidths_[c] = std::max(columnWidths_[c], str.length());

        if (maxWidth_ > 0)
          columnWidths_[c] = std::min(columnWidths_[c], maxWidth_);

        ++c;
      }

      rows_.emplace_back(depth, strs);

      maxDepth_ = std::max(maxDepth_, depth);
    }

    void write(bool header) {
      if (header) {
        QString str;

        int c = 0;

        if (index_)
          ++c;

        for (int i = 0; i < nc_; ++i) {
          int w = (formatted_ ? columnWidths_[c] : 0);

          auto str1 = header_[i];

          if (formatted_ && str1.length() > w)
            str1 = str1.mid(0, w);

          if (c > 0)
            str += sep_;

          if (formatted_)
            str += QString("%1").arg(str1, w);
          else
            str += str1;

          ++c;
        }

        if (formatted_) {
          *os_ << QString(maxDepth_, ' ').toStdString();
          *os_ << " " << str.toStdString() << "\n";
        }
        else
          *os_ << str.toStdString() << "\n";
      }

      int r = 0;

      for (const auto &row : rows_) {
        if (formatted_) {
          int d1 = row.depth;
          int d2 = maxDepth_ - d1;

          *os_ << QString(d1, '.').toStdString();
          *os_ << QString(d2, ' ').toStdString();
          *os_ << " ";
        }

        //---

        QString str;

        int c = 0;

        if (index_) {
          str += QString::number(r);

          ++c;
        }

        for (int i = 0; i < nc_; ++i) {
          int w = (formatted_ ? columnWidths_[c] : 0);

          auto str1 = row.strs[i];

          if (formatted_ && str1.length() > w)
            str1 = str1.mid(0, w);

          if (c > 0)
            str += sep_;

          if (formatted_)
            str += QString("%1").arg(str1, w);
          else
            str += str1;

          ++c;
        }

        *os_ << str.toStdString() << "\n";

        ++r;
      }
    }

   private:
    using ColumnWidths = std::map<int, int>;

    struct Row {
      int         depth { 0 };
      QStringList strs;

      Row(int depth, const QStringList &strs) :
       depth(depth), strs(strs) {
      }
    };

    using Rows = std::vector<Row>;

    std::ostream* os_        { nullptr };
    int           nc_        { 0 };
    int           maxWidth_  { -1 };
    ColumnWidths  columnWidths_;
    Rows          rows_;
    QStringList   header_;
    int           maxDepth_  { 0 };
    QString       sep_;
    bool          index_     { false };
    bool          formatted_ { false };
  };

  //---

  // list values
  int role = Qt::DisplayRole;

  int nc = 0;

  if (columns.empty())
    nc = model.data()->columnCount();
  else
    nc = int(columns.size());

  OutputRows output(os, nc, maxWidth, sep, index, formatted);

  //---

  auto outputColumns = [&]() {
    QStringList strs;

    if (columns.empty()) {
      for (int ic = 0; ic < nc; ++ic) {
        bool ok;

        auto var = CQChartsModelUtil::modelHeaderValue(model.data(), ic,
                                                       Qt::Horizontal, role, ok);

        auto str = var.toString();

        strs += str;
      }
    }
    else {
      for (const auto &c : columns) {
        bool ok;

        auto var = CQChartsModelUtil::modelHeaderValue(model.data(), c.column(),
                                                       Qt::Horizontal, role, ok);

        auto str = var.toString();

        strs += str;
      }
    }

    output.setHeader(strs);
  };

  //---

  std::function<void(const QModelIndex &, int)> outputHier;

  outputHier = [&](const QModelIndex &parent, int depth) -> void {
    int nr = model.data()->rowCount(parent);

    if (maxRows > 0)
      nr = std::min(nr, maxRows);

    for (int r = 0; r < nr; ++r) {
      QStringList strs;

      if (columns.empty()) {
        for (int ic = 0; ic < nc; ++ic) {
          CQChartsColumn c(ic);

          bool ok;

          auto var = CQChartsModelUtil::modelValue(charts(), model.data(), r, c, parent, role, ok);

          auto str = var.toString();

          strs += str;
        }
      }
      else {
        for (const auto &c : columns) {
          bool ok;

          auto var = CQChartsModelUtil::modelValue(charts(), model.data(), r, c, parent, role, ok);

          auto str = var.toString();

          strs += str;
        }
      }

      output.addRow(depth, strs);

      //---

      if (hier) {
        auto parent1 = model.data()->index(r, 0, parent);

        if (model.data()->rowCount(parent1) > 0) {
          outputHier(parent1, depth + 1);
        }
      }
    }
  };

  //---

  *os << std::flush;

  outputColumns();

  outputHier(QModelIndex(), 0);

  output.write(header);

  *os << std::flush;

  return true;
}

//------

// sort_charts_model command

void
CQChartsSortChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"     , ArgType::String , "model_id");
  addArg(argv, "-column"    , ArgType::Column , "column to sort");
  addArg(argv, "-decreasing", ArgType::Boolean, "invert sort");
}

QStringList
CQChartsSortChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsSortChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsSortChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool decreasing = argv.getParseBool("decreasing");

  //------

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  // get column
  auto column = argv.getParseColumn("column", model.data());
  if (! column.isValid()) return errorMsg("Invalid column");

  //---

  Qt::SortOrder order = (decreasing ? Qt::DescendingOrder : Qt::AscendingOrder);

  CQChartsCmds::sortModel(model, column.column(), order);

  return true;
}

//------

// filter_charts_model command

void
CQChartsFilterChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"  , ArgType::String , "model_id");
  addArg(argv, "-expr"   , ArgType::String , "filter expression");
  addArg(argv, "-column" , ArgType::Column , "column");
  addArg(argv, "-type"   , ArgType::String , "filter type");
  addArg(argv, "-indices", ArgType::Boolean, "return indices");
}

QStringList
CQChartsFilterChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "model") return cmds()->modelArgValues();
  else if (arg == "type" )
    return QStringList() <<
      "expression" << "regexp" << "wildcard" << "simple" << "selected" << "non-selected";

  return QStringList();
}

bool
CQChartsFilterChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsFilterChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto expr = argv.getParseStr("expr");
  auto type = argv.getParseStr("type");

  if (! argv.hasParseArg("type"))
    type = "expression";

  //------

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  auto *modelFilter = qobject_cast<CQChartsModelFilter *>(model.data());

  if (! modelFilter)
    return errorMsg("No filter support for model");

  //--

  // get column
  int icolumn = -1;

  if (argv.hasParseArg("column")) {
    auto column = argv.getParseColumn("column", model.data());

    icolumn = column.column();
  }

  modelFilter->setFilterKeyColumn(icolumn);

  //---

  // filter
  // TODO: selection model from view

  if      (type == "expr" || type == "expression")
    modelFilter->setExpressionFilter(expr);
  else if (type == "regex" || type == "regexp")
    modelFilter->setRegExpFilter(expr);
  else if (type == "wildcard")
    modelFilter->setWildcardFilter(expr);
  else if (type == "simple")
    modelFilter->setSimpleFilter(expr);
  else if (type == "selected")
    modelFilter->setSelectionFilter(false);
  else if (type == "non-selected" || type == "non_selected")
    modelFilter->setSelectionFilter(true);
  else if (type == "?")
    return cmdBase_->setCmdRc(getArgValues("type"));
  else
    return errorMsg(QString("Invalid type '%1'").arg(type));

  //---

  if (argv.hasParseArg("indices")) {
    int nr = modelFilter->sourceModel()->rowCount();

    std::vector<int> rows;

    for (int r = 0; r < nr; ++r) {
      if (modelFilter->filterAcceptsRow(r, QModelIndex()))
        rows.push_back(r);
    }

    return cmdBase_->setCmdRc(rows);
  }

  return true;
}

//------

// create_charts_correlation_model command

void
CQChartsCreateChartsCorrelationModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"  , ArgType::String , "model_id");
  addArg(argv, "-flip"   , ArgType::Boolean, "correlate rows instead of columns");
  addArg(argv, "-columns", ArgType::String , "columns to correlate");
}

QStringList
CQChartsCreateChartsCorrelationModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsCorrelationModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsCorrelationModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool flip = argv.getParseBool("flip");

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  CQChartsCmds::Columns columns;

  if (argv.hasParseArg("columns")) {
    auto columnsStr = argv.getParseStr("columns");

    if (! cmds()->stringToModelColumns(model, columnsStr, columns))
      return false;
  }

  //---

  CQChartsLoader loader(charts());

  CQChartsLoader::CorrelationData correlationData;

  correlationData.flip    = flip;
  correlationData.columns = columns;

  auto *correlationModel =
    loader.createCorrelationModel(modelData->currentModel().data(), correlationData);

  CQChartsCmds::ModelP correlationModelP(correlationModel);

  auto *modelData1 = charts()->initModelData(correlationModelP);

  //---

  return cmdBase_->setCmdRc(modelData1->id());
}

//------

// create_charts_folded_model command

void
CQChartsCreateChartsFoldedModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"       , ArgType::String , "model_id");
  addArg(argv, "-column"      , ArgType::Column , "column to fold");
  addArg(argv, "-fold_keep"   , ArgType::Boolean, "keep folded column");
  addArg(argv, "-fold_data"   , ArgType::Boolean, "show folded column child data");
  addArg(argv, "-bucket_count", ArgType::Integer, "bucket count");
  addArg(argv, "-bucket_delta", ArgType::Real   , "bucket delta");
}

QStringList
CQChartsCreateChartsFoldedModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsFoldedModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsFoldedModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  // get column
  auto column = argv.getParseColumn("column", model.data());

  //------

  CQFoldData foldData;

  if (argv.hasParseArg("fold_keep"))
    foldData.setKeepColumn(argv.getParseBool("fold_keep"));

  if (argv.hasParseArg("fold_data"))
    foldData.setShowColumnData(argv.getParseBool("fold_data"));

  if (column.isValid()) {
    foldData.setColumn(column.column());

    //---

    CQChartsModelTypeData columnTypeData;

    if (CQChartsModelUtil::columnValueType(charts(), model.data(), column, columnTypeData)) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

      if (typeData) {
        if (typeData->isNumeric()) {
          foldData.setType(CQBucketer::Type::REAL_AUTO);

          if (argv.hasParseArg("bucket_delta"))
            foldData.setDelta(argv.getParseReal("bucket_delta"));

          if (argv.hasParseArg("bucket_count"))
            foldData.setNumAuto(argv.getParseInt("bucket_count"));
        }

        if (typeData->isIntegral()) {
          foldData.setIntegral(true);
        }
      }
    }
  }

  auto *foldedModel = new CQFoldedModel(model.data(), foldData);

  //---

  auto *foldProxyModel = new QSortFilterProxyModel;

  foldProxyModel->setObjectName("foldProxyModel");

  foldProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  foldProxyModel->setSourceModel(foldedModel);

  CQChartsCmds::ModelP foldedModelP(foldProxyModel);

  auto *foldedModelData = charts()->initModelData(foldedModelP);

  //---

  return cmdBase_->setCmdRc(foldedModelData->id());
}

//------

// create_charts_bucket_model command

void
CQChartsCreateChartsBucketModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model" , ArgType::String , "model_id");
  addArg(argv, "-column", ArgType::Column , "column to bucket");
  addArg(argv, "-multi" , ArgType::Boolean, "multiple bucket columns");
  addArg(argv, "-start" , ArgType::Real   , "bucket start");
  addArg(argv, "-delta" , ArgType::Real   , "bucket delta");
  addArg(argv, "-min"   , ArgType::Real   , "bucket min");
  addArg(argv, "-max"   , ArgType::Real   , "bucket max");
  addArg(argv, "-count" , ArgType::Integer, "number of buckets");
}

QStringList
CQChartsCreateChartsBucketModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsBucketModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsBucketModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  // get column
  auto column = argv.getParseColumn("column", model.data());

  //------

  auto *bucketModel = new CQBucketModel(model.data());

  if (argv.getParseBool("multi"))
    bucketModel->setMultiColumn(true);

  if (column.isValid()) {
    bucketModel->setBucketColumn(column.column());

    //---

    CQChartsModelTypeData columnTypeData;

    if (CQChartsModelUtil::columnValueType(charts(), model.data(), column, columnTypeData)) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

      if (typeData) {
        if (typeData->isNumeric()) {
          if (argv.hasParseArg("start") || argv.hasParseArg("delta")) {
            bucketModel->setBucketType(CQBucketModel::BucketType::REAL_RANGE);

            if (argv.hasParseArg("start"))
              bucketModel->setBucketStart(argv.getParseReal("start"));

            if (argv.hasParseArg("delta"))
              bucketModel->setBucketDelta(argv.getParseReal("delta"));
          }
          else {
            bucketModel->setBucketType(CQBucketModel::BucketType::REAL_AUTO);

            if (argv.hasParseArg("min"))
              bucketModel->setBucketMin(argv.getParseReal("min"));

            if (argv.hasParseArg("max"))
              bucketModel->setBucketMax(argv.getParseReal("max"));

            if (argv.hasParseArg("count"))
              bucketModel->setBucketCount(argv.getParseInt("count"));
          }
        }

        if (typeData->isIntegral()) {
          bucketModel->setBucketIntegral(true);
        }
      }
    }
  }

  CQChartsCmds::ModelP bucketModelP(bucketModel);

  auto *modelData1 = charts()->initModelData(bucketModelP);

  //---

  return cmdBase_->setCmdRc(modelData1->id());
}

//------

// create_charts_subset_model command

void
CQChartsCreateChartsSubsetModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model" , ArgType::String , "model_id");
  addArg(argv, "-left"  , ArgType::Column , "left (start) column");
  addArg(argv, "-right" , ArgType::Column , "right (end) column");
  addArg(argv, "-top"   , ArgType::Integer, "top (start) row");
  addArg(argv, "-bottom", ArgType::Integer, "bottom (end) row");
}

QStringList
CQChartsCreateChartsSubsetModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsSubsetModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsSubsetModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto *model = modelData->currentModel().data();

  //------

  auto left  = argv.getParseColumn("left" , model);
  auto right = argv.getParseColumn("right", model);

  int top    = argv.getParseInt("top"   , -1);
  int bottom = argv.getParseInt("bottom", -1);

  if (! left .isValid()) left  = CQChartsColumn(0);
  if (! right.isValid()) right = CQChartsColumn(model->columnCount() - 1);

  if (top    < 0) top    = 0;
  if (bottom < 0) bottom = model->rowCount() - 1;

  //------

  auto *subsetModel = new CQSubSetModel(model);

  auto tlIndex = model->index(top   , left .column());
  auto brIndex = model->index(bottom, right.column());

  subsetModel->setBounds(tlIndex, brIndex);

  CQChartsCmds::ModelP subsetModelP(subsetModel);

  auto *modelData1 = charts()->initModelData(subsetModelP);

  //---

  return cmdBase_->setCmdRc(modelData1->id());
}

//------

// create_charts_transpose_model command

void
CQChartsCreateChartsTransposeModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model", ArgType::String, "model_id");
}

QStringList
CQChartsCreateChartsTransposeModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsTransposeModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsTransposeModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto *model = modelData->currentModel().data();

  //------

  auto *transposeModel = new CQTransposeModel(model);

  CQChartsCmds::ModelP transposeModelP(transposeModel);

  auto *modelData1 = charts()->initModelData(transposeModelP);

  //---

  return cmdBase_->setCmdRc(modelData1->id());
}

//------

// create_charts_summary_model command

void
CQChartsCreateChartsSummaryModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"      , ArgType::String , "model_id");
  addArg(argv, "-max_rows"   , ArgType::Integer, "maximum rows");
  addArg(argv, "-random"     , ArgType::Boolean, "random rows");
  addArg(argv, "-sorted"     , ArgType::Boolean, "sorted rows");
  addArg(argv, "-sort_column", ArgType::Integer, "sort column");
  addArg(argv, "-sort_role"  , ArgType::String , "sort role");
  addArg(argv, "-sort_order" , ArgType::Enum   , "sort order").
   addNameValue("ascending" , Qt::AscendingOrder ).
   addNameValue("descending", Qt::DescendingOrder);
  addArg(argv, "-paged"      , ArgType::Boolean, "paged");
  addArg(argv, "-page_size"  , ArgType::Integer, "page size");
  addArg(argv, "-page_number", ArgType::Integer, "page number");
}

QStringList
CQChartsCreateChartsSummaryModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "model"    ) return cmds()->modelArgValues();
  else if (arg == "sort_role") return cmds()->roleArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsSummaryModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsSummaryModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto *model = modelData->currentModel().data();

  //------

  auto *summaryModel = new CQSummaryModel(model);

  //---

  if (argv.hasParseArg("max_rows"))
    summaryModel->setMaxRows(argv.getParseInt("max_rows", summaryModel->maxRows()));

  if (argv.hasParseArg("random"))
    summaryModel->setRandomMode(argv.getParseBool("random", summaryModel->isRandomMode()));

  if (argv.hasParseArg("sorted"))
    summaryModel->setSortMode(argv.getParseBool("sorted", summaryModel->isSortMode()));

  if (argv.hasParseArg("sort_column"))
    summaryModel->setSortColumn(argv.getParseInt("sort_column", summaryModel->sortColumn()));

  if (argv.hasParseArg("sort_role")) {
    auto roleName = argv.getParseStr("sort_role");

    if (roleName == "?")
      return cmdBase_->setCmdRc(getArgValues("sort_role"));

    int sortRole = CQChartsModelUtil::nameToRole(model, roleName);

    if (sortRole < 0)
      return errorMsg("Invalid sort role");

    summaryModel->setSortRole(sortRole);
  }

  if (argv.hasParseArg("sort_order")) {
    Qt::SortOrder order = static_cast<Qt::SortOrder>(argv.getParseInt("sort_order"));

    summaryModel->setSortOrder(order);
  }

  if (argv.hasParseArg("paged"))
    summaryModel->setPagedMode(argv.getParseBool("paged", summaryModel->isPagedMode()));

  if (argv.hasParseArg("page_size"))
    summaryModel->setPageSize(argv.getParseInt("page_size", summaryModel->pageSize()));

  if (argv.hasParseArg("page_number"))
    summaryModel->setCurrentPage(argv.getParseInt("page_number", summaryModel->currentPage()));

  //---

  CQChartsCmds::ModelP summaryModelP(summaryModel);

  auto *modelData1 = charts()->initModelData(summaryModelP);

  //---

  return cmdBase_->setCmdRc(modelData1->id());
}

//------

// create_charts_collapse_model command

void
CQChartsCreateChartsCollapseModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model", ArgType::String, "model_id");
  addArg(argv, "-sum"  , ArgType::String, "columns to calculate sum");
  addArg(argv, "-mean" , ArgType::String, "columns to calculate mean");
}

QStringList
CQChartsCreateChartsCollapseModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsCollapseModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsCollapseModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  auto argStringToColumns = [&](const QString &name, bool &ok) {
    ok = true;

    std::vector<CQChartsColumn> columns;

    auto columnsStrs = argv.getParseStrs(name);

    for (int i = 0; i < columnsStrs.length(); ++i) {
      const auto &columnsStr = columnsStrs[i];

      if (! columnsStr.length())
        continue;

      std::vector<CQChartsColumn> columns1;

      if (! CQChartsModelUtil::stringToColumns(model.data(), columnsStr, columns1)) {
        ok = errorMsg("Bad columns name '" + columnsStr + "'");
        continue;
      }

      for (const auto &column : columns1)
        columns.push_back(column);
    }

    return columns;
  };

  // get sum and mean columns
  bool ok;
  auto sumColumns  = argStringToColumns("sum" , ok); if (! ok) return false;
  auto meanColumns = argStringToColumns("mean", ok); if (! ok) return false;

  //---

  auto *collapseModel = new CQCollapseModel(model.data());

  //------

  auto *columnTypeMgr = charts()->columnTypeMgr();

  for (int c = 0; c < model->columnCount(); ++c) {
    CQChartsModelTypeData columnTypeData;

    if (! CQChartsModelUtil::columnValueType(charts(), model.data(), CQChartsColumn(c),
                                             columnTypeData))
      continue;

    const auto *typeData = columnTypeMgr->getType(columnTypeData.type);
    if (! typeData) continue;

    if (typeData->isNumeric()) {
      if (typeData->isIntegral())
        collapseModel->setColumnType(c, CQBaseModelType::INTEGER);
      else
        collapseModel->setColumnType(c, CQBaseModelType::REAL);
    }
  }

  for (const auto &column : sumColumns) {
    int c = column.column();

    collapseModel->setColumnCollapseOp(c, CQCollapseModel::CollapseOp::SUM);
  }

  for (const auto &column : meanColumns) {
    int c = column.column();

    collapseModel->setColumnCollapseOp(c, CQCollapseModel::CollapseOp::MEAN);
  }

  //------

  auto *collapseProxyModel = new QSortFilterProxyModel;

  collapseProxyModel->setObjectName("collapseProxyModel");

  collapseProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  collapseProxyModel->setSourceModel(collapseModel);

  CQChartsCmds::ModelP collapseModelP(collapseProxyModel);

  auto *collapseModelData = charts()->initModelData(collapseModelP);

  //---

  return cmdBase_->setCmdRc(collapseModelData->id());
}

//------

// create_charts_pivot_model command

void
CQChartsCreateChartsPivotModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"         , ArgType::String , "model_id");
  addArg(argv, "-hcolumns"      , ArgType::String , "horizontal columns");
  addArg(argv, "-vcolumns"      , ArgType::String , "vertical columns");
  addArg(argv, "-dcolumns"      , ArgType::String , "data columns");
  addArg(argv, "-value_types"   , ArgType::String , "column value types");
  addArg(argv, "-include_totals", ArgType::Boolean, "include totals");
  addArg(argv, "-fill_value"    , ArgType::String , "fill_value");
  addArg(argv, "-separator"     , ArgType::String , "separator");
}

QStringList
CQChartsCreateChartsPivotModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsPivotModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPivotModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  auto argStringToColumns = [&](const QString &name, bool &ok) {
    ok = true;

    CQPivotModel::Columns columns;

    auto columnsStrs = argv.getParseStrs(name);

    for (int i = 0; i < columnsStrs.length(); ++i) {
      const auto &columnsStr = columnsStrs[i];

      if (! columnsStr.length())
        continue;

      std::vector<CQChartsColumn> columns1;

      if (! CQChartsModelUtil::stringToColumns(model.data(), columnsStr, columns1)) {
        ok = errorMsg("Bad columns name '" + columnsStr + "'");
        continue;
      }

      for (const auto &column : columns1)
        columns.push_back(column.column());
    }

    return columns;
  };

  bool ok;
  auto hColumns = argStringToColumns("hcolumns", ok); if (! ok) return false;
  auto vColumns = argStringToColumns("vcolumns", ok); if (! ok) return false;

  //------

  auto *pivotModel = new CQPivotModel(model.data());

  pivotModel->setHColumns(hColumns);
  pivotModel->setVColumns(vColumns);

  if (argv.hasParseArg("dcolumns")) {
    bool ok;
    auto dColumns = argStringToColumns("dcolumns", ok);
    if (! ok) { delete pivotModel; return false; }

    pivotModel->setValueColumns(dColumns);
  }

  if (argv.hasParseArg("value_types")) {
    // get value types
    auto valueTypeStr = argv.getParseStr("value_types");

    QStringList valueTypeStrs;

    if (! CQTcl::splitList(valueTypeStr, valueTypeStrs))
      return errorMsg(QString("Invalid value_type strings '%1'").arg(valueTypeStr));

    CQPivotModel::ColumnValueTypes columnValueTypes;

    for (const auto &valueTypeStr : valueTypeStrs) {
      // split into column/value_types
      QStringList columnTypes;

      int numColumnTypes = 0;

      if (CQTcl::splitList(valueTypeStr, columnTypes))
        numColumnTypes = columnTypes.size();

      if (numColumnTypes != 1 && numColumnTypes != 2)
        return errorMsg(QString("Invalid column/value type string '%1'").arg(valueTypeStr));

      CQChartsColumn column;
      QString        typeStr;

      if (columnTypes.size() == 2) {
        auto columnName = columnTypes[0];

        if (! CQChartsModelUtil::stringToColumn(model.data(), columnName, column))
          return errorMsg(QString("Invalid column '%1'").arg(columnName));

        typeStr = columnTypes[1];
      }
      else
        typeStr = columnTypes[0];

      QStringList types;

      if (! CQTcl::splitList(typeStr, types))
        return errorMsg(QString("Invalid types string '%1'").arg(typeStr));

      CQPivotModel::ValueTypes valueTypes;

      for (const auto &typeStr : types) {
        auto valueType = CQPivotModel::stringToValueType(typeStr);

        if (valueType == CQPivotModel::ValueType::NONE)
          return errorMsg("Invalid value type '" + typeStr + "'");

        valueTypes.push_back(valueType);
      }

      columnValueTypes[column.column()] = valueTypes;
    }

    pivotModel->setColumnValueTypes(columnValueTypes);
  }

  if (argv.hasParseArg("include_totals"))
    pivotModel->setIncludeTotals(true);
  else
    pivotModel->setIncludeTotals(false);

  if (argv.hasParseArg("fill_value"))
    pivotModel->setFillValue(argv.getParseStr("fill_value"));

  if (argv.hasParseArg("separator"))
    pivotModel->setSeparator(argv.getParseStr("separator")[0]);

  //---

  auto *pivotProxyModel = new QSortFilterProxyModel;

  pivotProxyModel->setObjectName("pivotProxyModel");

  pivotProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  pivotProxyModel->setSourceModel(pivotModel);

  CQChartsCmds::ModelP pivotModelP(pivotProxyModel);

  auto *pivotModelData = charts()->initModelData(pivotModelP);

  //---

  return cmdBase_->setCmdRc(pivotModelData->id());
}

//------

// create_charts_stats_model command

void
CQChartsCreateChartsStatsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"  , ArgType::String, "model_id");
  addArg(argv, "-columns", ArgType::String, "columns");
}

QStringList
CQChartsCreateChartsStatsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsCreateChartsStatsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsStatsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  auto model = modelData->currentModel();

  //---

  int nc = model->columnCount();

  //---

  auto argStringToColumns = [&](const QString &name, bool &ok) {
    ok = true;

    std::vector<CQChartsColumn> columns;

    auto columnsStrs = argv.getParseStrs(name);

    for (int i = 0; i < columnsStrs.length(); ++i) {
      const auto &columnsStr = columnsStrs[i];

      if (! columnsStr.length())
        continue;

      std::vector<CQChartsColumn> columns1;

      if (! CQChartsModelUtil::stringToColumns(model.data(), columnsStr, columns1)) {
        ok = errorMsg("Bad columns name '" + columnsStr + "'");
        continue;
      }

      for (const auto &column : columns1)
        columns.push_back(column);
    }

    return columns;
  };

  bool ok;
  auto columns = argStringToColumns("columns", ok); if (! ok) return false;

  using ColumnSet = std::set<int>;

  ColumnSet columnSet;

  for (const auto &column : columns)
    columnSet.insert(column.column());

  if (columnSet.empty()) {
    for (int c = 0; c < nc; ++c)
      columnSet.insert(c);
  }

  //---

  auto *columnTypeMgr = charts()->columnTypeMgr();

  auto *details = modelData->details();

  //---

  struct ColumnData {
    int      column;
    QString  name;
    QVariant mean;
    QVariant min;
    QVariant lmedian;
    QVariant median;
    QVariant umedian;
    QVariant max;
    QVariant outliers;
  };

  using ColumnDatas = std::vector<ColumnData>;

  ColumnDatas columnDatas;

  for (int c = 0; c < nc; ++c) {
    if (columnSet.find(c) == columnSet.end())
      continue;

    //---

    CQChartsModelTypeData columnTypeData;

    if (! CQChartsModelUtil::columnValueType(charts(), model.data(), CQChartsColumn(c),
                                             columnTypeData))
      continue;

    const auto *typeData = columnTypeMgr->getType(columnTypeData.type);
    if (! typeData) continue;

    if (! typeData->isNumeric() || typeData->isTime())
      continue;

    auto *columnDetails = details->columnDetails(CQChartsColumn(c));

    //QModelIndex parent;

    bool ok;

    auto var = CQChartsModelUtil::modelHeaderValue(model.data(), CQChartsColumn(c),
                                                   Qt::DisplayRole, ok);

    ColumnData data;

    data.column   = c;
    data.name     = var.toString();
    data.mean     = columnDetails->meanValue();
    data.min      = columnDetails->minValue();
    data.lmedian  = columnDetails->lowerMedianValue();
    data.median   = columnDetails->medianValue();
    data.umedian  = columnDetails->upperMedianValue();
    data.max      = columnDetails->maxValue();
    data.outliers = columnDetails->outlierValues();

    columnDatas.push_back(std::move(data));
  }

  //---

  static auto columnNames = QStringList() <<
    "name" << "mean" << "min" << "lower_median" << "median" << "upper_median" <<
    "max" << "outliers";

  auto nc1 = columnNames.size();
  auto nr1 = columnDatas.size();

  auto *statsModel = new CQDataModel(nc1, int(nr1));

  for (int c = 0; c < nc1; ++c) {
    CQChartsModelUtil::setModelHeaderValue(statsModel, c, Qt::Horizontal, columnNames[c]);
  }

  auto setStatsModeValue = [&](int row, int col, const QVariant &value) {
    CQChartsModelUtil::setModelValue(statsModel, row, CQChartsColumn(col),
                                     QModelIndex(), value, Qt::DisplayRole);
  };

  for (size_t r = 0; r < nr1; ++r) {
    const auto &data = columnDatas[r];

    int ri = int(r);

    setStatsModeValue(ri, 0, data.name    );
    setStatsModeValue(ri, 1, data.mean    );
    setStatsModeValue(ri, 2, data.min     );
    setStatsModeValue(ri, 3, data.lmedian );
    setStatsModeValue(ri, 4, data.median  );
    setStatsModeValue(ri, 5, data.umedian );
    setStatsModeValue(ri, 6, data.max     );
    setStatsModeValue(ri, 7, data.outliers);
  }

  //------

  auto *statsProxyModel = new QSortFilterProxyModel;

  statsProxyModel->setObjectName("statsProxyModel");

  statsProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  statsProxyModel->setSourceModel(statsModel);

  CQChartsCmds::ModelP statsModelP(statsProxyModel);

  auto *statsModelData = charts()->initModelData(statsModelP);

  //---

  return cmdBase_->setCmdRc(statsModelData->id());
}

//------

// create_charts_data_model command

void
CQChartsCreateChartsDataModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-rows"   , ArgType::Integer, "number of rows");
  addArg(argv, "-columns", ArgType::Integer, "number of columns");
}

QStringList
CQChartsCreateChartsDataModelCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsCreateChartsDataModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCreateChartsDataModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int nr = argv.getParseInt("rows"   , 0); // number of rows
  int nc = argv.getParseInt("columns", 0); // number of columns

  //---

  auto *model = new CQDataModel(nc, nr);

  //------

  auto *proxyModel = new QSortFilterProxyModel;

  proxyModel->setObjectName("proxyModel");

  proxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  proxyModel->setSourceModel(model);

  CQChartsCmds::ModelP proxyModelP(proxyModel);

  auto *modelData = charts()->initModelData(proxyModelP);

  //---

  return cmdBase_->setCmdRc(modelData->id());
}

//------

// create_charts_fractal_model command

void
CQChartsCreateChartsFractalModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-type" , ArgType::String , "fractal type").setRequired();
  addArg(argv, "-start", ArgType::Integer, "start index");
  addArg(argv, "-end"  , ArgType::Integer, "end index");
}

QStringList
CQChartsCreateChartsFractalModelCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsCreateChartsFractalModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsFractalModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto type = argv.getParseStr("type");

  int start = 0;
  int end   = 80000;

  if (argv.hasParseArg("start"))
    start = argv.getParseInt("int", start);

  if (argv.hasParseArg("end"))
    end = argv.getParseInt("int", end);

  if (start > end)
    return errorMsg("Invalid start/end");

  //---

  QAbstractItemModel *model = nullptr;

  if      (type == "lorenz")
    model = new CQLorenzModel(start, end);
  else if (type == "pickover")
    model = new CQPickoverModel(start, end);
  else if (type == "dragon3d")
    model = new CQDragon3DModel(start, end);
  else if (type == "leaf3d")
    model = new CQLeaf3DModel(start, end);

  if (! model)
    return errorMsg("Invalid model type");

  //------

  auto *proxyModel = new QSortFilterProxyModel;

  proxyModel->setObjectName("proxyModel");

  proxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  proxyModel->setSourceModel(model);

  CQChartsCmds::ModelP proxyModelP(proxyModel);

  auto *modelData = charts()->initModelData(proxyModelP);

  //---

  return cmdBase_->setCmdRc(modelData->id());
}

//------

// export_charts_model command

void
CQChartsExportChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model"  , ArgType::String, "model_id");
  addArg(argv, "-to"     , ArgType::String, "destination format");
  addArg(argv, "-file"   , ArgType::String, "file name");
  addArg(argv, "-hheader", ArgType::SBool , "output horizontal header");
  addArg(argv, "-vheader", ArgType::SBool , "output vertical header");
}

QStringList
CQChartsExportChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "model") return cmds()->modelArgValues();
  else if (arg == "to"   ) return QStringList() << "csv" << "tsv";

  return QStringList();
}

bool
CQChartsExportChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsExportChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto toName  = argv.getParseStr ("to", "csv");
  bool hheader = argv.getParseBool("hheader", true);
  bool vheader = argv.getParseBool("vheader", false);

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  //---

  // set output stream
  auto filename = argv.getParseStr("file", "");

  std::ofstream fos;
  std::ostream* os = nullptr;

  if (filename != "") {
    fos.open(filename.toLatin1().constData(), std::ios_base::out);

    if (fos.fail())
      return errorMsg("Failed to open '" + filename + "'");

    os = &fos;
  }
  else
    os = &std::cout;

  //---

  if      (toName.toLower() == "csv")
    modelData->exportModel(*os, CQBaseModelDataType::CSV, hheader, vheader);
  else if (toName.toLower() == "tsv")
    modelData->exportModel(*os, CQBaseModelDataType::TSV, hheader, vheader);
  else if (toName.toLower() == "json")
    modelData->exportModel(*os, CQBaseModelDataType::JSON, hheader, vheader);
  else if (toName.toLower() == "?")
    return cmdBase_->setCmdRc(getArgValues("to"));
  else
    return errorMsg("Invalid output format");

  return true;
}

//------

// remove_charts_model command

void
CQChartsRemoveChartsModelCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model", ArgType::String, "model_id");
}

QStringList
CQChartsRemoveChartsModelCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "model") return cmds()->modelArgValues();

  return QStringList();
}

bool
CQChartsRemoveChartsModelCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsRemoveChartsModelCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  //---

  if (! charts()->removeModelData(modelData))
    return errorMsg("Failed to remove model");

  //---

  return true;
}

//------

// get_charts_data command

void
CQChartsGetChartsDataCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-model"     , ArgType::String, "model_id");
  addArg(argv, "-view"      , ArgType::String, "view name");
  addArg(argv, "-plot"      , ArgType::String, "plot name");
  addArg(argv, "-type"      , ArgType::String, "type name");
  addArg(argv, "-annotation", ArgType::String, "annotation name");
  argv.endCmdGroup();

  addArg(argv, "-object", ArgType::String, "object id");

  addArg(argv, "-column", ArgType::Column , "column name or number");
  addArg(argv, "-header", ArgType::Boolean, "get header data");
  addArg(argv, "-row"   , ArgType::Row    , "row number or id");
  addArg(argv, "-ind"   , ArgType::String , "model index");

  addArg(argv, "-role", ArgType::String, "role id");

  addArg(argv, "-name", ArgType::String, "option name").setRequired();
  addArg(argv, "-data", ArgType::String, "option data");

  addArg(argv, "-hidden", ArgType::Boolean, "include hidden data").setHidden(true);

  addArg(argv, "-sync", ArgType::Boolean, "sync before query").setHidden(true);

  addArg(argv, "-quiet", ArgType::Boolean, "fail quietly").setHidden(true);
}

QStringList
CQChartsGetChartsDataCmd::
getArgValues(const QString &arg, const NameValueMap &nameValues)
{
  if      (arg == "model"     ) return cmds()->modelArgValues();
  else if (arg == "view"      ) return cmds()->viewArgValues();
  else if (arg == "plot"      ) return cmds()->plotArgValues(nullptr);
  else if (arg == "type"      ) return cmds()->plotTypeArgValues();
  else if (arg == "annotation") return cmds()->annotationArgValues(nullptr, nullptr);
  else if (arg == "role"      ) return cmds()->roleArgValues(nullptr);
  else if (arg == "name"      ) {
    bool hasModel      = (nameValues.find("model"     ) != nameValues.end());
    bool hasView       = (nameValues.find("view"      ) != nameValues.end());
    bool hasPlot       = (nameValues.find("plot"      ) != nameValues.end());
    bool hasObject     = (nameValues.find("object"    ) != nameValues.end());
    bool hasType       = (nameValues.find("type"      ) != nameValues.end());
    bool hasAnnotation = (nameValues.find("annotation") != nameValues.end());

    if      (hasModel) {
      auto names = QStringList() <<
        "value" << "meta_names" << "meta_kets" << "meta_data" <<
        "num_rows" << "num_columns" << "hierarchical" <<
        "header" << "row" << "column" << "map" << "duplicates" << "column_index" <<
        "title" << "expr_model" << "data_model" << "base_model" <<
        "name" << "ind" << "find" /* << "property.<name>" */ << "read_only";

      auto detailsNames = CQChartsModelColumnDetails::getLongNamedValues();

      for (const auto &detailsName : detailsNames)
        names << QString("details.%1").arg(detailsName);

      return names;
    }
    else if (hasView) {
      static auto names = QStringList() <<
       "plots" << "annotations" << "selected_objects" << "view_width" << "view_height" <<
       "pixel_width" << "pixel_height" << "pixel_position" << "properties" <<
       "script_select_proc" << "mouse_press" << "mouse_modifier" << "view_path" <<
       "custom_widgets_parent";
      return names;
    }
    else if (hasType) {
      static auto names = QStringList() <<
       "properties" << "parameters" << "parameter.<parameter_name>" << "<property_name>";
      return names;
    }
    else if (hasPlot) {
      if (! hasObject) {
        static auto names = QStringList() <<
         "model" << "view" << "value" << "map" << "annotations" << "objects" <<
         "selected_objects" << "inds" << "plot_width" << "plot_height" << "pixel_width" <<
         "pixel_height" << "pixel_position" << "properties" << "set_hidden" << "errors" <<
         "color_filter" << "symbol_type_filter" << "symbol_size_filter";
        return names;
      }
      else {
        static auto names = QStringList() <<
          "inds" << "rows" << "connected";
        return names;
      }
    }
    else if (hasAnnotation) {
      static auto names = QStringList() << "view" << "plot" << "properties";
      return names;
    }
    else {
      static auto names = QStringList() <<
       "models" << "views" << "plot_types" << "plots" << "annotations" << "current_model" <<
       "column_types" << "column_type.names" << "column_type.descs" << "annotation_types" <<
       "symbols" << "procs" << "proc_data" << "role_names" << "path_list" << "view_key" <<
       "max_symbol_size" << "max_font_size" << "max_line_width" << "pattern_names";
      return names;
    }
  }

  return QStringList();
}

bool
CQChartsGetChartsDataCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsGetChartsDataCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto objectId = argv.getParseStr ("object");
  auto header   = argv.getParseBool("header");
  auto name     = argv.getParseStr ("name");
  auto hidden   = argv.getParseBool("hidden");
  auto sync     = argv.getParseBool("sync");
  auto quiet    = argv.getParseBool("quiet");

  //---

  auto *charts = this->charts();

  // model data
  if      (argv.hasParseArg("model")) {
    // get model
    auto modelId = argv.getParseStr("model");

    auto *modelData = cmds()->getModelDataOrCurrent(modelId);
    if (! modelData) return errorMsg("No model data for '" + modelId + "'");

    auto model = modelData->currentModel();

    //---

    auto roleName = argv.getParseStr("role");

    int role = -1;

    if (roleName != "") {
      if (roleName == "?")
        return cmdBase_->setCmdRc(getArgValues("role"));

      role = CQChartsModelUtil::nameToRole(model.data(), roleName);

      if (role < 0)
        return errorMsg("Invalid role");
    }

    //---

    // get optional column and row
    auto column = argv.getParseColumn("column", model.data());
    auto row    = argv.getParseRow("row");

    //---

    // get optional model indices
    std::vector<int> prows;

    if (argv.hasParseArg("ind")) {
      int irow { 0 };

      if (! CQChartsModelUtil::stringToModelInd(model.data(), argv.getParseStr("ind"),
                                                irow, column, prows))
        return errorMsg("Invalid model index");

      row = CQChartsRow(irow);
    }

    //---

    // get column header or row, column value
    if      (name == "value") {
      QVariant var;

      // get column header value
      if (header) {
        if (! column.isValid())
          return errorMsg("Invalid header column specified");

        bool ok;

        var = CQChartsModelUtil::modelHeaderValue(model.data(), column, role, ok);

        if (! var.isValid()) {
          if (quiet)
            return cmdBase_->setCmdRc(QString());

          return errorMsg("Invalid header value");
        }
      }
      // get row value
      else {
        QModelIndex parent;

        auto np = prows.size();

        for (size_t i1 = np; i1 > 0; --i1)
          parent = model.data()->index(prows[i1 - 1], 0, parent);

#if 0
        auto ind = model.data()->index(row, column.column(), parent);

        if (! ind.isValid())
          return errorMsg("Invalid data row/column specified");
#endif

        bool ok;

        var = CQChartsModelUtil::modelValue(charts, model.data(), row.row(),
                                            column, parent, role, ok);

        if (! var.isValid()) {
          if (quiet)
            return cmdBase_->setCmdRc(QString());

          return errorMsg(QString("Invalid model value for row %1, column %2").
                            arg(row.row()).arg(column.toString()));
        }
      }

      return cmdBase_->setCmdRc(var);
    }
    // get meta names
    else if (name == "meta_names") {
      auto names = CQChartsModelUtil::modelMetaNames(model.data());

      return cmdBase_->setCmdRc(names);
    }
    // get meta data
    else if (name == "meta_keys") {
      auto metaName = argv.getParseStr("data");

      auto keys = CQChartsModelUtil::modelMetaNameKeys(model.data(), metaName);

      return cmdBase_->setCmdRc(keys);
    }
    // get meta data
    else if (name == "meta_data") {
      auto data = argv.getParseStr("data");

      QStringList strs;

      if (! CQTcl::splitList(data, strs) || strs.length() < 1)
        return errorMsg(QString("Invalid data string '%1'").arg(data));

      strs.push_back("");

      auto metaName = strs[0];
      auto metaKey  = strs[1];

      auto metaValue = CQChartsModelUtil::getModelMetaValue(model.data(), metaName, metaKey);

      if (! metaValue.isValid()) {
        if (quiet)
          return cmdBase_->setCmdRc(QString());

        return errorMsg("Invalid meta data");
      }

      return cmdBase_->setCmdRc(metaValue);
    }
    // number of rows, number of columns, hierarchical
    else if (name == "num_rows" || name == "num_columns" || name == "hierarchical") {
      auto *details = modelData->details();

      if      (name == "num_rows")
        return cmdBase_->setCmdRc(details->numRows());
      else if (name == "num_columns")
        return cmdBase_->setCmdRc(details->numColumns());
      else if (name == "hierarchical")
        return cmdBase_->setCmdRc(details->isHierarchical());
    }
    // get header value
    else if (name == "header") {
      const auto *details = modelData->details();

      auto nc = details->numColumns();

      if (! column.isValid()) {
        QVariantList vars;

        for (int c = 0; c < nc; ++c) {
          bool ok;

          auto var = CQChartsModelUtil::modelHeaderValue(model.data(), CQChartsColumn(c),
                                                         role, ok);

          vars.push_back(var);
        }

        return cmdBase_->setCmdRc(vars);
      }
      else {
        if (column.column() < 0 || column.column() >= nc)
          return errorMsg("Invalid column number");

        bool ok;

        auto var = CQChartsModelUtil::modelHeaderValue(model.data(), column, role, ok);

        return cmdBase_->setCmdRc(var);
      }
    }
    // get row value
    else if (name == "row") {
      auto *details = modelData->details();

      int nr = details->numRows();
      int nc = details->numColumns();

      if (row.row() < 0 || row.row() >= nr)
        return errorMsg("Invalid row number");

      QModelIndex parent; // TODO;

      QVariantList vars;

      for (int c = 0; c < nc; ++c) {
        bool ok;

        auto var = CQChartsModelUtil::modelValue(charts, model.data(), row.row(),
                                                 CQChartsColumn(c), parent, role, ok);

        vars.push_back(var);
      }

      return cmdBase_->setCmdRc(vars);
    }
    // get column value
    else if (name == "column") {
      auto *details = modelData->details();

      int nr = details->numRows();
      int nc = details->numColumns();

      if (column.column() < 0 || column.column() >= nc)
        return errorMsg("Invalid column number");

      QModelIndex parent; // TODO;

      QVariantList vars;

      for (int r = 0; r < nr; ++r) {
        bool ok;

        auto var = CQChartsModelUtil::modelValue(charts, model.data(), r, column,
                                                 parent, role, ok);

        vars.push_back(var);
      }

      return cmdBase_->setCmdRc(vars);
    }
    // column details named value
    else if (name.left(8) == "details.") {
      auto name1 = name.mid(8);

      const auto *details = modelData->details();

      // TODO: is_outlier, data_name

      if (CQChartsModelColumnDetails::isNamedValue(name1)) {
        // specified column
        if (argv.hasParseArg("column")) {
          if (! column.isValid() || column.column() >= details->numColumns())
            return errorMsg("Invalid column specified");

          auto *columnDetails = details->columnDetails(column);

          return cmdBase_->setCmdRc(columnDetails->getNamedValue(name1));
        }
        // all columns
        else {
          int nc = details->numColumns();

          QVariantList vars;

          for (int c = 0; c < nc; ++c) {
            auto *columnDetails = details->columnDetails(CQChartsColumn(c));

            vars.push_back(columnDetails->getNamedValue(name1));
          }

          return cmdBase_->setCmdRc(vars);
        }
      }
      else if (name1 == "parameter") {
        if (! argv.hasParseArg("column"))
          return errorMsg("No columns specified");

        if (! argv.hasParseArg("data"))
          return errorMsg("No data specified");

        auto data = argv.getParseStr("data");

        auto *columnDetails = details->columnDetails(column);

        const auto *columnType = columnDetails->columnType();
        if (! columnType) return errorMsg("No type for column");

        if (data == "?") {
          auto names = columnType->paramNames();

          return cmdBase_->setCmdRc(names);
        }
        else {
          auto *param = columnType->getParam(data);
          if (! param) return errorMsg("No parameter for '" + data + "'");

          QVariant var;

          if (columnType->getNameValueVariant(columnDetails->nameValues(), data,
                                              param->type(), var))
            return cmdBase_->setCmdRc(var);
          else
            return errorMsg("Invalid parameter value for '" + data + "'");
        }
      }
      else if (name1 == "correlation") {
        if (! argv.hasParseArg("column"))
          return errorMsg("No columns specified");

        if (! argv.hasParseArg("data"))
          return errorMsg("No data specified");

        auto data = argv.getParseStr("data");

        CQChartsColumn column1;

        if (! CQChartsModelUtil::stringToColumn(model.data(), data, column1))
          column1 = CQChartsColumn();

        if (! column1.isValid())
          return errorMsg("Invalid data column specified");

        double c = details->correlation(column, column1);

        return cmdBase_->setCmdRc(c);
      }
      else if (name1 == "unique_id") {
        if (! argv.hasParseArg("column"))
          return errorMsg("No columns specified");

        if (! argv.hasParseArg("data"))
          return errorMsg("No data specified");

        if (! column.isValid() || column.column() >= details->numColumns())
          return errorMsg("Invalid column specified");

        auto *columnDetails = details->columnDetails(column);

        auto dataStr = argv.getParseStr("data");

        auto id = columnDetails->uniqueId(dataStr);

        return cmdBase_->setCmdRc(id);
      }
      else {
        CQChartsModelTypeData columnTypeData;

        if (CQChartsModelUtil::columnValueType(charts, model.data(), column, columnTypeData)) {
          auto *columnTypeMgr = charts->columnTypeMgr();

          const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

          if (typeData) {
            for (const auto &param : typeData->params()) {
              if (name1 == param->name()) {
                QVariant var;

                columnTypeData.nameValues.nameValue(param->name(), var);

                if (var.isValid())
                  return cmdBase_->setCmdRc(var);
              }
            }
          }
        }

        return errorMsg(QString("Invalid column details name '%1'").arg(name));
      }
    }
    // map value
    else if (name == "map") {
      auto *details = modelData->details();

      if (! column.isValid() || column.column() >= details->numColumns())
        return errorMsg("Invalid column specified");

#if 0
      auto ind = model.data()->index(row.row(), column.column());
#endif

      QModelIndex parent;

      bool ok;

      int role = -1; // edit or display role

      auto var = CQChartsModelUtil::modelValue(charts, model.data(), row.row(),
                                               column, parent, role, ok);

      auto *columnDetails = details->columnDetails(column);

      double r = columnDetails->map(var);

      return cmdBase_->setCmdRc(r);
    }
    // duplicate rows
    else if (name == "duplicates") {
      auto *details = modelData->details();

      std::vector<int> inds;

      if (argv.hasParseArg("column")) {
        if (! column.isValid())
          return errorMsg("Invalid column specified");

        inds = details->duplicates(column);
      }
      else
        inds = details->duplicates();

      QVariantList vars;

      for (std::size_t i = 0; i < inds.size(); ++i)
        vars.push_back(inds[i]);

      return cmdBase_->setCmdRc(vars);
    }
    // get index for column name
    else if (name == "column_index") {
      auto data = argv.getParseStr("data");

      CQChartsColumn column;

      if (! CQChartsModelUtil::stringToColumn(model.data(), data, column))
        column = CQChartsColumn();

      return cmdBase_->setCmdRc(column.column());
    }
    // get title
    else if (name == "title") {
      auto *dataModel = CQChartsModelUtil::getDataModel(model.data());

      QString title;

      if (dataModel)
        title = dataModel->title();

      return cmdBase_->setCmdRc(title);
    }
    // get expr model
    else if (name == "expr_model") {
      auto *exprModel = CQChartsModelUtil::getExprModel(model.data());

      return cmdBase_->setCmdRc(exprModel ? CQUtil::addObjectAlias(exprModel) : "");
    }
    // get data model
    else if (name == "data_model") {
      auto *dataModel = CQChartsModelUtil::getDataModel(model.data());

      return cmdBase_->setCmdRc(dataModel ? CQUtil::addObjectAlias(dataModel) : "");
    }
    // get base model
    else if (name == "base_model") {
      auto *baseModel = CQChartsModelUtil::getBaseModel(model.data());

      return cmdBase_->setCmdRc(baseModel ? CQUtil::addObjectAlias(baseModel) : "");
    }
#if 0
    // model property
    else if (name.left(9) == "property.") {
      auto name1 = name.mid(9);

      if (name1 == "?") {
        QStringList names;

        modelData->getPropertyNames(names);

        return cmdBase_->setCmdRc(names);
      }

      //---

      QVariant value;

      if (! modelData->getPropertyData(name1, value))
        return errorMsg("Failed to get model property '" + name1 + "'");

      return cmdBase_->setCmdRc(value);
    }
#endif
    // get model name
    else if (name == "name") {
      auto name = modelData->name();

      return cmdBase_->setCmdRc(name);
    }
    // get model index
    else if (name == "ind") {
      return cmdBase_->setCmdRc(modelData->ind());
    }
    // get row for column data
    else if (name == "find") {
      if (argv.hasParseArg("column")) {
        if (! column.isValid())
          return errorMsg("Invalid column specified");
      }

      auto data = argv.getParseStr("data");

      std::vector<int> rows;

      (void) CQChartsModelUtil::findRows(model.data(), column, data,
               CQChartsModelUtil::MatchType::EXACT_SINGLE, rows);

      return cmdBase_->setCmdRc(rows);
    }
    else if (name == "read_only") {
      auto *dataModel = CQChartsModelUtil::getDataModel(model.data());

      if (! dataModel)
        return errorMsg(QString("Invalid model type"));

      auto b = dataModel->isReadOnly();

      return cmdBase_->setCmdRc(b);
    }
    else if (name == "?") {
      NameValueMap nameValues; nameValues["model"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid model value name '" + name + "' specified");
  }
  // view data
  else if (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    auto *view = cmds()->getViewByName(viewName);
    if (! view) return false;

    if      (name == "plots") {
      return cmdBase_->setCmdRc(cmds()->plotArgValues(view));
    }
    else if (name == "annotations") {
      return cmdBase_->setCmdRc(cmds()->annotationArgValues(view, nullptr));
    }
    else if (name == "selected_objects") {
      CQChartsView::Objs objs;

      view->allSelectedObjs(objs);

      QStringList ids;

      for (const auto &obj : objs) {
        auto *plotObj = qobject_cast<CQChartsPlotObj *>(obj);

        if (plotObj)
          ids.push_back(plotObj->plot()->id() + ":" + plotObj->id());
      }

      return cmdBase_->setCmdRc(ids);
    }
    else if (name == "key_obj") {
      auto *key = view->key();

      return cmdBase_->setCmdRc(key ? key->id() : QString());
    }
    else if (name == "view_width") {
      auto data = argv.getParseStr("data");

      double w = view->lengthViewWidth(CQChartsLength::view(data));

      return cmdBase_->setCmdRc(w);
    }
    else if (name == "view_height") {
      auto data = argv.getParseStr("data");

      double h = view->lengthViewHeight(CQChartsLength::view(data));

      return cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_width") {
      auto data = argv.getParseStr("data");

      double w = view->lengthPixelWidth(CQChartsLength::view(data));

      return cmdBase_->setCmdRc(w);
    }
    else if (name == "pixel_height") {
      auto data = argv.getParseStr("data");

      double h = view->lengthPixelHeight(CQChartsLength::view(data));

      return cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_position") {
      auto data = argv.getParseStr("data");

      auto p = view->positionToPixel(CQChartsPosition::view(data));

      return cmdBase_->setCmdRc(p.qpoint());
    }
    else if (name == "properties") {
      QStringList names;

      view->getPropertyNames(names, hidden);

      return cmdBase_->setCmdRc(names);
    }
    else if (name == "script_select_proc") {
      auto str = view->scriptSelectProc();

      return cmdBase_->setCmdRc(str);
    }
    else if (name == "mouse_press") {
      auto p = view->mousePressPoint();

      return cmdBase_->setCmdRc(p.qpoint());
    }
    else if (name == "mouse_modifier") {
      auto mod = view->mouseClickMod();

      auto res = CQChartsUtil::selModToString(mod);

      return cmdBase_->setCmdRc(res);
    }
    // get view path
    else if (name == "view_path") {
      return cmdBase_->setCmdRc(CQUtil::fullName(view));
    }
    // get view settings custom widgets frame path
    else if (name == "custom_widgets_parent") {
      auto *window       = view->window();
      auto *viewSettings = (window ? window->viewSettings() : nullptr);
      auto *frame        = (viewSettings ? viewSettings->widgetsFrame() : nullptr);

      return cmdBase_->setCmdRc(frame ? CQUtil::fullName(frame) : "");
    }
    else if (name == "?") {
      NameValueMap nameValues; nameValues["view"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid view name '" + name + "' specified");
  }
  // type data
  else if (argv.hasParseArg("type")) {
    auto typeName = argv.getParseStr("type");

    if (! charts->isPlotType(typeName))
      return errorMsg("No type '" + typeName + "'");

    auto *type = charts->plotType(typeName);

    if (! type)
      return errorMsg("No type '" + typeName + "'");

    //---

    if      (name == "properties") {
      QStringList names;

      type->propertyNames(names);

      return cmdBase_->setCmdRc(names);
    }
    else if (name == "parameters") {
      const auto &parameters = type->parameters();

      QStringList names;

      for (auto &parameter : parameters)
        names.push_back(parameter->name());

      return cmdBase_->setCmdRc(names);
    }
    else if (name.left(10) == "parameter.") {
      auto data = argv.getParseStr("data");

      if (! type->hasParameter(data))
        return errorMsg("No parameter '" + data + "'");

      const auto *parameter = type->getParameter(data);

      auto name1 = name.mid(10);

      if (name1 == "properties") {
        QStringList names;

        parameter->propertyNames(names);

        return cmdBase_->setCmdRc(names);
      }
      else if (parameter->hasProperty(name1)) {
        return cmdBase_->setCmdRc(parameter->getPropertyValue(name1));
      }
      else
        return errorMsg("Invalid type name 'parameter." + name1 + "' specified");
    }
    else if (type->hasProperty(name)) {
      return cmdBase_->setCmdRc(type->getPropertyValue(name));
    }
    else if (name == "?") {
      NameValueMap nameValues; nameValues["type"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid type name '" + name + "' specified");
  }
  // plot data
  else if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    CQChartsView *view = nullptr;

    auto *plot = cmds()->getPlotByName(view, plotName);
    if (! plot) return false;

    const auto &plotModel = plot->currentModel();

    //---

    auto roleName = argv.getParseStr("role");

    int role = -1;

    if (roleName != "") {
      if (roleName == "?")
        return cmdBase_->setCmdRc(getArgValues("role"));

      role = CQChartsModelUtil::nameToRole(plotModel.data(), roleName);

      if (role < 0)
        return errorMsg("Invalid role");
    }

    //---

    auto row = argv.getParseRow("row", plot);

    //---

    if (sync)
      plot->syncAll();

    //---

    // get model ind
    if      (name == "model") {
      auto *modelData = charts->getModelData(plotModel);
      if (! modelData) return errorMsg("No model data");

      return cmdBase_->setCmdRc(modelData->id());
    }
    // get view ind
    else if (name == "view") {
      auto *view = plot->view();

      return cmdBase_->setCmdRc(view->id());
    }
    // get view path
    else if (name == "view_path") {
      auto *view = plot->view();

      return cmdBase_->setCmdRc(CQUtil::fullName(view));
    }
    // get column header or row, column value
    else if (name == "value") {
      auto column = argv.getParseColumn("column", plotModel.data());

      //---

      QVariant var;

      if (header) {
        if (! column.isValid())
          return errorMsg("Invalid header column specified");

        bool ok;

        var = CQChartsModelUtil::modelHeaderValue(plotModel.data(), column, role, ok);

        if (! var.isValid()) {
          if (quiet)
            return cmdBase_->setCmdRc(QString());

          return errorMsg("Invalid header value");
        }
      }
      else {
        CQChartsModelIndex ind(plot, row.row(), column, QModelIndex());

        bool ok;

        auto var = plot->modelValue(ind, role, ok);

        bool rc;

        return cmdBase_->setCmdRc(CQChartsVariant::toString(var, rc));
      }
    }
    else if (name == "map") {
      auto *modelData = charts->getModelData(plotModel);
      if (! modelData) return errorMsg("No model data");

      auto column = argv.getParseColumn("column", plotModel.data());

      //---

      auto *details = modelData->details();

      if (! column.isValid() || column.column() >= details->numColumns())
        return errorMsg("Invalid column specified");

      CQChartsModelIndex ind(plot, row.row(), column, QModelIndex());

      bool ok;

      auto var = plot->modelValue(ind, role, ok);

      auto *columnDetails = details->columnDetails(column);

      double r = columnDetails->map(var);

      return cmdBase_->setCmdRc(r);
    }
    else if (name == "annotations") {
      return cmdBase_->setCmdRc(cmds()->annotationArgValues(nullptr, plot));
    }
    else if (name == "objects") {
      QVariantList vars;

      const auto &objs = plot->plotObjects();

      for (const auto &obj : objs)
        vars.push_back(obj->id());

      return cmdBase_->setCmdRc(vars);
    }
    else if (name == "selected_objects") {
      CQChartsPlot::PlotObjs objs;

      plot->selectedPlotObjs(objs);

      QStringList ids;

      for (const auto &obj : objs)
        ids.push_back(obj->id());

      return cmdBase_->setCmdRc(ids);
    }
    else if (name == "key_obj") {
      auto *key = plot->key();

      return cmdBase_->setCmdRc(key ? key->id() : QString());
    }
    else if (name == "xaxis_obj") {
      auto *axis = plot->xAxis();

      return cmdBase_->setCmdRc(axis ? axis->id() : QString());
    }
    else if (name == "yaxis_obj") {
      auto *axis = plot->yAxis();

      return cmdBase_->setCmdRc(axis ? axis->id() : QString());
    }
    else if (name == "title_obj") {
      auto *title = plot->title();

      return cmdBase_->setCmdRc(title ? title->id() : QString());
    }
    // get model indices or rows for object
    else if (name == "inds" || name == "rows") {
      if (! objectId.length())
        return errorMsg("Missing object id");

      // get object indices (unnormalized)
      auto inds = plot->getObjectInds(objectId);

      QVariantList vars;

      if (name == "inds") {
        for (int i = 0; i < inds.length(); ++i)
          vars.push_back(inds[i]);
      }
      else {
        std::set<int> rows;

        for (int i = 0; i < inds.length(); ++i)
          rows.insert(inds[i].row());

        for (const auto &r : rows)
          vars.push_back(CQModelUtil::intVariant(r));
      }

      return cmdBase_->setCmdRc(vars);
    }
    // get connected objects
    else if (name == "connected") {
      if (! objectId.length())
        return errorMsg("Missing object id");

      auto objs = plot->getObjectConnected(objectId);

      QVariantList vars;

      for (const auto &obj : objs)
        vars.push_back(obj->id());

      return cmdBase_->setCmdRc(vars);
    }
    else if (name == "plot_width") {
      auto data = argv.getParseStr("data");

      double w = plot->lengthPlotWidth(CQChartsLength::plot(data));

      return cmdBase_->setCmdRc(w);
    }
    else if (name == "plot_height") {
      auto data = argv.getParseStr("data");

      double h = plot->lengthPlotHeight(CQChartsLength::plot(data));

      return cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_width") {
      auto data = argv.getParseStr("data");

      double w = plot->lengthPixelWidth(CQChartsLength::plot(data));

      return cmdBase_->setCmdRc(w);
    }
    else if (name == "pixel_height") {
      auto data = argv.getParseStr("data");

      double h = plot->lengthPixelHeight(CQChartsLength::plot(data));

      return cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_position") {
      auto data = argv.getParseStr("data");

      auto p = plot->positionToPixel(CQChartsPosition::plot(data));

      return cmdBase_->setCmdRc(p.qpoint());
    }
    else if (name == "properties") {
      QStringList names;

      plot->getPropertyNames(names, hidden);

      return cmdBase_->setCmdRc(names);
    }
    else if (name == "set_hidden") {
      int id = argv.getParseInt("data", -1);

      if (id < 0)
        return errorMsg("Invalid data '" + argv.getParseStr("data") + "' specified");

      return cmdBase_->setCmdRc(plot->isSetHidden(id));
    }
    else if (name == "errors") {
      QStringList strs;

      plot->getErrors(strs);

      return cmdBase_->setCmdRc(strs);
    }
    else if (name == "color_filter") {
      auto typeNames = plot->colorFilterNames();

      return cmdBase_->setCmdRc(typeNames);
    }
    else if (name == "symbol_type_filter") {
      auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot);

      QStringList typeNames;

      if (pointPlot)
        typeNames = pointPlot->symbolTypeFilterNames();

      return cmdBase_->setCmdRc(typeNames);
    }
    else if (name == "symbol_size_filter") {
      auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot);

      QStringList typeNames;

      if (pointPlot)
        typeNames = pointPlot->symbolSizeFilterNames();

      return cmdBase_->setCmdRc(typeNames);
    }
    else if (name == "?") {
      NameValueMap nameValues; nameValues["plot"] = "";
      if (objectId.length()) nameValues["object"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid plot name '" + name + "' specified");
  }
  // annotation data
  else if (argv.hasParseArg("annotation")) {
    auto annotationName = argv.getParseStr("annotation");

    auto *annotation = cmds()->getAnnotationByName(annotationName);
    if (! annotation) return false;

    // get view ind
    if      (name == "view") {
      auto *view = annotation->view();

      if (view)
        return cmdBase_->setCmdRc(view->id());
      else
        return cmdBase_->setCmdRc(QString());
    }
    // get plot ind
    else if (name == "plot") {
      auto *plot = annotation->plot();

      if (plot)
        return cmdBase_->setCmdRc(plot->id());
      else
        return cmdBase_->setCmdRc(QString());
    }
    // get column header or row, column value
    else if (name == "properties") {
      QStringList names;

      annotation->getPropertyNames(names, hidden);

      return cmdBase_->setCmdRc(names);
    }
    // get widget path
    else if (name == "widget_path") {
      auto *widgetAnnotation = dynamic_cast<CQChartsWidgetAnnotation *>(annotation);
      if (! widgetAnnotation) return errorMsg("Invalid annotation for widget_path");

      return cmdBase_->setCmdRc(CQUtil::fullName(widgetAnnotation->widget().widget()));
    }
    else if (name == "?") {
      NameValueMap nameValues; nameValues["annotation"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid annotation name '" + name + "' specified");
  }
  // global charts data
  else {
    if      (name == "models") {
      return cmdBase_->setCmdRc(cmds()->modelArgValues());
    }
    else if (name == "views") {
      return cmdBase_->setCmdRc(cmds()->viewArgValues());
    }
    else if (name == "plot_types") {
      return cmdBase_->setCmdRc(cmds()->plotTypeArgValues());
    }
    else if (name == "plots") {
      return cmdBase_->setCmdRc(cmds()->plotArgValues(nullptr));
    }
    else if (name == "annotations") {
      return cmdBase_->setCmdRc(cmds()->annotationArgValues(nullptr, nullptr));
    }
    else if (name == "current_model") {
      auto *modelData = charts->currentModelData();
      if (! modelData) return errorMsg("No model data");

      return cmdBase_->setCmdRc(modelData->id());
    }
    else if (name == "column_types") {
      auto *columnTypeMgr = charts->columnTypeMgr();

      QStringList names;

      columnTypeMgr->typeNames(names);

      return cmdBase_->setCmdRc(names);
    }
    else if (name == "column_type.names" || name == "column_type.descs") {
       if (! argv.hasParseArg("model"))
         return errorMsg("Missing data for '" + name + "'");

      auto dataStr = argv.getParseStr("data");

      auto *columnTypeMgr = charts->columnTypeMgr();

      const auto *columnType = columnTypeMgr->getNamedType(dataStr);
      if (! columnType) return errorMsg("Invalid column type '" + dataStr + "' for '" + name + "'");

      QStringList names;

      if (name == "column_type.names") {
        for (const auto &param : columnType->params())
          names << param->name();
      }
      else {
        for (const auto &param : columnType->params())
          names << param->tip();
      }

      return cmdBase_->setCmdRc(names);
    }
    else if (name == "annotation_types") {
      auto names = CQChartsAnnotation::typeNames();

      return cmdBase_->setCmdRc(names);
    }
    else if (name == "symbols") {
      QStringList symbolNames;

      if (argv.hasParseArg("data")) {
        auto dataStr = argv.getParseStr("data");

        auto *symbolSet = charts->symbolSetMgr()->symbolSet(dataStr);

        if (! symbolSet)
          return errorMsg(QString("No Symbol Set '%1'").arg(dataStr));

        symbolNames = symbolSet->symbolNames();
      }
      else {
        symbolNames = CQChartsSymbolType::typeNames();
      }

      return cmdBase_->setCmdRc(symbolNames);
    }
    else if (name == "procs") {
      QStringList procs;

      charts->getProcNames(CQCharts::ProcType::TCL, procs);

      return cmdBase_->setCmdRc(procs);
    }
    else if (name == "proc_data") {
       if (! argv.hasParseArg("model"))
         return errorMsg("Missing data for '" + name + "'");

      auto dataStr = argv.getParseStr("data");

      QString args, body;

      charts->getProcData(CQCharts::ProcType::TCL, dataStr, args, body);

      auto strs = QStringList() << args << body;

      return cmdBase_->setCmdRc(strs);
    }
    else if (name == "role_names") {
      return cmdBase_->setCmdRc(cmds()->roleArgValues(nullptr));
    }
    else if (name == "path_list") {
      auto strs = charts->pathList();

      return cmdBase_->setCmdRc(strs);
    }
    else if (name == "view_key") {
      return cmdBase_->setCmdRc(charts->hasViewKey());
    }
    else if (name == "max_symbol_size") {
      return cmdBase_->setCmdRc(charts->maxSymbolSize());
    }
    else if (name == "max_font_size") {
      return cmdBase_->setCmdRc(charts->maxFontSize());
    }
    else if (name == "max_line_width") {
      return cmdBase_->setCmdRc(charts->maxLineWidth());
    }
    else if (name == "pattern_names") {
      return cmdBase_->setCmdRc(CQChartsFillPattern().enumNames());
    }
    else if (name == "?") {
      NameValueMap nameValues;

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid global name '" + name + "' specified");
  }

  return true;
}

//------

// set_charts_data command

void
CQChartsSetChartsDataCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-model"     , ArgType::String, "model_id");
  addArg(argv, "-view"      , ArgType::String, "view name");
  addArg(argv, "-type"      , ArgType::String, "type name");
  addArg(argv, "-plot"      , ArgType::String, "plot name");
  addArg(argv, "-annotation", ArgType::String, "annotation name");
  argv.endCmdGroup();

  addArg(argv, "-object", ArgType::String, "object id");

  addArg(argv, "-column", ArgType::Column , "column name or number");
  addArg(argv, "-header", ArgType::Boolean, "get header data");
  addArg(argv, "-row"   , ArgType::Row    , "row number or id");
  addArg(argv, "-ind"   , ArgType::String , "model index");

  addArg(argv, "-role", ArgType::String, "role id");

  addArg(argv, "-name" , ArgType::String, "option name");
  addArg(argv, "-value", ArgType::String, "option value");
  addArg(argv, "-data" , ArgType::String, "option data");

  addArg(argv, "-hidden", ArgType::Boolean, "include hidden data").setHidden(true);
}

QStringList
CQChartsSetChartsDataCmd::
getArgValues(const QString &arg, const NameValueMap &nameValues)
{
  if      (arg == "model"     ) return cmds()->modelArgValues();
  else if (arg == "view"      ) return cmds()->viewArgValues();
  else if (arg == "plot"      ) return cmds()->plotArgValues(nullptr);
  else if (arg == "type"      ) return cmds()->plotTypeArgValues();
  else if (arg == "annotation") return cmds()->annotationArgValues(nullptr, nullptr);
  else if (arg == "role"      ) return cmds()->roleArgValues(nullptr);
  else if (arg == "name"      ) {
    bool hasModel      = (nameValues.find("model"     ) != nameValues.end());
    bool hasView       = (nameValues.find("view"      ) != nameValues.end());
    bool hasPlot       = (nameValues.find("plot"      ) != nameValues.end());
    bool hasObject     = (nameValues.find("object"    ) != nameValues.end());
    bool hasType       = (nameValues.find("type"      ) != nameValues.end());
    bool hasAnnotation = (nameValues.find("annotation") != nameValues.end());

    if      (hasModel) {
      auto names = QStringList() <<
        "value" << "column_type" << "header_type" << "meta_data" << "name" <<
        "process_expression" << "size" << "read_only" << "details.parameter";

      return names;
    }
    else if (hasView) {
      static auto names = QStringList() <<
       "fit" << "zoom_full" << "script_select_proc";
      return names;
    }
    else if (hasType) {
      static auto names = QStringList();
      return names;
    }
    else if (hasPlot) {
      if (! hasObject) {
        static auto names = QStringList() <<
         "fit" << "zoom_full" << "updates_enabled" << "set_hidden" << "model" <<
         "color_filter" << "symbol_type_filter" << "symbol_size_filter";
        return names;
      }
      else {
        static auto names = QStringList() <<
          "select" << "tick_label";
        return names;
      }
    }
    else if (hasAnnotation) {
      static auto names = QStringList() <<
        "tick_label" << "fit";
      return names;
    }
    else {
      static auto names = QStringList() <<
       "path_list" << "view_key" << "max_symbol_size" << "max_font_size" <<
       "max_line_width";
      return names;
    }
  }

  return QStringList();
}

// set charts data
bool
CQChartsSetChartsDataCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsSetChartsDataCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto objectId = argv.getParseStr ("object");
  bool header   = argv.getParseBool("header");
  auto name     = argv.getParseStr ("name");
//bool hidden   = argv.getParseBool("hidden");
  auto value    = argv.getParseStr ("value");

  //---

  auto *charts = this->charts();

  // model data
  if      (argv.hasParseArg("model")) {
    // get model
    auto modelId = argv.getParseStr("model");

    auto *modelData = cmds()->getModelDataOrCurrent(modelId);
    if (! modelData) return errorMsg("No model data for '" + modelId + "'");

    auto model = modelData->currentModel();

    //---

    auto roleName = argv.getParseStr("role");

    int role = -1;

    if (roleName != "") {
      if (roleName == "?")
        return cmdBase_->setCmdRc(cmds()->roleArgValues(model.data()));

      role = CQChartsModelUtil::nameToRole(model.data(), roleName);

      if (role < 0)
        return errorMsg("Invalid role");
    }

    //---

    std::vector<int> prows;

    auto column = argv.getParseColumn("column", model.data());
    auto row    = argv.getParseRow("row");

    if (argv.hasParseArg("ind")) {
      int irow { 0 };

      if (! CQChartsModelUtil::stringToModelInd(model.data(), argv.getParseStr("ind"),
                                                irow, column, prows))
        return errorMsg("Invalid model index");

      row = CQChartsRow(irow);
    }

    //---

    // set column header or row, column value
    if      (name == "value") {
      if (header) {
        if (! column.isValid())
          return errorMsg("Invalid header column specified");

        if (! CQChartsModelUtil::setModelHeaderValue(model.data(), column, value, role))
          return errorMsg("Failed to set header value");
      }
      else {
        QModelIndex parent;

        auto np = prows.size();

        for (size_t i1 = np; i1 > 0; --i1)
          parent = model.data()->index(prows[i1 - 1], 0, parent);

        auto ind = model.data()->index(row.row(), column.column(), parent);

        if (! ind.isValid())
          return errorMsg(QString("Invalid data row/column specified '%1,%2'").
                           arg(row.row()).arg(column.column()));

        if (! CQChartsModelUtil::setModelValue(model.data(), row.row(), column, parent,
                                               value, role))
          return errorMsg("Failed to set row value");
      }
    }
    // set column type
    else if (name == "column_type") {
      if (column.isValid()) {
        if (! CQChartsModelUtil::setColumnTypeStr(charts, model.data(), column, value))
          return errorMsg(QString("Invalid column type '%1'").arg(value));
      }
      else {
        if (! CQChartsModelUtil::setColumnTypeStrs(charts, model.data(), value))
          return errorMsg(QString("Invalid column type string '%1'").arg(value));
      }
    }
    // set header type
    else if (name == "header_type") {
      if (column.isValid()) {
        if (! CQChartsModelUtil::setHeaderTypeStr(charts, model.data(), column, value))
          return errorMsg(QString("Invalid header type '%1'").arg(value));
      }
      else {
        if (! CQChartsModelUtil::setHeaderTypeStrs(charts, model.data(), value))
          return errorMsg(QString("Invalid header type string '%1'").arg(value));
      }
    }
    // set meta data
    else if (name == "meta_data") {
      auto data = argv.getParseStr("data");

      QStringList strs;

      if (! CQTcl::splitList(data, strs) || strs.length() < 1)
        return errorMsg(QString("Invalid data string '%1'").arg(data));

      strs.push_back("");

      auto metaName = strs[0];
      auto metaKey  = strs[1];

      CQChartsModelUtil::setModelMetaValue(model.data(), metaName, metaKey, value);
    }
    // set model name
    else if (name == "name") {
      charts->setModelName(modelData, value);
    }
    // set model process expression
    else if (name == "process_expression") {
      CQChartsExprModel::Function function;
      CQChartsColumn              column;
      QString                     expr;

      if (! CQChartsModelUtil::decodeExpression(model.data(), value, function, column, expr))
        return errorMsg("Invalid model expression '" + value + "'");

      CQChartsModelUtil::processExpression(model.data(), function, column, expr);
    }
    // data model size
    else if (name == "size") {
      // split into size (rows, cols)
      QStringList strs;

      if (! CQTcl::splitList(value, strs) || strs.length() != 2)
        return errorMsg(QString("Invalid size string '%1'").arg(value));

      bool ok1, ok2;

      long r = CQChartsUtil::toInt(strs[0], ok1);
      long c = CQChartsUtil::toInt(strs[1], ok2);

      if (! ok1 || ! ok2 || r < 0 || c < 0)
        return errorMsg(QString("Invalid size values '%1' and '%2'").arg(strs[0]).arg(strs[1]));

      auto *dataModel = CQChartsModelUtil::getDataModel(model.data());

      if (! dataModel)
        return errorMsg(QString("Invalid model type"));

      dataModel->resizeModel(int(c), int(r));
    }
#if 0
    // model property
    else if (name.left(9) == "property.") {
      auto name1 = name.mid(9);

      if (name1 == "?") {
        QStringList names;

        modelData->getPropertyNames(names);

        return cmdBase_->setCmdRc(names);
      }

      if (! modelData->setPropertyData(name1, value))
        return errorMsg("Failed to set model property '" + name1 + "' '" + value + "'");
    }
#endif
    else if (name == "read_only") {
      auto *dataModel = CQChartsModelUtil::getDataModel(model.data());

      if (! dataModel)
        return errorMsg(QString("Invalid model type"));

      bool ok;
      bool b = CQChartsCmdBaseArgs::stringToBool(value, &ok);

      dataModel->setReadOnly(b);
    }
    // column details named value
    else if (name.left(8) == "details.") {
      auto name1 = name.mid(8);

      if (name1 == "parameter") {
        if (! argv.hasParseArg("column"))
          return errorMsg("No columns specified");

        if (! argv.hasParseArg("data"))
          return errorMsg("No data specified");

        auto data = argv.getParseStr("data");

        auto *details = modelData->details();

        auto *columnDetails = details->columnDetails(column);

        const auto *columnType = columnDetails->columnType();
        if (! columnType) return errorMsg("No type for column");

        if (data == "?") {
          auto names = columnType->paramNames();

          return cmdBase_->setCmdRc(names);
        }
        else {
          auto *param = columnType->getParam(data);
          if (! param) return errorMsg("No parameter for '" + data + "'");

          QVariant var;

          auto nameValues = columnDetails->nameValues();

          if (! columnType->setNameValueVariant(nameValues, data, param->type(), value))
            return errorMsg("Invalid parameter value for '" + data + "'");

          auto *columnTypeMgr = charts->columnTypeMgr();

          if (! columnTypeMgr->setModelColumnType(model.data(), column, columnType->type(),
                                                  nameValues))
            return errorMsg("Failed to set column type data for '" + data + "'");
        }
      }
      else
        return errorMsg(QString("Invalid column details name '%1'").arg(name));
    }
    else if (name == "?") {
      NameValueMap nameValues; nameValues["model"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid model value name '" + name + "' specified");
  }
  // view data
  else if (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    auto *view = cmds()->getViewByName(viewName);
    if (! view) return false;

    if      (name == "fit") {
      view->fitSlot();
    }
    else if (name == "zoom_full") {
      view->zoomFullSlot();
    }
    else if (name == "script_select_proc") {
      view->setScriptSelectProc(value);
    }
    else if (name == "?") {
      NameValueMap nameValues; nameValues["view"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid view name '" + name + "' specified");
  }
  // type data
  else if (argv.hasParseArg("type")) {
     return errorMsg("Invalid type name '" + name + "' specified");
  }
  // plot data
  else if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    CQChartsView *view = nullptr;

    auto *plot = cmds()->getPlotByName(view, plotName);
    if (! plot) return false;

    if      (name == "fit") {
      plot->autoFit();
    }
    else if (name == "zoom_full") {
      plot->zoomFull();
    }
    else if (name == "updates_enabled") {
      bool ok;
      bool b = CQChartsCmdBaseArgs::stringToBool(value, &ok);

      plot->setUpdatesEnabled(b);

      if (b) {
        plot->updateRangeAndObjs();

        plot->drawObjs();
      }
    }
    else if (name == "set_hidden") {
      int id = argv.getParseInt("data", -1);
      if (id < 0) return errorMsg("Invalid data '" + argv.getParseStr("data") + "' specified");

      bool ok;
      bool b = CQChartsCmdBaseArgs::stringToBool(value, &ok);

      plot->setSetHidden(id, b);
    }
    else if (name == "select") {
      if (objectId.length()) {
        auto *obj = plot->getObject(objectId);
        if (! obj) return errorMsg("Invalid plot object id '" + objectId + "'");

        auto selMod = CQChartsUtil::stringToSelMod(value);
        if (selMod == CQChartsSelMod::NONE) selMod = CQChartsSelMod::REPLACE;

        plot->selectOneObj(obj, selMod);
      }
    }
    else if (name == "deselect") {
      plot->deselectAll();
    }
    else if (name == "model") {
      // get model
      auto *modelData = cmds()->getModelDataOrCurrent(value);
      if (! modelData) return errorMsg("No model data for '" + value + "'");

      plot->replaceModel(plot->currentModel(), modelData->currentModel());
    }
    else if (name == "extra_model") {
      // get model
      auto *modelData = cmds()->getModelDataOrCurrent(value);
      if (! modelData) return errorMsg("No model data for '" + value + "'");

      plot->addModel(modelData->currentModel());
    }
    else if (name == "tick_label") {
      if (objectId.length()) {
        auto *obj = plot->getObject(objectId);
        if (! obj) return errorMsg("Invalid plot object id '" + objectId + "'");

        // split into tick value/label
        QStringList strs;

        if (! CQTcl::splitList(value, strs) || strs.size() != 2)
          return errorMsg(QString("Invalid tick label '%1'").arg(value));

        bool ok;
        long i = CQChartsUtil::toInt(strs[0], ok);
        if (! ok) return errorMsg(QString("Invalid tick label position '%1'").arg(strs[0]));

        auto *xaxis = plot->xAxis();
        auto *yaxis = plot->yAxis();

        if      (xaxis == obj) {
          xaxis->setTickLabel(i, strs[1]);
        }
        else if (yaxis == obj) {
          yaxis->setTickLabel(i, strs[1]);
        }
      }
    }
    else if (name == "color_filter") {
      // split into color filters
      QStringList strs;

      if (CQTcl::splitList(value, strs))
        plot->setColorFilterNames(strs);
    }
    else if (name == "symbol_type_filter") {
      auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot);

      // split into symbol type filters
      QStringList strs;

      if (pointPlot && CQTcl::splitList(value, strs))
        pointPlot->setSymbolTypeFilterNames(strs);
    }
    else if (name == "symbol_size_filter") {
      auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot);

      // split into symbol size filters
      QStringList strs;

      if (pointPlot && CQTcl::splitList(value, strs))
        pointPlot->setSymbolSizeFilterNames(strs);
    }
    // plot object property
    else if (name == "?") {
      NameValueMap nameValues; nameValues["plot"] = "";
      if (objectId.length()) nameValues["object"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid plot name '" + name + "' specified");
  }
  // annotation data
  else if (argv.hasParseArg("annotation")) {
    auto annotationName = argv.getParseStr("annotation");

    auto *annotation = cmds()->getAnnotationByName(annotationName);
    if (! annotation) return false;

    if      (name == "tick_label") {
      auto *axisAnnotation = dynamic_cast<CQChartsAxisAnnotation *>(annotation);

      if (! axisAnnotation)
        return errorMsg("Invalid axis annotation");

      // split into tick value/label
      QStringList strs;

      if (! CQTcl::splitList(value, strs) || strs.size() != 2)
        return errorMsg(QString("Invalid tick label '%1'").arg(value));

      bool ok;
      long i = CQChartsUtil::toInt(strs[0], ok);
      if (! ok) return errorMsg(QString("Invalid tick label position '%1'").arg(strs[0]));

      axisAnnotation->axis()->setTickLabel(i, strs[1]);
    }
    else if (name == "fit") {
      auto *annotationGroup = dynamic_cast<CQChartsAnnotationGroup *>(annotation);

      if (! annotationGroup)
        return errorMsg("Invalid group annotation");

      annotationGroup->doLayout();
    }
    else if (name == "?") {
      NameValueMap nameValues; nameValues["annotation"] = "";

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid annotation name '" + name + "' specified");
  }
  else {
    if      (name == "path_list") {
      // split into path strings
      QStringList strs;

      if (! CQTcl::splitList(value, strs))
        return errorMsg(QString("Invalid path list '%1'").arg(value));

      charts->setPathList(strs);
    }
    else if (name == "view_key") {
      bool ok;
      bool b = CQChartsUtil::stringToBool(value, &ok);

      charts->setViewKey(b);
    }
    else if (name == "max_symbol_size") {
      bool ok;
      double r = CQChartsUtil::toReal(value, ok);

      charts->setMaxSymbolSize(r);
    }
    else if (name == "max_font_size") {
      bool ok;
      double r = CQChartsUtil::toReal(value, ok);

      charts->setMaxFontSize(r);
    }
    else if (name == "max_line_width") {
      bool ok;
      double r = CQChartsUtil::toReal(value, ok);

      charts->setMaxLineWidth(r);
    }
    else if (name == "?") {
      NameValueMap nameValues;

      return cmdBase_->setCmdRc(getArgValues("name", nameValues));
    }
    else
      return errorMsg("Invalid global name '" + name + "' specified");
  }

  return true;
}

//------

// print_charts_variant command

void
CQChartsPrintChartsVariantCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-type" , ArgType::String, "variant type");
  addArg(argv, "-value", ArgType::String, "variant string");
}

QStringList
CQChartsPrintChartsVariantCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsPrintChartsVariantCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsPrintChartsVariantCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto type  = argv.getParseStr("type");
  auto value = argv.getParseStr("value");

  CQChartsMetaObj metaObj;

  if (! CQUtil::setProperty(&metaObj, type, value))
    return false;

  metaObj.printType(std::cout, type);

  return true;
}

//------

// execute_charts_slot command

void
CQChartsExecuteChartsSlotCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-name", ArgType::String, "slot name");
  addArg(argv, "-args", ArgType::String, "args");
}

QStringList
CQChartsExecuteChartsSlotCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  // TODO: name
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

// set charts data
bool
CQChartsExecuteChartsSlotCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsExecuteChartsSlotCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  auto name = argv.getParseStr ("name");
  auto args = argv.getParseStrs("args");

  //---

  // view slot
  if      (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    auto *view = cmds()->getViewByName(viewName);
    if (! view) return false;

    if (name == "?") {
      auto names = view->getSlotNames();
      return cmdBase_->setCmdRc(names);
    }

    QVariant res;

    if (! view->executeSlot(name, args, res))
      return errorMsg("Failed to execute slot '" + name + "'");

    return cmdBase_->setCmdRc(res);
  }
  // plot slot
  else if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    CQChartsView *view = nullptr;

    auto *plot = cmds()->getPlotByName(view, plotName);
    if (! plot) return false;

    if (name == "?") {
      auto names = plot->getSlotNames();
      return cmdBase_->setCmdRc(names);
    }

    QVariant res;

    if (! plot->executeSlot(name, args, res))
      return errorMsg("Failed to execute slot '" + name + "'");

    return cmdBase_->setCmdRc(res);
  }

  return true;
}

//------

// create_charts_annotation_group command

void
CQChartsCreateChartsAnnotationGroupCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsAnnotationGroupCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsAnnotationGroupCmd::
execCmd(CQChartsCmdArgs &argv)
{
#if 0
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };
#endif

  //---

  CQPerfTrace trace("CQChartsCreateChartsAnnotationGroupCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  CQChartsAnnotationGroup *annotation = nullptr;

  if      (plot)
    annotation = plot->addAnnotationGroup();
  else if (view)
    annotation = view->addAnnotationGroup();
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_annotation command

void
CQChartsCreateChartsAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-type", ArgType::String, "annotation type").setRequired();
}

QStringList
CQChartsCreateChartsAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  auto typeStr = argv.getParseStr("type");

  auto type = CQChartsAnnotation::stringToType(typeStr);

  if (type == CQChartsAnnotation::Type::NONE)
    return errorMsg(QString("Invalid type '%1'").arg(typeStr));

  auto type1 = static_cast<CQChartsAnnotationType>(type);

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  CQChartsAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addAnnotation(type1);
  else if (view)
    annotation = view->addAnnotation(type1);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_arc_annotation command

void
CQChartsCreateChartsArcAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-start", ArgType::ObjRefPos, "start position");
  addArg(argv, "-end"  , ArgType::ObjRefPos, "end position");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsArcAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsArcAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsArcAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  auto start = argv.getParseObjRefPos(view, plot, "start");
  auto end   = argv.getParseObjRefPos(view, plot, "end"  );

  if (! start.isValid() || ! end.isValid())
    return errorMsg("Invalid start/end");

  //if (start == end)
  //  return errorMsg("Arc has zero length");

  //---

  CQChartsArcAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addArcAnnotation(start, end);
  else if (view)
    annotation = view->addArcAnnotation(start, end);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_arc_connector_annotation command

void
CQChartsCreateChartsArcConnectorAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-plot", ArgType::String, "plot name");

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-center", ArgType::ObjRefPos, "center");
  addArg(argv, "-radius", ArgType::Length   , "radius");

  addArg(argv, "-src_start_angle", ArgType::String, "source start angle");
  addArg(argv, "-src_span_angle" , ArgType::String, "source span angle");

  addArg(argv, "-dest_start_angle", ArgType::String, "destination start angle");
  addArg(argv, "-dest_span_angle" , ArgType::String, "destination span angle");

  addArg(argv, "-self", ArgType::Boolean, "connects to self");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsArcConnectorAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsArcConnectorAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  //auto errorMsg = [&](const QString &msg) { charts()->errorMsg(msg); return false; };

  //---

  CQPerfTrace trace("CQChartsCreateChartsArcConnectorAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getPlotArg(argv, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  auto center = argv.getParseObjRefPos(nullptr, plot, "center");
  auto radius = argv.getParseLength   (nullptr, plot, "radius");

  auto srcStartAngle = argv.getParseAngle("src_start_angle");
  auto srcSpanAngle  = argv.getParseAngle("src_span_angle");

  auto destStartAngle = argv.getParseAngle("dest_start_angle");
  auto destSpanAngle  = argv.getParseAngle("dest_span_angle");

  auto self = argv.getParseBool("self");

  //---

  auto *annotation =
    plot->addArcConnectorAnnotation(center, radius, srcStartAngle, srcSpanAngle,
                                    destStartAngle, destSpanAngle, self);

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_arrow_annotation command

void
CQChartsCreateChartsArrowAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-start", ArgType::ObjRefPos, "start position");
  addArg(argv, "-end"  , ArgType::ObjRefPos, "end position");

  addArg(argv, "-line_width", ArgType::Length, "connecting line width");

  addArg(argv, "-fhead"   , ArgType::String, "start arrow head type");
  addArg(argv, "-thead"   , ArgType::String, "end arrow head type");
  addArg(argv, "-mid_head", ArgType::String, "mid arrow head type");

  addArg(argv, "-angle", ArgType::String, "arrow head angle");
  addArg(argv, "-back_angle", ArgType::String, "arrow head back angle").setHidden();
  addArg(argv, "-length", ArgType::String, "arrow head length");
  addArg(argv, "-line_ends", ArgType::String, "use line for arrow head").setHidden();

  addArg(argv, "-filled"    , ArgType::SBool, "background is filled" ).setHidden();
  addArg(argv, "-fill_color", ArgType::Color, "background fill color").setHidden();

  addArg(argv, "-stroked"     , ArgType::SBool , "border is stroked"  ).setHidden();
  addArg(argv, "-stroke_color", ArgType::Color , "border stroke color").setHidden();
  addArg(argv, "-stroke_width", ArgType::Length, "border stroke width").setHidden();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsArrowAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsArrowAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsArrowAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  auto start = argv.getParseObjRefPos(view, plot, "start");
  auto end   = argv.getParseObjRefPos(view, plot, "end"  );

  if (! start.isValid() || ! end.isValid())
    return errorMsg("Invalid start/end");

  //---

  CQChartsArrowData arrowData;

  arrowData.setLineWidth(argv.getParseLength(view, plot, "line_width", arrowData.lineWidth()));

  if (argv.hasParseArg("fhead")) {
    CQChartsArrowData::HeadType headType { CQChartsArrowData::HeadType::NONE };
    bool                        lineEnds { false };
    bool                        visible  { false };

    if (CQChartsArrowData::nameToData(argv.getParseStr("fhead"), headType, lineEnds, visible)) {
      arrowData.setFHead        (visible);
      arrowData.setFHeadType    (headType);
      arrowData.setFrontLineEnds(lineEnds);
    }
  }

  if (argv.hasParseArg("thead")) {
    bool                        visible  { false };
    CQChartsArrowData::HeadType headType { CQChartsArrowData::HeadType::NONE };
    bool                        lineEnds { false };

    if (CQChartsArrowData::nameToData(argv.getParseStr("thead"), headType, lineEnds, visible)) {
      arrowData.setTHead       (visible);
      arrowData.setTHeadType   (headType);
      arrowData.setTailLineEnds(lineEnds);
    }
  }

  if (argv.hasParseArg("midHead")) {
    bool                        visible  { false };
    CQChartsArrowData::HeadType headType { CQChartsArrowData::HeadType::NONE };
    bool                        lineEnds { false };

    if (CQChartsArrowData::nameToData(argv.getParseStr("midHead"), headType, lineEnds, visible)) {
      arrowData.setMidHead    (visible);
      arrowData.setMidHeadType(headType);
      arrowData.setMidLineEnds(lineEnds);
    }
  }

  // single value (common head & tail angle), two values (separate head & tail angle),
  // three values (separate head, tail and mid angle)
  if (argv.hasParseArg("angle")) {
    // split into arrow angles
    QStringList strs;

    if (! CQTcl::splitList(argv.getParseStr("angle"), strs))
      return errorMsg(QString("Invalid angle string '%1'").arg(argv.getParseStr("angle")));

    if      (strs.length() == 1) {
      bool ok;
      double angle = CQChartsUtil::toReal(strs[0], ok);
      if (! ok) return errorMsg(QString("Invalid angle string '%1'").arg(strs[0]));
      if (angle > 0) arrowData.setAngle(CQChartsAngle(angle));
    }
    else if (strs.length() == 2) {
      bool ok1, ok2;
      double angle1 = CQChartsUtil::toReal(strs[0], ok1);
      double angle2 = CQChartsUtil::toReal(strs[1], ok2);
      if (! ok1 || ! ok2) return errorMsg(QString("Invalid angle strings '%1' '%2'").
                                           arg(strs[0]).arg(strs[1]));

      if (angle1 > 0) arrowData.setFrontAngle(CQChartsAngle(angle1));
      if (angle2 > 0) arrowData.setTailAngle (CQChartsAngle(angle2));
    }
    else if (strs.length() == 3) {
      bool ok1, ok2, ok3;
      double angle1 = CQChartsUtil::toReal(strs[0], ok1);
      double angle2 = CQChartsUtil::toReal(strs[1], ok2);
      double angle3 = CQChartsUtil::toReal(strs[2], ok3);
      if (! ok1 || ! ok2 || ! ok3)
        return errorMsg(QString("Invalid angle strings '%1' '%2' '%3'").
                          arg(strs[0]).arg(strs[1]).arg(strs[2]));

      if (angle1 > 0) arrowData.setFrontAngle(CQChartsAngle(angle1));
      if (angle2 > 0) arrowData.setTailAngle (CQChartsAngle(angle2));
      if (angle3 > 0) arrowData.setMidAngle  (CQChartsAngle(angle3));
    }
    else
      return errorMsg(QString("Invalid angle string '%1'").arg(argv.getParseStr("angle")));
  }

  // single value (common head & tail back angle), two values (separate head & tail back angle),
  // three values (separate head, tail and mid back angle)
  if (argv.hasParseArg("back_angle")) {
    // split into arrow angles
    QStringList strs;

    if (! CQTcl::splitList(argv.getParseStr("back_angle"), strs))
      return errorMsg(QString("Invalid back_angle string '%1'").
                       arg(argv.getParseStr("back_angle")));

    if      (strs.length() == 1) {
      bool ok; double angle = CQChartsUtil::toReal(strs[0], ok);
      if (! ok) return errorMsg(QString("Invalid back_angle string '%1'").arg(strs[0]));
      if (angle > 0) arrowData.setBackAngle(CQChartsAngle(angle));
    }
    else if (strs.length() == 2) {
      bool ok1; double angle1 = CQChartsUtil::toReal(strs[0], ok1);
      bool ok2; double angle2 = CQChartsUtil::toReal(strs[1], ok2);
      if (! ok1 || ! ok2) return errorMsg(QString("Invalid back_angle strings '%1' '%2'").
                                           arg(strs[0]).arg(strs[1]));

      if (angle1 > 0) arrowData.setFrontBackAngle(CQChartsAngle(angle1));
      if (angle2 > 0) arrowData.setTailBackAngle (CQChartsAngle(angle2));
    }
    else if (strs.length() == 3) {
      bool ok1; double angle1 = CQChartsUtil::toReal(strs[0], ok1);
      bool ok2; double angle2 = CQChartsUtil::toReal(strs[1], ok2);
      bool ok3; double angle3 = CQChartsUtil::toReal(strs[2], ok3);
      if (! ok1 || ! ok2 || ! ok3)
        return errorMsg(QString("Invalid back_angle strings '%1' '%2' '%3'").
                          arg(strs[0]).arg(strs[1]));

      if (angle1 > 0) arrowData.setFrontBackAngle(CQChartsAngle(angle1));
      if (angle2 > 0) arrowData.setTailBackAngle (CQChartsAngle(angle2));
      if (angle3 > 0) arrowData.setMidBackAngle  (CQChartsAngle(angle3));
    }
    else
      return errorMsg(QString("Invalid back_angle string '%1'").
                       arg(argv.getParseStr("back_angle")));
  }

  // single value (common head & tail length), two values (separate head & tail length),
  // three values (separate head, tail and mid length)
  if (argv.hasParseArg("length")) {
    // split into arrow lengths
    QStringList strs;

    if (! CQTcl::splitList(argv.getParseStr("length"), strs))
      return errorMsg(QString("Invalid length string '%1'").arg(argv.getParseStr("length")));

    if      (strs.length() == 1) {
      auto len = CQChartsLength(strs[0], (view ? CQChartsUnits::Type::VIEW :
                                                 CQChartsUnits::Type::PLOT));
      if (! len.isValid()) return errorMsg(QString("Invalid length string '%1'").arg(strs[0]));

      if (len.value() > 0) arrowData.setLength(len);
    }
    else if (strs.length() == 2) {
      auto len1 = CQChartsLength(strs[0], (view ? CQChartsUnits::Type::VIEW :
                                                  CQChartsUnits::Type::PLOT));
      auto len2 = CQChartsLength(strs[1], (view ? CQChartsUnits::Type::VIEW :
                                                  CQChartsUnits::Type::PLOT));
      if (! len1.isValid() || ! len2.isValid())
        return errorMsg(QString("Invalid length strings '%1' '%2'").arg(strs[0]).arg(strs[1]));

      if (len1.value() > 0) arrowData.setFrontLength(len1);
      if (len2.value() > 0) arrowData.setTailLength (len2);
    }
    else if (strs.length() == 3) {
      auto len1 = CQChartsLength(strs[0], (view ? CQChartsUnits::Type::VIEW :
                                                  CQChartsUnits::Type::PLOT));
      auto len2 = CQChartsLength(strs[1], (view ? CQChartsUnits::Type::VIEW :
                                                  CQChartsUnits::Type::PLOT));
      auto len3 = CQChartsLength(strs[2], (view ? CQChartsUnits::Type::VIEW :
                                                  CQChartsUnits::Type::PLOT));
      if (! len1.isValid() || ! len2.isValid() || ! len3.isValid())
        return errorMsg(QString("Invalid length strings '%1' '%2' '%3'").
                          arg(strs[0]).arg(strs[1]).arg(strs[2]));

      if (len1.value() > 0) arrowData.setFrontLength(len1);
      if (len2.value() > 0) arrowData.setTailLength (len2);
      if (len3.value() > 0) arrowData.setTailLength (len3);
    }
    else
      return errorMsg(QString("Invalid length string '%1'").arg(argv.getParseStr("length")));
  }

  // single value (common head & tail line ends), two values (separate head & tail line ends)
  if (argv.hasParseArg("line_ends")) {
    // split into line ends
    QStringList strs;

    if (! CQTcl::splitList(argv.getParseStr("line_ends"), strs))
      return errorMsg(QString("Invalid line_ends string '%1'").arg(argv.getParseStr("line_ends")));

    if      (strs.length() == 1) {
      bool ok; bool b = CQChartsCmdBaseArgs::stringToBool(strs[0], &ok);
      if (! ok) return errorMsg(QString("Invalid line_ends string '%1'").arg(strs[0]));
      arrowData.setLineEnds(b);
    }
    else if (strs.length() == 2) {
      bool ok1; bool b1 = CQChartsCmdBaseArgs::stringToBool(strs[0], &ok1);
      bool ok2; bool b2 = CQChartsCmdBaseArgs::stringToBool(strs[1], &ok2);
      if (! ok1 || ! ok2)
        return errorMsg(QString("Invalid line_ends strings '%1' '%2'").
                          arg(strs[0]).arg(strs[1]));

      arrowData.setFrontLineEnds(b1);
      arrowData.setTailLineEnds (b2);
    }
    else if (strs.length() == 3) {
      bool ok1; bool b1 = CQChartsCmdBaseArgs::stringToBool(strs[0], &ok1);
      bool ok2; bool b2 = CQChartsCmdBaseArgs::stringToBool(strs[1], &ok2);
      bool ok3; bool b3 = CQChartsCmdBaseArgs::stringToBool(strs[2], &ok3);
      if (! ok1 || ! ok2 || ! ok3)
        return errorMsg(QString("Invalid line_ends strings '%1' '%2' '%3'").
                          arg(strs[0]).arg(strs[1]).arg(strs[2]));

      arrowData.setFrontLineEnds(b1);
      arrowData.setTailLineEnds (b2);
      arrowData.setMidLineEnds  (b3);
    }
    else
      return errorMsg(QString("Invalid line_ends string '%1'").arg(argv.getParseStr("line_ends")));
  }

  //---

  CQChartsShapeData shapeData;

  auto &fill   = shapeData.fill();
  auto &stroke = shapeData.stroke();

  fill  .setVisible(true);
  stroke.setVisible(false);

  if (argv.hasParseArg("filled")) {
    fill.setVisible(argv.getParseBool("filled", fill.isVisible()));

    if (! fill.isVisible())
      stroke.setVisible(true);
  }

  fill.setColor(argv.getParseColor("fill_color", fill.color()));

  if (argv.hasParseArg("stroked"))
    stroke.setVisible(argv.getParseBool("stroked", stroke.isVisible()));

  stroke.setColor(argv.getParseColor ("stroke_color", stroke.color()));
  stroke.setWidth(argv.getParseLength(view, plot, "stroke_width", stroke.width()));

  //---

  if (start == end)
    return errorMsg("Arrow is zero length");

  CQChartsArrowAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addArrowAnnotation(start, end);
  else if (view)
    annotation = view->addArrowAnnotation(start, end);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setArrowData(arrowData);

  annotation->arrow()->setShapeData(shapeData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_axis_annotation command

void
CQChartsCreateChartsAxisAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-plot", ArgType::String, "plot name").setRequired();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-start", ArgType::Real, "start");
  addArg(argv, "-end"  , ArgType::Real, "end");

  addArg(argv, "-position", ArgType::Real, "position");

  addArg(argv, "-direction", ArgType::String, "Direction");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsAxisAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsAxisAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsAxisAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getPlotArg(argv, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  double start = argv.getParseReal("start");
  double end   = argv.getParseReal("end"  );

  double position = argv.getParseReal("position", 0.0);

  auto directionStr = argv.getParseStr("direction", "horizontal").toLower();

  Qt::Orientation direction = Qt::Horizontal;

  if      (directionStr == "h" || directionStr == "horiz" || directionStr == "horizontal")
    direction = Qt::Horizontal;
  else if (directionStr == "v" || directionStr == "vert"  || directionStr == "vertical"  )
    direction = Qt::Vertical;

  //---

  if (start == end)
    return errorMsg("Axis is zero length");

  CQChartsAxisAnnotation *annotation = nullptr;

  if (plot)
    annotation = plot->addAxisAnnotation(direction, start, end);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setPosition(position);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_button_annotation command

void
CQChartsCreateChartsButtonAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-position", ArgType::ObjRefPos, "position");

  addArg(argv, "-text", ArgType::String, "text");

  addArg(argv, "-font" , ArgType::String, "font");
  addArg(argv, "-color", ArgType::Color , "color");
  addArg(argv, "-alpha", ArgType::Real  , "alpha");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsButtonAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsButtonAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsButtonAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  auto text = argv.getParseStr("text", "Annotation");

  CQChartsTextData textData;

  textData.setFont (argv.getParseFont ("font" , textData.font      ()));
  textData.setColor(argv.getParseColor("color", textData.color     ()));
  textData.setAlpha(argv.getParseAlpha("alpha", textData.alpha     ()));

  //---

  CQChartsButtonAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("position")) {
    auto pos = argv.getParseObjRefPos(view, plot, "position");

    if      (plot)
      annotation = plot->addButtonAnnotation(pos, text);
    else if (view)
      annotation = view->addButtonAnnotation(pos, text);
  }
  else {
    auto pos = CQChartsObjRefPos::plot(CQChartsGeom::Point(0, 0));

    if      (plot)
      annotation = plot->addButtonAnnotation(pos, text);
    else if (view)
      annotation = view->addButtonAnnotation(pos, text);
  }

  if (! annotation)
    return errorMsg("Failed to create annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setTextData(textData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_ellipse_annotation command

void
CQChartsCreateChartsEllipseAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-center", ArgType::ObjRefPos, "center");

  addArg(argv, "-rx", ArgType::Length, "x radius");
  addArg(argv, "-ry", ArgType::Length, "y radius");

  addArg(argv, "-filled"      , ArgType::SBool , "background is filled"   ).setHidden();
  addArg(argv, "-fill_color"  , ArgType::Color , "background fill color"  ).setHidden();
  addArg(argv, "-fill_alpha"  , ArgType::Real  , "background fill alpha"  ).setHidden();
  addArg(argv, "-fill_pattern", ArgType::String, "background fill pattern").setHidden();

  addArg(argv, "-stroked"     , ArgType::SBool   , "border is stroked"  ).setHidden();
  addArg(argv, "-stroke_color", ArgType::Color   , "border stroke color").setHidden();
  addArg(argv, "-stroke_alpha", ArgType::Real    , "border stroke alpha").setHidden();
  addArg(argv, "-stroke_width", ArgType::Length  , "border stroke width").setHidden();
  addArg(argv, "-stroke_dash" , ArgType::LineDash, "border stroke dash" ).setHidden();

  addArg(argv, "-corner_size" , ArgType::Length, "corner size" ).setHidden();
  addArg(argv, "-border_sides", ArgType::Sides , "border sides").setHidden();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsEllipseAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsEllipseAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsEllipseAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  CQChartsBoxData boxData;

  auto &fill   = boxData.shape().fill();
  auto &stroke = boxData.shape().stroke();

  stroke.setVisible(true);

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  auto center = argv.getParseObjRefPos(view, plot, "center");

  if (! center.isValid())
    return errorMsg("Invalid ellipse center");

  //---

  auto rx = argv.getParseLength(view, plot, "rx");
  auto ry = argv.getParseLength(view, plot, "ry");

  fill.setVisible(argv.getParseBool   ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor  ("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseAlpha  ("fill_alpha"  , fill.alpha    ()));
  fill.setPattern(argv.getParsePattern("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseAlpha   ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  stroke.setCornerSize(argv.getParseLength(view, plot, "corner_size", stroke.cornerSize()));

  boxData.setBorderSides(argv.getParseSides("border_sides", boxData.borderSides()));

  //---

  if (! rx.isValid() || ! ry.isValid())
    return errorMsg("Invalid ellipse radius");

  if (rx.value() <= 0.0 || ry.value() <= 0.0)
    return errorMsg("Invalid ellipse radius");

  CQChartsEllipseAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addEllipseAnnotation(center, rx, ry);
  else if (view)
    annotation = view->addEllipseAnnotation(center, rx, ry);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_image_annotation command

void
CQChartsCreateChartsImageAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-position" , ArgType::ObjRefPos, "position");
  addArg(argv, "-rectangle", ArgType::Rect     , "rectangle bounding box");

  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-image", ArgType::String, "image file");
  addArg(argv, "-icon" , ArgType::String, "icon file");
  addArg(argv, "-svg"  , ArgType::String, "svg file");
  argv.endCmdGroup();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsImageAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsImageAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsImageAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  // get image
  CQChartsImage image;
  QString       imageName;

  if      (argv.hasParseArg("image")) {
    imageName = argv.getParseStr("image");

    image = CQChartsImage(imageName, CQChartsImage::Type::IMAGE);
  }
  else if (argv.hasParseArg("icon")) {
    imageName = argv.getParseStr("icon");

    image = CQChartsImage(imageName, CQChartsImage::Type::ICON);
  }
  else if (argv.hasParseArg("svg")) {
    imageName = argv.getParseStr("svg");

    image = CQChartsImage(imageName, CQChartsImage::Type::SVG);
  }
  else {
    return errorMsg("Invalid image type");
  }

  if (! image.isValid())
    return errorMsg(QString("Invalid image filename '%1'").arg(imageName));

  //---

  // create annotation
  CQChartsImageAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("rectangle")) {
    auto rect = argv.getParseRect(view, plot, "rectangle");

    if (! rect.isValid())
      return errorMsg("Invalid rectangle geometry");

    if      (plot)
      annotation = plot->addImageAnnotation(rect, image);
    else if (view)
      annotation = view->addImageAnnotation(rect, image);
  }
  else {
    auto pos = CQChartsObjRefPos::plot(CQChartsGeom::Point(0, 0));

    if (argv.hasParseArg("position"))
      pos = argv.getParseObjRefPos(view, plot, "position");

    if      (plot)
      annotation = plot->addImageAnnotation(pos, image);
    else if (view)
      annotation = view->addImageAnnotation(pos, image);
  }

  if (! annotation)
    return errorMsg("Failed to create image annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_path_annotation command

void
CQChartsCreateChartsPathAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-path", ArgType::String, "path string");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsPathAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsPathAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPathAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  // get path
  auto pathStr = argv.getParseStr("path");

  CQChartsPath path(pathStr);

  if (! path.isValid())
    return errorMsg(QString("Invalid path string '%1'").arg(pathStr));

  //---

  // create annotation
  CQChartsPathAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addPathAnnotation(path);
  else if (view)
    annotation = view->addPathAnnotation(path);

  if (! annotation)
    return errorMsg("Failed to create path annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_key_annotation command

void
CQChartsCreateChartsKeyAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-column", ArgType::Column, "column for unique item values");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsKeyAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsKeyAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsKeyAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  CQChartsColumn column;

  if (argv.hasParseArg("column")) {
    if (! plot)
      return errorMsg("Plot needed for key column");

    const auto &plotModel = plot->currentModel();

    column = argv.getParseColumn("column", plotModel.data());

    if (! column.isValid())
      return errorMsg("Invalid column");
  }

  CQChartsKeyAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addKeyAnnotation(column);
  else if (view)
    annotation = view->addKeyAnnotation();
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_pie_slice_annotation command

void
CQChartsCreateChartsPieSliceAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-position", ArgType::ObjRefPos, "point position");

  addArg(argv, "-inner_radius", ArgType::Length, "inner radius");
  addArg(argv, "-outer_radius", ArgType::Length, "outer radius");

  addArg(argv, "-start_angle", ArgType::String, "start angle");
  addArg(argv, "-span_angle" , ArgType::String, "span angle");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsPieSliceAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsPieSliceAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPieSliceAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  auto pos = argv.getParseObjRefPos(view, plot, "position");

  if (! pos.isValid())
    return errorMsg("Invalid position");

  auto innerRadius = argv.getParseLength(view, plot, "inner_radius");
  auto outerRadius = argv.getParseLength(view, plot, "outer_radius");

  auto startAngle = argv.getParseAngle("start_angle");
  auto spanAngle  = argv.getParseAngle("span_angle");

  if (innerRadius.value() < 0 || outerRadius.value() < 0)
    return errorMsg("Invalid radius value");

  //---

  if (innerRadius.value() < 0.0 || outerRadius.value() <= 0.0)
    return errorMsg("Invalid pie slice radii");

  CQChartsPieSliceAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addPieSliceAnnotation(pos, innerRadius, outerRadius, startAngle, spanAngle);
  else if (view)
    annotation = view->addPieSliceAnnotation(pos, innerRadius, outerRadius, startAngle, spanAngle);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_point_annotation command

void
CQChartsCreateChartsPointAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-position", ArgType::ObjRefPos, "point position");

  addArg(argv, "-symbol", ArgType::String, "symbol");
  addArg(argv, "-size"  , ArgType::Length, "symbol size");

  addArg(argv, "-filled"    , ArgType::SBool, "symbol background is filled" ).setHidden();
  addArg(argv, "-fill_color", ArgType::Color, "symbol background fill color").setHidden();
  addArg(argv, "-fill_alpha", ArgType::Real , "symbol background fill alpha").setHidden();

  addArg(argv, "-stroked"     , ArgType::SBool , "symbol border stroke visible").setHidden();
  addArg(argv, "-stroke_color", ArgType::Color , "symbol border stroke color"  ).setHidden();
  addArg(argv, "-stroke_alpha", ArgType::Real  , "symbol border stroke alpha"  ).setHidden();
  addArg(argv, "-stroke_width", ArgType::Length, "symbol border stroke width"  ).setHidden();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsPointAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);
  else if (arg == "type") return CQChartsSymbolType::typeNames();

  return QStringList();
}

bool
CQChartsCreateChartsPointAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPointAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  CQChartsSymbolData symbolData;

  auto &fill   = symbolData.fill();
  auto &stroke = symbolData.stroke();

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  auto pos = argv.getParseObjRefPos(view, plot, "position");

  if (! pos.isValid())
    return errorMsg("Invalid position");

  //---

  auto symbolStr = argv.getParseStr("symbol");

  if (symbolStr.length()) {
    if (symbolStr == "?")
      return cmdBase_->setCmdRc(getArgValues("symbol"));

    CQChartsSymbol symbol(symbolStr);

    if (! symbol.isValid())
      return errorMsg(QString("Invalid symbol '%1'").arg(symbolStr));

    symbolData.setSymbol(symbol);
  }

  symbolData.setSize(argv.getParseLength(view, plot, "size", symbolData.size()));

  fill.setVisible(argv.getParseBool ("filled"    , fill.isVisible()));
  fill.setColor  (argv.getParseColor("fill_color", fill.color    ()));
  fill.setAlpha  (argv.getParseAlpha("fill_alpha", fill.alpha    ()));

  stroke.setVisible(argv.getParseBool  ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseAlpha ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength(view, plot, "stroke_width", stroke.width()));

  //---

  CQChartsPointAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addPointAnnotation(pos, symbolData.symbol());
  else if (view)
    annotation = view->addPointAnnotation(pos, symbolData.symbol());
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setSymbolData(symbolData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_point_set_annotation command

void
CQChartsCreateChartsPointSetAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-rectangle", ArgType::Rect, "rectangle");

  addArg(argv, "-values", ArgType::Reals, "values");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsPointSetAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsPointSetAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPointSetAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  CQChartsRect rect;

  if (argv.hasParseArg("rectangle")) {
    rect = argv.getParseRect(view, plot, "rectangle");

    if (! rect.isValid())
      return errorMsg("Invalid rectangle value");
  }
  else {
    CQChartsGeom::Point start(0, 0);
    CQChartsGeom::Point end  (1, 1);

    if      (plot)
      rect = CQChartsRect::plot(CQChartsGeom::BBox(start, end));
    else if (view)
      rect = CQChartsRect::view(CQChartsGeom::BBox(start, end));
  }

  CQChartsPoints values;

  if (argv.hasParseArg("values")) {
    values = argv.getParsePoints(view, plot, "values");

    if (values.points().empty())
      return errorMsg("Invalid point values");
  }

  //---

  CQChartsPointSetAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addPointSetAnnotation(rect, values);
  else if (view)
    annotation = view->addPointSetAnnotation(rect, values);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_point3d_set_annotation command

void
CQChartsCreateChartsPoint3DSetAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-plot", ArgType::String, "plot name");

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-points", ArgType::String, "points");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsPoint3DSetAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsPoint3DSetAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPoint3DSetAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getPlotArg(argv, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  auto pointsStr = argv.getParseStr("points");

  // split into points
  QStringList pointStrs;

  if (! CQTcl::splitList(pointsStr, pointStrs))
    return errorMsg(QString("Invalid points '%1'").arg(pointsStr));

  std::vector<CQChartsGeom::Point3D> points;

  for (const auto &str : pointStrs) {
    CQChartsGeom::Point3D p;

    if (! CQChartsUtil::stringToPoint3D(str, p))
      return errorMsg(QString("Invalid point '%1'").arg(str));

    points.push_back(p);
  }

  //---

  CQChartsPoint3DSetAnnotation *annotation = nullptr;

  if (plot)
    annotation = plot->addPoint3DSetAnnotation(points);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_polygon_annotation command

void
CQChartsCreateChartsPolygonAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-points", ArgType::Polygon, "points string").setRequired();

  addArg(argv, "-filled"      , ArgType::SBool , "background is filled"   ).setHidden();
  addArg(argv, "-fill_color"  , ArgType::Color , "background fill color"  ).setHidden();
  addArg(argv, "-fill_alpha"  , ArgType::Real  , "background fill alpha"  ).setHidden();
  addArg(argv, "-fill_pattern", ArgType::String, "background fill pattern").setHidden();

  addArg(argv, "-stroked"     , ArgType::SBool   , "border is stroked"  ).setHidden();
  addArg(argv, "-stroke_color", ArgType::Color   , "border stroke color").setHidden();
  addArg(argv, "-stroke_alpha", ArgType::Real    , "border stroke alpha").setHidden();
  addArg(argv, "-stroke_width", ArgType::Length  , "border stroke width").setHidden();
  addArg(argv, "-stroke_dash" , ArgType::LineDash, "border stroke dash" ).setHidden();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsPolygonAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsPolygonAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPolygonAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  CQChartsBoxData boxData;

  auto &shapeData = boxData.shape();

  auto &fill   = shapeData.fill();
  auto &stroke = shapeData.stroke();

  stroke.setVisible(true);

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  auto points = argv.getParsePoly("points");

  fill.setVisible(argv.getParseBool   ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor  ("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseAlpha  ("fill_alpha"  , fill.alpha    ()));
  fill.setPattern(argv.getParsePattern("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseAlpha   ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  //---

  if (points.size() < 3)
    return errorMsg("Not enough points");

  //---

  CQChartsPolygonAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addPolygonAnnotation(CQChartsGeom::Polygon(points));
  else if (view)
    annotation = view->addPolygonAnnotation(CQChartsGeom::Polygon(points));
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_polyline_annotation command

void
CQChartsCreateChartsPolylineAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-points", ArgType::Polygon, "points string").setRequired();

//addArg(argv, "-filled"      , ArgType::SBool , "background is filled"   ).setHidden();
//addArg(argv, "-fill_color"  , ArgType::Color , "background fill color"  ).setHidden();
//addArg(argv, "-fill_alpha"  , ArgType::Real  , "background fill alpha"  ).setHidden();
//addArg(argv, "-fill_pattern", ArgType::String, "background fill pattern").setHidden();

  addArg(argv, "-stroked"     , ArgType::SBool   , "border is stroked"  ).setHidden();
  addArg(argv, "-stroke_color", ArgType::Color   , "border stroke color").setHidden();
  addArg(argv, "-stroke_alpha", ArgType::Real    , "border stroke alpha").setHidden();
  addArg(argv, "-stroke_width", ArgType::Length  , "border stroke width").setHidden();
  addArg(argv, "-stroke_dash" , ArgType::LineDash, "border stroke dash" ).setHidden();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsPolylineAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsPolylineAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsPolylineAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  CQChartsBoxData boxData;

  auto &shapeData = boxData.shape();

//auto &fill   = shapeData.fill();
  auto &stroke = shapeData.stroke();

  stroke.setVisible(true);

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  auto points = argv.getParsePoly("points");

//fill.setVisible(argv.getParseBool   ("filled"      , fill.isVisible()));
//fill.setColor  (argv.getParseColor  ("fill_color"  , fill.color    ()));
//fill.setAlpha  (argv.getParseAlpha  ("fill_alpha"  , fill.alpha    ()));
//fill.setPattern(argv.getParsePattern("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseAlpha   ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  //---

  if (points.size() < 2)
    return errorMsg("No points");

  //---

  CQChartsPolylineAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addPolylineAnnotation(points);
  else if (view)
    annotation = view->addPolylineAnnotation(points);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_rectangle_annotation command

void
CQChartsCreateChartsRectangleAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-rectangle" , ArgType::Rect, "rectangle bounding box");

  addArg(argv, "-start", ArgType::ObjRefPos, "start").setHidden();
  addArg(argv, "-end"  , ArgType::ObjRefPos, "end"  ).setHidden();

  addArg(argv, "-margin" , ArgType::Real, "margin" ).setHidden();
  addArg(argv, "-padding", ArgType::Real, "padding").setHidden();

  addArg(argv, "-filled"      , ArgType::SBool , "background is filled"   ).setHidden();
  addArg(argv, "-fill_color"  , ArgType::Color , "background fill color"  ).setHidden();
  addArg(argv, "-fill_alpha"  , ArgType::Real  , "background fill alpha"  ).setHidden();
  addArg(argv, "-fill_pattern", ArgType::String, "background fill pattern").setHidden();

  addArg(argv, "-stroked"     , ArgType::SBool   , "border is stroked"  ).setHidden();
  addArg(argv, "-stroke_color", ArgType::Color   , "border stroke color").setHidden();
  addArg(argv, "-stroke_alpha", ArgType::Real    , "border stroke alpha").setHidden();
  addArg(argv, "-stroke_width", ArgType::Length  , "border stroke width").setHidden();
  addArg(argv, "-stroke_dash" , ArgType::LineDash, "border stroke dash" ).setHidden();

  addArg(argv, "-corner_size" , ArgType::Length, "corner size" ).setHidden();
  addArg(argv, "-border_sides", ArgType::Sides , "border sides").setHidden();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsRectangleAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsRectangleAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsRectangleAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  CQChartsBoxData boxData;

  boxData.setMargin (CQChartsMargin());
  boxData.setPadding(CQChartsMargin());

  auto &fill   = boxData.shape().fill  ();
  auto &stroke = boxData.shape().stroke();

  stroke.setVisible(true);

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  boxData.setMargin (argv.getParseMargin(view, plot, "margin" , boxData.margin ()));
  boxData.setPadding(argv.getParseMargin(view, plot, "padding", boxData.padding()));

  fill.setVisible(argv.getParseBool   ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor  ("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseAlpha  ("fill_alpha"  , fill.alpha    ()));
  fill.setPattern(argv.getParsePattern("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseAlpha   ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  stroke.setCornerSize(argv.getParseLength(view, plot, "corner_size", stroke.cornerSize()));

  boxData.setBorderSides(argv.getParseSides("border_sides", boxData.borderSides()));

  //---

  CQChartsRectangleAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("start") || argv.hasParseArg("end")) {
    auto start = argv.getParseObjRefPos(view, plot, "start");
    auto end   = argv.getParseObjRefPos(view, plot, "end"  );

    auto rect = CQChartsViewPlotObj::makeRect(view, plot, start.position(), end.position());

    if (! rect.isValid())
      return errorMsg("Invalid rectangle geometry");

    if      (plot)
      annotation = plot->addRectangleAnnotation(rect);
    else if (view)
      annotation = view->addRectangleAnnotation(rect);
  }
  else if (argv.hasParseArg("rectangle")) {
    auto rect = argv.getParseRect(view, plot, "rectangle");

    if (! rect.isValid())
      return errorMsg("Invalid rectangle geometry");

    if      (plot)
      annotation = plot->addRectangleAnnotation(rect);
    else if (view)
      annotation = view->addRectangleAnnotation(rect);
  }
  else {
    CQChartsGeom::Point start(0, 0);
    CQChartsGeom::Point end  (1, 1);

    CQChartsRect rect;

    if      (plot)
      rect = CQChartsRect::plot(CQChartsGeom::BBox(start, end));
    else if (view)
      rect = CQChartsRect::view(CQChartsGeom::BBox(start, end));

    if      (plot)
      annotation = plot->addRectangleAnnotation(rect);
    else if (view)
      annotation = view->addRectangleAnnotation(rect);
  }

  if (! annotation)
    return errorMsg("Failed to create annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_shape_annotation command

void
CQChartsCreateChartsShapeAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-rectangle" , ArgType::Rect, "rectangle bounding box");

  addArg(argv, "-start", ArgType::ObjRefPos, "start").setHidden();
  addArg(argv, "-end"  , ArgType::ObjRefPos, "end"  ).setHidden();

  addArg(argv, "-margin" , ArgType::Real, "margin" ).setHidden();
  addArg(argv, "-padding", ArgType::Real, "padding").setHidden();

  addArg(argv, "-filled"      , ArgType::SBool , "background is filled"   ).setHidden();
  addArg(argv, "-fill_color"  , ArgType::Color , "background fill color"  ).setHidden();
  addArg(argv, "-fill_alpha"  , ArgType::Real  , "background fill alpha"  ).setHidden();
  addArg(argv, "-fill_pattern", ArgType::String, "background fill pattern").setHidden();

  addArg(argv, "-stroked"     , ArgType::SBool   , "border is stroked"  ).setHidden();
  addArg(argv, "-stroke_color", ArgType::Color   , "border stroke color").setHidden();
  addArg(argv, "-stroke_alpha", ArgType::Real    , "border stroke alpha").setHidden();
  addArg(argv, "-stroke_width", ArgType::Length  , "border stroke width").setHidden();
  addArg(argv, "-stroke_dash" , ArgType::LineDash, "border stroke dash" ).setHidden();

  addArg(argv, "-corner_size" , ArgType::Length, "corner size" ).setHidden();
  addArg(argv, "-border_sides", ArgType::Sides , "border sides").setHidden();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsShapeAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsShapeAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsShapeAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  CQChartsBoxData boxData;

  boxData.setMargin (CQChartsMargin());
  boxData.setPadding(CQChartsMargin());

  auto &fill   = boxData.shape().fill  ();
  auto &stroke = boxData.shape().stroke();

  stroke.setVisible(true);

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  boxData.setMargin (argv.getParseMargin(view, plot, "margin" , boxData.margin ()));
  boxData.setPadding(argv.getParseMargin(view, plot, "padding", boxData.padding()));

  fill.setVisible(argv.getParseBool   ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor  ("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseAlpha  ("fill_alpha"  , fill.alpha    ()));
  fill.setPattern(argv.getParsePattern("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseAlpha   ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  stroke.setCornerSize(argv.getParseLength(view, plot, "corner_size", stroke.cornerSize()));

  boxData.setBorderSides(argv.getParseSides("border_sides", boxData.borderSides()));

  //---

  CQChartsShapeAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("start") || argv.hasParseArg("end")) {
    auto start = argv.getParseObjRefPos(view, plot, "start");
    auto end   = argv.getParseObjRefPos(view, plot, "end"  );

    auto rect = CQChartsViewPlotObj::makeRect(view, plot, start.position(), end.position());

    if (! rect.isValid())
      return errorMsg("Invalid rectangle geometry");

    if      (plot)
      annotation = plot->addShapeAnnotation(rect);
    else if (view)
      annotation = view->addShapeAnnotation(rect);
  }
  else if (argv.hasParseArg("rectangle")) {
    auto rect = argv.getParseRect(view, plot, "rectangle");

    if (! rect.isValid())
      return errorMsg("Invalid rectangle geometry");

    if      (plot)
      annotation = plot->addShapeAnnotation(rect);
    else if (view)
      annotation = view->addShapeAnnotation(rect);
  }
  else {
    CQChartsGeom::Point start(0, 0);
    CQChartsGeom::Point end  (1, 1);

    CQChartsRect rect;

    if      (plot)
      rect = CQChartsRect::plot(CQChartsGeom::BBox(start, end));
    else if (view)
      rect = CQChartsRect::view(CQChartsGeom::BBox(start, end));

    if      (plot)
      annotation = plot->addShapeAnnotation(rect);
    else if (view)
      annotation = view->addShapeAnnotation(rect);
  }

  if (! annotation)
    return errorMsg("Failed to create annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_text_annotation command

void
CQChartsCreateChartsTextAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-position" , ArgType::ObjRefPos, "position");
  addArg(argv, "-rectangle", ArgType::Rect     , "rectangle bounding box");

  addArg(argv, "-text", ArgType::String, "text");

  addArg(argv, "-font"    , ArgType::String , "font");
  addArg(argv, "-color"   , ArgType::Color  , "color");
  addArg(argv, "-alpha"   , ArgType::Real   , "alpha");
  addArg(argv, "-angle"   , ArgType::String , "angle");
  addArg(argv, "-contrast", ArgType::SBool  , "contrast");
  addArg(argv, "-align"   , ArgType::Align  , "align string");
  addArg(argv, "-html"    , ArgType::Boolean, "html text");

  addArg(argv, "-filled"      , ArgType::SBool , "background is filled"   ).setHidden();
  addArg(argv, "-fill_color"  , ArgType::Color , "background fill color"  ).setHidden();
  addArg(argv, "-fill_alpha"  , ArgType::Real  , "background fill alpha"  ).setHidden();
//addArg(argv, "-fill_pattern", ArgType::String, "background fill pattern").setHidden();

  addArg(argv, "-stroked"     , ArgType::SBool   , "border is stroked"  ).setHidden();
  addArg(argv, "-stroke_color", ArgType::Color   , "border stroke color").setHidden();
  addArg(argv, "-stroke_alpha", ArgType::Real    , "border stroke alpha").setHidden();
  addArg(argv, "-stroke_width", ArgType::Length  , "border stroke width").setHidden();
  addArg(argv, "-stroke_dash" , ArgType::LineDash, "border stroke dash" ).setHidden();

  addArg(argv, "-corner_size" , ArgType::Length, "corner size" ).setHidden();
  addArg(argv, "-border_sides", ArgType::Sides , "border sides").setHidden();

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsTextAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsTextAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsTextAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  CQChartsTextData textData;
  CQChartsBoxData  boxData;

  auto &fill   = boxData.shape().fill();
  auto &stroke = boxData.shape().stroke();

  fill  .setVisible(false);
  stroke.setVisible(false);

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  auto text = argv.getParseStr("text", "Annotation");

  textData.setFont    (argv.getParseFont ("font"    , textData.font      ()));
  textData.setColor   (argv.getParseColor("color"   , textData.color     ()));
  textData.setAlpha   (argv.getParseAlpha("alpha"   , textData.alpha     ()));
  textData.setAngle   (argv.getParseAngle("angle"   , textData.angle     ()));
  textData.setContrast(argv.getParseBool ("contrast", textData.isContrast()));
  textData.setAlign   (argv.getParseAlign("align"   , textData.align     ()));
  textData.setHtml    (argv.getParseBool ("html"    , textData.isHtml    ()));

  fill.setVisible(argv.getParseBool   ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor  ("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseAlpha  ("fill_alpha"  , fill.alpha    ()));
//fill.setPattern(argv.getParsePattern("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseAlpha   ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  stroke.setCornerSize(argv.getParseLength(view, plot, "corner_size", stroke.cornerSize()));

  boxData.setBorderSides(argv.getParseSides("border_sides", boxData.borderSides()));

  //---

  CQChartsTextAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("position")) {
    auto pos = argv.getParseObjRefPos(view, plot, "position");

    if (! pos.isValid())
      return errorMsg("Invalid position");

    if      (plot)
      annotation = plot->addTextAnnotation(pos, text);
    else if (view)
      annotation = view->addTextAnnotation(pos, text);
  }
  else if (argv.hasParseArg("rectangle")) {
    auto rect = argv.getParseRect(view, plot, "rectangle");

    if (! rect.isValid())
      return errorMsg("Invalid text rectangle");

    if      (plot)
      annotation = plot->addTextAnnotation(rect, text);
    else if (view)
      annotation = view->addTextAnnotation(rect, text);
  }
  else {
    auto pos = CQChartsObjRefPos::plot(CQChartsGeom::Point(0, 0));

    if      (plot)
      annotation = plot->addTextAnnotation(pos, text);
    else if (view)
      annotation = view->addTextAnnotation(pos, text);
  }

  if (! annotation)
    return errorMsg("Failed to create annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setTextData(textData);
  annotation->setBoxData(boxData);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_value_set_annotation command

void
CQChartsCreateChartsValueSetAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-rectangle", ArgType::Rect, "rectangle");

  addArg(argv, "-values", ArgType::Reals, "values");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsValueSetAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsValueSetAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsValueSetAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  auto rect = argv.getParseRect(view, plot, "rectangle");

  if (! rect.isValid())
    return errorMsg("Invalid rectangle value");

  auto values = argv.getParseReals("values");

  //---

  CQChartsValueSetAnnotation *annotation = nullptr;

  if      (plot)
    annotation = plot->addValueSetAnnotation(rect, values);
  else if (view)
    annotation = view->addValueSetAnnotation(rect, values);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_widget_annotation command

void
CQChartsCreateChartsWidgetAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-position" , ArgType::ObjRefPos, "position");
  addArg(argv, "-rectangle", ArgType::Rect     , "rectangle bounding box");

  addArg(argv, "-widget", ArgType::String, "widget path");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsWidgetAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsWidgetAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCreateChartsWidgetAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  // get widget
  CQChartsWidget widget;

  if      (argv.hasParseArg("widget")) {
    auto widgetName = argv.getParseStr("widget");

    widget = CQChartsWidget(widgetName);

    if (! widget.isValid())
      return errorMsg(QString("Invalid widget name '%1'").arg(widgetName));
  }

  //---

  // create annotation
  CQChartsWidgetAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("rectangle")) {
    auto rect = argv.getParseRect(view, plot, "rectangle");

    if (! rect.isValid())
      return errorMsg("Invalid rectangle geometry");

    if (     plot)
      annotation = plot->addWidgetAnnotation(rect, widget);
    else if (view)
      annotation = view->addWidgetAnnotation(rect, widget);
  }
  else {
    auto pos = CQChartsObjRefPos::plot(CQChartsGeom::Point(0, 0));

    if (argv.hasParseArg("position"))
      pos = argv.getParseObjRefPos(view, plot, "position");

    if      (plot)
      annotation = plot->addWidgetAnnotation(pos, widget);
    else if (view)
      annotation = view->addWidgetAnnotation(pos, widget);
  }

  if (! annotation)
    return errorMsg("Failed to create widget annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// create_charts_symbol_map_key_annotation command

void
CQChartsCreateChartsSymbolMapKeyAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-plot", ArgType::String, "plot name").setRequired();

  addArg(argv, "-group", ArgType::String, "annotation group");

  addArg(argv, "-id" , ArgType::String, "annotation id" );
  addArg(argv, "-tip", ArgType::String, "annotation tip");

  addArg(argv, "-properties", ArgType::String, "name_values");
}

QStringList
CQChartsCreateChartsSymbolMapKeyAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsCreateChartsSymbolMapKeyAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
#if 0
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };
#endif

  //---

  CQPerfTrace trace("CQChartsCreateChartsSymbolMapKeyAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getPlotArg(argv, plot))
    return false;

  //---

  // get parent group
  CQChartsAnnotationGroup *group = nullptr;

  if (argv.hasParseArg("group")) {
    group = dynamic_cast<CQChartsAnnotationGroup *>(
              cmds()->getAnnotationByName(argv.getParseStr("group")));
    if (! group) return false;
  }

  //---

  // get id and tip
  auto id    = argv.getParseStr("id");
  auto tipId = argv.getParseStr("tip");

  //---

  auto *annotation = plot->addSymbolMapKeyAnnotation();

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  if (group)
    group->addAnnotation(annotation);

  //---

  // set properties
  cmds()->setAnnotationArgProperties(argv, annotation);

  //---

  return cmdBase_->setCmdRc(annotation->pathId());
}

//------

// remove_charts_annotation command

void
CQChartsRemoveChartsAnnotationCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  // -view or -plot needed for -all
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-id" , ArgType::String , "annotation id");
  addArg(argv, "-all", ArgType::Boolean, "all annotations");
  argv.endCmdGroup();
}

QStringList
CQChartsRemoveChartsAnnotationCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsRemoveChartsAnnotationCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsRemoveChartsAnnotationCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // only id needed for specific
  if (argv.hasParseArg("id")) {
    auto id = argv.getParseStr("id");

    auto *annotation = cmds()->getAnnotationByName(id);
    if (! annotation) return false;

    auto *plot = annotation->plot();
    auto *view = annotation->view();

    if (plot)
      plot->removeAnnotation(annotation);
    else
      view->removeAnnotation(annotation);
  }
  else {
    // get parent plot or view
    CQChartsView *view = nullptr;
    CQChartsPlot *plot = nullptr;

    if (! cmds()->getViewPlotArg(argv, view, plot))
      return false;

    if      (plot)
      plot->removeAllAnnotations();
    else if (view)
      view->removeAllAnnotations();
    else
      return errorMsg("-view or -plot needed for -all");
  }

  return true;
}

//------

// add_charts_key_item command

void
CQChartsAddChartsKeyItemCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-view"      , ArgType::String, "view name");
  addArg(argv, "-plot"      , ArgType::String, "plot name");
  addArg(argv, "-annotation", ArgType::String, "annotation name");
  argv.endCmdGroup();

  addArg(argv, "-id", ArgType::String , "item id");

  addArg(argv, "-group", ArgType::Boolean, "Add items to group");

  addArg(argv, "-row"  , ArgType::Integer, "item row");
  addArg(argv, "-col"  , ArgType::Integer, "item column");
  addArg(argv, "-nrows", ArgType::Integer, "item row count");
  addArg(argv, "-ncols", ArgType::Integer, "item column count");

  addArg(argv, "-text"    , ArgType::String, "item text");
  addArg(argv, "-color"   , ArgType::Color , "item color");
  addArg(argv, "-symbol"  , ArgType::Color , "item symbol");
  addArg(argv, "-gradient", ArgType::String, "item gradient");
}

QStringList
CQChartsAddChartsKeyItemCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view"      ) return cmds()->viewArgValues();
  else if (arg == "plot"      ) return cmds()->plotArgValues(nullptr);
  else if (arg == "annotation") return cmds()->annotationArgValues(nullptr, nullptr);

  return QStringList();
}

bool
CQChartsAddChartsKeyItemCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsAddChartsKeyItemCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent view, plot or key annotation
  CQChartsView*          view          = nullptr;
  CQChartsPlot*          plot          = nullptr;
  CQChartsKeyAnnotation* keyAnnotation = nullptr;

  if      (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    view = cmds()->getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    plot = cmds()->getPlotByName(nullptr, plotName);
    if (! plot) return false;

    view = plot->view();
  }
  else if (argv.hasParseArg("annotation")) {
    auto annotationName = argv.getParseStr("annotation");

    auto *annotation = cmds()->getAnnotationByName(annotationName);
    if (! annotation) return false;

    keyAnnotation = dynamic_cast<CQChartsKeyAnnotation *>(annotation);
    if (! keyAnnotation) return errorMsg("value must be a key annotation");
  }
  else {
    return errorMsg("-view, -plot or -annotation needed");
  }

  //---

  // get key
  CQChartsPlotKey *plotKey = nullptr;

  if      (plot) {
    plotKey = plot->key();
    if (! plotKey) return errorMsg("plot has no key");
  }
  else if (view) {
    // TODO: not supported yet
    auto *key = view->key();
    if (! key) return errorMsg("view has no key");
  }
  else if (keyAnnotation) {
    plotKey = dynamic_cast<CQChartsPlotKey *>(keyAnnotation->key());
    if (! plotKey) return errorMsg("annotation has no key");
  }
  else {
    return errorMsg("-view, -plot or -annotation needed");
  }

  //---

  // get position of next row, column
  int row = (! plotKey->isHorizontal() ? plotKey->maxRow() : 0);
  int col = (! plotKey->isHorizontal() ? 0 : plotKey->maxCol());

  row = argv.getParseInt("row", row);
  col = argv.getParseInt("col", col);

  int nrows = argv.getParseInt("nrows", 1);
  int ncols = argv.getParseInt("ncols", 1);

  //---

  // get text label
  auto text = argv.getParseStr("text");

  //---

  QString id;

  if (argv.hasParseArg("id"))
    id = argv.getParseStr("id");

  int ind = 0;

  //---

  auto addItems = [&](CQChartsKeyItem *item1, CQChartsKeyItem *item2) {
    if (id != "") {
      item1->setId(QString("keyitem:%1:%2").arg(id).arg(item1->typeName()));
      item2->setId(QString("keyitem:%1:%2").arg(id).arg(item2->typeName()));
    }
    else {
      item1->setId(QString("keyitem:%1:%2").arg(ind++).arg(item1->typeName()));
      item2->setId(QString("keyitem:%1:%2").arg(ind++).arg(item2->typeName()));
    }

    if (argv.hasParseArg("group")) {
      auto *groupItem = new CQChartsGroupKeyItem(plotKey);

      if (id != "")
        groupItem->setId(QString("keygroup:%1:%2").arg(id).arg(groupItem->typeName()));
      else
        groupItem->setId(QString("keygroup:%1:%2").arg(ind++).arg(groupItem->typeName()));

      groupItem->addRowItems(item1, item2);

      plotKey->addItem(groupItem, row, col);

      //plotKey->nextRowCol(row, col);
    }
    else {
      if (! plotKey->isHorizontal()) {
        plotKey->addItem(item1, row, col        , nrows, ncols);
        plotKey->addItem(item2, row, col + ncols, nrows, ncols);
      }
      else {
        plotKey->addItem(item1, row        , col, nrows, ncols);
        plotKey->addItem(item2, row + nrows, col, nrows, ncols);
      }
    }
  };

  auto addItem = [&](CQChartsKeyItem *item) {
    if (id != "")
      item->setId(QString("keyitem:%1:%2").arg(id).arg(item->typeName()));
    else
      item->setId(QString("keyitem:%1:%2").arg(ind++).arg(item->typeName()));

    plotKey->addItem(item, row, col, nrows, ncols);
  };

  //---

  // add text and symbol
  if      (argv.hasParseArg("symbol")) {
    CQChartsSymbolData symbolData;

    symbolData.setSymbol(CQChartsSymbol(argv.getParseStr("symbol")));

    if (argv.hasParseArg("color")) {
      auto color = argv.getParseColor("color");

      CQChartsStrokeData strokeData;
      CQChartsFillData   fillData;

      strokeData.setColor(color);
      fillData  .setColor(color);

      symbolData.setStroke(strokeData);
      symbolData.setFill  (fillData);
    }

    CQChartsUtil::ColorInd colorInd;

    auto *item1 = new CQChartsLineKeyItem(plotKey, colorInd, colorInd);
    auto *item2 = new CQChartsTextKeyItem(plotKey, text    , colorInd);

    item1->setSymbolData(symbolData);

    addItems(item1, item2);
  }
  // add text and color box
  else if (argv.hasParseArg("color")) {
    auto color = argv.getParseColor("color");

    CQChartsUtil::ColorInd colorInd;

    auto *item1 = new CQChartsColorBoxKeyItem(plotKey, colorInd, colorInd, colorInd);
    auto *item2 = new CQChartsTextKeyItem    (plotKey, text    , colorInd);

    item1->setColor(color);

    addItems(item1, item2);
  }
  // add text and gradient
  else if (argv.hasParseArg("gradient")) {
    auto palette = argv.getParseStr("gradient");

    auto *item = new CQChartsGradientKeyItem(plotKey);

    item->setPalette(CQChartsPaletteName(palette));

    addItem(item);
  }
  // add text
  else {
    CQChartsUtil::ColorInd colorInd;

    auto *item = new CQChartsTextKeyItem(plotKey->plot(), text, colorInd);

    addItem(item);
  }

  return true;
}

//------

// connect_charts_signal command

void
CQChartsConnectChartsSignalCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-view"      , ArgType::String, "view name");
  addArg(argv, "-plot"      , ArgType::String, "plot name");
  addArg(argv, "-annotation", ArgType::String, "annotation name");
  argv.endCmdGroup();

  addArg(argv, "-from", ArgType::String, "from connection name");
  addArg(argv, "-to"  , ArgType::String, "to procedure name");
}

QStringList
CQChartsConnectChartsSignalCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view"      ) return cmds()->viewArgValues();
  else if (arg == "plot"      ) return cmds()->plotArgValues(nullptr);
  else if (arg == "annotation") return cmds()->annotationArgValues(nullptr, nullptr);

  return QStringList();
}

bool
CQChartsConnectChartsSignalCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsConnectChartsSignalCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView*       view       = nullptr;
  CQChartsPlot*       plot       = nullptr;
  CQChartsAnnotation* annotation = nullptr;

  if      (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    view = cmds()->getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    plot = cmds()->getPlotByName(nullptr, plotName);
    if (! plot) return false;

    view = plot->view();
  }
  else if (argv.hasParseArg("annotation")) {
    auto annotationName = argv.getParseStr("annotation");

    annotation = cmds()->getAnnotationByName(annotationName);
    if (! annotation) return false;
  }

  //---

  auto fromName = argv.getParseStr("from");
  auto toName   = argv.getParseStr("to"  );

  auto createCmdsSlot = [&]() {
    return new CQChartsCmdsSlot(cmds(), view, plot, annotation, toName);
  };

  if      (plot) {
    if      (fromName == "objIdPressed") {
      cmds()->connect(plot, SIGNAL(objIdPressed(const QString &)),
                      createCmdsSlot(), SLOT(objIdPressed(const QString &)));
    }
    else if (fromName == "annotationIdPressed") {
      cmds()->connect(plot, SIGNAL(annotationIdPressed(const QString &)),
                      createCmdsSlot(), SLOT(annotationIdPressed(const QString &)));
    }
    else if (fromName == "plotObjsAdded") {
      cmds()->connect(plot, SIGNAL(plotObjsAdded()), createCmdsSlot(), SLOT(plotObjsAdded()));
    }
    else if (fromName == "selectionChanged") {
      cmds()->connect(plot, SIGNAL(selectionChanged()), createCmdsSlot(), SLOT(selectionChanged()));
    }
    else if (fromName == "selectPress") {
      cmds()->connect(plot, SIGNAL(selectPressSignal(const CQChartsGeom::Point &)),
                      createCmdsSlot(), SLOT(selectPress(const CQChartsGeom::Point &)));
    }
    else if (fromName == "animateStateChanged") {
      cmds()->connect(plot, SIGNAL(animateStateChanged(bool)),
                      createCmdsSlot(), SLOT(animateStateChanged(bool)));
    }
    else if (fromName == "currentModelChanged") {
      cmds()->connect(plot, SIGNAL(currentModelChanged()),
                      createCmdsSlot(), SLOT(currentModelChanged()));
    }
    else if (fromName == "?") {
      static auto names = QStringList() <<
        "objIdPressed" << "annotationIdPressed" << "plotObjsAdded" << "selectionChanged" <<
        "selectPress" << "animateStateChanged" << "currentModelChanged";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("unknown slot");
  }
  else if (view) {
    if      (fromName == "objIdPressed") {
      cmds()->connect(view, SIGNAL(objIdPressed(const QString &)),
                      createCmdsSlot(), SLOT(objIdPressed(const QString &)));
    }
    else if (fromName == "annotationIdPressed") {
      cmds()->connect(view, SIGNAL(annotationIdPressed(const QString &)),
                      createCmdsSlot(), SLOT(annotationIdPressed(const QString &)));
    }
    else if (fromName == "selectionChanged") {
      cmds()->connect(view, SIGNAL(selectionChanged()), createCmdsSlot(), SLOT(selectionChanged()));
    }
    else if (fromName == "keyEventPress") {
      cmds()->connect(view, SIGNAL(keyEventPress(const QString &)),
                      createCmdsSlot(), SLOT(keyEventPress(const QString &)));
    }
    else if (fromName == "viewResized") {
      cmds()->connect(view, SIGNAL(viewResized()),
                      createCmdsSlot(), SLOT(viewResized()));
    }
    else if (fromName == "?") {
      static auto names = QStringList() <<
        "objIdPressed" << "annotationIdPressed" << "selectionChanged" << "keyEventPress";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("unknown slot");
  }
  else if (annotation) {
    if      (fromName == "annotationIdPressed") {
      cmds()->connect(annotation, SIGNAL(pressed(const QString &)),
                      createCmdsSlot(), SLOT(annotationIdPressed(const QString &)));
    }
    else if (fromName == "?") {
      static auto names = QStringList() << "annotationIdPressed";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("unknown slot");
  }
  else {
    if      (fromName == "themeChanged") {
      cmds()->connect(charts(), SIGNAL(themeChanged()), createCmdsSlot(),
                      SLOT(themeChanged()));
    }
    else if (fromName == "interfaceThemeChanged") {
      cmds()->connect(charts(), SIGNAL(interfaceThemeChanged()),
                      createCmdsSlot(), SLOT(interfaceThemeChanged()));
    }
    else if (fromName == "?") {
      static auto names = QStringList() << "themeChanged" << "interfaceThemeChanged";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("unknown slot");
  }

  return true;
}

//------

// add_custom_widget command

void
CQChartsAddCustomWidgetCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-widget", ArgType::String, "widget name");
}

QStringList
CQChartsAddCustomWidgetCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsAddCustomWidgetCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsAddCustomWidgetCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // get widget
  auto widgetName = argv.getParseStr("widget");

  auto widget = CQChartsWidget(widgetName);

  if (! widget.isValid())
    return errorMsg(QString("Invalid widget name '%1'").arg(widgetName));

  if      (view)
    view->setCustomWidget(widget);
  else if (plot)
    plot->setCustomWidget(widget);

  return cmdBase_->setCmdRc(widget.path());
}

//------

// print_charts_image command

void
CQChartsPrintChartsImageCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-file", ArgType::String, "filename").setRequired();

  addArg(argv, "-layer", ArgType::String, "layer name");
}

QStringList
CQChartsPrintChartsImageCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsPrintChartsImageCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsPrintChartsImageCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  auto filename = argv.getParseStr("file");

  if (plot) {
    auto layerName = argv.getParseStr("layer");

    if (layerName.length()) {
      auto type = CQChartsLayer::nameType(layerName);

      if (! plot->printLayer(type, filename))
        return errorMsg("Failed to print layer");
    }
    else {
      view = plot->view();

      view->printFile(filename, plot);
    }
  }
  else
    view->printFile(filename);

  return true;
}

//------

// write_charts_data command

void
CQChartsWriteChartsDataCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-type", ArgType::String, "type");
  addArg(argv, "-file", ArgType::String, "filename");
}

QStringList
CQChartsWriteChartsDataCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsWriteChartsDataCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsWriteChartsDataCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  // set output stream
  auto filename = argv.getParseStr("file", "");

  std::ofstream fos;
  std::ostream* os = nullptr;

  if (filename != "") {
    fos.open(filename.toLatin1().constData(), std::ios_base::out);

    if (fos.fail())
      return errorMsg("Failed to open '" + filename + "'");

    os = &fos;
  }
  else
    os = &std::cout;

  //---

  auto type = argv.getParseStr("type");

  if      (plot) {
    if      (type == "") {
      plot->write(*os);
    }
    else if (type == "annotations") {
      const auto &annotations = plot->annotations();

      for (const auto &annotation : annotations)
        annotation->write(*os);
    }
    else if (type == "?") {
      static auto names = QStringList() << "" << "annotations";
      return cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid write type");
  }
  else if (view) {
    if      (type == "plots") {
      CQChartsView::Plots plots;

      view->getPlots(plots);

      for (const auto &plot : plots)
        plot->write(*os);
    }
    else if (type == "annotations") {
      const auto &annotations = view->annotations();

      for (const auto &annotation : annotations)
        annotation->write(*os);
    }
    else if (type == "?") {
      static auto names = QStringList() << "plots" << "annotations";
      return cmdBase_->setCmdRc(names);
    }
    else {
      return errorMsg("Invalid write type");
    }
  }

  return true;
}

//------

// write_charts_stats command

void
CQChartsWriteChartsStatsCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneReq);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-bbox", ArgType::String, "Bounding box");
}

QStringList
CQChartsWriteChartsStatsCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsWriteChartsStatsCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsWriteChartsDataCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  //---

  CQChartsGeom::BBox bbox;

  auto bboxStr = argv.getParseStr("bbox");

  if (bboxStr != "") {
    if (! bbox.fromString(bboxStr))
      errorMsg("Invalid bbox");
  }

  //---

  if      (view)
    view->writeStats(bbox);
  else if (plot) {
    //plot->writeStats();
  }

  return true;
}

//------

// show_charts_load_model_dlg command

void
CQChartsShowChartsLoadModelDlgCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-modal", ArgType::Boolean, "show modal");
}

QStringList
CQChartsShowChartsLoadModelDlgCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsShowChartsLoadModelDlgCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsShowChartsLoadModelDlgCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  bool modal = argv.getParseBool("modal");

  //---

  auto *dlg = new CQChartsLoadModelDlg(charts());

  if (modal)
    dlg->exec();
  else
    dlg->show();

  return cmdBase_->setCmdRc(dlg->modelId());
}

//------

// show_charts_manage_models_dlg command

void
CQChartsShowChartsManageModelsDlgCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-modal", ArgType::Boolean, "show modal");
}

QStringList
CQChartsShowChartsManageModelsDlgCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsShowChartsManageModelsDlgCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsShowChartsManageModelsDlgCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  bool modal = argv.getParseBool("modal");

  //---

  auto *dlg = new CQChartsManageModelsDlg(charts());

  if (modal)
    dlg->exec();
  else
    dlg->show();

  return true;
}

//------

// show_charts_create_plot_dlg command

void
CQChartsShowChartsCreatePlotDlgCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-model", ArgType::String , "model_id" );
  addArg(argv, "-view" , ArgType::String , "view name" );
  addArg(argv, "-modal", ArgType::Boolean, "show modal");
}

QStringList
CQChartsShowChartsCreatePlotDlgCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "model") return cmds()->modelArgValues();
  else if (arg == "view" ) return cmds()->viewArgValues();

  return QStringList();
}

bool
CQChartsShowChartsCreatePlotDlgCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsShowChartsCreatePlotDlgCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  bool modal = argv.getParseBool("modal");

  //---

  // get model
  auto modelId = argv.getParseStr("model");

  auto *modelData = cmds()->getModelDataOrCurrent(modelId);
  if (! modelData) return errorMsg("No model data for '" + modelId + "'");

  //---

  auto *dlg = new CQChartsCreatePlotDlg(charts(), modelData);

  auto viewName = argv.getParseStr("view");

  dlg->setViewName(viewName);

  if (modal)
    dlg->exec();
  else
    dlg->show();

  auto *plot = dlg->plot();

  return cmdBase_->setCmdRc(plot ? plot->pathId() : "");
}

//------

// show_charts_text_dlg command

void
CQChartsShowChartsTextDlgCmd::
addCmdArgs(CQChartsCmdArgs &)
{
}

QStringList
CQChartsShowChartsTextDlgCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsShowChartsTextDlgCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsShowChartsTextDlgCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  static CQChartsTextDlg *dlg;

  if (! dlg)
    dlg = new CQChartsTextDlg;

  dlg->show();

  return true;
}

//------

// show_charts_help_dlg command

void
CQChartsShowChartsHelpDlgCmd::
addCmdArgs(CQChartsCmdArgs &)
{
}

QStringList
CQChartsShowChartsHelpDlgCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsShowChartsHelpDlgCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsShowChartsHelpDlgCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsHelpDlgMgrInst->showDialog(charts());

  return true;
}

//------

#include <CQChartsAlphaEdit.h>
#include <CQChartsArrowDataEdit.h>
#include <CQChartsBoxDataEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsColumnsEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsFillPatternEdit.h>
#include <CQChartsFillUnderEdit.h>
#include <CQChartsFilterEdit.h>
#include <CQChartsKeyLocationEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsLineDashEdit.h>
#include <CQChartsLineDataEdit.h>
#include <CQChartsModelColumnEdit.h>
#include <CQChartsPolygonEdit.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsRectEdit.h>
#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsSymbolDataEdit.h>
#include <CQChartsSymbolTypeEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQChartsTextBoxDataEdit.h>

// test_charts_edit command

void
CQChartsTestEditCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  argv.startCmdGroup(CmdGroup::Type::OneOpt);
  addArg(argv, "-view", ArgType::String, "view name");
  addArg(argv, "-plot", ArgType::String, "plot name");
  argv.endCmdGroup();

  addArg(argv, "-type", ArgType::String, "type").setMultiple(true);

  addArg(argv, "-editable", ArgType::SBool, "editable");

}

QStringList
CQChartsTestEditCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if      (arg == "view") return cmds()->viewArgValues();
  else if (arg == "plot") return cmds()->plotArgValues(nullptr);

  return QStringList();
}

bool
CQChartsTestEditCmd::
execCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts()->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsTestEditCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get parent plot or view
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (! cmds()->getViewPlotArg(argv, view, plot))
    return false;

  if (plot)
    view = plot->view();

  // get types
  auto types = argv.getParseStrs("type");

  bool editable = argv.getParseBool("editable", true);

  //---

  // create parent dialog
  auto *dialog = new QDialog;

  auto *layout = new QGridLayout(dialog);

  //---

  // add type edits
  int numEdits = 0;

  auto addEdit = [&](QWidget *w, const QString &label) {
    layout->addWidget(new QLabel(label), numEdits, 0);
    layout->addWidget(w                , numEdits, 1);

    auto *editBase     = qobject_cast<CQChartsEditBase     *>(w);
    auto *lineEditBase = qobject_cast<CQChartsLineEditBase *>(w);

    if      (editBase) {
      if      (plot) editBase->setPlot(plot);
      else if (view) editBase->setView(view);
    }
    else if (lineEditBase) {
      if      (plot) lineEditBase->setPlot(plot);
      else if (view) lineEditBase->setView(view);

      lineEditBase->setEditable(editable);
    }

    ++numEdits;
  };

  for (int i = 0; i < types.length(); ++i) {
    const auto &type = types[i];

    if      (type == "alpha") {
      auto *edit = new CQChartsAlphaEdit; addEdit(edit, type);
    }
    else if (type == "arrow_data") {
      auto *edit = new CQChartsArrowDataEdit; addEdit(edit, type);
    }
    else if (type == "arrow_data_line") {
      auto *edit = new CQChartsArrowDataLineEdit; addEdit(edit, type);
    }
    else if (type == "box_data") {
      auto *edit = new CQChartsBoxDataEdit; addEdit(edit, type);
    }
    else if (type == "box_data_line") {
      auto *edit = new CQChartsBoxDataLineEdit; addEdit(edit, type);
    }
    else if (type == "color") {
      auto *edit = new CQChartsColorEdit; addEdit(edit, type);
    }
    else if (type == "color_line") {
      auto *edit = new CQChartsColorLineEdit; addEdit(edit, type);
    }
    else if (type == "column") {
      auto *edit = new CQChartsColumnEdit; addEdit(edit, type);
    }
    else if (type == "column_line") {
      auto *edit = new CQChartsColumnLineEdit; addEdit(edit, type);
    }
    else if (type == "columns") {
      auto *edit = new CQChartsColumnsEdit; addEdit(edit, type);
    }
    else if (type == "columns_line") {
      auto *edit = new CQChartsColumnsLineEdit; addEdit(edit, type);
    }
    else if (type == "fill_data") {
      auto *edit = new CQChartsFillDataEdit; addEdit(edit, type);
    }
    else if (type == "fill_data_line") {
      auto *edit = new CQChartsFillDataLineEdit; addEdit(edit, type);
    }
    else if (type == "fill_pattern") {
      auto *edit = new CQChartsFillPatternEdit; addEdit(edit, type);
    }
    else if (type == "fill_under_side") {
      auto *edit = new CQChartsFillUnderSideEdit; addEdit(edit, type);
    }
    else if (type == "fill_under_pos") {
      auto *edit = new CQChartsFillUnderPosEdit; addEdit(edit, type);
    }
    else if (type == "fill_under_pos_line") {
      auto *edit = new CQChartsFillUnderPosLineEdit; addEdit(edit, type);
    }
    else if (type == "filter") {
      auto *edit = new CQChartsFilterEdit; addEdit(edit, type);
    }
    else if (type == "key_location") {
      auto *edit = new CQChartsKeyLocationEdit; addEdit(edit, type);
    }
    else if (type == "length") {
      auto *edit = new CQChartsLengthEdit; addEdit(edit, type);
    }
    else if (type == "line_dash") {
      auto *edit = new CQChartsLineDashEdit; addEdit(edit, type);
    }
    else if (type == "line_data") {
      auto *edit = new CQChartsLineDataEdit; addEdit(edit, type);
    }
    else if (type == "line_data_line") {
      auto *edit = new CQChartsLineDataLineEdit; addEdit(edit, type);
    }
    else if (type == "model_column") {
      auto *edit = new CQChartsModelColumnEdit; addEdit(edit, type);
    }
    else if (type == "polygon") {
      auto *edit = new CQChartsPolygonEdit; addEdit(edit, type);
    }
    else if (type == "polygon_line") {
      auto *edit = new CQChartsPolygonLineEdit; addEdit(edit, type);
    }
    else if (type == "position") {
      auto *edit = new CQChartsPositionEdit; addEdit(edit, type);
    }
    else if (type == "rectangle") {
      auto *edit = new CQChartsRectEdit; addEdit(edit, type);
    }
    else if (type == "shape_data") {
      auto *edit = new CQChartsShapeDataEdit; addEdit(edit, type);
    }
    else if (type == "shape_data_line") {
      auto *edit = new CQChartsShapeDataLineEdit; addEdit(edit, type);
    }
    else if (type == "sides") {
      auto *edit = new CQChartsSidesEdit; addEdit(edit, type);
    }
    else if (type == "stroke_data") {
      auto *edit = new CQChartsStrokeDataEdit; addEdit(edit, type);
    }
    else if (type == "stroke_data_line") {
      auto *edit = new CQChartsStrokeDataLineEdit; addEdit(edit, type);
    }
    else if (type == "symbol_data") {
      auto *edit = new CQChartsSymbolDataEdit; addEdit(edit, type);
    }
    else if (type == "symbol_data_line") {
      auto *edit = new CQChartsSymbolDataLineEdit; addEdit(edit, type);
    }
    else if (type == "symbol_type") {
      auto *edit = new CQChartsSymbolTypeEdit; addEdit(edit, type);
    }
    else if (type == "text_box_data") {
      auto *edit = new CQChartsTextBoxDataEdit; addEdit(edit, type);
    }
    else if (type == "text_box_data_line") {
      auto *edit = new CQChartsTextBoxDataLineEdit; addEdit(edit, type);
    }
    else if (type == "text_data") {
      auto *edit = new CQChartsTextDataEdit; addEdit(edit, type);
    }
    else if (type == "text_data_line") {
      auto *edit = new CQChartsTextDataLineEdit; addEdit(edit, type);
    }
    else
      return errorMsg("Bad edit type '" + type + "'");
  }

  if (numEdits == 0)
    return false;

  layout->setRowStretch(numEdits + 1, 1);

  // show dialog
  dialog->show();

  return true;
}

//------

#ifdef CQCHARTS_DATA_FRAME
// data_frame command

void
CQChartsDataFrameCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-file", ArgType::String, "file to load");
}

QStringList
CQChartsDataFrameCmd::
getArgValues(const QString &option, const NameValueMap &nameValueMap)
{
  QStringList strs;

  if (option == "file") {
    auto p = nameValueMap.find("file");

    auto file = (p != nameValueMap.end() ? (*p).second : QString());

    return CQDataFrame::completeFile(file);
  }

  return strs;
}

bool
CQChartsDataFrameCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsDataFrameCmd::exec");

  addArgs(argv);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQDataFrame::showFrame(this);

  //---

  if (! argv.hasParseArg("file"))
    return false;

  auto filename = argv.getParseStr("file");

  auto *charts = this->charts();

  CQChartsFile file(charts, filename);

  CQDataFrame::loadFrameFile(file.resolve());

  return true;
}
#endif

//------

CQChartsPlot *
CQChartsCmds::
createPlot(CQChartsView *view, const ModelP &model, CQChartsPlotType *type, bool reuse)
{
  if (! view)
    view = getView(reuse);

  //---

  auto *plot = type->createAndInit(view, model);

  return plot;
}

bool
CQChartsCmds::
initPlot(CQChartsPlot *plot, const CQChartsNameValueData &nameValueData,
         const CQChartsGeom::BBox &bbox)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  auto *type = plot->type();

  const auto &model = plot->currentModel();

  // check column parameters exist
  for (const auto &nameValue : nameValueData.columns) {
    bool found = false;

    for (const auto &parameter : type->parameters()) {
      if (parameter->name() == nameValue.first) {
        found = true;
        break;
      }
    }

    if (! found)
      return errorMsg("Illegal column name '" + nameValue.first + "'");
  }

  // check other parameters exist
  for (const auto &nameValue : nameValueData.parameters) {
    bool found = false;

    for (const auto &parameter : type->parameters()) {
      if (parameter->name() == nameValue.first) {
        found = true;
        break;
      }
    }

    if (! found)
      return errorMsg("Illegal parameter name '" + nameValue.first + "'");
  }

  //---

  // set plot property for widgets for plot parameters
  for (const auto &parameter : type->parameters()) {
    if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN) {
      auto p = nameValueData.columns.find(parameter->name());

      if (p == nameValueData.columns.end())
        continue;

      CQChartsColumn column;

      const auto &value = (*p).second;

      if (! CQChartsModelUtil::stringToColumn(model.data(), value, column)) {
        (void) errorMsg("Bad column name '" + value + "'");
        continue;
      }

      auto scol = column.toString();

      if (! plot->setParameter(parameter, scol)) {
        (void) errorMsg("Failed to set parameter " + parameter->propName() + " '" + scol + "'");
        continue;
      }
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST) {
      auto p = nameValueData.columns.find(parameter->name());

      if (p == nameValueData.columns.end())
        continue;

      const auto &str = (*p).second;

      std::vector<CQChartsColumn> columns;

      if (! CQChartsModelUtil::stringToColumns(model.data(), str, columns)) {
        (void) errorMsg("Bad columns name '" + str + "'");
        continue;
      }

      auto s = CQChartsColumn::columnsToString(columns);

      if (! plot->setParameter(parameter, QVariant(s))) {
        (void) errorMsg("Failed to set parameter " + parameter->propName() + " '" + s + "'");
        continue;
      }
    }
    else {
      auto p = nameValueData.parameters.find(parameter->name());

      if (p == nameValueData.parameters.end())
        continue;

      const auto &value = (*p).second;

      QVariant var;

      if      (parameter->type() == CQChartsPlotParameter::Type::STRING) {
        var = QVariant(value);
      }
      else if (parameter->type() == CQChartsPlotParameter::Type::REAL) {
        bool ok;

        double r = CQChartsUtil::toReal(value, ok);

        if (! ok) {
          (void) errorMsg("Invalid real value '" + value + "' for '" +
                          parameter->typeName() + "'");
          continue;
        }

        var = CQModelUtil::realVariant(r);
      }
      else if (parameter->type() == CQChartsPlotParameter::Type::INTEGER) {
        bool ok;

        long i = CQChartsUtil::toInt(value.trimmed(), ok);

        if (! ok) {
          (void) errorMsg("Invalid integer value '" + value + "' for '" +
                          parameter->typeName() + "'");
          continue;
        }

        var = CQModelUtil::intVariant(i);
      }
      else if (parameter->type() == CQChartsPlotParameter::Type::ENUM) {
        var = QVariant(value);
      }
      else if (parameter->type() == CQChartsPlotParameter::Type::BOOLEAN) {
        bool ok;
        bool b = CQChartsCmdBaseArgs::stringToBool(value, &ok);

        if (! ok) {
          (void) errorMsg("Invalid boolean value '" + value + "' for '" +
                          parameter->typeName() + "'");
          continue;
        }

        var = QVariant(b);
      }
      else {
        continue;
      }

      if (! plot->setParameter(parameter, var)) {
        (void) errorMsg("Failed to set parameter " + parameter->propName() +
                        " '" + var.toString() + "'");
        continue;
      }
    }
  }

  //---

  // add plot to view and show
  auto *view = plot->view();

  view->addPlot(plot, bbox);

  //---

  return true;
}

QString
CQChartsCmds::
fixTypeName(const QString &typeName)
{
  auto typeName1 = typeName;

  // adjust typename for alias (TODO: add to typeData)
  if      (typeName1 == "piechart"      ) typeName1 = "pie";
  else if (typeName1 == "xyplot"        ) typeName1 = "xy";
  else if (typeName1 == "scatterplot"   ) typeName1 = "scatter";
  else if (typeName1 == "bar"           ) typeName1 = "barchart";
  else if (typeName1 == "force_directed") typeName1 = "forcedirected";
  else if (typeName1 == "boxplot"       ) typeName1 = "box";
  else if (typeName1 == "parallelplot"  ) typeName1 = "parallel";
  else if (typeName1 == "geometryplot"  ) typeName1 = "geometry";
  else if (typeName1 == "delaunayplot"  ) typeName1 = "delaunay";
  else if (typeName1 == "adjacencyplot" ) typeName1 = "adjacency";

  return typeName1;
}

//------

#if 0
void
CQChartsCmds::
setViewProperties(CQChartsView *view, const QString &properties)
{
  if (! view->setProperties(properties))
    charts_->errorMsg("Failed to set view properties '" + properties + "'");
}

void
CQChartsCmds::
setPlotProperties(CQChartsPlot *plot, const QString &properties)
{
  if (! plot->setProperties(properties))
    charts_->errorMsg("Failed to set plot properties '" + properties + "'");
}
#endif

bool
CQChartsCmds::
setAnnotationArgProperties(CQChartsCmdArgs &argv, CQChartsAnnotation *annotation)
{
  auto properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  return true;
}

bool
CQChartsCmds::
setAnnotationProperties(CQChartsAnnotation *annotation, const QString &properties)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  // split into properties
  QStringList strs;

  if (! CQTcl::splitList(properties, strs))
    return errorMsg(QString("Invalid properties string '%1'").arg(properties));

  bool rc = true;

  for (int i = 0; i < strs.length(); ++i) {
    // split into name/value
    QStringList strs1;

    if (! CQTcl::splitList(strs[i], strs1))
      return errorMsg(QString("Invalid property string '%1'").arg(strs[i]));

    if (strs1.size() != 2)
      return errorMsg(QString("Invalid property string '%1'").arg(strs[i]));

    if (! annotation->setProperty(strs1[0], strs1[1]))
      rc = false;
  }

  if (! rc)
    return errorMsg("Failed to set annotation properties '" + properties + "'");

  return true;
}

//------

bool
CQChartsCmds::
getViewPlotArg(CQChartsCmdArgs &argv, CQChartsView* &view, CQChartsPlot* &plot)
{
  view = nullptr;
  plot = nullptr;

  if      (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    plot = getPlotByName(view, plotName);
    if (! plot) return false;
  }
  else
    return false;

  return true;
}

bool
CQChartsCmds::
getViewArg(CQChartsCmdArgs &argv, CQChartsView* &view)
{
  view = nullptr;

  if (argv.hasParseArg("view")) {
    auto viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else
    return false;

  return true;
}

bool
CQChartsCmds::
getPlotArg(CQChartsCmdArgs &argv, CQChartsPlot* &plot)
{
  plot = nullptr;

  if (argv.hasParseArg("plot")) {
    auto plotName = argv.getParseStr("plot");

    CQChartsView *view = nullptr;

    plot = getPlotByName(view, plotName);
    if (! plot) return false;
  }
  else
    return false;

  return true;
}

CQChartsView *
CQChartsCmds::
getViewByName(const QString &viewName) const
{
  CQChartsView *view = nullptr;

  if (viewName != "") {
    if (viewName == "current")
      view = charts_->currentView();
    else
      view = charts_->getView(viewName);

    if (! view) {
      charts_->errorMsg("No view '" + viewName + "'");
      return nullptr;
    }
  }
  else {
    view = charts_->currentView();

    if (! view) {
      auto *th = const_cast<CQChartsCmds *>(this);

      view = th->getView(/*reuse*/true);
    }

    if (! view) {
      charts_->errorMsg("No view");
      return nullptr;
    }
  }

  return view;
}

//------

bool
CQChartsCmds::
getPlotsByName(CQChartsView *view, const Vars &plotNames, Plots &plots,
               Plots &extraPlots, bool checkView) const
{
  bool rc = true;

  for (const auto &plotName : plotNames) {
    if (CQChartsVariant::isList(plotName)) {
      auto listVars = plotName.toList();

      Vars plotNames1;

      for (int i = 0; i < listVars.length(); ++i)
        plotNames1.push_back(listVars[i]);

      if (! getPlotsByName(view, plotNames1, plots, extraPlots, checkView))
        rc = false;
    }
    else {
      auto *plot = getPlotByName(view, plotName.toString());

      if (plot) {
        if (plot->view() != view) {
          if (! checkView)
            extraPlots.push_back(plot);
          else
            rc = false;
        }
        else
          plots.push_back(plot);
      }
      else
        rc = false;
    }
  }

  return rc;
}

CQChartsPlot *
CQChartsCmds::
getOptPlotByName(CQChartsView *view, const QString &plotName) const
{
  CQChartsPlot *plot = nullptr;

  if (plotName != "")
    plot = getPlotByName(view, plotName);
  else
    plot = view->currentPlot();

  return plot;
}

CQChartsPlot *
CQChartsCmds::
getPlotByName(CQChartsView *view, const QString &plotName) const
{
  if (plotName == "current") {
    if (! view)
      view = charts_->currentView();

    return (view ? view->currentPlot() : nullptr);
  }

  if (view) {
    auto *plot = view->getPlotForId(plotName);

    if (plot)
      return plot;
  }

  CQCharts::Views views;

  charts_->getViews(views);

  for (auto &view : views) {
    CQChartsView::Plots plots;

    view->getPlots(plots);

    for (auto &plot : plots) {
      if (plot->pathId() == plotName)
        return plot;
    }
  }

  for (auto &view : views) {
    auto *plot = view->getPlotForId(plotName);

    if (plot)
      return plot;
  }

  charts_->errorMsg("No plot '" + plotName + "'");

  return nullptr;
}

//------

CQChartsAnnotation *
CQChartsCmds::
getAnnotationByName(const QString &name) const
{
  CQCharts::Views views;

  charts_->getViews(views);

  for (auto &view : views) {
    CQChartsView::Plots plots;

    view->getPlots(plots);

    for (auto &plot : plots) {
      const auto &annotations = plot->annotations();

      for (const auto &annotation : annotations) {
        if (annotation->id() == name)
          return annotation;

        if (annotation->pathId() == name)
          return annotation;
      }
    }

    //---

    const auto &annotations = view->annotations();

    for (const auto &annotation : annotations) {
      if (annotation->id() == name)
        return annotation;

      if (annotation->pathId() == name)
        return annotation;
    }
  }

  charts_->errorMsg("No annotation '" + name + "'");

  return nullptr;
}

//------

CQChartsKeyItem *
CQChartsCmds::
getKeyItemById(const QString &id) const
{
  CQCharts::Views views;

  charts_->getViews(views);

  for (auto &view : views) {
    CQChartsView::Plots plots;

    view->getPlots(plots);

    for (auto &plot : plots) {
      // check plot key
      auto *plotKey = plot->key();

      if (plotKey) {
        for (const auto &item : plotKey->items()) {
          if (item->id() == id)
            return item;
        }
      }

      //---

      // check key annotations
      const auto &annotations = plot->annotations();

      for (const auto &annotation : annotations) {
        auto *keyAnnotation = dynamic_cast<CQChartsKeyAnnotation *>(annotation);
        if (! keyAnnotation) continue;

        auto *key = dynamic_cast<CQChartsPlotKey *>(keyAnnotation->key());
        if (! key) continue;

        for (const auto &item : key->items()) {
          if (item->id() == id)
            return item;
        }
      }
    }
  }

  charts_->errorMsg("No key item '" + id + "'");

  return nullptr;
}

//------

bool
CQChartsCmds::
loadFileModel(const CQChartsFile &file, CQChartsFileType type, const CQChartsInputData &inputData)
{
  bool hierarchical;

  auto *model = loadFile(file, type, inputData, hierarchical);
  if (! model) return false;

  ModelP modelp(model);

  auto *modelData = charts_->initModelData(modelp);

  //---

#ifdef CQCHARTS_FOLDED_MODEL
  if (inputData.fold.length()) {
    CQChartsModelData::FoldData foldData;

    foldData.columnsStr = inputData.fold;

    modelData->foldModel(foldData);
  }
#endif

  //---

  sortModel(modelData->model(), inputData.sort);

  //---

  charts_->setModelFileName(modelData, file.resolve());

  return true;
}

QAbstractItemModel *
CQChartsCmds::
loadFile(const CQChartsFile &file, CQChartsFileType type, const CQChartsInputData &inputData,
         bool &hierarchical)
{
  CQChartsLoader loader(charts_);

  loader.setQtcl(cmdBase_->qtcl());

  return loader.loadFile(file, type, inputData, hierarchical);
}

void
CQChartsCmds::
setNameValue(const QString &name, const QString &value)
{
  cmdBase_->qtcl()->createVar(name, value);
}

//------

bool
CQChartsCmds::
sortModel(ModelP &model, const QString &args)
{
  if (! args.length())
    return false;

  auto columnStr = args.trimmed();

  Qt::SortOrder order = Qt::AscendingOrder;

  if (columnStr[0] == '+' || columnStr[0] == '-') {
    order = (columnStr[0] == '+' ? Qt::AscendingOrder : Qt::DescendingOrder);

    columnStr = columnStr.mid(1);
  }

  CQChartsColumn column;

  if (! CQChartsModelUtil::stringToColumn(model.data(), columnStr, column))
    return false;

  if (column.type() != CQChartsColumn::Type::DATA)
    return false;

  return sortModel(model, column.column(), order);
}

bool
CQChartsCmds::
sortModel(ModelP &model, int column, Qt::SortOrder order)
{
  model->sort(column, order);

  return true;
}

//------

CQChartsModelData *
CQChartsCmds::
getModelDataOrCurrent(const QString &id)
{
  if (id != "")
    return getModelData(id);

  return charts_->currentModelData();
}

CQChartsModelData *
CQChartsCmds::
getModelData(const QString &id)
{
  auto *modelData = charts_->getModelDataById(id);

  if (! modelData) {
    bool ok;

    long ind = CQChartsUtil::toInt(id.trimmed(), ok);

    modelData = charts_->getModelDataByInd(int(ind));
  }

  return modelData;
}

CQChartsView *
CQChartsCmds::
getView(bool reuse)
{
  CQChartsView *view = nullptr;

  if (reuse)
    view = charts_->currentView();

  if (! view)
    view = addView();

  return view;
}

CQChartsView *
CQChartsCmds::
addView()
{
  auto *view = charts_->addView();

  // TODO: handle multiple windows
  auto *window = charts_->createWindow(view);
  assert(window);

  return view;
}

QStringList
CQChartsCmds::
modelArgValues() const
{
  QStringList names;

  CQCharts::ModelDatas modelDatas;

  charts_->getModelDatas(modelDatas);

  for (auto &modelData : modelDatas)
    names.push_back(modelData->id());

  return names;
}

QStringList
CQChartsCmds::
viewArgValues() const
{
  QStringList names;

  CQCharts::Views views;

  charts_->getViews(views);

  for (auto &view : views)
    names.push_back(view->id());

  return names;
}

QStringList
CQChartsCmds::
plotArgValues(CQChartsView *view) const
{
  QStringList names;

  CQChartsView::Plots plots;

  if (view) {
    view->getPlots(plots);
  }
  else {
    CQCharts::Views views;

    charts_->getViews(views);

    for (auto &view : views)
      view->getPlots(plots);
  }

  for (const auto &plot : plots)
    names.push_back(plot->pathId());

  return names;
}

QStringList
CQChartsCmds::
plotTypeArgValues() const
{
  QStringList names, descs;

  charts_->getPlotTypeNames(names, descs);

  return names;
}

QStringList
CQChartsCmds::
annotationArgValues(CQChartsView *view, CQChartsPlot *plot) const
{
  QStringList names;

  if (view) {
    const auto &annotations = view->annotations();

    for (const auto &annotation : annotations)
      names.push_back(annotation->pathId());
  }
  else if (plot) {
    const auto &annotations = plot->annotations();

    for (const auto &annotation : annotations)
      names.push_back(annotation->pathId());
  }
  else {
    CQCharts::Views     views;
    CQChartsView::Plots plots;

    charts_->getViews(views);

    for (auto &view : views) {
      const auto &annotations = view->annotations();

      for (const auto &annotation : annotations)
        names.push_back(annotation->pathId());

      view->getPlots(plots);

      for (auto &plot : plots) {
        const auto &annotations = plot->annotations();

        for (const auto &annotation : annotations)
          names.push_back(annotation->pathId());
      }
    }
  }

  return names;
}

QStringList
CQChartsCmds::
roleArgValues(QAbstractItemModel *model) const
{
  return CQChartsModelUtil::roleNames(model);
}

bool
CQChartsCmds::
stringToModelColumns(const ModelP &model, const QString &columnsStr, Columns &columns)
{
  // split into strings per column
  QStringList columnStrs;

  if (! CQTcl::splitList(columnsStr, columnStrs)) {
    charts_->errorMsg("Bad columns '" + columnsStr + "'");
    return false;
  }

  for (const auto &columnStr : columnStrs) {
    CQChartsColumn column;

    if (! CQChartsModelUtil::stringToColumn(model.data(), columnStr, column)) {
      charts_->errorMsg("Bad column '" + columnStr + "'");
      return false;
    }

    columns.push_back(column);
  }

  return true;
}
