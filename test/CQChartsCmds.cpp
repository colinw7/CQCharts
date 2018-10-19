#include <CQChartsCmds.h>
#include <CQChartsCmdsArgs.h>
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
#include <CQChartsPaletteColorData.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQChartsValueSet.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsArrow.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>

#include <CQChartsLoadDlg.h>
#include <CQChartsModelDlg.h>
#include <CQChartsPlotDlg.h>
#include <CQChartsDataFilterModel.h>
#include <CQChartsAnalyzeModel.h>

#include <CQDataModel.h>
#include <CQSortModel.h>
#include <CQFoldedModel.h>
#include <CQSubSetModel.h>
#include <CQTransposeModel.h>

#include <CQUtil.h>
#include <CUnixFile.h>
#include <CHRTimer.h>

#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>
#endif

#include <QApplication>
#include <QStackedWidget>
#include <QSortFilterProxyModel>
#include <QFont>
#include <fstream>

//----

namespace {

bool stringToBool(const QString &str, bool *ok) {
  QString lstr = str.toLower();

  if (lstr == "0" || lstr == "false" || lstr == "no") {
    *ok = true;
    return false;
  }

  if (lstr == "1" || lstr == "true" || lstr == "yes") {
    *ok = true;
    return true;
  }

  *ok = false;

  return false;
}

}

#ifdef CQCharts_USE_TCL
class CQChartsTclCmd {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsTclCmd(CQChartsCmds *cmds, const QString &name) :
   cmds_(cmds), name_(name) {
    cmdId_ = cmds_->qtcl()->createObjCommand(name_,
               (CQTcl::ObjCmdProc) &CQChartsTclCmd::commandProc, (CQTcl::ObjCmdData) this);
  }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    CQChartsTclCmd *command = (CQChartsTclCmd *) clientData;

    Vars vars;

    for (int i = 1; i < objc; ++i) {
      Tcl_Obj *obj = const_cast<Tcl_Obj *>(objv[i]);

      vars.push_back(command->cmds_->qtcl()->variantFromObj(obj));
    }

    if (! command->cmds_->processCmd(command->name_, vars))
      return TCL_ERROR;

    return TCL_OK;
  }

 private:
  CQChartsCmds *cmds_  { nullptr };
  QString       name_;
  Tcl_Command   cmdId_ { nullptr };
};
#endif

//----

CQChartsCmds::
CQChartsCmds(CQCharts *charts) :
 charts_(charts)
{
#ifdef CQCharts_USE_TCL
  qtcl_ = new CQTcl();
#endif

  addCommands();
}

CQChartsCmds::
~CQChartsCmds()
{
#ifdef CQCharts_USE_TCL
  delete qtcl_;
#endif
}

void
CQChartsCmds::
addCommands()
{
  static bool cmdsAdded;

  if (! cmdsAdded) {
    addCommand("help");

    // load, process, sort, filter model
    addCommand("load_model"            );
    addCommand("process_model"         );
    addCommand("add_process_model_proc");
    addCommand("sort_model"            );
    addCommand("fold_model"            );
    addCommand("filter_model"          );
    addCommand("flatten_model"         );

    // correlation, subset
    addCommand("correlation_model");
    addCommand("subset_model"     );
    addCommand("transpose_model"  );

    // export
    addCommand("export_model");

    // measure text
    addCommand("measure_charts_text");

    // add view
    addCommand("create_view");

    // add/remove plot
    addCommand("create_plot");
    addCommand("remove_plot");

    // group/place plots
    addCommand("group_plots");
    addCommand("place_plots");

    // get/set charts property
    addCommand("get_charts_property");
    addCommand("set_charts_property");

    // get/set charts model data
    addCommand("get_charts_data");
    addCommand("set_charts_data");

    // annotations
    addCommand("create_text_shape"    );
    addCommand("create_arrow_shape"   );
    addCommand("create_rect_shape"    );
    addCommand("create_ellipse_shape" );
    addCommand("create_polygon_shape" );
    addCommand("create_polyline_shape");
    addCommand("create_point_shape"   );
    addCommand("remove_shape"         );

    // theme/palette
    addCommand("get_palette");
    addCommand("set_palette");

    // connect
    addCommand("connect_chart");

    // print
    addCommand("print_chart");

    // dialogs
    addCommand("load_model_dlg"  );
    addCommand("manage_model_dlg");
    addCommand("create_plot_dlg" );

    addCommand("sync_qt" );

#ifdef CQCharts_USE_TCL
    qtcl()->createAlias("echo", "puts");
#endif

    addCommand("sh");

    //---

    cmdsAdded = true;
  }
}

void
CQChartsCmds::
addCommand(const QString &name)
{
#ifdef CQCharts_USE_TCL
  new CQChartsTclCmd(this, name);
#else
  assert(false);
#endif

  commandNames_.push_back(name);
}

bool
CQChartsCmds::
processCmd(const QString &cmd, const Vars &vars)
{
  if (cmd == "help") {
    for (auto &name : commandNames_)
      std::cout << name.toStdString() << "\n";

    return true;
  }

  // load, process, sort, filter model
  if      (cmd == "load_model"            ) { loadModelCmd     (vars); }
  else if (cmd == "process_model"         ) { processModelCmd  (vars); }
  else if (cmd == "add_process_model_proc") { addProcessModelProcCmd(vars); }
  else if (cmd == "sort_model"            ) { sortModelCmd     (vars); }
  else if (cmd == "fold_model"            ) { foldModelCmd     (vars); }
  else if (cmd == "filter_model"          ) { filterModelCmd   (vars); }
  else if (cmd == "flatten_model"         ) { flattenModelCmd  (vars); }

  // correlation, subset
  else if (cmd == "correlation_model") { correlationModelCmd(vars); }
  else if (cmd == "subset_model"     ) { subsetModelCmd     (vars); }
  else if (cmd == "transpose_model"  ) { transposeModelCmd  (vars); }

  // export
  else if (cmd == "export_model") { exportModelCmd(vars); }

  // measure text
  else if (cmd == "measure_charts_text") { measureChartsTextCmd(vars); }

  // create view
  else if (cmd == "create_view") { createViewCmd(vars); }

  // create/remove plot
  else if (cmd == "create_plot") { createPlotCmd(vars); }
  else if (cmd == "remove_plot") { removePlotCmd(vars); }

  // group/place plots
  else if (cmd == "group_plots") { groupPlotsCmd(vars); }
  else if (cmd == "place_plots") { placePlotsCmd(vars); }

  // get/set property
  else if (cmd == "get_charts_property") { getChartsPropertyCmd(vars); }
  else if (cmd == "set_charts_property") { setChartsPropertyCmd(vars); }

  // get/set data
  else if (cmd == "get_charts_data") { getChartsDataCmd(vars); }
  else if (cmd == "set_charts_data") { setChartsDataCmd(vars); }

  // annotations
  else if (cmd == "create_text_shape"    ) { createTextShapeCmd    (vars); }
  else if (cmd == "create_arrow_shape"   ) { createArrowShapeCmd   (vars); }
  else if (cmd == "create_rect_shape"    ) { createRectShapeCmd    (vars); }
  else if (cmd == "create_ellipse_shape" ) { createEllipseShapeCmd (vars); }
  else if (cmd == "create_polygon_shape" ) { createPolygonShapeCmd (vars); }
  else if (cmd == "create_polyline_shape") { createPolylineShapeCmd(vars); }
  else if (cmd == "create_point_shape"   ) { createPointShapeCmd   (vars); }
  else if (cmd == "remove_shape"         ) { removeShapeCmd        (vars); }

  // palette (interface/theme)
  else if (cmd == "get_palette") { getPaletteCmd(vars); }
  else if (cmd == "set_palette") { setPaletteCmd(vars); }

  // connect
  else if (cmd == "connect_chart") { connectChartCmd(vars); }

  // print
  else if (cmd == "print_chart") { printChartCmd(vars); }

  // dialogs
  else if (cmd == "load_model_dlg"  ) { loadModelDlgCmd  (vars); }
  else if (cmd == "manage_model_dlg") { manageModelDlgCmd(vars); }
  else if (cmd == "create_plot_dlg" ) { createPlotDlgCmd (vars); }

  else if (cmd == "sync_qt" ) { syncQtCmd(vars); }

  else if (cmd == "sh") { shellCmd(vars); }

  else if (cmd == "exit") { exit(0); }

  else return false;

  return true;
}

//------

// load model from data
bool
CQChartsCmds::
loadModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("load_model", vars);

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
  argv.addCmdArg("-transpose"          , CQChartsCmdArg::Type::Boolean);

  argv.addCmdArg("-num_rows"   , CQChartsCmdArg::Type::Integer, "number of rows");
  argv.addCmdArg("-filter"     , CQChartsCmdArg::Type::String , "filter expression");
  argv.addCmdArg("-column_type", CQChartsCmdArg::Type::String , "column type");
  argv.addCmdArg("-name"       , CQChartsCmdArg::Type::String , "name");

  argv.addCmdArg("filename", CQChartsCmdArg::Type::String, "file name");

  if (! argv.parse())
    return false;

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

  inputData.transpose = argv.getParseBool("transpose");

  inputData.numRows = std::max(argv.getParseInt("num_rows"), 1);

  inputData.filter = argv.getParseStr("filter");

  QString columnTypes = argv.getParseStr("column_type");

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

  if (columnTypes != "") {
    ModelP model = modelData->currentModel();

    CQChartsUtil::setColumnTypeStrs(charts_, model.data(), columnTypes);
  }

  if (name.length())
    modelData->setName(name);

  setCmdRc(modelData->ind());

  return true;
}

//------

void
CQChartsCmds::
processModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("process_model", vars);

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model index").setRequired();
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column ,
                 "column for delete, modify, calc, query");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-add"    , CQChartsCmdArg::Type::Boolean, "add column");
  argv.addCmdArg("-delete" , CQChartsCmdArg::Type::Boolean, "delete column");
  argv.addCmdArg("-modify" , CQChartsCmdArg::Type::Boolean, "modify column values");
  argv.addCmdArg("-calc"   , CQChartsCmdArg::Type::Boolean, "calc column");
  argv.addCmdArg("-query"  , CQChartsCmdArg::Type::Boolean, "query column");
  argv.addCmdArg("-analyze", CQChartsCmdArg::Type::Boolean, "analyze data");
  argv.addCmdArg("-list"   , CQChartsCmdArg::Type::Boolean, "list data");
  argv.endCmdGroup();

  argv.addCmdArg("-header", CQChartsCmdArg::Type::String, "header label for add/modify");
  argv.addCmdArg("-type"  , CQChartsCmdArg::Type::String, "type data for add/modify");
  argv.addCmdArg("-expr"  , CQChartsCmdArg::Type::String, "expression for add/modify/calc/query");

  if (! argv.parse())
    return;

  //---

  int modelInd = argv.getParseInt("model", -1);

  QString header = argv.getParseStr("header");
  QString type   = argv.getParseStr("type");
  QString expr   = argv.getParseStr("expr");

  //---

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    setCmdError("No model data");
    return;
  }

  //---

  // get expr model
  ModelP model = modelData->currentModel();

  CQChartsExprModel *exprModel = CQChartsUtil::getExprModel(model.data());

  if (! exprModel) {
    setCmdError("Expression not supported for model");
    return;
  }

#if 0
  CQDataModel *dataModel = qobject_cast<CQDataModel *>(exprModel->sourceModel());

  if (dataModel)
    dataModel->setReadOnly(false);
