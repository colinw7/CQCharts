#include <CQChartsCmds.h>
#include <CQChartsCmdArgs.h>
#include <CQChartsLoader.h>

#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
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

#include <CQChartsLoadModelDlg.h>
#include <CQChartsManageModelsDlg.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQChartsFilterModel.h>
#include <CQChartsAnalyzeModel.h>
#include <CQChartsHelpDlg.h>

#include <CQColors.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>

#include <CQDataModel.h>
#include <CQSortModel.h>
#include <CQBucketModel.h>
#include <CQFoldedModel.h>
#include <CQSubSetModel.h>
#include <CQTransposeModel.h>
#include <CQSummaryModel.h>
#include <CQCollapseModel.h>
#include <CQPivotModel.h>
#include <CQPerfMonitor.h>
#include <CQCsvModel.h>
#include <CQTsvModel.h>

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
#include <QVBoxLayout>
#include <QFont>
#include <fstream>

//------

CQChartsCmds::
CQChartsCmds(CQCharts *charts) :
 charts_(charts)
{
  cmdBase_ = new CQChartsCmdBase;

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
    addCommand("sort_charts_model"   , new CQChartsSortChartsModelCmd   (this));
    addCommand("fold_charts_model"   , new CQChartsFoldChartsModelCmd   (this));
    addCommand("filter_charts_model" , new CQChartsFilterChartsModelCmd (this));
    addCommand("flatten_charts_model", new CQChartsFlattenChartsModelCmd(this));
    addCommand("copy_charts_model"   , new CQChartsCopyChartsModelCmd   (this));
    addCommand("export_charts_model" , new CQChartsExportChartsModelCmd (this));
    addCommand("write_charts_model"  , new CQChartsWriteChartsModelCmd  (this));
    addCommand("remove_charts_model" , new CQChartsRemoveChartsModelCmd (this));

    // define charts tcl proc
    addCommand("define_charts_proc", new CQChartsDefineChartsProcCmd(this));

    // correlation, bucket, folded, subset, transpose, summary, collapse, pivot, stats
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

    // measure/encode text
    addCommand("measure_charts_text", new CQChartsMeasureChartsTextCmd(this));
    addCommand("encode_charts_text" , new CQChartsEncodeChartsTextCmd (this));

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

    // get/set charts model data
    addCommand("get_charts_data", new CQChartsGetChartsDataCmd(this));
    addCommand("set_charts_data", new CQChartsSetChartsDataCmd(this));

    // annotations
    addCommand("create_charts_arrow_annotation"    ,
               new CQChartsCreateChartsArrowAnnotationCmd    (this));
    addCommand("create_charts_ellipse_annotation"  ,
               new CQChartsCreateChartsEllipseAnnotationCmd  (this));
    addCommand("create_charts_point_annotation"    ,
               new CQChartsCreateChartsPointAnnotationCmd    (this));
    addCommand("create_charts_polygon_annotation"  ,
               new CQChartsCreateChartsPolygonAnnotationCmd  (this));
    addCommand("create_charts_polyline_annotation" ,
               new CQChartsCreateChartsPolylineAnnotationCmd (this));
    addCommand("create_charts_rectangle_annotation",
               new CQChartsCreateChartsRectangleAnnotationCmd(this));
    addCommand("create_charts_text_annotation"     ,
               new CQChartsCreateChartsTextAnnotationCmd     (this));
    addCommand("create_charts_image_annotation"     ,
               new CQChartsCreateChartsImageAnnotationCmd    (this));
    addCommand("remove_charts_annotation"          ,
               new CQChartsRemoveChartsAnnotationCmd         (this));

    // theme/palette
    addCommand("create_charts_palette", new CQChartsCreateChartsPaletteCmd(this));
    addCommand("get_charts_palette"   , new CQChartsGetChartsPaletteCmd   (this));
    addCommand("set_charts_palette"   , new CQChartsSetChartsPaletteCmd   (this));

    // connect
    addCommand("connect_charts_signal", new CQChartsConnectChartsSignalCmd(this));

    // print, write
    addCommand("print_charts_image", new CQChartsPrintChartsImageCmd(this));
    addCommand("write_charts_data" , new CQChartsWriteChartsDataCmd(this));

    // dialogs
    addCommand("show_charts_load_model_dlg"   , new CQChartsShowChartsLoadModelDlgCmd(this));
    addCommand("show_charts_manage_models_dlg", new CQChartsShowChartsManageModelsDlgCmd(this));
    addCommand("show_charts_create_plot_dlg"  , new CQChartsShowChartsCreatePlotDlgCmd(this));
    addCommand("show_charts_help_dlg"         , new CQChartsShowChartsHelpDlgCmd(this));

    // test
    //addCommand("charts::test_edit", new CQChartsTestEditCmd(this));
    addCommand("test_charts_edit", new CQChartsTestEditCmd(this));

    //---

    cmdBase_->addCommands();

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

// load model from data
bool
CQChartsCmds::
loadChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::loadChartsModelCmd");

  // input data type
  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-csv" , CQChartsCmdArg::Type::Boolean, "load csv file");
  argv.addCmdArg("-tsv" , CQChartsCmdArg::Type::Boolean, "load tsv file");
  argv.addCmdArg("-json", CQChartsCmdArg::Type::Boolean, "load json file");
  argv.addCmdArg("-data", CQChartsCmdArg::Type::Boolean, "load gnuplot file");
  argv.addCmdArg("-expr", CQChartsCmdArg::Type::Boolean, "use expression model");
  argv.addCmdArg("-var" , CQChartsCmdArg::Type::String , "load from tcl variable(s)");
  argv.endCmdGroup();

  // input data control
  argv.addCmdArg("-comment_header"     , CQChartsCmdArg::Type::Boolean,
                 "first comment is horizontal header");
  argv.addCmdArg("-first_line_header"  , CQChartsCmdArg::Type::Boolean,
                 "first line is horizontal header");
  argv.addCmdArg("-first_column_header", CQChartsCmdArg::Type::Boolean,
                 "first column is vertical header");

  argv.addCmdArg("-separator", CQChartsCmdArg::Type::String , "separator char for csv");
  argv.addCmdArg("-columns"  , CQChartsCmdArg::Type::String , "column types");
  argv.addCmdArg("-transpose", CQChartsCmdArg::Type::Boolean, "transpose tcl data");

  argv.addCmdArg("-num_rows"   , CQChartsCmdArg::Type::Integer, "number of expression rows");
  argv.addCmdArg("-max_rows"   , CQChartsCmdArg::Type::Integer, "maximum number of file rows");
  argv.addCmdArg("-filter"     , CQChartsCmdArg::Type::String , "filter expression");
  argv.addCmdArg("-column_type", CQChartsCmdArg::Type::String , "column type");
  argv.addCmdArg("-name"       , CQChartsCmdArg::Type::String , "name for model");

  argv.addCmdArg("filename", CQChartsCmdArg::Type::String, "file name");

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
    QStringList strs = argv.getParseStrs("var");

    for (int i = 0; i < strs.length(); ++i)
      inputData.vars.push_back(strs[i]);

    fileType = CQChartsFileType::VARS;
  }

  inputData.commentHeader     = argv.getParseBool("comment_header"     );
  inputData.firstLineHeader   = argv.getParseBool("first_line_header"  );
  inputData.firstColumnHeader = argv.getParseBool("first_column_header");

  inputData.separator = argv.getParseStr("separator");

  QString columnsStr = argv.getParseStr("columns");

  if (! CQTcl::splitList(columnsStr, inputData.columns))
    return errorMsg(QString("Invalid columns string '%1'").arg(columnsStr));

  inputData.transpose = argv.getParseBool("transpose");

  if (argv.hasParseArg("num_rows"))
    inputData.numRows = std::max(argv.getParseInt("num_rows"), 1);

  if (argv.hasParseArg("max_rows"))
    inputData.maxRows = std::max(argv.getParseInt("max_rows"), 1);

  inputData.filter = argv.getParseStr("filter");

  QStringList columnTypes = argv.getParseStrs("column_type");

  QString name = argv.getParseStr("name");

  // TODO: columns (filter to columns)

  const Vars &filenameArgs = argv.getParseArgs();

  QString filename = (! filenameArgs.empty() ? filenameArgs[0].toString() : "");

  //---

  if (fileType == CQChartsFileType::NONE)
    return errorMsg("No file type");

  if (fileType != CQChartsFileType::EXPR && fileType != CQChartsFileType::VARS) {
    if (filename == "")
      return errorMsg("No filename");
  }
  else {
    if (filename != "")
      return errorMsg("Extra filename");
  }

  if (! loadFileModel(filename, fileType, inputData))
    return false;

  CQChartsModelData *modelData = charts_->currentModelData();

  if (! modelData)
    return false;

  if (columnTypes.length()) {
    ModelP model = modelData->currentModel();

    for (int i = 0; i < columnTypes.length(); ++i) {
      const QString &columnType = columnTypes[i];

      if (! CQChartsModelUtil::setColumnTypeStrs(charts_, model.data(), columnType))
        return errorMsg(QString("Invalid column type string '%1'").arg(columnType));
    }
  }

  if (name.length())
    modelData->setName(name);

  cmdBase_->setCmdRc(modelData->ind());

  return true;
}

//------

bool
CQChartsCmds::
processChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::processChartsModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model index").setRequired();
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column ,
                 "column for delete, modify, calc, query, analyze");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-add"    , CQChartsCmdArg::Type::Boolean, "add column");
  argv.addCmdArg("-delete" , CQChartsCmdArg::Type::Boolean, "delete column");
  argv.addCmdArg("-modify" , CQChartsCmdArg::Type::Boolean, "modify column values");
  argv.addCmdArg("-calc"   , CQChartsCmdArg::Type::Boolean, "calc column");
  argv.addCmdArg("-query"  , CQChartsCmdArg::Type::Boolean, "query column");
  argv.addCmdArg("-analyze", CQChartsCmdArg::Type::Boolean, "analyze data");
  argv.endCmdGroup();

  argv.addCmdArg("-header", CQChartsCmdArg::Type::String, "header label for add/modify");
  argv.addCmdArg("-type"  , CQChartsCmdArg::Type::String, "type data for add/modify");
  argv.addCmdArg("-expr"  , CQChartsCmdArg::Type::String, "expression for add/modify/calc/query");

  argv.addCmdArg("-force", CQChartsCmdArg::Type::Boolean, "force modify of original data");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int modelInd = argv.getParseInt("model", -1);

  QString header = argv.getParseStr("header");
  QString type   = argv.getParseStr("type");
  QString expr   = argv.getParseStr("expr");

  //---

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  //---

  // get expr model
  ModelP model = modelData->currentModel();

  CQChartsExprModel *exprModel = CQChartsModelUtil::getExprModel(model.data());

  //---

  // add new column (values from result of expression)
  if      (argv.getParseBool("add")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");

    int column;

    if (! exprModel->addExtraColumn(header, expr, column))
      return errorMsg("Failed to add column");

    //---

    if (type.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), column, type))
        return errorMsg(QString("Invalid column type '%1'").arg(type));
    }

    cmdBase_->setCmdRc(column);
  }
  // remove column (must be an added one)
  else if (argv.getParseBool("delete")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    if (! exprModel->removeExtraColumn(column.column()))
      return errorMsg("Failed to delete column");

    cmdBase_->setCmdRc(-1);
  }
  // modify column (values from result of expression)
  else if (argv.getParseBool("modify")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");

    CQChartsColumn column = argv.getParseColumn("column", model.data());

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
      if (! CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), column, type))
        return errorMsg(QString("Invalid column type '%1'").arg(type));
    }

    cmdBase_->setCmdRc(column.column());
  }
  // calculate values from result of expression
  else if (argv.getParseBool("calc")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    CQChartsExprModel::Values values;

    exprModel->calcColumn(column.column(), expr, values);

    QVariantList vars;

    for (const auto &var : values)
      vars.push_back(var);

    cmdBase_->setCmdRc(vars);
  }
  // query rows where result of expression is true
  else if (argv.getParseBool("query")) {
    if (! exprModel)
      return errorMsg("Expression not supported for model");

    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    CQChartsExprModel::Rows rows;

    exprModel->queryColumn(column.column(), expr, rows);

    using QVariantList = QList<QVariant>;

    QVariantList vars;

    for (const auto &row : rows)
      vars.push_back(row);

    cmdBase_->setCmdRc(vars);
  }
  else if (argv.getParseBool("analyze")) {
    CQChartsAnalyzeModel analyzeModel(charts_, modelData);

    if (type != "") {
      if (! charts_->isPlotType(type))
        return errorMsg("Invalid type '" + type + "'");

      CQChartsPlotType *plotType = charts_->plotType(type);
      assert(plotType);

      analyzeModel.analyzeType(plotType);
    }
    else {
      analyzeModel.analyze();
    }

    const CQChartsAnalyzeModel::TypeNameColumns &typeNameColumns = analyzeModel.typeNameColumns();

    QVariantList tvars;

    for (const auto &tnc : typeNameColumns) {
      QVariantList cvars;

      const QString &typeName = tnc.first;

      cvars.push_back(typeName);

      QVariantList tncvars;

      const CQChartsAnalyzeModel::NameColumns &nameColumns = tnc.second;

      for (const auto &nc : nameColumns) {
        const QString        &name   = nc.first;
        const CQChartsColumn &column = nc.second;

        QVariantList ncvars;

        ncvars.push_back(name);
        ncvars.push_back(column.toString());

        tncvars.push_back(ncvars);
      }

      cvars.push_back(tncvars);

      tvars.push_back(cvars);
    }

    analyzeModel.print();

    cmdBase_->setCmdRc(tvars);
  }
  else {
#if 0
    if (! argv.hasParseArg("expr"))
      return errorMsg("Missing expression");
    }

    CQChartsExprModel::Function function = CQChartsExprModel::Function::EVAL;

    if (expr.simplified().length())
      processExpression(model.data(), expr);
#endif
  }

  return true;
}

//------

bool
CQChartsCmds::
defineChartsProcCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::defineChartsProcCmd");

  argv.addCmdArg("name", CQChartsCmdArg::Type::String, "proc name").setRequired();
  argv.addCmdArg("args", CQChartsCmdArg::Type::String, "proc args").setRequired();
  argv.addCmdArg("body", CQChartsCmdArg::Type::String, "proc body").setRequired();

  bool rc;

  if (! argv.parse(rc))
    return rc;

  const Vars &pargs = argv.getParseArgs();

  if (pargs.size() != 3)
    return errorMsg("Usage: define_charts_proc <name> <args> <body>");

  //---

  QString name = pargs[0].toString();
  QString args = pargs[1].toString();
  QString body = pargs[2].toString();

  //---

  charts_->addProc(name, args, body);

  return true;
}

//------

bool
CQChartsCmds::
measureChartsTextCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::measureChartsTextCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String , "value name").setRequired();
  argv.addCmdArg("-text", CQChartsCmdArg::Type::String , "text string");
  argv.addCmdArg("-html", CQChartsCmdArg::Type::Boolean, "is html");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(view, plotName);
    if (! plot) return false;
  }

  //---

  QString name = argv.getParseStr ("name");
  QString text = argv.getParseStr ("text");
  bool    html = argv.getParseBool("html");

  //---

  QFont font;

  if      (plot)
    font = plot->view()->plotFont(plot, plot->view()->font());
  else if (view)
    font = view->viewFont(view->font());

  double tw = 0.0, ta = 0.0, td = 0.0;

  if (! html) {
    QFontMetricsF fm(font);

    tw = fm.width(text);
    ta = fm.ascent();
    td = fm.descent();
  }
  else {
    QTextDocument tdoc;

    tdoc.setHtml(text);
    tdoc.setDefaultFont(font);

    QAbstractTextDocumentLayout *layout = tdoc.documentLayout();

    QSizeF size = layout->documentSize();

    tw = size.width ();
    ta = size.height();
    td = 0.0;
  }

  if      (name == "width") {
    if      (plot)
      cmdBase_->setCmdRc(plot->pixelToWindowWidth(tw));
    else if (view)
      cmdBase_->setCmdRc(view->pixelToWindowWidth(tw));
  }
  else if (name == "height") {
    if      (plot)
      cmdBase_->setCmdRc(plot->pixelToWindowHeight(ta + td));
    else if (view)
      cmdBase_->setCmdRc(view->pixelToWindowHeight(ta + td));
  }
  else if (name == "ascent") {
    if      (plot)
      cmdBase_->setCmdRc(plot->pixelToWindowHeight(ta));
    else if (view)
      cmdBase_->setCmdRc(view->pixelToWindowHeight(ta));
  }
  else if (name == "descent") {
    if      (plot)
      cmdBase_->setCmdRc(plot->pixelToWindowHeight(td));
    else if (view)
      cmdBase_->setCmdRc(view->pixelToWindowHeight(td));
  }
  else if (name == "?") {
    QStringList names = QStringList() <<
      "width" << "height" << "ascent" << "descent";

    cmdBase_->setCmdRc(names);
  }
  else {
    return errorMsg(QString("Invalid value name '%1'").arg(name));
  }

  return true;
}

//------

bool
CQChartsCmds::
encodeChartsTextCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::encodeChartsTextCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-csv", CQChartsCmdArg::Type::Boolean, "encode text for csv");
  argv.addCmdArg("-tsv", CQChartsCmdArg::Type::Boolean, "encode text for tsv");
  argv.endCmdGroup();

  argv.addCmdArg("-text", CQChartsCmdArg::Type::String, "text string");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool    csv  = argv.getParseBool("csv" );
  bool    tsv  = argv.getParseBool("tsv" );
  QString text = argv.getParseStr ("text");

  QString text1 = text;

  if      (csv)
    text1 = CQCsvModel::encodeString(text);
  else if (tsv)
    text1 = CQTsvModel::encodeString(text);

  cmdBase_->setCmdRc(text1);

  return true;
}

//------

bool
CQChartsCmds::
createChartsViewCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsViewCmd");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = addView();

  //---

  cmdBase_->setCmdRc(view->id());

  return true;
}

//------

bool
CQChartsCmds::
removeChartsViewCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::removeChartsViewCmd");

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view_id").setRequired();

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  QString viewName = argv.getParseStr("view");

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return false;

  //---

  CQChartsWindow *window = CQChartsWindowMgrInst->getWindowForView(view);

  charts_->deleteWindow(window);

  return true;
}

//------

