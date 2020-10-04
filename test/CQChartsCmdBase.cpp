#include <CQChartsCmdBase.h>
#include <CQChartsInput.h>
#include <CQBaseModel.h>
#include <CQWidgetFactory.h>
#include <CQPerfMonitor.h>
#include <CQUtil.h>
#include <CQStrUtil.h>
#include <CQTclUtil.h>

#include <QApplication>
#include <QVBoxLayout>

namespace {

void
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}

}

class CQChartsTclCmd {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsTclCmd(CQChartsCmdBase *cmdBase, const QString &name) :
   cmdBase_(cmdBase), name_(name) {
    cmdId_ = cmdBase_->qtcl()->createObjCommand(name_,
               (CQTcl::ObjCmdProc) &CQChartsTclCmd::commandProc, (CQTcl::ObjCmdData) this);
  }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    auto *command = (CQChartsTclCmd *) clientData;

    Vars vars;

    for (int i = 1; i < objc; ++i) {
      auto *obj = const_cast<Tcl_Obj *>(objv[i]);

      vars.push_back(command->cmdBase_->qtcl()->variantFromObj(obj));
    }

    if (! command->cmdBase_->processCmd(command->name_, vars))
      return TCL_ERROR;

    return TCL_OK;
  }

 private:
  CQChartsCmdBase *cmdBase_  { nullptr };
  QString          name_;
  Tcl_Command      cmdId_ { nullptr };
};

//----

CQChartsCmdBase::
CQChartsCmdBase(CQCharts *charts) :
 charts_(charts)
{
  qtcl_ = charts_->cmdTcl();
}

CQChartsCmdBase::
~CQChartsCmdBase()
{
}

void
CQChartsCmdBase::
addCommands()
{
  static bool cmdsAdded;

  if (! cmdsAdded) {
    addCommand("help"    , new CQChartsBaseHelpCmd    (this));
    addCommand("complete", new CQChartsBaseCompleteCmd(this));

    // qt generic
    addCommand("qt_create_widget"   , new CQChartsBaseCreateWidgetCmd  (this));
    addCommand("qt_create_layout"   , new CQChartsBaseCreateLayoutCmd  (this));
    addCommand("qt_add_child_widget", new CQChartsBaseAddChildWidgetCmd(this));
    addCommand("qt_add_stretch"     , new CQChartsBaseAddStretchCmd    (this));
    addCommand("qt_connect_widget"  , new CQChartsBaseConnectWidgetCmd (this));
    addCommand("qt_activate_slot"   , new CQChartsBaseActivateSlotCmd  (this));

    addCommand("qt_get_property", new CQChartsBaseGetPropertyCmd(this));
    addCommand("qt_set_property", new CQChartsBaseSetPropertyCmd(this));
    addCommand("qt_has_property", new CQChartsBaseHasPropertyCmd(this));

    addCommand("qt_sync", new CQChartsBaseQtSyncCmd(this));

    addCommand("perf", new CQChartsBasePerfCmd(this));

    qtcl()->createAlias("echo", "puts");

    addCommand("assert", new CQChartsBaseAssertCmd(this));

    addCommand("sh", new CQChartsBaseShellCmd(this));

    //---

    cmdsAdded = true;
  }
}

void
CQChartsCmdBase::
addCommand(const QString &name, CQChartsCmdProc *proc)
{
  proc->setName(name);

  auto tclCmd = new CQChartsTclCmd(this, name);

  proc->setTclCmd(tclCmd);

  commandNames_.push_back(name);

  commandProcs_[name] = proc;
}

bool
CQChartsCmdBase::
processCmd(const QString &cmd, const Vars &vars)
{
  auto p = commandProcs_.find(cmd);

  if (p != commandProcs_.end()) {
    auto *proc = (*p).second;

    CQChartsCmdArgs argv(cmd, vars);

    return proc->exec(argv);
  }

  //---

  if (cmd == "exit") { exit(0); }

  //---

  return false;
}

//------