#endif

  //---

  // add new column (values from result of expression)
  if      (argv.getParseBool("add")) {
    if (! argv.hasParseArg("expr")) {
      setCmdError("Missing expression");
      return;
    }

    int column;

    if (! exprModel->addExtraColumn(header, expr, column)) {
      setCmdError("Failed to add column");
      return;
    }

    //---

    if (type.length()) {
      if (! CQChartsUtil::setColumnTypeStr(charts_, model.data(), column, type)) {
        setCmdError(QString("Invalid type '" + type + "' for column '%1'").arg(column));
        return;
      }
    }

    setCmdRc(column);
  }
  // remove column (must be an added one)
  else if (argv.getParseBool("delete")) {
    CQChartsColumn column = argv.getParseColumn("column", model.data());

    if (! exprModel->removeExtraColumn(column.column())) {
      setCmdError("Failed to delete column");
      return;
    }

    setCmdRc(-1);
  }
  // modify column (values from result of expression)
  else if (argv.getParseBool("modify")) {
    if (! argv.hasParseArg("expr")) {
      setCmdError("Missing expression");
      return;
    }

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    if (! exprModel->assignExtraColumn(header, column.column(), expr)) {
      setCmdError("Failed to modify column");
      return;
    }

    //---

    if (type.length()) {
      if (! CQChartsUtil::setColumnTypeStr(charts_, model.data(), column, type)) {
        setCmdError(QString("Invalid type '" + type + "' for column '%1'").
                           arg(column.column()));
        return;
      }
    }

    setCmdRc(column.column());
  }
  // calculate values from result of expression
  else if (argv.getParseBool("calc")) {
    if (! argv.hasParseArg("expr")) {
      setCmdError("Missing expression");
      return;
    }

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    CQChartsExprModel::Values values;

    exprModel->calcColumn(column.column(), expr, values);

    QVariantList vars;

    for (const auto &var : values)
      vars.push_back(var);

    setCmdRc(vars);
  }
  // query rows where result of expression is true
  else if (argv.getParseBool("query")) {
    if (! argv.hasParseArg("expr")) {
      setCmdError("Missing expression");
      return;
    }

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    CQChartsExprModel::Rows rows;

    exprModel->queryColumn(column.column(), expr, rows);

    using QVariantList = QList<QVariant>;

    QVariantList vars;

    for (const auto &row : rows)
      vars.push_back(row);

    setCmdRc(vars);
  }
  else if (argv.getParseBool("analyze")) {
    CQChartsAnalyzeModel analyzeModel(charts_, modelData);

    if (type != "") {
      if (! charts_->isPlotType(type)) {
        charts_->errorMsg("Invalid type '" + type + "'");
        return;
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

    setCmdRc(tvars);
  }
  // list values
  else if (argv.getParseBool("list")) {
    int nr = std::min(exprModel->rowCount(), 10);
    int nc = exprModel->columnCount();

    QModelIndex parent;
    int         role = Qt::DisplayRole;

    for (int r = 0; r < nr; ++r) {
      QStringList strs;

      for (int c = 0; c < nc; ++c) {
        bool ok;

        QVariant var = CQChartsUtil::modelValue(charts_, exprModel, r, c, parent, role, ok);

        QString str = var.toString();

        strs += str;
      }

      QString str = strs.join("\t");

      std::cerr << str.toStdString() << "\n";
    }
  }
  else {
#if 0
    if (! argv.hasParseArg("expr")) {
      setCmdError("Missing expression");
      return;
    }

    CQChartsExprModel::Function function = CQChartsExprModel::Function::EVAL;

    if (expr.simplified().length())
      processExpression(model.data(), expr);
#endif
  }
}

//------

void
CQChartsCmds::
addProcessModelProcCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("add_process_model_proc", vars);

  argv.addCmdArg("name", CQChartsCmdArg::Type::String, "proc name").setRequired();
  argv.addCmdArg("args", CQChartsCmdArg::Type::String, "proc args").setRequired();
  argv.addCmdArg("body", CQChartsCmdArg::Type::String, "proc body").setRequired();

  if (! argv.parse())
    return;

  const Vars &pargs = argv.getParseArgs();

  if (pargs.size() != 3) {
    charts_->errorMsg("Usage: add_process_model_proc <name> <args> <body>");
    return;
  }

  //---

  QString name = pargs[0].toString();
  QString args = pargs[1].toString();
  QString body = pargs[2].toString();

  //---

  charts_->addProc(name, args, body);
}

//------

void
CQChartsCmds::
measureChartsTextCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("measure_charts_text", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String, "value name").setRequired();
  argv.addCmdArg("-data", CQChartsCmdArg::Type::String, "data");

  if (! argv.parse())
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(view, plotName);
    if (! plot) return;
  }

  //---

  QString name = argv.getParseStr("name");
  QString data = argv.getParseStr("data");

  //---

  QFontMetricsF fm(view->font());

  if      (name == "width") {
    if      (plot)
      setCmdRc(plot->pixelToWindowWidth(fm.width(data)));
    else if (view)
      setCmdRc(view->pixelToWindowWidth(fm.width(data)));
  }
  else if (name == "height") {
    if      (plot)
      setCmdRc(plot->pixelToWindowHeight(fm.height()));
    else if (view)
      setCmdRc(view->pixelToWindowHeight(fm.height()));
  }
  else if (name == "ascent") {
    if      (plot)
      setCmdRc(plot->pixelToWindowHeight(fm.height()));
    else if (view)
      setCmdRc(view->pixelToWindowHeight(fm.height()));
  }
  else if (name == "descent") {
    if      (plot)
      setCmdRc(plot->pixelToWindowHeight(fm.descent()));
    else if (view)
      setCmdRc(view->pixelToWindowHeight(fm.descent()));
  }
  else {
    setCmdError(QString("Invalid value name '%1'").arg(name));
    return;
  }
}

//------

void
CQChartsCmds::
createViewCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_view", vars);

  if (! argv.parse())
    return;

  //---

  CQChartsView *view = addView();

  //---

  setCmdRc(view->id());
}

//------

void
CQChartsCmds::
createPlotCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_plot", vars);

  argv.addCmdArg("-view" , CQChartsCmdArg::Type::String , "view_id"  ).setRequired();
  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model_ind").setRequired();
  argv.addCmdArg("-type" , CQChartsCmdArg::Type::String , "type"     ).setRequired();

  argv.addCmdArg("-where"     , CQChartsCmdArg::Type::String, "filter");
  argv.addCmdArg("-columns"   , CQChartsCmdArg::Type::String, "columns");
  argv.addCmdArg("-bool"      , CQChartsCmdArg::Type::String, "name_values");
  argv.addCmdArg("-string"    , CQChartsCmdArg::Type::String, "name_values");
  argv.addCmdArg("-real"      , CQChartsCmdArg::Type::String, "name_values");
  argv.addCmdArg("-int"       , CQChartsCmdArg::Type::String, "name_values");
  argv.addCmdArg("-enum"      , CQChartsCmdArg::Type::String, "name_values");
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
    return;

  //---

  QString     viewName    = argv.getParseStr    ("view");
  int         modelInd    = argv.getParseInt    ("model", -1);
  QString     typeName    = argv.getParseStr    ("type");
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
  if (! view) return;

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
  }

  ModelP model = modelData->currentModel();

  //------

  CQChartsNameValueData nameValueData;

  // parse plot columns
  QString columnsStr = argv.getParseStr("columns");

  if (columnsStr.length()) {
    QStringList strs = stringToColumns(columnsStr);

    for (int j = 0; j < strs.size(); ++j) {
      const QString &nameValue = strs[j];

      auto pos = nameValue.indexOf('=');

      if (pos >= 0) {
        auto name  = nameValue.mid(0, pos).simplified();
        auto value = nameValue.mid(pos + 1).simplified();

        nameValueData.values[name] = value;
      }
      else {
        charts_->errorMsg("Invalid -columns option '" + columnsStr + "'");
      }
    }
  }

  //--

  // plot bool parameters
  QStringList boolStrs = argv.getParseStrs("bool");

  for (int i = 0; i < boolStrs.length(); ++i) {
    const QString &boolStr = boolStrs[i];

    auto pos = boolStr.indexOf('=');

    QString name, value;

    if (pos >= 0) {
      name  = boolStr.mid(0, pos).simplified();
      value = boolStr.mid(pos + 1).simplified();
    }
    else {
      name  = boolStr;
      value = "true";
    }

    bool ok;

    bool b = stringToBool(value, &ok);

    if (ok)
      nameValueData.bools[name] = b;
    else {
      charts_->errorMsg("Invalid -bool option '" + boolStr + "'");
    }
  }

  //--

  // plot string parameters
  QStringList stringStrs = argv.getParseStrs("string");

  for (int i = 0; i < stringStrs.length(); ++i) {
    const QString &stringStr = stringStrs[i];

    auto pos = stringStr.indexOf('=');

    QString name, value;

    if (pos >= 0) {
      name  = stringStr.mid(0, pos).simplified();
      value = stringStr.mid(pos + 1).simplified();
    }
    else {
      name  = stringStr;
      value = "";
    }

    nameValueData.strings[name] = value;
  }

  //--

  // plot real parameters
  QStringList realStrs = argv.getParseStrs("real");

  for (int i = 0; i < realStrs.length(); ++i) {
    const QString &realStr = realStrs[i];

    auto pos = realStr.indexOf('=');

    QString name;
    double  value = 0.0;

    if (pos >= 0) {
      bool ok;

      name  = realStr.mid(0, pos).simplified();
      value = realStr.mid(pos + 1).simplified().toDouble(&ok);
    }
    else {
      name  = realStr;
      value = 0.0;
    }

    nameValueData.reals[name] = value;
  }

  //--

  // plot integer parameters
  QStringList intStrs = argv.getParseStrs("int");

  for (int i = 0; i < intStrs.length(); ++i) {
    const QString &intStr = intStrs[i];

    auto pos = intStr.indexOf('=');

    QString name;
    int     value = 0;

    if (pos >= 0) {
      bool ok;

      name  = intStr.mid(0, pos).simplified();
      value = intStr.mid(pos + 1).simplified().toInt(&ok);
    }
    else {
      name  = intStr;
      value = 0.0;
    }

    nameValueData.ints[name] = value;
  }

  //--

  // plot integer parameters
  QStringList enumStrs = argv.getParseStrs("enum");

  for (int i = 0; i < enumStrs.length(); ++i) {
    const QString &enumStr = enumStrs[i];

    auto pos = enumStr.indexOf('=');

    QString name;
    QString value;

    if (pos >= 0) {
      name  = enumStr.mid(0, pos).simplified();
      value = enumStr.mid(pos + 1).simplified();
    }
    else {
      name  = enumStr;
      value = "";
    }

    nameValueData.enums[name] = value;
  }

  //------

  if (typeName == "") {
    charts_->errorMsg("No type specified for plot");
    return;
  }

  typeName = fixTypeName(typeName);

  // ignore if bad type
  if (! charts_->isPlotType(typeName)) {
    charts_->errorMsg("Invalid type '" + typeName + "' for plot");
    return;
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

      double pxmin = positionStrs[0].toDouble(&ok1);
      double pymin = positionStrs[1].toDouble(&ok2);
      double pxmax = positionStrs[2].toDouble(&ok3);
      double pymax = positionStrs[3].toDouble(&ok4);

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
  CQChartsPlot *plot = createPlot(view, model, modelData->selectionModel(), type,
                                  nameValueData, true, bbox);

  if (! plot) {
    charts_->errorMsg("Failed to create plot");
    return;
  }

  //------

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

  setCmdRc(plot->pathId());
}

//------

void
CQChartsCmds::
removePlotCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("remove_plot", vars);

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view_id").setRequired();

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot_id");
  argv.addCmdArg("-all" , CQChartsCmdArg::Type::Boolean);
  argv.endCmdGroup();

  if (! argv.parse())
    return;

  //---

  QString viewName = argv.getParseStr ("view");
  QString plotName = argv.getParseStr ("plot");
  bool    all      = argv.getParseBool("all");

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return;

  if (all) {
    view->removeAllPlots();
  }
  else {
    CQChartsPlot *plot = getPlotByName(view, plotName);
    if (! plot) return;

    view->removePlot(plot);
  }
}

//------