bool
CQChartsCmds::
createChartsPlotCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsPlotCmd");

  argv.addCmdArg("-view" , CQChartsCmdArg::Type::String , "view_id"  ).setRequired();
  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model_ind").setRequired();
  argv.addCmdArg("-type" , CQChartsCmdArg::Type::String , "type"     ).setRequired();
  argv.addCmdArg("-id"   , CQChartsCmdArg::Type::String , "plot id"  );

  argv.addCmdArg("-where"     , CQChartsCmdArg::Type::String, "filter");
  argv.addCmdArg("-columns"   , CQChartsCmdArg::Type::String, "columns");
  argv.addCmdArg("-parameter" , CQChartsCmdArg::Type::String, "name value").setHidden(true);

  argv.addCmdArg("-xintegral" , CQChartsCmdArg::Type::SBool).setHidden(true);
  argv.addCmdArg("-yintegral" , CQChartsCmdArg::Type::SBool).setHidden(true);
  argv.addCmdArg("-xlog"      , CQChartsCmdArg::Type::SBool).setHidden(true);
  argv.addCmdArg("-ylog"      , CQChartsCmdArg::Type::SBool).setHidden(true);

  argv.addCmdArg("-title"     , CQChartsCmdArg::Type::String, "title");
  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");
  argv.addCmdArg("-position"  , CQChartsCmdArg::Type::String, "position box");
  argv.addCmdArg("-xmin"      , CQChartsCmdArg::Type::Real  , "x");
  argv.addCmdArg("-ymin"      , CQChartsCmdArg::Type::Real  , "y");
  argv.addCmdArg("-xmax"      , CQChartsCmdArg::Type::Real  , "x");
  argv.addCmdArg("-ymax"      , CQChartsCmdArg::Type::Real  , "y");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  QString     viewName    = argv.getParseStr    ("view");
  int         modelInd    = argv.getParseInt    ("model", -1);
  QString     typeName    = argv.getParseStr    ("type");
  QString     id          = argv.getParseStr    ("id");
  QString     filterStr   = argv.getParseStr    ("where");
  QString     title       = argv.getParseStr    ("title");
  QStringList properties  = argv.getParseStrs   ("properties");
  QString     positionStr = argv.getParseStr    ("position");
  OptReal     xmin        = argv.getParseOptReal("xmin");
  OptReal     ymin        = argv.getParseOptReal("ymin");
  OptReal     xmax        = argv.getParseOptReal("xmax");
  OptReal     ymax        = argv.getParseOptReal("ymax");

  //---

  // get view
  CQChartsView *view = getViewByName(viewName);
  if (! view) return false;

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  //------

  CQChartsNameValueData nameValueData;

  //--

  // plot columns
  QStringList columnsStrs = argv.getParseStrs("columns");

  for (int i = 0; i < columnsStrs.length(); ++i) {
    const QString &columnsStr = columnsStrs[i];

    if (! columnsStr.length())
      continue;

    QStringList strs;

    if (! CQTcl::splitList(columnsStr, strs))
      return errorMsg(QString("Invalid columns string '%1'").arg(columnsStr));

    //QStringList strs = stringToNamedColumns(columnsStr);

    for (int j = 0; j < strs.size(); ++j) {
      const QString &nameValue = strs[j];

#if 1
      QStringList strs1;

      if (! CQTcl::splitList(nameValue, strs1))
        return errorMsg(QString("Invalid column name/value string '%1'").arg(nameValue));

      if (strs1.length() != 2)
        return errorMsg(QString("Invalid column name/value string '%1'").arg(nameValue));

      auto name  = strs1[0];
      auto value = strs1[1];
#else
      auto pos = nameValue.indexOf('=');

      if (pos < 0)
        return errorMsg("Invalid -columns option '" + columnsStr + "'");

      auto name  = nameValue.mid(0, pos).simplified();
      auto value = nameValue.mid(pos + 1).simplified();
#endif

      nameValueData.columns[name] = value;
    }
  }

  //--

  // plot parameter
  QStringList parameterStrs = argv.getParseStrs("parameter");

  for (int i = 0; i < parameterStrs.length(); ++i) {
    const QString &parameterStr = parameterStrs[i];

    QString name, value;

#if 1
    QStringList strs1;

    if (! CQTcl::splitList(parameterStr, strs1))
      return errorMsg(QString("Invalid parameter name/value string '%1'").arg(parameterStr));

    if (strs1.length() != 2)
      return errorMsg(QString("Invalid parameter name/value string '%1'").arg(parameterStr));

    name  = strs1[0];
    value = strs1[1];
#else
    auto pos = parameterStr.indexOf('=');

    if (pos >= 0) {
      name  = parameterStr.mid(0, pos).simplified();
      value = parameterStr.mid(pos + 1).simplified();
    }
    else {
      name  = parameterStr;
      value = "true";
    }
#endif

    nameValueData.parameters[name] = value;
  }

  //------

  if (typeName == "")
    return errorMsg("No type specified for plot");

  typeName = fixTypeName(typeName);

  // ignore if bad type
  if (! charts_->isPlotType(typeName))
    return errorMsg("Invalid type '" + typeName + "' for plot");

  CQChartsPlotType *type = charts_->plotType(typeName);
  assert(type);

  //------

  double vr = CQChartsView::viewportRange();

  CQChartsGeom::BBox bbox(0, 0, vr, vr);

  if (positionStr != "") {
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
  CQChartsPlot *plot = createPlot(view, model, type, true);

  if (! plot)
    return errorMsg("Failed to create plot");

  //---

  plot->setUpdatesEnabled(false);

  if (! initPlot(plot, nameValueData, bbox))
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

    plot->xAxis()->setValueType(xintegral ? CQChartsAxisValueType::Type::INTEGER :
                                            CQChartsAxisValueType::Type::REAL);
  }

  if (argv.hasParseArg("yintegral")) {
    bool yintegral = argv.getParseBool("yintegral");

    if (! type->allowYAxisIntegral() || ! plot->yAxis())
      return errorMsg("plot type does not support y integral option");

    plot->yAxis()->setValueType(yintegral ? CQChartsAxisValueType::Type::INTEGER :
                                            CQChartsAxisValueType::Type::REAL);
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
    QStringList strs;

    if (! CQTcl::splitList(properties[i], strs))
      return errorMsg(QString("Invalid properties string '%1'").arg(properties[i]));

    for (int j = 0; j < strs.length(); ++j) {
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

  cmdBase_->setCmdRc(plot->pathId());

  return true;
}

//------

bool
CQChartsCmds::
removeChartsPlotCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::removeChartsPlotCmd");

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view_id").setRequired();

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot_id");
  argv.addCmdArg("-all" , CQChartsCmdArg::Type::Boolean);
  argv.endCmdGroup();

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  QString viewName = argv.getParseStr ("view");
  QString plotName = argv.getParseStr ("plot");
  bool    all      = argv.getParseBool("all");

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return false;

  if (all) {
    view->removeAllPlots();
  }
  else {
    CQChartsPlot *plot = getPlotByName(view, plotName);
    if (! plot) return false;

    view->removePlot(plot);
  }

  return true;
}

//------

bool
CQChartsCmds::
getChartsPropertyCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::getChartsPropertyCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view"      , CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot"      , CQChartsCmdArg::Type::String, "plot name");
  argv.addCmdArg("-annotation", CQChartsCmdArg::Type::String, "annotation name");
  argv.endCmdGroup();

  argv.addCmdArg("-object", CQChartsCmdArg::Type::String , "object id");
  argv.addCmdArg("-name"  , CQChartsCmdArg::Type::String , "property name");
  argv.addCmdArg("-data"  , CQChartsCmdArg::Type::String , "return property name data");
  argv.addCmdArg("-hidden", CQChartsCmdArg::Type::Boolean, "include hidden data").setHidden(true);

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

  QString objectId = argv.getParseStr ("object");
  QString name     = argv.getParseStr ("name");
  bool    hidden   = argv.getParseBool("hidden");

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);

    if (! view)
      return errorMsg("Invalid view '" + viewName + "'");

    if      (name == "?") {
      QStringList names;

      view->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (argv.hasParseArg("data")) {
      QString data = argv.getParseStr("data");

      if      (data == "desc") {
        QString desc;

        if (! view->getPropertyDesc(name, desc, /*hidden*/true))
          return errorMsg("Failed to get view parameter description '" + name + "'");

        cmdBase_->setCmdRc(desc);
      }
      else if (data == "type") {
        QString type;

        if (! view->getPropertyType(name, type, /*hidden*/true))
          return errorMsg("Failed to get view parameter type '" + name + "'");

        cmdBase_->setCmdRc(type);
      }
      else if (data == "user_type") {
        QString type;

        if (! view->getPropertyUserType(name, type, /*hidden*/true))
          return errorMsg("Failed to get view parameter user type '" + name + "'");

        cmdBase_->setCmdRc(type);
      }
      else if (data == "owner") {
        QObject *obj = nullptr;

        if (! view->getPropertyObject(name, obj, /*hidden*/true))
          return errorMsg("Failed to get view parameter owner '" + name + "'");

        cmdBase_->setCmdRc(objToType(obj));
      }
      else if (data == "is_hidden") {
        bool hidden = false;

        if (! view->getPropertyIsHidden(name, hidden))
          return errorMsg("Failed to get view parameter hidden '" + name + "'");

        cmdBase_->setCmdRc(hidden);
      }
      else if (data == "is_style") {
        bool is_style = false;

        if (! view->getPropertyIsStyle(name, is_style))
          return errorMsg("Failed to get plot parameter is_style '" + name + "'");

        cmdBase_->setCmdRc(is_style);
      }
      else if (data == "?") {
        QStringList names = QStringList() <<
          "desc" << "type" << "user_type" << "owner" << "is_hidden" << "is_style";

        cmdBase_->setCmdRc(names);
      }
      else
        return errorMsg("Invalid view property name data '" + data + "'");
    }
    else {
      QVariant value;

      if (! view->getTclProperty(name, value))
        return errorMsg("Failed to get view parameter '" + name + "'");

      cmdBase_->setCmdRc(value);
    }
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsPlot *plot = getPlotByName(nullptr, plotName);

    if (! plot)
      return errorMsg("Invalid plot '" + plotName + "'");

    CQChartsPlotObj *plotObj = nullptr;

    if (objectId.length()) {
      plotObj = plot->getObject(objectId);

      if (! plotObj)
        return errorMsg("Invalid plot object id '" + objectId + "'");
    }

    // plot object property
    if (plotObj) {
      if (name == "?") {
        QStringList names;

        plot->getObjectPropertyNames(plotObj, names);

        cmdBase_->setCmdRc(names);
      }
      else {
        QVariant value;

        if (! CQUtil::getTclProperty(plotObj, name, value))
          return errorMsg("Failed to get plot parameter '" + name + "'");

        cmdBase_->setCmdRc(value);
      }
    }
    // plot property
    else {
      if      (name == "?") {
        QStringList names;

        plot->getPropertyNames(names, hidden);

        cmdBase_->setCmdRc(names);
      }
      else if (argv.hasParseArg("data")) {
        QString data = argv.getParseStr("data");

        if      (data == "desc") {
          QString desc;

          if (! plot->getPropertyDesc(name, desc, /*hidden*/true))
            return errorMsg("Failed to get plot parameter description '" + name + "'");

          cmdBase_->setCmdRc(desc);
        }
        else if (data == "type") {
          QString type;

          if (! plot->getPropertyType(name, type, /*hidden*/true))
            return errorMsg("Failed to get plot parameter type '" + name + "'");

          cmdBase_->setCmdRc(type);
        }
        else if (data == "user_type") {
          QString type;

          if (! plot->getPropertyUserType(name, type, /*hidden*/true))
            return errorMsg("Failed to get plot parameter user type '" + name + "'");

          cmdBase_->setCmdRc(type);
        }
        else if (data == "owner") {
          QObject *obj = nullptr;

          if (! plot->getPropertyObject(name, obj, /*hidden*/true))
            return errorMsg("Failed to get plot parameter owner '" + name + "'");

          cmdBase_->setCmdRc(objToType(obj));
        }
        else if (data == "is_hidden") {
          bool hidden = false;

          if (! plot->getPropertyIsHidden(name, hidden))
            return errorMsg("Failed to get plot parameter is_hidden '" + name + "'");

          cmdBase_->setCmdRc(hidden);
        }
        else if (data == "is_style") {
          bool is_style = false;

          if (! plot->getPropertyIsStyle(name, is_style))
            return errorMsg("Failed to get plot parameter is_style '" + name + "'");

          cmdBase_->setCmdRc(is_style);
        }
        else if (data == "?") {
          QStringList names = QStringList() <<
            "desc" << "type" << "user_type" << "owner" << "is_hidden" << "is_style";

          cmdBase_->setCmdRc(names);
        }
        else
          return errorMsg("Invalid plot property name data '" + data + "'");
      }
      else {
        QVariant value;

        if (! plot->getTclProperty(name, value))
          return errorMsg("Failed to get plot parameter '" + name + "'");

        cmdBase_->setCmdRc(value);
      }
    }
  }
  else if (argv.hasParseArg("annotation")) {
    QString annotationName = argv.getParseStr("annotation");

    CQChartsAnnotation *annotation = getAnnotationByName(annotationName);
    if (! annotation) return false;

    if      (name == "?") {
      QStringList names;

      annotation->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (argv.hasParseArg("data")) {
      QString data = argv.getParseStr("data");

      if      (data == "desc") {
        QString desc;

        if (! annotation->getPropertyDesc(name, desc, /*hidden*/true))
          return errorMsg("Failed to get annotation parameter description '" + name + "'");

        cmdBase_->setCmdRc(desc);
      }
      else if (data == "type") {
        QString type;

        if (! annotation->getPropertyType(name, type, /*hidden*/true))
          return errorMsg("Failed to get annotation parameter type '" + name + "'");

        cmdBase_->setCmdRc(type);
      }
      else if (data == "user_type") {
        QString type;

        if (! annotation->getPropertyUserType(name, type, /*hidden*/true))
          return errorMsg("Failed to get annotation parameter user type '" + name + "'");

        cmdBase_->setCmdRc(type);
      }
      else if (data == "owner") {
        QObject *obj = nullptr;

        if (! annotation->getPropertyObject(name, obj, /*hidden*/true))
          return errorMsg("Failed to get annotation parameter owner '" + name + "'");

        cmdBase_->setCmdRc(objToType(obj));
      }
      else if (data == "is_hidden") {
        bool hidden = false;

        if (! annotation->getPropertyIsHidden(name, hidden))
          return errorMsg("Failed to get annotation parameter hidden '" + name + "'");

        cmdBase_->setCmdRc(hidden);
      }
      else if (data == "is_style") {
        bool is_style = false;

        if (! annotation->getPropertyIsStyle(name, is_style))
          return errorMsg("Failed to get plot parameter is_style '" + name + "'");

        cmdBase_->setCmdRc(is_style);
      }
      else if (data == "?") {
        QStringList names = QStringList() <<
          "desc" << "type" << "user_type" << "owner" << "is_hidden" << "is_style";

        cmdBase_->setCmdRc(names);
      }
      else
        return errorMsg("Invalid annotation property name data '" + data + "'");
    }
    else {
      QVariant value;

      if (! annotation->getTclProperty(name, value))
        return errorMsg("Failed to get annotation parameter '" + name + "'");

      cmdBase_->setCmdRc(value);
    }
  }

  return true;
}

//------

bool
CQChartsCmds::
setChartsPropertyCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::setChartsPropertyCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view"      , CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot"      , CQChartsCmdArg::Type::String, "plot name");
  argv.addCmdArg("-annotation", CQChartsCmdArg::Type::String, "annotation name");
  argv.endCmdGroup();

  argv.addCmdArg("-name" , CQChartsCmdArg::Type::String, "property name");
  argv.addCmdArg("-value", CQChartsCmdArg::Type::String, "property view");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  QString name  = argv.getParseStr("name");
  QString value = argv.getParseStr("value");

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);

    if (! view)
      return errorMsg("Invalid view '" + viewName + "'");

    if (! view->setProperty(name, value))
      return errorMsg("Failed to set view parameter '" + name + "'");
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsPlot *plot = getPlotByName(nullptr, plotName);

    if (! plot)
      return errorMsg("Invalid plot '" + plotName + "'");

    if (! plot->setProperty(name, value))
      return errorMsg("Failed to set plot parameter '" + name + "'");
  }
  else if (argv.hasParseArg("annotation")) {
    QString annotationName = argv.getParseStr("annotation");

    CQChartsAnnotation *annotation = getAnnotationByName(annotationName);
    if (! annotation) return false;

    if (! annotation->setProperty(name, value))
      return errorMsg("Failed to set annotation parameter '" + name + "'");
  }

  return true;
}

//------

bool
CQChartsCmds::
createChartsPaletteCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsPaletteCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-theme"  , CQChartsCmdArg::Type::String , "new theme name");
  argv.addCmdArg("-palette", CQChartsCmdArg::Type::String , "new named name");
  argv.endCmdGroup();

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool    themeFlag   = argv.hasParseArg("theme"  );
  QString themeStr    = argv.getParseStr("theme"  );
  bool    paletteFlag = argv.hasParseArg("palette");
  QString paletteStr  = argv.getParseStr("palette");

  if      (themeFlag) {
    if (CQColorsMgrInst->getNamedTheme(themeStr))
      return errorMsg(QString("Theme %1 already exists").arg(themeStr));

    (void) CQColorsMgrInst->createTheme(themeStr);

    cmdBase_->setCmdRc(themeStr);
  }
  else if (paletteFlag) {
    if (CQColorsMgrInst->getNamedPalette(paletteStr))
      return errorMsg(QString("Palette %1 already exists").arg(paletteStr));

    (void) CQColorsMgrInst->createPalette(paletteStr);

    cmdBase_->setCmdRc(paletteStr);
  }

  return true;
}

//------