bool
CQChartsCmdBase::
qtCreateWidgetCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtCreateWidgetCmd");

  argv.addCmdArg("-parent", CQChartsCmdArg::Type::String, "parent name");
  argv.addCmdArg("-type"  , CQChartsCmdArg::Type::String, "widget type");
  argv.addCmdArg("-name"  , CQChartsCmdArg::Type::String, "widget name");

  if (! argv.parse())
    return false;

  QString typeName = argv.getParseStr("type");

  if (typeName == "?") {
    auto names = CQWidgetFactoryMgrInst->widgetFactoryNames();

    return setCmdRc(names);
  }

  QWidget *parentWidget = nullptr;

  if (argv.hasParseArg("parent")) {
    QString parentName = argv.getParseStr("parent");

    auto *parent = CQUtil::nameToObject(parentName);

    parentWidget = qobject_cast<QWidget *>(parent);

    if (! parentWidget) {
      errorMsg(QString("No parent '%1'").arg(parentName));
      return false;
    }
  }

  auto name = argv.getParseStr("name");

  if (! CQWidgetFactoryMgrInst->isWidgetFactory(typeName)) {
    errorMsg(QString("Invalid type '%1'").arg(typeName));
    return false;
  }

  auto *w = CQWidgetFactoryMgrInst->createWidget(typeName, parentWidget);

  if (! w) {
    errorMsg(QString("Failed to create '%1'").arg(typeName));
    return false;
  }

  if (name != "")
    w->setObjectName(name);

  setCmdRc(CQUtil::fullName(w));

  return true;
}

//------

bool
CQChartsCmdBase::
qtAddChildWidgetCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtAddChildWidgetCmd");

  argv.addCmdArg("-parent", CQChartsCmdArg::Type::String, "parent name");
  argv.addCmdArg("-child" , CQChartsCmdArg::Type::String, "child name");

  if (! argv.parse())
    return false;

  auto  parentName   = argv.getParseStr("parent");
  auto *parentWidget = qobject_cast<QWidget *>(CQUtil::nameToObject(parentName));

  if (! parentWidget) {
    errorMsg(QString("No parent '%1'").arg(parentName));
    return false;
  }

  auto  childName   = argv.getParseStr("child");
  auto *childWidget = qobject_cast<QWidget *>(CQUtil::nameToObject(childName));

  if (! childWidget) {
    errorMsg(QString("No widget '%1'").arg(childName));
    return false;
  }

  auto *layout = parentWidget->layout();

  if (! layout)
    layout = new QVBoxLayout(parentWidget);

  layout->addWidget(childWidget);

  setCmdRc(CQUtil::fullName(childWidget));

  return true;
}

//------

bool
CQChartsCmdBase::
qtAddStretchCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtAddStretchCmd");

  argv.addCmdArg("-parent", CQChartsCmdArg::Type::String, "parent name");

  if (! argv.parse())
    return false;

  auto  parentName   = argv.getParseStr("parent");
  auto *parentWidget = qobject_cast<QWidget *>(CQUtil::nameToObject(parentName));

  if (! parentWidget) {
    errorMsg(QString("No parent '%1'").arg(parentName));
    return false;
  }

  auto *layout = parentWidget->layout();

  if (! layout)
    layout = new QVBoxLayout(parentWidget);

  auto *boxLayout = qobject_cast<QBoxLayout *>(layout);

  if (boxLayout)
    boxLayout->addStretch(1);

  return true;
}

//------

bool
CQChartsCmdBase::
qtCreateLayoutCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtCreateLayoutCmd");

  argv.addCmdArg("-parent", CQChartsCmdArg::Type::String, "parent name");
  argv.addCmdArg("-type"  , CQChartsCmdArg::Type::String, "layout type");

  if (! argv.parse())
    return false;

  auto typeName = argv.getParseStr("type");

  if (typeName == "?") {
    auto names = CQWidgetFactoryMgrInst->layoutFactoryNames();

    return setCmdRc(names);
  }

  QWidget *parentWidget = nullptr;

  if (argv.hasParseArg("parent")) {
    QString parentName = argv.getParseStr("parent");

    parentWidget = qobject_cast<QWidget *>(CQUtil::nameToObject(parentName));

    if (! parentWidget) {
      errorMsg(QString("No parent '%1'").arg(parentName));
      return false;
    }
  }
  else {
    errorMsg("No parent");
    return false;
  }

  if (! CQWidgetFactoryMgrInst->isLayoutFactory(typeName)) {
    errorMsg(QString("Invalid type '%1'").arg(typeName));
    return false;
  }

  auto *l = CQWidgetFactoryMgrInst->createLayout(typeName, parentWidget);

  if (! l) {
    errorMsg(QString("Failed to create '%1'").arg(typeName));
    return false;
  }

  return true;
}