void
CQChartsCmds::
getChartsPropertyCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("get_charts_property", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view"      , CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot"      , CQChartsCmdArg::Type::String, "plot name");
  argv.addCmdArg("-annotation", CQChartsCmdArg::Type::String, "annotation name");
  argv.endCmdGroup();

  argv.addCmdArg("-object", CQChartsCmdArg::Type::String, "object id");

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String, "property name");

  if (! argv.parse())
    return;

  //---

  QString objectId = argv.getParseStr("object");

  QString name = argv.getParseStr("name");

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);

    if (! view) {
      charts_->errorMsg("Invalid view '" + viewName + "'");
      return;
    }

    if (name == "?") {
      QStringList names;

      view->getPropertyNames(names);

      setCmdRc(names);
    }
    else {
      QVariant value;

      if (! view->getProperty(name, value)) {
        charts_->errorMsg("Failed to get view parameter '" + name + "'");
        return;
      }

      setCmdRc(value);
    }
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsPlot *plot = getPlotByName(nullptr, plotName);

    if (! plot) {
      charts_->errorMsg("Invalid plot '" + plotName + "'");
      return;
    }

    CQChartsPlotObj *plotObj = nullptr;

    if (objectId.length()) {
      plotObj = plot->getObject(objectId);

      if (! plotObj) {
        charts_->errorMsg("Invalid plot object id '" + objectId + "'");
        return;
      }
    }

    if (plotObj) {
      if (name == "?") {
        QStringList names;

        plot->getObjectPropertyNames(plotObj, names);

        setCmdRc(names);
      }
      else {
        QVariant value;

        if (! CQUtil::getProperty(plotObj, name, value)) {
          charts_->errorMsg("Failed to get plot parameter '" + name + "'");
          return;
        }

        setCmdRc(value);
      }
    }
    else {
      if (name == "?") {
        QStringList names;

        plot->getPropertyNames(names);

        setCmdRc(names);
      }
      else {
        QVariant value;

        if (! plot->getProperty(name, value)) {
          charts_->errorMsg("Failed to get plot parameter '" + name + "'");
          return;
        }

        setCmdRc(value);
      }
    }
  }
  else if (argv.hasParseArg("annotation")) {
    QString annotationName = argv.getParseStr("annotation");

    CQChartsAnnotation *annotation = getAnnotationByName((CQChartsPlot *) nullptr, annotationName);

    if (! annotation)
      annotation = getAnnotationByName((CQChartsView *) nullptr, annotationName);

    if (! annotation) {
      charts_->errorMsg("Invalid annotation '" + annotationName + "'");
      return;
    }

    if (name == "?") {
      QStringList names;

      annotation->getPropertyNames(names);

      setCmdRc(names);
    }
    else {
      QVariant value;

      if (! annotation->getProperty(name, value)) {
        charts_->errorMsg("Failed to get annotation parameter '" + name + "'");
        return;
      }

      setCmdRc(value);
    }
  }
}

//------

void
CQChartsCmds::
setChartsPropertyCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("set_charts_property", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view"      , CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot"      , CQChartsCmdArg::Type::String, "plot name");
  argv.addCmdArg("-annotation", CQChartsCmdArg::Type::String, "annotation name");
  argv.endCmdGroup();

  argv.addCmdArg("-name" , CQChartsCmdArg::Type::String, "property name");
  argv.addCmdArg("-value", CQChartsCmdArg::Type::String, "property view");

  if (! argv.parse())
    return;

  //---

  QString name  = argv.getParseStr("name");
  QString value = argv.getParseStr("value");

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);

    if (! view) {
      charts_->errorMsg("Invalid view '" + viewName + "'");
      return;
    }

    if (! view->setProperty(name, value)) {
      charts_->errorMsg("Failed to set view parameter '" + name + "'");
      return;
    }
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsPlot *plot = getPlotByName(nullptr, plotName);

    if (! plot) {
      charts_->errorMsg("Invalid plot '" + plotName + "'");
      return;
    }

    if (! plot->setProperty(name, value)) {
      charts_->errorMsg("Failed to set plot parameter '" + name + "'");
      return;
    }
  }
  else if (argv.hasParseArg("annotation")) {
    QString annotationName = argv.getParseStr("annotation");

    CQChartsAnnotation *annotation = getAnnotationByName((CQChartsPlot *) nullptr, annotationName);

    if (! annotation)
      annotation = getAnnotationByName((CQChartsView *) nullptr, annotationName);

    if (! annotation) {
      charts_->errorMsg("Invalid annotation '" + annotationName + "'");
      return;
    }

    if (! annotation->setProperty(name, value)) {
      charts_->errorMsg("Failed to set annotation parameter '" + name + "'");
      return;
    }
  }
}

//------

void
CQChartsCmds::
getPaletteCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("get_palette", vars);

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name").setRequired();

  argv.addCmdArg("-interface"      , CQChartsCmdArg::Type::Boolean, "get interface palette");
  argv.addCmdArg("-palette"        , CQChartsCmdArg::Type::Integer, "get palette index");
  argv.addCmdArg("-get_color"      , CQChartsCmdArg::Type::Real   , "get color value");
  argv.addCmdArg("-get_color_scale", CQChartsCmdArg::Type::Boolean, "defined values");

  if (! argv.parse())
    return;

  //---

  QString viewName = argv.getParseStr("view");

  bool interface    = argv.getParseBool("interface");
  int  paletteIndex = argv.getParseInt ("palette"  );

  bool   getColorFlag  = argv.hasParseArg("get_color");
  double getColorValue = argv.getParseReal("get_color");
  bool   getColorScale = argv.getParseBool("get_color_scale");

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return;

  //---

  CQChartsGradientPalette *palette = nullptr;

  if (interface)
    interface = view->interfacePalette();
  else
    palette = view->themeObj()->palette(paletteIndex);

  if (getColorFlag) {
    QColor c = palette->getColor(getColorValue, getColorScale);

    setCmdRc(c.name());
  }
}

//------

void
CQChartsCmds::
setPaletteCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("set_palette", vars);

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name").setRequired();

  argv.addCmdArg("-interface"   , CQChartsCmdArg::Type::Boolean, "set interface palette");
  argv.addCmdArg("-palette"     , CQChartsCmdArg::Type::Integer, "set palette index");
  argv.addCmdArg("-color_type"  , CQChartsCmdArg::Type::String , "color type");
  argv.addCmdArg("-color_model" , CQChartsCmdArg::Type::String , "color model");
  argv.addCmdArg("-red_model"   , CQChartsCmdArg::Type::Integer, "red model");
  argv.addCmdArg("-green_model" , CQChartsCmdArg::Type::Integer, "green model");
  argv.addCmdArg("-blue_model"  , CQChartsCmdArg::Type::Integer, "blue model");
  argv.addCmdArg("-negate_red"  , CQChartsCmdArg::Type::SBool  , "negate red");
  argv.addCmdArg("-negate_green", CQChartsCmdArg::Type::SBool  , "negate green");
  argv.addCmdArg("-negate_blue" , CQChartsCmdArg::Type::SBool  , "negate blue");
  argv.addCmdArg("-red_min"     , CQChartsCmdArg::Type::Real   , "red min value");
  argv.addCmdArg("-green_min"   , CQChartsCmdArg::Type::Real   , "green min value");
  argv.addCmdArg("-blue_min"    , CQChartsCmdArg::Type::Real   , "blue min value");
  argv.addCmdArg("-red_max"     , CQChartsCmdArg::Type::Real   , "red max value");
  argv.addCmdArg("-green_max"   , CQChartsCmdArg::Type::Real   , "green max value");
  argv.addCmdArg("-blue_max"    , CQChartsCmdArg::Type::Real   , "blue max value");
  argv.addCmdArg("-defined"     , CQChartsCmdArg::Type::String , "defined values");

  if (! argv.parse())
    return;

  //---

  CQChartsPaletteColorData paletteData;

  QString viewName = argv.getParseStr("view");

  bool interface    = argv.getParseBool("interface");
  int  paletteIndex = argv.getParseInt ("palette"  );

  paletteData.colorTypeStr  = argv.getParseStr("color_type" , paletteData.colorTypeStr );
  paletteData.colorModelStr = argv.getParseStr("color_model", paletteData.colorModelStr);

  // alias for redModel, greenModel, blueModel
  paletteData.redModel   = argv.getParseOptInt("red_model"  );
  paletteData.greenModel = argv.getParseOptInt("green_model");
  paletteData.blueModel  = argv.getParseOptInt("blue_model" );

  // alias for negateRedm negateGreenm negateBlue
  paletteData.negateRed  = argv.getParseOptBool("negate_red"  );
  paletteData.negateGreen= argv.getParseOptBool("negate_green");
  paletteData.negateBlue = argv.getParseOptBool("negate_blue" );

  // alias for redMin, greenMin, blueMin, redMax, greenMax, blue_max
  paletteData.redMin   = argv.getParseOptReal("red_min"  );
  paletteData.greenMin = argv.getParseOptReal("green_min");
  paletteData.blueMin  = argv.getParseOptReal("green_min");

  paletteData.redMax   = argv.getParseOptReal("red_max"  );
  paletteData.greenMax = argv.getParseOptReal("green_max");
  paletteData.blueMax  = argv.getParseOptReal("green_max");

  QString definedStr = argv.getParseStr("defined");

  if (definedStr.length()) {
    QStringList strs = definedStr.split(" ", QString::SkipEmptyParts);

    if (strs.length()) {
      double dv = (strs.length() > 1 ? 1.0/(strs.length() - 1) : 0.0);

      paletteData.definedColors.clear();

      for (int j = 0; j < strs.length(); ++j) {
        int pos = strs[j].indexOf('=');

        double v = j*dv;
        QColor c;

        if (pos > 0) {
          QString lhs = strs[j].mid(0, pos).simplified();
          QString rhs = strs[j].mid(pos + 1).simplified();

          bool ok;

          v = lhs.toDouble(&ok);
          c = QColor(rhs);
        }
        else
          c = QColor(strs[j]);

        paletteData.definedColors.push_back(CQChartsDefinedColor(v, c));
      }
    }
  }

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return;

  //---

  CQChartsGradientPalette *palette = nullptr;

  if (interface)
    interface = view->interfacePalette();
  else
    palette = view->themeObj()->palette(paletteIndex);

  setPaletteData(palette, paletteData);

  //---

  view->updatePlots();

  CQChartsWindow *window = CQChartsWindowMgrInst->getWindowForView(view);

  if (window) {
    if (interface)
      window->updateInterfacePalette();
    else
      window->updateThemePalettes();
  }
}

//------

void
CQChartsCmds::
groupPlotsCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("group_plots", vars);

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name").setRequired();

  argv.addCmdArg("-x1x2"   , CQChartsCmdArg::Type::Boolean, "use shared x axis");
  argv.addCmdArg("-y1y2"   , CQChartsCmdArg::Type::Boolean, "use shared y axis");
  argv.addCmdArg("-overlay", CQChartsCmdArg::Type::Boolean, "overlay");

  if (! argv.parse())
    return;

  //---

  QString viewName = argv.getParseStr ("view");
  bool    x1x2     = argv.getParseBool("x1x2");
  bool    y1y2     = argv.getParseBool("y1y2");
  bool    overlay  = argv.getParseBool("overlay");

  const Vars &plotNames = argv.getParseArgs();

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return;

  //---

  Plots plots;

  getPlotsByName(view, plotNames, plots);

  //---

  if      (x1x2) {
    if (plots.size() != 2) {
      charts_->errorMsg("Need 2 plots for x1x2");
      return;
    }

    view->initX1X2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (y1y2) {
    if (plots.size() != 2) {
      charts_->errorMsg("Need 2 plots for y1y2");
      return;
    }

    view->initY1Y2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (overlay) {
    if (plots.size() < 2) {
      charts_->errorMsg("Need 2 or more plots for overlay");
      return;
    }

    view->initOverlay(plots, /*reset*/true);
  }
  else {
    charts_->errorMsg("No grouping specified");
    return;
  }
}

//------

void
CQChartsCmds::
placePlotsCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("place_plots", vars);

  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name").setRequired();

  argv.addCmdArg("-vertical"  , CQChartsCmdArg::Type::Boolean, "place vertical");
  argv.addCmdArg("-horizontal", CQChartsCmdArg::Type::Boolean, "place horizontal");
  argv.addCmdArg("-rows"      , CQChartsCmdArg::Type::Integer, "place using n rows");
  argv.addCmdArg("-columns"   , CQChartsCmdArg::Type::Integer, "place using n columns");

  if (! argv.parse())
    return;

  //---

  QString viewName   = argv.getParseStr ("view");
  bool    vertical   = argv.getParseBool("vertical");
  bool    horizontal = argv.getParseBool("horizontal");
  int     rows       = argv.getParseInt ("rows"   , -1);
  int     columns    = argv.getParseInt ("columns", -1);

  const Vars &plotNames = argv.getParseArgs();

  //---

  CQChartsView *view = getViewByName(viewName);
  if (! view) return;

  //---

  Plots plots;

  getPlotsByName(view, plotNames, plots);

  //---

  view->placePlots(plots, vertical, horizontal, rows, columns);
}