bool
CQChartsCmds::
getChartsPaletteCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::getChartsPaletteCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-theme"    , CQChartsCmdArg::Type::String , "get theme data");
  argv.addCmdArg("-palette"  , CQChartsCmdArg::Type::String , "get named palette data");
  argv.addCmdArg("-interface", CQChartsCmdArg::Type::Boolean, "get interface data");
  argv.endCmdGroup();

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String, "value name").setRequired();
  argv.addCmdArg("-data", CQChartsCmdArg::Type::String, "value name data");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool    themeFlag     = argv.hasParseArg ("theme"    );
  QString themeStr      = argv.getParseStr ("theme"    );
  bool    paletteFlag   = argv.hasParseArg ("palette"  );
  QString paletteStr    = argv.getParseStr ("palette"  );
  bool    interfaceFlag = argv.getParseBool("interface");

  QString nameStr = argv.getParseStr("name");

  bool    dataFlag = argv.hasParseArg("data");
  QString dataStr  = argv.getParseStr("data");

  //---

  // get global data
  if      (! themeFlag && ! paletteFlag && ! interfaceFlag) {
    if      (nameStr == "palettes") {
      QStringList names;

      CQColorsMgrInst->getPaletteNames(names);

      cmdBase_->setCmdRc(names);
    }
    else if (nameStr == "themes") {
      QStringList names;

      CQColorsMgrInst->getThemeNames(names);

      cmdBase_->setCmdRc(names);
    }
    else if (nameStr == "color_models") {
      int n = CQColorsPalette::numModels();

      QStringList names;

      for (int i = 0; i < n; ++i)
        names << CQColorsPalette::modelName(i).c_str();

      cmdBase_->setCmdRc(names);
    }
    else if (nameStr == "?") {
      QStringList names = QStringList() <<
        "palettes" << "themes" << "color_models";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid theme value name '%1'").arg(nameStr));
  }
  // get theme data
  else if (themeFlag) {
    CQColorsTheme *theme = CQColorsMgrInst->getNamedTheme(themeStr);
    if (! theme) return errorMsg(QString("Invalid theme '%1'").arg(themeStr));

    if      (nameStr == "name") {
      cmdBase_->setCmdRc(theme->name());
    }
    else if (nameStr == "desc") {
      cmdBase_->setCmdRc(theme->desc());
    }
    else if (nameStr == "palettes") {
      int n = theme->numPalettes();

      QStringList names;

      for (int i = 0; i < n; ++i)
        names << theme->palette(i)->name();

      cmdBase_->setCmdRc(names);
    }

#if 0
    else if (nameStr == "select_color") { cmdBase_->setCmdRc(theme->selectColor()); }
    else if (nameStr == "inside_color") { cmdBase_->setCmdRc(theme->insideColor()); }
#endif

    else if (nameStr == "?") {
#if 0
      QStringList names = QStringList() <<
        "name" << "desc" << "palettes" << "select_color" << "inside_color";
#else
      QStringList names = QStringList() <<
        "name" << "desc" << "palettes";
#endif

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid theme value name '%1'").arg(nameStr));
  }
  // get palette data
  else if (paletteFlag) {
    CQColorsPalette *palette = CQColorsMgrInst->getNamedPalette(paletteStr);
    if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(themeStr));

    if      (nameStr == "name") { cmdBase_->setCmdRc(palette->name()); }
    else if (nameStr == "desc") { cmdBase_->setCmdRc(palette->desc()); }

    else if (nameStr == "color_type") {
      cmdBase_->setCmdRc(CQColorsPalette::colorTypeToString(palette->colorType()));
    }
    else if (nameStr == "color_model") {
      cmdBase_->setCmdRc(CQColorsPalette::colorModelToString(palette->colorModel()));
    }

    // model
    else if (nameStr == "red_model"     ) { cmdBase_->setCmdRc(palette->redModel       ()); }
    else if (nameStr == "green_model"   ) { cmdBase_->setCmdRc(palette->greenModel     ()); }
    else if (nameStr == "blue_model"    ) { cmdBase_->setCmdRc(palette->blueModel      ()); }
    else if (nameStr == "gray"          ) { cmdBase_->setCmdRc(palette->isGray         ()); }
    else if (nameStr == "red_negative"  ) { cmdBase_->setCmdRc(palette->isRedNegative  ()); }
    else if (nameStr == "green_negative") { cmdBase_->setCmdRc(palette->isGreenNegative()); }
    else if (nameStr == "blue_negative" ) { cmdBase_->setCmdRc(palette->isBlueNegative ()); }
    else if (nameStr == "red_min"       ) { cmdBase_->setCmdRc(palette->redMin         ()); }
    else if (nameStr == "red_max"       ) { cmdBase_->setCmdRc(palette->redMax         ()); }
    else if (nameStr == "green_min"     ) { cmdBase_->setCmdRc(palette->greenMin       ()); }
    else if (nameStr == "green_max"     ) { cmdBase_->setCmdRc(palette->greenMax       ()); }
    else if (nameStr == "blue_min"      ) { cmdBase_->setCmdRc(palette->blueMin        ()); }
    else if (nameStr == "blue_max"      ) { cmdBase_->setCmdRc(palette->blueMax        ()); }

    // defined colors
    else if (nameStr == "defined_colors") {
      QVariantList vars;

      for (const auto &rc : palette->definedValueColors()) {
        double        r = rc.first;
        const QColor &c = rc.second;

        QStringList strs;

        strs << QString("%1").arg(r) << c.name();

        vars << strs;
      }

      cmdBase_->setCmdRc(vars);
    }
    else if (nameStr == "color") {
      if (! dataFlag) return errorMsg("Missing data for palette color");

      bool ok;

      int i = CQChartsUtil::toInt(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid color index '%1'").arg(dataStr));

      int n = palette->numDefinedColors();

      if (i < 0 || i >= n) return errorMsg(QString("Invalid color index '%1'").arg(dataStr));

      cmdBase_->setCmdRc(palette->definedColor(i));
    }

    else if (nameStr == "distinct") { cmdBase_->setCmdRc(palette->isDistinct()); }

    else if (nameStr == "interp_color") {
      if (! dataFlag) return errorMsg("Missing data for palette interp color");

      bool ok;

      double r = CQChartsUtil::toReal(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid interp color value '%1'").arg(dataStr));

      bool scale = false;

      QColor c = palette->getColor(r, scale);

      cmdBase_->setCmdRc(c);
    }

    else if (nameStr == "red_function"  ) { cmdBase_->setCmdRc(palette->redFunction  ()); }
    else if (nameStr == "green_function") { cmdBase_->setCmdRc(palette->greenFunction()); }
    else if (nameStr == "blue_function" ) { cmdBase_->setCmdRc(palette->blueFunction ()); }

    else if (nameStr == "cube_start"     ) { cmdBase_->setCmdRc(palette->cbStart       ()); }
    else if (nameStr == "cube_cycles"    ) { cmdBase_->setCmdRc(palette->cbCycles      ()); }
    else if (nameStr == "cube_saturation") { cmdBase_->setCmdRc(palette->cbSaturation  ()); }
    else if (nameStr == "cube_negative"  ) { cmdBase_->setCmdRc(palette->isCubeNegative()); }

    else if (nameStr == "?") {
      QStringList names = QStringList() <<
        "name" << "desc" << "color_type" << "color_model" <<
        "red_model" << "green_model" << "blue_model" << "gray" <<
        "red_negative" << "green_negative" << "blue_negative" <<
        "red_min" << "red_max" << "green_min" << "green_max" << "blue_min" << "blue_max" <<
        "colors" << "color" << "distinct" << "interp_color" <<
        "red_function" << "green_function" << "blue_function" <<
        "cube_start" << "cube_cycles" << "cube_saturation" << "cube_negative";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid palette value name '%1'").arg(nameStr));
  }
  // get interface data
  else if (interfaceFlag) {
    CQChartsInterfaceTheme *interface = charts_->interfaceTheme();
    assert(interface);

    if      (nameStr == "is_dark") {
      cmdBase_->setCmdRc(interface->isDark());
    }
    else if (nameStr == "interp_color") {
      if (! dataFlag) return errorMsg("Missing data for interface interp color");

      bool ok;

      double r = CQChartsUtil::toReal(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid interp color value '%1'").arg(dataStr));

      bool scale = false;

      QColor c = interface->interpColor(r, scale);

      cmdBase_->setCmdRc(c);
    }
    else if (nameStr == "?") {
      QStringList names = QStringList() <<
        "is_dark" << "interp_color";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid interface value name '%1'").arg(nameStr));
  }

  return true;
}

//------

bool
CQChartsCmds::
setChartsPaletteCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::setChartsPaletteCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-theme"    , CQChartsCmdArg::Type::String , "get theme data");
  argv.addCmdArg("-palette"  , CQChartsCmdArg::Type::String , "get named palette data");
  argv.addCmdArg("-interface", CQChartsCmdArg::Type::Boolean, "get interface data");
  argv.endCmdGroup();

  argv.addCmdArg("-name" , CQChartsCmdArg::Type::String, "value name").setRequired();
  argv.addCmdArg("-value", CQChartsCmdArg::Type::String, "value").setRequired();
  argv.addCmdArg("-data" , CQChartsCmdArg::Type::String, "value name data");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool    themeFlag     = argv.hasParseArg ("theme"    );
  QString themeStr      = argv.getParseStr ("theme"    );
  bool    paletteFlag   = argv.hasParseArg ("palette"  );
  QString paletteStr    = argv.getParseStr ("palette"  );
  bool    interfaceFlag = argv.getParseBool("interface");

  QString nameStr  = argv.getParseStr("name" );
  QString valueStr = argv.getParseStr("value");

//bool    dataFlag = argv.hasParseArg("data");
//QString dataStr  = argv.getParseStr("data");

  //---

  // set global data
  if      (! themeFlag && ! paletteFlag && ! interfaceFlag) {
    if (nameStr == "?") {
      QStringList names;

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid value name '%1'").arg(nameStr));
  }
  // set theme data
  else if (themeFlag) {
    CQColorsTheme *theme = CQColorsMgrInst->getNamedTheme(themeStr);
    if (! theme) return errorMsg(QString("Invalid theme '%1'").arg(themeStr));

    if      (nameStr == "name") theme->setName(valueStr);
    else if (nameStr == "desc") theme->setDesc(valueStr);

#if 0
    else if (nameStr == "select_color") { theme->setSelectColor(QColor(valueStr)); }
    else if (nameStr == "inside_color") { theme->setInsideColor(QColor(valueStr)); }
#endif

    else if (nameStr == "palettes") {
      QStringList strs;

      CQTcl::splitList(valueStr, strs);

      for (int i = 0; i < strs.length(); ++i) {
        CQColorsPalette *palette = CQColorsMgrInst->getNamedPalette(strs[i]);
        if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(strs[i]));
      }

      theme->setNamedPalettes(strs);
    }
    else if (nameStr == "?") {
#if 0
      QStringList names = QStringList() <<
        "name" << "desc" << "select_color" << "inside_color";
#else
      QStringList names = QStringList() <<
        "name" << "desc";
#endif

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid theme value name '%1'").arg(nameStr));
  }
  // set palette data
  else if (paletteFlag) {
    CQColorsPalette *palette = CQColorsMgrInst->getNamedPalette(paletteStr);
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

      int i = CQChartsUtil::toInt(valueStr, ok);
      if (! ok) return errorMsg(QString("Invalid model index '%1'").arg(valueStr));

      if      (nameStr == "red_model"  ) palette->setRedModel  (i);
      else if (nameStr == "green_model") palette->setGreenModel(i);
      else if (nameStr == "blue_model" ) palette->setBlueModel (i);
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
      using DefinedColor  = CQColorsPalette::DefinedColor;
      using DefinedColors = CQColorsPalette::DefinedColors;

      DefinedColors definedColors;

      //--

      QStringList strs;

      CQTcl::splitList(valueStr, strs);

      if (! strs.length()) return errorMsg(QString("Invalid defined colors '%1'").arg(valueStr));

      double dv = (strs.length() > 1 ? 1.0/(strs.length() - 1) : 0.0);

      for (int j = 0; j < strs.length(); ++j) {
        double v = j*dv;
        QColor c;

#if 1
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
#else
        int pos = strs[j].indexOf('=');

        if (pos > 0) {
          lhs = strs[j].mid(0, pos).simplified();
          rhs = strs[j].mid(pos + 1).simplified();
        }
        else {
          lhs    = strs[j];
          isPair = false;
        }
#endif

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

        definedColors.push_back(DefinedColor(v, c));
      }

      if (! definedColors.empty())
        palette->setDefinedColors(definedColors);
    }

    else if (nameStr == "?") {
      QStringList names = QStringList() <<
        "name" << "desc" << "color_type" << "color_model" <<
        "red_model" << "green_model" << "blue_model" <<
        "red_min" << "red_max" << "green_min" << "green_max" << "blue_min" << "blue_max" <<
        "distinct" << "red_function" << "green_function" << "blue_function" <<
        "cube_start" << "cube_cycles" << "cube_saturation" << "cube_negative" <<
        "defined_colors";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid palette value name '%1'").arg(nameStr));
  }
  // set interface data
  else if (interfaceFlag) {
    CQChartsInterfaceTheme *interface = charts_->interfaceTheme();
    assert(interface);

    if      (nameStr == "dark") {
      bool ok;

      bool b = CQChartsUtil::stringToBool(valueStr, &ok);
      if (! ok) return errorMsg(QString("Invalid boolean '%1'").arg(valueStr));

      interface->setDark(b);
    }

    else if (nameStr == "?") {
      QStringList names = QStringList() <<
        "dark";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid interface value name '%1'").arg(nameStr));
  }

  //---

#if 0
  view->updatePlots();

  CQChartsWindow *window = CQChartsWindowMgrInst->getWindowForView(view);

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

bool
CQChartsCmds::
groupChartsPlotsCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::groupChartsPlotsCmd");

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name").setRequired();

  argv.addCmdArg("-x1x2"   , CQChartsCmdArg::Type::Boolean, "use shared x axis");
  argv.addCmdArg("-y1y2"   , CQChartsCmdArg::Type::Boolean, "use shared y axis");
  argv.addCmdArg("-overlay", CQChartsCmdArg::Type::Boolean, "overlay");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  QString viewName = argv.getParseStr ("view");
  bool    x1x2     = argv.getParseBool("x1x2");
  bool    y1y2     = argv.getParseBool("y1y2");
  bool    overlay  = argv.getParseBool("overlay");

  const Vars &plotNames = argv.getParseArgs();

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return false;

  //---

  Plots plots;

  getPlotsByName(view, plotNames, plots);

  //---

  if      (x1x2) {
    if (plots.size() != 2)
      return errorMsg("Need 2 plots for x1x2");

    view->initX1X2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (y1y2) {
    if (plots.size() != 2)
      return errorMsg("Need 2 plots for y1y2");

    view->initY1Y2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (overlay) {
    if (plots.size() < 2)
      return errorMsg("Need 2 or more plots for overlay");

    view->initOverlay(plots, /*reset*/true);
  }
  else
    return errorMsg("No grouping specified");

  return true;
}

//------

bool
CQChartsCmds::
placeChartsPlotsCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::placeChartsPlotsCmd");

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name").setRequired();

  argv.addCmdArg("-vertical"  , CQChartsCmdArg::Type::Boolean, "place vertical");
  argv.addCmdArg("-horizontal", CQChartsCmdArg::Type::Boolean, "place horizontal");
  argv.addCmdArg("-rows"      , CQChartsCmdArg::Type::Integer, "place using n rows");
  argv.addCmdArg("-columns"   , CQChartsCmdArg::Type::Integer, "place using n columns");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  QString viewName   = argv.getParseStr ("view");
  bool    vertical   = argv.getParseBool("vertical");
  bool    horizontal = argv.getParseBool("horizontal");
  int     rows       = argv.getParseInt ("rows"   , -1);
  int     columns    = argv.getParseInt ("columns", -1);

  const Vars &plotNames = argv.getParseArgs();

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return false;

  //---

  Plots plots;

  getPlotsByName(view, plotNames, plots);

  //---

  view->placePlots(plots, vertical, horizontal, rows, columns, /*reset*/true);

  return true;
}

//------

bool
CQChartsCmds::
foldChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::foldChartsModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column to fold");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int modelInd = argv.getParseInt("model", -1);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->model();

  CQChartsColumn column = argv.getParseColumn("column", model.data());

  //---

  CQFoldData foldData(column.column());

  CQFoldedModel *foldedModel = new CQFoldedModel(model.data(), foldData);

  //---

  QSortFilterProxyModel *foldProxyModel = new QSortFilterProxyModel;

  foldProxyModel->setObjectName("foldProxyModel");

  foldProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  foldProxyModel->setSourceModel(foldedModel);

  ModelP foldedModelP(foldProxyModel);

  CQChartsModelData *foldedModelData = charts_->initModelData(foldedModelP);

  //---

  cmdBase_->setCmdRc(foldedModelData->ind());

  return true;
}

//------

bool
CQChartsCmds::
flattenChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::flattenChartsModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-group", CQChartsCmdArg::Type::Column , "grouping column id");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-mean" , CQChartsCmdArg::Type::Boolean, "calc mean of column values");
  argv.addCmdArg("-sum"  , CQChartsCmdArg::Type::Boolean, "calc sum of column values");
  argv.endCmdGroup();

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int  modelInd = argv.getParseInt ("model" , -1);
  bool meanFlag = argv.getParseBool("mean");
  bool sumFlag  = argv.getParseBool("sum");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  //---

  ModelP model = modelData->currentModel();

  CQChartsColumn groupColumn = argv.getParseColumn("group", model.data());

  //---

  class FlattenVisitor : public CQChartsModelVisitor {
   public:
    FlattenVisitor(CQCharts *charts, const CQChartsColumn &groupColumn) :
     charts_(charts), groupColumn_(groupColumn) {
    }

    State hierVisit(const QAbstractItemModel *model, const VisitData &data) override {
      ++hierRow_;

      bool ok;

      groupValue_[hierRow_] =
        CQChartsModelUtil::modelValue(charts_, model, data.row, 0, data.parent, ok);

      return State::OK;
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      int nc = numCols();

      if (isHierarchical()) {
        for (int c = 1; c < nc; ++c) {
          bool ok;

          QVariant var =
            CQChartsModelUtil::modelValue(charts_, model, data.row, c, data.parent, ok);

          if (ok)
            rowColValueSet_[hierRow_][c - 1].addValue(var);
        }
      }
      else if (groupColumn_.isValid()) {
        bool ok;

        QVariant groupVar =
          CQChartsModelUtil::modelValue(charts_, model, data.row, groupColumn_, data.parent, ok);

        auto p = valueGroup_.find(groupVar);

        if (p == valueGroup_.end()) {
          int group = valueGroup_.size();

          p = valueGroup_.insert(p, ValueGroup::value_type(groupVar, group));

          groupValue_[group] = groupVar;
        }

        int group = (*p).second;

        for (int c = 0; c < nc; ++c) {
          bool ok;

          QVariant var =
            CQChartsModelUtil::modelValue(charts_, model, data.row, c, data.parent, ok);

          if (ok)
            rowColValueSet_[group][c].addValue(var);
        }
      }
      else {
        for (int c = 0; c < nc; ++c) {
          bool ok;

          QVariant var =
            CQChartsModelUtil::modelValue(charts_, model, data.row, c, data.parent, ok);

          if (ok)
            rowColValueSet_[0][c].addValue(var);
        }
      }

      return State::OK;
    }

    int numHierColumns() const { return (isHierarchical() ? 1 : 0); }

    int numFlatColumns() const { return numCols() - numHierColumns(); }

    int numHierRows() const { return rowColValueSet_.size(); }

    QVariant groupValue(int row) {
      assert(row >= 0 && row <= int(groupValue_.size()));

      return groupValue_[row];
    }

    double hierSum(int r, int c) const {
      return valueSet(r, c).rsum();
    }

    double hierMean(int r, int c) const {
      return valueSet(r, c).rmean();
    }

   private:
    CQChartsValueSet &valueSet(int r, int c) const {
      assert(r >= 0 && r <= int(rowColValueSet_.size()));
      assert(c >= 0 && c <= numFlatColumns());

      FlattenVisitor *th = const_cast<FlattenVisitor *>(this);

      return th->rowColValueSet_[r][c];
    }

   private:
    using ValueGroup     = std::map<QVariant,int>;
    using GroupValue     = std::map<int,QVariant>;
    using ColValueSet    = std::map<int,CQChartsValueSet>;
    using RowColValueSet = std::map<QVariant,ColValueSet>;

    CQCharts*      charts_ { nullptr };
    CQChartsColumn groupColumn_;        // grouping column
    int            hierRow_ { -1 };
    ValueGroup     valueGroup_;         // map group column value to group number
    GroupValue     groupValue_;         // map group number to group column value
    RowColValueSet rowColValueSet_;     // values per hier row or group
  };

  FlattenVisitor flattenVisitor(charts_, groupColumn);

  CQChartsModelVisit::exec(charts_, model.data(), flattenVisitor);

  int nh = flattenVisitor.numHierColumns();
  int nc = flattenVisitor.numFlatColumns();
  int nr = flattenVisitor.numHierRows();

  //---

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQDataModel *dataModel = new CQDataModel(nc + nh, nr);

  CQChartsFilterModel *filterModel = new CQChartsFilterModel(charts_, dataModel);

  // set hierarchical column
  if (flattenVisitor.isHierarchical()) {
    bool ok;

    QString name = CQChartsModelUtil::modelHeaderString(model.data(), 0, Qt::Horizontal, ok);

    CQChartsModelUtil::setModelHeaderValue(dataModel, 0, Qt::Horizontal, name);
  }

  // set other columns and types
  for (int c = 0; c < nc; ++c) {
    bool isGroup = (groupColumn.column() == c);

    bool ok;

    QString name = CQChartsModelUtil::modelHeaderString(model.data(), c + nh, Qt::Horizontal, ok);

    CQChartsModelUtil::setModelHeaderValue(dataModel, c + nh, Qt::Horizontal, name);

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    if (! CQChartsModelUtil::columnValueType(charts_, model.data(), c + nh, columnType,
                                             columnBaseType, nameValues))
      continue;

    const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

    if (! typeData)
      continue;

    if (isGroup || typeData->isNumeric()) {
      if (! columnTypeMgr->setModelColumnType(dataModel, c + nh, columnType, nameValues))
        continue;
    }
  }

  //--

  for (int r = 0; r < nr; ++r) {
    if (flattenVisitor.isHierarchical()) {
      QVariant var = flattenVisitor.groupValue(r);

      CQChartsModelUtil::setModelValue(dataModel, r, 0, var);
    }

    for (int c = 0; c < nc; ++c) {
      bool isGroup = (groupColumn.column() == c);

      if (! isGroup) {
        double v = 0.0;

        if      (sumFlag)
          v = flattenVisitor.hierSum(r, c);
        else if (meanFlag)
          v = flattenVisitor.hierMean(r, c);

        CQChartsModelUtil::setModelValue(dataModel, r, c + nh, v);
      }
      else {
        QVariant v = flattenVisitor.groupValue(r);

        CQChartsModelUtil::setModelValue(dataModel, r, c + nh, v);
      }
    }
  }

  ModelP dataModelP(filterModel);

  CQChartsModelData *dataModelData = charts_->initModelData(dataModelP);

  //---

  cmdBase_->setCmdRc(dataModelData->ind());

  return true;
}

//------

bool
CQChartsCmds::
copyChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::copyChartsModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int modelInd = argv.getParseInt("model" , -1);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  QAbstractItemModel *newModel = modelData->copy();

  ModelP newModelP(newModel);

  CQChartsModelData *newModelData = charts_->initModelData(newModelP);

  //---

  cmdBase_->setCmdRc(newModelData->ind());

  return true;
}

//------

// TODO: combine with export. Support vertical header
bool
CQChartsCmds::
writeChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::writeChartsModelCmd");

  argv.addCmdArg("-model"    , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-header"   , CQChartsCmdArg::Type::SBool  , "show header");
  argv.addCmdArg("-max_rows" , CQChartsCmdArg::Type::Integer, "maximum number of rows to write");
  argv.addCmdArg("-max_width", CQChartsCmdArg::Type::Integer, "maximum column width");
  argv.addCmdArg("-hier"     , CQChartsCmdArg::Type::SBool  , "output hierarchically");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int modelInd = argv.getParseInt("model", -1);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  //------

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

  bool header = true;

  if (argv.hasParseArg("header")) {
    header = argv.getParseBool("header");
  }

  //------

  class OutputRows {
   public:
    OutputRows(int nc, int maxWidth=-1) :
     nc_(nc), maxWidth_(maxWidth) {
    }

    void setHeader(const QStringList &strs) {
      assert(strs.size() == nc_);

      for (int i = 0; i < nc_; ++i) {
        const QString &str = strs[i];

        columnWidths_[i] = std::max(columnWidths_[i], str.length());

        if (maxWidth_ > 0)
          columnWidths_[i] = std::min(columnWidths_[i], maxWidth_);
      }

      header_ = strs;
    }

    void addRow(int depth, const QStringList &strs) {
      assert(strs.size() == nc_);

      for (int i = 0; i < nc_; ++i) {
        const QString &str = strs[i];

        columnWidths_[i] = std::max(columnWidths_[i], str.length());

        if (maxWidth_ > 0)
          columnWidths_[i] = std::min(columnWidths_[i], maxWidth_);
      }

      rows_.push_back(Row(depth, strs));

      maxDepth_ = std::max(maxDepth_, depth);
    }

    void write(bool header) {
      if (header) {
        QString str;

        for (int i = 0; i < nc_; ++i) {
          int w = columnWidths_[i];

          QString str1 = header_[i];

          if (str1.length() > w)
            str1 = str1.mid(0, w);

          if (str.length())
            str += "|";

          str += QString("%1").arg(str1, w);
        }

        for (int i = 0; i < maxDepth_; ++i)
          std::cout << " ";

        std::cout << " ";

        std::cout << str.toStdString() << "\n";
      }

      for (const auto &row : rows_) {
        int d1 = row.depth;
        int d2 = maxDepth_ - d1;

        for (int i = 0; i < d1; ++i)
          std::cout << ".";

        for (int i = 0; i < d2; ++i)
          std::cout << " ";

        std::cout << " ";

        //---

        QString str;

        for (int i = 0; i < nc_; ++i) {
          int w = columnWidths_[i];

          QString str1 = row.strs[i];

          if (str1.length() > w)
            str1 = str1.mid(0, w);

          if (str.length())
            str += "|";

          str += QString("%1").arg(str1, w);
        }

        std::cout << str.toStdString() << "\n";
      }
    }

   private:
    using ColumnWidths = std::map<int,int>;

    struct Row {
      int         depth { 0 };
      QStringList strs;

      Row(int depth, const QStringList &strs) :
       depth(depth), strs(strs) {
      }
    };

    using Rows = std::vector<Row>;

    int          nc_       { 0 };
    int          maxWidth_ { -1 };
    ColumnWidths columnWidths_;
    Rows         rows_;
    QStringList  header_;
    int          maxDepth_ { 0 };
  };

  // list values
  int role = Qt::DisplayRole;

  int nc = model.data()->columnCount();

  OutputRows output(nc, maxWidth);

  //---

  auto outputColumns = [&]() {
    QStringList strs;

    for (int c = 0; c < nc; ++c) {
      bool ok;

      QVariant var = CQChartsModelUtil::modelHeaderValue(model.data(), c, Qt::Horizontal, role, ok);

      QString str = var.toString();

      strs += str;
    }

    output.setHeader(strs);
  };

  std::function<void(const QModelIndex &,int)> outputHier;

  outputHier = [&](const QModelIndex &parent, int depth) -> void {
    int nr = model.data()->rowCount(parent);

    if (maxRows > 0)
      nr = std::min(nr, maxRows);

    for (int r = 0; r < nr; ++r) {
      QStringList strs;

      for (int c = 0; c < nc; ++c) {
        bool ok;

        QVariant var = CQChartsModelUtil::modelValue(charts_, model.data(), r, c, parent, role, ok);

        QString str = var.toString();

        strs += str;
      }

      output.addRow(depth, strs);

      //---

      if (hier) {
        QModelIndex parent1 = model.data()->index(r, 0, parent);

        if (model.data()->rowCount(parent1) > 0) {
          outputHier(parent1, depth + 1);
        }
      }
    }
  };

  //---

  fflush(stdout); std::cout << std::flush;

  outputColumns();

  outputHier(QModelIndex(), 0);

  output.write(header);

  fflush(stdout); std::cout << std::flush;

  return true;
}

//------

bool
CQChartsCmds::
sortChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::sortChartsModelCmd");

  argv.addCmdArg("-model"     , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column"    , CQChartsCmdArg::Type::Column , "column to sort");
  argv.addCmdArg("-decreasing", CQChartsCmdArg::Type::Boolean, "invert sort");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int  modelInd   = argv.getParseInt   ("model" , -1);
  bool decreasing = argv.getParseBool  ("decreasing");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  CQChartsColumn column = argv.getParseColumn("column", model.data());

  //---

  Qt::SortOrder order = (decreasing ? Qt::DescendingOrder : Qt::AscendingOrder);

  sortModel(model, column.column(), order);

  return true;
}

//------

bool
CQChartsCmds::
filterChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::filterChartsModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-expr"  , CQChartsCmdArg::Type::String , "filter expression");
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column");
  argv.addCmdArg("-type"  , CQChartsCmdArg::Type::String , "filter type");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int     modelInd = argv.getParseInt("model", -1);
  QString expr     = argv.getParseStr("expr");
  QString type     = argv.getParseStr("type");

  if (! argv.hasParseArg("type"))
    type = "expression";

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  CQChartsModelFilter *modelFilter = qobject_cast<CQChartsModelFilter *>(model.data());

  if (! modelFilter)
    return errorMsg("No filter support for model");

  //------

  // get column
  int icolumn = -1;

  if (argv.hasParseArg("column")) {
    CQChartsColumn column = argv.getParseColumn("column", model.data());

    icolumn = column.column();
  }

  modelFilter->setFilterKeyColumn(icolumn);

  //------

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
  else if (type == "?") {
    QStringList names = QStringList() <<
      "expression" << "regexp" << "wildcard" << "simple" << "selected" << "non-selected";

    cmdBase_->setCmdRc(names);
  }
  else
    return errorMsg(QString("Invalid type '%1'").arg(type));

  return true;
}

//------

bool
CQChartsCmds::
createChartsCorrelationModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsCorrelationModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-flip" , CQChartsCmdArg::Type::Boolean, "correlate rows instead of columns");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int  modelInd = argv.getParseInt ("model", -1);
  bool flip     = argv.getParseBool("flip");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  //------

  CQChartsLoader loader(charts_);

  QAbstractItemModel *correlationModel =
    loader.createCorrelationModel(modelData->currentModel().data(), flip);

  ModelP correlationModelP(correlationModel);

  CQChartsModelData *modelData1 = charts_->initModelData(correlationModelP);

  //---

  cmdBase_->setCmdRc(modelData1->ind());

  return true;
}

//------

bool
CQChartsCmds::
createChartsFoldedModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsFoldedModelCmd");

  argv.addCmdArg("-model"       , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column"      , CQChartsCmdArg::Type::Column , "column to fold");
  argv.addCmdArg("-fold_keep"   , CQChartsCmdArg::Type::Boolean, "keep folded column");
  argv.addCmdArg("-fold_data"   , CQChartsCmdArg::Type::Boolean, "show folded column child data");
  argv.addCmdArg("-bucket_count", CQChartsCmdArg::Type::Integer, "bucket count");
  argv.addCmdArg("-bucket_delta", CQChartsCmdArg::Type::Real   , "bucket delta");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  //---

  // get column
  CQChartsColumn column = argv.getParseColumn("column", model.data());

  //------

  CQFoldData foldData;

  if (argv.hasParseArg("fold_keep"))
    foldData.setKeepColumn(argv.getParseBool("fold_keep"));

  if (argv.hasParseArg("fold_data"))
    foldData.setShowColumnData(argv.getParseBool("fold_data"));

  if (column.isValid()) {
    foldData.setColumn(column.column());

    //---

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    if (CQChartsModelUtil::columnValueType(charts_, model.data(), column, columnType,
                                           columnBaseType, nameValues)) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

      const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

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

  CQFoldedModel *foldedModel = new CQFoldedModel(model.data(), foldData);

  //---

  QSortFilterProxyModel *foldProxyModel = new QSortFilterProxyModel;

  foldProxyModel->setObjectName("foldProxyModel");

  foldProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  foldProxyModel->setSourceModel(foldedModel);

  ModelP foldedModelP(foldProxyModel);

  CQChartsModelData *foldedModelData = charts_->initModelData(foldedModelP);

  //---

  cmdBase_->setCmdRc(foldedModelData->ind());

  return true;
}

//------

bool
CQChartsCmds::
createChartsBucketModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsBucketModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column to bucket");
  argv.addCmdArg("-multi" , CQChartsCmdArg::Type::Boolean, "multiple bucket columns");
  argv.addCmdArg("-start" , CQChartsCmdArg::Type::Real   , "bucket start");
  argv.addCmdArg("-delta" , CQChartsCmdArg::Type::Real   , "bucket delta");
  argv.addCmdArg("-min"   , CQChartsCmdArg::Type::Real   , "bucket min");
  argv.addCmdArg("-max"   , CQChartsCmdArg::Type::Real   , "bucket max");
  argv.addCmdArg("-count" , CQChartsCmdArg::Type::Integer, "number of buckets");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  //---

  // get column
  CQChartsColumn column = argv.getParseColumn("column", model.data());

  //------

  CQBucketModel *bucketModel = new CQBucketModel(model.data());

  if (argv.getParseBool("multi"))
    bucketModel->setMultiColumn(true);

  if (column.isValid()) {
    bucketModel->setBucketColumn(column.column());

    //---

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    if (CQChartsModelUtil::columnValueType(charts_, model.data(), column, columnType,
                                           columnBaseType, nameValues)) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

      const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

      if (typeData) {
        if (typeData->isNumeric()) {
          if (argv.hasParseArg("start") || argv.hasParseArg("delta")) {
            bucketModel->setBucketType(CQBucketer::Type::REAL_RANGE);

            if (argv.hasParseArg("start"))
              bucketModel->setBucketStart(argv.getParseReal("start"));

            if (argv.hasParseArg("delta"))
              bucketModel->setBucketDelta(argv.getParseReal("delta"));
          }
          else {
            bucketModel->setBucketType(CQBucketer::Type::REAL_AUTO);

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

  ModelP bucketModelP(bucketModel);

  CQChartsModelData *modelData1 = charts_->initModelData(bucketModelP);

  //---

  cmdBase_->setCmdRc(modelData1->ind());

  return true;
}

//------

bool
CQChartsCmds::
createChartsSubsetModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsSubsetModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-left"  , CQChartsCmdArg::Type::Column , "left (start) column");
  argv.addCmdArg("-right" , CQChartsCmdArg::Type::Column , "right (end) column");
  argv.addCmdArg("-top"   , CQChartsCmdArg::Type::Integer, "top (start) row");
  argv.addCmdArg("-bottom", CQChartsCmdArg::Type::Integer, "bottom (end) row");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  QAbstractItemModel *model = modelData->currentModel().data();

  //------

  CQChartsColumn left  = argv.getParseColumn("left" , model);
  CQChartsColumn right = argv.getParseColumn("right", model);

  int top    = argv.getParseInt("top"   , -1);
  int bottom = argv.getParseInt("bottom", -1);

  if (! left .isValid()) left  = 0;
  if (! right.isValid()) right = model->columnCount() - 1;

  if (top    < 0) top    = 0;
  if (bottom < 0) bottom = model->rowCount() - 1;

  //------

  CQSubSetModel *subsetModel = new CQSubSetModel(model);

  QModelIndex tlIndex = model->index(top   , left .column());
  QModelIndex brIndex = model->index(bottom, right.column());

  subsetModel->setBounds(tlIndex, brIndex);

  ModelP subsetModelP(subsetModel);

  CQChartsModelData *modelData1 = charts_->initModelData(subsetModelP);

  //---

  cmdBase_->setCmdRc(modelData1->ind());

  return true;
}

//------

bool
CQChartsCmds::
createChartsTransposeModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsTransposeModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  QAbstractItemModel *model = modelData->currentModel().data();

  //------

  CQTransposeModel *transposeModel = new CQTransposeModel(model);

  ModelP transposeModelP(transposeModel);

  CQChartsModelData *modelData1 = charts_->initModelData(transposeModelP);

  //---

  cmdBase_->setCmdRc(modelData1->ind());

  return true;
}

//------

bool
CQChartsCmds::
createChartsSummaryModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsSummaryModelCmd");

  argv.addCmdArg("-model"      , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-max_rows"   , CQChartsCmdArg::Type::Integer, "maxumum rows");
  argv.addCmdArg("-random"     , CQChartsCmdArg::Type::Boolean, "random rows");
  argv.addCmdArg("-sorted"     , CQChartsCmdArg::Type::Boolean, "sorted rows");
  argv.addCmdArg("-sort_column", CQChartsCmdArg::Type::Integer, "sort column");
  argv.addCmdArg("-sort_role"  , CQChartsCmdArg::Type::String , "sort role");
  argv.addCmdArg("-sort_order" , CQChartsCmdArg::Type::Enum   , "sort order").
   addNameValue("ascending" , Qt::AscendingOrder ).
   addNameValue("descending", Qt::DescendingOrder);
  argv.addCmdArg("-paged"      , CQChartsCmdArg::Type::Boolean, "paged");
  argv.addCmdArg("-page_size"  , CQChartsCmdArg::Type::Integer, "page size");
  argv.addCmdArg("-page_number", CQChartsCmdArg::Type::Integer, "page number");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  QAbstractItemModel *model = modelData->currentModel().data();

  //------

  CQSummaryModel *summaryModel = new CQSummaryModel(model);

  //---

  if (argv.hasParseArg("max_rows"))
    summaryModel->setMaxRows(argv.getParseInt("max_rows", summaryModel->maxRows()));

  if (argv.hasParseArg("random"))
    summaryModel->setRandom(argv.getParseBool("random", summaryModel->isRandom()));

  if (argv.hasParseArg("sorted"))
    summaryModel->setSorted(argv.getParseBool("sorted", summaryModel->isSorted()));

  if (argv.hasParseArg("sort_column"))
    summaryModel->setSortColumn(argv.getParseInt("sort_column", summaryModel->sortColumn()));

  if (argv.hasParseArg("sort_role")) {
    QString roleName = argv.getParseStr("sort_role");

    if (roleName == "?") {
      cmdBase_->setCmdRc(CQChartsModelUtil::roleNames());
      return true;
    }

    int sortRole = CQChartsModelUtil::nameToRole(roleName);

    if (sortRole < 0)
      return errorMsg("Invalid sort role");

    summaryModel->setSortRole(sortRole);
  }

  if (argv.hasParseArg("sort_order")) {
    Qt::SortOrder order = (Qt::SortOrder) argv.getParseInt("sort_order");

    summaryModel->setSortOrder(order);
  }

  if (argv.hasParseArg("paged"))
    summaryModel->setPaged(argv.getParseBool("paged", summaryModel->isPaged()));

  if (argv.hasParseArg("page_size"))
    summaryModel->setPageSize(argv.getParseInt("page_size", summaryModel->pageSize()));

  if (argv.hasParseArg("page_number"))
    summaryModel->setCurrentPage(argv.getParseInt("page_number", summaryModel->currentPage()));

  //---

  ModelP summaryModelP(summaryModel);

  CQChartsModelData *modelData1 = charts_->initModelData(summaryModelP);

  //---

  cmdBase_->setCmdRc(modelData1->ind());

  return true;
}

//------

bool
CQChartsCmds::
createChartsCollapseModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsCollapseModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-sum"  , CQChartsCmdArg::Type::String , "columns to calculate sum");
  argv.addCmdArg("-mean" , CQChartsCmdArg::Type::String , "columns to calculate mean");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  //---

  auto argStringToColumns = [&](const QString &name) {
    std::vector<CQChartsColumn> columns;

    QStringList columnsStrs = argv.getParseStrs(name);

    for (int i = 0; i < columnsStrs.length(); ++i) {
      const QString &columnsStr = columnsStrs[i];

      if (! columnsStr.length())
        continue;

      std::vector<CQChartsColumn> columns1;

      if (! CQChartsModelUtil::stringToColumns(model.data(), columnsStr, columns1)) {
        (void) errorMsg("Bad columns name '" + columnsStr + "'");
        continue;
      }

      for (const auto &column : columns1)
        columns.push_back(column);
    }

    return columns;
  };

  // sum columns
  std::vector<CQChartsColumn> sumColumns  = argStringToColumns("sum" );
  std::vector<CQChartsColumn> meanColumns = argStringToColumns("mean");

  //---

  CQCollapseModel *collapseModel = new CQCollapseModel(model.data());

  //------

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  for (int c = 0; c < model->columnCount(); ++c) {
    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    if (! CQChartsModelUtil::columnValueType(charts_, model.data(), c, columnType,
                                             columnBaseType, nameValues))
      continue;

    const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);
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

  QSortFilterProxyModel *collapseProxyModel = new QSortFilterProxyModel;

  collapseProxyModel->setObjectName("collapseProxyModel");

  collapseProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  collapseProxyModel->setSourceModel(collapseModel);

  ModelP collapseModelP(collapseProxyModel);

  CQChartsModelData *collapseModelData = charts_->initModelData(collapseModelP);

  //---

  cmdBase_->setCmdRc(collapseModelData->ind());

  return true;
}

//------

bool
CQChartsCmds::
createChartsPivotModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsPivotModelCmd");

  argv.addCmdArg("-model"   , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-hcolumns", CQChartsCmdArg::Type::String , "horizontal columns");
  argv.addCmdArg("-vcolumns", CQChartsCmdArg::Type::String , "vertical columns");
  argv.addCmdArg("-dcolumn" , CQChartsCmdArg::Type::String , "data column");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  //---

  auto argStringToColumns = [&](const QString &name) {
    CQPivotModel::Columns columns;

    QStringList columnsStrs = argv.getParseStrs(name);

    for (int i = 0; i < columnsStrs.length(); ++i) {
      const QString &columnsStr = columnsStrs[i];

      if (! columnsStr.length())
        continue;

      std::vector<CQChartsColumn> columns1;

      if (! CQChartsModelUtil::stringToColumns(model.data(), columnsStr, columns1)) {
        (void) errorMsg("Bad columns name '" + columnsStr + "'");
        continue;
      }

      for (const auto &column : columns1)
        columns.push_back(column.column());
    }

    return columns;
  };

  CQPivotModel::Columns hColumns = argStringToColumns("hcolumns" );
  CQPivotModel::Columns vColumns = argStringToColumns("vcolumns");

  CQChartsColumn dcolumn;

  QString dcolumnStr = argv.getParseStr("dcolumn");

  if (! CQChartsModelUtil::stringToColumn(model.data(), dcolumnStr, dcolumn))
    dcolumn = CQChartsColumn();

  //------

  CQPivotModel *pivotModel = new CQPivotModel(model.data());

  pivotModel->setHColumns(hColumns);
  pivotModel->setVColumns(vColumns);

  pivotModel->setValueColumn(dcolumn.column());

  //---

  QSortFilterProxyModel *pivotProxyModel = new QSortFilterProxyModel;

  pivotProxyModel->setObjectName("pivotProxyModel");

  pivotProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  pivotProxyModel->setSourceModel(pivotModel);

  ModelP pivotModelP(pivotProxyModel);

  CQChartsModelData *pivotModelData = charts_->initModelData(pivotModelP);

  //---

  cmdBase_->setCmdRc(pivotModelData->ind());

  return true;
}


//------

bool
CQChartsCmds::
createChartsStatsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsStatsModelCmd");

  argv.addCmdArg("-model"  , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-columns", CQChartsCmdArg::Type::String , "columns");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  ModelP model = modelData->currentModel();

  //---

  int nc = model->columnCount();

  //---

  auto argStringToColumns = [&](const QString &name) {
    std::vector<CQChartsColumn> columns;

    QStringList columnsStrs = argv.getParseStrs(name);

    for (int i = 0; i < columnsStrs.length(); ++i) {
      const QString &columnsStr = columnsStrs[i];

      if (! columnsStr.length())
        continue;

      std::vector<CQChartsColumn> columns1;

      if (! CQChartsModelUtil::stringToColumns(model.data(), columnsStr, columns1)) {
        (void) errorMsg("Bad columns name '" + columnsStr + "'");
        continue;
      }

      for (const auto &column : columns1)
        columns.push_back(column);
    }

    return columns;
  };

  std::vector<CQChartsColumn> columns = argStringToColumns("columns");

  using ColumnSet = std::set<int>;

  ColumnSet columnSet;

  for (const auto &column : columns)
    columnSet.insert(column.column());

  if (columnSet.empty()) {
    for (int c = 0; c < nc; ++c)
      columnSet.insert(c);
  }

  //---

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQChartsModelDetails *details = modelData->details();

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

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    if (! CQChartsModelUtil::columnValueType(charts_, model.data(), c, columnType,
                                             columnBaseType, nameValues))
      continue;

    const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);
    if (! typeData) continue;

    if (! typeData->isNumeric() || typeData->isTime())
      continue;

    const CQChartsModelColumnDetails *columnDetails = details->columnDetails(c);

    //QModelIndex parent;

    bool ok;

    QVariant var = CQChartsModelUtil::modelHeaderValue(model.data(), c, Qt::DisplayRole, ok);

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

    columnDatas.push_back(data);
  }

  //---

  QStringList columnNames = QStringList() <<
   "name" << "mean" << "min" << "lower_median" << "median" << "upper_median" << "max" << "outliers";

  int nc1 = columnNames.size();
  int nr1 = columnDatas.size();

  CQDataModel *statsModel = new CQDataModel(nc1, nr1);

  for (int c = 0; c < nc1; ++c) {
    CQChartsModelUtil::setModelHeaderValue(statsModel, c, Qt::Horizontal, columnNames[c]);
  }

  for (int r = 0; r < nr1; ++r) {
    const ColumnData &data = columnDatas[r];

    CQChartsModelUtil::setModelValue(statsModel, r, 0, data.name    , Qt::DisplayRole);
    CQChartsModelUtil::setModelValue(statsModel, r, 1, data.mean    , Qt::DisplayRole);
    CQChartsModelUtil::setModelValue(statsModel, r, 2, data.min     , Qt::DisplayRole);
    CQChartsModelUtil::setModelValue(statsModel, r, 3, data.lmedian , Qt::DisplayRole);
    CQChartsModelUtil::setModelValue(statsModel, r, 4, data.median  , Qt::DisplayRole);
    CQChartsModelUtil::setModelValue(statsModel, r, 5, data.umedian , Qt::DisplayRole);
    CQChartsModelUtil::setModelValue(statsModel, r, 6, data.max     , Qt::DisplayRole);
    CQChartsModelUtil::setModelValue(statsModel, r, 7, data.outliers, Qt::DisplayRole);
  }

  //------

  QSortFilterProxyModel *statsProxyModel = new QSortFilterProxyModel;

  statsProxyModel->setObjectName("statsProxyModel");

  statsProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  statsProxyModel->setSourceModel(statsModel);

  ModelP statsModelP(statsProxyModel);

  CQChartsModelData *statsModelData = charts_->initModelData(statsModelP);

  //---

  cmdBase_->setCmdRc(statsModelData->ind());

  return true;
}

