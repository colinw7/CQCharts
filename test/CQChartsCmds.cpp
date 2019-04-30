#include <CQChartsCmds.h>
#include <CQChartsCmdArgs.h>
#include <CQChartsLoader.h>

#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsAxis.h>
#include <CQChartsAnnotation.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColor.h>
#include <CQChartsLineDash.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQChartsValueSet.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsArrow.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsInterfaceTheme.h>
#include <CQChartsTheme.h>

#include <CQChartsLoadModelDlg.h>
#include <CQChartsManageModelsDlg.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQChartsFilterModel.h>
#include <CQChartsAnalyzeModel.h>

#include <CQDataModel.h>
#include <CQSortModel.h>
#include <CQBucketModel.h>
#include <CQFoldedModel.h>
#include <CQSubSetModel.h>
#include <CQTransposeModel.h>
#include <CQSummaryModel.h>
#include <CQCollapseModel.h>
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
#include <QVBoxLayout>
#include <QFont>
#include <fstream>

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

    // define charts tcl proc
    addCommand("define_charts_proc", new CQChartsDefineChartsProcCmd(this));

    // correlation, bucket, folded, subset, transpose, summary, collapse, stats
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
    addCommand("create_charts_stats_model"   ,
               new CQChartsCreateChartsStatsModelCmd      (this));

    // measure text
    addCommand("measure_charts_text", new CQChartsMeasureChartsTextCmd(this));

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
    addCommand("create_charts_text_annotation"    ,
               new CQChartsCreateChartsTextAnnotationCmd    (this));
    addCommand("create_charts_arrow_annotation"   ,
               new CQChartsCreateChartsArrowAnnotationCmd   (this));
    addCommand("create_charts_rect_annotation"    ,
               new CQChartsCreateChartsRectAnnotationCmd    (this));
    addCommand("create_charts_ellipse_annotation" ,
               new CQChartsCreateChartsEllipseAnnotationCmd (this));
    addCommand("create_charts_polygon_annotation" ,
               new CQChartsCreateChartsPolygonAnnotationCmd (this));
    addCommand("create_charts_polyline_annotation",
               new CQChartsCreateChartsPolylineAnnotationCmd(this));
    addCommand("create_charts_point_annotation"   ,
               new CQChartsCreateChartsPointAnnotationCmd   (this));
    addCommand("remove_charts_annotation"         ,
               new CQChartsRemoveChartsAnnotationCmd        (this));

    // theme/palette
    addCommand("get_charts_palette", new CQChartsGetChartsPaletteCmd(this));
    addCommand("set_charts_palette", new CQChartsSetChartsPaletteCmd(this));

    // connect
    addCommand("connect_charts_signal", new CQChartsConnectChartsSignalCmd(this));

    // print, write
    addCommand("print_charts_image", new CQChartsPrintChartsImageCmd(this));
    addCommand("write_charts_data" , new CQChartsWriteChartsDataCmd(this));

    // dialogs
    addCommand("show_charts_load_model_dlg"   , new CQChartsShowChartsLoadModelDlgCmd(this));
    addCommand("show_charts_manage_models_dlg", new CQChartsShowChartsManageModelsDlgCmd(this));
    addCommand("show_charts_create_plot_dlg"  , new CQChartsShowChartsCreatePlotDlgCmd(this));

    // test
    addCommand("charts::test_edit", new CQChartsTestEditCmd(this));

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
}

//------

// load model from data
bool
CQChartsCmds::
loadChartsModelCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::loadChartsModelCmd");

  // input data type
  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-csv" , CQChartsCmdArg::Type::Boolean);
  argv.addCmdArg("-tsv" , CQChartsCmdArg::Type::Boolean);
  argv.addCmdArg("-json", CQChartsCmdArg::Type::Boolean);
  argv.addCmdArg("-data", CQChartsCmdArg::Type::Boolean);
  argv.addCmdArg("-expr", CQChartsCmdArg::Type::Boolean);
  argv.addCmdArg("-var" , CQChartsCmdArg::Type::String, "variable name");
  argv.endCmdGroup();

  // input data control
  argv.addCmdArg("-comment_header"     , CQChartsCmdArg::Type::Boolean);
  argv.addCmdArg("-first_line_header"  , CQChartsCmdArg::Type::Boolean);
  argv.addCmdArg("-first_column_header", CQChartsCmdArg::Type::Boolean);
  argv.addCmdArg("-separator"          , CQChartsCmdArg::Type::String );
  argv.addCmdArg("-columns"            , CQChartsCmdArg::Type::String );
  argv.addCmdArg("-transpose"          , CQChartsCmdArg::Type::Boolean);

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

  inputData.columns = columnsStr.split(" ", QString::SkipEmptyParts);

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

  if (fileType == CQChartsFileType::NONE) {
    charts_->errorMsg("No file type");
    return false;
  }

  if (fileType != CQChartsFileType::EXPR && fileType != CQChartsFileType::VARS) {
    if (filename == "") {
      charts_->errorMsg("No filename");
      return false;
    }
  }
  else {
    if (filename != "") {
      charts_->errorMsg("Extra filename");
      return false;
    }
  }

  if (! loadFileModel(filename, fileType, inputData))
    return false;

  CQChartsModelData *modelData = charts_->currentModelData();

  if (! modelData)
    return false;

  if (columnTypes.length()) {
    ModelP model = modelData->currentModel();

    for (int i = 0; i < columnTypes.length(); ++i)
      CQChartsModelUtil::setColumnTypeStrs(charts_, model.data(), columnTypes[i]);
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

  if (! argv.parse())
    return false;

  //---

  int modelInd = argv.getParseInt("model", -1);

  QString header = argv.getParseStr("header");
  QString type   = argv.getParseStr("type");
  QString expr   = argv.getParseStr("expr");

  //---

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    cmdBase_->setCmdError("No model data");
    return false;
  }

  //---

  // get expr model
  ModelP model = modelData->currentModel();

  CQChartsExprModel *exprModel = CQChartsModelUtil::getExprModel(model.data());

  //---

  // add new column (values from result of expression)
  if      (argv.getParseBool("add")) {
    if (! exprModel) {
      cmdBase_->setCmdError("Expression not supported for model");
      return false;
    }

    if (! argv.hasParseArg("expr")) {
      cmdBase_->setCmdError("Missing expression");
      return false;
    }

    int column;

    if (! exprModel->addExtraColumn(header, expr, column)) {
      cmdBase_->setCmdError("Failed to add column");
      return false;
    }

    //---

    if (type.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), column, type)) {
        cmdBase_->setCmdError(QString("Invalid type '" + type + "' for column '%1'").arg(column));
        return false;
      }
    }

    cmdBase_->setCmdRc(column);
  }
  // remove column (must be an added one)
  else if (argv.getParseBool("delete")) {
    if (! exprModel) {
      cmdBase_->setCmdError("Expression not supported for model");
      return false;
    }

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    if (! exprModel->removeExtraColumn(column.column())) {
      cmdBase_->setCmdError("Failed to delete column");
      return false;
    }

    cmdBase_->setCmdRc(-1);
  }
  // modify column (values from result of expression)
  else if (argv.getParseBool("modify")) {
    if (! exprModel) {
      cmdBase_->setCmdError("Expression not supported for model");
      return false;
    }

    if (! argv.hasParseArg("expr")) {
      cmdBase_->setCmdError("Missing expression");
      return false;
    }

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    if (exprModel->isOrigColumn(column.column())) {
      if (argv.getParseBool("force")) {
        exprModel->setReadOnly(false);

        bool rc = exprModel->assignColumn(header, column.column(), expr);

        exprModel->setReadOnly(true);

        if (! rc) {
          cmdBase_->setCmdError(QString("Failed to modify column '%1'").arg(column.column()));
          return false;
        }
      }
      else {
        cmdBase_->setCmdError("Use -force to modify original model data");
        return false;
      }
    }
    else {
      if (! exprModel->assignExtraColumn(header, column.column(), expr)) {
        cmdBase_->setCmdError(QString("Failed to modify column '%1'").arg(column.column()));
        return false;
      }
    }

    //---

    if (type.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), column, type)) {
        cmdBase_->setCmdError(QString("Invalid type '" + type + "' for column '%1'").
                              arg(column.column()));
        return false;
      }
    }

    cmdBase_->setCmdRc(column.column());
  }
  // calculate values from result of expression
  else if (argv.getParseBool("calc")) {
    if (! exprModel) {
      cmdBase_->setCmdError("Expression not supported for model");
      return false;
    }

    if (! argv.hasParseArg("expr")) {
      cmdBase_->setCmdError("Missing expression");
      return false;
    }

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
    if (! exprModel) {
      cmdBase_->setCmdError("Expression not supported for model");
      return false;
    }

    if (! argv.hasParseArg("expr")) {
      cmdBase_->setCmdError("Missing expression");
      return false;
    }

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
      if (! charts_->isPlotType(type)) {
        charts_->errorMsg("Invalid type '" + type + "'");
        return false;
      }

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
    if (! argv.hasParseArg("expr")) {
      cmdBase_->setCmdError("Missing expression");
      return false;
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
  CQPerfTrace trace("CQChartsCmds::defineChartsProcCmd");

  argv.addCmdArg("name", CQChartsCmdArg::Type::String, "proc name").setRequired();
  argv.addCmdArg("args", CQChartsCmdArg::Type::String, "proc args").setRequired();
  argv.addCmdArg("body", CQChartsCmdArg::Type::String, "proc body").setRequired();

  if (! argv.parse())
    return false;

  const Vars &pargs = argv.getParseArgs();

  if (pargs.size() != 3) {
    charts_->errorMsg("Usage: define_charts_proc <name> <args> <body>");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::measureChartsTextCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String , "value name").setRequired();
  argv.addCmdArg("-text", CQChartsCmdArg::Type::String , "text string");
  argv.addCmdArg("-html", CQChartsCmdArg::Type::Boolean, "is html");

  if (! argv.parse())
    return false;

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
    cmdBase_->setCmdError(QString("Invalid value name '%1'").arg(name));
    return false;
  }

  return true;
}