//------

void
CQChartsCmds::
foldModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("fold_model", vars);

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column to fold");

  if (! argv.parse())
    return;

  //---

  int modelInd = argv.getParseInt("model", -1);

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
  }

  ModelP model = modelData->model();

  CQChartsColumn column = argv.getParseColumn("column", model.data());

  //---

  CQFoldData foldData(column.column());

  CQFoldedModel *foldedModel = new CQFoldedModel(model.data(), foldData);

  QSortFilterProxyModel *foldProxyModel = new QSortFilterProxyModel;

  foldProxyModel->setObjectName("foldProxyModel");

  foldProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

  foldProxyModel->setSourceModel(foldedModel);

  ModelP foldedModelP(foldProxyModel);

  CQChartsModelData *foldedModelData = charts_->initModelData(foldedModelP);

  //---

  setCmdRc(foldedModelData->ind());
}

//------

void
CQChartsCmds::
flattenModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("flatten_model", vars);

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-group", CQChartsCmdArg::Type::Column , "grouping column id");

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-mean" , CQChartsCmdArg::Type::Boolean, "calc mean of column values");
  argv.addCmdArg("-sum"  , CQChartsCmdArg::Type::Boolean, "calc sum of column values");
  argv.endCmdGroup();

  if (! argv.parse())
    return;

  //---

  int  modelInd = argv.getParseInt ("model" , -1);
  bool meanFlag = argv.getParseBool("mean");
  bool sumFlag  = argv.getParseBool("sum");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
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

    State hierVisit(QAbstractItemModel *model, const VisitData &data) override {
      ++hierRow_;

      bool ok;

      groupValue_[hierRow_] =
        CQChartsUtil::modelValue(charts_, model, data.row, 0, data.parent, ok);

      return State::OK;
    }

    State visit(QAbstractItemModel *model, const VisitData &data) override {
      int nc = numCols();

      if (isHierarchical()) {
        for (int c = 1; c < nc; ++c) {
          bool ok;

          QVariant var = CQChartsUtil::modelValue(charts_, model, data.row, c, data.parent, ok);

          if (ok)
            rowColValueSet_[hierRow_][c - 1].addValue(var);
        }
      }
      else if (groupColumn_.isValid()) {
        bool ok;

        QVariant groupVar =
          CQChartsUtil::modelValue(charts_, model, data.row, groupColumn_, data.parent, ok);

        auto p = valueGroup_.find(groupVar);

        if (p == valueGroup_.end()) {
          int group = valueGroup_.size();

          p = valueGroup_.insert(p, ValueGroup::value_type(groupVar, group));

          groupValue_[group] = groupVar;
        }

        int group = (*p).second;

        for (int c = 0; c < nc; ++c) {
          bool ok;

          QVariant var = CQChartsUtil::modelValue(charts_, model, data.row, c, data.parent, ok);

          if (ok)
            rowColValueSet_[group][c].addValue(var);
        }
      }
      else {
        for (int c = 0; c < nc; ++c) {
          bool ok;

          QVariant var = CQChartsUtil::modelValue(charts_, model, data.row, c, data.parent, ok);

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
    CQChartsColumn groupColumn_;
    int            hierRow_ { -1 };
    ValueGroup     valueGroup_;
    GroupValue     groupValue_;
    RowColValueSet rowColValueSet_;
  };

  FlattenVisitor flattenVisitor(charts_, groupColumn);

  CQChartsModelVisit::exec(model.data(), flattenVisitor);

  int nh = flattenVisitor.numHierColumns();
  int nc = flattenVisitor.numFlatColumns();
  int nr = flattenVisitor.numHierRows();

  //---

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQChartsDataFilterModel *dataModel = new CQChartsDataFilterModel(charts_, nc + nh, nr);

  CQDataModel *model1 = dataModel->dataModel();

  // set hierarchical column
  if (flattenVisitor.isHierarchical()) {
    bool ok;

    QString name = CQChartsUtil::modelHeaderString(model.data(), 0, Qt::Horizontal, ok);

    CQChartsUtil::setModelHeaderValue(model1, 0, Qt::Horizontal, name);
  }

  // set other columns and types
  for (int c = 0; c < nc; ++c) {
    bool isGroup = (groupColumn.column() == c);

    bool ok;

    QString name = CQChartsUtil::modelHeaderString(model.data(), c + nh, Qt::Horizontal, ok);

    CQChartsUtil::setModelHeaderValue(model1, c + nh, Qt::Horizontal, name);

    CQBaseModel::Type  columnType;
    CQChartsNameValues nameValues;

    if (! CQChartsUtil::columnValueType(charts_, model.data(), c, columnType, nameValues))
      continue;

    CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

    if (! typeData)
      continue;

    if (isGroup || typeData->isNumeric()) {
      if (! columnTypeMgr->setModelColumnType(model1, c + nh, columnType, nameValues))
        continue;
    }
  }

  //--

  for (int r = 0; r < nr; ++r) {
    if (flattenVisitor.isHierarchical()) {
      QVariant var = flattenVisitor.groupValue(r);

      CQChartsUtil::setModelValue(model1, r, 0, var);
    }

    for (int c = 0; c < nc; ++c) {
      bool isGroup = (groupColumn.column() == c);

      if (! isGroup) {
        double v = 0.0;

        if      (sumFlag)
          v = flattenVisitor.hierSum(r, c);
        else if (meanFlag)
          v = flattenVisitor.hierMean(r, c);

        CQChartsUtil::setModelValue(model1, r, c + nh, v);
      }
      else {
        QVariant v = flattenVisitor.groupValue(r);

        CQChartsUtil::setModelValue(model1, r, c + nh, v);
      }
    }
  }

  ModelP dataModelP(dataModel);

  CQChartsModelData *dataModelData = charts_->initModelData(dataModelP);

  //---

  setCmdRc(dataModelData->ind());
}

//------

void
CQChartsCmds::
sortModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("sort_model", vars);

  argv.addCmdArg("-model"     , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-column"    , CQChartsCmdArg::Type::Column , "column to sort");
  argv.addCmdArg("-decreasing", CQChartsCmdArg::Type::Boolean, "invert sort");

  if (! argv.parse())
    return;

  //---

  int  modelInd   = argv.getParseInt   ("model" , -1);
  bool decreasing = argv.getParseBool  ("decreasing");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
  }

  ModelP model = modelData->currentModel();

  CQChartsColumn column = argv.getParseColumn("column", model.data());

  //---

  Qt::SortOrder order = (decreasing ? Qt::DescendingOrder : Qt::AscendingOrder);

  sortModel(model, column.column(), order);
}

//------

void
CQChartsCmds::
filterModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("filter_model", vars);

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-expr" , CQChartsCmdArg::Type::String , "filter expression");

  if (! argv.parse())
    return;

  //---

  int     modelInd = argv.getParseInt("model", -1);
  QString expr     = argv.getParseStr("expr");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
  }

  ModelP model = modelData->currentModel();

  CQChartsModelFilter *modelFilter = qobject_cast<CQChartsModelFilter *>(model.data());

  if (! modelFilter) {
    charts_->errorMsg("No filter support for model");
    return;
  }

  modelFilter->setExpressionFilter(expr);
}

//------

void
CQChartsCmds::
correlationModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("correlation_model", vars);

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-flip" , CQChartsCmdArg::Type::Boolean, "correlate rows instead of columns");

  if (! argv.parse())
    return;

  //---

  int  modelInd = argv.getParseInt ("model", -1);
  bool flip     = argv.getParseBool("flip");

  //------

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
  }

  //------

  CQChartsLoader loader(charts_);

  QAbstractItemModel *model1 =
    loader.createCorrelationModel(modelData->currentModel().data(), flip);

  ModelP modelp1(model1);

  CQChartsModelData *modelData1 = charts_->initModelData(modelp1);

  //---

  setCmdRc(modelData1->ind());
}

//------

void
CQChartsCmds::
subsetModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("subset_model", vars);

  argv.addCmdArg("-model" , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-left"  , CQChartsCmdArg::Type::Column , "left (start) column");
  argv.addCmdArg("-right" , CQChartsCmdArg::Type::Column , "right (end) column");
  argv.addCmdArg("-top"   , CQChartsCmdArg::Type::Integer, "top (start) row");
  argv.addCmdArg("-bottom", CQChartsCmdArg::Type::Integer, "bottom (end) row");

  if (! argv.parse())
    return;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
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

  ModelP modelp1(subsetModel);

  CQChartsModelData *modelData1 = charts_->initModelData(modelp1);

  //---

  setCmdRc(modelData1->ind());
}

//------

void
CQChartsCmds::
transposeModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("transpose_model", vars);

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model id");

  if (! argv.parse())
    return;

  //---

  // get model
  int modelInd = argv.getParseInt("model", -1);

  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
  }

  QAbstractItemModel *model = modelData->currentModel().data();

  //------

  CQTransposeModel *transposeModel = new CQTransposeModel(model);

  ModelP modelp1(transposeModel);

  CQChartsModelData *modelData1 = charts_->initModelData(modelp1);

  //---

  setCmdRc(modelData1->ind());
}

//------

void
CQChartsCmds::
exportModelCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("export_model", vars);

  argv.addCmdArg("-model"  , CQChartsCmdArg::Type::Integer, "model id");
  argv.addCmdArg("-to"     , CQChartsCmdArg::Type::String , "destination format");
  argv.addCmdArg("-file"   , CQChartsCmdArg::Type::String , "file name");
  argv.addCmdArg("-hheader", CQChartsCmdArg::Type::SBool  , "output horizontal header");
  argv.addCmdArg("-vheader", CQChartsCmdArg::Type::SBool  , "output vertical header");

  if (! argv.parse())
    return;

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
    return;
  }

  std::ofstream fos; bool isFile = false;

  if (filename.length()) {
    fos.open(filename.toLatin1().constData());

    if (fos.fail()) {
      charts_->errorMsg("Failed to open '" + filename + "'");
      return;
    }

    isFile = true;
  }

  ModelP model = modelData->currentModel();

  if      (toName.toLower() == "csv") {
    CQChartsUtil::exportModel(model.data(), CQBaseModel::DataType::CSV,
                              hheader, vheader, (isFile ? fos : std::cout));
  }
  else if (toName.toLower() == "tsv") {
    CQChartsUtil::exportModel(model.data(), CQBaseModel::DataType::TSV,
                              hheader, vheader, (isFile ? fos : std::cout));
  }
  else {
    charts_->errorMsg("Invalid output format");
    return;
  }
}

//------