//------

bool
CQChartsCmds::
exportChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::exportChartsModelCmd");

  argv.addCmdArg("-model"  , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-to"     , CQChartsCmdArg::Type::String , "destination format");
  argv.addCmdArg("-file"   , CQChartsCmdArg::Type::String , "file name");
  argv.addCmdArg("-hheader", CQChartsCmdArg::Type::SBool  , "output horizontal header");
  argv.addCmdArg("-vheader", CQChartsCmdArg::Type::SBool  , "output vertical header");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int     modelInd = argv.getParseInt ("model", -1);
  QString toName   = argv.getParseStr ("to", "csv");
  QString filename = argv.getParseStr ("file", "");
  bool    hheader  = argv.getParseBool("hheader", true);
  bool    vheader  = argv.getParseBool("vheader", false);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  std::ofstream fos; bool isFile = false;

  if (filename.length()) {
    fos.open(filename.toLatin1().constData());

    if (fos.fail())
      return errorMsg("Failed to open '" + filename + "'");

    isFile = true;
  }

  ModelP model = modelData->currentModel();

  if      (toName.toLower() == "csv") {
    CQChartsModelUtil::exportModel(model.data(), CQBaseModelDataType::CSV,
                                   hheader, vheader, (isFile ? fos : std::cout));
  }
  else if (toName.toLower() == "tsv") {
    CQChartsModelUtil::exportModel(model.data(), CQBaseModelDataType::TSV,
                                   hheader, vheader, (isFile ? fos : std::cout));
  }
  else if (toName.toLower() == "?") {
    QStringList names = QStringList() << "csv" << "tsv";

    cmdBase_->setCmdRc(names);
  }
  else
    return errorMsg("Invalid output format");

  return true;
}