//------

bool
CQChartsCmdBase::
qtConnectWidgetCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtConnectWidgetCmd");

  argv.addCmdArg("-name"  , CQChartsCmdArg::Type::String, "widget name");
  argv.addCmdArg("-signal", CQChartsCmdArg::Type::String, "signal name");
  argv.addCmdArg("-proc"  , CQChartsCmdArg::Type::String, "tcl proc name");

  if (! argv.parse())
    return false;

  auto  name   = argv.getParseStr("name");
  auto *widget = qobject_cast<QWidget *>(CQUtil::nameToObject(name));

  if (! widget) {
    errorMsg(QString("No widget '%1'").arg(name));
    return false;
  }

  auto signalName = argv.getParseStr("signal");
  auto procName   = argv.getParseStr("proc");

  auto *slot = new CQChartsCmdBaseSlot(this, procName);

  auto signalName2 = (QString("2") + signalName).toStdString();
  auto slotName1   = (QString("1") + signalName).toStdString();

  QObject::connect(widget, signalName2.c_str(), slot, slotName1.c_str());

  return true;
}

//------

bool
CQChartsCmdBase::
qtActivateSlotCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtActivateSlotCmd");

  argv.addCmdArg("-name", CQChartsCmdArg::Type::String, "widget name");
  argv.addCmdArg("-slot", CQChartsCmdArg::Type::String, "slot name");
  argv.addCmdArg("-args", CQChartsCmdArg::Type::String, "slot args");

  if (! argv.parse())
    return false;

  auto  name   = argv.getParseStr("name");
  auto *object = CQUtil::nameToObject(name);

  if (! object) {
    errorMsg(QString("No object '%1'").arg(name));
    return false;
  }

  auto slotName = argv.getParseStr("slot").toStdString();
  auto argsStr  = argv.getParseStr("args").toStdString();

  if (! CQUtil::activateSlot(object, slotName.c_str(), argsStr.c_str())) {
    errorMsg(QString("Invalid slot '%1'").arg(slotName.c_str()));
    return false;
  }

  return true;
}

//------

bool
CQChartsCmdBase::
qtGetPropertyCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtGetPropertyCmd");

  argv.addCmdArg("-object"  , CQChartsCmdArg::Type::String, "object name");
  argv.addCmdArg("-property", CQChartsCmdArg::Type::String, "property name");

  if (! argv.parse())
    return false;

  QString objectName = argv.getParseStr("object");
  QString propName   = argv.getParseStr("property");

  auto *obj = CQUtil::nameToObject(objectName);

  if (! obj) {
    errorMsg(QString("No object '%1'").arg(objectName));
    return false;
  }

  QVariant v;

  if (! CQUtil::getProperty(obj, propName, v)) {
    errorMsg(QString("Failed to get property '%1' for '%2'").arg(propName).arg(objectName));
    return false;
  }

  setCmdRc(v);

  return true;
}

//------

bool
CQChartsCmdBase::
qtSetPropertyCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtSetPropertyCmd");

  argv.addCmdArg("-object"  , CQChartsCmdArg::Type::String, "object name");
  argv.addCmdArg("-property", CQChartsCmdArg::Type::String, "property name");
  argv.addCmdArg("-value"   , CQChartsCmdArg::Type::String, "property value");

  if (! argv.parse())
    return false;

  QString objectName = argv.getParseStr("object");
  QString propName   = argv.getParseStr("property");
  QString value      = argv.getParseStr("value");

  auto *obj = CQUtil::nameToObject(objectName);

  if (! obj) {
    errorMsg(QString("No object '%1'").arg(objectName));
    return false;
  }

  if (! CQUtil::setProperty(obj, propName, value)) {
    errorMsg(QString("Failed to set property '%1' for '%2'").arg(propName).arg(objectName));
    return false;
  }

  return true;
}