// get charts data
void
CQChartsCmds::
getChartsDataCmd(const Vars &vars)
{
  using QVariantList = QList<QVariant>;

  CQChartsCmdArgs argv("get_charts_data", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model index");
  argv.addCmdArg("-view" , CQChartsCmdArg::Type::String , "view name");
  argv.addCmdArg("-type" , CQChartsCmdArg::Type::String , "type name");
  argv.addCmdArg("-plot" , CQChartsCmdArg::Type::String , "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-object", CQChartsCmdArg::Type::String, "object id");

  argv.addCmdArg("-column", CQChartsCmdArg::Type::Column , "column");
  argv.addCmdArg("-header", CQChartsCmdArg::Type::Boolean, "get header data");
  argv.addCmdArg("-row"   , CQChartsCmdArg::Type::Row    , "row number or id");
  argv.addCmdArg("-ind"   , CQChartsCmdArg::Type::String , "model index");

  argv.addCmdArg("-role", CQChartsCmdArg::Type::String, "role id");

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String, "option name").setRequired();
  argv.addCmdArg("-data", CQChartsCmdArg::Type::String, "option data");

  if (! argv.parse())
    return;

  //---

  QString objectId = argv.getParseStr("object");

  bool    header = argv.getParseBool("header");
  QString name   = argv.getParseStr ("name");
  QString data   = argv.getParseStr ("data");

  //---

  QString roleName = argv.getParseStr("role");

  int role = Qt::EditRole;

  if (roleName != "")
    role = CQChartsUtil::nameToRole(roleName);

  //---

  if      (argv.hasParseArg("model")) {
    int modelInd = argv.getParseInt("model", -1);

    // get model
    CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

    if (! modelData) {
      charts_->errorMsg("No model data");
      return;
    }

    ModelP model = modelData->currentModel();

    //---

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    int row = argv.getParseRow("row");

    if (argv.hasParseArg("ind")) {
#ifdef CQCharts_USE_TCL
      int irow, icol;

      if (! CQTclUtil::stringToModelIndex(argv.getParseStr("ind"), irow, icol)) {
        charts_->errorMsg("Invalid model index");
        return;
      }

      row    = irow;
      column = icol;
#else
      row    = 0;
      column = 0;
#endif
    }

    //---

    // get column header or row, column value
    if      (name == "value") {
      QVariant var;

      if (header) {
        if (! column.isValid()) {
          setCmdError("Invalid header column specified");
          return;
        }

        bool ok;

        var = CQChartsUtil::modelHeaderValue(model.data(), column, role, ok);

        if (! var.isValid()) {
          setCmdError("Invalid header value");
          return;
        }
      }
      else {
#if 0
        QModelIndex ind = model.data()->index(row, column.column());

        if (! ind.isValid()) {
          setCmdError("Invalid data row/column specified");
          return;
        }
#endif

        QModelIndex parent;

        bool ok;

        var = CQChartsUtil::modelValue(charts_, model.data(), row, column, parent, role, ok);

        if (! var.isValid()) {
          setCmdError("Invalid model value");
          return;
        }
      }

      setCmdRc(var);
    }
    else if (name == "num_rows" || name == "num_columns" || name == "hierarchical") {
      CQChartsModelDetails *details = modelData->details();

      if      (name == "num_rows")
        setCmdRc(details->numRows());
      else if (name == "num_columns")
        setCmdRc(details->numColumns());
      else if (name == "hierarchical")
        setCmdRc(details->isHierarchical());
    }
    else if (name == "header") {
      const CQChartsModelDetails *details = modelData->details();

      int nc = details->numColumns();

      QVariantList vars;

      for (int c = 0; c < nc; ++c) {
        bool ok;

        QVariant var = CQChartsUtil::modelHeaderValue(model.data(), c, role, ok);

        vars.push_back(var);
      }

      setCmdRc(vars);
    }
    else if (name == "row") {
      const CQChartsModelDetails *details = modelData->details();

      int nr = details->numRows();
      int nc = details->numColumns();

      if (row < 0 || row >= nr) {
        setCmdError("Invalid row number");
        return;
      }

      QModelIndex parent; // TODO;

      QVariantList vars;

      for (int c = 0; c < nc; ++c) {
        bool ok;

        QVariant var =
          CQChartsUtil::modelValue(charts_, model.data(), row, c, parent, role, ok);

        vars.push_back(var);
      }

      setCmdRc(vars);
    }
    else if (name == "column") {
      const CQChartsModelDetails *details = modelData->details();

      int nr = details->numRows();
      int nc = details->numColumns();

      if (column < 0 || column.column() >= nc) {
        setCmdError("Invalid row number");
        return;
      }

      QModelIndex parent; // TODO;

      QVariantList vars;

      for (int r = 0; r < nr; ++r) {
        bool ok;

        QVariant var =
          CQChartsUtil::modelValue(charts_, model.data(), r, column.column(), parent, role, ok);

        vars.push_back(var);
      }

      setCmdRc(vars);
    }
    // column named value
    else if (CQChartsModelColumnDetails::isNamedValue(name)) {
      const CQChartsModelDetails *details = modelData->details();

      if (argv.hasParseArg("column")) {
        if (! column.isValid() || column.column() >= details->numColumns()) {
          setCmdError("Invalid column specified");
          return;
        }

        const CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

        setCmdRc(columnDetails->getNamedValue(name));
      }
      else {
        int nc = details->numColumns();

        QVariantList vars;

        for (int c = 0; c < nc; ++c) {
          const CQChartsModelColumnDetails *columnDetails = details->columnDetails(c);

          vars.push_back(columnDetails->getNamedValue(name));
        }

        setCmdRc(vars);
      }
    }
    else if (name == "map") {
      CQChartsModelDetails *details = modelData->details();

      if (! column.isValid() || column.column() >= details->numColumns()) {
        setCmdError("Invalid column specified");
        return;
      }

#if 0
      QModelIndex ind = model.data()->index(row, column.column());
#endif

      QModelIndex parent;

      bool ok;

      QVariant var =
        CQChartsUtil::modelValue(charts_, model.data(), row, column.column(), parent, role, ok);

      CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

      double r = columnDetails->map(var);

      setCmdRc(r);
    }
    else if (name == "column_index") {
      CQChartsColumn column;

      if (! CQChartsUtil::stringToColumn(model.data(), data, column))
        column = -1;

      setCmdRc(column.column());
    }
    else {
      setCmdError("Invalid name '" + name + "' specified");
      return;
    }
  }
  else if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);
    if (! view) return;

    if      (name == "plots") {
      QVariantList vars;

      CQChartsView::Plots plots;

      view->getPlots(plots);

      for (const auto &plot : plots)
        vars.push_back(plot->pathId());

      setCmdRc(vars);
    }
    else if (name == "annotations") {
      QVariantList vars;

      const CQChartsView::Annotations &annotations = view->annotations();

      for (const auto &annotation : annotations)
        vars.push_back(annotation->pathId());

      setCmdRc(vars);
    }
    else if (name == "view_width") {
      CQChartsLength len(data, CQChartsLength::Units::VIEW);

      double w = view->lengthViewWidth(len);

      setCmdRc(w);
    }
    else if (name == "view_height") {
      CQChartsLength len(data, CQChartsLength::Units::VIEW);

      double h = view->lengthViewHeight(len);

      setCmdRc(h);
    }
    else if (name == "pixel_width") {
      CQChartsLength len(data, CQChartsLength::Units::VIEW);

      double w = view->lengthPixelWidth(len);

      setCmdRc(w);
    }
    else if (name == "pixel_height") {
      CQChartsLength len(data, CQChartsLength::Units::VIEW);

      double h = view->lengthPixelHeight(len);

      setCmdRc(h);
    }
    else {
      setCmdError("Invalid name '" + name + "' specified");
      return;
    }
  }
  else if (argv.hasParseArg("type")) {
    QString typeName = argv.getParseStr("type");

    if (! charts_->isPlotType(typeName)) {
      charts_->errorMsg("No type '" + typeName + "'");
      return;
    }

    CQChartsPlotType *type = charts_->plotType(typeName);

    if (! type) {
      charts_->errorMsg("No type '" + typeName + "'");
      return;
    }

    //---

    if      (name == "properties") {
      QStringList names;

      type->propertyNames(names);

      setCmdRc(names);
    }
    else if (name == "parameters") {
      const CQChartsPlotType::Parameters &parameters = type->parameters();

      QStringList names;

      for (auto &parameter : parameters)
        names.push_back(parameter->name());

      setCmdRc(names);
    }
    else if (name.left(10) == "parameter.") {
      if (! type->hasParameter(data)) {
        charts_->errorMsg("No parameter '" + data + "'");
        return;
      }

      const CQChartsPlotParameter &parameter = type->getParameter(data);

      QString name1 = name.mid(10);

      if (name1 == "properties") {
        QStringList names;

        parameter.propertyNames(names);

        setCmdRc(names);
      }
      else if (parameter.hasProperty(name1)) {
        setCmdRc(parameter.getPropertyValue(name1));
      }
      else {
        setCmdError("Invalid name 'parameter." + name1 + "' specified");
        return;
      }
    }
    else if (type->hasProperty(name)) {
      setCmdRc(type->getPropertyValue(name));
    }
    else {
      setCmdError("Invalid name '" + name + "' specified");
      return;
    }

    return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsView *view = nullptr;

    CQChartsPlot *plot = getPlotByName(view, plotName);
    if (! plot) return;

    int row = argv.getParseRow("row", plot);

    //---

    // get model ind
    if      (name == "model") {
      CQChartsModelData *modelData = charts_->getModelData(plot->model().data());

      if (! modelData) {
        charts_->errorMsg("No model data");
        return;
      }

      setCmdRc(modelData->ind());
    }
    // get column header or row, column value
    else if (name == "value") {
      CQChartsColumn column = argv.getParseColumn("column", plot->model().data());

      //---

      QVariant var;

      if (header) {
        if (! column.isValid()) {
          setCmdError("Invalid header column specified");
          return;
        }

        bool ok;

        var = CQChartsUtil::modelHeaderValue(plot->model().data(), column, role, ok);

        if (! var.isValid()) {
          setCmdError("Invalid header value");
          return;
        }
      }
      else {
        QModelIndex parent;

        bool ok;

        QVariant var = plot->modelValue(row, column, parent, role, ok);

        bool rc;

        setCmdRc(CQChartsVariant::toString(var, rc));
      }
    }
    else if (name == "map") {
      CQChartsModelData *modelData = charts_->getModelData(plot->model().data());

      if (! modelData) {
        charts_->errorMsg("No model data");
        return;
      }

      CQChartsColumn column = argv.getParseColumn("column", plot->model().data());

      //---

      CQChartsModelDetails *details = modelData->details();

      if (! column.isValid() || column.column() >= details->numColumns()) {
        setCmdError("Invalid column specified");
        return;
      }

      QModelIndex parent;

      bool ok;

      QVariant var = plot->modelValue(row, column, parent, role, ok);

      CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

      double r = columnDetails->map(var);

      setCmdRc(r);
    }
    else if (name == "annotations") {
      QVariantList vars;

      const CQChartsPlot::Annotations &annotations = plot->annotations();

      for (const auto &annotation : annotations)
        vars.push_back(annotation->pathId());

      setCmdRc(vars);
    }
    else if (name == "objects") {
      QVariantList vars;

      const CQChartsPlot::PlotObjs &objs = plot->plotObjects();

      for (const auto &obj : objs)
        vars.push_back(obj->id());

      setCmdRc(vars);
    }
    else if (name == "inds") {
      if (! objectId.length()) {
        charts_->errorMsg("Missing object id");
        return;
      }

      QList<QModelIndex> inds = plot->getObjectInds(objectId);

      QVariantList vars;

      for (int i = 0; i < inds.length(); ++i)
        vars.push_back(inds[i]);

      setCmdRc(vars);
    }
    else if (name == "plot_width") {
      CQChartsLength len(data, CQChartsLength::Units::PLOT);

      double w = plot->lengthPlotWidth(len);

      setCmdRc(w);
    }
    else if (name == "plot_height") {
      CQChartsLength len(data, CQChartsLength::Units::PLOT);

      double h = plot->lengthPlotHeight(len);

      setCmdRc(h);
    }
    else if (name == "pixel_width") {
      CQChartsLength len(data, CQChartsLength::Units::PLOT);

      double w = plot->lengthPixelWidth(len);

      setCmdRc(w);
    }
    else if (name == "pixel_height") {
      CQChartsLength len(data, CQChartsLength::Units::PLOT);

      double h = plot->lengthPixelHeight(len);

      setCmdRc(h);
    }
    else {
      setCmdError("Invalid name '" + name + "' specified");
      return;
    }
  }
  else {
    if      (name == "models") {
      QVariantList vars;

      CQCharts::ModelDatas modelDatas;

      charts_->getModelDatas(modelDatas);

      for (auto &modelData : modelDatas)
        vars.push_back(modelData->ind());

      setCmdRc(vars);
    }
    else if (name == "views") {
      QVariantList vars;

      CQCharts::Views views;

      charts_->getViews(views);

      for (auto &view : views)
        vars.push_back(view->id());

      setCmdRc(vars);
    }
    else if (name == "types") {
      QStringList names, descs;

      charts_->getPlotTypeNames(names, descs);

      setCmdRc(names);
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

      setCmdRc(vars);
    }
    else if (name == "current_model") {
      CQChartsModelData *modelData = charts_->currentModelData();

      if (! modelData) {
        charts_->errorMsg("No model data");
        return;
      }

      setCmdRc(modelData->ind());
    }
    else {
      setCmdError("Invalid name '" + name + "' specified");
      return;
    }
  }
}