//------

bool
CQChartsCmds::
createChartsViewCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsViewCmd");

  if (! argv.parse())
    return false;

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

  if (! argv.parse())
    return false;

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
  CQPerfTrace trace("CQChartsCmds::createChartsPlotCmd");

  argv.addCmdArg("-view" , CQChartsCmdArg::Type::String , "view_id"  ).setRequired();
  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model_ind").setRequired();
  argv.addCmdArg("-type" , CQChartsCmdArg::Type::String , "type"     ).setRequired();
  argv.addCmdArg("-id"   , CQChartsCmdArg::Type::String , "plot id"  );

  argv.addCmdArg("-where"     , CQChartsCmdArg::Type::String, "filter");
  argv.addCmdArg("-columns"   , CQChartsCmdArg::Type::String, "columns");
  argv.addCmdArg("-parameter" , CQChartsCmdArg::Type::String, "name value");
  argv.addCmdArg("-xintegral" , CQChartsCmdArg::Type::SBool);
  argv.addCmdArg("-yintegral" , CQChartsCmdArg::Type::SBool);
  argv.addCmdArg("-xlog"      , CQChartsCmdArg::Type::SBool);
  argv.addCmdArg("-ylog"      , CQChartsCmdArg::Type::SBool);
  argv.addCmdArg("-title"     , CQChartsCmdArg::Type::String, "title");
  argv.addCmdArg("-properties", CQChartsCmdArg::Type::String, "name_values");
  argv.addCmdArg("-position"  , CQChartsCmdArg::Type::String, "position box");
  argv.addCmdArg("-xmin"      , CQChartsCmdArg::Type::Real  , "x");
  argv.addCmdArg("-ymin"      , CQChartsCmdArg::Type::Real  , "y");
  argv.addCmdArg("-xmax"      , CQChartsCmdArg::Type::Real  , "x");
  argv.addCmdArg("-ymax"      , CQChartsCmdArg::Type::Real  , "y");

  if (! argv.parse())
    return false;

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

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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

    QStringList strs = stringToNamedColumns(columnsStr);

    for (int j = 0; j < strs.size(); ++j) {
      const QString &nameValue = strs[j];

      auto pos = nameValue.indexOf('=');

      if (pos >= 0) {
        auto name  = nameValue.mid(0, pos).simplified();
        auto value = nameValue.mid(pos + 1).simplified();

        nameValueData.columns[name] = value;
      }
      else {
        charts_->errorMsg("Invalid -columns option '" + columnsStr + "'");
      }
    }
  }

  //--

  // plot parameter
  QStringList parameterStrs = argv.getParseStrs("parameter");

  for (int i = 0; i < parameterStrs.length(); ++i) {
    const QString &parameterStr = parameterStrs[i];

    auto pos = parameterStr.indexOf('=');

    QString name, value;

    if (pos >= 0) {
      name  = parameterStr.mid(0, pos).simplified();
      value = parameterStr.mid(pos + 1).simplified();
    }
    else {
      name  = parameterStr;
      value = "true";
    }

    nameValueData.parameters[name] = value;
  }

  //------

  if (typeName == "") {
    charts_->errorMsg("No type specified for plot");
    return false;
  }

  typeName = fixTypeName(typeName);

  // ignore if bad type
  if (! charts_->isPlotType(typeName)) {
    charts_->errorMsg("Invalid type '" + typeName + "' for plot");
    return false;
  }

  CQChartsPlotType *type = charts_->plotType(typeName);
  assert(type);

  //------

  double vr = CQChartsView::viewportRange();

  CQChartsGeom::BBox bbox(0, 0, vr, vr);

  if (positionStr != "") {
    QStringList positionStrs = positionStr.split(" ", QString::SkipEmptyParts);

    if (positionStrs.length() == 4) {
      bool ok1, ok2, ok3, ok4;

      double pxmin = CQChartsUtil::toReal(positionStrs[0], ok1);
      double pymin = CQChartsUtil::toReal(positionStrs[1], ok2);
      double pxmax = CQChartsUtil::toReal(positionStrs[2], ok3);
      double pymax = CQChartsUtil::toReal(positionStrs[3], ok4);

      if (ok1 && ok2 && ok3 && ok4) {
        bbox = CQChartsGeom::BBox(pxmin, pymin, pxmax, pymax);
      }
      else
        charts_->errorMsg("Invalid position '" + positionStr + "'");
    }
    else {
      charts_->errorMsg("Invalid position '" + positionStr + "'");
    }
  }

  //------

  // create plot from init (argument) data
  CQChartsPlot *plot = createPlot(view, model, modelData->selectionModel(), type, true);

  if (! plot) {
    charts_->errorMsg("Failed to create plot");
    return false;
  }

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

    if (type->allowXLog())
      plot->setLogX(xlog);
    else
      charts_->errorMsg("plot type does not support x log option");
  }

  if (argv.hasParseArg("ylog")) {
    bool ylog = argv.getParseBool("ylog");

    if (type->allowYLog())
      plot->setLogY(ylog);
    else
      charts_->errorMsg("plot type does not support y log option");
  }

  //---

  // set x/y integral if allowed)
  if (argv.hasParseArg("xintegral")) {
    bool xintegral = argv.getParseBool("xintegral");

    if (type->allowXAxisIntegral() && plot->xAxis())
      plot->xAxis()->setIntegral(xintegral);
    else
      charts_->errorMsg("plot type does not support x integral option");
  }

  if (argv.hasParseArg("yintegral")) {
    bool yintegral = argv.getParseBool("yintegral");

    if (type->allowYAxisIntegral() && plot->yAxis())
      plot->yAxis()->setIntegral(yintegral);
    else
      charts_->errorMsg("plot type does not support y integral option");
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
    if (properties[i].length())
      setPlotProperties(plot, properties[i]);
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

  if (! argv.parse())
    return false;

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
  CQPerfTrace trace("CQChartsCmds::getChartsPropertyCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view"      , CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot"      , CQChartsCmdArg::Type::String, "plot name");
  argv.addCmdArg("-annotation", CQChartsCmdArg::Type::String, "annotation name");
  argv.endCmdGroup();

  argv.addCmdArg("-object", CQChartsCmdArg::Type::String, "object id");

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String, "property name");

  argv.addCmdArg("-desc", CQChartsCmdArg::Type::Boolean, "return property description");

  if (! argv.parse())
    return false;

  //---

  QString objectId = argv.getParseStr("object");

  QString name = argv.getParseStr("name");

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);

    if (! view) {
      charts_->errorMsg("Invalid view '" + viewName + "'");
      return false;
    }

    if      (name == "?") {
      QStringList names;
      bool        hidden = false;

      view->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (argv.hasParseArg("desc")) {
      QString desc;

      if (! view->getPropertyDesc(name, desc)) {
        charts_->errorMsg("Failed to get view parameter description '" + name + "'");
        return false;
      }

      cmdBase_->setCmdRc(desc);
    }
    else {
      QVariant value;

      if (! view->getProperty(name, value)) {
        charts_->errorMsg("Failed to get view parameter '" + name + "'");
        return false;
      }

      cmdBase_->setCmdRc(value);
    }
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsPlot *plot = getPlotByName(nullptr, plotName);

    if (! plot) {
      charts_->errorMsg("Invalid plot '" + plotName + "'");
      return false;
    }

    CQChartsPlotObj *plotObj = nullptr;

    if (objectId.length()) {
      plotObj = plot->getObject(objectId);

      if (! plotObj) {
        charts_->errorMsg("Invalid plot object id '" + objectId + "'");
        return false;
      }
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

        if (! CQUtil::getProperty(plotObj, name, value)) {
          charts_->errorMsg("Failed to get plot parameter '" + name + "'");
          return false;
        }

        cmdBase_->setCmdRc(value);
      }
    }
    // plot property
    else {
      if      (name == "?") {
        QStringList names;
        bool        hidden = false;

        plot->getPropertyNames(names, hidden);

        cmdBase_->setCmdRc(names);
      }
      else if (argv.hasParseArg("desc")) {
        QString desc;

        if (! plot->getPropertyDesc(name, desc)) {
          charts_->errorMsg("Failed to get plot parameter description '" + name + "'");
          return false;
        }

        cmdBase_->setCmdRc(desc);
      }
      else {
        QVariant value;

        if (! plot->getProperty(name, value)) {
          charts_->errorMsg("Failed to get plot parameter '" + name + "'");
          return false;
        }

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
      bool        hidden = false;

      annotation->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (argv.hasParseArg("desc")) {
      QString desc;

      if (! annotation->getPropertyDesc(name, desc)) {
        charts_->errorMsg("Failed to get annotation parameter description '" + name + "'");
        return false;
      }

      cmdBase_->setCmdRc(desc);
    }
    else {
      QVariant value;

      if (! annotation->getProperty(name, value)) {
        charts_->errorMsg("Failed to get annotation parameter '" + name + "'");
        return false;
      }

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
  CQPerfTrace trace("CQChartsCmds::setChartsPropertyCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view"      , CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot"      , CQChartsCmdArg::Type::String, "plot name");
  argv.addCmdArg("-annotation", CQChartsCmdArg::Type::String, "annotation name");
  argv.endCmdGroup();

  argv.addCmdArg("-name" , CQChartsCmdArg::Type::String, "property name");
  argv.addCmdArg("-value", CQChartsCmdArg::Type::String, "property view");

  if (! argv.parse())
    return false;

  //---

  QString name  = argv.getParseStr("name");
  QString value = argv.getParseStr("value");

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);

    if (! view) {
      charts_->errorMsg("Invalid view '" + viewName + "'");
      return false;
    }

    if (! view->setProperty(name, value)) {
      charts_->errorMsg("Failed to set view parameter '" + name + "'");
      return false;
    }
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsPlot *plot = getPlotByName(nullptr, plotName);

    if (! plot) {
      charts_->errorMsg("Invalid plot '" + plotName + "'");
      return false;
    }

    if (! plot->setProperty(name, value)) {
      charts_->errorMsg("Failed to set plot parameter '" + name + "'");
      return false;
    }
  }
  else if (argv.hasParseArg("annotation")) {
    QString annotationName = argv.getParseStr("annotation");

    CQChartsAnnotation *annotation = getAnnotationByName(annotationName);
    if (! annotation) return false;

    if (! annotation->setProperty(name, value)) {
      charts_->errorMsg("Failed to set annotation parameter '" + name + "'");
      return false;
    }
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

  if (! argv.parse())
    return false;

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

      CQChartsThemeMgrInst->getPaletteNames(names);

      cmdBase_->setCmdRc(names);
    }
    else if (nameStr == "themes") {
      QStringList names;

      CQChartsThemeMgrInst->getThemeNames(names);

      cmdBase_->setCmdRc(names);
    }
    else if (nameStr == "color_models") {
      int n = CQChartsGradientPalette::numModels();

      QStringList names;

      for (int i = 0; i < n; ++i)
        names << CQChartsGradientPalette::modelName(i).c_str();

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
    CQChartsTheme *theme = CQChartsThemeMgrInst->getTheme(themeStr);
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

    else if (nameStr == "select_color") { cmdBase_->setCmdRc(theme->selectColor()); }
    else if (nameStr == "inside_color") { cmdBase_->setCmdRc(theme->insideColor()); }

    else if (nameStr == "?") {
      QStringList names = QStringList() <<
        "name" << "desc" << "palettes" << "select_color" << "inside_color";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid theme value name '%1'").arg(nameStr));
  }
  // get palette data
  else if (paletteFlag) {
    CQChartsGradientPalette *palette = CQChartsThemeMgrInst->getNamedPalette(paletteStr);
    if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(themeStr));

    if      (nameStr == "name") { cmdBase_->setCmdRc(palette->name()); }
    else if (nameStr == "desc") { cmdBase_->setCmdRc(palette->desc()); }

    else if (nameStr == "color_type") {
      cmdBase_->setCmdRc(CQChartsGradientPalette::colorTypeToString(palette->colorType()));
    }
    else if (nameStr == "color_model") {
      cmdBase_->setCmdRc(CQChartsGradientPalette::colorModelToString(palette->colorModel()));
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
    else if (nameStr == "colors") {
      int n = palette->numColors();

      QVariantList colors;

      for (int i = 0; i < n; ++i)
        colors << palette->icolor(i);

      cmdBase_->setCmdRc(colors);
    }
    else if (nameStr == "color") {
      if (! dataFlag) return errorMsg("Missing data for palette color");

      bool ok;

      int i = CQChartsUtil::toInt(dataStr, ok);
      if (! ok) return errorMsg(QString("Invalid color index '%1'").arg(dataStr));

      int n = palette->numColors();

      if (i < 0 || i >= n) return errorMsg(QString("Invalid color index '%1'").arg(dataStr));

      cmdBase_->setCmdRc(palette->icolor(i));
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
  argv.addCmdArg("-value", CQChartsCmdArg::Type::String, "value value").setRequired();
  argv.addCmdArg("-data" , CQChartsCmdArg::Type::String, "value name data");

  if (! argv.parse())
    return false;

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
    CQChartsTheme *theme = CQChartsThemeMgrInst->getTheme(themeStr);
    if (! theme) return errorMsg(QString("Invalid theme '%1'").arg(themeStr));

    if      (nameStr == "name") theme->setName(valueStr);
    else if (nameStr == "desc") theme->setDesc(valueStr);

    else if (nameStr == "select_color") { theme->setSelectColor(QColor(valueStr)); }
    else if (nameStr == "inside_color") { theme->setInsideColor(QColor(valueStr)); }

    else if (nameStr == "?") {
      QStringList names = QStringList() <<
        "name" << "desc" << "select_color" << "inside_color";

      cmdBase_->setCmdRc(names);
    }
    else
      return errorMsg(QString("Invalid theme value name '%1'").arg(nameStr));
  }
  // set palette data
  else if (paletteFlag) {
    CQChartsGradientPalette *palette = CQChartsThemeMgrInst->getNamedPalette(paletteStr);
    if (! palette) return errorMsg(QString("Invalid palette '%1'").arg(themeStr));

    if      (nameStr == "name") palette->setName(valueStr);
    else if (nameStr == "desc") palette->setDesc(valueStr);

    else if (nameStr == "color_type") {
      palette->setColorType(CQChartsGradientPalette::stringToColorType(valueStr));
    }
    else if (nameStr == "color_model") {
      palette->setColorModel(CQChartsGradientPalette::stringToColorModel(valueStr));
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
      struct DefinedColor {
        double v { -1.0 };
        QColor c;

        DefinedColor(double v, const QColor &c) :
         v(v), c(c) {
        }
      };

      using DefinedColors = std::vector<DefinedColor>;

      //--

      QString definedStr = argv.getParseStr("defined");

      QStringList strs = definedStr.split(" ", QString::SkipEmptyParts);

      if (! strs.length()) return errorMsg(QString("Invalid defined colors '%1'").arg(valueStr));

      double dv = (strs.length() > 1 ? 1.0/(strs.length() - 1) : 0.0);

      DefinedColors definedColors;

      for (int j = 0; j < strs.length(); ++j) {
        int pos = strs[j].indexOf('=');

        double v = j*dv;
        QColor c;

        if (pos > 0) {
          QString lhs = strs[j].mid(0, pos).simplified();
          QString rhs = strs[j].mid(pos + 1).simplified();

          bool ok;

          v = CQChartsUtil::toReal(lhs, ok);
          c = QColor(rhs);
        }
        else
          c = QColor(strs[j]);

        definedColors.push_back(DefinedColor(v, c));
      }

      if (! definedColors.empty()) {
        palette->resetDefinedColors();

        for (const auto &definedColor : definedColors)
          palette->addDefinedColor(definedColor.v, definedColor.c);
      }
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
  CQPerfTrace trace("CQChartsCmds::groupChartsPlotsCmd");

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name").setRequired();

  argv.addCmdArg("-x1x2"   , CQChartsCmdArg::Type::Boolean, "use shared x axis");
  argv.addCmdArg("-y1y2"   , CQChartsCmdArg::Type::Boolean, "use shared y axis");
  argv.addCmdArg("-overlay", CQChartsCmdArg::Type::Boolean, "overlay");

  if (! argv.parse())
    return false;

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
    if (plots.size() != 2) {
      charts_->errorMsg("Need 2 plots for x1x2");
      return false;
    }

    view->initX1X2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (y1y2) {
    if (plots.size() != 2) {
      charts_->errorMsg("Need 2 plots for y1y2");
      return false;
    }

    view->initY1Y2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (overlay) {
    if (plots.size() < 2) {
      charts_->errorMsg("Need 2 or more plots for overlay");
      return false;
    }

    view->initOverlay(plots, /*reset*/true);
  }
  else {
    charts_->errorMsg("No grouping specified");
    return false;
  }

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

  if (! argv.parse())
    return false;

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

  view->placePlots(plots, vertical, horizontal, rows, columns);

  return true;
}

//------

bool
CQChartsCmds::
foldChartsModelCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::foldChartsModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column to fold");

  if (! argv.parse())
    return false;

  //---

  int modelInd = argv.getParseInt("model", -1);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::flattenChartsModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-group", CQChartsCmdArg::Type::Column , "grouping column id");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-mean" , CQChartsCmdArg::Type::Boolean, "calc mean of column values");
  argv.addCmdArg("-sum"  , CQChartsCmdArg::Type::Boolean, "calc sum of column values");
  argv.endCmdGroup();

  if (! argv.parse())
    return false;

  //---

  int  modelInd = argv.getParseInt ("model" , -1);
  bool meanFlag = argv.getParseBool("mean");
  bool sumFlag  = argv.getParseBool("sum");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::copyChartsModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");

  if (! argv.parse())
    return false;

  //---

  int modelInd = argv.getParseInt("model" , -1);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::writeChartsModelCmd");

  argv.addCmdArg("-model"    , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-header"   , CQChartsCmdArg::Type::SBool  , "show header");
  argv.addCmdArg("-max_rows" , CQChartsCmdArg::Type::Integer, "maximum number of rows to write");
  argv.addCmdArg("-max_width", CQChartsCmdArg::Type::Integer, "maximum column width");
  argv.addCmdArg("-hier"     , CQChartsCmdArg::Type::SBool  , "output hierarchically");

  if (! argv.parse())
    return false;

  //---

  int modelInd = argv.getParseInt("model", -1);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::sortChartsModelCmd");

  argv.addCmdArg("-model"     , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column"    , CQChartsCmdArg::Type::Column , "column to sort");
  argv.addCmdArg("-decreasing", CQChartsCmdArg::Type::Boolean, "invert sort");

  if (! argv.parse())
    return false;

  //---

  int  modelInd   = argv.getParseInt   ("model" , -1);
  bool decreasing = argv.getParseBool  ("decreasing");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::filterChartsModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-expr"  , CQChartsCmdArg::Type::String , "filter expression");
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column");
  argv.addCmdArg("-type"  , CQChartsCmdArg::Type::String , "filter type");

  if (! argv.parse())
    return false;

  //---

  int     modelInd = argv.getParseInt("model", -1);
  QString expr     = argv.getParseStr("expr");
  QString type     = argv.getParseStr("type");

  if (! argv.hasParseArg("type"))
    type = "expression";

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

  ModelP model = modelData->currentModel();

  CQChartsModelFilter *modelFilter = qobject_cast<CQChartsModelFilter *>(model.data());

  if (! modelFilter) {
    charts_->errorMsg("No filter support for model");
    return false;
  }

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
  else {
    charts_->errorMsg(QString("Invalid type '%1'").arg(type));
    return false;
  }

  return true;
}

//------

bool
CQChartsCmds::
createChartsCorrelationModelCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsCorrelationModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-flip" , CQChartsCmdArg::Type::Boolean, "correlate rows instead of columns");

  if (! argv.parse())
    return false;

  //---

  int  modelInd = argv.getParseInt ("model", -1);
  bool flip     = argv.getParseBool("flip");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::createChartsFoldedModelCmd");

  argv.addCmdArg("-model"       , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column"      , CQChartsCmdArg::Type::Column , "column to fold");
  argv.addCmdArg("-fold_keep"   , CQChartsCmdArg::Type::Boolean, "keep folded column");
  argv.addCmdArg("-fold_data"   , CQChartsCmdArg::Type::Boolean, "show folded column child data");
  argv.addCmdArg("-bucket_count", CQChartsCmdArg::Type::Integer, "bucket count");
  argv.addCmdArg("-bucket_delta", CQChartsCmdArg::Type::Real   , "bucket delta");

  if (! argv.parse())
    return false;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::createChartsBucketModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column to bucket");
  argv.addCmdArg("-multi" , CQChartsCmdArg::Type::Boolean, "multiple bucket columns");
  argv.addCmdArg("-start" , CQChartsCmdArg::Type::Real   , "bucket start");
  argv.addCmdArg("-delta" , CQChartsCmdArg::Type::Real   , "bucket delta");
  argv.addCmdArg("-min"   , CQChartsCmdArg::Type::Real   , "bucket min");
  argv.addCmdArg("-max"   , CQChartsCmdArg::Type::Real   , "bucket max");
  argv.addCmdArg("-count" , CQChartsCmdArg::Type::Integer, "number of buckets");

  if (! argv.parse())
    return false;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::createChartsSubsetModelCmd");

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-left"  , CQChartsCmdArg::Type::Column , "left (start) column");
  argv.addCmdArg("-right" , CQChartsCmdArg::Type::Column , "right (end) column");
  argv.addCmdArg("-top"   , CQChartsCmdArg::Type::Integer, "top (start) row");
  argv.addCmdArg("-bottom", CQChartsCmdArg::Type::Integer, "bottom (end) row");

  if (! argv.parse())
    return false;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::createChartsTransposeModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");

  if (! argv.parse())
    return false;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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

  if (! argv.parse())
    return false;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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

    if (sortRole < 0) {
      charts_->errorMsg("Invalid sort role");
      return false;
    }

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
  CQPerfTrace trace("CQChartsCmds::createChartsCollapseModelCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-sum"  , CQChartsCmdArg::Type::String , "columns to calculate sum");
  argv.addCmdArg("-mean" , CQChartsCmdArg::Type::String , "columns to calculate mean");

  if (! argv.parse())
    return false;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
        charts_->errorMsg("Bad columns name '" + columnsStr + "'");
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
createChartsStatsModelCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsStatsModelCmd");

  argv.addCmdArg("-model"  , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-columns", CQChartsCmdArg::Type::String , "columns");

  if (! argv.parse())
    return false;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
        charts_->errorMsg("Bad columns name '" + columnsStr + "'");
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

    QModelIndex parent;

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
  CQPerfTrace trace("CQChartsCmds::exportChartsModelCmd");

  argv.addCmdArg("-model"  , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-to"     , CQChartsCmdArg::Type::String , "destination format");
  argv.addCmdArg("-file"   , CQChartsCmdArg::Type::String , "file name");
  argv.addCmdArg("-hheader", CQChartsCmdArg::Type::SBool  , "output horizontal header");
  argv.addCmdArg("-vheader", CQChartsCmdArg::Type::SBool  , "output vertical header");

  if (! argv.parse())
    return false;

  //---

  int     modelInd = argv.getParseInt ("model", -1);
  QString toName   = argv.getParseStr ("to", "csv");
  QString filename = argv.getParseStr ("file", "");
  bool    hheader  = argv.getParseBool("hheader", true);
  bool    vheader  = argv.getParseBool("vheader", false);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

  std::ofstream fos; bool isFile = false;

  if (filename.length()) {
    fos.open(filename.toLatin1().constData());

    if (fos.fail()) {
      charts_->errorMsg("Failed to open '" + filename + "'");
      return false;
    }

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
  else {
    charts_->errorMsg("Invalid output format");
    return false;
  }

  return true;
}

//------

// get charts data
bool
CQChartsCmds::
getChartsDataCmd(CQChartsCmdArgs &argv)
{
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

  argv.addCmdArg("-hidden", CQChartsCmdArg::Type::Boolean, "include hidden data");

  if (! argv.parse())
    return false;

  //---

  QString objectId = argv.getParseStr("object");

  bool    header = argv.getParseBool("header");
  QString name   = argv.getParseStr ("name");
  QString data   = argv.getParseStr ("data");
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

    if (role < 0) {
      charts_->errorMsg("Invalid role");
      return false;
    }
  }

  //---

  // model data
  if      (argv.hasParseArg("model")) {
    int modelInd = argv.getParseInt("model", -1);

    // get model
    CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

    if (! modelData) {
      charts_->errorMsg("No model data");
      return false;
    }

    ModelP model = modelData->currentModel();

    //---

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    CQChartsRow row = argv.getParseRow("row");

    if (argv.hasParseArg("ind")) {
      int irow, icol;

      if (! CQTclUtil::stringToModelIndex(argv.getParseStr("ind"), irow, icol)) {
        charts_->errorMsg("Invalid model index");
        return false;
      }

      row    = irow;
      column = icol;
    }

    //---

    // get column header or row, column value
    if      (name == "value") {
      QVariant var;

      if (header) {
        if (! column.isValid()) {
          cmdBase_->setCmdError("Invalid header column specified");
          return false;
        }

        bool ok;

        var = CQChartsModelUtil::modelHeaderValue(model.data(), column, role, ok);

        if (! var.isValid()) {
          cmdBase_->setCmdError("Invalid header value");
          return false;
        }
      }
      else {
#if 0
        QModelIndex ind = model.data()->index(row, column.column());

        if (! ind.isValid()) {
          cmdBase_->setCmdError("Invalid data row/column specified");
          return false;
        }
#endif

        QModelIndex parent;

        bool ok;

        var = CQChartsModelUtil::modelValue(charts_, model.data(), row.row(),
                                            column, parent, role, ok);

        if (! var.isValid()) {
          cmdBase_->setCmdError("Invalid model value");
          return false;
        }
      }

      cmdBase_->setCmdRc(var);
    }
    // get meta data
    else if (name == "meta") {
      QVariant var = CQChartsModelUtil::modelMetaValue(model.data(), data);

      if (! var.isValid()) {
        cmdBase_->setCmdError("Invalid meta data");
        return false;
      }

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
        if (column < 0 || column.column() >= nc) {
          cmdBase_->setCmdError("Invalid column number");
          return false;
        }

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

      if (row.row() < 0 || row.row() >= nr) {
        cmdBase_->setCmdError("Invalid row number");
        return false;
      }

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

      if (column < 0 || column.column() >= nc) {
        cmdBase_->setCmdError("Invalid column number");
        return false;
      }

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
        if (! column.isValid() || column.column() >= details->numColumns()) {
          cmdBase_->setCmdError("Invalid column specified");
          return false;
        }

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

      if (! column.isValid() || column.column() >= details->numColumns()) {
        cmdBase_->setCmdError("Invalid column specified");
        return false;
      }

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
        if (! column.isValid()) {
          cmdBase_->setCmdError("Invalid column specified");
          return false;
        }

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
      CQChartsColumn column;

      if (! CQChartsModelUtil::stringToColumn(model.data(), data, column))
        column = -1;

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

      QVariant value;

      if (! CQUtil::getProperty(model.data(), name1, value)) {
        charts_->errorMsg("Failed to get model property '" + name1 + "'");
        return false;
      }

      cmdBase_->setCmdRc(value);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
        "value" << "meta" << "num_rows" << "num_columns" << "hierarchical" <<
        "header" << "row" << "column" << "map" << "duplicates" << "column_index" <<
        "title" << "property.<name>";

      names << CQChartsModelColumnDetails::getLongNamedValues();

      cmdBase_->setCmdRc(names);
    }
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
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
      CQChartsLength len(data, CQChartsUnits::VIEW);

      double w = view->lengthViewWidth(len);

      cmdBase_->setCmdRc(w);
    }
    else if (name == "view_height") {
      CQChartsLength len(data, CQChartsUnits::VIEW);

      double h = view->lengthViewHeight(len);

      cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_width") {
      CQChartsLength len(data, CQChartsUnits::VIEW);

      double w = view->lengthPixelWidth(len);

      cmdBase_->setCmdRc(w);
    }
    else if (name == "pixel_height") {
      CQChartsLength len(data, CQChartsUnits::VIEW);

      double h = view->lengthPixelHeight(len);

      cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_position") {
      CQChartsPosition pos(data, CQChartsUnits::VIEW);

      QPointF p = view->positionToPixel(pos);

      cmdBase_->setCmdRc(p);
    }
    else if (name == "properties") {
      QStringList names;

      view->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "plots" << "annotations" << "selected_objects" << "view_width" << "view_height" <<
       "pixel_width" << "pixel_height" << "pixel_position" << "properties";

      cmdBase_->setCmdRc(names);
    }
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
  }
  // type data
  else if (argv.hasParseArg("type")) {
    QString typeName = argv.getParseStr("type");

    if (! charts_->isPlotType(typeName)) {
      charts_->errorMsg("No type '" + typeName + "'");
      return false;
    }

    CQChartsPlotType *type = charts_->plotType(typeName);

    if (! type) {
      charts_->errorMsg("No type '" + typeName + "'");
      return false;
    }

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
      if (! type->hasParameter(data)) {
        charts_->errorMsg("No parameter '" + data + "'");
        return false;
      }

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
      else {
        cmdBase_->setCmdError("Invalid name 'parameter." + name1 + "' specified");
        return false;
      }
    }
    else if (type->hasProperty(name)) {
      cmdBase_->setCmdRc(type->getPropertyValue(name));
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "properties" << "parameters" << "parameter.<parameter_name>" << "<property_name>";

      cmdBase_->setCmdRc(names);
    }
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
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

      if (! modelData) {
        charts_->errorMsg("No model data");
        return false;
      }

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
        if (! column.isValid()) {
          cmdBase_->setCmdError("Invalid header column specified");
          return false;
        }

        bool ok;

        var = CQChartsModelUtil::modelHeaderValue(plot->model().data(), column, role, ok);

        if (! var.isValid()) {
          cmdBase_->setCmdError("Invalid header value");
          return false;
        }
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

      if (! modelData) {
        charts_->errorMsg("No model data");
        return false;
      }

      CQChartsColumn column = argv.getParseColumn("column", plot->model().data());

      //---

      CQChartsModelDetails *details = modelData->details();

      if (! column.isValid() || column.column() >= details->numColumns()) {
        cmdBase_->setCmdError("Invalid column specified");
        return false;
      }

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
      if (! objectId.length()) {
        charts_->errorMsg("Missing object id");
        return false;
      }

      QList<QModelIndex> inds = plot->getObjectInds(objectId);

      QVariantList vars;

      for (int i = 0; i < inds.length(); ++i)
        vars.push_back(inds[i]);

      cmdBase_->setCmdRc(vars);
    }
    else if (name == "plot_width") {
      CQChartsLength len(data, CQChartsUnits::PLOT);

      double w = plot->lengthPlotWidth(len);

      cmdBase_->setCmdRc(w);
    }
    else if (name == "plot_height") {
      CQChartsLength len(data, CQChartsUnits::PLOT);

      double h = plot->lengthPlotHeight(len);

      cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_width") {
      CQChartsLength len(data, CQChartsUnits::PLOT);

      double w = plot->lengthPixelWidth(len);

      cmdBase_->setCmdRc(w);
    }
    else if (name == "pixel_height") {
      CQChartsLength len(data, CQChartsUnits::PLOT);

      double h = plot->lengthPixelHeight(len);

      cmdBase_->setCmdRc(h);
    }
    else if (name == "pixel_position") {
      CQChartsPosition pos(data, CQChartsUnits::PLOT);

      QPointF p = plot->positionToPixel(pos);

      cmdBase_->setCmdRc(p);
    }
    else if (name == "properties") {
      QStringList names;

      plot->getPropertyNames(names, hidden);

      cmdBase_->setCmdRc(names);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "model" << "view" << "value" << "map" << "annotations" << "objects" <<
       "selected_objects" << "inds" << "plot_width" << "plot_height" << "pixel_width" <<
       "pixel_height" << "pixel_position" << "properties";

      cmdBase_->setCmdRc(names);
    }
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
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
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
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
    else if (name == "types") {
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

      if (! modelData) {
        charts_->errorMsg("No model data");
        return false;
      }

      cmdBase_->setCmdRc(modelData->ind());
    }
    else if (name == "annotation_types") {
      QStringList names = CQChartsAnnotation::typeNames();

      cmdBase_->setCmdRc(names);
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "models" << "views" << "types" << "plots" << "current_model" << "annotation_types";

      cmdBase_->setCmdRc(names);
    }
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
  }

  return true;
}

//------

// set charts data
bool
CQChartsCmds::
setChartsDataCmd(CQChartsCmdArgs &argv)
{
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
  argv.addCmdArg("-name"  , CQChartsCmdArg::Type::String , "data name");
  argv.addCmdArg("-value" , CQChartsCmdArg::Type::String , "data value");

  if (! argv.parse())
    return false;

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

    if (role < 0) {
      charts_->errorMsg("Invalid role");
      return false;
    }
  }

  //---

  if      (argv.hasParseArg("model")) {
    int modelInd = argv.getParseInt("model", -1);

    // get model
    CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

    if (! modelData) {
      charts_->errorMsg("No model data");
      return false;
    }

    ModelP model = modelData->currentModel();

    //---

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    CQChartsRow row = argv.getParseRow("row");

    // set column header or row, column value
    if      (name == "value") {
      if (header) {
        if (! column.isValid()) {
          cmdBase_->setCmdError("Invalid header column specified");
          return false;
        }

        if (! CQChartsModelUtil::setModelHeaderValue(model.data(), column, value, role))
          charts_->errorMsg("Failed to set header value");
      }
      else {
        QModelIndex ind = model.data()->index(row.row(), column.column());

        if (! ind.isValid()) {
          cmdBase_->setCmdError(QString("Invalid data row/column specified '%1,%2'").
            arg(row.row()).arg(column.column()));
          return false;
        }

        if (! CQChartsModelUtil::setModelValue(model.data(), row.row(), column, value, role))
          charts_->errorMsg("Failed to set row value");
      }
    }
    else if (name == "column_type") {
      if (column.isValid())
        CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), column, value);
      else
        CQChartsModelUtil::setColumnTypeStrs(charts_, model.data(), value);
    }
    else if (name == "name") {
      charts_->setModelName(modelData, value);
    }
    else if (name == "process") {
      CQChartsModelUtil::processExpression(model.data(), value);
    }
    // model property
    else if (name.left(9) == "property.") {
      QString name1 = name.mid(9);

      if (! CQUtil::setProperty(model.data(), name1, value)) {
        charts_->errorMsg("Failed to set model property '" + name1 + "'");
        return false;
      }
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "value" << "column_type" << "name" << "process" << "property.<name>";

      cmdBase_->setCmdRc(names);
    }
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
  }
  else if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);
    if (! view) return false;

    if      (name == "fit") {
      view->fitSlot();
    }
    else if (name == "?") {
      QStringList names = QStringList() <<
       "fit";

      cmdBase_->setCmdRc(names);
    }
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
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
    else if (name == "?") {
      QStringList names = QStringList() <<
       "updates_enabled";

      cmdBase_->setCmdRc(names);
    }
    else {
      cmdBase_->setCmdError("Invalid name '" + name + "' specified");
      return false;
    }
  }
  else {
    cmdBase_->setCmdError("Invalid name '" + name + "' specified");
    return false;
  }

  return true;
}