//------

bool
CQChartsCmds::
removeChartsModelCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::removeChartsModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  int modelInd = argv.getParseInt("model" , -1);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  if (! charts_->removeModelData(modelData))
    return errorMsg("Failed to remove model");

  //---

  return true;
}

//------

// get charts data
bool
CQChartsCmds::
getChartsDataCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  using QVariantList = QList<QVariant>;

  CQPerfTrace trace("CQChartsCmds::getChartsDataCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-model"     , CQChartsCmdArg::Type::Integer, "model index");
  argv.addCmdArg("-view"      , CQChartsCmdArg::Type::String , "view name");
  argv.addCmdArg("-type"      , CQChartsCmdArg::Type::String , "type name");
  argv.addCmdArg("-plot"      , CQChartsCmdArg::Type::String , "plot name");
  argv.addCmdArg("-annotation", CQChartsCmdArg::Type::String , "annotation name");
  argv.endCmdGroup();

  argv.addCmdArg("-object", CQChartsCmdArg::Type::String, "object id");

  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column");
  argv.addCmdArg("-header", CQChartsCmdArg::Type::Boolean, "get header data");
  argv.addCmdArg("-row"   , CQChartsCmdArg::Type::Row    , "row number or id");
  argv.addCmdArg("-ind"   , CQChartsCmdArg::Type::String , "model index");

  argv.addCmdArg("-role", CQChartsCmdArg::Type::String, "role id");

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String, "option name").setRequired();
  argv.addCmdArg("-data", CQChartsCmdArg::Type::String, "option data");

  argv.addCmdArg("-hidden", CQChartsCmdArg::Type::Boolean, "include hidden data").setHidden(true);

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  QString objectId = argv.getParseStr("object");

  bool    header = argv.getParseBool("header");
  QString name   = argv.getParseStr ("name");
  bool    hidden = argv.getParseBool("hidden");

  //---

  QString roleName = argv.getParseStr("role");

  int role = Qt::EditRole;

  if (roleName != "") {
    if (roleName == "?") {
      cmdBase_->setCmdRc(CQChartsModelUtil::roleNames());
      return true;
    }

    role = CQChartsModelUtil::nameToRole(roleName);

    if (role < 0)
      return errorMsg("Invalid role");
  }

  //---

  // model data
  if      (argv.hasParseArg("model")) {
    int modelInd = argv.getParseInt("model", -1);

    // get model
    CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

    if (! modelData)
      return errorMsg("No model data");

    ModelP model = modelData->currentModel();

    //---

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    CQChartsRow row = argv.getParseRow("row");

    if (argv.hasParseArg("ind")) {
      int irow, icol;

      if (! CQTclUtil::stringToModelIndex(argv.getParseStr("ind"), irow, icol))
        return errorMsg("Invalid model index");

      row    = irow;
      column = icol;
    }

    //---

    // get column header or row, column value
    if      (name == "value") {
      QVariant var;

      if (header) {
        if (! column.isValid())
          return errorMsg("Invalid header column specified");

        bool ok;

        var = CQChartsModelUtil::modelHeaderValue(model.data(), column, role, ok);

        if (! var.isValid())
          return errorMsg("Invalid header value");
      }
      else {
#if 0
        QModelIndex ind = model.data()->index(row, column.column());

        if (! ind.isValid())
          return errorMsg("Invalid data row/column specified");
#endif

        QModelIndex parent;

        bool ok;

        var = CQChartsModelUtil::modelValue(charts_, model.data(), row.row(),
                                            column, parent, role, ok);

        if (! var.isValid())
          return errorMsg("Invalid model value");
      }

      cmdBase_->setCmdRc(var);
    }
    // get meta data
    else if (name == "meta") {
      QString data = argv.getParseStr("data");

      QVariant var = CQChartsModelUtil::getModelMetaValue(model.data(), data);

      if (! var.isValid())
        return errorMsg("Invalid meta data");

      cmdBase_->setCmdRc(var);
    }
    // number of rows, number of columns, hierarchical
    else if (name == "num_rows" || name == "num_columns" || name == "hierarchical") {
      CQChartsModelDetails *details = modelData->details();

      if      (name == "num_rows")
        cmdBase_->setCmdRc(details->numRows());
      else if (name == "num_columns")
        cmdBase_->setCmdRc(details->numColumns());
      else if (name == "hierarchical")
        cmdBase_->setCmdRc(details->isHierarchical());
    }
    // header value
    else if (name == "header") {
      const CQChartsModelDetails *details = modelData->details();

      int nc = details->numColumns();

      if (! column.isValid()) {
        QVariantList vars;

        for (int c = 0; c < nc; ++c) {
          bool ok;

          QVariant var = CQChartsModelUtil::modelHeaderValue(model.data(), c, role, ok);

          vars.push_back(var);
        }

        cmdBase_->setCmdRc(vars);
      }
      else {
        if (column < 0 || column.column() >= nc)
          return errorMsg("Invalid column number");

        bool ok;

        QVariant var = CQChartsModelUtil::modelHeaderValue(model.data(), column, role, ok);

        cmdBase_->setCmdRc(var);
      }
    }
    // row value
    else if (name == "row") {
      const CQChartsModelDetails *details = modelData->details();

      int nr = details->numRows();
      int nc = details->numColumns();

      if (row.row() < 0 || row.row() >= nr)
        return errorMsg("Invalid row number");

      QModelIndex parent; // TODO;

      QVariantList vars;

      for (int c = 0; c < nc; ++c) {
        bool ok;

        QVariant var =
          CQChartsModelUtil::modelValue(charts_, model.data(), row.row(), c, parent, role, ok);

        vars.push_back(var);
      }

      cmdBase_->setCmdRc(vars);
    }
    // column value
    else if (name == "column") {
      const CQChartsModelDetails *details = modelData->details();

      int nr = details->numRows();
      int nc = details->numColumns();

      if (column < 0 || column.column() >= nc)
        return errorMsg("Invalid column number");

      QModelIndex parent; // TODO;

      QVariantList vars;

      for (int r = 0; r < nr; ++r) {
        bool ok;

        QVariant var =
          CQChartsModelUtil::modelValue(charts_, model.data(), r, column.column(), parent,
                                        role, ok);

        vars.push_back(var);
      }

      cmdBase_->setCmdRc(vars);
    }
    // column named value
    else if (CQChartsModelColumnDetails::isNamedValue(name)) {
      const CQChartsModelDetails *details = modelData->details();

      if (argv.hasParseArg("column")) {
        if (! column.isValid() || column.column() >= details->numColumns())
          return errorMsg("Invalid column specified");

        const CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

        cmdBase_->setCmdRc(columnDetails->getNamedValue(name));
      }
      else {
        int nc = details->numColumns();

        QVariantList vars;

        for (int c = 0; c < nc; ++c) {
          const CQChartsModelColumnDetails *columnDetails = details->columnDetails(c);

          vars.push_back(columnDetails->getNamedValue(name));
        }

        cmdBase_->setCmdRc(vars);
      }
    }
    // map value
    else if (name == "map") {
      CQChartsModelDetails *details = modelData->details();

      if (! column.isValid() || column.column() >= details->numColumns())
        return errorMsg("Invalid column specified");

#if 0
      QModelIndex ind = model.data()->index(row.row(), column.column());
#endif

      QModelIndex parent;

      bool ok;

      QVariant var =
        CQChartsModelUtil::modelValue(charts_, model.data(), row.row(), column.column(), parent,
                                      role, ok);

      CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

      double r = columnDetails->map(var);

      cmdBase_->setCmdRc(r);
    }
    // duplicate rows
    else if (name == "duplicates") {
      CQChartsModelDetails *details = modelData->details();

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

      cmdBase_->setCmdRc(vars);
    }
    // get index for column name
    else if (name == "column_index") {
      QString data = argv.getParseStr("data");

      CQChartsColumn column;

      if (! CQChartsModelUtil::stringToColumn(model.data(), data, column))
        column = CQChartsColumn();

      cmdBase_->setCmdRc(column.column());
    }
    // get title
    else if (name == "title") {
      CQDataModel *dataModel = CQChartsModelUtil::getDataModel(model.data());

      QString title;

      if (dataModel)
        title = dataModel->title();

      cmdBase_->setCmdRc(title);
    }
    // model property
    else if (name.left(9) == "property.") {
      QString name1 = name.mid(9);

      if (name1 == "?") {
        QStringList names;

        CQChartsModelUtil::getPropertyNames(model.data(), names);

        cmdBase_->setCmdRc(names);
      }
      else {
        QVariant value;

        if (! CQChartsModelUtil::getProperty(model.data(), name1, value))
          return errorMsg("Failed to get model property '" + name1 + "'");

        cmdBase_->setCmdRc(value);
      }
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
        "value" << "meta" << "num_rows" << "num_columns" << "hierarchical" <<
        "header" << "row" << "column" << "map" << "duplicates" << "column_index" <<
        "title" << "property.<name>";

      names << CQChartsModelColumnDetails::getLongNamedValues();

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }
  // view data
  else if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);
    if (! view) return false;

    if      (name == "plots") {
      QVariantList vars;

      CQChartsView::Plots plots;

      view->getPlots(plots);

      for (const auto &plot : plots)
        vars.push_back(plot->pathId());

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "annotations") {
      QVariantList vars;

      const CQChartsView::Annotations &annotations = view->annotations();

      for (const auto &annotation : annotations)
        vars.push_back(annotation->pathId());

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "selected_objects") {
      CQChartsView::Objs objs;

      view->allSelectedObjs(objs);

      QStringList ids;

      for (const auto &obj : objs) {
        CQChartsPlotObj *plotObj = qobject_cast<CQChartsPlotObj *>(obj);

        if (plotObj)
          ids.push_back(plotObj->plot()->id() + ":" + plotObj->id());
      }

      cmdBase_->setCmdRc(ids);
    }
    else if (name == "view_width") {
      QString data = argv.getParseStr("data");

      CQChartsLength len(data, CQChartsUnits::VIEW);

      double w = view->lengthViewWidth(len);

      cmdBase_->setCmdRc(w);
    }
    else if (name == "view_height") {
      QString data = argv.getParseStr("data");

      CQChartsLength len(data, CQChartsUnits::VIEW);

      double h = view->lengthViewHeight(len);

      cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_width") {
      QString data = argv.getParseStr("data");

      CQChartsLength len(data, CQChartsUnits::VIEW);

      double w = view->lengthPixelWidth(len);

      cmdBase_->setCmdRc(w);
    }
    else if (name == "pixel_height") {
      QString data = argv.getParseStr("data");

      CQChartsLength len(data, CQChartsUnits::VIEW);

      double h = view->lengthPixelHeight(len);

      cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_position") {
      QString data = argv.getParseStr("data");

      CQChartsPosition pos(data, CQChartsUnits::VIEW);

      QPointF p = view->positionToPixel(pos);

      cmdBase_->setCmdRc(p);
    }
    else if (name == "properties") {
      QStringList names;

      view->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (name == "script_select_proc") {
      QString str = view->scriptSelectProc();

      cmdBase_->setCmdRc(str);
    }
    else if (name == "mouse_press") {
      QPoint p = view->mousePressPoint();

      cmdBase_->setCmdRc(p);
    }
    else if (name == "mouse_modifier") {
      CQChartsSelMod mod = view->mouseClickMod();

      QString res;

      if      (mod == CQChartsSelMod::REPLACE) res = "replace";
      else if (mod == CQChartsSelMod::ADD    ) res = "add";
      else if (mod == CQChartsSelMod::REMOVE ) res = "remove";
      else if (mod == CQChartsSelMod::TOGGLE ) res = "toggle";
      else                                     res = "none";

      cmdBase_->setCmdRc(res);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "plots" << "annotations" << "selected_objects" << "view_width" << "view_height" <<
       "pixel_width" << "pixel_height" << "pixel_position" << "properties";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }
  // type data
  else if (argv.hasParseArg("type")) {
    QString typeName = argv.getParseStr("type");

    if (! charts_->isPlotType(typeName))
      return errorMsg("No type '" + typeName + "'");

    CQChartsPlotType *type = charts_->plotType(typeName);

    if (! type)
      return errorMsg("No type '" + typeName + "'");

    //---

    if      (name == "properties") {
      QStringList names;

      type->propertyNames(names);

      cmdBase_->setCmdRc(names);
    }
    else if (name == "parameters") {
      const CQChartsPlotType::Parameters &parameters = type->parameters();

      QStringList names;

      for (auto &parameter : parameters)
        names.push_back(parameter->name());

      cmdBase_->setCmdRc(names);
    }
    else if (name.left(10) == "parameter.") {
      QString data = argv.getParseStr("data");

      if (! type->hasParameter(data))
        return errorMsg("No parameter '" + data + "'");

      const CQChartsPlotParameter &parameter = type->getParameter(data);

      QString name1 = name.mid(10);

      if (name1 == "properties") {
        QStringList names;

        parameter.propertyNames(names);

        cmdBase_->setCmdRc(names);
      }
      else if (parameter.hasProperty(name1)) {
        cmdBase_->setCmdRc(parameter.getPropertyValue(name1));
      }
      else
        return errorMsg("Invalid name 'parameter." + name1 + "' specified");
    }
    else if (type->hasProperty(name)) {
      cmdBase_->setCmdRc(type->getPropertyValue(name));
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "properties" << "parameters" << "parameter.<parameter_name>" << "<property_name>";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }
  // plot data
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsView *view = nullptr;

    CQChartsPlot *plot = getPlotByName(view, plotName);
    if (! plot) return false;

    CQChartsRow row = argv.getParseRow("row", plot);

    //---

    // get model ind
    if      (name == "model") {
      CQChartsModelData *modelData = charts_->getModelData(plot->model().data());

      if (! modelData)
        return errorMsg("No model data");

      cmdBase_->setCmdRc(modelData->ind());
    }
    // get view ind
    else if (name == "view") {
      CQChartsView *view = plot->view();

      cmdBase_->setCmdRc(view->id());
    }
    // get column header or row, column value
    else if (name == "value") {
      CQChartsColumn column = argv.getParseColumn("column", plot->model().data());

      //---

      QVariant var;

      if (header) {
        if (! column.isValid())
          return errorMsg("Invalid header column specified");

        bool ok;

        var = CQChartsModelUtil::modelHeaderValue(plot->model().data(), column, role, ok);

        if (! var.isValid())
          return errorMsg("Invalid header value");
      }
      else {
        QModelIndex parent;

        bool ok;

        QVariant var = plot->modelValue(row.row(), column, parent, role, ok);

        bool rc;

        cmdBase_->setCmdRc(CQChartsVariant::toString(var, rc));
      }
    }
    else if (name == "map") {
      CQChartsModelData *modelData = charts_->getModelData(plot->model().data());

      if (! modelData)
        return errorMsg("No model data");

      CQChartsColumn column = argv.getParseColumn("column", plot->model().data());

      //---

      CQChartsModelDetails *details = modelData->details();

      if (! column.isValid() || column.column() >= details->numColumns())
        return errorMsg("Invalid column specified");

      QModelIndex parent;

      bool ok;

      QVariant var = plot->modelValue(row.row(), column, parent, role, ok);

      CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

      double r = columnDetails->map(var);

      cmdBase_->setCmdRc(r);
    }
    else if (name == "annotations") {
      QVariantList vars;

      const CQChartsPlot::Annotations &annotations = plot->annotations();

      for (const auto &annotation : annotations)
        vars.push_back(annotation->pathId());

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "objects") {
      QVariantList vars;

      const CQChartsPlot::PlotObjs &objs = plot->plotObjects();

      for (const auto &obj : objs)
        vars.push_back(obj->id());

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "selected_objects") {
      CQChartsPlot::PlotObjs objs;

      plot->selectedPlotObjs(objs);

      QStringList ids;

      for (const auto &obj : objs)
        ids.push_back(obj->id());

      cmdBase_->setCmdRc(ids);
    }
    else if (name == "inds") {
      if (! objectId.length())
        return errorMsg("Missing object id");

      QList<QModelIndex> inds = plot->getObjectInds(objectId);

      QVariantList vars;

      for (int i = 0; i < inds.length(); ++i)
        vars.push_back(inds[i]);

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "plot_width") {
      QString data = argv.getParseStr("data");

      CQChartsLength len(data, CQChartsUnits::PLOT);

      double w = plot->lengthPlotWidth(len);

      cmdBase_->setCmdRc(w);
    }
    else if (name == "plot_height") {
      QString data = argv.getParseStr("data");

      CQChartsLength len(data, CQChartsUnits::PLOT);

      double h = plot->lengthPlotHeight(len);

      cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_width") {
      QString data = argv.getParseStr("data");

      CQChartsLength len(data, CQChartsUnits::PLOT);

      double w = plot->lengthPixelWidth(len);

      cmdBase_->setCmdRc(w);
    }
    else if (name == "pixel_height") {
      QString data = argv.getParseStr("data");

      CQChartsLength len(data, CQChartsUnits::PLOT);

      double h = plot->lengthPixelHeight(len);

      cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_position") {
      QString data = argv.getParseStr("data");

      CQChartsPosition pos(data, CQChartsUnits::PLOT);

      QPointF p = plot->positionToPixel(pos);

      cmdBase_->setCmdRc(p);
    }
    else if (name == "properties") {
      QStringList names;

      plot->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (name == "set_hidden") {
      int id = argv.getParseInt("data", -1);

      if (id < 0)
        return errorMsg("Invalid data '" + argv.getParseStr("data") + "' specified");

      cmdBase_->setCmdRc(plot->isSetHidden(id));
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "model" << "view" << "value" << "map" << "annotations" << "objects" <<
       "selected_objects" << "inds" << "plot_width" << "plot_height" << "pixel_width" <<
       "pixel_height" << "pixel_position" << "properties" << "set_hidden";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }
  // annotation data
  else if (argv.hasParseArg("annotation")) {
    QString annotationName = argv.getParseStr("annotation");

    CQChartsAnnotation *annotation = getAnnotationByName(annotationName);
    if (! annotation) return false;

    // get view ind
    if      (name == "view") {
      CQChartsView *view = annotation->view();

      if (view)
        cmdBase_->setCmdRc(view->id());
      else
        cmdBase_->setCmdRc(QString());
    }
    // get plot ind
    else if (name == "plot") {
      CQChartsPlot *plot = annotation->plot();

      if (plot)
        cmdBase_->setCmdRc(plot->id());
      else
        cmdBase_->setCmdRc(QString());
    }
    // get column header or row, column value
    if      (name == "properties") {
      QStringList names;

      annotation->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "view" << "plot" << "properties";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }
  // global charts data
  else {
    if      (name == "models") {
      QVariantList vars;

      CQCharts::ModelDatas modelDatas;

      charts_->getModelDatas(modelDatas);

      for (auto &modelData : modelDatas)
        vars.push_back(modelData->ind());

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "views") {
      QVariantList vars;

      CQCharts::Views views;

      charts_->getViews(views);

      for (auto &view : views)
        vars.push_back(view->id());

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "plot_types") {
      QStringList names, descs;

      charts_->getPlotTypeNames(names, descs);

      cmdBase_->setCmdRc(names);
    }
    else if (name == "plots") {
      QVariantList vars;

      CQCharts::Views views;

      charts_->getViews(views);

      for (auto &view : views) {
        CQChartsView::Plots plots;

        view->getPlots(plots);

        for (auto &plot : plots)
          vars.push_back(plot->pathId());
      }

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "current_model") {
      CQChartsModelData *modelData = charts_->currentModelData();

      if (! modelData)
        return errorMsg("No model data");

      cmdBase_->setCmdRc(modelData->ind());
    }
    else if (name == "column_types") {
      CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

      QStringList names;

      columnTypeMgr->typeNames(names);

      cmdBase_->setCmdRc(names);
    }
    else if (name == "column_type.names" || name == "column_type.descs") {
       if (! argv.hasParseArg("model"))
         return errorMsg("Missing data for '" + name + "'");

      QString dataStr = argv.getParseStr("data");

      CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

      const CQChartsColumnType *columnType = columnTypeMgr->getNamedType(dataStr);
      if (! columnType) return errorMsg("Invalid type '" + dataStr + "' for '" + name + "'");

      QStringList names;

      if (name == "column_type.names") {
        for (const auto &param : columnType->params())
          names << param->name();
      }
      else {
        for (const auto &param : columnType->params())
          names << param->tip();
      }

      cmdBase_->setCmdRc(names);
    }
    else if (name == "annotation_types") {
      QStringList names = CQChartsAnnotation::typeNames();

      cmdBase_->setCmdRc(names);
    }
    else if (name == "symbols") {
      QStringList typeNames = CQChartsSymbol::typeNames();

      cmdBase_->setCmdRc(typeNames);
    }
    else if (name == "procs") {
      QStringList procs;

      charts_->getProcs(procs);

      cmdBase_->setCmdRc(procs);
    }
    else if (name == "proc_data") {
       if (! argv.hasParseArg("model"))
         return errorMsg("Missing data for '" + name + "'");

      QString dataStr = argv.getParseStr("data");

      QString args, body;

      charts_->getProcData(dataStr, args, body);

      QStringList strs = QStringList() << args << body;

      cmdBase_->setCmdRc(strs);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "models" << "views" << "plot_types" << "plots" << "current_model" <<
       "column_types" << "column_type.names" << "column_type.descs" << "annotation_types";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }

  return true;
}

//------

// set charts data
bool
CQChartsCmds::
setChartsDataCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::setChartsDataCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model index");
  argv.addCmdArg("-view" , CQChartsCmdArg::Type::String , "view name");
  argv.addCmdArg("-plot" , CQChartsCmdArg::Type::String , "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column to set");
  argv.addCmdArg("-header", CQChartsCmdArg::Type::Boolean, "get header data");
  argv.addCmdArg("-row"   , CQChartsCmdArg::Type::Row    , "row number or id");
  argv.addCmdArg("-role"  , CQChartsCmdArg::Type::String , "role id");
  argv.addCmdArg("-name"  , CQChartsCmdArg::Type::String , "option name");
  argv.addCmdArg("-value" , CQChartsCmdArg::Type::String , "option value");
  argv.addCmdArg("-data"  , CQChartsCmdArg::Type::String , "option data");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  bool    header = argv.getParseBool("header");
  QString name   = argv.getParseStr ("name");
  QString value  = argv.getParseStr ("value");

  //---

  QString roleName = argv.getParseStr("role");

  int role = Qt::EditRole;

  if (roleName != "") {
    if (roleName == "?") {
      cmdBase_->setCmdRc(CQChartsModelUtil::roleNames());
      return true;
    }

    role = CQChartsModelUtil::nameToRole(roleName);

    if (role < 0)
      return errorMsg("Invalid role");
  }

  //---

  if      (argv.hasParseArg("model")) {
    int modelInd = argv.getParseInt("model", -1);

    // get model
    CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

    if (! modelData)
      return errorMsg("No model data");

    ModelP model = modelData->currentModel();

    //---

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    CQChartsRow row = argv.getParseRow("row");

    // set column header or row, column value
    if      (name == "value") {
      if (header) {
        if (! column.isValid())
          return errorMsg("Invalid header column specified");

        if (! CQChartsModelUtil::setModelHeaderValue(model.data(), column, value, role))
          return errorMsg("Failed to set header value");
      }
      else {
        QModelIndex ind = model.data()->index(row.row(), column.column());

        if (! ind.isValid())
          return errorMsg(QString("Invalid data row/column specified '%1,%2'").
                           arg(row.row()).arg(column.column()));

        if (! CQChartsModelUtil::setModelValue(model.data(), row.row(), column, value, role))
          return errorMsg("Failed to set row value");
      }
    }
    else if (name == "column_type") {
      if (column.isValid()) {
        if (! CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), column, value))
          return errorMsg(QString("Invalid column type '%1'").arg(value));
      }
      else {
        if (! CQChartsModelUtil::setColumnTypeStrs(charts_, model.data(), value))
          return errorMsg(QString("Invalid column type string '%1'").arg(value));
      }
    }
    // get meta data
    else if (name == "meta") {
      QString data = argv.getParseStr("data");

      CQChartsModelUtil::setModelMetaValue(model.data(), data, value);
    }
    // set model name
    else if (name == "name") {
      charts_->setModelName(modelData, value);
    }
    // set model process expression
    else if (name == "process_expression") {
      CQChartsModelUtil::processExpression(model.data(), value);
    }
    // model property
    else if (name.left(9) == "property.") {
      QString name1 = name.mid(9);

      if (name1 == "?") {
        QStringList names;

        CQChartsModelUtil::getPropertyNames(model.data(), names);

        cmdBase_->setCmdRc(names);
      }
      else {
        if (! CQChartsModelUtil::setProperty(model.data(), name1, value))
          return errorMsg("Failed to set model property '" + name1 + "'");
      }
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "value" << "meta" << "column_type" << "name" << "process_expression" << "property.<name>";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }
  else if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);
    if (! view) return false;

    if      (name == "fit") {
      view->fitSlot();
    }
    else if (name == "script_select_proc") {
      view->setScriptSelectProc(value);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "fit";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsView *view = nullptr;

    CQChartsPlot *plot = getPlotByName(view, plotName);
    if (! plot) return false;

    if      (name == "updates_enabled") {
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

      if (id < 0)
        return errorMsg("Invalid data '" + argv.getParseStr("data") + "' specified");

      bool ok;

      bool b = CQChartsCmdBaseArgs::stringToBool(value, &ok);

      plot->setSetHidden(id, b);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "updates_enabled" << "set_hidden";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid name '" + name + "' specified");
  }
  else
    return errorMsg("Invalid name '" + name + "' specified");

  return true;
}

//------

bool
CQChartsCmds::
createChartsRectangleAnnotationCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsRectangleAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-rectangle" , CQChartsCmdArg::Type::Rect, "rectangle bounding box");

  argv.addCmdArg("-start", CQChartsCmdArg::Type::Position, "start").setHidden();
  argv.addCmdArg("-end"  , CQChartsCmdArg::Type::Position, "end"  ).setHidden();

  argv.addCmdArg("-margin" , CQChartsCmdArg::Type::Real, "margin" ).setHidden();
  argv.addCmdArg("-padding", CQChartsCmdArg::Type::Real, "padding").setHidden();

  argv.addCmdArg("-filled"      , CQChartsCmdArg::Type::SBool ,
                 "background is filled"   ).setHidden();
  argv.addCmdArg("-fill_color"  , CQChartsCmdArg::Type::Color ,
                 "background fill color"  ).setHidden();
  argv.addCmdArg("-fill_alpha"  , CQChartsCmdArg::Type::Real  ,
                 "background fill alpha"  ).setHidden();
//argv.addCmdArg("-fill_pattern", CQChartsCmdArg::Type::String,
//               "background fill pattern").setHidden();

  argv.addCmdArg("-stroked"     , CQChartsCmdArg::Type::SBool   ,
                 "border is stroked"  ).setHidden();
  argv.addCmdArg("-stroke_color", CQChartsCmdArg::Type::Color   ,
                 "border stroke color").setHidden();
  argv.addCmdArg("-stroke_alpha", CQChartsCmdArg::Type::Real    ,
                 "border stroke alpha").setHidden();
  argv.addCmdArg("-stroke_width", CQChartsCmdArg::Type::Length  ,
                 "border stroke width").setHidden();
  argv.addCmdArg("-stroke_dash" , CQChartsCmdArg::Type::LineDash,
                 "border stroke dash" ).setHidden();

  argv.addCmdArg("-corner_size" , CQChartsCmdArg::Type::Length, "corner size" ).setHidden();
  argv.addCmdArg("-border_sides", CQChartsCmdArg::Type::Sides , "border sides").setHidden();

  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;
  }

  //---

  CQChartsBoxData boxData;

  CQChartsFillData   &fill   = boxData.shape().fill();
  CQChartsStrokeData &stroke = boxData.shape().stroke();

  stroke.setVisible(true);

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  boxData.setMargin (argv.getParseReal("margin" , boxData.margin()));
  boxData.setPadding(argv.getParseReal("padding", boxData.padding()));

  fill.setVisible(argv.getParseBool ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseReal ("fill_alpha"  , fill.alpha    ()));
//fill.setPattern(argv.getParseStr  ("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseReal    ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  stroke.setCornerSize(argv.getParseLength(view, plot, "corner_size", stroke.cornerSize()));

  boxData.setBorderSides(argv.getParseSides("border_sides", boxData.borderSides()));

  //---

  CQChartsRectangleAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("start") || argv.hasParseArg("end")) {
    CQChartsPosition start = argv.getParsePosition(view, plot, "start");
    CQChartsPosition end   = argv.getParsePosition(view, plot, "end"  );

    CQChartsRect rect;

    if      (view)
      annotation = view->addRectangleAnnotation(rect);
    else if (plot)
      annotation = plot->addRectangleAnnotation(rect);

    if (annotation)
      annotation->setRectangle(start, end);
  }
  else if (argv.hasParseArg("rectangle")) {
    CQChartsRect rect = argv.getParseRect(view, plot, "rectangle");

    if      (view)
      annotation = view->addRectangleAnnotation(rect);
    else if (plot)
      annotation = plot->addRectangleAnnotation(rect);
  }
  else {
    CQChartsRect rect;

    if      (view)
      annotation = view->addRectangleAnnotation(rect);
    else if (plot)
      annotation = plot->addRectangleAnnotation(rect);

    if (annotation)
      annotation->setRectangle(CQChartsPosition(QPointF(0, 0)), CQChartsPosition(QPointF(1, 1)));
  }

  if (! annotation)
    return errorMsg("Failed to create annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  QStringList properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  //---

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsEllipseAnnotationCmd(CQChartsCmdArgs &argv)
{
#if 0
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };
#endif

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsEllipseAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-center", CQChartsCmdArg::Type::Position, "center");

  argv.addCmdArg("-rx", CQChartsCmdArg::Type::Length, "x radius");
  argv.addCmdArg("-ry", CQChartsCmdArg::Type::Length, "y radius");

  argv.addCmdArg("-filled"      , CQChartsCmdArg::Type::SBool ,
                 "background is filled"   ).setHidden();
  argv.addCmdArg("-fill_color"  , CQChartsCmdArg::Type::Color ,
                 "background fill color"  ).setHidden();
  argv.addCmdArg("-fill_alpha"  , CQChartsCmdArg::Type::Real  ,
                 "background fill alpha"  ).setHidden();
//argv.addCmdArg("-fill_pattern", CQChartsCmdArg::Type::String,
//               "background fill pattern").setHidden();

  argv.addCmdArg("-stroked"     , CQChartsCmdArg::Type::SBool   ,
                 "border is stroked"  ).setHidden();
  argv.addCmdArg("-stroke_color", CQChartsCmdArg::Type::Color   ,
                 "border stroke color").setHidden();
  argv.addCmdArg("-stroke_alpha", CQChartsCmdArg::Type::Real    ,
                 "border stroke alpha").setHidden();
  argv.addCmdArg("-stroke_width", CQChartsCmdArg::Type::Length  ,
                 "border stroke width").setHidden();
  argv.addCmdArg("-stroke_dash" , CQChartsCmdArg::Type::LineDash,
                 "border stroke dash" ).setHidden();

  argv.addCmdArg("-corner_size" , CQChartsCmdArg::Type::Length, "corner size" ).setHidden();
  argv.addCmdArg("-border_sides", CQChartsCmdArg::Type::Sides , "border sides").setHidden();

  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;
  }

  //---

  CQChartsBoxData boxData;

  CQChartsFillData   &fill   = boxData.shape().fill();
  CQChartsStrokeData &stroke = boxData.shape().stroke();

  stroke.setVisible(true);

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition center = argv.getParsePosition(view, plot, "center");

  CQChartsLength rx = argv.getParseLength(view, plot, "rx");
  CQChartsLength ry = argv.getParseLength(view, plot, "ry");

  fill.setVisible(argv.getParseBool ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseReal ("fill_alpha"  , fill.alpha    ()));