//------

// set charts data
void
CQChartsCmds::
setChartsDataCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("set_charts_data", vars);

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
    return;

  //---

  bool    header = argv.getParseBool("header");
  QString name   = argv.getParseStr ("name");
  QString value  = argv.getParseStr ("value");

  //---

  QString roleName = argv.getParseStr("role");

  int role = Qt::EditRole;

  if (roleName != "")
    role = CQChartsUtil::nameToRole(roleName);

  //---

  if      (argv.hasParseArg("model")) {
    int modelInd = argv.getParseInt("model", -1);

    // get model
    CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

    if (! modelData) {
      charts_->errorMsg("No model data");
      return;
    }

    ModelP model = modelData->currentModel();

    //---

    CQChartsColumn column = argv.getParseColumn("column", model.data());

    int row = argv.getParseRow("row");

    // set column header or row, column value
    if      (name == "value") {
      if (header) {
        if (! column.isValid()) {
          setCmdError("Invalid header column specified");
          return;
        }

        if (! CQChartsUtil::setModelHeaderValue(model.data(), column, value, role))
          charts_->errorMsg("Failed to set header value");
      }
      else {
        QModelIndex ind = model.data()->index(row, column.column());

        if (! ind.isValid()) {
          setCmdError("Invalid data row/column specified");
          return;
        }

        if (! CQChartsUtil::setModelValue(model.data(), row, column, value, role))
          charts_->errorMsg("Failed to set row value");
      }
    }
    else if (name == "column_type") {
      if (column.isValid())
        CQChartsUtil::setColumnTypeStr(charts_, model.data(), column, value);
      else
        CQChartsUtil::setColumnTypeStrs(charts_, model.data(), value);
    }
    else if (name == "name") {
      charts_->setModelName(modelData, value);
    }
    else if (name == "process") {
      CQChartsUtil::processExpression(model.data(), value);
    }
    else {
      setCmdError("Invalid name '" + name + "' specified");
      return;
    }
  }
  else if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    CQChartsView *view = getViewByName(viewName);
    if (! view) return;

    if (name == "fit")
      view->fitSlot();
    else {
      setCmdError("Invalid name '" + name + "' specified");
      return;
    }
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    CQChartsView *view = nullptr;

    CQChartsPlot *plot = getPlotByName(view, plotName);
    if (! plot) return;

    setCmdError("Invalid name '" + name + "' specified");
    return;
  }
  else {
    setCmdError("Invalid name '" + name + "' specified");
    return;
  }
}

//------

void
CQChartsCmds::
createRectShapeCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_rect_shape", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-start", CQChartsCmdArg::Type::Position, "start");
  argv.addCmdArg("-end"  , CQChartsCmdArg::Type::Position, "end"  );

  argv.addCmdArg("-margin" , CQChartsCmdArg::Type::String, "margin");
  argv.addCmdArg("-padding", CQChartsCmdArg::Type::String, "padding");

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
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;
  }

  //---

  CQChartsBoxData boxData;

  CQChartsFillData   &background = boxData.shape.background;
  CQChartsStrokeData &border     = boxData.shape.border;

  border.visible = true;

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition start = argv.getParsePosition(view, plot, "start");
  CQChartsPosition end   = argv.getParsePosition(view, plot, "end"  );

  boxData.margin  = argv.getParseReal("margin" , boxData.margin);
  boxData.padding = argv.getParseReal("padding", boxData.padding);

  background.visible = argv.getParseBool ("background"        , background.visible);
  background.color   = argv.getParseColor("background_color"  , background.color);
  background.alpha   = argv.getParseReal ("background_alpha"  , background.alpha);
//background.pattern = argv.getParseStr  ("background_pattern", background.pattern);

  border.visible    = argv.getParseBool    ("border"      , border.visible);
  border.color      = argv.getParseColor   ("border_color", border.color  );
  border.alpha      = argv.getParseReal    ("border_alpha", border.alpha  );
  border.width      = argv.getParseLength  (view, plot, "border_width", border.width);
  border.dash       = argv.getParseLineDash("border_dash" , border.dash   );
  border.cornerSize = argv.getParseLength  (view, plot, "corner_size", border.cornerSize);

  boxData.borderSides = argv.getParseSides("border_sides", boxData.borderSides);

  //---

  CQChartsRectAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addRectAnnotation(start, end);
  else if (plot)
    annotation = plot->addRectAnnotation(start, end);
  else
    return;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  setCmdRc(annotation->ind());
}

//------

void
CQChartsCmds::
createEllipseShapeCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_ellipse_shape", vars);

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
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;
  }

  //---

  CQChartsBoxData boxData;

  CQChartsFillData   &background = boxData.shape.background;
  CQChartsStrokeData &border     = boxData.shape.border;

  border.visible = true;

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition center = argv.getParsePosition(view, plot, "center");

  CQChartsLength rx = argv.getParseLength(view, plot, "rx");
  CQChartsLength ry = argv.getParseLength(view, plot, "ry");

  background.visible = argv.getParseBool ("background"        , background.visible);
  background.color   = argv.getParseColor("background_color"  , background.color);
  background.alpha   = argv.getParseReal ("background_alpha"  , background.alpha);
//background.pattern = argv.getParseStr  ("background_pattern", background.pattern);

  border.visible    = argv.getParseBool    ("border"      , border.visible);
  border.color      = argv.getParseColor   ("border_color", border.color  );
  border.alpha      = argv.getParseReal    ("border_alpha", border.alpha  );
  border.width      = argv.getParseLength  (view, plot, "border_width", border.width);
  border.dash       = argv.getParseLineDash("border_dash" , border.dash   );
  border.cornerSize = argv.getParseLength  (view, plot, "corner_size", border.cornerSize);

  boxData.borderSides = argv.getParseSides("border_sides", boxData.borderSides);

  //---

  CQChartsEllipseAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addEllipseAnnotation(center, rx, ry);
  else if (plot)
    annotation = plot->addEllipseAnnotation(center, rx, ry);
  else
    return;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setBoxData(boxData);

  setCmdRc(annotation->ind());
}

//------

void
CQChartsCmds::
createPolygonShapeCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_polygon_shape", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-points", CQChartsCmdArg::Type::Polygon, "points string");

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
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;
  }

  //---

  CQChartsShapeData shapeData;

  CQChartsFillData   &background = shapeData.background;
  CQChartsStrokeData &border     = shapeData.border;

  border.visible = true;

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  QPolygonF points = argv.getParsePoly("points");

  background.visible = argv.getParseBool ("background"        , background.visible);
  background.color   = argv.getParseColor("background_color"  , background.color);
  background.alpha   = argv.getParseReal ("background_alpha"  , background.alpha);
//background.pattern = argv.getParseStr  ("background_pattern", background.pattern);

  border.visible = argv.getParseBool    ("border"      , border.visible);
  border.color   = argv.getParseColor   ("border_color", border.color  );
  border.alpha   = argv.getParseReal    ("border_alpha", border.alpha  );
  border.width   = argv.getParseLength  (view, plot, "border_width", border.width  );
  border.dash    = argv.getParseLineDash("border_dash" , border.dash   );

  //---

  if (! points.length()) {
    std::cerr << "no points\n";
    return;
  }

  //---

  CQChartsPolygonAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addPolygonAnnotation(points);
  else if (plot)
    annotation = plot->addPolygonAnnotation(points);
  else
    return;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setShapeData(shapeData);

  setCmdRc(annotation->ind());
}

//------

void
CQChartsCmds::
createPolylineShapeCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_polyline_shape", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-points", CQChartsCmdArg::Type::Polygon, "points string");

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
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;
  }

  //---

  CQChartsShapeData shapeData;

  CQChartsFillData   &background = shapeData.background;
  CQChartsStrokeData &border     = shapeData.border;

  border.visible = true;

  QString viewName = argv.getParseStr("view");
  QString plotName = argv.getParseStr("plot");
  QString id       = argv.getParseStr("id");
  QString tipId    = argv.getParseStr("tip");

  QPolygonF points = argv.getParsePoly("points");

  background.visible = argv.getParseBool ("background"        , background.visible);
  background.color   = argv.getParseColor("background_color"  , background.color);
  background.alpha   = argv.getParseReal ("background_alpha"  , background.alpha);
//background.pattern = argv.getParseStr  ("background_pattern", background.pattern);

  border.visible = argv.getParseBool    ("border"      , border.visible);
  border.color   = argv.getParseColor   ("border_color", border.color  );
  border.alpha   = argv.getParseReal    ("border_alpha", border.alpha  );
  border.width   = argv.getParseLength  (view, plot, "border_width", border.width  );
  border.dash    = argv.getParseLineDash("border_dash" , border.dash   );

  //---

  if (! points.length()) {
    std::cerr << "no points\n";
    return;
  }

  //---

  CQChartsPolylineAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addPolylineAnnotation(points);
  else if (plot)
    annotation = plot->addPolylineAnnotation(points);
  else
    return;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setShapeData(shapeData);

  setCmdRc(annotation->ind());
}

//------

void
CQChartsCmds::
createTextShapeCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_text_shape", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-position", CQChartsCmdArg::Type::Position, "position");

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
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;
  }

  //---

  CQChartsTextData textData;
  CQChartsBoxData  boxData;

  CQChartsFillData   &background = boxData.shape.background;
  CQChartsStrokeData &border     = boxData.shape.border;

  background.visible = false;
  border    .visible = false;

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition pos = argv.getParsePosition(view, plot, "position");

  QString text = argv.getParseStr("text", "Annotation");

  textData.font     = argv.getParseFont ("font"    , textData.font    );
  textData.color    = argv.getParseColor("color"   , textData.color   );
  textData.alpha    = argv.getParseReal ("alpha"   , textData.alpha   );
  textData.angle    = argv.getParseReal ("angle"   , textData.angle   );
  textData.contrast = argv.getParseBool ("contrast", textData.contrast);
  textData.align    = argv.getParseAlign("align"   , textData.align   );
  textData.html     = argv.getParseBool ("html"    , textData.html    );


  background.visible = argv.getParseBool ("background"        , background.visible);
  background.color   = argv.getParseColor("background_color"  , background.color);
  background.alpha   = argv.getParseReal ("background_alpha"  , background.alpha);
//background.pattern = argv.getParseStr  ("background_pattern", background.pattern);

  border.visible    = argv.getParseBool    ("border"      , border.visible);
  border.color      = argv.getParseColor   ("border_color", border.color  );
  border.alpha      = argv.getParseReal    ("border_alpha", border.alpha  );
  border.width      = argv.getParseLength  (view, plot, "border_width", border.width);
  border.dash       = argv.getParseLineDash("border_dash" , border.dash   );
  border.cornerSize = argv.getParseLength  (view, plot, "corner_size", border.cornerSize);

  boxData.borderSides = argv.getParseSides("border_sides", boxData.borderSides);

  //---

  CQChartsTextAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addTextAnnotation(pos, text);
  else if (plot)
    annotation = plot->addTextAnnotation(pos, text);
  else
    return;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setTextData(textData);
  annotation->setBoxData(boxData);

  setCmdRc(annotation->pathId());
}

//------