//------

bool
CQChartsCmdBase::
qtHasPropertyCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtHasPropertyCmd");

  argv.addCmdArg("-object"  , CQChartsCmdArg::Type::String , "object name");
  argv.addCmdArg("-property", CQChartsCmdArg::Type::String , "property name");
  argv.addCmdArg("-writable", CQChartsCmdArg::Type::Boolean, "property is writable");

  if (! argv.parse())
    return false;

  QString objectName = argv.getParseStr("object");
  QString propName   = argv.getParseStr("property");

  auto *obj = CQUtil::nameToObject(objectName);

  if (! obj) {
    errorMsg(QString("No object '%1'").arg(objectName));
    return false;
  }

  bool b;

  if (argv.hasParseArg("writable"))
    b = CQUtil::hasWritableProperty(obj, propName);
  else
    b = CQUtil::hasProperty(obj, propName);

  setCmdRc(b);

  return true;
}

//------

bool
CQChartsCmdBase::
qtSyncCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtSyncCmd");

  argv.addCmdArg("-n", CQChartsCmdArg::Type::Integer, "loop count");

  if (! argv.parse())
    return false;

  int n = 1;

  if (argv.hasParseArg("n"))
    n = argv.getParseInt("n");

  for (int i = 0; i < n; ++i) {
    qApp->flush();

    qApp->processEvents();
  }

  return true;
}

//------

bool
CQChartsCmdBase::
perfCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::perfCmd");

  argv.addCmdArg("-start_recording", CQChartsCmdArg::Type::Boolean, "start recording");
  argv.addCmdArg("-end_recording"  , CQChartsCmdArg::Type::Boolean, "end recording"  );
  argv.addCmdArg("-tracing"        , CQChartsCmdArg::Type::SBool  , "enable tracing" );
  argv.addCmdArg("-debug"          , CQChartsCmdArg::Type::SBool  , "enable debug"   );

  if (! argv.parse())
    return false;

  //---

  if (argv.hasParseArg("start_recording"))
    CQPerfMonitorInst->startRecording();

  if (argv.hasParseArg("end_recording"))
    CQPerfMonitorInst->stopRecording();

  if (argv.hasParseArg("tracing"))
    CQPerfMonitorInst->setEnabled(argv.getParseBool("tracing"));

  if (argv.hasParseArg("debug"))
    CQPerfMonitorInst->setDebug(argv.getParseBool("debug"));

  return true;
}

//------

bool
CQChartsCmdBase::
assertCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::assertCmd");

  if (! argv.parse())
    return false;

  //---

  const auto &pargs = argv.getParseArgs();

  QString expr = (! pargs.empty() ? pargs[0].toString() : "");

  QVariant res;

  bool rc = qtcl()->evalExpr(expr, res, /*showError*/true);
  if (! rc) return false;

  bool b = res.toBool();

  return b;
}

//------

bool
CQChartsCmdBase::
shellCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::shellCmd");

  if (! argv.parse())
    return false;

  //---

  const auto &pargs = argv.getParseArgs();

  QString cmd = (! pargs.empty() ? pargs[0].toString() : "");

  //---

  if (cmd == "") {
    errorMsg("No command");
    return false;
  }

  int rc = system(cmd.toLatin1().constData());

  setCmdRc(rc);

  return true;
}

//------