//fill.setPattern(argv.getParseStr  ("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseReal    ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  stroke.setCornerSize(argv.getParseLength(view, plot, "corner_size", stroke.cornerSize()));

  boxData.setBorderSides(argv.getParseSides("border_sides", boxData.borderSides()));

  //---

  CQChartsEllipseAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addEllipseAnnotation(center, rx, ry);
  else if (plot)
    annotation = plot->addEllipseAnnotation(center, rx, ry);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  QStringList properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  //---

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsPolygonAnnotationCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsPolygonAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-points", CQChartsCmdArg::Type::Polygon, "points string").setRequired();

  argv.addCmdArg("-filled"      , CQChartsCmdArg::Type::SBool ,
                 "background is filled"   ).setHidden();
  argv.addCmdArg("-fill_color"  , CQChartsCmdArg::Type::Color ,
                 "background fill color"  ).setHidden();
  argv.addCmdArg("-fill_alpha"  , CQChartsCmdArg::Type::Real  ,
                 "background fill alpha"  ).setHidden();
//argv.addCmdArg("-fill_pattern", CQChartsCmdArg::Type::String,
//               "background fill pattern").setHidden();

  argv.addCmdArg("-stroked"     , CQChartsCmdArg::Type::SBool   ,
                 "border is stroked"  ).setHidden();
  argv.addCmdArg("-stroke_color", CQChartsCmdArg::Type::Color   ,
                 "border stroke color").setHidden();
  argv.addCmdArg("-stroke_alpha", CQChartsCmdArg::Type::Real    ,
                 "border stroke alpha").setHidden();
  argv.addCmdArg("-stroke_width", CQChartsCmdArg::Type::Length  ,
                 "border stroke width").setHidden();
  argv.addCmdArg("-stroke_dash" , CQChartsCmdArg::Type::LineDash,
                 "border stroke dash" ).setHidden();

  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;
  }

  //---

  CQChartsBoxData boxData;

  CQChartsShapeData &shapeData = boxData.shape();

  CQChartsFillData   &fill   = shapeData.fill();
  CQChartsStrokeData &stroke = shapeData.stroke();

  stroke.setVisible(true);

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  QPolygonF points = argv.getParsePoly("points");

  fill.setVisible(argv.getParseBool ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseReal ("fill_alpha"  , fill.alpha    ()));
//fill.setPattern(argv.getParseStr  ("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseReal    ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  //---

  if (! points.length())
    return errorMsg("No points");

  //---

  CQChartsPolygonAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addPolygonAnnotation(points);
  else if (plot)
    annotation = plot->addPolygonAnnotation(points);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  QStringList properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  //---

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsPolylineAnnotationCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsPolylineAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-points", CQChartsCmdArg::Type::Polygon, "points string").setRequired();

  argv.addCmdArg("-filled"      , CQChartsCmdArg::Type::SBool ,
                 "background is filled"   ).setHidden();
  argv.addCmdArg("-fill_color"  , CQChartsCmdArg::Type::Color ,
                 "background fill color"  ).setHidden();
  argv.addCmdArg("-fill_alpha"  , CQChartsCmdArg::Type::Real  ,
                 "background fill alpha"  ).setHidden();
//argv.addCmdArg("-fill_pattern", CQChartsCmdArg::Type::String,
//               "background fill pattern").setHidden();

  argv.addCmdArg("-stroked"     , CQChartsCmdArg::Type::SBool   ,
                 "border is stroked"  ).setHidden();
  argv.addCmdArg("-stroke_color", CQChartsCmdArg::Type::Color   ,
                 "border stroke color").setHidden();
  argv.addCmdArg("-stroke_alpha", CQChartsCmdArg::Type::Real    ,
                 "border stroke alpha").setHidden();
  argv.addCmdArg("-stroke_width", CQChartsCmdArg::Type::Length  ,
                 "border stroke width").setHidden();
  argv.addCmdArg("-stroke_dash" , CQChartsCmdArg::Type::LineDash,
                 "border stroke dash" ).setHidden();

  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;
  }

  //---

  CQChartsBoxData boxData;

  CQChartsShapeData &shapeData = boxData.shape();

  CQChartsFillData   &fill   = shapeData.fill();
  CQChartsStrokeData &stroke = shapeData.stroke();

  stroke.setVisible(true);

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  QPolygonF points = argv.getParsePoly("points");

  fill.setVisible(argv.getParseBool ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseReal ("fill_alpha"  , fill.alpha    ()));