//------

bool
CQChartsCmds::
createChartsRectAnnotationCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsRectAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-start", CQChartsCmdArg::Type::Position, "start");
  argv.addCmdArg("-end"  , CQChartsCmdArg::Type::Position, "end"  );
  argv.addCmdArg("-rect" , CQChartsCmdArg::Type::Rect    , "rect" );

  argv.addCmdArg("-margin" , CQChartsCmdArg::Type::Real, "margin");
  argv.addCmdArg("-padding", CQChartsCmdArg::Type::Real, "padding");

  argv.addCmdArg("-background"        , CQChartsCmdArg::Type::SBool , "background visible");
  argv.addCmdArg("-background_color"  , CQChartsCmdArg::Type::Color , "background color");
  argv.addCmdArg("-background_alpha"  , CQChartsCmdArg::Type::Real  , "background alpha");
//argv.addCmdArg("-background_pattern", CQChartsCmdArg::Type::String, "background pattern");

  argv.addCmdArg("-border"      , CQChartsCmdArg::Type::SBool   , "border visible");
  argv.addCmdArg("-border_color", CQChartsCmdArg::Type::Color   , "border color");
  argv.addCmdArg("-border_alpha", CQChartsCmdArg::Type::Real    , "border alpha");
  argv.addCmdArg("-border_width", CQChartsCmdArg::Type::Length  , "border width");
  argv.addCmdArg("-border_dash" , CQChartsCmdArg::Type::LineDash, "border dash");

  argv.addCmdArg("-corner_size" , CQChartsCmdArg::Type::Length, "corner size");
  argv.addCmdArg("-border_sides", CQChartsCmdArg::Type::Sides , "border sides");

  if (! argv.parse())
    return false;

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

  CQChartsFillData   &background = boxData.shape().background();
  CQChartsStrokeData &border     = boxData.shape().border();

  border.setVisible(true);

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  boxData.setMargin (argv.getParseReal("margin" , boxData.margin()));
  boxData.setPadding(argv.getParseReal("padding", boxData.padding()));

  background.setVisible(argv.getParseBool ("background"        , background.isVisible()));
  background.setColor  (argv.getParseColor("background_color"  , background.color    ()));
  background.setAlpha  (argv.getParseReal ("background_alpha"  , background.alpha    ()));