void
CQChartsCmds::
createArrowShapeCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_arrow_shape", vars);

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
  argv.addCmdArg("-empty"       , CQChartsCmdArg::Type::SBool , "empty arrows");
  argv.addCmdArg("-line_width"  , CQChartsCmdArg::Type::Length, "line width");
  argv.addCmdArg("-stroke_color", CQChartsCmdArg::Type::Color , "stroke color");
  argv.addCmdArg("-filled"      , CQChartsCmdArg::Type::SBool , "arrow filled");
  argv.addCmdArg("-fill_color"  , CQChartsCmdArg::Type::Color , "fill color");
  argv.addCmdArg("-labels"      , CQChartsCmdArg::Type::SBool , "debug labels");

  if (! argv.parse())
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;
  }

  //---

  CQChartsArrowData arrowData;

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition start = argv.getParsePosition(view, plot, "start");
  CQChartsPosition end   = argv.getParsePosition(view, plot, "end"  );

  arrowData.length    = argv.getParseLength(view, plot, "length", arrowData.length);
  arrowData.angle     = argv.getParseReal("angle"     , arrowData.angle);
  arrowData.backAngle = argv.getParseReal("back_angle", arrowData.backAngle);
  arrowData.fhead     = argv.getParseBool("fhead"     , arrowData.fhead);
  arrowData.thead     = argv.getParseBool("thead"     , arrowData.thead);
  arrowData.empty     = argv.getParseBool("empty"     , arrowData.empty);

  CQChartsShapeData shapeData;
  CQChartsTextData  textData;

  CQChartsStrokeData &stroke = shapeData.border;
  CQChartsFillData   &fill   = shapeData.background;

  stroke.width = argv.getParseLength(view, plot, "line_width", stroke.width);
  stroke.color = argv.getParseColor("stroke_color", stroke.color);

  fill.visible = argv.getParseBool ("filled"    , fill.visible);
  fill.color   = argv.getParseColor("fill_color", fill.color);

  textData.visible = argv.getParseBool("labels", textData.visible);

  //---

  CQChartsArrowAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addArrowAnnotation(start, end);
  else if (plot)
    annotation = plot->addArrowAnnotation(start, end);
  else
    return;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setArrowData(arrowData);

  annotation->arrow()->setShapeData    (shapeData);
  annotation->arrow()->setDebugTextData(textData);

  setCmdRc(annotation->ind());
}

//------

void
CQChartsCmds::
createPointShapeCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_point_shape", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String, "annotation id" );
  argv.addCmdArg("-tip", CQChartsCmdArg::Type::String, "annotation tip");

  argv.addCmdArg("-position", CQChartsCmdArg::Type::Position, "position");

  argv.addCmdArg("-size", CQChartsCmdArg::Type::Length, "point size");
  argv.addCmdArg("-type", CQChartsCmdArg::Type::String, "point type");

  argv.addCmdArg("-stroked", CQChartsCmdArg::Type::SBool, "stroke visible");
  argv.addCmdArg("-filled" , CQChartsCmdArg::Type::SBool, "fill visible");

  argv.addCmdArg("-line_width", CQChartsCmdArg::Type::String, "stroke width");
  argv.addCmdArg("-line_color", CQChartsCmdArg::Type::Color , "stroke color");
  argv.addCmdArg("-line_alpha", CQChartsCmdArg::Type::Real  , "stroke alpha");

  argv.addCmdArg("-fill_color", CQChartsCmdArg::Type::Color, "fill color");
  argv.addCmdArg("-fill_alpha", CQChartsCmdArg::Type::Real , "fill alpha");

  if (! argv.parse())
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;
  }

  //---

  CQChartsSymbolData pointData;

  QString id    = argv.getParseStr("id");
  QString tipId = argv.getParseStr("tip");

  CQChartsPosition pos = argv.getParsePosition(view, plot, "position");

  pointData.size = argv.getParseLength(view, plot, "size", pointData.size);

  QString typeStr = argv.getParseStr("type");

  if (typeStr.length())
    pointData.type = CQChartsSymbol::nameToType(typeStr);

  pointData.stroke.visible = argv.getParseBool("stroked", pointData.stroke.visible);
  pointData.fill  .visible = argv.getParseBool("filled" , pointData.fill  .visible);

  pointData.stroke.width = argv.getParseLength(view, plot, "line_width", pointData.stroke.width);
  pointData.stroke.color = argv.getParseColor ("line_color", pointData.stroke.color);
  pointData.stroke.alpha = argv.getParseReal  ("line_alpha", pointData.stroke.alpha);

  pointData.fill.color = argv.getParseColor("fill_color", pointData.fill.color);
  pointData.fill.alpha = argv.getParseReal ("fill_alpha", pointData.fill.alpha);

  //---

  CQChartsPointAnnotation *annotation = nullptr;

  if      (view)
    annotation = view->addPointAnnotation(pos, pointData.type);
  else if (plot)
    annotation = plot->addPointAnnotation(pos, pointData.type);
  else
    return;

  annotation->setId(id);
  annotation->setTipId(tipId);

  annotation->setPointData(pointData);

  setCmdRc(annotation->ind());
}

//------

void
CQChartsCmds::
removeShapeCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("remove_shape", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-id" , CQChartsCmdArg::Type::String , "shape id");
  argv.addCmdArg("-all", CQChartsCmdArg::Type::Boolean, "all shapes");
  argv.endCmdGroup();

  if (! argv.parse())
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if      (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else if (argv.hasParseArg("plot")) {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;
  }

  //---

  if (argv.hasParseArg("id")) {
    QString id = argv.getParseStr("id");

    if (view) {
      CQChartsAnnotation *annotation = getAnnotationByName(view, id);

      if (annotation)
        view->removeAnnotation(annotation);
    }
    else {
      CQChartsAnnotation *annotation = getAnnotationByName(view, id);

      if (annotation)
        plot->removeAnnotation(annotation);
    }
  }
  else {
    if (view)
      view->removeAllAnnotations();
    else
      plot->removeAllAnnotations();
  }
}

//------

void
CQChartsCmds::
connectChartCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("connect_chart", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneReq);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-from", CQChartsCmdArg::Type::String, "from connection name");
  argv.addCmdArg("-to"  , CQChartsCmdArg::Type::String, "to procedure name");

  if (! argv.parse())
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;

    view = plot->view();
  }

  //---

  QString fromName = argv.getParseStr("from");
  QString toName   = argv.getParseStr("to"  );

  CQChartsCmdsSlot *cmdsSlot = new CQChartsCmdsSlot(this, view, plot, toName);

  if      (fromName == "objIdPressed") {
    if (plot)
      connect(plot, SIGNAL(objIdPressed(const QString &)),
              cmdsSlot, SLOT(objIdPressed(const QString &)));
    else
      connect(view, SIGNAL(objIdPressed(const QString &)),
              cmdsSlot, SLOT(objIdPressed(const QString &)));
  }
  else if (fromName == "annotationIdPressed") {
    if (plot)
      connect(plot, SIGNAL(annotationIdPressed(const QString &)),
              cmdsSlot, SLOT(annotationIdPressed(const QString &)));
    else
      connect(view, SIGNAL(annotationIdPressed(const QString &)),
              cmdsSlot, SLOT(annotationIdPressed(const QString &)));
  }
  else if (fromName == "plotObjsAdded") {
    if (plot)
      connect(plot, SIGNAL(plotObjsAdded()),
              cmdsSlot, SLOT(plotObjsAdded()));
  }
  else {
    charts_->errorMsg("unknown slot");
    delete cmdsSlot;
    return;
  }

  return;
}

//------

void
CQChartsCmds::
printChartCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("print_chart", vars);

  argv.startCmdGroup(CQChartsCmdGroup::Type::OneOpt);
  argv.addCmdArg("-view", CQChartsCmdArg::Type::String, "view name");
  argv.addCmdArg("-plot", CQChartsCmdArg::Type::String, "plot name");
  argv.endCmdGroup();

  argv.addCmdArg("-file", CQChartsCmdArg::Type::String, "filename").setRequired();

  argv.addCmdArg("-layer", CQChartsCmdArg::Type::String, "layer name");

  if (! argv.parse())
    return;

  //---

  CQChartsView *view = nullptr;
  CQChartsPlot *plot = nullptr;

  if (argv.hasParseArg("view")) {
    QString viewName = argv.getParseStr("view");

    view = getViewByName(viewName);
    if (! view) return;
  }
  else {
    QString plotName = argv.getParseStr("plot");

    plot = getPlotByName(nullptr, plotName);
    if (! plot) return;

    view = plot->view();
  }

  //---

  QString fileName = argv.getParseStr("file");

  if (plot) {
    QString layerName = argv.getParseStr("layer");

    if (layerName.length()) {
      CQChartsLayer::Type type = CQChartsLayer::nameType(layerName);

      if (! plot->printLayer(type, fileName))
        charts_->errorMsg("Failed to print layer");
    }
    else
      view->printFile(fileName, plot);
  }
  else
    view->printFile(fileName);

  return;
}

//------

void
CQChartsCmds::
loadModelDlgCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("load_model_dlg", vars);

  argv.addCmdArg("-modal", CQChartsCmdArg::Type::Boolean, "show modal");

  if (! argv.parse())
    return;

  bool modal = argv.getParseBool("modal");

  //---

  CQChartsLoadDlg *dlg = new CQChartsLoadDlg(charts_);

  if (modal)
    dlg->exec();
  else
    dlg->show();

  setCmdRc(dlg->modelInd());
}

//------

void
CQChartsCmds::
manageModelDlgCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("manage_model_dlg", vars);

  argv.addCmdArg("-modal", CQChartsCmdArg::Type::Boolean, "show modal");

  if (! argv.parse())
    return;

  bool modal = argv.getParseBool("modal");

  //---

  CQChartsModelDlg *dlg = new CQChartsModelDlg(charts_);

  if (modal)
    dlg->exec();
  else
    dlg->show();
}

//------

void
CQChartsCmds::
createPlotDlgCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("create_plot_dlg", vars);

  argv.addCmdArg("-model", CQChartsCmdArg::Type::Integer, "model_ind" );
  argv.addCmdArg("-view" , CQChartsCmdArg::Type::String , "view name" );
  argv.addCmdArg("-modal", CQChartsCmdArg::Type::Boolean, "show modal");

  if (! argv.parse())
    return;

  bool modal = argv.getParseBool("modal");

  //---

  int     modelInd = argv.getParseInt("model", -1);
  QString viewName = argv.getParseStr("view");

  //---

  // get model
  CQChartsModelData *modelData = getModelDataOrCurrent(modelInd);

  if (! modelData) {
    charts_->errorMsg("No model data");
    return;
  }

  //---

  CQChartsPlotDlg *dlg = new CQChartsPlotDlg(charts_, modelData);

  dlg->setViewName(viewName);

  if (modal)
    dlg->exec();
  else
    dlg->show();

  CQChartsPlot *plot = dlg->plot();

  setCmdRc(plot ? plot->pathId() : "");
}

//------

void
CQChartsCmds::
syncQtCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("sync_qt", vars);

  qApp->processEvents();
}

//------

void
CQChartsCmds::
shellCmd(const Vars &vars)
{
  CQChartsCmdArgs argv("sh", vars);

  if (! argv.parse())
    return;

  //---

  const Vars &shArgs = argv.getParseArgs();

  QString cmd = (! shArgs.empty() ? shArgs[0].toString() : "");

  //---

  if (cmd == "") {
    charts_->errorMsg("No command");
    return;
  }

  int rc = system(cmd.toLatin1().constData());

  setCmdRc(rc);
}

//------

void
CQChartsCmds::
setPaletteData(CQChartsGradientPalette *palette, const CQChartsPaletteColorData &paletteData)
{
  if (paletteData.colorTypeStr != "") {
    CQChartsGradientPalette::ColorType colorType = CQChartsGradientPalette::ColorType::MODEL;

    if      (paletteData.colorTypeStr == "model"    )
      colorType = CQChartsGradientPalette::ColorType::MODEL;
    else if (paletteData.colorTypeStr == "defined"  )
      colorType = CQChartsGradientPalette::ColorType::DEFINED;
    else if (paletteData.colorTypeStr == "functions")
      colorType = CQChartsGradientPalette::ColorType::FUNCTIONS;
    else if (paletteData.colorTypeStr == "cubehelix")
      colorType = CQChartsGradientPalette::ColorType::CUBEHELIX;

    palette->setColorType(colorType);
  }

  //---

  if (paletteData.colorModelStr != "") {
    CQChartsGradientPalette::ColorModel colorModel = CQChartsGradientPalette::ColorModel::RGB;

    if      (paletteData.colorModelStr == "rgb")
      colorModel = CQChartsGradientPalette::ColorModel::RGB;
    else if (paletteData.colorModelStr == "hsv")
      colorModel = CQChartsGradientPalette::ColorModel::HSV;
    else if (paletteData.colorModelStr == "cmy")
      colorModel = CQChartsGradientPalette::ColorModel::CMY;
    else if (paletteData.colorModelStr == "yiq")
      colorModel = CQChartsGradientPalette::ColorModel::YIQ;
    else if (paletteData.colorModelStr == "xyz")
      colorModel = CQChartsGradientPalette::ColorModel::XYZ;

    palette->setColorModel(colorModel);
  }

  //---

  if (paletteData.redModel  ) palette->setRedModel  (*paletteData.redModel  );
  if (paletteData.greenModel) palette->setGreenModel(*paletteData.greenModel);
  if (paletteData.blueModel ) palette->setBlueModel (*paletteData.blueModel );

  if (paletteData.negateRed  ) palette->setRedNegative  (*paletteData.negateRed  );
  if (paletteData.negateGreen) palette->setGreenNegative(*paletteData.negateGreen);
  if (paletteData.negateBlue ) palette->setBlueNegative (*paletteData.negateBlue );

  if (paletteData.redMin  ) palette->setRedMin  (*paletteData.redMin  );
  if (paletteData.redMax  ) palette->setRedMax  (*paletteData.redMax  );
  if (paletteData.greenMin) palette->setGreenMin(*paletteData.greenMin);
  if (paletteData.greenMax) palette->setGreenMax(*paletteData.greenMax);
  if (paletteData.blueMin ) palette->setBlueMin (*paletteData.blueMin );
  if (paletteData.blueMax ) palette->setBlueMax (*paletteData.blueMax );

  //---

  if (! paletteData.definedColors.empty()) {
    palette->resetDefinedColors();

    for (const auto &definedColor : paletteData.definedColors)
      palette->addDefinedColor(definedColor.v, definedColor.c);
  }
}