//fill.setPattern(argv.getParseStr  ("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseReal    ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  //---

  if (! points.length())
    return errorMsg("No points");

  //---

  CQChartsPolylineAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addPolylineAnnotation(points);
  else if (plot)
    annotation = plot->addPolylineAnnotation(points);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  //---

  QStringList properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  //---

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsTextAnnotationCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsTextAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-position" , CQChartsCmdArg::Type::Position, "position");
  argv.addCmdArg("-rectangle", CQChartsCmdArg::Type::Rect    , "rectangle bounding box");

  argv.addCmdArg("-text", CQChartsCmdArg::Type::String, "text");

  argv.addCmdArg("-font"    , CQChartsCmdArg::Type::String , "font");
  argv.addCmdArg("-color"   , CQChartsCmdArg::Type::Color  , "color");
  argv.addCmdArg("-alpha"   , CQChartsCmdArg::Type::Real   , "alpha");
  argv.addCmdArg("-angle"   , CQChartsCmdArg::Type::Real   , "angle");
  argv.addCmdArg("-contrast", CQChartsCmdArg::Type::SBool  , "contrast");
  argv.addCmdArg("-align"   , CQChartsCmdArg::Type::Align  , "align string");
  argv.addCmdArg("-html"    , CQChartsCmdArg::Type::Boolean, "html text");

  argv.addCmdArg("-filled"      , CQChartsCmdArg::Type::SBool ,
                 "background is filled"   ).setHidden();
  argv.addCmdArg("-fill_color"  , CQChartsCmdArg::Type::Color ,
                 "background fill color"  ).setHidden();
  argv.addCmdArg("-fill_alpha"  , CQChartsCmdArg::Type::Real  ,
                 "background fill alpha"  ).setHidden();
//argv.addCmdArg("-fill_pattern", CQChartsCmdArg::Type::String,
//               "background fill pattern").setHidden();

  argv.addCmdArg("-stroked"     , CQChartsCmdArg::Type::SBool   ,
                 "border is stroked"  ).setHidden();
  argv.addCmdArg("-stroke_color", CQChartsCmdArg::Type::Color   ,
                 "border stroke color").setHidden();
  argv.addCmdArg("-stroke_alpha", CQChartsCmdArg::Type::Real    ,
                 "border stroke alpha").setHidden();
  argv.addCmdArg("-stroke_width", CQChartsCmdArg::Type::Length  ,
                 "border stroke width").setHidden();
  argv.addCmdArg("-stroke_dash" , CQChartsCmdArg::Type::LineDash,
                 "border stroke dash" ).setHidden();

  argv.addCmdArg("-corner_size" , CQChartsCmdArg::Type::Length, "corner size" ).setHidden();
  argv.addCmdArg("-border_sides", CQChartsCmdArg::Type::Sides , "border sides").setHidden();

  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;
  }

  //---

  CQChartsTextData textData;
  CQChartsBoxData  boxData;

  CQChartsFillData   &fill   = boxData.shape().fill();
  CQChartsStrokeData &stroke = boxData.shape().stroke();

  fill  .setVisible(false);
  stroke.setVisible(false);

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  QString text = argv.getParseStr("text", "Annotation");

  textData.setFont    (argv.getParseFont ("font"    , textData.font      ()));
  textData.setColor   (argv.getParseColor("color"   , textData.color     ()));
  textData.setAlpha   (argv.getParseReal ("alpha"   , textData.alpha     ()));
  textData.setAngle   (argv.getParseReal ("angle"   , textData.angle     ()));
  textData.setContrast(argv.getParseBool ("contrast", textData.isContrast()));
  textData.setAlign   (argv.getParseAlign("align"   , textData.align     ()));
  textData.setHtml    (argv.getParseBool ("html"    , textData.isHtml    ()));

  fill.setVisible(argv.getParseBool ("filled"      , fill.isVisible()));
  fill.setColor  (argv.getParseColor("fill_color"  , fill.color    ()));
  fill.setAlpha  (argv.getParseReal ("fill_alpha"  , fill.alpha    ()));
//fill.setPattern(argv.getParseStr  ("fill_pattern", fill.pattern  ()));

  stroke.setVisible(argv.getParseBool    ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor   ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseReal    ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength  (view, plot, "stroke_width", stroke.width()));
  stroke.setDash   (argv.getParseLineDash("stroke_dash" , stroke.dash     ()));

  stroke.setCornerSize(argv.getParseLength(view, plot, "corner_size", stroke.cornerSize()));

  boxData.setBorderSides(argv.getParseSides("border_sides", boxData.borderSides()));

  //---

  CQChartsTextAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("position")) {
    CQChartsPosition pos = argv.getParsePosition(view, plot, "position");

    if      (view)
      annotation = view->addTextAnnotation(pos, text);
    else if (plot)
      annotation = plot->addTextAnnotation(pos, text);
  }
  else if (argv.hasParseArg("rectangle")) {
    CQChartsRect rect = argv.getParseRect(view, plot, "rectangle");

    if      (view)
      annotation = view->addTextAnnotation(rect, text);
    else if (plot)
      annotation = plot->addTextAnnotation(rect, text);
  }
  else {
    CQChartsPosition pos(QPointF(0, 0));

    if      (view)
      annotation = view->addTextAnnotation(pos, text);
    else if (plot)
      annotation = plot->addTextAnnotation(pos, text);
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

  QStringList properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  //---

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsImageAnnotationCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsImageAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-position" , CQChartsCmdArg::Type::Position, "position");
  argv.addCmdArg("-rectangle", CQChartsCmdArg::Type::Rect    , "rectangle bounding box");

  argv.addCmdArg("-image", CQChartsCmdArg::Type::String, "image");

  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;
  }

  //---

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  QString name = argv.getParseStr("image");

  QImage image(name);

  if (image.isNull())
    return errorMsg(QString("Invalid image filename '%1'").arg(name));

  image.setText("", name);

  //---

  CQChartsImageAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("position")) {
    CQChartsPosition pos = argv.getParsePosition(view, plot, "position");

    if      (view)
      annotation = view->addImageAnnotation(pos, image);
    else if (plot)
      annotation = plot->addImageAnnotation(pos, image);
  }
  else if (argv.hasParseArg("rectangle")) {
    CQChartsRect rect = argv.getParseRect(view, plot, "rectangle");

    if      (view)
      annotation = view->addImageAnnotation(rect, image);
    else if (plot)
      annotation = plot->addImageAnnotation(rect, image);
  }
  else {
    CQChartsPosition pos(QPointF(0, 0));

    if      (view)
      annotation = view->addImageAnnotation(pos, image);
    else if (plot)
      annotation = plot->addImageAnnotation(pos, image);
  }

  if (! annotation)
    return errorMsg("Failed to create annotation");

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  //---

  QStringList properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  //---

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsArrowAnnotationCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::createChartsArrowAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-start", CQChartsCmdArg::Type::Position, "start position");
  argv.addCmdArg("-end"  , CQChartsCmdArg::Type::Position, "end position");

  argv.addCmdArg("-line_width", CQChartsCmdArg::Type::Length, "connecting line width");

  argv.addCmdArg("-fhead", CQChartsCmdArg::Type::String, "start arrow head type");
  argv.addCmdArg("-thead", CQChartsCmdArg::Type::String, "end arrow head type");

  argv.addCmdArg("-angle", CQChartsCmdArg::Type::String,
                 "arrow head angle");
  argv.addCmdArg("-back_angle", CQChartsCmdArg::Type::String,
                 "arrow head back angle").setHidden();
  argv.addCmdArg("-length", CQChartsCmdArg::Type::String,
                 "arrow head length");
  argv.addCmdArg("-line_ends", CQChartsCmdArg::Type::String,
                 "use line for arrow head").setHidden();

  argv.addCmdArg("-filled"    , CQChartsCmdArg::Type::SBool,
                 "background is filled" ).setHidden();
  argv.addCmdArg("-fill_color", CQChartsCmdArg::Type::Color,
                 "background fill color").setHidden();

  argv.addCmdArg("-stroked"     , CQChartsCmdArg::Type::SBool ,
                 "border is stroked"  ).setHidden();
  argv.addCmdArg("-stroke_color", CQChartsCmdArg::Type::Color ,
                 "border stroke color").setHidden();
  argv.addCmdArg("-stroke_width", CQChartsCmdArg::Type::Length,
                 "border stroke width").setHidden();

  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;
  }

  //---

  CQChartsArrowData arrowData;

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition start = argv.getParsePosition(view, plot, "start");
  CQChartsPosition end   = argv.getParsePosition(view, plot, "end"  );

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

  // single value (common head & tail angle), two values (separate head & tail angle)
  if (argv.hasParseArg("angle")) {
    QStringList strs;

    if (! CQTcl::splitList(argv.getParseStr("angle"), strs))
      return errorMsg(QString("Invalid angle string '%1'").arg(argv.getParseStr("angle")));

    if      (strs.length() == 1) {
      bool ok;
      double angle = CQChartsUtil::toReal(strs[0], ok);
      if (! ok) return errorMsg(QString("Invalid angle string '%1'").arg(strs[0]));
      if (angle > 0) arrowData.setAngle(angle);
    }
    else if (strs.length() == 2) {
      bool ok1, ok2;
      double angle1 = CQChartsUtil::toReal(strs[0], ok1);
      double angle2 = CQChartsUtil::toReal(strs[1], ok2);
      if (! ok1 || ! ok2) return errorMsg(QString("Invalid angle strings '%1' '%2'").
                                           arg(strs[0]).arg(strs[1]));

      if (angle1 > 0) arrowData.setFrontAngle(angle1);
      if (angle2 > 0) arrowData.setTailAngle (angle2);
    }
    else
      return errorMsg(QString("Invalid angle string '%1'").arg(argv.getParseStr("angle")));
  }

  // single value (common head & tail back angle), two values (separate head & tail back angle)
  if (argv.hasParseArg("back_angle")) {
    QStringList strs;

    if (! CQTcl::splitList(argv.getParseStr("back_angle"), strs))
      return errorMsg(QString("Invalid back_angle string '%1'").
                       arg(argv.getParseStr("back_angle")));

    if      (strs.length() == 1) {
      bool ok; double angle = CQChartsUtil::toReal(strs[0], ok);
      if (! ok) return errorMsg(QString("Invalid back_angle string '%1'").arg(strs[0]));
      if (angle > 0) arrowData.setBackAngle(angle);
    }
    else if (strs.length() == 2) {
      bool ok1; double angle1 = CQChartsUtil::toReal(strs[0], ok1);
      bool ok2; double angle2 = CQChartsUtil::toReal(strs[1], ok2);
      if (! ok1 && ! ok2) return errorMsg(QString("Invalid back_angle strings '%1' '%2'").
                                           arg(strs[0]).arg(strs[1]));

      if (angle1 > 0) arrowData.setFrontBackAngle(angle1);
      if (angle2 > 0) arrowData.setTailBackAngle (angle2);
    }
    else
      return errorMsg(QString("Invalid back_angle string '%1'").
                       arg(argv.getParseStr("back_angle")));
  }

  // single value (common head & tail length), two values (separate head & tail length)
  if (argv.hasParseArg("length")) {
    QStringList strs;

    if (! CQTcl::splitList(argv.getParseStr("length"), strs))
      return errorMsg(QString("Invalid length string '%1'").arg(argv.getParseStr("length")));

    if      (strs.length() == 1) {
      CQChartsLength len(strs[0], (view ? CQChartsUnits::VIEW : CQChartsUnits::PLOT));
      if (! len.isValid()) return errorMsg(QString("Invalid length string '%1'").arg(strs[0]));

      if (len.value() > 0) arrowData.setLength(len);
    }
    else if (strs.length() == 2) {
      CQChartsLength len1(strs[0], (view ? CQChartsUnits::VIEW : CQChartsUnits::PLOT));
      CQChartsLength len2(strs[1], (view ? CQChartsUnits::VIEW : CQChartsUnits::PLOT));
      if (! len1.isValid() || ! len2.isValid())
        return errorMsg(QString("Invalid length strings '%1' '%2'").arg(strs[0]).arg(strs[1]));

      if (len1.value() > 0) arrowData.setFrontLength(len1);
      if (len2.value() > 0) arrowData.setTailLength (len2);
    }
    else
      return errorMsg(QString("Invalid length string '%1'").arg(argv.getParseStr("length")));
  }

  // single value (common head & tail line ends), two values (separate head & tail line ends)
  if (argv.hasParseArg("line_ends")) {
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
      if (! ok1 && ! ok2) return errorMsg(QString("Invalid line_ends strings '%1' '%2'").
                                           arg(strs[0]).arg(strs[1]));

      arrowData.setFrontLineEnds(b1);
      arrowData.setTailLineEnds (b2);
    }
    else
      return errorMsg(QString("Invalid line_ends string '%1'").arg(argv.getParseStr("line_ends")));

    arrowData.setLineEnds(argv.getParseBool("line_ends", arrowData.isLineEnds()));
  }

  //---

  CQChartsShapeData shapeData;

  CQChartsFillData   &fill   = shapeData.fill();
  CQChartsStrokeData &stroke = shapeData.stroke();

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

  CQChartsArrowAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addArrowAnnotation(start, end);
  else if (plot)
    annotation = plot->addArrowAnnotation(start, end);
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setArrowData(arrowData);

  annotation->arrow()->setShapeData(shapeData);

  //---

  QStringList properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  //---

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsPointAnnotationCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---
  CQPerfTrace trace("CQChartsCmds::createChartsPointAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-position", CQChartsCmdArg::Type::Position, "point position");

  argv.addCmdArg("-type", CQChartsCmdArg::Type::String, "symbol type");
  argv.addCmdArg("-size", CQChartsCmdArg::Type::Length, "symbol size");

  argv.addCmdArg("-filled"    , CQChartsCmdArg::Type::SBool,
                 "symbol background is filled" ).setHidden();
  argv.addCmdArg("-fill_color", CQChartsCmdArg::Type::Color,
                 "symbol background fill color").setHidden();
  argv.addCmdArg("-fill_alpha", CQChartsCmdArg::Type::Real ,
                 "symbol background fill alpha").setHidden();

  argv.addCmdArg("-stroked"     , CQChartsCmdArg::Type::SBool ,
                 "symbol border stroke visible").setHidden();
  argv.addCmdArg("-stroke_color", CQChartsCmdArg::Type::Color ,
                 "symbol border stroke color"  ).setHidden();
  argv.addCmdArg("-stroke_alpha", CQChartsCmdArg::Type::Real  ,
                 "symbol border stroke alpha"  ).setHidden();
  argv.addCmdArg("-stroke_width", CQChartsCmdArg::Type::Length,
                 "symbol border stroke width"  ).setHidden();

  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;
  }

  //---

  CQChartsSymbolData symbolData;

  CQChartsFillData   &fill   = symbolData.fill();
  CQChartsStrokeData &stroke = symbolData.stroke();

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition pos = argv.getParsePosition(view, plot, "position");

  QString typeStr = argv.getParseStr("type");

  if (typeStr.length()) {
    if (typeStr == "?") {
      QStringList typeNames = CQChartsSymbol::typeNames();

      cmdBase_->setCmdRc(typeNames);

      return true;
    }

    CQChartsSymbol::Type type = CQChartsSymbol::nameToType(typeStr);

    if (type == CQChartsSymbol::Type::NONE)
      return errorMsg(QString("Invalid symbol type '%1'").arg(typeStr));

    symbolData.setType(type);
  }

  symbolData.setSize(argv.getParseLength(view, plot, "size", symbolData.size()));

  fill.setVisible(argv.getParseBool ("filled"    , fill.isVisible()));
  fill.setColor  (argv.getParseColor("fill_color", fill.color    ()));
  fill.setAlpha  (argv.getParseReal ("fill_alpha", fill.alpha    ()));

  stroke.setVisible(argv.getParseBool  ("stroked"     , stroke.isVisible()));
  stroke.setColor  (argv.getParseColor ("stroke_color", stroke.color    ()));
  stroke.setAlpha  (argv.getParseReal  ("stroke_alpha", stroke.alpha    ()));
  stroke.setWidth  (argv.getParseLength(view, plot, "stroke_width", stroke.width()));

  //---

  CQChartsPointAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addPointAnnotation(pos, symbolData.type());
  else if (plot)
    annotation = plot->addPointAnnotation(pos, symbolData.type());
  else
    return false;

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setSymbolData(symbolData);

  //---

  QStringList properties = argv.getParseStrs("properties");

  for (int i = 0; i < properties.length(); ++i) {
    if (properties[i].length())
      setAnnotationProperties(annotation, properties[i]);
  }

  //---

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
removeChartsAnnotationCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::removeChartsAnnotationCmd");

  // -view or -plot needed for -all
  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String , "annotation id");
  argv.addCmdArg("-all", CQChartsCmdArg::Type::Boolean, "all annotations");
  argv.endCmdGroup();

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // only id needed for specific
  if (argv.hasParseArg("id")) {
    QString id = argv.getParseStr("id");

    CQChartsAnnotation *annotation = getAnnotationByName(id);
    if (! annotation) return false;

    CQChartsPlot *plot = annotation->plot();
    CQChartsView *view = annotation->view();

    if (plot)
      plot->removeAnnotation(annotation);
    else
      view->removeAnnotation(annotation);
  }
  else {
    CQChartsView *view = nullptr;
    CQChartsPlot *plot = nullptr;

    if      (argv.hasParseArg("view")) {
      QString viewName = argv.getParseStr("view");

      view = getViewByName(viewName);
      if (! view) return false;
    }
    else if (argv.hasParseArg("plot")) {
      QString plotName = argv.getParseStr("plot");

      plot = getPlotByName(nullptr, plotName);
      if (! plot) return false;
    }

    if      (view)
      view->removeAllAnnotations();
    else if (plot)
      plot->removeAllAnnotations();
    else
      return errorMsg("-view or -plot needed for -all");
  }

  return true;
}

//------

bool
CQChartsCmds::
connectChartsSignalCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::connectChartsSignalCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-from", CQChartsCmdArg::Type::String, "from connection name");
  argv.addCmdArg("-to"  , CQChartsCmdArg::Type::String, "to procedure name");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;

    view = plot->view();
  }

  //---

  QString fromName = argv.getParseStr("from");
  QString toName   = argv.getParseStr("to"  );

  auto createCmdsSlot = [&]() {
    return new CQChartsCmdsSlot(this, view, plot, toName);
  };

  if      (fromName == "objIdPressed") {
    if (plot)
      connect(plot, SIGNAL(objIdPressed(const QString &)),
              createCmdsSlot(), SLOT(objIdPressed(const QString &)));
    else
      connect(view, SIGNAL(objIdPressed(const QString &)),
              createCmdsSlot(), SLOT(objIdPressed(const QString &)));
  }
  else if (fromName == "annotationIdPressed") {
    if (plot)
      connect(plot, SIGNAL(annotationIdPressed(const QString &)),
              createCmdsSlot(), SLOT(annotationIdPressed(const QString &)));
    else
      connect(view, SIGNAL(annotationIdPressed(const QString &)),
              createCmdsSlot(), SLOT(annotationIdPressed(const QString &)));
  }
  else if (fromName == "plotObjsAdded") {
    if (plot)
      connect(plot, SIGNAL(plotObjsAdded()), createCmdsSlot(), SLOT(plotObjsAdded()));
    else
      return errorMsg("unknown slot");
  }
  else if (fromName == "selectionChanged") {
    if (plot)
      connect(plot, SIGNAL(selectionChanged()), createCmdsSlot(), SLOT(selectionChanged()));
    else
      connect(view, SIGNAL(selectionChanged()), createCmdsSlot(), SLOT(selectionChanged()));
  }
  else if (fromName == "?") {
    QStringList names;

    if (plot)
      names = QStringList() << "objIdPressed" << "annotationIdPressed" <<
                               "plotObjsAdded" << "selectionChanged";
    else
      names = QStringList() << "objIdPressed" << "annotationIdPressed" << "selectionChanged";

    cmdBase_->setCmdRc(names);
  }
  else
    return errorMsg("unknown slot");

  return true;
}