bool
CQChartsCmdBase::
helpCmd(CQChartsCmdArgs &argv)
{
  argv.addCmdArg("-hidden" , CQChartsCmdArg::Type::Boolean, "show hidden");
  argv.addCmdArg("-verbose", CQChartsCmdArg::Type::Boolean, "verbose help");

  if (! argv.parse())
    return false;

  auto hidden  = argv.getParseBool("hidden");
  auto verbose = argv.getParseBool("verbose");

  //---

  const auto &pargs = argv.getParseArgs();

  QString pattern = (! pargs.empty() ? pargs[0].toString() : "");

  //---

  if (pattern.length()) {
    using Procs = std::vector<CQChartsCmdProc *>;

    Procs procs;

    auto p = commandProcs_.find(pattern);

    if (p != commandProcs_.end()) {
      procs.push_back((*p).second);
    }
    else {
      QRegExp re(pattern, Qt::CaseSensitive, QRegExp::Wildcard);

      for (auto &p : commandProcs_) {
        if (re.exactMatch(p.first))
          procs.push_back(p.second);
      }
    }

    if (procs.empty()) {
      std::cout << "Command not found\n";
      return true;
    }

    for (const auto &proc : procs) {
      if (verbose) {
        Vars vars;

        CQChartsCmdArgs args(proc->name(), vars);

        proc->addArgs(args);

        args.help(hidden);
      }
      else {
        std::cout << proc->name().toStdString() << "\n";
      }
    }
  }
  else {
    // all procs
    for (auto &p : commandProcs_) {
      auto *proc = p.second;

      if (verbose) {
        Vars vars;

        CQChartsCmdArgs args(proc->name(), vars);

        proc->addArgs(args);

        args.help(hidden);
      }
      else {
        std::cout << proc->name().toStdString() << "\n";
      }
    }
  }

  return true;
}

//------

bool
CQChartsCmdBase::
completeCmd(CQChartsCmdArgs &argv)
{
  argv.addCmdArg("-command", CQChartsCmdArg::Type::String, "complete command").setRequired();

  argv.addCmdArg("-option"     , CQChartsCmdArg::Type::String , "complete option");
  argv.addCmdArg("-value"      , CQChartsCmdArg::Type::String , "complete value");
  argv.addCmdArg("-name_values", CQChartsCmdArg::Type::String , "option name values");
  argv.addCmdArg("-all"        , CQChartsCmdArg::Type::Boolean, "get all matches");
  argv.addCmdArg("-exact_space", CQChartsCmdArg::Type::Boolean, "add space if exact");

  if (! argv.parse())
    return false;

  auto command    = argv.getParseStr ("command");
  auto allFlag    = argv.getParseBool("all");
  auto exactSpace = argv.getParseBool("exact_space");

  //---

  using NameValueMap = std::map<QString, QString>;

  NameValueMap nameValueMap;

  auto nameValues = argv.getParseStr ("name_values");

  QStringList nameValues1;

  (void) CQTcl::splitList(nameValues, nameValues1);

  for (const auto &nv : nameValues1) {
    QStringList nameValues2;

    (void) CQTcl::splitList(nv, nameValues2);

    if (nameValues2.length() == 2)
      nameValueMap[nameValues2[0]] = nameValues2[1];
  }

  //---

  if (argv.hasParseArg("option")) {
    // get option to complete
    auto option = argv.getParseStr ("option");

    auto optionStr = "-" + option;

    //---

    // get proc
    auto p = commandProcs_.find(command);
    if (p == commandProcs_.end()) return false;

    auto *proc = (*p).second;

    //---

    // get arg info
    Vars vars;

    CQChartsCmdArgs args(command, vars);

    proc->addArgs(args);

    //---

    if (argv.hasParseArg("value")) {
      // get option arg type
      auto *arg = args.getCmdOpt(option);
      if (! arg) return false;

      auto type = arg->type();

      //---

      // get value to complete
      auto value = argv.getParseStr("value");

      //---

      // complete by type
      QStringList strs;

      if      (type == CQChartsCmdArg::Type::Boolean) {
      }
      else if (type == CQChartsCmdArg::Type::Integer) {
      }
      else if (type == CQChartsCmdArg::Type::Real) {
      }
      else if (type == CQChartsCmdArg::Type::SBool) {
        strs << "0" << "1";
      }
      else if (type == CQChartsCmdArg::Type::Enum) {
        auto &nv = arg->nameValues();

        for (const auto &p : nv)
          strs.push_back(p.first);
      }
      else if (type == CQChartsCmdArg::Type::Color) {
      }
      else if (type == CQChartsCmdArg::Type::Font) {
      }
      else if (type == CQChartsCmdArg::Type::LineDash) {
      }
      else if (type == CQChartsCmdArg::Type::Length) {
      }
      else if (type == CQChartsCmdArg::Type::Position) {
      }
      else if (type == CQChartsCmdArg::Type::Rect) {
      }
      else if (type == CQChartsCmdArg::Type::Polygon) {
      }
      else if (type == CQChartsCmdArg::Type::Align) {
      }
      else if (type == CQChartsCmdArg::Type::Sides) {
      }
      else if (type == CQChartsCmdArg::Type::Column) {
      }
      else if (type == CQChartsCmdArg::Type::Row) {
      }
      else if (type == CQChartsCmdArg::Type::Reals) {
      }
      else if (type == CQChartsCmdArg::Type::String) {
        strs = proc->getArgValues(option, nameValueMap);
      }
      else {
        return setCmdRc(strs);
      }

      //---

      if (allFlag)
        return setCmdRc(strs);

      auto matchValues = CQStrUtil::matchStrs(value, strs);

      bool exact;

      auto newValue = CQStrUtil::longestMatch(matchValues, exact);

      if (newValue.length() >= value.length()) {
        if (exact && exactSpace)
          newValue += " ";

        return setCmdRc(newValue);
      }

      return setCmdRc(strs);
    }
    else {
      const auto &names = args.getCmdArgNames();

      if (allFlag) {
        QStringList strs;

        for (const auto &name : names)
          strs.push_back(name.mid(1));

        return setCmdRc(strs);
      }

      auto matchArgs = CQStrUtil::matchStrs(optionStr, names);

      bool exact;

      auto newOption = CQStrUtil::longestMatch(matchArgs, exact);

      if (newOption.length() >= optionStr.length()) {
        if (exact && exactSpace)
          newOption += " ";

        return setCmdRc(newOption);
      }
    }
  }
  else {
    const auto &cmds = qtcl()->commandNames();

    auto matchCmds = CQStrUtil::matchStrs(command, cmds);

    if (allFlag)
      return setCmdRc(matchCmds);

    bool exact;

    auto newCommand = CQStrUtil::longestMatch(matchCmds, exact);

    if (newCommand.length() >= command.length()) {
      if (exact && exactSpace)
        newCommand += " ";

      return setCmdRc(newCommand);
    }
  }

  return setCmdRc(QString());
}