//------

QStringList
CQChartsCmds::
stringToCmds(const QString &str) const
{
  QStringList lines = str.split('\n', QString::SkipEmptyParts);

  QStringList lines1;

  int i = 0;

  for ( ; i < lines.size(); ++i) {
    QString line = lines[i];

    bool join;

    while (! isCompleteLine(line, join)) {
      ++i;

      if (i >= lines.size())
        break;

      const QString &line1 = lines[i];

      if (! join)
        line += "\n" + line1;
      else
        line += line1;
    }

    lines1.push_back(line);
  }

  return lines1;
}

//------

void
CQChartsCmds::
setCmdRc(int rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  std::cerr << rc << "\n";
#endif
}

void
CQChartsCmds::
setCmdRc(double rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  std::cerr << rc << "\n";
#endif
}

void
CQChartsCmds::
setCmdRc(const QString &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  std::cerr << rc.toStdString() << "\n";
#endif
}

void
CQChartsCmds::
setCmdRc(const QVariant &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  std::cerr << rc.toString().toStdString() << "\n";
#endif
}

void
CQChartsCmds::
setCmdRc(const QStringList &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  for (int i = 0; i < rc.length(); ++i)
    std::cerr << rc[i].toStdString() << "\n";
#endif
}

void
CQChartsCmds::
setCmdRc(const QVariantList &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  for (int i = 0; i < rc.length(); ++i)
    std::cerr << rc[i].toString().toStdString() << "\n";
#endif
}

void
CQChartsCmds::
setCmdError(const QString &msg)
{
  charts_->errorMsg(msg);

  setCmdRc(QString());
}

//------

CQChartsPlot *
CQChartsCmds::
createPlot(CQChartsView *view, const ModelP &model, QItemSelectionModel *sm,
           CQChartsPlotType *type, const CQChartsNameValueData &nameValueData, bool reuse,
           const CQChartsGeom::BBox &bbox)
{
  if (! view)
    view = getView(reuse);

  //---

  CQChartsPlot *plot = type->create(view, model);

  if (sm)
    plot->setSelectionModel(sm);

  //---

  // check column parameters exist
  for (const auto &nameValue : nameValueData.values) {
    bool found = false;

    for (const auto &parameter : type->parameters()) {
      if (parameter->name() == nameValue.first) {
        found = true;
        break;
      }
    }

    if (! found) {
      charts_->errorMsg("Illegal column name '" + nameValue.first + "'");
      return nullptr;
    }
  }

  //---

  using NameSet = std::set<QString>;

  NameSet setNames;

  // set plot property for widgets for plot parameters
  for (const auto &parameter : type->parameters()) {
    if      (parameter->type() == "column") {
      auto p = nameValueData.values.find(parameter->name());

      if (p == nameValueData.values.end())
        continue;

      setNames.insert(parameter->name());

      CQChartsColumn column;

      if (! CQChartsUtil::stringToColumn(model.data(), (*p).second, column)) {
        charts_->errorMsg("Bad column name '" + (*p).second + "'");
        column = -1;
      }

      QString scol = column.toString();

      if (! CQUtil::setProperty(plot, parameter->propName(), scol))
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
    }
    else if (parameter->type() == "columns") {
      auto p = nameValueData.values.find(parameter->name());

      if (p == nameValueData.values.end())
        continue;

      setNames.insert(parameter->name());

      const QString str = (*p).second;

      std::vector<CQChartsColumn> columns;

      if (! CQChartsUtil::stringToColumns(model.data(), str, columns))
        charts_->errorMsg("Bad columns name '" + str + "'");

      QString s = CQChartsColumn::columnsToString(columns);

      if (! CQUtil::setProperty(plot, parameter->propName(), QVariant(s)))
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
    }
    else if (parameter->type() == "string") {
      auto p = nameValueData.strings.find(parameter->name());

      if (p == nameValueData.strings.end())
        continue;

      setNames.insert(parameter->name());

      QString str = (*p).second;

      if (! CQUtil::setProperty(plot, parameter->propName(), QVariant(str)))
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
    }
    else if (parameter->type() == "real") {
      auto p = nameValueData.reals.find(parameter->name());

      if (p == nameValueData.reals.end())
        continue;

      setNames.insert(parameter->name());

      double r = (*p).second;

      if (! CQUtil::setProperty(plot, parameter->propName(), QVariant(r)))
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
    }
    else if (parameter->type() == "int") {
      auto p = nameValueData.ints.find(parameter->name());

      if (p == nameValueData.ints.end())
        continue;

      setNames.insert(parameter->name());

      int i = (*p).second;

      if (! CQUtil::setProperty(plot, parameter->propName(), QVariant(i)))
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
    }
    else if (parameter->type() == "enum") {
      auto p = nameValueData.enums.find(parameter->name());

      if (p == nameValueData.enums.end())
        continue;

      setNames.insert(parameter->name());

      QString str = (*p).second;

      if (! CQUtil::setProperty(plot, parameter->propName(), QVariant(str)))
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
    }
    else if (parameter->type() == "bool") {
      auto p = nameValueData.bools.find(parameter->name());

      if (p == nameValueData.bools.end())
        continue;

      setNames.insert(parameter->name());

      bool b = (*p).second;

      if (! CQUtil::setProperty(plot, parameter->propName(), QVariant(b)))
        charts_->errorMsg("Failed to set parameter " + parameter->propName());
    }
    else
      assert(false);
  }

  for (const auto &nv : nameValueData.values) {
    if (setNames.find(nv.first) == setNames.end())
      charts_->errorMsg("Value not found '" + nv.first + "'");
  }

  for (const auto &nv : nameValueData.bools) {
    if (setNames.find(nv.first) == setNames.end())
      charts_->errorMsg("Value not found '" + nv.first + "'");
  }

  for (const auto &nv : nameValueData.strings) {
    if (setNames.find(nv.first) == setNames.end())
      charts_->errorMsg("Value not found '" + nv.first + "'");
  }

  for (const auto &nv : nameValueData.ints) {
    if (setNames.find(nv.first) == setNames.end())
      charts_->errorMsg("Value not found '" + nv.first + "'");
  }

  for (const auto &nv : nameValueData.reals) {
    if (setNames.find(nv.first) == setNames.end())
      charts_->errorMsg("Value not found '" + nv.first + "'");
  }

  for (const auto &nv : nameValueData.enums) {
    if (setNames.find(nv.first) == setNames.end())
      charts_->errorMsg("Value not found '" + nv.first + "'");
  }

  //---

  // add plot to view and show
  view->addPlot(plot, bbox);

  //---

  return plot;
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
getAnnotationByName(CQChartsView *view, const QString &name) const
{
  if (view) {
    CQChartsAnnotation *annotation = view->getAnnotationByName(name);

    if (annotation)
      return annotation;

    const CQChartsView::Annotations &annotations = view->annotations();

    for (const auto &annotation : annotations) {
      if (annotation->id() == name)
        return annotation;

      if (annotation->pathId() == name)
        return annotation;
    }

    charts_->errorMsg("No annotation '" + name + "'");

    return nullptr;
  }

  //---

  CQCharts::Views views;

  charts_->getViews(views);

  for (auto &view : views) {
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

CQChartsAnnotation *
CQChartsCmds::
getAnnotationByName(CQChartsPlot *plot, const QString &name) const
{
  if (plot) {
    CQChartsAnnotation *annotation = plot->getAnnotationByName(name);

    if (annotation)
      return annotation;

    const CQChartsPlot::Annotations &annotations = plot->annotations();

    for (const auto &annotation : annotations) {
      if (annotation->id() == name)
        return annotation;

      if (annotation->pathId() == name)
        return annotation;
    }

    charts_->errorMsg("No annotation '" + name + "'");

    return nullptr;
  }

  //---

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

#ifdef CQCharts_USE_TCL
  loader.setQtcl(qtcl());
#endif

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

  if (! CQChartsUtil::stringToColumn(model.data(), columnStr, column))
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

bool
CQChartsCmds::
isCompleteLine(QString &str, bool &join)
{
  join = false;

  if (! str.length())
    return true;

  if (str[str.size() - 1] == '\\') {
    str = str.mid(0, str.length() - 1);
    join = true;
    return false;
  }

  //---

  CQStrParse line(str);

  line.skipSpace();

  while (! line.eof()) {
    if      (line.isChar('{')) {
      if (! line.skipBracedString())
        return false;
    }
    else if (line.isChar('\"') || line.isChar('\'')) {
      if (! line.skipString())
        return false;
    }
    else {
      line.skipNonSpace();
    }

    line.skipSpace();
  }

  return true;
}

void
CQChartsCmds::
parseLine(const QString &line, bool log)
{
#ifdef CQCharts_USE_TCL
  int rc = qtcl()->eval(line, /*showError*/true, /*showResult*/log);

  if (rc != TCL_OK)
    charts_->errorMsg("Invalid line: '" + line + "'");
#else
  if (log)
    charts_->errorMsg("No eval");

  charts_->errorMsg("Invalid line: '" + line + "'");
#endif
}

QStringList
CQChartsCmds::
stringToColumns(const QString &str) const
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

//------

CQChartsCmdsSlot::
CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                 const QString &procName) :
 cmds_(cmds), view_(view), plot_(plot), procName_(procName)
{
}

void
CQChartsCmdsSlot::
objIdPressed(const QString &id)
{
#ifdef CQCharts_USE_TCL
  QString cmd = getTclIdCmd(id);

  cmds_->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
#else
  std::cerr << "objIdPressed: " << id.toStdString() << "\n";
#endif
}

void
CQChartsCmdsSlot::
annotationIdPressed(const QString &id)
{
#ifdef CQCharts_USE_TCL
  QString cmd = getTclIdCmd(id);

  cmds_->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
#else
  std::cerr << "annotationIdPressed: " << id.toStdString() << "\n";
#endif
}

void
CQChartsCmdsSlot::
plotObjsAdded()
{
  disconnect(plot_, SIGNAL(plotObjsAdded()),
             this, SLOT(plotObjsAdded()));

#ifdef CQCharts_USE_TCL
  QString cmd = getTclCmd();

  cmds_->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
#else
  std::cerr << "plotObjsAdded\n";
#endif

  connect(plot_, SIGNAL(plotObjsAdded()),
          this, SLOT(plotObjsAdded()));
}

QString
CQChartsCmdsSlot::
getTclCmd() const
{
  QString viewName = view_->id();

  QString cmd = procName_;

  cmd += " \"" + viewName + "\"";

  if (plot_)
    cmd += " \"" + plot_->pathId() + "\"";

  return cmd;
}

QString
CQChartsCmdsSlot::
getTclIdCmd(const QString &id) const
{
  QString viewName = view_->id();

  QString cmd = procName_;

  cmd += " \"" + viewName + "\"";

  if (plot_)
    cmd += " \"" + plot_->pathId() + "\"";

  cmd += " \"" + id + "\"";

  return cmd;
}