//background.setPattern(argv.getParseStr  ("background_pattern", background.pattern  ()));

  border.setVisible   (argv.getParseBool    ("border"      , border.isVisible()));
  border.setColor     (argv.getParseColor   ("border_color", border.color    ()));
  border.setAlpha     (argv.getParseReal    ("border_alpha", border.alpha    ()));
  border.setWidth     (argv.getParseLength  (view, plot, "border_width", border.width()));
  border.setDash      (argv.getParseLineDash("border_dash" , border.dash     ()));
  border.setCornerSize(argv.getParseLength  (view, plot, "corner_size", border.cornerSize()));

  boxData.setBorderSides(argv.getParseSides("border_sides", boxData.borderSides()));

  //---

  CQChartsRectAnnotation *annotation = nullptr;

  if      (argv.hasParseArg("start") || argv.hasParseArg("end")) {
    CQChartsPosition start = argv.getParsePosition(view, plot, "start");
    CQChartsPosition end   = argv.getParsePosition(view, plot, "end"  );

    CQChartsRect rect;

    if      (view)
      annotation = view->addRectAnnotation(rect);
    else if (plot)
      annotation = plot->addRectAnnotation(rect);

    if (annotation)
      annotation->setRect(start, end);
  }
  else if (argv.hasParseArg("rect")) {
    CQChartsRect rect = argv.getParseRect(view, plot, "rect");

    if      (view)
      annotation = view->addRectAnnotation(rect);
    else if (plot)
      annotation = plot->addRectAnnotation(rect);
  }
  else {
    CQChartsRect rect;

    if      (view)
      annotation = view->addRectAnnotation(rect);
    else if (plot)
      annotation = plot->addRectAnnotation(rect);

    if (annotation)
      annotation->setRect(CQChartsPosition(QPointF(0, 0)), CQChartsPosition(QPointF(1, 1)));
  }

  if (! annotation) {
    charts_->errorMsg("Failed to create annotation");
    return false;
  }

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsEllipseAnnotationCmd(CQChartsCmdArgs &argv)
{
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

  argv.addCmdArg("-background"        , CQChartsCmdArg::Type::SBool , "background visible");
  argv.addCmdArg("-background_color"  , CQChartsCmdArg::Type::Color , "background color");
  argv.addCmdArg("-background_alpha"  , CQChartsCmdArg::Type::Real  , "background alpha");
//argv.addCmdArg("-background_pattern", CQChartsCmdArg::Type::String, "background pattern");

  argv.addCmdArg("-border"      , CQChartsCmdArg::Type::SBool   , "border visible");
  argv.addCmdArg("-border_color", CQChartsCmdArg::Type::Color   , "border color");
  argv.addCmdArg("-border_alpha", CQChartsCmdArg::Type::Real    , "border alpha");
  argv.addCmdArg("-border_width", CQChartsCmdArg::Type::Length  , "border width");
  argv.addCmdArg("-border_dash" , CQChartsCmdArg::Type::LineDash, "border dash");

  argv.addCmdArg("-corner_size" , CQChartsCmdArg::Type::Length, "corner size");
  argv.addCmdArg("-border_sides", CQChartsCmdArg::Type::Sides , "border sides");

  if (! argv.parse())
    return false;

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

  CQChartsFillData   &background = boxData.shape().background();
  CQChartsStrokeData &border     = boxData.shape().border();

  border.setVisible(true);

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition center = argv.getParsePosition(view, plot, "center");

  CQChartsLength rx = argv.getParseLength(view, plot, "rx");
  CQChartsLength ry = argv.getParseLength(view, plot, "ry");

  background.setVisible(argv.getParseBool ("background"        , background.isVisible()));
  background.setColor  (argv.getParseColor("background_color"  , background.color    ()));
  background.setAlpha  (argv.getParseReal ("background_alpha"  , background.alpha    ()));
//background.setPattern(argv.getParseStr  ("background_pattern", background.pattern  ()));

  border.setVisible   (argv.getParseBool    ("border"      , border.isVisible()));
  border.setColor     (argv.getParseColor   ("border_color", border.color    ()));
  border.setAlpha     (argv.getParseReal    ("border_alpha", border.alpha    ()));
  border.setWidth     (argv.getParseLength  (view, plot, "border_width", border.width()));
  border.setDash      (argv.getParseLineDash("border_dash" , border.dash     ()));
  border.setCornerSize(argv.getParseLength  (view, plot, "corner_size", border.cornerSize()));

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

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsPolygonAnnotationCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsPolygonAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-points", CQChartsCmdArg::Type::Polygon, "points string").setRequired();

  argv.addCmdArg("-background"        , CQChartsCmdArg::Type::SBool , "background visible");
  argv.addCmdArg("-background_color"  , CQChartsCmdArg::Type::Color , "background color");
  argv.addCmdArg("-background_alpha"  , CQChartsCmdArg::Type::Real  , "background alpha");
//argv.addCmdArg("-background_pattern", CQChartsCmdArg::Type::String, "background pattern");

  argv.addCmdArg("-border"      , CQChartsCmdArg::Type::SBool   , "border visible");
  argv.addCmdArg("-border_color", CQChartsCmdArg::Type::Color   , "border color");
  argv.addCmdArg("-border_alpha", CQChartsCmdArg::Type::Real    , "border alpha");
  argv.addCmdArg("-border_width", CQChartsCmdArg::Type::Length  , "border width");
  argv.addCmdArg("-border_dash" , CQChartsCmdArg::Type::LineDash, "border dash");

  if (! argv.parse())
    return false;

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

  CQChartsFillData   &background = shapeData.background();
  CQChartsStrokeData &border     = shapeData.border();

  border.setVisible(true);

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  QPolygonF points = argv.getParsePoly("points");

  background.setVisible(argv.getParseBool ("background"        , background.isVisible()));
  background.setColor  (argv.getParseColor("background_color"  , background.color    ()));
  background.setAlpha  (argv.getParseReal ("background_alpha"  , background.alpha    ()));
//background.setPattern(argv.getParseStr  ("background_pattern", background.pattern  ()));

  border.setVisible(argv.getParseBool    ("border"      , border.isVisible()));
  border.setColor  (argv.getParseColor   ("border_color", border.color    ()));
  border.setAlpha  (argv.getParseReal    ("border_alpha", border.alpha    ()));
  border.setWidth  (argv.getParseLength  (view, plot, "border_width", border.width()));
  border.setDash   (argv.getParseLineDash("border_dash" , border.dash     ()));

  //---

  if (! points.length()) {
    cmdBase_->setCmdError("No points");
    return false;
  }

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

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsPolylineAnnotationCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsPolylineAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-points", CQChartsCmdArg::Type::Polygon, "points string").setRequired();

  argv.addCmdArg("-background"        , CQChartsCmdArg::Type::SBool , "background visible");
  argv.addCmdArg("-background_color"  , CQChartsCmdArg::Type::Color , "background color");
  argv.addCmdArg("-background_alpha"  , CQChartsCmdArg::Type::Real  , "background alpha");
//argv.addCmdArg("-background_pattern", CQChartsCmdArg::Type::String, "background pattern");

  argv.addCmdArg("-border"      , CQChartsCmdArg::Type::SBool   , "border visible");
  argv.addCmdArg("-border_color", CQChartsCmdArg::Type::Color   , "border color");
  argv.addCmdArg("-border_alpha", CQChartsCmdArg::Type::Real    , "border alpha");
  argv.addCmdArg("-border_width", CQChartsCmdArg::Type::Length  , "border width");
  argv.addCmdArg("-border_dash" , CQChartsCmdArg::Type::LineDash, "border dash");

  if (! argv.parse())
    return false;

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

  CQChartsFillData   &background = shapeData.background();
  CQChartsStrokeData &border     = shapeData.border();

  border.setVisible(true);

  QString viewName = argv.getParseStr("view");
  QString plotName = argv.getParseStr("plot");
  QString id       = argv.getParseStr("id");
  QString tipId    = argv.getParseStr("tip");

  QPolygonF points = argv.getParsePoly("points");

  background.setVisible(argv.getParseBool ("background"        , background.isVisible()));
  background.setColor  (argv.getParseColor("background_color"  , background.color    ()));
  background.setAlpha  (argv.getParseReal ("background_alpha"  , background.alpha    ()));
//background.setPattern(argv.getParseStr  ("background_pattern", background.pattern  ()));

  border.setVisible(argv.getParseBool    ("border"      , border.isVisible()));
  border.setColor  (argv.getParseColor   ("border_color", border.color    ()));
  border.setAlpha  (argv.getParseReal    ("border_alpha", border.alpha    ()));
  border.setWidth  (argv.getParseLength  (view, plot, "border_width", border.width()));
  border.setDash   (argv.getParseLineDash("border_dash" , border.dash     ()));

  //---

  if (! points.length()) {
    cmdBase_->setCmdError("No points");
    return false;
  }

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

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsTextAnnotationCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsTextAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-position", CQChartsCmdArg::Type::Position, "position");
  argv.addCmdArg("-rect"    , CQChartsCmdArg::Type::Rect    , "rect");

  argv.addCmdArg("-text", CQChartsCmdArg::Type::String, "text");

  argv.addCmdArg("-font"    , CQChartsCmdArg::Type::String , "font");
  argv.addCmdArg("-color"   , CQChartsCmdArg::Type::Color  , "color");
  argv.addCmdArg("-alpha"   , CQChartsCmdArg::Type::Real   , "alpha");
  argv.addCmdArg("-angle"   , CQChartsCmdArg::Type::Real   , "angle");
  argv.addCmdArg("-contrast", CQChartsCmdArg::Type::SBool  , "contrast");
  argv.addCmdArg("-align"   , CQChartsCmdArg::Type::Align  , "align string");
  argv.addCmdArg("-html"    , CQChartsCmdArg::Type::Boolean, "html text");

  argv.addCmdArg("-background"        , CQChartsCmdArg::Type::SBool , "background visible");
  argv.addCmdArg("-background_color"  , CQChartsCmdArg::Type::Color , "background color");
  argv.addCmdArg("-background_alpha"  , CQChartsCmdArg::Type::Real  , "background alpha");
//argv.addCmdArg("-background_pattern", CQChartsCmdArg::Type::String, "background pattern");

  argv.addCmdArg("-border"      , CQChartsCmdArg::Type::SBool   , "border visible");
  argv.addCmdArg("-border_color", CQChartsCmdArg::Type::Color   , "border color");
  argv.addCmdArg("-border_alpha", CQChartsCmdArg::Type::Real    , "border alpha");
  argv.addCmdArg("-border_width", CQChartsCmdArg::Type::Length  , "border width");
  argv.addCmdArg("-border_dash" , CQChartsCmdArg::Type::LineDash, "border dash");

  argv.addCmdArg("-corner_size" , CQChartsCmdArg::Type::Length, "corner size");
  argv.addCmdArg("-border_sides", CQChartsCmdArg::Type::Sides , "border sides");

  if (! argv.parse())
    return false;

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

  CQChartsFillData   &background = boxData.shape().background();
  CQChartsStrokeData &border     = boxData.shape().border();

  background.setVisible(false);
  border    .setVisible(false);

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

  background.setVisible(argv.getParseBool ("background"        , background.isVisible()));
  background.setColor  (argv.getParseColor("background_color"  , background.color    ()));
  background.setAlpha  (argv.getParseReal ("background_alpha"  , background.alpha    ()));
//background.setPattern(argv.getParseStr  ("background_pattern", background.pattern  ()));

  border.setVisible   (argv.getParseBool    ("border"      , border.isVisible()));
  border.setColor     (argv.getParseColor   ("border_color", border.color    ()));
  border.setAlpha     (argv.getParseReal    ("border_alpha", border.alpha    ()));
  border.setWidth     (argv.getParseLength  (view, plot, "border_width", border.width()));
  border.setDash      (argv.getParseLineDash("border_dash" , border.dash     ()));
  border.setCornerSize(argv.getParseLength  (view, plot, "corner_size", border.cornerSize()));

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
  else if (argv.hasParseArg("rect")) {
    CQChartsRect rect = argv.getParseRect(view, plot, "rect");

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

  if (! annotation) {
    charts_->errorMsg("Failed to create annotation");
    return false;
  }

  if (id != "")
    annotation->setId(id);

  if (tipId != "")
    annotation->setTipId(tipId);

  annotation->setTextData(textData);
  annotation->setBoxData(boxData);

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsArrowAnnotationCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsArrowAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-start", CQChartsCmdArg::Type::Position, "start position");
  argv.addCmdArg("-end"  , CQChartsCmdArg::Type::Position, "end position");

  argv.addCmdArg("-length"      , CQChartsCmdArg::Type::Length, "line length");
  argv.addCmdArg("-angle"       , CQChartsCmdArg::Type::Real  , "line angle");
  argv.addCmdArg("-back_angle"  , CQChartsCmdArg::Type::Real  , "arrow back angle");
  argv.addCmdArg("-fhead"       , CQChartsCmdArg::Type::SBool , "show start arrow");
  argv.addCmdArg("-thead"       , CQChartsCmdArg::Type::SBool , "show end arrow");
  argv.addCmdArg("-line_ends"   , CQChartsCmdArg::Type::SBool , "line ends");
  argv.addCmdArg("-line_width"  , CQChartsCmdArg::Type::Length, "line width");
  argv.addCmdArg("-border_color", CQChartsCmdArg::Type::Color , "border color");
  argv.addCmdArg("-border_width", CQChartsCmdArg::Type::Length, "border width");
  argv.addCmdArg("-filled"      , CQChartsCmdArg::Type::SBool , "is filled");
  argv.addCmdArg("-fill_color"  , CQChartsCmdArg::Type::Color , "fill color");

  if (! argv.parse())
    return false;

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

  arrowData.setLength   (argv.getParseLength(view, plot, "length", arrowData.length()));
  arrowData.setAngle    (argv.getParseReal  ("angle"     , arrowData.angle()));
  arrowData.setBackAngle(argv.getParseReal  ("back_angle", arrowData.backAngle()));
  arrowData.setFHead    (argv.getParseBool  ("fhead"     , arrowData.isFHead()));
  arrowData.setTHead    (argv.getParseBool  ("thead"     , arrowData.isTHead()));
  arrowData.setLineEnds (argv.getParseBool  ("line_ends" , arrowData.isLineEnds()));
  arrowData.setLineWidth(argv.getParseLength(view, plot, "line_width", arrowData.lineWidth()));

  CQChartsShapeData shapeData;

  CQChartsStrokeData &stroke = shapeData.border();
  CQChartsFillData   &fill   = shapeData.background();

  stroke.setWidth(argv.getParseLength(view, plot, "border_width", stroke.width()));
  stroke.setColor(argv.getParseColor("border_color", stroke.color()));

  fill.setVisible(argv.getParseBool ("filled"    , fill.isVisible()));
  fill.setColor  (argv.getParseColor("fill_color", fill.color()));

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

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
createChartsPointAnnotationCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::createChartsPointAnnotationCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-position", CQChartsCmdArg::Type::Position, "position");
  argv.addCmdArg("-type"    , CQChartsCmdArg::Type::String  , "symbol type");

  argv.addCmdArg("-size", CQChartsCmdArg::Type::Length, "symbol size");

  argv.addCmdArg("-stroked"   , CQChartsCmdArg::Type::SBool , "stroke visible");
  argv.addCmdArg("-line_width", CQChartsCmdArg::Type::Length, "stroke width");
  argv.addCmdArg("-line_color", CQChartsCmdArg::Type::Color , "stroke color");
  argv.addCmdArg("-line_alpha", CQChartsCmdArg::Type::Real  , "stroke alpha");

  argv.addCmdArg("-filled"    , CQChartsCmdArg::Type::SBool, "fill visible");
  argv.addCmdArg("-fill_color", CQChartsCmdArg::Type::Color, "fill color");
  argv.addCmdArg("-fill_alpha", CQChartsCmdArg::Type::Real , "fill alpha");

  if (! argv.parse())
    return false;

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

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition pos = argv.getParsePosition(view, plot, "position");

  QString typeStr = argv.getParseStr("type");

  if (typeStr.length())
    symbolData.setType(CQChartsSymbol::nameToType(typeStr));

  symbolData.setSize(argv.getParseLength(view, plot, "size", symbolData.size()));

  CQChartsStrokeData &strokeData = symbolData.stroke();

  strokeData.setVisible(argv.getParseBool  ("stroked", strokeData.isVisible()));
  strokeData.setWidth  (argv.getParseLength(view, plot, "line_width", strokeData.width()));
  strokeData.setColor  (argv.getParseColor ("line_color", strokeData.color()));
  strokeData.setAlpha  (argv.getParseReal  ("line_alpha", strokeData.alpha()));

  CQChartsFillData &fillData = symbolData.fill();

  fillData.setVisible(argv.getParseBool ("filled"    , fillData.isVisible()));
  fillData.setColor  (argv.getParseColor("fill_color", fillData.color    ()));
  fillData.setAlpha  (argv.getParseReal ("fill_alpha", fillData.alpha    ()));

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

  cmdBase_->setCmdRc(annotation->pathId());

  return true;
}

//------

bool
CQChartsCmds::
removeChartsAnnotationCmd(CQChartsCmdArgs &argv)
{
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

  if (! argv.parse())
    return false;

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
      charts_->errorMsg("-view or -plot needed for -all");
  }

  return true;
}