//------

bool
CQChartsCmds::
printChartsImageCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::printChartsImageCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-file", CQChartsCmdArg::Type::String, "filename").setRequired();

  argv.addCmdArg("-layer", CQChartsCmdArg::Type::String, "layer name");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;

    view = plot->view();
  }

  //---

  QString filename = argv.getParseStr("file");

  if (plot) {
    QString layerName = argv.getParseStr("layer");

    if (layerName.length()) {
      CQChartsLayer::Type type = CQChartsLayer::nameType(layerName);

      if (! plot->printLayer(type, filename))
        return errorMsg("Failed to print layer");
    }
    else
      view->printFile(filename, plot);
  }
  else
    view->printFile(filename);

  return true;
}

//------

bool
CQChartsCmds::
writeChartsDataCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::writeChartsDataCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-type", CQChartsCmdArg::Type::String, "type");
  argv.addCmdArg("-file", CQChartsCmdArg::Type::String, "filename");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }
  else {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;

    view = plot->view();
  }

  //---

  QString filename = argv.getParseStr("file");

  std::ofstream fos; bool isFile = false;

  if (filename.length()) {
    fos.open(filename.toLatin1().constData());

    if (fos.fail())
      return errorMsg("Failed to open '" + filename + "'");

    isFile = true;
  }

  //---

  QString type = argv.getParseStr("type");

  if      (plot) {
    if      (type == "") {
      plot->write(isFile ? fos : std::cout);
    }
    else if (type == "annotations") {
      const CQChartsPlot::Annotations &annotations = plot->annotations();

      for (const auto &annotation : annotations)
        annotation->write(isFile ? fos : std::cout);
    }
    else if (type == "?") {
      QStringList names = QStringList() << "" << "annotations";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg("Invalid write type");
  }
  else if (view) {
    if      (type == "plots") {
      CQChartsView::Plots plots;

      view->getPlots(plots);

      for (const auto &plot : plots)
        plot->write(isFile ? fos : std::cout);
    }
    else if (type == "annotations") {
      const CQChartsView::Annotations &annotations = view->annotations();

      for (const auto &annotation : annotations)
        annotation->write(isFile ? fos : std::cout);
    }
    else if (type == "?") {
      QStringList names = QStringList() << "plots" << "annotations";

      cmdBase_->setCmdRc(names);
    }
    else {
      return errorMsg("Invalid write type");
    }
  }

  return true;
}

//------

bool
CQChartsCmds::
showChartsLoadModelDlgCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::showChartsLoadModelDlgCmd");

  argv.addCmdArg("-modal", CQChartsCmdArg::Type::Boolean, "show modal");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  bool modal = argv.getParseBool("modal");

  //---

  CQChartsLoadModelDlg *dlg = new CQChartsLoadModelDlg(charts_);

  if (modal)
    dlg->exec();
  else
    dlg->show();

  cmdBase_->setCmdRc(dlg->modelInd());

  return true;
}

//------

bool
CQChartsCmds::
showChartsManageModelsDlgCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::showChartsManageModelsDlgCmd");

  argv.addCmdArg("-modal", CQChartsCmdArg::Type::Boolean, "show modal");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  bool modal = argv.getParseBool("modal");

  //---

  CQChartsManageModelsDlg *dlg = new CQChartsManageModelsDlg(charts_);

  if (modal)
    dlg->exec();
  else
    dlg->show();

  return true;
}

//------

bool
CQChartsCmds::
showChartsCreatePlotDlgCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::showChartsCreatePlotDlgCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model_ind" );
  argv.addCmdArg("-view" , CQChartsCmdArg::Type::String , "view name" );
  argv.addCmdArg("-modal", CQChartsCmdArg::Type::Boolean, "show modal");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  bool modal = argv.getParseBool("modal");

  //---

  int     modelInd = argv.getParseInt("model", -1);
  QString viewName = argv.getParseStr("view");

  //---

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData)
    return errorMsg("No model data");

  //---

  CQChartsCreatePlotDlg *dlg = new CQChartsCreatePlotDlg(charts_, modelData);

  dlg->setViewName(viewName);

  if (modal)
    dlg->exec();
  else
    dlg->show();

  CQChartsPlot *plot = dlg->plot();

  cmdBase_->setCmdRc(plot ? plot->pathId() : "");

  return true;
}

//------

bool
CQChartsCmds::
showChartsHelpDlgCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::showChartsHelpDlgCmd");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  CQChartsHelpDlgMgrInst->showDialog(charts_);

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
#include <CQChartsPolygonEdit.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsRectEdit.h>
#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsSymbolDataEdit.h>
#include <CQChartsSymbolEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQChartsTextBoxDataEdit.h>

bool
CQChartsCmds::
testEditCmd(CQChartsCmdArgs &argv)
{
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  CQPerfTrace trace("CQChartsCmds::testEditCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-type", CQChartsCmdArg::Type::String, "type").setMultiple(true);

  argv.addCmdArg("-editable", CQChartsCmdArg::Type::SBool, "editable");

  bool rc;

  if (! argv.parse(rc))
    return rc;

  //---

  // get view or plot
  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return false;
  }

  if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return false;

    view = plot->view();
  }

  // get types
  QStringList types = argv.getParseStrs("type");

  bool editable = argv.getParseBool("editable", true);

  //---

  // create parent dialog
  QDialog *dialog = new QDialog;

  QGridLayout *layout = new QGridLayout(dialog);

  //---

  // add type edits
  int numEdits = 0;

  auto addEdit = [&](QWidget *w, const QString &label) {
    layout->addWidget(new QLabel(label), numEdits, 0);
    layout->addWidget(w                , numEdits, 1);

    CQChartsEditBase     *editBase     = qobject_cast<CQChartsEditBase     *>(w);
    CQChartsLineEditBase *lineEditBase = qobject_cast<CQChartsLineEditBase *>(w);

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
    const QString &type = types[i];

    if      (type == "alpha") {
      CQChartsAlphaEdit *edit = new CQChartsAlphaEdit; addEdit(edit, type);
    }
    else if (type == "arrow_data") {
      CQChartsArrowDataEdit *edit = new CQChartsArrowDataEdit; addEdit(edit, type);
    }
    else if (type == "arrow_data_line") {
      CQChartsArrowDataLineEdit *edit = new CQChartsArrowDataLineEdit; addEdit(edit, type);
    }
    else if (type == "box_data") {
      CQChartsBoxDataEdit *edit = new CQChartsBoxDataEdit; addEdit(edit, type);
    }
    else if (type == "box_data_line") {
      CQChartsBoxDataLineEdit *edit = new CQChartsBoxDataLineEdit; addEdit(edit, type);
    }
    else if (type == "color") {
      CQChartsColorEdit *edit = new CQChartsColorEdit; addEdit(edit, type);
    }
    else if (type == "color_line") {
      CQChartsColorLineEdit *edit = new CQChartsColorLineEdit; addEdit(edit, type);
    }
    else if (type == "column") {
      CQChartsColumnEdit *edit = new CQChartsColumnEdit; addEdit(edit, type);
    }
    else if (type == "column_line") {
      CQChartsColumnLineEdit *edit = new CQChartsColumnLineEdit; addEdit(edit, type);
    }
    else if (type == "columns") {
      CQChartsColumnsEdit *edit = new CQChartsColumnsEdit; addEdit(edit, type);
    }
    else if (type == "columns_line") {
      CQChartsColumnsLineEdit *edit = new CQChartsColumnsLineEdit; addEdit(edit, type);
    }
    else if (type == "fill_data") {
      CQChartsFillDataEdit *edit = new CQChartsFillDataEdit; addEdit(edit, type);
    }
    else if (type == "fill_data_line") {
      CQChartsFillDataLineEdit *edit = new CQChartsFillDataLineEdit; addEdit(edit, type);
    }
    else if (type == "fill_pattern") {
      CQChartsFillPatternEdit *edit = new CQChartsFillPatternEdit; addEdit(edit, type);
    }
    else if (type == "fill_under_side") {
      CQChartsFillUnderSideEdit *edit = new CQChartsFillUnderSideEdit; addEdit(edit, type);
    }
    else if (type == "fill_under_pos") {
      CQChartsFillUnderPosEdit *edit = new CQChartsFillUnderPosEdit; addEdit(edit, type);
    }
    else if (type == "fill_under_pos_line") {
      CQChartsFillUnderPosLineEdit *edit = new CQChartsFillUnderPosLineEdit; addEdit(edit, type);
    }
    else if (type == "filter") {
      CQChartsFilterEdit *edit = new CQChartsFilterEdit; addEdit(edit, type);
    }
    else if (type == "key_location") {
      CQChartsKeyLocationEdit *edit = new CQChartsKeyLocationEdit; addEdit(edit, type);
    }
    else if (type == "length") {
      CQChartsLengthEdit *edit = new CQChartsLengthEdit; addEdit(edit, type);
    }
    else if (type == "line_dash") {
      CQChartsLineDashEdit *edit = new CQChartsLineDashEdit; addEdit(edit, type);
    }
    else if (type == "line_data") {
      CQChartsLineDataEdit *edit = new CQChartsLineDataEdit; addEdit(edit, type);
    }
    else if (type == "line_data_line") {
      CQChartsLineDataLineEdit *edit = new CQChartsLineDataLineEdit; addEdit(edit, type);
    }
    else if (type == "polygon") {
      CQChartsPolygonEdit *edit = new CQChartsPolygonEdit; addEdit(edit, type);
    }
    else if (type == "polygon_line") {
      CQChartsPolygonLineEdit *edit = new CQChartsPolygonLineEdit; addEdit(edit, type);
    }
    else if (type == "position") {
      CQChartsPositionEdit *edit = new CQChartsPositionEdit; addEdit(edit, type);
    }
    else if (type == "rectangle") {
      CQChartsRectEdit *edit = new CQChartsRectEdit; addEdit(edit, type);
    }
    else if (type == "shape_data") {
      CQChartsShapeDataEdit *edit = new CQChartsShapeDataEdit; addEdit(edit, type);
    }
    else if (type == "shape_data_line") {
      CQChartsShapeDataLineEdit *edit = new CQChartsShapeDataLineEdit; addEdit(edit, type);
    }
    else if (type == "sides") {
      CQChartsSidesEdit *edit = new CQChartsSidesEdit; addEdit(edit, type);
    }
    else if (type == "stroke_data") {
      CQChartsStrokeDataEdit *edit = new CQChartsStrokeDataEdit; addEdit(edit, type);
    }
    else if (type == "stroke_data_line") {
      CQChartsStrokeDataLineEdit *edit = new CQChartsStrokeDataLineEdit; addEdit(edit, type);
    }
    else if (type == "symbol_data") {
      CQChartsSymbolDataEdit *edit = new CQChartsSymbolDataEdit; addEdit(edit, type);
    }
    else if (type == "symbol_data_line") {
      CQChartsSymbolDataLineEdit *edit = new CQChartsSymbolDataLineEdit; addEdit(edit, type);
    }
    else if (type == "symbol_type") {
      CQChartsSymbolEdit *edit = new CQChartsSymbolEdit; addEdit(edit, type);
    }
    else if (type == "text_box_data") {
      CQChartsTextBoxDataEdit *edit = new CQChartsTextBoxDataEdit; addEdit(edit, type);
    }
    else if (type == "text_box_data_line") {
      CQChartsTextBoxDataLineEdit *edit = new CQChartsTextBoxDataLineEdit; addEdit(edit, type);
    }
    else if (type == "text_data") {
      CQChartsTextDataEdit *edit = new CQChartsTextDataEdit; addEdit(edit, type);
    }
    else if (type == "text_data_line") {
      CQChartsTextDataLineEdit *edit = new CQChartsTextDataLineEdit; addEdit(edit, type);
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

CQChartsPlot *
CQChartsCmds::
createPlot(CQChartsView *view, const ModelP &model, CQChartsPlotType *type, bool reuse)
{
  if (! view)
    view = getView(reuse);

  //---

  CQChartsPlot *plot = type->create(view, model);

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

  CQChartsPlotType *type = plot->type();

  ModelP model = plot->model();

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

      if (! CQChartsModelUtil::stringToColumn(model.data(), (*p).second, column)) {
        (void) errorMsg("Bad column name '" + (*p).second + "'");
        continue;
      }

      QString scol = column.toString();

      if (! plot->setParameter(parameter, scol)) {
        (void) errorMsg("Failed to set parameter " + parameter->propName());
        continue;
      }
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST) {
      auto p = nameValueData.columns.find(parameter->name());

      if (p == nameValueData.columns.end())
        continue;

      const QString str = (*p).second;

      std::vector<CQChartsColumn> columns;

      if (! CQChartsModelUtil::stringToColumns(model.data(), str, columns)) {
        (void) errorMsg("Bad columns name '" + str + "'");
        continue;
      }

      QString s = CQChartsColumn::columnsToString(columns);

      if (! plot->setParameter(parameter, QVariant(s))) {
        (void) errorMsg("Failed to set parameter " + parameter->propName());
        continue;
      }
    }
    else {
      auto p = nameValueData.parameters.find(parameter->name());

      if (p == nameValueData.parameters.end())
        continue;

      QString value = (*p).second;

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

        var = QVariant(r);
      }
      else if (parameter->type() == CQChartsPlotParameter::Type::INTEGER) {
        bool ok;

        int i = CQChartsUtil::toInt(value.simplified(), ok);

        if (! ok) {
          (void) errorMsg("Invalid integer value '" + value + "' for '" +
                          parameter->typeName() + "'");
          continue;
        }

        var = QVariant(i);
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
        (void) errorMsg("Failed to set parameter " + parameter->propName());
        continue;
      }
    }
  }

  //---

  // add plot to view and show
  CQChartsView *view = plot->view();

  view->addPlot(plot, bbox);

  //---

  return true;
}

QString
CQChartsCmds::
fixTypeName(const QString &typeName)
{
  QString typeName1 = typeName;

  // adjust typename for alias (TODO: add to typeData)
  if      (typeName1 == "piechart"     ) typeName1 = "pie";
  else if (typeName1 == "xyplot"       ) typeName1 = "xy";
  else if (typeName1 == "scatterplot"  ) typeName1 = "scatter";
  else if (typeName1 == "bar"          ) typeName1 = "barchart";
  else if (typeName1 == "boxplot"      ) typeName1 = "box";
  else if (typeName1 == "parallelplot" ) typeName1 = "parallel";
  else if (typeName1 == "geometryplot" ) typeName1 = "geometry";
  else if (typeName1 == "delaunayplot" ) typeName1 = "delaunay";
  else if (typeName1 == "adjacencyplot") typeName1 = "adjacency";

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
setAnnotationProperties(CQChartsAnnotation *annotation, const QString &properties)
{
#if 1
  auto errorMsg = [&](const QString &msg) {
    charts_->errorMsg(msg);
    return false;
  };

  //---

  QStringList strs;

  if (! CQTcl::splitList(properties, strs))
    return errorMsg(QString("Invalid properties string '%1'").arg(properties));

  for (int i = 0; i < strs.length(); ++i) {
    QStringList strs1;

    if (! CQTcl::splitList(strs[i], strs1))
      return errorMsg(QString("Invalid property string '%1'").arg(strs[i]));

    if (strs1.size() != 2)
      return errorMsg(QString("Invalid property string '%1'").arg(strs[i]));

    annotation->setProperty(strs1[0], strs1[1]);
  }
#else
  if (! annotation->setProperties(properties))
    return errorMsg("Failed to set annotation properties '" + properties + "'");
#endif

  return true;
}

//------

CQChartsView *
CQChartsCmds::
getViewByName(const QString &viewName) const
{
  CQChartsView *view = nullptr;

  if (viewName != "") {
    view = charts_->getView(viewName);

    if (! view) {
      charts_->errorMsg("No view '" + viewName + "'");
      return nullptr;
    }
  }
  else {
    view = charts_->currentView();

    if (! view) {
      CQChartsCmds *th = const_cast<CQChartsCmds *>(this);

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
getPlotsByName(CQChartsView *view, const Vars &plotNames, Plots &plots) const
{
  bool rc = true;

  for (const auto &plotName : plotNames) {
    if (plotName.type() == QVariant::List) {
      QList<QVariant> listVars = plotName.toList();

      Vars plotNames1;

      for (int i = 0; i < listVars.length(); ++i)
        plotNames1.push_back(listVars[i]);

      if (! getPlotsByName(view, plotNames1, plots))
        rc = false;
    }
    else {
      CQChartsPlot *plot = getPlotByName(view, plotName.toString());

      if (plot)
        plots.push_back(plot);
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
  if (view) {
    CQChartsPlot *plot = view->getPlot(plotName);

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
    CQChartsPlot *plot = view->getPlot(plotName);

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
      const CQChartsPlot::Annotations &annotations = plot->annotations();

      for (const auto &annotation : annotations) {
        if (annotation->id() == name)
          return annotation;

        if (annotation->pathId() == name)
          return annotation;
      }
    }

    //---

    const CQChartsView::Annotations &annotations = view->annotations();

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

bool
CQChartsCmds::
loadFileModel(const QString &filename, CQChartsFileType type, const CQChartsInputData &inputData)
{
  bool hierarchical;

  QAbstractItemModel *model = loadFile(filename, type, inputData, hierarchical);

  if (! model)
    return false;

  ModelP modelp(model);

  CQChartsModelData *modelData = charts_->initModelData(modelp);

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

  charts_->setModelFileName(modelData, filename);

  return true;
}

QAbstractItemModel *
CQChartsCmds::
loadFile(const QString &filename, CQChartsFileType type, const CQChartsInputData &inputData,
         bool &hierarchical)
{
  CQChartsLoader loader(charts_);

  loader.setQtcl(cmdBase_->qtcl());

  return loader.loadFile(filename, type, inputData, hierarchical);
}

//------

bool
CQChartsCmds::
sortModel(ModelP &model, const QString &args)
{
  if (! args.length())
    return false;

  QString columnStr = args.simplified();

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
getModelDataOrCurrent(int ind)
{
  if (ind >= 0)
    return getModelData(ind);

  return charts_->currentModelData();
}

CQChartsModelData *
CQChartsCmds::
getModelData(int ind)
{
  return charts_->getModelData(ind);
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
  CQChartsView *view = charts_->addView();

  // TODO: handle multiple windows
  CQChartsWindow *window = charts_->createWindow(view);
  assert(window);

  return view;
}

//------

#if 0
// comma separated list of '<name>=<value>' pairs where <value> is a
// list of space separated columns
QStringList
CQChartsCmds::
stringToNamedColumns(const QString &str) const
{
  //QStringList strs = str.split(",", QString::SkipEmptyParts);

  QStringList words;

  CQStrParse parse(str);

  QString word;
  int     num_rbrackets = 0;
  int     num_sbrackets = 0;
  int     num_cbrackets = 0;

  while (! parse.eof()) {
    if      (parse.isChar(',') && num_rbrackets == 0 && num_sbrackets == 0 && num_cbrackets == 0) {
      if (word.length())
        words.push_back(word);

      parse.skipChar();

      word = "";
    }
    else if (parse.isChar('[')) {
      ++num_sbrackets;

      word += parse.getChar();
    }
    else if (parse.isChar(']')) {
      --num_sbrackets;

      word += parse.getChar();
    }
    else if (parse.isChar('(')) {
      ++num_rbrackets;

      word += parse.getChar();
    }
    else if (parse.isChar(')')) {
      --num_rbrackets;

      word += parse.getChar();
    }
    else if (parse.isChar('{')) {
      ++num_cbrackets;

      word += parse.getChar();
    }
    else if (parse.isChar('}')) {
      --num_cbrackets;

      word += parse.getChar();
    }
    else if (parse.isChar('\"') || parse.isChar('\'')) {
      int pos = parse.getPos();

      parse.skipString();

      word += parse.getBefore(pos);
    }
    else {
      word += parse.getChar();
    }
  }

  if (word.length())
    words.push_back(word);

  return words;
}
#endif