//------

QStringList
CQChartsCmdBase::
stringToCmdData(const QString &str) const
{
  return CQChartsInput::stringToLines(str,
   [&](QString &line, bool &join) {
    return isCompleteLine(line, join);
   }
  );
}

//------

bool
CQChartsCmdBase::
setCmdRc(int rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(double rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const QString &rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const std::string &rc)
{
  qtcl()->setResult(QString(rc.c_str()));

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const QVariant &rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const QStringList &rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const QVariantList &rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdError(const QString &msg)
{
  errorMsg(msg);

  setCmdRc(QString());

  return false;
}

//------

QStringList
CQChartsCmdBase::
stringToCmds(const QString &str) const
{
  return CQChartsInput::stringToLines(str,
   [&](QString &line, bool &join) {
    return isCompleteLine(line, join);
   }
  );
}

//------

bool
CQChartsCmdBase::
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
CQChartsCmdBase::
parseLine(const QString &line, bool log)
{
  int rc = qtcl()->eval(line, /*showError*/true, /*showResult*/log);

  if (rc != TCL_OK)
    errorMsg("Invalid line: '" + line + "'");
}

//----

CQChartsCmdBaseSlot::
CQChartsCmdBaseSlot(CQChartsCmdBase *base, const QString &procName) :
 base_(base), procName_(procName) {
}

void
CQChartsCmdBaseSlot::
clicked()
{
  execProc();
}

void
CQChartsCmdBaseSlot::
clicked(bool)
{
  execProc();
}

void
CQChartsCmdBaseSlot::
execProc()
{
  base_->qtcl()->eval(procName_, /*showError*/true, /*showResult*/false);
}