//------

bool
CQChartsCmds::
connectChartsSignalCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::connectChartsSignalCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-from", CQChartsCmdArg::Type::String, "from connection name");
  argv.addCmdArg("-to"  , CQChartsCmdArg::Type::String, "to procedure name");

  if (! argv.parse())
    return false;

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
    else {
      charts_->errorMsg("unknown slot");
      return false;
    }
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
  else {
    charts_->errorMsg("unknown slot");
    return false;
  }

  return true;
}

//------

bool
CQChartsCmds::
printChartsImageCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmds::printChartsImageCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-file", CQChartsCmdArg::Type::String, "filename").setRequired();

  argv.addCmdArg("-layer", CQChartsCmdArg::Type::String, "layer name");

  if (! argv.parse())
    return false;

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
        charts_->errorMsg("Failed to print layer");
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
  CQPerfTrace trace("CQChartsCmds::writeChartsDataCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-type", CQChartsCmdArg::Type::String, "type");
  argv.addCmdArg("-file", CQChartsCmdArg::Type::String, "filename");

  if (! argv.parse())
    return false;

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

    if (fos.fail()) {
      charts_->errorMsg("Failed to open '" + filename + "'");
      return false;
    }

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
    else {
      charts_->errorMsg("Invalid write type");
    }
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
      charts_->errorMsg("Invalid write type");
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

  if (! argv.parse())
    return false;

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

  if (! argv.parse())
    return false;

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
  CQPerfTrace trace("CQChartsCmds::showChartsCreatePlotDlgCmd");

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model_ind" );
  argv.addCmdArg("-view" , CQChartsCmdArg::Type::String , "view name" );
  argv.addCmdArg("-modal", CQChartsCmdArg::Type::Boolean, "show modal");

  if (! argv.parse())
    return false;

  bool modal = argv.getParseBool("modal");

  //---

  int     modelInd = argv.getParseInt("model", -1);
  QString viewName = argv.getParseStr("view");

  //---

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return false;
  }

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
  CQPerfTrace trace("CQChartsCmds::testEditCmd");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-type", CQChartsCmdArg::Type::String, "type").setMultiple(true);

  argv.addCmdArg("-editable", CQChartsCmdArg::Type::SBool, "editable");

  if (! argv.parse())
    return false;

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
    else if (type == "rect") {
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
    else {
      charts_->errorMsg("Bad edit type '" + type + "'");
      return false;
    }
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
createPlot(CQChartsView *view, const ModelP &model, QItemSelectionModel *sm,
           CQChartsPlotType *type, bool reuse)
{
  if (! view)
    view = getView(reuse);

  //---

  CQChartsPlot *plot = type->create(view, model);

  if (sm)
    plot->setSelectionModel(sm);

  //---

  return plot;
}

bool
CQChartsCmds::
initPlot(CQChartsPlot *plot, const CQChartsNameValueData &nameValueData,
         const CQChartsGeom::BBox &bbox)
{
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

    if (! found) {
      charts_->errorMsg("Illegal column name '" + nameValue.first + "'");
      return false;
    }
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

    if (! found) {
      charts_->errorMsg("Illegal parameter name '" + nameValue.first + "'");
      return false;
    }
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
        charts_->errorMsg("Bad column name '" + (*p).second + "'");
        continue;
      }

      QString scol = column.toString();

      if (! plot->setParameter(parameter, scol)) {
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
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
        charts_->errorMsg("Bad columns name '" + str + "'");
        continue;
      }

      QString s = CQChartsColumn::columnsToString(columns);

      if (! plot->setParameter(parameter, QVariant(s))) {
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
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
          charts_->errorMsg("Invalid real value '" + value + "' for '" +
                            parameter->typeName() + "'");
          continue;
        }

        var = QVariant(r);
      }
      else if (parameter->type() == CQChartsPlotParameter::Type::INTEGER) {
        bool ok;

        int i = CQChartsUtil::toInt(value.simplified(), ok);

        if (! ok) {
          charts_->errorMsg("Invalid integer value '" + value + "' for '" +
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
          charts_->errorMsg("Invalid boolean value '" + value + "' for '" +
                            parameter->typeName() + "'");
          continue;
        }

        var = QVariant(b);
      }
      else {
        continue;
      }

      if (! plot->setParameter(parameter, var)) {
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
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

  charts_->setModelName(modelData, filename);

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
